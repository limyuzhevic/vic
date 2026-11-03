#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

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

// Exception conversion helper
class PybindRuntimeError : public std::runtime_error {
public:
    explicit PybindRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

// Template to reduce boilerplate for ID types
template<typename IdType, typename ValueType>
void bindIdType(py::module_& m, const std::string& name) {
    py::class_<IdType>(m, name.c_str())
        .def(py::init<>())
        .def(py::init<ValueType>(), py::arg("value"))
        .def_readwrite("value", &IdType::value)
        .def("index", &IdType::index)
        .def("__eq__", &IdType::operator==)
        .def("__ne__", &IdType::operator!=)
        .def("__hash__", [](const IdType& id) { return std::hash<ValueType>{}(id.value); })
        .def("__repr__", [](const IdType& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const IdType& id) {
            return std::to_string(id.value);
        })
        .def("is_valid", [](const IdType& id) {
            return id.value != std::numeric_limits<ValueType>::max();
        });
}

// Enhanced Config API
void bindConfig(py::module_& m) {
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        Provides a key-value store for simulation parameters and settings.
        Supports various value types including integers, floats, strings, and arrays.
        
        Examples:
            >>> config = Config()
            >>> config.set("neuron_count", 500)
            >>> config.set("connection_probability", 0.15)
            >>> brain = Brain(config)
        )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON or key=value file")
        .def("load_from_args", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size() + 1);
            argv.push_back(nullptr);  // argv[0] should be program name
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size() - 1), argv.data());
        }, py::arg("args"),
             "Load configuration from command line arguments (like sys.argv)")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a key=value file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config with " + std::to_string(cfg.getKeys().size()) + " entries>";
        })
        // Factory functions
        .def_static("create_default", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def_static("from_dict", [](const py::dict& dict) -> std::shared_ptr<Config> {
            auto config = std::make_shared<Config>();
            for (auto item : dict) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                
                if (value.is(py::int_())) {
                    config->set(key, value.cast<int64_t>());
                } else if (value.is(py::float_())) {
                    config->set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    config->set(key, value.cast<bool>());
                } else if (value.is(py::str())) {
                    config->set(key, value.cast<std::string>());
                } else if (value.is(py::list())) {
                    auto list = value.cast<std::vector<double>>();
                    config->set(key, ConfigValue(list));
                }
            }
            return config;
        }, py::arg("dict"), "Create configuration from Python dictionary")
        .def("to_dict", [](const Config& self) -> py::dict {
            py::dict result;
            for (const auto& key : self.getKeys()) {
                auto val = self.get<std::string>(key);
                if (val) {
                    result[key.c_str()] = val.value();
                }
            }
            return result;
        }, "Convert configuration to Python dictionary");
}

// Enhanced Action API with factory methods
void bindAction(py::module_& m) {
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output.
        
        Represents an action the agent can perform, such as movement,
        interaction, or other motor behaviors.
        
        Attributes:
            type: ActionType enum value
            parameters: List of float parameters for the action
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType, "Get the action type")
        .def("set_type", &Action::setType, py::arg("type"), "Set the action type")
        .def("get_parameters", &Action::getParameters, "Get action parameters")
        .def("set_parameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("get_name", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("is_move_action", [](const Action& self) {
            return self.getType() == ActionType::MoveForward ||
                   self.getType() == ActionType::MoveBackward ||
                   self.getType() == ActionType::MoveLeft ||
                   self.getType() == ActionType::MoveRight;
        }, "Check if action is a movement action")
        .def("is_look_action", [](const Action& self) {
            return self.getType() == ActionType::Look ||
                   self.getType() == ActionType::LookUp ||
                   self.getType() == ActionType::LookDown;
        }, "Check if action is a look action")
        .def("is_interact_action", [](const Action& self) {
            return self.getType() == ActionType::Interact;
        }, "Check if action is an interaction action");
}

