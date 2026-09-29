#include "../core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

void testConfigBasic() {
    std::cout << "Testing basic Config functionality..." << std::endl;
    
    // Create config
    nlm::Config config;
    
    // Set values
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("bool_value", true);
    config.set("string_value", "Hello World");
    config.set("int_vector", std::vector<int>{1, 2, 3, 4, 5});
    config.set("double_vector", std::vector<double>{1.1, 2.2, 3.3});
    config.set("string_vector", std::vector<std::string>{"a", "b", "c"});
    
    // Get values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && intVal.value() == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && doubleVal.value() == 3.14);
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && boolVal.value() == true);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && stringVal.value() == "Hello World");
    
    // Test getOr with defaults
    assert(config.getOr<int>("nonexistent", 99) == 99);
    assert(config.getOr<std::string>("nonexistent", "default") == "default");
    
    // Test has
    assert(config.has("int_value"));
    assert(!config.has("nonexistent"));
    
    // Test keys
    auto keys = config.getKeys();
    assert(keys.size() == 7); // All the keys we set
    
    // Test remove
    config.remove("int_value");
    assert(!config.has("int_value"));
    
    // Test summary
    auto summary = config.summary();
    assert(!summary.empty());
    
    std::cout << "✓ Basic Config functionality passed!" << std::endl;
}

void testConfigFileIO() {
    std::cout << "Testing Config file I/O..." << std::endl;
    
    // Create config
    nlm::Config config;
    
    // Set values
    config.set("test_int", 123);
    config.set("test_double", 45.67);
    config.set("test_bool", false);
    config.set("test_string", "Test String");
    config.set("test_vector", std::vector<int>{10, 20, 30});
    
    // Save to file
    std::string testFile = "/tmp/nlm_test_config.txt";
    bool saved = config.saveToFile(testFile);
    assert(saved);
    
    // Verify file exists and has content
    std::ifstream file(testFile);
    assert(file.good());
    std::string line;
    assert(std::getline(file, line)); // Header comment
    assert(!line.empty());
    
    // Load from file
    nlm::Config config2;
    bool loaded = config2.loadFromFile(testFile);
    assert(loaded);
    
    // Verify loaded values
    assert(config2.has("test_int"));
    assert(config2.has("test_double"));
    assert(config2.has("test_bool"));
    assert(config2.has("test_string"));
    assert(config2.has("test_vector"));
    
    std::cout << "✓ Config file I/O passed!" << std::endl;
    
    // Clean up
    std::remove(testFile.c_str());
}

void testConfigCommandLine() {
    std::cout << "Testing Config command line parsing..." << std::endl;
    
    nlm::Config config;
    
    // Simulate command line arguments
    const int argc = 5;
    char* argv[] = {
        (char*)"test",
        (char*)"--test_int=123",
        (char*)"--test_double=3.14",
        (char*)"--test_bool=true",
        (char*)"--test_string=Hello"
    };
    
    bool parsed = config.loadFromArgs(argc, argv);
    assert(parsed);
    
    // Verify parsed values
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && intVal.value() == 123);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && doubleVal.value() == 3.14);
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && boolVal.value() == true);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && stringVal.value() == "Hello");
    
    std::cout << "✓ Config command line parsing passed!" << std::endl;
}

void testConfigMoveSemantics() {
    std::cout << "Testing Config move semantics..." << std::endl;
    
    nlm::Config config1;
    config1.set("test", "value");
    
    nlm::Config config2(std::move(config1));
    assert(config2.has("test"));
    assert(config1.has("test") == false); // Should be moved from
    
    config2.set("test2", "value2");
    
    config1 = std::move(config2);
    assert(config1.has("test2"));
    
    std::cout << "✓ Config move semantics passed!" << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration System Tests ===" << std::endl;
    
    testConfigBasic();
    testConfigFileIO();
    testConfigCommandLine();
    testConfigMoveSemantics();
    
    std::cout << "\n=== All Tests Passed! ===" << std::endl;
    return 0;
}
