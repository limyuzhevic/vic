// NeuralDynamics - Real integrate-and-fire neuron dynamics
// Implements LIF, Hodgkin-Huxley, and FitzHugh-Nagumo dynamics

#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"

namespace nlm {

// Neuron dynamics interface
class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    // Update neuron dynamics for one timestep
    virtual bool update(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) = 0;
    
    // Get dynamics type name
    virtual const char* getTypeName() const = 0;
    
    // Reset dynamics state
    virtual void reset(Neuron* neuron) = 0;
};

// Synaptic dynamics interface  
class SynapticDynamics {
public:
    virtual ~SynapticDynamics() = default;
    
    // Update synaptic state for one timestep
    virtual void update(double currentTime, TimestepDuration dt) = 0;
    
    // Get efficacy based on recent activity
    virtual float getEfficacy() const = 0;
    
    // Reset to baseline state
    virtual void reset() = 0;
};

// Implement different neuron models
class LIFDynamics : public NeuralDynamics {
public:
    bool update(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) override;
    const char* getTypeName() const override { return "LIF"; }
    void reset(Neuron* neuron) override;
};

class HHNDynamics : public NeuralDynamics {
public:
    bool update(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) override;
    const char* getTypeName() const override { return "Hodgkin-Huxley"; }
    void reset(Neuron* neuron) override;
};

class FHNDynamics : public NeuralDynamics {
public:
    bool update(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) override;
    const char* getTypeName() const override { return "FitzHugh-Nagumo"; }
    void reset(Neuron* neuron) override;
};

// Synaptic dynamics implementation (Tsodyks-Markram model)
class TSMDSynapticDynamics : public SynapticDynamics {
public:
    TSMDSynapticDynamics();
    void update(double currentTime, TimestepDuration dt) override;
    float getEfficacy() const override;
    void reset() override;
    
    // Configure synaptic parameters
    void configure(float u_max, float tau_d, float tau_f, float x_0, float y_0);
    
    // Record pre-synaptic spike
    void recordPreSpike(double time);
    
    // Record post-synaptic spike  
    void recordPostSpike(double time);
    
    // Get utilization factor
    float getUtilization() const { return u_; }
    
    // Get facilitation variable
    float getFacilitation() const { return x_; }
    
    // Get depression variable
    float getDepression() const { return y_; }
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
