/*
 * ProfaneDB - A Protocol Buffer database.
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

profanedb::server::Server::Server()
{
}

profanedb::server::Server::~Server()
{
    server->Shutdown();
}

void profanedb::server::Server::Run()
{
    std::string address("0.0.0.0:50051");

    grpc::ServerBuilder builder;
    
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    server = builder.BuildAndStart();
    
    std::cout << "Server listening on " << address << std::endl;
    
    HandleRpcs();
}

void profanedb::server::Server::HandleRpcs()
{
    server->Wait();
}

grpc::Status profanedb::server::Server::DbServiceImpl::Get(grpc::ServerContext * context, const profanedb::protobuf::GetReq * request, profanedb::protobuf::GetResp * response)
{
    db.Get(*request);
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Put(grpc::ServerContext * context, const profanedb::protobuf::PutReq * request, profanedb::protobuf::PutResp * response)
{
    db.Put(*request);
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Delete(grpc::ServerContext * context, const profanedb::protobuf::DelReq * request, profanedb::protobuf::DelResp * response)
{
    db.Delete(*request);
    
    return grpc::Status::OK;
}
