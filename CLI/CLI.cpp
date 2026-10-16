// NLM CLI Entry Point
// Implements the main CLI functionality

#include "CLI.hpp"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <ctime>
#include <iomanip>

namespace nlm {

CLI::CLI() : interactiveMode(true), scriptMode(false), autoSave(true), 
    validationMode(true) {
    parser = std::make_unique<ArgumentParser>(0, nullptr);
    configManager = std::make_unique<ConfigManager>();
    sessionManager = std::make_unique<SessionManager>();
    outputFormatter = std::make_unique<OutputFormatter>();
}

bool CLI::initialize(int argc, char** argv) {
    try {
        // Initialize argument parser
        parser = std::make_unique<ArgumentParser>(argc, argv);
        
        if (!parser->parse()) {
            return false;
        }
        
        // Check for script mode
        std::string scriptFile = parser->getArg("--script", "");
        if (!scriptFile.empty()) {
            scriptMode = true;
            return runScript(scriptFile);
        }
        
        // Setup all components
        setupAllComponents();
        
        return true;
    } catch (const std::exception& e) {
        std::cerr << "CLI initialization error: " << e.what() << std::endl;
        return false;
    }
}

void CLI::setupAllComponents() {
    // Setup configuration manager
    configManager->setAutoBackup(true, 30);
    configManager->setHistoryEnabled(true, 100);
    
    // Setup session manager
    sessionManager->setAutoCleanup(true, 100, 30);
    
    // Setup output formatter
    std::string outputFormat = parser->getArg("--output.format", "text");
    if (outputFormat == "json") {
        outputFormatter->setFormat(OutputFormat::JSON);
    } else if (outputFormat == "yaml") {
        outputFormatter->setFormat(OutputFormat::YAML);
    } else if (outputFormat == "csv") {
        outputFormatter->setFormat(OutputFormat::CSV);
    }
    
    // Setup logging level
    std::string logLevel = parser->getArg("--log.level", "info");
    
    // Load default configuration
    loadDefaultConfiguration();
    
    // Apply command line overrides
    applyConfigurationOverrides();
    
    // Validate configuration
    validationMode = parser->getBool("--validate", true);
    
    if (validationMode) {
        validateConfig();
    }
}

void CLI::loadDefaultConfiguration() {
    // Load default configuration sections
    setupBrainConfiguration();
    setupMemoryConfiguration();
    setupPlasticityConfiguration();
    setupDevelopmentConfiguration();
    setupNeuromodulationConfiguration();
    setupPerformanceConfiguration();
    setupOutputConfiguration();
    setupDebugConfiguration();
    setupLoggingConfiguration();
    setupExperimentConfiguration();
}

void CLI::applyConfigurationOverrides() {
    // Apply command line argument overrides
    auto args = parser->getAllArgs();
    
    for (const auto& [key, value] : args) {
        if (key.find(".") != std::string::npos) {
            // Handle nested configuration (e.g., brain.neuron_count)
            size_t dotPos = key.find(".");
            std::string section = key.substr(0, dotPos);
            std::string option = key.substr(dotPos + 1);
            
            if (configManager->hasSection(section)) {
                ConfigSection& section = configManager->getSection(section);
                if (value.empty()) {
                    // Flag, set to true
                    section.values[option] = true;
                } else {
                    section.values[option] = value;
                }
            }
        }
    }
}

void CLI::initializeBrain() {
    NLM_LOG_INFO("Initializing brain with configuration...");
    
    // Create brain with current configuration
    brain = std::make_shared<Brain>(config);
    
    // Initialize brain
    if (!brain->initialize()) {
        handleError("Failed to initialize brain!");
    }
    
    // Validate brain state
    brain->validateConfiguration();
    brain->validateMemoryUsage();
    brain->validateSystemState();
    
    NLM_LOG_INFO("Brain initialized successfully");
}

void CLI::runExperiments() {
    NLM_LOG_INFO("Running NLM experiments...");
    
    // Run the three tests from the original main.cpp
    logSectionHeader("TEST 1: BASIC NEURAL CONNECTIVITY");
    Timer test1Timer("Basic Connectivity Test");
    test1Timer.start();
    runBasicConnectivityTest(brain);
    test1Timer.stop();
    printTimingResults(test1Timer);
    
    // Reset brain for plasticity experiment
    brain->reset();
    initializeBrain();
    
    logSectionHeader("TEST 2: PLASTICITY LEARNING EXPERIMENT");
    Timer test2Timer("Plasticity Experiment");
    test2Timer.start();
    runPlasticityExperiment(brain);
    test2Timer.stop();
    printTimingResults(test2Timer);
    
    // Reset and run STDP verification
    brain->reset();
    initializeBrain();
    
    logSectionHeader("TEST 3: STDP VERIFICATION");
    Timer test3Timer("STDP Verification Test");
    test3Timer.start();
    runStdpVerification(brain);
    test3Timer.stop();
    printTimingResults(test3Timer);
}

void CLI::outputResults() {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== NLM Simulation Complete ===");
    
    // Output configuration summary
    if (configManager) {
        NLM_LOG_INFO("Configuration Summary:");
        NLM_LOG_INFO("  Neuron count: " + std::to_string(config->getOr<size_t>("brain.neuron_count", 0)));
        NLM_LOG_INFO("  Region count: " + std::to_string(config->getOr<size_t>("brain.region_count", 0)));
        NLM_LOG_INFO("  Simulation timestep: " + std::to_string(config->getOr<double>("simulation_timestep", 0.0)));
        NLM_LOG_INFO("  Plasticity learning rate: " + std::to_string(config->getOr<double>("plasticity.learning_rate", 0.0)));
        NLM_LOG_INFO("  Neuromodulation baseline: " + std::to_string(config->getOr<double>("neuromodulation.dopamine_baseline", 0.0)));
    }
    
    // Output brain status
    if (brain) {
        brain->logStatus();
    }
    
    // Save auto session if enabled
    if (autoSave) {
        saveAutoSession();
    }
}

void CLI::saveAutoSession() {
    std::string sessionPath = getSessionFilename("auto");
    NLM_LOG_INFO("Saving auto session to: " + sessionPath);
    saveSession(sessionPath);
}

std::string CLI::getSessionFilename(const std::string& prefix) const {
    std::time_t now = std::time(nullptr);
    char timestamp[100];
    std::strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", std::localtime(&now));
    
    return "session_" + prefix + "_" + timestamp + ".json";
}

bool CLI::run() {
    if (scriptMode) {
        return true; // Already handled in initialize
    }
    
    return runInteractive();
}

bool CLI::runScript(const std::string& scriptPath) {
    std::cout << "Running NLM in script mode from: " << scriptPath << std::endl;
    
    // In a full implementation, this would execute the script
    // For now, just load and display
    std::ifstream file(scriptPath);
    if (!file.is_open()) {
        std::cerr << "Cannot open script file: " << scriptPath << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    
    // Parse script and execute commands
    if (!parser->parse(buffer.str())) {
        const auto& errors = parser->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Script error: " << error << std::endl;
        }
        return false;
    }
    
    return runInteractive();
}

bool CLI::runInteractive() {
    std::cout << "NLM Interactive Mode" << std::endl;
    std::cout << "Type 'help' for commands, 'exit' to quit" << std::endl;
    
    // In a full implementation, this would provide an interactive shell
    // For now, just run the simulation
    
    std::cout << "Running NLM simulation..." << std::endl;
    
    // This would call the actual simulation code
    // For now, just a placeholder
    
    return true;
}

bool CLI::loadConfig(const std::string& configPath) {
    if (!configManager->loadConfig(configPath)) {
        const auto& errors = configManager->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Config error: " << error << std::endl;
        }
        return false;
    }
    return true;
}

