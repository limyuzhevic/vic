#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <set>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    // Neural network for semantic vectors
    struct NeuralNetwork {
        std::vector<float> weights;
        float learningRate;
        float weightDecay;
        size_t inputDim;
        size_t hiddenDim;
        
        NeuralNetwork(size_t inputSize, size_t hiddenSize)
            : inputDim(inputSize), hiddenDim(hiddenSize), learningRate(0.01f), weightDecay(0.0001f) {
            size_t totalWeights = (inputDim + 1) * (hiddenDim + 1);
            weights.assign(totalWeights, 0.0f);
        }
        
        // Apply neural network to input pattern
        std::vector<float> forward(const std::vector<float>& input) {
            std::vector<float> hidden(hiddenDim, 0.0f);
            
            // Input to hidden layer
            for (size_t j = 0; j < hiddenDim; ++j) {
                float sum = 0.0f;
                // Bias
                size_t biasIdx = (0 * (hiddenDim + 1)) + j + 1;
                sum += weights[biasIdx];
                
                // Input connections
                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (hiddenDim + 1) + j + 1;
                    if (i < input.size()) {
                        sum += weights[weightIdx] * input[i];
                    }
                }
                
                hidden[j] = sigmoid(sum);
            }
            
            return hidden;
        }
        
        // Apply learning for concept
        void learn(const std::vector<float>& input, const std::vector<float>& target,
                  float error, float learningRate = 0.01f) {
            std::vector<float> hidden(hiddenDim, 0.0f);
            
            // Compute hidden activations
            for (size_t j = 0; j < hiddenDim; ++j) {
                float sum = 0.0f;
                size_t biasIdx = (0 * (hiddenDim + 1)) + j + 1;
                sum += weights[biasIdx];
                
                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (hiddenDim + 1) + j + 1;
                    if (i < input.size()) {
                        sum += weights[weightIdx] * input[i];
                    }
                }
                
                hidden[j] = sigmoid(sum);
            }
            
            // Backpropagate
            float lambda = 1.0f;
            for (size_t j = 0; j < hiddenDim; ++j) {
                float output = hidden[j];
                float grad = (target.size() > j ? target[j] : 0.0f) - output;
                float deriv = output * (1.0f - output);
                
                // Update bias
                size_t biasIdx = (0 * (hiddenDim + 1)) + j + 1;
                weights[biasIdx] += learningRate * grad * deriv * lambda;
                
                // Update weights
                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (hiddenDim + 1) + j + 1;
                    if (i < input.size()) {
                        weights[weightIdx] += learningRate * grad * deriv * lambda * input[i];
                    }
                }
                
                // Apply weight decay
                weights[biasIdx] *= (1.0f - weightDecay);
                for (size_t i = 0; i < inputDim; ++i) {
                    size_t weightIdx = (i + 1) * (hiddenDim + 1) + j + 1;
                    if (i < input.size()) {
                        weights[weightIdx] *= (1.0f - weightDecay);
                    }
                }
            }
        }
        
        static float sigmoid(float x) { return 1.0f / (1.0f + std::exp(-x)); }
    };
    
    NeuralNetwork* network;
    SimulationStep simulationTime;
    
    Impl() : network(nullptr), simulationTime(0) {}
    ~Impl() {
        if (network) {
            delete network;
        }
    }
};

NeuralSemanticMemory::NeuralSemanticMemory() : pImpl(new Impl()) {
    capacity_ = 1000;  // Default capacity
    hierarchyBuilt_ = false;
}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

void NeuralSemanticMemory::initialize(Brain* brain) {
    brain_ = brain;
    
    if (brain_ && !pImpl->network) {
        // Create neural network for semantic processing
        // Network dimension depends on semantic vector size
        pImpl->network = new NeuralSemanticMemory::Impl::NeuralNetwork(256, 128);
        
        // If there are existing concepts, initialize them
        if (!concepts_.empty()) {
            // Build initial network from existing concepts
            buildConceptHierarchy();
        }
        
        NLM_LOG_INFO("NeuralSemanticMemory initialized with " + std::to_string(concepts_.size()) + " concepts");
    }
}

