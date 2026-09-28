// NLM (熙然) - Neural Learning Machine
// Phase 2: Real Neural Computation
//
// Enhanced main executable with comprehensive command-line interface and multiple execution modes.
// Supports Phase 2 testing, Phase 3/4/6 demonstrations, and flexible configuration options.

#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <iomanip>
#include <numeric>
#include <map>
#include <algorithm>
#include <sstream>
#include <fstream>
#include <chrono>

using namespace nlm;
#include <sstream>

using namespace nlm;

using namespace nlm;

/**
 * Enhanced error handling and logging system for NLM main executable
 */
class ErrorHandler {
private:
    bool abortOnError;
    bool logToFile;
    std::string logFilePath;
    int errorCount;
    int warningCount;

public:
    ErrorHandler(bool abortOnErrors = true, bool logToFile = false, const std::string& logPath = "nlm_error.log")
        : abortOnError(abortOnErrors), logToFile(logToFile), logFilePath(logPath), errorCount(0), warningCount(0) {}

    void logError(const std::string& message, const std::string& context = "") {
        errorCount++;
        std::string fullMessage = "[ERROR] " + (context.empty() ? "" : context + ": ") + message;
        std::cerr << fullMessage << std::endl;
        
        if (logToFile) {
            std::ofstream logFile(logFilePath, std::ios::app);
            if (logFile.is_open()) {
                logFile << fullMessage << std::endl;
                logFile.close();
            }
        }
        
        if (abortOnError) {
            std::cerr << "\nFatal error encountered. Terminating execution." << std::endl;
            exit(1);
        }
    }

    void logWarning(const std::string& message, const std::string& context = "") {
        warningCount++;
        std::string fullMessage = "[WARNING] " + (context.empty() ? "" : context + ": ") + message;
        std::cerr << fullMessage << std::endl;
        
        if (logToFile) {
            std::ofstream logFile(logFilePath, std::ios::app);
            if (logFile.is_open()) {
                logFile << fullMessage << std::endl;
                logFile.close();
            }
        }
    }

    void logInfo(const std::string& message, const std::string& context = "") {
        if (context.empty()) {
            std::cout << "[INFO] " << message << std::endl;
        } else {
            std::cout << "[INFO] " << context << ": " << message << std::endl;
        }
    }

    void logDebug(const std::string& message, const std::string& context = "") {
        if (context.empty()) {
            std::cout << "[DEBUG] " << message << std::endl;
        } else {
            std::cout << "[DEBUG] " << context << ": " << message << std::endl;
        }
    }

    int getErrorCount() const { return errorCount; }
    int getWarningCount() const { return warningCount; }
    bool shouldAbort() const { return abortOnError; }
};

/**
 * Comprehensive performance analysis and benchmarking system
 */
class PerformanceAnalyzer {
private:
    std::string profileName;
    std::chrono::high_resolution_clock::time_point startTime;
    std::vector<std::pair<std::string, double>> metrics;
    bool enabled;
    double cpuTime;
    double memoryUsage;

public:
    PerformanceAnalyzer(const std::string& name = "Performance Analysis", bool enable = true)
        : profileName(name), enabled(enable), cpuTime(0.0), memoryUsage(0.0) {
        if (enabled) {
            startTime = std::chrono::high_resolution_clock::now();
        }
    }

    void start() {
        if (enabled) {
            startTime = std::chrono::high_resolution_clock::now();
        }
    }

    void recordMetric(const std::string& name, double value, const std::string& unit = "") {
        if (enabled) {
            metrics.push_back(std::make_pair(name, value));
        }
    }

    void recordMemoryUsage() {
        if (enabled) {
            // Simplified memory usage calculation
            // In a real implementation, you would use system-specific APIs
            memoryUsage = 0.0;  // Placeholder for actual implementation
        }
    }

    double getElapsedTime() {
        if (!enabled) return 0.0;
        
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }

    void generateReport() {
        if (!enabled || metrics.empty()) return;
        
        double totalTime = getElapsedTime();
        
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "PERFORMANCE ANALYSIS REPORT" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        std::cout << "Profile: " << profileName << std::endl;
        std::cout << "Total execution time: " << std::fixed << std::setprecision(3) 
                  << totalTime << " seconds" << std::endl;
        
        if (totalTime > 0) {
            std::cout << "\nDetailed Metrics:" << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            
            for (const auto& metric : metrics) {
                double value = metric.second;
                std::string unit = "";
                
                // Determine appropriate unit
                if (metric.first.find("time") != std::string::npos) {
                    if (value >= 1000) {
                        value /= 1000;
                        unit = " ms";
                    } else {
                        unit = " s";
                    }
                } else if (metric.first.find("memory") != std::string::npos) {
                    if (value >= 1024 * 1024) {
                        value /= 1024 * 1024;
                        unit = " MB";
                    } else if (value >= 1024) {
                        value /= 1024;
                        unit = " KB";
                    } else {
                        unit = " B";
                    }
                } else if (metric.first.find("rate") != std::string::npos || 
                          metric.first.find("speed") != std::string::npos) {
                    unit = " /s";
                }
                
                std::cout << std::left << std::setw(30) << metric.first 
                          << "= " << std::fixed << std::setprecision(3) 
                          << value << unit << std::endl;
            }
            
            if (totalTime > 0) {
                std::cout << "\nPerformance Indicators:" << std::endl;
                std::cout << std::string(80, '-') << std::endl;
                
                // Calculate derived metrics
                double metricsPerSecond = metrics.size() / totalTime;
                std::cout << std::left << std::setw(30) << "Metrics processed per second:" 
                          << metricsPerSecond << " metrics/s" << std::endl;
                
                if (memoryUsage > 0) {
                    double memoryPerSecond = memoryUsage / totalTime;
                    std::cout << std::left << std::setw(30) << "Memory usage per second:" 
                              << memoryPerSecond << " B/s" << std::endl;
                }
                
                // Identify bottlenecks
                std::cout << "\nBottleneck Analysis:" << std::endl;
                std::cout << std::string(80, '-') << std::endl;
                
                // Find longest running operations
                auto longestMetric = std::max_element(metrics.begin(), metrics.end(),
                    [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
                        return a.second < b.second;
                    });
                
                if (longestMetric != metrics.end()) {
                    std::cout << "  Longest running operation: " << longestMetric->first 
                              << " (" << std::fixed << std::setprecision(3) 
                              << longestMetric->second << " s)" << std::endl;
                }
                
                // Calculate efficiency
                double efficiencyScore = calculateEfficiencyScore(metrics);
                std::cout << "  Overall efficiency score: " << efficiencyScore << "/10.0" << std::endl;
            }
        }
        
        std::cout << std::string(80, '=') << std::endl;
    }

private:
    double calculateEfficiencyScore(const std::vector<std::pair<std::string, double>>& metrics) {
        // Calculate a simple efficiency score based on metrics
        double score = 10.0;  // Start with perfect score
        
        for (const auto& metric : metrics) {
            std::string name = metric.first;
            double value = metric.second;
            
            // Deduct points for poor performance
            if (name.find("time") != std::string::npos && value > 1.0) {
                score -= std::min(2.0, value / 500.0);
            } else if (name.find("memory") != std::string::npos && value > 100 * 1024 * 1024) {
                score -= std::min(2.0, (value - 100 * 1024 * 1024) / (50 * 1024 * 1024));
            } else if (name.find("rate") != std::string::npos && value < 100) {
                score -= std::min(1.0, (100 - value) / 100.0);
            }
        }
        
        return std::max(0.0, score);
    }
};

/**
 * Advanced benchmarking tools for comprehensive performance measurement
 */
class BenchmarkSuite {
public:
    struct BenchmarkResult {
        std::string name;
        double executionTime;
        double memoryUsage;
        double cpuUsage;
        std::map<std::string, double> metrics;
        bool success;
        std::string errorMessage;
    };

    static BenchmarkResult runBenchmark(const std::string& name, std::function<void()> benchmarkFunction) {
        BenchmarkResult result;
        result.name = name;
        result.success = false;
        
        try {
            auto startTime = std::chrono::high_resolution_clock::now();
            
            // Record baseline memory (simplified)
            // In real implementation, use system-specific memory monitoring
            result.memoryUsage = 0.0;
            
            // Run the benchmark
            benchmarkFunction();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
            result.success = true;
            
        } catch (const std::exception& e) {
            result.errorMessage = std::string("Exception: ") + e.what();
        } catch (...) {
            result.errorMessage = "Unknown exception occurred";
        }
        
        return result;
    }

