// server.hpp
#pragma once
#include "kvstore.hpp"
#include <string>

class KVServer {
public:
    KVServer(KVStore* kv, int port);
    void run(); // Start the server

private:
    KVStore* kvstore;
    int port;

    void handle_client(int client_socket);
};
