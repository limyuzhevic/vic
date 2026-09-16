#include "Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    
    // Test loading JSON file
    bool loaded = config.loadFromFile("/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_162aa8d0-0279-47ef-8176-53f27848c35a/test_config.json");
    
    if (!loaded) {
        std::cerr << "Failed to load config file!" << std::endl;
        return 1;
    }
    
    // Test JSON parsing results
    std::cout << "Config file loaded successfully!" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test specific values
    auto host = config.get<std::string>("server.host");
    if (host) std::cout << "server.host = " << *host << std::endl;
    
    auto port = config.get<int64_t>("server.port");
    if (port) std::cout << "server.port = " << *port << std::endl;
    
    auto ssl = config.get<bool>("server.ssl");
    if (ssl) std::cout << "server.ssl = " << (*ssl ? "true" : "false") << std::endl;
    
    auto name = config.get<std::string>("database.name");
    if (name) std::cout << "database.name = " << *name << std::endl;
    
    auto max_connections = config.get<int64_t>("database.max_connections");
    if (max_connections) std::cout << "database.max_connections = " << *max_connections << std::endl;
    
    auto timeout = config.get<double>("database.timeout");
    if (timeout) std::cout << "database.timeout = " << *timeout << std::endl;
    
    auto enabled = config.get<bool>("database.enabled");
    if (enabled) std::cout << "database.enabled = " << (*enabled ? "true" : "false") << std::endl;
    
    auto version = config.get<double>("version");
    if (version) std::cout << "version = " << *version << std::endl;
    
    auto max_items = config.get<int64_t>("max_items");
    if (max_items) std::cout << "max_items = " << *max_items << std::endl;
    
    return 0;
}