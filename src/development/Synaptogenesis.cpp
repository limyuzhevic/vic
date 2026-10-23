#include "Synaptogenesis.hpp"
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Random/Random.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    // Activity-dependent synaptogenesis parameters
    float hebbianWeight;
    float activityThreshold;
    float targetNeuronIdleness;
    float connectivityDecay;
    
    // Neuromodulation influence
    float dopamineBoost;
    float neuromodulationExponent;
    
    // Structural constraints
    float maxSynapsesPerNeuron;
    float minDistanceForNewSynapse;
    
    // Developmental modulation
    float developmentalBoost;
    float criticalPeriodDuration;
    
    // Competition and pruning parameters
    float competitionStrength;
    float redundancyThreshold;
    
    // State tracking
    std::vector<NeuronId> recentlyActiveNeurons;
    std::vector<NeuronId> idleNeurons;
    std::vector<std::pair<NeuronId, NeuronId>> existingConnections;
    
    Impl() : 
        formationRate(0.001f), targetDensity(0.1f),
        hebbianWeight(0.1f), activityThreshold(0.5f),
        targetNeuronIdleness(0.8f), connectivityDecay(0.99f),
        dopamineBoost(1.5f), neuromodulationExponent(1.2f),
        maxSynapsesPerNeuron(50.0f), minDistanceForNewSynapse(0.1f),
        developmentalBoost(1.0f), criticalPeriodDuration(1000.0f),
        competitionStrength(0.3f), redundancyThreshold(0.9f) {}
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
    
    // Get neuromodulation influence
    float neuromodulationFactor = 1.0f;
    if (auto* dopamine = brain->getDopamine()) {
        neuromodulationFactor = dopamine->getPlasticityFactor() / dopamine->getPlasticityFactor(); // Normalize
    }
    
    neuromodulationFactor *= pImpl->dopamineBoost;
    neuromodulationFactor = std::pow(neuroregulationFactor, pImpl->neuromodulationExponent);
    
    // Get current brain state
    auto regions = brain->getRegions();
    
    // Reset state tracking
    pImpl->recentlyActiveNeurons.clear();
    pImpl->idleNeurons.clear();
    
    // First pass: identify recently active neurons and idle neurons
    for (const auto& region : regions) {
        for (const auto& pop : region->getPopulations()) {
            for (const auto* neuron : pop->getNeurons()) {
                float activityLevel = computeNeuronActivity(neuron);
                NeuronId neuronId = neuron->getId();
                
                if (activityLevel > pImpl->activityThreshold) {
                    pImpl->recentlyActiveNeurons.push_back(neuronId);
                } else {
                    pImpl->idleNeurons.push_back(neuronId);
                }
                
                // Track existing connections
                auto incomingSynapses = region->getSynapsesTo(neuronId);
                auto outgoingSynapses = region->getSynapsesFrom(neuronId);
                pImpl->existingConnections.reserve(incomingSynapses.size() + outgoingSynapses.size());
                
                for (const auto* syn : incomingSynapses) {
                    pImpl->existingConnections.push_back(std::make_pair(syn->getSourceNeuron(), neuronId));
                }
                for (const auto* syn : outgoingSynapses) {
                    pImpl->existingConnections.push_back(std::make_pair(neuronId, syn->getDestinationNeuron()));
                }
            }
        }
    }
    
    // Calculate current synapse density
    float currentDensity = calculateCurrentDensity(pImpl->existingConnections, brain);
    
    // Determine if we need new synapses
    float targetSynapseCount = calculateTargetSynapseCount(brain);
    float currentSynapseCount = static_cast<float>(pImpl->existingConnections.size());
    
    if (currentSynapseCount >= targetSynapseCount * 0.95f) {
        // Close to target, skip synaptogenesis
        return;
    }
    
    // Determine number of new synapses to create
    size_t maxNewSynapses = static_cast<size_t>((targetSynapseCount - currentSynapseCount) * pImpl->formationRate);
    if (maxNewSynapses == 0) return;
    
    // Create new synapses
    createNewSynapses(brain, rng, maxNewSynapses, neuromodulationFactor);
    
    // Update developmental state
    updateDevelopmentalState(brain);
    
    // Apply competition and redundancy checks
    applyCompetitionRules(brain, rng);
}

float Synaptogenesis::Impl::calculateCurrentDensity(const std::vector<std::pair<NeuronId, NeuronId>>& connections, Brain* brain) const {
    if (!brain) return 0.0f;
    
    size_t totalPotentialConnections = 0;
    size_t totalExistingConnections = connections.size();
    
    // Estimate total potential connections (simplified)
    for (const auto& region : brain->getRegions()) {
        size_t neuronCount = region->getTotalNeuronCount();
        size_t popCount = region->getPopulationCount();
        
        if (popCount > 1) {
            // Rough estimate of potential connections
            totalPotentialConnections += neuronCount * neuronCount * 0.01f; // 1% connection probability
        }
    }
    
    if (totalPotentialConnections == 0) return 0.0f;
    
    return static_cast<float>(totalExistingConnections) / static_cast<float>(totalPotentialConnections);
}

