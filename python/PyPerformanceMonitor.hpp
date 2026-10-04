// Performance monitoring utilities for enhanced NLM Python bindings
// Provides benchmarking and performance measurement capabilities

#pragma once

#include <functional>
#include <chrono>
#include <unordered_map>
#include <string>
#include <sstream>
#include <vector>
#include <memory>

namespace nlm {

class PyPerformanceMonitor {
public:
    struct PerformanceStats {
        double total_time;
        size_t steps_completed;
        double avg_firing_rate;
        size_t active_neurons;
        size_t total_spikes;
        double memory_usage;
        std::chrono::system_clock::time_point start_time;
        std::chrono::system_clock::time_point end_time;
    };
    
    // Measure performance of a simulation function
    static PerformanceStats measurePerformance(
        std::function<void()> simulation_func,
        size_t steps_to_run) {
        
        PerformanceStats stats;
        stats.start_time = std::chrono::system_clock::now();
        
        auto start_cpu = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < steps_to_run; ++i) {
            simulation_func();
        }
        
        auto end_cpu = std::chrono::high_resolution_clock::now();
        stats.end_time = std::chrono::system_clock::now();
        
        stats.total_time = std::chrono::duration<double>(end_cpu - start_cpu).count();
        stats.steps_completed = steps_to_run;
        
        return stats;
    }
    
    // Format performance statistics
    static std::string formatStats(const PerformanceStats& stats) {
        std::stringstream ss;
        ss << "Performance Statistics:\n";
        ss << "  Total time: " << stats.total_time << " seconds\n";
        ss << "  Steps completed: " << stats.steps_completed << "\n";
        ss << "  Steps per second: " << (stats.steps_completed / stats.total_time) << "\n";
        ss << "  Average firing rate: " << stats.avg_firing_rate << " Hz\n";
        ss << "  Active neurons: " << stats.active_neurons << "\n";
        ss << "  Total spikes: " << stats.total_spikes << "\n";
        ss << "  Memory usage: " << stats.memory_usage << " MB\n";
        
        return ss.str();
    }
    
    // Benchmark different brain configurations
    static std::unordered_map<std::string, double> benchmarkBrains(
        const std::vector<std::pair<std::string, std::function<std::shared_ptr<class Brain>()>>>& brain_configs,
        size_t num_steps = 100) {
        
        std::unordered_map<std::string, double> results;
        
        for (const auto& config : brain_configs) {
            auto start = std::chrono::high_resolution_clock::now();
            
            auto brain = config.second();
            brain->initialize();
            
            for (size_t i = 0; i < num_steps; ++i) {
                brain->step(i);
            }
            
            auto end = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double>(end - start).count();
            
            results[config.first] = duration;
        }
        
        return results;
    }
};

} // namespace nlm
