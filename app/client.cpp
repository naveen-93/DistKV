#include "client.hpp"
#include <iostream>
#include <sstream>

void print_help() {
    std::cout << "Commands:\n"
              << "  put <key> <value>\n"
              << "  get <key>\n"
              << "  delete <key>\n"
              << "  persist\n"
              << "  help\n"
              << "  exit\n";
}

int main() {
    try {
        KVClient client("127.0.0.1", 12345);
        std::string line;
        std::cout << "DistKV Client CLI\n";
        print_help();

        while (true) {
            std::cout << "> ";
            if (!std::getline(std::cin, line)) break;

            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;

            if (cmd == "put") {
                std::string key, value;
                iss >> key >> value;
                if (client.put(key, value)) std::cout << "OK\n";
                else std::cout << "ERROR\n";

            } else if (cmd == "get") {
                std::string key;
                iss >> key;
                std::cout << client.get(key);

            } else if (cmd == "delete") {
                std::string key;
                iss >> key;
                if (client.remove(key)) std::cout << "OK\n";
                else std::cout << "NOT_FOUND\n";

            } else if (cmd == "persist") {
                if (client.persist()) std::cout << "OK\n";

            } else if (cmd == "help") {
                print_help();

            } else if (cmd == "exit") {
                break;

            } else {
                std::cout << "Unknown command\n";
            }
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
