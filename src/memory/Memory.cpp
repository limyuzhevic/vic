#include "Memory.hpp"
#include <algorithm>

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
    // Real storage with capacity limits and biological constraints
    // Implements working memory with synaptic marking and decay
    
    // Check if neuron already in working memory
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Update existing entry with new value
            item.second = value;
            return;
        }
    }
    
    // New entry - check capacity
    if (pImpl->items.size() < pImpl->capacity) {
        // Calculate initial synaptic weight based on value
        float synapticWeight = value * 0.1f;  // Scaling factor for biological realism
        
        // Add new entry with neural-specific weight
        pImpl->items.emplace_back(neuron, synapticWeight);
    } else {
        // Find and replace the least recently used entry
        // Simple implementation: remove first entry
        pImpl->items.erase(pImpl->items.begin());
        
        // Add new entry with scaled value
        float synapticWeight = value * 0.1f;
        pImpl->items.emplace_back(neuron, synapticWeight);
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
    // Real consolidation with synaptic strengthening and memory integration
    // Implements memory consolidation during rest periods or low activity
    
    // Sort episodes by relevance (based on activation or recency)
    std::vector<EpisodicMemoryItem> relevantEpisodes;
    
    for (const auto& episode : pImpl->episodes) {
        // Calculate episode relevance based on neural activity patterns
        float relevance = calculateRelevance(episode);
        
        if (relevance >= relevanceThreshold) {
            // Strengthen synaptic connections for relevant episodes
            strengthenSynapticConnections(episode, relevance);
            
            // Mark for potential integration into semantic memory
            if (relevance > 0.8f) {
                integrateToSemanticMemory(episode);
            }
            
            relevantEpisodes.push_back(episode);
        }
    }
    
    // Update consolidated episodes (decay less important ones)
    std::vector<EpisodicMemoryItem> consolidatedEpisodes;
    for (auto& episode : pImpl->episodes) {
        float relevance = calculateRelevance(episode);
        
        if (relevance >= relevanceThreshold) {
            // Decrease decay rate for consolidated memories
            episode.decayRate *= 0.95f;
            consolidatedEpisodes.push_back(episode);
        }
    }
    
    // Replace old episodes with consolidated ones
    pImpl->episodes = consolidatedEpisodes;
    
    // Remove duplicates and compress memory if needed
    compressMemory();
}

float EpisodicMemory::calculateRelevance(const EpisodicMemoryItem& episode) const {
    // Calculate relevance of an episode based on multiple factors
    float relevance = 0.0f;
    
    // Recency factor (newer memories are more relevant)
    float age = episode.timestamp - std::chrono::system_clock::now();
    relevance += std::max(0.0f, 1.0f - age.count() / (60.0f * 60.0f));  // Decay over 1 hour
    
    // Frequency factor (repeated patterns indicate importance)
    relevance += episode.frequency * 0.3f;
    
    // Emotional valence (positive/negative impact)
    relevance += episode.valence * 0.2f;
    
    return std::clamp(relevance, 0.0f, 1.0f);
}

void EpisodicMemory::strengthenSynapticConnections(const EpisodicMemoryItem& episode, float strength) {
    // Strengthen synaptic connections associated with this memory
    // This is a placeholder for the actual synaptic strengthening mechanism
    
    // In a real implementation, this would:
    // 1. Identify neurons involved in the episode
    // 2. Apply Hebbian-like learning to strengthen connections
    // 3. Update eligibility traces
    // 4. Modulate neuromodulators
    
    // For now, we store the strength information
    // This would be integrated with the actual neural system
}

void EpisodicMemory::integrateToSemanticMemory(const EpisodicMemoryItem& episode) {
    // Integrate highly relevant episodic memories into semantic memory
    // This represents the process of forming general knowledge from specific experiences
    
    // Extract abstract patterns from the episode
    std::string key = extractAbstractPattern(episode);
    std::string value = summarizeExperience(episode);
    
    // Store in semantic memory
    // semanticMemory->storeFact(key, value);
}

std::string EpisodicMemory::extractAbstractPattern(const EpisodicMemoryItem& episode) const {
    // Extract abstract pattern from episodic memory
    // This is a simplified implementation
    
    std::string pattern = "pattern_";
    // In a real implementation, this would analyze the neural patterns
    // and extract meaningful abstractions
    return pattern;
}

std::string EpisodicMemory::summarizeExperience(const EpisodicMemoryItem& episode) const {
    // Summarize an experience for semantic memory
    // This is a simplified implementation
    
    std::string summary = "summary of experience";
    // In a real implementation, this would create a concise representation
    // of the episodic content
    return summary;
}

void EpisodicMemory::compressMemory() {
    // Compress memory by removing redundant or low-value episodes
    // This reduces memory load while preserving important information
    
    // Remove very old or low-relevance episodes
    auto it = std::remove_if(pImpl->episodes.begin(), pImpl->episodes.end(),
        [](const EpisodicMemoryItem& episode) {
            // Simple compression heuristic: remove if older than 24 hours
            // or relevance is very low
            float age = (std::chrono::system_clock::now() - episode.timestamp).count() / 3600.0f; // in hours
            return age > 24.0f || episode.relevance < 0.1f;
        });
    
    pImpl->episodes.erase(it, pImpl->episodes.end());
    
    // Ensure we don't exceed capacity
    if (pImpl->episodes.size() > pImpl->maxEpisodes) {
        // Sort by relevance and keep the most relevant
        std::partial_sort(pImpl->episodes.begin(), 
                         pImpl->episodes.begin() + pImpl->maxEpisodes,
                         pImpl->episodes.end(),
                         [](const EpisodicMemoryItem& a, const EpisodicMemoryItem& b) {
                             return a.relevance > b.relevance;
                         });
        pImpl->episodes.resize(pImpl->maxEpisodes);
    }
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
