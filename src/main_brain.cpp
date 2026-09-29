#include "main_brain.hpp"
#include "core/Logger/Logger.hpp"
#include <stdexcept>

namespace nlm {

std::shared_ptr<Brain> BrainInitializer::createBrain(const Config& config) {
    double timestep = config.getOr<double>("simulation_timestep", 0.001);
    SimulationClock clock(timestep);
    NLM_LOG_INFO("Simulation clock initialized with timestep: " + std::to_string(timestep) + "s");
    
    // Initialize brain
    NLM_LOG_INFO("");
    NLM_LOG_INFO("Initializing NLM Brain...");
    auto brain = std::make_shared<Brain>(std::const_pointer_cast<Config>(config));
    
    if (!brain->initialize()) {
        NLM_LOG_ERROR("Failed to initialize brain!");
        throw std::runtime_error("Brain initialization failed");
    }
    
    brain->logStatus();
    return brain;
}

} // namespace nlm