#include "Synaptogenesis.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <random>
#include <algorithm>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    // Synapses formed in current step for activity tracking
    std::vector<std::pair<NeuronId, NeuronId>> recentlyFormed;
    
    // Hebbian learning strength decay
    float hebbianDecayRate;
    
    // STDP parameters
    float stdpWindowSize;      // Time window for STDP (ms)
    float preSpikeWeightChange;  // Weight change for pre->post spike
    float postSpikeWeightChange; // Weight change for post->pre spike
    
    // Competition and selection parameters
    float selectionPressure;    // Strength of synaptic competition
    float synapticScaling;      // Rate of synaptic weight normalization
    
    // Activity history for Hebbian learning
    std::vector<float> preSpikeActivity;
    std::vector<float> postSpikeActivity;
    
    Impl() : hebbianDecayRate(0.01f),
             stdpWindowSize(20.0f),  // 20ms STDP window
             preSpikeWeightChange(0.01f),
             postSpikeWeightChange(-0.012f),
             selectionPressure(0.3f),
             synapticScaling(0.001f) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl()),
    formationRate_(0.05f), targetDensity_(1.8f),
    spontaneousRate_(0.005f), hebbianStrength_(0.8f),
    stdpLearningRate_(0.01f), totalSynapsesFormed_(0), currentDensity_(0.0f) {}

Synaptogenesis::~Synaptogenesis() = default;

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng, TimestepDuration dt, 
                           float neuromodulationFactor) {
    if (!brain) return;
    
    // Calculate current synapse density
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t totalNeurons = brain->getTotalNeuronCount();
    if (totalNeurons > 0) {
        currentDensity_ = static_cast<float>(totalSynapses) / totalNeurons;
    }
    
    // Adjust formation rate based on developmental stage
    float stageFactor = 1.0f;
    if (brain->getDevelopmentSystem()) {
        stageFactor = brain->getDevelopmentSystem()->getPlasticityModifier();
    }
    
    // Calculate target adjustment based on neuromodulation
    float targetAdjustment = neuromodulationFactor * 1.0f;
    
    // Spontaneous synaptogenesis
    float spontaneousFormation = spontaneousRate_ * stageFactor * targetAdjustment * dt * 1000.0f;
    
    // Activity-dependent synaptogenesis (Hebbian)
    float activityFormation = hebbianStrength_ * stageFactor * dt * 1000.0f;
    
    // STDP-driven synaptogenesis based on recent activity
    float stdpFormation = stdpLearningRate_ * stageFactor * dt * 1000.0f;
    
    // Determine total formation events
    float totalFormationEvents = spontaneousFormation + activityFormation + stdpFormation;
    
    // Limit formation based on current density relative to target
    float densityRatio = currentDensity_ / targetDensity_;
    float densityLimitation = (densityRatio < 1.0f) ? (1.0f - densityRatio) : 0.0f;
    totalFormationEvents *= (1.0f + densityLimitation * 0.5f);
    
    // Cap formation events
    totalFormationEvents = std::min(totalFormationEvents, 100.0f);
    
    // Form synapses based on weighted probabilities
    size_t maxFormations = static_cast<size_t>(totalFormationEvents);
    
    for (size_t i = 0; i < maxFormations; ++i) {
        // Choose formation mechanism based on relative weights
        float spontaneousProb = spontaneousFormation / (totalFormationEvents + 1e-8f);
        float activityProb = spontaneousFormation + activityFormation;
        
        float r = rng.uniformReal(0.0, 1.0);
        
        if (r < spontaneousProb && currentDensity_ < targetDensity_ * 0.8f) {
            // Spontaneous formation between random neurons
            if (brain->getTotalNeuronCount() >= 2) {
                auto neurons = brain->getRegionIds();
                if (neurons.size() >= 2) {
                    size_t n1 = rng.pickRandomIndex(static_cast<uint64_t>(neurons.size()));
                    size_t n2 = rng.pickRandomIndex(static_cast<uint64_t>(neurons.size()));
                    while (n1 == n2) {
                        n2 = rng.pickRandomIndex(static_cast<uint64_t>(neurons.size()));
                    }
                    // Create synapse
                    SynapseId synId = brain->getSynapsePool()->allocate(
                        NeuronId(n1), NeuronId(n2));
                    if (synId.value != UINT64_MAX) {
                        totalSynapsesFormed_++;
                        pImpl->recentlyFormed.emplace_back(NeuronId(n1), NeuronId(n2));
                    }
                }
            }
        } else if (r < activityProb && currentDensity_ < targetDensity_) {
            // Activity-dependent (Hebbian) formation - implement based on activity
        } else if (currentDensity_ < targetDensity_) {
            // STDP-based formation - implement based on spike timing
        }
    }
    
    // Apply Hebbian learning to existing synapses
    for (const auto& [source, target] : pImpl->recentlyFormed) {
        auto* synapse = brain->getSynapsePool()->getState<SynapseState>(0); // Simplified
        if (synapse) {
            updateHebbianWeight(synapse, preSpikeActivity[source.value % preSpikeActivity.size()],
                              postSpikeActivity[target.value % postSpikeActivity.size()],
                              hebbianStrength_, dt);
        }
    }
    
    // Decay activity history
    for (auto& activity : preSpikeActivity) {
        activity *= (1.0f - pImpl->hebbianDecayRate);
    }
    for (auto& activity : postSpikeActivity) {
        activity *= (1.0f - pImpl->hebbianDecayRate);
    }
}

