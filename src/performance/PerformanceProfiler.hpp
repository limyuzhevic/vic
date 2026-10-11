// NLM Performance Monitoring and Benchmarking Tools
// Advanced performance monitoring, profiling, and benchmarking utilities

#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace nlm {

// Performance metrics collection
struct PerformanceMetrics {
    // Timing metrics
    double totalTime;           // Total execution time in seconds
    double cpuTime;             // CPU time in seconds
    double wallTime;            // Wall clock time in seconds
    
    // Memory metrics
    size_t peakMemoryUsage;     // Peak memory usage in bytes
    size_t allocatedMemory;     // Total allocated memory in bytes
    size_t deallocatedMemory;   // Total deallocated memory in bytes
    
    // Simulation metrics
    size_t stepsExecuted;       // Number of simulation steps
    size_t neuronsActive;       // Number of active neurons
    size_t spikesGenerated;     // Number of spikes generated
    size_t synapsesActive;      // Number of active synapses
    
    // Learning metrics
    double learningRate;        // Effective learning rate
    double plasticityStrength;  // Total plasticity strength
    double memoryRetention;    // Memory retention efficiency
    
    // Throughput metrics
    double stepsPerSecond;      // Simulation steps per second
    double neuronsPerSecond;    // Neuron operations per second
    double spikesPerSecond;     // Spike processing rate
    
    // Quality metrics
    double stabilityScore;      // Network stability score (0.0 - 1.0)
    double efficiencyScore;     // Computational efficiency score
    double accuracyScore;       // Simulation accuracy score
    
    // Profiling data
    std::map<std::string, double> customMetrics;
    
    PerformanceMetrics() : totalTime(0.0), cpuTime(0.0), wallTime(0.0),
                           peakMemoryUsage(0), allocatedMemory(0), deallocatedMemory(0),
                           stepsExecuted(0), neuronsActive(0), spikesGenerated(0), synapsesActive(0),
                           learningRate(0.0), plasticityStrength(0.0), memoryRetention(0.0),
                           stepsPerSecond(0.0), neuronsPerSecond(0.0), spikesPerSecond(0.0),
                           stabilityScore(0.0), efficiencyScore(0.0), accuracyScore(0.0) {}
};

// High-resolution timer for profiling
class HighResTimer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool isRunning;
    
public:
    HighResTimer() : isRunning(false) {}
    
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        isRunning = true;
    }
    
    void stop() {
        if (isRunning) {
            endTime = std::chrono::high_resolution_clock::now();
            isRunning = false;
        }
    }
    
    double elapsedSeconds() const {
        if (!isRunning) {
            return std::chrono::duration<double>(
                endTime - startTime).count();
        } else {
            return std::chrono::duration<double>(
                std::chrono::high_resolution_clock::now() - startTime).count();
        }
    }
    
    double elapsedMilliseconds() const {
        return elapsedSeconds() * 1000.0;
    }
    
    bool isRunning() const { return isRunning; }
    void reset() { isRunning = false; }
};

// Memory profiler
class MemoryProfiler {
private:
    size_t peakMemoryUsage;
    size_t allocatedMemory;
    size_t deallocatedMemory;
    
public:
    MemoryProfiler() : peakMemoryUsage(0), allocatedMemory(0), deallocatedMemory(0) {}
    
    void trackAllocation(size_t size) {
        allocatedMemory += size;
        if (allocatedMemory > peakMemoryUsage) {
            peakMemoryUsage = allocatedMemory;
        }
    }
    
    void trackDeallocation(size_t size) {
        deallocatedMemory += size;
    }
    
    size_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    size_t getAllocatedMemory() const { return allocatedMemory; }
    size_t getDeallocatedMemory() const { return deallocatedMemory; }
    
    double getCurrentMemoryUsage() const {
        return static_cast<double>(allocatedMemory - deallocatedMemory);
    }
    
