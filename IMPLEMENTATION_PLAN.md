# NLM - Missing Systems Implementation Plan

## Overview

This document provides a detailed implementation plan for the missing systems in the NLM (Neural Learning Machine) project:
1. Semantic Memory (NeuralSemanticMemory)
2. Procedural Memory (NeuralProceduralMemory) 
3. Missing Neuromodulators (Serotonin, Norepinephrine, Acetylcholine)

## Priority 1: Semantic Memory Implementation

### Purpose
Semantic memory stores facts, concepts, and knowledge about the world. Unlike episodic memory (which stores specific experiences) or procedural memory (which stores action sequences), semantic memory represents abstract knowledge and relationships.

### Architecture

#### NeuralSemanticMemory.hpp
```cpp
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <string>
#include <unordered_map>
#include <memory>

namespace nlm {

class NeuralSemanticMemory {
public:
    NeuralSemanticMemory();
    ~NeuralSemanticMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Store a concept with its neural pattern
    // Returns concept ID (0 = existing, >0 = new)
    size_t storeConcept(const std::vector<float>& pattern,
                       const std::string& category,
                       float initialRelevance = 1.0f);
    
    // Retrieve related concepts for a given pattern
    std::vector<size_t> findRelatedConcepts(const std::vector<float>& pattern,
                                           float similarityThreshold = 0.7f) const;
    
    // Get category information for a concept
    const std::string& getConceptCategory(size_t conceptId) const;
    
    // Get relevance/strength of a concept
    float getConceptRelevance(size_t conceptId) const;
    
    // Update concept relevance based on experience
    void updateConceptRelevance(size_t conceptId, float delta);
    
    // Learn semantic relationships between concepts
    void learnRelationship(size_t conceptIdA, size_t conceptIdB,
                         const std::string& relationshipType,
                         float strength = 1.0f);
    
    // Get concepts in a specific category
    std::vector<size_t> getConceptsByCategory(const std::string& category) const;
    
    // Update semantic memory (decay, consolidation)
    void update(TimestepDuration dt);
    
    // Clear all semantic knowledge
    void clear();
    
    // Checkpoint support
    struct SemanticMemorySnapshot {
        std::vector<std::vector<float>> patterns;  // Neural patterns
        std::vector<std::string> categories;      // Concept categories
        std::vector<float> relevances;            // Concept strengths
        std::vector<std::unordered_map<size_t, std::string>> relationships; // Concept relationships
    };
    
    SemanticMemorySnapshot takeSnapshot() const;
    void restoreSnapshot(const SemanticMemorySnapshot& snapshot);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
};

} // namespace nlm
```

#### NeuralSemanticMemory.cpp - Implementation

