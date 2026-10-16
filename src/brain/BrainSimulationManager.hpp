#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace nlm {

class BrainSimulationManager {
public:
    // Create simulation manager with configuration
    explicit BrainSimulationManager(std::shared_ptr<Config> config);
    
    ~BrainSimulationManager();
    
    // Initialize simulation manager
    bool initialize();
    
    // Execute one simulation step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Reset simulation state
    void reset();
    
    // Save/load simulation state (checkpointing)
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Configure checkpoint manager
    void configureCheckpoint(const std::string& directory, size_t maxCheckpoints,
                             size_t checkpointInterval, bool compressionEnabled);
    
    // Get current simulation time
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    TimestepDuration getTimestep() const;
    
    // Get simulation statistics
    size_t getTotalSpikeCount() const;
    size_t getActiveNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Simulation lifecycle
    void start();
    void pause();
    void stop();
    bool isRunning() const;
    
    // Reset simulation statistics
    void resetStatistics();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Log simulation manager status
    void logStatus() const;

private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm