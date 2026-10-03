#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_set>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // Facts stored as neurons (key-value pairs)
    std::vector<NeuronId> factNeurons;
    std::vector<std::string> factKeys;
    
    // Associations between facts (knowledge graph)
    std::vector<std::tuple<NeuronId, NeuronId, float>> associations;
    
    Impl() : brain(nullptr) {}
};

NeuralSemanticMemory::NeuralSemanticMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
{
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("NeuralSemanticMemory initialized");
}

void NeuralSemanticMemory::storeFact(const std::string& key, const std::string& value, float confidence) {
    // Find or create fact neuron
    NeuronId neuron = findOrCreateFactNeuron(key);
    
    // Store fact value
    // In a full implementation, facts would be stored in neural patterns
    // For now, we maintain confidence and key-value mapping
    
    // Update confidence
    factConfidence_[key] = std::clamp(confidence, 0.0f, 1.0f);
    
    // Store value in neuron (simplified)
    if (brain_ && neuron != INVALID_NEURON_ID) {
        // Encode value as activation
        float activation = confidence;
        brain_->injectCurrent(neuron, activation * 10.0f);
    }
    
    NLM_LOG_INFO("Stored semantic fact: " + key + " = " + value + " (confidence: " + std::to_string(confidence) + ")");
}

std::string NeuralSemanticMemory::retrieveFact(const std::string& key) const {
    auto it = factConfidence_.find(key);
    if (it != factConfidence_.end()) {
        // In a full implementation, the value would be retrieved from neural pattern
        // For now, we return a placeholder based on confidence
        if (it->second > 0.5f) {
            return "fact_" + key + "_value";
        }
    }
    return "";
}

