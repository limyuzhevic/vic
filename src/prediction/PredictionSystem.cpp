#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>
#include <unordered_map>
#include <random>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    Neuromodulator* neuromodulator;
    NeuralWorkingMemory* workingMemory;
    
    // Neural prediction subsystem
    NeuralPrediction neuralPrediction;
    PredictionErrorSignal errorSignal;
    ActionConsequencePredictor actionPredictor;
    
    // Prediction state
    SimulationStep lastStep;
    SimulationStep currentStep;
    Timestamp currentTime;
    
    // Hierarchical prediction
    bool hierarchicalEnabled;
    size_t numTimeScales;
    std::vector<std::vector<float>> multiScalePredictions;
    std::vector<float> scaleConfidence;
    
    // Planning state
    std::vector<std::vector<float>> possibleFutureStates;
    std::vector<float> actionValues;
    std::vector<ActionType> actionHistory;
    
    // Learning state
    std::vector<std::pair<SensoryInput, std::vector<ActionType>>> experiences;
    std::vector<float> rewards;
    
    // Working memory integration
    std::unordered_map<NeuronId, float> predictionStates;
    
    // Expectation formation
    std::vector<float> currentExpectations;
    std::vector<float> expectationConfidence;
    
    // Surprise detection
    float surpriseThreshold;
    float accumulatedSurprise;
    SimulationStep surpriseHistorySize;
    
    // Predictive coding state
    std::vector<float> forwardModel;
    std::vector<float> inverseModel;
    std::vector<float> predictionErrorSignals;
    
    // Temporal difference components
    float tdError;
    float discountFactor;
    float learningRate;
    
    // Random number generator for exploration
    std::mt19937 rng;
    std::uniform_real_distribution<float> uniformDist;
    
    Impl() : brain(nullptr), neuromodulator(nullptr), workingMemory(nullptr),
             lastStep(0), currentStep(0), currentTime(0.0),
             hierarchicalEnabled(false), numTimeScales(1),
             surpriseThreshold(0.3f), accumulatedSurprise(0.0f),
             surpriseHistorySize(100), tdError(0.0f),
             discountFactor(0.95f), learningRate(0.1f) {
        rng.seed(std::random_device{}());
        uniformDist = std::uniform_real_distribution<float>(0.0f, 1.0f);
    }
};

