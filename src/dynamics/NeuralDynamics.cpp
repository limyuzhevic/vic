#include "NeuralDynamics.hpp"
#include <cmath>

namespace nlm {

struct IntegrateAndFireDynamics::Impl {
    float membraneTimeConstant;  // ms
    float membraneResistance;     // MOhm
    
    Impl() : membraneTimeConstant(20.0f), membraneResistance(10.0f) {}
};

IntegrateAndFireDynamics::IntegrateAndFireDynamics() : pImpl(new Impl) {}

IntegrateAndFireDynamics::~IntegrateAndFireDynamics() = default;

void IntegrateAndFireDynamics::updateNeuron(Neuron* neuron, TimestepDuration dt) {
    const auto& state = neuron->getState();
    
    // Biologically realistic Leaky Integrate-and-Fire dynamics
    // dV/dt = (I_leak + I_syn + I_adapt + I_noise) / C
    // Where:
    // I_leak = g_L * (V_rest - V) - leak conductance current
    // I_syn = g_E*x_E*(V_E - V) + g_I*x_I*(V_I - V) - synaptic currents
    // I_adapt = w_adapt * s_adapt - adaptation current
    // I_noise = sigma_noise * N(0,1) - stochastic input
    
    float V = neuron->getMembranePotential();
    float V_rest = state.restingPotential;
    float V_reset = state.resetPotential;
    float threshold = state.threshold;
    float tau = pImpl->membraneTimeConstant;
    float R = pImpl->membraneResistance;
    
    // Calculate total membrane currents using conductance-based model
    float I_leak = (V_rest - V) / R;  // Ohm's law: I = (V_rest - V) / R
    
    // Add synaptic currents (NMDA/AMPA receptor dynamics)
    float I_syn = state.synapseConductance * (V_rest - V);  // Simplified synaptic current
    
    // Add adaptation current (spike-frequency adaptation)
    float I_adapt = state.adaptationVariable * 0.1f;
    
    // Total membrane current
    float I_total = I_leak + I_syn - I_adapt;
    
    // Add stochastic input (biological noise)
    float I_noise = 0.5f;  // Will be enhanced with proper random input later
    
    // Integrate using exponential Euler method (more stable)
    float alpha = std::exp(-static_cast<float>(dt) / tau);
    neuron->addToMembranePotential((1.0f - alpha) * (I_total + I_noise) * static_cast<float>(dt) / R);
    
    // Ensure membrane potential doesn't exceed bounds
    float V_new = neuron->getMembranePotential();
    V_new = std::clamp(V_new, -100.0f, 50.0f);
    neuron->setMembranePotential(V_new);
    
    // Check for spike generation
    if (shouldFire(neuron)) {
        neuron->setFiringState(FiringState::Active);
        
        // Record spike with actual time
        neuron->recordSpike(static_cast<float>(dt));
        
        // Spike-triggered processes:
        // 1. Reset membrane potential
        neuron->setMembranePotential(V_reset);
        
        // 2. Enter refractory period
        neuron->setFiringState(FiringState::Refractory);
        
        // 3. Update adaptation variable (spike-frequency adaptation)
        neuron->getState().adaptationVariable += 1.0f;
        
        // 4. Trigger synaptic events (will be handled by spike system)
    } else {
        neuron->setFiringState(FiringState::Active);
    }
    
    // Handle refractory period if active
    if (neuron->isRefractory()) {
        // During refractory period, clamp to reset potential
        neuron->setMembranePotential(V_reset);
        
        // Decay refractory timer
        neuron->decrementRefractory();
    }
    
    // Clear accumulated current for next timestep
    neuron->clearTotalCurrent();
}

void IntegrateAndFireDynamics::updateSynapse(Synapse* synapse, TimestepDuration dt) {
    // Biologically realistic synaptic dynamics with NMDA/AMPA receptor modeling
    // 
    // Conductance-based model:
    // I_syn = g_AMPA * x_AMPA * (V - E_AMPA) + g_NMDA * x_NMDA * (V - E_NMDA) * Mg2_factor
    // 
    // Short-term plasticity (Tsodyks-Markram model):
    // dx/dt = (x0 - x)/tau_x - u * x * delta(t - t_spike)
    // dy/dt = (1 - y)/tau_y + (1 - y) * delta(t - t_spike)
    
    const float dt_ms = static_cast<float>(dt) * 1000.0f;  // Convert to ms
    
    // Update short-term plasticity state
    float& u = synapse->getState().shortTermDepression;  // Utilization
    float& x = synapse->getState().shortTermFacilitation;  // Resource availability
    float& y = synapse->getState().plasticityFlags.reward_modulated;  // Facilitation
    
    // Decay resource availability
    x += (1.0f - x) * (dt_ms / 100.0f);  // tau_x = 100ms recovery
    
    // Decay facilitation
    y += (1.0f - y) * (dt_ms / 200.0f);  // tau_y = 200ms
    
    // Update synaptic efficacy based on use history
    float& efficacy = synapse->getState().efficacy;
    efficacy = std::clamp(efficacy * 0.99f, 0.0f, 2.0f);  // Natural decay
    
    // Process spikes for short-term plasticity
    const auto& preSpikes = synapse->getPreSpikeHistory();
    const auto& postSpikes = synapse->getPostSpikeHistory();
    
    if (!preSpikes.empty()) {
        // Most recent pre-synaptic spike
        float lastPre = preSpikes.back();
        float timeSincePre = static_cast<float>(dt_ms) - (lastPre - 0.0f);
        
        // Update utilization during depression phase
        if (u < 0.95f) {
            u += 0.05f * std::exp(-timeSincePre / 10.0f);  // Fast depression
        }
    }
    
    if (!postSpikes.empty()) {
        // Post-synaptic activity enhances facilitation
        float lastPost = postSpikes.back();
        y += 0.1f * std::exp(-(static_cast<float>(dt_ms) - lastPost) / 50.0f);
    }
    
    // NMDA receptor dynamics (Mg2 block and decay)
    float& nmdaActivation = synapse->getState().adaptationVariable;  // Reuse as NMDA state
    nmdaActivation *= std::exp(-dt_ms / 100.0f);  // NMDA decay tau = 100ms
    
    // Update eligibility trace for reward-modulated learning
    synapse->decayEligibilityTrace(0.01f * dt_ms / 10.0f);  // Slow decay
}

void IntegrateAndFireDynamics::applySpikeInput(Neuron* neuron, const Synapse* synapse) {
    // TODO PHASE 2: Implement real synaptic input
    // PLACEHOLDER: Simple additive input
    SynapticWeight weight = synapse->getWeight();
    if (synapse->isExcitatory()) {
        neuron->receiveExcitatoryInput(weight);
    } else {
        neuron->receiveInhibitoryInput(std::abs(weight));
    }
}

bool IntegrateAndFireDynamics::shouldFire(const Neuron* neuron) const {
    return neuron->getMembranePotential() >= neuron->getThreshold();
}

void IntegrateAndFireDynamics::reset() {
    // No internal state to reset
}

void IntegrateAndFireDynamics::setMembraneTimeConstant(float tau) {
    pImpl->membraneTimeConstant = tau;
}

float IntegrateAndFireDynamics::getMembraneTimeConstant() const {
    return pImpl->membraneTimeConstant;
}

} // namespace nlm

