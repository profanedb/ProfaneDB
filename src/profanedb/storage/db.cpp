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
    rocksdb::DB::Open(
    		config.GetRocksConfig().GetOptions(),
			config.GetRocksConfig().GetName(),
			&db);
}

profanedb::storage::Db::~Db()
{
	delete this->db;
}

profanedb::protobuf::GetResp profanedb::storage::Db::Get(const protobuf::GetReq & request)
{
	std::string type = request.key();
	// Split at first dollar sign ($), here starts the actual key value
	type = type.substr(0, type.find('$'));
	// The last dot (.) separates the message type name from the field type name
	type = type.substr(0, type.rfind('.'));

	std::string * serialized = new std::string();
	db->Get(rocksdb::ReadOptions(), request.key(), serialized);

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
