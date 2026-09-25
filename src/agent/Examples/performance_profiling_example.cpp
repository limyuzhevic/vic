#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <chrono>
#include <thread>
#include <algorithm>
#include <iomanip>

// NLM headers
#include "src/agent/AdvancedPerformance.hpp"
#include "src/agent/AdvancedConfig.hpp"

using namespace nlm;

// Performance profiling example
class PerformanceProfilingExample {
public:
    PerformanceProfilingExample() {
        std::cout << "=== NLM Performance Profiling Example ===" << std::endl;
    }
    
    void runExample() {
        std::cout << "Running performance profiling example..." << std::endl;
        
        // Initialize performance system
        AdvancedPerformance& perfSystem = AdvancedPerformance::getInstance();
        
        // Step 1: Set up performance configuration
        setupPerformanceConfiguration(perfSystem);
        
        // Step 2: Start performance monitoring
        startPerformanceMonitoring(perfSystem);
        
        // Step 3: Run simulation and collect performance data
        runSimulationWithProfiling(perfSystem);
        
        // Step 4: Analyze performance
        analyzePerformance(perfSystem);
        
        // Step 5: Detect performance regressions
        detectPerformanceRegressions(perfSystem);
        
        // Step 6: Detect performance bottlenecks
        detectPerformanceBottlenecks(perfSystem);
        
        // Step 7: Generate optimization recommendations
        generateOptimizationRecommendations(perfSystem);
        
        // Step 8: Run automated optimization
        runAutomatedOptimization(perfSystem);
        
        // Step 9: Generate performance report
        generatePerformanceReport(perfSystem);
        
        // Stop performance monitoring
        stopPerformanceMonitoring(perfSystem);
        
        std::cout << "Performance profiling example completed successfully!" << std::endl;
    }
    
private:
    void setupPerformanceConfiguration(AdvancedPerformance& perfSystem) {
        std::cout << "\n1. Setting up performance configuration..." << std::endl;
        
        // Create performance configuration
        nlm::Config perfConfig;
        perfConfig.setValue("performance.profiling_enabled", true);
        perfConfig.setValue("performance.bottleneck_detection", true);
        perfConfig.setValue("performance.optimization_enabled", true);
        perfConfig.setValue("performance.regression_testing", true);
        perfConfig.setValue("performance.real_time_monitoring", true);
        perfConfig.setValue("performance.metric_collection", true);
        perfConfig.setValue("performance.analysis_depth", 5);
        perfConfig.setValue("performance.threshold_checking", true);
        
        perfSystem.setPerformanceConfig(perfConfig);
        std::cout << "  Performance configuration set successfully!" << std::endl;
        
        // Set performance thresholds
        std::map<std::string, double> thresholds;
        thresholds["simulation_time"] = 100.0;  // seconds
        thresholds["memory_usage"] = 1024.0;   // MB
        thresholds["cpu_usage"] = 80.0;        // percent
        thresholds["throughput"] = 1000.0;     // operations per second
        thresholds["latency"] = 10.0;         // milliseconds
        
        // Note: In real implementation, thresholds would be set through perfSystem
        // For demonstration, we'll show the concept
        std::cout << "  Performance thresholds configured!" << std::endl;
    }
    
    void startPerformanceMonitoring(AdvancedPerformance& perfSystem) {
        std::cout << "\n2. Starting performance monitoring..." << std::endl;
        
        // Start real-time performance monitoring
        perfSystem.startRealTimeMonitoring();
        std::cout << "  Real-time performance monitoring started!" << std::endl;
        
        // Create performance profiling session
        std::string sessionId = perfSystem.startProfileSession(
            "Neural network training simulation",
            "steady_state"
        );
        
        std::cout << "  Performance profiling session started: " << sessionId << std::endl;
        
        currentSessionId_ = sessionId;
    }
    
    void runSimulationWithProfiling(AdvancedPerformance& perfSystem) {
        std::cout << "\n3. Running simulation with performance profiling..." << std::endl;
        
        // Simulate neural network operations
        std::cout << "  Simulating neural network operations..." << std::endl;
        
        for (int step = 0; step < 10; ++step) {
            double timestep = 0.1;  // seconds
            
            // Simulate neural network computation
            simulateNeuralNetworkStep(step, timestep);
            
            // Simulate memory allocation
            simulateMemoryAllocation(step);
            
            // Simulate I/O operations
            simulateIOOperations(step);
            
            // Simulate thread operations
            simulateThreadOperations(step);
            
            if (step % 2 == 0) {
                std::cout << "  Completed step " << step << " (" << (step * timestep) << "s)" << std::endl;
            }
        }
        
        std::cout << "  Simulation completed successfully!" << std::endl;
    }
    
