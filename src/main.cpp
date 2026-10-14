NLM (熙然) - Neural Learning Machine
// Comprehensive CLI Interface and Multi-Mode Neural Simulation System
// Phase 2+: Enhanced Real Neural Computation with Advanced Features

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Experiment.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <fstream>
#include <sstream>
#include <chrono>
#include <ctime>
#include <map>
#include <algorithm>
#include <filesystem>
#include <thread>
#include <atomic>
#include <cstdint>
#include <cmath>
#include <limits>
#include <stdexcept>
#include <variant>
#include <optional>
#include <cstdlib>
#include <csignal>
#include <iomanip>

namespace fs = std::filesystem;

using namespace nlm;
using namespace std::chrono;

// =============================================================================
// Command Line Argument Parsing
// =============================================================================

struct CommandLineOptions {
    std::string configFile = "configs/default.cfg";
    std::string outputFile;
    std::string logFile;
    std::string jsonOutputFile;
    std::string csvOutputFile;
    std::string summaryFile;
    
    // Execution modes
    bool help = false;
    bool version = false;
    bool listModes = false;
    std::string mode = "test"; // test, simulate, experiment, benchmark, analysis, development
    
    // Verbosity and logging
    bool verbose = false;
    bool quiet = false;
    int logLevel = 1; // 0=debug, 1=info, 2=warning, 3=error
    bool progress = false;
    bool noColors = false;
    bool debug = false;
    
    // Simulation parameters
    size_t neuronCount = 500;
    size_t regionCount = 1;
    size_t maxSteps = 10000;
    double timeStep = 0.001;
    double timeLimit = 0.0;
    uint64_t randomSeed = 42;
    
    // Output and formatting
    bool jsonOutput = false;
    bool csvOutput = false;
    bool summary = false;
    bool benchmark = false;
    bool detailed = false;
    
    // Parallel and performance
    bool parallel = false;
    size_t threads = std::thread::hardware_concurrency();
    bool profiling = false;
    bool timing = true;
    
    // Plugin-like architecture
    std::vector<std::string> enabledTests;
    std::vector<std::string> disabledTests;
    std::vector<std::string> customSequence;
    
    // Runtime modification
    bool reloadConfig = false;
    std::vector<std::string> runtimeSettings;
    
    // Error handling
    bool failFast = false;
    std::string errorLogFile;
    
    // Development and debugging
    bool debugMode = false;
    bool validateConfig = false;
    bool saveState = false;
    std::string stateFile;
};

class ArgumentParser {
public:
    static CommandLineOptions parse(int argc, char** argv) {
        CommandLineOptions options;
        
        // Default configuration file paths
        std::vector<std::string> defaultConfigPaths = {
            "configs/default.cfg",
            "configs/user.cfg",
            "default.cfg"
        };
        
        // Check for help and version flags first
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--help" || arg == "-h") {
                options.help = true;
            } else if (arg == "--version") {
                options.version = true;
            } else if (arg == "--list-modes") {
                options.listModes = true;
            }
        }
        
        // Parse all arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --key=value format
            if (arg.find('=') != std::string::npos && arg.find('--') == 0) {
                size_t pos = arg.find('=');
                std::string key = arg.substr(2, pos - 2);
                std::string value = arg.substr(pos + 1);
                setOption(options, key, value);
            }
            // Handle --key value format
            else if (arg.find('--') == 0 && (i + 1 < argc)) {
                std::string key = arg.substr(2);
                std::string value = argv[++i];
                setOption(options, key, value);
            }
            // Handle -key format (short options)
            else if (arg.find('-') == 0 && arg.size() > 1 && 
                    arg[1] != '-' && arg.size() > 2) {
                std::string key = arg.substr(1);
                setShortOption(options, key);
            }
            // Handle config file
            else if (arg.find("--config") == 0) {
                if (arg.find('=') != std::string::npos) {
                    options.configFile = arg.substr(arg.find('=') + 1);
                } else if (i + 1 < argc) {
                    options.configFile = argv[++i];
                }
            }
        }
        
        // Check for default config files if specified config doesn't exist
        if (!fs::exists(options.configFile)) {
            for (const auto& path : defaultConfigPaths) {
                if (fs::exists(path)) {
                    options.configFile = path;
                    break;
                }
            }
        }
        
        return options;
    }
    
private:
    static void setOption(CommandLineOptions& options, const std::string& key, 
                         const std::string& value) {
        if (key == "mode") {
            options.mode = value;
        } else if (key == "neuron-count" || key == "neuron_count") {
            options.neuronCount = std::stoull(value);
        } else if (key == "region-count" || key == "region_count") {
            options.regionCount = std::stoull(value);
        } else if (key == "max-steps" || key == "max_steps") {
            options.maxSteps = std::stoull(value);
        } else if (key == "time-step" || key == "time_step") {
            options.timeStep = std::stod(value);
        } else if (key == "time-limit" || key == "time_limit") {
            options.timeLimit = std::stod(value);
        } else if (key == "random-seed" || key == "random_seed") {
            options.randomSeed = std::stoull(value);
        } else if (key == "output") {
            options.outputFile = value;
        } else if (key == "log-file" || key == "log_file") {
            options.logFile = value;
        } else if (key == "json-output" || key == "json_output") {
            options.jsonOutputFile = value;
        } else if (key == "csv-output" || key == "csv_output") {
            options.csvOutputFile = value;
        } else if (key == "summary-file" || key == "summary_file") {
            options.summaryFile = value;
        } else if (key == "log-level" || key == "log_level") {
            options.logLevel = std::stoi(value);
        } else if (key == "enabled-tests" || key == "enabled_tests") {
            options.enabledTests = split(value, ',');
        } else if (key == "disabled-tests" || key == "disabled_tests") {
            options.disabledTests = split(value, ',');
        } else if (key == "custom-sequence" || key == "custom_sequence") {
            options.customSequence = split(value, ',');
        } else if (key == "threads") {
            options.threads = std::stoull(value);
        } else if (key == "error-log") {
            options.errorLogFile = value;
        } else if (key == "state-file" || key == "state_file") {
            options.stateFile = value;
        }
    }
    
    static void setShortOption(CommandLineOptions& options, const std::string& key) {
        if (key == "v") {
            options.verbose = true;
        } else if (key == "q") {
            options.quiet = true;
        } else if (key == "p") {
            options.progress = true;
        } else if (key == "j") {
            options.jsonOutput = true;
        } else if (key == "c") {
            options.benchmark = true;
        } else if (key == "d") {
            options.debug = true;
        }
    }
    
    static std::vector<std::string> split(const std::string& str, char delimiter) {
        std::vector<std::string> result;
        std::stringstream ss(str);
        std::string item;
        while (std::getline(ss, item, delimiter)) {
            if (!item.empty()) {
                result.push_back(item);
            }
        }
        return result;
    }
};

// =============================================================================
// Error Handling and Logging
// =============================================================================

enum class ExitCode {
    Success = 0,
    GeneralError = 1,
    ValidationError = 2,
    ConfigError = 3,
    InitializationError = 4,
    RuntimeError = 5,
    BenchmarkError = 6
};

class ErrorHandler {
public:
    static void logError(const std::string& message, ExitCode code = ExitCode::GeneralError,
                        const std::string& context = "") {
        std::cerr << "[ERROR] " << message;
        if (!context.empty()) {
            std::cerr << " (Context: " << context << ")";
        }
        std::cerr << std::endl;
        
        if (errorLogFile.empty()) {
            errorLogFile = "nlm_error.log";
        }
        std::ofstream errorFile(errorLogFile, std::ios::app);
        if (errorFile.is_open()) {
            auto now = system_clock::now();
            auto time = system_clock::to_time_t(now);
            errorFile << "[" << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S") << "] "
                     << message;
            if (!context.empty()) errorFile << " (Context: " << context << ")";
            errorFile << std::endl;
        }
    }
    
    static void logWarning(const std::string& message, const std::string& context = "") {
        std::cerr << "[WARNING] " << message;
        if (!context.empty()) {
            std::cerr << " (Context: " << context << ")";
        }
        std::cerr << std::endl;
    }
    
    static void logInfo(const std::string& message, const std::string& context = "") {
        if (!quiet) {
            std::cout << "[INFO] " << message;
            if (!context.empty()) {
                std::cout << " (Context: " << context << ")";
            }
            std::cout << std::endl;
        }
    }
    
    static void logDebug(const std::string& message, const std::string& context = "") {
        if (debugEnabled) {
            std::cout << "[DEBUG] " << message;
            if (!context.empty()) {
                std::cout << " (Context: " << context << ")";
            }
            std::cout << std::endl;
        }
    }
    
    static void setDebugEnabled(bool enabled) {
        debugEnabled = enabled;
    }
    
    static void setErrorLogFile(const std::string& filename) {
        errorLogFile = filename;
    }
    
private:
    static bool debugEnabled;
    static std::string errorLogFile;
};

bool ErrorHandler::debugEnabled = false;
std::string ErrorHandler::errorLogFile = "";

// =============================================================================
// Configuration System
// =============================================================================

class ConfigurationManager {
public:
    static bool loadConfiguration(const std::string& configFile, std::shared_ptr<Config>& config) {
        if (!config) {
            config = std::make_shared<Config>();
        }
        
        // Clear existing configuration
        config->clear();
        
        // Load from file
        bool loaded = config->loadFromFile(configFile);
        if (!loaded) {
            ErrorHandler::logWarning("Failed to load configuration from file: " + configFile,
                                    "ConfigurationManager::loadConfiguration");
        }
        
        // Apply default values
        applyDefaults(config);
        
        return loaded;
    }
    
    static void applyDefaults(std::shared_ptr<Config>& config) {
        // Phase 2 specific defaults
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // STDP parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Structural plasticity
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    }
    
    static bool validateConfiguration(const std::shared_ptr<Config>& config) {
        bool valid = true;
        
        // Validate neuron count
        auto neuronCount = config->getOr<int64_t>("neuron_count", 500);
        if (neuronCount <= 0 || neuronCount > 1000000) {
            ErrorHandler::logError("Invalid neuron count: " + std::to_string(neuronCount),
                                 ExitCode::ValidationError, "validateConfiguration");
            valid = false;
        }
        
        // Validate region count
        auto regionCount = config->getOr<int64_t>("region_count", 1);
        if (regionCount <= 0 || regionCount > 100) {
            ErrorHandler::logError("Invalid region count: " + std::to_string(regionCount),
                                 ExitCode::ValidationError, "validateConfiguration");
            valid = false;
        }
        
        // Validate time step
        auto timeStep = config->getOr<double>("simulation_timestep", 0.001);
        if (timeStep <= 0.0 || timeStep > 1.0) {
            ErrorHandler::logError("Invalid time step: " + std::to_string(timeStep),
                                 ExitCode::ValidationError, "validateConfiguration");
            valid = false;
        }
        
        return valid;
    }
    
    static void printConfigurationSummary(const std::shared_ptr<Config>& config) {
        ErrorHandler::logInfo("Configuration Summary:", "ConfigurationManager");
        std::cout << config->summary() << std::endl;
    }
};

// =============================================================================
// Performance Monitoring
// =============================================================================

class PerformanceMonitor {
public:
    static void startTiming() {
        startTime = high_resolution_clock::now();
    }
    
    static void stopTiming() {
        auto endTime = high_resolution_clock::now();
        auto duration = duration_cast<milliseconds>(endTime - startTime);
        totalTimeMs = duration.count();
    }
    
    static void startTestTiming(const std::string& testName) {
        testStartTimes[testName] = high_resolution_clock::now();
        currentTest = testName;
    }
    
    static void stopTestTiming(const std::string& testName) {
        if (testStartTimes.find(testName) != testStartTimes.end()) {
            auto endTime = high_resolution_clock::now();
            auto duration = duration_cast<milliseconds>(endTime - testStartTimes[testName]);
            testTimes[testName] = duration.count();
            testStartTimes.erase(testName);
        }
    }
    
    static double getTotalTime() { return totalTimeMs / 1000.0; }
    
    static std::map<std::string, double> getTestTimes() {
        std::map<std::string, double> testTimesSec;
        for (const auto& pair : testTimes) {
            testTimesSec[pair.first] = pair.second / 1000.0;
        }
        return testTimesSec;
    }
    
    static void printPerformanceReport() {
        ErrorHandler::logInfo("=== Performance Report ===", "PerformanceMonitor");
        ErrorHandler::logInfo("Total execution time: " + std::to_string(getTotalTime()) + " seconds",
                            "PerformanceMonitor");
        
        ErrorHandler::logInfo("Per-test timing:", "PerformanceMonitor");
        for (const auto& pair : testTimes) {
            ErrorHandler::logInfo("  " + pair.first + ": " + std::to_string(pair.second / 1000.0) + "s",
                                "PerformanceMonitor");
        }
    }
    
private:
    static high_resolution_clock::time_point startTime;
    static double totalTimeMs;
    static std::map<std::string, high_resolution_clock::time_point> testStartTimes;
    static std::map<std::string, long long> testTimes;
    static std::string currentTest;
};

high_resolution_clock::time_point PerformanceMonitor::startTime;
double PerformanceMonitor::totalTimeMs = 0.0;
std::map<std::string, high_resolution_clock::time_point> PerformanceMonitor::testStartTimes;
std::map<std::string, long long> PerformanceMonitor::testTimes;
std::string PerformanceMonitor::currentTest = "";

// =============================================================================
// Output Formatters
// =============================================================================

class OutputFormatter {
public:
    static void printJSON(const std::string& filename, const std::map<std::string, std::string>& data) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            ErrorHandler::logError("Failed to open JSON output file: " + filename,
                                 ExitCode::GeneralError, "OutputFormatter::printJSON");
            return;
        }
        
        file << "{\n";
        bool first = true;
        for (const auto& pair : data) {
            if (!first) file << ",\n";
            file << "  \"" << pair.first << "\": \"" << pair.second << "\"";
            first = false;
        }
        file << "\n}\n";
        file.close();
        ErrorHandler::logInfo("JSON output written to: " + filename, "OutputFormatter");
    }
    
    static void printCSV(const std::string& filename, const std::vector<std::vector<std::string>>& data) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            ErrorHandler::logError("Failed to open CSV output file: " + filename,
                                 ExitCode::GeneralError, "OutputFormatter::printCSV");
            return;
        }
        
        for (size_t i = 0; i < data.size(); ++i) {
            for (size_t j = 0; j < data[i].size(); ++j) {
                file << data[i][j];
                if (j < data[i].size() - 1) file << ",";
            }
            if (i < data.size() - 1) file << "\n";
        }
        file.close();
        ErrorHandler::logInfo("CSV output written to: " + filename, "OutputFormatter");
    }
    
    static void printProgress(const std::string& message, int percentage) {
        if (!showProgress) return;
        
        std::cout << "\r[";
        int barWidth = 50;
        int pos = barWidth * percentage / 100;
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "=";
            else if (i == pos) std::cout << ">";
            else std::cout << " ";
        }
        std::cout << "] " << percentage << "% " << message;
        std::cout.flush();
        
        if (percentage >= 100) {
            std::cout << std::endl;
        }
    }
    
    static void printBanner() {
        std::cout << R"(
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     Phase 2+: Real Neural Computation                         ║
    ║                                                               ║
    ╚═════════════════════════════════════════════════════════════╝
    )" << std::endl;
    }
    
    static void printHelp(const CommandLineOptions& options) {
        printBanner();
        std::cout << "\n";
        std::cout << "USAGE: nlm [OPTIONS]\n\n";
        std::cout << "Core Options:\n";
        std::cout << "  -h, --help                    Show this help message and exit\n";
        std::cout << "  --version                     Show version information and exit\n";
        std::cout << "  --list-modes                  List all available execution modes\n\n";
        std::cout << "Execution Modes:\n";
        std::cout << "  --mode=MODE                   Set execution mode (default: test)\n";
        std::cout << "  Modes: test, simulate, experiment, benchmark, analysis, development, full-integration\n\n";
        std::cout << "Configuration:\n";
        std::cout << "  --config=FILE                 Load configuration from FILE (default: configs/default.cfg)\n";
        std::cout << "  --neuron-count=N              Set number of neurons (default: 500)\n";
        std::cout << "  --region-count=N              Set number of regions (default: 1)\n";
        std::cout << "  --max-steps=N                 Set maximum simulation steps (default: 10000)\n";
        std::cout << "  --time-step=SEC               Set simulation timestep (default: 0.001)\n";
        std::cout << "  --time-limit=SEC              Set simulation time limit (default: unlimited)\n";
        std::cout << "  --random-seed=N               Set random seed (default: 42)\n\n";
        std::cout << "Logging and Verbosity:\n";
        std::cout << "  -v, --verbose                 Enable verbose logging\n";
        std::cout << "  -q, --quiet                   Suppress non-error output\n";
        std::cout << "  --log-level=LEVEL             Set log level (0=debug, 1=info, 2=warning, 3=error)\n";
        std::cout << "  --progress                    Show progress indicators\n";
        std::cout << "  --log-file=FILE               Log output to FILE\n";
        std::cout << "  --no-colors                   Disable colored output\n\n";
        std::cout << "Output:\n";
        std::cout << "  --output=FILE                 Save brain state to FILE\n";
        std::cout << "  --json-output=FILE            Export results as JSON to FILE\n";
        std::cout << "  --csv-output=FILE             Export results as CSV to FILE\n";
        std::cout << "  --summary=FILE                Save summary report to FILE\n";
        std::cout << "  --benchmark                   Run in benchmark mode\n";
        std::cout << "  --detailed                    Enable detailed output\n\n";
        std::cout << "Parallel and Performance:\n";
        std::cout << "  --parallel                    Enable parallel execution\n";
        std::cout << "  --threads=N                   Set number of threads (default: CPU cores)\n";
        std::cout << "  --profiling                   Enable profiling\n";
        std::cout << "  --timing                      Enable detailed timing\n\n";
        std::cout << "Plugin System:\n";
        std::cout << "  --enabled-tests=TEST1,TEST2     Enable specific tests\n";
        std::cout << "  --disabled-tests=TEST1,TEST2   Disable specific tests\n";
        std::cout << "  --custom-sequence=TEST1,TEST2  Run custom test sequence\n\n";
        std::cout << "Error Handling:\n";
        std::cout << "  --fail-fast                   Stop on first error\n";
        std::cout << "  --error-log=FILE              Log all errors to FILE\n\n";
        std::cout << "Development and Debugging:\n";
        std::cout << "  -d, --debug                   Enable debug mode\n";
        std::cout << "  --validate-config             Validate configuration before execution\n";
        std::cout << "  --save-state=FILE             Save brain state to FILE\n";
        std::cout << "  --reload-config               Reload configuration from file\n\n";
        std::cout << "Example usage:\n";
        std::cout << "  nlm --mode=simulate --neuron-count=1000 --verbose\n";
        std::cout << "  nlm --mode=experiment --config=my_config.cfg --json-output=results.json\n";
        std::cout << "  nlm --mode=benchmark --threads=4 --profiling\n";
    }
    
    static void printModesList() {
        std::cout << "\nAvailable Execution Modes:\n\n";
        
        std::cout << "1. test\n";
        std::cout << "   Basic connectivity and functionality test\n";
        std::cout << "   Runs minimal simulations to verify core functionality\n\n";
        
        std::cout << "2. simulate\n";
        std::cout << "   Standard simulation mode with neural dynamics\n";
        std::cout << "   Includes plasticity, development, and neuromodulation\n\n";
        
        std::cout << "3. experiment\n";
        std::cout << "   Advanced learning experiments\n";
        std::cout << "   Includes STDP verification, plasticity learning, and memory tests\n\n";
        
        std::cout << "4. benchmark\n";
        std::cout << "   Performance benchmarking\n";
        std::cout << "   Measures simulation speed for different network sizes\n\n";
        
        std::cout << "5. analysis\n";
        std::cout << "   Data analysis mode\n";
        std::cout << "   Analyzes simulation results and generates statistics\n\n";
        
        std::cout << "6. development\n";
        std::cout << "   Development testing mode\n";
        std::cout << "   Tests developmental processes like synaptogenesis and pruning\n\n";
        
        std::cout << "7. full-integration\n";
        std::cout << "   Complete system integration test\n";
        std::cout << "   Runs all tests and verifies system cohesion\n\n";
    }
    
private:
    static bool showProgress;
};

bool OutputFormatter::showProgress = true;

// =============================================================================
// Test Framework
// =============================================================================

class TestResult {
public:
    std::string name;
    bool passed;
    std::string message;
    double duration;
    std::map<std::string, std::string> metrics;
    
    TestResult(const std::string& n, bool p = false, const std::string& m = "")
        : name(n), passed(p), message(m), duration(0.0) {}
};

class TestFramework {
public:
    static std::vector<TestResult> runTestSuite(const CommandLineOptions& options, 
                                               std::shared_ptr<Brain> brain) {
        std::vector<TestResult> results;
        
        PerformanceMonitor::startTiming();
        
        // Test 1: Basic Connectivity Test
        if (isTestEnabled(options, "connectivity")) {
            results.push_back(runConnectivityTest(brain, options));
        }
        
        // Test 2: Plasticity Learning Experiment
        if (isTestEnabled(options, "plasticity")) {
            results.push_back(runPlasticityTest(brain, options));
        }
        
        // Test 3: STDP Verification
        if (isTestEnabled(options, "stdp")) {
            results.push_back(runStdpTest(brain, options));
        }
        
        // Test 4: Memory Integration Test
        if (isTestEnabled(options, "memory")) {
            results.push_back(runMemoryTest(brain, options));
        }
        
        // Test 5: Neuromodulation Test
        if (isTestEnabled(options, "neuromodulation")) {
            results.push_back(runNeuromodulationTest(brain, options));
        }
        
        // Test 6: Development Test
        if (isTestEnabled(options, "development")) {
            results.push_back(runDevelopmentTest(brain, options));
        }
        
        // Test 7: Full Integration Test
        if (isTestEnabled(options, "integration")) {
            results.push_back(runIntegrationTest(brain, options));
        }
        
        PerformanceMonitor::stopTiming();
        
        return results;
    }
    
    static bool isTestEnabled(const CommandLineOptions& options, const std::string& testName) {
        if (!options.customSequence.empty()) {
            // Check if test is in custom sequence
            return std::find(options.customSequence.begin(), options.customSequence.end(), testName) != options.customSequence.end();
        }
        
        // Check if test is explicitly enabled
        if (!options.enabledTests.empty()) {
            return std::find(options.enabledTests.begin(), options.enabledTests.end(), testName) != options.enabledTests.end();
        }
        
        // Check if test is explicitly disabled
        if (!options.disabledTests.empty()) {
            return std::find(options.disabledTests.begin(), options.disabledTests.end(), testName) == options.disabledTests.end();
        }
        
        // Default: all tests enabled except benchmark
        return testName != "benchmark";
    }
    
