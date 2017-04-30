#ifndef SERVER_H
#define SERVER_H

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>

#include <profanedb/server/parser.h>

#include <profanedb/protobuf/db.pb.h>
#include <profanedb/protobuf/db.grpc.pb.h>

using profanedb::protobuf::Db;

namespace profanedb {
namespace server {

class Server
{
public:
    Server();
    ~Server();
    
    void Run();
    
private:
    void HandleRpcs();
    
    std::unique_ptr<grpc::Server> server;
    
    class DbServiceImpl : public Db::Service {
    public:
        grpc::Status Get(grpc::ServerContext* context, const profanedb::protobuf::GetReq* request, profanedb::protobuf::GetResp* response) override;
        
        grpc::Status Put(grpc::ServerContext* context, const profanedb::protobuf::PutReq* request, profanedb::protobuf::PutResp* response) override;
        
        grpc::Status Delete(grpc::ServerContext* context, const profanedb::protobuf::DelReq* request, profanedb::protobuf::DelResp* response) override;
        
    private:
        Parser parser;
    };
    DbServiceImpl service;
};

}
}

#endif // SERVER_H
