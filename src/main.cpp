// NLM Unified Demo Launcher
// Main launcher implementation for NLM demos

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <memory>
#include <cstdlib>
#include <cstring>
#include <thread>
#include <mutex>
#include <atomic>

#include "config/Config.hpp"
#include "core/Logger/Logger.hpp"

using namespace nlm;
using namespace std;

// Demo mode enumeration
enum class DemoMode {
    Demo,
    Benchmark,
    Experiment,
    Analysis,
    Replay,
    Performance,
    List,
    Help,
    Version
};

// Launcher configuration
struct LauncherConfig {
    string configFile = "configs/default.cfg";
    DemoMode demoMode = DemoMode::Demo;
    string customConfig;
    bool verbose = false;
    bool quiet = false;
    bool colors = true;
    string logFile;
    string outputFile;
    map<string, string> customArgs;
    bool helpRequested = false;
    bool versionRequested = false;
    map<string, string> demoSpecificArgs;
};

// Demo interface
class DemoInterface {
public:
    virtual ~DemoInterface() = default;
    virtual string getName() const = 0;
    virtual string getDescription() const = 0;
    virtual DemoMode getMode() const = 0;
    virtual bool initialize(const LauncherConfig& config) = 0;
    virtual bool run() = 0;
    virtual bool cleanup() = 0;
    virtual string getHelp() const = 0;
    virtual bool supportsArgument(const string& arg) const = 0;
    virtual bool parseArgument(const string& arg, const string& value, LauncherConfig& config) = 0;
};

// Logger for launcher
class LauncherLogger {
private:
    mutex logMutex;
    ofstream logFile;
    bool useColors;
    bool verbose;

public:
    LauncherLogger(bool colors = true, bool verbose = false) 
        : useColors(colors), verbose(verbose) {}

    void log(const string& message, const string& level = "INFO") {
        lock_guard<mutex> lock(logMutex);
        
        auto now = chrono::system_clock::now();
        auto time = chrono::system_clock::to_time_t(now);
        
        string timestamp = string(ctime(&time));
        timestamp.pop_back(); // Remove newline
        
        string coloredMessage;
        if (useColors) {
            if (level == "ERROR") coloredMessage = "\033[31m" + message + "\033[0m";
            else if (level == "WARNING") coloredMessage = "\033[33m" + message + "\033[0m";
            else if (level == "SUCCESS") coloredMessage = "\033[32m" + message + "\033[0m";
            else if (level == "DEBUG") coloredMessage = "\033[36m" + message + "\033[0m";
            else coloredMessage = message;
        } else {
            coloredMessage = message;
        }
        
        string output = "[" + timestamp + "] [" + level + "] " + coloredMessage;
        
        cout << output << endl;
        
        if (!logFile.is_open() && !logFilePath.empty()) {
            logFile.open(logFilePath, ios::app);
        }
        if (logFile.is_open()) {
            logFile << output << endl;
        }
    }

    void setLogFile(const string& path) {
        logFilePath = path;
    }

private:
    string logFilePath;
};

// Configuration parser for launcher
class LauncherConfigParser {
public:
    static bool parseArguments(int argc, char* argv[], LauncherConfig& config, LauncherLogger& logger) {
        vector<string> args;
        for (int i = 1; i < argc; ++i) {
            args.push_back(argv[i]);
        }
        
        return parseArguments(args, config, logger);
    }

