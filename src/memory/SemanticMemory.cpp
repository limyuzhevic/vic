#include "SemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Types/Types.hpp"
#include <algorithm>
#include <cmath>
#include <unordered_map>
#include <vector>
#include <string>
#include <sstream>

namespace nlm {

struct SemanticMemory::Impl {
    // Semantic knowledge base - organized facts and relationships
    std::unordered_map<std::string, std::string> facts_;
    std::unordered_map<std::string, std::vector<std::string>> associations_;
    std::unordered_map<std::string, float> confidenceScores_;
    
    // Knowledge organization
    std::vector<std::string> categories_;
    std::vector<std::string> concepts_;
    
    // Learning mechanisms
    float learningRate_;
    float consolidationThreshold_;
    size_t maxFacts_;
    
    // Integration with brain
    Brain* brain_;
    
    Impl() : learningRate_(0.1f), consolidationThreshold_(0.7f), maxFacts_(1000), brain_(nullptr) {}
    
    ~Impl() {
        clear();
    }
    
    // Store fact with learning mechanism
    void storeFact(const std::string& key, const std::string& value) {
        facts_[key] = value;
        confidenceScores_[key] = 1.0f;
        
        // Extract concept and category
        std::string category = extractCategory(key);
        std::string concept = extractConcept(key);
        
        categories_.push_back(category);
        concepts_.push_back(concept);
        
        // Establish associations
        establishAssociations(key, value);
        
        // Consolidate if needed
        consolidate(consolidationThreshold_);
        
        NLM_LOG_DEBUG("SemanticMemory: Stored fact '" + key + "' -> '" + value + "'");
    }
    
    std::string retrieveFact(const std::string& key) const {
        auto it = facts_.find(key);
        if (it == facts_.end()) return "";
        return it->second;
    }
    
    bool hasFact(const std::string& key) const {
        return facts_.find(key) != facts_.end();
    }
    
    void clear() {
        facts_.clear();
        associations_.clear();
        confidenceScores_.clear();
        categories_.clear();
        concepts_.clear();
        NLM_LOG_DEBUG("SemanticMemory: Cleared all knowledge");
    }
    
    // Get all facts as pairs
    std::vector<std::pair<std::string, std::string>> getAllFacts() const {
        std::vector<std::pair<std::string, std::string>> result;
        result.reserve(facts_.size());
        for (const auto& pair : facts_) {
            result.emplace_back(pair.first, pair.second);
        }
        return result;
    }
    
    // Search facts with partial matching
    std::vector<std::pair<std::string, std::string>> search(const std::string& query) const {
        std::vector<std::pair<std::string, std::string>> results;
        
        for (const auto& fact : facts_) {
            // Simple keyword matching
            if (containsIgnoreCase(fact.first, query) || 
                containsIgnoreCase(fact.second, query)) {
                results.emplace_back(fact.first, fact.second);
            }
        }
        
        // Sort by confidence
        std::sort(results.begin(), results.end(),
            [this](const auto& a, const auto& b) {
                return confidenceScores_.at(a.first) > confidenceScores_.at(b.first);
            });
        
        return results;
    }
    
    // Update fact with learning
    void updateFact(const std::string& key, const std::string& newValue, float learningRate = -1.0f) {
        if (hasFact(key)) {
            // Extract new concept and category
            std::string concept = extractConcept(key);
            std::string category = extractCategory(key);
            
            facts_[key] = newValue;
            
            // Increase confidence if value changed
            if (learningRate < 0.0f) {
                learningRate = learningRate_ * 0.5f;
            }
            
            confidenceScores_[key] = std::min(1.0f, confidenceScores_[key] + learningRate);
            
            // Update associations
            updateAssociations(key, newValue);
            
            NLM_LOG_DEBUG("SemanticMemory: Updated fact '" + key + "' to '" + newValue + "' (confidence: " + std::to_string(confidenceScores_[key]) + ")");
        }
    }
    
    // Pattern completion for partial facts
    std::string completePattern(const std::string& partial) const {
        std::string bestMatch = "";
        float bestScore = 0.0f;
        
        for (const auto& fact : facts_) {
            // Simple pattern matching
            if (startsWith(fact.first, partial) || startsWith(fact.second, partial)) {
                float score = confidenceScores_.at(fact.first);
                if (score > bestScore) {
                    bestScore = score;
                    bestMatch = fact.first + " -> " + fact.second;
                }
            }
        }
        
        return bestMatch;
    }
    