bool Synaptogenesis::formHebbianSynapse(NeuronId source, NeuronId target, 
                                       Brain* brain, float similarity,
                                       const RandomGenerator& rng) {
    if (!brain) return false;
    
    // Check if synapse already exists
    if (brain->getSynapsePool()->getSource(SynapseIndex(0)) == source.value &&
        brain->getSynapsePool()->getDestination(SynapseIndex(0)) == target.value) {
        return false;
    }
    
    // Calculate formation probability based on similarity
    float formationProb = formationRate_ * similarity * hebbianStrength_;
    formationProb = std::min(formationProb, 1.0f);
    
    if (rng.bernoulli(formationProb)) {
        // Create synapse
        SynapseId synId = brain->getSynapsePool()->allocate(source, target);
        if (synId.value != UINT64_MAX) {
            totalSynapsesFormed_++;
            
            // Set initial weight based on similarity
            auto* synapse = brain->getSynapsePool()->getState<SynapseState>(0);
            if (synapse) {
                synapse->weight = similarity * 0.5f + 0.1f;
            }
            
            return true;
        }
    }
    
    return false;
}

bool Synaptogenesis::formSTDPSynapse(NeuronId source, NeuronId target,
                                    Timestamp preSpikeTime, Timestamp postSpikeTime,
                                    Brain* brain, const RandomGenerator& rng) {
    if (!brain) return false;
    
    // Calculate STDP eligibility based on spike timing
    float timeDifference = postSpikeTime - preSpikeTime;
    
    // STDP window
    if (std::abs(timeDifference) > pImpl->stdpWindowSize) {
        return false;  // Outside STDP window
    }
    
    // Determine direction of weight change
    float deltaW = 0.0f;
    if (timeDifference > 0) {
        // Post spike after pre spike: potentiation
        deltaW = pImpl->postSpikeWeightChange * stdpLearningRate_;
    } else {
        // Pre spike after post spike: depression
        deltaW = pImpl->preSpikeWeightChange * stdpLearningRate_;
    }
    
    // Check formation probability
    float formationProb = std::abs(deltaW) * 10.0f; // Higher probability for larger changes
    formationProb = std::min(formationProb, 1.0f);
    
    if (rng.bernoulli(formationProb)) {
        // Create synapse
        SynapseId synId = brain->getSynapsePool()->allocate(source, target);
        if (synId.value != UINT64_MAX) {
            totalSynapsesFormed_++;
            
            // Set initial weight based on STDP delta
            auto* synapse = brain->getSynapsePool()->getState<SynapseState>(0);
            if (synapse) {
                synapse->weight = 0.1f + std::abs(deltaW);
            }
            
            return true;
        }
    }
    
    return false;
}

void Synaptogenesis::updateHebbianWeight(Synapse* synapse, float preActivity,
                                        float postActivity, float learningRate,
                                        TimestepDuration dt) {
    if (!synapse) return;
    
    // Hebbian learning rule: weight change proportional to product of activities
    float weightChange = learningRate * preActivity * postActivity * dt * 1000.0f;
    
    // Apply learning with bounds
    float newWeight = synapse->weight + weightChange;
    newWeight = std::max(0.0f, std::min(newWeight, 2.0f));
    
    synapse->weight = newWeight;
}

void Synaptogenesis::updateSTDPWeight(Synapse* synapse, Timestamp preSpikeTime,
                                     Timestamp postSpikeTime, float learningRate,
                                     TimestepDuration dt) {
    if (!synapse) return;
    
    Timestamp now = static_cast<Timestamp>(dt * 1000.0); // Convert to ms
    
    // Calculate STDP contribution
    float timeDifference = now - preSpikeTime;
    
    if (std::abs(timeDifference) > pImpl->stdpWindowSize) {
        return; // Outside window
    }
    
    float deltaW = 0.0f;
    if (timeDifference > 0) {
        deltaW = pImpl->postSpikeWeightChange * learningRate;
    } else {
        deltaW = pImpl->preSpikeWeightChange * learningRate;
    }
    
    // Apply STDP update
    float newWeight = synapse->weight + deltaW * dt * 1000.0f;
    newWeight = std::max(0.0f, std::min(newWeight, 2.0f));
    
    synapse->weight = newWeight;
}

} // namespace nlm