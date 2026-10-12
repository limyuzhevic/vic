#include "PredictionSystem.hpp"
#include "../core/Random/Random.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    RandomGenerator* rng;
    
    // Prediction neurons for temporal dynamics
    std::vector<NeuronId> predictionNeurons;
    
    // Bayesian inference components
    struct Hypothesis {
        std::vector<float> stateVector;
        float weight;                    // Prior probability * likelihood
        std::vector<float> uncertainty;  // Variance per dimension
    };
    
    std::vector<Hypothesis> activeHypotheses;
    std::vector<float> hypothesisWeights;
    
    // Neural prediction state
    std::vector<float> currentPrediction;
    std::vector<float> predictionUncertainty;
    
    // Learning components
    float learningRate;
    float explorationFactor;
    
    // Error tracking
    std::deque<PredictionError> errorHistory;
    std::vector<float> errorComponents;
    
    // Hierarchical prediction
    size_t predictionDepth;
    float predictionHorizon;
    
    Impl() : brain(nullptr), rng(nullptr), learningRate(0.01f), 
             explorationFactor(0.1f), predictionDepth(1), predictionHorizon(0.1f) {}
};

PredictionSystem::PredictionSystem()
    : predictionNeurons_(100), predictionHorizon_(0.1f), learningRate_(0.01f),
      uncertaintyThreshold_(0.5f), neuromodulationEnabled_(true), 
      hierarchicalPredictionEnabled_(false), brain_(nullptr), 
      lastPredictionTime_(-1.0f) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain, size_t predictionNeurons) {
    brain_ = brain;
    predictionNeurons_ = predictionNeurons;
    
    pImpl->brain = brain;
    pImpl->rng = brain_->getRandomGenerator();
    
    // Create prediction neurons using brain reference
    createPredictionNeurons();
    
    NLM_LOG_INFO(std::to_string(predictionNeurons_) + " prediction neurons initialized");
}

void PredictionSystem::createPredictionNeurons() {
    if (!brain_) return;
    
    pImpl->predictionNeurons.clear();
    
    // Create a separate region for prediction neurons
    RegionId predRegionId = brain_->addRegion("Prediction_Region");
    auto* region = brain_->getRegion(predRegionId);
    
    if (region) {
        // Create prediction neuron population
        PopulationId predPopId = region->addPopulation(predictionNeurons_, NeuronType::Internal);
        
        // Initialize neurons with random properties for diverse prediction capabilities
        auto* predPop = region->getPopulation(predPopId);
        if (predPop) {
            for (auto* neuron : predPop->getNeurons()) {
                // Set neuron parameters for prediction dynamics
                neuron->setThreshold(-40.0f);  // Higher threshold for prediction dynamics
                neuron->setRestingPotential(-65.0f);
                neuron->setResetPotential(-65.0f);
                neuron->setLeakConductance(5.0f);
                
                // Enable Hebbian plasticity for learning predictions
                neuron->enablePlasticity(true, true, false);
                
                // Random initial state for diverse prediction patterns
                neuron->initializeRandom(*pImpl->rng);
                
                // Inject initial current to activate prediction dynamics
                neuron->injectCurrent(0.5f * pImpl->rng->uniformReal(-1.0f, 1.0f));
            }
        }
    }
}

std::pair<std::unique_ptr<SensoryInput>, PredictionError> PredictionSystem::predictNextState(
    const SensoryInput& currentState, Timestamp currentTime) {
    
    // Update last prediction time
    lastPredictionTime_ = currentTime;
    
    // Get current sensory data
    const auto& currentData = currentState.getData();
    
    // Generate multiple predictive hypotheses using neural dynamics
    std::vector<std::vector<float>> hypotheses;
    std::vector<float> hypothesisWeights;
    
    // Generate temporal sequences of predictions (1-3 steps ahead)
    for (size_t depth = 1; depth <= (hierarchicalPredictionEnabled_ ? 3 : 1); ++depth) {
        // Use neural dynamics to generate predictions at different time scales
        auto predictedState = generateNeuralPrediction(currentData, depth, currentTime);
        
        if (!predictedState.empty()) {
            hypotheses.push_back(predictedState);
            
            // Calculate hypothesis weight based on neural activation and similarity
            float weight = calculateHypothesisWeight(predictedState, currentData, depth, currentTime);
            hypothesisWeights.push_back(weight);
        }
    }
    
    // If no hypotheses generated, return current state with high uncertainty
    if (hypotheses.empty()) {
        std::vector<float> emptyData;
        SensoryInput input(emptyData);
        PredictionError error;
        error.magnitude = 1.0f;
        error.uncertainty = 1.0f;
        return {std::make_unique<SensoryInput>(input), error};
    }
    
    // Apply Bayesian inference to combine hypotheses
    PredictionState bayesianPrediction = bayesianPrediction(currentData, hypotheses, hypothesisWeights);
    
    // Create sensory input from neural prediction
    SensoryInput predictedInput(bayesianPrediction.stateVector);
    
    // Compute prediction error
    PredictionError error = computeBayesianError(bayesianPrediction, currentData);
    
    // Update prediction weights based on error (Bayesian learning)
    updatePredictionWeights(error);
    
    // Apply neuromodulation if enabled
    if (neuromodulationEnabled_) {
        applyNeuromodulation(error);
    }
    
    return {std::make_unique<SensoryInput>(predictedInput), error};
}

