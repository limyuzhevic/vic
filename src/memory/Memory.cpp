#include "Memory.hpp"
#include <algorithm>
#include <string>
#include <sstream>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    struct Item {
        NeuronId neuron;
        float value;
        size_t accessCount; // Track access frequency for eviction priority
        
        Item(NeuronId n, float v) : neuron(n), value(v), accessCount(0) {}
    };
    
    std::vector<Item> items;
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
    
    // Find item by neuron ID
    Item* find(NeuronId neuron) {
        for (auto& item : items) {
            if (item.neuron == neuron) {
                item.accessCount++; // Increment access count on access
                return &item;
            }
        }
        return nullptr;
    }
    
    // Remove least accessed item
    void evictLeastAccessed() {
        if (items.empty()) return;
        
        // Find item with minimum access count
        auto minIt = std::min_element(items.begin(), items.end(),
            [](const Item& a, const Item& b) {
                return a.accessCount < b.accessCount;
            });
        
        // Move to end and remove
        size_t index = std::distance(items.begin(), minIt);
        std::swap(items[index], items.back());
        items.pop_back();
    }
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    auto* item = pImpl->find(neuron);
    if (item) {
        item->value = value;
        item->accessCount++; // Update access count
        return;
    }
    
    // If at capacity, evict least accessed item
    if (pImpl->items.size() >= pImpl->capacity) {
        pImpl->evictLeastAccessed();
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
        item.value *= (1.0f - decayRate);
    }
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    std::vector<EpisodicMemoryItem> consolidatedEpisodes; // High-relevance episodes moved here
    size_t maxEpisodes;
    
    Impl(size_t max) : maxEpisodes(max) {}
    
    // Calculate episode relevance based on neurons involved
    float calculateRelevance(const EpisodicMemoryItem& episode) const {
        float relevance = 0.0f;
        // Simple heuristic: more neurons = higher relevance
        // In a real implementation, this would consider metadata and neural activity
        relevance += static_cast<float>(episode.neurons.size()) * 0.1f;
        
        // Consider metadata relevance
        if (!episode.metadata.empty()) {
            // Simple count of important keywords in metadata
            size_t importantWords = std::count(episode.metadata.begin(), episode.metadata.end(), '!') * 0.5f;
            relevance += importantWords;
        }
        
        // Normalize to [0, 1]
        return std::min(relevance, 1.0f);
    }
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Calculate relevance
    float relevance = pImpl->calculateRelevance(episode);
    
    if (relevance >= 0.7f) {
        // High relevance - consolidate to permanent memory
        pImpl->consolidatedEpisodes.push_back(episode);
    } else {
        // Low relevance - store in working episodic memory
        if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
            pImpl->episodes.erase(pImpl->episodes.begin());
        }
        pImpl->episodes.push_back(episode);
        
        // Optionally compress if episode is less relevant
        if (relevance < 0.3f) {
            EpisodicMemoryItem compressed = compressEpisode(episode);
            // Store compressed version for efficiency
            if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
                pImpl->episodes.erase(pImpl->episodes.begin());
            }
            pImpl->episodes.push_back(compressed);
        }
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    if (index < pImpl->episodes.size()) {
        return pImpl->episodes[index];
    } else if (index < pImpl->consolidatedEpisodes.size()) {
        // Try consolidated memory if index out of range in main storage
        return pImpl->consolidatedEpisodes[index - pImpl->episodes.size()];
    }
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    return pImpl->episodes.size() + pImpl->consolidatedEpisodes.size();
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    
    // Get from consolidated first (highest priority)
    size_t consolidatedCount = std::min(count, pImpl->consolidatedEpisodes.size());
    for (size_t i = 0; i < consolidatedCount; ++i) {
        result.push_back(pImpl->consolidatedEpisodes[pImpl->consolidatedEpisodes.size() - 1 - i]);
    }
    
    // Get from recent episodic memory if needed
    size_t remainingCount = count - consolidatedCount;
    size_t start = remainingCount > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - remainingCount;
    for (size_t i = start; i < pImpl->episodes.size(); ++i) {
        result.push_back(pImpl->episodes[i]);
    }
    
    return result;
}

