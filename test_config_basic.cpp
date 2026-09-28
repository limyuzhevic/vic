#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    nlm::Config config;
    
    std::cout << "Testing NLM Configuration System (basic)..." << std::endl;
    
    // Test basic setting and getting
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test getOr
    int val = config.getOr<int>("int_value", 100);
    assert(val == 42);
    
    int missingVal = config.getOr<int>("missing_key", 100);
    assert(missingVal == 100);
    
    std::cout << "Basic config test passed!" << std::endl;
    
    // Test different types
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    
    // Test vectors
    std::vector<int> intArray = {1, 2, 3};
    config.set("int_array", intArray, nlm::ConfigSource::Runtime);
    
    auto retrievedArray = config.get<std::vector<int>>("int_array");
    assert(retrievedArray.has_value());
    assert(retrievedArray.value().size() == 3);
    assert(retrievedArray.value()[0] == 1);
    assert(retrievedArray.value()[2] == 3);
    
    std::cout << "Basic config test passed!" << std::endl;
    
    // Test sources
    nlm::Config config2;
    config2.set("file_value", "from_file", nlm::ConfigSource::File);
    config2.set("cmdline_value", "from_cmdline", nlm::ConfigSource::CommandLine);
    config2.set("runtime_value", 123, nlm::ConfigSource::Runtime);
    
    assert(config2.getOr<std::string>("file_value", "") == "from_file");
    assert(config2.getOr<std::string>("cmdline_value", "") == "from_cmdline");
    assert(config2.getOr<int>("runtime_value", 0) == 123);
    
    std::cout << "Config sources test passed!" << std::endl;
    
    // Test batch operations (basic version)
    std::map<std::string, nlm::ConfigValue> batchValues;
    batchValues["batch_int"] = 100;
    
    // Implement batchSet manually for now
    for (const auto& pair : batchValues) {
        config.set(pair.first, pair.second, nlm::ConfigSource::Runtime);
    }
    
    assert(config.getOr<int>("batch_int", 0) == 100);
    
    std::cout << "Batch operations test passed!" << std::endl;
    
    std::cout << "\nAll basic tests passed!" << std::endl;
    return 0;
}