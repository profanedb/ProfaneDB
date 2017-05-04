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

#include "parser.h"

profanedb::storage::Parser::Parser(const Config::ProfaneDB & profaneConfig)
    : rootDescriptorDb(
        new google::protobuf::compiler::SourceTreeDescriptorDatabase(
    new RootSourceTree {profaneConfig.includePath, profaneConfig.profaneDbOptions}
    )
)
, schemaDescriptorDb(
    new google::protobuf::compiler::SourceTreeDescriptorDatabase(
        new RootSourceTree {profaneConfig.schemaDefinition}
    )
)
, mergedSchemaDescriptorDb(
    new google::protobuf::MergedDescriptorDatabase(rootDescriptorDb.get(), schemaDescriptorDb.get()))
, schemaPool(new DescriptorPool(mergedSchemaDescriptorDb.get()))
, normalizedDescriptorDb(new SimpleDescriptorDatabase)
{
    schemaDescriptorDb->RecordErrorsTo(&errCollector);

    // Load ProfaneDB options to be used during file import
    schemaPool->FindFileByName("profanedb/protobuf/options.proto");

    // TODO Move the whole loading to functions

    // Import all `.proto` files from the schema definition into the pool,
    // so that FindMessageTypeByName can then be used
    for (auto const & file:
            boost::filesystem::recursive_directory_iterator(
                profaneConfig.schemaDefinition,
                boost::filesystem::symlink_option::recurse)) {

        // Files are loaded by extension, so care should be taken to name them appropriately
        if (file.path().extension() == ".proto") {
            // For the pool every file is relative to the mapping provided before in config
            const FileDescriptor * fileD = schemaPool->FindFileByName(
                                               file.path().lexically_relative(profaneConfig.schemaDefinition).string());

            FileDescriptorProto * fileProto = new FileDescriptorProto;
            fileD->CopyTo(fileProto);

            for (int i = 0; i < fileD->message_type_count(); i++) {
                const Descriptor * message = fileD->message_type(i);
                
                this->ParseMessageDescriptor(*message, *fileProto->mutable_message_type(i));
            }
            normalizedDescriptorDb->AddAndOwn(fileProto);
        }
    }
}

void profanedb::storage::Parser::ParseMessageDescriptor(
    const google::protobuf::Descriptor & descriptor,
    google::protobuf::DescriptorProto & proto)
{
    // Recurse into nested messages
    for (int j = 0; j < descriptor.nested_type_count(); j++) {
        google::protobuf::DescriptorProto * nestedProto = proto.mutable_nested_type(j);
        this->ParseMessageDescriptor(*descriptor.nested_type(j), *nestedProto);
    }
    
    NormalizedDescriptor normalized(descriptor, proto);
    
    normalizedDescriptors.insert(
        std::pair<std::string, NormalizedDescriptor> (descriptor.full_name(), normalized));
}

profanedb::storage::Parser::NormalizedDescriptor::NormalizedDescriptor(
    const google::protobuf::Descriptor & descriptor,
    google::protobuf::DescriptorProto & proto)
{
    for (int k = 0; k < descriptor.field_count(); k++) {
        const FieldDescriptor * fd = descriptor.field(k);
        const Descriptor * nested = fd->message_type();

        // If a nested message is keyable change its type to string to allow references, 
        // and keyable messages are flagged for normalization
        if (nested != NULL && IsKeyable(*nested)) {
            proto.mutable_field(k)->set_type(FieldDescriptorProto_Type_TYPE_STRING);
            proto.mutable_field(k)->clear_type_name();
            
            this->keyableMessageReferences.insert(fd);
            
        } else if (fd->options().GetExtension(profanedb::protobuf::options).key()) {
            this->key = fd;
        }
    }
}

bool profanedb::storage::Parser::NormalizedDescriptor::IsKeyable(const google::protobuf::Descriptor& descriptor)
{
    for (int l = 0; l < descriptor.field_count(); l++) {
        if (descriptor.field(l)->options().GetExtension(profanedb::protobuf::options).key())
            return true;
    }

    return false;
}

const google::protobuf::FieldDescriptor & profanedb::storage::Parser::NormalizedDescriptor::GetKey() const
{
    return *this->key;
}

const std::set<const google::protobuf::FieldDescriptor *> & profanedb::storage::Parser::NormalizedDescriptor::GetKeyableReferences() const
{
    return this->keyableMessageReferences;
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
