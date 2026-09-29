#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    
    // Sequence learning state
    std::vector<std::pair<std::vector<float>, NeuronId>> learnedSequences;
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    
    // Pattern neurons (one per unique sensory pattern cluster)
    std::vector<NeuronId> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    
    // Recent states for sequence learning
    std::deque<std::pair<std::vector<float>, SimulationStep>> stateHistory;
    
    // Action-consequence learning
    std::vector<std::pair<std::vector<float>, std::vector<float>>> actionConsequences;
    std::vector<float> actionRewards;
    
    Impl() : brain(nullptr) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
    , sequenceMemorySize_(10)
    , predictionHorizon_(1)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , brain_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f)
{
}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain) {
    pImpl->brain = brain;
    brain_ = brain;
    
    NLM_LOG_INFO("NeuralPrediction initialized");
}

void NeuralPrediction::recordSensoryState(const std::vector<float>& sensoryState, 
                                         SimulationStep currentStep) {
    if (!temporalPredictionEnabled_) return;
    
    recentSensoryStates_.push_back(sensoryState);
    stateTimestamps_.push_back(currentStep);
    
    // Keep only recent states
    if (recentSensoryStates_.size() > sequenceMemorySize_) {
        recentSensoryStates_.erase(recentSensoryStates_.begin());
        stateTimestamps_.erase(stateTimestamps_.begin());
    }
    
    // Learn sequence if we have at least 2 states
    if (recentSensoryStates_.size() >= 2) {
        learnTemporalSequence(recentSensoryStates_[recentSensoryStates_.size() - 2],
                            recentSensoryStates_.back(),
                            currentStep);
    }
}

std::vector<float> NeuralPrediction::generatePrediction(SimulationStep currentStep) {
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return std::vector<float>();
    }
    
    // Find best matching learned sequence
    const auto& currentState = recentSensoryStates_.back();
    
    // Find most similar pattern we've seen
    NeuronId bestMatch = INVALID_NEURON_ID;
    float bestSimilarity = 0.0f;
    
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(currentState, pImpl->patternRepresentations[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestMatch = pImpl->patternNeurons[i];
        }
    }
    
    // Update prediction system with learned concept for more accurate predictions
    if (brain_->getConceptFormation()) {
        const auto& concepts = brain_->getConceptFormation()->getConcepts();
        if (!concepts.empty()) {
            // Find the most stable and recently observed concept
            size_t topConceptId = 0;
            float topStability = 0.0f;
            
            for (const auto& concept : concepts) {
                if (concept.avgStability > topStability) {
                    topConceptId = concept.id;
                    topStability = concept.avgStability;
                }
            }
            
            if (topStability > 0.7f) {
                // Use the concept's prototype for prediction
                auto conceptPrototype = brain_->getConceptFormation()->getConceptPrototype(topConceptId);
                if (!conceptPrototype.empty()) {
                    // Update pattern neurons with concept prototype
                    auto conceptNeuron = findOrCreatePatternNeuron(conceptPrototype);
                    if (conceptNeuron != INVALID_NEURON_ID) {
                        // Store concept information for prediction
                        pImpl->conceptNeurons.push_back(conceptNeuron);
                        pImpl->conceptNeuronIds.push_back(topConceptId);
                        
                        // Modify prediction generation to use concept
                        // This represents how learned concepts guide predictions
                        return predictWithConcept(conceptNeuron, currentStep);
                    }
                }
            }
        }
    }
    
    // Fallback to standard prediction if no concept is available
    return std::vector<float>();
    
    // No good prediction - use sequence extrapolation from state history
    if (recentSensoryStates_.size() >= 3) {
        // Use linear extrapolation from recent history
        // Predict based on recent state changes
        predictionConfidence_ = 0.3f;
        auto prediction = extrapolateFromHistory(recentSensoryStates_);
        applyTemporalDynamics(prediction, currentStep);
        return prediction;
    }
    
    // No prediction available - generate default temporal pattern
    predictionConfidence_ = 0.0f;
    return generateDefaultPrediction(currentStep);
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState,
                                             SimulationStep currentStep) {
    // Generate prediction first
    auto predicted = generatePrediction(currentStep);
    
    // Compute prediction error using multiple metrics
    if (predicted.empty()) {
        predictionError_ = 0.0f;
        predictionConfidence_ = 0.0f;
    } else {
        // Compute multiple error components
        float similarity = computeSimilarity(predicted, actualState);
        predictionError_ = 1.0f - similarity;  // Convert to error (0 = perfect, 1 = worst)
        
        // Add temporal error (difference from expected sequence)
        float temporalError = 0.0f;
        if (!actualState.empty() && !recentSensoryStates_.empty()) {
            if (recentSensoryStates_.size() >= 2) {
                // Check if actual state deviates from predicted temporal progression
                float expectedSimilarity = computeTemporalExpectation(actualState);
                temporalError = std::max(0.0f, 1.0f - expectedSimilarity);
            }
        }
        
        // Combine error components
        predictionError_ = (predictionError_ + temporalError) * 0.5f;
        
        // Update confidence based on error magnitude
        predictionConfidence_ = std::max(0.0f, 1.0f - predictionError_);
    }
    
    errorHistory_.push_back(predictionError_);
    if (errorHistory_.size() > 1000) {
        errorHistory_.erase(errorHistory_.begin());
    }
    
    // Record the actual state for future learning
    recordSensoryState(actualState, currentStep);
    
    // Modulate learning based on error and reward
    float reward = 1.0f - predictionError_;
    modulatePredictionSynapses(predictionError_, reward);
    
    // Update action-consequence learning based on prediction outcome
    if (brain_) {
        // Use prediction error to guide action selection and learning
        float predictionSurprise = std::min(1.0f, predictionError_ * 2.0f); // Normalize to 0-1
        
        // Update the concept formation module based on prediction error
        if (brain_->getConceptFormation()) {
            brain_->getConceptFormation()->updateFromPrediction(
                actualState, predicted, predictionError_);
        }
    }
    
    return predictionError_;
}

