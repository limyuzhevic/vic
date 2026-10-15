// Advanced user interface utilities for NLM
// Provides enhanced command-line handling, help systems, and output formatting

#pragma once

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>
#include <chrono>

namespace nlm {

// Advanced argument parser with comprehensive help system
class AdvancedCLIParser {
public:
    struct CLIOptions {
        std::string programName;
        std::string version;
        std::string description;
        std::string usage;
        std::string helpText;
        bool showAdvancedOptions;
        bool showExamples;
        bool showDefaults;
        std::map<std::string, std::string> optionDescriptions;
        std::vector<std::string> exampleCommands;
        std::map<std::string, std::string> advancedOptions;
    };
    
    struct ParsedArguments {
        std::map<std::string, std::string> options;
        std::vector<std::string> positionalArgs;
        std::string configFile;
        std::string outputFile;
        bool helpRequested;
        bool versionRequested;
        bool validateRequested;
        bool verboseMode;
        bool quietMode;
        std::chrono::system_clock::time_point startTime;
    };
    
    AdvancedCLIParser() {}
    
    // Parse command line arguments with comprehensive help system
    ParsedArguments parseArgs(int argc, char* argv[], const CLIOptions& options) {
        ParsedArguments result;
        result.startTime = std::chrono::system_clock::now();
        
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--help" || arg == "-h") {
                result.helpRequested = true;
                displayHelp(options);
            } else if (arg == "--version" || arg == "-v") {
                result.versionRequested = true;
                displayVersion(options);
            } else if (arg == "--validate" || arg == "--validate-config") {
                result.validateRequested = true;
                // Would trigger configuration validation
            } else if (arg == "--verbose" || arg == "-V") {
                result.verboseMode = true;
            } else if (arg == "--quiet" || arg == "-q") {
                result.quietMode = true;
            } else if (arg == "--examples" || arg == "--help-examples") {
                displayExamples(options);
            } else if (arg == "--advanced" || arg == "--advanced-options") {
                displayAdvancedOptions(options);
            } else if (arg.find("--config-file=") == 0) {
                result.configFile = arg.substr(13);
            } else if (arg.find("--output-file=") == 0) {
                result.outputFile = arg.substr(13);
            } else if (arg.find("--") == 0 && arg.find("=") != std::string::npos) {
                size_t equalsPos = arg.find("=");
                std::string key = arg.substr(2, equalsPos - 3);
                std::string value = arg.substr(equalsPos + 1);
                result.options[key] = value;
            } else if (arg.find("--") == 0) {
                std::string key = arg.substr(2);
                result.options[key] = "true";
            } else {
                // Positional argument
                result.positionalArgs.push_back(arg);
            }
        }
        
