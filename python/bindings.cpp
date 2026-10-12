#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <stdexcept>
#include <sstream>
#include <algorithm>
#include <cmath>

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

// Enhanced exception hierarchy
class PyNLMException : public std::runtime_error {
public:
    PyNLMException(const std::string& msg) : std::runtime_error(msg) {}
    virtual const char* what() const noexcept override {
        return std::runtime_error::what();
    }
};

class ValidationError : public PyNLMException {
public:
    ValidationError(const std::string& msg) : PyNLMException("Validation error: " + msg) {}
};

class InitializationError : public PyNLMException {
public:
    InitializationError(const std::string& msg) : PyNLMException("Brain initialization failed: " + msg) {}
};

class ConfigurationError : public PyNLMException {
public:
    ConfigurationError(const std::string& msg) : PyNLMException("Configuration error: " + msg) {}
};

class RuntimeError : public PyNLMException {
public:
    RuntimeError(const std::string& msg) : PyNLMException("Runtime error: " + msg) {}
};

class SimulationError : public PyNLMException {
public:
    SimulationError(const std::string& msg) : PyNLMException("Simulation error: " + msg) {}
};

// Validation helper functions
inline void validateNeuronCount(size_t neuronCount) {
    if (neuronCount == 0) {
        throw ValidationError("Neuron count must be greater than 0");
    }
    if (neuronCount > 1000000) {  // Reasonable upper limit
        throw ValidationError("Neuron count too large (max: 1,000,000)");
    }
}

inline void validateRegionCount(size_t regionCount) {
    if (regionCount == 0) {
        throw ValidationError("Region count must be greater than 0");
    }
}

inline void validateStepCount(uint64_t steps) {
    if (steps == 0) {
        throw ValidationError("Step count must be greater than 0");
    }
    if (steps > 1000000) {
        throw ValidationError("Step count too large (max: 1,000,000)");
    }
}

inline void validateConfigValue(const std::string& key, double value, double min = -std::numeric_limits<double>::infinity(), 
                               double max = std::numeric_limits<double>::infinity()) {
    if (value < min || value > max) {
        throw ValidationError("Configuration value for '" + key + "' must be between " + 
                           std::to_string(min) + " and " + std::to_string(max));
    }
}

// Context manager with enhanced error handling
class BrainScope {
public:
    BrainScope(std::shared_ptr<Brain> brain) : brain_(brain), initialized_(false) {
        if (!brain_) {
            throw ValidationError("Cannot create BrainScope with null brain pointer");
        }
        try {
            brain_->initialize();
            initialized_ = true;
        } catch (const std::exception& e) {
            throw InitializationError(std::string("Failed to initialize brain in scope: ") + e.what());
        }
    }
    
    ~BrainScope() {
        if (brain_ && initialized_) {
            try {
                brain_->reset();
            } catch (...) {
                // Ignore errors during cleanup
            }
        }
    }
    
    Brain* get() { return brain_.get(); }
    Brain* operator->() { return brain_.get(); }
    
    bool isInitialized() const { return initialized_; }
    
private:
    std::shared_ptr<Brain> brain_;
    bool initialized_;
};

// Enhanced SensoryInput validation
class ValidatedVision : public Vision {
public:
    ValidatedVision() : Vision() {}
    ValidatedVision(size_t width, size_t height, size_t channels) : Vision(width, height, channels) {
        validateVisionParameters(width, height, channels);
    }
    
    static void validateVisionParameters(size_t width, size_t height, size_t channels) {
        if (width == 0 || height == 0) {
            throw ValidationError("Vision dimensions must be greater than 0");
        }
        if (width > 10000 || height > 10000) {
            throw ValidationError("Vision dimensions too large (max: 10000x10000)");
        }
        if (channels == 0 || channels > 10) {
            throw ValidationError("Vision channels must be between 1 and 10");
        }
    }
    
    void validateData(const std::vector<float>& data) const {
        if (data.empty()) {
            throw ValidationError("Vision data cannot be empty");
        }
        size_t expectedSize = getWidth() * getHeight() * getChannels();
        if (data.size() != expectedSize) {
            throw ValidationError("Vision data size mismatch: expected " + std::to_string(expectedSize) +
                               " elements, got " + std::to_string(data.size()));
        }
    }
};