    void simulateNeuralNetworkStep(int step, double timestep) {
        // Simulate neural network computation
        // This is where performance data would be collected
        
        // Simulate computation time based on step
        double computationTime = 0.1 + (step * 0.01);  // vary with step
        
        // Simulate memory usage
        double memoryUsage = 512.0 + (step * 10.0);  // increase with step
        
        // Simulate CPU usage
        double cpuUsage = 30.0 + (step * 2.0);  // vary with step
        
        // Simulate throughput
        double throughput = 800.0 + (step * 20.0);  // increase with step
        
        // Simulate latency
        double latency = 5.0 + (step * 0.5);  // increase with step
        
        // Collect performance data
        collectPerformanceData(step, timestep, computationTime, memoryUsage, cpuUsage, throughput, latency);
    }
    
    void simulateMemoryAllocation(int step) {
        // Simulate memory allocation
        // This is where memory allocation performance would be collected
        
        // Simulate allocation time
        double allocationTime = 0.001 + (step * 0.0001);
        
        // Simulate memory fragmentation
        double fragmentation = 0.1 + (step * 0.001);
        
        // Collect memory performance data
        collectMemoryPerformanceData(step, allocationTime, fragmentation);
    }
    
    void simulateIOOperations(int step) {
        // Simulate I/O operations
        // This is where I/O performance would be collected
        
        // Simulate I/O time
        double ioTime = 0.01 + (step * 0.001);
        
        // Simulate cache efficiency
        double cacheEfficiency = 0.8 + (step * 0.002);
        
        // Collect I/O performance data
        collectIOPerformanceData(step, ioTime, cacheEfficiency);
    }
    
    void simulateThreadOperations(int step) {
        // Simulate thread operations
        // This is where thread performance would be collected
        
        // Simulate thread contention
        double threadContention = 0.2 + (step * 0.001);
        
        // Simulate thread utilization
        double threadUtilization = 0.7 + (step * 0.001);
        
        // Collect thread performance data
        collectThreadPerformanceData(step, threadContention, threadUtilization);
    }
    
    void collectPerformanceData(int step, double timestep, double computationTime, 
                               double memoryUsage, double cpuUsage, double throughput, double latency) {
        // Collect performance data
        // This is where performance metrics would be collected
        
        // In real implementation, this would call perfSystem to record metrics
        // For demonstration, we'll just log the data
        
        if (debugMode_) {
            std::cout << "    Performance data - Step: " << step 
                      << ", Time: " << timestep << "s, "
                      << "Compute: " << computationTime << "s, "
                      << "Memory: " << memoryUsage << "MB, "
                      << "CPU: " << cpuUsage << "%, "
                      << "Throughput: " << throughput << " ops/s, "
                      << "Latency: " << latency << "ms" << std::endl;
        }
    }
    
    void collectMemoryPerformanceData(int step, double allocationTime, double fragmentation) {
        // Collect memory performance data
        if (debugMode_) {
            std::cout << "    Memory data - Step: " << step 
                      << ", Allocation: " << allocationTime << "s, "
                      << "Fragmentation: " << (fragmentation * 100) << "%" << std::endl;
        }
    }
    
    void collectIOPerformanceData(int step, double ioTime, double cacheEfficiency) {
        // Collect I/O performance data
        if (debugMode_) {
            std::cout << "    I/O data - Step: " << step 
                      << ", I/O Time: " << ioTime << "s, "
                      << "Cache Efficiency: " << (cacheEfficiency * 100) << "%" << std::endl;
        }
    }
    
    void collectThreadPerformanceData(int step, double threadContention, double threadUtilization) {
        // Collect thread performance data
        if (debugMode_) {
            std::cout << "    Thread data - Step: " << step 
                      << ", Contention: " << (threadContention * 100) << "%, "
                      << "Utilization: " << (threadUtilization * 100) << "%" << std::endl;
        }
    }
    
