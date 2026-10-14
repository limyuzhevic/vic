#include "Memory.hpp"
#include <algorithm>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    // Memory items with priority and metadata
    struct MemoryItem {
        NeuronId neuronId;
        float value;
        uint64_t timestamp;  // For recency-based eviction
        float priority;      // 0.0-1.0, higher priority = more important
        float novelty;       // Novelty score (0.0-1.0)
        float predictionError; // Prediction error contribution (0.0-1.0)
        bool gateByAttention; // Whether this item is gated by attention
        float decayRate;    // Individual decay rate
        
        MemoryItem() : neuronId(), value(0.0f), timestamp(0), priority(0.5f), 
                      novelty(0.0f), predictionError(0.0f), gateByAttention(false),
                      decayRate(0.1f) {}
    };
    
    std::vector<MemoryItem> items;
    size_t capacity;
    float globalDecayRate;
    uint64_t nextTimestamp;
    
    // Eviction policy: 0 = FIFO, 1 = LIFO, 2 = priority-based, 3 = recency-based
    int evictionPolicy;
    
    Impl(size_t cap) : capacity(cap), globalDecayRate(0.1f), nextTimestamp(0), evictionPolicy(2) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value, float priority, bool gateByAttention) {
    // Check if neuron already exists
    auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
                          [neuron](const Impl::MemoryItem& item) { return item.neuronId == neuron; });
    
    if (it != pImpl->items.end()) {
        // Update existing item
        it->value = value;
        it->priority = priority;
        it->gateByAttention = gateByAttention;
        it->timestamp = pImpl->nextTimestamp++;
        
        // Calculate novelty based on value change (simplified)
        static std::unordered_map<NeuronId, float> lastValues;
        if (lastValues.find(neuron) != lastValues.end()) {
            it->novelty = std::abs(value - lastValues[neuron]) / 10.0f;  // Normalize
        }
        lastValues[neuron] = value;
        
    } else {
        // Create new item with advanced initialization
        Impl::MemoryItem newItem;
        newItem.neuronId = neuron;
        newItem.value = value;
        newItem.priority = priority;
        newItem.gateByAttention = gateByAttention;
        newItem.timestamp = pImpl->nextTimestamp++;
        newItem.novelty = 0.5f;  // Default novelty
        newItem.predictionError = 0.0f;
        newItem.decayRate = pImpl->globalDecayRate + (priority * 0.05f);  // Priority affects decay
        
        pImpl->items.push_back(newItem);
        
        // Apply eviction policy if over capacity
        if (pImpl->items.size() > pImpl->capacity) {
            evictBasedOnPolicy();
        }
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
                          [neuron](const Impl::MemoryItem& item) { return item.neuronId == neuron; });n    
    if (it != pImpl->items.end()) {
        return it->value;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    for (const auto& item : pImpl->items) {
        if (item.neuronId == neuron) {
            return true;
        }
    }
    return false;
}

void WorkingMemory::evictBasedOnPolicy() {
    switch (pImpl->evictionPolicy) {
        case 0: // FIFO - remove oldest
            if (!pImpl->items.empty()) {
                pImpl->items.erase(pImpl->items.begin());
            }
            break;
            
        case 1: // LIFO - remove newest
            if (!pImpl->items.empty()) {
                pImpl->items.pop_back();
            }
            break;
            
        case 2: // Priority-based - remove lowest priority
            std::sort(pImpl->items.begin(), pImpl->items.end(),
                     [](const Impl::MemoryItem& a, const Impl::MemoryItem& b) {
                         return a.priority > b.priority; // Higher priority first
                     });
            if (!pImpl->items.empty()) {
                pImpl->items.pop_back(); // Remove lowest priority
            }
            break;
            
        case 3: // Recency-based - remove least recent
            std::sort(pImpl->items.begin(), pImpl->items.end(),
                     [](const Impl::MemoryItem& a, const Impl::MemoryItem& b) {
                         return a.timestamp > b.timestamp; // Newer first
                     });
            if (!pImpl->items.empty()) {
                pImpl->items.pop_back(); // Remove oldest
            }
            break;
            
        default:
            // Default: remove oldest
            if (!pImpl->items.empty()) {
                pImpl->items.erase(pImpl->items.begin());
            }
            break;
    }
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
    // Update global decay rate
    pImpl->globalDecayRate = decayRate;
    
    // Apply decay to all items
    for (auto& item : pImpl->items) {
        // Individual decay calculation
        float effectiveDecay = item.decayRate + decayRate * 0.5f; // Combined with individual decay
        item.value *= (1.0f - effectiveDecay);
        
        // Cap value to prevent negative values
        if (item.value < 0.0f) item.value = 0.0f;
        if (item.value > 1.0f) item.value = 1.0f;
    }
    
    // Remove very low-value items (cleanup)
    pImpl->items.erase(std::remove_if(pImpl->items.begin(), pImpl->items.end(),
                                    [](const Impl::MemoryItem& item) { return item.value < 0.01f; }),
                     pImpl->items.end());
}

