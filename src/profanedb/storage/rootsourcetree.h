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

#ifndef PROFANEDB_STORAGE_ROOTSOURCETREE_H
#define PROFANEDB_STORAGE_ROOTSOURCETREE_H

#include <exception>

#include <boost/filesystem.hpp>

#include <google/protobuf/compiler/importer.h>

namespace profanedb {
namespace storage {

class RootSourceTree : public google::protobuf::compiler::DiskSourceTree
{
public:
    RootSourceTree(std::initializer_list<boost::filesystem::path> mappings);
};
}
}

#endif // PROFANEDB_STORAGE_ROOTSOURCETREE_H
