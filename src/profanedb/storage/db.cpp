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

#include "db.h"

profanedb::storage::Db::Db(Config config)
  : config(config)
  , parser(config.GetProfaneConfig())
  , normalizer(parser)
{
}

profanedb::storage::Db::~Db()
{
	delete this->db;
}

profanedb::protobuf::GetResp profanedb::storage::Db::Get(const protobuf::GetReq & request)
{
	std::string * serialized = new std::string();
	db->Get(rocksdb::ReadOptions(), request.key().SerializeAsString(), serialized);

	return *protobuf::GetResp().New();
}

profanedb::protobuf::PutResp profanedb::storage::Db::Put(const protobuf::PutReq & request)
{
    auto map = normalizer.NormalizeMessage(request.serializable());
    
    for (auto const & obj: map) {
        std::cout << obj.first << std::endl;
        db->Put(rocksdb::WriteOptions(), obj.first, obj.second.SerializeAsString());
    }
    
    return *protobuf::PutResp().New();
}

profanedb::protobuf::DelResp profanedb::storage::Db::Delete(const protobuf::DelReq & request)
{
}
