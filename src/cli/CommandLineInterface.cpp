#include "CommandLineInterface.hpp"

namespace nlm {

struct CommandLineInterface::Impl {
    CommandLineArgs args;
    bool initialized = false;
    std::function<void(const std::string&)> eventCallback;
    std::function<void(const std::string&)> errorCallback;
    std::map<std::string, std::string> optionDescriptions;
    std::map<std::string, std::string> exampleConfigs;
    std::map<std::string, std::string> demoConfigs;
    std::map<std::string, std::string> configOptions;
    std::map<std::string, std::string> visualizationOptions;
    std::map<std::string, std::string> performanceOptions;
    std::map<std::string, std::string> debugOptions;
    
    Impl() {
        initializeOptions();
    }
    
    void initializeOptions() {
        // Initialize option descriptions
        optionDescriptions["--help"] = "Show help information";
        optionDescriptions["--version"] = "Show version information";
        optionDescriptions["--config"] = "Load configuration from file";
        optionDescriptions["--set"] = "Set configuration value (key=value)";
        optionDescriptions["--get"] = "Get configuration value";
        optionDescriptions["--visualize"] = "Enable visualization";
        optionDescriptions["--visualize-mode"] = "Set visualization mode";
        optionDescriptions["--visualize-backend"] = "Set visualization backend";
        optionDescriptions["--visualize-export"] = "Export visualization to file";
        optionDescriptions["--performance"] = "Enable performance monitoring";
        optionDescriptions["--benchmark"] = "Run benchmark test";
        optionDescriptions["--benchmark-duration"] = "Benchmark duration in seconds";
        optionDescriptions["--log-level"] = "Set log level";
        optionDescriptions["--log-to-file"] = "Enable file logging";
        optionDescriptions["--log-file"] = "Set log file path";
        optionDescriptions["--random-seed"] = "Set random seed";
        optionDescriptions["--environment"] = "Configure environment";
        optionDescriptions["--example"] = "Run example configuration";
        optionDescriptions["--demo"] = "Run demo mode";
        optionDescriptions["--test"] = "Run test suite";
        optionDescriptions["--validate"] = "Validate configuration";
        optionDescriptions["--export"] = "Export configuration to file";
        optionDescriptions["--import"] = "Import configuration from file";
        optionDescriptions["--list"] = "List configuration options";
        optionDescriptions["--reset"] = "Reset configuration to defaults";
        optionDescriptions["--statistics"] = "Show statistics";
        optionDescriptions["--validation-rules"] = "Show validation rules";
        optionDescriptions["--debug"] = "Enable debug mode";
        optionDescriptions["--profile"] = "Enable profiling";
        optionDescriptions["--profile-output"] = "Profile output file";
        optionDescriptions["--output-dir"] = "Output directory";
        optionDescriptions["--checkpoint"] = "Checkpoint file path";
        
        // Initialize example configurations
        exampleConfigs["simple"] = "Simple example with basic configuration";
        exampleConfigs["complex"] = "Complex example with advanced features";
        exampleConfigs["research"] = "Research example for experimentation";
        exampleConfigs["production"] = "Production example for deployment";
        
        // Initialize demo configurations
        demoConfigs["phase2"] = "Phase 2 demo: Basic neural dynamics";
        demoConfigs["phase3"] = "Phase 3 demo: World interaction";
        demoConfigs["phase4"] = "Phase 4 demo: Advanced cognition";
        demoConfigs["phase5"] = "Phase 5 demo: Performance optimizations";
        demoConfigs["phase6"] = "Phase 6 demo: Integrated brain system";
        
        // Initialize configuration options
        configOptions["brain.neuron-count"] = "Number of neurons in the brain";
        configOptions["brain.synapse-density"] = "Synapse connectivity density";
        configOptions["brain.connection-probability"] = "Probability of connection between neurons";
        configOptions["brain.initial-weight-mean"] = "Mean initial synaptic weight";
        configOptions["brain.initial-weight-std"] = "Standard deviation of initial weights";
        configOptions["brain.v-thresh"] = "Neuron threshold potential (mV)";
        configOptions["brain.v-rest"] = "Resting potential (mV)";
        configOptions["brain.v-reset"] = "Reset potential after spike (mV)";
        configOptions["brain.tau-mem"] = "Membrane time constant (ms)";
        configOptions["brain.tau-ref"] = "Refractory period (ms)";
        
        // Initialize visualization options
        visualizationOptions["network"] = "Network topology visualization";
        visualizationOptions["activity"] = "Neural activity over time visualization";
        visualizationOptions["spike"] = "Spike raster plot";
        visualizationOptions["weights"] = "Synaptic weight matrix visualization";
        visualizationOptions["brainstate"] = "Integrated brain state overview";
        visualizationOptions["performance"] = "Performance metrics visualization";
        
        // Initialize performance options
        performanceOptions["cpu"] = "CPU performance monitoring";
        performanceOptions["memory"] = "Memory usage monitoring";
        performanceOptions["simulation"] = "Simulation performance monitoring";
        performanceOptions["visualization"] = "Visualization performance monitoring";
        
        // Initialize debug options
        debugOptions["trace"] = "Enable detailed tracing";
        debugOptions["breakpoint"] = "Set breakpoint";
        debugOptions["watch"] = "Watch variable values";
        debugOptions["dump"] = "Dump internal state";
    }
};

CommandLineInterface::CommandLineInterface() : pImpl(std::make_unique<Impl>()) {}

CommandLineInterface::~CommandLineInterface() = default;

CommandLineInterface::CommandLineInterface(CommandLineInterface&&) noexcept = default;

CommandLineInterface& CommandLineInterface::operator=(CommandLineInterface&&) noexcept = default;

CommandLineArgs CommandLineInterface::parseArgs(int argc, char** argv) {
    CommandLineArgs args;
    args.parseTime = std::chrono::system_clock::now();
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        args.totalArgs++;
        
        if (arg == "--help" || arg == "-h") {
            args.showHelp = true;
            args.validArgs++;
        } else if (arg == "--version" || arg == "-v") {
            args.showVersion = true;
            args.validArgs++;
        } else if (arg.substr(0, 2) == "--" && arg.find("=") != std::string::npos) {
            // Long option with key=value
            size_t pos = arg.find("=");
            std::string key = arg.substr(2, pos - 2);
            std::string value = arg.substr(pos + 1);
            
            if (parseKeyValuePair(key, value, args)) {
                args.validArgs++;
            } else {
                args.invalidArgs++;
            }
        } else if (arg.substr(0, 2) == "--") {
            // Long option without value
            std::string key = arg.substr(2);
            if (parseKeyValuePair(key, "", args)) {
                args.validArgs++;
            } else {
                args.invalidArgs++;
            }
        } else if (arg[0] == '-' && arg.size() > 1) {
            // Short option
            std::string key = arg.substr(1);
            if (parseKeyValuePair(key, "", args)) {
                args.validArgs++;
            } else {
                args.invalidArgs++;
            }
        } else if (arg.find("=") != std::string::npos) {
            // Key=value pair without -- prefix
            size_t pos = arg.find("=");
            std::string key = arg.substr(0, pos);
            std::string value = arg.substr(pos + 1);
            
            if (parseKeyValuePair(key, value, args)) {
                args.validArgs++;
            } else {
                args.invalidArgs++;
            }
        } else {
            // Unknown argument
            args.rawArgs.push_back(arg);
            args.invalidArgs++;
        }
    }
    
