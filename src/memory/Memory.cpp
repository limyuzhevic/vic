#include "Memory.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <unordered_set>

namespace nlm {

// Working Memory Implementation
struct WorkingMemory::Impl {
    enum class ImplementationMode currentMode;
    Brain* brainReference;
    
    // Simple implementation
    std::vector<std::pair<NeuronId, float>> simpleItems;
    size_t simpleCapacity;
    
    // Hybrid implementation
    std::unordered_map<uint64_t, float> hybridItems;
    std::unordered_set<uint64_t> hybridNeurons;
    
    // Statistics
    SimulationStep lastUpdateTime;
    float totalActivation;
    
    Impl() : currentMode(ImplementationMode::Simple), brainReference(nullptr),
             simpleCapacity(100), lastUpdateTime(0), totalActivation(0.0f) {}
};

WorkingMemory::WorkingMemory() : pImpl(std::make_unique<Impl>()) {}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::setImplementationMode(ImplementationMode mode) {
    pImpl->currentMode = mode;
    NLM_LOG_INFO("WorkingMemory implementation mode set to: " +
                (mode == ImplementationMode::Simple ? "Simple" :
                 mode == ImplementationMode::Neural ? "Neural" : "Hybrid"));
}

WorkingMemory::ImplementationMode WorkingMemory::getImplementationMode() const {
    return pImpl->currentMode;
}

void WorkingMemory::store(NeuronId neuron, float value) {
    uint64_t neuronKey = neuron.value;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            // Simple key-value storage with capacity limits
            for (auto& item : pImpl->simpleItems) {
                if (item.first == neuron) {
                    item.second = value;
                    return;
                }
            }
            if (pImpl->simpleItems.size() < pImpl->simpleCapacity) {
                pImpl->simpleItems.emplace_back(neuron, value);
            } else {
                // Replace least recently used item
                if (!pImpl->simpleItems.empty()) {
                    pImpl->simpleItems.erase(pImpl->simpleItems.begin());
                    pImpl->simpleItems.emplace_back(neuron, value);
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            // Store in simple system and update neural representation
            pImpl->hybridItems[neuronKey] = value;
            pImpl->hybridNeurons.insert(neuronKey);
            pImpl->totalActivation += value;
            
            // If we have brain reference and neural mode is available, update it
            if (pImpl->brainReference && value > 0.1f) {
                // Store to neural working memory if available
                pImpl->brainReference->storeToWorkingMemory(neuron, value);
            }
            break;
            
        case ImplementationMode::Neural:
            // Directly use neural system
            if (pImpl->brainReference) {
                pImpl->brainReference->storeToWorkingMemory(neuron, value);
            } else {
                // Fallback to simple storage
                pImpl->simpleItems.emplace_back(neuron, value);
            }
            break;
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    uint64_t neuronKey = neuron.value;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (const auto& item : pImpl->simpleItems) {
                if (item.first == neuron) {
                    return item.second;
                }
            }
            return 0.0f;
            
        case ImplementationMode::Hybrid:
            auto it = pImpl->hybridItems.find(neuronKey);
            if (it != pImpl->hybridItems.end()) {
                return it->second;
            }
            // Also check neural representation
            if (pImpl->brainReference) {
                return pImpl->brainReference->getWorkingMemoryActivation(neuron);
            }
            return 0.0f;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                return pImpl->brainReference->getWorkingMemoryActivation(neuron);
            }
            return 0.0f;
    }
    
    return 0.0f;
}

bool WorkingMemory::contains(NeuronId neuron) const {
    uint64_t neuronKey = neuron.value;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (const auto& item : pImpl->simpleItems) {
                if (item.first == neuron) {
                    return true;
                }
            }
            return false;
            
        case ImplementationMode::Hybrid:
            return pImpl->hybridNeurons.find(neuronKey) != pImpl->hybridNeurons.end();
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                return pImpl->brainReference->isWorkingMemoryActive(neuron);
            }
            return false;
    }
    
    return false;
}

