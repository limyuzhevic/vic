#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/numpy.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>

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

// =============================================================================
// Custom Exception Hierarchy
// =============================================================================

class NLMRuntimeError : public std::runtime_error {
public:
    explicit NLMRuntimeError(const std::string& message) : std::runtime_error(message) {}
};

class NLMConfigurationError : public NLMRuntimeError {
public:
    explicit NLMConfigurationError(const std::string& message) : NLMRuntimeError(message) {}
};

class NLMBrainError : public NLMRuntimeError {
public:
    explicit NLMBrainError(const std::string& message) : NLMRuntimeError(message) {}
};

class NLMFileError : public NLMRuntimeError {
public:
    explicit NLMFileError(const std::string& message) : NLMRuntimeError(message) {}
};

class NLMWorldError : public NLMRuntimeError {
public:
    explicit NLMWorldError(const std::string& message) : NLMRuntimeError(message) {}
};

// =============================================================================
// Utility Functions
// =============================================================================

namespace utils {
    // Convert vector to numpy array
    template<typename T>
    py::array_t<T> vectorToNumpy(const std::vector<T>& vec) {
        return py::array_t<T>(vec.size(), vec.data());
    }

    // Convert numpy array to vector
    template<typename T>
    std::vector<T> numpyToVector(const py::array_t<T>& arr) {
        return std::vector<T>(arr.data(), arr.data() + arr.size());
    }

    // Safe type conversion for config values
    template<typename T>
    T getConfigValueSafe(const ConfigValue& value) {
        if constexpr (std::is_same_v<T, int>) {
            if (std::holds_alternative<int>(value)) {
                return std::get<int>(value);
            }
        } else if constexpr (std::is_same_v<T, double>) {
            if (std::holds_alternative<double>(value)) {
                return std::get<double>(value);
            }
        } else if constexpr (std::is_same_v<T, bool>) {
            if (std::holds_alternative<bool>(value)) {
                return std::get<bool>(value);
            }
        } else if constexpr (std::is_same_v<T, std::string>) {
            if (std::holds_alternative<std::string>(value)) {
                return std::get<std::string>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<int>>) {
            if (std::holds_alternative<std::vector<int>>(value)) {
                return std::get<std::vector<int>>(value);
            }
        } else if constexpr (std::is_same_v<T, std::vector<double>>) {
            if (std::holds_alternative<std::vector<double>>(value)) {
                return std::get<std::vector<double>>(value);
            }
        }
        throw NLMConfigurationError("Config value type mismatch for key");
    }

    // Format error messages
    std::string formatError(const std::string& operation, const std::string& context) {
        std::ostringstream oss;
        oss << "NLM " << operation << " error";
        if (!context.empty()) {
            oss << ": " << context;
        }
        return oss.str();
    }

    // Create action from ActionType
    Action createAction(ActionType type) {
        return Action(type);
    }

    // Create action from ActionType with parameters
    Action createAction(ActionType type, const std::vector<float>& params) {
        return Action(type, params);
    }
}

// =============================================================================
// Python Context Manager Classes
// =============================================================================

class BrainContextManager {
public:
    BrainContextManager(std::shared_ptr<Brain> brain) : brain_(brain) {
        if (brain_) {
            brain_->reset();
        }
    }

    ~BrainContextManager() {
        if (brain_) {
            // Brain state is maintained across context
        }
    }

    std::shared_ptr<Brain> getBrain() const { return brain_; }

    void reset() {
        if (brain_) {
            brain_->reset();
        }
    }

    void step(SimulationStep step) {
        if (brain_) {
            brain_->step(step);
        }
    }

    void step(SimulationStep step, Timestamp time) {
        if (brain_) {
            brain_->step(step, time);
        }
    }

private:
    std::shared_ptr<Brain> brain_;
};

class WorldContextManager {
public:
    WorldContextManager(std::shared_ptr<SimpleWorld> world) : world_(world) {
        if (world_) {
            world_->reset();
        }
    }

    ~WorldContextManager() {
        if (world_) {
            // World state is maintained across context
        }
    }

    std::shared_ptr<SimpleWorld> getWorld() const { return world_; }

    void reset() {
        if (world_) {
            world_->reset();
        }
    }

