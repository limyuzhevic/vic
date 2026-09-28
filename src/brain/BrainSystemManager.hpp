#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../core/Random/Random.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/ActionSystem.hpp"
#include "../neuromodulation/NeuromodulationManager.hpp"
#include "../memory/MemorySystemManager.hpp"
#include "../development/DevelopmentManager.hpp"
#include "../performance/CheckpointManager.hpp"
#include "../plasticity/PlasticitySystem.hpp"
#include <memory>
#include <string>

namespace nlm {

// Brain System Manager
// Main orchestrator that coordinates all brain subsystems
// Manages the core simulation loop and region management
// This replaces the monolithic Brain class implementation

class BrainSystemManager {
public:
    BrainSystemManager();
    ~BrainSystemManager();
    
    // Initialize brain systems with configuration
    bool initialize(std::shared_ptr<Config> config);
    
    // Main simulation loop
    void run(double duration);
    
    // Core simulation step
    void step(double currentTime);
    
    // Process sensory input
    void receiveSensoryInput(const SensoryInput& input);
    
    // Get actions based on current brain state
    std::unique_ptr<Action> getAction();
    
    // Save brain state
    bool save(const std::string& filepath) const;
    
    // Load brain state
    bool load(const std::string& filepath);
    
    // Reset brain state
    void reset();
    
    // Get brain statistics
    void logStatus() const;
    
    // Get system managers for access
    NeuromodulationManager* getNeuromodulationManager() { return neuromodulationManager.get(); }
    MemorySystemManager* getMemorySystemManager() { return memorySystemManager.get(); }
    DevelopmentManager* getDevelopmentManager() { return developmentManager.get(); }
    CheckpointManager* getCheckpointManager() { return checkpointManager.get(); }
    ActionSystem* getActionSystem() { return actionSystem.get(); }
    PlasticitySystem* getPlasticitySystem() { return plasticitySystem.get(); }
    
    // Configure system parameters
    void configure(const std::string& checkpointDir, size_t maxCheckpoints,
                   float neuromodulationStrength, float memoryDecayRate,
                   float developmentSpeed, float plasticityModulation);
    
    // Get current simulation state
    double getCurrentTime() const { return currentTime_; }
    size_t getCurrentStep() const { return currentStep_; }
    bool isRunning() const { return isRunning_; }
    
    // Get brain statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    float getExcitationInhibitionRatio() const;
    
    // System health monitoring
    bool isSystemHealthy() const;
    std::string getSystemStatus() const;
    
    // Emergency stop
    void emergencyStop();
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
private:
    // Private implementation for PIMPL pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core simulation state
    double currentTime_;
    size_t currentStep_;
    bool isRunning_;
    
    // Configuration
    std::shared_ptr<Config> config_;
};

} // namespace nlm
