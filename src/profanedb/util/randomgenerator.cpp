/*
 * ProfaneDB - A Protocol Buffers database.
 * Copyright (C) 2017  "Giorgio Azzinnaro" <giorgio.azzinnaro@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "randomgenerator.h"

using boost::random::mt19937;
using boost::random::uniform_int_distribution;
using boost::random::uniform_real_distribution;

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;

using std::numeric_limits;

namespace profanedb {
namespace util {

RandomGenerator::RandomGenerator(mt19937 & gen)
  : gen(gen)
{
}

#define RANDOM_VALUE(TYPE, BODY) template<> TYPE RandomGenerator::RandomValue< TYPE >() { BODY }
#define INT_RANDOM_VALUE(TYPE) RANDOM_VALUE(TYPE, \
    uniform_int_distribution< TYPE > range(       \
        numeric_limits< TYPE >::min(),            \
        numeric_limits< TYPE >::max());           \
    return range(this->gen);                      )

INT_RANDOM_VALUE(google::protobuf::int32);
INT_RANDOM_VALUE(google::protobuf::int64);
INT_RANDOM_VALUE(google::protobuf::uint32);
INT_RANDOM_VALUE(google::protobuf::uint64);

#undef INT_RANDOM_VALUE

#define REAL_RANDOM_VALUE(TYPE) RANDOM_VALUE(TYPE, \
    uniform_real_distribution<> range(             \
        numeric_limits< TYPE >::min(),             \
        numeric_limits< TYPE >::max());            \
    return range(this->gen);                       )

REAL_RANDOM_VALUE(double);
REAL_RANDOM_VALUE(float);

#undef REAL_RANDOM_VALUE

RANDOM_VALUE(google::protobuf::string,
    std::string random;         
    std::string chars(
        "abcdefghijklmnopqrstuvwxyz"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "1234567890"
        "!@#$%^&*()"
        "`~-_=+[{]}\\|;:'\",<.>/? ");
    
    uniform_int_distribution<> index_dist(0, chars.size() - 1);
    for(int i = 0; i < this->RandomValue<uint>(); i++)
        random += chars[index_dist(gen)];
        
    return random;
)

RANDOM_VALUE(bool,
    uniform_int_distribution<> range(0, 1);
    return (bool) range(this->gen);
)
#undef RANDOM_VALUE

void RandomGenerator::FillRandomly(Message * message)
{
    // Descriptor is used to iterate through the existing fields
    const Descriptor * descriptor = message->GetDescriptor();
    
    // Reflection is used to set the values without knowing the types at compile time
    const Reflection * reflection = message->GetReflection();
    
    for(int i = 0; i < descriptor->field_count(); i++) {
        const FieldDescriptor * fd = descriptor->field(i);
        
        // Fill field randomly
        this->GenerateField(message, reflection, fd);
    }
}

void RandomGenerator::GenerateField(
    Message * message,
    const Reflection * reflection,
    const FieldDescriptor * fd)
{
    // TODO Enums
    if (fd->is_repeated()) {
        for (uint k = 0; k < this->RandomValue<google::protobuf::uint32>(); k++) {
            switch (fd->cpp_type()) {
            #define HANDLE_TYPE(CPPTYPE, METHOD, TYPE)                           \
            case FieldDescriptor::CPPTYPE_##CPPTYPE:                             \
                reflection->Add##METHOD(message, fd, this->RandomValue<TYPE>()); \
                break;
        
            HANDLE_TYPE(INT32 , Int32 , google::protobuf::int32 );
            HANDLE_TYPE(INT64 , Int64 , google::protobuf::int64 );
            HANDLE_TYPE(UINT32, UInt32, google::protobuf::uint32);
            HANDLE_TYPE(UINT64, UInt64, google::protobuf::uint64);
            HANDLE_TYPE(STRING, String, google::protobuf::string);
            HANDLE_TYPE(DOUBLE, Double, double                  );
            HANDLE_TYPE(FLOAT , Float , float                   );
            HANDLE_TYPE(BOOL  , Bool  , bool                    );
        
            #undef HANDLE_TYPE
            
            case FieldDescriptor::CPPTYPE_MESSAGE:
                this->FillRandomly(reflection->AddMessage(message, fd));
            }
        }
    }
    else {
        switch (fd->cpp_type()) {
        #define HANDLE_TYPE(CPPTYPE, METHOD, TYPE)                           \
        case FieldDescriptor::CPPTYPE_##CPPTYPE:                             \
            reflection->Set##METHOD(message, fd, this->RandomValue<TYPE>()); \
            break;
        
        HANDLE_TYPE(INT32 , Int32 , google::protobuf::int32 );
        HANDLE_TYPE(INT64 , Int64 , google::protobuf::int64 );
        HANDLE_TYPE(UINT32, UInt32, google::protobuf::uint32);
        HANDLE_TYPE(UINT64, UInt64, google::protobuf::uint64);
        HANDLE_TYPE(STRING, String, google::protobuf::string);
        HANDLE_TYPE(DOUBLE, Double, double                  );
        HANDLE_TYPE(FLOAT , Float , float                   );
        HANDLE_TYPE(BOOL  , Bool  , bool                    );
        
        #undef HANDLE_TYPE
        
        case FieldDescriptor::CPPTYPE_MESSAGE:
            this->FillRandomly(reflection->MutableMessage(message, fd));
            break;
        }
    }
}

}
}
