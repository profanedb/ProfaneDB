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
    std::shared_ptr<Storage> storage,
    std::shared_ptr<Loader> loader)
  : loader(loader)
  , storage(storage)
{
}

MessageTreeNode Marshaller::Marshal(const Message & message)
{
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
        if (normalizedField->message_type() == Key::descriptor()) {
            const Message & nested = message.GetReflection()->GetMessage(message, field);
            const MessageTreeNode & nestedMessageTree = this->Marshal(nested);
            
            // The nested messages generated with recursion are added to this tree node,
            // because of recursion, the caller will eventually get back the whole tree,
            // with the parameter message as root
            *messageTree.add_children() = nestedMessageTree;
            
            // The field in the normalized message is assigned the root key from the contained message tree
            normalizedMessage->GetReflection()
                ->MutableMessage(normalizedMessage, normalizedField)
                ->MergeFrom(nestedMessageTree.message().key());
        }
        else {
            // Other fields are simply copied over,
            // we can't use Protobuf's own MergeFrom function
            // because it checks whether the two messages have the same descriptor.
            // We are pretty much sure at this point that we can copy the data,
            // because messages in normalizedPool differ from those in schemaPool
            // only on nested keyable messages, which we've already taken care of
            this->CopyField(field, message, normalizedMessage);
        }
    }
    
    // Now that normalizedMessage has been populated with the correct data,
    // actual marshalling (or serialization) occurs
    normalizedMessage->SerializeToString(messageTree.mutable_message()->mutable_payload());
    
    return messageTree;
}

const Message & Marshaller::Unmarshal(const StorableMessage & storable)
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
    normalizedMessage->ParseFromString(storable.payload());
    
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
            originalMessage->GetReflection()
                ->MutableMessage(originalMessage, originalField)
                ->MergeFrom(nestedMessage);
        }
        else { // if field is not a reference
            
            // Just like in Marshal, other fields are simply copied over,
            // as normalized and original descriptors look the same except for nested keyable messages
            this->CopyField(normalizedField, *normalizedMessage, originalMessage);
        }
    }
        
    
    return *originalMessage;
}

void Marshaller::CopyField(
    const FieldDescriptor * fromField,
    const Message & from,
    Message * to)
{
    const Reflection * fromReflection = from.GetReflection();
    const Reflection * toReflection = to->GetReflection();
    const FieldDescriptor * toField = to->GetDescriptor()->field(fromField->index());
    
    if (fromField->is_repeated()) {
        for (int y = 0; y < fromReflection->FieldSize(from, fromField); y++) {
            switch (fromField->cpp_type()) {
                #define HANDLE_TYPE(CPPTYPE, METHOD)                          \
                case FieldDescriptor::CPPTYPE_##CPPTYPE:                      \
                    toReflection->Add##METHOD(to, toField,                    \
                    fromReflection->GetRepeated##METHOD(from, fromField, y)); \
                    break;
                    
                    HANDLE_TYPE(INT32 , Int32 );
                    HANDLE_TYPE(INT64 , Int64 );
                    HANDLE_TYPE(UINT32, UInt32);
                    HANDLE_TYPE(UINT64, UInt64);
                    HANDLE_TYPE(DOUBLE, Double);
                    HANDLE_TYPE(FLOAT , Float );
                    HANDLE_TYPE(BOOL  , Bool  );
                    HANDLE_TYPE(STRING, String);
                    HANDLE_TYPE(ENUM  , Enum  );
                    #undef HANDLE_TYPE
                    
                case FieldDescriptor::CPPTYPE_MESSAGE:
                    toReflection->AddMessage(to, toField)->MergeFrom(
                        fromReflection->GetRepeatedMessage(from, fromField, y));
                    break;
            }
        }
    } else {
        switch (fromField->cpp_type()) {
            #define HANDLE_TYPE(CPPTYPE, METHOD)               \
            case FieldDescriptor::CPPTYPE_##CPPTYPE:           \
                toReflection->Set##METHOD(to, toField,         \
                fromReflection->Get##METHOD(from, fromField)); \
                break;
                
                HANDLE_TYPE(INT32 , Int32 );
                HANDLE_TYPE(INT64 , Int64 );
                HANDLE_TYPE(UINT32, UInt32);
                HANDLE_TYPE(UINT64, UInt64);
                HANDLE_TYPE(DOUBLE, Double);
                HANDLE_TYPE(FLOAT , Float );
                HANDLE_TYPE(BOOL  , Bool  );
                HANDLE_TYPE(STRING, String);
                HANDLE_TYPE(ENUM  , Enum  );
                #undef HANDLE_TYPE
                
            case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                toReflection->MutableMessage(to, toField)->MergeFrom(
                    fromReflection->GetMessage(from, fromField));
                break;
        }
    }
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
    
    if (fd->is_repeated()) {
        for (int y = 0; y < reflection->FieldSize(message, fd);  y++) {
            switch (fd->cpp_type()) {
                #define HANDLE_TYPE(CPPTYPE,  METHOD)                                                              \
                case FieldDescriptor::CPPTYPE_##CPPTYPE:                                                           \
                    *key.mutable_value() += "$" + std::to_string(reflection->GetRepeated##METHOD(message, fd, y)); \
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
                    *key.mutable_value() += "$" + std::to_string(reflection->GetRepeatedEnum(message, fd, y)->index());
                    break;
                case FieldDescriptor::CPPTYPE_STRING:
                    *key.mutable_value() += "$" + reflection->GetRepeatedString(message, fd, y);
                    break;
                case FieldDescriptor::CPPTYPE_MESSAGE:
                    *key.mutable_value() += "$" + reflection->GetRepeatedMessage(message, fd, y).SerializeAsString();
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
