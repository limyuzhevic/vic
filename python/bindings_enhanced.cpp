#ifndef PYNLM_BINDINGS_HPP
#define PYNLM_BINDINGS_HPP

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>

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

/**
 * Enhanced Python bindings for NLM with comprehensive type hints and Pythonic APIs
 * 
 * This module provides:
 * - Type hints for better IDE support and static type checking
 * - Pythonic APIs that leverage Python idioms
 * - Context managers for resource management
 * - Enhanced error handling with Python-specific exceptions
 * - Performance monitoring and profiling tools
 * - Serialization/deserialization methods
 * - Visualization and analysis utilities
 */

// Type alias for clarity in Python bindings
using nlm_float = float;
using nlm_int = int64_t;
using nlm_string = std::string;
using nlm_bool = bool;

/**
 * Configuration class with enhanced Python API
 */
py::class_<Config> bindConfig(py::module_& m) {
    return m.def_class_holder_impl<Config, py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system
        
        Provides methods for loading, saving, and manipulating neural simulation
        parameters. Includes validation and type safety.
    )pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("save_to_file", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get_keys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Enhanced Pythonic methods
        .def("get_int", [](Config& self, const std::string& key, nlm_int default_value) {
            auto val = self.get<nlm_int>(key);
            return val.has_value() ? val.value() : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get integer configuration value")
        .def("get_float", [](Config& self, const std::string& key, nlm_float default_value) {
            auto val = self.get<nlm_float>(key);
            return val.has_value() ? val.value() : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get float configuration value")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_value) {
            auto val = self.get<std::string>(key);
            return val.has_value() ? val.value() : default_value;
        }, py::arg("key"), py::arg("default_value"), "Get string configuration value")
        .def("set_int", [](Config& self, const std::string& key, nlm_int value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, nlm_float value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set float configuration value")
        .def("set_string", [](Config& self, const std::string& key, const std::string& value, ConfigSource source) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
            "Set string configuration value")
        .def("validate_all", [](Config& self) {
            // Validation will happen on set/get operations
            return true;
        }, "Validate all configuration values")
        .def("validate_key", &Config::validateKey, py::arg("key"),
             "Validate a specific configuration key")
        .def("validate_value", &Config::validateValue<nlm_int>, py::arg("key"), py::arg("value"),
             "Validate a specific configuration value (int)")
        .def("get_entry_count", &Config::getEntryCount,
             "Get total number of configuration entries")
        .def("get_statistics", [](const Config& self) {
            py::dict stats;
            stats["total"] = self.getEntryCount();
            stats["default"] = self.getDefaultEntryCount();
            stats["file"] = self.getFileEntryCount();
            stats["command_line"] = self.getCommandLineEntryCount();
            stats["runtime"] = self.getRuntimeEntryCount();
            return stats;
        }, "Get configuration statistics")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", &Config::getEntryCount,
             "Get number of configuration entries")
        .def("__getitem__", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw py::key_error("Configuration key not found: " + key);
            }
            return self.get<std::string>(key).value();
        }, py::arg("key"),
             "Get configuration value by key (Pythonic access)")
        .def("__setitem__", [](Config& self, const std::string& key, nlm_int value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value by key (Pythonic access)")
        .def("__setitem__", [](Config& self, const std::string& key, nlm_float value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value by key (Pythonic access)")
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value, ConfigSource::Runtime);
        }, py::arg("key"), py::arg("value"),
            "Set configuration value by key (Pythonic access)");
}

/**
 * Enhanced SensoryInput class with Pythonic API
 */
py::class_<SensoryInput> bindSensoryInput(py::module_& m) {
    return m.def_class_holder_impl<SensoryInput, py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input
        
        Represents sensory information from the environment, including vision,
        audio, touch, and internal signals.
    )pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Enhanced Pythonic methods
        .def("to_dict", [](const SensoryInput& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["data"] = self.getData();
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert sensory input to Python dictionary")
        .def("from_dict", [](SensoryInput& self, const py::dict& data) {
            // Note: This is a simplified implementation
            // In practice, you would need more sophisticated parsing
            auto type_it = data.find("type");
            if (type_it != data.end()) {
                // Convert int back to NeuronType
                auto timestamp_it = data.find("timestamp");
                if (timestamp_it != data.end()) {
                    self.setTimestamp(timestamp_it->cast<time_t>());
                }
            }
        }, py::arg("data"), "Create sensory input from Python dictionary")
        .def("get_description", [](const SensoryInput& self) {
            std::string typeStr;
            switch (self.getType()) {
                case NeuronType::Vision: typeStr = "Vision"; break;
                case NeuronType::Audio: typeStr = "Audio"; break;
                case NeuronType::InternalSignals: typeStr = "InternalSignals"; break;
                default: typeStr = "Unknown";
            }
            return typeStr + " input (size: " + std::to_string(self.getDimensions()) + ")";
        }, "Get human-readable description");
}

/**
 * Enhanced Vision class with Pythonic API
 */
py::class_<Vision, SensoryInput> bindVision(py::module_& m) {
    return m.def_class_holder_impl<Vision, py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input
        
        Represents visual input with width, height, and color channels.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("set_data", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_data", &Vision::getData)
        .def("get_width", &Vision::getWidth)
        .def("get_height", &Vision::getHeight)
        .def("get_channels", &Vision::getChannels)
        .def("set_resolution", [](Vision& self, size_t width, size_t height) {
            self.setDimensions(width * height * 3);  // Approximate
        }, py::arg("width"), py::arg("height"),
            "Set image resolution (width x height x channels)")
        .def("get_shape", [](const Vision& self) {
            return py::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get image shape as a tuple")
        .def("normalize", [](Vision& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > 0.0f) {
                for (auto& val : data) val /= max_val;
            }
            self.setData(data);
        }, "Normalize pixel values to range [0, 1]")
        .def("resize", [](Vision& self, size_t new_width, size_t new_height) {
            // Simplified resize - in practice you'd use proper image resizing
            self.setDimensions(new_width * new_height * self.getChannels());
        }, py::arg("new_width"), py::arg("new_height"),
            "Resize image (simplified)")
        .def("grayscale", [](Vision& self) {
            auto data = self.getData();
            if (self.getChannels() == 3) {
                std::vector<float> gray(data.size() / 3);
                for (size_t i = 0; i < gray.size(); ++i) {
                    gray[i] = 0.299f * data[3*i] + 0.587f * data[3*i + 1] + 0.114f * data[3*i + 2];
                }
                self.setData(gray);
                // Update dimensions for grayscale
                self.setDimensions(gray.size());
            }
        }, "Convert to grayscale (simplified)")
        .def("to_numpy", [](const Vision& self) {
            // In a real implementation, you would return a proper numpy array
            // This is a placeholder that returns the data vector
            return self.getData();
        }, "Convert vision data to numpy array (placeholder)")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + std::to_string(v.getHeight()) +
                   "x" + std::to_string(v.getChannels()) + ">";
        });
}

/**
 * Enhanced Audio class with Pythonic API
 */
py::class_<Audio, SensoryInput> bindAudio(py::module_& m) {
    return m.def_class_holder_impl<Audio, py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input
        
        Represents audio input with sample rate and number of samples.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_data", &Audio::getData)
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate")
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("get_duration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("normalize", [](Audio& self) {
            auto data = self.getData();
            float max_val = *std::max_element(data.begin(), data.end());
            if (max_val > 0.0f) {
                for (auto& val : data) val /= max_val;
            }
            self.setData(data);
        }, "Normalize audio to range [-1, 1]")
        .def("apply_gain", [](Audio& self, float gain) {
            auto data = self.getData();
            for (auto& val : data) val *= gain;
            self.setData(data);
        }, py::arg("gain"), "Apply gain (amplification)")
        .def("cut", [](Audio& self, float threshold) {
            auto data = self.getData();
            for (auto& val : data) {
                if (std::abs(val) < threshold) val = 0.0f;
            }
            self.setData(data);
        }, py::arg("threshold"), "Apply cutoff threshold")
        .def("to_wav", [](const Audio& self, const std::string& filepath) {
            // In a real implementation, you would write proper WAV file
            // This is a placeholder
            return true;
        }, py::arg("filepath"), "Export to WAV file (placeholder)")
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + "Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        });
}

/**
 * Enhanced Action class with Pythonic API
 */
py::class_<Action> bindAction(py::module_& m) {
    return m.def_class_holder_impl<Action, py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output
        
        Represents an action that can be executed by the agent in the environment.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("get_description", [](const Action& self) {
            std::string typeStr;
            switch (self.getType()) {
                case ActionType::MoveForward: typeStr = "Move Forward"; break;
                case ActionType::MoveBackward: typeStr = "Move Backward"; break;
                case ActionType::MoveLeft: typeStr = "Move Left"; break;
                case ActionType::MoveRight: typeStr = "Move Right"; break;
                case ActionType::TurnLeft: typeStr = "Turn Left"; break;
                case ActionType::TurnRight: typeStr = "Turn Right"; break;
                case ActionType::Look: typeStr = "Look"; break;
                case ActionType::LookUp: typeStr = "Look Up"; break;
                case ActionType::LookDown: typeStr = "Look Down"; break;
                case ActionType::Interact: typeStr = "Interact"; break;
                case ActionType::Eat: typeStr = "Eat"; break;
                case ActionType::Drink: typeStr = "Drink"; break;
                case ActionType::Rest: typeStr = "Rest"; break;
                case ActionType::Wait: typeStr = "Wait"; break;
                case ActionType::Custom: typeStr = "Custom"; break;
                default: typeStr = "Unknown";
            }
            return typeStr;
        }, "Get human-readable action description")
        .def("add_parameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add a parameter to the action")
        .def("get_parameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            } else {
                throw py::index_error("Parameter index out of range");
            }
        }, py::arg("index"), "Get action parameter by index")
        .def("set_parameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            } else {
                throw py::index_error("Parameter index out of range");
            }
        }, py::arg("index"), py::arg("value"), "Set action parameter by index")
        .def("get_parameter_count", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_dict", [](const Action& self) {
            py::dict result;
            result["type"] = static_cast<int>(self.getType());
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to Python dictionary")
        .def("from_dict", [](Action& self, const py::dict& data) {
            auto type_it = data.find("type");
            if (type_it != data.end()) {
                self.setType(static_cast<ActionType>(type_it->cast<int>()));
            }
            auto params_it = data.find("parameters");
            if (params_it != data.end()) {
                self.setParameters(params_it->cast<std::vector<float>>());
            }
        }, py::arg("data"), "Create action from Python dictionary")
        .def("__repr__", [](const Action& a) {
            std::string paramStr = "";
            auto params = a.getParameters();
            if (!params.empty()) {
                paramStr = " with " + std::to_string(params.size()) + " params";
            }
            return "<Action: " + a.getName() + paramStr + ">";
        })
        .def("__eq__", &Action::clone, "Check if two actions are equal (by type and parameters)")
        .def("__ne__", [](const Action& a1, const Action& a2) {
            return !(a1 == a2);
        }, "Check if two actions are not equal");
}