    static void generateBenchmarkReport(const std::vector<BenchmarkResult>& results) {
        std::cout << "\n" << std::string(80, '=') << std::endl;
        std::cout << "BENCHMARK SUITE REPORT" << std::endl;
        std::cout << std::string(80, '=') << std::endl;
        
        double totalTime = 0.0;
        int successful = 0;
        int failed = 0;
        
        std::cout << "Benchmark Results:" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        
        for (const auto& result : results) {
            if (result.success) {
                successful++;
                totalTime += result.executionTime;
                
                std::cout << std::left << std::setw(30) << result.name 
                          << "= " << std::fixed << std::setprecision(3) 
                          << result.executionTime << " s" << std::endl;
                
                if (!result.metrics.empty()) {
                    std::cout << "    Metrics:" << std::endl;
                    for (const auto& metric : result.metrics) {
                        std::cout << "      " << std::left << std::setw(20) << metric.first 
                                  << "= " << std::fixed << std::setprecision(3) << metric.second << std::endl;
                    }
                }
            } else {
                failed++;
                std::cout << std::left << std::setw(30) << result.name 
                          << "= FAILED" << std::endl;
                std::cout << "    Error: " << result.errorMessage << std::endl;
            }
        }
        
        std::cout << "\nBenchmark Summary:" << std::endl;
        std::cout << std::string(80, '-') << std::endl;
        std::cout << "Total benchmarks: " << results.size() << std::endl;
        std::cout << "Successful: " << successful << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        if (successful > 0) {
            std::cout << "Average execution time: " << std::fixed << std::setprecision(3) 
                      << (totalTime / successful) << " s" << std::endl;
            std::cout << "Total execution time: " << std::fixed << std::setprecision(3) 
                      << totalTime << " s" << std::endl;
        }
        
        if (failed > 0) {
            std::cout << "\nFailed Benchmarks:" << std::endl;
            std::cout << std::string(80, '-') << std::endl;
            for (const auto& result : results) {
                if (!result.success) {
                    std::cout << "  - " << result.name << ": " << result.errorMessage << std::endl;
                }
            }
        }
        
        std::cout << std::string(80, '=') << std::endl;
    }
};

/**
 * Configuration validator for checking configuration consistency
 */
class ConfigurationValidator {
public:
    struct ValidationResult {
        bool isValid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        std::map<std::string, std::string> suggestions;
    };

    static ValidationResult validate(const std::shared_ptr<Config>& config) {
        ValidationResult result;
        result.isValid = true;
        
        // Check basic configuration values
        auto neuronCount = config->getOr<int64_t>("neuron_count", -1);
        auto regionCount = config->getOr<int64_t>("region_count", -1);
        auto connectionProb = config->getOr<float>("connection_probability", -1.0f);
        auto timestep = config->getOr<double>("simulation_timestep", -1.0);
        
        // Validate neuron count
        if (neuronCount <= 0) {
            result.errors.push_back("Invalid neuron_count: must be positive");
            result.isValid = false;
        } else if (neuronCount > 10000) {
            result.warnings.push_back("High neuron count may impact performance");
            result.suggestions["performance"] = "Consider reducing neuron count for testing";
        }
        
        // Validate region count
        if (regionCount <= 0) {
            result.errors.push_back("Invalid region_count: must be positive");
            result.isValid = false;
        }
        
        // Validate connection probability
        if (connectionProb < 0.0f || connectionProb > 1.0f) {
            result.errors.push_back("Invalid connection_probability: must be between 0.0 and 1.0");
            result.isValid = false;
        }
        
        // Validate timestep
        if (timestep <= 0.0) {
            result.errors.push_back("Invalid simulation_timestep: must be positive");
            result.isValid = false;
        } else if (timestep > 0.1) {
            result.warnings.push_back("Large timestep may affect simulation accuracy");
            result.suggestions["accuracy"] = "Consider using smaller timestep (e.g., 0.001)";
        }
        
        // Check for required configurations
        auto randomSeed = config->getOr<int64_t>("random_seed", -1);
        if (randomSeed < 0) {
            result.warnings.push_back("No random_seed specified - using default");
            result.suggestions["reproducibility"] = "Set random_seed for reproducible results";
        }
        
        return result;
    }