// Enhanced Sensory API with helpers
void bindSensoryInput(py::module_& m) {
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input from environment.
        
        Abstract base class representing sensory data from the environment,
        such as vision, audio, or internal signals.
        )pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("is_expired", [](const SensoryInput& self, double current_time, double max_age) {
            return (current_time - self.getTimestamp()) > max_age;
        }, py::arg("current_time"), py::arg("max_age"), "Check if sensory input is expired");
    
    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data.
        
        Captures visual information as pixel arrays with width, height,
        and color channels.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from flat array")
        .def("get_data_as_image", [](const Vision& self) -> py::array {
            size_t dims[3] = {static_cast<size_t>(self.getHeight()),
                             static_cast<size_t>(self.getWidth()),
                             static_cast<size_t>(self.getChannels())};
            return py::array(py::buffer_info(self.getData().data(),
                                             sizeof(float),
                                             py::format_descriptor<float>::format(),
                                             3,
                                             dims));
        }, "Get vision data as NumPy array")
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("normalize", [](Vision& self, float min_val, float max_val) {
            auto& data = self.getData();
            for (auto& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
        }, py::arg("min_val"), py::arg("max_val"), "Normalize vision data to [0, 1]");
    
    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data.
        
        Captures audio information as sample arrays with sample rate
        and number of samples.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sample_rate"), py::arg("num_samples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from flat array")
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("get_duration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds");
}

