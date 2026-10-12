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
    try {
        // Validate neuron ID
        if (neuron.index() == 0) {
            Logger::getGlobal().error("WorkingMemory::store: Invalid neuron ID: " + std::to_string(neuron.index()));
            return;
        }
        
        // Validate value
        if (std::isnan(value) || std::isinf(value)) {
            Logger::getGlobal().error("WorkingMemory::store: Invalid value for neuron " + std::to_string(neuron.index()) + ": " + std::to_string(value));
            return;
        }
        
        // Check capacity
        if (pImpl->items.size() >= pImpl->capacity) {
            Logger::getGlobal().warning("WorkingMemory::store: Working memory at capacity, clearing old items");
            // Simple strategy: clear half the items
            size_t clearCount = pImpl->capacity / 2;
            if (clearCount > 0) {
                pImpl->items.erase(pImpl->items.begin(), pImpl->items.begin() + clearCount);
            }
        }
        
        // Check for existing entry
        for (auto& item : pImpl->items) {
            if (item.first == neuron) {
                item.second = value;
                return;
            }
        }
        
        // Add new entry
        pImpl->items.emplace_back(neuron, value);
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("WorkingMemory::store: Memory allocation failed while storing neuron " + std::to_string(neuron.index()));
    } catch (const std::exception& e) {
        Logger::getGlobal().error("WorkingMemory::store: Exception while storing neuron " + std::to_string(neuron.index()) + ": " + std::string(e.what()));
    }
}

float WorkingMemory::retrieve(NeuronId neuron) const {
    try {
        // Validate neuron ID
        if (neuron.index() == 0) {
            Logger::getGlobal().error("WorkingMemory::retrieve: Invalid neuron ID: " + std::to_string(neuron.index()));
            return 0.0f;
        }
        
        for (const auto& item : pImpl->items) {
            if (item.first == neuron) {
                return item.second;
            }
        }
        
        return 0.0f;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("WorkingMemory::retrieve: Exception while retrieving neuron " + std::to_string(neuron.index()) + ": " + std::string(e.what()));
        return 0.0f;
    }
}

bool WorkingMemory::contains(NeuronId neuron) const {
    try {
        // Validate neuron ID
        if (neuron.index() == 0) {
            Logger::getGlobal().error("WorkingMemory::contains: Invalid neuron ID: " + std::to_string(neuron.index()));
            return false;
        }
        
        for (const auto& item : pImpl->items) {
            if (item.first == neuron) {
                return true;
            }
        }
        
        return false;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("WorkingMemory::contains: Exception while checking neuron " + std::to_string(neuron.index()) + ": " + std::string(e.what()));
        return false;
    }
}

