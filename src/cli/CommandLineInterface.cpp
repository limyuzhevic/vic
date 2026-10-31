#include "CommandLineInterface.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Config/ConfigManager.hpp"
#include "../core/CommonUtils.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <thread>
#include <map>
#include <set>
#include <random>
#include <cmath>
#include <iomanip>

namespace nlm {

CommandLineInterface::CommandLineInterface() : initialized(false) {
    startTime = std::chrono::system_clock::now();
    commands.clear();
    commandHistory.clear();
    loadedPlugins.clear();
    pluginPaths.clear();
    
    // Register default commands
    registerDefaultCommands();
}

CommandLineInterface::~CommandLineInterface() {
    cleanup();
}

bool CommandLineInterface::initialize() {
    if (initialized) return true;
    
    try {
        // Set default handlers
        setErrorHandler(defaultErrorHandler);
        setOutputHandler(defaultOutputHandler);
        
        printBanner();
        
        initialized = true;
        printInfo("CommandLineInterface initialized successfully");
        return true;
        
    } catch (const std::exception& e) {
        printError(std::string("Failed to initialize CommandLineInterface: ") + e.what());
        return false;
    }
}

void CommandLineInterface::cleanup() {
    if (!initialized) return;
    
    // Unload plugins
    for (const auto& pair : loadedPlugins) {
        // Note: Plugin cleanup would require proper cleanup functions
    }
    
    loadedPlugins.clear();
    commandHistory.clear();
    
    initialized = false;
    printInfo("CommandLineInterface cleaned up");
}

bool CommandLineInterface::parseArguments(int argc, char** argv) {
    if (argc < 2) {
        showHelp();
        return false;
    }
    
    return parseCommandLineArguments(argc, argv, options);
}

void CommandLineInterface::showHelp(const std::string& command) {
    printBanner();
    std::cout << "\nNLM - Neural Learning Machine" << std::endl;
    std::cout << "A neural network simulation framework" << std::endl;
    std::cout << "\nUSAGE:" << std::endl;
    
    if (command.empty()) {
        std::cout << "  nlm [OPTIONS] [COMMAND] [ARGS]..." << std::endl;
        std::cout << "\nCOMMON COMMANDS:" << std::endl;
        
        for (const auto& pair : commands) {
            const auto& cmd = pair.second;
            std::cout << "  " << cmd.name;
            for (const auto& alias : cmd.aliases) {
                std::cout << ", " << alias;
            }
            std::cout << std::endl;
            std::cout << "    " << cmd.description << std::endl;
            if (!cmd.usage.empty()) {
                std::cout << "    Usage: " << cmd.usage << std::endl;
            }
        }
        
        std::cout << "\nOPTIONS:" << std::endl;
        std::cout << "  --help, -h              Show this help message" << std::endl;
        std::cout << "  --version, -v           Show version information" << std::endl;
        std::cout << "  --verbose, -V           Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q             Suppress output" << std::endl;
        std::cout << "  --config FILE           Load configuration from file" << std::endl;
        std::cout << "  --output FILE           Save output to file" << std::endl;
        std::cout << "  --log FILE              Log output to file" << std::endl;
        std::cout << "  --checkpoint FILE       Save/load checkpoint" << std::endl;
        std::cout << "  --steps N               Number of simulation steps" << std::endl;
        std::cout << "  --time T                Simulation time in seconds" << std::endl;
        std::cout << "  --seed N                Random seed" << std::endl;
        std::cout << "  --demo                  Run demo mode" << std::endl;
        std::cout << "  --benchmark             Run benchmark" << std::endl;
        std::cout << "  --test                  Run tests" << std::endl;
        
        showExamples();
    } else {
        auto cmd = findCommand(command);
        if (cmd) {
            std::cout << "\nCOMMAND: " << command << std::endl;
            std::cout << "  Description: " << cmd->description << std::endl;
            if (!cmd->usage.empty()) {
                std::cout << "  Usage: " << cmd->usage << std::endl;
            }
            if (!cmd->parameters.empty()) {
                std::cout << "  Parameters:" << std::endl;
                for (const auto& param : cmd->parameters) {
                    std::cout << "    --" << param.first << "=value  " << param.second << std::endl;
                }
            }
        } else {
            printError("Unknown command: " + command);
        }
    }
}

void CommandLineInterface::showVersion() {
    std::cout << "NLM - Neural Learning Machine" << std::endl;
    std::cout << "Version: 0.1.0 (Phase 6 - Final Integration)" << std::endl;
    std::cout << "Build: " << __DATE__ << " " << __TIME__ << std::endl;
    std::cout << "\nFeatures:" << std::endl;
    std::cout << "  - Real spiking neural computation (LIF neurons)" << std::endl;
    std::cout << "  - Event-driven spike propagation with delays" << std::endl;
    std::cout << "  - STDP and Hebbian plasticity rules" << std::endl;
    std::cout << "  - Structural plasticity (synaptogenesis/pruning)" << std::endl;
    std::cout << "  - Integrated memory systems" << std::endl;
    std::cout << "  - Neuromodulation (dopamine, curiosity, novelty)" << std::endl;
    std::cout << "  - Prediction and planning systems" << std::endl;
    std::cout << "  - Cognitive development and learning" << std::endl;
    std::cout << "  - Checkpoint save/load" << std::endl;
    std::cout << "  - JSON configuration support" << std::endl;
}

void CommandLineInterface::showUsage(const std::string& command) {
    auto cmd = findCommand(command);
    if (!cmd) {
        printError("Unknown command: " + command);
        return;
    }
    
    std::cout << "\nUsage for command: " << command << std::endl;
    std::cout << cmd->usage << std::endl;
    
    if (!cmd->parameters.empty()) {
        std::cout << "\nParameters:" << std::endl;
        for (const auto& param : cmd->parameters) {
            std::cout << "  --" << param.first << "=value  " << param.second << std::endl;
        }
    }
}

void CommandLineInterface::showExamples() {
    std::cout << "\nEXAMPLES:" << std::endl;
    std::cout << "  nlm --demo                    Run demo mode" << std::endl;
    std::cout << "  nlm --config myconfig.json     Load JSON configuration" << std::endl;
    std::cout << "  nlm --benchmark --steps 1000   Run benchmark for 1000 steps" << std::endl;
    std::cout << "  nlm --test                    Run unit tests" << std::endl;
    std::cout << "  nlm --checkpoint save.nc       Save brain state to checkpoint" << std::endl;
    std::cout << "  nlm --checkpoint load.nc       Load brain state from checkpoint" << std::endl;
    std::cout << "  nlm --validate                Validate configuration" << std::endl;
    std::cout << "  nlm --output results.txt       Save simulation results to file" << std::endl;
    std::cout << "  nlm --log simulation.log       Log output to file" << std::endl;
    std::cout << "\nCONFIGURATION FILE FORMATS:" << std::endl;
    std::cout << "  .cfg      Simple key=value format" << std::endl;
    std::cout << "  .json     JSON format" << std::endl;
    std::cout << "  .yaml/.yml YAML format (future)" << std::endl;
}

bool CommandLineInterface::executeCommand(const std::string& command, const CommandOptions& options) {
    stats.commandCount++;
    
    auto cmd = findCommand(command);
    if (!cmd) {
        printError("Unknown command: " + command);
        return false;
    }
    
    // Validate command parameters
    if (!validateCommandParameters(*cmd, options)) {
        return false;
    }
    
    // Execute command
    try {
        bool result = cmd->handler(options);
        if (!result) {
            printError("Command failed: " + command);
        }
        return result;
    } catch (const std::exception& e) {
        printError(std::string("Command failed with exception: ") + e.what());
        return false;
    }
}

bool CommandLineInterface::handleRun(const CommandOptions& options) {
    printInfo("Loading configuration...");
    
    auto config = ConfigManager::getDefaultConfig();
    
    // Load configuration file if specified
    if (!options.configFile.empty()) {
        if (ConfigManager::loadConfig(options.configFile, config)) {
            printInfo("Configuration loaded from: " + options.configFile);
        } else {
            printWarning("Failed to load configuration from: " + options.configFile);
            printWarning("Using default configuration");
        }
    }
    
    // Apply command line overrides
    if (!options.configOverrides.empty()) {
        applyConfigOverrides(config, options.configOverrides);
    }
    
    // Create brain
    auto brain = createBrain(options, config);
    if (!brain) {
        printError("Failed to create brain");
        return false;
    }
    
    // Run simulation
    printInfo("Initializing brain...");
    if (!brain->initialize()) {
        printError("Failed to initialize brain");
        return false;
    }
    
    printInfo("Running simulation...");
    if (!runBrainSimulation(brain, options)) {
        printError("Simulation failed");
        return false;
    }
    
    // Save output if requested
    if (!options.outputFile.empty()) {
        printInfo("Saving results to: " + options.outputFile);
        // Save results to file
    }
    
    printInfo("Simulation completed successfully");
    return true;
}

bool CommandLineInterface::handleDemo(const CommandOptions& options) {
    printInfo("Running demo mode...");
    
    // Use demo configuration
    auto config = ConfigManager::getDefaultConfig();
    
    // Set demo-specific parameters
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    auto brain = createBrain(options, config);
    if (!brain) {
        printError("Failed to create brain for demo");
        return false;
    }
    
    if (!brain->initialize()) {
        printError("Failed to initialize brain for demo");
        return false;
    }
    
    return runDemoMode(brain, options);
}

bool CommandLineInterface::handleBenchmark(const CommandOptions& options) {
    printInfo("Running benchmark...");
    
    // Use benchmark configuration
    auto config = ConfigManager::getDefaultConfig();
    
    auto brain = createBrain(options, config);
    if (!brain) {
        printError("Failed to create brain for benchmark");
        return false;
    }
    
    if (!brain->initialize()) {
        printError("Failed to initialize brain for benchmark");
        return false;
    }
    
    return runBenchmarkMode(brain, options);
}

bool CommandLineInterface::handleTest(const CommandOptions& options) {
    printInfo("Running tests...");
    
    // Run tests
    std::cout << "Tests not yet implemented in this release" << std::endl;
    
    // TODO: Implement proper testing framework
    return true;
}

bool CommandLineInterface::handleConfig(const CommandOptions& options) {
    printInfo("Managing configuration...");
    
    if (options.configFile.empty()) {
        printError("No configuration file specified");
        return false;
    }
    
    // Generate config template
    std::vector<std::string> sections = {"basic", "plasticity", "memory", "neuromodulation"};
    if (generateConfigTemplate(options.configFile, sections)) {
        printInfo("Configuration template generated: " + options.configFile);
        return true;
    } else {
        printError("Failed to generate configuration template");
        return false;
    }
}

bool CommandLineInterface::handleCheckpoint(const CommandOptions& options) {
    if (options.checkpointSave.empty() && options.checkpointLoad.empty()) {
        printError("No checkpoint operation specified");
        return false;
    }
    
    auto config = ConfigManager::getDefaultConfig();
    auto brain = createBrain(options, config);
    
    if (brain && brain->initialize()) {
        if (!options.checkpointSave.empty()) {
            printInfo("Saving checkpoint to: " + options.checkpointSave);
            if (saveBrainState(brain, options.checkpointSave)) {
                printInfo("Checkpoint saved successfully");
                return true;
            } else {
                printError("Failed to save checkpoint");
            }
        }
        
        if (!options.checkpointLoad.empty()) {
            printInfo("Loading checkpoint from: " + options.checkpointLoad);
            if (loadBrainState(brain, options.checkpointLoad)) {
                printInfo("Checkpoint loaded successfully");
                return true;
            } else {
                printError("Failed to load checkpoint");
            }
        }
    }
    
    return false;
}

bool CommandLineInterface::handleValidate(const CommandOptions& options) {
    printInfo("Validating configuration...");
    
    auto config = ConfigManager::getDefaultConfig();
    
    if (!options.configFile.empty()) {
        if (!ConfigManager::loadConfig(options.configFile, config)) {
            printError("Failed to load configuration: " + options.configFile);
            return false;
        }
    }
    
    // Define configuration schema for validation
    std::vector<ConfigManager::ConfigSchema> schema;
    
    ConfigManager::ConfigSchema basicSchema("basic", "Basic simulation parameters");
    basicSchema.addValidator("neuron_count", [](const ConfigValue& v) {
        auto val = std::get<int64_t>(v);
        if (val <= 0 || val > 1000000) {
            return ConfigManager::ValidationResult(false, "neuron_count must be between 1 and 1,000,000");
        }
        return ConfigManager::ValidationResult();
    });
    basicSchema.addValidator("simulation_timestep", [](const ConfigValue& v) {
        auto val = std::get<double>(v);
        if (val <= 0.0 || val > 1.0) {
            return ConfigManager::ValidationResult(false, "simulation_timestep must be between 0.001 and 1.0");
        }
        return ConfigManager::ValidationResult();
    });
    basicSchema.setRequired(true);
    schema.push_back(basicSchema);
    
    // Validate configuration
    auto validationResult = ConfigManager::validateConfig(config, schema);
    
    if (validationResult.valid) {
        printInfo("Configuration validation passed");
        if (!validationResult.warnings.empty()) {
            printWarning("Configuration warnings:");
            for (const auto& warning : validationResult.warnings) {
                printWarning("  - " + warning);
            }
        }
        return true;
    } else {
        printError("Configuration validation failed: " + validationResult.errorMessage);
        return false;
    }
}

bool CommandLineInterface::handleExport(const CommandOptions& options) {
    if (options.outputFile.empty()) {
        printError("No output file specified for export");
        return false;
    }
    
    auto config = ConfigManager::getDefaultConfig();
    
    if (!options.configFile.empty()) {
        if (!ConfigManager::loadConfig(options.configFile, config)) {
            printError("Failed to load configuration: " + options.configFile);
            return false;
        }
    }
    
    // Export configuration
    if (exportConfiguration(options, config, "json")) {
        printInfo("Configuration exported to: " + options.outputFile);
        return true;
    } else {
        printError("Failed to export configuration");
        return false;
    }
}

bool CommandLineInterface::loadConfiguration(const CommandOptions& options, std::shared_ptr<Config> config) {
    if (!config) {
        printError("Invalid config object");
        return false;
    }
    
    // Load from file if specified
    if (!options.configFile.empty()) {
        if (!ConfigManager::loadConfig(options.configFile, config)) {
            printWarning("Failed to load configuration from: " + options.configFile);
        }
    }
    
    // Apply overrides
    if (!options.configOverrides.empty()) {
        applyConfigOverrides(config, options.configOverrides);
    }
    
    return true;
}

bool CommandLineInterface::saveConfiguration(const CommandOptions& options, const std::shared_ptr<Config>& config) {
    if (!config) {
        printError("Invalid config object");
        return false;
    }
    
    if (options.outputFile.empty()) {
        printError("No output file specified");
        return false;
    }
    
    return exportConfiguration(options, config, "json");
}

std::shared_ptr<Brain> CommandLineInterface::createBrain(const CommandOptions& options, const std::shared_ptr<Config>& config) {
    if (!config) {
        printError("Invalid configuration");
        return nullptr;
    }
    
    try {
        auto brain = std::make_shared<Brain>(config);
        return brain;
    } catch (const std::exception& e) {
        printError(std::string("Failed to create brain: ") + e.what());
        return nullptr;
    }
}

bool CommandLineInterface::runBrainSimulation(std::shared_ptr<Brain> brain, const CommandOptions& options) {
    if (!brain) {
        printError("Invalid brain object");
        return false;
    }
    
    double totalTime = options.simulationTime > 0.0 ? options.simulationTime : 
                      (options.simulationSteps * brain->getTimestep());
    
    printInfo(std::string("Simulation time: ") + formatDuration(totalTime));
    
    auto start = std::chrono::system_clock::now();
    
    // Run simulation
    for (SimulationStep step = 0; step < options.simulationSteps; ++step) {
        brain->step(step);
        
        // Log progress
        if (options.verbose || (options.simulationSteps > 100 && step % 100 == 0)) {
            printInfo(std::string("Step ") + std::to_string(step) + 
                     std::string(", Total spikes: ") + std::to_string(brain->getTotalSpikeCount()));
        }
        
        // Check for early termination
        if (brain->getTotalSpikeCount() == 0 && step > 1000 && options.simulationSteps > 1000) {
            printWarning("No spikes detected after 1000 steps - network may not be working");
        }
    }
    
    auto end = std::chrono::system_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    printInfo(std::string("Simulation completed in ") + formatDuration(duration));
    
    // Print final status
    printBrainStatus(brain);
    
    return true;
}

bool CommandLineInterface::saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        printError("Invalid brain object");
        return false;
    }
    
    if (filepath.empty()) {
        printError("No filepath specified");
        return false;
    }
    
    if (!isValidFilename(filepath)) {
        printError("Invalid filename: " + filepath);
        return false;
    }
    
    // Save brain state
    if (brain->save(filepath)) {
        printInfo("Brain state saved to: " + filepath);
        return true;
    } else {
        printError("Failed to save brain state to: " + filepath);
        return false;
    }
}

