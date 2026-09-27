#include "Synaptogenesis.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Neuron.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Synaptogenesis::Impl {
    float formationRate;
    float targetDensity;
    
    Impl() : formationRate(0.001f), targetDensity(0.1f) {}
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
    // Phase 2: Implement real synaptogenesis (formation of new synapses)
    // Activity-dependent synapse formation based on Hebbian principles and competitive mechanisms
    
    if (!brain) return;
    
    // Get current synapse count for density calculation
    size_t currentSynapseCount = 0;
    for (const auto& region : brain->getRegions()) {
        currentSynapseCount += region->getSynapseCount();
    }
    
    // Estimate maximum possible synapses using biological constraints
    // Assume ~10 synapses per neuron on average with regional variations
    size_t maxPossibleSynapses = calculateMaxPossibleSynapses(brain);
    
    // Calculate target synapse count based on target density with developmental modulation
    float developmentalModulation = 1.0f; // Will be set based on developmental stage
    float targetSynapseCount = maxPossibleSynapses * pImpl->targetDensity * developmentalModulation;
    float currentDensity = static_cast<float>(currentSynapseCount) / maxPossibleSynapses;
    
    // Synaptogenesis only occurs when below target density
    if (currentDensity < pImpl->targetDensity) {
        // Calculate formation probability with biological constraints
        float formationProbability = calculateFormationProbability(currentDensity, brain);
        
        // Sample neurons for potential new synapse formation with competition
        std::vector<NeuronId> allNeurons = collectAllNeurons(brain);
        
        if (allNeurons.size() >= 2) {
            // Apply competitive Hebbian rules for synapse formation
            // Only form synapses between neurons with correlated activity
            int successfulFormations = attemptSynaptogenesis(allNeurons, brain, rng, formationProbability, targetSynapseCount - currentSynapseCount);
            
            // Log developmental progress
            if (successfulFormations > 0) {
                NLM_LOG_INFO("Synaptogenesis formed " + std::to_string(successfulFormations) + " new synapses");
            }
        }
    }
}

void Synaptogenesis::initializeDevelopment(Brain* brain) {
    // Initialize synaptogenesis based on developmental stage and network state
    if (!brain) return;
    
    // Set target density based on initial network properties
    size_t totalNeurons = brain->getTotalNeuronCount();
    size_t currentSynapses = 0;
    
    for (const auto& region : brain->getRegions()) {
        currentSynapses += region->getSynapseCount();
    }
    
    if (totalNeurons > 0) {
        float currentDensity = static_cast<float>(currentSynapses) / (totalNeurons * 10.0f);
        
        // Adjust target density based on initial network connectivity
        if (currentDensity < 0.05f) {
            // Sparse initial network, set higher target density
            pImpl->targetDensity = 0.15f;
        } else if (currentDensity > 0.2f) {
            // Dense initial network, set lower target density
            pImpl->targetDensity = 0.1f;
        } else {
            // Moderate density, use default target
            pImpl->targetDensity = 0.1f;
        }
    }
    
    // Set formation rate based on developmental principles
    pImpl->formationRate = 0.001f; // Base rate for synaptic formation
    
    NLM_LOG_INFO("Synaptogenesis initialized with target density: " + std::to_string(pImpl->targetDensity));
}

// Helper methods for synaptogenesis

void Synaptogenesis::calculateNeuronActivities(const std::vector<NeuronId>& allNeurons, Brain* brain, 
                                               std::vector<float>& activities) const {
    // Calculate activity levels for all neurons
    for (size_t i = 0; i < allNeurons.size(); ++i) {
        activities[i] = calculateNeuronActivity(allNeurons[i], brain);
    }
}

float Synaptogenesis::calculateNeuronActivity(NeuronId neuronId, Brain* brain) const {
    // Calculate activity for a specific neuron
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            if (neuron->getId() == neuronId) {
                // Calculate activity based on multiple factors
                float activity = 0.0f;
                
                // Firing state contribution
                if (neuron->getState().firingState == FiringState::Active) {
                    activity += 1.0f;
                } else if (neuron->getState().firingState == FiringState::Refractory) {
                    activity += 0.3f;
                }
                
                // Membrane potential deviation
                float membraneDeviation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                activity += std::max(0.0f, membraneDeviation / 20.0f);
                
                // Firing rate contribution
                float firingRate = neuron->getFiringRate();
                activity += std::min(1.0f, firingRate * 2.0f);
                
                return activity;
            }
        }
    }
    
    return 0.0f;
}

