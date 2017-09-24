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

#ifndef PROFANEDB_FORMAT_PROTOBUF_MARSHALLER_H
#define PROFANEDB_FORMAT_PROTOBUF_MARSHALLER_H

#include <memory>

#include <profanedb/protobuf/options.pb.h>
#include <profanedb/vault/storage.h>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>

#include <boost/log/trivial.hpp>

#include <profanedb/format/marshaller.h>

#include "loader.h"

namespace profanedb {
namespace format {
namespace protobuf {

class Marshaller : public profanedb::format::Marshaller<google::protobuf::Message>
{
public:
    Marshaller(
        std::shared_ptr<profanedb::vault::Storage> storage,
        std::shared_ptr<Loader> loader);
    
    virtual profanedb::protobuf::MessageTreeNode Marshal(const google::protobuf::Message & message) override;
    virtual const google::protobuf::Message & Unmarshal(const profanedb::protobuf::StorableMessage & storable) override;
    
    // TODO Instead of iterating through all fields, Loader should give this info
    virtual const profanedb::protobuf::Key & GetKey(const google::protobuf::Message & message) override;
private:
    // Loader contains the schemaPool and normalizedPool
    const std::shared_ptr<Loader> loader;
    
    // Because a StorableMessage only holds references to its children objects,
    // Storage is used to recursively retrieve them.
    const std::shared_ptr<profanedb::vault::Storage> storage;
    
    // Copy a Message, can't use MergeFrom because it checks Descriptors
    void CopyMessage(
        const google::protobuf::Message & from,
        google::protobuf::Message * to);

    // Copy a field from a message to another.
    // Differs from MergeFrom because it doesn't check whether Descriptors match.
    void CopyField(
        const google::protobuf::FieldDescriptor * fromField,
        const google::protobuf::Message & from,
        google::protobuf::Message * to);
    
    // Convert a field from a message to a Key object
    profanedb::protobuf::Key FieldToKey(
        const google::protobuf::Message & message,
        const google::protobuf::FieldDescriptor * fd);
};
}
}
}

#endif // PROFANEDB_FORMAT_PROTOBUF_MARSHALLER_H
