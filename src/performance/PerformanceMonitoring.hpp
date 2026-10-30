// Performance Monitoring and Statistics System
// Provides tools for profiling brain performance, monitoring system resources,
// and collecting statistics for analysis and optimization.

#ifndef NLM_PERFORMANCE_MONITORING_H
#define NLM_PERFORMANCE_MONITORING_H

#include <chrono>
#include <vector>
#include <string>
#include <map>
#include <memory>
#include <thread>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <atomic>

namespace nlm {

// Time measurement utilities
class Timer {
public:
    Timer() : start_time_(std::chrono::high_resolution_clock::now()) {}
    
    double elapsed() const {
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time_);
        return duration.count() / 1000000.0; // Convert to seconds
    }
    
    void reset() {
        start_time_ = std::chrono::high_resolution_clock::now();
    }
    
private:
    std::chrono::high_resolution_clock::time_point start_time_;
};

// Performance metrics collection
struct PerformanceMetrics {
    // Timing metrics
    double totalSimulationTime = 0.0;
    double averageStepTime = 0.0;
    double maxStepTime = 0.0;
    double minStepTime = 0.0;
    
    // Activity metrics
    size_t totalSteps = 0;
    size_t totalSpikes = 0;
    size_t totalConnections = 0;
    size_t totalMemoryUsage = 0;
    
    // Learning metrics
    double totalLearningStrength = 0.0;
    size_t plasticSynapses = 0;
    size_t structuralChanges = 0;
    
    // Efficiency metrics
    double spikesPerSecond = 0.0;
    double connectionsPerStep = 0.0;
    double memoryPerNeuron = 0.0;
    double learningEfficiency = 0.0;
};

// Performance profiler for brain simulation
class PerformanceProfiler {
public:
    PerformanceProfiler() : running_(false), total_steps_(0), spike_count_(0) {}
    
    void start() {
        running_ = true;
        start_time_ = std::chrono::high_resolution_clock::now();
        step_times_.clear();
        total_steps_ = 0;
        spike_count_ = 0;
    }
    
    void stop() {
        running_ = false;
        if (start_time_.time_since_epoch().count() > 0) {
            auto end_time = std::chrono::high_resolution_clock::now();
            total_simulation_time_ = std::chrono::duration<double>(end_time - start_time_).count();
        }
    }
    
    void recordStep(size_t steps, size_t spikes) {
        if (running_) {
            auto step_start = std::chrono::high_resolution_clock::now();
            total_steps_ += steps;
            spike_count_ += spikes;
            step_times_.push_back(step_start);
        }
    }
    
    void finalize() {
        if (!step_times_.empty() && total_simulation_time_ > 0) {
            calculateMetrics();
        }
    }
    
    const PerformanceMetrics& getMetrics() const { return metrics_; }
    
    void printReport() const {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "=== Performance Profile Report ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Timing Metrics:" << std::endl;
        std::cout << "  Total simulation time: " << metrics_.totalSimulationTime << "s" << std::endl;
        std::cout << "  Average step time: " << metrics_.averageStepTime << "s" << std::endl;
        std::cout << "  Max step time: " << metrics_.maxStepTime << "s" << std::endl;
        std::cout << "  Min step time: " << metrics_.minStepTime << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Activity Metrics:" << std::endl;
        std::cout << "  Total steps processed: " << metrics_.totalSteps << std::endl;
        std::cout << "  Total spikes generated: " << metrics_.totalSpikes << std::endl;
        std::cout << "  Total connections: " << metrics_.totalConnections << std::endl;
        std::cout << "  Memory usage: " << metrics_.totalMemoryUsage << " bytes" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Learning Metrics:" << std::endl;
        std::cout << "  Total learning strength: " << metrics_.totalLearningStrength << std::endl;
        std::cout << "  Plastic synapses: " << metrics_.plasticSynapses << std::endl;
        std::cout << "  Structural changes: " << metrics_.structuralChanges << std::endl;
        std::cout << "  Learning efficiency: " << metrics_.learningEfficiency << std::endl;
        std::cout << std::endl;
        
        std::cout << "Efficiency Metrics:" << std::endl;
        std::cout << "  Spikes per second: " << metrics_.spikesPerSecond << std::endl;
        std::cout << "  Connections per step: " << metrics_.connectionsPerStep << std::endl;
        std::cout << "  Memory per neuron: " << metrics_.memoryPerNeuron << " bytes" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== End Performance Profile ===" << std::endl;
    }
    
