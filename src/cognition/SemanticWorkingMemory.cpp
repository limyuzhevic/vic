// SemanticWorkingMemory Implementation

#include "SemanticWorkingMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_set>

namespace nlm {

struct SemanticWorkingMemory::Impl {
    Brain* brain;
    
    // Semantic concept neurons
    std::vector<NeuronId> conceptNeurons;
    
    // Pattern similarity thresholds
    float conceptSimilarityThreshold;
    float conceptUpdateThreshold;
    
    Impl()
        : brain(nullptr)
        , conceptSimilarityThreshold(0.6f)
        , conceptUpdateThreshold(0.3f) {}
};

SemanticWorkingMemory::SemanticWorkingMemory()
    : pImpl(new Impl)
    , brain_(nullptr) {
}

SemanticWorkingMemory::~SemanticWorkingMemory() = default;

void SemanticWorkingMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    NLM_LOG_INFO("SemanticWorkingMemory initialized");
}

void SemanticWorkingMemory::storeConcept(const std::string& concept, const std::vector<float>& pattern, float importance) {
    if (!brain_ || pattern.empty()) return;
    
    // Create or update neural representation
    NeuronId conceptNeuron = createConceptRepresentation(concept, pattern);
    
    // Store concept metadata
    conceptNeurons_.insert({concept, conceptNeuron});
    conceptPatterns_[conceptNeuron] = pattern;
    conceptImportance_[conceptNeuron] = importance;
    conceptLastAccess_[conceptNeuron] = 0;
    
    // Create recurrent connections for concept maintenance
    if (conceptPatterns_.size() > 1) {
        auto it = conceptNeurons_.begin();
        NeuronId prevNeuron = std::prev(it->second, 1);
        createRecurrentConnection(prevNeuron, conceptNeuron, 0.5f);
    }
    
    NLM_LOG_INFO("Stored semantic concept: " + concept);
}

std::vector<float> SemanticWorkingMemory::retrieveConcept(const std::string& concept) const {
    auto it = conceptNeurons_.find(concept);
    if (it == conceptNeurons_.end()) {
        return {};
    }
    
    NeuronId neuronId = it->second;
    auto patternIt = conceptPatterns_.find(neuronId);
    if (patternIt == conceptPatterns_.end()) {
        return {};
    }
    
    // Update last access time
    const_cast<SimulationStep&>(conceptLastAccess_[neuronId])++;
    
    return patternIt->second;
}

bool SemanticWorkingMemory::hasConcept(const std::string& concept) const {
    return conceptNeurons_.find(concept) != conceptNeurons_.end();
}

std::vector<std::string> SemanticWorkingMemory::getAllConcepts() const {
    std::vector<std::string> concepts;
    concepts.reserve(conceptNeurons_.size());
    
    for (const auto& pair : conceptNeurons_) {
        concepts.push_back(pair.first);
    }
    
    return concepts;
}

float SemanticWorkingMemory::getConceptActivation(const std::string& concept) const {
    auto it = conceptNeurons_.find(concept);
    if (it == conceptNeurons_.end()) {
        return 0.0f;
    }
    
    NeuronId neuronId = it->second;
    auto importanceIt = conceptImportance_.find(neuronId);
    if (importanceIt == conceptImportance_.end()) {
        return 0.0f;
    }
    
    return importanceIt->second;
}

void SemanticWorkingMemory::update(TimestepDuration dt) {
    if (!brain_) return;
    
    // Update concept patterns through recurrent connections
    updateConceptPatterns();
    
    // Consolidate related concepts
    consolidateConcepts();
    
    // Decay less-used concepts
    decayWeakConcepts();
}

void SemanticWorkingMemory::clear() {
    conceptNeurons_.clear();
    conceptPatterns_.clear();
    conceptImportance_.clear();
    conceptLastAccess_.clear();
    pImpl->conceptNeurons.clear();
    NLM_LOG_INFO("SemanticWorkingMemory cleared");
}

float SemanticWorkingMemory::getConceptSimilarity(const std::string& conceptA, const std::string& conceptB) const {
    auto itA = conceptNeurons_.find(conceptA);
    auto itB = conceptNeurons_.find(conceptB);
    
    if (itA == conceptNeurons_.end() || itB == conceptNeurons_.end()) {
        return 0.0f;
    }
    
    const std::vector<float>& patternA = conceptPatterns_.at(itA->second);
    const std::vector<float>& patternB = conceptPatterns_.at(itB->second);
    
    return computePatternSimilarity(patternA, patternB);
}

