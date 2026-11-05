#include "BrainPrediction.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <algorithm>

namespace nlm {

void BrainPrediction::configureFromConfig(const Config* config) {
    if (!config) return;
    
    predictionConfidence_ = config->getOr<float>("prediction.confidence", 0.5f);
    predictionHorizon_ = config->getOr<float>("prediction.horizon", 1.0f);
    errorThreshold_ = config->getOr<float>("prediction.error.threshold", 0.1f);
    
    predictionWindow_ = static_cast<size_t>(config->getOr<int64_t>("prediction.window", 10));
    confidenceDecay_ = config->getOr<float>("prediction.confidence.decay", 0.95f);
    rewardPredictionWeight_ = config->getOr<float>("prediction.reward.weight", 0.3f);
    noveltyPredictionWeight_ = config->getOr<float>("prediction.novelty.weight", 0.7f);
    errorCorrectionFactor_ = config->getOr<float>("prediction.error.correction", 0.5f);
    
    // Initialize prediction systems
    predictionSystem_ = std::make_unique<PredictionSystem>();
    neuralPrediction_ = std::make_unique<NeuralPrediction>();
    
    NLM_LOG_INFO("BrainPrediction configured from config:");
    NLM_LOG_INFO("  Prediction confidence: " + std::to_string(predictionConfidence_));
    NLM_LOG_INFO("  Prediction horizon: " + std::to_string(predictionHorizon_));
    NLM_LOG_INFO("  Error threshold: " + std::to_string(errorThreshold_));
    NLM_LOG_INFO("  Prediction window: " + std::to_string(predictionWindow_));
}

void BrainPrediction::setPredictionParameters(float predictionConfidence, float predictionHorizon, float errorThreshold) {
    predictionConfidence_ = std::clamp(predictionConfidence, 0.0f, 1.0f);
    predictionHorizon_ = std::clamp(predictionHorizon, 0.1f, 10.0f);
    errorThreshold_ = std::clamp(errorThreshold, 0.01f, 1.0f);
}

void BrainPrediction::updateAllPrediction(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Update prediction systems
    predictionSystem_->update(brain, timestep);
    
    // Update neural predictions
    neuralPrediction_->update(brain, timestep);
    
    // Update prediction confidence based on prediction accuracy
    predictionConfidence_ *= confidenceDecay_;
    predictionAccuracy_ = std::clamp(predictionAccuracy_, 0.0f, 1.0f);
    
    // Collect statistics
    collectPredictionStats(brain);
}

void BrainPrediction::updatePredictionSystem(Brain* brain, const SensoryInput& sensoryInput) {
    if (!predictionSystem_) return;
    
    predictionSystem_->updatePrediction(brain, sensoryInput);
}

void BrainPrediction::updatePredictionError(Brain* brain, const SensoryInput& sensoryInput, TimestepDuration timestep) {
    if (!predictionSystem_) return;
    
    predictionSystem_->updatePredictionError(brain, sensoryInput, timestep);
}

void BrainPrediction::updatePredictionConfidence(Brain* brain, TimestepDuration timestep) {
    // Update prediction confidence based on prediction accuracy
    float confidenceUpdate = (predictionAccuracy_ - predictionConfidence_) * errorCorrectionFactor_;
    predictionConfidence_ += confidenceUpdate * timestep;
    
    predictionConfidence_ = std::clamp(predictionConfidence_, 0.0f, 1.0f);
}

void BrainPrediction::resetPrediction(Brain* brain) {
    if (predictionSystem_) predictionSystem_->reset();
    if (neuralPrediction_) neuralPrediction_->reset();
    
    predictionAccuracy_ = 0.5f;
}

void BrainPrediction::enablePrediction(Brain* brain, bool enable) {
    if (enable) {
        predictionSystem_->enable(true);
        neuralPrediction_->enable(true);
    } else {
        predictionSystem_->enable(false);
        neuralPrediction_->enable(false);
    }
}

void BrainPrediction::setPredictionHorizon(Brain* brain, float horizon) {
    predictionHorizon_ = std::clamp(horizon, 0.1f, 10.0f);
    
    if (predictionSystem_) {
        predictionSystem_->setPredictionHorizon(predictionHorizon_);
    }
}

void BrainPrediction::processPredictions(Brain* brain, const std::vector<Action>& actionHistory) {
    if (!brain || !neuralPrediction_) return;
    
    // Process historical action sequences for prediction
    neuralPrediction_->processActionHistory(brain, actionHistory);
}

void BrainPrediction::generatePredictions(Brain* brain, const SensoryInput& currentInput) {
    if (!brain || !neuralPrediction_) return;
    
    // Generate predictions based on current sensory input
    neuralPrediction_->generatePredictions(brain, currentInput);
}

void BrainPrediction::evaluatePredictionAccuracy(Brain* brain, const Action& actualAction) {
    if (!brain || !predictionSystem_) return;
    
    // Evaluate how well predictions matched actual outcomes
    float accuracy = predictionSystem_->evaluatePrediction(actualAction);
    
    // Update prediction accuracy based on evaluation
    predictionAccuracy_ = (predictionAccuracy_ * 0.9f) + (accuracy * 0.1f);
}

void BrainPrediction::updatePredictionModel(Brain* brain, const SensoryInput& input, float reward) {
    if (!brain || !predictionSystem_) return;
    
    // Update prediction model based on new input and reward
    predictionSystem_->updateModel(input, reward);
}

void BrainPrediction::handlePredictionFailures(Brain* brain, const SensoryInput& input) {
    if (!brain || !predictionSystem_) return;
    
    // Handle prediction failures by adjusting model
    float failurePenalty = 0.1f;
    predictionSystem_->handleFailure(input, failurePenalty);
}

void BrainPrediction::initializePredictionSystems(Brain* brain) {
    if (!brain) return;
    
    // Establish connections between prediction systems
    if (predictionSystem_ && neuralPrediction_) {
        predictionSystem_->setNeuralPredictionReference(neuralPrediction_.get());
    }
}

void BrainPrediction::validatePredictionState(Brain* brain) const {
    // Validate prediction system state
    if (predictionSystem_ && !predictionSystem_->isEnabled()) {
        NLM_LOG_WARNING("Prediction system is disabled");
    }
    
    if (neuralPrediction_ && !neuralPrediction_->isEnabled()) {
        NLM_LOG_WARNING("Neural prediction system is disabled");
    }
    
    if (predictionConfidence_ < 0.1f || predictionConfidence_ > 0.9f) {
        NLM_LOG_WARNING("Unusual prediction confidence: " + std::to_string(predictionConfidence_));
    }
}

void BrainPrediction::updatePredictionConnections(Brain* brain) {
    if (!brain) return;
    
    // Update connections between prediction systems
    // This would maintain consistency across prediction processes
}

void BrainPrediction::processPredictionLearning(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Process learning in prediction systems
    // This would adapt prediction parameters based on experience
}

} // namespace nlm