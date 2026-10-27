// Example Basic Plugins for NLM Plugin System
// Demonstrates plugin architecture and extensibility

#ifndef NLM_EXAMPLE_PLUGINS_HPP
#define NLM_EXAMPLE_PLUGINS_HPP

#include "PluginSystem.hpp"
#include <chrono>

namespace nlm {

// Example Plugin: Memory Statistics Plugin
class MemoryStatisticsPlugin : public BasePlugin<MemoryStatisticsPlugin> {
public:
    std::string pluginName() const { return "MemoryStats"; }
    std::string pluginDescription() const { return "Provides memory usage statistics for the brain"; }
    
    bool initialize() override {
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        initialized_ = false;
    }
    
    void configure(const Config& config) override {
        // Read configuration
        refreshInterval_ = config.getOr("memory_stats_refresh_interval", 60.0f);
        trackMemoryUsage_ = config.getOr("memory_stats_track_memory", true);
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"Core"}; // Depends on core NLM functionality
    }
    
    std::vector<std::string> getProvidedServices() const override {
        return {"MemoryStatistics", "MemoryUsage"};
    }
    
    void onBrainInitialize(Brain* brain) override {
        brain_ = brain;
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    void onBrainStep(Brain* brain, size_t step) override {
        if (initialized_ && std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - lastUpdate_).count() > refreshInterval_) {
            updateStatistics();
            lastUpdate_ = std::chrono::high_resolution_clock::now();
        }
    }
    
    std::unordered_map<std::string, double> getStatistics() const override {
        std::unordered_map<std::string, double> stats;
        
        if (brain_) {
            stats["neuron_count"] = static_cast<double>(brain_->getTotalNeuronCount());
            stats["synapse_count"] = static_cast<double>(brain_->getTotalSynapseCount());
            stats["active_neurons"] = static_cast<double>(brain_->getActiveNeuronCount());
            stats["firing_neurons"] = static_cast<double>(brain_->getFiringNeuronCount());
            
            if (startTime_ != std::chrono::high_resolution_clock::time_point{}) {
                auto now = std::chrono::high_resolution_clock::now();
                auto duration = std::chrono::duration<double>(now - startTime_);
                stats["simulation_time_seconds"] = duration.count();
                stats["neurons_per_second"] = brain_->getTotalNeuronCount() / duration.count();
            }
        }
        
        stats["refresh_interval"] = refreshInterval_;
        stats["initialized"] = initialized_ ? 1.0 : 0.0;
        
        return stats;
    }
    
private:
    void updateStatistics() {
        if (brain_) {
            // Update internal statistics
            neuronCount_ = brain_->getTotalNeuronCount();
            synapseCount_ = brain_->getTotalSynapseCount();
            activeNeuronCount_ = brain_->getActiveNeuronCount();
            firingNeuronCount_ = brain_->getFiringNeuronCount();
        }
    }
    
    Brain* brain_ = nullptr;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point lastUpdate_;
    float refreshInterval_ = 60.0f;
    bool trackMemoryUsage_ = true;
    bool initialized_ = false;
    
    // Cached statistics
    size_t neuronCount_ = 0;
    size_t synapseCount_ = 0;
    size_t activeNeuronCount_ = 0;
    size_t firingNeuronCount_ = 0;
};

// Example Plugin: Performance Monitor Plugin
class PerformanceMonitorPlugin : public BasePlugin<PerformanceMonitorPlugin> {
public:
    std::string pluginName() const { return "PerformanceMonitor"; }
    std::string pluginDescription() const { return "Monitors and reports brain performance metrics"; }
    
    bool initialize() override {
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        initialized_ = false;
    }
    
    void configure(const Config& config) override {
        enablePerformanceLogging_ = config.getOr("perf_enable_logging", true);
        enablePerformanceBenchmarks_ = config.getOr("perf_enable_benchmarks", true);
        benchmarkInterval_ = config.getOr("perf_benchmark_interval", 10.0f);
    }
    
    std::vector<std::string> getDependencies() const override {
        return {};
    }
    
    std::vector<std::string> getProvidedServices() const override {
        return {"PerformanceMetrics", "Benchmarking", "PerformanceLogging"};
    }
    
