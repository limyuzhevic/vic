#pragma once

/**
 * Unified CLI Framework for NLM Demos
 * 
 * Provides:
 * - Common command-line argument parsing
 * - Configuration file integration
 * - Parameter validation
 * - Help system
 * - Version information
 */

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <memory>
#include <chrono>
#include <ctime>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "core/Config/UnifiedConfig.hpp"

namespace nlm {

/**
 * CLI option definition
 */
struct CLIOption {
    std::string longName;
    char shortName;
    std::string description;
    bool hasArgument;
    std::string defaultValue;
    std::string type;
    
    CLIOption(const std::string& longN, char shortN, const std::string& desc,
              bool hasArg = false, const std::string& defaultVal = "",
              const std::string& optType = "string")
        : longName(longN), shortName(shortN), description(desc),
          hasArgument(hasArg), defaultValue(defaultVal), type(optType) {}
};

/**
 * CLI framework for demo applications
 */
class CLI {
private:
    std::vector<CLIOption> options_;
    std::map<std::string, std::string> parsedArgs_;
    std::string programName_;
    std::string version_;
    std::string description_;
    
public:
    CLI(const std::string& name, const std::string& ver, const std::string& desc)
        : programName_(name), version_(ver), description_(desc) {
        initializeDefaultOptions();
    }
    
    /**
     * Add a CLI option
     */
    void addOption(const CLIOption& option) {
        options_.push_back(option);
    }
    
    /**
     * Parse command line arguments
     */
    void parseArgs(int argc, char* argv[]) {
        parsedArgs_.clear();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Check for help/version flags
            if (arg == "--help" || arg == "-h") {
                showHelp();
                exit(0);
            } else if (arg == "--version" || arg == "-v") {
                showVersion();
                exit(0);
            }
            
            // Check for --key=value or --key value format
            if (arg.find("--") == 0) {
                size_t eqPos = arg.find('=');
                if (eqPos != std::string::npos) {
                    // --key=value format
                    std::string key = arg.substr(2, eqPos - 2);
                    std::string value = arg.substr(eqPos + 1);
                    parsedArgs_[key] = value;
                } else if (i + 1 < argc && argv[i + 1][0] != '-') {
                    // --key value format
                    std::string key = arg.substr(2);
                    std::string value = argv[++i];
                    parsedArgs_[key] = value;
                } else {
                    // Boolean flag
                    std::string key = arg.substr(2);
                    parsedArgs_[key] = "true";
                }
            } else if (arg.find("-") == 0 && arg.length() == 2) {
                // Single character flag
                char flag = arg[1];
                auto optionIt = std::find_if(options_.begin(), options_.end(),
                    [flag](const CLIOption& opt) { return opt.shortName == flag; });
                
                if (optionIt != options_.end()) {
                    if (optionIt->hasArgument && i + 1 < argc && argv[i + 1][0] != '-') {
                        // -k value format
                        std::string key = optionIt->longName;
                        std::string value = argv[++i];
                        parsedArgs_[key] = value;
                    } else {
                        // Boolean flag
                        std::string key = optionIt->longName;
                        parsedArgs_[key] = "true";
                    }
                }
            }
        }
    }
    
    /**
     * Get parsed argument value
     */
    std::string getArg(const std::string& key) const {
        auto it = parsedArgs_.find(key);
        return it != parsedArgs_.end() ? it->second : "";
    }
    
    /**
     * Get argument as boolean
     */
    bool getBoolArg(const std::string& key) const {
        std::string value = getArg(key);
        return value == "true" || value == "1" || value == "yes";
    }
    
    /**
     * Get argument as integer
     */
    int getIntArg(const std::string& key) const {
        std::string value = getArg(key);
        try {
            return std::stoi(value);
        } catch (...) {
            return 0;
        }
    }
    
    /**
     * Get argument as float
     */
    float getFloatArg(const std::string& key) const {
        std::string value = getArg(key);
        try {
            return std::stof(value);
        } catch (...) {
            return 0.0f;
        }
    }
    