    void analyzePerformance(AdvancedPerformance& perfSystem) {
        std::cout << "\n4. Analyzing performance..." << std::endl;
        
        if (currentSessionId_.empty()) {
            std::cout << "  No performance session to analyze!" << std::endl;
            return;
        }
        
        // Get active session
        PerformanceProfileSession* session = perfSystem.getActiveSession(currentSessionId_);
        if (!session) {
            std::cout << "  Failed to get performance session!" << std::endl;
            return;
        }
        
        // Analyze performance type
        PerformanceAnalysisType analysisType = perfSystem.analyzePerformanceType(*session);
        std::cout << "  Performance analysis type: " << performanceAnalysisTypeToString(analysisType) << std::endl;
        
        // Analyze performance trends
        std::cout << "  Analyzing performance trends..." << std::endl;
        
        // Calculate performance metrics
        calculatePerformanceMetrics(*session);
        
        // Generate performance summary
        generatePerformanceSummary(*session);
        
        std::cout << "  Performance analysis completed!" << std::endl;
    }
    
    void calculatePerformanceMetrics(const PerformanceProfileSession& session) {
        // Calculate performance metrics
        // This is where performance metrics would be calculated
        
        // Calculate average throughput
        double totalTime = session.getDurationSeconds();
        double avgThroughput = getSampleCount(session) / totalTime;
        
        // Calculate average memory usage
        double avgMemoryUsage = getAverageMemoryUsage(session);
        
        // Calculate average CPU usage
        double avgCPUUsage = getAverageCPUUsage(session);
        
        if (debugMode_) {
            std::cout << "    Performance metrics:" << std::endl;
            std::cout << "      Total time: " << totalTime << "s" << std::endl;
            std::cout << "      Average throughput: " << avgThroughput << " ops/s" << std::endl;
            std::cout << "      Average memory usage: " << avgMemoryUsage << " MB" << std::endl;
            std::cout << "      Average CPU usage: " << avgCPUUsage << "%" << std::endl;
        }
    }
    
    double getSampleCount(const PerformanceProfileSession& session) const {
        // Get number of samples
        // In real implementation, this would count samples in session
        return session.samples.size();
    }
    
    double getAverageMemoryUsage(const PerformanceProfileSession& session) const {
        // Calculate average memory usage
        // In real implementation, this would analyze memory metrics from samples
        return 512.0;  // Placeholder
    }
    
    double getAverageCPUUsage(const PerformanceProfileSession& session) const {
        // Calculate average CPU usage
        // In real implementation, this would analyze CPU metrics from samples
        return 50.0;  // Placeholder
    }
    
    void generatePerformanceSummary(const PerformanceProfileSession& session) {
        // Generate performance summary
        // This is where performance summary would be generated
        
        double totalTime = session.getDurationSeconds();
        
        std::cout << "  Performance summary:" << std::endl;
        std::cout << "    Total execution time: " << totalTime << "s" << std::endl;
        std::cout << "    Number of samples: " << getSampleCount(session) << std::endl;
        
        // Calculate performance score
        double performanceScore = calculatePerformanceScore(session);
        std::cout << "    Performance score: " << performanceScore << "/100" << std::endl;
    }
    
    double calculatePerformanceScore(const PerformanceProfileSession& session) const {
        // Calculate performance score based on metrics
        // This is where performance score would be calculated
        
        // Placeholder calculation
        double throughputScore = getSampleCount(session) > 0 ? 80.0 : 0.0;
        double efficiencyScore = 90.0;
        double scalabilityScore = 85.0;
        
        return (throughputScore + efficiencyScore + scalabilityScore) / 3.0;
    }
    
    void detectPerformanceRegressions(AdvancedPerformance& perfSystem) {
        std::cout << "\n5. Detecting performance regressions..." << std::endl;
        
        // Create baseline session
        std::string baselineSessionId = createBaselineSession(perfSystem);
        
        // Create current session
        std::string currentSessionId = createCurrentSession(perfSystem);
        
        if (!baselineSessionId.empty() && !currentSessionId.empty()) {
            // Detect performance regressions
            std::vector<PerformanceRegression> regressions = 
                perfSystem.detectPerformanceRegressions(baselineSessionId, currentSessionId);
            
            std::cout << "  Performance regression detection results:" << std::endl;
            std::cout << "    Total regressions detected: " << regressions.size() << std::endl;
            
            for (size_t i = 0; i < regressions.size(); ++i) {
                const auto& regression = regressions[i];
                std::cout << "    " << (i + 1) << ". " << regression.metricName << 
                          " (" << regression.regressionPercentage << "%)" << std::endl;
                std::cout << "       Baseline: " << regression.baselineValue << 
                          ", Current: " << regression.currentValue << std::endl;
                std::cout << "       Severity: " << regression.severity << ", "
                          << "Trend: " << performanceTrendToString(regression.trend) << std::endl;
            }
        } else {
            std::cout << "  Failed to detect performance regressions (missing sessions)!" << std::endl;
        }
    }
    