    void reset() {
        peakMemoryUsage = 0;
        allocatedMemory = 0;
        deallocatedMemory = 0;
    }
};

// Simulation profiler
class SimulationProfiler {
private:
    HighResTimer timer;
    MemoryProfiler memoryProfiler;
    
    struct StepMetrics {
        double stepTime;
        size_t spikesGenerated;
        size_t neuronsActive;
        double learningScore;
        std::map<std::string, double> customData;
    };
    
    std::vector<StepMetrics> stepMetrics;
    size_t stepsRecorded;
    
public:
    SimulationProfiler() : stepsRecorded(0) {}
    
    void startStepProfiling() {
        timer.start();
    }
    
    void recordStepMetrics(size_t spikesGenerated, size_t neuronsActive, 
                          double learningScore = 0.0,
                          const std::map<std::string, double>& customData = {}) {
        timer.stop();
        
        StepMetrics metrics;
        metrics.stepTime = timer.elapsedSeconds();
        metrics.spikesGenerated = spikesGenerated;
        metrics.neuronsActive = neuronsActive;
        metrics.learningScore = learningScore;
        metrics.customData = customData;
        
        stepMetrics.push_back(metrics);
        stepsRecorded++;
        
        timer.start();
    }
    
    PerformanceMetrics getPerformanceMetrics() const {
        PerformanceMetrics metrics;
        
        if (!stepMetrics.empty()) {
            double totalTime = 0.0;
            size_t totalSpikes = 0;
            size_t totalNeuronsActive = 0;
            double totalLearningScore = 0.0;
            
            for (const auto& step : stepMetrics) {
                totalTime += step.stepTime;
                totalSpikes += step.spikesGenerated;
                totalNeuronsActive += step.neuronsActive;
                totalLearningScore += step.learningScore;
            }
            
            metrics.totalTime = totalTime;
            metrics.stepsExecuted = stepsRecorded;
            metrics.spikesGenerated = totalSpikes;
            metrics.neuronsActive = totalNeuronsActive;
            metrics.learningRate = stepsRecorded > 0 ? totalLearningScore / stepsRecorded : 0.0;
            metrics.stepsPerSecond = stepsRecorded > 0 ? stepsRecorded / totalTime : 0.0;
            metrics.spikesPerSecond = totalSpikes > 0 ? static_cast<double>(totalSpikes) / totalTime : 0.0;
            metrics.neuronsPerSecond = totalNeuronsActive > 0 ? static_cast<double>(totalNeuronsActive) / totalTime : 0.0;
            
            // Calculate stability score (based on variance in step times)
            if (stepsRecorded > 1) {
                double meanStepTime = totalTime / stepsRecorded;
                double variance = 0.0;
                for (const auto& step : stepMetrics) {
                    double diff = step.stepTime - meanStepTime;
                    variance += diff * diff;
                }
                variance /= stepsRecorded;
                double stdDev = std::sqrt(variance);
                metrics.stabilityScore = std::max(0.0, 1.0 - stdDev / meanStepTime);
            } else {
                metrics.stabilityScore = 1.0;
            }
        }
        
        metrics.peakMemoryUsage = memoryProfiler.getPeakMemoryUsage();
        
        return metrics;
    }
    
    void recordMemoryAllocation(size_t size) {
        memoryProfiler.trackAllocation(size);
    }
    
    void recordMemoryDeallocation(size_t size) {
        memoryProfiler.trackDeallocation(size);
    }
    
    std::vector<StepMetrics> getStepMetrics() const { return stepMetrics; }
    size_t getStepsRecorded() const { return stepsRecorded; }
    
    void reset() {
        stepMetrics.clear();
        stepsRecorded = 0;
        memoryProfiler.reset();
    }
};

// Benchmarking utility
class Benchmarker {
private:
    struct BenchmarkConfig {
        size_t neuronCount;
        size_t steps;
        double timestep;
        std::string name;
        std::string description;
        std::map<std::string, std::string> customParams;
    };
    
