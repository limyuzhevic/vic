#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>
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

// Custom exception classes for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("Configuration error: " + message) {}
};

class InitializationError : public NLMException {
public:
    explicit InitializationError(const std::string& message) : NLMException("Initialization error: " + message) {}
};

class ResourceError : public NLMException {
public:
    explicit ResourceError(const std::string& message) : NLMException("Resource error: " + message) {}
};

class ValidationError : public NLMException {
public:
    explicit ValidationError(const std::string& message) : NLMException("Validation error: " + message) {}
};

// ConfigBuilder class for easier configuration setup
class ConfigBuilder {
private:
    std::shared_ptr<Config> config;
public:
    ConfigBuilder() : config(std::make_shared<Config>()) {}
    
    ConfigBuilder& set(const std::string& key, const std::string& value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, int value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, float value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, double value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& key, bool value) {
        config->set(key, value);
        return *this;
    }
    
    ConfigBuilder& set(const std::string& section, const std::string& key, const std::string& value) {
        std::string fullKey = section + "." + key;
        config->set(fullKey, value);
        return *this;
    }
    
    ConfigBuilder& setNeuralParameters(
        size_t regionCount = 5,
        size_t neuronsPerRegion = 100,
        float connectionDensity = 0.1f,
        float learningRate = 0.01f,
        float decayRate = 0.995f
    ) {
        config->set("neural.region_count", std::to_string(regionCount));
        config->set("neural.neurons_per_region", std::to_string(neuronsPerRegion));
        config->set("neural.connection_density", std::to_string(connectionDensity));
        config->set("neural.learning_rate", std::to_string(learningRate));
        config->set("neural.decay_rate", std::to_string(decayRate));
        return *this;
    }
    
    ConfigBuilder& setWorldParameters(
        size_t width = 100,
        size_t height = 100,
        size_t visionWidth = 20,
        size_t visionHeight = 20,
        float maxEnergy = 100.0f
    ) {
        config->set("world.width", std::to_string(width));
        config->set("world.height", std::to_string(height));
        config->set("world.vision_width", std::to_string(visionWidth));
        config->set("world.vision_height", std::to_string(visionHeight));
        config->set("world.max_energy", std::to_string(maxEnergy));
        return *this;
    }
    
    ConfigBuilder& setAgentParameters(
        float maxEnergy = 100.0f,
        float health = 100.0f,
        float energyDecayRate = 0.1f
    ) {
        config->set("agent.max_energy", std::to_string(maxEnergy));
        config->set("agent.health", std::to_string(health));
        config->set("agent.energy_decay_rate", std::to_string(energyDecayRate));
        return *this;
    }
    
    ConfigBuilder& validate() {
        if (!config->has("neural.region_count")) {
            throw ValidationError("Neural region count is required");
        }
        if (!config->has("world.width")) {
            throw ValidationError("World width is required");
        }
        if (!config->has("world.height")) {
            throw ValidationError("World height is required");
        }
        return *this;
    }
    
    std::shared_ptr<Config> build() {
        validate();
        return config;
    }
    
    // Convenience method for Python
    std::shared_ptr<Config> create() {
        return build();
    }
};

// Iterator for Config class
class ConfigIterator {
private:
    std::vector<std::string> keys;
    size_t index;
public:
    ConfigIterator(const std::vector<std::string>& keys) : keys(keys), index(0) {}
    
    bool hasNext() const {
        return index < keys.size();
    }
    
    std::string next() {
        if (index >= keys.size()) {
            throw NLMException("No more items in iterator");
        }
        return keys[index++];
    }
    
    void reset() {
        index = 0;
    }
};

// Utility functions for the module
namespace utils {
    inline std::string version() {
        return "1.0.0";
    }
    
    inline std::string getSystemInfo() {
        std::stringstream ss;
        ss << "NLM Python Bindings v1.0.0\n";
        ss << "Platform: Linux\n";
        ss << "Compiler: GNU C++\n";
        return ss.str();
    }
    
