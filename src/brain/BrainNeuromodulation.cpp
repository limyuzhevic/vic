#include "BrainNeuromodulation.hpp"
#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../sensory/SensoryInput.hpp"
#include <algorithm>

namespace nlm {

void BrainNeuromodulation::configureFromConfig(const Config* config) {
    if (!config) return;
    
    baseDopamine_ = config->getOr<float>("neuromod.dopamine.base", 0.5f);
    curiosityGain_ = config->getOr<float>("neuromod.curiosity.gain", 1.0f);
    noveltyThreshold_ = config->getOr<float>("neuromod.novelty.threshold", 0.1f);
    predictionErrorWeight_ = config->getOr<float>("neuromod.prediction_error.weight", 0.5f);
    
    // Initialize neuromodulation systems
    dopamine_ = std::make_unique<Dopamine>();
    curiosity_ = std::make_unique<Curiosity>();
    novelty_ = std::make_unique<Novelty>();
    predictionError_ = std::make_unique<PredictionError>();
    
    NLM_LOG_INFO("BrainNeuromodulation configured from config:");
    NLM_LOG_INFO("  Base dopamine: " + std::to_string(baseDopamine_));
    NLM_LOG_INFO("  Curiosity gain: " + std::to_string(curiosityGain_));
    NLM_LOG_INFO("  Novelty threshold: " + std::to_string(noveltyThreshold_));
    NLM_LOG_INFO("  Prediction error weight: " + std::to_string(predictionErrorWeight_));
}

void BrainNeuromodulation::setNeuromodulationParameters(float baseDopamine, float curiosityGain, float noveltyThreshold) {
    baseDopamine_ = std::clamp(baseDopamine, 0.0f, 1.0f);
    curiosityGain_ = std::clamp(curiosityGain, 0.0f, 2.0f);
    noveltyThreshold_ = std::clamp(noveltyThreshold, 0.0f, 1.0f);
}

void BrainNeuromodulation::updateAllNeuromodulation(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Update all neuromodulation signals
    updateDopamineSignal(brain, timestep);
    updateNoveltySignal(brain, SensoryInput(), timestep);  // Placeholder for actual sensory input
    updateCuriositySignal(brain, noveltyLevel_, predictionErrorLevel_, timestep);
    updatePredictionErrorSignal(brain, SensoryInput(), timestep);  // Placeholder for actual sensory input
    
    // Apply neuromodulation effects on neural activity
    applyNeuromodulationEffects(brain, timestep);
    
    // Collect statistics
    collectNeuromodulationStats(brain);
}

void BrainNeuromodulation::updateDopamineSignal(Brain* brain, TimestepDuration timestep) {
    if (!dopamine_) return;
    
    // Update dopamine based on reward prediction error
    float reward = 0.0f;  // Would come from agent's reward system
    dopamine_->update(reward, timestep);
    
    dopamineLevel_ = dopamine_->getLevel();
}

void BrainNeuromodulation::updateCuriositySignal(Brain* brain, float novelty, float predictionError, TimestepDuration timestep) {
    if (!curiosity_) return;
    
    // Update curiosity based on novelty and prediction error
    curiosity_->update(novelty, predictionError, timestep);
    
    curiosityLevel_ = curiosity_->getLevel();
}

void BrainNeuromodulation::updateNoveltySignal(Brain* brain, const SensoryInput& sensoryInput, TimestepDuration timestep) {
    if (!novelty_) return;
    
    // Update novelty detection based on sensory input
    novelty_->update(sensoryInput, timestep);
    
    noveltyLevel_ = novelty_->getLevel();
}

void BrainNeuromodulation::updatePredictionErrorSignal(Brain* brain, const SensoryInput& sensoryInput, TimestepDuration timestep) {
    if (!predictionError_) return;
    
    // Update prediction error based on sensory input
    predictionError_->update(sensoryInput, timestep);
    
    predictionErrorLevel_ = predictionError_->getError();
}

void BrainNeuromodulation::applyNeuromodulationEffects(Brain* brain, TimestepDuration timestep) {
    if (!brain) return;
    
    // Apply dopamine effects on neural excitability
    if (dopamine_) {
        float dopamineLevel = dopamine_->getLevel();
        float excitabilityMod = dopamineLevel * 0.5f;
        
        if (excitabilityMod > 0.0f) {
            for (auto& region : brain->getRegions()) {
                for (auto& pop : region->getPopulations()) {
                    for (auto* neuron : pop->getNeurons()) {
                        neuron->injectCurrent(excitabilityMod);
                    }
                }
            }
        }
    }
    
    // Apply curiosity effects on exploration
    if (curiosity_) {
        float curiosityLevel = curiosity_->getLevel();
        float explorationMod = curiosityLevel * 0.3f;
        
        // Modify action selection probabilities based on curiosity
        // This would involve modifying the planner or action selection system
    }
    
    // Calculate overall neuromodulation influence
    neuromodulationInfluence_ = (dopamineLevel_ + curiosityLevel_ + noveltyLevel_) / 3.0f;
}

void BrainNeuromodulation::resetNeuromodulation(Brain* brain) {
    if (dopamine_) dopamine_->reset();
    if (curiosity_) curiosity_->reset();
    if (novelty_) novelty_->reset();
    if (predictionError_) predictionError_->reset();
    
    dopamineLevel_ = 0.0f;
    curiosityLevel_ = 0.0f;
    noveltyLevel_ = 0.0f;
    predictionErrorLevel_ = 0.0f;
    neuromodulationInfluence_ = 0.0f;
}

void BrainNeuromodulation::applyExternalStimuli(Brain* brain, float reward, float curiosityDrive) {
    if (!brain) return;
    
    // Apply external reward stimuli
    if (dopamine_) {
        // This would typically come from the agent's reward system
        float scaledReward = std::clamp(reward * 2.0f, 0.0f, 1.0f);
        dopamine_->setLevel(scaledReward);
    }
    
    // Apply curiosity drive
    if (curiosity_) {
        float scaledCuriosity = std::clamp(curiosityDrive, 0.0f, 1.0f);
        curiosity_->setLevel(scaledCuriosity);
    }
}

void BrainNeuromodulation::processRewardPrediction(Brain* brain, float reward, float predictedReward) {
    if (!dopamine_) return;
    
    // Process reward prediction error for dopamine modulation
    float rewardPredictionError = reward - predictedReward;
    dopamine_->setLevel(std::clamp(rewardPredictionError, 0.0f, 1.0f));
}

void BrainNeuromodulation::processExplorationBias(Brain* brain, float curiosity, float novelty) {
    if (!curiosity_) return;
    
    // Modify exploration behavior based on curiosity and novelty
    float explorationBias = curiosity * (1.0f + novelty);
    curiosity_->setLevel(std::clamp(explorationBias, 0.0f, 1.0f));
}

void BrainNeuromodulation::processSensorySurprise(Brain* brain, const SensoryInput& input) {
    if (!novelty_) return;
    
    // Detect surprising sensory input for novelty processing
    novelty_->update(input, 0.001);  // Use small timestep for surprise detection
}

void BrainNeuromodulation::collectNeuromodulationStats(Brain* brain) {
    // Collect neuromodulation system statistics
    // This would include various metrics about neuromodulator activity
    
    // For now, just ensure levels are within valid range
    dopamineLevel_ = std::clamp(dopamineLevel_, 0.0f, 1.0f);
    curiosityLevel_ = std::clamp(curiosityLevel_, 0.0f, 1.0f);
    noveltyLevel_ = std::clamp(noveltyLevel_, 0.0f, 1.0f);
    predictionErrorLevel_ = std::clamp(predictionErrorLevel_, 0.0f, 1.0f);
}

void BrainNeuromodulation::validateNeuromodulationState(Brain* brain) const {
    // Validate neuromodulation system state
    if (dopamine_ && (dopamine_->getLevel() < 0.0f || dopamine_->getLevel() > 1.0f)) {
        NLM_LOG_WARNING("Invalid dopamine level: " + std::to_string(dopamine_->getLevel()));
    }
    
    if (curiosity_ && (curiosity_->getLevel() < 0.0f || curiosity_->getLevel() > 1.0f)) {
        NLM_LOG_WARNING("Invalid curiosity level: " + std::to_string(curiosity_->getLevel()));
    }
}

} // namespace nlm