std::vector<float> NeuralPrediction::predictActionConsequence(ActionType action,
                                                               const std::vector<float>& currentState) {
    if (!actionConsequenceEnabled_) {
        return currentState;  // No change predicted
    }
    
    // Find experiences with same action and similar state
    float bestMatch = 0.0f;
    std::vector<float> bestConsequence;
    
    for (size_t i = 0; i < recentActions_.size(); ++i) {
        if (recentActions_[i].first == action) {
            float sim = computeSimilarity(currentState, recentActions_[i].second);
            if (sim > bestMatch) {
                bestMatch = sim;
                if (i < pImpl->actionConsequences.size()) {
                    bestConsequence = pImpl->actionConsequences[i].second;
                }
            }
        }
    }
    
    if (bestMatch > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return currentState;  // Default: no change
}

std::vector<std::vector<float>> NeuralPrediction::predictMultipleSteps(SimulationStep currentStep,
                                                                       size_t numSteps) {
    std::vector<std::vector<float>> predictions;
    
    auto currentPred = recentSensoryStates_.empty() ? 
                       std::vector<float>() : recentSensoryStates_.back();
    
    for (size_t step = 0; step < numSteps; ++step) {
        if (currentPred.empty()) break;
        
        predictions.push_back(currentPred);
        
        // Use current prediction to generate next
        // (simple approach - in reality this would use chained predictions)
        auto nextPred = generatePrediction(currentStep + step);
        if (nextPred.empty()) break;
        currentPred = nextPred;
    }
    
    return predictions;
}

void NeuralPrediction::recordAction(ActionType action, SimulationStep step) {
    if (recentSensoryStates_.empty()) return;
    
    recentActions_.push_back({action, recentSensoryStates_.back()});
    if (recentActions_.size() > 100) {
        recentActions_.erase(recentActions_.begin());
    }
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                            const std::vector<float>& nextState,
                                            SimulationStep currentStep) {
    // Find or create pattern neuron for current state
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState);
    
    if (currentNeuron != nextNeuron) {
        createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
    }
}