    // Get related concepts
    std::vector<std::string> getRelatedConcepts(const std::string& concept) const {
        std::vector<std::string> related;
        
        for (const auto& assoc : associations_) {
            if (assoc.first.find(concept) != std::string::npos || 
                concept.find(assoc.first) != std::string::npos) {
                related.insert(related.end(), assoc.second.begin(), assoc.second.end());
            }
        }
        
        // Remove duplicates
        std::sort(related.begin(), related.end());
        related.erase(std::unique(related.begin(), related.end()), related.end());
        
        return related;
    }
    
    // Get all concepts
    std::vector<std::string> getAllConcepts() const {
        std::vector<std::string> result;
        result.reserve(concepts_.size());
        for (const auto& concept : concepts_) {
            result.push_back(concept);
        }
        return result;
    }
    
    // Get all categories
    std::vector<std::string> getAllCategories() const {
        std::vector<std::string> result;
        result.reserve(categories_.size());
        for (const auto& category : categories_) {
            result.push_back(category);
        }
        return result;
    }
    
    // Consolidation (remove old/less confident facts)
    void consolidate(float threshold) {
        if (facts_.size() <= maxFacts_) return;
        
        std::vector<std::pair<std::string, float>> factScores;
        for (const auto& pair : confidenceScores_) {
            factScores.emplace_back(pair.first, pair.second);
        }
        
        // Sort by confidence (ascending)
        std::sort(factScores.begin(), factScores.end(),
            [](const auto& a, const auto& b) {
                return a.second < b.second;
            });
        
        // Remove lowest confidence facts
        size_t toRemove = facts_.size() - maxFacts_ / 2;  // Keep 50% of capacity
        for (size_t i = 0; i < toRemove && i < factScores.size(); ++i) {
            const auto& factToRemove = factScores[i].first;
            facts_.erase(factToRemove);
            confidenceScores_.erase(factToRemove);
            
            // Clean up from other data structures
            eraseFromVector(categories_, extractCategory(factToRemove));
            eraseFromVector(concepts_, extractConcept(factToRemove));
            associations_.erase(factToRemove);
        }
        
        NLM_LOG_DEBUG("SemanticMemory: Consolidated knowledge, removed " + std::to_string(toRemove) + " facts");
    }
    
    // Internal helper methods
    std::string extractCategory(const std::string& key) const {
        // Simple heuristic: first word or capitalized sequence
        std::istringstream iss(key);
        std::string word;
        if (iss >> word) {
            return word;
        }
        return "General";
    }
    
    std::string extractConcept(const std::string& key) const {
        // Extract the main concept from key
        size_t pos = key.find(' ');
        if (pos != std::string::npos) {
            return key.substr(0, pos);
        }
        return key;
    }
    
    void establishAssociations(const std::string& key, const std::string& value) {
        // Simple association building
        size_t keyWords = countWords(key);
        size_t valueWords = countWords(value);
        
        if (keyWords > 1) {
            std::string mainKey = extractFirstWord(key);
            associations_[mainKey].push_back(value);
        }
        
        if (valueWords > 1) {
            std::string mainValue = extractFirstWord(value);
            associations_[mainValue].push_back(key);
        }
    }
    
    void updateAssociations(const std::string& key, const std::string& value) {
        // Update existing associations
        std::string concept = extractConcept(key);
        
        auto it = associations_.find(concept);
        if (it != associations_.end()) {
            // Refresh associations by removing and re-adding
            associations_.erase(it);
            establishAssociations(key, value);
        }
    }
    
    bool containsIgnoreCase(const std::string& str, const std::string& substr) const {
        std::string lowerStr = toLower(str);
        std::string lowerSubstr = toLower(substr);
        return lowerStr.find(lowerSubstr) != std::string::npos;
    }
    
    std::string toLower(const std::string& str) const {
        std::string result = str;
        std::transform(result.begin(), result.end(), result.begin(),
            [](unsigned char c) { return std::tolower(c); });
        return result;
    }
    
