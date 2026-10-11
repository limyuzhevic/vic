#include "HelperFunctions.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Config/DefaultConfig.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include <sstream>
#include <cmath>

namespace nlm {

std::string PythonHelperFunctions::runSimulation(std::shared_ptr<Brain> brain, size_t steps, bool verbose) {
    std::stringstream result;
    for (size_t step = 0; step < steps; ++step) {
        brain->step(step, step * 0.001);
        
        if (verbose && (step % 100 == 0 || step == steps - 1)) {
            result << "Step " << step << ": "
                   << brain->getFiringNeuronCount() << " firing neurons, "
                   << brain->getTotalSpikeCount() << " total spikes" << std::endl;
        }
    }
    return result.str();
}

std::shared_ptr<Config> PythonHelperFunctions::createPhase6Config() {
    auto config = std::make_shared<Config>();
    DefaultConfig::applyPhase6Defaults(*config);
    return config;
}

std::shared_ptr<AgentBrain> PythonHelperFunctions::createLearningAgent(
    std::shared_ptr<Brain> brain,
    std::shared_ptr<SimpleWorld> world,
    bool enableRewardModulation,
    bool enableStructuralPlasticity,
    bool enableDevelopment,
    bool enableCuriosity) {
    
    auto agent = std::make_shared<AgentBrain>(brain);
    agent->initialize(*world);
    
    if (enableRewardModulation) agent->enableRewardModulation(true);
    if (enableStructuralPlasticity) agent->enableStructuralPlasticity(true);
    if (enableDevelopment) agent->enableDevelopment(true);
    if (enableCuriosity) agent->enableCuriosity(true);
    
    return agent;
}

std::vector<std::string> PythonHelperFunctions::simulateEpisode(
    std::shared_ptr<AgentBrain> agent,
    std::shared_ptr<SimpleWorld> world,
    size_t maxSteps,
    bool recordMetrics) {
    std::vector<std::string> metrics;
    
    for (size_t step = 0; step < maxSteps; ++step) {
        // Update world
        world->update(0.1);
        
        // Get sensory input
        auto percept = world->getSensoryPercept();
        
        // Process in brain
        agent->processSensoryInput(percept);
        auto brain = agent->getBrain();
        brain->step(step);
        
        // Get action
        auto action = agent->decodeMotorCommand();
        
        // Apply action in world
        world->applyMotorCommand(action, world->getSimulationTime());
        
        // Apply reward if recording metrics
        if (recordMetrics && step % 10 == 0) {
            auto internal = world->getSensoryPercept().getInternal();
            float reward = internal ? internal->getValue() : 0.0f;
            agent->applyRewardModulation(reward, 0.0);
            
            std::stringstream metric;
            metric << "Step " << step 
                   << ": Action=" << static_cast<int>(action.getType())
                   << ", Reward=" << reward
                   << ", Firing=" << brain->getFiringNeuronCount()
                   << ", Stage=" << static_cast<int>(agent->getDevelopmentalStage());
            metrics.push_back(metric.str());
        }
    }
    
    return metrics;
}

} // namespace nlm
