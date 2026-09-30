// Neuromodulation.hpp
// Neuromodulation system interface

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"

namespace nlm {

class NeuromodulationSystem {
public:
    void initialize(std::shared_ptr<Config> config, std::shared_ptr<Brain> brain);
    void processStep(SimulationStep currentStep, Timestamp currentTime);
    
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionError();
    
private:
    void applyDopamineEffects();
    void applyNeuromodulationToPlasticity(float factor);
    
    std::shared_ptr<Config> config_;
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    float neuromodulationLevel_;
};

} // namespace nlm
