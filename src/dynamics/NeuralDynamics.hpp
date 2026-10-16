#include "../core/Types/Types.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include <memory>
#include <algorithm>

namespace nlm {

class NeuralDynamics {
public:
    virtual ~NeuralDynamics() = default;
    
    virtual void updateNeuron(Neuron* neuron, TimestepDuration dt) = 0;
    virtual void updateSynapse(Synapse* synapse, TimestepDuration dt) = 0;
    virtual void applySpikeInput(Neuron* neuron, const Synapse* synapse) = 0;
    virtual bool shouldFire(const Neuron* neuron) const = 0;
    virtual void reset() = 0;
};

class IntegrateAndFireDynamics : public NeuralDynamics {
public:
    IntegrateAndFireDynamics() : timeConstant(20.0f), resistance(10.0f), thresholdFactor(1.0f) {
        initializeConstants();
    }
    
    ~IntegrateAndFireDynamics() override = default;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {
        if (!neuron) {
            return;
        }
        
        const auto& state = neuron->getState();
        float V = neuron->getMembranePotential();
        
        // Enhanced integrate-and-fire dynamics with membrane time constant
        // dV/dt = (-(V - V_rest) + I * R) / tau
        float V_rest = state.restingPotential;
        float I = neuron->getTotalCurrent();
        float tau = timeConstant;
        
        // Use exponential integration for better stability: V(t) = V_rest + (V - V_rest) * exp(-t/tau) + I * R * (1 - exp(-t/tau))
        float alpha = std::exp(-dt / tau);
        float I_term = I * resistance * (1.0f - alpha);
        
        neuron->setMembranePotential(V_rest + (V - V_rest) * alpha + I_term);
        
        // Check for firing with hysteresis
        if (shouldFire(neuron)) {
            neuron->setFiringState(FiringState::Active);
            neuron->recordSpike(dt);  // Use actual dt for time tracking
            neuron->setMembranePotential(2.0f * state.threshold); // Supra-threshold peak
        }
        
        // Refractory mechanism with exponential recovery
        if (neuron->isRefractory()) {
            float recoveryFactor = neuron->getRefractoryRemaining() / 
                                  static_cast<float>(neuron->getRefractoryPeriod());
            neuron->setMembranePotential(state.resetPotential * (1.0f - recoveryFactor));
        }
        
        // Apply leak conductance for more realistic dynamics
        float leakFactor = 1.0f - dt / (state.leakConductance * 1000.0f); // Convert nS to standard units
        float V_leak = neuron->getMembranePotential();
        float V_leak_target = state.restingPotential;
        neuron->setMembranePotential(V_leak_target + (V_leak - V_leak_target) * leakFactor);
        
        // Clear current for next step
        neuron->clearTotalCurrent();
    }
    
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {
        if (!synapse) {
            return;
        }
        
        // Implement realistic synaptic dynamics with short-term plasticity
        float weight = synapse->getWeight();
        float efficacy = synapse->getEfficacy();
        
        // Synaptic depression/facilitation model
        float adaptationFactor = 1.0f + (efficacy - 1.0f) * dt * 0.1f;
        float newWeight = weight * std::max(0.0f, std::min(1.0f, adaptationFactor));
        
        synapse->setWeight(newWeight);
        synapse->step(dt); // Update eligibility traces
    }
    
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {
        if (!neuron || !synapse) {
            return;
        }
        
        SynapticWeight weight = synapse->getWeight();
        float efficacy = synapse->getEfficacy();
        
        // Biologically realistic synaptic transmission
        // Stronger synapses produce larger postsynaptic potentials
        float transmissionFactor = efficacy * weight;
        
        if (synapse->isExcitatory()) {
            neuron->receiveExcitatoryInput(transmissionFactor * 0.8f); // Scaling factor for realistic PSPs
        } else if (synapse->isInhibitory()) {
            neuron->receiveInhibitoryInput(transmissionFactor * 0.5f); // Different scaling for inhibition
        }
        
        // Record spike history for STDP
        const auto& state = neuron->getState();
        neuron->recordSpike(dt); // Use actual dt for spike timing
    }
    
    bool shouldFire(const Neuron* neuron) const override {
        if (!neuron) return false;
        
        const auto& state = neuron->getState();
        float V = neuron->getMembranePotential();
        
        // Firing threshold with adaptive dynamics
        float dynamicThreshold = state.threshold * thresholdFactor;
        
        // Incorporate adaptation variable for spike-frequency adaptation
        float adaptationThreshold = dynamicThreshold + state.adaptationVariable * 0.5f;
        
        return V >= adaptationThreshold;
    }
    
