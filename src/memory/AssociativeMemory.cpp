#include "AssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <queue>
#include <unordered_set>

namespace nlm {

struct AssociativeMemory::Impl {
    // Core associative networks - Hebbian learning
    std::unordered_map<uint64_t, float> associations_;  // key = combined neuron ID
    std::unordered_map<uint64_t, std::string> neuronToString_;
    
    // Spreading activation system
    std::unordered_map<uint64_t, float> activationLevels_;
    std::queue<uint64_t> activationQueue_;
    
    // Pattern storage and retrieval
    std::unordered_map<uint64_t, std::vector<float>> patterns_;
    std::unordered_map<uint64_t, std::vector<uint64_t>> reverseAssociations_;
    
    // Learning parameters
    float learningRate_;
    float decayRate_;
    float inhibitionStrength_;
    size_t maxAssociations_;
    int maxActivationSpreadSteps_;
    
    // Integration with brain
    Brain* brain_;
    
    Impl() : learningRate_(0.1f), decayRate_(0.95f), inhibitionStrength_(0.3f), 
             maxAssociations_(10000), maxActivationSpreadSteps_(5), brain_(nullptr) {}
    
    ~Impl() {
        clear();
    }
    
    // Create association between two neural patterns
    void associate(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength = 1.0f) {
        // Convert patterns to neuron IDs for indexing
        uint64_t idA = patternToId(patternA);
        uint64_t idB = patternToId(patternB);
        
        // Create bidirectional association
        uint64_t combinedId = combineIds(idA, idB);
        associations_[combinedId] = std::max(associations_[combinedId], strength);
        
        // Store patterns for retrieval
        patterns_[idA] = patternA;
        patterns_[idB] = patternB;
        
        // Update reverse associations
        reverseAssociations_[idA].push_back(idB);
        reverseAssociations_[idB].push_back(idA);
        
        // Establish string representations for debugging
        neuronToString_[idA] = patternToString(patternA);
        neuronToString_[idB] = patternToString(patternB);
        
        NLM_LOG_DEBUG("AssociativeMemory: Associated patterns " + std::to_string(idA) + " and " + std::to_string(idB) + 
                     " with strength " + std::to_string(strength));
        
        // Auto-consolidate if needed
        consolidate(consolidationThreshold_);
    }
    
    // Create association based on experience
    void associateFromExperience(const EpisodicMemoryItem& episode) {
        // Convert sensory state to pattern
        std::vector<float> sensoryPattern = episode.sensoryState;
        
        // Extract action-related pattern
        std::vector<float> actionPattern;
        if (episode.action != ActionType::Wait) {
            actionPattern = encodeAction(episode.action);
        }
        
        // Extract outcome pattern
        std::vector<float> outcomePattern = episode.resultingSensoryState;
        
        // Create associations between experience elements
        if (!sensoryPattern.empty() && !actionPattern.empty()) {
            float strength = episode.reward * episode.novelty;  // Reward and novelty strengthen associations
            associate(sensoryPattern, actionPattern, strength);
        }
        
        if (!actionPattern.empty() && !outcomePattern.empty()) {
            float strength = episode.resultingReward * episode.novelty;
            associate(actionPattern, outcomePattern, strength);
        }
        
        if (!sensoryPattern.empty() && !outcomePattern.empty()) {
            float strength = (episode.reward + episode.resultingReward) * 0.5f * episode.novelty;
            associate(sensoryPattern, outcomePattern, strength);
        }
        
        NLM_LOG_DEBUG("AssociativeMemory: Created associations from experience with reward " + std::to_string(episode.reward) + 
                     " and novelty " + std::to_string(episode.novelty));
    }
    