size_t Synaptogenesis::Impl::calculateTargetSynapseCount(Brain* brain) const {
    if (!brain) return 0;
    
    size_t totalNeurons = 0;
    for (const auto& region : brain->getRegions()) {
        totalNeurons += region->getTotalNeuronCount();
    }
    
    // Target synapse count based on target density
    float targetDensity = std::min(targetDensity, 1.0f); // Clamp to 1.0
    return static_cast<size_t>(totalNeurons * totalNeurons * targetDensity * 0.01f);
}

void Synaptogenesis::Impl::createNewSynapses(Brain* brain, RandomGenerator& rng, size_t maxNewSynapses, float neuromodulationFactor) {
    if (!brain || maxNewSynapses == 0) return;
    
    std::vector<std::pair<NeuronId, NeuronId>> candidateConnections;
    
    // Generate candidate connections based on activity patterns
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        if (neurons.size() < 2) continue;
        
        // Shuffle neurons for random sampling
        std::shuffle(neurons.begin(), neurons.end(), rng);
        
        // Sample neuron pairs for potential new synapses
        size_t sampleSize = std::min(size_t(50), neurons.size() / 2);
        
        for (size_t i = 0; i < sampleSize; ++i) {
            for (size_t j = i + 1; j < sampleSize && j < neurons.size(); ++j) {
                NeuronId sourceId = neurons[i]->getId();
                NeuronId targetId = neurons[j]->getId();
                
                // Check if connection already exists
                bool connectionExists = false;
                for (const auto& conn : existingConnections) {
                    if ((conn.first == sourceId && conn.second == targetId) ||
                        (conn.first == targetId && conn.second == sourceId)) {
                        connectionExists = true;
                        break;
                    }
                }
                
                if (connectionExists) continue;
                
                // Check for redundancy (too similar neurons)
                if (isRedundantConnection(sourceId, targetId, neurons)) {
                    continue;
                }
                
                // Calculate connection probability based on activity
                float connectionProb = calculateConnectionProbability(sourceId, targetId, neurons);
                
                if (rng.randFloat() < connectionProb) {
                    candidateConnections.push_back(std::make_pair(sourceId, targetId));
                }
                
                if (candidateConnections.size() >= maxNewSynapses) {
                    break;
                }
            }
            
            if (candidateConnections.size() >= maxNewSynapses) {
                break;
            }
        }
    }
    
    // Create actual synapses for candidates
    for (const auto& conn : candidateConnections) {
        NeuronId sourceId = conn.first;
        NeuronId targetId = conn.second;
        
        // Find the actual neurons
        Neuron* sourceNeuron = nullptr;
        Neuron* targetNeuron = nullptr;
        
        for (const auto& region : brain->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (Neuron* neuron : neurons) {
                if (neuron->getId() == sourceId) sourceNeuron = neuron;
                if (neuron->getId() == targetId) targetNeuron = neuron;
                
                if (sourceNeuron && targetNeuron) break;
            }
            
            if (sourceNeuron && targetNeuron) break;
        }
        
        if (sourceNeuron && targetNeuron) {
            // Check synapse limit per neuron
            if (getSynapseCountForNeuron(sourceNeuron) >= maxSynapsesPerNeuron ||
                getSynapseCountForNeuron(targetNeuron) >= maxSynapsesPerNeuron) {
                continue;
            }
            
            // Create the synapse
            createSynapseBetween(brain, sourceNeuron, targetNeuron, rng, neuromodulationFactor);
        }
    }
}

bool Synaptogenesis::Impl::isRedundantConnection(NeuronId sourceId, NeuronId targetId, const std::vector<Neuron*>& neurons) const {
    // Check if neurons are too similar in terms of location or type
    // Simplified version: check neuron properties
    
    // Find neuron properties (would need access to neuron properties)
    // For now, use a simple heuristic based on neuron IDs
    
    // Neurons with similar IDs might be in same region/population
    if (std::abs(static_cast<int>(sourceId.index()) - static_cast<int>(targetId.index())) < 10) {
        return true; // Too similar
    }
    
    return false;
}

float Synaptogenesis::Impl::calculateConnectionProbability(NeuronId sourceId, NeuronId targetId, const std::vector<Neuron*>& neurons) const {
    // Calculate probability based on:
    // 1. Activity correlation (Hebbian)
    // 2. Neuromodulation influence
    // 3. Developmental stage
    // 4. Competition
    
    float baseProbability = 0.01f; // Base connection probability
    
    // Add activity-dependent component (Hebbian-like)
    float activityComponent = getActivityCorrelation(sourceId, targetId, neurons) * hebbianWeight;
    
    // Add neuromodulation component
    float neuromodulationComponent = neuromodulationFactor * 0.1f;
    
    // Add developmental component
    float developmentalComponent = developmentalBoost * 0.05f;
    
    // Add competition component (reduce probability if too connected)
    float competitionComponent = getConnectionLoad(sourceId, targetId) * competitionStrength * 0.5f;
    
    // Combine components
    float probability = baseProbability + activityComponent + neuromodulationComponent + 
                       developmentalComponent - competitionComponent;
    
    // Clamp probability
    return std::max(0.0f, std::min(probability, 0.1f));
}

