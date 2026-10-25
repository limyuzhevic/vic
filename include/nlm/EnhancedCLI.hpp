#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <chrono>

using namespace nlm;

// Command-line options structure
struct NLMOptions {
    bool verbose = false;
    bool quiet = false;
    bool benchmark = false;
    bool configHelp = false;
    std::string configFile = "configs/default.cfg";
    std::string outputFile = "";
    std::string logFile = "";
    std::string logLevel = "INFO";
    std::vector<std::string> enabledTests;
    std::vector<std::string> disabledTests;
    double duration = 0.0;
    SimulationStep maxSteps = 0;
    int randomSeed = 42;
    bool help = false;
    bool version = false;
};

// Command-line interface class
class NLMCommandLine {
public:
    static NLMOptions parseArgs(int argc, char** argv);
    static void printHelp();
    static void printVersion();
    static void printConfigHelp();
    static bool isTestEnabled(const std::string& testName, const NLMOptions& options);
    static bool isTestDisabled(const std::string& testName, const NLMOptions& options);
};

// Configuration loader class
class ConfigLoader {
public:
    static bool loadConfig(std::shared_ptr<Config> config, const std::string& configFile);
    static bool applyCommandLineOverrides(std::shared_ptr<Config> config, int argc, char** argv);
    static void printConfigSummary(const std::shared_ptr<Config>& config);
    static void validateConfig(std::shared_ptr<Config> config);
    static std::string logLevelToString(const std::string& level);
    static LogLevel stringToLogLevel(const std::string& level);
};

// Enhanced logger class
class EnhancedLogger {
public:
    static void setupLogger(const NLMOptions& options);
    static void logBenchmark(const std::string& operation, double duration);
    static void logProgress(int current, int total, const std::string& message);
    static void setBenchmarkMode(bool enabled);
};

// Test framework for modular execution
struct TestResult {
    std::string name;
    bool passed;
    double duration;
    std::string message;
    std::map<std::string, std::string> metrics;
};

using TestFunction = std::function<void(std::shared_ptr<Brain>)>;

class TestFramework {
public:
    static TestResult runTest(const std::string& name, TestFunction testFunc, std::shared_ptr<Brain> brain);
    static std::vector<TestResult> runAllTests(std::shared_ptr<Brain> brain, const NLMOptions& options);
    static void printTestResults(const std::vector<TestResult>& results);
    static void exportTestResults(const std::vector<TestResult>& results, const std::string& outputFile);
    static std::string formatDuration(double seconds);
};

// Integration tester for cognitive systems
class IntegrationTester {
public:
    static void testMemorySystems(std::shared_ptr<Brain> brain);
    static void testPredictionSystems(std::shared_ptr<Brain> brain);
    static void testNeuromodulationSystems(std::shared_ptr<Brain> brain);
    static void testDevelopmentSystems(std::shared_ptr<Brain> brain);
    static void testCognitionSystems(std::shared_ptr<Brain> brain);
    static void testVisualizationIntegration(std::shared_ptr<Brain> brain);
    static void testAgentIntegration(std::shared_ptr<Brain> brain);
};

// Performance monitoring
struct PerformanceMetrics {
    SimulationStep stepsCompleted;
    double simulationTime;
    double wallClockTime;
    size_t memoryUsage;
    float cpuUsage;
    std::map<std::string, double> systemMetrics;
    std::vector<double> spikeRates;
    std::vector<double> firingRates;
};

class PerformanceMonitor {
public:
    static PerformanceMetrics startMonitoring();
    static void stopMonitoring(const PerformanceMetrics& metrics);
    static void logPerformance(const PerformanceMetrics& metrics);
    static std::string formatMetrics(const PerformanceMetrics& metrics);
};

// Configuration validation
class ConfigValidator {
public:
    static bool validate(const std::shared_ptr<Config>& config);
    static std::vector<std::string> getValidationErrors(const std::shared_ptr<Config>& config);
    static bool checkRequiredKeys(const std::shared_ptr<Config>& config);
    static bool checkValueRanges(const std::shared_ptr<Config>& config);
};

// Result exporter for analysis
class ResultExporter {
public:
    static bool exportToJSON(const std::vector<TestResult>& results, const std::string& filepath);
    static bool exportToCSV(const std::vector<TestResult>& results, const std::string& filepath);
    static bool exportToTOML(const std::vector<TestResult>& results, const std::string& filepath);
    static std::string generateAnalysisReport(const std::vector<TestResult>& results);
};

// Main header with all declarations
// This would typically be split across multiple headers
// For now, keeping it in one header for simplicity