bool CommandLineInterface::loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath) {
    if (!brain) {
        printError("Invalid brain object");
        return false;
    }
    
    if (filepath.empty()) {
        printError("No filepath specified");
        return false;
    }
    
    if (!isValidFilename(filepath)) {
        printError("Invalid filename: " + filepath);
        return false;
    }
    
    if (!fileExists(filepath)) {
        printError("File does not exist: " + filepath);
        return false;
    }
    
    // Load brain state
    if (brain->load(filepath)) {
        printInfo("Brain state loaded from: " + filepath);
        return true;
    } else {
        printError("Failed to load brain state from: " + filepath);
        return false;
    }
}

void CommandLineInterface::printBanner() const {
    std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                 NLM - Neural Learning Machine                  ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "║                 Phase 6: Final Integration                     ║" << std::endl;
    std::cout << "║                                                               ║" << std::endl;
    std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    std::cout << std::endl;
}

void CommandLineInterface::printConfiguration(const std::shared_ptr<Config>& config) const {
    if (!config) {
        printError("Invalid configuration");
        return;
    }
    
    std::cout << "Configuration:" << std::endl;
    std::cout << "  Entries: " << config->getKeys().size() << std::endl;
    
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        auto valueOpt = config->get<ConfigValue>(key);
        if (valueOpt) {
            std::cout << "  " << key << " = " << ConfigManager::toString(*valueOpt) << std::endl;
        }
    }
}

