#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <cassert>

int main() {
    nlm::Config config;
    
    // Set test values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    // Save to file
    bool saved = config.saveToFile("/tmp/test_config.json");
    assert(saved);
    
    // Read back the file and check values
    std::ifstream file("/tmp/test_config.json");
    std::string line;
    bool found_int = false;
    bool found_double = false;
    bool found_string = false;
    bool found_bool = false;
    
    while (std::getline(file, line)) {
        if (line.find("int_value = 42") != std::string::npos) {
            found_int = true;
        }
        if (line.find("double_value = 3.14") != std::string::npos) {
            found_double = true;
        }
        if (line.find("string_value = \"hello\"") != std::string::npos) {
            found_string = true;
        }
        if (line.find("bool_value = true") != std::string::npos) {
            found_bool = true;
        }
    }
    
    assert(found_int, "int_value not found or has PLACEHOLDER_VALUE");
    assert(found_double, "double_value not found or has PLACEHOLDER_VALUE");
    assert(found_string, "string_value not found or has PLACEHOLDER_VALUE");
    assert(found_bool, "bool_value not found or has PLACEHOLDER_VALUE");
    
    std::cout << "All saveToFile tests passed!" << std::endl;
    return 0;
}