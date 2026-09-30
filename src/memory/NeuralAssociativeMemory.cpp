#include "NeuralAssociativeMemory.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <queue>
#include <set>
#include <numeric>

namespace nlm {

struct NeuralAssociativeMemory::Impl {
    Brain* brain;
    std::unordered_map<size_t, float> patternStats;
    std::unordered_map<size_t, std::set<size_t>> activationGraph;
    SimulationStep lastCleanupStep;
    
    Impl() : brain(nullptr), lastCleanupStep(0) {}
};

NeuralAssociativeMemory::NeuralAssociativeMemory()
    : pImpl(new Impl)
    , brain_(nullptr)
    , totalAssociationStrength(0.0f)
    , lastLearningStep(0)
    , explorationBias(0.5f)
    , exploitationBias(0.5f)
{
}

NeuralAssociativeMemory::~NeuralAssociativeMemory() = default;

void NeuralAssociativeMemory::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    // Initialize with existing episodic memory if available
    if (brain_) {
        auto* episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            buildConceptAssociations();
        }
    }
    
    NLM_LOG_INFO("NeuralAssociativeMemory initialized");
}

void NeuralAssociativeMemory::associate(const std::vector<float>& patternA,
                                       const std::vector<float>& patternB,
                                       float strength) {
    if (patternA.empty() || patternB.empty() || !brain_) return;
    
    size_t neuronA = findPatternNeuron(patternA);
    size_t neuronB = findPatternNeuron(patternB);
    
    if (neuronA != INVALID_NEURON_ID && neuronB != INVALID_NEURON_ID) {
        // Check if association already exists
        for (auto& assoc : associations_) {
            if ((assoc.patternA == patternA && assoc.patternB == patternB) ||
                (assoc.patternB == patternA && assoc.patternA == patternB)) {
                // Update existing association
                assoc.strength = std::clamp(assoc.strength + strength * 0.1f, 0.0f, 1.0f);
                assoc.lastActivated = pImpl->brain ? pImpl->brain->getCurrentStep() : 0.0f;
                return;
            }
        }
        
        // Create new association with concept labels
        std::string conceptA = generateConceptLabel(patternA);
        std::string conceptB = generateConceptLabel(patternB);
        
        associations_.emplace_back(patternA, patternB, conceptA, conceptB, strength);
        
        // Update activation graph
        pImpl->activationGraph[neuronA].insert(neuronB);
        pImpl->activationGraph[neuronB].insert(neuronA);
        
        // Update statistics
        pImpl->patternStats[neuronA] += strength;
        pImpl->patternStats[neuronB] += strength;
        totalAssociationStrength += strength;
        
        // Log association creation
        NLM_LOG_DEBUG("Created association between concepts: " + conceptA + " <-> " + conceptB +
                     " (strength: " + std::to_string(strength) + ")");
    }
}

