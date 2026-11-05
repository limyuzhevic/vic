# Performance Monitoring and Profiling Tools
// Enhanced Performance Monitoring for NLM

#include <chrono>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cmath>

namespace nlm {

// Performance measurement structures
struct PerformanceMetrics {
    double totalTime;           // Total execution time in seconds
    double cpuTime;            // CPU time in seconds
    double memoryUsage;        // Memory usage in MB
    size_t totalSteps;         // Total simulation steps
    size_t totalSpikes;        // Total spikes generated
    size_t neuronCount;        // Number of neurons
    size_t synapseCount;       // Number of synapses
    double stepsPerSecond;     // Steps per second
    double spikesPerSecond;    // Spikes per second
    double efficiency;         // Performance efficiency score
    double throughput;        // Data throughput
    double scalability;       // Scalability factor
};

struct TimeMeasurement {
    std::string name;          // Measurement name
    double duration;           // Duration in seconds
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool isRunning;           // Whether measurement is currently active
};

struct MemoryProfile {
    double peakMemory;        // Peak memory usage in MB
    double currentMemory;     // Current memory usage in MB
    std::vector<double> samples; // Memory samples over time
    double sampleInterval;    // Sampling interval in seconds
};

struct SpikeAnalysis {
    std::vector<size_t> spikeCounts;   // Spike count per time step
    double firingRate;               // Average firing rate in Hz
    double coefficientOfVariation;   // Firing regularity
    size_t maxSpikes;                // Maximum spikes in a single step
    double burstFrequency;          // Burst frequency
    double meanInterSpikeInterval;   // Average time between spikes
};

class PerformanceMonitor {
private:
    PerformanceMetrics currentMetrics;
    std::vector<TimeMeasurement> timeMeasurements;
    MemoryProfile memoryProfile;
    SpikeAnalysis spikeAnalysis;
    std::string sessionId;
    bool isMonitoring;
    
    // Internal helper methods
    void updateTimeMeasurements() {}
    void updateMemoryProfile() {}
    void updateSpikeAnalysis() {}
    void calculateEfficiency() {}
    void calculateThroughput() {}
    void calculateScalability() {}

public:
    PerformanceMonitor() : isMonitoring(false) {
        sessionId = "perf_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        currentMetrics.totalTime = 0.0;
        currentMetrics.cpuTime = 0.0;
        currentMetrics.memoryUsage = 0.0;
        currentMetrics.totalSteps = 0;
        currentMetrics.totalSpikes = 0;
        currentMetrics.neuronCount = 0;
        currentMetrics.synapseCount = 0;
        currentMetrics.stepsPerSecond = 0.0;
        currentMetrics.spikesPerSecond = 0.0;
        currentMetrics.efficiency = 0.0;
        currentMetrics.throughput = 0.0;
        currentMetrics.scalability = 0.0;
    }
    
    void startMeasurement(const std::string& name) {
        TimeMeasurement tm;
        tm.name = name;
        tm.startTime = std::chrono::high_resolution_clock::now();
        tm.isRunning = true;
        timeMeasurements.push_back(tm);
    }
    
    void stopMeasurement(const std::string& name) {
        for (auto& tm : timeMeasurements) {
            if (tm.name == name && tm.isRunning) {
                tm.endTime = std::chrono::high_resolution_clock::now();
                tm.duration = std::chrono::duration<double>(tm.endTime - tm.startTime).count();
                tm.isRunning = false;
                break;
            }
        }
    }
    
    void startMonitoring() {
        isMonitoring = true;
        startMeasurement("total_execution");
        memoryProfile.samples.clear();
        spikeAnalysis.spikeCounts.clear();
    }
    
    void stopMonitoring() {
        isMonitoring = false;
        stopMeasurement("total_execution");
        
        // Calculate final metrics
        calculateFinalMetrics();
    }
    
    void recordSpikeCount(size_t spikes, double timestep) {
        if (isMonitoring) {
            spikeAnalysis.spikeCounts.push_back(spikes);
            currentMetrics.totalSpikes += spikes;
        }
    }
    
    void updateMemoryUsage(double memoryMB) {
        if (isMonitoring) {
            memoryProfile.currentMemory = memoryMB;
            memoryProfile.peakMemory = std::max(memoryProfile.peakMemory, memoryMB);
            memoryProfile.samples.push_back(memoryMB);
        }
    }
    
