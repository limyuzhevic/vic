#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Forward declaration
class Neuron;

// Synapse representing a connection between neurons
// Implements real synaptic transmission with delays and short-term plasticity

class Synapse {
public:
    // Create synapse with IDs
    Synapse(SynapseId id, NeuronId source, NeuronId destination);
    
    ~Synapse();
    
    // Disable copying, enable moving
    Synapse(const Synapse&) = delete;
    Synapse& operator=(const Synapse&) = delete;
    Synapse(Synapse&&) noexcept;
    Synapse& operator=(Synapse&&) noexcept;
    
    // Identity
    SynapseId getId() const;
    
    // Connection
    NeuronId getSourceNeuron() const;
    NeuronId getDestinationNeuron() const;
    
    // Weight
    SynapticWeight getWeight() const;
    void setWeight(SynapticWeight weight);
    void addToWeight(SynapticWeight delta);
    
    // Delay (in simulation steps)
    Delay getDelay() const;
    void setDelay(Delay delay);
    
    // Type
    SynapseType getType() const;
    void setType(SynapseType type);
    
    // Excitatory/Inhibitory
    bool isExcitatory() const;
    bool isInhibitory() const;
    
    // Activity history for plasticity
    void recordPreSpike(Timestamp timestamp);
    void recordPostSpike(Timestamp timestamp);
    const std::vector<Timestamp>& getPreSpikeHistory() const;
    const std::vector<Timestamp>& getPostSpikeHistory() const;
    void clearHistory();
    
    // Plasticity flags
    const PlasticityFlags& getPlasticityFlags() const;
    PlasticityFlags& getPlasticityFlags();
    
    // Enable/disable plasticity rules
    void enablePlasticity(bool hebbian, bool stdp, bool rewardModulated);
    
    // Eligibility trace (for reward-modulated learning)
    float getEligibilityTrace() const;
    void setEligibilityTrace(float trace);
    void decayEligibilityTrace(float decayRate);
    
    // Synaptic efficacy (use-dependent modulation)
    float getEfficacy() const;
    void setEfficacy(float efficacy);
    