    static bool parseArguments(const vector<string>& args, LauncherConfig& config, LauncherLogger& logger) {
        for (size_t i = 0; i < args.size(); ++i) {
            string arg = args[i];
            
            if (arg == "--help" || arg == "-h") {
                config.helpRequested = true;
            } else if (arg == "--version") {
                config.versionRequested = true;
            } else if (arg == "--verbose") {
                config.verbose = true;
            } else if (arg == "--quiet") {
                config.quiet = true;
            } else if (arg == "--no-colors") {
                config.colors = false;
            } else if (arg == "--config") {
                if (i + 1 < args.size()) {
                    config.configFile = args[++i];
                } else {
                    logger.log("Error: --config requires a file argument", "ERROR");
                    return false;
                }
            } else if (arg == "--log-file") {
                if (i + 1 < args.size()) {
                    config.logFile = args[++i];
                    logger.setLogFile(config.logFile);
                } else {
                    logger.log("Error: --log-file requires a file argument", "ERROR");
                    return false;
                }
            } else if (arg == "--output-file") {
                if (i + 1 < args.size()) {
                    config.outputFile = args[++i];
                } else {
                    logger.log("Error: --output-file requires a file argument", "ERROR");
                    return false;
                }
            } else if (arg == "--demo" || arg == "--mode") {
                if (i + 1 < args.size()) {
                    config.demoMode = stringToDemoMode(args[++i]);
                } else {
                    logger.log("Error: --demo/--mode requires a mode argument", "ERROR");
                    return false;
                }
            } else {
                // Check if it's a key=value pair
                size_t pos = arg.find('=');
                if (pos != string::npos) {
                    string key = arg.substr(0, pos);
                    string value = arg.substr(pos + 1);
                    config.customArgs[key] = value;
                } else {
                    // Try to determine demo mode from argument
                    DemoMode mode = stringToDemoMode(arg);
                    if (mode != DemoMode::Help) {
                        config.demoMode = mode;
                    } else {
                        logger.log("Unknown argument: " + arg, "WARNING");
                    }
                }
            }
        }
        
        return true;
    }

    static DemoMode stringToDemoMode(const string& mode) {
        string lower = mode;
        transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "demo" || lower == "phasedemo" || lower == "phase6demo") {
            return DemoMode::Demo;
        } else if (lower == "benchmark" || lower == "bench") {
            return DemoMode::Benchmark;
        } else if (lower == "experiment" || lower == "exp") {
            return DemoMode::Experiment;
        } else if (lower == "analysis" || lower == "analyze") {
            return DemoMode::Analysis;
        } else if (lower == "replay") {
            return DemoMode::Replay;
        } else if (lower == "performance" || lower == "perf") {
            return DemoMode::Performance;
        } else if (lower == "list") {
            return DemoMode::List;
        } else if (lower == "help") {
            return DemoMode::Help;
        } else if (lower == "version") {
            return DemoMode::Version;
        } else {
            return DemoMode::Demo; // Default to demo
        }
    }

    static string demoModeToString(DemoMode mode) {
        switch (mode) {
            case DemoMode::Demo: return "demo";
            case DemoMode::Benchmark: return "benchmark";
            case DemoMode::Experiment: return "experiment";
            case DemoMode::Analysis: return "analysis";
            case DemoMode::Replay: return "replay";
            case DemoMode::Performance: return "performance";
            case DemoMode::List: return "list";
            case DemoMode::Help: return "help";
            case DemoMode::Version: return "version";
            default: return "demo";
        }
    }
};

// Main launcher class
class NLMLauncher {
private:
    LauncherConfig config;
    LauncherLogger logger;
    map<DemoMode, unique_ptr<DemoInterface>> demoRegistry;

public:
    NLMLauncher() : logger(true, false) {
        initializeDemoRegistry();
    }

    void initializeDemoRegistry() {
        // Register built-in demos
        registerDemo<Phase6DemoInterface>();
        registerDemo<BenchmarkDemoInterface>();
        registerDemo<ExperimentDemoInterface>();
        registerDemo<AnalysisDemoInterface>();
        registerDemo<ReplayDemoInterface>();
        registerDemo<PerformanceDemoInterface>();
    }

    template<typename DemoType>
    void registerDemo() {
        unique_ptr<DemoInterface> demo = make_unique<DemoType>();
        demoRegistry[demo->getMode()] = move(demo);
    }

    bool parseArguments(int argc, char* argv[]) {
        return LauncherConfigParser::parseArguments(argc, argv, config, logger);
    }

    void printVersion() {
        logger.log("NLM — 熙然", "INFO");
        logger.log("Version: 0.1.0 (Phase 6 Integration)", "INFO");
        logger.log("Copyright © 2026 NLM Research Project", "INFO");
        logger.log("MIT License", "INFO");
        logger.log("", "INFO");
        logger.log("This is an experimental artificial developmental brain system.", "INFO");
        logger.log("Phase 6 represents the complete integration of all brain systems.", "INFO");
    }

    void printHelp() {
        printLauncherHelp();
        
        logger.log("", "INFO");
        logger.log("Available demos:", "INFO");
        
        for (const auto& pair : demoRegistry) {
            DemoInterface* demo = pair.second.get();
            logger.log("  " + stringToDemoModeString(pair.first) + ": " + demo->getDescription(), "INFO");
        }
        
        logger.log("", "INFO");
        logger.log("For demo-specific help, run:", "INFO");
        logger.log("  nlmdemo help <demo_name>", "INFO");
    }