    // Retrieve patterns associated with a query pattern
    std::vector<std::vector<float>> retrieve(const std::vector<float>& queryPattern, size_t maxResults = 5) const {
        std::vector<std::vector<float>> results;
        
        // Find similar patterns
        std::vector<std::pair<float, uint64_t>> similarities;
        
        for (const auto& pair : patterns_) {
            if (pair.first != patternToId(queryPattern)) {
                float similarity = computeSimilarity(queryPattern, pair.second);
                if (similarity > 0.1f) {
                    similarities.emplace_back(similarity, pair.first);
                }
            }
        }
        
        // Sort by similarity (descending)
        std::sort(similarities.begin(), similarities.end(),
            [](const auto& a, const auto& b) {
                return a.first > b.first;
            });
        
        // Take top results
        size_t count = std::min(maxResults, similarities.size());
        for (size_t i = 0; i < count; ++i) {
            uint64_t patternId = similarities[i].second;
            auto it = patterns_.find(patternId);
            if (it != patterns_.end()) {
                results.push_back(it->second);
            }
        }
        
        return results;
    }
    
    // Get association strength between two patterns
    float getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const {
        uint64_t idA = patternToId(patternA);
        uint64_t idB = patternToId(patternB);
        uint64_t combinedId = combineIds(idA, idB);
        
        auto it = associations_.find(combinedId);
        if (it != associations_.end()) {
            return it->second;
        }
        
        // Try reverse order
        uint64_t combinedIdReverse = combineIds(idB, idA);
        auto itReverse = associations_.find(combinedIdReverse);
        if (itReverse != associations_.end()) {
            return itReverse->second;
        }
        
        return 0.0f;
    }
    
    // Update association based on outcome
    void updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta) {
        uint64_t idA = patternToId(patternA);
        uint64_t idB = patternToId(patternB);
        uint64_t combinedId = combineIds(idA, idB);
        
        auto it = associations_.find(combinedId);
        if (it != associations_.end()) {
            it->second += delta;
            it->second = std::clamp(it->second, 0.0f, 1.0f);
            
            NLM_LOG_DEBUG("AssociativeMemory: Updated association strength to " + std::to_string(it->second));
        }
    }
    
    // Spread activation from cue pattern through association network
    std::vector<std::vector<float>> spreadActivation(const std::vector<float>& cuePattern, size_t steps = 2) const {
        std::vector<std::vector<float>> activatedPatterns;
        
        // Convert cue to ID
        uint64_t cueId = patternToId(cuePattern);
        
        // Initialize activation queue
        std::queue<std::pair<uint64_t, float>> activationQueue;
        activationQueue.push({cueId, 1.0f});
        
        std::unordered_set<uint64_t> visited;
        visited.insert(cueId);
        
        for (int step = 0; step < maxActivationSpreadSteps_; ++step) {
            size_t queueSize = activationQueue.size();
            
            for (size_t i = 0; i < queueSize; ++i) {
                if (activationQueue.empty()) break;
                
                auto current = activationQueue.front();
                activationQueue.pop();
                
                uint64_t currentId = current.first;
                float activation = current.second;
                
                // Find associated patterns
                auto it = reverseAssociations_.find(currentId);
                if (it != reverseAssociations_.end()) {
                    for (uint64_t associatedId : it->second) {
                        if (visited.find(associatedId) == visited.end()) {
                            // Calculate activation based on association strength
                            float strength = getAssociationStrengthFromId(currentId, associatedId);
                            float newActivation = activation * strength * (1.0f - inhibitionStrength_);
                            
                            if (newActivation > 0.01f) {
                                activationQueue.push({associatedId, newActivation});
                                visited.insert(associatedId);
                            }
                        }
                    }
                }
            }
        }
        
        // Extract patterns from visited IDs
        for (uint64_t id : visited) {
            auto it = patterns_.find(id);
            if (it != patterns_.end()) {
                activatedPatterns.push_back(it->second);
            }
        }
        
        return activatedPatterns;
    }
    
    // Clear all associations
    void clear() {
        associations_.clear();
        neuronToString_.clear();
        patterns_.clear();
        reverseAssociations_.clear();
        activationLevels_.clear();
        
        while (!activationQueue_.empty()) activationQueue_.pop();
        
        NLM_LOG_DEBUG("AssociativeMemory: Cleared all associations");
    }
    
    // Get number of associations
    size_t getAssociationCount() const {
        return associations_.size();
    }
    
