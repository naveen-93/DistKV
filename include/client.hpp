#pragma once
#include <string>

class KVClient {
public:
    KVClient(const std::string& host, int port);
    ~KVClient();
    bool put(const std::string& key, const std::string& value);
    std::string get(const std::string& key);
    bool remove(const std::string& key);
    bool persist();

private:
    int sockfd;
    std::string send_request(const std::string& req);
};
