#include "server.hpp"
#include <iostream>
#include <thread>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

KVServer::KVServer(KVStore* kv, int port) : kvstore(kv), port(port) {}

void KVServer::run() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return;
    }

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return;
    }

    if (listen(server_fd, 5) < 0) {
        perror("listen");
        return;
    }

    std::cout << "KVServer listening on port " << port << "\n";

    while (true) {
        int client_sock = accept(server_fd, nullptr, nullptr);
        if (client_sock < 0) {
            perror("accept");
            continue;
        }
        std::thread(&KVServer::handle_client, this, client_sock).detach();
    }
}

void KVServer::handle_client(int client_sock) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int n = read(client_sock, buffer, sizeof(buffer) - 1);
        if (n <= 0) break;

        std::string request(buffer);
        std::istringstream iss(request);
        std::string cmd;
        iss >> cmd;

        std::string response;

        if (cmd == "PUT") {
            std::string key, value;
            iss >> key >> value;
            if (kvstore->put(key, value)) response = "OK\n";
            else response = "ERROR\n";

        } else if (cmd == "GET") {
            std::string key;
            iss >> key;
            std::string val;
            if (kvstore->get(key, val)) response = val + "\n";
            else response = "KEY NOT_FOUND\n";

        } else if (cmd == "DELETE") {
            std::string key;
            iss >> key;
            if (kvstore->remove(key)) response = "OK\n";
            else response = "KEY NOT_FOUND\n";

        } else if (cmd == "PERSIST") {
            kvstore->persist();
            response = "OK\n";

        } else {
            response = "UNKNOWN_CMD\n";
        }

        write(client_sock, response.c_str(), response.size());
    }
    close(client_sock);
}
