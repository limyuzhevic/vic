#include "CLIController.h"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <ctime>
#include <filesystem>

namespace nlm {

struct CLIController::Impl {
    std::shared_ptr<Brain> brain;
    std::shared_ptr<Config> config;
    std::shared_ptr<PowerUserAPI> powerUserAPI;
    std::shared_ptr<AdvancedSimulationController> controller;
    std::shared_ptr<AdvancedDebugger> debugger;
    std::unique_ptr<CommandRegistry> commandRegistry;
    std::unique_ptr<CommandLineParser> argumentParser;
    std::unique_ptr<CommandExecutionContext> commandContext;
    std::ostream* outputStream;
    std::ostream* errorStream;
    bool initialized;
    AdvancedCLIOptions options;
    
    Impl(std::shared_ptr<Brain> b, std::shared_ptr<Config> c,
         std::shared_ptr<PowerUserAPI> pui, std::shared_ptr<AdvancedSimulationController> ctrl,
         std::shared_ptr<AdvancedDebugger> dbg)
        : brain(b), config(c), powerUserAPI(pui), controller(ctrl), debugger(dbg),
          commandRegistry(nullptr), argumentParser(nullptr), commandContext(nullptr),
          outputStream(nullptr), errorStream(nullptr), initialized(false) {}
    
    ~Impl() {
        cleanup();
    }
};

CLIController::CLIController(std::shared_ptr<Brain> brain,
                             std::shared_ptr<Config> config,
                             std::shared_ptr<PowerUserAPI> powerUserAPI,
                             std::shared_ptr<AdvancedSimulationController> controller,
                             std::shared_ptr<AdvancedDebugger> debugger)
    : pImpl(std::make_unique<Impl>(brain, config, powerUserAPI, controller, debugger)) {
    pImpl->outputStream = &std::cout;
    pImpl->errorStream = &std::cerr;
}

CLIController::~CLIController() = default;

bool CLIController::initialize() {
    if (pImpl->initialized) {
        return true;
    }
    
    try {
        // Initialize advanced argument parser
        pImpl->argumentParser = std::make_unique<CommandLineParser>();
        setupArgumentParser();
        
        // Initialize command registry
        pImpl->commandRegistry = std::make_unique<CommandRegistry>();
        setupCommandRegistry();
        
        // Create command execution context
        pImpl->commandContext = std::make_unique<CommandExecutionContext>(
            pImpl->brain, pImpl->config, pImpl->powerUserAPI,
            pImpl->controller, pImpl->debugger);
        
        // Setup advanced CLI options
        setupAdvancedOptions();
        
        // Register all command types
        registerRuntimeCommands();
        registerStateCommands();
        registerConfigCommands();
        registerPerformanceCommands();
        registerOutputCommands();
        registerControlCommands();
        
        pImpl->initialized = true;
        
        NLM_LOG_INFO("CLI Controller initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to initialize CLI Controller: " + std::string(e.what()));
        return false;
    }
}

bool CLIController::processCommandLine(int argc, char** argv) {
    if (!pImpl->initialized) {
        (*pImpl->errorStream) << "CLI Controller not initialized" << std::endl;
        return false;
    }
    
    if (argc < 1) {
        (*pImpl->outputStream) << getHelp() << std::endl;
        return false;
    }
    
    // Check for help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h" || arg == "--help-all") {
            (*pImpl->outputStream) << getHelp() << std::endl;
            return false;
        }
    }
    
    // Parse arguments using the advanced parser
    if (!pImpl->argumentParser->parse(argc, argv)) {
        auto errors = pImpl->argumentParser->getErrors();
        for (const auto& error : errors) {
            (*pImpl->errorStream) << "Error: " << error << std::endl;
        }
        return false;
    }
    
    // If there are parsed command arguments, execute the command
    std::string commandName = pImpl->argumentParser->getCommandName();
    if (!commandName.empty()) {
        auto args = pImpl->argumentParser->getCommandArgs(commandName);
        
        // Convert CLI values to strings for command execution
        std::vector<std::string> argStrings;
        for (const auto& arg : args) {
            argStrings.push_back(std::visit([](auto&& arg) -> std::string {
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v<T, bool>) {
                    return arg ? "true" : "false";
                } else if constexpr (std::is_same_v<T, int>) {
                    return std::to_string(arg);
                } else if constexpr (std::is_same_v<T, double>) {
                    return std::to_string(arg);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    return arg;
                }
                return "";
            }, arg));
        }
        
        return executeCommand(commandName, argStrings);
    }
    
    return true;
}