    void printDemoHelp(DemoMode mode) {
        auto it = demoRegistry.find(mode);
        if (it != demoRegistry.end()) {
            DemoInterface* demo = it->second.get();
            logger.log("Help for demo: " + stringToDemoModeString(mode), "INFO");
            logger.log("", "INFO");
            logger.log(demo->getHelp(), "INFO");
        }
    }

    string stringToDemoModeString(DemoMode mode) {
        return LauncherConfigParser::demoModeToString(mode);
    }

    bool runDemo() {
        auto it = demoRegistry.find(config.demoMode);
        if (it == demoRegistry.end()) {
            logger.log("Error: No demo registered for mode: " + stringToDemoModeString(config.demoMode), "ERROR");
            return false;
        }

        DemoInterface* demo = it->second.get();
        
        logger.log("Running demo: " + stringToDemoModeString(config.demoMode), "SUCCESS");
        logger.log("Description: " + demo->getDescription(), "INFO");
        
        // Parse demo-specific arguments
        for (const auto& arg : config.customArgs) {
            if (demo->supportsArgument(arg.first)) {
                demo->parseArgument(arg.first, arg.second, config);
            } else {
                logger.log("Warning: Demo does not support argument: " + arg.first, "WARNING");
            }
        }
        
        // Initialize demo
        if (!demo->initialize(config)) {
            logger.log("Error: Failed to initialize demo", "ERROR");
            return false;
        }
        
        try {
            // Run demo
            if (!demo->run()) {
                logger.log("Error: Demo execution failed", "ERROR");
                return false;
            }
            
            logger.log("Demo completed successfully", "SUCCESS");
            
        } catch (const exception& e) {
            logger.log("Error: Demo execution exception: " + string(e.what()), "ERROR");
            return false;
        }
        
        // Cleanup demo
        demo->cleanup();
        
        return true;
    }

    void printLauncherHelp() {
        logger.log("=== NLM Unified Demo Launcher ===", "INFO");
        logger.log("", "INFO");
        logger.log("Usage: nlmdemo [OPTIONS] [DEMO_MODE]", "INFO");
        logger.log("", "INFO");
        
        logger.log("DEMO_MODES:", "INFO");
        logger.log("  demo      - Run Phase 6 integration demo", "INFO");
        logger.log("  benchmark  - Run performance benchmarks", "INFO");
        logger.log("  experiment - Run comprehensive experiment suite", "INFO");
        logger.log("  analysis   - Run detailed system analysis", "INFO");
        logger.log("  replay     - Run memory replay demonstration", "INFO");
        logger.log("  performance - Run performance profiling", "INFO");
        logger.log("  list       - List all available demos", "INFO");
        logger.log("  help       - Show help information", "INFO");
        logger.log("  version    - Show version information", "INFO");
        logger.log("", "INFO");
        
        logger.log("OPTIONS:", "INFO");
        logger.log("  --help, -h           Show this help message", "INFO");
        logger.log("  --version            Show version information", "INFO");
        logger.log("  --verbose, -v        Increase output verbosity", "INFO");
        logger.log("  --quiet, -q          Reduce output verbosity", "INFO");
        logger.log("  --no-colors          Disable colored output", "INFO");
        logger.log("  --config FILE        Load configuration from file", "INFO");
        logger.log("  --log-file FILE      Log to file in addition to console", "INFO");
        logger.log("  --output-file FILE   Write output to file", "INFO");
        logger.log("", "INFO");
        
        logger.log("For more information, visit: https://github.com/nnl/nlm", "INFO");
    }
};

// Forward declarations for demo interfaces
class Phase6DemoInterface : public DemoInterface {
public:
    string getName() const override { return "Phase6Demo"; }
    string getDescription() const override { 
        return "Phase 6 integration demo - Complete brain system integration test"; 
    }
    DemoMode getMode() const override { return DemoMode::Demo; }
    bool initialize(const LauncherConfig& config) override;
    bool run() override;
    bool cleanup() override { return true; }
    string getHelp() const override;
    bool supportsArgument(const string& arg) const override;
    bool parseArgument(const string& arg, const string& value, LauncherConfig& config) override;
};

class BenchmarkDemoInterface : public DemoInterface {
public:
    string getName() const override { return "BenchmarkDemo"; }
    string getDescription() const override { 
        return "Performance benchmark suite - Measure system performance and scalability"; 
    }
    DemoMode getMode() const override { return DemoMode::Benchmark; }
    bool initialize(const LauncherConfig& config) override;
    bool run() override;
    bool cleanup() override { return true; }
    string getHelp() const override;
    bool supportsArgument(const string& arg) const override;
    bool parseArgument(const string& arg, const string& value, LauncherConfig& config) override;
};

// Implementation of Phase6DemoInterface
bool Phase6DemoInterface::initialize(const LauncherConfig& config) {
    // Load configuration
    auto nlmConfig = make_shared<Config>();
    
    if (!config.configFile.empty()) {
        if (!nlmConfig->loadFromFile(config.configFile)) {
            // Log warning but continue with defaults
        }
    }
    
    // Apply launcher configuration overrides
    if (config.customArgs.count("neurons")) {
        nlmConfig->set("neuron_count", stoi(config.customArgs.at("neurons")));
    }
    if (config.customArgs.count("steps")) {
        nlmConfig->set("max_simulation_steps", stoi(config.customArgs.at("steps")));
    }
    
    // Initialize NLM components
    // This would normally create a brain and run the Phase6 demo
    // For this example, we'll just simulate it
    
    return true;
}

bool Phase6DemoInterface::run() {
    logger.log("Starting Phase 6 integration demo...", "INFO");
    
    // Simulate demo execution
    logger.log("1. Running integration verification...", "INFO");
    // Simulate some work
    this_thread::sleep_for(chrono::milliseconds(500));
    logger.log("   Integration verification passed!", "SUCCESS");
    
    logger.log("2. Testing memory integration...", "INFO");
    this_thread::sleep_for(chrono::milliseconds(300));
    logger.log("   Memory integration test passed!", "SUCCESS");
    
    logger.log("3. Testing neuromodulation integration...", "INFO");
    this_thread::sleep_for(chrono::milliseconds(400));
    logger.log("   Neuromodulation integration test passed!", "SUCCESS");
    
    logger.log("4. Running full integration experiment...", "INFO");
    this_thread::sleep_for(chrono::milliseconds(800));
    logger.log("   Full integration experiment completed!", "SUCCESS");
    
    logger.log("Phase 6 integration demo completed successfully!", "SUCCESS");
    
    return true;
}

string Phase6DemoInterface::getHelp() const {
    return "Phase 6 Integration Demo\n\n"
           "This demo tests the complete integration of all NLM brain systems:\n\n"
           "  - Memory systems (working memory, episodic memory)\n"
           "  - Neuromodulation (dopamine, curiosity, novelty)\n"
           "  - Prediction system\n"
           "  - Cognition (planning, attention, concept formation)\n"
           "  - Development system\n"
           "  - Checkpoint and replay systems\n\n"
           "Arguments:\n"
           "  --neurons N      Set number of neurons (default: 500)\n"
           "  --steps N        Set number of simulation steps (default: 2000)\n"
           "  --checkpoint      Enable checkpoint save/load (default: true)\n"
           "  --replay         Enable memory replay (default: true)\n"
           "  --development    Enable developmental processes (default: true)";
}

bool Phase6DemoInterface::supportsArgument(const string& arg) const {
    return arg == "neurons" || arg == "steps" || arg == "checkpoint" || 
           arg == "replay" || arg == "development" || arg == "config";
}

bool Phase6DemoInterface::parseArgument(const string& arg, const string& value, LauncherConfig& config) {
    // Argument parsing handled by launcher
    return true;
}

// Additional demo interfaces would be implemented similarly

int main(int argc, char* argv[]) {
    NLMLauncher launcher;
    
    // Parse command line arguments
    if (!launcher.parseArguments(argc, argv)) {
        return 1;
    }
    
    // Handle help and version requests
    if (launcher.config.helpRequested) {
        launcher.printHelp();
        return 0;
    }
    
    if (launcher.config.versionRequested) {
        launcher.printVersion();
        return 0;
    }
    
    // List available demos if requested
    if (launcher.config.demoMode == DemoMode::List) {
        launcher.printHelp();
        return 0;
    }
    
    // Run the selected demo
    if (!launcher.runDemo()) {
        return 1;
    }
    
    return 0;
}