// Config command implementation

#include "Command.hpp"
#include <iostream>

namespace nlm {
namespace cli {

class ConfigCommand : public Command {
public:
    std::string getName() const override { return "config"; }
    std::string getDescription() const override { 
        return "Manage configuration files and settings"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::CONFIGURATION; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for config command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid config options");
            return false;
        }
        
        StatusReporter::info("Managing configuration files...");
        
        // List configuration if requested
        if (args.empty() || args[0] == "--list") {
            StatusReporter::info("Current configuration:");
            StatusReporter::info("  Config file: configs/default.cfg");
            StatusReporter::info("  Environment variables loaded");
        }
        
        // Load configuration if specified
        if (hasArg("--config-file", args)) {
            std::string configFile = getArgValue("--config-file", args);
            StatusReporter::info("Loading configuration from: " + configFile);
            if (!loadConfigFile(configFile)) {
                StatusReporter::error("Failed to load configuration file");
                return false;
            }
        }
        
        // Export configuration if requested
        if (hasArg("--output-file", args)) {
            std::string outputFile = getArgValue("--output-file", args);
            StatusReporter::info("Exporting configuration to: " + outputFile);
            if (!exportConfigFile(outputFile)) {
                StatusReporter::error("Failed to export configuration file");
                return false;
            }
        }
        
        // Validate configuration if requested
        if (hasArg("--validate", args)) {
            StatusReporter::info("Validating configuration...");
            if (!validateConfig()) {
                StatusReporter::error("Configuration validation failed");
                return false;
            }
        }
        
        StatusReporter::success("Configuration command completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: config");
        StatusReporter::info("Description: Manage configuration files and settings");
        StatusReporter::info("Usage: nlm config [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --list                List current configuration");
        StatusReporter::info("  --config-file FILE     Configuration file to load");
        StatusReporter::info("  --output-file FILE     Configuration file to export to");
        StatusReporter::info("  --validate            Validate current configuration");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm config --list");
        StatusReporter::info("  nlm config --config-file myconfig.cfg --validate");
        StatusReporter::info("  nlm config --output-file exported.cfg");
    }
    
    std::string getUsage() const override {
        return "nlm config [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm config --list",
            "nlm config --config-file myconfig.cfg --validate",
            "nlm config --output-file exported.cfg"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        // Parse command line arguments
        // This is a simplified implementation
        for (const auto& arg : args) {
            if (arg == "--list") {
                listConfig = true;
            } else if (arg == "--validate") {
                validateConfig = true;
            } else if (arg == "--export-config") {
                exportConfig = true;
            }
        }
    }
    
    bool validateArguments() const {
        // Simple validation
        if (listConfig && validateConfig && exportConfig) {
            StatusReporter::warning("Multiple actions requested; performing all actions");
        }
        return true;
    }
    
    bool hasArg(const std::string& option, const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == option) {
                return true;
            }
        }
        return false;
    }
    
    std::string getArgValue(const std::string& option, const std::vector<std::string>& args) {
        for (size_t i = 0; i < args.size() - 1; ++i) {
            if (args[i] == option) {
                return args[i + 1];
            }
        }
        return "";
    }
    
    bool loadConfigFile(const std::string& filename) {
        StatusReporter::info("Loading configuration from: " + filename);
        // In a real implementation, this would load the configuration file
        return true;
    }
    
    bool exportConfigFile(const std::string& filename) {
        StatusReporter::info("Exporting configuration to: " + filename);
        // In a real implementation, this would export the configuration file
        return true;
    }
    
    bool validateConfig() {
        StatusReporter::info("Validating configuration...");
        // In a real implementation, this would validate the configuration
        return true;
    }
    
    bool listConfig = false;
    bool validateConfig = false;
    bool exportConfig = false;
};

// Debug command implementation
class DebugCommand : public Command {
public:
    std::string getName() const override { return "debug"; }
    std::string getDescription() const override { 
        return "Debug and profiling tools for neural simulations"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::DEBUGGING; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for debug command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid debug options");
            return false;
        }
        
        StatusReporter::info("Starting debug session...");
        
        // Attach debugger if requested
        if (attachDebugger) {
            StatusReporter::info("Attaching debugger to running process...");
            // In a real implementation, this would attach to the debugger
        }
        
