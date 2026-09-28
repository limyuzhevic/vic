#include "NLMCli.hpp"
#include <algorithm>
#include <sstream>
#include <iomanip>

namespace nlm {

bool NLMCli::parseArguments(int argc, char* argv[]) {
    args.command = "run"; // Default command
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.helpRequested = true;
        } else if (arg == "--version") {
            args.versionRequested = true;
        } else if (arg == "run") {
            args.command = "run";
        } else if (arg == "demo") {
            args.command = "demo";
        } else if (arg == "config") {
            args.command = "config";
        } else if (arg == "list") {
            args.command = "list";
        } else if (arg == "--help" || arg == "-h") {
            args.helpRequested = true;
        } else if (arg == "--version") {
            args.versionRequested = true;
        } else if (arg.find("--") == 0) {
            // Option argument (e.g., --episodes 5)
            std::string option = arg.substr(2);
            size_t pos = option.find('=');
            if (pos != std::string::npos) {
                args.options[option.substr(0, pos)] = option.substr(pos + 1);
            } else {
                args.options[option] = "";
            }
        } else {
            // Positional argument
            args.positionalArgs.push_back(arg);
        }
    }
    
    return true;
}

void NLMCli::executeCommand() {
    if (args.helpRequested) {
        executeHelpCommand();
        return;
    }
    
    if (args.versionRequested) {
        executeVersionCommand();
        return;
    }
    
    if (args.command == "run") {
        executeRunCommand();
    } else if (args.command == "demo") {
        if (!args.positionalArgs.empty()) {
            executeDemoCommand(args.positionalArgs[0]);
        } else {
            throw std::runtime_error("Demo name required. Use 'list' to see available demos.");
        }
    } else if (args.command == "config") {
        executeConfigCommand();
    } else if (args.command == "list") {
        executeListCommand();
    } else {
        throw std::runtime_error("Unknown command: " + args.command);
    }
}

std::string NLMCli::generateHelpText() const {
    std::stringstream ss;
    
    ss << "NLM - Neural Learning Machine Command Line Interface\n";
    ss << "======================================================\n\n";
    
    ss << "USAGE:\n";
    ss << "  nlm [OPTIONS] [COMMAND] [ARGS...]\n\n";
    
    ss << "COMMANDS:\n";
    ss << "  run          Run a simulation (default command)\n";
    ss << "  demo <name>  Run a specific demo\n";
    ss << "  config       Display or modify configuration\n";
    ss << "  list         List available commands and demos\n\n";
    
    ss << "OPTIONS:\n";
    ss << "  --help, -h     Show this help message\n";
    ss << "  --version      Show version information\n";
    ss << "  --config=FILE  Load configuration from file\n";
    ss << "  --output=FILE  Save output to file\n";
    ss << "  --verbose, -v  Enable verbose output\n\n";
    
    ss << "DEMO NAMES:\n";
    ss << "  phase3       Phase 3: Basic world interaction\n";
    ss << "  phase4       Phase 4: Emerging cognition\n";
    ss << "  phase6       Phase 6: Final integration\n\n";
    
    ss << "EXAMPLES:\n";
    ss << "  nlm                                    # Run default simulation\n";
    ss << "  nlm --config=myconfig.json demo phase6 # Run Phase 6 demo with config\n";
    ss << "  nlm --episodes=10 --steps=200 run     # Run with custom parameters\n";
    ss << "  nlm demo phase3                       # Run Phase 3 demo\n\n";
    
    ss << "For more information about a specific command, use:\n";
    ss << "  nlm <command> --help\n";
    
    return ss.str();
}

void NLMCli::executeRunCommand() const {
    std::cout << "Running NLM simulation...\n";
    
    // Create configuration from command line options
    auto config = createConfigFromArgs();
    
    // Parse common simulation parameters
    int episodes = 5;
    int stepsPerEpisode = 200;
    
    if (args.options.find("episodes") != args.options.end()) {
        episodes = std::stoi(args.options.at("episodes"));
    }
    
    if (args.options.find("steps") != args.options.end()) {
        stepsPerEpisode = std::stoi(args.options.at("steps"));
    }
    
    std::cout << "Configuration:\n";
    std::cout << "  Episodes: " << episodes << "\n";
    std::cout << "  Steps per episode: " << stepsPerEpisode << "\n";
    std::cout << "  Neuron count: " << config->getOr<size_t>("neuron_count", 1000) << "\n";
    std::cout << "  Region count: " << config->getOr<size_t>("region_count", 2) << "\n\n";
    
    // For now, run a simple simulation
    // In a full implementation, this would create a brain, world, and agent
    // and run the simulation loop
    
    std::cout << "Simulation would run " << episodes << " episodes of " << stepsPerEpisode 
              << " steps each.\n";
    std::cout << "Total steps: " << episodes * stepsPerEpisode << "\n";
    
    // Placeholder for actual simulation
    // This would involve creating Brain, SimpleWorld, and AgentBrain objects
    // and running the simulation loop as shown in the demo files
    
    std::cout << "Simulation completed!\n";
}