void WorkingMemory::setEvictionPolicy(int policy) {
    pImpl->evictionPolicy = std::clamp(policy, 0, 3);
}

int WorkingMemory::getEvictionPolicy() const {
    return pImpl->evictionPolicy;
}

float WorkingMemory::getTotalMemoryLoad() const {
    float total = 0.0f;
    for (const auto& item : pImpl->items) {
        total += item.value * item.priority;
    }
    return total;
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
    
    // Enhanced consolidation with relevance-based pruning
    if (pImpl->episodes.empty()) {
        return;
    }
    
    NLM_LOG_INFO("Consolidating " + std::to_string(pImpl->episodes.size()) + 
                " episodic memories, relevance threshold: " + std::to_string(relevanceThreshold));
    
    // Calculate relevance scores based on multiple factors
    std::vector<std::pair<float, size_t>> scored;  // (relevanceScore, index)
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        float score = 0.0f;
        
        // Factor 1: Recency (more recent = higher relevance)
        float recencyScore = 1.0f / (pImpl->episodes[i].age + 1);
        score += recencyScore * 0.25f;
        
        // Factor 2: Reward magnitude (higher reward = higher relevance)
        float rewardScore = std::min(pImpl->episodes[i].reward, 1.0f) * 0.20f;
        score += rewardScore;
        
        // Factor 3: Energy (higher energy = more salient)
        float energyScore = std::min(pImpl->episodes[i].energy, 1.0f) * 0.15f;
        score += energyScore;
        
        // Factor 4: Novelty (higher novelty = higher relevance for learning)
        float noveltyScore = std::min(pImpl->episodes[i].novelty, 1.0f) * 0.20f;
        score += noveltyScore;
        
        // Factor 5: Resulting reward (consolidation of successful outcomes)
        float resultingRewardScore = std::min(pImpl->episodes[i].resultingReward, 1.0f) * 0.10f;
        score += resultingRewardScore;
        
        scored.emplace_back(score, i);
    }
    
    // Sort by relevance score (descending)
    std::sort(scored.begin(), scored.end(),
              [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Apply consolidation based on relevance threshold
    size_t keepCount = 0;
    for (const auto& item : scored) {
        if (item.first >= relevanceThreshold) {
            keepCount++;
        } else {
            break;
        }
    }
    
    // Also maintain maximum capacity (keep 80% of capacity for consolidation)
    size_t maxCapacity = pImpl->maxEpisodes * 0.8f;
    if (keepCount > maxCapacity) {
        keepCount = maxCapacity;
    }
    
    // Remove low-relevance episodes
    size_t removed = 0;
    for (size_t i = scored.size(); i > keepCount; --i) {
        // Remove from episodes_ (need to rebuild vector)
        removed++;
    }
    
    // Rebuild episodes_ with only high-relevance episodes
    std::vector<EpisodicMemoryItem> newEpisodes;
    for (size_t i = 0; i < keepCount; ++i) {
        newEpisodes.push_back(pImpl->episodes[scored[i].second]);
    }
    
    pImpl->episodes = std::move(newEpisodes);
    
    NLM_LOG_INFO("Consolidation complete: removed " + std::to_string(removed) + 
                " low-relevance episodic memories, kept " + std::to_string(pImpl->episodes.size()) + 
                " high-relevance memories");
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
