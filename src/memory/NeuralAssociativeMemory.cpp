#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    
    // Track learning activity for Hebbian plasticity
    std::vector<float> preSpikeHistory;
    std::vector<float> postSpikeHistory;
    
    // Timing information for STDP
    SimulationStep lastPreSpikeTime;
    SimulationStep lastPostSpikeTime;
    
    Impl() : brain(nullptr), lastPreSpikeTime(0), lastPostSpikeTime(0) {
        preSpikeHistory.reserve(100);
        postSpikeHistory.reserve(100);
    }
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , learningEnabled_(true)
    , spreadingEnabled_(true)
    , hebbianLearningRate_(0.01f)
    , stdpLearningRate_(0.012f)
    , activationDecayRate_(0.01f)
    , maxSpreadSteps_(3)
{
    NLM_LOG_INFO("NeuralAssociativeMemory created");
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralAssociativeMemory initialized with brain reference");
}

void NeuralAssociativeMemory::learnPattern(const std::vector<float>& pattern,
                                          SimulationStep currentTime,
                                          float reward) {
    if (!learningEnabled_ || pattern.empty() || !brain_) {
        return;
    }
    
    // Convert pattern to neural representation
    NeuronId patternNeuron = getOrCreatePatternNeuron(pattern, currentTime);
    
    if (patternNeuron == INVALID_NEURON_ID) {
        return;
    }
    
    // Update pattern activation based on reward
    float activationUpdate = reward * hebbianLearningRate_;
    
    auto it = patternSets_.find(patternNeuron);
    if (it != patternSets_.end()) {
        it->second.currentActivation = std::min(1.0f, 
                                               it->second.currentActivation + activationUpdate);
    }
    
    // Hebbian plasticity: reinforce association
    for (const auto& otherPattern : patternSets_) {
        if (otherPattern.first == patternNeuron) {
            continue;
        }
        
        // Create or update association with Hebbian learning
        createNeuralAssociation(patternNeuron, otherPattern.first,
                               hebbianLearningRate_, currentTime);
    }
    
    NLM_LOG_INFO("Pattern learned - neuron: " + std::to_string(patternNeuron.getId()) +
                " activation: " + std::to_string(patternSets_[patternNeuron].currentActivation));
}

void NeuralAssociativeMemory::learnFromEpisode(const EpisodicMemoryItem& episode,
                                               SimulationStep currentTime) {
    if (!brain_ || episode.sensoryState.empty() || episode.resultingSensoryState.empty()) {
        return;
    }
    
    // Create neural patterns for episode states
    NeuronId sourceNeuron = getOrCreatePatternNeuron(episode.sensoryState, currentTime);
    NeuronId targetNeuron = getOrCreatePatternNeuron(episode.resultingSensoryState, currentTime);
    
    if (sourceNeuron == INVALID_NEURON_ID || targetNeuron == INVALID_NEURON_ID) {
        return;
    }
    
    // Calculate association strength based on reward and similarity
    float baseStrength = 0.8f;  // High for strongly correlated experiences
    if (episode.reward > 0.0f) {
        baseStrength *= episode.reward;
    }
    
    // Create Hebbian association
    createNeuralAssociation(sourceNeuron, targetNeuron, baseStrength, currentTime);
    
    // Also create self-association for pattern stability
    createNeuralAssociation(sourceNeuron, sourceNeuron, 0.5f, currentTime);
    createNeuralAssociation(targetNeuron, targetNeuron, 0.5f, currentTime);
    
    NLM_LOG_INFO("Association learned from episode - source: " + std::to_string(sourceNeuron.getId()) +
                " target: " + std::to_string(targetNeuron.getId()) +
                " strength: " + std::to_string(baseStrength));
}

