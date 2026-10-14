// Advanced NLM Command Line Interface Implementation
// Enhanced command system for NLM (熙然) neural network simulation

#include "core/AdvancedCommandSystem.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <cmath>

namespace nlm {

AdvancedCommandSystem::AdvancedCommandSystem(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config)
    : brain(brain), config(config), profileActive(false), totalStepTime(0.0), stepCount(0) {
    defineCommands();
}

void AdvancedCommandSystem::registerCommand(CommandType type, const std::string& description,
                                           std::function<void(const Command&, std::shared_ptr<Brain>, std::shared_ptr<Config>)> handler) {
    commands[type] = {type, {}, description, handler};
}

bool AdvancedCommandSystem::executeCommand(const std::string& input, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    Command cmd = parseCommand(input);
    if (cmd.type == CommandType::Help) {
        showHelp();
        return true;
    }
    
    if (cmd.type == CommandType::Exit) {
        return false;
    }
    
    if (validateCommand(cmd)) {
        executeCommand(cmd, brain, config);
        return true;
    }
    
    std::cout << "Error: Invalid command or arguments" << std::endl;
    return false;
}

Command AdvancedCommandSystem::parseCommand(const std::string& input) {
    Command cmd;
    std::string command = input;
    std::vector<std::string> args;
    
    // Parse command and arguments
    size_t pos = input.find(' ');
    if (pos != std::string::npos) {
        command = input.substr(0, pos);
        std::string remaining = input.substr(pos + 1);
        
        // Simple argument parsing (handles quoted arguments)
        size_t argStart = 0;
        bool inQuote = false;
        char quoteChar = 0;
        
        for (size_t i = 0; i <= remaining.size(); ++i) {
            if (i == remaining.size() || remaining[i] == ' ' && !inQuote) {
                if (argStart < i) {
                    std::string arg = remaining.substr(argStart, i - argStart);
                    // Remove quotes if present
                    if (arg.size() >= 2 && 
                        ((arg.front() == '"' && arg.back() == '"') ||
                         (arg.front() == '\'' && arg.back() == '\''))) {
                        arg = arg.substr(1, arg.size() - 2);
                    }
                    args.push_back(arg);
                }
                argStart = i + 1;
            } else if (!inQuote && (remaining[i] == '"' || remaining[i] == '\'')) {
                inQuote = true;
                quoteChar = remaining[i];
                argStart = i + 1;
            } else if (inQuote && remaining[i] == quoteChar) {
                inQuote = false;
                quoteChar = 0;
            }
        }
    }
    
    cmd.args = args;
    
    // Map command string to CommandType
    std::string lowerCommand = command;
    std::transform(lowerCommand.begin(), lowerCommand.end(), lowerCommand.begin(), ::tolower);
    
    if (lowerCommand == "help") cmd.type = CommandType::Help;
    else if (lowerCommand == "version") cmd.type = CommandType::Version;
    else if (lowerCommand == "status") cmd.type = CommandType::Status;
    else if (lowerCommand == "create-simulation" || lowerCommand == "create") cmd.type = CommandType::CreateSimulation;
    else if (lowerCommand == "benchmark") cmd.type = CommandType::Benchmark;
    else if (lowerCommand == "profile") cmd.type = CommandType::Profile;
    else if (lowerCommand == "visualize") cmd.type = CommandType::Visualize;
    else if (lowerCommand == "compare") cmd.type = CommandType::Compare;
    else if (lowerCommand == "debug-mode") cmd.type = CommandType::DebugMode;
    else if (lowerCommand == "step") cmd.type = CommandType::Step;
    else if (lowerCommand == "break") cmd.type = CommandType::Breakpoint;
    else if (lowerCommand == "inspect") cmd.type = CommandType::Inspect;
    else if (lowerCommand == "patch") cmd.type = CommandType::Patch;
    else if (lowerCommand == "config") cmd.type = CommandType::Config;
    else if (lowerCommand == "save") cmd.type = CommandType::Save;
    else if (lowerCommand == "load") cmd.type = CommandType::Load;
    else if (lowerCommand == "reset") cmd.type = CommandType::Reset;
    else if (lowerCommand == "exit" || lowerCommand == "quit") cmd.type = CommandType::Exit;
    else cmd.type = CommandType::Help; // Default to help for unknown commands
    
    // Find handler for this command type
    auto it = commands.find(cmd.type);
    if (it != commands.end()) {
        cmd.handler = it->second.handler;
    }
    
    return cmd;
}

void AdvancedCommandSystem::showHelp() const {
    std::cout << "=== NLM Advanced Command System ===\n" << std::endl;
    
    for (const auto& pair : commands) {
        const Command& cmd = pair.second;
        std::cout << "  " << getCommandName(cmd.type) << std::endl;
        std::cout << "    " << cmd.description << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "For more information about a specific command, use: help <command>" << std::endl;
}

void AdvancedCommandSystem::showCommandHelp(CommandType type) const {
    auto it = commands.find(type);
    if (it != commands.end()) {
        const Command& cmd = it->second;
        std::cout << "=== Command: " << getCommandName(type) << " ===\n" << std::endl;
        std::cout << cmd.description << std::endl;
        std::cout << "\nUsage: " << getCommandName(type) << " [arguments]" << std::endl;
        std::cout << "\nExamples:" << std::endl;
        printExamples(type);
    }
}

std::vector<std::string> AdvancedCommandSystem::completeCommand(const std::string& partial) const {
    std::vector<std::string> completions;
    std::string lowerPartial = partial;
    std::transform(lowerPartial.begin(), lowerPartial.end(), lowerPartial.begin(), ::tolower);
    
    for (const auto& pair : commands) {
        const Command& cmd = pair.second;
        std::string cmdName = getCommandName(cmd.type);
        std::string lowerCmdName = cmdName;
        std::transform(lowerCmdName.begin(), lowerCmdName.end(), lowerCmdName.begin(), ::tolower);
        
        if (lowerCmdName.find(lowerPartial) == 0) {
            completions.push_back(cmdName);
        }
    }
    
    return completions;
}

AdvancedCommandSystem::BenchmarkResult AdvancedCommandSystem::runBenchmark(const std::string& benchmarkType) {
    BenchmarkResult result;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Run benchmark based on type
    if (benchmarkType == "spiking") {
        // Focus on spiking activity
        brain->reset();
        brain->initialize();
        
        for (size_t i = 0; i < 1000; ++i) {
            brain->step(i, i * 0.001);
        }
        
        result.executionTime = getExecutionTime(startTime);
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = brain->getTotalSynapseCount();
        result.learningRate = 0.01f;
        result.accuracy = calculateAccuracy();
        
    } else if (benchmarkType == "learning") {
        // Focus on learning plasticity
        brain->reset();
        brain->initialize();
        
        // Enable plasticity
        auto* stdp = brain->getSTDP();
        if (stdp) stdp->configure(0.02f, 0.015f, 20.0f);
        
        // Run learning experiment
        for (size_t i = 0; i < 2000; ++i) {
            brain->step(i, i * 0.001);
        }
        
        result.executionTime = getExecutionTime(startTime);
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = brain->getTotalSynapseCount();
        result.learningRate = 0.02f;
        result.accuracy = calculateAccuracy();
        
    } else {
        // Default comprehensive benchmark
        brain->reset();
        brain->initialize();
        
        for (size_t i = 0; i < 1500; ++i) {
            brain->step(i, i * 0.001);
        }
        
        result.executionTime = getExecutionTime(startTime);
        result.neuronCount = brain->getTotalNeuronCount();
        result.synapseCount = brain->getTotalSynapseCount();
        result.learningRate = 0.015f;
        result.accuracy = calculateAccuracy();
    }
    
    return result;
}

AdvancedCommandSystem::ProfileData AdvancedCommandSystem::startProfile() {
    profileActive = true;
    profileStartTime = std::chrono::high_resolution_clock::now();
    currentProfile.totalTime = 0.0;
    currentProfile.stepCount = 0;
    currentProfile.spikeCount = 0;
    currentProfile.avgFiringRate = 0.0f;
    
    // Initialize component times
    currentProfile.componentTimes.clear();
    currentProfile.componentTimes["spiking"] = 0.0;
    currentProfile.componentTimes["plasticity"] = 0.0;
    currentProfile.componentTimes["memory"] = 0.0;
    currentProfile.componentTimes["cognition"] = 0.0;
    
    return currentProfile;
}

void AdvancedCommandSystem::stopProfile() {
    if (profileActive) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - profileStartTime);
        currentProfile.totalTime = duration.count() / 1000000.0; // Convert to seconds
        profileActive = false;
    }
}