        return result;
    }
    
    // Display comprehensive help information
    void displayHelp(const CLIOptions& options) {
        std::cout << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "NLM - Neural Learning Machine" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << std::endl;
        std::cout << "Version: " << options.version << std::endl;
        std::cout << std::endl;
        std::cout << "Description: " << options.description << std::endl;
        std::cout << std::endl;
        std::cout << "Usage: " << options.usage << std::endl;
        std::cout << std::endl;
        
        if (options.showDefaults) {
            std::cout << "Default Configuration:" << std::endl;
            for (const auto& pair : options.optionDescriptions) {
                std::cout << "  " << pair.first << " (default: " << getDefaultDescription(pair.first) << ")" << std::endl;
            }
            std::cout << std::endl;
        }
        
        std::cout << "Options:" << std::endl;
        for (const auto& pair : options.optionDescriptions) {
            std::cout << "  --" << pair.first << std::endl;
            std::cout << "      " << pair.second << std::endl;
            std::cout << std::endl;
        }
        
        if (options.showExamples) {
            std::cout << "Examples:" << std::endl;
            for (const auto& example : options.exampleCommands) {
                std::cout << "  " << example << std::endl;
            }
            std::cout << std::endl;
        }
        
        if (options.showAdvancedOptions) {
            std::cout << "Advanced Options:" << std::endl;
            for (const auto& pair : options.advancedOptions) {
                std::cout << "  --" << pair.first << std::endl;
                std::cout << "      " << pair.second << std::endl;
                std::cout << std::endl;
            }
        }
        
        std::cout << "Additional Help:" << std::endl;
        std::cout << "  --help, -h: Display this help message" << std::endl;
        std::cout << "  --version, -v: Show version information" << std::endl;
        std::cout << "  --validate: Validate configuration" << std::endl;
        std::cout << "  --verbose, -V: Enable verbose output" << std::endl;
        std::cout << "  --quiet, -q: Quiet mode (minimal output)" << std::endl;
        std::cout << "  --examples: Show usage examples" << std::endl;
        std::cout << "  --advanced: Show advanced options" << std::endl;
        std::cout << std::endl;
        std::cout << std::string(80, '=') << std::endl;
    }
    
    // Display version information
    void displayVersion(const CLIOptions& options) {
        std::cout << "NLM " << options.version << std::endl;
        std::cout << "Built: " << __DATE__ << " " << __TIME__ << std::endl;
        std::cout << "Description: " << options.description << std::endl;
    }
    
    // Display examples
    void displayExamples(const CLIOptions& options) {
        std::cout << "NLM Usage Examples:" << std::endl;
        std::cout << std::endl;
        for (const auto& example : options.exampleCommands) {
            std::cout << "  " << example << std::endl;
        }
        std::cout << std::endl;
        std::cout << "For more detailed help, use:" << std::endl;
        std::cout << "  " << options.programName << " --help" << std::endl;
    }
    
    // Display advanced options
    void displayAdvancedOptions(const CLIOptions& options) {
        std::cout << "Advanced NLM Options:" << std::endl;
        std::cout << std::endl;
        for (const auto& pair : options.advancedOptions) {
            std::cout << "  --" << pair.first << std::endl;
            std::cout << "      " << pair.second << std::endl;
            std::cout << std::endl;
        }
    }
    
    // Generate help options from configuration
    static CLIOptions generateHelpOptions(const std::string& programName, const std::string& version, 
                                         const std::string& description, const std::string& usage) {
        CLIOptions options;
        options.programName = programName;
        options.version = version;
        options.description = description;
        options.usage = usage;
        options.showAdvancedOptions = true;
        options.showExamples = true;
        options.showDefaults = true;
        
        // Basic options
        options.optionDescriptions = {
            {"help", "Display help information and exit"},
            {"version", "Show version information and exit"},
            {"validate", "Validate configuration file and exit"},
            {"verbose", "Enable verbose output mode"},
            {"quiet", "Quiet mode (minimal output)"},
            {"examples", "Show usage examples and exit"},
            {"advanced", "Show advanced options and exit"}
        };
        
        // Example commands
        options.exampleCommands = {
            "nlm --help, nlm -h: Display help information",
            "nlm --version, nlm -v: Show version information",
            "nlm --validate: Validate configuration",
            "nlm --neuron_count=1000 --region_count=1: Basic configuration",
            "nlm --enable_development --enable_curiosity --enable_dopamine: Enable learning systems",
            "nlm --checkpoint_dir=./checkpoints --checkpoint_interval=500: Configure checkpointing"
        };
        
        // Advanced options
        options.advancedOptions = {
            {"config_file", "Load configuration from file"},
            {"output_file", "Save results to file"},
            {"profiling", "Enable performance profiling"},
            {"profiler_output", "Set profiler output file"},
            {"memory_pool_size", "Set memory pool size"},
            {"event_queue_size", "Set event queue size"},
            {"enable_vectorization", "Enable SIMD vectorization"},
            {"enable_parallel_processing", "Enable parallel processing"},
            {"validation_rules", "Load validation rules from file"},
            {"environment_prefix", "Environment variable prefix for configuration"}
        };
        
        return options;
    }
    
private:
    std::string getDefaultDescription(const std::string& option) const {
        // Map option names to default descriptions
        static const std::map<std::string, std::string> defaults = {
            {"neuron_count", "1000"},
            {"region_count", "1"},
            {"connection_probability", "0.1"},
            {"random_seed", "42"},
            {"simulation_timestep", "0.001"},
            {"stdp_ltp_weight", "0.01"},
            {"stdp_ltd_weight", "0.012"},
            {"stdp_tau", "20.0"},
            {"synaptogenesis_rate", "0.0001"},
            {"pruning_rate", "0.00001"},
            {"enable_checkpointing", "false"},
            {"enable_development", "false"},
            {"enable_curiosity", "false"},
            {"enable_dopamine", "false"},
            {"enable_novelty", "false"},
            {"enable_vectorization", "true"},
            {"enable_parallel_processing", "true"}
        };
        
        auto it = defaults.find(option);
        if (it != defaults.end()) {
            return it->second;
        }
        return "N/A";
    }
};

// Enhanced output formatter for NLM
class OutputFormatter {
public:
    struct FormatOptions {
        bool jsonOutput;
        bool verboseOutput;
        bool includeTimestamps;
        bool includeMemoryUsage;
        bool includePerformanceStats;
        std::string outputEncoding;
        bool colorOutput;
        bool compactOutput;
    };
    
    OutputFormatter() {}
    
    // Format configuration output
    std::string formatConfiguration(const std::map<std::string, std::string>& config, const FormatOptions& options) {
        if (options.jsonOutput) {
            return formatAsJSON(config, options);
        } else {
            return formatAsText(config, options);
        }
    }
    