void EpisodicMemory::clear() {
    pImpl->episodes.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Phase 2: Implement episodic memory consolidation
    // Move high-relevance episodes to permanent memory or make them more accessible
    
    // First pass: move episodes above threshold to consolidated storage
    for (const auto& episode : pImpl->episodes) {
        if (pImpl->calculateRelevance(episode) >= relevanceThreshold) {
            pImpl->consolidatedEpisodes.push_back(episode);
        }
    }
    
    // Clear processed episodes from main storage
    pImpl->episodes.clear();
    
    // Apply compression to consolidated episodes to save space
    std::vector<EpisodicMemoryItem> compressedConsolidated;
    for (const auto& episode : pImpl->consolidatedEpisodes) {
        compressedConsolidated.push_back(compressEpisode(episode));
    }
    
    pImpl->consolidatedEpisodes = compressedConsolidated;
}

// Add helper function implementations
EpisodicMemoryItem compressEpisode(const EpisodicMemoryItem& episode) {
    EpisodicMemoryItem compressed = episode;
    
    // Simplify neuron vector (keep only key neurons)
    // For now, just reduce size by half (in real implementation, use smarter selection)
    if (compressed.neurons.size() > 2) {
        compressed.neurons.resize(std::max(size_t(2), compressed.neurons.size() / 2));
    }
    
    if (compressed.values.size() > 2) {
        compressed.values.resize(std::max(size_t(2), compressed.values.size() / 2));
    }
    
    // Compress metadata
    if (!compressed.metadata.empty()) {
        // Remove excessive punctuation and whitespace
        std::string compressedMetadata = compressed.metadata;
        // Simple compression: remove repeated characters and reduce length
        if (compressedMetadata.length() > 100) {
            compressedMetadata = compressedMetadata.substr(0, 100) + "...";
        }
        compressed.metadata = compressedMetadata;
    }
    
    return compressed;
}

void consolidateEpisodicMemory(float relevanceThreshold) {
    // Move high-relevance episodes to consolidated storage
    // This would typically be called during consolidation
    
    // For now, this is a placeholder that can be expanded
    // In a real implementation, this would:
    // 1. Iterate through all episodic memory items
    // 2. Move items with relevance >= threshold to consolidated storage
    // 3. Remove processed items from main storage to save space
    // 4. Update metadata or create summaries for consolidated items
}

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

// Add helper function implementations
EpisodicMemoryItem compressEpisode(const EpisodicMemoryItem& episode) {
    EpisodicMemoryItem compressed = episode;
    
    // Simplify neuron vector (keep only key neurons)
    // For now, just reduce size by half (in real implementation, use smarter selection)
    if (compressed.neurons.size() > 2) {
        compressed.neurons.resize(std::max(size_t(2), compressed.neurons.size() / 2));
    }
    
    if (compressed.values.size() > 2) {
        compressed.values.resize(std::max(size_t(2), compressed.values.size() / 2));
    }
    
    // Compress metadata
    if (!compressed.metadata.empty()) {
        // Remove excessive punctuation and whitespace
        std::string compressedMetadata = compressed.metadata;
        // Simple compression: remove repeated characters and reduce length
        if (compressedMetadata.length() > 100) {
            compressedMetadata = compressedMetadata.substr(0, 100) + "...";
        }
        compressed.metadata = compressedMetadata;
    }
    
    return compressed;
}

void consolidateEpisodicMemory(float relevanceThreshold) {
    // Move high-relevance episodes to consolidated storage
    // This would typically be called during consolidation
    
    // For now, this is a placeholder that can be expanded
    // In a real implementation, this would:
    // 1. Iterate through all episodic memory items
    // 2. Move items with relevance >= threshold to consolidated storage
    // 3. Remove processed items from main storage to save space
    // 4. Update metadata or create summaries for consolidated items
}
