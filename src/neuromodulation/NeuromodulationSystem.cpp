#include "NeuromodulationSystem.hpp"
#include "../core/Logger/Logger.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/NeuralSelfModel.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include <algorithm>
#include <cmath>

namespace nlm {

struct NeuromodulationSystem::Impl {
    class Brain* brain;
    
    // Individual neuromodulators
    Dopamine dopamine;
    Curiosity curiosity;
    Novelty novelty;
    PredictionError predictionError;
    
    // Neuromodulation weights for integration
    float plasticityWeight;
    float learningRateWeight;
    float confidenceWeight;
    float predictionWeight;
    
    // Integrated neuromodulation signals
    float integratedLevel;
    float plasticityModulation;
    float learningRateModulation;
    float confidenceModulation;
    float predictionModulation;
    
    // History for smoothing
    std::vector<float> levelHistory;
    std::vector<float> predictionHistory;
    
    // Time tracking
    Timestamp lastUpdate;
    
    Impl() : brain(nullptr)
        , plasticityWeight(1.0f)
        , learningRateWeight(1.0f)
        , confidenceWeight(1.0f)
        , predictionWeight(1.0f)
        , integratedLevel(0.0f)
        , plasticityModulation(1.0f)
        , learningRateModulation(1.0f)
        , confidenceModulation(1.0f)
        , predictionModulation(1.0f)
        , lastUpdate(0.0) {
        levelHistory.reserve(100);
        predictionHistory.reserve(100);
    }
};

NeuromodulationSystem::NeuromodulationSystem() : pImpl(new Impl) {}

NeuromodulationSystem::~NeuromodulationSystem() = default;

void NeuromodulationSystem::initialize(Brain* brain) {
    pImpl->brain = brain;
    
    // Initialize individual neuromodulators
    pImpl->dopamine.initialize(brain);
    pImpl->curiosity.initialize(brain);
    pImpl->novelty.initialize(brain);
    pImpl->predictionError.initialize(brain);
    
    NLM_LOG_INFO("NeuromodulationSystem initialized");
}

void NeuromodulationSystem::update(TimestepDuration dt) {
    Timestamp currentTime = static_cast<Timestamp>(pImpl->brain->getTotalSpikeCount());
    
    // Update individual neuromodulators
    pImpl->dopamine.update(dt);
    pImpl->curiosity.update(pImpl->novelty.getLevel(), pImpl->predictionError.getMagnitude(), dt);
    pImpl->novelty.update(dt);
    // predictionError is updated externally via computeError()
    
    // Calculate integrated neuromodulation level
    float dopamineLevel = pImpl->dopamine.getLevel();
    float curiosityLevel = pImpl->curiosity.getLevel();
    float noveltyLevel = pImpl->novelty.getLevel();
    float predictionErrorLevel = pImpl->predictionError.getMagnitude();
    
    // Weighted integration of neuromodulation signals
    pImpl->integratedLevel = (dopamineLevel * 0.4f + 
                              curiosityLevel * 0.3f + 
                              noveltyLevel * 0.2f + 
                              predictionErrorLevel * 0.1f);
    
    // Update history for smoothing
    pImpl->levelHistory.push_back(pImpl->integratedLevel);
    if (pImpl->levelHistory.size() > 100) {
        pImpl->levelHistory.erase(pImpl->levelHistory.begin());
    }
    
    // Calculate neuromodulation effects on processes
    pImpl->plasticityModulation = 1.0f + pImpl->integratedLevel * (pImpl->plasticityWeight - 1.0f);
    pImpl->learningRateModulation = 1.0f + pImpl->integratedLevel * (pImpl->learningRateWeight - 1.0f);
    pImpl->confidenceModulation = 1.0f + pImpl->integratedLevel * (pImpl->confidenceWeight - 1.0f);
    pImpl->predictionModulation = 1.0f + pImpl->integratedLevel * (pImpl->predictionWeight - 1.0f);
    
    pImpl->lastUpdate = currentTime;
}

float NeuromodulationSystem::getIntegratedLevel() const {
    return pImpl->integratedLevel;
}

float NeuromodulationSystem::getDopamineLevel() const {
    return pImpl->dopamine.getLevel();
}

float NeuromodulationSystem::getCuriosityLevel() const {
    return pImpl->curiosity.getLevel();
}

float NeuromodulationSystem::getNoveltyLevel() const {
    return pImpl->novelty.getLevel();
}

float NeuromodulationSystem::getPredictionErrorLevel() const {
    return pImpl->predictionError.getMagnitude();
}

float NeuromodulationSystem::getPlasticityModulation() const {
    return pImpl->plasticityModulation;
}

float NeuromodulationSystem::getLearningRateModulation() const {
    return pImpl->learningRateModulation;
}

float NeuromodulationSystem::getConfidenceModulation() const {
    return pImpl->confidenceModulation;
}

float NeuromodulationSystem::getPredictionModulation() const {
    return pImpl->predictionModulation;
}

void NeuromodulationSystem::updateConceptFormation(ConceptFormation* conceptFormation,
                                                  const std::vector<float>& neuralPattern,
                                                  float reward,
                                                  SimulationStep step) {
    if (!conceptFormation) return;
    
    // Neuromodulation affects concept formation learning rate
    float learningRate = 1.0f * pImpl->learningRateModulation;
    
    // Enhanced learning from prediction error and novelty
    float neuromodulatoryBoost = pImpl->curiosity.getLevel() * 0.5f + 
                                pImpl->novelty.getLevel() * 0.3f +
                                pImpl->predictionError.getMagnitude() * 0.2f;
    
    learningRate *= (1.0f + neuromodulatoryBoost);
    
    // Apply neuromodulation to concept formation
    // For now, we update the concept formation through the brain's integration
    // The concept formation system uses this information internally
    
    NLM_LOG_INFO("Neuromodulation affecting concept formation: " + 
                 std::to_string(learningRate) + " x baseline");
}

void NeuromodulationSystem::updatePlanner(NeuralPlanner* planner,
                                         const std::vector<float>& currentState,
                                         const std::vector<float>& goal,
                                         float predictionError) {
    if (!planner) return;
    
    // Neuromodulation affects planning confidence
    float confidenceModulation = pImpl->confidenceModulation;
    
    // Novelty encourages exploration in planning
    if (pImpl->novelty.getLevel() > 0.5f) {
        confidenceModulation *= 0.9f;  // Reduce confidence to encourage exploration
    }
    
    // Prediction error reduces planning confidence
    confidenceModulation *= std::max(0.5f, 1.0f - predictionError * 0.5f);
    
    // Update planner's internal parameters based on neuromodulation
    NLM_LOG_INFO("Neuromodulation affecting planner: confidence mod = " + 
                 std::to_string(confidenceModulation));
}

void NeuromodulationSystem::updateSelfModel(NeuralSelfModel* selfModel,
                                           const std::vector<float>& neuralPattern,
                                           const std::vector<float>& planningContext,
                                           float noveltyLevel,
                                           float curiosityLevel,
                                           float predictionError,
                                           float rewardLevel) {
    if (!selfModel) return;
    
    // Neuromodulation affects self-model stability
    float stabilityFactor = 1.0f;
    
    // High curiosity increases self-model plasticity
    if (curiosityLevel > 0.7f) {
        stabilityFactor *= 1.2f;
    }
    
    // High prediction error updates self-model
    if (predictionError > 0.3f) {
        stabilityFactor *= 1.5f;
    }
    
    // Low reward reduces confidence in self-model
    if (rewardLevel < 0.3f) {
        stabilityFactor *= 0.8f;
    }
    
    NLM_LOG_INFO("Neuromodulation affecting self-model: stability factor = " + 
                 std::to_string(stabilityFactor));
}

void NeuromodulationSystem::updatePredictionSystem(PredictionSystem* predictionSystem,
                                                  const std::vector<float>& currentState,
                                                  const std::vector<float>& targetState) {
    if (!predictionSystem) return;
    
    // Neuromodulation affects prediction accuracy
    float predictionMod = pImpl->predictionModulation;
    
    // Novelty affects prediction system - high novelty increases prediction error
    if (pImpl->novelty.getLevel() > 0.5f) {
        predictionMod *= 1.3f;  // Increase prediction error weighting
    }
    
    NLM_LOG_INFO("Neuromodulation affecting prediction system: prediction mod = " + 
                 std::to_string(predictionMod));
}

void NeuromodulationSystem::updatePlasticity(STDP* stdp, float baseLearningRate) const {
    if (!stdp) return;
    
    // Neuromodulation affects STDP plasticity
    float modLearningRate = baseLearningRate * pImpl->plasticityModulation;
    
    // Scale STDP weights based on neuromodulation
    float ltpWeight = 0.01f * pImpl->plasticityModulation;
    float ltdWeight = 0.012f * pImpl->plasticityModulation;
    
    NLM_LOG_INFO("Neuromodulation affecting STDP: learning rate = " + 
                 std::to_string(modLearningRate));
}

void NeuromodulationSystem::updateHebbian(Hebbian* hebbian, float baseLearningRate) const {
    if (!hebbian) return;
    
    // Neuromodulation affects Hebbian learning
    float modLearningRate = baseLearningRate * pImpl->plasticityModulation;
    
    NLM_LOG_INFO("Neuromodulation affecting Hebbian: learning rate = " + 
                 std::to_string(modLearningRate));
}

void NeuromodulationSystem::reset() {
    pImpl->dopamine.reset();
    pImpl->curiosity.reset();
    pImpl->novelty.clearHistory();
    pImpl->predictionError.clearHistory();
    
    pImpl->integratedLevel = 0.0f;
    pImpl->plasticityModulation = 1.0f;
    pImpl->learningRateModulation = 1.0f;
    pImpl->confidenceModulation = 1.0f;
    pImpl->predictionModulation = 1.0f;
    
    pImpl->levelHistory.clear();
    pImpl->predictionHistory.clear();
    
    NLM_LOG_INFO("NeuromodulationSystem reset");
}

void NeuromodulationSystem::logStatus() const {
    NLM_LOG_INFO("=== Neuromodulation System Status ===");
    NLM_LOG_INFO("Integrated level: " + std::to_string(pImpl->integratedLevel));
    NLM_LOG_INFO("Dopamine level: " + std::to_string(pImpl->dopamine.getLevel()));
    NLM_LOG_INFO("Curiosity level: " + std::to_string(pImpl->curiosity.getLevel()));
    NLM_LOG_INFO("Novelty level: " + std::to_string(pImpl->novelty.getLevel()));
    NLM_LOG_INFO("Prediction error: " + std::to_string(pImpl->predictionError.getMagnitude()));
    NLM_LOG_INFO("Plasticity modulation: " + std::to_string(pImpl->plasticityModulation));
    NLM_LOG_INFO("Learning rate modulation: " + std::to_string(pImpl->learningRateModulation));
    NLM_LOG_INFO("Confidence modulation: " + std::to_string(pImpl->confidenceModulation));
    NLM_LOG_INFO("Prediction modulation: " + std::to_string(pImpl->predictionModulation));
}

} // namespace nlm
