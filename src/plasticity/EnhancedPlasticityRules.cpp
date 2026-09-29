// EnhancedPlasticityRules.cpp - Implementation of enhanced plasticity rules
#include "EnhancedPlasticityRules.hpp"
#include "src/brain/Synapse.hpp"
#include "src/brain/Neuron.hpp"
#include "src/core/Logger/Logger.hpp"
#include <random>
#include <algorithm>
#include <cmath>
#include <stdexcept>

namespace nlm {

// Helper function for random number generation
inline float randomFloat(float min = 0.0f, float max = 1.0f) {
    static std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> dist(min, max);
    return dist(rng);
}

// EnhancedPlasticityRule implementation
void EnhancedPlasticityRule::applyPlasticity(Synapse* synapse, 
                                            const std::vector<Timestamp>& preSpikes,
                                            const std::vector<Timestamp>& postSpikes,
                                            TimestepDuration dt,
                                            double timeSinceLastUpdate) {
    // This method should be overridden by derived classes
    // Base implementation does nothing
}

// AdvancedSTDP implementation
AdvancedSTDP::AdvancedSTDP() {
    configure(0.01f, 0.012f, 20.0f);
}

void AdvancedSTDP::applyPlasticity(Synapse* synapse, 
                                   const std::vector<Timestamp>& preSpikes,
                                   const std::vector<Timestamp>& postSpikes,
                                   TimestepDuration dt,
                                   double timeSinceLastUpdate) {
    if (!synapse) return;
    
    // Calculate weight change using STDP formula
    float weightChange = 0.0f;
    
    // Process pre-post spike pairs for LTP
    for (Timestamp preTime : preSpikes) {
        for (Timestamp postTime : postSpikes) {
            float delta_t = static_cast<float>(postTime - preTime);
            
            if (delta_t > 0 && delta_t < tau_) {
                // LTP: post fires after pre (causal)
                float contribution = ltpWeight_ * std::exp(-delta_t / tau_);
                weightChange += contribution * learningRateNorm_;
            } else if (delta_t < 0 && std::abs(delta_t) < tau_) {
                // LTD: pre fires after post (anti-causal)
                float contribution = ltdWeight_ * std::exp(std::abs(delta_t) / tau_);
                weightChange -= contribution * learningRateNorm_;
            }
        }
    }
    
    // Apply weight change with neuromodulation
    float modulationFactor = getModulationFactor(synapse);
    weightChange *= modulationFactor;
    
    applyWeightChange(synapse, weightChange);
}

void AdvancedSTDP::updateSpikeTraces(const std::vector<Timestamp>& spikeTimes,
                                    std::vector<float>& trace,
                                    float decayRate) {
    double currentTime = std::chrono::duration<double>(std::chrono::high_resolution_clock::now().time_since_epoch()).count();
    
    // Decay existing traces
    for (float& traceVal : trace) {
        traceVal *= std::exp(-decayRate * (currentTime - getLastUpdateTime()));
    }
    
    // Add new spikes to traces
    for (Timestamp spikeTime : spikeTimes) {
        double spikeTimeDouble = static_cast<double>(spikeTime);
        for (size_t i = 0; i < trace.size(); ++i) {
            float delta = static_cast<float>(currentTime - spikeTimeDouble);
            if (delta < 100.0) { // Within 100ms window
                trace[i] += 1.0f * std::exp(-delta / 10.0f); // Fast decay
            }
        }
    }
}

// RewardModulatedSTDP implementation
void RewardModulatedSTDP::applyPlasticity(Synapse* synapse,
                                         const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt,
                                         double timeSinceLastUpdate) {
    if (!synapse) return;
    
    // Get current reward prediction error and dopamine
    float rpe = getRewardPredictionError();
    float dopamine = getDopamineLevel();
    
    // Calculate reward modulation factor
    float rewardMod = calculateRewardModulation(rpe, dopamine);
    
    // Apply standard STDP with reward modulation
    AdvancedSTDP::applyPlasticity(synapse, preSpikes, postSpikes, dt, timeSinceLastUpdate);
    
    // Additional reward-based modulation
    float rewardFactor = baselineLearningRate_ * rewardMod;
    applyWeightChange(synapse, rewardFactor * 0.001f); // Small additional change
}

float RewardModulatedSTDP::calculateRewardModulation(float rpe, float dopamine) {
    // Complex reward modulation function
    float mod = 1.0f + rewardMod_ * (rpe * 0.5f + dopamine * 0.3f);
    return std::max(0.05f, std::min(2.0f, mod)); // Clamp to reasonable range
}

// ConceptBasedHebbian implementation
void ConceptBasedHebbian::applyPlasticity(Synapse* synapse,
                                         const std::vector<Timestamp>& preSpikes,
                                         const std::vector<Timestamp>& postSpikes,
                                         TimestepDuration dt,
                                         double timeSinceLastUpdate) {
    if (!synapse) return;
    
    // Calculate coincidence detection
    float coincidence = calculateCoincidenceDetection(preSpikes, postSpikes);
    
    // Calculate concept association strength
    ConceptId preConcept = getPreSpikeConcept(preSpikes);
    ConceptId postConcept = getPostSpikeConcept(postSpikes);
    float conceptStrength = getConceptAssociation(preConcept, postConcept);
    
    // Apply Hebbian learning based on concept association
    float weightChange = 0.0f;
    
    if (conceptStrength > 0.0f) {
        // Conceptually correlated firing strengthens synapse
        weightChange = learningRate_ * conceptStrength * coincidence;
    } else {
        // Unassociated firing has minimal effect
        weightChange = learningRate_ * calculateWeakCoincidence(preSpikes, postSpikes) * 0.1f;
    }
    
    // Apply weight change with concept-based bounds
    applyWeightChange(synapse, weightChange);
    
    // Update concept associations
    updateConceptAssociation(preConcept, postConcept, weightChange);
}

float ConceptBasedHebbian::calculateCoincidenceDetection(const std::vector<Timestamp>& preSpikes,
                                                         const std::vector<Timestamp>& postSpikes) {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    float minTimeDiff = std::numeric_limits<float>::max();
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float diff = std::abs(static_cast<float>(post - pre));
            if (diff < minTimeDiff) {
                minTimeDiff = diff;
            }
        }
    }
    
    return std::exp(-minTimeDiff / associationWindow_);
}

