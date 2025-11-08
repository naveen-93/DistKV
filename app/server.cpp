#include "server.hpp"
#include "kvstore.hpp"
#include <iostream>

int main(int argc, char* argv[]) {
    int port = 12345; // default port
    if (argc > 1) {
        port = std::stoi(argv[1]);
    }

    try {
        // Create KVStore using a storage file
        KVStore store("data.log");

        // Create server
        KVServer server(&store, port);

        std::cout << "Starting DistKV Server on port " << port << "\n";
        server.run(); // This blocks and handles clients
    }
    catch (const std::exception& e) {
        std::cerr << "Server error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
