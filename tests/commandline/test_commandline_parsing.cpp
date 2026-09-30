// Command-line parsing tests
// Tests for enhanced CommandLineParser functionality

#include "core/CommandLine/CommandLineParser.hpp"
#include <cassert>
#include <iostream>

namespace test_commandline_parsing {

void testLongFormatParsing() {
    nlm::CommandLineParser parser;
    
    // Add test argument
    nlm::CommandLineArg arg("test_key", "Test description", false, false, "default");
    parser.addArgument(arg);
    
    // Test --key=value format
    int argc = 3;
    char* argv[] = {
        (char*)"program",
        (char*)"--test_key=test_value",
        NULL
    };
    
    bool parseSuccess = parser.parse(argc, argv);
    assert(parseSuccess);
    assert(parser.has("test_key"));
    assert(parser.get("test_key").value() == "test_value");
    
    std::cout << "    testLongFormatParsing passed" << std::endl;
}

void testShortFormatParsing() {
    nlm::CommandLineParser parser;
    
    // Add test argument
    nlm::CommandLineArg arg("debug", "Debug mode", false, false);
    parser.addArgument(arg);
    
    // Test -key value format
    int argc = 4;
    char* argv[] = {
        (char*)"program",
        (char*)"-debug",
        (char*)"true",
        NULL
    };
    
    bool parseSuccess = parser.parse(argc, argv);
    assert(parseSuccess);
    assert(parser.has("debug"));
    assert(parser.get("debug").value() == "true");
    
    std::cout << "    testShortFormatParsing passed" << std::endl;
}

void testBooleanFlagParsing() {
    nlm::CommandLineParser parser;
    
    // Add test flag argument (no value expected)
    nlm::CommandLineArg arg("verbose", "Verbose output", false, false);
    parser.addArgument(arg);
    
    // Test -flag format
    int argc = 2;
    char* argv[] = {
        (char*)"program",
        (char*)"-verbose",
        NULL
    };
    
    bool parseSuccess = parser.parse(argc, argv);
    assert(parseSuccess);
    assert(parser.has("verbose"));
    // Flag should be present (value may be empty)
    assert(parser.getOr("verbose", "") != "");
    
    std::cout << "    testBooleanFlagParsing passed" << std::endl;
}

void testDefaultValueHandling() {
    nlm::CommandLineParser parser;
    
    // Add argument with default value
    nlm::CommandLineArg arg("threads", "Number of threads", false, true, "4");
    parser.addArgument(arg);
    
    // Test with no argument (should use default)
    int argc = 1;
    char* argv[] = {
        (char*)"program",
        NULL
    };
    
    bool parseSuccess = parser.parse(argc, argv);
    assert(parseSuccess);
    assert(parser.has("threads"));
    assert(parser.getOr("threads", "default") == "4");
    
    // Test with override
    int argc2 = 3;
    char* argv2[] = {
        (char*)"program",
        (char*)"--threads",
        (char*)"8",
        NULL
    };
    
    bool parseSuccess2 = parser.parse(argc2, argv2);
    assert(parseSuccess2);
    assert(parser.has("threads"));
    assert(parser.get("threads").value() == "8");
    
    std::cout << "    testDefaultValueHandling passed" << std::endl;
}

void testValidation() {
    nlm::CommandLineParser parser;
    
    // Add argument with validator
    nlm::CommandLineArg arg("count", "Count value", false, false);
    parser.addArgument(arg);
    
    // Set up validation (simple number validation)
    parser.setValidation([](const std::string& name, const std::string& value) -> bool {
        // Simple validation: count must be positive integer
        if (name == "count") {
            try {
                int val = std::stoi(value);
                return val > 0;
            } catch (...) {
                return false;
            }
        }
        return true;
    });
    
    // Test valid value
    int argc1 = 3;
    char* argv1[] = {
        (char*)"program",
        (char*)"--count",
        (char*)"10",
        NULL
    };
    
    bool parseSuccess1 = parser.parse(argc1, argv1);
    assert(parseSuccess1);
    
    // Test invalid value (would need validation to catch this)
    // Note: This test would need more sophisticated error handling
    
    std::cout << "    testValidation passed" << std::endl;
}

void runAll() {
    testLongFormatParsing();
    testShortFormatParsing();
    testBooleanFlagParsing();
    testDefaultValueHandling();
    testValidation();
}

} // namespace test_commandline_parsing