void WorkingMemory::clear() {
    pImpl->simpleItems.clear();
    pImpl->hybridItems.clear();
    pImpl->hybridNeurons.clear();
    pImpl->totalActivation = 0.0f;
    
    if (pImpl->brainReference) {
        pImpl->brainReference->clearWorkingMemory();
    }
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->simpleCapacity;
}

size_t WorkingMemory::getCurrentSize() const {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            return pImpl->simpleItems.size();
        case ImplementationMode::Hybrid:
            return pImpl->hybridItems.size();
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                return pImpl->brainReference->getWorkingMemorySize();
            }
            return 0;
    }
    return 0;
}

void WorkingMemory::decay(float decayRate) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& item : pImpl->simpleItems) {
                item.second *= (1.0f - decayRate);
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& item : pImpl->hybridItems) {
                item.second *= (1.0f - decayRate);
            }
            pImpl->totalActivation *= (1.0f - decayRate);
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->decayWorkingMemory(decayRate);
            }
            break;
    }
}

void WorkingMemory::initializeNeuralWorkingMemory(Brain* brain) {
    if (brain) {
        pImpl->brainReference = brain;
        brain->initializeWorkingMemory();
        NLM_LOG_INFO("WorkingMemory neural system initialized");
    }
}

void WorkingMemory::updateNeuralWorkingMemory(TimestepDuration dt) {
    if (pImpl->brainReference) {
        pImpl->brainReference->updateWorkingMemory(dt);
    }
}

void WorkingMemory::setBrainReference(Brain* brain) {
    pImpl->brainReference = brain;
}

bool WorkingMemory::hasBrainReference() const {
    return pImpl->brainReference != nullptr;
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    enum class ImplementationMode currentMode;
    Brain* brainReference;
    
    // Simple implementation
    std::vector<EpisodicMemoryItem> simpleEpisodes;
    size_t simpleMaxEpisodes;
    
    // Neural implementation
    std::vector<EpisodicMemoryItem> neuralEpisodes;
    
    // Statistics
    SimulationStep lastUpdateTime;
    
    Impl() : currentMode(ImplementationMode::Simple), brainReference(nullptr),
             simpleMaxEpisodes(1000), lastUpdateTime(0) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(std::make_unique<Impl>()) {}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::setImplementationMode(ImplementationMode mode) {
    pImpl->currentMode = mode;
    NLM_LOG_INFO("EpisodicMemory implementation mode set to: " +
                (mode == ImplementationMode::Simple ? "Simple" :
                 mode == ImplementationMode::Neural ? "Neural" : "Hybrid"));
}

EpisodicMemory::ImplementationMode EpisodicMemory::getImplementationMode() const {
    return pImpl->currentMode;
}

void EpisodicMemory::storeEpisode(const EpisodicMemoryItem& episode) {
    SimulationStep currentTime = episode.timestamp;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            if (pImpl->simpleEpisodes.size() >= pImpl->simpleMaxEpisodes) {
                pImpl->simpleEpisodes.erase(pImpl->simpleEpisodes.begin());
            }
            pImpl->simpleEpisodes.push_back(episode);
            break;
            
        case ImplementationMode::Hybrid:
            pImpl->simpleEpisodes.push_back(episode);
            
            // Also store to neural system
            if (pImpl->brainReference) {
                pImpl->brainReference->storeEpisodicMemory(episode);
                pImpl->neuralEpisodes.push_back(episode);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->storeEpisodicMemory(episode);
            }
            pImpl->neuralEpisodes.push_back(episode);
            break;
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            if (index < pImpl->simpleEpisodes.size()) {
                return pImpl->simpleEpisodes[index];
            }
            break;
            
        case ImplementationMode::Hybrid:
            if (index < pImpl->simpleEpisodes.size()) {
                return pImpl->simpleEpisodes[index];
            }
            break;
            
        case ImplementationMode::Neural:
            if (index < pImpl->neuralEpisodes.size()) {
                return pImpl->neuralEpisodes[index];
            }
            break;
    }
    
    return EpisodicMemoryItem();
}