std::vector<float> NeuralAssociativeMemory::completePattern(
    const std::vector<float>& partialPattern,
    float similarityThreshold) const {
    
    std::vector<float> completedPattern;
    
    if (partialPattern.empty() || patternSets_.empty()) {
        return completedPattern;
    }
    
    // Find the best matching pattern neuron
    NeuronId bestMatchNeuron = INVALID_NEURON_ID;
    float bestScore = 0.0f;
    
    for (const auto& patternPair : patternSets_) {
        NeuronId patternNeuron = patternPair.first;
        float completionScore = computePatternCompletionScore(patternNeuron, partialPattern);
        
        if (completionScore > bestScore) {
            bestScore = completionScore;
            bestMatchNeuron = patternNeuron;
        }
    }
    
    if (bestMatchNeuron == INVALID_NEURON_ID || bestScore < similarityThreshold) {
        return completedPattern;
    }
    
    // If cache exists, return cached completion
    auto cacheIt = completionCache_.find(bestMatchNeuron);
    if (cacheIt != completionCache_.end()) {
        return cacheIt->second;
    }
    
    // Create completed pattern through spreading activation
    completedPattern = patternSets_.at(bestMatchNeuron).patternPrototype;
    
    // Amplify activation for the matched pattern
    auto& matchedPattern = patternSets_.at(bestMatchNeuron);
    float activationBoost = matchedPattern.currentActivation * 2.0f;
    
    for (float& value : completedPattern) {
        value *= activationBoost;
    }
    
    // Cache the completed pattern
    completionCache_[bestMatchNeuron] = completedPattern;
    
    NLM_LOG_INFO("Pattern completed - match score: " + std::to_string(bestScore) +
                " activation boost: " + std::to_string(activationBoost));
    
    return completedPattern;
}

std::vector<NeuronId> NeuralAssociativeMemory::getAssociations(NeuronId patternNeuron) const {
    std::vector<NeuronId> associatedNeurons;
    
    for (const auto& assoc : associations_) {
        if (assoc.sourceNeurons.empty() && assoc.targetNeurons.empty()) {
            continue;
        }
        
        // Check if patternNeuron is in source or target
        bool isSource = std::find(assoc.sourceNeurons.begin(),
                                 assoc.sourceNeurons.end(), patternNeuron) != assoc.sourceNeurons.end();
        bool isTarget = std::find(assoc.targetNeurons.begin(),
                                 assoc.targetNeurons.end(), patternNeuron) != assoc.targetNeurons.end();
        
        if (isSource) {
            associatedNeurons.insert(associatedNeurons.end(),
                                   assoc.targetNeurons.begin(),
                                   assoc.targetNeurons.end());
        }
        if (isTarget) {
            associatedNeurons.insert(associatedNeurons.end(),
                                   assoc.sourceNeurons.begin(),
                                   assoc.sourceNeurons.end());
        }
    }
    
    // Remove duplicates
    std::sort(associatedNeurons.begin(), associatedNeurons.end());
    associatedNeurons.erase(std::unique(associatedNeurons.begin(), associatedNeurons.end()),
                           associatedNeurons.end());
    
    return associatedNeurons;
}

float NeuralAssociativeMemory::getPatternActivation(NeuronId patternNeuron) const {
    auto it = patternSets_.find(patternNeuron);
    if (it != patternSets_.end()) {
        return it->second.currentActivation;
    }
    return 0.0f;
}

float NeuralAssociativeMemory::getAssociationStrength(NeuronId sourceNeuron,
                                                    NeuronId targetNeuron) const {
    float maxStrength = 0.0f;
    
    for (const auto& assoc : associations_) {
        bool sourceMatch = std::find(assoc.sourceNeurons.begin(),
                                   assoc.sourceNeurons.end(), sourceNeuron) != assoc.sourceNeurons.end();
        bool targetMatch = std::find(assoc.targetNeurons.begin(),
                                   assoc.targetNeurons.end(), targetNeuron) != assoc.targetNeurons.end();
        
        if (sourceMatch && targetMatch) {
            maxStrength = std::max(maxStrength, assoc.strength);
        }
    }
    
    return maxStrength;
}

void NeuralAssociativeMemory::activatePatternSet(NeuronId patternNeuron,
                                               float activationStrength) {
    auto it = patternSets_.find(patternNeuron);
    if (it == patternSets_.end()) {
        return;
    }
    
    // Boost activation
    it->second.currentActivation = std::min(1.0f, 
                                           it->second.currentActivation + activationStrength);
    
    // Reactivate associated pattern neurons
    std::vector<NeuronId> associated = getAssociations(patternNeuron);
    
    for (NeuronId associatedNeuron : associated) {
        auto& assocPattern = patternSets_[associatedNeuron];
        assocPattern.currentActivation = std::min(1.0f, 
                                                 assocPattern.currentActivation + activationStrength * 0.5f);
    }
    
    // Update completion cache
    clearCompletionCache();
    
    NLM_LOG_INFO("Pattern set activated - neuron: " + std::to_string(patternNeuron.getId()) +
                " new activation: " + std::to_string(it->second.currentActivation));
}

