// CommandLineParser Tests
#include "core/CommandLineParser/CommandLineParser.hpp"
#include <cassert>
#include <iostream>

namespace test_command_line_parser {

void testCommandLineParserCreation() {
    nlm::CommandLineParser parser("test_program");
    
    // Add some test options
    parser.addOption("verbose", "Enable verbose output", false, false);
    parser.addOption("count", "n", "Number of items", false, true, "10", "int", "--count 5");
    
    std::cout << "    testCommandLineParserCreation passed" << std::endl;
}

void testCommandLineParserHelp() {
    nlm::CommandLineParser parser("test_program");
    
    parser.addOption("verbose", "v", "Enable verbose output", false, false);
    parser.addOption("output", "o", "Output file", false, true, "out.txt", "string", "--output results.txt");
    
    // Test help doesn't crash
    parser.printHelp();
    
    std::cout << "    testCommandLineParserHelp passed" << std::endl;
}

void runAll() {
    testCommandLineParserCreation();
    testCommandLineParserHelp();
}

} // namespace test_command_line_parser
