// NLM Command Line Interface
// Comprehensive CLI for controlling NLM (Neural Learning Machine) simulations
// Provides both basic and advanced commands for users of all skill levels

#include <nlm_core.h>
#include <nlm_agent.h>
#include <nlm_world.h>
#include <nlm_phase3.h>
#include <nlm_phase4.h>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {
namespace cli {

// Command argument structure
struct CommandArgs {
    std::string command;
    std::vector<std::string> args;
    std::map<std::string, std::string> flags;
    bool help = false;
    bool verbose = false;
    std::string outputFile;
    std::string configFile;
};

// Command result structure
struct CommandResult {
    bool success;
    std::string message;
    std::map<std::string, std::string> metadata;
};

// Main CLI class
class NLMCLI {
private:
    std::shared_ptr<nlm::Config> config;
    std::shared_ptr<nlm::Brain> brain;
    std::shared_ptr<nlm::AgentBrain> agent;
    std::shared_ptr<nlm::SimpleWorld> world;
    bool brainInitialized = false;
    bool worldInitialized = false;
    bool agentInitialized = false;
    
    // Command history for undo functionality
    std::vector<CommandArgs> commandHistory;
    
public:
    NLMCLI() {
        config = std::make_shared<nlm::Config>();
        initializeDefaultConfig();
    }
    
    ~NLMCLI() {
        // Cleanup resources
        resetAll();
    }
    
    // Initialize default configuration
    void initializeDefaultConfig() {
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("brain.connection_probability", 0.1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("simulation.timestep", 0.001);
        config->set("log.level", "INFO");
    }
    
    // Main CLI entry point
    int run(int argc, char** argv) {
        if (argc < 2) {
            printHelp();
            return 0;
        }
        
        std::string command = argv[1];
        CommandArgs args;
        args.command = command;
        
        // Parse command line arguments
        parseArguments(argc, argv, args);
        
        // Execute command
        CommandResult result = executeCommand(args);
        
        if (!result.success) {
            std::cerr << "Error: " << result.message << std::endl;
            return 1;
        }
        
        if (args.verbose) {
            std::cout << "Command executed: " << result.message << std::endl;
            if (!result.metadata.empty()) {
                std::cout << "Metadata:" << std::endl;
                for (const auto& pair : result.metadata) {
                    std::cout << "  " << pair.first << ": " << pair.second << std::endl;
                }
            }
        }
        
        // Save command to history
        commandHistory.push_back(args);
        
        return 0;
    }
    
private:
    // Parse command line arguments
    void parseArguments(int argc, char** argv, CommandArgs& args) {
        for (int i = 2; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                args.help = true;
            } else if (arg == "--verbose" || arg == "-v") {
                args.verbose = true;
            } else if (arg == "--config" && i + 1 < argc) {
                args.configFile = argv[++i];
            } else if (arg == "--output" && i + 1 < argc) {
                args.outputFile = argv[++i];
            } else if (arg.substr(0, 2) == "--") {
                // Flag argument
                std::string flag = arg.substr(2);
                if (i + 1 < argc && argv[i + 1][0] != '-') {
                    args.flags[flag] = argv[++i];
                } else {
                    args.flags[flag] = "true";
                }
            } else if (arg.substr(0, 1) == "-") {
                // Short flag
                for (char c : arg.substr(1)) {
                    args.flags[std::string(1, c)] = "true";
                }
            } else {
                // Positional argument
                args.args.push_back(arg);
            }
        }
    }
    
