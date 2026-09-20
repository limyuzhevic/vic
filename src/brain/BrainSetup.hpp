// BrainSetup.hpp - handles brain initialization and region setup
#pragma once

#include "Brain.hpp"
#include "../core/Types/Types.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

class BrainSetup {
public:
    static bool initializeBrain(Brain::Impl* pImpl);
    static bool createRegions(Brain::Impl* pImpl, size_t neuronCount, size_t regionCount, float connectionProbability);
    static void collectSensoryMotorNeurons(Brain::Impl* pImpl);
};

} // namespace nlm