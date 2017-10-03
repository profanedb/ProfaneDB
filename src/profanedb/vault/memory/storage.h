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

#ifndef PROFANEDB_VAULT_MEMORY_STORAGE_H
#define PROFANEDB_VAULT_MEMORY_STORAGE_H

#include <unordered_map>

#include <profanedb/vault/storage.h>

namespace profanedb {
namespace vault {
namespace memory {

// A Storage backend using temporary memory
class Storage : public profanedb::vault::Storage
{
public:
    virtual void Store(const profanedb::protobuf::StorableMessage & storable) override;
    
protected:
    virtual const google::protobuf::Any LoadFromStorage(const profanedb::protobuf::Key & key) const override;
    
private:
    std::unordered_map<std::string, const google::protobuf::Any &> memory;
};
}
}
}

#endif // PROFANEDB_VAULT_ROCKSDB_STORAGE_H

