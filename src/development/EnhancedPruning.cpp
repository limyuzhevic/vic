#include "EnhancedPruning.hpp"
#include "../../brain/Brain.hpp"
#include "../../brain/Synapse.hpp"
#include "../../core/RandomGenerator.hpp"
#include <cmath>
#include <algorithm>
#include <vector>
#include <map>
#include <queue>
#include <set>

namespace nlm {

struct EnhancedPruning::Impl {
    float threshold;                   // Basic pruning threshold
    float experienceThreshold;          // Experience-based pruning threshold
    float activityThreshold;             // Activity-based pruning threshold
    float targetDensity;                 // Target synaptic density
    bool homeostaticScaling;            // Enable homeostatic scaling
    
    // Pruning strategy
    PruningStrategy strategy;
    
    // Connectivity preservation
    float connectivityPreservation;     // Minimum connectivity to maintain
    
    // State tracking
    std::map<SynapseId, float> synapseExperience;     // Track synapse experience
    std::map<SynapseId, float> synapseActivity;       // Track synapse activity
    std::map<NeuronId, int> neuronConnectionCount;    // Track neuron connectivity
    
    Impl() : threshold(0.5f), experienceThreshold(0.2f), activityThreshold(0.1f),
             targetDensity(0.1f), homeostaticScaling(true),
             strategy(EXPERIENCE_DEPENDENT), connectivityPreservation(0.8f) {}
};

EnhancedPruning::EnhancedPruning() : pImpl(new Impl) {}

EnhancedPruning::~EnhancedPruning() = default;

void EnhancedPruning::update(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Get current synapses
    std::vector<Synapse*> synapses = brain->getAllSynapses();
    
    // Update synapse statistics
    updateSynapseStatistics(brain, synapses);
    
    // Determine which synapses to prune
    std::vector<Synapse*> toPrune = selectSynapsesToPrune(brain, synapses, rng);
    
    // Apply pruning
    for (Synapse* synapse : toPrune) {
        pruneSynapse(synapse);
    }
    
    // Apply homeostatic scaling if enabled
    if (pImpl->homeostaticScaling) {
        applyHomeostaticScaling(brain, rng);
    }
}

bool EnhancedPruning::pruneSynapse(Synapse* synapse) {
    if (!synapse) return false;
    
    // Check connectivity preservation constraint
    NeuronId pre = synapse->getSourceNeuron();
    NeuronId post = synapse->getDestinationNeuron();
    
    // Check if neuron already meets minimum connectivity
    if (pImpl->neuronConnectionCount[pre] < 1 || pImpl->neuronConnectionCount[post] < 1) {
        return false;
    }
    
    // Get structural plasticity controller
    auto* structuralPlasticity = new StructuralPlasticity();
    
    // Use structural plasticity to remove the synapse
    return structuralPlasticity->removeSynapse(nullptr, synapse->getId());
}

void EnhancedPruning::setExperienceThreshold(float threshold) {
    pImpl->experienceThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float EnhancedPruning::getExperienceThreshold() const {
    return pImpl->experienceThreshold;
}

void EnhancedPruning::setActivityThreshold(float threshold) {
    pImpl->activityThreshold = std::clamp(threshold, 0.0f, 1.0f);
}

float EnhancedPruning::getActivityThreshold() const {
    return pImpl->activityThreshold;
}

void EnhancedPruning::setTargetDensity(float density) {
    pImpl->targetDensity = std::clamp(density, 0.0f, 1.0f);
}

float EnhancedPruning::getTargetDensity() const {
    return pImpl->targetDensity;
}

void EnhancedPruning::setHomeostaticScaling(bool enabled) {
    pImpl->homeostaticScaling = enabled;
}

bool EnhancedPruning::isHomeostaticScalingEnabled() const {
    return pImpl->homeostaticScaling;
}

void EnhancedPruning::setPruningStrategy(PruningStrategy strategy) {
    pImpl->strategy = strategy;
}

EnhancedPruning::PruningStrategy EnhancedPruning::getPruningStrategy() const {
    return pImpl->strategy;
}

void EnhancedPruning::setConnectivityPreservation(float preservation) {
    pImpl->connectivityPreservation = std::clamp(preservation, 0.0f, 1.0f);
}

float EnhancedPruning::getConnectivityPreservation() const {
    return pImpl->connectivityPreservation;
}

void EnhancedPruning::updateSynapseStatistics(Brain* brain, const std::vector<Synapse*>& synapses) {
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        SynapseId id = synapse->getId();
        
        // Update experience (based on synapse age and usage)
        pImpl->synapseExperience[id] = 
            pImpl->synapseExperience[id] + 1.0f;
        
        // Update activity (based on spike history)
        const auto& preSpikes = synapse->getPreSpikeHistory();
        const auto& postSpikes = synapse->getPostSpikeHistory();
        
        float activity = static_cast<float>(preSpikes.size() + postSpikes.size());
        pImpl->synapseActivity[id] = activity;
        
        // Update neuron connection counts
        NeuronId pre = synapse->getSourceNeuron();
        NeuronId post = synapse->getDestinationNeuron();
        
        pImpl->neuronConnectionCount[pre]++;
        pImpl->neuronConnectionCount[post]++;
    }
}

std::vector<Synapse*> EnhancedPruning::selectSynapsesToPrune(
    Brain* brain,
    const std::vector<Synapse*>& synapses,
    RandomGenerator& rng) {
    
    std::vector<Synapse*> toPrune;
    
    switch (pImpl->strategy) {
        case EXPERIENCE_DEPENDENT:
            toPrune = selectExperienceBasedPrune(synapses);
            break;
            
        case ACTIVITY_DEPENDENT:
            toPrune = selectActivityBasedPrune(synapses);
            break;
            
        case COMPETITIVE:
            toPrune = selectCompetitivePrune(brain, synapses, rng);
            break;
            
        case HOMEOSTATIC:
            toPrune = selectHomeostaticPrune(brain, synapses);
            break;
            
        case DEVELOPER_GUIDED:
            toPrune = selectDeveloperGuidedPrune(brain, synapses, rng);
            break;
            
        default:
            toPrune = selectBasicPrune(synapses);
            break;
    }
    
    // Apply connectivity preservation constraint
    return applyConnectivityPreservation(brain, toPrune);
}

std::vector<Synapse*> EnhancedPruning::selectExperienceBasedPrune(
    const std::vector<Synapse*>& synapses) {
    
    std::vector<Synapse*> toPrune;
    
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        SynapseId id = synapse->getId();
        
        // Prune if experience is below threshold
        if (pImpl->synapseExperience.count(id) > 0 && 
            pImpl->synapseExperience[id] < pImpl->experienceThreshold) {
            toPrune.push_back(synapse);
        }
    }
    