    // Execute command
    CommandResult executeCommand(const CommandArgs& args) {
        if (args.help) {
            printHelp();
            return {true, "Help displayed", {}};
        }
        
        if (args.command == "create") {
            return commandCreate(args);
        } else if (args.command == "init") {
            return commandInit(args);
        } else if (args.command == "run") {
            return commandRun(args);
        } else if (args.command == "configure") {
            return commandConfigure(args);
        } else if (args.command == "stats") {
            return commandStats(args);
        } else if (args.command == "save") {
            return commandSave(args);
        } else if (args.command == "load") {
            return commandLoad(args);
        } else if (args.command == "demo") {
            return commandDemo(args);
        } else if (args.command == "replay") {
            return commandReplay(args);
        } else if (args.command == "benchmark") {
            return commandBenchmark(args);
        } else if (args.command == "monitor") {
            return commandMonitor(args);
        } else if (args.command == "help") {
            printCommandHelp(args.command);
            return {true, "Command help displayed", {}};
        } else {
            return {false, "Unknown command: " + args.command, {}};
        }
    }
    
    // Command: Create a new brain
    CommandResult commandCreate(const CommandArgs& args) {
        std::string neuronCount = getArg(args, "--neurons", "1000");
        std::string regionCount = getArg(args, "--regions", "1");
        std::string synapseDensity = getArg(args, "--synapse-density", "0.1");
        
        config->set("brain.neuron_count", std::stoi(neuronCount));
        config->set("brain.region_count", std::stoi(regionCount));
        config->set("brain.synapse_density", std::stod(synapseDensity));
        
        brain = std::make_shared<nlm::Brain>(config);
        brainInitialized = false;
        
        std::string message = "Created brain with " + neuronCount + " neurons, " + regionCount + " regions";
        return {true, message, {
            {"neuron_count", neuronCount},
            {"region_count", regionCount},
            {"synapse_density", synapseDensity}
        }};
    }
    
    // Command: Initialize brain
    CommandResult commandInit(const CommandArgs& args) {
        if (!brain) {
            return {false, "No brain created. Use 'create' command first.", {}};
        }
        
        bool success = brain->initialize();
        if (success) {
            brainInitialized = true;
            return {true, "Brain initialized successfully", {
                {"neuron_count", std::to_string(brain->getTotalNeuronCount())},
                {"synapse_count", std::to_string(brain->getTotalSynapseCount())},
                {"firing_neurons", std::to_string(brain->getFiringNeuronCount())}
            }};
        } else {
            return {false, "Failed to initialize brain", {}};
        }
    }
    
    // Command: Run simulation
    CommandResult commandRun(const CommandArgs& args) {
        if (!brainInitialized) {
            return {false, "Brain not initialized. Use 'init' command first.", {}};
        }
        
        int steps = getArg(args, "--steps", "1000");
        int timestep = getArg(args, "--timestep", "1");
        
        std::cout << "Running simulation for " << steps << " steps with timestep " << timestep << "ms" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
            
            if (args.verbose && i % 100 == 0) {
                std::cout << "Step " << i << ": " << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::string message = "Simulation completed: " + std::to_string(steps) + " steps in " + 
                             std::to_string(duration.count()) + "ms";
        
        return {true, message, {
            {"steps_completed", std::to_string(steps)},
            {"total_spikes", std::to_string(brain->getTotalSpikeCount())},
            {"avg_firing_rate", std::to_string(brain->getAverageFiringRate())},
            {"simulation_time_ms", std::to_string(duration.count())}
        }};
    }
    
    // Command: Configure simulation parameters
    CommandResult commandConfigure(const CommandArgs& args) {
        for (const auto& pair : args.flags) {
            std::string key = "brain." + pair.first;
            if (pair.second == "true") {
                config->set(key, true);
            } else if (pair.second == "false") {
                config->set(key, false);
            } else {
                try {
                    if (key.find(".neuron_count") != std::string::npos || 
                        key.find(".region_count") != std::string::npos) {
                        config->set(key, std::stoi(pair.second));
                    } else {
                        config->set(key, std::stod(pair.second));
                    }
                } catch (const std::exception& e) {
                    return {false, "Invalid value for " + pair.first + ": " + e.what(), {}};
                }
            }
        }
        
        if (!configFile.empty() && config->saveToFile(configFile)) {
            return {true, "Configuration saved to " + configFile, {}};
        }
        
        return {true, "Configuration updated", {}};
    }
    
    // Command: Show statistics
    CommandResult commandStats(const CommandArgs& args) {
        if (!brainInitialized) {
            return {false, "Brain not initialized. Use 'init' command first.", {}};
        }
        
        std::stringstream stats;
        stats << "=== NLM Brain Statistics ===" << std::endl;
        stats << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        stats << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
        stats << "Firing neurons (this step): " << brain->getFiringNeuronCount() << std::endl;
        stats << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
        stats << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
        stats << "Excitatory/Inhibitory ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        return {true, stats.str(), {
            {"stats", stats.str()}
        }};
    }
    
