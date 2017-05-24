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

#ifndef PROFANEDB_BOOT_SCHEMA_H
#define PROFANEDB_BOOT_SCHEMA_H

#include <profanedb/protobuf/storage.pb.h>

namespace profanedb {
namespace boot {

// A Schema defines a Key for each message,
// a Key is a unique identifier for that given message.
// This data might be retrieved directly from the message,
// or the Schema might need to parse some definition to know what to do
template <typename Message>
class Schema {
public:
    virtual ~Schema() = 0;
    
    // Check whether a Message has a key, therefore can be stored
    virtual bool IsKeyable(const Message & message) const = 0;
    
    // Extract a Key from a Message
    virtual protobuf::Key GetKey(const Message & message) const = 0;
    
    // Retrieve nested messages from a message
    virtual std::vector<const Message *> GetNestedMessages(const Message & message) const = 0;
};
}
}

#endif // PROFANEDB_BOOT_SCHEMA_H
