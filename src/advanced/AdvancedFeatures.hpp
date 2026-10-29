#pragma once

#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include "core/Config/Config.hpp"

namespace nlm {

// Forward declarations
class Brain;
class Config;

/**
 * Advanced plasticity rules for expert users
 */
class AdvancedPlasticity {
public:
    AdvancedPlasticity();
    ~AdvancedPlasticity();
    
    /**
     * Configure custom STDP parameters
     */
    void configureSTDP(float ltpWeight, float ltdWeight, float tau, float learningRate = 1.0f);
    
    /**
     * Configure custom Hebbian parameters
     */
    void configureHebbian(float strength, float decayRate, float maxWeight);
    
    /**
     * Configure structural plasticity parameters
     */
    void configureStructuralPlasticity(float synaptogenesisRate, float pruningRate, float growthRate);
    
    /**
     * Add custom plasticity rule
     */
    void addCustomPlasticityRule(const std::string& name, 
                               std::function<float(const Synapse&, float)> ruleFunction);
    
    /**
     * Apply advanced plasticity to brain
     */
    void applyToBrain(Brain* brain, double timestep);
    
    /**
     * Get plasticity statistics
     */
    std::string getStatistics() const;
    
    /**
     * Reset plasticity rules
     */
    void reset();

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Multi-threading support for large-scale simulations
 */
class ParallelSimulation {
public:
    ParallelSimulation(size_t numThreads = 0);
    ~ParallelSimulation();
    
    /**
     * Initialize parallel simulation
     */
    bool initialize();
    
    /**
     * Distribute simulation across threads
     */
    void distributeSimulation(Brain* brain, const std::vector<std::vector<size_t>>& neuronGroups);
    
    /**
     * Wait for simulation completion
     */
    void waitForCompletion();
    
    /**
     * Get thread utilization
     */
    float getThreadUtilization() const;
    
    /**
     * Get simulation speedups
     */
    float getSpeedup() const;
    
    /**
     * Set thread affinity
     */
    void setThreadAffinity(const std::vector<std::vector<int>>& cpuLists);
    
    /**
     * Enable memory pooling
     */
    void enableMemoryPooling(bool enable);
    
    /**
     * Get memory usage statistics
     */
    std::string getMemoryStats() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Advanced configuration for expert users
 */
class ExpertConfiguration {
public:
    ExpertConfiguration();
    ~ExpertConfiguration();
    
    /**
     * Load configuration with expert overrides
     */
    bool loadExpertConfig(const std::string& baseConfigFile, 
                        const std::unordered_map<std::string, std::string>& overrides);
    
    /**
     * Validate configuration for expert mode
     */
    bool validateExpertConfig(Config* config) const;
    
    /**
     * Apply expert optimization presets
     */
    void applyExpertPreset(const std::string& presetName);
    
    /**
     * Get available expert presets
     */
    static std::vector<std::string> getAvailableExpertPresets();
    
    /**
     * Configure for specific use case
     */
    void configureForUseCase(const std::string& useCase);
    
    /**
     * Add custom configuration validation
     */
    void addValidationRule(const std::string& ruleName,
                           std::function<bool(const Config&)> validator);

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Performance profiling and optimization tools
 */
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    /**
     * Start performance profiling
     */
    void startProfiling();
    
    /**
     * Stop performance profiling
     */
    void stopProfiling();
    
    /**
     * Record benchmark results
     */
    void recordBenchmark(const std::string& benchmarkName, 
                        double duration, 
                        size_t neurons, 
                        size_t synapses);
    
    /**
     * Generate performance report
     */
    std::string generatePerformanceReport() const;
    
    /**
     * Export profiling data
     */
    bool exportProfilingData(const std::string& filepath) const;
    
    /**
     * Compare with previous runs
     */
    std::string compareWithPrevious(const std::string& previousDataFile) const;
    
    /**
     * Enable detailed timing
     */
    void enableDetailedTiming(bool enable);
    
    /**
     * Get memory usage statistics
     */
    std::string getMemoryUsageStats() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Scientific computing integration hooks
 */
class ScientificComputingIntegration {
public:
    ScientificComputingIntegration();
    ~ScientificComputingIntegration();
    
    /**
     * Import NumPy arrays (simulated)
     */
    bool importNumPyData(const std::string& filename, 
                        std::vector<std::vector<float>>& data);
    
    /**
     * Export data in NumPy format (simulated)
     */
    bool exportNumPyData(const std::string& filename,
                        const std::vector<std::vector<float>>& data);
    
    /**
     * Connect to SciPy for analysis
     */
    bool connectToSciPy(const std::string& endpoint = "localhost:5000");
    
    /**
     * Perform scientific analysis on brain data
     */
    std::string performScientificAnalysis(Brain* brain, 
                                          const std::string& analysisType);
    
    /**
     * Set up data collection for machine learning
     */
    void setupDataCollection(const std::string& outputPath,
                            const std::vector<std::string>& featureTypes);
    
    /**
     * Get integration status
     */
    std::string getIntegrationStatus() const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * Factory for creating expert-level components
 */
class ExpertFactory {
public:
    static std::unique_ptr<AdvancedPlasticity> createAdvancedPlasticity(
        const std::unordered_map<std::string, std::string>& options);
    
    static std::unique_ptr<ParallelSimulation> createParallelSimulation(
        size_t numThreads = 0,
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::unique_ptr<ExpertConfiguration> createExpertConfiguration(
        const std::string& baseConfigFile,
        const std::unordered_map<std::string, std::string>& overrides = {});
    
    static std::unique_ptr<PerformanceProfiler> createPerformanceProfiler(
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::unique_ptr<ScientificComputingIntegration> createScientificComputingIntegration(
        const std::unordered_map<std::string, std::string>& options = {});
    
    static std::vector<std::string> getAvailableExpertComponents();
};

} // namespace nlm

