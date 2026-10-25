#ifndef NEUROMODULATION_SYSTEM_H
#define NEUROMODULATION_SYSTEM_H

#include "neuromodulation/Neuromodulator.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "brain/NeuralRegion.hpp"
#include "brain/Neuron.hpp"

namespace nlm {

class NeuromodulationSystem {
public:
    virtual ~NeuromodulationSystem() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(TimestepDuration timestep) = 0;
    
    virtual Dopamine* getDopamine() = 0;
    virtual Curiosity* getCuriosity() = 0;
    virtual Novelty* getNovelty() = 0;
    virtual PredictionError* getPredictionError() = 0;
    
    virtual void applyDopamineToNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                       float dopamineLevel) = 0;
    virtual void applyNoveltyToLearning(float noveltyLevel) = 0;
    virtual void applyCuriosityToExploration() = 0;
    virtual void applyPredictionErrorToPlasticity(float predictionError) = 0;
    
    virtual float getDopamineLevel() const = 0;
    virtual float getNoveltyLevel() const = 0;
    virtual float getCuriosityLevel() const = 0;
    virtual float getPredictionErrorLevel() const = 0;
    
protected:
    Brain* brain_ = nullptr;
};

} // namespace nlm

#endif // NEUROMODULATION_SYSTEM_H