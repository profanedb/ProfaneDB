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
    std::shared_ptr<Loader> loader;
    std::shared_ptr<ProtobufMarshaller> marshaller;
    
    Format()
    {
        rocksdb::Options rocksOptions;
        rocksOptions.create_if_missing = true;
        
        rocksdb::DB *rocks;
        rocksdb::DB::Open(rocksOptions, "/tmp/profane", &rocks);
        
        auto storage = std::make_shared<RocksStorage>(std::unique_ptr<rocksdb::DB>(rocks));
        
        auto includeSourceTree = new Loader::RootSourceTree{
            "/usr/include", "/home/giorgio/Documents/ProfaneDB/ProfaneDB/src"};
            
        auto schemaSourceTree = new Loader::RootSourceTree{"/home/giorgio/Documents/ProfaneDB/ProfaneDB/test/profanedb/test/protobuf/schema"};
            
        this->loader = std::make_shared<Loader>(
            std::unique_ptr<Loader::RootSourceTree>(includeSourceTree),
            std::unique_ptr<Loader::RootSourceTree>(schemaSourceTree));
            
        this->marshaller = std::make_shared<ProtobufMarshaller>(storage, loader);
    }
};

BOOST_FIXTURE_TEST_CASE(load, Format)
{
    loader->GetPool(Loader::SCHEMA).FindFileByName("test.proto");
}

BOOST_FIXTURE_TEST_CASE(marshal, Format)
{
    schema::Test message;
    
    BOOST_TEST_MESSAGE(message.DebugString());
    
    marshaller->Marshal(message);
}
