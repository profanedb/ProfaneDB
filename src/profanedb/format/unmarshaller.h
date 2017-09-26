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

#ifndef PROFANEDB_FORMAT_UNMARSHALLER_H
#define PROFANEDB_FORMAT_UNMARSHALLER_H

#include <profanedb/protobuf/storage.pb.h>

namespace profanedb {
namespace format {

// Unmarshaller deserialises a single message as coming from the database back to the original format,
// it would usually require to interact with profanedb::vault::Storage as well
template<typename Message>
class Unmarshaller
{
public:
    // Turn a stored message back into the original format
    virtual const Message & Unmarshal(const profanedb::protobuf::StorableMessage & storable) = 0;
};
}
}

#endif // PROFANEDB_FORMAT_UNMARSHALLER_H