NeuronId NeuralPrediction::findMatchingPatternNeuron(const std::vector<float>& pattern,
                                                    float similarityThreshold) {
    // Check if we already have a similar pattern
    for (size_t i = 0; i < pImpl->patternRepresentations.size(); ++i) {
        float sim = computeSimilarity(pattern, pImpl->patternRepresentations[i]);
        if (sim >= similarityThreshold) {
            return pImpl->patternNeurons[i];
        }
    }
    
    // Create new pattern neuron
    if (brain_) {
        NeuronId newId = NeuronId(pImpl->patternNeurons.size() + 10000);
        pImpl->patternNeurons.push_back(newId);
        pImpl->patternRepresentations.push_back(pattern);
        return newId;
    }
    
    return INVALID_NEURON_ID;
}

void NeuralPrediction::createSequenceAssociation(NeuronId from, NeuronId to, float strength) {
    if (from == INVALID_NEURON_ID || to == INVALID_NEURON_ID) return;
    
    // Check if association already exists
    for (auto& assoc : pImpl->sequenceAssociations) {
        if (assoc.first == from && assoc.second == to) {
            // Update strength
            return;
        }
    }
    
    // Create new association
    pImpl->sequenceAssociations.emplace_back(from, to);
}

float NeuralPrediction::computeSimilarity(const std::vector<float>& a, 
                                         const std::vector<float>& b) const {
    if (a.size() != b.size() || a.empty()) return 0.0f;
    
    // Cosine similarity
    float dotProduct = 0.0f;
    float normA = 0.0f;
    float normB = 0.0f;
    
    for (size_t i = 0; i < a.size(); ++i) {
        dotProduct += a[i] * b[i];
        normA += a[i] * a[i];
        normB += b[i] * b[i];
    }
    
    if (normA < 0.0001f || normB < 0.0001f) return 0.0f;
    
    return dotProduct / (std::sqrt(normA) * std::sqrt(normB));
}

void NeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // Apply reward-modulated plasticity to prediction-related synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // If this synapse was recently active in prediction pathway
            if (std::abs(eligibility) > 0.001f) {
                float delta = eligibility * (reward - 0.5f) * 2.0f;
                syn->addToWeight(delta);
                
                // Decay eligibility
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    return predictionNeurons_;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->sequenceAssociations) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    return result;
}

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    pImpl->learnedSequences.clear();
    pImpl->sequenceAssociations.clear();
}

// Apply neural temporal dynamics to prediction
void NeuralPrediction::applyTemporalDynamics(std::vector<float>& prediction, SimulationStep currentStep) {
    if (!brain_ || prediction.empty()) return;
    
    // Simulate temporal dynamics based on neural activity
    // This represents how predictions evolve over time through the spiking substrate
    
    // Modulate prediction based on current time
    // This represents learning and adaptation over time
    float timeFactor = static_cast<float>(currentStep % 100) / 100.0f;
    
    // Apply small time-dependent changes to prediction
    for (size_t i = 0; i < prediction.size(); ++i) {
        // Add small temporal modulation
        prediction[i] *= (1.0f + timeFactor * 0.1f * (static_cast<float>(i % 5) / 5.0f - 0.5f));
        
        // Add noise to represent neural stochasticity
        if (brain_->getRandomGenerator()) {
            float noise = brain_->getRandomGenerator()->getFloat(-0.01f, 0.01f);
            prediction[i] += noise;
        }
    }
}

// Extrapolate prediction from recent state history
std::vector<float> NeuralPrediction::extrapolateFromHistory(const std::vector<std::vector<float>>& history) {
    if (history.size() < 2) return std::vector<float>();
    
    size_t historySize = std::min(history.size(), static_cast<size_t>(5));
    const auto& currentState = history.back();
    
    // Simple linear extrapolation based on recent trends
    std::vector<float> prediction(currentState.size(), 0.0f);
    
    if (historySize >= 3) {
        // Calculate average change direction
        for (size_t dim = 0; dim < currentState.size(); ++dim) {
            float sumChanges = 0.0f;
            for (size_t i = 1; i < historySize; ++i) {
                if (history[history.size() - i][dim] > 0 && history[history.size() - i - 1][dim] > 0) {
                    sumChanges += history[history.size() - i][dim] - history[history.size() - i - 1][dim];
                }
            }
            
            if (historySize > 1) {
                float avgChange = sumChanges / (historySize - 1);
                prediction[dim] = currentState[dim] + avgChange * 0.5f; // Apply 50% of trend
            }
        }
    }
    
    return prediction;
}