    std::vector<BenchmarkConfig> benchmarks;
    std::vector<PerformanceMetrics> results;
    
public:
    Benchmarker() {}
    
    void addBenchmark(const std::string& name, const std::string& description,
                     size_t neuronCount, size_t steps, double timestep = 0.001) {
        BenchmarkConfig config;
        config.name = name;
        config.description = description;
        config.neuronCount = neuronCount;
        config.steps = steps;
        config.timestep = timestep;
        benchmarks.push_back(config);
    }
    
    // Simple benchmark execution
    PerformanceMetrics runBenchmark(const BenchmarkConfig& config) {
        HighResTimer timer;
        SimulationProfiler profiler;
        
        std::cout << "Running benchmark: " << config.name << "\n";
        std::cout << "  Neurons: " << config.neuronCount << "\n";
        std::cout << "  Steps: " << config.steps << "\n";
        std::cout << "  Timestep: " << config.timestep << "s\n";
        std::cout << "  Starting...\n";
        
        timer.start();
        profiler.startStepProfiling();
        
        // Execute benchmark (placeholder - in real implementation would create brain and run simulation)
        for (size_t step = 0; step < config.steps; ++step) {
            profiler.startStepProfiling();
            
            // Simulate some computation
            size_t spikesGenerated = static_cast<size_t>(config.neuronCount * 0.01 * (step % 10));
            size_t neuronsActive = static_cast<size_t>(config.neuronCount * 0.05 * (step % 5));
            
            // Record step metrics
            profiler.recordStepMetrics(spikesGenerated, neuronsActive);
            
            // Simulate memory usage
            if (step % 100 == 0) {
                profiler.recordMemoryAllocation(1024); // 1KB
            }
            if (step % 150 == 0) {
                profiler.recordMemoryDeallocation(512); // 512B
            }
        }
        
        PerformanceMetrics metrics = profiler.getPerformanceMetrics();
        metrics.cpuTime = timer.elapsedSeconds();
        metrics.wallTime = metrics.cpuTime;
        
        std::cout << "  Completed in " << std::fixed << std::setprecision(2) 
                  << metrics.totalTime << "s\n";
        std::cout << "  Steps/sec: " << std::fixed << std::setprecision(0) 
                  << metrics.stepsPerSecond << "\n";
        std::cout << "  Memory usage: " << metrics.peakMemoryUsage / 1024 << "KB\n";
        
        return metrics;
    }
    
    void runAllBenchmarks() {
        std::cout << "========================================\n";
        std::cout << "NLM Performance Benchmark Suite\n";
        std::cout << "========================================\n\n";
        
        // Add standard benchmarks if none exist
        if (benchmarks.empty()) {
            addBenchmark("Small Network", "Small network (100 neurons, 1000 steps)", 100, 1000);
            addBenchmark("Medium Network", "Medium network (500 neurons, 1000 steps)", 500, 1000);
            addBenchmark("Large Network", "Large network (1000 neurons, 500 steps)", 1000, 500);
            addBenchmark("Dense Network", "Dense network (2000 neurons, 200 steps)", 2000, 200);
        }
        
        for (const auto& benchmark : benchmarks) {
            PerformanceMetrics result = runBenchmark(benchmark);
            results.push_back(result);
            
            std::cout << "\nResults for " << benchmark.name << ":\n";
            printResults(result);
            std::cout << "\n";
        }
        
        printSummary();
    }
    
    void printResults(const PerformanceMetrics& metrics) const {
        std::cout << "  Total Time:      " << std::fixed << std::setprecision(2) 
                  << metrics.totalTime << "s\n";
        std::cout << "  Steps/sec:       " << std::fixed << std::setprecision(0) 
                  << metrics.stepsPerSecond << "\n";
        std::cout << "  Spikes/sec:      " << std::fixed << std::setprecision(1) 
                  << metrics.spikesPerSecond << "\n";
        std::cout << "  Memory Peak:     " << metrics.peakMemoryUsage / 1024 << "KB\n";
        std::cout << "  Learning Rate:   " << std::fixed << std::setprecision(4) 
                  << metrics.learningRate << "\n";
        std::cout << "  Stability Score: " << std::fixed << std::setprecision(3) 
                  << metrics.stabilityScore << "\n";
        std::cout << "  Efficiency:      " << std::fixed << std::setprecision(3) 
                  << metrics.efficiencyScore << "\n";
    }
    
