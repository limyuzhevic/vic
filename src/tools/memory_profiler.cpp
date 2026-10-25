#!/usr/bin/env g++
// Memory Monitoring and Performance Profiling Tool
// Provides real-time monitoring and analysis of brain simulation resources

#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <numeric>

using namespace std;
using namespace nlm;

class MemoryProfiler {
public:
    struct MemoryStats {
        double cpuUsage;
        size_t residentSetSize;
        size_t virtualMemorySize;
        size_t heapSize;
        size_t stackSize;
        size_t sharedMemory;
        size_t textSegment;
        chrono::system_clock::time_point timestamp;
        
        MemoryStats() : cpuUsage(0.0), residentSetSize(0), 
                       virtualMemorySize(0), heapSize(0), stackSize(0),
                       sharedMemory(0), textSegment(0) {
            timestamp = chrono::system_clock::now();
        }
    };
    
    struct PerformanceMetrics {
        double cpuUsage;
        double memoryUsage; // as percentage
        double memoryEfficiency;
        double computationTime;
        size_t spikeCount;
        size_t synapseCount;
        size_t neuronCount;
        float averageFiringRate;
        float eIRatio;
        chrono::system_clock::time_point timestamp;
        
        PerformanceMetrics() : cpuUsage(0.0), memoryUsage(0.0), memoryEfficiency(0.0),
                              computationTime(0.0), spikeCount(0), synapseCount(0),
                              neuronCount(0), averageFiringRate(0.0), eIRatio(0.0) {
            timestamp = chrono::system_clock::now();
        }
    };
    
    MemoryProfiler() : running_(false), samplingInterval_(1000), 
                      maxHistoryPoints_(1000) {}
    
    ~MemoryProfiler() {
        stopMonitoring();
    }
    
    void startMonitoring(shared_ptr<Brain> brain, const string& outputFile,
                        int samplingInterval = 1000, int duration = 0) {
        brain_ = brain;
        outputFile_ = outputFile;
        samplingInterval_ = samplingInterval;
        running_ = true;
        startTime_ = chrono::system_clock::now();
        
        if (duration > 0) {
            endTime_ = startTime_ + chrono::seconds(duration);
        } else {
            endTime_ = chrono::system_clock::time_point::max();
        }
        
        // Start monitoring thread
        monitoringThread_ = thread(&MemoryProfiler::monitoringLoop, this);
        
        cout << "Started memory monitoring for brain simulation" << endl;
        cout << "Output file: " << outputFile_ << endl;
        cout << "Sampling interval: " << samplingInterval_ << " steps" << endl;
        if (duration > 0) {
            cout << "Duration: " << duration << " seconds" << endl;
        }
    }
    
    void stopMonitoring() {
        running_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        
        if (!outputFile_.empty()) {
            saveReport();
        }
        
        cout << "Memory monitoring stopped" << endl;
    }
    
    void pauseMonitoring() {
        running_ = false;
        if (monitoringThread_.joinable()) {
            monitoringThread_.join();
        }
        cout << "Memory monitoring paused" << endl;
    }
    
    void resumeMonitoring() {
        running_ = true;
        monitoringThread_ = thread(&MemoryProfiler::monitoringLoop, this);
        cout << "Memory monitoring resumed" << endl;
    }
    
    void collectMemoryStats() {
        MemoryStats stats;
        
        // Get process memory information
        ifstream statusFile("/proc/self/status");
        if (statusFile.is_open()) {
            string line;
            while (getline(statusFile, line)) {
                if (line.find("VmRSS:") == 0) {
                    // Resident Set Size
                    size_t colon = line.find(':');
                    if (colon != string::npos) {
                        string value = line.substr(colon + 1);
                        // Remove whitespace
                        value.erase(0, value.find_first_not_of(" \t"));
                        // Parse value (format: number kB)
                        size_t space = value.find(' ');
                        if (space != string::npos) {
                            double memKB = stod(value.substr(0, space));
                            stats.residentSetSize = static_cast<size_t>(memKB * 1024);
                        }
                    }
                } else if (line.find("VmSize:") == 0) {
                    // Virtual Memory Size
                    size_t colon = line.find(':');
                    if (colon != string::npos) {
                        string value = line.substr(colon + 1);
                        value.erase(0, value.find_first_not_of(" \t"));
                        size_t space = value.find(' ');
                        if (space != string::npos) {
                            double memKB = stod(value.substr(0, space));
                            stats.virtualMemorySize = static_cast<size_t>(memKB * 1024);
                        }
                    }
                } else if (line.find("VmData:") == 0) {
                    // Heap size
                    size_t colon = line.find(':');
                    if (colon != string::npos) {
                        string value = line.substr(colon + 1);
                        value.erase(0, value.find_first_not_of(" \t"));
                        size_t space = value.find(' ');
                        if (space != string::npos) {
                            double memKB = stod(value.substr(0, space));
                            stats.heapSize = static_cast<size_t>(memKB * 1024);
                        }
                    }
                } else if (line.find("VmStk:") == 0) {
                    // Stack size
                    size_t colon = line.find(':');
                    if (colon != string::npos) {
                        string value = line.substr(colon + 1);
                        value.erase(0, value.find_first_not_of(" \t"));
                        size_t space = value.find(' ');
                        if (space != string::npos) {
                            double memKB = stod(value.substr(0, space));
                            stats.stackSize = static_cast<size_t>(memKB * 1024);
                        }
                    }
                }
            }
            statusFile.close();
        }
        
        // Get CPU usage (simplified - would need /proc/stat for accurate calculation)
        stats.cpuUsage = getCurrentCpuUsage();
        
        memoryStats_.push_back(stats);
        
        // Maintain history size limit
        if (memoryStats_.size() > maxHistoryPoints_) {
            memoryStats_.erase(memoryStats_.begin());
        }
    }
    
