// Test program for Config improvements
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>

void testBasicConfig() {
    nlm::Config config;
    
    // Test setting and getting values
    config.set("string_key", "hello");
    config.set("int_key", 42);
    config.set("double_key", 3.14);
    config.set("bool_key", true);
    
    assert(config.has("string_key"));
    assert(config.has("int_key"));
    assert(config.has("double_key"));
    assert(config.has("bool_key"));
    
    // Test type-safe getters
    assert(config.get<std::string>("string_key").value() == "hello");
    assert(config.get<int>("int_key").value() == 42);
    assert(config.get<double>("double_key").value() == 3.14);
    assert(config.get<bool>("bool_key").value() == true);
    
    // Test type checking
    assert(config.has<std::string>("string_key"));
    assert(config.has<int>("int_key"));
    assert(!config.has<double>("string_key")); // Type mismatch
    
    // Test getOr with default values
    assert(config.getOr<std::string>("missing_key", "default") == "default");
    assert(config.getOr<int>("missing_key", 99) == 99);
    
    std::cout << "✓ Basic config tests passed" << std::endl;
}

void testFileIO() {
    nlm::Config config;
    
    // Set some values
    config.set("test_string", "test value");
    config.set("test_int", 123);
    config.set("test_bool", false);
    config.set("test_double", 2.718);
    
    // Save to file
    assert(config.saveToFile("test_config.cfg"));
    
    // Clear and reload
    config.clear();
    assert(config.loadFromFile("test_config.cfg"));
    
    // Verify values were loaded
    assert(config.get<std::string>("test_string").value() == "test value");
    assert(config.get<int>("test_int").value() == 123);
    assert(config.get<bool>("test_bool").value() == false);
    assert(config.get<double>("test_double").value() == 2.718);
    
    std::cout << "✓ File I/O tests passed" << std::endl;
}

void testCommandLineParsing() {
    nlm::Config config;
    
    const char* args[] = {"program", "--string_key", "value", "-int_key", "42", "-bool_key", "true"};
    
    assert(config.loadFromArgs(5, const_cast<char**>(args)));
    
    assert(config.get<std::string>("string_key").value() == "value");
    assert(config.get<int>("int_key").value() == 42);
    assert(config.get<bool>("bool_key").value() == true);
    
    std::cout << "✓ Command line parsing tests passed" << std::endl;
}

void testSummary() {
    nlm::Config config;
    
    config.set("key1", "value1");
    config.set("key2", 42);
    config.set("key3", 3.14);
    config.set("key4", true);
    
    std::string summary = config.summary();
    
    assert(summary.find("Configuration (4 entries)") != std::string::npos);
    assert(summary.find("key1 = \"value1\"") != std::string::npos);
    assert(summary.find("key2 = 42") != std::string::npos);
    assert(summary.find("key3 = 3.14") != std::string::npos);
    assert(summary.find("key4 = true") != std::string::npos);
    
    std::cout << "✓ Summary tests passed" << std::endl;
}

int main() {
    testBasicConfig();
    testFileIO();
    testCommandLineParsing();
    testSummary();
    
    std::cout << "\n✅ All tests passed!" << std::endl;
    return 0;
}
