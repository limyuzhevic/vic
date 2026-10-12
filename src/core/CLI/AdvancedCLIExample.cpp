// Advanced CLI Parser Example
// This example shows how to use the new Advanced CLI Parser
// with backward compatibility to the existing Config::loadFromArgs() method.

#include "core/CLI/AdvancedCLI.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>

using namespace nlm;

int main(int argc, char** argv) {
    std::cout << "=== NLM Advanced CLI Parser Example ===" << std::endl << std::endl;

    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Create the advanced CLI parser
    CLIParser cli;

    std::cout << "Setting up advanced CLI parser..." << std::endl;

    // ===== BASIC ARGUMENT TYPES =====

    // Positional arguments
    cli.addPositional("input_file", "Input file path", true);
    cli.addPositional("output_file", "Output file path", false);

    // Flag arguments
    cli.addFlag("verbose", "Enable verbose output");
    cli.addFlag("quiet", "Suppress non-essential output");
    cli.addFlag("debug", "Enable debug mode");

    // Option arguments
    cli.addOption("input", "Input file path");
    cli.addOption("output", "Output file path");
    cli.addOption("config", "Configuration file path");

    // Enum arguments
    cli.addEnum("format", "Output format (json|yaml|toml)", {"json", "yaml", "toml"}, "json");

    // Range arguments
    cli.addRange("steps", "Number of simulation steps", 100, 1000, 500);

    // File arguments
    cli.addFile("weights", "Neural network weights file", false);
    cli.addFile("config_file", "Configuration file", true);

    // Path arguments
    cli.addPath("checkpoint", "Checkpoint file path", false, true);

    // Vector arguments
    cli.addVector("learning_rates", "Learning rates (comma-separated)", "float");
    cli.addVector("batch_sizes", "Batch sizes (comma-separated)", "int");
    cli.addVector("hidden_sizes", "Hidden layer sizes (comma-separated)", "int");

    // ===== EXPERT FEATURES =====

    // Environment variable support
    cli.addEnvVariable("verbose", "NLM_VERBOSE");
    cli.addEnvVariable("output", "NLM_OUTPUT_FORMAT");
    cli.addEnvVariable("steps", "NLM_STEPS");

    // Configuration file integration
    cli.addConfigFile("config", "json");

    // Defaults file support
    cli.setDefaultsFile("configs/defaults.cfg");

    // Profile management
    cli.addProfile("default");
    cli.addProfile("fast");
    cli.addProfile("high_quality");
    cli.setProfile("default");

    // ===== COMMAND STRUCTURE =====

    // Subcommands
    cli.addSubcommand("run", "Run neural network simulation");
    cli.addSubcommand("train", "Train neural network model");
    cli.addSubcommand("simulate", "Run simulation with current brain state");

    // Nested options
    cli.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
    cli.addNestedOption("run", "learning_rate", "Learning rate", "0.001");
    cli.addNestedOption("run", "batch_size", "Batch size", "32");

    cli.addNestedOption("train", "epochs", "Number of epochs", "100");
    cli.addNestedOption("train", "learning_rate", "Learning rate", "0.01");

    cli.addNestedOption("simulate", "timestep", "Simulation timestep", "0.001");

    // ===== VALIDATION =====

    // Add validators
    cli.addValidator("steps", [](const ConfigValue& v) {
        if (std::holds_alternative<int>(v)) {
            int steps = std::get<int>(v);
            return steps >= 0 && steps <= 1000;
        }
        return false;
    });

    // Add suggestions
    cli.addSuggestions("format", {"json", "yaml", "toml"});

    std::cout << "\nParsing command line arguments..." << std::endl;

    // Parse command line arguments
    if (!cli.parse(argc, argv)) {
        std::cout << "\nError parsing command line:" << std::endl;
        std::cout << cli.getErrorReport() << std::endl;
        std::cout << "\nUsage:" << std::endl;
        std::cout << cli.getHelpText();
        return 1;
    }

    std::cout << "\nParsing successful!" << std::endl;

    // ===== BACKWARD COMPATIBILITY TEST =====

    std::cout << "\nTesting backward compatibility with Config::loadFromArgs()..." << std::endl;

    // Create a new Config object
    auto config = std::make_shared<Config>();

    // Set some values using the old-style Config::loadFromArgs()
    int old_argc = 5;
    char* old_argv[] = {
        (char*)"test_program",
        (char*)"--input=data.txt",
        (char*)"--output=result.txt",
        (char*)"--format=json",
        (char*)"--steps=1000"
    };

    bool old_style_success = config->loadFromArgs(old_argc, old_argv);
    if (old_style_success) {
        std::cout << "✓ Old-style Config::loadFromArgs() still works" << std::endl;

        // Display old-style values
        std::cout << "  Old-style values:" << std::endl;
        for (const auto& key : config->getKeys()) {
            std::cout << "    " << key << " = ";
            if (key == "input" || key == "output" || key == "format") {
                std::cout << config->getOr<std::string>(key, "") << std::endl;
            } else if (key == "steps") {
                std::cout << config->getOr<int>(key, 0) << std::endl;
            }
        }
    } else {
        std::cout << "✗ Old-style Config::loadFromArgs() failed" << std::endl;
    }

    // ===== NEW ADVANCED CLI INTEGRATION =====

    std::cout << "\nTesting new Advanced CLI integration with Config..." << std::endl;

    // Create a new Config object for the advanced CLI
    auto newConfig = std::make_shared<Config>();

    // Apply the parsed arguments to the new config
    bool new_style_success = cli.applyToConfig(newConfig);
    if (new_style_success) {
        std::cout << "✓ New Advanced CLI integration successful" << std::endl;

        // Display new-style values
        std::cout << "  New-style values:" << std::endl;
        for (const auto& key : newConfig->getKeys()) {
            std::cout << "    " << key << " = ";
            ConfigValue value = newConfig->getRawValue(key);
            std::visit([&](const auto& val) {
                std::cout << val << std::endl;
            }, value);
        }
    } else {
        std::cout << "✗ New Advanced CLI integration failed" << std::endl;
        std::cout << cli.getErrorReport() << std::endl;
    }

    // ===== COMMAND EXECUTION DEMO =====

    std::cout << "\nExecuting commands based on parsed arguments..." << std::endl;

    // Determine which command to execute
    std::string command = "run"; // Default
    if (cli.hasArgument("command")) {
        command = std::get<std::string>(cli.getArgumentValue("command"));
    }

    std::cout << "Executing command: " << command << std::endl;

    if (command == "run") {
        std::cout << "  Running neural network simulation..." << std::endl;

        // Extract run-specific options
        std::string nnType = newConfig->getOr<std::string>("run.nn_type", "autoencoder");
        double learningRate = newConfig->getOr<double>("run.learning_rate", 0.001);
        int batchSize = newConfig->getOr<int>("run.batch_size", 32);
        int steps = newConfig->getOr<int>("run.steps", 1000);

        std::cout << "    Neural Network Type: " << nnType << std::endl;
        std::cout << "    Learning Rate: " << learningRate << std::endl;
        std::cout << "    Batch Size: " << batchSize << std::endl;
        std::cout << "    Steps: " << steps << std::endl;

        // Simulate running the neural network
        std::cout << "    Simulating neural network for " << steps << " steps..." << std::endl;
        for (int i = 0; i < steps; i += 100) {
            std::cout << "      Step " << i << "/" << steps << std::endl;
        }

        std::cout << "    Simulation completed successfully!" << std::endl;

    } else if (command == "train") {
        std::cout << "  Training neural network model..." << std::endl;

        // Extract training-specific options
        int epochs = newConfig->getOr<int>("train.epochs", 100);
        double learningRate = newConfig->getOr<double>("train.learning_rate", 0.01);
        int batchSize = newConfig->getOr<int>("train.batch_size", 64);

        std::cout << "    Epochs: " << epochs << std::endl;
        std::cout << "    Learning Rate: " << learningRate << std::endl;
        std::cout << "    Batch Size: " << batchSize << std::endl;

        // Simulate training
        std::cout << "    Training for " << epochs << " epochs..." << std::endl;
        for (int i = 0; i < epochs; i += 10) {
            std::cout << "      Epoch " << i << "/" << epochs << std::endl;
        }

        std::cout << "    Training completed successfully!" << std::endl;

    } else if (command == "simulate") {
        std::cout << "  Running simulation with current brain state..." << std::endl;

        // Extract simulation-specific options
        double timestep = newConfig->getOr<double>("simulate.timestep", 0.001);
        int maxSteps = newConfig->getOr<int>("simulate.max_steps", 10000);

        std::cout << "    Simulation Timestep: " << timestep << "s" << std::endl;
        std::cout << "    Maximum Steps: " << maxSteps << std::endl;

        // Simulate the brain
        std::cout << "    Running brain simulation for " << maxSteps << " steps..." << std::endl;
        for (int i = 0; i < maxSteps; i += 1000) {
            std::cout << "      Step " << i << "/" << maxSteps << std::endl;
        }

        std::cout << "    Simulation completed successfully!" << std::endl;
    }

    std::cout << "\n=== Example Complete ===" << std::endl;
    std::cout << "This example demonstrates:" << std::endl;
    std::cout << "✓ Setting up advanced argument types" << std::endl;
    std::cout << "✓ Implementing expert features (env vars, config files, profiles)" << std::endl;
    std::cout << "✓ Using subcommands and nested options" << std::endl;
    std::cout << "✓ Validating arguments with custom validators" << std::endl;
    std::cout << "✓ Integrating with the existing Config class" << std::endl;
    std::cout << "✓ Maintaining backward compatibility with Config::loadFromArgs()" << std::endl;
    std::cout << "✓ Executing commands based on parsed arguments" << std::endl;

    return 0;
}