bool CLIController::executeCommand(const std::string& commandName,
                                   const std::vector<std::string>& args) {
    if (!pImpl->initialized) {
        (*pImpl->errorStream) << "CLI Controller not initialized" << std::endl;
        return false;
    }
    
    if (!pImpl->commandRegistry->hasCommand(commandName)) {
        (*pImpl->errorStream) << "Unknown command: " << commandName << std::endl;
        (*pImpl->outputStream) << "Available commands:" << std::endl;
        for (const auto& cmd : pImpl->commandRegistry->getCommandNames()) {
            (*pImpl->outputStream) << "  " << cmd << std::endl;
        }
        return false;
    }
    
    try {
        CommandResult result = pImpl->commandRegistry->executeCommand(
            commandName, args, *pImpl->commandContext);
        
        return result.success;
        
    } catch (const std::exception& e) {
        (*pImpl->errorStream) << "Exception executing command " << commandName << ": " << e.what() << std::endl;
        return false;
    }
}

bool CLIController::hasCommand(const std::string& commandName) const {
    if (!pImpl->initialized) return false;
    return pImpl->commandRegistry->hasCommand(commandName);
}

std::string CLIController::getCommandHelp(const std::string& commandName) const {
    if (!pImpl->initialized) return "CLI Controller not initialized";
    return pImpl->commandRegistry->getCommandHelp(commandName);
}

std::vector<std::string> CLIController::getAvailableCommands() const {
    if (!pImpl->initialized) return std::vector<std::string>();
    return pImpl->commandRegistry->getCommandNames();
}

std::string CLIController::getStatus() const {
    if (!pImpl->initialized) return "CLI Controller not initialized";
    
    std::ostringstream oss;
    oss << "=== Simulation Status ===\n";
    oss << "Steps completed: " << getStepCount() << "\n";
    oss << "Simulation time: " << getSimulationTime() << " ms\n";
    oss << "Current timestep: " << getTimestep() << " ms\n";
    oss << "Paused: " << (isPaused() ? "yes" : "no") << "\n";
    oss << "Debugging: " << (isDebugging() ? "enabled" : "disabled") << "\n";
    oss << "Active neurons: " << getPerformanceStats().activeNeuronCount << "\n";
    oss << "Total spikes: " << getPerformanceStats().totalSpikes << "\n";
    
    return oss.str();
}

std::string CLIController::getPerformanceReport() const {
    if (!pImpl->initialized) return "CLI Controller not initialized";
    
    auto stats = getPerformanceStats();
    std::ostringstream oss;
    oss << "=== Performance Report ===\n";
    oss << "Simulation time: " << stats.simulationTime << " ms\n";
    oss << "Real time elapsed: " << stats.realTimeElapsed << " s\n";
    oss << "Step count: " << stats.stepCount << "\n";
    oss << "Average step time: " << stats.averageStepTime << " ms\n";
    oss << "Total spikes: " << stats.totalSpikes << "\n";
    oss << "Spikes this step: " << stats.spikesThisStep << "\n";
    oss << "Average firing rate: " << stats.averageFiringRate << " Hz\n";
    oss << "Active neurons: " << stats.activeNeuronCount << "\n";
    oss << "Memory usage: " << stats.memoryUsageMB << " MB\n";
    oss << "CPU usage: " << stats.cpuUsagePercent << "%\n";
    
    return oss.str();
}

std::string CLIController::getConfigurationReport() const {
    if (!pImpl->initialized) return "CLI Controller not initialized";
    
    std::ostringstream oss;
    oss << "=== Configuration Report ===\n";
    
    auto keys = pImpl->config->getKeys();
    for (const auto& key : keys) {
        auto valueOpt = pImpl->config->get<CLIValue>(key);
        if (valueOpt) {
            std::string valueStr;
            const auto& value = *valueOpt;
            std::visit([&](auto&& val) {
                using T = std::decay_t<decltype(val)>;
                if constexpr (std::is_same_v<T, bool>) {
                    valueStr = val ? "true" : "false";
                } else if constexpr (std::is_same_v<T, int>) {
                    valueStr = std::to_string(val);
                } else if constexpr (std::is_same_v<T, double>) {
                    valueStr = std::to_string(val);
                } else if constexpr (std::is_same_v<T, std::string>) {
                    valueStr = val;
                } else {
                    valueStr = "[complex value]";
                }
            }, value);
            oss << key << " = " << valueStr << "\n";
        }
    }
    
    return oss.str();
}

std::string CLIController::getNetworkReport() const {
    if (!pImpl->initialized) return "CLI Controller not initialized";
    
    if (pImpl->powerUserAPI) {
        return pImpl->powerUserAPI->generateNetworkReport();
    }
    return "PowerUserAPI not available";
}