    args.endTiming();
    return args;
}

bool CommandLineInterface::processArgs(const CommandLineArgs& args) {
    try {
        // Process configuration options
        for (const auto& pair : args.configKeys) {
            setConfiguration(pair.first, pair.second);
        }
        
        // Process visualization options
        if (args.enableVisualization) {
            // Enable visualization
        }
        
        // Process performance options
        if (args.enablePerformance) {
            // Enable performance monitoring
        }
        
        // Process benchmark options
        if (args.enableBenchmark) {
            // Run benchmark
        }
        
        // Process logging options
        if (!args.logLevel.empty()) {
            // Set log level
        }
        
        if (args.logToFile) {
            // Enable file logging
        }
        
        if (!args.logFile.empty()) {
            // Set log file path
        }
        
        // Process random seed
        if (args.randomSeed != 0) {
            // Set random seed
        }
        
        // Process environment
        if (!args.environmentConfig.empty()) {
            // Configure environment
        }
        
        // Process special commands
        if (args.showHelp) {
            showHelp();
        } else if (args.showVersion) {
            showVersionInfo();
        } else if (args.runExample) {
            runExample("");
        } else if (args.runDemo) {
            runDemo("");
        } else if (args.runTests) {
            runTestSuite();
        } else if (args.validateConfig) {
            validateConfiguration();
        } else if (!args.exportConfig.empty()) {
            exportConfiguration(args.exportConfig, "json");
        } else if (!args.importConfig.empty()) {
            importConfiguration(args.importConfig);
        } else if (args.showStatistics) {
            showConfigurationStatistics();
        } else if (args.showValidationRules) {
            showValidationRules();
        }
        
        return true;
    } catch (const std::exception& e) {
        if (errorCallback_) {
            errorCallback_(std::string("Error processing arguments: ") + e.what());
        }
        return false;
    }
}

