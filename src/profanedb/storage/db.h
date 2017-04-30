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
