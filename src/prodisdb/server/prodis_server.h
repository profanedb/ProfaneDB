#ifndef PRODISSERVER_H
#define PRODISSERVER_H

#include <grpc++/grpc++.h>

#include <prodisdb/protobuf/db.pb.h>
#include <prodisdb/protobuf/db.grpc.pb.h>

using prodisdb::protobuf::Empty;
using prodisdb::protobuf::Serializable;
using prodisdb::protobuf::PrimaryKey;

using prodisdb::protobuf::Db;

class ProdisServer
{
public:
    ~ProdisServer();
    
    void Run();
    
private:
    
    void HandleRpcs();
    
    std::unique_ptr<grpc::ServerCompletionQueue> cq;
    Db::AsyncService service;
    std::unique_ptr<grpc::Server> server;
    
    class CallData {
    public:
        CallData(Db::AsyncService *service, grpc::ServerCompletionQueue *cq);
        void Proceed();
        
    private:
        Db::AsyncService *mService;
        grpc::ServerCompletionQueue *mCq;
        
        grpc::ServerContext mContext;
        
        Empty pbEmpty;
        Serializable pbSerializable;
        PrimaryKey pbPrimaryKey;
        
        grpc::ServerAsyncResponseWriter<Serializable> serializableResp;
        grpc::ServerAsyncResponseWriter<Empty> emptyResp;
        
        enum CallStatus { CREATE, PROCESS, FINISH };
        CallStatus mStatus;
    };
};

#endif // PRODISSERVER_H
