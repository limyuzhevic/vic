// Brain Initialization Header

#pragma once

#include "Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include <memory>

namespace nlm {

class BrainInitialization {
public:
    static void initialize(Brain::Impl& impl, std::shared_ptr<Config> config);
    static void createRegions(Brain::Impl& impl, size_t neuronCount, size_t regionCount, float connectionProbability);
    static void initializeIntegratedSystems(Brain::Impl& impl, size_t neuronCount, std::shared_ptr<Config> config);
    static void registerSpikeHandlers(Brain::Impl& impl);
};

} // namespace nlm