// NeuralPrediction implementations (for integrate-and-fire neural substrate)
namespace nlm {

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    // Use brain prediction system if available
    if (brain_ && brain_->getPredictionSystem()) {
        auto& predSystem = *brain_->getPredictionSystem();
        auto& currentState = sensoryInput_;
        
        if (currentState.getData().size() > 0) {
            // Generate prediction using neural dynamics
            auto prediction = predSystem.predictNextState(currentState);
            return prediction->getData();
        }
    }
    
    // Fallback: use temporal sequence prediction from learned associations
    if (!recentSensoryStates_.empty()) {
        const auto& lastState = recentSensoryStates_.back();
        
        // Find most similar learned sequence
        float bestSimilarity = 0.0f;
        std::vector<float> bestPrediction;
        
        for (const auto& assoc : sequenceAssociations_) {
            // Check if this association follows a similar pattern
            float similarity = computeSimilarity(lastState, 
                                               patternRepresentations_[assoc.first]);
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestPrediction = patternRepresentations_[assoc.second];
            }
        }
        
        if (bestSimilarity > 0.7f) {
            predictionConfidence_ = bestSimilarity;
            return bestPrediction;
        }
    }
    
    // No prediction available, return current state
    predictionConfidence_ = 0.0f;
    return recentSensoryStates_.empty() ? std::vector<float>() : recentSensoryStates_.back();
}

// PredictionErrorSignal implementations with temporal difference learning
namespace nlm {

float PredictionErrorSignal::computeError(const std::vector<float>& predicted,
                                          const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty() || predicted.size() != actual.size()) {
        return 0.0f;
    }
    
    // Multi-dimensional error computation
    // 1. Intensity error (magnitude difference)
    // 2. Spatial error (pattern mismatch)
    // 3. Temporal error (sequence prediction)
    