PredictionState PredictionSystem::bayesianPrediction(
    const std::vector<float>& currentState,
    const std::vector<std::vector<float>>& hypotheses,
    const std::vector<float>& hypothesisWeights) {
    
    PredictionState result;
    result.predictionHorizon = predictionHorizon_;
    
    if (hypotheses.empty()) return result;
    
    // Normalize hypothesis weights
    float totalWeight = std::accumulate(hypothesisWeights.begin(), hypothesisWeights.end(), 0.0f);
    if (totalWeight > 0.0f) {
        for (float& weight : hypothesisWeights) {
            weight /= totalWeight;
        }
    } else {
        std::fill(hypothesisWeights.begin(), hypothesisWeights.end(), 1.0f / hypotheses.size());
    }
    
    // Weighted average of hypotheses (Bayesian fusion)
    size_t stateDim = hypotheses[0].size();
    result.stateVector.resize(stateDim, 0.0f);
    result.uncertaintyVector.resize(stateDim, 0.0f);
    
    float totalWeightSum = 0.0f;
    for (size_t i = 0; i < hypotheses.size(); ++i) {
        float weight = hypothesisWeights[i];
        totalWeightSum += weight;
        
        for (size_t j = 0; j < stateDim; ++j) {
            result.stateVector[j] += hypotheses[i][j] * weight;
        }
    }
    
    // Compute uncertainty as weighted variance
    for (size_t j = 0; j < stateDim; ++j) {
        float mean = result.stateVector[j];
        float variance = 0.0f;
        
        for (size_t i = 0; i < hypotheses.size(); ++i) {
            float diff = hypotheses[i][j] - mean;
            variance += hypothesisWeights[i] * diff * diff;
        }
        
        result.uncertaintyVector[j] = std::sqrt(variance + 0.01f);  // Add small noise
    }
    
    // Convert uncertainty to confidence
    float totalUncertainty = std::accumulate(result.uncertaintyVector.begin(), 
                                            result.uncertaintyVector.end(), 0.0f);
    result.confidence = 1.0f / (1.0f + totalUncertainty);
    
    return result;
}

void PredictionSystem::updatePredictions(const PredictionState& predicted,
                                       const std::vector<float>& actualObservation,
                                       Timestamp currentTime) {
    
    PredictionError error = computeBayesianError(predicted, actualObservation);
    error.timestamp = currentTime;
    
    // Add to error history
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 1000) {
        errorHistory_.pop_front();
    }
    
    // Update error components for learning
    for (size_t i = 0; i < error.errorComponents.size() && i < pImpl->errorComponents.size(); ++i) {
        pImpl->errorComponents[i] = pImpl->errorComponents[i] * (1.0f - learningRate_) +
                                  error.errorComponents[i] * learningRate_;
    }
}

PredictionError PredictionSystem::computeBayesianError(const PredictionState& predicted,
                                                     const std::vector<float>& actual) {
    PredictionError error;
    
    if (predicted.stateVector.size() != actual.size() || actual.empty()) {
        error.magnitude = 1.0f;
        error.uncertainty = 1.0f;
        return error;
    }
    
    // Compute component-wise error with uncertainty weighting
    error.errorComponents.resize(actual.size(), 0.0f);
    float weightedSum = 0.0f;
    float uncertaintySum = 0.0f;
    
    for (size_t i = 0; i < actual.size(); ++i) {
        float diff = predicted.stateVector[i] - actual[i];
        
        // Uncertainty-weighted error
        float uncertainty = predicted.uncertaintyVector.empty() ? 1.0f : predicted.uncertaintyVector[i];
        float weightedError = std::abs(diff) / (uncertainty + 0.01f);
        
        error.errorComponents[i] = weightedError;
        weightedSum += weightedError * weightedError;
        uncertaintySum += uncertainty;
    }
    
    // Overall magnitude (normalized)
    error.magnitude = std::sqrt(weightedSum / actual.size());
    
    // Uncertainty (average prediction uncertainty)
    float avgUncertainty = uncertaintySum / actual.size();
    error.uncertainty = std::min(1.0f, avgUncertainty * 2.0f);  // Scale to 0-1
    
    // Normalize to 0-1 range (assuming max reasonable error of 2.0)
    error.magnitude = std::min(1.0f, error.magnitude / 2.0f);
    
    return error;
}

