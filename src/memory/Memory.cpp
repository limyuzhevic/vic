#include "Memory.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../brain/Brain.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <queue>

namespace nlm {

// ===========================================================================
// Phase 2: LEGACY MEMORY SYSTEM - DELEGATION IMPLEMENTATIONS
// ============================================================================

// Working Memory Implementation
struct WorkingMemory::Impl {
    std::vector<std::pair<NeuronId, float>> items;
    size_t capacity;
    NeuralWorkingMemory* neuralWorkingMemory;  // Pointer to Phase 4-6 implementation
    
    Impl(size_t cap) : capacity(cap), neuralWorkingMemory(nullptr) {}
};

WorkingMemory::WorkingMemory() : pImpl(new Impl(100)) {
}

WorkingMemory::~WorkingMemory() = default;

void WorkingMemory::initialize(NeuralWorkingMemory* neuralWM) {
    pImpl->neuralWorkingMemory = neuralWM;
}

void WorkingMemory::store(NeuronId neuron, float value) {
    // Delegate to neural implementation
    if (pImpl->neuralWorkingMemory) {
        std::vector<float> pattern = {value};
        pImpl->neuralWorkingMemory->store(pattern, 1.0f);
    } else {
        // Legacy implementation
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
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    if (pImpl->neuralWorkingMemory) {
        // Simplified: find closest match in working memory
        const auto& neurons = pImpl->neuralWorkingMemory->getMemoryNeurons();
        for (size_t i = 0; i < neurons.size(); ++i) {
            if (neurons[i] == neuron) {
                return pImpl->neuralWorkingMemory->getNeuronActivation(neuron);
            }
        }
        return 0.0f;
    } else {
        for (const auto& item : pImpl->items) {
            if (item.first == neuron) {
                return item.second;
            }
        }
        return 0.0f;
    }
}

bool WorkingMemory::contains(NeuronId neuron) const {
    if (pImpl->neuralWorkingMemory) {
        return std::find(pImpl->neuralWorkingMemory->getMemoryNeurons().begin(),
                        pImpl->neuralWorkingMemory->getMemoryNeurons().end(), neuron) !=
                        pImpl->neuralWorkingMemory->getMemoryNeurons().end();
    } else {
        for (const auto& item : pImpl->items) {
            if (item.first == neuron) {
                return true;
            }
        }
        return false;
    }
}

void WorkingMemory::clear() {
    if (pImpl->neuralWorkingMemory) {
        pImpl->neuralWorkingMemory->clear();
    } else {
        pImpl->items.clear();
    }
}

size_t WorkingMemory::getCapacity() const {
    return pImpl->capacity;
}

size_t WorkingMemory::getCurrentSize() const {
    if (pImpl->neuralWorkingMemory) {
        return pImpl->neuralWorkingMemory->getActiveTraces();
    } else {
        return pImpl->items.size();
    }
}

void WorkingMemory::decay(float decayRate) {
    // Neural implementation handles decay internally
}

// Episodic Memory Implementation
struct EpisodicMemory::Impl {
    std::vector<std::vector<float>> episodes;
    std::vector<SimulationStep> timestamps;
    size_t maxEpisodes;
    NeuralEpisodicMemory* neuralEpisodicMemory;  // Pointer to Phase 4-6 implementation
    
    Impl(size_t max) : maxEpisodes(max), neuralEpisodicMemory(nullptr) {}
};

EpisodicMemory::EpisodicMemory() : pImpl(new Impl(1000)) {
}

EpisodicMemory::~EpisodicMemory() = default;

void EpisodicMemory::initialize(NeuralEpisodicMemory* neuralEM) {
    pImpl->neuralEpisodicMemory = neuralEM;
}

void EpisodicMemory::storeEpisode(const std::vector<float>& sensoryState, float reward, SimulationStep timestamp) {
    if (pImpl->neuralEpisodicMemory) {
        pImpl->neuralEpisodicMemory->storeEpisode(sensoryState, reward, timestamp);
    } else {
        if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
            pImpl->episodes.erase(pImpl->episodes.begin());
            pImpl->timestamps.erase(pImpl->timestamps.begin());
        }
        pImpl->episodes.push_back(sensoryState);
        pImpl->timestamps.push_back(timestamp);
    }
}

std::vector<float> EpisodicMemory::retrieveEpisode(SimulationStep timestamp) const {
    if (pImpl->neuralEpisodicMemory) {
        // Find closest episode
        size_t closestIdx = 0;
        float minDiff = std::numeric_limits<float>::max();
        
        for (size_t i = 0; i < pImpl->timestamps.size(); ++i) {
            float diff = std::abs(static_cast<float>(pImpl->timestamps[i] - timestamp));
            if (diff < minDiff) {
                minDiff = diff;
                closestIdx = i;
            }
        }
        
        if (pImpl->timestamps.size() > 0) {
            return pImpl->episodes[closestIdx];
        }
        return {};
    } else {
        for (size_t i = 0; i < pImpl->timestamps.size(); ++i) {
            if (pImpl->timestamps[i] == timestamp) {
                return pImpl->episodes[i];
            }
        }
        return {};
    }
}

size_t EpisodicMemory::getEpisodeCount() const {
    if (pImpl->neuralEpisodicMemory) {
        return pImpl->neuralEpisodicMemory->getEpisodeCount();
    } else {
        return pImpl->episodes.size();
    }
}

std::vector<std::vector<float>> EpisodicMemory::getRecentEpisodes(size_t count) const {
    if (pImpl->neuralEpisodicMemory) {
        // Get indices of recent episodes
        auto recentIndices = pImpl->neuralEpisodicMemory->getRecentEpisodes(count);
        std::vector<std::vector<float>> result;
        for (size_t idx : recentIndices) {
            if (idx < pImpl->episodes.size()) {
                result.push_back(pImpl->episodes[idx]);
            }
        }
        return result;
    } else {
        std::vector<std::vector<float>> result;
        size_t start = count > pImpl->episodes.size() ? 0 : pImpl->episodes.size() - count;
        for (size_t i = start; i < pImpl->episodes.size(); ++i) {
            result.push_back(pImpl->episodes[i]);
        }
        return result;
    }
}

void EpisodicMemory::clear() {
    if (pImpl->neuralEpisodicMemory) {
        pImpl->neuralEpisodicMemory->clear();
    } else {
        pImpl->episodes.clear();
        pImpl->timestamps.clear();
    }
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    if (pImpl->neuralEpisodicMemory) {
        pImpl->neuralEpisodicMemory->consolidate(relevanceThreshold);
    }
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
    ConceptFormation* conceptFormation;  // Pointer to Phase 5 implementation
    
    Impl() : conceptFormation(nullptr) {}
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::initialize(ConceptFormation* conceptForm) {
    pImpl->conceptFormation = conceptForm;
}

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    if (pImpl->conceptFormation) {
        // Convert fact to concept
        std::vector<float> pattern(1, value.length() * 0.01f);  // Simple encoding
        pImpl->conceptFormation->presentExperience(pattern, std::vector<float>(1, 0.5f), 0.5f, 0);
    } else {
        for (auto& fact : pImpl->facts) {
            if (fact.first == key) {
                fact.second = value;
                return;
            }
        }
        pImpl->facts.emplace_back(key, value);
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    if (pImpl->conceptFormation) {
        // Simplified: return empty string for neural implementation
        return "";
    } else {
        for (const auto& fact : pImpl->facts) {
            if (fact.first == key) {
                return fact.second;
            }
        }
        return "";
    }
}

bool SemanticMemory::hasFact(const std::string& key) const {
    if (pImpl->conceptFormation) {
        // Simplified: return false for neural implementation
        return false;
    } else {
        for (const auto& fact : pImpl->facts) {
            if (fact.first == key) {
                return true;
            }
        }
        return false;
    }
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    if (pImpl->conceptFormation) {
        // Return empty for neural implementation
        return {};
    } else {
        return pImpl->facts;
    }
}

void SemanticMemory::clear() {
    if (pImpl->conceptFormation) {
        pImpl->conceptFormation->clear();
    } else {
        pImpl->facts.clear();
    }
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
    NeuralPlanner* neuralPlanner;  // Pointer to Phase 5 implementation
    ConceptFormation* conceptFormation;  // Pointer to Phase 5 implementation
    
    Impl() : neuralPlanner(nullptr), conceptFormation(nullptr) {}
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {
}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::initialize(NeuralPlanner* planner, ConceptFormation* concepts) {
    pImpl->neuralPlanner = planner;
    pImpl->conceptFormation = concepts;
}

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    if (pImpl->neuralPlanner) {
        // Convert skill to neural planning action
        ActionType action = static_cast<ActionType>(pattern.size() % 10);  // Simplified mapping
        pImpl->neuralPlanner->setActionQuality(action, 1.0f);
    } else {
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
}

std::vector<NeuronId> ProceduralMemory::getSkillPattern(const std::string& name) const {
    if (pImpl->neuralPlanner) {
        // Simplified: return empty for neural implementation
        return {};
    } else {
        for (const auto& skill : pImpl->skills) {
            if (skill.name == name) {
                return skill.neuralPattern;
            }
        }
        return {};
    }
}

std::vector<std::pair<std::string, std::vector<NeuronId>>> ProceduralMemory::getAllSkills() const {
    if (pImpl->neuralPlanner) {
        // Return empty for neural implementation
        return {};
    } else {
        std::vector<std::pair<std::string, std::vector<NeuronId>>> result;
        for (const auto& skill : pImpl->skills) {
            result.emplace_back(skill.name, skill.neuralPattern);
        }
        return result;
    }
}

void ProceduralMemory::updateProficiency(const std::string& name, float delta) {
    if (pImpl->neuralPlanner) {
        // Neural planner handles proficiency internally
    } else {
        for (auto& skill : pImpl->skills) {
            if (skill.name == name) {
                skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
                return;
            }
        }
    }
}

void ProceduralMemory::clear() {
    if (pImpl->neuralPlanner) {
        pImpl->neuralPlanner->clearCache();
    } else {
        pImpl->skills.clear();
    }
}

// Associative Memory Implementation
struct AssociativeMemory::Impl {
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    NeuralAssociativeMemory* neuralAssociativeMemory;  // Pointer to Phase 4 implementation
    
    Impl() : neuralAssociativeMemory(nullptr) {}
};

AssociativeMemory::AssociativeMemory() : pImpl(new Impl) {
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::initialize(NeuralAssociativeMemory* neuralAM) {
    pImpl->neuralAssociativeMemory = neuralAM;
}

void AssociativeMemory::associate(NeuronId a, NeuronId b, float strength) {
    if (pImpl->neuralAssociativeMemory) {
        std::vector<float> patternA = {static_cast<float>(a.value) * 0.001f};
        std::vector<float> patternB = {static_cast<float>(b.value) * 0.001f};
        pImpl->neuralAssociativeMemory->associate(patternA, patternB, strength);
    } else {
        for (auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                std::get<2>(assoc) = strength;
                return;
            }
        }
        pImpl->associations.emplace_back(a, b, strength);
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    if (pImpl->neuralAssociativeMemory) {
        // Simplified: return empty for neural implementation
        return {};
    } else {
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
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    if (pImpl->neuralAssociativeMemory) {
        // Simplified: return 0 for neural implementation
        return 0.0f;
    } else {
        for (const auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                return std::get<2>(assoc);
            }
        }
        return 0.0f;
    }
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    if (pImpl->neuralAssociativeMemory) {
        // Neural implementation handles updates internally
    } else {
        for (auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
                return;
            }
        }
    }
}

void AssociativeMemory::clear() {
    if (pImpl->neuralAssociativeMemory) {
        pImpl->neuralAssociativeMemory->clear();
    } else {
        pImpl->associations.clear();
    }
}

} // namespace nlm