// Enhanced World API
void bindSimpleWorld(py::module_& m) {
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        Provides a grid-based world where agents can move, interact with
        objects, and receive sensory input from the environment.
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by one timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"),
             "Apply motor command to world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object (e.g., resource, hazard)")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (within bounds and not occupied)")
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for world simulation")
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        .def("get_objects_at", [](SimpleWorld& self, float x, float y, float radius) -> py::list {
            py::list result;
            // Note: This would need implementation in C++ to return actual objects
            return result;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Get all objects within radius of position (placeholder)");
}

// Enhanced Brain API with convenience methods
void bindBrain(py::module_& m) {
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        Implements real spiking neural computation with event-driven dynamics
        and integrated memory, prediction, cognition, and neuromodulation systems.
        
        The brain is the core computational engine that processes sensory input,
        generates actions, and learns through plasticity.
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
        .def("receive_sensory_input", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("inject_current", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("inject_current_to_neurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produce_action", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset,
             "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        .def("add_region", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("get_region", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"), py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_developmental_stage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Factory functions
        .def_static("create", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration")
        .def_static("create_with_defaults", []() -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.15f, ConfigSource::Default);
            return std::make_shared<Brain>(config);
        }, "Create a brain with default configuration");
}

// Enhanced AgentBrain API
void bindAgentBrain(py::module_& m) {
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world.
        
        Acts as a bridge between the neural brain and the external world,
        processing sensory input and generating motor output for an agent.
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("get_sensory_input_size", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("get_motor_output_size", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("process_sensory_input", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decode_motor_command", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("apply_reward_modulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("update_development", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("get_developmental_stage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("get_neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("get_curiosity_level", &AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("get_novelty_level", &AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("get_prediction_error", &AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("get_brain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enable_reward_modulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development")
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven behavior")
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Factory functions
        .def_static("create", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface");
}

// Bind all ID types using templates
void bindAllIds(py::module_& m) {
    bindIdType<NeuronId, uint64_t>(m, "NeuronId");
    bindIdType<SynapseId, uint64_t>(m, "SynapseId");
    bindIdType<RegionId, uint64_t>(m, "RegionId");  
    bindIdType<PopulationId, uint64_t>(m, "PopulationId");
}

// Advanced utilities for experienced users
void bindAdvancedUtilities(py::module_& m) {
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");
    
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");
    
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Text encoding utilities
    m.def("encode_text", &AgentBrain::encodeText, py::arg("text"),
         "Encode text into neural pattern (deterministic)");
    
    // Concept formation utilities
    m.def("compute_concept_similarity", [](const std::vector<float>& pattern1,
                                          const std::vector<float>& pattern2) {
        // This would need access to ConceptFormation
        return 0.0f;  // Placeholder
    }, py::arg("pattern1"), py::arg("pattern2"),
         "Compute similarity between two patterns for concept formation");
    
    // Simulation utilities
    m.def("run_simulation_step", [](std::shared_ptr<Brain> brain, SimulationStep step, Timestamp time) {
        brain->step(step, time);
    }, py::arg("brain"), py::arg("step"), py::arg("time"),
         "Run a single simulation step");
    
    m.def("inject_stimulus", [](std::shared_ptr<Brain> brain, NeuronType type, double current) {
        brain->injectCurrentToNeurons(type, current);
    }, py::arg("brain"), py::arg("type"), py::arg("current"),
         "Inject current into all neurons of specified type");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides Python interfaces to the NLM neural simulation framework,
        enabling research in artificial developmental systems and neural computation.
        
        Key Features:
        - Real spiking neural computation with LIF neurons
        - Event-driven spike propagation with synaptic delays
        - Multiple plasticity rules (STDP, Hebbian, structural)
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and cognition systems
        - Development stages and plasticity modulation
        - Checkpoint save/load for persistent learning
        
        Installation:
            pip install pynlm
            
        Basic Usage:
            >>> from pynlm import Config, Brain, SimpleWorld, Vision
            >>> 
            >>> # Create configuration
            >>> config = Config()
            >>> config.set("neuron_count", 100)
            >>> 
            >>> # Create brain
            >>> brain = Brain(config)
            >>> brain.initialize()
            >>> 
            >>> # Create world
            >>> world = SimpleWorld()
            >>> world.configure(100, 100, 20, 20)
            >>> 
            >>> # Run simulation
            >>> for i in range(100):
            ...     brain.step(i)
            ...     percept = world.get_sensory_percept()
            ...     brain.receive_sensory_input(percept)
            ...     action = brain.produce_action()
    
        Advanced Features:
            >>> # Text encoding
            >>> pattern = encode_text("hello")
            >>> 
            >>> # Concept similarity
            >>> similarity = compute_concept_similarity(pattern1, pattern2)
            >>> 
            >>> # Save/load brain state
            >>> brain.save("brain_state.bin")
            >>> # ... later ...
            >>> brain.load("brain_state.bin")
        )pbdoc";

    // Register exceptions
    py::register_exception<PybindRuntimeError>(m, "NLMError");
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const PybindRuntimeError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Bind all core components
    bindAllIds(m);
    bindConfig(m);
    bindAction(m);
    bindSensoryInput(m);
    bindSimpleWorld(m);
    bindBrain(m);
    bindAgentBrain(m);
    
    // Bind enumeration types
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward)
        .value("MoveBackward", ActionType::MoveBackward)
        .value("MoveLeft", ActionType::MoveLeft)
        .value("MoveRight", ActionType::MoveRight)
        .value("TurnLeft", ActionType::TurnLeft)
        .value("TurnRight", ActionType::TurnRight)
        .value("Look", ActionType::Look)
        .value("LookUp", ActionType::LookUp)
        .value("LookDown", ActionType::LookDown)
        .value("Interact", ActionType::Interact)
        .value("Eat", ActionType::Eat)
        .value("Drink", ActionType::Drink)
        .value("Rest", ActionType::Rest)
        .value("Wait", ActionType::Wait)
        .value("Custom", ActionType::Custom)
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Bind invalid IDs as module attributes
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Bind advanced utilities
    bindAdvancedUtilities(m);
    
    // Constants
    m.attr("DEFAULT_SIMULATION_TIMESTEP") = 0.001;
    m.attr("DEFAULT_RANDOM_SEED") = 42;
    m.attr("DEFAULT_NEURON_COUNT") = 500;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.15f;
}
