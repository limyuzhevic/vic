#include "CommandLineInterface.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>
#include <iomanip>
#include <thread>
#include <functional>
#include <map>

namespace nlm {

struct CommandLineInterface::Impl {
    // Core components
    std::shared_ptr<Config> config;
    std::shared_ptr<Brain> brain;
    
    // Command registry
    std::map<std::string, std::function<bool(const std::vector<std::string>&)>> commandHandlers;
    
    // Command history
    std::vector<std::string> commandHistory;
    size_t historyMaxSize;
    
    // Performance monitoring
    bool performanceMonitoringEnabled;
    std::chrono::system_clock::time_point simulationStartTime;
    std::chrono::duration<double> totalSimulationTime;
    size_t totalStepsExecuted;
    
    // Simulation state
    bool simulationRunning;
    bool interactiveMode;
    
    // Output formatting
    std::string prompt;
    bool verboseOutput;
    
    Impl() : historyMaxSize(100)
           , performanceMonitoringEnabled(false)
           , totalSimulationTime(0.0)
           , totalStepsExecuted(0)
           , simulationRunning(false)
           , interactiveMode(false)
           , verboseOutput(false)
           , prompt("> ")
    {
        initializeCommandHandlers();
    }
    
    void initializeCommandHandlers() {
        // Basic commands
        commandHandlers["help"] = [this](const std::vector<std::string>& args) { return handleHelp(args); };
        commandHandlers["set"] = [this](const std::vector<std::string>& args) { return handleSet(args); };
        commandHandlers["get"] = [this](const std::vector<std::string>& args) { return handleGet(args); };
        commandHandlers["run"] = [this](const std::vector<std::string>& args) { return handleRun(args); };
        commandHandlers["stats"] = [this](const std::vector<std::string>& args) { return handleStats(args); };
        commandHandlers["save"] = [this](const std::vector<std::string>& args) { return handleSave(args); };
        commandHandlers["load"] = [this](const std::vector<std::string>& args) { return handleLoad(args); };
        commandHandlers["reset"] = [this](const std::vector<std::string>& args) { return handleReset(args); };
        commandHandlers["step"] = [this](const std::vector<std::string>& args) { return handleStep(args); };
        commandHandlers["exit"] = [this](const std::vector<std::string>& args) { return handleExit(args); };
        commandHandlers["quit"] = [this](const std::vector<std::string>& args) { return handleExit(args); };
        
        // Simulation control
        commandHandlers["start"] = [this](const std::vector<std::string>& args) { 
            if (!simulationRunning) {
                simulationRunning = true;
                startPerformanceMonitoring();
                std::cout << "Simulation started.\n";
                return true;
            }
            std::cout << "Simulation already running.\n";
            return false;
        };
        
        commandHandlers["stop"] = [this](const std::vector<std::string>& args) {
            if (simulationRunning) {
                simulationRunning = false;
                stopPerformanceMonitoring();
                std::cout << "Simulation stopped.\n";
                return true;
            }
            std::cout << "Simulation not running.\n";
            return false;
        };
        
        commandHandlers["status"] = [this](const std::vector<std::string>& args) {
            printSimulationStatus();
            return true;
        };
    }
};

CommandLineInterface::CommandLineInterface() : pImpl(std::make_unique<Impl>()) {}

CommandLineInterface::~CommandLineInterface() = default;

bool CommandLineInterface::initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain) {
    if (!config || !brain) {
        std::cerr << "Error: Invalid configuration or brain provided.\n";
        return false;
    }
    
    pImpl->config = config;
    pImpl->brain = brain;
    
    std::cout << "NLM Command Line Interface initialized.\n";
    std::cout << "Type 'help' for available commands.\n";
    
    return true;
}

bool CommandLineInterface::parseCommand(const std::string& commandLine) {
    // Trim whitespace
    std::string trimmed = commandLine;
    size_t start = trimmed.find_first_not_of(" \t\r\n");
    size_t end = trimmed.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return true;  // Empty line
    }
    trimmed = trimmed.substr(start, end - start + 1);
    
    // Add to history
    addToHistory(trimmed);
    
    // Parse command and arguments
    std::vector<std::string> tokens;
    std::istringstream iss(trimmed);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    if (tokens.empty()) {
        return true;
    }
    
    std::string command = tokens[0];
    std::vector<std::string> args(tokens.begin() + 1, tokens.end());
    
    // Find and execute command handler
    auto it = pImpl->commandHandlers.find(command);
    if (it != pImpl->commandHandlers.end()) {
        return it->second(args);
    } else {
        std::cout << "Unknown command: " << command << "\n";
        std::cout << "Type 'help' for available commands.\n";
        return false;
    }
}

