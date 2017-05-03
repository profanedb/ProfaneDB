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

#include "config.h"

profanedb::storage::Config::Config(
    profanedb::storage::Config::ProfaneDB profane,
    profanedb::storage::Config::RocksDB rocks)

  : profaneConfig(profane)
  , rocksConfig(rocks)
{
}

profanedb::storage::Config::ProfaneDB & profanedb::storage::Config::GetProfaneConfig()
{
    return this->profaneConfig;
}

profanedb::storage::Config::RocksDB & profanedb::storage::Config::GetRocksConfig()
{
    return this->rocksConfig;
}

profanedb::storage::Config::ProfaneDB::ProfaneDB(
    boost::filesystem::path schema,
    boost::filesystem::path options,
    boost::filesystem::path include)

  : schemaDefinition(schema)
  , sourceTree(new google::protobuf::compiler::DiskSourceTree)
{
    sourceTree->MapPath("", include.string());
    sourceTree->MapPath("", options.string());
    sourceTree->MapPath("", schema.string());
    
    google::protobuf::io::ZeroCopyInputStream * inputStream = sourceTree->Open("");
    if (inputStream == NULL)
        throw std::runtime_error(sourceTree->GetLastErrorMessage());
}

std::shared_ptr<google::protobuf::compiler::SourceTree> profanedb::storage::Config::ProfaneDB::GetSourceTree() const
{
    return std::dynamic_pointer_cast<google::protobuf::compiler::SourceTree>(this->sourceTree);
}

const boost::filesystem::path & profanedb::storage::Config::ProfaneDB::GetSchemaDefinitionPath() const
{
    return this->schemaDefinition;
}

profanedb::storage::Config::RocksDB::RocksDB(rocksdb::Options options, std::string name)
  : options(options)
  , name(name)
{
}
