#define BOOST_TEST_MODULE Unmarshaller
#include <boost/test/included/unit_test.hpp>

#include <boost/log/expressions.hpp>

#include <profanedb/test/protobuf/schema/test.pb.h>
#include <profanedb/protobuf/storage.pb.h>

#include <profanedb/db.hpp>

#include <profanedb/format/protobuf/marshaller.h>
#include <profanedb/format/protobuf/unmarshaller.h>

#include <profanedb/vault/memory/storage.h>

#include <profanedb/util/randomgenerator.h>

using profanedb::format::protobuf::Loader;
using profanedb::format::Marshaller;
using ProtobufMarshaller = profanedb::format::protobuf::Marshaller;
using profanedb::format::Unmarshaller;
using ProtobufUnmarshaller = profanedb::format::protobuf::Unmarshaller;

using profanedb::vault::Storage;
using MemoryStorage = profanedb::vault::memory::Storage;

using profanedb::Db;

using profanedb::protobuf::MessageTreeNode;
using profanedb::protobuf::Key;

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
    std::shared_ptr<Storage> storage;
    std::shared_ptr< ProtobufMarshaller > marshaller;
    std::shared_ptr< ProtobufUnmarshaller > unmarshaller;
    
    std::unique_ptr< Db<Message> > profane;
    
    RandomGenerator randomGen;
    
    Format()
      : gen(std::time(0))
      , randomGen(gen)
    {
        auto includeSourceTree = new Loader::RootSourceTree({
            "/usr/include", "/home/giorgio/Documents/ProfaneDB/ProfaneDB/src"});
            
        auto schemaSourceTree = new Loader::RootSourceTree({
            "/home/giorgio/Documents/ProfaneDB/ProfaneDB/test/profanedb/test/protobuf/schema"});
            
        this->loader = std::make_shared<Loader>(
            std::unique_ptr<Loader::RootSourceTree>(includeSourceTree),
            std::unique_ptr<Loader::RootSourceTree>(schemaSourceTree));
        
        this->storage = std::make_shared<MemoryStorage>();
            
        this->marshaller = std::make_shared<ProtobufMarshaller>(loader);
        this->unmarshaller = std::make_shared<ProtobufUnmarshaller>(storage, loader);
        
        this->profane = std::make_unique< Db<Message> >(storage, marshaller, unmarshaller);
    }
};

BOOST_FIXTURE_TEST_SUITE(unmarshal, Format)

// TODO Acutal test
#define RANDOM_TEST(MESSAGE)                            \
BOOST_AUTO_TEST_CASE( MESSAGE )                         \
{                                                       \
    schema::MESSAGE message;                            \
    randomGen.FillRandomly(&message);                   \
    Key key = profane->Put(message);                    \
    profane->Get(key);                                  \
}

RANDOM_TEST(KeyInt);
RANDOM_TEST(KeyStr);
RANDOM_TEST(RepeatedKeyInt);
RANDOM_TEST(NonKeyableNested);
RANDOM_TEST(KeyableNested);
RANDOM_TEST(MessageAsKey);

#undef RANDOM_TEST

BOOST_AUTO_TEST_SUITE_END()

