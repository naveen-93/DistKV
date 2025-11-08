#include "client.hpp"
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

KVClient::KVClient(const std::string& host, int port) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("socket");
        throw std::runtime_error("Failed to create socket");
    }

    sockaddr_in serv_addr{};
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, host.c_str(), &serv_addr.sin_addr) <= 0) {
        perror("inet_pton");
        throw std::runtime_error("Invalid address");
    }

    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("connect");
        throw std::runtime_error("Connection failed");
    }
}

KVClient::~KVClient() {
    close(sockfd);
}

std::string KVClient::send_request(const std::string& req) {
    send(sockfd, req.c_str(), req.size(), 0);
    char buffer[1024];
    int n = read(sockfd, buffer, sizeof(buffer) - 1);
    if (n <= 0) return "";
    buffer[n] = '\0';
    return std::string(buffer);
}

bool KVClient::put(const std::string& key, const std::string& value) {
    return send_request("PUT " + key + " " + value) == "OK\n";
}

std::string KVClient::get(const std::string& key) {
    return send_request("GET " + key);
}

bool KVClient::remove(const std::string& key) {
    return send_request("DELETE " + key) == "OK\n";
}

bool KVClient::persist() {
    return send_request("PERSIST") == "OK\n";
}