PredictionSystem::PredictionSystem()
    : pImpl(new Impl)
    , neuralPrediction_(&pImpl->neuralPrediction)
    , brain_(nullptr)
    , neuromodulator_(nullptr)
    , workingMemory_(nullptr)
    , hierarchicalPredictionEnabled_(false)
    , numTimeScales_(1)
    , currentStep_(0)
    , currentTime_(0.0)
    , surpriseThreshold_(0.3f)
    , accumulatedSurprise_(0.0f)
    , tdError_(0.0f)
    , discountFactor_(0.95f)
    , learningRate_(0.1f) {}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain,
                                 Neuromodulator* neuromodulator,
                                 NeuralWorkingMemory* workingMemory) {
    brain_ = brain;
    neuromodulator_ = neuromodulator;
    workingMemory_ = workingMemory;
    
    if (brain_) {
        pImpl->brain = brain;
        pImpl->neuromodulator = neuromodulator;
        pImpl->workingMemory = workingMemory;
        
        pImpl->neuralPrediction.initialize(brain);
        pImpl->errorSignal.initialize(brain);
        pImpl->actionPredictor.initialize(brain);
        
        // Initialize forward and inverse models
        pImpl->forwardModel.resize(100, 0.0f);
        pImpl->inverseModel.resize(100, 0.0f);
        
        // Initialize expectations
        pImpl->currentExpectations.resize(50, 0.0f);
        pImpl->expectationConfidence.resize(50, 0.5f);
        
        // Initialize prediction error signals
        pImpl->predictionErrorSignals.resize(50, 0.0f);
        
        NLM_LOG_INFO("PredictionSystem initialized with integrated neural substrate");
    }
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!isPredictionReady()) {
        return currentState.clone();
    }
    
    // Generate prediction using forward model
    const auto& currentData = currentState.getData();
    
    // Use neural prediction to generate next state
    auto predictedData = pImpl->neuralPrediction.generatePrediction(currentStep_);
    
    // Apply predictive coding: combine forward model prediction with neural prediction
    std::vector<float> forwardPrediction = currentData;
    if (!pImpl->forwardModel.empty()) {
        // Simple forward model application (in real implementation, would be more complex)
        float weight = 0.7f;
        for (size_t i = 0; i < currentData.size() && i < pImpl->forwardModel.size(); ++i) {
            forwardPrediction[i] = weight * currentData[i] + (1.0f - weight) * pImpl->forwardModel[i];
        }
    }
    
    // Blend forward model with neural prediction
    std::vector<float> blendedPrediction = forwardPrediction;
    if (!predictedData.empty()) {
        for (size_t i = 0; i < std::min(predictedData.size(), blendedPrediction.size()); ++i) {
            blendedPrediction[i] = 0.6f * blendedPrediction[i] + 0.4f * predictedData[i];
        }
    }
    
    // Create new sensory input with predicted data
    if (currentState.getType() == "Vision") {
        auto& vision = const_cast<Vision&>(dynamic_cast<const Vision&>(currentState));
        auto prediction = std::make_unique<Vision>(vision.getWidth(), vision.getHeight(), vision.getChannels());
        prediction->setData(blendedPrediction);
        prediction->setTimestamp(currentState.getTimestamp() + 0.1);
        return prediction;
    }
    else if (currentState.getType() == "Audio") {
        auto& audio = const_cast<Audio&>(dynamic_cast<const Audio&>(currentState));
        auto prediction = std::make_unique<Audio>(audio.getSampleRate(), audio.getNumSamples());
        prediction->setData(blendedPrediction);
        prediction->setTimestamp(currentState.getTimestamp() + 0.1);
        return prediction;
    }
    else {
        auto internal = std::make_unique<InternalSignals>();
        for (float val : blendedPrediction) {
            internal->addSignal(val);
        }
        internal->setTimestamp(currentState.getTimestamp() + 0.1);
        return internal;
    }
}