void NeuralSemanticMemory::storeConcept(const std::string& key, const std::vector<float>& pattern) {
    if (brain_ && pImpl->network) {
        // Check if concept already exists
        if (concepts_.find(key) != concepts_.end()) {
            // Update existing concept
            conceptPatterns_[key] = pattern;
            conceptUsageCount_[key]++;
            conceptLastUsed_[key] = pImpl->simulationTime;
            return;
        }
        
        // Check capacity
        if (concepts_.size() >= capacity_) {
            // Remove least recently used concept
            std::string leastRecentKey;
            SimulationStep oldestTime = std::numeric_limits<SimulationStep>::max();
            
            for (const auto& pair : conceptLastUsed_) {
                if (pair.second < oldestTime) {
                    oldestTime = pair.second;
                    leastRecentKey = pair.first;
                }
            }
            
            if (!leastRecentKey.empty()) {
                // Remove concept
                auto it = conceptNeurons_.find(leastRecentKey);
                if (it != conceptNeurons_.end()) {
                    conceptNeurons_.erase(it);
                }
                conceptPatterns_.erase(leastRecentKey);
                conceptStrength_.erase(leastRecentKey);
                conceptUsageCount_.erase(leastRecentKey);
                conceptLastUsed_.erase(leastRecentKey);
                conceptCreationTime_.erase(leastRecentKey);
                conceptFrozen_.erase(leastRecentKey);
                conceptActivation_.erase(leastRecentKey);
                
                // Update hierarchy
                conceptParent_.erase(leastRecentKey);
                auto it2 = conceptChildren_.find(leastRecentKey);
                if (it2 != conceptChildren_.end()) {
                    conceptChildren_.erase(it2);
                }
                hierarchyBuilt_ = false;
            }
        }
        
        // Store concept
        concepts_.insert(key);
        conceptPatterns_[key] = pattern;
        conceptStrength_[key] = 1.0f;
        conceptUsageCount_[key] = 1;
        conceptLastUsed_[key] = pImpl->simulationTime;
        conceptCreationTime_[key] = pImpl->simulationTime;
        conceptFrozen_[key] = false;
        conceptActivation_[key] = 0.0f;
        
        // Create neural representation
        createConceptNeuron(key, pattern);
        
        // Update network
        std::vector<float> target(conceptPatterns_[key].size(), 0.0f);
        pImpl->network->learn(pattern, target, 1.0f);
        
        // Mark hierarchy as needing rebuild
        hierarchyBuilt_ = false;
        
        NLM_LOG_DEBUG("Stored semantic concept: " + key);
    }
}

std::vector<float> NeuralSemanticMemory::retrieveConcept(const std::string& key) const {
    auto it = conceptPatterns_.find(key);
    if (it != conceptPatterns_.end()) {
        return it->second;
    }
    return std::vector<float>();
}

bool NeuralSemanticMemory::hasConcept(const std::string& key) const {
    return concepts_.find(key) != concepts_.end();
}

std::vector<std::string> NeuralSemanticMemory::getConceptKeys() const {
    std::vector<std::string> keys;
    keys.reserve(concepts_.size());
    for (const auto& key : concepts_) {
        keys.push_back(key);
    }
    return keys;
}

float NeuralSemanticMemory::semanticSimilarity(const std::string& conceptA,
                                              const std::string& conceptB) const {
    auto itA = conceptPatterns_.find(conceptA);
    auto itB = conceptPatterns_.find(conceptB);
    
    if (itA == conceptPatterns_.end() || itB == conceptPatterns_.end()) {
        return 0.0f;
    }
    
    return computeSemanticDistance(itA->second, itB->second);
}

void NeuralSemanticMemory::storeRelation(const std::string& subject,
                                       const std::string& predicate,
                                       const std::string& object,
                                       float strength) {
    if (!hasConcept(subject) || !hasConcept(predicate) || !hasConcept(object)) {
        NLM_LOG_WARNING("Cannot store relation - missing concepts: " + subject + " -> " + predicate + " -> " + object);
        return;
    }
    
    // Check if relation already exists
    for (const auto& rel : relations_) {
        if (std::get<0>(rel) == subject &&
            std::get<1>(rel) == predicate &&
            std::get<2>(rel) == object) {
            return;  // Already exists
        }
    }
    
    // Store relation
    relations_.emplace_back(subject, predicate, object);
    subjectToObjects_[subject].push_back(object);
    objectToSubjects_[object].push_back(subject);
    subjectToPredicates_[subject].push_back(predicate);
    predicateToObjects_[predicate].push_back(object);
    
    NLM_LOG_DEBUG("Stored relation: " + subject + " " + predicate + " " + object);
}

