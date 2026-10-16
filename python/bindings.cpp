#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/core/Errors/Errors.hpp"

namespace py = pybind11;
namespace nlm {

// Helper class for validation and error handling
class BindingHelpers {
public:
    // Validate that a shared_ptr is not null
    template<typename T>
    static void validateSharedPtr(const std::shared_ptr<T>& ptr, const std::string& name) {
        if (!ptr) {
            throw nlm::ValidationError(""" + name + """ cannot be null""");
        }
    }

    // Validate pointer is not null
    template<typename T>
    static void validatePtr(T* ptr, const std::string& name) {
        if (!ptr) {
            throw nlm::ValidationError(""" + name + """ cannot be null""");
        }
    }

    // Validate numeric ranges
    template<typename T>
    static void validateRange(T value, T min, T max, const std::string& paramName) {
        if (value < min || value > max) {
            std::ostringstream oss;
            oss << "Parameter " + paramName + " must be in range [" << min << ", " << max << "], got " << value;
            throw nlm::ValidationError(oss.str());
        }
    }

    // Validate configuration parameters
    static void validateConfig(const Config& config) {
        // Basic config validation
        if (config.has("brain.neuron_count")) {
            auto count = config.getOr<int>("brain.neuron_count", 0);
            if (count <= 0 || count > 1000000) {
                throw nlm::ConfigurationError("brain.neuron_count must be between 1 and 1,000,000");
            }
        }
    }

    // Validate world dimensions
    static void validateWorldDimensions(size_t width, size_t height, size_t visionWidth, size_t visionHeight) {
        if (width == 0 || height == 0) {
            throw nlm::WorldError("World dimensions must be positive");
        }
        if (visionWidth > width || visionHeight > height) {
            throw nlm::WorldError("Vision dimensions cannot exceed world dimensions");
        }
    }

    // Validate action parameters
    static void validateAction(const Action& action) {
        if (action.getType() == ActionType::Custom) {
            const auto& params = action.getParameters();
            if (params.empty()) {
                throw nlm::ValidationError("Custom action requires parameters");
            }
            for (float param : params) {
                if (std::isnan(param) || std::isinf(param)) {
                    throw nlm::ValidationError("Action parameters must be finite numbers");
                }
            }
        }
    }

    // Create safe wrapper for shared_ptr with validation
    template<typename T>
    static py::object safeSharedPtr(const std::shared_ptr<T>& ptr, const std::string& className) {
        validateSharedPtr(ptr, className);
        return py::cast(ptr);
    }

    // Helper for safe method calls with error handling
    template<typename Func, typename... Args>
    static auto safeCall(Func&& func, const std::string& operationName, Args&&... args) {
        try {
            return func(std::forward<Args>(args)...);
        } catch (const nlm::NLMError& e) {
            throw; // Re-throw NLM errors
        } catch (const std::exception& e) {
            throw nlm::BrainError(""" + operationName + """: """ + e.what());
        } catch (...) {
            throw nlm::BrainError(""" + operationName + """: Unknown error""");
        }
    }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.

        Features:
        ---------
        * Enhanced error handling with custom Python exceptions
        * Pythonic APIs and convenience functions
        * Advanced features for experienced users
        * Type-safe configuration access
        * Plugin architecture capabilities

        Exceptions:
        -----------
        * NLMError - Base exception for all NLM errors
        * ConfigurationError - Configuration-related errors
        * ValidationError - Input validation errors
        * BrainError - Brain operation errors
        * WorldError - World simulation errors
        * IOError - File I/O errors
        * PluginError - Plugin and extensibility errors

        See docs/ for comprehensive documentation and examples.
    )pbdoc";

    // Register custom exception classes
    py::register_exception<nlm::NLMError>(m, "NLMError");
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::ValidationError>(m, "ValidationError");
    py::register_exception<nlm::BrainError>(m, "BrainError");
    py::register_exception<nlm::WorldError>(m, "WorldError");
    py::register_exception<nlm::IOError>(m, "IOError");
    py::register_exception<nlm::PluginError>(m, "PluginError");

