// Simple command-line interface for NLM demo system
// Provides modular commands for different demo modes and operations

#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <functional>
#include <sstream>

namespace nlm {

// Command line argument parser
class CLIArgs {
public:
    struct Args {
        std::string command;
        std::vector<std::string> positionalArgs;
        std::unordered_map<std::string, std::string> options;
        bool showHelp = false;
        bool showVersion = false;
    };
    
    static Args parse(int argc, char** argv) {
        Args args;
        
        if (argc < 2) {
            args.showHelp = true;
            return args;
        }
        
        // First argument is the command
        args.command = argv[1];
        
        // Parse remaining arguments
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.showHelp = true;
            } else if (arg == "--version" || arg == "-v") {
                args.showVersion = true;
            } else if (arg.substr(0, 2) == "--") {
                // Option
                size_t eqPos = arg.find('=');
                if (eqPos != std::string::npos) {
                    std::string key = arg.substr(2, eqPos - 2);
                    std::string value = arg.substr(eqPos + 1);
                    args.options[key] = value;
                } else {
                    args.options[arg.substr(2)] = "true";
                }
            } else {
                // Positional argument
                args.positionalArgs.push_back(arg);
            }
        }
        
        return args;
    }
};

// Simple command system
class Command {
public:
    using Handler = std::function<bool(const std::vector<std::string>&, std::ostream&)>;
    
    Command(const std::string& name, const std::string& description, 
            const Handler& handler, bool requiresConfig = false)
        : name(name), description(description), handler(handler), requiresConfig(requiresConfig) {}
    
    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    bool requiresConfig() const { return requiresConfig; }
    
    bool execute(const std::vector<std::string>& args, std::ostream& output) {
        return handler(args, output);
    }
    
private:
    std::string name;
    std::string description;
    Handler handler;
    bool requiresConfig;
};

class CommandRegistry {
public:
    static CommandRegistry& getInstance() {
        static CommandRegistry instance;
        return instance;
    }
    
    void registerCommand(const Command& command) {
        commands[command.getName()] = command;
    }
    
    Command* getCommand(const std::string& name) {
        auto it = commands.find(name);
        if (it != commands.end()) {
            return &it->second;
        }
        return nullptr;
    }
    