void NeuralAssociativeMemory::associateFromExperience(
    const EnhancedEpisodicMemoryItem& episode) {
    if (!episode.modalities.sensoryState.empty() && 
        !episode.modalities.motorState.empty()) {
        // Associate sensory state with motor state (action-outcome relationship)
        associate(episode.modalities.sensoryState, 
                 episode.modalities.motorState, 
                 0.8f * episode.modalities.reward);
    }
    
    // Associate with emotional context
    if (std::abs(episode.modalities.emotion) > 0.1f) {
        std::vector<float> emotionPattern(1, episode.modalities.emotion);
        if (!episode.modalities.sensoryState.empty()) {
            associate(emotionPattern, episode.modalities.sensoryState, 
                     std::abs(episode.modalities.emotion) * 0.5f);
        }
    }
    
    // Associate temporal sequence
    if (episode.temporalSequence.size() > 1) {
        std::vector<float> sequencePattern;
        for (size_t i = 0; i < std::min(size_t(5), episode.temporalSequence.size()); ++i) {
            sequencePattern.push_back(static_cast<float>(episode.temporalSequence[i]) / 10000.0f);
        }
        
        if (!sequencePattern.empty() && !episode.modalities.sensoryState.empty()) {
            associate(sequencePattern, episode.modalities.sensoryState, 0.6f);
        }
    }
    
    // Record activation for later consolidation
    for (size_t i = 0; i < std::min(size_t(10), episode.activeNeurons.size()); ++i) {
        NeuronId neuron = episode.activeNeurons[i];
        if (neuron != INVALID_NEURON_ID) {
            pImpl->activationGraph[neuron.value].insert(i);
        }
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::retrieve(
    const std::vector<float>& queryPattern,
    size_t maxResults,
    float similarityThreshold) const {
    
    std::vector<std::vector<float>> results;
    std::vector<std::pair<float, std::vector<float>>> scored;
    
    // Find best matching pattern neuron
    size_t queryNeuron = findPatternNeuron(queryPattern);
    
    if (queryNeuron == INVALID_NEURON_ID) {
        // No exact match, find by similarity
        for (const auto& pn : patternBank_) {
            float similarity = computeSimilarity(queryPattern, pn);
            if (similarity > similarityThreshold) {
                scored.emplace_back(similarity, pn);
            }
        }
    } else {
        // Find directly connected patterns
        auto it = pImpl->activationGraph.find(queryNeuron);
        if (it != pImpl->activationGraph.end()) {
            for (size_t neighbor : it->second) {
                if (neighbor < patternBank_.size()) {
                    // Find pattern in bank
                    for (size_t i = 0; i < patternBank_.size(); ++i) {
                        if (i == neighbor) {
                            float strength = 0.0f;
                            // Find association strength
                            for (const auto& assoc : associations_) {
                                if ((assoc.patternA == patternBank_[i] || 
                                     (assoc.patternB == patternBank_[i] && 
                                      (assoc.patternA == queryPattern || assoc.patternB == queryPattern))) {
                                    strength = assoc.strength;
                                    break;
                                }
                            }
                            scored.emplace_back(strength, patternBank_[i]);
                            break;
                        }
                    }
                }
            }
        }
    }
    
    // Sort by strength
    std::sort(scored.begin(), scored.end(),
        [](const auto& a, const auto& b) { return a.first > b.first; });
    
    // Return top results
    for (size_t i = 0; i < std::min(maxResults, scored.size()); ++i) {
        results.push_back(scored[i].second);
    }
    
    return results;
}

std::vector<std::vector<float>> NeuralAssociativeMemory::spreadActivation(
    const std::vector<float>& cuePattern,
    size_t steps,
    float activationThreshold) const {
    
    std::vector<std::vector<float>> activated;
    std::set<size_t> visited;
    
    // Find initial pattern neuron
    size_t currentNeuron;
    for (size_t i = 0; i < patternBank_.size(); ++i) {
        if (computeSimilarity(cuePattern, patternBank_[i]) > 0.5f) {
            currentNeuron = i;
            activated.push_back(patternBank_[i]);
            visited.insert(currentNeuron);
            break;
        }
    }
    
    if (currentNeuron == INVALID_NEURON_ID) {
        return activated;
    }
    
    // Spread through association network
    for (size_t step = 0; step < steps; ++step) {
        std::vector<size_t> nextNeurons;
        
        auto it = pImpl->activationGraph.find(currentNeuron);
        if (it != pImpl->activationGraph.end()) {
            for (size_t neighbor : it->second) {
                if (visited.find(neighbor) == visited.end()) {
                    // Check activation threshold
                    float activation = getActivationFor(neighbor);
                    if (activation > activationThreshold) {
                        nextNeurons.push_back(neighbor);
                        visited.insert(neighbor);
                        
                        // Find pattern for this neuron
                        for (size_t i = 0; i < patternBank_.size(); ++i) {
                            if (i == neighbor) {
                                activated.push_back(patternBank_[i]);
                                break;
                            }
                        }
                    }
                }
            }
        }
        
        if (nextNeurons.empty()) break;
        
        // Pick highest activated next neuron
        currentNeuron = nextNeurons[0];
    }
    
    return activated;
}

std::vector<std::string> NeuralAssociativeMemory::extractConcepts(
    const std::vector<const EnhancedEpisodicMemoryItem*>& episodes) const {
    
    std::vector<std::string> concepts;
    std::unordered_map<std::string, int> conceptCounts;
    
    for (const auto* episode : episodes) {
        if (!episode) continue;
        
        // Extract concepts from sensory pattern
        std::string sensoryConcept = generateConceptLabel(episode->modalities.sensoryState);
        concepts.push_back(sensoryConcept);
        conceptCounts[sensoryConcept]++;
        
        // Extract concepts from action
        if (!episode->modalities.motorState.empty()) {
            std::string actionConcept = generateActionConcept(episode->modalities.motorState);
            concepts.push_back(actionConcept);
            conceptCounts[actionConcept]++;
        }
        
        // Extract concepts from emotion
        std::string emotionConcept = generateEmotionConcept(episode->modalities.emotion);
        concepts.push_back(emotionConcept);
        conceptCounts[emotionConcept]++;
    }
    
    // Filter concepts that appear frequently
    for (const auto& pair : conceptCounts) {
        if (pair.second >= 2) {  // Appear in at least 2 episodes
            concepts.push_back(pair.first);
        }
    }
    
    return concepts;
}

void NeuralAssociativeMemory::createSemanticLinks(
    const std::vector<std::string>& concepts) {
    
    for (size_t i = 0; i < concepts.size(); ++i) {
        for (size_t j = i + 1; j < concepts.size(); ++j) {
            if (concepts[i] != concepts[j]) {
                // Create semantic association between concepts
                float strength = 0.5f + 0.5f * (i + j) / concepts.size();
                
                // Convert concepts back to patterns (simplified)
                std::vector<float> patternA(1, static_cast<float>(i) / concepts.size());
                std::vector<float> patternB(1, static_cast<float>(j) / concepts.size());
                
                associate(patternA, patternB, strength);
            }
        }
    }
}

void NeuralAssociativeMemory::buildSchemaFromAssociations() {
    schemas_.clear();
    
    // Group patterns by concept
    std::unordered_map<std::string, std::vector<std::vector<float>>> conceptPatternMap;
    
    for (const auto& assoc : associations_) {
        conceptPatternMap[assoc.conceptA].push_back(assoc.patternA);
        conceptPatternMap[assoc.conceptB].push_back(assoc.patternB);
    }
    
    // Create schema patterns by averaging patterns within each concept
    for (const auto& pair : conceptPatternMap) {
        if (pair.second.empty()) continue;
        
        std::vector<float> schemaPattern(pair.second[0].size(), 0.0f);
        float count = 0.0f;
        
        for (const auto& pattern : pair.second) {
            for (size_t i = 0; i < pattern.size() && i < schemaPattern.size(); ++i) {
                schemaPattern[i] += pattern[i];
            }
            count += 1.0f;
        }
        
        if (count > 0.0f) {
            for (size_t i = 0; i < schemaPattern.size(); ++i) {
                schemaPattern[i] /= count;
            }
        }
        
        schemas_.push_back(schemaPattern);
        activeConcepts_.push_back(pair.first);
    }
}

std::vector<std::vector<float>> NeuralAssociativeMemory::getSchemaPatterns() const {
    return schemas_;
}

std::vector<float> NeuralAssociativeMemory::suggestNovelPattern() const {
    std::vector<float> novelPattern;
    
    // Generate pattern that's different from all schemas
    float maxDistance = 0.0f;
    std::vector<float> selectedSchema;
    
    for (const auto& schema : schemas_) {
        float distance = computeSchemaDistance(schema, schemas_);
        if (distance > maxDistance) {
            maxDistance = distance;
            selectedSchema = schema;
        }
    }
    
    if (!selectedSchema.empty()) {
        // Create variation of selected schema
        novelPattern = selectedSchema;
        for (float& value : novelPattern) {
            value += (brain_->getRandomGenerator()->uniformReal(-0.2f, 0.2f) * 0.5f);
            value = std::clamp(value, 0.0f, 1.0f);
        }
    }
    
    return novelPattern;
}

std::vector<float> NeuralAssociativeMemory::suggestExploitationPattern() const {
    std::vector<float> exploitationPattern;
    
    // Use the most strongly activated schema
    float maxStrength = 0.0f;
    std::vector<float> bestSchema;
    
    for (const auto& schema : schemas_) {
        float strength = computeSchemaStrength(schema);
        if (strength > maxStrength) {
            maxStrength = strength;
            bestSchema = schema;
        }
    }
    
    if (!bestSchema.empty()) {
        exploitationPattern = bestSchema;
        
        // Add some noise for exploration within exploitation
        for (float& value : exploitationPattern) {
            value += brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f) * 0.2f;
            value = std::clamp(value, 0.0f, 1.0f);
        }
    }
    
    return exploitationPattern;
}

float NeuralAssociativeMemory::monitorLearningProgress() const {
    if (associations_.empty()) return 0.0f;
    
    // Calculate average association strength
    float totalStrength = 0.0f;
    for (const auto& assoc : associations_) {
        totalStrength += assoc.strength;
    }
    
    float avgStrength = totalStrength / associations_.size();
    
    // Decay over time based on last learning step
    SimulationStep currentStep = brain_ ? brain_->getCurrentStep() : 0;
    float ageDecay = std::exp(-0.001f * (currentStep - lastLearningStep_));
    
    return avgStrength * ageDecay;
}

float NeuralAssociativeMemory::evaluatePredictionAccuracy() const {
    // Simplified prediction accuracy based on association consistency
    float totalConfidence = 0.0f;
    int confidenceCount = 0;
    
    for (const auto& assoc : associations_) {
        // Higher strength associations have higher confidence
        totalConfidence += assoc.strength * assoc.strength;  // Square for exponential weighting
        confidenceCount++;
    }
    
    if (confidenceCount == 0) return 0.0f;
    
    float baseAccuracy = std::sqrt(totalConfidence / confidenceCount);
    
    // Adjust based on concept diversity
    std::set<std::string> uniqueConcepts;
    for (const auto& assoc : associations_) {
        uniqueConcepts.insert(assoc.conceptA);
        uniqueConcepts.insert(assoc.conceptB);
    }
    
    conceptDiversity = static_cast<float>(uniqueConcepts.size()) / associations_.size();
    
    return baseAccuracy * (0.7f + 0.3f * conceptDiversity);
}

size_t NeuralAssociativeMemory::selectBasedOnMemory(
    const std::vector<std::vector<float>>& options) const {
    
    if (options.empty()) return 0;
    
    std::vector<float> scores(options.size(), 0.0f);
    
    for (size_t i = 0; i < options.size(); ++i) {
        float baseScore = evaluateOptionWithMemory(options[i]);
        
        // Add novelty bonus
        float noveltyBonus = 0.0f;
        for (const auto& schema : schemas_) {
            float distance = computePatternDistance(options[i], schema);
            noveltyBonus += distance * 0.1f;
        }
        
        // Add exploration bonus
        float explorationBonus = explorationBias_ * 0.3f;
        
        scores[i] = baseScore + noveltyBonus + explorationBonus;
    }
    
    // Find maximum score
    size_t bestIndex = 0;
    float maxScore = scores[0];
    
    for (size_t i = 1; i < scores.size(); ++i) {
        if (scores[i] > maxScore) {
            maxScore = scores[i];
            bestIndex = i;
        }
    }
    
    return bestIndex;
}

float NeuralAssociativeMemory::evaluateOptionWithMemory(
    const std::vector<float>& option) const {
    
    float totalScore = 0.0f;
    
    // Compare with schemas
    for (const auto& schema : schemas_) {
        if (schema.empty()) continue;
        
        float similarity = computePatternDistance(option, schema);
        totalScore += similarity * 0.5f;
    }
    
    // Compare with episodic memory patterns (if available)
    if (brain_) {
        auto* episodicMem = brain_->getEpisodicMemory();
        if (episodicMem) {
            // Find most similar episodes
            auto similar = episodicMem->retrieveSimilar(option, 3);
            for (const auto* episode : similar) {
                if (!episode) continue;
                
                float rewardSimilarity = std::abs(option[0] - episode->modalities.reward) * 0.3f;
                float emotionSimilarity = std::abs(option[0] - episode->modalities.emotion) * 0.2f;
                totalScore += (rewardSimilarity + emotionSimilarity) * 0.4f;
            }
        }
    }
    
    // Add random noise for exploration
    float noise = brain_ ? brain_->getRandomGenerator()->uniformReal(-0.1f, 0.1f) : 0.0f;
    totalScore += noise * 0.1f;
    
    return std::clamp(totalScore, 0.0f, 1.0f);
}

void NeuralAssociativeMemory::adaptStrategyBasedOnMemoryStats() {
    // Adjust exploration-exploitation balance based on learning progress
    float learningProgress = monitorLearningProgress();
    
    // High learning progress -> increase exploitation
    // Low learning progress -> increase exploration
    if (learningProgress > 0.7f) {
        explorationBias_ = 0.3f;
        exploitationBias_ = 0.7f;
    } else if (learningProgress < 0.3f) {
        explorationBias_ = 0.7f;
        exploitationBias_ = 0.3f;
    } else {
        explorationBias_ = 0.5f;
        exploitationBias_ = 0.5f;
    }
    
    // Adapt based on concept diversity
    if (!activeConcepts_.empty()) {
        float conceptCoverage = static_cast<float>(activeConcepts_.size()) / 10.0f;  // Normalize to max 10 concepts
        explorationBias_ *= (0.5f + 0.5f * conceptCoverage);  // More concepts = less exploration
    }
    
    // Ensure biases stay within valid range
    explorationBias_ = std::clamp(explorationBias_, 0.1f, 0.9f);
    exploitationBias_ = 1.0f - explorationBias_;
}

void NeuralAssociativeMemory::balanceExplorationExploitation() {
    // Simplified version - just balance the biases
    adaptStrategyBasedOnMemoryStats();
}

void NeuralAssociativeMemory::clear() {
    associations_.clear();
    patternBank_.clear();
    pImpl->activationGraph.clear();
    pImpl->patternStats.clear();
    pImpl->lastCleanupStep = 0;
    
    schemas_.clear();
    activeConcepts_.clear();
    conceptPatterns_.clear();
    conceptToPatternMap_.clear();
    
    totalAssociationStrength = 0.0f;
    lastLearningStep_ = 0;
    explorationBias_ = 0.5f;
    exploitationBias_ = 0.5f;
    
    NLM_LOG_INFO("NeuralAssociativeMemory cleared");
}

void NeuralAssociativeMemory::resetAssociationWeights() {
    for (auto& assoc : associations_) {
        assoc.strength *= 0.5f;  // Decay all associations
    }
    
    totalAssociationStrength = 0.0f;
    for (const auto& assoc : associations_) {
        totalAssociationStrength += assoc.strength;
    }
    
    NLM_LOG_INFO("NeuralAssociativeMemory association weights reset");
}

float NeuralAssociativeMemory::getAverageAssociationStrength() const {
    if (associations_.empty()) return 0.0f;
    return totalAssociationStrength / associations_.size();
}

std::vector<std::string> NeuralAssociativeMemory::getActiveConcepts() const {
    return activeConcepts_;
}

// Helper methods implementation

NeuronId NeuralAssociativeMemory::findPatternNeuron(
    const std::vector<float>& pattern) const {
    
    // Check if pattern already exists in bank
    for (size_t i = 0; i < patternBank_.size(); ++i) {
        if (computeSimilarity(pattern, patternBank_[i]) > 0.8f) {
            return NeuronId(i + 50000);  // Offset to distinguish from other neuron IDs
        }
    }
    
    return INVALID_NEURON_ID;
}

float NeuralAssociativeMemory::computeSimilarity(
    const std::vector<float>& a,
    const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    float dot = 0.0f, normA = 0.0f, normB = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        dot += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dot / (std::sqrt(normA) * std::sqrt(normB));
}

void NeuralAssociativeMemory::updateActivationLevels(const std::vector<size_t>& activated) {
    for (size_t neuron : activated) {
        auto it = pImpl->activationGraph.find(neuron);
        if (it != pImpl->activationGraph.end()) {
            for (size_t neighbor : it->second) {
                pImpl->patternStats[neighbor] = std::min(1.0f, 
                    pImpl->patternStats[neighbor] * 1.1f);
            }
        }
    }
}

void NeuralAssociativeMemory::buildConceptAssociations() {
    // This would use the brain's episodic memory to build concepts
    if (brain_ && brain_->getEpisodicMemory()) {
        // Get recent episodes
        auto recent = brain_->getEpisodicMemory()->getRecentEpisodes(20);
        
        // Extract concepts and create associations
        std::vector<std::string> concepts = extractConcepts(recent);
        if (!concepts.empty()) {
            createSemanticLinks(concepts);
        }
    }
}

void NeuralAssociativeMemory::updateSchemaFromAssociations() {
    // Rebuild schemas periodically
    static SimulationStep lastUpdate = 0;
    if (brain_) {
        SimulationStep currentStep = brain_->getCurrentStep();
        if (currentStep - lastUpdate > 1000) {  // Update every 1000 steps
            buildSchemaFromAssociations();
            lastUpdate = currentStep;
        }
    }
}

float NeuralAssociativeMemory::getActivationFor(size_t neuronId) const {
    auto it = pImpl->patternStats.find(neuronId);
    if (it != pImpl->patternStats.end()) {
        return it->second;
    }
    return 0.0f;
}

std::string NeuralAssociativeMemory::generateConceptLabel(
    const std::vector<float>& pattern) const {
    
    if (pattern.empty()) return "empty";
    
    // Generate concept label based on pattern characteristics
    float avgValue = std::accumulate(pattern.begin(), pattern.end(), 0.0f) / pattern.size();
    float variance = 0.0f;
    
    for (float val : pattern) {
        variance += (val - avgValue) * (val - avgValue);
    }
    variance /= pattern.size();
    
    std::string concept = "concept_";
    
    if (avgValue < 0.33f) {
        concept += "low_";
    } else if (avgValue < 0.67f) {
        concept += "medium_";
    } else {
        concept += "high_";
    }
    
    if (variance < 0.01f) {
        concept += "uniform";
    } else if (variance < 0.05f) {
        concept += "stable";
    } else {
        concept += "dynamic";
    }
    
    return concept;
}

std::string NeuralAssociativeMemory::generateActionConcept(
    const std::vector<float>& action) const {
    
    if (action.empty()) return "unknown_action";
    
    float maxValue = *std::max_element(action.begin(), action.end());
    size_t maxIndex = std::distance(action.begin(), 
                                  std::max_element(action.begin(), action.end()));
    
    std::string actionNames[] = {"move_forward", "move_backward", "turn_left", "turn_right", 
                                 "interact", "wait"};
    
    if (maxIndex < sizeof(actionNames)/sizeof(actionNames[0])) {
        return "action_" + std::string(actionNames[maxIndex]);
    }
    
    return "action_custom_" + std::to_string(maxIndex);
}

std::string NeuralAssociativeMemory::generateEmotionConcept(float emotion) const {
    if (std::abs(emotion) < 0.33f) return "neutral";
    else if (emotion < 0.0f) return "negative";
    else return "positive";
}

float NeuralAssociativeMemory::computeSchemaDistance(
    const std::vector<float>& pattern,
    const std::vector<std::vector<float>>& schemas) const {
    
    if (schemas.empty()) return 1.0f;
    
    float minDistance = 1.0f;
    
    for (const auto& schema : schemas) {
        float distance = computePatternDistance(pattern, schema);
        if (distance < minDistance) {
            minDistance = distance;
        }
    }
    
    return minDistance;
}

float NeuralAssociativeMemory::computeSchemaStrength(
    const std::vector<float>& schema) const {
    
    float strength = 0.0f;
    
    for (const auto& assoc : associations_) {
        // Check if schema matches either pattern in association
        float match1 = computePatternDistance(schema, assoc.patternA);
        float match2 = computePatternDistance(schema, assoc.patternB);
        
        strength += std::max(0.0f, 1.0f - std::min(match1, match2));
    }
    
    return strength / std::max(1.0f, static_cast<float>(associations_.size()));
}

float NeuralAssociativeMemory::computePatternDistance(
    const std::vector<float>& a,
    const std::vector<float>& b) const {
    
    if (a.size() != b.size() || a.empty()) return 1.0f;
    
    float distance = 0.0f;
    for (size_t i = 0; i < a.size(); ++i) {
        distance += std::abs(a[i] - b[i]);
    }
    
    return distance / a.size();
}

} // namespace nlm