    // Get all associations for debugging
    std::vector<std::pair<std::string, std::string>> getAllAssociations() const {
        std::vector<std::pair<std::string, std::string>> result;
        
        for (const auto& pair : associations_) {
            // Find which IDs this represents
            for (const auto& pattern : patterns_) {
                uint64_t combinedId = combineIds(pattern.first, pattern.first);  // Simplification
                if (combinedId == pair.first) {
                    result.emplace_back(neuronToString_.at(pattern.first), neuronToString_.at(pattern.first));
                    break;
                }
            }
        }
        
        return result;
    }
    
    // Consolidate associations (remove weak ones)
    void consolidate(float threshold) {
        if (associations_.size() <= maxAssociations_ / 2) return;
        
        std::vector<std::pair<uint64_t, float>> associationScores;
        for (const auto& pair : associations_) {
            associationScores.emplace_back(pair.first, pair.second);
        }
        
        // Sort by strength (ascending)
        std::sort(associationScores.begin(), associationScores.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });
        
        // Remove weakest associations
        size_t toRemove = associations_.size() - maxAssociations_ / 2;
        for (size_t i = 0; i < toRemove && i < associationScores.size(); ++i) {
            uint64_t idToRemove = associationScores[i].first;
            
            // Remove reverse associations
            for (auto it = reverseAssociations_.begin(); it != reverseAssociations_.end(); ) {
                auto& assocList = it->second;
                assocList.erase(std::remove_if(assocList.begin(), assocList.end(),
                    [idToRemove](uint64_t id) { return id == idToRemove; }), assocList.end());
                
                if (assocList.empty()) {
                    it = reverseAssociations_.erase(it);
                } else {
                    ++it;
                }
            }
            
            // Remove from other structures
            associations_.erase(idToRemove);
            patterns_.erase(idToRemove);
            
            // Clean up string mapping
            for (auto it = neuronToString_.begin(); it != neuronToString_.end(); ) {
                if (it->first == idToRemove) {
                    it = neuronToString_.erase(it);
                } else {
                    ++it;
                }
            }
        }
        
        NLM_LOG_DEBUG("AssociativeMemory: Consolidated associations, removed " + std::to_string(toRemove) + " weak associations");
    }
    
    // Configuration
    void setLearningRate(float rate) {
        learningRate_ = std::max(0.0f, std::min(rate, 1.0f));
    }
    
    void setDecayRate(float rate) {
        decayRate_ = std::max(0.0f, std::min(rate, 1.0f));
    }
    
    void setInhibitionStrength(float strength) {
        inhibitionStrength_ = std::max(0.0f, std::min(strength, 1.0f));
    }
    
    void setMaxAssociations(size_t max) {
        maxAssociations_ = max;
    }
    
    void setMaxActivationSpreadSteps(int steps) {
        maxActivationSpreadSteps_ = steps;
    }
    
    // Integration with brain
    void setBrain(Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("AssociativeMemory: Connected to brain");
    }
    
    Brain* getBrain() const {
        return brain_;
    }
    
    // Statistical analysis
    float getAverageAssociationStrength() const {
        if (associations_.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& pair : associations_) {
            sum += pair.second;
        }
        return sum / associations_.size();
    }
    
    // Internal helper methods
    uint64_t patternToId(const std::vector<float>& pattern) const {
        // Simple hash function for pattern identification
        std::hash<float> hasher;
        uint64_t hash = 0;
        for (float val : pattern) {
            hash ^= hasher(val) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
        }
        return hash;
    }
    
    uint64_t combineIds(uint64_t idA, uint64_t idB) const {
        // Combine two IDs into a single ID for bidirectional lookup
        if (idA < idB) {
            return (idA << 32) | idB;
        } else {
            return (idB << 32) | idA;
        }
    }
    
    std::string patternToString(const std::vector<float>& pattern) const {
        std::ostringstream oss;
        oss << "pattern:";
        for (size_t i = 0; i < pattern.size() && i < 10; ++i) {  // Limit length
            oss << std::fixed << std::setprecision(2) << pattern[i];
            if (i < pattern.size() - 1) oss << ",";
        }
        if (pattern.size() > 10) oss << "...";
        return oss.str();
    }
    
    float computeSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
        if (a.size() != b.size() || a.empty()) return 0.0f;
        
        float dotProduct = 0.0f;
        float normA = 0.0f;
        float normB = 0.0f;
        
        for (size_t i = 0; i < a.size(); ++i) {
            dotProduct += a[i] * b[i];
            normA += a[i] * a[i];
            normB += b[i] * b[i];
        }
        
        float cosineSimilarity = dotProduct / (std::sqrt(normA) * std::sqrt(normB) + 1e-8f);
        return std::max(0.0f, cosineSimilarity);
    }
    
    float getAssociationStrengthFromId(uint64_t idA, uint64_t idB) const {
        uint64_t combinedId = combineIds(idA, idB);
        auto it = associations_.find(combinedId);
        if (it != associations_.end()) {
            return it->second;
        }
        return 0.0f;
    }
    
    std::vector<float> encodeAction(ActionType action) {
        // Simple encoding of action types
        std::vector<float> actionPattern(8, 0.0f);
        
        switch (action) {
            case ActionType::MoveForward: actionPattern[0] = 1.0f; break;
            case ActionType::MoveBackward: actionPattern[1] = 1.0f; break;
            case ActionType::TurnLeft: actionPattern[2] = 1.0f; break;
            case ActionType::TurnRight: actionPattern[3] = 1.0f; break;
            case ActionType::LookLeft: actionPattern[4] = 1.0f; break;
            case ActionType::LookRight: actionPattern[5] = 1.0f; break;
            case ActionType::Interact: actionPattern[6] = 1.0f; break;
            case ActionType::Wait: actionPattern[7] = 1.0f; break;
        }
        
        return actionPattern;
    }
    
    // Get consolidation threshold
    float getConsolidationThreshold() const {
        return maxAssociations_ > 0 ? 1.0f / maxAssociations_ : 0.0f;
    }
};