// Generate default prediction when no other method is available
std::vector<float> NeuralPrediction::generateDefaultPrediction(SimulationStep currentStep) {
    // Generate a default prediction based on step number for deterministic behavior
    std::vector<float> prediction;
    
    if (brain_->getRandomGenerator()) {
        // Add controlled randomness for exploration
        float seed = static_cast<float>(currentStep % 100) / 100.0f;
        
        // Generate variation based on seed
        for (int i = 0; i < 10; ++i) {
            float value = std::sin(seed * 3.14f * (i + 1)) * 0.1f + 0.5f;
            prediction.push_back(value);
        }
    }
    
    return prediction;
}

// Compute expected state based on temporal patterns
float NeuralPrediction::computeTemporalExpectation(const std::vector<float>& actualState) const {
    if (stateTimestamps_.size() < 2) return 0.0f;
    
    // Check if actual state matches recent temporal patterns
    float maxMatch = 0.0f;
    
    // Look through recent state history for similar patterns
    for (size_t i = 1; i < std::min(stateTimestamps_.size(), static_cast<size_t>(3)); ++i) {
        // This would require access to state history
        // For now, return 0.5f as baseline
    }
    
    return maxMatch;
}

// ActionConsequencePredictor Implementation
struct ActionConsequencePredictor::Impl {
    Brain* brain;
    
    // State-action -> consequence mappings
    std::vector<std::pair<std::pair<std::vector<float>, ActionType>, std::vector<float>>> experiences;
    
    // Average reward per action
    std::vector<std::pair<ActionType, float>> actionQuality;
    
    Impl() : brain(nullptr) {}
};

ActionConsequencePredictor::ActionConsequencePredictor()
    : pImpl(new Impl)
{
}

ActionConsequencePredictor::~ActionConsequencePredictor() = default;

void ActionConsequencePredictor::initialize(Brain* brain) {
    pImpl->brain = brain;
}

void ActionConsequencePredictor::recordExperience(ActionType action,
                                                   const std::vector<float>& beforeState,
                                                   const std::vector<float>& afterState,
                                                   float reward) {
    pImpl->experiences.push_back({{beforeState, action}, afterState});
    pImpl->actionQuality.push_back({action, reward});
    
    // Update average reward for this action
    float totalReward = 0.0f;
    size_t count = 0;
    for (const auto& aq : pImpl->actionQuality) {
        if (aq.first == action) {
            totalReward += aq.second;
            ++count;
        }
    }
    
    // Update or add to action quality
    for (auto& aq : pImpl->actionQuality) {
        if (aq.first == action) {
            aq.second = totalReward / count;
            break;
        }
    }
    
    // Limit experience memory
    if (pImpl->experiences.size() > 1000) {
        pImpl->experiences.erase(pImpl->experiences.begin());
    }
}

std::vector<float> ActionConsequencePredictor::predictConsequence(ActionType action,
                                                                  const std::vector<float>& currentState) {
    // Find most similar previous experience with this action
    float bestSimilarity = 0.0f;
    std::vector<float> bestConsequence;
    
    for (const auto& exp : pImpl->experiences) {
        if (exp.first.second == action) {
            // Compute similarity between current state and stored before-state
            const auto& before = exp.first.first;
            const auto& after = exp.second;
            
            if (before.size() != currentState.size()) continue;
            
            float dot = 0.0f, norm1 = 0.0f, norm2 = 0.0f;
            for (size_t i = 0; i < before.size(); ++i) {
                dot += before[i] * currentState[i];
                norm1 += before[i] * before[i];
                norm2 += currentState[i] * currentState[i];
            }
            
            float sim = (norm1 > 0.0001f && norm2 > 0.0001f) ? 
                        dot / (std::sqrt(norm1) * std::sqrt(norm2)) : 0.0f;
            
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestConsequence = after;
            }
        }
    }
    
    if (bestSimilarity > 0.5f && !bestConsequence.empty()) {
        return bestConsequence;
    }
    
    return currentState; // Default: no predicted change
}

