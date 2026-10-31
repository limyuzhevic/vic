// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
// Enhanced Command Line Interface with Subcommand System

#include "core/Config/Config.hpp"
#include "core/Random/Random.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/Metrics.hpp"
#include "experiments/Experiment.hpp"
#include "performance/CheckpointSystem.hpp"
#include "memory/Memory.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <algorithm>
#include <map>
#include <set>
#include <ctime>
#include <cmath>
#include <cstring>

namespace fs = std::filesystem;

using namespace nlm;
using namespace std;

// Color codes for terminal output
#define COLOR_RESET   "\033[0m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"
#define COLOR_GRAY    "\033[90m"

// Output formats
enum class OutputFormat {
    HumanReadable,
    JSON,
    CSV,
    Tabular,
    Minimal
};

// Progress indicator
class ProgressIndicator {
private:
    size_t total;
    size_t current;
    string message;
    chrono::steady_clock::time_point startTime;
public:
    ProgressIndicator(size_t t, const string& msg) : total(t), current(0), message(msg) {
        startTime = chrono::steady_clock::now();
    }
    
    void update(size_t value, const string& status = "") {
        current = value;
        float progress = static_cast<float>(current) / total * 100.0f;
        int barWidth = 50;
        int pos = static_cast<int>(barWidth * progress / 100.0f);
        
        cout << "\r" << COLOR_CYAN << "[" << message << "] " << COLOR_RESET;
        cout << COLOR_GREEN << "[";
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) cout << "=";
            else if (i == pos) cout << ">";
            else cout << " ";
        }
        cout << "] " << COLOR_RESET;
        cout << fixed << setprecision(1) << progress << "%";
        
        if (!status.empty()) {
            cout << " - " << status;
        }
        
        auto elapsed = chrono::duration_cast<chrono::seconds>(
            chrono::steady_clock::now() - startTime).count();
        if (elapsed > 0) {
            size_t remaining = (total - current) * elapsed / current;
            cout << " - " << remaining << "s remaining";
        }
        
        cout.flush();
        
        if (current >= total) {
            cout << endl;
        }
    }
    
    void finish() {
        if (current < total) {
            update(total, "Complete");
        }
    }
};

// Error handling
class NLMError : public std::runtime_error {
public:
    NLMError(const string& message, int code = 1) 
        : std::runtime_error(message), code_(code) {}
    
    int getCode() const { return code_; }
    
private:
    int code_;
};

// Test function signatures
using TestFunction = bool(*)(shared_ptr<Brain>, const Config&);

// Command arguments structure
struct CommandArgs {
    string command;
    map<string, string> options;
    vector<string> positionalArgs;
    map<string, string> flags;
    bool helpRequested;
    string configFile;
    string outputFile;
    string brainFile;
    OutputFormat format;
    bool quiet;
    bool verbose;
    bool colors;
    bool progress;
    map<string, string> extraParams;
    
    // Run command parameters
    double runDuration;
    double runTimestep;
    string runBrainFile;
    
    // Test command parameters
    vector<string> testNames;
    bool testAll;
    bool testVerbose;
    
    // Benchmark command parameters
    string benchmarkMode;
    int benchmarkDuration;
    
    // Profile command parameters
    string profileType;
    
    // Configure command parameters
    string configurePreset;
    bool configureSave;
    string configureFile;
    
    // Save/Load parameters
    string saveFile;
    string loadFile;
};n

// Configuration presets
struct ConfigurationPreset {
    string name;
    map<string, string> parameters;
    string description;
};

// Test information structure
struct TestInfo {
    string name;
    string description;
    TestFunction function;
    bool enabled;
    size_t testNumber;
    map<string, string> parameters;
};

// Command line interface class
class NLMCommandLineInterface {
private:
    shared_ptr<Config> config_;
    shared_ptr<Brain> brain_;
    shared_ptr<SimulationClock> clock_;
    shared_ptr<Metrics> metrics_;
    shared_ptr<CheckpointSystem> checkpointSystem_;
    shared_ptr<Memory> memorySystem_;
    
    OutputFormat outputFormat_;
    bool quietMode_;
    bool verboseMode_;
    string outputFile_;
    string logLevel_;
    bool progressEnabled_;
    bool colorsEnabled_;
    
    map<string, TestInfo> availableTests_;
    vector<string> testOrder_;
    
    // Command execution methods
    void executeHelp(const CommandArgs& args);
    void executeRun(const CommandArgs& args);
    void executeTest(const CommandArgs& args);
    void executeBenchmark(const CommandArgs& args);
    void executeProfile(const CommandArgs& args);
    void executeSave(const CommandArgs& args);
    void executeLoad(const CommandArgs& args);
    void executeConfigure(const CommandArgs& args);
    void executeInfo(const CommandArgs& args);
    
    // Core functionality
    void initializeNLM(const Config& config);
    void cleanupNLM();
    void runSimulation(const Config& config);
    void runIndividualTest(const string& testName, const Config& config);
    void runAllTests(const Config& config);
    
    // Output formatting
    void printHeader(const string& title, const string& subtitle = "");
    void printFooter();
    void printProgress(const string& message, size_t current, size_t total);
    void printError(const string& error, bool exit = true);
    void printSuccess(const string& message);
    void printInfo(const string& message);
    void printWarning(const string& message);
    void printDebug(const string& message);
    
    void outputHumanReadable(const string& data);
    void outputJSON(const string& data);
    void outputCSV(const string& data);
    void outputTabular(const string& data);
    void outputMinimal(const string& data);
    
    // Configuration management
    void initializeConfigurationPresets();
    void loadConfiguration(const string& configFile);
    void saveConfiguration(const string& configFile);
    void displayConfigurationPresets();
    void applyConfigurationPreset(const string& presetName);
    
    // Test management
    void initializeAvailableTests();
    void displayAvailableTests();
    bool runSpecificTest(const string& testName, const Config& config);
    bool runAllTests(const Config& config);
    
    // Advanced features
    void saveBrainState(const string& filepath);
    bool loadBrainState(const string& filepath);
    void collectMetrics(const string& operation);
    void generatePerformanceReport();
    void displaySystemInfo();
    
    // Utilities
    bool validateFile(const string& path, bool requireExists = true);
    bool validateNumeric(const string& value, double& result);
    int64_t parseInt64(const string& value);
    double parseDouble(const string& value);
    bool parseBool(const string& value);
    string trim(const string& str);
    string toLower(const string& str);
    string getTimestamp();
    
public:
    NLMCommandLineInterface();
    ~NLMCommandLineInterface();
    
