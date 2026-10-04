// World helper class for enhanced Python bindings
// Provides additional functionality for world creation and simulation

#pragma once

#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <random>

namespace nlm {

class PyWorldHelper {
public:
    // Create a world with advanced configuration
    static std::shared_ptr<SimpleWorld> createWorldWithConfig(
        float width = 20.0f,
        float height = 20.0f,
        size_t vision_width = 8,
        size_t vision_height = 8,
        float max_energy = 100.0f,
        float energy_decay_rate = 0.01f) {
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, vision_width, vision_height);
        world->setMaxEnergy(max_energy);
        world->setEnergyDecayRate(energy_decay_rate);
        
        return world;
    }
    
    // Add resource to world with random positioning
    static void addRandomResource(
        std::shared_ptr<SimpleWorld> world,
        float value_min = 10.0f,
        float value_max = 50.0f,
        float radius = 0.5f) {
        
        if (!world) return;
        
        // Generate random position that's not occupied
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis_x(0.0f, world->getWidth());
        std::uniform_real_distribution<float> dis_y(0.0f, world->getHeight());
        
        float x, y;
        int attempts = 0;
        do {
            x = dis_x(gen);
            y = dis_y(gen);
            attempts++;
            if (attempts > 100) break; // Prevent infinite loop
        } while (!world->isValidPosition(x, y));
        
        std::uniform_real_distribution<float> dis_value(value_min, value_max);
        float value = dis_value(gen);
        
        world->addObject(WorldObject(x, y, WorldObjectType::Resource, value, radius));
    }
    
    // Run world simulation with periodic logging
    static std::vector<std::string> runWorldSimulation(
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t num_steps,
        double timestep = 0.1,
        bool log_detailed = false) {
        
        if (!world || !agent) {
            throw std::runtime_error("World or agent is null");
        }
        
        std::vector<std::string> logs;
        
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(timestep);
            
            if (log_detailed) {
                std::stringstream ss;
                ss << "Step " << step << ": "
                   << "Energy: " << world->getAgentBody().energy
                   << ", Time: " << world->getSimulationTime();
                logs.push_back(ss.str());
            }
        }
        
        return logs;
    }
};

} // namespace nlm
