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

#include "loader.h"

profanedb::storage::Loader::Loader(profanedb::storage::Parser & parser)
  : parser(parser)
{
}

profanedb::storage::Loader::RootSourceTree::RootSourceTree(std::initializer_list<path> mappings)
  : google::protobuf::compiler::DiskSourceTree()
{
    for (const auto & path: mappings)
        this->MapPath("", path.string());
    
    google::protobuf::io::ZeroCopyInputStream * inputStream = this->Open("");
    if (inputStream == nullptr)
        throw std::runtime_error(this->GetLastErrorMessage());
}

void profanedb::storage::Loader::SetIncludePaths(std::initializer_list<path> paths)
{
    this->includeDescDb = std::unique_ptr<SourceTreeDescriptorDatabase>(
        new SourceTreeDescriptorDatabase(new RootSourceTree(paths)));
}

void profanedb::storage::Loader::LoadSchema(path path)
{
    this->schemaDescDb = std::unique_ptr<SourceTreeDescriptorDatabase>(
        new SourceTreeDescriptorDatabase(new RootSourceTree{path}));
    
    google::protobuf::MergedDescriptorDatabase schemaDb(this->includeDescDb.get(), this->schemaDescDb.get());
    google::protobuf::DescriptorPool schemaPool(&schemaDb);

    // Load ProfaneDB options, which schema definitions refer to
    schemaPool.FindFileByName("profanedb/protobuf/options.proto");

    for (auto const & file: recursive_directory_iterator(path, symlink_option::recurse)) {
        if (file.path().extension() == ".proto") {
            const FileDescriptor * fileDesc = schemaPool.FindFileByName(
                file.path().lexically_relative(path).string());

            FileDescriptorProto * fileProto = new FileDescriptorProto;
            fileDesc->CopyTo(fileProto);

            for (int i = 0; i < fileDesc->message_type_count(); i++) {
                const Descriptor * message = fileDesc->message_type(i);

                this->parser->ParseMessageDescriptor();
            }
        }
    }
}
