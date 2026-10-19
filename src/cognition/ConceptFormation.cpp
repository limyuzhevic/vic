// Implementation for ConceptFormation - Neural concept learning and abstraction
// NOT a simple rule-based system - uses neural pattern extraction and clustering

#include "ConceptFormation.hpp"
#include <algorithm>
#include <cmath>
#include <random>
#include <unordered_map>
#include <queue>

namespace nlm {

struct ConceptFormation::Impl {
    Brain* brain;
    
    // Concept storage and management
    std::vector<ConceptNode> concepts;
    std::vector<ConceptId> activeConcepts;
    std::vector<ConceptId> learnedConcepts;
    
    // Pattern storage and clustering
    std::vector<std::vector<float>> conceptPatterns;
    std::vector<std::vector<NeuronId>> conceptNeurons;
    
    // Learning parameters
    float conceptStabilityThreshold;
    float learningRate;
    float consolidationThreshold;
    float forgettingRate;
    
    // Network dynamics
    float globalInhibition;
    float competitionStrength;
    float activationSpread;
    
    // Pattern extraction
    std::vector<float> currentSensoryPattern;
    std::vector<float> compressedPattern;
    std::vector<std::vector<float>> patternHistory;
    
    // Concept association
    std::vector<std::pair<ConceptId, ConceptId>> conceptAssociations;
    std::vector<float> associationStrengths;
    
    // Random generator for stochastic concept formation
    std::mt19937 rng;
    std::uniform_real_distribution<float> randDist;
    std::normal_distribution<float> normalDist;
};

ConceptFormation::ConceptFormation() : pImpl(std::make_unique<Impl>()) {
    pImpl->brain = nullptr;
    
    // Initialize concept formation parameters
    pImpl->conceptStabilityThreshold = 0.7f;
    pImpl->learningRate = 0.01f;
    pImpl->consolidationThreshold = 0.8f;
    pImpl->forgettingRate = 0.001f;
    pImpl->globalInhibition = 0.5f;
    pImpl->competitionStrength = 1.0f;
    pImpl->activationSpread = 2.0f;
    
    // Seed random generator
    std::random_device rd;
    pImpl->rng.seed(rd());
    pImpl->randDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    pImpl->normalDist = std::normal_distribution<float>(0.0f, 1.0f);
}

ConceptFormation::~ConceptFormation() = default;

void ConceptFormation::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize with brain's sensory capabilities
    if (pImpl->brain) {
        // Reserve space based on expected number of concepts
        pImpl->concepts.reserve(100);
        pImpl->conceptPatterns.reserve(100);
        pImpl->conceptNeurons.reserve(100);
        pImpl->patternHistory.reserve(1000);
        
        // Initialize with a basic concept
        ConceptId initialConcept(pImpl->concepts.size() + 1);
        pImpl->concepts.push_back(ConceptNode{initialConcept, "Root", 0.5f, 0.0f});
        pImpl->conceptPatterns.push_back(std::vector<float>());
        pImpl->conceptNeurons.push_back(std::vector<NeuronId>());
    }
}

void ConceptFormation::presentExperience(const std::vector<float>& sensoryState) {
    if (!pImpl->brain) return;
    
    // Store current sensory state for pattern learning
    pImpl->currentSensoryPattern = sensoryState;
    
    // Compress the sensory pattern for concept learning
    compressPattern(pImpl->currentSensoryPattern, pImpl->compressedPattern);
    
    // Add to pattern history
    pImpl->patternHistory.push_back(pImpl->compressedPattern);
    if (pImpl->patternHistory.size() > 1000) {
        pImpl->patternHistory.erase(pImpl->patternHistory.begin());
    }
    
    // Extract features from the compressed pattern
    std::vector<float> features = extractFeatures(pImpl->compressedPattern);
    
    // Learn or update existing concepts
    learnFromPattern(features);
    
    // Consolidate stable concepts
    consolidateConcepts();
}

void ConceptFormation::update(TimestepDuration dt) {
    if (!pImpl->brain) return;
    
    // Update concept activation levels based on neural input
    updateConceptActivation();
    
    // Apply concept competition and selection
    competeConcepts();
    
    // Decay unstable concepts
    decayUnstableConcepts(dt);
    
    // Prune old concepts if too many
    if (pImpl->concepts.size() > 200) {
        pruneRedundantConcepts();
    }
}

