#pragma once

#include "../core/Types/Types.hpp"
#include <vector>
#include <cmath>
#include <algorithm>

namespace nlm {

// Bursting neuron types
enum class BurstingType {
    IntrinsicBurster,
    RegularSpiking,
    FastSpiking,
    IntrinsicOscillator,
    LowThresholdSpiking
};

// Adaptive neuron parameters
struct AdaptiveNeuronParams {
    float adaptationStrength;      // Strength of spike-frequency adaptation
    float adaptationTimeConstant;  // Time constant for adaptation variable
    float adaptationThreshold;     // Threshold for adaptation
    float subthresholdAdaptation;  // Adaptation below threshold
};

// Integrate-and-fire with adaptation parameters
struct AdaptiveIFParams {
    float adaptationVariable;      // For spike-frequency adaptation
    float adaptationConductance;   // Adaptive conductance
    float adaptationTau;           // Time constant for adaptation
};

// Hodgkin-Huxley style model parameters
struct HHModelParams {
    float maxSodiumConductance;    // Maximum sodium conductance (mS/cm²)
    float maxPotassiumConductance; // Maximum potassium conductance (mS/cm²)
    float leakConductance;        // Leak conductance (mS/cm²)
    float restingPotential;       // Resting potential (mV)
    float sodiumReversal;         // Sodium reversal potential (mV)
    float potassiumReversal;       // Potassium reversal potential (mV)
    
    // gating variable time constants
    float alphaNaTau;             // Sodium activation time constant
    float betaNaTau;              // Sodium inactivation time constant
    float alphaKTau;              // Potassium activation time constant
    
    // steady-state values
    float VhalfNa;                // Half-activation voltage for sodium
    float VhalfK;                 // Half-activation voltage for potassium
    float slopeNa;                // Slope factor for sodium
    float slopeK;                 // Slope factor for potassium
};

// Resonant neuron parameters
struct ResonantNeuronParams {
    float resonanceFrequency;     // Natural frequency (Hz)
    float resonanceQFactor;       // Quality factor (0-1, lower = more resonance)
    float membraneTimeConstant;   // Membrane time constant (ms)
    float couplingStrength;       // Coupling to resonance oscillator
    float externalInputPhase;     // Phase of external input
};

// Base neuron interface for all neuron types
class AdvancedNeuronBase {
public:
    virtual ~AdvancedNeuronBase() = default;
    
    // Core simulation step
    virtual bool step(Timestamp currentTime, TimestepDuration dt) = 0;
    
    // Reset neuron to initial state
    virtual void reset() = 0;
    
    // Get current membrane potential
    virtual MembranePotential getMembranePotential() const = 0;
    
    // Check if neuron is in bursting mode
    virtual bool isBursting() const = 0;
    
    // Get current burst state
    virtual float getBurstState() const = 0;
    
    // Record spike event
    virtual void recordSpike(Timestamp timestamp) = 0;
    
    // Get spike history
    virtual const std::vector<Timestamp>& getSpikeHistory() const = 0;
    
    // Get neuron type
    virtual NeuronType getType() const = 0;
    
    // Set neuron parameters
    virtual void setParameters(const void* params) = 0;
    
    // Get parameters
    virtual const void* getParameters() const = 0;
};

// Bursty neuron implementation
class BurstingNeuron : public AdvancedNeuronBase {
public:
    BurstingNeuron(NeuronId id);
    ~BurstingNeuron() override;
    
    // Core simulation
    bool step(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    
    // Bursting control
    void setBurstingType(BurstingType type);
    void setBurstFrequency(float frequencyHz);
    void setBurstDuration(float durationMs);
    void setInterBurstInterval(float intervalMs);
    
    // State access
    bool isBursting() const override;
    float getBurstState() const override;
    
    // Advanced dynamics
    void enableAdaptation(bool enable);
    void setAdaptationRate(float rate);
    
    // Neuron interface
    MembranePotential getMembranePotential() const override;
    NeuronType getType() const override;
    void setParameters(const void* params) override;
    const void* getParameters() const override;
    
    // Spike history
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override;
    
    // Configuration
    void configureForPattern(bool requiresBursting);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Adaptive neuron implementation
class AdaptiveNeuron : public AdvancedNeuronBase {
public:
    AdaptiveNeuron(NeuronId id);
    ~AdaptiveNeuron() override;
    
    // Core simulation with adaptation
    bool step(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    
    // Adaptation control
    void setAdaptationParams(const AdaptiveNeuronParams& params);
    void enableSubthresholdAdaptation(bool enable);
    void setAdaptationThreshold(float threshold);
    
    // State access
    bool isBursting() const override { return false; }
    float getBurstState() const override { return 0.0f; }
    
    // Neuron interface
    MembranePotential getMembranePotential() const override;
    NeuronType getType() const override;
    void setParameters(const void* params) override;
    const void* getParameters() const override;
    
    // Spike history
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override;
    
    // Activity metrics
    float getFiringRate() const;
    float getAdaptationLevel() const;
    float getRecentFiringRate(float windowMs) const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Integrate-and-fire with adaptation
class AdaptiveIntegrateAndFire : public AdvancedNeuronBase {
public:
    AdaptiveIntegrateAndFire(NeuronId id);
    ~AdaptiveIntegrateAndFire() override;
    
    // Core simulation with adaptive threshold
    bool step(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    
    // Adaptive IF parameters
    void setAdaptiveIFParams(const AdaptiveIFParams& params);
    void setThresholdAdaptation(float adaptationStrength);
    void setReferencePotential(float refPot);
    
    // State access
    bool isBursting() const override { return false; }
    float getBurstState() const override { return 0.0f; }
    
    // Neuron interface
    MembranePotential getMembranePotential() const override;
    NeuronType getType() const override;
    void setParameters(const void* params) override;
    const void* getParameters() const override;
    
    // Spike history
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override;
    
    // Adaptive metrics
    float getAdaptationVariable() const;
    float getThresholdEvolution() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Simplified Hodgkin-Huxley model
class HodgkinHuxleyNeuron : public AdvancedNeuronBase {
public:
    HodgkinHuxleyNeuron(NeuronId id);
    ~HodgkinHuxleyNeuron() override;
    
    // Core simulation with HH dynamics
    bool step(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    
    // HH model parameters
    void setHHModelParams(const HHModelParams& params);
    void setInitialGatingVariables(float m, float h, float n);
    
    // State access
    bool isBursting() const override;
    float getBurstState() const override;
    
    // Neuron interface
    MembranePotential getMembranePotential() const override;
    NeuronType getType() const override;
    void setParameters(const void* params) override;
    const void* getParameters() const override;
    
    // Spike history
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override;
    
    // HH state variables
    float getSodiumActivation() const;
    float getSodiumInactivation() const;
    float getPotassiumActivation() const;
    
    // Burst detection
    void analyzeBurstingPattern();
    bool isInBurst() const;
    float getBurstFrequency() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Resonant neuron implementation
class ResonantNeuron : public AdvancedNeuronBase {
public:
    ResonantNeuron(NeuronId id);
    ~ResonantNeuron() override;
    
    // Core simulation with resonance
    bool step(Timestamp currentTime, TimestepDuration dt) override;
    void reset() override;
    
    // Resonant neuron parameters
    void setResonantNeuronParams(const ResonantNeuronParams& params);
    void setResonanceFrequency(float frequencyHz);
    void setResonanceQFactor(float qFactor);
    
    // State access
    bool isBursting() const override;
    float getBurstState() const override;
    
    // Neuron interface
    MembranePotential getMembranePotential() const override;
    NeuronType getType() const override;
    void setParameters(const void* params) override;
    const void* getParameters() const override;
    
    // Spike history
    void recordSpike(Timestamp timestamp) override;
    const std::vector<Timestamp>& getSpikeHistory() const override;
    
    // Resonance metrics
    float getResonanceFrequency() const;
    float getResonanceQuality() const;
    float getPhaseResponse() const;
    
    // Input filtering
    void setInputFilter(float cutoffFrequencyHz);
    void setPhaseShift(float phaseShift);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Neuron factory for creating different neuron types
class AdvancedNeuronFactory {
public:
    static AdvancedNeuronBase* createNeuron(NeuronType type, NeuronId id);
    static AdvancedNeuronBase* createBurstingNeuron(NeuronId id, BurstingType burstType);
    static AdvancedNeuronBase* createAdaptiveNeuron(NeuronId id);
    static AdvancedNeuronBase* createAdaptiveIFNeuron(NeuronId id);
    static AdvancedNeuronBase* createHodgkinHuxleyNeuron(NeuronId id);
    static AdvancedNeuronBase* createResonantNeuron(NeuronId id);
    
    static void destroyNeuron(AdvancedNeuronBase* neuron);
};

} // namespace nlm