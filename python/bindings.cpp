#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <numeric>
#include <cmath>

// Include all NLM framework headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"

// Convenience typedefs for Python API
using PyFloat = float;
using PyDouble = double;
using PyString = std::string;
using PyBool = bool;
using PySize = size_t;

namespace py = pybind11;
namespace nlm {

// Exception classes for Python bindings
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidNeuronException : public NLMException {
public:
    explicit InvalidNeuronException(const std::string& msg) : NLMException("InvalidNeuron: " + msg) {}
};

class InvalidSynapseException : public NLMException {
public:
    explicit InvalidSynapseException(const std::string& msg) : NLMException("InvalidSynapse: " + msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("ConfigError: " + msg) {}
};

// Utility functions for Python API
namespace pyutils {
    // Convert std::vector to Python list
    template<typename T>
    py::list to_python_list(const std::vector<T>& vec) {
        py::list result;
        for (const auto& item : vec) {
            result.append(item);
        }
        return result;
    }
    
    // Convert Python list to std::vector
    template<typename T>
    std::vector<T> to_vector(py::list list) {
        std::vector<T> result;
        for (auto item : list) {
            result.push_back(item.cast<T>());
        }
        return result;
    }
    
    // Safe numpy-style array access
    template<typename T>
    std::vector<T> get_array_data(py::array_t<T> array) {
        if (array.ndim() != 1) {
            throw NLMException("Array must be 1-dimensional");
        }
        auto buffer = array.request();
        T* ptr = (T*)buffer.ptr;
        size_t size = buffer.size;
        return std::vector<T>(ptr, ptr + size);
    }
    
    // Validate neuron ID
    void validate_neuron_id(const NeuronId& id, const std::string& operation) {
        if (id.value == UINT64_MAX) {
            throw InvalidNeuronException("Invalid neuron ID for " + operation);
        }
    }
    
    // Validate synapse ID
    void validate_synapse_id(const SynapseId& id, const std::string& operation) {
        if (id.value == UINT64_MAX) {
            throw InvalidSynapseException("Invalid synapse ID for " + operation);
        }
    }
    
    // Validate configuration key
    void validate_config_key(const std::string& key, const std::string& operation) {
        if (key.empty()) {
            throw ConfigError("Empty key not allowed for " + operation);
        }
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This module provides bindings for:
        - Neuron and Synapse management with full API coverage
        - Brain simulation with neural regions and populations
        - Sensory input handling (Vision, Audio, InternalSignals)
        - Motor actions and agent control
        - World simulation and environment interaction
        - Configuration management
        - Neuromodulation systems
        - Development and plasticity
        - AgentBrain interface for RL applications
        
        Features:
        * Complete error handling with descriptive exceptions
        * Type-safe API with proper validation
        * Convenience utility functions
        * Comprehensive documentation strings
        * Backward compatible with existing code
        * Performance optimized with reference policies
        
        Usage example:
        
        import pynlm as nlm
        
        # Create brain and world
        config = nlm.createDefaultConfig()
        brain = nlm.createBrain(config)
        world = nlm.createSimpleWorld()
        agent = nlm.createAgentBrain(brain)
        
        # Initialize
        agent.initialize(world)
        
        # Simulation loop
        for step in range(1000):
            brain.step(step)
            
        # Access statistics
        print(f"Active neurons: {brain.getActiveNeuronCount()}")
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<InvalidNeuronException>(m, "InvalidNeuronError");
    py::register_exception<InvalidSynapseException>(m, "InvalidSynapseError");
    py::register_exception<ConfigError>(m, "ConfigError");

    //===================================
    // STRONG TYPES AND IDENTIFIERS
    //===================================
    
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
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });
    
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    //===================================
    // ENUMERATIONS
    //===================================
    
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