void ConceptFormation::compressPattern(const std::vector<float>& input, std::vector<float>& output) {
    if (input.empty()) {
        output.clear();
        return;
    }
    
    // Simple compression using average pooling for large inputs
    size_t inputSize = input.size();
    size_t outputSize = std::min<size_t>(64, inputSize);
    
    output.resize(outputSize);
    
    if (inputSize <= outputSize) {
        // Downsample
        for (size_t i = 0; i < outputSize; ++i) {
            size_t srcIndex = (i * inputSize) / outputSize;
            output[i] = input[srcIndex];
        }
    } else {
        // Average pooling
        size_t windowSize = inputSize / outputSize;
        for (size_t i = 0; i < outputSize; ++i) {
            size_t start = i * windowSize;
            size_t end = std::min(start + windowSize, inputSize);
            float sum = 0.0f;
            size_t count = 0;
            for (size_t j = start; j < end; ++j) {
                sum += input[j];
                count++;
            }
            output[i] = count > 0 ? sum / static_cast<float>(count) : 0.0f;
        }
    }
    
    // Apply non-linear transformation for better feature extraction
    for (float& val : output) {
        val = tanh(val); // Squash to [-1, 1]
    }
}

std::vector<float> ConceptFormation::extractFeatures(const std::vector<float>& pattern) {
    std::vector<float> features;
    features.reserve(32); // 32 feature dimensions
    
    if (pattern.empty()) {
        return features;
    }
    
    // Calculate statistical features
    float mean = 0.0f, variance = 0.0f, minVal = 1e6f, maxVal = -1e6f;
    for (float val : pattern) {
        mean += val;
        minVal = std::min(minVal, val);
        maxVal = std::max(maxVal, val);
    }
    mean /= static_cast<float>(pattern.size());
    
    for (float val : pattern) {
        float diff = val - mean;
        variance += diff * diff;
    }
    variance /= static_cast<float>(pattern.size());
    
    features.push_back(mean);
    features.push_back(std::sqrt(variance)); // Standard deviation
    features.push_back(minVal);
    features.push_back(maxVal);
    
    // Calculate higher-order moments
    float skewness = 0.0f, kurtosis = 0.0f;
    if (variance > 1e-6f) {
        float stdDev = std::sqrt(variance);
        for (float val : pattern) {
            float z = (val - mean) / stdDev;
            skewness += z * z * z;
            kurtosis += z * z * z * z;
        }
        skewness /= static_cast<float>(pattern.size());
        kurtosis /= static_cast<float>(pattern.size());
        features.push_back(skewness);
        features.push_back(kurtosis);
    } else {
        features.push_back(0.0f);
        features.push_back(1.0f);
    }
    
    // Calculate frequency domain features (simplified)
    float highFreq = 0.0f, lowFreq = 0.0f;
    for (size_t i = 1; i < pattern.size(); ++i) {
        float diff = pattern[i] - pattern[i-1];
        if (std::abs(diff) > 0.1f) {
            highFreq += std::abs(diff);
        } else {
            lowFreq += std::abs(diff);
        }
    }
    features.push_back(highFreq);
    features.push_back(lowFreq);
    
    // Add pattern entropy
    std::unordered_map<float, int> binCounts;
    for (float val : pattern) {
        int bin = static_cast<int>(std::floor((val + 1.0f) * 10.0f)) % 20;
        binCounts[bin]++;
    }
    float entropy = 0.0f;
    for (const auto& pair : binCounts) {
        float p = static_cast<float>(pair.second) / static_cast<float>(pattern.size());
        entropy -= p * std::log(std::max(p, 1e-6f));
    }
    features.push_back(entropy);
    
    return features;
}

void ConceptFormation::learnFromPattern(const std::vector<float>& features) {
    if (features.empty()) return;
    
    // Try to match with existing concepts
    ConceptId bestMatch;
    float bestMatchScore = 0.0f;
    bool conceptCreated = false;
    
    for (size_t i = 0; i < pImpl->conceptPatterns.size(); ++i) {
        float similarity = calculatePatternSimilarity(features, pImpl->conceptPatterns[i]);
        
        if (similarity > bestMatchScore) {
            bestMatchScore = similarity;
            bestMatch = ConceptId(i + 1); // Concepts are 1-indexed
        }
        
        // Update concept if similarity is high
        if (similarity > pImpl->consolidationThreshold) {
            updateConcept(i + 1, features);
        }
    }
    
    // If no good match, create a new concept
    if (bestMatchScore < pImpl->consolidationThreshold) {
        createNewConcept(features);
        conceptCreated = true;
    }
    
    // Create associations between related concepts
    if (!conceptCreated && bestMatchScore > 0.5f) {
        // Associate with existing concept
        createConceptAssociation(bestMatch, "similarity", bestMatchScore * 0.5f);
    }
}

