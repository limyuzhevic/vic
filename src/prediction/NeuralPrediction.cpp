#include "NeuralPrediction.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <random>

namespace nlm {

struct NeuralPrediction::Impl {
    Brain* brain;
    PredictionSystem* predictionSystem;
    
    // Temporal sequence learning state
    std::vector<std::pair<std::vector<float>, NeuronId>> learnedSequences;
    std::vector<std::pair<NeuronId, NeuronId>> sequenceAssociations;
    
    // Pattern neurons (one per unique sensory pattern cluster)
    std::vector<NeuronId> patternNeurons;
    std::vector<std::vector<float>> patternRepresentations;
    
    // Recent states for sequence learning
    std::deque<std::pair<std::vector<float>, SimulationStep>> stateHistory;
    
    // Action-consequence learning using forward/backward models
    std::vector<std::vector<float>> actionConsequences;  // Indexed by action type
    std::vector<float> actionRewards;
    
    // Bayesian inference components
    std::vector<PredictiveDistribution> predictiveDistributions;
    std::vector<float> distributionWeights;
    
    // Neural prediction state
    std::vector<float> currentPrediction;
    std::vector<float> predictionUncertainty;
    
    // Learning parameters
    float learningRate;
    float explorationFactor;
    
    // Spiking neural dynamics for prediction
    struct PredictionNeuron {
        NeuronId id;
        std::vector<float> weights;
        std::vector<float> biases;
        std::vector<float> activationHistory;
        SimulationStep lastActive;
        
        PredictionNeuron() : id(INVALID_NEURON_ID), lastActive(-1) {}
    };
    
    std::vector<PredictionNeuron> predictionNeurons;
    