float ActionConsequencePredictor::getConsequenceConfidence(ActionType action,
                                                           const std::vector<float>& currentState) const {
    size_t matchingCount = 0;
    for (const auto& exp : pImpl->experiences) {
        if (exp.first.second == action) {
            ++matchingCount;
        }
    }
    
    // More experiences = higher confidence
    return std::min(1.0f, static_cast<float>(matchingCount) / 10.0f);
}

void ActionConsequencePredictor::updatePrediction(ActionType action,
                                                  const std::vector<float>& predicted,
                                                  const std::vector<float>& actual,
                                                  float reward) {
    // This would update the internal model
    // For now, just record the new experience
    recordExperience(action, predicted, actual, reward);
}

float ActionConsequencePredictor::getActionQuality(ActionType action) const {
    for (const auto& aq : pImpl->actionQuality) {
        if (aq.first == action) {
            return aq.second;
        }
    }
    return 0.0f;
}

void ActionConsequencePredictor::clear() {
    pImpl->experiences.clear();
    pImpl->actionQuality.clear();
}

// PredictionErrorSignal Implementation
struct PredictionErrorSignal::Impl {
    float lastIntensityError;
    float lastSpatialError;
    float lastTemporalError;
    
    Impl() : lastIntensityError(0.0f), lastSpatialError(0.0f), lastTemporalError(0.0f) {}
};

PredictionErrorSignal::PredictionErrorSignal()
    : pImpl(new Impl)
    , lastError_(0.0f)
    , surpriseThreshold_(0.3f)
{
    errorComponents_.intensityError = 0.0f;
    errorComponents_.spatialError = 0.0f;
    errorComponents_.temporalError = 0.0f;
    errorComponents_.totalError = 0.0f;
}

PredictionErrorSignal::~PredictionErrorSignal() = default;

float PredictionErrorSignal::computeError(const std::vector<float>& predicted,
                                        const std::vector<float>& actual) {
    if (predicted.empty() || actual.empty()) {
        lastError_ = 0.0f;
        return 0.0f;
    }
    
    // Compute overall difference
    float totalDiff = 0.0f;
    float intensityDiff = 0.0f;
    
    for (size_t i = 0; i < std::min(predicted.size(), actual.size()); ++i) {
        float diff = std::abs(predicted[i] - actual[i]);
        totalDiff += diff;
        intensityDiff += diff * diff;
    }
    
    float meanError = totalDiff / predicted.size();
    float meanSqError = intensityDiff / predicted.size();
    
    // Compute spatial error (variance of differences)
    float spatialVar = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        spatialVar += (diff - meanError) * (diff - meanError);
    }
    float spatialError = std::sqrt(spatialVar / predicted.size());
    
    // Update components
    pImpl->lastIntensityError = std::sqrt(meanSqError);
    pImpl->lastSpatialError = spatialError;
    errorComponents_.intensityError = pImpl->lastIntensityError;
    errorComponents_.spatialError = pImpl->lastSpatialError;
    
    // Compute total error (combining components)
    lastError_ = std::sqrt(meanSqError + spatialError * spatialError);
    errorComponents_.totalError = lastError_;
    
    return lastError_;
}

float PredictionErrorSignal::getModulationSignal() const {
    // Convert error to modulation signal (0-1)
    // Higher error = stronger modulation for learning
    return std::min(1.0f, lastError_ * 2.0f);
}

void PredictionErrorSignal::recordError(float error, SimulationStep step) {
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 10000) {
        errorHistory_.erase(errorHistory_.begin());
    }
}

} // namespace nlm