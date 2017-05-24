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

profanedb::vault::rocksdb::Storage::Storage(std::shared_ptr<DB> rocksDb)
  : rocksDb(rocksDb)
{
}

void profanedb::vault::rocksdb::Storage::Store(const profanedb::protobuf::StorableMessage & storable)
{
    this->rocksDb->Put(::rocksdb::WriteOptions(),
                       storable.key().SerializeAsString(),
                       storable.payload());
}

profanedb::protobuf::StorableMessage profanedb::vault::rocksdb::Storage::Retrieve(const profanedb::protobuf::Key & key) const
{
    protobuf::StorableMessage stored;
    
    *stored.mutable_key() = key;
    
    this->rocksDb->Get(::rocksdb::ReadOptions(),
                       key.SerializeAsString(),
                       stored.mutable_payload());
    
    return stored;
}