void AdvancedCommandSystem::printProfileReport() const {
    if (currentProfile.totalTime <= 0.0) {
        std::cout << "No profile data available. Run profile first." << std::endl;
        return;
    }
    
    std::cout << "=== Performance Profile Report ===" << std::endl;
    std::cout << "Total execution time: " << std::fixed << std::setprecision(3) 
              << currentProfile.totalTime << " seconds" << std::endl;
    std::cout << "Steps executed: " << currentProfile.stepCount << std::endl;
    std::cout << "Total spikes: " << currentProfile.spikeCount << std::endl;
    std::cout << "Average firing rate: " << std::fixed << std::setprecision(2)
              << currentProfile.avgFiringRate << " Hz" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Component times:" << std::endl;
    for (const auto& pair : currentProfile.componentTimes) {
        std::cout << "  " << pair.first << ": " << std::fixed << std::setprecision(3)
                  << pair.second << " seconds (" 
                  << (pair.second / currentProfile.totalTime * 100.0) << "%)" << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Performance summary:" << std::endl;
    std::cout << "  Steps per second: " << (currentProfile.stepCount / currentProfile.totalTime) << std::endl;
    std::cout << "  Spikes per second: " << (currentProfile.spikeCount / currentProfile.totalTime) << std::endl;
}

void AdvancedCommandSystem::saveTemplate(const std::string& name, const std::string& description) {
    SimulationTemplate template;
    template.name = name;
    template.description = description;
    
    // Save current configuration as template
    auto keys = config->getKeys();
    for (const std::string& key : keys) {
        auto value = config->get<std::string>(key);
        if (value.has_value()) {
            template.parameters[key] = value.value();
        }
    }
    
    templates.push_back(template);
    std::cout << "Template saved: " << name << std::endl;
}

bool AdvancedCommandSystem::loadTemplate(const std::string& name) {
    for (const auto& template : templates) {
        if (template.name == name) {
            // Apply template parameters
            for (const auto& param : template.parameters) {
                config->set(param.first, param.second, ConfigSource::File);
            }
            std::cout << "Template loaded: " << name << std::endl;
            return true;
        }
    }
    
    std::cout << "Template not found: " << name << std::endl;
    return false;
}

void AdvancedCommandSystem::listTemplates() const {
    std::cout << "=== Available Templates ===" << std::endl;
    for (const auto& template : templates) {
        std::cout << "  " << template.name << ": " << template.description << std::endl;
    }
}

void AdvancedCommandSystem::saveState(const std::string& filepath) {
    if (brain->save(filepath)) {
        std::cout << "Brain state saved to: " << filepath << std::endl;
    } else {
        std::cout << "Failed to save brain state to: " << filepath << std::endl;
    }
}

bool AdvancedCommandSystem::loadState(const std::string& filepath) {
    if (brain->load(filepath)) {
        std::cout << "Brain state loaded from: " << filepath << std::endl;
        return true;
    } else {
        std::cout << "Failed to load brain state from: " << filepath << std::endl;
        return false;
    }
}

void AdvancedCommandSystem::printState() const {
    brain->logStatus();
}

void AdvancedCommandSystem::processConfigCommand(const std::string& key, const std::string& value) {
    config->set(key, value, ConfigSource::Runtime);
    std::cout << "Configuration updated: " << key << " = " << value << std::endl;
}

void AdvancedCommandSystem::printConfigSummary() const {
    std::cout << config->summary() << std::endl;
}

void AdvancedCommandSystem::resetBrain() {
    brain->reset();
    brain->initialize();
    std::cout << "Brain reset and reinitialized" << std::endl;
}

void AdvancedCommandSystem::printBrainStatus() const {
    std::cout << "=== Brain Status ===" << std::endl;
    std::cout << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "Regions: " << brain->getRegionCount() << std::endl;
    std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "Average firing rate: " << std::fixed << std::setprecision(2)
              << brain->getAverageFiringRate() << " Hz" << std::endl;
}

// Helper methods
std::string AdvancedCommandSystem::getCommandName(CommandType type) const {
    switch (type) {
        case CommandType::CreateSimulation: return "create-simulation";
        case CommandType::Benchmark: return "benchmark";
        case CommandType::Profile: return "profile";
        case CommandType::Visualize: return "visualize";
        case CommandType::Compare: return "compare";
        case CommandType::DebugMode: return "debug-mode";
        case CommandType::Step: return "step";
        case CommandType::Breakpoint: return "break";
        case CommandType::Inspect: return "inspect";
        case CommandType::Patch: return "patch";
        case CommandType::Help: return "help";
        case CommandType::Version: return "version";
        case CommandType::Status: return "status";
        case CommandType::Config: return "config";
        case CommandType::Save: return "save";
        case CommandType::Load: return "load";
        case CommandType::Reset: return "reset";
        case CommandType::Exit: return "exit";
        default: return "unknown";
    }
}

double AdvancedCommandSystem::getExecutionTime(std::chrono::high_resolution_clock::time_point startTime) {
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime);
    return duration.count() / 1000000.0; // Convert to seconds
}