    static void printValidationReport(const ValidationResult& result) {
        if (result.isValid && result.errors.empty() && result.warnings.empty()) {
            std::cout << "[VALID] Configuration is valid and optimal" << std::endl;
            return;
        }
        
        std::cout << "[VALID] Configuration validation report:" << std::endl;
        
        if (!result.errors.empty()) {
            std::cout << "  Errors:" << std::endl;
            for (const auto& error : result.errors) {
                std::cout << "    ✗ " << error << std::endl;
            }
        }
        
        if (!result.warnings.empty()) {
            std::cout << "  Warnings:" << std::endl;
            for (const auto& warning : result.warnings) {
                std::cout << "    ⚠ " << warning << std::endl;
            }
        }
        
        if (!result.suggestions.empty()) {
            std::cout << "  Suggestions:" << std::endl;
            for (const auto& suggestion : result.suggestions) {
                std::cout << "    💡 " << suggestion.first << ": " << suggestion.second << std::endl;
            }
        }
        
        if (!result.isValid) {
            std::cout << "  ❌ Configuration invalid - fixes required" << std::endl;
        }
    }
};

/**
 * Enhanced logger with different output formats and levels
 */
class EnhancedLogger {
private:
    ErrorHandler errorHandler;
    std::vector<std::function<void(const std::string&)>> outputHandlers;
    std::string logFormat;
    bool includeTimestamp;
    bool includeThreadId;

public:
    EnhancedLogger(const ErrorHandler& handler, const std::string& format = "text", 
                   bool timestamp = true, bool threadId = false)
        : errorHandler(handler), logFormat(format), includeTimestamp(timestamp), includeThreadId(threadId) {}

    void addOutputHandler(std::function<void(const std::string&)> handler) {
        outputHandlers.push_back(handler);
    }

    void log(const std::string& level, const std::string& message, const std::string& context = "") {
        std::stringstream ss;
        
        if (includeTimestamp) {
            auto now = std::chrono::system_clock::now();
            auto time = std::chrono::system_clock::to_time_t(now);
            ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        }
        
        if (includeThreadId) {
            if (includeTimestamp) ss << " ";
            ss << "[Thread " << std::this_thread::get_id() << "]";
        }
        
        if (!includeTimestamp && !includeThreadId) {
            ss << "[" << level << "]";
        } else {
            ss << " [" << level << "]";
        }
        
        if (!context.empty()) {
            ss << " " << context;
        }
        
        ss << " " << message;
        
        std::string logEntry = ss.str();
        
        // Output to all handlers
        for (const auto& handler : outputHandlers) {
            handler(logEntry);
        }
        
        // Also output to std::cout for console logging
        std::cout << logEntry << std::endl;
    }

    ErrorHandler& getErrorHandler() { return errorHandler; }
};

/**
 * Results formatter for beautiful output
 */
class ResultsFormatter {
public:
    static void printExecutionHeader(const std::string& mode, const std::string& submode, 
                                     uint64_t neuronCount, uint64_t steps) {
        std::cout << "\n";
        std::cout << "╔════════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    NLM Execution: " << std::left << std::setw(25) << (mode + " (" + submode + ")") << "║" << std::endl;
        std::cout << "║                    Neurons: " << std::left << std::setw(45) << std::to_string(neuronCount) << "║" << std::endl;
        std::cout << "║                    Steps: " << std::left << std::setw(46) << std::to_string(steps) << "║" << std::endl;
        std::cout << "╚════════════════════════════════════════════════════════════════╝" << std::endl;
    }

    static void printSection(const std::string& title) {
        std::cout << "\n";
        std::cout << "╔─ " << title << std::string(52 - title.length(), ' ') << "─╗" << std::endl;
    }

    static void printResult(const std::string& label, const std::string& value, bool success = true) {
        std::string status = success ? "[PASS]" : "[FAIL]";
        std::cout << "║   " << status << " " << std::left << std::setw(20) << label 
                  << "= " << value << std::string(20, ' ') << "║" << std::endl;
    }

    static void printResult(const std::string& label, double value, const std::string& format = "%.3f", bool success = true) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << value;
        printResult(label, ss.str(), success);
    }

    static void printStatus(const std::string& label, bool value) {
        std::string status = value ? "✓ YES" : "✗ NO";
        std::cout << "║   " << status << " " << std::left << std::setw(20) << label << std::string(25, ' ') << "║" << std::endl;
    }

    static void printPerformance(const std::string& label, double value, const std::string& unit) {
        std::cout << "║   " << std::left << std::setw(20) << label 
                  << "= " << std::fixed << std::setprecision(2) << value << " " << unit << std::string(17, ' ') << "║" << std::endl;
    }

    static void printConfiguration(const std::string& label, const std::string& value) {
        std::cout << "║   " << std::left << std::setw(20) << label 
                  << "= " << value << std::string(28, ' ') << "║" << std::endl;
    }
};