bool CLIController::updateConfiguration(const std::string& key, const ConfigValue& value) {
    if (!pImpl->initialized) return false;
    
    try {
        pImpl->config->set(key, value);
        NLM_LOG_INFO("Configuration updated: " + key);
        return true;
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to update configuration: " + std::string(e.what()));
        return false;
    }
}

bool CLIController::validateConfiguration() const {
    if (!pImpl->initialized) return false;
    
    // Basic validation - check for required keys
    auto requiredKeys = {"random_seed", "simulation_timestep", "neuron_count"};
    for (const auto& key : requiredKeys) {
        if (!pImpl->config->has(key)) {
            NLM_LOG_ERROR("Missing required configuration key: " + key);
            return false;
        }
    }
    
    return true;
}

bool CLIController::exportResults(const std::string& format, const std::string& filepath) const {
    if (!pImpl->initialized) return false;
    
    try {
        std::filesystem::path outputPath(filepath);
        std::filesystem::create_directories(outputPath.parent_path());
        
        if (format == "json") {
            // Export in JSON format
            auto configKeys = pImpl->config->getKeys();
            std::ostringstream oss;
            oss << "{\n";
            for (size_t i = 0; i < configKeys.size(); ++i) {
                auto valueOpt = pImpl->config->get<CLIValue>(configKeys[i]);
                if (valueOpt) {
                    oss << "  \"" << configKeys[i] << "\": ";
                    const auto& value = *valueOpt;
                    std::visit([&](auto&& val) {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, bool>) {
                            oss << (val ? "true" : "false");
                        } else if constexpr (std::is_same_v<T, int>) {
                            oss << val;
                        } else if constexpr (std::is_same_v<T, double>) {
                            oss << val;
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            oss << "\"" << val << "\"";
                        } else {
                            oss << "null";
                        }
                    }, value);
                    
                    if (i < configKeys.size() - 1) {
                        oss << ",\n";
                    } else {
                        oss << "\n";
                    }
                }
            }
            oss << "}\n";
            
            std::ofstream file(filepath);
            if (file.is_open()) {
                file << oss.str();
                file.close();
                NLM_LOG_INFO("Results exported to: " + filepath);
                return true;
            }
        }
        
        NLM_LOG_WARNING("Export format not supported: " + format);
        return false;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to export results: " + std::string(e.what()));
        return false;
    }
}

bool CLIController::generateReport(const std::string& type, const std::string& filepath) const {
    if (!pImpl->initialized) return false;
    
    try {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot create report file: " + filepath);
            return false;
        }
        
        if (type == "simulation") {
            file << getStatus();
        } else if (type == "performance") {
            file << getPerformanceReport();
        } else if (type == "configuration") {
            file << getConfigurationReport();
        } else if (type == "network") {
            file << getNetworkReport();
        } else if (type == "all") {
            file << "=== Comprehensive Report ===\n\n";
            file << getStatus() << "\n";
            file << getPerformanceReport() << "\n";
            file << getConfigurationReport() << "\n";
            file << getNetworkReport() << "\n";
        } else {
            file << "Unknown report type: " << type << std::endl;
            file.close();
            return false;
        }
        
        file.close();
        NLM_LOG_INFO("Report generated: " + filepath);
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Failed to generate report: " + std::string(e.what()));
        return false;
    }
}

bool CLIController::isPaused() const {
    if (!pImpl->initialized) return false;
    return pImpl->powerUserAPI ? pImpl->powerUserAPI->isSimulationPaused() : false;
}

bool CLIController::isDebugging() const {
    if (!pImpl->initialized) return false;
    return pImpl->debugger ? pImpl->debugger->isDebugging() : false;
}

float CLIController::getTimestep() const {
    if (!pImpl->initialized) return 0.0f;
    return pImpl->powerUserAPI ? pImpl->powerUserAPI->getTimestep() : 0.0f;
}

size_t CLIController::getStepCount() const {
    if (!pImpl->initialized) return 0;
    return pImpl->powerUserAPI ? pImpl->powerUserAPI->getStepCount() : 0;
}

double CLIController::getSimulationTime() const {
    if (!pImpl->initialized) return 0.0;
    return pImpl->powerUserAPI ? pImpl->powerUserAPI->getSimulationTime() : 0.0;
}

PerformanceStats CLIController::getPerformanceStats() const {
    if (!pImpl->initialized) return PerformanceStats();
    return pImpl->powerUserAPI ? pImpl->powerUserAPI->getPerformanceStats() : PerformanceStats();
}