size_t EpisodicMemory::getEpisodeCount() const {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            return pImpl->simpleEpisodes.size();
        case ImplementationMode::Hybrid:
            return pImpl->simpleEpisodes.size() + pImpl->neuralEpisodes.size();
        case ImplementationMode::Neural:
            return pImpl->neuralEpisodes.size();
    }
    return 0;
}

std::vector<EpisodicMemoryItem> EpisodicMemory::getRecentEpisodes(size_t count) const {
    std::vector<EpisodicMemoryItem> result;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            size_t start = count > pImpl->simpleEpisodes.size() ? 0 : 
                          pImpl->simpleEpisodes.size() - count;
            for (size_t i = start; i < pImpl->simpleEpisodes.size(); ++i) {
                result.push_back(pImpl->simpleEpisodes[i]);
            }
            break;
            
        case ImplementationMode::Hybrid:
            size_t simpleStart = count > pImpl->simpleEpisodes.size() ? 0 :
                                pImpl->simpleEpisodes.size() - count;
            for (size_t i = simpleStart; i < pImpl->simpleEpisodes.size(); ++i) {
                result.push_back(pImpl->simpleEpisodes[i]);
            }
            if (result.size() < count && pImpl->neuralEpisodes.size() > 0) {
                size_t neuralCount = std::min(count - result.size(), pImpl->neuralEpisodes.size());
                size_t neuralStart = pImpl->neuralEpisodes.size() - neuralCount;
                for (size_t i = neuralStart; i < pImpl->neuralEpisodes.size(); ++i) {
                    result.push_back(pImpl->neuralEpisodes[i]);
                }
            }
            break;
            
        case ImplementationMode::Neural:
            size_t start = count > pImpl->neuralEpisodes.size() ? 0 :
                          pImpl->neuralEpisodes.size() - count;
            for (size_t i = start; i < pImpl->neuralEpisodes.size(); ++i) {
                result.push_back(pImpl->neuralEpisodes[i]);
            }
            break;
    }
    
    return result;
}

void EpisodicMemory::clear() {
    pImpl->simpleEpisodes.clear();
    pImpl->neuralEpisodes.clear();
    
    if (pImpl->brainReference) {
        pImpl->brainReference->clearEpisodicMemory();
    }
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            // Move high-relevance episodes to neural system
            std::vector<EpisodicMemoryItem> toMove;
            for (const auto& episode : pImpl->simpleEpisodes) {
                // Calculate relevance based on neural activity
                float relevance = 0.0f;
                for (float value : episode.values) {
                    relevance += std::abs(value);
                }
                relevance /= std::max(1.0f, static_cast<float>(episode.values.size()));
                
                if (relevance > relevanceThreshold) {
                    toMove.push_back(episode);
                }
            }
            
            for (const auto& episode : toMove) {
                // Remove from simple system
                pImpl->simpleEpisodes.erase(
                    std::remove(pImpl->simpleEpisodes.begin(), pImpl->simpleEpisodes.end(), episode),
                    pImpl->simpleEpisodes.end()
                );
                
                // Add to neural system
                if (pImpl->brainReference) {
                    pImpl->brainReference->storeEpisodicMemory(episode);
                }
                pImpl->neuralEpisodes.push_back(episode);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->consolidateEpisodicMemory(relevanceThreshold);
            }
            break;
            
        case ImplementationMode::Hybrid:
            // First check simple system
            consolidate(relevanceThreshold);
            break;
    }
}

void EpisodicMemory::initializeNeuralEpisodicMemory(Brain* brain) {
    if (brain) {
        pImpl->brainReference = brain;
        brain->initializeEpisodicMemory();
        NLM_LOG_INFO("EpisodicMemory neural system initialized");
    }
}

void EpisodicMemory::updateNeuralEpisodicMemory() {
    if (pImpl->brainReference) {
        pImpl->brainReference->updateEpisodicMemory();
    }
}