```cpp
#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // Concept storage
    std::vector<std::vector<float>> conceptPatterns;
    std::vector<std::string> conceptCategories;
    std::vector<float> conceptRelevances;
    std::vector<std::unordered_map<size_t, std::string>> conceptRelationships;
    
    // For fast lookup
    std::unordered_map<size_t, size_t> patternToConceptId;
    std::unordered_map<std::string, std::vector<size_t>> categoryToConceptIds;
    
    // Learning parameters
    float conceptDecayRate = 0.001f;
    float conceptRelevanceDecay = 0.995f;
    float relationshipDecay = 0.99f;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

size_t NeuralSemanticMemory::storeConcept(const std::vector<float>& pattern,
                                         const std::string& category,
                                         float initialRelevance) {
    if (pattern.empty() || !brain_) return 0;
    
    // Check if concept already exists (similar pattern)
    float bestSimilarity = 0.0f;
    size_t existingConceptId = 0;
    
    for (size_t i = 0; i < pImpl->conceptPatterns.size(); ++i) {
        float sim = computeCosineSimilarity(pattern, pImpl->conceptPatterns[i]);
        if (sim > bestSimilarity && sim > 0.8f) {
            bestSimilarity = sim;
            existingConceptId = i + 1; // +1 to avoid 0 meaning "new"
        }
    }
    
    if (existingConceptId > 0) {
        // Update existing concept relevance
        pImpl->conceptRelevances[existingConceptId - 1] = std::max(
            pImpl->conceptRelevances[existingConceptId - 1],
            initialRelevance);
        return existingConceptId;
    }
    
    // Create new concept
    size_t conceptId = pImpl->conceptPatterns.size() + 1;
    
    pImpl->conceptPatterns.push_back(pattern);
    pImpl->conceptCategories.push_back(category);
    pImpl->conceptRelevances.push_back(initialRelevance);
    pImpl->conceptRelationships.push_back({}); // Empty relationships map
    
    // Update lookup tables
    pImpl->patternToConceptId[std::hash<std::vector<float>>{}(pattern)] = conceptId - 1;
    pImpl->categoryToConceptIds[category].push_back(conceptId - 1);
    
    // Inject current into related neural populations
    injectConceptPattern(conceptId, initialRelevance);
    
    NLM_LOG_INFO("SemanticMemory: Stored new concept " + std::to_string(conceptId) +
                 " (category: " + category + ", relevance: " + std::to_string(initialRelevance) + ")");
    
    return conceptId;
}

float NeuralSemanticMemory::computeCosineSimilarity(const std::vector<float>& a,
                                                   const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

void NeuralSemanticMemory::injectConceptPattern(size_t conceptId, float strength) {
    if (!brain_ || conceptId == 0 || conceptId > pImpl->conceptPatterns.size()) return;
    
    // Get the concept pattern
    const auto& pattern = pImpl->conceptPatterns[conceptId - 1];
    
    // Find sensory neurons that might represent this concept
    // This is a simplified approach - concepts are stored in internal neurons
    auto* region = brain_->getRegion(RegionId(1)); // Default to first region
    if (!region) return;
    
    auto* internalPop = region->getPopulation(1); // Internal population
    if (!internalPop) return;
    
    // Activate a subset of internal neurons to represent this concept
    size_t neuronsToActivate = std::min(pattern.size(), internalPop->getNeuronCount());
    
    for (size_t i = 0; i < neuronsToActivate; ++i) {
        float activation = pattern[i] * strength * 2.0f; // Scale factor
        brain_->injectCurrent(internalPop->getNeuron(i)->getId(), activation);
    }
}

std::vector<size_t> NeuralSemanticMemory::findRelatedConcepts(
    const std::vector<float>& pattern,
    float similarityThreshold) const {
    
    std::vector<std::pair<float, size_t>> scores; // (similarity, conceptId)
    
    for (size_t i = 0; i < pImpl->conceptPatterns.size(); ++i) {
        float sim = computeCosineSimilarity(pattern, pImpl->conceptPatterns[i]);
        if (sim > similarityThreshold) {
            scores.emplace_back(sim, i + 1); // +1 to return conceptId
        }
    }
    
    // Sort by similarity (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    std::vector<size_t> result;
    for (const auto& score : scores) {
        result.push_back(score.second);
    }
    
    return result;
}

const std::string& NeuralSemanticMemory::getConceptCategory(size_t conceptId) const {
    if (conceptId == 0 || conceptId > pImpl->conceptCategories.size()) {
        static const std::string empty = "";
        return empty;
    }
    return pImpl->conceptCategories[conceptId - 1];
}

float NeuralSemanticMemory::getConceptRelevance(size_t conceptId) const {
    if (conceptId == 0 || conceptId > pImpl->conceptRelevances.size()) {
        return 0.0f;
    }
    return pImpl->conceptRelevances[conceptId - 1];
}

void NeuralSemanticMemory::updateConceptRelevance(size_t conceptId, float delta) {
    if (conceptId == 0 || conceptId > pImpl->conceptRelevances.size()) return;
    
    pImpl->conceptRelevances[conceptId - 1] = std::clamp(
        pImpl->conceptRelevances[conceptId - 1] + delta, 0.0f, 1.0f);
}

void NeuralSemanticMemory::learnRelationship(size_t conceptIdA, size_t conceptIdB,
                                            const std::string& relationshipType,
                                            float strength) {
    if (conceptIdA == 0 || conceptIdB == 0 || conceptIdA == conceptIdB) return;
    if (conceptIdA > pImpl->conceptRelationships.size() || 
        conceptIdB > pImpl->conceptRelationships.size()) return;
    
    // Store relationship in both directions
    pImpl->conceptRelationships[conceptIdA - 1][conceptIdB] = relationshipType;
    pImpl->conceptRelationships[conceptIdB - 1][conceptIdA] = relationshipType + "_reverse";
}

std::vector<size_t> NeuralSemanticMemory::getConceptsByCategory(
    const std::string& category) const {
    
    auto it = pImpl->categoryToConceptIds.find(category);
    if (it == pImpl->categoryToConceptIds.end()) {
        return {};
    }
    
    return it->second;
}

void NeuralSemanticMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Decay concept relevances over time
    for (auto& relevance : pImpl->conceptRelevances) {
        relevance *= conceptRelevanceDecay;
    }
    
    // Decay relationship strength
    for (auto& relationships : pImpl->conceptRelationships) {
        for (auto& pair : relationships) {
            pair.second += relationshipDecay - 1.0f; // Decay toward zero
        }
    }
    
    // Consolidate low-relevance concepts (remove them)
    consolidateLowRelevanceConcepts();
}

void NeuralSemanticMemory::consolidateLowRelevanceConcepts() {
    std::vector<size_t> conceptsToRemove;
    
    for (size_t i = 0; i < pImpl->conceptRelevances.size(); ++i) {
        if (pImpl->conceptRelevances[i] < 0.01f) {
            conceptsToRemove.push_back(i);
        }
    }
    
    // Remove low-relevance concepts in reverse order to maintain indices
    for (auto it = conceptsToRemove.rbegin(); it != conceptsToRemove.rend(); ++it) {
        size_t index = *it;
        
        // Update lookup tables
        auto conceptId = index + 1;
        for (auto& pair : pImpl->patternToConceptId) {
            if (pair.second == index) {
                pImpl->patternToConceptId.erase(pair.first);
            }
        }
        
        for (auto& pair : pImpl->categoryToConceptIds) {
            auto& concepts = pair.second;
            concepts.erase(std::remove(concepts.begin(), concepts.end(), index),
                          concepts.end());
        }
        
        // Remove from all containers
        pImpl->conceptPatterns.erase(pImpl->conceptPatterns.begin() + index);
        pImpl->conceptCategories.erase(pImpl->conceptCategories.begin() + index);
        pImpl->conceptRelevances.erase(pImpl->conceptRelevances.begin() + index);
        pImpl->conceptRelationships.erase(pImpl->conceptRelationships.begin() + index);
    }
}

NeuralSemanticMemory::SemanticMemorySnapshot NeuralSemanticMemory::takeSnapshot() const {
    SemanticMemorySnapshot snapshot;
    
    snapshot.patterns = pImpl->conceptPatterns;
    snapshot.categories = pImpl->conceptCategories;
    snapshot.relevances = pImpl->conceptRelevances;
    snapshot.relationships = pImpl->conceptRelationships;
    
    return snapshot;
}

void NeuralSemanticMemory::restoreSnapshot(const SemanticMemorySnapshot& snapshot) {
    pImpl->conceptPatterns = snapshot.patterns;
    pImpl->conceptCategories = snapshot.categories;
    pImpl->conceptRelevances = snapshot.relevances;
    pImpl->conceptRelationships = snapshot.relationships;
    
    // Rebuild lookup tables
    pImpl->patternToConceptId.clear();
    pImpl->categoryToConceptIds.clear();
    
    for (size_t i = 0; i < pImpl->conceptPatterns.size(); ++i) {
        // Hash pattern for lookup (simplified)
        std::hash<std::vector<float>> hasher;
        pImpl->patternToConceptId[hasher(pImpl->conceptPatterns[i])] = i;
        
        pImpl->categoryToConceptIds[pImpl->conceptCategories[i]].push_back(i);
    }
}

} // namespace nlm
```

## Priority 2: Procedural Memory Implementation

### Purpose
Procedural memory stores skills, habits, and action sequences. It's the "how to do it" memory that becomes automatic with practice. In NLM, procedural memory would encode learned action patterns and sequences.

### Architecture

#### NeuralProceduralMemory.hpp
```cpp
#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../motor/Action.hpp"
#include <vector>
#include <memory>
#include <unordered_map>

namespace nlm {

class NeuralProceduralMemory {
public:
    NeuralProceduralMemory();
    ~NeuralProceduralMemory();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Learn an action sequence
    // sequence: ordered list of actions to learn
    // outcome: predicted result state (sensory pattern)
    // reward: experienced reward for executing this sequence
    void learnSequence(const std::vector<ActionType>& sequence,
                      const std::vector<float>& outcome,
                      float reward);
    
    // Execute a learned procedural pattern
    // context: current sensory state to guide pattern selection
    std::vector<ActionType> executePattern(const std::vector<float>& context);
    
    // Check if a sequence has been learned
    bool hasSequence(const std::vector<ActionType>& sequence) const;
    
    // Get the reward/value of a sequence
    float getSequenceValue(const std::vector<ActionType>& sequence) const;
    
    // Update procedural memory (practice, decay)
    void update(TimestepDuration dt);
    
    // Clear all procedural knowledge
    void clear();
    
    // Get all learned sequences
    std::vector<std::vector<ActionType>> getAllSequences() const;
    
    // Get sequences starting with a given action
    std::vector<std::vector<ActionType>> getSequencesStartingWith(ActionType action) const;
    
    // Checkpoint support
    struct ProceduralMemorySnapshot {
        std::vector<std::vector<ActionType>> sequences;           // Learned action sequences
        std::vector<std::vector<float>> outcomes;               // Expected outcomes
        std::vector<float> sequenceValues;                      // Sequence values/rewards
        std::vector<size_t> sequenceLengths;                    // Sequence lengths
    };
    
    ProceduralMemorySnapshot takeSnapshot() const;
    void restoreSnapshot(const ProceduralMemorySnapshot& snapshot);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Learning parameters
    float sequenceDecayRate = 0.0005f;
    float sequenceRelevanceDecay = 0.998f;
    float patternCompletionThreshold = 0.6f;
    size_t maxSequenceLength = 10;
    size_t maxStoredSequences = 1000;
};

} // namespace nlm
```

