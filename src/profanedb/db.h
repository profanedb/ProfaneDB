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

#include <profanedb/boot/schema.h>
#include <profanedb/vault/storage.h>

#include <profanedb/protobuf/storage.pb.h>

#ifndef PROFANEDB_DB_H
#define PROFANEDB_DB_H

namespace profanedb {

// Db should be the main interface when embedding ProfaneDB
template<typename Message>
class Db
{
public:
    Db();
    ~Db();

    virtual const Message & Get(const protobuf::Key & key) const = 0;
    virtual bool Put(const Message & message) = 0;
    virtual bool Delete(const protobuf::Key & key) = 0;

private:
    std::shared_ptr< boot::Schema<Message> > schema;
    std::shared_ptr<vault::Storage> storage;
};
}

#endif // PROFANEDB_DB_H
