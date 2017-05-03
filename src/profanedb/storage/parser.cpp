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

#include "parser.h"

profanedb::storage::Parser::Parser(const Config::ProfaneDB & profaneConfig)
  : descriptorDb(new compiler::SourceTreeDescriptorDatabase(profaneConfig.GetSourceTree().get()))
  , pool(new DescriptorPool(descriptorDb.get()))
{
    descriptorDb->RecordErrorsTo(&errCollector);

    // Load ProfaneDB options to be used during file import
    pool->FindFileByName("profanedb/protobuf/options.proto");
    
    // Import all `.proto` files in kDbSchema into the pool,
    // so that FindMessageTypeByName can then be used
    for (auto const & file:
        boost::filesystem::recursive_directory_iterator(
            profaneConfig.GetSchemaDefinitionPath(),
            boost::filesystem::symlink_option::recurse)) {
        
        if (file.path().extension() == ".proto") {
            // For the pool every file is relative to the mapping provided before (kDbSchema)
            const FileDescriptor * fileD = pool->FindFileByName(
                file.path().lexically_relative(profaneConfig.GetSchemaDefinitionPath()).string());
            
            for (int i = 0; i < fileD->message_type_count(); i++) {
                const Descriptor * message = fileD->message_type(i);
                
                DescriptorProto * descriptorProto;
               
                if (ParseMessageDescriptor(*message)) {
                    descriptorProto = fileDescProto.add_message_type();
                    message->CopyTo(descriptorProto);
                }
            }
        }
    }
    
    // Now build a file with all generated messages in an empty pool
    DescriptorPool newPool{};
    *fileDescProto.mutable_name() = "profanedb_generated_schema";
    std::cout << fileDescProto.DebugString() << std::endl;
    newPool.BuildFile(fileDescProto);
}

profanedb::storage::Parser::~Parser()
{
}

bool profanedb::storage::Parser::ParseMessageDescriptor(const google::protobuf::Descriptor & descriptor)
{
    // A DescriptorProto is needed to generate the message the way it will be serialized,
    // with keys replacing
    google::protobuf::DescriptorProto * descriptorProto;
    
    bool hasKey = false;
    
    for (int k = 0; k < descriptor.field_count(); k++) {
        const FieldDescriptor * field = descriptor.field(k);
        
        // Recursively call this function to add all nested messages
        const google::protobuf::Descriptor * nested = field->message_type();
        if (nested != NULL) {
            if (ParseMessageDescriptor(*nested)) {
                // If the nested message has a primary key, set the field to string to hold a reference
                descriptorProto->mutable_field(k)->set_type(FieldDescriptorProto_Type_TYPE_STRING);
            }
        }

        profanedb::protobuf::FieldOptions options = field->options().GetExtension(profanedb::protobuf::options);

        if (options.key()) {
            hasKey = true;
        }
    }
    
    return hasKey;
}

map< std::string, const google::protobuf::Message & > profanedb::storage::Parser::NormalizeMessage(const google::protobuf::Any & serializable)
{
    // The Descriptor is manually extracted from the pool,
    // removing the prepending `type.googleapis.com/` in the Any message
    string type = serializable.type_url();
    const Descriptor * definition = pool->FindMessageTypeByName(type.substr(type.rfind('/')+1, string::npos));
    
    Message * container = messageFactory.GetPrototype(definition)->New();
    serializable.UnpackTo(container);
    
    return NormalizeMessage(*container);
}

map< std::string, const google::protobuf::Message & > profanedb::storage::Parser::NormalizeMessage(const google::protobuf::Message & message)
{
    auto dependencies = new map< std::string, const google::protobuf::Message & >();
    
    // TODO This only takes set fields into account. Maybe using Descriptor::field(0 <= i < field_count()) is better
    auto fields = new std::vector< const FieldDescriptor * >();
    message.GetReflection()->ListFields(message, fields);
    
    DescriptorProto * descProto = new DescriptorProto();
    message.GetDescriptor()->CopyTo(descProto);
    
    string key;
    
    for (auto const & fd: *fields) {
        if (fd->message_type() != NULL) {
            auto nested = NormalizeMessage(message.GetReflection()->GetMessage(message, fd, &messageFactory));
            
            // TODO If first nested message has primary key set a reference
            if (nested.size() > 0) {
                
                // The nested message might contain other messages, all of them are stored in the dependency map
                dependencies->insert(nested.begin(), nested.end());
            }
        } else {
            auto options = fd->options().GetExtension(profanedb::protobuf::options);
            
            // TODO This only uses a single key, one could set multiple keys
            if (options.key()) {
                key = FieldToKey(message, *fd);
            }
        }
    }
    
    dependencies->insert( std::pair< string, const google::protobuf::Message & >(key, message) );
    
    return *dependencies;
}

string profanedb::storage::Parser::FieldToKey(const google::protobuf::Message & container, const google::protobuf::FieldDescriptor & fd)
{
    const Reflection * reflection = container.GetReflection();
    
    string key_value;
    
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
            key_value = reflection->GetMessage(container, &fd, &messageFactory).SerializeAsString();
            break;
    }
    
    return fd.full_name() + '$' + key_value;
}

profanedb::storage::Parser::ErrorCollector::ErrorCollector()
{
}

void profanedb::storage::Parser::ErrorCollector::AddError(const string & filename, int line, int column, const string & message)
{
    if (line == -1) { // Entire file error
        std::cerr << filename << " error: " << message << "\n";
    } else {
        std::cerr << filename << " " << line+1 << ":" << column+1 << " error: " << message << "\n";
    }
}

void profanedb::storage::Parser::ErrorCollector::AddWarning(const string & filename, int line, int column, const string & message)
{
    std::cerr << filename << " " << line+1 << ":" << column+1 << " warning: " << message << "\n";
}