float AdvancedCommandSystem::calculateAccuracy() {
    // Simple accuracy calculation based on stability
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t firingNeurons = brain->getFiringNeuronCount();
    
    if (totalSpikes == 0) return 0.0f;
    
    // Calculate accuracy as a measure of stable activity
    float stability = static_cast<float>(firingNeurons) / brain->getTotalNeuronCount();
    float learningProgress = std::min(1.0f, static_cast<float>(totalSpikes) / 10000.0f);
    
    return stability * 0.6f + learningProgress * 0.4f;
}

void AdvancedCommandSystem::defineCommands() {
    // Register all advanced commands with their handlers
    registerCommand(CommandType::CreateSimulation, 
                   "Create a new simulation with predefined parameters",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleCreateSimulationCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Benchmark,
                   "Run performance benchmarks",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleBenchmarkCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Profile,
                   "Start/stop performance profiling",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleProfileCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Visualize,
                   "Launch visualization dashboard",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleVisualizeCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::DebugMode,
                   "Enable/disable interactive debugging mode",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleDebugModeCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Step,
                   "Execute a single simulation step",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleStepCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Breakpoint,
                   "Set or clear debugging breakpoints",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleBreakpointCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Inspect,
                   "Inspect internal brain state and parameters",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleInspectCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Patch,
                   "Apply runtime patches to brain behavior",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handlePatchCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Help,
                   "Show help information about commands",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleHelpCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Version,
                   "Show version information",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleVersionCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Status,
                   "Show current brain and simulation status",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleStatusCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Config,
                   "Manage configuration settings",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleConfigCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Save,
                   "Save current brain state to file",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleSaveCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Load,
                   "Load brain state from file",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleLoadCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Reset,
                   "Reset brain and reinitialize with current configuration",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleResetCommand(cmd, brain, config);
                   });
    
    registerCommand(CommandType::Exit,
                   "Exit the simulation",
                   [this](const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
                       this->handleExitCommand(cmd, brain, config);
                   });
}

