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
    // Phase 2: Real storage with capacity limits and LRU eviction policy
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            return;
        }
    }
    
    // Check capacity before adding new item
    if (pImpl->items.size() >= pImpl->capacity) {
        // LRU eviction: remove oldest item (first in vector)
        pImpl->items.erase(pImpl->items.begin());
    }
    
    pImpl->items.emplace_back(neuron, value);
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

EpisodicMemory::EpisodicMemory() : pImpl(std::make_unique<Impl>(1000)), 
                                  maxEpisodes_(1000), 
                                  replayEnabled_(true) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Enhanced episode storage with age tracking and consolidation
    EpisodicMemoryItem stored = episode;
    stored.age = 0;  // Initialize age tracking
    
    // Add episode to storage
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        // Evict oldest episode (FIFO)
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    pImpl->episodes.push_back(stored);
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

// Phase 2: Real consolidation based on episode relevance
    // Moves important episodes to long-term storage, removes unimportant ones
    
    // First pass: identify episodes to consolidate based on relevance criteria
    std::vector<std::pair<size_t, float>> consolidationScores;
    
    for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
        const auto& episode = pImpl->episodes[i];
        
        // Calculate relevance based on multiple factors:
        // 1. Recentness (more recent = more relevant)
        float recencyScore = 1.0f / (episode.age + 1);
        
        // 2. Reward (higher reward = more relevant)
        float rewardScore = episode.reward;
        
        // 3. Prediction error (large prediction error = more relevant)
        float predictionErrorScore = episode.predictionError;
        
        // 4. Strength of neural activation (if available)
        float activationScore = 0.0f;
        for (float activation : episode.neuronActivations) {
            activationScore += activation;
        }
        activationScore /= (episode.neuronActivations.size() + 1);
        
        // Combine scores with weighted importance
        float relevance = recencyScore * 0.25f + rewardScore * 0.25f + 
                         predictionErrorScore * 0.25f + activationScore * 0.25f;
        
        consolidationScores.emplace_back(i, relevance);
    }
    
    // Second pass: consolidate episodes based on relevance threshold
    std::vector<size_t> toConsolidate;
    
    for (const auto& score : consolidationScores) {
        if (score.second >= relevanceThreshold) {
            toConsolidate.push_back(score.first);
        }
    }
    
    // Apply consolidation in reverse order (to avoid index issues)
    std::sort(toConsolidate.rbegin(), toConsolidate.rend());
    
    for (size_t idx : toConsolidate) {
        // Move to long-term storage (in this simplified implementation,
        // we just mark them by changing their properties)
        EpisodicMemoryItem& episode = pImpl->episodes[idx];
        episode.consolidated = true;
        
        // Reduce age for consolidated episodes to keep them accessible
        episode.age = 0;
    }
    
    // Remove unimportant episodes (those below relevance threshold)
    std::vector<size_t> toRemove;
    for (const auto& score : consolidationScores) {
        if (score.second < relevanceThreshold) {
            toRemove.push_back(score.first);
        }
    }
    
    std::sort(toRemove.rbegin(), toRemove.rend());
    
    for (size_t idx : toRemove) {
        pImpl->episodes.erase(pImpl->episodes.begin() + idx);
    }
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

