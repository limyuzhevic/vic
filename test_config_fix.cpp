#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>

void testConfigSaveLoad() {
    std::cout << "Testing Config save/load functionality..." << std::endl;
    
    nlm::Config config;
    
    // Set various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", "hello world", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("int_array", std::vector<int64_t>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    config.set("double_array", std::vector<double>({1.1, 2.2, 3.3}), nlm::ConfigSource::Runtime);
    config.set("string_array", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Verify values are set correctly
    assert(config.has("int_value"));
    assert(config.get<int64_t>("int_value").value() == 42);
    assert(config.has("double_value"));
    assert(config.get<double>("double_value").value() == 3.14159);
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello world");
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    // Save to file
    const std::string testFile = "test_config_output.cfg";
    bool saveResult = config.saveToFile(testFile);
    assert(saveResult);
    
    // Check if file was created and has content
    assert(std::filesystem::exists(testFile));
    std::ifstream file(testFile);
    assert(file.good());
    
    // Read and verify file content
    std::string line;
    std::vector<std::string> keys;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            keys.push_back(key);
        }
    }
    
    // Verify all keys are present
    auto configKeys = config.getKeys();
    assert(keys.size() == configKeys.size());
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "✓ Config save/load test passed" << std::endl;
}

void testConfigLoadFromFile() {
    std::cout << "Testing Config load from file functionality..." << std::endl;
    
    // Create a test config file
    const std::string testFile = "test_config_input.cfg";
    std::ofstream file(testFile);
    file << "# Test configuration file\n";
    file << "# Integer value\n";
    file << "int_value = 100\n";
    file << "# Double value\n";
    file << "double_value = 2.71828\n";
    file << "# String value\n";
    file << "string_value = \"test string\"\n";
    file << "# Boolean value\n";
    file << "bool_value = true\n";
    file << "# Integer array\n";
    file << "int_array = [1, 2, 3]\n";
    file << "# String array\n";
    file << "string_array = [\"a\", \"b\", \"c\"]\n";
    file.close();
    
    nlm::Config config;
    bool loadResult = config.loadFromFile(testFile);
    assert(loadResult);
    
    // Verify values
    assert(config.has("int_value"));
    assert(config.get<int64_t>("int_value").value() == 100);
    assert(config.has("double_value"));
    assert(config.get<double>("double_value").value() == 2.71828);
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "test string");
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    // Clean up
    std::remove(testFile.c_str());
    
    std::cout << "✓ Config load from file test passed" << std::endl;
}

void testConfigSummary() {
    std::cout << "Testing Config summary functionality..." << std::endl;
    
    nlm::Config config;
    config.set("test_key1", "value1", nlm::ConfigSource::Runtime);
    config.set("test_key2", 123, nlm::ConfigSource::File);
    config.set("test_key3", 3.14, nlm::ConfigSource::CommandLine);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration (3 entries):") != std::string::npos);
    assert(summary.find("test_key1 = \"value1\"") != std::string::npos);
    assert(summary.find("test_key2 = 123") != std::string::npos);
    assert(summary.find("test_key3 = 3.14") != std::string::npos);
    
    std::cout << "✓ Config summary test passed" << std::endl;
}

int main() {
    std::cout << "=== NLM Configuration System Tests ===" << std::endl;
    
    try {
        testConfigSaveLoad();
        testConfigLoadFromFile();
        testConfigSummary();
        
        std::cout << std::endl;
        std::cout << "=== All Configuration Tests Passed! ===" << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Test failed with unknown exception" << std::endl;
        return 1;
    }
    
    return 0;
}