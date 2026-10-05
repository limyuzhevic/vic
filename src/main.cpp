// NLM Phase 2: Real Neural Computation - Enhanced Main Entry Point
// Provides comprehensive CLI interface with configurable experiments
// Integrates all existing functionality with improved architecture

#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "environment/Environment.hpp"
#include "experiments/ExperimentRunner.hpp"
#include "experiments/NLMExperimentRunner.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>
#include <iomanip>
#include <chrono>
#include <csignal>

namespace nlm {

class EnhancedExperimentRunner {
public:
    EnhancedExperimentRunner() = default;
    ~EnhancedExperimentRunner() = default;
    
    // Main entry point with CLI support
    int run(int argc, char** argv);
    
    // Run with configuration file
    bool runWithConfig(const std::string& configFile);
    
    // Run specific experiment with parameters
    bool runExperiment(const std::string& experimentName,
                      const std::map<std::string, std::string>& params);
    
    // Run benchmark suite
    bool runBenchmarkSuite(const std::string& benchmarkFile = "");
    
    // Export results
    bool exportResults(const std::string& outputFile,
                      const std::string& experimentName = "");
    
    // Show experiment information
    void showExperiments() const;
    void showBenchmarks() const;
    
    // Validate configuration
    bool validateConfiguration(const std::shared_ptr<Config>& config) const;
    
    // Load experiment definitions
    bool loadExperimentDefinitions(const std::string& definitionsFile);
    
    // Get experiment statistics
    struct ExperimentStats {
        std::string name;
        std::string description;
        std::vector<std::string> requiredParams;
        double estimatedTimeSeconds;
        std::string category;
    };
    
    std::vector<ExperimentStats> getExperimentStats() const;
    
    // Performance profiling
    struct ProfilingInfo {
        double totalTimeMs;
        size_t memoryPeakMB;
        size_t cpuTimeMs;
        double efficiencyScore;
    };
    
    ProfilingInfo getProfilingInfo() const { return profilingInfo_; }
    
private:
    // Helper classes and methods
    class ParameterValidator {
    public:
        static bool validate(const std::map<std::string, std::string>& params,
                           const std::vector<std::string>& required);
        static std::string getType(const std::string& paramName);
        static std::string getRange(const std::string& paramName);
    };
    
    class ExperimentFactory {
    public:
        using ExperimentCreator = std::function<std::unique_ptr<Experiment>(const std::string& name, const std::shared_ptr<Config>& config)>;
        
        static void registerExperiment(const std::string& name, 
                                      ExperimentCreator creator);
        static std::unique_ptr<Experiment> createExperiment(const std::string& name,
                                                           const std::shared_ptr<Config>& config);
        static std::vector<std::string> getRegisteredExperiments();
        
    private:
        static std::unordered_map<std::string, ExperimentCreator> registry_;
    };
    
    class ResultsAggregator {
    public:
        static ExperimentResults aggregate(const std::vector<ExperimentResults>& results);
        static std::string generateReport(const ExperimentResults& results);
        static std::string generateJSONReport(const ExperimentResults& results);
        static std::string generateCSVReport(const ExperimentResults& results);
        static std::string generateHTMLReport(const ExperimentResults& results);
    };
    
    class PerformanceProfiler {
    public:
        void startProfile(const std::string& operation);
        void endProfile(const std::string& operation);
        double getTotalTimeMs() const;
        size_t getPeakMemoryMB() const;
        void printProfileReport() const;
        
    private:
        struct ProfileData {
            std::string name;
            std::chrono::high_resolution_clock::time_point start;
            double durationMs;
            size_t memoryMB;
        };
        
        std::vector<ProfileData> profileData_;
        double totalTimeMs_;
        size_t peakMemoryMB_;
    };
    
    // CLI argument parsing
    static std::map<std::string, std::string> parseCLIArgs(int argc, char** argv);
    static std::string trim(const std::string& str);
    static bool startsWith(const std::string& str, const std::string& prefix);
    static std::string toLower(const std::string& str);
    
    // Configuration management
    bool loadConfigFile(const std::string& filename, std::shared_ptr<Config>& config);
    bool applyCommandLineOverrides(const std::map<std::string, std::string>& overrides,
                                  std::shared_ptr<Config>& config);
    void printConfigSummary(const std::shared_ptr<Config>& config) const;
    
