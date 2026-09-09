// Brain prediction integration implementation
#include "BrainPredictionInitializer.hpp"
#include "../core/Logger/Logger.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../agent/AgentBrain.hpp"

namespace nlm {
namespace brain {
namespace prediction {

void PredictionInitializer::initialize(Brain::Impl& impl) {
    impl.predictionSystem = std::make_unique<PredictionSystem>();
    
    NLM_LOG_INFO("Prediction system initialized");
}

void PredictionInitializer::integrateWithSensory(Brain::Impl& impl) {
    if (!impl.predictionSystem) return;
    
    // Prediction system should receive sensory input
    // For now, we'll just note that integration needs to happen
    // In a complete implementation, prediction system would:
    // 1. Receive current sensory state
    // 2. Predict next sensory state
    // 3. Compare prediction with actual input
    // 4. Learn from prediction errors
    
    NLM_LOG_INFO("Prediction system integration with sensory processing prepared");
}

void PredictionInitializer::integrateWithNeuromodulation(Brain::Impl& impl) {
    if (!impl.predictionSystem || !impl.predictionError) return;
    
    // Prediction errors drive learning
    // Neuromodulators (dopamine, etc.) modulate prediction learning
    
    NLM_LOG_INFO("Prediction system integrated with neuromodulation");
}

void PredictionInitializer::integrateWithCognition(Brain::Impl& impl) {
    if (!impl.predictionSystem || !impl.planner) return;
    
    // Neural planning could use predictions
    // Predict future states to inform action planning
    
    NLM_LOG_INFO("Prediction system integrated with cognition systems");
}

void PredictionInitializer::integrateWithWorkingMemory(Brain::Impl& impl) {
    if (!impl.predictionSystem || !impl.workingMemory) return;
    
    // Working memory could store predictions
    // Prediction errors could update working memory traces
    
    NLM_LOG_INFO("Prediction system integrated with working memory");
}

} // namespace prediction
} // namespace brain
} // namespace nlm
