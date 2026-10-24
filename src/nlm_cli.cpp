#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <algorithm>
#include <cctype>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

using namespace nlm;

// Command line parsing
class CLIParser {
private:
    std::map<std::string, std::string> options;
    std::vector<std::string> args;
    std::string helpText;

public:
    CLIParser(int argc, char** argv) {
        parseArgs(argc, argv);
    }

    void parseArgs(int argc, char** argv) {
        options.clear();
        args.clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key=value format
            if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
                size_t pos = arg.find("=");
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                options[key] = value;
            }
            // Handle --flag format
            else if (arg.find("--") == 0) {
                std::string key = arg.substr(2);
                options[key] = "true";
            }
            // Handle positional arguments
            else {
                args.push_back(arg);
            }
        }
    }

    bool hasOption(const std::string& key) const {
        return options.find(key) != options.end();
    }

    std::string getOption(const std::string& key, const std::string& defaultValue = "") const {
        auto it = options.find(key);
        if (it != options.end()) {
            return it->second;
        }
        return defaultValue;
    }

    std::string getPosArg(size_t index, const std::string& defaultValue = "") const {
        if (index < args.size()) {
            return args[index];
        }
        return defaultValue;
    }

    void printHelp() const {
        std::cout << helpText << std::endl;
    }

    void setHelp(const std::string& help) {
        helpText = help;
    }
};

