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

#ifndef PROFANEDB_STORAGE_PARSER_H
#define PROFANEDB_STORAGE_PARSER_H

#include <iostream>
#include <map>
#include <string>

#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/io/zero_copy_stream.h>
#include <google/protobuf/dynamic_message.h>

#include <boost/filesystem.hpp>

#include <profanedb/protobuf/db.pb.h>
#include <profanedb/protobuf/options.pb.h>

#include "config.h"
#include "rootsourcetree.h"

using namespace google::protobuf;

namespace profanedb {
namespace storage {
    
// Given a Any message, Parser looks for the corresponding definition in .proto files,
// and generates a map with keys of nested messages
class Parser
{
friend class Normalizer;

public:
    Parser(const Config::ProfaneDB & profaneConfig);
    ~Parser();

    // Using the provided schema, get a map with all nested messages and their unique key
    map<std::string, const Message &> NormalizeMessage(const Any & serializable);
    map<std::string, const Message &> NormalizeMessage(const Message & message);

private:
    std::unique_ptr<compiler::SourceTreeDescriptorDatabase> rootDescriptorDb;
    std::unique_ptr<compiler::SourceTreeDescriptorDatabase> schemaDescriptorDb;
    std::unique_ptr<MergedDescriptorDatabase> mergedSchemaDescriptorDb;
    
    std::shared_ptr<DescriptorPool> schemaPool;
    
    std::unique_ptr<SimpleDescriptorDatabase> normalizedDescriptorDb;
    
    DynamicMessageFactory messageFactory;
    
    // Given a Field
    string FieldToKey(const Message & container, const FieldDescriptor & fd);
    
    // A simple ErrorCollector for debug, write to stderr
    class ErrorCollector : public compiler::MultiFileErrorCollector {
    public:
        ErrorCollector();
        void AddError(const string & filename, int line, int column, const string & message) override;
        void AddWarning(const string & filename, int line, int column, const string & message) override;
    };

    ErrorCollector errCollector;
};
}
}

#endif // PROFANEDB_STORAGE_PARSER_H