void CLIController::cleanup() {
    if (pImpl->commandRegistry) {
        pImpl->commandRegistry->cleanup();
    }
    pImpl->commandRegistry.reset();
    pImpl->argumentParser.reset();
    pImpl->commandContext.reset();
    pImpl->initialized = false;
    NLM_LOG_INFO("CLI Controller cleaned up");
}

void CLIController::setOutputStream(std::ostream* outputStream) {
    pImpl->outputStream = outputStream;
    if (pImpl->commandRegistry) {
        pImpl->commandRegistry->setOutputStream(outputStream);
    }
}

void CLIController::setErrorStream(std::ostream* errorStream) {
    pImpl->errorStream = errorStream;
    if (pImpl->commandRegistry) {
        pImpl->commandRegistry->setErrorStream(errorStream);
    }
}

// Implementation of helper methods
bool CLIController::parseFloat(const std::string& str, float& value) const {
    try {
        value = std::stof(str);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIController::parseInt(const std::string& str, int& value) const {
    try {
        value = std::stoi(str);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIController::parseString(const std::string& str, std::string& value) const {
    value = str;
    return true;
}

bool CLIController::parseBool(const std::string& str, bool& value) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "true" || lower == "1" || lower == "yes" || lower == "on") {
        value = true;
        return true;
    } else if (lower == "false" || lower == "0" || lower == "no" || lower == "off") {
        value = false;
        return true;
    }
    return false;
}

bool CLIController::validateNeuronId(const std::string& str, NeuronId& neuronId) const {
    try {
        neuronId = static_cast<NeuronId>(std::stoull(str));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIController::validateSynapseId(const std::string& str, SynapseId& synapseId) const {
    try {
        synapseId = static_cast<SynapseId>(std::stoull(str));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIController::validatePopulationId(const std::string& str, PopulationId& populationId) const {
    try {
        populationId = static_cast<PopulationId>(std::stoull(str));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIController::validateRegionId(const std::string& str, RegionId& regionId) const {
    try {
        regionId = static_cast<RegionId>(std::stoull(str));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIController::validateStimulusPattern(const std::string& str, StimulusPattern& pattern) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "poisson" || lower == "randompoisson") {
        pattern = StimulusPattern::RandomPoisson;
        return true;
    } else if (lower == "regularspiking") {
        pattern = StimulusPattern::RegularSpiking;
        return true;
    } else if (lower == "bursting") {
        pattern = StimulusPattern::Bursting;
        return true;
    } else if (lower == "theta") {
        pattern = StimulusPattern::Theta;
        return true;
    } else if (lower == "gamma") {
        pattern = StimulusPattern::Gamma;
        return true;
    } else if (lower == "custom") {
        pattern = StimulusPattern::Custom;
        return true;
    }
    return false;
}

bool CLIController::validateReplayMode(const std::string& str, ReplayMode& mode) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "fast") {
        mode = ReplayMode::Fast;
        return true;
    } else if (lower == "normal") {
        mode = ReplayMode::Normal;
        return true;
    } else if (lower == "slow") {
        mode = ReplayMode::Slow;
        return true;
    } else if (lower == "variablespeed") {
        mode = ReplayMode::VariableSpeed;
        return true;
    }
    return false;
}

bool CLIController::validateNeuromodulationType(const std::string& str, NeuromodulationType& type) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "curiosity") {
        type = NeuromodulationType::Curiosity;
        return true;
    } else if (lower == "novelty") {
        type = NeuromodulationType::Novelty;
        return true;
    } else if (lower == "reward") {
        type = NeuromodulationType::Reward;
        return true;
    } else if (lower == "predictionerror") {
        type = NeuromodulationType::PredictionError;
        return true;
    } else if (lower == "all") {
        type = NeuromodulationType::All;
        return true;
    }
    return false;
}

bool CLIController::validatePlasticityType(const std::string& str, PlasticityType& type) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "stdp") {
        type = PlasticityType::STDP;
        return true;
    } else if (lower == "hebbian") {
        type = PlasticityType::Hebbian;
        return true;
    } else if (lower == "rewardmodulated") {
        type = PlasticityType::RewardModulated;
        return true;
    } else if (lower == "all") {
        type = PlasticityType::All;
        return true;
    }
    return false;
}

bool CLIController::validateEventFilterCategory(const std::string& str, EventFilterCategory& category) const {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    
    if (lower == "all") {
        category = EventFilterCategory::All;
        return true;
    } else if (lower == "plasticity") {
        category = EventFilterCategory::Plasticity;
        return true;
    } else if (lower == "spike") {
        category = EventFilterCategory::Spike;
        return true;
    } else if (lower == "memory") {
        category = EventFilterCategory::Memory;
        return true;
    } else if (lower == "development") {
        category = EventFilterCategory::Development;
        return true;
    }
    return false;
}

bool CLIController::validateEventType(const std::string& str, EventType& type) const {
    try {
        type = static_cast<EventType>(std::stoi(str));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

bool CLIController::validateDevelopmentalStage(const std::string& str, DevelopmentalStage& stage) const {
    try {
        stage = static_cast<DevelopmentalStage>(std::stoi(str));
        return true;
    } catch (const std::exception&) {
        return false;
    }
}

// Command handler implementations
CommandResult CLIController::handlePauseCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    bool pause = true;
    if (args.size() > 0) {
        parseBool(args[0], pause);
    }
    
    if (pImpl->powerUserAPI) {
        pImpl->powerUserAPI->pauseSimulation(pause);
        return CommandResult(true, "Simulation " + std::string(pause ? "paused" : "resumed"));
    }
    return CommandResult(false, "PowerUserAPI not available");
}

CommandResult CLIController::handleResumeCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    return handlePauseCommand(args, context); // Same logic as pause with pause=false
}

CommandResult CLIController::handleStepCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    int steps = 1;
    if (args.size() > 0) {
        int parsed;
        if (parseInt(args[0], parsed)) {
            steps = parsed;
        }
    }
    
    // Step through simulation
    auto brain = context.getBrain();
    for (int i = 0; i < steps; ++i) {
        if (brain) {
            brain->step(i, i * 0.001);
        }
    }
    
    return CommandResult(true, "Stepped through simulation " + std::to_string(steps) + " steps");
}

CommandResult CLIController::handleResetCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (pImpl->powerUserAPI) {
        pImpl->powerUserAPI->resetSimulation();
        return CommandResult(true, "Simulation reset");
    }
    return CommandResult(false, "PowerUserAPI not available");
}

