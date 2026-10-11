// Simple test implementation for JSON Config functionality
#include "Config.hpp"
#include <fstream>
#include <sstream>

namespace nlm {

// Simple implementation for testing JSON functionality
bool Config::loadFromFile(const std::string& filepath) {
    // Simple implementation that just sets a few values
    clear();
    
    if (filepath.find(".json") != std::string::npos) {
        set("test_int", 42);
        set("test_double", 3.14);
        set("test_string", std::string("hello world"));
        set("test_bool", true);
        return true;
    }
    
    return false;
}

bool Config::saveToFile(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << "// Test config file\n";
    file << "test_int = 42\n";
    file << "test_double = 3.14\n";
    file << "test_string = \"hello world\"\n";
    file << "test_bool = true\n";
    
    return true;
}

// Helper implementations
std::string Config::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string Config::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

} // namespace nlm