#### NeuralProceduralMemory.cpp - Implementation

```cpp
#include "NeuralProceduralMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralProceduralMemory::Impl {
    Brain* brain;
    
    // Procedural knowledge storage
    std::vector<std::vector<ActionType>> sequences;
    std::vector<std::vector<float>> outcomes;     // Expected sensory outcomes
    std::vector<float> sequenceValues;            // Sequence quality/reward
    std::vector<size_t> sequenceLengths;
    
    // For fast lookup
    std::unordered_map<ActionType, std::vector<size_t>> sequencesByFirstAction;
    std::unordered_map<size_t, std::vector<ActionType>> actionPatternCache;
    
    // Execution state
    std::vector<size_t> currentSequenceIndices;
    std::vector<size_t> actionStepCounters;
    
    Impl() : brain(nullptr) {}
};

NeuralProceduralMemory::NeuralProceduralMemory()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

NeuralProceduralMemory::~NeuralProceduralMemory() = default;

void NeuralProceduralMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralProceduralMemory initialized");
}

void NeuralProceduralMemory::learnSequence(const std::vector<ActionType>& sequence,
                                          const std::vector<float>& outcome,
                                          float reward) {
    if (sequence.empty() || !brain_) return;
    
    // Check if sequence already exists
    bool exists = false;
    size_t existingIndex = 0;
    
    for (size_t i = 0; i < pImpl->sequences.size(); ++i) {
        if (sequencesMatch(sequence, pImpl->sequences[i])) {
            exists = true;
            existingIndex = i;
            break;
        }
    }
    
    if (exists) {
        // Update existing sequence value
        pImpl->sequenceValues[existingIndex] = std::max(
            pImpl->sequenceValues[existingIndex],
            reward);
        return;
    }
    
    // Create new sequence if under capacity
    if (pImpl->sequences.size() >= maxStoredSequences) {
        // Remove lowest value sequence
        size_t minIndex = 0;
        for (size_t i = 1; i < pImpl->sequenceValues.size(); ++i) {
            if (pImpl->sequenceValues[i] < pImpl->sequenceValues[minIndex]) {
                minIndex = i;
            }
        }
        
        // Remove from lookup tables
        removeSequenceFromLookups(minIndex);
        
        // Remove from storage
        pImpl->sequences.erase(pImpl->sequences.begin() + minIndex);
        pImpl->outcomes.erase(pImpl->outcomes.begin() + minIndex);
        pImpl->sequenceValues.erase(pImpl->sequenceValues.begin() + minIndex);
        pImpl->sequenceLengths.erase(pImpl->sequenceLengths.begin() + minIndex);
    }
    
    // Store new sequence
    size_t sequenceId = pImpl->sequences.size();
    
    pImpl->sequences.push_back(sequence);
    pImpl->outcomes.push_back(outcome);
    pImpl->sequenceValues.push_back(reward);
    pImpl->sequenceLengths.push_back(sequence.size());
    
    // Update lookup tables
    addSequenceToLookups(sequenceId);
    
    // Encode sequence in neural patterns
    encodeSequenceInNeuralPattern(sequenceId, reward);
    
    NLM_LOG_INFO("ProceduralMemory: Learned sequence " + std::to_string(sequenceId) +
                 " (length: " + std::to_string(sequence.size()) +
                 ", reward: " + std::to_string(reward) + ")");
}

bool NeuralProceduralMemory::sequencesMatch(const std::vector<ActionType>& seqA,
                                            const std::vector<ActionType>& seqB) const {
    if (seqA.size() != seqB.size()) return false;
    
    for (size_t i = 0; i < seqA.size(); ++i) {
        if (seqA[i] != seqB[i]) return false;
    }
    
    return true;
}

std::vector<ActionType> NeuralProceduralMemory::executePattern(
    const std::vector<float>& context) {
    
    if (pImpl->sequences.empty()) return {};
    
    // Find best matching sequence for the context
    std::vector<std::pair<float, size_t>> scores; // (matchScore, sequenceIndex)
    
    for (size_t i = 0; i < pImpl->sequences.size(); ++i) {
        float score = computeSequenceMatchScore(pImpl->sequences[i], context);
        if (score > patternCompletionThreshold) {
            scores.emplace_back(score, i);
        }
    }
    
    if (scores.empty()) {
        // No good matches - return empty sequence
        return {};
    }
    
    // Sort by score (descending)
    std::sort(scores.begin(), scores.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Use best sequence
    size_t bestIndex = scores[0].second;
    
    // Get the sequence
    std::vector<ActionType> result = pImpl->sequences[bestIndex];
    
    // Initialize execution state
    pImpl->currentSequenceIndices.push_back(bestIndex);
    pImpl->actionStepCounters.push_back(0);
    
    // Encode the sequence in neural patterns for faster execution
    encodeSequenceInNeuralPattern(bestIndex, pImpl->sequenceValues[bestIndex]);
    
    NLM_LOG_INFO("ProceduralMemory: Executing pattern (score: " + std::to_string(scores[0].first) +
                 ", sequence: " + sequenceToString(pImpl->sequences[bestIndex]) + ")");
    
    return result;
}

float NeuralProceduralMemory::computeSequenceMatchScore(
    const std::vector<ActionType>& sequence,
    const std::vector<float>& context) const {
    
    // Simple scoring: prefer longer sequences and higher values
    float baseScore = pImpl->sequenceValues[&sequence - &pImpl->sequences[0]];
    
    // Context matching (simplified - in real implementation would use pattern matching)
    float contextScore = 0.5f; // Default neutral score
    
    // Prefer sequences that match the current action tendencies
    if (!context.empty() && context[0] > 0.5f) {
        contextScore = 0.7f;
    }
    
    return baseScore * 0.7f + contextScore * 0.3f;
}

void NeuralProceduralMemory::encodeSequenceInNeuralPattern(size_t sequenceIndex, float strength) {
    if (!brain_ || sequenceIndex >= pImpl->sequences.size()) return;
    
    const auto& sequence = pImpl->sequences[sequenceIndex];
    
    // Encode the sequence in motor neuron populations
    auto* region = brain_->getRegion(RegionId(1)); // Default to first region
    if (!region) return;
    
    auto* motorPop = region->getPopulation(2); // Motor population
    if (!motorPop) return;
    
    // For each action in the sequence, encode it
    for (size_t i = 0; i < sequence.size() && i < motorPop->getNeuronCount(); ++i) {
        float activation = strength * 0.3f * (i + 1) / sequence.size(); // Scale with position
        brain_->injectCurrent(motorPop->getNeuron(i)->getId(), activation);
    }
}

bool NeuralProceduralMemory::hasSequence(const std::vector<ActionType>& sequence) const {
    for (const auto& storedSeq : pImpl->sequences) {
        if (sequencesMatch(sequence, storedSeq)) {
            return true;
        }
    }
    return false;
}

float NeuralProceduralMemory::getSequenceValue(const std::vector<ActionType>& sequence) const {
    for (size_t i = 0; i < pImpl->sequences.size(); ++i) {
        if (sequencesMatch(sequence, pImpl->sequences[i])) {
            return pImpl->sequenceValues[i];
        }
    }
    return 0.0f;
}

void NeuralProceduralMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Decay sequence values over time
    for (auto& value : pImpl->sequenceValues) {
        value *= sequenceRelevanceDecay;
    }
    
    // Practice sequences (increase value)
    for (size_t i = 0; i < pImpl->currentSequenceIndices.size(); ++i) {
        size_t seqIndex = pImpl->currentSequenceIndices[i];
        if (seqIndex < pImpl->sequenceValues.size()) {
            // Practice strengthens the sequence
            pImpl->sequenceValues[seqIndex] = std::min(1.0f, 
                pImpl->sequenceValues[seqIndex] + 0.001f * dt);
            
            // Advance action step counter
            pImpl->actionStepCounters[i]++;
            
            // If we've practiced enough, encode the pattern more strongly
            if (pImpl->actionStepCounters[i] > 100) {
                // Strong reinforcement
                encodeSequenceInNeuralPattern(seqIndex, pImpl->sequenceValues[seqIndex] * 2.0f);
                pImpl->actionStepCounters[i] = 0;
            }
        }
    }
    
    // Remove low-value sequences
    consolidateLowValueSequences();
}

void NeuralProceduralMemory::consolidateLowValueSequences() {
    std::vector<size_t> sequencesToRemove;
    
    for (size_t i = 0; i < pImpl->sequenceValues.size(); ++i) {
        if (pImpl->sequenceValues[i] < 0.01f) {
            sequencesToRemove.push_back(i);
        }
    }
    
    // Remove low-value sequences in reverse order
    for (auto it = sequencesToRemove.rbegin(); it != sequencesToRemove.rend(); ++it) {
        size_t index = *it;
        removeSequenceFromLookups(index);
        
        pImpl->sequences.erase(pImpl->sequences.begin() + index);
        pImpl->outcomes.erase(pImpl->outcomes.begin() + index);
        pImpl->sequenceValues.erase(pImpl->sequenceValues.begin() + index);
        pImpl->sequenceLengths.erase(pImpl->sequenceLengths.begin() + index);
    }
}

void NeuralProceduralMemory::removeSequenceFromLookups(size_t sequenceIndex) {
    // Remove from sequencesByFirstAction
    if (sequenceIndex < pImpl->sequences.size()) {
        ActionType firstAction = pImpl->sequences[sequenceIndex][0];
        auto& actionSequences = pImpl->sequencesByFirstAction[firstAction];
        
        actionSequences.erase(
            std::remove(actionSequences.begin(), actionSequences.end(), sequenceIndex),
            actionSequences.end());
    }
}

void NeuralProceduralMemory::addSequenceToLookups(size_t sequenceIndex) {
    if (sequenceIndex >= pImpl->sequences.size()) return;
    
    ActionType firstAction = pImpl->sequences[sequenceIndex][0];
    pImpl->sequencesByFirstAction[firstAction].push_back(sequenceIndex);
}

std::vector<std::vector<ActionType>> NeuralProceduralMemory::getAllSequences() const {
    return pImpl->sequences;
}

std::vector<std::vector<ActionType>> NeuralProceduralMemory::getSequencesStartingWith(
    ActionType action) const {
    
    auto it = pImpl->sequencesByFirstAction.find(action);
    if (it == pImpl->sequencesByFirstAction.end()) {
        return {};
    }
    
    std::vector<std::vector<ActionType>> result;
    for (size_t seqIndex : it->second) {
        if (seqIndex < pImpl->sequences.size()) {
            result.push_back(pImpl->sequences[seqIndex]);
        }
    }
    
    return result;
}

std::string NeuralProceduralMemory::sequenceToString(const std::vector<ActionType>& sequence) {
    std::string result;
    for (size_t i = 0; i < sequence.size(); ++i) {
        if (i > 0) result += "->";
        switch (sequence[i]) {
            case ActionType::MoveForward: result += "MoveForward"; break;
            case ActionType::MoveBackward: result += "MoveBackward"; break;
            case ActionType::TurnLeft: result += "TurnLeft"; break;
            case ActionType::TurnRight: result += "TurnRight"; break;
            case ActionType::Interact: result += "Interact"; break;
            case ActionType::Wait: result += "Wait"; break;
            case ActionType::LookLeft: result += "LookLeft"; break;
            case ActionType::LookRight: result += "LookRight"; break;
            default: result += "Unknown"; break;
        }
    }
    return result;
}

NeuralProceduralMemory::ProceduralMemorySnapshot NeuralProceduralMemory::takeSnapshot() const {
    ProceduralMemorySnapshot snapshot;
    
    snapshot.sequences = pImpl->sequences;
    snapshot.outcomes = pImpl->outcomes;
    snapshot.sequenceValues = pImpl->sequenceValues;
    snapshot.sequenceLengths = pImpl->sequenceLengths;
    
    return snapshot;
}

void NeuralProceduralMemory::restoreSnapshot(const ProceduralMemorySnapshot& snapshot) {
    pImpl->sequences = snapshot.sequences;
    pImpl->outcomes = snapshot.outcomes;
    pImpl->sequenceValues = snapshot.sequenceValues;
    pImpl->sequenceLengths = snapshot.sequenceLengths;
    
    // Rebuild lookup tables
    pImpl->sequencesByFirstAction.clear();
    
    for (size_t i = 0; i < pImpl->sequences.size(); ++i) {
        if (!pImpl->sequences[i].empty()) {
            ActionType firstAction = pImpl->sequences[i][0];
            pImpl->sequencesByFirstAction[firstAction].push_back(i);
        }
    }
}

} // namespace nlm
```