        // Profile if requested
        if (profile) {
            StatusReporter::info("Starting performance profiling...");
            // In a real implementation, this would start profiling
        }
        
        // Validate if requested
        if (validate) {
            StatusReporter::info("Running system validation...");
            // In a real implementation, this would validate the system
        }
        
        // Set memory debugging if requested
        if (memoryDebug) {
            StatusReporter::info("Enabling memory debugging...");
            // In a real implementation, this would enable memory debugging
        }
        
        // Set thread debugging if requested
        if (threadDebug) {
            StatusReporter::info("Enabling thread debugging...");
            // In a real implementation, this would enable thread debugging
        }
        
        StatusReporter::success("Debug session completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: debug");
        StatusReporter::info("Description: Debug and profiling tools for neural simulations");
        StatusReporter::info("Usage: nlm debug [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --attach              Attach debugger to running process");
        StatusReporter::info("  --profile, -p         Enable performance profiling");
        StatusReporter::info("  --validate, -v        Validate system state");
        StatusReporter::info("  --memory              Enable memory debugging");
        StatusReporter::info("  --threads             Enable thread debugging");
        StatusReporter::info("  --log-level LEVEL     Set log level for debugging");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm debug --attach");
        StatusReporter::info("  nlm debug --profile --log-level debug");
        StatusReporter::info("  nlm debug --validate --memory");
    }
    
    std::string getUsage() const override {
        return "nlm debug [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm debug --attach",
            "nlm debug --profile --log-level debug",
            "nlm debug --validate --memory"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == "--attach") {
                attachDebugger = true;
            } else if (arg == "--profile" || arg == "-p") {
                profile = true;
            } else if (arg == "--validate" || arg == "-v") {
                validate = true;
            } else if (arg == "--memory") {
                memoryDebug = true;
            } else if (arg == "--threads") {
                threadDebug = true;
            } else if (arg == "--log-level" && !logLevel.empty()) {
                // Log level handling would be done elsewhere
            }
        }
    }
    
    bool validateArguments() const {
        // Validate log level if set
        if (!logLevel.empty()) {
            if (logLevel != "error" && logLevel != "warning" && 
                logLevel != "info" && logLevel != "debug") {
                StatusReporter::error("Invalid log level: " + logLevel);
                return false;
            }
        }
        
        return true;
    }
    
    bool attachDebugger = false;
    bool profile = false;
    bool validate = false;
    bool memoryDebug = false;
    bool threadDebug = false;
    std::string logLevel;
};

// Validate command implementation
class ValidateCommand : public Command {
public:
    std::string getName() const override { return "validate"; }
    std::string getDescription() const override { 
        return "System validation and testing commands"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::VALIDATION; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for validate command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid validate options");
            return false;
        }
        
        StatusReporter::info("Running system validation...");
        
        // Check memory if requested
        if (checkMemory) {
            StatusReporter::info("Checking memory allocation...");
            // In a real implementation, this would check memory allocation
        }
        
        // Check threads if requested
        if (checkThreads) {
            StatusReporter::info("Checking thread safety...");
            // In a real implementation, this would check thread safety
        }
        
        // Check configuration if requested
        if (checkConfig) {
            StatusReporter::info("Checking configuration...");
            // In a real implementation, this would check configuration
        }
        
        // Check plugins if requested
        if (checkPlugins) {
            StatusReporter::info("Checking plugins...");
            // In a real implementation, this would check plugins
        }
        
        StatusReporter::success("System validation completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: validate");
        StatusReporter::info("Description: System validation and testing commands");
        StatusReporter::info("Usage: nlm validate [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --memory              Check memory allocation");
        StatusReporter::info("  --threads             Check thread safety");
        StatusReporter::info("  --config              Validate configuration");
        StatusReporter::info("  --plugins             Check plugins");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm validate --memory --threads");
        StatusReporter::info("  nlm validate --config --plugins");
    }
    
    std::string getUsage() const override {
        return "nlm validate [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm validate --memory --threads",
            "nlm validate --config --plugins"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == "--memory") {
                checkMemory = true;
            } else if (arg == "--threads") {
                checkThreads = true;
            } else if (arg == "--config") {
                checkConfig = true;
            } else if (arg == "--plugins") {
                checkPlugins = true;
            }
        }
    }
    
    bool validateArguments() const {
        return true;
    }
    
    bool checkMemory = false;
    bool checkThreads = false;
    bool checkConfig = false;
    bool checkPlugins = false;
};

