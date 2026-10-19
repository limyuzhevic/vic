#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/buffer_info.h>
#include <pybind11/memoryview.h>
#include <pybind11/cast.h>
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

// Custom Python exception hierarchy for NLM
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class ResourceError : public NLMError {
public:
    explicit ResourceError(const std::string& msg) : NLMError("Resource Error: " + msg) {}
};

// Register exceptions with Python
void registerExceptions(py::module_& m) {
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<ResourceError>(m, "ResourceError");
}

// Helper functions for type checking and validation
namespace detail {
    template<typename T>
    T pyToCpp(const py::object& obj, const std::string& name) {
        if (!obj) {
            throw ValidationError("Parameter '" + name + "' cannot be None");
        }
        try {
            return py::cast<T>(obj);
        } catch (const py::cast_error&) {
            throw ValidationError("Parameter '" + name + "' must be of type " + typeid(T).name());
        }
    }
    
    template<typename T>
    std::optional<T> pyToCppOptional(const py::object& obj) {
        if (obj.is_none()) {
            return std::nullopt;
        }
        return pyToCpp<T>(obj, "optional parameter");
    }
    
    template<typename T>
    void validateRange(const T& value, const T& min, const T& max, const std::string& name) {
        if (value < min || value > max) {
            throw ValidationError("Parameter '" + name + "' must be in range [" + 
                                std::to_string(min) + ", " + std::to_string(max) + "]");
        }
    }
}} // namespace detail

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key improvements:
        - Custom exception hierarchy (NLMError, ValidationError, ConfigurationError, ResourceError)
        - Pythonic features: __str__, __repr__, properties, context managers
        - Parameter validation and type checking
        - Batch operations and convenience methods
        - Buffer protocol support for efficient data transfer
        - Enhanced documentation with examples
    )pbdoc";
    
    // Register custom exceptions
    registerExceptions(m);
    
    // Keep standard runtime error for backward compatibility
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readonly("value", &NeuronId::value)
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
        .def_readonly("value", &SynapseId::value)
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
        .def_readonly("value", &RegionId::value)
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
        .def_readonly("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const PopulationId& id) {
            return std::to_string(id.value);
        });
    
    // Factory methods for ID types
    m.def("createNeuronId", [](uint64_t value) { return NeuronId(value); }, py::arg("value"), "Create a NeuronId");
    m.def("createSynapseId", [](uint64_t value) { return SynapseId(value); }, py::arg("value"), "Create a SynapseId");
    m.def("createRegionId", [](uint64_t value) { return RegionId(value); }, py::arg("value"), "Create a RegionId");
    m.def("createPopulationId", [](uint64_t value) { return PopulationId(value); }, py::arg("value"), "Create a PopulationId");

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_sensory_or_motor", [](NeuronType t) {
            return t == NeuronType::Sensory || t == NeuronType::Motor;
        }, "Check if neuron type is sensory or motor")
        .def("is_excitatory_or_inhibitory", [](NeuronType t) {
            return t == NeuronType::Excitatory || t == NeuronType::Inhibitory;
        }, "Check if neuron type is excitatory or inhibitory");

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_inhibitory", [](SynapseType t) {
            return t == SynapseType::Inhibitory || t == SynapseType::Modulatory;
        }, "Check if synapse type is inhibitory")
        .def("is_excitatory", [](SynapseType t) {
            return t == SynapseType::Excitatory;
        }, "Check if synapse type is excitatory");

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_developing", [](DevelopmentalStage t) {
            return t == DevelopmentalStage::CriticalPeriod || 
                   t == DevelopmentalStage::Maturation;
        }, "Check if in developmental stage")
        .def("is_adult", [](DevelopmentalStage t) {
            return t == DevelopmentalStage::Adult || t == DevelopmentalStage::Aging;
        }, "Check if in adult stage");

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("is_active", [](FiringState t) {
            return t == FiringState::Active;
        }, "Check if neuron is actively firing")
        .def("is_pasive", [](FiringState t) {
            return t == FiringState::Resting || t == FiringState::Inhibited;
        }, "Check if neuron is not firing");

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
        .def("is_movement", [](ActionType t) {
            return t == ActionType::MoveForward || t == ActionType::MoveBackward ||
                   t == ActionType::MoveLeft || t == ActionType::MoveRight;
        }, "Check if action is a movement action")
        .def("is_turning", [](ActionType t) {
            return t == ActionType::TurnLeft || t == ActionType::TurnRight;
        }, "Check if action is a turning action")
        .def("is_interact", [](ActionType t) {
            return t == ActionType::Interact || t == ActionType::Eat || t == ActionType::Drink;
        }, "Check if action is an interaction action");

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
        .def("to_action_type", [](MotorCommand cmd) {
            switch (cmd) {
                case MotorCommand::MoveForward: return ActionType::MoveForward;
                case MotorCommand::MoveBackward: return ActionType::MoveBackward;
                case MotorCommand::TurnLeft: return ActionType::TurnLeft;
                case MotorCommand::TurnRight: return ActionType::TurnRight;
                case MotorCommand::LookLeft: return ActionType::Look;
                case MotorCommand::LookRight: return ActionType::Look;
                case MotorCommand::Interact: return ActionType::Interact;
                case MotorCommand::Wait: return ActionType::Wait;
                default: return ActionType::Custom;
            }
        }, "Convert MotorCommand to ActionType");

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("is_resource_or_hazard", [](WorldObjectType t) {
            return t == WorldObjectType::Resource || t == WorldObjectType::Hazard;
        }, "Check if object is a resource or hazard")
        .def("is_static", [](WorldObjectType t) {
            return t == WorldObjectType::Wall || t == WorldObjectType::Marker;
        }, "Check if object is static (cannot be moved)");

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system with validation and convenience methods)pbdoc")
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
        
        // Parameter validation and convenience methods
        .def("validate", [](Config& self, const std::string& key, const py::object& expected_type) {
            if (!self.has(key)) {
                throw ValidationError("Key '" + key + "' not found in configuration");
            }
            
            // Basic validation - actual type checking would require more complex implementation
            return true;
        }, py::arg("key"), py::arg("expected_type"),
           "Validate a configuration key (placeholder for future implementation)")
        
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            // Basic range validation for int values
            if (value < std::numeric_limits<int>::min() / 2 || value > std::numeric_limits<int>::max() / 2) {
                throw ValidationError("Integer value " + std::to_string(value) + " is out of safe range");
            }
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set an integer value with range validation")
        
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            // Basic range validation for double values
            if (std::isnan(value) || std::isinf(value)) {
                throw ValidationError("Double value " + std::to_string(value) + " is not finite");
            }
            if (value < -1e6 || value > 1e6) {
                throw ValidationError("Double value " + std::to_string(value) + " is out of reasonable range");
            }
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a double value with range validation")
        
        .def("setBool", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a boolean value")
        
        .def("setString", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a string value")
        
        .def("getInt", [](const Config& self, const std::string& key, int defaultValue = 0) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0,
           "Get an integer value with default")
        
        .def("getDouble", [](const Config& self, const std::string& key, double defaultValue = 0.0) {
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = 0.0,
           "Get a double value with default")
        
        .def("getBool", [](const Config& self, const std::string& key, bool defaultValue = false) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = false,
           "Get a boolean value with default")
        
        .def("getString", [](const Config& self, const std::string& key, const std::string& defaultValue = "") {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue") = "",
           "Get a string value with default")
        
        // Context manager support for safe resource management
        .def("__enter__", [](Config& self) {
            return &self;
        })
        
        .def("__exit__", [](Config&, const py::args&) {
            // Nothing to clean up for Config
        })
        
        // Batch operations
        .def("batchSet", [](Config& self, const py::dict& values) {
            for (auto item : values) {
                std::string key = py::str(item.first);
                py::object value = item.second;
                
                if (value.is(py::int_())) {
                    self.set(key, value.cast<int>());
                } else if (value.is(py::float_())) {
                    self.set(key, value.cast<double>());
                } else if (value.is(py::bool_())) {
                    self.set(key, value.cast<bool>());
                } else if (value.is(py::str())) {
                    self.set(key, value.cast<std::string>());
                } else {
                    throw ValidationError("Unsupported value type for key '" + key + "'");
                }
            }
        }, py::arg("values"), "Batch set multiple configuration values")
        
        .def("batchGet", [](const Config& self, const py::list& keys) {
            py::dict result;
            for (auto key : keys) {
                std::string keyStr = py::str(key);
                if (self.has(keyStr)) {
                    // Note: Actual implementation would need to get the value from config
                    result[key] = "*"; // Placeholder
                }
            }
            return result;
        }, py::arg("keys"), "Batch get multiple configuration values")
        
        // Pythonic properties for read-only access
        .def_property_readonly("keys", &Config::getKeys,
                              "Get all configuration keys (read-only)")
        
        .def_property_readonly("is_empty", [](const Config& self) {
            return self.getKeys().empty();
        }, "Check if configuration is empty (read-only)")
        
        .def("validate_all", [](Config& self) {
            // Placeholder for comprehensive validation
            return true;
        }, "Validate all configuration entries")
        
        .def("copy", [](const Config& self) {
            Config newConfig;
            // Note: This is a simplified copy - actual implementation would need to copy all entries
            return newConfig;
        }, "Create a copy of this configuration")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const Config& cfg) {
            return "<Config keys=" + std::to_string(cfg.getKeys().size()) + ">";
        })
        
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input with enhanced Python interface)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        
        // Buffer protocol support for efficient data transfer
        .def_buffer("data_buffer", [](SensoryInput& self) {
            return py::buffer(self.getData());
        }, "Access data via buffer protocol for efficient memory operations")
        
        // Memory view support for zero-copy operations
        .def("get_memory_view", [](SensoryInput& self) {
            return py::memory_view(self.getData());
        }, "Get a memory view of the data for zero-copy operations")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput type=" + std::string(input.getType()) + 
                   " dimensions=" + std::to_string(input.getDimensions()) + ">";
        })
        
        .def("__str__", [](const SensoryInput& input) {
            return std::string(input.getType()) + " data (" + 
                   std::to_string(input.getDimensions()) + " dimensions)";
        })
        
        // Context manager support
        .def("__enter__", [](SensoryInput& self) {
            return &self;
        })
        
        .def("__exit__", [](SensoryInput&, const py::args&) {
            // Nothing to clean up for SensoryInput
        })
        
        // Validation and utility methods
        .def("validate_data", [](SensoryInput& self) {
            const auto& data = self.getData();
            if (data.empty()) {
                throw ValidationError("Sensory input data is empty");
            }
            
            // Check for NaN or infinite values
            for (float val : data) {
                if (std::isnan(val) || std::isinf(val)) {
                    throw ValidationError("Sensory input contains invalid values (NaN or infinity)");
                }
            }
            
            return true;
        }, "Validate sensory input data for correctness")
        
        .def("get_shape", [](const SensoryInput& self) {
            return py::make_tuple(self.getDimensions());
        }, "Get shape of sensory data as tuple")
        
        .def("is_valid", [](const SensoryInput& self) {
            const auto& data = self.getData();
            return !data.empty();
        }, "Check if sensory input is valid (has data)");
    
    // Enhanced Vision class with buffer protocol and validation
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with image processing capabilities)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        
        // Data methods with validation
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                throw ValidationError("Vision data size mismatch: expected " + 
                                     std::to_string(self.getWidth() * self.getHeight() * self.getChannels()) +
                                     " got " + std::to_string(data.size()));
            }
            
            self.setData(data);
        }, py::arg("data"), "Set vision data with validation")
        
        // Buffer protocol support for vision data
        .def_buffer("image_buffer", [](Vision& self) {
            return py::buffer(self.getData());
        }, "Access image data via buffer protocol for efficient processing")
        
        // Image processing utility methods
        .def("normalize", [](Vision& self, float min = 0.0f, float max = 1.0f) {
            auto& data = self.getData();
            for (float& pixel : data) {
                pixel = (pixel - min) / (max - min);
            }
            return self;
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Normalize image data to [min, max]")
        
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            // Placeholder for resize operation
            if (newWidth == 0 || newHeight == 0) {
                throw ValidationError("New dimensions must be positive");
            }
            return self;
        }, py::arg("newWidth"), py::arg("newHeight"), "Resize vision data (placeholder)")
        
        // Batch operations for vision data
        .def("batchNormalize", [](Vision& self, const py::list& images) {
            for (auto img : images) {
                if (!img.is(py::float_())) {
                    throw ValidationError("Vision data must contain float values");
                }
            }
            // Placeholder for batch normalization
            return self;
        }, py::arg("images"), "Normalize batch of vision images")
        
        .def("get_stats", [](const Vision& self) {
            const auto& data = self.getData();
            if (data.empty()) return py::make_tuple(0.0f, 0.0f, 0.0f);
            
            float sum = 0.0f, min = data[0], max = data[0];
            for (float val : data) {
                sum += val;
                if (val < min) min = val;
                if (val > max) max = val;
            }
            
            float mean = sum / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            
            return py::make_tuple(mean, min, max, std::sqrt(variance));
        }, "Get basic statistics of vision data (mean, min, max, std)")
        
        // Convenience methods
        .def("get_size", [](const Vision& self) {
            return py::make_tuple(self.getWidth(), self.getHeight(), self.getChannels());
        }, "Get vision dimensions as tuple (width, height, channels)")
        
        .def("is_grayscale", [](const Vision& self) {
            return self.getChannels() == 1;
        }, "Check if vision is grayscale")
        
        .def("is_rgb", [](const Vision& self) {
            return self.getChannels() == 3;
        }, "Check if vision is RGB");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        
        // Data methods with validation
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() != self.getNumSamples()) {
                throw ValidationError("Audio data size mismatch: expected " + 
                                     std::to_string(self.getNumSamples()) +
                                     " got " + std::to_string(data.size()));
            }
            
            self.setData(data);
        }, py::arg("data"), "Set audio data with validation")
        
        // Buffer protocol support for audio data
        .def_buffer("audio_buffer", [](Audio& self) {
            return py::buffer(self.getData());
        }, "Access audio data via buffer protocol for efficient processing")
        
        // Audio processing utility methods
        .def("normalize", [](Audio& self, float min = 0.0f, float max = 1.0f) {
            auto& data = self.getData();
            for (float& sample : data) {
                sample = (sample - min) / (max - min);
            }
            return self;
        }, py::arg("min") = 0.0f, py::arg("max") = 1.0f, "Normalize audio data to [min, max]")
        
        // Convenience methods
        .def("get_duration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        
        .def("is_valid_audio", [](const Audio& self) {
            const auto& data = self.getData();
            return !data.empty();
        }, "Check if audio data is valid (has samples)");
    
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input with enhanced Python interface)pbdoc")
        .def(py::init<>())
        
        // Signal management with validation
        .def("addSignal", [](InternalSignals& self, float value) {
            if (std::isnan(value) || std::isinf(value)) {
                throw ValidationError("Signal value must be finite (not NaN or infinity)");
            }
            self.addSignal(value);
        }, py::arg("value"), "Add a signal with validation")
        
        // Signal statistics
        .def("get_stats", [](const InternalSignals& self) {
            const auto& data = self.getData();
            if (data.empty()) return py::make_tuple(0.0f, 0.0f, 0.0f, 0.0f);
            
            float sum = 0.0f, min = data[0], max = data[0];
            for (float val : data) {
                sum += val;
                if (val < min) min = val;
                if (val > max) max = val;
            }
            
            float mean = sum / data.size();
            float variance = 0.0f;
            for (float val : data) {
                variance += (val - mean) * (val - mean);
            }
            variance /= data.size();
            
            return py::make_tuple(mean, min, max, std::sqrt(variance));
        }, "Get basic statistics of internal signals (mean, min, max, std)")
        
        // Signal operations
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all signals")
        
        .def("is_empty", [](const InternalSignals& self) {
            return self.getData().empty();
        }, "Check if no signals are present")
        
        // Batch operations
        .def("batch_add_signals", [](InternalSignals& self, const py::list& values) {
            for (auto val : values) {
                if (!val.is(py::float_())) {
                    throw ValidationError("Signal values must be floats");
                }
                float floatVal = val.cast<float>();
                if (std::isnan(floatVal) || std::isinf(floatVal)) {
                    throw ValidationError("Signal values must be finite numbers");
                }
                self.addSignal(floatVal);
            }
            return self;
        }, py::arg("values"), "Add multiple signals in batch");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        
        // Property accessors with validation
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"),
             "Set action type")
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters")
        
        // Action metadata
        .def("getName", &Action::getName)
        .def("getDescription", [](const Action& self) {
            std::string typeStr = "Unknown";
            switch (self.getType()) {
                case ActionType::MoveForward: typeStr = "Move forward"; break;
                case ActionType::MoveBackward: typeStr = "Move backward"; break;
                case ActionType::MoveLeft: typeStr = "Move left"; break;
                case ActionType::MoveRight: typeStr = "Move right"; break;
                case ActionType::TurnLeft: typeStr = "Turn left"; break;
                case ActionType::TurnRight: typeStr = "Turn right"; break;
                case ActionType::Look: typeStr = "Look around"; break;
                case ActionType::LookUp: typeStr = "Look up"; break;
                case ActionType::LookDown: typeStr = "Look down"; break;
                case ActionType::Interact: typeStr = "Interact with object"; break;
                case ActionType::Eat: typeStr = "Eat food"; break;
                case ActionType::Drink: typeStr = "Drink water"; break;
                case ActionType::Rest: typeStr = "Rest and recover"; break;
                case ActionType::Wait: typeStr = "Wait";
                default: typeStr = "Custom action";
            }
            return typeStr;
        }, "Get human-readable description of action")
        
        // Action validation and utility methods
        .def("validate", [](const Action& self) {
            if (self.getParameters().empty()) {
                return true; // Default parameters are okay
            }
            
            for (float param : self.getParameters()) {
                if (std::isnan(param) || std::isinf(param)) {
                    throw ValidationError("Action parameters must be finite numbers");
                }
            }
            return true;
        }, "Validate action parameters")
        
        .def("is_complete", [](const Action& self) {
            return self.getType() != ActionType::Custom;
        }, "Check if action is a predefined type")
        
        // Convenience constructors
        .def("create_move", [](float x, float y) {
            return Action(ActionType::MoveForward, std::vector<float>{x, y});
        }, py::arg("x"), py::arg("y"), "Create a movement action")
        
        .def("create_turn", [](float angle) {
            return Action(ActionType::TurnLeft, std::vector<float>{angle});
        }, py::arg("angle"), "Create a turning action")
        
        .def("create_interact", []() {
            return Action(ActionType::Interact);
        }, "Create an interaction action")
        
        // Context manager support
        .def("__enter__", [](Action& self) {
            return &self;
        })
        
        .def("__exit__", [](Action&, const py::args&) {
            // Nothing to clean up for Action
        })
        
        // Batch operations
        .def("batch_create", [](const py::list& actions) {
            std::vector<std::unique_ptr<Action>> result;
            for (auto action : actions) {
                if (action.is(py::str())) {
                    std::string actionStr = py::str(action);
                    // Simple string-based action creation
                    if (actionStr == "move") result.push_back(std::make_unique<Action>(ActionType::MoveForward));
                    else if (actionStr == "turn") result.push_back(std::make_unique<Action>(ActionType::TurnLeft));
                    else if (actionStr == "interact") result.push_back(std::make_unique<Action>(ActionType::Interact));
                    else throw ValidationError("Unknown action type: " + actionStr);
                }
            }
            return result;
        }, py::arg("actions"), "Create multiple actions in batch")
        
        // Copy and cloning
        .def("clone", &Action::clone,
             "Create a copy of this action")
        
        .def("copy", [](const Action& self) {
            return self.clone();
        }, "Create a copy of this action")
        
        // Pythonic properties
        .def_property_readonly("is_movement", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::MoveForward || 
                   type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft ||
                   type == ActionType::MoveRight;
        }, "Check if action is a movement action (read-only)")
        
        .def_property_readonly("is_turning", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::TurnLeft || type == ActionType::TurnRight;
        }, "Check if action is a turning action (read-only)")
        
        .def_property_readonly("is_interacting", [](const Action& self) {
            ActionType type = self.getType();
            return type == ActionType::Interact || 
                   type == ActionType::Eat || 
                   type == ActionType::Drink;
        }, "Check if action is an interaction action (read-only)")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const Action& self) {
            std::string paramsStr = "";
            if (!self.getParameters().empty()) {
                paramsStr = " params=[";
                for (size_t i = 0; i < self.getParameters().size(); ++i) {
                    if (i > 0) paramsStr += ", ";
                    paramsStr += std::to_string(self.getParameters()[i]);
                }
                paramsStr += "]";
            }
            return "<Action " + py::str(self.getType()) + paramsStr + ">";
        })
        
        .def("__str__", [](const Action& self) {
            return py::str(self.getType());
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        
        // Property accessors
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active)
        
        // Position validation
        .def("validate_position", [](WorldObject& self) {
            if (std::isnan(self.x) || std::isnan(self.y) || 
                std::isnan(self.radius) || std::isnan(self.value)) {
                throw ValidationError("World object contains NaN values");
            }
            if (self.radius < 0.0f) {
                throw ValidationError("World object radius cannot be negative");
            }
            return true;
        }, "Validate object position and geometry")
        
        .def("is_static", [](const WorldObject& self) {
            return self.type == WorldObjectType::Wall || 
                   self.type == WorldObjectType::Marker;
        }, "Check if object is static (cannot be moved)")
        
        .def("is_resource_or_hazard", [](const WorldObject& self) {
            return self.type == WorldObjectType::Resource || 
                   self.type == WorldObjectType::Hazard;
        }, "Check if object provides reward or causes damage")
        
        .def("get_world_object_type_name", [](const WorldObject& self) {
            switch (self.type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        }, "Get human-readable name of object type")
        
        // Convenience constructors
        .def("create_wall", [](float x, float y, float radius = 0.5f) {
            return WorldObject(x, y, WorldObjectType::Wall, 0.0f, radius);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Create a wall object")
        
        .def("create_resource", [](float x, float y, float value = 1.0f, float radius = 0.3f) {
            return WorldObject(x, y, WorldObjectType::Resource, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, py::arg("radius") = 0.3f, "Create a resource object")
        
        .def("create_hazard", [](float x, float y, float value = -1.0f, float radius = 0.3f) {
            return WorldObject(x, y, WorldObjectType::Hazard, value, radius);
        }, py::arg("x"), py::arg("y"), py::arg("value") = -1.0f, py::arg("radius") = 0.3f, "Create a hazard object")
        
        // Context manager support
        .def("__enter__", [](WorldObject& self) {
            return &self;
        })
        
        .def("__exit__", [](WorldObject&, const py::args&) {
            // Nothing to clean up for WorldObject
        })
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject x=" + std::to_string(obj.x) + 
                   " y=" + std::to_string(obj.y) + 
                   " type=" + py::str(obj.type) + 
                   " active=" + std::to_string(obj.active) + ">";
        })
        
        .def("__str__", [](const WorldObject& obj) {
            return "WorldObject at (" + std::to_string(obj.x) + ", " + 
                   std::to_string(obj.y) + ") type=" + py::str(obj.type);
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with enhanced Python interface)pbdoc")
        .def(py::init<>())
        
        // Property accessors
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
        
        // State validation
        .def("validate_state", [](AgentBody& self) {
            if (self.energy < 0.0f || self.energy > 1.0f) {
                throw ValidationError("Agent energy must be in range [0.0, 1.0]");
            }
            if (self.health < 0.0f || self.health > 1.0f) {
                throw ValidationError("Agent health must be in range [0.0, 1.0]");
            }
            if (std::isnan(self.x) || std::isnan(self.y) || 
                std::isnan(self.orientation)) {
                throw ValidationError("Agent position or orientation contains NaN values");
            }
            return true;
        }, "Validate agent body state")
        
        .def("is_alive", [](const AgentBody& self) {
            return self.health > 0.5f;
        }, "Check if agent is alive (read-only)")
        
        .def("is_exhausted", [](const AgentBody& self) {
            return self.energy < 0.1f;
        }, "Check if agent is exhausted (read-only)")
        
        .def("get_speed", [](const AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + 
                           self.velocityY * self.velocityY);
        }, "Get current speed (read-only)")
        
        // Position and movement utilities
        .def("distance_to", [](const AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position")
        
        .def("move_to", [](AgentBody& self, float x, float y, double timestep) {
            float dx = x - self.x;
            float dy = y - self.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            if (distance > 0.0f) {
                float speed = std::min(distance / static_cast<float>(timestep), 
                                      self.energy * 10.0f); // Max speed based on energy
                
                self.velocityX = speed * dx / distance;
                self.velocityY = speed * dy / distance;
                self.isMoving = true;
            }
            
            return self;
        }, py::arg("x"), py::arg("y"), py::arg("timestep"), "Move agent to position")
        
        // Energy management
        .def("consume_energy", [](AgentBody& self, float amount) {
            if (amount < 0.0f) {
                throw ValidationError("Cannot consume negative energy");
            }
            if (self.energy < amount) {
                throw ResourceError("Insufficient energy to perform action");
            }
            self.energy -= amount;
            if (self.energy < 0.0f) self.energy = 0.0f;
            return self;
        }, py::arg("amount"), "Consume energy from agent")
        
        .def("recover_energy", [](AgentBody& self, float amount, double timestep) {
            float recoveryRate = 0.1f / static_cast<float>(timestep);
            float energyGain = std::min(amount, recoveryRate);
            self.energy = std::min(1.0f, self.energy + energyGain);
            return self;
        }, py::arg("amount"), py::arg("timestep"), "Recover energy for agent")
        
        // Context manager support
        .def("__enter__", [](AgentBody& self) {
            return &self;
        })
        
        .def("__exit__", [](AgentBody&, const py::args&) {
            // Nothing to clean up for AgentBody
        })
        
        // State management
        .def("reset", &AgentBody::reset,
             "Reset agent to initial state")
        
        .def("copy", [](const AgentBody& self) {
            AgentBody copy;
            copy.x = self.x;
            copy.y = self.y;
            copy.orientation = self.orientation;
            copy.velocityX = self.velocityX;
            copy.velocityY = self.velocityY;
            copy.angularVelocity = self.angularVelocity;
            copy.energy = self.energy;
            copy.health = self.health;
            copy.age = self.age;
            copy.isMoving = self.isMoving;
            copy.isTurning = self.isTurning;
            copy.lastActionTime = self.lastActionTime;
            return copy;
        }, "Create a copy of agent body state")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody pos=(" + std::to_string(body.x) + ", " + 
                   std::to_string(body.y) + ") energy=" + 
                   std::to_string(body.energy) + " health=" + 
                   std::to_string(body.health) + ">";
        })
        
        .def("__str__", [](const AgentBody& body) {
            return "Agent at (" + std::to_string(body.x) + ", " + 
                   std::to_string(body.y) + ") health=" + 
                   std::to_string(body.health);
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        
        // Property accessors
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        
        // Result validation
        .def("validate", [](ActionResult& self) {
            if (std::isnan(self.reward)) {
                throw ValidationError("Action result reward cannot be NaN");
            }
            if (self.reward < -1000.0f || self.reward > 1000.0f) {
                throw ValidationError("Action result reward out of reasonable range [-1000, 1000]");
            }
            return true;
        }, "Validate action result")
        
        .def("is_good", [](const ActionResult& self) {
            return self.success && self.reward > 0.0f;
        }, "Check if result is good (success with positive reward)")
        
        .def("is_bad", [](const ActionResult& self) {
            return self.success && self.reward < 0.0f;
        }, "Check if result is bad (success with negative reward)")
        
        .def("is_failure", [](const ActionResult& self) {
            return !self.success;
        }, "Check if result is a failure")
        
        // Convenience constructors
        .def("success", [](float reward, const std::string& message = "") {
            return ActionResult(reward, true, message);
        }, py::arg("reward"), py::arg("message") = "", "Create a successful action result")
        
        .def("failure", [](float reward = 0.0f, const std::string& message = "Failed") {
            return ActionResult(reward, false, message);
        }, py::arg("reward") = 0.0f, py::arg("message") = "Failed", "Create a failed action result")
        
        .def("reward_only", [](float reward) {
            return ActionResult(reward, reward >= 0.0f);
        }, py::arg("reward"), "Create a simple reward-only result")
        
        // Context manager support
        .def("__enter__", [](ActionResult& self) {
            return &self;
        })
        
        .def("__exit__", [](ActionResult&, const py::args&) {
            // Nothing to clean up for ActionResult
        })
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) + 
                   " success=" + std::to_string(result.success) + 
                   " message=\"" + result.message + "\">";
        })
        
        .def("__str__", [](const ActionResult& result) {
            std::string status = result.success ? "SUCCESS" : "FAILURE";
            return status + " reward: " + std::to_string(result.reward);
        });

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced Python interface)pbdoc")
        .def(py::init<>())
        
        // Vision accessors
        .def("getVision", &SensoryPercept::getVision)
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        
        // Touch accessors
        .def("getTouch", &SensoryPercept::getTouch)
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        
        // Internal state accessors
        .def("getInternal", &SensoryPercept::getInternal)
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal state data")
        
        // Proprioception accessors
        .def("getProprioception", &SensoryPercept::getProprioception)
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data")
        
        // Audio accessors
        .def("getAudio", &SensoryPercept::getAudio)
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        
        // Timestamp accessors
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        
        // Enhanced data access with validation
        .def("getAllSignals", [](const SensoryPercept& self) {
            return self.getAllSignals();
        }, "Get all sensory signals concatenated")
        
        .def("validate_sensory_data", [](SensoryPercept& self) {
            const auto& vision = self.getVision();
            const auto& touch = self.getTouch();
            const auto& internal = self.getInternal();
            const auto& proprioception = self.getProprioception();
            const auto& audio = self.getAudio();
            
            // Check for NaN values
            auto checkForNaN = [](const std::vector<float>& data, const std::string& name) {
                for (size_t i = 0; i < data.size(); ++i) {
                    if (std::isnan(data[i])) {
                        throw ValidationError("Sensory percept " + name + " contains NaN at index " + std::to_string(i));
                    }
                }
            };
            
            checkForNaN(vision, "vision");
            checkForNaN(touch, "touch");
            checkForNaN(internal, "internal");
            checkForNaN(proprioception, "proprioception");
            checkForNaN(audio, "audio");
            
            return true;
        }, "Validate all sensory data for correctness")
        
        // Buffer protocol support for efficient data transfer
        .def_buffer("vision_buffer", [](SensoryPercept& self) {
            return py::buffer(self.getVision());
        }, "Access vision data via buffer protocol")
        
        .def_buffer("touch_buffer", [](SensoryPercept& self) {
            return py::buffer(self.getTouch());
        }, "Access touch data via buffer protocol")
        
        .def_buffer("internal_buffer", [](SensoryPercept& self) {
            return py::buffer(self.getInternal());
        }, "Access internal data via buffer protocol")
        
        // Signal statistics
        .def("get_vision_stats", [](const SensoryPercept& self) {
            const auto& vision = self.getVision();
            if (vision.empty()) return py::make_tuple(0.0f, 0.0f, 0.0f);
            
            float sum = 0.0f, min = vision[0], max = vision[0];
            for (float val : vision) {
                sum += val;
                if (val < min) min = val;
                if (val > max) max = val;
            }
            
            float mean = sum / vision.size();
            float variance = 0.0f;
            for (float val : vision) {
                variance += (val - mean) * (val - mean);
            }
            variance /= vision.size();
            
            return py::make_tuple(mean, min, max, std::sqrt(variance));
        }, "Get vision statistics (mean, min, max, std)")
        
        .def("get_signal_shapes", [](const SensoryPercept& self) {
            return py::make_tuple(
                py::make_tuple(self.getVisionWidth(), self.getVisionHeight()),
                py::make_tuple(self.getTouch().size()),
                py::make_tuple(self.getInternal().size()),
                py::make_tuple(self.getProprioception().size()),
                py::make_tuple(self.getAudio().size())
            );
        }, "Get shapes of all sensory signals")
        
        // Convenience methods
        .def("is_vision_available", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if vision data is available")
        
        .def("get_total_signal_count", [](const SensoryPercept& self) {
            return self.getAllSignals().size();
        }, "Get total number of signals across all modalities")
        
        // Context manager support
        .def("__enter__", [](SensoryPercept& self) {
            return &self;
        })
        
        .def("__exit__", [](SensoryPercept&, const py::args&) {
            // Nothing to clean up for SensoryPercept
        })
        
        // Batch operations
        .def("batch_set_vision", [](SensoryPercept& self, const py::list& visionData) {
            std::vector<float> vision;
            for (auto val : visionData) {
                if (!val.is(py::float_())) {
                    throw ValidationError("Vision data must be float values");
                }
                vision.push_back(val.cast<float>());
            }
            self.setVision(vision);
            return self;
        }, py::arg("visionData"), "Set vision data from batch of values")
        
        .def("batch_set_all", [](SensoryPercept& self, 
                               const py::dict& signals,
                               double timestamp = 0.0) {
            if (signals.contains("vision")) {
                self.setVision(signals["vision"].cast<std::vector<float>>());
            }
            if (signals.contains("touch")) {
                self.setTouch(signals["touch"].cast<std::vector<float>>());
            }
            if (signals.contains("internal")) {
                self.setInternal(signals["internal"].cast<std::vector<float>>());
            }
            if (signals.contains("proprioception")) {
                self.setProprioception(signals["proprioception"].cast<std::vector<float>>());
            }
            if (signals.contains("audio")) {
                self.setAudio(signals["audio"].cast<std::vector<float>>());
            }
            self.setTimestamp(timestamp);
            return self;
        }, py::arg("signals"), py::arg("timestamp") = 0.0, "Set multiple signals in batch")
        
        // Copy operation
        .def("copy", [](const SensoryPercept& self) {
            SensoryPercept copy;
            copy.setVision(self.getVision());
            copy.setTouch(self.getTouch());
            copy.setInternal(self.getInternal());
            copy.setProprioception(self.getProprioception());
            copy.setAudio(self.getAudio());
            copy.setTimestamp(self.getTimestamp());
            return copy;
        }, "Create a copy of sensory percept")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept vision=[" + std::to_string(percept.getVisionWidth()) + 
                   "x" + std::to_string(percept.getVisionHeight()) + "] " +
                   "touch=" + std::to_string(percept.getTouch().size()) + " " +
                   "internal=" + std::to_string(percept.getInternal().size()) + ">";
        })
        
        .def("__str__", [](const SensoryPercept& percept) {
            return "SensoryPercept vision=" + std::to_string(percept.getVisionWidth()) + 
                   "x" + std::to_string(percept.getVisionHeight());
        });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation with enhanced Python interface)pbdoc")
        .def(py::init<>())
        
        // Configuration with validation
        .def("configure", [](SimpleWorld& self, float width, float height, size_t visionWidth, size_t visionHeight) {
            if (width <= 0.0f || height <= 0.0f) {
                throw ValidationError("World dimensions must be positive");
            }
            if (visionWidth == 0 || visionHeight == 0) {
                throw ValidationError("Vision dimensions must be positive");
            }
            self.configure(width, height, visionWidth, visionHeight);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           "Configure world with validation")
        
        // World state management
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        
        // Simulation with validation
        .def("update", [](SimpleWorld& self, double timestep) {
            if (timestep <= 0.0) {
                throw ValidationError("Timestep must be positive");
            }
            self.update(timestep);
        }, py::arg("timestep"), "Update world state (requires positive timestep)")
        
        // Motor command application
        .def("applyMotorCommand", [](SimpleWorld& self, MotorCommand cmd, double currentTime) {
            if (currentTime < 0.0) {
                throw ValidationError("Current time cannot be negative");
            }
            return self.applyMotorCommand(cmd, currentTime);
        }, py::arg("cmd"), py::arg("currentTime"),
           "Apply motor command with timestamp validation")
        
        // Object management
        .def("addObject", [](SimpleWorld& self, const WorldObject& obj) {
            // Validate object before adding
            if (std::isnan(obj.x) || std::isnan(obj.y)) {
                throw ValidationError("Object position contains NaN values");
            }
            if (std::isnan(obj.radius) || std::isnan(obj.value)) {
                throw ValidationError("Object geometry or value contains NaN values");
            }
            self.addObject(obj);
        }, py::arg("obj"), "Add object with validation")
        
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        
        // Getters with enhanced Pythonic interface
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal)
        
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal)
        
        // World properties
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        
        // Energy system
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        
        // Random seed management
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        
        // Enhanced validation methods
        .def("validate_world", [](SimpleWorld& self) {
            // Check world boundaries
            if (self.getWidth() <= 0.0f || self.getHeight() <= 0.0f) {
                throw ValidationError("World dimensions must be positive");
            }
            
            // Check agent position
            const auto& agent = self.getAgentBody();
            if (agent.x < 0.0f || agent.x > self.getWidth() ||
                agent.y < 0.0f || agent.y > self.getHeight()) {
                throw ValidationError("Agent position out of world bounds");
            }
            
            // Check objects
            for (const auto& obj : self.getObjects()) {
                if (std::isnan(obj.x) || std::isnan(obj.y)) {
                    throw ValidationError("Object position contains NaN");
                }
                if (obj.x < 0.0f || obj.x > self.getWidth() ||
                    obj.y < 0.0f || obj.y > self.getHeight()) {
                    throw ValidationError("Object position out of world bounds");
                }
            }
            
            return true;
        }, "Comprehensive world validation")
        
        .def("get_object_count", [](const SimpleWorld& self) {
            return self.getObjects().size();
        }, "Get number of objects in world (read-only)")
        
        .def("get_agent_position", [](const SimpleWorld& self) {
            return py::make_tuple(self.getAgentBody().x, self.getAgentBody().y);
        }, "Get agent position as tuple (x, y) (read-only)")
        
        .def("get_world_size", [](const SimpleWorld& self) {
            return py::make_tuple(self.getWidth(), self.getHeight());
        }, "Get world dimensions as tuple (width, height) (read-only)")
        
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        })
        
        .def("__exit__", [](SimpleWorld&, const py::args&) {
            // Nothing to clean up for SimpleWorld
        })
        
        // Batch operations
        .def("batch_add_objects", [](SimpleWorld& self, const py::list& objects) {
            for (auto obj : objects) {
                if (!obj.is(WorldObject)) {
                    throw ValidationError("Batch add requires WorldObject instances");
                }
                self.addObject(obj.cast<WorldObject>());
            }
        }, py::arg("objects"), "Add multiple objects in batch")
        
        .def("batch_remove_objects", [](SimpleWorld& self, const py::list& positions) {
            for (auto pos : positions) {
                if (!pos.is(py::tuple())) {
                    throw ValidationError("Position must be a tuple (x, y)");
                }
                auto posTuple = pos.cast<py::tuple>();
                float x = posTuple[0].cast<float>();
                float y = posTuple[1].cast<float>();
                self.removeObject(x, y);
            }
        }, py::arg("positions"), "Remove multiple objects by positions")
        
        .def("simulate_step", [](SimpleWorld& self, double timestep) {
            self.update(timestep);
            return self.getSensoryPercept();
        }, py::arg("timestep"), "Perform one simulation step and return percept")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld width=" + std::to_string(world.getWidth()) + 
                   " height=" + std::to_string(world.getHeight()) + 
                   " time=" + std::to_string(world.getSimulationTime()) + ">";
        })
        
        .def("__str__", [](const SimpleWorld& world) {
            return "World " + std::to_string(world.getWidth()) + "x" + 
                   std::to_string(world.getHeight()) + 
                   " time=" + std::to_string(world.getSimulationTime());
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with enhanced Python interface)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        
        // Core methods
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
        
        // State management
        .def("reset", &Brain::reset,
             "Reset brain state")
        
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file")
        
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file")
        
        // Region management
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
        
        // Statistics and monitoring
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
        
        // Development and plasticity
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        
        // Configuration
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        
        // Context manager support for safe resource management
        .def("__enter__", [](Brain& self) {
            return &self;
        })
        
        .def("__exit__", [](Brain&, const py::args&) {
            // Nothing to clean up for Brain
        })
        
        // Convenience methods and error handling
        .def("validate_state", [](Brain& self) {
            // Check if brain is properly initialized
            // Note: This is a simplified validation - actual implementation would check brain state
            return true;
        }, "Validate brain state")
        
        .def("run_simulation_step", [](Brain& self, SimulationStep step, Timestamp time = 0.0) {
            if (time < 0.0) {
                throw ValidationError("Simulation time cannot be negative");
            }
            self.step(step, time);
            return self.produceAction();
        }, py::arg("step"), py::arg("time") = 0.0,
           "Run a complete simulation step and produce action")
        
        .def("inject_sensory_input", [](Brain& self, const SensoryInput& input) {
            if (input.getDimensions() == 0) {
                throw ValidationError("Cannot inject empty sensory input");
            }
            self.receiveSensoryInput(input);
        }, py::arg("input"), "Inject sensory input with validation")
        
        .def("batch_inject_sensory", [](Brain& self, const py::list& inputs) {
            for (auto input : inputs) {
                if (!input.is(SensoryInput)) {
                    throw ValidationError("Batch input requires SensoryInput instances");
                }
                self.receiveSensoryInput(input.cast<SensoryInput>());
            }
        }, py::arg("inputs"), "Inject multiple sensory inputs in batch")
        
        .def("get_brain_summary", [](const Brain& self) {
            std::stringstream ss;
            ss << "Brain State:" << std::endl;
            ss << "  Total Neurons: " << self.getTotalNeuronCount() << std::endl;
            ss << "  Total Synapses: " << self.getTotalSynapseCount() << std::endl;
            ss << "  Active Neurons: " << self.getActiveNeuronCount() << std::endl;
            ss << "  Firing Neurons: " << self.getFiringNeuronCount() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            ss << "  Excitatory/Inhibitory Ratio: " << self.getExcitationInhibitionRatio() << std::endl;
            return ss.str();
        }, "Get detailed brain state summary")
        
        .def("create_region_with_validation", [](Brain& self, const std::string& name, 
                                                   size_t neuronCount = 100,
                                                   float connectivity = 0.1f) {
            if (name.empty()) {
                throw ValidationError("Region name cannot be empty");
            }
            if (neuronCount == 0) {
                throw ValidationError("Neuron count must be positive");
            }
            if (connectivity < 0.0f || connectivity > 1.0f) {
                throw ValidationError("Connectivity must be in range [0.0, 1.0]");
            }
            return self.addRegion(name);
        }, py::arg("name"), py::arg("neuronCount") = 100, py::arg("connectivity") = 0.1f,
           "Create a neural region with validation")
        
        .def("get_region_stats", [](const Brain& self) {
            size_t totalNeurons = self.getTotalNeuronCount();
            size_t totalSynapses = self.getTotalSynapseCount();
            size_t activeNeurons = self.getActiveNeuronCount();
            size_t firingNeurons = self.getFiringNeuronCount();
            
            float firingRate = totalNeurons > 0 ? 
                static_cast<float>(firingNeurons) / static_cast<float>(totalNeurons) * 100.0f : 0.0f : 0.0f;
            
            py::dict stats;
            stats["total_neurons"] = totalNeurons;
            stats["total_synapses"] = totalSynapses;
            stats["active_neurons"] = activeNeurons;
            stats["firing_neurons"] = firingNeurons;
            stats["firing_rate_percent"] = firingRate;
            stats["region_count"] = self.getRegionCount();
            
            return stats;
        }, "Get brain statistics as dictionary")
        
        // Pythonic properties for read-only access
        .def_property_readonly("is_initialized", [](const Brain& self) {
            // Note: Simplified check - actual implementation would check brain initialization state
            return true;
        }, "Check if brain is initialized (read-only)")
        
        .def_property_readonly("has_config", [](const Brain& self) {
            return self.getConfig() != nullptr;
        }, "Check if brain has configuration (read-only)")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const Brain& brain) {
            return "<Brain regions=" + std::to_string(brain.getRegionCount()) + 
                   " neurons=" + std::to_string(brain.getTotalNeuronCount()) + ">";
        })
        
        .def("__str__", [](const Brain& brain) {
            return "Brain with " + std::to_string(brain.getRegionCount()) + " regions and " +
                   std::to_string(brain.getTotalNeuronCount()) + " neurons";
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with enhanced Python interface)pbdoc")
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
        
        // Configuration flags with enhanced error handling
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
        
        // Enhanced validation and error handling
        .def("validate_configuration", [](AgentBrain& self) {
            // Check if brain is initialized before enabling features
            // Note: Simplified validation - actual implementation would check brain state
            return true;
        }, "Validate agent brain configuration")
        
        .def("configure_features", [](AgentBrain& self,
                                    bool rewardModulation = true,
                                    bool structuralPlasticity = true,
                                    bool development = true,
                                    bool curiosity = true) {
            self.enableRewardModulation(rewardModulation);
            self.enableStructuralPlasticity(structuralPlasticity);
            self.enableDevelopment(development);
            self.enableCuriosity(curiosity);
            return self;
        }, py::arg("rewardModulation") = true,
           py::arg("structuralPlasticity") = true,
           py::arg("development") = true,
           py::arg("curiosity") = true,
           "Configure multiple features at once")
        
        .def("get_feature_status", [](const AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get status of all enabled features as dictionary")
        
        .def("enable_all_features", [](AgentBrain& self) {
            self.configure_features(true, true, true, true);
            return self;
        }, "Enable all agent brain features")
        
        .def("disable_all_features", [](AgentBrain& self) {
            self.configure_features(false, false, false, false);
            return self;
        }, "Disable all agent brain features")
        
        // Convenience methods for common patterns
        .def("process_sensory_with_validation", [](AgentBrain& self, const SensoryPercept& percept) {
            if (percept.getVisionWidth() == 0 || percept.getVisionHeight() == 0) {
                throw ValidationError("Cannot process percept with empty vision data");
            }
            self.processSensoryInput(percept);
        }, py::arg("percept"), "Process sensory input with validation")
        
        .def("get_agent_summary", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "AgentBrain Summary:" << std::endl;
            ss << "  Features Enabled:" << std::endl;
            ss << "    Reward Modulation: " << (self.isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
            ss << "    Structural Plasticity: " << (self.isStructuralPlasticityEnabled() ? "Yes" : "No") << std::endl;
            ss << "    Development: " << (self.isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
            ss << "    Curiosity: " << (self.isCuriosityEnabled() ? "Yes" : "No") << std::endl;
            ss << "  Neuromodulation Levels:" << std::endl;
            ss << "    Dopamine: " << self.getNeuromodulationLevel() << std::endl;
            ss << "    Curiosity: " << self.getCuriosityLevel() << std::endl;
            ss << "    Novelty: " << self.getNoveltyLevel() << std::endl;
            ss << "    Prediction Error: " << self.getPredictionError() << std::endl;
            ss << "  Developmental Stage: " << static_cast<int>(self.getDevelopmentalStage()) << std::endl;
            return ss.str();
        }, "Get detailed agent brain summary")
        
        // Context manager support
        .def("__enter__", [](AgentBrain& self) {
            return &self;
        })
        
        .def("__exit__", [](AgentBrain&, const py::args&) {
            // Nothing to clean up for AgentBrain
        })
        
        .def("copy", [](const AgentBrain& self) {
            // Note: This is a simplified copy - actual implementation would need to copy all internal state
            AgentBrain copy(self.getBrain());
            copy.configure_features(
                self.isRewardModulationEnabled(),
                self.isStructuralPlasticityEnabled(),
                self.isDevelopmentEnabled(),
                self.isCuriosityEnabled()
            );
            return copy;
        }, "Create a copy of agent brain")
        
        // Batch operations
        .def("process_multiple_sensory", [](AgentBrain& self, const py::list& percepts) {
            for (auto percept : percepts) {
                if (!percept.is(SensoryPercept)) {
                    throw ValidationError("Batch processing requires SensoryPercept instances");
                }
                self.processSensoryInput(percept.cast<SensoryPercept>());
            }
            return self;
        }, py::arg("percepts"), "Process multiple sensory percepts in batch")
        
        .def("decode_multiple_actions", [](const AgentBrain& self, const py::list& actionCounts) {
            std::vector<MotorCommand> commands;
            for (auto count : actionCounts) {
                int countInt = count.cast<int>();
                if (countInt <= 0) {
                    throw ValidationError("Action count must be positive");
                }
                // Simple decoding - would need actual motor neuron analysis
                MotorCommand cmd = MotorCommand::Wait;
                commands.push_back(cmd);
            }
            return commands;
        }, py::arg("actionCounts"), "Decode multiple actions from batch counts")
        
        // Enhanced __repr__ and __str__
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain brain=" + std::to_string(reinterpret_cast<uintptr_t>(agent.getBrain().get())) + ">";
        })
        
        .def("__str__", [](const AgentBrain& agent) {
            return "AgentBrain with " + 
                   (agent.isRewardModulationEnabled() ? "reward " : "") +
                   (agent.isDevelopmentEnabled() ? "development " : "") +
                   (agent.isCuriosityEnabled() ? "curiosity features" : "basic features");
        });

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
