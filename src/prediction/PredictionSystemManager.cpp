#include "PredictionSystemManager.hpp"
#include "../core/Logger/Logger.hpp"
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

struct PredictionSystemManager::Impl {
    std::unique_ptr<PredictionSystem> predictionSystem;
    Brain* brain;
};

PredictionSystemManager::PredictionSystemManager() {
    pImpl = std::make_unique<Impl>();
    predictionSystem = std::make_unique<PredictionSystem>();
}

PredictionSystemManager::~PredictionSystemManager() = default;

void PredictionSystemManager::initialize(Brain* brain) {
    if (!brain) {
        NLM_LOG_ERROR("Cannot initialize PredictionSystemManager: invalid brain pointer");
        return;
    }
    
    pImpl->brain = brain;
    predictionSystem_->initialize(pImpl->brain);
    
    NLM_LOG_INFO("Prediction system initialized");
}

void PredictionSystemManager::update() {
    // Prediction system updates would be implemented here
    // Currently requires sensory input and predictions
    // TODO: Integrate with sensory input pipeline
    
    // For now, just track prediction error history as placeholder
    if (predictionSystem_) {
        float error = predictionSystem_->getPredictionError();
        if (error > 0.0f) {
            NLM_LOG_DEBUG("Prediction error: " + std::to_string(error));
        }
    }
}

void PredictionSystemManager::configureFromConfig(const Config& config) {
    // Configure prediction system parameters
    // TODO: Add prediction system specific configuration options
    
    NLM_LOG_INFO("Prediction system configured from config");
}

void PredictionSystemManager::reset() {
    if (predictionSystem_) {
        predictionSystem_->reset();
    }
    NLM_LOG_INFO("Prediction system reset");
}

} // namespace nlm