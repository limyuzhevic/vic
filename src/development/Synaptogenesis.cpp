#include "Synaptogenesis.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Neuron.hpp"
#include "../../brain/NeuralRegion.hpp"
#include "../../core/Random/Random.hpp"
#include "../../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_set>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    float criticalPeriodBonus;
    float activityDependency;
    float maturityModulation;
    
    // Statistics
    size_t totalSynapsesCreated;
    size_t totalSpontaneousCreations;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f),
             criticalPeriodBonus(2.0f), activityDependency(0.7f), 
             maturityModulation(0.5f), totalSynapsesCreated(0), totalSpontaneousCreations(0) {}
};

Synaptogenesis::Synaptogenesis() : pImpl(new Impl) {}

Synaptogenesis::~Synaptogenesis() = default;

float Synaptogenesis::getFormationRate() const {
    return pImpl->formationRate;
}

void Synaptogenesis::setFormationRate(float rate) {
    pImpl->formationRate = std::clamp(rate, 0.0f, 0.1f);
}

float Synaptogenesis::getTargetDensity() const {
    return pImpl->targetDensity;
}

void Synaptogenesis::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.01f, 0.5f);
}

void Synaptogenesis::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Calculate current metrics
    double currentDensity = calculateConnectivityDensity(brain);
    double activityScore = calculateActivityScore(brain);
    
    // Determine developmental stage from brain's development system
    DevelopmentalStage stage = DevelopmentalStage::Initial;
    if (brain->getDevelopmentSystem()) {
        stage = brain->getDevelopmentSystem()->getStage();
    }
    
    // Activity-dependent formation rate
    float baseRate = pImpl->formationRate;
    
    // Critical period enhancement
    float criticalPeriodModifier = 1.0f;
    if (stage == DevelopmentalStage::CriticalPeriod) {
        criticalPeriodModifier = pImpl->criticalPeriodBonus;
    }
    
    // Activity dependency
    float activityModifier = 1.0f + pImpl->activityDependency * activityScore;
    
    // Neuron maturity modulation
    float maturityModifier = 1.0f + pImpl->maturityModulation * brain->getDevelopmentalAge() / 100.0f;
    
    // Effective formation rate
    float effectiveRate = baseRate * criticalPeriodModifier * activityModifier * maturityModifier;
    
    // Target-driven formation
    float targetDrivenRate = 0.0f;
    if (currentDensity < pImpl->targetDensity) {
        targetDrivenRate = (pImpl->targetDensity - currentDensity) * 0.1f;
    }
    
    // Use the higher of activity-driven and target-driven formation
    float formationProbability = std::max(effectiveRate, targetDrivenRate);
    formationProbability = std::min(formationRate, formationProbability);
    
    // Attempt synapse formation
    attemptSynaptogenesis(brain, rng, formationProbability, currentDensity, activityScore);
    
    // Log statistics
    if (pImpl->totalSynapsesCreated > 0) {
        NLM_LOG_DEBUG("Synaptogenesis: created " + std::to_string(pImpl->totalSynapsesCreated) + 
                     " synapses, density: " + std::to_string(currentDensity));
    }
}

double Synaptogenesis::calculateConnectivityDensity(Brain* brain) const {
    if (!brain) return 0.0;
    
    size_t totalSynapses = 0;
    size_t maxPossibleSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            size_t neurons = population->getNeuronCount();
            maxPossibleSynapses += neurons * neurons; // Complete graph assumption
        }
        totalSynapses += region->getSynapseCount();
    }
    
    return maxPossibleSynapses > 0 ? static_cast<double>(totalSynapses) / maxPossibleSynapses : 0.0;
}

double Synaptogenesis::calculateActivityScore(Brain* brain) const {
    if (!brain) return 0.0;
    
    double totalActivity = 0.0;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        for (const auto& population : region->getPopulations()) {
            const auto& spikes = population->getSpikeHistory();
            if (!spikes.empty()) {
                double activity = static_cast<double>(spikes.size());
                totalActivity += activity;
                neuronCount++;
            }
        }
    }
    
    return neuronCount > 0 ? totalActivity / neuronCount : 0.0;
}

void Synaptogenesis::attemptSynaptogenesis(Brain* brain, RandomGenerator& rng, 
                                            float formationProbability, 
                                            double currentDensity, double activityScore) {
    if (!brain) return;
    
    std::unordered_set<std::pair<NeuronId, NeuronId>> existingConnections;
    
    // Collect existing connections
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        for (size_t i = 0; i < neurons.size(); ++i) {
            for (size_t j = i + 1; j < neurons.size(); ++j) {
                NeuronId id1 = neurons[i]->getId();
                NeuronId id2 = neurons[j]->getId();
                existingConnections.insert({id1, id2});
                existingConnections.insert({id2, id1});
            }
        }
    }
    
    // Attempt synapse formation
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        size_t neuronCount = neurons.size();
        
        if (neuronCount < 2) continue;
        
        // For each neuron, attempt to form connections
        for (size_t i = 0; i < neuronCount; ++i) {
            NeuronId source = neurons[i]->getId();
            double sourceMaturity = neurons[i]->getMaturity();
            
            // Get activity level
            float sourceActivity = 0.0f;
            const auto& spikes = neurons[i]->getSpikeHistory();
            if (!spikes.empty()) {
                sourceActivity = static_cast<float>(spikes.size());
            }
            
            // Only form synapses from mature neurons
            if (sourceMaturity < 0.1) continue;
            
            // Probabilistic synapse formation
            if (!rng.bernoulli(formationProbability)) continue;
            
            // Choose random target neuron
            size_t targetIdx = rng.uniformInt(0, static_cast<int>(neuronCount) - 1);
            if (targetIdx == static_cast<size_t>(i)) continue;
            
            NeuronId target = neurons[targetIdx]->getId();
            
            // Check if connection already exists
            if (existingConnections.find({source, target}) != existingConnections.end()) {
                continue;
            }
            
            // Activity-based weight determination
            float targetMaturity = neurons[targetIdx]->getMaturity();
            double avgMaturity = (sourceMaturity + targetMaturity) / 2.0;
            
            // Initial synaptic weight based on maturity and activity
            float weight = 0.05f + 0.15f * avgMaturity * (1.0f + sourceActivity / 10.0f);
            weight = std::clamp(weight, 0.01f, 0.5f);
            
            // Create synapse if within limits
            if (region->getSynapseCount() < region->getMaxSynapses()) {
                SynapseId synId = region->addSynapse(source, target, weight, 1);
                if (synId != INVALID_SYNAPSE_ID) {
                    ++pImpl->totalSynapsesCreated;
                    if (sourceActivity < 0.1f) {
                        ++pImpl->totalSpontaneousCreations;
                    }
                }
            }
        }
    }
}

} // namespace nlm
