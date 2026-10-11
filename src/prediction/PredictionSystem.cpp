#include "PredictionSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace nlm {

struct PredictionSystem::Impl {
    Brain* brain;
    NeuralWorkingMemory* workingMemory;
    
    // Integration with neural prediction system
    std::unique_ptr<NeuralPrediction> neuralPrediction;
    
    // Current state tracking
    std::vector<float> currentSensoryPattern;
    std::vector<float> predictedPattern;
    float predictionError;
    float predictionConfidence;
    
    // Learning parameters
    float predictionErrorThreshold;
    float confidenceUpdateRate;
    float errorDecayRate;
    
    Impl() : brain(nullptr), workingMemory(nullptr), 
             predictionError(0.0f), predictionConfidence(0.0f),
             predictionErrorThreshold(0.5f), confidenceUpdateRate(0.1f),
             errorDecayRate(0.01f) {
        neuralPrediction = std::make_unique<NeuralPrediction>();
    }
};

PredictionSystem::PredictionSystem() 
    : pImpl(new Impl)
    , workingMemory_(nullptr)
    , brain_(nullptr)
    , temporalPredictionEnabled_(true)
    , actionConsequencePredictionEnabled_(true) {
}

PredictionSystem::~PredictionSystem() = default;

void PredictionSystem::initialize(Brain* brain, NeuralWorkingMemory* workingMemory) {
    if (!brain) {
        NLM_LOG_ERROR("PredictionSystem: Cannot initialize with null brain pointer");
        return;
    }
    
    brain_ = brain;
    workingMemory_ = workingMemory;
    
    // Initialize neural prediction system with brain reference
    pImpl->brain = brain;
    pImpl->workingMemory = workingMemory;
    pImpl->neuralPrediction->initialize(brain);
    
    // Configure prediction parameters
    pImpl->neuralPrediction->enableTemporalPrediction(temporalPredictionEnabled_);
    pImpl->neuralPrediction->enableActionConsequencePrediction(actionConsequencePredictionEnabled_);
    
    NLM_LOG_INFO("PredictionSystem initialized with brain reference and working memory integration");
}

std::unique_ptr<SensoryInput> PredictionSystem::predictNextState(const SensoryInput& currentState) {
    if (!brain_ || !pImpl->neuralPrediction) {
        NLM_LOG_ERROR("PredictionSystem::predictNextState: Not initialized properly");
        return currentState.clone();
    }
    
    // Convert current state to neural pattern
    std::vector<float> currentPattern = sensoryInputToNeuralPattern(currentState);
    pImpl->currentSensoryPattern = currentPattern;
    
    // Store current state in neural prediction system for learning
    static SimulationStep stepCounter = 0;
    pImpl->neuralPrediction->recordSensoryState(currentPattern, stepCounter++);
    
    // Generate prediction using neural prediction system
    std::vector<float> predictedPattern = pImpl->neuralPrediction->generatePrediction(stepCounter);
    
    if (predictedPattern.empty()) {
        // No prediction available - return current state as fallback
        NLM_LOG_WARNING("PredictionSystem::predictNextState: No prediction generated, returning current state");
        pImpl->predictionError = 1.0f;  // Maximum error
        pImpl->predictionConfidence = 0.0f;
        return currentState.clone();
    }
    
    pImpl->predictedPattern = predictedPattern;
    pImpl->predictionConfidence = pImpl->neuralPrediction->getPredictionConfidence();
    
    // Convert back to SensoryInput (Vision for now, but could be extended)
    auto predictedInput = neuralPatternToSensoryInput(predictedPattern);
    
    NLM_LOG_DEBUG("PredictionSystem::predictNextState: Generated prediction with confidence " + 
                 std::to_string(pImpl->predictionConfidence));
    
    return predictedInput;
}

