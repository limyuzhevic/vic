#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Sleep system: Implements rest/rest cycle for memory consolidation
// Sleep is essential for episodic memory consolidation and synaptic homeostasis
// Based on biological sleep cycles: NREM (slow wave), REM (paradoxical), awake

class Sleep {
public:
    Sleep();
    ~Sleep();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Get current sleep state
    enum class SleepState {
        Awake,
        NREM,      // Non-rapid eye movement sleep (deep rest)
        REM,       // Rapid eye movement sleep (dreaming)
        Transition  // Between states
    };
    
    SleepState getState() const;
    
    // Update sleep state and perform consolidation
    void update(class Brain* brain, TimestepDuration dt);
    
    // Get sleep depth (0.0 = awake, 1.0 = deep sleep)
    float getSleepDepth() const;
    
    // Get current sleep stage
    std::string getCurrentStageName() const;
    
    // Get time spent in current state
    TimestepDuration getTimeInCurrentState() const;
    
    // Get memory consolidation rate
    float getConsolidationRate() const;
    
    // Set sleep parameters
    void setSleepSchedule(float awakeDuration, float nremDuration, float remDuration);
    void setConsolidationStrength(float strength);
    
    // Get consolidated memories
    const std::vector<std::string>& getConsolidatedMemories() const;
    
    // Reset
    void reset();
    
    // Is currently sleeping?
    bool isSleeping() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