    void reset() override {
        // Reset internal time constants
        initializeConstants();
    }
    
    // Public parameter setters/getters
    void setMembraneTimeConstant(float tau) {
        timeConstant = std::max(1.0f, std::min(100.0f, tau)); // Clamp to reasonable range
    }
    
    float getMembraneTimeConstant() const {
        return timeConstant;
    }
    
    void setMembraneResistance(float R) {
        resistance = std::max(1.0f, std::min(1000.0f, R)); // Clamp to reasonable range
    }
    
    float getMembraneResistance() const {
        return resistance;
    }
    
    void setThresholdFactor(float factor) {
        thresholdFactor = std::max(0.5f, std::min(3.0f, factor));
    }
    
    float getThresholdFactor() const {
        return thresholdFactor;
    }
    
private:
    void initializeConstants() {
        // Initialize with biologically plausible values
        timeConstant = 20.0f;   // Membrane time constant in ms
        resistance = 10.0f;     // Membrane resistance in MOhm
        thresholdFactor = 1.0f; // Threshold scaling factor
    }
    
    float timeConstant;
    float resistance;
    float thresholdFactor;
};

class HodgkinHuxleyDynamics : public NeuralDynamics {
public:
    HodgkinHuxleyDynamics() {
        // Initialize HH parameters (biologically realistic values)
        gNa = 120.0f;      // Sodium conductance (mS/cm^2)
        gK = 36.0f;       // Potassium conductance (mS/cm^2)
        gL = 0.3f;        // Leak conductance (mS/cm^2)
        EL = -54.387f;     // Leak reversal potential (mV)
        VNa = 50.0f;      // Sodium reversal potential (mV)
        VK = -77.0f;      // Potassium reversal potential (mV)
        Cm = 1.0f;        // Membrane capacitance (μF/cm^2)
        
        // Initialize gating variables
        m = 0.05f;        // Sodium activation
        h = 0.6f;         // Sodium inactivation
        n = 0.32f;        // Potassium activation
    }
    
    ~HodgkinHuxleyDynamics() override = default;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {
        if (!neuron) {
            return;
        }
        
        const auto& state = neuron->getState();
        float V = neuron->getMembranePotential();
        
        // Update gating variables with standard HH equations
        float alpha_m = (2.5f - 0.1f * V) / (std::exp(2.5f - 0.1f * V) - 1.0f);
        float beta_m = 4.0f * std::exp(V / 18.0f);
        float alpha_h = 0.07f * std::exp(V / 20.0f);
        float beta_h = 1.0f / (std::exp(3.0f - 0.1f * V) + 1.0f);
        float alpha_n = (0.1f - 0.01f * V) / (std::exp(1.0f - 0.1f * V) - 1.0f);
        float beta_n = 0.125f * std::exp(-V / 80.0f);
        
        float dm_dt = alpha_m * (1.0f - m) - beta_m * m;
        float dh_dt = alpha_h * (1.0f - h) - beta_h * h;
        float dn_dt = alpha_n * (1.0f - n) - beta_n * n;
        
        // Update gating variables
        m += dm_dt * dt;
        h += dh_dt * dt;
        n += dn_dt * dt;
        
        // Clamp gating variables
        m = std::max(0.0f, std::min(1.0f, m));
        h = std::max(0.0f, std::min(1.0f, h));
        n = std::max(0.0f, std::min(1.0f, n));
        
        // Calculate ion currents
        float INa = gNa * m * m * m * h * (V - VNa);
        float IK = gK * n * n * n * n * (V - VK);
        float IL = gL * (V - EL);
        
        // Update membrane potential using HH equation: C * dV/dt = -INa - IK - IL + I_external
        float I_external = neuron->getTotalCurrent();
        float dV_dt = (-INa - IK - IL + I_external) / Cm;
        
        neuron->setMembranePotential(V + dV_dt * dt);
        
        // Check for firing with HH dynamics
        if (shouldFire(neuron)) {
            neuron->setFiringState(FiringState::Active);
            neuron->recordSpike(dt);
        }
        
        // Update adaptation variable for spike-frequency adaptation
        float adaptation_dt = neuron->isFiring() ? 0.01f : -0.001f;
        state.adaptationVariable += adaptation_dt;
        
        // Clear external current
        neuron->clearTotalCurrent();
    }
    
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {
        // HH synapses don't have specific dynamics in this simplified model
        if (synapse) {
            synapse->step(dt);
        }
    }
    
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {
        if (!neuron || !synapse) {
            return;
        }
        
        SynapticWeight weight = synapse->getWeight();
        
        // HH neurons receive standard synaptic input
        if (synapse->isExcitatory()) {
            neuron->receiveExcitatoryInput(weight * 1.5f);
        } else if (synapse->isInhibitory()) {
            neuron->receiveInhibitoryInput(weight * 0.8f);
        }
        
        neuron->recordSpike(dt);
    }
    
