// Test program to verify Config class implementation
#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include "Config.hpp"

void test_basic_operations() {
    std::cout << "Testing basic operations..." << std::endl;
    nlm::Config config;
    
    // Test set and get for different types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    assert(config.get<double>("double_value").value() > 3.13);
    assert(config.get<std::string>("string_value").value() == "hello");
    assert(config.get<bool>("bool_value").value() == true);
    
    std::cout << "  Basic operations: PASSED" << std::endl;
}

void test_get_or() {
    std::cout << "Testing getOr with defaults..." << std::endl;
    nlm::Config config;
    
    int defaultVal = config.getOr<int>("missing", 100);
    assert(defaultVal == 100);
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    int existingVal = config.getOr<int>("existing", 100);
    assert(existingVal == 50);
    
    std::cout << "  getOr with defaults: PASSED" << std::endl;
}

void test_remove_and_keys() {
    std::cout << "Testing remove and getKeys..." << std::endl;
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    
    config.remove("key2");
    keys = config.getKeys();
    assert(keys.size() == 2);
    assert(!config.has("key2"));
    
    std::cout << "  remove and getKeys: PASSED" << std::endl;
}

void test_simple_format() {
    std::cout << "Testing simple key=value format..." << std::endl;
    nlm::Config config;
    
    // Test loading from simple format
    std::string testFile = "/tmp/test_simple.cfg";
    std::ofstream file(testFile);
    file << "# Test config file\n";
    file << "int_value = 42\n";
    file << "double_value = 3.14\n";
    file << "string_value = \"hello world\"\n";
    file << "bool_value = true\n";
    file << "comment_line = value  # comment\n";
    file.close();
    
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    assert(config.has("double_value"));
    assert(config.get<double>("double_value").value() > 3.13);
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello world");
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    // Test saving
    bool saved = config.saveToFile("/tmp/test_simple_out.cfg");
    assert(saved);
    
    std::filesystem::remove(testFile);
    std::filesystem::remove("/tmp/test_simple_out.cfg");
    
    std::cout << "  Simple format: PASSED" << std::endl;
}

void test_json_format() {
    std::cout << "Testing JSON format..." << std::endl;
    nlm::Config config;
    
    // Create a JSON config file
    std::string testFile = "/tmp/test_json.cfg";
    std::ofstream file(testFile);
    file << "{\n";
    file << "  \"int_value\": 42,\n";
    file << "  \"double_value\": 3.14,\n";
    file << "  \"string_value\": \"hello\",\n";
    file << "  \"bool_value\": true,\n";
    file << "  \"array_value\": [1, 2, 3]\n";
    file << "}\n";
    file.close();
    
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    assert(config.has("double_value"));
    assert(config.get<double>("double_value").value() > 3.13);
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello");
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    std::filesystem::remove(testFile);
    
    std::cout << "  JSON format: PASSED" << std::endl;
}

void test_yaml_format() {
    std::cout << "Testing YAML format..." << std::endl;
    nlm::Config config;
    
    // Create a YAML config file
    std::string testFile = "/tmp/test_yaml.cfg";
    std::ofstream file(testFile);
    file << "int_value: 42\n";
    file << "double_value: 3.14\n";
    file << "string_value: hello\n";
    file << "bool_value: true\n";
    file << "array_value: [1, 2, 3]\n";
    file.close();
    
    bool loaded = config.loadFromFile(testFile);
    assert(loaded);
    
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    assert(config.has("double_value"));
    assert(config.get<double>("double_value").value() > 3.13);
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello");
    assert(config.has("bool_value"));
    assert(config.get<bool>("bool_value").value() == true);
    
    std::filesystem::remove(testFile);
    
    std::cout << "  YAML format: PASSED" << std::endl;
}

void test_command_line_args() {
    std::cout << "Testing command line argument parsing..." << std::endl;
    nlm::Config config;
    
    // Simulate command line args
    const char* argv[] = {
        "test",
        "--int_value=42",
        "--double_value=3.14",
        "--string_value=hello",
        "--bool_value=true",
        "-i", "100"
    };
    int argc = 6;
    
    bool loaded = config.loadFromArgs(argc, const_cast<char**>(argv));
    assert(loaded);
    
    assert(config.has("int_value"));
    assert(config.get<int>("int_value").value() == 42);
    assert(config.has("string_value"));
    assert(config.get<std::string>("string_value").value() == "hello");
    
    std::cout << "  Command line args: PASSED" << std::endl;
}

void test_summary() {
    std::cout << "Testing summary output..." << std::endl;
    nlm::Config config;
    
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", "value", nlm::ConfigSource::File);
    
    std::string summary = config.summary();
    assert(summary.find("Configuration (2 entries)") != std::string::npos);
    assert(summary.find("key1") != std::string::npos);
    assert(summary.find("key2") != std::string::npos);
    
    std::cout << "  Summary output: PASSED" << std::endl;
}

int main() {
    std::cout << "=== Config Class Test Suite ===" << std::endl;
    
    test_basic_operations();
    test_get_or();
    test_remove_and_keys();
    test_simple_format();
    test_json_format();
    test_yaml_format();
    test_command_line_args();
    test_summary();
    
    std::cout << "\n=== All Tests Passed! ===" << std::endl;
    return 0;
}
