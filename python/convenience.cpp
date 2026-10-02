// Python convenience functions for NLM

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <stringstream>
#include <iostream>
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// =============================================================================
// CONVENIENCE FUNCTIONS
// =============================================================================

// Quick start functions for common scenarios
inline std::shared_ptr<Brain> createBrain() {
    auto config = std::make_shared<Config>();
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

inline std::shared_ptr<Brain> createBrain(const std::string& configFile) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(configFile);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

inline std::shared_ptr<Brain> createBrain(int neuronCount, float connectionDensity = 0.1f) {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", neuronCount);
    config->set("brain.synapse_density", connectionDensity);
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

// Agent simulation convenience
inline void runAgentSimulation(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<SimpleWorld>& world,
    int numSteps,
    bool enableLearning = true
) {
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    if (enableLearning) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
    }
    
    for (int step = 0; step < numSteps; ++step) {
        world->update(0.1);
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        if (enableLearning) {
            float reward = 0.0f;
            if (auto& internal = percept->getInternal(); !internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, 0.0);
            agent->updateDevelopment(0.1);
        }
    }
}

// Enhanced configuration helpers
inline std::shared_ptr<Config> createConfigFromFile(const std::string& filename) {
    auto config = std::make_shared<Config>();
    config->loadFromFile(filename);
    return config;
}

inline std::shared_ptr<Config> createConfigFromArgs(int argc, char** argv) {
    auto config = std::make_shared<Config>();
    config->loadFromArgs(argc, argv);
    return config;
}

inline std::shared_ptr<Config> createOptimizedConfig(bool fastSimulation = false) {
    auto config = std::make_shared<Config>();
    
    if (fastSimulation) {
        // Optimized for speed
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.dopamine.scale", 1.0);
    } else {
        // Balanced performance
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
    }
    
    return config;
}

// Enhanced sensory percept helpers
inline std::shared_ptr<SensoryPercept> createVisionPercept(
    int width = 16,
    int height = 16,
    const std::vector<float>& data = std::vector<float>()
) {
    auto percept = std::make_shared<SensoryPercept>();
    if (!data.empty()) {
        // Fill vision data (simplified)
        percept->setVision(data);
    }
    return percept;
}

inline std::shared_ptr<SensoryPercept> createTouchPercept(
    const std::vector<float>& touchData = std::vector<float>()
) {
    auto percept = std::make_shared<SensoryPercept>();
    if (!touchData.empty()) {
        percept->setTouch(touchData);
    }
    return percept;
}

// Simulation utilities
inline double runTimedSimulation(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<SimpleWorld>& world,
    double simulationTime,
    double timestep = 0.1,
    bool enableLearning = true
) {
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    if (enableLearning) {
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
    }
    
    int steps = static_cast<int>(simulationTime / timestep);
    double actualTime = 0.0;
    
    for (int step = 0; step < steps; ++step) {
        world->update(timestep);
        actualTime += timestep;
        
        auto percept = world->getSensoryPercept();
        agent->processSensoryInput(percept);
        brain->step(step);
        
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, actualTime);
        
        if (enableLearning) {
            float reward = 0.0f;
            if (auto& internal = percept->getInternal(); !internal.empty()) {
                reward = internal[0];
            }
            agent->applyRewardModulation(reward, 0.0);
            agent->updateDevelopment(timestep);
        }
    }
    
    return actualTime;
}

// Diagnostic and debugging helpers
inline std::string getBrainSummary(const std::shared_ptr<Brain>& brain) {
    std::stringstream ss;
    ss << "Brain Summary:" << std::endl;
    ss << "  Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
    ss << "  Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
    ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
    ss << "  Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
    ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
    ss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
    return ss.str();
}

inline std::string getAgentSummary(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<SimpleWorld>& world,
    const std::shared_ptr<AgentBrain>& agent
) {
    std::stringstream ss;
    ss << "Agent Summary:" << std::endl;
    ss << "  " << getBrainSummary(brain);
    ss << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
    ss << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
    ss << "  Neuromodulation: " << agent->getNeuromodulationLevel() << std::endl;
    ss << "  Prediction Error: " << agent->getPredictionError() << std::endl;
    ss << "  World Time: " << world->getSimulationTime() << std::endl;
    ss << "  World Size: " << world->getWidth() << "x" << world->getHeight() << std::endl;
    return ss.str();
}

inline std::vector<std::string> getAllConfigKeys(const std::shared_ptr<Config>& config) {
    return config->getKeys();
}

inline bool hasConfigKey(const std::shared_ptr<Config>& config, const std::string& key) {
    return config->has(key);
}

inline float getFloatConfig(const std::shared_ptr<Config>& config, const std::string& key, float defaultValue = 0.0f) {
    if (config->has(key)) {
        // Note: Config.get() would need to be implemented for float values
        return defaultValue;
    }
    return defaultValue;
}

} // namespace nlm