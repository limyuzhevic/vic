// Brain helper class for enhanced Python bindings
// Provides additional functionality for brain creation and simulation

#pragma once

#include "../src/brain/Brain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/agent/AgentBrain.hpp"
#include <string>
#include <vector>
#include <memory>
#include <random>
#include <chrono>

namespace nlm {

class PyBrainHelper {
public:
    // Create a brain with default configuration but custom parameters
    static std::shared_ptr<Brain> createBrainWithParams(
        size_t neuron_count,
        size_t region_count = 1,
        double connection_probability = 0.1,
        const std::string& config_file = "") {
        
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Runtime);
        config->set("brain.region_count", static_cast<int64_t>(region_count), ConfigSource::Runtime);
        config->set("brain.connection_probability", connection_probability, ConfigSource::Runtime);
        
        if (!config_file.empty()) {
            config->loadFromFile(config_file);
        }
        
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw std::runtime_error("Failed to initialize brain");
        }
        
        return brain;
    }
    
    // Run a simulation loop with customizable parameters
    static std::vector<double> runSimulation(
        std::shared_ptr<Brain> brain,
        size_t num_steps,
        double timestep = 0.1,
        bool log_stats = false) {
        
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::vector<double> firing_rates;
        firing_rates.reserve(num_steps);
        
        for (size_t step = 0; step < num_steps; ++step) {
            brain->step(static_cast<SimulationStep>(step), static_cast<Timestamp>(step * timestep));
            
            if (log_stats) {
                firing_rates.push_back(brain->getAverageFiringRate());
            }
        }
        
        return firing_rates;
    }
    
    // Run agent simulation with world
    static std::vector<double> runAgentSimulation(
        std::shared_ptr<Brain> brain,
        std::shared_ptr<SimpleWorld> world,
        std::shared_ptr<AgentBrain> agent,
        size_t num_steps,
        double timestep = 0.1) {
        
        if (!brain || !world || !agent) {
            throw std::runtime_error("Brain, world, or agent is null");
        }
        
        std::vector<double> stats;
        
        for (size_t step = 0; step < num_steps; ++step) {
            // Update world
            world->update(timestep);
            
            // Get sensory input
            const auto& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Run brain step
            brain->step(static_cast<SimulationStep>(step), world->getSimulationTime());
            
            // Decode action
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Log stats every 100 steps
            if (step % 100 == 0) {
                stats.push_back(brain->getAverageFiringRate());
            }
        }
        
        return stats;
    }
    
    // Save brain with timestamp
    static bool saveBrainWithTimestamp(
        std::shared_ptr<Brain> brain,
        const std::string& base_path,
        bool include_stats = false) {
        
        if (!brain) return false;
        
        // Add timestamp to filename
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()) % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
        ss << "_" << ms.count() << "_";
        
        std::string filename = base_path + "_" + ss.str() + "brain.bin";
        return brain->save(filename);
    }
    
    // Create multiple brain regions with specific characteristics
    static std::vector<RegionId> createRegions(
        std::shared_ptr<Brain> brain,
        const std::vector<std::string>& region_names,
        const std::vector<double>& connection_probabilities = {}) {
        
        if (!brain) {
            throw std::runtime_error("Brain is null");
        }
        
        std::vector<RegionId> region_ids;
        size_t num_regions = std::min(region_names.size(), connection_probabilities.size());
        
        for (size_t i = 0; i < num_regions; ++i) {
            auto region_id = brain->addRegion(region_names[i]);
            region_ids.push_back(region_id);
            
            // In a full implementation, you would configure region-specific properties
        }
        
        return region_ids;
    }
};

} // namespace nlm