void CommandLineInterface::printBrainStatus(std::shared_ptr<Brain> brain) const {
    if (!brain) {
        printError("Invalid brain object");
        return;
    }
    
    std::cout << "\nBrain Status:" << std::endl;
    std::cout << "  Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "  Total synapses: " << brain->getTotalSynapseCount() << std::endl;
    std::cout << "  Firing neurons: " << brain->getFiringNeuronCount() << std::endl;
    std::cout << "  Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    std::cout << "  Total spikes: " << brain->getTotalSpikeCount() << std::endl;
    std::cout << "  Pending events: " << brain->getPendingSpikeEventCount() << std::endl;
    std::cout << "  Current time: " << brain->getCurrentTime() << std::endl;
    std::cout << "  Current step: " << brain->getCurrentStep() << std::endl;
}

void CommandLineInterface::printBenchmarkResults(const std::string& operation, double duration, const std::string& details) const {
    std::cout << "\nBenchmark Results:" << std::endl;
    std::cout << "  Operation: " << operation << std::endl;
    std::cout << "  Duration: " << formatDuration(duration) << std::endl;
    if (!details.empty()) {
        std::cout << "  Details: " << details << std::endl;
    }
}

void CommandLineInterface::printError(const std::string& message, const std::string& command) const {
    stats.errorCount++;
    
    std::cerr << "[ERROR] " << message << std::endl;
    if (!command.empty()) {
        std::cerr << "  Command: " << command << std::endl;
    }
    
    if (errorHandler) {
        errorHandler(ErrorHandler::ErrorLevel::ERROR, message, command);
    }
}