    // Sort by experience (prune least experienced first)
    std::sort(toPrune.begin(), toPrune.end(),
        [this](Synapse* a, Synapse* b) {
            SynapseId idA = a->getId();
            SynapseId idB = b->getId();
            return (pImpl->synapseExperience[idA] < pImpl->synapseExperience[idB]);
        });
    
    return toPrune;
}

std::vector<Synapse*> EnhancedPruning::selectActivityBasedPrune(
    const std::vector<Synapse*>& synapses) {
    
    std::vector<Synapse*> toPrune;
    
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        SynapseId id = synapse->getId();
        
        // Prune if activity is below threshold
        if (pImpl->synapseActivity.count(id) > 0 &&
            pImpl->synapseActivity[id] < pImpl->activityThreshold) {
            toPrune.push_back(synapse);
        }
    }
    
    // Sort by activity (prune least active first)
    std::sort(toPrune.begin(), toPrune.end(),
        [this](Synapse* a, Synapse* b) {
            SynapseId idA = a->getId();
            SynapseId idB = b->getId();
            return (pImpl->synapseActivity[idA] < pImpl->synapseActivity[idB]);
        });
    
    return toPrune;
}

std::vector<Synapse*> EnhancedPruning::selectCompetitivePrune(
    Brain* brain,
    const std::vector<Synapse*>& synapses,
    RandomGenerator& rng) {
    
    std::vector<Synapse*> toPrune;
    
    // Calculate total synaptic weight
    float totalWeight = 0.0f;
    for (Synapse* synapse : synapses) {
        if (synapse) {
            totalWeight += synapse->getWeight();
        }
    }
    
    // Determine number of synapses to prune based on target density
    size_t totalSynapses = synapses.size();
    size_t targetSynapses = static_cast<size_t>(totalSynapses * pImpl->targetDensity);
    size_t toPruneCount = totalSynapses - targetSynapses;
    
    // Select synapses to prune using competition
    if (toPruneCount > 0) {
        std::vector<std::pair<float, Synapse*>> weightSynapsePairs;
        
        for (Synapse* synapse : synapses) {
            if (synapse && rng.nextFloat() < 0.3f) { // Random sampling for efficiency
                weightSynapsePairs.push_back(
                    std::make_pair(synapse->getWeight(), synapse));
            }
        }
        
        // Sort by weight (prune weakest connections)
        std::sort(weightSynapsePairs.begin(), weightSynapsePairs.end(),
            [](const std::pair<float, Synapse*>& a, const std::pair<float, Synapse*>& b) {
                return a.first < b.first;
            });
        
        // Prune weakest synapses up to toPruneCount
        size_t pruned = 0;
        for (const auto& pair : weightSynapsePairs) {
            if (pruned >= toPruneCount) break;
            toPrune.push_back(pair.second);
            pruned++;
        }
    }
    
    return toPrune;
}

