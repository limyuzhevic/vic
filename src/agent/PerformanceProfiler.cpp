// PerformanceProfiler.cpp - Implementation of performance profiling tools
// Provides performance monitoring, bottleneck analysis, and optimization suggestions

#include "agent/PerformanceProfiler.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "agent/AgentBrain.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <sstream>

namespace nlm {

struct PerformanceProfiler::Impl {
    bool profiling;
    size_t profilingInterval;
    bool detailedAnalysis;
    std::chrono::steady_clock::time_point startTime;
    std::vector<double> firingRates;
    std::vector<size_t> spikeCounts;
    std::vector<double> memoryUsages;
    std::vector<std::chrono::steady_clock::time_point> timestamps;
    
    Impl() : profiling(false), profilingInterval(100), detailedAnalysis(false) {}
};

PerformanceProfiler::PerformanceProfiler() : pImpl(std::make_unique<Impl>()) {}

PerformanceProfiler::~PerformanceProfiler() = default;

void PerformanceProfiler::startProfiling() {
    pImpl->profiling = true;
    pImpl->startTime = std::chrono::steady_clock::now();
    pImpl->firingRates.clear();
    pImpl->spikeCounts.clear();
    pImpl->memoryUsages.clear();
    pImpl->timestamps.clear();
    
    NLM_LOG_INFO("Performance profiling started");
}

void PerformanceProfiler::stopProfiling() {
    pImpl->profiling = false;
    
    auto endTime = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - pImpl->startTime);
    
    NLM_LOG_INFO(std::string("Performance profiling stopped. Duration: ") + 
                std::to_string(duration.count()) + std::string(" seconds"));
}

std::map<std::string, double> PerformanceProfiler::getProfileResults() const {
    std::map<std::string, double> results;
    
    if (pImpl->firingRates.empty()) {
        return results;
    }
    
    // Basic statistics
    double totalTime = pImpl->timestamps.empty() ? 0.0 : 
        std::chrono::duration_cast<std::chrono::milliseconds>(
            pImpl->timestamps.back() - pImpl->timestamps.front()
        ).count() / 1000.0;
    
    if (totalTime == 0.0) return results;
    
    // Calculate statistics
    double avgFiringRate = std::accumulate(pImpl->firingRates.begin(), 
                                          pImpl->firingRates.end(), 0.0) / pImpl->firingRates.size();
    
    double maxFiringRate = *std::max_element(pImpl->firingRates.begin(), pImpl->firingRates.end());
    double minFiringRate = *std::min_element(pImpl->firingRates.begin(), pImpl->firingRates.end());
    
    size_t totalSpikes = std::accumulate(pImpl->spikeCounts.begin(), 
                                        pImpl->spikeCounts.end(), 0ULL);
    
    double avgMemoryUsage = pImpl->memoryUsages.empty() ? 0.0 : 
        std::accumulate(pImpl->memoryUsages.begin(), pImpl->memoryUsages.end(), 0.0) / 
        pImpl->memoryUsages.size();
    
    // Performance metrics
    results["avg_firing_rate"] = avgFiringRate;
    results["max_firing_rate"] = maxFiringRate;
    results["min_firing_rate"] = minFiringRate;
    results["total_spikes"] = static_cast<double>(totalSpikes);
    results["avg_memory_usage"] = avgMemoryUsage;
    results["simulation_time_seconds"] = totalTime;
    results["steps_per_second"] = pImpl->spikeCounts.size() / totalTime;
    
    if (pImpl->detailedAnalysis) {
        // Advanced metrics
        double firingRateStdDev = 0.0;
        if (pImpl->firingRates.size() > 1) {
            double mean = avgFiringRate;
            double sumSq = 0.0;
            for (double rate : pImpl->firingRates) {
                sumSq += (rate - mean) * (rate - mean);
            }
            firingRateStdDev = std::sqrt(sumSq / (pImpl->firingRates.size() - 1));
        }
        
        results["firing_rate_std_dev"] = firingRateStdDev;
        results["firing_rate_cv"] = avgFiringRate > 0.0 ? firingRateStdDev / avgFiringRate : 0.0;
    }
    
    return results;
}

std::string PerformanceProfiler::getBottleneckAnalysis() const {
    std::stringstream ss;
    
    if (pImpl->firingRates.empty()) {
        return "No profiling data available.";
    }
    
    double avgFiringRate = std::accumulate(pImpl->firingRates.begin(), 
                                          pImpl->firingRates.end(), 0.0) / pImpl->firingRates.size();
    
    ss << "Performance Analysis:" << std::endl;
    ss << "  Average firing rate: " << avgFiringRate << " Hz" << std::endl;
    
    if (avgFiringRate > 50.0) {
        ss << "  BOTTLENECK: High firing rate (>50 Hz) may indicate excessive excitation." << std::endl;
        ss << "  Recommendation: Reduce synaptic strengths or increase inhibition." << std::endl;
    } else if (avgFiringRate < 5.0) {
        ss << "  BOTTLENECK: Low firing rate (<5 Hz) may indicate insufficient excitation." << std::endl;
        ss << "  Recommendation: Increase synaptic strengths or decrease inhibition." << std::endl;
    } else {
        ss << "  OPTIMAL: Firing rate within normal range (5-50 Hz)." << std::endl;
    }
    
    // Check for spikes in firing rate
    bool hasSpikes = false;
    for (size_t i = 1; i < pImpl->firingRates.size(); ++i) {
        if (std::abs(pImpl->firingRates[i] - pImpl->firingRates[i-1]) > 10.0) {
            hasSpikes = true;
            break;
        }
    }
    
    if (hasSpikes) {
        ss << "  BOTTLENECK: Large fluctuations in firing rate detected." << std::endl;
        ss << "  Recommendation: Investigate plasticity parameters or input patterns." << std::endl;
    }
    
    return ss.str();
}

