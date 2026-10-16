#pragma once

#include "CommandLineOptions.hpp"
#include "core/ErrorHandling/ErrorHandling.hpp"
#include "core/TestFramework/TestFramework.hpp"
#include "core/AdvancedCommands/AdvancedCommands.hpp"
#include "core/PerformanceOptimization/PerformanceOptimization.hpp"
#include <string>
#include <memory>
#include <map>
#include <chrono>

namespace nlm {

class NLMApplication {
public:
    NLMApplication(const CommandLineOptions::Options& options);
    ~NLMApplication() = default;
    
    bool initialize();
    void run();
    void cleanup();
    
    // Mode handlers
    void runSimulationMode();
    void runConnectivityTest();
    void runPlasticityTest();
    void runSTDPTest();
    void runBenchmark();
    void exportConfiguration();
    void runTestMode();
    void runAdvancedCommandMode();
    void runPerformanceAnalysisMode();
    void runRealTimeMonitoringMode();
    
    // Advanced command execution
    CommandResult executeAdvancedCommand(const std::string& commandLine);
    std::string getCommandHelp() const;
    
    // Command history management
    void addToCommandHistory(const std::string& commandLine);
    std::vector<std::string> getCommandHistory() const;
    void clearCommandHistory();
    CommandResult replayCommand(const std::string& commandLine);
    
    // Performance monitoring
    void startPerformanceMonitoring();
    void stopPerformanceMonitoring();
    std::string getPerformanceReport() const;
    std::string getDetailedPerformanceReport() const;
    
private:
    CommandLineOptions::Options options_;
    std::shared_ptr<Config> config_;
    std::shared_ptr<Logger> logger_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<class SimulationClock> clock_;
    std::unique_ptr<AdvancedCommandSystem> commandSystem_;
    std::unique_ptr<PerformanceProfiler> profiler_;
    std::unique_ptr<SimulationPerformanceAnalyzer> performanceAnalyzer_;
    std::unique_ptr<RealTimePerformanceMonitor> realtimeMonitor_;
    
    void setupLogging();
    void setupConfiguration();
    void setupBrain();
    void setupCommandSystem();
    void setupPerformanceTools();
    void runStandardTests();
    void printPerformanceStats();
    
    // Statistics
    struct PerfStats {
        size_t totalSteps = 0;
        size_t totalSpikes = 0;
        double startTime = 0.0;
        double endTime = 0.0;
    } stats_;
};

// Learning Experiment structure (extracted from main.cpp)
struct LearningExperiment {
    std::shared_ptr<Brain> brain;
    uint64_t seed;
    size_t initialSynapseCount;
    std::vector<float> initialWeights;
    std::vector<float> finalWeights;
    std::vector<NeuronId> mostActiveNeurons;
    
    LearningExperiment(std::shared_ptr<Brain> b, uint64_t s) 
        : brain(b), seed(s), initialSynapseCount(0) {}
    
    void recordInitialState();
    void recordFinalState();
    void computeStatistics();
};

// Test functions (extracted from main.cpp)
void printBanner();
void runBasicConnectivityTest(std::shared_ptr<Brain> brain);
void runPlasticityExperiment(std::shared_ptr<Brain> brain);
void runStdpVerification(std::shared_ptr<Brain> brain);

} // namespace nlm