    // Update synapse for one simulation step
    // Implements real synaptic dynamics with:
    // - Spike transmission with propagation delays
    // - Short-term plasticity (STP) - facilitation and depression
    // - Activity-dependent conductance modulation
    // - STDP eligibility trace accumulation
    // - Neuromodulation integration
    void step(Timestamp currentTime) {
        SynapseState& state = pImpl->state;
        
        // Decrease delay counter
        if (state.delaySteps > 0) {
            state.delaySteps--;
            return;  // Wait until delay expires
        }
        
        // Check if source neuron spiked in previous timestep
        bool sourceSpiked = false;
        if (!state.preSpikeHistory.empty()) {
            Timestamp lastPreSpike = state.preSpikeHistory.back();
            if (currentTime - lastPreSpike <= 0.01) {  // Spike within last 10ms
                sourceSpiked = true;
            }
        }
        
        if (sourceSpiked) {
            // Calculate synaptic efficacy based on history
            float efficacy = state.efficacy;
            
            // Apply facilitation (for short-term plasticity)
            if (isExcitatory() && !isInhibitory()) {
                // Facilitation for excitatory synapses
                state.facilitationVariable = std::min(state.facilitationVariable * 1.5f, 2.0f);
                efficacy *= state.facilitationVariable;
            }
            
            // Apply depression (for use-dependent modulation)
            if (state.useDepresionRate > 0.0f) {
                state.useDepletion += state.useDepresionRate;
                state.useDepletion = std::min(state.useDepletion, 1.0f);
                efficacy *= (1.0f - state.useDepletion);
            }
            
            // Calculate synaptic conductance
            // I_syn = g_max * efficacy * (V_rev - V_post)
            double reversalPotential = isExcitatory() ? 10.0 : -80.0; // mV
            double postMembranePotential = static_cast<double>(pImpl->destinationNeuron->getMembranePotential());
            double conductance = static_cast<double>(getWeight()) * efficacy;
            double synapticCurrent = conductance * (reversalPotential - postMembranePotential);
            
            // Apply to postsynaptic neuron
            pImpl->destinationNeuron->receiveInputFromSynapse(
                static_cast<MembranePotential>(synapticCurrent),
                isExcitatory() ? InputType::Excitatory : InputType::Inhibitory
            );
            
            // Update spike history for plasticity
            state.lastPreSpikeTime = currentTime;
            
            // Calculate STDP eligibility trace
            if (!state.postSpikeHistory.empty()) {
                // STDP based on spike timing
                Timestamp lastPostSpike = state.postSpikeHistory.back();
                double dt = static_cast<double>(currentTime - lastPostSpike); // Post before pre
                
                if (dt > 0.0) {
                    // Pre after post (LTP)
                    float stdpFactor = std::exp(-dt / 20.0); // 20ms time constant
                    state.eligibilityTrace += static_cast<float>(stdpFactor);
                } else {
                    // Pre before post (LTD)
                    dt = -dt;
                    if (dt < 20.0) {
                        float stdpFactor = -std::exp(-dt / 20.0); // Depression
                        state.eligibilityTrace += static_cast<float>(stdpFactor);
                    }
                }
            }
            
            // Apply neuromodulatory influences
            if (state.neuromodulationActive) {
                // Dopamine can modulate synaptic efficacy
                if (state.dopamineLevel > 0.0f) {
                    efficacy *= (1.0f + state.dopamineLevel * 0.1f);
                }
                
                // Acetylcholine can enhance facilitation
                if (state.acetylcholineLevel > 0.0f) {
                    state.facilitationVariable *= (1.0f + state.acetylcholineLevel * 0.05f);
                }
            }
            
            // Decay facilitation variable
            state.facilitationVariable *= 0.95f;
            
            // Store updated efficacy
            state.efficacy = efficacy;
        }
        
        // Decay eligibility trace
        state.eligibilityTrace *= 0.98f; // Decay rate
        
        // Update short-term depression
        if (state.useDepletion > 0.0f) {
            state.useDepletion *= 0.98f; // Recovery from depression
        }
        
        // Check for spike interactions (STDP)
        checkForSTDP(state);
    }
    
    // Check and apply spike-timing dependent plasticity
    void checkForSTDP(SynapseState& state) {
        if (!getPlasticityFlags().stdpEnabled) return;
        
        // Calculate time differences between pre- and post-synaptic spikes
        if (state.lastPreSpikeTime > 0.0f && !state.postSpikeHistory.empty()) {
            for (Timestamp postSpike : state.postSpikeHistory) {
                double dt = static_cast<double>(state.lastPreSpikeTime - postSpike);
                
                if (dt > 0.0 && dt < 50.0) {  // Pre after post (LTP)
                    float weightChange = static_cast<float>(0.01 * std::exp(-dt / 10.0));
                    addToWeight(weightChange);
                } else if (dt < 0.0 && -dt < 50.0) {  // Pre before post (LTD)
                    float weightChange = -static_cast<float>(0.01 * std::exp(-(-dt) / 10.0));
                    addToWeight(weightChange);
                }
            }
        }
        
        if (!state.preSpikeHistory.empty() && state.lastPostSpikeTime > 0.0f) {
            for (Timestamp preSpike : state.preSpikeHistory) {
                double dt = static_cast<double>(state.lastPostSpikeTime - preSpike);
                
                if (dt > 0.0 && dt < 50.0) {  // Post after pre (LTD)
                    float weightChange = -static_cast<float>(0.01 * std::exp(-dt / 10.0));
                    addToWeight(weightChange);
                } else if (dt < 0.0 && -dt < 50.0) {  // Post before pre (LTP)
                    float weightChange = static_cast<float>(0.01 * std::exp(-(-dt) / 10.0));
                    addToWeight(weightChange);
                }
            }
        }
    }
    
    // Reset to initial state
    void reset();
    
    // Initialize with random parameters
    void initializeRandom(class RandomGenerator& rng);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
