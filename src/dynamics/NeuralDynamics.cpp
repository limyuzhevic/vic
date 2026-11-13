#include "NeuralDynamics.hpp"
#include <cmath>
#include <limits>
#include <stdexcept>

namespace nlm {

// Helper function for safe numerical operations
inline bool isFinite(float value) {
    return std::isfinite(value) && std::abs(value) < std::numeric_limits<float>::max();
}

// Helper function to validate neuron pointer
inline void validateNeuron(const Neuron* neuron, const char* context) {
    if (!neuron) {
        throw std::invalid_argument("Neuron pointer is null in " + std::string(context));
    }
}

// Helper function to validate synapse pointer  
inline void validateSynapse(const Synapse* synapse, const char* context) {
    if (!synapse) {
        throw std::invalid_argument("Synapse pointer is null in " + std::string(context));
    }
}

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Update neuron state using its existing LIF implementation
    // This delegates to the neuron's own stepLIF() method rather than
    // reimplementing LIF dynamics, avoiding code duplication
    
    validateNeuron(neuron, "updateNeuron");
    
    // Validate timestep duration
    if (dt <= 0.0f) {
        throw std::invalid_argument("Invalid timestep duration in updateNeuron: " + std::to_string(dt));
    }
    
    neuron->stepLIF(0.0, dt);  // 0.0 timestamp since we're not at real time
    
    // Clear any accumulated current after the step
    // neuron->clearTotalCurrent(); // This is called within stepLIF() in Neuron.cpp
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Delegate to synapse's existing step() implementation to avoid code duplication
    
    validateSynapse(synapse, "updateSynapse");
    
    // Validate timestep duration
    if (dt <= 0.0f) {
        throw std::invalid_argument("Invalid timestep duration in updateSynapse: " + std::to_string(dt));
    }
    
    synapse->step(0.0);  // 0.0 timestamp since we're not at real time
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply synaptic input using the neuron's existing input methods
    // This avoids reimplementing synaptic transmission logic
    
    validateNeuron(neuron, "applySpikeInput");
    validateSynapse(synapse, "applySpikeInput");
    
    SynapticWeight weight = synapse->getWeight();
    
    // Validate synaptic weight
    if (!isFinite(weight)) {
        throw std::invalid_argument("Invalid synaptic weight in applySpikeInput: " + std::to_string(weight));
    }
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    // Check if neuron should fire based on its membrane potential and threshold
    // This is handled by the neuron's checkThreshold() method
    
    validateNeuron(neuron, "shouldFire");
    
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

} // namespace nlm

namespace nlm {

// Hodgkin-Huxley dynamics implementation
struct HodgkinHuxleyDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;            // nF
    
    // HH gating variables
    float m;  // activation variable
    float h;  // inactivation variable  
    float n;  // potassium activation variable
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), capacitance(1.0f),
             m(0.05f), h(0.6f), n(0.32f) {}
};

HodgkinHuxleyDynamics::HodgkinHuxleyDynamics() : pImpl(new Impl) {}

HodgkinHuxleyDynamics::~HodgkinHuxleyDynamics() = default;

void HodgkinHuxleyDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Implement Hodgkin-Huxley dynamics for the neuron
    // This is a simplified HH model that captures the essential dynamics
    // of action potential generation
    
    validateNeuron(neuron, "updateNeuron (HH)");
    
    // Validate timestep duration
    if (dt <= 0.0f) {
        throw std::invalid_argument("Invalid timestep duration in HH updateNeuron: " + std::to_string(dt));
    }
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    
    // Validate membrane potential
    if (!isFinite(V) || V < -100.0f || V > 100.0f) {
        throw std::invalid_argument("Invalid membrane potential in HH updateNeuron: " + std::to_string(V));
    }
    
    // External current input
    float I_ext = neuron->getTotalCurrent();
    
    // Validate external current
    if (!isFinite(I_ext) || I_ext < -1000.0f || I_ext > 1000.0f) {
        throw std::invalid_argument("Invalid external current in HH updateNeuron: " + std::to_string(I_ext));
    }
    
    // Update HH gating variables using differential equations
    TimestepDuration dt_ms = static_cast<TimestepDuration>(dt * 1000.0f);  // Convert to ms
    
    float alpha_m = (2.5f - 0.1f * (V - 65.0f)) / (expf((2.5f - 0.1f * (V - 65.0f)) / 10.でf) - 1.0f);
    float beta_m = 4.0f * expf(-(V - 65.0f) / 18.0f);
    
    float alpha_h = 0.07f * expf(-(V - 65.0f) / 20.0f);
    float beta_h = 1.0f / (expf((30.0f - V) / 10.0f) + 1.0f);
    
    float alpha_n = (0.1f * (V - 65.0f) + 1.0f) / (expf((0.1f * (V - 65.0f) + 1.0f) / 10.0f) - 1.0f);
    float beta_n =  competes 0.125f * expf(-(V - 65.0f) / 80.0f);
    
    // Update gating variables
    pImpl->m += dt_ms * (alpha_m * (1.0f - pImpl->m) - beta_m * pImpl->m);
    pImpl->h += dt_ms * (alpha_h * (1.0f - pImpl->h) - beta_h * pImpl->h);
    pImpl->n += dt_ms * (alpha_n * (1.0f - pImpl->n) - beta_n * pImpl->n);
    
    // Clamp gating variables to [0, 1] with validation
    if (pImpl->m < 0.0f || pImpl->m > 1.0f || pImpl->h < 0.0f || pImpl->h > 1.0f || pImpl->n < 0.0f || pImpl->n > 1.0f) {
        pImpl->m = std::clamp(pImpl->m, 0.0f, 1.0f);
        pImpl->h = std::clamp(pImpl->h, 0.0f, 1.0f);
        pImpl->n = std::clamp(pImpl->n, 0.0f, 1.0f);
    }
    
    // Compute ionic currents
    float I_na = 120.0f * pImpl->m * pImpl->m * pImpl->m * pImpl->h * (V - 50.0f);
    float I_k = 36.0f * pImpl->n * pImpl->n * pImpl->n * (V - 77.0f);
    float I_leak = 0.3f * (V - 65.0f);
    
    // Validate ionic currents
    if (!isFinite(I_na) || !isFinite(I_k) || !isFinite(I_leak)) {
        throw std::runtime_error("Non-finite ionic currents in HH updateNeuron");
    }
    
    // Update membrane potential using HH equation: C*dV/dt = I_ext - I_na - I_k - I_leak
    float dV = (I_ext - I_na - I_k - I_leak) / pImpl->capacitance;
    neuron->setMembranePotential(V + dV * dt_ms / 1000.0f);
    
    // Check for spike threshold
    if (neuron->getMembranePotential() > 40.0f) {
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
}

void HodgkinHuxleyDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Synapse dynamics are the same for all neuron types
    // Delegate to synapse's existing step() implementation with validation
    
    validateSynapse(synapse, "updateSynapse (HH)");
    
    // Validate timestep duration
    if (dt <= 0.0f) {
        throw std::invalid_argument("Invalid timestep duration in HH updateSynapse: " + std::to_string(dt));
    }
    
    synapse->step(0.0);  // Delegate to synapse's implementation
}

void HodgkinHuxleyDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply synaptic input based on synapse type
    // Use neuron's existing input methods with validation
    
    validateNeuron(neuron, "applySpikeInput (HH)");
    validateSynapse(synapse, "applySpikeInput (HH)");
    
    SynapticWeight weight = synapse->getWeight();
    
    // Validate synaptic weight
    if (!isFinite(weight)) {
        throw std::invalid_argument("Invalid synaptic weight in HH applySpikeInput: " + std::to_string(weight));
    }
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool HodgkinHuxleyDynamics::shouldFire(const Neuron* neuron) const {
    // For HH dynamics, firing is determined by the membrane potential crossing threshold
    // Check if neuron should fire based on its membrane potential and threshold
    
    validateNeuron(neuron, "shouldFire (HH)");
    
    return neuron->getMembranePotential() >= 40.0f;
}

void HodgkinHuxleyDynamics::reset() {
    // Reset HH gating variables to initial values
    // Ensure pImpl is not null before accessing
    if (pImpl) {
        pImpl->m = 0.05f;
        pImpl->h = 0.6f;
        pImpl->n = 0.32f;
    }
}

void HodgkinHuxleyDynamics::setMembraneTimeConstant(float tau) {
    // Validate membrane time constant parameter
    if (!isFinite(tau) || tau <= 0.0f) {
        throw std::invalid_argument("Invalid membrane time constant in HH setMembraneTimeConstant: " + std::to_string(tau));
    }
    
    pImpl->membraneTimeConstant = tau;
}

float HodgkinHuxleyDynamics::getMembraneTimeConstant() const {
    // Return membrane time constant
    return pImpl->membraneTimeConstant;
}

// FitzHugh-Nagumo dynamics implementation
struct FitzHughNagumoDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    float capacitance;            // nF
    float recoveryTimeConstant;   // ms
    
    // FHN variables
    float recoveryVariable;       // w variable (slow recovery)
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f), capacitance(1.0f),
             recoveryTimeConstant(200.0f), recoveryVariable(0.0f) {}
};

FitzHughNagumoDynamics::FitzHughNagumoDynamics() : pImpl(new Impl) {}

FitzHughNagumoDynamics::~FitzHughNagumoDynamics() = default;

void FitzHughNagumoDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    // Implement FitzHugh-Nagumo dynamics (simplified excitation model)
    // This is a reduced 2-variable model that captures the essential
    // dynamics of excitable media and neural excitability
    
    const auto& state = neuron->getState();
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    
    // External current input
    float I_ext = neuron->getTotalCurrent();
    
    // FHN differential equations:
    // dv/dt = (V - V^3/3 - w + I_ext) / tau
    // dw/dt = (V + a - b*w) / tau_w
    
    TimestepDuration dt_ms = static_cast<TimestepDuration>(dt * 1000.0f);  // Convert to ms
    
    // Compute derivatives
    float dv_dt = (V - (V * V * V) / 3.0f - pImpl->recoveryVariable + I_ext) / pImpl->membraneTimeConstant;
    float dw_dt = (V + 0.7f - 0.8f * pImpl->recoveryVariable) / pImpl->recoveryTimeConstant;
    
    // Update variables
    float new_V = V + dv_dt * dt_ms;
    float new_w = pImpl->recoveryVariable + dw_dt * dt_ms;
    
    // Apply spike threshold and reset
    if (new_V > 0.5f) {
        // Spike detected - use reset potential
        new_V = -1.2f;  // Reset potential
        new_w = new_w + 0.1f;  // Add to recovery variable for next spike
        neuron->setFiringState(FiringState::Active);
        neuron->recordSpike(0.0);  // TODO: pass actual time
    }
    
    // Update neuron state
    neuron->setMembranePotential(new_V);
    pImpl->recoveryVariable = new_w;
}

void FitzHughNagumoDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Synapse dynamics are the same for all neuron types
    // Delegate to synapse's existing step() implementation with validation
    
    validateSynapse(synapse, "updateSynapse (FHN)");
    
    // Validate timestep duration
    if (dt <= 0.0f) {
        throw std::invalid_argument("Invalid timestep duration in FHN updateSynapse: " + std::to_string(dt));
    }
    
    synapse->step(0.0);  // Delegate to synapse's implementation
}

void FitzHughNagumoDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // Apply synaptic input based on synapse type
    // Use neuron's existing input methods with validation
    
    validateNeuron(neuron, "applySpikeInput (FHN)");
    validateSynapse(synapse, "applySpikeInput (FHN)");
    
    SynapticWeight weight = synapse->getWeight();
    
    // Validate synaptic weight
    if (!isFinite(weight)) {
        throw std::invalid_argument("Invalid synaptic weight in FHN applySpikeInput: " + std::to_string(weight));
    }
    
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool FitzHughNagumoDynamics::shouldFire(const Neuron* neuron) const {
    // For FHN dynamics, firing is determined by crossing the threshold
    // Check if neuron should fire based on its membrane potential and threshold
    
    validateNeuron(neuron, "shouldFire (FHN)");
    
    return neuron->getMembranePotential() >= 0.5f;
}

void FitzHughNagumoDynamics::reset() {
    // Reset FHN variables to initial values
    // Ensure pImpl is not null before accessing
    if (pImpl) {
        pImpl->recoveryVariable = 0.0f;
    }
}

void FitzHughNagumoDynamics::setMembraneTimeConstant(float tau) {
    // Validate membrane time constant parameter
    if (!isFinite(tau) || tau <= 0.0f) {
        throw std::invalid_argument("Invalid membrane time constant in FHN setMembraneTimeConstant: " + std::to_string(tau));
    }
    
    pImpl->membraneTimeConstant = tau;
}

float FitzHughNagumoDynamics::getMembraneTimeConstant() const {
    // Return membrane time constant
    return pImpl->membraneTimeConstant;
}

} // namespace nlm
