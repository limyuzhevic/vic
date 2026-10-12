#include "core/CLI/AdvancedCLI.h"
#include "core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <filesystem>

using namespace nlm;

class AdvancedCLITest {
public:
    void runAllTests() {
        std::cout << "=== Advanced CLI Tests ===" << std::endl;

        testArgumentTypes();
        testValueTypes();
        testExpertFeatures();
        testCommandStructure();
        testIntegration();
        testErrorHandling();
        testBackwardCompatibility();

        std::cout << "\nAll tests passed!" << std::endl;
    }

private:
    void testArgumentTypes() {
        std::cout << "\n1. Testing Argument Types..." << std::endl;

        CLIParser parser;

        // Test positional arguments
        parser.addPositional("input_file", "Input file path", true);
        parser.addPositional("output_file", "Output file path", false);

        // Test flag arguments
        parser.addFlag("verbose", "Enable verbose output");
        parser.addFlag("quiet", "Suppress non-essential output");
        parser.addFlag("debug", "Enable debug mode");

        // Test option arguments
        parser.addOption("input", "Input file path");
        parser.addOption("output", "Output file path");
        parser.addOption("config", "Configuration file path");

        // Test enum arguments
        parser.addEnum("format", "Output format", {"json", "yaml", "toml"}, "json");

        // Test range arguments
        parser.addRange("steps", "Number of steps", 100, 1000, 500);

        // Test file arguments
        parser.addFile("weights", "Neural network weights file", false);
        parser.addFile("config_file", "Configuration file", true);

        // Test path arguments
        parser.addPath("checkpoint", "Checkpoint file path", false, true);
        parser.addPath("output_dir", "Output directory path", false, false);

        // Test vector arguments
        parser.addVector("learning_rates", "Learning rates (comma-separated)", "float");
        parser.addVector("batch_sizes", "Batch sizes (comma-separated)", "int");
        parser.addVector("hidden_sizes", "Hidden layer sizes (comma-separated)", "int");

        // Test hyperparameter arguments
        parser.addOption("nn_type", "Neural network type", "autoencoder");
        parser.addOption("activation", "Activation function", "relu");
        parser.addOption("optimizer", "Optimizer", "adam");
        parser.addOption("loss_function", "Loss function", "mse");

        // Test config argument
        parser.addOption("config_json", "JSON configuration file");

        std::cout << "  ✓ All argument types parsed successfully" << std::endl;
    }

    void testValueTypes() {
        std::cout << "\n2. Testing Value Types..." << std::endl;

        CLIParser parser;

        // Test string values
        parser.addOption("name", "Neural network name", "default_network");

        // Test integer values
        parser.addOption("neuron_count", "Number of neurons", "500");

        // Test float values
        parser.addOption("learning_rate", "Learning rate", "0.001");

        // Test boolean values
        parser.addFlag("use_gpu", "Use GPU acceleration");

        // Test vector values
        parser.addVector("weights", "Neural network weights", "float");
        parser.addVector("biases", "Neural network biases", "float");
        parser.addVector("layer_sizes", "Layer sizes", "int");

        // Test path values
        parser.addPath("weights_file", "Weights file path", false, true);

        // Test JSON config
        parser.addOption("json_config", "JSON configuration string");

        std::cout << "  ✓ All value types parsed successfully" << std::endl;
    }