// Command handlers
void AdvancedCommandSystem::handleCreateSimulationCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Create Simulation ===" << std::endl;
    
    if (cmd.args.size() < 1) {
        std::cout << "Usage: create-simulation <template_name>" << std::endl;
        std::cout << "Available templates:" << std::endl;
        listTemplates();
        return;
    }
    
    std::string templateName = cmd.args[0];
    
    if (templateName == "default") {
        // Set default configuration
        config->set("neuron_count", 500, ConfigSource::File);
        config->set("region_count", 1, ConfigSource::File);
        config->set("connection_probability", 0.15f, ConfigSource::File);
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::File);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::File);
        std::cout << "Created default simulation" << std::endl;
        
    } else if (templateName == "complex") {
        // Set complex configuration for advanced learning
        config->set("neuron_count", 2000, ConfigSource::File);
        config->set("region_count", 3, ConfigSource::File);
        config->set("connection_probability", 0.1f, ConfigSource::File);
        config->set("stdp_ltp_weight", 0.01f, ConfigSource::File);
        config->set("stdp_ltd_weight", 0.012f, ConfigSource::File);
        config->set("synaptogenesis_rate", 0.00005f, ConfigSource::File);
        config->set("pruning_rate", 0.000005f, ConfigSource::File);
        std::cout << "Created complex simulation" << std::endl;
        
    } else {
        std::cout << "Unknown template: " << templateName << std::endl;
        listTemplates();
        return;
    }
    
    // Reinitialize brain with new configuration
    brain->reset();
    brain->initialize();
    std::cout << "Simulation created and brain initialized" << std::endl;
}

