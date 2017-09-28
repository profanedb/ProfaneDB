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

#include "config.h"

// TODO OS dependent default paths

using boost::program_options::value;
using boost::log::trivial::severity_level;
using boost::filesystem::path;
using boost::filesystem::ifstream;

namespace profanedb {
namespace server {

// TODO Config file
Config::Config(int argc, char * argv[])
  : conf("Configuration")
  , cmd("Command line options")
  , file("File configuration")
{
    conf.add_options()
        ("log_level",
         value<severity_level>(),
         "trace, debug, info, warning, error, fatal")
        
        ("proto_path,I",
         value< std::vector< path > >()
            ->default_value(std::vector<path>(), "/usr/include")
            ->composing(),
         "Specify the paths to import proto files (google/protobuf/..., profanedb/protobuf/...)")
        
        ("schema_path,S",
         value< std::vector< path > >()->composing(),
         "Specify the paths to load the user defined schema");
        
    cmd.add_options()
        ("help,h",
         "Display this help message")
        
        ("profanedb_config_file,c",
         value<path>()->default_value("/etc/profanedb/server.conf"),
         "The path to ProfaneDB configuration file");
    cmd.add(conf);
    
    file.add_options()
        ("rocksdb.path",
         value<path>()->default_value("/var/profanedb/rocksdb"),
         "Set the path RocksDB uses to store its content");
    file.add(conf);
    
    
    // Command line parameters have the highest priority
    boost::program_options::store(
        boost::program_options::parse_command_line(argc, argv, cmd), vm
    );
    
    // Now read config file
    // TODO Check file exists
    ifstream config_file(vm["profanedb_config_file"].as<path>());
    
    // Parse config file and set missing options
    boost::program_options::store(
        boost::program_options::parse_config_file(config_file, file), vm
    );
    
    boost::program_options::notify(vm);
}

const bool Config::ShowHelp() const
{
    if (this->vm.count("help")) {
        std::cout << this->cmd << std::endl;
        return true;
    }
    return false;
}

const severity_level Config::LogLevel() const
{
    return this->vm["log_level"].as<severity_level>();
}

const std::vector<path> Config::IncludePath() const
{
    return this->vm["proto_path"].as< std::vector< path > >();
}

const std::vector<path> Config::SchemaPath() const
{
    return this->vm["schema_path"].as< std::vector< path > >();
}

const path Config::RocksPath() const
{
    return this->vm["rocksdb.path"].as<path>();
}

// TODO From file
const rocksdb::Options Config::RocksOptions() const
{
    rocksdb::Options options;
    options.create_if_missing = true;
    
    return options;
}


}
}
