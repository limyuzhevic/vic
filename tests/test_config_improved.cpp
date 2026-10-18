#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include "core/Config/Config.hpp"

using namespace nlm;

void testConfigTypeConversion() {
    std::cout << "Testing Config type conversion..." << std::endl;
    
    Config config;
    
    // Test integer conversion from string
    config.set("int_value", "42", ConfigSource::Runtime);
    assert(config.has("int_value"));
    auto intVal = config.get<int64_t>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    std::cout << "  ✓ Integer conversion passed" << std::endl;
    
    // Test double conversion from string
    config.set("double_value", "3.14159", ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.14 && *doubleVal < 3.15);
    std::cout << "  ✓ Double conversion passed" << std::endl;
    
    // Test boolean conversion from string
    config.set("bool_true", "true", ConfigSource::Runtime);
    config.set("bool_false", "false", ConfigSource::Runtime);
    auto boolTrue = config.get<bool>("bool_true");
    auto boolFalse = config.get<bool>("bool_false");
    assert(boolTrue.has_value() && *boolTrue == true);
    assert(boolFalse.has_value() && *boolFalse == false);
    std::cout << "  ✓ Boolean conversion passed" << std::endl;
    
    // Test int list conversion
    config.set("int_list", "[1, 2, 3, 4, 5]", ConfigSource::Runtime);
    auto intList = config.get<std::vector<int>>("int_list");
    assert(intList.has_value());
    assert(intList->size() == 5);
    assert((*intList)[0] == 1 && (*intList)[4] == 5);
    std::cout << "  ✓ Int list conversion passed" << std::endl;
    
    // Test string preservation
    config.set("string_val", "hello world", ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_val");
    assert(stringVal.has_value());
    assert(*stringVal == "hello world");
    std::cout << "  ✓ String preservation passed" << std::endl;
    
    std::cout << "All type conversion tests passed!" << std::endl << std::endl;
}

void testConfigFileIO() {
    std::cout << "Testing Config file I/O..." << std::endl;
    
    // Create config with various types
    Config config;
    config.set("integer", 42, ConfigSource::Runtime);
    config.set("double", 3.14159, ConfigSource::Runtime);
    config.set("boolean", true, ConfigSource::Runtime);
    config.set("text", "Hello World", ConfigSource::Runtime);
    config.set("list", std::vector<int>({1, 2, 3, 4, 5}), ConfigSource::Runtime);
    config.set("description", "Test configuration file", ConfigSource::Runtime);
    
    // Save to file
    std::string filename = "test_config.cfg";
    bool saveSuccess = config.saveToFile(filename);
    assert(saveSuccess);
    std::cout << "  ✓ File save successful" << std::endl;
    
    // Verify file exists and has content
    assert(std::filesystem::exists(filename));
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Skip first comment line
    assert(line.find("integer = 42") != std::string::npos);
    assert(line.find("double = 3.14159") != std::string::npos);
    assert(line.find("boolean = true") != std::string::npos);
    assert(line.find("text = \"Hello World\"") != std::string::npos);
    std::cout << "  ✓ File content verified" << std::endl;
    
    // Clean up
    std::filesystem::remove(filename);
    
    std::cout << "All file I/O tests passed!" << std::endl << std::endl;
}

void testConfigCommandLineParsing() {
    std::cout << "Testing Config command-line parsing..." << std::endl;
    
    Config config;
    
    // Simulate command line arguments
    const char* argv[] = {"test", "--int_val=123", "--str_val=hello", "--flag=true", "-bool=false"};
    int argc = 6;
    
    config.loadFromArgs(argc, const_cast<char**>(argv));
    
    // Check parsed values
    assert(config.has("int_val"));
    assert(config.has("str_val"));
    assert(config.has("flag"));
    assert(config.has("bool"));
    
    auto intVal = config.get<int64_t>("int_val");
    auto strVal = config.get<std::string>("str_val");
    auto flagVal = config.get<bool>("flag");
    auto boolVal = config.get<bool>("bool");
    
    assert(intVal.has_value() && *intVal == 123);
    assert(strVal.has_value() && *strVal == "hello");
    assert(flagVal.has_value() && *flagVal == true);
    assert(boolVal.has_value() && *boolVal == false);
    
    std::cout << "  ✓ Command-line parsing passed" << std::endl;
    std::cout << "All command-line parsing tests passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                     Config Class Tests                          ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
    
    testConfigTypeConversion();
    testConfigFileIO();
    testConfigCommandLineParsing();
    
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                    All Config Tests Passed!                      ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    
    return 0;
}
