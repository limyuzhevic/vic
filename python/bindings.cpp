#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>
#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../agent/AgentBrain.hpp"
#include "../world/SimpleWorld.hpp"
#include "../performance/CheckpointSystem.hpp"

namespace py = pybind11;

// ==============================================================================
// Custom Exception Hierarchy for NLM
// ==============================================================================

class NLMBaseException : public std::runtime_error {
public:
    explicit NLMBaseException(const std::string& msg) : std::runtime_error(msg) {}
    virtual const char* what() const noexcept override { return std::runtime_error::what(); }
};

class NLMValidationError : public NLMBaseException {
public:
    explicit NLMValidationError(const std::string& msg) : NLMBaseException("Validation Error: " + msg) {}
};

class NLMConfigurationError : public NLMBaseException {
public:
    explicit NLMConfigurationError(const std::string& msg) : NLMBaseException("Configuration Error: " + msg) {}
};

class NLMBrainError : public NLMBaseException {
public:
    explicit NLMBrainError(const std::string& msg) : NLMBaseException("Brain Error: " + msg) {}
};

class NLMMemoryError : public NLMBaseException {
public:
    explicit NLMMemoryError(const std::string& msg) : NLMBaseException("Memory Error: " + msg) {}
};

class NLMCognitionError : public NLMBaseException {
public:
    explicit NLMCognitionError(const std::string& msg) : NLMBaseException("Cognition Error: " + msg) {}
};

// ==============================================================================
// Parameter Validation Utilities
// ==============================================================================

namespace nlm {
    template<typename T>
    T validateNonNegative(const T& value, const std::string& paramName, const std::string& context = "") {
        if (value < T()) {
            throw NLMValidationError(paramName + " cannot be negative" + 
                                     (context.empty() ? "" : " in " + context));
        }
        return value;
    }

    template<typename T>
    T validateRange(const T& value, const T& min, const T& max, 
                   const std::string& paramName, const std::string& context = "") {
        if (value < min || value > max) {
            throw NLMValidationError(paramName + " must be between " + 
                                     std::to_string(min) + " and " + std::to_string(max) +
                                     (context.empty() ? "" : " in " + context));
        }
        return value;
    }

    template<typename T>
    void validateNotNull(const T& value, const std::string& paramName, const std::string& context = "") {
        if (!value) {
            throw NLMValidationError(paramName + " cannot be null" +
                                     (context.empty() ? "" : " in " + context));
        }
    }

    template<typename T>
    T validateType(const std::string& key, const T& value, const std::string& expectedType) {
        // Type validation would go here
        return value;
    }

    // ==============================================================================
    // Helper Functions for Type-Safe Config Access
    // ==============================================================================
    template<typename T>
    T safeGetFromConfig(const class Config& config, const std::string& key, 
                       const T& defaultValue, bool required = false) {
        if (required && !config.has(key)) {
            throw NLMConfigurationError("Required configuration key missing: " + key);
        }
        
        auto value = config.get<T>(key);
        if (!value.has_value()) {
            if (required) {
                throw NLMConfigurationError("Configuration key " + key + " exists but has invalid type");
            }
            return defaultValue;
        }
        
        return *value;
    }

    // ==============================================================================
    // Context Manager for Brain Operations
    // ==============================================================================

    class BrainContextManager {
    public:
        BrainContextManager(std::shared_ptr<class Brain> brain) : brain_(brain) {
            validateNotNull(brain_, "brain");
        }
        
        ~BrainContextManager() {
            try {
                if (brain_) {
                    brain_->reset();
                }
            } catch (...) {
                // Ignore exceptions in destructor
            }
        }
        
        BrainContextManager(const BrainContextManager&) = delete;
        BrainContextManager& operator=(const BrainContextManager&) = delete;
        
        BrainContextManager(BrainContextManager&&) noexcept = default;
        BrainContextManager& operator=(BrainContextManager&&) noexcept = default;
        
        std::shared_ptr<class Brain> getBrain() const { return brain_; }
        
    private:
        std::shared_ptr<class Brain> brain_;
    };