    void update(Timestamp timestep) {
        if (world_) {
            world_->update(timestep);
        }
    }

private:
    std::shared_ptr<SimpleWorld> world_;
};

// =============================================================================
// Type Conversion and Helper Functions
// =============================================================================

namespace pybind11 {
    // Custom pybind11 overloads for better Python integration
    template<typename... Args>
    py::class_<Action> action_class(py::module_& m, const char* name, Args&&... args) {
        return m.def("createAction", &utils::createAction, py::arg("type"),
            "Create an action from ActionType", std::forward<Args>(args)...)
            .def("createActionWithParams", &utils::createAction, py::arg("type"), py::arg("params"),
                "Create an action from ActionType with parameters");
    }
}

// =============================================================================
// Python Bindings Module
// =============================================================================

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides enhanced Pythonic interfaces for Brain, Config, AgentBrain, SimpleWorld,
        and related classes with improved error handling, numpy support, and convenience methods.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMRuntimeError>(m, "RuntimeError");
    py::register_exception<NLMConfigurationError>(m, "ConfigurationError");
    py::register_exception<NLMBrainError>(m, "BrainError");
    py::register_exception<NLMFileError>(m, "FileError");
    py::register_exception<NLMWorldError>(m, "WorldError");

    // =============================================================================
    // Core Type Definitions
    // =============================================================================

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

    // Enums
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