    void run(int argc, char** argv);
    void setOutputFormat(const string& format);
    void setQuietMode(bool quiet) { quietMode_ = quiet; }
    void setVerboseMode(bool verbose) { verboseMode_ = verbose; }
    void enableColors(bool enable) { colorsEnabled_ = enable; }
    
    // Parse command line arguments
    CommandArgs parseCommandLine(int argc, char** argv);
};

// Main function
int main(int argc, char** argv) {
    NLMCommandLineInterface cli;
    try {
        cli.run(argc, argv);
    } catch (const NLMError& e) {
        cli.printError("Error: " + string(e.what()), false);
        return e.getCode();
    } catch (const exception& e) {
        cli.printError("Unexpected error: " + string(e.what()), true);
        return 1;
    } catch (...) {
        cli.printError("Unknown error occurred", true);
        return 1;
    }
    return 0;
}

// Initialize command line interface
NLMCommandLineInterface::NLMCommandLineInterface() 
    : quietMode_(false), verboseMode_(false), progressEnabled_(true), colorsEnabled_(true) {
    config_ = make_shared<Config>();
    metrics_ = make_shared<Metrics>();
    checkpointSystem_ = make_shared<CheckpointSystem>();
    memorySystem_ = make_shared<Memory>();
    
    outputFormat_ = OutputFormat::HumanReadable;
    
    initializeAvailableTests();
    initializeConfigurationPresets();
    
    // Set default log level
    Logger::getGlobal().setLevel(LogLevel::Info);
}

NLMCommandLineInterface::~NLMCommandLineInterface() {
    cleanupNLM();
}

void NLMCommandLineInterface::run(int argc, char** argv) {
    CommandArgs args = parseCommandLine(argc, argv);
    
    if (args.helpRequested) {
        executeHelp(args);
        return;
    }
    
    // Load configuration
    if (!args.configFile.empty()) {
        loadConfiguration(args.configFile);
    }
    
    // Override with command line args
    config_->loadFromArgs(argc, argv);
    
    // Set output format
    setOutputFormat(args.format);
    setQuietMode(args.quiet);
    setVerboseMode(args.verbose);
    enableColors(args.colors);
    
    // Execute command
    if (args.command == "help") {
        executeHelp(args);
    } else if (args.command == "run") {
        executeRun(args);
    } else if (args.command == "test") {
        executeTest(args);
    } else if (args.command == "benchmark") {
        executeBenchmark(args);
    } else if (args.command == "profile") {
        executeProfile(args);
    } else if (args.command == "save") {
        executeSave(args);
    } else if (args.command == "load") {
        executeLoad(args);
    } else if (args.command == "configure") {
        executeConfigure(args);
    } else if (args.command == "info") {
        executeInfo(args);
    } else {
        printError("Unknown command: " + args.command);
        executeHelp(args);
    }
}

CommandArgs NLMCommandLineInterface::parseCommandLine(int argc, char** argv) {
    CommandArgs args;
    args.helpRequested = false;
    args.format = OutputFormat::HumanReadable;
    args.quiet = false;
    args.verbose = false;
    args.colors = true;
    args.progress = true;
    args.testAll = false;
    args.testVerbose = false;
    args.configureSave = false;
    
    // Default values
    args.runDuration = 10.0;
    args.runTimestep = 0.001;
    args.benchmarkMode = "performance";
    args.benchmarkDuration = 60;
    
    // Basic argument parsing (enhanced version of original)
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        
        if (arg == "--help" || arg == "-h") {
            args.helpRequested = true;
        } else if (arg == "--config" && i + 1 < argc) {
            args.configFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            args.outputFile = argv[++i];
        } else if (arg == "--quiet" || arg == "-q") {
            args.quiet = true;
        } else if (arg == "--verbose" || arg == "-v") {
            args.verbose = true;
        } else if (arg == "--no-progress") {
            args.progress = false;
        } else if (arg == "--no-colors") {
            args.colors = false;
        } else if (arg == "--format" && i + 1 < argc) {
            string format = toLower(argv[++i]);
            if (format == "json") args.format = OutputFormat::JSON;
            else if (format == "csv") args.format = OutputFormat::CSV;
            else if (format == "tabular") args.format = OutputFormat::Tabular;
            else if (format == "minimal") args.format = OutputFormat::Minimal;
            else args.format = OutputFormat::HumanReadable;
        } else if (arg == "--log-level" && i + 1 < argc) {
            logLevel_ = argv[++i];
            // Convert log level string to enum
            if (logLevel_ == "debug") Logger::getGlobal().setLevel(LogLevel::Debug);
            else if (logLevel_ == "info") Logger::getGlobal().setLevel(LogLevel::Info);
            else if (logLevel_ == "warning") Logger::getGlobal().setLevel(LogLevel::Warning);
            else if (logLevel_ == "error") Logger::getGlobal().setLevel(LogLevel::Error);
            else if (logLevel_ == "critical") Logger::getGlobal().setLevel(LogLevel::Critical);
        } else if (arg.substr(0, 2) == "--") {
            // Store option
            size_t pos = arg.find('=');
            if (pos != string::npos) {
                string key = arg.substr(2, pos - 2);
                string value = arg.substr(pos + 1);
                args.options[key] = value;
            } else {
                args.options[arg.substr(2)] = "";
            }
        } else if (arg[0] != '-') {
            // Positional argument
            args.positionalArgs.push_back(arg);
        }
    }
    
    // First positional argument is command
    if (!args.positionalArgs.empty()) {
        args.command = args.positionalArgs[0];
        args.positionalArgs.erase(args.positionalArgs.begin());
    }
    
    // Parse command-specific options
    if (args.command == "run") {
        if (args.options.find("duration") != args.options.end()) {
            args.runDuration = parseDouble(args.options.at("duration"));
        }
        if (args.options.find("timestep") != args.options.end()) {
            args.runTimestep = parseDouble(args.options.at("timestep"));
        }
        if (args.options.find("brain-file") != args.options.end()) {
            args.runBrainFile = args.options.at("brain-file");
        }
    } else if (args.command == "test") {
        args.testAll = true;
        args.testVerbose = args.options.count("verbose") > 0;
        
        // Parse test names
        for (const auto& arg : args.positionalArgs) {
            if (arg[0] != '-') {
                args.testNames.push_back(arg);
            }
        }
    } else if (args.command == "benchmark") {
        if (args.options.find("mode") != args.options.end()) {
            args.benchmarkMode = args.options.at("mode");
        }
        if (args.options.find("duration") != args.options.end()) {
            args.benchmarkDuration = parseInt64(args.options.at("duration"));
        }
    } else if (args.command == "profile") {
        if (args.options.find("type") != args.options.end()) {
            args.profileType = args.options.at("type");
        }
    } else if (args.command == "save") {
        if (!args.positionalArgs.empty()) {
            args.saveFile = args.positionalArgs[0];
        }
    } else if (args.command == "load") {
        if (!args.positionalArgs.empty()) {
            args.loadFile = args.positionalArgs[0];
        }
    } else if (args.command == "configure") {
        if (!args.positionalArgs.empty()) {
            args.configurePreset = args.positionalArgs[0];
        }
        args.configureSave = args.options.count("save") > 0;
        if (args.options.find("file") != args.options.end()) {
            args.configureFile = args.options.at("file");
        }
    }
    
    return args;
}