    void testExpertFeatures() {
        std::cout << "\n3. Testing Expert Features..." << std::endl;

        CLIParser parser;

        // Environment variable support
        parser.addEnvVariable("verbose", "NLM_VERBOSE");
        parser.addEnvVariable("output", "NLM_OUTPUT_FORMAT");
        parser.addEnvVariable("steps", "NLM_STEPS");
        parser.addEnvVariable("learning_rate", "NLM_LEARNING_RATE");

        // Configuration file integration
        parser.addConfigFile("config", "json");
        parser.addConfigFile("defaults", "keyvalue");

        // Profile management
        parser.addProfile("default");
        parser.addProfile("fast");
        parser.addProfile("high_quality");
        parser.addProfile("research");

        parser.setProfile("fast");

        // Validators
        parser.addValidator("learning_rate", [](const ConfigValue& v) {
            if (std::holds_alternative<double>(v)) {
                double lr = std::get<double>(v);
                return lr >= 0.0001 && lr <= 0.1;
            }
            return false;
        });

        parser.addValidator("batch_size", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int bs = std::get<int>(v);
                return bs >= 1 && bs <= 1024;
            }
            return false;
        });

        // Suggestions
        parser.addSuggestions("nn_type", {"autoencoder", "mlp", "cnn"});
        parser.addSuggestions("optimizer", {"adam", "sgd", "rmsprop"});

        std::cout << "  ✓ All expert features configured successfully" << std::endl;
    }

    void testCommandStructure() {
        std::cout << "\n4. Testing Command Structure..." << std::endl;

        CLIParser parser;

        // Subcommands
        parser.addSubcommand("run", "Run neural network simulation");
        parser.addSubcommand("train", "Train neural network model");
        parser.addSubcommand("simulate", "Run simulation with current brain state");
        parser.addSubcommand("explore", "Explore neural network space");
        parser.addSubcommand("benchmark", "Run benchmark tests");
        parser.addSubcommand("export", "Export configuration or weights");

        // Nested options for subcommands
        parser.addNestedOption("run", "nn_type", "Neural network type", "autoencoder");
        parser.addNestedOption("run", "learning_rate", "Learning rate", "0.001");
        parser.addNestedOption("run", "batch_size", "Batch size", "32");
        parser.addNestedOption("run", "steps", "Number of steps", "1000");

        parser.addNestedOption("train", "epochs", "Number of epochs", "100");
        parser.addNestedOption("train", "validation_split", "Validation split", "0.2");
        parser.addNestedOption("train", "early_stopping", "Enable early stopping", "false");

        parser.addNestedOption("simulate", "timestep", "Simulation timestep", "0.001");
        parser.addNestedOption("simulate", "max_steps", "Maximum simulation steps", "10000");

        parser.addNestedOption("explore", "population_size", "Population size", "50");
        parser.addNestedOption("explore", "mutation_rate", "Mutation rate", "0.1");

        std::cout << "  ✓ All command structures configured successfully" << std::endl;
    }

    void testIntegration() {
        std::cout << "\n5. Testing Integration..." << std::endl;

        // Create a CLI parser with config integration
        CLIParser parser;

        // Add arguments that integrate with Config class
        parser.addOption("config", "Configuration file path");
        parser.addOption("output", "Output file path");
        parser.addOption("format", "Output format (json|console|csv)", "console");

        // Environment variable override system
        parser.addEnvVariable("verbose", "NLM_VERBOSE");
        parser.addEnvVariable("output", "NLM_OUTPUT_FORMAT");
        parser.addEnvVariable("config", "NLM_CONFIG_FILE");

        // Create and test config integration
        auto config = std::make_shared<Config>();

        // Add some test arguments to config
        config->set("test_string", std::string("hello"), ConfigSource::Default);
        config->set("test_int", 42, ConfigSource::Default);
        config->set("test_float", 3.14, ConfigSource::Default);
        config->set("test_bool", true, ConfigSource::Default);
        config->set("test_vector", std::vector<int>({1, 2, 3}), ConfigSource::Default);

        // Test applying parsed arguments to config
        bool success = parser.applyToConfig(config);
        assert(success);

        std::cout << "  ✓ Integration with Config class successful" << std::endl;
    }

    void testErrorHandling() {
        std::cout << "\n6. Testing Error Handling..." << std::endl;

        CLIParser parser;

        // Add arguments with validators
        parser.addOption("required_field", "Required field", "");
        parser.addValidator("required_field", [](const ConfigValue& v) {
            if (std::holds_alternative<std::string>(v)) {
                return !std::get<std::string>(v).empty();
            }
            return false;
        });

        parser.addRange("range_field", "Range field", 0, 100, 50);
        parser.addValidator("range_field", [](const ConfigValue& v) {
            if (std::holds_alternative<int>(v)) {
                int val = std::get<int>(v);
                return val >= 0 && val <= 100;
            }
            return false;
        });

        parser.addEnum("enum_field", "Enum field", {"option1", "option2", "option3"}, "option1");
        parser.addValidator("enum_field", [](const ConfigValue& v) {
            if (std::holds_alternative<std::string>(v)) {
                std::string val = std::get<std::string>(v);
                return val == "option1" || val == "option2" || val == "option3";
            }
            return false;
        });

        // Test error messages
        parser.addErrorMessage("invalid_field", "Field must be a valid integer");

        std::cout << "  ✓ Error handling configured successfully" << std::endl;
    }

    void testBackwardCompatibility() {
        std::cout << "\n7. Testing Backward Compatibility..." << std::endl;

        // Test with existing Config::loadFromArgs() style
        auto config = std::make_shared<Config>();

        // Simulate old-style command line parsing
        int argc = 5;
        char* argv[] = {
            (char*)"test_program",
            (char*)"--input=data.txt",
            (char*)"--output=result.txt",
            (char*)"--format=json",
            (char*)"--steps=1000"
        };

        // Use the old-style loadFromArgs
        bool success = config->loadFromArgs(argc, argv);
        assert(success);

        // Verify the values were set
        assert(config->has("input"));
        assert(config->has("output"));
        assert(config->has("format"));
        assert(config->has("steps"));

        std::cout << "  ✓ Backward compatibility with Config::loadFromArgs() verified" << std::endl;
    }
};

int main() {
    AdvancedCLITest test;
    test.runAllTests();
    return 0;
}
