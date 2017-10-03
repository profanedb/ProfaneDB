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

#ifndef PROFANEDB_DB_H
#define PROFANEDB_DB_H

#include <memory>

#include <profanedb/protobuf/storage.pb.h>

#include <profanedb/vault/storage.h>
#include <profanedb/format/marshaller.h>
#include <profanedb/format/unmarshaller.h>

#include <boost/log/trivial.hpp>

namespace profanedb {

// Db should be the main interface when embedding ProfaneDB
template<typename Message>
class Db
{
public:
    Db(std::shared_ptr<vault::Storage> storage,
       std::shared_ptr< format::Marshaller<Message> > marshaller,
       std::shared_ptr< format::Unmarshaller<Message> > unmarshaller)
      : storage(storage)
      , marshaller(marshaller)
      , unmarshaller(unmarshaller)
    {
    }

    const Message & Get(const protobuf::Key & key) const
    {
        BOOST_LOG_TRIVIAL(debug) << "Retrieving " << key.message_type();
        
        return this->unmarshaller->Unmarshal(this->storage->Retrieve(key));
    }
    
    profanedb::protobuf::Key Put(const Message & message)
    {
        BOOST_LOG_TRIVIAL(debug) << "Marshalling message for storage";
        const protobuf::MessageTreeNode & messageTree = this->marshaller->Marshal(message);
        
        BOOST_LOG_TRIVIAL(debug) << "Storing " << messageTree.message().key().message_type();
        
        this->StoreMessageTree(messageTree);
        
        return messageTree.message().key();
    }
    
    bool Delete(const protobuf::Key & key)
    {
        BOOST_LOG_TRIVIAL(debug) << "Deleting " << key.message_type();
        // TODO
    }

protected:
    // Store a message tree recursively
    void StoreMessageTree(const protobuf::MessageTreeNode & messageTree)
    {
        // Traverse all children messages...
        for (const auto & child: messageTree.children())
            this->StoreMessageTree(child);
        
        // ... and store from the last nested ones to the root
        this->storage->Store(messageTree.message());
    }

private:
    std::shared_ptr<vault::Storage> storage;
    std::shared_ptr< format::Marshaller<Message> > marshaller;
    std::shared_ptr< format::Unmarshaller<Message> > unmarshaller;
};
}

#endif // PROFANEDB_DB_H
