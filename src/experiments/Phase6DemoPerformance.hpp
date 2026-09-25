#pragma once

/**
 * Performance Profiling Integration for Phase6Demo
 * 
 * This header provides the integration between the performance profiler
 * and the Phase6Demo visualization system.
 * 
 * Features:
 * - Command-line option parsing
 * - Automatic profiling setup and teardown
 * - Integration with visualization timing
 * - Export formatting options
 * - System event tracking
 */

#include "experiments/PerformanceProfiler.hpp"
#include "experiments/Phase6IntegratedExperiment.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"
#include <string>
#include <vector>
#include <map>

namespace nlm {

/**
 * Enhanced Phase6Demo configuration with profiling options
 */
struct Phase6DemoConfig {
    // Original visualization configuration
    std::string windowTitle;
    int width;
    int height;
    bool enableTextVisualization;
    double updateRate;
    bool autoSave;
    std::string exportPath;
    std::string exportFormat;
    bool enableWebSocket;
    int webSocketPort;
    bool showHelp;
    
    // Performance profiling configuration
    bool enableProfiling;
    std::string profileOutputDir;
    std::string profileFormat;
    bool profileConsoleOutput;
    double profileIntervalMs;
    bool profileMemory;
    bool profileCPU;
    bool profileSystem;
    
    // Brain configuration
    uint64_t maxSteps;
    size_t neuronCount;
    
    Phase6DemoConfig() : windowTitle("NLM Phase 6 Demo"), width(80), height(40),
                         enableTextVisualization(true), updateRate(30.0), autoSave(false),
                         exportPath("./output"), exportFormat("json"), enableWebSocket(false),
                         webSocketPort(8080), showHelp(false),
                         enableProfiling(false), profileOutputDir("./profiles"),
                         profileFormat("text"), profileConsoleOutput(true),
                         profileIntervalMs(1.0), profileMemory(true), profileCPU(true),
                         profileSystem(true), maxSteps(2000), neuronCount(500) {}
};

/**
 * Performance metrics collector for Phase6Demo
 */
class Phase6DemoMetrics {
public:
    Phase6DemoMetrics() : totalSteps(0), totalVisualizationTime(0.0), 
                         totalExportTime(0.0), peakMemoryUsage(0), 
                         spikeCount(0), plasticityCount(0), 
                         developmentCount(0), predictionCount(0), 
                         memoryCount(0) {}
    
    // Record metrics
    void recordStep(uint64_t step, double visualizationTimeUs, 
                   double exportTimeUs, size_t memoryUsage);
    void recordSpikeEvent();
    void recordPlasticityEvent();
    void recordDevelopmentEvent();
    void recordPredictionEvent();
    void recordMemoryEvent();
    
    // Get statistics
    double getAverageFPS() const;
    double getAverageStepTime() const;
    double getPeakMemoryUsage() const;
    double getTotalRuntime() const;
    
    // Get metrics
    uint64_t getTotalSteps() const { return totalSteps; }
    double getTotalVisualizationTime() const { return totalVisualizationTime; }
    double getTotalExportTime() const { return totalExportTime; }
    double getPeakMemoryUsage() const { return peakMemoryUsage; }
    
    // System metrics
    uint64_t spikeCount;
    uint64_t plasticityCount;
    uint64_t developmentCount;
    uint64_t predictionCount;
    uint64_t memoryCount;
    
private:
    uint64_t totalSteps;
    double totalVisualizationTime;
    double totalExportTime;
    size_t peakMemoryUsage;
    double startTime;
    std::vector<double> stepTimes;
};

/**
 * Performance analysis for Phase6Demo
 */
class Phase6DemoPerformanceAnalyzer {
public:
    Phase6DemoPerformanceAnalyzer() = default;
    
    // Analyze metrics
    std::string analyzePerformance(const Phase6DemoMetrics& metrics) const;
    std::string generatePerformanceReport(const Phase6DemoMetrics& metrics,
                                         const std::string& demoName) const;
    std::string generateBottleneckAnalysis(const Phase6DemoDemoMetrics& metrics) const;
    
    // System-specific analysis
    std::string analyzeMemoryPerformance(const Phase6DemoMetrics& metrics) const;
    std::string analyzeCPUPerformance(const Phase6DemoMetrics& metrics) const;
    std::string analyzeVisualizationPerformance(const Phase6DemoMetrics& metrics) const;
    
    // Trend analysis
    std::string analyzePerformanceTrends(const std::vector<Phase6DemoMetrics>& history) const;
    
private:
    std::string formatMetric(const std::string& name, double value, const std::string& unit) const;
    std::string getPerformanceGrade(double performanceScore) const;
};

/**
 * Integration utilities for Phase6Demo performance profiling
 */
namespace Phase6DemoProfiling {
    
    // Configuration parsing
    bool parseCommandLine(int argc, char* argv[], Phase6DemoConfig& config);
    void applyProfileConfig(Phase6DemoConfig& config, PerformanceProfiler& profiler);
    
    // Integration with experiment
    void setupProfilingForExperiment(PerformanceProfiler& profiler, 
                                     Phase6DemoConfig& config);
    void integrateProfilingWithVisualization(PerformanceProfiler& profiler,
                                            Phase6DemoConfig& config);
    
    // Metrics collection
    Phase6DemoMetrics collectDemoMetrics(const Brain& brain, uint64_t step,
                                        double visualizationTimeUs, double exportTimeUs);
    
    // Output and reporting
    void exportPhase6DemoMetrics(const Phase6DemoMetrics& metrics, 
                                const std::string& baseFilename);
    void generatePerformanceAnalysis(const Phase6DemoMetrics& metrics,
                                    const std::string& analysisFilename);
    
    // Command-line help
    std::string getPhase6DemoProfilingHelp();
}

} // namespace nlm