bool CommandLineInterface::executeCommand(const std::string& command, const std::vector<std::string>& args) {
    auto it = pImpl->commandHandlers.find(command);
    if (it != pImpl->commandHandlers.end()) {
        return it->second(args);
    } else {
        std::cout << "Unknown command: " << command << "\n";
        return false;
    }
}

bool CommandLineInterface::handleHelp(const std::vector<std::string>& args) {
    std::cout << "=== NLM Command Line Interface ===\n";
    std::cout << "\nBasic Commands:\n";
    std::cout << "  help                    Show this help message\n";
    std::cout << "  set <key> <value>       Set a configuration value\n";
    std::cout << "  get <key>               Get a configuration value\n";
    std::cout << "  run <steps> [timestep]   Run simulation for specified steps\n";
    std::cout << "  stats                   Show simulation statistics\n";
    std::cout << "  save <file>             Save brain state to file\n";
    std::cout << "  load <file>             Load brain state from file\n";
    std::cout << "  reset                   Reset simulation\n";
    std::cout << "  step <steps>            Run specified number of simulation steps\n";
    
    std::cout << "\nSimulation Control:\n";
    std::cout << "  start                   Start simulation\n";
    std::cout << "  stop                    Stop simulation\n";
    std::cout << "  status                  Show simulation status\n";
    
    std::cout << "\nOther Commands:\n";
    std::cout << "  exit, quit              Exit command line interface\n";
    
    return true;
}

bool CommandLineInterface::handleSet(const std::vector<std::string>& args) {
    if (args.size() < 2) {
        std::cout << "Usage: set <key> <value>\n";
        return false;
    }
    
    std::string key = args[0];
    std::string value = args[1];
    
    if (pImpl->config) {
        try {
            // Try to parse value as different types
            if (value == "true" || value == "false") {
                bool boolValue = (value == "true");
                pImpl->config->set(key, boolValue, ConfigSource::Runtime);
            } else {
                // Try to parse as float
                size_t pos;
                float floatValue = std::stof(value, &pos);
                if (pos == value.size()) {
                    pImpl->config->set(key, floatValue, ConfigSource::Runtime);
                } else {
                    // Try to parse as int
                    int intValue = std::stoi(value, &pos);
                    if (pos == value.size()) {
                        pImpl->config->set(key, intValue, ConfigSource::Runtime);
                    } else {
                        // Treat as string
                        pImpl->config->set(key, value, ConfigSource::Runtime);
                    }
                }
            }
            
            std::cout << "Set " << key << " = " << value << "\n";
            return true;
        } catch (const std::exception& e) {
            std::cout << "Error setting configuration: " << e.what() << "\n";
            return false;
        }
    }
    
    std::cout << "No configuration available.\n";
    return false;
}

bool CommandLineInterface::handleGet(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: get <key>\n";
        return false;
    }
    
    std::string key = args[0];
    
    if (pImpl->config && pImpl->config->has(key)) {
        // Try to get value with different type hints
        try {
            bool boolVal = pImpl->config->getOr<bool>(key, false);
            if (boolVal != false || pImpl->config->get<bool>(key)) {
                std::cout << key << " = " << (boolVal ? "true" : "false") << "\n";
                return true;
            }
        } catch (...) {}
        
        try {
            int intVal = pImpl->config->getOr<int>(key, 0);
            if (pImpl->config->get<int>(key)) {
                std::cout << key << " = " << intVal << "\n";
                return true;
            }
        } catch (...) {}
        
        try {
            float floatVal = pImpl->config->getOr<float>(key, 0.0f);
            if (pImpl->config->get<float>(key)) {
                std::cout << key << " = " << floatVal << "\n";
                return true;
            }
        } catch (...) {}
        
        try {
            std::string stringVal = pImpl->config->getOr<std::string>(key, "");
            if (pImpl->config->get<std::string>(key)) {
                std::cout << key << " = \"" << stringVal << "\"\n";
                return true;
            }
        } catch (...) {}
        
        // If no specific type found, just show summary
        std::cout << key << " = " << pImpl->config->summary() << "\n";
        return true;
    }
    
    std::cout << "Key not found: " << key << "\n";
    return false;
}

bool CommandLineInterface::handleRun(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: run <steps> [timestep]\n";
        return false;
    }
    
    try {
        size_t steps = std::stoul(args[0]);
        double timestep = 0.001; // Default 1ms
        
        if (args.size() > 1) {
            timestep = std::stod(args[1]);
        }
        
        return runSimulation(steps, timestep);
    } catch (const std::exception& e) {
        std::cout << "Error running simulation: " << e.what() << "\n";
        return false;
    }
}