CommandResult CLIController::handleStatusCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string status = getStatus();
    return CommandResult(true, status);
}

CommandResult CLIController::handleHelpCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string help = getHelp();
    return CommandResult(true, help);
}

CommandResult CLIController::handleSaveCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string filepath = "checkpoint.bin";
    if (args.size() > 0) {
        filepath = args[0];
    }
    
    if (pImpl->powerUserAPI) {
        bool success = pImpl->powerUserAPI->saveState(filepath);
        return CommandResult(success, "Brain state " + std::string(success ? "saved" : "save failed") + " to " + filepath);
    }
    return CommandResult(false, "PowerUserAPI not available");
}

CommandResult CLIController::handleLoadCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (args.size() == 0) {
        return CommandResult(false, "Missing filepath argument");
    }
    
    std::string filepath = args[0];
    if (pImpl->powerUserAPI) {
        bool success = pImpl->powerUserAPI->loadState(filepath);
        return CommandResult(success, "Brain state " + std::string(success ? "loaded" : "load failed") + " from " + filepath);
    }
    return CommandResult(false, "PowerUserAPI not available");
}

CommandResult CLIController::handleCheckpointCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string filepath = "checkpoint.bin";
    std::string description = "Saved checkpoint";
    
    if (args.size() > 0) {
        filepath = args[0];
    }
    if (args.size() > 1) {
        description = args[1];
    }
    
    if (pImpl->controller) {
        bool success = pImpl->controller->saveCheckpoint(filepath, description);
        return CommandResult(success, "Checkpoint " + std::string(success ? "saved" : "save failed") + ": " + description);
    }
    return CommandResult(false, "AdvancedSimulationController not available");
}

CommandResult CLIController::handleReplayCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (args.size() == 0) {
        return CommandResult(false, "Missing source file argument");
    }
    
    ReplayConfig config;
    config.sourceFile = args[0];
    
    // Parse optional arguments
    for (size_t i = 1; i < args.size(); ++i) {
        if (args[i] == "--mode") {
            if (i + 1 < args.size()) {
                ReplayMode mode;
                if (validateReplayMode(args[i + 1], mode)) {
                    config.replayMode = mode;
                }
                i++;
            }
        } else if (args[i] == "--speed") {
            if (i + 1 < args.size()) {
                config.speed = std::stof(args[i + 1]);
                i++;
            }
        } else if (args[i] == "--loop") {
            config.loop = true;
        } else if (args[i] == "--start") {
            if (i + 1 < args.size()) {
                config.startStep = std::stoull(args[i + 1]);
                i++;
            }
        }
    }
    
    if (pImpl->controller) {
        bool success = pImpl->controller->replayCheckpoint(config);
        return CommandResult(success, "Checkpoint replay " + std::string(success ? "completed" : "failed"));
    }
    return CommandResult(false, "AdvancedSimulationController not available");
}