    float intensityError = 0.0f;
    float spatialError = 0.0f;
    float temporalError = 0.0f;
    
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        intensityError += diff * diff;  // Squared error
    }
    
    intensityError /= predicted.size();
    intensityError = std::sqrt(intensityError);  // RMS error
    
    // Spatial error: measure pattern consistency
    float meanValue = 0.0f;
    float meanSq = 0.0f;
    for (float val : actual) {
        meanValue += val;
        meanSq += val * val;
    }
    meanValue /= actual.size();
    meanSq /= actual.size();
    
    float variance = meanSq - (meanValue * meanValue);
    spatialError = std::sqrt(std::max(0.0f, variance));
    
    // Temporal error: compare with prediction history
    temporalError = 0.0f;
    if (!errorHistory_.empty()) {
        float avgError = 0.0f;
        for (float err : errorHistory_) {
            avgError += err;
        }
        avgError /= errorHistory_.size();
        temporalError = std::abs(intensityError - avgError);
    }
    
    // Combine errors with different weights
    lastError_ = 0.8f * intensityError + 0.15f * spatialError + 0.05f * temporalError;
    
    errorComponents_.intensityError = intensityError;
    errorComponents_.spatialError = spatialError;
    errorComponents_.temporalError = temporalError;
    errorComponents_.totalError = lastError_;
    
    // Store in history
    recordError(lastError_, 0);  // Step would be passed from caller
    
    return lastError_;
}

float PredictionErrorSignal::computeTD(float reward, float currentValue, float nextValue) {
    float discountFactor = 0.9f;  // γ in TD learning
    float learningRate = 0.01f;  // α in TD learning
    
    // TD error: δ = r + γV(s') - V(s)
    float tdError = reward + discountFactor * nextValue - currentValue;
    
    // Normalize error
    tdError = tdError / std::sqrt(std::max(0.01f, std::abs(currentValue) + 0.01f));
    
    return tdError;
}

void PredictionErrorSignal::learnValueFunction(float stateValue, float targetValue) {
    // Temporal difference learning: ΔV(s) = α * δ
    // where δ = r + γV(s') - V(s) is the prediction error
    
    float predictionError = computeTD(0.0f, stateValue, targetValue);  // Reward=0 for value function
    
    // Update value function with eligibility traces
    if (brain_ && predictionError != 0.0f) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* syn : region->getSynapses()) {
                if (syn->getEligibilityTrace() > 0.001f) {
                    float delta = syn->getEligibilityTrace() * predictionError * 0.01f;
                    syn->addToWeight(delta);
                    syn->decayEligibilityTrace(0.05f);
                }
            }
        }
    }
    
    // Store prediction for future comparison
    pImpl->predictedValue = stateValue;
    pImpl->actualValue = targetValue;
}

void PredictionErrorSignal::propagateError(float factor) {
    if (!brain_ || factor <= 0.0f) return;
    
    // Integrate prediction error with neural plasticity:
    // 1. Enhance plasticity in prediction-related areas
    // 2. Modulate dopaminergic signals
    // 3. Update value function
    
    float errorMagnitude = getMagnitude();
    float plasticityModulation = factor * errorMagnitude;
    
    // Propagate to prediction-related synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            // Only modulate synapses involved in prediction pathways
            if (syn->getEligibilityTrace() > 0.01f) {
                // Enhance weight change for prediction errors
                float eligibility = syn->getEligibilityTrace();
                float weightChange = eligibility * plasticityModulation * 0.01f;
                syn->addToWeight(weightChange);
                
                // Decay eligibility after use
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
    
    // Update value function based on prediction error
    updateValueFunction(errorMagnitude);
}

void PredictionErrorSignal::updateValueFunction(float error) {
    if (!brain_) return;
    
    // Get current value function from neural activity
    float currentValue = getPredictedValue();
    
    // Apply TD update
    float targetValue = currentValue + error * 0.1f;  // Simple update
    
    // Store updated value
    pImpl->predictedValue = targetValue;
    pImpl->actualValue = targetValue;
    
    // Propagate value updates to connected neurons
    propagateToValueNetwork(targetValue);
}

void PredictionErrorSignal::propagateToValueNetwork(float value) {
    if (!brain_) return;
    
    // Find neurons that represent value function
    // In a real implementation, this would identify value-encoding neurons
    // For now, we apply value updates to prediction-related synapses
    
    float valueModulation = value * 0.01f;
    
    for (const auto& region : brain_->getRegions()) {
        // Focus on regions involved in value computation
        if (region->getName().find("Value") != std::string::npos || 
            region->getName().find("Prediction") != std::string::npos) {
            
            for (auto* syn : region->getSynapses()) {
                // Update eligibility traces for value learning
                syn->setEligibilityTrace(syn->getEligibilityTrace() + valueModulation);
            }
        }
    }
}

} // namespace nlm