    Impl() : brain(nullptr), predictionSystem(nullptr), learningRate(0.01f), 
             explorationFactor(0.1f) {}
};

NeuralPrediction::NeuralPrediction()
    : pImpl(new Impl)
    , sequenceMemorySize_(10)
    , predictionHorizon_(1)
    , temporalPredictionEnabled_(true)
    , actionConsequenceEnabled_(true)
    , bayesianInferenceEnabled_(true)
    , brain_(nullptr)
    , predictionSystem_(nullptr)
    , predictionError_(0.0f)
    , predictionConfidence_(0.5f) {}

NeuralPrediction::~NeuralPrediction() = default;

void NeuralPrediction::initialize(Brain* brain, PredictionSystem* predictionSystem) {
    pImpl->brain = brain;
    pImpl->predictionSystem = predictionSystem;
    brain_ = brain;
    predictionSystem_ = predictionSystem;
    
    // Initialize action consequences storage
    pImpl->actionConsequences.resize(10);  // 10 action types
    pImpl->actionRewards.resize(10, 0.0f);
    
    // Initialize prediction neurons using brain's neural substrate
    initializePredictionNeurons();
    
    NLM_LOG_INFO("NeuralPrediction initialized with Bayesian inference and spiking dynamics");
}

void NeuralPrediction::initializePredictionNeurons() {
    if (!brain_) return;
    
    pImpl->predictionNeurons.clear();
    
    // Create a prediction-specific region
    RegionId predRegionId = brain_->addRegion("Prediction_Dynamics");
    auto* region = brain_->getRegion(predRegionId);
    
    if (region) {
        // Create prediction neuron population with diverse initial states
        PopulationId predPopId = region->addPopulation(50, NeuronType::Internal);
        auto* predPop = region->getPopulation(predPopId);
        
        if (predPop) {
            for (size_t i = 0; i < predPop->getNeurons().size(); ++i) {
                auto* neuron = predPop->getNeurons()[i];
                
                // Set neuron properties for prediction dynamics
                neuron->setThreshold(-35.0f);  // Slightly lower threshold for prediction
                neuron->setRestingPotential(-65.0f);
                neuron->setResetPotential(-65.0f);
                neuron->setLeakConductance(3.0f);
                
                // Enable plasticity for learning prediction dynamics
                neuron->enablePlasticity(true, true, true);  // STDP, Hebbian, reward-modulated
                
                // Initialize with diverse state for different prediction roles
                neuron->initializeRandom(*brain_->getRandomGenerator());
                
                // Store in prediction neurons list
                pImpl->predictionNeurons.push_back(neuron->getId());
                
                // Create prediction neuron entry
                Impl::PredictionNeuron predNeuron;
                predNeuron.id = neuron->getId();
                predNeuron.weights.resize(20, 0.0f);  // Input weights for prediction
                predNeuron.biases.resize(20, 0.0f);   // Bias terms
                predNeuron.activationHistory.reserve(100);
                
                // Initialize weights randomly
                float weightScale = 1.0f / std::sqrt(20.0f);
                for (float& weight : predNeuron.weights) {
                    weight = pImpl->brain->getRandomGenerator()->uniformReal(-weightScale, weightScale);
                }
                
                // Initialize biases
                for (float& bias : predNeuron.biases) {
                    bias = pImpl->brain->getRandomGenerator()->uniformReal(-1.0f, 1.0f);
                }
                
                // Store prediction neuron
                predictionNeurons_.push_back(predNeuron);
            }
        }
    }
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
    
    // Update predictive distributions with new observation
    if (bayesianInferenceEnabled_) {
        updatePredictiveDistribution(sensoryState, currentStep);
    }
}

std::pair<std::vector<float>, float> NeuralPrediction::generatePrediction(
    SimulationStep currentStep) {
    
    if (!temporalPredictionEnabled_ || recentSensoryStates_.empty()) {
        return {std::vector<float>(), 0.0f};
    }
    
    // Find best matching learned sequence
    const auto& currentState = recentSensoryStates_.back();
    
    // Method 1: Use learned sequence associations
    std::vector<float> predictedState = generateFromSequences(currentState);
    
    // Method 2: Use predictive distributions (Bayesian)
    if (bayesianInferenceEnabled_ && !pImpl->predictiveDistributions.empty()) {
        std::vector<float> bayesianPrediction = sampleFromPredictiveDistribution();
        if (!bayesianPrediction.empty() && currentState.size() == bayesianPrediction.size()) {
            // Blend learned sequences with Bayesian predictions
            float blendFactor = 0.6f;  // Weight for learned sequences
            predictedState.clear();
            for (size_t i = 0; i < currentState.size(); ++i) {
                predictedState.push_back(
                    currentState[i] * (1.0f - blendFactor) + 
                    bayesianPrediction[i] * blendFactor
                );
            }
        }
    }
    
    // Method 3: Use neural prediction neurons
    if (!predictionNeurons_.empty() && brain_) {
        std::vector<float> neuralPrediction = generateFromNeurons(currentState, currentStep);
        if (!neuralPrediction.empty() && currentState.size() == neuralPrediction.size()) {
            // Blend with other predictions
            float blendFactor = 0.4f;
            predictedState.clear();
            for (size_t i = 0; i < currentState.size(); ++i) {
                predictedState.push_back(
                    currentState[i] * (1.0f - blendFactor) + 
                    neuralPrediction[i] * blendFactor
                );
            }
        }
    }
    
    // If no prediction generated, return current state as baseline
    if (predictedState.empty()) {
        predictedState = currentState;
        predictionConfidence_ = 0.0f;
    } else {
        // Calculate confidence based on prediction quality
        predictionConfidence_ = calculatePredictionConfidence(currentState, predictedState);
    }
    
    return {predictedState, predictionConfidence_};
}

std::pair<std::vector<float>, float> NeuralPrediction::predictActionConsequence(
    ActionType action,
    const std::vector<float>& currentState,
    SimulationStep currentStep) {
    
    if (!actionConsequenceEnabled_) {
        return {currentState, 0.0f};
    }
    
    std::vector<float> predictedConsequence = currentState;
    float confidence = 0.0f;
    
    // Method 1: Use learned action-consequence mappings
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx < pImpl->actionConsequences.size() && 
        !pImpl->actionConsequences[actionIdx].empty()) {
        
        // Find most similar stored experience
        float bestSimilarity = 0.0f;
        std::vector<float> bestConsequence;
        
        for (size_t i = 0; i < pImpl->actionConsequences[actionIdx].size(); ++i) {
            float sim = computeSimilarity(currentState, 
                                         pImpl->actionConsequences[actionIdx][i]);
            if (sim > bestSimilarity) {
                bestSimilarity = sim;
                bestConsequence = pImpl->actionConsequences[actionIdx][i];
            }
        }
        
        if (bestSimilarity > 0.5f && !bestConsequence.empty()) {
            predictedConsequence = bestConsequence;
            confidence = bestSimilarity;
        }
    }
    
