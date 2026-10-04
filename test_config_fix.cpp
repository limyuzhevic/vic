// Test program to verify Config.cpp fixes
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config.cpp fixes..." << std::endl;
    
    // Test basic Config operations
    nlm::Config config;
    
    // Test creation
    std::cout << "Test 1: Basic creation and has()..." << std::endl;
    assert(!config.has("test_key"));
    std::cout << "   PASS: Config created correctly" << std::endl;
    
    // Test setting and getting int
    std::cout << "Test 2: Setting and getting int..." << std::endl;
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    std::cout << "   PASS: Int value set and retrieved correctly" << std::endl;
    
    // Test setting and getting string
    std::cout << "Test 3: Setting and getting string..." << std::endl;
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    assert(config.has("string_value"));
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    std::cout << "   PASS: String value set and retrieved correctly" << std::endl;
    
    // Test setting and getting double
    std::cout << "Test 4: Setting and getting double..." << std::endl;
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    assert(config.has("double_value"));
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    std::cout << "   PASS: Double value set and retrieved correctly" << std::endl;
    
    // Test setting and getting bool
    std::cout << "Test 5: Setting and getting bool..." << std::endl;
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    assert(config.has("bool_value"));
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    std::cout << "   PASS: Boolean value set and retrieved correctly" << std::endl;
    
    // Test getOr functionality
    std::cout << "Test 6: getOr functionality..." << std::endl;
    int default_val = 100;
    int missing_val = config.getOr<int>("missing", default_val);
    assert(missing_val == default_val);
    
    int existing_val = config.getOr<int>("int_value", default_val);
    assert(existing_val == 42);
    std::cout << "   PASS: getOr functionality works correctly" << std::endl;
    
    // Test remove functionality
    std::cout << "Test 7: remove functionality..." << std::endl;
    config.set("to_remove", "value", nlm::ConfigSource::Runtime);
    assert(config.has("to_remove"));
    
    config.remove("to_remove");
    assert(!config.has("to_remove"));
    std::cout << "   PASS: Remove functionality works correctly" << std::endl;
    
    // Test getKeys functionality
    std::cout << "Test 8: getKeys functionality..." << std::endl;
    config.clear();
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    std::cout << "   PASS: getKeys functionality works correctly" << std::endl;
    
    // Test clear functionality
    std::cout << "Test 9: clear functionality..." << std::endl;
    config.clear();
    assert(config.getKeys().size() == 0);
    std::cout << "   PASS: Clear functionality works correctly" << std::endl;
    
    // Test ConfigSource handling
    std::cout << "Test 10: ConfigSource handling..." << std::endl;
    config.set("source_test", "default", nlm::ConfigSource::Default);
    config.set("source_test", "file", nlm::ConfigSource::File);
    config.set("source_test", "command_line", nlm::ConfigSource::CommandLine);
    config.set("source_test", "runtime", nlm::ConfigSource::Runtime);
    
    auto sourceVal = config.get<std::string>("source_test");
    assert(sourceVal.has_value());
    assert(*sourceVal == "runtime"); // Last set should be kept
    std::cout << "   PASS: ConfigSource handling works correctly" << std::endl;
    
    std::cout << "\nAll Config.cpp tests passed successfully!" << std::endl;
    return 0;
}