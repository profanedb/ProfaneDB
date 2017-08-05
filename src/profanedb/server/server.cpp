/*
 * ProfaneDB - A Protocol Buffers database.
 * Copyright (C) 2017  "Giorgio Azzinnaro" <giorgio.azzinnaro@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "server.h"

using profanedb::format::protobuf::Loader;
using profanedb::format::Marshaller;
using ProtobufMarshaller = profanedb::format::protobuf::Marshaller;
using profanedb::vault::Storage;
using RocksStorage = profanedb::vault::rocksdb::Storage;
using profanedb::server::Config;

using profanedb::protobuf::GetReq;
using profanedb::protobuf::GetResp;
using profanedb::protobuf::PutReq;
using profanedb::protobuf::PutResp;
using profanedb::protobuf::DelReq;
using profanedb::protobuf::DelResp;

using google::protobuf::Message;

using grpc::ServerBuilder;
using grpc::ServerContext;
using grpc::Status;

using boost::program_options::variables_map;

namespace profanedb {
namespace server {

Server::Server(const Config & config)
  : config(config)
{
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= config.LogLevel()
    );
    
    rocksdb::DB *rocks;
    rocksdb::DB::Open(config.RocksOptions(), config.RocksPath().string(), &rocks);
    
    auto storage = std::make_shared<RocksStorage>(std::unique_ptr<rocksdb::DB>(rocks));
    
    auto includeSourceTree = new Loader::RootSourceTree(config.IncludePath());
    
    auto schemaSourceTree = new Loader::RootSourceTree(config.SchemaPath());
    
    auto loader = std::make_shared<Loader>(
        std::unique_ptr<Loader::RootSourceTree>(includeSourceTree),
        std::unique_ptr<Loader::RootSourceTree>(schemaSourceTree));
    
    auto marshaller = std::make_shared<ProtobufMarshaller>(storage, loader);
    
    service = std::make_unique<DbServiceImpl>(storage, marshaller, loader);
}

Server::~Server()
{
    server->Shutdown();
}

void Server::Run()
{
    std::string address("0.0.0.0:50051");

    ServerBuilder builder;
    
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());
    
    server = builder.BuildAndStart();
    
    BOOST_LOG_TRIVIAL(info) << "Server listening on " << address << std::endl;
    
    HandleRpcs();
}

void Server::HandleRpcs()
{
    server->Wait();
}

Server::DbServiceImpl::DbServiceImpl(
    std::shared_ptr<RocksStorage> storage,
    std::shared_ptr<ProtobufMarshaller> marshaller,
    std::shared_ptr<Loader> loader)
  : rocksdbStorage(storage)
  , protobufMarshaller(marshaller)
  , profane(std::make_unique< profanedb::Db<Message> >(storage, marshaller))
  , loader(loader)
{
}

Status Server::DbServiceImpl::Get(ServerContext * context, const GetReq * request, GetResp * response)
{
    BOOST_LOG_TRIVIAL(debug) << "GET request from " << context->peer();
    
    response->mutable_message()->PackFrom(this->profane->Get(request->key()));
    
    return Status::OK;
}

Status Server::DbServiceImpl::Put(ServerContext * context, const PutReq * request, PutResp * response)
{
    BOOST_LOG_TRIVIAL(debug) << "PUT request from " << context->peer();
    
    // Because the incoming request brings a google::protobuf::Any message,
    // we must dynamically create the actual message according to its type
    // (which comes with `type.googleapis.com/` prepended)
    std::string type = request->serializable().type_url();
    Message * unpackedMessage =
      this->loader
          ->CreateMessage(Loader::SCHEMA, type.substr(type.rfind('/')+1, std::string::npos))
          ->New();

    request->serializable().UnpackTo(unpackedMessage);
    BOOST_LOG_TRIVIAL(trace) << "Unpacked message" << std::endl << unpackedMessage->DebugString();

    this->profane->Put(*unpackedMessage);
    
    return Status::OK;
}

Status Server::DbServiceImpl::Delete(ServerContext * context, const DelReq * request, DelResp * response)
{
    BOOST_LOG_TRIVIAL(debug) << "DELETE request from " << context->peer();
    
    return Status::OK;
}

}
}
