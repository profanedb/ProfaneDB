#ifndef DB_H
#define DB_H

#include "parser.h"

#include <rocksdb/db.h>

#include <profanedb/protobuf/db.pb.h>

namespace profanedb {
namespace storage {

class Db
{
public:
    Db();
   
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
