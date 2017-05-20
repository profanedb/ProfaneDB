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

#ifndef PROFANEDB_STORAGE_ROCKS_H
#define PROFANEDB_STORAGE_ROCKS_H

#include "storage.h"

#include <rocksdb/db.h>

namespace profanedb {
namespace storage {

// Use RocksDB as storage layer
class Rocks : profanedb::storage::Storage
{
public:
    Rocks(std::shared_ptr<rocksdb::DB> database);
    ~Rocks();

protected:
    void Store(const profanedb::protobuf::Key & key, const std::string & payload) override;
    std::string Retrieve(const profanedb::protobuf::Key & key) override;
    
private:
    std::shared_ptr<rocksdb::DB> database;
};
}
}

#endif // PROFANEDB_STORAGE_ROCKS_H