bool CommandLineInterface::handleStats(const std::vector<std::string>& args) {
    printSimulationStatus();
    printPerformanceStats();
    return true;
}

bool CommandLineInterface::handleSave(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: save <file>\n";
        return false;
    }
    
    return saveConfigToFile(args[0]);
}

bool CommandLineInterface::handleLoad(const std::vector<std::string>& args) {
    if (args.empty()) {
        std::cout << "Usage: load <file>\n";
        return false;
    }
    
    return loadConfigFromFile(args[0]);
}

bool CommandLineInterface::handleReset(const std::vector<std::string>& args) {
    resetSimulation();
    std::cout << "Simulation reset.\n";
    return true;
}

bool CommandLineInterface::handleStep(const std::vector<std::string>& args) {
    size_t steps = 1;
    if (!args.empty()) {
        try {
            steps = std::stoul(args[0]);
        } catch (const std::exception& e) {
            std::cout << "Error parsing steps: " << e.what() << "\n";
            return false;
        }
    }
    
    return stepSimulation(steps);
}

bool CommandLineInterface::handleExit(const std::vector<std::string>& args) {
    std::cout << "Exiting command line interface.\n";
    return true;
}

bool CommandLineInterface::runSimulation(size_t steps, double timestep) {
    if (!pImpl->brain) {
        std::cout << "No brain available for simulation.\n";
        return false;
    }
    
    if (pImpl->simulationRunning) {
        std::cout << "Simulation already running.\n";
        return false;
    }
    
    std::cout << "Running simulation for " << steps << " steps (timestep: " << timestep << "s)...\n";
    
    startPerformanceMonitoring();
    pImpl->simulationRunning = true;
    
    for (size_t i = 0; i < steps && pImpl->simulationRunning; ++i) {
        pImpl->brain->step(i, timestep * i);
        pImpl->totalStepsExecuted++;
        
        if (pImpl->verboseOutput) {
            std::cout << "Step " << i << ": " 
                     << pImpl->brain->getTotalSpikeCount() << " spikes, "
                     << pImpl->brain->getFiringNeuronCount() << " firing neurons\n";
        }
    }
    
    pImpl->simulationRunning = false;
    stopPerformanceMonitoring();
    
    std::cout << "Simulation completed.\n";
    std::cout << "Total spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
    std::cout << "Total steps: " << pImpl->totalStepsExecuted << "\n";
    
    return true;
}

bool CommandLineInterface::stepSimulation(size_t steps) {
    if (!pImpl->brain) {
        std::cout << "No brain available for simulation.\n";
        return false;
    }
    
    std::cout << "Running " << steps << " simulation steps...\n";
    
    startPerformanceMonitoring();
    
    for (size_t i = 0; i < steps; ++i) {
        pImpl->brain->step(i, 0.001 * i); // Default 1ms timestep
        pImpl->totalStepsExecuted++;
    }
    
    stopPerformanceMonitoring();
    
    std::cout << "Completed " << steps << " steps.\n";
    return true;
}

bool CommandLineInterface::setConfigValue(const std::string& key, const std::string& value) {
    return handleSet({key, value});
}

std::string CommandLineInterface::getConfigValue(const std::string& key) const {
    if (!pImpl->config || !pImpl->config->has(key)) {
        return "";
    }
    
    // Try to get as string for simplicity
    try {
        return pImpl->config->getOr<std::string>(key, "");
    } catch (...) {
        return "";
    }
}