float ConceptBasedHebbian::calculateWeakCoincidence(const std::vector<Timestamp>& preSpikes,
                                                     const std::vector<Timestamp>& postSpikes) {
    if (preSpikes.empty() || postSpikes.empty()) return 0.0f;
    
    float totalCorrelation = 0.0f;
    for (Timestamp pre : preSpikes) {
        for (Timestamp post : postSpikes) {
            float diff = std::abs(static_cast<float>(post - pre));
            if (diff > associationWindow_) {
                totalCorrelation += std::exp(-diff / (associationWindow_ * 10.0f));
            }
        }
    }
    
    return totalCorrelation / (preSpikes.size() * postSpikes.size());
}

void ConceptBasedHebbian::updateConceptAssociation(ConceptId conceptA, ConceptId conceptB, float weightChange) {
    auto key = std::make_pair(conceptA, conceptB);
    auto reverseKey = std::make_pair(conceptB, conceptA);
    
    if (conceptAssociations_.find(key) != conceptAssociations_.end()) {
        conceptAssociations_[key] += weightChange * 0.01f; // Slow concept learning
        conceptAssociations_[reverseKey] += weightChange * 0.01f;
    }
}

// ActivityDependentStructuralPlasticity implementation
void ActivityDependentStructuralPlasticity::applyPlasticity(Synapse* synapse,
                                                           const std::vector<Timestamp>& preSpikes,
                                                           const std::vector<Timestamp>& postSpikes,
                                                           TimestepDuration dt,
                                                           double timeSinceLastUpdate) {
    if (!synapse) return;
    
    // Calculate local neuronal activity
    float localActivity = calculateLocalActivity(preSpikes, postSpikes);
    
    // Apply structural plasticity based on activity
    if (localActivity > activityThreshold_) {
        // Synaptogenesis: create new synapses
        float synaptogenesisProbability = synaptogenesisRate_ * localActivity;
        if (shouldCreateSynapse(synaptogenesisProbability)) {
            createNewSynapse();
        }
        
        // Strengthen existing synapses
        float strengthening = synaptogenesisProbability * 0.1f;
        applyWeightChange(synapse, strengthening);
    } else {
        // Pruning: remove weak synapses
        float pruningProbability = pruningRate_ * (1.0f - localActivity);
        if (shouldPruneSynapse(pruningProbability) && synapse->getWeight() < 0.1f) {
            pruneSynapse(synapse);
        }
        
        // Depression of existing synapses
        float depression = pruningProbability * 0.05f;
        applyWeightChange(synapse, -depression);
    }
}

float ActivityDependentStructuralPlasticity::calculateLocalActivity(const std::vector<Timestamp>& preSpikes,
                                                                     const std::vector<Timestamp>& postSpikes) {
    float preActivity = static_cast<float>(preSpikes.size());
    float postActivity = static_cast<float>(postSpikes.size());
    return (preActivity + postActivity) * 0.5f;
}

bool ActivityDependentStructuralPlasticity::shouldCreateSynapse(float probability) {
    return randomFloat() < probability;
}

bool ActivityDependentStructuralPlasticity::shouldPruneSynapse(float probability) {
    return randomFloat() < probability;
}

float ActivityDependentStructuralPlasticity::calculateNeuronalActivity(Neuron* neuron) {
    if (!neuron) return 0.0f;
    
    const auto& state = neuron->getState();
    float membraneActivity = std::abs(state.membranePotential - state.restingPotential);
    float firingActivity = neuron->isFiring() ? 1.0f : 0.0f;
    
    return (membraneActivity * 0.7f) + (firingActivity * 0.3f);
}

} // namespace nlm