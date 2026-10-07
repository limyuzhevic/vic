// ComprehensiveExample.hpp - Demonstration of NLM Performance Profiling System
// Shows how all profiling components work together

#pragma once

/**
 * ComprehensiveExample - Complete demonstration of NLM performance profiling
 * 
 * This example demonstrates the complete NLM performance profiling system,
 * showing how all components work together to provide comprehensive
 * performance monitoring and analysis for the Neural Learning Machine.
 * 
 * The example covers:
 * 1. Core profiling infrastructure setup
 * 2. Performance monitoring configuration
 * 3. Data collection and processing
 * 4. Analysis and reporting
 * 5. Export and integration
 * 6. Advanced features (dynamic profiling, memory leak detection, etc.)
 * 7. Integration with existing debug system
 * 8. Real-world usage scenarios
 * 
 * Features:
 * - Complete end-to-end demonstration
 * - Multiple integration points
 * - Practical configuration examples
 * - Error handling and recovery
 * - Performance optimization recommendations
 * - Real-time monitoring capabilities
 * - Post-mortem analysis
 * 
 * This example is designed to be:
 * - Educational: Shows how to use each component
 * - Practical: Shows real-world usage
 * - Comprehensive: Covers all major functionality
 * - Self-contained: Can be compiled and run independently
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace nlm {

/**
 * Example configuration for a typical NLM simulation
 */
struct ExampleSimulationConfig {
    std::string name;
    size_t neuronCount;
    size_t timestepCount;
    std::string hardwareTarget;
    std::string optimizationLevel;
    bool enableProfiling;
    std::string outputDirectory;
    
    ExampleSimulationConfig() : neuronCount(10000), timestepCount(1000), 
                             enableProfiling(true), outputDirectory("./output") {}
};

/**
 * Performance profiling demonstration
 */
class PerformanceProfilingExample {
private:
    std::string exampleName_;
    ExampleSimulationConfig simulationConfig_;
    
    // Profiling components
    std::shared_ptr<NLPerformanceProfiler> profiler_;
    std::shared_ptr<PerformanceMonitor> monitor_;
    std::shared_ptr<ProfilingDataCollector> dataCollector_;
    std::shared_ptr<NLPerformanceAnalyzer> analyzer_;
    std::shared_ptr<ExportIntegrationManager> exportIntegration_;
    std::shared_ptr<AdvancedProfilerManager> advancedProfiler_;
    
    // Integration
    std::shared_ptr<NLMIntegrationManager> integrationManager_;
    
    // Example state
    bool profilingEnabled_;
    uint64_t currentTimestep_;
    double totalProcessingTime_;
    size_t totalSpikesGenerated_;
    
public:
    PerformanceProfilingExample(const std::string& name = "NLM Performance Profiling Example");
    ~PerformanceProfilingExample();
    
    // Initialize the profiling system
    void initialize(const ExampleSimulationConfig& config);
    
    // Run the simulation with profiling
    void runSimulationWithProfiling();
    
    // Simulate a single timestep with profiling
    void simulateTimestep(size_t timestep, size_t neuronCount, 
                         double timestepTimeMs, size_t spikesGenerated,
                         size_t activeNeurons);
    
    // Demonstrate core profiling functionality
    void demonstrateCoreProfiling();
    
    // Demonstrate performance monitoring
    void demonstratePerformanceMonitoring();
    
    // Demonstrate data collection
    void demonstrateDataCollection();
    
    // Demonstrate analysis and reporting
    void demonstrateAnalysisAndReporting();
    
    // Demonstrate export and integration
    void demonstrateExportAndIntegration();
    
    // Demonstrate advanced features
    void demonstrateAdvancedFeatures();
    
    // Demonstrate integration with debug system
    void demonstrateDebugSystemIntegration();
    
    // Generate example configuration
    static ExampleSimulationConfig createDefaultConfig();
    static ExampleSimulationConfig createHighPerformanceConfig();
    static ExampleSimulationConfig createDevelopmentConfig();
    
    // Generate configuration files
    static void generateConfigurationFiles(const ExampleSimulationConfig& config, const std::string& outputDir);
    
    // Generate example reports
    static void generateExampleReports(const std::string& outputDir);
    
    // Print example usage
    static void printExampleUsage();
    
    // Get example name
    std::string getExampleName() const { return exampleName_; }
    
private:
    // Setup profiling components
    void setupCoreProfiling();
    void setupPerformanceMonitoring();
    void setupDataCollection();
    void setupAnalysis();
    void setupExport();
    void setupAdvancedFeatures();
    
    // Setup integration
    void setupIntegration();
    
    // Demonstrate specific components
    void demonstrateProfiler(const ExampleSimulationConfig& config);
    void demonstrateMonitor();
    void demonstrateDataCollector();
    void demonstrateAnalyzer();
    void demonstrateExportIntegration();
    void demonstrateAdvancedProfiler();
    