    // Format experiment results
    std::string formatResults(const std::map<std::string, double>& results, const FormatOptions& options) {
        if (options.jsonOutput) {
            return formatResultsAsJSON(results, options);
        } else {
            return formatResultsAsText(results, options);
        }
    }
    
    // Format performance metrics
    std::string formatPerformance(const std::map<std::string, double>& metrics, const FormatOptions& options) {
        std::ostringstream oss;
        
        if (options.jsonOutput) {
            oss << "{\n";
            bool first = true;
            for (const auto& pair : metrics) {
                if (!first) oss << ",\n";
                oss << "  \"" << pair.first << "\": " << pair.second;
                first = false;
            }
            oss << "\n}\n";
        } else {
            oss << "Performance Metrics:" << std::endl;
            oss << std::string(50, '=') << std::endl;
            for (const auto& pair : metrics) {
                oss << std::left << std::setw(30) << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        return oss.str();
    }
    
    // Format timing information
    std::string formatTiming(const std::chrono::system_clock::time_point& startTime, const std::chrono::system_clock::time_point& endTime, const FormatOptions& options) {
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::ostringstream oss;
        
        if (options.jsonOutput) {
            oss << "{\n";
            oss << "  \"total_duration_ms\": " << duration.count() << ",\n";
            oss << "  \"format\": \"chrono\"\n";
            oss << "}\n";
        } else {
            oss << "Timing Information:" << std::endl;
            oss << std::string(30, '-') << std::endl;
            oss << "Total Duration: " << duration.count() << " ms" << std::endl;
            oss << "Start Time: " << std::chrono::system_clock::to_time_t(startTime) << std::endl;
            oss << "End Time: " << std::chrono::system_clock::to_time_t(endTime) << std::endl;
        }
        
        return oss.str();
    }
    
    // Format memory information
    std::string formatMemory(const std::map<std::string, size_t>& memoryInfo, const FormatOptions& options) {
        std::ostringstream oss;
        
        if (options.jsonOutput) {
            oss << "{\n";
            bool first = true;
            for (const auto& pair : memoryInfo) {
                if (!first) oss << ",\n";
                oss << "  \"" << pair.first << "\": " << pair.second;
                first = false;
            }
            oss << "\n}\n";
        } else {
            oss << "Memory Information:" << std::endl;
            oss << std::string(30, '-') << std::endl;
            for (const auto& pair : memoryInfo) {
                oss << std::left << std::setw(25) << pair.first << ": " << pair.second << " bytes" << std::endl;
            }
        }
        
        return oss.str();
    }
    
    // Format status information
    std::string formatStatus(const std::map<std::string, std::string>& status, const FormatOptions& options) {
        std::ostringstream oss;
        
        if (options.jsonOutput) {
            oss << "{\n";
            bool first = true;
            for (const auto& pair : status) {
                if (!first) oss << ",\n";
                oss << "  \"" << pair.first << "\": \"" << pair.second << "\"";
                first = false;
            }
            oss << "\n}\n";
        } else {
            oss << "Status:" << std::endl;
            oss << std::string(30, '-') << std::endl;
            for (const auto& pair : status) {
                oss << std::left << std::setw(20) << pair.first << ": " << pair.second << std::endl;
            }
        }
        
        return oss.str();
    }
    
private:
    std::string formatAsJSON(const std::map<std::string, std::string>& config, const FormatOptions& options) {
        std::ostringstream oss;
        oss << "{\n";
        
        bool first = true;
        for (const auto& pair : config) {
            if (!first) oss << ",\n";
            oss << "  \"" << pair.first << "\": \"" << escapeJSON(pair.second) << "\"";
            first = false;
        }
        
        oss << "\n}\n";
        return oss.str();
    }
    
    std::string formatAsText(const std::map<std::string, std::string>& config, const FormatOptions& options) {
        std::ostringstream oss;
        oss << "Configuration:" << std::endl;
        oss << std::string(20, '-') << std::endl;
        
        for (const auto& pair : config) {
            oss << std::left << std::setw(20) << pair.first << ": " << pair.second << std::endl;
        }
        
        return oss.str();
    }
    
    std::string formatResultsAsJSON(const std::map<std::string, double>& results, const FormatOptions& options) {
        std::ostringstream oss;
        oss << "{\n";
        
        bool first = true;
        for (const auto& pair : results) {
            if (!first) oss << ",\n";
            oss << "  \"" << pair.first << "\": " << pair.second;
            first = false;
        }
        
        oss << "\n}\n";
        return oss.str();
    }
    
    std::string formatResultsAsText(const std::map<std::string, double>& results, const FormatOptions& options) {
        std::ostringstream oss;
        oss << "Results:" << std::endl;
        oss << std::string(20, '-') << std::endl;
        
        for (const auto& pair : results) {
            oss << std::left << std::setw(20) << pair.first << ": " << pair.second << std::endl;
        }
        
        return oss.str();
    }
    
    std::string escapeJSON(const std::string& input) {
        std::ostringstream oss;
        for (char c : input) {
            switch (c) {
                case '"': oss << "\\\""; break;
                case '\\': oss << "\\\\"; break;
                case '/': oss << "\\/"; break;
                case '\b': oss << "\\b"; break;
                case '\f': oss << "\\f"; break;
                case '\n': oss << "\\n"; break;
                case '\r': oss << "\\r"; break;
                case '\t': oss << "\\t"; break;
                default:
                    if (c < 32 || c > 126) {
                        oss << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                    } else {
                        oss << c;
                    }
                    break;
            }
        }
        return oss.str();
    }
};

// Progress reporter for long-running operations
class ProgressReporter {
public:
    struct ProgressOptions {
        bool showProgress;
        bool showEstimatedTime;
        bool showCompletionPercentage;
        std::string progressFormat;
        std::string completionMessage;
    };
    
    ProgressReporter() : lastReportTime_(0), totalSteps_(0) {}
    
    // Report progress for current operation
    void reportProgress(int currentStep, int totalSteps, const std::string& message = "", const ProgressOptions& options = ProgressOptions()) {
        if (!options.showProgress) return;
        
        auto currentTime = std::chrono::system_clock::now();
        auto timeSinceLastReport = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastReportTime_).count();
        
        // Update every second or every 10% progress
        if (timeSinceLastReport >= 1 || (totalSteps_ > 0 && currentStep % std::max(1, totalSteps_ / 10) == 0)) {
            displayProgress(currentStep, totalSteps, message, options);
            lastReportTime_ = currentTime;
        }
    }
    
    // Report completion
    void reportCompletion(const std::string& message = "", const ProgressOptions& options = ProgressOptions()) {
        if (options.completionMessage.empty()) {
            std::cout << "Complete!" << std::endl;
        } else {
            std::cout << options.completionMessage << std::endl;
        }
        
        if (options.showCompletionPercentage) {
            std::cout << "100% complete" << std::endl;
        }
    }
    
    // Set total steps for progress reporting
    void setTotalSteps(int totalSteps) {
        totalSteps_ = totalSteps;
    }
    
private:
    void displayProgress(int currentStep, int totalSteps, const std::string& message, const ProgressOptions& options) {
        std::cout << "\r";
        
        if (options.showCompletionPercentage) {
            float percentage = (totalSteps > 0) ? (static_cast<float>(currentStep) / totalSteps) * 100.0f : 0.0f;
            std::cout << std::fixed << std::setprecision(1) << percentage << "%";
        }
        
        std::cout << " [";
        int barWidth = 30;
        int progressWidth = (totalSteps > 0) ? (currentStep * barWidth) / totalSteps : 0;
        
        for (int i = 0; i < barWidth; ++i) {
            if (i < progressWidth) {
                std::cout << "=";
            } else {
                std::cout << " ";
            }
        }
        
        std::cout << "]";
        
        if (currentStep > 0 && totalSteps > 0) {
            std::cout << " " << currentStep << "/" << totalSteps;
            
            if (options.showEstimatedTime && totalSteps_ > 0) {
                double estimatedTotalSeconds = (durationEstimate_.count() > 0) ? 
                    (durationEstimate_.count() * static_cast<double>(totalSteps)) / currentStep : 0.0;
                double remainingSeconds = estimatedTotalSeconds - durationEstimate_.count();
                
                std::chrono::seconds remaining(remainingSeconds);
                if (remaining.count() > 60) {
                    int minutes = remaining.count() / 60;
                    int seconds = remaining.count() % 60;
                    std::cout << " (~" << minutes << "m " << seconds << "s remaining)";
                } else {
                    std::cout << " (~" << remaining.count() << "s remaining)";
                }
            }
        }
        
        if (!message.empty()) {
            std::cout << " - " << message;
        }
        
        std::cout << std::flush;
        
        // Start timing if this is the first report
        if (durationEstimate_.count() == 0 && currentStep > 0) {
            durationEstimate_ = std::chrono::steady_clock::now() - startTime_;
        }
        
        startTime_ = std::chrono::steady_clock::now();
    }
    
    std::chrono::steady_clock::time_point startTime_;
    std::chrono::steady_clock::duration durationEstimate_;
    std::chrono::system_clock::time_point lastReportTime_;
    int totalSteps_;
};

} // namespace nlm