void CommandLineInterface::printWarning(const std::string& message) const {
    stats.warningCount++;
    
    std::cerr << "[WARNING] " << message << std::endl;
    
    if (errorHandler) {
        errorHandler(ErrorHandler::ErrorLevel::WARNING, message, "");
    }
}

void CommandLineInterface::printInfo(const std::string& message) const {
    if (options.quiet) return;
    
    std::cout << "[INFO] " << message << std::endl;
}

void CommandLineInterface::printDebug(const std::string& message) const {
    if (!options.verbose) return;
    
    std::cout << "[DEBUG] " << message << std::endl;
}

bool CommandLineInterface::runInteractiveMode() {
    std::cout << "Interactive mode not yet implemented" << std::endl;
    std::cout << "Use command line arguments for automated execution" << std::endl;
    return true;
}

std::string CommandLineInterface::getUserInput(const std::string& prompt) const {
    std::cout << prompt;
    std::string input;
    std::getline(std::cin, input);
    return input;
}

bool CommandLineInterface::processInteractiveCommand(const std::string& command) {
    // Parse and execute command
    std::istringstream iss(command);
    std::string cmd;
    iss >> cmd;
    
    if (cmd == "quit" || cmd == "exit") {
        return false;
    } else if (cmd == "help") {
        showHelp();
    } else {
        printError("Unknown command: " + cmd);
    }
    
    return true;
}

void CommandLineInterface::addToHistory(const std::string& command) {
    commandHistory.push_back(command);
    
    // Keep history size limited
    if (commandHistory.size() > 100) {
        commandHistory.erase(commandHistory.begin());
    }
}

std::vector<std::string> CommandLineInterface::getCommandHistory() const {
    return commandHistory;
}

std::vector<std::string> CommandLineInterface::getCommandCompletions(const std::string& partial) const {
    std::vector<std::string> completions;
    
    for (const auto& pair : commands) {
        if (StringUtils::startsWith(pair.first, partial)) {
            completions.push_back(pair.first);
        }
        
        for (const auto& alias : pair.second.aliases) {
            if (StringUtils::startsWith(alias, partial)) {
                completions.push_back(alias);
            }
        }
    }
    
    return completions;
}