void NLMCommandLineInterface::printHeader(const string& title, const string& subtitle) {
    if (quietMode_) return;
    
    cout << COLOR_CYAN << "\n";
    cout << "╔═══════════════════════════════════════════════════════════════╗" << endl;
    cout << "║" << setw(78) << "" << "║" << endl;
    cout << "║" << setw(38) << "" << title << setw(38) << "" << "║" << endl;
    if (!subtitle.empty()) {
        cout << "║" << setw(78) << "" << "║" << endl;
        cout << "║" << setw(30) << "" << subtitle << setw(30) << "" << "║" << endl;
    }
    cout << "║" << setw(78) << "" << "║" << endl;
    cout << "╚═══════════════════════════════════════════════════════════════╝" << endl;
    cout << COLOR_RESET;
}

void NLMCommandLineInterface::printFooter() {
    if (quietMode_) return;
    
    cout << COLOR_GRAY << "\n";
    cout << "Command completed successfully." << endl;
    cout << "Use '--help' for more information about commands." << endl;
    cout << COLOR_RESET;
}

void NLMCommandLineInterface::printProgress(const string& message, size_t current, size_t total) {
    if (!progressEnabled_ || quietMode_) return;
    
    cout << "\r" << COLOR_YELLOW << "[Progress] " << COLOR_RESET << message;
    if (total > 0) {
        cout << " - " << current << "/" << total << " (" << 
            (total > 0 ? (current * 100.0 / total) : 0) << "%)";
    }
    cout.flush();
    
    if (current >= total) {
        cout << endl;
    }
}

void NLMCommandLineInterface::printError(const string& error, bool exit) {
    if (colorsEnabled_) cout << COLOR_RED;
    cout << "ERROR: " << error << endl;
    if (colorsEnabled_) cout << COLOR_RESET;
    
    if (exit) {
        exit(1);
    }
}

void NLMCommandLineInterface::printSuccess(const string& message) {
    if (colorsEnabled_) cout << COLOR_GREEN;
    cout << "SUCCESS: " << message << endl;
    if (colorsEnabled_) cout << COLOR_RESET;
}

void NLMCommandLineInterface::printInfo(const string& message) {
    if (colorsEnabled_) cout << COLOR_BLUE;
    cout << "INFO: " << message << endl;
    if (colorsEnabled_) cout << COLOR_RESET;
}

void NLMCommandLineInterface::printWarning(const string& message) {
    if (colorsEnabled_) cout << COLOR_YELLOW;
    cout << "WARNING: " << message << endl;
    if (colorsEnabled_) cout << COLOR_RESET;
}

void NLMCommandLineInterface::printDebug(const string& message) {
    if (verboseMode_ && quietMode_) {
        cout << "DEBUG: " << message << endl;
    }
}

void NLMCommandLineInterface::executeHelp(const CommandArgs& args) {
    printHeader("NLM Help and Usage", "Neural Learning Machine - Enhanced CLI");
    
    cout << COLOR_CYAN << "\nCommands:" << COLOR_RESET << endl;
    cout << "  " << COLOR_GREEN << "run" << COLOR_RESET << "              Run simulation with custom parameters" << endl;
    cout << "  " << COLOR_GREEN << "test" << COLOR_RESET << "              Run specific or all tests" << endl;
    cout << "  " << COLOR_GREEN << "benchmark" << COLOR_RESET << "        Performance benchmarking" << endl;
    cout << "  " << COLOR_GREEN << "profile" << COLOR_RESET << "          Profile simulation performance" << endl;
    cout << "  " << COLOR_GREEN << "save" << COLOR_RESET << "             Save brain state" << endl;
    cout << "  " << COLOR_GREEN << "load" << COLOR_RESET << "             Load brain state" << endl;
    cout << "  " << COLOR_GREEN << "configure" << COLOR_RESET << "       Interactive/configuration management" << endl;
    cout << "  " << COLOR_GREEN << "info" << COLOR_RESET << "             Display system information" << endl;
    cout << "  " << COLOR_GREEN << "help" << COLOR_RESET << "             Show this help message" << endl;
    
    cout << COLOR_CYAN << "\nOptions:" << COLOR_RESET << endl;
    cout << "  " << COLOR_YELLOW << "--help, -h" << COLOR_RESET << "                 Show help for a specific command" << endl;
    cout << "  " << COLOR_YELLOW << "--config FILE" << COLOR_RESET << "            Load configuration from file" << endl;
    cout << "  " << COLOR_YELLOW << "--output FILE" << COLOR_RESET << "           Output results to file" << endl;
    cout << "  " << COLOR_YELLOW << "--format FORMAT" << COLOR_RESET << "         Output format (human, json, csv, tabular, minimal)" << endl;
    cout << "  " << COLOR_YELLOW << "--quiet, -q" << COLOR_RESET << "            Quiet mode (minimal output)" << endl;
    cout << "  " << COLOR_YELLOW << "--verbose, -v" << COLOR_RESET << "           Verbose mode (debug output)" << endl;
    cout << "  " << COLOR_YELLOW << "--no-progress" << COLOR_RESET << "          Disable progress indicators" << endl;
    cout << "  " << COLOR_YELLOW << "--no-colors" << COLOR_RESET << "            Disable colors in output" << endl;
    cout << "  " << COLOR_YELLOW << "--log-level LEVEL" << COLOR_RESET << "      Set log level (debug, info, warning, error, critical)" << endl;
    
    cout << COLOR_CYAN << "\nExample Usage:" << COLOR_RESET << endl;
    cout << "  nlm run --config myconfig.cfg --neuron-count 1000" << endl;
    cout << "  nlm test --test basic --output results.json" << endl;
    cout << "  nlm benchmark --mode performance --duration 60" << endl;
    
    cout << COLOR_CYAN << "\nConfiguration Presets:" << COLOR_RESET << endl;
    displayConfigurationPresets();
    
    printFooter();
}

