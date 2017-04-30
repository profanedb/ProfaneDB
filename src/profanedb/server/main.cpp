#include <iostream>
#include <string>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <profanedb/protobuf/options.pb.h>
#include <profanedb/server/server.h>

using namespace google::protobuf;

int main(int argc, char* argv[]) {
    profanedb::server::Server server;
    server.Run();
}
