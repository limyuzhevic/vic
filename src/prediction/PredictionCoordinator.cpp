#include "PredictionCoordinator.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

struct PredictionCoordinator::Impl {
    std::shared_ptr<Config> config;
    Brain* brain;
    std::unique_ptr<PredictionSystem> predictionSystem;
    
    Impl(std::shared_ptr<Config> cfg) : config(cfg), brain(nullptr) {
        predictionSystem = std::make_unique<PredictionSystem>();
    }
    
    ~Impl() = default;
};

PredictionCoordinator::PredictionCoordinator(std::shared_ptr<Config> config) : pImpl(new Impl(config)), predictionHorizon(10), predictionThreshold(0.5f) {}

PredictionCoordinator::~PredictionCoordinator() {
    delete pImpl;
}

bool PredictionCoordinator::initialize(Brain* brain) {
    NLM_LOG_INFO("Initializing PredictionCoordinator...");
    
    pImpl->brain = brain;
    
    // The PredictionSystem doesn't have an initialize method currently
    // It will be updated through regular steps
    
    NLM_LOG_INFO("PredictionCoordinator initialized successfully");
    return true;
}

void PredictionCoordinator::update(Timestamp time, TimestepDuration timestep) {
    // Update prediction system
    // For now, just track prediction error history
    if (pImpl->predictionSystem) {
        // Prediction system would be updated with sensory observations
        // This would involve computing prediction errors
    }
}

float PredictionCoordinator::getPredictionError() const {
    // Return current prediction error
    return 0.0f;  // Placeholder
}

float PredictionCoordinator::getPredictionConfidence() const {
    // Return current prediction confidence
    return 1.0f;  // Placeholder
}

void PredictionCoordinator::reset() {
    NLM_LOG_INFO("Resetting PredictionCoordinator...");
    NLM_LOG_INFO("PredictionCoordinator reset complete");
}

} // namespace nlm