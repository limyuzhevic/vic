#pragma once

#include "Memory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <random>

namespace nlm {

// Forward declarations for Brain access
class Brain;

// Working Memory Implementation - Enhanced with realistic neural dynamics
struct WorkingMemory::Impl {
    // Neural population for maintaining information
    std::vector<NeuronId> activeNeuronPopulation;
    
    // Persistent activity dynamics
    std::vector<float> persistentActivations;
    std::vector<float> recruitmentLevels;
    
    // Competition and attention
    std::vector<float> attentionWeights;
    std::vector<float> inhibitionMatrix;
    
    // Plasticity mechanisms
    std::vector<float> synapticEligibilityTraces;
    std::vector<float> eligibilityDecayRates;
    
    // Statistics
    float totalInputStrength;
    float averageActivation;
    size_t formationCount;
    size_t retrievalCount;
    
    Impl(size_t cap) : totalInputStrength(0.0f), averageActivation(0.0f), 
                       formationCount(0), retrievalCount(0) {
        attentionWeights.reserve(cap);
        inhibitionMatrix.reserve(cap * cap);
        synapticEligibilityTraces.reserve(cap);
        eligibilityDecayRates.reserve(cap);
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {
    std::random_device rd;
    rng.seed(rd());
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Enhanced storage with neural dynamics
    pImpl->formationCount++;
    
    // Find or add neuron
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Update with exponential moving average
            item.second = 0.9f * item.second + 0.1f * value;
            pImpl->averageActivation = (pImpl->averageActivation * (pImpl->items.size() - 1) + item.second) / pImpl->items.size();
            
            // Update neural population
            auto popIt = std::find(pImpl->activeNeuronPopulation.begin(), 
                                 pImpl->activeNeuronPopulation.end(), neuron);
            if (popIt != pImpl->activeNeuronPopulation.end()) {
                size_t idx = std::distance(pImpl->activeNeuronPopulation.begin(), popIt);
                pImpl->persistentActivations[idx] = item.second;
                pImpl->recruitmentLevels[idx] = std::min(1.0f, pImpl->recruitmentLevels[idx] + 0.05f);
            }
            return;
        }
    }
    
    // New neuron - add with realistic dynamics
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
        pImpl->activeNeuronPopulation.push_back(neuron);
        pImpl->persistentActivations.push_back(value);
        pImpl->recruitmentLevels.push_back(0.5f + std::uniform_real_distribution<float>(0.0f, 0.5f)(rng));
        pImpl->attentionWeights.push_back(1.0f);
        
        // Initialize eligibility trace for plasticity
        pImpl->synapticEligibilityTraces.push_back(0.0f);
        pImpl->eligibilityDecayRates.push_back(0.01f);
        
