#define BOOST_TEST_MODULE Marshaller
#include <boost/test/included/unit_test.hpp>

#include <boost/log/expressions.hpp>

#include <profanedb/test/protobuf/schema/test.pb.h>
#include <profanedb/protobuf/storage.pb.h>

#include <profanedb/format/protobuf/marshaller.h>
#include <profanedb/vault/rocksdb/storage.h>

#include <profanedb/util/randomgenerator.h>

using profanedb::format::protobuf::Loader;
using profanedb::format::Marshaller;
using ProtobufMarshaller = profanedb::format::protobuf::Marshaller;
using profanedb::vault::Storage;
using profanedb::protobuf::MessageTreeNode;

using RocksStorage = profanedb::vault::rocksdb::Storage;

using profanedb::util::RandomGenerator;

using google::protobuf::Message;

using boost::random::random_device;
using boost::random::mt19937;

struct Format
{
private:
    mt19937 gen;

public:
    std::shared_ptr<Loader> loader;
    std::shared_ptr<ProtobufMarshaller> marshaller;
    std::shared_ptr<RocksStorage> storage;
    
    RandomGenerator randomGen;
    
    Format()
      : gen(std::time(0))
      , randomGen(gen)
    {
        rocksdb::Options rocksOptions;
        rocksOptions.create_if_missing = true;
        
        rocksdb::DB *rocks;
        rocksdb::DB::Open(rocksOptions, "/tmp/profane", &rocks);
        
        this->storage = std::make_shared<RocksStorage>(std::unique_ptr<rocksdb::DB>(rocks));
        
        auto includeSourceTree = new Loader::RootSourceTree({
            "/usr/include", "/home/giorgio/Documents/ProfaneDB/ProfaneDB/src"});
            
        auto schemaSourceTree = new Loader::RootSourceTree({
            "/home/giorgio/Documents/ProfaneDB/ProfaneDB/test/profanedb/test/protobuf/schema"});
            
        this->loader = std::make_shared<Loader>(
            std::unique_ptr<Loader::RootSourceTree>(includeSourceTree),
            std::unique_ptr<Loader::RootSourceTree>(schemaSourceTree));
            
        this->marshaller = std::make_shared<ProtobufMarshaller>(storage, loader);
    }
};

BOOST_FIXTURE_TEST_SUITE(marshal, Format)

// TODO Acutal test
#define RANDOM_TEST(MESSAGE)                            \
BOOST_AUTO_TEST_CASE( MESSAGE )                         \
{                                                       \
    schema::MESSAGE message;                            \
    randomGen.FillRandomly(&message);                   \
    BOOST_TEST_MESSAGE(message.DebugString());          \
    MessageTreeNode tree = marshaller->Marshal(message);\
    BOOST_TEST_MESSAGE(tree.DebugString());             \
}

RANDOM_TEST(KeyInt);
RANDOM_TEST(KeyStr);
RANDOM_TEST(RepeatedKeyInt);
RANDOM_TEST(NonKeyableNested);
RANDOM_TEST(KeyableNested);
RANDOM_TEST(MessageAsKey);

#undef RANDOM_TEST

BOOST_AUTO_TEST_SUITE_END()
