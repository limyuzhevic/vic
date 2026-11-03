#include "Maturation.hpp"

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    float maturationRate;
    float activityHistory[20];  // For activity-based maturation control
    size_t historyIndex;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , maturationRate(0.001f)
        , historyIndex(0) {
        for (size_t i = 0; i < 20; ++i) {
            activityHistory[i] = 0.0f;
        }
    }
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    pImpl->progress = std::clamp(progress, 0.0f, 1.0f);
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    // PHASE 2: Real maturation implementation
    // Implements biological neural development and maturation with activity-dependent control
    
    if (!brain) return;
    
    // Get current developmental stage
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    // Calculate network-wide activity and statistics
    float averageActivity = calculateAverageNetworkActivity(brain);
    float networkComplexity = calculateNetworkComplexity(brain);
    
    // Update activity history for long-term memory
    pImpl->activityHistory[pImpl->historyIndex] = averageActivity;
    pImpl->historyIndex = (pImpl->historyIndex + 1) % 20;
    
    // Calculate maturation based on developmental stage and activity
    float maturationDelta = 0.0f;
    
    switch (stage) {
        case DevelopmentalStage::Early:
            // Rapid maturation during early development
            maturationDelta = pImpl->maturationRate * 10.0f;
            maturationDelta *= calculateEarlyDevelopmentFactors(brain, averageActivity);
            break;
            
        case DevelopmentalStage::Middle:
            // Slower, activity-dependent maturation during middle development
            maturationDelta = pImpl->maturationRate;
            maturationDelta *= calculateMiddleDevelopmentFactors(brain, averageActivity, networkComplexity);
            break;
            
        case DevelopmentalStage::Late:
            // Fine-tuning and stabilization during late development
            maturationDelta = pImpl->maturationRate * 0.1f;
            maturationDelta *= calculateLateDevelopmentFactors(brain, averageActivity);
            break;
            
        default:
            maturationDelta = 0.0f;
            break;
    }
    
    // Ensure progress stays within bounds
    pImpl->progress = std::min(pImpl->progress + maturationDelta, 1.0f);
    
    // Update neural properties based on maturation progress
    updateNeuralProperties(brain);
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

void Maturation::updateNeuralProperties(Brain* brain) {
    // Update neural properties based on maturation progress
    if (!brain) return;
    
    float maturationProgress = getProgress();
    
    // Interpolate between immature and mature neural properties
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                auto& state = neuron->getState();
                
                // Update resting potential
                state.restingPotential = interpolate(
                    state.restingPotential,
                    getMatureRestingPotential(),
                    maturationProgress
                );
                
                // Update threshold
                state.threshold = interpolate(
                    state.threshold,
                    getMatureThreshold(),
                    maturationProgress
                );
                
                // Update leak conductance (related to time constant)
                float currentLeak = state.leakConductance;
                float matureLeak = 1.0f / getMatureTimeConstant();
                state.leakConductance = interpolate(currentLeak, matureLeak, maturationProgress);
                
                // Update refractory period based on maturation
                if (maturationProgress > 0.8f) {
                    // Near mature: use mature refractory period
                    neuron->setRefatoryPeriod(9);  // From Neuron.hpp: 9 steps
                } else {
                    // Developing: shorter refractory period
                    int developingPeriod = static_cast<int>(9 * (1.0f - maturationProgress) + 1.0f);
                    neuron->setRefatoryPeriod(static_cast<uint32_t>(developingPeriod));
                }
            }
        }
    }
}

float Maturation::calculateAverageNetworkActivity(Brain* brain) const {
    // Calculate average network activity across all neurons
    if (!brain) return 0.0f;
    
    float totalActivity = 0.0f;
    int neuronCount = 0;
    
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                const auto& state = neuron->getState();
                float activity = (state.lastSpikeTime > 0.0f) ? 1.0f : 0.0f;
                totalActivity += activity;
                neuronCount++;
            }
        }
    }
    
    return neuronCount > 0 ? totalActivity / neuronCount : 0.0f;
}

float Maturation::calculateNetworkComplexity(Brain* brain) const {
    // Calculate network complexity based on connectivity patterns
    if (!brain) return 0.0f;
    
    float complexity = 0.0f;
    
    for (auto& region : brain->getRegions()) {
        int synapseCount = 0;
        int neuronCount = 0;
        
        for (auto& pop : region->getPopulations()) {
            neuronCount += pop->getNeuronCount();
            synapseCount += pop->getSynapseCount();
        }
        
        if (neuronCount > 0) {
            float connectivityDensity = static_cast<float>(synapseCount) / neuronCount;
            complexity += connectivityDensity;
        }
    }
    
    return complexity / std::max(1.0f, static_cast<float>(brain->getRegions().size()));
}

float Maturation::calculateEarlyDevelopmentFactors(Brain* brain, float averageActivity) const {
    // Calculate factors specific to early development
    float factors = 1.0f;
    
    // Higher maturation rate during early development
    if (averageActivity > 0.5f) {
        factors *= 1.5f;  // Activity-dependent acceleration
    }
    
    // Network size influences maturation speed
    int neuronCount = 0;
    for (auto& region : brain->getRegions()) {
        for (auto& pop : region->getPopulations()) {
            neuronCount += pop->getNeuronCount();
        }
    }
    
    if (neuronCount > 100) {
        factors *= 0.8f;  // Slower for larger networks
    }
    
    return factors;
}

float Maturation::calculateMiddleDevelopmentFactors(Brain* brain, float averageActivity, float networkComplexity) const {
    // Calculate factors specific to middle development
    float factors = 1.0f;
    
    // Balance between activity and complexity
    factors *= (1.0f + averageActivity) * 0.5f;
    factors *= (1.0f + networkComplexity) * 0.3f;
    
    return factors;
}

float Maturation::calculateLateDevelopmentFactors(Brain* brain, float averageActivity) const {
    // Calculate factors specific to late development
    float factors = 1.0f;
    
    // Fine-tuning based on final activity levels
    if (averageActivity > 0.7f) {
        factors *= 0.9f;  // Slight down-regulation for high activity
    } else if (averageActivity < 0.3f) {
        factors *= 1.2f;  // Slight up-regulation for low activity
    }
    
    return factors;
}

float Maturation::interpolate(float current, float target, float progress) const {
    // Linear interpolation between current and target values
    return current + (target - current) * progress;
}

} // namespace nlm

