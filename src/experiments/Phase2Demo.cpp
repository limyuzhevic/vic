#include "src/experiments/Phase2Experiment.hpp"
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>

namespace nlm {

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Phase 2 Demo ===" << std::endl;
    std::cout << "Neural Learning Machine - Phase 2: Real Neural Computation" << std::endl;
    std::cout << "" << std::endl;
    
    // Initialize logging
    Logger::getInstance().setLevel(Logger::Level::Info);
    
    // Create a simple configuration with Phase 2 defaults
    auto config = std::make_shared<Config>();
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    
    // Create brain with configuration
    auto brain = std::make_shared<Brain>(config);
    
    if (!brain->initialize()) {
        std::cerr << "ERROR: Failed to initialize brain!" << std::endl;
        return 1;
    }
    
    std::cout << "Brain initialized successfully:" << std::endl;
    brain->logStatus();
    std::cout << std::endl;
    
    // Create and run Phase 2 experiment
    Phase2Experiment experiment;
    
    std::cout << "=== Running Phase 2 Experiments ===" << std::endl;
    experiment.runAll(brain);
    
    std::cout << std::endl;
    std::cout << "=== Demo Complete ===" << std::endl;
    std::cout << "The NLM Phase 2 demonstrates:" << std::endl;
    std::cout << "  • Real LIF neuron dynamics" << std::endl;
    std::cout << "  • Event-driven spike propagation" << std::endl;
    std::cout << "  • STDP and Hebbian plasticity" << std::endl;
    std::cout << "  • Learning through experience" << std::endl;
    std::cout << "" << std::endl;
    
    return 0;
}

} // namespace nlm