    /**
     * Get argument as double
     */
    double getDoubleArg(const std::string& key) const {
        std::string value = getArg(key);
        try {
            return std::stod(value);
        } catch (...) {
            return 0.0;
        }
    }
    
    /**
     * Check if argument was provided
     */
    bool hasArg(const std::string& key) const {
        return parsedArgs_.find(key) != parsedArgs_.end();
    }
    
    /**
     * Load configuration from parsed arguments
     */
    void loadConfig(std::shared_ptr<Config> config) {
        // Load from config file if specified
        std::string configFile = getArg("config");
        if (!configFile.empty()) {
            config->loadFromFile(configFile);
        }
        
        // Override with command line arguments
        for (const auto& pair : parsedArgs_) {
            if (pair.first != "help" && pair.first != "version") {
                config->setFromString(pair.first, pair.second, "command_line");
            }
        }
    }
    
    /**
     * Show help information
     */
    void showHelp() const {
        std::cout << "Usage: " << programName_ << " [OPTIONS]" << std::endl << std::endl;
        std::cout << description_ << std::endl << std::endl;
        std::cout << "Options:" << std::endl;
        
        for (const auto& option : options_) {
            std::cout << "  ";
            if (option.shortName != '\0') {
                std::cout << "-" << option.shortName << ", ";
            }
            std::cout << "--" << option.longName;
            
            if (option.hasArgument) {
                std::cout << "=" << option.type;
            }
            
            std::cout << std::endl;
            std::cout << "    " << option.description << std::endl;
            if (!option.defaultValue.empty()) {
                std::cout << "    Default: " << option.defaultValue << std::endl;
            }
            std::cout << std::endl;
        }
        
        std::cout << "Commonly used options:" << std::endl;
        std::cout << "  -h, --help                    Show this help message" << std::endl;
        std::cout << "  -v, --version                 Show version information" << std::endl;
        std::cout << "  --config=FILE                 Load configuration from file" << std::endl;
        std::cout << "  --help-all                    Show all available options" << std::endl;
        std::cout << std::endl;
        
        std::cout << "For more information, visit: https://github.com/nlm-project/nlm" << std::endl;
    }
    
    /**
     * Show version information
     */
    void showVersion() const {
        std::cout << programName_ << " version " << version_ << std::endl;
        std::cout << "Built with C++20" << std::endl;
        std::cout << "NLM (熙然) - Neural Learning Machine" << std::endl;
    }
    
protected:
    virtual void initializeDefaultOptions() {
        // Common options for all demos
        options_.emplace_back("help", 'h', "Show help message", false, "", "flag");
        options_.emplace_back("version", 'v', "Show version information", false, "", "flag");
        options_.emplace_back("config", '\0', "Load configuration from file", true, "default.cfg", "filename");
        
        // Debug and logging options
        options_.emplace_back("verbose", 'V', "Enable verbose output", false, "false", "flag");
        options_.emplace_back("quiet", 'q', "Suppress non-essential output", false, "false", "flag");
        options_.emplace_back("log-level", '\0', "Set log level", true, "INFO", "string");
        options_.emplace_back("log-file", '\0', "Log to file", true, "", "filename");
        
        // Experiment control options
        options_.emplace_back("episodes", 'e', "Number of episodes (Phase 3)", true, "5", "int");
        options_.emplace_back("steps", 's', "Number of steps per episode", true, "200", "int");
        options_.emplace_back("trials", '\0', "Number of trials (Phase 4)", true, "50", "int");
        options_.emplace_back("max-steps", '\0', "Maximum simulation steps", true, "10000", "int");
    }
};

/**
 * Base class for demo applications
 */
class DemoApp {
protected:
    std::shared_ptr<Config> config_;
    std::unique_ptr<CLI> cli_;
    bool verbose_ = false;
    bool quiet_ = false;
    
public:
    DemoApp(const std::string& name, const std::string& version, const std::string& desc)
        : cli_(std::make_unique<CLI>(name, version, desc)) {
        config_ = std::make_shared<Config>();
    }
    
    virtual ~DemoApp() = default;
    
