// BaseExperiment.hpp - Abstract base class for all experiments
#pragma once

/**
 * BaseExperiment - Abstract base class for all NLM experiments
 * 
 * This class defines the common interface and functionality for all
 * experiment types (Phase 3, Phase 4, Phase 6, etc.). It provides
 * common workflow management, logging, and configuration handling.
 */

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../experiments/Experiment.hpp"
#include "../experiments/Metrics.hpp"
#include <memory>
#include <string>
#include <chrono>
#include <vector>

namespace nlm {

/**
 * Experiment type enumeration
 */
enum class ExperimentType {
    Unknown = 0,
    Phase3 = 1,
    Phase4 = 2,
    Phase5 = 3,
    Phase6 = 4
};

/**
 * Base abstract experiment class
 * 
 * All concrete experiment classes must inherit from this class and
 * implement the pure virtual methods to define their specific behavior.
 */
class BaseExperiment {
public:
    virtual ~BaseExperiment() = default;
    
    /**
     * Initialize the experiment
     * 
     * @return true if initialization succeeds, false otherwise
     */
    virtual bool initialize() = 0;
    
    /**
     * Run the experiment
     * 
     * @return true if execution completes successfully, false on failure
     */
    virtual bool run() = 0;
    
    /**
     * Shutdown the experiment
     */
    virtual void shutdown() = 0;
    
    /**
     * Get the experiment name
     * 
     * @return Human-readable name of the experiment
     */
    virtual const std::string& getName() const = 0;
    
    /**
     * Get the experiment type
     * 
     * @return Type of experiment (Phase3, Phase4, Phase6, etc.)
     */
    virtual ExperimentType getType() const = 0;
    
    /**
     * Get experiment results
     * 
     * @return Results from experiment execution
     */
    virtual const Metrics& getResults() const = 0;
    
    /**
     * Check if experiment has been initialized
     * 
     * @return true if experiment is initialized
     */
    bool isInitialized() const { return initialized_; }
    
    /**
     * Get experiment start time
     * 
     * @return Start time as time_t
     */
    time_t getStartTime() const { return startTime_; }
    
    /**
     * Get experiment end time
     * 
     * @return End time as time_t
     */
    time_t getEndTime() const { return endTime_; }
    
    /**
     * Get total wall-clock execution time
     * 
     * @return Execution time in seconds
     */
    double getWallClockTime() const { return wallClockTime_; }
    
protected:
    BaseExperiment() : initialized_(false), startTime_(0), endTime_(0), wallClockTime_(0.0) {}
    
    /**
     * Set initialization status
     * 
     * @param initialized New initialization status
     */
    void setInitialized(bool initialized) { initialized_ = initialized; }
    
    /**
     * Set start time
     * 
     * @param startTime Start time
     */
    void setStartTime(time_t startTime) { startTime_ = startTime; }
    
    /**
     * Set end time and calculate wall-clock time
     * 
     * @param endTime End time
     */
    void setEndTime(time_t endTime) {
        endTime_ = endTime;
        wallClockTime_ = std::chrono::duration<double>(
            std::chrono::high_resolution_clock::now() - startWallClockTime_)
            .count();
    }
    
    /**
     * Get logger instance
     * 
     * @return Reference to global logger
     */
    Logger& getLogger() { return Logger::getInstance(); }
    
    /**
     * Create a default configuration
     * 
     * @return Shared pointer to new configuration
     */
    std::shared_ptr<Config> createDefaultConfig() {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        return config;
    }
    
    /**
     * Validate configuration
     * 
     * @param config Configuration to validate
     * @return true if configuration is valid, false otherwise
     */
    bool validateConfig(const std::shared_ptr<Config>& config) {
        if (!config) {
            getLogger().logError("Configuration is null");
            return false;
        }
        
        auto neuronCountOpt = config->get<int64_t>("neuron_count");
        if (neuronCountOpt && *neuronCountOpt <= 0) {
            getLogger().logError("Neuron count must be positive: " + std::to_string(*neuronCountOpt));
            return false;
        }
        
        auto regionCountOpt = config->get<int64_t>("region_count");
        if (regionCountOpt && *regionCountOpt <= 0) {
            getLogger().logError("Region count must be positive: " + std::to_string(*regionCountOpt));
            return false;
        }
        
        return true;
    }
    
    /**
     * Log experiment start
     */
    void logStart() {
        startTime_ = time(nullptr);
        startWallClockTime_ = std::chrono::high_resolution_clock::now();
        getLogger().logInfo("Starting experiment: " + getName());
    }
    
    /**
     * Log experiment completion
     */
    void logCompletion() {
        endTime_ = time(nullptr);
        getLogger().logInfo("Completed experiment: " + getName());
    }
    
private:
    bool initialized_;
    time_t startTime_;
    time_t endTime_;
    double wallClockTime_;
    std::chrono::high_resolution_clock::time_point startWallClockTime_;
};

} // namespace nlm
