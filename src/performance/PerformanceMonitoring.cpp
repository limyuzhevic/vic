// Performance Monitoring Implementation
// Implements performance tracking, profiling, and analysis for NLM brain simulations

#include "PerformanceMonitoring.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "core/Logger/Logger.hpp"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

// PerformanceProfiler implementation
PerformanceProfiler::PerformanceProfiler() : running_(false), total_steps_(0), spike_count_(0) {}

void PerformanceProfiler::start() {
    running_ = true;
    start_time_ = std::chrono::high_resolution_clock::now();
    step_times_.clear();
    total_steps_ = 0;
    spike_count_ = 0;
}

void PerformanceProfiler::stop() {
    running_ = false;
    if (start_time_.time_since_epoch().count() > 0) {
        auto end_time = std::chrono::high_resolution_clock::now();
        total_simulation_time_ = std::chrono::duration<double>(end_time - start_time_).count();
    }
}

void PerformanceProfiler::recordStep(size_t steps, size_t spikes) {
    if (running_) {
        auto step_start = std::chrono::high_resolution_clock::now();
        total_steps_ += steps;
        spike_count_ += spikes;
        step_times_.push_back(step_start);
    }
}

void PerformanceProfiler::finalize() {
    if (!step_times_.empty() && total_simulation_time_ > 0) {
        calculateMetrics();
    }
}

void PerformanceProfiler::calculateMetrics() {
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

// BrainPerformanceAnalyzer implementation
BrainPerformanceAnalyzer::BrainStatistics BrainPerformanceAnalyzer::analyze() const {
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
        
        // We'll add more detailed analysis methods if they exist
        // For now, set placeholder values
        stats.workingMemoryActive = 0;
        stats.episodicMemoryEpisodes = 0;
        stats.semanticMemoryConcepts = 0;
        stats.hebbianConnections = 0;
        stats.stdpConnections = 0;
        stats.rewardModulatedConnections = 0;
        stats.averageSynapticWeight = 0.0;
        stats.criticalPeriodActive = 0;
        stats.maturationProgress = 0;
        
        // System statistics
        stats.simulationSteps = 0;
        stats.simulationTime = 0.0;
        stats.memoryUsageMB = 0.0;
    }
    
    return stats;
}

// SimulationProfiler implementation
SimulationProfiler::BenchmarkResult SimulationProfiler::benchmark(
    std::shared_ptr<Brain> brain, 
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

std::string SimulationProfiler::BenchmarkResult::toString() const {
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(3);
    oss << "Benchmark Result: " << configuration_name << "\n";
    oss << "  Description: " << description << "\n";
    oss << "  Total time: " << total_time << "s\n";
    oss << "  Average step time: " << avg_step_time << "s\n";
    oss << "  Steps processed: " << steps_processed << "\n";
    oss << "  Spikes generated: " << spikes_generated << "\n";
    oss << "  Spikes per second: " << spikes_per_second << "\n";
    oss << "  Memory efficiency: " << memory_efficiency << "\n";
    
    oss << "  Additional metrics:\n";
    for (const auto& metric : metrics) {
        oss << "    " << metric.first << ": " << metric.second << "\n";
    }
    
    return oss.str();
}

// Calculate memory efficiency
std::atomic<size_t> SimulationProfiler::total_allocated_memory_{0};
std::atomic<size_t> SimulationProfiler::peak_memory_usage_{0};

void SimulationProfiler::allocateMemory(size_t size) {
    total_allocated_memory_ += size;
    peak_memory_usage_ = std::max(peak_memory_usage_.load(), total_allocated_memory_.load());
}

void SimulationProfiler::deallocateMemory(size_t size) {
    total_allocated_memory_ -= size;
}

double SimulationProfiler::getCurrentMemoryUsage() {
    return static_cast<double>(total_allocated_memory_.load());
}

double SimulationProfiler::getPeakMemoryUsage() {
    return static_cast<double>(peak_memory_usage_.load());
}

} // namespace nlm