    std::string createBaselineSession(AdvancedPerformance& perfSystem) {
        // Create baseline session
        // This is where baseline session would be created
        
        // For demonstration, we'll create a simple baseline session
        PerformanceProfileSession baselineSession;
        baselineSession.sessionId = "baseline_1.0";
        baselineSession.description = "Baseline performance (version 1.0)";
        baselineSession.scenario = "steady_state";
        baselineSession.isActive = false;
        
        // Set baseline metrics
        baselineSession.baselineMetrics["simulation_time"] = 100.0;
        baselineSession.baselineMetrics["memory_usage"] = 512.0;
        baselineSession.baselineMetrics["cpu_usage"] = 50.0;
        baselineSession.baselineMetrics["throughput"] = 800.0;
        baselineSession.baselineMetrics["latency"] = 10.0;
        
        // Note: In real implementation, this would add session to perfSystem
        // For demonstration, we'll just return a session ID
        
        return "baseline_1.0";
    }
    
    std::string createCurrentSession(AdvancedPerformance& perfSystem) {
        // Create current session
        // This is where current session would be created
        
        // For demonstration, we'll create a simple current session
        PerformanceProfileSession currentSession;
        currentSession.sessionId = "current_1.1";
        currentSession.description = "Current performance (version 1.1)";
        currentSession.scenario = "steady_state";
        currentSession.isActive = false;
        
        // Set current metrics
        currentSession.currentMetrics["simulation_time"] = 80.0;
        currentSession.currentMetrics["memory_usage"] = 512.0;
        currentSession.currentMetrics["cpu_usage"] = 45.0;
        currentSession.currentMetrics["throughput"] = 1000.0;
        currentSession.currentMetrics["latency"] = 8.0;
        
        // Note: In real implementation, this would add session to perfSystem
        // For demonstration, we'll just return a session ID
        
        return "current_1.1";
    }
    
    void detectPerformanceBottlenecks(AdvancedPerformance& perfSystem) {
        std::cout << "\n6. Detecting performance bottlenecks..." << std::endl;
        
        // Get active session
        PerformanceProfileSession* session = perfSystem.getActiveSession(currentSessionId_);
        if (!session) {
            std::cout << "  Failed to get performance session!" << std::endl;
            return;
        }
        
        // Detect performance bottlenecks
        std::vector<PerformanceBottleneck> bottlenecks = perfSystem.detectPerformanceBottlenecks(*session);
        
        std::cout << "  Performance bottleneck detection results:" << std::endl;
        std::cout << "    Total bottlenecks detected: " << bottlenecks.size() << std::endl;
        
        for (size_t i = 0; i < bottlenecks.size(); ++i) {
            const auto& bottleneck = bottlenecks[i];
            std::cout << "    " << (i + 1) << ". " << bottleneck.componentName << 
                      " - " << performanceMetricTypeToString(bottleneck.metricType) << std::endl;
            std::cout << "       Current: " << bottleneck.currentValue << 
                      ", Threshold: " << bottleneck.thresholdValue << std::endl;
            std::cout << "       Type: " << bottleneck.bottleneckType << ", "
                      << "Recommendation: " << bottleneck.recommendation << std::endl;
        }
        
        std::cout << "  Bottleneck detection completed!" << std::endl;
    }
    
    void generateOptimizationRecommendations(AdvancedPerformance& perfSystem) {
        std::cout << "\n7. Generating optimization recommendations..." << std::endl;
        
        // Get active session
        PerformanceProfileSession* session = perfSystem.getActiveSession(currentSessionId_);
        if (!session) {
            std::cout << "  Failed to get performance session!" << std::endl;
            return;
        }
        
        // Generate optimization recommendations
        std::vector<PerformanceOptimization> optimizations = 
            perfSystem.generateOptimizationRecommendations(*session);
        
        std::cout << "  Optimization recommendations:" << std::endl;
        std::cout << "    Total optimizations: " << optimizations.size() << std::endl;
        
        for (size_t i = 0; i < optimizations.size(); ++i) {
            const auto& optimization = optimizations[i];
            std::cout << "    " << (i + 1) << ". " << optimization.description << std::endl;
            std::cout << "       Component: " << optimization.component << ", "
                      << "Gain: " << optimization.estimatedGainPercentage << "%" << std::endl;
            std::cout << "       Priority: " << optimization.getPriorityLabel() << ", "
                      << "Risk: " << optimization.riskLevel << std::endl;
        }
        
        std::cout << "  Optimization recommendations generated!" << std::endl;
    }
    
