#include "db.h"

profanedb::storage::Db::Db()
{
    options.create_if_missing = true;
    
    rocksdb::DB::Open(options, "/tmp/profanedb", &db);
}

profanedb::storage::Db::~Db()
{
}

profanedb::protobuf::GetResp profanedb::storage::Db::Get(const profanedb::protobuf::GetReq & request)
{
}

profanedb::protobuf::PutResp profanedb::storage::Db::Put(const profanedb::protobuf::PutReq & request)
{
    auto map = parser.ParseMessage(request.serializable());
    
    for (auto const & obj: map) {
        std::cout << obj.first << ":" << std::endl << obj.second.DebugString() << std::endl;
    }
}

profanedb::protobuf::DelResp profanedb::storage::Db::Delete(const profanedb::protobuf::DelReq & request)
{
}
