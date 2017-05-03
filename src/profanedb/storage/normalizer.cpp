/*
 * ProfaneDB - A Protocol Buffer database.
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

std::map<std::string, const google::protobuf::Message> profanedb::storage::Normalizer::NormalizeMessage(
    const google::protobuf::Any & serializable)
{
    // Any messages have a type url beginning with `type.googleapis.com/`, this is stripped
    std::string type = serializable.type_url();
    auto definition = std::unique_ptr<const google::protobuf::Descriptor>(schemaPool->FindMessageTypeByName(type.substr(type.rfind('/')+1, std::string::npos)));
    
    // Having the definition our message factory can simply generate a container,
    auto container = std::unique_ptr<google::protobuf::Message>(messageFactory.GetPrototype(definition.get())->New());
    
    // and convert the bytes coming from Any into it
    serializable.UnpackTo(container.get());
    
    // The method getting a Message as paramater does the actual normalization of data
    return this->NormalizeMessage(*container);
}

std::map<std::string, const google::protobuf::Message> profanedb::storage::Normalizer::NormalizeMessage(
    const google::protobuf::Message & message) const
{
    auto dependencies = std::map<std::string, const google::protobuf::Message>();
    Parser::NormalizedDescriptor & normalizedDesc = parser.normalizedDescriptors.at(message.GetDescriptor()->full_name());
    
    dependencies.insert(std::pair<std::string, const google::protobuf::Message>(
        FieldToKey(message, normalizedDesc.GetKey()),
        message));
    
    return dependencies;
}

std::string profanedb::storage::Normalizer::FieldToKey(const google::protobuf::Message & container, const google::protobuf::FieldDescriptor & fd) const
{
    const Reflection * reflection = container.GetReflection();

    std::string key_value;

    switch (fd.cpp_type()) {
        case FieldDescriptor::CPPTYPE_INT32:
            key_value = std::to_string(reflection->GetInt32(container, &fd));
            break;
        case FieldDescriptor::CPPTYPE_INT64:
            key_value = std::to_string(reflection->GetInt64(container, &fd));
            break;
        case FieldDescriptor::CPPTYPE_UINT32:
            key_value = std::to_string(reflection->GetUInt32(container, &fd));
            break;
        case FieldDescriptor::CPPTYPE_UINT64:
            key_value = std::to_string(reflection->GetUInt64(container, &fd));
            break;
        case FieldDescriptor::CPPTYPE_DOUBLE:
            key_value = std::to_string(reflection->GetDouble(container, &fd));
            break;
        case FieldDescriptor::CPPTYPE_FLOAT:
            key_value = std::to_string(reflection->GetFloat(container, &fd));
            break;
        case FieldDescriptor::CPPTYPE_BOOL:
            key_value = std::to_string(reflection->GetBool(container, &fd));
            break;
        case FieldDescriptor::CPPTYPE_ENUM:
            key_value = std::to_string(reflection->GetEnum(container, &fd)->index());
            break;
        case FieldDescriptor::CPPTYPE_STRING:
            key_value = reflection->GetString(container, &fd);
            break;
        case FieldDescriptor::CPPTYPE_MESSAGE:
            key_value = reflection->GetMessage(container, &fd/*, &messageFactory*/).SerializeAsString();
            break;
    }

    return fd.full_name() + '$' + key_value;
}