float Synaptogenesis::Impl::getActivityCorrelation(NeuronId sourceId, NeuronId targetId, const std::vector<Neuron*>& neurons) const {
    // Simplified activity correlation based on neuron types and positions
    // In a real implementation, this would use actual neural activity history
    
    // Heuristic: neurons with different types and positions have lower correlation
    // This promotes diverse connectivity
    
    float correlation = 0.5f; // Base correlation
    
    // Reduce correlation for neurons close in ID space (likely same region)
    float idDifference = std::abs(static_cast<int>(sourceId.index()) - static_cast<int>(targetId.index()));
    if (idDifference < 20) {
        correlation *= 0.5f;
    }
    
    return correlation;
}

size_t Synaptogenesis::Impl::getConnectionLoad(NeuronId sourceId, NeuronId targetId) const {
    // Get current connection load for this pair
    size_t load = 0;
    for (const auto& conn : existingConnections) {
        if ((conn.first == sourceId && conn.second == targetId) ||
            (conn.first == targetId && conn.second == sourceId)) {
            load++;
        }
    }
    return load;
}

size_t Synaptogenesis::Impl::getSynapseCountForNeuron(Neuron* neuron) const {
    // Count synapses connected to this neuron
    size_t count = 0;
    NeuronId neuronId = neuron->getId();
    
    for (const auto& conn : existingConnections) {
        if (conn.first == neuronId || conn.second == neuronId) {
            count++;
        }
    }
    
    return count;
}

void Synaptogenesis::Impl::createSynapseBetween(Brain* brain, Neuron* sourceNeuron, Neuron* targetNeuron, 
                                                RandomGenerator& rng, float neuromodulationFactor) {
    // Find the region containing these neurons
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        bool sourceFound = false, targetFound = false;
        
        for (Neuron* neuron : neurons) {
            if (neuron == sourceNeuron) sourceFound = true;
            if (neuron == targetNeuron) targetFound = true;
        }
        
        if (sourceFound && targetFound) {
            // Create synapse in this region
            // For now, we'll use the brain's addInterRegionConnection method
            // In a real implementation, synapses would be created at the region level
            
            // This is a simplified version - in reality, synapse creation would
            // involve more complex biological rules and region-specific logic
            break;
        }
    }
}

float Synaptogenesis::Impl::computeNeuronActivity(Neuron* neuron) const {
    // Compute neuron activity based on membrane potential
    if (!neuron) return 0.0f;
    
    const auto& state = neuron->getState();
    float normalizedPotential = (state.membranePotential - state.restingPotential) /
                                std::max(state.threshold - state.restingPotential, 1.0f);
    
    return std::clamp(normalizedPotential, 0.0f, 1.0f);
}

void Synaptogenesis::Impl::updateDevelopmentalState(Brain* brain) {
    if (!brain) return;
    
    // Update developmental stage effects
    auto developmentSystem = brain->getDevelopmentSystem();
    if (!developmentSystem) return;
    
    auto developmentalStage = brain->getDevelopmentalStage();
    
    // Adjust synaptogenesis rate based on developmental stage
    switch (developmentalStage) {
        case DevelopmentalStage::Initial:
            developmentalBoost = 2.0f; // High plasticity early
            break;
        case DevelopmentalStage::CriticalPeriod:
            developmentalBoost = 1.5f; // Elevated plasticity
            break;
        case DevelopmentalStage::Maturation:
            developmentalBoost = 1.0f; // Normal plasticity
            break;
        case DevelopmentalStage::Adult:
            developmentalBoost = 0.8f; // Reduced plasticity
            break;
    }
}

void Synaptogenesis::Impl::applyCompetitionRules(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Apply competition rules to prevent over-connectivity
    // This is a simplified version
    
    for (const auto& region : brain->getRegions()) {
        auto neurons = region->getAllNeurons();
        
        // Identify neurons with too many connections
        std::vector<Neuron*> overconnectedNeurons;
        for (Neuron* neuron : neurons) {
            if (getSynapseCountForNeuron(neuron) > maxSynapsesPerNeuron * redundancyThreshold) {
                overconnectedNeurons.push_back(neuron);
            }
        }
        
        // Prune some synapses from overconnected neurons (simplified)
        for (Neuron* neuron : overconnectedNeurons) {
            // In a real implementation, this would involve actual synaptic pruning
            // For now, just log the need for pruning
        }
    }
}

} // namespace nlm