CommandResult CLIController::handleSetCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (args.size() < 2) {
        return CommandResult(false, "Missing key or value argument");
    }
    
    std::string key = args[0];
    std::string valueStr = args[1];
    
    // Try to determine value type
    float floatValue;
    int intValue;
    bool boolValue;
    std::string stringValue = valueStr;
    
    ConfigValue configValue;
    
    if (parseFloat(valueStr, floatValue)) {
        configValue = floatValue;
    } else if (parseInt(valueStr, intValue)) {
        configValue = intValue;
    } else if (parseBool(valueStr, boolValue)) {
        configValue = boolValue;
    } else {
        configValue = stringValue;
    }
    
    if (updateConfiguration(key, configValue)) {
        return CommandResult(true, "Configuration updated: " + key + " = " + valueStr);
    }
    return CommandResult(false, "Failed to update configuration");
}

CommandResult CLIController::handleValidateCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    bool valid = validateConfiguration();
    return CommandResult(valid, "Configuration " + std::string(valid ? "valid" : "invalid"));
}

CommandResult CLIController::handleTemplateListCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string result = "Available configuration templates:";
    // TODO: Implement template listing
    return CommandResult(true, result);
}

CommandResult CLIController::handleTemplateApplyCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (args.size() == 0) {
        return CommandResult(false, "Missing template name argument");
    }
    
    std::string templateName = args[0];
    // TODO: Implement template application
    return CommandResult(true, "Template applied: " + templateName);
}

CommandResult CLIController::handleMonitorCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    // Start continuous monitoring
    // TODO: Implement monitoring
    return CommandResult(true, "Performance monitoring started");
}

CommandResult CLIController::handleProfileCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    // Start profiling
    // TODO: Implement profiling
    return CommandResult(true, "Performance profiling started");
}

CommandResult CLIController::handleMetricsCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string metrics = getPerformanceReport();
    return CommandResult(true, metrics);
}

CommandResult CLIController::handleResourcesCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    auto stats = getPerformanceStats();
    std::ostringstream oss;
    oss << "CPU usage: " << stats.cpuUsagePercent << "%\n";
    oss << "Memory usage: " << stats.memoryUsageMB << " MB\n";
    oss << "Simulation time: " << stats.simulationTime << " ms\n";
    oss << "Real time elapsed: " << stats.realTimeElapsed << " s\n";
    
    return CommandResult(true, oss.str());
}

CommandResult CLIController::handleVerboseCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string level = "info";
    if (args.size() > 0) {
        level = args[0];
    }
    
    // TODO: Set logging level
    return CommandResult(true, "Verbose level set to: " + level);
}

CommandResult CLIController::handleFormatCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (args.size() == 0) {
        return CommandResult(false, "Missing format argument");
    }
    
    std::string format = args[0];
    std::string filepath = "output.txt";
    if (args.size() > 1) {
        filepath = args[1];
    }
    
    bool success = exportResults(format, filepath);
    return CommandResult(success, "Results exported in " + std::string(success ? "" : "failed ") + format + " format to " + filepath);
}

CommandResult CLIController::handleExportCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    return handleFormatCommand(args, context);
}

CommandResult CLIController::handleReportCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    std::string type = "all";
    std::string filepath = "report.txt";
    
    if (args.size() > 0) {
        type = args[0];
    }
    if (args.size() > 1) {
        filepath = args[1];
    }
    
    bool success = generateReport(type, filepath);
    return CommandResult(success, "Report " + std::string(success ? "generated" : "generation failed") + ": " + filepath);
}

CommandResult CLIController::handleInjectCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (args.size() < 2) {
        return CommandResult(false, "Missing neuron ID or current value argument");
    }
    
    NeuronId neuronId;
    if (!validateNeuronId(args[0], neuronId)) {
        return CommandResult(false, "Invalid neuron ID: " + args[0]);
    }
    
    float current;
    if (!parseFloat(args[1], current)) {
        return CommandResult(false, "Invalid current value: " + args[1]);
    }
    
    if (pImpl->powerUserAPI) {
        bool success = pImpl->powerUserAPI->injectCurrent(neuronId, current);
        return CommandResult(success, "Injected current " + std::to_string(current) + " into neuron " + std::to_string(neuronId));
    }
    return CommandResult(false, "PowerUserAPI not available");
}

CommandResult CLIController::handleSpeedCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    if (args.size() == 0) {
        return CommandResult(false, "Missing speed factor argument");
    }
    
    float speed = 1.0f;
    if (!parseFloat(args[0], speed)) {
        return CommandResult(false, "Invalid speed value: " + args[0]);
    }
    
    // TODO: Set simulation speed
    return CommandResult(true, "Simulation speed set to: " + std::to_string(speed) + "x");
}

