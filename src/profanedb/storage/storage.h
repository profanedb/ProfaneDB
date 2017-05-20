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

#ifndef PROFANEDB_STORAGE_STORAGE_H
#define PROFANEDB_STORAGE_STORAGE_H

#include <google/protobuf/message.h>

#include <profanedb/protobuf/storage.pb.h>

namespace profanedb {
namespace storage {

// Handles storage and retrieval of objects,
// subclass to implement a storage layer
class Storage {
public:
	virtual ~Storage() = 0;
    
    void Put(const profanedb::protobuf::MessageTreeNode & messageTree);
    const profanedb::protobuf::MessageTreeNode Get(const profanedb::protobuf::Key & key) const;
    
protected:
    virtual void Store(const profanedb::protobuf::Key & key, const std::string & payload) = 0;
    virtual const std::string Retrieve(const profanedb::protobuf::Key & key) const = 0;
};
}
}

#endif /* PROFANEDB_STORAGE_STORAGE_H */