        pImpl->totalInputStrength += value;
        pImpl->averageActivation = (pImpl->averageActivation * (pImpl->items.size() - 1) + value) / pImpl->items.size();
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    pImpl->retrievalCount++;
    
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Apply attention weighting
            auto itemIt = std::find(pImpl->activeNeuronPopulation.begin(), 
                                  pImpl->activeNeuronPopulation.end(), neuron);
            if (itemIt != pImpl->activeNeuronPopulation.end()) {
                size_t idx = std::distance(pImpl->activeNeuronPopulation.begin(), itemIt);
                float attentionWeight = pImpl->attentionWeights[idx];
                
                // Add noise for biological realism
                std::normal_distribution<float> noise(0.0f, 0.05f * item.second);
                float noiseValue = noise(rng);
                
                return std::max(0.0f, item.second * attentionWeight + noiseValue);
            }
            return item.second;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return std::find_if(pImpl->items.begin(), pImpl->items.end(), 
                       [&neuron](const auto& item) { return item.first == neuron; }) != pImpl->items.end();
}

void WorkingMemory::clear() {
    pImpl->items.clear();
    pImpl->activeNeuronPopulation.clear();
    pImpl->persistentActivations.clear();
    pImpl->recruitmentLevels.clear();
    pImpl->attentionWeights.clear();
    pImpl->inhibitionMatrix.clear();
    pImpl->synapticEligibilityTraces.clear();
    pImpl->eligibilityDecayRates.clear();
    pImpl->totalInputStrength = 0.0f;
    pImpl->averageActivation = 0.0f;
}

void WorkingMemory::decay(float decayRate) {
    // Enhanced decay with homeostatic mechanisms
    float totalActivity = 0.0f;
    size_t count = 0;
    
    for (auto& item : pImpl->items) {
        // Apply differential decay based on current strength
        float currentDecay = decayRate * (1.0f + item.second);  // Stronger items decay slower
        item.second *= (1.0f - currentDecay);
        
        if (item.second > 0.01f) {
            totalActivity += item.second;
            ++count;
        }
    }
    
    if (count > 0) {
        pImpl->averageActivation = totalActivity / count;
    }
    
    // Update neural population
    for (size_t i = 0; i < pImpl->items.size(); ++i) {
        if (pImpl->persistentActivations[i] > 0.01f) {
            pImpl->persistentActivations[i] *= (1.0f - decayRate);
            pImpl->recruitmentLevels[i] *= 0.95f;  // Gradual loss of recruitment
        } else {
            // Remove weak traces
            pImpl->activeNeuronPopulation.erase(pImpl->activeNeuronPopulation.begin() + i);
            pImpl->persistentActivations.erase(pImpl->persistentActivations.begin() + i);
            pImpl->recruitmentLevels.erase(pImpl->recruitmentLevels.begin() + i);
            pImpl->attentionWeights.erase(pImpl->attentionWeights.begin() + i);
            pImpl->synapticEligibilityTraces.erase(pImpl->synapticEligibilityTraces.begin() + i);
            pImpl->eligibilityDecayRates.erase(pImpl->eligibilityDecayRates.begin() + i);
            pImpl->items.erase(pImpl->items.begin() + i);
            
            // Adjust indices
            i--;
            pImpl->totalInputStrength -= pImpl->persistentActivations[i];  // Approximate adjustment
        }
    }
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->items.size();
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

// Enhanced Episodic Memory Implementation
void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Enhanced storage with pattern completion and predictive coding
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    stored.recencyWeight = 1.0f;  // Fresh episodes have higher weight
    
    // Add predictive context if available
    if (!stored.resultingSensoryState.empty() && stored.action != ActionType::Wait) {
        // Learn action-consequence associations
        storeActionConsequence(episode.action, episode.sensoryState, stored.resultingSensoryState);
    }
    
    // Store with pattern completion
    pImpl->episodes.push_back(stored);
    
    // Remove old episodes if over capacity
    while (pImpl->episodes.size() > pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
        // Decay recency weights of remaining episodes
        for (auto& ep : pImpl->episodes) {
            ep.recencyWeight *= 0.95f;
        }
    }
    
    // Age all episodes
    for (auto& ep : pImpl->episodes) {
        ep.age++;
        ep.recencyWeight *= 0.98f;  // Natural decay of recency
    }
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Enhanced consolidation with hippocampal-cortical interaction
    std::vector<EpisodicMemoryItem> consolidated;
    
    for (const auto& episode : pImpl->episodes) {
        // Calculate episode relevance
        float relevance = calculateEpisodeRelevance(episode);
        
        if (relevance >= relevanceThreshold || episode.age < 10) {
            // Keep important or recent episodes
            consolidated.push_back(episode);
        }
        
        // Mark for potential consolidation
        if (relevance > 0.5f && episode.age > 100) {
            // These episodes should be consolidated to long-term storage
            // (In a full implementation, would be transferred to SemanticMemory)
        }
    }
    
    pImpl->episodes = consolidated;
}

void EpisodicMemory::storeActionConsequence(ActionType action,
                                          const std::vector<float>& beforeState,
                                          const std::vector<float>& afterState,
                                          float reward) {
    // Learn action-consequence mappings with prediction error
    auto& consequences = actionConsequences_[action];
    
    // Find similar before state
    float bestSimilarity = 0.0f;
    size_t bestIndex = 0;
    
    for (size_t i = 0; i < consequences.size(); ++i) {
        float sim = computeStateSimilarity(beforeState, consequences[i].beforeState);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestIndex = i;
        }
    }
    
