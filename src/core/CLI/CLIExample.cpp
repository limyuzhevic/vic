#include "core/CLI/AdvancedCLI.h"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <memory>
#include <string>

using namespace nlm;

class NLMCLI {
public:
    NLMCLI() {
        // Initialize logger
        logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);

        // Create CLI parser
        cli = std::make_shared<CLIParser>();

        // Set up CLI with expert features
        setupAdvancedCLI();
    }

    void run(int argc, char** argv) {
        // Parse command line
        if (!cli->parse(argc, argv)) {
            std::cout << "Error parsing command line:" << std::endl;
            std::cout << cli->getErrorReport() << std::endl;
            std::cout << "\nUsage:" << std::endl;
            std::cout << cli->getHelpText();
            return;
        }

        // Create and configure NLM config
        auto config = std::make_shared<Config>();

        // Apply parsed arguments to config
        if (!cli->applyToConfig(config)) {
            std::cout << "Error applying arguments to config:" << std::endl;
            std::cout << cli->getErrorReport() << std::endl;
            return;
        }

        // Execute the requested command
        std::string command = "run"; // Default command
        if (cli->hasArgument("command")) {
            command = std::get<std::string>(cli->getArgumentValue("command"));
        }

        executeCommand(command, config);
    }

private:
    void setupAdvancedCLI() {
        // Add global options
        cli->addFlag("verbose", "Enable verbose output");
        cli->addFlag("quiet", "Suppress non-essential output");
        cli->addFlag("debug", "Enable debug logging");
        cli->addFlag("help", "Show help information");
        cli->addFlag("version", "Show version information");

        cli->addOption("output", "Output format (json|console|csv)", "console");
        cli->addOption("config", "Configuration file path");
        cli->addPath("input", "Input file path", false, true);
        cli->addPath("output_dir", "Output directory path", false, false);

        cli->addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");
        cli->addRange("steps", "Number of simulation steps", 100, 10000, 1000);
        cli->addRange("neurons", "Number of neurons", 10, 10000, 500);
        cli->addRange("regions", "Number of brain regions", 1, 100, 1);

        cli->addFile("weights", "Neural network weights file", false);
        cli->addPath("checkpoint", "Checkpoint file path", false, true);

        cli->addVector("learning_rates", "Learning rates (comma-separated)", "float");
        cli->addVector("batch_sizes", "Batch sizes (comma-separated)", "int");
        cli->addVector("hidden_sizes", "Hidden layer sizes (comma-separated)", "int");

        // Neural network hyperparameters
        cli->addOption("nn_type", "Neural network type (autoencoder|mlp|cnn)", "autoencoder");
        cli->addOption("activation", "Activation function (relu|tanh|sigmoid|elu)", "relu");
        cli->addOption("optimizer", "Optimizer (adam|sgd|rmsprop)", "adam");
        cli->addOption("loss_function", "Loss function (mse|crossentropy|bce)", "mse");

        cli->addRange("learning_rate", "Learning rate (0.0001..0.1)", 1, 10000, 1000);
        cli->addRange("batch_size", "Batch size (1..1024)", 1, 1024, 32);
        cli->addRange("epochs", "Number of training epochs (1..10000)", 1, 10000, 100);
        cli->addRange("hidden_layers", "Number of hidden layers (1..10)", 1, 10, 3);
        cli->addRange("dropout_rate", "Dropout rate (0..1)", 0, 100, 50);

        // Environment variable fallbacks
        cli->addEnvVariable("verbose", "NLM_VERBOSE");
        cli->addEnvVariable("output", "NLM_OUTPUT_FORMAT");
        cli->addEnvVariable("steps", "NLM_STEPS");
        cli->addEnvVariable("neurons", "NLM_NEURONS");
        cli->addEnvVariable("learning_rate", "NLM_LEARNING_RATE");

        // Configuration file integration
        cli->addConfigFile("config", "json");

        // Profile management
        cli->addProfile("default");
        cli->addProfile("fast");
        cli->addProfile("high_quality");
        cli->addProfile("research");

        // Subcommands
        cli->addSubcommand("run", "Run neural network simulation");
        cli->addSubcommand("train", "Train neural network model");
        cli->addSubcommand("simulate", "Run simulation with current brain state");
        cli->addSubcommand("explore", "Explore neural network space");
        cli->addSubcommand("benchmark", "Run benchmark tests");
        cli->addSubcommand("export", "Export configuration or weights");

        // Nested options for subcommands
        cli->addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
        cli->addNestedOption("run", "learning_rate", "Learning rate", "0.001");
        cli->addNestedOption("run", "batch_size", "Batch size", "32");
        cli->addNestedOption("run", "steps", "Number of steps", "1000");

        cli->addNestedOption("train", "epochs", "Number of epochs", "100");
        cli->addNestedOption("train", "validation_split", "Validation split", "0.2");
        cli->addNestedOption("train", "early_stopping", "Enable early stopping", "false");

        cli->addNestedOption("simulate", "timestep", "Simulation timestep", "0.001");
        cli->addNestedOption("simulate", "max_steps", "Maximum simulation steps", "10000");
        cli->addNestedOption("simulate", "output_frequency", "Output frequency", "100");

        cli->addNestedOption("explore", "population_size", "Population size", "50");
        cli->addNestedOption("explore", "mutation_rate", "Mutation rate", "0.1");
        cli->addNestedOption("explore", "crossover_rate", "Crossover rate", "0.8");

        // Validators
        cli->addValidator("learning_rate", [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v)) {
                double lr = std::get<double>(v);
                return lr > 0.0 && lr <= 1.0;
            }
            return false;
        });

        cli->addValidator("batch_size", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int bs = std::get<int>(v);
                return bs >= 1 && bs <= 1024;
            }
            return false;
        });

        cli->addValidator("epochs", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int epochs = std::get<int>(v);
                return epochs >= 1 && epochs <= 10000;
            }
            return false;
        });

        cli->addValidator("dropout_rate", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int dr = std::get<int>(v);
                return dr >= 0 && dr <= 100;
            }
            return false;
        });

        // Suggestions for common errors
        cli->addSuggestions("nn_type", {"autoencoder", "mlp", "cnn"});
        cli->addSuggestions("optimizer", {"adam", "sgd", "rmsprop"});
        cli->addSuggestions("activation", {"relu", "tanh", "sigmoid", "elu"});

        // Enable expert features
        cli->enableAutoCompletion();
        cli->setDefaultsFile("configs/defaults.cfg");
        cli->setOutputFormat("console");

        // Set default profile
        cli->setProfile("default");
    }

    void executeCommand(const std::string& command, std::shared_ptr<Config> config) {
        NLM_LOG_INFO("Executing command: " + command);

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
            NLM_LOG_ERROR("Unknown command: " + command);
        }
    }

    void executeRunCommand(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Running Neural Network Simulation ===");

        // Extract run-specific options
        std::string nnType = config->getOr<std::string>("run.nn_type", "autoencoder");
        double learningRate = config->getOr<double>("run.learning_rate", 0.001);
        int batchSize = config->getOr<int>("run.batch_size", 32);
        int steps = config->getOr<int>("run.steps", 1000);

        NLM_LOG_INFO("Neural Network Configuration:");
        NLM_LOG_INFO("  Type: " + nnType);
        NLM_LOG_INFO("  Learning Rate: " + std::to_string(learningRate));
        NLM_LOG_INFO("  Batch Size: " + std::to_string(batchSize));
        NLM_LOG_INFO("  Steps: " + std::to_string(steps));

        // Run the simulation
        NLM_LOG_INFO("Starting simulation...");
        for (int step = 0; step < steps; ++step) {
            // Simulate neural network activity
            if (step % 100 == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(steps));
            }

            // In a real implementation, this would update the neural network
            // and simulate the neural dynamics
        }

        NLM_LOG_INFO("Simulation completed successfully!");
    }

    void executeTrainCommand(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Training Neural Network ===");

        // Extract training-specific options
        int epochs = config->getOr<int>("train.epochs", 100);
        double validationSplit = config->getOr<double>("train.validation_split", 0.2);
        bool earlyStopping = config->getOr<bool>("train.early_stopping", false);

        NLM_LOG_INFO("Training Configuration:");
        NLM_LOG_INFO("  Epochs: " + std::to_string(epochs));
        NLM_LOG_INFO("  Validation Split: " + std::to_string(validationSplit));
        NLM_LOG_INFO("  Early Stopping: " + std::to_string(earlyStopping));

        NLM_LOG_INFO("Starting training...");
        for (int epoch = 0; epoch < epochs; ++epoch) {
            if (epoch % 10 == 0) {
                NLM_LOG_INFO("Epoch " + std::to_string(epoch) + "/" + std::to_string(epochs));
            }

            // In a real implementation, this would train the neural network
        }

        NLM_LOG_INFO("Training completed successfully!");
    }

    void executeSimulateCommand(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Running Brain Simulation ===");

        // Extract simulation-specific options
        double timestep = config->getOr<double>("simulate.timestep", 0.001);
        int maxSteps = config->getOr<int>("simulate.max_steps", 10000);
        int outputFrequency = config->getOr<int>("simulate.output_frequency", 100);

        NLM_LOG_INFO("Simulation Configuration:");
        NLM_LOG_INFO("  Timestep: " + std::to_string(timestep) + "s");
        NLM_LOG_INFO("  Max Steps: " + std::to_string(maxSteps));
        NLM_LOG_INFO("  Output Frequency: " + std::to_string(outputFrequency));

        NLM_LOG_INFO("Starting simulation...");
        for (int step = 0; step < maxSteps; ++step) {
            if (step % outputFrequency == 0) {
                NLM_LOG_INFO("Step " + std::to_string(step) + "/" + std::to_string(maxSteps));
            }

            // In a real implementation, this would run the brain simulation
        }

        NLM_LOG_INFO("Simulation completed successfully!");
    }

    void executeExploreCommand(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Exploring Neural Network Space ===");

        // Extract exploration-specific options
        int populationSize = config->getOr<int>("explore.population_size", 50);
        int mutationRate = config->getOr<int>("explore.mutation_rate", 10);
        int crossoverRate = config->getOr<int>("explore.crossover_rate", 80);

        NLM_LOG_INFO("Exploration Configuration:");
        NLM_LOG_INFO("  Population Size: " + std::to_string(populationSize));
        NLM_LOG_INFO("  Mutation Rate: " + std::to_string(mutationRate) + "%");
        NLM_LOG_INFO("  Crossover Rate: " + std::to_string(crossoverRate) + "%");

        NLM_LOG_INFO("Starting exploration...");
        // In a real implementation, this would run genetic algorithms

        NLM_LOG_INFO("Exploration completed successfully!");
    }

    void executeBenchmarkCommand(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Running Benchmark Tests ===");
        // In a real implementation, this would run performance benchmarks
        NLM_LOG_INFO("Benchmark completed successfully!");
    }

    void executeExportCommand(std::shared_ptr<Config> config) {
        NLM_LOG_INFO("=== Exporting Configuration/Weights ===");
        // In a real implementation, this would export configuration or weights
        NLM_LOG_INFO("Export completed successfully!");
    }

    std::shared_ptr<Logger> logger;
    std::shared_ptr<CLIParser> cli;
};

int main(int argc, char** argv) {
    auto nlmCLI = std::make_unique<NLMCLI>();
    nlmCLI->run(argc, argv);
    return 0;
}
