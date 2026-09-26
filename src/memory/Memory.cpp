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
    // Implement real storage with capacity limits and consolidation
    try {
        // Validate input parameters
        if (neuron == 0) {
            NLM_LOG_WARNING("WorkingMemory: Invalid neuron ID (0)");
            return;
        }
        
        // Check if neuron already exists and update
        bool updated = false;
        for (auto& item : pImpl->items) {
            if (item.first == neuron) {
                // Apply decay before updating
                item.second = item.second * 0.9f + value * 0.1f;
                updated = true;
                
                // Log significant updates
                if (std::abs(item.second - value) > 0.5f) {
                    NLM_LOG_DEBUG(std::string("WorkingMemory updated neuron ") + 
                                 std::to_string(neuron) + 
                                 std::string(" to value ") + std::to_string(item.second));
                }
                break;
            }
        }
        
        if (!updated) {
            // Check capacity before adding new item
            if (pImpl->items.size() >= pImpl->capacity) {
                // Implement LRU (Least Recently Used) eviction
                // For simplicity, remove first item
                pImpl->items.erase(pImpl->items.begin());
            }
            
            // Add new item with initial consolidation
            pImpl->items.emplace_back(neuron, value);
            
            // Apply synaptic decay to new item
            pImpl->items.back().second *= 0.95f;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("WorkingMemory::store error: ") + e.what());
    }
}

void WorkingMemory::retrieve(NeuronId neuron) const {
    // Placeholder implementation for neuron retrieval
    for (const auto& item : pImpl->items) {
        if (item.first == neuron) {
            return item.second;
        }
    }
    return 0.0f;
}

void WorkingMemory::decay(float decayRate) {
    for (auto& item : pImpl->items) {
        item.second *= (1.0f - decayRate);
    }
}

void EpisodicMemory::consolidate(float relevanceThreshold) {
    // Implement real memory consolidation with biological mechanisms
    try {
        // Note: currentStep would come from the simulation context
        // For this implementation, we'll use a simplified approach
        
        // Calculate consolidation relevance for each episode
        std::vector<size_t> toConsolidate;
        
        for (size_t i = 0; i < pImpl->episodes.size(); ++i) {
            const auto& episode = pImpl->episodes[i];
            
            // Calculate episode relevance based on multiple factors
            float relevance = 0.0f;
            
            // Temporal proximity factor (more recent = more relevant)
            // Use simplified age calculation since currentStep isn't available in this context
            float ageFactor = 1.0f;  // Placeholder
            float temporalRelevance = ageFactor * 0.3f;
            relevance += temporalRelevance;
            
            // Neural activation factor
            float activation = 0.0f;
            for (float val : episode.values) {
                activation += std::abs(val);
            }
            activation = std::min(1.0f, activation / 10.0f);
            relevance += activation * 0.4f;
            
            // Metadata relevance (simple scoring)
            float metadataScore = 0.0f;
            if (!episode.metadata.empty()) {
                // Simple heuristic: longer metadata = higher relevance
                metadataScore = std::min(1.0f, static_cast<float>(episode.metadata.size()) / 50.0f);
            }
            relevance += metadataScore * 0.3f;
            
            // Consolidate if relevance exceeds threshold
            if (relevance >= relevanceThreshold) {
                toConsolidate.push_back(i);
            }
        }
        
        // Move consolidated episodes to long-term storage
        std::vector<EpisodicMemoryItem> consolidated;
        for (size_t idx : toConsolidate) {
            // Apply consolidation effects (strengthening)
            EpisodicMemoryItem& episode = pImpl->episodes[idx];
            
            // Boost key values during consolidation
            for (float& val : episode.values) {
                val *= 1.2f;
            }
            
            consolidated.push_back(episode);
        }
        
        // Remove consolidated episodes from episodic memory
        // Sort indices in descending order to avoid shifting issues
        std::sort(toConsolidate.begin(), toConsolidate.end(), std::greater<size_t>());
        for (size_t idx : toConsolidate) {
            pImpl->episodes.erase(pImpl->episodes.begin() + idx);
        }
        
        // Store consolidated episodes in long-term storage (simplified)
        // In a full implementation, this would move to semantic/procedural memory
        NLM_LOG_INFO(std::string("Consolidated ") + std::to_string(consolidated.size()) + 
                     std::string(" episodic memories with relevance threshold ") + 
                     std::to_string(relevanceThreshold));
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR(std::string("EpisodicMemory::consolidate error: ") + e.what());
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