bool CLI::saveConfig(const std::string& configPath) const {
    if (!configManager->saveConfig(configPath)) {
        const auto& errors = configManager->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Config error: " << error << std::endl;
        }
        return false;
    }
    return true;
}

bool CLI::validateConfig() {
    if (!configManager) {
        return false;
    }
    return configManager->validateConfig();
}

bool CLI::validateArguments() const {
    if (!parser || parser->hasErrors()) {
        return false;
    }
    return true;
}

bool CLI::saveSession(const std::string& sessionPath) {
    if (!sessionManager->saveSession(sessionPath)) {
        const auto& errors = sessionManager->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Session error: " << error << std::endl;
        }
        return false;
    }
    return true;
}

bool CLI::loadSession(const std::string& sessionPath) {
    if (!sessionManager->loadSession(sessionPath)) {
        const auto& errors = sessionManager->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Session error: " << error << std::endl;
        }
        return false;
    }
    return true;
}

bool CLI::compareSessions(const std::string& sessionPath1, const std::string& sessionPath2) {
    if (!sessionManager->compareSessions(sessionPath1, sessionPath2)) {
        const auto& errors = sessionManager->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Session comparison error: " << error << std::endl;
        }
        return false;
    }
    return true;
}

bool CLI::backupSession(const std::string& sessionPath) {
    if (!sessionManager->backupSession(sessionPath)) {
        const auto& errors = sessionManager->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Session backup error: " << error << std::endl;
        }
        return false;
    }
    return true;
}