std::vector<std::unique_ptr<SensoryInput>> PredictionSystem::predictSequence(
    const SensoryInput& currentState,
    size_t numSteps) {
    std::vector<std::unique_ptr<SensoryInput>> predictions;
    
    if (!isPredictionReady()) {
        predictions.push_back(currentState.clone());
        return predictions;
    }
    
    const auto& currentData = currentState.getData();
    
    // Generate multi-step prediction using temporal difference methods
    std::vector<float> currentStateCopy = currentData;
    
    for (size_t step = 0; step < numSteps; ++step) {
        // Generate prediction for this step
        std::vector<float> stepPrediction = currentStateCopy;
        
        // Apply temporal dynamics (simple model)
        if (step > 0) {
            for (size_t i = 0; i < stepPrediction.size() && i < pImpl->predictionErrorSignals.size(); ++i) {
                // Decay prediction error signals over time
                pImpl->predictionErrorSignals[i] *= pImpl->discountFactor;
                
                // Update state based on prediction errors
                stepPrediction[i] += pImpl->predictionErrorSignals[i] * 0.1f;
            }
        }
        
        // Store current prediction
        if (currentState.getType() == "Vision") {
            auto& vision = const_cast<Vision&>(dynamic_cast<const Vision&>(currentState));
            auto prediction = std::make_unique<Vision>(vision.getWidth(), vision.getHeight(), vision.getChannels());
            prediction->setData(stepPrediction);
            prediction->setTimestamp(currentState.getTimestamp() + 0.1 * (step + 1));
            predictions.push_back(std::move(prediction));
        }
        else if (currentState.getType() == "Audio") {
            auto& audio = const_cast<Audio&>(dynamic_cast<const Audio&>(currentState));
            auto prediction = std::make_unique<Audio>(audio.getSampleRate(), audio.getNumSamples());
            prediction->setData(stepPrediction);
            prediction->setTimestamp(currentState.getTimestamp() + 0.1 * (step + 1));
            predictions.push_back(std::move(prediction));
        }
        else {
            auto internal = std::make_unique<InternalSignals>();
            for (float val : stepPrediction) {
                internal->addSignal(val);
            }
            internal->setTimestamp(currentState.getTimestamp() + 0.1 * (step + 1));
            predictions.push_back(std::move(internal));
        }
        
        // Update state for next iteration
        currentStateCopy = stepPrediction;
        
        // Apply temporal difference update
        if (step < numSteps - 1) {
            pImpl->tdError = pImpl->discountFactor * pImpl->tdError;
            // Update expectations based on TD error
            for (size_t i = 0; i < std::min(stepPrediction.size(), pImpl->currentExpectations.size()); ++i) {
                pImpl->currentExpectations[i] += pImpl->learningRate * pImpl->tdError;
                pImpl->expectationConfidence[i] = std::min(1.0f, pImpl->expectationConfidence[i] + 0.01f);
            }
        }
    }
    
    return predictions;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted,
                                         const SensoryInput& actual) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Compute prediction error using multiple methods
    float error = 0.0f;
    if (!predData.empty() && !actualData.empty()) {
        if (predData.size() == actualData.size()) {
            float sumError = 0.0f;
            for (size_t i = 0; i < predData.size(); ++i) {
                float diff = predData[i] - actualData[i];
                sumError += diff * diff;
            }
            error = std::sqrt(sumError / predData.size());
        }
        else {
            // Pad shorter array with zeros for comparison
            size_t maxSize = std::max(predData.size(), actualData.size());
            float sumError = 0.0f;
            for (size_t i = 0; i < maxSize; ++i) {
                float pred = (i < predData.size()) ? predData[i] : 0.0f;
                float act = (i < actualData.size()) ? actualData[i] : 0.0f;
                float diff = pred - act;
                sumError += diff * diff;
            }
            error = std::sqrt(sumError / maxSize);
        }
    }
    
    // Update temporal state
    pImpl->lastStep = currentStep_;
    currentStep_++;
    currentTime_ += 0.1; // Fixed timestep for simplicity
    
    // Update neural prediction with actual observation
    pImpl->neuralPrediction.updateWithObservation(actualData, currentStep_);
    
    // Compute prediction error using neuromodulation system
    float neuromodulatoryError = 0.0f;
    if (neuromodulator_) {
        neuromodulatoryError = neuromodulator_->getRewardPredictionError();
    }
    
    // Combine errors
    float totalError = error * 0.7f + neuromodulatoryError * 0.3f;
    
    // Record error for learning
    pImpl->neuralPrediction.getErrorHistory().push_back(totalError);
    if (pImpl->neuralPrediction.getErrorHistory().size() > 10000) {
        pImpl->neuralPrediction.getErrorHistory().erase(
            pImpl->neuralPrediction.getErrorHistory().begin());
    }
    
    // Update expectation violation detection
    pImpl->accumulatedSurprise_ += totalError;
    if (pImpl->accumulatedSurprise_ > surpriseThreshold_) {
        NLM_LOG_INFO("Expectation violation detected - high surprise in prediction system");
    }
    
    // Store error for confidence calculation
    pImpl->lastPredictionError_.push_back(totalError);
    if (pImpl->lastPredictionError_.size() > 100) {
        pImpl->lastPredictionError_.erase(pImpl->lastPredictionError_.begin());
    }
    
    // Update forward and inverse models with prediction errors
    if (!predData.empty() && !actualData.empty() && predData.size() == actualData.size()) {
        for (size_t i = 0; i < predData.size() && i < pImpl->forwardModel.size(); ++i) {
            float predictionError = actualData[i] - predData[i];
            
            // Update forward model (learning from errors)
            pImpl->forwardModel[i] += learningRate_ * predictionError;
            
            // Update inverse model (mapping predictions back to actions)
            if (i < pImpl->inverseModel.size()) {
                pImpl->inverseModel[i] += learningRate_ * predictionError;
            }
            
            // Store prediction error signal
            if (i < pImpl->predictionErrorSignals.size()) {
                pImpl->predictionErrorSignals[i] = predictionError;
            }
        }
    }
}

