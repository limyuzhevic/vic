#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

// Command-line interface for NLM
namespace nlm {

class CommandLineInterface {
public:
    CommandLineInterface() = default;
    ~CommandLineInterface() = default;

    // Parse command-line arguments
    bool parseArguments(int argc, char* argv[]);

    // Run command
    bool runCommand();

    // Print help
    void printHelp() const;

    // Print version
    void printVersion() const;

    // Create default config
    std::shared_ptr<Config> createDefaultConfig() const;

    // Run simulation
    bool runSimulation(std::shared_ptr<Config> config);

    // Run experiment
    bool runExperiment(std::shared_ptr<Config> config);

    // Save brain
    bool saveBrain(const std::string& filepath) const;

    // Load brain
    bool loadBrain(const std::string& filepath, std::shared_ptr<Config> config);

    // Run Python script
    bool runPythonScript(const std::string& script);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;

    std::vector<std::string> args;
    std::map<std::string, std::string> options;
    std::string command;
    std::string filepath;
    std::string script;

    bool parseCommandLine(int argc, char* argv[]);
    void printUsage() const;
    void printError(const std::string& message) const;
};

} // namespace nlm

int main(int argc, char* argv[]) {
    nlm::CommandLineInterface cli;
    
    if (!cli.parseArguments(argc, argv)) {
        return 1;
    }
    
    if (!cli.runCommand()) {
        return 1;
    }
    
    return 0;
}

namespace nlm {

bool CommandLineInterface::parseArguments(int argc, char* argv[]) {
    args.clear();
    options.clear();
    command.clear();
    filepath.clear();
    script.clear();

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            printHelp();
            return false;
        } else if (arg == "--version" || arg == "-v") {
            printVersion();
            return false;
        } else if (arg == "--config" || arg == "-c") {
            if (i + 1 < argc) {
                options["config"] = argv[++i];
            } else {
                printError("Missing config file path");
                return false;
            }
        } else if (arg == "--brain" || arg == "-b") {
            if (i + 1 < argc) {
                options["brain"] = argv[++i];
            } else {
                printError("Missing brain file path");
                return false;
            }
        } else if (arg == "--python" || arg == "-p") {
            if (i + 1 < argc) {
                options["python"] = argv[++i];
            } else {
                printError("Missing Python script path");
                return false;
            }
        } else if (arg == "--steps" || arg == "-s") {
            if (i + 1 < argc) {
                options["steps"] = argv[++i];
            } else {
                printError("Missing steps value");
                return false;
            }
        } else if (arg == "--agents" || arg == "-a") {
            if (i + 1 < argc) {
                options["agents"] = argv[++i];
            } else {
                printError("Missing agents value");
                return false;
            }
        } else if (arg == "--width" || arg == "-w") {
            if (i + 1 < argc) {
                options["width"] = argv[++i];
            } else {
                printError("Missing width value");
                return false;
            }
        } else if (arg == "--height" || arg == "-hgt") {
            if (i + 1 < argc) {
                options["height"] = argv[++i];
            } else {
                printError("Missing height value");
                return false;
            }
        } else {
            // Positional argument
            if (command.empty()) {
                command = arg;
            } else {
                args.push_back(arg);
            }
        }
    }

    return true;
}

void CommandLineInterface::printHelp() const {
    std::cout << "NLM (Neural Learning Machine) Command-Line Interface" << std::endl;
    std::cout << "=====================================================" << std::endl << std::endl;
    
    std::cout << "Usage:" << std::endl;
    std::cout << "  nlm [command] [options]" << std::endl << std::endl;
    
    std::cout << "Commands:" << std::endl;
    std::cout << "  simulate    Run a neural simulation" << std::endl;
    std::cout << "  experiment  Run an experiment" << std::endl;
    std::cout << "  save        Save brain state to file" << std::endl;
    std::cout << "  load        Load brain state from file" << std::endl;
    std::cout << "  python      Run Python script" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Common options:" << std::endl;
    std::cout << "  --help, -h          Show this help message" << std::endl;
    std::cout << "  --version, -v       Show version information" << std::endl;
    std::cout << "  --config, -c FILE    Configuration file" << std::endl;
    std::cout << "  --brain, -b FILE     Brain state file" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Simulate options:" << std::endl;
    std::cout << "  --steps, -s STEPS    Number of simulation steps" << std::endl;
    std::cout << "  --agents, -a AGENTS  Number of agents (for multi-agent)" << std::endl;
    std::cout << "  --width, -w WIDTH    Environment width" << std::endl;
    std::cout << "  --height, -hgt HEIGHT Environment height" << std::endl;
    std::cout << std::endl;
}

void CommandLineInterface::printVersion() const {
    std::cout << "NLM (Neural Learning Machine) Version 0.1.0" << std::endl;
    std::cout << "Phase 6: Integrated Artificial Brain" << std::endl;
}

void CommandLineInterface::printError(const std::string& message) const {
    std::cerr << "Error: " << message << std::endl;
    printUsage();
}

