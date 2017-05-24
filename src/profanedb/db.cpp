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

#include "db.h"

template<typename Message>
profanedb::Db<Message>::Db(
    std::shared_ptr< profanedb::boot::Schema<Message> > schema,
    std::shared_ptr<profanedb::vault::Storage> storage)
  : schema(schema)
  , storage(storage)
{
}

template<typename Message>
profanedb::Db<Message>::~Db()
{
}

template<typename Message>
const Message & profanedb::Db<Message>::Get(const protobuf::Key & key) const
{
  this->schema->
}

bool profanedb::Db<Message>::Put(const Message & message)
{
}

bool profanedb::Db<Message>::Delete(const protobuf::Key & key)
{
}