// Context manager for world simulation with validation
class WorldScope {
public:
    WorldScope(std::shared_ptr<SimpleWorld> world) : world_(world), reset_(false) {
        if (!world_) {
            throw ValidationError("Cannot create WorldScope with null world pointer");
        }
        try {
            world_->reset();
            reset_ = true;
        } catch (const std::exception& e) {
            throw RuntimeError(std::string("Failed to reset world in scope: ") + e.what());
        }
    }
    
    ~WorldScope() {
        if (world_ && reset_) {
            try {
                world_->reset();
            } catch (...) {
                // Ignore errors during cleanup
            }
        }
    }
    
    SimpleWorld* get() { return world_.get(); }
    SimpleWorld* operator->() { return world_.get(); }
    
    bool isReset() const { return reset_; }
    
private:
    std::shared_ptr<SimpleWorld> world_;
    bool reset_;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        ============================================
        
        Enhanced Python bindings for the NLM C++ neural simulation framework with:
        - Comprehensive error handling and validation
        - Enhanced exception hierarchy
        - Context managers for resource management
        - Input validation and sanitization
        - Detailed error messages and debugging support
        
        Usage example:
        ```python
        import pynlm
        
        # Create a brain with validation
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Use context manager for safe resource management
        with brain.asContextManager() as scope:
            for step in range(1000):
                scope.step(step)
                
        # Save results with validation
        brain.save("validated_brain.bin")
        ```
    )pbdoc";
    
    // Enhanced exception registration
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<PyNLMException>(m, "NLMException");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<RuntimeError>(m, "RuntimeError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Neuron ID type with validation
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
        .def("validate", [](const NeuronId& id) {
            if (id.value == 0) {
                throw ValidationError("Neuron ID value cannot be 0");
            }
            return true;
        }, "Validate neuron ID");

    // Synapse ID type with validation
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
        .def("validate", [](const SynapseId& id) {
            if (id.value == 0) {
                throw ValidationError("Synapse ID value cannot be 0");
            }
            return true;
        }, "Validate synapse ID");

    // Enhanced Region ID type with validation
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
        .def("validate", [](const RegionId& id) {
            if (id.value == 0) {
                throw ValidationError("Region ID value cannot be 0");
            }
            return true;
        }, "Validate region ID");

    // Enhanced Population ID type with validation
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("validate", [](const PopulationId& id) {
            if (id.value == 0) {
                throw ValidationError("Population ID value cannot be 0");
            }
            return true;
        }, "Validate population ID");

    // Neuron type enumeration
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse type enumeration
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stage enumeration
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing state enumeration
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Enhanced Config class with validation
    py::class_<Config>(m, "Config", R"pbdoc(Validated configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                self.loadFromFile(filepath);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to load configuration from file: ") + e.what());
            }
        }, py::arg("filepath"), "Load configuration from a JSON file with validation")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to load configuration from arguments: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"), "Load configuration from command line arguments with validation")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                self.saveToFile(filepath);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to save configuration to file: ") + e.what());
            }
        }, py::arg("filepath"), "Save configuration to a JSON file with validation")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](Config& self, const std::string& key, const std::string& default_value) {
            try {
                return self.get<std::string>(key).value_or(default_value);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to get configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("default_value") = "",
           "Get configuration value as string with default, with validation")
        .def("getInt", [](Config& self, const std::string& key, int64_t default_value) {
            try {
                return self.get<int64_t>(key).value_or(default_value);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to get int configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("default_value") = 0,
           "Get configuration value as int with default, with validation")
        .def("getFloat", [](Config& self, const std::string& key, float default_value) {
            try {
                return self.get<float>(key).value_or(default_value);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to get float configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("default_value") = 0.0f,
           "Get configuration value as float with default, with validation")
        .def("getBool", [](Config& self, const std::string& key, bool default_value) {
            try {
                return self.get<bool>(key).value_or(default_value);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to get bool configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("default_value") = false,
           "Get configuration value as bool with default, with validation")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            try {
                // Validate common configuration keys
                if (key == "brain.neuron_count") {
                    validateNeuronCount(std::stoull(value));
                } else if (key == "brain.region_count") {
                    validateRegionCount(std::stoull(value));
                } else if (key == "brain.connection_probability") {
                    float val = std::stof(value);
                    validateConfigValue(key, val, 0.0f, 1.0f);
                }
                
                self.set(key, value);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to set configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value with validation")
        .def("setInt", [](Config& self, const std::string& key, int64_t value) {
            try {
                if (key == "brain.neuron_count") {
                    validateNeuronCount(value);
                } else if (key == "brain.region_count") {
                    validateRegionCount(value);
                }
                
                self.set(key, value);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to set int configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value as int with validation")
        .def("setFloat", [](Config& self, const std::string& key, float value) {
            try {
                if (key == "brain.connection_probability") {
                    validateConfigValue(key, value, 0.0f, 1.0f);
                }
                
                self.set(key, value);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to set float configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value as float with validation")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            try {
                self.set(key, value);
            } catch (const std::exception& e) {
                throw ConfigurationError(std::string("Failed to set bool configuration value for key '") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("value"),
           "Set configuration value as bool with validation")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("validate", [](Config& self) {
            // Basic validation
            if (self.has("brain.neuron_count")) {
                size_t neuronCount = self.getInt("brain.neuron_count", 1000);
                validateNeuronCount(neuronCount);
            }
            if (self.has("brain.region_count")) {
                size_t regionCount = self.getInt("brain.region_count", 1);
                validateRegionCount(regionCount);
            }
            return true;
        }, "Validate configuration parameters")
        .def("__repr__", [](const Config& cfg) {
            return "<ValidatedConfig: " + cfg.summary() + ">";
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for validated sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Enhanced Vision sensory input with validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Validated vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                // Cast to validated vision
                ValidatedVision& validatedVision = reinterpret_cast<ValidatedVision&>(self);
                validatedVision.validateData(data);
                self.setData(data);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to set vision data: ") + e.what());
            }
        }, py::arg("data"), "Set vision data with validation")
        .def("validateData", [](Vision& self, const std::vector<float>& data) {
            ValidatedVision& validatedVision = reinterpret_cast<ValidatedVision&>(self);
            validatedVision.validateData(data);
            return true;
        }, py::arg("data"), "Validate vision data")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("setResolution", [](Vision& self, size_t width, size_t height, size_t channels) {
            ValidatedVision::validateVisionParameters(width, height, channels);
            self.setResolution(width, height, channels);
        }, py::arg("width"), py::arg("height"), py::arg("channels") = 3,
           "Set vision sensor resolution with validation");

    // Enhanced Audio sensory input with validation
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Validated audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw ValidationError("Audio data cannot be empty");
            }
            // Validate audio parameters
            size_t sampleRate = self.getSampleRate();
            size_t numSamples = self.getNumSamples();
            if (data.size() != numSamples) {
                throw ValidationError("Audio data size mismatch: expected " + std::to_string(numSamples) +
                                   " samples, got " + std::to_string(data.size()));
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate with validation")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("validate", [](Audio& self) {
            if (self.getSampleRate() == 0) {
                throw ValidationError("Audio sample rate cannot be 0");
            }
            if (self.getNumSamples() == 0) {
                throw ValidationError("Audio sample count cannot be 0");
            }
            return true;
        }, "Validate audio parameters");

    // Enhanced InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Validated internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", [](InternalSignals& self, float value) {
            if (std::isnan(value) || std::isinf(value)) {
                throw ValidationError("Invalid signal value: " + std::to_string(value));
            }
            self.addSignal(value);
        }, py::arg("value"), "Add signal value with validation")
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getNumSignals", &InternalSignals::getNumSignals)
        .def("validate", [](InternalSignals& self) {
            // Validate internal signals
            auto signals = self.getAllSignals();
            for (float value : signals) {
                if (std::isnan(value) || std::isinf(value)) {
                    throw ValidationError("Invalid signal value in internal signals");
                }
            }
            return true;
        }, "Validate internal signals");

    // Enhanced Action class with validation
    py::class_<Action>(m, "Action", R"pbdoc(Validated action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            if (params.size() > 1000) {
                throw ValidationError("Action parameters too large (max: 1000)");
            }
            for (float value : params) {
                if (std::isnan(value) || std::isinf(value)) {
                    throw ValidationError("Invalid parameter value in action parameters");
                }
            }
            self.setParameters(params);
        }, py::arg("params"), "Set action parameters with validation")
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("validate", [](const Action& action) {
            if (action.getType() == ActionType::Custom && action.getParameters().empty()) {
                throw ValidationError("Custom action must have parameters");
            }
            return true;
        }, "Validate action")
        .def("toDict", [](const Action& action) {
            py::dict d;
            d["type"] = action.getType();
            d["parameters"] = action.getParameters();
            d["name"] = action.getName();
            return d;
        }, "Convert action to dictionary");

    // Enhanced WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(Validated world object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def("validate", [](WorldObject& obj) {
            if (obj.x < 0 || obj.y < 0) {
                throw ValidationError("World object position must be non-negative");
            }
            if (obj.radius <= 0) {
                throw ValidationError("World object radius must be greater than 0");
            }
            return true;
        }, "Validate world object")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        .def("toDict", [](const WorldObject& obj) {
            py::dict d;
            d["x"] = obj.x;
            d["y"] = obj.y;
            d["radius"] = obj.radius;
            d["type"] = obj.type;
            d["value"] = obj.value;
            d["active"] = obj.active;
            return d;
        }, "Convert world object to dictionary");

    // Enhanced AgentBody class with validation
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Validated agent body state)pbdoc")
        .def(py::init<>())
        .def("validate", [](AgentBody& body) {
            if (body.x < 0 || body.y < 0) {
                throw ValidationError("Agent body position must be non-negative");
            }
            if (body.orientation < 0 || body.orientation >= 360) {
                throw ValidationError("Agent body orientation must be between 0 and 360 degrees");
            }
            if (body.energy < 0 || body.energy > 1000) {
                throw ValidationError("Agent body energy must be between 0 and 1000");
            }
            if (body.health < 0 || body.health > 100) {
                throw ValidationError("Agent body health must be between 0 and 100");
            }
            return true;
        }, "Validate agent body state")
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
        .def("toDict", [](const AgentBody& body) {
            py::dict d;
            d["x"] = body.x;
            d["y"] = body.y;
            d["orientation"] = body.orientation;
            d["velocityX"] = body.velocityX;
            d["velocityY"] = body.velocityY;
            d["angularVelocity"] = body.angularVelocity;
            d["energy"] = body.energy;
            d["health"] = body.health;
            d["age"] = body.age;
            d["isMoving"] = body.isMoving;
            d["isTurning"] = body.isTurning;
            d["lastActionTime"] = body.lastActionTime;
            return d;
        }, "Convert agent body to dictionary");

    // Enhanced ActionResult class with validation
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Validated action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def("validate", [](ActionResult& result) {
            if (std::isnan(result.reward) || std::isinf(result.reward)) {
                throw ValidationError("Action reward must be a finite number");
            }
            if (result.reward < -1000 || result.reward > 1000) {
                throw ValidationError("Action reward value out of reasonable range (-1000 to 1000)");
            }
            return true;
        }, "Validate action result")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("toDict", [](const ActionResult& result) {
            py::dict d;
            d["reward"] = result.reward;
            d["success"] = result.success;
            d["message"] = result.message;
            return d;
        }, "Convert action result to dictionary");

    // Enhanced SensoryPercept class with validation
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Validated sensory percept data)pbdoc")
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
        .def("validate", [](SensoryPercept& percept) {
            // Validate all sensory inputs
            auto vision = percept.getVision();
            if (vision && vision->getWidth() > 0 && vision->getHeight() > 0) {
                vision->validate();
            }
            
            auto audio = percept.getAudio();
            if (audio) {
                audio->validate();
            }
            
            auto internal = percept.getInternal();
            if (internal) {
                internal->validate();
            }
            
            return true;
        }, "Validate sensory percept")
        .def("toDict", [](const SensoryPercept& percept) {
            py::dict d;
            d["vision"] = percept.getVision();
            d["touch"] = percept.getTouch();
            d["internal"] = percept.getInternal();
            d["proprioception"] = percept.getProprioception();
            d["audio"] = percept.getAudio();
            d["timestamp"] = percept.getTimestamp();
            return d;
        }, "Convert sensory percept to dictionary");

    // Enhanced SimpleWorld class with validation
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Validated simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                            size_t visionWidth, size_t visionHeight) {
            if (width == 0 || height == 0) {
                throw ValidationError("World dimensions must be greater than 0");
            }
            if (visionWidth == 0 || visionHeight == 0) {
                throw ValidationError("World vision dimensions must be greater than 0");
            }
            if (visionWidth > width || visionHeight > height) {
                throw ValidationError("World vision dimensions cannot exceed world dimensions");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"),
           py::arg("visionWidth"), py::arg("visionHeight"),
           "Configure world dimensions and vision sensor with validation")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", [](SimpleWorld& self, float x, float y) {
            if (x < 0 || y < 0) {
                throw ValidationError("Agent start position must be non-negative");
            }
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"), "Set agent starting position with validation")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
        .def("applyMotorCommand", [](SimpleWorld& self, const MotorCommand& cmd, float currentTime) {
            if (currentTime < 0) {
                throw ValidationError("Current time must be non-negative");
            }
            self.applyMotorCommand(cmd, currentTime);
        }, py::arg("cmd"), py::arg("currentTime"),
           "Apply motor command to world with validation")
        .def("getSensoryPercept", [](SimpleWorld& self) {
            auto percept = self.getSensoryPercept();
            percept->validate();
            return percept;
        }, py::return_value_policy::reference_internal,
           "Get validated sensory percept from world")
        .def("getAgentBody", [](SimpleWorld& self) {
            auto body = self.getAgentBody();
            body->validate();
            return body;
        }, py::return_value_policy::reference_internal,
           "Get validated agent body state")
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            obj.validate();
            self.addObject(obj);
        }, py::arg("obj"), "Add world object with validation")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position")
        .def("isValidPosition", [](SimpleWorld& self, float x, float y) {
            if (x < 0 || y < 0) {
                return false;
            }
            if (x >= self.getWidth() || y >= self.getHeight()) {
                return false;
            }
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
           "Validate position and check if valid in world")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("validate", [](SimpleWorld& self) {
            // Validate world state
            if (self.getWidth() == 0 || self.getHeight() == 0) {
                throw ValidationError("World must have valid dimensions");
            }
            if (self.getMaxEnergy() <= 0) {
                throw ValidationError("World max energy must be greater than 0");
            }
            if (self.getEnergyDecayRate() < 0 || self.getEnergyDecayRate() > 1) {
                throw ValidationError("World energy decay rate must be between 0 and 1");
            }
            return true;
        }, "Validate world state")
        .def("toDict", [](const SimpleWorld& world) {
            py::dict d;
            d["width"] = world.getWidth();
            d["height"] = world.getHeight();
            d["visionWidth"] = world.getVisionWidth();
            d["visionHeight"] = world.getVisionHeight();
            d["maxEnergy"] = world.getMaxEnergy();
            d["energyDecayRate"] = world.getEnergyDecayRate();
            d["simulationTime"] = world.getSimulationTime();
            d["randomSeed"] = world.getRandomSeed();
            return d;
        }, "Convert world to dictionary");

    // Enhanced Brain class with validation
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Validated central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            try {
                bool success = self.initialize();
                if (!success) {
                    throw InitializationError("Brain initialization returned false");
                }
                return success;
            } catch (const std::exception& e) {
                throw InitializationError(std::string("Brain initialization failed: ") + e.what());
            }
        }, "Initialize the brain with configuration and validation")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            try {
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to receive sensory input: ") + e.what());
            }
        }, py::arg("input"), "Inject sensory input into the brain with validation")
        .def("injectCurrent", [](Brain& self, NeuronId neuron, MembranePotential current) {
            if (std::isnan(current) || std::isinf(current)) {
                throw ValidationError("Membrane potential current must be a finite number");
            }
            self.injectCurrent(neuron, current);
        }, py::arg("neuron"), py::arg("current"),
           "Inject current into a specific neuron with validation")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, MembranePotential current) {
            if (std::isnan(current) || std::isinf(current)) {
                throw ValidationError("Membrane potential current must be a finite number");
            }
            self.injectCurrentToNeurons(type, current);
        }, py::arg("type"), py::arg("current"),
           "Inject current into all neurons of a specific type with validation")
        .def("produceAction", [](Brain& self) {
            try {
                return self.produceAction();
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to produce action: ") + e.what());
            }
        }, "Produce motor action based on neural activity with validation")
        .def("reset", [](Brain& self) {
            try {
                self.reset();
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to reset brain: ") + e.what());
            }
        }, "Reset brain state with validation")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                bool success = self.save(filepath);
                if (!success) {
                    throw RuntimeError("Failed to save brain state");
                }
                return success;
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to save brain: ") + e.what());
            }
        }, py::arg("filepath"), "Save brain state to file with validation")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                bool success = self.load(filepath);
                if (!success) {
                    throw RuntimeError("Failed to load brain state");
                }
                return success;
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to load brain: ") + e.what());
            }
        }, py::arg("filepath"), "Load brain state from file with validation")
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
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("validate", [](Brain& self) {
            // Validate brain state
            size_t neuronCount = self.getTotalNeuronCount();
            validateNeuronCount(neuronCount);
            
            size_t regionCount = self.getRegionCount();
            validateRegionCount(regionCount);
            
            // Validate configuration
            auto config = self.getConfig();
            config->validate();
            
            return true;
        }, "Validate brain state")
        .def("toDict", [](const Brain& brain) {
            py::dict d;
            d["totalNeurons"] = brain.getTotalNeuronCount();
            d["totalSynapses"] = brain.getTotalSynapseCount();
            d["firingNeurons"] = brain.getFiringNeuronCount();
            d["activeNeurons"] = brain.getActiveNeuronCount();
            d["avgFiringRate"] = brain.getAverageFiringRate();
            d["eIratio"] = brain.getExcitationInhibitionRatio();
            d["totalSpikes"] = brain.getTotalSpikeCount();
            d["developmentStage"] = brain.getDevelopmentalStage();
            return d;
        }, "Convert brain to dictionary")
        .def("__enter__", [](py::object self) {
            auto brain = self.cast<std::shared_ptr<Brain>>();
            return BrainScope(brain);
        })
        .def("__exit__", [](BrainScope& scope, py::args) {
            // Cleanup handled by scope destructor
        })
        .def("asContextManager", [](std::shared_ptr<Brain> brain) {
            return BrainScope(brain);
        }, "Return a context manager for automatic brain management with validation");

    // Enhanced AgentBrain class with validation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Validated agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            try {
                // Validate percept before processing
                percept.validate();
                self.processSensoryInput(percept);
            } catch (const ValidationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to process sensory input: ") + e.what());
            }
        }, py::arg("percept"), "Process sensory percept and inject into brain with validation")
        .def("decodeMotorCommand", [](AgentBrain& self) {
            try {
                return self.decodeMotorCommand();
            } catch (const std::exception& e) {
                throw RuntimeError(std::string("Failed to decode motor command: ") + e.what());
            }
        }, "Decode brain motor activity into motor command with validation")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            if (std::isnan(reward) || std::isinf(reward)) {
                throw ValidationError("Reward must be a finite number");
            }
            if (std::isnan(predictedReward) || std::isinf(predictedReward)) {
                throw ValidationError("Predicted reward must be a finite number");
            }
            self.applyRewardModulation(reward, predictedReward);
        }, py::arg("reward"), py::arg("predictedReward"),
           "Apply reward-based neuromodulation with validation")
        .def("updateDevelopment", [](AgentBrain& self, float timestep) {
            if (timestep < 0) {
                throw ValidationError("Time step must be non-negative");
            }
            self.updateDevelopment(timestep);
        }, py::arg("timestep"), "Update development system with validation")
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
        .def("validate", [](AgentBrain& self) {
            // Validate agent brain state
            auto brain = self.getBrain();
            brain->validate();
            
            // Validate neuromodulation levels
            if (self.getCuriosityLevel() < 0 || self.getCuriosityLevel() > 1) {
                throw ValidationError("Curiosity level must be between 0 and 1");
            }
            if (self.getNoveltyLevel() < 0 || self.getNoveltyLevel() > 1) {
                throw ValidationError("Novelty level must be between 0 and 1");
            }
            
            return true;
        }, "Validate agent brain state")
        .def("toDict", [](const AgentBrain& agent) {
            py::dict d;
            d["rewardModulation"] = agent.isRewardModulationEnabled();
            d["structuralPlasticity"] = agent.isStructuralPlasticityEnabled();
            d["development"] = agent.isDevelopmentEnabled();
            d["curiosity"] = agent.isCuriosityEnabled();
            d["curiosityLevel"] = agent.getCuriosityLevel();
            d["noveltyLevel"] = agent.getNoveltyLevel();
            d["predictionError"] = agent.getPredictionError();
            return d;
        }, "Convert agent brain to dictionary");

    // Enhanced factory functions with validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        try {
            config->validate();
        } catch (const std::exception& e) {
            throw ConfigurationError(std::string("Failed to validate default config: ") + e.what());
        }
        return config;
    }, "Create a default configuration with validation", py::return_value_policy::reference_internal);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            throw ValidationError("Config cannot be null");
        }
        
        auto brain = std::make_shared<Brain>(config);
        try {
            bool success = brain->initialize();
            if (!success) {
                throw InitializationError("Brain initialization failed");
            }
            
            brain->validate();
        } catch (const std::exception& e) {
            throw InitializationError(std::string("Failed to create and initialize brain: ") + e.what());
        }
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration and validation", py::return_value_policy::reference_internal);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        try {
            world->validate();
        } catch (const std::exception& e) {
            throw RuntimeError(std::string("Failed to create and validate world: ") + e.what());
        }
        return world;
    }, "Create a new simple world with validation", py::return_value_policy::reference_internal);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw ValidationError("Brain cannot be null");
        }
        
        auto agent = std::make_shared<AgentBrain>(brain);
        try {
            agent->validate();
        } catch (const std::exception& e) {
            throw RuntimeError(std::string("Failed to create and validate agent brain: ") + e.what());
        }
        return agent;
    }, py::arg("brain"), "Create a new agent brain interface with validation", py::return_value_policy::reference_internal);

    // Enhanced utility functions with validation
    m.def("create_brain_scope", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw ValidationError("Cannot create BrainScope with null brain pointer");
        }
        return BrainScope(brain);
    }, "Create a context manager for automatic brain resource management with validation", py::return_value_policy::reference_internal);

    m.def("create_world_scope", [](std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw ValidationError("Cannot create WorldScope with null world pointer");
        }
        return WorldScope(world);
    }, "Create a context manager for automatic world resource management with validation", py::return_value_policy::reference_internal);

    // Enhanced error helper functions
    m.def("validate_simulation_parameters", [](uint64_t steps, size_t neurons, size_t regions) {
        validateStepCount(steps);
        validateNeuronCount(neurons);
        validateRegionCount(regions);
        return true;
    }, py::arg("steps"), py::arg("neurons"), py::arg("regions"),
       "Validate simulation parameters");

    // Version and metadata
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced NLM (Neural Learning Machine) Python Bindings with validation";

    // Enhanced error classes for user consumption
    py::class_<PyNLMException>(m, "NLMException", py::module_local())
        .def(py::init<const std::string&>())
        .def("what", &PyNLMException::what);

    py::class_<ValidationError>(m, "ValidationError", py::module_local())
        .def(py::init<const std::string&>())
        .def("what", &ValidationError::what);

    py::class_<InitializationError>(m, "InitializationError", py::module_local())
        .def(py::init<const std::string&>())
        .def("what", &InitializationError::what);

    py::class_<ConfigurationError>(m, "ConfigurationError", py::module_local())
        .def(py::init<const std::string&>())
        .def("what", &ConfigurationError::what);

    py::class_<RuntimeError>(m, "RuntimeError", py::module_local())
        .def(py::init<const std::string&>())
        .def("what", &RuntimeError::what);

    py::class_<SimulationError>(m, "SimulationError", py::module_local())
        .def(py::init<const std::string&>())
        .def("what", &SimulationError::what);
}

} // namespace nlm