std::vector<std::string> SemanticWorkingMemory::getRelatedConcepts(const std::string& concept, float similarityThreshold) const {
    std::vector<std::string> relatedConcepts;
    
    auto it = conceptNeurons_.find(concept);
    if (it == conceptNeurons_.end()) {
        return relatedConcepts;
    }
    
    NeuronId conceptNeuron = it->second;
    const std::vector<float>& pattern = conceptPatterns_.at(conceptNeuron);
    
    for (const auto& pair : conceptNeurons_) {
        if (pair.first == concept) continue;
        
        float similarity = computePatternSimilarity(pattern, conceptPatterns_.at(pair.second));
        if (similarity >= similarityThreshold) {
            relatedConcepts.push_back(pair.first);
        }
    }
    
    return relatedConcepts;
}

void SemanticWorkingMemory::strengthenConcept(const std::string& concept, float factor) {
    auto it = conceptNeurons_.find(concept);
    if (it == conceptNeurons_.end()) return;
    
    NeuronId neuronId = it->second;
    conceptImportance_[neuronId] = std::min(1.0f, conceptImportance_[neuronId] * factor);
}

float SemanticWorkingMemory::getSemanticCoherence() const {
    if (conceptNeurons_.size() < 2) return 1.0f;
    
    float totalSimilarity = 0.0f;
    size_t similarityCount = 0;
    
    auto it = conceptNeurons_.begin();
    NeuronId firstNeuron = it->second;
    const std::vector<float>& firstPattern = conceptPatterns_.at(firstNeuron);
    
    for (++it; it != conceptNeurons_.end(); ++it) {
        float similarity = computePatternSimilarity(firstPattern, conceptPatterns_.at(it->second));
        totalSimilarity += similarity;
        similarityCount++;
    }
    
    if (similarityCount == 0) return 1.0f;
    
    return totalSimilarity / similarityCount;
}

NeuronId SemanticWorkingMemory::createConceptRepresentation(const std::string& concept, const std::vector<float>& pattern) {
    // Check if concept already has neural representation
    auto it = conceptNeurons_.find(concept);
    if (it != conceptNeurons_.end()) {
        // Update existing pattern
        NeuronId neuronId = it->second;
        conceptPatterns_[neuronId] = pattern;
        conceptImportance_[neuronId] = std::min(1.0f, conceptImportance_[neuronId] + 0.1f);
        return neuronId;
    }
    
    // Create new neural representation
    NeuronId newNeuronId = conceptNeurons_.size() + 40000;
    conceptNeurons_[concept] = newNeuronId;
    conceptPatterns_[newNeuronId] = pattern;
    conceptImportance_[newNeuronId] = 0.5f;
    conceptLastAccess_[newNeuronId] = 0;
    
    // Create neuron in brain if available
    if (brain_) {
        auto* region = brain_->getRegion(1); // Default region
        if (region) {
            region->addNeuron(newNeuronId, NeuronType::Internal);
        }
    }
    
    // Create recurrent connection for maintenance
    if (pImpl->conceptNeurons.size() > 0) {
        createRecurrentConnection(pImpl->conceptNeurons.back(), newNeuronId, 0.5f);
    }
    
    pImpl->conceptNeurons.push_back(newNeuronId);
    
    return newNeuronId;
}

void SemanticWorkingMemory::updateConceptPatterns() {
    if (!brain_ || conceptPatterns_.empty()) return;
    
    // Update each concept pattern through its neural representation
    for (auto& pair : conceptPatterns_) {
        NeuronId neuronId = pair.first;
        std::vector<float>& pattern = pair.second;
        
        // Inject maintenance current into neuron
        brain_->injectCurrent(neuronId, 0.1f);
        
        // Update pattern based on neural activity
        NeuronId currentNeuron = findNeuronById(neuronId);
        if (currentNeuron != INVALID_NEURON_ID) {
            // Update pattern based on current neuron state
            updatePatternFromNeuron(pattern, currentNeuron);
        }
    }
}