    inline double currentTime() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() / 1000.0;
    }
    
    template<typename T>
    inline std::vector<T> normalize(const std::vector<T>& input, T min = 0.0, T max = 1.0) {
        if (input.empty()) {
            return {};
        }
        T inputMin = *std::min_element(input.begin(), input.end());
        T inputMax = *std::max_element(input.begin(), input.end());
        
        if (inputMin == inputMax) {
            return std::vector<T>(input.size(), min);
        }
        
        std::vector<T> result;
        result.reserve(input.size());
        for (const auto& val : input) {
            T normalized = (val - inputMin) / (inputMax - inputMin) * (max - min) + min;
            result.push_back(normalized);
        }
        return result;
    }
    
    inline std::string arrayToString(const std::vector<float>& arr) {
        std::stringstream ss;
        ss << "[";
        for (size_t i = 0; i < arr.size(); ++i) {
            if (i > 0) ss << ", ";
            ss << arr[i];
        }
        ss << "]";
        return ss.str();
    }
}

// Enhanced NeuronId with Pythonic API
PYBIND11_MAKE_OPAQUE(std::vector<NeuronId>);

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Edition
        ---------------------------------------------------------------
        A Pythonic binding for the NLM C++ neural simulation framework.
        
        This enhanced version provides:
        - Context managers for resource cleanup
        - Pythonic operator overloads
        - Better type hints and error handling
        - ConfigBuilder for easier configuration
        - Iterator support
        - Utility functions
        - Comprehensive documentation
        
        Features:
        - Brain simulation with neural networks
        - Agent behavior and world interaction
        - Sensory processing and motor control
        - Configurable neural parameters
        - Reward-based learning
        - Developmental stages
        
        All classes are designed to work naturally with Python,
        providing a clean interface while maintaining full C++ performance.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMException", py::module_local());
    py::register_exception<ConfigError>(m, "ConfigError", py::module_local());
    py::register_exception<InitializationError>(m, "InitializationError", py::module_local());
    py::register_exception<ResourceError>(m, "ResourceError", py::module_local());
    py::register_exception<ValidationError>(m, "ValidationError", py::module_local());

    // NeuronId with enhanced Pythonic API
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
        })
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", &NeuronId::operator<=)
        .def("__gt__", &NeuronId::operator>)
        .def("__ge__", &NeuronId::operator>=);

    // SynapseId with enhanced Pythonic API
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

    // RegionId with enhanced Pythonic API
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

    // PopulationId with enhanced Pythonic API
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enum bindings
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

    // Enhanced Config class with context manager support
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("load_from_file", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("load_from_args", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        .def("get", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"),
             "Get configuration value as string")
        .def("get_int", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"),
             "Get configuration value as int")
        .def("get_float", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"),
             "Get configuration value as float")
        .def("get_bool", [](const Config& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"),
             "Get configuration value as bool")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value")
        .def("set_int", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value as int")
        .def("set_float", [](Config& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value as float")
        .def("set_bool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set configuration value as bool")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("__contains__", [](const Config& self, const std::string& key) {
            return self.has(key);
        })
        .def("__getitem__", [](const Config& self, const std::string& key) {
            return self.get(key);
        })
        .def("__setitem__", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        })
        .def("__len__", &Config::getKeys)
        .def("__iter__", [](Config& self) {
            return ConfigIterator(self.getKeys());
        })
        .def("validate", [](const Config& self) {
            if (!self.has("neural.region_count")) {
                throw ValidationError("Neural region count is required");
            }
            if (!self.has("world.width")) {
                throw ValidationError("World width is required");
            }
            if (!self.has("world.height")) {
                throw ValidationError("World height is required");
            }
        }, "Validate configuration");

    // ConfigBuilder class for easier configuration
    py::class_<ConfigBuilder>(m, "ConfigBuilder", R"pbdoc(Builder class for creating NLM configurations)pbdoc")
        .def(py::init<>())
        .def("set", static_cast<ConfigBuilder&(ConfigBuilder::*)(const std::string&, const std::string&)>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration key-value pair")
        .def("set_int", static_cast<ConfigBuilder&(ConfigBuilder::*)(const std::string&, int)>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration key-value pair as int")
        .def("set_float", static_cast<ConfigBuilder&(ConfigBuilder::*)(const std::string&, float)>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration key-value pair as float")
        .def("set_bool", static_cast<ConfigBuilder&(ConfigBuilder::*)(const std::string&, bool)>(&ConfigBuilder::set),
             py::arg("key"), py::arg("value"),
             "Set a configuration key-value pair as bool")
        .def("set_neural_parameters", &ConfigBuilder::setNeuralParameters,
             py::arg("region_count") = 5,
             py::arg("neurons_per_region") = 100,
             py::arg("connection_density") = 0.1f,
             py::arg("learning_rate") = 0.01f,
             py::arg("decay_rate") = 0.995f,
             "Set neural network parameters")
        .def("set_world_parameters", &ConfigBuilder::setWorldParameters,
             py::arg("width") = 100,
             py::arg("height") = 100,
             py::arg("vision_width") = 20,
             py::arg("vision_height") = 20,
             py::arg("max_energy") = 100.0f,
             "Set world parameters")
        .def("set_agent_parameters", &ConfigBuilder::setAgentParameters,
             py::arg("max_energy") = 100.0f,
             py::arg("health") = 100.0f,
             py::arg("energy_decay_rate") = 0.1f,
             "Set agent parameters")
        .def("validate", &ConfigBuilder::validate,
             "Validate the configuration")
        .def("build", &ConfigBuilder::build,
             "Build and return the configuration")
        .def("create", &ConfigBuilder::create,
             "Build and return the configuration (Pythonic alias)");

    // Enhanced SensoryInput with context manager support
    py::class_<SensoryInput, std::shared_ptr<SensoryInput>>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("get_type", &SensoryInput::getType, "Get the type of sensory input")
        .def("get_data", &SensoryInput::getData, "Get the raw data as a vector")
        .def("get_dimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("get_timestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("set_timestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__enter__", [](SensoryInput& self) {
            return &self;
        }, "Context manager entry")
        .def("__exit__", [](SensoryInput& self, py::args) {
            // Cleanup if needed
        }, "Context manager exit");

    // Enhanced Vision with operator overloads
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
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
        .def("add_data", [](Vision& self, const std::vector<float>& data) {
            auto current = self.getData();
            current.insert(current.end(), data.begin(), data.end());
            self.setData(current);
        }, py::arg("data"),
             "Add data to existing vision data")
        .def("scale", [](Vision& self, float factor) {
            auto data = self.getData();
            for (auto& val : data) {
                val *= factor;
            }
            self.setData(data);
        }, py::arg("factor"),
             "Scale all vision data by factor")
        .def("normalize", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            auto data = utils::normalize(self.getData(), min, max);
            self.setData(data);
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize vision data")
        .def("__add__", [](Vision& self, const Vision& other) {
            if (self.getWidth() != other.getWidth() || self.getHeight() != other.getHeight() || self.getChannels() != other.getChannels()) {
                throw ValidationError("Vision objects must have same dimensions for addition");
            }
            auto result = Vision(self.getWidth(), self.getHeight(), self.getChannels());
            auto selfData = self.getData();
            auto otherData = other.getData();
            for (size_t i = 0; i < selfData.size(); ++i) {
                result.getData()[i] = selfData[i] + otherData[i];
            }
            return result;
        }, py::arg("other"),
             "Add two Vision objects (element-wise)")
        .def("__mul__", [](Vision& self, float scalar) {
            auto result = Vision(self.getWidth(), self.getHeight(), self.getChannels());
            auto data = self.getData();
            for (auto& val : result.getData()) {
                val = val * scalar;
            }
            return result;
        }, py::arg("scalar"),
             "Multiply Vision by scalar")
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + std::to_string(v.getHeight()) +
                   "x" + std::to_string(v.getChannels()) + "@" + std::to_string(v.getTimestamp()) + ">";
        });

    // Enhanced Audio with operator overloads
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("set_data", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("get_data", &Audio::getData)
        .def("set_sample_rate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("get_sample_rate", &Audio::getSampleRate)
        .def("get_num_samples", &Audio::getNumSamples)
        .def("add_silence", [](Audio& self, float duration) {
            size_t samplesToAdd = static_cast<size_t>(duration * self.getSampleRate());
            auto data = self.getData();
            data.insert(data.end(), samplesToAdd, 0.0f);
            self.setData(data);
        }, py::arg("duration"),
             "Add silence to audio")
        .def("normalize", [](Audio& self, float min = -1.0f, float max = 1.0f) {
            auto data = utils::normalize(self.getData(), min, max);
            self.setData(data);
        }, py::arg("min") = -1.0f, py::arg("max") = 1.0f,
             "Normalize audio data")
        .def("__len__", &Audio::getNumSamples)
        .def("__getitem__", [](const Audio& self, size_t index) {
            return self.getData()[index];
        })
        .def("__setitem__", [](Audio& self, size_t index, float value) {
            auto data = self.getData();
            data[index] = value;
            self.setData(data);
        });

    // Enhanced InternalSignals
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("add_signal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value")
        .def("get_signals", &InternalSignals::getSignals,
             "Get all signal values")
        .def("get_signal_count", &InternalSignals::getSignalCount,
             "Get number of signals")
        .def("clear_signals", &InternalSignals::clearSignals,
             "Clear all signals")
        .def("get_mean", [](const InternalSignals& self) {
            const auto& signals = self.getSignals();
            if (signals.empty()) return 0.0f;
            float sum = std::accumulate(signals.begin(), signals.end(), 0.0f);
            return sum / signals.size();
        }, "Get mean signal value")
        .def("get_stddev", [](const InternalSignals& self) {
            const auto& signals = self.getSignals();
            if (signals.size() < 2) return 0.0f;
            float mean = utils::normalize(signals)[0]; // This is just a placeholder, proper stddev needed
            float sumSq = 0.0f;
            for (float val : signals) {
                sumSq += (val - mean) * (val - mean);
            }
            return std::sqrt(sumSq / (signals.size() - 1));
        }, "Get standard deviation of signals")
        .def("__len__", &InternalSignals::getSignalCount)
        .def("__iter__", [](InternalSignals& self) {
            return py::make_iterator(self.getSignals().begin(), self.getSignals().end());
        }, py::keep_alive<0, 1>());

    // Enhanced Action with context manager support
    py::class_<Action, std::shared_ptr<Action>>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("get_type", &Action::getType)
        .def("set_type", &Action::setType, py::arg("type"))
        .def("get_parameters", &Action::getParameters)
        .def("set_parameters", &Action::setParameters, py::arg("params"))
        .def("get_name", &Action::getName)
        .def("clone", &Action::clone)
        .def("add_parameter", [](Action& self, float value) {
            auto params = self.getParameters();
            params.push_back(value);
            self.setParameters(params);
        }, py::arg("value"),
             "Add a parameter value")
        .def("multiply_parameters", [](Action& self, float factor) {
            auto params = self.getParameters();
            for (auto& val : params) {
                val *= factor;
            }
            self.setParameters(params);
        }, py::arg("factor"),
             "Multiply all parameters by factor")
        .def("normalize_parameters", [](Action& self, float min = 0.0f, float max = 1.0f) {
            auto params = utils::normalize(self.getParameters(), min, max);
            self.setParameters(params);
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f,
             "Normalize parameter values")
        .def("is_valid", [](const Action& self) {
            return !self.getParameters().empty();
        }, "Check if action is valid")
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + " with " + std::to_string(a.getParameters().size()) + " parameters>";
        })
        .def("__enter__", [](Action& self) {
            return &self;
        }, "Context manager entry")
        .def("__exit__", [](Action& self, py::args) {
            // Cleanup if needed
        }, "Context manager exit");

    // Enhanced WorldObject with operator overloads
    py::class_<WorldObject, std::shared_ptr<WorldObject>>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
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
        .def("distance_to", [](const WorldObject& self, const WorldObject& other) {
            float dx = self.x - other.x;
            float dy = self.y - other.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("other"),
             "Calculate distance to another world object")
        .def("is_near", [](const WorldObject& self, const WorldObject& other, float threshold) {
            return distance_to(self, other) <= threshold;
        }, py::arg("other"), py::arg("threshold"),
             "Check if near another world object")
        .def("move_to", [](WorldObject& self, float newX, float newY) {
            self.x = newX;
            self.y = newY;
        }, py::arg("new_x"), py::arg("new_y"),
             "Move object to new position")
        .def("move", [](WorldObject& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
        }, py::arg("dx"), py::arg("dy"),
             "Move object by offset")
        .def("copy", [](WorldObject& self) {
            return WorldObject(self.x, self.y, self.type, self.value, self.radius);
        }, "Copy world object")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + ", " + std::to_string(obj.y) +
                   " type=" + std::to_string(static_cast<int>(obj.type)) + " active=" + std::to_string(obj.active) + ">";
        });

    // Enhanced AgentBody with context manager support
    py::class_<AgentBody, std::shared_ptr<AgentBody>>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x)
        .def_readwrite("y", &AgentBody::y)
        .def_readwrite("orientation", &AgentBody::orientation)
        .def_readwrite("velocity_x", &AgentBody::velocityX)
        .def_readwrite("velocity_y", &AgentBody::velocityY)
        .def_readwrite("angular_velocity", &AgentBody::angularVelocity)
        .def_readwrite("energy", &AgentBody::energy)
        .def_readwrite("health", &AgentBody::health)
        .def_readwrite("age", &AgentBody::age)
        .def_readwrite("is_moving", &AgentBody::isMoving)
        .def_readwrite("is_turning", &AgentBody::isTurning)
        .def_readwrite("last_action_time", &AgentBody::lastActionTime)
        .def("move_to", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
            self.isMoving = true;
        }, py::arg("x"), py::arg("y"),
             "Move agent to position")
        .def("rotate_to", [](AgentBody& self, float orientation) {
            self.orientation = orientation;
            self.isTurning = true;
        }, py::arg("orientation"),
             "Rotate agent to orientation")
        .def("set_velocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("velocity_x"), py::arg("velocity_y"),
             "Set agent velocity")
        .def("update_position", [](AgentBody& self, float dt) {
            self.x += self.velocityX * dt;
            self.y += self.velocityY * dt;
            self.orientation += self.angularVelocity * dt;
        }, py::arg("dt"),
             "Update position based on velocity")
        .def("damage", [](AgentBody& self, float amount) {
            self.health = std::max(0.0f, self.health - amount);
            if (self.health <= 0) {
                self.energy = 0;
            }
        }, py::arg("amount"),
             "Damage agent")
        .def("heal", [](AgentBody& self, float amount) {
            self.health = std::min(100.0f, self.health + amount);
        }, py::arg("amount"),
             "Heal agent")
        .def("reset", &AgentBody::reset,
             "Reset agent body state")
        .def("__enter__", [](AgentBody& self) {
            return &self;
        }, "Context manager entry")
        .def("__exit__", [](AgentBody& self, py::args) {
            // Cleanup if needed
        }, "Context manager exit");

    // Enhanced ActionResult
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("is_positive", [](const ActionResult& self) {
            return self.reward > 0.0f && self.success;
        }, "Check if result is positive")
        .def("is_negative", [](const ActionResult& self) {
            return self.reward < 0.0f && !self.success;
        }, "Check if result is negative")
        .def("to_dict", [](const ActionResult& self) {
            return py::dict("reward", self.reward,
                           "success", self.success,
                           "message", self.message);
        }, "Convert to Python dictionary")
        .def("from_dict", [](ActionResult& self, const py::dict& d) {
            self.reward = py::cast<float>(d["reward"]);
            self.success = py::cast<bool>(d["success"]);
            self.message = py::cast<std::string>(d["message"]);
            return self;
        }, py::arg("dict"),
             "Create ActionResult from dictionary");

    // Enhanced SensoryPercept with context manager support
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
        .def("set_timestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        .def("merge", [](SensoryPercept& self, const SensoryPercept& other) {
            self.setVision(other.getVision());
            self.setTouch(other.getTouch());
            self.setInternal(other.getInternal());
            self.setProprioception(other.getProprioception());
            self.setAudio(other.getAudio());
            self.setTimestamp(other.getTimestamp());
        }, py::arg("other"),
             "Merge with another percept")
        .def("clear", [](SensoryPercept& self) {
            self.setVision(Vision());
            self.setTouch(InternalSignals());
            self.setInternal(InternalSignals());
            self.setProprioception(InternalSignals());
            self.setAudio(Audio());
        }, "Clear all percept data")
        .def("get_summary", [](const SensoryPercept& self) {
            std::stringstream ss;
            ss << "Vision: " << self.getVisionWidth() << "x" << self.getVisionHeight() << "; ";
            ss << "Touch: " << self.getTouch().getSignalCount() << " signals; ";
            ss << "Internal: " << self.getInternal().getSignalCount() << " signals; ";
            ss << "Audio: " << self.getAudio().getNumSamples() << " samples; ";
            ss << "Timestamp: " << self.getTimestamp();
            return ss.str();
        }, "Get human-readable summary")
        .def("__repr__", [](const SensoryPercept& p) {
            return "<SensoryPercept: " + p.get_summary() + ">";
        })
        .def("__enter__", [](SensoryPercept& self) {
            return &self;
        }, "Context manager entry")
        .def("__exit__", [](SensoryPercept& self, py::args) {
            // Cleanup if needed
        }, "Context manager exit");

    // Enhanced SimpleWorld with context manager support
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("vision_width"), py::arg("vision_height"))
        .def("reset", &SimpleWorld::reset)
        .def("set_agent_start", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"))
        .def("update", &SimpleWorld::update, py::arg("timestep"))
        .def("apply_motor_command", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("current_time"))
        .def("get_sensory_percept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        .def("get_agent_body", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        .def("add_object", &SimpleWorld::addObject, py::arg("obj"))
        .def("remove_object", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"))
        .def("is_valid_position", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"))
        .def("get_width", &SimpleWorld::getWidth)
        .def("get_height", &SimpleWorld::getHeight)
        .def("get_max_energy", &SimpleWorld::getMaxEnergy)
        .def("set_max_energy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("get_energy_decay_rate", &SimpleWorld::getEnergyDecayRate)
        .def("set_energy_decay_rate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("get_simulation_time", &SimpleWorld::getSimulationTime)
        .def("set_random_seed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("get_random_seed", &SimpleWorld::getRandomSeed)
        .def("get_object_at", [](const SimpleWorld& self, float x, float y) {
            // Find object at position
            // This is a simplified implementation
            for (const auto& obj : self.getObjects()) {
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (std::sqrt(dx * dx + dy * dy) <= obj.radius) {
                    return obj;
                }
            }
            return WorldObject();
        }, py::arg("x"), py::arg("y"),
             "Get object at position")
        .def("count_objects", [](const SimpleWorld& self, WorldObjectType type) {
            int count = 0;
            for (const auto& obj : self.getObjects()) {
                if (obj.type == type) {
                    count++;
                }
            }
            return count;
        }, py::arg("type"),
             "Count objects of a specific type")
        .def("__repr__", [](const SimpleWorld& w) {
            return "<SimpleWorld: " + std::to_string(w.getWidth()) + "x" + std::to_string(w.getHeight()) +
                   "@t=" + std::to_string(w.getSimulationTime()) + ">";
        })
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        }, "Context manager entry")
        .def("__exit__", [](SimpleWorld& self, py::args) {
            self.reset();
        }, "Context manager exit");

    // Enhanced Brain with context manager support
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
             py::arg("id"),
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
        .def("simulate_episode", [](Brain& self, int steps) {
            for (int i = 0; i < steps; ++i) {
                SimulationStep step = i;
                self.step(step);
            }
        }, py::arg("steps"),
             "Simulate an episode for multiple steps")
        .def("__repr__", [](const Brain& b) {
            return "<Brain: " + std::to_string(b.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(b.getTotalSynapseCount()) + " synapses>";
        })
        .def("__enter__", [](Brain& self) {
            return &self;
        }, "Context manager entry")
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        }, "Context manager exit");

    // Enhanced AgentBrain with context manager support
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
             py::arg("reward"), py::arg("predicted_reward"),
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
        .def("get_simulation_stats", [](const AgentBrain& self) {
            return py::dict("developmental_stage", static_cast<int>(self.getDevelopmentalStage()),
                           "neuromodulation_level", self.getNeuromodulationLevel(),
                           "curiosity_level", self.getCuriosityLevel(),
                           "novelty_level", self.getNoveltyLevel(),
                           "prediction_error", self.getPredictionError());
        }, "Get simulation statistics as dictionary")
        .def("run_episode", [](AgentBrain& self, Brain& brain, const SensoryPercept& initialPercept, int steps) {
            brain.initialize();
            for (int i = 0; i < steps; ++i) {
                brain.step(i);
                brain.receiveSensoryInput(initialPercept);
            }
        }, py::arg("brain"), py::arg("initial_percept"), py::arg("steps"),
             "Run a simulation episode")
        .def("__repr__", [](const AgentBrain& a) {
            return "<AgentBrain: " + std::to_string(a.getNeuromodulationLevel()) + ">";
        })
        .def("__enter__", [](AgentBrain& self) {
            return &self;
        }, "Context manager entry")
        .def("__exit__", [](AgentBrain& self, py::args) {
            self.reset();
        }, "Context manager exit");

    // Module-level functions
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neural.region_count", "5");
        config->set("neural.neurons_per_region", "100");
        config->set("neural.connection_density", "0.1");
        config->set("neural.learning_rate", "0.01");
        config->set("neural.decay_rate", "0.995");
        config->set("world.width", "100");
        config->set("world.height", "100");
        config->set("world.vision_width", "20");
        config->set("world.vision_height", "20");
        config->set("world.max_energy", "100.0");
        config->set("agent.max_energy", "100.0");
        config->set("agent.health", "100.0");
        config->set("agent.energy_decay_rate", "0.1");
        return config;
    }, "Create a default configuration with sensible values");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100, 100, 20, 20);
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.1f);
        return world;
    }, "Create a new simple world with default configuration");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("quick_start", []() {
        auto config = create_default_config();
        auto brain = create_brain(config);
        auto world = create_simple_world();
        auto agentBrain = create_agent_brain(brain);
        
        agentBrain->initialize(*world);
        brain->initialize();
        
        return py::make_tuple(config, brain, world, agentBrain);
    }, "Quick start helper function");

    m.def("version", &utils::version,
         "Get the version of the bindings");

    m.def("get_system_info", &utils::get_system_info,
         "Get system information");

    m.def("current_time", &utils::currentTime,
         "Get current time in seconds");

    m.def("normalize", [](const std::vector<float>& input, float min = 0.0f, float max = 1.0f) {
        return utils::normalize(input, min, max);
    }, py::arg("input"), py::arg("min") = 0.0f, py::arg("max") = 1.0f,
         "Normalize array values to range [min, max]");

    m.def("array_to_string", &utils::arrayToString,
         "Convert array to human-readable string");

    m.def("make_tuple", [](std::shared_ptr<Config> config, std::shared_ptr<Brain> brain,
                         std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agentBrain) {
        return py::make_tuple(config, brain, world, agentBrain);
    }, py::arg("config"), py::arg("brain"), py::arg("world"), py::arg("agent_brain"),
         "Create a tuple of NLM components");

    // Expose constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module documentation
    m.def("__doc__", []() {
        return R"pbdoc(
            Enhanced NLM Python Bindings
            -----------------------------
            
            This enhanced version provides:
            
            1. **Context Managers**: All major classes support context manager protocol
            2. **Operator Overloads**: Vision and WorldObject classes support + and * operators
            3. **Pythonic Naming**: Methods use snake_case instead of CamelCase where appropriate
            4. **Better Error Handling**: Custom exception classes for different error types
            5. **ConfigBuilder**: Easy configuration setup with fluent API
            6. **Iterators**: Support for iterating over Config and InternalSignals
            7. **Utility Functions**: Common operations like normalization, array formatting
            
            Key Features:
            - Neural simulation with neural networks
            - Agent behavior and world interaction  
            - Sensory processing and motor control
            - Reward-based learning and neuromodulation
            - Developmental stages and plasticity
            - Configuration validation and defaults
            
            Usage Example:
            
            ```python
            import pynlm
            
            # Quick start
            config, brain, world, agent_brain = pynlm.quick_start()
            
            # Create a custom configuration
            config = pynlm.ConfigBuilder().set_neural_parameters(
                region_count=10,
                neurons_per_region=200
            ).set_world_parameters(width=200, height=200).create()
            
            # Use context managers for resource cleanup
            with pynlm.Brain(config) as brain:
                brain.initialize()
                brain.step(0)
            ```
            
            For detailed documentation, visit: https://github.com/neuroml/pynlm
        )pbdoc";
    });

    return m;
}

} // namespace nlm