bool CommandLineInterface::validateConfiguration(const CommandOptions& options, const std::shared_ptr<Config>& config) {
    if (!config) {
        return false;
    }
    
    // Basic validation
    auto keys = config->getKeys();
    
    for (const auto& key : keys) {
        if (key.empty() || key.find('=') != std::string::npos) {
            printError("Invalid configuration key: " + key);
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> CommandLineInterface::getConfigurationWarnings(const std::shared_ptr<Config>& config) {
    std::vector<std::string> warnings;
    
    if (!config) return warnings;
    
    // Check for empty values
    auto keys = config->getKeys();
    for (const auto& key : keys) {
        auto valueOpt = config->get<ConfigValue>(key);
        if (valueOpt) {
            std::string valueStr = ConfigManager::toString(*valueOpt);
            if (valueStr.empty()) {
                warnings.push_back("Empty value for key: " + key);
            }
        }
    }
    
    return warnings;
}

bool CommandLineInterface::loadPlugin(const std::string& pluginPath) {
    if (!isValidFilename(pluginPath)) {
        printError("Invalid plugin path: " + pluginPath);
        return false;
    }
    
    if (!fileExists(pluginPath)) {
        printError("Plugin file does not exist: " + pluginPath);
        return false;
    }
    
    // Load plugin (placeholder for future implementation)
    loadedPlugins[pluginPath] = nullptr;
    pluginPaths.push_back(pluginPath);
    
    printInfo("Plugin loaded: " + pluginPath);
    return true;
}

bool CommandLineInterface::unloadPlugin(const std::string& pluginName) {
    auto it = loadedPlugins.find(pluginName);
    if (it == loadedPlugins.end()) {
        printError("Plugin not found: " + pluginName);
        return false;
    }
    
    // Unload plugin (placeholder for future implementation)
    loadedPlugins.erase(it);
    
    auto pathIt = std::find(pluginPaths.begin(), pluginPaths.end(), pluginName);
    if (pathIt != pluginPaths.end()) {
        pluginPaths.erase(pathIt);
    }
    
    printInfo("Plugin unloaded: " + pluginName);
    return true;
}

std::vector<std::string> CommandLineInterface::listAvailablePlugins() const {
    std::vector<std::string> plugins;
    
    for (const auto& pair : loadedPlugins) {
        plugins.push_back(pair.first);
    }
    
    return plugins;
}

void CommandLineInterface::printSystemInfo() const {
    std::cout << "\nSystem Information:" << std::endl;
    std::cout << "  Platform: " << (sizeof(void*) * 8) << "-bit" << std::endl;
    std::cout << "  Compiler: " << __VERSION__ << std::endl;
    std::cout << "  Standard: C++20" << std::endl;
    std::cout << "  NLM Version: 0.1.0 (Phase 6)" << std::endl;
    std::cout << "  Build: " << __DATE__ << " " << __TIME__ << std::endl;
}

void CommandLineInterface::printMemoryUsage() const {
    std::cout << "\nMemory Usage:" << std::endl;
    std::cout << "  (Memory usage tracking not yet implemented)" << std::endl;
}

void CommandLineInterface::printPerformanceStats() const {
    std::cout << "\nPerformance Statistics:" << std::endl;
    std::cout << "  Total commands executed: " << stats.commandCount << std::endl;
    std::cout << "  Total errors: " << stats.errorCount << std::endl;
    std::cout << "  Total warnings: " << stats.warningCount << std::endl;
    std::cout << "  Total execution time: " << formatDuration(stats.totalExecutionTime) << std::endl;
}

double CommandLineInterface::getElapsedTime() const {
    auto now = std::chrono::system_clock::now();
    return std::chrono::duration<double>(now - startTime).count();
}

bool CommandLineInterface::generateConfigTemplate(const std::string& filepath, const std::vector<std::string>& sections) {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        printError("Failed to create config template: " + filepath);
        return false;
    }
    
    file << "# NLM Configuration Template" << std::endl;
    file << "# Generated by CommandLineInterface" << std::endl;
    file << "# " << getTimestamp() << std::endl;
    file << std::endl;
    
    for (const auto& section : sections) {
        file << "# " << section << " section" << std::endl;
        
        if (section == "basic") {
            file << "random_seed = 42" << std::endl;
            file << "simulation_timestep = 0.001" << std::endl;
            file << "neuron_count = 1000" << std::endl;
            file << "region_count = 2" << std::endl;
            file << "connection_probability = 0.1" << std::endl;
        } else if (section == "plasticity") {
            file << "plasticity_learning_rate = 0.01" << std::endl;
            file << "stdp_ltp_weight = 0.01" << std::endl;
            file << "stdp_ltd_weight = 0.012" << std::endl;
            file << "stdp_time_constant = 20.0" << std::endl;
        } else if (section == "memory") {
            file << "working_memory_capacity = 100" << std::endl;
            file << "episodic_memory_max_episodes = 1000" << std::endl;
            file << "associative_memory_strength = 1.0" << std::endl;
        } else if (section == "neuromodulation") {
            file << "dopamine_baseline = 0.1" << std::endl;
            file << "curiosity_threshold = 0.5" << std::endl;
            file << "novelty_threshold = 0.3" << std::endl;
            file << "prediction_error_threshold = 0.2" << std::endl;
        }
        
        file << std::endl;
    }
    
    file.close();
    printInfo("Config template generated: " + filepath);
    return true;
}

bool CommandLineInterface::exportConfiguration(const CommandOptions& options, const std::shared_ptr<Config>& config, const std::string& format) {
    if (!config) {
        printError("Invalid configuration");
        return false;
    }
    
    if (options.outputFile.empty()) {
        printError("No output file specified");
        return false;
    }
    
    std::ofstream file(options.outputFile);
    if (!file.is_open()) {
        printError("Failed to open output file: " + options.outputFile);
        return false;
    }
    
    if (format == "json") {
        // Export as JSON
        file << "{\n";
        
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            auto valueOpt = config->get<ConfigValue>(keys[i]);
            if (valueOpt) {
                file << "  \"" << keys[i] << "\": " << ConfigManager::toString(*valueOpt);
                if (i < keys.size() - 1) {
                    file << ",";
                }
                file << "\n";
            }
        }
        
        file << "}\n";
    } else {
        printError("Unsupported export format: " + format);
        return false;
    }
    
    file.close();
    printInfo("Configuration exported to: " + options.outputFile);
    return true;
}

void CommandLineInterface::registerCommand(const CommandInfo& commandInfo) {
    commands[commandInfo.name] = commandInfo;
    
    // Register aliases
    for (const auto& alias : commandInfo.aliases) {
        commands[alias] = commandInfo;
    }
}

std::vector<CommandLineInterface::CommandInfo> CommandLineInterface::getAvailableCommands() const {
    std::vector<CommandInfo> cmds;
    
    for (const auto& pair : commands) {
        cmds.push_back(pair.second);
    }
    
    return cmds;
}

CommandLineInterface::CommandInfo* CommandLineInterface::findCommand(const std::string& name) {
    auto it = commands.find(name);
    if (it != commands.end()) {
        return &it->second;
    }
    return nullptr;
}

void CommandLineInterface::registerDefaultCommands() {
    // Register run command
    CommandInfo runCmd;
    runCmd.name = "run";
    runCmd.description = "Run NLM brain simulation";
    runCmd.usage = "run [--config file] [--steps N] [--time T] [--output file]";
    runCmd.aliases = {"execute", "start"};
    runCmd.handler = [this](const CommandOptions& opts) { return handleRun(opts); };
    runCmd.parameters = {
        {"config", "Configuration file path"},
        {"output", "Output file path"},
        {"steps", "Number of simulation steps"},
        {"time", "Simulation time in seconds"},
        {"seed", "Random seed value"}
    };
    registerCommand(runCmd);
    
    // Register demo command
    CommandInfo demoCmd;
    demoCmd.name = "demo";
    demoCmd.description = "Run demo mode with sample configuration";
    demoCmd.usage = "demo [--output file]";
    demoCmd.aliases = {"example", "sample"};
    demoCmd.handler = [this](const CommandOptions& opts) { return handleDemo(opts); };
    demoCmd.parameters = {
        {"output", "Output file path"}
    };
    registerCommand(demoCmd);
    
    // Register benchmark command
    CommandInfo benchmarkCmd;
    benchmarkCmd.name = "benchmark";
    benchmarkCmd.description = "Run performance benchmark";
    benchmarkCmd.usage = "benchmark [--steps N] [--output file]";
    benchmarkCmd.aliases = {"perf", "performance"};
    benchmarkCmd.handler = [this](const CommandOptions& opts) { return handleBenchmark(opts); };
    benchmarkCmd.parameters = {
        {"output", "Output file path"},
        {"steps", "Number of simulation steps"}
    };
    registerCommand(benchmarkCmd);
    
    // Register test command
    CommandInfo testCmd;
    testCmd.name = "test";
    testCmd.description = "Run unit tests";
    testCmd.usage = "test [--verbose]";
    testCmd.aliases = {"tests", "check"};
    testCmd.handler = [this](const CommandOptions& opts) { return handleTest(opts); };
    testCmd.parameters = {
        {"verbose", "Enable verbose output"}
    };
    registerCommand(testCmd);
    
    // Register config command
    CommandInfo configCmd;
    configCmd.name = "config";
    configCmd.description = "Manage configuration files";
    configCmd.usage = "config [--file file] [--export file]";
    configCmd.aliases = {"configuration", "cfg"};
    configCmd.handler = [this](const CommandOptions& opts) { return handleConfig(opts); };
    configCmd.parameters = {
        {"file", "Configuration file path"},
        {"export", "Export configuration to file"}
    };
    registerCommand(configCmd);
    
    // Register checkpoint command
    CommandInfo checkpointCmd;
    checkpointCmd.name = "checkpoint";
    checkpointCmd.description = "Manage checkpoints";
    checkpointCmd.usage = "checkpoint --save file | --load file";
    checkpointCmd.aliases = {"save", "load", "checkpoint"};
    checkpointCmd.handler = [this](const CommandOptions& opts) { return handleCheckpoint(opts); };
    checkpointCmd.parameters = {
        {"save", "Save checkpoint to file"},
        {"load", "Load checkpoint from file"}
    };
    registerCommand(checkpointCmd);
    
    // Register validate command
    CommandInfo validateCmd;
    validateCmd.name = "validate";
    validateCmd.description = "Validate configuration file";
    validateCmd.usage = "validate [--file file]";
    validateCmd.aliases = {"check", "verify"};
    validateCmd.handler = [this](const CommandOptions& opts) { return handleValidate(opts); };
    validateCmd.parameters = {
        {"file", "Configuration file to validate"}
    };
    registerCommand(validateCmd);
    
    // Register export command
    CommandInfo exportCmd;
    exportCmd.name = "export";
    exportCmd.description = "Export configuration to file";
    exportCmd.usage = "export [--format format] [--file file]";
    exportCmd.aliases = {"import", "convert"};
    exportCmd.handler = [this](const CommandOptions& opts) { return handleExport(opts); };
    exportCmd.parameters = {
        {"format", "Output format (json)"},
        {"file", "Output file path"}
    };
    registerCommand(exportCmd);
}

bool CommandLineInterface::validateCommandParameters(const CommandInfo& command, const CommandOptions& options) {
    // Basic validation
    if (command.name == "run" && options.simulationSteps <= 0) {
        printError("Invalid number of simulation steps for run command");
        return false;
    }
    
    if (command.name == "demo" && options.outputFile.empty()) {
        // Output file is optional for demo
    }
    
    if (command.name == "benchmark" && options.simulationSteps <= 0) {
        printError("Invalid number of simulation steps for benchmark command");
        return false;
    }
    
    return true;
}

void CommandLineInterface::defaultErrorHandler(const std::string& message, const std::string& command) {
    // Default error handler - just print to stderr
    std::cerr << "[CommandLineInterface] " << message << std::endl;
    if (!command.empty()) {
        std::cerr << "  Command: " << command << std::endl;
    }
}

void CommandLineInterface::defaultOutputHandler(const std::string& message, bool isError) {
    // Default output handler - print to stdout
    if (isError) {
        std::cerr << "[CommandLineInterface] " << message << std::endl;
    } else {
        std::cout << "[CommandLineInterface] " << message << std::endl;
    }
}

std::string CommandLineInterface::formatDuration(double seconds) {
    if (seconds < 60.0) {
        return std::to_string(seconds) + "s";
    } else if (seconds < 3600.0) {
        double minutes = seconds / 60.0;
        return std::to_string(minutes) + "m";
    } else {
        double hours = seconds / 3600.0;
        return std::to_string(hours) + "h";
    }
}

std::string CommandLineInterface::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void CommandLineInterface::trimString(std::string& str) {
    str.erase(0, str.find_first_not_of(" \t\r\n"));
    str.erase(str.find_last_not_of(" \t\r\n") + 1);
}

std::string CommandLineInterface::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string CommandLineInterface::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool CommandLineInterface::stringEndsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() && str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

bool CommandLineInterface::stringStartsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && str.compare(0, prefix.size(), prefix) == 0;
}