    void updateBrainStats(size_t neurons, size_t synapses, size_t steps) {
        if (isMonitoring) {
            currentMetrics.neuronCount = neurons;
            currentMetrics.synapseCount = synapses;
            currentMetrics.totalSteps = steps;
        }
    }
    
    PerformanceMetrics getMetrics() const {
        return currentMetrics;
    }
    
    std::string getSessionId() const {
        return sessionId;
    }
    
    void generateReport(const std::string& filename) {
        std::ofstream reportFile(filename);
        if (!reportFile.is_open()) {
            return;
        }
        
        reportFile << "NLM Performance Report" << std::endl;
        reportFile << "=====================" << std::endl;
        reportFile << "Session ID: " << sessionId << std::endl;
        reportFile << "Total Execution Time: " << currentMetrics.totalTime << " seconds" << std::endl;
        reportFile << "Total Steps: " << currentMetrics.totalSteps << std::endl;
        reportFile << "Total Spikes: " << currentMetrics.totalSpikes << std::endl;
        reportFile << "Average Firing Rate: " << currentMetrics.spikesPerSecond << " Hz" << std::endl;
        reportFile << "Steps per Second: " << currentMetrics.stepsPerSecond << std::endl;
        reportFile << "Peak Memory Usage: " << currentMetrics.memoryUsage << " MB" << std::endl;
        reportFile << "Efficiency Score: " << currentMetrics.efficiency << std::endl;
        reportFile << "Throughput: " << currentMetrics.throughput << " ops/second" << std::endl;
        reportFile << "Scalability Factor: " << currentMetrics.scalability << std::endl;
        
        // Time measurements
        reportFile << "\nDetailed Timing:" << std::endl;
        for (const auto& tm : timeMeasurements) {
            reportFile << "  " << tm.name << ": " << tm.duration << " seconds" << std::endl;
        }
        
        // Spike analysis
        reportFile << "\nSpike Analysis:" << std::endl;
        reportFile << "  Firing Rate: " << spikeAnalysis.firingRate << " Hz" << std::endl;
        reportFile << "  Coefficient of Variation: " << spikeAnalysis.coefficientOfVariation << std::endl;
        reportFile << "  Max Spikes per Step: " << spikeAnalysis.maxSpikes << std::endl;
        
        // Memory profile
        reportFile << "\nMemory Profile:" << std::endl;
        reportFile << "  Peak Memory: " << memoryProfile.peakMemory << " MB" << std::endl;
        reportFile << "  Average Memory: " << calculateAverageMemory() << " MB" << std::endl;
        
        reportFile.close();
    }
    
private:
    void calculateFinalMetrics() {
        // Calculate duration from time measurements
        double totalDuration = 0.0;
        for (const auto& tm : timeMeasurements) {
            if (!tm.isRunning) { // Only stopped measurements
                totalDuration += tm.duration;
            }
        }
        currentMetrics.totalTime = totalDuration;
        
        if (totalDuration > 0.0) {
            currentMetrics.stepsPerSecond = static_cast<double>(currentMetrics.totalSteps) / totalDuration;
            currentMetrics.spikesPerSecond = static_cast<double>(currentMetrics.totalSpikes) / totalDuration;
        }
        
        calculateEfficiency();
        calculateThroughput();
        calculateScalability();
    }
    
    double calculateAverageMemory() const {
        if (memoryProfile.samples.empty()) {
            return 0.0;
        }
        double sum = 0.0;
        for (double sample : memoryProfile.samples) {
            sum += sample;
        }
        return sum / memoryProfile.samples.size();
    }
    
    void calculateEfficiency() {
        // Efficiency is based on steps per second relative to theoretical maximum
        double theoreticalMaxStepsPerSecond = 1000000.0; // Assumed maximum
        currentMetrics.efficiency = (currentMetrics.stepsPerSecond / theoreticalMaxStepsPerSecond) * 100.0;
    }
    
    void calculateThroughput() {
        // Throughput considers both computational and memory operations
        double operationsPerSecond = (currentMetrics.stepsPerSecond * currentMetrics.synapseCount) + 
                                   (currentMetrics.spikesPerSecond * 10.0); // Estimate for spike processing
        currentMetrics.throughput = operationsPerSecond;
    }
    