    // Method 2: Use forward model with neural dynamics
    std::vector<float> forwardPrediction = forwardModel(action, currentState, currentStep);
    if (!forwardPrediction.empty() && currentState.size() == forwardPrediction.size()) {
        float sim = computeSimilarity(currentState, forwardPrediction);
        if (sim > confidence) {
            confidence = sim;
            if (confidence > 0.5f) {
                predictedConsequence = forwardPrediction;
            }
        }
    }
    
    return {predictedConsequence, confidence};
}

float NeuralPrediction::updateWithObservation(const std::vector<float>& actualState,
                                           SimulationStep currentStep,
                                           const std::vector<float>& rewardSignals) {
    // Generate prediction first
    auto predictionResult = generatePrediction(currentStep);
    std::vector<float> predictedState = predictionResult.first;
    
    // Compute error
    float error = 1.0f;
    if (!predictedState.empty() && !actualState.empty()) {
        error = computePredictionError(predictedState, actualState);
    }
    
    // Add to error history
    errorHistory_.push_back(error);
    if (errorHistory_.size() > 1000) {
        errorHistory_.pop_front();
    }
    
    // Update prediction confidence based on error
    predictionConfidence_ = 1.0f - error;
    
    // Record the actual state for future learning
    recordSensoryState(actualState, currentStep);
    
    // Modulate learning based on error and rewards
    float reward = rewardSignals.empty() ? 1.0f - error : 
                  std::max(0.0f, rewardSignals[0]);
    modulatePredictionSynapses(error, reward);
    
    // Update action-consequence learning if action was recorded
    if (!recentActions_.empty()) {
        auto lastAction = recentActions_.back().first;
        learnActionConsequence(lastAction, predictedState, actualState, 
                              reward, currentStep);
    }
    
    predictionError_ = error;
    return error;
}

void NeuralPrediction::recordAction(ActionType action, SimulationStep step) {
    if (recentSensoryStates_.empty()) return;
    
    recentActions_.push_back({action, recentSensoryStates_.back()});
    if (recentActions_.size() > 100) {
        recentActions_.erase(recentActions_.begin());
    }
}

void NeuralPrediction::recordActionEffect(ActionType action,
                                         const std::vector<float>& beforeState,
                                         const std::vector<float>& afterState,
                                         float reward) {
    size_t actionIdx = static_cast<size_t>(action);
    if (actionIdx >= pImpl->actionConsequences.size()) return;
    
    // Store the action-consequence mapping
    pImpl->actionConsequences[actionIdx].push_back(afterState);
    pImpl->actionRewards[actionIdx] = (pImpl->actionRewards[actionIdx] * 0.9f) + 
                                     reward * 0.1f;
    
    // Keep memory bounded
    if (pImpl->actionConsequences[actionIdx].size() > 100) {
        pImpl->actionConsequences[actionIdx].erase(
            pImpl->actionConsequences[actionIdx].begin());
    }
}

void NeuralPrediction::learnTemporalSequence(const std::vector<float>& currentState,
                                            const std::vector<float>& nextState,
                                            SimulationStep currentStep) {
    // Find or create pattern neuron for current state
    NeuronId currentNeuron = findMatchingPatternNeuron(currentState);
    NeuronId nextNeuron = findMatchingPatternNeuron(nextState);
    
    if (currentNeuron != INVALID_NEURON_ID && nextNeuron != INVALID_NEURON_ID) {
        // Create sequence association
        createSequenceAssociation(currentNeuron, nextNeuron, 0.8f);
        
        // Store in learned sequences for prediction
        pImpl->learnedSequences.push_back({currentState, currentNeuron});
        if (pImpl->learnedSequences.size() > 1000) {
            pImpl->learnedSequences.erase(pImpl->learnedSequences.begin());
        }
    }
}

