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

using google::protobuf::Message;

using grpc::ServerBuilder;

profanedb::server::Server::Server()
{
    // TODO Config
    boost::log::core::get()->set_filter(
        boost::log::trivial::severity >= boost::log::trivial::debug
    );
    
    // TODO Config
    rocksdb::Options rocksOptions;
    rocksOptions.create_if_missing = true;
    
    rocksdb::DB *rocks;
    rocksdb::DB::Open(rocksOptions, "/tmp/profane", &rocks);
    
    auto storage = std::make_shared<RocksStorage>(std::unique_ptr<rocksdb::DB>(rocks));
    
    // TODO Should be from config
    auto includeSourceTree = new Loader::RootSourceTree{
        "/usr/include", "/home/giorgio/Documents/ProfaneDB/src"};
    
    // TODO Config
    auto schemaSourceTree = new Loader::RootSourceTree{"/home/giorgio/Documents/ProfaneDB/test"};
    
    auto loader = std::make_shared<Loader>(
        std::unique_ptr<Loader::RootSourceTree>(includeSourceTree),
        std::unique_ptr<Loader::RootSourceTree>(schemaSourceTree));
    
    auto marshaller = std::make_shared<ProtobufMarshaller>(storage, loader);
    
    service = std::make_unique<DbServiceImpl>(
        std::make_unique< profanedb::Db<Message> >(storage, marshaller));
}

profanedb::server::Server::~Server()
{
    server->Shutdown();
}

void profanedb::server::Server::Run()
{
    std::string address("0.0.0.0:50051");

    ServerBuilder builder;
    
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(service.get());
    
    server = builder.BuildAndStart();
    
    BOOST_LOG_TRIVIAL(info) << "Server listening on " << address << std::endl;
    
    HandleRpcs();
}

void profanedb::server::Server::HandleRpcs()
{
    server->Wait();
}

profanedb::server::Server::DbServiceImpl::DbServiceImpl(std::unique_ptr< profanedb::Db<Message> > profane)
  : profane(std::move(profane))
{
}

grpc::Status profanedb::server::Server::DbServiceImpl::Get(grpc::ServerContext * context, const profanedb::protobuf::GetReq * request, profanedb::protobuf::GetResp * response)
{
    BOOST_LOG_TRIVIAL(debug) << "GET request from " << context->peer();
    
    response->mutable_message()->PackFrom(this->profane->Get(request->key()));
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Put(grpc::ServerContext * context, const profanedb::protobuf::PutReq * request, profanedb::protobuf::PutResp * response)
{
    BOOST_LOG_TRIVIAL(debug) << "PUT request from " << context->peer();
    
    // TODO Unpack
    // this->profanedb.Put(request->serializable());
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Delete(grpc::ServerContext * context, const profanedb::protobuf::DelReq * request, profanedb::protobuf::DelResp * response)
{
    BOOST_LOG_TRIVIAL(debug) << "DELETE request from " << context->peer();
    
    return grpc::Status::OK;
}
