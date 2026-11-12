// BrainNeuromodulation.hpp - Neuromodulation system access
#pragma once

#include "../core/Types/Types.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

class BrainNeuromodulation {
public:
    BrainNeuromodulation(Brain* brain);
    ~BrainNeuromodulation();
    
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    float getAverageFiringRate() const;
    
private:
    Brain* brain_;
};

} // namespace nlm
