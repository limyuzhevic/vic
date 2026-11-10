// NLM Command Line Interface
// Provides common operations for NLM simulation

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>

using namespace nlm;

// CLI Command definitions
enum class CLICommand {
    Unknown,
    Help,
    Version,
    Info,
    Run,
    ConfigLoad,
    ConfigSave,
    ConfigShow,
    ConfigSet,
    BrainSave,
    BrainLoad,
    Simulate,
    Batch,
    HelpVersionInfo,  // Combined help and version
    Exit
};

struct CLIArgs {
    CLICommand command;
    std::string configFile;
    std::string brainFile;
    std::string outputFile;
    std::vector<std::string> configKeys;
    std::vector<std::string> configValues;
    int steps;
    int neurons;
    double timestep;
    bool verbose;
    bool quiet;
    
    CLIArgs() : command(CLICommand::Help),
                steps(100), neurons(100), timestep(0.001),
                verbose(false), quiet(false) {}
};

// Function prototypes
bool parseArgs(int argc, char** argv, CLIArgs& args);
CLICommand parseCommand(const std::string& cmd);
bool runBasicSimulation(const CLIArgs& args);
bool runFullSimulation(const CLIArgs& args);
bool handleConfigCommands(const CLIArgs& args);
bool handleBrainCommands(const CLIArgs& args);
void printHelp();
void printVersion();
void printInfo(const CLIArgs& args);
std::string formatTime(double seconds);

// Main CLI implementation
int main(int argc, char** argv) {
    // Initialize logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    CLIArgs args;
    
    // Parse command line arguments
    if (!parseArgs(argc, argv, args)) {
        printHelp();
        return 1;
    }
    
    // Execute command
    switch (args.command) {
        case CLICommand::Help:
        case CLICommand::HelpVersionInfo:
            printHelp();
            break;
        case CLICommand::Version:
            printVersion();
            break;
        case CLICommand::Info:
            printInfo(args);
            break;
        case CLICommand::Run:
        case CLICommand::Simulate:
            if (!runBasicSimulation(args)) {
                return 1;
            }
            break;
        case CLICommand::ConfigLoad:
        case CLICommand::ConfigSave:
        case CLICommand::ConfigShow:
        case CLICommand::ConfigSet:
            if (!handleConfigCommands(args)) {
                return 1;
            }
            break;
        case CLICommand::BrainSave:
        case CLICommand::BrainLoad:
            if (!handleBrainCommands(args)) {
                return 1;
            }
            break;
        case CLICommand::Batch:
            // Future: batch processing
            std::cout << "Batch processing not yet implemented" << std::endl;
            break;
        case CLICommand::Unknown:
        case CLICommand::Exit:
        default:
            std::cout << "Unknown or invalid command" << std::endl;
            printHelp();
            return 1;
    }
    
    return 0;
}

bool parseArgs(int argc, char** argv, CLIArgs& args) {
    if (argc < 2) {
        args.command = CLICommand::Help;
        return true;
    }
    
    std::string arg = argv[1];
    args.command = parseCommand(arg);
    
    // Parse additional arguments based on command
    for (int i = 2; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.command = CLICommand::Help;
            break;
        } else if (arg == "--version" || arg == "-v") {
            args.command = CLICommand::Version;
        } else if (arg == "--info") {
            args.command = CLICommand::Info;
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--verbose" || arg == "-V") {
            args.verbose = true;
        } else if (arg.find("--config=") == 0) {
            args.configFile = arg.substr(9);
        } else if (arg.find("--brain=") == 0) {
            args.brainFile = arg.substr(8);
        } else if (arg.find("--output=") == 0) {
            args.outputFile = arg.substr(9);
        } else if (arg.find("--steps=") == 0) {
            args.steps = std::stoi(arg.substr(8));
        } else if (arg.find("--neurons=") == 0) {
            args.neurons = std::stoi(arg.substr(10));
        } else if (arg.find("--timestep=") == 0) {
            args.timestep = std::stod(arg.substr(11));
        } else if (arg.find("--config-key=") == 0) {
            args.configKeys.push_back(arg.substr(13));
        } else if (arg.find("--config-value=") == 0) {
            if (args.configKeys.size() > args.configValues.size()) {
                args.configValues.push_back(arg.substr(16));
            }
        } else if (arg == "--run" || arg == "-r") {
            args.command = CLICommand::Run;
        } else if (arg == "--simulate") {
            args.command = CLICommand::Simulate;
        } else if (arg == "--config-load") {
            args.command = CLICommand::ConfigLoad;
        } else if (arg == "--config-save") {
            args.command = CLICommand::ConfigSave;
        } else if (arg == "--config-show") {
            args.command = CLICommand::ConfigShow;
        } else if (arg == "--brain-save") {
            args.command = CLICommand::BrainSave;
        } else if (arg == "--brain-load") {
            args.command = CLICommand::BrainLoad;
        } else {
            std::cout << "Unknown argument: " << arg << std::endl;
            return false;
        }
    }
    
    return true;
}