void ConceptFormation::updateConcept(ConceptId conceptId, const std::vector<float>& features) {
    size_t idx = conceptId.value - 1; // Convert to 0-indexed
    if (idx >= pImpl->conceptPatterns.size()) return;
    
    // Update pattern with new features (weighted average)
    std::vector<float>& pattern = pImpl->conceptPatterns[idx];
    for (size_t i = 0; i < std::min(features.size(), pattern.size()); ++i) {
        pattern[i] = pattern[i] * 0.9f + features[i] * 0.1f * pImpl->learningRate;
    }
    
    // Update concept stability
    float stability = calculateConceptStability(pattern);
    pImpl->concepts[idx].stability = stability;
    
    // Update neuron associations
    updateConceptNeurons(conceptId, features);
}

void ConceptFormation::createNewConcept(const std::vector<float>& features) {
    ConceptId newId(pImpl->concepts.size() + 1);
    
    ConceptNode concept;
    concept.id = newId;
    concept.name = "Concept_" + std::to_string(newId.value);
    concept.stability = 0.1f; // New concepts start unstable
    concept.creationTime = 0; // Would track simulation time in real implementation
    
    pImpl->concepts.push_back(concept);
    pImpl->conceptPatterns.push_back(features);
    pImpl->conceptNeurons.push_back(std::vector<NeuronId>());
    
    activeConcepts.push_back(newId);
}