    void calculateScalability() {
        // Simple scalability calculation based on synapse density
        if (currentMetrics.neuronCount > 0) {
            double synapseDensity = static_cast<double>(currentMetrics.synapseCount) / currentMetrics.neuronCount;
            currentMetrics.scalability = std::min(synapseDensity / 100.0, 1.0); // Normalize to 1.0
        } else {
            currentMetrics.scalability = 0.0;
        }
    }
};

// Global performance monitor instance
PerformanceMonitor globalPerformanceMonitor;

// Convenience functions for easy integration
inline void startPerformanceMonitoring() {
    globalPerformanceMonitor.startMonitoring();
}

inline void stopPerformanceMonitoring() {
    globalPerformanceMonitor.stopMonitoring();
}

inline void recordPerformanceMetrics(size_t neurons, size_t synapses, size_t steps, size_t spikes, double memoryMB, double timestep) {
    globalPerformanceMonitor.updateBrainStats(neurons, synapses, steps);
    globalPerformanceMonitor.recordSpikeCount(spikes, timestep);
    globalPerformanceMonitor.updateMemoryUsage(memoryMB);
}

inline void generatePerformanceReport(const std::string& filename) {
    globalPerformanceMonitor.generateReport(filename);
}

inline std::string getPerformanceSessionId() {
    return globalPerformanceMonitor.getSessionId();
}

inline PerformanceMetrics getCurrentPerformanceMetrics() {
    return globalPerformanceMonitor.getMetrics();
}

// Batch performance analyzer for multiple runs
class PerformanceBatchAnalyzer {
private:
    std::vector<PerformanceMetrics> runMetrics;
    std::string batchId;

public:
    PerformanceBatchAnalyzer() {
        batchId = "batch_" + std::to_string(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    }
    
    void addRunMetrics(const PerformanceMetrics& metrics) {
        runMetrics.push_back(metrics);
    }
    
    void generateBatchReport(const std::string& filename) {
        std::ofstream reportFile(filename);
        if (!reportFile.is_open()) {
            return;
        }
        
        reportFile << "NLM Performance Batch Analysis" << std::endl;
        reportFile << "==============================" << std::endl;
        reportFile << "Batch ID: " << batchId << std::endl;
        reportFile << "Number of Runs: " << runMetrics.size() << std::endl;
        
        if (runMetrics.empty()) {
            reportFile.close();
            return;
        }
        
        // Calculate statistics
        double avgStepsPerSec = 0.0;
        double avgSpikesPerSec = 0.0;
        double avgMemory = 0.0;
        double avgEfficiency = 0.0;
        
        for (const auto& metrics : runMetrics) {
            avgStepsPerSec += metrics.stepsPerSecond;
            avgSpikesPerSec += metrics.spikesPerSecond;
            avgMemory += metrics.memoryUsage;
            avgEfficiency += metrics.efficiency;
        }
        
        size_t count = runMetrics.size();
        avgStepsPerSec /= count;
        avgSpikesPerSec /= count;
        avgMemory /= count;
        avgEfficiency /= count;
        
        reportFile << "\nAverage Metrics:" << std::endl;
        reportFile << "  Steps per Second: " << avgStepsPerSec << std::endl;
        reportFile << "  Spikes per Second: " << avgSpikesPerSec << std::endl;
        reportFile << "  Memory Usage: " << avgMemory << " MB" << std::endl;
        reportFile << "  Efficiency: " << avgEfficiency << "%" << std::endl;
        
        // Individual run details
        reportFile << "\nIndividual Run Details:" << std::endl;
        for (size_t i = 0; i < runMetrics.size(); ++i) {
            reportFile << "  Run " << i + 1 << ": "
                      << "Steps/sec: " << runMetrics[i].stepsPerSecond
                      << ", Spikes/sec: " << runMetrics[i].spikesPerSecond
                      << ", Memory: " << runMetrics[i].memoryUsage << " MB"
                      << std::endl;
        }
        
        // Identify outliers and bottlenecks
        reportFile << "\nAnalysis:" << std::endl;
        reportFile << "  Fastest Run: " << findFastestRun() << std::endl;
        reportFile << "  Most Memory Efficient: " << findMostMemoryEfficient() << std::endl;
        reportFile << "  Most Scalable: " << findMostScalable() << std::endl;
        
        reportFile.close();
    }
    
private:
    std::string findFastestRun() const {
        if (runMetrics.empty()) return "N/A";
        
        size_t fastestIdx = 0;
        double fastestSpeed = runMetrics[0].stepsPerSecond;
        
        for (size_t i = 1; i < runMetrics.size(); ++i) {
            if (runMetrics[i].stepsPerSecond > fastestSpeed) {
                fastestSpeed = runMetrics[i].stepsPerSecond;
                fastestIdx = i;
            }
        }
        
        return "Run " + std::to_string(fastestIdx + 1) + " (" + std::to_string(fastestSpeed) + " steps/sec)";
    }
    
    std::string findMostMemoryEfficient() const {
        if (runMetrics.empty()) return "N/A";
        
        size_t efficientIdx = 0;
        double mostEfficient = runMetrics[0].memoryUsage;
        
        for (size_t i = 1; i < runMetrics.size(); ++i) {
            if (runMetrics[i].memoryUsage < mostEfficient) {
                mostEfficient = runMetrics[i].memoryUsage;
                efficientIdx = i;
            }
        }
        
        return "Run " + std::to_string(efficientIdx + 1) + " (" + std::to_string(mostEfficient) + " MB)";
    }
    
    std::string findMostScalable() const {
        if (runMetrics.empty()) return "N/A";
        
        size_t mostScalableIdx = 0;
        double mostScalable = runMetrics[0].scalability;
        
        for (size_t i = 1; i < runMetrics.size(); ++i) {
            if (runMetrics[i].scalability > mostScalable) {
                mostScalable = runMetrics[i].scalability;
                mostScalableIdx = i;
            }
        }
        
        return "Run " + std::to_string(mostScalableIdx + 1) + " (" + std::to_string(mostScalable * 100) + "% scalability)";
    }
};

} // namespace nlm

// Usage example functions
inline void examplePerformanceMonitoring() {
    std::cout << "=== Performance Monitoring Example ===" << std::endl;
    
    // Start monitoring
    nlm::startPerformanceMonitoring();
    
    // Simulate some work
    for (int i = 0; i < 1000; ++i) {
        // Simulate brain operation
        nlm::recordPerformanceMetrics(500, 2500, i, rand() % 100, 100.0 + (rand() % 50), 0.001);
        
        if (i % 100 == 0) {
            std::cout << "Step " << i << ": " << nlm::getCurrentPerformanceMetrics().stepsPerSecond << " steps/sec" << std::endl;
        }
    }
    
    // Stop monitoring and generate report
    nlm::stopPerformanceMonitoring();
    nlm::generatePerformanceReport("performance_report.txt");
    
    std::cout << "Performance report generated: performance_report.txt" << std::endl;
    std::cout << "Session ID: " << nlm::getPerformanceSessionId() << std::endl;
}

inline void exampleBatchAnalysis() {
    std::cout << "=== Performance Batch Analysis Example ===" << std::endl;
    
    nlm::PerformanceBatchAnalyzer analyzer;
    
    // Simulate multiple runs with different parameters
    for (int run = 0; run < 5; ++run) {
        nlm::PerformanceMetrics metrics;
        metrics.totalSteps = (run + 1) * 1000;
        metrics.totalSpikes = (run + 1) * 500;
        metrics.stepsPerSecond = static_cast<double>(metrics.totalSteps) / 10.0;
        metrics.spikesPerSecond = static_cast<double>(metrics.totalSpikes) / 10.0;
        metrics.memoryUsage = 100.0 + (run * 20);
        metrics.efficiency = 85.0 - (run * 5);
        metrics.throughput = metrics.stepsPerSecond * 100.0;
        metrics.scalability = 0.5 + (run * 0.1);
        
        analyzer.addRunMetrics(metrics);
        
        std::cout << "Completed run " << run + 1 << ": " << metrics.stepsPerSecond << " steps/sec" << std::endl;
    }
    
    analyzer.generateBatchReport("batch_performance_report.txt");
    std::cout << "Batch report generated: batch_performance_report.txt" << std::endl;
}
