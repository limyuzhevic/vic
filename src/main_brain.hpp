#pragma once

#include <memory>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "core/SimulationClock/SimulationClock.hpp"

namespace nlm {

class BrainInitializer {
public:
    static std::shared_ptr<Brain> createBrain(const Config& config);
};

} // namespace nlm