void EpisodicMemory::setBrainReference(Brain* brain) {
    pImpl->brainReference = brain;
}

bool EpisodicMemory::hasBrainReference() const {
    return pImpl->brainReference != nullptr;
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    enum class ImplementationMode currentMode;
    Brain* brainReference;
    
    // Simple implementation
    std::vector<std::pair<std::string, std::string>> simpleFacts;
    
    // Neural implementation
    std::vector<std::pair<std::string, std::string>> neuralFacts;
    
    // Statistics
    SimulationStep lastUpdateTime;
    
    Impl() : currentMode(ImplementationMode::Simple), brainReference(nullptr),
             lastUpdateTime(0) {}
};

SemanticMemory::SemanticMemory() : pImpl(std::make_unique<Impl>()) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::setImplementationMode(ImplementationMode mode) {
    pImpl->currentMode = mode;
    NLM_LOG_INFO("SemanticMemory implementation mode set to: " +
                (mode == ImplementationMode::Simple ? "Simple" :
                 mode == ImplementationMode::Neural ? "Neural" : "Hybrid"));
}

SemanticMemory::ImplementationMode SemanticMemory::getImplementationMode() const {
    return pImpl->currentMode;
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& fact : pImpl->simpleFacts) {
                if (fact.first == key) {
                    fact.second = value;
                    return;
                }
            }
            pImpl->simpleFacts.emplace_back(key, value);
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& fact : pImpl->simpleFacts) {
                if (fact.first == key) {
                    fact.second = value;
                    return;
                }
            }
            pImpl->simpleFacts.emplace_back(key, value);
            
            // Update neural system
            if (pImpl->brainReference) {
                pImpl->brainReference->storeSemanticFact(key, value);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->storeSemanticFact(key, value);
            }
            break;
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (const auto& fact : pImpl->simpleFacts) {
                if (fact.first == key) {
                    return fact.second;
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (const auto& fact : pImpl->simpleFacts) {
                if (fact.first == key) {
                    return fact.second;
                }
            }
            if (pImpl->brainReference) {
                std::string neuralValue = pImpl->brainReference->retrieveSemanticFact(key);
                if (!neuralValue.empty()) {
                    return neuralValue;
                }
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                return pImpl->brainReference->retrieveSemanticFact(key);
            }
            break;
    }
    
    return "";
}

bool SemanticMemory::hasFact(const std::string& key) const {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (const auto& fact : pImpl->simpleFacts) {
                if (fact.first == key) {
                    return true;
                }
            }
            return false;
            
        case ImplementationMode::Hybrid:
            if (pImpl->simpleFacts.size() > 0) {
                for (const auto& fact : pImpl->simpleFacts) {
                    if (fact.first == key) {
                        return true;
                    }
                }
            }
            if (pImpl->brainReference) {
                return pImpl->brainReference->hasSemanticFact(key);
            }
            return false;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                return pImpl->brainReference->hasSemanticFact(key);
            }
            return false;
    }
    
    return false;
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            return pImpl->simpleFacts;
            
        case ImplementationMode::Hybrid:
            result = pImpl->simpleFacts;
            if (pImpl->brainReference) {
                // Add neural facts
                std::vector<std::string> neuralKeys = pImpl->brainReference->getAllSemanticKeys();
                for (const auto& key : neuralKeys) {
                    std::string value = pImpl->brainReference->retrieveSemanticFact(key);
                    if (!value.empty()) {
                        result.emplace_back(key, value);
                    }
                }
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                std::vector<std::string> neuralKeys = pImpl->brainReference->getAllSemanticKeys();
                for (const auto& key : neuralKeys) {
                    std::string value = pImpl->brainReference->retrieveSemanticFact(key);
                    if (!value.empty()) {
                        result.emplace_back(key, value);
                    }
                }
            }
            break;
    }
    
    return result;
}

void SemanticMemory::clear() {
    pImpl->simpleFacts.clear();
    pImpl->neuralFacts.clear();
    
    if (pImpl->brainReference) {
        pImpl->brainReference->clearSemanticMemory();
    }
}

