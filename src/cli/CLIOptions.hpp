// NLM (熙然) - Neural Learning Machine
// CLI Framework
// Modern C++ command-line interface with argument parsing, help system, and execution modes

#ifndef NLM_CLI_FRAMEWORK_HPP
#define NLM_CLI_FRAMEWORK_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <memory>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <cctype>

namespace nlm {

// Logging levels for CLI output
enum class LogLevel {
    Quiet = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4
};

// Output formats
enum class OutputFormat {
    Text,
    JSON,
    CSV
};

// Execution modes
enum class ExecutionMode {
    BrainOnly,
    AgentWorldSimulation,
    DevelopmentLearning,
    AnalysisDebugging,
    BatchProcessing
};

// CLI Option definition
struct CLIOption {
    std::string name;
    char shortOpt;
    std::string description;
    std::string defaultValue;
    std::function<void(const std::string&)> callback;
    bool required = false;
    bool hasValue = true;
};

// CLI Framework class
class CLI {
public:
    CLI(const std::string& name, const std::string& version)
        : name_(name), version_(version) {
    }

    // Add option to CLI
    void addOption(const std::string& name, char shortOpt, const std::string& description,
                   const std::string& defaultValue = "",
                   std::function<void(const std::string&)> callback = nullptr,
                   bool required = false) {
        options_[name] = CLIOption{
            name, shortOpt, description, defaultValue, callback, required, true
        };
        shortOptions_[shortOpt] = name;
    }

    // Add flag option
    void addFlag(const std::string& name, char shortOpt, const std::string& description,
                 std::function<void()> callback = nullptr) {
        options_[name] = CLIOption{
            name, shortOpt, description, "false", [callback]() { if (callback) callback(); }, false
        };
        shortOptions_[shortOpt] = name;
    }

    // Parse command line arguments
    bool parse(int argc, char** argv) {
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            // Handle --help
            if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
            
            // Handle --version
            if (arg == "--version") {
                printVersion();
                return false;
            }
            
            // Handle --verbose
            if (arg == "--verbose") {
                logLevel_ = LogLevel::Debug;
                continue;
            }
            
            // Handle --quiet
            if (arg == "--quiet") {
                logLevel_ = LogLevel::Quiet;
                continue;
            }
            
            // Handle --output
            if (arg.find("--output=") == 0) {
                outputFile_ = arg.substr(9);
                outputFormat_ = parseOutputFormat(outputFile_);
                continue;
            }
            
            // Handle --config
            if (arg.find("--config=") == 0) {
                configFile_ = arg.substr(8);
                continue;
            }
            
            // Handle execution mode
            if (arg.find("--mode=") == 0) {
                executionMode_ = parseExecutionMode(arg.substr(7));
                continue;
            }
            
            // Handle short options
            if (arg.length() == 2 && arg[0] == '-' && std::isalpha(arg[1])) {
                char opt = arg[1];
                if (shortOptions_.find(opt) != shortOptions_.end()) {
                    std::string optName = shortOptions_[opt];
                    if (options_[optName].hasValue) {
                        if (i + 1 < argc && argv[i + 1][0] != '-') {
                            std::string value = argv[++i];
                            processOption(optName, value);
                        } else {
                            std::cerr << "Error: Option " << arg << " requires a value\n";
                            return false;
                        }
                    } else {
                        processOption(optName);
                    }
                } else {
                    std::cerr << "Error: Unknown option " << arg << "\n";
                    return false;
                }
                continue;
            }
            
            // Handle long options
            if (arg.find("--") == 0) {
                size_t eqPos = arg.find('=');
                std::string optName;
                if (eqPos != std::string::npos) {
                    optName = arg.substr(2, eqPos - 2);
                    std::string value = arg.substr(eqPos + 1);
                    processOption(optName, value);
                } else {
                    optName = arg.substr(2);
                    // Check if next argument is a value
                    if (i + 1 < argc && argv[i + 1][0] != '-') {
                        std::string value = argv[++i];
                        processOption(optName, value);
                    } else {
                        processOption(optName);
                    }
                }
                continue;
            }
            
            // Handle file arguments
            if (arg[0] != '-') {
                positionalArgs_.push_back(arg);
            }
        }
        
        // Check required options
        for (const auto& pair : options_) {
            if (pair.second.required && !isOptionSet(pair.first)) {
                std::cerr << "Error: Required option --" << pair.first << " is missing\n";
                return false;
            }
        }
        
        return true;
    }

    // Print help information
    void printHelp() {
        std::cout << std::string(60, '=') << std::endl;
        std::cout << "  " << name_ << " - " << version_ << std::endl;
        std::cout << std::string(60, '=') << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << name_ << " [OPTIONS] [ARGS...]" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Execution Modes:" << std::endl;
        std::cout << "  --mode=brain-only          Simple neural computation" << std::endl;
        std::cout << "  --mode=agent-world         Full agent-world simulation" << std::endl;
        std::cout << "  --mode=development         Development and learning mode" << std::endl;
        std::cout << "  --mode=analysis            Analysis and debugging mode" << std::endl;
        std::cout << "  --mode=batch               Batch processing mode" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Options:" << std::endl;
        for (const auto& pair : options_) {
            const auto& opt = pair.second;
            if (opt.hasValue) {
                std::cout << "  -" << opt.shortOpt << ", --" << opt.name;
                if (!opt.defaultValue.empty()) {
                    std::cout << "=" << opt.defaultValue;
                }
                std::cout << "\n      " << opt.description << std::endl;
            } else {
                std::cout << "  -" << opt.shortOpt << ", --" << opt.name << "\n";
                std::cout << "      " << opt.description << std::endl;
            }
        }
        
        std::cout << std::endl;
        std::cout << "General Options:" << std::endl;
        std::cout << "  -h, --help                 Show this help message" << std::endl;
        std::cout << "  --version                  Show version information" << std::endl;
        std::cout << "  --verbose                  Increase output verbosity" << std::endl;
        std::cout << "  --quiet                    Decrease output verbosity" << std::endl;
        std::cout << "  --output=<file>            Output to file (text, json, csv)" << std::endl;
        std::cout << "  --config=<file>            Load configuration from file" << std::endl;
        std::cout << std::endl;
        std::cout << "Examples:" << std::endl;
        std::cout << "  " << name_ << " --mode=agent-world --config=simulation.cfg" << std::endl;
        std::cout << "  " << name_ << " --mode=brain-only --neuron-count=1000 --output=results.json" << std::endl;
        std::cout << "  " << name_ << " --mode=batch --input-dir=configs --output-dir=results" << std::endl;
        std::cout << std::endl;
    }

    // Print version information
    void printVersion() {
        std::cout << name_ << " " << version_ << std::endl;
        std::cout << "Built with C++20" << std::endl;
        std::cout << "Copyright (c) 2026 NLM Project" << std::endl;
    }

    // Get option value
    template<typename T>
    T getOption(const std::string& name, T defaultValue) {
        if (optionValues_.find(name) != optionValues_.find(name)) {
            // In a real implementation, this would convert to type T
            return defaultValue;
        }
        return defaultValue;
    }

    // Check if option is set
    bool isOptionSet(const std::string& name) const {
        return optionValues_.find(name) != optionValues_.end();
    }

    // Get output format
    OutputFormat getOutputFormat() const { return outputFormat_; }
    
    // Get log level
    LogLevel getLogLevel() const { return logLevel_; }
    
    // Get execution mode
    ExecutionMode getExecutionMode() const { return executionMode_; }
    
    // Get config file
    const std::string& getConfigFile() const { return configFile_; }
    
    // Get output file
    const std::string& getOutputFile() const { return outputFile_; }
    
    // Get positional arguments
    const std::vector<std::string>& getPositionalArgs() const { return positionalArgs_; }
    
    // Get current timestamp for logging
    std::string getTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        ss << "." << std::setfill('0') << std::setw(3) << ms.count();
        return ss.str();
    }

private:
    std::string name_;
    std::string version_;
    std::map<std::string, CLIOption> options_;
    std::map<char, std::string> shortOptions_;
    std::map<std::string, std::string> optionValues_;
    std::vector<std::string> positionalArgs_;
    std::string configFile_;
    std::string outputFile_;
    OutputFormat outputFormat_ = OutputFormat::Text;
    LogLevel logLevel_ = LogLevel::Info;
    ExecutionMode executionMode_ = ExecutionMode::AgentWorldSimulation;

    // Parse output format from file extension
    OutputFormat parseOutputFormat(const std::string& filename) {
        if (filename.length() >= 4) {
            std::string ext = filename.substr(filename.length() - 4);
            if (ext == ".json") return OutputFormat::JSON;
            if (ext == ".csv") return OutputFormat::CSV;
        }
        return OutputFormat::Text;
    }

    // Parse execution mode
    ExecutionMode parseExecutionMode(const std::string& modeStr) {
        std::string lower = modeStr;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
        
        if (lower == "brain-only" || lower == "brain_only") return ExecutionMode::BrainOnly;
        if (lower == "agent-world" || lower == "agent_world") return ExecutionMode::AgentWorldSimulation;
        if (lower == "development" || lower == "development-learning") return ExecutionMode::DevelopmentLearning;
        if (lower == "analysis" || lower == "debugging") return ExecutionMode::AnalysisDebugging;
        if (lower == "batch") return ExecutionMode::BatchProcessing;
        
        return ExecutionMode::AgentWorldSimulation;
    }

    // Process option with value
    void processOption(const std::string& name, const std::string& value) {
        if (options_.find(name) != options_.end()) {
            if (options_[name].callback) {
                if (options_[name].hasValue) {
                    options_[name].callback(value);
                }
            }
            optionValues_[name] = value;
        }
    }

    // Process option without value (flag)
    void processOption(const std::string& name) {
        if (options_.find(name) != options_.end() && !options_[name].hasValue) {
            if (options_[name].callback) {
                options_[name].callback("");
            }
            optionValues_[name] = "true";
        }
    }
};

} // namespace nlm

#endif // NLM_CLI_FRAMEWORK_HPP