// Command line tools
class NLMCLI {
public:
    static void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Command Line Interface                                     ║
    ║                                                               ║
    ╚═════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }

    static void printUsage() {
        std::cout << R"(
USAGE:
    nlm_cli [MODE] [OPTIONS]

MODES:
    demo                    Run a quick demo (default)
    experiment              Run specified experiment
    benchmark               Run performance benchmark
    config                  Manage configuration files
    help                    Show this help message

MODES WITH SUBCOMMANDS:
    config [command]        Configuration file commands
        commands: load, save, list, info

EXAMPLES:
    nlm_cli demo --neuron_count=1000
    nlm_cli experiment --file=experiment.cfg
    nlm_cli benchmark --repeats=10
    nlm_cli config save config.json

For more information about a specific mode, use:
    nlm_cli <MODE> --help
)" << std::endl;
    }

    static void printDemoHelp() {
        std::cout << R"(
DEMO MODE:
Run a quick demonstration of NLM functionality.

OPTIONS:
    --neuron_count=COUNT     Number of neurons (default: 500)
    --steps=STEPS            Number of simulation steps (default: 100)
    --help                   Show this help

EXAMPLE:
    nlm_cli demo --neuron_count=1000 --steps=200
)" << std::endl;
    }

    static void printExperimentHelp() {
        std::cout << R"(
EXPERIMENT MODE:
Run a specific experiment configuration.

OPTIONS:
    --file=FILE             Configuration file path
    --steps=STEPS           Override number of steps in config
    --help                  Show this help

EXAMPLE:
    nlm_cli experiment --file=configs/default.cfg
)" << std::endl;
    }

    static void printBenchmarkHelp() {
        std::cout << R"(
BENCHMARK MODE:
Run performance benchmarks.

OPTIONS:
    --neuron_counts=LIST    Comma-separated neuron counts (default: 500,1000,5000)
    --steps=STEPS           Number of simulation steps (default: 100)
    --repeats=REPEATS       Number of repetitions (default: 3)
    --help                  Show this help

EXAMPLE:
    nlm_cli benchmark --neuron_counts=100,500,1000 --repeats=5
)" << std::endl;
    }

    static void runDemo(const std::map<std::string, std::string>& options) {
        std::cout << "=== NLM Demo Mode ===" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);

        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Get options
        int64_t neuronCount = 500;
        if (options.find("neuron_count") != options.end()) {
            neuronCount = std::stoll(options.at("neuron_count"));
        }
        
        int steps = 100;
        if (options.find("steps") != options.end()) {
            steps = std::stoi(options.at("steps"));
        }

        // Configure
        config->set("neuron_count", neuronCount, ConfigSource::Runtime);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
        config->set("connection_probability", 0.15f, ConfigSource::Runtime);

        std::cout << "Configuration:" << std::endl;
        std::cout << "  Neurons: " << neuronCount << std::endl;
        std::cout << "  Steps: " << steps << std::endl;
        std::cout << std::endl;

        // Create and initialize brain
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }

        std::cout << "Brain initialized successfully." << std::endl;
        std::cout << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
        std::cout << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
        std::cout << std::endl;

        // Run simulation
        std::cout << "Running simulation..." << std::endl;
        for (int step = 0; step < steps; ++step) {
            brain.step(step, step * 0.001);
            
            if (step % 20 == 0 || step == steps - 1) {
                std::cout << "Step " << step << 
                    ": " << brain.getFiringNeuronCount() << " firing neurons, " <<
                    brain.getTotalSpikeCount() << " total spikes" << std::endl;
            }
        }

        std::cout << std::endl;
        std::cout << "=== Demo Complete ===" << std::endl;
        std::cout << "Total spikes generated: " << brain.getTotalSpikeCount() << std::endl;
        std::cout << "Average firing rate: " << brain.getAverageFiringRate() << std::endl;
        std::cout << "E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
    }

    static void runExperiment(const std::map<std::string, std::string>& options) {
        std::cout << "=== NLM Experiment Mode ===" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);

        // Create configuration
        auto config = std::make_shared<Config>();
        
        // Try to load from file if specified
        if (options.find("file") != options.end()) {
            std::string configFile = options.at("file");
            if (config->loadFromFile(configFile)) {
                std::cout << "Loaded configuration from: " << configFile << std::endl;
            } else {
                std::cout << "Could not load configuration from: " << configFile 
                    << ". Using defaults." << std::endl;
            }
        }

        // Get steps override if specified
        if (options.find("steps") != options.end()) {
            int steps = std::stoi(options.at("steps"));
            config->set("max_simulation_steps", static_cast<int64_t>(steps), ConfigSource::Runtime);
        }

        std::cout << "Running experiment with configuration:" << std::endl;
        std::cout << config->summary() << std::endl;
        std::cout << std::endl;

        // Create brain
        nlm::Brain brain(config);
        if (!brain.initialize()) {
            std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
            return;
        }

        // Run experiment
        std::cout << "Running experiment..." << std::endl;
        for (int step = 0; step < 100; ++step) {
            brain.step(step, step * 0.001);
            
            if (step % 20 == 0) {
                std::cout << "Step " << step << 
                    ": " << brain.getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }

        std::cout << std::endl;
        std::cout << "=== Experiment Complete ===" << std::endl;
        std::cout << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
    }

    static void runBenchmark(const std::map<std::string, std::string>& options) {
        std::cout << "=== NLM Benchmark Mode ===" << std::endl;
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);

        // Parse neuron counts
        std::vector<int64_t> neuronCounts = {500, 1000, 5000};
        if (options.find("neuron_counts") != options.end()) {
            std::string countsStr = options.at("neuron_counts");
            std::stringstream ss(countsStr);
            std::string item;
            neuronCounts.clear();
            while (std::getline(ss, item, ',')) {
                neuronCounts.push_back(std::stoll(item));
            }
        }

        // Get other options
        int steps = 100;
        if (options.find("steps") != options.end()) {
            steps = std::stoi(options.at("steps"));
        }

        int repeats = 3;
        if (options.find("repeats") != options.end()) {
            repeats = std::stoi(options.at("repeats"));
        }

        std::cout << "Benchmark Configuration:" << std::endl;
        std::cout << "  Neuron counts: ";
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << neuronCounts[i];
        }
        std::cout << std::endl;
        std::cout << "  Steps per run: " << steps << std::endl;
        std::cout << "  Repetitions: " << repeats << std::endl;
        std::cout << std::endl;

        // Run benchmarks
        for (size_t i = 0; i < neuronCounts.size(); ++i) {
            std::cout << "=== Benchmark " << neuronCounts[i] << " neurons ===" << std::endl;
            
            double totalTime = 0.0;
            size_t totalSpikes = 0;
            
            for (int r = 0; r < repeats; ++r) {
                // Create configuration
                auto config = std::make_shared<Config>();
                config->set("neuron_count", neuronCounts[i], ConfigSource::Runtime);
                config->set("region_count", static_cast<int64_t>(1), ConfigSource::Runtime);
                
                nlm::Brain brain(config);
                if (!brain.initialize()) {
                    std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
                    return;
                }

                // Run simulation
                for (int step = 0; step < steps; ++step) {
                    brain.step(step, step * 0.001);
                }

                totalTime += 0.0; // Would need actual timing
                totalSpikes += brain.getTotalSpikeCount();
                
                if (r == repeats - 1) {
                    std::cout << "  Run " << (r + 1) << ": " 
                        << brain.getTotalSpikeCount() << " spikes" << std::endl;
                }
            }

            double avgTime = totalTime / repeats;
            double avgSpikes = static_cast<double>(totalSpikes) / repeats;
            
            std::cout << "  Average: " << avgSpikes << " spikes, " 
                << avgTime << " seconds" << std::endl;
            std::cout << "  Spikes per second: " << (avgSpikes / std::max(avgTime, 0.001)) << std::endl;
            std::cout << std::endl;
        }

        std::cout << "=== Benchmark Complete ===" << std::endl;
    }

    static void runConfigCommands(const std::vector<std::string>& args) {
        std::cout << "=== NLM Config Commands ===" << std::endl;
        
        if (args.empty()) {
            std::cout << "Available commands:" << std::endl;
            std::cout << "  load <file>     Load configuration from file" << std::endl;
            std::cout << "  save <file>     Save configuration to file" << std::endl;
            std::cout << "  list            List all configuration keys" << std::endl;
            std::cout << "  info            Show current configuration" << std::endl;
            return;
        }

        std::string command = args[0];
        
        if (command == "list") {
            auto config = std::make_shared<Config>();
            std::vector<std::string> keys = config->getKeys();
            std::cout << "Configuration keys:" << std::endl;
            for (const auto& key : keys) {
                std::cout << "  " << key << std::endl;
            }
        }
        else if (command == "info") {
            auto config = std::make_shared<Config>();
            std::cout << "Current configuration:" << std::endl;
            std::cout << config->summary() << std::endl;
        }
        else if (command == "load") {
            if (args.size() < 2) {
                std::cout << "ERROR: Missing filename for load command" << std::endl;
                return;
            }
            
            auto config = std::make_shared<Config>();
            if (config->loadFromFile(args[1])) {
                std::cout << "Configuration loaded from: " << args[1] << std::endl;
            } else {
                std::cout << "ERROR: Could not load configuration from: " << args[1] << std::endl;
            }
        }
        else if (command == "save") {
            if (args.size() < 2) {
                std::cout << "ERROR: Missing filename for save command" << std::endl;
                return;
            }
            
            auto config = std::make_shared<Config>();
            if (config->saveToFile(args[1])) {
                std::cout << "Configuration saved to: " << args[1] << std::endl;
            } else {
                std::cout << "ERROR: Could not save configuration to: " << args[1] << std::endl;
            }
        }
        else {
            std::cout << "ERROR: Unknown config command: " << command << std::endl;
        }
    }
};