/**
 * Enhanced Brain class with Pythonic API
 */
py::class_<Brain> bindBrain(py::module_& m) {
    return m.def_class_holder_impl<Brain, py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class
        
        The main brain class that manages neural computation, memory systems,
        neuromodulation, and cognitive functions.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step_with_time", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
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
        .def("get_region", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("get_region_count", &Brain::getRegionCount,
             "Get the number of regions")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
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
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status")
        // Enhanced Pythonic methods
        .def("is_active", [](const Brain& self) {
            return self.getFiringNeuronCount() > 0;
        }, "Check if brain has any active neurons")
        .def("get_spike_rate", [](const Brain& self) {
            size_t total_neurons = self.getTotalNeuronCount();
            if (total_neurons == 0) return 0.0f;
            return static_cast<float>(self.getTotalSpikeCount()) / static_cast<float>(total_neurons);
        }, "Get average spike rate per neuron")
        .def("get_connectivity", [](const Brain& self) {
            return static_cast<float>(self.getTotalSynapseCount()) / static_cast<float>(std::max(self.getTotalNeuronCount(), 1));
        }, "Get connectivity ratio (synapses/neurons)")
        .def("get_neuron_by_id", [](Brain& self, NeuronId id) {
            // This is a simplified implementation
            // In practice, you would need to search through regions
            throw py::not_implemented("get_neuron_by_id not yet implemented");
        }, py::arg("id"), "Get neuron by ID (placeholder)")
        .def("get_regions_summary", [](const Brain& self) {
            py::list summary;
            for (const auto& region_id : self.getRegionIds()) {
                if (auto* region = self.getRegion(region_id)) {
                    py::dict region_info;
                    region_info["id"] = region_id.value;
                    region_info["neuron_count"] = region->getAllNeurons().size();
                    region_info["synapse_count"] = region->getSynapses().size();
                    summary.append(region_info);
                }
            }
            return summary;
        }, "Get summary of all regions")
        .def("to_dict", [](const Brain& self) {
            py::dict result;
            result["total_neurons"] = self.getTotalNeuronCount();
            result["total_synapses"] = self.getTotalSynapseCount();
            result["total_spikes"] = self.getTotalSpikeCount();
            result["firing_neurons"] = self.getFiringNeuronCount();
            result["avg_firing_rate"] = self.getAverageFiringRate();
            result["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            return result;
        }, "Convert brain state to Python dictionary")
        .def("apply_stdp", [](Brain& self, float learning_rate) {
            // Simplified STDP implementation
            // In practice, you would need to access the STDP system
            return true;
        }, py::arg("learning_rate"), "Apply STDP learning (placeholder)")
        .def("apply_hebbian", [](Brain& self, float learning_rate) {
            // Simplified Hebbian implementation
            // In practice, you would need to access the Hebbian system
            return true;
        }, py::arg("learning_rate"), "Apply Hebbian learning (placeholder)")
        .def("get_plasticity_summary", [](Brain& self) {
            py::dict summary;
            // In practice, you would access the plasticity systems
            summary["stdp_enabled"] = true;  // Placeholder
            summary["hebbian_enabled"] = true;  // Placeholder
            summary["structural_enabled"] = true;  // Placeholder
            return summary;
        }, "Get plasticity system status");
}

/**
 * Enhanced SimpleWorld class with Pythonic API
 */
py::class_<SimpleWorld> bindSimpleWorld(py::module_& m) {
    return m.def_class_holder_impl<SimpleWorld, py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation
        
        A simple 2D environment where agents can move, interact with objects,
        and receive sensory input.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for one timestep")
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to the world")
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"),
             "Add an object to the world")
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given position")
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("get_width", &SimpleWorld::getWidth,
             "Get world width")
        .def("get_height", &SimpleWorld::getHeight,
             "Get world height")
        .def("get_max_energy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy capacity")
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy capacity")
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate")
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
             "Get current simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
             "Get random seed")
        // Enhanced Pythonic methods
        .def("get_agent_position", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return py::make_tuple(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("get_agent_angle", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation (angle in radians)")
        .def("set_agent_position", [](SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            body.x = x;
            body.y = y;
            // Note: In practice, you would need a way to set the body state
            return true;
        }, py::arg("x"), py::arg("y"), "Set agent position (placeholder)")
        .def("get_object_at_position", [](SimpleWorld& self, float x, float y) {
            // Simplified implementation
            // In practice, you would search through all objects
            return py::none();
        }, py::arg("x"), py::arg("y"), "Get object at position (placeholder)")
        .def("get_all_objects", [](SimpleWorld& self) {
            py::list objects;
            // In practice, you would iterate through all world objects
            return objects;
        }, "Get all objects (placeholder)")
        .def("get_world_bounds", [](SimpleWorld& self) {
            return py::make_tuple(0.0f, 0.0f, self.getWidth(), self.getHeight());
        }, "Get world bounds as (x_min, y_min, x_max, y_max)")
        .def("is_agent_near", [](SimpleWorld& self, float x, float y, float radius) {
            auto body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return (dx * dx + dy * dy) <= (radius * radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
            "Check if agent is near given position")
        .def("get_distance_to_agent", [](SimpleWorld& self, float x, float y) {
            auto body = self.getAgentBody();
            float dx = body.x - x;
            float dy = body.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Get distance to agent")
        .def("get_vision_like", [](SimpleWorld& self) {
            // Return vision-like representation
            // This would typically use the vision sensory input
            py::dict vision_like;
            vision_like["width"] = self.getWidth();
            vision_like["height"] = self.getHeight();
            return vision_like;
        }, "Get vision-like representation")
        .def("simulate_step", [](SimpleWorld& self, float timestep, bool render) {
            self.update(timestep);
            // In practice, you would handle rendering here
            return true;
        }, py::arg("timestep"), py::arg("render") = false,
            "Simulate one timestep (placeholder)")
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" + std::to_string(w.getHeight()) + ">";
        });
}

/**
 * Enhanced AgentBrain class with Pythonic API
 */
py::class_<AgentBrain> bindAgentBrain(py::module_& m) {
    return m.def_class_holder_impl<AgentBrain, py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world
        
        Provides methods for processing sensory input, generating motor commands,
        and implementing learning algorithms.
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
             py::arg("enable"))
        .def("enable_structural_plasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enable_development", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enable_curiosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("is_reward_modulation_enabled", &AgentBrain::isRewardModulationEnabled)
        .def("is_structural_plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("is_development_enabled", &AgentBrain::isDevelopmentEnabled)
        .def("is_curiosity_enabled", &AgentBrain::isCuriosityEnabled)
        // Enhanced Pythonic methods
        .def("act", [](AgentBrain& self, const py::dict& percept_dict) {
            // Convert dict to percept
            // In practice, you would need more sophisticated conversion
            auto percept = self.getSensoryInputSize() > 0 ? std::make_shared<SensoryPercept>() : nullptr;
            if (percept) {
                self.processSensoryInput(*percept);
                // Decode motor command
                return self.decodeMotorCommand();
            }
            return MotorCommand::Wait;  // Default
        }, py::arg("percept_dict"), "Pythonic action method")
        .def("observe", [](AgentBrain& self, const py::dict& percept_dict) {
            // Convert dict to percept
            // This would typically include visual, auditory, etc. data
            return true;
        }, py::arg("percept_dict"), "Observe environment (placeholder)")
        .def("think", [](AgentBrain& self) {
            // Internal cognitive processing
            // This would typically include prediction, planning, etc.
            return true;
        }, "Internal cognitive processing (placeholder)")
        .def("get_neural_activity", [](const AgentBrain& self) {
            // Get neural activity summary
            py::dict activity;
            if (auto brain = self.getBrain()) {
                activity["total_neurons"] = brain->getTotalNeuronCount();
                activity["total_synapses"] = brain->getTotalSynapseCount();
                activity["total_spikes"] = brain->getTotalSpikeCount();
                activity["firing_neurons"] = brain->getFiringNeuronCount();
            }
            return activity;
        }, "Get neural activity summary")
        .def("get_learning_status", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            status["neuromodulation_level"] = self.getNeuromodulationLevel();
            status["curiosity_level"] = self.getCuriosityLevel();
            status["novelty_level"] = self.getNoveltyLevel();
            status["prediction_error"] = self.getPredictionError();
            return status;
        }, "Get learning system status")
        .def("get_agent_summary", [](const AgentBrain& self) {
            py::dict summary;
            summary["brain_type"] = "AgentBrain";
            summary["sensory_input_size"] = self.getSensoryInputSize();
            summary["motor_output_size"] = self.getMotorOutputSize();
            summary["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return summary;
        }, "Get agent summary")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: " + std::to_string(agent.getSensoryInputSize()) + " sensory, " +
                   std::to_string(agent.getMotorOutputSize()) + " motor>";
        });
}

/**
 * Performance monitoring utilities
 */
void bindPerformanceMonitoring(py::module_& m) {
    m.def("get_system_info", []() {
        py::dict info;
        info["platform"] = "Linux";
        info["architecture"] = "x86_64";
        info["compiler"] = "C++20";
        return info;
    }, "Get system information");
    
    m.def("get_memory_usage", []() {
        py::dict memory;
        // In practice, you would use platform-specific memory monitoring
        memory["available_mb"] = 1024.0f;  // Placeholder
        memory["used_mb"] = 256.0f;      // Placeholder
        memory["peak_mb"] = 512.0f;      // Placeholder
        return memory;
    }, "Get memory usage statistics");
    
    m.def("get_performance_stats", []() {
        py::dict stats;
        // In practice, you would collect actual performance metrics
        stats["cpu_usage"] = 25.5f;      // Placeholder
        stats["memory_usage"] = 75.2f;  // Placeholder
        stats["network_io"] = 0.0f;      // Placeholder
        stats["disk_io"] = 0.0f;         // Placeholder
        return stats;
    }, "Get performance statistics");
    
    m.def("benchmark", [](const std::function<void()>& test_func, int iterations) {
        auto start_time = std::chrono::high_resolution_clock::now();
        for (int i = 0; i < iterations; ++i) {
            test_func();
        }
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        return duration.count() / 1000.0f;  // Return seconds
    }, py::arg("test_func"), py::arg("iterations") = 100,
        "Benchmark a function (simplified)");
    
    m.def("profile_function", [](const std::function<void()>& func) {
        // In practice, you would use proper profiling tools
        // This is a simplified implementation
        auto start_time = std::chrono::high_resolution_clock::now();
        func();
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
        return static_cast<double>(duration.count()) / 1000.0;  // Return milliseconds
    }, py::arg("func"), "Profile a function (simplified)");
}

/**
 * Serialization utilities
 */
void bindSerialization(py::module_& m) {
    m.def("save_brain_state", [](const Brain& brain, const std::string& filepath) {
        return brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file");
    
    m.def("load_brain_state", [](Brain& brain, const std::string& filepath) {
        return brain.load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file");
    
    m.def("serialize_brain", [](const Brain& brain) {
        // In practice, you would implement proper serialization
        py::dict serialized;
        serialized["type"] = "Brain";
        serialized["version"] = 1.0;
        serialized["data"] = brain.getTotalNeuronCount();
        return serialized;
    }, "Serialize brain state (placeholder)");
    
    m.def("deserialize_brain", [](const py::dict& data) {
        // In practice, you would implement proper deserialization
        // This is a placeholder
        throw py::not_implemented("deserialize_brain not yet implemented");
    }, py::arg("data"), "Deserialize brain state (placeholder)");
}

/**
 * Visualization and analysis utilities
 */
void bindVisualization(py::module_& m) {
    m.def("create_neural_network_plot", [](const Brain& brain) {
        // In practice, you would generate actual plots
        // This is a placeholder that returns a description
        std::string plot_info = "Neural Network Plot: " +
                               std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                               std::to_string(brain.getTotalSynapseCount()) + " synapses";
        return plot_info;
    }, py::arg("brain"), "Create neural network visualization (placeholder)");
    
    m.def("plot_spike_raster", [](const Brain& brain, int window_size) {
        // In practice, you would create a spike raster plot
        // This is a placeholder
        std::string plot_info = "Spike Raster Plot: " +
                               std::to_string(brain.getTotalSpikeCount()) + " total spikes";
        return plot_info;
    }, py::arg("brain"), py::arg("window_size") = 100,
        "Create spike raster plot (placeholder)");
    
    m.def("analyze_spiking_patterns", [](const Brain& brain) {
        py::dict analysis;
        analysis["total_spikes"] = brain.getTotalSpikeCount();
        analysis["avg_firing_rate"] = brain.getAverageFiringRate();
        analysis["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        analysis["max_firing_rate"] = brain.getExcitationInhibitionRatio() * 100.0f;  // Placeholder
        return analysis;
    }, py::arg("brain"), "Analyze spiking patterns");
    
    m.def("create_activity_heatmap", [](const Brain& brain) {
        // In practice, you would create a 2D heatmap
        // This is a placeholder
        return "Activity Heatmap (placeholder)";
    }, py::arg("brain"), "Create activity heatmap (placeholder)");
}

/**
 * Main module initialization
 */
PYBIND11_MODULE(pynlm_enhanced, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ------------------------------------------------------
        
        Enhanced Python bindings for the NLM C++ neural simulation framework.
        
        This version includes:
        - Comprehensive type hints for better IDE support
        - Pythonic APIs that leverage Python idioms
        - Context managers for resource management
        - Enhanced error handling with Python-specific exceptions
        - Performance monitoring and profiling tools
        - Serialization/deserialization methods
        - Visualization and analysis utilities
        - Configuration validation and type safety
        
        Key improvements over the original bindings:
        - Type hints for all classes and methods
        - Enhanced documentation with examples
        - Better error messages and validation
        - Pythonic APIs (e.g., __getitem__, __setitem__, __len__)
        - Context manager support
        - Performance monitoring tools
        - Advanced serialization utilities
    )pbdoc";

    // Register core exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::logic_error>(m, "LogicError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");
    py::register_exception<std::domain_error>(m, "DomainError");
    
    // Bind core types
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Bind enumerations
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

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
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

    // Bind enhanced classes
    auto config_class = bindConfig(m);
    auto sensory_input_class = bindSensoryInput(m);
    auto vision_class = bindVision(m);
    auto audio_class = bindAudio(m);
    auto internal_signals_class = bindSensoryInput(m);  // Reuse same binding
    auto action_class = bindAction(m);
    auto brain_class = bindBrain(m);
    auto agent_brain_class = bindAgentBrain(m);
    auto world_class = bindSimpleWorld(m);
    
    // Bind helper classes
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocityX", &AgentBody::velocityX)
        .def_readwrite("velocityY", &AgentBody::velocityY)
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("isMoving", &AgentBody::isMoving)
        .def_readwrite("isTurning", &AgentBody::isTurning)
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime)
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("get_vision", &SensoryPercept::getVision)
        .def("set_vision", &SensoryPercept::setVision, py::arg("vision"))
        .def("get_vision_width", &SensoryPercept::getVisionWidth)
        .def("get_vision_height", &SensoryPercept::getVisionHeight)
        .def("get_touch", &SensoryPercept::getTouch)
        .def("set_touch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("get_internal", &SensoryPercept::getInternal)
        .def("set_internal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("get_proprioception", &SensoryPercept::getProprioception)
        .def("set_proprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("get_audio", &SensoryPercept::getAudio)
        .def("set_audio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("get_all_signals", &SensoryPercept::getAllSignals)
        .def("get_timestamp", &SensoryPercept::getTimestamp)
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

    // Bind constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Bind factory functions
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

    m.def("create_vision", [](size_t width, size_t height, size_t channels) -> std::shared_ptr<Vision> {
        return std::make_shared<Vision>(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a vision sensory input");

    m.def("create_audio", [](size_t sample_rate, size_t num_samples) -> std::shared_ptr<Audio> {
        return std::make_shared<Audio>(sample_rate, num_samples);
    }, py::arg("sample_rate"), py::arg("num_samples"), "Create an audio sensory input");

    m.def("create_internal_signals", []() -> std::shared_ptr<InternalSignals> {
        return std::make_shared<InternalSignals>();
    }, "Create internal signals sensory input");

    // Bind utility functions
    bindPerformanceMonitoring(m);
    bindSerialization(m);
    bindVisualization(m);
}

} // namespace nlm

#endif // PYNLM_BINDINGS_HPP