// BrainNeuromodulation.hpp - Neuromodulation systems management
#pragma once

#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../core/Types/Types.hpp"

namespace nlm {

class Brain;

class BrainNeuromodulation {
public:
    static void update(Brain& brain);
    static void updateNoveltySystem(Brain& brain);
    static void updateCuriositySystem(Brain& brain);
    static void updateDopamineSystem(Brain& brain);
    static void applyDopamineEffects(Brain& brain);
    static void applyCuriosityEffects(Brain& brain);
    static void applyNoveltyEffects(Brain& brain);
    static void applyPredictionErrorEffects(Brain& brain);
    static void updatePlasticityModulation(Brain& brain);
    static void updateRewardPrediction(Brain& brain);
    static void updateExplorationDrive(Brain& brain);
    static void updateNoveltyDetection(Brain& brain);
    static void updateMotivation(Brain& brain);
    static void updateAttentionModulation(Brain& brain);
    static void updateLearningRate(Brain& brain);

private:
    static float calculateDopamineLevel(Brain& brain);
    static float calculateCuriosityLevel(Brain& brain);
    static float calculateNoveltyLevel(Brain& brain);
    static float calculatePredictionError(Brain& brain);
    static void applyDopamineToNeurons(Brain& brain, float level);
    static void applyCuriosityToNeurons(Brain& brain, float level);
    static void applyNoveltyToNeurons(Brain& brain, float level);
};