int main(int argc, char** argv) {
    // Set up help text
    CLIParser parser(argc, argv);
    
    // Print banner
    NLMCLI::printBanner();
    
    // Determine mode
    std::string mode = "demo"; // Default mode
    if (!parser.getPosArg(0).empty()) {
        mode = parser.getPosArg(0);
    }
    
    // Convert to lowercase for comparison
    std::transform(mode.begin(), mode.end(), mode.begin(), 
                   [](unsigned char c) { return std::tolower(c); });
    
    try {
        if (mode == "demo") {
            if (parser.hasOption("help") || parser.hasOption("h")) {
                NLMCLI::printDemoHelp();
                return 0;
            }
            NLMCLI::runDemo(parser.options);
        }
        else if (mode == "experiment") {
            if (parser.hasOption("help") || parser.hasOption("h")) {
                NLMCLI::printExperimentHelp();
                return 0;
            }
            NLMCLI::runExperiment(parser.options);
        }
        else if (mode == "benchmark") {
            if (parser.hasOption("help") || parser.hasOption("h")) {
                NLMCLI::printBenchmarkHelp();
                return 0;
            }
            NLMCLI::runBenchmark(parser.options);
        }
        else if (mode == "config") {
            // Get remaining arguments
            std::vector<std::string> configArgs;
            for (size_t i = 1; i < parser.args.size(); ++i) {
                configArgs.push_back(parser.args[i]);
            }
            NLMCLI::runConfigCommands(configArgs);
        }
        else if (mode == "help") {
            NLMCLI::printUsage();
        }
        else {
            std::cerr << "ERROR: Unknown mode: " << mode << std::endl;
            std::cout << std::endl;
            NLMCLI::printUsage();
            return 1;
        }
    }
    catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