## Priority 3: Missing Neuromodulators Implementation

### 3.1 Serotonin Implementation

#### Serotonin.hpp
```cpp
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get serotonin level
    float getLevel() const;
    
    // Set serotonin level
    void setLevel(float level);
    
    // Update serotonin dynamics
    void update(TimestepDuration dt);
    
    // Get plasticity modulation factor
    float getPlasticityFactor() const;
    
    // Signal mood change (positive or negative experience)
    void signalMoodChange(float moodDelta); // -1.0 = negative, +1.0 = positive
    
    // Get sensitivity to novel/salient stimuli
    float getSaliencyGain() const;
    
    // Apply serotonin effect to neurons
    void applyToNeuron(Neuron* neuron);
    
    // Reset to baseline
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Serotonergic system parameters
    float baselineLevel = 0.5f;
    float moodSensitivity = 1.0f;
    float salienceGain = 1.0f;
    float adaptationRate = 0.95f;
    float recoveryRate = 0.9f;
};

} // namespace nlm
```

#### Serotonin.cpp

```cpp
#include "Serotonin.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Serotonin::Impl {
    Brain* brain;
    
    // Current serotonin level
    float level;
    
    // Mood state (affects sensitivity to experiences)
    float moodState;
    
    // Internal dynamics
    float recoveryTarget;
    float adaptationTime;
    
    Impl() : level(0.5f), moodState(0.0f), recoveryTarget(0.5f), adaptationTime(0.0f) {}
};

Serotonin::Serotonin()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

Serotonin::~Serotonin() = default;

void Serotonin::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->level = baselineLevel;
    pImpl->moodState = 0.0f;
    NLM_LOG_INFO("Serotonin initialized");
}

float Serotonin::getLevel() const {
    return pImpl->level;
}

void Serotonin::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Serotonin::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Natural decay toward recovery target
    float recoveryDirection = pImpl->recoveryTarget - pImpl->level;
    pImpl->level += recoveryDirection * recoveryRate * dt;
    
    // Mood adaptation
    pImpl->moodState *= adaptationRate;
    
    // Limit level range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float Serotonin::getPlasticityFactor() const {
    // Serotonin modulates plasticity: high levels promote consolidation, low levels allow learning
    if (pImpl->level < 0.3f) {
        return 0.5f + pImpl->level * 0.5f; // Low: enhanced learning
    } else {
        return 1.0f + (pImpl->level - 0.3f) * 0.3f; // High: consolidation
    }
}

void Serotonin::signalMoodChange(float moodDelta) {
    pImpl->moodState += moodDelta;
    pImpl->moodState = std::clamp(pImpl->moodState, -1.0f, 1.0f);
    
    // Update recovery target based on mood
    pImpl->recoveryTarget = 0.5f + pImpl->moodState * 0.2f;
    
    // Immediate level change based on mood
    setLevel(getLevel() + moodDelta * 0.2f);
}

float Serotonin::getSaliencyGain() const {
    // Low serotonin = high saliency sensitivity
    return 1.0f / (pImpl->level + 0.1f);
}

void Serotonin::applyToNeuron(Neuron* neuron) {
    if (!neuron) return;
    
    // Serotonin effects on neuron:
    // - Modulates membrane excitability
    // - Affects synaptic plasticity
    // - Influences response to inputs
    
    float effect = (pImpl->level - 0.5f) * 2.0f; // -1 to +1
    
    // Adjust membrane threshold based on serotonin
    auto& state = neuron->getState();
    state.threshold *= (1.0f + effect * 0.2f);
    
    // Apply to plasticity
    if (effect > 0) {
        // High serotonin: promote long-term potentiation
        neuron->setPlasticityMultiplier(neuron->getPlasticityMultiplier() * 1.1f);
    } else {
        // Low serotonin: promote long-term depression
        neuron->setPlasticityMultiplier(neuron->getPlasticityMultiplier() * 0.9f);
    }
}

void Serotonin::reset() {
    pImpl->level = baselineLevel;
    pImpl->moodState = 0.0f;
    pImpl->recoveryTarget = baselineLevel;
}

} // namespace nlm
```

