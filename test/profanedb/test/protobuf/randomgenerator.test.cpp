#define BOOST_TEST_MODULE RandomGenerator
#include <boost/test/included/unit_test.hpp>

#include <profanedb/util/randomgenerator.h>

using profanedb::util::RandomGenerator;

using boost::random::mt19937;

using google::protobuf::int32;
using google::protobuf::int64;
using google::protobuf::uint32;
using google::protobuf::uint64;
using google::protobuf::string;

struct Generator
{
private:
    mt19937 gen;
    
public:
    RandomGenerator randomGen;
    
    Generator()
      : gen(std::time(0))
      , randomGen(gen)
    {}
};

BOOST_FIXTURE_TEST_SUITE(randomgenerator, Generator)

#define RANDOM(TYPE)                            \
BOOST_AUTO_TEST_CASE(test_##TYPE)               \
{                                               \
    TYPE a = randomGen.RandomValue< TYPE >();   \
    TYPE b = randomGen.RandomValue< TYPE >();   \
    BOOST_TEST_MESSAGE( #TYPE );                \
    BOOST_TEST_MESSAGE(a);                      \
    BOOST_TEST_MESSAGE(b);                      \
    BOOST_TEST(a != b);                         \
}

RANDOM(int32);
RANDOM(int64);
RANDOM(uint32);
RANDOM(uint64);
RANDOM(string);
RANDOM(double);
RANDOM(float);
RANDOM(bool);

#undef RANDOM

BOOST_AUTO_TEST_SUITE_END()
