#include "NeuralAssociativeMemory.hpp"
#include "../brain/Neuron.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <sstream>
#include <iomanip>
#include <queue>
#include <unordered_set>
#include <limits>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Pattern storage with sparse coding
    std::vector<std::pair<NeuronId, std::vector<float>>> patternNeurons_;
    
    // Association matrix for bidirectional memory
    // Stores (patternA, patternB, strength) tuples
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations_;
    
    // Neuromodulatory integration
    Dopamine* dopamine_;
    Acetylcholine* acetylcholine_;
    
    // Working memory integration for temporal context
    NeuralWorkingMemory* workingMemory_;
    
    // Episodic memory integration for contextual learning
    NeuralEpisodicMemory* episodicMemory_;
    
    // Competition management
    std::priority_queue<std::pair<float, size_t>> competitionQueue_;
    
    // Activation spreading for associative recall
    std::vector<float> activationBuffer_;
    std::vector<std::pair<float, size_t>> activationQueue_;
    
    // Capacity management
    size_t maxAssociations_;
    size_t maxPatterns_;
    float competitionThreshold_;
    float decayRate_;
    
    // Statistics
    size_t totalAssociationsCreated_;
    size_t totalRetrievals_;
    float averageAssociationStrength_;
    
    // Random generator for stochastic learning
    std::mt19937 rng;
    std::uniform_real_distribution<float> floatDist;
    std::bernoulli_distribution patternDist;
    std::uniform_int_distribution<size_t> patternSelectDist;
    
    Impl() 
        : brain(nullptr)
        , dopamine_(nullptr)
        , acetylcholine_(nullptr)
        , workingMemory_(nullptr)
        , episodicMemory_(nullptr)
        , maxAssociations_(1000)
        , maxPatterns_(100)
        , competitionThreshold_(0.3f)
        , decayRate_(0.005f)
        , totalAssociationsCreated_(0)
        , totalRetrievals_(0)
        , averageAssociationStrength_(0.0f)
        , rng(std::random_device{}())
        , floatDist(0.0f, 1.0f)
        , patternDist(0.8f)
        , patternSelectDist(0, maxPatterns_ - 1) {
        
        activationBuffer_.reserve(1000);
        activationQueue_.reserve(1000);
    }
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , patternNeurons_()
    , associations_()
{
    NLM_LOG_INFO("NeuralAssociativeMemory constructed with default capacity");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Get neuromodulators
    if (brain) {
        pImpl->dopamine_ = brain->getDopamine();
        pImpl->acetylcholine_ = dynamic_cast<Acetylcholine*>(brain->getDopamine());
        pImpl->workingMemory_ = brain->getWorkingMemory();
        pImpl->episodicMemory_ = brain->getEpisodicMemory();
    }
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized with neural associative memory system");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    // Find or create neurons for these patterns
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return;
    
    // Create or update association
    createAssociation(neuronA, neuronB, strength);
    
    // Apply neuromodulatory effects
    applyNeuromodulatoryAssociation(neuronA, neuronB, strength);
    
    // Integrate with working memory for context
    integrateWithWorkingMemory(patternA, patternB);
    
    // Apply plasticity rules to strengthen connections
    applyPlasticityToAssociations(neuronA, neuronB, strength);
    
    // Log association
    NLM_LOG_DEBUG(std::string("Association created: pattern1=") + 
                 std::to_string(neuronA.value) + ", pattern2=" + 
                 std::to_string(neuronB.value) + ", strength=" + 
                 std::to_string(strength));
}

void NeuralAssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    if (!brain_ || episode.sensoryState.empty()) return;
    
    // Extract patterns from the episode
    std::vector<float> patternA = episode.sensoryState;
    
    // Create context pattern from episode properties
    std::vector<float> contextPattern;
    contextPattern.reserve(5);
    contextPattern.push_back(episode.positionX);
    contextPattern.push_back(episode.positionY);
    contextPattern.push_back(episode.orientation);
    contextPattern.push_back(static_cast<float>(episode.action) / 10.0f);
    contextPattern.push_back(std::tanh(episode.reward));
    
    // Create outcome pattern from episode consequences
    std::vector<float> patternB;
    if (!episode.resultingSensoryState.empty()) {
        patternB = episode.resultingSensoryState;
    } else {
        patternB = patternA;
        for (size_t i = 0; i < patternB.size(); ++i) {
            patternB[i] *= std::tanh(episode.reward);
        }
    }
    
    // Calculate association strength based on episode properties
    float baseStrength = 0.5f;
    float rewardFactor = std::tanh(episode.reward) * 0.4f;
    float noveltyFactor = episode.novelty * 0.3f;
    float energyFactor = std::tanh(episode.energy) * 0.3f;
    
    float strength = baseStrength + rewardFactor + noveltyFactor + energyFactor;
    strength = std::min(1.0f, strength);
    
    // Create the association
    associate(patternA, patternB, strength);
    
    // Log experience-based association
    NLM_LOG_DEBUG(std::string("Experience-based association created: reward=") + 
                 std::to_string(std::fixed) + std::setprecision(3) + std::to_string(episode.reward) +
                 ", novelty=" + std::to_string(std::fixed) + std::setprecision(3) + std::to_string(episode.novelty));
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(const std::vector<float>& queryPattern,
                                                                   size_t maxResults) const {
    if (queryPattern.empty() || !brain_) return std::vector<std::vector<float>>();
    
    std::vector<std::pair<float, std::vector<float>>> scoredResults;
    
    // Find most similar pattern
    size_t bestPatternIdx = findMostSimilarPattern(queryPattern);
    
    if (bestPatternIdx != static_cast<size_t>(-1)) {
        // Retrieve the pattern
        const auto& patternPair = pImpl->patternNeurons_[bestPatternIdx];
        float patternStrength = 0.0f;
        
        // Calculate association strength from query to this pattern
        float similarity = computeSimilarity(queryPattern, patternPair.second);
        
        // Get associated patterns
        std::vector<NeuronId> associatedNeurons = getAssociatedNeurons(patternPair.first);
        
        for (NeuronId assocNeuron : associatedNeurons) {
            // Find the associated pattern
            for (const auto& patternPair : pImpl->patternNeurons_) {
                if (patternPair.first == assocNeuron) {
                    float strength = getAssociationStrength(patternPair.first, patternPair.first);
                    
                    // Combine similarity and association strength
                    float score = similarity * strength * 1.5f;
                    
                    scoredResults.emplace_back(score, patternPair.second);
                    break;
                }
            }
        }
    }
    
    // Also check direct associations of the query pattern itself
    NeuronId queryNeuron = findPatternNeuron(queryPattern);
    if (queryNeuron != INVALID_NEURON_ID) {
        std::vector<NeuronId> directAssociations = getAssociatedNeurons(queryNeuron);
        
        for (NeuronId assocNeuron : directAssociations) {
            // Find the associated pattern
            for (const auto& patternPair : pImpl->patternNeurons_) {
                if (patternPair.first == assocNeuron) {
                    // Check if we already have this pattern
                    bool alreadyAdded = false;
                    for (const auto& result : scoredResults) {
                        if (result.second == patternPair.second) {
                            alreadyAdded = true;
                            break;
                        }
                    }
                    
                    if (!alreadyAdded) {
                        scoredResults.emplace_back(0.8f, patternPair.second);
                    }
                }
            }
        }
    }
    
    // Sort by score and return top results
    std::sort(scoredResults.begin(), scoredResults.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<std::vector<float>> results;
    results.reserve(std::min(maxResults, scoredResults.size()));
    
    for (size_t i = 0; i < std::min(maxResults, scoredResults.size()); ++i) {
        results.push_back(scoredResults[i].second);
    }
    
    pImpl->totalRetrievals_ += results.size();
    
    return results;
}

float NeuralAssociativeMemory::getAssociationStrength(const std::vector<float>& patternA,
                                                    const std::vector<float>& patternB) const {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return 0.0f;
    
    return getAssociationStrength(neuronA, neuronB);
}

void NeuralAssociativeMemory::updateAssociation(const std::vector<float>& patternA,
                                                const std::vector<float>& patternB,
                                                float delta) {
    NeuronId neuronA = findPatternNeuron(patternA);
    NeuronId neuronB = findPatternNeuron(patternB);
    
    if (neuronA == INVALID_NEURON_ID || neuronB == INVALID_NEURON_ID) return;
    
    updateAssociation(neuronA, neuronB, delta);
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(const std::vector<float>& cuePattern,
                                                                           size_t steps) const {
    if (cuePattern.empty() || !brain_) return std::vector<std::vector<float>>();
    
    std::vector<std::vector<float>> activatedPatterns;
    std::queue<std::pair<std::vector<float>, size_t>> activationQueue;
    
    // Start with the cue pattern
    activationQueue.push({cuePattern, 0});
    activatedPatterns.push_back(cuePattern);
    
    while (!activationQueue.empty()) {
        auto current = activationQueue.front();
        activationQueue.pop();
        
        if (current.second >= steps) continue;
        
        // Find similar patterns to current pattern
        NeuronId cueNeuron = findPatternNeuron(current.first);
        if (cueNeuron == INVALID_NEURON_ID) continue;
        
        std::vector<NeuronId> associatedNeurons = getAssociatedNeurons(cueNeuron);
        
        for (NeuronId assocNeuron : associatedNeurons) {
            // Find the pattern for this neuron
            for (const auto& patternPair : pImpl->patternNeurons_) {
                if (patternPair.first == assocNeuron) {
                    // Check if already activated
                    bool alreadyActivated = false;
                    for (const auto& pattern : activatedPatterns) {
                        if (pattern == patternPair.second) {
                            alreadyActivated = true;
                            break;
                        }
                    }
                    
                    if (!alreadyActivated) {
                        activatedPatterns.push_back(patternPair.second);
                        activationQueue.push({patternPair.second, current.second + 1});
                    }
                }
            }
        }
    }
    
    return activatedPatterns;
}

void NeuralAssociativeMemory::clear() {
    pImpl->patternNeurons_.clear();
    pImpl->associations_.clear();
    pImpl->competitionQueue_ = std::priority_queue<std::pair<float, size_t>>();
    pImpl->activationBuffer_.clear();
    pImpl->activationQueue_.clear();
    
    NLM_LOG_INFO("NeuralAssociativeMemory cleared all associative patterns");
}

size_t NeuralAssociativeMemory::getAssociationCount() const {
    return pImpl->associations_.size();
}

// Private implementation methods

NeuronId NeuralAssociativeMemory::findPatternNeuron(const std::vector<float>& pattern) {
    if (pattern.empty() || !brain_) return INVALID_NEURON_ID;
    
    // Check if pattern already exists
    for (const auto& patternPair : pImpl->patternNeurons_) {
        if (computeSimilarity(pattern, patternPair.second) > 0.9f) {
            return patternPair.first;
        }
    }
    
    // Create new pattern neuron
    if (pImpl->patternNeurons_.size() >= pImpl->maxPatterns_) {
        // Remove weakest pattern for capacity management
        removeWeakestPattern();
    }
    
    // Create new neuron in the brain
    RegionId patternRegion = INVALID_REGION_ID;
    if (pImpl->brain_) {
        patternRegion = pImpl->brain_->addRegion("associative_pattern");
    }
    
    // Create unique neuron ID
    NeuronId newNeuron(pImpl->patternNeurons_.size() + 1000000);
    
    // Store pattern
    pImpl->patternNeurons_.emplace_back(newNeuron, pattern);
    
    return newNeuron;
}

float NeuralAssociativeMemory::computeSimilarity(const std::vector<float>& a,
                                               const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        // Apply context weighting based on position in pattern
        float weight = 1.0f;
        if (i < 3) {  // Positional encoding gets higher weight
            weight = 2.0f;
        }
        
        dot += a[i] * b[i] * weight;
        normA += a[i] * a[i] * weight;
        normB += b[i] * b[i] * weight;
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / std::sqrt(normA * normB);
}

size_t NeuralAssociativeMemory::findMostSimilarPattern(const std::vector<float>& queryPattern) const {
    if (queryPattern.empty()) return static_cast<size_t>(-1);
    
    float bestSimilarity = 0.0f;
    size_t bestIdx = static_cast<size_t>(-1);
    
    for (size_t i = 0; i < pImpl->patternNeurons_.size(); ++i) {
        float similarity = computeSimilarity(queryPattern, pImpl->patternNeurons_[i].second);
        
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestIdx = i;
        }
    }
    
    return bestSimilarity > 0.5f ? bestIdx : static_cast<size_t>(-1);
}

void NeuralAssociativeMemory::createAssociation(NeuronId neuronA, NeuronId neuronB, float strength) {
    if (neuronA == neuronB) return; // No self-associations
    
    // Check if association already exists
    for (const auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            // Update existing association
            std::get<2>(assoc) = strength;
            return;
        }
    }
    
    // Check capacity
    if (pImpl->associations_.size() >= pImpl->maxAssociations_) {
        removeWeakestAssociation();
    }
    
    // Create new association (store in bidirectional order for consistency)
    if (neuronA.value < neuronB.value) {
        pImpl->associations_.emplace_back(neuronA, neuronB, strength);
    } else {
        pImpl->associations_.emplace_back(neuronB, neuronA, strength);
    }
    
    pImpl->totalAssociationsCreated_++;
    pImpl->averageAssociationStrength_ = (pImpl->averageAssociationStrength_ * 
                                           (pImpl->totalAssociationsCreated_ - 1) + 
                                           strength) / pImpl->totalAssociationsCreated_;
}

