# NLM Phase 2 Unified Experiment Framework
// This header provides a unified framework for Phase 2 experiments
// with configurable parameters and enhanced error handling

#pragma once

#include "core/Config/Config.hpp"
#include "core/Constants.hpp"
#include <string>
#include <memory>
#include <vector>
#include <functional>
#include <chrono>
#include <fstream>
#include <sstream>

namespace nlm {
namespace Phase2 {

// Experiment modes
enum class ExperimentMode {
    BASIC_CONNECTIVITY,
    PLASTICITY_LEARNING,
    STDP_VERIFICATION,
    ALL_TESTS,
    BENCHMARK
};

// Experiment results structure
struct ExperimentResult {
    ExperimentMode mode;
    bool success;
    std::string message;
    double executionTime;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    
    // Test-specific metrics
    struct {
        size_t spikesGenerated;
        float meanWeightChange;
        size_t strengthenedSynapses;
        size_t weakenedSynapses;
        size_t unchangedSynapses;
        float totalReward;
    } metrics;
    
    // Configuration used
    std::shared_ptr<Config> config;
    
    ExperimentResult() : success(false), executionTime(0.0), metrics() {}
    
    void startTimer() {
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    void endTimer() {
        auto end = std::chrono::high_resolution_clock::now();
        executionTime = std::chrono::duration<double>(end - startTime).count();
        endTime = end;
    }
    
    std::string getDurationString() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3) << executionTime << "s";
        return oss.str();
    }
};

// Unified experiment runner
class ExperimentRunner {
public:
    ExperimentRunner() = default;
    ~ExperimentRunner() = default;
    
    // Run a specific experiment mode
    ExperimentResult runExperiment(ExperimentMode mode, const Config& config);
    
    // Run all experiments
    std::vector<ExperimentResult> runAllExperiments(const Config& config);
    
    // Run experiments with custom parameters
    std::vector<ExperimentResult> runExperiments(const std::vector<ExperimentMode>& modes, const Config& config);
    
    // Load experiment configuration from file
    static Config loadExperimentConfig(const std::string& configFile);
    
    // Save experiment results to file
    static bool saveResults(const std::vector<ExperimentResult>& results, const std::string& outputFile);
    
    // Generate experiment report
    static std::string generateReport(const std::vector<ExperimentResult>& results);
    
    // Validate configuration for experiment
    static bool validateConfig(const Config& config, ExperimentMode mode);
    
    // Get default configuration for experiment mode
    static Config getDefaultConfig(ExperimentMode mode);
    
private:
    // Internal experiment implementations
    ExperimentResult runBasicConnectivity(const Config& config);
    ExperimentResult runPlasticityLearning(const Config& config);
    ExperimentResult runStdpVerification(const Config& config);
    ExperimentResult runBenchmark(const Config& config);
    
    // Common helper functions
    void applyConfigToBrain(const Config& config, std::shared_ptr<Brain> brain);
    void logExperimentStart(ExperimentMode mode, const Config& config);
    void logExperimentResult(const ExperimentResult& result);
    bool checkExperimentSuccess(ExperimentMode mode, const ExperimentResult& result);
};

// Experiment configuration presets
class ExperimentPresets {
public:
    static Config getQuickTestConfig();
    static Config getComprehensiveConfig();
    static Config getBenchmarkConfig();
    static Config getValidationConfig();
    static Config getPerformanceConfig();
    
    static std::string configToString(const Config& config);
    static bool configFromString(Config& config, const std::string& configStr);
    
private:
    static Config createBaseConfig();
    static void applyPerformanceOptimizations(Config& config);
    static void applyValidationChecks(Config& config);
};

// Experiment validator
class ExperimentValidator {
public:
    struct ValidationResult {
        bool valid;
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
    };
    
    static ValidationResult validateConfig(const Config& config, ExperimentMode mode);
    static ValidationResult validateBrain(const std::shared_ptr<Brain>& brain, ExperimentMode mode);
    static ValidationResult validateEnvironment(const Config& config, ExperimentMode mode);
    
private:
    static bool checkNeuronCount(const Config& config);
    static bool checkTimestep(const Config& config);
    static bool checkPlasticityParameters(const Config& config);
    static bool checkMemoryRequirements(const Config& config);
    static bool checkPerformanceParameters(const Config& config);
    
    static void addError(ValidationResult& result, const std::string& error);
    static void addWarning(ValidationResult& result, const std::string& warning);
};

// Experiment performance monitor
class ExperimentPerformanceMonitor {
public:
    struct PerformanceMetrics {
        double cpuUsage;
        size_t memoryUsageMB;
        size_t totalSpikes;
        float averageFiringRate;
        double simulationTime;
        double wallClockTime;
    };
    
    static PerformanceMetrics startMonitoring();
    static PerformanceMetrics stopMonitoring();
    static PerformanceMetrics getCurrentMetrics();
    static void resetMonitoring();
    
    static std::string formatMetrics(const PerformanceMetrics& metrics);
    static std::string benchmarkExperiment(const std::function<void()>& experimentFunc);
    
private:
    static PerformanceMetrics currentMetrics_;
    static std::chrono::high_resolution_clock::time_point startTime_;
    static bool monitoring_;
};

// Enhanced output formatter
class ExperimentOutputFormatter {
public:
    struct FormatOptions {
        bool showProgress;           // Show progress during experiments
        bool showDetails;            // Show detailed results
        bool showTiming;             // Show timing information
        bool showMemoryUsage;        // Show memory usage
        bool showCPUUsage;           // Show CPU usage
        bool saveToFile;             // Save output to file
        std::string outputFile;      // Output file path
        std::string format;          // Output format (text, json, csv)
    };
    
    static void printExperimentStart(const std::string& title);
    static void printExperimentProgress(ExperimentMode mode, int currentStep, int totalSteps);
    static void printExperimentResult(const ExperimentResult& result);
    static void printExperimentSummary(const std::vector<ExperimentResult>& results);
    static void printPerformanceMetrics(const ExperimentPerformanceMonitor::PerformanceMetrics& metrics);
    static void printConfiguration(const Config& config);
    
    static std::string formatResult(const ExperimentResult& result, const FormatOptions& options = FormatOptions());
    static std::string formatResults(const std::vector<ExperimentResult>& results, const FormatOptions& options = FormatOptions());
    static std::string formatConfiguration(const Config& config);
    
    static std::string getStatusSymbol(bool success);
    static std::string getModeName(ExperimentMode mode);
    static std::string formatTime(double seconds);
    static std::string formatMemory(size_t bytes);
}; 

} // namespace Phase2
} // namespace nlm