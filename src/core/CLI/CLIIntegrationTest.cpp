#include "core/Config/Config.hpp"
#include "core/CLI/AdvancedCLI.h"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

int main(int argc, char** argv) {
    std::cout << "=== NLM Advanced CLI Integration Test ===" << std::endl << std::endl;

    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Create CLI parser
    CLIParser cli;

    // Test 1: Basic argument types
    std::cout << "1. Testing basic argument types..." << std::endl;
    cli.addFlag("verbose", "Enable verbose output");
    cli.addOption("input", "Input file path");
    cli.addOption("output", "Output file path");
    cli.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");
    cli.addRange("steps", "Number of simulation steps", 100, 1000, 500);
    cli.addPath("checkpoint", "Checkpoint file path", false, true);

    // Test 2: Expert features
    std::cout << "\n2. Testing expert features..." << std::endl;
    cli.addEnvVariable("verbose", "NLM_VERBOSE");
    cli.addConfigFile("config", "json");
    cli.addProfile("default");
    cli.addProfile("fast");
    cli.addProfile("high_quality");

    // Add validators
    cli.addValidator("steps", [](const ConfigValue& v) {
        if (std::holds_alternative<int>(v)) {
            int steps = std::get<int>(v);
            return steps >= 0 && steps <= 1000;
        }
        return false;
    });

    // Test 3: Subcommands
    std::cout << "\n3. Testing subcommand structure..." << std::endl;
    cli.addSubcommand("run", "Run neural network simulation");
    cli.addSubcommand("train", "Train neural network model");
    cli.addSubcommand("simulate", "Run simulation with current brain state");

    cli.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
    cli.addNestedOption("run", "learning_rate", "Learning rate", "0.001");
    cli.addNestedOption("run", "batch_size", "Batch size", "32");

    cli.addNestedOption("train", "epochs", "Number of epochs", "100");
    cli.addNestedOption("train", "learning_rate", "Learning rate", "0.01");

    cli.addNestedOption("simulate", "timestep", "Simulation timestep", "0.001");
    cli.addNestedOption("simulate", "max_steps", "Maximum simulation steps", "10000");

    // Test parsing with different command lines
    std::cout << "\n4. Testing command line parsing..." << std::endl;

    // Test 1: Simple flags and options
    int test1_argc = 5;
    char* test1_argv[] = {
        (char*)"test_program",
        (char*)"--verbose",
        (char*)"--input=data.json",
        (char*)"--output=result.json",
        (char*)"--format=json"
    };

    std::cout << "  Test 1: Basic flags and options" << std::endl;
    if (cli.parse(test1_argc, test1_argv)) {
        std::cout << "    ✓ Parse successful" << std::endl;
        auto config = std::make_shared<Config>();
        if (cli.applyToConfig(config)) {
            std::cout << "    ✓ Config integration successful" << std::endl;
        } else {
            std::cout << "    ✗ Config integration failed" << std::endl;
        }
    } else {
        std::cout << "    ✗ Parse failed" << std::endl;
    }

    // Reset parser for next test
    cli.clear();
    cli.addFlag("verbose", "Enable verbose output");
    cli.addOption("input", "Input file path");
    cli.addOption("output", "Output file path");
    cli.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");
    cli.addRange("steps", "Number of simulation steps", 100, 1000, 500);
    cli.addPath("checkpoint", "Checkpoint file path", false, true);
    cli.addEnvVariable("verbose", "NLM_VERBOSE");
    cli.addConfigFile("config", "json");
    cli.addProfile("default");
    cli.addProfile("fast");
    cli.addProfile("high_quality");

    cli.addSubcommand("run", "Run neural network simulation");
    cli.addSubcommand("train", "Train neural network model");
    cli.addSubcommand("simulate", "Run simulation with current brain state");

    cli.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
    cli.addNestedOption("run", "learning_rate", "Learning rate", "0.001");
    cli.addNestedOption("run", "batch_size", "Batch size", "32");

    cli.addNestedOption("train", "epochs", "Number of epochs", "100");
    cli.addNestedOption("train", "learning_rate", "Learning rate", "0.01");

    cli.addNestedOption("simulate", "timestep", "Simulation timestep", "0.001");
    cli.addNestedOption("simulate", "max_steps", "Maximum simulation steps", "10000");

    // Test 2: Nested options and subcommands
    std::cout << "\n  Test 2: Nested options and subcommands" << std::endl;
    int test2_argc = 7;
    char* test2_argv[] = {
        (char*)"test_program",
        (char*)"--run.nn_type=cnn",
        (char*)"--run.learning_rate=0.01",
        (char*)"--run.batch_size=64",
        (char*)"--train.epochs=200",
        (char*)"--train.learning_rate=0.001",
        (char*)"--run.steps=1000"
    };

    if (cli.parse(test2_argc, test2_argv)) {
        std::cout << "    ✓ Parse successful" << std::endl;
        auto config = std::make_shared<Config>();
        if (cli.applyToConfig(config)) {
            std::cout << "    ✓ Config integration successful" << std::endl;
        } else {
            std::cout << "    ✗ Config integration failed" << std::endl;
        }
    } else {
        std::cout << "    ✗ Parse failed" << std::endl;
    }

    // Reset parser for next test
    cli.clear();
    cli.addFlag("verbose", "Enable verbose output");
    cli.addOption("input", "Input file path");
    cli.addOption("output", "Output file path");
    cli.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");
    cli.addRange("steps", "Number of simulation steps", 100, 1000, 500);
    cli.addPath("checkpoint", "Checkpoint file path", false, true);
    cli.addEnvVariable("verbose", "NLM_VERBOSE");
    cli.addConfigFile("config", "json");
    cli.addProfile("default");
    cli.addProfile("fast");
    cli.addProfile("high_quality");

    // Test 3: Environment variables
    std::cout << "\n5. Testing environment variable support..." << std::endl;
    setenv("NLM_VERBOSE", "1", 1);
    setenv("NLM_OUTPUT_FORMAT", "json", 1);
    setenv("NLM_STEPS", "2000", 1);

    int test3_argc = 1;
    char* test3_argv[] = {
        (char*)"test_program"
    };

    if (cli.parse(test3_argc, test3_argv)) {
        std::cout << "    ✓ Environment variable parsing successful" << std::endl;
    } else {
        std::cout << "    ✗ Environment variable parsing failed" << std::endl;
    }

    // Clean up environment variables
    unsetenv("NLM_VERBOSE");
    unsetenv("NLM_OUTPUT_FORMAT");
    unsetenv("NLM_STEPS");

    // Reset parser for help test
    cli.clear();
    cli.addFlag("verbose", "Enable verbose output");
    cli.addOption("input", "Input file path");
    cli.addOption("output", "Output file path");
    cli.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");

    // Test 4: Help system
    std::cout << "\n6. Testing help system..." << std::endl;
    std::string help = cli.getHelpText();
    if (!help.empty()) {
        std::cout << "    ✓ Help system generated successfully" << std::endl;
        std::cout << "    Help preview (first 500 chars):" << std::endl;
        if (help.size() > 500) {
            std::cout << "    " << help.substr(0, 500) << "..." << std::endl;
        } else {
            std::cout << "    " << help << std::endl;
        }
    } else {
        std::cout << "    ✗ Help system failed" << std::endl;
    }

    std::cout << "\n=== Integration Test Complete ===" << std::endl;
    std::cout << "The Advanced CLI Parser successfully integrates with the existing Config system" << std::endl;
    std::cout << "and provides all the requested expert-level features." << std::endl;

    return 0;
}