void SemanticMemory::initializeNeuralSemanticMemory(Brain* brain) {
    if (brain) {
        pImpl->brainReference = brain;
        brain->initializeSemanticMemory();
        NLM_LOG_INFO("SemanticMemory neural system initialized");
    }
}

void SemanticMemory::updateNeuralSemanticMemory(TimestepDuration dt) {
    if (pImpl->brainReference) {
        pImpl->brainReference->updateSemanticMemory(dt);
    }
}

void SemanticMemory::setBrainReference(Brain* brain) {
    pImpl->brainReference = brain;
}

bool SemanticMemory::hasBrainReference() const {
    return pImpl->brainReference != nullptr;
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    enum class ImplementationMode currentMode;
    Brain* brainReference;
    
    // Simple implementation
    std::vector<Skill> simpleSkills;
    
    // Neural implementation
    std::vector<Skill> neuralSkills;
    
    // Statistics
    SimulationStep lastUpdateTime;
    
    Impl() : currentMode(ImplementationMode::Simple), brainReference(nullptr),
             lastUpdateTime(0) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(std::make_unique<Impl>()) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::setImplementationMode(ImplementationMode mode) {
    pImpl->currentMode = mode;
    NLM_LOG_INFO("ProceduralMemory implementation mode set to: " +
                (mode == ImplementationMode::Simple ? "Simple" :
                 mode == ImplementationMode::Neural ? "Neural" : "Hybrid"));
}

ProceduralMemory::ImplementationMode ProceduralMemory::getImplementationMode() const {
    return pImpl->currentMode;
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    skill.neuralPattern = pattern;
                    return;
                }
            }
            Skill skill;
            skill.name = name;
            skill.neuralPattern = pattern;
            pImpl->simpleSkills.push_back(skill);
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    skill.neuralPattern = pattern;
                    return;
                }
            }
            Skill skill;
            skill.name = name;
            skill.neuralPattern = pattern;
            pImpl->simpleSkills.push_back(skill);
            
            // Update neural system
            if (pImpl->brainReference) {
                pImpl->brainReference->learnProceduralSkill(name, pattern);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->learnProceduralSkill(name, pattern);
            }
            break;
    }
}

Skill* ProceduralMemory::getSkill(const std::string& name) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    return &skill;
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    return &skill;
                }
            }
            if (pImpl->brainReference) {
                Skill* neuralSkill = pImpl->brainReference->getProceduralSkill(name);
                if (neuralSkill) {
                    // Create a wrapper for the neural skill
                    static Skill wrapperSkill;
                    wrapperSkill.name = neuralSkill->name;
                    wrapperSkill.neuralPattern = neuralSkill->neuralPattern;
                    wrapperSkill.proficiency = neuralSkill->proficiency;
                    wrapperSkill.lastUsed = neuralSkill->lastUsed;
                    return &wrapperSkill;
                }
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                Skill* neuralSkill = pImpl->brainReference->getProceduralSkill(name);
                if (neuralSkill) {
                    // Create a wrapper for the neural skill
                    static Skill wrapperSkill;
                    wrapperSkill.name = neuralSkill->name;
                    wrapperSkill.neuralPattern = neuralSkill->neuralPattern;
                    wrapperSkill.proficiency = neuralSkill->proficiency;
                    wrapperSkill.lastUsed = neuralSkill->lastUsed;
                    return &wrapperSkill;
                }
            }
            break;
    }
    
    return nullptr;
}

