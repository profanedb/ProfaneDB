#include "server.h"

prodisdb::server::Server::Server()
{
}

prodisdb::server::Server::~Server()
{
    server->Shutdown();
}

void prodisdb::server::Server::Run()
{
    std::string address("0.0.0.0:50051");

    grpc::ServerBuilder builder;
    
    builder.AddListeningPort(address, grpc::InsecureServerCredentials());
    builder.RegisterService(&service);
    
    server = builder.BuildAndStart();
    
    std::cout << "Server listening on " << address << std::endl;
    
    HandleRpcs();
}

void prodisdb::server::Server::HandleRpcs()
{
    server->Wait();
}

grpc::Status prodisdb::server::Server::DbServiceImpl::Get(grpc::ServerContext* context, const prodisdb::protobuf::PrimaryKey* request, prodisdb::protobuf::Serializable* response)
{
}

grpc::Status prodisdb::server::Server::DbServiceImpl::Create(grpc::ServerContext* context, const prodisdb::protobuf::Serializable* request, prodisdb::protobuf::Empty* response)
{
    parser.ParseMessage(*request);
}

grpc::Status prodisdb::server::Server::DbServiceImpl::Delete(::grpc::ServerContext* context, const prodisdb::protobuf::PrimaryKey* request, prodisdb::protobuf::Empty* response)
{
}

grpc::Status prodisdb::server::Server::DbServiceImpl::Update(grpc::ServerContext* context, const prodisdb::protobuf::Serializable* request, prodisdb::protobuf::Empty* response)
{
}
