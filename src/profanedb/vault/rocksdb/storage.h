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

#ifndef PROFANEDB_VAULT_ROCKSDB_STORAGE_H
#define PROFANEDB_VAULT_ROCKSDB_STORAGE_H

#include <rocksdb/db.h>

#include <profanedb/vault/storage.h>

using rocksdb::DB;

namespace profanedb {
namespace vault {
namespace rocksdb {

class Storage : profanedb::vault::Storage
{
public:
    Storage(std::shared_ptr<DB> rocksDb);
    
    virtual void Store(const profanedb::protobuf::StorableMessage & storable) override;
    virtual profanedb::protobuf::StorableMessage Retrieve(const profanedb::protobuf::Key & key) const override;
    
private:
    const std::shared_ptr<DB> rocksDb;
};
}
}
}

#endif // PROFANEDB_VAULT_ROCKSDB_STORAGE_H