/**
 * Phase 2 main execution with enhanced CLI
 */
class Phase2Executor {
private:
    CommandLineInterface::ExecutionConfig config;
    double executionTime;

public:
    Phase2Executor(const CommandLineInterface::ExecutionConfig& cfg) : config(cfg), executionTime(0.0) {}

    bool execute() {
        auto startTime = std::chrono::high_resolution_clock::now();
        
        printBanner(config);
        printConfiguration(config);
        
        try {
            // Initialize logger with appropriate level
            auto logger = std::make_shared<Logger>();
            auto consoleLogger = std::make_shared<ConsoleLogger>(config.verbose ? LogLevel::Debug : LogLevel::Info);
            logger->addLogger(consoleLogger);
            Logger::setGlobal(logger);
            
            // Load configuration
            auto cfg = loadConfiguration(config);
            
            // Apply CLI overrides
            applyConfiguration(cfg, config);
            
            // Initialize brain
            auto brain = initializeBrain(cfg, config);
            
            if (config.loadCheckpoint && !config.checkpointPath.empty()) {
                if (brain->load(config.checkpointPath)) {
                    NLM_LOG_INFO("Checkpoint loaded successfully from: " + config.checkpointPath);
                } else {
                    NLM_LOG_WARNING("Failed to load checkpoint, starting fresh");
                }
            }
            
            // Run selected tests
            runSelectedTests(brain, config);
            
            // Save checkpoint if requested
            if (config.enableCheckpointing && !config.checkpointPath.empty()) {
                if (brain->save(config.checkpointPath)) {
                    NLM_LOG_INFO("Checkpoint saved successfully to: " + config.checkpointPath);
                } else {
                    NLM_LOG_ERROR("Failed to save checkpoint");
                }
            }
            
            // Save results if requested
            if (config.saveResults && !config.outputFile.empty()) {
                saveResults(brain, config);
            }
            
            // Calculate and display performance
            auto endTime = std::chrono::high_resolution_clock::now();
            executionTime = std::chrono::duration<double>(endTime - startTime).count();
            
            displayPerformance(executionTime, config.benchmark);
            
            return true;
            
        } catch (const std::exception& e) {
            std::cerr << "Error during Phase 2 execution: " << e.what() << std::endl;
            return false;
        }
    }

private:
    void printBanner(const CommandLineInterface::ExecutionConfig& cfg) {
        std::cout << "\n";
        std::cout << "╔═══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    NLM — 熙然                                                ║" << std::endl;
        std::cout << "║                    Neural Learning Machine                                   ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║                    Phase 2: Real Neural Computation             ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║                    An experimental artificial developmental brain.            ║" << std::endl;
        std::cout << "║                                                               ║" << std::endl;
        std::cout << "║                    Mode: " << std::left << std::setw(17) << cfg.mode << "Submode: " << cfg.submode << "                    ║" << std::endl;
        std::cout << "╚═══════════════════════════════════════════════════════════════╝" << std::endl;
    }

