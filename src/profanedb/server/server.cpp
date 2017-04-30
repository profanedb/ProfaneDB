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

grpc::Status profanedb::server::Server::DbServiceImpl::Get(grpc::ServerContext* context, const profanedb::protobuf::GetReq* request, profanedb::protobuf::GetResp* response)
{
    db.Get(*request);
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Put(grpc::ServerContext* context, const profanedb::protobuf::PutReq* request, profanedb::protobuf::PutResp* response)
{
    db.Put(*request);
    
    return grpc::Status::OK;
}

grpc::Status profanedb::server::Server::DbServiceImpl::Delete(grpc::ServerContext* context, const profanedb::protobuf::DelReq* request, profanedb::protobuf::DelResp* response)
{
    db.Delete(*request);
    
    return grpc::Status::OK;
}