void PredictionSystem::updatePredictionWeights(const PredictionError& error) {
    if (hypotheses_.empty()) return;
    
    float totalWeight = std::accumulate(hypothesisWeights_.begin(), hypothesisWeights_.end(), 0.0f);
    
    // Reduce weights of hypotheses that produced large errors
    for (size_t i = 0; i < hypotheses_.size(); ++i) {
        float weightAdjustment = (1.0f - error.magnitude) * learningRate_ * 
                               (0.5f + pImpl->rng->uniformReal(-0.1f, 0.1f));
        
        hypothesisWeights_[i] *= (1.0f - weightAdjustment);
    }
    
    // Add small exploration weights to less explored hypotheses
    float explorationMass = explorationFactor_ * (1.0f - totalWeight);
    if (explorationMass > 0.0f) {
        size_t randomIdx = static_cast<size_t>(pImpl->rng->uniformInt(0, hypotheses_.size() - 1));
        hypothesisWeights_[randomIdx] += explorationMass;
    }
}

void PredictionSystem::applyNeuromodulation(const PredictionError& error) {
    // Use brain reference to access neuromodulation systems
    if (!brain_ || !brain_->getPredictionErrorSignal()) return;
    
    // Convert prediction error to neuromodulatory signal
    float modulationStrength = error.magnitude * (1.0f - error.uncertainty);
    
    // Apply neuromodulation effects on prediction neurons
    auto* predRegion = brain_->getRegion(RegionId(1));  // Prediction region
    if (predRegion) {
        for (auto* syn : predRegion->getSynapses()) {
            float currentModulation = syn->getEligibilityTrace() * modulationStrength;
            syn->addToWeight(currentModulation * 0.01f);
        }
    }
}

std::vector<NeuronId> PredictionSystem::getPredictionNeurons() const {
    return pImpl->predictionNeurons;
}

std::vector<NeuronId> PredictionSystem::getActivePredictionNeurons() const {
    std::vector<NeuronId> activeNeurons;
    
    if (!brain_) return activeNeurons;
    
    // Check prediction neurons for activity
    for (NeuronId neuronId : pImpl->predictionNeurons) {
        for (const auto& region : brain_->getRegions()) {
            auto neurons = region->getAllNeurons();
            for (auto* neuron : neurons) {
                if (neuron->getId() == neuronId && neuron->isFiring()) {
                    activeNeurons.push_back(neuronId);
                }
            }
        }
    }
    
    return activeNeurons;
}

void PredictionSystem::updateFromNeuralActivity(const std::vector<float>& neuralPattern,
                                              Timestamp currentTime) {
    // Integrate neural activity patterns into prediction model
    if (neuralPattern.empty() || !brain_) return;
    
    // Use neural pattern to update prediction state
    currentPrediction_ = neuralPattern;
    
    // Check for prediction error patterns in neural activity
    std::vector<float> predictedFromPattern = generatePatternBasedPrediction(neuralPattern, currentTime);
    
    PredictionError error;
    if (!predictedFromPattern.empty()) {
        error = computeBayesianError(
            {predictedFromPattern, std::vector<float>(predictedFromPattern.size(), 0.01f), {}, 0.7f, 0.1f},
            neuralPattern);
    }
    
    if (error.magnitude > uncertaintyThreshold_) {
        // Significant prediction error - trigger learning
        updatePredictionWeights(error);
    }
}

float PredictionSystem::getPredictionError() const {
    if (errorHistory_.empty()) return 0.0f;
    return errorHistory_.back().magnitude;
}

float PredictionSystem::getConfidence() const {
    if (errorHistory_.empty()) return 0.5f;
    return 1.0f - errorHistory_.back().magnitude;
}

void PredictionSystem::clearHistory() {
    errorHistory_.clear();
    pImpl->errorComponents.clear();
}

void PredictionSystem::train(const std::vector<SensoryInput>& observations) {
    if (observations.empty() || !brain_) return;
    
    NLM_LOG_INFO(std::to_string(observations.size()) + " observations for prediction training");
    
    // Collect prediction states from neural activity
    std::vector<std::vector<float>> stateSequences;
    std::vector<std::vector<float>> nextStates;
    
    for (size_t i = 0; i < observations.size() - 1; ++i) {
        const auto& data1 = observations[i].getData();
        const auto& data2 = observations[i + 1].getData();
        
        if (!data1.empty() && !data2.empty()) {
            stateSequences.push_back(data1);
            nextStates.push_back(data2);
        }
    }
    
    // Learn temporal relationships using STDP
    learnTemporalPredictions(stateSequences, nextStates);
}

