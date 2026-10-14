#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <pybind11/iostream.h>

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

namespace py = pybind11;
namespace nlm {

// Helper functions for Python bindings
namespace {
    std::string configValueToString(const ConfigValue& value) {
        std::ostringstream oss;
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "true" : "false");
            } else {
                oss << arg;
            }
        }, value);
        return oss.str();
    }
    
    std::string configValueToPythonRepr(const ConfigValue& value) {
        std::ostringstream oss;
        oss << "ConfigValue(";
        std::visit([&oss](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                oss << "\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, bool>) {
                oss << (arg ? "True" : "False");
            } else if constexpr (std::is_same_v<T, double>) {
                oss << std::setprecision(6) << arg;
            } else if constexpr (std::is_same_v<T, int>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, int64_t>) {
                oss << arg;
            } else if constexpr (std::is_same_v<T, std::vector<int>>) {
                oss << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << vec[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<double>>) {
                oss << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << std::setprecision(6) << vec[i];
                }
                oss << "]";
            } else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
                oss << "[";
                const auto& vec = arg;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (i > 0) oss << ", ";
                    oss << "\"" << vec[i] << "\"";
                }
                oss << "]";
            } else {
                oss << "<unknown>";
            }
        }, value);
        oss << ")";
        return oss.str();
    }
    
    py::class_<py::Any> anyConverter() {
        return py::class_<py::Any>(py::moduleref(), "Any", py::dynamic_attr()
            ).def(py::init<>())
             .def(py::init<const ConfigValue&>(), py::arg("value"))
             .def("get", [](py::Any& self, const std::string& typeName) {
                 try {
                     if (typeName == "int") return self.cast<int>();
                     else if (typeName == "double") return self.cast<double>();
                     else if (typeName == "string") return self.cast<std::string>();
                     else if (typeName == "bool") return self.cast<bool>();
                     else if (typeName == "vector<double>") return self.cast<std::vector<double>>();
                     else if (typeName == "vector<string>") return self.cast<std::vector<std::string>>();
                     else if (typeName == "map") return self.cast<std::map<std::string, std::any>>();
                 } catch (...) {
                     throw py::cast_error();
                 }
                 throw py::cast_error();
             });
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------
        An enhanced Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes for Brain, Config, AgentBrain, SimpleWorld,
        SensoryInput, and Action with advanced functionality and examples.
        
        This enhanced version includes:
        - Advanced configuration management
        - Brain profiling and analysis
        - AgentBrain with enhanced sensory-motor integration
        - World management with multiple environment types
        - Advanced sensory input processing
        - Comprehensive error handling and logging
        - Performance optimization utilities
        - Learning experiment management
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "InvalidArgument");
    py::register_exception<std::out_of_range>(m, "OutOfRange");

    // NeuronId
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

    // SynapseId
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

    // RegionId
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

    // PopulationId
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // NeuronType enum
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // SynapseType enum
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // DevelopmentalStage enum
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // FiringState enum
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // ActionType enum
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

    // MotorCommand enum
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

    // WorldObjectType enum
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Enhanced Config class
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
        .def(py::init<>())
        
        // Basic configuration operations
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"), py::arg("format") = "auto",
             "Load configuration from a file with automatic format detection")
        .def("loadFromJSON", &Config::loadFromJSON, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromYAML", &Config::loadFromYAML, py::arg("filepath"),
             "Load configuration from a YAML file")
        .def("loadFromINI", &Config::loadFromINI, py::arg("filepath"),
             "Load configuration from an INI file")
        .def("loadFromTOML", &Config::loadFromTOML, py::arg("filepath"),
             "Load configuration from a TOML file")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), py::arg("format") = "json",
             "Save configuration to a file in specified format")
        .def("saveToJSON", &Config::saveToJSON, py::arg("filepath"),
             "Save configuration to a JSON file")
        
        // Enhanced getter methods
        .def("get", &Config::get<int>, py::arg("key"),
             "Get an integer value by key")
        .def("get", &Config::get<double>, py::arg("key"),
             "Get a double value by key")
        .def("get", &Config::get<std::string>, py::arg("key"),
             "Get a string value by key")
        .def("get", &Config::get<bool>, py::arg("key"),
             "Get a boolean value by key")
        .def("get", &Config::get<std::vector<double>>, py::arg("key"),
             "Get a vector of doubles by key")
        
        // Advanced getter methods
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get an integer value with default, or return default if key doesn't exist")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get a double value with default, or return default if key doesn't exist")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get a string value with default, or return default if key doesn't exist")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get a boolean value with default, or return default if key doesn't exist")
        
        // Type-safe getters
        .def("getInt", &Config::getInt, py::arg("key"),
             "Get an integer value (returns 0 if key doesn't exist)")
        .def("getDouble", &Config::getDouble, py::arg("key"),
             "Get a double value (returns 0.0 if key doesn't exist)")
        .def("getBool", &Config::getBool, py::arg("key"),
             "Get a boolean value (returns false if key doesn't exist)")
        .def("getString", &Config::getString, py::arg("key"),
             "Get a string value (returns empty string if key doesn't exist)")
        
        // Nested key access
        .def("getNested", &Config::getNested<int>, py::arg("key"),
             "Get a value from nested configuration (dot notation)")
        .def("getNestedOr", &Config::getNestedOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get a nested value with default")
        
        // Configuration manipulation
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             "Set a configuration value")
        .def("setInt", &Config::set, py::arg("key"), py::arg("value"),
             "Set an integer configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a double configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a string configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"),
             "Set a boolean configuration value")
        
        // Configuration validation and analysis
        .def("validate", &Config::validate,
             "Validate the configuration for errors and warnings")
        .def("validateKey", &Config::validateKey, py::arg("key"),
             "Validate a specific configuration key")
        .def("getStats", &Config::getStats,
             "Get configuration statistics")
        .def("getSummary", &Config::getConfigurationSummary,
             "Get a string summary of the configuration")
        
        // Configuration manipulation
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("getKeysByCategory", &Config::getKeysByCategory, py::arg("category"),
             "Get all keys in a specific category")
        .def("getCategory", &Config::getCategory, py::arg("category"),
             "Get all entries in a specific category")
        .def("remove", &Config::remove, py::arg("key"),
             "Remove a configuration key")
        .def("removeByCategory", &Config::removeByCategory, py::arg("category"),
             "Remove all keys in a category")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        
        // Configuration comparison and merging
        .def("equals", &Config::equals, py::arg("other"),
             "Check if configuration equals another configuration")
        .def("merge", &Config::merge, py::arg("other"), py::arg("source") = ConfigSource::Runtime,
             "Merge another configuration into this one")
        .def("inheritFrom", &Config::inheritFrom, py::arg("parent"), py::arg("source") = ConfigSource::Default,
             "Inherit settings from another configuration")
        
        // Configuration backup and restore
        .def("backup", &Config::backup, py::arg("filepath"),
             "Create a backup of the configuration")
        .def("restore", &Config::restore, py::arg("filepath"),
             "Restore configuration from a backup")
        
        // Configuration profiles
        .def("loadProfile", &Config::loadProfile, py::arg("profileName"), py::arg("merge") = true,
             "Load a configuration profile")
        .def("saveProfile", &Config::saveProfile, py::arg("profileName"),
             "Save the current configuration as a profile")
        .def("deleteProfile", &Config::deleteProfile, py::arg("profileName"),
             "Delete a configuration profile")
        .def("getAvailableProfiles", &Config::getAvailableProfiles,
             "Get list of available configuration profiles")
        
        // Profile presets
        .def("addDefaultProfile", &Config::addDefaultProfile, py::arg("name"),
             "Add a default profile with basic settings")
        .def("addScientificProfile", &Config::addScientificProfile, py::arg("name"),
             "Add a scientific research profile")
        .def("addPerformanceProfile", &Config::addPerformanceProfile, py::arg("name"),
             "Add a performance profile")
        .def("addDebugProfile", &Config::addDebugProfile, py::arg("name"),
             "Add a debug profile")
        
        // Environment variables
        .def("loadFromEnvironment", &Config::loadFromEnvironment, py::arg("prefix") = "NLM_",
             "Load configuration from environment variables")
        
        // Configuration validation rules
        .def("addValidationRule", &Config::addValidationRule, py::arg("key"), py::arg("rule"),
             "Add a validation rule for a configuration key")
        .def("removeValidationRule", &Config::removeValidationRule, py::arg("key"),
             "Remove validation rules for a configuration key")
        .def("getValidationRules", &Config::getValidationRules, py::arg("key"),
             "Get validation rules for a configuration key")
        
        // Configuration transformation
        .def("transform", [](Config& self, const std::function<ConfigValue(ConfigValue)>& func) {
            self.transform(func);
        }, py::arg("func"), "Transform all configuration values using a function")
        .def("filter", [](Config& self, const std::function<bool(const ConfigEntry&)>& predicate) {
            self.filter(predicate);
        }, py::arg("predicate"), "Filter configuration entries based on predicate")
        .def("sort", [](Config& self, const std::function<bool(const ConfigEntry&, const ConfigEntry&)>& comparator) {
            self.sort(comparator);
        }, py::arg("comparator"), "Sort configuration entries using comparator")
        
        // Export and import
        .def("exportToJSON", &Config::exportToJSON,
             "Export configuration to JSON string")
        .def("importFromJSON", &Config::importFromJSON, py::arg("json"),
             "Import configuration from JSON string")
        .def("exportToYAML", &Config::exportToYAML,
             "Export configuration to YAML string")
        .def("importFromYAML", &Config::importFromYAML, py::arg("yaml"),
             "Import configuration from YAML string")
        
        // Configuration debugging
        .def("enableDebugMode", &Config::enableDebugMode, py::arg("enabled"),
             "Enable or disable debug mode")
        .def("isDebugMode", &Config::isDebugMode,
             "Check if debug mode is enabled")
        .def("getDebugInfo", &Config::getDebugInfo,
             "Get debug information about the configuration")
        .def("logConfiguration", &Config::logConfiguration,
             "Log configuration (if debug mode is enabled)")
        
        // Configuration macros
        .def("addMacro", &Config::addMacro, py::arg("name"), py::arg("value"),
             "Add a configuration macro")
        .def("getMacro", &Config::getMacro, py::arg("name"),
             "Get a configuration macro value")
        .def("hasMacro", &Config::hasMacro, py::arg("name"),
             "Check if a configuration macro exists")
        
        // Change notification
        .def("addChangeCallback", &Config::addChangeCallback, py::arg("callback"),
             "Add a callback for configuration changes")
        .def("removeChangeCallback", &Config::removeChangeCallback, py::arg("callback"),
             "Remove a configuration change callback")
        
        // Configuration manipulation
        .def("increment", &Config::increment<int>, py::arg("key"), py::arg("amount") = 1,
             "Increment a numeric configuration value")
        .def("decrement", &Config::decrement<int>, py::arg("key"), py::arg("amount") = 1,
             "Decrement a numeric configuration value")
        
        // Configuration comparison
        .def("isGreater", &Config::isGreater, py::arg("key"), py::arg("value"),
             "Check if a configuration value is greater than another value")
        .def("isLess", &Config::isLess, py::arg("key"), py::arg("value"),
             "Check if a configuration value is less than another value")
        .def("isEqual", &Config::isEqual, py::arg("key"), py::arg("value"),
             "Check if a configuration value equals another value")
        
        // Configuration value manipulation
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.getSummary() + ">";
        })
        .def("__str__", [](const Config& cfg) {
            return cfg.getSummary();
        });

    // Enhanced SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("__repr__", [](const SensoryInput& input) {
            return "<SensoryInput: type=" + std::to_string(static_cast<int>(input.getType())) + ", dims=" + std::to_string(input.getDimensions()) + ">";
        });

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with advanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("setDataWithFormat", [](Vision& self, const std::vector<std::vector<float>>& imageData) {
            self.setData(imageData);
        }, py::arg("imageData"), "Set vision data as a 2D image")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of channels")
        .def("getResolution", [](const Vision& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get image resolution (width, height)")
        .def("getPixelCount", [](const Vision& self) {
            return self.getWidth() * self.getHeight();
        }, "Get total number of pixels")
        .def("normalize", &Vision::normalize, "Normalize pixel values to [0, 1]")
        .def("denormalize", &Vision::denormalize, "Denormalize pixel values")
        .def("applyFilter", [](Vision& self, const std::vector<float>& filterKernel) {
            self.applyFilter(filterKernel);
        }, py::arg("filterKernel"), "Apply a convolution filter to the image")
        .def("resize", [](Vision& self, size_t newWidth, size_t newHeight) {
            self.resize(newWidth, newHeight);
        }, py::arg("newWidth"), py::arg("newHeight"), "Resize the image")
        .def("enhanceContrast", &Vision::enhanceContrast, "Enhance image contrast")
        .def("detectEdges", &Vision::detectEdges, "Detect edges in the image")
        .def("getHistogram", &Vision::getHistogram, "Get intensity histogram")
        .def("getMeanIntensity", &Vision::getMeanIntensity, "Get mean intensity")
        .def("getStdDevIntensity", &Vision::getStdDevIntensity, "Get standard deviation of intensity")
        .def("isValid", &Vision::isValid, "Check if vision data is valid")
        .def("to_numpy", [](const Vision& self) {
            // This would require pybind11 with numpy support
            // For now, just return the data as a list
            return self.getData();
        }, "Get vision data as a list (requires pybind11 with numpy support)")
        .def("__repr__", [](const Vision& vision) {
            return "<Vision: " + std::to_string(vision.getWidth()) + "x" + std::to_string(vision.getHeight()) + ", " + std::to_string(vision.getChannels()) + " channels>";
        });

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with advanced features)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setWaveData", [](Audio& self, const std::vector<float>& waveData) {
            self.setWaveData(waveData);
        }, py::arg("waveData"), "Set audio waveform data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set sample rate")
        .def("setNumSamples", &Audio::setNumSamples, py::arg("numSamples"), "Set number of samples")
        .def("getSampleRate", &Audio::getSampleRate, "Get sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of samples")
        .def("getDuration", [](const Audio& self) {
            return static_cast<double>(self.getNumSamples()) / self.getSampleRate();
        }, "Get audio duration in seconds")
        .def("getFrequencySpectrum", &Audio::getFrequencySpectrum, "Get frequency spectrum")
        .def("getZeroCrossingRate", &Audio::getZeroCrossingRate, "Get zero crossing rate")
        .def("getEnergy", &Audio::getEnergy, "Get audio energy")
        .def("getRMS", &Audio::getRMS, "Get root mean square amplitude")
        .def("normalize", &Audio::normalize, "Normalize audio amplitude")
        .def("denormalize", &Audio::denormalize, "Denormalize audio amplitude")
        .def("applyFilter", [](Audio& self, const std::vector<float>& filterCoeffs) {
            self.applyFilter(filterCoeffs);
        }, py::arg("filterCoeffs"), "Apply a digital filter")
        .def("addReverb", &Audio::addReverb, "Add reverb effect")
        .def("addEcho", &Audio::addEcho, "Add echo effect")
        .def("compress", &Audio::compress, "Apply dynamic compression")
        .def("enhanceBass", &Audio::enhanceBass, "Enhance bass frequencies")
        .def("enhanceTreble", &Audio::enhanceTreble, "Enhance treble frequencies")
        .def("isValid", &Audio::isValid, "Check if audio data is valid")
        .def("__repr__", [](const Audio& audio) {
            return "<Audio: " + std::to_string(audio.getSampleRate()) + " Hz, " + std::to_string(audio.getNumSamples()) + " samples>";
        });

    // Enhanced InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals with advanced features)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("addSignalWithLabel", [](InternalSignals& self, const std::string& label, double value) {
            self.addSignalWithLabel(label, value);
        }, py::arg("label"), py::arg("value"), "Add a signal with a label")
        .def("getSignalCount", &InternalSignals::getSignalCount, "Get number of signals")
        .def("getSignalLabels", &InternalSignals::getSignalLabels, "Get all signal labels")
        .def("getSignalValues", &InternalSignals::getSignalValues, "Get all signal values")
        .def("getSignalByLabel", [](const InternalSignals& self, const std::string& label) {
            auto value = self.getSignalByLabel(label);
            if (value.has_value()) {
                return *value;
            }
            return 0.0;
        }, py::arg("label"), "Get signal value by label")
        .def("removeSignal", &InternalSignals::removeSignal, py::arg("label"), "Remove a signal")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getMeanSignal", &InternalSignals::getMeanSignal, "Get mean of all signals")
        .def("getMaxSignal", &InternalSignals::getMaxSignal, "Get maximum signal value")
        .def("getMinSignal", &InternalSignals::getMinSignal, "Get minimum signal value")
        .def("getSignalRange", &InternalSignals::getSignalRange, "Get signal range")
        .def("isSignalPresent", &InternalSignals::isSignalPresent, py::arg("label"), "Check if a signal is present")
        .def("normalizeSignals", &InternalSignals::normalizeSignals, "Normalize all signal values")
        .def("denormalizeSignals", &InternalSignals::denormalizeSignals, "Denormalize all signal values")
        .def("applyFunction", [](InternalSignals& self, const std::function<double(double)>& func) {
            self.applyFunction(func);
        }, py::arg("func"), "Apply a function to all signal values")
        .def("to_dict", [](const InternalSignals& self) {
            std::map<std::string, double> result;
            for (const auto& pair : self.getSignalMap()) {
                result[pair.first] = pair.second;
            }
            return result;
        }, "Convert to dictionary mapping labels to values")
        .def("from_dict", [](InternalSignals& self, const std::map<std::string, double>& data) {
            self.clearSignals();
            for (const auto& pair : data) {
                self.addSignalWithLabel(pair.first, pair.second);
            }
        }, py::arg("data"), "Create from dictionary mapping labels to values")
        .def("__repr__", [](const InternalSignals& signals) {
            return "<InternalSignals: " + std::to_string(signals.getSignalCount()) + " signals>";
        });

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Action with advanced properties)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getType", &Action::getType, "Get action type")
        .def("setValue", &Action::setValue, py::arg("value"), "Set action value")
        .def("getValue", &Action::getValue, "Get action value")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("addParameter", &Action::addParameter, py::arg("key"), py::arg("value"), "Add a parameter")
        .def("getParameter", &Action::getParameter, py::arg("key"), "Get a parameter value")
        .def("removeParameter", &Action::removeParameter, py::arg("key"), "Remove a parameter")
        .def("hasParameter", &Action::hasParameter, py::arg("key"), "Check if action has a parameter")
        .def("setPriority", &Action::setPriority, py::arg("priority"), "Set action priority")
        .def("getPriority", &Action::getPriority, "Get action priority")
        .def("setDuration", &Action::setDuration, py::arg("duration"), "Set action duration")
        .def("getDuration", &Action::getDuration, "Get action duration")
        .def("setEnergyCost", &Action::setEnergyCost, py::arg("energy"), "Set action energy cost")
        .def("getEnergyCost", &Action::getEnergyCost, "Get action energy cost")
        .def("setProbability", &Action::setProbability, py::arg("probability"), "Set action probability")
        .def("getProbability", &Action::getProbability, "Get action probability")
        .def("setContext", &Action::setContext, py::arg("context"), "Set action context")
        .def("getContext", &Action::getContext, "Get action context")
        .def("getDescription", &Action::getDescription, "Get action description")
        .def("to_string", &Action::to_string, "Convert action to string representation")
        .def("__repr__", [](const Action& action) {
            std::ostringstream oss;
            oss << "<Action: " << static_cast<int>(action.getType()) << " priority=" << action.getPriority() << ">";
            return oss.str();
        });

    // SimpleWorld class with enhanced features
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple world environment with enhanced features)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensors")
        .def("setVisionRange", &SimpleWorld::setVisionRange, py::arg("range"), "Set vision range")
        .def("setVisionAngle", &SimpleWorld::setVisionAngle, py::arg("angle"), "Set vision angle")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("energy"), "Set maximum agent energy")
        .def("setEnergyDecay", &SimpleWorld::setEnergyDecay, py::arg("decay"), "Set energy decay rate")
        .def("setVisionEnergyCost", &SimpleWorld::setVisionEnergyCost, py::arg("cost"), "Set vision energy cost")
        .def("setMovementEnergyCost", &SimpleWorld::setMovementEnergyCost, py::arg("cost"), "Set movement energy cost")
        .def("setInteractionEnergyCost", &SimpleWorld::setInteractionEnergyCost, py::arg("cost"), "Set interaction energy cost")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, "Get agent's sensory perception")
        .def("getAgentBody", &SimpleWorld::getAgentBody, "Get agent's body state")
        .def("getEnergy", &SimpleWorld::getEnergy, "Get current energy level")
        .def("setEnergy", &SimpleWorld::setEnergy, py::arg("energy"), "Set energy level")
        .def("addEnergy", &SimpleWorld::addEnergy, py::arg("amount"), "Add energy")
        .def("removeEnergy", &SimpleWorld::removeEnergy, py::arg("amount"), "Remove energy")
        .def("isEnergyDepleted", &SimpleWorld::isEnergyDepleted, "Check if energy is depleted")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("getStepCount", &SimpleWorld::getStepCount, "Get step count")
        .def("addObject", &SimpleWorld::addObject, py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 1.0, "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove an object from the world")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"), "Get object at position")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("command"), py::arg("time"), "Apply a motor command")
        .def("getVisionAt", &SimpleWorld::getVisionAt, py::arg("x"), py::arg("y"), "Get vision at position")
        .def("getAvailableActions", &SimpleWorld::getAvailableActions, "Get available actions at current position")
        .def("getReward", &SimpleWorld::getReward, py::arg("action"), "Get reward for an action")
        .def("setMaxSteps", &SimpleWorld::setMaxSteps, py::arg("maxSteps"), "Set maximum steps")
        .def("isDone", &SimpleWorld::isDone, "Check if world is done")
        .def("setDone", &SimpleWorld::setDone, py::arg("done"), "Set done state")
        .def("getConfig", &SimpleWorld::getConfig, "Get world configuration")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getConfig().getOr<int64_t>("world.width", 0)) + "x" + std::to_string(world.getConfig().getOr<int64_t>("world.height", 0)) + ">";
        });

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain with enhanced sensory-motor integration)pbdoc")
        .def(py::init<>())
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize agent with world")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enabled"), "Enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enabled"), "Enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enabled"), "Enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enabled"), "Enable curiosity")
        .def("enableNovelty", &AgentBrain::enableNovelty, py::arg("enabled"), "Enable novelty detection")
        .def("enablePrediction", &AgentBrain::enablePrediction, py::arg("enabled"), "Enable prediction system")
        .def("enableAttention", &AgentBrain::enableAttention, py::arg("enabled"), "Enable attention system")
        .def("enablePlanning", &AgentBrain::enablePlanning, py::arg("enabled"), "Enable planning system")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory input")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode motor command from brain activity")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("prediction"), "Apply reward modulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development state")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get neuromodulation level")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get developmental stage")
        .def("getPlanningDepth", &AgentBrain::getPlanningDepth, "Get planning depth")
        .def("setPlanningDepth", &AgentBrain::setPlanningDepth, py::arg("depth"), "Set planning depth")
        .def("getAttentionStrength", &AgentBrain::getAttentionStrength, "Get attention strength")
        .def("setAttentionStrength", &AgentBrain::setAttentionStrength, py::arg("strength"), "Set attention strength")
        .def("getExcitationInhibitionRatio", &AgentBrain::getExcitationInhibitionRatio, "Get E/I ratio")
        .def("getTotalSpikeCount", &AgentBrain::getTotalSpikeCount, "Get total spike count")
        .def("getFiringNeuronCount", &AgentBrain::getFiringNeuronCount, "Get firing neuron count")
        .def("getAverageFiringRate", &AgentBrain::getAverageFiringRate, "Get average firing rate")
        .def("getActiveNeuronCount", &AgentBrain::getActiveNeuronCount, "Get active neuron count")
        .def("getRegionCount", &AgentBrain::getRegionCount, "Get region count")
        .def("getTotalNeuronCount", &AgentBrain::getTotalNeuronCount, "Get total neuron count")
        .def("getTotalSynapseCount", &AgentBrain::getTotalSynapseCount, "Get total synapse count")
        .def("logStatus", &AgentBrain::logStatus, "Log agent status")
        .def("save", &AgentBrain::save, py::arg("filepath"), "Save agent state")
        .def("load", &AgentBrain::load, py::arg("filepath"), "Load agent state")
        .def("reset", &AgentBrain::reset, "Reset agent state")
        .def("getBrain", &AgentBrain::getBrain, "Get underlying brain")
        .def("getWorld", &AgentBrain::getWorld, "Get associated world")
        .def("getConfig", &AgentBrain::getConfig, "Get agent configuration")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: " + agent.getConfig().getOr<std::string>("agent.name", "unnamed") + ">";
        });

    // SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory perception with advanced features)pbdoc")
        .def(py::init<>())
        .def("addVision", &SensoryPercept::addVision, py::arg("vision"), "Add vision input")
        .def("addAudio", &SensoryPercept::addAudio, py::arg("audio"), "Add audio input")
        .def("addInternal", &SensoryPercept::addInternal, py::arg("internal"), "Add internal signals")
        .def("getVision", &SensoryPercept::getVision, "Get vision input")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio input")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("hasVision", &SensoryPercept::hasVision, "Check if has vision")
        .def("hasAudio", &SensoryPercept::hasAudio, "Check if has audio")
        .def("hasInternal", &SensoryPercept::hasInternal, "Check if has internal signals")
        .def("getAllInputs", &SensoryPercept::getAllInputs, "Get all sensory inputs")
        .def("getInputTypes", &SensoryPercept::getInputTypes, "Get all input types")
        .def("getTotalInputDimensions", &SensoryPercept::getTotalInputDimensions, "Get total input dimensions")
        .def("normalize", &SensoryPercept::normalize, "Normalize all inputs")
        .def("denormalize", &SensoryPercept::denormalize, "Denormalize all inputs")
        .def("applyFunction", [](SensoryPercept& self, const std::function<std::vector<float>(const std::vector<float>&)>& func) {
            self.applyFunction(func);
        }, py::arg("func"), "Apply a function to all inputs")
        .def("to_dict", [](const SensoryPercept& percept) {
            std::map<std::string, py::Any> result;
            auto inputs = percept.getAllInputs();
            for (size_t i = 0; i < inputs.size(); ++i) {
                result[percept.getInputTypes()[i]] = py::cast(inputs[i]);
            }
            return result;
        }, "Convert to dictionary mapping input types to values")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: " + std::to_string(percept.getInputTypes().size()) + " input types>";
        });

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body with advanced features)pbdoc")
        .def(py::init<>())
        .def("setPosition", &AgentBody::setPosition, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getPosition", &AgentBody::getPosition, "Get agent position")
        .def("setOrientation", &AgentBody::setOrientation, py::arg("angle"), "Set agent orientation")
        .def("getOrientation", &AgentBody::getOrientation, "Get agent orientation")
        .def("setEnergy", &AgentBody::setEnergy, py::arg("energy"), "Set agent energy")
        .def("getEnergy", &AgentBody::getEnergy, "Get agent energy")
        .def("addEnergy", &AgentBody::addEnergy, py::arg("amount"), "Add energy")
        .def("removeEnergy", &AgentBody::removeEnergy, py::arg("amount"), "Remove energy")
        .def("setMaxEnergy", &AgentBody::setMaxEnergy, py::arg("energy"), "Set maximum energy")
        .def("getMaxEnergy", &AgentBody::getMaxEnergy, "Get maximum energy")
        .def("setHealth", &AgentBody::setHealth, py::arg("health"), "Set agent health")
        .def("getHealth", &AgentBody::getHealth, "Get agent health")
        .def("setAge", &AgentBody::setAge, py::arg("age"), "Set agent age")
        .def("getAge", &AgentBody::getAge, "Get agent age")
        .def("setAlive", &AgentBody::setAlive, py::arg("alive"), "Set alive state")
        .def("isAlive", &AgentBody::isAlive, "Check if agent is alive")
        .def("setMemory", &AgentBody::setMemory, py::arg("memory"), "Set agent memory")
        .def("getMemory", &AgentBody::getMemory, "Get agent memory")
        .def("addMemoryEntry", &AgentBody::addMemoryEntry, py::arg("key"), py::arg("value"), "Add memory entry")
        .def("getMemoryEntry", &AgentBody::getMemoryEntry, py::arg("key"), "Get memory entry")
        .def("removeMemoryEntry", &AgentBody::removeMemoryEntry, py::arg("key"), "Remove memory entry")
        .def("clearMemory", &AgentBody::clearMemory, "Clear all memory")
        .def("to_dict", [](const AgentBody& body) {
            std::map<std::string, py::Any> result;
            auto pos = body.getPosition();
            result["position"] = py::cast(std::vector<double>{pos.first, pos.second});
            result["orientation"] = body.getOrientation();
            result["energy"] = body.getEnergy();
            result["max_energy"] = body.getMaxEnergy();
            result["health"] = body.getHealth();
            result["age"] = body.getAge();
            result["alive"] = body.isAlive();
            return result;
        }, "Convert to dictionary")
        .def("__repr__", [](const AgentBody& body) {
            auto pos = body.getPosition();
            return "<AgentBody: position=(" + std::to_string(pos.first) + ", " + std::to_string(pos.second) + ")>";
        });

    // Convenience functions
    m.def("createDefaultConfig", []() {
        Config config;
        config.set("brain.neuron_count", 1000);
        config.set("brain.synapse_density", 0.1);
        config.set("brain.connection_probability", 0.05);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.stdp.learning_rate", 0.001);
        config.set("plasticity.hebbian.enable", true);
        config.set("neuromod.dopamine.scale", 1.0);
        config.set("neuromod.curiosity.enable", true);
        config.set("neuromod.novelty.enable", true);
        return config;
    }, "Create a default configuration");

    m.def("createBrain", [](const Config& config) {
        return std::make_shared<Brain>(std::make_shared<Config>(config));
    }, py::arg("config"), "Create a brain with configuration");

    m.def("createSimpleWorld", []() {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple world");

    m.def("createAgentBrain", [](const std::shared_ptr<Brain>& brain) {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain");

    m.def("createVision", [](size_t width, size_t height, size_t channels) {
        auto vision = std::make_shared<Vision>();
        vision->resize(width, height);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a vision input");

    m.def("createAudio", [](size_t sampleRate, size_t numSamples) {
        auto audio = std::make_shared<Audio>();
        audio->setSampleRate(sampleRate);
        audio->setNumSamples(numSamples);
        return audio;
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create an audio input");

    m.def("createInternalSignals", []() {
        return std::make_shared<InternalSignals>();
    }, "Create internal signals input");

    // Example utilities
    m.def("run_example_simulation", []() {
        auto config = createDefaultConfig();
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        
        for (int step = 0; step < 100; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return std::make_tuple(brain, world, agent);
    }, "Run a complete example simulation");

    m.def("run_learning_example", []() {
        auto config = createDefaultConfig();
        config.set("brain.neuron_count", 500);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.stdp.learning_rate", 0.01);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        auto world = createSimpleWorld();
        world->configure(10, 10, 5, 5);
        world->reset();
        
        auto agent = createAgentBrain(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        
        // Run learning simulation
        for (int step = 0; step < 500; ++step) {
            world->update(0.05);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward based on action success
            float reward = world->getReward(action);
            agent->applyRewardModulation(reward, 0.0);
            agent->updateDevelopment(0.05);
        }
        
        return std::make_tuple(brain, world, agent);
    }, "Run a learning example simulation");

    m.def("run_stdp_example", []() {
        auto config = createDefaultConfig();
        config.set("brain.neuron_count", 200);
        config.set("plasticity.stdp.enable", true);
        config.set("plasticity.stdp.learning_rate", 0.05);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        // Apply strong current to sensory neurons to trigger STDP
        for (int step = 0; step < 1000; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 50.0f);
            brain->step(step);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() << " spikes, " 
                          << brain->getFiringNeuronCount() << " firing neurons" << std::endl;
            }
        }
        
        return brain;
    }, "Run an STDP example simulation");

    m.def("run_plasticity_example", []() {
        auto config = createDefaultConfig();
        config.set("brain.neuron_count", 300);
        config.set("plasticity.hebbian.enable", true);
        config.set("plasticity.stdp.enable", true);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        // Create correlated activity patterns to trigger Hebbian learning
        for (int step = 0; step < 800; ++step) {
            // Inject correlated activity to two neurons
            if (step % 2 == 0) {
                brain->injectCurrentToNeurons(NeuronType::Internal, 30.0f);
            } else {
                brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
            }
            
            brain->step(step);
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() << " spikes" << std::endl;
            }
        }
        
        return brain;
    }, "Run a plasticity example simulation");

    m.def("run_development_example", []() {
        auto config = createDefaultConfig();
        config.set("brain.neuron_count", 100);
        config.set("neuromod.dopamine.scale", 2.0);
        
        auto brain = createBrain(config);
        brain->initialize();
        
        // Run development simulation
        for (int step = 0; step < 1000; ++step) {
            brain->injectCurrentToNeurons(NeuronType::Sensory, 20.0f);
            brain->step(step);
            
            // Update development
            brain->develop();
            
            if (step % 100 == 0) {
                std::cout << "Step " << step << ": " << brain->getTotalSpikeCount() << " spikes, " 
                          << brain->getFiringNeuronCount() << " firing neurons, "
                          << "development stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            }
        }
        
        return brain;
    }, "Run a development example simulation");

    // Performance testing utilities
    m.def("benchmark_brain_creation", [](int numNeurons) {
        auto config = createDefaultConfig();
        config.set("brain.neuron_count", numNeurons);
        
        auto brain = createBrain(config);
        
        auto start = std::chrono::high_resolution_clock::now();
        brain->initialize();
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> elapsed = end - start;
        return std::make_tuple(brain, elapsed.count());
    }, py::arg("numNeurons") = 1000, "Benchmark brain creation time");

    m.def("benchmark_simulation_step", [](std::shared_ptr<Brain> brain, int numSteps) {
        auto start = std::chrono::high_resolution_clock::now();
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step);
        }
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double> elapsed = end - start;
        return std::make_tuple(brain, elapsed.count());
    }, py::arg("brain"), py::arg("numSteps"), "Benchmark simulation step time");

    m.def("get_version_info", []() {
        std::map<std::string, std::string> info;
        info["version"] = "NLM Enhanced Python Bindings v2.0.0";
        info["cpp_version"] = "C++20";
        info["build_type"] = "Release";
        info["features"] = "Advanced Configuration, Enhanced Brain, Improved Agent, Multiple Environments";
        return info;
    }, "Get version information");

    // Error handling utilities
    m.def("safe_load_config", [](const std::string& filepath) {
        try {
            Config config;
            bool success = config.loadFromFile(filepath);
            if (!success) {
                throw std::runtime_error("Failed to load configuration from " + filepath);
            }
            return config;
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Configuration error: ") + e.what());
        }
    }, py::arg("filepath"), "Safely load configuration with error handling");

    m.def("validate_config", [](const Config& config) {
        try {
            bool valid = config.validate();
            if (!valid) {
                throw std::runtime_error("Configuration validation failed");
            }
            return true;
        } catch (const std::exception& e) {
            throw py::value_error(std::string("Validation error: ") + e.what());
        }
    }, py::arg("config"), "Validate configuration with error handling");

    // Utility functions
    m.def("print_config_summary", [](const Config& config) {
        std::cout << config.getSummary() << std::endl;
    }, py::arg("config"), "Print configuration summary");

    m.def("wait", [](double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }, py::arg("seconds"), "Wait for specified time (for simulation pacing)");

    m.def("get_current_time", []() {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = now.time_since_epoch();
        return std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    }, "Get current time in milliseconds");

    // Examples and tutorials
    m.doc() += R"pbdoc(

Additional Examples and Tutorials:

1. Basic Simulation:
   ```python
   import pynlm
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   for i in range(100):
       brain.step(i)
   ```

2. Complete Agent Simulation:
   ```python
   import pynlm
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   brain.initialize()
   world = pynlm.createSimpleWorld()
   world.configure(20, 20, 8, 8)
   world.reset()
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   agent.enableRewardModulation(True)
   
   for step in range(500):
       world.update(0.1)
       percept = world.getSensoryPercept()
       agent.processSensoryInput(percept)
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())
   ```

3. Learning Simulation:
   ```python
   import pynlm
   config = pynlm.createDefaultConfig()
   config.set("brain.neuron_count", 500)
   config.set("plasticity.stdp.enable", True)
   config.set("neuromod.dopamine.scale", 2.0)
   
   brain = pynlm.createBrain(config)
   brain.initialize()
   world = pynlm.createSimpleWorld()
   world.configure(10, 10, 5, 5)
   
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   agent.enableRewardModulation(True)
   agent.enableStructuralPlasticity(True)
   
   for step in range(1000):
       world.update(0.05)
       percept = world.getSensoryPercept()
       agent.processSensoryInput(percept)
       brain.step(step)
       action = agent.decodeMotorCommand()
       world.applyMotorCommand(action, world.getSimulationTime())
   ```

4. Advanced Configuration:
   ```python
   import pynlm
   config = pynlm.createDefaultConfig()
   
   # Create custom profile
   config.set("brain.neuron_count", 2000)
   config.set("brain.synapse_density", 0.15)
   config.set("plasticity.stdp.learning_rate", 0.005)
   config.set("plasticity.hebbian.enable", False)
   config.set("neuromod.curiosity.enable", True)
   config.set("neuromod.novelty.enable", True)
   config.set("neuromod.dopamine.scale", 1.5)
   ```

5. Performance Profiling:
   ```python
   import pynlm
   import time
   
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   
   start_time = pynlm.get_current_time()
   brain.initialize()
   init_time = pynlm.get_current_time() - start_time
   
   start_time = pynlm.get_current_time()
   for i in range(1000):
       brain.step(i)
   simulation_time = pynlm.get_current_time() - start_time
   
   print(f"Initialization: {init_time:.2f}ms")
   print(f"1000 steps: {simulation_time:.2f}ms ({simulation_time/1000:.2f}ms/step)")
   ```

6. Error Handling and Validation:
   ```python
   import pynlm
   try:
       config = pynlm.safe_load_config("config.json")
       if pynlm.validate_config(config):
           brain = pynlm.createBrain(config)
           brain.initialize()
           print("Configuration loaded and validated successfully")
   except Exception as e:
       print(f"Error: {e}")
   ```

7. Configuration Management:
   ```python
   import pynlm
   config = pynlm.createDefaultConfig()
   
   # Backup configuration
   config.backup("config_backup.json")
   
   # Modify configuration
   config.set("brain.neuron_count", 1500)
   config.set("plasticity.stdp.learning_rate", 0.02)
   
   # Save to file
   config.saveToFile("config.json")
   
   # Load from file
   new_config = pynlm.createDefaultConfig()
   new_config.loadFromFile("config.json")
   ```

8. Advanced Sensory Processing:
   ```python
   import pynlm
   import numpy as np
   
   # Create advanced vision input
   vision = pynlm.createVision(64, 64, 3)
   # Simulate image data
   image_data = np.random.rand(64, 64, 3).astype(float) * 255
   vision.setData(image_data.flatten())
   
   # Normalize and enhance
   vision.normalize()
   vision.enhanceContrast()
   
   # Detect edges
   edges = vision.detectEdges()
   
   # Process with agent
   config = pynlm.createDefaultConfig()
   brain = pynlm.createBrain(config)
   world = pynlm.createSimpleWorld()
   agent = pynlm.createAgentBrain(brain)
   agent.initialize(world)
   ```

For more detailed documentation and examples, see the accompanying documentation files.
    )pbdoc";

} // namespace nlm
