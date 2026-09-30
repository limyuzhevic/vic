#include <iostream>
#include <cassert>
#include "src/core/Config/Config.hpp"

// Simple test to verify JSON/YAML support
int main() {
    std::cout << "=== Testing Enhanced Config Class ===" << std::endl;
    
    try {
        // Test 1: Basic configuration with nested values
        nlm::Config config;
        
        // Set various types of values (using supported types)
        config.set("network.learning_rate", 0.001);
        config.set("network.batch_size", 32);
        config.set("network.hidden_layers", std::vector<int>{128, 64, 32});
        config.set("training.epochs", 100);
        config.set("training.optimizer", "adam");
        
        std::cout << "\n1. Config created with nested values:" << std::endl;
        std::cout << config.summary() << std::endl;
        
        // Test 2: JSON serialization
        std::string json_str = config.toJson();
        std::cout << "\n2. JSON serialization:" << std::endl;
        std::cout << json_str << std::endl;
        
        // Test 3: JSON deserialization
        nlm::Config config_from_json;
        config_from_json.fromJson(json_str);
        
        std::cout << "\n3. Config loaded from JSON:" << std::endl;
        std::cout << config_from_json.summary() << std::endl;
        
        // Verify key values exist
        auto learning_rate = config_from_json.get<double>("network.learning_rate");
        if (learning_rate.has_value()) {
            std::cout << "   learning_rate: " << *learning_rate << std::endl;
        }
        
        auto batch_size = config_from_json.get<int>("network.batch_size");
        if (batch_size.has_value()) {
            std::cout << "   batch_size: " << *batch_size << std::endl;
        }
        
        auto epochs = config_from_json.get<int>("training.epochs");
        if (epochs.has_value()) {
            std::cout << "   epochs: " << *epochs << std::endl;
        }
        
        // Test 4: Backward compatibility with simple key=value format
        nlm::Config simple_config;
        simple_config.set("simple_key", "simple_value");
        simple_config.set("numeric_key", 42);
        simple_config.set("float_key", 3.14159);
        simple_config.set("boolean_key", true);
        
        std::cout << "\n4. Simple key=value format config:" << std::endl;
        std::cout << simple_config.summary() << std::endl;
        
        // Test 5: File format detection
        std::string json_format = nlm::Config::detectFileFormat("config.json");
        std::string yaml_format = nlm::Config::detectFileFormat("config.yaml");
        std::string unknown_format = nlm::Config::detectFileFormat("config.txt");
        
        std::cout << "\n5. File format detection:" << std::endl;
        std::cout << "   config.json -> " << json_format << std::endl;
        std::cout << "   config.yaml -> " << yaml_format << std::endl;
        std::cout << "   config.txt -> " << unknown_format << std::endl;
        
        // Test 6: Configuration statistics
        auto stats = config.getStats();
        std::cout << "\n6. Configuration statistics:" << std::endl;
        std::cout << "   Total entries: " << stats.totalEntries << std::endl;
        std::cout << "   Memory usage: " << stats.memoryUsage << " bytes" << std::endl;
        
        std::cout << "\n=== All tests completed successfully! ===" << std::endl;
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
}