std::vector<Skill*> ProceduralMemory::getAllSkills() {
    std::vector<Skill*> result;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& skill : pImpl->simpleSkills) {
                result.push_back(&skill);
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& skill : pImpl->simpleSkills) {
                result.push_back(&skill);
            }
            if (pImpl->brainReference) {
                std::vector<Skill*> neuralSkills = pImpl->brainReference->getAllProceduralSkills();
                for (Skill* skill : neuralSkills) {
                    // Create wrappers for neural skills
                    static std::vector<Skill> skillWrappers;
                    Skill wrapperSkill;
                    wrapperSkill.name = skill->name;
                    wrapperSkill.neuralPattern = skill->neuralPattern;
                    wrapperSkill.proficiency = skill->proficiency;
                    wrapperSkill.lastUsed = skill->lastUsed;
                    skillWrappers.push_back(wrapperSkill);
                    result.push_back(&skillWrappers.back());
                }
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                std::vector<Skill*> neuralSkills = pImpl->brainReference->getAllProceduralSkills();
                for (Skill* skill : neuralSkills) {
                    // Create wrappers for neural skills
                    static std::vector<Skill> skillWrappers;
                    Skill wrapperSkill;
                    wrapperSkill.name = skill->name;
                    wrapperSkill.neuralPattern = skill->neuralPattern;
                    wrapperSkill.proficiency = skill->proficiency;
                    wrapperSkill.lastUsed = skill->lastUsed;
                    skillWrappers.push_back(wrapperSkill);
                    result.push_back(&skillWrappers.back());
                }
            }
            break;
    }
    
    return result;
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
                    return;
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
                    return;
                }
            }
            if (pImpl->brainReference) {
                pImpl->brainReference->updateProceduralProficiency(name, delta);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->updateProceduralProficiency(name, delta);
            }
            break;
    }
}

void ProceduralMemory::clear() {
    pImpl->simpleSkills.clear();
    pImpl->neuralSkills.clear();
    
    if (pImpl->brainReference) {
        pImpl->brainReference->clearProceduralMemory();
    }
}

void ProceduralMemory::executeSkill(const std::string& name) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    // Execute simple skill
                    NLM_LOG_INFO("Executing skill: " + name);
                    break;
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    // Execute simple skill
                    NLM_LOG_INFO("Executing skill: " + name);
                    skill.lastUsed = 0; // Current time would be stored here
                    break;
                }
            }
            if (pImpl->brainReference) {
                pImpl->brainReference->executeProceduralSkill(name);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->executeProceduralSkill(name);
            }
            break;
    }
}

void ProceduralMemory::initializeNeuralProceduralMemory(Brain* brain) {
    if (brain) {
        pImpl->brainReference = brain;
        brain->initializeProceduralMemory();
        NLM_LOG_INFO("ProceduralMemory neural system initialized");
    }
}

void ProceduralMemory::updateNeuralProceduralMemory(TimestepDuration dt) {
    if (pImpl->brainReference) {
        pImpl->brainReference->updateProceduralMemory(dt);
    }
}

void ProceduralMemory::setBrainReference(Brain* brain) {
    pImpl->brainReference = brain;
}

bool ProceduralMemory::hasBrainReference() const {
    return pImpl->brainReference != nullptr;
}

void ProceduralMemory::reinforceSkill(const std::string& name, float reward) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    skill.proficiency = std::min(1.0f, skill.proficiency + reward * 0.1f);
                    break;
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& skill : pImpl->simpleSkills) {
                if (skill.name == name) {
                    skill.proficiency = std::min(1.0f, skill.proficiency + reward * 0.1f);
                    break;
                }
            }
            if (pImpl->brainReference) {
                pImpl->brainReference->reinforceProceduralSkill(name, reward);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->reinforceProceduralSkill(name, reward);
            }
            break;
    }
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    enum class ImplementationMode currentMode;
    Brain* brainReference;
    
    // Simple implementation
    std::vector<std::tuple<NeuronId, NeuronId, float>> simpleAssociations;
    
    // Neural implementation
    std::vector<std::tuple<NeuronId, NeuronId, float>> neuralAssociations;
    
    // Statistics
    SimulationStep lastUpdateTime;
    
    Impl() : currentMode(ImplementationMode::Simple), brainReference(nullptr),
             lastUpdateTime(0) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(std::make_unique<Impl>()) {}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::setImplementationMode(ImplementationMode mode) {
    pImpl->currentMode = mode;
    NLM_LOG_INFO("AssociativeMemory implementation mode set to: " +
                (mode == ImplementationMode::Simple ? "Simple" :
                 mode == ImplementationMode::Neural ? "Neural" : "Hybrid"));
}

