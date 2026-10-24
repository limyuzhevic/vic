#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/iostream.h>
#include <memory>
#include <string>
#include <vector>
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

namespace py = pybind11;
namespace nlm {

// Enhanced exception handling for Python
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

void throw_if_error(bool condition, const std::string& msg) {
    if (!condition) {
        throw NLMException(msg);
    }
}

// Python context manager for Brain
class BrainWrapper {
public:
    std::shared_ptr<Brain> brain;
    
    explicit BrainWrapper(std::shared_ptr<Config> config) : brain(std::make_shared<Brain>(config)) {}
    explicit BrainWrapper(std::shared_ptr<Brain> b) : brain(b) {}
    
    void __enter__() {
        if (!brain->initialize()) {
            throw NLMException("Failed to initialize brain in context manager");
        }
    }
    
    void __exit__(py::object type, py::object value, py::object traceback) {
        brain->reset();
    }
    
    // Convenience methods
    std::shared_ptr<Brain> getBrain() const { return brain; }
    uint64_t getTotalNeuronCount() const { return brain->getTotalNeuronCount(); }
    uint64_t getTotalSynapseCount() const { return brain->getTotalSynapseCount(); }
    uint64_t getFiringNeuronCount() const { return brain->getFiringNeuronCount(); }
    double getAverageFiringRate() const { return brain->getAverageFiringRate(); }
    double getExcitationInhibitionRatio() const { return brain->getExcitationInhibitionRatio(); }
    uint64_t getTotalSpikeCount() const { return brain->getTotalSpikeCount(); }
    std::string getDevelopmentalStageName() const {
        switch (brain->getDevelopmentalStage()) {
            case DevelopmentalStage::Initial: return "Initial";
            case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
            case DevelopmentalStage::Maturation: return "Maturation";
            case DevelopmentalStage::Adult: return "Adult";
            case DevelopmentalStage::Aging: return "Aging";
            default: return "Unknown";
        }
    }
    
    // Batch operations for performance
    void batchStep(size_t numSteps) {
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(step, step * 0.001);
        }
    }
    
    // Configuration convenience methods
    void configure(const std::string& key, const std::string& value) {
        brain->getConfig()->set(key, value);
    }
    
    void configure(const std::string& key, int64_t value) {
        brain->getConfig()->set(key, value);
    }
    
    void configure(const std::string& key, double value) {
        brain->getConfig()->set(key, value);
    }
    
    void configure(const std::string& key, bool value) {
        brain->getConfig()->set(key, value);
    }
    
    bool hasConfig(const std::string& key) const {
        return brain->getConfig()->has(key);
    }
    
    py::dict getConfigSummary() const {
        auto config = brain->getConfig();
        py::dict summary;
        auto keys = config->getKeys();
        for (const auto& key : keys) {
            summary[key.c_str()] = config->summary();
        }
        return summary;
    }
};

// Enhanced Config wrapper for better Python interface
class ConfigWrapper {
public:
    std::shared_ptr<Config> config;
    
    ConfigWrapper() : config(std::make_shared<Config>()) {}
    
    // Load configuration from JSON string
    bool loadFromJSON(const std::string& jsonStr) {
        // For now, just parse basic key=value format
        // A full JSON parser would be more robust
        try {
            // Simple key=value parsing for basic JSON-like format
            size_t pos = 0;
            while (pos < jsonStr.size()) {
                size_t equalPos = jsonStr.find('=', pos);
                if (equalPos == std::string::npos) break;
                
                size_t nextNewline = jsonStr.find('\n', equalPos);
                if (nextNewline == std::string::npos) {
                    nextNewline = jsonStr.size();
                }
                
                std::string key = jsonStr.substr(pos, equalPos - pos);
                std::string value = jsonStr.substr(equalPos + 1, nextNewline - equalPos - 1);
                
                // Remove whitespace
                key.erase(0, key.find_first_not_of(" \t\r\n"));
                key.erase(key.find_last_not_of(" \t\r\n") + 1);
                value.erase(0, value.find_first_not_of(" \t\r\n"));
                value.erase(value.find_last_not_of(" \t\r\n") + 1);
                
                // Try to parse value as int, float, bool, or string
                try {
                    size_t endPos;
                    int intVal = std::stoi(value, &endPos);
                    if (endPos == value.size()) {
                        config->set(key, intVal);
                    } else {
                        float floatVal = std::stof(value, &endPos);
                        if (endPos == value.size()) {
                            config->set(key, floatVal);
                        } else {
                            bool boolVal;
                            if (value == "true" || value == "1") {
                                boolVal = true;
                            } else if (value == "false" || value == "0") {
                                boolVal = false;
                            } else {
                                config->set(key, value);
                                continue;
                            }
                            config->set(key, boolVal);
                        }
                    }
                } catch (const std::exception&) {
                    config->set(key, value);
                }
                
                pos = nextNewline + 1;
            }
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    std::string toJSON() const {
        std::stringstream ss;
        ss << "{\n";
        auto keys = config->getKeys();
        for (size_t i = 0; i < keys.size(); ++i) {
            ss << "  \"" << keys[i] << "\": \"";
            
            // Try to get value
            auto val = config->get<std::string>(keys[i]);
            if (val) {
                ss << *val << "\"";
            } else {
                auto intVal = config->get<int>(keys[i]);
                if (intVal) {
                    ss << *intVal << "\"";
                } else {
                    auto floatVal = config->get<double>(keys[i]);
                    if (floatVal) {
                        ss << *floatVal << "\"";
                    } else {
                        auto boolVal = config->get<bool>(keys[i]);
                        if (boolVal) {
                            ss << (*boolVal ? "true" : "false") << "\"";
                        } else {
                            ss << "null\"";
                        }
                    }
                }
            }
            
            if (i < keys.size() - 1) {
                ss << ",\n";
            } else {
                ss << "\n";
            }
        }
        ss << "}\n";
        return ss.str();
    }
    
    // Convenience setters with type inference
    template<typename T>
    void set(const std::string& key, T value) {
        config->set(key, value);
    }
    
    // Get value with type safety
    template<typename T>
    py::object get(const std::string& key) const {
        auto val = config->get<T>(key);
        if (val) {
            return py::cast(*val);
        }
        return py::none();
    }
    
    // Get with default
    template<typename T>
    T getOr(const std::string& key, T defaultValue) const {
        return config->getOr(key, defaultValue);
    }
    
    bool has(const std::string& key) const {
        return config->has(key);
    }
    
    std::vector<std::string> getKeys() const {
        return config->getKeys();
    }
    
    void clear() {
        config->clear();
    }
    
    std::string summary() const {
        return config->summary();
    }
    
    bool saveToFile(const std::string& filepath) const {
        return config->saveToFile(filepath);
    }
    
    bool loadFromFile(const std::string& filepath) {
        return config->loadFromFile(filepath);
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An enhanced Python interface for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action
        with improved error handling and Pythonic interface.
    )pbdoc";

    // Register enhanced exception types
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Enhanced NeuronId with better representation
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
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });

    // Enhanced SynapseId
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
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });

    // Enhanced RegionId
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
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });

    // Enhanced PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .doc() = "Types of neurons in the NLM system";

    // Enhanced SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .doc() = "Types of synapses in the NLM system";

    // Enhanced DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .doc() = "Developmental stages of the brain";

    // Enhanced FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .doc() = "States of neuron firing";

    // Enhanced ActionType enum
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
        .export_values()
        .doc() = "Types of actions an agent can take";

    // Enhanced MotorCommand enum
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .doc() = "Low-level motor commands";

    // Enhanced WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .doc() = "Types of objects in the world";

    // Enhanced Config class
    py::class_<ConfigWrapper>(m, "Config", R"pbdoc(Enhanced configuration interface for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &ConfigWrapper::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON format)")
        .def("loadFromJSON", &ConfigWrapper::loadFromJSON, py::arg("jsonStr"),
             "Load configuration from JSON string")
        .def("saveToFile", &ConfigWrapper::saveToFile, py::arg("filepath"),
             "Save configuration to a file (JSON format)")
        .def("has", &ConfigWrapper::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", &ConfigWrapper::getKeys,
             "Get all configuration keys")
        .def("clear", &ConfigWrapper::clear,
             "Clear all configuration entries")
        .def("toJSON", &ConfigWrapper::toJSON,
             "Export configuration to JSON string")
        .def("summary", &ConfigWrapper::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const ConfigWrapper& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Convenience methods
        .def("set", py::overload_cast<const std::string&, const std::string&>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value as string")
        .def("set", py::overload_cast<const std::string&, int64_t>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value as integer")
        .def("set", py::overload_cast<const std::string&, double>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value as float")
        .def("set", py::overload_cast<const std::string&, bool>(&ConfigWrapper::set),
             py::arg("key"), py::arg("value"),
             "Set configuration value as boolean")
        .def("get", py::overload_cast<const std::string&>(&ConfigWrapper::get),
             py::arg("key"),
             "Get configuration value as Python object")
        .def("getOr", py::overload_cast<const std::string&, int64_t>(&ConfigWrapper::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with integer default")
        .def("getOr", py::overload_cast<const std::string&, double>(&ConfigWrapper::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with float default")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&ConfigWrapper::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with string default")
        .def("getOr", py::overload_cast<const std::string&, bool>(&ConfigWrapper::getOr),
             py::arg("key"), py::arg("defaultValue"),
             "Get configuration value with boolean default");

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision class
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
        .def("getShape", [](Vision& self) {
            return std::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get tensor shape as (width, height, channels)");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getSignalCount)
        .def("getSignal", [](InternalSignals& self, size_t index) {
            return self.getSignal(index);
        }, py::arg("index"), "Get signal at index");

    // Enhanced Action class
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
        .def("toDict", [](const Action& action) {
            py::dict result;
            result["type"] = action.getName();
            result["parameters"] = action.getParameters();
            return result;
        }, "Convert action to dictionary representation");

    // Enhanced WorldObject class
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
        .def_readwrite("active", &WorldObject::active)
        .def("distanceTo", [](const WorldObject& obj, float x, float y) {
            return std::sqrt(std::pow(obj.x - x, 2) + std::pow(obj.y - y, 2));
        }, py::arg("x"), py::arg("y"), "Calculate distance to position");

    // Enhanced AgentBody class
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
        .def("reset", &AgentBody::reset)
        .def("move", [](AgentBody& body, float dx, float dy, float dt) {
            body.x += dx;
            body.y += dy;
            body.velocityX = dx / dt;
            body.velocityY = dy / dt;
            body.isMoving = (dx != 0 || dy != 0);
        }, py::arg("dx"), py::arg("dy"), py::arg("dt"), "Move the agent by dx, dy over dt time");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccess", [](const ActionResult& result) {
            return result.success;
        }, "Check if action was successful");

    // Enhanced SensoryPercept class
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
        .def("addSignal", [](SensoryPercept& percept, const std::string& type, float value) {
            if (type == "vision") {
                auto vision = percept.getVision();
                vision.addSignal(value);
                percept.setVision(vision);
            } else if (type == "internal") {
                auto internal = percept.getInternal();
                internal.addSignal(value);
                percept.setInternal(internal);
            }
        }, py::arg("type"), py::arg("value"), "Add signal to percept");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get agent's sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectCount", [](SimpleWorld& world) {
            return world.getObjectCount();
        }, "Get number of objects in world")
        .def("getObjects", [](SimpleWorld& world) {
            return world.getObjects();
        }, "Get all objects in world (reference)");

    // Enhanced Brain class with context manager support
    py::class_<BrainWrapper>(m, "Brain", R"pbdoc(Enhanced brain interface with context manager support)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create new brain with configuration")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create brain wrapper from existing brain")
        // Context manager support
        .def("__enter__", &BrainWrapper::__enter__)
        .def("__exit__", &BrainWrapper::__exit__, py::arg("type"), py::arg("value"), py::arg("traceback"))
        // Core brain operations
        .def("initialize", &BrainWrapper::getBrain, "Initialize the brain")
        .def("step", [](BrainWrapper& brain, SimulationStep step) {
            brain.brain->step(step, step * 0.001);
        }, py::arg("step"), "Perform simulation step")
        .def("stepWithTime", [](BrainWrapper& brain, SimulationStep step, Timestamp time) {
            brain.brain->step(step, time);
        }, py::arg("step"), py::arg("time"), "Perform simulation step with timestamp")
        .def("receiveSensoryInput", &BrainWrapper::getBrain, py::arg("input"),
             "Inject sensory input into the brain")
        .def("injectCurrent", &BrainWrapper::getBrain, py::arg("neuron"), py::arg("current"),
             "Inject current into specific neuron")
        .def("injectCurrentToNeurons", &BrainWrapper::getBrain, py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &BrainWrapper::getBrain,
             "Produce motor action based on neural activity")
        .def("reset", &BrainWrapper::getBrain, "Reset brain state")
        .def("save", &BrainWrapper::getBrain, py::arg("filepath"),
             "Save brain state to file")
        .def("load", &BrainWrapper::getBrain, py::arg("filepath"),
             "Load brain state from file")
        .def("addRegion", &BrainWrapper::getBrain, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &BrainWrapper::getBrain, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID")
        .def("getRegionCount", &BrainWrapper::getBrain,
             "Get the number of regions")
        .def("getRegionIds", &BrainWrapper::getBrain,
             "Get all region IDs")
        // Statistics and monitoring
        .def("getTotalNeuronCount", &BrainWrapper::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &BrainWrapper::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &BrainWrapper::getBrain,
             "Get count of active neurons")
        .def("getFiringNeuronCount", &BrainWrapper::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &BrainWrapper::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &BrainWrapper::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &BrainWrapper::getTotalSpikeCount,
             "Get total spike count")
        .def("getDevelopmentalStage", &BrainWrapper::getBrain,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &BrainWrapper::getBrain, py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &BrainWrapper::getBrain,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &BrainWrapper::getBrain, "Log brain status")
        .def("isInitialized", [](const BrainWrapper& brain) {
            return brain.brain->getTotalNeuronCount() > 0;
        }, "Check if brain is initialized")
        // Batch operations for performance
        .def("batchStep", &BrainWrapper::batchStep, py::arg("numSteps"),
             "Perform multiple simulation steps at once")
        // Configuration convenience methods
        .def("configure", py::overload_cast<const std::string&, const std::string&>(&BrainWrapper::configure),
             py::arg("key"), py::arg("value"),
             "Set configuration value as string")
        .def("configure", py::overload_cast<const std::string&, int64_t>(&BrainWrapper::configure),
             py::arg("key"), py::arg("value"),
             "Set configuration value as integer")
        .def("configure", py::overload_cast<const std::string&, double>(&BrainWrapper::configure),
             py::arg("key"), py::arg("value"),
             "Set configuration value as float")
        .def("configure", py::overload_cast<const std::string&, bool>(&BrainWrapper::configure),
             py::arg("key"), py::arg("value"),
             "Set configuration value as boolean")
        .def("hasConfig", &BrainWrapper::hasConfig, py::arg("key"),
             "Check if configuration key exists")
        .def("getConfigSummary", &BrainWrapper::getConfigSummary,
             "Get configuration as dictionary");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create a new agent brain interface")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        // Subsystem control
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<ConfigWrapper> {
        return std::make_shared<ConfigWrapper>();
    }, "Create a default configuration wrapper");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<BrainWrapper> {
        return std::make_shared<BrainWrapper>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("createBrainFromExisting", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<BrainWrapper> {
        return std::make_shared<BrainWrapper>(brain);
    }, py::arg("brain"), "Create brain wrapper from existing brain");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Type aliases for convenience
    m.def("NeuronId", &NeuronId);
    m.def("SynapseId", &SynapseId);
    m.def("RegionId", &RegionId);
    m.def("PopulationId", &PopulationId);

    // Example usage functions
    m.def("example_basic_usage", []() {
        py::object config = m.attr("createDefaultConfig")();
        py::object brain = m.attr("createBrain")(config);
        brain.attr("__enter__")();
        
        py::object world = m.attr("createSimpleWorld")();
        world.attr("configure")(10, 10, 8, 8);
        world.attr("reset")();
        
        py::object agent = m.attr("createAgentBrain")(brain);
        agent.attr("initialize")(world);
        
        for (int step = 0; step < 100; ++step) {
            world.attr("update")(0.1);
            py::object percept = world.attr("getSensoryPercept")();
            agent.attr("processSensoryInput")(percept);
            brain.attr("step")(step);
            py::object action = agent.attr("decodeMotorCommand")();
            world.attr("applyMotorCommand")(action, world.attr("getSimulationTime")());
        }
        
        brain.attr("__exit__")();
        
        return std::make_tuple(brain.attr("getTotalNeuronCount")(), 
                              brain.attr("getTotalSpikeCount")());
    }, "Example of basic usage");

    m.def("example_learning_scenario", []() {
        py::object config = m.attr("createDefaultConfig")();
        config.attr("set")("neuron_count", 500);
        config.attr("set")("connection_probability", 0.2);
        
        py::object brain = m.attr("createBrain")(config);
        py::object world = m.attr("createSimpleWorld")();
        world.attr("configure")(15, 15, 10, 10);
        
        py::object agent = m.attr("createAgentBrain")(brain);
        agent.attr("initialize")(world);
        agent.attr("enableRewardModulation")(true);
        agent.attr("enableCuriosity")(true);
        
        // Run simulation
        for (int step = 0; step < 500; ++step) {
            world.attr("update")(0.1);
            py::object percept = world.attr("getSensoryPercept")();
            agent.attr("processSensoryInput")(percept);
            brain.attr("step")(step);
            py::object action = agent.attr("decodeMotorCommand")();
            world.attr("applyMotorCommand")(action, world.attr("getSimulationTime")());
            
            // Apply reward
            double reward = percept.attr("getInternal")().attr("__getitem__")("[0]");
            agent.attr("applyRewardModulation")(reward, 0.0);
        }
        
        return std::make_tuple(
            brain.attr("getTotalSpikeCount")(),
            agent.attr("getCuriosityLevel")(),
            agent.attr("getNoveltyLevel")()
        );
    }, "Example of learning scenario");

    // Utility functions
    m.def("createVision", [](size_t width, size_t height, size_t channels) {
        return Vision(width, height, channels);
    }, py::arg("width") = 8, py::arg("height") = 8, py::arg("channels") = 3,
       "Create vision sensory input");

    m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
        return Audio(sampleRate, numSamples);
    }, py::arg("sampleRate") = 44100, py::arg("numSamples") = 1024,
       "Create audio sensory input");

    m.def("createAction", py::overload_cast<ActionType>(&Action::Action), py::arg("type"),
          "Create action from type");

    m.def("createAction", py::overload_cast<ActionType, std::vector<float>>(&Action::Action),
          py::arg("type"), py::arg("parameters"),
          "Create action from type and parameters");

    m.def("getNeuronTypes", []() {
        py::list types;
        for (const auto& type : {NeuronType::Excitatory, NeuronType::Inhibitory,
                                 NeuronType::Modulatory, NeuronType::Sensory,
                                 NeuronType::Motor, NeuronType::Internal}) {
            types.append(type);
        }
        return types;
    }, "Get all neuron types");

    m.def("getActionTypes", []() {
        py::list types;
        for (const auto& type : {ActionType::MoveForward, ActionType::MoveBackward,
                                 ActionType::TurnLeft, ActionType::TurnRight,
                                 ActionType::Look, ActionType::LookUp, ActionType::LookDown,
                                 ActionType::Interact, ActionType::Eat, ActionType::Drink,
                                 ActionType::Rest, ActionType::Wait, ActionType::Custom}) {
            types.append(type);
        }
        return types;
    }, "Get all action types");

    // Performance optimization helpers
    m.def("benchmark", [](py::object brain, size_t numSteps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (size_t step = 0; step < numSteps; ++step) {
            brain.attr("step")(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return duration.count();
    }, py::arg("brain"), py::arg("numSteps"),
       "Benchmark brain performance");

    // Validation functions
    m.def("validateConfiguration", [](const ConfigWrapper& config) {
        auto keys = config.getKeys();
        py::dict errors;
        
        if (config.has("neuron_count")) {
            auto neuronCount = config.getOr("neuron_count", 1);
            if (neuronCount <= 0) {
                errors["neuron_count"] = "Must be positive";
            }
        }
        
        if (config.has("region_count")) {
            auto regionCount = config.getOr("region_count", 1);
            if (regionCount <= 0) {
                errors["region_count"] = "Must be positive";
            }
        }
        
        if (config.has("connection_probability")) {
            auto prob = config.getOr("connection_probability", 0.1);
            if (prob < 0.0 || prob > 1.0) {
                errors["connection_probability"] = "Must be between 0.0 and 1.0";
            }
        }
        
        return errors;
    }, "Validate configuration and return errors");

    // Serialization utilities
    m.def("serializeBrain", [](py::object brain) {
        auto brainWrapper = brain.cast<std::shared_ptr<BrainWrapper>>();
        std::stringstream ss;
        // In a real implementation, this would serialize to binary format
        ss << "Brain serialization placeholder\n";
        ss << "Neurons: " << brainWrapper->getTotalNeuronCount() << "\n";
        ss << "Spikes: " << brainWrapper->getTotalSpikeCount() << "\n";
        return ss.str();
    }, "Serialize brain state to string");

    m.def("deserializeBrain", [](const std::string& data, py::object config) {
        // In a real implementation, this would deserialize from binary format
        auto brainWrapper = std::make_shared<BrainWrapper>(
            config.cast<std::shared_ptr<Config>>()
        );
        return brainWrapper;
    }, py::arg("data"), py::arg("config"),
       "Deserialize brain state from string");

    // Progress tracking
    m.def("runWithProgress", [](py::object brain, size_t totalSteps,
                                std::function<bool(size_t)> callback) {
        for (size_t step = 0; step < totalSteps; ++step) {
            brain.attr("step")(step);
            if (callback(step)) {
                break;  // Early termination if callback returns true
            }
        }
        return brain.attr("getTotalSpikeCount")();
    }, py::arg("brain"), py::arg("totalSteps"), py::arg("callback"),
       "Run simulation with progress callback");

    // Comparison functions
    m.def("compareBrains", [](py::object brain1, py::object brain2) {
        auto wrapper1 = brain1.cast<std::shared_ptr<BrainWrapper>>();
        auto wrapper2 = brain2.cast<std::shared_ptr<BrainWrapper>>();
        
        py::dict comparison;
        comparison["neuron_count_match"] = (wrapper1->getTotalNeuronCount() == wrapper2->getTotalNeuronCount());
        comparison["synapse_count_match"] = (wrapper1->getTotalSynapseCount() == wrapper2->getTotalSynapseCount());
        comparison["firing_rate_ratio"] = wrapper1->getAverageFiringRate() / wrapper2->getAverageFiringRate();
        
        return comparison;
    }, "Compare two brains");

    // Statistical analysis
    m.def("analyzeStatistics", [](py::object brain) {
        auto wrapper = brain.cast<std::shared_ptr<BrainWrapper>>();
        
        py::dict stats;
        stats["total_neurons"] = wrapper->getTotalNeuronCount();
        stats["total_synapses"] = wrapper->getTotalSynapseCount();
        stats["firing_neurons"] = wrapper->getFiringNeuronCount();
        stats["average_firing_rate"] = wrapper->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = wrapper->getExcitationInhibitionRatio();
        stats["total_spikes"] = wrapper->getTotalSpikeCount();
        stats["developmental_stage"] = wrapper->getDevelopmentalStageName();
        
        return stats;
    }, "Analyze brain statistics");

    // Memory usage estimation
    m.def("estimateMemoryUsage", [](py::object brain) {
        auto wrapper = brain.cast<std::shared_ptr<BrainWrapper>>();
        
        size_t neuronMemory = wrapper->getTotalNeuronCount() * sizeof(float) * 10; // Rough estimate
        size_t synapseMemory = wrapper->getTotalSynapseCount() * sizeof(float) * 4; // Rough estimate
        size_t configMemory = 1024; // Rough estimate
        
        size_t totalMemory = neuronMemory + synapseMemory + configMemory;
        
        py::dict memoryInfo;
        memoryInfo["estimated_mb"] = totalMemory / (1024 * 1024);
        memoryInfo["neuron_mb"] = neuronMemory / (1024 * 1024);
        memoryInfo["synapse_mb"] = synapseMemory / (1024 * 1024);
        memoryInfo["config_mb"] = configMemory / (1024 * 1024);
        
        return memoryInfo;
    }, "Estimate memory usage of brain");
}