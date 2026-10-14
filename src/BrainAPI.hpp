// BrainAPI.hpp - Simplified user interface for NLM

/**
 * BrainAPI - Easy-to-use interface for NLM users
 * 
 * This API provides a simplified interface to the complex NLM brain system.
 * It handles configuration, initialization, and basic operations while
 * maintaining access to advanced features for power users.
 */

#pragma once

#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <functional>
#include <unordered_map>
#include <any>
#include <vector>

namespace nlm {

// Forward declarations
class Brain;
struct DevelopmentalStage;
struct DetailedSpikeEvent;
struct Action;
struct SensoryInput;

/**
 * Statistics structure for brain status reporting
 */
struct BrainAPIStats {
    int totalNeurons;
    int totalSynapses;
    float avgFiringRate;
    float eI_Ratio;
    int totalSpikes;
    int workingMemoryTraced;
    int memoryEpisodes;
    float dopamineLevel;
    float curiosityLevel;
    float noveltyLevel;
    
    BrainAPIStats()
        : totalNeurons(0), totalSynapses(0), avgFiringRate(0.0f), eI_Ratio(0.0f),
          totalSpikes(0), workingMemoryTraced(0), memoryEpisodes(0),
          dopamineLevel(0.0f), curiosityLevel(0.0f), noveltyLevel(0.0f) {}
};

/**
 * Main user interface class for NLM brains
 */
class BrainAPI {
public:
    /**
     * Default constructor
     */
    BrainAPI();
    
    /**
     * Destructor
     */
    ~BrainAPI();
    
    /**
     * Create a default brain with standard configuration
     * 
     * @return Unique pointer to new BrainAPI instance
     */
    static std::unique_ptr<BrainAPI> createDefault();
    
    /**
     * Create a brain from configuration file
     * 
     * @param configFile Path to configuration file
     * @return Unique pointer to new BrainAPI instance
     */
    static std::unique_ptr<BrainAPI> createFromFile(const std::string& configFile);
    
    /**
     * Create a brain from custom configuration
     * 
     * @param config Configuration object
     * @return Unique pointer to new BrainAPI instance
     */
    static std::unique_ptr<BrainAPI> createCustom(const Config& config);
    
    /**
     * Initialize the brain with current configuration
     * 
     * @return true if initialization successful, false otherwise
     */
    bool initialize();
    
    /**
     * Step the brain forward by specified steps
     * 
     * @param steps Number of simulation steps to advance
     */
    void step(int steps = 1);
    
    /**
     * Step the brain forward by specified time delta
     * 
     * @param timeDelta Time to advance in seconds
     */
    void step(double timeDelta);
    
    /**
     * Get current brain statistics
     * 
     * @return Statistics structure
     */
    BrainAPIStats getStats() const;
    
    /**
     * Reset brain to initial state
     */
    void reset();
    
    /**
     * Save brain state to file
     * 
     * @param path File path to save to
     * @return true if save successful, false otherwise
     */
    bool save(const std::string& path);
    
    /**
     * Load brain state from file
     * 
     * @param path File path to load from
     * @return true if load successful, false otherwise
     */
    bool load(const std::string& path);
    
    /**
     * Enable or disable a brain feature
     * 
     * @param feature Feature name
     * @param enable true to enable, false to disable
     */
    void enableFeature(const std::string& feature, bool enable);
    
    /**
     * Check if a feature is enabled
     * 
     * @param feature Feature name
     * @return true if feature is enabled, false otherwise
     */
    bool isFeatureEnabled(const std::string& feature) const;
    
    /**
     * Set developmental stage
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentStage(DevelopmentalStage stage);
    
    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentStage() const;
    
    /**
     * Set step callback function
     * 
     * @param callback Function to call on each step
     */
    void setStepCallback(std::function<void(int)> callback);
    
    /**
     * Set spike event callback function
     * 
     * @param callback Function to call on spike events
     */
    void setSpikeCallback(std::function<void(const DetailedSpikeEvent&)> callback);
    
    /**
     * Get configuration object
     * 
     * @return Reference to configuration object
     */
    Config& getConfig();
    
    /**
     * Get configuration object (const version)
     * 
     * @return Const reference to configuration object
     */
    const Config& getConfig() const;
    
    /**
     * Load a configuration profile
     * 
     * @param profileName Name of profile to load
     * @return true if profile loaded successfully, false otherwise
     */
    bool loadProfile(const std::string& profileName);
    
    /**
     * Get list of available configuration profiles
     * 
     * @return Vector of available profile names
     */
    std::vector<std::string> getAvailableProfiles() const;
    
    /**
     * Check if configuration is valid
     * 
     * @return true if configuration is valid, false otherwise
     */
    bool validateConfiguration() const;
    
    /**
     * Log current brain status
     */
    void logStatus() const;
    
    /**
     * Get underlying Brain instance (for advanced users)
     * 
     * @return Shared pointer to underlying Brain instance
     */
    std::shared_ptr<Brain> getBrain();

private:
    /**
     * Internal implementation details
     */
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    
    bool initialized_;
    
    std::function<void(int)> stepCallback_;
    std::function<void(const DetailedSpikeEvent&)> spikeCallback_;
    
    /**
     * Set up event handlers
     */
    void setupEventHandlers();
    
    friend class BrainAPITest;
};

/**
 * Backward compatibility functions for existing NLM code
 */
namespace {
    /**
     * Step the global brain (backward compatibility)
     * 
     * @param steps Number of steps to advance
     */
    void brainStep(int steps);
    
    /**
     * Step the global brain with time delta (backward compatibility)
     * 
     * @param time Time to advance in seconds
     */
    void brainStepWithTime(double time);
    
    /**
     * Reset the global brain (backward compatibility)
     */
    void brainReset();
    
    /**
     * Save global brain state (backward compatibility)
     * 
     * @param path File path to save to
     * @return true if save successful
     */
    bool brainSave(const std::string& path);
    
    /**
     * Load global brain state (backward compatibility)
     * 
     * @param path File path to load from
     * @return true if load successful
     */
    bool brainLoad(const std::string& path);
    
    /**
     * Get total neuron count (backward compatibility)
     * 
     * @return Total number of neurons
     */
    size_t getTotalNeurons();
    
    /**
     * Get total synapse count (backward compatibility)
     * 
     * @return Total number of synapses
     */
    size_t getTotalSynapses();
    
    /**
     * Get average firing rate (backward compatibility)
     * 
     * @return Average firing rate
     */
    float getAverageFiringRate();
    
    /**
     * Get E/I ratio (backward compatibility)
     * 
     * @return Excitation/inhibition ratio
     */
    float getExcitationInhibitionRatio();
    
    /**
     * Initialize global brain API
     */
    void initializeGlobalBrainAPI();
}

} // namespace nlm
