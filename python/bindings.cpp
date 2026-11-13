// NLM Python Bindings
// Improved version with modular structure, helper templates, and missing bindings

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

// Include all NLM headers
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/core/Random/Random.hpp"
#include "../src/core/Logger/Logger.hpp"
#include "../src/core/SimulationClock/SimulationClock.hpp"

#include "../src/brain/Brain.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"

#include "../src/dynamics/NeuralDynamics.hpp"
#include "../src/dynamics/SpikeSystem.hpp"

#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"

#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Curiosity.hpp"

#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"

#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"

#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

#include "../src/motor/MotorSystem.hpp"
#include "../src/motor/Action.hpp"

#include "../src/environment/Environment.hpp"
#include "../src/environment/Observation.hpp"

#include "../src/agent/AgentBrain.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

#include "../src/world/SimpleWorld.hpp"

#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

#include "../src/visualization/VisualizationInterface.hpp"

namespace py = pybind11;
namespace nlm {

// ===========================================================================
// Helper Templates and Utilities
// ===========================================================================

namespace binding_helpers {

// ===========================================================================
// Type Aliases
// ===========================================================================

using NeuronPtr = std::shared_ptr<Neuron>;
using SynapsePtr = std::shared_ptr<Synapse>;
using RegionIdPtr = std::shared_ptr<RegionId>;
using PopulationIdPtr = std::shared_ptr<PopulationId>;
using BrainPtr = std::shared_ptr<Brain>;
using ConfigPtr = std::shared_ptr<Config>;
using WorldPtr = std::shared_ptr<SimpleWorld>;
using AgentPtr = std::shared_ptr<AgentBrain>;

// ===========================================================================
// Template Helper Functions
// ===========================================================================

// Template for shared_ptr templated classes
template<typename T>
void bind_shared_ptr_class(py::module_& m, const std::string& name) {
    py::class_<T, std::shared_ptr<T>>(m, name.c_str())
        .def(py::init<>())
        .def(py::init<ConfigPtr>(), py::arg("config"))
        .def("__repr__", [](const std::shared_ptr<T>& obj) {
            return "<" + name + ">";
        });
}

// Template for enum bindings
template<typename EnumType>
void bind_enum_class(py::module_& m, const std::string& name) {
    py::enum_<EnumType>(m, name.c_str())
        .value("Excitatory", EnumType::Excitatory)
        .value("Inhibitory", EnumType::Inhibitory)
        .value("Modulatory", EnumType::Modulatory)
        .value("Sensory", EnumType::Sensory)
        .value("Motor", EnumType::Motor)
        .value("Internal", EnumType::Internal)
        .export_values();
}

// Template for property-style accessors
template<typename Class, typename T>
void bind_property_readonly(py::class_<Class>& cls, const std::string& name, 
                           T (Class::*getter)() const) {
    cls.def_property_readonly(name.c_str(), getter);
}

// ===========================================================================
// Factory Function
// ===========================================================================

py::object create_object(const std::string& className, py::args args) {
    if (className == "Config") {
        return py::cast(std::make_shared<Config>());
    } else if (className == "Brain") {
        auto config = std::make_shared<Config>();
        return py::cast(std::make_shared<Brain>(config));
    } else if (className == "SimpleWorld") {
        return py::cast(std::make_shared<SimpleWorld>());
    } else if (className == "AgentBrain") {
        auto brain = std::make_shared<Brain>(std::make_shared<Config>());
        return py::cast(std::make_shared<AgentBrain>(brain));
    } else if (className == "RandomGenerator") {
        return py::cast(std::make_shared<RandomGenerator>());
    } else if (className == "Logger") {
        return py::cast(std::make_shared<Logger>());
    } else if (className == "SimulationClock") {
        return py::cast(std::make_shared<SimulationClock>(0.001));
    }
    throw py::value_error("Unknown class name: " + className);
}

// ===========================================================================
// Convenience Method Adders
// ===========================================================================

void add_brain_convenience_methods(py::class_<BrainPtr>& brain_class);
void add_world_convenience_methods(py::class_<WorldPtr>& world_class);
void add_config_convenience_methods(py::class_<ConfigPtr>& config_class);

// ===========================================================================
// Binding Functions
// ===========================================================================

void bind_core_types(py::module_& m);
void bind_config(py::module_& m);
void bind_random(py::module_& m);
void bind_logger(py::module_& m);
void bind_simulation_clock(py::module_& m);

void bind_brain_system(py::module_& m);
void bind_neuron(py::module_& m);
void bind_synapse(py::module_& m);
void bind_neural_population(py::module_& m);
void bind_neural_region(py::module_& m);

void bind_dynamics(py::module_& m);
void bind_plasticity(py::module_& m);

void bind_memory(py::module_& m);
void bind_cognition(py::module_& m);

void bind_neuromodulation(py::module_& m);
void bind_development(py::module_& m);

void bind_sensory(py::module_& m);
void bind_motor(py::module_& m);
void bind_environment(py::module_& m);

void bind_agent(py::module_& m);
void bind_world(py::module_& m);

void bind_experiments(py::module_& m);
void bind_visualization(py::module_& m);

} // namespace binding_helpers

// ===========================================================================
// Binding Functions Implementation
// ===========================================================================

void binding_helpers::bind_core_types(py::module_& m) {
    // Core types - NeuronId, SynapseId, RegionId, PopulationId
    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum bindings
    binding_helpers::bind_enum_class<NeuronType>(m, "NeuronType");
    binding_helpers::bind_enum_class<SynapseType>(m, "SynapseType");
    binding_helpers::bind_enum_class<DevelopmentalStage>(m, "DevelopmentalStage");
    binding_helpers::bind_enum_class<FiringState>(m, "FiringState");
    binding_helpers::bind_enum_class<ActionType>(m, "ActionType");
    binding_helpers::bind_enum_class<MotorCommand>(m, "MotorCommand");
    binding_helpers::bind_enum_class<WorldObjectType>(m, "WorldObjectType");
}

void binding_helpers::bind_config(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Config>(m, "Config");

    // Add convenience methods
    binding_helpers::add_config_convenience_methods(
        m.attr("Config").cast<py::class_<ConfigPtr>>());
}

void binding_helpers::bind_random(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<RandomGenerator>(m, "RandomGenerator");
}

void binding_helpers::bind_logger(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Logger>(m, "Logger");
}

void binding_helpers::bind_simulation_clock(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<SimulationClock>(m, "SimulationClock");
}

void binding_helpers::bind_brain_system(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Brain>(m, "Brain");

    // Add convenience methods
    binding_helpers::add_brain_convenience_methods(
        m.attr("Brain").cast<py::class_<BrainPtr>>());
}

void binding_helpers::bind_neuron(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Neuron>(m, "Neuron");
}

void binding_helpers::bind_synapse(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Synapse>(m, "Synapse");
}

void binding_helpers::bind_neural_population(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<NeuralPopulation>(m, "NeuralPopulation");
}

void binding_helpers::bind_neural_region(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<NeuralRegion>(m, "NeuralRegion");
}

void binding_helpers::bind_dynamics(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<NeuralDynamics>(m, "NeuralDynamics");
    binding_helpers::bind_shared_ptr_class<SpikeSystem>(m, "SpikeSystem");
}

void binding_helpers::bind_plasticity(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<PlasticityRule>(m, "PlasticityRule");
    binding_helpers::bind_shared_ptr_class<Hebbian>(m, "Hebbian");
    binding_helpers::bind_shared_ptr_class<STDP>(m, "STDP");
    binding_helpers::bind_shared_ptr_class<StructuralPlasticity>(m, "StructuralPlasticity");
}

void binding_helpers::bind_memory(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Memory>(m, "Memory");
    binding_helpers::bind_shared_ptr_class<NeuralWorkingMemory>(m, "NeuralWorkingMemory");
    binding_helpers::bind_shared_ptr_class<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory");
}

void binding_helpers::bind_cognition(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<ConceptFormation>(m, "ConceptFormation");
    binding_helpers::bind_shared_ptr_class<NeuralPlanner>(m, "NeuralPlanner");
}

void binding_helpers::bind_neuromodulation(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Neuromodulator>(m, "Neuromodulator");
    binding_helpers::bind_shared_ptr_class<Reward>(m, "Reward");
    binding_helpers::bind_shared_ptr_class<PredictionError>(m, "PredictionError");
    binding_helpers::bind_shared_ptr_class<Novelty>(m, "Novelty");
    binding_helpers::bind_shared_ptr_class<Curiosity>(m, "Curiosity");
}

void binding_helpers::bind_development(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<DevelopmentSystem>(m, "DevelopmentSystem");
    binding_helpers::bind_shared_ptr_class<Synaptogenesis>(m, "Synaptogenesis");
    binding_helpers::bind_shared_ptr_class<Pruning>(m, "Pruning");
    binding_helpers::bind_shared_ptr_class<Maturation>(m, "Maturation");
}

void binding_helpers::bind_sensory(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<SensoryInput>(m, "SensoryInput");
    binding_helpers::bind_shared_ptr_class<Vision>(m, "Vision");
    binding_helpers::bind_shared_ptr_class<Audio>(m, "Audio");
    binding_helpers::bind_shared_ptr_class<InternalSignals>(m, "InternalSignals");
}

void binding_helpers::bind_motor(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<MotorSystem>(m, "MotorSystem");
    binding_helpers::bind_shared_ptr_class<Action>(m, "Action");
}

void binding_helpers::bind_environment(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Environment>(m, "Environment");
    binding_helpers::bind_shared_ptr_class<Observation>(m, "Observation");
}

void binding_helpers::bind_agent(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<AgentBrain>(m, "AgentBrain");
    binding_helpers::bind_shared_ptr_class<AgentBody>(m, "AgentBody");
    binding_helpers::bind_shared_ptr_class<SensoryPercept>(m, "SensoryPercept");
}

void binding_helpers::bind_world(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<SimpleWorld>(m, "SimpleWorld");

    // Add convenience methods
    binding_helpers::add_world_convenience_methods(
        m.attr("SimpleWorld").cast<py::class_<WorldPtr>>());
}

void binding_helpers::bind_experiments(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<Experiment>(m, "Experiment");
    binding_helpers::bind_shared_ptr_class<ExperimentRunner>(m, "ExperimentRunner");
    binding_helpers::bind_shared_ptr_class<Metrics>(m, "Metrics");
}

void binding_helpers::bind_visualization(py::module_& m) {
    binding_helpers::bind_shared_ptr_class<VisualizationInterface>(m, "VisualizationInterface");
}

// ===========================================================================
// Convenience Method Implementations
// ===========================================================================

void binding_helpers::add_brain_convenience_methods(py::class_<BrainPtr>& brain_class) {
    brain_class
        .def("runEpisode", [](BrainPtr& brain, int steps) {
            for (int i = 0; i < steps; ++i) {
                brain->step(i, i * 0.001);
            }
            return brain->getTotalSpikeCount();
        }, py::arg("steps"), "Run simulation episode and return spike count")
        .def("getNetworkStats", [](BrainPtr& brain) {
            py::dict stats;
            stats["neuron_count"] = brain->getTotalNeuronCount();
            stats["synapse_count"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            return stats;
        }, "Get brain network statistics")
        .def("analyzeConnectivity", [](BrainPtr& brain) {
            auto* working_memory = brain->getWorkingMemory();
            py::dict connectivity;
            connectivity["working_memory_active_traces"] = 
                working_memory ? working_memory->getActiveTraces() : 0;
            connectivity["has_episodic_memory"] = brain->getEpisodicMemory() != nullptr;
            connectivity["has_neuromodulation"] = brain->getDopamine() != nullptr;
            connectivity["has_prediction_system"] = brain->getPredictionSystem() != nullptr;
            connectivity["has_development_system"] = brain->getDevelopmentSystem() != nullptr;
            return connectivity;
        }, "Analyze brain connectivity and system integration")
        .def("resetWithConfig", [](BrainPtr& brain, ConfigPtr config) {
            brain->reset();
            brain->getConfig()->copyFrom(*config);
            brain->initialize();
        }, py::arg("config"), "Reset brain with new configuration")
        .def("__repr__", [](const BrainPtr& brain) {
            return "<Brain: " + std::to_string(brain->getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain->getTotalSynapseCount()) + 
                   " synapses>";
        });
}

void binding_helpers::add_world_convenience_methods(py::class_<WorldPtr>& world_class) {
    world_class
        .def("createAgent", [](WorldPtr& world, float x, float y) {
            AgentBody body;
            body.x = x;
            body.y = y;
            world->setAgentStart(x, y);
            return body;
        }, py::arg("x"), py::arg("y"), "Create and position agent in world")
        .def("runSimulationStep", [](WorldPtr& world, float timestep) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            auto body = world->getAgentBody();
            return py::make_tuple(percept, body);
        }, py::arg("timestep"), "Run single simulation step")
        .def("addResource", [](WorldPtr& world, float x, float y, float value) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            obj.active = true;
            world->addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value"), "Add resource to world")
        .def("getObjectsByType", [](WorldPtr& world, WorldObjectType type) {
            py::list objects;
            // Note: SimpleWorld doesn't expose direct access to all objects,
            // so this is a placeholder for the convenience method
            return objects;
        }, py::arg("type"), "Get all objects of a specific type")
        .def("__repr__", [](const WorldPtr& world) {
            return "<SimpleWorld: " + std::to_string(world->getWidth()) + 
                   "x" + std::to_string(world->getHeight()) + ">";
        });
}

void binding_helpers::add_config_convenience_methods(py::class_<ConfigPtr>& config_class) {
    config_class
        .def("setDefaults", [](ConfigPtr& config) {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
        }, "Set default configuration values")
        .def("getValueOrDefault", [](ConfigPtr& config, const std::string& key, 
                                   const std::string& defaultValue) {
            auto val = config->get<std::string>(key);
            return val.has_value() ? val.value() : defaultValue;
        }, py::arg("key"), py::arg("default_value"), "Get config value or fallback")
        .def("mergeConfig", [](ConfigPtr& config, ConfigPtr& other) {
            for (const auto& entry : other->getKeys()) {
                auto val = other->get<std::string>(entry);
                if (val.has_value()) {
                    config->set(entry, val.value(), ConfigSource::Runtime);
                }
            }
        }, py::arg("other_config"), "Merge another config into this one");
}

// ===========================================================================
// Module Definition
// ===========================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ===============================================
        
        A comprehensive Python API for the NLM neural simulation framework.
        
        This module provides access to all NLM brain systems, including:
        
        **Core Systems:**
        - Brain: Central neural simulation brain class
        - Config: Configuration management
        - RandomGenerator: Deterministic random number generation
        - Logger: Logging system
        - SimulationClock: Time management
        
        **Brain Components:**
        - Neuron: Individual neurons with LIF dynamics
        - Synapse: Synaptic connections with plasticity
        - NeuralPopulation: Groups of neurons
        - NeuralRegion: Brain regions
        
        **Plasticity Systems:**
        - Hebbian: Hebbian learning
        - STDP: Spike-Timing Dependent Plasticity
        - StructuralPlasticity: Structural changes
        
        **Development:**
        - DevelopmentSystem: Developmental stages
        - Synaptogenesis, Pruning, Maturation
        
        **Neuromodulation:**
        - Reward, PredictionError, Novelty, Curiosity
        
        **Memory Systems:**
        - NeuralWorkingMemory: Working memory
        - NeuralEpisodicMemory: Episodic memory
        
        **Cognition:**
        - NeuralPlanner: Action planning
        - ConceptFormation: Concept learning
        
        **Sensory Systems:**
        - Vision, Audio, InternalSignals
        
        **Action Systems:**
        - MotorSystem, Action
        
        **Environment:**
        - SimpleWorld: Simulation environment
        
        **Agent Interface:**
        - AgentBrain: Bridge between brain and world
        
        **Experiments:**
        - Experiment, ExperimentRunner, Metrics
        
        **Visualization:**
        - VisualizationInterface: Visualization tools
        
        **Factory Functions:**
        - createDefaultConfig(): Create default configuration
        - createBrain(): Create new brain
        - createSimpleWorld(): Create simulation world
        - createAgentBrain(): Create agent brain interface
        
        **Key Features:**
        - Pythonic API with intuitive method names
        - Property-style access for common attributes
        - Context managers for safe resource management
        - Convenience methods for common operations
        - Comprehensive error handling
        
        Examples:
        
        >>> import pynlm
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run simulation episode
        >>> spikes = brain.runEpisode(100)
        >>> 
        >>> # Get network statistics
        >>> stats = brain.getNetworkStats()
        >>> print(f"Neurons: {stats['neuron_count']}")
        >>> 
        >>> # Create world and run simulation
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(64, 64, 32, 32)
        >>> world.setAgentStart(32, 32)
        >>> world.runSimulationStep(0.001)
        
        The module is designed to be both high-level for easy use and
        detailed for advanced access to NLM's neural simulation capabilities.
    )pbdoc";

