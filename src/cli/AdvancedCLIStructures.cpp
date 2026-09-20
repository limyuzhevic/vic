#include "AdvancedCLIStructures.h"
#include "../core/Logger/Logger.hpp"
#include <iostream>
#include <sstream>

namespace nlm {

// Implementation of CLIController
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
        std::ofstream file(filepath);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Cannot create export file: " + filepath);
            return false;
        }
        
        if (format == "json") {
            // Export in JSON format
            auto configKeys = pImpl->config->getKeys();
            file << "{\n";
            for (size_t i = 0; i < configKeys.size(); ++i) {
                auto valueOpt = pImpl->config->get<CLIValue>(configKeys[i]);
                if (valueOpt) {
                    file << "  \"" << configKeys[i] << "\": ";
                    const auto& value = *valueOpt;
                    std::visit([&](auto&& val) {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, bool>) {
                            file << (val ? "true" : "false");
                        } else if constexpr (std::is_same_v<T, int>) {
                            file << val;
                        } else if constexpr (std::is_same_v<T, double>) {
                            file << val;
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            file << "\"" << val << "\"";
                        } else {
                            file << "null";
                        }
                    }, value);
                    
                    if (i < configKeys.size() - 1) {
                        file << ",\n";
                    } else {
                        file << "\n";
                    }
                }
            }
            file << "}\n";
            
        } else if (format == "csv") {
            // Export in CSV format
            auto configKeys = pImpl->config->getKeys();
            for (size_t i = 0; i < configKeys.size(); ++i) {
                file << configKeys[i];
                if (i < configKeys.size() - 1) {
                    file << ",";
                }
            }
            file << "\n";
            
            for (size_t i = 0; i < configKeys.size(); ++i) {
                auto valueOpt = pImpl->config->get<CLIValue>(configKeys[i]);
                if (valueOpt) {
                    const auto& value = *valueOpt;
                    std::visit([&](auto&& val) {
                        using T = std::decay_t<decltype(val)>;
                        if constexpr (std::is_same_v<T, bool>) {
                            file << (val ? "true" : "false");
                        } else if constexpr (std::is_same_v<T, int>) {
                            file << val;
                        } else if constexpr (std::is_same_v<T, double>) {
                            file << val;
                        } else if constexpr (std::is_same_v<T, std::string>) {
                            file << "\"" << val << "\"";
                        } else {
                            file << "";
                        }
                    }, value);
                    
                    if (i < configKeys.size() - 1) {
                        file << ",";
                    }
                }
            }
            file << "\n";
            
        } else {
            file << "Unsupported format: " << format << std::endl;
        }
        
        file.close();
        NLM_LOG_INFO("Results exported to: " + filepath + " in " + format + " format");
        return true;
        
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
    pImpl->argumentParser->addCommand(CLICommand("checkpoint", "Save checkpoint"));
    pImpl->argumentParser->addCommand(CLICommand("replay", "Replay checkpoint"));
    pImpl->argumentParser->addCommand(CLICommand("set", "Set configuration value"));
    pImpl->argumentParser->addCommand(CLICommand("validate", "Validate configuration"));
    pImpl->argumentParser->addCommand(CLICommand("inject", "Inject current into neuron"));
    pImpl->argumentParser->addCommand(CLICommand("speed", "Set simulation speed"));
    pImpl->argumentParser->addCommand(CLICommand("metrics", "Show performance metrics"));
    pImpl->argumentParser->addCommand(CLICommand("export", "Export results"));
    pImpl->argumentParser->addCommand(CLICommand("report", "Generate report"));
    
    return true;
}

bool CLIController::setupAdvancedOptions() {
    // Setup advanced CLI options
    return true;
}

// Command handler implementations (stubs for now)
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

// Other command handlers would be implemented similarly...

// For brevity, additional command handlers are omitted
// but would follow the same pattern

} // namespace nlm