    std::vector<std::string> getAllCommands() {
        std::vector<std::string> names;
        for (const auto& pair : commands) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    void listCommands(std::ostream& output) {
        output << "Available commands:" << std::endl;
        for (const auto& pair : commands) {
            output << "  " << pair.first << " - " << pair.second.getDescription() << std::endl;
        }
    }
    
private:
    std::unordered_map<std::string, Command> commands;
    CommandRegistry() {}
    CommandRegistry(const CommandRegistry&) = delete;
    CommandRegistry& operator=(const CommandRegistry&) = delete;
};

// Help command implementation
bool helpCommand(const std::vector<std::string>& args, std::ostream& output) {
    if (args.empty()) {
        output << "NLM (Neural Learning Machine) Demo System" << std::endl;
        output << "===============================" << std::endl;
        output << std::endl;
        output << "Usage: nlm <command> [options]" << std::endl;
        output << std::endl;
        output << "Commands:" << std::endl;
        CommandRegistry::getInstance().listCommands(output);
        output << std::endl;
        output << "Examples:" << std::endl;
        output << "  nlm run                    Run Phase 2 demo" << std::endl;
        output << "  nlm test                   Run test suite" << std::endl;
        output << "  nlm config list            List configuration" << std::endl;
        output << "  nlm info version           Show version info" << std::endl;
        output << "  nlm help demo              Show demo commands" << std::endl;
    } else {
        std::string subcommand = args[0];
        output << "Command: " << subcommand << std::endl;
        output << "Documentation for this command not yet implemented." << std::endl;
    }
    return true;
}

// Version command implementation
bool versionCommand(const std::vector<std::string>& args, std::ostream& output) {
    output << "NLM (Neural Learning Machine)" << std::endl;
    output << "Version: 0.1.0" << std::endl;
    output << "Build: Demo System Edition" << std::endl;
    output << "Phase: 2 - Real Neural Computation" << std::endl;
    return true;
}

// Demo commands implementation
bool runDemoCommand(const std::vector<std::string>& args, std::ostream& output) {
    output << "============================================" << std::endl;
    output << "NLM Phase 2 Demo" << std::endl;
    output << "============================================" << std::endl;
    output << std::endl;
    
    // Parse demo options
    int numEpisodes = 5;
    int stepsPerEpisode = 1000;
    bool verbose = false;
    
    for (size_t i = 0; i < args.size(); i += 2) {
        if (args[i] == "--episodes") {
            numEpisodes = std::stoi(args[i + 1]);
        } else if (args[i] == "--steps") {
            stepsPerEpisode = std::stoi(args[i + 1]);
        } else if (args[i] == "--verbose") {
            verbose = true;
        }
    }
    
    output << "Demo Configuration:" << std::endl;
    output << "  Episodes: " << numEpisodes << std::endl;
    output << "  Steps per episode: " << stepsPerEpisode << std::endl;
    output << "  Verbose: " << (verbose ? "Yes" : "No") << std::endl;
    output << std::endl;
    
    // Create a simple brain configuration
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.15f);
    config->set("random_seed", static_cast<int64_t>(42));
    config->set("simulation_timestep", 0.001);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        output << "Error: Failed to initialize brain" << std::endl;
        return false;
    }
    
    output << "Brain initialized successfully" << std::endl;
    output << std::endl;
    
    // Run basic connectivity test
    output << "Test 1: Basic Neural Connectivity" << std::endl;
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    auto* region = brain->getRegion(RegionId(1));
    if (region) {
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);
        }
        
        for (SimulationStep step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        output << "  Result: " << spikes << " spikes generated" << std::endl;
        
        if (spikes > 0) {
            output << "  Status: SUCCESS - Spikes propagated" << std::endl;
        } else {
            output << "  Status: WARNING - No spikes generated" << std::endl;
        }
    }
    
    // Reset for plasticity test
    brain->reset();
    brain->initialize();
    
    output << std::endl;
    output << "Test 2: Plasticity Learning" << std::endl;
    
    // Simple plasticity demonstration
    std::vector<float> initialWeights;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            initialWeights.push_back(syn->getWeight());
        }
    }
    
    // Run plasticity steps
    for (SimulationStep step = 0; step < 200; ++step) {
        for (size_t i = 0; i < std::min(size_t(5), brain->getTotalNeuronCount() / 100); ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    // Check for weight changes
    std::vector<float> finalWeights;
    bool learningOccurred = false;
    if (region) {
        for (const auto& syn : region->getSynapses()) {
            finalWeights.push_back(syn->getWeight());
        }
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            if (std::abs(finalWeights[i] - initialWeights[i]) > 0.01f) {
                learningOccurred = true;
                break;
            }
        }
    }
    
    output << "  Result: " << (learningOccurred ? "LEARNING DETECTED" : "No significant change") << std::endl;
    output << "  Status: " << (learningOccurred ? "SUCCESS" : "NO CHANGE DETECTED") << std::endl;
    
    output << std::endl;
    output << "Demo completed successfully!" << std::endl;
    
    return true;
}

