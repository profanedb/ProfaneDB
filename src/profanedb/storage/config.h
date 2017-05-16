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

#ifndef PROFANEDB_STORAGE_CONFIG_H
#define PROFANEDB_STORAGE_CONFIG_H

#include <iostream>
#include <string>
#include <exception>

#include <google/protobuf/compiler/importer.h>

#include <rocksdb/options.h>

#include <boost/filesystem.hpp>

namespace profanedb {
namespace storage {

class Config
{
public:
    class ProfaneDB {
    public:
        ProfaneDB(boost::filesystem::path schema,
                  boost::filesystem::path options,
                  boost::filesystem::path include = boost::filesystem::path("/usr/include"));
        
        const boost::filesystem::path schemaDefinition;
        const boost::filesystem::path profaneDbOptions;
        const boost::filesystem::path includePath;
    };
    
    class RocksDB {
    public:
        RocksDB(rocksdb::Options options, std::string name);
        
        const rocksdb::Options GetOptions() const;
        const std::string GetName() const;

    private:
        rocksdb::Options options;
        std::string name;
    };
    
    Config(ProfaneDB profane, RocksDB rocks);
    
    ProfaneDB & GetProfaneConfig();
    RocksDB & GetRocksConfig();
    
private:
    ProfaneDB profaneConfig;
    RocksDB rocksConfig;
};
}
}

#endif // PROFANEDB_STORAGE_CONFIG_H
