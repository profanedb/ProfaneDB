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

void profanedb::storage::Storage::Put(const profanedb::protobuf::MessageTreeNode & messageTree)
{
    for (auto const & child: messageTree.children()) {
        Put(child);
    }
    
    const profanedb::protobuf::StorableMessage & current = messageTree.message();
    
    this->Store(current.key(), current.payload());
}

const profanedb::protobuf::MessageTreeNode profanedb::storage::Storage::Get(const profanedb::protobuf::Key & key) const
{
    profanedb::protobuf::MessageTreeNode messageTree;
    *messageTree.mutable_message()->mutable_key() = key;
    *messageTree.mutable_message()->mutable_payload() = this->Retrieve(key);
    
    // TODO Parse message and foreach nested
    // messageTree.add_children( Retrieve(nestedKey) )
    
    return messageTree;
}