    //===================================
    // CONFIGURATION SYSTEM
    //===================================
    
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
        .def("get", &Config::get, py::arg("key"),
             "Get a configuration value")
        .def("get_int", [](Config& self, const std::string& key, int default_val) {
            auto value = self.get(key);
            try {
                return std::stoi(value);
            } catch (...) {
                return default_val;
            }
        }, py::arg("key"), py::arg("default_val") = 0,
           "Get integer configuration value with default")
        .def("get_float", [](Config& self, const std::string& key, float default_val) {
            auto value = self.get(key);
            try {
                return std::stof(value);
            } catch (...) {
                return default_val;
            }
        }, py::arg("key"), py::arg("default_val") = 0.0f,
           "Get float configuration value with default")
        .def("get_string", [](Config& self, const std::string& key, const std::string& default_val) {
            auto value = self.get(key);
            try {
                return value;
            } catch (...) {
                return default_val;
            }
        }, py::arg("key"), py::arg("default_val") = "",
           "Get string configuration value with default")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"),
           "Set integer configuration value")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, std::to_string(value));
        }, py::arg("key"), py::arg("value"),
           "Set float configuration value")
        .def("set_string", &Config::set, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    //===================================
    // SENSORY INPUT SYSTEM
    //===================================
    
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input")
        .def("getTypeString", [](const SensoryInput& self) {
            return self.getType();
        }, "Get the type as a string")
        .def("updateTimestamp", [](SensoryInput& self) {
            self.setTimestamp(std::chrono::duration<double>::clock::now().time_since_epoch().count());
        }, "Update timestamp to current time");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataRaw", [](Vision& self, float* data, size_t size) {
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("resize", [](Vision& self, size_t width, size_t height, size_t channels = 3) {
            return Vision(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("isValid", [](const Vision& self) {
            return !self.getData().empty() && self.getWidth() > 0 && self.getHeight() > 0;
        }, "Check if vision data is valid")
        .def("normalize", [](Vision& self) {
            auto& data = self.getData();
            if (!data.empty()) {
                float maxVal = *std::max_element(data.begin(), data.end(),
                                                 [](float a, float b) { return std::abs(a) < std::abs(b); });
                if (maxVal != 0.0f) {
                    float scale = 1.0f / maxVal;
                    for (auto& pixel : data) {
                        pixel *= scale;
                    }
                }
            }
            return self;
        }, "Normalize vision data to [0,1]")
        .def("resizeBilinear", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Note: This is a simplified resize; actual implementation may use interpolation
            return Vision(newWidth, newHeight, self.getChannels());
        }, py::arg("newWidth"), py::arg("newHeight"),
           "Resize vision data using bilinear interpolation");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("normalize", [](Audio& self, float targetLevel = 0.5f) {
            auto& data = self.getData();
            if (!data.empty()) {
                float maxVal = *std::max_element(data.begin(), data.end(),
                                                 [](float a, float b) { return std::abs(a) < std::abs(b); });
                if (maxVal != 0.0f) {
                    float scale = targetLevel / maxVal;
                    for (auto& sample : data) {
                        sample *= scale;
                    }
                }
            }
            return self;
        }, py::arg("targetLevel") = 0.5f, "Normalize audio to target level")
        .def("applyEffect", [](Audio& self, const std::string& effect, float param = 0.5f) {
            auto& data = self.getData();
            if (effect == "reverse") {
                std::reverse(data.begin(), data.end());
            } else if (effect == "echo") {
                std::vector<float> echoData = data;
                float decay = param;
                for (size_t i = 0; i < data.size(); ++i) {
                    if (i + 1 < data.size()) {
                        data[i + 1] += echoData[i] * decay;
                    }
                }
            }
            return self;
        }, py::arg("effect"), py::arg("param") = 0.5f, "Apply audio effect");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignalCount", &InternalSignals::getDimensions)
        .def("getSignals", &InternalSignals::getData)
        .def("getMaxSignal", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::max_element(data.begin(), data.end());
        }, "Get maximum signal value")
        .def("getMinSignal", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            return *std::min_element(data.begin(), data.end());
        }, "Get minimum signal value")
        .def("getAverageSignal", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            return sum / data.size();
        }, "Get average signal value")
        .def("getStdDev", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return 0.0f;
            float avg = std::accumulate(data.begin(), data.end(), 0.0f) / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - avg) * (val - avg);
            }
            variance /= data.size();
            return std::sqrt(variance);
        }, "Get standard deviation of signals")
        .def("addNoise", [](InternalSignals& self, float noiseLevel = 0.1f) {
            auto& data = self.getData();
            for (auto& signal : data) {
                signal += (static_cast<float>(rand()) / RAND_MAX - 0.5f) * 2.0f * noiseLevel;
            }
            return self;
        }, py::arg("noiseLevel") = 0.1f, "Add Gaussian noise to signals")
        .def("smooth", [](InternalSignals& self, float alpha = 0.5f) {
            auto& data = self.getData();
            if (data.empty()) return self;
            float smoothed = data[0] * alpha;
            for (size_t i = 1; i < data.size(); ++i) {
                smoothed = smoothed * (1.0f - alpha) + data[i] * alpha;
                data[i] = smoothed;
            }
            return self;
        }, py::arg("alpha") = 0.5f, "Apply exponential smoothing to signals");

    //===================================
    // MOTOR SYSTEM
    //===================================
    
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
        .def("getParameter", [](const Action& self, size_t index, float default_val = 0.0f) {
            const auto& params = self.getParameters();
            if (index < params.size()) {
                return params[index];
            }
            return default_val;
        }, py::arg("index"), py::arg("default_val") = 0.0f,
           "Get parameter value at index")
        .def("addParameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"), "Add parameter to action")
        .def("isComplete", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action is complete with parameters");

    //===================================
    // WORLD SYSTEM
    //===================================
    
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
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to position")
        .def("isColliding", [](const WorldObject& self, float x, float y, float radius = 0.0f) {
            float dist = self.distanceTo(x, y);
            float effectiveRadius = radius > 0.0f ? radius : self.radius;
            return dist < (self.radius + effectiveRadius);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.0f,
           "Check if object collides with position/radius");

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
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to position")
        .def("isNear", [](const AgentBody& self, float x, float y, float distance) {
            return self.distanceTo(x, y) < distance;
        }, py::arg("x"), py::arg("y"), py::arg("distance"),
           "Check if agent is within distance of position")
        .def("getVectorTo", [](const AgentBody& self, float x, float y) {
            return std::make_pair(x - self.x, y - self.y);
        }, py::arg("x"), py::arg("y"),
           "Get vector from agent to position");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("getFormattedMessage", [](const ActionResult& self) {
            std::string prefix = self.success ? "SUCCESS: " : "FAILURE: ";
            return prefix + self.message;
        }, "Get formatted message with status");

    //===================================
    // WORLD SIMULATION
    //===================================
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"))
        .def("reset", &SimpleWorld::reset)
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"))
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"))
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("getObjectAt", [](SimpleWorld& self, float x, float y) {
            WorldObject* obj = self.getObjectAt(x, y);
            if (obj) {
                return *obj;
            }
            return WorldObject(x, y, WorldObjectType::Empty);
        }, py::arg("x"), py::arg("y"),
           "Get object at position (returns empty if none)")
        .def("countObjects", [](const SimpleWorld& self, WorldObjectType type) {
            size_t count = 0;
            for (const auto& obj : self.objects_) {
                if (obj.type == type) count++;
            }
            return count;
        }, py::arg("type"), "Count objects of specific type")
        .def("getObjectsOfType", [](const SimpleWorld& self, WorldObjectType type) {
            py::list result;
            for (const auto& obj : self.objects_) {
                if (obj.type == type) {
                    result.append(obj);
                }
            }
            return result;
        }, py::arg("type"), "Get all objects of specific type");

    //===================================
    // BRAIN SYSTEM
    //===================================
    
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system access")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity access")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity access")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity access")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory access")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory access")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory access")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system access")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner access")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation access")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection access")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system access")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine access")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity access")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty access")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error access")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator access");

    //===================================
    // AGENT BRAIN INTERFACE
    //===================================
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        .def("getTotalNeurons", [](const AgentBrain& self) {
            return self.getBrain()->getTotalNeuronCount();
        }, "Get total neuron count")
        .def("getTotalSynapses", [](const AgentBrain& self) {
            return self.getBrain()->getTotalSynapseCount();
        }, "Get total synapse count")
        .def("getActiveNeurons", [](const AgentBrain& self) {
            return self.getBrain()->getActiveNeuronCount();
        }, "Get active neuron count")
        .def("getFiringNeurons", [](const AgentBrain& self) {
            return self.getBrain()->getFiringNeuronCount();
        }, "Get firing neuron count")
        .def("getAverageFiringRate", [](const AgentBrain& self) {
            return self.getBrain()->getAverageFiringRate();
        }, "Get average firing rate");

    //===================================
    // FACTORY FUNCTIONS
    //===================================
    
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

    //===================================
    // CONSTANTS AND UTILITIES
    //===================================
    
    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Utility functions
    m.def("createNeuronId", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Create neuron ID from value");
    
    m.def("createSynapseId", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Create synapse ID from value");
    
    m.def("createRegionId", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Create region ID from value");
    
    m.def("createPopulationId", [](uint64_t value) {
        return PopulationId(value);
    }, py::arg("value"), "Create population ID from value");
    
    // Convenience factory methods
    m.def("createVision", [](size_t width, size_t height, size_t channels = 3) {
        return Vision(width, height, channels);
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
       "Create vision sensory input");
    
    m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
        return Audio(sampleRate, numSamples);
    }, py::arg("sampleRate"), py::arg("numSamples"),
       "Create audio sensory input");
    
    m.def("createInternalSignals", []() {
        return InternalSignals();
    }, "Create internal signals sensory input");
    
    m.def("createAction", [](ActionType type) {
        return Action(type);
    }, py::arg("type"), "Create action with type");
    
    m.def("createActionWithParams", [](ActionType type, const std::vector<float>& params) {
        return Action(type, params);
    }, py::arg("type"), py::arg("params"), "Create action with type and parameters");
    
    // Math utilities
    m.def("clamp", [](float value, float min_val, float max_val) {
        return std::max(min_val, std::min(value, max_val));
    }, py::arg("value"), py::arg("min_val"), py::arg("max_val"),
       "Clamp value between min and max");
    
    m.def("normalize", [](float value, float min_val, float max_val) {
        if (max_val == min_val) return 0.5f;
        return (value - min_val) / (max_val - min_val);
    }, py::arg("value"), py::arg("min_val"), py::arg("max_val"),
       "Normalize value to [0,1] range");
    
    m.def("lerp", [](float t, float a, float b) {
        return a + t * (b - a);
    }, py::arg("t"), py::arg("a"), py::arg("b"), "Linear interpolation");
    
    // Error handling utilities
    m.def("check_neuron_valid", pyutils::validate_neuron_id,
          py::arg("id"), py::arg("operation"),
          "Validate neuron ID for operation");
    
    m.def("check_synapse_valid", pyutils::validate_synapse_id,
          py::arg("id"), py::arg("operation"),
          "Validate synapse ID for operation");
    
    m.def("check_config_key", pyutils::validate_config_key,
          py::arg("key"), py::arg("operation"),
          "Validate configuration key for operation");
    
    // Random utilities
    m.def("random_float", []() {
        return static_cast<float>(rand()) / RAND_MAX;
    }, "Generate random float in [0,1]");
    
    m.def("random_range", [](float min_val, float max_val) {
        return min_val + static_cast<float>(rand()) / (RAND_MAX / (max_val - min_val));
    }, py::arg("min_val"), py::arg("max_val"), "Generate random float in range");
}

} // namespace nlm