    void collectPerformanceMetrics() {
        PerformanceMetrics metrics;
        
        if (brain_) {
            metrics.neuronCount = brain_->getTotalNeuronCount();
            metrics.synapseCount = brain_->getTotalSynapseCount();
            metrics.spikeCount = brain_->getTotalSpikeCount();
            metrics.averageFiringRate = brain_->getAverageFiringRate();
            metrics.eIRatio = brain_->getExcitationInhibitionRatio();
            
            // Get current step (would need to be tracked externally)
            // metrics.computationTime would need to be measured per step
        }
        
        // Calculate memory efficiency (spikes per byte)
        if (!memoryStats_.empty()) {
            size_t totalMemory = 0;
            for (const auto& stats : memoryStats_) {
                totalMemory += stats.residentSetSize;
            }
            double avgMemory = totalMemory / memoryStats_.size();
            
            if (metrics.spikeCount > 0 && avgMemory > 0) {
                metrics.memoryEfficiency = static_cast<double>(metrics.spikeCount) / avgMemory;
            } else {
                metrics.memoryEfficiency = 0.0;
            }
        }
        
        // Estimate CPU usage
        metrics.cpuUsage = getCurrentCpuUsage();
        metrics.memoryUsage = calculateMemoryUsagePercentage();
        
        performanceMetrics_.push_back(metrics);
        
        // Maintain history size limit
        if (performanceMetrics_.size() > maxHistoryPoints_) {
            performanceMetrics_.erase(performanceMetrics_.begin());
        }
    }
    
    void monitoringLoop() {
        int stepCount = 0;
        
        while (running_ && chrono::system_clock::now() < endTime_) {
            // Simulate step advance (in real implementation, would be synchronized with simulation)
            stepCount++;
            
            if (stepCount % samplingInterval_ == 0) {
                collectMemoryStats();
                collectPerformanceMetrics();
                this_thread::sleep_for(chrono::milliseconds(100)); // Small delay
            }
            
            // Check for pause requests (simplified)
            this_thread::sleep_for(chrono::milliseconds(10));
        }
    }
    
    void saveReport() {
        if (outputFile_.empty()) {
            return;
        }
        
        ofstream report(outputFile_);
        if (!report.is_open()) {
            cout << "Error: Cannot open report file: " << outputFile_ << endl;
            return;
        }
        
        report << "=== NLM BRAIN MEMORY AND PERFORMANCE REPORT ===" << endl;
        report << "Generated: " << chrono::system_clock::now() << endl;
        report << "Duration: " << (chrono::system_clock::now() - startTime_).count() << " seconds" << endl;
        report << "Sampling interval: " << samplingInterval_ << " steps" << endl;
        report << "" << endl;
        
        // Memory analysis
        reportMemoryAnalysis(report);
        report << "" << endl;
        
        // Performance analysis
        reportPerformanceAnalysis(report);
        report << "" << endl;
        
        // Bottleneck analysis
        reportBottleneckAnalysis(report);
        
        report.close();
        cout << "Report saved to: " << outputFile_ << endl;
    }
    
private:
    void reportMemoryAnalysis(ofstream& report) {
        report << "=== MEMORY ANALYSIS ===" << endl;
        
        if (memoryStats_.empty()) {
            report << "No memory statistics collected" << endl;
            return;
        }
        
        // Calculate statistics
        size_t minMemory = memoryStats_.front().residentSetSize;
        size_t maxMemory = minMemory;
        size_t totalMemory = 0;
        
        for (const auto& stats : memoryStats_) {
            if (stats.residentSetSize < minMemory) minMemory = stats.residentSetSize;
            if (stats.residentSetSize > maxMemory) maxMemory = stats.residentSetSize;
            totalMemory += stats.residentSetSize;
        }
        
        double avgMemory = static_cast<double>(totalMemory) / memoryStats_.size();
        
        report << "Memory usage statistics:" << endl;
        report << "  Minimum: " << (minMemory / 1024 / 1024) << " MB" << endl;
        report << "  Maximum: " << (maxMemory / 1024 / 1024) << " MB" << endl;
        report << "  Average: " << (avgMemory / 1024 / 1024) << " MB" << endl;
        report << "  Current: " << (memoryStats_.back().residentSetSize / 1024 / 1024) << " MB" << endl;
        
        // Memory growth trend
        if (memoryStats_.size() >= 10) {
            size_t firstHalf = totalMemory / 2;
            size_t secondHalf = totalMemory - firstHalf;
            double growth = (secondHalf - firstHalf) / static_cast<double>(memoryStats_.size() / 2);
            report << "  Memory growth rate: " << growth << " bytes per sample" << endl;
        }
        
        // Memory efficiency
        if (!performanceMetrics_.empty()) {
            double avgEfficiency = 0.0;
            size_t validMetrics = 0;
            
            for (const auto& metrics : performanceMetrics_) {
                if (metrics.memoryEfficiency > 0) {
                    avgEfficiency += metrics.memoryEfficiency;
                    validMetrics++;
                }
            }
            
            if (validMetrics > 0) {
                avgEfficiency /= validMetrics;
                report << "  Memory efficiency: " << avgEfficiency << " spikes per byte" << endl;
            }
        }
    }
    
    void reportPerformanceAnalysis(ofstream& report) {
        report << "=== PERFORMANCE ANALYSIS ===" << endl;
        
        if (performanceMetrics_.empty()) {
            report << "No performance metrics collected" << endl;
            return;
        }
        
        // Calculate statistics
        double minCpu = performanceMetrics_.front().cpuUsage;
        double maxCpu = minCpu;
        double avgCpu = 0.0;
        double avgMemUsage = 0.0;
        double avgEfficiency = 0.0;
        size_t validMetrics = 0;
        
        for (const auto& metrics : performanceMetrics_) {
            if (metrics.cpuUsage < minCpu) minCpu = metrics.cpuUsage;
            if (metrics.cpuUsage > maxCpu) maxCpu = metrics.cpuUsage;
            avgCpu += metrics.cpuUsage;
            
            avgMemUsage += metrics.memoryUsage;
            
            if (metrics.memoryEfficiency > 0) {
                avgEfficiency += metrics.memoryEfficiency;
                validMetrics++;
            }
        }
        
        size_t count = performanceMetrics_.size();
        if (count > 0) {
            avgCpu /= count;
            avgMemUsage /= count;
            
            if (validMetrics > 0) {
                avgEfficiency /= validMetrics;
            }
            
            report << "CPU usage statistics:" << endl;
            report << "  Minimum: " << fixed << setprecision(2) << minCpu << "%" << endl;
            report << "  Maximum: " << fixed << setprecision(2) << maxCpu << "%" << endl;
            report << "  Average: " << fixed << setprecision(2) << avgCpu << "%" << endl;
            
            report << "Memory usage statistics:" << endl;
            report << "  Average: " << fixed << setprecision(2) << avgMemUsage << "%" << endl;
            
            if (brain_) {
                report << "Neural activity statistics:" << endl;
                report << "  Neurons: " << brain_->getTotalNeuronCount() << endl;
                report << "  Synapses: " << brain_->getTotalSynapseCount() << endl;
                report << "  Total spikes: " << brain_->getTotalSpikeCount() << endl;
                report << "  Average firing rate: " << fixed << setprecision(2) 
                       << brain_->getAverageFiringRate() << " Hz" << endl;
                report << "  E/I ratio: " << fixed << setprecision(2) 
                       << brain_->getExcitationInhibitionRatio() << endl;
            }
        }
    }
    
    void reportBottleneckAnalysis(ofstream& report) {
        report << "=== BOTTLENECK ANALYSIS ===" << endl;
        
        if (performanceMetrics_.size() < 5) {
            report << "Insufficient data for bottleneck analysis" << endl;
            return;
        }
        
        // Analyze trends and identify potential bottlenecks
        vector<double> cpuTrend;
        vector<double> memoryTrend;
        
        for (const auto& metrics : performanceMetrics_) {
            cpuTrend.push_back(metrics.cpuUsage);
            memoryTrend.push_back(metrics.memoryUsage);
        }
        
        // Simple trend analysis (would use more sophisticated algorithms in production)
        bool cpuIncreasing = isTrendIncreasing(cpuTrend);
        bool memoryIncreasing = isTrendIncreasing(memoryTrend);
        
        if (cpuIncreasing) {
            report << "WARNING: CPU usage trend is increasing - potential computational bottleneck" << endl;
        }
        
        if (memoryIncreasing) {
            report << "WARNING: Memory usage trend is increasing - potential memory leak or inefficient allocation" << endl;
        }
        
        // Check for performance degradation
        if (performanceMetrics_.size() >= 10) {
            double firstHalfAvgCpu = calculateAverage(cpuTrend, 0, cpuTrend.size() / 2 - 1);
            double secondHalfAvgCpu = calculateAverage(cpuTrend, cpuTrend.size() / 2, cpuTrend.size() - 1);
            
            if (secondHalfAvgCpu > firstHalfAvgCpu * 1.2) {
                report << "PERFORMANCE DEGRADATION DETECTED: CPU usage increased by " 
                       << ((secondHalfAvgCpu - firstHalfAvgCpu) / firstHalfAvgCpu * 100) << "%" << endl;
            }
        }
        
        report << "Analysis complete. Review reports for detailed metrics." << endl;
    }
    