    void printSummary() const {
        std::cout << "========================================\n";
        std::cout << "Benchmark Summary\n";
        std::cout << "========================================\n";
        
        if (results.empty()) {
            std::cout << "No benchmark results available.\n";
            return;
        }
        
        std::cout << std::left << std::setw(20) << "Benchmark" 
                  << std::setw(12) << "Neurons" 
                  << std::setw(10) << "Steps" 
                  << std::setw(12) << "Steps/s" 
                  << std::setw(12) << "Time(s)" 
                  << std::setw(12) << "Memory(KB)" 
                  << std::setw(12) << "Learning\n";
        
        std::cout << std::string(90, '-') << "\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& benchmark = benchmarks[i];
            const auto& result = results[i];
            
            std::cout << std::left << std::setw(20) << benchmark.name
                      << std::setw(12) << benchmark.neuronCount
                      << std::setw(10) << benchmark.steps
                      << std::setw(12) << std::fixed << std::setprecision(0) << result.stepsPerSecond
                      << std::setw(12) << std::fixed << std::setprecision(2) << result.totalTime
                      << std::setw(12) << result.peakMemoryUsage / 1024
                      << std::setw(12) << std::fixed << std::setprecision(4) << result.learningRate
                      << "\n";
        }
        
        std::cout << std::string(90, '-') << "\n";
        
        // Calculate average performance
        double avgStepsPerSecond = 0.0;
        double avgTime = 0.0;
        double avgMemory = 0.0;
        
        for (const auto& result : results) {
            avgStepsPerSecond += result.stepsPerSecond;
            avgTime += result.totalTime;
            avgMemory += result.peakMemoryUsage;
        }
        
        if (!results.empty()) {
            avgStepsPerSecond /= results.size();
            avgTime /= results.size();
            avgMemory /= results.size();
            
            std::cout << "Average Performance:\n";
            std::cout << std::left << std::setw(20) << "Average"
                      << std::setw(12) << "-"
                      << std::setw(10) << "-"
                      << std::setw(12) << std::fixed << std::setprecision(0) << avgStepsPerSecond
                      << std::setw(12) << std::fixed << std::setprecision(2) << avgTime
                      << std::setw(12) << avgMemory / 1024
                      << "\n";
        }
    }
    
    const std::vector<PerformanceMetrics>& getResults() const { return results; }
    
    // Save benchmark results to file
    bool saveResults(const std::string& filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filepath << std::endl;
            return false;
        }
        
        file << "NLM Benchmark Results\n";
        file << "Generated: " << getCurrentTimestamp() << "\n\n";
        
        for (size_t i = 0; i < results.size(); ++i) {
            file << "Benchmark: " << benchmarks[i].name << "\n";
            file << "Description: " << benchmarks[i].description << "\n";
            file << "Neurons: " << benchmarks[i].neuronCount << "\n";
            file << "Steps: " << benchmarks[i].steps << "\n";
            file << "Timestep: " << benchmarks[i].timestep << "\n";
            file << "Total Time: " << std::fixed << std::setprecision(3) << results[i].totalTime << "s\n";
            file << "Steps/sec: " << std::fixed << std::setprecision(1) << results[i].stepsPerSecond << "\n";
            file << "Memory Peak: " << results[i].peakMemoryUsage / 1024 << "KB\n";
            file << "Learning Rate: " << std::fixed << std::setprecision(4) << results[i].learningRate << "\n";
            file << "Stability Score: " << std::fixed << std::setprecision(3) << results[i].stabilityScore << "\n";
            file << "---\n\n";
        }
        
        file.close();
        return true;
    }
    
