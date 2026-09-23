#pragma once

#include "../core/Types/Types.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include <vector>

namespace nlm {

// Forward declarations
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;
class Neuron;
class Synapse;
class STDP;
class Hebbian;

class BrainNeuromodulation {
public:
    // ===========================================
    // DOPAMINE NEUROMODULATION
    // ===========================================
    
    static void updateDopamine(Dopamine* dopamine, float reward, float predictionError,
                               TimestepDuration dt);
    static void applyDopamineToNeurons(Dopamine* dopamine, std::vector<Neuron*>& neurons);
    static void applyDopamineToPlasticity(Dopamine* dopamine, STDP* stdp, Hebbian* hebbian);
    
    // ===========================================
    // CURiosity NEUROMODULATION
    // ===========================================
    
    static void updateCuriosity(Curiosity* curiosity, float noveltyLevel,
                                float predictionError, TimestepDuration dt);
    static void applyCuriosityToActionSelection(Curiosity* curiosity,
                                                MotorCommand currentAction,
                                                float explorationChance);
    
    // ===========================================
    // NOVELTY NEUROMODULATION
    // ===========================================
    
    static void updateNovelty(Novelty* novelty, const std::vector<float>& vision,
                              const std::vector<float>& previousVision,
                              TimestepDuration dt);
    
    // ===========================================
    // PREDICTION ERROR NEUROMODULATION
    // ===========================================
    
    static void updatePredictionError(PredictionError* predictionError,
                                      float reward, float predictedReward,
                                      TimestepDuration dt);
    static void applyPredictionErrorToLearning(PredictionError* predictionError,
                                               std::vector<Synapse*>& synapses,
                                               float learningRate);
    
    // ===========================================
    // NEUROMODULATION STATISTICS AND ACCESSORS
    // ===========================================
    
    static void logNeuromodulationStatus(Dopamine* dopamine,
                                         Curiosity* curiosity,
                                         Novelty* novelty,
                                         PredictionError* predictionError);
};

} // namespace nlm