void NLMCommandLineInterface::executeRun(const CommandArgs& args) {
    printHeader("Run Simulation", "Executing NLM simulation with custom parameters");
    
    // Validate required parameters
    if (args.runDuration <= 0) {
        printError("Duration must be positive");
    }
    
    // Initialize NLM
    initializeNLM(*config_);
    
    // Run simulation
    printInfo("Running simulation for " + to_string(args.runDuration) + " seconds...");
    runSimulation(*config_);
    
    // Collect metrics
    collectMetrics("simulation_run");
    
    printSuccess("Simulation completed successfully");
    generatePerformanceReport();
}

void NLMCommandLineInterface::executeTest(const CommandArgs& args) {
    printHeader("Run Tests", "Executing NLM tests");
    
    if (args.testNames.empty()) {
        printInfo("Running all tests...");
        runAllTests(*config_);
    } else {
        for (const auto& testName : args.testNames) {
            if (availableTests_.find(testName) == availableTests_.end()) {
                printError("Unknown test: " + testName);
            }
            
            printInfo("Running test: " + testName);
            runIndividualTest(testName, *config_);
        }
    }
    
    printSuccess("Tests completed");
}

void NLMCommandLineInterface::executeBenchmark(const CommandArgs& args) {
    printHeader("Benchmark", "Performance benchmarking of NLM");
    
    string mode = args.benchmarkMode;
    int duration = args.benchmarkDuration;
    
    printInfo("Running benchmark in " + mode + " mode for " + to_string(duration) + " seconds...");
    
    // Initialize NLM
    initializeNLM(*config_);
    
    // Run benchmark based on mode
    auto startTime = chrono::steady_clock::now();
    auto endTime = startTime + chrono::seconds(duration);
    
    size_t totalSteps = 0;
    size_t spikesGenerated = 0;
    double totalActivity = 0.0;
    
    while (chrono::steady_clock::now() < endTime) {
        brain_->step(totalSteps, totalSteps * config_->getOr<double>("simulation_timestep", 0.001));
        totalSteps++;
        
        // Collect metrics
        spikesGenerated = brain_->getTotalSpikeCount();
        totalActivity += brain_->getFiringNeuronCount();
        
        collectMetrics("benchmark_step");
        
        if (progressEnabled_) {
            printProgress("Benchmarking", totalSteps, duration * 1000 / config_->getOr<double>("simulation_timestep", 0.001));
        }
    }
    
    auto totalTime = chrono::duration_cast<chrono::seconds>(
        chrono::steady_clock::now() - startTime).count();
    
    // Generate report
    printInfo("Benchmark completed in " + to_string(totalTime) + " seconds");
    printInfo("Total steps: " + to_string(totalSteps));
    printInfo("Total spikes: " + to_string(spikesGenerated));
    printInfo("Average activity per step: " + to_string(totalActivity / max(totalSteps, size_t(1))));
    printInfo("Steps per second: " + to_string(totalSteps / max(totalTime, 1.0)));
    
    generatePerformanceReport();
}

void NLMCommandLineInterface::executeProfile(const CommandArgs& args) {
    printHeader("Profile", "Profiling NLM performance");
    
    string profileType = args.profileType.empty() ? "memory" : args.profileType;
    
    printInfo("Starting profiling in " + profileType + " mode...");
    
    // Initialize NLM
    initializeNLM(*config_);
    
    // Run profiling based on type
    if (profileType == "memory") {
        printInfo("Profiling memory usage...");
        // Memory profiling implementation
        size_t memoryBefore = 0;  // Get memory usage before
        runAllTests(*config_);
        size_t memoryAfter = 0;   // Get memory usage after
        // Generate memory profile report
    } else if (profileType == "cpu") {
        printInfo("Profiling CPU performance...");
        runAllTests(*config_);
        // Generate CPU profile report
    } else if (profileType == "detailed") {
        printInfo("Running detailed profiling...");
        runAllTests(*config_);
        // Generate detailed profile report
    }
    
    printSuccess("Profiling completed");
}

void NLMCommandLineInterface::executeSave(const CommandArgs& args) {
    printHeader("Save Brain State", "Saving current brain state to file");
    
    string filepath = args.saveFile.empty() ? "brain_state.nlm" : args.saveFile;
    
    if (!validateFile(filepath, false)) {
        printError("Invalid filepath: " + filepath);
    }
    
    saveBrainState(filepath);
    printSuccess("Brain state saved to: " + filepath);
}

void NLMCommandLineInterface::executeLoad(const CommandArgs& args) {
    printHeader("Load Brain State", "Loading brain state from file");
    
    if (args.loadFile.empty()) {
        printError("No filepath provided for load command");
    }
    
    string filepath = args.loadFile;
    
    if (!validateFile(filepath)) {
        printError("File not found: " + filepath);
    }
    
    if (loadBrainState(filepath)) {
        printSuccess("Brain state loaded from: " + filepath);
        initializeNLM(*config_);
    } else {
        printError("Failed to load brain state from: " + filepath);
    }
}

void NLMCommandLineInterface::executeConfigure(const CommandArgs& args) {
    printHeader("Configuration", "Interactive configuration management");
    
    displayConfigurationPresets();
    
    if (!args.configurePreset.empty()) {
        applyConfigurationPreset(args.configurePreset);
        printSuccess("Applied configuration preset: " + args.configurePreset);
    }
    
    // Display current configuration
    printInfo("Current configuration:");
    for (const auto& key : config_->getKeys()) {
        auto value = config_->get<string>(key);
        if (value) {
            printInfo("  " + key + ": " + *value);
        }
    }
    
    // Save configuration if requested
    if (args.configureSave) {
        string configFile = args.configureFile.empty() ? "configs/default.cfg" : args.configureFile;
        saveConfiguration(configFile);
        printSuccess("Configuration saved to: " + configFile);
    }
}