CLICommand parseCommand(const std::string& cmd) {
    if (cmd == "help" || cmd == "--help" || cmd == "-h") {
        return CLICommand::Help;
    } else if (cmd == "version" || cmd == "--version" || cmd == "-v") {
        return CLICommand::Version;
    } else if (cmd == "info" || cmd == "--info") {
        return CLICommand::Info;
    } else if (cmd == "run" || cmd == "--run" || cmd == "-r") {
        return CLICommand::Run;
    } else if (cmd == "simulate" || cmd == "--simulate") {
        return CLICommand::Simulate;
    } else if (cmd == "config") {
        return CLICommand::ConfigLoad;
    } else if (cmd == "brain") {
        return CLICommand::BrainSave;
    } else {
        return CLICommand::Unknown;
    }
}

bool runBasicSimulation(const CLIArgs& args) {
    std::cout << "Running NLM basic simulation..." << std::endl;
    std::cout << "Neurons: " << args.neurons << std::endl;
    std::cout << "Steps: " << args.steps << std::endl;
    std::cout << "Timestep: " << args.timestep << "s" << std::endl;
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(args.neurons), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("simulation_timestep", args.timestep, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain" << std::endl;
        return false;
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t step = 0; step < static_cast<size_t>(args.steps); ++step) {
        // Inject some input
        brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        brain->step(step, step * args.timestep);
        
        if (args.verbose && step % 100 == 0) {
            std::cout << "  Step " << step << "/" << args.steps 
                      << " | Firing: " << brain->getFiringNeuronCount() 
                      << " | Spikes: " << brain->getTotalSpikeCount() << std::endl;
        }
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double totalTime = std::chrono::duration<double>(endTime - startTime).count();
    
    std::cout << std::endl;
    std::cout << "Simulation completed in " << formatTime(totalTime) << std::endl;
    std::cout << "Steps per second: " << (args.steps / totalTime) << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    
    return true;
}

bool handleConfigCommands(const CLIArgs& args) {
    auto config = std::make_shared<Config>();
    
    switch (args.command) {
        case CLICommand::ConfigLoad:
            if (args.configFile.empty()) {
                std::cout << "Configuration file required for load command" << std::endl;
                return false;
            }
            if (config->loadFromFile(args.configFile)) {
                std::cout << "Configuration loaded from: " << args.configFile << std::endl;
            } else {
                std::cout << "Failed to load configuration from: " << args.configFile << std::endl;
                return false;
            }
            break;
        case CLICommand::ConfigSave:
            if (args.outputFile.empty()) {
                std::cout << "Output file required for save command" << std::endl;
                return false;
            }
            if (config->saveToFile(args.outputFile)) {
                std::cout << "Configuration saved to: " << args.outputFile << std::endl;
            } else {
                std::cout << "Failed to save configuration to: " << args.outputFile << std::endl;
                return false;
            }
            break;
        case CLICommand::ConfigShow:
            std::cout << config->summary() << std::endl;
            break;
        case CLICommand::ConfigSet:
            for (size_t i = 0; i < args.configKeys.size() && i < args.configValues.size(); ++i) {
                config->set(args.configKeys[i], args.configValues[i], ConfigSource::Runtime);
                std::cout << "Set " << args.configKeys[i] << " = " << args.configValues[i] << std::endl;
            }
            break;
        default:
            std::cout << "Invalid config command" << std::endl;
            return false;
    }
    
    return true;
}

bool handleBrainCommands(const CLIArgs& args) {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cout << "Failed to initialize brain" << std::endl;
        return false;
    }
    
    switch (args.command) {
        case CLICommand::BrainSave:
            if (args.brainFile.empty()) {
                std::cout << "Brain file required for save command" << std::endl;
                return false;
            }
            if (brain->save(args.brainFile)) {
                std::cout << "Brain saved to: " << args.brainFile << std::endl;
            } else {
                std::cout << "Failed to save brain to: " << args.brainFile << std::endl;
                return false;
            }
            break;
        case CLICommand::BrainLoad:
            if (args.brainFile.empty()) {
                std::cout << "Brain file required for load command" << std::endl;
                return false;
            }
            if (brain->load(args.brainFile)) {
                std::cout << "Brain loaded from: " << args.brainFile << std::endl;
            } else {
                std::cout << "Failed to load brain from: " << args.brainFile << std::endl;
                return false;
            }
            break;
        default:
            std::cout << "Invalid brain command" << std::endl;
            return false;
    }
    
    return true;
}