float PredictionSystem::computePredictionError(const SensoryInput& predicted,
                                              const SensoryInput& actual,
                                              PredictionError* predictionErrorModule) {
    const auto& predData = predicted.getData();
    const auto& actualData = actual.getData();
    
    // Use prediction error module for sophisticated error computation
    float error = 0.0f;
    if (predictionErrorModule && predData.size() == actualData.size()) {
        for (size_t i = 0; i < predData.size(); ++i) {
            error += std::abs(predData[i] - actualData[i]);
        }
        error /= predData.size();
    }
    
    // Compute error components (intensity, spatial, temporal)
    if (!predData.empty() && !actualData.empty()) {
        float intensityError = 0.0f;
        float spatialError = 0.0f;
        
        for (size_t i = 0; i < std::min(predData.size(), actualData.size()); ++i) {
            float diff = predData[i] - actualData[i];
            intensityError += diff * diff;
            
            // Simple spatial error calculation
            if (i > 0 && i < predData.size() - 1) {
                float predSpatial = (predData[i-1] + predData[i+1]) / 2.0f;
                float actSpatial = (actualData[i-1] + actualData[i+1]) / 2.0f;
                spatialError += std::abs(predSpatial - actSpatial);
            }
        }
        
        intensityError = std::sqrt(intensityError / predData.size());
        spatialError /= std::max(1.0f, static_cast<float>(predData.size()) - 2.0f);
        
        // Update prediction error signal components
        if (predictionErrorModule) {
            predictionErrorModule->computeError(predData[0], actualData[0]);
        }
    }
    
    // Integrate with neuromodulation
    if (neuromodulator_) {
        // Send error signal to neuromodulator for learning modulation
        neuromodulator_->signalEvent(error, PredictionError);
    }
    
    return error;
}

std::unique_ptr<SensoryInput> PredictionSystem::predictActionConsequence(
    ActionType action,
    const SensoryInput& currentState) {
    if (!isPredictionReady()) {
        return currentState.clone();
    }
    
    const auto& currentData = currentState.getData();
    
    // Use action consequence predictor
    auto predictedData = pImpl->actionPredictor.predictConsequence(action, currentData);
    
    // Apply inverse model for action-effect prediction
    std::vector<float> inversePrediction = currentData;
    if (!pImpl->inverseModel.empty()) {
        for (size_t i = 0; i < currentData.size() && i < pImpl->inverseModel.size(); ++i) {
            // Apply inverse model to predict state after action
            inversePrediction[i] += pImpl->inverseModel[i] * 0.1f;
        }
    }
    
    // Blend action consequence prediction with inverse model prediction
    std::vector<float> blendedPrediction = inversePrediction;
    if (!predictedData.empty()) {
        for (size_t i = 0; i < std::min(predictedData.size(), blendedPrediction.size()); ++i) {
            blendedPrediction[i] = 0.7f * blendedPrediction[i] + 0.3f * predictedData[i];
        }
    }
    
    // Create prediction with same type as current state
    if (currentState.getType() == "Vision") {
        auto& vision = const_cast<Vision&>(dynamic_cast<const Vision&>(currentState));
        auto prediction = std::make_unique<Vision>(vision.getWidth(), vision.getHeight(), vision.getChannels());
        prediction->setData(blendedPrediction);
        prediction->setTimestamp(currentState.getTimestamp() + 0.1);
        return prediction;
    }
    else if (currentState.getType() == "Audio") {
        auto& audio = const_cast<Audio&>(dynamic_cast<const Audio&>(currentState));
        auto prediction = std::make_unique<Audio>(audio.getSampleRate(), audio.getNumSamples());
        prediction->setData(blendedPrediction);
        prediction->setTimestamp(currentState.getTimestamp() + 0.1);
        return prediction;
    }
    else {
        auto internal = std::make_unique<InternalSignals>();
        for (float val : blendedPrediction) {
            internal->addSignal(val);
        }
        internal->setTimestamp(currentState.getTimestamp() + 0.1);
        return internal;
    }
}