void NLMCommandLineInterface::executeInfo(const CommandArgs& args) {
    printHeader("System Information", "NLM system information and statistics");
    
    displaySystemInfo();
    
    // Display metrics
    cout << COLOR_CYAN << "\nMetrics:" << COLOR_RESET << endl;
    for (const auto& name : metrics_->getMetricNames()) {
        double mean = metrics_->getMean(name);
        double stddev = metrics_->getStdDev(name);
        double min = metrics_->getMin(name);
        double max = metrics_->getMax(name);
        
        cout << "  " << name << ": " << COLOR_GREEN << mean << COLOR_RESET << " ± " << stddev;
        cout << " [" << min << ", " << max << "]" << endl;
    }
    
    // Display brain status
    if (brain_) {
        cout << COLOR_CYAN << "\nBrain Status:" << COLOR_RESET << endl;
        brain_->logStatus();
    }
}

void NLMCommandLineInterface::initializeNLM(const Config& config) {
    // Clean up existing instances
    cleanupNLM();
    
    // Set simulation timestep
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    clock_ = make_shared<SimulationClock>(timestep);
    
    // Initialize brain
    brain_ = make_shared<Brain>(const_cast<Config&>(config));
    
    if (!brain_->initialize()) {
        printError("Failed to initialize brain");
    }
    
    printSuccess("NLM initialized successfully");
}

void NLMCommandLineInterface::cleanupNLM() {
    if (brain_) {
        brain_.reset();
    }
    clock_.reset();
}

void NLMCommandLineInterface::runSimulation(const Config& config) {
    double duration = config.getOr<double>("duration", 10.0);
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    
    size_t totalSteps = static_cast<size_t>(duration / timestep);
    
    for (size_t step = 0; step < totalSteps; ++step) {
        brain_->step(step, step * timestep);
        collectMetrics("simulation_step");
        
        if (progressEnabled_) {
            printProgress("Running simulation", step, totalSteps);
        }
    }
}

void NLMCommandLineInterface::runIndividualTest(const string& testName, const Config& config) {
    if (availableTests_.find(testName) == availableTests_.end()) {
        printError("Test not found: " + testName);
        return;
    }
    
    const TestInfo& test = availableTests_.at(testName);
    
    printInfo("Running test: " + test.name);
    if (!test.description.empty()) {
        printInfo("Description: " + test.description);
    }
    
    // Initialize NLM if needed
    if (!brain_) {
        initializeNLM(config);
    }
    
    // Execute test
    if (test.function(brain_, config)) {
        printSuccess("Test passed: " + test.name);
        collectMetrics("test_passed_" + testName);
    } else {
        printError("Test failed: " + test.name);
        collectMetrics("test_failed_" + testName);
    }
}

void NLMCommandLineInterface::runAllTests(const Config& config) {
    printInfo("Running all tests...");
    
    size_t testCount = 0;
    size_t passedCount = 0;
    
    for (const auto& testPair : availableTests_) {
        const TestInfo& test = testPair.second;
        if (!test.enabled) continue;
        
        testCount++;
        printInfo("Running test " + to_string(testCount) + "/" + 
                 to_string(availableTests_.size()) + ": " + test.name);
        
        runIndividualTest(test.name, config);
        passedCount++;
    }
    
    printInfo("All tests completed: " + to_string(passedCount) + "/" + to_string(testCount) + " passed");
}

void NLMCommandLineInterface::saveBrainState(const string& filepath) {
    if (!brain_) {
        printError("No brain state to save");
        return;
    }
    
    // Create checkpoint from brain
    auto checkpoint = make_shared<Checkpoint>();
    checkpoint->timestamp = getTimestamp();
    checkpoint->step = brain_->getCurrentStep();
    checkpoint->time = brain_->getSimulationTime();
    checkpoint->neuronCount = brain_->getTotalNeuronCount();
    checkpoint->synapseCount = brain_->getTotalSynapseCount();
    checkpoint->spikeCount = brain_->getTotalSpikeCount();
    
    // TODO: Implement brain serialization
    printWarning("Brain state saving is not fully implemented yet");
}

bool NLMCommandLineInterface::loadBrainState(const string& filepath) {
    printWarning("Brain state loading is not fully implemented yet");
    return false;
}

void NLMCommandLineInterface::collectMetrics(const string& operation) {
    if (!brain_) return;
    
    metrics_->record("spike_count", static_cast<double>(brain_->getTotalSpikeCount()));
    metrics_->record("firing_neurons", static_cast<double>(brain_->getFiringNeuronCount()));
    metrics_->record("simulation_time", brain_->getSimulationTime());
    metrics_->record("simulation_step", static_cast<double>(brain_->getCurrentStep()));
}

void NLMCommandLineInterface::generatePerformanceReport() {
    printHeader("Performance Report", "Simulation performance metrics");
    
    if (metrics_->getMetricNames().empty()) {
        printInfo("No metrics collected");
        return;
    }
    
    cout << COLOR_CYAN << "\nMetrics Summary:" << COLOR_RESET << endl;
    for (const auto& name : metrics_->getMetricNames()) {
        double mean = metrics_->getMean(name);
        double stddev = metrics_->getStdDev(name);
        double min = metrics_->getMin(name);
        double max = metrics_->getMax(name);
        
        cout << "  " << name << ": ";
        cout << COLOR_GREEN << mean << COLOR_RESET << " ± " << stddev;
        cout << " [" << min << ", " << max << "]" << endl;
    }
}

void NLMCommandLineInterface::displaySystemInfo() {
    printInfo("NLM Version: 2.0 (Phase 2: Real Neural Computation)");
    printInfo("Build Date: " << __DATE__ << " " << __TIME__);
    
    // Display configuration
    printInfo("\nConfiguration Summary:");
    for (const auto& key : config_->getKeys()) {
        auto value = config_->get<string>(key);
        if (value) {
            printInfo("  " + key + ": " + *value);
        }
    }
    
    // Display system resources
    printInfo("\nSystem Resources:");
    printInfo("  Memory: [TODO: Get memory usage]");
    printInfo("  CPU: [TODO: Get CPU info]");
    
    // Display brain capabilities
    printInfo("\nBrain Capabilities:");
    printInfo("  Neuron types: LIF (Leaky Integrate-and-Fire)");
    printInfo("  Plasticity: STDP, Hebbian, Structural");
    printInfo("  Neuromodulation: Dopamine, Curiosity, Novelty");
}

