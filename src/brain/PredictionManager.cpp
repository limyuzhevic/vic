#include "PredictionManager.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

PredictionManager::PredictionManager(Brain* brain)
    : brain_(brain)
    , predictionSystem(std::make_unique<PredictionSystem>()) {
}

PredictionManager::~PredictionManager() = default;

bool PredictionManager::initialize() {
    NLM_LOG_INFO("Initializing Prediction Manager...");
    
    if (!brain_) return false;
    
    // Prediction system doesn't have initialize method currently
    // It's already initialized in Brain constructor
    
    NLM_LOG_INFO("Prediction Manager initialized successfully");
    return true;
}

void PredictionManager::reset() {
    // Prediction system maintains state across steps
}

void PredictionManager::step(double timestep) {
    updatePredictionSystem(timestep);
    updatePredictionError(timestep);
}

void PredictionManager::updatePredictionSystem(double timestep) {
    // The prediction system would be updated with sensory observations
    // For now, this is a placeholder for future implementation
    // Prediction would integrate with AgentBrain to process sensory input
    if (predictionSystem) {
        // Would process predictions and update confidence
    }
}

void PredictionManager::updatePredictionError(double timestep) {
    // Prediction error is computed and updated in AgentBrain
    // through reward modulation
}

} // namespace nlm