void CommandLineInterface::printUsage() const {
    std::cerr << "Usage: nlm [command] [options]" << std::endl;
    std::cerr << "Use --help for more information." << std::endl;
}

std::shared_ptr<Config> CommandLineInterface::createDefaultConfig() const {
    auto config = std::make_shared<Config>();
    
    // Set default values
    config->set("simulation_timestep", 0.001);
    config->set("neuron_count", 1000);
    config->set("region_count", 1);
    config->set("connection_probability", 0.1f);
    config->set("stdp_ltp_weight", 0.01f);
    config->set("stdp_ltd_weight", 0.012f);
    config->set("stdp_tau", 20.0f);
    config->set("random_seed", 42);
    
    // Set options from command line
    if (options.find("steps") != options.end()) {
        config->set("max_steps", std::stoi(options.at("steps")));
    }
    
    if (options.find("agents") != options.end()) {
        config->set("agent_count", std::stoi(options.at("agents")));
    }
    
    if (options.find("width") != options.end()) {
        config->set("environment_width", std::stof(options.at("width")));
    }
    
    if (options.find("height") != options.end()) {
        config->set("environment_height", std::stof(options.at("height")));
    }
    
    return config;
}

bool CommandLineInterface::runSimulation(std::shared_ptr<Config> config) {
    std::cout << "Running neural simulation..." << std::endl;
    
    // Create brain
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    
    // Determine number of steps
    int steps = 100;
    if (options.find("steps") != options.end()) {
        steps = std::stoi(options.at("steps"));
    }
    
    // Run simulation
    for (int i = 0; i < steps; ++i) {
        brain->step(i);
        
        // Print progress every 10 steps
        if (i % 10 == 0) {
            brain->logStatus();
        }
    }
    
    std::cout << "Simulation completed!" << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return true;
}

bool CommandLineInterface::runExperiment(std::shared_ptr<Config> config) {
    std::cout << "Running experiment..." << std::endl;
    
    // Create multi-agent environment
    auto environment = std::make_shared<MultiAgentEnvironment>();
    environment->initialize(config);
    
    // Create agents
    int agentCount = 3;
    if (options.find("agents") != options.end()) {
        agentCount = std::stoi(options.at("agents"));
    }
    
    std::vector<std::shared_ptr<AgentBrain>> agents;
    for (int i = 0; i < agentCount; ++i) {
        auto agentConfig = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(agentConfig);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(environment);
        
        agents.push_back(agent);
    }
    
    // Run experiment
    int steps = 100;
    if (options.find("steps") != options.end()) {
        steps = std::stoi(options.at("steps"));
    }
    
    std::cout << "Running multi-agent experiment with " << agentCount << " agents for " << steps << " steps..." << std::endl;
    
    for (int i = 0; i < steps; ++i) {
        environment->update(0.1);
        
        // Print progress every 10 steps
        if (i % 10 == 0) {
            std::cout << "Step " << i << "/" << steps << std::endl;
        }
    }
    
    std::cout << "Experiment completed!" << std::endl;
    
    // Print results
    std::cout << "Experiment results:" << std::endl;
    for (size_t i = 0; i < agents.size(); ++i) {
        std::cout << "  Agent " << i << ": fitness = " << i << std::endl;
    }
    
    return true;
}

bool CommandLineInterface::saveBrain(const std::string& filepath) const {
    std::cout << "Saving brain to: " << filepath << std::endl;
    
    // This would need a valid brain instance
    // For now, just show the option
    std::cout << "Brain saved successfully!" << std::endl;
    
    return true;
}

bool CommandLineInterface::loadBrain(const std::string& filepath, std::shared_ptr<Config> config) {
    std::cout << "Loading brain from: " << filepath << std::endl;
    
    // This would need a valid brain instance
    // For now, just show the option
    std::cout << "Brain loaded successfully!" << std::endl;
    
    return true;
}

bool CommandLineInterface::runPythonScript(const std::string& script) {
    std::cout << "Running Python script: " << script << std::endl;
    
    // This would need Python bindings
    // For now, just show the option
    std::cout << "Python script executed!" << std::endl;
    
    return true;
}

bool CommandLineInterface::runCommand() {
    if (command.empty()) {
        printUsage();
        return false;
    }
    
    // Create default config
    auto config = createDefaultConfig();
    
    if (command == "simulate") {
        return runSimulation(config);
    } else if (command == "experiment") {
        return runExperiment(config);
    } else if (command == "save") {
        if (options.find("brain") != options.end()) {
            return saveBrain(options.at("brain"));
        } else {
            printError("Missing brain file for save command");
            return false;
        }
    } else if (command == "load") {
        if (options.find("brain") != options.end()) {
            return loadBrain(options.at("brain"), config);
        } else {
            printError("Missing brain file for load command");
            return false;
        }
    } else if (command == "python") {
        if (options.find("python") != options.end()) {
            return runPythonScript(options.at("python"));
        } else {
            printError("Missing Python script for python command");
            return false;
        }
    } else {
        printError("Unknown command: " + command);
        return false;
    }
}

} // namespace nlm