void NeuralAssociativeMemory::applyNeuromodulatoryAssociation(NeuronId neuronA, NeuronId neuronB, float strength) {
    // Dopamine modulates reward-based association strengthening
    if (pImpl->dopamine_) {
        float plasticityFactor = pImpl->dopamine_->getPlasticityFactor();
        float modulatedStrength = strength * (1.0f + plasticityFactor * 0.5f);
        
        // In a real implementation, this would apply neuromodulatory effects
        // to the synaptic connections between the pattern neurons
    }
    
    // Acetylcholine modulates attention and memory consolidation
    if (pImpl->acetylcholine_) {
        float attentionGain = pImpl->acetylcholine_->getAttentionGain();
        // Apply attention-based modulation
    }
}

void NeuralAssociativeMemory::integrateWithWorkingMemory(const std::vector<float>& patternA,
                                                         const std::vector<float>& patternB) {
    if (!pImpl->workingMemory_) return;
    
    // Store patterns in working memory for temporal context
    if (!patternA.empty()) {
        pImpl->workingMemory_->store(patternA, 0.8f);
    }
    
    if (!patternB.empty()) {
        pImpl->workingMemory_->store(patternB, 0.7f);
    }
}

void NeuralAssociativeMemory::applyPlasticityToAssociations(NeuronId neuronA, NeuronId neuronB, float strength) {
    if (!pImpl->brain_) return;
    
    // Apply STDP for spike-timing dependent plasticity
    STDP* stdp = pImpl->brain_->getSTDP();
    if (stdp) {
        // In a real implementation, this would find the actual synapses
        // between the pattern neurons and apply STDP rules
    }
    
    // Apply Hebbian plasticity for co-activation
    Hebbian* hebbian = pImpl->brain_->getHebbian();
    if (hebbian) {
        // Apply Hebbian learning based on association strength
        float learningRate = hebbian->getLearningRate();
        float maxWeight = hebbian->getMaxWeight();
        
        float delta = (strength - 0.5f) * learningRate * 0.1f;
        delta = std::min(delta, maxWeight - 0.5f);
        
        // Apply to associated synapses
        applyHebbianUpdate(neuronA, neuronB, delta);
    }
}

