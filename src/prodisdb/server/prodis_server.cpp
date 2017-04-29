#include "prodis_server.h"

ProdisServer::~ProdisServer()
{
    server->Shutdown();
    cq->Shutdown();
}