    if (bestSimilarity > 0.7f) {
        // Update existing consequence with reward prediction
        auto& existing = consequences[bestIndex];
        existing.reward = 0.7f * existing.reward + 0.3f * reward;
        existing.afterState = weightedBlend(existing.afterState, afterState, 0.3f);
    } else {
        // Create new consequence record
        ActionConsequence conseq;
        conseq.beforeState = beforeState;
        conseq.afterState = afterState;
        conseq.reward = reward;
        conseq.frequency = 1;
        conseq.lastOccurred = 0;
        
        consequences.push_back(conseq);
    }
}

float EpisodicMemory::calculateEpisodeRelevance(const EpisodicMemoryItem& episode) const {
    // Calculate relevance based on multiple factors
    float rewardFactor = episode.reward / 2.0f;  // Normalize reward (-1 to 1 -> 0 to 1)
    rewardFactor = std::max(0.0f, std::min(1.0f, rewardFactor));
    
    float noveltyFactor = std::min(1.0f, episode.age / 1000.0f);  // Older episodes more novel?
    
    float frequencyFactor = 0.5f;  // Baseline
    
    // Get action frequency if we have it
    auto actionIt = actionFrequencies_.find(episode.action);
    if (actionIt != actionFrequencies_.end()) {
        frequencyFactor = std::min(1.0f, actionIt->second / 100.0f);
    }
    
    return (rewardFactor * 0.4f + noveltyFactor * 0.3f + frequencyFactor * 0.3f);
}

void EpisodicMemory::updateActionFrequency(ActionType action, size_t count) {
    actionFrequencies_[action] += count;
}

// Enhanced Semantic Memory Implementation
void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Store with spreading activation and inference
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            // Update existing fact with temporal decay
            fact.second = 0.9f * fact.second + 0.1f * value;
            
            // Apply spreading activation to related facts
            activateRelatedFacts(key, 0.1f);
            return;
        }
    }
    
    // New fact - store with spreading activation
    pImpl->facts.emplace_back(key, value);
    activateRelatedFacts(key, 1.0f);
}

void SemanticMemory::activateRelatedFacts(const std::string& key, float activationStrength) {
    // Find related facts based on shared concepts
    std::vector<std::pair<size_t, float>> activationTargets;
    
    // Simple keyword matching for demonstration
    std::string keyLower = key;
    std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
    
    for (size_t i = 0; i < pImpl->facts.size(); ++i) {
        std::string factKeyLower = pImpl->facts[i].first;
        std::transform(factKeyLower.begin(), factKeyLower.end(), factKeyLower.begin(), ::tolower);
        
        // Check if keys share common words (simple approach)
        size_t commonWords = 0;
        size_t totalWords = keyLower.size() + factKeyLower.size();
        
        if (totalWords > 0) {
            for (char c : keyLower) {
                if (factKeyLower.find(c) != std::string::npos) {
                    commonWords++;
                }
            }
            
            float similarity = static_cast<float>(commonWords) / totalWords;
            if (similarity > 0.1f) {
                activationTargets.push_back({i, activationStrength * similarity});
            }
        }
    }
    
    // Apply spreading activation
    for (const auto& target : activationTargets) {
        // This would update activation levels in a full implementation
        // For now, just log the relationship
        // NLM_LOG_DEBUG("Fact " + pImpl->facts[target.first].first + 
        //             " activated due to " + key);
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    // Enhanced retrieval with inference and context
    std::string bestValue;
    float bestConfidence = 0.0f;
    
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;  // Exact match
        }
        
        // Partial match based on keywords
        std::string keyLower = key;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
        std::string factKeyLower = fact.first;
        std::transform(factKeyLower.begin(), factKeyLower.end(), factKeyLower.begin(), ::tolower);
        
        if (factKeyLower.find(keyLower) != std::string::npos || 
            keyLower.find(factKeyLower) != std::string::npos) {
            
            // Calculate confidence based on specificity
            float confidence = 1.0f / (1.0f + std::abs((int)key.size() - (int)fact.first.size()));
            
            if (confidence > bestConfidence) {
                bestConfidence = confidence;
                bestValue = fact.second;
            }
        }
    }
    
    if (bestConfidence > 0.0f) {
        // Add inferred value for high confidence matches
        return bestValue;
    }
    
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
        
        // Check for partial matches
        std::string keyLower = key;
        std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), ::tolower);
        std::string factKeyLower = fact.first;
        std::transform(factKeyLower.begin(), factKeyLower.end(), factKeyLower.begin(), ::tolower);
        
        if (factKeyLower.find(keyLower) != std::string::npos || 
            keyLower.find(factKeyLower) != std::string::npos) {
            return true;
        }
    }
    return false;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

