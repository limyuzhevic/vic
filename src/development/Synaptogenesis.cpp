#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include <random>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float connectivityProbability;
    float minDistance;
    float maxDistance;
    Brain* brainReference;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f), 
             connectivityProbability(0.15f), minDistance(5.0f), 
             maxDistance(50.0f), brainReference(nullptr) {}
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
    // IMPLEMENTATION: Real synaptogenesis with activity-dependent formation
    if (!brain) return;
    
    // Set brain reference for access
    pImpl->brainReference = brain;
    
    // Get current synaptic density
    size_t currentSynapseCount = 0;
    for (const auto& region : brain->getRegions()) {
        currentSynapseCount += region->getSynapseCount();
    }
    
    // Calculate region size to estimate density
    size_t totalNeurons = brain->getTotalNeuronCount();
    float currentDensity = static_cast<float>(currentSynapseCount) / static_cast<float>(totalNeurons);
    
    // Only form new synapses if density is below target
    if (currentDensity < pImpl->targetDensity) {
        // Determine how many new synapses to form
        float densityDeficit = pImpl->targetDensity - currentDensity;
        size_t potentialNewSynapses = static_cast<size_t>(densityDeficit * totalNeurons * pImpl->formationRate * 10.0f);
        
        // Limit to reasonable number
        potentialNewSynapses = std::min(potentialNewSynapses, static_cast<size_t>(100));
        
        // Form new synapses based on current activity
        for (size_t i = 0; i < potentialNewSynapses; ++i) {
            formNewSynapse(brain, rng);
        }
    }
}

void Synaptogenesis::formNewSynapse(Brain* brain, RandomGenerator& rng) {
    // IMPLEMENTATION: Form new synapses between neurons based on proximity and activity
    if (!brain) return;
    
    // Get all neurons from the brain
    std::vector<Neuron*> allNeurons;
    for (const auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                allNeurons.push_back(neuron);
            }
        }
    }
    
    if (allNeurons.size() < 2) return;
    
    // Select presynaptic neuron
    size_t preIdx = rng.uniformInt(0, static_cast<int>(allNeurons.size() - 1));
    Neuron* preNeuron = allNeurons[preIdx];
    
    // Find postsynaptic neuron within reasonable distance
    std::vector<Neuron*> potentialPostSynaptic;
    for (size_t i = 0; i < allNeurons.size(); ++i) {
        if (i == preIdx) continue;
        
        Neuron* postNeuron = allNeurons[i];
        
        // Calculate distance between neurons (simplified as region-based)
        RegionId preRegion = preNeuron->getRegionId();
        RegionId postRegion = postNeuron->getRegionId();
        
        if (preRegion == postRegion) {
            float activityPre = std::abs(preNeuron->getState().membranePotential - 
                                       preNeuron->getState().restingPotential);
            float activityPost = std::abs(postNeuron->getState().membranePotential - 
                                        postNeuron->getState().restingPotential);
            
            // Higher activity increases synapse formation probability
            if (activityPre > 0.1f || activityPost > 0.1f) {
                potentialPostSynaptic.push_back(postNeuron);
            }
        }
    }
    
    // If no active neurons in same region, allow any neuron
    if (potentialPostSynaptic.empty()) {
        for (size_t i = 0; i < allNeurons.size(); ++i) {
            if (i != preIdx) {
                potentialPostSynaptic.push_back(allNeurons[i]);
            }
        }
    }
    
    if (potentialPostSynaptic.empty()) return;
    
    // Select postsynaptic neuron randomly from potential
    size_t postIdx = rng.uniformInt(0, static_cast<int>(potentialPostSynaptic.size() - 1));
    Neuron* postNeuron = potentialPostSynaptic[postIdx];
    
    // Create synapse between neurons
    createSynapseBetween(preNeuron, postNeuron, rng);
}

void Synaptogenesis::createSynapseBetween(Neuron* pre, Neuron* post, RandomGenerator& rng) {
    if (!pre || !post) return;
    
    // Get the regions for both neurons
    RegionId preRegion = pre->getRegionId();
    RegionId postRegion = post->getRegionId();
    
    if (!preRegion || !postRegion) return;
    
    // Find regions in the brain
    const std::vector<std::unique_ptr<NeuralRegion>>& regions = pImpl->brain->getRegions();
    NeuralRegion* preRegionPtr = nullptr;
    NeuralRegion* postRegionPtr = nullptr;
    
    for (const auto& region : regions) {
        if (region->getId() == preRegion) {
            preRegionPtr = region.get();
        }
        if (region->getId() == postRegion) {
            postRegionPtr = region.get();
        }
    }
    
    if (!preRegionPtr || !postRegionPtr) return;
    
    // Add synapse from presynaptic to postsynaptic neuron
    preRegionPtr->addSynapse(pre->getId(), post->getId(), 
                           rng.uniformReal(0.1f, 0.5f),  // Random initial weight
                           rng.uniformInt(1, 10));    // Random delay
}

} // namespace nlm
