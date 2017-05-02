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

#ifndef DB_H
#define DB_H

#include <iostream>

#include "parser.h"

#include <rocksdb/db.h>

#include <profanedb/protobuf/db.pb.h>

namespace profanedb {
namespace storage {

// Db should be the main interface when embedding ProfaneDB
class Db
{
public:
    Db();
    ~Db();
   
    protobuf::GetResp Get(const protobuf::GetReq & request);
    protobuf::PutResp Put(const protobuf::PutReq & request);
    protobuf::DelResp Delete(const protobuf::DelReq & request);
private:
    rocksdb::DB * db;
    rocksdb::Options options;
    
    Parser parser;
};
}
}

#endif // DB_H
