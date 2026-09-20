#pragma once

#include "../brain/Neuron.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// Adaptive Threshold Neuron: Sliding threshold that adapts based on recent activity
// Model inspired by biological adaptive threshold mechanisms in cortical neurons
// The threshold rises with recent spiking activity (adaptation) and decays with time
class AdaptiveThresholdNeuron : public Neuron {
public:
    AdaptiveThresholdNeuron(NeuronId id) : Neuron(id) {
        // Additional adaptive threshold parameters
        resetAdaptationThreshold();
        recordAdaptationParameters();
    }
    
    ~AdaptiveThresholdNeuron() override = default;
    
    // Set adaptive threshold parameters
    void setAdaptationTimeConstant(float tau) { adaptationTimeConstant_ = tau; }
    float getAdaptationTimeConstant() const { return adaptationTimeConstant_; }
    
    void setAdaptationStrength(float strength) { adaptationStrength_ = strength; }
    float getAdaptationStrength() const { return adaptationStrength_; }
    
    void setBaselineThreshold(float baseline) { baselineThreshold_ = baseline; }
    float getBaselineThreshold() const { return baselineThreshold_; }
    
    // Get current adaptive threshold
    float getAdaptiveThreshold() const {
        return baselineThreshold_ + (currentAdaptationLevel_ * adaptationStrength_);
    }
    
    // Reset adaptation level (for experimental control)
    void resetAdaptationLevel() { currentAdaptationLevel_ = 0.0f; }
    
    // Override the step function to include adaptive threshold logic
    void step(Timestamp currentTime) override {
        // First call the base LIF step to update membrane potential
        bool fired = stepLIF(currentTime);
        
        // Update adaptive threshold based on recent activity
        if (fired) {
            // Strong adaptation when neuron fires
            currentAdaptationLevel_ += adaptationStrength_;
        }
        
        // Exponential decay of adaptation
        float decayFactor = std::exp(-1.0f / adaptationTimeConstant_);
        currentAdaptationLevel_ *= decayFactor;
        
        // Clamp adaptation level to prevent runaway
        currentAdaptationLevel_ = std::clamp(currentAdaptationLevel_, 0.0f, 1.0f);
    }
    
private:
    void resetAdaptationThreshold() {
        adaptationTimeConstant_ = 50.0f;  // ms
        adaptationStrength_ = 0.1f;       // dimensionless
        baselineThreshold_ = -55.0f;     // mV
        currentAdaptationLevel_ = 0.0f;
    }
    
    void recordAdaptationParameters() {
        // Record neuron parameters for experimental tracking
        // This is a placeholder for advanced parameter recording
    }
    
    // Adaptive threshold state
    float adaptationTimeConstant_;    // Time constant for adaptation decay (ms)
    float adaptationStrength_;       // Strength of adaptation effect
    float baselineThreshold_;        // Baseline threshold (mV)
    float currentAdaptationLevel_;   // Current level of adaptation (0-1)
};

// Resonator Neuron: Bandpass filter with resonance properties
// Model inspired by resonant membrane models in cortical neurons
// Provides frequency-selective responses and enhanced signal detection
class ResonatorNeuron : public Neuron {
public:
    ResonatorNeuron(NeuronId id) : Neuron(id) {
        // Initialize resonance parameters
        resetResonanceParameters();
        recordResonanceParameters();
    }
    
    ~ResonatorNeuron() override = default;
    
    // Set resonance parameters
    void setResonanceFrequency(float freq) { resonanceFrequency_ = freq; }
    float getResonanceFrequency() const { return resonanceFrequency_; }
    
    void setResonanceQ(float q) { resonanceQ_ = q; }
    float getResonanceQ() const { return resonanceQ_; }
    
    void setAlpha(float alpha) { alpha_ = alpha; }
    float getAlpha() const { return alpha_; }
    
    void setBeta(float beta) { beta_ = beta; }
    float getBeta() const { return beta_; }
    
    // Get resonance amplification factor for current frequency
    float getResonanceAmplification(float frequency) const {
        if (frequency <= 0.0f) return 1.0f;
        
        // Calculate amplification using resonance formula
        float bandwidth = resonanceFrequency_ / resonanceQ_;
        float amplification = 1.0f / (1.0f + std::pow(frequency - resonanceFrequency_, 2) / (bandwidth * bandwidth));
        
        return amplification;
    }
    
    // Override step function to include resonance dynamics
    void step(Timestamp currentTime) override {
        // Get base membrane potential from parent
        MembranePotential basePotential = getMembranePotential();
        
        // Apply resonance filtering to membrane potential
        // This acts as a bandpass filter around the resonance frequency
        float filteredPotential = applyResonanceFilter(basePotential, getCurrentFiringRate());
        
        // Set the filtered potential (base step already updated membrane potential)
        setMembranePotential(filteredPotential);
        
        // Update resonance state
        updateResonanceState(currentTime);
        
        // Check threshold with resonance amplification
        bool thresholdCrossed = basePotential >= (getThreshold() * getResonanceAmplification(getCurrentFiringRate()));
        
        // Handle firing with resonance-specific behavior
        if (thresholdCrossed) {
            // Enhanced firing for resonant frequencies
            float firingBoost = getResonanceAmplification(getCurrentFiringRate()) - 1.0f;
            setFiringState(FiringState::Active);
            
            // Record resonant spike
            recordResonantSpike(currentTime);
            
            // Reset membrane potential with resonant effects
            setMembranePotential(getResetPotential() + firingBoost * 0.1f);
            
            // Enter refractory period
            setRefractoryPeriod(1);  // Simple 1ms refractory for now
            setFiringState(FiringState::Refractory);
        }
    }
    
private:
    void resetResonanceParameters() {
        // Default resonance parameters for alpha rhythm-like behavior
        resonanceFrequency_ = 10.0f;    // Hz
        resonanceQ_ = 5.0f;           // Quality factor
        alpha_ = 0.5f;                // Membrane time constant modifier
        beta_ = 0.1f;                 // Resonance strength
    }
    
    void recordResonanceParameters() {
        // Record parameters for experimental tracking
    }
    
    float applyResonanceFilter(MembranePotential potential, float frequency) const {
        // Apply a resonant filter to the membrane potential
        // This implements a simplified version of resonant membrane dynamics
        
        float amplification = getResonanceAmplification(frequency);
        float filtered = potential * (1.0f - beta_) + amplification * beta_ * getMembranePotential();
        
        return std::clamp(filtered, -100.0f, 50.0f);
    }
    
    void updateResonanceState(Timestamp currentTime) {
        // Update resonance state variables
        // This could include phase of oscillation, etc.
    }
    
    void recordResonantSpike(Timestamp timestamp) {
        // Record spike with resonance information
        // This could store phase information
    }
    
    // Resonance state variables
    float resonanceFrequency_;   // Resonant frequency (Hz)
    float resonanceQ_;          // Quality factor
    float alpha_;               // Membrane time constant modifier
    float beta_;                // Resonance strength
    float phase_;               // Phase of oscillation
    
    // Helper to get current firing rate (simplified)
    float getCurrentFiringRate() const {
        // This is a placeholder - in real implementation would track instantaneous rate
        return 1.0f;  // Assume 1 Hz for now
    }
};

// Calcium Dynamics Neuron: Add calcium-dependent processes
// Model inspired by calcium dynamics in neurons affecting plasticity and excitability
class CalciumDynamicsNeuron : public Neuron {
public:
    CalciumDynamicsNeuron(NeuronId id) : Neuron(id) {
        resetCalciumParameters();
        recordCalciumParameters();
    }
    
    ~CalciumDynamicsNeuron() override = default;
    
    // Calcium parameters
    void setCalciumConductance(float gCa) { calciumConductance_ = gCa; }
    float getCalciumConductance() const { return calciumConductance_; }
    
    void setCalciumReversalPotential(float vCa) { calciumReversalPotential_ = vCa; }
    float getCalciumReversalPotential() const { return calciumReversalPotential_; }
    
    void setCalciumTimeConstant(float tauCa) { calciumTimeConstant_ = tauCa; }
    float getCalciumTimeConstant() const { return calciumTimeConstant_; }
    
    // Calcium dynamics
    float getInternalCalcium() const { return internalCalcium_; }
    void setInternalCalcium(float ca) { internalCalcium_ = std::clamp(ca, 0.0f, 1.0f); }
    
    void updateCalcium(TimestepDuration dt) {
        // Simple calcium dynamics model
        // d[Ca]/dt = -[Ca]/tau + input
        float decay = internalCalcium_ / calciumTimeConstant_;
        setInternalCalcium(internalCalcium_ - decay * dt * 1000.0f + calciumInput_ * dt * 1000.0f);
    }
    
    void addCalciumInput(float amount) { calciumInput_ += amount; }
    void clearCalciumInput() { calciumInput_ = 0.0f; }
    
    // Override step function to include calcium dynamics
    void step(Timestamp currentTime) override {
        // Update calcium dynamics
        updateCalcium(0.001f);  // Assuming 1ms timestep
        
        // Calcium affects excitability and plasticity
        float calciumFactor = 1.0f + calciumEffect_ * internalCalcium_;
        
        // Apply calcium effects to membrane potential
        float currentPotential = getMembranePotential();
        // Calcium typically causes depolarization (increases potential)
        currentPotential *= (1.0f + (internalCalcium_ * calciumConductance_ * 0.01f));
        
        // Use base step but with calcium-modified potential
        setMembranePotential(currentPotential);
        
        // Calcium also affects threshold adaptation
        float calciumThresholdMod = calciumThresholdModulation_ * internalCalcium_;
        float originalThreshold = getThreshold();
        setThreshold(originalThreshold + calciumThresholdMod);
        
        // Call base step with modified calcium effects
        Neuron::step(currentTime);
        
        // Reset threshold after step
        setThreshold(originalThreshold);
        
        clearCalciumInput();
    }
    
    // Methods for plasticity modulation by calcium
    float getCalciumPlasticityFactor() const {
        return 1.0f + calciumPlasticityStrength_ * internalCalcium_;
    }
    
    void setCalciumPlasticityStrength(float strength) { calciumPlasticityStrength_ = strength; }
    
private:
    void resetCalciumParameters() {
        calciumConductance_ = 1.0f;       // nS
        calciumReversalPotential_ = 100.0f;  // mV
        calciumTimeConstant_ = 50.0f;    // ms
        internalCalcium_ = 0.0f;
        calciumInput_ = 0.0f;
        calciumEffect_ = 0.02f;          // Effect on excitability
        calciumThresholdModulation_ = 2.0f;  // Effect on threshold
        calciumPlasticityStrength_ = 0.5f;   // Effect on plasticity
    }
    
    void recordCalciumParameters() {
        // Record parameters for experimental tracking
    }
    
    // Calcium dynamics parameters
    float calciumConductance_;             // Calcium channel conductance (nS)
    float calciumReversalPotential_;       // Calcium reversal potential (mV)
    float calciumTimeConstant_;            // Calcium decay time constant (ms)
    float internalCalcium_;                // Internal calcium concentration (normalized)
    float calciumInput_;                   // Calcium input this step
    float calciumEffect_;                  // Effect of calcium on excitability
    float calciumThresholdModulation_;     // Effect of calcium on threshold
    float calciumPlasticityStrength_;      // Effect of calcium on plasticity
};

} // namespace nlm