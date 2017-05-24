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

#ifndef PROFANEDB_VAULT_STORAGE_H
#define PROFANEDB_VAULT_STORAGE_H

#include <profanedb/protobuf/storage.pb.h>

namespace profanedb {
namespace vault {

// Storage takes care of saving and retrieving the data from the actual DB
class Storage {
public:
    virtual ~Storage() = 0;
    
    virtual void Store(const protobuf::StorableMessage & storable) = 0;
    virtual protobuf::StorableMessage Retrieve(const protobuf::Key & key) const = 0;
};
}
}

#endif // PROFANEDB_VAULT_STORAGE_H


