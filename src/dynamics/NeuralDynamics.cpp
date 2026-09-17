#include "NeuralDynamics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// LIF Dynamics Implementation
bool LIFDynamics::update(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) {
    // Real Leaky Integrate-and-Fire dynamics
    bool fired = false;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    float V_rest = state.restingPotential;
    float threshold = state.threshold;
    float resetPotential = state.resetPotential;
    float leakConductance = state.leakConductance;
    
    // Handle refractory period
    if (state.refractoryRemaining > 0) {
        --state.refractoryRemaining;
        if (state.refractoryRemaining == 0) {
            neuron->setFiringState(FiringState::Resting);
        }
        return false;
    }
    
    // LIF equation: dV/dt = (V_rest - V)/tau + I/C
    // Discretized with exponential Euler for stability
    float tau = 1.0f / (leakConductance / neuron->getMembraneCapacitance());
    
    // Get synaptic input
    float synapticCurrent = neuron->getTotalCurrent();
    
    // Exponential Euler integration
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    float beta = leakConductance / neuron->getMembraneCapacitance();
    
    // Update membrane potential
    float V_new = V * alpha + (V_rest + synapticCurrent / leakConductance) * (1.0f - alpha);
    
    // Apply spike-frequency adaptation
    if (state.adaptationVariable > 0.0f) {
        V_new -= state.adaptationVariable * 0.01f;
        state.adaptationVariable *= 0.95f; // Decay adaptation
    }
    
    // Clamp to prevent instability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    
    // Check for spike
    if (V_new >= threshold) {
        fired = true;
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(resetPotential);
        neuron->setLastSpikeTime(currentTime);
        neuron->recordSpike(currentTime);
        
        // Enter refractory period
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
        
        // Update adaptation for spike-frequency adaptation
        state.adaptationVariable += 1.0f;
    } else {
        neuron->setMembranePotential(V_new);
        neuron->setFiringState(FiringState::Active);
    }
    
    // Clear synaptic current
    neuron->clearTotalCurrent();
    
    return fired;
}

void LIFDynamics::reset(Neuron* neuron) {
    // Reset to initial state
    neuron->reset();
}

// HHN Dynamics Implementation  
bool HHNDynamics::update(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) {
    // Real Hodgkin-Huxley dynamics (placeholder - would implement ion channels)
    // For now, delegate to LIF but with different parameters
    bool fired = false;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    float V_rest = state.restingPotential;
    float threshold = state.threshold;
    float resetPotential = state.resetPotential;
    
    // Simple HH-like dynamics with voltage-gated channels (placeholder)
    // In real implementation: integrate m^3 h v equations
    
    // For now, use modified LIF with HH-like adaptation
    float adaptation = 1.0f + state.adaptationVariable * 0.5f;
    
    float tau = 20.0f / adaptation; // Modified time constant
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    
    float synapticCurrent = neuron->getTotalCurrent();
    float V_new = V * alpha + (V_rest + synapticCurrent / state.leakConductance) * (1.0f - alpha);
    
    // HH-like voltage dependence
    float m_inf = 0.5f * (1.0f + std::tanh((V_new - (-40.0f)) / 10.0f));
    float h_inf = 1.0f / (1.0f + std::exp((V_new - (-70.0f)) / 10.0f));
    
    // Apply channel dynamics
    V_new = V_rest + 120.0f * m_inf * m_inf * m_inf * h_inf - 36.0f * m_inf * m_inf * m_inf * h_inf;
    
    // Check for spike
    if (V_new >= threshold) {
        fired = true;
        neuron->setFiringState(FiringState::Active);
        neuron->setMembranePotential(resetPotential);
        neuron->setLastSpikeTime(currentTime);
        neuron->recordSpike(currentTime);
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
    } else {
        neuron->setMembranePotential(V_new);
        neuron->setFiringState(FiringState::Active);
    }
    
    neuron->clearTotalCurrent();
    return fired;
}

void HHNDynamics::reset(Neuron* neuron) {
    neuron->reset();
}