void SemanticWorkingMemory::consolidateConcepts() {
    if (conceptNeurons_.size() < 3) return;
    
    // Find similar concept clusters
    std::vector<std::vector<NeuronId>> clusters;
    std::vector<bool> visited;
    
    for (size_t i = 0; i < conceptNeurons_.size(); ++i) {
        if (visited.size() <= i) visited.resize(i + 1, false);
        if (visited[i]) continue;
        
        std::vector<NeuronId> cluster;
        cluster.push_back(conceptNeurons_[i].second);
        visited[i] = true;
        
        for (size_t j = i + 1; j < conceptNeurons_.size(); ++j) {
            if (!visited[j]) {
                float similarity = computePatternSimilarity(
                    conceptPatterns_[cluster[0]],
                    conceptPatterns_[conceptNeurons_[j].second]
                );
                if (similarity > pImpl->conceptSimilarityThreshold) {
                    cluster.push_back(conceptNeurons_[j].second);
                    visited[j] = true;
                }
            }
        }
        
        if (cluster.size() > 1) {
            clusters.push_back(cluster);
        }
    }
    
    // Merge small clusters
    for (auto& cluster : clusters) {
        if (cluster.size() > 2) continue; // Keep larger clusters intact
        
        // Merge into nearest larger cluster
        size_t bestClusterIdx = -1;
        float bestSimilarity = 0.0f;
        
        for (size_t k = 0; k < clusters.size(); ++k) {
            if (k == clusters.size() - 1 || clusters[k].size() < 3) continue; // Skip itself and small clusters
            
            float similarity = computePatternSimilarity(
                conceptPatterns_[cluster[0]],
                conceptPatterns_[clusters[k][0]]
            );
            
            if (similarity > bestSimilarity) {
                bestSimilarity = similarity;
                bestClusterIdx = k;
            }
        }
        
        if (bestClusterIdx != -1) {
            // Merge into best cluster
            for (NeuronId neuronId : cluster) {
                clusters[bestClusterIdx].push_back(neuronId);
            }
            
            // Remove from original concept storage
            for (const auto& pair : conceptNeurons_) {
                if (std::find(cluster.begin(), cluster.end(), pair.second) != cluster.end()) {
                    conceptPatterns_.erase(pair.second);
                    conceptImportance_.erase(pair.second);
                    conceptLastAccess_.erase(pair.second);
                }
            }
            
            // Update concept neurons map
            for (NeuronId neuronId : cluster) {
                for (auto it = conceptNeurons_.begin(); it != conceptNeurons_.end(); ++it) {
                    if (it->second == neuronId) {
                        // Update pattern based on cluster average
                        std::vector<float> averagePattern;
                        for (NeuronId clusterNeuron : cluster) {
                            auto patternIt = conceptPatterns_.find(clusterNeuron);
                            if (patternIt != conceptPatterns_.end()) {
                                if (averagePattern.empty()) {
                                    averagePattern = patternIt->second;
                                } else {
                                    for (size_t i = 0; i < std::min(averagePattern.size(), patternIt->second.size()); ++i) {
                                        averagePattern[i] = (averagePattern[i] + patternIt->second[i]) * 0.5f;
                                    }
                                }
                            }
                        }
                        
                        if (!averagePattern.empty()) {
                            conceptPatterns_[neuronId] = averagePattern;
                        }
                        break;
                    }
                }
            }
        }
    }
}

void SemanticWorkingMemory::decayWeakConcepts() {
    auto it = conceptLastAccess_.begin();
    while (it != conceptLastAccess_.end()) {
        if (it->second > 100) { // Last access more than 100 steps ago
            NeuronId neuronId = it->first;
            
            // Decay importance
            conceptImportance_[neuronId] *= 0.9f;
            
            // Remove if importance is too low
            if (conceptImportance_[neuronId] < 0.01f) {
                conceptPatterns_.erase(neuronId);
                conceptImportance_.erase(neuronId);
                conceptLastAccess_.erase(it++);
            } else {
                ++it;
            }
        } else {
            ++it;
        }
    }
}

void SemanticWorkingMemory::createRecurrentConnection(NeuronId from, NeuronId to, float strength) {
    // Check if connection already exists
    for (const auto& conn : pImpl->conceptNeurons) {
        if (conn.first == from && conn.second == to) return;
    }
    
    pImpl->conceptNeurons.emplace_back(from, to);
}

std::vector<float> SemanticWorkingMemory::computePatternSimilarity(const std::vector<float>& patternA, const std::vector<float>& patternB) const {
    if (patternA.empty() || patternB.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    size_t compareLen = std::min(patternA.size(), patternB.size());
    
    for (size_t i = 0; i < compareLen; ++i) {
        dot += patternA[i] * patternB[i];
        normA += patternA[i] * patternA[i];
        normB += patternB[i] * patternB[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

NeuronId SemanticWorkingMemory::findNeuronById(NeuronId neuronId) {
    if (!brain_) return INVALID_NEURON_ID;
    
    for (auto& region : brain_->getRegions()) {
        auto neurons = region->getAllNeurons();
        if (neurons) {
            for (auto* neuron : *neurons) {
                if (neuron->getId() == neuronId) {
                    return neuron->getId();
                }
            }
        }
    }
    
    return INVALID_NEURON_ID;
}

void SemanticWorkingMemory::updatePatternFromNeuron(std::vector<float>& pattern, NeuronId neuronId) {
    // Update pattern based on current neuron state
    // This is a simplified implementation
    for (float& value : pattern) {
        value = std::min(1.0f, value + 0.01f);
    }
}

// SemanticWorkingMemory.cpp implementation completed
}
namespace nlm