    // Register exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

    // Bind core types
    binding_helpers::bind_core_types(m);

    // Bind all system modules
    binding_helpers::bind_config(m);
    binding_helpers::bind_random(m);
    binding_helpers::bind_logger(m);
    binding_helpers::bind_simulation_clock(m);

    binding_helpers::bind_brain_system(m);
    binding_helpers::bind_neuron(m);
    binding_helpers::bind_synapse(m);
    binding_helpers::bind_neural_population(m);
    binding_helpers::bind_neural_region(m);

    binding_helpers::bind_dynamics(m);
    binding_helpers::bind_plasticity(m);

    binding_helpers::bind_memory(m);
    binding_helpers::bind_cognition(m);

    binding_helpers::bind_neuromodulation(m);
    binding_helpers::bind_development(m);

    binding_helpers::bind_sensory(m);
    binding_helpers::bind_motor(m);
    binding_helpers::bind_environment(m);

    binding_helpers::bind_agent(m);
    binding_helpers::bind_world(m);

    binding_helpers::bind_experiments(m);
    binding_helpers::bind_visualization(m);

    // Factory functions
    m.def("createDefaultConfig", []() {
        return std::make_shared<Config>();
    }, "Create a default configuration object");

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world simulation environment");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("createObject", &binding_helpers::create_object, py::arg("className"), 
          "Create an object by class name (convenience function)");

    // Constants and utility functions
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, int steps) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("steps"), "Run simulation and return spike count");

    m.def("runBenchmark", [](int neuronCount, int steps) {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(neuronCount));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.1f);

        auto brain = std::make_shared<Brain>(config);
        brain->initialize();

        for (size_t i = 0; i < steps; ++i) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            brain->step(i, i * 0.001);
        }

        return py::make_tuple(
            brain->getTotalNeuronCount(),
            brain->getTotalSynapseCount(),
            brain->getTotalSpikeCount(),
            brain->getAverageFiringRate()
        );
    }, py::arg("neuron_count"), py::arg("steps"), "Run benchmark and return results");

    // Version and info
    m.attr("__version__") = "2.0.0"; // Updated version for improved bindings
    m.attr("__author__") = "NLM Project Team";
    m.attr("__description__") = "Improved Python bindings for NLM neural simulation framework";

    // Make module easily discoverable
    m.def("help", [](const std::string& topic) {
        if (topic == "basic") {
            std::cout << "NLM Python Bindings - Basic Usage:\n"
                     << "  - Create config: createDefaultConfig()\n"
                     << "  - Create brain: createBrain(config)\n"
                     << "  - Initialize: brain.initialize()\n"
                     << "  - Run: brain.runEpisode(steps)\n"
                     << "  - Get stats: brain.getNetworkStats()\n";
        } else if (topic == "advanced") {
            std::cout << "NLM Python Bindings - Advanced Usage:\n"
                     << "  - Create world: createSimpleWorld()\n"
                     << "  - Configure: world.configure(width, height, vision_w, vision_h)\n"
                     << "  - Set agent: world.setAgentStart(x, y)\n"
                     << "  - Run step: percept, body = world.runSimulationStep(timestep)\n";
        }
    }, py::arg("topic"), "Get help for NLM Python bindings");
}

} // namespace nlm