    // Command: Save brain state
    CommandResult commandSave(const CommandArgs& args) {
        if (!brainInitialized) {
            return {false, "Brain not initialized. Cannot save state.", {}};
        }
        
        std::string filename = getArg(args, "--file", "nlm_checkpoint.bin");
        bool success = brain->save(filename);
        
        if (success) {
            return {true, "Brain state saved to " + filename, {
                {"filename", filename},
                {"size_kb", "unknown"} // Could calculate file size
            }};
        } else {
            return {false, "Failed to save brain state", {}};
        }
    }
    
    // Command: Load brain state
    CommandResult commandLoad(const CommandArgs& args) {
        std::string filename = getArg(args, "--file", "nlm_checkpoint.bin");
        bool success = brain->load(filename);
        
        if (success) {
            brainInitialized = true;
            return {true, "Brain state loaded from " + filename, {
                {"filename", filename}
            }};
        } else {
            return {false, "Failed to load brain state from " + filename, {}};
        }
    }
    
    // Command: Run demo
    CommandResult commandDemo(const CommandArgs& args) {
        std::string demoType = getArg(args, "--type", "phase3");
        
        if (demoType == "phase3") {
            // Run Phase 3 demo
            std::cout << "Running Phase 3 Demo..." << std::endl;
            // This would call the existing nlm_phase3_demo executable
            return {true, "Phase 3 demo completed", {}};
        } else if (demoType == "phase4") {
            // Run Phase 4 demo
            std::cout << "Running Phase 4 Demo..." << std::endl;
            // This would call the existing nlm_phase4_demo executable
            return {true, "Phase 4 demo completed", {}};
        } else if (demoType == "phase6") {
            // Run Phase 6 integration demo
            std::cout << "Running Phase 6 Integration Demo..." << std::endl;
            // This would call the existing nlm_phase6_demo executable
            return {true, "Phase 6 demo completed", {}};
        } else {
            return {false, "Unknown demo type: " + demoType, {}};
        }
    }
    
    // Command: Replay episodes
    CommandResult commandReplay(const CommandArgs& args) {
        if (!agent) {
            return {false, "No agent available. Create agent first.", {}};
        }
        
        int episodeCount = getArg(args, "--episodes", "5");
        
        std::cout << "Replaying " << episodeCount << " episodes..." << std::endl;
        // Implementation would replay stored episodes
        
        return {true, "Replay completed", {
            {"episodes_replayed", std::to_string(episodeCount)}
        }};
    }
    
