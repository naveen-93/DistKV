#include "kvstore.hpp"
#include <iostream>
#include <sstream>

void print_help() {
    std::cout << "Commands:\n"
              << "  put <key> <value>  - Store key-value pair\n"
              << "  get <key>          - Retrieve value for key\n"
              << "  delete <key>       - Remove key\n"
              << "  persist            - Compact storage file\n"
              << "  help               - Show this help\n"
              << "  exit               - Exit program\n";
}

int main() {
    try {
        KVStore store("data.log");
        std::string line;
        
        std::cout << "DistKV - Simple Key-Value Store\n";
        std::cout << "Type 'help' for commands\n\n";
        
        while (true) {
            std::cout << "> ";
            
            if (!std::getline(std::cin, line)) {
                break;
            }
            
            std::istringstream iss(line);
            std::string cmd;
            iss >> cmd;
            
            if (cmd.empty()) {
                continue;
            }
            
            if (cmd == "put") {
                std::string key, value;
                if (!(iss >> key >> value)) {
                    std::cout << "Usage: put <key> <value>\n";
                    continue;
                }
                
                if (store.put(key, value)) {
                    std::cout << "OK\n";
                } else {
                    std::cout << "ERROR: Failed to store key\n";
                }
                
            } else if (cmd == "get") {
                std::string key;
                if (!(iss >> key)) {
                    std::cout << "Usage: get <key>\n";
                    continue;
                }
                
                std::string val;
                if (store.get(key, val)) {
                    std::cout << val << "\n";
                } else {
                    std::cout << "NOT FOUND\n";
                }
                
            } else if (cmd == "delete") {
                std::string key;
                if (!(iss >> key)) {
                    std::cout << "Usage: delete <key>\n";
                    continue;
                }
                
                if (store.remove(key)) {
                    std::cout << "OK\n";
                } else {
                    std::cout << "NOT FOUND\n";
                }
                
            } else if (cmd == "persist") {
                store.persist();
                std::cout << "OK - Storage compacted\n";
                
            } else if (cmd == "help") {
                print_help();
                
            } else if (cmd == "exit" || cmd == "quit") {
                std::cout << "Goodbye!\n";
                break;
                
            } else {
                std::cout << "Unknown command: " << cmd << "\n";
                std::cout << "Type 'help' for available commands\n";
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}