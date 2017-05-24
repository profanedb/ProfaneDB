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

#ifndef PROFANEDB_STORAGE_SERVER_H
#define PROFANEDB_STORAGE_SERVER_H

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>

#include <profanedb/db.h>

#include <profanedb/protobuf/db.pb.h>
#include <profanedb/protobuf/db.grpc.pb.h>

namespace profanedb {
namespace server {

// This is a thin layer to use ProfaneDB with gRPC
class Server
{
public:
    Server();
    ~Server();
    
    void Run();
    
private:
    void HandleRpcs();
    
    std::unique_ptr<grpc::Server> server;
    
    class DbServiceImpl : public profanedb::protobuf::Db::Service {
    public:
        DbServiceImpl(profanedb::Db<google::protobuf::Message> & profanedb);
        
        grpc::Status Get(grpc::ServerContext * context, const profanedb::protobuf::GetReq * request, profanedb::protobuf::GetResp* response) override;
        
        grpc::Status Put(grpc::ServerContext * context, const profanedb::protobuf::PutReq * request, profanedb::protobuf::PutResp * response) override;
        
        grpc::Status Delete(grpc::ServerContext * context, const profanedb::protobuf::DelReq * request, profanedb::protobuf::DelResp * response) override;
        
    private:
        profanedb::Db<google::protobuf::Message> & profanedb;
    };
    DbServiceImpl service;
};

}
}

#endif // PROFANEDB_STORAGE_SERVER_H