std::vector<Synapse*> EnhancedPruning::selectHomeostaticPrune(
    Brain* brain,
    const std::vector<Synapse*>& synapses) {
    
    // Calculate current vs target density
    size_t totalSynapses = synapses.size();
    float targetSynapses = static_cast<float>(totalSynapses) * pImpl->targetDensity;
    
    std::vector<Synapse*> toPrune;
    
    if (totalSynapses > targetSynapses) {
        // Need to prune synapses
        size_t toPruneCount = totalSynapses - static_cast<size_t>(targetSynapses);
        
        // Select synapses for pruning based on multiple factors
        std::vector<std::pair<float, Synapse*>> pruneScorePairs;
        
        for (Synapse* synapse : synapses) {
            if (!synapse) continue;
            
            float score = calculatePruneScore(synapse);
            pruneScorePairs.push_back(std::make_pair(score, synapse));
        }
        
        // Sort by prune score (prune highest scores first)
        std::sort(pruneScorePairs.begin(), pruneScorePairs.end(),
            std::greater<std::pair<float, Synapse*>>());
        
        // Prune highest scoring synapses
        size_t pruned = 0;
        for (const auto& pair : pruneScorePairs) {
            if (pruned >= toPruneCount) break;
            toPrune.push_back(pair.second);
            pruned++;
        }
    }
    
    return toPrune;
}

std::vector<Synapse*> EnhancedPruning::selectDeveloperGuidedPrune(
    Brain* brain,
    const std::vector<Synapse*>& synapses,
    RandomGenerator& rng) {
    
    // Developer-guided pruning considers neural development stage
    // and functional maturation of synapses
    
    std::vector<Synapse*> toPrune;
    
    // Get current developmental stage from brain
    DevelopmentalStage stage = brain->getDevelopmentalStage();
    
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        // Calculate developmental appropriateness score
        float devScore = calculateDevelopmentalPruneScore(synapse, stage);
        
        // Prune if score indicates synapse is developmentally inappropriate
        if (devScore < 0.3f) {
            // Add small random variation
            if (rng.nextFloat() < 0.7f + devScore) {
                toPrune.push_back(synapse);
            }
        }
    }
    
    return toPrune;
}

std::vector<Synapse*> EnhancedPruning::selectBasicPrune(
    const std::vector<Synapse*>& synapses) {
    
    std::vector<Synapse*> toPrune;
    
    for (Synapse* synapse : synapses) {
        if (!synapse) continue;
        
        // Basic threshold pruning
        if (std::abs(synapse->getWeight()) < pImpl->threshold) {
            toPrune.push_back(synapse);
        }
    }
    
    return toPrune;
}

void EnhancedPruning::applyHomeostaticScaling(Brain* brain, RandomGenerator& rng) {
    if (!brain) return;
    
    // Get current synapses
    std::vector<Synapse*> synapses = brain->getAllSynapses();
    
    // Calculate total synaptic weight
    float totalWeight = 0.0f;
    for (Synapse* synapse : synapses) {
        if (synapse) {
            totalWeight += synapse->getWeight();
        }
    }
    
    // Calculate target total weight
    float targetTotalWeight = totalWeight * pImpl->targetDensity;
    
    // Scale synaptic weights to reach target
    float scaleFactor = (totalWeight > 0.0f) ? 
        targetTotalWeight / totalWeight : 1.0f;
    
    // Apply scaling
    for (Synapse* synapse : synapses) {
        if (synapse) {
            float newWeight = synapse->getWeight() * scaleFactor;
            synapse->setWeight(newWeight);
        }
    }
}

float EnhancedPruning::calculatePruneScore(Synapse* synapse) {
    if (!synapse) return 0.0f;
    
    SynapseId id = synapse->getId();
    
    float score = 0.0f;
    
    // Factor 1: Synaptic weight
    score += std::abs(synapse->getWeight()) * 0.3f;
    
    // Factor 2: Synapse activity
    if (pImpl->synapseActivity.count(id) > 0) {
        score += pImpl->synapseActivity[id] * 0.4f;
    }
    
    // Factor 3: Synapse experience
    if (pImpl->synapseExperience.count(id) > 0) {
        score += (1.0f - pImpl->synapseExperience[id]) * 0.3f;
    }
    
    return score;
}

float EnhancedPruning::calculateDevelopmentalPruneScore(
    Synapse* synapse,
    DevelopmentalStage stage) {
    
    if (!synapse) return 0.0f;
    
    float score = 0.0f;
    
    // Different developmental stages favor different synaptic properties
    switch (stage) {
        case DevelopmentalStage::EMBRYONIC:
            // Embryonic stage favors formation over pruning
            score = 0.2f;
            break;
            
        case DevelopmentalStage::MATURATION:
            // Maturation stage favors pruning of weak connections
            score = std::abs(synapse->getWeight()) * 0.6f;
            break;
            
        case DevelopmentalStage::ADULT:
            // Adult stage favors homeostatic balance
            score = 0.5f;
            break;
            
        default:
            score = 0.5f;
    }
    
    // Adjust based on synapse weight
    score += std::abs(synapse->getWeight()) * 0.3f;
    
    return std::min(1.0f, score);
}

std::vector<Synapse*> EnhancedPruning::applyConnectivityPreservation(
    Brain* brain,
    const std::vector<Synapse*>& synapsesToPrune) {
    
    if (!brain) return synapsesToPrune;
    
    // Check if connectivity preservation constraint is satisfied
    size_t totalSynapses = brain->getTotalSynapseCount();
    size_t neurons = brain->getTotalNeuronCount();
    
    if (totalSynapses == 0) return synapsesToPrune;
    
    float currentDensity = static_cast<float>(totalSynapses) / static_cast<float>(neurons);
    float targetDensity = pImpl->connectivityPreservation;
    
    if (currentDensity <= targetDensity) {
        // Already meets connectivity requirement, no need for additional pruning
        return std::vector<Synapse*>();
    }
    
    // Need to ensure we don't prune below connectivity threshold
    // Filter synapses to ensure minimum connectivity is maintained
    std::vector<Synapse*> finalPrune;
    
    for (Synapse* synapse : synapsesToPrune) {
        if (!synapse) continue;
        
        // Check if pruning this synapse would drop below threshold
        if (currentDensity - 1.0f / neurons > targetDensity) {
            finalPrune.push_back(synapse);
            currentDensity -= 1.0f / neurons;
        }
    }
    
    return finalPrune;
}

} // namespace nlm