    size_t countWords(const std::string& str) const {
        std::istringstream iss(str);
        size_t count = 0;
        std::string word;
        while (iss >> word) {
            count++;
        }
        return count;
    }
    
    std::string extractFirstWord(const std::string& str) const {
        std::istringstream iss(str);
        std::string word;
        if (iss >> word) {
            return word;
        }
        return str;
    }
    
    template<typename T>
    void eraseFromVector(std::vector<T>& vec, const T& value) {
        auto it = std::find(vec.begin(), vec.end(), value);
        if (it != vec.end()) {
            vec.erase(it);
        }
    }
    
    // Integration with brain
    void setBrain(Brain* brain) {
        brain_ = brain;
        NLM_LOG_INFO("SemanticMemory: Connected to brain");
    }
    
    Brain* getBrain() const {
        return brain_;
    }
    
    // Statistical analysis
    float getAverageConfidence() const {
        if (confidenceScores_.empty()) return 0.0f;
        float sum = 0.0f;
        for (const auto& pair : confidenceScores_) {
            sum += pair.second;
        }
        return sum / confidenceScores_.size();
    }
    
    size_t getFactCount() const {
        return facts_.size();
    }
    
    float getLearningRate() const {
        return learningRate_;
    }
    
    void setLearningRate(float rate) {
        learningRate_ = std::max(0.0f, std::min(rate, 1.0f));
    }
    
    float getConsolidationThreshold() const {
        return consolidationThreshold_;
    }
    
    void setConsolidationThreshold(float threshold) {
        consolidationThreshold_ = std::max(0.0f, std::min(threshold, 1.0f));
    }
    
    size_t getMaxFacts() const {
        return maxFacts_;
    }
    
    void setMaxFacts(size_t max) {
        maxFacts_ = max;
    }
};

// SemanticMemory implementation

SemanticMemory::SemanticMemory() : pImpl(std::make_unique<Impl>()) {
    NLM_LOG_INFO("SemanticMemory initialized with max facts " + std::to_string(pImpl->getMaxFacts()));
}

SemanticMemory::~SemanticMemory() = default;

void SemanticMemory::storeFact(const std::string& key, const std::string& value) {
    pImpl->storeFact(key, value);
}

std::string SemanticMemory::retrieveFact(const std::string& key) const {
    return pImpl->retrieveFact(key);
}

bool SemanticMemory::hasFact(const std::string& key) const {
    return pImpl->hasFact(key);
}

void SemanticMemory::clear() {
    pImpl->clear();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::getAllFacts() const {
    return pImpl->getAllFacts();
}

std::vector<std::pair<std::string, std::string>> SemanticMemory::search(const std::string& query) const {
    return pImpl->search(query);
}

void SemanticMemory::updateFact(const std::string& key, const std::string& newValue, float learningRate) {
    pImpl->updateFact(key, newValue, learningRate);
}

std::string SemanticMemory::completePattern(const std::string& partial) const {
    return pImpl->completePattern(partial);
}

std::vector<std::string> SemanticMemory::getRelatedConcepts(const std::string& concept) const {
    return pImpl->getRelatedConcepts(concept);
}

std::vector<std::string> SemanticMemory::getAllConcepts() const {
    return pImpl->getAllConcepts();
}

std::vector<std::string> SemanticMemory::getAllCategories() const {
    return pImpl->getAllCategories();
}

void SemanticMemory::consolidate(float threshold) {
    pImpl->consolidate(threshold);
}

void SemanticMemory::setLearningRate(float rate) {
    pImpl->setLearningRate(rate);
}

void SemanticMemory::setConsolidationThreshold(float threshold) {
    pImpl->setConsolidationThreshold(threshold);
}

void SemanticMemory::setMaxFacts(size_t max) {
    pImpl->setMaxFacts(max);
}

void SemanticMemory::initialize(Brain* brain) {
    if (pImpl) {
        pImpl->setBrain(brain);
    }
}

float SemanticMemory::getAverageConfidence() const {
    return pImpl ? pImpl->getAverageConfidence() : 0.0f;
}

size_t SemanticMemory::getFactCount() const {
    return pImpl ? pImpl->getFactCount() : 0;
}

float SemanticMemory::getLearningRate() const {
    return pImpl ? pImpl->getLearningRate() : 0.0f;
}

} // namespace nlm
