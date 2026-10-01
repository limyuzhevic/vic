#include "CLI/CLI.hpp"
#include <iostream>
#include <cassert>

int main(int argc, char** argv) {
    // Test 1: Basic CLI parser creation
    nlm::CLIArgumentParser parser("test", "Test parser");
    parser.setVersion("1.0.0");
    
    // Test 2: Add argument
    parser.addArgument("test-arg", "t", "Test argument", "default", 
                     nlm::CLI::validateNonEmptyString, nullptr, false, "string");
    
    // Test 3: Add flag
    parser.addFlag("verbose", "Verbose output", nullptr);
    parser.addFlag("quiet", "Quiet output", nullptr);
    
    // Test 4: Parse with help
    int testArgs = 2;
    char* testArgv[] = {const_cast<char*>("test"), const_cast<char*>("--help")};
    
    std::cout << "=== Test CLI Framework ===" << std::endl;
    std::cout << "Testing argument parsing..." << std::endl;
    
    if (!parser.parse(testArgs, testArgv)) {
        std::cerr << "Failed to parse help argument" << std::endl;
        return 1;
    }
    
    std::cout << "Help requested: " << (parser.hasArgument("help") ? "YES" : "NO") << std::endl;
    
    // Test 5: Test with valid arguments
    char* validArgs[] = {
        const_cast<char*>("test"),
        const_cast<char*>("--test-arg"),
        const_cast<char*>("value1"),
        const_cast<char*>("--verbose")
    };
    
    std::cout << "\nTesting with valid arguments..." << std::endl;
    
    if (!parser.parse(4, validArgs)) {
        std::cerr << "Failed to parse valid arguments" << std::endl;
        return 1;
    }
    
    std::cout << "Argument --test-arg present: " << (parser.hasArgument("test-arg") ? "YES" : "NO") << std::endl;
    
    auto argOpt = parser.getArgument<std::string>("test-arg");
    if (argOpt) {
        std::cout << "Value: " << *argOpt << std::endl;
    }
    
    std::cout << "Verbose flag: " << (parser.getFlag("verbose") ? "YES" : "NO") << std::endl;
    
    // Test 6: Test --set functionality
    char* setArgs[] = {
        const_cast<char*>("test"),
        const_cast<char*>("--set"),
        const_cast<char*>("key=value")
    };
    
    std::cout << "\nTesting --set argument..." << std::endl;
    
    if (!parser.parse(3, setArgs)) {
        std::cerr << "Failed to parse --set argument" << std::endl;
        return 1;
    }
    
    std::cout << "All tests passed!" << std::endl;
    return 0;
}
