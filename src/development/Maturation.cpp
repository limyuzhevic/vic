#include "Maturation.hpp"
#include "../../brain/Brain.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    float growthRate;
    float synapticMaturationRate;
    float firingActivityThreshold;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , growthRate(0.01f)
        , synapticMaturationRate(0.005f)
        , firingActivityThreshold(5.0f) {}
};

Maturation::Maturation() : pImpl(std::make_unique<Impl>()) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain) {
        return;
    }
    
    // Biological maturation model based on activity-dependent plasticity
    float newProgress = pImpl->progress;
    
    // Activity-dependent maturation: neurons mature based on firing activity
    float totalFiringRate = 0.0f;
    size_t neuronCount = 0;
    
    // Get firing statistics from brain
    totalFiringRate = brain->getAverageFiringRate();
    neuronCount = brain->getTotalNeuronCount();
    
    if (neuronCount > 0) {
        float averageFiringRate = totalFiringRate / neuronCount;
        
        // Maturation rate depends on firing activity
        float activityBasedRate = pImpl->growthRate * std::min(1.0f, averageFiringRate / pImpl->firingActivityThreshold);
        
        // Apply maturation
        newProgress += activityBasedRate;
    }
    
    // Synaptic maturation: maturation also depends on synaptic connectivity
    float synapticInfluence = calculateSynapticMaturationInfluence(brain);
    float synapticMaturation = pImpl->synapticMaturationRate * synapticInfluence;
    
    newProgress += synapticMaturation;
    
    // Age-dependent maturation: maturation progresses naturally over time
    float ageBasedRate = pImpl->growthRate * 0.5f; // Slower than activity-dependent
    newProgress += ageBasedRate;
    
    // Clamp progress to [0, 1]
    newProgress = std::clamp(newProgress, 0.0f, 1.0f);
    
    // Update progress
    pImpl->progress = newProgress;
    
    // Apply developmental changes to neurons based on maturation progress
    applyMaturationEffects(brain);
}

MembranePotential Maturation::getMatureThreshold() const {
    return pImpl->matureThreshold;
}

MembranePotential Maturation::getMatureRestingPotential() const {
    return pImpl->matureRestingPotential;
}

float Maturation::getMatureTimeConstant() const {
    return pImpl->matureTimeConstant;
}

float Maturation::getGrowthRate() const {
    return pImpl->growthRate;
}

void Maturation::setGrowthRate(float rate) {
    pImpl->growthRate = std::max(0.0f, rate);
}

float Maturation::getSynapticMaturationRate() const {
    return pImpl->synapticMaturationRate;
}

void Maturation::setSynapticMaturationRate(float rate) {
    pImpl->synapticMaturationRate = std::max(0.0f, rate);
}

float Maturation::calculateSynapticMaturationInfluence(Brain* brain) {
    if (!brain) return 0.0f;
    
    auto regions = brain->getRegions();
    if (regions.empty()) return 0.0f;
    
    float totalMaturationInfluence = 0.0f;
    float totalSynapses = 0.0f;
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto synapses = region->getSynapses();
        totalSynapses += synapses.size();
        
        // Calculate average synaptic weight
        float averageWeight = 0.0f;
        for (auto* synapse : synapses) {
            if (synapse) {
                averageWeight += synapse->getWeight();
            }
        }
        
        if (!synapses.empty()) {
            averageWeight /= synapses.size();
            
            // Stronger synapses contribute more to maturation
            float weightInfluence = std::min(1.0f, std::abs(averageWeight) / 1.0f);
            totalMaturationInfluence += weightInfluence * synapses.size();
        }
    }
    
    return totalSynapses > 0 ? totalMaturationInfluence / totalSynapses : 0.0f;
}

void Maturation::applyMaturationEffects(Brain* brain) {
    if (!brain || pImpl->progress <= 0.0f) {
        return;
    }
    
    auto regions = brain->getRegions();
    
    for (const auto& region : regions) {
        if (!region) continue;
        
        auto neurons = region->getAllNeurons();
        
        for (auto* neuron : neurons) {
            if (!neuron) continue;
            
            // Apply maturation effects based on current progress
            applyNeuronMaturation(neuron);
        }
    }
}

void Maturation::applyNeuronMaturation(Neuron* neuron) {
    if (!neuron) return;
    
    float currentProgress = pImpl->progress;
    
    // Adjust membrane properties based on maturation progress
    if (currentProgress > 0.3f) {
        // Moderate maturation: adjust threshold
        MembranePotential currentThreshold = neuron->getThreshold();
        MembranePotential targetThreshold = pImpl->matureThreshold;
        
        // Gradually approach mature threshold
        float thresholdAdjust = (targetThreshold - currentThreshold) * 0.1f * currentProgress;
        neuron->setThreshold(currentThreshold + thresholdAdjust);
    }
    
    if (currentProgress > 0.6f) {
        // Advanced maturation: adjust resting potential
        MembranePotential currentResting = neuron->getRestingPotential();
        MembranePotential targetResting = pImpl->matureRestingPotential;
        
        float restingAdjust = (targetResting - currentResting) * 0.1f * (currentProgress - 0.3f);
        neuron->setRestingPotential(currentResting + restingAdjust);
    }
    
    if (currentProgress > 0.8f) {
        // Near mature: adjust time constant
        float currentTimeConstant = neuron->getTimeConstant();
        float targetTimeConstant = pImpl->matureTimeConstant;
        
        float tcAdjust = (targetTimeConstant - currentTimeConstant) * 0.1f * (currentProgress - 0.6f);
        neuron->setTimeConstant(currentTimeConstant + tcAdjust);
    }
    
    // Plasticity adjustments based on maturation
    if (currentProgress > 0.5f) {
        // Mature neurons have different plasticity characteristics
        neuron->setPlasticityEnabled(true);
        
        // Adjust plasticity parameters based on maturation
        float plasticityFactor = std::min(1.0f, currentProgress);
        neuron->setPlasticityFactor(plasticityFactor);
    }
    
    // Synaptic properties maturation
    if (currentProgress > 0.7f) {
        // Mature synapses have different characteristics
        neuron->setSynapticMaturationLevel(currentProgress);
    }
}

} // namespace nlm
