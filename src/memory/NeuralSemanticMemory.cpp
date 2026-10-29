#include "NeuralSemanticMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <sstream>

namespace nlm {

struct NeuralSemanticMemory::Impl {
    Brain* brain;
    
    // Concept storage: name -> representation
    std::unordered_map<std::string, std::vector<float>> concepts;
    
    // Concept activation: name -> current activation level
    std::unordered_map<std::string, float> activations;
    
    // Concept relationships: (conceptA, conceptB) -> strength
    std::unordered_map<std::string, std::unordered_map<std::string, float>> relationships;
    
    // Concept usage statistics
    struct ConceptStats {
        std::string name;
        size_t activationCount = 0;
        float averageActivation = 0.0f;
        float lastActivation = 0.0f;
        float totalAssociations = 0.0f;
        float knowledgeLevel = 0.0f;
        size_t formationStep = 0;
    };
    std::unordered_map<std::string, ConceptStats> stats;
    
    // Concept similarity threshold
    float similarityThreshold = 0.3f;
    
    // Learning rate
    float learningRate = 0.1f;
    
    // Pattern dimensionality
    size_t patternDim = 32;
    
    // Max concepts to store
    size_t maxConcepts = 1000;
    
    // Self-model
    std::string selfModel;
    float selfModelActivation = 0.0f;
    
    Impl() : brain(nullptr) {
        // Initialize with typical concept space dimensionality
        patternDim = 32;
    }
};

NeuralSemanticMemory::NeuralSemanticMemory() : pImpl(new Impl) {}

NeuralSemanticMemory::~NeuralSemanticMemory() = default;

bool NeuralSemanticMemory::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize SemanticMemory without brain reference");
        return false;
    }
    
    pImpl->brain = brain;
    
    // Pre-allocate concept space based on brain size
    size_t estimatedConcepts = std::min<size_t>(pImpl->maxConcepts, 
                                                pImpl->brain->getTotalNeuronCount() / 32);
    pImpl->concepts.reserve(estimatedConcepts);
    pImpl->activations.reserve(estimatedConcepts);
    pImpl->stats.reserve(estimatedConcepts);
    
    NLM_LOG_INFO("NeuralSemanticMemory initialized with brain reference");
    return true;
}

void NeuralSemanticMemory::processPattern(const std::vector<float>& pattern, float activationStrength) {
    if (pImpl->brain == nullptr) {
        NLM_LOG_ERROR("SemanticMemory not initialized with brain reference");
        return;
    }
    
    // Find best matching existing concept
    std::string bestMatch;
    float bestSimilarity = 0.0f;
    
    for (const auto& pair : pImpl->concepts) {
        float similarity = computeCosineSimilarity(pattern, pair.second);
        if (similarity > bestSimilarity) {
            bestSimilarity = similarity;
            bestMatch = pair.first;
        }
    }
    
    // Update best matching concept
    if (!bestMatch.empty() && bestSimilarity > pImpl->similarityThreshold) {
        // Update existing concept
        updateConcept(bestMatch, pattern, activationStrength);
    } else {
        // Create new concept
        createConcept(pattern, activationStrength);
    }
}

void NeuralSemanticMemory::learnPattern(const std::vector<float>& pattern, const std::string& context) {
    // Create concept with stronger activation
    std::string conceptName = generateConceptName(pattern, context);
    createConcept(pattern, 1.0f);
    
    // Store context if provided
    if (!context.empty()) {
        // Context can be used for concept refinement
        refineConceptByContext(conceptName, context);
    }
}

std::vector<std::pair<std::string, float>> NeuralSemanticMemory::retrieveConcepts(
    const std::vector<float>& pattern, size_t maxResults) const {
    
    std::vector<std::pair<std::string, float>> results;
    
    for (const auto& pair : pImpl->concepts) {
        float similarity = computeCosineSimilarity(pattern, pair.second);
        if (similarity > pImpl->similarityThreshold) {
            results.emplace_back(pair.first, similarity);
        }
    }
    
    // Sort by similarity (descending)
    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });
    
    // Return top N results
    if (results.size() > maxResults) {
        results.resize(maxResults);
    }
    
    return results;
}