    std::string getFormattedMetrics() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << "SimulationTime: " << metrics_.totalSimulationTime << "s\n";
        oss << "AvgStepTime: " << metrics_.averageStepTime << "s\n";
        oss << "SpikesPerSecond: " << metrics_.spikesPerSecond << "\n";
        oss << "FiringRate: " << (metrics_.totalSteps > 0 ? metrics_.totalSpikes / metrics_.totalSteps : 0.0) << " Hz\n";
        oss << "ConnectionsPerStep: " << metrics_.connectionsPerStep << "\n";
        return oss.str();
    }
    
private:
    void calculateMetrics() {
        if (step_times_.size() < 2) {
            metrics_.totalSimulationTime = total_simulation_time_;
            metrics_.averageStepTime = 0.0;
            metrics_.maxStepTime = 0.0;
            metrics_.minStepTime = 0.0;
        } else {
            // Calculate step times based on recorded timestamps
            std::vector<double> times;
            auto it = step_times_.begin();
            auto end = step_times_.end();
            
            auto prev = *it++;
            for (; it != end; ++it) {
                auto duration = std::chrono::duration_cast<std::chrono::microseconds>(*it - prev).count() / 1000000.0;
                times.push_back(duration);
                prev = *it;
            }
            
            // Calculate timing statistics
            if (!times.empty()) {
                metrics_.totalSimulationTime = total_simulation_time_;
                metrics_.minStepTime = *std::min_element(times.begin(), times.end());
                metrics_.maxStepTime = *std::max_element(times.begin(), times.end());
                double sum = std::accumulate(times.begin(), times.end(), 0.0);
                metrics_.averageStepTime = sum / times.size();
            }
        }
        
        // Calculate derived metrics
        metrics_.totalSteps = total_steps_;
        metrics_.totalSpikes = spike_count_;
        
        if (metrics_.totalSimulationTime > 0) {
            metrics_.spikesPerSecond = static_cast<double>(metrics_.totalSpikes) / metrics_.totalSimulationTime;
            metrics_.connectionsPerStep = static_cast<double>(metrics_.totalConnections) / metrics_.totalSteps;
        }
        
        if (metrics_.totalSteps > 0) {
            metrics_.memoryPerNeuron = static_cast<double>(metrics_.totalMemoryUsage) / metrics_.totalSteps;
        }
    }
    
    bool running_ = false;
    std::chrono::high_resolution_clock::time_point start_time_;
    double total_simulation_time_ = 0.0;
    std::vector<std::chrono::high_resolution_clock::time_point> step_times_;
    std::atomic<size_t> total_steps_{0};
    std::atomic<size_t> spike_count_{0};
    PerformanceMetrics metrics_;
};

// Brain performance analyzer
class BrainPerformanceAnalyzer {
public:
    BrainPerformanceAnalyzer(std::shared_ptr<Brain> brain) : brain_(brain) {}
    
    struct BrainStatistics {
        // Structural statistics
        size_t neuronCount = 0;
        size_t synapseCount = 0;
        size_t regionCount = 0;
        size_t populationCount = 0;
        
        // Activity statistics
        size_t currentFiring = 0;
        size_t totalSpikes = 0;
        double averageFiringRate = 0.0;
        double excitationInhibitionRatio = 0.0;
        
        // Developmental statistics
        double developmentalStage = 0.0;
        size_t criticalPeriodActive = 0;
        size_t maturationProgress = 0;
        
        // Learning statistics
        size_t hebbianConnections = 0;
        size_t stdpConnections = 0;
        size_t rewardModulatedConnections = 0;
        double averageSynapticWeight = 0.0;
        
        // Memory statistics
        size_t workingMemoryActive = 0;
        size_t episodicMemoryEpisodes = 0;
        size_t semanticMemoryConcepts = 0;
        