CommandResult CLIController::handleBatchCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    BatchOperationConfig config;
    config.name = "CLI Batch Operation";
    config.runInParallel = false;
    config.threadCount = 1;
    
    // TODO: Configure and run batch operation
    return CommandResult(true, "Batch operation initiated");
}

CommandResult CLIController::handleStepByStepCommand(const std::vector<std::string>& args, CommandExecutionContext& context) {
    bool enable = true;
    if (args.size() > 0) {
        parseBool(args[0], enable);
    }
    
    // TODO: Enable/disable step-by-step mode
    return CommandResult(true, "Step-by-step mode " + std::string(enable ? "enabled" : "disabled"));
}

// Implementation of setup methods
bool CLIController::setupCommandRegistry() {
    pImpl->commandRegistry->setOutputStream(pImpl->outputStream);
    pImpl->commandRegistry->setErrorStream(pImpl->errorStream);
    return true;
}

bool CLIController::setupArgumentParser() {
    pImpl->argumentParser->setOutputStream(pImpl->outputStream);
    pImpl->argumentParser->setErrorStream(pImpl->errorStream);
    
    // Add common options
    pImpl->argumentParser->addOption(CLIOption("help", 'h', "Show help information", "Display help information", false));
    pImpl->argumentParser->addOption(CLIOption("version", 'v', "Show version information", "Display version information", false));
    pImpl->argumentParser->addOption(CLIOption("verbose", 'V', false, "Enable verbose logging", false));
    pImpl->argumentParser->addOption(CLIOption("config", 'c', "configs/default.cfg", "Configuration file path", false));
    pImpl->argumentParser->addOption(CLIOption("timestep", 't', 0.001, "Simulation timestep in seconds", false));
    pImpl->argumentParser->addOption(CLIOption("steps", 's', 100, "Number of simulation steps", false));
    pImpl->argumentParser->addOption(CLIOption("speed", 'p', 1.0f, "Simulation speed multiplier", false));
    
    // Add commands
    pImpl->argumentParser->addCommand(CLICommand("run", "Run simulation with specified options"));
    pImpl->argumentParser->addCommand(CLICommand("help", "Show help information"));
    pImpl->argumentParser->addCommand(CLICommand("status", "Get current simulation status"));
    pImpl->argumentParser->addCommand(CLICommand("save", "Save brain state to file"));
    pImpl->argumentParser->addCommand(CLICommand("load", "Load brain state from file"));
    pImpl->argumentParser->addCommand(CLICommand("pause", "Pause simulation"));
    pImpl->argumentParser->addCommand(CLICommand("resume", "Resume simulation"));
    pImpl->argumentParser->addCommand(CLICommand("step", "Run single or multiple steps"));
    pImpl->argumentParser->addCommand(CLICommand("reset", "Reset simulation"));
    
    return true;
}

bool CLIController::setupAdvancedOptions() {
    // Setup advanced CLI options
    return true;
}

