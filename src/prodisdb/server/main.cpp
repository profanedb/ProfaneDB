#include <iostream>
#include <string>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/io/zero_copy_stream.h>

#include <prodisdb/protobuf/options.pb.h>
#include <prodisdb/server/server.h>

const std::string PROTO_DIR = "/home/giorgio/Documents/ProdisDB/test"; // TODO Should be config

using namespace google::protobuf;

int main(int argc, char* argv[]) {
    prodisdb::server::Server server;
    server.Run();
}