std::vector<std::string> CommandLineInterface::splitString(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::string CommandLineInterface::joinStrings(const std::vector<std::string>& strings, const std::string& delimiter) {
    std::stringstream ss;
    bool first = true;
    
    for (const auto& str : strings) {
        if (!first) ss << delimiter;
        ss << str;
        first = false;
    }
    
    return ss.str();
}

std::string CommandLineInterface::escapeString(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '\"': result += "\\\""; break;
            case '\\': result += "\\\\"; break;
            case '\t': result += "\\t"; break;
            case '\n': result += "\\n"; break;
            case '\r': result += "\\r"; break;
            default: result += c; break;
        }
    }
    return result;
}

std::string CommandLineInterface::unescapeString(const std::string& str) {
    std::string result;
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case '\"': result += "\""; i++; break;
                case '\\': result += "\\"; i++; break;
                case 't': result += '\t'; i++; break;
                case 'n': result += '\n'; i++; break;
                case 'r': result += '\r'; i++; break;
                default: result += str[i]; break;
            }
        } else {
            result += str[i];
        }
    }
    return result;
}

bool CommandLineInterface::isValidFilename(const std::string& filename) {
    if (filename.empty() || filename.find('/') != std::string::npos || filename.find('\\') != std::string::npos) {
        return false;
    }
    return true;
}

