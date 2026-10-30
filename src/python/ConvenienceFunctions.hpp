#pragma once

#include "core/Config/Config.hpp"
#include "agent/AgentBrain.hpp"
#include "world/SimpleWorld.hpp"

namespace nlm {
    // Common configuration setup helper
    inline void setupDefaultBrainConfig(std::shared_ptr<Config> config, 
                                        size_t neuronCount = 100,
                                        size_t regionCount = 1,
                                        double connectionProbability = 0.1) {
        config->set("neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(regionCount), ConfigSource::Default);
        config->set("connection_probability", connectionProbability, ConfigSource::Default);
    }
    
    // World configuration helper
    inline void configureWorld(nlm::SimpleWorld& world,
                              size_t width = 10,
                              size_t height = 10,
                              size_t visionWidth = 8,
                              size_t visionHeight = 8) {
        world.configure(width, height, visionWidth, visionHeight);
    }
    
    // Agent setup helper
    inline void setupAgentBrain(std::shared_ptr<AgentBrain> agent,
                               std::shared_ptr<SimpleWorld> world) {
        agent->initialize(world);
    }
    
    // Batch stepping helper
    inline void stepBatch(std::shared_ptr<Brain> brain, size_t startStep, size_t endStep) {
        for (nlm::SimulationStep step = startStep; step < endStep; ++step) {
            brain->step(step);
        }
    }
    
    // Performance metrics helper
    inline std::map<std::string, double> getPerformanceMetrics(std::shared_ptr<Brain> brain) {
        std::map<std::string, double> metrics;
        metrics["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        metrics["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        metrics["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        metrics["avg_firing_rate"] = brain->getAverageFiringRate();
        metrics["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        return metrics;
    }
    
    // Configuration validation helper
    inline std::vector<std::string> validateConfig(std::shared_ptr<Config> config) {
        auto keys = config->getKeys();
        std::vector<std::string> errors;
        
        for (const auto& key : keys) {
            auto value = config->get<std::string>(key);
            if (!value.has_value()) {
                errors.push_back("Key \"" + key + "\" has invalid value type");
            }
        }
        
        return errors;
    }
    
    // State snapshot helper
    inline std::map<std::string, std::variant<int, double, std::string>> snapshotState(std::shared_ptr<Brain> brain) {
        std::map<std::string, std::variant<int, double, std::string>> snapshot;
        snapshot["neuron_count"] = static_cast<int>(brain->getTotalNeuronCount());
        snapshot["synapse_count"] = static_cast<int>(brain->getTotalSynapseCount());
        snapshot["firing_count"] = static_cast<int>(brain->getFiringNeuronCount());
        snapshot["avg_rate"] = brain->getAverageFiringRate();
        snapshot["total_spikes"] = static_cast<int>(brain->getTotalSpikeCount());
        snapshot["stage"] = static_cast<int>(brain->getDevelopmentalStage());
        return snapshot;
    }
}

// Global functions for Python bindings compatibility
inline void setupDefaultBrainConfig(std::shared_ptr<nlm::Config> config, 
                                    size_t neuronCount = 100,
                                    size_t regionCount = 1,
                                    double connectionProbability = 0.1) {
    nlm::setupDefaultBrainConfig(config, neuronCount, regionCount, connectionProbability);
}

inline void configureWorld(nlm::SimpleWorld& world,
                          size_t width = 10,
                          size_t height = 10,
                          size_t visionWidth = 8,
                          size_t visionHeight = 8) {
    nlm::configureWorld(world, width, height, visionWidth, visionHeight);
}

inline void setupAgentBrain(std::shared_ptr<nlm::AgentBrain> agent,
                           std::shared_ptr<nlm::SimpleWorld> world) {
    nlm::setupAgentBrain(agent, world);
}