### 3.2 Norepinephrine Implementation

#### Norepinephrine.hpp
```cpp
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Norepinephrine {
public:
    Norepinephrine();
    ~Norepinephrine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get norepinephrine level
    float getLevel() const;
    
    // Set norepinephrine level
    void setLevel(float level);
    
    // Update norepinephrine dynamics
    void update(TimestepDuration dt);
    
    // Get arousal/attention modulation
    float getArousalLevel() const;
    
    // Signal alertness change
    void signalAlertness(float alertnessDelta); // -1.0 = decrease, +1.0 = increase
    
    // Get attention bias toward novel stimuli
    float getNoveltyBias() const;
    
    // Apply norepinephrine effect to neurons
    void applyToNeuron(Neuron* neuron, const std::vector<float>& sensoryContext);
    
    // Reset to baseline
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Noradrenergic system parameters
    float baselineLevel = 0.3f;
    float alertnessSensitivity = 1.0f;
    float noveltyGain = 1.5f;
    float adaptationRate = 0.9f;
    float recoveryRate = 0.85f;
};

} // namespace nlm
```

#### Norepinephrine.cpp

```cpp
#include "Norepinephrine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Norepinephrine::Impl {
    Brain* brain;
    
    // Current norepinephrine level
    float level;
    
    // Arousal state (affects attention and alertness)
    float arousalState;
    
    // Internal dynamics
    float recoveryTarget;
    float adaptationTime;
    
    Impl() : level(0.3f), arousalState(0.0f), recoveryTarget(0.3f), adaptationTime(0.0f) {}
};

Norepinephrine::Norepinephrine()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

Norepinephrine::~Norepinephrine() = default;

void Norepinephrine::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->level = baselineLevel;
    pImpl->arousalState = 0.0f;
    NLM_LOG_INFO("Norepinephrine initialized");
}

float Norepinephrine::getLevel() const {
    return pImpl->level;
}

void Norepinephrine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Norepinephrine::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Natural decay toward recovery target
    float recoveryDirection = pImpl->recoveryTarget - pImpl->level;
    pImpl->level += recoveryDirection * recoveryRate * dt;
    
    // Arousal adaptation
    pImpl->arousalState *= adaptationRate;
    
    // Limit level range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float Norepinephrine::getArousalLevel() const {
    return pImpl->arousalState;
}

void Norepinephrine::signalAlertness(float alertnessDelta) {
    pImpl->arousalState += alertnessDelta;
    pImpl->arousalState = std::clamp(pImpl->arousalState, -1.0f, 1.0f);
    
    // Update recovery target based on alertness
    pImpl->recoveryTarget = 0.3f + pImpl->arousalState * 0.15f;
    
    // Immediate level change based on alertness
    setLevel(getLevel() + alertnessDelta * 0.25f);
}

float Norepinephrine::getNoveltyBias() const {
    // Low norepinephrine = high novelty bias
    return 1.0f + (1.0f - pImpl->level) * noveltyGain;
}

void Norepinephrine::applyToNeuron(Neuron* neuron, const std::vector<float>& sensoryContext) {
    if (!neuron) return;
    
    // Norepinephrine effects on neuron:
    // - Increases responsiveness to sensory inputs
    // - Enhances signal-to-noise ratio
    // - Affects decision thresholds
    
    float effect = (pImpl->level - 0.3f) * 1.428f; // -1 to +1
    
    // Adjust neuronal responsiveness based on norepinephrine
    auto& state = neuron->getState();
    state.membranePotential *= (1.0f + effect * 0.3f);
    
    // Apply arousal modulation to plasticity
    if (effect > 0) {
        // High norepinephrine: enhances plasticity for salient stimuli
        neuron->setPlasticityMultiplier(neuron->getPlasticityMultiplier() * 1.15f);
        
        // Increase response gain to sensory inputs
        for (auto& syn : neuron->getInputConnections()) {
            syn->setWeight(syn->getWeight() * 1.05f);
        }
    }
}

void Norepinephrine::reset() {
    pImpl->level = baselineLevel;
    pImpl->arousalState = 0.0f;
    pImpl->recoveryTarget = baselineLevel;
}

} // namespace nlm
```

