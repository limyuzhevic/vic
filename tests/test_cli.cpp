// Command-Line Interface Tests
// Tests command-line interface parsing and execution

#include "core/CLI/CLIParsed.hpp"
#include "core/CLI/CLIValidator.hpp"
#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <string>

namespace test_cli {

void testCLIParsing() {
    nlm::CLIParser parser;
    
    auto args = parser.parse({
        "--config", "test_config.json",
        "--iterations", "100",
        "--verbose",
        "--load-level", "5"
    });
    
    assert(args.configFile == "test_config.json");
    assert(args.iterations == 100);
    assert(args.verbose == true);
    assert(args.loadLevel == 5);
    
    std::cout << "    testCLIParsing passed" << std::endl;
}

void testCLIValidation() {
    nlm::CLIValidator validator;
    
    // Test valid commands
    assert(validator.validateCommand("run", {
        {"--config", "file.json"},
        {"--iterations", "50"}
    }));
    
    assert(validator.validateCommand("test", {
        {"--test-mode", "unit"},
        {"--output", "json"}
    }));
    
    // Test invalid commands
    assert(!validator.validateCommand("run", {}));
    assert(!validator.validateCommand("invalid_command", {}));
    
    // Test invalid arguments
    assert(!validator.validateCommand("run", {
        {"--config", "file.txt"} // Invalid extension
    }));
    
    std::cout << "    testCLIValidation passed" << std::endl;
}

void testCLIDefaults() {
    nlm::CLIParser parser;
    
    // Test with minimal arguments
    auto args = parser.parse({"--iterations", "10"});
    
    assert(args.iterations == 10);
    // Default values should be applied
    assert(args.verbose == false);
    
    // Test with all arguments
    auto args2 = parser.parse({
        "--config", "default_config.json",
        "--iterations", "500",
        "--verbose",
        "--load-level", "3",
        "--precision", "double",
        "--memory-limit", "8"
    });
    
    assert(args2.configFile == "default_config.json");
    assert(args2.iterations == 500);
    assert(args2.verbose == true);
    assert(args2.loadLevel == 3);
    assert(args2.precision == "double");
    assert(args2.memoryLimit == 8);
    
    std::cout << "    testCLIDefaults passed" << std::endl;
}

void testCLISpecialArguments() {
    nlm::CLIParser parser;
    
    // Test array arguments
    auto args1 = parser.parse({
        "--feature", "attention",
        "--feature", "prediction",
        "--feature", "working_memory"
    });
    
    // Test flag arguments
    auto args2 = parser.parse({
        "--enable-feature", "attention",
        "--enable-feature", "prediction",
        "--disable-feature", "working_memory"
    });
    
    // Test numeric ranges
    auto args3 = parser.parse({
        "--learning-rate", "0.001",
        "--batch-size", "32"
    });
    
    assert(std::abs(args3.learningRate - 0.001f) < 0.0001f);
    assert(args3.batchSize == 32);
    
    std::cout << "    testCLISpecialArguments passed" << std::endl;
}

void testCLIErrorHandling() {
    nlm::CLIValidator validator;
    
    // Test error reporting
    auto errors = validator.validateCommand("run", {
        {"--invalid-flag", "value"},
        {"--missing-required", ""}
    });
    
    assert(!errors.empty());
    assert(validator.getErrorCount() > 0);
    
    // Test help generation
    auto helpText = validator.generateHelp("run");
    assert(!helpText.empty());
    
    std::cout << "    testCLIErrorHandling passed" << std::endl;
}

void runAll() {
    testCLIParsing();
    testCLIValidation();
    testCLIDefaults();
    testCLISpecialArguments();
    testCLIErrorHandling();
}

} // namespace test_cli