// Plugin command implementation
class PluginCommand : public Command {
public:
    std::string getName() const override { return "plugin"; }
    std::string getDescription() const override { 
        return "Plugin management commands"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::PLUGIN; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for plugin command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid plugin options");
            return false;
        }
        
        StatusReporter::info("Managing plugins...");
        
        // List plugins if requested
        if (listPlugins) {
            StatusReporter::info("Available plugins:");
            // In a real implementation, this would list available plugins
        }
        
        // Load plugins if requested
        if (!loadPath.empty()) {
            StatusReporter::info("Loading plugins from: " + loadPath);
            // In a real implementation, this would load plugins
        }
        
        // Unload plugin if requested
        if (!unloadName.empty()) {
            StatusReporter::info("Unloading plugin: " + unloadName);
            // In a real implementation, this would unload the plugin
        }
        
        // Enable plugin if requested
        if (!enableName.empty()) {
            StatusReporter::info("Enabling plugin: " + enableName);
            // In a real implementation, this would enable the plugin
        }
        
        // Disable plugin if requested
        if (!disableName.empty()) {
            StatusReporter::info("Disabling plugin: " + disableName);
            // In a real implementation, this would disable the plugin
        }
        
        // Show plugin info if requested
        if (!infoName.empty()) {
            StatusReporter::info("Showing plugin info: " + infoName);
            // In a real implementation, this would show plugin info
        }
        
        StatusReporter::success("Plugin command completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: plugin");
        StatusReporter::info("Description: Plugin management commands");
        StatusReporter::info("Usage: nlm plugin [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --list                List available plugins");
        StatusReporter::info("  --load PATH           Load plugins from directory");
        StatusReporter::info("  --unload NAME         Unload specific plugin");
        StatusReporter::info("  --enable NAME         Enable plugin");
        StatusReporter::info("  --disable NAME        Disable plugin");
        StatusReporter::info("  --info NAME           Show plugin information");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm plugin --list");
        StatusReporter::info("  nlm plugin --load ./plugins");
        StatusReporter::info("  nlm plugin --info MyPlugin");
    }
    
    std::string getUsage() const override {
        return "nlm plugin [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm plugin --list",
            "nlm plugin --load ./plugins",
            "nlm plugin --info MyPlugin"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == "--list") {
                listPlugins = true;
            } else if (arg == "--load" && !loadPath.empty()) {
                // Load path handling would be done elsewhere
            } else if (arg == "--unload" && !unloadName.empty()) {
                // Unload name handling would be done elsewhere
            } else if (arg == "--enable" && !enableName.empty()) {
                // Enable name handling would be done elsewhere
            } else if (arg == "--disable" && !disableName.empty()) {
                // Disable name handling would be done elsewhere
            } else if (arg == "--info" && !infoName.empty()) {
                // Info name handling would be done elsewhere
            }
        }
    }
    
    bool validateArguments() const {
        return true;
    }
    
    bool listPlugins = false;
    std::string loadPath;
    std::string unloadName;
    std::string enableName;
    std::string disableName;
    std::string infoName;
};

// Profile command implementation
class ProfileCommand : public Command {
public:
    std::string getName() const override { return "profile"; }
    std::string getDescription() const override { 
        return "Performance profiling and benchmarking"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::PERFORMANCE; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for profile command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid profile options");
            return false;
        }
        
        StatusReporter::info("Starting performance profiling...");
        
        // Run benchmark if requested
        if (benchmark) {
            StatusReporter::info("Running benchmark mode...");
            // In a real implementation, this would run the benchmark
        }
        
        // Run profiler if requested
        if (profile) {
            StatusReporter::info("Running profiler...");
            // In a real implementation, this would run the profiler
        }
        
        // Run analysis if requested
        if (analyze) {
            StatusReporter::info("Running analysis...");
            // In a real implementation, this would run the analysis
        }
        
        // Set output file if requested
        if (!outputFile.empty()) {
            StatusReporter::info("Output results to: " + outputFile);
            // In a real implementation, this would set the output file
        }
        
        StatusReporter::success("Profiling command completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: profile");
        StatusReporter::info("Description: Performance profiling and benchmarking");
        StatusReporter::info("Usage: nlm profile [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --benchmark, -b       Run benchmark");
        StatusReporter::info("  --profile, -p         Run profiler");
        StatusReporter::info("  --analyze, -a         Run analysis");
        StatusReporter::info("  --output-file FILE     Output results to file");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm profile --benchmark");
        StatusReporter::info("  nlm profile --profile --output-file perf.json");
        StatusReporter::info("  nlm profile --analyze --output-format json");
    }
    
    std::string getUsage() const override {
        return "nlm profile [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm profile --benchmark",
            "nlm profile --profile --output-file perf.json",
            "nlm profile --analyze --output-format json"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == "--benchmark" || arg == "-b") {
                benchmark = true;
            } else if (arg == "--profile" || arg == "-p") {
                profile = true;
            } else if (arg == "--analyze" || arg == "-a") {
                analyze = true;
            } else if (arg == "--output-file" && !outputFile.empty()) {
                // Output file handling would be done elsewhere
            }
        }
    }
    
    bool validateArguments() const {
        return true;
    }
    
    bool benchmark = false;
    bool profile = false;
    bool analyze = false;
    std::string outputFile;
};

// Benchmark command implementation
class BenchmarkCommand : public Command {
public:
    std::string getName() const override { return "benchmark"; }
    std::string getDescription() const override { 
        return "Performance benchmarking of neural simulations"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::PERFORMANCE; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for benchmark command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid benchmark options");
            return false;
        }
        
        StatusReporter::info("Running neural network benchmarks...");
        
        // Run benchmark with specified parameters
        StatusReporter::info("Benchmark configuration:");
        StatusReporter::info("  Neurons: " + std::to_string(neuronCount));
        StatusReporter::info("  Steps: " + std::to_string(steps));
        StatusReporter::info("  Threads: " + std::to_string(threads));
        
        StatusReporter::success("Benchmark command completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: benchmark");
        StatusReporter::info("Description: Performance benchmarking of neural simulations");
        StatusReporter::info("Usage: nlm benchmark [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --neuron-count N      Number of neurons (default: 1000)");
        StatusReporter::info("  --steps N             Number of simulation steps (default: 1000)");
        StatusReporter::info("  --threads N           Number of threads (default: 1)");
        StatusReporter::info("  --output-file FILE     Output results to file");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm benchmark --neuron-count 2000 --steps 500");
        StatusReporter::info("  nlm benchmark --threads 4 --output-file results.csv");
    }
    
    std::string getUsage() const override {
        return "nlm benchmark [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm benchmark --neuron-count 2000 --steps 500",
            "nlm benchmark --threads 4 --output-file results.csv"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == "--neuron-count" && neuronCount == 1000) {
                // Neuron count handling would be done elsewhere
            } else if (arg == "--steps" && steps == 1000) {
                // Steps handling would be done elsewhere
            } else if (arg == "--threads" && threads == 1) {
                // Threads handling would be done elsewhere
            } else if (arg == "--output-file" && !outputFile.empty()) {
                // Output file handling would be done elsewhere
            }
        }
    }
    
    bool validateArguments() const {
        if (neuronCount <= 0) {
            StatusReporter::error("Neuron count must be positive");
            return false;
        }
        
        if (steps <= 0) {
            StatusReporter::error("Steps must be positive");
            return false;
        }
        
        if (threads <= 0) {
            StatusReporter::error("Threads must be positive");
            return false;
        }
        
        return true;
    }
    
    int neuronCount = 1000;
    int steps = 1000;
    int threads = 1;
    std::string outputFile;
};

// Analyze command implementation
class AnalyzeCommand : public Command {
public:
    std::string getName() const override { return "analyze"; }
    std::string getDescription() const override { 
        return "System analysis and reporting"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::ANALYSIS; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for analyze command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid analyze options");
            return false;
        }
        
        StatusReporter::info("Running system analysis...");
        
        // Set analysis type
        StatusReporter::info("Analysis type: " + analysisType);
        
        // Set output format
        if (!outputFormat.empty()) {
            StatusReporter::info("Output format: " + outputFormat);
        }
        