void NeuralAssociativeMemory::applyHebbianUpdate(NeuronId neuronA, NeuronId neuronB, float delta) {
    if (!pImpl->brain_) return;
    
    // Find or create synapses between the neurons
    // In a real implementation, this would use proper synapse management
    
    // For now, just update the association strength
    for (auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::min(1.0f, std::max(0.0f, std::get<2>(assoc) + delta));
            break;
        }
    }
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociatedNeurons(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    for (const auto& assoc : pImpl->associations_) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    
    return result;
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId neuronA, NeuronId neuronB) const {
    for (const auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void NeuralAssociativeMemory::updateAssociation(NeuronId neuronA, NeuronId neuronB, float delta) {
    for (auto& assoc : pImpl->associations_) {
        if ((std::get<0>(assoc) == neuronA && std::get<1>(assoc) == neuronB) ||
            (std::get<0>(assoc) == neuronB && std::get<1>(assoc) == neuronA)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            break;
        }
    }
}

void NeuralAssociativeMemory::removeWeakestPattern() {
    if (pImpl->patternNeurons_.empty()) return;
    
    // Find pattern with lowest average activation (placeholder)
    size_t weakestIdx = 0;
    float weakestScore = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < pImpl->patternNeurons_.size(); ++i) {
        // In a real implementation, this would compute pattern strength
        float score = 0.0f; // Placeholder
        if (score < weakestScore) {
            weakestScore = score;
            weakestIdx = i;
        }
    }
    
    pImpl->patternNeurons_.erase(pImpl->patternNeurons_.begin() + weakestIdx);
}

void NeuralAssociativeMemory::removeWeakestAssociation() {
    if (pImpl->associations_.empty()) return;
    
    // Find association with weakest strength
    size_t weakestIdx = 0;
    float weakestStrength = std::numeric_limits<float>::max();
    
    for (size_t i = 0; i < pImpl->associations_.size(); ++i) {
        float strength = std::get<2>(pImpl->associations_[i]);
        if (strength < weakestStrength) {
            weakestStrength = strength;
            weakestIdx = i;
        }
    }
    
    pImpl->associations_.erase(pImpl->associations_.begin() + weakestIdx);
}

} // namespace nlm
