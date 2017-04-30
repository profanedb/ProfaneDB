#ifndef SERVER_H
#define SERVER_H

#include <grpc++/grpc++.h>
#include <grpc/support/log.h>

#include <profanedb/storage/db.h>

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
        grpc::Status Get(grpc::ServerContext * context, const profanedb::protobuf::GetReq * request, profanedb::protobuf::GetResp* response) override;
        
        grpc::Status Put(grpc::ServerContext * context, const profanedb::protobuf::PutReq * request, profanedb::protobuf::PutResp * response) override;
        
        grpc::Status Delete(grpc::ServerContext * context, const profanedb::protobuf::DelReq * request, profanedb::protobuf::DelResp * response) override;
        
    private:
        profanedb::storage::Db db;
    };
    DbServiceImpl service;
};

}
}

#endif // SERVER_H
