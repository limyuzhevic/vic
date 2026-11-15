#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides a comprehensive Python API for creating and
        simulating artificial developmental brains. It includes classes for
        Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Complete brain simulation with spiking neurons
        - Agent-based world interaction
        - Memory and prediction systems
        - Neuromodulation and development
        - Checkpointing and persistence
        - Extensive configuration options
        
        The API is designed to be intuitive and Pythonic, with strong
        type safety and comprehensive documentation.
        
        Basic Usage Example:
        ------------------
        import nlm
        
        # Create a brain configuration
        config = nlm.Config()
        config.set("neuron_count", 500)
        config.set("region_count", 2)
        config.set("connection_probability", 0.1)
        
        # Create and initialize the brain
        brain = nlm.Brain(config)
        brain.initialize()
        
        # Create world and agent
        world = nlm.SimpleWorld()
        world.configure(20.0, 20.0, 16, 16)
        
        agent = nlm.AgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run a learning simulation
        for episode in range(10):
            world.reset()
            agent.reset()
            brain.reset()
            
            for step in range(100):
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process input
                agent.processSensoryInput(percept)
                
                # Simulate brain
                brain.step(step, step * 0.001)
                
                # Get action
                action = agent.decodeMotorCommand()
                
                # Apply action
                result = world.applyMotorCommand(action, world.getSimulationTime())
                
                # Apply reward
                agent.applyRewardModulation(result.reward, 0.0)
                
                # Update development
                agent.updateDevelopment(0.1)
                
                # Check if simulation ended
                if world.getAgentBody().health <= 0:
                    break
        
        print(f"Learning complete! Final preference: {agent.getLearnedPreference():.3f}")
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ---------------------------------------------------------------------
    // Core Type Definitions
    // ---------------------------------------------------------------------

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron.
        
        Neuron IDs are generated automatically by the brain and are used
        to refer to specific neurons in the neural network.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse.
        
        Synapse IDs are generated automatically by the brain and are used
        to refer to specific synaptic connections.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region.
        
        Region IDs are used to organize neurons into functional groups
        and manage local connectivity patterns.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population.
        
        Population IDs group neurons that share similar characteristics,
        connectivity patterns, or functional roles.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enums with enhanced documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Enumeration of neuron types.
        
        Different neuron types have distinct physiological properties:
        - Excitatory: Promotes neural activity through positive feedback
        - Inhibitory: Suppresses neural activity through negative feedback
        - Modulatory: Regulates other neurons' activity
        - Sensory: Processes external sensory information
        - Motor: Generates motor output commands
        - Internal: Manages internal state and regulation
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Neuron that promotes neural activity")
        .value("Inhibitory", NeuronType::Inhibitory, "Neuron that suppresses neural activity")
        .value("Modulatory", NeuronType::Modulatory, "Neuron that regulates other neurons")
        .value("Sensory", NeuronType::Sensory, "Neuron that processes sensory information")
        .value("Motor", NeuronType::Motor, "Neuron that generates motor commands")
        .value("Internal", NeuronType::Internal, "Neuron that manages internal regulation")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Enumeration of synapse types.
        
        Synapses can be classified based on their transmission characteristics:
        - Excitatory: Releases excitatory neurotransmitters
        - Inhibitory: Releases inhibitory neurotransmitters
        - Modulatory: Releases neuromodulators
        - Electrical: Direct electrical coupling
        - GapJunction: Electrical gap junctions
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Enumeration of developmental stages.
        
        The brain progresses through distinct developmental phases:
        - Initial: Primitive neural structure
        - CriticalPeriod: Heightened plasticity
        - Maturation: Refinement of connections
        - Adult: Stable functionality
        - Aging: Gradual decline
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Primitive neural structure")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Heightened plasticity phase")
        .value("Maturation", DevelopmentalStage::Maturation, "Connection refinement phase")
        .value("Adult", DevelopmentalStage::Adult, "Stable functionality phase")
        .value("Aging", DevelopmentalStage::Aging, "Gradual decline phase")
        .export_values();

    // ---------------------------------------------------------------------
    // Configuration System
    // ---------------------------------------------------------------------

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration system for NLM parameters.
        
        The Config class allows setting and getting configuration parameters
        that control the behavior of the neural simulation. Configuration
        supports multiple value types (integers, floats, strings, booleans,
        and arrays) and can be loaded from files or command-line arguments.
        
        Common configuration parameters:
        - neuron_count: Number of neurons in the brain
        - region_count: Number of brain regions
        - connection_probability: Probability of synaptic connections
        - random_seed: Random seed for reproducible simulations
        - simulation_timestep: Duration of each simulation step
        - stdp_ltp_weight: STDP long-term potentiation weight
        - stdp_ltd_weight: STDP long-term depression weight
        - synaptogenesis_rate: Rate of new synapse formation
        - pruning_rate: Rate of synapse elimination
    )pbdoc")
        .def(py::init<>())
        .def("set", static_cast<void (Config::*)(const std::string&, const ConfigValue&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             R"pbdoc(Set a configuration value.
             
             Args:
                 key: Configuration key (e.g., "neuron_count")
                 value: Configuration value (int, float, str, bool, or list)
             ")pbdoc")
        .def("set", static_cast<void (Config::*)(const std::string&, int)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Convenience method for setting integer values")
        .def("set", static_cast<void (Config::*)(const std::string&, int64_t)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Convenience method for setting int64 values")
        .def("set", static_cast<void (Config::*)(const std::string&, double)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Convenience method for setting float/double values")
        .def("set", static_cast<void (Config::*)(const std::string&, bool)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Convenience method for setting boolean values")
        .def("set", static_cast<void (Config::*)(const std::string&, const std::string&)>(&Config::set),
             py::arg("key"), py::arg("value"),
             "Convenience method for setting string values")
        .def("get", static_cast<std::optional<int> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as integer")
        .def("get", static_cast<std::optional<int64_t> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as int64")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as double")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as boolean")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&)>(&Config::get),
             py::arg("key"),
             "Get configuration value as string")
        .def("getOr", static_cast<int (Config::*)(const std::string&, const int&)>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default")
        .def("getOr", static_cast<int64_t (Config::*)(const std::string&, const int64_t&)>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (int64)")
        .def("getOr", static_cast<double (Config::*)(const std::string&, const double&)>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (double)")
        .def("getOr", static_cast<bool (Config::*)(const std::string&, const bool&)>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (boolean)")
        .def("getOr", static_cast<std::string (Config::*)(const std::string&, const std::string&)>(&Config::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with default (string)")
        .def("has", &Config::has, py::arg("key"),
             "Check if configuration key exists")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration values")
        .def("summary", &Config::summary,
             "Get configuration summary")
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from file")
        .def("loadFromArgs", &Config::loadFromArgs, py::arg("argc"), py::arg("argv"),
             "Load configuration from command-line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to file")
        .def("toJson", &Config::toJson,
             "Export configuration as JSON")
        .def("fromJson", &Config::fromJson, py::arg("json"),
             "Create configuration from JSON");

    // ---------------------------------------------------------------------
    // Brain Simulation
    // ---------------------------------------------------------------------

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        The main neural brain simulation.
        
        The Brain class represents a complete artificial neural system that
        simulates spiking neural dynamics, learning, memory, and cognition.
        It integrates multiple specialized systems:
        
        - Neural components (neurons, synapses, populations, regions)
        - Neural dynamics (spike generation, propagation, plasticity)
        - Memory systems (working memory, episodic memory)
        - Prediction systems (temporal prediction, error computation)
        - Cognitive systems (planning, attention, concept formation)
        - Neuromodulation systems (dopamine, curiosity, novelty)
        - Developmental systems (growth, maturation, aging)
        
        The brain operates in closed-loop with the world: sensory input
        → neural processing → action → world consequences → reward → learning.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // ---------------------------------------------------------------------
    // Agent Interface
    // ---------------------------------------------------------------------

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Interface between NLM brain and world simulation.
        
        The AgentBrain class provides a simplified interface for using the NLM
        in world interaction scenarios. It handles the closed-loop brain-world
        interaction by managing sensory input, motor output, and reward processing.
        
        Key responsibilities:
        - Receive sensory percepts from world
        - Decode motor commands from brain activity
        - Apply reward-based neuromodulation
        - Update developmental processes
        - Maintain agent state and preferences
        
        This class is typically used for:
        - Interactive learning demonstrations
        - Behavioral experiments
        - Environment exploration tasks
        - Skill acquisition studies
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ---------------------------------------------------------------------
    // World Simulation
    // ---------------------------------------------------------------------

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple world simulation for NLM agents.
        
        The SimpleWorld class provides a minimal 2D environment for testing
        and evaluating NLM agents. It includes:
        
        - 2D grid world with obstacles
        - Resource collection (positive reward)
        - Hazard avoidance (negative reward)
        - Energy management system
        - Velocity and movement physics
        - Touch sensors for proximity detection
        - Vision system with ray casting
        - Collision detection and handling
        
        The world provides:
        - **Resources**: Positive reward when collected (+10.0f)
        - **Hazards**: Negative reward when hit (-15.0f)
        - **Movement**: Small negative costs (-0.05f to -0.08f)
        - **Actions**: Turn, move, look, interact
        - **Energy**: Decays over time, refilled by resources
        - **Health**: Decreases from hazard contact
        
        The world is designed for agent learning and experimentation,
        with clear reward signals for reinforcement learning.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and sensing")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update,
             py::arg("timestep"),
             "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getVisionWidth", &SimpleWorld::getVisionWidth)
        .def("getVisionHeight", &SimpleWorld::getVisionHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"))
        .def("nextRandom", &SimpleWorld::nextRandom);

    // ---------------------------------------------------------------------
    // Helper Functions
    // ---------------------------------------------------------------------

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")
        .def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}
    // Setup callback function for PyBind11
    
} // namespace nlm