ActionType PredictionSystem::selectActionBasedOnPrediction(
    const std::vector<std::unique_ptr<SensoryInput>>& possibleStates) {
    if (possibleStates.empty() || !isPredictionReady()) {
        return ActionType::Wait;
    }
    
    // Evaluate each possible future state using internal model
    std::vector<float> stateValues;
    std::vector<std::vector<float>> stateSequences;
    
    for (const auto& state : possibleStates) {
        const auto& data = state->getData();
        
        // Simple evaluation: average value as proxy for goodness
        float value = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        
        // Apply action confidence from working memory
        float wmConfidence = getWorkingMemoryConfidence();
        float adjustedValue = value * wmConfidence;
        
        // Apply temporal discounting
        adjustedValue *= pImpl->discountFactor;
        
        // Apply prediction confidence
        float predictionConf = getConfidence();
        adjustedValue *= predictionConf;
        
        stateValues.push_back(adjustedValue);
        stateSequences.push_back(data);
    }
    
    // Find best state (highest value)
    size_t bestIndex = std::max_element(stateValues.begin(), stateValues.end()) - stateValues.begin();
    
    // Get the best sequence for planning
    const auto& bestSequence = stateSequences[bestIndex];
    
    // Apply exploration based on neuromodulation
    float explorationProbability = 0.0f;
    if (neuromodulator_) {
        explorationProbability = neuromodulator_->getExplorationBias();
    }
    
    float randomValue = pImpl->uniformDist(pImpl->rng);
    if (randomValue < explorationProbability) {
        // Explore: choose random action
        size_t numActions = 5; // Approximate number of action types
        size_t randomAction = static_cast<size_t>(pImpl->uniformDist(pImpl->rng) * numActions);
        switch (randomAction) {
            case 0: return ActionType::MoveForward;
            case 1: return ActionType::MoveBackward;
            case 2: return ActionType::TurnLeft;
            case 3: return ActionType::TurnRight;
            default: return ActionType::Wait;
        }
    }
    
    // Exploit: choose best action based on prediction
    // Map best state to action (simplified)
    switch (bestIndex % 5) {
        case 0: return ActionType::MoveForward;
        case 1: return ActionType::MoveBackward;
        case 2: return ActionType::TurnLeft;
        case 3: return ActionType::TurnRight;
        default: return ActionType::Wait;
    }
}

void PredictionSystem::enableHierarchicalPrediction(bool enable, size_t numScales) {
    hierarchicalPredictionEnabled_ = enable;
    numTimeScales_ = enable ? numScales : 1;
    
    if (enable) {
        pImpl->multiScalePredictions.resize(numScales);
        pImpl->scaleConfidence.resize(numScales, 0.5f);
        
        NLM_LOG_INFO("Hierarchical prediction enabled with " + std::to_string(numScales) + " time scales");
    }
}

float PredictionSystem::getPredictionError() const {
    if (pImpl->neuralPrediction.getPredictionError() > 0.0f) {
        return pImpl->neuralPrediction.getPredictionError();
    }
    return 0.0f;
}

float PredictionSystem::getConfidence() const {
    float avgError = 0.0f;
    const auto& errorHistory = pImpl->neuralPrediction.getErrorHistory();
    
    if (!errorHistory.empty()) {
        float sum = std::accumulate(errorHistory.begin(), errorHistory.end(), 0.0f);
        avgError = sum / errorHistory.size();
        return 1.0f - std::min(1.0f, avgError);
    }
    
    return 0.5f;
}

const std::vector<float>& PredictionSystem::getErrorHistory() const {
    return pImpl->neuralPrediction.getErrorHistory();
}

void PredictionSystem::clearHistory() {
    pImpl->neuralPrediction.clearHistory();
    pImpl->lastPredictionError_.clear();
    pImpl->currentExpectations.clear();
    pImpl->expectationConfidence.clear();
    pImpl->predictionErrorSignals.clear();
}