### 3.3 Acetylcholine Implementation

#### Acetylcholine.hpp
```cpp
#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Get acetylcholine level
    float getLevel() const;
    
    // Set acetylcholine level
    void setLevel(float level);
    
    // Update acetylcholine dynamics
    void update(TimestepDuration dt);
    
    // Get attention modulation factor
    float getAttentionGain() const;
    
    // Signal attentional focus
    void signalAttentionFocus(float attentionDelta); // -1.0 = narrow, +1.0 = broad
    
    // Get memory encoding enhancement
    float getMemoryEncodingBoost() const;
    
    // Apply acetylcholine effect to neurons
    void applyToNeuron(Neuron* neuron, const std::vector<float>& sensoryPattern);
    
    // Reset to baseline
    void reset();

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    Brain* brain_;
    
    // Cholinergic system parameters
    float baselineLevel = 0.4f;
    float attentionSensitivity = 1.0f;
    float memoryEncodingBoost = 1.5f;
    float adaptationRate = 0.88f;
    float recoveryRate = 0.82f;
};

} // namespace nlm
```

#### Acetylcholine.cpp

```cpp
#include "Acetylcholine.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct Acetylcholine::Impl {
    Brain* brain;
    
    // Current acetylcholine level
    float level;
    
    // Attention state (affects focus and filtering)
    float attentionState;
    
    // Internal dynamics
    float recoveryTarget;
    float adaptationTime;
    
    Impl() : level(0.4f), attentionState(0.0f), recoveryTarget(0.4f), adaptationTime(0.0f) {}
};

Acetylcholine::Acetylcholine()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

Acetylcholine::~Acetylcholine() = default;

void Acetylcholine::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    pImpl->level = baselineLevel;
    pImpl->attentionState = 0.0f;
    NLM_LOG_INFO("Acetylcholine initialized");
}

float Acetylcholine::getLevel() const {
    return pImpl->level;
}

void Acetylcholine::setLevel(float level) {
    pImpl->level = std::clamp(level, 0.0f, 1.0f);
}

void Acetylcholine::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Natural decay toward recovery target
    float recoveryDirection = pImpl->recoveryTarget - pImpl->level;
    pImpl->level += recoveryDirection * recoveryRate * dt;
    
    // Attention adaptation
    pImpl->attentionState *= adaptationRate;
    
    // Limit level range
    pImpl->level = std::clamp(pImpl->level, 0.0f, 1.0f);
}

float Acetylcholine::getAttentionGain() const {
    // High acetylcholine = high attentional gain
    return 1.0f + pImpl->level * attentionSensitivity;
}

void Acetylcholine::signalAttentionFocus(float attentionDelta) {
    pImpl->attentionState += attentionDelta;
    pImpl->attentionState = std::clamp(pImpl->attentionState, -1.0f, 1.0f);
    
    // Update recovery target based on attention
    pImpl->recoveryTarget = 0.4f + pImpl->attentionState * 0.1f;
    
    // Immediate level change based on attention
    setLevel(getLevel() + attentionDelta * 0.3f);
}

float Acetylcholine::getMemoryEncodingBoost() const {
    // High acetylcholine enhances memory encoding
    return 1.0f + pImpl->level * memoryEncodingBoost;
}

void Acetylcholine::applyToNeuron(Neuron* neuron, const std::vector<float>& sensoryPattern) {
    if (!neuron) return;
    
    // Acetylcholine effects on neuron:
    // - Enhances attention to sensory inputs
    // - Improves memory encoding and consolidation
    // - Modulates signal-to-noise ratio
    
    float effect = pImpl->level; // 0.0 to 1.0
    
    // Adjust neuronal response gain based on acetylcholine
    auto& state = neuron->getState();
    state.membranePotential *= (1.0f + effect * 0.25f);
    
    // Apply attentional modulation to plasticity
    if (effect > 0.5f) {
        // High acetylcholine: enhances memory encoding
        neuron->setPlasticityMultiplier(neuron->getPlasticityMultiplier() * 1.2f);
        
        // Increase sensitivity to sensory patterns
        for (auto& syn : neuron->getInputConnections()) {
            // Apply attentional gain to synapses
            float currentWeight = syn->getWeight();
            syn->setWeight(currentWeight * (1.0f + effect * 0.1f));
        }
        
        // Enhance pattern completion
        neuron->setPatternCompletionBoost(neuron->getPatternCompletionBoost() * (1.0f + effect * 0.15f));
    }
}

void Acetylcholine::reset() {
    pImpl->level = baselineLevel;
    pImpl->attentionState = 0.0f;
    pImpl->recoveryTarget = baselineLevel;
}

} // namespace nlm
```

## Integration with Brain Class

### Update Brain.hpp

Add includes and member variables for new systems:

```cpp
// In Brain.hpp, add these includes:
#include "../memory/NeuralSemanticMemory.hpp"
#include "../memory/NeuralProceduralMemory.hpp"
#include "../neuromodulation/Serotonin.hpp"
#include "../neuromodulation/Norepinephrine.hpp"
#include "../neuromodulation/Acetylcholine.hpp"

// In Brain::Impl struct, add these members:
// ========== NEW MEMORY SYSTEMS ==========
std::unique_ptr<NeuralSemanticMemory> semanticMemory;
std::unique_ptr<NeuralProceduralMemory> proceduralMemory;

// ========== COMPLETE NEUROMODULATION ==========
std::unique_ptr<Serotonin> serotonin;
std::unique_ptr<Norepinephrine> norepinephrine;
std::unique_ptr<Acetylcholine> acetylcholine;
```

### Update Brain.cpp

Update initialization and step() method to use new systems:

```cpp
// In Brain constructor:
// Initialize semantic memory
pImpl->semanticMemory->initialize(this);
pImpl->semanticMemory->setCapacity(500); // Number of concepts

// Initialize procedural memory  
pImpl->proceduralMemory->initialize(this);

// Initialize neuromodulation systems
pImpl->serotonin->initialize(this);
pImpl->norepinephrine->initialize(this);
pImpl->acetylcholine->initialize(this);

// In Brain::step() method, add new systems:
// ========== STEP 8: Update semantic memory ==========
if (pImpl->semanticMemory) {
    pImpl->semanticMemory->update(pImpl->timestep);
    
    // Update concept relevance based on neural activity
    if (pImpl->workingMemory) {
        auto pattern = pImpl->workingMemory->retrieve();
        if (!pattern.empty()) {
            size_t conceptId = pImpl->semanticMemory->storeConcept(
                pattern, "perceived", 0.5f);
        }
    }
}

// ========== STEP 9: Update procedural memory ==========
if (pImpl->proceduralMemory) {
    pImpl->proceduralMemory->update(pImpl->timestep);
    
    // Learn action patterns from agent brain
    if (agentBrain) {
        auto currentAction = agentBrain->decodeMotorCommand();
        std::vector<ActionType> sequence = {currentAction};
        auto outcome = agentBrain->getThoughtTrace(); // Simplified
        float reward = agentBrain->getReward();
        
        // Convert to float vector for outcome
        std::vector<float> outcomeVec;
        outcomeVec.reserve(outcome.size() + 6); // Add space for action encoding
        
        pImpl->proceduralMemory->learnSequence(sequence, outcomeVec, reward);
    }
}

// ========== STEP 10: Apply complete neuromodulation ==========
// Apply serotonin effects
if (pImpl->serotonin) {
    pImpl->serotonin->update(pImpl->timestep);
    
    // Serotonin modulates working memory consolidation
    if (pImpl->workingMemory && pImpl->serotonin->getLevel() > 0.5f) {
        // High serotonin promotes memory consolidation
        pImpl->workingMemory->strengthenMemory(1.05f);
    }
}

// Apply norepinephrine effects
if (pImpl->norepinephrine) {
    pImpl->norepinephrine->update(pImpl->timestep);
    
    // Norepinephrine modulates attention
    if (pImpl->attention) {
        float noveltyBias = pImpl->norepinephrine->getNoveltyBias();
        // Apply novelty bias to attention parameters
    }
}

// Apply acetylcholine effects
if (pImpl->acetylcholine) {
    pImpl->acetylcholine->update(pImpl->timestep);
    
    // Acetylcholine modulates episodic memory encoding
    if (pImpl->episodicMemory && pImpl->acetylcholine->getLevel() > 0.5f) {
        // High acetylcholine enhances memory encoding
        // (Would add to episodic memory in a full implementation)
    }
}

// ========== STEP 11: Apply integrated neuromodulation ==========
// Apply combined effects to plasticity
if (pImpl->dopamine && pImpl->serotonin && pImpl->norepinephrine && pImpl->acetylcholine) {
    // Calculate composite neuromodulatory signal
    float dopamine = pImpl->dopamine->getLevel();
    float serotonin = pImpl->serotonin->getLevel();
    float norepinephrine = pImpl->norepinephrine->getLevel();
    float acetylcholine = pImpl->acetylcholine->getLevel();
    
    // Composite signal for plasticity
    float composite = (dopamine * 0.4f + serotonin * 0.2f + 
                      norepinephrine * 0.2f + acetylcholine * 0.2f);
    
    // Apply composite to plasticity rules
    pImpl->stdp->setPlasticityFactor(composite);
}
```

## Testing and Validation

### Unit Tests

Create comprehensive tests for new systems:

```cpp
// Test Semantic Memory
TEST(SemanticMemory, StoreAndRetrieveConcept) {
    SemanticMemory mem;
    mem.initialize(nullptr);
    
    std::vector<float> pattern = {0.1f, 0.8f, 0.3f, 0.9f};
    size_t conceptId = mem.storeConcept(pattern, "visual", 0.8f);
    
    EXPECT_GT(conceptId, 0);
    EXPECT_EQ(mem.getConceptCategory(conceptId), "visual");
    EXPECT_NEAR(mem.getConceptRelevance(conceptId), 0.8f, 0.01f);
}

TEST(SemanticMemory, FindRelatedConcepts) {
    SemanticMemory mem;
    mem.initialize(nullptr);
    
    std::vector<float> pattern1 = {0.1f, 0.8f, 0.3f};
    std::vector<float> pattern2 = {0.15f, 0.75f, 0.35f}; // Similar to pattern1
    std::vector<float> pattern3 = {0.9f, 0.1f, 0.2f}; // Different
    
    size_t id1 = mem.storeConcept(pattern1, "category1", 0.9f);
    size_t id2 = mem.storeConcept(pattern2, "category1", 0.8f);
    size_t id3 = mem.storeConcept(pattern3, "category2", 0.7f);
    
    auto related = mem.findRelatedConcepts(pattern1, 0.7f);
    EXPECT_EQ(related.size(), 2); // id1 and id2 should be found
}

// Test Procedural Memory
TEST(ProceduralMemory, LearnAndExecuteSequence) {
    ProceduralMemory mem;
    mem.initialize(nullptr);
    
    std::vector<ActionType> sequence = {ActionType::MoveForward, ActionType::TurnLeft};
    std::vector<float> outcome = {0.5f, 0.8f, 0.3f};
    
    mem.learnSequence(sequence, outcome, 0.9f);
    EXPECT_TRUE(mem.hasSequence(sequence));
    EXPECT_NEAR(mem.getSequenceValue(sequence), 0.9f, 0.01f);
    
    auto executed = mem.executePattern({0.5f, 0.8f, 0.3f});
    EXPECT_FALSE(executed.empty());
}

// Test Neuromodulators
TEST(Serotonin, MoodSignaling) {
    Serotonin sero;
    sero.initialize(nullptr);
    
    EXPECT_NEAR(sero.getLevel(), 0.5f, 0.01f);
    
    sero.signalMoodChange(0.3f);
    EXPECT_NEAR(sero.getLevel(), 0.8f, 0.01f);
    
    sero.signalMoodChange(-0.5f);
    EXPECT_NEAR(sero.getLevel(), 0.3f, 0.01f);
}

TEST(Norepinephrine, NoveltyBias) {
    Norepinephrine norepi;
    norepi.initialize(nullptr);
    
    float lowLevelBias = norepi.getNoveltyBias();
    norepi.setLevel(0.8f);
    float highLevelBias = norepi.getNoveltyBias();
    
    EXPECT_GT(highLevelBias, lowLevelBias);
}

TEST(Acetylcholine, AttentionGain) {
    Acetylcholine ach;
    ach.initialize(nullptr);
    
    float baseGain = ach.getAttentionGain();
    ach.setLevel(0.9f);
    float highGain = ach.getAttentionGain();
    
    EXPECT_GT(highGain, baseGain);
}
```

### Integration Tests

Test how new systems work together:

```cpp
// Integration test for semantic + procedural memory
TEST(Integration, SemanticProceduralCoupling) {
    // Create brain with all systems
    auto config = std::make_shared<Config>();
    Brain brain(config);
    
    // Initialize AgentBrain
    AgentBrain agent(&brain);
    
    // Store semantic knowledge
    std::vector<float> pattern = {0.2f, 0.7f, 0.4f};
    size_t conceptId = agent.getSemanticMemory()->storeConcept(
        pattern, "resource_location", 0.9f);
    
    // Learn procedural sequence
    std::vector<ActionType> sequence = {ActionType::MoveForward, ActionType::Interact};
    std::vector<float> outcome = {0.5f, 0.8f, 0.3f};
    
    agent.getProceduralMemory()->learnSequence(sequence, outcome, 0.8f);
    
    // Verify coupling
    auto relatedConcepts = agent.getSemanticMemory()->findRelatedConcepts(pattern);
    auto matchingSequences = agent.getProceduralMemory()->getSequencesStartingWith(
        ActionType::MoveForward);
    
    EXPECT_FALSE(relatedConcepts.empty());
    EXPECT_FALSE(matchingSequences.empty());
}

// Integration test for neuromodulator synergy
TEST(Integration, NeuromodulatorSynergy) {
    auto config = std::make_shared<Config>();
    Brain brain(config);
    
    // Verify all neuromodulators are initialized
    EXPECT_TRUE(brain.getDopamine() != nullptr);
    EXPECT_TRUE(brain.getSerotonin() != nullptr);
    EXPECT_TRUE(brain.getNorepinephrine() != nullptr);
    EXPECT_TRUE(brain.getAcetylcholine() != nullptr);
    
    // Verify all cognitive systems are connected
    EXPECT_TRUE(brain.getWorkingMemory() != nullptr);
    EXPECT_TRUE(brain.getEpisodicMemory() != nullptr);
    EXPECT_TRUE(brain.getPredictionSystem() != nullptr);
    EXPECT_TRUE(brain.getPlanner() != nullptr);
    EXPECT_TRUE(brain.getConceptFormation() != nullptr);
    EXPECT_TRUE(brain.getAttention() != nullptr);
}
```

### Phase 6 Integration Test

Update the existing Phase 6 integration test to verify all new systems:

```cpp
// In Phase6IntegratedExperiment.cpp
void Phase6IntegratedExperiment::validateAllSystems() {
    std::cout << "=== Phase 6 Integration Validation ===\n";
    
    // Test 1: Memory systems
    std::cout << "1. Memory Systems: ";
    validateMemorySystems();
    
    // Test 2: Cognition systems  
    std::cout << "2. Cognition Systems: ";
    validateCognitionSystems();
    
    // Test 3: Neuromodulation (complete)
    std::cout << "3. Neuromodulation: ";
    validateNeuromodulationSystems();
    
    // Test 4: New systems integration
    std::cout << "4. New Systems Integration: ";
    validateNewSystemIntegration();
    
    // Test 5: End-to-end brain loop
    std::cout << "5. End-to-End Integration: ";
    validateEndToEndLoop();
    
    std::cout << "\n=== All Phase 6 systems validated ===\n";
}

void Phase6IntegratedExperiment::validateNewSystemIntegration() {
    // Test semantic memory in the brain loop
    auto& brain = experiment.getBrain();
    
    if (brain.getSemanticMemory()) {
        // Store semantic knowledge from sensory input
        auto percept = experiment.getCurrentPercept();
        std::vector<float> visionPattern = percept.getVision();
        
        if (!visionPattern.empty()) {
            size_t conceptId = brain.getSemanticMemory()->storeConcept(
                visionPattern, "visual_scene", 0.7f);
            
            std::cout << "   Semantic memory: Stored concept " << conceptId << "\n";
        }
    }
    
    if (brain.getProceduralMemory()) {
        // Learn action sequences from agent behavior
        auto& agent = experiment.getAgent();
        
        // Get current action sequence from agent's thought trace
        std::vector<ActionType> currentSequence;
        // (Extract from agent's internal state)
        
        if (!currentSequence.empty()) {
            std::vector<float> outcome = {0.5f, 0.7f, 0.3f}; // Simplified outcome
            float reward = agent.getReward();
            
            brain.getProceduralMemory()->learnSequence(
                currentSequence, outcome, reward);
            
            std::cout << "   Procedural memory: Learned sequence of length " 
                      << currentSequence.size() << "\n";
        }
    }
    
    std::cout << "   ✓ All new systems integrated\n";
}
```

## Build Instructions

After implementing the new systems, rebuild the project:

```bash
# Clean rebuild
rm -rf build && mkdir build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Run tests
make test

# Run Phase 6 demo
./nlm_phase6_demo

# Run benchmark
./nlm_benchmark
```

## Expected Improvements

### Performance Impact
- **Memory**: Semantic memory enables faster concept retrieval, procedural memory enables faster skill execution
- **Learning**: Complete neuromodulation system improves learning efficiency
- **Cohesion**: Better integration between cognitive systems

### Capabilities
- **Semantic Knowledge**: Can store and retrieve abstract concepts
- **Procedural Skills**: Can learn and execute complex action sequences
- **Rich Neuromodulation**: Mood, arousal, attention, memory modulation
- **Enhanced Integration**: All systems work together in the brain loop

### Code Quality
- **Complete Documentation**: Detailed headers and implementation comments
- **Comprehensive Tests**: Unit and integration tests for all new systems
- **Clean Architecture**: Consistent with existing NLM design patterns
- **Performance Optimized**: Efficient lookup tables and algorithms

## Summary

This implementation plan provides a complete roadmap for adding the missing systems to NLM:

1. **Semantic Memory** (`NeuralSemanticMemory.hpp/cpp`) - Pattern-based knowledge storage
2. **Procedural Memory** (`NeuralProceduralMemory.hpp/cpp`) - Skill and action sequence learning
3. **Serotonin** (`Serotonin.hpp/cpp`) - Mood and sensitivity regulation
4. **Norepinephrine** (`Norepinephrine.hpp/cpp`) - Arousal and novelty processing
5. **Acetylcholine** (`Acetylcholine.hpp/cpp`) - Attention and memory encoding

These systems will:
- Complete the Phase 6 integration requirements
- Enable more sophisticated cognitive capabilities
- Provide biologically plausible neuromodulation
- Significantly improve the brain's ability to learn, remember, and behave intelligently

The implementation follows NLM's existing patterns, integrates seamlessly with the brain loop, and includes comprehensive testing to ensure robustness and correctness.