    /**
     * Parse command line arguments
     */
    virtual void parseArgs(int argc, char* argv[]) {
        cli_->parseArgs(argc, argv);
        
        // Set verbose/quiet flags
        verbose_ = cli_->getBoolArg("verbose");
        quiet_ = cli_->getBoolArg("quiet");
        
        // Load configuration
        cli_->loadConfig(config_);
        
        // Apply config to brain if needed
        configureFromConfig();
    }
    
    /**
     * Configure application from config
     */
    virtual void configureFromConfig() = 0;
    
    /**
     * Run the demo
     */
    virtual int run(int argc, char* argv[]) {
        parseArgs(argc, argv);
        return execute();
    }
    
    /**
     * Execute the demo logic
     */
    virtual int execute() = 0;
    
    /**
     * Get configuration
     */
    std::shared_ptr<Config> getConfig() const { return config_; }
    
    /**
     * Get CLI
     */
    CLI* getCLI() const { return cli_.get(); }
    
    /**
     * Print output with appropriate formatting
     */
    void print(const std::string& message) {
        if (quiet_) return;
        std::cout << message << std::endl;
    }
    
    /**
     * Print verbose output
     */
    void printVerbose(const std::string& message) {
        if (verbose_) {
            std::cout << "[VERBOSE] " << message << std::endl;
        }
    }
    
    /**
     * Print error message
     */
    void printError(const std::string& message) {
        std::cerr << "[ERROR] " << message << std::endl;
    }
    
    /**
     * Print warning message
     */
    void printWarning(const std::string& message) {
        std::cerr << "[WARNING] " << message << std::endl;
    }
    
    /**
     * Print info message
     */
    void printInfo(const std::string& message) {
        if (!quiet_) {
            std::cout << "[INFO] " << message << std::endl;
        }
    }
    
    /**
     * Get current timestamp
     */
    std::string getTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

/**
 * Macro for adding CLI options to demo apps
 */
#define ADD_CLI_OPTION(app, longName, shortName, description, hasArg, defaultVal, type) \
    app.getCLI()->addOption(CLI::CLIOption(longName, shortName, description, hasArg, defaultVal, type))

/**
 * Performance monitoring utilities
 */
class PerformanceMonitor {
private:
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::time_point endTime_;
    std::vector<double> timings_;
    std::string currentTask_;
    
public:
    PerformanceMonitor() {
        reset();
    }
    
    void reset() {
        startTime_ = std::chrono::steady_clock::now();
        timings_.clear();
        currentTask_ = "";
    }
    
    void startTask(const std::string& taskName) {
        currentTask_ = taskName;
        startTime_ = std::chrono::steady_clock::now();
    }
    
    void endTask() {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime_);
        timings_.push_back(duration.count());
        std::cout << "Task \"" << currentTask_ << "\": " << duration.count() << " ms" << std::endl;
        currentTask_ = "";
    }
    
    double getTotalTime() const {
        if (timings_.empty()) return 0.0;
        return std::accumulate(timings_.begin(), timings_.end(), 0.0);
    }
    
    size_t getTaskCount() const {
        return timings_.size();
    }
    
    std::vector<double> getAllTimings() const {
        return timings_;
    }
};

/**
 * Results export utilities
 */
class ResultsExporter {
public:
    /**
     * Export results to JSON format
     */
    static std::string exportToJSON(const std::string& data, const std::string& filename = "") {
        // This is a simplified JSON export - in practice, you'd use a proper JSON library
        if (!filename.empty()) {
            std::ofstream file(filename);
            file << data;
            file.close();
        }
        return data;
    }
    
    /**
     * Export results to CSV format
     */
    static std::string exportToCSV(const std::string& data, const std::string& filename = "") {
        // This is a simplified CSV export - in practice, you'd use a proper CSV library
        if (!filename.empty()) {
            std::ofstream file(filename);
            file << data;
            file.close();
        }
        return data;
    }
    
    /**
     * Export results to YAML format
     */
    static std::string exportToYAML(const std::string& data, const std::string& filename = "") {
        // This is a simplified YAML export - in practice, you'd use a proper YAML library
        if (!filename.empty()) {
            std::ofstream file(filename);
            file << data;
            file.close();
        }
        return data;
    }
};

} // namespace nlm