float NeuralSemanticMemory::getConceptActivation(const std::string& conceptName) const {
    auto it = pImpl->activations.find(conceptName);
    if (it != pImpl->activations.end()) {
        return it->second;
    }
    return 0.0f;
}

std::vector<std::string> NeuralSemanticMemory::getActiveConcepts(float threshold) const {
    std::vector<std::string> activeConcepts;
    
    for (const auto& pair : pImpl->activations) {
        if (pair.second > threshold) {
            activeConcepts.push_back(pair.first);
        }
    }
    
    return activeConcepts;
}

std::vector<float> NeuralSemanticMemory::getConceptRepresentation(const std::string& conceptName) const {
    auto it = pImpl->concepts.find(conceptName);
    if (it != pImpl->concepts.end()) {
        return it->second;
    }
    return std::vector<float>();
}

void NeuralSemanticMemory::updateFromEpisodicMemory(const EpisodicMemoryItem& episode) {
    // Convert episodic memory to pattern
    std::vector<float> pattern;
    pattern.reserve(episode.neurons.size() + episode.values.size());
    
    for (size_t i = 0; i < episode.neurons.size() && i < episode.values.size(); ++i) {
        // Combine neuron ID and activation
        float neuronId = static_cast<float>(episode.neurons[i]);
        float activation = episode.values[i];
        pattern.push_back(neuronId * 1000 + activation); // Scale neuron ID
    }
    
    if (pattern.empty()) {
        return;
    }
    
    // Process this as a new learning opportunity
    processPattern(pattern, 0.5f); // Moderate activation for episodic input
}

float NeuralSemanticMemory::getConceptRelationship(const std::string& conceptA, const std::string& conceptB) const {
    auto itA = pImpl->relationships.find(conceptA);
    if (itA != pImpl->relationships.end()) {
        auto itB = itA->second.find(conceptB);
        if (itB != itA->second.end()) {
            return itB->second;
        }
    }
    return 0.0f;
}

void NeuralSemanticMemory::setConceptRelationship(const std::string& conceptA, const std::string& conceptB, float strength) {
    // Update bidirectional relationship
    pImpl->relationships[conceptA][conceptB] = strength;
    pImpl->relationships[conceptB][conceptA] = strength;
    
    // Update statistics
    auto& statsA = pImpl->stats[conceptA];
    auto& statsB = pImpl->stats[conceptB];
    statsA.totalAssociations += strength;
    statsB.totalAssociations += strength;
}

std::vector<NeuralSemanticMemory::ConceptStats> NeuralSemanticMemory::getConceptStats() const {
    std::vector<ConceptStats> result;
    for (const auto& pair : pImpl->stats) {
        result.push_back(pair.second);
    }
    return result;
}

void NeuralSemanticMemory::clear() {
    pImpl->concepts.clear();
    pImpl->activations.clear();
    pImpl->relationships.clear();
    pImpl->stats.clear();
    pImpl->selfModel.clear();
    pImpl->selfModelActivation = 0.0f;
}

size_t NeuralSemanticMemory::getMaxConcepts() const {
    return pImpl->maxConcepts;
}

size_t NeuralSemanticMemory::getCurrentConceptCount() const {
    return pImpl->concepts.size();
}

std::string NeuralSemanticMemory::getSelfModel() const {
    return pImpl->selfModel;
}

void NeuralSemanticMemory::updateSelfModel(const std::vector<float>& currentState) {
    // Create simple self-model based on current neural activity
    std::stringstream ss;
    ss << "SelfModel_" << pImpl->selfModelActivation;
    
    // Update based on current state
    float currentActivation = computeOverallActivation(currentState);
    pImpl->selfModelActivation = (pImpl->selfModelActivation + currentActivation) * 0.5f;
    pImpl->selfModel = ss.str();
}

// Helper methods
float NeuralSemanticMemory::Impl::computeCosineSimilarity(const std::vector<float>& a, const std::vector<float>& b) const {
    if (a.empty() || b.empty()) return 0.0f;
    
    size_t dim = std::min(a.size(), b.size());
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < dim; ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    normA = std::sqrt(normA);
    normB = std::sqrt(normB);
    
    if (normA == 0.0f || normB == 0.0f) return 0.0f;
    
    return dotProduct / (normA * normB);
}