std::map<std::string, double> PerformanceProfiler::profileSimulation(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    size_t steps) {
    PerformanceProfiler profiler;
    profiler.startProfiling();
    
    // Run simulation
    for (size_t i = 0; i < steps; ++i) {
        // Get observation from world
        const SensoryPercept& percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(i);
        
        // Get motor command
        MotorCommand cmd = agent->decodeMotorCommand();
        
        // Apply action to world
        world->applyMotorCommand(cmd, world->getSimulationTime());
        
        // Record performance metrics
        profiler.pImpl->firingRates.push_back(brain->getAverageFiringRate());
        profiler.pImpl->spikeCounts.push_back(brain->getTotalSpikeCount());
        
        // Record timestamp
        profiler.pImpl->timestamps.push_back(std::chrono::steady_clock::now());
        
        // Update world
        world->update(0.1); // Fixed timestep for profiling
    }
    
    profiler.stopProfiling();
    
    return profiler.getProfileResults();
}

std::map<std::string, double> PerformanceProfiler::optimizeConfiguration(
    std::shared_ptr<Config> config) {
    std::map<std::string, double> optimizations;
    
    // Check common optimization opportunities
    auto neuronCount = config->getOr<int64_t>("neuron_count", 1000);
    auto connectionProbability = config->getOr<float>("connection_probability", 0.05);
    auto stdpLearningRate = config->getOr<float>("stdp_ltp_weight", 0.01f);
    
    optimizations["neuron_count"] = neuronCount;
    optimizations["connection_probability"] = connectionProbability;
    optimizations["stdp_learning_rate"] = stdpLearningRate;
    
    // Suggest optimizations based on current configuration
    if (neuronCount > 5000) {
        optimizations["suggested_neuron_count"] = 3000.0;  // Reduce for better performance
    }
    
    if (connectionProbability > 0.2f) {
        optimizations["suggested_connection_probability"] = 0.1f;  // Reduce for stability
    }
    
    if (stdpLearningRate > 0.05f) {
        optimizations["suggested_stdp_learning_rate"] = 0.01f;  // Reduce for stability
    }
    
    return optimizations;
}

std::map<std::string, double> PerformanceProfiler::benchmarkMemoryUsage(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent) {
    std::map<std::string, double> memoryResults;
    
    // This is a simplified benchmark - in a real implementation,
    // you would use system-specific memory measurement tools
    
    // Get brain memory usage (simplified)
    size_t neuronCount = brain->getTotalNeuronCount();
    size_t synapseCount = brain->getTotalSynapseCount();
    
    // Estimate memory usage (rough approximation)
    double neuronMemory = neuronCount * 100.0;  // ~100 bytes per neuron
    double synapseMemory = synapseCount * 80.0;  // ~80 bytes per synapse
    double totalMemory = neuronMemory + synapseMemory;
    
    memoryResults["estimated_neuron_memory_kb"] = neuronMemory / 1024.0;
    memoryResults["estimated_synapse_memory_kb"] = synapseMemory / 1024.0;
    memoryResults["estimated_total_memory_kb"] = totalMemory / 1024.0;
    memoryResults["neurons_per_kb"] = neuronMemory > 0.0 ? neuronCount / (neuronMemory / 1024.0) : 0.0;
    
    return memoryResults;
}

std::map<std::string, std::string> PerformanceProfiler::analyzePerformanceBottlenecks(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent) {
    std::map<std::string, std::string> bottlenecks;
    
    double firingRate = brain->getAverageFiringRate();
    size_t totalSpikes = brain->getTotalSpikeCount();
    size_t neuronCount = brain->getTotalNeuronCount();
    
    // Analyze firing rate
    if (firingRate > 100.0) {
        bottlenecks["firing_rate"] = "VERY_HIGH - Consider reducing input strengths or increasing inhibition";
    } else if (firingRate > 50.0) {
        bottlenecks["firing_rate"] = "HIGH - Monitor for runaway excitation";
    } else if (firingRate < 1.0) {
        bottlenecks["firing_rate"] = "VERY_LOW - May need stronger inputs or reduced inhibition";
    } else if (firingRate < 5.0) {
        bottlenecks["firing_rate"] = "LOW - Consider enhancing input or plasticity";
    } else {
        bottlenecks["firing_rate"] = "OPTIMAL - Firing rate in good range (5-50 Hz)";
    }
    
    // Analyze spike count relative to neuron count
    double spikeDensity = totalSpikes / neuronCount;
    if (spikeDensity > 10.0) {
        bottlenecks["spike_density"] = "VERY_HIGH - Many spikes per neuron";
    } else if (spikeDensity > 5.0) {
        bottlenecks["spike_density"] = "HIGH - Many spikes per neuron";
    } else if (spikeDensity < 0.1) {
        bottlenecks["spike_density"] = "VERY_LOW - Few spikes per neuron";
    } else if (spikeDensity < 0.5) {
        bottlenecks["spike_density"] = "LOW - Consider enhancing activity";
    } else {
        bottlenecks["spike_density"] = "GOOD - Reasonable spike density";
    }
    
    return bottlenecks;
}

} // namespace nlm