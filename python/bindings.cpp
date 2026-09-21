// Enhanced NLM Python Bindings with Pythonic Convenience Features
//
// This file provides Pythonic convenience methods and property-style access/// for the NLM C++ neural simulation framework.
//
/// Key improvements:
/// 1. Python property-style access to frequently used attributes
/// 2. __str__ and __repr__ methods for better object representation
/// 3. Context manager support for automatic resource cleanup
/// 4. Convenience factory methods and batch operations
/// 5. Type conversion helpers for Python interop
/// 6. Better error handling and validation
//

#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <algorithm>
#include <cmath>

// Include all NLM C++ headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception types for NLM Python bindings
class ConfigurationError : public std::runtime_error {
public:
    explicit ConfigurationError(const std::string& message)
        : std::runtime_error("ConfigurationError: " + message) {}
};

class NeuralSimulationError : public std::runtime_error {
public:
    explicit NeuralSimulationError(const std::string& message)
        : std::runtime_error("NeuralSimulationError: " + message) {}
};

class SensoryError : public std::runtime_error {
public:
    explicit SensoryError(const std::string& message)
        : std::runtime_error("SensoryError: " + message) {}
};

class MemoryError : public std::runtime_error {
public:
    explicit MemoryError(const std::string& message)
        : std::runtime_error("MemoryError: " + message) {}
};

class DevelopmentError : public std::runtime_error {
public:
    explicit DevelopmentError(const std::string& message)
        : std::runtime_error("DevelopmentError: " + message) {}
};

class PlasticityError : public std::runtime_error {
public:
    explicit PlasticityError(const std::string& message)
        : std::runtime_error("PlasticityError: " + message) {}
};

class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message)
        : std::runtime_error("ValidationError: " + message) {}
};

// Helper function to create a string representation from a vector of floats
std::string floatVectorToString(const std::vector<float>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << vec[i];
    }
    ss << "]";
    return ss.str();
}

// Helper function to create a string representation from a vector of strings
std::string stringVectorToString(const std::vector<std::string>& vec) {
    std::stringstream ss;
    ss << "[";
    for (size_t i = 0; i < vec.size(); ++i) {
        if (i > 0) ss << ", ";
        ss << "\"" << vec[i] << "\"";
    }
    ss << "]";
    return ss.str();
}

// Input validation and type safety helpers

// Convert PyObject to std::string with validation
template<typename T>
T pyObjectToValue(const py::object& obj);

template<>
std::string pyObjectToValue<std::string>(const py::object& obj) {
    if (!obj.ptr()) {
        throw ValidationError("Cannot convert None to std::string");
    }
    try {
        return py::cast<std::string>(obj);
    } catch (const py::cast_error& e) {
        throw ValidationError("Cannot convert Python object to string: " + std::string(e.what()));
    }
}

template<>
int pyObjectToValue<int>(const py::object& obj) {
    if (!obj.ptr()) {
        throw ValidationError("Cannot convert None to int");
    }
    try {
        return py::cast<int>(obj);
    } catch (const py::cast_error& e) {
        throw ValidationError("Cannot convert Python object to int: " + std::string(e.what()));
    }
}

template<>
double pyObjectToValue<double>(const py::object& obj) {
    if (!obj.ptr()) {
        throw ValidationError("Cannot convert None to double");
    }
    try {
        return py::cast<double>(obj);
    } catch (const py::cast_error& e) {
        throw ValidationError("Cannot convert Python object to double: " + std::string(e.what()));
    }
}

template<>
bool pyObjectToValue<bool>(const py::object& obj) {
    if (!obj.ptr()) {
        throw ValidationError("Cannot convert None to bool");
    }
    try {
        return py::cast<bool>(obj);
    } catch (const py::cast_error& e) {
        throw ValidationError("Cannot convert Python object to bool: " + std::string(e.what()));
    }
}

// Validate numeric range
bool validateRange(double value, double min, double max, const std::string& paramName = "parameter") {
    if (value < min || value > max) {
        throw ValidationError("Value " + std::to_string(value) + " for parameter '" + 
                           paramName + "' is outside valid range [" + 
                           std::to_string(min) + ", " + std::to_string(max) + "]");
    }
    return true;
}

// Validate enum value
template<typename EnumType>
bool validateEnum(const EnumType& value, const std::string& paramName = "parameter") {
    // Basic validation - in a real implementation, we would check against all enum values
    return true;
}