// AssociativeMemory implementation

AssociativeMemory::AssociativeMemory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("AssociativeMemory initialized with max associations " + std::to_string(pImpl->getMaxAssociations()));
}

AssociativeMemory::~AssociativeMemory() = default;

void AssociativeMemory::associate(const std::vector<float>& patternA, const std::vector<float>& patternB, float strength) {
    pImpl->associate(patternA, patternB, strength);
}

void AssociativeMemory::associateFromExperience(const EpisodicMemoryItem& episode) {
    pImpl->associateFromExperience(episode);
}

std::vector<std::vector<float>> AssociativeMemory::retrieve(const std::vector<float>& queryPattern, size_t maxResults) const {
    return pImpl->retrieve(queryPattern, maxResults);
}

float AssociativeMemory::getAssociationStrength(const std::vector<float>& patternA, const std::vector<float>& patternB) const {
    return pImpl->getAssociationStrength(patternA, patternB);
}

void AssociativeMemory::updateAssociation(const std::vector<float>& patternA, const std::vector<float>& patternB, float delta) {
    pImpl->updateAssociation(patternA, patternB, delta);
}

std::vector<std::vector<float>> AssociativeMemory::spreadActivation(const std::vector<float>& cuePattern, size_t steps) const {
    return pImpl->spreadActivation(cuePattern, steps);
}

void AssociativeMemory::clear() {
    pImpl->clear();
}

void AssociativeMemory::consolidate(float threshold) {
    pImpl->consolidate(threshold);
}

void AssociativeMemory::setLearningRate(float rate) {
    pImpl->setLearningRate(rate);
}

void AssociativeMemory::setDecayRate(float rate) {
    pImpl->setDecayRate(rate);
}

void AssociativeMemory::setInhibitionStrength(float strength) {
    pImpl->setInhibitionStrength(strength);
}

void AssociativeMemory::setMaxAssociations(size_t max) {
    pImpl->setMaxAssociations(max);
}

void AssociativeMemory::setMaxActivationSpreadSteps(int steps) {
    pImpl->setMaxActivationSpreadSteps(steps);
}

void AssociativeMemory::initialize(Brain* brain) {
    if (pImpl) {
        pImpl->setBrain(brain);
    }
}

size_t AssociativeMemory::getAssociationCount() const {
    return pImpl ? pImpl->getAssociationCount() : 0;
}

float AssociativeMemory::getAverageAssociationStrength() const {
    return pImpl ? pImpl->getAverageAssociationStrength() : 0.0f;
}

} // namespace nlm
