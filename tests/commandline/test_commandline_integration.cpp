// Command-line integration tests
// Tests for end-to-end command-line parsing and configuration integration

#include "core/CommandLine/CommandLineParser.hpp"
#include "core/CommandLine/CommandLineOptions.hpp"
#include "core/CommandLine/CommandLineValidation.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

namespace test_commandline_integration {

void testCommandLineToConfigIntegration() {
    // Test that command line arguments properly update Config
    nlm::Config config;
    
    // Add some configuration entries
    config.set("existing_key", "existing_value", nlm::ConfigSource::File);
    
    // Parse command line arguments
    nlm::CommandLineParser parser;
    
    // Add test arguments
    nlm::CommandLineArg arg1("config_key", "Config key", false, false);
    nlm::CommandLineArg arg2("number_value", "Number value", false, false);
    parser.addArgument(arg1);
    parser.addArgument(arg2);
    
    // Simulate command line: --config_key=test --number_value=42
    int argc = 5;
    char* argv[] = {
        (char*)"program",
        (char*)"--config_key=test_value",
        (char*)"-number_value",
        (char*)"42",
        NULL
    };
    
    bool parseSuccess = parser.parse(argc, argv);
    assert(parseSuccess);
    assert(parser.has("config_key"));
    assert(parser.has("number_value"));
    
    // Apply to config
    parser.applyToConfig(config);
    
    // Check that values were applied
    assert(config.has("config_key"));
    assert(config.has("number_value"));
    
    // Note: Config string parsing would need to be implemented
    // For now, this is a basic structure test
    
    std::cout << "    testCommandLineToConfigIntegration passed" << std::endl;
}

void testCommandLineValidationIntegration() {
    // Test that command line validation works with argument parsing
    nlm::CommandLineParser parser;
    nlm::CommandLineValidation validator;
    
    // Add arguments
    nlm::CommandLineArg arg1("count", "Count", false, false);
    nlm::CommandLineArg arg2("name", "Name", false, false);
    parser.addArgument(arg1);
    parser.addArgument(arg2);
    
    // Parse command line
    int argc = 5;
    char* argv[] = {
        (char*)"program",
        (char*)"--count",
        (char*)"5",
        (char*)"--name",
        (char*)"test",
        NULL
    };
    
    bool parseSuccess = parser.parse(argc, argv);
    assert(parseSuccess);
    
    // Validate arguments
    bool validationSuccess = validator.validate(parser.getAllArgs());
    assert(validationSuccess);
    
    // Test with invalid arguments (would need custom validation rules)
    
    std::cout << "    testCommandLineValidationIntegration passed" << std::endl;
}

void testOptionCategoryHelp() {
    nlm::CommandLineOptions options;
    options.initializeAllOptions();
    
    // Test that help can be generated for each category
    auto categories = options.getActiveCategories();
    assert(!categories.empty());
    
    // This is mainly to ensure no crashes
    options.displayHelpByCategory();
    
    std::cout << "    testOptionCategoryHelp passed" << std::endl;
}

void runAll() {
    testCommandLineToConfigIntegration();
    testCommandLineValidationIntegration();
    testOptionCategoryHelp();
}

} // namespace test_commandline_integration