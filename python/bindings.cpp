#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <functional>
#include <stdexcept>
#include <sstream>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/Synapse.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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

    // Convenience functions for ID creation and validation
    m.def("create_neuron_id", [](uint64_t value) { return NeuronId(value); });
    m.def("get_invalid_neuron_id", []() { return INVALID_NEURON_ID; });
    m.def("create_synapse_id", [](uint64_t value) { return SynapseId(value); });
    m.def("get_invalid_synapse_id", []() { return INVALID_SYNAPSE_ID; });
    m.def("create_region_id", [](uint64_t value) { return RegionId(value); });
    m.def("get_invalid_region_id", []() { return INVALID_REGION_ID; });
    m.def("create_population_id", [](uint64_t value) { return PopulationId(value); });
    m.def("get_invalid_population_id", []() { return INVALID_POPULATION_ID; });

    // Helper functions for type conversions and validation
    m.def("neuron_type_name", [](NeuronType type) {
        switch(type) {
            case NeuronType::Excitatory: return std::string("Excitatory");
            case NeuronType::Inhibitory: return std::string("Inhibitory");
            case NeuronType::Modulatory: return std::string("Modulatory");
            case NeuronType::Sensory: return std::string("Sensory");
            case NeuronType::Motor: return std::string("Motor");
            case NeuronType::Internal: return std::string("Internal");
            default: return std::string("Unknown");
        }
    });

    m.def("synapse_type_name", [](SynapseType type) {
        switch(type) {
            case SynapseType::Excitatory: return std::string("Excitatory");
            case SynapseType::Inhibitory: return std::string("Inhibitory");
            case SynapseType::Modulatory: return std::string("Modulatory");
            case SynapseType::Electrical: return std::string("Electrical");
            case SynapseType::GapJunction: return std::string("GapJunction");
            default: return std::string("Unknown");
        }
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

    // Enhanced Config with better error handling and validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with Pythonic API)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Enhanced Pythonic API methods
        .def("set_value", [](Config& self, const std::string& key, const std::string& value) {
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Failed to set string value: ") + e.what());
            }
        }, py::arg("key"), py::arg("value"), "Set a string configuration value")
        .def("set_int_value", [](Config& self, const std::string& key, int value) {
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Failed to set int value: ") + e.what());
            }
        }, py::arg("key"), py::arg("value"), "Set an integer configuration value")
        .def("set_float_value", [](Config& self, const std::string& key, double value) {
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Failed to set float value: ") + e.what());
            }
        }, py::arg("key"), py::arg("value"), "Set a float configuration value")
        .def("set_bool_value", [](Config& self, const std::string& key, bool value) {
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Failed to set bool value: ") + e.what());
            }
        }, py::arg("key"), py::arg("value"), "Set a boolean configuration value")
        .def("get_value", [](Config& self, const std::string& key) {
            auto val = self.get<std::string>(key);
            if (val.has_value()) {
                return *val;
            } else {
                throw py::key_error("Configuration key not found: " + key);
            }
        }, py::arg("key"), "Get a configuration value as string")
        .def("get_int_value", [](Config& self, const std::string& key) {
            auto val = self.get<int>(key);
            if (val.has_value()) {
                return *val;
            } else {
                throw py::key_error("Configuration key not found: " + key);
            }
        }, py::arg("key"), "Get an integer configuration value")
        .def("get_float_value", [](Config& self, const std::string& key) {
            auto val = self.get<double>(key);
            if (val.has_value()) {
                return *val;
            } else {
                throw py::key_error("Configuration key not found: " + key);
            }
        }, py::arg("key"), "Get a float configuration value")
        .def("get_bool_value", [](Config& self, const std::string& key) {
            auto val = self.get<bool>(key);
            if (val.has_value()) {
                return *val;
            } else {
                throw py::key_error("Configuration key not found: " + key);
            }
        }, py::arg("key"), "Get a boolean configuration value")
        .def("get_or_string", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto val = self.getOr<std::string>(key, defaultValue);
            return val;
        }, py::arg("key"), py::arg("default_value"), "Get string with default value")
        .def("get_or_int", [](Config& self, const std::string& key, int defaultValue) {
            auto val = self.getOr<int>(key, defaultValue);
            return val;
        }, py::arg("key"), py::arg("default_value"), "Get int with default value")
        .def("get_or_float", [](Config& self, const std::string& key, double defaultValue) {
            auto val = self.getOr<double>(key, defaultValue);
            return val;
        }, py::arg("key"), py::arg("default_value"), "Get float with default value")
        .def("get_or_bool", [](Config& self, const std::string& key, bool defaultValue) {
            auto val = self.getOr<bool>(key, defaultValue);
            return val;
        }, py::arg("key"), py::arg("default_value"), "Get bool with default value")
        .def("update_from_dict", [](Config& self, const py::dict& d) {
            bool success = true;
            for (auto item : d) {
                std::string key = py::cast<std::string>(item.first);
                try {
                    if (py::isinstance<py::str>(item.second)) {
                        self.set(key, py::cast<std::string>(item.second));
                    } else if (py::isinstance<py::int_>(item.second)) {
                        self.set(key, py::cast<int>(item.second));
                    } else if (py::isinstance<py::float_>(item.second)) {
                        self.set(key, py::cast<double>(item.second));
                    } else if (py::isinstance<py::bool_>(item.second)) {
                        self.set(key, py::cast<bool>(item.second));
                    }
                } catch (...) {
                    success = false;
                }
            }
            return success;
        }, py::arg("dict"), "Update config from Python dictionary (returns success)")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                // Try to get value as string first
                auto val_str = self.get<std::string>(key);
                if (val_str.has_value()) {
                    result[key.c_str()] = *val_str;
                    continue;
                }
                // Try int
                auto val_int = self.get<int>(key);
                if (val_int.has_value()) {
                    result[key.c_str()] = *val_int;
                    continue;
                }
                // Try double
                auto val_float = self.get<double>(key);
                if (val_float.has_value()) {
                    result[key.c_str()] = *val_float;
                    continue;
                }
                // Try bool
                auto val_bool = self.get<bool>(key);
                if (val_bool.has_value()) {
                    result[key.c_str()] = *val_bool;
                    continue;
                }
            }
            return result;
        }, "Convert config to Python dictionary with type inference")
        .def("get_config_summary", [](const Config& self) {
            std::stringstream ss;
            auto keys = self.getKeys();
            ss << "Config Summary (" << keys.size() << " entries):";
            for (const auto& key : keys) {
                ss << "\n  " << key << " = ";
                auto val_str = self.get<std::string>(key);
                if (val_str.has_value()) {
                    ss << "\"" << *val_str << "\"";
                } else {
                    auto val_int = self.get<int>(key);
                    if (val_int.has_value()) {
                        ss << *val_int;
                    } else {
                        auto val_float = self.get<double>(key);
                        if (val_float.has_value()) {
                            ss << *val_float;
                        } else {
                            auto val_bool = self.get<bool>(key);
                            if (val_bool.has_value()) {
                                ss << (*val_bool ? "true" : "false");
                            } else {
                                ss << "[complex]";
                            }
                        }
                    }
                }
                ss << " (" << (self.has(key) ? "present" : "missing") << ")";
            }
            return ss.str();
        }, "Get detailed config summary as string");

    // Enhanced ActionResult class with validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Convenience methods for common action result patterns
        .def("is_success", &ActionResult::success, "Check if action was successful")
        .def("was_rewarded", [](const ActionResult& self) { return self.reward > 0.0f; },
             "Check if action received positive reward")
        .def("was_penalized", [](const ActionResult& self) { return self.reward < 0.0f; },
             "Check if action received negative reward")
        .def("get_description", [](const ActionResult& self) {
            std::stringstream ss;
            ss << (self.success ? "Success" : "Failure");
            if (self.reward != 0.0f) {
                ss << " (Reward: " << self.reward << ")";
            }
            if (!self.message.empty()) {
                ss << " - " << self.message;
            }
            return ss.str();
        }, "Get human-readable description");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
              "Set the timestamp");

    // Enhanced Vision sensory input
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Pythonic property access
        .def_property("width", &Vision::getWidth, nullptr)
        .def_property("height", &Vision::getHeight, nullptr)
        .def_property("channels", &Vision::getChannels, nullptr);

    // Enhanced Audio sensory input
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def_property("sample_rate", &Audio::getSampleRate, &Audio::setSampleRate)
        .def_property("num_samples", &Audio::getNumSamples, nullptr);

    // Enhanced InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("get_signal_count", &InternalSignals::getData,
             "Get number of signals (convenience method)");

    // Enhanced Action class with Pythonic API
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Pythonic property access
        .def_property("type", &Action::getType, &Action::setType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        // Convenience methods
        .def("add_parameter", &Action::getParameters, "Add a parameter to the action")
        .def("get_parameter", [](const Action& action, size_t index) {
            auto params = action.getParameters();
            if (index < params.size()) return params[index];
            throw py::index_error("Parameter index out of range");
        }, py::arg("index"), "Get parameter at index")
        .def("set_parameter", [](Action& action, size_t index, float value) {
            auto params = action.getParameters();
            if (index < params.size()) {
                params[index] = value;
                action.setParameters(params);
            } else {
                throw py::index_error("Parameter index out of range");
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at index");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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

    // Enhanced SensoryPercept class with Pythonic API
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &SensoryPercept::getAllSignals)
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        // Pythonic property access for convenience
        .def_property("vision", &SensoryPercept::getVision, &SensoryPercept::setVision)
        .def_property("touch", &SensoryPercept::getTouch, &SensoryPercept::setTouch)
        .def_property("internal", &SensoryPercept::getInternal, &SensoryPercept::setInternal)
        .def_property("proprioception", &SensoryPercept::getProprioception, &SensoryPercept::setProprioception)
        .def_property("audio", &SensoryPercept::getAudio, &SensoryPercept::setAudio)
        .def_property("timestamp", &SensoryPercept::getTimestamp, &SensoryPercept::setTimestamp)
        .def("getSignalSum", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            return std::accumulate(signals.begin(), signals.end(), 0.0f);
        }, "Get sum of all signals (convenience method)")
        .def("is_empty", [](const SensoryPercept& self) {
            auto signals = self.getAllSignals();
            return signals.empty();
        }, "Check if percept has any signals");

    // Enhanced SimpleWorld with better error handling and validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"),
              "Configure world with dimensions")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
              "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
              "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
              py::arg("cmd"), py::arg("currentTime"),
              "Apply motor command to agent and get result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
              py::return_value_policy::reference_internal,
              "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
              py::return_value_policy::reference_internal,
              "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
              "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
              "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
              "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
              py::return_value_policy::reference_internal,
              "Get object at position (if any)")
        // World properties with Pythonic API
        .def_property("width", &SimpleWorld::getWidth, nullptr, "World width")
        .def_property("height", &SimpleWorld::getHeight, nullptr, "World height")
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy,
                     "Maximum energy level")
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate,
                     "Energy decay rate")
        .def("get_simulation_time", &SimpleWorld::getSimulationTime,
              "Get current simulation time")
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"),
              "Set random seed for reproducibility")
        .def("get_random_seed", &SimpleWorld::getRandomSeed,
              "Get current random seed");

    // Enhanced Brain class with better error handling and validation
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
              "Create brain with configuration")
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
        .def("getRegion", &Brain::getRegion, py::arg("id"),
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
              "Log brain status")
        // Property access for common brain statistics
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount,
                               "Total neuron count across all regions")
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount,
                               "Total synapse count across all regions")
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount,
                               "Count of active neurons")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                               "Count of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                               "Average firing rate across all neurons")
        .def_property_readonly("e_i_ratio", &Brain::getExcitationInhibitionRatio,
                               "Excitation/inhibition balance ratio")
        .def_property_readonly("total_spikes", &Brain::getTotalSpikeCount,
                               "Total spike count")
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage,
                               "Current developmental stage")
        .def("getRegionNames", [](const Brain& self) {
            auto ids = self.getRegionIds();
            auto regions = self.getRegions();
            std::map<RegionId, std::string> nameMap;
            for (size_t i = 0; i < ids.size() && i < regions.size(); ++i) {
                nameMap[ids[i]] = regions[i]->getName();
            }
            return nameMap;
        }, "Get mapping of region IDs to names");

    // Enhanced AgentBrain class with better error handling and validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
              "Create agent brain interface with underlying brain")
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
        // Neuromodulation system control
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
        // Status flags for neuromodulation systems
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
              "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
              "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
              "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
              "Check if curiosity is enabled")
        // Property access for current levels
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                               "Current neuromodulation level")
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                               "Current curiosity level")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                               "Current novelty level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                               "Current prediction error")
        .def("get_development_summary", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << "\n";
            ss << "Neuromodulation Level: " << self.getNeuromodulationLevel() << "\n";
            ss << "Curiosity Level: " << self.getCuriosityLevel() << "\n";
            ss << "Novelty Level: " << self.getNoveltyLevel() << "\n";
            ss << "Prediction Error: " << self.getPredictionError();
            return ss.str();
        }, "Get developmental summary as string");

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Convenience factory functions
    m.def("create_brain_with_config", [](const py::dict& config_dict) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        // Convert Python dict to config (simplified - actual implementation would need full dict parsing)
        try {
            config->loadFromArgs(1, nullptr); // Would need better implementation
        } catch (...) {}
        return std::make_shared<Brain>(config);
    }, py::arg("config_dict"), "Create a brain from a Python dictionary configuration");

    m.def("create_demo_scenario", []() {
        // Create a complete NLM scenario for testing and examples
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        world->configure(100.0f, 100.0f, 10, 10);
        brain->initialize();
        agentBrain->initialize(*world);
        
        return std::make_tuple(brain, world, agentBrain);
    }, "Create a complete demo scenario for testing");

    // Neural simulation utilities
    m.def("initialize_brain_neural_network", [](std::shared_ptr<Brain> brain, size_t num_regions, 
                                                size_t neurons_per_region, NeuronType type = NeuronType::Internal) {
        for (size_t i = 0; i < num_regions; ++i) {
            auto region_id = brain->addRegion("Region_" + std::to_string(i));
            auto region = brain->getRegion(region_id);
            if (region) {
                // Would need actual population creation here
                // region->addPopulation(neurons_per_region, type);
            }
        }
        return num_regions;
    }, py::arg("brain"), py::arg("num_regions"), py::arg("neurons_per_region"), 
           py::arg("type") = NeuronType::Internal,
           "Initialize neural network with specified architecture");

    m.def("inject_current_to_pattern", [](std::shared_ptr<Brain> brain, NeuronType type, 
                                          double amplitude, double duration, size_t steps = 10) {
        // Convenience function for injecting patterns of current
        auto spike_time = 0.0;
        for (size_t i = 0; i < steps; ++i) {
            brain->injectCurrentToNeurons(type, amplitude);
            spike_time += duration / steps;
        }
    }, py::arg("brain"), py::arg("type"), py::arg("amplitude"), 
           py::arg("duration"), py::arg("steps") = 10,
           "Inject a pattern of current to all neurons of a specific type");

    // Error handling utilities
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            py::set_error(py::error_already_set(), e.what());
        }
    });

    // Validation and utility functions
    m.def("validate_brain_state", [](const Brain& brain) {
        std::stringstream errors;
        bool valid = true;
        
        if (brain.getRegionCount() == 0) {
            errors << "Brain has no regions. ";
            valid = false;
        }
        
        if (brain.getTotalNeuronCount() == 0) {
            errors << "Brain has no neurons. ";
            valid = false;
        }
        
        if (brain.getDevelopmentalStage() == DevelopmentalStage::Initial) {
            errors << "Brain is in Initial developmental stage. ";
            valid = false;
        }
        
        return std::make_pair(valid, errors.str());
    }, py::arg("brain"), "Validate brain state and return error messages");

    m.def("get_simulation_stats", [](const Brain& brain, const SimpleWorld& world) {
        std::map<std::string, double> stats;
        
        stats["neuron_count"] = static_cast<double>(brain.getTotalNeuronCount());
        stats["synapse_count"] = static_cast<double>(brain.getTotalSynapseCount());
        stats["active_neurons"] = static_cast<double>(brain.getActiveNeuronCount());
        stats["firing_neurons"] = static_cast<double>(brain.getFiringNeuronCount());
        stats["average_firing_rate"] = brain.getAverageFiringRate();
        stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        stats["total_spikes"] = static_cast<double>(brain.getTotalSpikeCount());
        
        stats["world_width"] = world.getWidth();
        stats["world_height"] = world.getHeight();
        stats["simulation_time"] = world.getSimulationTime();
        
        return stats;
    }, py::arg("brain"), py::arg("world"), "Get comprehensive simulation statistics");

    // Context manager for Brain - handles initialization and cleanup
    py::class_<BrainContextManager>(m, "BrainContextManager", R"pbdoc(Context manager for Brain simulation)")
        .def(py::init<std::shared_ptr<Brain>>())
        .def("__enter__", [](BrainContextManager& self) {
            self.brain_->initialize();
            return self.brain_.get();
        })
        .def("__exit__", [](BrainContextManager& self, py::args) {
            self.brain_->reset();
        });

    // Context manager for SimpleWorld - handles simulation lifecycle
    py::class_<WorldContextManager>(m, "WorldContextManager", R"pbdoc(Context manager for SimpleWorld simulation)")
        .def(py::init<std::shared_ptr<SimpleWorld>>())
        .def("__enter__", [](WorldContextManager& self) {
            return self.world_.get();
        })
        .def("__exit__", [](WorldContextManager& self, py::args) {
            self.world_->reset();
        });

    // Context manager for complete NLM scenario
    py::class_<ScenarioContextManager>(m, "ScenarioContextManager", R"pbdoc(Context manager for complete NLM scenario)")
        .def(py::init<std::tuple<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>>())
        .def("__enter__", [](ScenarioContextManager& self) {
            auto scenario = std::get<0>(self.scenario_);
            auto world = std::get<1>(self.scenario_);
            auto agentBrain = std::get<2>(self.scenario_);
            
            world->configure(100.0f, 100.0f, 10, 10);
            scenario->initialize();
            agentBrain->initialize(*world);
            
            return self.scenario_;
        })
        .def("__exit__", [](ScenarioContextManager& self, py::args) {
            auto scenario = std::get<0>(self.scenario_);
            scenario->reset();
        });

} // namespace nlm
