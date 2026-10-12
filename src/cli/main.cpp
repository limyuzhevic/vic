// NLM Command Line Interface (CLI)
// Provides user-friendly access to NLM functionality

#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <map>
#include <algorithm>
#include <cstdint>

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"

using namespace nlm;
using namespace std;
namespace chrono = std::chrono;

class NLMCLI {
public:
    NLMCLI() : verbose_(false), quiet_(false) {}
    
    void printBanner() {
        cout << R"(
╔═══════════════════════════════════════════════════════════════╗
║                                                               ║
║     NLM — 熙然                                                ║
║     Neural Learning Machine                                   ║
║                                                               ║
║     Phase 6: Final Integration                                ║
║                                                               ║
║     Advanced Spiking Neural Network                           ║
║     for Research and Development                             ║
║                                                               ║
╚═══════════════════════════════════════════════════════════════╝

Researchers and developers: Build and run experiments with ease! 🎯
                )" << endl;
    }
    
    void printHelp(const string& command = "") {
        if (command.empty()) {
            printBanner();
            cout << R"(
USAGE:
    nlm <command> [options] [arguments]

COMMANDS:
    run          Run a simulation with brain
    experiment   Run predefined experiments
    config       Configuration management
    checkpoint   Checkpoint/save management
    benchmark    Performance benchmarking
    validate     Validate and verify system
    info         Show system information
    help         Show this help message

EXAMPLES:
    nlm run --neurons 1000 --steps 10000
    nlm experiment phase6
    nlm config create myconfig.json
    nlm benchmark memory
    nlm validate integration

For detailed help on a specific command:
    nlm help <command>

Key Features:
    • Unified interface for all NLM functionality
    • JSON/YAML configuration support
    • Smart defaults for quick prototyping
    • Integration with existing C++ backend
    • Enhanced logging and progress reporting
                )" << endl;
        } else if (command == "run") {
            cout << "nlm run - Run a simulation with brain" << endl;
            cout << "  Options:" << endl;
            cout << "    --neurons N        Number of neurons (default: 1000)" << endl;
            cout << "    --regions R        Number of brain regions (default: 1)" << endl;
            cout << "    --steps S          Simulation steps (default: 1000)" << endl;
            cout << "    --timestep T       Simulation timestep in seconds (default: 0.001)" << endl;
            cout << "    --config FILE      Load configuration from file" << endl;
            cout << "    --checkpoint FILE  Save/load checkpoint" << endl;
            cout << "    --enable-dev       Enable development system" << endl;
            cout << "    --enable-curiosity Enable curiosity-driven exploration" << endl;
            cout << "    --enable-replay    Enable episodic replay" << endl;
            cout << "    --verbose          Enable verbose logging" << endl;
            cout << "    --quiet            Quiet mode (errors only)" << endl;
        } else if (command == "experiment") {
            cout << "nlm experiment - Run predefined experiments" << endl;
            cout << "  Built-in experiments:" << endl;
            cout << "    phase3          Phase 3: World interaction" << endl;
            cout << "    phase4          Phase 4: Advanced cognition" << endl;
            cout << "    phase5          Phase 5: Performance" << endl;
            cout << "    phase6          Phase 6: Final integration" << endl;
            cout << "  Options:" << endl;
            cout << "    --neurons N      Number of neurons (default varies by experiment)" << endl;
            cout << "    --steps S        Maximum steps (default: 2000)" << endl;
            cout << "    --config FILE    Load experiment configuration" << endl;
            cout << "    --output FILE    Save results to file" << endl;
            cout << "    --verbose        Enable verbose logging" << endl;
        } else if (command == "config") {
            cout << "nlm config - Configuration management" << endl;
            cout << "  Subcommands:" << endl;
            cout << "    create FILE      Create default configuration" << endl;
            cout << "    load FILE        Load configuration" << endl;
            cout << "    save FILE        Save configuration" << endl;
            cout << "    show FILE        Display configuration" << endl;
            cout << "    edit KEY=VALUE   Edit a configuration value" << endl;
            cout << "    validate FILE    Validate configuration file" << endl;
        } else if (command == "checkpoint") {
            cout << "nlm checkpoint - Checkpoint/save management" << endl;
            cout << "  Options:" << endl;
            cout << "    save NAME        Save current brain state" << endl;
            cout << "    load NAME        Load brain state" << endl;
            cout << "    list            List available checkpoints" << endl;
            cout << "    delete NAME     Delete checkpoint" << endl;
            cout << "    dir DIRECTORY   Set checkpoint directory" << endl;
        }
    }
    
    bool parseArgs(int argc, char** argv, map<string, string>& options) {
        for (int i = 0; i < argc; ++i) {
            string arg = argv[i];
            
            if (arg.substr(0, 2) == "--") {
                size_t pos = arg.find('=');
                if (pos != string::npos) {
                    string key = arg.substr(2, pos - 2);
                    string value = arg.substr(pos + 1);
                    options[key] = value;
                } else {
                    options[arg.substr(2)] = "true";
                }
            }
        }
        return true;
    }
    
    void log(const string& message, bool error = false, bool warn = false) {
        if (quiet_ && !error) return;
        
        if (error) {
            cerr << "[ERROR] " << message << endl;
        } else if (warn) {
            cerr << "[WARN]  " << message << endl;
        } else {
            cout << message << endl;
            if (verbose_) {
                auto logger = Logger::getInstance().getLogger(LogLevel::Info);
                if (logger) {
                    logger->log(message);
                }
            }
        }
    }
    
    // Setters for verbosity
    void setVerbose(bool v) { verbose_ = v; }
    void setQuiet(bool q) { quiet_ = q; }
    
private:
    bool verbose_;
    bool quiet_;
};

int main(int argc, char** argv) {
    NLMCLI cli;
    
    if (argc < 2) {
        cli.printHelp();
        return 0;
    }
    
    string command = argv[1];
    
    if (command == "help") {
        if (argc >= 3) {
            cli.printHelp(argv[2]);
        } else {
            cli.printHelp();
        }
        return 0;
    }
    
    if (command == "run") {
        map<string, string> options;
        cli.parseArgs(argc, argv, options);
        
        // Parse options
        size_t neurons = 1000;
        size_t regions = 1;
        size_t steps = 1000;
        double timestep = 0.001;
        string configFile = "configs/default.cfg";
        string checkpointFile;
        bool enableDev = false;
        bool enableCuriosity = false;
        bool enableReplay = false;
        
        if (options.count("neurons")) neurons = stoull(options["neurons"]);
        if (options.count("regions")) regions = stoull(options["regions"]);
        if (options.count("steps")) steps = stoull(options["steps"]);
        if (options.count("timestep")) timestep = stod(options["timestep"]);
        if (options.count("config")) configFile = options["config"];
        if (options.count("checkpoint")) checkpointFile = options["checkpoint"];
        if (options.count("enable-dev")) enableDev = true;
        if (options.count("enable-curiosity")) enableCuriosity = true;
        if (options.count("enable-replay")) enableReplay = true;
        if (options.count("verbose")) cli.setVerbose(true);
        if (options.count("quiet")) cli.setQuiet(true);
        
        cli.printBanner();
        cli.log("=== NLM Phase 6: Advanced Brain Simulation ===");
        cli.log("Running simulation with configuration:");
        cli.log("  Neurons: " + to_string(neurons));
        cli.log("  Regions: " + to_string(regions));
        cli.log("  Steps: " + to_string(steps));
        cli.log("  Timestep: " + to_string(timestep) + "s");
        cli.log("  Config file: " + configFile);
        cli.log("  Enable development: " + string(enableDev ? "Yes" : "No"));
        cli.log("  Enable curiosity: " + string(enableCuriosity ? "Yes" : "No"));
        cli.log("  Enable replay: " + string(enableReplay ? "Yes" : "No"));
        
        // Initialize logger
        auto logger = make_shared<Logger>();
        auto consoleLogger = make_shared<ConsoleLogger>(LogLevel::Info);
        logger->addLogger(consoleLogger);
        Logger::setGlobal(logger);
        
        // Load configuration
        auto config = make_shared<Config>();
        if (config->loadFromFile(configFile)) {
            cli.log("Loaded configuration from: " + configFile);
        } else {
            cli.log("Using default configuration.");
        }
        
        // Override with command line options
        config->set("neuron_count", static_cast<int64_t>(neurons), ConfigSource::CommandLine);
        config->set("region_count", static_cast<int64_t>(regions), ConfigSource::CommandLine);
        config->set("simulation_timestep", timestep, ConfigSource::CommandLine);
        config->set("enable_development", enableDev, ConfigSource::CommandLine);
        config->set("enable_curiosity", enableCuriosity, ConfigSource::CommandLine);
        config->set("enable_replay", enableReplay, ConfigSource::CommandLine);
        
        // Initialize brain
        cli.log("");
        cli.log("Initializing NLM Brain...");
        auto brain = make_shared<Brain>(config);
        
        if (!brain->initialize()) {
            cli.log("Failed to initialize brain!", true);
            return 1;
        }
        
        brain->logStatus();
        
        // Run simulation
        cli.log("");
        cli.log("=== Running Simulation ===");
        
        auto startTime = chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, step * timestep);
            
            if (step % 100 == 0) {
                cli.log("Step " + to_string(step) + 
                       " | Firing neurons: " + to_string(brain->getFiringNeuronCount()) +
                       " | Total spikes: " + to_string(brain->getTotalSpikeCount()) +
                       " | Average firing rate: " + to_string(brain->getAverageFiringRate()));
            }
            
            // Save checkpoint if requested
            if (!checkpointFile.empty() && step % 1000 == 0) {
                string checkpointName = checkpointFile + "_step_" + to_string(step);
                if (brain->saveCheckpoint(checkpointName)) {
                    cli.log("Checkpoint saved: " + checkpointName);
                } else {
                    cli.log("Failed to save checkpoint: " + checkpointName, true);
                }
            }
        }
        
        auto endTime = chrono::high_resolution_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
        
        brain->logStatus();
        
        cli.log("");
        cli.log("=== Simulation Complete ===");
        cli.log("Total simulation time: " + to_string(duration.count()) + " seconds");
        cli.log("Average performance: " + to_string(steps / duration.count()) + " steps/second");
        
        return 0;
    }
    
    // For now, implement only the 'run' command
    cli.log("Command not yet implemented: " + command, true);
    cli.printHelp();
    return 1;
}