void AdvancedCommandSystem::handleBenchmarkCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Benchmark ===" << std::endl;
    
    std::string benchmarkType = "default";
    if (cmd.args.size() > 0) {
        benchmarkType = cmd.args[0];
    }
    
    std::cout << "Running benchmark: " << benchmarkType << std::endl;
    
    BenchmarkResult result = runBenchmark(benchmarkType);
    
    std::cout << "=== Benchmark Results ===" << std::endl;
    std::cout << "Type: " << benchmarkType << std::endl;
    std::cout << "Execution time: " << std::fixed << std::setprecision(3) 
              << result.executionTime << " seconds" << std::endl;
    std::cout << "Neurons: " << result.neuronCount << std::endl;
    std::cout << "Synapses: " << result.synapseCount << std::endl;
    std::cout << "Learning rate: " << std::fixed << std::setprecision(3) 
              << result.learningRate << std::endl;
    std::cout << "Accuracy: " << std::fixed << std::setprecision(3) 
              << result.accuracy << std::endl;
    std::cout << "Performance: " << (1000.0 / result.executionTime) << " steps/second" << std::endl;
}

void AdvancedCommandSystem::handleProfileCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Profile Control ===" << std::endl;
    
    if (cmd.args.size() < 1) {
        std::cout << "Usage: profile [start|stop|report]" << std::endl;
        return;
    }
    
    std::string action = cmd.args[0];
    
    if (action == "start") {
        currentProfile = startProfile();
        std::cout << "Performance profiling started" << std::endl;
        std::cout << "Run the simulation, then use 'profile stop' to stop profiling" << std::endl;
        
    } else if (action == "stop") {
        stopProfile();
        printProfileReport();
        
    } else if (action == "report") {
        printProfileReport();
        
    } else {
        std::cout << "Unknown profile action: " << action << std::endl;
        std::cout << "Usage: profile [start|stop|report]" << std::endl;
    }
}

void AdvancedCommandSystem::handleDebugModeCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Debug Mode ===" << std::endl;
    
    if (cmd.args.size() < 1) {
        std::cout << "Usage: debug-mode [enable|disable|status]" << std::endl;
        return;
    }
    
    std::string action = cmd.args[0];
    
    if (action == "enable") {
        debugger.debugMode = true;
        std::cout << "Debug mode enabled" << std::endl;
        std::cout << "Use 'break <type> <value>' to set breakpoints" << std::endl;
        std::cout << "Use 'step' to execute single steps" << std::endl;
        
    } else if (action == "disable") {
        debugger.debugMode = false;
        debugger.breakpoints.clear();
        std::cout << "Debug mode disabled" << std::endl;
        
    } else if (action == "status") {
        std::cout << "Debug mode: " << (debugger.debugMode ? "enabled" : "disabled") << std::endl;
        std::cout << "Breakpoints: " << debugger.breakpoints.size() << std::endl;
        std::cout << "Step count: " << debugger.stepCount << std::endl;
        
    } else {
        std::cout << "Unknown debug action: " << action << std::endl;
        std::cout << "Usage: debug-mode [enable|disable|status]" << std::endl;
    }
}