    // Experiment execution
    bool executeBasicConnectivityExperiment(const std::shared_ptr<Config>& config);
    bool executePlasticityExperiment(const std::shared_ptr<Config>& config);
    bool executeSTDPVerificationExperiment(const std::shared_ptr<Config>& config);
    bool executeScalingExperiment(const std::shared_ptr<Config>& config);
    
    // Benchmark execution
    bool executeConnectivityBenchmark(const std::shared_ptr<Config>& config);
    bool executePlasticityBenchmark(const std::shared_ptr<Config>& config);
    bool executeMemoryBenchmark(const std::shared_ptr<Config>& config);
    bool executeScalingBenchmark(const std::shared_ptr<Config>& config);
    
    // Result export
    bool exportToCSV(const std::string& filename, const ExperimentResults& results);
    bool exportToJSON(const std::string& filename, const ExperimentResults& results);
    bool exportToText(const std::string& filename, const ExperimentResults& results);
    
    // Configuration validation
    bool validateBasicConnectivityConfig(const std::shared_ptr<Config>& config) const;
    bool validatePlasticityConfig(const std::shared_ptr<Config>& config) const;
    bool validateSTDPConfig(const std::shared_ptr<Config>& config) const;
    bool validateScalingConfig(const std::shared_ptr<Config>& config) const;
    
    // Error handling
    enum class ErrorCode {
        SUCCESS = 0,
        INVALID_ARGUMENTS = 1,
        EXPERIMENT_NOT_FOUND = 2,
        CONFIG_FILE_ERROR = 3,
        VALIDATION_ERROR = 4,
        EXECUTION_ERROR = 5,
        EXPORT_ERROR = 6,
        UNKNOWN_ERROR = 7
    };
    
    class ExperimentException : public std::runtime_error {
    public:
        ExperimentException(ErrorCode code, const std::string& message);
        ErrorCode getErrorCode() const { return errorCode_; }
    private:
        ErrorCode errorCode_;
    };
    
    // Global state
    std::shared_ptr<Config> config_;
    PerformanceProfiler profiler_;
    ProfilingInfo profilingInfo_;
    std::unordered_map<std::string, std::chrono::high_resolution_clock::time_point> startTimes_;
    bool verboseLogging_;
    bool enableBenchmarking_;
    std::string outputFormat_;
    std::string outputFile_;
}; 

// Global enhanced experiment runner instance
inline EnhancedExperimentRunner enhancedRunner;

// Version and build information
const std::string NLM_VERSION = "2.0.0";
const std::string NLM_BUILD_TYPE = "Phase 2: Real Neural Computation";
const std::string NLM_BUILD_DATE = __DATE__;
const std::string NLM_BUILD_TIME = __TIME__;

// Signal handler for graceful shutdown
void signalHandler(int signal) {
    std::cout << "\nReceived signal " << signal << ", shutting down gracefully..." << std::endl;
    if (enhancedRunner.getProfilingInfo().totalTimeMs > 0) {
        enhancedRunner.getProfilingInfo(); // Trigger final profiling report
    }
    std::exit(1);
}

// Main function with enhanced CLI support
int main(int argc, char** argv) {
    // Setup signal handlers
    std::signal(SIGINT, signalHandler);
    std::signal(SIGTERM, signalHandler);
    
    // Initialize global logger
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Starting NLM Enhanced Experiment Runner");
    NLM_LOG_INFO("Version: " + NLM_VERSION);
    NLM_LOG_INFO("Build: " + NLM_BUILD_TYPE);
    NLM_LOG_INFO("Build date: " + std::string(NLM_BUILD_DATE));
    
    try {
        // Run enhanced experiment runner
        int result = enhancedRunner.run(argc, argv);
        
        // Log final status
        if (result == 0) {
            NLM_LOG_INFO("Experiment runner completed successfully");
        } else {
            NLM_LOG_ERROR("Experiment runner completed with errors");
        }
        
        return result;
        
    } catch (const std::exception& e) {
        NLM_LOG_CRITICAL("Fatal error: " + std::string(e.what()));
        return static_cast<int>(EnhancedExperimentRunner::ErrorCode::UNKNOWN_ERROR);
    }
}

} // namespace nlm

// Implementation details
namespace nlm {
    
