/*
 * Enhanced Logging and Debugging for NLM
 * 
 * This file implements the enhanced logging and debugging capabilities
 * for the NLM (Neural Learning Machine) codebase as specified in the requirements.
 * 
 * Features implemented:
 * 1. Enhanced Brain::logStatus() - Detailed statistics and system health monitoring
 * 2. Debug mode support - Debug logging levels and debug mode to Brain class
 * 3. Performance monitoring - Timing, memory usage, and performance metrics
 * 4. Error handling - Improved error reporting and error codes
 * 5. System health checks - Health checks for all subsystems
 * 6. Event logging - Logging for important events (spikes, learning, memory updates)
 * 7. Configuration validation - Validation for configuration parameters
 * 8. Connection tracking - Track connections between systems
 * 
 * The implementation follows the project's coding standards and best practices.
 */

#pragma once

#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include <string>
#include <memory>
#include <sstream>
#include <chrono>
#include <thread>
#include <iomanip>
#include <ctime>
#include <sys/resource.h>
#include <unordered_set>
#include <map>
#include <limits>
#include <numeric>
#include <cstdint>
#include <optional>
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <sys/resource.h>
#include <unordered_set>
#include <map>
#include <limits>
#include <numeric>
#include <cstdint>
#include <optional>

namespace nlm {

// System statistics structure for logging and debugging
struct SystemStats {
    size_t totalNeurons;
    size_t totalSynapses;
    size_t totalSpikes;
    size_t pendingSpikeEvents;
    size_t activeNeurons;
    size_t firingNeurons;
    float averageFiringRate;
    float excitationInhibitionRatio;
    size_t memoryTraces;
    size_t episodicEpisodes;
    float dopamineLevel;
    size_t workingMemoryCapacity;
    size_t episodicMaxEpisodes;
    size_t associativeAssociations;
    double systemUptime;
    uint64_t memoryUsage;
    uint64_t cpuUsage;
    std::chrono::steady_clock::time_point startTime;
    
    SystemStats() : totalNeurons(0), totalSynapses(0), totalSpikes(0), pendingSpikeEvents(0),
                   activeNeurons(0), firingNeurons(0), averageFiringRate(0.0f),
                   excitationInhibitionRatio(0.0f), memoryTraces(0), episodicEpisodes(0),
                   dopamineLevel(0.0f), workingMemoryCapacity(0), episodicMaxEpisodes(0),
                   associativeAssociations(0), systemUptime(0.0), memoryUsage(0),
                   cpuUsage(0), startTime(std::chrono::steady_clock::now()) {}
};

// Connection information for tracking
struct ConnectionInfo {
    std::string sourceSystem;
    std::string targetSystem;
    float weight;
    Delay delay;
    size_t connectionCount;
    bool isActive;
    std::chrono::steady_clock::time_point lastUpdate;
    
    ConnectionInfo() : weight(0.0f), delay(1), connectionCount(0), isActive(false) {}
};

// Brain class with enhanced logging and debugging capabilities
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules
    void updatePlasticity();
    
    // Apply developmental changes
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state
    bool save(const std::string& filepath) const;
    
    // Load brain state
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Memory system accessors
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system accessor
    PredictionSystem* getPredictionSystem();
    
    // Cognition system accessors
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system accessor
    DevelopmentSystem* getDevelopmentSystem();
    
    // Neuromodulation system accessors
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Configuration accessor
    std::shared_ptr<const Config> getConfig() const;
    
    // Random generator accessor
    RandomGenerator* getRandomGenerator();
    
    // ========== ENHANCED LOGGING AND DEBUGGING METHODS ==========
    
    // Enhanced logging method with detailed statistics
    void logStatus() const;
    
    // Debug mode support
    void enableDebugMode(bool enable);
    bool isDebugModeEnabled() const;
    
    // Performance monitoring
    void enablePerformanceMonitoring(bool enable);
    bool isPerformanceMonitoringEnabled() const;
    
    // System health checks
    void enableSystemHealthChecks(bool enable);
    bool areSystemHealthChecksEnabled() const;
    
    // Error handling and error codes
    void enableErrorCodes(bool enable);
    bool areErrorCodesEnabled() const;
    
    // Event logging
    void enableEventLogging(bool enable);
    bool isEventLoggingEnabled() const;
    
    // Connection tracking
    void enableConnectionTracking(bool enable);
    bool isConnectionTrackingEnabled() const;
    
    // Performance logging
    void logPerformance(const std::string& description, double elapsedTime) const;
    
    // Error logging with codes
    void logError(int errorCode, const std::string& message, const std::string& category = "Error") const;
    
    // System event logging
    void logSystemEvent(const std::string& event, const std::string& category = "System") const;
    
    // System health check
    bool performSystemHealthCheck() const;
    
    // Configuration validation
    bool validateConfiguration() const;
    
    // Connection validation
    bool validateConnections() const;
    
    // Get system statistics
    SystemStats getSystemStats() const;
    
    // Get connection information
    std::vector<ConnectionInfo> getConnections() const;
    
    // Get system performance metrics
    float getCPUUsage() const;
    uint64_t getMemoryUsage() const;
    
    // Performance metrics structure
    struct PerformanceMetrics {
        double lastStepTime;
        double averageStepTime;
        double minStepTime;
        double maxStepTime;
        size_t stepsCount;
        uint64_t memoryAllocated;
        std::chrono::steady_clock::time_point lastUpdate;
        
        PerformanceMetrics() : lastStepTime(0.0), averageStepTime(0.0), minStepTime(0.0),
                              maxStepTime(0.0), stepsCount(0), memoryAllocated(0),
                              lastUpdate(std::chrono::steady_clock::now()) {}
    };
    
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Debug information
    std::string getDebugInfo() const;
    std::string getHealthReport() const;
    
    // Get step count and current time
    size_t getStepCount() const;
    Timestamp getCurrentTime() const;
    
private:
    // Implementation details
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