    bool shouldFire(const Neuron* neuron) const override {
        if (!neuron) return false;
        
        float V = neuron->getMembranePotential();
        
        // HH firing threshold (typically around 0 mV)
        return V >= 0.0f;
    }
    
    void reset() override {
        // Reset gating variables to initial values
        m = 0.05f;
        h = 0.6f;
        n = 0.32f;
    }
    
private:
    // HH parameters
    float gNa;      // Sodium conductance
    float gK;       // Potassium conductance
    float gL;       // Leak conductance
    float EL;       // Leak reversal potential
    float VNa;      // Sodium reversal potential
    float VK;       // Potassium reversal potential
    float Cm;       // Membrane capacitance
    
    // Gating variables
    float m;        // Sodium activation
    float h;        // Sodium inactivation
    float n;        // Potassium activation
};

class FitzHughNagumoDynamics : public NeuralDynamics {
public:
    FitzHughNagumoDynamics() {
        // FHN parameters for excitable dynamics
        a = 0.7f;      // Recovery variable parameter
        b = 0.8f;      // Recovery variable parameter
        c = 0.7f;      // Time scale separation
        I_th = 0.5f;   // Threshold current
    }
    
    ~FitzHughNagumoDynamics() override = default;
    
    void updateNeuron(Neuron* neuron, TimestepDuration dt) override {
        if (!neuron) {
            return;
        }
        
        const auto& state = neuron->getState();
        float V = neuron->getMembranePotential();
        float W = state.adaptationVariable; // Use adaptation variable as recovery variable
        
        // FHN equations:
        // dV/dt = c * (V - V^3/3 + W + I)
        // dW/dt = (V - a*W + b) / c
        
        float I_ext = neuron->getTotalCurrent();
        float I = I_ext / 100.0f + I_th; // Normalize and add threshold
        
        float dV_dt = c * (V - (V * V * V) / 3.0f + W + I);
        float dW_dt = (V - a * W + b) / c;
        
        // Update variables
        neuron->setMembranePotential(V + dV_dt * dt);
        state.adaptationVariable += dW_dt * dt;
        
        // Check for firing (threshold crossing)
        if (shouldFire(neuron)) {
            neuron->setFiringState(FiringState::Active);
            neuron->recordSpike(dt);
        }
        
        // Refractory and leak effects
        if (neuron->isRefractory()) {
            float V_leak = neuron->getMembranePotential();
            neuron->setMembranePotential(V_leak * 0.5f + state.resetPotential * 0.5f);
        }
        
        neuron->clearTotalCurrent();
    }
    
    void updateSynapse(Synapse* synapse, TimestepDuration dt) override {
        if (synapse) {
            synapse->step(dt);
        }
    }
    
    void applySpikeInput(Neuron* neuron, const Synapse* synapse) override {
        if (!neuron || !synapse) {
            return;
        }
        
        SynapticWeight weight = synapse->getWeight();
        
        if (synapse->isExcitatory()) {
            neuron->receiveExcitatoryInput(weight * 2.0f);
        }
        
        neuron->recordSpike(dt);
    }
    
    bool shouldFire(const Neuron* neuron) const override {
        if (!neuron) return false;
        
        float V = neuron->getMembranePotential();
        
        // FHN firing threshold (typically around +2 mV)
        return V >= 2.0f;
    }
    
    void reset() override {
        // Reset FHN state variables
        a = 0.7f;
        b = 0.8f;
        c = 0.7f;
    }
    
private:
    // FHN parameters
    float a;      // Recovery variable parameter
    float b;      // Recovery variable parameter
    float c;      // Time scale separation
    float I_th;   // Threshold current
};

} // namespace nlm