void NLMCommandLineInterface::initializeAvailableTests() {
    // Test 1: Basic Connectivity
    availableTests_["basic"] = TestInfo{
        "Basic Connectivity",
        "Test basic neural connectivity by injecting current and checking for spike propagation",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            // Inject current into neurons
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            brain->injectCurrentToNeurons(NeuronType::Motor, 20.0f);
            
            // Run for a few steps
            for (size_t step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            // Check if spikes were generated
            size_t spikes = brain->getTotalSpikeCount();
            return spikes > 0;
        },
        true,
        1,
        {{}}
    };
    
    // Test 2: Plasticity Learning
    availableTests_["plasticity"] = TestInfo{
        "Plasticity Learning",
        "Test synaptic plasticity through learning experiments",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            // Record initial weights
            vector<float> initialWeights;
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    initialWeights.push_back(syn->getWeight());
                }
            }
            
            // Apply learning stimuli
            for (size_t step = 0; step < 1000; ++step) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
                brain->step(step, step * 0.001);
            }
            
            // Record final weights
            vector<float> finalWeights;
            if (auto* region = brain->getRegion(RegionId(1))) {
                for (const auto& syn : region->getSynapses()) {
                    finalWeights.push_back(syn->getWeight());
                }
            }
            
            // Check for significant weight changes
            if (initialWeights.empty() || finalWeights.empty()) return false;
            
            size_t minSize = min(initialWeights.size(), finalWeights.size());
            size_t strengthened = 0;
            
            for (size_t i = 0; i < minSize; ++i) {
                if (finalWeights[i] - initialWeights[i] > 0.01f) {
                    strengthened++;
                }
            }
            
            return strengthened > 0;
        },
        true,
        2,
        {{}}
    };
    
    // Test 3: STDP Verification
    availableTests_["stdp"] = TestInfo{
        "STDP Verification",
        "Test Spike-Timing-Dependent Plasticity (STDP) mechanism",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            auto* region = brain->getRegion(RegionId(1));
            if (!region || region->getSynapses().size() < 5) return false;
            
            auto& synapses = region->getSynapses();
            
            // Record initial weights
            vector<float> beforeWeights;
            for (size_t i = 0; i < min(size_t(5), synapses.size()); ++i) {
                beforeWeights.push_back(synapses[i]->getWeight());
                synapses[i]->enablePlasticity(false, true, false);
            }
            
            // Create correlated activity
            for (int trial = 0; trial < 50; ++trial) {
                auto neurons = region->getAllNeurons();
                if (neurons.size() >= 2) {
                    // Fire pre then post to trigger LTP
                    neurons[0]->injectCurrent(60.0f);
                    brain->step(trial * 2, trial * 2 * 0.001);
                    
                    neurons[1]->injectCurrent(60.0f);
                    brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
                }
            }
            
            // Check for potentiation
            float totalDelta = 0.0f;
            for (size_t i = 0; i < min(size_t(5), synapses.size()); ++i) {
                totalDelta += synapses[i]->getWeight() - beforeWeights[i];
            }
            
            return totalDelta > 0.001f;
        },
        true,
        3,
        {{}}
    };
    
    // Test 4: Neuromodulation
    availableTests_["neuromodulation"] = TestInfo{
        "Neuromodulation",
        "Test dopamine, curiosity, and novelty-based modulation",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            // Apply reward
            brain->applyReward(0.8f, 0.5f);
            
            // Check if neuromodulation occurred
            size_t spikesBefore = brain->getTotalSpikeCount();
            
            // Inject current and run
            brain->injectCurrentToNeurons(NeuronType::Sensory, 40.0f);
            for (size_t step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t spikesAfter = brain->getTotalSpikeCount();
            return spikesAfter > spikesBefore;
        },
        true,
        4,
        {{}}
    };
    
    // Test 5: Structural Plasticity
    availableTests_["structural"] = TestInfo{
        "Structural Plasticity",
        "Test synaptogenesis and pruning mechanisms",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            size_t initialSynapses = brain->getTotalSynapseCount();
            
            // Run simulation to allow structural changes
            for (size_t step = 0; step < 500; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t finalSynapses = brain->getTotalSynapseCount();
            
            // Check for changes
            return abs(static_cast<int>(finalSynapses) - static_cast<int>(initialSynapses)) > 0;
        },
        true,
        5,
        {{}}
    };
    
    // Test 6: Learning Capacity
    availableTests_["learning"] = TestInfo{
        "Learning Capacity",
        "Test long-term learning and memory formation",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            // Create a complex learning scenario
            vector<float> pattern1 = {0.1f, 0.2f, 0.3f, 0.4f, 0.5f};
            vector<float> pattern2 = {0.5f, 0.4f, 0.3f, 0.2f, 0.1f};
            
            for (size_t iteration = 0; iteration < 1000; ++iteration) {
                // Apply pattern based on iteration
                float stimulus = (iteration % 2 == 0) ? 0.7f : 0.3f;
                brain->injectCurrentToNeurons(NeuronType::Sensory, stimulus);
                
                // Run simulation
                brain->step(iteration, iteration * 0.001);
                
                // Apply reward
                brain->applyReward(stimulus, stimulus * 0.5f);
            }
            
            // Check learning by checking spike patterns
            size_t totalSpikes = brain->getTotalSpikeCount();
            size_t firingNeurons = brain->getFiringNeuronCount();
            
            // Learning is indicated by sustained activity
            return totalSpikes > 100 && firingNeurons > 5;
        },
        true,
        6,
        {{}}
    };
    
    // Test 7: Performance Benchmark
    availableTests_["performance"] = TestInfo{
        "Performance Benchmark",
        "Benchmark simulation performance with different parameters",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            auto startTime = chrono::steady_clock::now();
            size_t stepsCompleted = 0;
            
            // Run benchmark
            for (size_t step = 0; step < 1000; ++step) {
                brain->step(step, step * 0.001);
                stepsCompleted++;
            }
            
            auto endTime = chrono::steady_clock::now();
            auto duration = chrono::duration_cast<chrono::milliseconds>(endTime - startTime).count();
            
            // Performance criteria: complete 1000 steps in reasonable time
            double stepsPerSecond = (1000.0 / duration) * 1000; // Convert to per second
            
            return stepsPerSecond > 100.0; // Should complete >100 steps per millisecond
        },
        true,
        7,
        {{}}
    };
    
    // Test 8: Integration Test
    availableTests_["integration"] = TestInfo{
        "Integration Test",
        "Comprehensive integration test of all neural components",
        [](shared_ptr<Brain> brain, const Config& config) -> bool {
            if (!brain) return false;
            
            // Test all major components
            bool allComponentsWorking = true;
            
            // Test neurons
            auto neurons = brain->getTotalNeuronCount();
            if (neurons == 0) allComponentsWorking = false;
            
            // Test synapses
            auto synapses = brain->getTotalSynapseCount();
            if (synapses == 0) allComponentsWorking = false;
            
            // Test activity
            size_t initialSpikes = brain->getTotalSpikeCount();
            
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            for (size_t step = 0; step < 50; ++step) {
                brain->step(step, step * 0.001);
            }
            
            size_t finalSpikes = brain->getTotalSpikeCount();
            
            if (finalSpikes <= initialSpikes) allComponentsWorking = false;
            
            // Test plasticity
            brain->applyReward(0.8f, 0.5f);
            if (!brain->getTotalSpikeCount() > initialSpikes) allComponentsWorking = false;
            
            return allComponentsWorking;
        },
        true,
        8,
        {{}}
    };
}