private:
    std::string getCurrentTimestamp() const {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// Visualization data collector
class VisualizationDataCollector {
private:
    struct VisualizationData {
        std::vector<double> neuronActivities;
        std::vector<double> spikeTimes;
        std::vector<std::vector<double>> synapticWeights;
        std::vector<double> membranePotentials;
        std::vector<double> learningSignals;
        std::chrono::system_clock::time_point timestamp;
        std::string recordingMode;
    };
    
    std::vector<VisualizationData> recordedData;
    std::string outputDirectory;
    
public:
    VisualizationDataCollector() : outputDirectory("./visualization") {}
    
    void setOutputDirectory(const std::string& directory) {
        outputDirectory = directory;
    }
    
    void recordStep(const std::vector<double>& neuronActivities,
                   const std::vector<double>& spikeTimes,
                   const std::vector<std::vector<double>>& synapticWeights,
                   const std::vector<double>& membranePotentials,
                   const std::vector<double>& learningSignals,
                   const std::string& mode = "standard") {
        VisualizationData data;
        data.neuronActivities = neuronActivities;
        data.spikeTimes = spikeTimes;
        data.synapticWeights = synapticWeights;
        data.membranePotentials = membranePotentials;
        data.learningSignals = learningSignals;
        data.timestamp = std::chrono::system_clock::now();
        data.recordingMode = mode;
        
        recordedData.push_back(data);
    }
    
    void saveData(const std::string& filename = "visualization_data.json") const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "{\n";
        file << "  \"data_points\": " << recordedData.size() << ",\n";
        file << "  \"recording_mode\": \"" << recordedData.empty() ? "unknown" : recordedData[0].recordingMode << "\",\n";
        file << "  \"data\": [\n";
        
        for (size_t i = 0; i < recordedData.size(); ++i) {
            file << "    {\n";
            file << "      \"timestamp\": \"" << getTimestampString(recordedData[i].timestamp) << "\",\n";
            file << "      \"neuron_activities\": [";
            for (size_t j = 0; j < recordedData[i].neuronActivities.size(); ++j) {
                file << recordedData[i].neuronActivities[j];
                if (j < recordedData[i].neuronActivities.size() - 1) {
                    file << ", ";
                }
            }
            file << "],\n";
            file << "      \"spike_times\": [";
            for (size_t j = 0; j < recordedData[i].spikeTimes.size(); ++j) {
                file << recordedData[i].spikeTimes[j];
                if (j < recordedData[i].spikeTimes.size() - 1) {
                    file << ", ";
                }
            }
            file << "],\n";
            file << "      \"membrane_potentials\": [";
            for (size_t j = 0; j < recordedData[i].membranePotentials.size(); ++j) {
                file << recordedData[i].membranePotentials[j];
                if (j < recordedData[i].membranePotentials.size() - 1) {
                    file << ", ";
                }
            }
            file << "],\n";
            file << "      \"learning_signals\": [";
            for (size_t j = 0; j < recordedData[i].learningSignals.size(); ++j) {
                file << recordedData[i].learningSignals[j];
                if (j < recordedData[i].learningSignals.size() - 1) {
                    file << ", ";
                }
            }
            file << "],\n";
            file << "      \"synaptic_weights\": [";
            for (size_t j = 0; j < recordedData[i].synapticWeights.size(); ++j) {
                file << "[";
                for (size_t k = 0; k < recordedData[i].synapticWeights[j].size(); ++k) {
                    file << recordedData[i].synapticWeights[j][k];
                    if (k < recordedData[i].synapticWeights[j].size() - 1) {
                        file << ", ";
                    }
                }
                file << "]";
                if (j < recordedData[i].synapticWeights.size() - 1) {
                    file << ", ";
                }
            }
            file << "]\n";
            file << "    }";
            if (i < recordedData.size() - 1) {
                file << ",\n";
            } else {
                file << "\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    void saveCSV(const std::string& filename = "visualization_data.csv") const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return;
        }
        
        // Write header
        file << "timestamp,neuron_0,neuron_1,neuron_2,neuron_3,neuron_4,spike_count,membrane_mean,learning_mean\n";
        
        for (const auto& data : recordedData) {
            file << getTimestampString(data.timestamp) << ",";
            
            // Neuron activities
            for (size_t i = 0; i < data.neuronActivities.size(); ++i) {
                file << data.neuronActivities[i];
                if (i < data.neuronActivities.size() - 1) {
                    file << ",";
                }
            }
            file << ",";
            
            // Spike count
            file << data.spikeTimes.size() << ",";
            
            // Average membrane potential
            double avgMembrane = 0.0;
            if (!data.membranePotentials.empty()) {
                for (double v : data.membranePotentials) avgMembrane += v;
                avgMembrane /= data.membranePotentials.size();
            }
            file << avgMembrane << ",";
            
            // Average learning signal
            double avgLearning = 0.0;
            if (!data.learningSignals.empty()) {
                for (double v : data.learningSignals) avgLearning += v;
                avgLearning /= data.learningSignals.size();
            }
            file << avgLearning << "\n";
        }
        
        file.close();
    }
    
    void savePythonPlot(const std::string& filename = "plot_data.py") const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to open file for writing: " << filename << std::endl;
            return;
        }
        
        file << "import matplotlib.pyplot as plt\n";
        file << "import numpy as np\n\n";
        
        // Generate time data
        file << "# Generate time points\n";
        file << "time = np.linspace(0, " << recordedData.size() << ".0, " << recordedData.size() << ")\n\n";
        
        // Plot spike count
        file << "# Plot spike count\n";
        file << "spike_counts = [";
        for (size_t i = 0; i < recordedData.size(); ++i) {
            file << recordedData[i].spikeTimes.size();
            if (i < recordedData.size() - 1) {
                file << ", ";
            }
        }
        file << "]\n\n";
        
        file << "fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(10, 12))\n\n";
        
        // Subplot 1: Spike count
        file << "ax1.plot(time, spike_counts, 'b-', linewidth=2, label='Spike Count')\n";
        file << "ax1.set_ylabel('Spike Count')\n";
        file << "ax1.set_title('NLM Visualization Data')\n";
        file << "ax1.legend()\n";
        file << "ax1.grid(True, alpha=0.3)\n\n";
        
        // Subplot 2: Learning signals
        file << "learning_means = []\n";
        file << "for data in recorded_data:\n";
        file << "    if data['learning_signals']:\n";
        file << "        learning_means.append(sum(data['learning_signals']) / len(data['learning_signals']))\n";
        file << "    else:\n";
        file << "        learning_means.append(0.0)\n\n";
        
        file << "ax2.plot(time, learning_means, 'r-', linewidth=2, label='Learning Signal')\n";
        file << "ax2.set_ylabel('Learning Signal')\n";
        file << "ax2.legend()\n";
        file << "ax2.grid(True, alpha=0.3)\n\n";
        
        // Subplot 3: Membrane potential
        file << "membrane_means = []\n";
        file << "for data in recorded_data:\n";
        file << "    if data['membrane_potentials']:\n";
        file << "        membrane_means.append(sum(data['membrane_potentials']) / len(data['membrane_potentials']))\n";
        file << "    else:\n";
        file << "        membrane_means.append(0.0)\n\n";
        
        file << "ax3.plot(time, membrane_means, 'g-', linewidth=2, label='Membrane Potential')\n";
        file << "ax3.set_ylabel('Membrane Potential')\n";
        file << "ax3.set_xlabel('Time Step')\n";
        file << "ax3.legend()\n";
        file << "ax3.grid(True, alpha=0.3)\n\n";
        
        file << "plt.tight_layout()\n";
        file << "plt.savefig('nlm_visualization.png', dpi=150)\n";
        file << "print('Visualization plot saved to nlm_visualization.png')\n";
        
        file.close();
    }
    
    const std::vector<VisualizationData>& getRecordedData() const { return recordedData; }
    size_t getDataPoints() const { return recordedData.size(); }
    
    void clear() {
        recordedData.clear();
    }
    
