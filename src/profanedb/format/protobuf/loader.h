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

#ifndef PROFANEDB_FORMAT_PROTOBUF_LOADER_H
#define PROFANEDB_FORMAT_PROTOBUF_LOADER_H

#include <profanedb/protobuf/options.pb.h>
#include <profanedb/protobuf/storage.pb.h>

#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/importer.h>

#include <boost/filesystem.hpp>
#include <boost/log/trivial.hpp>

namespace profanedb {
namespace format {
namespace protobuf {

// Loader is a utility class to populate a schemaPool and normalizedPool
// which profanedb::format::protobuf::Marshaller requires
class Loader
{
public:
    // RootSourceTree is a utility class that creates a DiskSourceTree,
    // mapping all the paths provided to the root ("/") path for easier import.
    // Paths are available for Loader to populate its Pool;
    class RootSourceTree : public google::protobuf::compiler::DiskSourceTree {
        friend class Loader;
        
    public:
        RootSourceTree(std::initializer_list<boost::filesystem::path> paths);
        
    private:
        std::vector<boost::filesystem::path> paths;
    };
    
    // include has the import directories
    // ("/usr/include/[google/protobuf/*.proto]", "src/[profanedb/protobuf/*.proto]")
    // schema the user proto files, with ProfaneDB options set
    Loader(
        std::unique_ptr<RootSourceTree> include,
        std::unique_ptr<RootSourceTree> schema);
    
    const google::protobuf::DescriptorPool & GetSchemaPool() const;
    const google::protobuf::DescriptorPool & GetNormalizedPool() const;
    
private:
    // Given a Protobuf FileDescriptor from the pool, parse all of its messages,
    // find the keyable messages, and return a FileDescriptorProto,
    // ready to be put in the normalizedDescriptorDb
    google::protobuf::FileDescriptorProto ParseFile(
        const google::protobuf::FileDescriptor * fileDescriptor);
    
    // Parse a Descriptor and its nested messages
    google::protobuf::DescriptorProto ParseAndNormalizeDescriptor(
        const google::protobuf::Descriptor * descriptor);
    
    // Check whether a Descriptor has a field with key option set
    bool IsKeyable(const google::protobuf::Descriptor * descriptor) const;
    
    class BoostLogErrorCollector
        : public google::protobuf::DescriptorPool::ErrorCollector {
      virtual void AddError(const std::string & filename,
                            const std::string & element_name,
                            const google::protobuf::Message * descriptor,
                            google::protobuf::DescriptorPool::ErrorCollector::ErrorLocation location,
                            const std::string & message) override;

      virtual void AddWarning(const std::string & filename,
                              const std::string & element_name,
                              const google::protobuf::Message * descriptor,
                              google::protobuf::DescriptorPool::ErrorCollector::ErrorLocation location,
                              const std::string & message) override;
    };
//    google::protobuf::DescriptorPool::ErrorCollector & errorCollector;
    BoostLogErrorCollector errorCollector;

    std::unique_ptr<RootSourceTree> includeSourceTree;
    std::unique_ptr<RootSourceTree> schemaSourceTree;
    
    google::protobuf::compiler::SourceTreeDescriptorDatabase includeDb;
    google::protobuf::compiler::SourceTreeDescriptorDatabase schemaDb;
    
    google::protobuf::SimpleDescriptorDatabase normalizedDescriptorDb;
    
    // schemaPool keeps track of the original messages
    google::protobuf::DescriptorPool schemaPool;
    
    // For each keyable message in schema, there is a normalized version
    // which has Key in place of nested keyable messages
    google::protobuf::DescriptorPool normalizedPool;
};
}
}
}

#endif // PROFANEDB_FORMAT_PROTOBUF_LOADER_H
