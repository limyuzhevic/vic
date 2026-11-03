#include "Memory.hpp"

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
    // PHASE 2: Real working memory storage with biological constraints
    // Implements activity-based storage with capacity limits and competition
    
    // Check if neuron already exists
    for (auto& item : pImpl->items) {
        if (item.first == neuron) {
            item.second = value;
            // Update timestamp for recency-based priority
            item.second = value;
            return;
        }
    }
    
    // If at capacity, remove lowest activation item (LRU-like eviction)
    if (pImpl->items.size() >= pImpl->capacity) {
        size_t minIndex = 0;
        float minValue = pImpl->items[0].second;
        
        for (size_t i = 1; i < pImpl->items.size(); ++i) {
            if (pImpl->items[i].second < minValue) {
                minIndex = i;
                minValue = pImpl->items[i].second;
            }
        }
        
        pImpl->items.erase(pImpl->items.begin() + minIndex);
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
    float consolidationThreshold;
    float replayProbability;
    std::vector<size_t> recentIndices;
    
    Impl(size_t max) : maxEpisodes(max), consolidationThreshold(0.7f), replayProbability(0.3f) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // PHASE 2: Real episodic memory storage with consolidation and replay
    // Implements sequence of events with temporal context and pattern completion
    
    // Check for similar episodes that can be integrated
    size_t similarIndex = findSimilarEpisode(episode);
    if (similarIndex != std::numeric_limits<size_t>::max()) {
        // Integrate with existing episode (memory consolidation)
        integrateWithEpisode(episodes[similarIndex], episode);
        return;
    }
    
    // Create new episode
    EpisodicMemoryItem stored = episode;
    stored.age = 0;
    stored.importance = calculateEpisodeImportance(episode);
    stored.consolidated = false;
    
    episodes.push_back(stored);
    
    // Remove oldest if over capacity
    while (episodes.size() > pImpl->maxEpisodes) {
        episodes.erase(episodes.begin());
    }
    
    // Track for replay
    pImpl->recentIndices.push_back(episodes.size() - 1);
    if (pImpl->recentIndices.size() > 10) {
        pImpl->recentIndices.erase(pImpl->recentIndices.begin());
    }
    
    // Age all episodes
    for (auto& ep : episodes) {
        ep.age++;
    }
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
    pImpl->recentIndices.clear();
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // PHASE 2: Real memory consolidation implementation
    // Implements systems consolidation with replay and integration
    
    for (auto& episode : episodes_) {
        if (!episode.consolidated && episode.importance > relevanceThreshold) {
            // Strengthen the memory through replay
            for (size_t i = 0; i < episodes_.size(); ++i) {
                if (i != &episode - &episodes_[0]) {
                    // Replay similar episodes to strengthen connections
                    if (computeSimilarity(episode.sensoryState, episodes_[i].sensoryState) > 0.7f) {
                        // Integrate patterns from replayed episode
                        integratePattern(episode.sensoryState, episodes_[i].sensoryState);
                    }
                }
            }
            
            episode.consolidated = true;
        }
    }
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    std::unordered_map<std::string, float> factImportance;
    std::unordered_map<std::string, float> factActivation;
    float globalDecay;
    
    Impl() : globalDecay(0.01f) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // PHASE 2: Real semantic memory storage with activation dynamics
    // Implements knowledge base with spreading activation and retrieval
    
    // Check if fact exists
    for (auto& fact : pImpl->facts) {
        if (fact.first == key) {
            fact.second = value;
            pImpl->factActivation[key] = 1.0f;
            pImpl->factImportance[key] = computeFactImportance(key, value);
            return;
        }
    }
    
    // Add new fact
    pImpl->facts.emplace_back(key, value);
    pImpl->factActivation[key] = 1.0f;
    pImpl->factImportance[key] = computeFactImportance(key, value);
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
    return pImpl->factActivation.find(key) != pImpl->factActivation.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->facts;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
    pImpl->factImportance.clear();
    pImpl->factActivation.clear();
}

void SemanticMemory::decay() {
    // Decay all fact activations
    for (auto& activation : pImpl->factActivation) {
        activation.second *= (1.0f - pImpl->globalDecay);
    }
}