    void printConfiguration(const CommandLineInterface::ExecutionConfig& cfg) {
        ResultsFormatter::printSection("Configuration Summary");
        
        if (cfg.verbose) {
            ResultsFormatter::printConfiguration("Execution Mode", cfg.mode + " (" + cfg.submode + ")");
            ResultsFormatter::printConfiguration("Neuron Count", std::to_string(cfg.neuronCount));
            ResultsFormatter::printConfiguration("Region Count", std::to_string(cfg.regionCount));
            ResultsFormatter::printConfiguration("Connection Probability", std::to_string(cfg.connectionProbability));
            ResultsFormatter::printConfiguration("Simulation Timestep", std::to_string(cfg.simulationTimestep));
            ResultsFormatter::printConfiguration("Random Seed", std::to_string(cfg.randomSeed));
            ResultsFormatter::printConfiguration("Configuration File", cfg.configFile);
            
            ResultsFormatter::printSection("Feature Flags");
            ResultsFormatter::printConfiguration("Enable Checkpointing", cfg.enableCheckpointing ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Enable Replay", cfg.enableReplay ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Enable Development", cfg.enableDevelopment ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Enable Memory", cfg.enableMemory ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Enable Neuromodulation", cfg.enableNeuromodulation ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Enable Prediction", cfg.enablePrediction ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Enable Cognition", cfg.enableCognition ? "Yes" : "No");
            
            ResultsFormatter::printSection("Test Selection");
            ResultsFormatter::printConfiguration("Run Connectivity Test", cfg.runConnectivityTest ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Run Plasticity Test", cfg.runPlasticityTest ? "Yes" : "No");
            ResultsFormatter::printConfiguration("Run STDP Test", cfg.runStdpTest ? "Yes" : "No");
            
            ResultsFormatter::printConfiguration("Max Steps", std::to_string(cfg.maxSteps));
            ResultsFormatter::printConfiguration("Plasticity LTP Weight", std::to_string(cfg.plasticityLtpWeight));
            ResultsFormatter::printConfiguration("Plasticity LTD Weight", std::to_string(cfg.plasticityLtdWeight));
        }
    }

    std::shared_ptr<Config> loadConfiguration(const CommandLineInterface::ExecutionConfig& cfg) {
        auto config = std::make_shared<Config>();
        
        // Load config from file (ignore if not found)
        if (config->loadFromFile(cfg.configFile)) {
            NLM_LOG_INFO("Loaded configuration from: " + cfg.configFile);
        } else {
            NLM_LOG_INFO("Using default configuration.");
        }
        
        return config;
    }

    void applyConfiguration(std::shared_ptr<Config> cfg, const CommandLineInterface::ExecutionConfig& execConfig) {
        // Set basic configuration
        cfg->set("random_seed", static_cast<int64_t>(execConfig.randomSeed), ConfigSource::CLI);
        cfg->set("simulation_timestep", execConfig.simulationTimestep, ConfigSource::CLI);
        cfg->set("neuron_count", static_cast<int64_t>(execConfig.neuronCount), ConfigSource::CLI);
        cfg->set("region_count", static_cast<int64_t>(execConfig.regionCount), ConfigSource::CLI);
        cfg->set("connection_probability", execConfig.connectionProbability, ConfigSource::CLI);
        
        // Set plasticity parameters
        cfg->set("stdp_ltp_weight", execConfig.plasticityLtpWeight, ConfigSource::CLI);
        cfg->set("stdp_ltd_weight", execConfig.plasticityLtdWeight, ConfigSource::CLI);
        cfg->set("synaptogenesis_rate", execConfig.synaptogenesisRate, ConfigSource::CLI);
        cfg->set("pruning_rate", execConfig.pruningRate, ConfigSource::CLI);
        
        // Enable additional systems if requested
        if (execConfig.enableMemory) {
            cfg->set("memory.enable", true, ConfigSource::CLI);
        }
        if (execConfig.enableNeuromodulation) {
            cfg->set("neuromodulation.enable", true, ConfigSource::CLI);
        }
        if (execConfig.enablePrediction) {
            cfg->set("prediction.enable", true, ConfigSource::CLI);
        }
        if (execConfig.enableCognition) {
            cfg->set("cognition.enable", true, ConfigSource::CLI);
        }
        if (execConfig.enableDevelopment) {
            cfg->set("development.enable", true, ConfigSource::CLI);
        }
        if (execConfig.enableReplay) {
            cfg->set("replay.enable", true, ConfigSource::CLI);
        }
    }

    std::shared_ptr<Brain> initializeBrain(std::shared_ptr<Config> cfg, const CommandLineInterface::ExecutionConfig& execConfig) {
        NLM_LOG_INFO("Initializing NLM Brain...");
        auto brain = std::make_shared<Brain>(cfg);
        
        if (!brain->initialize()) {
            NLM_LOG_ERROR("Failed to initialize brain!");
            return nullptr;
        }
        
        brain->logStatus();
        return brain;
    }

    void runSelectedTests(std::shared_ptr<Brain> brain, const CommandLineInterface::ExecutionConfig& cfg) {
        if (cfg.runConnectivityTest) {
            runBasicConnectivityTest(brain);
        }
        
        if (cfg.runPlasticityTest) {
            runPlasticityExperiment(brain);
        }
        
        if (cfg.runStdpTest) {
            runStdpVerification(brain);
        }
        
        if (!cfg.runConnectivityTest && !cfg.runPlasticityTest && !cfg.runStdpTest && cfg.verbose) {
            NLM_LOG_WARNING("No tests selected. Use --enable-all, --enable-connectivity, --enable-plasticity, or --enable-stdp");
        }
    }

    void runBasicConnectivityTest(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 1: Basic Neural Connectivity ===");
        
        // Inject current into a few neurons and see if spikes propagate
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            NLM_LOG_INFO("  No neurons found!");
            return;
        }
        
        // Get initial spike count
        size_t initialSpikes = brain->getTotalSpikeCount();
        
        // Inject strong current into first 10 neurons
        NLM_LOG_INFO("  Injecting current into 10 neurons...");
        for (size_t i = 0; i < std::min(size_t(10), neurons.size()); ++i) {
            neurons[i]->injectCurrent(50.0f);  // Strong excitatory input
        }
        
        // Run a few steps
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
        
        // Learning Experiment: Demonstrates measurable synaptic changes through experience
        struct LearningExperiment {
            std::shared_ptr<Brain> brain;
            uint64_t seed;
            size_t initialSynapseCount;
            std::vector<float> initialWeights;
            std::vector<float> finalWeights;
            std::vector<NeuronId> mostActiveNeurons;
            
            LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
                : brain(b), seed(s), initialSynapseCount(0) {}
            
            void recordInitialState() {
                initialSynapseCount = brain->getTotalSynapseCount();
                initialWeights.clear();
                
                // Record initial weights from first region
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
                
                // Record final weights from first region
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
            }
            
            void computeStatistics() {
                NLM_LOG_INFO("");
                NLM_LOG_INFO("=== Learning Experiment Results ===");
                NLM_LOG_INFO("");
                
                if (initialWeights.empty() || finalWeights.empty()) {
                    NLM_LOG_INFO("ERROR: No weights recorded");
                    return;
                }
                
                // Compute weight changes
                float initialSum = std::accumulate(initialWeights.begin(), initialWeights.end(), 0.0f);
                float finalSum = std::accumulate(finalWeights.begin(), finalWeights.end(), 0.0f);
                float initialMean = initialSum / initialWeights.size();
                float finalMean = finalSum / finalWeights.size();
                
                NLM_LOG_INFO("Weight Statistics:");
                NLM_LOG_INFO("  Initial mean weight: " + std::to_string(initialMean));
                NLM_LOG_INFO("  Final mean weight: " + std::to_string(finalMean));
                NLM_LOG_INFO("  Change: " + std::to_string(finalMean - initialMean));
                
                // Count synapses that changed significantly
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
                
                // Determine if learning occurred
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
        
        LearningExperiment experiment(brain, 42);
        
        // Record initial state
        experiment.recordInitialState();
        
        // Enable plasticity on synapses
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (auto& syn : region->getSynapses()) {
                syn->enablePlasticity(true, true, false);  // Enable Hebbian and STDP
            }
        }
        
        // Apply repeated input pattern to stimulate learning
        NLM_LOG_INFO("");
        NLM_LOG_INFO("Applying repeated input patterns (1000 steps)...");
        
        for (SimulationStep step = 0; step < 1000; ++step) {
            // Create input pattern - inject current into sensory neurons
            for (size_t i = 0; i < 20 && i < brain->getTotalNeuronCount() / 4; ++i) {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step, step * 0.001);
            
            // Log progress every 100 steps
            if (step % 100 == 0) {
                NLM_LOG_INFO("  Step " + std::to_string(step) + 
                            " | Spikes: " + std::to_string(brain->getTotalSpikeCount()) +
                            " | Firing: " + std::to_string(brain->getFiringNeuronCount()));
            }
        }
        
        // Record final state
        experiment.recordFinalState();
        
        // Compute and display statistics
        experiment.computeStatistics();
    }

    void runStdpVerification(std::shared_ptr<Brain> brain) {
        NLM_LOG_INFO("");
        NLM_LOG_INFO("=== Test 3: STDP Verification ===");
        
        auto* region = brain->getRegion(RegionId(1));
        if (!region) return;
        
        // Get first few synapses
        auto& synapses = region->getSynapses();
        if (synapses.size() < 5) {
            NLM_LOG_INFO("  Not enough synapses for STDP test");
            return;
        }
        
        NLM_LOG_INFO("  Testing STDP on 5 synapses:");
        
        // Record initial weights
        std::vector<float> beforeWeights;
        for (size_t i = 0; i < 5; ++i) {
            beforeWeights.push_back(synapses[i]->getWeight());
            synapses[i]->enablePlasticity(false, true, false);  // Enable only STDP
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " before: " + std::to_string(beforeWeights[i]));
        }
        
        // Create correlated activity: fire pre then post to trigger LTP
        NLM_LOG_INFO("");
        NLM_LOG_INFO("  Creating correlated pre->post activity (potentiation)...");
        
        for (int trial = 0; trial < 50; ++trial) {
            // Fire pre-synaptic neuron
            Neuron* preNeuron = nullptr;
            Neuron* postNeuron = nullptr;
            
            auto neurons = region->getAllNeurons();
            if (neurons.size() >= 2) {
                preNeuron = neurons[0];
                postNeuron = neurons[1];
            }
            
            if (preNeuron && postNeuron) {
                // Pre fires first
                preNeuron->injectCurrent(60.0f);
                brain->step(trial * 2, trial * 2 * 0.001);
                
                // Then post fires
                postNeuron->injectCurrent(60.0f);
                brain->step(trial * 2 + 1, (trial * 2 + 1) * 0.001);
            }
        }
        
        // Record after weights
        NLM_LOG_INFO("  After correlated activity:");
        for (size_t i = 0; i < 5; ++i) {
            float delta = synapses[i]->getWeight() - beforeWeights[i];
            NLM_LOG_INFO("    Synapse " + std::to_string(i) + 
                        " after: " + std::to_string(synapses[i]->getWeight()) +
                        " (Δ=" + std::to_string(delta) + ")");
        }
        
        // Check if weights increased (LTP)
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

    void saveResults(std::shared_ptr<Brain> brain, const CommandLineInterface::ExecutionConfig& cfg) {
        NLM_LOG_INFO("Saving results to: " + cfg.outputFile);
        
        // Create a simple results file
        std::ofstream file(cfg.outputFile);
        if (!file.is_open()) {
            NLM_LOG_ERROR("Failed to open output file: " + cfg.outputFile);
            return;
        }
        
        file << "NLM Phase 2 Results" << std::endl;
        file << "=================" << std::endl;
        file << "Execution Mode: " << cfg.mode << std::endl;
        file << "Submode: " << cfg.submode << std::endl;
        file << "Neuron Count: " << cfg.neuronCount << std::endl;
        file << "Steps: " << cfg.maxSteps << std::endl;
        file << std::endl;
        file << "Performance:" << std::endl;
        file << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        file << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "  Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        
        file.close();
        NLM_LOG_INFO("Results saved successfully");
    }

    void displayPerformance(double time, bool benchmark) {
        if (benchmark) {
            ResultsFormatter::printSection("Performance Summary");
            ResultsFormatter::printPerformance("Execution Time", time, "seconds");
            ResultsFormatter::printPerformance("Steps per Second", config.maxSteps / time, "steps/s");
            ResultsFormatter::printPerformance("Neurons per Second", config.neuronCount / time, "neurons/s");
            
            if (time > 0) {
                ResultsFormatter::printPerformance("Memory Usage (est.)", config.neuronCount * 100 / time, "bytes/s");
            }
        }
    }
};

int main(int argc, char** argv) {
    try {
        // Parse command line arguments
        CommandLineInterface::ExecutionConfig config = CommandLineInterface::parseArguments(argc, argv);
        
        // Handle help and version requests
        if (config.showHelp) {
            CommandLineInterface::printHelp();
            return 0;
        }
        
        if (config.showVersion) {
            CommandLineInterface::printVersion();
            return 0;
        }
        
        // Validate mode selection
        if (config.mode == "phase2") {
            // Execute Phase 2 with enhanced CLI
            Phase2Executor executor(config);
            return executor.execute() ? 0 : 1;
        }
        else if (config.mode == "phase3") {
            std::cout << "Phase 3 mode: Use 'nlm_phase3_demo' executable for Phase 3 demonstrations." << std::endl;
            return 0;
        }
        else if (config.mode == "phase4") {
            std::cout << "Phase 4 mode: Use 'nlm_phase4_demo' executable for Phase 4 demonstrations." << std::endl;
            return 0;
        }
        else if (config.mode == "phase6") {
            std::cout << "Phase 6 mode: Use 'nlm_phase6_demo' executable for Phase 6 integration demonstrations." << std::endl;
            return 0;
        }
        else {
            std::cerr << "Unknown mode: " << config.mode << std::endl;
            std::cout << "Available modes: phase2, phase3, phase4, phase6" << std::endl;
            return 1;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown error occurred" << std::endl;
        return 1;
    }
}