    void runAutomatedOptimization(AdvancedPerformance& perfSystem) {
        std::cout << "\n8. Running automated optimization..." << std::endl;
        
        // Run automated optimization
        bool optimizationSuccessful = perfSystem.runAutomatedOptimization(currentSessionId_);
        
        if (optimizationSuccessful) {
            std::cout << "  Automated optimization successful!" << std::endl;
            
            // Run optimization verification
            std::cout << "  Running optimization verification..." << std::endl;
            
            // Note: In real implementation, this would verify optimization
            // For demonstration, we'll just log the attempt
            std::cout << "  Optimization verification would be performed here..." << std::endl;
        } else {
            std::cout << "  Automated optimization failed!" << std::endl;
        }
        
        std::cout << "  Automated optimization completed!" << std::endl;
    }
    
    void generatePerformanceReport(AdvancedPerformance& perfSystem) {
        std::cout << "\n9. Generating performance report..." << std::endl;
        
        // Generate performance report
        std::string report = perfSystem.generatePerformanceReport(currentSessionId_);
        
        if (!report.empty()) {
            std::cout << "  Performance report generated:" << std::endl;
            std::cout << "    Report length: " << report.size() << " characters" << std::endl;
            
            // Save report to file
            std::string filename = "performance_report.txt";
            std::ofstream reportFile(filename);
            if (reportFile.is_open()) {
                reportFile << report;
                reportFile.close();
                std::cout << "    Report saved to " << filename << "!" << std::endl;
            } else {
                std::cout << "    Failed to save report to " << filename << "!" << std::endl;
            }
        } else {
            std::cout << "  Failed to generate performance report!" << std::endl;
        }
        
        std::cout << "  Performance report generation completed!" << std::endl;
    }
    
    void stopPerformanceMonitoring(AdvancedPerformance& perfSystem) {
        std::cout << "\n10. Stopping performance monitoring..." << std::endl;
        
        // End performance session
        if (!currentSessionId_.empty()) {
            perfSystem.endProfileSession(currentSessionId_);
            std::cout << "  Performance session ended: " << currentSessionId_ << std::endl;
            currentSessionId_.clear();
        }
        
        // Stop real-time monitoring
        perfSystem.stopRealTimeMonitoring();
        std::cout << "  Real-time performance monitoring stopped!" << std::endl;
        
        std::cout << "  Performance monitoring stopped successfully!" << std::endl;
    }
    
    std::string performanceAnalysisTypeToString(PerformanceAnalysisType type) const {
        switch (type) {
            case PerformanceAnalysisType::Bottleneck: return "Bottleneck";
            case PerformanceAnalysisType::Regression: return "Regression";
            case PerformanceAnalysisType::Optimization: return "Optimization";
            case PerformanceAnalysisType::Comparative: return "Comparative";
            case PerformanceAnalysisType::Predictive: return "Predictive";
            case PerformanceAnalysisType::SystemWide: return "SystemWide";
            default: return "Unknown";
        }
    }
    
    std::string performanceTrendToString(PerformanceTrend trend) const {
        switch (trend) {
            case PerformanceTrend::Improving: return "Improving";
            case PerformanceTrend::Degrading: return "Degrading";
            case PerformanceTrend::Stable: return "Stable";
            case PerformanceTrend::Volatile: return "Volatile";
            default: return "Unknown";
        }
    }
    
    std::string performanceMetricTypeToString(PerformanceMetricType type) const {
        switch (type) {
            case PerformanceMetricType::Allocation: return "Allocation";
            case PerformanceMetricType::Deallocation: return "Deallocation";
            case PerformanceMetricType::LockContention: return "LockContention";
            case PerformanceMetricType::MemoryUsage: return "MemoryUsage";
            case PerformanceMetricType::CPUUsage: return "CPUUsage";
            case PerformanceMetricType::SIMDEfficiency: return "SIMDEfficiency";
            case PerformanceMetricType::Throughput: return "Throughput";
            case PerformanceMetricType::Latency: return "Latency";
            case PerformanceMetricType::CacheHitRate: return "CacheHitRate";
            case PerformanceMetricType::ThreadUtilization: return "ThreadUtilization";
            case PerformanceMetricType::IOWait: return "IOWait";
            default: return "Unknown";
        }
    }
    
private:
    std::string currentSessionId_;
    bool debugMode_ = false;
};

int main() {
    std::cout << "=== NLM Performance Profiling Example ===" << std::endl;
    
    // Create example instance
    PerformanceProfilingExample example;
    
    // Run example
    example.runExample();
    
    std::cout << "\n=== Example Complete ===" << std::endl;
    return 0;
}