    // Generate example data
    std::vector<ProfileDataPoint> generateExampleEventData() const;
    std::vector<TimestepPerformance> generateExampleTimestepData() const;
    
    // Process and analyze example data
    void processExampleData();
    void analyzeExampleData();
    
    // Generate example reports
    void generateExampleProfilerReport() const;
    void generateExampleMonitorReport() const;
    void generateExampleAnalysisReport() const;
    
    // Export example data
    void exportExampleData() const;
    
    // Setup file output
    void setupFileOutput(const std::string& outputDir);
    
    // Print progress
    void printProgress(const std::string& message) const;
};

/**
 * Real-time monitoring example
 */
class RealTimeMonitoringExample {
private:
    std::string exampleName_;
    bool monitoringEnabled_;
    
    // Monitoring components
    std::shared_ptr<PerformanceMonitor> monitor_;
    std::shared_ptr<ProfilingDataCollector> dataCollector_;
    
    // Monitoring state
    std::vector<PerformanceMetrics> metricsHistory_;
    std::vector<std::string> alertMessages_;
    
public:
    RealTimeMonitoringExample(const std::string& name = "Real-Time Monitoring Example");
    
    // Start real-time monitoring
    void startMonitoring();
    
    // Stop monitoring
    void stopMonitoring();
    
    // Update monitoring data
    void updateMetrics(const PerformanceMetrics& metrics);
    
    // Add alert
    void addAlert(const std::string& message);
    
    // Generate monitoring report
    std::string generateMonitoringReport() const;
    
    // Print monitoring status
    void printMonitoringStatus() const;
    
private:
    void setupMonitoringComponents();
    void setupAlerts();
};

/**
 * Performance benchmarking example
 */
class PerformanceBenchmarkingExample {
private:
    std::string exampleName_;
    bool benchmarkingEnabled_;
    
    // Benchmarking components
    std::shared_ptr<NLPerformanceAnalyzer> analyzer_;
    std::shared_ptr<PerformanceBenchmark> benchmark_;
    
    // Benchmark configuration
    struct BenchmarkConfig {
        std::string name;
        std::vector<size_t> neuronCounts;
        std::vector<size_t> timestepCounts;
        std::vector<std::string> optimizationLevels;
        double targetPerformance;
        size_t maxMemoryUsageMB;
    };
    
public:
    PerformanceBenchmarkingExample(const std::string& name = "Performance Benchmarking Example");
    
    // Run performance benchmarks
    void runBenchmarkSuite();
    
    // Generate benchmark report
    std::string generateBenchmarkReport() const;
    
    // Print benchmark results
    void printBenchmarkResults() const;
    
private:
    void setupBenchmarkingComponents();
    void runSingleBenchmark(size_t neuronCount, size_t timestepCount, 
                           const std::string& optimizationLevel);
    BenchmarkConfig createDefaultBenchmarkConfig();
};

/**
 * Complete system demonstration
 */
class CompleteSystemDemonstration {
private:
    std::string exampleName_;
    bool demonstrationComplete_;
    
public:
    CompleteSystemDemonstration(const std::string& name = "Complete System Demonstration");
    
    // Run complete demonstration
    void runCompleteDemonstration();
    
    // Generate demonstration report
    std::string generateDemonstrationReport() const;
    
    // Print demonstration summary
    void printDemonstrationSummary() const;
    
private:
    void demonstrateCoreProfilingIntegration();
    void demonstrateAdvancedProfilingFeatures();
    void demonstrateRealTimeMonitoring();
    void demonstratePerformanceBenchmarking();
    void generateComprehensiveReport();
};

/**
 * Example utilities
 */
class ExampleUtilities {
public:
    // Generate example configuration files
    static void generateExampleConfigs(const std::string& outputDir);
    
    // Generate example data files
    static void generateExampleDataFiles(const std::string& outputDir);
    
    // Generate example report files
    static void generateExampleReportFiles(const std::string& outputDir);
    
    // Create example directory structure
    static void createExampleDirectoryStructure(const std::string& baseDir);
    
    // Generate example configuration
    static std::string generateExampleConfiguration(const ExampleSimulationConfig& config);
    
    // Generate example data summary
    static std::string generateExampleDataSummary(const std::vector<ProfileDataPoint>& data);
    
    // Format example data for output
    static std::string formatExampleData(const ProfileDataPoint& data) const;
    
    // Print example information
    static void printExampleHeader(const std::string& title);
    static void printExampleSection(const std::string& title);
    static void printExampleFooter();
    
    // Print example configuration
    static void printExampleConfiguration(const ExampleSimulationConfig& config);
    
    // Print example results
    static void printExampleResults(const std::string& results);
    
private:
    static const std::string getExampleVersion();
};

/**
 * Example main function
 */
void runComprehensiveExample();
void runRealTimeMonitoringExample();
void runPerformanceBenchmarkingExample();
void runCompleteSystemDemonstration();

} // namespace nlm