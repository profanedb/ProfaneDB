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
#include "util.h"

namespace profanedb {
namespace format {
namespace protobuf {

class Marshaller : public profanedb::format::Marshaller<google::protobuf::Message>
{
public:
    Marshaller(std::shared_ptr<Loader> loader);
    
    virtual profanedb::protobuf::MessageTreeNode Marshal(const google::protobuf::Message & message) override;
private:
    // Loader contains the schemaPool and normalizedPool
    const std::shared_ptr<Loader> loader;
    
    // Convert a field from a message to a Key object
    profanedb::protobuf::Key FieldToKey(
        const google::protobuf::Message & message,
        const google::protobuf::FieldDescriptor * fd);
};
}
}
}

#endif // PROFANEDB_FORMAT_PROTOBUF_MARSHALLER_H
