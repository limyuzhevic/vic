# This file contains comprehensive improvements to the NLM Python bindings (pynlm)
# to make them more user-friendly, Pythonic, and feature-complete based on the documentation requirements.

// This is a supplemental bindings file that can be merged with the existing bindings.cpp
// to enhance the existing Python bindings while maintaining backward compatibility.

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

using namespace pybind11;

namespace nlm_improvements {

/**
 * Enhanced convenience functions for easier Python usage
 * These match the examples in the documentation
 */

// Helper for common configuration patterns
inline std::shared_ptr<Config> create_brain_config() {
    auto config = std::make_shared<Config>();
    config->set("brain.neuron_count", 1000);
    config->set("brain.synapse_density", 0.1f);
    config->set("brain.connection_probability", 0.05f);
    return config;
}

// Enhanced brain creation with validation
inline std::shared_ptr<Brain> create_brain_simplified(const std::shared_ptr<Config>& config = nullptr) {
    auto effective_config = config ? config : create_brain_config();
    
    // Validate essential parameters
    if (!effective_config->has("brain.neuron_count")) {
        throw std::runtime_error("Brain configuration missing neuron_count");
    }
    
    return std::make_shared<Brain>(effective_config);
}

// Enhanced world creation with default parameters
inline std::shared_ptr<SimpleWorld> create_enhanced_world(
    size_t width = 20,
    size_t height = 20,
    size_t visionWidth = 8,
    size_t visionHeight = 8
) {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width, height, visionWidth, visionHeight);
    world->setAgentStart(width / 2.0, height / 2.0);
    return world;
}

// Enhanced agent creation with validation
inline std::shared_ptr<AgentBrain> create_enhanced_agent(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<SimpleWorld>& world = nullptr
) {
    auto agent = std::make_shared<AgentBrain>(brain);
    if (world) {
        agent->initialize(world);
    }
    return agent;
}

// Convenience method for common simulation loop
inline void run_simulation_step(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<SimpleWorld>& world,
    const std::shared_ptr<AgentBrain>& agent,
    size_t step
) {
    if (!brain || !world || !agent) {
        throw std::runtime_error("All brain, world, and agent objects must be provided");
    }
    
    // Update world
    world->update(0.1);
    
    // Get sensory input
    auto percept = world->getSensoryPercept();
    
    // Process sensory input
    agent->processSensoryInput(percept);
    
    // Run brain step
    brain->step(step);
    
    // Get and apply action
    auto action = agent->decodeMotorCommand();
    world->applyMotorCommand(action, world->getSimulationTime());
}

// Enhanced statistics collection
inline std::unordered_map<std::string, double> get_brain_statistics(const std::shared_ptr<Brain>& brain) {
    std::unordered_map<std::string, double> stats;
    
    if (brain) {
        stats["neurons_firing"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["development_stage"] = static_cast<double>(static_cast<int>(brain->getDevelopmentalStage()));
        stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain->getActiveNeuronCount());
    }
    
    return stats;
}