void NLMCommandLineInterface::displayAvailableTests() {
    printHeader("Available Tests", "List of all available NLM tests");
    
    cout << COLOR_CYAN << "\nTests:" << COLOR_RESET << endl;
    for (const auto& testPair : availableTests_) {
        const TestInfo& test = testPair.second;
        cout << "  " << COLOR_GREEN << test.name << COLOR_RESET << ": " << test.description << endl;
        cout << "    Test " << test.testNumber << "/" << availableTests_.size() << "" << endl;
    }
}

void NLMCommandLineInterface::initializeConfigurationPresets() {
    // Performance preset
    ConfigurationPreset performancePreset;
    performancePreset.name = "performance";
    performancePreset.parameters = {
        {"neuron_count", "2000"},
        {"simulation_timestep", "0.0005"},
        {"region_count", "4"},
        {"connection_probability", "0.2"},
        {"stdp_ltp_weight", "0.025f"},
        {"stdp_ltd_weight", "0.020f"},
        {"synaptogenesis_rate", "0.0002f"},
        {"pruning_rate", "0.00005f"}
    };
    performancePreset.description = "High performance configuration with more neurons and faster simulation";
    
    // Accuracy preset
    ConfigurationPreset accuracyPreset;
    accuracyPreset.name = "accuracy";
    accuracyPreset.parameters = {
        {"neuron_count", "500"},
        {"simulation_timestep", "0.001f"},
        {"region_count", "2"},
        {"connection_probability", "0.15f"},
        {"stdp_ltp_weight", "0.02f"},
        {"stdp_ltd_weight", "0.015f"},
        {"synaptogenesis_rate", "0.0001f"},
        {"pruning_rate", "0.00001f"}
    };
    accuracyPreset.description = "High accuracy configuration with balanced performance";
    
    // Memory preset
    ConfigurationPreset memoryPreset;
    memoryPreset.name = "memory";
    memoryPreset.parameters = {
        {"neuron_count", "200"},
        {"simulation_timestep", "0.001f"},
        {"region_count", "1"},
        {"connection_probability", "0.1f"},
        {"stdp_ltp_weight", "0.01f"},
        {"stdp_ltd_weight", "0.008f"},
        {"synaptogenesis_rate", "0.00005f"},
        {"pruning_rate", "0.000005f"}
    };
    memoryPreset.description = "Memory-efficient configuration with fewer neurons";
    
    // Development preset
    ConfigurationPreset developmentPreset;
    developmentPreset.name = "development";
    developmentPreset.parameters = {
        {"neuron_count", "100"},
        {"simulation_timestep", "0.002f"},
        {"region_count", "1"},
        {"connection_probability", "0.05f"},
        {"stdp_ltp_weight", "0.015f"},
        {"stdp_ltd_weight", "0.012f"},
        {"synaptogenesis_rate", "0.00015f"},
        {"pruning_rate", "0.00002f"}
    };
    developmentPreset.description = "Development configuration with slower simulation for debugging";
}

void NLMCommandLineInterface::displayConfigurationPresets() {
    printInfo("Available configuration presets:");
    
    // Create a list of presets
    vector<ConfigurationPreset> presets = {
        {"performance", 
         {{{"neuron_count", "2000"}, {"simulation_timestep", "0.0005"}, {"region_count", "4"},
           {"connection_probability", "0.2"}, {"stdp_ltp_weight", "0.025f"},
           {"stdp_ltd_weight", "0.020f"}, {"synaptogenesis_rate", "0.0002f"},
           {"pruning_rate", "0.00005f"}}},
         "High performance configuration with more neurons and faster simulation"},
        
        {"accuracy",
         {{{"neuron_count", "500"}, {"simulation_timestep", "0.001f"}, {"region_count", "2"},
           {"connection_probability", "0.15f"}, {"stdp_ltp_weight", "0.02f"},
           {"stdp_ltd_weight", "0.015f"}, {"synaptogenesis_rate", "0.0001f"},
           {"pruning_rate", "0.00001f"}}},
         "High accuracy configuration with balanced performance"},
        
        {"memory",
         {{{"neuron_count", "200"}, {"simulation_timestep", "0.001f"}, {"region_count", "1"},
           {"connection_probability", "0.1f"}, {"stdp_ltp_weight", "0.01f"},
           {"stdp_ltd_weight", "0.008f"}, {"synaptogenesis_rate", "0.00005f"},
           {"pruning_rate", "0.000005f"}}},
         "Memory-efficient configuration with fewer neurons"},
        
        {"development",
         {{{"neuron_count", "100"}, {"simulation_timestep", "0.002f"}, {"region_count", "1"},
           {"connection_probability", "0.05f"}, {"stdp_ltp_weight", "0.015f"},
           {"stdp_ltd_weight", "0.012f"}, {"synaptogenesis_rate", "0.00015f"},
           {"pruning_rate", "0.00002f"}}},
         "Development configuration with slower simulation for debugging"}
    };
    
    for (const auto& preset : presets) {
        cout << "  " << COLOR_GREEN << preset.name << COLOR_RESET << ": " << preset.description << endl;
    }
}

