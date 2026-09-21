#include "CommandSystem.hpp"
#include "Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"

namespace nlm {

// Helper function to get simulation step count
SimulationStep getMaxStepCount() {
    auto config = std::make_shared<Config>();
    return config->getOr<size_t>("max_simulation_steps", 10000);
}

} // namespace nlm


