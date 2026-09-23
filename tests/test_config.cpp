// Config Tests
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_config {

void testConfigCreation() {
    nlm::Config config;
    assert(!config.has("test"));
    
    std::cout << "    testConfigCreation passed" << std::endl;
}

void testConfigSetAndGet() {
    nlm::Config config;
    
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
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
    
    std::cout << "    testConfigSetAndGet passed" << std::endl;
}

void testConfigGetOr() {
    nlm::Config config;
    
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    
    std::cout << "    testConfigGetOr passed" << std::endl;
}

void testConfigRemove() {
    nlm::Config config;
    
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    
    config.remove("test");
    assert(!config.has("test"));
    
    std::cout << "    testConfigRemove passed" << std::endl;
}

void testConfigLoadFromFile() {
    nlm::Config config;
    
    // Test loading from empty file (should not crash)
    bool success = config.loadFromFile("/nonexistent/file.txt");
    // Should return false for non-existent file
    assert(!success);
    
    // Test loading from file with invalid format (should not crash)
    // Create a temporary file with invalid content
    std::ofstream tempFile("temp_config.txt");
    tempFile << "invalid line without equals" << std::endl;
    tempFile << "key=value" << std::endl;
    tempFile.close();
    
    success = config.loadFromFile("temp_config.txt");
    // Should succeed despite invalid line
    assert(success);
    
    // Check that valid key was loaded
    assert(config.has("key"));
    
    // Clean up
    std::remove("temp_config.txt");
    
    std::cout << "    testConfigLoadFromFile passed" << std::endl;
}

void testConfigLoadFromArgs() {
    nlm::Config config;
    
    // Test empty args
    bool success = config.loadFromArgs(1, nullptr);
    assert(success);
    
    // Test null argv
    success = config.loadFromArgs(0, nullptr);
    assert(success);
    
    // Test malformed args (should not crash)
    const char* argv[] = {"program", "--invalid"};
    success = config.loadFromArgs(2, const_cast<char**>(argv));
    // Should handle gracefully
    assert(success);
    
    // Test valid args
    const char* validArgs[] = {"program", "--test_key=test_value", "-another_key", "another_value"};
    success = config.loadFromArgs(4, const_cast<char**>(validArgs));
    assert(success);
    assert(config.has("test_key"));
    assert(config.has("another_key"));
    
    auto testVal = config.get<std::string>("test_key");
    assert(testVal.has_value() && *testVal == "test_value");
    
    auto anotherVal = config.get<std::string>("another_key");
    assert(anotherVal.has_value() && *anotherVal == "another_value");
    
    // Clean up
    std::remove("temp_config.txt");
    
    std::cout << "    testConfigLoadFromFile passed" << std::endl;
}

void testConfigLoadFromArgs() {
    nlm::Config config;
    
    // Test empty args
    bool success = config.loadFromArgs(1, nullptr);
    assert(success);
    
    // Test null argv
    success = config.loadFromArgs(0, nullptr);
    assert(success);
    
    // Test malformed args (should not crash)
    const char* argv[] = {"program", "--invalid"};
    success = config.loadFromArgs(2, const_cast<char**>(argv));
    // Should handle gracefully
    assert(success);
    
    // Test valid args
    const char* validArgs[] = {"program", "--test_key=test_value", "-another_key", "another_value"};
    success = config.loadFromArgs(4, const_cast<char**>(validArgs));
    assert(success);
    assert(config.has("test_key"));
    assert(config.has("another_key"));
    
    auto testVal = config.get<std::string>("test_key");
    assert(testVal.has_value() && *testVal == "test_value");
    
    auto anotherVal = config.get<std::string>("another_key");
    assert(anotherVal.has_value() && *anotherVal == "another_value");
    
    std::cout << "    testConfigLoadFromArgs passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigSetAndGet();
    testConfigGetOr();
    testConfigRemove();
    testConfigKeys();
    testConfigLoadFromFile();
    testConfigLoadFromArgs();
}
