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
using google::protobuf::compiler::SourceTreeDescriptorDatabase;
using google::protobuf::MergedDescriptorDatabase;
using google::protobuf::FileDescriptor;
using google::protobuf::FileDescriptorProto;
using google::protobuf::Descriptor;
using google::protobuf::DescriptorProto;
using google::protobuf::FieldDescriptor;
using google::protobuf::FieldDescriptorProto_Type;
using google::protobuf::DescriptorPool;
using google::protobuf::Message;

using profanedb::protobuf::Key;

namespace profanedb {
namespace format {
namespace protobuf {

Loader::RootSourceTree::RootSourceTree(std::vector<path> paths)
  : paths(paths)
{
    if (paths.size() == 0)
        throw std::runtime_error("Mapping is empty");
    
    for (const auto & path: paths) {
        this->MapPath("", path.string());
        
        BOOST_LOG_TRIVIAL(debug) << "Mapping " << path.string();
    }
    
    ZeroCopyInputStream * inputStream = this->Open("");
    if (inputStream == nullptr)
        throw std::runtime_error(this->GetLastErrorMessage());
}

Loader::Loader(
    std::unique_ptr<RootSourceTree> include,
    std::unique_ptr<RootSourceTree> schema)
  : includeSourceTree(std::move(include))
  , schemaSourceTree(std::move(schema))
  , includeDb(includeSourceTree.get())
  , schemaDb(schemaSourceTree.get())
  , schemaPool(new MergedDescriptorDatabase(&includeDb, &schemaDb), &errorCollector)
  , normalizedPool(new MergedDescriptorDatabase(&includeDb, &normalizedDescriptorDb), &errorCollector)
{
     // profanedb.protobuf.options.key is defined in here
     // It is used to mark the primary key on Messages
//      const FileDescriptor * optionsFile = schemaPool.FindFileByName("profanedb/protobuf/options.proto");
//      FileDescriptorProto optionsProto;
//      optionsFile->CopyTo(&optionsProto);
//      normalizedDescriptorDb.AddAndOwn(&optionsProto);

//      BOOST_LOG_TRIVIAL(debug) << "Loading profanedb/protobuf/options.proto and copying to normalized descriptor db";

    // Just in case schema is defined in more than one place
    for (const auto & path: schemaSourceTree->paths) {
        
        BOOST_LOG_TRIVIAL(debug) << "Loading schema at " << path.string();
        
        // Iterate through all files in that mapped path
        for (const auto & file: recursive_directory_iterator(path, symlink_option::recurse)) {
            
            // Only consider files ending in .proto
            // TODO This might be configured differently
            if (file.path().extension() == ".proto") {
                
                // The file is now retrieved, and its path for Protobuf must be relative to the mapping
                // it's parsed, normalized (nested keyable messages are removed)
                FileDescriptorProto * normalizedProto = this->ParseFile(
                      schemaPool.FindFileByName(file.path().lexically_relative(path).string()));
                
                BOOST_LOG_TRIVIAL(debug) << "Adding normalized proto " << normalizedProto->name();
                BOOST_LOG_TRIVIAL(trace) << std::endl << normalizedProto->DebugString();
                // The normalizedDescriptorDb keeps these new Descriptors
                normalizedDescriptorDb.AddAndOwn(normalizedProto);
            }
        }
    }
}

FileDescriptorProto * Loader::ParseFile(
    const FileDescriptor * fileDescriptor)
{
    BOOST_LOG_TRIVIAL(debug) << "Parsing file " << fileDescriptor->name();
    // BOOST_LOG_TRIVIAL(trace) << fileDescriptor->DebugString(); // Redundant, is done for each message later

    // A FileDescriptorProto is needed to edit messages and populate the normalized descriptor database
    FileDescriptorProto * normFileDescProto = new FileDescriptorProto;
    fileDescriptor->CopyTo(normFileDescProto);
                
    // storage.proto dependency must be added
    // as profanedb.protobuf.Key is used in normalized messages
    *normFileDescProto->add_dependency() = "profanedb/protobuf/storage.proto";
    
    // For each message in the file...
    for (int i = 0; i < fileDescriptor->message_type_count(); i++) {
        // ... parse it, make nested messages of type profanedb.protobuf.Key
        *normFileDescProto->mutable_message_type(i) = *this->ParseAndNormalizeDescriptor(fileDescriptor->message_type(i));
    }
    
    return normFileDescProto;
}

DescriptorProto * Loader::ParseAndNormalizeDescriptor(
    const Descriptor * descriptor)
{
    BOOST_LOG_TRIVIAL(debug) << "Parsing descriptor " << descriptor->full_name();
    BOOST_LOG_TRIVIAL(trace) << std::endl << descriptor->DebugString();
    
    DescriptorProto * normDescProto = new DescriptorProto;
    descriptor->CopyTo(normDescProto);
    
    // Recurse for all messages DEFINED within this message
    for (int j = 0; j < descriptor->nested_type_count(); j++) {
        *normDescProto->mutable_nested_type(j) = *this->ParseAndNormalizeDescriptor(descriptor->nested_type(j));
    }
        
    // Now the actual Descriptor normalization
    for (int k = 0; k < descriptor->field_count(); k++) {
        const FieldDescriptor * field = descriptor->field(k);
        
        // NULL if not a message
        const Descriptor * nestedMessage = field->message_type();
        
        // If this field is effectively a message,
        // and that Message is keyable...
        if(nestedMessage != NULL && this->IsKeyable(nestedMessage)) {
            // ... make the field in the normalized descriptor a Key object
            // TODO Maybe we could use an option here as well
            normDescProto->mutable_field(k)->set_type(
                FieldDescriptorProto_Type::FieldDescriptorProto_Type_TYPE_MESSAGE); // Redundant, is message already
            normDescProto->mutable_field(k)->set_type_name(Key::descriptor()->full_name()); // TODO Should include Key in normalizedPool
            
            BOOST_LOG_TRIVIAL(trace) << "Message " << descriptor->name()
                                     << " has keyable nested message " << field->name()
                                     << " of type " << nestedMessage->name();
        }
    }
    
    return normDescProto;
}

bool Loader::IsKeyable(const Descriptor * descriptor) const
{
    for (int i = 0; i < descriptor->field_count(); i++) {
        // If any field in message has profanedb::protobuf::options::key set
        if (descriptor->field(i)->options().GetExtension(profanedb::protobuf::options).key())
            return true;
    }
    return false;
}

const DescriptorPool & Loader::GetPool(PoolType poolType) const
{
    return (poolType == SCHEMA) ? this->schemaPool : this->normalizedPool;
}

const Descriptor * Loader::GetDescriptor(PoolType poolType, std::string typeName) const
{
    BOOST_LOG_TRIVIAL(debug) << "Getting descriptor "
                             << typeName
                             << " from "
                             << ((poolType == SCHEMA)
                                ? "SCHEMA"
                                : "NORMALIZED")
                             << " pool";

    const Descriptor * descriptor = this->GetPool(poolType).FindMessageTypeByName(typeName);

    if (descriptor == NULL)
        throw std::runtime_error(typeName + " doesn't exist");

    BOOST_LOG_TRIVIAL(trace) << std::endl << descriptor->DebugString();

    return descriptor;
}

const Message * Loader::CreateMessage(PoolType poolType, std::string typeName)
{
    BOOST_LOG_TRIVIAL(debug) << "Creating message " << typeName;

    return this->messageFactory.GetPrototype(
          this->GetDescriptor(poolType, typeName));
}

void Loader::BoostLogErrorCollector::AddError(
    const std::string & filename,
    const std::string & element_name,
    const Message * descriptor,
    DescriptorPool::ErrorCollector::ErrorLocation location,
    const std::string & message)
{
    BOOST_LOG_TRIVIAL(error) << message;
}

void Loader::BoostLogErrorCollector::AddWarning(
    const std::string & filename,
    const std::string & element_name,
    const Message * descriptor,
    DescriptorPool::ErrorCollector::ErrorLocation location,
    const std::string & message)
{
    BOOST_LOG_TRIVIAL(warning) << message;
}

}
}
}