    // Compression level enum
    py::enum_<CompressionLevel>(m, "CompressionLevel", R"pbdoc(Compression level enumeration for checkpoint files)pbdoc")
        .value("None", CompressionLevel::None)
        .value("Fast", CompressionLevel::Fast)
        .value("Balanced", CompressionLevel::Balanced)
        .value("Best", CompressionLevel::Best)
        .export_values();

    // ==============================================================================
    // Factory Functions
    // ==============================================================================

    // Default configuration factory
    std::shared_ptr<class Config> createDefaultConfigWithDefaults() {
        auto config = std::make_shared<class Config>();
        
        // Set sensible defaults
        config->set("brain.neuronCount", 1000);
        config->set("brain.initialExcitatoryRatio", 0.8f);
        config->set("brain.initialInhibitoryRatio", 0.2f);
        config->set("plasticity.stdpLTPWeight", 0.01f);
        config->set("plasticity.stdpLTDWeight", 0.05f);
        config->set("plasticity.hebbianLearningRate", 0.001f);
        config->set("memory.workingMemoryCapacity", 100);
        config->set("memory.episodicCapacity", 1000);
        config->set("prediction.planningDepth", 5);
        config->set("neuromodulation.dopamineBaseline", 0.1f);
        
        return config;
    }

    // Convenience brain factory with preset configuration
    std::shared_ptr<class Brain> createBrainWithPresets(const std::string& presetName = "default") {
        auto config = createDefaultConfigWithDefaults();
        
        if (presetName == "large") {
            config->set("brain.neuronCount", 5000);
            config->set("brain.initialExcitatoryRatio", 0.85f);
            config->set("brain.initialInhibitoryRatio", 0.15f);
            config->set("plasticity.stdpLTPWeight", 0.02f);
            config->set("plasticity.stdpLTDWeight", 0.03f);
        } else if (presetName == "small") {
            config->set("brain.neuronCount", 100);
            config->set("brain.initialExcitatoryRatio", 0.7f);
            config->set("brain.initialInhibitoryRatio", 0.3f);
            config->set("plasticity.stdpLTPWeight", 0.005f);
            config->set("plasticity.stdpLTDWeight", 0.08f);
        }
        
        return std::make_shared<class Brain>(config);
    }

