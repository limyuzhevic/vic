#pragma once

// Base types used across all brain subsystems
#include "../core/Types/Types.hpp"
#include <memory>
#include <vector>
#include <functional>

namespace nlm {

// Forward declarations for Brain class
class Brain;
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class CheckpointManager;

// Subsystem base interface
class ISubsystem {
public:
    virtual ~ISubsystem() = default;
    virtual bool initialize(Brain* brain) = 0;
    virtual void update(float dt) = 0;
    virtual void reset() = 0;
    virtual void logStatus() const = 0;
};

// Progress callback type
using ProgressCallback = std::function<void(float)>;

// Forward declarations for all subsystems
class NeuralComputationSystem;
class PlasticityManager;
class MemorySystems;
class CognitionSystem;
class DevelopmentSystem;
class NeuromodulationSystem;
class EnvironmentInterface;

// Brain coordinator manages all subsystems and their interactions
class BrainCoordinator {
public:
    BrainCoordinator(std::shared_ptr<Config> config);
    ~BrainCoordinator();
    
    // Initialize all subsystems
    bool initialize(Brain* brain);
    
    // Main coordination step
    void step(SimulationStep step, Timestamp time);
    
    // Subsystem management
    NeuralComputationSystem* getNeuralComputation() const;
    PlasticityManager* getPlasticity() const;
    MemorySystems* getMemory() const;
    CognitionSystem* getCognition() const;
    DevelopmentSystem* getDevelopment() const;
    NeuromodulationSystem* getNeuromodulation() const;
    EnvironmentInterface* getEnvironment() const;
    
    // Progress tracking
    void setProgressCallback(ProgressCallback callback);
    
    // Reset all subsystems
    void reset();
    
    // Get status from all subsystems
    void logStatus() const;
    
    // State management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