        // Set output file
        if (!outputFile.empty()) {
            StatusReporter::info("Output file: " + outputFile);
        }
        
        // Run analysis
        StatusReporter::info("Running analysis...");
        // In a real implementation, this would run the analysis
        
        StatusReporter::success("Analysis command completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: analyze");
        StatusReporter::info("Description: System analysis and reporting");
        StatusReporter::info("Usage: nlm analyze [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --type TYPE           Analysis type: performance, memory, network");
        StatusReporter::info("  --output-file FILE    Output results to file");
        StatusReporter::info("  --output-format FMT   Output format: text, json, csv");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm analyze --type performance");
        StatusReporter::info("  nlm analyze --type memory --output-format json");
        StatusReporter::info("  nlm analyze --type network --output-file network_stats.csv");
    }
    
    std::string getUsage() const override {
        return "nlm analyze [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm analyze --type performance",
            "nlm analyze --type memory --output-format json",
            "nlm analyze --type network --output-file network_stats.csv"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == "--type" && !analysisType.empty()) {
                // Analysis type handling would be done elsewhere
            } else if (arg == "--output-format" && !outputFormat.empty()) {
                // Output format handling would be done elsewhere
            } else if (arg == "--output-file" && !outputFile.empty()) {
                // Output file handling would be done elsewhere
            }
        }
    }
    
    bool validateArguments() const {
        if (analysisType != "performance" && analysisType != "memory" && 
            analysisType != "network" && analysisType != "system") {
            StatusReporter::error("Invalid analysis type: " + analysisType);
            return false;
        }
        
        return true;
    }
    
    std::string analysisType = "performance";
    std::string outputFormat = "text";
    std::string outputFile;
};

// Utility command implementation
class UtilityCommand : public Command {
public:
    std::string getName() const override { return "utility"; }
    std::string getDescription() const override { 
        return "Utility commands and tools"; 
    }
    CommandCategory getCategory() const override { return CommandCategory::UTILITIES; }
    
    bool execute(const std::vector<std::string>& args) override {
        // Parse command line arguments for utility command
        parseArguments(args);
        
        if (!validateArguments()) {
            StatusReporter::error("Invalid utility options");
            return false;
        }
        
        StatusReporter::info("Running utility command...");
        
        // Run conversion if requested
        if (convert) {
            StatusReporter::info("Running conversion utility...");
            // In a real implementation, this would run the conversion
        }
        
        // Run validation if requested
        if (validate) {
            StatusReporter::info("Running validation utility...");
            // In a real implementation, this would run the validation
        }
        
        // Show format info if requested
        if (showFormatInfo) {
            StatusReporter::info("Format information:");
            // In a real implementation, this would show format information
        }
        
        StatusReporter::success("Utility command completed");
        return true;
    }
    
    void showHelp() const override {
        StatusReporter::info("Command: utility");
        StatusReporter::info("Description: Utility commands and tools");
        StatusReporter::info("Usage: nlm utility [OPTIONS]");
        StatusReporter::info("")
        StatusReporter::info("Options:");
        StatusReporter::info("  --convert             Run conversion utility");
        StatusReporter::info("  --validate            Run validation utility");
        StatusReporter::info("  --format-info         Show supported formats");
        StatusReporter::info("  --help, -h            Show this help message");
        StatusReporter::info("")
        StatusReporter::info("Examples:");
        StatusReporter::info("  nlm utility --convert");
        StatusReporter::info("  nlm utility --validate");
    }
    
    std::string getUsage() const override {
        return "nlm utility [OPTIONS]";
    }
    
    std::vector<std::string> getExamples() const override {
        return {
            "nlm utility --convert",
            "nlm utility --validate"
        };
    }
    
private:
    void parseArguments(const std::vector<std::string>& args) {
        for (const auto& arg : args) {
            if (arg == "--convert") {
                convert = true;
            } else if (arg == "--validate") {
                validate = true;
            } else if (arg == "--format-info") {
                showFormatInfo = true;
            }
        }
    }
    
    bool validateArguments() const {
        return true;
    }
    
    bool convert = false;
    bool validate = false;
    bool showFormatInfo = false;
};

} // namespace cli
} // namespace nlm