private:
    std::string getTimestampString(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
};

// Main benchmark application
class BenchmarkApplication {
private:
    Benchmarker benchmarker;
    VisualizationDataCollector visualizer;
    
public:
    BenchmarkApplication() {
        // Configure visualization output
        visualizer.setOutputDirectory("./benchmark_visualization");
    }
    
    void runFullBenchmark() {
        std::cout << "========================================\n";
        std::cout << "NLM Full Benchmark Suite\n";
        std::cout << "========================================\n\n";
        
        // Run benchmarks
        benchmarker.runAllBenchmarks();
        
        // Save results
        benchmarker.saveResults("benchmark_results.json");
        
        std::cout << "\nBenchmark suite completed!\n";
        std::cout << "Results saved to benchmark_results.json\n";
    }
    
    void runPerformanceProfiling(const std::string& mode = "quick") {
        std::cout << "========================================\n";
        std::cout << "NLM Performance Profiling\n";
        std::cout << "========================================\n\n";
        
        if (mode == "quick") {
            std::cout << "Running quick performance profile...\n";
            // Create a quick benchmark
            Benchmarker::BenchmarkConfig config;
            config.name = "Quick Profile";
            config.description = "Quick performance profile";
            config.neuronCount = 100;
            config.steps = 500;
            config.timestep = 0.001;
            
            benchmarker.runAllBenchmarks();
            benchmarker.saveResults("quick_profile_results.json");
            
        } else if (mode == "thorough") {
            std::cout << "Running thorough performance profile...\n";
            benchmarker.runAllBenchmarks();
            benchmarker.saveResults("thorough_profile_results.json");
            
        } else if (mode == "stress") {
            std::cout << "Running stress test...\n";
            benchmarker.addBenchmark("Stress Test", "Stress test with large network", 5000, 100);
            benchmarker.runAllBenchmarks();
            benchmarker.saveResults("stress_test_results.json");
        }
        
        std::cout << "\nPerformance profiling completed!\n";
    }
    