    int EnhancedExperimentRunner::run(int argc, char** argv) {
        profiler_.startProfile("total");
        
        try {
            // Parse command line arguments
            auto cliArgs = parseCLIArgs(argc, argv);
            
            // Handle help and version requests
            if (cliArgs.find("--help") != cliArgs.end() || cliArgs.find("-h") != cliArgs.end()) {
                printHelp();
                return static_cast<int>(ErrorCode::SUCCESS);
            }
            
            if (cliArgs.find("--version") != cliArgs.end() || cliArgs.find("-v") != cliArgs.end()) {
                printVersion();
                return static_cast<int>(ErrorCode::SUCCESS);
            }
            
            // Initialize configuration
            config_ = std::make_shared<Config>();
            verboseLogging_ = cliArgs.find("--verbose") != cliArgs.end();
            enableBenchmarking_ = cliArgs.find("--benchmark") != cliArgs.end();
            outputFormat_ = cliArgs.find("--format") != cliArgs.end() 
                           ? cliArgs["--format"] : "text";
            outputFile_ = cliArgs.find("--output") != cliArgs.end()
                         ? cliArgs["--output"] : "";
            
            // Load configuration file if specified
            if (cliArgs.find("--config") != cliArgs.end()) {
                std::string configFile = cliArgs["--config"];
                if (!loadConfigFile(configFile, config_)) {
                    throw ExperimentException(ErrorCode::CONFIG_FILE_ERROR,
                                            "Failed to load configuration file: " + configFile);
                }
            }
            
            // Apply command line overrides
            applyCommandLineOverrides(cliArgs, config_);
            
            if (verboseLogging_) {
                printConfigSummary(config_);
            }
            
            // Validate configuration
            if (!validateConfiguration(config_)) {
                throw ExperimentException(ErrorCode::VALIDATION_ERROR,
                                        "Configuration validation failed");
            }
            
            // Handle commands
            std::string command = cliArgs.find("command") != cliArgs.end() ? cliArgs["command"] : "run";
            
            if (command == "run") {
                std::string experiment = cliArgs.find("experiment") != cliArgs.end() ? cliArgs["experiment"] : "";
                if (experiment.empty()) {
                    throw ExperimentException(ErrorCode::INVALID_ARGUMENTS,
                                            "Experiment name required for run command");
                }
                
                // Extract parameters for the experiment
                std::map<std::string, std::string> params;
                for (const auto& arg : cliArgs) {
                    if (arg.first.substr(0, 2) == "--" && 
                        std::find_if(arg.first.begin(), arg.first.end(), ::isalpha) != arg.first.end()) {
                        // Parameter argument
                        params[arg.first.substr(2)] = arg.second;
                    }
                }
                
                return runExperiment(experiment, params) ? 0 : 1;
                
            } else if (command == "benchmark") {
                std::string benchmarkFile = cliArgs.find("benchmark") != cliArgs.end() ? cliArgs["benchmark"] : "";
                return runBenchmarkSuite(benchmarkFile) ? 0 : 1;
                
            } else if (command == "list") {
                std::string type = cliArgs.find("type") != cliArgs.end() ? cliArgs["type"] : "experiments";
                if (type == "experiments") {
                    showExperiments();
                } else if (type == "benchmarks") {
                    showBenchmarks();
                }
                return static_cast<int>(ErrorCode::SUCCESS);
                
            } else if (command == "export") {
                std::string experiment = cliArgs.find("experiment") != cliArgs.end() ? cliArgs["experiment"] : "";
                std::string output = outputFile_;
                if (output.empty() && cliArgs.find("--output") != cliArgs.end()) {
                    output = cliArgs["--output"];
                }
                if (output.empty()) {
                    throw ExperimentException(ErrorCode::INVALID_ARGUMENTS,
                                            "Output file required for export command");
                }
                return exportResults(output, experiment) ? 0 : 1;
                
            } else {
                throw ExperimentException(ErrorCode::INVALID_ARGUMENTS,
                                        "Unknown command: " + command);
            }
            
        } catch (const ExperimentException& e) {
            NLM_LOG_ERROR("Error: " + std::string(e.what()));
            return static_cast<int>(e.getErrorCode());
        } catch (const std::exception& e) {
            NLM_LOG_ERROR("Unexpected error: " + std::string(e.what()));
            return static_cast<int>(ErrorCode::UNKNOWN_ERROR);
        }
    }
    
    // Additional helper implementations would continue...
    
} // namespace nlm