    static TestResult runConnectivityTest(std::shared_ptr<Brain> brain, const CommandLineOptions& options) {
        PerformanceMonitor::startTestTiming("connectivity");
        TestResult result("connectivity", false, "Basic connectivity test");
        
        try {
            ErrorHandler::logInfo("Running connectivity test...", "TestFramework");
            
            auto* region = brain->getRegion(RegionId(1));
            if (!region) {
                throw std::runtime_error("No region found");
            }
            
            auto neurons = region->getAllNeurons();
            if (neurons.empty()) {
                throw std::runtime_error("No neurons found");
            }
            
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
                neurons[i]->injectCurrent(50.0f);
            }
            
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->step(step, step * options.timeStep);
            }
            
            size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
            
            if (spikes > 0) {
                result.passed = true;
                result.message = "Connectivity test passed: " + std::to_string(spikes) + " spikes generated";
            } else {
                result.message = "Connectivity test failed: no spikes generated";
            }
            
        } catch (const std::exception& e) {
            result.message = "Connectivity test exception: " + std::string(e.what());
        }
        
        PerformanceMonitor::stopTestTiming("connectivity");
        return result;
    }
    
    static TestResult runPlasticityTest(std::shared_ptr<Brain> brain, const CommandLineOptions& options) {
        PerformanceMonitor::startTestTiming("plasticity");
        TestResult result("plasticity", false, "Plasticity learning experiment");
        
        try {
            ErrorHandler::logInfo("Running plasticity test...", "TestFramework");
            
            LearningExperiment experiment(brain, options.randomSeed);
            experiment.recordInitialState();
            
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (auto& syn : region->getSynapses()) {
                    syn->enablePlasticity(true, true, false);
                }
            }
            
            for (SimulationStep step = 0; step < 100; ++step) {
                for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 4; ++i) {
                    brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                }
                brain->step(step, step * options.timeStep);
                
                if (options.progress && step % 20 == 0) {
                    OutputFormatter::printProgress("Plasticity test", (step * 100) / 100);
                }
            }
            
            experiment.recordFinalState();
            experiment.computeStatistics();
            
            bool learningOccurred = (std::abs(experiment.getLearningMagnitude()) > 0.001f);
            
            if (learningOccurred) {
                result.passed = true;
                result.message = "Plasticity test passed: learning detected with magnitude " + std::to_string(experiment.getLearningMagnitude());
            } else {
                result.message = "Plasticity test failed: no learning detected";
            }
            
        } catch (const std::exception& e) {
            result.message = "Plasticity test exception: " + std::string(e.what());
        }
        
        PerformanceMonitor::stopTestTiming("plasticity");
        return result;
    }
    
    static TestResult runStdpTest(std::shared_ptr<Brain> brain, const CommandLineOptions& options) {
        PerformanceMonitor::startTestTiming("stdp");
        TestResult result("stdp", false, "STDP verification test");
        
        try {
            ErrorHandler::logInfo("Running STDP test...", "TestFramework");
            
            auto* region = brain->getRegion(RegionId(1));
            if (!region) throw std::runtime_error("No region found");
            
            auto& synapses = region->getSynapses();
            if (synapses.size() < 3) throw std::runtime_error("Not enough synapses for STDP test");
            
            std::vector<float> beforeWeights;
            for (size_t i = 0; i < std::min(size_t(3), synapses.size()); ++i) {
                beforeWeights.push_back(synapses[i]->getWeight());
                synapses[i]->enablePlasticity(false, true, false);
            }
            
            // Create correlated pre->post activity
            for (int trial = 0; trial < 30; ++trial) {
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    neurons[0]->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * options.timeStep);
                    neurons[1]->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * options.timeStep);
                }
            }
            
            float totalDelta = 0.0f;
            for (size_t i = 0; i < beforeWeights.size(); ++i) {
                totalDelta += synapses[i]->getWeight() - beforeWeights[i];
            }
            
            if (totalDelta > 0.001f) {
                result.passed = true;
                result.message = "STDP test passed: LTP detected with delta " + std::to_string(totalDelta);
            } else if (totalDelta < -0.001f) {
                result.message = "STDP test warning: LTD detected (delta " + std::to_string(totalDelta) + ")";
                result.passed = true;
            } else {
                result.message = "STDP test failed: no significant weight change (delta " + std::to_string(totalDelta) + ")";
            }
            
        } catch (const std::exception& e) {
            result.message = "STDP test exception: " + std::string(e.what());
        }
        
        PerformanceMonitor::stopTestTiming("stdp");
        return result;
    }
    
    static TestResult runMemoryTest(std::shared_ptr<Brain> brain, const CommandLineOptions& options) {
        PerformanceMonitor::startTestTiming("memory");
        TestResult result("memory", false, "Memory integration test");
        
        try {
            ErrorHandler::logInfo("Running memory test...", "TestFramework");
            
            // Test working memory
            auto* workingMem = brain->getWorkingMemory();
            if (!workingMem) {
                throw std::runtime_error("Working memory system not available");
            }
            
            // Store some patterns
            std::vector<float> pattern1 = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
            std::vector<float> pattern2 = {0.9f, 0.1f, 0.7f, 0.4f, 0.6f};
            
            workingMem->storePattern(pattern1);
            workingMem->storePattern(pattern2);
            
            // Retrieve patterns
            auto retrieved1 = workingMem->retrievePattern(pattern1);
            auto retrieved2 = workingMem->retrievePattern(pattern2);
            
            float similarity1 = workingMem->patternSimilarity(retrieved1, pattern1);
            float similarity2 = workingMem->patternSimilarity(retrieved2, pattern2);
            
            if (similarity1 > 0.9f && similarity2 > 0.9f) {
                result.passed = true;
                result.message = "Memory test passed: pattern retrieval successful (similarity: " + std::to_string(similarity1) + ", " + std::to_string(similarity2) + ")";
            } else {
                result.message = "Memory test failed: poor pattern similarity (" + std::to_string(similarity1) + ", " + std::to_string(similarity2) + ")";
            }
            
        } catch (const std::exception& e) {
            result.message = "Memory test exception: " + std::string(e.what());
        }
        
        PerformanceMonitor::stopTestTiming("memory");
        return result;
    }
    
    static TestResult runNeuromodulationTest(std::shared_ptr<Brain> brain, const CommandLineOptions& options) {
        PerformanceMonitor::startTestTiming("neuromodulation");
        TestResult result("neuromodulation", false, "Neuromodulation test");
        
        try {
            ErrorHandler::logInfo("Running neuromodulation test...", "TestFramework");
            
            // Test dopamine system
            auto* dopamine = brain->getDopamine();
            if (!dopamine) {
                throw std::runtime_error("Dopamine system not available");
            }
            
            float initialDopamine = dopamine->getCurrentLevel();
            
            // Apply reward signal
            Reward reward(1.0f, RewardType::Positive, "test_reward");
            brain->applyNeuromodulation(reward);
            
            float finalDopamine = dopamine->getCurrentLevel();
            
            if (finalDopamine > initialDopamine) {
                result.passed = true;
                result.message = "Neuromodulation test passed: dopamine level increased from " + std::to_string(initialDopamine) + " to " + std::to_string(finalDopamine);
            } else {
                result.message = "Neuromodulation test failed: dopamine level did not increase (initial: " + std::to_string(initialDopamine) + ", final: " + std::to_string(finalDopamine) + ")";
            }
            
        } catch (const std::exception& e) {
            result.message = "Neuromodulation test exception: " + std::string(e.what());
        }
        
        PerformanceMonitor::stopTestTiming("neuromodulation");
        return result;
    }
    
    static TestResult runDevelopmentTest(std::shared_ptr<Brain> brain, const CommandLineOptions& options) {
        PerformanceMonitor::startTestTiming("development");
        TestResult result("development", false, "Development system test");
        
        try {
            ErrorHandler::logInfo("Running development test...", "TestFramework");
            
            // Test developmental stage
            auto currentStage = brain->getDevelopmentalStage();
            brain->setDevelopmentalStage(DevelopmentalStage::Juvenile);
            
            auto newStage = brain->getDevelopmentalStage();
            
            if (newStage == DevelopmentalStage::Juvenile) {
                result.passed = true;
                result.message = "Development test passed: stage changed from " + developmentalStageToString(currentStage) + " to " + developmentalStageToString(newStage);
            } else {
                result.message = "Development test failed: stage not changed (still " + developmentalStageToString(newStage) + ")";
            }
            
        } catch (const std::exception& e) {
            result.message = "Development test exception: " + std::string(e.what());
        }
        
        PerformanceMonitor::stopTestTiming("development");
        return result;
    }
    
    static TestResult runIntegrationTest(std::shared_ptr<Brain> brain, const CommandLineOptions& options) {
        PerformanceMonitor::startTestTiming("integration");
        TestResult result("integration", false, "Full integration test");
        
        try {
            ErrorHandler::logInfo("Running integration test...", "TestFramework");
            
            // Reset and reinitialize brain for clean state
            brain->reset();
            brain->initialize();
            
            // Run a basic simulation to test integration
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            for (SimulationStep step = 0; step < 50; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
                brain->step(step, step * options.timeStep);
            }
            
            size_t finalSpikes = brain->getTotalSpikeCount();
            size_t spikesGenerated = finalSpikes - initialSpikes;
            
            if (spikesGenerated > 0) {
                result.passed = true;
                result.message = "Integration test passed: system integration successful with " + std::to_string(spikesGenerated) + " spikes generated";
            } else {
                result.message = "Integration test failed: no spikes generated during integration test";
            }
            
        } catch (const std::exception& e) {
            result.message = "Integration test exception: " + std::string(e.what());
        }
        
        PerformanceMonitor::stopTestTiming("integration");
        return result;
    }
    
    static TestResult runBenchmark(const CommandLineOptions& options) {
        TestResult result("benchmark", false, "Performance benchmark");
        
        try {
            ErrorHandler::logInfo("Running benchmark...", "TestFramework");
            
            std::vector<std::pair<size_t, size_t>> benchmarks = {
                {100, 1000},
                {500, 1000},
                {1000, 500},
                {2000, 200}
            };
            
            std::vector<BenchmarkResult> results;
            
            for (const auto& [neurons, steps] : benchmarks) {
                results.push_back(runBenchmarkTest(neurons, steps));
                OutputFormatter::printProgress("Benchmark", benchmarks.size());
            }
            
            // Analyze results
            double minTime = std::numeric_limits<double>::max();
            double maxTime = 0.0;
            double totalTime = 0.0;
            
            for (const auto& r : results) {
                if (r.totalTime < minTime) minTime = r.totalTime;
                if (r.totalTime > maxTime) maxTime = r.totalTime;
                totalTime += r.totalTime;
            }
            
            double avgTime = totalTime / results.size();
            
            if (maxTime > 0.0) {
                result.passed = true;
                result.message = "Benchmark passed: average time " + std::to_string(avgTime) + "s (min: " + std::to_string(minTime) + "s, max: " + std::to_string(maxTime) + "s)";
            } else {
                result.message = "Benchmark failed: invalid timing measurements";
            }
            
        } catch (const std::exception& e) {
            result.message = "Benchmark test exception: " + std::string(e.what());
        }
        
        return result;
    }
    
    static BenchmarkResult runBenchmarkTest(size_t neuronCount, size_t steps) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto startTime = high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            for (size_t i = 0; i < std::min(size_t(10), neuronCount / 10); ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            brain->step(step, step * 0.001);
        }
        
        auto endTime = high_resolution_clock::now();
        double totalTime = duration_cast<duration<double>>(endTime - startTime).count();
        
        BenchmarkResult benchmarkResult;
        benchmarkResult.neurons = brain->getTotalNeuronCount();
        benchmarkResult.synapses = brain->getTotalSynapseCount();
        benchmarkResult.steps = steps;
        benchmarkResult.totalTime = totalTime;
        benchmarkResult.stepsPerSecond = steps / totalTime;
        benchmarkResult.spikesPerStep = static_cast<double>(brain->getTotalSpikeCount()) / steps;
        benchmarkResult.synapsesPerNeuron = static_cast<double>(benchmarkResult.synapses) / benchmarkResult.neurons;
        
        return benchmarkResult;
    }
};