void NLMCli::executeDemoCommand(const std::string& demoName) const {
    std::cout << "Running " << demoName << " demo...\n";
    
    if (demoName == "phase3") {
        std::cout << "Phase 3 demo: World interaction and learning\n";
        std::cout << "This demo shows how brains learn from world interactions.\n";
        // Would create and run Phase3Demo
    } else if (demoName == "phase4") {
        std::cout << "Phase 4 demo: Emerging cognition\n";
        std::cout << "This demo shows how complex behaviors emerge from neural dynamics.\n";
        // Would create and run Phase4Demo
    } else if (demoName == "phase6") {
        std::cout << "Phase 6 demo: Final integration\n";
        std::cout << "This demo shows the complete integrated brain system.\n";
        // Would create and run Phase6Demo
    } else {
        throw std::runtime_error("Unknown demo: " + demoName);
    }
    
    std::cout << demoName << " demo completed!\n";
}

void NLMCli::executeConfigCommand() const {
    std::cout << "NLM Configuration System\n";
    std::cout << "=========================\n\n";
    
    // Create default config
    auto config = std::make_shared<Config>();
    
    // Add some common configuration options
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    
    std::cout << "Current configuration keys:\n";
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        std::cout << "  " << key << "\n";
    }
    
    std::cout << "\nConfiguration summary:\n";
    std::cout << config->summary() << "\n";
}

void NLMCli::executeListCommand() const {
    std::cout << "Available Commands:\n";
    std::cout << "  run          - Run a simulation with customizable parameters\n";
    std::cout << "  demo <name>  - Run a specific demo (phase3, phase4, phase6)\n";
    std::cout << "  config       - Display and manage configuration\n";
    std::cout << "  list         - List all available commands\n\n";
    
    std::cout << "Available Demos:\n";
    std::cout << "  phase3       - Basic world interaction and learning\n";
    std::cout << "  phase4       - Emerging cognition experiments\n";
    std::cout << "  phase6       - Final integration demonstration\n\n";
    
    std::cout << "Configuration Parameters:\n";
    std::cout << "  neuron_count             - Number of neurons in the brain\n";
    std::cout << "  region_count             - Number of brain regions\n";
    std::cout << "  connection_probability   - Probability of synaptic connections\n";
    std::cout << "  random_seed              - Random number generator seed\n";
    std::cout << "  simulation_timestep      - Simulation time step in seconds\n";
    std::cout << "  stdp_ltp_weight          - STDP long-term potentiation weight\n";
    std::cout << "  stdp_ltd_weight          - STDP long-term depression weight\n";
    std::cout << "  stdp_tau                 - STDP time constant\n\n";
    
    std::cout << "Example commands:\n";
    std::cout << "  nlm                          # Run default simulation\n";
    std::cout << "  nlm demo phase6              # Run Phase 6 demo\n";
    std::cout << "  nlm --episodes=10 --steps=200 run\n";
    std::cout << "  nlm --config=myconfig.json demo phase3\n";
}

void NLMCli::executeHelpCommand() const {
    std::cout << helpText << std::endl;
}

void NLMCli::executeVersionCommand() const {
    std::cout << "NLM - Neural Learning Machine v0.1.0\n";
    std::cout << "Phase 6: Final Integration\n";
    std::cout << "Build with CMake " << CMAKE_VERSION << "\n";
    std::cout << "C++20 compatible\n";
}

std::shared_ptr<Config> NLMCli::createConfigFromArgs() const {
    auto config = std::make_shared<Config>();
    
    // Set some default values
    config->set("neuron_count", size_t(1000));
    config->set("region_count", size_t(2));
    config->set("connection_probability", 0.1f);
    config->set("random_seed", uint64_t(42));
    config->set("simulation_timestep", 0.001);
    
    // Override with command line options if present
    if (args.options.find("neuron_count") != args.options.end()) {
        config->set("neuron_count", static_cast<size_t>(std::stoi(args.options.at("neuron_count"))));
    }
    
    if (args.options.find("region_count") != args.options.end()) {
        config->set("region_count", static_cast<size_t>(std::stoi(args.options.at("region_count"))));
    }
    
    if (args.options.find("connection_probability") != args.options.end()) {
        config->set("connection_probability", std::stof(args.options.at("connection_probability")));
    }
    
    if (args.options.find("random_seed") != args.options.end()) {
        config->set("random_seed", static_cast<uint64_t>(std::stoull(args.options.at("random_seed"))));
    }
    
    if (args.options.find("simulation_timestep") != args.options.end()) {
        config->set("simulation_timestep", std::stod(args.options.at("simulation_timestep")));
    }
    
    return config;
}

std::string NLMCli::formatOptions(const std::map<std::string, std::string>& options) const {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& pair : options) {
        if (!first) {
            ss << ", ";
        }
        ss << pair.first << "=" << pair.second;
        first = false;
    }
    
    return ss.str();
}

void NLMCli::printUsage(const std::string& command) const {
    std::cout << "Usage: nlm " << command << " [options] [args]...\n";
    std::cout << "Use 'nlm --help' for more information.\n";
}

} // namespace nlm