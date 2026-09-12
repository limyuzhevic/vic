#pragma once

#include "Dopamine.hpp"
#include "Acetylcholine.hpp"
#include "Noradrenaline.hpp"
#include "Serotonin.hpp"
#include "Curiosity.hpp"
#include "Novelty.hpp"
#include "PredictionError.hpp"

namespace nlm {

class NeuromodulationIntegration {
public:
    NeuromodulationIntegration();
    ~NeuromodulationIntegration();
    
    void initialize(Brain* brain);
    void update(float dt);
    void applyNeuromodulation(Brain* brain);
    
private:
    Brain* brain_;
};

} // namespace nlm
