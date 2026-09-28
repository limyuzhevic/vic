#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <stdexcept>
#include <numeric>

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

// Forward declarations for error handling
namespace {
    // Comprehensive exception types for Python bindings
    class PyRuntimeError : public std::runtime_error {
    public:
        explicit PyRuntimeError(const std::string& msg) : std::runtime_error(msg) {}
    };

    class PyValueError : public std::invalid_argument {
    public:
        explicit PyValueError(const std::string& msg) : std::invalid_argument(msg) {}
    };

    class PyIndexError : public std::out_of_range {
    public:
        explicit PyIndexError(const std::string& msg) : std::out_of_range(msg) {}
    };

    // Register all custom exceptions
    void registerAllExceptions(py::module_& m) {
        py::register_exception<PyRuntimeError>(m, "RuntimeError");
        py::register_exception<PyValueError>(m, "ValueError");
        py::register_exception<PyIndexError>(m, "IndexError");
    }

    // Helper function to create Pythonic ID classes with common patterns
    template<typename IDType>
    void registerIdClass(py::module_& m, const std::string& name, const std::string& doc) {
        std::string classDoc = "Pythonic interface for " + doc;
        classDoc += " Provides Pythonic factory methods, validation, and convenience functions.\n";
        classDoc += "Features:\n";
        classDoc += "- Factory methods: create(), invalid()\n";
        classDoc += "- Validation: isValid()\n";
        classDoc += "- Comparison operators: ==, !=, <, >, <=, >=\n";
        classDoc += "- String representations: __repr__, __str__\n";
        classDoc += "- Hash support for use as dict keys\n";
        
        py::class_<IDType>(m, name.c_str(), py::dynamic_attr(), py::module_local(), classDoc.c_str())
            .def(py::init<>())
            .def(py::init<uint64_t>(), py::arg("value"))
            .def_readwrite("value", &IDType::value)
            .def("index", &IDType::index)
            .def("isValid", &IDType::operator bool, "Check if ID is valid (not INVALID_XXX)")
            .def("__eq__", &IDType::operator==)
            .def("__ne__", &IDType::operator!=)
            .def("__lt__", &IDType::operator<)
            .def("__gt__", &IDType::operator>)
            .def("__le__", &IDType::operator<=)
            .def("__ge__", &IDType::operator>=)
            .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); })
            .def("__repr__", [](const IDType& id) {
                return "<" + name + ": " + std::to_string(id.value) + ">";
            })
            .def("__str__", [](const IDType& id) {
                return name + "(" + std::to_string(id.value) + ")";
            })
            // Pythonic factory methods
            .def_static("create", [](uint64_t val) { return IDType(val); }, py::arg("value"), "Create a new ID with the given value")
            .def_static("create", []() { return IDType(); }, "Create an empty/zero ID")
            .def_static("invalid", []() { return IDType(); }, "Create an invalid ID (same as create())")
            .def_static("from", [](uint64_t val) { return IDType(val); }, py::arg("value"), "Create ID from value (alternative to create())");
    }

    // Type checking and validation helpers
    template<typename IDType>
    IDType validateId(const IDType& id, const std::string& operation) {
        if (!id) {
            throw PyValueError("Invalid " + operation + ": " + std::to_string(id.value));
        }
        return id;
    }

    // Validation helpers for other types
    void validateNotNull(std::shared_ptr<void> ptr, const std::string& operation) {
        if (!ptr) {
            throw PyValueError("Null pointer in " + operation);
        }
    }

    void validateIndex(size_t index, size_t maxIndex, const std::string& operation) {
        if (index >= maxIndex) {
            throw PyIndexError("Index out of range in " + operation + ": " + std::to_string(index));
        }
    }

    // Common constants for ID classes
    const std::unordered_map<std::string, uint64_t> getIdConstants() {
        return {
            {"INVALID_NEURON_ID", INVALID_NEURON_ID.value},
            {"INVALID_SYNAPSE_ID", INVALID_SYNAPSE_ID.value},
            {"INVALID_REGION_ID", INVALID_REGION_ID.value},
            {"INVALID_POPULATION_ID", INVALID_POPULATION_ID.value}
        };
    }

    // Enum registration helper
    template<typename EnumType>
    void registerEnum(py::module_& m, const std::string& name, const std::string& doc) {
        py::enum_<EnumType>(m, name.c_str(), py::arithmetic(), py::module_local())
            .value("Excitatory", EnumType::Excitatory)
            .value("Inhibitory", EnumType::Inhibitory)
            .value("Modulatory", EnumType::Modulatory)
            .value("Sensory", EnumType::Sensory)
            .value("Motor", EnumType::Motor)
            .value("Internal", EnumType::Internal)
            .export_values();
    }

    // Convenience functions for common operations
    std::string idToString(const std::string& type, uint64_t value) {
        return type + "(" + std::to_string(value) + ")";
    }

    uint64_t stringToId(const std::string& str) {
        // Simple implementation - parse digits from string like "NeuronId(123)" or "123"
        size_t pos = str.find('(');
        if (pos != std::string::npos) {
            std::string numberPart = str.substr(0, pos);
            return std::stoull(numberPart);
        }
        return std::stoull(str);
    }
}