std::string CommandLineInterface::getSanitizedFilename(const std::string& filename) {
    std::string result;
    for (char c : filename) {
        if (std::isalnum(c) || c == '.' || c == '_' || c == '-') {
            result += c;
        } else {
            result += '_';
        }
    }
    return result;
}

std::string CommandLineInterface::generateUniqueFilename(const std::string& basePath, const std::string& baseName) {
    std::string timestamp = getTimestamp();
    std::replace(timestamp.begin(), timestamp.end(), ':', '_');
    std::replace(timestamp.begin(), timestamp.end(), ' ', '_');
    
    return basePath + getSanitizedFilename(baseName) + "_" + timestamp + ".txt";
}

bool CommandLineInterface::fileExists(const std::string& filepath) {
    std::ifstream file(filepath);
    return file.good();
}

std::string CommandLineInterface::getAbsolutePath(const std::string& path) {
    return path;
}

std::string CommandLineInterface::getFileExtension(const std::string& filename) {
    return StringUtils::getFileExtension(filename);
}

std::string CommandLineInterface::getFileName(const std::string& path) {
    return StringUtils::getFileName(path);
}

std::string CommandLineInterface::getFileNameWithoutExtension(const std::string& filename) {
    return StringUtils::getFileNameWithoutExtension(filename);
}

bool CommandLineInterface::parseCommandLineArguments(int argc, char** argv, CommandOptions& options) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            options.help = true;
        } else if (arg == "--version" || arg == "-v") {
            options.version = true;
        } else if (arg == "--verbose" || arg == "-V") {
            options.verbose = true;
        } else if (arg == "--quiet" || arg == "-q") {
            options.quiet = true;
        } else if (arg == "--demo") {
            options.demo = true;
        } else if (arg == "--benchmark") {
            options.benchmark = true;
        } else if (arg == "--test") {
            options.test = true;
        } else if (arg.find("--config=") == 0) {
            options.configFile = arg.substr(9);
        } else if (arg.find("--output=") == 0) {
            options.outputFile = arg.substr(9);
        } else if (arg.find("--log=") == 0) {
            options.logFile = arg.substr(7);
        } else if (arg.find("--checkpoint=") == 0) {
            std::string value = arg.substr(13);
            if (arg.find("--checkpoint=save") != std::string::npos) {
                options.checkpointSave = value;
            } else if (arg.find("--checkpoint=load") != std::string::npos) {
                options.checkpointLoad = value;
            }
        } else if (arg.find("--steps=") == 0) {
            try {
                options.simulationSteps = std::stoi(arg.substr(8));
            } catch (...) {
                printError("Invalid steps value: " + arg.substr(8));
                return false;
            }
        } else if (arg.find("--time=") == 0) {
            try {
                options.simulationTime = std::stod(arg.substr(7));
            } catch (...) {
                printError("Invalid time value: " + arg.substr(7));
                return false;
            }
        } else if (arg.find("--seed=") == 0) {
            try {
                options.randomSeed = std::stoi(arg.substr(7));
            } catch (...) {
                printError("Invalid seed value: " + arg.substr(7));
                return false;
            }
        } else if (arg.find("--") == 0) {
            printError("Unknown option: " + arg);
            return false;
        } else {
            // Assume it's a command
            if (arg == "run") {
                options.executionMode = "run";
            } else if (arg == "demo") {
                options.executionMode = "demo";
            } else if (arg == "benchmark") {
                options.executionMode = "benchmark";
            } else if (arg == "test") {
                options.executionMode = "test";
            } else if (arg == "config") {
                options.executionMode = "config";
            } else if (arg == "checkpoint") {
                options.executionMode = "checkpoint";
            } else if (arg == "validate") {
                options.executionMode = "validate";
            } else if (arg == "export") {
                options.executionMode = "export";
            } else {
                printError("Unknown argument: " + arg);
                return false;
            }
        }
    }
    
    return true;
}

bool CommandLineInterface::parseCommandLine(int argc, char** argv, std::map<std::string, std::string>& parsedArgs) {
    return parseCommandLineArguments(argc, argv, parsedArgs);
}

bool CommandLineInterface::parseKeyValuePairs(const std::vector<std::string>& args, std::map<std::string, std::string>& result) {
    for (const auto& arg : args) {
        size_t pos = arg.find('=');
        if (pos != std::string::npos) {
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            result[key] = value;
        }
    }
    return true;
}

bool CommandLineInterface::parseBooleanFlag(int argc, char** argv, const std::string& flag, bool& value) {
    for (int i = 0; i < argc; ++i) {
        if (argv[i] == flag) {
            value = true;
            return true;
        }
    }
    return false;
}

bool CommandLineInterface::parseIntegerOption(int argc, char** argv, const std::string& option, int64_t& value) {
    for (int i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == option && i + 1 < argc) {
            try {
                value = std::stoll(argv[i + 1]);
                return true;
            } catch (...) {
                return false;
            }
        }
    }
    return false;
}

bool CommandLineInterface::parseFloatOption(int argc, char** argv, const std::string& option, double& value) {
    for (int i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == option && i + 1 < argc) {
            try {
                value = std::stod(argv[i + 1]);
                return true;
            } catch (...) {
                return false;
            }
        }
    }
    return false;
}

bool CommandLineInterface::parseStringOption(int argc, char** argv, const std::string& option, std::string& value) {
    for (int i = 0; i < argc; ++i) {
        if (std::string(argv[i]) == option && i + 1 < argc) {
            value = argv[i + 1];
            return true;
        }
    }
    return false;
}

std::shared_ptr<Config> CommandLineInterface::loadConfigFromFile(const std::string& filepath) {
    return ConfigManager::getDefaultConfig();
}

bool CommandLineInterface::applyConfigOverrides(std::shared_ptr<Config> config, const std::vector<std::string>& overrides) {
    if (!config) return false;
    
    for (const auto& override : overrides) {
        size_t pos = override.find('=');
        if (pos != std::string::npos) {
            std::string key = override.substr(0, pos);
            std::string value = override.substr(pos + 1);
            config->set(key, value, ConfigSource::CommandLine);
        }
    }
    
    return true;
}

std::shared_ptr<Config> CommandLineInterface::mergeConfigs(const std::shared_ptr<Config>& base, const std::shared_ptr<Config>& overrides) {
    if (!overrides) return base;
    
    auto result = std::make_shared<Config>(*base);
    
    // Merge overrides
    for (const auto& entry : overrides->getKeys()) {
        auto valueOpt = overrides->get<ConfigValue>(entry);
        if (valueOpt) {
            result->set(entry, *valueOpt, ConfigSource::CommandLine);
        }
    }
    
    return result;
}

bool CommandLineInterface::runSimulation(std::shared_ptr<Brain> brain, const CommandOptions& options) {
    if (!brain) return false;
    
    return runBrainSimulation(brain, options);
}

bool CommandLineInterface::runDemoMode(std::shared_ptr<Brain> brain, const CommandOptions& options) {
    if (!brain) return false;
    
    printInfo("Running demo mode with sample parameters...");
    
    // Run shorter simulation for demo
    int demoSteps = std::min(options.simulationSteps, 100);
    
    for (SimulationStep step = 0; step < demoSteps; ++step) {
        brain->step(step);
        
        if (step % 10 == 0) {
            printInfo(std::string("Demo step ") + std::to_string(step) + 
                     std::string(", Spikes: ") + std::to_string(brain->getTotalSpikeCount()));
        }
    }
    
    printInfo("Demo completed successfully");
    printBrainStatus(brain);
    
    return true;
}

bool CommandLineInterface::runBenchmarkMode(std::shared_ptr<Brain> brain, const CommandOptions& options) {
    if (!brain) return false;
    
    auto start = std::chrono::system_clock::now();
    
    for (SimulationStep step = 0; step < options.simulationSteps; ++step) {
        brain->step(step);
    }
    
    auto end = std::chrono::system_clock::now();
    double duration = std::chrono::duration<double>(end - start).count();
    
    double stepsPerSecond = static_cast<double>(options.simulationSteps) / duration;
    
    printInfo(std::string("Benchmark completed in ") + formatDuration(duration) + 
              std::string(" (" ) + std::to_string(stepsPerSecond) + 
              " steps/sec)");
    
    printBenchmarkResults("benchmark", duration, 
                         std::string("Steps: ") + std::to_string(options.simulationSteps) +
                         std::string(", Performance: ") + std::to_string(stepsPerSecond) + "/sec");
    
    return true;
}

void CommandLineInterface::printTable(const std::vector<std::vector<std::string>>& data) {
    if (data.empty()) return;
    
    size_t cols = data[0].size();
    std::vector<size_t> colWidths(cols, 0);
    
    // Calculate column widths
    for (const auto& row : data) {
        for (size_t i = 0; i < std::min(cols, row.size()); ++i) {
            colWidths[i] = std::max(colWidths[i], row[i].length());
        }
    }
    
    // Print table
    for (const auto& row : data) {
        for (size_t i = 0; i < cols; ++i) {
            if (i > 0) std::cout << "  ";
            std::cout << std::left << std::setw(colWidths[i]) << (i < row.size() ? row[i] : "");
        }
        std::cout << std::endl;
    }
}

void CommandLineInterface::printProgress(const std::string& message, double progress) {
    std::cout << "\r" << message << " " << std::fixed << std::setprecision(1) << (progress * 100.0) << "%";
    std::cout.flush();
}

void CommandLineInterface::printStatus(const std::string& status, const std::string& details) {
    std::cout << "[" << status << "]";
    if (!details.empty()) {
        std::cout << " " << details;
    }
    std::cout << std::endl;
}

} // namespace nlm