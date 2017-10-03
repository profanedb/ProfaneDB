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

#include "storage.h"

using profanedb::protobuf::StorableMessage;
using profanedb::protobuf::Key;

using google::protobuf::Any;

namespace profanedb {
namespace vault {
namespace memory {
    
void Storage::Store(const StorableMessage & storable)
{
    this->memory.insert({storable.key().SerializeAsString(), storable.payload()});
}

const Any Storage::LoadFromStorage(const Key & key) const
{
    return this->memory.at(key.SerializeAsString());
}

}
}
}