float NeuralSemanticMemory::Impl::computeOverallActivation(const std::vector<float>& pattern) const {
    float sum = 0.0f;
    for (float val : pattern) {
        sum += std::abs(val);
    }
    return sum / std::max<float>(pattern.size(), 1.0f);
}

std::string NeuralSemanticMemory::Impl::generateConceptName(const std::vector<float>& pattern, const std::string& context) const {
    std::stringstream ss;
    
    if (!context.empty()) {
        ss << context << "_";
    }
    
    // Generate name based on pattern characteristics
    float avgActivation = computeOverallActivation(pattern);
    ss << "Concept_" << avgActivation << "_" << concepts.size();
    
    return ss.str();
}

void NeuralSemanticMemory::updateConcept(const std::string& name, const std::vector<float>& pattern, float activationStrength) {
    // Update concept representation (average with existing)
    auto& concept = pImpl->concepts[name];
    
    // Ensure concept has correct dimensionality
    if (concept.size() != pattern.size()) {
        concept.resize(pattern.size(), 0.0f);
    }
    
    // Update concept with pattern
    for (size_t i = 0; i < pattern.size(); ++i) {
        concept[i] = concept[i] * 0.9f + pattern[i] * 0.1f; // Weighted average
    }
    
    // Update activation
    pImpl->activations[name] = (pImpl->activations[name] * 0.7f) + (activationStrength * 0.3f);
    
    // Update statistics
    auto& stats = pImpl->stats[name];
    stats.activationCount++;
    stats.lastActivation = activationStrength;
    stats.averageActivation = (stats.averageActivation * (stats.activationCount - 1) + activationStrength) / stats.activationCount;
    stats.knowledgeLevel = std::min(1.0f, stats.knowledgeLevel + 0.01f);
}

void NeuralSemanticMemory::createConcept(const std::vector<float>& pattern, float activationStrength) {
    if (pImpl->concepts.size() >= pImpl->maxConcepts) {
        // Remove least used concept
        std::string leastUsedConcept;
        size_t minUsage = SIZE_MAX;
        
        for (const auto& pair : pImpl->stats) {
            if (pair.second.activationCount < minUsage) {
                minUsage = pair.second.activationCount;
                leastUsedConcept = pair.first;
            }
        }
        
        if (!leastUsedConcept.empty()) {
            pImpl->concepts.erase(leastUsedConcept);
            pImpl->activations.erase(leastUsedConcept);
            pImpl->relationships.erase(leastUsedConcept);
            pImpl->stats.erase(leastUsedConcept);
        }
    }
    
    // Generate concept name
    std::string conceptName = generateConceptName(pattern, "");
    
    // Create new concept
    pImpl->concepts[conceptName] = pattern;
    pImpl->activations[conceptName] = activationStrength;
    
    // Initialize statistics
    pImpl->stats[conceptName] = ConceptStats{
        conceptName,
        1,
        activationStrength,
        activationStrength,
        0.0f,
        0.5f,
        0
    };
}

void NeuralSemanticMemory::refineConceptByContext(const std::string& conceptName, const std::string& context) {
    // Simple context-based refinement
    if (pImpl->concepts.find(conceptName) != pImpl->concepts.end()) {
        // Could add context-specific modifications here
        // For now, just log the context
        NLM_LOG_INFO("Refining concept " + conceptName + " with context: " + context);
    }
}

// Observe social behavior for learning
void NeuralSemanticMemory::observeSocialBehavior(const std::vector<float>& socialPattern, float socialSignal) {
    // Learn from social observations
    // Social patterns can be added as new concepts or reinforce existing ones
    
    // Add social pattern as a potential new concept
    std::string socialConceptName = "Social_Behavior_" + std::to_string(socialSignal);
    
    // Create social concept with modified pattern (enhanced based on signal)
    std::vector<float> enhancedPattern = socialPattern;
    for (float& val : enhancedPattern) {
        val = val * (1.0f + socialSignal * 0.1f); // Amplify based on social signal
    }
    
    createConcept(enhancedPattern, socialSignal * 0.5f);
    
    // Create associations between social behavior and existing concepts
    std::vector<std::string> activeConcepts = getActiveConcepts(0.3f);
    for (const std::string& conceptName : activeConcepts) {
        setConceptRelationship(conceptName, socialConceptName, socialSignal * 0.2f);
    }
}

} // namespace nlm