float Synaptogenesis::calculateActivityModulation(Brain* brain) const {
    // Calculate activity-based modulation for synaptogenesis
    if (!brain) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            const auto& state = neuron->getState();
            
            // Calculate activity based on firing state and membrane potential
            float activity = 0.0f;
            if (state.firingState == FiringState::Active) {
                activity = 1.5f;
            } else if (state.firingState == FiringState::Refractory) {
                activity = 0.5f;
            } else {
                float membraneDeviation = std::abs(state.membranePotential - state.restingPotential);
                activity = std::max(0.0f, membraneDeviation / 20.0f);
                
                // Boost activity for neurons with sustained firing
                float firingRate = neuron->getFiringRate();
                if (firingRate > 0.1f) {
                    activity *= (1.0f + firingRate);
                }
            }
            
            totalActivity += activity;
            ++neuronCount;
        }
    }
    
    return (neuronCount > 0) ? (totalActivity / neuronCount) : 0.0f;
}

size_t Synaptogenesis::calculateMaxPossibleSynapses(Brain* brain) const {
    // Calculate biologically plausible maximum synapses with regional variations
    size_t totalNeurons = brain->getTotalNeuronCount();
    
    if (totalNeurons == 0) return 0;
    
    // Base estimate with variations based on neuron types
    size_t baseMaxSynapses = totalNeurons * 8; // Reduced from 10 to be more realistic
    
    // Regional variations based on neuron type composition
    size_t typeBasedMaxSynapses = 0;
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        if (neurons.empty()) continue;
        
        // Count neuron types in region
        size_t totalNeuronsInRegion = neurons.size();
        size_t excitatoryNeurons = 0;
        size_t inhibitoryNeurons = 0;
        
        for (const auto& neuron : neurons) {
            NeuronType type = neuron->getType();
            if (type == NeuronType::Excitatory || type == NeuronType::Internal) {
                excitatoryNeurons++;
            } else if (type == NeuronType::Inhibitory) {
                inhibitoryNeurons++;
            }
        }
        
        // Adjust max synapses based on regional composition
        // Excitatory regions can form more connections
        float excitatoryRatio = static_cast<float>(excitatoryNeurons) / totalNeuronsInRegion;
        float inhibitoryRatio = static_cast<float>(inhibitoryNeurons) / totalNeuronsInRegion;
        
        // Higher excitatory ratios allow for more synapses
        float regionalMultiplier = 1.0f + (excitatoryRatio * 0.5f) - (inhibitoryRatio * 0.3f);
        typeBasedMaxSynapses += static_cast<size_t>(totalNeuronsInRegion * 10 * regionalMultiplier);
    }
    
    return std::max(baseMaxSynapses, typeBasedMaxSynapses);
}

float Synaptogenesis::calculateFormationProbability(float currentDensity, Brain* brain) const {
    // Calculate formation probability based on multiple biological factors
    
    // Base formation rate
    float formationProbability = pImpl->formationRate;
    
    // Activity-dependent modulation (Hebbian principle)
    float activityModulation = calculateActivityModulation(brain);
    formationProbability *= (1.0f + activityModulation);
    
    // Density-dependent regulation (homeostatic)
    formationProbability *= (1.0f + (pImpl->targetDensity - currentDensity) * 2.0f);
    
    // Development stage modulation (higher formation in early stages)
    // This would normally integrate with DevelopmentSystem
    // For now, assume developmental factor based on overall brain activity
    formationProbability *= (1.0f + calculateNetworkActivity(brain) * 0.5f);
    
    // Temporal modulation (oscillating formation rate)
    // Simulate bursts of synaptogenesis
    static float formationPhase = 0.0f;
    formationPhase += 0.1f;
    float temporalModulation = 1.0f + 0.3f * std::sin(formationPhase);
    formationProbability *= temporalModulation;
    
    // Activity-dependent consolidation
    if (formationProbability > 0.5f) {
        formationProbability = std::min(formationProbability, 0.5f); // Cap probability
    }
    
    return formationProbability;
}

float Synaptogenesis::calculateNetworkActivity(Brain* brain) const {
    // Calculate overall network activity for activity-dependent synaptogenesis
    if (!brain) return 0.0f;
    
    float totalActivity = 0.0f;
    size_t neuronCount = 0;
    
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            // Calculate activity based on firing rate and membrane potential
            float activity = 0.0f;
            if (neuron->getState().firingState == FiringState::Active) {
                activity = 1.5f;
            } else if (neuron->getState().firingState == FiringState::Refractory) {
                activity = 0.5f;
            } else {
                float membraneDeviation = std::abs(neuron->getState().membranePotential - neuron->getState().restingPotential);
                activity = std::max(0.0f, membraneDeviation / 20.0f);
                
                // Boost activity for neurons with sustained firing
                float firingRate = neuron->getFiringRate();
                if (firingRate > 0.1f) {
                    activity *= (1.0f + firingRate);
                }
            }
            
            totalActivity += activity;
            ++neuronCount;
        }
    }
    
    return (neuronCount > 0) ? (totalActivity / neuronCount) : 0.0f;
}

