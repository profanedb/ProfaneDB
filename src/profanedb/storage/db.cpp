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

#include "db.h"

profanedb::storage::Db::Db(profanedb::storage::Config config)
  : config(config)
  , parser(config.GetProfaneConfig())
  , normalizer(parser)
{
    // rocksdb::DB::Open(options, name, &db);
}

profanedb::storage::Db::~Db()
{
}

profanedb::protobuf::GetResp profanedb::storage::Db::Get(const profanedb::protobuf::GetReq & request)
{
}

profanedb::protobuf::PutResp profanedb::storage::Db::Put(const profanedb::protobuf::PutReq & request)
{
    auto map = parser.NormalizeMessage(request.serializable());
    
    for (auto const & obj: map) {
        std::cout << obj.first << ":" << std::endl << obj.second.SerializeAsString() << std::endl;
    }
}

profanedb::protobuf::DelResp profanedb::storage::Db::Delete(const profanedb::protobuf::DelReq & request)
{
}
