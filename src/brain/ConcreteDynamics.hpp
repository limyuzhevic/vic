#pragma once

#include "../brain/Neuron.hpp"
#include "../core/Random/Random.hpp"
#include <cmath>

namespace nlm {

// Concrete implementation of LIF neuron dynamics
class LIFNeuronDynamics : public NeuronDynamics {
public:
    LIFNeuronDynamics() = default;
    ~LIFNeuronDynamics() override = default;
    
    void initialize(const NeuronState& state) override {
        currentTime_ = 0.0f;
        // Store neuron parameters for LIF integration
        restingPotential_ = state.restingPotential;
        threshold_ = state.threshold;
        resetPotential_ = state.resetPotential;
        leakConductance_ = state.leakConductance;
        membraneCapacitance_ = 1.0f;  // nF
        timeConstant_ = 20.0f;       // ms
    }
    
    bool update(MembranePotential& membranePotential, 
                FiringRate& firingRate,
                Timestamp currentTime,
                TimestepDuration dt) override {
        currentTime_ = currentTime;
        
        // LIF dynamics: Leaky Integrate-and-Fire
        // dV/dt = (V_rest - V)/tau + I/C
        MembranePotential& V = membranePotential;
        
        // Leak contribution
        float leakContribution = (restingPotential_ - V) / timeConstant_;
        
        // Synaptic input would be added by caller
        // For now, simulate some input
        float synapticContribution = 0.0f; // Would come from synaptic input
        
        // Update membrane potential
        V = V + static_cast<float>(dt) * 1000.0f * leakContribution + synapticContribution;
        
        // Clamp to prevent instability
        V = std::clamp(V, -100.0f, 50.0f);
        
        // Check for spike
        if (V >= threshold_) {
            firingRate = 1000.0f;  // 1 kHz firing rate when spiking
            return true;
        }
        
        firingRate = 0.0f;
        return false;
    }
    
    void onSpike(Timestamp currentTime) override {
        // Handle post-spike dynamics
        // Reset would be handled by caller
        spikeCount_++;
    }
    
    void reset() override {
        // Reset dynamics
        currentTime_ = 0.0f;
        spikeCount_ = 0;
    }
    
    const std::string& getName() const override {
        static const std::string name = "LIFDynamics";
        return name;
    }
    
private:
    float restingPotential_ = -70.0f;
    float threshold_ = -55.0f;
    float resetPotential_ = -70.0f;
    float leakConductance_ = 10.0f;
    float membraneCapacitance_ = 1.0f;
    float timeConstant_ = 20.0f;
    float currentTime_ = 0.0f;
    uint32_t spikeCount_ = 0;
};

// Concrete implementation of STDP plasticity rule
class STDPPlasticityRule : public PlasticityRule {
public:
    STDPPlasticityRule() = default;
    ~STDPPlasticityRule() override = default;
    
    void initialize(const PlasticityFlags& flags) override {
        enabled_ = flags.stdp;
        learningRate_ = 0.01f;
        timeWindow_ = 20.0f;  // ms
        preSpikeTime_ = -1.0f;
        postSpikeTime_ = -1.0f;
    }
    
    void updateWeights(float& weight, 
                      float preSpikeTime,
                      float postSpikeTime,
                      float eligibilityTrace) override {
        if (!enabled_) return;
        
        // Store spike times for STDP
        preSpikeTime_ = preSpikeTime;
        postSpikeTime_ = postSpikeTime;
        
        // Calculate STDP weight change
        float deltaW = 0.0f;
        
        if (preSpikeTime_ >= 0 && postSpikeTime_ >= 0) {
            float dt = postSpikeTime_ - preSpikeTime_;
            
            if (dt > 0) {
                // Long-term potentiation (LTP)
                deltaW = learningRate_ * std::exp(-dt / timeWindow_);
            } else {
                // Long-term depression (LTD)
                deltaW = -learningRate_ * std::exp(dt / timeWindow_);
            }
        }
        
        // Apply weight change with eligibility trace
        weight += deltaW * eligibilityTrace;
    }
    
    void setLearningRate(float rate) override {
        learningRate_ = rate;
    }
    
    float getLearningRate() const override {
        return learningRate_;
    }
    
    const std::string& getName() const override {
        static const std::string name = "STDP";
        return name;
    }
    
    bool isEnabled() const override {
        return enabled_;
    }
    
private:
    bool enabled_ = false;
    float learningRate_ = 0.01f;
    float timeWindow_ = 20.0f;  // ms
    float preSpikeTime_ = -1.0f;
    float postSpikeTime_ = -1.0f;
};

// Concrete implementation of sensory processing
class BasicSensoryProcessor : public SensoryProcessor {
public:
    BasicSensoryProcessor() = default;
    ~BasicSensoryProcessor() override = default;
    
    MembranePotential processInput(const class SensoryInput& input,
                                  Timestamp timestamp) override {
        // Simple sensory processing based on input intensity
        float intensity = input.getIntensity();
        float gain = getGain();
        
        // Apply gain and convert to membrane potential
        return intensity * gain * 0.1f;  // Scale to mV
    }
    
    void setGain(float gain) override {
        gain_ = gain;
    }
    
    float getGain() const override {
        return gain_;
    }
    
    const std::string& getName() const override {
        static const std::string name = "BasicSensoryProcessor";
        return name;
    }
    
    bool isActive() const override {
        return active_;
    }
    
    void setActive(bool active) override {
        active_ = active;
    }
    
private:
    float gain_ = 1.0f;
    bool active_ = true;
};

} // namespace nlm
