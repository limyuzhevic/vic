// Neuron LIF Integration implementation
#include "NeuronLIFIntegration.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {

bool NeuronLIFIntegration::integrate(Timestamp currentTime, TimestepDuration dt) {
    // Handle refractory period first
    handleRefractoryPeriod();
    if (neuron->getState().refractoryRemaining > 0) {
        return false;
    }
    
    // Update membrane potential using LIF dynamics
    updateMembranePotential(currentTime, dt);
    
    // Apply spike-frequency adaptation
    applyAdaptation();
    
    // Check for spike
    if (checkForSpike()) {
        handleSpike(currentTime);
        return true;
    }
    
    // Set appropriate firing state
    neuron->setFiringState(FiringState::Active);
    
    // Clear synaptic input for next step
    neuron->clearTotalCurrent();
    
    return false;
}

void NeuronLIFIntegration::handleRefractoryPeriod() {
    if (neuron->getState().refractoryRemaining > 0) {
        --neuron->getState().refractoryRemaining;
        // During refractory period, clear synaptic input but don't integrate
        neuron->clearTotalCurrent();
        if (neuron->getState().refractoryRemaining == 0) {
            neuron->setFiringState(FiringState::Resting);
        }
    }
}

void NeuronLIFIntegration::updateMembranePotential(Timestamp currentTime, TimestepDuration dt) {
    auto& state = neuron->getState();
    
    // LIF dynamics: Leaky Integrate-and-Fire
    // dV/dt = (V_rest - V)/tau + I/C
    // Discrete approximation: V_new = V + dt * ((V_rest - V)/tau + I/C)
    
    MembranePotential& V = state.membranePotential;
    MembranePotential V_rest = state.restingPotential;
    MembranePotential V_reset = state.resetPotential;
    MembranePotential threshold = state.threshold;
    
    // Synaptic input contributes to membrane potential change
    float synapticContribution = calculateSynapticContribution();
    
    // Leak contribution
    float leakContribution = calculateLeakContribution();
    
    // Update membrane potential using exponential Euler integration
    V = V + static_cast<float>(dt) * 1000.0f * (leakContribution + synapticContribution);
    
    // Clamp membrane potential to prevent instability
    V = clampMembranePotential(V);
}

void NeuronLIFIntegration::applyAdaptation() {
    if (neuron->getState().adaptationVariable > 0.0f) {
        neuron->addToMembranePotential(-neuron->getState().adaptationVariable * 0.01f);
        neuron->getState().adaptationVariable *= 0.95f;  // Decay adaptation
    }
}

bool NeuronLIFIntegration::checkForSpike() const {
    return neuron->getState().membranePotential >= neuron->getState().threshold;
}

void NeuronLIFIntegration::handleSpike(Timestamp currentTime) {
    auto& state = neuron->getState();
    
    // Update firing state
    neuron->setFiringState(FiringState::Active);
    state.lastSpikeTime = static_cast<float>(currentTime);
    
    // Record spike
    neuron->recordSpike(currentTime);
    
    // Reset membrane potential
    state.membranePotential = state.resetPotential;
    
    // Enter refractory period
    state.refractoryRemaining = state.refractoryPeriod;
    state.firingState = FiringState::Refractory;
    
    // Update adaptation for spike-frequency adaptation
    state.adaptationVariable += 1.0f;
}

void NeuronLIFIntegration::clearIntegrationState() {
    neuron->clearTotalCurrent();
}

MembranePotential NeuronLIFIntegration::calculateLeakContribution() const {
    auto& state = neuron->getState();
    float tau = TIME_CONSTANT;  // ms
    return (state.restingPotential - state.membranePotential) / tau;
}

MembranePotential NeuronLIFIntegration::calculateSynapticContribution() const {
    float C = MEMBRANE_CAPACITANCE;  // nF
    return neuron->getState().synapticInput / C;
}

MembranePotential NeuronLIFIntegration::clampMembranePotential(MembranePotential V) const {
    return std::clamp(V, MIN_MEMBRANE_POTENTIAL, MAX_MEMBRANE_POTENTIAL);
}

} // namespace nlm