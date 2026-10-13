// Config Tests
// Tests for the Configuration system

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config {

void testConfigCreation() {
    std::cout << "Testing Config creation..." << std::endl;
    
    nlm::Config config;
    
    // Test basic set/get operations
    config.set("test_key", "test_value");
    assert(config.has("test_key"));
    assert(config.getOr<std::string>("test_key", "") == "test_value");
    
    // Test integer
    config.set("test_int", 42);
    assert(config.getOr<int>("test_int", 0) == 42);
    
    // Test double
    config.set("test_double", 3.14);
    assert(std::abs(config.getOr<double>("test_double", 0.0) - 3.14) < 0.001);
    
    // Test boolean
    config.set("test_bool", true);
    assert(config.getOr<bool>("test_bool", false) == true);
    
    std::cout << "    Config creation test passed" << std::endl;
}

void testConfigLoadFromFile() {
    std::cout << "Testing Config load from file..." << std::endl;
    
    // Create a temporary config file
    std::string temp_filename = "/tmp/test_config.cfg";
    std::ofstream file(temp_filename);
    
    file << "# Test config file\n";
    file << "key1 = value1\n";
    file << "key2 = 42\n";
    file << "key3 = 3.14\n";
    file << "key4 = true\n";
    file.close();
    
    // Test loading
    nlm::Config config;
    assert(config.loadFromFile(temp_filename));
    
    assert(config.getOr<std::string>("key1", "") == "value1");
    assert(config.getOr<int>("key2", 0) == 42);
    assert(std::abs(config.getOr<double>("key3", 0.0) - 3.14) < 0.001);
    assert(config.getOr<bool>("key4", false) == true);
    
    // Test loading non-existent file
    assert(!config.loadFromFile("/tmp/nonexistent.cfg"));
    
    // Clean up
    std::remove(temp_filename.c_str());
    
    std::cout << "    Config load from file test passed" << std::endl;
}

void testConfigLoadFromArgs() {
    std::cout << "Testing Config load from args..." << std::endl;
    
    nlm::Config config;
    
    // Test --key=value format
    char* argv[] = {
        (char*)"program",
        (char*)"--key1=value1",
        (char*)"--key2=42",
        (char*)"--key3=3.14",
        (char*)"--key4=true"
    };
    
    assert(config.loadFromArgs(5, argv));
    
    assert(config.getOr<std::string>("key1", "") == "value1");
    assert(config.getOr<int>("key2", 0) == 42);
    assert(std::abs(config.getOr<double>("key3", 0.0) - 3.14) < 0.001);
    assert(config.getOr<bool>("key4", false) == true);
    
    // Test -key value format
    nlm::Config config2;
    char* argv2[] = {
        (char*)"program",
        (char*)"-key1", (char*)"value1",
        (char*)"-key2", (char*)"42",
        (char*)"-key3", (char*)"3.14",
        (char*)"-key4", (char*)"true"
    };
    
    assert(config2.loadFromArgs(9, argv2));
    
    assert(config2.getOr<std::string>("key1", "") == "value1");
    assert(config2.getOr<int>("key2", 0) == 42);
    assert(std::abs(config2.getOr<double>("key3", 0.0) - 3.14) < 0.001);
    assert(config2.getOr<bool>("key4", false) == true);
    
    std::cout << "    Config load from args test passed" << std::endl;
}

void testConfigSaveToFile() {
    std::cout << "Testing Config save to file..." << std::endl;
    
    nlm::Config config;
    config.set("test_key", "test_value");
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_bool", true);
    
    std::string temp_filename = "/tmp/test_save_config.cfg";
    assert(config.saveToFile(temp_filename));
    
    // Verify the file was created and has content
    std::ifstream file(temp_filename);
    assert(file.good());
    
    // Read and verify content (basic check)
    std::string line;
    bool found_test_key = false;
    while (std::getline(file, line)) {
        if (line.find("test_key") != std::string::npos) {
            found_test_key = true;
            break;
        }
    }
    assert(found_test_key);
    
    file.close();
    std::remove(temp_filename.c_str());
    
    std::cout << "    Config save to file test passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing Config summary..." << std::endl;
    
    nlm::Config config;
    config.set("key1", "value1");
    config.set("key2", 42);
    config.set("key3", 3.14);
    
    std::string summary = config.summary();
    
    // Basic checks on summary
    assert(summary.find("Configuration") != std::string::npos);
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    assert(summary.find("key3") != std::string::npos);
    
    std::cout << "    Config summary test passed" << std::endl;
}

void testConfigRemove() {
    std::cout << "Testing Config remove..." << std::endl;
    
    nlm::Config config;
    config.set("key1", "value1");
    config.set("key2", "value2");
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.remove("key1");
    assert(!config.has("key1"));
    assert(config.has("key2"));
    
    config.remove("key2");
    assert(!config.has("key2"));
    
    std::cout << "    Config remove test passed" << std::endl;
}

void testConfigClear() {
    std::cout << "Testing Config clear..." << std::endl;
    
    nlm::Config config;
    config.set("key1", "value1");
    config.set("key2", "value2");
    
    assert(config.has("key1"));
    assert(config.has("key2"));
    
    config.clear();
    assert(!config.has("key1"));
    assert(!config.has("key2"));
    assert(config.getKeys().empty());
    
    std::cout << "    Config clear test passed" << std::endl;
}

void testConfigKeys() {
    std::cout << "Testing Config keys..." << std::endl;
    
    nlm::Config config;
    config.set("key1", "value1");
    config.set("key2", "value2");
    config.set("key3", "value3");
    
    std::vector<std::string> keys = config.getKeys();
    assert(keys.size() == 3);
    
    // Check that all keys are present
    bool has_key1 = false, has_key2 = false, has_key3 = false;
    for (const std::string& key : keys) {
        if (key == "key1") has_key1 = true;
        if (key == "key2") has_key2 = true;
        if (key == "key3") has_key3 = true;
    }
    
    assert(has_key1 && has_key2 && has_key3);
    
    std::cout << "    Config keys test passed" << std::endl;
}

void runAll() {
    testConfigCreation();
    testConfigLoadFromFile();
    testConfigLoadFromArgs();
    testConfigSaveToFile();
    testConfigSummary();
    testConfigRemove();
    testConfigClear();
    testConfigKeys();
    
    std::cout << std::endl;
    std::cout << "All Config tests passed!" << std::endl;
}

} // namespace test_config