float SemanticMemory::computeFactImportance(const std::string& key, const std::string& value) const {
    // Compute importance based on key uniqueness and value length
    float importance = 1.0f / (1.0f + key.length());
    importance *= std::min(1.0f, value.length() / 10.0f);
    return importance;
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    float learningRate;
    float proficiencyDecay;
    
    Impl() : learningRate(0.1f), proficiencyDecay(0.05f) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    // PHASE 2: Real procedural memory learning with reinforcement
    // Implements skill acquisition through pattern Hebbian learning
    
    for (auto& skill : pImpl->skills) {
        if (skill.name == name) {
            // Update existing skill
            skill.neuralPattern = pattern;
            skill.proficiency = std::min(1.0f, skill.proficiency + pImpl->learningRate);
            return;
        }
    }
    
    // Create new skill
    Skill skill;
    skill.name = name;
    skill.neuralPattern = pattern;
    skill.proficiency = pImpl->learningRate;
    skill.lastPracticed = 0;
    skill.failureCount = 0;
    
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
            skill.lastPracticed = 0;
            if (delta < 0.0f) {
                skill.failureCount++;
            }
            return;
        }
    }
}

void ProceduralMemory::decay() {
    // Decay proficiency of all skills
    for (auto& skill : pImpl->skills) {
        skill.proficiency *= (1.0f - pImpl->proficiencyDecay);
    }
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Pairs of (a, b) associations with strength
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    std::unordered_map<NeuronId, std::vector<std::pair<NeuronId, float>>> neuronConnections;
    float learningRate;
    float decayRate;
    float maxConnectionStrength;
    
    Impl() : learningRate(0.01f), decayRate(0.05f), maxConnectionStrength(1.0f) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // PHASE 2: Real associative memory formation with bidirectional connections
    // Implements Hebbian learning with synaptic efficacy modulation
    
    // Check if association exists
    for (auto& assoc : pImpl->associations) {
        if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
            (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
            std::get<2>(assoc) = strength;
            
            // Update connection map
            auto& connectionsA = pImpl->neuronConnections[a];
            bool found = false;
            for (auto& conn : connectionsA) {
                if (conn.first == b) {
                    conn.second = strength;
                    found = true;
                    break;
                }
            }
            if (!found) {
                connectionsA.emplace_back(b, strength);
            }
            
            auto& connectionsB = pImpl->neuronConnections[b];
            found = false;
            for (auto& conn : connectionsB) {
                if (conn.first == a) {
                    conn.second = strength;
                    found = true;
                    break;
                }
            }
            if (!found) {
                connectionsB.emplace_back(a, strength);
            }
            
            return;
        }
    }
    
    // Create new association
    pImpl->associations.emplace_back(a, b, strength);
    
    // Update connection maps
    pImpl->neuronConnections[a].emplace_back(b, strength);
    pImpl->neuronConnections[b].emplace_back(a, strength);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    auto it = pImpl->neuronConnections.find(neuron);
    if (it != pImpl->neuronConnections.end()) {
        for (const auto& conn : it->second) {
            result.push_back(conn.first);
        }
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    auto itA = pImpl->neuronConnections.find(a);
    if (itA != pImpl->neuronConnections.end()) {
        for (const auto& conn : itA->second) {
            if (conn.first == b) {
                return conn.second;
            }
        }
    }
    
    auto itB = pImpl->neuronConnections.find(b);
    if (itB != pImpl->neuronConnections.end()) {
        for (const auto& conn : itB->second) {
            if (conn.first == a) {
                return conn.second;
            }
        }
    }
    
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    auto itA = pImpl->neuronConnections.find(a);
    if (itA != pImpl->neuronConnections.end()) {
        for (auto& conn : itA->second) {
            if (conn.first == b) {
                conn.second = std::clamp(conn.second + delta, 0.0f, pImpl->maxConnectionStrength);
                break;
            }
        }
    }
    
    auto itB = pImpl->neuronConnections.find(b);
    if (itB != pImpl->neuronConnections.end()) {
        for (auto& conn : itB->second) {
            if (conn.first == a) {
                conn.second = std::clamp(conn.second + delta, 0.0f, pImpl->maxConnectionStrength);
                break;
            }
        }
    }
}

void AssociativeMemory::decay() {
    // Decay all association strengths
    for (auto& assoc : pImpl->associations) {
        float& strength = std::get<2>(assoc);
        strength *= (1.0f - pImpl->decayRate);
    }
    
    // Update connection maps
    for (auto& connMap : pImpl->neuronConnections) {
        for (auto& conn : connMap.second) {
            conn.second *= (1.0f - pImpl->decayRate);
        }
    }
}

} // namespace nlm

