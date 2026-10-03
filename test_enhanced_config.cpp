#include <iostream>
#include "../src/core/Config/Config.hpp"

int main() {
    std::cout << "NLM Configuration System - Enhanced Version" << std::endl;
    std::cout << "=" << std::string(50, '=') << std::endl << std::endl;
    
    nlm::Config config;
    
    // Test basic configuration setting
    config.set("server_port", 8080, nlm::ConfigSource::Runtime);
    config.set("host_name", std::string("localhost"), nlm::ConfigSource::Runtime);
    config.set("debug_mode", true, nlm::ConfigSource::Runtime);
    config.set("max_connections", 100, nlm::ConfigSource::Runtime);
    config.set("timeout", 30.5, nlm::ConfigSource::Runtime);
    
    // Test vector values
    config.set("allowed_ports", std::vector<int>({80, 443, 8080, 3000}), nlm::ConfigSource::Runtime);
    config.set("server_ips", std::vector<std::string>({"192.168.1.1", "10.0.0.1", "172.16.0.1"}), nlm::ConfigSource::Runtime);
    
    // Test nested configuration (map)
    std::unordered_map<std::string, std::string> db_config;
    db_config["host"] = "localhost";
    db_config["port"] = "5432";
    db_config["database"] = "neural_net";
    db_config["username"] = "admin";
    config.set("database", db_config, nlm::ConfigSource::Runtime);
    
    // Display configuration summary
    std::cout << "Configuration Summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test getting values
    std::cout << "Getting values:" << std::endl;
    auto port = config.get<int>("server_port");
    if (port) std::cout << "  server_port = " << *port << std::endl;
    
    auto debug = config.get<bool>("debug_mode");
    if (debug) std::cout << "  debug_mode = " << (*debug ? "true" : "false") << std::endl;
    
    auto timeout = config.get<double>("timeout");
    if (timeout) std::cout << "  timeout = " << *timeout << std::endl;
    
    // Test getOr with default
    auto workers = config.getOr<int>("worker_threads", 4);
    std::cout << "  worker_threads (with default) = " << workers << std::endl;
    
    // Test has
    std::cout << "Checking existence:" << std::endl;
    std::cout << "  has server_port: " << (config.has("server_port") ? "yes" : "no") << std::endl;
    std::cout << "  has missing_key: " << (config.has("missing_key") ? "yes" : "no") << std::endl;
    
    // Test removing
    std::cout << "\nRemoving 'debug_mode' and checking:" << std::endl;
    config.remove("debug_mode");
    std::cout << "  has debug_mode after removal: " << (config.has("debug_mode") ? "yes" : "no") << std::endl;
    
    // Test saveToFile with backward compatibility (INI format)
    std::cout << "\nSaving to config.ini:" << std::endl;
    try {
        bool saved = config.saveToFile("config.ini");
        std::cout << "  Save to config.ini: " << (saved ? "success" : "failed") << std::endl;
    } catch (const nlm::ConfigError& e) {
        std::cout << "  Error: " << e.what() << std::endl;
    }
    
    // Test file loading
    std::cout << "\nLoading from config.ini:" << std::endl;
    try {
        config.clear();
        config.loadFromFile("config.ini");
        std::cout << "  Loaded from config.ini successfully" << std::endl;
        std::cout << "  Entries after load: " << config.getKeys().size() << std::endl;
    } catch (const nlm::ConfigError& e) {
        std::cout << "  Error: " << e.what() << std::endl;
    }
    
    // Test schema validation
    std::cout << "\nTesting schema validation:" << std::endl;
    try {
        nlm::ConfigSchema schema;
        schema.addField("server_port", "int", true, "Server port number", "1", "65535");
        schema.addField("host_name", "string", true, "Server hostname", "", "");
        schema.addField("max_connections", "int", false, "Maximum connections", "0", "1000");
        
        bool valid = config.validate(schema);
        std::cout << "  Schema valid: " << (valid ? "yes" : "no") << std::endl;
        
        std::string errors = config.getValidationErrors(schema);
        if (!errors.empty()) {
            std::cout << "  Validation errors:" << std::endl << errors;
        }
    } catch (const nlm::ConfigError& e) {
        std::cout << "  Error: " << e.what() << std::endl;
    }
    
    std::cout << "\n" << std::string(50, '=') << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    
    return 0;
}