std::vector<NeuronId> Synaptogenesis::collectAllNeurons(Brain* brain) const {
    // Collect all neuron IDs from all regions
    std::vector<NeuronId> allNeurons;
    
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            allNeurons.push_back(neuron->getId());
        }
    }
    
    return allNeurons;
}

size_t Synaptogenesis::selectNeuronWithActivity(const std::vector<float>& activities, RandomGenerator& rng) const {
    // Select neuron based on activity level (prefer more active neurons)
    float totalActivity = 0.0f;
    for (float activity : activities) {
        totalActivity += activity;
    }
    
    if (totalActivity <= 0.0f) {
        return rng.randomIndex(activities.size());
    }
    
    float selectionValue = rng.randomFloat() * totalActivity;
    float cumulativeActivity = 0.0f;
    
    for (size_t i = 0; i < activities.size(); ++i) {
        cumulativeActivity += activities[i];
        if (cumulativeActivity >= selectionValue) {
            return i;
        }
    }
    
    return activities.size() - 1;
}

size_t Synaptogenesis::selectCorrelatedNeuron(size_t referenceIndex, size_t maxIndex, 
                                             const std::vector<float>& activities, RandomGenerator& rng) const {
    // Select neuron with correlated activity (Hebbian principle)
    float referenceActivity = activities[referenceIndex];
    
    if (referenceActivity <= 0.0f) {
        return rng.randomIndex(maxIndex);
    }
    
    // Create probability distribution favoring neurons with similar activity levels
    std::vector<float> selectionProbabilities(maxIndex, 0.0f);
    float totalProbability = 0.0f;
    
    for (size_t i = 0; i < maxIndex; ++i) {
        if (i == referenceIndex) continue;
        
        float activityDifference = std::abs(activities[i] - referenceActivity);
        float correlation = std::exp(-activityDifference * 2.0f); // Gaussian correlation
        selectionProbabilities[i] = correlation;
        totalProbability += correlation;
    }
    
    if (totalProbability <= 0.0f) {
        return rng.randomIndex(maxIndex);
    }
    
    // Normalize probabilities
    for (size_t i = 0; i < maxIndex; ++i) {
        selectionProbabilities[i] /= totalProbability;
    }
    
    // Select based on probability distribution
    float selectionValue = rng.randomFloat() * totalProbability;
    float cumulativeProbability = 0.0f;
    
    for (size_t i = 0; i < maxIndex; ++i) {
        if (i == referenceIndex) continue;
        
        cumulativeProbability += selectionProbabilities[i];
        if (cumulativeProbability >= selectionValue) {
            return i;
        }
    }
    
    // Fallback to random selection
    return rng.randomIndex(maxIndex);
}

float Synaptogenesis::calculateActivityCorrelation(size_t idx1, size_t idx2, const std::vector<float>& activities) const {
    // Calculate activity correlation between two neurons
    if (idx1 == idx2 || activities[idx1] == 0.0f || activities[idx2] == 0.0f) {
        return 1.0f; // Default correlation for self or zero activity
    }
    
    float correlation = 1.0f - std::abs(activities[idx1] - activities[idx2]) / std::max(activities[idx1], activities[idx2]);
    return std::max(0.1f, correlation); // Minimum correlation threshold
}

float Synaptogenesis::activityLevelPenalty(size_t idx1, size_t idx2, const std::vector<float>& activities) const {
    // Calculate penalty for high activity levels (homeostatic regulation)
    float penalty = 0.0f;
    
    // Penalize neurons with very high activity (prevent runaway excitation)
    float highActivityThreshold = 2.0f;
    if (activities[idx1] > highActivityThreshold) {
        penalty += (activities[idx1] - highActivityThreshold) * 0.1f;
    }
    if (activities[idx2] > highActivityThreshold) {
        penalty += (activities[idx2] - highActivityThreshold) * 0.1f;
    }
    
    return penalty;
}

bool Synaptogenesis::synapseExists(Brain* brain, NeuronId sourceId, NeuronId targetId) const {
    // Check if synapse already exists between two neurons
    for (const auto& region : brain->getRegions()) {
        const auto& synapses = region->getSynapses();
        for (const auto& synapse : synapses) {
            if ((synapse->getSourceNeuron() == sourceId && synapse->getDestinationNeuron() == targetId) ||
                (synapse->getSourceNeuron() == targetId && synapse->getDestinationNeuron() == sourceId)) {
                return true;
            }
        }
    }
    return false;
}