void NeuralPrediction::learnActionConsequence(ActionType action,
                                             const std::vector<float>& beforeState,
                                             const std::vector<float>& afterState,
                                             float reward,
                                             SimulationStep step) {
    // Use forward model to learn action consequences
    std::vector<float> forwardPred = forwardModel(action, beforeState, step);
    
    if (!forwardPred.empty() && computeSimilarity(forwardPred, afterState) > 0.7f) {
        // High prediction accuracy - strengthen forward model
        size_t stateDim = beforeState.size();
        for (size_t i = 0; i < stateDim; ++i) {
            float target = afterState[i];
            float prediction = forwardPred[i];
            float error = target - prediction;
            
            // Update forward model weights
            for (auto& predNeuron : predictionNeurons_) {
                // In real implementation, this would update synaptic weights
                // For now, just log the learning
            }
        }
    }
    
    // Learn backward model: infer actions from state changes
    std::vector<ActionType> inferredActions = inferActions(afterState);
    if (!inferredActions.empty()) {
        // Update backward model weights
        for (ActionType inferredAction : inferredActions) {
            size_t actionIdx = static_cast<size_t>(inferredAction);
            if (actionIdx < pImpl->actionConsequences.size()) {
                pImpl->actionConsequences[actionIdx].push_back(afterState);
            }
        }
    }
}

void NeuralPrediction::clearHistory() {
    errorHistory_.clear();
    recentSensoryStates_.clear();
    stateTimestamps_.clear();
    recentActions_.clear();
    pImpl->learnedSequences.clear();
    pImpl->sequenceAssociations.clear();
    pImpl->predictiveDistributions.clear();
    pImpl->distributionWeights.clear();
    
    // Reset prediction neurons
    for (auto& predNeuron : predictionNeurons_) {
        predNeuron.activationHistory.clear();
        predNeuron.lastActive = -1;
    }
}

void NeuralPrediction::updatePredictionFromNeuralActivity(
    const std::vector<float>& neuralPattern,
    Timestamp currentTime) {
    
    if (neuralPattern.empty() || !brain_) return;
    
    // Update predictive distributions with neural activity
    if (bayesianInferenceEnabled_) {
        updatePredictiveDistribution(neuralPattern, currentTime);
    }
    
    // Activate prediction neurons based on pattern
    updatePredictionNeurons(neuralPattern, currentTime);
}

void NeuralPrediction::modulatePredictionSynapses(float error, float reward) {
    if (!brain_) return;
    
    // Apply reward-modulated plasticity to prediction-related synapses
    for (const auto& region : brain_->getRegions()) {
        for (auto* syn : region->getSynapses()) {
            float eligibility = syn->getEligibilityTrace();
            
            // If this synapse was recently active in prediction pathway
            if (std::abs(eligibility) > 0.001f) {
                // Compute weight change based on prediction error
                float predictionError = std::min(1.0f, error);
                float learningSignal = (reward - 0.5f) * (1.0f - predictionError);
                
                float delta = eligibility * learningSignal * 0.01f;
                syn->addToWeight(delta);
                
                // Decay eligibility trace
                syn->decayEligibilityTrace(0.1f);
            }
        }
    }
}

std::vector<NeuronId> NeuralPrediction::getPredictionNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& neuron : predictionNeurons_) {
        result.push_back(neuron.id);
    }
    return result;
}

std::vector<NeuronId> NeuralPrediction::getSequenceNeurons() const {
    std::vector<NeuronId> result;
    for (const auto& assoc : pImpl->sequenceAssociations) {
        result.push_back(assoc.first);
        result.push_back(assoc.second);
    }
    return result;
}

std::vector<std::pair<std::vector<float>, float>> NeuralPrediction::predictMultipleSteps(
    SimulationStep currentStep,
    size_t numSteps,
    ActionType plannedAction) {
    
    std::vector<std::pair<std::vector<float>, float>> predictions;
    
    // Get current state for prediction
    std::vector<float> currentState;
    if (!recentSensoryStates_.empty()) {
        currentState = recentSensoryStates_.back();
    }
    
    if (currentState.empty()) return predictions;
    
    // Generate multi-step prediction
    std::vector<float> state = currentState;
    
    for (size_t step = 0; step < numSteps; ++step) {
        std::vector<float> nextState;
        float confidence = 0.0f;
        
        if (step == 0 && plannedAction != ActionType::Wait) {
            // First step uses action prediction
            auto actionResult = predictActionConsequence(plannedAction, state, currentStep + step);
            nextState = actionResult.first;
            confidence = actionResult.second;
        } else {
            // Subsequent steps use temporal prediction
            auto tempResult = generatePrediction(currentStep + step);
            nextState = tempResult.first;
            confidence = tempResult.second;
        }
        
        if (!nextState.empty()) {
            predictions.push_back({nextState, confidence});
            state = nextState;
        } else {
            break;
        }
    }
    
    return predictions;
}