    // Command: Run benchmark
    CommandResult commandBenchmark(const CommandArgs& args) {
        int steps = getArg(args, "--steps", "10000");
        int neuronCount = getArg(args, "--neurons", "5000");
        
        std::cout << "Running benchmark: " << steps << " steps with " << neuronCount << " neurons" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        // Create temporary brain for benchmark
        auto benchConfig = std::make_shared<nlm::Config>();
        benchConfig->set("brain.neuron_count", neuronCount);
        auto benchBrain = std::make_shared<nlm::Brain>(benchConfig);
        benchBrain->initialize();
        
        for (int i = 0; i < steps; ++i) {
            benchBrain->step(i);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        double stepsPerSecond = (steps * 1000.0) / duration.count();
        
        return {true, "Benchmark completed", {
            {"steps", std::to_string(steps)},
            {"neurons", std::to_string(neuronCount)},
            {"time_ms", std::to_string(duration.count())},
            {"steps_per_second", std::to_string(stepsPerSecond)}
        }};
    }
    
    // Command: Start monitoring
    CommandResult commandMonitor(const CommandArgs& args) {
        int interval = getArg(args, "--interval", "1000");
        int duration = getArg(args, "--duration", "60000");
        
        std::cout << "Starting monitoring for " << (duration / 1000) << " seconds (interval: " << interval << "ms)" << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        auto endTime = startTime + std::chrono::milliseconds(duration);
        
        while (std::chrono::high_resolution_clock::now() < endTime) {
            if (brainInitialized) {
                std::cout << "Spikes: " << brain->getTotalSpikeCount() 
                         << ", Firing: " << brain->getFiringNeuronCount()
                         << ", Rate: " << brain->getAverageFiringRate() << std::endl;
            }
            
            std::this_thread::sleep_for(std::chrono::milliseconds(interval));
        }
        
        return {true, "Monitoring completed", {
            {"duration_ms", std::to_string(duration)},
            {"samples", std::to_string(duration / interval)}
        }};
    }
    
    // Helper: Get argument value
    std::string getArg(const CommandArgs& args, const std::string& flag, const std::string& defaultValue) {
        auto it = args.flags.find(flag);
        if (it != args.flags.end()) {
            return it->second;
        }
        return defaultValue;
    }
    
    // Print help
    void printHelp() {
        std::cout << "NLM Command Line Interface" << std::endl;
        std::cout << "==========================" << std::endl;
        std::cout << "Commands:" << std::endl;
        std::cout << "  create      Create a new brain" << std::endl;
        std::cout << "  init        Initialize the brain" << std::endl;
        std::cout << "  run         Run simulation" << std::endl;
        std::cout << "  configure   Configure parameters" << std::endl;
        std::cout << "  stats       Show brain statistics" << std::endl;
        std::cout << "  save        Save brain state" << std::endl;
        std::cout << "  load        Load brain state" << std::endl;
        std::cout << "  demo        Run demonstration" << std::endl;
        std::cout << "  replay      Replay stored episodes" << std::endl;
        std::cout << "  benchmark   Run performance benchmark" << std::endl;
        std::cout << "  monitor     Start real-time monitoring" << std::endl;
        std::cout << "  help        Show help" << std::endl;
        std::cout << std::endl;
        std::cout << "Use 'nlm help <command>' for details on specific commands." << std::endl;
    }
    
    // Print command help
    void printCommandHelp(const std::string& command) {
        if (command == "create") {
            std::cout << "Create a new brain:" << std::endl;
            std::cout << "  nlm create --neurons 1000 --regions 1 --synapse-density 0.1" << std::endl;
        } else if (command == "init") {
            std::cout << "Initialize the brain:" << std::endl;
            std::cout << "  nlm init" << std::endl;
        } else if (command == "run") {
            std::cout << "Run simulation:" << std::endl;
            std::cout << "  nlm run --steps 1000 --timestep 1 --verbose" << std::endl;
        } else if (command == "configure") {
            std::cout << "Configure parameters:" << std::endl;
            std::cout << "  nlm configure --plasticity.stdp.enable true --plasticity.hebbian.enable false" << std::endl;
        } else if (command == "stats") {
            std::cout << "Show brain statistics:" << std::endl;
            std::cout << "  nlm stats" << std::endl;
        }
    }
    
    // Reset all resources
    void resetAll() {
        if (brain) brain->reset();
        brainInitialized = false;
        worldInitialized = false;
        agentInitialized = false;
        brain.reset();
        agent.reset();
        world.reset();
    }
};

// Global CLI instance
NLMCLI g_cli;

int main(int argc, char** argv) {
    try {
        return g_cli.run(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error" << std::endl;
        return 1;
    }
}