void PredictionSystem::train(const SensoryInput& observation,
                            const std::vector<ActionType>& actions,
                            float reward) {
    if (!brain_) return;
    
    const auto& obsData = observation.getData();
    
    // Store experience for future learning
    pImpl->experiences.push_back({observation.clone(), actions});
    pImpl->rewards.push_back(reward);
    
    if (pImpl->experiences.size() > 1000) {
        pImpl->experiences.erase(pImpl->experiences.begin());
        pImpl->rewards.erase(pImpl->rewards.begin());
    }
    
    // Update neural prediction with new experience
    pImpl->neuralPrediction.recordSensoryState(obsData, currentStep_);
    
    // Update action consequence predictor
    for (const auto& action : actions) {
        pImpl->actionPredictor.recordExperience(action, obsData, obsData, reward);
    }
    
    // Update forward model with reward prediction
    if (!pImpl->forwardModel.empty() && reward > 0.0f) {
        // Reinforce forward model predictions that led to positive rewards
        for (size_t i = 0; i < pImpl->forwardModel.size(); ++i) {
            pImpl->forwardModel[i] += learningRate_ * reward * 0.01f;
        }
    }
    
    // Update expectations based on reward
    for (size_t i = 0; i < std::min(obsData.size(), pImpl->currentExpectations.size()); ++i) {
        pImpl->currentExpectations[i] += learningRate_ * (reward - 0.5f);
        pImpl->expectationConfidence[i] = std::min(1.0f, pImpl->expectationConfidence[i] + learningRate_ * reward);
    }
    
    // Compute and update temporal difference error
    pImpl->tdError = reward - pImpl->currentExpectations.empty() ? 0.0f : 
                     pImpl->currentExpectations[0];
    
    // Update prediction error signals
    for (size_t i = 0; i < pImpl->predictionErrorSignals.size(); ++i) {
        pImpl->predictionErrorSignals[i] = pImpl->discountFactor * pImpl->predictionErrorSignals[i] + 
                                           learningRate_ * pImpl->tdError;
    }
    
    currentStep_++;
    
    // Store prediction in working memory for future reference
    storePredictionInWorkingMemory(obsData);
}

std::vector<NeuronId> PredictionSystem::getPredictionNeurons() const {
    std::vector<NeuronId> neurons;
    
    if (brain_) {
        for (const auto& region : brain_->getRegions()) {
            for (auto* neuron : region->getNeurons()) {
                // Check if neuron is involved in prediction (heuristic: recent spikes)
                if (neuron->getRecentSpikeCount() > 0) {
                    neurons.push_back(neuron->getId());
                }
            }
        }
    }
    
    return neurons;
}

void PredictionSystem::storePredictionInWorkingMemory(const std::vector<float>& prediction) {
    if (!workingMemory_ || prediction.empty()) return;
    
    // Store prediction pattern in working memory
    pImpl->workingMemory->store(prediction, 1.0f);
    
    // Update internal prediction states
    for (size_t i = 0; i < std::min(prediction.size(), pImpl->predictionStates.size()); ++i) {
        pImpl->predictionStates[NeuronId(i)] = prediction[i];
    }
}

std::vector<float> PredictionSystem::retrievePredictionFromWorkingMemory() const {
    std::vector<float> prediction;
    
    if (!workingMemory_) return prediction;
    
    // Retrieve working memory content
    prediction = workingMemory_->retrieve();
    
    // Merge with internal prediction states
    for (size_t i = 0; i < std::min(prediction.size(), pImpl->predictionStates.size()); ++i) {
        prediction[i] = (prediction[i] + pImpl->predictionStates[NeuronId(i)]) * 0.5f;
    }
    
    return prediction;
}

float PredictionSystem::getWorkingMemoryConfidence() const {
    if (!workingMemory_ || !brain_) return 0.0f;
    
    // Get memory activity level as confidence proxy
    float memoryActivity = workingMemory_->getMemoryActivity();
    
    // Get prediction confidence from neural prediction system
    float predictionConfidence = getConfidence();
    
    // Get expectation confidence
    float avgExpectationConfidence = 0.0f;
    if (!pImpl->expectationConfidence.empty()) {
        float sum = std::accumulate(pImpl->expectationConfidence.begin(), 
                                  pImpl->expectationConfidence.end(), 0.0f);
        avgExpectationConfidence = sum / pImpl->expectationConfidence.size();
    }
    
    // Combine all confidence factors
    return (memoryActivity + predictionConfidence + avgExpectationConfidence) / 3.0f;
}

} // namespace nlm