void AdvancedCommandSystem::handleStepCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Single Step ===" << std::endl;
    
    size_t steps = 1;
    if (cmd.args.size() > 0) {
        try {
            steps = std::stoul(cmd.args[0]);
        } catch (...) {
            std::cout << "Invalid number of steps" << std::endl;
            return;
        }
    }
    
    std::cout << "Executing " << steps << " step(s)..." << std::endl;
    
    for (size_t i = 0; i < steps; ++i) {
        brain->step(i, i * 0.001);
        
        if (debugger.checkBreakpoints(brain)) {
            std::cout << "Breakpoint triggered at step " << i << std::endl;
            break;
        }
    }
    
    std::cout << "Step completed" << std::endl;
    brain->logStatus();
}

void AdvancedCommandSystem::handleBreakpointCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Breakpoints ===" << std::endl;
    
    if (cmd.args.size() < 2) {
        std::cout << "Usage: break <type> <value> [clear]" << std::endl;
        std::cout << "Types: step, spike, learning, memory" << std::endl;
        std::cout << "Example: break step 1000" << std::endl;
        return;
    }
    
    std::string typeStr = cmd.args[0];
    std::string valueStr = cmd.args[1];
    bool clearMode = (cmd.args.size() > 2 && cmd.args[2] == "clear");
    
    Debugger::BreakpointType type;
    if (typeStr == "step") type = Debugger::BreakpointType::Step;
    else if (typeStr == "spike") type = Debugger::BreakpointType::Spike;
    else if (typeStr == "learning") type = Debugger::BreakpointType::Learning;
    else if (typeStr == "memory") type = Debugger::BreakpointType::Memory;
    else {
        std::cout << "Unknown breakpoint type: " << typeStr << std::endl;
        return;
    }
    
    size_t value = 0;
    try {
        value = std::stoul(valueStr);
    } catch (...) {
        std::cout << "Invalid breakpoint value: " << valueStr << std::endl;
        return;
    }
    
    if (clearMode) {
        debugger.clearBreakpoint(type);
        std::cout << "Cleared breakpoints of type: " << typeStr << std::endl;
    } else {
        debugger.setBreakpoint(type, value);
        std::cout << "Set breakpoint: " << typeStr << " at " << value << std::endl;
    }
}

void AdvancedCommandSystem::handleInspectCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Inspect Brain State ===" << std::endl;
    
    if (cmd.args.size() < 1) {
        std::cout << "Usage: inspect [neurons|synapses|memory|plasticity|cognition]" << std::endl;
        return;
    }
    
    std::string category = cmd.args[0];
    
    if (category == "neurons") {
        std::cout << "Neurons:" << std::endl;
        std::cout << "  Total: " << brain->getTotalNeuronCount() << std::endl;
        std::cout << "  Active: " << brain->getActiveNeuronCount() << std::endl;
        std::cout << "  Firing: " << brain->getFiringNeuronCount() << std::endl;
        std::cout << "  Avg firing rate: " << std::fixed << std::setprecision(2) 
                  << brain->getAverageFiringRate() << " Hz" << std::endl;
        
    } else if (category == "synapses") {
        std::cout << "Synapses:" << std::endl;
        std::cout << "  Total: " << brain->getTotalSynapseCount() << std::endl;
        std::cout << "  Pending events: " << brain->getPendingSpikeEventCount() << std::endl;
        std::cout << "  E/I ratio: " << std::fixed << std::setprecision(3) 
                  << brain->getExcitationInhibitionRatio() << std::endl;
        
    } else if (category == "memory") {
        std::cout << "Memory Systems:" << std::endl;
        auto* workingMem = brain->getWorkingMemory();
        if (workingMem) std::cout << "  Working memory capacity: " << workingMem->getCapacity() << std::endl;
        
    } else if (category == "plasticity") {
        std::cout << "Plasticity Systems:" << std::endl;
        auto* stdp = brain->getSTDP();
        if (stdp) std::cout << "  STDP weights: LTP=" << stdp->getLTPWeight() 
                          << ", LTD=" << stdp->getLTDWeight() << std::endl;
        
    } else if (category == "cognition") {
        std::cout << "Cognition Systems:" << std::endl;
        auto* planner = brain->getPlanner();
        if (planner) std::cout << "  Planning depth: " << planner->getPlanningDepth() << std::endl;
        
    } else {
        std::cout << "Unknown category: " << category << std::endl;
    }
}

