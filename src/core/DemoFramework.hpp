// NLM (熙然) - Neural Learning Machine
// Unified Demo/Test Framework
//
// This module provides a comprehensive command-line interface for running
// different NLM demonstrations, tests, and benchmarks with advanced options
// for both beginners and advanced users.

#pragma once

#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <chrono>
#include <map>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Demo/Test modes
enum class DemoMode {
    BASIC_CONNECTIVITY,  // Test basic neural connectivity
    PLASTICITY_LEARNING, // Plasticity learning experiment
    STDP_VERIFICATION,   // STDP mechanism verification
    PHASE6_INTEGRATION,   // Phase 6 integration test
    TEXT_DEMO,           // Text teaching and thought demo
    BENCHMARK,           // Performance benchmark
    ALL_TESTS,          // Run all tests sequentially
    CUSTOM_CONFIG       // Run with custom configuration
};

// Command-line options structure
struct DemoOptions {
    DemoMode mode;
    std::string configFile;
    std::string outputFile;
    std::string checkpointPath;
    bool verbose;
    bool quiet;
    bool progress;
    bool saveResults;
    std::vector<std::string> customArgs;
    uint64_t steps;
    size_t neuronCount;
    size_t regionCount;
    float connectionProbability;
    std::string logLevel;
    bool enableColors;
    std::string profileOutput;
    bool runContinuous;
    uint64_t runTime;
    size_t maxMemoryUsage;
    bool enableAdvanced;
};

// Progress monitoring
class ProgressMonitor {
public:
    ProgressMonitor();
    void start(const std::string& description);
    void update(float progress, const std::string& message = "");
    void complete(const std::string& message = "");
    bool shouldContinue() const;
    void setUpdateInterval(float interval);
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    void start(const std::string& label);
    void stop();
    void reset();
    double getCurrentTime() const;
    double getTotalTime() const;
    const std::map<std::string, double>& getResults() const;
    void printResults() const;
    void saveResults(const std::string& filename) const;
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Main demo runner class
class DemoRunner {
public:
    DemoRunner();
    ~DemoRunner();
    
    // Parse command-line arguments
    bool parseArguments(int argc, char** argv, DemoOptions& options);
    
    // Parse configuration from file
    bool parseConfigFile(const std::string& filename, DemoOptions& options);
    
    // Parse command-line specific arguments
    bool parseDemoArguments(const std::vector<std::string>& args, DemoOptions& options);
    
    // Setup logging based on options
    void setupLogging(const DemoOptions& options);
    
    // Run a specific demo mode
    bool runDemo(const DemoOptions& options);
    
    // Run all demos
    bool runAllDemos(const DemoOptions& options);
    
    // Save results to file
    bool saveResults(const std::string& filename, const std::string& results);
    
    // Load results from file
    bool loadResults(const std::string& filename, std::string& results);
    
    // Print usage information
    void printUsage() const;
    
    // Print version information
    void printVersion() const;
    
    // Get demo name from mode
    static std::string getDemoName(DemoMode mode);
    
    // Get demo description from mode
    static std::string getDemoDescription(DemoMode mode);
    
    // List all available demos
    static void listAvailableDemos();
    
    // Get current timestamp for file naming
    static std::string getTimestamp();
    
    // Validate configuration options
    static bool validateOptions(const DemoOptions& options);
    
    // Set working directory for outputs
    void setWorkingDirectory(const std::string& directory);
    
    // Get default configuration path
    static std::string getDefaultConfigPath();
    
    // Get available config files
    static std::vector<std::string> getAvailableConfigs();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private helper methods
    bool runBasicConnectivityDemo(const DemoOptions& options);
    bool runPlasticityLearningDemo(const DemoOptions& options);
    bool runStdpVerificationDemo(const DemoOptions& options);
    bool runPhase6IntegrationDemo(const DemoOptions& options);
    bool runTextDemo(const DemoOptions& options);
    bool runBenchmarkDemo(const DemoOptions& options);
    bool runCustomConfigDemo(const DemoOptions& options);
    
    // Common demo setup
    bool setupBrain(const DemoOptions& options, std::shared_ptr<Brain>& brain);
    bool setupLogging(const DemoOptions& options);
    
    // Progress reporting
    void reportProgress(const std::string& message, float progress = -1.0f);
    
    // Error handling
    void handleError(const std::string& error, bool fatal = true);
};

// Global demo runner instance
DemoRunner& getDemoRunner();

// Convenience functions for common operations
bool runDemoByName(const std::string& demoName);
bool runDemoWithConfig(const std::string& configFile, const std::string& demoName);
bool runBenchmark(const std::map<std::string, std::pair<size_t, size_t>>& benchmarks);

} // namespace nlm