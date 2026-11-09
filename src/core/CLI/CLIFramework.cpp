#include "CLIFramework.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cctype>
#include <sys/resource.h>
#include <unistd.h>

namespace nlm {

struct CLIFramework::Impl {
    std::chrono::high_resolution_clock::time_point lastCPUTime;
    clock_t lastCPU;  
};

CLIFramework::CLIFramework() : pImpl(std::make_unique<Impl>()) {
    pImpl->lastCPU = clock();
    pImpl->lastCPUTime = std::chrono::high_resolution_clock::now();
}

CLIFramework::~CLIFramework() = default;

bool CLIFramework::initialize(const std::string& demoName, int argc, char** argv) {
    config.demoName = demoName;
    
    // Parse arguments
    if (!parseArguments(argc, argv)) {
        return false;
    }
    
    // Setup logging
    if (!setupLogging()) {
        return false;
    }
    
    // Load configuration
    if (!loadConfiguration()) {
        return false;
    }
    
    return true;
}

bool CLIFramework::parseArguments(int argc, char** argv) {
    if (argc < 1) return false;
    
    // Parse predefined arguments first
    if (!parsePredefinedArguments(argc, argv)) {
        return false;
    }
    
    // Parse custom arguments
    if (!parseCustomArguments(argc, argv)) {
        return false;
    }
    
    return true;
}

bool CLIFramework::parsePredefinedArguments(int argc, char** argv) {
    // Help flag
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--help" || arg == "-h" || arg == "/?" || arg == "--help-all") {
            showHelp();
            return false;
        }
        if (arg == "--version") {
            showVersion();
            return false;
        }
        if (arg == "--config" && i + 1 < argc) {
            config.configFile = argv[++i];
        }
        if (arg == "--log-file" && i + 1 < argc) {
            config.logFile = argv[++i];
        }
        if (arg == "--checkpoint" && i + 1 < argc) {
            config.checkpointPath = argv[++i];
        }
        if (arg == "--log-level" && i + 1 < argc) {
            std::string level = argv[++i];
            if (level == "debug") config.logLevel = LogLevel::Debug;
            else if (level == "info") config.logLevel = LogLevel::Info;
            else if (level == "warning") config.logLevel = LogLevel::Warning;
            else if (level == "error") config.logLevel = LogLevel::Error;
            else if (level == "critical") config.logLevel = LogLevel::Critical;
        }
        if (arg == "--no-colors") {
            config.enableColors = false;
        }
        if (arg == "--verbose") {
            config.enableVerbose = true;
        }
        if (arg == "--perf-monitor") {
            config.enablePerformanceMonitoring = true;
        }
        if (arg == "--auto-save") {
            config.autoSaveCheckpoint = true;
        }
        if (arg == "--checkpoint-interval" && i + 1 < argc) {
            config.checkpointInterval = std::stof(argv[++i]);
        }
        if (arg == "--max-memory" && i + 1 < argc) {
            config.maxMemoryUsageMB = std::stoi(argv[++i]);
        }
    }
    
    return true;
}

bool CLIFramework::parseCustomArguments(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        // Process long arguments (--key=value)
        if (arg.substr(0, 2) == "--" && arg.find('=') != std::string::npos) {
            if (!processLongArgument(arg)) {
                return false;
            }
        }
        // Process short arguments (-k value)
        else if (arg[0] == '-' && arg.size() > 1 && arg[1] != '-') {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                if (!processShortArgument(arg, argv[++i])) {
                    return false;
                }
            } else {
                showError("Missing value for option: " + arg);
                return false;
            }
        }
    }
    
    return true;
}

bool CLIFramework::processLongArgument(const std::string& arg) {
    size_t pos = arg.find('=');
    if (pos == std::string::npos) {
        showError("Invalid argument format: " + arg);
        return false;
    }
    
    std::string key = arg.substr(2, pos - 2);
    std::string value = arg.substr(pos + 1);
    
    key = trim(key);
    value = trim(value);
    
    // Store argument
    arguments[key] = value;
    
    return true;
}