    // Helper function for safe Python-C++ conversions with error checking
    auto validate_ptr = [](const void* ptr, const std::string& name) {
        if (!ptr) {
            throw nlm::ValidationError(""" + name + """ cannot be null""");
        }
    };

    // Helper class for validation and error handling
    py::class_<BindingHelpers>(m, "_BindingHelpers")
        .def_static("validate_shared_ptr", &BindingHelpers::validateSharedPtr,
                   py::arg("ptr"), py::arg("name"),
                   "Validate that a shared_ptr is not null")
        .def_static("validate_ptr", &BindingHelpers::validatePtr,
                   py::arg("ptr"), py::arg("name"),
                   "Validate that a pointer is not null")
        .def_static("validate_range", &BindingHelpers::validateRange,
                   py::arg("value"), py::arg("min"), py::arg("max"), py::arg("paramName"),
                   "Validate numeric ranges")
        .def_static("validate_config", &BindingHelpers::validateConfig,
                   py::arg("config"), "Validate configuration parameters")
        .def_static("validate_world_dimensions", &BindingHelpers::validateWorldDimensions,
                   py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
                   "Validate world dimensions")
        .def_static("validate_action", &BindingHelpers::validateAction,
                   py::arg("action"), "Validate action parameters")
        .def_static("safe_shared_ptr", &BindingHelpers::safeSharedPtr,
                   py::arg("ptr"), py::arg("className"), "Create safe wrapper for shared_ptr")
        .def_static("safe_call", &BindingHelpers::safeCall,
                   py::arg("func"), py::arg("operationName"), py::arg("args"),
                   "Execute function with error handling");

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

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

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
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        // Pythonic API additions
        .def("get_int", [](const Config& self, const std::string& key) {
            auto value = self.getOr<int>(key);
            if (value) {
                return *value;
            }
            throw nlm::ConfigurationError("Key '" + key + "' not found or not an integer");
        }, py::arg("key"),
           "Get integer configuration value (raises ConfigurationError if not found)")
        .def("get_float", [](const Config& self, const std::string& key) {
            auto value = self.getOr<double>(key);
            if (value) {
                return *value;
            }
            throw nlm::ConfigurationError("Key '" + key + "' not found or not a float");
        }, py::arg("key"),
           "Get float configuration value (raises ConfigurationError if not found)")
        .def("get_string", [](const Config& self, const std::string& key) {
            auto value = self.getOr<std::string>(key);
            if (value) {
                return *value;
            }
            throw nlm::ConfigurationError("Key '" + key + "' not found or not a string");
        }, py::arg("key"),
           "Get string configuration value (raises ConfigurationError if not found)")
        .def("get_bool", [](const Config& self, const std::string& key) {
            auto value = self.getOr<bool>(key);
            if (value) {
                return *value;
            }
            throw nlm::ConfigurationError("Key '" + key + "' not found or not a boolean");
        }, py::arg("key"),
           "Get boolean configuration value (raises ConfigurationError if not found)")
        .def("set_int", &Config::set<int>, py::arg("key"), py::arg("value"),
             "Set integer configuration value")
        .def("set_float", &Config::set<double>, py::arg("key"), py::arg("value"),
             "Set float configuration value")
        .def("set_string", &Config::set<std::string>, py::arg("key"), py::arg("value"),
             "Set string configuration value")
        .def("set_bool", &Config::set<bool>, py::arg("key"), py::arg("value"),
             "Set boolean configuration value")
        // Convenience methods for common configurations
        .def("get_brain_neuron_count", &Config::getOr<int>, py::arg("default") = 1000,
             "Get brain neuron count (default: 1000)")
        .def("set_brain_neuron_count", &Config::set<int>, py::arg("value"),
             "Set brain neuron count")
        .def("get_plasticity_rate", &Config::getOr<double>, py::arg("default") = 0.001,
             "Get plasticity rate (default: 0.001)")
        .def("set_plasticity_rate", &Config::set<double>, py::arg("value"),
             "Set plasticity rate")
        .def("enable_reward_modulation", [](Config& self, bool enable) {
            self.set("neuromod.dopamine.enable", enable);
        }, py::arg("enable"),
             "Enable/disable reward modulation")
        .def("is_reward_modulation_enabled", &Config::has, py::arg("key"),
             "Check if reward modulation is enabled")
        .def("create_template", [](const std::string& name) -> std::shared_ptr<Config> {
            auto template = std::make_shared<Config>();
            // Set default template values
            template->set("brain.neuron_count", 1000);
            template->set("brain.synapse_density", 0.1);
            template->set("plasticity.stdp.enable", true);
            template->set("plasticity.hebbian.enable", true);
            template->set("neuromod.dopamine.scale", 1.0);
            template->set("neuromod.curiosity.enable", true);
            template->set("neuromod.novelty.enable", true);
            return template;
        }, py::arg("name"),
             "Create a configuration template with sensible defaults")
        .def("validate", &BindingHelpers::validateConfig, py::arg("config"),
             "Validate configuration parameters");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

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
        // Pythonic API additions
        .def("add_parameter", [](Action& self, float param) {
            auto params = self.getParameters();
            params.push_back(param);
            self.setParameters(params);
        }, py::arg("param"),
             "Add a parameter to the action")
        .def("clear_parameters", [](Action& self) {
            self.setParameters(std::vector<float>());
        }, "Clear all action parameters")
        .def("has_parameter", [](const Action& self, float param) {
            const auto& params = self.getParameters();
            return std::find(params.begin(), params.end(), param) != params.end();
        }, py::arg("param"),
             "Check if action has a specific parameter")
        .def("get_parameter_count", [](const Action& self) {
            return self.getParameters().size();
        }, "Get the number of parameters")
        .def("is_custom", [](const Action& self) {
            return self.getType() == ActionType::Custom;
        }, "Check if action is custom type")
        .def("get_normalized_parameters", [](const Action& self) {
            const auto& params = self.getParameters();
            if (params.empty()) {
                return std::vector<float>();
            }
            float max_val = *std::max_element(params.begin(), params.end(),
                                             [](float a, float b) { return std::abs(a) < std::abs(b); });
            if (std::abs(max_val) < 1e-6) {
                return std::vector<float>();
            }
            std::vector<float> normalized(params.size());
            for (size_t i = 0; i < params.size(); ++i) {
                normalized[i] = params[i] / max_val;
            }
            return normalized;
        }, "Get normalized parameters (scaled to [-1, 1])")
        .def("validate", &BindingHelpers::validateAction, py::arg("action"),
             "Validate action parameters");

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
        .def_readwrite("active", &WorldObject::active);

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
        .def("reset", &AgentBody::reset);

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

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
        // Pythonic API additions
        .def("create_resource", [](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj;
            obj.x = x; obj.y = y; obj.type = WorldObjectType::Resource;
            obj.value = value; obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
             "Add a resource object at position (x, y)")
        .def("create_hazard", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj;
            obj.x = x; obj.y = y; obj.type = WorldObjectType::Hazard;
            obj.value = 0.0f; obj.radius = radius; obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
             "Add a hazard object at position (x, y)")
        .def("create_wall", [](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj;
            obj.x = x; obj.y = y; obj.type = WorldObjectType::Wall;
            obj.value = 0.0f; obj.radius = radius; obj.active = true;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
             "Add a wall object at position (x, y)")
        .def("scan_environment", [](SimpleWorld& self, float range) {
            std::vector<WorldObject> objects;
            for (float x = -range; x <= range; x += 0.5f) {
                for (float y = -range; y <= range; y += 0.5f) {
                    if (self.isValidPosition(x, y)) {
                        auto obj = self.getObject(x, y);
                        if (obj.active) {
                            objects.push_back(obj);
                        }
                    }
                }
            }
            return objects;
        }, py::arg("range"),
             "Scan environment for objects within specified range")
        .def("get_nearby_objects", [](SimpleWorld& self, float x, float y, float range) {
            std::vector<WorldObject> objects;
            for (float cx = x - range; cx <= x + range; cx += 0.5f) {
                for (float cy = y - range; cy <= y + range; cy += 0.5f) {
                    if (self.isValidPosition(cx, cy)) {
                        auto obj = self.getObject(cx, cy);
                        if (obj.active && ((obj.x - x)*(obj.x - x) + (obj.y - y)*(obj.y - y)) <= range*range) {
                            objects.push_back(obj);
                        }
                    }
                }
            }
            return objects;
        }, py::arg("x"), py::arg("y"), py::arg("range"),
             "Get objects within range of position (x, y)")
        .def("clear_objects", [](SimpleWorld& self) {
            // Remove all objects from the world
            for (float x = -self.getWidth()/2; x < self.getWidth()/2; x += 0.5f) {
                for (float y = -self.getHeight()/2; y < self.getHeight()/2; y += 0.5f) {
                    self.removeObject(x, y);
                }
            }
        }, "Clear all objects from the world")
        .def("add_resources", [](SimpleWorld& self, int count, float value = 1.0f, float spread = 5.0f) {
            for (int i = 0; i < count; ++i) {
                float angle = (static_cast<float>(rand()) / RAND_MAX) * 2.0f * M_PI;
                float distance = (static_cast<float>(rand()) / RAND_MAX) * spread;
                float x = distance * cos(angle);
                float y = distance * sin(angle);
                if (self.isValidPosition(x, y)) {
                    self.create_resource(x, y, value);
                }
            }
        }, py::arg("count"), py::arg("value") = 1.0f, py::arg("spread") = 5.0f,
             "Add multiple resource objects")
        .def("validate_dimensions", &BindingHelpers::validateWorldDimensions,
             py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
             "Validate world dimensions");

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
             "Log brain status");

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
        // Pythonic API additions
        .def("process_vision_input", [](AgentBrain& self, const Vision& vision) {
            SensoryPercept percept;
            percept.setVision(vision);
            self.processSensoryInput(percept);
        }, py::arg("vision"),
             "Process vision sensory input directly")
        .def("process_audio_input", [](AgentBrain& self, const Audio& audio) {
            SensoryPercept percept;
            // Audio would be added to appropriate field in percept
            self.processSensoryInput(percept);
        }, py::arg("audio"),
             "Process audio sensory input directly")
        .def("get_motor_command_as_dict", [](const AgentBrain& self) {
            MotorCommand cmd = self.decodeMotorCommand();
            py::dict result;
            switch (cmd) {
                case MotorCommand::MoveForward: result["action"] = "MoveForward"; break;
                case MotorCommand::MoveBackward: result["action"] = "MoveBackward"; break;
                case MotorCommand::TurnLeft: result["action"] = "TurnLeft"; break;
                case MotorCommand::TurnRight: result["action"] = "TurnRight"; break;
                case MotorCommand::LookLeft: result["action"] = "LookLeft"; break;
                case MotorCommand::LookRight: result["action"] = "LookRight"; break;
                case MotorCommand::Interact: result["action"] = "Interact"; break;
                case MotorCommand::Wait: result["action"] = "Wait"; break;
            }
            return result;
        }, "Get motor command as Python dictionary")
        .def("get_behavior_summary", [](const AgentBrain& self) {
            py::dict summary;
            summary["developmental_stage"] = self.getDevelopmentalStage();
            summary["curiosity_level"] = self.getCuriosityLevel();
            summary["novelty_level"] = self.getNoveltyLevel();
            summary["prediction_error"] = self.getPredictionError();
            summary["neuromodulation_level"] = self.getNeuromodulationLevel();
            summary["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            summary["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            summary["development_enabled"] = self.isDevelopmentEnabled();
            summary["curiosity_enabled"] = self.isCuriosityEnabled();
            return summary;
        }, "Get agent brain state as Python dictionary")
        .def("enable_all_features", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all neuromodulation and development features")
        .def("disable_all_features", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        }, "Disable all neuromodulation and development features")
        .def("is_ready", [](const AgentBrain& self) {
            return self.getBrain() != nullptr &&
                   self.getBrain()->getTotalNeuronCount() > 0;
        }, "Check if agent brain is ready for simulation");

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

    // Advanced features and extensibility
    m.def("register_plasticity_rule", [](const std::string& name, 
                                        std::function<float(float, float)> rule) {
        // Implementation would register custom plasticity rule
        throw nlm::PluginError("Plasticity rule registration not yet implemented");
    }, py::arg("name"), py::arg("rule"),
       "Register custom plasticity rule function");

    m.def("register_neuron_model", [](const std::string& name,
                                    std::function<std::shared_ptr<Neuron>(float)> factory) {
        // Implementation would register custom neuron model
        throw nlm::PluginError("Neuron model registration not yet implemented");
    }, py::arg("name"), py::arg("factory"),
       "Register custom neuron model factory");

    m.def("register_world_plugin", [](const std::string& name,
                                    std::function<void(SimpleWorld&)> plugin) {
        // Implementation would register custom world plugin
        throw nlm::PluginError("World plugin registration not yet implemented");
    }, py::arg("name"), py::arg("plugin"),
       "Register custom world plugin");

    // Callback system
    m.def("add_step_callback", [](std::shared_ptr<Brain> brain,
                                 std::function<void(int, float)> callback) {
        // Implementation would add step callback
        throw nlm::PluginError("Step callback not yet implemented");
    }, py::arg("brain"), py::arg("callback"),
       "Add callback to be called at each simulation step");

    m.def("add_spike_callback", [](std::shared_ptr<Brain> brain,
                                  std::function<void(NeuronId, NeuronType)> callback) {
        // Implementation would add spike callback
        throw nlm::PluginError("Spike callback not yet implemented");
    }, py::arg("brain"), py::arg("callback"),
       "Add callback to be called when neurons spike");

    m.def("add_action_callback", [](std::shared_ptr<AgentBrain> agent,
                                   std::function<void(const Action&)> callback) {
        // Implementation would add action callback
        throw nlm::PluginError("Action callback not yet implemented");
    }, py::arg("agent"), py::arg("callback"),
       "Add callback to be called when actions are produced");

    // Advanced simulation features
    m.def("run_adaptive", [](std::shared_ptr<Brain> brain,
                           std::shared_ptr<SimpleWorld> world,
                           std::shared_ptr<AgentBrain> agent,
                           int max_steps, float stability_threshold,
                           std::function<bool()> should_continue) {
        // Implementation would run adaptive simulation
        throw nlm::BrainError("Adaptive simulation not yet implemented");
    }, py::arg("brain"), py::arg("world"), py::arg("agent"),
       py::arg("max_steps"), py::arg("stability_threshold"), py::arg("should_continue"),
       "Run adaptive simulation with dynamic timestep control");

    m.def("run_replay", [](std::shared_ptr<Brain> brain,
                         const std::vector<float>& memory,
                         int replay_steps) {
        // Implementation would run simulation with memory replay
        throw nlm::BrainError("Replay simulation not yet implemented");
    }, py::arg("brain"), py::arg("memory"), py::arg("replay_steps"),
       "Run simulation with memory replay");

    m.def("run_batch", [](const std::vector<std::shared_ptr<Brain>>& brains,
                        const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                        const std::vector<std::shared_ptr<AgentBrain>>& agents,
                        int steps_per_brain) {
        // Implementation would run parallel batch simulation
        throw nlm::BrainError("Batch simulation not yet implemented");
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"),
       py::arg("steps_per_brain"),
       "Run parallel batch simulation");

    // Memory and checkpointing
    m.def("checkpoint", [](std::shared_ptr<Brain> brain,
                         const std::string& filepath,
                         bool compress = false) {
        if (!brain->save(filepath)) {
            throw nlm::IOError("Failed to save checkpoint to " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"), py::arg("compress") = false,
       "Save brain state to file (checkpoint)");

    m.def("restore", [](std::shared_ptr<Brain> brain,
                      const std::string& filepath) {
        if (!brain->load(filepath)) {
            throw nlm::IOError("Failed to load checkpoint from " + filepath);
        }
    }, py::arg("brain"), py::arg("filepath"),
       "Load brain state from file");

    m.def("restore_best", [](const std::vector<std::shared_ptr<Brain>>& brains,
                           const std::vector<std::string>& filepaths) {
        // Implementation would find and restore best performing checkpoint
        throw nlm::IOError("Best checkpoint restoration not yet implemented");
    }, py::arg("brains"), py::arg("filepaths"),
       "Restore best performing brain from checkpoints");

    // Visualization and analysis
    m.def("plot_activity", [](std::shared_ptr<Brain> brain,
                            const std::string& output_file) {
        // Implementation would generate activity plots
        throw nlm::PluginError("Plot activity not yet implemented");
    }, py::arg("brain"), py::arg("output_file"),
       "Generate neural activity plots");

    m.def("plot_connectivity", [](std::shared_ptr<Brain> brain,
                                const std::string& output_file) {
        // Implementation would generate connectivity matrices
        throw nlm::PluginError("Plot connectivity not yet implemented");
    }, py::arg("brain"), py::arg("output_file"),
       "Generate connectivity matrix plots");

    m.def("analyze_simulation", [](std::shared_ptr<Brain> brain,
                                  std::shared_ptr<SimpleWorld> world,
                                  const std::string& output_file) {
        // Implementation would perform comprehensive analysis
        throw nlm::PluginError("Analyze simulation not yet implemented");
    }, py::arg("brain"), py::arg("world"), py::arg("output_file"),
       "Perform comprehensive simulation analysis");

    // Memory management
    m.def("set_memory_limit", [](std::shared_ptr<Brain> brain,
                               size_t limit_bytes) {
        // Implementation would set memory limit
        throw nlm::BrainError("Memory limit setting not yet implemented");
    }, py::arg("brain"), py::arg("limit_bytes"),
       "Set memory limit for brain operation");

    m.def("compact_memory", [](std::shared_ptr<Brain> brain) {
        // Implementation would compact unused memory
        throw nlm::BrainError("Memory compaction not yet implemented");
    }, py::arg("brain"),
       "Compact unused memory in brain");

    // Advanced configuration options
    m.def("get_system_info", []() {
        py::dict info;
        info["version"] = "NLM 0.1.0";
        info["max_neurons"] = 1000000;
        info["max_synapses"] = 10000000;
        info["memory_pools"] = true;
        info["parallel_processing"] = true;
        info["visualization"] = true;
        info["plugins"] = true;
        return info;
    }, "Get system information and capabilities");

    m.def("create_experimental_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set experimental configuration
        config->set("brain.neuron_count", 10000);
        config->set("brain.synapse_density", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.stdp.learning_rate", 0.01);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.curiosity.exploration_rate", 0.3);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.novelty.sensitivity", 1.5);
        config->set("memory.working.capacity", 1000);
        config->set("memory.episodic.capacity", 100);
        config->set("memory.associative.capacity", 500);
        return config;
    }, "Create experimental configuration for advanced research");
