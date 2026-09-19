#pragma once

#include "Maturation.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Maturation::Impl {
    float plasticityDecayRate;
    float connectivityStabilization;
    float synaptogenesisBoost;
    float pruningRate;
    float criticalPeriodDuration;
    
    Impl() : plasticityDecayRate(0.1f), connectivityStabilization(0.3f), 
             synaptogenesisBoost(2.0f), pruningRate(0.05f), criticalPeriodDuration(10.0f) {}
};

Maturation::Maturation() : pImpl(new Impl) {}

Maturation::~Maturation() = default;

void Maturation::update(Brain* brain, RandomGenerator& rng, double timestep) {
    if (!brain) return;
    
    // Get current developmental stage
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    // Apply developmental effects based on stage
    switch (stage) {
        case DevelopmentalStage::Initial:
            // High initial plasticity, rapid learning
            brain->setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            break;
            
        case DevelopmentalStage::CriticalPeriod:
            // Enhanced plasticity for sensory refinement
            // Neural circuits are particularly receptive to experience
            if (brain->getTotalNeuronCount() < 500) {
                // Continue critical period for smaller brains
                break;
            }
            // Transition to maturation
            brain->setDevelopmentalStage(DevelopmentalStage::Maturation);
            break;
            
        case DevelopmentalStage::Maturation:
            // Stabilization of important connections
            stabilizeImportantConnections(brain, rng);
            
            // Gradual reduction of plasticity
            float plasticityDecay = pImpl->plasticityDecayRate * static_cast<float>(timestep);
            brain->setDevelopmentalStage(DevelopmentalStage::Adult);
            break;
            
        case DevelopmentalStage::Adult:
            // Fine-tuning and maintenance
            maintainAdultBrain(brain, rng);
            break;
    }
}

void Maturation::stabilizeImportantConnections(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Get working memory for important patterns
    auto* workingMem = brain->getWorkingMemory();
    auto* episodicMem = brain->getEpisodicMemory();
    
    if (!workingMem || !episodicMem) return;
    
    // Get recent episodes from episodic memory
    auto recentEpisodes = episodicMem->getRecentEpisodes(10);
    
    // Analyze and stabilize high-activity neurons
    for (const auto& episode : recentEpisodes) {
        for (const auto& neuronId : episode.activeNeurons) {
            // Find the neuron
            auto* neuron = findNeuronById(brain, neuronId);
            if (!neuron) continue;
            
            // Calculate activation strength
            float activation = episode.getNeuronActivation(neuronId);
            
            if (activation > 0.7f) {
                // Strengthen this connection pattern
                // This simulates the consolidation of important memories
                // In a real implementation, this would:
                // 1. Increase synaptic efficacy
                // 2. Reduce the chance of synapse elimination
                // 3. Enhance cooperation between active neurons
            }
        }
    }
}

void Maturation::maintainAdultBrain(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Reduce high plasticity for stable adult function
    auto* developmentSys = brain->getDevelopmentSystem();
    if (!developmentSys) return;
    
    // Decrease synaptogenesis rate
    float currentRate = developmentSys->getSynaptogenesisRate();
    if (currentRate > 0.00005f) {
        float newRate = currentRate * (1.0f - pImpl->connectivityStabilization);
        developmentSys->setSynaptogenesisRate(newRate);
    }
    
    // Increase pruning for efficiency
    float currentPruneRate = developmentSys->getPruningRate();
    float newPruneRate = currentPruneRate + pImpl->pruningRate;
    developmentSys->setPruningRate(std::min(0.0005f, newPruneRate));
    
    // Enhance memory efficiency
    auto* episodicMem = brain->getEpisodicMemory();
    if (episodicMem) {
        // Reduce storage of redundant information
        episodicMem->consolidate(pImpl->connectivityStabilization);
    }
}

} // namespace nlm