bool CLIFramework::processShortArgument(const std::string& arg, const std::string& value) {
    std::string key = arg.substr(1);
    key = trim(key);
    
    // Store option
    options[key] = value;
    
    return true;
}

bool CLIFramework::setupLogging() {
    auto& logger = Logger::getGlobal();
    
    // Clear existing loggers
    logger.clearLoggers();
    
    // Add console logger
    auto consoleLogger = std::make_shared<ConsoleLogger>(config.logLevel);
    consoleLogger->setUseColors(config.enableColors);
    
    if (!config.logFile.empty()) {
        consoleLogger->setOutputFile(config.logFile);
    }
    
    logger.addLogger(consoleLogger);
    
    NLM_LOG_INFO("CLI Framework initialized for demo: " + config.demoName);
    NLM_LOG_INFO("Log level: " + std::to_string(static_cast<int>(config.logLevel)));
    
    return true;
}

bool CLIFramework::loadConfiguration() {
    nlmConfig = std::make_shared<Config>();
    
    // Load from file if specified
    if (!config.configFile.empty()) {
        if (nlmConfig->loadFromFile(config.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + config.configFile);
        } else {
            NLM_LOG_WARNING("Failed to load configuration from: " + config.configFile);
            NLM_LOG_INFO("Using default configuration.");
        }
    }
    
    // Load from command line
    int argc = 0;  // We'll need this from main
    char** argv = nullptr;  // We'll need this from main
    nlmConfig->loadFromArgs(argc, argv);
    
    return true;
}

void CLIFramework::startPerformanceMonitoring() {
    startTime = std::chrono::high_resolution_clock::now();
}

void CLIFramework::stopPerformanceMonitoring() {
    auto endTime = std::chrono::high_resolution_clock::now();
    performanceMetrics.wallClockTime = std::chrono::duration<double>(endTime - startTime).count();
}

void CLIFramework::updatePerformanceMetrics(const std::shared_ptr<Brain>& brain) {
    if (!brain) return;
    
    performanceMetrics.memoryUsageMB = getCurrentMemoryUsage();
    performanceMetrics.cpuUsage = getCurrentCPUUsage();
    performanceMetrics.totalSpikesGenerated = brain->getTotalSpikeCount();
    performanceMetrics.neuronsActive = brain->getActiveNeuronCount();
    performanceMetrics.synapsesActive = brain->getTotalSynapseCount();
    performanceMetrics.avgFiringRate = brain->getAverageFiringRate();
    
    if (performanceMetrics.wallClockTime > 0.0) {
        performanceMetrics.simulationSpeed = performanceMetrics.totalSpikesGenerated / performanceMetrics.wallClockTime;
    }
}

void CLIFramework::showHelp() const {
    std::cout << "\n";
    std::cout << "NLM Demo Framework - Command Line Interface\n";
    std::cout << "===========================================\n\n";
    
    std::cout << "Usage:\n";
    std::cout << "  ./demo_name [options] [demo_args]\n\n";
    
    std::cout << "Basic Options:\n";
    std::cout << "  --help, -h              Show this help message\n";
    std::cout << "  --version               Show version information\n";
    std::cout << "  --config <file>         Load configuration from file\n";
    std::cout << "  --log-file <file>       Log output to file\n";
    std::cout << "  --checkpoint <file>     Save/load checkpoint from file\n";
    std::cout << "  --log-level <level>     Set log level (debug, info, warning, error, critical)\n";
    std::cout << "  --no-colors             Disable colored output\n";
    std::cout << "  --verbose               Enable verbose output\n\n";
    
    std::cout << "Performance Options:\n";
    std::cout << "  --perf-monitor          Enable performance monitoring\n";
    std::cout << "  --auto-save             Auto-save checkpoints\n";
    std::cout << "  --checkpoint-interval <seconds>   Auto-save interval\n";
    std::cout << "  --max-memory <MB>       Maximum memory usage (MB)\n\n";
    
    std::cout << "Demo-specific options are passed to the demo implementation.\n";
    std::cout << "\n";
}

