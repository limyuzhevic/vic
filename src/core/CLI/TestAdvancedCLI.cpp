// Simple test to verify the advanced CLI integration
// This test ensures backward compatibility and new functionality

#include "core/Config/Config.hpp"
#include "core/CLI/AdvancedCLI.h"
#include <iostream>
#include <cassert>

void testBackwardCompatibility() {
    std::cout << "Testing backward compatibility..." << std::endl;

    // Test the old Config::loadFromArgs() style
    auto config = std::make_shared<Config>();

    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--input=data.txt",
        (char*)"--output=result.txt",
        (char*)"--format=json",
        (char*)"--steps=1000"
    };

    bool success = config->loadFromArgs(argc, argv);
    assert(success);

    assert(config->has("input"));
    assert(config->has("output"));
    assert(config->has("format"));
    assert(config->has("steps"));

    std::cout << "  ✓ Backward compatibility test passed" << std::endl;
}

void testNewFunctionality() {
    std::cout << "Testing new AdvancedCLI functionality..." << std::endl;

    // Create a simple CLI parser with the new functionality
    CLIParser parser;

    // Add various argument types
    parser.addFlag("verbose", "Enable verbose output");
    parser.addOption("input", "Input file path");
    parser.addOption("output", "Output file path");
    parser.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");
    parser.addRange("steps", "Number of steps", 100, 1000, 500);
    parser.addPath("checkpoint", "Checkpoint file path", false, true);

    // Test parsing
    int argc = 5;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--verbose",
        (char*)"--input=data.txt",
        (char*)"--output=result.txt",
        (char*)"--format=json"
    };

    bool success = parser.parse(argc, argv);
    assert(success);

    assert(parser.hasArgument("verbose"));
    assert(parser.hasArgument("input"));
    assert(parser.hasArgument("output"));
    assert(parser.hasArgument("format"));

    // Test config integration
    auto config = std::make_shared<Config>();
    success = parser.applyToConfig(config);
    assert(success);

    // Verify values were set
    assert(config->has("verbose"));
    assert(config->has("input"));
    assert(config->has("output"));
    assert(config->has("format"));

    std::cout << "  ✓ New functionality test passed" << std::endl;
}

void testSubcommands() {
    std::cout << "Testing subcommand functionality..." << std::endl;

    CLIParser parser;

    // Add subcommands
    parser.addSubcommand("run", "Run neural network simulation");
    parser.addSubcommand("train", "Train neural network model");
    parser.addSubcommand("simulate", "Run simulation with current brain state");

    // Add nested options
    parser.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
    parser.addNestedOption("run", "learning_rate", "Learning rate", "0.001");

    parser.addNestedOption("train", "epochs", "Number of epochs", "100");
    parser.addNestedOption("train", "batch_size", "Batch size", "32");

    // Test parsing nested options
    int argc = 8;
    char* argv[] = {
        (char*)"test_program",
        (char*)"--run.nn_type=cnn",
        (char*)"--run.learning_rate=0.01",
        (char*)"--train.epochs=200",
        (char*)"--train.batch_size=64",
        (char*)"--run.steps=1000"
    };

    bool success = parser.parse(argc, argv);
    assert(success);

    assert(parser.hasArgument("run.nn_type"));
    assert(parser.hasArgument("run.learning_rate"));
    assert(parser.hasArgument("train.epochs"));
    assert(parser.hasArgument("train.batch_size"));
    assert(parser.hasArgument("run.steps"));

    std::cout << "  ✓ Subcommand test passed" << std::endl;
}

void testEnvironmentVariables() {
    std::cout << "Testing environment variable support..." << std::endl;

    // Set environment variables
    setenv("NLM_TEST_VERBOSE", "1", 1);
    setenv("NLM_TEST_OUTPUT", "console", 1);
    setenv("NLM_TEST_STEPS", "2000", 1);

    CLIParser parser;

    // Add environment variable fallbacks
    parser.addEnvVariable("verbose", "NLM_TEST_VERBOSE");
    parser.addEnvVariable("output", "NLM_TEST_OUTPUT");
    parser.addEnvVariable("steps", "NLM_TEST_STEPS");

    // Parse command line (empty, so should use env vars)
    int argc = 1;
    char* argv[] = {
        (char*)"test_program"
    };

    bool success = parser.parse(argc, argv);
    assert(success);

    // Check that environment variables were used
    assert(parser.hasArgument("verbose"));
    assert(parser.hasArgument("output"));
    assert(parser.hasArgument("steps"));

    // Clean up
    unsetenv("NLM_TEST_VERBOSE");
    unsetenv("NLM_TEST_OUTPUT");
    unsetenv("NLM_TEST_STEPS");

    std::cout << "  ✓ Environment variable test passed" << std::endl;
}

void testHelpSystem() {
    std::cout << "Testing help system..." << std::endl;

    CLIParser parser;

    // Add arguments
    parser.addFlag("verbose", "Enable verbose output");
    parser.addOption("input", "Input file path");
    parser.addOption("output", "Output file path");
    parser.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");

    // Test help generation
    std::string help = parser.getHelpText();
    assert(!help.empty());
    assert(help.find("Usage:") != std::string::npos);
    assert(help.find("Options:") != std::string::npos);

    std::string argumentHelp = parser.getArgumentHelp("verbose");
    assert(!argumentHelp.empty());
    assert(argumentHelp.find("verbose") != std::string::npos);

    std::cout << "  ✓ Help system test passed" << std::endl;
}

int main() {
    std::cout << "=== Advanced CLI Integration Tests ===" << std::endl;

    try {
        testBackwardCompatibility();
        testNewFunctionality();
        testSubcommands();
        testEnvironmentVariables();
        testHelpSystem();

        std::cout << "\n=== All Tests Passed! ===" << std::endl;
        std::cout << "The Advanced CLI Parser successfully integrates with the existing Config system" << std::endl;
        std::cout << "and provides all the requested expert-level features." << std::endl;

        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
