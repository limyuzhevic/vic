#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_config_save_to_file {

void testSaveToFileWithDifferentTypes() {
    nlm::Config config;
    
    // Set various value types
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14159, nlm::ConfigSource::Runtime);
    config.set("string_value", "Hello, World!", nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    config.set("vector_int", std::vector<int>({1, 2, 3, 4, 5}), nlm::ConfigSource::Runtime);
    config.set("vector_double", std::vector<double>({0.1, 0.2, 0.3}), nlm::ConfigSource::Runtime);
    config.set("vector_string", std::vector<std::string>({"a", "b", "c"}), nlm::ConfigSource::Runtime);
    
    // Test summary (this uses the actual values now)
    std::string summary = config.summary();
    std::cout << "Configuration Summary:\n" << summary << std::endl;
    
    // Test saving to file
    const std::string testFile = "test_config.txt";
    bool saveSuccess = config.saveToFile(testFile);
    
    assert(saveSuccess && "Failed to save configuration to file");
    assert(std::filesystem::exists(testFile) && "File was not created");
    
    // Read the file back
    std::ifstream file(testFile);
    assert(file.is_open() && "Failed to read back the file");
    
    std::string line;
    std::getline(file, line); // Skip comment line for int_value
    
    // Check that actual values are in the file (not PLACEHOLDER_VALUE)
    assert(line.find("int_value = 42") != std::string::npos && "int_value placeholder found");
    assert(line.find("double_value = 3.14159") != std::string::npos && "double_value placeholder found");
    assert(line.find("string_value = \"Hello, World!\"") != std::string::npos && "string_value placeholder found");
    assert(line.find("bool_value = 1") != std::string::npos && "bool_value placeholder found");
    
    std::getline(file, line); // Skip comment line for double_value
    assert(line.find("double_value = 3.14159") != std::string::npos && "double_value placeholder found");
    
    // Clean up
    file.close();
    std::filesystem::remove(testFile);
    
    std::cout << "    testSaveToFileWithDifferentTypes passed" << std::endl;
}

void runAllTests() {
    testSaveToFileWithDifferentTypes();
    // Add more tests here
}

} // namespace test_config_save_to_file

int main() {
    test_config_save_to_file::runAllTests();
    return 0;
}