void WorkingMemory::clear() {
    try {
        pImpl->items.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("WorkingMemory::clear: Exception during clear: " + std::string(e.what()));
    }
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
    try {
        // Validate episode
        if (std::isnan(episode.reward)) {
            Logger::getGlobal().error("EpisodicMemory::storeEpisode: Invalid episode reward");
            return;
        }
        
        // Check capacity
        if (pImpl->episodes.size() >= pImpl->maxEpisodes) {
            Logger::getGlobal().warning("EpisodicMemory::storeEpisode: Memory at capacity, erasing oldest episode");
            pImpl->episodes.erase(pImpl->episodes.begin());
        }
        
        pImpl->episodes.push_back(episode);
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("EpisodicMemory::storeEpisode: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("EpisodicMemory::storeEpisode: Exception while storing episode: " + std::string(e.what()));
    }
}

EpisodicMemoryItem EpisodicMemory::retrieveEpisode(size_t index) const {
    try {
        // Validate index
        if (index >= pImpl->episodes.size()) {
            Logger::getGlobal().error("EpisodicMemory::retrieveEpisode: Invalid episode index: " + std::to_string(index) + " (total: " + std::to_string(pImpl->episodes.size()) + ")");
            return EpisodicMemoryItem();
        }
        
        return pImpl->episodes[index];
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("EpisodicMemory::retrieveEpisode: Exception while retrieving episode: " + std::string(e.what()));
        return EpisodicMemoryItem();
    }
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
    try {
        pImpl->episodes.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("EpisodicMemory::clear: Exception during clear: " + std::string(e.what()));
    }
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // TODO PHASE 2: Implement real consolidation
}

// Semantic Memory Implementation
struct SemanticMemory::Impl {
    std::vector<std::pair<std::string, std::string>> facts;
};

SemanticMemory::SemanticMemory() : pImpl(new Impl) {}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    try {
        // Validate key and value
        if (key.empty()) {
            Logger::getGlobal().error("SemanticMemory::storeFact: Empty key");
            return;
        }
        
        if (key.length() > 1024) {
            Logger::getGlobal().warning("SemanticMemory::storeFact: Key too long (" + std::to_string(key.length()) + " chars)");
        }
        
        if (value.empty()) {
            Logger::getGlobal().warning("SemanticMemory::storeFact: Empty value for key: " + key);
        }
        
        if (value.length() > 4096) {
            Logger::getGlobal().warning("SemanticMemory::storeFact: Value too long (" + std::to_string(value.length()) + " chars)");
        }
        
        for (auto& fact : pImpl->facts) {
            if (fact.first == key) {
                fact.second = value;
                return;
            }
        }
        
        pImpl->facts.emplace_back(key, value);
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("SemanticMemory::storeFact: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SemanticMemory::storeFact: Exception while storing fact: " + std::string(e.what()));
    }
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    try {
        // Validate key
        if (key.empty()) {
            Logger::getGlobal().error("SemanticMemory::retrieveFact: Empty key provided");
            return "";
        }
        
        for (const auto& fact : pImpl->facts) {
            if (fact.first == key) {
                return fact.second;
            }
        }
        
        return "";
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SemanticMemory::retrieveFact: Exception while retrieving fact: " + std::string(e.what()));
        return "";
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
    try {
        pImpl->facts.clear();
    } catch (const std::exception& e) {
        Logger::getGlobal().error("SemanticMemory::clear: Exception during clear: " + std::string(e.what()));
    }
}

// Procedural Memory Implementation
struct ProceduralMemory::Impl {
    std::vector<Skill> skills;
};

ProceduralMemory::ProceduralMemory() : pImpl(new Impl) {}

ProceduralMemory::~ProceduralMemory() = default;

void ProceduralMemory::learnSkill(const std::string& name, const std::vector<NeuronId>& pattern) {
    try {
        // Validate skill name
        if (name.empty()) {
            Logger::getGlobal().error("ProceduralMemory::learnSkill: Empty skill name");
            return;
        }
        
        if (name.length() > 256) {
            Logger::getGlobal().warning("ProceduralMemory::learnSkill: Skill name too long (" + std::to_string(name.length()) + " chars)");
        }
        
        // Validate pattern
        if (pattern.size() > 1000) {
            Logger::getGlobal().warning("ProceduralMemory::learnSkill: Very large neural pattern size (" + std::to_string(pattern.size()) + ")");
        }
        
        for (const auto& neuron : pattern) {
            if (neuron.index() == 0) {
                Logger::getGlobal().error("ProceduralMemory::learnSkill: Invalid neuron in pattern for skill: " + name);
                return;
            }
        }
        
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
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("ProceduralMemory::learnSkill: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("ProceduralMemory::learnSkill: Exception while learning skill: " + std::string(e.what()));
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
    try {
        // Validate name and delta
        if (name.empty()) {
            Logger::getGlobal().error("ProceduralMemory::updateProficiency: Empty skill name");
            return;
        }
        
        if (std::isnan(delta) || std::isinf(delta)) {
            Logger::getGlobal().error("ProceduralMemory::updateProficiency: Invalid delta for skill: " + name + " (" + std::to_string(delta) + ")");
            return;
        }
        
        for (auto& skill : pImpl->skills) {
            if (skill.name == name) {
                skill.proficiency = std::clamp(skill.proficiency + delta, 0.0f, 1.0f);
                return;
            }
        }
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("ProceduralMemory::updateProficiency: Exception while updating proficiency: " + std::string(e.what()));
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
    try {
        // Validate neuron IDs
        if (a.index() == 0 || b.index() == 0) {
            Logger::getGlobal().error("AssociativeMemory::associate: Invalid neuron IDs");
            return;
        }
        
        // Validate strength
        if (std::isnan(strength) || std::isinf(strength)) {
            Logger::getGlobal().error("AssociativeMemory::associate: Invalid association strength: " + std::to_string(strength));
            return;
        }
        
        if (strength < 0.0f || strength > 1.0f) {
            Logger::getGlobal().warning("AssociativeMemory::associate: Association strength out of range [0,1]: " + std::to_string(strength));
            strength = std::clamp(strength, 0.0f, 1.0f);
        }
        
        for (auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                std::get<2>(assoc) = strength;
                return;
            }
        }
        
        pImpl->associations.emplace_back(a, b, strength);
        
    } catch (const std::bad_alloc&) {
        Logger::getGlobal().error("AssociativeMemory::associate: Memory allocation failed");
    } catch (const std::exception& e) {
        Logger::getGlobal().error("AssociativeMemory::associate: Exception while creating association: " + std::string(e.what()));
    }
}

std::vector<NeuronId> AssociativeMemory::getAssociations(NeuronId neuron) const {
    try {
        // Validate neuron ID
        if (neuron.index() == 0) {
            Logger::getGlobal().error("AssociativeMemory::getAssociations: Invalid neuron ID: " + std::to_string(neuron.index()));
            return std::vector<NeuronId>();
        }
        
        std::vector<NeuronId> result;
        for (const auto& assoc : pImpl->associations) {
            if (std::get<0>(assoc) == neuron) {
                result.push_back(std::get<1>(assoc));
            } else if (std::get<1>(assoc) == neuron) {
                result.push_back(std::get<0>(assoc));
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("AssociativeMemory::getAssociations: Exception while retrieving associations: " + std::string(e.what()));
        return std::vector<NeuronId>();
    }
}

float AssociativeMemory::getAssociationStrength(NeuronId a, NeuronId b) const {
    try {
        // Validate neuron IDs
        if (a.index() == 0 || b.index() == 0) {
            Logger::getGlobal().error("AssociativeMemory::getAssociationStrength: Invalid neuron IDs");
            return 0.0f;
        }
        
        for (const auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                return std::get<2>(assoc);
            }
        }
        
        return 0.0f;
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("AssociativeMemory::getAssociationStrength: Exception while retrieving strength: " + std::string(e.what()));
        return 0.0f;
    }
}

void AssociativeMemory::updateAssociation(NeuronId a, NeuronId b, float delta) {
    try {
        // Validate neuron IDs
        if (a.index() == 0 || b.index() == 0) {
            Logger::getGlobal().error("AssociativeMemory::updateAssociation: Invalid neuron IDs");
            return;
        }
        
        // Validate delta
        if (std::isnan(delta) || std::isinf(delta)) {
            Logger::getGlobal().error("AssociativeMemory::updateAssociation: Invalid delta: " + std::to_string(delta));
            return;
        }
        
        for (auto& assoc : pImpl->associations) {
            if ((std::get<0>(assoc) == a && std::get<1>(assoc) == b) ||
                (std::get<0>(assoc) == b && std::get<1>(assoc) == a)) {
                std::get<2>(assoc) = std::clamp(std::get<2>(assoc) + delta, 0.0f, 1.0f);
                return;
            }
        }
        
    } catch (const std::exception& e) {
        Logger::getGlobal().error("AssociativeMemory::updateAssociation: Exception while updating association: " + std::string(e.what()));
    }
}

void AssociativeMemory::clear() {
    pImpl->associations.clear();
}

} // namespace nlm
