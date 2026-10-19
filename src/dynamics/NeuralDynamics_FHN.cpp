#include "NeuralDynamics.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <cmath>
#include <algorithm>
#include <iostream>

namespace nlm {

class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics() {
        reset();
    }
    
    ~FitzHughNagumoDynamics() override = default;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {
        if (!neuron) return;
        
        const NeuronState& state = neuron->getState();
        float V = neuron->getMembranePotential();
        
        // FHN dynamics: dv/dt = 0.5*(v-w+V)-dw/dt = (v+a-b)/tau*w + I
        // where v is membrane potential, w is recovery variable
        
        // Synaptic input as external current
        float I_syn = neuron->getTotalCurrent() * 0.01f; // Scale factor
        
        // Compute derivatives
        float dv_dt = 0.5f * (v_ - w_ + V);  // dv/dt = 0.5*(v-w+V)
        float dw_dt = (v_ + a_ - b_ * w_) / tau_ + I_syn;  // dw/dt = (v+a-b)/tau*w + I
        
        // Euler integration
        v_ += dv_dt * dt * 0.001f; // dt in ms
        w_ += dw_dt * dt * 0.001f;
        
        // Update membrane potential
        neuron->setMembranePotential(v_ * 100.0f); // Convert to mV
        
        // Check for spike threshold (FHN typically spikes around +40mV)
        if (v_ > threshold_) {
            neuron->recordSpike(0.0); // TODO: pass actual time
            neuron->setFiringState(FiringState::Active);
            
            // Reset recovery variable after spike
            w_ = w_reset_;
        }
            
            // Reset recovery variable after spike
            w_ = w_reset_;
        }
        
        // Apply spike-frequency adaptation (slow recovery)
        if (adaptation_counter_ > 0) {
            w_ += adaptation_rate_ * adaptation_counter_;
            adaptation_counter_ = std::max(0, adaptation_counter_ - 1);
        }
        
        // Clear synaptic current
        neuron->clearTotalCurrent();
    }
    
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {
        if (!synapse) return;
        
        // Simple synaptic dynamics for FHN
        // Apply exponential decay to synaptic efficacy
        float currentEfficacy = synapse->getEfficacy();
        float decay = std::exp(-dt * 0.001f / tau_s_); // Convert dt to ms
        float newEfficacy = currentEfficacy * decay;
        synapse->setEfficacy(newEfficacy);
        
        // Update eligibility trace for plasticity
        float eligibility = synapse->getEligibilityTrace();
        eligibility *= decay;
        synapse->setEligibilityTrace(eligibility);
    }
    
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {
        if (!neuron || !synapse) return;
        
        // Apply synaptic input based on synapse type and weight
        SynapticWeight weight = synapse->getWeight();
        
        // FHN-specific synaptic scaling
        float synWeight = weight * synapse->getEfficacy();
        
        if (synapse->isExcitatory()) {
            // Excitatory synapses increase membrane potential
            float excitatoryInput = synWeight * 0.1f; // Scale factor
            neuron->receiveExcitatoryInput(excitatoryInput);
        } else if (synapse->isInhibitory()) {
            // Inhibitory synapses decrease membrane potential
            float inhibitoryInput = synWeight * 0.1f; // Scale factor
            neuron->receiveInhibitoryInput(inhibitoryInput);
        }
    }
    
    bool shouldFire(const Neuron* neuron) const override {
        if (!neuron) return false;
        return neuron->checkThreshold();
    }
    
    void reset() override {
        // Reset FHN state variables
        v_ = state_resting_v_;
        w_ = compute_w_steady_state(state_resting_v_);
        
        // Reset internal state
        last_spike_time_ = -1.0f;
        adaptation_counter_ = 0;
    }
    
    // FHN parameter configuration methods
    void setVoltageReset(float w_reset) { w_reset_ = w_reset; }
    void setRecoveryTimeConstant(float tau) { tau_ = tau; }
    void setThreshold(float threshold) { threshold_ = threshold; }
    void setExcitability(float a) { a_ = a; }
    void setThresholdParameter(float b) { b_ = b; }
    void setSynapticTimeConstant(float tau_s) { tau_s_ = tau_s; }
    void setAdaptationRate(float rate) { adaptation_rate_ = rate; }
    
    // Get parameter values
    float getVoltageReset() const { return w_reset_; }
    float getRecoveryTimeConstant() const { return tau_; }
    float getThreshold() const { return threshold_; }
    float getExcitability() const { return a_; }
    float getThresholdParameter() const { return b_; }
    
private:
    // FitzHugh-Nagumo state variables
    float v_;   // Membrane potential
    float w_;   // Recovery variable
    
    // Model parameters
    float w_reset_;    // Recovery variable after spike
    float tau_;        // Recovery time constant (ms)
    float threshold_;  // Spike threshold
    float a_;          // Excitability parameter
    float b_;          // Threshold parameter
    float tau_s_;      // Synaptic time constant (ms)
    float adaptation_rate_; // Spike-frequency adaptation rate
    
    float state_resting_v_; // Steady-state membrane potential (mV)
    
    // Internal state tracking
    float last_spike_time_;
    int adaptation_counter_;
    
    // Helper function for steady-state recovery variable
    float compute_w_steady_state(float v) const {
        return v + a_ - b_;
    }
};

} // namespace nlm