        // System statistics
        double memoryUsageMB = 0.0;
        size_t simulationSteps = 0;
        double simulationTime = 0.0;
    };
    
    BrainStatistics analyze() const {
        BrainStatistics stats;
        
        if (brain_) {
            // Structural statistics
            stats.neuronCount = brain_->getTotalNeuronCount();
            stats.synapseCount = brain_->getTotalSynapseCount();
            stats.regionCount = brain_->getRegionCount();
            stats.populationCount = brain_->getPopulationCount();
            
            // Activity statistics
            stats.currentFiring = brain_->getFiringNeuronCount();
            stats.totalSpikes = brain_->getTotalSpikeCount();
            stats.averageFiringRate = brain_->getAverageFiringRate();
            stats.excitationInhibitionRatio = brain_->getExcitationInhibitionRatio();
            
            // Developmental statistics
            stats.developmentalStage = brain_->getDevelopmentalStage();
            
            // We can add more detailed analysis here
            // This would involve iterating through regions, neurons, synapses
        }
        
        return stats;
    }
    
    void printBrainStatistics(const BrainStatistics& stats) const {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "=== Brain Performance Analysis ===" << std::endl;
        std::cout << std::endl;
        
        std::cout << "Structural Properties:" << std::endl;
        std::cout << "  Neurons: " << stats.neuronCount << std::endl;
        std::cout << "  Synapses: " << stats.synapseCount << std::endl;
        std::cout << "  Regions: " << stats.regionCount << std::endl;
        std::cout << "  Populations: " << stats.populationCount << std::endl;
        std::cout << std::endl;
        
        std::cout << "Activity Metrics:" << std::endl;
        std::cout << "  Currently firing: " << stats.currentFiring << std::endl;
        std::cout << "  Total spikes: " << stats.totalSpikes << std::endl;
        std::cout << "  Average firing rate: " << stats.averageFiringRate << " Hz" << std::endl;
        std::cout << "  E/I ratio: " << stats.excitationInhibitionRatio << std::endl;
        std::cout << std::endl;
        
        std::cout << "Developmental State:" << std::endl;
        std::cout << "  Stage: " << stats.developmentalStage << std::endl;
        std::cout << std::endl;
        
        std::cout << "Learning State:" << std::endl;
        std::cout << "  Hebbian connections: " << stats.hebbianConnections << std::endl;
        std::cout << "  STDP connections: " << stats.stdpConnections << std::endl;
        std::cout << "  Reward-modulated connections: " << stats.rewardModulatedConnections << std::endl;
        std::cout << "  Average synaptic weight: " << stats.averageSynapticWeight << std::endl;
        std::cout << std::endl;
        
        std::cout << "Memory State:" << std::endl;
        std::cout << "  Working memory active: " << stats.workingMemoryActive << std::endl;
        std::cout << "  Episodic memory episodes: " << stats.episodicMemoryEpisodes << std::endl;
        std::cout << "  Semantic memory concepts: " << stats.semanticMemoryConcepts << std::endl;
        std::cout << std::endl;
        
        std::cout << "System Metrics:" << std::endl;
        std::cout << "  Memory usage: " << stats.memoryUsageMB << " MB" << std::endl;
        std::cout << "  Simulation steps: " << stats.simulationSteps << std::endl;
        std::cout << "  Simulation time: " << stats.simulationTime << "s" << std::endl;
        std::cout << std::endl;
        
        std::cout << "=== End Analysis ===" << std::endl;
    }
    
private:
    std::shared_ptr<Brain> brain_;
};

// Simulation profiler for benchmarking different configurations
class SimulationProfiler {
public:
    SimulationProfiler() = default;
    
    struct BenchmarkResult {
        std::string configuration_name;
        std::string description;
        double total_time;
        double avg_step_time;
        size_t steps_processed;
        size_t spikes_generated;
        double spikes_per_second;
        double memory_efficiency;
        std::map<std::string, double> metrics;
    };
    
