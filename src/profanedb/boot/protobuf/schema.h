/*
 * <one line to give the program's name and a brief idea of what it does.>
 * Copyright (C) 2017  <copyright holder> <email>
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

#ifndef PROFANEDB_BOOT_PROTOBUF_SCHEMA_H
#define PROFANEDB_BOOT_PROTOBUF_SCHEMA_H

#include <google/protobuf/message.h>

#include <profanedb/protobuf/storage.pb.h>
#include <profanedb/protobuf/options.pb.h>

#include <profanedb/boot/schema.h>

namespace profanedb {
namespace boot {
namespace protobuf {    

using google::protobuf::Message;
using google::protobuf::Reflection;
using google::protobuf::Descriptor;
using google::protobuf::FieldDescriptor;

// Redundant here, might be relevant for other kind of "Message" classes,
// to copy and paste from profanedb::boot::Schema interface
// Can be changed if collisions occur
using Message = google::protobuf::Message;

// Protobuf effectively stores all info regarding a Message in its Descriptor,
// so considering the Message was created before, we don't require any dependency here
class Schema : public profanedb::boot::Schema <Message>
{
public:
    Schema();
    ~Schema();
    
    // Check whether a Message has a key, therefore can be stored
    virtual bool IsKeyable(const Message & message) const override;
    
    // The Key is defined as an option of the given message
    virtual profanedb::protobuf::Key GetKey(const Message & message) const override;
    
    // Retrieve nested messages from a message
    virtual std::vector<const Message *> GetNestedMessages(const Message & message) const override;
    
private:
    static profanedb::protobuf::Key FieldToKey(const Message & message, const FieldDescriptor * fd);
};
}
}
}

#endif // PROFANEDB_BOOT_PROTOBUFSCHEMA_H
