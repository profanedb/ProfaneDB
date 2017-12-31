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

#ifndef PROFANEDB_UTIL_RANDOMGENERATOR_H
#define PROFANEDB_UTIL_RANDOMGENERATOR_H

#include <limits>

#include <boost/random.hpp>
#include <boost/random/random_device.hpp>
#include <boost/random/uniform_int_distribution.hpp>

#include <google/protobuf/message.h>
#include <google/protobuf/stubs/port.h>

namespace profanedb {
namespace util {

// RandomGenerator takes care of filling a protobuf message with random data for testing/benchmark purposes
class RandomGenerator
{
public:
    RandomGenerator(boost::random::mt19937 & gen);
   
    // Iterate through all fields and fill with random data
    void FillRandomly(google::protobuf::Message * message);
    
    // Template function to generate random values for fields
    template<typename T>
    T RandomValue();
    
private:
    boost::random::mt19937 & gen;
    
    void GenerateField(
        google::protobuf::Message * message,
        const google::protobuf::Reflection * reflection,
        const google::protobuf::FieldDescriptor * fd
    );
};
}
}

#endif // PROFANEDB_UTIL_RANDOMGENERATOR_H