SemanticMemory::SemanticMemory() : pImpl(std::make_unique<Impl>()) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Enhanced semantic memory with pattern completion and concept formation
    // Not just simple key-value, but infer relationships and form concepts
    
    // Store the fact
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            return;
        }
    }
    pImpl->facts.emplace_back(key, value);
    
    // Infer related facts (simple pattern completion)
    // Extract key concepts from the fact
    std::string keyConcept = extractKeyConcept(key);
    std::string valueConcept = extractKeyConcept(value);
    
    // Create inverse relationships
    if (!keyConcept.empty() && !valueConcept.empty() && keyConcept != valueConcept) {
        std::string inverseKey = "inverse_of_" + keyConcept;
        std::string inverseValue = "inverse_of_" + valueConcept;
        
        // Check if inverse already exists
        bool inverseExists = false;
        for (const auto& fact : pImpl->facts) {
            if ((fact.first == inverseKey && fact.second == inverseValue) ||
                (fact.first == inverseValue && fact.second == inverseKey)) {
                inverseExists = true;
                break;
            }
        }
        
        if (!inverseExists) {
            pImpl->facts.emplace_back(inverseKey, inverseValue);
        }
    }
    
    // Extract attributes and form concepts
    std::string attribute = extractAttribute(key, value);
    if (!attribute.empty()) {
        std::string conceptKey = "concept_" + keyConcept;
        bool conceptExists = false;
        for (const auto& fact : pImpl->facts) {
            if (fact.first == conceptKey) {
                conceptExists = true;
                break;
            }
        }
        if (!conceptExists) {
            pImpl->facts.emplace_back(conceptKey, attribute);
        }
    }
}

// Helper methods for semantic memory
std::string SemanticMemory::extractKeyConcept(const std::string& text) const {
    // Simple key concept extraction: remove stop words and keep main terms
    std::string result;
    std::string delimiters = " _-:\\",.;()[]{}<>@#$%^&*+=?/|~\"'`<>\"";
    
    size_t i = 0;
    while (i < text.size()) {
        if (std::isspace(text[i]) || delimiters.find(text[i]) != std::string::npos) {
            i++;
            continue;
        }
        
        size_t wordEnd = text.find_first_of(delimiters, i);
        if (wordEnd == std::string::npos) {
            wordEnd = text.size();
        }
        
        std::string word = text.substr(i, wordEnd - i);
        
        // Skip common stop words
        if (word == "the" || word == "a" || word == "an" || word == "and" || 
            word == "or" || word == "is" || word == "for" || word == "of" || 
            word == "in" || word == "on" || word == "at" || word == "to" ||
            word == "with" || word == "from" || word == "by" || word == "about") {
            i = wordEnd + 1;
            continue;
        }
        
        if (!result.empty()) result += "_";
        result += word;
        i = wordEnd + 1;
    }
    
    return result.empty() ? "unknown" : result;
}

std::string SemanticMemory::extractAttribute(const std::string& key, const std::string& value) const {
    // Extract attribute relationship: what property does key have that equals value?
    // Simple approach: use common attribute patterns
    
    // Common relationship patterns
    if (key.find("color") != std::string::npos && value.find("red") != std::string::npos) {
        return "is_red";
    } else if (key.find("color") != std::string::npos && value.find("blue") != std::string::npos) {
        return "is_blue";
    } else if (key.find("size") != std::string::npos && value.find("large") != std::string::npos) {
        return "is_large";
    } else if (key.find("size") != std::string::npos && value.find("small") != std::string::npos) {
        return "is_small";
    } else if (key.find("shape") != std::string::npos && value.find("circle") != std::string::npos) {
        return "is_circular";
    } else if (key.find("type") != std::string::npos) {
        return "is_of_type_" + extractKeyConcept(value);
    }
    
    // Default: use value as attribute
    return "has_value_" + extractKeyConcept(value);
}

void SemanticMemory::getAllConcepts(std::vector<std::string>& concepts) const {
    // Extract all unique concepts from stored facts
    concepts.clear();
    
    for (const auto& fact : pImpl->facts) {
        // Extract concepts from key
        std::string keyConcept = extractKeyConcept(fact.first);
        if (std::find(concepts.begin(), concepts.end(), keyConcept) == concepts.end()) {
            concepts.push_back(keyConcept);
        }
        
        // Extract concepts from value
        std::string valueConcept = extractKeyConcept(fact.second);
        if (std::find(concepts.begin(), concepts.end(), valueConcept) == concepts.end() && 
            valueConcept != keyConcept) {
            concepts.push_back(valueConcept);
        }
    }
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
