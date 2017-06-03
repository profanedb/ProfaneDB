#define BOOST_TEST_MODULE Marshaller
#include <boost/test/included/unit_test.hpp>

#include <profanedb/test/protobuf/schema/test.pb.h>

#include <profanedb/format/protobuf/marshaller.h>
#include <profanedb/vault/rocksdb/storage.h>

using profanedb::format::protobuf::Loader;
using profanedb::format::Marshaller;
using ProtobufMarshaller = profanedb::format::protobuf::Marshaller;
using profanedb::vault::Storage;

// FIXME Should mock this
using RocksStorage = profanedb::vault::rocksdb::Storage;

using google::protobuf::Message;

struct Format
{
    std::shared_ptr<Marshaller<Message>> marshaller;
    
    Format()
    {
        rocksdb::Options rocksOptions;
        rocksOptions.create_if_missing = true;
        
        rocksdb::DB *rocks;
        rocksdb::DB::Open(rocksOptions, "/tmp/profane", &rocks);
        
        auto storage = std::make_shared<RocksStorage>(std::unique_ptr<rocksdb::DB>(rocks));
        
        auto includeSourceTree = new Loader::RootSourceTree{
            "/usr/include", "/home/giorgio/Documents/ProfaneDB/ProfaneDBLib/src"};
            
        auto schemaSourceTree = new Loader::RootSourceTree{"/home/giorgio/Documents/ProfaneDB/ProfaneDBLib/test/profanedb/test/protobuf/schema"};
            
        auto loader = std::make_shared<Loader>(
            std::unique_ptr<Loader::RootSourceTree>(includeSourceTree),
                                                   std::unique_ptr<Loader::RootSourceTree>(schemaSourceTree));
            
        this->marshaller = std::make_shared<ProtobufMarshaller>(storage, loader);
    }
};

BOOST_FIXTURE_TEST_CASE(first, Format)
{
    schema::Test message;
    
    BOOST_TEST_MESSAGE(message.DebugString());
    
    marshaller->Marshal(message);
}