void printHelp() {
    std::cout << "NLM - Neural Learning Machine Command Line Interface" << std::endl;
    std::cout << "=============================================" << std::endl << std::endl;
    
    std::cout << "USAGE:" << std::endl;
    std::cout << "  nlm-cli [command] [options]" << std::endl << std::endl;
    
    std::cout << "COMMANDS:" << std::endl;
    std::cout << "  run, -r              Run basic simulation" << std::endl;
    std::cout << "  simulate              Run simulation with current settings" << std::endl;
    std::cout << "  config                Configuration management" << std::endl;
    std::cout << "  brain                 Brain state management" << std::endl;
    std::cout << "  help, -h, --help      Show this help" << std::endl;
    std::cout << "  version, -v, --version Show version" << std::endl;
    std::cout << "  info, --info          Show system information" << std::endl;
    std::cout << "  quiet, -q             Quiet mode (suppress output)" << std::endl;
    std::cout << "  verbose, -V           Verbose mode (detailed output)" << std::endl << std::endl;
    
    std::cout << "CONFIGURATION COMMANDS:" << std::endl;
    std::cout << "  config --load=file      Load configuration from file" << std::endl;
    std::cout << "  config --save=file      Save configuration to file" << std::endl;
    std::cout << "  config --show           Show current configuration" << std::endl;
    std::cout << "  config --set=key=value  Set configuration value" << std::endl << std::endl;
    
    std::cout << "BRAIN COMMANDS:" << std::endl;
    std::cout << "  brain --save=file       Save brain state" << std::endl;
    std::cout << "  brain --load=file       Load brain state" << std::endl << std::endl;
    
    std::cout << "SIMULATION OPTIONS:" << std::endl;
    std::cout << "  --steps=N              Number of simulation steps (default: 100)" << std::endl;
    std::cout << "  --neurons=N            Number of neurons (default: 100)" << std::endl;
    std::cout << "  --timestep=T           Simulation timestep in seconds (default: 0.001)" << std::endl;
    std::cout << "  --output=file          Output file for simulation results" << std::endl << std::endl;
    
    std::cout << "EXAMPLES:" << std::endl;
    std::cout << "  nlm-cli run --steps=1000 --neurons=500 --output=sim.json" << std::endl;
    std::cout << "  nlm-cli config --load=default.cfg" << std::endl;
    std::cout << "  nlm-cli brain --save=checkpoint.brain" << std::endl;
    std::cout << "  nlm-cli version" << std::endl;
}

void printVersion() {
    std::cout << "NLM Neural Learning Machine" << std::endl;
    std::cout << "Version 0.1.0 (Phase 2 - Real Neural Computation)" << std::endl;
    std::cout << "Built with C++20 and event-driven neural dynamics" << std::endl;
    std::cout << std::endl;
    std::cout << "Key features:" << std::endl;
    std::cout << "  - Leaky Integrate-and-Fire (LIF) neurons" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  - Neuromodulation (reward, curiosity, novelty)" << std::endl;
    std::cout << "  - Agent system with sensory-motor loops" << std::endl;
    std::cout << "  - World simulation with environment interaction" << std::endl;
}

void printInfo(const CLIArgs& args) {
    std::cout << "NLM System Information" << std::endl;
    std::cout << "=====================" << std::endl << std::endl;
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Steps: " << args.steps << std::endl;
    std::cout << "  Neurons: " << args.neurons << std::endl;
    std::cout << "  Timestep: " << args.timestep << "s" << std::endl;
    std::cout << "  Verbose: " << (args.verbose ? "Yes" : "No") << std::endl;
    std::cout << "  Quiet: " << (args.quiet ? "Yes" : "No") << std::endl;
    
    std::cout << std::endl;
    std::cout << "Build Information:" << std::endl;
    std::cout << "  C++ Standard: C++20" << std::endl;
    std::cout << "  Python Bindings: Available (pynlm)" << std::endl;
    std::cout << "  Build Type: Release" << std::endl;
    
    std::cout << std::endl;
    std::cout << "Available Demos:" << std::endl;
    std::cout << "  ./nlm                 - Phase 2 demo (core neural computation)" << std::endl;
    std::cout << "  ./nlm_benchmark       - Performance benchmark" << std::endl;
    std::cout << "  ./nlm_phase3_demo     - World interaction demo" << std::endl;
    std::cout << "  ./nlm_phase4_demo     - Cognitive capabilities demo" << std::endl;
    std::cout << "  ./nlm_phase6_demo     - Full integration test" << std::endl;
}

std::string formatTime(double seconds) {
    std::stringstream ss;
    if (seconds < 60.0) {
        ss << std::fixed << std::setprecision(2) << seconds << "s";
    } else if (seconds < 3600.0) {
        int minutes = static_cast<int>(seconds / 60.0);
        double remaining = seconds - (minutes * 60.0);
        ss << minutes << "m " << std::fixed << std::setprecision(1) << remaining << "s";
    } else {
        int hours = static_cast<int>(seconds / 3600.0);
        double remaining = seconds - (hours * 3600.0);
        int minutes = static_cast<int>(remaining / 60.0);
        remaining -= (minutes * 60.0);
        ss << hours << "h " << minutes << "m " << std::fixed << std::setprecision(1) << remaining << "s";
    }
    return ss.str();
}