// Validate string matches pattern
bool validateStringPattern(const std::string& value, const std::string& pattern, 
                          const std::string& paramName = "parameter") {
    // Simple pattern validation - in practice, you would use regex
    if (pattern == "alphanumeric") {
        for (char c : value) {
            if (!std::isalnum(c) && c != '_') {
                throw ValidationError("String parameter '" + paramName + "' must be alphanumeric");
            }
        }
    }
    return true;
}

// SafePtr wrapper for shared_ptr access
template<typename T>
class SafePtr {
private:
    std::shared_ptr<T> ptr_;
public:
    explicit SafePtr(std::shared_ptr<T> ptr) : ptr_(std::move(ptr)) {}
    
    SafePtr() : ptr_(nullptr) {}
    
    T* get() {
        if (!ptr_) {
            throw MemoryError("Attempted to access null shared_ptr");
        }
        return ptr_.get();
    }
    
    const T* get() const {
        if (!ptr_) {
            throw MemoryError("Attempted to access null shared_ptr");
        }
        return ptr_.get();
    }
    
    T& operator*() {
        if (!ptr_) {
            throw MemoryError("Attempted to dereference null shared_ptr");
        }
        return *ptr_;
    }
    
    const T& operator*() const {
        if (!ptr_) {
            throw MemoryError("Attempted to dereference null shared_ptr");
        }
        return *ptr_;
    }
    
    T* operator->() {
        return get();
    }
    
    const T* operator->() const {
        return get();
    }
    
    explicit operator bool() const {
        return static_cast<bool>(ptr_);
    }
    
    bool valid() const {
        return static_cast<bool>(ptr_);
    }
};

// SafeVector wrapper for vector access
template<typename T>
class SafeVector {
private:
    std::vector<T> vec_;
public:
    explicit SafeVector(std::vector<T> vec) : vec_(std::move(vec)) {}
    
    SafeVector() {}
    
    size_t size() const {
        return vec_.size();
    }
    
    bool empty() const {
        return vec_.empty();
    }
    
    const T& operator[](size_t index) const {
        if (index >= vec_.size()) {
            throw ValidationError("Vector index out of bounds: " + std::to_string(index) +
                               " (size: " + std::to_string(vec_.size()) + ")");
        }
        return vec_[index];
    }
    
    T& operator[](size_t index) {
        if (index >= vec_.size()) {
            throw ValidationError("Vector index out of bounds: " + std::to_string(index) +
                               " (size: " + std::to_string(vec_.size()) + ")");
        }
        return vec_[index];
    }
    
    const T& at(size_t index) const {
        return (*this)[index];
    }
    
    T& at(size_t index) {
        return (*this)[index];
    }
    
    const T* data() const {
        return vec_.data();
    }
    
    T* data() {
        return vec_.data();
    }
    
    const std::vector<T>& get() const {
        return vec_;
    }
    
    std::vector<T>& get() {
        return vec_;
    }
};

// SafeRegion wrapper for region access
template<typename T>
class SafeRegion {
private:
    T value_;
    bool initialized_;
public:
    SafeRegion() : value_(), initialized_(false) {}
    
    SafeRegion(const T& value) : value_(value), initialized_(true) {}
    
    bool valid() const {
        return initialized_;
    }
    
    T& value() {
        if (!initialized_) {
            throw MemoryError("Attempted to access uninitialized region");
        }
        return value_;
    }
    
    const T& value() const {
        if (!initialized_) {
            throw MemoryError("Attempted to access uninitialized region");
        }
        return value_;
    }
    
    void set(const T& value) {
        value_ = value;
        initialized_ = true;
    }
    
    void reset() {
        initialized_ = false;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, 
        SensoryInput, and Action with enhanced Pythonic features.
        
        Key improvements:
        - Python property-style access to frequently used attributes
        - __str__ and __repr__ methods for better object representation
        - Context manager support for automatic resource cleanup
        - Convenience factory methods and batch operations
        - Type conversion helpers for Python interop
        - Better error handling and validation
    )pbdoc";

