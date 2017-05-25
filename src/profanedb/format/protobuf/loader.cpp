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

#include "loader.h"

using boost::filesystem::path;
using boost::filesystem::recursive_directory_iterator;
using boost::filesystem::symlink_option;

using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::MergedDescriptorDatabase;
using google::protobuf::compiler::SourceTreeDescriptorDatabase;
using google::protobuf::FileDescriptor;
using google::protobuf::FileDescriptorProto;
using google::protobuf::Descriptor;
using google::protobuf::DescriptorProto;
using google::protobuf::FieldDescriptor;
using google::protobuf::FieldDescriptorProto_Type;

using profanedb::protobuf::Key;

profanedb::format::protobuf::Loader::RootSourceTree::RootSourceTree(std::initializer_list<path> paths)
{
    for (const auto & path: paths)
        this->MapPath("", path.string());
    
    ZeroCopyInputStream * inputStream = this->Open("");
    if (inputStream == nullptr)
        throw std::runtime_error(this->GetLastErrorMessage());
}

profanedb::format::protobuf::Loader::Loader(
    std::unique_ptr<RootSourceTree> include,
    std::unique_ptr<RootSourceTree> schema)
  : includeSourceTree(std::move(include))
  , schemaSourceTree(std::move(schema))
  , includeDb(includeSourceTree.get())
  , schemaDb(schemaSourceTree.get())
  , schemaPool(new MergedDescriptorDatabase(&includeDb, &schemaDb))
  , normalizedPool(&normalizedDescriptorDb)
{
    // profanedb.protobuf.options.key is defined in here
    // It is used to mark the primary key on Messages
    schemaPool.FindFileByName("profanedb/protobuf/options.proto");
    
    // Just in case schema is defined in more than one place
    for (const auto & path: schemaSourceTree->paths) {
        
        // Iterate through all files in that mapped path
        for (const auto & file: recursive_directory_iterator(path, symlink_option::recurse)) {
            
            // Only consider files ending in .proto
            // TODO This might be configured differently
            if (file.path().extension() == ".proto") {
                // The file is now retrieved, and its path for Protobuf must be relative to the mapping
                this->ParseFile(schemaPool.FindFileByName(file.path().lexically_relative(path).string()));
            }
        }
    }
}

FileDescriptorProto profanedb::format::protobuf::Loader::ParseFile(
    const FileDescriptor * fileDescriptor)
{
    // A FileDescriptorProto is needed to edit messages and populate the normalized descriptor database
    FileDescriptorProto normFileDescProto;
    fileDescriptor->CopyTo(&normFileDescProto);
    
    // For each message in the file...
    for (int i = 0; i < fileDescriptor->message_type_count(); i++) {
        // ... parse it, make nested messages Key objects
        *normFileDescProto.mutable_message_type(i) = this->ParseAndNormalizeDescriptor(fileDescriptor->message_type(i));
    }
    
    return normFileDescProto;
}

DescriptorProto profanedb::format::protobuf::Loader::ParseAndNormalizeDescriptor(
    const Descriptor * descriptor)
{
    DescriptorProto normDescProto;
    descriptor->CopyTo(&normDescProto);
    
    // Recurse for all messages DEFINED within this message
    for (int j = 0; j < descriptor->nested_type_count(); j++) {
        *normDescProto.mutable_nested_type(j) = this->ParseAndNormalizeDescriptor(descriptor->nested_type(j));
    }
        
    // Now the actual Descriptor normalization
    for (int k = 0; k < descriptor->field_count(); k++) {
        const FieldDescriptor * field = descriptor->field(k);
        
        // NULL if not a message
        const Descriptor * nestedMessage = field->message_type();
        
        // If this field is effectively a message,
        // and that Message is keyable...
        if(nestedMessage != nullptr && this->IsKeyable(nestedMessage)) {
            // ... make the field in the normalized descriptor a Key object
            // TODO Maybe we could use an option here as well
            normDescProto.mutable_field(k)->set_type(
                FieldDescriptorProto_Type::FieldDescriptorProto_Type_TYPE_MESSAGE); // Redundant, is message already
            normDescProto.mutable_field(k)->set_type_name(Key::descriptor()->full_name()); // TODO Should include Key in normalizedPool
        }
    }
    
    return normDescProto;
}

bool profanedb::format::protobuf::Loader::IsKeyable(const Descriptor * descriptor) const
{
    for (int i = 0; i < descriptor->field_count(); i++) {
        // If any field in message has profanedb::protobuf::options::key set
        if (descriptor->field(i)->options().GetExtension(profanedb::protobuf::options).key())
            return true;
    }
    return false;
}

const google::protobuf::DescriptorPool & profanedb::format::protobuf::Loader::GetSchemaPool() const
{
    return this->schemaPool;
}

const google::protobuf::DescriptorPool & profanedb::format::protobuf::Loader::GetNormalizedPool() const
{
    return this->normalizedPool;
}

