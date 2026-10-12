// NLM Advanced CLI Demonstration
// This program demonstrates how to use the new Advanced CLI Parser
// with the existing Config class while maintaining backward compatibility.

#include "core/CLI/AdvancedCLI.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>

using namespace nlm;

class NLMAdvancedCLI {
public:
    NLMAdvancedCLI() {
        // Initialize the CLI parser with advanced features
        setupAdvancedCLI();
    }

    void run(int argc, char** argv) {
        std::cout << "=== NLM Advanced CLI Parser ===" << std::endl << std::endl;

        // Parse command line arguments
        if (!cli.parse(argc, argv)) {
            std::cout << "Error parsing command line:" << std::endl;
            std::cout << cli.getErrorReport() << std::endl;
            std::cout << "\nUsage:" << std::endl;
            std::cout << cli.getHelpText();
            return;
        }

        // Create and configure NLM config
        auto config = std::make_shared<Config>();

        // Apply parsed arguments to config
        if (!cli.applyToConfig(config)) {
            std::cout << "Error applying arguments to config:" << std::endl;
            std::cout << cli.getErrorReport() << std::endl;
            return;
        }

        // Display configuration summary
        std::cout << "\n=== Configuration Applied ===" << std::endl;
        std::cout << config->summary() << std::endl;

        // Execute the requested command
        std::string command = "run"; // Default command
        if (cli.hasArgument("command")) {
            command = std::get<std::string>(cli.getArgumentValue("command"));
        }

        executeCommand(command, config);
    }

private:
    void setupAdvancedCLI() {
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
        cli.addRange("steps", "Number of simulation steps", 100, 10000, 1000);
        cli.addRange("neurons", "Number of neurons", 10, 10000, 500);
        cli.addRange("regions", "Number of brain regions", 1, 100, 1);

        // File arguments
        cli.addFile("weights", "Neural network weights file", false);
        cli.addFile("config_file", "Configuration file", true);

        // ===== VALUE TYPES =====

        // String values
        cli.addOption("name", "Neural network name", "default_network");

        // Integer values
        cli.addOption("neuron_count", "Number of neurons", "500");

        // Float values
        cli.addOption("learning_rate", "Learning rate", "0.001");

        // Boolean values (flags)
        cli.addFlag("use_gpu", "Use GPU acceleration");
        cli.addFlag("enable_plasticity", "Enable synaptic plasticity");
        cli.addFlag("enable_structural_plasticity", "Enable structural plasticity");

        // Vector arguments
        cli.addVector("learning_rates", "Learning rates (comma-separated)", "float");
        cli.addVector("batch_sizes", "Batch sizes (comma-separated)", "int");
        cli.addVector("hidden_sizes", "Hidden layer sizes (comma-separated)", "int");

        // ===== NEURAL NETWORK HYPERPARAMETERS =====

        // Neural network architecture
        cli.addOption("nn_type", "Neural network type (autoencoder|mlp|cnn)", "autoencoder");
        cli.addOption("activation", "Activation function (relu|tanh|sigmoid|elu)", "relu");
        cli.addOption("optimizer", "Optimizer (adam|sgd|rmsprop)", "adam");
        cli.addOption("loss_function", "Loss function (mse|crossentropy|bce)", "mse");

        // Training hyperparameters
        cli.addOption("learning_rate", "Learning rate (0.0001..0.1)", "0.001");
        cli.addOption("batch_size", "Batch size (1..1024)", "32");
        cli.addOption("epochs", "Number of training epochs (1..10000)", "100");
        cli.addOption("hidden_layers", "Number of hidden layers (1..10)", "3");
        cli.addOption("dropout_rate", "Dropout rate (0..1)", "0.5");
        cli.addOption("weight_decay", "Weight decay (0.00001..0.1)", "0.00001");

        // ===== EXPERT FEATURES =====

        // Environment variable support
        cli.addEnvVariable("verbose", "NLM_VERBOSE");
        cli.addEnvVariable("output", "NLM_OUTPUT_FORMAT");
        cli.addEnvVariable("steps", "NLM_STEPS");
        cli.addEnvVariable("neurons", "NLM_NEURONS");
        cli.addEnvVariable("learning_rate", "NLM_LEARNING_RATE");
        cli.addEnvVariable("batch_size", "NLM_BATCH_SIZE");
        cli.addEnvVariable("epochs", "NLM_EPOCHS");

        // Configuration file integration
        cli.addConfigFile("config", "json");

        // Defaults file support
        cli.setDefaultsFile("configs/defaults.cfg");

        // Profile management
        cli.addProfile("default");
        cli.addProfile("fast");
        cli.addProfile("high_quality");
        cli.addProfile("research");

        // Set default profile
        cli.setProfile("default");

        // ===== COMMAND STRUCTURE =====

        // Subcommands
        cli.addSubcommand("run", "Run neural network simulation");
        cli.addSubcommand("train", "Train neural network model");
        cli.addSubcommand("simulate", "Run simulation with current brain state");
        cli.addSubcommand("explore", "Explore neural network space");
        cli.addSubcommand("benchmark", "Run benchmark tests");
        cli.addSubcommand("export", "Export configuration or weights");

        // Nested options for subcommands
        // Run subcommand options
        cli.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
        cli.addNestedOption("run", "learning_rate", "Learning rate", "0.001");
        cli.addNestedOption("run", "batch_size", "Batch size", "32");
        cli.addNestedOption("run", "steps", "Number of steps", "1000");
        cli.addNestedOption("run", "simulation_timestep", "Simulation timestep", "0.001");

        // Train subcommand options
        cli.addNestedOption("train", "epochs", "Number of epochs", "100");
        cli.addNestedOption("train", "validation_split", "Validation split", "0.2");
        cli.addNestedOption("train", "early_stopping", "Enable early stopping", "false");
        cli.addNestedOption("train", "learning_rate", "Learning rate", "0.01");
        cli.addNestedOption("train", "batch_size", "Batch size", "64");

        // Simulate subcommand options
        cli.addNestedOption("simulate", "timestep", "Simulation timestep", "0.001");
        cli.addNestedOption("simulate", "max_steps", "Maximum simulation steps", "10000");
        cli.addNestedOption("simulate", "output_frequency", "Output frequency", "100");

        // Explore subcommand options
        cli.addNestedOption("explore", "population_size", "Population size", "50");
        cli.addNestedOption("explore", "mutation_rate", "Mutation rate", "0.1");
        cli.addNestedOption("explore", "crossover_rate", "Crossover rate", "0.8");
        cli.addNestedOption("explore", "generations", "Number of generations", "100");

        // ===== VALIDATION =====

        // Add validators for various argument types
        cli.addValidator("learning_rate", [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v)) {
                double lr = std::get<double>(v);
                return lr >= 0.0001 && lr <= 0.1;
            }
            return false;
        });

        cli.addValidator("batch_size", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int bs = std::get<int>(v);
                return bs >= 1 && bs <= 1024;
            }
            return false;
        });

        cli.addValidator("epochs", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int epochs = std::get<int>(v);
                return epochs >= 1 && epochs <= 10000;
            }
            return false;
        });

        cli.addValidator("dropout_rate", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int dr = std::get<int>(v);
                return dr >= 0 && dr <= 100;
            }
            return false;
        });

        cli.addValidator("weight_decay", [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v)) {
                double wd = std::get<double>(v);
                return wd >= 0.00001 && wd <= 0.1;
            }
            return false;
        });

        // ===== ERROR HANDLING AND SUGGESTIONS =====

        // Add error messages
        cli.addErrorMessage("invalid_learning_rate", "Learning rate must be between 0.0001 and 0.1");
        cli.addErrorMessage("invalid_batch_size", "Batch size must be between 1 and 1024");

        // Add suggestions for common errors
        cli.addSuggestions("nn_type", {"autoencoder", "mlp", "cnn"});
        cli.addSuggestions("optimizer", {"adam", "sgd", "rmsprop"});
        cli.addSuggestions("activation", {"relu", "tanh", "sigmoid", "elu"});
        cli.addSuggestions("loss_function", {"mse", "crossentropy", "bce"});

        // ===== OUTPUT FORMATTING =====

        // Set output format
        cli.setOutputFormat("console");

        // ===== THREAD SAFETY =====

        // Enable thread safety
        cli.enableThreadSafety();

        // ===== AUTO-COMPLETION =====

        // Enable auto-completion
        cli.enableAutoCompletion();
    }

    void executeCommand(const std::string& command, std::shared_ptr<Config> config) {
        std::cout << "\n=== Executing Command: " << command << " ===" << std::endl;

        if (command == "run") {
            executeRunCommand(config);
        } else if (command == "train") {
            executeTrainCommand(config);
        } else if (command == "simulate") {
            executeSimulateCommand(config);
        } else if (command == "explore") {
            executeExploreCommand(config);
        } else if (command == "benchmark") {
            executeBenchmarkCommand(config);
        } else if (command == "export") {
            executeExportCommand(config);
        } else {
            std::cout << "Error: Unknown command: " << command << std::endl;
            std::cout << "Available commands:" << std::endl;
            for (const auto& cmd : cli.getCommandCompletions("")) {
                std::cout << "  " << cmd << std::endl;
            }
        }
    }

    void executeRunCommand(std::shared_ptr<Config> config) {
        std::cout << "Running neural network simulation..." << std::endl;

        // Extract run-specific options
        std::string nnType = config->getOr<std::string>("run.nn_type", "autoencoder");
        double learningRate = config->getOr<double>("run.learning_rate", 0.001);
        int batchSize = config->getOr<int>("run.batch_size", 32);
        int steps = config->getOr<int>("run.steps", 1000);
        double timestep = config->getOr<double>("run.simulation_timestep", 0.001);

        std::cout << "\nRun Configuration:" << std::endl;
        std::cout << "  Neural Network Type: " << nnType << std::endl;
        std::cout << "  Learning Rate: " << learningRate << std::endl;
        std::cout << "  Batch Size: " << batchSize << std::endl;
        std::cout << "  Steps: " << steps << std::endl;
        std::cout << "  Simulation Timestep: " << timestep << "s" << std::endl;

        // Run the simulation
        std::cout << "\nStarting simulation..." << std::endl;
        for (int step = 0; step < steps; ++step) {
            // In a real implementation, this would update the neural network
            // and simulate the neural dynamics

            if (step % 100 == 0 || step == steps - 1) {
                std::cout << "  Step " << step << "/" << steps << std::endl;
            }
        }

        std::cout << "\nSimulation completed successfully!" << std::endl;
    }

    void executeTrainCommand(std::shared_ptr<Config> config) {
        std::cout << "Training neural network model..." << std::endl;

        // Extract training-specific options
        int epochs = config->getOr<int>("train.epochs", 100);
        double validationSplit = config->getOr<double>("train.validation_split", 0.2);
        bool earlyStopping = config->getOr<bool>("train.early_stopping", false);
        double learningRate = config->getOr<double>("train.learning_rate", 0.01);
        int batchSize = config->getOr<int>("train.batch_size", 64);

        std::cout << "\nTraining Configuration:" << std::endl;
        std::cout << "  Epochs: " << epochs << std::endl;
        std::cout << "  Validation Split: " << validationSplit << std::endl;
        std::cout << "  Early Stopping: " << (earlyStopping ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  Learning Rate: " << learningRate << std::endl;
        std::cout << "  Batch Size: " << batchSize << std::endl;

        std::cout << "\nStarting training..." << std::endl;
        for (int epoch = 0; epoch < epochs; ++epoch) {
            // In a real implementation, this would train the neural network

            if (epoch % 10 == 0 || epoch == epochs - 1) {
                std::cout << "  Epoch " << epoch << "/" << epochs << std::endl;
            }
        }

        std::cout << "\nTraining completed successfully!" << std::endl;
    }

    void executeSimulateCommand(std::shared_ptr<Config> config) {
        std::cout << "Running simulation with current brain state..." << std::endl;

        // Extract simulation-specific options
        double timestep = config->getOr<double>("simulate.timestep", 0.001);
        int maxSteps = config->getOr<int>("simulate.max_steps", 10000);
        int outputFrequency = config->getOr<int>("simulate.output_frequency", 100);

        std::cout << "\nSimulation Configuration:" << std::endl;
        std::cout << "  Simulation Timestep: " << timestep << "s" << std::endl;
        std::cout << "  Maximum Steps: " << maxSteps << std::endl;
        std::cout << "  Output Frequency: " << outputFrequency << std::endl;

        std::cout << "\nStarting simulation..." << std::endl;
        for (int step = 0; step < maxSteps; ++step) {
            // In a real implementation, this would run the brain simulation

            if (step % outputFrequency == 0) {
                std::cout << "  Step " << step << "/" << maxSteps << std::endl;
            }
        }

        std::cout << "\nSimulation completed successfully!" << std::endl;
    }

    void executeExploreCommand(std::shared_ptr<Config> config) {
        std::cout << "Exploring neural network space..." << std::endl;

        // Extract exploration-specific options
        int populationSize = config->getOr<int>("explore.population_size", 50);
        int mutationRate = config->getOr<int>("explore.mutation_rate", 10);
        int crossoverRate = config->getOr<int>("explore.crossover_rate", 80);
        int generations = config->getOr<int>("explore.generations", 100);

        std::cout << "\nExploration Configuration:" << std::endl;
        std::cout << "  Population Size: " << populationSize << std::endl;
        std::cout << "  Mutation Rate: " << mutationRate << "%" << std::endl;
        std::cout << "  Crossover Rate: " << crossoverRate << "%" << std::endl;
        std::cout << "  Generations: " << generations << std::endl;

        std::cout << "\nStarting exploration..." << std::endl;
        // In a real implementation, this would run genetic algorithms

        for (int gen = 0; gen < generations; ++gen) {
            if (gen % 20 == 0 || gen == generations - 1) {
                std::cout << "  Generation " << gen << "/" << generations << std::endl;
            }
        }

        std::cout << "\nExploration completed successfully!" << std::endl;
    }

    void executeBenchmarkCommand(std::shared_ptr<Config> config) {
        std::cout << "Running benchmark tests..." << std::endl;
        // In a real implementation, this would run performance benchmarks
        std::cout << "Benchmark completed successfully!" << std::endl;
    }

    void executeExportCommand(std::shared_ptr<Config> config) {
        std::cout << "Exporting configuration or weights..." << std::endl;
        // In a real implementation, this would export configuration or weights
        std::cout << "Export completed successfully!" << std::endl;
    }

    std::shared_ptr<CLIParser> cli;
};

int main(int argc, char** argv) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);

    // Create and run the advanced CLI
    auto nlmCLI = std::make_unique<NLMAdvancedCLI>();
    nlmCLI->run(argc, argv);

    return 0;
}
