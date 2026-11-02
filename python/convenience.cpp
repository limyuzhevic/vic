#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <string>
#include <sstream>
#include <cstdlib>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {
namespace pybindings {

// Helper types for convenience functions
using SimulationStep = uint64_t;
using Timestamp = double;
using TimestepDuration = double;

// Default configuration presets for different use cases
inline std::shared_ptr<Config> createBeginnerConfig() {
    auto config = std::make_shared<Config>();
    // Minimal setup for learning
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config->set("connection_probability", 0.15f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
    config->set("stdp_tau", 20.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("pruning_rate", 0.00001f, ConfigSource::Default);
    return config;
}

inline std::shared_ptr<Config> createResearchConfig() {
    auto config = std::make_shared<Config>();
    // More complex setup for research
    config->set("random_seed", static_cast<int64_t>(12345), ConfigSource::Default);
    config->set("simulation_timestep", 0.0005, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
    config->set("connection_probability", 0.1f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.01f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.012f, ConfigSource::Default);
    config->set("stdp_tau", 15.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.00005f, ConfigSource::Default);
    config->set("pruning_rate", 0.000005f, ConfigSource::Default);
    return config;
}

inline std::shared_ptr<Config> createHighDensityConfig() {
    auto config = std::make_shared<Config>();
    // High-density for studying population effects
    config->set("random_seed", static_cast<int64_t>(999), ConfigSource::Default);
    config->set("simulation_timestep", 0.002, ConfigSource::Default);
    config->set("neuron_count", static_cast<int64_t>(20000), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(8), ConfigSource::Default);
    config->set("connection_probability", 0.05f, ConfigSource::Default);
    config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
    config->set("stdp_ltd_weight", 0.025f, ConfigSource::Default);
    config->set("stdp_tau", 30.0f, ConfigSource::Default);
    config->set("synaptogenesis_rate", 0.00001f, ConfigSource::Default);
    config->set("pruning_rate", 0.000001f, ConfigSource::Default);
    return config;
}

// Simplified simulation runner for common patterns
inline void runSimulationSteps(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    std::shared_ptr<AgentBrain> agent,
    size_t steps,
    double worldTimestep = 0.1,
    bool enableLearning = true,
    bool enableDevelopment = false,
    bool enableCuriosity = false
) {
    if (!brain || !agent || !world) {
        throw std::runtime_error("All brain, agent, and world must be provided");
    }
    
    brain->initialize();
    world->reset();
    agent->initialize(*world);
    
    if (enableLearning) {
        agent->enableRewardModulation(true);
    }
    if (enableDevelopment) {
        agent->enableDevelopment(true);
    }
    if (enableCuriosity) {
        agent->enableCuriosity(true);
    }
    
    for (size_t step = 0; step < steps; ++step) {
        // World update
        world->update(worldTimestep);
        
        // Get sensory input
        const auto& percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain step
        brain->step(static_cast<SimulationStep>(step));
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Check for learning
        if (enableLearning) {
            // Calculate reward based on world state
            float reward = 0.0f;
            auto& agentBody = world->getAgentBody();
            
            // Simple reward heuristic
            auto* nearestResource = world->getObjectAt(agentBody.x + 0.5f, agentBody.y + 0.5f);
            if (nearestResource && nearestResource->type == WorldObjectType::Resource) {
                reward = nearestResource->value;
            }
            
            auto* nearestHazard = world->getObjectAt(agentBody.x + 0.5f, agentBody.y + 0.5f);
            if (nearestHazard && nearestHazard->type == WorldObjectType::Hazard) {
                reward -= nearestHazard->value;
            }
            
            agent->applyRewardModulation(reward, 0.0f);  // Simple predicted reward of 0
        }
        
        // Log progress every 100 steps
        if (step % 100 == 0) {
            std::cout << "Step " << step << ": "
                     << "Firing: " << brain->getFiringNeuronCount() << ", "
                     << "Spikes: " << brain->getTotalSpikeCount() << ", "
                     << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
        }
    }
}

// Convenience visualization helper
inline std::string generateBrainSnapshot(std::shared_ptr<Brain> brain) {
    if (!brain) return "Invalid brain";
    
    std::stringstream ss;
    ss << "=== BRAIN SNAPSHOT ===" << std::endl;
    ss << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    ss << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
    ss << "E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    ss << "Average Firing Rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    ss << "Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    ss << "" << std::endl;
    ss << "Regional Statistics:" << std::endl;
    
    auto regions = brain->getRegionIds();
    for (size_t i = 0; i < regions.size(); ++i) {
        auto* region = brain->getRegion(regions[i]);
        if (region) {
            ss << "  Region " << (i + 1) << ": "
               << region->getTotalNeuronCount() << " neurons, "
               << region->getPopulationCount() << " populations" << std::endl;
        }
    }
    
    ss << "=====================" << std::endl;
    return ss.str();
}

// Quick demo: Learning agent in world
inline void runLearningDemo(
    std::shared_ptr<Config> config,
    size_t steps = 1000,
    const std::string& configName = "Default Learning Demo"
) {
    std::cout << "=== " << configName << " ===" << std::endl;
    
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Configure world
    world->configure(20, 20, 8, 8);
    world->setAgentStart(10.0f, 10.0f);
    
    // Enable learning features
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Brain initialized with " << config->getOr<int64_t>("neuron_count", 500) << " neurons" << std::endl;
    std::cout << "World: " << world->getWidth() << "x" << world->getHeight() << " with " << world->getVisionWidth() << "x" << world->getVisionHeight() << " vision" << std::endl;
    
    runSimulationSteps(brain, world, agent, steps);
    
    std::cout << "\nFinal Brain Status:" << std::endl;
    std::cout << generateBrainSnapshot(brain);
}

// Quick demo: Pattern association learning
inline void runPatternLearningDemo(
    std::shared_ptr<Config> config,
    size_t steps = 2000,
    const std::string& configName = "Pattern Association Demo"
) {
    std::cout << "=== " << configName << " ===" << std::endl;
    
    auto brain = std::make_shared<Brain>(config);
    auto world = std::make_shared<SimpleWorld>();
    auto agent = std::make_shared<AgentBrain>(brain);
    
    // Configure for pattern learning - more sensory neurons
    config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Runtime);
    config->set("region_count", static_cast<int64_t>(3), ConfigSource::Runtime);
    world->configure(30, 30, 16, 16);
    world->setAgentStart(15.0f, 15.0f);
    
    // Enable all learning features for pattern association
    agent->enableRewardModulation(true);
    agent->enableCuriosity(true);
    agent->enableDevelopment(true);
    
    std::cout << "Pattern learning brain initialized with " << config->getOr<int64_t>("neuron_count", 2000) << " neurons" << std::endl;
    std::cout << "Vision resolution: " << world->getVisionWidth() << "x" << world->getVisionHeight() << " (higher for pattern detail)" << std::endl;
    
    // Add some resources and hazards
    for (int i = 0; i < 10; ++i) {
        float x = static_cast<float>(rand() % 20 + 5);
        float y = static_cast<float>(rand() % 20 + 5);
        world->addObject(WorldObject(x, y, WorldObjectType::Resource, 10.0f, 0.3f));
    }
    
    for (int i = 0; i < 5; ++i) {
        float x = static_cast<float>(rand() % 20 + 5);
        float y = static_cast<float>(rand() % 20 + 5);
        world->addObject(WorldObject(x, y, WorldObjectType::Hazard, 5.0f, 0.3f));
    }
    
    runSimulationSteps(brain, world, agent, steps, 0.05);
    
    std::cout << "\nPattern Learning Results:" << std::endl;
    std::cout << generateBrainSnapshot(brain);
}

} // namespace pybindings
} // namespace nlm