// =============================================================================
// Learning Experiment Structure (for plasticity test)
// =============================================================================

struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    float learningMagnitude;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), learningMagnitude(0.0f) {}
    
    void recordInitialState() {
        initialSynapseCount = brain->getTotalSynapseCount();
        initialWeights.clear();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        NLM_LOG_INFO("Initial state recorded:");
        NLM_LOG_INFO("  Synapses: " + std::to_string(initialSynapseCount));
        if (!initialWeights.empty()) {
            float sum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
            float mean = sum / initialWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
    }
    
    void recordFinalState() {
        finalWeights.clear();
        
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        mostActiveNeurons = brain->getSpikeSystem()->getMostActiveNeurons(10);
        
        NLM_LOG_INFO("Final state recorded:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        if (!finalWeights.empty()) {
            float sum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
            float mean = sum / finalWeights.size();
            NLM_LOG_INFO("  Mean weight: " + std::to_string(mean));
        }
        
        computeLearningMagnitude();
    }
    
    void computeLearningMagnitude() {
        if (initialWeights.empty() || finalWeights.empty()) {
            learningMagnitude = 0.0f;
            return;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        learningMagnitude = std::abs(finalMean - initialMean);
        
        NLM_LOG_INFO("Learning magnitude: " + std::to_string(learningMagnitude));
    }
    
    float getLearningMagnitude() const {
        return learningMagnitude;
    }
    
    void computeStatistics() {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Learning Experiment Results ===");
        NLM_LOG_INFO("");
        
        if (initialWeights.empty() || finalWeights.empty()) {
            NLM_LOG_INFO("ERROR: No weights recorded");
            return;
        }
        
        float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
        float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
        float initialMean = initialSum / initialWeights.size();
        float finalMean = finalSum / finalWeights.size();
        
        NLM_LOG_INFO("Weight Statistics:");
        NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
        NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
        NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
        
        size_t strengthened = 0;
        size_t weakened = 0;
        size_t unchanged = 0;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            if (delta > 0.01f) ++strengthened;
            else if (delta < -0.01f) ++weakened;
            else ++unchanged;
        }
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Synaptic Changes:");
        NLM_LOG_INFO("  Strengthened: " + std::to_string(strengthened));
        NLM_LOG_INFO("  Weakened: " + std::to_string(weakened));
        NLM_LOG_INFO("  Unchanged: " + std::to_string(unchanged));
        
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Spike Activity:");
        NLM_LOG_INFO("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()));
        NLM_LOG_INFO("  Most active neurons recorded: " + std::to_string(mostActiveNeurons.size()));
        
        bool learningOccurred = (std::abs(finalMean - initialMean) > 0.001f) ||
                               (strengthened > 0 || weakened > 0);
        
        NLM_LOG_INFO("");
        if (learningOccurred) {
            NLM_LOG_INFO("✓ LEARNING DETECTED: Synaptic weights changed through experience");
        } else {
            NLM_LOG_INFO("✗ NO LEARNING: Weights did not change significantly");
        }
    }
};

// =============================================================================
// Mode-Specific Functions
// =============================================================================

void runModeTest(const CommandLineOptions& options) {
    ErrorHandler::logInfo("Running in TEST mode", "main");
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timeStep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain in TEST mode", ExitCode::InitializationError);
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    auto results = TestFramework::runTestSuite(options, brain);
    
    // Print test results
    ErrorHandler::logInfo("=== Test Suite Results ===", "main");
    bool allPassed = true;
    for (const auto& result : results) {
        ErrorHandler::logInfo("Test: " + result.name + " - " + result.message, "main");
        if (!result.passed) allPassed = false;
    }
    
    if (allPassed) {
        ErrorHandler::logInfo("All tests passed!", "main");
    } else {
        ErrorHandler::logError("Some tests failed!", ExitCode::RuntimeError);
        std::exit(static_cast<int>(ExitCode::RuntimeError));
    }
}

void runModeSimulate(const CommandLineOptions& options) {
    ErrorHandler::logInfo("Running in SIMULATE mode", "main");
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timeStep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain in SIMULATE mode", ExitCode::InitializationError);
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    ErrorHandler::logInfo("Starting simulation for " + std::to_string(options.maxSteps) + " steps", "main");
    
    PerformanceMonitor::startTiming();
    
    for (SimulationStep step = 0; step < options.maxSteps; ++step) {
        // Inject input based on step
        for (size_t i = 0; i < 5 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 10.0f);
        }
        
        brain->step(step, step * options.timeStep);
        
        if (options.progress && step % 100 == 0) {
            OutputFormatter::printProgress("Simulation", (step * 100) / options.maxSteps);
        }
    }
    
    PerformanceMonitor::stopTiming();
    
    ErrorHandler::logInfo("Simulation complete", "main");
    brain->logStatus();
    
    PerformanceMonitor::printPerformanceReport();
}

void runModeExperiment(const CommandLineOptions& options) {
    ErrorHandler::logInfo("Running in EXPERIMENT mode", "main");
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timeStep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain in EXPERIMENT mode", ExitCode::InitializationError);
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    // Run basic connectivity test
    runBasicConnectivityTest(brain);
    
    // Reset and run plasticity experiment
    brain->reset();
    brain->initialize();
    
    runPlasticityExperiment(brain);
    
    // Reset and run STDP verification
    brain->reset();
    brain->initialize();
    
    runStdpVerification(brain);
    
    // Run memory and neuromodulation tests
    brain->reset();
    brain->initialize();
    
    runMemoryIntegrationTest(brain);
    
    brain->reset();
    brain->initialize();
    
    runNeuromodulationTest(brain);
    
    brain->logStatus();
    
    PerformanceMonitor::printPerformanceReport();
}

void runModeBenchmark(const CommandLineOptions& options) {
    ErrorHandler::logInfo("Running in BENCHMARK mode", "main");
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timeStep, ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain in BENCHMARK mode", ExitCode::InitializationError);
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    std::vector<std::pair<size_t, size_t>> benchmarks = {
        {100, 1000},
        {500, 1000},
        {1000, 500},
        {2000, 200}
    };
    
    std::vector<BenchmarkResult> results;
    
    for (size_t i = 0; i < benchmarks.size(); ++i) {
        const auto& bench = benchmarks[i];
        results.push_back(runBenchmarkTest(bench.first, bench.second));
        
        if (options.progress) {
            OutputFormatter::printProgress("Benchmark", i * 100 / benchmarks.size());
        }
    }
    
    printBenchmarkResults(results);
    
    PerformanceMonitor::printPerformanceReport();
}

void runModeAnalysis(const CommandLineOptions& options) {
    ErrorHandler::logInfo("Running in ANALYSIS mode", "main");
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timeStep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain in ANALYSIS mode", ExitCode::InitializationError);
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    // Run a comprehensive analysis
    ErrorHandler::logInfo("=== Analysis Mode ===", "main");
    
    // Analyze neural properties
    ErrorHandler::logInfo("Neural Network Analysis:", "main");
    ErrorHandler::logInfo("  Total neurons: " + std::to_string(brain->getTotalNeuronCount()), "main");
    ErrorHandler::logInfo("  Total synapses: " + std::to_string(brain->getTotalSynapseCount()), "main");
    ErrorHandler::logInfo("  Active neurons: " + std::to_string(brain->getActiveNeuronCount()), "main");
    ErrorHandler::logInfo("  Firing neurons: " + std::to_string(brain->getFiringNeuronCount()), "main");
    ErrorHandler::logInfo("  Average firing rate: " + std::to_string(brain->getAverageFiringRate()), "main");
    ErrorHandler::logInfo("  Excitation/inhibition ratio: " + std::to_string(brain->getExcitationInhibitionRatio()), "main");
    ErrorHandler::logInfo("  Total spikes: " + std::to_string(brain->getTotalSpikeCount()), "main");
    
    // Save analysis results
    std::map<std::string, std::string> analysisData;
    analysisData["total_neurons"] = std::to_string(brain->getTotalNeuronCount());
    analysisData["total_synapses"] = std::to_string(brain->getTotalSynapseCount());
    analysisData["active_neurons"] = std::to_string(brain->getActiveNeuronCount());
    analysisData["firing_neurons"] = std::to_string(brain->getFiringNeuronCount());
    analysisData["avg_firing_rate"] = std::to_string(brain->getAverageFiringRate());
    analysisData["e_i_ratio"] = std::to_string(brain->getExcitationInhibitionRatio());
    analysisData["total_spikes"] = std::to_string(brain->getTotalSpikeCount());
    analysisData["mode"] = "analysis";
    analysisData["timestamp"] = getCurrentTimestamp();
    
    if (!options.jsonOutputFile.empty()) {
        OutputFormatter::printJSON(options.jsonOutputFile, analysisData);
    }
    
    PerformanceMonitor::printPerformanceReport();
}

void runModeDevelopment(const CommandLineOptions& options) {
    ErrorHandler::logInfo("Running in DEVELOPMENT mode", "main");
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timeStep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain in DEVELOPMENT mode", ExitCode::InitializationError);
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    // Test developmental processes
    ErrorHandler::logInfo("=== Development Mode ===", "main");
    
    // Test synaptogenesis
    ErrorHandler::logInfo("Testing synaptogenesis...");
    auto* plasticity = brain->getStructuralPlasticity();
    if (plasticity) {
        size_t initialSynapses = brain->getTotalSynapseCount();
        brain->develop();
        size_t finalSynapses = brain->getTotalSynapseCount();
        ErrorHandler::logInfo("  Synaptogenesis: " + std::to_string(finalSynapses - initialSynapses) + " new synapses", "main");
    }
    
    // Test pruning
    ErrorHandler::logInfo("Testing synaptic pruning...");
    if (plasticity) {
        size_t initialSynapses = brain->getTotalSynapseCount();
        brain->develop();
        size_t finalSynapses = brain->getTotalSynapseCount();
        ErrorHandler::logInfo("  Pruning: " + std::to_string(initialSynapses - finalSynapses) + " synapses removed", "main");
    }
    
    // Test maturation
    ErrorHandler::logInfo("Testing maturation...");
    auto currentStage = brain->getDevelopmentalStage();
    brain->setDevelopmentalStage(DevelopmentalStage::Juvenile);
    auto newStage = brain->getDevelopmentalStage();
    ErrorHandler::logInfo("  Maturation: " + developmentalStageToString(currentStage) + " -> " + developmentalStageToString(newStage), "main");
    
    brain->logStatus();
    
    PerformanceMonitor::printPerformanceReport();
}

void runModeFullIntegration(const CommandLineOptions& options) {
    ErrorHandler::logInfo("Running in FULL INTEGRATION mode", "main");
    
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(options.randomSeed), ConfigSource::Default);
    config->set("simulation_timestep", options.timeStep, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(options.neuronCount), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(options.regionCount), ConfigSource::Default);
    
    auto brain = std::make_shared<Brain>(config);
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain in FULL INTEGRATION mode", ExitCode::InitializationError);
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    // Run comprehensive integration test
    ErrorHandler::logInfo("=== Full Integration Test ===", "main");
    
    // Test all systems
    bool connectivityTest = runConnectivityIntegration(brain) != nullptr;
    bool plasticityTest = runPlasticityIntegration(brain) != nullptr;
    bool memoryTest = runMemoryIntegration(brain) != nullptr;
    bool neuromodulationTest = runNeuromodulationIntegration(brain) != nullptr;
    bool developmentTest = runDevelopmentIntegration(brain) != nullptr;
    
    ErrorHandler::logInfo("Integration Results:", "main");
    ErrorHandler::logInfo("  Connectivity: " + std::string(connectivityTest ? "✓" : "✗"), "main");
    ErrorHandler::logInfo("  Plasticity: " + std::string(plasticityTest ? "✓" : "✗"), "main");
    ErrorHandler::logInfo("  Memory: " + std::string(memoryTest ? "✓" : "✗"), "main");
    ErrorHandler::logInfo("  Neuromodulation: " + std::string(neuromodulationTest ? "✓" : "✗"), "main");
    ErrorHandler::logInfo("  Development: " + std::string(developmentTest ? "✓" : "✗"), "main");
    
    bool allTestsPassed = connectivityTest && plasticityTest && memoryTest && 
                        neuromodulationTest && developmentTest;
    
    if (allTestsPassed) {
        ErrorHandler::logInfo("All integration tests passed!", "main");
    } else {
        ErrorHandler::logError("Some integration tests failed!", ExitCode::RuntimeError);
        std::exit(static_cast<int>(ExitCode::RuntimeError));
    }
    
    brain->logStatus();
    
    PerformanceMonitor::printPerformanceReport();
}

// =============================================================================
// Individual Test Functions (for integration mode)
// =============================================================================

std::shared_ptr<Brain> runConnectivityIntegration(std::shared_ptr<Brain> brain) {
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return nullptr;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) return nullptr;
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    return spikes > 0 ? brain : nullptr;
}

std::shared_ptr<Brain> runPlasticityIntegration(std::shared_ptr<Brain> brain) {
    LearningExperiment experiment(brain, 42);
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    for (SimulationStep step = 0; step < 100; ++step) {
        for (size_t i = 0; i < 10 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        brain->step(step, step * 0.001);
    }
    
    experiment.recordFinalState();
    return (experiment.getLearningMagnitude() > 0.001f) ? brain : nullptr;
}

std::shared_ptr<Brain> runMemoryIntegration(std::shared_ptr<Brain> brain) {
    auto* workingMem = brain->getWorkingMemory();
    if (!workingMem) return nullptr;
    
    std::vector<float> pattern1 = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    std::vector<float> pattern2 = {0.9f, 0.1f, 0.7f, 0.4f, 0.6f};
    
    workingMem->storePattern(pattern1);
    workingMem->storePattern(pattern2);
    
    auto retrieved1 = workingMem->retrievePattern(pattern1);
    auto retrieved2 = workingMem->retrievePattern(pattern2);
    
    float similarity1 = workingMem->patternSimilarity(retrieved1, pattern1);
    float similarity2 = workingMem->patternSimilarity(retrieved2, pattern2);
    
    return (similarity1 > 0.9f && similarity2 > 0.9f) ? brain : nullptr;
}

std::shared_ptr<Brain> runNeuromodulationIntegration(std::shared_ptr<Brain> brain) {
    auto* dopamine = brain->getDopamine();
    if (!dopamine) return nullptr;
    
    float initialDopamine = dopamine->getCurrentLevel();
    
    Reward reward(1.0f, RewardType::Positive, "integration_test");
    brain->applyNeuromodulation(reward);
    
    float finalDopamine = dopamine->getCurrentLevel();
    
    return (finalDopamine > initialDopamine) ? brain : nullptr;
}

std::shared_ptr<Brain> runDevelopmentIntegration(std::shared_ptr<Brain> brain) {
    auto currentStage = brain->getDevelopmentalStage();
    brain->setDevelopmentalStage(DevelopmentalStage::Juvenile);
    auto newStage = brain->getDevelopmentalStage();
    
    return (newStage == DevelopmentalStage::Juvenile) ? brain : nullptr;
}

// =============================================================================
// Original Test Functions (for backward compatibility)
// =============================================================================

void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto neurons = region->getAllNeurons();
    if (neurons.empty()) {
        NLM_LOG_INFO("  No neurons found!");
        return;
    }
    
    size_t initialSpikes = brain->getTotalSpikeCount();
    
    NLM_LOG_INFO("  Injecting current into 10 neurons...");
    for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
        neurons[i]->injectCurrent(50.0f);
    }
    
    for (SimulationStep step = 0; step < 50; ++step) {
        brain->step(step, step * 0.001);
    }
    
    size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
    NLM_LOG_INFO("  Spikes generated: " + std::to_string(spikes));
    
    if (spikes > 0) {
        NLM_LOG_INFO("  ✓ Spikes propagate through network");
    } else {
        NLM_LOG_INFO("  ! No spikes - checking neuron parameters...");
        for (size_t i = 0; i < std::min(size_t(3), neurons.size()); ++i) {
            NLM_LOG_INFO("    Neuron " + std::to_string(i) + 
                        " V=" + std::to_string(neurons[i]->getMembranePotential()) +
                        " thresh=" + std::to_string(neurons[i]->getThreshold()));
        }
    }
}

void runPlasticityExperiment(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 2: Plasticity Learning Experiment ===");
    
    LearningExperiment experiment(brain, 42);
    
    experiment.recordInitialState();
    
    if (auto* region = brain->getRegion(RegionId(1))) {
        for (auto& syn : region->getSynapses()) {
            syn->enablePlasticity(true, true, false);
        }
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
    
    for (SimulationStep step = 0; step < 1000; ++step) {
        for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
        }
        
        brain->step(step, step * 0.001);
        
        if (step % 100 == 0) {
            NLM_LOG_INFO("  Step " + std::to_string(step) + 
                        " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                        " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
        }
    }
    
    experiment.recordFinalState();
    experiment.computeStatistics();
}

void runStdpVerification(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 3: STDP Verification ===");
    
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    auto& synapses = region->getSynapses();
    if (synapses.size() < 5) {
        NLM_LOG_INFO("  Not enough synapses for STDP test");
        return;
    }
    
    NLM_LOG_INFO("  Testing STDP on 5 synapses:");
    
    std::vector<float> beforeWeights;
    for (size_t i = 0; i < 5; ++i) {
        beforeWeights.push_back(synapses[i]->getWeight());
        synapses[i]->enablePlasticity(false, true, false);
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
    }
    
    NLM_LOG_INFO("");
    NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
    
    for (int trial = 0; trial < 50; ++trial) {
        Neuron* preNeuron = nullptr;
        Neuron* postNeuron = nullptr;
        
        auto neurons = region->getAllNeurons();
        if (neurons.size() >= 2) {
            preNeuron = neurons[0];
            postNeuron = neurons[1];
        }
        
        if (preNeuron && postNeuron) {
            preNeuron->injectCurrent(60.0f);
            brain->step(trial * 2, trial * 2 * 0.001);
            
            postNeuron->injectCurrent(60.0f);
            brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
        }
    }
    
    NLM_LOG_INFO("  After correlated activity:");
    for (size_t i = 0; i < 5; ++i) {
        float delta = synapses[i]->getWeight() - beforeWeights[i];
        NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
    }
    
    float totalDelta = 0.0f;
    for (size_t i = 0; i < 5; ++i) {
        totalDelta += synapses[i]->getWeight() - beforeWeights[i];
    }
    
    NLM_LOG_INFO("");
    if (totalDelta > 0.001f) {
        NLM_LOG_INFO("  ✓ STDP WORKING: Pre-before-post produced potentiation");
    } else if (totalDelta < -0.001f) {
        NLM_LOG_INFO("  ! STDP reversed: Check parameters");
    } else {
        NLM_LOG_INFO("  ! No change: STDP may not be triggering");
    }
}

void runMemoryIntegrationTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 4: Memory Integration Test ===");
    
    auto* workingMem = brain->getWorkingMemory();
    if (!workingMem) {
        NLM_LOG_INFO("  Working memory system not available");
        return;
    }
    
    std::vector<float> pattern1 = {0.1f, 0.5f, 0.3f, 0.8f, 0.2f};
    std::vector<float> pattern2 = {0.9f, 0.1f, 0.7f, 0.4f, 0.6f};
    
    workingMem->storePattern(pattern1);
    workingMem->storePattern(pattern2);
    
    auto retrieved1 = workingMem->retrievePattern(pattern1);
    auto retrieved2 = workingMem->retrievePattern(pattern2);
    
    float similarity1 = workingMem->patternSimilarity(retrieved1, pattern1);
    float similarity2 = workingMem->patternSimilarity(retrieved2, pattern2);
    
    NLM_LOG_INFO("  Pattern 1 similarity: " + std::to_string(similarity1));
    NLM_LOG_INFO("  Pattern 2 similarity: " + std::to_string(similarity2));
    
    if (similarity1 > 0.9f && similarity2 > 0.9f) {
        NLM_LOG_INFO("  ✓ Memory integration test passed");
    } else {
        NLM_LOG_INFO("  ✗ Memory integration test failed");
    }
}

void runNeuromodulationTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 5: Neuromodulation Test ===");
    
    auto* dopamine = brain->getDopamine();
    if (!dopamine) {
        NLM_LOG_INFO("  Dopamine system not available");
        return;
    }
    
    float initialDopamine = dopamine->getCurrentLevel();
    
    Reward reward(1.0f, RewardType::Positive, "test_reward");
    brain->applyNeuromodulation(reward);
    
    float finalDopamine = dopamine->getCurrentLevel();
    
    NLM_LOG_INFO("  Initial dopamine: " + std::to_string(initialDopamine));
    NLM_LOG_INFO("  Final dopamine: " + std::to_string(finalDopamine));
    
    if (finalDopamine > initialDopamine) {
        NLM_LOG_INFO("  ✓ Neuromodulation test passed");
    } else {
        NLM_LOG_INFO("  ✗ Neuromodulation test failed");
    }
}

void runDevelopmentTest(std::shared_ptr<Brain> brain) {
    NLM_LOG_INFO("");
    NLM_LOG_INFO("=== Test 6: Development System Test ===");
    
    auto currentStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("  Current developmental stage: " + developmentalStageToString(currentStage));
    
    brain->setDevelopmentalStage(DevelopmentalStage::Juvenile);
    auto newStage = brain->getDevelopmentalStage();
    NLM_LOG_INFO("  Set to Juvenile stage: " + developmentalStageToString(newStage));
    
    if (newStage == DevelopmentalStage::Juvenile) {
        NLM_LOG_INFO("  ✓ Development test passed");
    } else {
        NLM_LOG_INFO("  ✗ Development test failed");
    }
}

// =============================================================================
// Utility Functions
// =============================================================================

std::string getCurrentTimestamp() {
    auto now = system_clock::now();
    auto time = system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

std::string developmentalStageToString(DevelopmentalStage stage) {
    switch (stage) {
        case DevelopmentalStage::Embryonic: return "Embryonic";
        case DevelopmentalStage::Larval: return "Larval";
        case DevelopmentalStage::Juvenile: return "Juvenile";
        case DevelopmentalStage::Adult: return "Adult";
        default: return "Unknown";
    }
}

// =============================================================================
// Signal Handling
// =============================================================================

void signalHandler(int signum) {
    ErrorHandler::logWarning("Received signal " + std::to_string(signum), "signalHandler");
    std::exit(static_cast<int>(ExitCode::RuntimeError));
}

void setupSignalHandlers() {
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
}

// =============================================================================
// Main Function
// =============================================================================

int main(int argc, char** argv) {
    // Setup signal handlers
    setupSignalHandlers();
    
    // Parse command line arguments
    CommandLineOptions options = ArgumentParser::parse(argc, argv);
    
    // Handle help and version
    if (options.help) {
        OutputFormatter::printHelp(options);
        return 0;
    }
    
    if (options.version) {
        std::cout << "NLM Phase 2+ Version 2.0.0" << std::endl;
        return 0;
    }
    
    if (options.listModes) {
        OutputFormatter::printModesList();
        return 0;
    }
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    // Configure error logging
    ErrorHandler::setDebugEnabled(options.debug);
    if (!options.errorLogFile.empty()) {
        ErrorHandler::setErrorLogFile(options.errorLogFile);
    }
    
    // Print banner
    OutputFormatter::printBanner();
    
    ErrorHandler::logInfo("Starting NLM with mode: " + options.mode, "main");
    
    // Initialize configuration
    auto config = std::make_shared<Config>();
    
    // Load configuration from file
    bool configLoaded = ConfigurationManager::loadConfiguration(options.configFile, config);
    if (configLoaded) {
        ErrorHandler::logInfo("Loaded configuration from: " + options.configFile, "main");
    }
    
    // Override with command line arguments
    config->loadFromArgs(argc, argv);
    
    // Apply defaults
    ConfigurationManager::applyDefaults(config);
    
    // Validate configuration
    if (options.validateConfig || options.debug) {
        if (!ConfigurationManager::validateConfiguration(config)) {
            ErrorHandler::logError("Configuration validation failed", ExitCode::ConfigError);
            std::exit(static_cast<int>(ExitCode::ConfigError));
        }
    }
    
    // Print configuration summary
    if (options.verbose || options.debug) {
        ConfigurationManager::printConfigurationSummary(config);
    }
    
    // Save initial state if requested
    if (options.saveState) {
        std::string stateFile = options.stateFile.empty() ? "brain_initial_state.dat" : options.stateFile;
        // Note: Would need to implement brain state saving here
        ErrorHandler::logInfo("State saving not yet implemented", "main");
    }
    
    // Initialize simulation clock
    double timestep = config->getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    ErrorHandler::logInfo("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s", "main");
    
    // Initialize brain
    ErrorHandler::logInfo("Initializing NLM Brain...", "main");
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        ErrorHandler::logError("Failed to initialize brain!", "main");
        std::exit(static_cast<int>(ExitCode::InitializationError));
    }
    
    brain->logStatus();
    
    // Run selected mode
    try {
        if (options.mode == "test") {
            runModeTest(options);
        } else if (options.mode == "simulate") {
            runModeSimulate(options);
        } else if (options.mode == "experiment") {
            runModeExperiment(options);
        } else if (options.mode == "benchmark") {
            runModeBenchmark(options);
        } else if (options.mode == "analysis") {
            runModeAnalysis(options);
        } else if (options.mode == "development") {
            runModeDevelopment(options);
        } else if (options.mode == "full-integration") {
            runModeFullIntegration(options);
        } else {
            ErrorHandler::logError("Unknown mode: " + options.mode, ExitCode::ValidationError);
            std::exit(static_cast<int>(ExitCode::ValidationError));
        }
    } catch (const std::exception& e) {
        ErrorHandler::logError(std::string("Exception during execution: ") + e.what(), "main");
        std::exit(static_cast<int>(ExitCode::RuntimeError));
    }
    
    ErrorHandler::logInfo("=== Execution Complete ===", "main");
    
    return 0;
}
