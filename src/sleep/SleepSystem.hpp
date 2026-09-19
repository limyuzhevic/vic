#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../neuromodulation/Acetylcholine.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <vector>
#include <chrono>
#include <algorithm>
#include <cmath>

namespace nlm {

// SleepSystem: Manages sleep cycles, memory consolidation, and neural plasticity changes during sleep
// Implements multi-stage sleep (wake, REM, deep NREM, consolidation) with memory replay and synaptic renormalization

class SleepSystem {
public:
    SleepSystem();
    ~SleepSystem();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Update sleep system based on current time
    void update(Timestamp currentTime, SimulationStep currentStep);
    
    // Start sleep cycle
    bool startSleep();
    
    // End sleep cycle
    void endSleep();
    
    // Check if brain is currently sleeping
    bool isSleeping() const;
    
    // Get current sleep stage
    SleepStage getSleepStage() const;
    
    // Get current sleep depth (0.0 to 1.0)
    float getSleepDepth() const;
    
    // Get sleep duration in seconds
    float getSleepDuration() const;
    
    // Get time elapsed in current sleep stage
    float getTimeInCurrentStage() const;
    
    // Force sleep transition to next stage
    void advanceToNextStage();
    
    // Get memory replay statistics
    size_t getTotalMemoriesReplayed() const;
    size_t getReplayCount() const;
    
    // Get consolidation statistics
    size_t getTotalMemoriesConsolidated() const;
    size_t getConsolidationCount() const;
    
    // Get synaptic plasticity statistics
    float getAverageSynapticWeight() const;
    float getSynapticPruningRate() const;
    
    // Reset sleep system
    void reset();
    
    // Configuration
    void setSleepDuration(float duration);  // Total sleep duration in seconds
    void setWakeReadyDuration(Timestamp duration);
    void setREMAmplitude(float amplitude);
    void setDeepNREMThreshold(float threshold);
    void setConsolidationPower(float power);
    void setSynapticDownscalingRate(float rate);
    void setSynapticUplearningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