std::vector<NeuronId> NeuralAssociativeMemory::getActivePatterns() const {
    std::vector<NeuronId> activePatterns;
    
    for (const auto& patternPair : patternSets_) {
        if (patternPair.second.currentActivation > 0.1f) {
            activePatterns.push_back(patternPair.first);
        }
    }
    
    return activePatterns;
}

void NeuralAssociativeMemory::update(TimestepDuration dt) {
    if (!brain_) {
        return;
    }
    
    // Decay pattern activations
    decayPatternActivations(dt);
    
    // Remove weak patterns
    removeWeakPatterns(0.05f);
    
    // Update completion cache
    updateCompletionCache();
    
    // Apply Hebbian plasticity to strengthen active associations
    for (auto& assoc : associations_) {
        if (assoc.eligibilityTrace > 0.01f) {
            // Apply Hebbian learning
            updateHebbianPlasticity(assoc, pImpl->brain->getCurrentStep(), 1.0f);
        }
    }
    
    // Log pattern stats occasionally
    static SimulationStep lastLogTime = 0;
    SimulationStep currentStep = pImpl->brain->getCurrentStep();
    if (currentStep - lastLogTime > 10000) {
        logPatternStats();
        lastLogTime = currentStep;
    }
}

void NeuralAssociativeMemory::clear() {
    patternSets_.clear();
    associations_.clear();
    clearCompletionCache();
    
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

const PatternSet* NeuralAssociativeMemory::getPatternSet(NeuronId patternNeuron) const {
    auto it = patternSets_.find(patternNeuron);
    if (it != patternSets_.end()) {
        return &it->second;
    }
    return nullptr;
}

NeuronId NeuralAssociativeMemory::getOrCreatePatternNeuron(
    const std::vector<float>& pattern,
    SimulationStep currentTime) {
    
    // Check if we already have a similar pattern
    for (auto& patternPair : patternSets_) {
        const PatternSet& existingPattern = patternPair.second;
        if (computePatternSimilarity(pattern, existingPattern.patternPrototype) > 0.8f) {
            return patternPair.first;
        }
    }
    
    // Create new pattern neuron
    NeuronId newNeuronId(patternSets_.size() + 40000);  // Offset to avoid conflicts
    
    PatternSet newPattern;
    newPattern.patternNeuron = newNeuronId;
    newPattern.patternPrototype = pattern;
    newPattern.neurons.push_back(newNeuronId);
    newPattern.currentActivation = 0.0f;
    newPattern.decayRate = activationDecayRate_;
    
    // Store activation mapping
    for (size_t i = 0; i < pattern.size(); ++i) {
        newPattern.activationMap[newNeuronId] = pattern[i];
    }
    
    patternSets_[newNeuronId] = newPattern;
    
    NLM_LOG_INFO("New pattern neuron created - id: " + std::to_string(newNeuronId.getId()) +
                " similarity threshold: 0.8f");
    
    return newNeuronId;
}

float NeuralAssociativeMemory::computePatternSimilarity(const std::vector<float>& pattern1,
                                                    const std::vector<float>& pattern2) const {
    if (pattern1.size() != pattern2.size() || pattern1.empty()) {
        return 0.0f;
    }
    
    float dotProduct = 0.0f;
    float norm1 = 0.0f;
    float norm2 = 0.0f;
    
    for (size_t i = 0; i < pattern1.size(); ++i) {
        dotProduct += pattern1[i] * pattern2[i];
        norm1 += pattern1[i] * pattern1[i];
        norm2 += pattern2[i] * pattern2[i];
    }
    
    if (norm1 < 0.0001f || norm2 < 0.0001f) {
        return 0.0f;
    }
    
    return dotProduct / (std::sqrt(norm1) * std::sqrt(norm2));
}

void NeuralAssociativeMemory::createNeuralAssociation(NeuronId sourceNeuron,
                                                   NeuronId targetNeuron,
                                                   float strength,
                                                   SimulationStep currentTime) {
    PatternAssociation newAssoc;
    
    // Create source pattern set if it doesn't exist
    if (patternSets_.find(sourceNeuron) == patternSets_.end()) {
        PatternSet sourcePattern;
        sourcePattern.patternNeuron = sourceNeuron;
        sourcePattern.currentActivation = 0.5f;
        patternSets_[sourceNeuron] = sourcePattern;
    }
    
    // Create target pattern set if it doesn't exist
    if (patternSets_.find(targetNeuron) == patternSets_.end()) {
        PatternSet targetPattern;
        targetPattern.patternNeuron = targetNeuron;
        targetPattern.currentActivation = 0.5f;
        patternSets_[targetNeuron] = targetPattern;
    }
    
    // Add neurons to association
    newAssoc.sourceNeurons = {sourceNeuron};
    newAssoc.targetNeurons = {targetNeuron};
    newAssoc.strength = strength;
    newAssoc.eligibilityTrace = 0.0f;
    newAssoc.lastUpdated = currentTime;
    newAssoc.sourcePatternScore = 0.5f;
    newAssoc.targetPatternScore = 0.5f;
    
    // Add to associations list
    associations_.push_back(newAssoc);
    
    NLM_LOG_INFO("Neural association created - source: " + std::to_string(sourceNeuron.getId()) +
                " target: " + std::to_string(targetNeuron.getId()) +
                " strength: " + std::to_string(strength));
}

void NeuralAssociativeMemory::updateHebbianPlasticity(PatternAssociation& assoc,
                                                  SimulationStep currentTime,
                                                  float reward) {
    // Hebbian learning: strengthen association based on co-activation
    float weightChange = hebbianLearningRate_ * reward * assoc.eligibilityTrace;
    
    assoc.strength = std::clamp(assoc.strength + weightChange, 0.0f, 1.0f);
    assoc.eligibilityTrace = 0.0f;  // Reset after update
    assoc.lastUpdated = currentTime;
    
    NLM_LOG_INFO("Hebbian plasticity updated - new strength: " + std::to_string(assoc.strength));
}

void NeuralAssociativeMemory::updateSTDPPlasticity(PatternAssociation& assoc,
                                              SimulationStep currentTime,
                                              const std::vector<float>& preSpikeHistory,
                                              const std::vector<float>& postSpikeHistory) {
    // STDP: spike timing dependent plasticity
    if (!preSpikeHistory.empty() && !postSpikeHistory.empty()) {
        float timeDifference = static_cast<float>(std::abs(
            static_cast<int64_t>(currentTime) - static_cast<int64_t>(assoc.lastUpdated)));
        
        float stdpChange = 0.0f;
        
        if (timeDifference < 20) {  // Close in time - LTP
            stdpChange = stdpLearningRate_ * (1.0f - timeDifference / 20.0f);
        } else {  // Far apart - LTD
            stdpChange = -stdpLearningRate_ * (timeDifference - 20.0f) / 100.0f;
        }
        
        assoc.strength = std::clamp(assoc.strength + stdpChange, 0.0f, 1.0f);
        assoc.lastUpdated = currentTime;
        
        NLM_LOG_INFO("STDP plasticity updated - time diff: " + std::to_string(timeDifference) +
                    " change: " + std::to_string(stdpChange) +
                    " new strength: " + std::to_string(assoc.strength));
    }
}

void NeuralAssociativeMemory::propagateActivation(
    NeuronId startNeuron,
    std::vector<float>& accumulatedActivation,
    size_t maxSteps) const {
    
    if (maxSteps == 0 || accumulatedActivation.empty()) {
        return;
    }
    
    // Find associated neurons
    std::vector<NeuronId> associated = getAssociations(startNeuron);
    
    if (associated.empty()) {
        return;
    }
    
    // Apply spreading activation
    for (size_t step = 0; step < std::min(maxSteps, size_t(3)); ++step) {
        std::vector<float> stepActivations;
        float totalInput = 0.0f;
        
        for (NeuronId assocNeuron : associated) {
            float assocStrength = getAssociationStrength(startNeuron, assocNeuron);
            
            if (assocStrength > 0.1f) {
                totalInput += assocStrength;
                stepActivations.push_back(assocStrength);
            }
        }
        
        if (totalInput < 0.01f) {
            break;  // No significant activation
        }
        
        // Normalize and apply to accumulated activations
        float normalization = 1.0f / totalInput;
        for (size_t i = 0; i < stepActivations.size(); ++i) {
            stepActivations[i] *= normalization;
        }
        
        // Apply to pattern
        for (size_t i = 0; i < std::min(stepActivations.size(), accumulatedActivation.size()); ++i) {
            accumulatedActivation[i] += stepActivations[i] * 0.5f;
        }
        
        // Update for next step
        std::vector<NeuronId> nextLevel;
        for (NeuronId assocNeuron : associated) {
            if (getAssociationStrength(startNeuron, assocNeuron) > 0.1f) {
                nextLevel.push_back(assocNeuron);
            }
        }
        
        associated = nextLevel;
    }
}

float NeuralAssociativeMemory::computePatternCompletionScore(
    NeuronId patternNeuron,
    const std::vector<float>& queryPattern) const {
    
    auto patternIt = patternSets_.find(patternNeuron);
    if (patternIt == patternSets_.end()) {
        return 0.0f;
    }
    
    const PatternSet& pattern = patternIt->second;
    
    // Compute similarity with query pattern
    float similarity = computePatternSimilarity(pattern.patternPrototype, queryPattern);
    
    // Boost score based on current activation
    float activationBoost = pattern.currentActivation * 0.5f;
    
    return similarity + activationBoost;
}

void NeuralAssociativeMemory::decayPatternActivations(TimestepDuration dt) {
    float decayFactor = std::pow(1.0f - activationDecayRate_, dt);
    
    for (auto& patternPair : patternSets_) {
        patternPair.second.currentActivation *= decayFactor;
    }
}

void NeuralAssociativeMemory::removeWeakPatterns(float activationThreshold) {
    std::vector<NeuronId> toRemove;
    
    for (const auto& patternPair : patternSets_) {
        if (patternPair.second.currentActivation < activationThreshold) {
            toRemove.push_back(patternPair.first);
        }
    }
    
    // Remove weak patterns
    for (NeuronId neuronId : toRemove) {
        patternSets_.erase(neuronId);
        NLM_LOG_INFO("Removed weak pattern - neuron: " + std::to_string(neuronId.getId()));
    }
}

void NeuralAssociativeMemory::updateCompletionCache() const {
    completionCache_.clear();
    
    for (const auto& patternPair : patternSets_) {
        NeuronId patternNeuron = patternPair.first;
        const PatternSet& pattern = patternPair.second;
        
        if (pattern.currentActivation > 0.1f) {
            completionCache_[patternNeuron] = pattern.patternPrototype;
        }
    }
}

void NeuralAssociativeMemory::clearCompletionCache() {
    completionCache_.clear();
}

void NeuralAssociativeMemory::logPatternStats() const {
    NLM_LOG_INFO("=== NeuralAssociativeMemory Statistics ===");
    NLM_LOG_INFO("Pattern sets: " + std::to_string(patternSets_.size()));
    NLM_LOG_INFO("Associations: " + std::to_string(associations_.size()));
    
    float totalActivation = 0.0f;
    size_t activePatterns = 0;
    
    for (const auto& patternPair : patternSets_) {
        totalActivation += patternPair.second.currentActivation;
        if (patternPair.second.currentActivation > 0.1f) {
            activePatterns++;
        }
    }
    
    NLM_LOG_INFO("Total pattern activation: " + std::to_string(totalActivation));
    NLM_LOG_INFO("Active patterns: " + std::to_string(activePatterns));
    
    if (!patternSets_.empty()) {
        float avgActivation = totalActivation / patternSets_.size();
        NLM_LOG_INFO("Average pattern activation: " + std::to_string(avgActivation));
    }
}

float NeuralAssociativeMemory::getAveragePatternActivation() const {
    if (patternSets_.empty()) {
        return 0.0f;
    }
    
    float totalActivation = 0.0f;
    for (const auto& patternPair : patternSets_) {
        totalActivation += patternPair.second.currentActivation;
    }
    
    return totalActivation / patternSets_.size();
}

} // namespace nlm