#pragma once
#include "../core/Exceptions/NLMErrors.hpp"

#include "../core/Types/Types.hpp"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <sstream>
#include <thread>
#include <atomic>

namespace nlm {

/**
 * Base class for all experiment types in the NLM project
 * Provides common interface and behavior for all experiments
 */
class ExperimentBase {
public:
    /**
     * Constructor with optional name
     * @param name Experiment name (default: "Experiment")
     */
    ExperimentBase(const std::string& name = "Experiment");
    
    /**
     * Virtual destructor
     */
    virtual ~ExperimentBase();

    // Experiment lifecycle management - template method pattern
    /**
     * Execute the complete experiment lifecycle
     * This is a template method that calls all lifecycle steps in order
     */
    bool executeLifecycle();
    
    /**
     * Initialize the experiment before execution
     * Must be implemented by derived classes
     */
    virtual bool initialize() = 0;
    
    /**
     * Run the experiment
     * Must be implemented by derived classes
     */
    virtual bool run() = 0;
    
    /**
     * Finalize the experiment after execution
     * Must be implemented by derived classes
     */
    virtual bool finalize() = 0;

    // Common experiment patterns with default implementations
    /**
     * Validate experiment configuration and parameters
     * Can be overridden by derived classes
     */
    virtual bool validate();
    
    /**
     * Setup experiment environment and resources
     * Called after initialization
     */
    virtual bool setup();
    
    /**
     * Cleanup experiment environment and resources
     * Called before finalization
     */
    virtual bool teardown();
    
    /**
     * Generate experiment report
     * Called after successful execution
     */
    virtual bool report();

    // Experiment metadata management
    /**
     * Get experiment name
     */
    const std::string& getName() const;
    
    /**
     * Set experiment name
     */
    void setName(const std::string& name);
    
    /**
     * Get random seed for reproducibility
     */
    uint64_t getSeed() const;
    
    /**
     * Set random seed for reproducibility
     */
    void setSeed(uint64_t seed);

    // Experimental parameter management
    /**
     * Add experiment parameter
     */
    void addParameter(const std::string& key, const std::string& value);
    
    /**
     * Get experiment parameter value
     */
    std::string getParameter(const std::string& key) const;
    
    /**
     * Check if parameter exists
     */
    bool hasParameter(const std::string& key) const;
    
    /**
     * Clear all parameters
     */
    void clearParameters();

    // Result collection and reporting
    /**
     * Add experiment result
     */
    void addResult(const std::string& key, double value);
    
    /**
     * Get experiment result value
     */
    double getResult(const std::string& key) const;
    
    /**
     * Check if result exists
     */
    bool hasResult(const std::string& key) const;
    
    /**
     * Get all experiment results
     */
    const std::unordered_map<std::string, double>& getResults() const;
    
    /**
     * Generate formatted experiment report
     */
    std::string generateReport() const;

    // Hooks for experiment-specific customization
    /**
     * Called after initialization
     */
    virtual void onInitialize();
    
    /**
     * Called after setup
     */
    virtual void onSetup();
    
    /**
     * Called during run phase
     */
    virtual void onRun();
    
    /**
     * Called before teardown
     */
    virtual void onTeardown();
    
    /**
     * Called after finalization
     */
    virtual void onFinalize();

    // Performance monitoring capabilities
    /**
     * Start timing for performance measurement
     */
    void startTiming();
    
    /**
     * Stop timing and record elapsed time
     */
    void stopTiming();
    
    /**
     * Get elapsed time in seconds
     */
    double getElapsedTime() const;
    
    /**
     * Record performance metric
     */
    void recordPerformanceMetric(const std::string& name, double value);
    
    /**
     * Get all performance metrics
     */
    const std::unordered_map<std::string, double>& getPerformanceMetrics() const;

    // Serialization/deserialization
    /**
     * Serialize experiment state to string
     */
    virtual std::string serialize() const;
    
    /**
     * Deserialize experiment state from string
     */
    virtual bool deserialize(const std::string& data);

    // Validation and error handling patterns
    /**
     * Validate experiment configuration and state
     */
    virtual void validateExperiment() const;
    
    /**
     * Handle experiment errors with NLM exceptions
     */
    virtual void handleError(const NLMException& e);
    
    /**
     * Check if experiment is thread-safe
     */
    virtual bool isThreadSafe() const;

    // Configuration management
    /**
     * Set experiment configuration
     */
    void setConfig(const std::unordered_map<std::string, std::string>& config);
    
    /**
     * Get experiment configuration
     */
    const std::unordered_map<std::string, std::string>& getConfig() const;
    
    /**
     * Clear configuration
     */
    void clearConfig();

    // Status and progress tracking
    /**
     * Check if experiment is initialized
     */
    bool isInitialized() const;
    
    /**
     * Check if experiment is running
     */
    bool isRunning() const;
    
    /**
     * Get current experiment progress (0.0 to 1.0)
     */
    virtual double getProgress() const;

protected:
    /**
     * Add metric to experiment (using existing Experiment class)
     */
    void addMetric(const std::string& name, double value);
    
    /**
     * Get metric value from experiment (using existing Experiment class)
     */
    double getMetric(const std::string& name) const;
    
    /**
     * Add step data to experiment (using existing Experiment class)
     */
    void addStepData(SimulationStep step, size_t neuronCount, size_t synapseCount,
                    size_t firingCount, float avgFiringRate);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