bool NeuralSemanticMemory::hasFact(const std::string& key) const {
    return factConfidence_.find(key) != factConfidence_.end();
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getAllFacts() const {
    std::vector<std::pair<std::string, std::string>> result;
    for (const auto& kv : factConfidence_) {
        std::string value = retrieveFact(kv.first);
        result.emplace_back(kv.first, value);
    }
    return result;
}

void NeuralSemanticMemory::updateFactConfidence(const std::string& key, float delta) {
    auto it = factConfidence_.find(key);
    if (it != factConfidence_.end()) {
        it->second = std::clamp(it->second + delta, 0.0f, 1.0f);
    }
}

float NeuralSemanticMemory::getFactConfidence(const std::string& key) const {
    auto it = factConfidence_.find(key);
    if (it != factConfidence_.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralSemanticMemory::clearFact(const std::string& key) {
    factConfidence_.erase(key);
    // Also remove from knowledge graph
    // (simplified implementation)
}

void NeuralSemanticMemory::clear() {
    factConfidence_.clear();
    knowledgeGraph_.clear();
}

size_t NeuralSemanticMemory::getFactCount() const {
    return factConfidence_.size();
}

void NeuralSemanticMemory::learnFromExperience(const std::vector<float>& sensoryPattern,
                                            const std::vector<float>& actionPattern,
                                            float reward) {
    if (!brain_) return;
    
    // Extract facts from the experience
    // In a full implementation, this would analyze patterns to extract regularities
    
    // Create knowledge based on experience
    // For example: "When sensoryPattern X, actionPattern Y results in reward Z"
    
    // Store as facts
    std::string sensoryFact = "sensory_pattern_" + std::to_string(sensoryPattern.size());
    std::string actionFact = "action_pattern_" + std::to_string(actionPattern.size());
    
    storeFact(sensoryFact, "has_pattern", 0.8f * reward);
    storeFact(actionFact, "performed_action", 0.7f * reward);
    
    // Create association
    addKnowledgeConnection(sensoryFact, actionFact, 0.6f * reward);
    
    NLM_LOG_INFO("NeuralSemanticMemory learned from experience: reward = " + std::to_string(reward));
}

std::vector<float> NeuralSemanticMemory::predictNextSensory(const std::vector<float>& currentState) const {
    // Simple prediction based on similar stored patterns
    std::vector<float> prediction(currentState.size(), 0.0f);
    
    if (currentState.empty()) return prediction;
    
    // For each stored fact, add its contribution to prediction
    for (const auto& kv : factConfidence_) {
        if (kv.second > 0.3f) {
            // Distribute activation based on fact relevance
            float contribution = kv.second * 0.1f;
            for (size_t i = 0; i < prediction.size() && i < currentState.size(); ++i) {
                prediction[i] += contribution;
            }
        }
    }
    
    return prediction;
}

std::vector<std::tuple<std::string, std::string, float>> NeuralSemanticMemory::getKnowledgeGraph() const {
    return knowledgeGraph_;
}

void NeuralSemanticMemory::addKnowledgeConnection(const std::string& fact1, const std::string& fact2, float strength) {
    // Find or create neurons for facts
    NeuronId neuron1 = findOrCreateFactNeuron(fact1);
    NeuronId neuron2 = findOrCreateFactNeuron(fact2);
    
    if (neuron1 != INVALID_NEURON_ID && neuron2 != INVALID_NEURON_ID) {
        // Check if association exists
        for (auto& assoc : knowledgeGraph_) {
            if ((std::get<0>(assoc) == fact1 && std::get<1>(assoc) == fact2) ||
                (std::get<0>(assoc) == fact2 && std::get<1>(assoc) == fact1)) {
                // Update strength
                if (fact1 == std::get<0>(assoc)) {
                    std::get<2>(assoc) = strength;
                } else {
                    std::get<0>(assoc) = fact1;  // Store as ordered pair
                    std::get<2>(assoc) = strength;
                }
                return;
            }
        }
        
        // Create new association
        knowledgeGraph_.emplace_back(fact1, fact2, strength);
    }
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getSimilarFacts(const std::string& query) const {
    std::vector<std::pair<std::string, std::string>> result;
    
    // Find facts with similar keys or values
    for (const auto& kv : factConfidence_) {
        // Simple similarity check - in full implementation would use semantic similarity
        if (keySimilarity(query, kv.first) > 0.5f) {
            result.push_back(kv);
        }
    }
    
    return result;
}

void NeuralSemanticMemory::consolidate(float relevanceThreshold) {
    // Remove facts with low confidence
    auto it = factConfidence_.begin();
    while (it != factConfidence_.end()) {
        if (it->second < relevanceThreshold) {
            it = factConfidence_.erase(it);
        } else {
            ++it;
        }
    }
    
    // Also consolidate knowledge graph
    auto graphIt = knowledgeGraph_.begin();
    while (graphIt != knowledgeGraph_.end()) {
        if (std::get<2>(*graphIt) < relevanceThreshold * 0.5f) {
            graphIt = knowledgeGraph_.erase(graphIt);
        } else {
            ++graphIt;
        }
    }
}

float NeuralSemanticMemory::getSemanticSimilarity(const std::string& fact1, const std::string& fact2) const {
    if (fact1 == fact2) return 1.0f;
    if (!hasFact(fact1) || !hasFact(fact2)) return 0.0f;
    
    // Check if facts are directly connected
    for (const auto& assoc : knowledgeGraph_) {
        if ((std::get<0>(assoc) == fact1 && std::get<1>(assoc) == fact2) ||
            (std::get<0>(assoc) == fact2 && std::get<1>(assoc) == fact1)) {
            return std::get<2>(assoc);
        }
    }
    
    // Check for indirect connections via shared neighbors
    std::set<std::string> neighbors1, neighbors2;
    for (const auto& assoc : knowledgeGraph_) {
        if (std::get<0>(assoc) == fact1) {
            neighbors1.insert(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == fact1) {
            neighbors1.insert(std::get<0>(assoc));
        }
        
        if (std::get<0>(assoc) == fact2) {
            neighbors2.insert(std::get<1>(assoc));
        } else if (std::get<1>(assoc) == fact2) {
            neighbors2.insert(std::get<0>(assoc));
        }
    }
    
    // Find intersection
    float common = 0;
    for (const auto& n1 : neighbors1) {
        if (neighbors2.find(n1) != neighbors2.end()) {
            common++;
        }
    }
    
    if (neighbors1.empty() && neighbors2.empty()) return 0.0f;
    return common / std::sqrt(static_cast<float>(neighbors1.size() * neighbors2.size()));
}

NeuronId NeuralSemanticMemory::findOrCreateFactNeuron(const std::string& key) {
    // Check if we already have a neuron for this fact
    for (size_t i = 0; i < factKeys.size(); ++i) {
        if (factKeys[i] == key) {
            return factNeurons[i];
        }
    }
    
    // Create new fact neuron
    NeuronId newId(10000 + factKeys.size());  // Reserve IDs for facts
    factNeurons.push_back(newId);
    factKeys.push_back(key);
    
    return newId;
}

float NeuralSemanticMemory::keySimilarity(const std::string& key1, const std::string& key2) const {
    // Simple string similarity based on common characters
    std::unordered_set<char> chars1(key1.begin(), key1.end());
    std::unordered_set<char> chars2(key2.begin(), key2.end());
    
    size_t intersection = 0;
    for (char c : chars1) {
        if (chars2.find(c) != chars2.end()) {
            intersection++;
        }
    }
    
    size_t unionSize = chars1.size() + chars2.size() - intersection;
    if (unionSize == 0) return 1.0f;
    
    return static_cast<float>(intersection) / static_cast<float>(unionSize);
}

} // namespace nlm
