/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
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

#include "storage.h"

using profanedb::protobuf::StorableMessage;
using profanedb::protobuf::Key;

namespace profanedb {
namespace vault {
namespace rocksdb {

Storage::Storage(std::unique_ptr<DB> rocksDb)
  : rocksDb(std::move(rocksDb))
{
}

void Storage::Store(const StorableMessage & storable)
{
    this->rocksDb->Put(::rocksdb::WriteOptions(),
                       storable.key().SerializeAsString(),
                       storable.payload());
}

StorableMessage Storage::Retrieve(const Key & key) const
{
    StorableMessage stored;
    
    *stored.mutable_key() = key;
    
    this->rocksDb->Get(::rocksdb::ReadOptions(),
                       key.SerializeAsString(),
                       stored.mutable_payload());
    
    return stored;
}

}
}
}