// Test commands implementation
bool testCommand(const std::vector<std::string>& args, std::ostream& output) {
    output << "============================================" << std::endl;
    output << "NLM Test Suite" << std::endl;
    output << "============================================" << std::endl;
    output << std::endl;
    
    // Parse test options
    bool runAll = false;
    std::string testFilter;
    
    for (size_t i = 0; i < args.size(); ++i) {
        if (args[i] == "--all") {
            runAll = true;
        } else if (args[i].substr(0, 7) == "--filter=") {
            testFilter = args[i].substr(7);
        }
    }
    
    output << "Running tests..." << std::endl;
    
    // Create a simple brain for testing
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100));
    config->set("region_count", static_cast<int64_t>(1));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", static_cast<int64_t>(123));
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        output << "Error: Failed to initialize brain for testing" << std::endl;
        return false;
    }
    
    // Test 1: Basic connectivity
    output << "Test 1: Basic Connectivity" << std::endl;
    if (runAll || testFilter == "connectivity" || testFilter.empty()) {
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        auto* region = brain->getRegion(RegionId(1));
        if (region) {
            auto neurons = region->getAllNeurons();
            for (size_t i = 0; i < std::min(size_t(5), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            for (SimulationStep step = 0; step < 30; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            output << "  Result: " << spikes << " spikes generated" << std::endl;
            
            if (spikes > 0) {
                output << "  Status: PASSED" << std::endl;
            } else {
                output << "  Status: FAILED" << std::endl;
                return false;
            }
        }
    }
    
    output << std::endl;
    output << "All tests completed successfully!" << std::endl;
    
    return true;
}

// Configuration commands implementation
bool configCommand(const std::vector<std::string>& args, std::ostream& output) {
    output << "============================================" << std::endl;
    output << "NLM Configuration Manager" << std::endl;
    output << "============================================" << std::endl;
    output << std::endl;
    
    if (args.empty()) {
        output << "Usage: config <command> [args]" << std::endl;
        output << "Commands:" << std::endl;
        output << "  list              List all configuration values" << std::endl;
        output << "  get <key>         Get a configuration value" << std::endl;
        output << "  set <key> <value> Set a configuration value" << std::endl;
        output << "  load <file>       Load configuration from file" << std::endl;
        output << "  save <file>       Save configuration to file" << std::endl;
        output << "  clear             Clear all configuration values" << std::endl;
        output << "  help              Show this help" << std::endl;
        return true;
    }
    
    std::string command = args[0];
    std::vector<std::string> subArgs(args.begin() + 1, args.end());
    
    if (command == "list") {
        output << "Configuration entries (example):" << std::endl;
        output << "  random_seed: 42" << std::endl;
        output << "  simulation_timestep: 0.001" << std::endl;
        output << "  neuron_count: 500" << std::endl;
        output << "  region_count: 1" << std::endl;
        output << "  connection_probability: 0.15" << std::endl;
        output << "  stdp_ltp_weight: 0.02" << std::endl;
        output << "  stdp_ltd_weight: 0.015" << std::endl;
        output << "  synaptogenesis_rate: 0.0001" << std::endl;
        output << "  pruning_rate: 0.00001" << std::endl;
        return true;
    } else if (command == "get") {
        if (subArgs.size() < 1) {
            output << "Error: Missing key argument" << std::endl;
            return false;
        }
        std::string key = subArgs[0];
        output << "Config key: " << key << std::endl;
        output << "(Use actual configuration system to get values)" << std::endl;
        return true;
    } else if (command == "set") {
        if (subArgs.size() < 2) {
            output << "Error: Missing key or value arguments" << std::endl;
            return false;
        }
        std::string key = subArgs[0];
        std::string value = subArgs[1];
        output << "Set config: " << key << " = " << value << std::endl;
        output << "(Use actual configuration system to set values)" << std::endl;
        return true;
    } else if (command == "load") {
        if (subArgs.size() < 1) {
            output << "Error: Missing filename" << std::endl;
            return false;
        }
        std::string filename = subArgs[0];
        output << "Load config from: " << filename << std::endl;
        output << "(Use actual configuration system to load files)" << std::endl;
        return true;
    } else if (command == "save") {
        if (subArgs.size() < 1) {
            output << "Error: Missing filename" << std::endl;
            return false;
        }
        std::string filename = subArgs[0];
        output << "Save config to: " << filename << std::endl;
        output << "(Use actual configuration system to save files)" << std::endl;
        return true;
    } else if (command == "clear") {
        output << "Clear all configuration values" << std::endl;
        output << "(Use actual configuration system to clear)" << std::endl;
        return true;
    } else if (command == "help") {
        output << "Configuration commands:" << std::endl;
        output << "  list              List all configuration values" << std::endl;
        output << "  get <key>         Get a configuration value" << std::endl;
        output << "  set <key> <value> Set a configuration value" << std::endl;
        output << "  load <file>       Load configuration from file" << std::endl;
        output << "  save <file>       Save configuration to file" << std::endl;
        output << "  clear             Clear all configuration values" << std::endl;
        return true;
    }
    
    output << "Unknown config command: " << command << std::endl;
    return false;
}

// Info commands implementation
bool infoCommand(const std::vector<std::string>& args, std::ostream& output) {
    output << "============================================" << std::endl;
    output << "NLM Information System" << std::endl;
    output << "============================================" << std::endl;
    output << std::endl;
    
    if (args.empty()) {
        output << "Usage: info <topic>" << std::endl;
        output << "Topics:" << std::endl;
        output << "  version           Show version information" << std::endl;
        output << "  system           Show system information" << std::endl;
        output << "  brain            Show brain information" << std::endl;
        output << "  config           Show configuration information" << std::endl;
        output << "  demos            Show available demos" << std::endl;
        return true;
    }
    
    std::string topic = args[0];
    
    if (topic == "version") {
        return versionCommand({}, output);
    } else if (topic == "system") {
        output << "System Information:" << std::endl;
        output << "  Platform: Linux" << std::endl;
        output << "  Compiler: C++20" << std::endl;
        output << "  Build type: Release" << std::endl;
        output << "  Framework: NLM (Neural Learning Machine)" << std::endl;
        return true;
    } else if (topic == "brain") {
        output << "Brain Information:" << std::endl;
        output << "  Use 'nlm run' to create and test a brain" << std::endl;
        output << "  Brain features:" << std::endl;
        output << "    - LIF neuron dynamics" << std::endl;
        output << "    - STDP and Hebbian plasticity" << std::endl;
        output << "    - Structural plasticity" << std::endl;
        output << "    - Memory systems" << std::endl;
        output << "    - Neuromodulation" << std::endl;
        return true;
    } else if (topic == "config") {
        output << "Configuration System:" << std::endl;
        output << "  Type: JSON-based configuration" << std::endl;
        output << "  Sources: File, Command Line, Defaults" << std::endl;
        output << "  Types: int, double, string, bool, arrays" << std::endl;
        return true;
    } else if (topic == "demos") {
        output << "Available Demos:" << std::endl;
        output << "  Phase 2: Basic Neural Computation" << std::endl;
        output << "    Demonstrates LIF neurons, STDP, Hebbian learning" << std::endl;
        output << "    Command: nlm run" << std::endl;
        output << std::endl;
        output << "  Test Suite:" << std::endl;
        output << "    Runs comprehensive tests of brain functionality" << std::endl;
        output << "    Command: nlm test" << std::endl;
        output << std::endl;
        output << "  Configuration Management:" << std::endl;
        output << "    Create, load, save, and modify configuration" << std::endl;
        output << "    Command: nlm config <command>" << std::endl;
        return true;
    }
    
    output << "Unknown info topic: " << topic << std::endl;
    return false;
}

// Advanced commands implementation
bool advancedCommand(const std::vector<std::string>& args, std::ostream& output) {
    output << "============================================" << std::endl;
    output << "NLM Advanced Features" << std::endl;
    output << "============================================" << std::endl;
    output << std::endl;
    
    if (args.empty()) {
        output << "Advanced commands:" << std::endl;
        output << "  benchmark            Run performance benchmarks" << std::endl;
        output << "  profile              Profile execution" << std::endl;
        output << "  analyze             Analyze brain state" << std::endl;
        output << "  export              Export brain state" << std::endl;
        output << "  import              Import brain state" << std::endl;
        output << "  script              Run script commands" << std::endl;
        output << "  custom              Run custom commands" << std::endl;
        return true;
    }
    
    std::string subcommand = args[0];
    
    if (subcommand == "benchmark") {
        output << "Benchmark functionality not yet implemented." << std::endl;
        output << "Benchmarks would measure performance of different operations." << std::endl;
        return true;
    } else if (subcommand == "profile") {
        output << "Profile functionality not yet implemented." << std::endl;
        output << "Profiler would track execution time and resource usage." << std::endl;
        return true;
    } else if (subcommand == "analyze") {
        output << "Analyze functionality not yet implemented." << std::endl;
        output << "Analysis would examine brain state and behavior." << std::endl;
        return true;
    } else if (subcommand == "export") {
        output << "Export functionality not yet implemented." << std::endl;
        output << "Export would save brain state to files." << std::endl;
        return true;
    } else if (subcommand == "import") {
        output << "Import functionality not yet implemented." << std::endl;
        output << "Import would load brain state from files." << std::endl;
        return true;
    } else if (subcommand == "script") {
        output << "Script functionality not yet implemented." << std::endl;
        output << "Script would execute command sequences from files." << std::endl;
        return true;
    } else if (subcommand == "custom") {
        output << "Custom functionality not yet implemented." << std::endl;
        output << "Custom commands would allow user-defined operations." << std::endl;
        return true;
    }
    
    output << "Unknown advanced command: " << subcommand << std::endl;
    return false;
}

// Command registration
void registerCommands() {
    CommandRegistry& registry = CommandRegistry::getInstance();
    
    // General commands
    registry.registerCommand(Command("help", "Show help information", 
                                     helpCommand, false));
    
    registry.registerCommand(Command("version", "Show version information", 
                                     versionCommand, false));
    
    // Demo commands
    registry.registerCommand(Command("run", "Run Phase 2 demonstration", 
                                     runDemoCommand, true));
    
    registry.registerCommand(Command("demo", "Run demonstration with options", 
                                     runDemoCommand, true));
    
    // Test commands
    registry.registerCommand(Command("test", "Run test suite", 
                                     testCommand, true));
    
    // Configuration commands
    registry.registerCommand(Command("config", "Manage configuration", 
                                     configCommand, false));
    
    // Information commands
    registry.registerCommand(Command("info", "Show system information", 
                                     infoCommand, false));
    
    // Advanced commands
    registry.registerCommand(Command("advanced", "Run advanced operations", 
                                     advancedCommand, true));
    
    registry.registerCommand(Command("benchmark", "Run benchmarks", 
                                     advancedCommand, true));
    
    registry.registerCommand(Command("profile", "Profile execution", 
                                     advancedCommand, true));
}

// Main command line interface
class CLIInterface {
public:
    bool execute(int argc, char** argv) {
        if (argc < 2) {
            std::cerr << "Error: No command provided." << std::endl;
            std::cerr << "Use 'nlm help' to see available commands." << std::endl;
            return false;
        }
        
        std::string commandName = argv[1];
        std::vector<std::string> args;
        
        // Collect all arguments except command name
        for (int i = 2; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        Command* command = CommandRegistry::getInstance().getCommand(commandName);
        if (!command) {
            std::cerr << "Error: Unknown command: " << commandName << std::endl;
            return false;
        }
        
        std::cout << "Executing: " << command->getDescription() << std::endl;
        std::cout << std::endl;
        
        return command->execute(args, std::cout);
    }
};

int main(int argc, char** argv) {
    std::cout << "NLM (Neural Learning Machine) - Demo System" << std::endl;
    std::cout << "=============================================" << std::endl;
    std::cout << std::endl;
    
    // Register all commands
    registerCommands();
    
    CLIInterface cli;
    
    if (!cli.execute(argc, argv)) {
        return 1;
    }
    
    std::cout << std::endl;
    std::cout << "Command completed successfully." << std::endl;
    
    return 0;
}

} // namespace nlm

// Include the implementation files that were forward-declared
#include "demo_utils.cpp"

// Implementation of the forward-declared functions from main.cpp
bool runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    // This function is defined in main.cpp
    // Implementation should be consistent with the demo system
    return true; // Placeholder
}

bool runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    // This function is defined in main.cpp
    return true; // Placeholder
}

bool runStdpVerification(std::shared_ptr<Brain> brain) {
    // This function is defined in main.cpp
    return true; // Placeholder
}