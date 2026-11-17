#include "Memory.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <random>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // TODO PHASE 2: Implement real storage with capacity limits
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    if (pImpl->items.size() < pImpl->capacity) {
        pImpl->items.emplace_back(neuron, value);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
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
        item.second *= (1.0f - decayRate);
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
    if (pImpl->episodes.empty()) return;
    
    // Enhanced consolidation mechanism with multiple phases
    
    // 1. Synaptic consolidation: reactivate and strengthen recent memories
    consolidateSynaptic(pImpl->episodes);
    
    // 2. Systems consolidation: transfer important memories to semantic memory
    consolidateSystems(pImpl->episodes, relevanceThreshold);
    
    // 3. Reorganization: integrate related memories into coherent schemas
    reorganizeSchemas(pImpl->episodes);
    
    // 4. Pruning: remove irrelevant or low-quality memories
    pruneUnimportant(pImpl->episodes, relevanceThreshold);
}

void EpisodicMemory::consolidateSynaptic(const std::vector<EpisodicMemoryItem>& episodes) {
    // Synaptic consolidation: reactivate and we
    // This simulates hippocampal-neocortical interactions during sleep
    
    for (const auto& episode : episodes) {
        // Reactivate recent episodes to We
        float reactivationWeight = std::pow(2.0f, -episode.timeSince / 10.0f); // Exponential weighting
        
        // We
        // Actual synaptic changes would modify hippocampal connections
        // For simulation, we update the episode's importance
        episode.relevance *= reactivationWeight;
        episode.quality *= 0.95f; // Natural decay during consolidation
    }
}

void EpisodicMemory::consolidateSystems(const std::vector<EpisodicMemoryItem>& episodes, float relevanceThreshold) {
    // Systems consolidation: transfer high-quality memories to semantic memory
    
    for (const auto& episode : episodes) {
        // Check transfer criteria
        bool shouldTransfer = episode.relevance > relevanceThreshold && episode.quality > 0.8f;
        
        if (shouldTransfer) {
            // Extract semantic features
            std::string concept = extractConceptFromEpisode(episode);
            float conceptStrength = episode.quality * episode.relevance;
            
            // Transfer to semantic memory (would update SemanticMemory)
            // For simulation, we mark episode as transferred
            episode.isSemantic = true;
            
            // Reduce episodic importance after transfer
            episode.relevance *= 0.5f;
            episode.quality *= 0.8f;
        }
    }
}

void EpisodicMemory::reorganizeSchemas(const std::vector<EpisodicMemoryItem>& episodes) {
    // Schema reorganization: integrate related memories into coherent structures
    
    // Group episodes by temporal proximity and content similarity
    std::vector<std::vector<EpisodicMemoryItem>> schemas;
    
    for (const auto& episode : episodes) {
        // Find or create schema for this episode
        size_t schemaIndex = findOrCreateSchema(schemas, episode);
        
        if (schemaIndex < schemas.size()) {
            // Integrate episode into schema
            integrateIntoSchema(schemas[schemaIndex], episode);
        }
    }
    
    // Update schema coherence and strength
    for (auto& schema : schemas) {
        updateSchemaCoherence(schema);
        enhanceSchemaStrength(schema);
    }
}

void EpisodicMemory::pruneUnimportant(const std::vector<EpisodicMemoryItem>& episodes, float relevanceThreshold) {
    // Memory pruning: remove low-quality or irrelevant memories
    
    for (auto it = pImpl->episodes.begin(); it != pImpl->episodes.end();) {
        const auto& episode = *it;
        
        // Prune criteria:
        // 1. Very low relevance
        // 2. Low quality
        // 3. Very old and not well-consolidated
        // 4. Redundant with newer memories
        
        bool shouldRemove = false;
        
        if (episode.relevance < relevanceThreshold * 0.5f) {
            shouldRemove = true; // Very low relevance
        } else if (episode.quality < 0.3f) {
            shouldRemove = true; // Very low quality
        } else if (episode.timeSince > 1000.0f && episode.consolidation < 0.5f) {
            shouldRemove = true; // Old and not consolidated
        } else if (isRedundant(episode)) {
            shouldRemove = true; // Redundant memory
        }
        
        if (shouldRemove) {
            // Remove episode
            it = pImpl->episodes.erase(it);
        } else {
            ++it;
        }
    }
}

float EpisodicMemory::getConsolidationEfficiency() const {
    if (pImpl->episodes.empty()) return 0.0f;
    
    float totalQuality = 0.0f;
    float totalRelevance = 0.0f;
    
    for (const auto& episode : pImpl->episodes) {
        totalQuality += episode.quality;
        totalRelevance += episode.relevance;
    }
    
    float avgQuality = totalQuality / static_cast<float>(pImpl->episodes.size());
    float avgRelevance = totalRelevance / static_cast<float>(pImpl->episodes.size());
    
    return (avgQuality + avgRelevance) * 0.5f;
}

void EpisodicMemory::updateMemoryTrace(EpisodicMemoryItem& episode, TimestepDuration dt) {
    // Update memory trace with simulated replay
    
    // Memory decay
    episode.decayRate = 0.001f + episode.intensity * 0.01f;
    episode.quality *= (1.0f - episode.decayRate * dt);
    episode.relevance *= (1.0f - episode.decayRate * 0.5f * dt);
    
    // Spontaneous replay during consolidation periods
    if (dt > 0.01f) { // During longer simulation steps
        float replayProbability = std::min(1.0f, episode.relevance * 0.1f);
        if ((std::rand() / (RAND_MAX + 1.0f)) < replayProbability) {
            // Reactivate memory
            episode.reactivationCount++;
            episode.consolidation *= 1.05f; // Enhance consolidation
            
            // Replay can strengthen memory
            if (episode.reactivationCount > 3) {
                episode.quality *= 1.02f;
            }
        }
    }
    
    // Consolidation progress
    if (episode.consolidation < 1.0f) {
        episode.consolidation = std::min(1.0f, episode.consolidation + 0.01f * dt);
    }
}