bool CommandLineInterface::saveConfigToFile(const std::string& filepath) const {
    if (!pImpl->config) {
        std::cout << "No configuration available.\n";
        return false;
    }
    
    try {
        if (pImpl->config->saveToFile(filepath)) {
            std::cout << "Configuration saved to: " << filepath << "\n";
            return true;
        } else {
            std::cout << "Failed to save configuration to: " << filepath << "\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "Error saving configuration: " << e.what() << "\n";
        return false;
    }
}

bool CommandLineInterface::loadConfigFromFile(const std::string& filepath) {
    if (!pImpl->config) {
        std::cout << "No configuration available.\n";
        return false;
    }
    
    try {
        if (pImpl->config->loadFromFile(filepath)) {
            std::cout << "Configuration loaded from: " << filepath << "\n";
            return true;
        } else {
            std::cout << "Failed to load configuration from: " << filepath << "\n";
            return false;
        }
    } catch (const std::exception& e) {
        std::cout << "Error loading configuration: " << e.what() << "\n";
        return false;
    }
}

void CommandLineInterface::resetSimulation() {
    if (pImpl->brain) {
        pImpl->brain->reset();
    }
    
    resetNeuronHistory(); // Reset homeostatic neuron history
    pImpl->totalStepsExecuted = 0;
    
    stopPerformanceMonitoring();
}

void CommandLineInterface::printSimulationStatus() const {
    if (!pImpl->brain) {
        std::cout << "No brain available.\n";
        return;
    }
    
    std::cout << "=== Simulation Status ===\n";
    std::cout << "Brain state: " << (pImpl->simulationRunning ? "Running" : "Stopped") << "\n";
    std::cout << "Total spikes: " << pImpl->brain->getTotalSpikeCount() << "\n";
    std::cout << "Total steps: " << pImpl->totalStepsExecuted << "\n";
    std::cout << "Active neurons: " << pImpl->brain->getActiveNeuronCount() << "\n";
    std::cout << "Firing neurons: " << pImpl->brain->getFiringNeuronCount() << "\n";
    std::cout << "Average firing rate: " << pImpl->brain->getAverageFiringRate() << " Hz\n";
    
    if (pImpl->brain->getRegionCount() > 0) {
        std::cout << "\nRegion Information:\n";
        for (size_t i = 0; i < pImpl->brain->getRegionCount(); ++i) {
            auto region = pImpl->brain->getRegion(pImpl->brain->getRegionIds()[i]);
            if (region) {
                std::cout << "  Region " << i+1 << ": " 
                         << region->getTotalNeuronCount() << " neurons, "
                         << region->getSynapseCount() << " synapses\n";
            }
        }
    }
}

void CommandLineInterface::startPerformanceMonitoring() {
    pImpl->performanceMonitoringEnabled = true;
    pImpl->simulationStartTime = std::chrono::system_clock::now();
}

void CommandLineInterface::stopPerformanceMonitoring() {
    if (pImpl->performanceMonitoringEnabled) {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double>(endTime - pImpl->simulationStartTime);
        
        pImpl->totalSimulationTime += duration;
        pImpl->performanceMonitoringEnabled = false;
    }
}

void CommandLineInterface::printPerformanceStats() const {
    std::cout << "=== Performance Statistics ===\n";
    std::cout << "Total simulation time: " << pImpl->totalSimulationTime.count() << " seconds\n";
    
    if (pImpl->totalSimulationTime.count() > 0 && pImpl->totalStepsExecuted > 0) {
        double stepsPerSecond = pImpl->totalStepsExecuted / pImpl->totalSimulationTime.count();
        std::cout << "Performance: " << stepsPerSecond << " steps/second\n";
        
        if (pImpl->brain) {
            double spikesPerSecond = pImpl->brain->getTotalSpikeCount() / pImpl->totalSimulationTime.count();
            std::cout << "Spike rate: " << spikesPerSecond << " spikes/second\n";
        }
    }
}

void CommandLineInterface::startInteractiveMode() {
    pImpl->interactiveMode = true;
    std::cout << "Entering interactive mode. Type 'help' for commands.\n";
    
    std::string line;
    while (pImpl->interactiveMode) {
        std::cout << getPrompt();
        std::getline(std::cin, line);
        
        if (line.empty()) {
            continue;
        }
        
        if (!parseCommand(line)) {
            std::cout << "Command failed.\n";
        }
    }
}

std::string CommandLineInterface::getPrompt() const {
    std::stringstream ss;
    ss << "NLM";
    if (pImpl->brain) {
        ss << " steps:" << pImpl->totalStepsExecuted;
    }
    ss << " " << pImpl->prompt;
    return ss.str();
}

void CommandLineInterface::addToHistory(const std::string& command) {
    pImpl->commandHistory.push_back(command);
    if (pImpl->commandHistory.size() > pImpl->historyMaxSize) {
        pImpl->commandHistory.erase(pImpl->commandHistory.begin());
    }
}

std::vector<std::string> CommandLineInterface::getCommandHistory() const {
    return pImpl->commandHistory;
}

void CommandLineInterface::registerCommandHandler(const std::string& command, 
                                                 std::function<bool(const std::vector<std::string>&)> handler) {
    pImpl->commandHandlers[command] = handler;
}

void CommandLineInterface::resetNeuronHistory() {
    // This would call the homeostatic plasticity reset
    // For now, we'll just log that it's being reset
    std::cout << "Resetting neuron activity history for homeostatic plasticity.\n";
}

} // namespace nlm
