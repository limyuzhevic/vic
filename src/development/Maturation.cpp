#include "Maturation.hpp"
#include "../brain/Brain.hpp"
#include <algorithm>
#include <stdexcept>
#include <cmath>
#include <unordered_map>
#include <chrono>

namespace nlm {

struct Maturation::Impl {
    float progress;
    MembranePotential matureThreshold;
    MembranePotential matureRestingPotential;
    float matureTimeConstant;
    std::unordered_map<NeuronId, std::chrono::steady_clock::time_point> neuronMaturationTimes;
    float basePlasticityRate;
    
    Impl() 
        : progress(0.0f)
        , matureThreshold(-55.0f)
        , matureRestingPotential(-70.0f)
        , matureTimeConstant(20.0f)
        , basePlasticityRate(1.0f) {}
    
    ~Impl() {
        neuronMaturationTimes.clear();
    }
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

float Maturation::getProgress() const {
    return pImpl->progress;
}

void Maturation::setProgress(float progress) {
    if (progress < 0.0f || progress > 1.0f) {
        throw std::invalid_argument("Progress must be between 0 and 1");
    }
    pImpl->progress = progress;
}

void Maturation::update(Brain* brain, SimulationStep currentStep) {
    if (!brain) {
        throw std::invalid_argument("Brain pointer cannot be null");
    }
    
    try {
        // Get all neurons in the brain
        std::vector<Neuron*> allNeurons;
        for (const auto& region : brain->getRegions()) {
            for (auto& pop : region->getPopulations()) {
                for (Neuron* n : pop->getNeurons()) {
                    allNeurons.push_back(n);
                }
            }
        }
        
        if (allNeurons.empty()) {
            return;
        }
        
        // Update maturation progress based on elapsed time
        float maturationSpeed = 0.01f;  // Progress per step
        pImpl->progress = std::min(pImpl->progress + maturationSpeed, 1.0f);
        
        // Apply maturation effects to neurons
        for (Neuron* neuron : allNeurons) {
            if (!neuron) continue;
            
            // Get neuron state and update based on maturation progress
            NeuronState& state = neuron->getState();
            
            // Smooth transition of neuron properties towards mature values
            float maturationFactor = pImpl->progress;
            
            // Update membrane threshold (becomes more negative = more stable)
            state.threshold = -55.0f + maturationFactor * 15.0f;  // From -40 to -55
            
            // Update resting potential (becomes more negative)
            state.restingPotential = -70.0f + maturationFactor * 10.0f;  // From -60 to -70
            
            // Update time constant (decreases with maturation for faster dynamics)
            state.timeConstant = 20.0f * (1.0f - maturationFactor * 0.5f);  // From 20 to 10
            
            // Update synaptic weights based on maturation stage
            if (pImpl->progress > 0.5f) {
                // Later maturation stages stabilize weights
                float stabilizationFactor = (pImpl->progress - 0.5f) * 2.0f;  // 0 to 1
                for (auto* synapse : neuron->getIncomingSynapses()) {
                    if (synapse) {
                        // Stabilize synaptic weights towards their current value
                        float currentWeight = synapse->getWeight();
                        float stabilizedWeight = currentWeight * (1.0f - stabilizationFactor * 0.3f);
                        synapse->addToWeight(stabilizedWeight - currentWeight);
                    }
                }
            }
            
            // Update developmental stage properties
            if (pImpl->progress > 0.3f && pImpl->progress <= 0.6f) {
                // Critical period - increased plasticity for sensory learning
                brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            } else if (pImpl->progress > 0.6f && pImpl->progress <= 0.9f) {
                // Maturation period - normal learning
                brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
            } else if (pImpl->progress > 0.9f) {
                // Adult stage - refined, stable network
                brain->setDevelopmentalStage(DevelopmentalStage::Adult);
            }
        }
        
        // Record maturation times for neurons that just matured
        auto now = std::chrono::steady_clock::time_point();
        for (Neuron* neuron : allNeurons) {
            if (neuron) {
                NeuronId neuronId = neuron->getId();
                // Record if this is a new neuron or if it's reaching a maturation milestone
                if (pImpl->progress > 0.33f && pImpl->progress <= 0.66f) {
                    // Record maturation time at mid-development
                    pImpl->neuronMaturationTimes[neuronId] = now;
                }
            }
        }
        
        // Clean up old maturation records
        auto currentTime = std::chrono::steady_clock::now();
        for (auto it = pImpl->neuronMaturationTimes.begin(); it != pImpl->neuronMaturationTimes.end(); ) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(currentTime - it->second);
            if (duration.count() > 3600) {  // Remove records older than 1 hour
                it = pImpl->neuronMaturationTimes.erase(it);
            } else {
                ++it;
            }
        }
        
    } catch (const std::exception& e) {
        // Log error but don't crash the system
        // In a real implementation, this would use the logging system
        throw;
    }
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

} // namespace nlm
