#define BOOST_TEST_MODULE Marshaller
#include <boost/test/included/unit_test.hpp>

#include <profanedb/test/protobuf/schema/test.pb.h>
#include <profanedb/protobuf/storage.pb.h>

#include <profanedb/format/protobuf/marshaller.h>
#include <profanedb/vault/rocksdb/storage.h>

using profanedb::format::protobuf::Loader;
using profanedb::format::Marshaller;
using ProtobufMarshaller = profanedb::format::protobuf::Marshaller;
using profanedb::vault::Storage;
using profanedb::protobuf::MessageTreeNode;

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


#define DEBUG_MESSAGE BOOST_TEST_MESSAGE(message.DebugString());           \
                      MessageTreeNode tree = marshaller->Marshal(message); \
                      BOOST_TEST_MESSAGE(tree.DebugString());

BOOST_FIXTURE_TEST_CASE(int_key, Format)
{
    schema::KeyInt message;
    message.set_int_key(17526); // TODO Should be random
    
    DEBUG_MESSAGE

    BOOST_TEST(std::to_string(message.int_key()) == tree.message().key().value());
}

BOOST_FIXTURE_TEST_CASE(string_key, Format)
{
    schema::KeyStr message;
    message.set_string_key("hp9gh3bv3wgqq"); // TODO Should be random
    
    DEBUG_MESSAGE
    
    BOOST_TEST(message.string_key() == tree.message().key().value());
}

BOOST_FIXTURE_TEST_CASE(repeated_int_key, Format)
{
    schema::RepeatedKeyInt message;
    message.add_int_key_repeated(29813);
    message.add_int_key_repeated(465560);
    message.add_int_key_repeated(8746);
    
    DEBUG_MESSAGE
}

BOOST_FIXTURE_TEST_CASE(nonkeyable_nested, Format)
{
    schema::NonKeyableNested message;
    message.set_int_key(98125489);
    message.mutable_nested_nonkeyable_message()->set_boolean(true);
    
    DEBUG_MESSAGE
}

BOOST_FIXTURE_TEST_CASE(keyable_nested, Format)
{
    schema::KeyableNested message;
    message.set_str_key("oht24oh24t");
    message.mutable_nested_keyable()->set_int_key(11290142);
    
    DEBUG_MESSAGE
}

#undef DEBUG_MESSAGE
