// Enhanced Argument Parser Tests
//
// Tests for the new enhanced ArgumentParser class

#include "core/ArgumentParser.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <fstream>
#include <filesystem>

namespace test_argument_parser {

void testArgumentParserCreation() {
    nlm::ArgumentParser parser;
    std::cout << "    testArgumentParserCreation passed" << std::endl;
}

void testOptionAddition() {
    nlm::ArgumentParser parser;
    
    // Add a custom option
    parser.addOption("--test-option", "-t", nlm::ArgumentParser::Option::Type::VALUE,
                     "Test option for testing", "default_value", "Testing", false);
    
    std::cout << "    testOptionAddition passed" << std::endl;
}

void testHelpDisplay() {
    nlm::ArgumentParser parser;
    
    // Capture help output (we can't easily capture stdout, so we just verify it doesn't crash)
    parser.displayHelp();
    
    std::cout << "    testHelpDisplay passed" << std::endl;
}

void testVersionDisplay() {
    nlm::ArgumentParser parser;
    
    // Capture version output (we can't easily capture stdout, so we just verify it doesn't crash)
    parser.displayVersion();
    
    std::cout << "    testVersionDisplay passed" << std::endl;
}

void testParsingHelp() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --help
    bool result = parser.parse(2, new char*[2]{(char*)"nlm", (char*)"--help"}, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingHelp passed" << std::endl;
}

void testParsingVersion() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --version
    bool result = parser.parse(2, new char*[2]{(char*)"nlm", (char*)"--version"}, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingVersion passed" << std::endl;
}

void testParsingConfigFile() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Create a temporary config file
    std::ofstream configFile("temp_test_config.cfg");
    configFile << "random_seed = 99" << std::endl;
    configFile << "neuron_count = 2000" << std::endl;
    configFile.close();
    
    // Test parsing --config
    char* args[] = {(char*)"nlm", (char*)"--config", (char*)"temp_test_config.cfg"};
    bool result = parser.parse(3, args, config);
    assert(result == true);  // Parsing should succeed
    
    // Clean up
    std::remove("temp_test_config.cfg");
    
    std::cout << "    testParsingConfigFile passed" << std::endl;
}

void testParsingJSONConfig() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Create a temporary JSON config file
    std::ofstream jsonFile("temp_test_config.json");
    jsonFile << "{\"random_seed\": 123, \"neuron_count\": 3000}" << std::endl;
    jsonFile.close();
    
    // Test parsing --json-config
    char* args[] = {(char*)"nlm", (char*)"--json-config", (char*)"temp_test_config.json"};
    bool result = parser.parse(3, args, config);
    assert(result == true);  // Parsing should succeed
    
    // Clean up
    std::remove("temp_test_config.json");
    
    std::cout << "    testParsingJSONConfig passed" << std::endl;
}

void testParsingConfigValue() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --config-value
    char* args[] = {(char*)"nlm", (char*)"--config-value", (char*)"random_seed=456"};
    bool result = parser.parse(3, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingConfigValue passed" << std::endl;
}

void testParsingConfigObject() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --config-object
    char* args[] = {(char*)"nlm", (char*)"--config-object", (char*)"brain.neuron_count=1000"};
    bool result = parser.parse(3, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingConfigObject passed" << std::endl;
}

void testParsingPreset() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --preset
    char* args[] = {(char*)"nlm", (char*)"--preset", (char*)"research"};
    bool result = parser.parse(3, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingPreset passed" << std::endl;
}

void testParsingVerbose() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --verbose
    char* args[] = {(char*)"nlm", (char*)"--verbose"};
    bool result = parser.parse(2, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingVerbose passed" << std::endl;
}

void testParsingQuiet() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --quiet
    char* args[] = {(char*)"nlm", (char*)"--quiet"};
    bool result = parser.parse(2, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingQuiet passed" << std::endl;
}

void testParsingListConfigs() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --list-configs
    char* args[] = {(char*)"nlm", (char*)"--list-configs"};
    bool result = parser.parse(2, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingListConfigs passed" << std::endl;
}

void testParsingShowConfig() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --show-config
    char* args[] = {(char*)"nlm", (char*)"--show-config"};
    bool result = parser.parse(2, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingShowConfig passed" << std::endl;
}

void testParsingValidateConfig() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --validate-config
    char* args[] = {(char*)"nlm", (char*)"--validate-config"};
    bool result = parser.parse(2, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingValidateConfig passed" << std::endl;
}

void testParsingDryRun() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --dry-run
    char* args[] = {(char*)"nlm", (char*)"--dry-run"};
    bool result = parser.parse(2, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingDryRun passed" << std::endl;
}

void testParsingExportConfig() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --export-config
    char* args[] = {(char*)"nlm", (char*)"--export-config", (char*)"test_export.cfg"};
    bool result = parser.parse(3, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingExportConfig passed" << std::endl;
}

void testParsingMergeConfigs() {
    nlm::ArgumentParser parser;
    nlm::Config config;
    
    // Test parsing --merge-configs
    char* args[] = {(char*)"nlm", (char*)"--merge-configs", (char*)"config1.cfg,config2.cfg"};
    bool result = parser.parse(3, args, config);
    assert(result == true);  // Parsing should succeed
    
    std::cout << "    testParsingMergeConfigs passed" << std::endl;
}

void runAll() {
    testArgumentParserCreation();
    testOptionAddition();
    testHelpDisplay();
    testVersionDisplay();
    testParsingHelp();
    testParsingVersion();
    testParsingConfigFile();
    testParsingJSONConfig();
    testParsingConfigValue();
    testParsingConfigObject();
    testParsingPreset();
    testParsingVerbose();
    testParsingQuiet();
    testParsingListConfigs();
    testParsingShowConfig();
    testParsingValidateConfig();
    testParsingDryRun();
    testParsingExportConfig();
    testParsingMergeConfigs();
    
    std::cout << std::endl << "=== All Argument Parser Tests PASSED ===" << std::endl;
}

} // namespace test_argument_parser
