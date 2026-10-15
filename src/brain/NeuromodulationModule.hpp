#pragma once

#include "../core/Types/Types.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"

namespace nlm {

class NeuromodulationModule {
public:
    NeuromodulationModule(std::shared_ptr<Dopamine> dopamine,
                         std::shared_ptr<Curiosity> curiosity,
                         std::shared_ptr<Novelty> novelty);
    
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    
    void update(SimulationStep currentStep, Timestamp currentTime,
                const std::vector<float>& rewardSignals,
                float noveltyLevel);
    void applyToNeurons(class NeuralProcessor* neuralProcessor);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