std::string EpisodicMemory::extractConceptFromEpisode(const EpisodicMemoryItem& episode) const {
    // Extract semantic concept from episodic memory
    // This is a simplified version - real implementation would use
    // more sophisticated pattern matching and abstraction
    
    // For demonstration, extract key information based on episode content
    if (episode.content.empty()) {
        return "generic_event";
    }
    
    // Simple heuristic: first word or few words as concept
    size_t spacePos = episode.content.find(' ');
    if (spacePos != std::string::npos) {
        return episode.content.substr(0, spacePos);
    }
    
    return episode.content;
}

size_t EpisodicMemory::findOrCreateSchema(std::vector<std::vector<EpisodicMemoryItem>>& schemas, const EpisodicMemoryItem& episode) const {
    // Find existing schema that matches this episode, or create new one
    
    for (size_t i = 0; i < schemas.size(); ++i) {
        if (schemaMatches(schemas[i], episode)) {
            return i;
        }
    }
    
    // Create new schema
    schemas.push_back(std::vector<EpisodicMemoryItem>{episode});
    return schemas.size() - 1;
}

bool EpisodicMemory::schemaMatches(const std::vector<EpisodicMemoryItem>& schema, const EpisodicMemoryItem& episode) const {
    // Check if episode fits into existing schema
    
    if (schema.empty()) return true;
    
    // Simple matching: compare temporal proximity and content similarity
    const EpisodicMemoryItem& firstEpisode = schema.front();
    
    // Temporal proximity (within 10 time units)
    if (std::abs(episode.timestamp - firstEpisode.timestamp) < 10.0f) {
        return true;
    }
    
    // Check concept matching
    std::string episodeConcept = extractConceptFromEpisode(episode);
    std::string schemaConcept = extractConceptFromEpisode(firstEpisode);
    
    return episodeConcept == schemaConcept;
}

void EpisodicMemory::integrateIntoSchema(std::vector<EpisodicMemoryItem>& schema, const EpisodicMemoryItem& episode) {
    // Integrate episode into schema
    
    // Add to schema
    schema.push_back(episode);
    
    // Limit schema size to prevent overgrowth
    if (schema.size() > 50) {
        // Remove oldest episodes
        schema.erase(schema.begin());
    }
}

void EpisodicMemory::updateSchemaCoherence(std::vector<EpisodicMemoryItem>& schema) {
    // Update schema coherence based on episode relationships
    
    float coherence = 0.0f;
    
    for (size_t i = 1; i < schema.size(); ++i) {
        // Calculate temporal coherence
        float timeDiff = std::abs(schema[i].timestamp - schema[i-1].timestamp);
        float temporalCoherence = std::exp(-timeDiff / 100.0f);
        
        // Calculate content coherence
        std::string concept1 = extractConceptFromEpisode(schema[i-1]);
        std::string concept2 = extractConceptFromEpisode(schema[i]);
        float contentCoherence = (concept1 == concept2) ? 1.0f : 0.5f;
        
        coherence += temporalCoherence * 0.6f + contentCoherence * 0.4f;
    }
    
    if (!schema.empty()) {
        coherence /= static_cast<float>(schema.size() - 1);
    }
    
    // Apply coherence to all episodes in schema
    for (auto& ep : schema) {
        ep.schemaCoherence = coherence;
    }
}

void EpisodicMemory::enhanceSchemaStrength(std::vector<EpisodicMemoryItem>& schema) {
    // Enhance overall schema strength
    
    float schemaStrength = 0.0f;
    for (const auto& episode : schema) {
        schemaStrength += episode.quality * episode.relevance * episode.schemaCoherence;
    }
    
    if (!schema.empty()) {
        schemaStrength /= static_cast<float>(schema.size());
    }
    
    // Apply strength to episodes
    for (auto& episode : schema) {
        episode.schemaStrength = schemaStrength;
        
        // High schema strength can improve individual episode quality
        if (episode.schemaStrength > 0.7f) {
            episode.quality *= 1.01f;
        }
    }
}

bool EpisodicMemory::isRedundant(const EpisodicMemoryItem& episode) const {
    // Check if episode is redundant with newer memories
    
    for (const auto& other : pImpl->episodes) {
        if (&other == &episode) continue; // Skip self
        
        // Check if other episode is newer and similar
        if (other.timestamp > episode.timestamp + 10.0f) { // Newer
            // Check for similarity
            std::string concept1 = extractConceptFromEpisode(episode);
            std::string concept2 = extractConceptFromEpisode(other);
            
            if (concept1 == concept2) {
                // Similar concept, check if it's better consolidated
                if (other.consolidation > episode.consolidation * 1.5f) {
                    return true; // Redundant
                }
            }
        }
    }
    
    return false;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return fact.second;
        }
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    for (const auto& fact : pImpl->facts) {
        if (fact.first == key) {
            return true;
        }
    }
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.neuralPattern = pattern;
            return;
        }
    }
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
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

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    for (auto& skill : pImpl->skills) {
        result.push_back(&skill);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            return;
        }
    }
    pImpl->associations.emplace_back(a, b, strength);
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

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm
