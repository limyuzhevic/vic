#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    struct Item {
        NeuronId neuron;
        float value;
        float decayFactor;
        size_t accessCount;
        Timestamp lastAccessTime;
        
        Item(NeuronId n, float v) : neuron(n), value(v), decayFactor(1.0f), accessCount(0), lastAccessTime(0.0f) {}
    };
    
    std::vector<Item> items;
    size_t capacity;
    float defaultDecayRate;
    
    Impl(size_t cap) : capacity(cap), defaultDecayRate(0.01f) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // TODO PHASE 2: Implement real storage with capacity limits
    for (auto& item : pImpl->items) {
        if (item.neuron == neuron) {
            item.value = value;
            item.decayFactor = 1.0f;
            item.accessCount++;
            return;
        }
    }
    
    if (pImpl->items.size() >= pImpl->capacity) {
        // Evict least recently accessed item (simple FIFO for now)
        pImpl->items.erase(pImpl->items.begin());
    }
    pImpl->items.emplace_back(neuron, value);
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.neuron == neuron) {
            return item.value;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.neuron == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->items.clear();
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::decay(float decayRate) {
    for (auto& item : pImpl->items) {
        item.value *= item.decayFactor * (1.0f - decayRate);
        item.decayFactor *= 0.99f;  // Gradual decay of decay factor
    }
}

// Get all items with their access counts (for attention selection)
std::vector<std::pair<NeuronId, float>> WorkingMemory::getAllItems() const {
    std::vector<std::pair<NeuronId, float>> result;
    result.reserve(pImpl->items.size());
    for (const auto& item : pImpl->items) {
        result.emplace_back(item.neuron, item.value);
    }
    return result;
}

// Update item value and reset decay
void WorkingMemory::updateItem(NeuronId neuron, float newValue) {
    for (auto& item : pImpl->items) {
        if (item.neuron == neuron) {
            item.value = newValue;
            item.decayFactor = 1.0f;
            item.accessCount++;
            break;
        }
    }
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    pImpl->episodes.push_back(episode);
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    }
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // TODO PHASE 2: Implement real consolidation
    // Implement synaptic strengthening for important episodes
    
    // Mark recent episodes as important for consolidation
    if (pImpl->episodes.empty()) return;
    
    // Calculate relevance based on duration, reward, and neural activation
    std::vector<bool> toConsolidate;
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Calculate episode relevance (0.0 to 1.0)
        float relevance = 0.0f;
        
        // Factor 1: Duration (longer episodes more relevant)
        float durationFactor = std::min(1.0f, static_cast<float>(episode.timestamp) / 1000.0f);
        relevance += durationFactor * 0.3f;
        
        // Factor 2: Neural activation (high activation = more relevant)
        float activationSum = 0.0f;
        for (float val : episode.values) {
            activationSum += val;
        }
        float activationFactor = std::min(1.0f, activationSum / 10.0f);
        relevance += activationFactor * 0.4f;
        
        // Factor 3: Reward (positive reward = more relevant)
        float rewardFactor = std::min(1.0f, (episode.reward + 1.0f) / 2.0f);
        relevance += rewardFactor * 0.3f;
        
        toConsolidate.push_back(relevance >= relevanceThreshold);
    }
    
    // Remove irrelevant episodes (low relevance)
    auto newEnd = std::remove_if(pImpl->episodes.begin(), pImpl->episodes.end(),
        [&toConsolidate](const EpisodicMemoryItem& episode, size_t index) {
            return !toConsolidate[index];
        });
    pImpl->episodes.erase(newEnd, pImpl->episodes.end());
    
    // For consolidated episodes, increase their salience for replay
    for (auto& episode : pImpl->episodes) {
        if (toConsolidate[&episode - &pImpl->episodes[0]]) {
            episode.metadata += "|CONSOLIDATED";
        }
    }
}

// Get episodes with relevance scores for replay
std::vector<std::pair<EpisodicMemoryItem, float>> EpisodicMemory::getRelevantEpisodes(
    size_t maxCount, float minRelevance) const {
    
    struct EpisodeWithRelevance {
        EpisodicMemoryItem episode;
        float relevance;
    };
    
    std::vector<EpisodeWithRelevance> episodesWithRelevance;
    episodesWithRelevance.reserve(pImpl->episodes.size());
    
    for (const auto& episode : pImpl->episodes) {
        EpisodeWithRelevance er;
        er.episode = episode;
        
        // Calculate relevance (simplified version of consolidation logic)
        float relevance = 0.0f;
        
        // Neural activation
        float activationSum = 0.0f;
        for (float val : episode.values) {
            activationSum += val;
        }
        relevance += std::min(1.0f, activationSum / 10.0f) * 0.5f;
        
        // Reward
        float rewardFactor = std::min(1.0f, (episode.reward + 1.0f) / 2.0f);
        relevance += rewardFactor * 0.5f;
        
        er.relevance = relevance;
        if (relevance >= minRelevance) {
            episodesWithRelevance.push_back(er);
        }
    }
    
    // Sort by relevance (highest first)
    std::sort(episodesWithRelevance.begin(), episodesWithRelevance.end(),
        [](const EpisodeWithRelevance& a, const EpisodeWithRelevance& b) {
            return a.relevance > b.relevance;
        });
    
    // Take top N
    size_t count = std::min(maxCount, episodesWithRelevance.size());
    std::vector<std::pair<EpisodicMemoryItem, float>> result;
    result.reserve(count);
    for (size_t i = 0; i < count; ++i) {
        result.push_back(std::make_pair(
            episodesWithRelevance[i].episode,
            episodesWithRelevance[i].relevance
        ));
    }
    
    return result;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    struct Concept {
        std::string name;
        std::vector<NeuronId> neuralPattern;  // Pattern of neurons representing this concept
        std::vector<std::string> properties;  // String properties for quick access
        float activationLevel;  // Current activation/salience
        float stability;  // How stable the concept is (0.0 to 1.0)
        Timestamp lastUpdated;
        
        Concept(const std::string& n) : name(n), activationLevel(0.0f), stability(1.0f), lastUpdated(0.0f) {}
    };
    
    std::vector<Concept> concepts;
    std::vector<std::pair<std::string, std::string>> simpleFacts;  // For backward compatibility
    
    // Association strength between concepts
    std::map<std::string, std::map<std::string, float>> associations;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Primary simple fact storage (backward compatibility)
    for (auto& fact : pImpl->simpleFacts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->simpleFacts.emplace_back(key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    // Try concept-based retrieval first
    for (const auto& concept : pImpl->concepts) {
        if (concept.name == key) {
            // Return as JSON-like string for now
            return "{\"concept\":\"" + concept.name + "\",\"activation\":" + 
                   std::to_string(concept.activationLevel) + "}";
        }
    }
    
    // Fall back to simple facts
    for (const auto& fact : pImpl->simpleFacts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& concept : pImpl->concepts) {
        if (concept.name == key) {
            return true;
        }
    }
    for (const auto& fact : pImpl->simpleFacts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

void SemanticMemory::storeConcept(const std::string& name, const std::vector<NeuronId>& pattern) {
    // Check if concept already exists
    for (auto& concept : pImpl->concepts) {
        if (concept.name == name) {
            concept.neuralPattern = pattern;
            concept.lastUpdated = 0.0f;  // Will be set by caller
            return;
        }
    }
    
    // Create new concept
    Concept newConcept(name);
    newConcept.neuralPattern = pattern;
    newConcept.lastUpdated = 0.0f;
    pImpl->concepts.push_back(newConcept);
}

float SemanticMemory::getConceptActivation(const std::string& name) const {
    for (const auto& concept : pImpl->concepts) {
        if (concept.name == name) {
            return concept.activationLevel;
        }
    }
    return 0.0f;
}

void SemanticMemory::updateConceptActivation(const std::string& name, float delta) {
    for (auto& concept : pImpl->concepts) {
        if (concept.name == name) {
            concept.activationLevel = std::max(0.0f, std::min(1.0f, concept.activationLevel + delta));
            return;
        }
    }
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result = pImpl->simpleFacts;
    
    // Add concept information
    for (const auto& concept : pImpl->concepts) {
        std::string value = "concept:neural_pattern_size=" + std::to_string(concept.neuralPattern.size()) +
                           ",activation=" + std::to_string(concept.activationLevel) +
                           ",stability=" + std::to_string(concept.stability);
        result.emplace_back("concept_" + concept.name, value);
    }
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->concepts.clear();
    pImpl->simpleFacts.clear();
    pImpl->associations.clear();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    struct Skill {
        std::string name;
        std::vector<NeuronId> neuralPattern;
        float proficiency;
        float stability;
        Timestamp lastPracticed;
        Timestamp lastImproved;
        std::vector<float> performanceHistory;
        
        Skill() : proficiency(0.0f), stability(1.0f), lastPracticed(0.0f), lastImproved(0.0f) {}
    };
    
    std::vector<Skill> skills;
    float defaultLearningRate;
    size_t maxHistorySize;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {
    pImpl->defaultLearningRate = 0.1f;
    pImpl->maxHistorySize = 100;
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            skill.stability = 1.0f;
            skill.lastImproved = 0.0f;  // Will be set by caller
            return;
        }
    }
    
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.stability = 1.0f;
    skill.lastImproved = 0.0f;
    pImpl->skills.push_back(skill);
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

const Skill* ProceduralMemory::getSkill(const std::string& name) const {
    for (const auto& skill : pImpl->skills) {
        if (skill.name == name) {
            return &skill;
        }
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

std::vector<const Skill*> ProceduralMemory::getAllSkills() const {
    std::vector<const Skill*> result;
    for (const auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

std::vector<Skill*> ProceduralMemory::getSkillsAboveProficiency(float threshold) {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        if (skill.proficiency >= threshold) {
            result.push_back(&skill);
        }
    }
    return result;
}

std::vector<const Skill*> ProceduralMemory::getSkillsAboveProficiency(float threshold) const {
    std::vector<const Skill*> result;
    for (const auto& skill : pImpl->skills) {
        if (skill.proficiency >= threshold) {
            result.push_back(&skill);
        }
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            skill.lastImproved = 0.0f;  // Will be set by caller
            return;
        }
    }
}

void ProceduralMemory::practiceSkill(const std::string& name, float performance) {
    Skill* skill = getSkill(name);
    if (!skill) return;
    
    // Update proficiency based on performance
    skill->proficiency = std::min(1.0f, skill->proficiency + performance * pImpl->defaultLearningRate);
    skill->lastPracticed = 0.0f;  // Will be set by caller
    
    // Add to performance history
    skill->performanceHistory.push_back(performance);
    if (skill->performanceHistory.size() > pImpl->maxHistorySize) {
        skill->performanceHistory.erase(skill->performanceHistory.begin());
    }
    
    // Improve stability based on consistent performance
    if (skill->performanceHistory.size() >= 5) {
        float avgPerformance = 0.0f;
        for (float p : skill->performanceHistory) {
            avgPerformance += p;
        }
        avgPerformance /= skill->performanceHistory.size();
        
        // High, consistent performance increases stability
        if (avgPerformance > 0.8f) {
            skill->stability = std::min(1.0f, skill->stability + 0.1f);
        }
    }
}

float ProceduralMemory::getAverageProficiency() const {
    if (pImpl->skills.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& skill : pImpl->skills) {
        sum += skill.proficiency;
    }
    return sum / pImpl->skills.size();
}

std::vector<Skill*> ProceduralMemory::getBestSkills(size_t count) {
    // Sort skills by proficiency (descending)
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    
    std::sort(result.begin(), result.end(),
        [](Skill* a, Skill* b) { return a->proficiency > b->proficiency; });
    
    // Take top N
    size_t takeCount = std::min(count, result.size());
    return std::vector<Skill*>(result.begin(), result.begin() + takeCount);
}

std::vector<const Skill*> ProceduralMemory::getBestSkills(size_t count) const {
    // Sort skills by proficiency (descending)
    std::vector<const Skill*> result;
    for (const auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    
    std::sort(result.begin(), result.end(),
        [](const Skill* a, const Skill* b) { return a->proficiency > b->proficiency; });
    
    // Take top N
    size_t takeCount = std::min(count, result.size());
    return std::vector<const Skill*>(result.begin(), result.begin() + takeCount);
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    struct Association {
        std::vector<NeuronId> patternA;
        std::vector<NeuronId> patternB;
        float strength;
        float stability;
        float frequency;
        Timestamp lastActivated;
        
        Association(const std::vector<NeuronId>& a, const std::vector<NeuronId>& b) 
            : patternA(a), patternB(b), strength(0.0f), stability(1.0f), frequency(0), lastActivated(0.0f) {}
    };
    
    std::vector<Association> associations;
    size_t maxAssociations;
    float defaultStrength;
    
    // For fast lookup by individual neurons
    std::map<NeuronId, std::vector<size_t>> neuronToAssociationIndices;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {
    pImpl->maxAssociations = 1000;
    pImpl->defaultStrength = 0.5f;
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB, float strength) {
    if (patternA.empty() || patternB.empty()) return;
    
    // Check if similar association already exists
    for (auto& assoc : pImpl->associations) {
        bool similarA = true, similarB = true;
        if (assoc.patternA.size() == patternA.size()) {
            for (size_t i = 0; i < patternA.size(); ++i) {
                if (assoc.patternA[i] != patternA[i]) {
                    similarA = false;
                    break;
                }
            }
        } else {
            similarA = false;
        }
        
        if (assoc.patternB.size() == patternB.size()) {
            for (size_t i = 0; i < patternB.size(); ++i) {
                if (assoc.patternB[i] != patternB[i]) {
                    similarB = false;
                    break;
                }
            }
        } else {
            similarB = false;
        }
        
        if (similarA && similarB) {
            assoc.strength = strength;
            assoc.lastActivated = 0.0f;  // Will be set by caller
            return;
        }
    }
    
    // Check if we have capacity
    if (pImpl->associations.size() >= pImpl->maxAssociations) {
        // Remove weakest association
        size_t weakestIdx = 0;
        for (size_t i = 1; i < pImpl->associations.size(); ++i) {
            if (pImpl->associations[i].strength < pImpl->associations[weakestIdx].strength) {
                weakestIdx = i;
            }
        }
        
        // Remove from neuron lookup
        for (NeuronId neuron : pImpl->associations[weakestIdx].patternA) {
            auto& indices = pImpl->neuronToAssociationIndices[neuron];
            indices.erase(std::remove(indices.begin(), indices.end(), weakestIdx), indices.end());
            if (indices.empty()) {
                pImpl->neuronToAssociationIndices.erase(neuron);
            }
        }
        for (NeuronId neuron : pImpl->associations[weakestIdx].patternB) {
            auto& indices = pImpl->neuronToAssociationIndices[neuron];
            indices.erase(std::remove(indices.begin(), indices.end(), weakestIdx), indices.end());
            if (indices.empty()) {
                pImpl->neuronToAssociationIndices.erase(neuron);
            }
        }
        
        pImpl->associations.erase(pImpl->associations.begin() + weakestIdx);
    }
    
    // Add new association
    Association newAssoc(patternA, patternB);
    newAssoc.strength = strength;
    size_t newIdx = pImpl->associations.size();
    pImpl->associations.push_back(newAssoc);
    
    // Update lookup index
    for (NeuronId neuron : patternA) {
        pImpl->neuronToAssociationIndices[neuron].push_back(newIdx);
    }
    for (NeuronId neuron : patternB) {
        pImpl->neuronToAssociationIndices[neuron].push_back(newIdx);
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(const std::vector<NeuronId>& pattern) const {
    if (pattern.empty()) return {};
    
    std::vector<NeuronId> result;
    
    // First, find exact pattern matches
    for (const auto& assoc : pImpl->associations) {
        if (assoc.patternA == pattern) {
            result.insert(result.end(), assoc.patternB.begin(), assoc.patternB.end());
        } else if (assoc.patternB == pattern) {
            result.insert(result.end(), assoc.patternA.begin(), assoc.patternA.end());
        }
    }
    
    // Then, find partial matches using neurons
    for (NeuronId neuron : pattern) {
        auto it = pImpl->neuronToAssociationIndices.find(neuron);
        if (it != pImpl->neuronToAssociationIndices.end()) {
            for (size_t idx : it->second) {
                const auto& assoc = pImpl->associations[idx];
                
                // Add pattern A if it contains the query neuron and we haven't added it already
                if (std::find(assoc.patternA.begin(), assoc.patternA.end(), neuron) != assoc.patternA.end() &&
                    std::find(result.begin(), result.end(), assoc.patternA[0]) == result.end()) {
                    result.insert(result.end(), assoc.patternA.begin(), assoc.patternA.end());
                }
                
                // Add pattern B if it contains the query neuron and we haven't added it already
                if (std::find(assoc.patternB.begin(), assoc.patternB.end(), neuron) != assoc.patternB.end() &&
                    std::find(result.begin(), result.end(), assoc.patternB[0]) == result.end()) {
                    result.insert(result.end(), assoc.patternB.begin(), assoc.patternB.end());
                }
            }
        }
    }
    
    // Remove duplicates
    std::sort(result.begin(), result.end());
    result.erase(std::unique(result.begin(), result.end()), result.end());
    
    return result;
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    auto it = pImpl->neuronToAssociationIndices.find(neuron);
    if (it != pImpl->neuronToAssociationIndices.end()) {
        for (size_t idx : it->second) {
            const auto& assoc = pImpl->associations[idx];
            
            // Add other neurons from both patterns
            for (NeuronId other : assoc.patternA) {
                if (other != neuron && std::find(result.begin(), result.end(), other) == result.end()) {
                    result.push_back(other);
                }
            }
            for (NeuronId other : assoc.patternB) {
                if (other != neuron && std::find(result.begin(), result.end(), other) == result.end()) {
                    result.push_back(other);
                }
            }
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB) const {
    for (const auto& assoc : pImpl->associations) {
        bool matchA = (assoc.patternA == patternA);
        bool matchB = (assoc.patternB == patternB);
        if ((matchA && matchB) || 
            (matchA && std::find(assoc.patternB.begin(), assoc.patternB.end(), patternB[0]) != assoc.patternB.end()) ||
            (matchB && std::find(assoc.patternA.begin(), assoc.patternA.end(), patternA[0]) != assoc.patternA.end())) {
            return assoc.strength;
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB, float delta) {
    for (auto& assoc : pImpl->associations) {
        bool matchA = (assoc.patternA == patternA);
        bool matchB = (assoc.patternB == patternB);
        if ((matchA && matchB) || 
            (matchA && std::find(assoc.patternB.begin(), assoc.patternB.end(), patternB[0]) != assoc.patternB.end()) ||
            (matchB && std::find(assoc.patternA.begin(), assoc.patternA.end(), patternA[0]) != assoc.patternA.end())) {
            
            assoc.strength = std::clamp(assoc.strength + delta, 0.0f, 1.0f);
            assoc.frequency++;
            return;
        }
    }
}

bool AssociativeMemory::findAssociation(const std::vector<NeuronId>& pattern, std::vector<NeuronId>& matchedPattern) const {
    for (const auto& assoc : pImpl->associations) {
        if (assoc.patternA == pattern) {
            matchedPattern = assoc.patternB;
            return true;
        } else if (assoc.patternB == pattern) {
            matchedPattern = assoc.patternA;
            return true;
        }
    }
    return false;
}

std::vector<Association> AssociativeMemory::getAllAssociations() const {
    return pImpl->associations;
}

void AssociativeMemory::updateFromActivation(const std::vector<NeuronId>& patternA, const std::vector<NeuronId>& patternB) {
    // Find existing association
    for (auto& assoc : pImpl->associations) {
        if ((assoc.patternA == patternA && std::find(assoc.patternB.begin(), assoc.patternB.end(), patternB[0]) != assoc.patternB.end()) ||
            (assoc.patternB == patternB && std::find(assoc.patternA.begin(), assoc.patternA.end(), patternA[0]) != assoc.patternA.end())) {
            
            // Strengthen association based on co-activation
            assoc.strength = std::min(1.0f, assoc.strength + 0.01f);
            assoc.frequency++;
            assoc.lastActivated = 0.0f;  // Will be set by caller
            return;
        }
    }
    
    // Create new association with moderate strength
    associate(patternA, patternB, 0.3f);
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
    pImpl->neuronToAssociationIndices.clear();
}

float AssociativeMemory::getAverageStrength() const {
    if (pImpl->associations.empty()) return 0.0f;
    
    float sum = 0.0f;
    for (const auto& assoc : pImpl->associations) {
        sum += assoc.strength;
    }
    return sum / pImpl->associations.size();
}

std::vector<Association> AssociativeMemory::getStrongestAssociations(size_t count) const {
    // Create vector of pairs (association, strength)
    std::vector<std::pair<Association, float>> assocStrengths;
    assocStrengths.reserve(pImpl->associations.size());
    
    for (const auto& assoc : pImpl->associations) {
        assocStrengths.emplace_back(assoc, assoc.strength);
    }
    
    // Sort by strength (descending)
    std::sort(assocStrengths.begin(), assocStrengths.end(),
        [](const std::pair<Association, float>& a, const std::pair<Association, float>& b) {
            return a.second > b.second;
        });
    
    // Take top N
    size_t takeCount = std::min(count, assocStrengths.size());
    std::vector<Association> result;
    result.reserve(takeCount);
    for (size_t i = 0; i < takeCount; ++i) {
        result.push_back(assocStrengths[i].first);
    }
    
    return result;
}

} // namespace nlm
