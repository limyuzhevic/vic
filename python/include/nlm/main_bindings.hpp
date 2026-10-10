// NLM Python Bindings - Main Module
// Main bindings file that includes all components and sets up the Python module

#pragma once

#include <pybind11/pybind11.h>
#include "exceptions.hpp"
#include "validation.hpp"
#include "config_bindings.hpp"
#include "type_bindings.hpp"
#include "sensory_bindings.hpp"
#include "action_bindings.hpp"
#include "agent_bindings.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Exception handling with custom Python exceptions
        - Runtime type checking and validation
        - Comprehensive documentation with examples
        - Performance monitoring utilities
        - Helper functions for common operations
        - Bounds checking for arrays and vectors
        - Type safety for critical parameters
        
        Quick Start:
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> brain = pynlm.Brain(config)
        >>> world = pynlm.SimpleWorld()
        >>> agent = pynlm.AgentBrain(brain)
        >>> # Your code here...
    )pbdoc";

    // Register exception translators
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // Register NLM custom exceptions
    ConfigError::register(m);
    BrainError::register(m);
    WorldError::register(m);
    AgentError::register(m);
    ValidationError::register(m);
    MemoryError::register(m);

    // Bind all components
    bind_types(m);
    bind_config(m);
    bind_sensory(m);
    bind_action(m);
    bind_agent(m);

    // Factory functions for common operations
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration)
    
    Returns:
        Default Config instance
    
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> config.loadFromFile("my_config.json")
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw ConfigError("Config cannot be null");
            }
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            return brain;
        } catch (const ConfigError& e) {
            throw;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Failed to create brain: ") + e.what());
        }
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration)
    
    Args:
        config: Configuration object for the brain
    
    Returns:
        Brain instance
    
    Raises:
        ConfigError: If config is invalid
        BrainError: If brain cannot be created
    
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        try {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(100, 100, 21, 21); // Default world size
            return world;
        } catch (const std::exception& e) {
            throw WorldError(std::string("Failed to create world: ") + e.what());
        }
    }, R"pbdoc(Create a new simple world)
    
    Returns:
        SimpleWorld instance with default configuration
    
    Example:
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(100, 100, 21, 21)
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw BrainError("Brain cannot be null");
            }
            return std::make_shared<AgentBrain>(brain);
        } catch (const BrainError& e) {
            throw;
        } catch (const std::exception& e) {
            throw AgentError(std::string("Failed to create agent brain: ") + e.what());
        }
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface)
    
    Args:
        brain: Brain instance to interface with
    
    Returns:
        AgentBrain instance
    
    Raises:
        BrainError: If brain is invalid
        AgentError: If agent brain cannot be created
    
    Example:
        >>> brain = pynlm.createBrain(config)
        >>> agent = pynlm.createAgentBrain(brain)
    )pbdoc");

    // Helper functions for common operations
    m.def("runEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                         size_t maxSteps, double timestep) -> double {
        try {
            validation::check_bounds("max steps", maxSteps, 1, 1000000);
            validation::check_bounds("timestep", timestep, 0.001, 1.0);
            
            if (!agent || !world) {
                throw ValidationError("Agent and world cannot be null");
            }
            
            agent->initialize(*world);
            double totalReward = 0.0;
            
            for (size_t step = 0; step < maxSteps; ++step) {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                if (!percept) {
                    break;
                }
                
                // Process input
                agent->processSensoryInput(*percept);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                if (!action) {
                    break;
                }
                
                // Apply action
                auto result = world->applyMotorCommand(*action, world->getSimulationTime());
                if (result) {
                    totalReward += result->reward;
                    
                    // Apply reward modulation
                    agent->applyRewardModulation(result->reward, 0.0); // Placeholder for predicted reward
                    
                    // Update development
                    agent->updateDevelopment(timestep);
                }
                
                // Update world
                world->update(timestep);
            }
            
            return totalReward;
        } catch (const ValidationError& e) {
            throw;
        } catch (const AgentError& e) {
            throw;
        } catch (const WorldError& e) {
            throw;
        } catch (const std::exception& e) {
            throw AgentError(std::string("Failed to run episode: ") + e.what());
        }
    }, py::arg("agent"), py::arg("world"), py::arg("maxSteps"), py::arg("timestep"), R"pbdoc(Run a complete episode in the world)
    
    Args:
        agent: Agent brain instance
        world: World instance
        maxSteps: Maximum number of steps to run
        timestep: Time step for simulation
    
    Returns:
        Total reward accumulated during the episode
    
    Raises:
        ValidationError: If parameters are invalid
        AgentError: If agent fails
        WorldError: If world fails
    
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> world = pynlm.createSimpleWorld()
        >>> agent = pynlm.createAgentBrain(brain)
        >>> reward = pynlm.runEpisode(agent, world, maxSteps=1000, timestep=0.1)
        >>> print(f"Episode finished with total reward: {reward}")
    )pbdoc");

    m.def("getSystemInfo", []() {
        std::map<std::string, std::string> info;
        info["version"] = "0.1.0";
        info["cpp_standard"] = "C++20";
        info["pybind11_version"] = PYBIND11_VERSION;
        info["description"] = "Neural Learning Machine Python Bindings";
        return info;
    }, R"pbdoc(Get system information about the NLM bindings)
    
    Returns:
        Dictionary with system information
    
    Example:
        >>> info = pynlm.getSystemInfo()
        >>> print(info["version"])
    )pbdoc");

    // Performance monitoring utilities (placeholders)
    m.def("getPerformanceStats", [](const std::shared_ptr<Brain>& brain) {
        // Get performance statistics from brain
        // This is a placeholder - would need brain to expose performance methods
        std::map<std::string, double> stats;
        stats["neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["memory_mb"] = 0.0; // Placeholder
        stats["cpu_time"] = 0.0; // Placeholder
        return stats;
    }, py::arg("brain"), R"pbdoc(Get performance statistics for a brain)
    
    Args:
        brain: Brain instance to get statistics for
    
    Returns:
        Dictionary of performance statistics
    
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> stats = pynlm.getPerformanceStats(brain)
        >>> print(f"Neurons: {stats['neurons']}, Synapses: {stats['synapses']}")
    )pbdoc");

    m.def("benchmarkEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                               size_t numEpisodes, double timestep) -> double {
        try {
            validation::check_bounds("num episodes", numEpisodes, 1, 1000);
            validation::check_bounds("timestep", timestep, 0.001, 1.0);
            
            if (!agent || !world) {
                throw ValidationError("Agent and world cannot be null");
            }
            
            double startTime = 0.0; // Placeholder - would use real timing
            
            for (size_t i = 0; i < numEpisodes; ++i) {
                auto percept = world->getSensoryPercept();
                if (percept) {
                    agent->processSensoryInput(*percept);
                }
                
                auto action = agent->decodeMotorCommand();
                if (action) {
                    auto result = world->applyMotorCommand(*action, world->getSimulationTime());
                    if (result) {
                        agent->applyRewardModulation(result->reward, 0.0);
                    }
                }
                
                world->update(timestep);
            }
            
            double endTime = 0.0; // Placeholder
            double elapsed = endTime - startTime;
            
            return (numEpisodes > 0) ? (elapsed / numEpisodes) : 0.0;
        } catch (const ValidationError& e) {
            throw;
        } catch (const std::exception& e) {
            throw AgentError(std::string("Failed to benchmark: ") + e.what());
        }
    }, py::arg("agent"), py::arg("world"), py::arg("numEpisodes"), py::arg("timestep"), R"pbdoc(Run benchmark episodes)
    
    Args:
        agent: Agent brain instance
        world: World instance
        numEpisodes: Number of episodes to run
        timestep: Time step for simulation
    
    Returns:
        Average time per episode in seconds
    
    Raises:
        ValidationError: If parameters are invalid
        AgentError: If benchmark fails
    
    Example:
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> world = pynlm.createSimpleWorld()
        >>> agent = pynlm.createAgentBrain(brain)
        >>> avg_time = pynlm.benchmarkEpisode(agent, world, numEpisodes=10, timestep=0.1)
        >>> print(f"Average episode time: {avg_time:.3f}s")
    )pbdoc");

    // Expose invalid ID sentinels as Python constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";

} // namespace nlm