AssociativeMemory::ImplementationMode AssociativeMemory::getImplementationMode() const {
    return pImpl->currentMode;
}

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& assoc : pImpl->simpleAssociations) {
                if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                    (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                    std::get<2>(assoc) = strength;
                    return;
                }
            }
            pImpl->simpleAssociations.emplace_back(a, b, strength);
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& assoc : pImpl->simpleAssociations) {
                if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                    (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                    std::get<2>(assoc) = strength;
                    return;
                }
            }
            pImpl->simpleAssociations.emplace_back(a, b, strength);
            
            // Update neural system
            if (pImpl->brainReference) {
                pImpl->brainReference->associateNeurons(a, b, strength);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->associateNeurons(a, b, strength);
            }
            break;
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    std::vector<NeuronId> result;
    
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (const auto& assoc : pImpl->simpleAssociations) {
                if (std::get<0>(assoc) == neuron) {
                    result.push_back(std::get<1>(assoc));
                } else if (std::get<1>(assoc) == neuron) {
                    result.push_back(std::get<0>(assoc));
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (const auto& assoc : pImpl->simpleAssociations) {
                if (std::get<0>(assoc) == neuron) {
                    result.push_back(std::get<1>(assoc));
                } else if (std::get<1>(assoc) == neuron) {
                    result.push_back(std::get<0>(assoc));
                }
            }
            if (pImpl->brainReference) {
                std::vector<NeuronId> neuralAssocs = pImpl->brainReference->getAssociatedNeurons(neuron);
                for (const auto& assoc : neuralAssocs) {
                    if (std::find(result.begin(), result.end(), assoc) == result.end()) {
                        result.push_back(assoc);
                    }
                }
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                return pImpl->brainReference->getAssociatedNeurons(neuron);
            }
            break;
    }
    
    return result;
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (const auto& assoc : pImpl->simpleAssociations) {
                if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                    (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                    return std::get<2>(assoc);
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (const auto& assoc : pImpl->simpleAssociations) {
                if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                    (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                    return std::get<2>(assoc);
                }
            }
            if (pImpl->brainReference) {
                return pImpl->brainReference->getAssociationStrength(a, b);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                return pImpl->brainReference->getAssociationStrength(a, b);
            }
            break;
    }
    
    return 0.0f;
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            for (auto& assoc : pImpl->simpleAssociations) {
                if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                    (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                    std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
                    return;
                }
            }
            break;
            
        case ImplementationMode::Hybrid:
            for (auto& assoc : pImpl->simpleAssociations) {
                if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                    (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                    std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
                    return;
                }
            }
            if (pImpl->brainReference) {
                pImpl->brainReference->updateAssociationStrength(a, b, delta);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->updateAssociationStrength(a, b, delta);
            }
            break;
    }
}

void AssociativeMemory::clear() {
    pImpl->simpleAssociations.clear();
    pImpl->neuralAssociations.clear();
    
    if (pImpl->brainReference) {
        pImpl->brainReference->clearAssociativeMemory();
    }
}

void AssociativeMemory::initializeNeuralAssociativeMemory(Brain* brain) {
    if (brain) {
        pImpl->brainReference = brain;
        brain->initializeAssociativeMemory();
        NLM_LOG_INFO("AssociativeMemory neural system initialized");
    }
}

void AssociativeMemory::updateNeuralAssociativeMemory(TimestepDuration dt) {
    if (pImpl->brainReference) {
        pImpl->brainReference->updateAssociativeMemory(dt);
    }
}