bool CLI::restoreSession(const std::string& sessionPath) {
    if (!sessionManager->restoreSession(sessionPath)) {
        const auto& errors = sessionManager->getErrors();
        for (const auto& error : errors) {
            std::cerr << "Session restore error: " << error << std::endl;
        }
        return false;
    }
    return true;
}

void CLI::setOutputFormat(const std::string& format) {
    if (format == "json") {
        outputFormatter->setFormat(OutputFormat::JSON);
    } else if (format == "yaml") {
        outputFormatter->setFormat(OutputFormat::YAML);
    } else if (format == "csv") {
        outputFormatter->setFormat(OutputFormat::CSV);
    }
}

void CLI::setOutputFilter(const std::string& filter) {
    outputFormatter->setFilter(OutputFilter());
}

void CLI::setLogLevel(const std::string& level) {
    // Set log level
}

void CLI::setProgress(const std::string& title, double progress, const std::string& message) {
    outputFormatter->updateProgress(title, progress, message);
}

void CLI::finishProgress(const std::string& title) {
    outputFormatter->finishProgress(title);
}

void CLI::addBrainOptions() {
    // Add brain-related arguments
    parser->addOption("brain.neuron_count", "Number of neurons", ArgType::INTEGER, "1000");
    parser->addOption("brain.region_count", "Number of brain regions", ArgType::INTEGER, "2");
    parser->addOption("brain.connection_probability", "Connection probability", ArgType::FLOAT, "0.1");
    parser->addOption("brain.random_seed", "Random seed", ArgType::INTEGER, "42");
    parser->addOption("brain.simulation_timestep", "Simulation timestep", ArgType::FLOAT, "0.001");
}

void CLI::addMemoryOptions() {
    parser->addOption("memory.working_memory_capacity", "Working memory capacity", ArgType::INTEGER, "100");
    parser->addOption("memory.episodic_memory_capacity", "Episodic memory capacity", ArgType::INTEGER, "1000");
    parser->addOption("memory.decay_rate", "Memory decay rate", ArgType::FLOAT, "0.01");
    parser->addOption("memory.retrieval_threshold", "Retrieval threshold", ArgType::FLOAT, "0.5");
}

void CLI::addPlasticityOptions() {
    parser->addOption("plasticity.learning_rate", "Plasticity learning rate", ArgType::FLOAT, "0.01");
    parser->addOption("plasticity.stdp_ltp_weight", "STDP LTP weight", ArgType::FLOAT, "0.01");
    parser->addOption("plasticity.stdp_ltd_weight", "STDP LTD weight", ArgType::FLOAT, "0.012");
    parser->addOption("plasticity.stdp_time_constant", "STDP time constant", ArgType::FLOAT, "20.0");
    parser->addOption("plasticity.hebbian_strength", "Hebbian strength", ArgType::FLOAT, "0.005");
}

void CLI::addDevelopmentOptions() {
    parser->addOption("development.synaptogenesis_rate", "Synaptogenesis rate", ArgType::FLOAT, "0.0001");
    parser->addOption("development.pruning_rate", "Pruning rate", ArgType::FLOAT, "0.00001");
    parser->addOption("development.maturation_rate", "Maturation rate", ArgType::FLOAT, "0.001");
    parser->addOption("development.critical_period", "Critical period enabled", ArgType::BOOLEAN, "true");
}

void CLI::addNeuromodulationOptions() {
    parser->addOption("neuromodulation.dopamine_baseline", "Dopamine baseline", ArgType::FLOAT, "0.1");
    parser->addOption("neuromodulation.dopamine_gain", "Dopamine gain", ArgType::FLOAT, "1.0");
    parser->addOption("neuromodulation.curiosity_rate", "Curiosity rate", ArgType::FLOAT, "0.01");
    parser->addOption("neuromodulation.novelty_sensitivity", "Novelty sensitivity", ArgType::FLOAT, "0.5");
    parser->addOption("neuromodulation.reward_discount_factor", "Reward discount factor", ArgType::FLOAT, "0.99");
}

void CLI::addPerformanceOptions() {
    parser->addOption("performance.optimization_level", "Optimization level", ArgType::INTEGER, "2");
    parser->addOption("performance.thread_count", "Number of threads", ArgType::INTEGER, "4");
    parser->addOption("performance.vectorization_enabled", "Vectorization enabled", ArgType::BOOLEAN, "true");
    parser->addOption("performance.sparse_connectivity", "Sparse connectivity", ArgType::BOOLEAN, "true");
}

void CLI::addOutputOptions() {
    parser->addOption("output.format", "Output format", ArgType::OUTPUT_FORMAT, "text");
    parser->addOption("output.verbose", "Verbose output", ArgType::BOOLEAN, "true");
    parser->addOption("output.quiet", "Quiet mode", ArgType::BOOLEAN, "false");
    parser->addOption("output.progress_interval", "Progress interval", ArgType::INTEGER, "100");
    parser->addOption("output.log_to_file", "Log to file", ArgType::BOOLEAN, "false");
    parser->addOption("output.log_filename", "Log filename", ArgType::STRING, "nlm.log");
}

void CLI::addDebugOptions() {
    parser->addFlag("debug.mode", "Enable debug mode", false);
    parser->addOption("debug.trace_level", "Trace level", ArgType::INTEGER, "0");
    parser->addFlag("debug.validate_every_step", "Validate every step", false);
}

void CLI::addLoggingOptions() {
    parser->addOption("logging.log_level", "Log level", ArgType::LOG_LEVEL, "info");
    parser->addOption("logging.log_to_file", "Log to file", ArgType::BOOLEAN, "false");
    parser->addOption("logging.log_filename", "Log filename", ArgType::STRING, "nlm.log");
    parser->addOption("logging.log_append", "Append to log file", ArgType::BOOLEAN, "true");
}

void CLI::addExperimentOptions() {
    parser->addOption("experiment.max_simulation_steps", "Maximum simulation steps", ArgType::INTEGER, "10000");
    parser->addOption("experiment.simulation_time_limit", "Simulation time limit", ArgType::FLOAT, "0.0");
    parser->addOption("experiment.random_seed", "Random seed", ArgType::INTEGER, "42");
    parser->addOption("experiment.checkpoint_interval", "Checkpoint interval", ArgType::INTEGER, "1000");
}

void CLI::printHelp() const {
    if (parser) {
        parser->printHelp();
    }
}

void CLI::printVersion() const {
    std::cout << "NLM Version 0.1.0 - Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Built with C++20" << std::endl;
}

void CLI::printConfiguration() const {
    if (configManager) {
        configManager->listConfigs();
    }
}

void CLI::printSystemInfo() const {
    std::cout << "NLM System Information:" << std::endl;
    std::cout << "  Version: 0.1.0" << std::endl;
    std::cout << "  Phase: 2 - Real Neural Computation" << std::endl;
    std::cout << "  Compiler: C++20" << std::endl;
    std::cout << "  Build Type: Release" << std::endl;
}

bool CLI::handleError(const std::string& error) const {
    std::cerr << "Error: " << error << std::endl;
    return false;
}

void CLI::setupErrorHandling() {
    // Setup error handling
}

bool CLI::parseCommand(const std::string& commandLine) {
    // Parse and execute a command line
    // In a full implementation, this would parse commands like a shell
    return true;
}

bool CLI::executeCommand(const std::string& command, const std::map<std::string, std::string>& args) {
    // Execute a command with arguments
    // This would dispatch to the appropriate command handler
    return true;
}

bool CLI::processCLIArgs(int argc, char** argv) {
    // Process command line arguments
    // This would initialize the parser and process arguments
    return true;
}

// Session command implementations
bool CLI::sessionSaveCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for session save" << std::endl;
        return false;
    }
    
    std::string description;
    auto descIt = args.find("description");
    if (descIt != args.end()) {
        description = descIt->second;
    }
    
    // Create session state
    SessionState state;
    state.sessionId = getSessionFilename("manual");
    state.description = description;
    
    return saveSession(pathIt->second);
}

bool CLI::sessionLoadCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for session load" << std::endl;
        return false;
    }
    
    return loadSession(pathIt->second);
}

bool CLI::sessionListCommand(const std::map<std::string, std::string>& args) {
    // List available sessions
    // In a full implementation, this would list sessions from session manager
    std::cout << "Available sessions:" << std::endl;
    std::cout << "  (session listing would appear here)" << std::endl;
    return true;
}

bool CLI::sessionCompareCommand(const std::map<std::string, std::string>& args) {
    auto path1It = args.find("session1");
    auto path2It = args.find("session2");
    
    if (path1It == args.end() || path2It == args.end()) {
        std::cerr << "Error: Missing session1 or session2 arguments for session compare" << std::endl;
        return false;
    }
    
    return compareSessions(path1It->second, path2It->second);
}

bool CLI::sessionDeleteCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for session delete" << std::endl;
        return false;
    }
    
    return sessionManager->deleteSession(pathIt->second);
}

bool CLI::sessionBackupCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for session backup" << std::endl;
        return false;
    }
    
    return backupSession(pathIt->second);
}

bool CLI::sessionRestoreCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for session restore" << std::endl;
        return false;
    }
    
    return restoreSession(pathIt->second);
}

// Configuration command implementations
bool CLI::configSaveCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for config save" << std::endl;
        return false;
    }
    
    return saveConfig(pathIt->second);
}

bool CLI::configLoadCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for config load" << std::endl;
        return false;
    }
    
    return loadConfig(pathIt->second);
}

bool CLI::configValidateCommand(const std::map<std::string, std::string>& args) {
    return validateConfig();
}

bool CLI::configListCommand(const std::map<std::string, std::string>& args) {
    return configManager->listConfigs();
}

bool CLI::configMergeCommand(const std::map<std::string, std::string>& args) {
    auto sourceIt = args.find("source");
    if (sourceIt == args.end()) {
        std::cerr << "Error: Missing source argument for config merge" << std::endl;
        return false;
    }
    
    return configManager->mergeConfig(sourceIt->second);
}

