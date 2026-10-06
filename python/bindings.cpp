#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <chrono>

// Python exception translation
namespace nlm {
    void registerPythonExceptions(py::module_& m);
}

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Helper functions for Pythonic interface
template <typename T>
std::vector<T> toVector(const py::list& py_list) {
    std::vector<T> result;
    result.reserve(py_list.size());
    for (auto item : py_list) {
        result.push_back(item.cast<T>());
    }
    return result;
}

template <typename K, typename V>
std::unordered_map<K, V> toMap(const py::dict& py_dict) {
    std::unordered_map<K, V> result;
    for (auto item : py_dict) {
        result[item.first.cast<K>()] = item.second.cast<V>();
    }
    return result;
}

class BrainCheckpoint {
private:
    std::shared_ptr<nlm::Brain> brain;
    std::string filepath;
    bool saved;
    bool loaded;

public:
    BrainCheckpoint(std::shared_ptr<nlm::Brain> brain_ref, const std::string& filepath)
        : brain(brain_ref), filepath(filepath), saved(false), loaded(false) {}

    ~BrainCheckpoint() {
        if (saved && !loaded) {
            brain->save(filepath);
        }
    }

    void save() {
        if (brain->save(filepath)) {
            saved = true;
        } else {
            throw std::runtime_error("Failed to save brain checkpoint");
        }
    }

    void load() {
        if (brain->load(filepath)) {
            loaded = true;
        } else {
            throw std::runtime_error("Failed to load brain checkpoint");
        }
    }

    void __enter__() {
        return;
    }

    py::dict __exit__(py::object exc_type, py::object exc_val, py::object exc_traceback) {
        if (saved && !loaded) {
            brain->save(filepath);
        }
        return py::dict();
    }

    bool is_saved() const { return saved; }
    bool is_loaded() const { return loaded; }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        A Pythonic binding for the NLM C++ neural simulation framework.
        
        Key improvements:
        - Python properties (e.g., brain.region_ids instead of brain.get_region_ids())
        - Context managers (e.g., with brain.checkpoint("model.sav") as chk)
        - Automatic type conversions (Python lists → vectors, dicts → maps)
        - Pythonic API (__len__, __getitem__, iteration support)
        - Enhanced documentation with examples
        - Python exception translation for better error handling
        - Convenience functions for common operations
        
