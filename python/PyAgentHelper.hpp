// Agent helper class for enhanced Python bindings
// Provides additional functionality for agent creation and learning

#pragma once

#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

namespace nlm {

class PyAgentHelper {
public:
    // Create an agent with all learning enabled
    static std::shared_ptr<AgentBrain> createLearningAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        bool enable_reward_modulation = true,
        bool enable_structural_plasticity = true,
        bool enable_development = true,
        bool enable_curiosity = true) {
        
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        if (enable_reward_modulation) agent->enableRewardModulation(true);
        if (enable_structural_plasticity) agent->enableStructuralPlasticity(true);
        if (enable_development) agent->enableDevelopment(true);
        if (enable_curiosity) agent->enableCuriosity(true);
        
        return agent;
    }
    
    // Create agent with custom neuromodulation levels
    static std::shared_ptr<AgentBrain> createCuriosityAgent(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        float curiosity_threshold = 0.5f,
        float novelty_threshold = 0.3f) {
        
        auto agent = createLearningAgent(brain, world, true, true, true, true);
        
        // In a full implementation, you would set these thresholds
        return agent;
    }
    
    // Run learning episode
    static std::vector<std::pair<double, double>> runLearningEpisode(
        std::shared_ptr<AgentBrain> agent,
        std::shared_ptr<SimpleWorld> world,
        size_t num_steps,
        double timestep = 0.1,
        bool track_performance = true) {
        
        if (!agent || !world) {
            throw std::runtime_error("Agent or world is null");
        }
        
        std::vector<std::pair<double, double>> metrics;
        
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(timestep);
            
            const auto& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto brain = agent->getBrain();
            brain->step(static_cast<SimulationStep>(step), world->getSimulationTime());
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (track_performance && step % 50 == 0) {
                double curiosity = agent->getCuriosityLevel();
                double novelty = agent->getNoveltyLevel();
                metrics.push_back(std::make_pair(curiosity, novelty));
            }
        }
        
        return metrics;
    }
};

} // namespace nlm
