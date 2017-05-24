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

#ifndef PROFANEDB_STORAGE_LOADER_H
#define PROFANEDB_STORAGE_LOADER_H

#include <memory>

#include <boost/filesystem.hpp>

#include "parser.h"

namespace profanedb {
namespace storage {

using boost::filesystem::path;
using boost::filesystem::recursive_directory_iterator;
using boost::filesystem::symlink_option;

using google::protobuf::DescriptorDatabase;
using google::protobuf::compiler::SourceTreeDescriptorDatabase;
using google::protobuf::FileDescriptor;
using google::protobuf::FileDescriptorProto;
using google::protobuf::Descriptor;

// A Loader
class Loader
{
public:
    Loader(std::shared_ptr<Parser> parser);
    
    void SetIncludePaths(std::initializer_list<path> paths);
    void LoadSchema(path path);

    std::unique_ptr<DescriptorPool> GetSchemaPool();
    
private:
    std::shared_ptr<Parser> parser;
    
    // DescriptorDatabases must outlive their DescriptorPool,
    // so other classes can access schemaPool,
    // but make sure Loader doesn't go out of scope
    std::unique_ptr<DescriptorDatabase> includeDescDb;
    std::unique_ptr<DescriptorDatabase> schemaDescDb;
    std::unique_ptr<DescriptorPool> schemaPool;
    
    // A DiskSourceTree where paths are automatically mapped to root ("")
    class RootSourceTree : public google::protobuf::compiler::DiskSourceTree {
    public:
        RootSourceTree(std::initializer_list<path> mappings);
    };
};
}
}

#endif // PROFANEDB_STORAGE_LOADER_H