        Examples:
        >>> import pynlm
        >>> config = pynlm.create_default_config()
        >>> brain = pynlm.create_brain(config)
        >>> brain.initialize()
        >>> 
        >>> # Python property access
        >>> regions = brain.region_ids
        >>> count = len(brain)
        >>> 
        >>> # Context manager for checkpointing
        >>> with brain.checkpoint("model.sav"):
        ...     brain.step(step)
        ...     # Auto-saved on exit
        >>> 
        >>> # Type conversion
        >>> region_ids = [1, 2, 3, 4]  # Python list
        >>> neurons_per_region = brain.get_neurons_by_type("Excitatory")
    )pbdoc";

    // Register C++ exceptions as Python exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::logic_error>(m, "LogicError");

    //====================================================================
    // CORE TYPES WITH PYTHONIC INTERFACE
    //====================================================================

    // NeuronId with properties and operators
    py::class_<nlm::NeuronId>(m, "NeuronId", "Unique identifier for a neuron")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::NeuronId::value, "The neuron identifier value")
        .def("index", &nlm::NeuronId::index, "Get neuron index")
        .def("to_string", [](const nlm::NeuronId& id) { return std::to_string(id.value); },
             "Convert ID to string")
        .def(py::self == py::self, "Check equality")
        .def(py::self != py::self, "Check inequality")
        .def("__hash__", [](const nlm::NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in sets/dicts")
        .def("__repr__", [](const nlm::NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        });

    // SynapseId with properties and operators
    py::class_<nlm::SynapseId>(m, "SynapseId", "Unique identifier for a synapse")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::SynapseId::value, "The synapse identifier value")
        .def("index", &nlm::SynapseId::index, "Get synapse index")
        .def("to_string", [](const nlm::SynapseId& id) { return std::to_string(id.value); },
             "Convert ID to string")
        .def(py::self == py::self, "Check equality")
        .def(py::self != py::self, "Check inequality")
        .def("__hash__", [](const nlm::SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in sets/dicts")
        .def("__repr__", [](const nlm::SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId with properties and operators
    py::class_<nlm::RegionId>(m, "RegionId", "Unique identifier for a brain region")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::RegionId::value, "The region identifier value")
        .def("index", &nlm::RegionId::index, "Get region index")
        .def("to_string", [](const nlm::RegionId& id) { return std::to_string(id.value); },
             "Convert ID to string")
        .def(py::self == py::self, "Check equality")
        .def(py::self != py::self, "Check inequality")
        .def("__hash__", [](const nlm::RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in sets/dicts")
        .def("__repr__", [](const nlm::RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    // PopulationId with properties and operators
    py::class_<nlm::PopulationId>(m, "PopulationId", "Unique identifier for a neuron population")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &nlm::PopulationId::value, "The population identifier value")
        .def("index", &nlm::PopulationId::index, "Get population index")
        .def(py::self == py::self, "Check equality")
        .def(py::self != py::self, "Check inequality")
        .def("__hash__", [](const nlm::PopulationId& id) { return std::hash<uint64_t>{}(id.value); },
             "Get hash for use in sets/dicts")
        .def("__repr__", [](const nlm::PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // Enum types
    py::enum_<nlm::NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", nlm::NeuronType::Excitatory)
        .value("Inhibitory", nlm::NeuronType::Inhibitory)
        .value("Modulatory", nlm::NeuronType::Modulatory)
        .value("Sensory", nlm::NeuronType::Sensory)
        .value("Motor", nlm::NeuronType::Motor)
        .value("Internal", nlm::NeuronType::Internal)
        .export_values();

    py::enum_<nlm::SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", nlm::SynapseType::Excitatory)
        .value("Inhibitory", nlm::SynapseType::Inhibitory)
        .value("Modulatory", nlm::SynapseType::Modulatory)
        .value("Electrical", nlm::SynapseType::Electrical)
        .value("GapJunction", nlm::SynapseType::GapJunction)
        .export_values();

    py::enum_<nlm::DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", nlm::DevelopmentalStage::Initial)
        .value("CriticalPeriod", nlm::DevelopmentalStage::CriticalPeriod)
        .value("Maturation", nlm::DevelopmentalStage::Maturation)
        .value("Adult", nlm::DevelopmentalStage::Adult)
        .value("Aging", nlm::DevelopmentalStage::Aging)
        .export_values();

    py::enum_<nlm::FiringState>(m, "FiringState", "Neuron firing state enumeration")
        .value("Resting", nlm::FiringState::Resting)
        .value("Active", nlm::FiringState::Active)
        .value("Refractory", nlm::FiringState::Refractory)
        .value("Inhibited", nlm::FiringState::Inhibited)
        .export_values();

    py::enum_<nlm::ActionType>(m, "ActionType", "Action type enumeration")
        .value("MoveForward", nlm::ActionType::MoveForward)
        .value("MoveBackward", nlm::ActionType::MoveBackward)
        .value("MoveLeft", nlm::ActionType::MoveLeft)
        .value("MoveRight", nlm::ActionType::MoveRight)
        .value("TurnLeft", nlm::ActionType::TurnLeft)
        .value("TurnRight", nlm::ActionType::TurnRight)
        .value("Look", nlm::ActionType::Look)
        .value("LookUp", nlm::ActionType::LookUp)
        .value("LookDown", nlm::ActionType::LookDown)
        .value("Interact", nlm::ActionType::Interact)
        .value("Eat", nlm::ActionType::Eat)
        .value("Drink", nlm::ActionType::Drink)
        .value("Rest", nlm::ActionType::Rest)
        .value("Wait", nlm::ActionType::Wait)
        .value("Custom", nlm::ActionType::Custom)
        .export_values();

    py::enum_<nlm::MotorCommand>(m, "MotorCommand", "Motor command enumeration")
        .value("MoveForward", nlm::MotorCommand::MoveForward)
        .value("MoveBackward", nlm::MotorCommand::MoveBackward)
        .value("TurnLeft", nlm::MotorCommand::TurnLeft)
        .value("TurnRight", nlm::MotorCommand::TurnRight)
        .value("LookLeft", nlm::MotorCommand::LookLeft)
        .value("LookRight", nlm::MotorCommand::LookRight)
        .value("Interact", nlm::MotorCommand::Interact)
        .value("Wait", nlm::MotorCommand::Wait)
        .export_values();

    py::enum_<nlm::WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", nlm::WorldObjectType::Empty)
        .value("Resource", nlm::WorldObjectType::Resource)
        .value("Hazard", nlm::WorldObjectType::Hazard)
        .value("Wall", nlm::WorldObjectType::Wall)
        .value("Marker", nlm::WorldObjectType::Marker)
        .export_values();

    //====================================================================
    // CONFIGURATION CLASS WITH PYTHONIC INTERFACE
    //====================================================================

    py::class_<nlm::Config>(m, "Config", "Configuration class for NLM system")
        .def(py::init<>())
        .def("loadFromFile", &nlm::Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](nlm::Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", &nlm::Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file")
        .def("has", &nlm::Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &nlm::Config::getKeys,
             "Get all configuration keys")
        .def("clear", &nlm::Config::clear,
             "Clear all configuration entries")
        .def("summary", &nlm::Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const nlm::Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Pythonic properties and type-safe access
        .def_property_readonly("keys", &nlm::Config::getKeys,
                              "Get all configuration keys as a Python list")
        // Type-safe getters with defaults
        .def("get_float", [](const nlm::Config& self, const std::string& key, float default_value = 0.0f) {
            try {
                return self.get<float>(key, default_value);
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value") = 0.0f,
           "Get a float value from configuration with default")
        .def("get_int", [](const nlm::Config& self, const std::string& key, int default_value = 0) {
            try {
                return self.get<int>(key, default_value);
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get an integer value from configuration with default")
        .def("get_string", [](const nlm::Config& self, const std::string& key, const std::string& default_value = "") {
            try {
                return self.get<std::string>(key, default_value);
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value") = "",
           "Get a string value from configuration with default")
        .def("get_bool", [](const nlm::Config& self, const std::string& key, bool default_value = false) {
            try {
                return self.get<bool>(key, default_value);
            } catch (...) {
                return default_value;
            }
        }, py::arg("key"), py::arg("default_value") = false,
           "Get a boolean value from configuration with default")
        // Setters
        .def("set_float", &nlm::Config::set<float>, py::arg("key"), py::arg("value"),
             "Set a float value in configuration")
        .def("set_int", &nlm::Config::set<int>, py::arg("key"), py::arg("value"),
             "Set an integer value in configuration")
        .def("set_string", &nlm::Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set a string value in configuration")
        .def("set_bool", &nlm::Config::set<bool>, py::arg("key"), py::arg("value"),
             "Set a boolean value in configuration")
        // Dict-like access
        .def("__contains__", &nlm::Config::has,
             "Check if key exists (supports 'in' operator)")
        .def("__getitem__", [](nlm::Config& self, const std::string& key) {
            try {
                return self.get<std::string>(key);
            } catch (...) {
                throw py::key_error("Key '" + key + "' not found");
            }
        }, py::arg("key"), "Get value by key (supports dict-like access)")
        .def("__setitem__", &nlm::Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set value by key (supports dict-like access)");

    //====================================================================
    // SENSORY INPUT CLASSES
    //====================================================================

    py::class_<nlm::SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &nlm::SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &nlm::SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &nlm::SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &nlm::SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &nlm::SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<nlm::Vision, nlm::SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](nlm::Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &nlm::Vision::getWidth)
        .def("getHeight", &nlm::Vision::getHeight)
        .def("getChannels", &nlm::Vision::getChannels)
        .def_property_readonly("shape", [](const nlm::Vision& v) {
            return std::make_tuple(v.getWidth(), v.getHeight(), v.getChannels());
        }, "Get vision dimensions as (width, height, channels) tuple");

    py::class_<nlm::Audio, nlm::SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](nlm::Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &nlm::Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &nlm::Audio::getSampleRate)
        .def("getNumSamples", &nlm::Audio::getNumSamples)
        .def_property_readonly("duration", [](const nlm::Audio& a) {
            return a.getNumSamples() / static_cast<float>(a.getSampleRate());
        }, "Get audio duration in seconds");

    py::class_<nlm::InternalSignals, nlm::SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &nlm::InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &nlm::InternalSignals::clearSignals)
        .def("getSignalCount", &nlm::InternalSignals::getSignalCount,
             "Get number of internal signals")
        .def("__len__", &nlm::InternalSignals::getSignalCount)
        .def("__iter__", [](const nlm::InternalSignals& signals) {
            return py::make_iterator(signals.getSignals().begin(),
                                    signals.getSignals().end());
        }, py::return_value_policy::reference_internal);

    //====================================================================
    // MOTOR ACTION CLASS
    //====================================================================

    py::class_<nlm::Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<nlm::ActionType>(), py::arg("type"))
        .def(py::init<nlm::ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def_property("type", &nlm::Action::getType, &nlm::Action::setType,
                     "The action type")
        .def_property("parameters", &nlm::Action::getParameters, &nlm::Action::setParameters,
                     "The action parameters")
        .def("getName", &nlm::Action::getName)
        .def("clone", &nlm::Action::clone);

    //====================================================================
    // WORLD OBJECT CLASS
    //====================================================================

    py::class_<nlm::WorldObject>(m, "WorldObject", "World object representation")
        .def(py::init<>())
        .def(py::init<float, float, nlm::WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &nlm::WorldObject::x)
        .def_readwrite("y", &nlm::WorldObject::y)
        .def_readwrite("radius", &nlm::WorldObject::radius)
        .def_readwrite("type", &nlm::WorldObject::type)
        .def_readwrite("value", &nlm::WorldObject::value)
        .def_readwrite("active", &nlm::WorldObject::active)
        .def("distance_to", [](const nlm::WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate Euclidean distance to position (x, y)")
        .def("is_at", [](const nlm::WorldObject& self, float x, float y, float tolerance = 0.1f) {
            float dx = std::abs(self.x - x);
            float dy = std::abs(self.y - y);
            return (dx <= tolerance) && (dy <= tolerance);
        }, py::arg("x"), py::arg("y"), py::arg("tolerance") = 0.1f,
           "Check if object is at position (x, y) within tolerance")
        .def("move_to", [](nlm::WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"),
           "Move object to position (x, y)");

    //====================================================================
    // AGENT BODY CLASS
    //====================================================================

    py::class_<nlm::AgentBody>(m, "AgentBody", "Agent body state")
        .def(py::init<>())
        .def_readwrite("x", &nlm::AgentBody::x)
        .def_readwrite("y", &nlm::AgentBody::y)
        .def_readwrite("orientation", &nlm::AgentBody::orientation)
        .def_readwrite("velocityX", &nlm::AgentBody::velocityX)
        .def_readwrite("velocityY", &nlm::AgentBody::velocityY)
        .def_readwrite("angularVelocity", &nlm::AgentBody::angularVelocity)
        .def_readwrite("energy", &nlm::AgentBody::energy)
        .def_readwrite("health", &nlm::AgentBody::health)
        .def_readwrite("age", &nlm::AgentBody::age)
        .def_readwrite("isMoving", &nlm::AgentBody::isMoving)
        .def_readwrite("isTurning", &nlm::AgentBody::isTurning)
        .def_readwrite("lastActionTime", &nlm::AgentBody::lastActionTime)
        .def("reset", &nlm::AgentBody::reset)
        .def("speed", [](const nlm::AgentBody& self) {
            return std::sqrt(self.velocityX*self.velocityX + self.velocityY*self.velocityY);
        }, "Get current speed")
        .def("is_idle", [](const nlm::AgentBody& self) {
            return !self.isMoving && !self.isTurning;
        }, "Check if agent is idle");

    //====================================================================
    // ACTION RESULT CLASS
    //====================================================================

    py::class_<nlm::ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &nlm::ActionResult::reward)
        .def_readwrite("success", &nlm::ActionResult::success)
        .def_readwrite("message", &nlm::ActionResult::message)
        .def("is_positive", [](const nlm::ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is positive (success with reward)")
        .def("is_negative", [](const nlm::ActionResult& self) {
            return !self.success || self.reward < 0.0f;
        }, "Check if result is negative (failure or penalty)");

    //====================================================================
    // SENSORY PERCEPT CLASS
    //====================================================================

    py::class_<nlm::SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
        .def(py::init<>())
        .def("getVision", &nlm::SensoryPercept::getVision)
        .def("setVision", &nlm::SensoryPercept::setVision, py::arg("vision"))
        .def("getVisionWidth", &nlm::SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &nlm::SensoryPercept::getVisionHeight)
        .def("getTouch", &nlm::SensoryPercept::getTouch)
        .def("setTouch", &nlm::SensoryPercept::setTouch, py::arg("touch"))
        .def("getInternal", &nlm::SensoryPercept::getInternal)
        .def("setInternal", &nlm::SensoryPercept::setInternal, py::arg("internal"))
        .def("getProprioception", &nlm::SensoryPercept::getProprioception)
        .def("setProprioception", &nlm::SensoryPercept::setProprioception, py::arg("proprioception"))
        .def("getAudio", &nlm::SensoryPercept::getAudio)
        .def("setAudio", &nlm::SensoryPercept::setAudio, py::arg("audio"))
        .def("getAllSignals", &nlm::SensoryPercept::getAllSignals)
        .def("getTimestamp", &nlm::SensoryPercept::getTimestamp)
        .def("setTimestamp", &nlm::SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("is_sensory_available", [](const nlm::SensoryPercept& self) {
            return self.getVisionWidth() > 0 || self.getTouch().size() > 0;
        }, "Check if any sensory input is available");

    //====================================================================
    // SIMPLE WORLD CLASS WITH PYTHONIC INTERFACE
    //====================================================================

    py::class_<nlm::SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
        .def(py::init<>())
        .def("configure", &nlm::SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &nlm::SimpleWorld::reset)
        .def("setAgentStart", &nlm::SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &nlm::SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &nlm::SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &nlm::SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &nlm::SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &nlm::SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &nlm::SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &nlm::SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &nlm::SimpleWorld::getWidth)
        .def("getHeight", &nlm::SimpleWorld::getHeight)
        .def("getMaxEnergy", &nlm::SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &nlm::SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &nlm::SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &nlm::SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &nlm::SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &nlm::SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &nlm::SimpleWorld::getRandomSeed)
        .def("getObjectCount", &nlm::SimpleWorld::getObjectCount)
        .def("__len__", &nlm::SimpleWorld::getObjectCount)
        // Context manager support
        .def("__enter__", [](nlm::SimpleWorld& self) -> nlm::SimpleWorld& { return self; })
        .def("__exit__", [](nlm::SimpleWorld& /*self*/, py::args) {
            // Clean up world state if needed
        })
        // Convenience methods
        .def("get_objects", &nlm::SimpleWorld::getObjectCount,
             "Get number of objects in world");

    //====================================================================
    // BRAIN CLASS WITH PYTHONIC INTERFACE
    //====================================================================

    py::class_<nlm::Brain>(m, "Brain", "Central neural simulation brain class")
        .def(py::init<std::shared_ptr<nlm::Config>>(), py::arg("config"))
        .def("initialize", &nlm::Brain::initialize,
             "Initialize the brain with configuration")
        // Main simulation step methods
        .def("step", static_cast<void (nlm::Brain::*)(nlm::SimulationStep)>(&nlm::Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step_with_time", static_cast<void (nlm::Brain::*)(nlm::SimulationStep, nlm::Timestamp)>(&nlm::Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &nlm::Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &nlm::Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &nlm::Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &nlm::Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &nlm::Brain::reset,
             "Reset brain state")
        // Save/load operations
        .def("save", &nlm::Brain::save, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &nlm::Brain::load, py::arg("filepath"),
             "Load brain state from file")
        // Region management
        .def("addRegion", &nlm::Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &nlm::Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        // Python properties for common operations
        .def_property_readonly("region_ids", &nlm::Brain::getRegionIds,
                              "Get all region IDs (property)")
        .def_property_readonly("region_count", &nlm::Brain::getRegionCount,
                              "Get number of regions (property)")
        .def_property_readonly("total_neuron_count", &nlm::Brain::getTotalNeuronCount,
                              "Get total neuron count (property)")
        .def_property_readonly("total_synapse_count", &nlm::Brain::getTotalSynapseCount,
                              "Get total synapse count (property)")
        .def_property_readonly("active_neuron_count", &nlm::Brain::getActiveNeuronCount,
                              "Get active neuron count (property)")
        .def_property_readonly("firing_neuron_count", &nlm::Brain::getFiringNeuronCount,
                              "Get firing neuron count (property)")
        .def_property_readonly("average_firing_rate", &nlm::Brain::getAverageFiringRate,
                              "Get average firing rate (property)")
        .def_property_readonly("excitation_inhibition_ratio", &nlm::Brain::getExcitationInhibitionRatio,
                              "Get excitation/inhibition ratio (property)")
        .def_property_readonly("total_spike_count", &nlm::Brain::getTotalSpikeCount,
                              "Get total spike count (property)")
        // Development
        .def("get_developmental_stage", &nlm::Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("set_developmental_stage", &nlm::Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        // Configuration
        .def("get_config", &nlm::Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        // Logging
        .def("logStatus", &nlm::Brain::logStatus,
             "Log brain status")
        // Convenience methods
        .def("get_regions", &nlm::Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_neurons_by_type", [](nlm::Brain& self, nlm::NeuronType type) {
            std::vector<nlm::Neuron*> neurons;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                auto region_neurons = region->getAllNeurons();
                for (const auto& neuron : region_neurons) {
                    if (neuron->getType() == type) {
                        neurons.push_back(neuron);
                    }
                }
            }
            return neurons;
        }, py::arg("type"), "Get neurons by type")
        .def("get_synapses_by_type", [](nlm::Brain& self, nlm::SynapseType type) {
            std::vector<nlm::Synapse*> synapses;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                auto region_synapses = region->getSynapses();
                for (const auto& synapse : region_synapses) {
                    if (synapse->getType() == type) {
                        synapses.push_back(synapse);
                    }
                }
            }
            return synapses;
        }, py::arg("type"), "Get synapses by type")
        .def("most_active_regions", [](nlm::Brain& self, size_t count = 5) {
            auto regions = self.getRegions();
            std::vector<std::pair<float, const nlm::NeuralRegion*>> active_regions;
            
            for (const auto& region : regions) {
                float activity = region->getAverageFiringRate();
                active_regions.push_back({activity, region.get()});
            }
            
            std::partial_sort(active_regions.begin(), active_regions.begin() + count, active_regions.end(),
                            [](const auto& a, const auto& b) { return a.first > b.first; });
            
            std::vector<nlm::RegionId> result;
            result.reserve(count);
            for (size_t i = 0; i < count && i < active_regions.size(); ++i) {
                // Note: Need to get region ID from region - this would require access to region management
                // For now, return empty list as placeholder
                result.push_back(nlm::RegionId());
            }
            return result;
        }, py::arg("count") = 5, "Get most active regions")
        .def("__len__", &nlm::Brain::getRegionCount,
             "Get number of regions (supports len())")
        // Context manager for checkpointing
        .def("checkpoint", [](std::shared_ptr<nlm::Brain> brain, const std::string& filepath) {
            return BrainCheckpoint(brain, filepath);
        }, py::arg("filepath"),
           "Create checkpoint context manager for saving/restoring brain state");

    //====================================================================
    // AGENT BRAIN CLASS WITH PYTHONIC INTERFACE
    //====================================================================

    py::class_<nlm::AgentBrain>(m, "AgentBrain", "Agent brain interface")
        .def(py::init<std::shared_ptr<nlm::Brain>>(), py::arg("brain"))
        .def("initialize", &nlm::AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &nlm::AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &nlm::AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &nlm::AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &nlm::AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &nlm::AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &nlm::AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentStage", &nlm::AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &nlm::AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &nlm::AgentBrain::getCuriosityLevel,
             "Get curiosity level")
        .def("getNoveltyLevel", &nlm::AgentBrain::getNoveltyLevel,
             "Get novelty level")
        .def("getPredictionError", &nlm::AgentBrain::getPredictionError,
             "Get prediction error")
        .def("reset", &nlm::AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &nlm::AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &nlm::AgentBrain::enableRewardModulation,
             py::arg("enable"))
        .def("enableStructuralPlasticity", &nlm::AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &nlm::AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &nlm::AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &nlm::AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &nlm::AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &nlm::AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &nlm::AgentBrain::isCuriosityEnabled)
        // Context manager support
        .def("__enter__", [](nlm::AgentBrain& self) -> nlm::AgentBrain& { return self; })
        .def("__exit__", [](nlm::AgentBrain& /*self*/, py::args) {
            // Clean up agent state if needed
        });

    //====================================================================
    // FACTORY FUNCTIONS AND UTILITIES
    //====================================================================

    // Factory functions
    m.def("create_default_config", []() -> std::shared_ptr<nlm::Config> {
        return std::make_shared<nlm::Config>();
    }, "Create a default configuration");

    m.def("create_brain", [](std::shared_ptr<nlm::Config> config) -> std::shared_ptr<nlm::Brain> {
        return std::make_shared<nlm::Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<nlm::SimpleWorld> {
        return std::make_shared<nlm::SimpleWorld>();
    }, "Create a new simple world");

    m.def("create_agent_brain", [](std::shared_ptr<nlm::Brain> brain) -> std::shared_ptr<nlm::AgentBrain> {
        return std::make_shared<nlm::AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(nlm::INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(nlm::INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(nlm::INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(nlm::INVALID_POPULATION_ID);

    // Convenience utility functions
    m.def("simulate_step", [](nlm::Brain& brain, nlm::SimulationStep step, nlm::Timestamp time = 0.0f) {
        brain.step(step, time);
    }, py::arg("brain"), py::arg("step"), py::arg("time") = 0.0f,
       "Convenience function to perform a single simulation step");

    m.def("create_agent_episode", [](nlm::AgentBrain& agent, float duration = 1.0f) {
        agent.reset();
        return "Episode completed (placeholder)";
    }, py::arg("agent"), py::arg("duration") = 1.0f,
       "Create a simple agent episode");

    // Example usage function
    m.def("example_usage", []() {
        py::print("NLM Python Bindings - Enhanced Example Usage:");
        py::print("==============================================");
        py::print("");
        py::print("1. Basic usage with Pythonic properties:")
        py::print("   config = pynlm.create_default_config()");
        py::print("   config.set_float('brain.neuron_count', 1000)");
        py::print("   brain = pynlm.create_brain(config)");
        py::print("   brain.initialize()");
        py::print("   ");
        py::print("   # Use properties instead of get_() methods:")
        py::print("   regions = brain.region_ids  # property, not brain.get_region_ids()");
        py::print("   neurons = brain.total_neuron_count  # property, not brain.get_total_neuron_count()");
        py::print("   count = len(brain)  # Pythonic __len__ support");
        py::print("   ");
        py::print("2. Context manager for checkpointing:")
        py::print("   with brain.checkpoint('model.sav') as chk:")
        py::print("       brain.step(i)");
        py::print("       # Auto-saved on exit");
        py::print("   ");
        py::print("3. Type conversion helpers:");
        py::print("   region_list = [1, 2, 3, 4]  # Python list");
        py::print("   neurons_list = brain.get_neurons_by_type('Excitatory')  # Returns vector<Neuron*>");
        py::print("   ");
        py::print("4. Error handling with Python exceptions:");
        py::print("   try:")
        py::print("       brain.getRegion(invalid_id)");
        py::print("   except pynlm.ValueError as e:");
        py::print("       print(f'Error: {e}')");
        py::print("");
        py::print("For more examples, visit: https://github.com/your-repo/nlm-python");
        return "Example usage printed to console";
    }, "Print enhanced example usage to console");

    //====================================================================
    // ERROR HANDLING AND VALIDATION
    //====================================================================

    // Advanced exception handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::out_of_range& e) {
            PyErr_SetString(PyExc_IndexError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });

    // Validation helper functions
    m.def("validate_config", [](const nlm::Config& config) {
        std::vector<std::string> errors;
        
        if (config.has("brain.neuron_count") && config.getOr("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        if (config.has("brain.synapse_density") && 
            (config.getOr("brain.synapse_density", 0.0) < 0.0 || 
             config.getOr("brain.synapse_density", 0.0) > 1.0)) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
        
        if (!errors.empty()) {
            throw std::invalid_argument("Configuration validation failed:\n" + 
                std::accumulate(errors.begin(), errors.end(), std::string(),
                               [](const std::string& a, const std::string& b) {
                                   return a + "\n" + b;
                               }));
        }
        
        return "Configuration is valid";
    }, py::arg("config"), "Validate configuration and throw ValueError if invalid");

    m.def("get_config_errors", [](const nlm::Config& config) {
        std::vector<std::string> errors;
        
        if (config.has("brain.neuron_count") && config.getOr("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be positive");
        }
        
        if (config.has("brain.synapse_density") && 
            (config.getOr("brain.synapse_density", 0.0) < 0.0 || 
             config.getOr("brain.synapse_density", 0.0) > 1.0)) {
            errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
        }
        
        return errors;
    }, py::arg("config"), "Get validation errors for configuration");

    // Safe operation helpers
    m.def("safe_brain_operation", [](std::shared_ptr<nlm::Brain> brain, 
                                   std::function<void(std::shared_ptr<nlm::Brain>)> operation) {
        try {
            if (!brain) {
                throw std::invalid_argument("Brain pointer is null");
            }
            operation(brain);
            return "Operation completed successfully";
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Operation failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("operation"), 
       "Safely execute a brain operation with error handling");

    //====================================================================
    // TYPE CONVERSIONS
    //====================================================================

    m.def("to_vector", &toVector<float>, py::arg("py_list"),
          "Convert Python list to C++ vector (float)");

    m.def("to_map", &toMap<std::string, float>, py::arg("py_dict"),
          "Convert Python dict to C++ map (string -> float)");

    m.def("get_dict_value", &getDictValue<float>, py::arg("py_dict"), py::arg("key"), py::arg("default_value") = 0.0f,
          "Get value from Python dict with default (float)");

    //====================================================================
    // ADVANCED FEATURES
    //====================================================================

    // Smart brain with enhanced Python features
    m.def("create_smart_brain", [](std::shared_ptr<nlm::Config> config) {
        auto brain = std::make_shared<nlm::Brain>(config);
        
        class SmartBrain {
        public:
            std::shared_ptr<nlm::Brain> brain_;
            
            SmartBrain(std::shared_ptr<nlm::Brain> brain) : brain_(brain) {}
            
            // Context manager support
            SmartBrain& __enter__() {
                if (!brain_->initialize()) {
                    throw std::runtime_error("Failed to initialize brain");
                }
                return *this;
            }
            
            void __exit__(int /*exception_code*/, const char* /*exception_message*/, const char* /*traceback_message*/) {
                if (brain_) {
                    brain_->reset();
                }
            }
            
            // Property-like access
            int get_neuron_count() const { return brain_->getTotalNeuronCount(); }
            int get_synapse_count() const { return brain_->getTotalSynapseCount(); }
            float get_firing_rate() const { return brain_->getAverageFiringRate(); }
            
            // Enhanced simulation
            std::string run_simulation(int steps) {
                if (steps < 0) {
                    throw std::invalid_argument("Number of steps cannot be negative");
                }
                
                try {
                    for (int i = 0; i < steps; ++i) {
                        brain_->step(i);
                    }
                    return std::to_string(steps) + " simulation steps completed";
                } catch (const std::exception& e) {
                    throw std::runtime_error(std::string("Simulation failed: ") + e.what());
                }
            }
            
            // Dictionary-like state access
            std::map<std::string, py::object> get_state_dict() {
                std::map<std::string, py::object> state;
                state["neuron_count"] = py::cast(brain_->getTotalNeuronCount());
                state["synapse_count"] = py::cast(brain_->getTotalSynapseCount());
                state["firing_count"] = py::cast(brain_->getFiringNeuronCount());
                state["average_firing_rate"] = py::cast(brain_->getAverageFiringRate());
                state["total_spikes"] = py::cast(brain_->getTotalSpikeCount());
                return state;
            }
            
            // Enhanced string representation
            std::string to_string() const {
                std::stringstream ss;
                ss << "SmartBrain:" << std::endl;
                ss << "  Neuron count: " << brain_->getTotalNeuronCount() << std::endl;
                ss << "  Synapse count: " << brain_->getTotalSynapseCount() << std::endl;
                ss << "  Firing count: " << brain_->getFiringNeuronCount() << std::endl;
                ss << "  Average firing rate: " << brain_->getAverageFiringRate() << std::endl;
                ss << "  Total spikes: " << brain_->getTotalSpikeCount() << std::endl;
                return ss.str();
            }
        };
        
        return std::make_shared<SmartBrain>(brain);
    }, py::arg("config"), "Create a smart brain with enhanced Python features");

    // World object factory
    m.def("create_world_object", [](float x, float y, nlm::WorldObjectType type, 
                                   float value = 0.0f, float radius = 0.5f) {
        return nlm::WorldObject(x, y, type, value, radius);
    }, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f, py::arg("radius") = 0.5f,
       "Create a world object with specified properties");
}

} // namespace nlm