void NeuralPrediction::updatePredictiveDistribution(const std::vector<float>& observation,
                                                 SimulationStep step) {
    
    // Compute statistics of recent observations for predictive distribution
    if (observation.empty()) return;
    
    PredictiveDistribution dist;
    dist.mean = observation;
    
    // Compute variance based on recent observations
    float varianceSum = 0.0f;
    size_t count = 0;
    
    for (size_t i = 0; i < std::min(size_t(10), recentSensoryStates_.size()); ++i) {
        size_t idx = recentSensoryStates_.size() - 1 - i;
        const auto& state = recentSensoryStates_[idx];
        
        if (state.size() == observation.size()) {
            for (size_t j = 0; j < observation.size(); ++j) {
                float diff = state[j] - observation[j];
                dist.variance.push_back(diff * diff);
                varianceSum += diff * diff;
            }
            count++;
        }
    }
    
    if (count > 0 && !observation.empty()) {
        // Compute average variance
        float avgVariance = varianceSum / (count * observation.size());
        dist.variance.resize(observation.size(), avgVariance);
        dist.weight = std::min(1.0f, static_cast<float>(count) / 10.0f);
        
        pImpl->predictiveDistributions.push_back(dist);
        pImpl->distributionWeights.push_back(dist.weight);
        
        // Keep distributions bounded
        if (pImpl->predictiveDistributions.size() > 50) {
            pImpl->predictiveDistributions.erase(
                pImpl->predictiveDistributions.begin());
            pImpl->distributionWeights.erase(
                pImpl->distributionWeights.begin());
        }
    }
}

std::vector<float> NeuralPrediction::sampleFromPredictiveDistribution() const {
    if (pImpl->predictiveDistributions.empty()) return std::vector<float>();
    
    // Sample from the most confident predictive distribution
    size_t bestIdx = 0;
    float bestWeight = 0.0f;
    
    for (size_t i = 0; i < pImpl->distributionWeights.size(); ++i) {
        if (pImpl->distributionWeights[i] > bestWeight) {
            bestWeight = pImpl->distributionWeights[i];
            bestIdx = i;
        }
    }
    
    const auto& dist = pImpl->predictiveDistributions[bestIdx];
    std::vector<float> sample = dist.mean;
    
    // Add noise based on variance
    for (size_t i = 0; i < sample.size() && i < dist.variance.size(); ++i) {
        float stdDev = std::sqrt(dist.variance[i]);
        float noise = (static_cast<float>(rand()) / RAND_MAX * 2.0f - 1.0f) * stdDev;
        sample[i] += noise;
        
        // Clamp to reasonable range
        sample[i] = std::max(0.0f, std::min(1.0f, sample[i]));
    }
    
    return sample;
}

std::vector<float> NeuralPrediction::forwardModel(
    ActionType action,
    const std::vector<float>& currentState,
    SimulationStep step) const {
    
    // Simple forward model that predicts action consequences
    std::vector<float> predictedState = currentState;
    
    // Apply action-specific dynamics
    switch (action) {
        case ActionType::MoveForward:
            // Simulate forward movement by shifting features
            for (size_t i = 0; i < predictedState.size(); ++i) {
                if (i > 0) {
                    predictedState[i - 1] = predictedState[i] * 0.9f;
                }
            }
            if (!predictedState.empty()) {
                predictedState.back() *= 1.1f;
            }
            break;
            
        case ActionType::MoveBackward:
            // Shift features in opposite direction
            for (size_t i = predictedState.size(); i > 0; --i) {
                predictedState[i] = predictedState[i - 1] * 0.9f;
            }
            if (!predictedState.empty()) {
                predictedState[0] *= 1.1f;
            }
            break;
            
        case ActionType::TurnLeft:
        case ActionType::TurnRight:
            // Rotate/scale features
            float rotation = (action == ActionType::TurnLeft) ? -0.1f : 0.1f;
            for (size_t i = 0; i < predictedState.size(); ++i) {
                predictedState[i] = predictedState[i] * std::cos(rotation) +
                                 (i < predictedState.size() - 1 ? predictedState[i + 1] : 0.0f) * std::sin(rotation);
            }
            break;
            
        case ActionType::Interact:
            // Significant state change for interaction
            for (size_t i = 0; i < predictedState.size(); ++i) {
                predictedState[i] = 0.5f + 0.5f * static_cast<float>(rand()) / RAND_MAX;
            }
            break;
            
        case ActionType::Wait:
            // Small decay for waiting
            for (size_t i = 0; i < predictedState.size(); ++i) {
                predictedState[i] *= 0.95f;
            }
            break;
    }
    
    return predictedState;
}

