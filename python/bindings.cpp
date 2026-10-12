#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <utility>

// Include NLM headers
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

// Custom exception classes for better error handling
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class PyConfigurationError : public PyNLMError {
public:
    explicit PyConfigurationError(const std::string& msg) : PyNLMError("Configuration Error: " + msg) {}
};

class PySimulationError : public PyNLMError {
public:
    explicit PySimulationError(const std::string& msg) : PyNLMError("Simulation Error: " + msg) {}
};

class PyValidationError : public PyNLMError {
public:
    explicit PyValidationError(const std::string& msg) : PyNLMError("Validation Error: " + msg) {}
};

// Forward declarations for helper functions
template<typename T>
T&& move_if_not_const(T&& t) {
    return std::forward<T>(t);
}

PYBIND11_MODULE(pynlm, m) {
    // Module-level documentation and metadata
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        An experimental artificial developmental brain simulator that creates neural systems
        beginning in primitive states and acquiring complex abilities through environmental interaction.
        
        This module provides Python bindings for the NLM C++ neural simulation framework,
        offering classes for brain simulation, world management, agent interfaces, and more.
        
        Key Features:
        - Modern C++20 implementation with Python bindings
        - Comprehensive neural network simulation capabilities
        - Advanced learning and development systems
        - Performance monitoring and profiling tools
        - Flexible configuration and checkpoint management
        
        Quick Start:
            import pynlm
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            for i in range(100):
                brain.step(i)
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<PyNLMError>(m, "NLMError");
    py::register_exception<PyConfigurationError>(m, "ConfigurationError");
    py::register_exception<PySimulationError>(m, "SimulationError");
    py::register_exception<PyValidationError>(m, "ValidationError");
    
    // Helper functions for creating objects with builder pattern
    m.def("createBrainWithConfig", [](const std::shared_ptr<Config>& config) {
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a brain with the given configuration");
    
    m.def("createWorldWithSettings", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
       "Create a simple world with the specified dimensions");
    
    m.def("createAgentWithBrainWorld", [](const std::shared_ptr<Brain>& brain, const std::shared_ptr<SimpleWorld>& world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain interface connecting brain and world");
    
    // NeuronId with enhanced functionality
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index, "Get the neuron index")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return "NeuronId(" + std::to_string(id.value) + ")";
        });
    
    // SynapseId with enhanced functionality
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index, "Get the synapse index")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return "SynapseId(" + std::to_string(id.value) + ")";
        });
    
    // RegionId with enhanced functionality
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index, "Get the region index")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return "RegionId(" + std::to_string(id.value) + ")";
        });
    
    // PopulationId with enhanced functionality
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index, "Get the population index")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });
    
    // Enumerations with better documentation
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neurons that promote firing")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neurons that suppress firing")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neurons that affect plasticity")
        .value("Sensory", NeuronType::Sensory, "Sensory neurons that receive external input")
        .value("Motor", NeuronType::Motor, "Motor neurons that generate output actions")
        .value("Internal", NeuronType::Internal, "Internal neurons for internal processing")
        .export_values();
    
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapses that increase firing")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapses that decrease firing")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapses that affect plasticity")
        .value("Electrical", SynapseType::Electrical, "Electrical synapses with fast transmission")
        .value("GapJunction", SynapseType::GapJunction, "Gap junctions for electrical coupling")
        .export_values();
    
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period for learning")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();
    
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting, "Neuron is at rest")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();
    
    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)pbdoc")
        .value("MoveForward", ActionType::MoveForward, "Move forward in the world")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward in the world")
        .value("MoveLeft", ActionType::MoveLeft, "Move left in the world")
        .value("MoveRight", ActionType::MoveRight, "Move right in the world")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left in the world")
        .value("TurnRight", ActionType::TurnRight, "Turn right in the world")
        .value("Look", ActionType::Look, "Look around")
        .value("LookUp", ActionType::LookUp, "Look up")
        .value("LookDown", ActionType::LookDown, "Look down")
        .value("Interact", ActionType::Interact, "Interact with environment")
        .value("Eat", ActionType::Eat, "Eat food")
        .value("Drink", ActionType::Drink, "Drink water")
        .value("Rest", ActionType::Rest, "Rest and recover")
        .value("Wait", ActionType::Wait, "Wait without moving")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();
    
    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right")
        .value("LookLeft", MotorCommand::LookLeft, "Look left")
        .value("LookRight", MotorCommand::LookRight, "Look right")
        .value("Interact", MotorCommand::Interact, "Interact with object")
        .value("Wait", MotorCommand::Wait, "Wait without action")
        .export_values();
    
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty, "Empty space")
        .value("Resource", WorldObjectType::Resource, "Resource that can be collected")
        .value("Hazard", WorldObjectType::Hazard, "Hazard that causes damage")
        .value("Wall", WorldObjectType::Wall, "Wall that blocks movement")
        .value("Marker", WorldObjectType::Marker, "Marker for special locations")
        .export_values();
    
    // Config class with enhanced functionality
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
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
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("get", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), "Get a configuration value")
        .def("getAsInt", [](const Config& self, const std::string& key) {
            return self.getAsInt(key);
        }, py::arg("key"), "Get an integer configuration value")
        .def("getAsFloat", [](const Config& self, const std::string& key) {
            return self.getAsFloat(key);
        }, py::arg("key"), "Get a float configuration value")
        .def("getAsString", [](const Config& self, const std::string& key) {
            return self.getAsString(key);
        }, py::arg("key"), "Get a string configuration value")
        .def("getAsBool", [](const Config& self, const std::string& key) {
            return self.getAsBool(key);
        }, py::arg("key"), "Get a boolean configuration value")
        .def("setFromDict", [](Config& self, const py::dict& dict) {
            py::object py_key, py_value;
            for (auto item : dict) {
                py_key = item.first;
                py_value = item.second;
                std::string key = py_key.cast<std::string>();
                
                // Try different types
                try {
                    self.set(key, py_value.cast<int>());
                } catch (...) {
                    try {
                        self.set(key, py_value.cast<float>());
                    } catch (...) {
                        try {
                            self.set(key, py_value.cast<std::string>());
                        } catch (...) {
                            try {
                                self.set(key, py_value.cast<bool>());
                            } catch (...) {
                                // If all conversions fail, skip this key
                            }
                        }
                    }
                }
            }
            return 0;
        }, py::arg("dict"), "Set configuration from a Python dictionary")
        .def("to_dict", [](const Config& self) {
            py::dict result;
            auto keys = self.getKeys();
            for (const auto& key : keys) {
                result[key] = self.get(key);
            }
            return result;
        }, "Convert configuration to Python dictionary")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__len__", [](const Config& self) {
            return self.getKeys().size();
        })
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        })
        .def("__getitem__", [](Config& self, const std::string& key) {
            return self.get(key);
        })
        .def("__setitem__", [](Config& self, const std::string& key, const py::object& value) {
            // Try different types
            try {
                self.set(key, value.cast<int>());
            } catch (...) {
                try {
                    self.set(key, value.cast<float>());
                } catch (...) {
                    try {
                        self.set(key, value.cast<std::string>());
                    } catch (...) {
                        try {
                            self.set(key, value.cast<bool>());
                        } catch (...) {
                            throw PyConfigurationError("Cannot convert value to any supported type");
                        }
                    }
                }
            }
        });
    
    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getDataSize", [](const SensoryInput& self) {
            return self.getData().size();
        }, "Get the size of sensory data")
        .def("normalize", [](SensoryInput& self, float minVal, float maxVal) {
            auto data = self.getData();
            for (auto& val : data) {
                val = (val - minVal) / (maxVal - minVal);
            }
            return self;
        }, py::arg("minVal"), py::arg("maxVal"), "Normalize sensory data to [0, 1] range");
    
    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("setDataFromImage", [](Vision& self, const py::array_t<float>& image) {
            // Convert numpy array to vector
            auto buf = image.request();
            size_t size = buf.size;
            std::vector<float> data(size);
            std::memcpy(data.data(), buf.ptr, size * sizeof(float));
            self.setData(data);
        }, py::arg("image"), "Set vision data from numpy array")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("getArea", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get image area (width * height)")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Note: This is a simplified resize implementation
            // Actual implementation would depend on Vision class capabilities
            return self;
        }, py::arg("newWidth"), py::arg("newHeight"), "Resize vision data");
    
    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("getDuration", [](const Audio& self) {
            return self.getNumSamples() / (self.getSampleRate() / 1000.0f); // in seconds
        }, "Get audio duration in seconds")
        .def("normalize", [](Audio& self) {
            auto data = self.getData();
            if (data.empty()) return self;
            
            float max_val = *std::max_element(data.begin(), data.end());
            float min_val = *std::min_element(data.begin(), data.end());
            
            for (auto& val : data) {
                val = (val - min_val) / (max_val - min_val);
            }
            return self;
        }, "Normalize audio data to [0, 1] range");
    
    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("getSignalCount", [](const InternalSignals& self) {
            return self.getData().size();
        }, "Get number of signals")
        .def("getSignalSum", [](const InternalSignals& self) {
            auto data = self.getData();
            return std::accumulate(data.begin(), data.end(), 0.0f);
        }, "Get sum of all signals")
        .def("getSignalAverage", [](const InternalSignals& self) {
            auto data = self.getData();
            if (data.empty()) return 0.0f;
            return std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
        }, "Get average of all signals");
    
    // Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get action name")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get number of parameters")
        .def("getParameter", [](const Action& self, size_t index) {
            auto params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            throw PySimulationError("Parameter index out of range");
        }, py::arg("index"), "Get parameter at specified index")
        .def("setParameter", [](Action& self, size_t index, float value) {
            auto params = self.getParameters();
            if (index < params.size()) {
                params[index] = value;
                self.setParameters(params);
            } else {
                throw PySimulationError("Parameter index out of range");
            }
        }, py::arg("index"), py::arg("value"), "Set parameter at specified index")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("to_string", [](const Action& self) {
            return self.getName();
        }, "Get string representation of action");
    
    // WorldObject class
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
        .def("distanceTo", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to given coordinates")
        .def("isCollidingWith", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            return distance < (self.radius + other.radius);
        }, py::arg("other"), "Check if colliding with another object")
        .def("to_dict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = static_cast<int>(self.type);
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert world object to Python dictionary")
        .def("from_dict", [](WorldObject& self, const py::dict& data) {
            self.x = data["x"].cast<float>();
            self.y = data["y"].cast<float>();
            self.radius = data.get("radius", 0.5f).cast<float>();
            self.type = static_cast<WorldObjectType>(data["type"].cast<int>());
            self.value = data.get("value", 0.0f).cast<float>();
            self.active = data.get("active", true).cast<bool>();
            return self;
        }, py::arg("data"), "Update world object from Python dictionary");
    
    // AgentBody class
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
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to given coordinates")
        .def("getSpeed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get current speed")
        .def("getDirection", [](const AgentBody& self) {
            float angle = std::atan2(self.velocityY, self.velocityX);
            return angle * 180.0f / M_PI; // Convert to degrees
        }, "Get current direction in degrees")
        .def("update", [](AgentBody& self, float dt, float accelX, float accelY, float maxSpeed) {
            // Simplified physics update
            self.velocityX += accelX * dt;
            self.velocityY += accelY * dt;
            
            float speed = getSpeed(self);
            if (speed > maxSpeed) {
                float scale = maxSpeed / speed;
                self.velocityX *= scale;
                self.velocityY *= scale;
            }
            
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            
            return self;
        }, py::arg("dt"), py::arg("accelX"), py::arg("accelY"), py::arg("maxSpeed"),
           "Update position based on velocity and acceleration")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state")
        .def("to_dict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert agent body to Python dictionary")
        .def("from_dict", [](AgentBody& self, const py::dict& data) {
            self.x = data["x"].cast<float>();
            self.y = data["y"].cast<float>();
            self.orientation = data.get("orientation", 0.0f).cast<float>();
            self.velocityX = data.get("velocityX", 0.0f).cast<float>();
            self.velocityY = data.get("velocityY", 0.0f).cast<float>();
            self.angularVelocity = data.get("angularVelocity", 0.0f).cast<float>();
            self.energy = data.get("energy", 100.0f).cast<float>();
            self.health = data.get("health", 1.0f).cast<float>();
            self.age = data.get("age", 0.0f).cast<float>();
            self.isMoving = data.get("isMoving", false).cast<bool>();
            self.isTurning = data.get("isTurning", false).cast<bool>();
            self.lastActionTime = data.get("lastActionTime", 0.0f).cast<float>();
            return self;
        }, py::arg("data"), "Update agent body from Python dictionary");
    
    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to Python dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& data) {
            self.reward = data.get("reward", 0.0f).cast<float>();
            self.success = data.get("success", false).cast<bool>();
            self.message = data.get("message", "").cast<std::string>();
            return self;
        }, py::arg("data"), "Update action result from Python dictionary")
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if action result is positive (reward > 0)")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if action result is negative (reward < 0)")
        .def("isNeutral", [](const ActionResult& self) {
            return self.reward == 0.0f;
        }, "Check if action result is neutral (reward = 0)");
    
    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, py::return_value_policy::reference_internal,
             "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, py::return_value_policy::reference_internal,
             "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, py::return_value_policy::reference_internal,
             "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, py::return_value_policy::reference_internal,
             "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, py::return_value_policy::reference_internal,
             "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all signals as dictionary")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().getData().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().getData().empty();
        }, "Check if percept has touch data")
        .def("hasInternal", [](const SensoryPercept& self) {
            return !self.getInternal().getData().empty();
        }, "Check if percept has internal signals")
        .def("hasProprioception", [](const SensoryPercept& self) {
            return !self.getProprioception().getData().empty();
        }, "Check if percept has proprioception data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().getData().empty();
        }, "Check if percept has audio data")
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            result["timestamp"] = self.getTimestamp();
            
            auto vision = self.getVision();
            if (!vision.getData().empty()) {
                py::dict vision_data;
                vision_data["width"] = vision.getWidth();
                vision_data["height"] = vision.getHeight();
                vision_data["channels"] = vision.getChannels();
                vision_data["data"] = vision.getData();
                result["vision"] = vision_data;
            }
            
            auto touch = self.getTouch();
            if (!touch.getData().empty()) {
                result["touch"] = touch.getData();
            }
            
            auto internal = self.getInternal();
            if (!internal.getData().empty()) {
                result["internal"] = internal.getData();
            }
            
            auto proprioception = self.getProprioception();
            if (!proprioception.getData().empty()) {
                result["proprioception"] = proprioception.getData();
            }
            
            auto audio = self.getAudio();
            if (!audio.getData().empty()) {
                py::dict audio_data;
                audio_data["sample_rate"] = audio.getSampleRate();
                audio_data["num_samples"] = audio.getNumSamples();
                audio_data["data"] = audio.getData();
                result["audio"] = audio_data;
            }
            
            return result;
        }, "Convert sensory percept to Python dictionary")
        .def("from_dict", [](SensoryPercept& self, const py::dict& data) {
            if (data.contains("timestamp")) {
                self.setTimestamp(data["timestamp"].cast<float>());
            }
            
            if (data.contains("vision")) {
                auto vision_dict = data["vision"].cast<py::dict>();
                Vision vision;
                vision.setData(vision_dict["data"].cast<std::vector<float>>());
                vision.setWidth(vision_dict["width"].cast<size_t>());
                vision.setHeight(vision_dict["height"].cast<size_t>());
                vision.setChannels(vision_dict["channels"].cast<size_t>());
                self.setVision(vision);
            }
            
            if (data.contains("touch")) {
                self.setTouch(data["touch"].cast<std::vector<float>>());
            }
            
            if (data.contains("internal")) {
                self.setInternal(data["internal"].cast<std::vector<float>>());
            }
            
            if (data.contains("proprioception")) {
                self.setProprioception(data["proprioception"].cast<std::vector<float>>());
            }
            
            if (data.contains("audio")) {
                auto audio_dict = data["audio"].cast<py::dict>();
                Audio audio;
                audio.setData(audio_dict["data"].cast<std::vector<float>>());
                audio.setSampleRate(audio_dict["sample_rate"].cast<size_t>());
                audio.setNumSamples(audio_dict["num_samples"].cast<size_t>());
                self.setAudio(audio);
            }
            
            return self;
        }, py::arg("data"), "Update sensory percept from Python dictionary");
    
    // SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for given timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get sensory percept from world")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid in world")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for world")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("to_dict", [](const SimpleWorld& self) {
            py::dict result;
            result["width"] = self.getWidth();
            result["height"] = self.getHeight();
            result["maxEnergy"] = self.getMaxEnergy();
            result["energyDecayRate"] = self.getEnergyDecayRate();
            result["simulationTime"] = self.getSimulationTime();
            result["randomSeed"] = self.getRandomSeed();
            return result;
        }, "Convert world to Python dictionary")
        .def("from_dict", [](SimpleWorld& self, const py::dict& data) {
            size_t width = data.get("width", 100).cast<size_t>();
            size_t height = data.get("height", 100).cast<size_t>();
            size_t visionWidth = data.get("visionWidth", 8).cast<size_t>();
            size_t visionHeight = data.get("visionHeight", 8).cast<size_t>();
            self.configure(width, height, visionWidth, visionHeight);
            
            float maxEnergy = data.get("maxEnergy", 100.0f).cast<float>();
            self.setMaxEnergy(maxEnergy);
            
            float energyDecayRate = data.get("energyDecayRate", 0.01f).cast<float>();
            self.setEnergyDecayRate(energyDecayRate);
            
            float simulationTime = data.get("simulationTime", 0.0f).cast<float>();
            // Note: SimpleWorld doesn't have direct simulation time setter
            // This would need to be modified in the C++ code
            
            int64_t randomSeed = data.get("randomSeed", 42).cast<int64_t>();
            self.setRandomSeed(randomSeed);
            
            return self;
        }, py::arg("data"), "Update world from Python dictionary");
    
    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with given configuration")
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
        .def("to_dict", [](const Brain& self) {
            py::dict result;
            result["total_neuron_count"] = self.getTotalNeuronCount();
            result["total_synapse_count"] = self.getTotalSynapseCount();
            result["firing_neuron_count"] = self.getFiringNeuronCount();
            result["average_firing_rate"] = self.getAverageFiringRate();
            result["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            result["total_spike_count"] = self.getTotalSpikeCount();
            result["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return result;
        }, "Convert brain to Python dictionary")
        .def("from_dict", [](Brain& self, const py::dict& data) {
            // Note: This is a simplified example - actual implementation would need
            // to handle more complex state restoration
            return self;
        }, py::arg("data"), "Update brain from Python dictionary (simplified)")
        .def("simulateEpisode", [](Brain& self, size_t num_steps) {
            for (size_t step = 0; step < num_steps; ++step) {
                self.step(step);
            }
            return self.getTotalSpikeCount();
        }, py::arg("num_steps"), "Run simulation episode and return spike count")
        .def("getLearningSummary", [](const Brain& self) {
            py::dict summary;
            summary["total_spikes"] = self.getTotalSpikeCount();
            summary["firing_rate"] = self.getAverageFiringRate();
            summary["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            summary["region_count"] = self.getRegionCount();
            return summary;
        }, "Get learning summary as Python dictionary");
    
    // AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface with given brain")
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
             py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        .def("to_dict", [](const AgentBrain& self) {
            py::dict result;
            result["neuromodulation_level"] = self.getNeuromodulationLevel();
            result["curiosity_level"] = self.getCuriosityLevel();
            result["novelty_level"] = self.getNoveltyLevel();
            result["prediction_error"] = self.getPredictionError();
            result["development_stage"] = self.getDevelopmentalStage();
            result["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            result["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            result["development_enabled"] = self.isDevelopmentEnabled();
            result["curiosity_enabled"] = self.isCuriosityEnabled();
            return result;
        }, "Convert agent brain to Python dictionary")
        .def("from_dict", [](AgentBrain& self, const py::dict& data) {
            // Note: This is a simplified example - actual implementation would need
            // to handle agent state restoration
            return self;
        }, py::arg("data"), "Update agent brain from Python dictionary (simplified)")
        .def("runEpisode", [](AgentBrain& self, const std::shared_ptr<SimpleWorld>& world, size_t num_steps) {
            auto percept = world->getSensoryPercept();
            self.processSensoryInput(percept);
            
            for (size_t step = 0; step < num_steps; ++step) {
                // Note: This would need access to brain and world
                // For simplicity, we just update development
                self.updateDevelopment(0.1f);
            }
            
            return self.getCuriosityLevel();
        }, py::arg("world"), py::arg("num_steps"), "Run agent episode and return curiosity level (simplified)");
    
    // Module-level convenience functions
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
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Version information
    m.attr("__version__") = "1.0.0";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = "Neural Learning Machine Python Bindings";
    
    // Additional helper functions
    m.def("get_version", []() {
        return std::make_tuple("1.0.0", "NLM Research Team", "Neural Learning Machine Python Bindings");
    }, "Get version information as tuple");
    
    m.def("validate_simulation", [](const std::shared_ptr<Brain>& brain, 
                                     const std::shared_ptr<SimpleWorld>& world,
                                     const std::shared_ptr<AgentBrain>& agent) {
        std::vector<std::string> errors;
        
        if (!brain) {
            errors.push_back("Brain is not initialized");
        }
        
        if (world && (!world->getSensoryPercept().getData().empty())) {
            errors.push_back("World has sensory data but should be empty for validation");
        }
        
        if (agent && (!agent->getBrain())) {
            errors.push_back("Agent has no brain attached");
        }
        
        if (!errors.empty()) {
            throw PyValidationError("Simulation validation failed:\n" + 
                                   std::accumulate(errors.begin(), errors.end(), std::string(),
                                                 [](const std::string& a, const std::string& b) {
                                                     return a + "  - " + b + "\n";
                                                 }));
        }
        
        return true;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       "Validate simulation setup and return True if valid");
    
    m.def("createSimulationFromDict", [](const py::dict& config) {
        auto config_ptr = std::make_shared<Config>();
        
        try {
            py::object py_key, py_value;
            for (auto item : config) {
                py_key = item.first;
                py_value = item.second;
                std::string key = py_key.cast<std::string>();
                
                try {
                    config_ptr->set(key, py_value.cast<int>());
                } catch (...) {
                    try {
                        config_ptr->set(key, py_value.cast<float>());
                    } catch (...) {
                        try {
                            config_ptr->set(key, py_value.cast<std::string>());
                        } catch (...) {
                            try {
                                config_ptr->set(key, py_value.cast<bool>());
                            } catch (...) {
                                // Skip keys that can't be converted
                            }
                        }
                    }
                }
            }
            
            auto brain = std::make_shared<Brain>(config_ptr);
            brain->initialize();
            
            return std::make_tuple(brain, config_ptr);
            
        } catch (const std::exception& e) {
            throw PyConfigurationError(std::string("Failed to create simulation from dictionary: ") + e.what());
        }
    }, py::arg("config"), "Create simulation objects from Python dictionary");
}

} // namespace nlm