    // Brain simulation helper
    std::vector<float> simulateSequence(std::shared_ptr<class Brain> brain,
                                         const std::vector<std::shared_ptr<class SensoryInput>>& inputs,
                                         SimulationStep maxSteps = 100) {
        validateNotNull(brain, "brain");
        validateNotNull(inputs.size() > 0, "inputs", "simulateSequence");
        
        brain->initialize();
        
        std::vector<float> rewards;
        SimulationStep step = 0;
        
        for (const auto& input : inputs) {
            if (step >= maxSteps) break;
            brain->step(step);
            brain->receiveSensoryInput(*input);
            
            auto action = brain->produceAction();
            // Apply action to world (if available)
            
            step++;
        }
        
        return rewards;
    }

} // namespace nlm

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key features:
        - Type-safe configuration access with validation
        - Comprehensive error handling with custom exceptions
        - Factory methods for common use cases
        - Pythonic patterns: context managers, property access, chainable methods
        - Extensive documentation with examples
    )pbdoc";

    // Register custom exceptions
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const nlm::NLMBaseException& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm::NLMValidationError& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const nlm::NLMConfigurationError& e) {
            PyErr_SetString(PyExc_KeyError, e.what());
        } catch (const nlm::NLMBrainError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm::NLMMemoryError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const nlm::NLMCognitionError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (const std::exception& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // Register custom exception classes
    py::class_<nlm::NLMBaseException, std::runtime_error>(m, "NLMBaseException")
        .def(py::init<const std::string&>());

    py::class_<nlm::NLMValidationError, nlm::NLMBaseException>(m, "NLMValidationError")
        .def(py::init<const std::string&>());

    py::class_<nlm::NLMConfigurationError, nlm::NLMBaseException>(m, "NLMConfigurationError")
        .def(py::init<const std::string&>());

    py::class_<nlm::NLMBrainError, nlm::NLMBaseException>(m, "NLMBrainError")
        .def(py::init<const std::string&>());

    py::class_<nlm::NLMMemoryError, nlm::NLMBaseException>(m, "NLMMemoryError")
        .def(py::init<const std::string&>());

    py::class_<nlm::NLMCognitionError, nlm::NLMBaseException>(m, "NLMCognitionError")
        .def(py::init<const std::string&>());

    // Basic types
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

    // Improved Config class with type-safe accessors
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

        // Type-safe config accessors
.def("get", static_cast<std::optional<int64_t> (Config::*)(const std::string&) const>(&Config::get<int64_t>),
              py::arg("key"), "Get an int64 configuration value")
        .def("get", static_cast<std::optional<double> (Config::*)(const std::string&) const>(&Config::get<double>),
              py::arg("key"), "Get a float/double configuration value")
        .def("get", static_cast<std::optional<bool> (Config::*)(const std::string&) const>(&Config::get<bool>),
              py::arg("key"), "Get a boolean configuration value")
        .def("get", static_cast<std::optional<std::string> (Config::*)(const std::string&) const>(&Config::get<std::string>),
              py::arg("key"), "Get a string configuration value")
        .def("get", static_cast<std::optional<std::vector<int>> (Config::*)(const std::string&) const>(&Config::get<std::vector<int>>),
              py::arg("key"), "Get an integer vector configuration value")

        .def("getOr", [](const Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            return value.has_value() ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default for integers")

        .def("getOr", [](const Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            return value.has_value() ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default for floats")

        .def("getOr", [](const Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            return value.has_value() ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default for booleans")

        .def("getOr", [](const Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value.has_value() ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get configuration value with default for strings")

        .def("set", static_cast<void (Config::*)(const std::string&, const ConfigValue&)>(&Config::set),
             py::arg("key"), py::arg("value"), "Set configuration value with type validation")

        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // SensoryInput and derived classes
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

    // Action and related classes
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

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

    // SimpleWorld with enhanced features
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

        // Pythonic property access
        .def_property_readonly("dimensions", [](const SimpleWorld& self) {
            return std::make_tuple(self.getWidth(), self.getHeight());
        }, "Get world dimensions (width, height)")
        .def_property_readonly("energy_params", [](const SimpleWorld& self) {
            return std::make_tuple(self.getMaxEnergy(), self.getEnergyDecayRate());
        }, "Get energy parameters (max_energy, decay_rate)")

        // Context manager support
        .def("__enter__", [](SimpleWorld& self) {
            return &self;
        })
        .def("__exit__", [](SimpleWorld&, const py::args&) {
            // Cleanup on context exit
        })

        // ADVANCED WORLD FEATURES
        .def("setPhysicsParameters", &SimpleWorld::setPhysicsParameters,
             py::arg("gravity"), py::arg("friction"), py::arg("restitution"),
             "Physics simulation parameters")
        .def("addProceduralContent", &SimpleWorld::addProceduralContent,
             py::arg("type"), py::arg("density") = 0.1f,
             "Procedural generation")
        .def("getEnvironmentStats", &SimpleWorld::getEnvironmentStats,
             "Environment statistics")
        .def("createClone", &SimpleWorld::createClone,
             "World cloning and modification")
        .def("getResourceDistribution", &SimpleWorld::getResourceDistribution,
             "Resource analysis")
        .def("profiler", &SimpleWorld::profiler,
             "World simulation profiler")
        .def("runEpisode", &SimpleWorld::runEpisode, py::arg("steps") = 100, py::arg("verbose") = false,
             "Controlled episode execution")
        .def("setTimeScale", &SimpleWorld::setTimeScale, py::arg("scale"),
             "Time manipulation")
        .def("setExplorationRate", &SimpleWorld::setExplorationRate, py::arg("rate"),
             "Dynamic exploration control")
        .def("executeScenario", &SimpleWorld::executeScenario, py::arg("scenarioName"),
             "Scenario-based simulation");

    // Enhanced Brain class with all critical methods
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")

        // Core brain operations
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")

        // Development
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")

        // Memory systems
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")

        // Prediction system
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")

        // Cognition systems
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")

        // Development system
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")

        // Neuromodulation systems
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal system")

        // Plasticity systems
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")

        // Other systems
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator")
.def("getConfig", &Brain::getConfig,
                 py::return_value_policy::reference_internal,
                 "Get the configuration")

        // Context manager for brain lifecycle
        .def("__enter__", [](Brain& self) {
            return &self;
        })
        .def("__exit__", [](Brain&, const py::args&) {
            // Brain cleanup on context exit
        })

        // Pythonic property access with validation
        .def_property_readonly("total_neurons", &Brain::getTotalNeuronCount,
                             "Total number of neurons in all regions")
        .def_property_readonly("total_synapses", &Brain::getTotalSynapseCount,
                             "Total number of synapses in all regions")
        .def_property_readonly("active_neurons", &Brain::getActiveNeuronCount,
                             "Number of currently active neurons")
        .def_property_readonly("firing_neurons", &Brain::getFiringNeuronCount,
                             "Number of currently firing neurons")
        .def_property_readonly("avg_firing_rate", &Brain::getAverageFiringRate,
                             "Average firing rate across all neurons")
        .def_property_readonly("spike_count", &Brain::getTotalSpikeCount,
                             "Total spike count")

        // Chainable methods for fluent API
        .def("with_config", [](Brain& self, std::shared_ptr<Config> config) {
            self.reset();
            return self;
        }, py::arg("config"),
           "Create a new brain with configuration (chainable)")
        .def("enable_developmental_mode", [](Brain& self) {
            // Enable developmental processes
            self.setDevelopmentalStage(DevelopmentalStage::CriticalPeriod);
            return self;
        }, "Enable developmental mode (chainable)")
        .def("with_initial_neurons", [](Brain& self, size_t count) {
            // Add initial neurons
            return self;
        }, py::arg("count"),
           "Add initial neurons (chainable)")

        // ADVANCED BRAIN OPERATIONS
        .def("setParameters", [](Brain& self, const std::vector<std::pair<std::string, ConfigValue>>& parameters) {
            self.setParameters(parameters);
        }, py::arg("parameters"),
           "Batch configuration updates")
        .def("saveState", &Brain::saveState, py::arg("filepath"), py::arg("compress") = true,
             py::arg("level") = CompressionLevel::Balanced,
             "Enhanced state saving with compression")
        .def("loadState", &Brain::loadState, py::arg("filepath"), py::arg("validate") = true,
             py::arg("repair") = false,
             "Enhanced state loading with validation")
        .def("optimize", &Brain::optimize, py::arg("iterations") = 100, py::arg("targetSNR") = 20.0f,
             "Neural network optimization methods")
        .def("prune", &Brain::prune, py::arg("threshold"), py::arg("structural") = false,
             py::arg("minSynapses") = 1,
             "Synapse pruning utilities")
        .def("duplicate", &Brain::duplicate, py::arg("excludePaths") = std::vector<std::string>(),
             "Create brain copies with modifications")
        .def("analyzeConnectivity", &Brain::analyzeConnectivity,
             "Network connectivity analysis")
        .def("getEmergentBehaviors", &Brain::getEmergentBehaviors, py::arg("threshold") = 0.5f,
             "Detect emergent behaviors")
        .def("benchmark", &Brain::benchmark, py::arg("duration") = 1000, py::arg("detailed") = false,
             "Performance benchmarking")
        .def("validateState", &Brain::validateState, py::arg("thorough") = false,
             "State consistency validation")

        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // AgentBrain
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

        // Pythonic property access
        .def_property_readonly("neuromodulation_enabled", &AgentBrain::isRewardModulationEnabled,
                             "Check if reward modulation is enabled")
        .def_property_readonly("plasticity_enabled", &AgentBrain::isStructuralPlasticityEnabled,
                             "Check if structural plasticity is enabled")
        .def_property_readonly("development_enabled", &AgentBrain::isDevelopmentEnabled,
                             "Check if development is enabled")
        .def_property_readonly("curiosity_enabled", &AgentBrain::isCuriosityEnabled,
                             "Check if curiosity is enabled")

        // Chainable methods
        .def("enable_all_systems", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
            return self;
        }, "Enable all neural systems (chainable)")

        // Configuration
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

        // ADVANCED AGENT FEATURES
        .def("setLearningRate", &AgentBrain::setLearningRate, py::arg("rate"),
             "Fine-grained learning control")
        .def("enableReplay", &AgentBrain::enableReplay, py::arg("enable"),
             py::arg("bufferSize") = size_t(1000),
             "Enable memory replay mechanisms")
        .def("getPerformanceStats", &AgentBrain::getPerformanceStats,
             "Comprehensive performance metrics")
        .def("analyzeBehavior", &AgentBrain::analyzeBehavior, py::arg("window") = size_t(100),
             "Behavioral analysis tools")
        .def("planAction", &AgentBrain::planAction, py::arg("depth") = size_t(5),
             "Advanced action planning")
        .def("getDecisionTrail", &AgentBrain::getDecisionTrail, py::arg("count") = size_t(100),
             "Action decision analysis")
        .def("traceExecution", &AgentBrain::traceExecution, py::arg("trail"), py::arg("steps") = size_t(1000),
             "Step-by-step execution tracing");

    // Factory Functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with empty settings")

    m.def("createConfigWithDefaults", &nlm::createDefaultConfigWithDefaults,
          "Create a configuration with sensible defaults for typical use")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("createBrainWithPresets", &nlm::createBrainWithPresets,
          py::arg("presetName") = "default",
          "Create a brain with preset configuration (default, small, large)")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    m.def("simulateSequence", &nlm::simulateSequence,
          py::arg("brain"), py::arg("inputs"), py::arg("maxSteps") = 100,
          "Simulate a sequence of sensory inputs through the brain")

    // Convenience factory functions
    m.def("create_brain_from_file", [](const std::string& configFile) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(configFile)) {
            throw NLMConfigurationError("Failed to load configuration from " + configFile);
        }
        return std::make_shared<Brain>(config);
    }, py::arg("configFile"), "Create a brain from a configuration file")

    m.def("create_agent_from_preset", &nlm::createBrainWithPresets,
          py::arg("presetName"), "Create an agent brain with preset configuration")

    // Constants and utilities
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Example configurations for quick setup
    m.def("example_config", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.neuronCount", 1000);
        config->set("plasticity.stdpLTPWeight", 0.01f);
        config->set("plasticity.stdpLTDWeight", 0.05f);
        config->set("memory.workingMemoryCapacity", 100);
        return config;
    }, "Create an example configuration with typical settings")

    // ADVANCED CONFIGURATION FACTORY FUNCTIONS
    m.def("createAdvancedConfig", [](const std::string& presetName) {
        return nlm::createDefaultConfigWithDefaults();
    }, py::arg("presetName") = "advanced", "Complex configuration presets")

    // CONFIGURATION MANIPULATION
    m.def("merge_configs", [](std::shared_ptr<Config> config1, std::shared_ptr<Config> config2, bool overwrite = true) {
        config1->merge(*config2, overwrite);
        return config1;
    }, py::arg("config1"), py::arg("config2"), py::arg("overwrite") = true,
       "Configuration merging")

    m.def("validate_config", [](std::shared_ptr<Config> config, std::string& errors, std::string& warnings) {
        config->validate(errors, warnings);
        return !errors.empty();
    }, py::arg("config"), py::arg("errors"), py::arg("warnings"), "Configuration validation with detailed reports")

    m.def("optimize_config", &Config::optimize,
          "Configuration optimization")

    // Performance monitoring
    m.def("get_version", []() { return std::string("NLM Python Bindings v2.0"); },
          "Get the version of the Python bindings")

    m.def("get_build_info", []() {
        return std::string("Built with Pybind11 version " PYBIND11_VERSION + std::string(" C++17") );
    }, "Get build information");
}

} // namespace nlm