    // =============================================================================
    // Config Class with Enhanced Python Interface
    // =============================================================================

    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration system for NLM with Python-friendly interface)pbdoc")
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
        // Enhanced configuration access methods
        .def("getInt", [](Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value) return *value;
            throw NLMConfigurationError("No int value found for key: " + key);
        }, py::arg("key"), "Get integer configuration value")
        .def("getDouble", [](Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) return *value;
            throw NLMConfigurationError("No double value found for key: " + key);
        }, py::arg("key"), "Get double configuration value")
        .def("getBool", [](Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            throw NLMConfigurationError("No bool value found for key: " + key);
        }, py::arg("key"), "Get boolean configuration value")
        .def("getString", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw NLMConfigurationError("No string value found for key: " + key);
        }, py::arg("key"), "Get string configuration value")
        .def("getIntVector", [](Config& self, const std::string& key) {
            auto value = self.get<std::vector<int>>(key);
            if (value) return *value;
            throw NLMConfigurationError("No int vector found for key: " + key);
        }, py::arg("key"), "Get integer vector configuration value")
        .def("getDoubleVector", [](Config& self, const std::string& key) {
            auto value = self.get<std::vector<double>>(key);
            if (value) return *value;
            throw NLMConfigurationError("No double vector found for key: " + key);
        }, py::arg("key"), "Get double vector configuration value")
        .def("getStringVector", [](Config& self, const std::string& key) {
            auto value = self.get<std::vector<std::string>>(key);
            if (value) return *value;
            throw NLMConfigurationError("No string vector found for key: " + key);
        }, py::arg("key"), "Get string vector configuration value")
        // Set methods with type checking
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set<int>(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value) {
            self.set<double>(key, value);
        }, py::arg("key"), py::arg("value"), "Set double configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set<bool>(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("setString", [](Config& self, const std::string& key, const std::string& value) {
            self.set<std::string>(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("setIntVector", [](Config& self, const std::string& key, const std::vector<int>& value) {
            self.set<std::vector<int>>(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer vector configuration value")
        .def("setDoubleVector", [](Config& self, const std::string& key, const std::vector<double>& value) {
            self.set<std::vector<double>>(key, value);
        }, py::arg("key"), py::arg("value"), "Set double vector configuration value")
        .def("setStringVector", [](Config& self, const std::string& key, const std::vector<std::string>& value) {
            self.set<std::vector<std::string>>(key, value);
        }, py::arg("key"), py::arg("value"), "Set string vector configuration value")
        // Type-safe getOr methods
        .def("getOrInt", [](Config& self, const std::string& key, int defaultValue) {
            return self.getOr<int>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get integer configuration value or default")
        .def("getOrDouble", [](Config& self, const std::string& key, double defaultValue) {
            return self.getOr<double>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get double configuration value or default")
        .def("getOrBool", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getOr<bool>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean configuration value or default")
        .def("getOrString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getOr<std::string>(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get string configuration value or default")
        // Convenience methods
        .def("configureBrain", [](Config& self, int neuronCount, double synapseDensity) {
            self.setInt("brain.neuron_count", neuronCount);
            self.setDouble("brain.synapse_density", synapseDensity);
        }, py::arg("neuronCount"), py::arg("synapseDensity"), "Quick configure brain parameters")
        .def("configureWorld", [](Config& self, int width, int height) {
            self.setInt("world.width", width);
            self.setInt("world.height", height);
        }, py::arg("width"), py::arg("height"), "Quick configure world parameters")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // =============================================================================
    // Sensory Input Classes
    // =============================================================================

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
              py::arg("channels") = 3)
        .def("setData", [](Vision& self, const py::array_t<float>& data) {
            auto vec = utils::numpyToVector<float>(data);
            self.setData(vec);
        }, py::arg("data"), "Set vision data from numpy array (recommended for performance)")
        .def("setDataList", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data from Python list (alternative)")
        .def("getDataAsNumpy", [](Vision& self) {
            return utils::vectorToNumpy<float>(self.getData());
        }, "Get vision data as numpy array (recommended for performance)")
        .def("getDataAsList", [](Vision& self) {
            return self.getData();
        }, "Get vision data as Python list (alternative)")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const py::array_t<float>& data) {
            auto vec = utils::numpyToVector<float>(data);
            self.setData(vec);
        }, py::arg("data"), "Set audio data from numpy array (recommended)")
        .def("setDataList", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data from Python list (alternative)")
        .def("getDataAsNumpy", [](Audio& self) {
            return utils::vectorToNumpy<float>(self.getData());
        }, "Get audio data as numpy array (recommended)")
        .def("getDataAsList", [](Audio& self) {
            return self.getData();
        }, "Get audio data as Python list (alternative)")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // =============================================================================
    // Action Class with Enhanced Python Interface
    // =============================================================================

    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation with factory methods and utilities)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        // Action factory methods
        .def_static("createMoveForward", []() {
            return Action(ActionType::MoveForward);
        }, "Create a MoveForward action")
        .def_static("createMoveBackward", []() {
            return Action(ActionType::MoveBackward);
        }, "Create a MoveBackward action")
        .def_static("createTurnLeft", []() {
            return Action(ActionType::TurnLeft);
        }, "Create a TurnLeft action")
        .def_static("createTurnRight", []() {
            return Action(ActionType::TurnRight);
        }, "Create a TurnRight action")
        .def_static("createInteract", []() {
            return Action(ActionType::Interact);
        }, "Create an Interact action")
        .def_static("createLook", [](ActionType lookDirection) {
            return Action(ActionType::Look, std::vector<float>{static_cast<float>(lookDirection)});
        }, py::arg("lookDirection"), "Create a Look action with direction")
        .def_static("createWait", []() {
            return Action(ActionType::Wait);
        }, "Create a Wait action")
        .def_static("createEat", []() {
            return Action(ActionType::Eat);
        }, "Create an Eat action")
        .def_static("createRest", []() {
            return Action(ActionType::Rest);
        }, "Create a Rest action")
        // Action utilities
        .def("isComplete", &Action::isComplete, "Check if action is complete")
        .def("getParameterCount", &Action::getParameterCount, "Get number of parameters")
        .def("hasParameter", &Action::hasParameter, py::arg("index"), "Check if action has parameter at index")
        .def("getParameterValue", &Action::getParameter, py::arg("index"), "Get parameter value at index")
        .def("setParameterValue", &Action::setParameter, py::arg("index"), py::arg("value"), "Set parameter value at index")
        .def("validateParameters", &Action::validateParameters, "Validate action parameters")
        .def("toString", &Action::toString, "Get string representation of action");

    // =============================================================================
    // WorldObject Class
    // =============================================================================

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
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("contains", [](const WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return dx * dx + dy * dy <= self.radius * self.radius;
        }, py::arg("x"), py::arg("y"), "Check if point (x, y) is inside object");

    // =============================================================================
    // AgentBody Class
    // =============================================================================

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state with Python properties)pbdoc")
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
        .def("getPosition", [](AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get agent position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set agent position")
        .def("getVelocity", [](AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get agent velocity as (vx, vy) tuple")
        .def("setVelocity", [](AgentBody& self, float vx, float vy) {
            self.velocityX = vx;
            self.velocityY = vy;
        }, py::arg("vx"), py::arg("vy"), "Set agent velocity")
        .def("getAngularVelocity", [](AgentBody& self) {
            return std::make_pair(std::cos(self.angularVelocity), std::sin(self.angularVelocity));
        }, "Get agent orientation as (cos, sin) tuple")
        .def("setAngularVelocity", [](AgentBody& self, float angle) {
            self.angularVelocity = angle;
        }, py::arg("angle"), "Set agent angular velocity (angle in radians)");

    // =============================================================================
    // ActionResult Class
    // =============================================================================

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isSuccessful", [](ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        .def("toString", [](ActionResult& self) {
            std::ostringstream oss;
            oss << "ActionResult(reward=" << self.reward 
                << ", success=" << (self.success ? "true" : "false")
                << ", message=\"" << self.message << "\")";
            return oss.str();
        }, "Get string representation of action result");

    // =============================================================================
    // SensoryPercept Class
    // =============================================================================

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data with enhanced Python interface)pbdoc")
        .def(py::init<>())
        .def("getVision", [](SensoryPercept& self) {
            return &self.getVision();
        }, "Get vision data as Vision object (read-only access)")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", [](SensoryPercept& self) {
            return &self.getTouch();
        }, "Get touch data as InternalSignals object (read-only access)")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data")
        .def("getInternal", [](SensoryPercept& self) {
            return &self.getInternal();
        }, "Get internal signals as InternalSignals object (read-only access)")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals")
        .def("getProprioception", [](SensoryPercept& self) {
            return &self.getProprioception();
        }, "Get proprioception as InternalSignals object (read-only access)")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception")
        .def("getAudio", [](SensoryPercept& self) {
            return &self.getAudio();
        }, "Get audio data as Audio object (read-only access)")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all signals as concatenated vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp)
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp")
        .def("hasVision", [](SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if vision data is present")
        .def("hasAudio", [](SensoryPercept& self) {
            return self.getAudio().getNumSamples() > 0;
        }, "Check if audio data is present")
        .def("hasInternal", [](SensoryPercept& self) {
            return !self.getInternal().getAllSignals().empty();
        }, "Check if internal signals are present")
        .def("hasProprioception", [](SensoryPercept& self) {
            return !self.getProprioception().getAllSignals().empty();
        }, "Check if proprioception is present")
        .def("toString", [](SensoryPercept& self) {
            std::ostringstream oss;
            oss << "SensoryPercept(vision=" << self.getVisionWidth() << "x" << self.getVisionHeight()
                << ", audio=" << self.getAudio().getNumSamples() << " samples"
                << ", internal signals=" << self.getInternal().getAllSignals().size()
                << ", proprioception=" << self.getProprioception().getAllSignals().size() << ")";
            return oss.str();
        }, "Get string representation of sensory percept");

    // =============================================================================
    // SimpleWorld Class
    // =============================================================================

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world with enhanced Python interface and utilities)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"),
              "Configure world dimensions and agent vision")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world by timestep seconds")
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
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position (x, y)")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position (x, y) is valid (not blocked)")
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
        // World utilities and advanced features
        .def("rayCast", [](SimpleWorld& self, float startX, float startY, float angle, float maxDistance) {
            // Implementation of ray casting for sensor simulation
            float dx = std::cos(angle) * maxDistance;
            float dy = std::sin(angle) * maxDistance;
            float endX = startX + dx;
            float endY = startY + dy;
            
            // Find the first object hit by the ray
            float closestDist = maxDistance;
            WorldObject* hitObject = nullptr;
            
            for (const auto& obj : self.getObjects()) {
                if (!obj.active) continue;
                
                // Line-sphere intersection
                float objDx = obj.x - startX;
                float objDy = obj.y - startY;
                float objDist = std::sqrt(objDx * objDx + objDy * objDy);
                
                if (objDist < closestDist) {
                    // Check if ray actually hits the sphere
                    float cosTheta = (objDx * dx + objDy * dy) / (objDist * maxDistance);
                    if (cosTheta > 0) {
                        closestDist = objDist;
                        hitObject = const_cast<WorldObject*>(&obj);
                    }
                }
            }
            
            if (hitObject) {
                return std::make_tuple(true, hitObject->type, hitObject->value, closestDist);
            }
            return std::make_tuple(false, WorldObjectType::Empty, 0.0f, maxDistance);
        }, py::arg("startX"), py::arg("startY"), py::arg("angle"), py::arg("maxDistance"),
             "Cast a ray from agent position to detect objects (returns: hit, type, value, distance)")
        .def("getObjectsInRange", [](SimpleWorld& self, float x, float y, float radius) {
            std::vector<WorldObject> objects;
            for (const auto& obj : self.getObjects()) {
                if (!obj.active) continue;
                float dx = obj.x - x;
                float dy = obj.y - y;
                if (dx * dx + dy * dy <= radius * radius) {
                    objects.push_back(obj);
                }
            }
            return objects;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Get all objects within radius of position (x, y)")
        .def("getClosestObject", [](SimpleWorld& self, float x, float y) {
            float closestDist = std::numeric_limits<float>::max();
            WorldObject* closest = nullptr;
            
            for (const auto& obj : self.getObjects()) {
                if (!obj.active) continue;
                float dx = obj.x - x;
                float dy = obj.y - y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist < closestDist) {
                    closestDist = dist;
                    closest = const_cast<WorldObject*>(&obj);
                }
            }
            
            if (closest) {
                return std::make_tuple(true, *closest);
            }
            return std::make_tuple(false, WorldObject());
        }, py::arg("x"), py::arg("y"),
             "Get closest object to position (x, y)")
        .def("getObjectsByType", [](SimpleWorld& self, WorldObjectType type) {
            std::vector<WorldObject> objects;
            for (const auto& obj : self.getObjects()) {
                if (obj.active && obj.type == type) {
                    objects.push_back(obj);
                }
            }
            return objects;
        }, py::arg("type"), "Get all objects of specified type")
        .def("createResource", [](SimpleWorld& self, float x, float y, float value) {
            WorldObject obj(x, y, WorldObjectType::Resource, value);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("value"),
             "Create and add a resource object at position (x, y)")
        .def("createHazard", [](SimpleWorld& self, float x, float y, float severity) {
            WorldObject obj(x, y, WorldObjectType::Hazard, severity);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("severity"),
             "Create and add a hazard object at position (x, y)")
        .def("createWall", [](SimpleWorld& self, float x, float y, float radius) {
            WorldObject obj(x, y, WorldObjectType::Wall, 1.0f, radius);
            self.addObject(obj);
            return obj;
        }, py::arg("x"), py::arg("y"), py::arg("radius"),
             "Create and add a wall object at position (x, y)")
        .def("toString", [](SimpleWorld& self) {
            std::ostringstream oss;
            oss << "SimpleWorld(size=" << self.getWidth() << "x" << self.getHeight()
                << ", time=" << self.getSimulationTime() << ", agents=" << self.getAgentCount() << ")";
            return oss.str();
        }, "Get string representation of world");

    // =============================================================================
    // AgentBrain Class
    // =============================================================================

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with Python properties and context managers)pbdoc")
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
        // Configuration methods as Python properties
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Convenience methods for common agent patterns
        .def("processStep", [](AgentBrain& self, const SimpleWorld& world, 
                             const SensoryPercept& percept, double timestep) {
            self.processSensoryInput(percept);
            self.updateDevelopment(timestep);
        }, py::arg("world"), py::arg("percept"), py::arg("timestep"),
             "Process sensory input and update development (convenience method)")
        .def("executeAction", [](AgentBrain& self, const SimpleWorld& world,
                               MotorCommand action, Timestamp timestamp) {
            world.applyMotorCommand(action, timestamp);
        }, py::arg("world"), py::arg("action"), py::arg("timestamp"),
             "Execute action in world (convenience method)")
        .def("getStatus", [](AgentBrain& self) {
            std::ostringstream oss;
            oss << "AgentBrain(dev_stage=" << static_cast<int>(self.getDevelopmentalStage())
                << ", curiosity=" << self.getCuriosityLevel()
                << ", novelty=" << self.getNoveltyLevel()
                << ", reward_mod=" << (self.isRewardModulationEnabled() ? "enabled" : "disabled")
                << ", struct_plastic=" << (self.isStructuralPlasticityEnabled() ? "enabled" : "disabled")
                << ", development=" << (self.isDevelopmentEnabled() ? "enabled" : "disabled")
                << ", curiosity=" << (self.isCuriosityEnabled() ? "enabled" : "disabled") << ")";
            return oss.str();
        }, "Get agent brain status summary")
        .def("toString", [](AgentBrain& self) {
            return self.getStatus();
        }, "Get string representation of agent brain");

    // =============================================================================
    // Brain Class with Enhanced Python Interface
    // =============================================================================

    py::class_<Brain>(m, "Brain", R"pbdoc(Enhanced brain simulation with Python properties, context managers, and advanced inspection)")
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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")
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
        // Convenience and Pythonic methods
        .def("getRegionByName", [](Brain& self, const std::string& name) {
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                if (region->getName() == name) {
                    return region.get();
                }
            }
            throw NLMBrainError("Region not found: " + name);
        }, py::arg("name"), "Get region by name (Pythonic accessor)")
        .def("getNeuronCountByType", [](Brain& self, NeuronType type) {
            size_t count = 0;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                count += region->getPopulationCountByType(type);
            }
            return count;
        }, py::arg("type"), "Get neuron count by type")
        .def("getSynapseCountByType", [](Brain& self, SynapseType type) {
            // Note: This would require exposing synapse counting methods
            // For now, this is a placeholder for future enhancement
            throw NLMBrainError("Synapse type counting not yet implemented");
        }, py::arg("type"), "Get synapse count by type")
        .def("getActiveRegions", [](Brain& self) {
            std::vector<RegionId> activeRegions;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                if (region->isActive()) {
                    activeRegions.push_back(region->getId());
                }
            }
            return activeRegions;
        }, "Get list of active region IDs")
        .def("getFiringRegions", [](Brain& self) {
            std::vector<RegionId> firingRegions;
            auto regions = self.getRegions();
            for (const auto& region : regions) {
                if (region->isFiring()) {
                    firingRegions.push_back(region->getId());
                }
            }
            return firingRegions;
        }, "Get list of firing region IDs")
        .def("getStatistics", [](Brain& self) {
            std::ostringstream oss;
            oss << "Brain Statistics:"
                << "\n  Total neurons: " << self.getTotalNeuronCount()
                << "\n  Total synapses: " << self.getTotalSynapseCount()
                << "\n  Active neurons: " << self.getActiveNeuronCount()
                << "\n  Firing neurons: " << self.getFiringNeuronCount()
                << "\n  Average firing rate: " << self.getAverageFiringRate()
                << "\n  Total spikes: " << self.getTotalSpikeCount()
                << "\n  Pending spikes: " << self.getPendingSpikeEventCount()
                << "\n  Regions: " << self.getRegionCount()
                << "\n  Development stage: " << static_cast<int>(self.getDevelopmentalStage());
            return oss.str();
        }, "Get comprehensive brain statistics as string")
        .def("toString", [](Brain& self) {
            return self.getStatistics();
        }, "Get string representation of brain")
        // Context manager support
        .def("__enter__", [](Brain& self) {
            return &self;
        })
        .def("__exit__", [](Brain& self, py::args) {
            self.reset();
        });

    // =============================================================================
    // Factory Functions
    // =============================================================================

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with Pythonic methods")

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration")

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world")

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface")

    // =============================================================================
    // Module-level constants and utilities
    // =============================================================================

    m.attr("VERSION") = "3.0.0-enhanced";  // Enhanced version with Python improvements

    m.def("getDefaultBrainConfig", []() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", 1000);
        config->setDouble("brain.synapse_density", 0.1);
        config->setInt("world.width", 20);
        config->setInt("world.height", 20);
        config->setInt("brain.v_thresh", -50);
        config->setInt("brain.v_rest", -70);
        return config;
    }, "Get a pre-configured default brain config")

    m.def("getQuickTestConfig", []() {
        auto config = std::make_shared<Config>();
        config->setInt("brain.neuron_count", 100);
        config->setDouble("brain.synapse_density", 0.05);
        return config;
    }, "Get a minimal config for quick testing")

    m.def("formatError", &utils::formatError, py::arg("operation"), py::arg("context"),
          "Format error messages for better debugging")

    m.def("createAction", &utils::createAction, py::arg("type"),
          "Create an action from ActionType (convenience function)")

    m.def("createActionWithParams", &utils::createAction, py::arg("type"), py::arg("params"),
          "Create an action from ActionType with parameters (convenience function)")

    // =============================================================================
    // Module Documentation
    // =============================================================================

    m.attr("__version__") = "3.0.0-enhanced";
    m.attr("__author__") = "NLM Research Team";
    m.attr("__description__") = R"pbdoc(
        Enhanced NLM Python bindings with:
        - Custom exception hierarchy (ConfigurationError, BrainError, FileError, WorldError)
        - Pythonic properties and context managers
        - NumPy array support for Vision/Audio data
        - Enhanced configuration API with type-safe access
        - Brain state inspection APIs
        - Action factory methods
        - World utilities (ray casting, object management)
        - Comprehensive error handling and documentation
        - Improved code organization and maintainability
    )pbdoc";
}

} // namespace nlm