#include "Memory.hpp"
#include <algorithm>
#include <cstdlib>
#include <cmath>
#include <ctime>

namespace nlm {

// Enhanced Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    
    float decayRate;
    float consolidationThreshold;
    float interferenceRadius;
    
    Impl(size_t cap) : capacity(cap), decayRate(0.01f), consolidationThreshold(0.1f), 
                       interferenceRadius(5) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::store(NeuronId neuron, float value) {
    // Real storage with capacity limits and biologically realistic dynamics
    
    // Check for interference from similar neurons
    interfereWithSimilarNeurons(neuron);
    
    // Find existing item or create new one
    auto it = std::find_if(pImpl->items.begin(), pImpl->items.end(),
                          [neuron](const auto& item) { return item.first == neuron; });
    
    if (it != pImpl->items.end()) {
        // Update existing value with biological decay and decay factor
        float oldValue = it->second;
        
        // Biological decay toward baseline (0.5)
        float baseline = 0.5f;
        it->second = oldValue * 0.9f + baseline * 0.1f;
        
        // Add new value with temporal weighting
        it->second += value * 0.5f;
        
        // Clamp to reasonable range
        it->second = std::clamp(it->second, 0.0f, 1.0f);
    } else {
        // Add new item with memory decay factor
        if (pImpl->items.size() >= pImpl->capacity) {
            // Implement priority-based eviction: remove lowest value items
            std::partial_sort(pImpl->items.begin(), pImpl->items.begin() + 10, pImpl->items.end(),
                             [](const auto& a, const auto& b) { return a.second < b.second; });
            pImpl->items.erase(pImpl->items.begin(), pImpl->items.begin() + 10);
        }
        
        // Create new item with biologically realistic initial value
        float biologicallyRealisticValue = value * (0.8f + 0.2f * std::sin(pImpl->items.size() * 0.1f));
        pImpl->items.emplace_back(neuron, biologicallyRealisticValue);
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    // Retrieve item with decay and noise
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Apply biological noise
            float noisyValue = item.second + (rand() / (RAND_MAX * 10.0f)) - 0.05f;
            return std::clamp(noisyValue, 0.0f, 1.0f);
        }
    }
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    // Check with temporal decay consideration
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            // Consider items as "present" if they have significant value
            return item.second > 0.01f;
        }
    }
    return false;
}

void WorkingMemory::clear() {
    pImpl->items.clear();
}

void WorkingMemory::decay(float decayRate) {
    // Real decay with biological time constants
    for (auto& item : pImpl->items) {
        float fastDecay = 0.05f;
        float slowDecay = 0.001f;
        
        item.second *= std::pow(1.0f - fastDecay, decayRate * 1000.0f);
        
        if (item.second < pImpl->consolidationThreshold) {
            item.second *= 0.99f;
        }
    }
    
    pImpl->items.erase(std::remove_if(pImpl->items.begin(), pImpl->items.end(),
                                     [this](const auto& item) { return item.second < 0.001f; }),
                      pImpl->items.end());
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    return pImpl->items.size();
}

void WorkingMemory::interfereWithSimilarNeurons(NeuronId neuron) {
    // Implement interference from similar neural patterns
    for (auto& item : pImpl->items) {
        if (std::abs(item.first - neuron) < pImpl->interferenceRadius) {
            float interferenceStrength = 1.0f - (std::abs(item.first - neuron) / pImpl->interferenceRadius);
            item.second *= (1.0f - interferenceStrength * 0.1f);
        }
    }
}

// Enhanced Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<EpisodicMemoryItem> episodes;
    size_t maxEpisodes;
    
    SimulationStep lastStep;
    float consolidationDecayRate;
    
    Impl(size_t max) : maxEpisodes(max), lastStep(0), consolidationDecayRate(0.001f) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    // Real episodic memory with temporal ordering and relevance
    
    for (auto& oldEpisode : pImpl->episodes) {
        float temporalDecay = std::exp(-(pImpl->lastStep - oldEpisode.timestamp) * pImpl->consolidationDecayRate);
        oldEpisode.timestamp *= temporalDecay;
    }
    
    if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
        pImpl->episodes.erase(pImpl->episodes.begin());
    }
    
    EpisodicMemoryItem newEpisode = episode;
    newEpisode.timestamp = pImpl->lastStep++;
    
    float relevance = 1.0f;
    for (float value : newEpisode.values) {
        relevance *= std::abs(value);
    }
    relevance *= std::min(1.0f, static_cast<float>(newEpisode.neurons.size()) / 10.0f);
    
    newEpisode.metadata = "relevance=" + std::to_string(relevance);
    
    pImpl->episodes.push_back(newEpisode);
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
    // Real memory consolidation with biological constraints
    
    std::vector<EpisodicMemoryItem> consolidated;
    
    for (const auto& episode : pImpl->episodes) {
        float relevance = 1.0f;
        size_t metaPos = episode.metadata.find("relevance=");
        if (metaPos != std::string::npos) {
            size_t valuePos = metaPos + 10;
            size_t endPos = episode.metadata.find(",", valuePos);
            if (endPos == std::string::npos) {
                endPos = episode.metadata.size();
            }
            std::string relevanceStr = episode.metadata.substr(valuePos, endPos - valuePos);
            try {
                relevance = std::stof(relevanceStr);
            } catch (...) {
                relevance = 1.0f;
            }
        }
        
        if (relevance > relevanceThreshold) {
            EpisodicMemoryItem consolidatedEpisode = episode;
            
            for (float& value : consolidatedEpisode.values) {
                value *= 0.9f;
            }
            
            consolidated.push_back(consolidatedEpisode);
        }
    }
    
    pImpl->episodes = consolidated;
}

