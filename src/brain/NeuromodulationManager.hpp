#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class NeuromodulationManager {
public:
    NeuromodulationManager(Brain* brain);
    ~NeuromodulationManager();
    
    bool initialize();
    void step(double timestep);
    void reset();
    
    // Neuromodulator access
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    
    // Neuromodulation effects
    void applyDopamineEffects(double timestep);
    void applyCuriosityEffects(double timestep);
    void applyNoveltyEffects(double timestep);
    void applyPredictionErrorEffects(double timestep);
    
private:
    Brain* brain_;
    std::unique_ptr<Dopamine> dopamine;
    std::unique_ptr<Curiosity> curiosity;
    std::unique_ptr<PredictionError> predictionError;
    std::unique_ptr<Novelty> novelty;
};

} // namespace nlm
