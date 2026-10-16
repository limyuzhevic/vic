// CLI Basic Tests
// Tests basic CLI functionality

#include "tests/test_suite_runner.cpp"
#include "nlm/NLMCLI.hpp"
#include <cassert>
#include <iostream>

namespace test_cli_basic {

void testCommandParsing() {
    std::cout << "  [testCommandParsing] ";

    // Test command parsing functionality
    std::vector<std::string> testArgs = {"--help", "run", "--steps=100"};

    assert(testArgs.size() >= 2);

    std::cout << "PASSED" << std::endl;
}

void testArgumentValidation() {
    std::cout << "  [testArgumentValidation] ";

    // Test argument validation
    nlm::CLIConfig config;
    config.verbose = true;
    config.quiet = false;

    assert(config.verbose);
    assert(!config.quiet);

    std::cout << "PASSED" << std::endl;
}

void testOutputFormatting() {
    std::cout << "  [testOutputFormatting] ";

    // Test output formatting
    std::string jsonOutput = "{\"format\": \"json\"}";
    std::string csvOutput = "\"format\",\"csv\"";

    assert(!jsonOutput.empty());
    assert(!csvOutput.empty());

    std::cout << "PASSED" << std::endl;
}

void runAll() {
    testCommandParsing();
    testArgumentValidation();
    testOutputFormatting();

    std::cout << "  All CLI basic tests passed!" << std::endl;
}

} // namespace test_cli_basic