// Enhanced Python bindings for NLM with better error handling and advanced features

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <cassert>
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

// Helper function for type conversions and validation
template<typename T>
T py_convert(const py::object& obj) {
    if (!obj.is_none()) {
        return obj.cast<T>();
    }
    throw py::value_error("Expected a value but got None");
}

// Custom exception classes for better error handling
class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMValueError : public std::runtime_error {
public:
    explicit NLMValueError(const std::string& msg) : std::runtime_error(msg) {}
};

class NLMConfigurationError : public std::runtime_error {
public:
    explicit NLMConfigurationError(const std::string& msg) : std::runtime_error(msg) {}
};

// Register custom exceptions
void registerCustomExceptions(py::module_& m) {
    py::register_exception<NLMRuntimeError>(m, "RuntimeError");
    py::register_exception<NLMValueError>(m, "ValueError");
    py::register_exception<NLMConfigurationError>(m, "ConfigurationError");
}

// Enhanced validation utilities
class ValidationHelper {
public:
    template<typename T>
    static T validateRange(const T& value, const T& min, const T& max, 
                          const std::string& name) {
        if (value < min || value > max) {
            throw NLMValueError("Value " + name + " must be between " + 
                               std::to_string(min) + " and " + std::to_string(max) +
                               ", got " + std::to_string(value));
        }
        return value;
    }
    
    static std::string validateFilePath(const std::string& path) {
        if (path.empty()) {
            throw NLMValueError("File path cannot be empty");
        }
        return path;
    }
    
    static size_t validateSize(size_t size, size_t max_size, 
                              const std::string& name) {
        if (size > max_size) {
            throw NLMValueError("Size of " + name + " cannot exceed " +
                               std::to_string(max_size) + ", got " +
                               std::to_string(size));
        }
        return size;
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Enhanced Version
        --------------------------------------------------------------
        A Python binding for the NLM C++ neural simulation framework with
        improved error handling, advanced configuration options, and
        comprehensive utility functions.
        
        Key Features:
        - Enhanced error handling with custom Python exceptions
        - Advanced configuration management
        - Validation utilities for robust parameter checking
        - Extended neural network analysis tools
        - Improved memory management and safety
        - Performance monitoring utilities
        - Advanced sensory and motor control interfaces
    )pbdoc";

    // Register custom exceptions
    registerCustomExceptions(m);

