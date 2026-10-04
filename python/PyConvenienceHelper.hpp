// Convenience functions for enhanced NLM Python bindings
// Provides high-level functions for common use cases

#pragma once

#include "../src/core/Config/Config.hpp"
#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <algorithm>

namespace nlm {

namespace convenience {
    // Run a complete agent simulation with all systems
    static std::unordered_map<std::string, double> runCompleteSimulation(
        const std::string& world_size = "20x20",
        size_t num_steps = 1000,
        const std::string& config_file = "") {
        
        // Parse world size
        size_t width = 20, height = 20;
        if (world_size != "20x20") {
            // Simple parsing - in real implementation use more robust parsing
            size_t sep_pos = world_size.find('x');
            if (sep_pos != std::string::npos) {
                width = std::stoul(world_size.substr(0, sep_pos));
                height = std::stoul(world_size.substr(sep_pos + 1));
            }
        }
        
        // Create brain
        auto config = std::make_shared<Config>();
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height), 8, 8);
        world->reset();
        
        // Create agent
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        agent->enableAllLearning();
        
        // Run simulation
        for (size_t step = 0; step < num_steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step, world->getSimulationTime());
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        // Return summary statistics
        std::unordered_map<std::string, double> results;
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["firing_rate"] = brain->getAverageFiringRate();
        results["active_neurons"] = brain->getActiveNeuronCount();
        results["simulation_time"] = static_cast<double>(num_steps * 0.1);
        
        return results;
    }
    
    // Create a demo configuration
    static std::shared_ptr<Config> createDemoConfig() {
        auto config = std::make_shared<Config>();
        
        // Basic brain configuration
        config->set("brain.neuron_count", 500, ConfigSource::Default);
        config->set("brain.region_count", 2, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0, ConfigSource::Default);
        config->set("brain.v_rest", -70.0, ConfigSource::Default);
        config->set("brain.v_reset", -75.0, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0, ConfigSource::Default);
        config->set("brain.tau_ref", 2.0, ConfigSource::Default);
        
        // Plasticity configuration
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        // Neuromodulation configuration
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        return config;
    }
    
    // Create a simple world with resources
    static std::shared_ptr<SimpleWorld> createResourceWorld(
        float width = 30.0f,
        float height = 30.0f,
        int num_resources = 10) {
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 10, 10);
        world->reset();
        world->setAgentStart(width/2, height/2);
        
        // Add resources (simplified - in real implementation use helper from PyWorldHelper)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis_x(0.0f, width);
        std::uniform_real_distribution<float> dis_y(0.0f, height);
        
        for (int i = 0; i < num_resources; ++i) {
            float x, y;
            int attempts = 0;
            do {
                x = dis_x(gen);
                y = dis_y(gen);
                attempts++;
                if (attempts > 100) break;
            } while (!world->isValidPosition(x, y));
            
            std::uniform_real_distribution<float> dis_value(10.0f, 100.0f);
            float value = dis_value(gen);
            
            world->addObject(WorldObject(x, y, WorldObjectType::Resource, value, 0.8f));
        }
        
        return world;
    }
}

} // namespace nlm
