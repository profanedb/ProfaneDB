#ifndef SERVER_H
#define SERVER_H

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>

#include <prodisdb/server/parser.h>

#include <prodisdb/protobuf/db.pb.h>
#include <prodisdb/protobuf/db.grpc.pb.h>

using prodisdb::protobuf::Db;

namespace prodisdb {
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
        grpc::Status Get(grpc::ServerContext* context, const prodisdb::protobuf::GetReq* request, prodisdb::protobuf::GetResp* response) override;
        
        grpc::Status Put(grpc::ServerContext* context, const prodisdb::protobuf::PutReq* request, prodisdb::protobuf::PutResp* response) override;
        
        grpc::Status Delete(grpc::ServerContext* context, const prodisdb::protobuf::DelReq* request, prodisdb::protobuf::DelResp* response) override;
        
    private:
        Parser parser;
    };
    DbServiceImpl service;
};

}
}

#endif // SERVER_H