void CLIFramework::showVersion() const {
    std::cout << "NLM Demo Framework - Version 1.0.0\n";
    std::cout << "NLM (熙然) - Neural Learning Machine\n";
    std::cout << "Phase 6 Final Integration\n";
    std::cout << "\n";
}

void CLIFramework::showConfiguration() const {
    std::cout << "\n";
    std::cout << "Current Configuration:\n";
    std::cout << "======================\n";
    std::cout << "Demo Name: " << config.demoName << "\n";
    std::cout << "Config File: " << (config.configFile.empty() ? "Default" : config.configFile) << "\n";
    std::cout << "Log File: " << (config.logFile.empty() ? "Console" : config.logFile) << "\n";
    std::cout << "Log Level: " << static_cast<int>(config.logLevel) << "\n";
    std::cout << "Colors: " << (config.enableColors ? "Enabled" : "Disabled") << "\n";
    std::cout << "Verbose: " << (config.enableVerbose ? "Enabled" : "Disabled") << "\n";
    std::cout << "Performance Monitoring: " << (config.enablePerformanceMonitoring ? "Enabled" : "Disabled") << "\n";
    std::cout << "Auto-save Checkpoints: " << (config.autoSaveCheckpoint ? "Enabled" : "Disabled") << "\n";
    std::cout << "Checkpoint Interval: " << config.checkpointInterval << "s\n";
    std::cout << "Max Memory Usage: " << config.maxMemoryUsageMB << "MB\n";
    std::cout << "\n";
}

void CLIFramework::showPerformanceMetrics() const {
    std::cout << "\n";
    std::cout << "Performance Metrics:\n";
    std::cout << "===================\n";
    std::cout << "CPU Usage: " << std::fixed << std::setprecision(2) << performanceMetrics.cpuUsage << "%\n";
    std::cout << "Memory Usage: " << std::fixed << std::setprecision(2) << performanceMetrics.memoryUsageMB << "MB\n";
    std::cout << "Wall Clock Time: " << std::fixed << std::setprecision(3) << performanceMetrics.wallClockTime << "s\n";
    std::cout << "Total Spikes Generated: " << performanceMetrics.totalSpikesGenerated << "\n";
    std::cout << "Neurons Active: " << performanceMetrics.neuronsActive << "\n";
    std::cout << "Synapses Active: " << performanceMetrics.synapsesActive << "\n";
    std::cout << "Average Firing Rate: " << std::fixed << std::setprecision(2) << performanceMetrics.avgFiringRate << " Hz\n";
    std::cout << "Simulation Speed: " << std::fixed << std::setprecision(2) << performanceMetrics.simulationSpeed << " spikes/sec\n";
    std::cout << "\n";
}

bool CLIFramework::saveCheckpoint(const std::shared_ptr<Brain>& brain) const {
    if (!brain || config.checkpointPath.empty()) {
        return false;
    }
    
    NLM_LOG_INFO("Saving checkpoint to: " + config.checkpointPath);
    return brain->save(config.checkpointPath);
}

bool CLIFramework::loadCheckpoint(std::shared_ptr<Brain>& brain) const {
    if (!brain || config.checkpointPath.empty()) {
        return false;
    }
    
    NLM_LOG_INFO("Loading checkpoint from: " + config.checkpointPath);
    return brain->load(config.checkpointPath);
}

int CLIFramework::executeDemo(std::function<int(const CLIConfig&, const std::shared_ptr<Config>&)> demoFunction) {
    // Start performance monitoring if enabled
    if (config.enablePerformanceMonitoring) {
        startPerformanceMonitoring();
    }
    
    try {
        // Execute the demo
        int result = demoFunction(config, nlmConfig);
        
        // Stop performance monitoring
        if (config.enablePerformanceMonitoring) {
            stopPerformanceMonitoring();
            showPerformanceMetrics();
        }
        
        // Auto-save checkpoint if enabled
        if (config.autoSaveCheckpoint) {
            saveCheckpoint(brain);  // Need to get brain from somewhere
        }
        
        return result;
    } catch (const std::exception& e) {
        showError("Exception in demo: " + std::string(e.what()));
        return 1;
    }
}