void Synaptogenesis::createNewSynapse(Brain* brain, NeuronId sourceId, NeuronId targetId, RandomGenerator& rng) {
    // Create a new synapse between two neurons with biologically plausible properties
    
    // Determine synapse type based on neuron types
    NeuronType sourceType = determineNeuronType(brain, sourceId);
    NeuronType targetType = determineNeuronType(brain, targetId);
    
    // Determine synapse type
    SynapseType synapseType = SynapseType::Excitatory;
    if (sourceType == NeuronType::Inhibitory && targetType == NeuronType::Excitatory) {
        synapseType = SynapseType::Inhibitory;
    } else if (sourceType == NeuronType::Sensory || targetType == NeuronType::Sensory) {
        synapseType = SynapseType::Modulatory;
    } else if (sourceType == NeuronType::Motor || targetType == NeuronType::Motor) {
        synapseType = SynapseType::Electrical;
    }
    
    // Calculate synaptic weight based on activity correlation and type
    float sourceActivity = calculateNeuronActivity(sourceId, brain);
    float targetActivity = calculateNeuronActivity(targetId, brain);
    
    float activitySum = sourceActivity + targetActivity;
    float weight = 0.0f;
    
    if (activitySum > 0.0f) {
        // Weight based on relative activities
        float sourceContribution = sourceActivity / activitySum;
        float targetContribution = targetActivity / activitySum;
        
        // Add random component
        weight = (sourceContribution * 0.8f + targetContribution * 0.2f + rng.randomFloat() * 0.2f - 0.1f);
        
        // Adjust for synapse type
        if (synapseType == SynapseType::Inhibitory) {
            weight = -weight; // Inhibitory synapses have negative weight
        }
        
        // Constrain weight range based on synapse type
        if (synapseType == SynapseType::Inhibitory) {
            weight = std::max(-1.0f, std::min(weight, -0.1f));
        } else {
            weight = std::max(-0.5f, std::min(weight, 1.0f));
        }
    } else {
        weight = rng.randomFloat() * 2.0f - 1.0f; // Range [-1, 1]
        if (synapseType == SynapseType::Inhibitory) {
            weight = -weight;
        }
    }
    
    // Find the region containing both neurons
    for (auto& region : brain->getRegions()) {
        // Check if both neurons are in this region
        bool sourceInRegion = false;
        bool targetInRegion = false;
        
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            if (neuron->getId() == sourceId) sourceInRegion = true;
            if (neuron->getId() == targetId) targetInRegion = true;
        }
        
        if (sourceInRegion && targetInRegion) {
            // Add synapse to this region
            region->addSynapse(sourceId, targetId, weight, 1); // Initial plasticity level
            return;
        }
    }
    
    // If neurons not found in same region, add to first region (simplified)
    if (!brain->getRegions().empty()) {
        brain->getRegions()[0]->addSynapse(sourceId, targetId, weight, 1);
    }
}

NeuronType Synaptogenesis::determineNeuronType(Brain* brain, NeuronId neuronId) const {
    // Determine neuron type by searching through all regions
    for (const auto& region : brain->getRegions()) {
        const auto& neurons = region->getAllNeurons();
        for (const auto& neuron : neurons) {
            if (neuron->getId() == neuronId) {
                return neuron->getType();
            }
        }
    }
    
    return NeuronType::Internal; // Default type
}

int Synaptogenesis::attemptSynaptogenesis(const std::vector<NeuronId>& allNeurons, Brain* brain, 
                                         RandomGenerator& rng, float formationProbability, size_t attempts) {
    // Attempt synaptogenesis with competitive Hebbian rules
    int successfulFormations = 0;
    
    // Calculate activity correlations for competitive selection
    std::vector<float> neuronActivities(allNeurons.size(), 0.0f);
    calculateNeuronActivities(allNeurons, brain, neuronActivities);
    
    // Sample neuron pairs with competitive selection
    size_t maxAttempts = std::min(attempts, static_cast<size_t>(100)); // Limit attempts for performance
    
    for (size_t attempt = 0; attempt < maxAttempts; ++attempt) {
        // Prefer pairs with correlated activities (Hebbian)
        size_t idx1 = selectNeuronWithActivity(neuronActivities, rng);
        size_t idx2 = selectCorrelatedNeuron(idx1, allNeurons.size(), neuronActivities, rng);
        
        while (idx2 == idx1) {
            idx2 = selectCorrelatedNeuron(idx1, allNeurons.size(), neuronActivities, rng);
        }
        
        NeuronId sourceId = allNeurons[idx1];
        NeuronId targetId = allNeurons[idx2];
        
        // Check if synapse already exists
        if (!synapseExists(brain, sourceId, targetId)) {
            // Calculate formation probability based on activity correlation
            float activityCorrelation = calculateActivityCorrelation(idx1, idx2, neuronActivities);
            
            // Enhanced formation with competitive rules
            float formationChance = formationProbability * activityCorrelation * (1.0f + activityLevelPenalty(idx1, idx2, neuronActivities));
            
            if (rng.randomFloat() < formationChance) {
                createNewSynapse(brain, sourceId, targetId, rng);
                successfulFormations++;
            }
        }
    }
    
    return successfulFormations;
}

} // namespace nlm