void PredictionSystem::learnTemporalPredictions(
    const std::vector<std::vector<float>>& states,
    const std::vector<std::vector<float>>& nextStates) {
    
    if (states.size() != nextStates.size()) return;
    
    // For each pair of states, find or create prediction pathway
    for (size_t i = 0; i < states.size(); ++i) {
        const auto& state = states[i];
        const auto& nextState = nextStates[i];
        
        // Find similar current states
        size_t bestMatch = findBestStateMatch(state, i > 10 ? i - 10 : 0, i);
        
        if (bestMatch < states.size()) {
            // We have a temporal sequence - strengthen prediction pathway
            strengthenPredictionPathway(states[bestMatch], nextState, 0.5f);
        }
    }
}

std::vector<float> PredictionSystem::generateNeuralPrediction(
    const std::vector<float>& currentState,
    size_t stepsAhead,
    Timestamp currentTime) {
    
    // In a real implementation, this would use actual spiking neural dynamics
    // For now, use a simplified model that incorporates prediction depth
    
    std::vector<float> prediction = currentState;
    
    // Apply prediction dynamics based on steps ahead
    for (size_t step = 0; step < stepsAhead; ++step) {
        // Simple forward dynamics model
        for (float& value : prediction) {
            // Add some dynamics based on prediction step
            float factor = static_cast<float>(stepsAhead - step) / static_cast<float>(stepsAhead);
            value = value * factor + pImpl->rng->uniformReal(-0.1f, 0.1f) * (1.0f - factor);
            
            // Apply boundedness (assuming state values are in 0-1 range)
            value = std::clamp(value, 0.0f, 1.0f);
        }
    }
    
    return prediction;
}

float PredictionSystem::calculateHypothesisWeight(
    const std::vector<float>& prediction,
    const std::vector<float>& currentState,
    size_t depth,
    Timestamp currentTime) {
    
    // Similarity between prediction and current state
    float similarity = computeSimilarity(prediction, currentState);
    
    // Reward based on prediction depth (deeper predictions are less likely)
    float depthPenalty = std::exp(-static_cast<float>(depth) / 2.0f);
    
    // Random exploration factor
    float exploration = pImpl->rng->uniformReal(0.0f, explorationFactor_);
    
    // Combine factors
    float weight = similarity * depthPenalty * (1.0f - exploration);
    
    return std::max(0.01f, weight);  // Ensure minimum weight
}

void PredictionSystem::strengthenPredictionPathway(
    const std::vector<float>& fromState,
    const std::vector<float>& toState,
    float strength) {
    
    // In a real implementation, this would strengthen specific synapses
    // For now, just log the learning
    NLM_LOG_INFO("Prediction pathway strengthened: " + std::to_string(fromState.size()) + " dim");
}

size_t PredictionSystem::findBestStateMatch(const std::vector<float>& targetState,
                                          size_t startIdx, size_t endIdx) {
    
    if (startIdx >= endIdx) return startIdx;
    
    float bestSimilarity = -1.0f;
    size_t bestIdx = startIdx;
    
    for (size_t i = startIdx; i <= endIdx && i < states.size(); ++i) {
        float sim = computeSimilarity(targetState, states[i]);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            bestIdx = i;
        }
    }
    
    return bestIdx;
}

std::vector<float> PredictionSystem::generatePatternBasedPrediction(
    const std::vector<float>& pattern,
    Timestamp currentTime) {
    
    // Convert neural pattern to state prediction
    std::vector<float> prediction = pattern;
    
    // Apply pattern transformation based on time
    float timeFactor = std::sin(currentTime * 0.01f) * 0.5f + 0.5f;
    
    for (float& value : prediction) {
        value = value * timeFactor + (1.0f - timeFactor) * value * 0.9f;
    }
    
    return prediction;
}

float PredictionSystem::computePredictionConfidence(const PredictionState& prediction) const {
    return prediction.confidence;
}

PredictionSystem::Statistics PredictionSystem::getStatistics() const {
    Statistics stats;
    stats.totalPredictions = errorHistory_.size();
    stats.successfulPredictions = 0;
    
    float totalError = 0.0f;
    float totalConfidence = 0.0f;
    
    for (const auto& error : errorHistory_) {
        totalError += error.magnitude;
        totalConfidence += (1.0f - error.magnitude);
        
        if (error.magnitude < uncertaintyThreshold_) {
            stats.successfulPredictions++;
        }
    }
    
    if (!errorHistory_.empty()) {
        stats.meanPredictionError = totalError / errorHistory_.size();
        stats.meanConfidence = totalConfidence / errorHistory_.size();
    }
    
    return stats;
}

float PredictionSystem::computeSimilarity(const std::vector<float>& a,
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

} // namespace nlm