void AdvancedCommandSystem::handlePatchCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Patch Brain ===" << std::endl;
    
    if (cmd.args.size() < 2) {
        std::cout << "Usage: patch <parameter> <value>" << std::endl;
        std::cout << "Example: patch stdp_ltp_weight 0.05" << std::endl;
        return;
    }
    
    std::string param = cmd.args[0];
    std::string value = cmd.args[1];
    
    try {
        if (param == "stdp_ltp_weight") {
            float floatValue = std::stof(value);
            auto* stdp = brain->getSTDP();
            if (stdp) stdp->configure(floatValue, stdp->getLTDWeight(), stdp->getTau());
            std::cout << "Patched STDP LTP weight to: " << floatValue << std::endl;
        } else if (param == "stdp_ltd_weight") {
            float floatValue = std::stof(value);
            auto* stdp = brain->getSTDP();
            if (stdp) stdp->configure(stdp->getLTPWeight(), floatValue, stdp->getTau());
            std::cout << "Patched STDP LTD weight to: " << floatValue << std::endl;
        } else if (param == "synaptogenesis_rate") {
            float floatValue = std::stof(value);
            auto* plasticity = brain->getStructuralPlasticity();
            if (plasticity) plasticity->setSynaptogenesisRate(floatValue);
            std::cout << "Patched synaptogenesis rate to: " << floatValue << std::endl;
        } else {
            std::cout << "Unknown parameter: " << param << std::endl;
            std::cout << "Available parameters: stdp_ltp_weight, stdp_ltd_weight, synaptogenesis_rate" << std::endl;
        }
    } catch (...) {
        std::cout << "Error applying patch: " << value << std::endl;
    }
}

void AdvancedCommandSystem::handleHelpCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    if (cmd.args.size() < 1) {
        showHelp();
    } else {
        std::string topic = cmd.args[0];
        std::string lowerTopic = topic;
        std::transform(lowerTopic.begin(), lowerTopic.end(), lowerTopic.begin(), ::tolower);
        
        if (lowerTopic == "help") {
            showHelp();
        } else {
            // Find command type
            CommandType type;
            if (lowerTopic == "create-simulation" || lowerTopic == "create") type = CommandType::CreateSimulation;
            else if (lowerTopic == "benchmark") type = CommandType::Benchmark;
            else if (lowerTopic == "profile") type = CommandType::Profile;
            else if (lowerTopic == "visualize") type = CommandType::Visualize;
            else if (lowerTopic == "compare") type = CommandType::Compare;
            else if (lowerTopic == "debug-mode") type = CommandType::DebugMode;
            else if (lowerTopic == "step") type = CommandType::Step;
            else if (lowerTopic == "break") type = CommandType::Breakpoint;
            else if (lowerTopic == "inspect") type = CommandType::Inspect;
            else if (lowerTopic == "patch") type = CommandType::Patch;
            else if (lowerTopic == "config") type = CommandType::Config;
            else if (lowerTopic == "save") type = CommandType::Save;
            else if (lowerTopic == "load") type = CommandType::Load;
            else if (lowerTopic == "reset") type = CommandType::Reset;
            else if (lowerTopic == "exit") type = CommandType::Exit;
            else if (lowerTopic == "version") type = CommandType::Version;
            else if (lowerTopic == "status") type = CommandType::Status;
            else {
                std::cout << "Unknown help topic: " << topic << std::endl;
                return;
            }
            
            showCommandHelp(type);
        }
    }
}

