#include "BrainNeuromodulation.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// ===========================================
// DOPAMINE NEUROMODULATION
// ===========================================

void BrainNeuromodulation::updateDopamine(Dopamine* dopamine, float reward, float predictionError,
                                         TimestepDuration dt) {
    if (!dopamine) return;

    // Apply reward prediction error to dopamine
    dopamine->updatePredictionError(predictionError, dt);
    dopamine->updateLevel(reward, predictionError, dt);

    // Apply dopamine effects on neural excitability
    float dopamineLevel = dopamine->getLevel();
    float plasticityFactor = dopamine->getPlasticityFactor();
}

void BrainNeuromodulation::applyDopamineToNeurons(Dopamine* dopamine,
                                                 std::vector<Neuron*>& neurons) {
    if (!dopamine) return;

    float dopamineLevel = dopamine->getLevel();
    float excitabilityMod = dopamineLevel * 0.5f;
    if (excitabilityMod > 0.0f) {
        for (auto* neuron : neurons) {
            neuron->injectCurrent(excitabilityMod);
        }
    }
}

void BrainNeuromodulation::applyDopamineToPlasticity(Dopamine* dopamine,
                                                    STDP* stdp, Hebbian* hebbian) {
    if (!dopamine || !stdp || !hebbian) return;

    float plasticityFactor = dopamine->getPlasticityFactor();

    // Scale STDP learning rates based on dopamine
    float ltpWeight = 0.01f * plasticityFactor;
    float ltdWeight = 0.012f * plasticityFactor;
    float tau = 20.0f;

    stdp->configure(ltpWeight, ltdWeight, tau);

    // Apply to Hebbian plasticity
    float hebbianFactor = plasticityFactor;
}

// ===========================================
// CURiosity NEUROMODULATION
// ===========================================

void BrainNeuromodulation::updateCuriosity(Curiosity* curiosity, float noveltyLevel,
                                          float predictionError, TimestepDuration dt) {
    if (!curiosity) return;

    // Update curiosity based on novelty and prediction error
    curiosity->updateNovelty(noveltyLevel, dt);
    curiosity->updatePredictionError(predictionError, dt);
    curiosity->updateLevel(noveltyLevel, predictionError, dt);
}

void BrainNeuromodulation::applyCuriosityToActionSelection(Curiosity* curiosity,
                                                          MotorCommand currentAction,
                                                          float explorationChance) {
    if (!curiosity) return;

    float curiosityLevel = curiosity->getLevel();
    if (curiosityLevel > 0.3f) {
        // High curiosity = more exploration
        float exploreChance = curiosityLevel * explorationChance;
        // Would select random action based on exploreChance
    }
}

// ===========================================
// NOVELTY NEUROMODULATION
// ===========================================

void BrainNeuromodulation::updateNovelty(Novelty* novelty, const std::vector<float>& vision,
                                        const std::vector<float>& previousVision,
                                        TimestepDuration dt) {
    if (!novelty) return;

    // Compute novelty as difference from previous sensory input
    float totalDiff = 0.0f;
    for (size_t i = 0; i < vision.size() && i < previousVision.size(); ++i) {
        float diff = std::abs(vision[i] - previousVision[i]);
        totalDiff += diff;
    }

    float noveltyLevel = totalDiff / std::max<size_t>(vision.size(), 1);
    noveltyLevel *= 0.99f;  // Decay

    novelty->updateLevel(noveltyLevel, dt);
}

// ===========================================
// PREDICTION ERROR NEUROMODULATION
// ===========================================

void BrainNeuromodulation::updatePredictionError(PredictionError* predictionError,
                                                 float reward, float predictedReward,
                                                 TimestepDuration dt) {
    if (!predictionError) return;

    // Compute and update prediction error
    float error = reward - predictedReward;
    predictionError->updateError(error, dt);
    predictionError->updateLevel(error, dt);
}

void BrainNeuromodulation::applyPredictionErrorToLearning(PredictionError* predictionError,
                                                           std::vector<Synapse*>& synapses,
                                                           float learningRate) {
    if (!predictionError) return;

    float errorLevel = predictionError->getLevel();
    // Apply prediction error to synaptic learning
    for (auto* syn : synapses) {
        float eligibility = syn->getEligibilityTrace();
        float delta = eligibility * errorLevel * learningRate;
        syn->addToWeight(delta);
        syn->decayEligibilityTrace(0.1f);
    }
}

// ===========================================
// NEUROMODULATION STATISTICS AND ACCESSORS
// ===========================================

void BrainNeuromodulation::logNeuromodulationStatus(Dopamine* dopamine,
                                                   Curiosity* curiosity,
                                                   Novelty* novelty,
                                                   PredictionError* predictionError) {
    if (dopamine) {
        NLM_LOG_INFO("Dopamine level: " + std::to_string(dopamine->getLevel()));
    }
    if (curiosity) {
        NLM_LOG_INFO("Curiosity level: " + std::to_string(curiosity->getLevel()));
    }
    if (novelty) {
        NLM_LOG_INFO("Novelty level: " + std::to_string(novelty->getLevel()));
    }
    if (predictionError) {
        NLM_LOG_INFO("Prediction error level: " + std::to_string(predictionError->getLevel()));
    }
}

} // namespace nlm