void CLIFramework::registerArgument(const std::string& name, const std::string& description,
                                   bool required, const std::string& defaultValue) {
    argumentDescriptions[name] = description + (required ? " (required)" : " (optional)") + 
                                (defaultValue.empty() ? "" : " [default: " + defaultValue + "]");
}

void CLIFramework::registerOption(const std::string& name, const std::string& description,
                                 bool* flag, const std::string& defaultValue) {
    optionDescriptions[name] = description + 
                               (defaultValue.empty() ? "" : " [default: " + defaultValue + "]");
    optionFlags[name] = flag;
}

std::string CLIFramework::getArgument(const std::string& name) const {
    auto it = arguments.find(name);
    return (it != arguments.end()) ? it->second : "";
}

bool CLIFramework::getOption(const std::string& name) const {
    auto it = options.find(name);
    if (it == options.end()) {
        return false;
    }
    return parseBool(it->second);
}

int CLIFramework::getOptionInt(const std::string& name) const {
    auto it = options.find(name);
    if (it == options.end()) {
        return 0;
    }
    return std::stoi(it->second);
}

float CLIFramework::getOptionFloat(const std::string& name) const {
    auto it = options.find(name);
    if (it == options.end()) {
        return 0.0f;
    }
    return std::stof(it->second);
}

std::string CLIFramework::getOptionString(const std::string& name) const {
    auto it = options.find(name);
    return (it != options.end()) ? it->second : "";
}

void CLIFramework::showError(const std::string& message) const {
    std::cerr << "[ERROR] " << message << std::endl;
}

void CLIFramework::showWarning(const std::string& message) const {
    std::cerr << "[WARNING] " << message << std::endl;
}

bool CLIFramework::parseBool(const std::string& str) const {
    std::string lower = toLower(str);
    if (lower == "true" || lower == "yes" || lower == "1" || lower == "on") {
        return true;
    } else if (lower == "false" || lower == "no" || lower == "0" || lower == "off") {
        return false;
    }
    return false;
}

template<typename T>
T CLIFramework::parseNumber(const std::string& str) const {
    std::stringstream ss(str);
    T result;
    ss >> result;
    return result;
}

std::string CLIFramework::trim(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\r\n");
    size_t end = str.find_last_not_of(" \t\r\n");
    if (start == std::string::npos) {
        return "";
    }
    return str.substr(start, end - start + 1);
}

std::string CLIFramework::toLower(const std::string& str) const {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

double CLIFramework::getCurrentMemoryUsage() const {
#ifdef __linux__
    // Get memory usage from /proc/self/status
    std::ifstream statusFile("/proc/self/status");
    if (!statusFile.is_open()) {
        return 0.0;
    }
    
    std::string line;
    while (std::getline(statusFile, line)) {
        if (line.find("VmRSS:") != std::string::npos) {
            size_t pos = line.find(" ");
            std::string kb = line.substr(pos + 1);
            double mb = std::stod(kb) / 1024.0;
            return mb;
        }
    }
#endif
    return 0.0;
}

double CLIFramework::getCurrentCPUUsage() const {
#ifdef __linux__
    clock_t now = clock();
    double elapsedCPU = static_cast<double>(now - pImpl->lastCPU) / CLOCKS_PER_SEC;
    
    auto nowTime = std::chrono::high_resolution_clock::now();
    double elapsedTime = std::chrono a::duration<double>(nowTime - pImpl->lastCPUTime).count();
    
    if (elapsedTime > 0.0) {
        return elapsedCPU / elapsedTime *  ΙOO;
    }
#endif
    return 0.0;
}

} // namespace nlm