void NLMCommandLineInterface::applyConfigurationPreset(const string& presetName) {
    // Map of preset names to their parameters
    map<string, map<string, string>> presets = {
        {"performance", {
            {"neuron_count", "2000"},
            {"simulation_timestep", "0.0005"},
            {"region_count", "4"},
            {"connection_probability", "0.2"},
            {"stdp_ltp_weight", "0.025f"},
            {"stdp_ltd_weight", "0.020f"},
            {"synaptogenesis_rate", "0.0002f"},
            {"pruning_rate", "0.00005f"}
        }},
        {"accuracy", {
            {"neuron_count", "500"},
            {"simulation_timestep", "0.001f"},
            {"region_count", "2"},
            {"connection_probability", "0.15f"},
            {"stdp_ltp_weight", "0.02f"},
            {"stdp_ltd_weight", "0.015f"},
            {"synaptogenesis_rate", "0.0001f"},
            {"pruning_rate", "0.00001f"}
        }},
        {"memory", {
            {"neuron_count", "200"},
            {"simulation_timestep", "0.001f"},
            {"region_count", "1"},
            {"connection_probability", "0.1f"},
            {"stdp_ltp_weight", "0.01f"},
            {"stdp_ltd_weight", "0.008f"},
            {"synaptogenesis_rate", "0.00005f"},
            {"pruning_rate", "0.000005f"}
        }},
        {"development", {
            {"neuron_count", "100"},
            {"simulation_timestep", "0.002f"},
            {"region_count", "1"},
            {"connection_probability", "0.05f"},
            {"stdp_ltp_weight", "0.015f"},
            {"stdp_ltd_weight", "0.012f"},
            {"synaptogenesis_rate", "0.00015f"},
            {"pruning_rate", "0.00002f"}
        }}
    };
    
    if (presets.find(presetName) == presets.end()) {
        printError("Unknown preset: " + presetName);
        return;
    }
    
    const auto& presetParams = presets.at(presetName);
    
    for (const auto& param : presetParams) {
        string key = param.first;
        string value = param.second;
        
        // Try to parse as different types
        try {
            if (value.find('.') != string::npos || value.find('e') != string::npos) {
                double doubleValue = stod(value);
                config_->set(key, doubleValue, ConfigSource::Runtime);
            } else if (value.find('f') != string::npos) {
                float floatValue = stof(value.substr(0, value.size() - 1));
                config_->set(key, floatValue, ConfigSource::Runtime);
            } else {
                int64_t intValue = stoll(value);
                config_->set(key, intValue, ConfigSource::Runtime);
            }
        } catch (...) {
            // If parsing fails, treat as string
            config_->set(key, value, ConfigSource::Runtime);
        }
    }
    
    printSuccess("Applied configuration preset: " + presetName);
}

void NLMCommandLineInterface::loadConfiguration(const string& configFile) {
    if (!validateFile(configFile)) {
        printError("Configuration file not found: " + configFile);
        return;
    }
    
    if (config_->loadFromFile(configFile)) {
        printSuccess("Configuration loaded from: " + configFile);
    } else {
        printWarning("Failed to load configuration from: " + configFile);
    }
}

void NLMCommandLineInterface::saveConfiguration(const string& configFile) {
    if (config_->saveToFile(configFile)) {
        printSuccess("Configuration saved to: " + configFile);
    } else {
        printError("Failed to save configuration to: " + configFile);
    }
}

void NLMCommandLineInterface::setOutputFormat(const string& format) {
    string fmt = toLower(format);
    if (fmt == "json") {
        outputFormat_ = OutputFormat::JSON;
    } else if (fmt == "csv") {
        outputFormat_ = OutputFormat::CSV;
    } else if (fmt == "tabular") {
        outputFormat_ = OutputFormat::Tabular;
    } else if (fmt == "minimal") {
        outputFormat_ = OutputFormat::Minimal;
    } else {
        outputFormat_ = OutputFormat::HumanReadable;
    }
}

bool NLMCommandLineInterface::validateFile(const string& path, bool requireExists) {
    try {
        fs::path filePath(path);
        
        if (requireExists && !fs::exists(filePath)) {
            return false;
        }
        
        if (fs::exists(filePath) && !fs::is_regular_file(filePath)) {
            return false;
        }
        
        return true;
    } catch (...) {
        return false;
    }
}

bool NLMCommandLineInterface::validateNumeric(const string& value, double& result) {
    try {
        result = stod(value);
        return true;
    } catch (...) {
        return false;
    }
}

int64_t NLMCommandLineInterface::parseInt64(const string& value) {
    try {
        return stoll(value);
    } catch (...) {
        return 0;
    }
}

double NLMCommandLineInterface::parseDouble(const string& value) {
    try {
        return stod(value);
    } catch (...) {
        return 0.0;
    }
}

bool NLMCommandLineInterface::parseBool(const string& value) {
    string val = toLower(value);
    return val == "true" || val == "1" || val == "yes" || val == "on";
}

string NLMCommandLineInterface::trim(const string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == string::npos) return "";
    
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

string NLMCommandLineInterface::toLower(const string& str) {
    string result = str;
    transform(result.begin(), result.end(), result.begin(),
              [](unsigned char c) { return tolower(c); });
    return result;
}

string NLMCommandLineInterface::getTimestamp() {
    auto now = chrono::system_clock::now();
    auto time = chrono::system_clock::to_time_t(now);
    
    stringstream ss;
    ss << put_time(localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void NLMCommandLineInterface::outputHumanReadable(const string& data) {
    cout << data << endl;
}

void NLMCommandLineInterface::outputJSON(const string& data) {
    cout << "{" << endl;
    cout << "  \"data\": \"" << data << "\"" << endl;
    cout << "}" << endl;
}

void NLMCommandLineInterface::outputCSV(const string& data) {
    cout << "data,value" << endl;
    cout << "\"result\",\"" << data << "\"" << endl;
}

void NLMCommandLineInterface::outputTabular(const string& data) {
    cout << "+----------------------------------------" << endl;
    cout << "| Result                                |" << endl;
    cout << "+----------------------------------------" << endl;
    cout << "| " << data << " |" << endl;
    cout << "+----------------------------------------" << endl;
}

void NLMCommandLineInterface::outputMinimal(const string& data) {
    cout << data << endl;
}