    void onBrainStep(Brain* brain, size_t step) override {
        if (!initialized_) return;
        
        // Collect performance metrics
        collectPerformanceMetrics(brain, step);
        
        // Log performance data if enabled
        if (enablePerformanceLogging_) {
            logPerformanceMetrics(step);
        }
        
        // Run benchmarks if enabled
        if (enablePerformanceBenchmarks_ && 
            std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - lastBenchmark_).count() > benchmarkInterval_) {
            runPerformanceBenchmarks(brain);
            lastBenchmark_ = std::chrono::high_resolution_clock::now();
        }
    }
    
    std::unordered_map<std::string, double> getStatistics() const override {
        std::unordered_map<std::string, double> stats;
        
        stats["enable_performance_logging"] = enablePerformanceLogging_ ? 1.0 : 0.0;
        stats["enable_performance_benchmarks"] = enablePerformanceBenchmarks_ ? 1.0 : 0.0;
        stats["benchmark_interval"] = benchmarkInterval_;
        stats["initialized"] = initialized_ ? 1.0 : 0.0;
        
        if (stepsProcessed_ > 0) {
            stats["avg_firing_rate"] = totalFiringRate_ / stepsProcessed_;
            stats["max_firing_rate"] = maxFiringRate_;
            stats["min_firing_rate"] = minFiringRate_;
            stats["steps_processed"] = static_cast<double>(stepsProcessed_);
        }
        
        return stats;
    }
    
private:
    void collectPerformanceMetrics(Brain* brain, size_t step) {
        stepsProcessed_++;
        
        if (brain) {
            float firingRate = brain->getAverageFiringRate();
            totalFiringRate_ += firingRate;
            maxFiringRate_ = std::max(maxFiringRate_, firingRate);
            minFiringRate_ = std::min(minFiringRate_, firingRate);
            
            // Additional metrics
            neuronCount_ = brain->getTotalNeuronCount();
            activeNeuronCount_ = brain->getActiveNeuronCount();
        }
    }
    
    void logPerformanceMetrics(size_t step) {
        // Log to file or monitoring system
        // For now, just print to console
        if (step % 100 == 0) {
            float avgFiringRate = (stepsProcessed_ > 0) ? (totalFiringRate_ / stepsProcessed_) : 0.0f;
            // In real implementation, this would log to a monitoring system
        }
    }
    
    void runPerformanceBenchmarks(Brain* brain) {
        if (!brain) return;
        
        // Run various performance benchmarks
        // This would include: neuron processing speed, memory usage, etc.
        
        // Example benchmark: neuron processing time
        Timer timer;
        
        // Simulate neuron processing (simplified)
        for (size_t i = 0; i < 1000; ++i) {
            // Simulate processing a neuron
            float dummy = 1.0f / (static_cast<float>(i) + 1.0f);
            (void)dummy;  // Avoid unused variable warning
        }
        
        double benchmarkTime = timer.elapsed();
        double neuronsPerSecond = 1000.0 / benchmarkTime;
        
        // Store benchmark results
        benchmarkResults_["neuron_processing_speed"] = neuronsPerSecond;
        
        // Log benchmark results
        if (enablePerformanceLogging_) {
            // Log benchmark results
        }
    }
    
    struct Timer {
        std::chrono::high_resolution_clock::time_point start;
        Timer() : start(std::chrono::high_resolution_clock::now()) {}
        double elapsed() const {
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            return duration.count() / 1000000.0;  // Convert to seconds
        }
    };
    
    bool initialized_ = false;
    bool enablePerformanceLogging_ = true;
    bool enablePerformanceBenchmarks_ = true;
    float benchmarkInterval_ = 10.0f;
    
    std::chrono::high_resolution_clock::time_point lastBenchmark_;
    
    size_t stepsProcessed_ = 0;
    float totalFiringRate_ = 0.0f;
    float maxFiringRate_ = 0.0f;
    float minFiringRate_ = 0.0f;
    
    // Brain statistics
    size_t neuronCount_ = 0;
    size_t activeNeuronCount_ = 0;
    
    // Benchmark results
    std::unordered_map<std::string, double> benchmarkResults_;
};

// Example Plugin: Development Stage Plugin
class DevelopmentStagePlugin : public BasePlugin<DevelopmentStagePlugin> {
public:
    std::string pluginName() const { return "DevelopmentStage"; }
    std::string pluginDescription() const { return "Manages brain development stages and plasticity changes"; }
    
