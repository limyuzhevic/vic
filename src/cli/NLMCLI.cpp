#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/SimpleWorld.hpp"
#include "agent/AgentBrain.hpp"
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace nlm {

class NLMCLI {
public:
    NLMCLI(int argc, char** argv) : argc(argc), argv(argv) {
        parseArgs();
    }

    ~NLMCLI() = default;

    bool run() {
        if (verbose) {
            std::cout << "NLM CLI starting..." << std::endl;
        }
        
        if (helpCommand) {
            printHelp();
            return true;
        }
        
        if (versionCommand) {
            printVersion();
            return true;
        }
        
        if (command.empty()) {
            std::cerr << "Error: No command specified. Use 'nlm help' for available commands." << std::endl;
            return false;
        }
        
        if (command == "help") {
            printHelp();
            return true;
        }
        
        if (command == "version") {
            printVersion();
            return true;
        }
        
        // Initialize logger
        auto logger = std::make_shared<Logger>();
        auto consoleLogger = std::make_shared<ConsoleLogger>(verbose ? LogLevel::Info : LogLevel::Warning);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        if (command == "run") {
            return runCommand();
        } else if (command == "config") {
            return configCommand();
        } else if (command == "experiment") {
            return experimentCommand();
        } else if (command == "checkpoint") {
            return checkpointCommand();
        } else if (command == "visualize") {
            return visualizeCommand();
        } else if (command == "benchmark") {
            return benchmarkCommand();
        } else if (command == "profile") {
            return profileCommand();
        } else {
            std::cerr << "Error: Unknown command: " << command << std::endl;
            return false;
        }
    }

private:
    void parseArgs() {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                helpCommand = true;
            } else if (arg == "--version") {
                versionCommand = true;
            } else if (arg == "--verbose" || arg == "-v") {
                verbose = true;
            } else if (arg == "--quiet" || arg == "-q") {
                quiet = true;
            } else if (arg.find("--") == 0) {
                // Handle --key=value and --key value patterns
                if (arg.find('=') != std::string::npos) {
                    std::string key = arg.substr(2, arg.find('=') - 2);
                    std::string value = arg.substr(arg.find('=') + 1);
                    globalConfig.set(key, value, ConfigSource::CommandLine);
                }
            } else {
                // Non-option argument
                if (command.empty()) {
                    command = arg;
                } else {
                    args.push_back(arg);
                }
            }
        }
    }

    void printHelp() {
        std::cout << "NLM Command Line Interface" << std::endl;
        std::cout << "===========================" << std::endl << std::endl;
        
        std::cout << "Commands:" << std::endl;
        std::cout << "  run           Run a simulation or experiment" << std::endl;
        std::cout << "  config        Configuration management" << std::endl;
        std::cout << "  experiment    Manage experiments" << std::endl;
        std::cout << "  checkpoint    Checkpoint management" << std::endl;
        std::cout << "  visualize     Visualization tools" << std::endl;
        std::cout << "  benchmark     Performance benchmarking" << std::endl;
        std::cout << "  profile        Performance profiling" << std::endl;
        std::cout << "  help          Show help information" << std::endl << std::endl;
        
        std::cout << "Options:" << std::endl;
        std::cout << "  --version     Show version information" << std::endl;
        std::cout << "  --help, -h    Show this help message" << std::endl;
        std::cout << "  --verbose, -v Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q   Suppress non-essential output" << std::endl << std::endl;
        
        std::cout << "For detailed help on any command, use: nlm help <COMMAND>" << std::endl;
    }

    void printVersion() {
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
        std::cout << "Version: Phase 6 Integration" << std::endl;
        std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Copyright: 2024-2026 NLM Research" << std::endl;
    }

    bool runCommand() {
        std::cout << "=== NLM Simulation Run ===" << std::endl;
        
        // Determine config file
        std::string configFile = "configs/default.cfg";
        if (!args.empty()) {
            configFile = args[0];
        }
        
        // Load configuration
        auto config = std::make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            std::cout << "Loaded configuration from: " << configFile << std::endl;
        } else {
            std::cout << "Using default configuration." << std::endl;
        }
        
        // Override with global CLI config
        for (const auto& [key, value] : globalConfig.getKeys()) {
            std::string strValue;
            if (value.index() == typeid(std::string).hash_code()) {
                strValue = std::get<std::string>(value);
            } else if (value.index() == typeid(int64_t).hash_code()) {
                strValue = std::to_string(std::get<int64_t>(value));
            } else if (value.index() == typeid(double).hash_code()) {
                strValue = std::to_string(std::get<double>(value));
            } else if (value.index() == typeid(bool).hash_code()) {
                strValue = std::get<bool>(value) ? "true" : "false";
            }
            config->set(key, strValue, ConfigSource::CommandLine);
        }
        
        // Set default values
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Determine number of steps
        size_t steps = 1000;
        if (args.size() >= 2) {
            try {
                steps = std::stoul(args[1]);
            } catch (...) {
                std::cerr << "Error: Invalid number of steps: " << args[1] << std::endl;
                return false;
            }
        }
        
        std::cout << "Simulation steps: " << steps << std::endl;
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            std::cerr << "Error: Failed to initialize brain!" << std::endl;
            return false;
        }
        
        // Create simple world
        SimpleWorld world;
        world.initialize(16, 16);
        
        // Create agent
        AgentBrain agent(brain);
        agent.initialize(world);
        
        // Run simulation
        std::cout << "Running simulation..." << std::endl;
        for (size_t step = 0; step < steps; ++step) {
            // Get observation
            SensoryPercept percept = world.observe(brain->getRegions()[0].get());
            
            // Process sensory input
            agent.processSensoryInput(percept);
            
            // Brain step
            brain->step(step, step * 0.001);
            
            // Get motor command
            MotorCommand cmd = agent.decodeMotorCommand();
            
            // Apply action to world
            world.applyAction(brain->getRegions()[0].get(), cmd);
            
            // Log progress
            if (step % 100 == 0) {
                std::cout << "Step " << step << 
                    " | Spikes: " << brain->getTotalSpikeCount() <<
                    " | Firing: " << brain->getFiringNeuronCount() <<
                    " | Reward: " << world.getLastReward() << std::endl;
            }
        }
        
        std::cout << "Simulation completed!" << std::endl;
        std::cout << "Final stats:" << std::endl;
        std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        
        return true;
    }

    bool configCommand() {
        std::cout << "=== NLM Configuration Management ===" << std::endl;
        
        if (args.empty()) {
            std::cout << "Available config commands:" << std::endl;
            std::cout << "  show       Show current configuration" << std::endl;
            std::cout << "  set <key> <value>  Set configuration value" << std::endl;
            std::cout << "  get <key>  Get configuration value" << std::endl;
            std::cout << "  save <file> Save configuration to file" << std::endl;
            std::cout << "  load <file> Load configuration from file" << std::endl;
            std::cout << "  list        List all configuration keys" << std::endl;
            return true;
        }
        
        if (args[0] == "show") {
            if (args.size() >= 2) {
                // Show specific key
                std::string key = args[1];
                if (globalConfig.has(key)) {
                    std::cout << key << " = " << globalConfig.getOr<std::string>(key, "") << std::endl;
                } else {
                    std::cout << "Key not found: " << key << std::endl;
                }
            } else {
                // Show all config
                std::cout << globalConfig.summary() << std::endl;
            }
        } else if (args[0] == "set") {
            if (args.size() >= 3) {
                std::string key = args[1];
                std::string value = args[2];
                globalConfig.set(key, value, ConfigSource::Runtime);
                std::cout << "Set " << key << " = " << value << std::endl;
            } else {
                std::cerr << "Error: Missing arguments for set command" << std::endl;
                return false;
            }
        } else if (args[0] == "get") {
            if (args.size() >= 2) {
                std::string key = args[1];
                if (globalConfig.has(key)) {
                    std::cout << globalConfig.getOr<std::string>(key, "") << std::endl;
                } else {
                    std::cout << "Key not found: " << key << std::endl;
                }
            } else {
                std::cerr << "Error: Missing key for get command" << std::endl;
                return false;
            }
        } else if (args[0] == "save") {
            if (args.size() >= 2) {
                if (globalConfig.saveToFile(args[1])) {
                    std::cout << "Configuration saved to: " << args[1] << std::endl;
                } else {
                    std::cerr << "Error: Failed to save configuration to " << args[1] << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Error: Missing filename for save command" << std::endl;
                return false;
            }
        } else if (args[0] == "load") {
            if (args.size() >= 2) {
                if (globalConfig.loadFromFile(args[1])) {
                    std::cout << "Configuration loaded from: " << args[1] << std::endl;
                } else {
                    std::cerr << "Error: Failed to load configuration from " << args[1] << std::endl;
                    return false;
                }
            } else {
                std::cerr << "Error: Missing filename for load command" << std::endl;
                return false;
            }
        } else if (args[0] == "list") {
            std::cout << "Configuration keys:" << std::endl;
            for (const auto& key : globalConfig.getKeys()) {
                std::cout << "  " << key << std::endl;
            }
        } else {
            std::cerr << "Error: Unknown config command: " << args[0] << std::endl;
            return false;
        }
        
        return true;
    }

    bool experimentCommand() {
        std::cout << "=== NLM Experiment Management ===" << std::endl;
        std::cout << "Experiment management not yet implemented." << std::endl;
        return true;
    }

    bool checkpointCommand() {
        std::cout << "=== NLM Checkpoint Management ===" << std::endl;
        std::cout << "Checkpoint management not yet implemented." << std::endl;
        return true;
    }

    bool visualizeCommand() {
        std::cout << "=== NLM Visualization ===" << std::endl;
        std::cout << "Visualization not yet implemented." << std::endl;
        return true;
    }

    bool benchmarkCommand() {
        std::cout << "=== NLM Benchmarking ===" << std::endl;
        std::cout << "Benchmarking not yet implemented." << std::endl;
        return true;
    }

    bool profileCommand() {
        std::cout << "=== NLM Performance Profiling ===" << std::endl;
        std::cout << "Profiling not yet implemented." << std::endl;
        return true;
    }

    // Member variables
    int argc;
    char** argv;
    std::string command;
    std::vector<std::string> args;
    std::map<std::string, ConfigValue> globalConfigData;
    Config globalConfig;
    bool helpCommand = false;
    bool versionCommand = false;
    bool verbose = false;
    bool quiet = false;
};

int main(int argc, char** argv) {
    try {
        NLMCLI cli(argc, argv);
        if (!cli.run()) {
            return 1;
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }
}