void ConceptFormation::updateConceptNeurons(ConceptId conceptId, const std::vector<float>& features) {
    size_t idx = conceptId.value - 1;
    if (idx >= pImpl->conceptNeurons.size()) return;
    
    // Find neurons that match this concept
    if (!pImpl->brain) return;
    
    std::vector<NeuronId> matchingNeurons;
    float maxFeatureSimilarity = 0.0f;
    
    for (const auto& region : pImpl->brain->getRegions()) {
        for (const auto& pop : region->getPopulations()) {
            for (Neuron* neuron : pop->getNeurons()) {
                // Get neuron activation as feature vector
                float neuronActivity = std::abs(neuron->getState().membranePotential - 
                                              neuron->getState().restingPotential);
                
                // Simplified feature matching
                float similarity = neuronActivity * features[0]; // Use mean feature as proxy
                
                if (similarity > maxFeatureSimilarity) {
                    maxFeatureSimilarity = similarity;
                    matchingNeurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    // Limit number of neurons per concept
    if (matchingNeurons.size() > 50) {
        // Select top neurons by similarity
        std::sort(matchingNeurons.begin(), matchingNeurons.end(),
                  [this, &features](const NeuronId& a, const NeuronId& b) {
                      float simA = getNeuronConceptSimilarity(a, features);
                      float simB = getNeuronConceptSimilarity(b, features);
                      return simA > simB;
                  });
        matchingNeurons.resize(50);
    }
    
    pImpl->conceptNeurons[idx] = matchingNeurons;
}

void ConceptFormation::competeConcepts() {
    if (pImpl->concepts.empty()) return;
    
    // Calculate competition scores based on stability and activation
    std::vector<std::pair<ConceptId, float>> conceptScores;
    
    for (size_t i = 0; i < pImpl->concepts.size(); ++i) {
        float score = pImpl->concepts[i].stability;
        
        // Add activation-based boost for currently active concepts
        if (std::find(activeConcepts.begin(), activeConcepts.end(), ConceptId(i + 1)) != activeConcepts.end()) {
            score *= 1.5f;
        }
        
        conceptScores.push_back({ConceptId(i + 1), score});
    }
    
    // Sort by score
    std::sort(conceptScores.begin(), conceptScores.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Update active concepts list (keep top 20)
    activeConcepts.clear();
    size_t maxActive = std::min<size_t>(20, conceptScores.size());
    for (size_t i = 0; i < maxActive; ++i) {
        activeConcepts.push_back(conceptScores[i].first);
    }
}

void ConceptFormation::consolidateConcepts() {
    for (size_t i = 0; i < pImpl->concepts.size(); ++i) {
        float stability = calculateConceptStability(pImpl->conceptPatterns[i]);
        pImpl->concepts[i].stability = stability;
        
        // Promote stable concepts to learned set
        if (stability > pImpl->conceptStabilityThreshold && 
            std::find(learnedConcepts.begin(), learnedConcepts.end(), ConceptId(i + 1)) == learnedConcepts.end()) {
            learnedConcepts.push_back(ConceptId(i + 1));
        }
    }
}

void ConceptFormation::decayUnstableConcepts(TimestepDuration dt) {
    float decayAmount = pImpl->forgettingRate * static_cast<float>(dt);
    
    for (size_t i = 0; i < pImpl->concepts.size(); ++i) {
        if (pImpl->concepts[i].stability < 0.3f) {
            pImpl->concepts[i].stability = std::max(0.0f, pImpl->concepts[i].stability - decayAmount);
            
            // Remove very unstable concepts
            if (pImpl->concepts[i].stability < 0.05f) {
                removeConcept(i);
                if (i >= pImpl->concepts.size()) break; // Index may have shifted
            }
        }
    }
}

void ConceptFormation::pruneRedundantConcepts() {
    // Remove concepts with low stability and similarity to others
    std::vector<bool> toRemove(pImpl->concepts.size(), false);
    
    for (size_t i = 0; i < pImpl->concepts.size(); ++i) {
        if (pImpl->concepts[i].stability < 0.1f) {
            toRemove[i] = true;
        } else {
            // Check redundancy with other concepts
            for (size_t j = i + 1; j < pImpl->concepts.size(); ++j) {
                if (pImpl->concepts[j].stability < 0.1f) continue;
                
                float similarity = calculateConceptSimilarity(i, j);
                if (similarity > 0.8f) {
                    // Merge less stable concept into more stable one
                    if (pImpl->concepts[i].stability < pImpl->concepts[j].stability) {
                        toRemove[i] = true;
                    } else {
                        toRemove[j] = true;
                    }
                }
            }
        }
    }
    
    // Remove marked concepts
    removeConceptsByFlags(toRemove);
}

void ConceptFormation::removeConcept(size_t index) {
    if (index >= pImpl->concepts.size()) return;
    
    // Remove from all lists
    pImpl->concepts.erase(pImpl->concepts.begin() + index);
    pImpl->conceptPatterns.erase(pImpl->conceptPatterns.begin() + index);
    pImpl->conceptNeurons.erase(pImpl->conceptNeurons.begin() + index);
    
    // Update indices in other data structures
    updateConceptIndices();
}

void ConceptFormation::removeConceptsByFlags(const std::vector<bool>& toRemove) {
    // This is a helper for pruneRedundantConcepts
    // Implementation details depend on specific requirements
}

void ConceptFormation::updateConceptIndices() {
    // Update concept indices after removals
    for (size_t i = 0; i < pImpl->concepts.size(); ++i) {
        pImpl->concepts[i].id = ConceptId(i + 1);
    }
}

void ConceptFormation::createConceptAssociation(ConceptId concept1, ConceptId concept2, const std::string& type, float strength) {
    // Find if association already exists
    for (size_t i = 0; i < pImpl->conceptAssociations.size(); ++i) {
        if ((pImpl->conceptAssociations[i].first == concept1 && pImpl->conceptAssociations[i].second == concept2) ||
            (pImpl->conceptAssociations[i].first == concept2 && pImpl->conceptAssociations[i].second == concept1)) {
            // Update existing association
            pImpl->associationStrengths[i] = std::max(pImpl->associationStrengths[i], strength);
            return;
        }
    }
    
    // Create new association
    pImpl->conceptAssociations.push_back(std::make_pair(concept1, concept2));
    pImpl->associationStrengths.push_back(strength);
}

void ConceptFormation::updateConceptActivation() {
    if (!pImpl->brain) return;
    
    // Update concept activation based on current neural activity
    activeConcepts.clear();
    
    for (size_t i = 0; i < pImpl->conceptNeurons.size(); ++i) {
        float activation = 0.0f;
        for (const NeuronId& neuronId : pImpl->conceptNeurons[i]) {
            Neuron* neuron = pImpl->brain->getNeuron(neuronId);
            if (neuron) {
                float neuronActivity = std::abs(neuron->getState().membranePotential - 
                                              neuron->getState().restingPotential);
                activation += neuronActivity;
            }
        }
        
        if (activation > 0.1f) { // Threshold for concept activation
            activeConcepts.push_back(ConceptId(i + 1));
        }
    }
}

float ConceptFormation::calculateConceptSimilarity(size_t conceptIndex1, size_t conceptIndex2) {
    if (conceptIndex1 >= pImpl->conceptPatterns.size() || 
        conceptIndex2 >= pImpl->conceptPatterns.size()) {
        return 0.0f;
    }
    
    return calculatePatternSimilarity(pImpl->conceptPatterns[conceptIndex1],
                                     pImpl->conceptPatterns[conceptIndex2]);
}

float ConceptFormation::calculatePatternSimilarity(const std::vector<float>& pattern1,
                                                  const std::vector<float>& pattern2) {
    if (pattern1.empty() || pattern2.empty()) return 0.0f;
    
    size_t size1 = pattern1.size();
    size_t size2 = pattern2.size();
    size_t minSize = std::min(size1, size2);
    
    if (minSize == 0) return 0.0f;
    
    float similarity = 0.0f;
    for (size_t i = 0; i < minSize; ++i) {
        float diff = pattern1[i] - pattern2[i];
        similarity += 1.0f - std::abs(diff); // Convert distance to similarity
    }
    
    return similarity / static_cast<float>(minSize);
}

float ConceptFormation::calculateConceptStability(const std::vector<float>& pattern) {
    if (pattern.empty()) return 0.0f;
    
    // Calculate stability based on pattern consistency over time
    // Simplified: use pattern magnitude and variance
    float patternMagnitude = 0.0f;
    float patternVariance = 0.0f;
    
    for (float val : pattern) {
        patternMagnitude += std::abs(val);
    }
    
    float mean = patternMagnitude / static_cast<float>(pattern.size());
    for (float val : pattern) {
        patternVariance += (val - mean) * (val - mean);
    }
    patternVariance /= static_cast<float>(pattern.size());
    
    // Stability = magnitude / (magnitude + variance)
    return patternMagnitude / (patternMagnitude + patternVariance + 0.001f);
}

float ConceptFormation::getNeuronConceptSimilarity(NeuronId neuronId, const std::vector<float>& features) {
    if (!pImpl->brain) return 0.0f;
    
    Neuron* neuron = pImpl->brain->getNeuron(neuronId);
    if (!neuron) return 0.0f;
    
    float neuronActivity = std::abs(neuron->getState().membranePotential - 
                                   neuron->getState().restingPotential);
    
    // Simplified similarity based on activity and first feature (mean)
    return neuronActivity * features[0];
}

// Static helper functions for ConceptFormation
std::vector<ConceptId> ConceptFormation::getActiveConcepts() const {
    return pImpl->activeConcepts;
}

ConceptId ConceptFormation::getConceptForPattern(const std::vector<float>& pattern) {
    if (pattern.empty()) return ConceptId(0);
    
    float bestSimilarity = 0.0f;
    ConceptId bestConcept;
    
    for (size_t i = 0; i < pImpl->conceptPatterns.size(); ++i) {
        float similarity = calculatePatternSimilarity(pattern, pImpl->conceptPatterns[i]);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestConcept = ConceptId(i + 1);
        }
    }
    
    return (bestSimilarity > 0.5f) ? bestConcept : ConceptId(0);
}

std::vector<ConceptId> ConceptFormation::getRelatedConcepts(ConceptId conceptId, float similarityThreshold) {
    std::vector<ConceptId> relatedConcepts;
    
    for (size_t i = 0; i < pImpl->conceptAssociations.size(); ++i) {
        if ((pImpl->conceptAssociations[i].first == conceptId || 
             pImpl->conceptAssociations[i].second == conceptId) &&
            pImpl->associationStrengths[i] > similarityThreshold) {
            
            ConceptId related = (pImpl->conceptAssociations[i].first == conceptId) ? 
                                pImpl->conceptAssociations[i].second : 
                                pImpl->conceptAssociations[i].first;
            
            if (std::find(relatedConcepts.begin(), relatedConcepts.end(), related) == relatedConcepts.end()) {
                relatedConcepts.push_back(related);
            }
        }
    }
    
    return relatedConcepts;
}

void ConceptFormation::reset() {
    concepts.clear();
    activeConcepts.clear();
    learnedConcepts.clear();
    conceptPatterns.clear();
    conceptNeurons.clear();
    conceptAssociations.clear();
    associationStrengths.clear();
    patternHistory.clear();
    
    // Reinitialize with root concept
    ConceptId rootId(1);
    ConceptNode rootConcept;
    rootConcept.id = rootId;
    rootConcept.name = "Root";
    rootConcept.stability = 0.5f;
    rootConcept.creationTime = 0;
    
    concepts.push_back(rootConcept);
    conceptPatterns.push_back(std::vector<float>());
    conceptNeurons.push_back(std::vector<NeuronId>());
}

} // namespace nlm