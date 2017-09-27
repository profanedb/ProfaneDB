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

#include "marshaller.h"

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

Marshaller::Marshaller(
    std::shared_ptr<Loader> loader)
  : loader(loader)
{
}

MessageTreeNode Marshaller::Marshal(const Message & message)
{
    BOOST_LOG_TRIVIAL(debug) << "Marshalling message " << message.GetTypeName();
    BOOST_LOG_TRIVIAL(trace) << std::endl << message.DebugString();
    
    MessageTreeNode messageTree;
    
    // The normalized message will be filled with data coming from input message,
    // replacing references to other objects with their keys.
    // It will then be serialized and set as storable message payload in messageTree;
    Message * normalizedMessage
        = this->loader->CreateMessage(Loader::NORMALIZED,
                                      message.GetTypeName())->New();
    
    // Only fields which are set in the message are processed
    std::vector< const FieldDescriptor * > setFields;
    message.GetReflection()->ListFields(message, &setFields);
    
    for (const auto field: setFields) {
        // Before anything else, the key must be found
        if (field->options().GetExtension(profanedb::protobuf::options).key()) {
            *messageTree.mutable_message()->mutable_key() = this->FieldToKey(message, field);
        }
        
        // TODO Maybe we should use another option to distinguish nested keyable messages,
        // and what about nested keyable messages contained in non-keyable messages?
        
        // Check whether in the normalizedPool the message is considered keyable
        const FieldDescriptor * normalizedField = normalizedMessage->GetDescriptor()->field(field->index());
        // HACK It seems Descriptor comparison doesn't work as they come from different Pools
        if (normalizedField->message_type() != NULL &&
            normalizedField->message_type()->full_name() == Key::descriptor()->full_name()) {
            const Message & nested = message.GetReflection()->GetMessage(message, field);
            const MessageTreeNode & nestedMessageTree = this->Marshal(nested);
            
            // The nested messages generated with recursion are added to this tree node,
            // because of recursion, the caller will eventually get back the whole tree,
            // with the parameter message as root
            *messageTree.add_children() = nestedMessageTree;
            
            // The field in the normalized message is assigned the root key from the contained message tree
            CopyMessage(nestedMessageTree.message().key(),
                              normalizedMessage->GetReflection()->MutableMessage(
                                normalizedMessage, normalizedField));
        }
        else {
            // Other fields are simply copied over,
            // we can't use Protobuf's own MergeFrom function
            // because it checks whether the two messages have the same descriptor.
            // We are pretty much sure at this point that we can copy the data,
            // because messages in normalizedPool differ from those in schemaPool
            // only on nested keyable messages, which we've already taken care of
            CopyField(field, message, normalizedMessage);
        }
    }
    
    // Now that normalizedMessage has been populated with the correct data,
    // actual marshalling (or serialization) occurs
    messageTree.mutable_message()->mutable_payload()->PackFrom(*normalizedMessage);
    
    return messageTree;
}

// TODO Should serialize differently... int(123) shouldn't become string("123")
Key Marshaller::FieldToKey(
    const Message & message,
    const FieldDescriptor * fd)
{
    Key key;
    *key.mutable_message_type() = message.GetTypeName();
    *key.mutable_field() = fd->name();
    
    const Reflection * reflection = message.GetReflection();
    
// TODO If concatenating strings, and they contain the separator, it might mess up,
// we need something much more stable than this
#define REP_SEP "$"
    if (fd->is_repeated()) {
        for (int y = 0; y < reflection->FieldSize(message, fd);  y++) {
            switch (fd->cpp_type()) {
                #define HANDLE_TYPE(CPPTYPE,  METHOD)                                                              \
                case FieldDescriptor::CPPTYPE_##CPPTYPE:                                                           \
                    *key.mutable_value() += REP_SEP + std::to_string(reflection->GetRepeated##METHOD(message, fd, y)); \
                    break;
                    
                    HANDLE_TYPE(INT32 , Int32 );
                    HANDLE_TYPE(INT64 , Int64 );
                    HANDLE_TYPE(UINT32, UInt32);
                    HANDLE_TYPE(UINT64, UInt64);
                    HANDLE_TYPE(DOUBLE, Double);
                    HANDLE_TYPE(FLOAT , Float );
                    HANDLE_TYPE(BOOL  , Bool  );
                    #undef HANDLE_TYPE
                    
                case FieldDescriptor::CPPTYPE_ENUM:
                    *key.mutable_value() += REP_SEP + std::to_string(reflection->GetRepeatedEnum(message, fd, y)->index());
                    break;
                case FieldDescriptor::CPPTYPE_STRING:
                    *key.mutable_value() += REP_SEP + reflection->GetRepeatedString(message, fd, y);
                    break;
                case FieldDescriptor::CPPTYPE_MESSAGE:
                    *key.mutable_value() += REP_SEP + reflection->GetRepeatedMessage(message, fd, y).SerializeAsString();
                    break;
            }
        }
    } else {
        switch (fd->cpp_type()) {
            #define HANDLE_TYPE(CPPTYPE, METHOD)                                             \
            case FieldDescriptor::CPPTYPE_##CPPTYPE:                                         \
                *key.mutable_value() = std::to_string(reflection->Get##METHOD(message, fd)); \
                break;
                
                HANDLE_TYPE(INT32 , Int32 );
                HANDLE_TYPE(INT64 , Int64 );
                HANDLE_TYPE(UINT32, UInt32);
                HANDLE_TYPE(UINT64, UInt64);
                HANDLE_TYPE(DOUBLE, Double);
                HANDLE_TYPE(FLOAT , Float );
                HANDLE_TYPE(BOOL  , Bool  );
                #undef HANDLE_TYPE
                
            case FieldDescriptor::CPPTYPE_ENUM:
                *key.mutable_value() = std::to_string(reflection->GetEnum(message, fd)->index());
                break;
            case FieldDescriptor::CPPTYPE_STRING:
                *key.mutable_value() = reflection->GetString(message, fd);
                break;
            case FieldDescriptor::CPPTYPE_MESSAGE:
                *key.mutable_value() = reflection->GetMessage(message, fd).SerializeAsString();
                break;
        }
    }
    
    return key;
}

}
}
}