    // Configuration with enhanced validation
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(
        Enhanced configuration class for NLM system with validation and
        JSON/YAML support.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            try {
                std::string validated_path = ValidationHelper::validateFilePath(filepath);
                if (!self.loadFromFile(validated_path)) {
                    throw NLMConfigurationError("Failed to load configuration from: " + filepath);
                }
                return true;
            } catch (const NLMConfigurationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Error loading configuration: ") + e.what());
            }
        }, py::arg("filepath"),
             "Load configuration from a JSON/YAML file with validation")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                return self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Error loading from command line: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            try {
                std::string validated_path = ValidationHelper::validateFilePath(filepath);
                if (!self.saveToFile(validated_path)) {
                    throw NLMConfigurationError("Failed to save configuration to: " + filepath);
                }
                return true;
            } catch (const NLMConfigurationError& e) {
                throw;
            } catch (const std::exception& e) {
                throw NLMConfigurationError(std::string("Error saving configuration: ") + e.what());
            }
        }, py::arg("filepath"),
            "Save configuration to a file with validation")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("get", [](const Config& self, const std::string& key) {
            try {
                auto val = self.get<std::string>(key);
                if (val.has_value()) {
                    return py::make_tuple("string", val.value());
                }
                val = self.get<int>(key);
                if (val.has_value()) {
                    return py::make_tuple("int", val.value());
                }
                val = self.get<double>(key);
                if (val.has_value()) {
                    return py::make_tuple("double", val.value());
                }
                val = self.get<bool>(key);
                if (val.has_value()) {
                    return py::make_tuple("bool", val.value());
                }
                return py::make_tuple("unknown", "");
            } catch (const std::exception& e) {
                throw NLMValueError(std::string("Error getting config value: ") + e.what());
            }
        }, py::arg("key"),
            "Get a configuration value with type information")
        .def("getOr", [](const Config& self, const std::string& key, const py::object& default_value) {
            try {
                // Try different types
                auto val = self.get<std::string>(key);
                if (val.has_value()) return val.value();
                val = self.get<int>(key);
                if (val.has_value()) return val.value();
                val = self.get<double>(key);
                if (val.has_value()) return val.value();
                val = self.get<bool>(key);
                if (val.has_value()) return val.value();
                
                // Return default value preserving type
                return default_value.cast<std::string>();
            } catch (const std::exception& e) {
                throw NLMValueError(std::string("Error getting config value with default: ") + e.what());
            }
        }, py::arg("key"), py::arg("default_value"),
            "Get configuration value with default fallback")
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            try {
                if (py::isinstance<py::str>(value)) {
                    self.set(key, value.cast<std::string>(), ConfigSource::CommandLine);
                } else if (py::isinstance<py::int_>(value)) {
                    self.set(key, value.cast<int>(), ConfigSource::CommandLine);
                } else if (py::isinstance<py::float_>(value)) {
                    self.set(key, value.cast<double>(), ConfigSource::CommandLine);
                } else if (py::isinstance<py::bool_>(value)) {
                    self.set(key, value.cast<bool>(), ConfigSource::CommandLine);
                } else {
                    throw NLMValueError("Unsupported value type for configuration key: " + key);
                }
            } catch (const NLMValueError& e) {
                throw;
            } catch (const std::exception& e) {
                throw NLMValueError(std::string("Error setting configuration: ") + e.what());
            }
        }, py::arg("key"), py::arg("value"),
            "Set a configuration value with type validation")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a detailed summary of the configuration")
        .def("validate", [](const Config& self) {
            return self.validate();
        }, "Validate configuration and return (is_valid, message)")
        .def("getValidationReport", [](const Config& self) {
            return self.getValidationReport();
        }, "Get validation report with warnings and errors")
        .def("__repr__", [](const Config& cfg) {
            return "<EnhancedConfig: " + cfg.summary() + ">";
        });

    // Enhanced Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Enhanced vision sensory input with validation and utility functions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data, bool validate) {
            if (validate) {
                for (float val : data) {
                    if (std::isnan(val) || std::isinf(val)) {
                        throw NLMValueError("Vision data contains invalid values");
                    }
                }
            }
            self.setData(data);
        }, py::arg("data"), py::arg("validate") = true,
            "Set vision data with optional validation")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getNormalized", [](Vision& self) {
            auto data = self.getData();
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            
            if (max_val - min_val > 0.001f) {
                std::vector<float> normalized;
                normalized.reserve(data.size());
                for (float val : data) {
                    normalized.push_back((val - min_val) / (max_val - min_val));
                }
                return normalized;
            } else {
                return std::vector<float>(data.size(), 0.5f);
            }
        }, "Get normalized vision data (0-1 range)")
        .def("getStatistics", [](Vision& self) {
            auto data = self.getData();
            if (data.empty()) {
                return std::make_tuple(0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
            }
            
            float sum = std::accumulate(data.begin(), data.end(), 0.0f);
            float mean = sum / data.size();
            
            float sq_sum = 0.0f;
            for (float val : data) {
                sq_sum += (val - mean) * (val - mean);
            }
            float stddev = std::sqrt(sq_sum / data.size());
            
            float min_val = *std::min_element(data.begin(), data.end());
            float max_val = *std::max_element(data.begin(), data.end());
            
            return std::make_tuple(mean, stddev, min_val, max_val, sum);
        }, "Get vision data statistics")
        .def("isValid", [](Vision& self) {
            if (self.getData().empty()) {
                return std::make_pair(false, "Vision data is empty");
            }
            
            auto data = self.getData();
            for (size_t i = 0; i < data.size(); ++i) {
                if (std::isnan(data[i]) || std::isinf(data[i])) {
                    return std::make_pair(false, "Vision data contains invalid values at index " + std::to_string(i));
                }
            }
            
            return std::make_pair(true, "Vision data is valid");
        }, "Validate vision data and return (is_valid, message)");

    // Enhanced Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Enhanced audio sensory input with processing and analysis capabilities.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw NLMValueError("Audio data cannot be empty");
            }
            
            for (size_t i = 0; i < data.size(); ++i) {
                if (std::isnan(data[i]) || std::isinf(data[i])) {
                    throw NLMValueError("Audio data contains invalid values at index " + std::to_string(i));
                }
            }
            
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", [](Audio& self, size_t sampleRate) {
            ValidationHelper::validateRange(sampleRate, 8000, 192000, "sample rate");
            self.setSampleRate(sampleRate);
        }, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("normalize", [](Audio& self) {
            auto data = self.getData();
            if (data.empty()) return;
            
            float max_val = *std::max_element(data.begin(), data.end(),
                                            [](float a, float b) { return std::abs(a) < std::abs(b); });
            
            if (max_val > 1.0f) {
                std::vector<float> normalized;
                normalized.reserve(data.size());
                for (float val : data) {
                    normalized.push_back(val / max_val);
                }
                self.setData(normalized);
            }
        }, "Normalize audio data to range [-1, 1]");

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(
        Enhanced action representation with validation and utility functions.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", [](Action& self, const std::vector<float>& params) {
            for (float param : params) {
                if (std::isnan(param) || std::isinf(param)) {
                    throw NLMValueError("Action parameters contain invalid values");
                }
            }
            self.setParameters(params);
        }, py::arg("params"))
        .def("getName", &Action::getName)
        .def("getCategory", [](const Action& self) {
            std::string action_name = self.getName();
            if (action_name.find("Move") != std::string::npos) {
                return "Movement";
            } else if (action_name.find("Look") != std::string::npos) {
                return "Sensing";
            } else if (action_name.find("Interact") != std::string::npos) {
                return "Interaction";
            } else if (action_name.find("Eat") != std::string::npos || action_name.find("Drink") != std::string::npos) {
                return "Consumption";
            } else if (action_name.find("Rest") != std::string::npos || action_name.find("Wait") != std::string::npos) {
                return "Idle";
            } else {
                return "Custom";
            }
        }, "Get action category")
        .def("validateParameters", [](const Action& self) {
            auto params = self.getParameters();
            std::vector<std::string> errors;
            
            for (size_t i = 0; i < params.size(); ++i) {
                if (std::isnan(params[i]) || std::isinf(params[i])) {
                    errors.push_back("Invalid parameter value at index " + std::to_string(i));
                }
            }
            
            if (errors.empty()) {
                return std::make_pair(true, std::string("Action parameters are valid"));
            } else {
                std::string error_msg = "Action parameter validation failed:\n";
                for (const auto& error : errors) {
                    error_msg += "  - " + error + "\n";
                }
                return std::make_pair(false, error_msg);
            }
        }, "Validate action parameters")
        .def("clone", &Action::clone,
             "Create a copy of this action")
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["name"] = self.getName();
            result["category"] = self.getCategory();
            result["parameters"] = self.getParameters();
            return result;
        }, "Convert action to dictionary representation");

    // Enhanced AgentBody class
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Enhanced agent body state with validation and utility functions.
    )pbdoc")
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
        .def("validateState", [](AgentBody& self) {
            std::vector<std::string> errors;
            
            if (self.x < 0 || self.x > 100) {
                errors.push_back("X position out of bounds [0, 100]");
            }
            
            if (self.y < 0 || self.y > 100) {
                errors.push_back("Y position out of bounds [0, 100]");
            }
            
            if (self.energy < 0 || self.energy > 100) {
                errors.push_back("Energy must be between 0 and 100");
            }
            
            if (self.health <= 0) {
                errors.push_back("Health must be positive");
            }
            
            if (errors.empty()) {
                return std::make_pair(true, std::string("Agent body state is valid"));
            } else {
                std::string error_msg = "Agent body validation failed:\n";
                for (const auto& error : errors) {
                    error_msg += "  - " + error + "\n";
                }
                return std::make_pair(false, error_msg);
            }
        }, "Validate agent body state")
        .def("getPosition", [](AgentBody& self) {
            return py::make_tuple(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("getVelocity", [](AgentBody& self) {
            return py::make_tuple(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("getSpeed", [](AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get agent speed")
        .def("getHeading", [](AgentBody& self) {
            return std::atan2(self.velocityY, self.velocityX);
        }, "Get agent heading angle in radians");

    // Enhanced ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Enhanced action result with validation and analysis capabilities.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositive", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if result is positive")
        .def("isNegative", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if result is negative")
        .def("getConfidence", [](const ActionResult& self) {
            float confidence = std::abs(self.reward) / 10.0f;
            if (self.success) confidence *= 1.5f;
            return std::min(1.0f, confidence);
        }, "Get result confidence")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            result["is_positive"] = self.isPositive();
            result["confidence"] = self.getConfidence();
            return result;
        }, "Convert action result to dictionary");

    // Enhanced SensoryPercept class
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Enhanced sensory percept with validation and processing capabilities.
    )pbdoc")
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
        .def("validate", [](SensoryPercept& self) {
            std::vector<std::string> errors;
            
            auto vision = self.getVision();
            for (size_t i = 0; i < vision.size(); ++i) {
                if (std::isnan(vision[i]) || std::isinf(vision[i])) {
                    errors.push_back("Vision data contains invalid values at index " + std::to_string(i));
                }
            }
            
            auto touch = self.getTouch();
            for (size_t i = 0; i < touch.size(); ++i) {
                if (std::isnan(touch[i]) || std::isinf(touch[i])) {
                    errors.push_back("Touch data contains invalid values at index " + std::to_string(i));
                }
            }
            
            auto internal = self.getInternal();
            for (size_t i = 0; i < internal.size(); ++i) {
                if (std::isnan(internal[i]) || std::isinf(internal[i])) {
                    errors.push_back("Internal data contains invalid values at index " + std::to_string(i));
                }
            }
            
            auto proprioception = self.getProprioception();
            for (size_t i = 0; i < proprioception.size(); ++i) {
                if (std::isnan(proprioception[i]) || std::isinf(proprioception[i])) {
                    errors.push_back("Proprioception data contains invalid values at index " + std::to_string(i));
                }
            }
            
            auto audio = self.getAudio();
            for (size_t i = 0; i < audio.size(); ++i) {
                if (std::isnan(audio[i]) || std::isinf(audio[i])) {
                    errors.push_back("Audio data contains invalid values at index " + std::to_string(i));
                }
            }
            
            if (errors.empty()) {
                return std::make_pair(true, std::string("Sensory percept is valid"));
            } else {
                std::string error_msg = "Sensory percept validation failed:\n";
                for (const auto& error : errors) {
                    error_msg += "  - " + error + "\n";
                }
                return std::make_pair(false, error_msg);
            }
        }, "Validate sensory percept")
        .def("getSignalStats", [](SensoryPercept& self) {
            py::dict result;
            
            auto vision = self.getVision();
            if (!vision.empty()) {
                float sum = std::accumulate(vision.begin(), vision.end(), 0.0f);
                float mean = sum / vision.size();
                float sq_sum = 0.0f;
                for (float val : vision) {
                    sq_sum += (val - mean) * (val - mean);
                }
                float stddev = std::sqrt(sq_sum / vision.size());
                result["vision_mean"] = mean;
                result["vision_stddev"] = stddev;
                result["vision_min"] = *std::min_element(vision.begin(), vision.end());
                result["vision_max"] = *std::max_element(vision.begin(), vision.end());
            }
            
            return result;
        }, "Get statistics for all sensory signals");

    // Enhanced SimpleWorld class
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Enhanced simple world with advanced features and validation.
    )pbdoc")
        .def(py::init<>())
        .def("configure", [](SimpleWorld& self, size_t width, size_t height,
                          size_t visionWidth, size_t visionHeight,
                          float maxEnergy, float energyDecayRate) {
            ValidationHelper::validateRange(width, 16, 1024, "world width");
            ValidationHelper::validateRange(height, 16, 1024, "world height");
            ValidationHelper::validateRange(visionWidth, 8, 256, "vision width");
            ValidationHelper::validateRange(visionHeight, 8, 256, "vision height");
            ValidationHelper::validateRange(maxEnergy, 10.0f, 1000.0f, "max energy");
            ValidationHelper::validateRange(energyDecayRate, 0.1f, 0.99f, "energy decay rate");
            
            self.configure(width, height, visionWidth, visionHeight);
            self.setMaxEnergy(maxEnergy);
            self.setEnergyDecayRate(energyDecayRate);
        }, py::arg("width"), py::arg("height"),
            py::arg("visionWidth"), py::arg("visionHeight"),
            py::arg("maxEnergy") = 100.0f, py::arg("energyDecayRate") = 0.95f,
            "Configure world with enhanced validation")
        .def("validateState", [](SimpleWorld& self) {
            std::vector<std::string> errors;
            
            if (self.objects.size() > 1000) {
                errors.push_back("Too many objects in world (limit: 1000)");
            }
            
            for (const auto& obj : self.objects) {
                if (obj.radius <= 0.0f) {
                    errors.push_back("Object radius must be positive");
                    break;
                }
            }
            
            if (self.agentBody.energy < 0 || self.agentBody.energy > self.maxEnergy) {
                errors.push_back("Agent energy out of valid range");
            }
            
            if (errors.empty()) {
                return std::make_pair(true, std::string("World state is valid"));
            } else {
                std::string error_msg = "World validation failed:\n";
                for (const auto& error : errors) {
                    error_msg += "  - " + error + "\n";
                }
                return std::make_pair(false, error_msg);
            }
        }, "Validate world state");

    // Enhanced Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Enhanced brain class with advanced analysis and monitoring capabilities.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self, bool validate_config = true) {
            if (validate_config) {
                auto config = self.getConfig();
                if (config) {
                    auto validation = config->validate();
                    if (!validation.first) {
                        throw NLMConfigurationError("Invalid configuration: " + validation.second);
                    }
                }
            }
            
            bool success = self.initialize();
            if (!success) {
                throw NLMRuntimeError("Failed to initialize brain");
            }
            return success;
        }, py::arg("validate_config") = true,
            "Initialize brain with optional configuration validation")
        .def("analyzeNetworkTopology", [](Brain& self) {
            py::dict result;
            
            int total_neurons = self.getTotalNeuronCount();
            int total_synapses = self.getTotalSynapseCount();
            int active_neurons = self.getActiveNeuronCount();
            int firing_neurons = self.getFiringNeuronCount();
            
            result["total_neurons"] = total_neurons;
            result["total_synapses"] = total_synapses;
            result["active_neurons"] = active_neurons;
            result["firing_neurons"] = firing_neurons;
            
            if (total_neurons > 0) {
                result["activity_ratio"] = static_cast<float>(active_neurons) / total_neurons;
                result["firing_ratio"] = static_cast<float>(firing_neurons) / total_neurons;
                result["synapse_density"] = static_cast<float>(total_synapses) / total_neurons;
            } else {
                result["activity_ratio"] = 0.0f;
                result["firing_ratio"] = 0.0f;
                result["synapse_density"] = 0.0f;
            }
            
            return result;
        }, "Analyze network topology")
        .def("getPerformanceReport", [](Brain& self) {
            py::dict report;
            
            auto metrics = self.getPerformanceMetrics();
            report["basic_metrics"] = metrics;
            
            int total_neurons = self.getTotalNeuronCount();
            int total_synapses = self.getTotalSynapseCount();
            float avg_firing_rate = self.getAverageFiringRate();
            
            report["derived_metrics"] = py::dict(
                "synapse_per_neuron", total_neurons > 0 ? static_cast<float>(total_synapses) / total_neurons : 0.0f,
                "firing_rate_normalized", std::min(avg_firing_rate / 100.0f, 1.0f),
                "network_efficiency", (avg_firing_rate > 0) ? static_cast<float>(total_synapses) / avg_firing_rate : 0.0f
            );
            
            return report;
        }, "Get comprehensive performance report");

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Enhanced agent brain interface with advanced neuromodulation and learning.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("getPerformanceMetrics", [](AgentBrain& self) {
            py::dict metrics;
            
            metrics["sensory_input_size"] = self.getSensoryInputSize();
            metrics["motor_output_size"] = self.getMotorOutputSize();
            metrics["neuromodulation_level"] = self.getNeuromodulationLevel();
            metrics["curiosity_level"] = self.getCuriosityLevel();
            metrics["novelty_level"] = self.getNoveltyLevel();
            metrics["prediction_error"] = self.getPredictionError();
            metrics["expected_reward"] = self.getExpectedReward();
            metrics["total_reward"] = self.getTotalReward();
            metrics["development_stage"] = self.getDevelopmentalStage();
            
            return metrics;
        }, "Get comprehensive agent performance metrics");

    // Convenience factory functions with enhanced error handling
    m.def("createDefaultConfig", []() {
        try {
            return std::make_shared<Config>();
        } catch (const std::exception& e) {
            throw NLMRuntimeError(std::string("Failed to create default config: ") + e.what());
        }
    }, "Create a default configuration with error handling");

    m.def("createBrain", [](std::shared_ptr<Config> config) {
        try {
            if (!config) {
                throw NLMConfigurationError("Configuration cannot be null");
            }
            return std::make_shared<Brain>(config);
        } catch (const NLMConfigurationError& e) {
            throw;
        } catch (const std::exception& e) {
            throw NLMRuntimeError(std::string("Failed to create brain: ") + e.what());
        }
    }, py::arg("config"), "Create a new brain with configuration validation");

    m.def("createSimpleWorld", []() {
        try {
            return std::make_shared<SimpleWorld>();
        } catch (const std::exception& e) {
            throw NLMRuntimeError(std::string("Failed to create simple world: ") + e.what());
        }
    }, "Create a new simple world with error handling");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) {
        try {
            if (!brain) {
                throw NLMConfigurationError("Brain cannot be null");
            }
            return std::make_shared<AgentBrain>(brain);
        } catch (const NLMConfigurationError& e) {
            throw;
        } catch (const std::exception& e) {
            throw NLMRuntimeError(std::string("Failed to create agent brain: ") + e.what());
        }
    }, py::arg("brain"), "Create a new agent brain interface with validation");

    // Enhanced utility functions
    m.def("validateConfiguration", [](const Config& config) {
        return config.validate();
    }, "Validate configuration and return (is_valid, message)");

    m.def("validateBrain", [](Brain& brain) {
        return brain.validateState();
    }, "Validate brain state and return (is_valid, message)");

    m.def("validateAgentBrain", [](AgentBrain& agent) {
        return agent.validateState();
    }, "Validate agent brain state and return (is_valid, message)");

    m.def("validateWorld", [](SimpleWorld& world) {
        return world.validateState();
    }, "Validate world state and return (is_valid, message)");

    m.def("getSystemStatus", []() {
        py::dict status;
        
        status["version"] = "1.0.0-enhanced";
        status["python_bindings"] = true;
        status["error_handling"] = "enhanced";
        status["validation"] = "comprehensive";
        status["advanced_features"] = true;
        
        return status;
    }, "Get enhanced system status information");

    // Constants and invalid identifiers
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module version and information
    m.attr("__version__") = "1.0.0-enhanced";
    m.attr("__author__") = "NLM Development Team";
    m.attr("__description__") = "Enhanced NLM Python bindings with advanced error handling and validation";

    // Convenience functions
    m.def("quick_test", []() {
        try {
            auto config = std::make_shared<Config>();
            config->set("neuron_count", 100);
            config->set("region_count", 5);
            
            auto brain = std::make_shared<Brain>(config);
            brain->initialize();
            
            auto world = std::make_shared<SimpleWorld>();
            world->configure(32, 32, 16, 16);
            
            auto agent = std::make_shared<AgentBrain>(brain);
            agent->initialize(*world);
            
            // Run quick simulation
            for (int step = 0; step < 100; ++step) {
                auto percept = world->observe(brain.get());
                agent->processSensoryInput(percept);
                brain->step(step, step * 0.001);
                auto motor_cmd = agent->decodeMotorCommand();
                world->applyMotorCommand(*agent, motor_cmd);
            }
            
            return std::make_tuple(true, std::to_string(agent->getTotalReward()), brain->getAverageFiringRate());
        } catch (const std::exception& e) {
            return std::make_tuple(false, std::string("Error: ") + e.what(), 0.0f);
        }
    }, "Quick test of enhanced functionality");

} // namespace nlm