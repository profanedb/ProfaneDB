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

#ifndef PROFANEDB_FORMAT_MARSHALLER_H
#define PROFANEDB_FORMAT_MARSHALLER_H

#include <profanedb/protobuf/storage.pb.h>

namespace profanedb {
namespace format {

template<typename Message>
class Marshaller
{
public:
    virtual ~Marshaller() = 0;
    
    virtual profanedb::protobuf::MessageTreeNode Marshal(const Message & message) = 0;
    virtual const Message & Unmarshal(const profanedb::protobuf::StorableMessage & storable) = 0;
};
}
}

#endif // PROFANEDB_FORMAT_MARSHALLER_H

