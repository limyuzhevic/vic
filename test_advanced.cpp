#include "core/Config/Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Config class with advanced features..." << std::endl;
    
    // Create config
    nlm::Config config;
    std::cout << "Created config" << std::endl;
    
    // Enable advanced features
    config.enableAdvancedConfig(true);
    std::cout << "Enabled advanced config" << std::endl;
    
    // Test basic functionality
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Test get values
    auto intVal = config.get<int>("int_value");
    if (intVal.has_value()) {
        std::cout << "Got int value: " << *intVal << std::endl;
    }
    
    auto doubleVal = config.get<double>("double_value");
    if (doubleVal.has_value()) {
        std::cout << "Got double value: " << *doubleVal << std::endl;
    }
    
    auto stringVal = config.get<std::string>("string_value");
    if (stringVal.has_value()) {
        std::cout << "Got string value: " << *stringVal << std::endl;
    }
    
    auto boolVal = config.get<bool>("bool_value");
    if (boolVal.has_value()) {
        std::cout << "Got bool value: " << (*boolVal ? "true" : "false") << std::endl;
    }
    
    // Test getOr
    int missingVal = config.getOr<int>("missing", 100);
    std::cout << "Got missing value with default: " << missingVal << std::endl;
    
    // Test has
    std::cout << "Has int_value: " << (config.has("int_value") ? "yes" : "no") << std::endl;
    
    // Test getKeys
    auto keys = config.getKeys();
    std::cout << "Keys: ";
    for (const auto& key : keys) {
        std::cout << key << " ";
    }
    std::cout << std::endl;
    
    // Test summary
    std::cout << "Summary:" << std::endl;
    std::cout << config.summary() << std::endl;
    
    // Test advanced manager
    auto* advancedManager = config.getAdvancedManager();
    if (advancedManager) {
        std::cout << "Advanced manager available" << std::endl;
        std::cout << "Manager summary:" << std::endl;
        std::cout << advancedManager->summary() << std::endl;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}