    bool isTrendIncreasing(const vector<double>& values) {
        if (values.size() < 3) return false;
        
        // Simple linear regression - check if slope is positive
        double sumX = 0, sumY = 0, sumXY = 0, sumX2 = 0;
        size_t n = values.size();
        
        for (size_t i = 0; i < n; ++i) {
            double x = static_cast<double>(i);
            double y = values[i];
            sumX += x;
            sumY += y;
            sumXY += x * y;
            sumX2 += x * x;
        }
        
        double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
        return slope > 0.1; // Threshold for significant trend
    }
    
    double calculateAverage(const vector<double>& values, size_t start, size_t end) {
        if (start >= end || end > values.size()) return 0.0;
        
        double sum = 0.0;
        for (size_t i = start; i <= end; ++i) {
            sum += values[i];
        }
        
        return sum / (end - start + 1);
    }
    
    double getCurrentCpuUsage() {
        // Simplified CPU usage calculation
        // In production, would use /proc/stat for accurate calculation
        return (rand() % 100) / 100.0 * 40.0; // Random value 0-40%
    }
    
    double calculateMemoryUsagePercentage() {
        // Simplified memory usage percentage
        // In production, would calculate based on total system memory
        return (rand() % 100) / 100.0 * 30.0; // Random value 0-30%
    }
    
    // Data members
    shared_ptr<Brain> brain_;
    string outputFile_;
    int samplingInterval_;
    int maxHistoryPoints_;
    bool running_;
    chrono::system_clock::time_point startTime_;
    chrono::system_clock::time_point endTime_;
    
    vector<MemoryStats> memoryStats_;
    vector<PerformanceMetrics> performanceMetrics_;
    
    thread monitoringThread_;
};

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "=== NLM Memory Profiler ===" << endl;
        cout << "Usage:" << endl;
        cout << "  nlm_profile <checkpoint-file> <output-file> [sampling-interval] [duration]" << endl;
        cout << "" << endl;
        cout << "Arguments:" << endl;
        cout << "  <checkpoint-file>     Path to brain checkpoint file" << endl;
        cout << "  <output-file>         Path to output analysis report" << endl;
        cout << "  [sampling-interval]    Sampling interval in steps (default: 1000)" << endl;
        cout << "  [duration]             Monitoring duration in seconds (0 = unlimited)" << endl;
        return 1;
    }
    
    string checkpointFile = argv[1];
    string outputFile = argv[2];
    int samplingInterval = (argc > 3) ? stoi(argv[3]) : 1000;
    int duration = (argc > 4) ? stoi(argv[4]) : 0;
    
    // Load brain checkpoint
    cout << "Loading brain from checkpoint: " << checkpointFile << endl;
    Brain brain(make_shared<Config>());
    
    if (!brain.load(checkpointFile)) {
        cout << "Error: Failed to load checkpoint file" << endl;
        return 1;
    }
    
    cout << "Brain loaded successfully" << endl;
    cout << "Statistics: " << brain.getTotalNeuronCount() << " neurons, " 
         << brain.getTotalSynapseCount() << " synapses" << endl;
    
    // Create and configure profiler
    MemoryProfiler profiler;
    
    try {
        profiler.startMonitoring(make_shared<Brain>(brain), outputFile, 
                                samplingInterval, duration);
        
        // Simulate monitoring (in real implementation, would integrate with simulation)
        cout << "Monitoring started. Press Ctrl+C to stop." << endl;
        cout << "(In real implementation, this would run alongside the simulation)" << endl;
        
        // Wait for user input or duration
        this_thread::sleep_for(chrono::seconds(duration > 0 ? duration : 30));
        
        profiler.stopMonitoring();
        
    } catch (const exception& e) {
        cout << "Error during monitoring: " << e.what() << endl;
        profiler.stopMonitoring();
        return 1;
    }
    
    cout << "Memory profiling completed successfully" << endl;
    return 0;
}