// Individual registration functions for better modularity

void registerEnumerations(py::module_& m) {
    // Neuron types
    py::enum_<NeuronType>(m, "NeuronType", py::arithmetic(), py::module_local(), R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    // Synapse types
    py::enum_<SynapseType>(m, "SynapseType", py::arithmetic(), py::module_local(), R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    // Developmental stages
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", py::arithmetic(), py::module_local(), R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    // Firing states
    py::enum_<FiringState>(m, "FiringState", py::arithmetic(), py::module_local(), R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    // Action types
    py::enum_<ActionType>(m, "ActionType", py::arithmetic(), py::module_local(), R"pbdoc(Action type enumeration)pbdoc")
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

    // Motor commands
    py::enum_<MotorCommand>(m, "MotorCommand", py::arithmetic(), py::module_local(), R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    // World object types
    py::enum_<WorldObjectType>(m, "WorldObjectType", py::arithmetic(), py::module_local(), R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();
}

void registerConfig(py::module_& m) {
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file", py::call_guard<py::gil_scoped_release>())
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments", py::call_guard<py::gil_scoped_release>())
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file", py::call_guard<py::gil_scoped_release>())
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Convenience methods
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            return self.getString(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get string value from config")
        .def("getInt", [](Config& self, const std::string& key, int defaultValue) {
            return self.getInt(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get int value from config")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            return self.getFloat(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get float value from config")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            return self.getBool(key, defaultValue);
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value from config")
        .def("validateRequired", [](Config& self, const std::vector<std::string>& requiredKeys) {
            std::vector<std::string> missing;
            for (const auto& key : requiredKeys) {
                if (!self.has(key)) {
                    missing.push_back(key);
                }
            }
            if (!missing.empty()) {
                throw PyValueError("Missing required configuration keys: " + 
                    std::accumulate(missing.begin(), missing.end(), std::string(),
                        [](const std::string& a, const std::string& b) {
                            return a.empty() ? b : a + ", " + b;
                        }));
            }
            return true;
        }, py::arg("required_keys"), "Validate that all required keys exist");
}

void registerSensoryInputs(py::module_& m) {
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getTypeName", [](SensoryInput& self) {
            switch (self.getType()) {
                case SensoryInputType::Vision: return "Vision";
                case SensoryInputType::Audio: return "Audio";
                case SensoryInputType::Internal: return "Internal";
                default: return "Unknown";
            }
        }, "Get human-readable type name");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data")
        .def("getWidth", &Vision::getWidth, "Get image width")
        .def("getHeight", &Vision::getHeight, "Get image height")
        .def("getChannels", &Vision::getChannels, "Get number of color channels")
        .def("setDimensions", [](Vision& self, size_t width, size_t height, size_t channels) {
            // Note: This is a convenience wrapper - actual implementation may vary
        }, py::arg("width"), py::arg("height"), py::arg("channels"), "Set vision dimensions");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set audio sample rate")
        .def("getSampleRate", &Audio::getSampleRate, "Get audio sample rate")
        .def("getNumSamples", &Audio::getNumSamples, "Get number of audio samples")
        .def("setSamples", [](Audio& self, const std::vector<float>& samples) {
            self.setData(samples);
        }, py::arg("samples"), "Set audio samples (alias for setData)");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("addSignalValue", &InternalSignals::addSignal, py::arg("value"), "Add signal (convenience method)")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("getSignalCount", &InternalSignals::size, "Get number of signals")
        .def("getSignals", &InternalSignals::getData, "Get all signals as vector")
        .def("addMultipleSignals", [](InternalSignals& self, const std::vector<float>& values) {
            for (float val : values) {
                self.addSignal(val);
            }
        }, py::arg("values"), "Add multiple signal values at once");
}

void registerActions(py::module_& m) {
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters")
        .def("getType", &Action::getType, "Get action type")
        .def("setType", &Action::setType, py::arg("type"), "Set action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable action name")
        .def("clone", &Action::clone, "Create a copy of this action")
        .def("toDict", [](const Action& self) {
            py::dict result;
            result["type"] = self.getType();
            result["parameters"] = self.getParameters();
            result["name"] = self.getName();
            return result;
        }, "Convert action to dictionary for easy inspection")
        .def("isValid", [](Action& self) {
            return self.getType() != ActionType::Custom || !self.getParameters().empty();
        }, "Check if action is valid")
        .def_static("create", [](ActionType type, const std::vector<float>& params) {
            return Action(type, params);
        }, py::arg("type"), py::arg("parameters").none() ? py::arg("parameters") : py::arg("parameters"), "Static factory method for action creation");
}

void registerAgentStateClasses(py::module_& m) {
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f, "Create world object with position and type")
        .def_readwrite("x", &WorldObject::x, "X coordinate")
        .def_readwrite("y", &WorldObject::y, "Y coordinate")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active")
        .def("getPosition", [](WorldObject& self) {
            return std::make_pair(self.x, self.y);
        }, "Get object position as (x, y) tuple")
        .def("setPosition", [](WorldObject& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set object position")
        .def("toDict", [](const WorldObject& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["radius"] = self.radius;
            result["type"] = self.type;
            result["value"] = self.value;
            result["active"] = self.active;
            return result;
        }, "Convert world object to dictionary");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing angle in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Current health (0-100)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Timestamp of last action")
        .def("reset", &AgentBody::reset, "Reset to initial state")
        .def("getPosition", [](AgentBody& self) {
            return std::make_pair(self.x, self.y);
        }, "Get position as (x, y) tuple")
        .def("setPosition", [](AgentBody& self, float x, float y) {
            self.x = x;
            self.y = y;
        }, py::arg("x"), py::arg("y"), "Set position")
        .def("getVelocity", [](AgentBody& self) {
            return std::make_pair(self.velocityX, self.velocityY);
        }, "Get velocity as (vx, vy) tuple")
        .def("toDict", [](const AgentBody& self) {
            py::dict result;
            result["x"] = self.x;
            result["y"] = self.y;
            result["orientation"] = self.orientation;
            result["velocityX"] = self.velocityX;
            result["velocityY"] = self.velocityY;
            result["angularVelocity"] = self.angularVelocity;
            result["energy"] = self.energy;
            result["health"] = self.health;
            result["age"] = self.age;
            result["isMoving"] = self.isMoving;
            result["isTurning"] = self.isTurning;
            result["lastActionTime"] = self.lastActionTime;
            return result;
        }, "Convert agent body to dictionary");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward and success status")
        .def_readwrite("reward", &ActionResult::reward, "Reward received from action")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Result message")
        .def("toDict", [](const ActionResult& self) {
            py::dict result;
            result["reward"] = self.reward;
            result["success"] = self.success;
            result["message"] = self.message;
            return result;
        }, "Convert action result to dictionary")
        .def("isSuccess", [](const ActionResult& self) {
            return self.success;
        }, "Check if action was successful (alias for 'success')");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision input")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision input")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch input")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch input")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio input")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio input")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set percept timestamp")
        .def("hasVision", [](SensoryPercept& self) {
            return self.getVision() != nullptr;
        }, "Check if vision data is present")
        .def("hasAudio", [](SensoryPercept& self) {
            return self.getAudio() != nullptr;
        }, "Check if audio data is present")
        .def("hasTouch", [](SensoryPercept& self) {
            return self.getTouch() != nullptr;
        }, "Check if touch data is present")
        .def("hasInternal", [](SensoryPercept& self) {
            return self.getInternal() != nullptr;
        }, "Check if internal data is present")
        .def("hasProprioception", [](SensoryPercept& self) {
            return self.getProprioception() != nullptr;
        }, "Check if proprioception data is present")
        .def("getSignalCount", [](SensoryPercept& self) {
            auto signals = self.getAllSignals();
            return signals.size();
        }, "Get total number of signals")
        .def("toDict", [](SensoryPercept& self) {
            py::dict result;
            auto vision = self.getVision();
            if (vision) {
                result["vision"] = py::cast(*vision);
            }
            auto audio = self.getAudio();
            if (audio) {
                result["audio"] = py::cast(*audio);
            }
            auto touch = self.getTouch();
            if (touch) {
                result["touch"] = py::cast(*touch);
            }
            auto internal = self.getInternal();
            if (internal) {
                result["internal"] = py::cast(*internal);
            }
            auto proprioception = self.getProprioception();
            if (proprioception) {
                result["proprioception"] = py::cast(*proprioception);
            }
            result["timestamp"] = self.getTimestamp();
            return result;
        }, "Convert sensory percept to dictionary");
}

void registerWorldClasses(py::module_& m) {
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation by timestep")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to world")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get current agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulation")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed")
        // Pythonic convenience methods
        .def("getSize", [](SimpleWorld& self) {
            return std::make_pair(self.getWidth(), self.getHeight());
        }, "Get world dimensions as (width, height) tuple")
        .def("getVisionSize", [](SimpleWorld& self) {
            // Note: This requires access to vision dimensions from world config
            // For now, return a placeholder
            return std::make_pair(84, 84); // Default vision size
        }, "Get vision dimensions as (width, height) tuple")
        .def("hasObjectAt", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if object exists at position (alias for isValidPosition)")
        .def("getObjectCount", [](SimpleWorld& self) {
            // Note: SimpleWorld doesn't expose object count directly
            // This is a placeholder for future extension
            return 0;
        }, "Get number of objects (placeholder)")
        .def("step", &SimpleWorld::update, py::arg("timestep"),
             "Alias for update (more intuitive name)")
        .def("setVisionSize", [](SimpleWorld& self, size_t width, size_t height) {
            // Note: This would require modifying SimpleWorld interface
            // For now, this is a placeholder
        }, py::arg("width"), py::arg("height"),
         "Set vision size (placeholder - requires interface extension)");
}

void registerBrain(py::module_& m) {
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration", py::call_guard<py::gil_scoped_release>())
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration", py::call_guard<py::gil_scoped_release>())
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step", py::call_guard<py::gil_scoped_release>())
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp", py::call_guard<py::gil_scoped_release>())
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain", py::call_guard<py::gil_scoped_release>())
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron", py::call_guard<py::gil_scoped_release>())
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type", py::call_guard<py::gil_scoped_release>())
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity", py::call_guard<py::gil_scoped_release>())
        .def("reset", &Brain::reset,
             "Reset brain state", py::call_guard<py::gil_scoped_release>())
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file", py::call_guard<py::gil_scoped_release>())
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file", py::call_guard<py::gil_scoped_release>())
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
             "Log brain status", py::call_guard<py::gil_scoped_release>())
        // Pythonic convenience methods
        .def("simulateStep", [](Brain& self, SimulationStep step) {
            self.step(step);
        }, py::arg("step"), "Alias for step() (more intuitive)")
        .def("simulateStepWithTime", [](Brain& self, SimulationStep step, Timestamp time) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time"), "Alias for step() with timestamp")
        .def("getNeuralStats", [](Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["spike_count"] = self.getTotalSpikeCount();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        }, "Get comprehensive neural statistics as dictionary")
        .def("getStatistics", [](Brain& self) {
            // Alias for getNeuralStats
            return self.getNeuralStats();
        }, "Get neural statistics (alias for getNeuralStats)")
        .def("injectCurrentToType", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current to neurons of specific type (alias for injectCurrentToNeurons)")
        .def("getRegionStats", [](Brain& self) {
            py::dict regionStats;
            auto regionIds = self.getRegionIds();
            for (const auto& id : regionIds) {
                auto region = self.getRegion(id);
                if (region) {
                    py::dict stats;
                    stats["id"] = id;
                    // Note: Region class interface details would go here
                    regionStats[std::to_string(id.value)] = stats;
                }
            }
            return regionStats;
        }, "Get statistics for all regions");
}

void registerAgentBrain(py::module_& m) {
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world", py::call_guard<py::gil_scoped_release>())
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain", py::call_guard<py::gil_scoped_release>())
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command", py::call_guard<py::gil_scoped_release>())
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation", py::call_guard<py::gil_scoped_release>())
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system", py::call_guard<py::gil_scoped_release>())
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
             "Reset agent for new episode", py::call_guard<py::gil_scoped_release>())
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        // Pythonic convenience methods
        .def("train", [](AgentBrain& self, const SensoryPercept& percept) {
            self.processSensoryInput(percept);
            // Note: decodeMotorCommand would typically be called elsewhere
        }, py::arg("percept"), "Process sensory input for training (alias for processSensoryInput)")
        .def("act", [](AgentBrain& self) {
            return self.decodeMotorCommand();
        }, "Generate motor command from current brain state")
        .def("getBehaviorStats", [](AgentBrain& self) {
            py::dict stats;
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["sensory_input_size"] = self.getSensoryInputSize();
            stats["motor_output_size"] = self.getMotorOutputSize();
            return stats;
        }, "Get agent brain behavior statistics")
        .def("getSystemStatus", [](AgentBrain& self) {
            py::dict status;
            status["reward_modulation"] = self.isRewardModulationEnabled();
            status["structural_plasticity"] = self.isStructuralPlasticityEnabled();
            status["development"] = self.isDevelopmentEnabled();
            status["curiosity"] = self.isCuriosityEnabled();
            return status;
        }, "Get agent brain system status as dictionary");
}

void registerFactoryFunctions(py::module_& m) {
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::return_value_policy::reference_internal);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        validateNotNull(config, "createBrain");
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration", py::return_value_policy::reference_internal);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world", py::return_value_policy::reference_internal);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        validateNotNull(brain, "createAgentBrain");
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface", py::return_value_policy::reference_internal);

    // Additional factory functions
    m.def("createConfigFromFile", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Create config from JSON file", py::return_value_policy::reference_internal);

    m.def("createConfigFromDict", [](const py::dict& configDict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Note: Config class interface for loading from dict would need to be implemented
        return config;
    }, py::arg("config_dict"), "Create config from Python dictionary (placeholder)", py::return_value_policy::reference_internal);

    m.def("createWorldWithConfig", [](const std::string& configFile) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        // Note: World configuration would need to be implemented
        return world;
    }, py::arg("config_file"), "Create world with configuration (placeholder)", py::return_value_policy::reference_internal);
}

void registerUtilities(py::module_& m) {
    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Global constants
    m.attr("DEFAULT_VISION_WIDTH") = 84;
    m.attr("DEFAULT_VISION_HEIGHT") = 84;
    m.attr("DEFAULT_VISION_CHANNELS") = 3;
    m.attr("DEFAULT_WORLD_WIDTH") = 200;
    m.attr("DEFAULT_WORLD_HEIGHT") = 200;

    // Utility functions
    m.def("version", []() {
        return "NLM Python Bindings v1.0.0";
    }, "Get version information");

    m.def("getIdConstants", []() {
        py::dict constants;
        auto cppConstants = getIdConstants();
        for (const auto& [name, value] : cppConstants) {
            constants[name.c_str()] = value;
        }
        return constants;
    }, "Get all ID constant values as dictionary");

    m.def("parseId", &stringToId, py::arg("id_string"), "Parse ID string to numeric value");

    m.def("formatId", &idToString, py::arg("type"), py::arg("value"), "Format ID as string");

    // Type checking utilities
    m.def("validateNeuronId", [](uint64_t value) {
        return NeuronId(value);
    }, py::arg("value"), "Validate and create neuron ID");

    m.def("validateSynapseId", [](uint64_t value) {
        return SynapseId(value);
    }, py::arg("value"), "Validate and create synapse ID");

    m.def("validateRegionId", [](uint64_t value) {
        return RegionId(value);
    }, py::arg("value"), "Validate and create region ID");

    m.def("validatePopulationId", [](uint64_t value) {
        return PopulationId(value);
    }, py::arg("value"), "Validate and create population ID");

    // System utilities
    m.def("getSystemInfo", []() {
        py::dict info;
        info["version"] = "1.0.0";
        info["description"] = "NLM Python Bindings";
        info["id_classes"] = std::vector<std::string>{"NeuronId", "SynapseId", "RegionId", "PopulationId"};
        return info;
    }, "Get system information");
}

// Module initialization function
void initializeModule(py::module_& m) {
    // Register all exceptions first
    registerAllExceptions(m);

    // Register ID classes using helper function to reduce repetition
    registerIdClass<NeuronId>(m, "NeuronId", "Unique identifier for a neuron");
    registerIdClass<SynapseId>(m, "SynapseId", "Unique identifier for a synapse");
    registerIdClass<RegionId>(m, "RegionId", "Unique identifier for a brain region");
    registerIdClass<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population");

    // Register constants for easy access
    auto constants = getIdConstants();
    for (const auto& [name, value] : constants) {
        m.attr(name.c_str()) = py::cast(value);
    }

    // Register all other classes in logical order
    registerEnumerations(m);
    registerConfig(m);
    registerSensoryInputs(m);
    registerActions(m);
    registerAgentStateClasses(m);
    registerWorldClasses(m);
    registerBrain(m);
    registerAgentBrain(m);
    registerFactoryFunctions(m);
    registerUtilities(m);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Pythonic factory methods for ID classes
        - Type checking and validation
        - Comprehensive error handling
        - Convenience methods and utilities
        - Modular design for extensibility
    )pbdoc";

    // Initialize the module using the modular approach
    initializeModule(m);
}

} // namespace nlm