void AdvancedCommandSystem::handleVersionCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== NLM Advanced Command System ===" << std::endl;
    std::cout << "Version: 1.0.0" << std::endl;
    std::cout << "Phase: Phase 6 - Final Integration" << std::endl;
    std::cout << "Built with C++20" << std::endl;
    std::cout << "Features: Advanced commands, debugging, visualization, profiling" << std::endl;
}

void AdvancedCommandSystem::handleStatusCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    printState();
    printConfigSummary();
}

void AdvancedCommandSystem::handleConfigCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "=== Configuration Management ===" << std::endl;
    
    if (cmd.args.size() < 1) {
        std::cout << "Usage: config <get|set|list|save|load> [key] [value]" << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  config get neuron_count" << std::endl;
        std::cout << "  config set neuron_count 1000" << std::endl;
        std::cout << "  config list" << std::endl;
        return;
    }
    
    std::string action = cmd.args[0];
    
    if (action == "get") {
        if (cmd.args.size() < 2) {
            std::cout << "Usage: config get <key>" << std::endl;
            return;
        }
        std::string key = cmd.args[1];
        auto value = config->get<std::string>(key);
        if (value.has_value()) {
            std::cout << key << " = " << value.value() << std::endl;
        } else {
            std::cout << "Key not found: " << key << std::endl;
        }
        
    } else if (action == "set") {
        if (cmd.args.size() < 3) {
            std::cout << "Usage: config set <key> <value>" << std::endl;
            return;
        }
        std::string key = cmd.args[1];
        std::string value = cmd.args[2];
        processConfigCommand(key, value);
        
    } else if (action == "list") {
        auto keys = config->getKeys();
        std::cout << "Configuration keys:" << std::endl;
        for (const std::string& key : keys) {
            std::cout << "  " << key << std::endl;
        }
        
    } else if (action == "save") {
        if (cmd.args.size() < 2) {
            std::cout << "Usage: config save <filename>" << std::endl;
            return;
        }
        std::string filename = cmd.args[1];
        config->saveToFile(filename);
        std::cout << "Configuration saved to: " << filename << std::endl;
        
    } else if (action == "load") {
        if (cmd.args.size() < 2) {
            std::cout << "Usage: config load <filename>" << std::endl;
            return;
        }
        std::string filename = cmd.args[1];
        config->loadFromFile(filename);
        std::cout << "Configuration loaded from: " << filename << std::endl;
        
    } else {
        std::cout << "Unknown config action: " << action << std::endl;
        std::cout << "Usage: config <get|set|list|save|load> [key] [value]" << std::endl;
    }
}

void AdvancedCommandSystem::handleSaveCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::string filepath = "brain_state.sav";
    if (cmd.args.size() > 0) {
        filepath = cmd.args[0];
    }
    saveState(filepath);
}

void AdvancedCommandSystem::handleLoadCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::string filepath = "brain_state.sav";
    if (cmd.args.size() > 0) {
        filepath = cmd.args[0];
    }
    loadState(filepath);
}

void AdvancedCommandSystem::handleResetCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    resetBrain();
    std::cout << "Brain reset completed" << std::endl;
}

void AdvancedCommandSystem::handleExitCommand(const Command& cmd, std::shared_ptr<Brain> brain, std::shared_ptr<Config> config) {
    std::cout << "Exiting NLM simulation..." << std::endl;
    // Note: This will cause the calling program to exit
    // The actual exit should be handled by the calling application
    std::exit(0);
}

} // namespace nlm
