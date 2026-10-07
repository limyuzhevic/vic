#include "Memory.hpp"
#include <algorithm>
#include <unordered_map>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::unordered_map<NeuronId, float> items;  // O(1) lookup instead of O(N)
    size_t capacity;
    
    Impl(size_t cap) : capacity(cap) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // TODO PHASE 2: Implement real storage with capacity limits
    pImpl->items[neuron] = value;  // O(1) update instead of O(N) search
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    auto it = pImpl->items.find(neuron);
    if (it != pImpl->items.end()) {
        return it->second;
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    return pImpl->items.find(neuron) != pImpl->items.end();
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
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::unordered_map<std::string, std::string> facts;  // O(1) lookup instead of O(N)
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    pImpl->facts[key] = value;  // O(1) update instead of O(N) search
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    auto it = pImpl->facts.find(key);
    if (it != pImpl->facts.end()) {
        return it->second;
    }
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->facts.find(key) != pImpl->facts.end();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    result.reserve(pImpl->facts.size());
    for (const auto& pair : pImpl->facts) {
        result.push_back(pair);
    }
    return result;
}

void SemanticMemory::clear() {
    pImpl->facts.clear();
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::unordered_map<std::string, Skill> skills;  // O(1) lookup instead of O(N)
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    pImpl->skills[name] = Skill{name, pattern, 0.0f};  // O(1) update instead of O(N) search
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        return &it->second;
    }
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    result.reserve(pImpl->skills.size());
    for (auto& pair : pImpl->skills) {
        result.push_back(&pair.second);
    }
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    auto it = pImpl->skills.find(name);
    if (it != pImpl->skills.end()) {
        it->second.proficiency = std::clamp(it->second.proficiency + delta, 0.0f, 1.0f);
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    // Use multiple hash maps for O(1) bidirectional lookups
    std::unordered_map<NeuronId, std::vector<NeuronId>> forwardMap;  // neuronId -> list of associated neurons
    std::unordered_map<NeuronId, std::vector<NeuronId>> reverseMap; // neuronId -> list of neurons associated with it
    std::unordered_map<std::pair<NeuronId, NeuronId>, float> strengthMap; // pair(a,b) -> strength
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    // Update both directions
    pImpl->forwardMap[a].push_back(b);
    pImpl->reverseMap[b].push_back(a);
    pImpl->strengthMap[{a, b}] = strength;
    pImpl->strengthMap[{b, a}] = strength;  // Store reverse direction for fast lookup
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    // Get forward associations
    auto itForward = pImpl->forwardMap.find(neuron);
    if (itForward != pImpl->forwardMap.end()) {
        result.insert(result.end(), itForward->second.begin(), itForward->second.end());
    }
    
    // Get reverse associations
    auto itReverse = pImpl->reverseMap.find(neuron);
    if (itReverse != pImpl->reverseMap.end()) {
        result.insert(result.end(), itReverse->second.begin(), itReverse->second.end());
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    auto it = pImpl->strengthMap.find({a, b});
    if (it != pImpl->strengthMap.end()) {
        return it->second;
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    auto it = pImpl->strengthMap.find({a, b});
    if (it != pImpl->strengthMap.end()) {
        it->second = std::clamp(it->second + delta, 0.0f, 1.0f);
        pImpl->strengthMap[{b, a}] = it->second;  // Update reverse direction
    }
}

void AssociativeMemory::clear() {
    pImpl->forwardMap.clear();
    pImpl->reverseMap.clear();
    pImpl->strengthMap.clear();
}

} // namespace nlm