bool CLI::configDiffCommand(const std::map<std::string, std::string>& args) {
    auto sourceIt = args.find("source");
    if (sourceIt == args.end()) {
        std::cerr << "Error: Missing source argument for config diff" << std::endl;
        return false;
    }
    
    return configManager->diffConfig(sourceIt->second);
}

// Output command implementations
bool CLI::outputFormatCommand(const std::map<std::string, std::string>& args) {
    auto formatIt = args.find("format");
    if (formatIt == args.end()) {
        std::cerr << "Error: Missing format argument for output format" << std::endl;
        return false;
    }
    
    setOutputFormat(formatIt->second);
    return true;
}

bool CLI::outputFilterCommand(const std::map<std::string, std::string>& args) {
    auto filterIt = args.find("filter");
    if (filterIt == args.end()) {
        std::cerr << "Error: Missing filter argument for output filter" << std::endl;
        return false;
    }
    
    setOutputFilter(filterIt->second);
    return true;
}

bool CLI::outputProgressCommand(const std::map<std::string, std::string>& args) {
    auto titleIt = args.find("title");
    auto progressIt = args.find("progress");
    
    if (titleIt == args.end() || progressIt == args.end()) {
        std::cerr << "Error: Missing title or progress argument for output progress" << std::endl;
        return false;
    }
    
    double progress = std::stod(progressIt->second);
    std::string message;
    auto messageIt = args.find("message");
    if (messageIt != args.end()) {
        message = messageIt->second;
    }
    
    setProgress(titleIt->second, progress, message);
    return true;
}

bool CLI::outputStatisticsCommand(const std::map<std::string, std::string>& args) {
    // Output statistics
    // In a full implementation, this would output statistics from output formatter
    std::cout << "Statistics output would appear here" << std::endl;
    return true;
}

bool CLI::outputSummaryCommand(const std::map<std::string, std::string>& args) {
    // Output summary
    outputResults();
    return true;
}

// Utility command implementations
bool CLI::helpCommand(const std::map<std::string, std::string>& args) {
    printHelp();
    return true;
}

bool CLI::versionCommand(const std::map<std::string, std::string>& args) {
    printVersion();
    return true;
}

bool CLI::exitCommand(const std::map<std::string, std::string>& args) {
    std::cout << "Exiting NLM..." << std::endl;
    return true;
}

bool CLI::runCommand(const std::map<std::string, std::string>& args) {
    runExperiments();
    return true;
}

bool CLI::scriptCommand(const std::map<std::string, std::string>& args) {
    auto pathIt = args.find("path");
    if (pathIt == args.end()) {
        std::cerr << "Error: Missing path argument for script command" << std::endl;
        return false;
    }
    
    return runScript(pathIt->second);
}

// Configuration group implementations
void CLI::setupBrainConfiguration(const std::string& section) {
    configManager->setupBrainConfiguration(section);
}

void CLI::setupMemoryConfiguration(const std::string& section) {
    configManager->setupMemoryConfiguration(section);
}

void CLI::setupPlasticityConfiguration(const std::string& section) {
    configManager->setupPlasticityConfiguration(section);
}

void CLI::setupDevelopmentConfiguration(const std::string& section) {
    configManager->setupDevelopmentConfiguration(section);
}

void CLI::setupNeuromodulationConfiguration(const std::string& section) {
    configManager->setupNeuromodulationConfiguration(section);
}

void CLI::setupPerformanceConfiguration(const std::string& section) {
    configManager->setupPerformanceConfiguration(section);
}

void CLI::setupOutputConfiguration(const std::string& section) {
    configManager->setupOutputConfiguration(section);
}

void CLI::setupDebugConfiguration(const std::string& section) {
    configManager->setupDebugConfiguration(section);
}

void CLI::setupLoggingConfiguration(const std::string& section) {
    configManager->setupLoggingConfiguration(section);
}

void CLI::setupExperimentConfiguration(const std::string& section) {
    configManager->setupExperimentConfiguration(section);
}

} // namespace nlm