// Enhanced agent statistics collection
inline std::unordered_map<std::string, double> get_agent_statistics(const std::shared_ptr<AgentBrain>& agent) {
    std::unordered_map<std::string, double> stats;
    
    if (agent) {
        stats["curiosity_level"] = agent->getCuriosityLevel();
        stats["novelty_level"] = agent->getNoveltyLevel();
        stats["prediction_error"] = agent->getPredictionError();
        stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
        stats["development_stage"] = static_cast<double>(static_cast<int>(agent->getDevelopmentalStage()));
        stats["reward_modulation_enabled"] = agent->isRewardModulationEnabled() ? 1.0 : 0.0;
        stats["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled() ? 1.0 : 0.0;
        stats["development_enabled"] = agent->isDevelopmentEnabled() ? 1.0 : 0.0;
        stats["curiosity_enabled"] = agent->isCuriosityEnabled() ? 1.0 : 0.0;
    }
    
    return stats;
}

// Enhanced world statistics collection
inline std::unordered_map<std::string, double> get_world_statistics(const std::shared_ptr<SimpleWorld>& world) {
    std::unordered_map<std::string, double> stats;
    
    if (world) {
        stats["width"] = static_cast<double>(world->getWidth());
        stats["height"] = static_cast<double>(world->getHeight());
        stats["simulation_time"] = world->getSimulationTime();
        stats["max_energy"] = world->getMaxEnergy();
        stats["is_valid_agent_position"] = world->isValidPosition(world->getAgentBody().x, world->getAgentBody().y) ? 1.0 : 0.0;
    }
    
    return stats;
}

// Enhanced action creation with better parameter handling
inline std::unique_ptr<Action> create_action_enhanced(
    ActionType type,
    std::vector<float> parameters = std::vector<float>()
) {
    auto action = std::make_unique<Action>(type);
    if (!parameters.empty()) {
        action->setParameters(parameters);
    }
    return action;
}

// Enhanced action utilities
inline std::string get_action_name(ActionType type) {
    // Implementation would map ActionType to string names
    return "Action_" + std::to_string(static_cast<int>(type));
}

// Batch processing utilities
inline std::vector<std::unordered_map<std::string, double>> run_batch_simulation(
    const std::shared_ptr<Brain>& brain,
    const std::shared_ptr<SimpleWorld>& world,
    const std::shared_ptr<AgentBrain>& agent,
    size_t num_steps,
    size_t log_interval = 100
) {
    std::vector<std::unordered_map<std::string, double>> batch_stats;
    
    for (size_t step = 0; step < num_steps; ++step) {
        run_simulation_step(brain, world, agent, step);
        
        if (step % log_interval == 0) {
            auto stats = std::unordered_map<std::string, double>{
                {"step", static_cast<double>(step)},
                {"brain_stats", get_brain_statistics(brain)},
                {"agent_stats", get_agent_statistics(agent)},
                {"world_stats", get_world_statistics(world)}
            };
            batch_stats.push_back(stats);
        }
    }
    
    return batch_stats;
}

// Configuration utilities
inline std::unordered_map<std::string, std::string> get_config_summary(const std::shared_ptr<Config>& config) {
    std::unordered_map<std::string, std::string> summary;
    if (config) {
        summary["keys_count"] = std::to_string(config->getKeys().size());
        summary["raw_summary"] = config->summary();
    }
    return summary;
}

/**
 * Enhanced bindings module that supplements the existing bindings.cpp
 */
PYBIND11_MODULE(pynlm_improvements, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) - Enhanced Python Bindings
        
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides additional convenience functions and improved API based on common usage patterns.
        
        Key improvements over the basic bindings:
        - Simplified factory functions (createBrain, createWorld, createAgent)
        - Enhanced convenience methods for common operations
        - Better error handling and validation
        - Comprehensive statistics collection
        - Batch processing utilities
        - Configuration helpers
        - Action creation utilities
    )pbdoc";

    // Register exception types
    register_exception<std::runtime_error>(m, "NLMError");
    register_exception<std::invalid_argument>(m, "InvalidArgumentError");

    // Add enhanced factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return create_brain_simplified(config);
    }, arg("config") = nullptr, "Create a brain with optional configuration");

    m.def("createSimpleWorld", [](size_t width, size_t height,
                                 size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        return create_enhanced_world(width, height, visionWidth, visionHeight);
    }, arg("width") = 20, arg("height") = 20,
       arg("visionWidth") = 8, arg("visionHeight") = 8,
       "Create a simple world with default parameters");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, arg("brain"), "Create an agent brain interface");

    // Add enhanced convenience methods
    m.def("run_simulation_step", &run_simulation_step,
          "Run a single step of the simulation with brain, world, and agent");

    m.def("get_brain_statistics", &get_brain_statistics,
          "Get comprehensive statistics from a brain");

    m.def("get_agent_statistics", &get_agent_statistics,
          "Get comprehensive statistics from an agent brain");

    m.def("get_world_statistics", &get_world_statistics,
          "Get comprehensive statistics from a world");

    m.def("create_action", &create_action_enhanced,
          "Create an action with type and optional parameters");

    m.def("get_action_name", &get_action_name,
          "Get human-readable name for an action type");

    m.def("run_batch_simulation", &run_batch_simulation,
          "Run a batch simulation and collect statistics at intervals");

    m.def("get_config_summary", &get_config_summary,
          "Get a summary of configuration settings");

    // Add convenience classes and methods
    m.attr("DEFAULT_WORLD_SIZE") = 20;
    m.attr("DEFAULT_VISION_SIZE") = 8;
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
}

} // namespace nlm_improvements