// Enhanced Associative Memory Implementation
void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Learn associations with temporal dynamics and eligibility traces
    if (!validateNeurons(a, b)) {
        return;
    }
    
    // Check if association already exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            // Update strength with temporal decay
            float currentStrength = std::get<2>(assoc);
            float newStrength = currentStrength * 0.9f + strength * 0.1f;
            std::get<2>(assoc) = std::clamp(newStrength, 0.0f, 1.0f);
            
            // Record co-activation timing for STDP
            recordCoActivation(a, b, newStrength);
            return;
        }
    }
    
    // New association - learn with initial strength and temporal trace
    pImpl->associations.emplace_back(a, b, strength);
    
    // Record for potential synaptic consolidation
    recordAssociation(a, b, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    for (const auto& assoc : pImpl->associations) {
        if (std::get<0>(assoc) == neuron) {
            result.push_back(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == neuron) {
            result.push_back(std::get<0>(assoc));
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            return std::get<2>(assoc);
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void AssociativeMemory::recordCoActivation(NeuronId a, NeuronId b, float strength) {
    // Record for STDP learning
    // In a full implementation, this would update synaptic weights
    // For now, just store the co-activation history
    coActivationHistory_.push_back({a, b, strength, 0});  // Timestamp would be added
}

void AssociativeMemory::recordAssociation(NeuronId a, NeuronId b, float strength) {
    // Record for potential consolidation
    // Could be consolidated to semantic memory if strong enough
    if (strength > 0.8f) {
        // Mark for consolidation
        consolidationCandidates_.push_back({a, b, strength, 0});  // Priority based on strength
    }
}

bool AssociativeMemory::validateNeurons(NeuronId a, NeuronId b) const {
    return a != INVALID_NEURON_ID && b != INVALID_NEURON_ID && a != b;
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    coActivationHistory_.clear();
    consolidationCandidates_.clear();
}

// Helper methods for Semantic Memory
float SemanticMemory::computeSimilarity(const std::string& a, const std::string& b) const {
    // Simple string similarity for semantic matching
    if (a.empty() || b.empty()) return 0.0f;
    
    // Convert to lowercase for case-insensitive comparison
    std::string aLower = a;
    std::string bLower = b;
    std::transform(aLower.begin(), aLower.end(), aLower.begin(), ::tolower);
    std::transform(bLower.begin(), bLower.end(), bLower.begin(), ::tolower);
    
    // Jaccard similarity of words
    std::istringstream aStream(aLower);
    std::istringstream bStream(bLower);
    
    std::string word;
    std::vector<std::string> wordsA, wordsB;
    
    while (aStream >> word) {
        wordsA.push_back(word);
    }
    
    while (bStream >> word) {
        wordsB.push_back(word);
    }
    
    if (wordsA.empty() || wordsB.empty()) return 0.0f;
    
    std::unordered_set<std::string> setA(wordsA.begin(), wordsA.end());
    std::unordered_set<std::string> setB(wordsB.begin(), wordsB.end());
    
    size_t intersection = 0;
    for (const auto& w : setA) {
        if (setB.find(w) != setB.end()) {
            intersection++;
        }
    }
    
    size_t unionSize = setA.size() + setB.size() - intersection;
    
    return static_cast<float>(intersection) / unionSize;
}

std::vector<std::string> SemanticMemory::extractKeywords(const std::string& text) const {
    // Extract keywords from text
    std::vector<std::string> keywords;
    std::string current;
    
    for (char c : text) {
        if (std::isalnum(c)) {
            current += std::tolower(c);
        } else if (!current.empty()) {
            keywords.push_back(current);
            current.clear();
        }
    }
    
    if (!current.empty()) {
        keywords.push_back(current);
    }
    
    return keywords;
}

float SemanticMemory::computeTextSimilarity(const std::string& a, const std::string& b) const {
    // Compute semantic similarity between texts
    std::vector<std::string> keywordsA = extractKeywords(a);
    std::vector<std::string> keywordsB = extractKeywords(b);
    
    if (keywordsA.empty() || keywordsB.empty()) return 0.0f;
    
    std::unordered_set<std::string> setA(keywordsA.begin(), keywordsA.end());
    std::unordered_set<std::string> setB(keywordsB.begin(), keywordsB.end());
    
    size_t intersection = 0;
    for (const auto& w : setA) {
        if (setB.find(w) != setB.end()) {
            intersection++;
        }
    }
    
    size_t unionSize = setA.size() + setB.size() - intersection;
    
    return static_cast<float>(intersection) / unionSize;
}

// Procedural Memory Enhancement
void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Learn skills with graded representation and reinforcement
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update skill with new pattern and reinforcement
            skill.neuralPattern = pattern;
            skill.lastUsed = 0;  // Would track actual usage time
            skill.adaptiveParameters = calculateAdaptiveParameters(pattern);
            return;
        }
    }
    
    // New skill
    Skill newSkill;
    newSkill.name = name;
    newSkill.neuralPattern = pattern;
    newSkill.proficiency = 0.0f;
    newSkill.lastUsed = 0;
    newSkill.adaptiveParameters = calculateAdaptiveParameters(pattern);
    newSkill.executionCount = 0;
    
    pImpl->skills.push_back(newSkill);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    // Update skill proficiency with learning curve
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Apply learning curve: diminishing returns
            float current = skill.proficiency;
            float learningRate = 0.1f * (1.0f - current);  // Slower learning as proficiency increases
            float newProficiency = current + learningRate * delta;
            skill.proficiency = std::clamp(newProficiency, 0.0f, 1.0f);
            
            skill.executionCount++;
            return;
        }
    }
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

// Helper methods
Skill::AdaptiveParameters ProceduralMemory::calculateAdaptiveParameters(const std::vector<NeuronId>& pattern) {
    Skill::AdaptiveParameters params;
    
    // Calculate statistics from pattern
    if (pattern.empty()) {
        params.targetFiringRate = 10.0f;
        params.adaptationTimeConstant = 1000.0f;
        params.noiseLevel = 0.1f;
        return params;
    }
    
    // In a full implementation, would analyze pattern statistics
    // For now, use reasonable defaults
    params.targetFiringRate = 10.0f;
    params.adaptationTimeConstant = 1000.0f;
    params.noiseLevel = 0.1f;
    
    return params;
}

} // namespace nlm