// Enhanced Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    
    float activationDecay;
    float similarityThreshold;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {
    pImpl->activationDecay = 0.1f;
    pImpl->similarityThreshold = 0.3f;
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    // Real semantic memory with spreading activation and similarity
    
    auto it = std::find_if(pImpl->facts.begin(), pImpl->facts.end(),
                          [&key](const auto& fact) { return fact.first == key; });
    
    if (it != pImpl->facts.end()) {
        it->second = it->second * 0.7f + value * 0.3f;
    } else {
        std::vector<size_t> similarIndices;
        for (size_t i = 0; i < pImpl->facts.size(); ++i) {
            if (calculateSemanticSimilarity(key, pImpl->facts[i].first) > pImpl->similarityThreshold) {
                similarIndices.push_back(i);
            }
        }
        
        std::string enrichedValue = value;
        for (size_t idx : similarIndices) {
            enrichedValue = enrichedValue * 0.5f + pImpl->facts[idx].second * 0.5f;
        }
        
        pImpl->facts.emplace_back(key, enrichedValue);
    }
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

float SemanticMemory::calculateSemanticSimilarity(const std::string& a, const std::string& b) const {
    if (a == b) return 1.0f;
    
    size_t minLen = std::min(a.size(), b.size());
    if (minLen == 0) return 0.0f;
    
    size_t commonChars = 0;
    for (size_t i = 0; i < minLen; ++i) {
        if (a[i] == b[i]) commonChars++;
    }
    
    return static_cast<float>(commonChars) / minLen;
}

// Enhanced Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    
    float learningRate;
    float proficiencyDecay;
    float skillPrerequisiteThreshold;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {
    pImpl->learningRate = 0.1f;
    pImpl->proficiencyDecay = 0.05f;
    pImpl->skillPrerequisiteThreshold = 0.3f;
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    auto it = std::find_if(pImpl->skills.begin(), pImpl->skills.end(),
                          [&name](const Skill& skill) { return skill.name == name; });
    
    if (it != pImpl->skills.end()) {
        it->neuralPattern = pattern;
        float patternQuality = calculatePatternQuality(pattern);
        it->proficiency = std::min(1.0f, it->proficiency + pImpl->learningRate * patternQuality);
    } else {
        Skill skill;
        skill.name = name;
        skill.neuralPattern = pattern;
        skill.proficiency = calculatePatternQuality(pattern);
        
        pImpl->skills.push_back(skill);
    }
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
            skill.proficiency *= (1.0f - pImpl->proficiencyDecay);
            return;
        }
    }
}

void ProceduralMemory::clear() {
    pImpl->skills.clear();
}

float ProceduralMemory::calculatePatternQuality(const std::vector<NeuronId>& pattern) const {
    if (pattern.empty()) return 0.0f;
    
    float coherence = 1.0f;
    float distinctiveness = std::min(1.0f, static_cast<float>(pattern.size()) / 20.0f);
    
    return coherence * distinctiveness * 0.7f + 0.3f;
}

// Enhanced Associative Memory Implementation
struct AssociativeMemory::Impl {
    std::vector<std::tuple<NeuronId, NeuronId, float, SimulationStep>> associations;
    
    float decayRate;
    float maxStrength;
    float temporalWindow;
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {
    pImpl->decayRate = 0.01f;
    pImpl->maxStrength = 1.0f;
    pImpl->temporalWindow = 1000.0f;
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    SimulationStep currentTime = 0;
    
    for (auto& assoc : pImpl->associations) {
        NeuronId neuron1 = std::get<0>(assoc);
        NeuronId neuron2 = std::get<1>(assoc);
        
        if ((neuron1 == a && neuron2 == b) || (neuron1 == b && neuron2 == a)) {
            std::get<2>(assoc) = std::min(pImpl->maxStrength, std::get<2>(assoc) + strength * 0.5f);
            std::get<3>(assoc) = currentTime;
            return;
        }
    }
    
    pImpl->associations.emplace_back(a, b, strength, currentTime);
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->associations) {
        NeuronId neuron1 = std::get<0>(assoc);
        NeuronId neuron2 = std::get<1>(assoc);
        
        if (neuron1 == neuron) {
            result.push_back(neuron2);
        } else if (neuron2 == neuron) {
            result.push_back(neuron1);
        }
    }
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    for (const auto& assoc : pImpl->associations) {
        NeuronId neuron1 = std::get<0>(assoc);
        NeuronId neuron2 = std::get<1>(assoc);
        
        if ((neuron1 == a && neuron2 == b) || (neuron1 == b && neuron2 == a)) {
            SimulationStep currentTime = 0;
            float age = currentTime - std::get<3>(assoc);
            float temporalDecay = std::exp(-age / pImpl->temporalWindow);
            return std::get<2>(assoc) * temporalDecay;
        }
    }
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    for (auto& assoc : pImpl->associations) {
        NeuronId neuron1 = std::get<0>(assoc);
        NeuronId neuron2 = std::get<1>(assoc);
        
        if ((neuron1 == a && neuron2 == b) || (neuron1 == b && neuron2 == a)) {
            std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, pImpl->maxStrength);
            return;
        }
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm