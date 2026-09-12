#include "Synaptogenesis.hpp"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <random>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    // Synaptogenesis parameters
    float activityThreshold;      // Minimum neural activity to trigger formation
    float connectivityProbability; // Probability of forming new connections
    float preferentialAttachment;  // Weight given to high-activity neurons
    float homeostaticTarget;      // Target for homeostatic regulation
    float structuralConstants;    // Baseline structural plasticity rate
    
    // State tracking
    float currentDensity;
    size_t lastSynapseCount;
    float accumulatedActivity;
    
    Impl() : 
        formationRate(0.0001f), 
        targetDensity(0.1f),
        activityThreshold(0.5f),
        connectivityProbability(0.05f),
        preferentialAttachment(0.3f),
        homeostaticTarget(0.08f),
        structuralConstants(0.0001f),
        currentDensity(0.0f),
        lastSynapseCount(0),
        accumulatedActivity(0.0f) {}
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
    // Real synaptogenesis implementing biologically plausible mechanisms
    if (!brain) return;
    
    // Get current brain state
    auto* region = brain->getRegion(RegionId(1));
    if (!region) return;
    
    size_t currentSynapseCount = region->getSynapseCount();
    float currentDensity = static_cast<float>(currentSynapseCount) / brain->getMaxSynapseCount();
    pImpl->currentDensity = currentDensity;
    
    // Update activity metrics
    pImpl->accumulatedActivity += brain->getTotalSpikeCount();
    
    // Apply homeostatic regulation to maintain target density
    float densityError = pImpl->targetDensity - pImpl->currentDensity;
    if (std::abs(densityError) > 0.01f) {
        // Adjust formation rate based on homeostatic needs
        float adjustment = densityError * 0.1f * pImpl->formationRate;
        pImpl->formationRate += adjustment;
        pImpl->formationRate = std::max(0.00001f, std::min(0.001f, pImpl->formationRate));
    }
    
    // Calculate formation probability based on multiple factors
    float activityFactor = std::min(1.0f, pImpl->accumulatedActivity / 1000.0f);
    float densityFactor = 1.0f - std::abs(pImpl->currentDensity - pImpl->targetDensity) * 5.0f;
    float homeostaticFactor = 1.0f + std::abs(pImpl->currentDensity - pImpl->homeostaticTarget) * 2.0f;
    
    float formationProbability = pImpl->formationRate * 
                                 activityFactor * 
                                 densityFactor * 
                                 homeostaticFactor;
    
    // For each potential new synapse, decide whether to create it
    size_t neuronsToConsider = std::min(size_t(50), brain->getTotalNeuronCount());
    for (size_t i = 0; i < neuronsToConsider; ++i) {
        if ((float)rand() / RAND_MAX < formationProbability) {
            // Find suitable source and destination neurons
            auto neurons = brain->getAllNeurons();
            if (neurons.size() < 2) break;
            
            // Choose neurons based on activity (preferentially attach to active neurons)
            std::vector<std::pair<Neuron*, float>> neuronActivity;
            for (Neuron* neuron : neurons) {
                float activity = neuron->getFiringRate();
                float weight = activity * (1.0f - pImpl->preferentialAttachment) + 
                             pImpl->preferentialAttachment;
                neuronActivity.push_back(std::make_pair(neuron, weight));
            }
            
            // Select source neuron (presynaptic)
            Neuron* source = selectNeuronByWeight(neuronActivity, rng);
            
            // Select destination neuron (postsynaptic) - different from source
            Neuron* dest = nullptr;
            do {
                dest = selectNeuronByWeight(neuronActivity, rng);
            } while (dest == source && neuronActivity.size() > 1);
            
            if (source && dest && source->getId() != dest->getId()) {
                // Check if synapse already exists
                if (!brain->hasSynapse(source->getId(), dest->getId())) {
                    // Create new synapse with biologically plausible parameters
                    float weight = rng.uniformReal(-0.5f, 0.5f);
                    float delay = rng.uniformInt(1, 5);
                    
                    brain->createSynapse(source->getId(), dest->getId(), weight, delay);
                    
                    // Update plasticity flags
                    auto* synapse = brain->getSynapse(source->getId(), dest->getId());
                    if (synapse) {
                        synapse->enablePlasticity(true, true, true); // Enable all plasticity types
                    }
                    
                    // Log formation
                    if (currentSynapseCount % 100 == 0) {
                        // This would normally use logging system
                    }
                }
            }
        }
    }
    
    // Reset activity for next cycle (decay)
    pImpl->accumulatedActivity *= 0.9f;
}

// Helper function to select neuron based on activity weight
Neuron* Synaptogenesis::selectNeuronByWeight(
    const std::vector<std::pair<Neuron*, float>>& neurons, 
    RandomGenerator& rng) {
    if (neurons.empty()) return nullptr;
    
    float totalWeight = 0.0f;
    for (const auto& pair : neurons) {
        totalWeight += pair.second;
    }
    
    float selection = rng.uniformReal(0.0f, totalWeight);
    float cumulative = 0.0f;
    
    for (const auto& pair : neurons) {
        cumulative += pair.second;
        if (selection <= cumulative) {
            return pair.first;
        }
    }
    
    return neurons.back().first;
}

} // namespace nlm