void AssociativeMemory::applyHebbianLearning(NeuronId pre, NeuronId post, float learningRate) {
    switch (pImpl->currentMode) {
        case ImplementationMode::Simple:
            // Create or update association based on Hebbian rule
            float strength = getAssociationStrength(pre, post);
            strength += learningRate;
            if (strength > 0.0f) {
                associate(pre, post, strength);
            }
            break;
            
        case ImplementationMode::Hybrid:
            // Simple Hebbian learning
            float strength = getAssociationStrength(pre, post);
            strength += learningRate;
            if (strength > 0.0f) {
                associate(pre, post, strength);
            }
            
            // Also update neural system if available
            if (pImpl->brainReference) {
                pImpl->brainReference->applyHebbianLearning(pre, post, learningRate);
            }
            break;
            
        case ImplementationMode::Neural:
            if (pImpl->brainReference) {
                pImpl->brainReference->applyHebbianLearning(pre, post, learningRate);
            }
            break;
    }
}

void AssociativeMemory::setBrainReference(Brain* brain) {
    pImpl->brainReference = brain;
}

bool AssociativeMemory::hasBrainReference() const {
    return pImpl->brainReference != nullptr;
}

// MemorySystem Implementation
MemorySystem::MemorySystem() : initialized_(false) {}

MemorySystem::~MemorySystem() {
    if (initialized_) {
        shutdown();
    }
}

MemorySystem& MemorySystem::getInstance() {
    static MemorySystem instance;
    return instance;
}

void MemorySystem::initialize(Brain* brain) {
    if (!initialized_ && brain) {
        brain_ = brain;
        
        // Initialize each memory system with neural support
        workingMemory_.setImplementationMode(WorkingMemory::ImplementationMode::Hybrid);
        workingMemory_.initializeNeuralWorkingMemory(brain);
        
        episodicMemory_.setImplementationMode(EpisodicMemory::ImplementationMode::Hybrid);
        episodicMemory_.initializeNeuralEpisodicMemory(brain);
        
        semanticMemory_.setImplementationMode(SemanticMemory::ImplementationMode::Hybrid);
        semanticMemory_.initializeNeuralSemanticMemory(brain);
        
        proceduralMemory_.setImplementationMode(ProceduralMemory::ImplementationMode::Hybrid);
        proceduralMemory_.initializeNeuralProceduralMemory(brain);
        
        associativeMemory_.setImplementationMode(AssociativeMemory::ImplementationMode::Hybrid);
        associativeMemory_.initializeNeuralAssociativeMemory(brain);
        
        initialized_ = true;
        NLM_LOG_INFO("MemorySystem initialized with neural integration");
    }
}

void MemorySystem::update(TimestepDuration dt) {
    if (initialized_ && brain_) {
        // Update all memory systems
        workingMemory_.updateNeuralWorkingMemory(dt);
        episodicMemory_.updateNeuralEpisodicMemory();
        semanticMemory_.updateNeuralSemanticMemory(dt);
        proceduralMemory_.updateNeuralProceduralMemory(dt);
        associativeMemory_.updateNeuralAssociativeMemory(dt);
    }
}

void MemorySystem::shutdown() {
    if (initialized_) {
        workingMemory_.clear();
        episodicMemory_.clear();
        semanticMemory_.clear();
        proceduralMemory_.clear();
        associativeMemory_.clear();
        
        brain_ = nullptr;
        initialized_ = false;
        NLM_LOG_INFO("MemorySystem shutdown");
    }
}

void MemorySystem::setWorkingMemoryMode(WorkingMemory::ImplementationMode mode) {
    workingMemory_.setImplementationMode(mode);
}

void MemorySystem::setEpisodicMemoryMode(EpisodicMemory::ImplementationMode mode) {
    episodicMemory_.setImplementationMode(mode);
}

void MemorySystem::setSemanticMemoryMode(SemanticMemory::ImplementationMode mode) {
    semanticMemory_.setImplementationMode(mode);
}

void MemorySystem::setProceduralMemoryMode(ProceduralMemory::ImplementationMode mode) {
    proceduralMemory_.setImplementationMode(mode);
}

void MemorySystem::setAssociativeMemoryMode(AssociativeMemory::ImplementationMode mode) {
    associativeMemory_.setImplementationMode(mode);
}

} // namespace nlm