// FHN Dynamics Implementation
bool FHNDynamics::update(Neuron* neuron, Timestamp currentTime, TimestepDuration dt) {
    // Real FitzHugh-Nagumo dynamics (excitable system)
    bool fired = false;
    
    const auto& state = neuron->getState();
    float V = state.membranePotential;
    float W = state.adaptationVariable; // Using adaptation variable as W
    float threshold = state.threshold;
    float resetPotential = state.resetPotential;
    
    // FHN equations:
    // dV/dt = (V - V^3/3 - W + I) / tau
    // dW/dt = epsilon * (V + a - b*W)
    
    float tau = 1.0f;
    float epsilon = 0.08f;
    float a = 0.7f;
    float b = 0.8f;
    
    float synapticCurrent = neuron->getTotalCurrent();
    
    // Compute derivatives
    float dVdt = (V - V*V*V/3.0f - W + synapticCurrent) / tau;
    float dWdt = epsilon * (V + a - b * W);
    
    // Forward Euler integration
    float V_new = V + dVdt * static_cast<float>(dt);
    float W_new = W + dWdt * static_cast<float>(dt);
    
    // Spike detection (threshold crossing with reset)
    if (V_new >= threshold) {
        fired = true;
        V_new = resetPotential; // Reset
        // W gets incremented for afterpotential
        W_new += 1.2f;
    }
    
    // Clamp for numerical stability
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    W_new = std::clamp(W_new, -5.0f, 5.0f);
    
    neuron->setMembranePotential(V_new);
    neuron->setFiringState(fired ? FiringState::Refractory : FiringState::Active);
    neuron->setState().adaptationVariable = W_new; // Update W
    
    if (fired) {
        neuron->setLastSpikeTime(currentTime);
        neuron->recordSpike(currentTime);
        neuron->setRefractoryPeriod(state.refractoryPeriod);
        neuron->setFiringState(FiringState::Refractory);
    }
    
    neuron->clearTotalCurrent();
    return fired;
}

void FHNDynamics::reset(Neuron* neuron) {
    neuron->reset();
    neuron->setState().adaptationVariable = 0.0f; // Reset W variable
}

// TSMDSynapticDynamics Implementation
struct TSMDSynapticDynamics::Impl {
    float u_;               // Utilization factor (0-1)
    float x_;               // Facilitation variable
    float y_;               // Depression variable
    float u_max_;           // Maximum utilization
    float tau_d_;           // Depression time constant
    float tau_f_;           // Facilitation time constant
    double last_pre_spike_; // Time of last pre-synaptic spike
    double last_post_spike_; // Time of last post-synaptic spike
    
    Impl() : u_(1.0f), x_(0.0f), y_(1.0f), u_max_(1.0f), 
             tau_d_(200.0f), tau_f_(100.0f), 
             last_pre_spike_(-1.0), last_post_spike_(-1.0) {}
};

TSMDSynapticDynamics::TSMDSynapticDynamics() : pImpl(std::make_unique<Impl>()) {}

void TSMDSynapticDynamics::update(double currentTime, TimestepDuration dt) {
    // Tsodyks-Markram synaptic plasticity model
    Impl& impl = *pImpl;
    
    float dt_f = static_cast<float>(dt);
    
    // Update facilitation (fast recovery)
    if (impl.last_pre_spike_ >= 0.0) {
        float t_since_pre = static_cast<float>(currentTime - impl.last_pre_spike_);
        impl.x_ *= std::exp(-t_since_pre / impl.tau_f_);
    }
    
    // Update depression (slow)
    if (impl.last_post_spike_ >= 0.0 || impl.last_pre_spike_ >= 0.0) {
        float t_since_activity = static_cast<float>(
            currentTime - std::max(impl.last_post_spike_, impl.last_pre_spike_)
        );
        impl.y_ += (1.0f - impl.y_) * (1.0f - std::exp(-t_since_activity / impl.tau_d_));
    }
    
    // Clamp values
    impl.x_ = std::clamp(impl.x_, 0.0f, 2.0f);
    impl.y_ = std::clamp(impl.y_, 0.0f, 2.0f);
    
    // Update utilization
    impl.u_ = impl.u_max_ * impl.x_ * impl.y_;
    impl.u_ = std::clamp(impl.u_, 0.0f, impl.u_max_);
}

float TSMDSynapticDynamics::getEfficacy() const {
    return pImpl->u_;
}

void TSMDSynapticDynamics::reset() {
    *pImpl = Impl(); // Reinitialize with defaults
}

void TSMDSynapticDynamics::configure(float u_max, float tau_d, float tau_f, float x_0, float y_0) {
    pImpl->u_max_ = u_max;
    pImpl->tau_d_ = tau_d;
    pImpl->tau_f_ = tau_f;
    pImpl->x_ = x_0;
    pImpl->y_ = y_0;
    pImpl->u_ = u_max * x_0 * y_0;
}

void TSMDSynapticDynamics::recordPreSpike(double time) {
    pImpl->last_pre_spike_ = time;
}

void TSMDSynapticDynamics::recordPostSpike(double time) {
    pImpl->last_post_spike_ = time;
}

} // namespace nlm