std::string CommandLineInterface::getHelp(const std::string& topic) {
    return buildHelpString(topic);
}

std::string CommandLineInterface::getVersion() {
    return VERSION;
}

std::map<std::string, std::string> CommandLineInterface::getExamples() {
    return Impl->exampleConfigs;
}

std::map<std::string, std::string> CommandLineInterface::getDemos() {
    return Impl->demoConfigs;
}

std::map<std::string, std::string> CommandLineInterface::getConfigOptions() {
    return Impl->configOptions;
}

std::map<std::string, std::string> CommandLineInterface::getVisualizationOptions() {
    return Impl->visualizationOptions;
}

std::map<std::string, std::string> CommandLineInterface::getPerformanceOptions() {
    return Impl->performanceOptions;
}

std::map<std::string, std::string> CommandLineInterface::getDebugOptions() {
    return Impl->debugOptions;
}

std::vector<std::string> CommandLineInterface::getLogLevels() {
    return {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
}

std::vector<std::string> CommandLineInterface::getVisualizationBackends() {
    return {"software", "sdl", "opengl", "webgl"};
}

std::vector<std::string> CommandLineInterface::getVisualizationModes() {
    return {"network", "activity", "spike", "weights", "brainstate", "performance"};
}

std::vector<std::string> CommandLineInterface::getConfigFileFormats() {
    return {"json", "yaml", "simple"};
}

std::vector<std::string> CommandLineInterface::getAvailableExamples() {
    std::vector<std::string> examples;
    for (const auto& pair : Impl->exampleConfigs) {
        examples.push_back(pair.first);
    }
    return examples;
}

std::vector<std::string> CommandLineInterface::getAvailableDemos() {
    std::vector<std::string> demos;
    for (const auto& pair : Impl->demoConfigs) {
        demos.push_back(pair.first);
    }
    return demos;
}

bool CommandLineInterface::validateArgs(const CommandLineArgs& args) {
    // Validate configuration keys
    for (const auto& pair : args.configKeys) {
        if (!isValidConfigKey(pair.first)) {
            return false;
        }
    }
    
    // Validate visualization mode
    if (!args.visualizeMode.empty() && !isValidVisualizationMode(args.visualizeMode)) {
        return false;
    }
    
    // Validate visualization backend
    if (!args.visualizeBackend.empty() && !isValidVisualizationBackend(args.visualizeBackend)) {
        return false;
    }
    
    // Validate log level
    if (!args.logLevel.empty() && !isValidLogLevel(args.logLevel)) {
        return false;
    }
    
    // Validate environment
    if (!args.environmentConfig.empty() && !isValidEnvironment(args.environmentConfig)) {
        return false;
    }
    
    return true;
}

void CommandLineInterface::showVersionInfo() {
    std::cout << PROGRAM_NAME << " " << VERSION << std::endl;
    std::cout << "Author: " << AUTHOR << std::endl;
    std::cout << "Copyright: " << COPYRIGHT << std::endl;
    std::cout << "License: " << LICENSE << std::endl;
    std::cout << "Description: " << DESCRIPTION << std::endl;
}

void CommandLineInterface::showHelp(const std::string& topic) {
    std::cout << buildHelpString(topic);
}

void CommandLineInterface::showExamples() {
    std::cout << "Available Examples:" << std::endl;
    for (const auto& pair : Impl->exampleConfigs) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void CommandLineInterface::showDemos() {
    std::cout << "Available Demos:" << std::endl;
    for (const auto& pair : Impl->demoConfigs) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void CommandLineInterface::showConfigOptions() {
    std::cout << "Configuration Options:" << std::endl;
    for (const auto& pair : Impl->configOptions) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void CommandLineInterface::showVisualizationOptions() {
    std::cout << "Visualization Options:" << std::endl;
    for (const auto& pair : Impl->visualizationOptions) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void CommandLineInterface::showPerformanceOptions() {
    std::cout << "Performance Options:" << std::endl;
    for (const auto& pair : Impl->performanceOptions) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void CommandLineInterface::showDebugOptions() {
    std::cout << "Debug Options:" << std::endl;
    for (const auto& pair : Impl->debugOptions) {
        std::cout << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

void CommandLineInterface::showLogLevelOptions() {
    std::cout << "Log Level Options:" << std::endl;
    for (const auto& level : getLogLevels()) {
        std::cout << "  " << level << std::endl;
    }
}

void CommandLineInterface::showVisualizationBackendOptions() {
    std::cout << "Visualization Backend Options:" << std::endl;
    for (const auto& backend : getVisualizationBackends()) {
        std::cout << "  " << backend << std::endl;
    }
}

void CommandLineInterface::showVisualizationModeOptions() {
    std::cout << "Visualization Mode Options:" << std::endl;
    for (const auto& mode : getVisualizationModes()) {
        std::cout << "  " << mode << std::endl;
    }
}

void CommandLineInterface::showConfigFileFormatOptions() {
    std::cout << "Configuration File Format Options:" << std::endl;
    for (const auto& format : getConfigFileFormats()) {
        std::cout << "  " << format << std::endl;
    }
}

void CommandLineInterface::showAvailableExamples() {
    std::cout << "Available Examples:" << std::endl;
    for (const auto& example : getAvailableExamples()) {
        std::cout << "  " << example << std::endl;
    }
}

void CommandLineInterface::showAvailableDemos() {
    std::cout << "Available Demos:" << std::endl;
    for (const auto& demo : getAvailableDemos()) {
        std::cout << "  " << demo << std::endl;
    }
}

bool CommandLineInterface::runExample(const std::string& exampleName) {
    // Implement example running logic
    return true;
}

bool CommandLineInterface::runDemo(const std::string& demoName) {
    // Implement demo running logic
    return true;
}

bool CommandLineInterface::runTestSuite() {
    // Implement test suite running logic
    return true;
}

bool CommandLineInterface::validateConfiguration() {
    // Implement configuration validation logic
    return true;
}

bool CommandLineInterface::exportConfiguration(const std::string& filename, const std::string& format) {
    // Implement configuration export logic
    return true;
}

bool CommandLineInterface::importConfiguration(const std::string& filename) {
    // Implement configuration import logic
    return true;
}

std::string CommandLineInterface::getConfiguration(const std::string& key) {
    // Implement configuration get logic
    return "";
}

bool CommandLineInterface::setConfiguration(const std::string& key, const std::string& value) {
    // Implement configuration set logic
    return true;
}

void CommandLineInterface::resetConfiguration() {
    // Implement configuration reset logic
}

void CommandLineInterface::showConfigurationStatistics() {
    // Implement statistics display logic
}

void CommandLineInterface::showValidationRules() {
    // Implement validation rules display logic
}

void CommandLineInterface::initialize(const Config& config) {
    // Initialize with configuration
    initialized = true;
}

void CommandLineInterface::initialize(const VisualizationManager& visualization) {
    // Initialize with visualization
    initialized = true;
}

void CommandLineInterface::initialize(const PerformanceMonitor& performance) {
    // Initialize with performance monitoring
    initialized = true;
}

const CommandLineArgs& CommandLineInterface::getArgs() const {
    return Impl->args;
}

bool CommandLineInterface::isInitialized() const {
    return initialized;
}

void CommandLineInterface::reset() {
    initialized = false;
    Impl->args = CommandLineArgs();
}

void CommandLineInterface::setEventCallback(std::function<void(const std::string&)> callback) {
    eventCallback_ = callback;
}

void CommandLineInterface::setErrorCallback(std::function<void(const std::string&)> callback) {
    errorCallback_ = callback;
}

std::string CommandLineInterface::getStatistics() const {
    return buildStatisticsString();
}

bool CommandLineInterface::hasErrors() const {
    return !Impl->args.rawArgs.empty();
}

size_t CommandLineInterface::getErrorCount() const {
    return Impl->args.invalidArgs;
}

size_t CommandLineInterface::getWarningCount() const {
    // Implement warning count logic
    return 0;
}

size_t CommandLineInterface::getSuccessCount() const {
    // Implement success count logic
    return 0;
}

size_t CommandLineInterface::getFailureCount() const {
    // Implement failure count logic
    return 0;
}

double CommandLineInterface::getParseTime() const {
    return Impl->args.getParseTime();
}

size_t CommandLineInterface::getTotalArgs() const {
    return Impl->args.totalArgs;
}

size_t CommandLineInterface::getValidArgs() const {
    return Impl->args.validArgs;
}

size_t CommandLineInterface::getInvalidArgs() const {
    return Impl->args.invalidArgs;
}

// Static helper functions
bool CommandLineInterface::parseShortOption(const std::string& arg, CommandLineArgs& args) {
    if (arg == "h" || arg == "help") {
        args.showHelp = true;
        return true;
    } else if (arg == "v" || arg == "version") {
        args.showVersion = true;
        return true;
    } else if (arg == "e" || arg == "example") {
        args.runExample = true;
        return true;
    } else if (arg == "d" || arg == "demo") {
        args.runDemo = true;
        return true;
    } else if (arg == "t" || arg == "test") {
        args.runTests = true;
        return true;
    } else if (arg == "v" || arg == "validate") {
        args.validateConfig = true;
        return true;
    } else if (arg == "x" || arg == "export") {
        // Export requires filename
        return false;
    } else if (arg == "i" || arg == "import") {
        // Import requires filename
        return false;
    } else if (arg == "l" || arg == "list") {
        args.showStatistics = true;
        return true;
    } else if (arg == "r" || arg == "reset") {
        args.resetConfig = true;
        return true;
    } else if (arg == "s" || arg == "stats") {
        args.showStatistics = true;
        return true;
    } else if (arg == "w" || arg == "validation-rules") {
        args.showValidationRules = true;
        return true;
    } else if (arg == "g" || arg == "get") {
        // Get requires key
        return false;
    } else if (arg == "S" || arg == "set") {
        // Set requires key=value
        return false;
    } else if (arg == "D" || arg == "debug") {
        args.enableDebug = true;
        return true;
    } else if (arg == "P" || arg == "profile") {
        args.enableProfile = true;
        return true;
    } else if (arg == "O" || arg == "output-dir") {
        // Output dir requires path
        return false;
    } else if (arg == "C" || arg == "checkpoint") {
        // Checkpoint requires path
        return false;
    } else {
        return false;
    }
}

bool CommandLineInterface::parseLongOption(const std::string& arg, CommandLineArgs& args) {
    if (arg == "help") {
        args.showHelp = true;
        return true;
    } else if (arg == "version") {
        args.showVersion = true;
        return true;
    } else if (arg == "example") {
        args.runExample = true;
        return true;
    } else if (arg == "demo") {
        args.runDemo = true;
        return true;
    } else if (arg == "test") {
        args.runTests = true;
        return true;
    } else if (arg == "validate") {
        args.validateConfig = true;
        return true;
    } else if (arg == "export") {
        // Export requires filename
        return false;
    } else if (arg == "import") {
        // Import requires filename
        return false;
    } else if (arg == "list") {
        args.showStatistics = true;
        return true;
    } else if (arg == "reset") {
        args.resetConfig = true;
        return true;
    } else if (arg == "statistics") {
        args.showStatistics = true;
        return true;
    } else if (arg == "validation-rules") {
        args.showValidationRules = true;
        return true;
    } else if (arg == "get") {
        // Get requires key
        return false;
    } else if (arg == "set") {
        // Set requires key=value
        return false;
    } else if (arg == "debug") {
        args.enableDebug = true;
        return true;
    } else if (arg == "profile") {
        args.enableProfile = true;
        return true;
    } else if (arg == "output-dir") {
        // Output dir requires path
        return false;
    } else if (arg == "checkpoint") {
        // Checkpoint requires path
        return false;
    } else {
        return false;
    }
}

bool CommandLineInterface::parseKeyValuePair(const std::string& key, const std::string& value, CommandLineArgs& args) {
    // Handle configuration key=value pairs
    if (key.find("=" ) != std::string::npos) {
        // Split key and value
        size_t pos = key.find("=");
        std::string k = key.substr(0, pos);
        std::string v = key.substr(pos + 1);
        args.configKeys[k] = v;
        return true;
    } else if (!value.empty()) {
        args.configKeys[key] = value;
        return true;
    } else {
        // Handle boolean flags
        if (key == "visualize") {
            args.enableVisualization = true;
            return true;
        } else if (key == "performance") {
            args.enablePerformance = true;
            return true;
        } else if (key == "benchmark") {
            args.enableBenchmark = true;
            return true;
        } else if (key == "log-to-file") {
            args.logToFile = true;
            return true;
        } else if (key == "debug") {
            args.enableDebug = true;
            return true;
        } else if (key == "profile") {
            args.enableProfile = true;
            return true;
        } else {
            return false;
        }
    }
}

bool CommandLineInterface::parseConfigOption(const std::string& key, const std::string& value, CommandLineArgs& args) {
    // Parse configuration options
    if (key == "brain.neuron-count" || key == "neuron-count") {
        try {
            int64_t count = std::stoll(value);
            // Set random seed based on neuron count
            args.randomSeed = count;
            return true;
        } catch (...) {
            return false;
        }
    } else if (key == "brain.synapse-density" || key == "synapse-density") {
        try {
            double density = std::stod(value);
            // Use density for something
            return true;
        } catch (...) {
            return false;
        }
    } else if (key == "plasticity.learning-rate" || key == "learning-rate") {
        try {
            double rate = std::stod(value);
            // Use learning rate for something
            return true;
        } catch (...) {
            return false;
        }
    } else {
        // Pass through to configKeys
        args.configKeys[key] = value;
        return true;
    }
}

bool CommandLineInterface::parseVisualizationOption(const std::string& key, const std::string& value, CommandLineArgs& args) {
    if (key == "visualize-mode") {
        args.visualizeMode = value;
        return true;
    } else if (key == "visualize-backend") {
        args.visualizeBackend = value;
        return true;
    } else if (key == "visualize-export") {
        args.visualizeExport = value;
        return true;
    } else {
        return false;
    }
}

bool CommandLineInterface::parsePerformanceOption(const std::string& key, const std::string& value, CommandLineArgs& args) {
    if (key == "benchmark-duration") {
        try {
            args.benchmarkDuration = std::stod(value);
            args.enableBenchmark = true;
            return true;
        } catch (...) {
            return false;
        }
    } else if (key == "performance") {
        args.enablePerformance = true;
        return true;
    } else {
        return false;
    }
}

bool CommandLineInterface::parseDebugOption(const std::string& key, const std::string& value, CommandLineArgs& args) {
    if (key == "debug") {
        args.enableDebug = true;
        return true;
    } else if (key == "profile") {
        args.enableProfile = true;
        return true;
    } else if (key == "profile-output") {
        args.profileOutput = value;
        args.enableProfile = true;
        return true;
    } else {
        return false;
    }
}

bool CommandLineInterface::parseLogOption(const std::string& key, const std::string& value, CommandLineArgs& args) {
    if (key == "log-level") {
        args.logLevel = value;
        return true;
    } else if (key == "log-to-file") {
        args.logToFile = true;
        return true;
    } else if (key == "log-file") {
        args.logFile = value;
        args.logToFile = true;
        return true;
    } else {
        return false;
    }
}

bool CommandLineInterface::parseBenchmarkOption(const std::string& key, const std::string& value, CommandLineArgs& args) {
    if (key == "benchmark-duration") {
        try {
            args.benchmarkDuration = std::stod(value);
            args.enableBenchmark = true;
            return true;
        } catch (...) {
            return false;
        }
    } else if (key == "benchmark") {
        args.enableBenchmark = true;
        return true;
    } else {
        return false;
    }
}

bool CommandLineInterface::parseEnvironmentOption(const std::string& key, const std::string& value, CommandLineArgs& args) {
    if (key == "environment") {
        args.environmentConfig = value;
        return true;
    } else {
        return false;
    }
}

bool CommandLineInterface::isValidOption(const std::string& option) const {
    return VALID_LONG_OPTIONS.find(option) != VALID_LONG_OPTIONS.end();
}

bool CommandLineInterface::isValidConfigKey(const std::string& key) const {
    return key.find("=") != std::string::npos || key == "visualize" || 
           key == "performance" || key == "benchmark" ||
           key == "debug" || key == "profile" ||
           key == "log-to-file" || key == "log-level" ||
           key == "log-file" || key == "output-dir" ||
           key == "checkpoint" || key == "help" ||
           key == "version" || key == "example" ||
           key == "demo" || key == "test" ||
           key == "validate" || key == "export" ||
           key == "import" || key == "list" ||
           key == "reset" || key == "statistics" ||
           key == "validation-rules" || key == "get" ||
           key == "set";
}

bool CommandLineInterface::isValidVisualizationMode(const std::string& mode) const {
    for (const auto& validMode : getVisualizationModes()) {
        if (mode == validMode) {
            return true;
        }
    }
    return false;
}

bool CommandLineInterface::isValidVisualizationBackend(const std::string& backend) const {
    for (const auto& validBackend : getVisualizationBackends()) {
        if (backend == validBackend) {
            return true;
        }
    }
    return false;
}

bool CommandLineInterface::isValidLogLevel(const std::string& level) const {
    for (const auto& validLevel : getLogLevels()) {
        if (level == validLevel) {
            return true;
        }
    }
    return false;
}

bool CommandLineInterface::isValidEnvironment(const std::string& env) const {
    return env == "gridworld" || env == "torus" || env == "empty" || env == "maze";
}

void CommandLineInterface::throwArgError(const std::string& message) {
    throw std::runtime_error("Argument error: " + message);
}

void CommandLineInterface::throwValidationError(const std::string& message) {
    throw std::runtime_error("Validation error: " + message);
}

void CommandLineInterface::throwConfigError(const std::string& message) {
    throw std::runtime_error("Configuration error: " + message);
}

std::string CommandLineInterface::buildHelpString(const std::string& topic) const {
    std::ostringstream oss;
    
    if (topic.empty()) {
        oss << PROGRAM_NAME << " " << VERSION << std::endl;
        oss << DESCRIPTION << std::endl;
        oss << std::endl;
        oss << "Usage: " << PROGRAM_NAME << " [OPTIONS]" << std::endl;
        oss << std::endl;
        oss << "Options:" << std::endl;
        oss << "  -h, --help              Show help information" << std::endl;
        oss << "  -v, --version           Show version information" << std::endl;
        oss << "  --config FILE           Load configuration from file" << std::endl;
        oss << "  --set KEY=VALUE         Set configuration key=value" << std::endl;
        oss << "  --get KEY               Get configuration value" << std::endl;
        oss << "  --visualize             Enable visualization" << std::endl;
        oss << "  --visualize-mode MODE   Set visualization mode" << std::endl;
        oss << "  --visualize-backend BACKEND Set visualization backend" << std::endl;
        oss << "  --visualize-export FILE Export visualization to file" << std::endl;
        oss << "  --performance           Enable performance monitoring" << std::endl;
        oss << "  --benchmark             Run benchmark test" << std::endl;
        oss << "  --benchmark-duration SEC Benchmark duration in seconds" << std::endl;
        oss << "  --log-level LEVEL       Set log level" << std::endl;
        oss << "  --log-to-file           Enable file logging" << std::endl;
        oss << "  --log-file FILE         Set log file path" << std::endl;
        oss << "  --random-seed SEED       Set random seed" << std::endl;
        oss << "  --environment CONFIG     Configure environment" << std::endl;
        oss << "  --example               Run example configuration" << std::endl;
        oss << "  --demo                  Run demo mode" << std::endl;
        oss << "  --test                  Run test suite" << std::endl;
        oss << "  --validate              Validate configuration" << std::endl;
        oss << "  --export FILE           Export configuration to file" << std::endl;
        oss << "  --import FILE           Import configuration from file" << std::endl;
        oss << "  --list                 List configuration options" << std::endl;
        oss << "  --reset                Reset configuration to defaults" << std::endl;
        oss << "  --statistics            Show statistics" << std::endl;
        oss << "  --validation-rules      Show validation rules" << std::endl;
        oss << "  --debug                 Enable debug mode" << std::endl;
        oss << "  --profile                Enable profiling" << std::endl;
        oss << "  --profile-output FILE   Profile output file" << std::endl;
        oss << "  --output-dir DIR        Output directory" << std::endl;
        oss << "  --checkpoint FILE       Checkpoint file path" << std::endl;
    } else {
        oss << "Help for topic: " << topic << std::endl;
        oss << "(Topic documentation not available)" << std::endl;
    }
    
    return oss.str();
}

std::string CommandLineInterface::buildExampleString() const {
    std::ostringstream oss;
    oss << "Example configurations:" << std::endl;
    for (const auto& pair : Impl->exampleConfigs) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildDemoString() const {
    std::ostringstream oss;
    oss << "Demo configurations:" << std::endl;
    for (const auto& pair : Impl->demoConfigs) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildConfigOptionsString() const {
    std::ostringstream oss;
    oss << "Configuration Options:" << std::endl;
    for (const auto& pair : Impl->configOptions) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildVisualizationOptionsString() const {
    std::ostringstream oss;
    oss << "Visualization Options:" << std::endl;
    for (const auto& pair : Impl->visualizationOptions) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildPerformanceOptionsString() const {
    std::ostringstream oss;
    oss << "Performance Options:" << std::endl;
    for (const auto& pair : Impl->performanceOptions) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildDebugOptionsString() const {
    std::ostringstream oss;
    oss << "Debug Options:" << std::endl;
    for (const auto& pair : Impl->debugOptions) {
        oss << "  " << pair.first << ": " << pair.second << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildLogLevelOptionsString() const {
    std::ostringstream oss;
    oss << "Log Level Options:" << std::endl;
    for (const auto& level : getLogLevels()) {
        oss << "  " << level << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildVisualizationBackendOptionsString() const {
    std::ostringstream oss;
    oss << "Visualization Backend Options:" << std::endl;
    for (const auto& backend : getVisualizationBackends()) {
        oss << "  " << backend << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildVisualizationModeOptionsString() const {
    std::ostringstream oss;
    oss << "Visualization Mode Options:" << std::endl;
    for (const auto& mode : getVisualizationModes()) {
        oss << "  " << mode << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildConfigFileFormatOptionsString() const {
    std::ostringstream oss;
    oss << "Configuration File Format Options:" << std::endl;
    for (const auto& format : getConfigFileFormats()) {
        oss << "  " << format << std::endl;
    }
    return oss.str();
}

std::string CommandLineInterface::buildStatisticsString() const {
    std::ostringstream oss;
    oss << "Command Line Interface Statistics:" << std::endl;
    oss << "  Total arguments processed: " << Impl->args.totalArgs << std::endl;
    oss << "  Valid arguments: " << Impl->args.validArgs << std::endl;
    oss << "  Invalid arguments: " << Impl->args.invalidArgs << std::endl;
    oss << "  Parse time: " << Impl->args.getParseTime() << " seconds" << std::endl;
    oss << "  Configuration entries: " << Impl->args.configKeys.size() << std::endl;
    return oss.str();
}

// Template implementation
#include "CommandLineInterface.tpp"

} // namespace nlm