void PredictionSystem::updatePredictions(const SensoryInput& predicted, const SensoryInput& actual) {
    if (!brain_ || !pImpl->neuralPrediction) {
        return;
    }
    
    // Convert to neural patterns for error computation
    std::vector<float> predictedPattern = sensoryInputToNeuralPattern(predicted);
    std::vector<float> actualPattern = sensoryInputToNeuralPattern(actual);
    
    // Update neural prediction system with actual observation
    static SimulationStep stepCounter = 0;
    float error = pImpl->neuralPrediction->updateWithObservation(actualPattern, stepCounter++);
    
    // Store error for history and confidence calculation
    pImpl->predictionError = error;
    errorHistory_.push_back(error);
    
    // Maintain error history size
    if (errorHistory_.size() > 1000) {
        errorHistory_.pop_front();
    }
    
    // Update confidence based on error (inverse relationship)
    pImpl->predictionConfidence = std::max(0.0f, 1.0f - error * 2.0f);
    pImpl->predictionConfidence = std::min(1.0f, pImpl->predictionConfidence);
    
    // Decay error over time (for confidence calculation)
    for (auto& e : errorHistory_) {
        e = std::max(0.0f, e - pImpl->errorDecayRate);
    }
    
    NLM_LOG_DEBUG("PredictionSystem::updatePredictions: Error = " + std::to_string(error) + 
                 ", Confidence = " + std::to_string(pImpl->predictionConfidence));
}

float PredictionSystem::getPredictionError() const {
    return pImpl->predictionError;
}

float PredictionSystem::getPredictionConfidence() const {
    return pImpl->predictionConfidence;
}

const std::deque<float>& PredictionSystem::getErrorHistory() const {
    return errorHistory_;
}

void PredictionSystem::clearHistory() {
    errorHistory_.clear();
    if (pImpl->neuralPrediction) {
        pImpl->neuralPrediction->clearHistory();
    }
}

void PredictionSystem::train(const SensoryInput& observation) {
    if (!brain_ || !pImpl->neuralPrediction) {
        return;
    }
    
    // Convert observation to neural pattern
    std::vector<float> observationPattern = sensoryInputToNeuralPattern(observation);
    
    // Store in working memory for future predictions
    if (workingMemory_) {
        workingMemory_->store(observationPattern, 1.0f);
    }
    
    // Train neural prediction system
    static SimulationStep stepCounter = 0;
    pImpl->neuralPrediction->recordSensoryState(observationPattern, stepCounter++);
    
    NLM_LOG_DEBUG("PredictionSystem::train: Trained with observation pattern");
}

void PredictionSystem::processSensoryInput(const SensoryInput& input) {
    if (!brain_ || !pImpl->neuralPrediction) {
        return;
    }
    
    // Convert to neural pattern
    std::vector<float> inputPattern = sensoryInputToNeuralPattern(input);
    
    // Store in working memory
    if (workingMemory_) {
        workingMemory_->store(inputPattern, 1.0f);
    }
    
    // Record in neural prediction system
    static SimulationStep stepCounter = 0;
    pImpl->neuralPrediction->recordSensoryState(inputPattern, stepCounter++);
    
    // Update prediction based on this input
    // (This is called by the brain during step processing)
}

std::vector<float> PredictionSystem::getWorkingMemoryPatterns() const {
    if (!workingMemory_) {
        return std::vector<float>();
    }
    
    return workingMemory_->retrieve();
}

std::unique_ptr<SensoryInput> PredictionSystem::neuralPatternToSensoryInput(const std::vector<float>& pattern) const {
    // For now, create a Vision sensory input
    // In a real implementation, this would be more sophisticated
    size_t patternSize = pattern.size();
    
    // Determine image dimensions based on pattern size
    // This is a simplified approach - real implementation would know the format
    size_t width = 16;
    size_t height = 16;
    
    if (patternSize <= width * height) {
        auto vision = std::make_unique<Vision>(width, height);
        vision->setData(pattern);
        return vision;
    } else {
        // Truncate pattern
        std::vector<float> truncated(pattern.begin(), pattern.begin() + width * height);
        auto vision = std::make_unique<Vision>(width, height);
        vision->setData(truncated);
        return vision;
    }
}

std::vector<float> PredictionSystem::sensoryInputToNeuralPattern(const SensoryInput& input) const {
    // Extract neural pattern from sensory input
    const std::vector<float>& data = input.getData();
    return data;
}

} // namespace nlm
