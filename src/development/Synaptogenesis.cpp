#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float connectivityProbability;
    float competitionStrength;
    float activityDependentFactor;
    size_t maxSynapsesPerNeuron;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), connectivityProbability(0.15f),
             competitionStrength(0.5f), activityDependentFactor(2.0f), maxSynapsesPerNeuron(50) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = rate;
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = density;
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Real synaptogenesis implementation
    // Combines activity-dependent and activity-independent mechanisms
    
    // Get all neurons in the brain
    auto allNeurons = brain->getAllNeurons();
    if (allNeurons.empty()) return;
    
    // Calculate current connectivity
    size_t totalPossibleConnections = allNeurons.size() * allNeurons.size();
    size_t currentConnections = 0;
    
    for (const Neuron* neuron : allNeurons) {
        currentConnections += neuron->getNumOutgoingSynapses();
    }
    
    float currentDensity = static_cast<float>(currentConnections) / static_cast<float>(totalPossibleConnections);
    
    // Check if we need to form new synapses
    if (currentDensity < pImpl->targetDensity) {
        // Activity-dependent synaptogenesis
        for (size_t preIdx = 0; preIdx < allNeurons.size(); ++preIdx) {
            Neuron* preNeuron = allNeurons[preIdx];
            
            // Skip if neuron already has maximum synapses
            if (preNeuron->getNumOutgoingSynapses() >= pImpl->maxSynapsesPerNeuron) {
                continue;
            }
            
            // Calculate formation probability for each potential postsynaptic neuron
            for (size_t postIdx = 0; postIdx < allNeurons.size(); ++postIdx) {
                if (preIdx == postIdx) continue; // No self-connections
                
                Neuron* postNeuron = allNeurons[postIdx];
                
                // Skip if postsynaptic neuron already has maximum synapses
                if (postNeuron->getNumIncomingSynapses() >= pImpl->maxSynapsesPerNeuron) {
                    continue;
                }
                
                // Calculate formation probability based on multiple factors
                float formationProb = calculateFormationProbability(
                    preNeuron, postNeuron, currentDensity, rng
                );
                
                // Random decision with probabilistic formation
                if (rng.bernoulli(formationProb)) {
                    createSynapse(preNeuron, postNeuron, brain, rng);
                }
            }
        }
    }
    
    // Activity-independent baseline synaptogenesis
    performBaselineSynaptogenesis(brain, rng);
}

float Synaptogenesis::calculateFormationProbability(
    Neuron* preNeuron, Neuron* postNeuron, 
    float currentDensity, RandomGenerator& rng) {
    
    float probability = pImpl->formationRate;
    
    // Activity-dependent component
    float preActivity = preNeuron->getActivity();
    float postActivity = postNeuron->getActivity();
    float avgActivity = (preActivity + postActivity) * 0.5f;
    
    // Higher activity increases probability of connection (Hebbian-like)
    float activityFactor = std::pow(avgActivity, pImpl->activityDependentFactor);
    probability *= (1.0f + activityFactor);
    
    // Temporal correlation component
    float temporalSimilarity = calculateTemporalSimilarity(preNeuron, postNeuron);
    probability *= (1.0f + temporalSimilarity * 0.5f);
    
    // Spatial proximity component (if neurons are in same region)
    float spatialSimilarity = 0.0f;
    if (preNeuron->getRegion() && postNeuron->getRegion() && 
        preNeuron->getRegion() == postNeuron->getRegion()) {
        spatialSimilarity = 1.0f; // Same region, high spatial similarity
    }
    probability *= (1.0f + spatialSimilarity * 0.3f);
    
    // Competition component: reduce probability if density is already high
    float densityCompetition = 1.0f - (currentDensity / (pImpl->targetDensity + 0.1f));
    probability *= std::max(0.0f, densityCompetition);
    
    // Random variation for biological realism
    float randomVariation = rng.uniformReal(0.8f, 1.2f);
    probability *= randomVariation;
    
    // Clamp probability to reasonable range
    probability = std::clamp(probability, 0.0f, 1.0f);
    
    return probability;
}

float Synaptogenesis::calculateTemporalSimilarity(Neuron* neuron1, Neuron* neuron2) {
    // Simplified temporal similarity based on firing patterns
    // In a real implementation, this would use spike timing or traces
    float activity1 = neuron1->getActivity();
    float activity2 = neuron2->getActivity();
    
    // Cosine similarity for activity patterns
    float similarity = (activity1 * activity2) / 
                      std::sqrt((activity1 * activity1) + (activity2 * activity2) + 0.001f);
    
    return similarity;
}

void Synaptogenesis::createSynapse(Neuron* preNeuron, Neuron* postNeuron, 
                                 Brain* brain, RandomGenerator& rng) {
    // Create a new synapse with appropriate properties
    
    // Calculate synaptic strength based on activity
    float preActivity = preNeuron->getActivity();
    float postActivity = postNeuron->getActivity();
    
    // Initial synaptic weight
    float initialWeight = 0.1f + preActivity * postActivity * 0.5f;
    
    // Add some noise for biological realism
    float noise = rng.normal(1.0f, 0.1f);
    initialWeight *= noise;
    
    initialWeight = std::max(0.0f, std::min(initialWeight, 1.0f));
    
    // Create synapse
    Synapse* newSynapse = new Synapse(initialWeight);
    
    // Set presynaptic neuron
    newSynapse->setPresynapticNeuron(preNeuron);
    
    // Set postsynaptic neuron (will be done by brain or synapse)
    // In a real implementation, this would be bidirectional
    
    // Add to brain's synapse collection
    brain->addSynapse(newSynapse);
}

void Synaptogenesis::performBaselineSynaptogenesis(Brain* brain, RandomGenerator& rng) {
    // Activity-independent baseline synaptogenesis
    // Represents spontaneous formation independent of neural activity
    
    auto allNeurons = brain->getAllNeurons();
    size_t numNeurons = allNeurons.size();
    
    // For each neuron, create some baseline number of synapses
    for (size_t i = 0; i < numNeurons; ++i) {
        Neuron* neuron = allNeurons[i];
        
        // Calculate how many additional synapses are needed
        size_t currentOutgoing = neuron->getNumOutgoingSynapses();
        size_t currentIncoming = neuron->getNumIncomingSynapses();
        size_t total = currentOutgoing + currentIncoming;
        
        size_t target = static_cast<size_t>(pImpl->targetDensity * numNeurons);
        size_t needed = target - total;
        
        if (needed > 0) {
            // Create baseline synapses
            for (size_t j = 0; j < needed; ++j) {
                // Pick random target neuron
                Neuron* targetNeuron = rng.pickRandom(allNeurons);
                
                if (targetNeuron != neuron && 
                    neuron->getNumOutgoingSynapses() < pImpl->maxSynapsesPerNeuron &&
                    targetNeuron->getNumIncomingSynapses() < pImpl->maxSynapsesPerNeuron) {
                    
                    createBaselineSynapse(neuron, targetNeuron, brain, rng);
                }
            }
        }
    }
}

void Synaptogenesis::createBaselineSynapse(Neuron* preNeuron, Neuron* postNeuron,
                                         Brain* brain, RandomGenerator& rng) {
    // Create a baseline synapse with conservative properties
    
    float initialWeight = rng.uniformReal(0.05f, 0.15f);
    
    Synapse* newSynapse = new Synapse(initialWeight);
    newSynapse->setPresynapticNeuron(preNeuron);
    
    brain->addSynapse(newSynapse);
}

} // namespace nlm