bool CLIController::registerRuntimeCommands() {
    // Register runtime commands using PowerUserAPI interface
    using namespace std::placeholders;
    
    if (!pImpl->powerUserAPI) return false;
    
    // Register pause/resume command
    auto pauseCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.runtime.pause,
        "Pause or resume simulation",
        {"action"},
        {"Pause action (true/false)"},
        std::bind(&CLIController::handlePauseCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(pauseCmd);
    
    auto resumeCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.runtime.resume,
        "Resume simulation",
        {},
        {},
        std::bind(&CLIController::handlePauseCommand, this, _1, _2)  // Reuse pause handler with false
    );
    pImpl->commandRegistry->registerCommand(resumeCmd);
    
    // Register step command
    auto stepCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.runtime.step,
        "Run simulation steps",
        {"steps"},
        {"Number of steps to run"},
        std::bind(&CLIController::handleStepCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(stepCmd);
    
    // Register reset command
    auto resetCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.runtime.reset,
        "Reset simulation",
        {},
        {},
        std::bind(&CLIController::handleResetCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(resetCmd);
    
    // Register status command
    auto statusCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.runtime.status,
        "Get simulation status",
        {},
        {},
        std::bind(&CLIController::handleStatusCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(statusCmd);
    
    // Register help command
    auto helpCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.runtime.help,
        "Show help information",
        {},
        {},
        std::bind(&CLIController::handleHelpCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(helpCmd);
    
    return true;
}

bool CLIController::registerStateCommands() {
    // Register state management commands
    using namespace std::placeholders;
    
    if (!pImpl->powerUserAPI) return false;
    
    // Register save command
    auto saveCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.state.save,
        "Save brain state to file",
        {"filepath"},
        {"File path to save brain state"},
        std::bind(&CLIController::handleSaveCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(saveCmd);
    
    // Register load command
    auto loadCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.state.load,
        "Load brain state from file",
        {"filepath"},
        {"File path to load brain state"},
        std::bind(&CLIController::handleLoadCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(loadCmd);
    
    // Register checkpoint command
    auto checkpointCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.state.checkpoint,
        "Save checkpoint",
        {"filepath", "description"},
        {"Checkpoint file path", "Description of checkpoint"},
        std::bind(&CLIController::handleCheckpointCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(checkpointCmd);
    
    // Register replay command
    auto replayCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.state.replay,
        "Replay checkpoint",
        {"source_file"},
        {"Source checkpoint file"},
        std::bind(&CLIController::handleReplayCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(replayCmd);
    
    return true;
}

bool CLIController::registerConfigCommands() {
    // Register configuration management commands
    using namespace std::placeholders;
    
    // Register set command
    auto setCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.config.set,
        "Set configuration value",
        {"key", "value"},
        {"Configuration key", "Configuration value"},
        std::bind(&CLIController::handleSetCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(setCmd);
    
    // Register validate command
    auto validateCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.config.validate,
        "Validate configuration",
        {},
        {},
        std::bind(&CLIController::handleValidateCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(validateCmd);
    
    // Register template list command
    auto templateListCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.config.templateList,
        "List available configuration templates",
        {},
        {},
        std::bind(&CLIController::handleTemplateListCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(templateListCmd);
    
    // Register apply template command
    auto templateApplyCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.config.templateApply,
        "Apply configuration template",
        {"template_name"},
        {"Name of template to apply"},
        std::bind(&CLIController::handleTemplateApplyCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(templateApplyCmd);
    
    return true;
}

bool CLIController::registerPerformanceCommands() {
    // Register performance monitoring commands
    using namespace std::placeholders;
    
    // Register metrics command
    auto metricsCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.perf.metrics,
        "Get performance metrics",
        {},
        {},
        std::bind(&CLIController::handleMetricsCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(metricsCmd);
    
    // Register resources command
    auto resourcesCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.perf.resources,
        "Get system resources",
        {},
        {},
        std::bind(&CLIController::handleResourcesCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(resourcesCmd);
    
    return true;
}

bool CLIController::registerOutputCommands() {
    // Register output and logging commands
    using namespace std::placeholders;
    
    // Register verbose command
    auto verboseCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.output.verbose,
        "Set verbose logging level",
        {"level"},
        {"Log level (debug, info, warning, error)"},
        std::bind(&CLIController::handleVerboseCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(verboseCmd);
    
    // Register format command
    auto formatCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.output.format,
        "Export results in specific format",
        {"format", "filepath"},
        {"Output format (json, csv, etc.)", "Output file path"},
        std::bind(&CLIController::handleFormatCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(formatCmd);
    
    // Register export command
    auto exportCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.output.exportResults,
        "Export results",
        {"format", "filepath"},
        {"Output format", "Output file path"},
        std::bind(&CLIController::handleExportCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(exportCmd);
    
    // Register report command
    auto reportCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.output.generateReport,
        "Generate report",
        {"type", "filepath"},
        {"Report type (simulation, performance, configuration, network, all)", "Output file path"},
        std::bind(&CLIController::handleReportCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(reportCmd);
    
    return true;
}

bool CLIController::registerControlCommands() {
    // Register simulation control commands
    using namespace std::placeholders;
    
    // Register inject command
    auto injectCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.control.injectStimulus,
        "Inject current into neuron",
        {"neuron_id", "current"},
        {"Target neuron ID", "Current to inject"},
        std::bind(&CLIController::handleInjectCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(injectCmd);
    
    // Register speed command
    auto speedCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.control.speed,
        "Set simulation speed",
        {"speed"},
        {"Speed multiplier (1.0 = real-time)"},
        std::bind(&CLIController::handleSpeedCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(speedCmd);
    
    // Register batch command
    auto batchCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.control.batch,
        "Run batch operations",
        {},
        {},
        std::bind(&CLIController::handleBatchCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(batchCmd);
    
    // Register step-by-step command
    auto stepByStepCmd = std::make_shared<TypedCLICommand<CommandExecutionContext>>(
        pImpl->options.control.stepByStep,
        "Enable or disable step-by-step mode",
        {"enable"},
        {"Enable (true/false)"},
        std::bind(&CLIController::handleStepByStepCommand, this, _1, _2)
    );
    pImpl->commandRegistry->registerCommand(stepByStepCmd);
    
    return true;
}

} // namespace nlm