    void generateVisualizationData(const std::string& mode = "quick") {
        std::cout << "========================================\n";
        std::cout << "Generating Visualization Data\n";
        std::cout << "========================================\n\n";
        
        // Simulate some data collection
        for (int i = 0; i < 20; ++i) {
            // Create sample data
            std::vector<double> neuronActivities(10, 0.1 * (i % 3));
            std::vector<double> spikeTimes;
            std::vector<double> synapticWeights(10, 0.5);
            std::vector<double> membranePotentials(10, 0.05 * (i % 4));
            std::vector<double> learningSignals(10, 0.2 * (i % 2));
            
            // Add some spike times for realism
            if (i % 3 == 0) {
                spikeTimes.push_back(i * 0.1);
                spikeTimes.push_back(i * 0.1 + 0.5);
            }
            
            visualizer.recordStep(neuronActivities, spikeTimes, 
                                 std::vector<std::vector<double>>(10, synapticWeights), 
                                 membranePotentials, learningSignals, mode);
        }
        
        // Save visualization data
        visualizer.saveData("visualization_data.json");
        visualizer.saveCSV("visualization_data.csv");
        visualizer.savePythonPlot("plot_data.py");
        
        std::cout << "Visualization data saved:\n";
        std::cout << "  - visualization_data.json\n";
        std::cout << "  - visualization_data.csv\n";
        std::cout << "  - plot_data.py (for matplotlib)\n";
        std::cout << "\nVisualization generation completed!\n";
    }
};