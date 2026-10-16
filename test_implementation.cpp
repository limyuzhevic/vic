#include "core/Config/Config.hpp"
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Testing JSON/YAML Config Parser Implementation ===" << std::endl;
    
    // Test 1: Basic Config creation
    nlm::Config config;
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "hello world", nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    std::cout << "Basic config creation: PASSED" << std::endl;
    
    // Test 2: Get values
    auto intVal = config.get<int>("test_int");
    auto stringVal = config.get<std::string>("test_string");
    auto doubleVal = config.get<double>("test_double");
    auto boolVal = config.get<bool>("test_bool");
    
    if (intVal && intVal.value() == 42 &&
        stringVal && stringVal.value() == "hello world" &&
        doubleVal && std::abs(doubleVal.value() - 3.14159) < 0.0001 &&
        boolVal && boolVal.value() == true) {
        std::cout << "Value retrieval: PASSED" << std::endl;
    } else {
        std::cout << "Value retrieval: FAILED" << std::endl;
        return 1;
    }
    
    // Test 3: Get keys
    auto keys = config.getKeys();
    if (keys.size() == 4) {
        std::cout << "Key management: PASSED" << std::endl;
    } else {
        std::cout << "Key management: FAILED" << std::endl;
        return 1;
    }
    
    // Test 4: File format detection
    std::string jsonFormat = nlm::Config::getFileFormat("test.json");
    std::string yamlFormat = nlm::Config::getFileFormat("test.yaml");
    std::string ymlFormat = nlm::Config::getFileFormat("test.yml");
    std::string cfgFormat = nlm::Config::getFileFormat("test.cfg");
    
    if (jsonFormat == "json" && yamlFormat == "yaml" && ymlFormat == "yml" && cfgFormat == "cfg") {
        std::cout << "File format detection: PASSED" << std::endl;
    } else {
        std::cout << "File format detection: FAILED" << std::endl;
        return 1;
    }
    
    // Test 5: JSON serialization
    std::string jsonString = config.saveToJsonString();
    if (!jsonString.empty() && jsonString.find("test_int") != std::string::npos) {
        std::cout << "JSON serialization: PASSED" << std::endl;
    } else {
        std::cout << "JSON serialization: FAILED" << std::endl;
        return 1;
    }
    
    // Test 6: Summary
    std::cout << "\n=== Configuration Summary ===" << std::endl;
    std::cout << config.summary() << std::endl;
    
    std::cout << "\n=== All Tests Passed! ===" << std::endl;
    return 0;
}