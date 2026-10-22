// src/debug/ConfigurationProfiler.cpp
#include "ConfigurationProfiler.hpp"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <algorithm>
#include <numeric>

namespace nlm {
namespace debug {

ConfigurationProfiler::ConfigurationProfiler(std::shared_ptr<Config> config) : config_(config) {
    session_id_ = generateSessionId();
}

ConfigurationProfiler::~ConfigurationProfiler() {
}

void ConfigurationProfiler::profilePerformance(std::shared_ptr<Brain> brain, const std::string& description) {
    BrainPerformanceMetrics metrics;
    metrics.configuration_id = description.empty() ? "unknown" : description;
    metrics.execution_time_ms = 0.0;  // Would be measured in real usage
    metrics.total_spikes = brain ? brain->getTotalSpikeCount() : 0;
    metrics.total_neurons = brain ? brain->getTotalNeuronCount() : 0;
    metrics.total_synapses = brain ? brain->getTotalSynapseCount() : 0;
    
    // Calculate average firing rate
    if (metrics.total_neurons > 0 && metrics.execution_time_ms > 0) {
        metrics.average_firing_rate = (metrics.total_spikes / metrics.execution_time_ms) * 1000.0;  // Hz
    }
    
    metrics.weight_change_magnitude = 0.0;  // Would be calculated from initial/final weights
    metrics.memory_footprint_bytes = 0;    // Would be measured in real usage
    
    performance_history_.push_back(metrics);
}

void ConfigurationProfiler::profileMemoryUsage(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    // This would be a more detailed memory profiling implementation
    // For now, we'll update the most recent performance metrics
    if (!performance_history_.empty()) {
        auto& last_metrics = performance_history_.back();
        // Estimate memory usage based on brain components
        last_metrics.memory_footprint_bytes = 
            (last_metrics.total_neurons * 64) +           // Neurons (approx 64 bytes each)
            (last_metrics.total_synapses * 32) +         // Synapses (approx 32 bytes each)
            (last_metrics.total_neurons * 8) +           // Connections
            1024;                                        // Overhead
    }
}

void ConfigurationProfiler::profileNeuralDynamics(std::shared_ptr<Brain> brain) {
    if (!brain) return;
    
    if (!performance_history_.empty()) {
        auto& last_metrics = performance_history_.back();
        
        // Calculate weight change magnitude
        if (auto* region = brain->getRegion(RegionId(1))) {
            double total_weight_change = 0.0;
            size_t synapse_count = 0;
            
            for (const auto& syn : region->getSynapses()) {
                double weight = syn->getWeight();
                // Simplified weight change calculation
                total_weight_change += std::abs(weight);
                synapse_count++;
            }
            
            if (synapse_count > 0) {
                last_metrics.weight_change_magnitude = total_weight_change / synapse_count;
            }
        }
    }
}

void ConfigurationProfiler::generateConfigurationReport(const std::string& output_file) const {
    std::ofstream file(output_file);
    if (!file.is_open()) {
        return;
    }
    
    file << "Configuration Profile Report" << std::endl;
    file << "===========================" << std::endl;
    file << "Session ID: " << session_id_ << std::endl;
    file << "Configuration source: " << (config_ ? config_->summary() : "Unknown") << std::endl;
    file << "Number of performance runs: " << performance_history_.size() << std::endl;
    
    if (performance_history_.empty()) {
        file << "\nNo performance data available." << std::endl;
        file.close();
        return;
    }
    
    file << "\nPerformance Summary:" << std::endl;
    file << "====================" << std::endl;
    
    double total_execution_time = 0.0;
    uint64_t total_spikes = 0;
    uint64_t total_neurons = 0;
    uint64_t total_synapses = 0;
    
    for (size_t i = 0; i < performance_history_.size(); ++i) {
        const auto& metrics = performance_history_[i];
        
        file << "\nRun " << (i + 1) << ": " << metrics.configuration_id << std::endl;
        file << "  Execution time: " << metrics.execution_time_ms << " ms" << std::endl;
        file << "  Total spikes: " << metrics.total_spikes << std::endl;
        file << "  Total neurons: " << metrics.total_neurons << std::endl;
        file << "  Total synapses: " << metrics.total_synapses << std::endl;
        file << "  Average firing rate: " << metrics.average_firing_rate << " Hz" << std::endl;
        file << "  Weight change magnitude: " << metrics.weight_change_magnitude << std::endl;
        file << "  Memory footprint: " << metrics.memory_footprint_bytes << " bytes" << std::endl;
        
        total_execution_time += metrics.execution_time_ms;
        total_spikes += metrics.total_spikes;
        total_neurons += metrics.total_neurons;
        total_synapses += metrics.total_synapses;
    }
    
    file << "\nOverall Summary:" << std::endl;
    file << "=================" << std::endl;
    file << "Average execution time: " << (total_execution_time / performance_history_.size()) << " ms" << std::endl;
    file << "Average total spikes: " << (total_spikes / performance_history_.size()) << std::endl;
    file << "Average total neurons: " << (total_neurons / performance_history_.size()) << std::endl;
    file << "Average total synapses: " << (total_synapses / performance_history_.size()) << std::endl;
    
    if (performance_history_.size() > 1) {
        file << "\nPerformance Comparison:" << std::endl;
        file << "=====================" << std::endl;
        
        double min_time = performance_history_[0].execution_time_ms;
        double max_time = performance_history_[0].execution_time_ms;
        
        for (const auto& metrics : performance_history_) {
            min_time = std::min(min_time, metrics.execution_time_ms);
            max_time = std::max(max_time, metrics.execution_time_ms);
        }
        
        file << "Execution time range: " << min_time << " - " << max_time << " ms" << std::endl;
        
        double time_variation = ((max_time - min_time) / min_time) * 100.0;
        file << "Time variation: " << time_variation << "%" << std::endl;
    }
    
    file.close();
}

std::string ConfigurationProfiler::generateSessionId() {
    auto now = std::chrono::high_resolution_clock::now();
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch());
    return "config_profiler_" + std::to_string(now_ms.count());
}

} // namespace debug
} // namespace nlm
