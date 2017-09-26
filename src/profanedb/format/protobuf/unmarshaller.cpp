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

#include "unmarshaller.h"

using google::protobuf::Message;
using google::protobuf::Descriptor;
using google::protobuf::DescriptorPool;
using google::protobuf::FieldDescriptor;
using google::protobuf::Reflection;

using profanedb::vault::Storage;

using profanedb::protobuf::MessageTreeNode;
using profanedb::protobuf::StorableMessage;
using profanedb::protobuf::Key;

namespace profanedb {
namespace format {
namespace protobuf {

Unmarshaller::Unmarshaller(
    std::shared_ptr<Storage> storage,
    std::shared_ptr<Loader> loader)
  : loader(loader)
  , storage(storage)
{
}

const Message & Unmarshaller::Unmarshal(const StorableMessage & storable)
{
    // An empty normalized message is generated using the Key
    Message * normalizedMessage
        = this->loader->CreateMessage(Loader::NORMALIZED,
                                      storable.key().message_type())->New();

    // The original message is also retrieved
    Message * originalMessage
        = this->loader->CreateMessage(Loader::SCHEMA,
                                      storable.key().message_type())->New();
    
    // StorableMessage payload contains the serialized normalized message,
    // as previously stored into the DB
    storable.payload().UnpackTo(normalizedMessage);
    
    // Similarly to Marshal, set fields are retrieved
    std::vector< const FieldDescriptor * > setFields;
    normalizedMessage->GetReflection()->ListFields(*normalizedMessage, &setFields);
    
    for (const auto normalizedField: setFields) {
        
        // Whenever a Key is found, it is considered a separate normalized message to be retrieved
        if (normalizedField->type() == FieldDescriptor::TYPE_MESSAGE
            && normalizedField->message_type() == Key::descriptor()
        ) {
            // Get the key of the nested message (is embedded as Key object)
            Key nestedKey;
            nestedKey.MergeFrom(normalizedMessage->GetReflection()->GetMessage(*normalizedMessage, normalizedField));
            
            // Retrieve the nested message from storage and unmarshal it as well
            const Message & nestedMessage = this->Unmarshal(this->storage->Retrieve(nestedKey));
            
            // We need the original descriptor field to set the message
            const FieldDescriptor * originalField = originalMessage->GetDescriptor()->field(normalizedField->index());
            
            // Copy the nested unmarshalled message to the original one
            CopyMessage(nestedMessage,
                              originalMessage->GetReflection()->MutableMessage(
                                originalMessage, originalField));
        }
        else { // if field is not a reference
            
            // Just like in Marshal, other fields are simply copied over,
            // as normalized and original descriptors look the same except for nested keyable messages
            CopyField(normalizedField, *normalizedMessage, originalMessage);
        }
    }
            
    return *originalMessage;
}

}
}
}