    bool initialize() override {
        currentStage_ = DevelopmentalStage::Initial;
        initialized_ = true;
        return true;
    }
    
    void shutdown() override {
        initialized_ = false;
    }
    
    void configure(const Config& config) override {
        enableDevelopment_ = config.getOr("development_enable", true);
        developmentUpdateInterval_ = config.getOr("development_update_interval", 1000.0f);
        enableCriticalPeriod_ = config.getOr("development_enable_critical_period", true);
    }
    
    std::vector<std::string> getDependencies() const override {
        return {"Neuromodulation"};
    }
    
    std::vector<std::string> getProvidedServices() const override {
        return {"DevelopmentStage", "PlasticityManagement", "CriticalPeriod"};
    }
    
    void onBrainInitialize(Brain* brain) override {
        brain_ = brain;
    }
    
    void onBrainStep(Brain* brain, size_t step) override {
        if (!initialized_ || !brain) return;
        
        // Update development stage
        if (step % static_cast<size_t>(developmentUpdateInterval_) == 0) {
            updateDevelopmentStage(step);
        }
        
        // Apply development effects
        applyDevelopmentEffects(brain, step);
    }
    
    std::unordered_map<std::string, double> getStatistics() const override {
        std::unordered_map<std::string, double> stats;
        
        stats["current_stage"] = static_cast<int>(currentStage_);
        stats["enable_development"] = enableDevelopment_ ? 1.0 : 0.0;
        stats["enable_critical_period"] = enableCriticalPeriod_ ? 1.0 : 0.0;
        stats["development_update_interval"] = developmentUpdateInterval_;
        stats["initialized"] = initialized_ ? 1.0 : 0.0;
        
        return stats;
    }
    
private:
    void updateDevelopmentStage(size_t step) {
        // Simulate development stage progression
        if (step < 100) {
            currentStage_ = DevelopmentalStage::Initial;
        } else if (step < 1000) {
            currentStage_ = DevelopmentalStage::CriticalPeriod;
        } else if (step < 10000) {
            currentStage_ = DevelopmentalStage::Maturation;
        } else {
            currentStage_ = DevelopmentalStage::Adult;
        }
        
        // Log stage change
        // NLM_LOG_INFO("Development stage changed to: " + std::to_string(static_cast<int>(currentStage_)));
    }
    
    void applyDevelopmentEffects(Brain* brain, size_t step) {
        if (!enableDevelopment_ || !brain) return;
        
        // Apply development-specific effects
        switch (currentStage_) {
            case DevelopmentalStage::Initial:
                // High plasticity during initial stage
                if (auto* sp = brain->getStructuralPlasticity()) {
                    sp->setSynaptogenesisRate(0.0001f);  // Higher initial rate
                    sp->setPruningRate(0.00001f);
                }
                break;
            
            case DevelopmentalStage::CriticalPeriod:
                // Moderate plasticity with sensitive periods
                if (auto* sp = brain->getStructuralPlasticity()) {
                    sp->setSynaptogenesisRate(0.00005f);
                    sp->setPruningRate(0.00002f);
                }
                break;
            
            case DevelopmentalStage::Maturation:
                // Plasticity decreasing as brain matures
                if (auto* sp = brain->getStructuralPlasticity()) {
                    sp->setSynaptogenesisRate(0.00001f);
                    sp->setPruningRate(0.000005f);
                }
                break;
            
            case DevelopmentalStage::Adult:
                // Stable adult stage
                if (auto* sp = brain->getStructuralPlasticity()) {
                    sp->setSynaptogenesisRate(0.000001f);
                    sp->setPruningRate(0.000001f);
                }
                break;
        }
    }
    
    Brain* brain_ = nullptr;
    DevelopmentalStage currentStage_ = DevelopmentalStage::Initial;
    bool initialized_ = false;
    bool enableDevelopment_ = true;
    bool enableCriticalPeriod_ = true;
    float developmentUpdateInterval_ = 1000.0f;
};

// Plugin registration macros
NLM_REGISTER_PLUGIN(MemoryStatisticsPlugin)
NLM_REGISTER_PLUGIN(PerformanceMonitorPlugin)
NLM_REGISTER_PLUGIN(DevelopmentStagePlugin)

} // namespace nlm

#endif // NLM_EXAMPLE_PLUGINS_HPP