std::vector<ActionType> NeuralPrediction::inferActions(const std::vector<float>& state) const {
    std::vector<ActionType> possibleActions;
    
    // Simple heuristic for action inference based on state changes
    if (!state.empty()) {
        float mean = std::accumulate(state.begin(), state.end(), 0.0f) / state.size();
        float variance = 0.0f;
        
        for (float value : state) {
            variance += (value - mean) * (value - mean);
        }
        variance /= state.size();
        
        // Infer likely action based on state properties
        if (variance > 0.1f) {
            // High variance suggests interaction or turning
            possibleActions.push_back(ActionType::Interact);
            possibleActions.push_back(ActionType::TurnLeft);
            possibleActions.push_back(ActionType::TurnRight);
        } else if (mean > 0.7f) {
            // High mean suggests movement forward
            possibleActions.push_back(ActionType::MoveForward);
        } else if (mean < 0.3f) {
            // Low mean suggests movement backward
            possibleActions.push_back(ActionType::MoveBackward);
        }
    }
    
    return possibleActions;
}

std::vector<float> NeuralPrediction::generateFromSequences(
    const std::vector<float>& currentState) const {
    
    std::vector<float> predictedState = currentState;
    float bestSimilarity = 0.0f;
    
    // Find most similar learned sequence
    for (const auto& seq : pImpl->learnedSequences) {
        float sim = computeSimilarity(currentState, seq.first);
        if (sim > bestSimilarity) {
            bestSimilarity = sim;
            
            // If we have an association, get the next state
            for (const auto& assoc : pImpl->sequenceAssociations) {
                if (assoc.first == seq.second) {
                    // Find matching pattern for next state
                    for (size_t i = 0; i < pImpl->patternNeurons.size(); ++i) {
                        if (pImpl->patternNeurons[i] == assoc.second) {
                            predictedState = pImpl->patternRepresentations[i];
                            break;
                        }
                    }
                    break;
                }
            }
        }
    }
    
    return predictedState;
}

std::vector<float> NeuralPrediction::generateFromNeurons(
    const std::vector<float>& currentState,
    SimulationStep step) {
    
    std::vector<float> prediction = currentState;
    
    // Use neural prediction neurons to generate prediction
    for (const auto& predNeuron : predictionNeurons_) {
        // Check if neuron is active
        bool isActive = false;
        if (brain_) {
            for (const auto& region : brain_->getRegions()) {
                auto neurons = region->getAllNeurons();
                for (auto* neuron : neurons) {
                    if (neuron->getId() == predNeuron.id && neuron->isFiring()) {
                        isActive = true;
                        break;
                    }
                }
                if (isActive) break;
            }
        }
        
        if (isActive) {
            // Neuron contributes to prediction
            for (size_t i = 0; i < std::min<size_t>(prediction.size(), predNeuron.weights.size()); ++i) {
                prediction[i] += predNeuron.weights[i];
            }
        }
    }
    
    // Normalize prediction
    float sum = std::accumulate(prediction.begin(), prediction.end(), 0.0f);
    if (sum > 0.0f) {
        for (float& value : prediction) {
            value /= sum;
        }
    }
    
    return prediction;
}