    BenchmarkResult benchmark(std::shared_ptr<Brain> brain, 
                              const std::string& config_name,
                              const std::string& description,
                              size_t steps) {
        BenchmarkResult result;
        result.configuration_name = config_name;
        result.description = description;
        
        // Run benchmark
        Timer timer;
        PerformanceProfiler profiler;
        profiler.start();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
            profiler.recordStep(1, brain->getFiringNeuronCount());
        }
        
        profiler.stop();
        profiler.finalize();
        
        const auto& metrics = profiler.getMetrics();
        
        // Fill result
        result.total_time = timer.elapsed();
        result.avg_step_time = metrics.averageStepTime;
        result.steps_processed = steps;
        result.spikes_generated = metrics.totalSpikes;
        result.spikes_per_second = metrics.spikesPerSecond;
        result.memory_efficiency = calculateMemoryEfficiency(brain);
        
        // Add custom metrics
        result.metrics["total_spikes"] = static_cast<double>(metrics.totalSpikes);
        result.metrics["firing_rate"] = metrics.spikesPerSecond;
        result.metrics["avg_step_time"] = metrics.averageStepTime;
        result.metrics["memory_per_neuron"] = metrics.memoryPerNeuron;
        
        return result;
    }
    
    void printBenchmarkResults(const std::vector<BenchmarkResult>& results) const {
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "=== Simulation Benchmark Results ===" << std::endl;
        std::cout << std::endl;
        
        // Print header
        std::cout << std::setw(30) << std::left << "Configuration";
        std::cout << std::setw(25) << std::left << "Description";
        std::cout << std::setw(12) << std::left << "Total Time";
        std::cout << std::setw(12) << std::left << "Avg Step";
        std::cout << std::setw(10) << std::left << "Steps";
        std::cout << std::setw(12) << std::left << "Spikes/sec";
        std::cout << std::setw(15) << std::left << "Memory Eff";
        std::cout << std::endl;
        
        std::cout << std::string(120, '-') << std::endl;
        
        // Print results
        for (const auto& result : results) {
            std::cout << std::setw(30) << std::left << result.configuration_name;
            std::cout << std::setw(25) << std::left << result.description;
            std::cout << std::setw(12) << std::left << result.total_time;
            std::cout << std::setw(12) << std::left << result.avg_step_time;
            std::cout << std::setw(10) << std::left << result.steps_processed;
            std::cout << std::setw(12) << std::left << result.spikes_per_second;
            std::cout << std::setw(15) << std::left << result.memory_efficiency;
            std::cout << std::endl;
        }
        
        std::cout << std::string(120, '-') << std::endl;
        std::cout << std::endl;
        std::cout << "=== End Benchmark ===" << std::endl;
    }
    
private:
    double calculateMemoryEfficiency(std::shared_ptr<Brain> brain) const {
        // Simple memory efficiency calculation
        // This would be more sophisticated in a real implementation
        size_t neurons = brain->getTotalNeuronCount();
        size_t synapses = brain->getTotalSynapseCount();
        
        if (neurons == 0) return 0.0;
        
        // Estimate memory usage based on structure
        // In reality, this would use actual memory profiling
        double neuronMemory = neurons * sizeof(NeuronState); // Rough estimate
        double synapseMemory = synapses * sizeof(SynapseData); // Rough estimate
        
        return neuronMemory + synapseMemory; // Lower is better (more efficient)
    }
};

// Utility functions for performance analysis
namespace performance {
    
    // Calculate theoretical maximum firing rate
    double calculateMaxFiringRate(size_t neuron_count, double timestep) {
        return 1000.0 / (timestep * neuron_count); // Neurons per second
    }
    
    // Calculate energy efficiency (rough estimate)
    double calculateEnergyEfficiency(size_t spikes, double time, size_t neurons) {
        if (time <= 0 || neurons == 0) return 0.0;
        return static_cast<double>(spikes) / (time * neurons); // Spikes per neuron-second
    }
    
    // Format performance metrics for output
    std::string formatMetrics(const PerformanceMetrics& metrics) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(3);
        oss << "Performance: " << metrics.spikesPerSecond << " Hz, "
            << metrics.averageStepTime << "s/step, "
            << metrics.connectionsPerStep << " conn/step";
        return oss.str();
    }
}

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_H
