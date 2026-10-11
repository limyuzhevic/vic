// Debug Config tests focusing on loadFromFile functionality
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <filesystem>
#include <fstream>

void testSimpleFormat() {
    nlm::Config config;
    
    // Create a test config file
    std::string testFile = "test_simple_debug.cfg";
    std::ofstream file(testFile);
    file << "key1 = value1\n";
    file << "key2 = 42\n";
    file << "key3 = true\n";
    file << "key4 = \"quoted value\"\n";
    file.close();
    
    // Load the file
    bool result = config.loadFromFile(testFile);
    std::cout << "Load result: " << (result ? "true" : "false") << std::endl;
    
    // Debug: show summary
    std::cout << "Config summary:\n" << config.summary() << std::endl;
    
    // Verify loaded values
    assert(config.has("key1"));
    assert(config.has("key2"));
    assert(config.has("key3"));
    assert(config.has("key4"));
    
    // Check values
    auto val1 = config.get<std::string>("key1");
    std::cout << "key1: has_value = " << (val1.has_value() ? "true" : "false");
    if (val1.has_value()) std::cout << ", value = \"" << *val1 << "\"";
    std::cout << std::endl;
    
    auto val2 = config.get<int>("key2");
    std::cout << "key2: has_value = " << (val2.has_value() ? "true" : "false");
    if (val2.has_value()) std::cout << ", value = " << *val2;
    std::cout << std::endl;
    
    auto val3 = config.get<bool>("key3");
    std::cout << "key3: has_value = " << (val3.has_value() ? "true" : "false");
    if (val3.has_value()) std::cout << ", value = " << (*val3 ? "true" : "false");
    std::cout << std::endl;
    
    auto val4 = config.get<std::string>("key4");
    std::cout << "key4: has_value = " << (val4.has_value() ? "true" : "false");
    if (val4.has_value()) std::cout << ", value = \"" << *val4 << "\"";
    std::cout << std::endl;
    
    // Cleanup
    std::filesystem::remove(testFile);
    
    std::cout << "    testSimpleFormat passed" << std::endl;
}

int main() {
    std::cout << "=== Debug Config Tests ===" << std::endl;
    testSimpleFormat();
    std::cout << std::endl;
    std::cout << "=== All Tests PASSED ===" << std::endl;
    return 0;
}
