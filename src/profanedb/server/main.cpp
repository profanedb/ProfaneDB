#include "server.h" 

int main(int argc, char* argv[]) {
    
    profanedb::server::Server server;
    server.Run();
    
    return 0;
}
