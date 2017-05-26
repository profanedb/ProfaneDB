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
using ProtobufMarshaller = profanedb::format::protobuf::Marshaller;
using RocksStorage = profanedb::vault::rocksdb::Storage;

using google::protobuf::Message;

using grpc::ServerBuilder;

using rocksdb::DB;

profanedb::server::Server::Server()
{
    boost::di::bind<profanedb::format::Marshaller<Message>>().to<ProtobufMarshaller>();
    boost::di::bind<profanedb::vault::Storage>().to<RocksStorage>();
    
    auto injector = boost::di::make_injector();
//     service = std::make_unique<DbServiceImpl>(new DbServiceImpl(
//         injector.create< profanedb::Db<google::protobuf::Message> >());
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
    
    std::cout << "Server listening on " << address << std::endl;
    
    HandleRpcs();
}

void profanedb::server::Server::HandleRpcs()
{
    server->Wait();
}

profanedb::server::Server::DbServiceImpl::DbServiceImpl(profanedb::Db<Message> & profanedb)
  : profanedb(profanedb)
{
}

grpc::Status profanedb::server::Server::DbServiceImpl::Get(grpc::ServerContext * context, const profanedb::protobuf::GetReq * request, profanedb::protobuf::GetResp * response)
{
    response->mutable_message()->PackFrom(this->profanedb.Get(request->key()));
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Put(grpc::ServerContext * context, const profanedb::protobuf::PutReq * request, profanedb::protobuf::PutResp * response)
{
    // TODO Unpack
    // this->profanedb.Put(request->serializable());
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Delete(grpc::ServerContext * context, const profanedb::protobuf::DelReq * request, profanedb::protobuf::DelResp * response)
{
    
    return grpc::Status::OK;
}
