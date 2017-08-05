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

#ifndef PROFANEDB_SERVER_CONFIG_H
#define PROFANEDB_SERVER_CONFIG_H

#include <iostream>

#include <boost/program_options.hpp>

#include <boost/log/trivial.hpp>
#include <boost/filesystem.hpp>

#include <rocksdb/db.h>

namespace profanedb {
namespace server {

class Config
{
public:
    Config(int argc, char * argv[]);
    
    // If asked to do so, display usage message and return true
    const bool ShowHelp() const;
    
    // Proto path are the import paths (eg /usr/include),
    // where google/protobuf/*.proto and profanedb/protobuf/*.proto are
    // By default it would usually be /usr/include and /usr/local/include
    const std::vector< boost::filesystem::path > IncludePath() const;
    
    // Schema path must be defined for the user proto definitions
    const std::vector< boost::filesystem::path > SchemaPath() const;
    
    const boost::log::trivial::severity_level LogLevel() const;
    
    // Where RocksDB should save its content
    const boost::filesystem::path RocksPath() const;
    
    // RocksDB options, usually would come from file
    const rocksdb::Options RocksOptions() const;
    
private:
    boost::program_options::options_description desc;
    boost::program_options::variables_map vm;
};
}
}

#endif // PROFANEDB_SERVER_CONFIG_H
