#pragma once

#include <vector>
#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declarations
class Synapse;

// Dopamine neuromodulator - implements biologically realistic dopamine dynamics
// Handles prediction error signaling, reward responses, and plasticity modulation
class Dopamine {
public:
    Dopamine();
    ~Dopamine();
    
    // Disable copying, enable moving
    Dopamine(const Dopamine&) = delete;
    Dopamine& operator=(const Dopamine&) = delete;
    Dopamine(Dopamine&&) noexcept;
    Dopamine& operator=(Dopamine&&) noexcept;
    
    // Neuromodulator interface
    const char* getName() const;
    
    // Level management
    float getLevel() const;
    void setLevel(float level);
    
    // Plasticity factor calculation
    float getPlasticityFactor() const;
    
    // Main update function - biologically plausible dopamine dynamics
    void update(float predictionError, float reward, float contextSignal, TimestepDuration dt);
    
    // Signal processing for different events
    void signalReward(float reward);
    void signalRewardPredictionError(float error);
    
    // Eligibility trace for reward-modulated learning
    float getEligibilityTrace() const;
    void updateEligibilityTrace(float predictionError, float reward, TimestepDuration dt);
    void decayEligibilityTrace(float decayRate);
    
    // Configuration and modulation
    void setVoltageDependency(float dependency);
    void setContextGain(float gain);
    void setPredictionErrorGain(float gain);
    void setRewardGain(float gain);
    
    // Internal state access
    float getFilteredPredictionError() const;
    
    // State management
    void reset();
    
    // Integration with plasticity system (future)
    // void applyToSynapse(Synapse* synapse);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