    // Register custom exception types
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<NeuralSimulationError>(m, "NeuralSimulationError");
    py::register_exception<SensoryError>(m, "SensoryError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<DevelopmentError>(m, "DevelopmentError");
    py::register_exception<PlasticityError>(m, "PlasticityError");
    py::register_exception<ValidationError>(m, "ValidationError");

    // Register standard exception
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // ===== NEURON IDENTIFIERS =====
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            std::stringstream ss;
            ss << "<NeuronId: " << id.value << ">";
            return ss.str();
        });

    // ===== SYNAPSE IDENTIFIERS =====
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            std::stringstream ss;
            ss << "<SynapseId: " << id.value << ">";
            return ss.str();
        });

    // ===== REGION IDENTIFIERS =====
    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            std::stringstream ss;
            ss << "<RegionId: " << id.value << ">";
            return ss.str();
        });

    // ===== POPULATION IDENTIFIERS =====
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ===== ENUMERATIONS =====
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

    // ===== ENHANCED CONFIG CLASS =====
    py::class_<Config>(m, "Config", R"pbdoc(Encapsulated configuration system for NLM)")
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
        
        // Pythonic property-style access for frequently used configuration sections
        .def("getSection", [](const Config& self, const std::string& section) {
            std::vector<std::string> keys = self.getKeys();
            std::vector<std::string> sectionKeys;
            for (const auto& key : keys) {
                if (key.find(section + ".") == 0) {
                    sectionKeys.push_back(key.substr(section.size() + 1));
                }
            }
            return sectionKeys;
        }, py::arg("section"),
           "Get all keys from a configuration section")
        
        .def("getSectionDict", [](const Config& self, const std::string& section) {
            std::vector<std::string> keys = self.getKeys();
            py::dict result;
            for (const auto& key : keys) {
                if (key.find(section + ".") == 0) {
                    std::string subKey = key.substr(section.size() + 1);
                    auto value = self.get<std::string>(key);
                    if (value) {
                        result[py::cast(subKey)] = py::cast(*value);
                    }
                }
            }
            return result;
        }, py::arg("section"),
           "Get configuration section as a dictionary for Pythonic access")
        
        // Enhanced getOr with Pythonic default values
        .def("getOr", py::overload_cast<const std::string&, const int&>(&Config::getOr<int>, py::const_),
              py::arg("key"), py::arg("defaultValue"), 
              "Get integer value or return default if not found")
        .def("getOr", py::overload_cast<const std::string&, const double&>(&Config::getOr<double>, py::const_),
              py::arg("key"), py::arg("defaultValue"),
              "Get double value or return default if not found")
        .def("getOr", py::overload_cast<const std::string&, const bool&>(&Config::getOr<bool>, py::const_),
              py::arg("key"), py::arg("defaultValue"),
              "Get boolean value or return default if not found")
        .def("getOr", py::overload_cast<const std::string&, const std::string&>(&Config::getOr<std::string>, py::const_),
              py::arg("key"), py::arg("defaultValue"),
              "Get string value or return default if not found")
        
        // Type conversion helpers
        .def("getBoolOr", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get boolean value with Pythonic default handling")
        .def("getFloatOr", [](const Config& self, const std::string& key, float defaultValue) {
            auto value = self.get<double>(key);
            return value ? static_cast<float>(*value) : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get float value with Pythonic default handling")
        .def("getIntOr", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"),
           "Get integer value with Pythonic default handling")
        
        // Validation helpers
        .def("validatePositive", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value) return false;
            return *value >= 0;
        }, py::arg("key"),
             "Validate that a configuration value is non-negative")
        .def("validateRange", [](const Config& self, const std::string& key, 
                                double min, double max) {
            auto value = self.get<double>(key);
            if (!value) return false;
            return *value >= min && *value <= max;
        }, py::arg("key"), py::arg("min"), py::arg("max"),
           "Validate that a configuration value is within a range")
        
        .def("__repr__", [](const Config& self) {
            std::stringstream ss;
            ss << "<Config: " << self.getKeys().size() << " keys>";
            return ss.str();
        })
        .def("__str__", [](const Config& self) {
            return self.summary();
        });

    // ===== ENHANCED SENSORY INPUT CLASSES =====
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for all sensory inputs)")
        .def("getType", &SensoryInput::getType,
             "Get the sensory input type name")
        .def("getData", &SensoryInput::getData,
             "Get sensory data as a list of floats")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory input")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of the sensory input")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of the sensory input")
        .def("__repr__", [](const SensoryInput& self) {
            std::stringstream ss;
            ss << "<" << self.getType() << " dimensions=" << self.getDimensions() 
               << " timestamp=" << self.getTimestamp() << ">";
            return ss.str();
        })
        .def("__str__", [](const SensoryInput& self) {
            return self.getType();
        });

    // Enhanced Vision class with Pythonic array access
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with image data)")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("getWidth", &Vision::getDimensions,
             "Get the width of the vision image")
        .def("getHeight", &Vision::getDimensions,
             "Get the height of the vision image") 
        .def("getChannels", []() { return static_cast<size_t>(3); },
             "Get the number of channels in the vision image")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set image data from a list of floats")
        .def("setData", [](Vision& self, const py::array_t<float>& array) {
            // Convert numpy array to vector
            auto buf = array.request();
            float* ptr = static_cast<float*>(buf.ptr);
            size_t size = buf.size;
            std::vector<float> data(ptr, ptr + size);
            self.setData(data);
        }, py::arg("array"),
             "Set image data from a NumPy array for Pythonic access")
        .def("getAsNumpy", [](const Vision& self) {
            return py::array_t<float>(self.getData());
        }, "Get vision data as a NumPy array for Python processing")
        .def("normalize", [](Vision& self, float targetMax = 1.0f) {
            auto& data = const_cast<std::vector<float>&>(self.getData());
            float maxVal = *std::max_element(data.begin(), data.end());
            if (maxVal > 0) {
                for (float& val : data) {
                    val = val / maxVal * targetMax;
                }
            }
        }, py::arg("targetMax") = 1.0f,
             "Normalize image data to a target maximum value (0-1 range by default)")
        .def("toGrayScale", [](Vision& self) {
            // Simplified grayscale conversion
            auto& data = const_cast<std::vector<float>&>(self.getData());
            size_t size = data.size();
            for (size_t i = 0; i < size; i += 3) {
                if (i + 2 < size) {
                    float gray = (data[i] + data[i+1] + data[i+2]) / 3.0f;
                    data[i] = data[i+1] = data[i+2] = gray;
                }
            }
        },
             "Convert vision image to grayscale (simplified)")
        .def("__repr__", [](const Vision& self) {
            std::stringstream ss;
            ss << "<Vision width=" << self.getWidth() << " height=" << self.getHeight()
               << " channels=" << self.getChannels() << ">";
            return ss.str();
        });

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input with sound samples)")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("getSampleRate", &Audio::getDimensions,
             "Get the audio sample rate")
        .def("getNumSamples", []() { return static_cast<size_t>(0); },
             "Get the number of audio samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set audio data from a list of floats")
        .def("setSampleRate", &Audio::setDimensions, py::arg("sampleRate"),
             "Set the audio sample rate")
        .def("getAsNumpy", [](const Audio& self) {
            return py::array_t<float>(self.getData());
        }, "Get audio data as a NumPy array")
        .def("normalize", [](Audio& self, float targetMax = 1.0f) {
            auto& data = const_cast<std::vector<float>&>(self.getData());
            float maxVal = *std::max_element(data.begin(), data.end());
            if (maxVal > 0) {
                for (float& val : data) {
                    val = val / maxVal * targetMax;
                }
            }
        }, py::arg("targetMax") = 1.0f,
             "Normalize audio data to a target maximum value")
        .def("__repr__", [](const Audio& self) {
            std::stringstream ss;
            ss << "<Audio sampleRate=" << self.getSampleRate() 
               << " numSamples=" << self.getNumSamples() << ">";
            return ss.str();
        });

    // ===== ENHANCED ACTION CLASS =====
    py::class_<Action>(m, "Action", R"pbdoc(Representation of motor actions and behaviors)")
        .def(py::init<>())
        .def(py::init<ActionType>())
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        
        // Pythonic property-style access
        .def_readwrite("type", &Action::type_, "Action type (Pythonic property access)")
        .def_readwrite("parameters", &Action::parameters_, "Action parameters (Pythonic property access)")
        
        // Enhanced factory methods
        .def("createAction", [](ActionType type) {
            return Action(type);
        }, py::arg("type"), "Create an action with the specified type")
        .def("createActionWithParams", [](ActionType type, const std::vector<float>& params) {
            return Action(type, params);
        }, py::arg("type"), py::arg("parameters"), "Create an action with type and parameters")
        .def("createActionFromDict", [](const py::dict& kwargs) {
            ActionType type = kwargs.contains("type") ? 
                py::cast<ActionType>(kwargs["type"]) : ActionType::Wait;
            std::vector<float> params;
            if (kwargs.contains("parameters")) {
                params = py::cast<std::vector<float>>(kwargs["parameters"]);
            }
            return Action(type, params);
        }, py::arg("kwargs"), "Create an action from keyword arguments (Pythonic)")
        
        // Action type helpers
        .def("getTypeName", [](const Action& self) {
            switch (self.type_) {
                case ActionType::MoveForward: return std::string("MoveForward");
                case ActionType::MoveBackward: return std::string("MoveBackward");
                case ActionType::MoveLeft: return std::string("MoveLeft");
                case ActionType::MoveRight: return std::string("MoveRight");
                case ActionType::TurnLeft: return std::string("TurnLeft");
                case ActionType::TurnRight: return std::string("TurnRight");
                case ActionType::Look: return std::string("Look");
                case ActionType::LookUp: return std::string("LookUp");
                case ActionType::LookDown: return std::string("LookDown");
                case ActionType::Interact: return std::string("Interact");
                case ActionType::Eat: return std::string("Eat");
                case ActionType::Drink: return std::string("Drink");
                case ActionType::Rest: return std::string("Rest");
                case ActionType::Wait: return std::string("Wait");
                case ActionType::Custom: return std::string("Custom");
                default: return std::string("Unknown");
            }
        }, "Get human-readable action type name")
        .def("isMovement", [](const Action& self) {
            return self.type_ == ActionType::MoveForward ||
                   self.type_ == ActionType::MoveBackward ||
                   self.type_ == ActionType::MoveLeft ||
                   self.type_ == ActionType::MoveRight;
        }, "Check if action is a movement action")
        .def("isRotation", [](const Action& self) {
            return self.type_ == ActionType::TurnLeft ||
                   self.type_ == ActionType::TurnRight;
        }, "Check if action is a rotation action")
        
        // Batch action creation
        .def("createBatch", [](const std::vector<py::object>& actionObjects) {
            std::vector<std::unique_ptr<Action>> actions;
            for (const auto& obj : actionObjects) {
                actions.push_back(py::cast<std::unique_ptr<Action>>(obj)));
            }
            return actions;
        }, py::arg("actionObjects"), "Create a batch of actions from Python objects")
        
        // Parameter manipulation helpers
        .def("addParameter", [](Action& self, float value) {
            self.parameters_.push_back(value);
        }, py::arg("value"), "Add a parameter to the action")
        .def("setParameter", [](Action& self, size_t index, float value) {
            if (index < self.parameters_.size()) {
                self.parameters_[index] = value;
            }
        }, py::arg("index"), py::arg("value"), "Set a parameter at a specific index")
        .def("getParameterOr", [](const Action& self, size_t index, float defaultValue) {
            if (index < self.parameters_.size()) {
                return self.parameters_[index];
            }
            return defaultValue;
        }, py::arg("index"), py::arg("defaultValue"), "Get parameter at index or return default")
        
        .def("__repr__", [](const Action& self) {
            std::stringstream ss;
            ss << "<Action type=" << static_cast<int>(self.type_) << " params=" << floatVectorToString(self.parameters_) << ">";
            return ss.str();
        })
        .def("__str__", [](const Action& self) {
            return self.getTypeName();
        });

    // ===== ACTION BATCH CLASS =====
    py::class_<ActionBatch>(m, "ActionBatch", R"pbdoc(A collection of actions for batch processing)")
        .def(py::init<>())
        .def("addAction", [](ActionBatch& self, Action action) {
            self.actions_.push_back(std::make_unique<Action>(std::move(action)));
        }, py::arg("action"), "Add an action to the batch")
        .def("addActions", [](ActionBatch& self, const std::vector<Action>& actions) {
            for (const auto& action : actions) {
                self.actions_.push_back(std::make_unique<Action>(action));
            }
        }, py::arg("actions"), "Add multiple actions to the batch")
        .def("getActions", &ActionBatch::getActions,
             "Get all actions in the batch")
        .def("getActionCount", &ActionBatch::getActionCount,
             "Get the number of actions in the batch")
        .def("clear", &ActionBatch::clear,
             "Clear all actions from the batch")
        .def("getMovementActions", [](const ActionBatch& self) {
            std::vector<Action> movementActions;
            for (const auto& action : self.getActions()) {
                if (action.isMovement()) {
                    movementActions.push_back(action);
                }
            }
            return movementActions;
        }, "Get all movement actions in the batch")
        .def("getRotationActions", [](const ActionBatch& self) {
            std::vector<Action> rotationActions;
            for (const auto& action : self.getActions()) {
                if (action.isRotation()) {
                    rotationActions.push_back(action);
                }
            }
            return rotationActions;
        }, "Get all rotation actions in the batch")
        .def("filterByType", [](const ActionBatch& self, ActionType type) {
            std::vector<Action> filteredActions;
            for (const auto& action : self.getActions()) {
                if (action.getType() == type) {
                    filteredActions.push_back(action);
                }
            }
            return filteredActions;
        }, py::arg("type"), "Filter actions by type")
        .def("__repr__", [](const ActionBatch& self) {
            std::stringstream ss;
            ss << "<ActionBatch " << self.getActionCount() << " actions>";
            return ss.str();
        })
        .def("__len__", &ActionBatch::getActionCount);

    // ===== ENHANCED WORLD CLASS WITH CONTEXT MANAGER =====
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and vision settings")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, 
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("interact", &SimpleWorld::interact,
             "Interact with nearby objects")
        
        // Pythonic property-style access
        .def_readonly("width", &SimpleWorld::getWidth, "World width (Pythonic property)")
        .def_readonly("height", &SimpleWorld::getHeight, "World height (Pythonic property)")
        .def_readonly("agent", &SimpleWorld::getAgentBody, "Agent body (Pythonic property)")
        .def_readonly("sensory", &SimpleWorld::getSensoryPercept, "Sensory percept (Pythonic property)")
        
        // Enhanced configuration helpers
        .def("setupEnvironment", [](SimpleWorld& self, float width, float height, 
                                  size_t visionWidth, size_t visionHeight) {
            self.configure(width, height, visionWidth, visionHeight);
            self.setAgentStart(width/2, height/2);
        }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
           "Setup world with default agent position")
        .def("addResource", [](SimpleWorld& self, float x, float y, float radius, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("value"),
           "Add a resource object to the world")
        .def("addHazard", [](SimpleWorld& self, float x, float y, float radius, float damage) {
            WorldObject obj(x, y, WorldObjectType::Hazard, damage, radius);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("damage"),
           "Add a hazard object to the world")
        
        // Context manager support
        .def("__enter__", [](SimpleWorld& self) -> SimpleWorld& {
            return self;
        }, "Enter the world context")
        .def("__exit__", [](SimpleWorld& self, py::object, py::object, py::object) {
            self.reset();
        }, "Exit the world context (reset world)")
        
        .def("__repr__", [](const SimpleWorld& self) {
            std::stringstream ss;
            ss << "<SimpleWorld " << self.getWidth() << "x" << self.getHeight() 
               << " at t=" << self.getSimulationTime() << ">";
            return ss.str();
        })
        .def("__str__", [](const SimpleWorld& self) {
            return std::to_string(self.getWidth()) + "x" + std::to_string(self.getHeight());
        });

    // ===== ENHANCED BRAIN CLASS =====
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with Pythonic enhancements)")
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
        
        // Pythonic property-style access for frequently used brain states
        .def_readonly("total_neurons", &Brain::getTotalNeuronCount, "Total neuron count (Pythonic property)")
        .def_readonly("total_synapses", &Brain::getTotalSynapseCount, "Total synapse count (Pythonic property)")
        .def_readonly("active_neurons", &Brain::getActiveNeuronCount, "Active neuron count (Pythonic property)")
        .def_readonly("firing_neurons", &Brain::getFiringNeuronCount, "Firing neuron count (Pythonic property)")
        .def_readonly("total_spikes", &Brain::getTotalSpikeCount, "Total spike count (Pythonic property)")
        .def_readonly("average_firing_rate", &Brain::getAverageFiringRate, "Average firing rate (Pythonic property)")
        .def_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio, "E/I ratio (Pythonic property)")
        .def_readonly("region_count", &Brain::getRegionCount, "Region count (Pythonic property)")
        
        // Brain development and plasticity helpers
        .def("enableDevelopment", [](Brain& self) {
            self.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
        }, "Enable brain development")
        .def("disableDevelopment", [](Brain& self) {
            self.setDevelopmentalStage(DevelopmentalStage::Adult);
        }, "Disable brain development")
        .def("isDevelopmentEnabled", [](const Brain& self) {
            return self.getDevelopmentalStage() != DevelopmentalStage::Adult;
        }, "Check if development is enabled")
        .def("advanceDevelopment", [](Brain& self) {
            DevelopmentalStage current = self.getDevelopmentalStage();
            if (current == DevelopmentalStage::Initial) {
                self.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            } else if (current == DevelopmentalStage::CriticalPeriod) {
                self.setDevelopmentalStage(DevelopmentalStage::Maturation);
            }
        }, "Advance brain development stage")
        
        // Statistics and monitoring helpers
        .def("getStatistics", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["excitation_inhibition_ratio"] = self.getExcitationInhibitionRatio();
            stats["region_count"] = self.getRegionCount();
            return stats;
        }, "Get brain statistics as a dictionary")
        
        // Context manager for brain simulation
        .def("__enter__", [](Brain& self) -> Brain& {
            return self;
        }, "Enter the brain simulation context")
        .def("__exit__", [](Brain& self, py::object, py::object, py::object) {
            self.reset();
        }, "Exit the brain simulation context (reset brain)")
        
        .def("__repr__", [](const Brain& self) {
            std::stringstream ss;
            ss << "<Brain regions=" << self.getRegionCount() 
               << " neurons=" << self.getTotalNeuronCount() << ">";
            return ss.str();
        })
        .def("__str__", [](const Brain& self) {
            return "NLM Brain Simulation";
        });

    // ===== ENHANCED AGENT BRAIN CLASS =====
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(High-level interface for agent-world interaction)")
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
        
        // Pythonic property-style access for agent state
        .def_readonly("developmental_stage", &AgentBrain::getDevelopmentalStage, "Developmental stage (Pythonic property)")
        .def_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel, "Neuromodulation level (Pythonic property)")
        .def_readonly("curiosity_level", &AgentBrain::getCuriosityLevel, "Curiosity level (Pythonic property)")
        .def_readonly("novelty_level", &AgentBrain::getNoveltyLevel, "Novelty level (Pythonic property)")
        .def_readonly("prediction_error", &AgentBrain::getPredictionError, "Prediction error (Pythonic property)")
        
        // Agent control helpers
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        
        // Agent learning and adaptation helpers
        .def("applyLearningSignal", [](AgentBrain& self, float reward, float prediction) {
            self.applyRewardModulation(reward, prediction);
        }, py::arg("reward"), py::arg("prediction"),
           "Apply learning signal with reward and prediction")
        .def("updateWithExperience", [](AgentBrain& self, const SensoryPercept& percept, 
                                  float reward, double timestep) {
            self.processSensoryInput(percept);
            self.applyRewardModulation(reward, reward * 0.9);
            self.updateDevelopment(timestep);
        }, py::arg("percept"), py::arg("reward"), py::arg("timestep"),
           "Update agent with a complete experience (percept + reward)")
        
        .def("getStateDict", [](const AgentBrain& self) {
            py::dict state;
            state["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            state["neuromodulation_level"] = self.getNeuromodulationLevel();
            state["curiosity_level"] = self.getCuriosityLevel();
            state["novelty_level"] = self.getNoveltyLevel();
            state["prediction_error"] = self.getPredictionError();
            return state;
        }, "Get agent state as a dictionary")
        
        .def("__repr__", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "<AgentBrain stage=" << static_cast<int>(self.getDevelopmentalStage()) << ">";
            return ss.str();
        })
        .def("__str__", [](const AgentBrain& self) {
            return "Agent Brain Interface";
        });

    // ===== FACTORY FUNCTIONS =====
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration object")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world object")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")
    
    // Factory function for actions
    m.def("createAction", [](ActionType type) {
        return Action(type);
    }, py::arg("type"), "Create an action with the specified type")
    
    m.def("createActionWithParams", [](ActionType type, const std::vector<float>& params) {
        return Action(type, params);
    }, py::arg("type"), py::arg("parameters"), "Create an action with type and parameters")
    
    // Type conversion helpers
    m.def("createVisionFromArray", [](const py::array_t<float>& array) {
        py::buffer_info buf = array.request();
        float* ptr = static_cast<float*>(buf.ptr);
        size_t size = buf.size;
        std::vector<float> data(ptr, ptr + size);
        
        // Determine dimensions from array shape
        size_t width = 20, height = 20, channels = 3;
        if (buf.ndim == 1 && buf.shape[0] > 0) {
            // 1D array - assume grayscale
            width = static_cast<size_t>(std::sqrt(size));
            height = size / width;
            channels = 1;
        } else if (buf.ndim == 2) {
            // 2D array
            height = buf.shape[0];
            width = buf.shape[1];
            channels = 1;
        } else if (buf.ndim == 3) {
            // 3D array
            height = buf.shape[0];
            width = buf.shape[1];
            channels = buf.shape[2];
        }
        
        Vision vision(width, height, channels);
        vision.setData(data);
        return vision;
    }, py::arg("array"), "Create a Vision object from a NumPy array")
    
    m.def("createVisionFromDict", [](const py::dict& kwargs) {
        size_t width = kwargs.contains("width") ? 
            py::cast<size_t>(kwargs["width"]) : 20;
        size_t height = kwargs.contains("height") ? 
            py::cast<size_t>(kwargs["height"]) : 20;
        size_t channels = kwargs.contains("channels") ? 
            py::cast<size_t>(kwargs["channels"]) : 3;
        
        Vision vision(width, height, channels);
        
        if (kwargs.contains("data")) {
            std::vector<float> data = py::cast<std::vector<float>>(kwargs["data"]);
            vision.setData(data);
        }
        
        return vision;
    }, py::arg("kwargs"), "Create a Vision object from keyword arguments")
    
    // Batch creation helper
    m.def("createActionBatch", [](const std::vector<py::object>& actionObjects) {
        std::vector<std::unique_ptr<Action>> actions;
        for (const auto& obj : actionObjects) {
            actions.push_back(py::cast<std::unique_ptr<Action>>(obj));
        }
        return actions;
    }, py::arg("actionObjects"), "Create a batch of actions from Python objects")
    
    // Helper functions for common patterns
    m.def("createExplorationBatch", [](size_t numActions) {
        std::vector<std::unique_ptr<Action>> actions;
        for (size_t i = 0; i < numActions; ++i) {
            if (i % 3 == 0) {
                actions.push_back(std::make_unique<Action>(ActionType::MoveForward));
            } else if (i % 3 == 1) {
                actions.push_back(std::make_unique<Action>(ActionType::TurnLeft, {0.1}));
            } else {
                actions.push_back(std::make_unique<Action>(ActionType::TurnRight, {0.1}));
            }
        }
        return actions;
    }, py::arg("numActions"), "Create a batch of exploration actions")
    
    m.def("createNavigationBatch", [](float targetX, float targetY) {
        std::vector<std::unique_ptr<Action>> actions;
        // Simple navigation to target (placeholder logic)
        actions.push_back(std::make_unique<Action>(ActionType::MoveForward));
        actions.push_back(std::make_unique<Action>(ActionType::TurnLeft, {0.5}));
        actions.push_back(std::make_unique<Action>(ActionType::MoveForward));
        return actions;
    }, py::arg("targetX"), py::arg("targetY"), "Create a navigation action batch")
    
    // Type checking and conversion helpers
    m.def("checkNeuronId", [](const NeuronId& id) {
        return id.value < 1000000; // Sanity check
    }, py::arg("id"), "Check if neuron ID is valid (sanity check)")
    
    m.def("checkSynapseId", [](const SynapseId& id) {
        return id.value < 10000000; // Sanity check
    }, py::arg("id"), "Check if synapse ID is valid (sanity check)")
    
    m.def("checkRegionId", [](const RegionId& id) {
        return id.value < 100; // Sanity check
    }, py::arg("id"), "Check if region ID is valid (sanity check)")
    
    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Validation functions
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        auto keys = config.getKeys();
        for (const auto& key : keys) {
            auto value = config.get<double>(key);
            if (value && *value < 0) {
                errors.push_back("Negative value for key: " + key);
            }
        }
        return errors;
    }, py::arg("config"), "Validate configuration and return any errors")
    
    m.def("formatBrainState", [](const Brain& brain) {
        std::stringstream ss;
        ss << "Brain State:" << std::endl;
        ss << "  Neurons: " << brain.getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain.getTotalSynapseCount() << std::endl;
        ss << "  Active: " << brain.getActiveNeuronCount() << std::endl;
        ss << "  Firing: " << brain.getFiringNeuronCount() << std::endl;
        ss << "  Total Spikes: " << brain.getTotalSpikeCount() << std::endl;
        ss << "  Average Firing Rate: " << brain.getAverageFiringRate() << std::endl;
        ss << "  E/I Ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        ss << "  Regions: " << brain.getRegionCount();
        return ss.str();
    }, "Format brain state as a readable string")
    
    m.def("formatActionBatch", [](const ActionBatch& batch) {
        std::stringstream ss;
        ss << "Action Batch (" << batch.getActionCount() << " actions):" << std::endl;
        for (size_t i = 0; i < batch.getActionCount(); ++i) {
            ss << "  " << i << ": " << batch.getActions()[i].getTypeName();
            if (!batch.getActions()[i].getParameters().empty()) {
                ss << " (params: " << floatVectorToString(batch.getActions()[i].getParameters()) << ")";
            }
            ss << std::endl;
        }
        return ss.str();
    }, "Format action batch as a readable string");

} // namespace nlm