void NeuralPrediction::updatePredictionNeurons(const std::vector<float>& pattern,
                                             Timestamp currentTime) {
    
    for (auto& predNeuron : predictionNeurons_) {
        // Compute activation based on input pattern
        float activation = 0.0f;
        
        for (size_t i = 0; i < std::min<size_t>(pattern.size(), predNeuron.weights.size()); ++i) {
            activation += pattern[i] * predNeuron.weights[i] + predNeuron.biases[i];
        }
        
        // Apply sigmoid activation
        activation = 1.0f / (1.0f + std::exp(-activation * 10.0f));
        
        // Update activation history
        predNeuron.activationHistory.push_back(activation);
        if (predNeuron.activationHistory.size() > 100) {
            predNeuron.activationHistory.erase(predNeuron.activationHistory.begin());
        }
        
        // Store last activation time
        if (activation > 0.5f) {
            predNeuron.lastActive = currentTime;
        }
        
        // In a real implementation, this would update actual neuron membrane potentials
        // and trigger spikes based on activation thresholds
    }
}

float NeuralPrediction::calculatePredictionConfidence(
    const std::vector<float>& currentState,
    const std::vector<float>& predictedState) const {
    
    if (currentState.size() != predictedState.size() || currentState.empty()) {
        return 0.0f;
    }
    
    // Similarity-based confidence
    float similarity = computeSimilarity(currentState, predictedState);
    
    // Bonus for temporal coherence with recent states
    float temporalBonus = 0.0f;
    if (!recentSensoryStates_.empty()) {
        size_t minSize = std::min<size_t>(3, recentSensoryStates_.size());
        float temporalCoherence = 0.0f;
        
        for (size_t i = 0; i < minSize - 1; ++i) {
            size_t idx1 = recentSensoryStates_.size() - 1 - i;
            size_t idx2 = recentSensoryStates_.size() - 1 - i - 1;
            
            if (idx1 < recentSensoryStates_.size() && idx2 < recentSensoryStates_.size()) {
                float sim = computeSimilarity(recentSensoryStates_[idx1], 
                                            recentSensoryStates_[idx2]);
                temporalCoherence += sim;
            }
        }
        
        if (minSize > 1) {
            temporalBonus = temporalCoherence / (minSize - 1);
        }
    }
    
    // Combine similarity and temporal coherence
    float confidence = (similarity * 0.7f) + (temporalBonus * 0.3f);
    
    return std::min(1.0f, confidence * 2.0f);  // Scale to 0-1
}

float NeuralPrediction::computePredictionError(
    const std::vector<float>& predicted,
    const std::vector<float>& actual) const {
    
    if (predicted.size() != actual.size() || predicted.empty()) {
        return 1.0f;
    }
    
    float totalError = 0.0f;
    for (size_t i = 0; i < predicted.size(); ++i) {
        float diff = predicted[i] - actual[i];
        totalError += diff * diff;
    }
    
    float meanError = totalError / predicted.size();
    return std::min(1.0f, std::sqrt(meanError));
}

float NeuralPrediction::computeSimilarity(const std::vector<float>& a,
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

NeuralPrediction::PredictionStats NeuralPrediction::getStats() const {
    PredictionStats stats;
    stats.totalPredictions = errorHistory_.size();
    stats.successfulPredictions = 0;
    
    float totalError = 0.0f;
    float totalConfidence = 0.0f;
    float totalExploration = 0.0f;
    
    for (float error : errorHistory_) {
        totalError += error;
        totalConfidence += (1.0f - error);
        
        if (error < 0.5f) {  // Consider prediction successful if error < 0.5
            stats.successfulPredictions++;
        }
    }
    
    if (!errorHistory_.empty()) {
        stats.meanPredictionError = totalError / errorHistory_.size();
        stats.meanConfidence = totalConfidence / errorHistory_.size();
    }
    
    // Estimate exploration rate from recent actions
    float recentExploration = 0.0f;
    size_t actionCount = 0;
    
    for (size_t i = 0; i < std::min<size_t>(10, recentActions_.size()); ++i) {
        if (recentActions_[recentActions_.size() - 1 - i].first == ActionType::Wait) {
            recentExploration += 1.0f;
        }
        actionCount++;
    }
    
    stats.explorationRate = (actionCount > 0) ? (recentExploration / actionCount) : 0.0f;
    
    return stats;
}

} // namespace nlm