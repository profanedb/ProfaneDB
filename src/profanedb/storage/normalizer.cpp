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

#include "normalizer.h"

profanedb::storage::Normalizer::Normalizer(Parser & parser)
  : parser(parser)
  , schemaPool(parser.schemaPool)
  , normalizedMergedDescriptorDb(
      new google::protobuf::MergedDescriptorDatabase(parser.rootDescriptorDb.get(), parser.normalizedDescriptorDb.get()))
  , normalizedPool(new google::protobuf::DescriptorPool(normalizedMergedDescriptorDb.get()))
{
}

std::map<std::string, const google::protobuf::Message &> profanedb::storage::Normalizer::NormalizeMessage(
    const google::protobuf::Any & serializable)
{
    // Any messages have a type url beginning with `type.googleapis.com/`, this is stripped
    std::string type = serializable.type_url();
    const google::protobuf::Descriptor * definition = schemaPool->FindMessageTypeByName(type.substr(type.rfind('/')+1, std::string::npos));
    
    // Having the definition our message factory can simply generate a container,
    auto container = std::shared_ptr<google::protobuf::Message>(messageFactory.GetPrototype(definition)->New());
    
    // and convert the bytes coming from Any into it
    serializable.UnpackTo(container.get());
    
    // The method getting a Message as paramater does the actual normalization of data
    return this->NormalizeMessage(*container);
}

std::map<std::string, const google::protobuf::Message &> profanedb::storage::Normalizer::NormalizeMessage(const google::protobuf::Message & message)
{
    auto dependencies = std::map<std::string, const google::protobuf::Message &>();
    Parser::NormalizedDescriptor & normalizedDesc = parser.normalizedDescriptors.at(message.GetTypeName());
    
    google::protobuf::Message * normalizedMessage = messageFactory.GetPrototype(normalizedPool->FindMessageTypeByName(message.GetTypeName()))->New();
    
    std::vector< const google::protobuf::FieldDescriptor * > setFields;
    message.GetReflection()->ListFields(message, &setFields);
    
    for (const auto & fd: setFields) {
        if (normalizedDesc.GetKeyableReferences().find(fd) != normalizedDesc.GetKeyableReferences().end()) {
            const google::protobuf::Message & nestedMessage = message.GetReflection()->GetMessage(message, fd, &messageFactory);
            
            auto nestedDependencies = this->NormalizeMessage(nestedMessage);
            dependencies.insert(nestedDependencies.begin(), nestedDependencies.end());
            
            const google::protobuf::FieldDescriptor * referenceFieldDescriptor = normalizedMessage->GetDescriptor()->field(fd->index());
            
            Parser::NormalizedDescriptor & nestedNormalizedDesc = parser.normalizedDescriptors.at(nestedMessage.GetTypeName());
            normalizedMessage->GetReflection()->SetString(
                normalizedMessage,
                referenceFieldDescriptor,
                this->FieldToKey(nestedMessage, nestedNormalizedDesc.GetKey()));
            // TODO If we made NormalizeMessage return the top level key in a clear way we'd avoid all this
        }
        else {
            this->CopyField(*fd, message, *normalizedMessage);
        }
    }
    
    dependencies.insert(std::pair<std::string, const google::protobuf::Message &>(
        this->FieldToKey(message, normalizedDesc.GetKey()),
        *normalizedMessage));
    
    return dependencies;
}

std::string profanedb::storage::Normalizer::FieldToKey(const google::protobuf::Message & container, const google::protobuf::FieldDescriptor & fd) const
{
    const google::protobuf::Reflection * reflection = container.GetReflection();

    std::string key_value;
    
    if (fd.is_repeated()) {
        for (int y = 0; y < reflection->FieldSize(container, &fd);  y++) {
            switch (fd.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE,  METHOD)                                                                      \
                case google::protobuf::FieldDescriptor::CPPTYPE_##CPPTYPE:                                 \
                    key_value += "$" + std::to_string(reflection->GetRepeated##METHOD(container, &fd, y)); \
                    break;
                
                HANDLE_TYPE(INT32 , Int32 );
                HANDLE_TYPE(INT64 , Int64 );
                HANDLE_TYPE(UINT32, UInt32);
                HANDLE_TYPE(UINT64, UInt64);
                HANDLE_TYPE(DOUBLE, Double);
                HANDLE_TYPE(FLOAT , Float );
                HANDLE_TYPE(BOOL  , Bool  );
#undef HANDLE_TYPE

                case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
                key_value += "$" + std::to_string(reflection->GetRepeatedEnum(container, &fd, y)->index());
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
                key_value += "$" + reflection->GetRepeatedString(container, &fd, y);
                    break;
                case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
                key_value += "$" + reflection->GetRepeatedMessage(container, &fd, y).SerializeAsString();
                    break;
            }
        }
    } else {
        switch (fd.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD)                                                   \
        case google::protobuf::FieldDescriptor::CPPTYPE_##CPPTYPE:               	   \
            key_value = "$" + std::to_string(reflection->Get##METHOD(container, &fd)); \
            break;

            HANDLE_TYPE(INT32 , Int32 );
            HANDLE_TYPE(INT64 , Int64 );
            HANDLE_TYPE(UINT32, UInt32);
            HANDLE_TYPE(UINT64, UInt64);
            HANDLE_TYPE(DOUBLE, Double);
            HANDLE_TYPE(FLOAT , Float );
            HANDLE_TYPE(BOOL  , Bool  );
#undef HANDLE_TYPE

        case google::protobuf::FieldDescriptor::CPPTYPE_ENUM:
            key_value = "$" + std::to_string(reflection->GetEnum(container, &fd)->index());
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_STRING:
            key_value = "$" + reflection->GetString(container, &fd);
            break;
        case google::protobuf::FieldDescriptor::CPPTYPE_MESSAGE:
            key_value = "$" + reflection->GetMessage(container, &fd/*, &messageFactory*/).SerializeAsString();
            break;
        }
    }

    return fd.full_name() + key_value;
}

void profanedb::storage::Normalizer::CopyField(
    const google::protobuf::FieldDescriptor & fromField,
    const google::protobuf::Message & from,
    google::protobuf::Message & to)
{
    const google::protobuf::Reflection * fromReflection = from.GetReflection();
    const google::protobuf::Reflection * toReflection = to.GetReflection();
    const google::protobuf::FieldDescriptor * toField = to.GetDescriptor()->field(fromField.index());
    
    if (fromField.is_repeated()) {
        for (int y = 0; y < fromReflection->FieldSize(from, &fromField); y++) {
            switch (fromField.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD)                                               \
                case google::protobuf::FieldDescriptor::CPPTYPE_##CPPTYPE:         \
                    toReflection->Add##METHOD(&to, toField,                        \
                        fromReflection->GetRepeated##METHOD(from, &fromField, y)); \
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
                    toReflection->AddMessage(&to, toField)->MergeFrom(
                        fromReflection->GetRepeatedMessage(from, &fromField, y));
                    break;
            }
        }
    } else {
        switch (fromField.cpp_type()) {
#define HANDLE_TYPE(CPPTYPE, METHOD)                                   \
            case google::protobuf::FieldDescriptor::CPPTYPE_##CPPTYPE: \
                toReflection->Set##METHOD(&to, toField,                \
                    fromReflection->Get##METHOD(from, &fromField));    \
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
                toReflection->MutableMessage(&to, toField)->MergeFrom(
                    fromReflection->GetMessage(from, &fromField));
                break;
        }
    }

}