std::vector<std::string> NeuralSemanticMemory::retrieveRelation(const std::string& subject,
                                                               const std::string& predicate) const {
    std::vector<std::string> objects;
    
    // Check if predicate is inverse
    bool isInverse = false;
    std::string subjectReversed;
    
    for (const auto& rel : relations_) {
        if (std::get<1>(rel) == predicate && std::get<2>(rel) == subject) {
            objects.push_back(std::get<0>(rel));
            isInverse = true;
            subjectReversed = subject;
            break;
        }
    }
    
    if (isInverse) {
        // Return subjects for the subject
        auto it = objectToSubjects_.find(subjectReversed);
        if (it != objectToSubjects_.end()) {
            for (const auto& obj : it->second) {
                objects.push_back(obj);
            }
        }
    } else {
        // Direct lookup
        auto it = subjectToObjects_.find(subject);
        if (it != subjectToObjects_.end()) {
            for (const auto& obj : it->second) {
                // Check if predicate matches
                for (const auto& rel : relations_) {
                    if (std::get<0>(rel) == subject &&
                        std::get<1>(rel) == predicate &&
                        std::get<2>(rel) == obj) {
                        objects.push_back(obj);
                        break;
                    }
                }
            }
        }
    }
    
    return objects;
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getRelationsForSubject(const std::string& subject) const {
    std::vector<std::pair<std::string, std::string>> relations;
    
    auto itPredicates = subjectToPredicates_.find(subject);
    if (itPredicates != subjectToPredicates_.end()) {
        for (const auto& predicate : itPredicates->second) {
            auto itObjects = predicateToObjects_.find(predicate);
            if (itObjects != predicateToObjects_.end()) {
                for (const auto& object : itObjects->second) {
                    // Verify this is the right subject
                    for (const auto& rel : relations_) {
                        if (std::get<0>(rel) == subject &&
                            std::get<1>(rel) == predicate &&
                            std::get<2>(rel) == object) {
                            relations.emplace_back(predicate, object);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return relations;
}

std::vector<std::pair<std::string, std::string>> NeuralSemanticMemory::getRelationsForObject(const std::string& object) const {
    std::vector<std::pair<std::string, std::string>> relations;
    
    auto itSubjects = objectToSubjects_.find(object);
    if (itSubjects != objectToSubjects_.end()) {
        for (const auto& subject : itSubjects->second) {
            auto itPredicates = subjectToPredicates_.find(subject);
            if (itPredicates != subjectToPredicates_.end()) {
                for (const auto& predicate : itPredicates->second) {
                    // Verify this is the right object
                    for (const auto& rel : relations_) {
                        if (std::get<0>(rel) == subject &&
                            std::get<1>(rel) == predicate &&
                            std::get<2>(rel) == object) {
                            relations.emplace_back(subject, predicate);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    return relations;
}

void NeuralSemanticMemory::learnFromActivity(const std::vector<float>& pattern, float strength) {
    if (brain_ && pImpl->network) {
        // Find nearest concept
        std::string nearestConcept = findNearestConcept(pattern, 0.5f);
        
        if (!nearestConcept.empty() && !conceptFrozen_[nearestConcept]) {
            // Learn from activity to refine concept representation
            std::vector<float> target(conceptPatterns_[nearestConcept].size(), 0.0f);
            pImpl->network->learn(pattern, target, strength);
            
            // Update concept strength based on activity
            conceptStrength_[nearestConcept] += strength;
            conceptActivation_[nearestConcept] += 0.1f;
            conceptUsageCount_[nearestConcept]++;
            conceptLastUsed_[nearestConcept] = pImpl->simulationTime;
        } else if (pattern.size() > 0) {
            // Create new concept from pattern
            std::string conceptName = "concept_" + std::to_string(conceptCount_++);
            storeConcept(conceptName, pattern);
        }
    }
}

void NeuralSemanticMemory::propagateKnowledge(const std::string& sourceConcept, float influenceStrength) {
    if (sourceConcept.empty() || !hasConcept(sourceConcept)) {
        return;
    }
    
    // Find related concepts through relations
    auto itRelations = getRelationsForSubject(sourceConcept);
    for (const auto& rel : itRelations) {
        // Propagate influence to object concept
        std::string objectConcept = rel.second;
        if (hasConcept(objectConcept)) {
            // Increase activation of related concept
            conceptActivation_[objectConcept] += influenceStrength * 0.1f;
            
            // Also propagate through object relations
            auto itReverseRelations = getRelationsForObject(objectConcept);
            for (const auto& rel2 : itReverseRelations) {
                std::string relatedConcept = rel2.first;
                if (hasConcept(relatedConcept) && relatedConcept != sourceConcept) {
                    conceptActivation_[relatedConcept] += influenceStrength * 0.05f;
                }
            }
        }
    }
}

std::vector<std::string> NeuralSemanticMemory::extractConceptHierarchy(const std::string& concept) const {
    buildConceptHierarchy();
    
    std::vector<std::string> hierarchy;
    
    // Get path from concept to root
    std::string current = concept;
    while (!current.empty() && conceptParent_.find(current) != conceptParent_.end()) {
        hierarchy.push_back(current);
        current = conceptParent_.at(current);
    }
    if (!current.empty()) {
        hierarchy.push_back(current);  // Root
    }
    
    // Reverse to get root to leaf
    std::reverse(hierarchy.begin(), hierarchy.end());
    return hierarchy;
}

std::vector<std::pair<std::string, float>> NeuralSemanticMemory::getConceptActivations() const {
    std::vector<std::pair<std::string, float>> activations;
    
    for (const auto& pair : conceptActivation_) {
        if (pair.second > 0.01f) {
            activations.emplace_back(pair.first, pair.second);
        }
    }
    
    // Sort by activation
    std::sort(activations.begin(), activations.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    return activations;
}

void NeuralSemanticMemory::updateConceptWeights(float learningRate) {
    for (auto& pair : conceptStrength_) {
        if (!conceptFrozen_[pair.first]) {
            // Decay unused concepts
            if (conceptUsageCount_[pair.first] == 0) {
                pair.second *= (1.0f - learningRate);
            } else {
                // Encourage used concepts
                pair.second += learningRate * 0.1f;
            }
            
            // Clamp to reasonable range
            pair.second = std::clamp(pair.second, 0.01f, 10.0f);
        }
    }
}

void NeuralSemanticMemory::clear() {
    concepts_.clear();
    conceptPatterns_.clear();
    conceptStrength_.clear();
    conceptUsageCount_.clear();
    conceptLastUsed_.clear();
    conceptCreationTime_.clear();
    conceptFrozen_.clear();
    conceptActivation_.clear();
    
    // Clear relations
    relations_.clear();
    subjectToObjects_.clear();
    objectToSubjects_.clear();
    subjectToPredicates_.clear();
    predicateToObjects_.clear();
    
    // Clear hierarchy
    conceptParent_.clear();
    conceptChildren_.clear();
    hierarchyBuilt_ = false;
    
    NLM_LOG_INFO("NeuralSemanticMemory cleared");
}

void NeuralSemanticMemory::decayConcepts(float decayRate) {
    for (auto& pair : conceptStrength_) {
        if (!conceptFrozen_[pair.first]) {
            pair.second *= (1.0f - decayRate);
        }
    }
}

void NeuralSemanticMemory::freezeConcept(const std::string& key) {
    if (concepts_.find(key) != concepts_.end()) {
        conceptFrozen_[key] = true;
    }
}

float NeuralSemanticMemory::getConceptStrength(const std::string& key) const {
    auto it = conceptStrength_.find(key);
    if (it != conceptStrength_.end()) {
        return it->second;
    }
    return 0.0f;
}

void NeuralSemanticMemory::setConceptStrength(const std::string& key, float strength) {
    if (concepts_.find(key) != concepts_.end()) {
        conceptStrength_[key] = std::clamp(strength, 0.01f, 10.0f);
    }
}

bool NeuralSemanticMemory::isActive(const std::string& key) const {
    auto it = conceptActivation_.find(key);
    return (it != conceptActivation_.end() && it->second > 0.1f);
}

void NeuralSemanticMemory::markUsed(const std::string& key) {
    if (concepts_.find(key) != concepts_.end()) {
        conceptUsageCount_[key]++;
        conceptLastUsed_[key] = pImpl->simulationTime;
        conceptActivation_[key] += 0.1f;
    }
}

std::vector<std::pair<std::string, float>> NeuralSemanticMemory::getSimilarConcepts(const std::string& concept,
                                                                                    size_t maxResults) const {
    std::vector<std::pair<std::string, float>> similar;
    
    auto itPattern = conceptPatterns_.find(concept);
    if (itPattern == conceptPatterns_.end()) {
        return similar;
    }
    
    for (const auto& pair : conceptPatterns_) {
        if (pair.first == concept) {
            continue;
        }
        
        float similarity = computeSemanticDistance(itPattern->second, pair.second);
        similar.emplace_back(pair.first, similarity);
    }
    
    // Sort by similarity (descending)
    std::sort(similar.begin(), similar.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Limit results
    if (similar.size() > maxResults) {
        similar.resize(maxResults);
    }
    
    return similar;
}

NeuralSemanticMemory::ConceptStats NeuralSemanticMemory::getConceptStats(const std::string& key) const {
    ConceptStats stats;
    
    stats.frequency = conceptUsageCount_[key];
    stats.lastUsed = conceptLastUsed_[key];
    stats.activation = conceptActivation_[key];
    stats.strength = conceptStrength_[key];
    stats.creationTime = conceptCreationTime_[key];
    
    return stats;
}

void NeuralSemanticMemory::decayConcept(const std::string& key, float timeFactor) {
    if (!conceptFrozen_[key]) {
        // Decay strength based on age and time factor
        auto it = conceptCreationTime_.find(key);
        if (it != conceptCreationTime_.end()) {
            float age = static_cast<float>(pImpl->simulationTime - it->second);
            float decayAmount = timeFactor * (age / 10000.0f);
            conceptStrength_[key] *= (1.0f - decayAmount);
            
            // Also decay activation
            conceptActivation_[key] *= (1.0f - decayAmount);
        }
    }
}

void NeuralSemanticMemory::createConceptNeuron(const std::string& key, const std::vector<float>& pattern) {
    // Simple implementation: store pattern directly
    // In a real implementation, this would create an actual neural population
    conceptNeurons_[key] = 0;  // Placeholder - would be NeuronId in real implementation
}

std::string NeuralSemanticMemory::findNearestConcept(const std::vector<float>& pattern, float threshold) const {
    if (pattern.empty() || concepts_.empty()) {
        return "";
    }
    
    std::string bestMatch;
    float bestDistance = 1.0f;  // Max distance
    
    for (const auto& pair : conceptPatterns_) {
        float distance = computeSemanticDistance(pattern, pair.second);
        
        if (distance < bestDistance) {
            bestDistance = distance;
            bestMatch = pair.first;
        }
    }
    
    if (bestDistance < threshold) {
        return bestMatch;
    }
    
    return "";
}

float NeuralSemanticMemory::computeSemanticDistance(const std::vector<float>& patternA,
                                                    const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) {
        return 1.0f;
    }
    
    size_t minSize = std::min(patternA.size(), patternB.size());
    if (minSize == 0) {
        return 1.0f;
    }
    
    float sumSq = 0.0f;
    for (size_t i = 0; i < minSize; ++i) {
        float diff = patternA[i] - patternB[i];
        sumSq += diff * diff;
    }
    
    return std::sqrt(sumSq);
}

void NeuralSemanticMemory::propagateActivation(const std::string& sourceConcept, float strength) {
    // Simplified implementation
    if (sourceConcept.empty() || !hasConcept(sourceConcept)) {
        return;
    }
    
    // Increase activation of related concepts
    auto itRelations = getRelationsForSubject(sourceConcept);
    for (const auto& rel : itRelations) {
        std::string objectConcept = rel.second;
        if (hasConcept(objectConcept)) {
            conceptActivation_[objectConcept] += strength * 0.1f;
        }
    }
}

void NeuralSemanticMemory::buildConceptHierarchy() {
    if (hierarchyBuilt_) {
        return;
    }
    
    // Clear existing hierarchy
    conceptParent_.clear();
    conceptChildren_.clear();
    
    if (concepts_.size() <= 1) {
        hierarchyBuilt_ = true;
        return;
    }
    
    // Create simple hierarchy based on usage
    std::vector<std::string> keys;
    keys.reserve(concepts_.size());
    for (const auto& key : concepts_) {
        keys.push_back(key);
    }
    
    // Sort by usage to create hierarchy (most used -> least used)
    std::sort(keys.begin(), keys.end(),
              [this](const std::string& a, const std::string& b) {
                  return conceptUsageCount_[a] > conceptUsageCount_[b];
              });
    
    // Create hierarchy: first concept is root, rest are children
    if (!keys.empty()) {
        std::string root = keys[0];
        
        for (size_t i = 1; i < keys.size(); ++i) {
            conceptParent_[keys[i]] = root;
            conceptChildren_[root].push_back(keys[i]);
        }
    }
    
    hierarchyBuilt_ = true;
}

} // namespace nlm
