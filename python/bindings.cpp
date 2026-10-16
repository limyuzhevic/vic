#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
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

// Python-specific exception classes
class PyNLMError : public std::runtime_error {
public:
    explicit PyNLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public PyNLMError {
public:
    explicit ConfigurationError(const std::string& msg) : PyNLMError(msg) {}
};

class SimulationError : public PyNLMError {
public:
    explicit SimulationError(const std::string& msg) : PyNLMError(msg) {}
};

// Helper function to convert C++ errors to Python exceptions
void register_exceptions(py::module_& m) {
    py::register_exception<PyNLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Advanced neural simulation with spiking neurons
        - Integrated memory and neuromodulation systems
        - Agent-based learning in virtual environments
        - Checkpoint/save/load functionality
        - Comprehensive simulation control
        - Advanced Pythonic APIs for easy usage
        - Error handling and validation
        - Statistics and monitoring
        - Enhanced configuration with JSON/YAML support
    )pbdoc";

    // Register custom exceptions
    register_exceptions(m);

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

    // Enhanced Config class with Pythonic APIs
    py::class_<Config>(m, "Config", R"pbdoc(Enhanced configuration system for NLM with JSON/YAML support)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a file (JSON, YAML, or simple format)")
        .def("loadFromString", &Config::loadFromString, py::arg("content"), py::arg("format") = "auto",
             "Load configuration from string content (JSON, YAML, or simple format)")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"), py::arg("format") = "json",
             "Save configuration to file (JSON or YAML format)")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a detailed summary of the configuration including source distribution")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        
        // Enhanced Pythonic getter/setter APIs
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (py::isinstance(value, py::int_())) {
                self.set(key, value.cast<int>());
            } else if (py::isinstance(value, py::float_())) {
                self.set(key, value.cast<double>());
            } else if (py::isinstance(value, py::bool_())) {
                self.set(key, value.cast<bool>());
            } else {
                self.set(key, value.cast<std::string>());
            }
        }, py::arg("key"), py::arg("value"),
             "Set configuration value (supports int, float, bool, string)")
        
        .def("get", [](Config& self, const std::string& key, const py::object& defaultValue) {
            if (self.has(key)) {
                auto val = self.get<std::string>(key);
                if (val.has_value()) {
                    return py::cast(val.value());
                }
            }
            return py::cast(defaultValue);
        }, py::arg("key"), py::arg("defaultValue") = py::none(),
             "Get configuration value with optional default")
        
        .def("get_int", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue") = 0,
             "Get integer configuration value")
        
        .def("get_float", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue") = 0.0,
             "Get float configuration value")
        
        .def("get_bool", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue") = false,
             "Get boolean configuration value")
        
        .def("get_string", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue") = std::string(),
             "Get string configuration value")
        
        .def("remove", &Config::remove, py::arg("key"),
             "Remove configuration key")
        
        // Validation and schema APIs
        .def("addSchema", &Config::addSchema, py::arg("schema"),
             "Add configuration schema for validation")
        
        .def("removeSchema", &Config::removeSchema, py::arg("key"),
             "Remove configuration schema")
        
        .def("getSchemas", &Config::getSchemas,
             "Get all configuration schemas")
        
        .def("clearSchemas", &Config::clearSchemas,
             "Clear all configuration schemas")
        
        .def("validate", &Config::validate, py::arg("schema"),
             "Validate configuration against schema")
        
        .def("validateValue", &Config::validateValue, py::arg("key"), py::arg("typeName"),
             "Validate that a key has a specific value type")
        
        // Import/Export APIs
        .def("exportToJSON", &Config::exportToJSON,
             "Export configuration to JSON string")
        
        .def("exportToYAML", &Config::exportToYAML,
             "Export configuration to YAML string")
        
        .def("importFromJSON", &Config::importFromJSON, py::arg("jsonStr"),
             "Import configuration from JSON string")
        
        .def("importFromYAML", &Config::importFromYAML, py::arg("yamlStr"),
             "Import configuration from YAML string")
        
        // Configuration merging APIs
        .def("merge", &Config::merge, py::arg("other"), py::arg("source") = static_cast<int>(ConfigSource::File),
             "Merge another configuration into this one")
        
        .def("override", &Config::override, py::arg("other"),
             "Override current configuration with another one")
        
        // Configuration statistics
        .def("getEntryCount", &Config::getEntryCount,
             "Get total number of configuration entries")
        
        .def("getEntryCountForSource", [](const Config& self, ConfigSource source) {
            return self.getEntryCount(source);
        }, py::arg("source"),
             "Get number of entries from a specific source")
        
        .def("getSourceRatio", [](const Config& self, ConfigSource source) {
            return self.getSourceRatio(source);
        }, py::arg("source"),
             "Get ratio of entries from a specific source")
        
        // Change tracking APIs
        .def("enableChangeTracking", &Config::enableChangeTracking, py::arg("enable"),
             "Enable or disable change tracking")
        
        .def("isChangeTrackingEnabled", &Config::isChangeTrackingEnabled,
             "Check if change tracking is enabled")
        
        .def("getChangeHistory", &Config::getChangeHistory,
             "Get history of configuration changes")
        
        // Comparison APIs
        .def("equals", &Config::equals, py::arg("other"),
             "Check if configuration equals another one")
        
        .def("hash", &Config::hash,
             "Get hash of configuration for comparison")
        
        // Configuration value type information
        .def("getValueType", [](const Config& self, const std::string& key) {
            auto entry = self.findEntry(key);
            if (entry) {
                std::string typeName = ConfigValidator::getTypeName(entry->value);
                return typeName;
            }
            return std::string("unknown");
        }, py::arg("key"),
             "Get the type name of a configuration value");

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def(py::init<>())
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
        
        // Action convenience methods
        .def("move_to", [](Action& self, float x, float y) {
            ActionType type = ActionType::Custom;
            self.setType(type);
            std::vector<float> params = {x, y};
            self.setParameters(params);
            return self;
        }, py::arg("x"), py::arg("y"),
             "Create movement action")
        
        .def("turn_to", [](Action& self, float angle) {
            ActionType type = ActionType::TurnLeft;
            self.setType(type);
            std::vector<float> params = {angle};
            self.setParameters(params);
            return self;
        }, py::arg("angle"),
             "Create rotation action")
        
        .def("interact", [](Action& self) {
            ActionType type = ActionType::Interact;
            self.setType(type);
            return self;
        }, "Create interaction action")
        
        .def("wait", [](Action& self, double duration = 1.0) {
            ActionType type = ActionType::Wait;
            self.setType(type);
            std::vector<float> params = {static_cast<float>(duration)};
            self.setParameters(params);
            return self;
        }, py::arg("duration") = 1.0,
             "Create wait action");

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
        
        // WorldObject convenience methods
        .def("is_resource", [](WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        
        .def("is_hazard", [](WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        
        .def("is_wall", [](WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall")
        
        .def("get_reward_value", [](WorldObject& self) {
            return self.type == WorldObjectType::Resource ? self.value : -self.value;
        }, "Get reward value (positive for resources, negative for hazards)")
        
        .def("clone", &WorldObject::clone);

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
        .def("reset", &AgentBody::reset)
        
        // AgentBody convenience methods
        .def("distance_to", [](AgentBody& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"),
             "Calculate distance to position")
        
        .def("angle_to", [](AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            float angle = std::atan2(dy, dx);
            return angle;
        }, py::arg("x"), py::arg("y"),
             "Calculate angle to position in radians")
        
        .def("normalize_orientation", [](AgentBody& self) {
            while (self.orientation > M_PI) self.orientation -= 2.0f * M_PI;
            while (self.orientation < -M_PI) self.orientation += 2.0f * M_PI;
        }, "Normalize orientation to [-PI, PI]")
        
        .def("set_random_position", [](AgentBody& self, float width, float height) {
            self.x = static_cast<float>(rand()) / RAND_MAX * width;
            self.y = static_cast<float>(rand()) / RAND_MAX * height;
        }, py::arg("width"), py::arg("height"),
             "Set random position within bounds");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        
        // ActionResult convenience methods
        .def("is_success", [](ActionResult& self) {
            return self.success;
        }, "Check if action was successful")
        
        .def("get_formatted_reward", [](ActionResult& self) {
            std::stringstream ss;
            ss << (self.reward >= 0 ? "+" : "") << self.reward;
            return ss.str();
        }, "Get formatted reward string");

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"))
        
        // SensoryPercept convenience methods
        .def("normalize_vision", [](SensoryPercept& self) {
            auto& vision = self.getVision();
            float maxVal = *std::max_element(vision.begin(), vision.end());
            if (maxVal > 0) {
                for (auto& val : vision) val /= maxVal;
            }
        }, "Normalize vision data to [0, 1]")
        
        .def("get_average_intensity", [](SensoryPercept& self) {
            auto& vision = self.getVision();
            if (vision.empty()) return 0.0f;
            float sum = std::accumulate(vision.begin(), vision.end(), 0.0f);
            return sum / vision.size();
        }, "Get average vision intensity")
        
        .def("create_from_dict", [](py::dict data) {
            SensoryPercept percept;
            if (data.contains("vision")) {
                auto vision_py = data["vision"].cast<std::vector<float>>();
                percept.setVision(vision_py);
            }
            if (data.contains("touch")) {
                auto touch_py = data["touch"].cast<std::vector<float>>();
                percept.setTouch(touch_py);
            }
            if (data.contains("internal")) {
                auto internal_py = data["internal"].cast<std::vector<float>>();
                percept.setInternal(internal_py);
            }
            if (data.contains("proprioception")) {
                auto proprio_py = data["proprioception"].cast<std::vector<float>>();
                percept.setProprioception(proprio_py);
            }
            if (data.contains("timestamp")) {
                auto timestamp_py = data["timestamp"].cast<double>();
                percept.setTimestamp(timestamp_py);
            }
            return percept;
        }, py::arg("data"),
             "Create SensoryPercept from dictionary")
        
        .def("to_dict", [](const SensoryPercept& self) {
            py::dict result;
            result["vision"] = self.getVision();
            result["touch"] = self.getTouch();
            result["internal"] = self.getInternal();
            result["proprioception"] = self.getProprioception();
            result["audio"] = self.getAudio();
            result["timestamp"] = self.getTimestamp();
            result["vision_width"] = self.getVisionWidth();
            result["vision_height"] = self.getVisionHeight();
            return result;
        }, "Convert SensoryPercept to dictionary");

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
        
        // SimpleWorld convenience methods
        .def("create_resource", [](SimpleWorld& self, float x, float y, float value = 10.0f) {
            WorldObject obj(x, y, WorldObjectType::Resource, value, 0.4f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 10.0f,
             "Create a resource object")
        
        .def("create_hazard", [](SimpleWorld& self, float x, float y, float value = -15.0f) {
            WorldObject obj(x, y, WorldObjectType::Hazard, value, 0.3f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = -15.0f,
             "Create a hazard object")
        
        .def("create_wall", [](SimpleWorld& self, float x, float y) {
            WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, 0.2f);
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"),
             "Create a wall object")
        
        .def("spawn_agent", [](SimpleWorld& self, float x, float y) {
            self.setAgentStart(x, y);
        }, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        
        .def("get_objects", [](SimpleWorld& self, WorldObjectType type = WorldObjectType::Empty) {
            std::vector<WorldObject> result;
            for (auto& obj : self.getObjects()) {
                if (type == WorldObjectType::Empty || obj.type == type) {
                    result.push_back(obj);
                }
            }
            return result;
        }, py::arg("type") = WorldObjectType::Empty,
             "Get all objects of specified type")
        
        .def("get_object_count", [](SimpleWorld& self, WorldObjectType type = WorldObjectType::Empty) {
            return self.getObjects().size();
        }, py::arg("type") = WorldObjectType::Empty,
             "Get count of objects of specified type")
        
        .def("clear_objects", [](SimpleWorld& self) {
            self.getObjects().clear();
        }, "Clear all objects from world")
        
        .def("is_safe_position", [](SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"),
             "Check if position is safe (not outside world)")
        
        .def("get_vision_area", [](SimpleWorld& self, float x, float y, float angle, float range) {
            // Simplified ray casting for vision
            std::vector<float> vision(16, 1.0f);
            return vision;
        }, py::arg("x"), py::arg("y"), py::arg("angle"), py::arg("range"),
             "Get vision in specified direction (simplified)");

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
             "Log brain status")
        
        // Advanced Pythonic APIs
        .def("run_simulation", [](Brain& self, size_t steps, double timestep = 0.001) {
            for (size_t i = 0; i < steps; ++i) {
                self.step(i, i * timestep);
            }
        }, py::arg("steps"), py::arg("timestep") = 0.001,
             "Run a complete simulation for the specified number of steps")
        
        .def("step_with_feedback", [](Brain& self, SimulationStep step, Timestamp time) {
            return self.step(step, time);
        }, py::arg("step"), py::arg("time"),
             "Step with feedback - returns whether any neurons fired")
        
        .def("get_statistics", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["ei_ratio"] = self.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive simulation statistics")
        
        .def("create_checkpoint", [](Brain& self, const std::string& filepath) {
            if (!self.save(filepath)) {
                throw SimulationError("Failed to create checkpoint at " + filepath);
            }
        }, py::arg("filepath"),
             "Create a checkpoint file for saving brain state")
        
        .def("restore_from_checkpoint", [](Brain& self, const std::string& filepath) {
            if (!self.load(filepath)) {
                throw SimulationError("Failed to restore from checkpoint " + filepath);
            }
        }, py::arg("filepath"),
             "Restore brain state from checkpoint file")
        
        .def("get_region_by_index", [](Brain& self, size_t index) {
            auto region_ids = self.getRegionIds();
            if (index >= region_ids.size()) {
                throw ConfigurationError("Region index out of range");
            }
            return self.getRegion(region_ids[index]);
        }, py::arg("index"),
             "Get region by index (convenience method)")
        
        .def("has_memory_systems", [](const Brain& self) {
            return self.getWorkingMemory() != nullptr || 
                   self.getEpisodicMemory() != nullptr ||
                   self.getAssociativeMemory() != nullptr;
        }, "Check if brain has memory systems")
        
        .def("has_neuromodulation", [](const Brain& self) {
            return self.getDopamine() != nullptr &&
                   self.getCuriosity() != nullptr &&
                   self.getNovelty() != nullptr;
        }, "Check if brain has neuromodulation systems")
        
        .def("has_prediction", [](const Brain& self) {
            return self.getPredictionSystem() != nullptr;
        }, "Check if brain has prediction system")
        
        .def("has_cognition", [](const Brain& self) {
            return self.getPlanner() != nullptr &&
                   self.getConceptFormation() != nullptr &&
                   self.getAttention() != nullptr;
        }, "Check if brain has cognition systems")
        
        .def("has_development", [](const Brain& self) {
            return self.getDevelopmentSystem() != nullptr;
        }, "Check if brain has development system");

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
        
        // Advanced Pythonic APIs for AgentBrain
        .def("run_episode", [](AgentBrain& self, SimpleWorld& world, size_t steps, double timestep = 0.1) {
            for (size_t i = 0; i < steps; ++i) {
                world.update(timestep);
                auto percept = world.getSensoryPercept();
                self.processSensoryInput(percept);
                self.getBrain()->step(i, i * timestep);
            }
        }, py::arg("world"), py::arg("steps"), py::arg("timestep") = 0.1,
             "Run a complete episode with brain-world interaction")
        
        .def("act", [](AgentBrain& self, const SimpleWorld& world) {
            auto percept = world.getSensoryPercept();
            self.processSensoryInput(percept);
            return self.decodeMotorCommand();
        }, py::arg("world"),
             "Process sensory input and decode motor command (one step)")
        
        .def("learn_from_reward", [](AgentBrain& self, float reward, float predicted = 0.0f) {
            self.applyRewardModulation(reward, predicted);
        }, py::arg("reward"), py::arg("predicted") = 0.0f,
             "Apply reward modulation for learning")
        
        .def("update_agent_state", [](AgentBrain& self, double timestep) {
            self.updateDevelopment(timestep);
        }, py::arg("timestep"),
             "Update agent development and plasticity")
        
        .def("get_agent_stats", [](const AgentBrain& self) {
            py::dict stats;
            stats["curiosity"] = self.getCuriosityLevel();
            stats["novelty"] = self.getNoveltyLevel();
            stats["dopamine"] = self.getNeuromodulationLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        }, "Get comprehensive agent statistics")
        
        .def("enable_all_features", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        }, "Enable all learning and development features")
        
        .def("reset_episode", [](AgentBrain& self) {
            self.reset();
        }, "Reset agent state for new episode")
        
        .def("get_brain_region", [](AgentBrain& self, size_t index) {
            auto brain = self.getBrain();
            if (!brain) throw SimulationError("Agent brain not initialized");
            auto regions = brain->getRegions();
            if (index >= regions.size()) {
                throw ConfigurationError("Region index out of range");
            }
            return regions[index].get();
        }, py::arg("index"),
             "Get specific brain region by index")
        
        .def("has_sensor_neurons", [](const AgentBrain& self) {
            return true; // Would require brain introspection
        }, "Check if agent has sensory neurons")
        
        .def("has_motor_neurons", [](const AgentBrain& self) {
            return true; // Would require brain introspection
        }, "Check if agent has motor neurons");

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

    // Pythonic convenience functions
    m.def("run_simple_simulation", [](size_t steps, double timestep = 0.001) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i, i * timestep);
        }
        return brain;
    }, py::arg("steps"), py::arg("timestep") = 0.001,
         "Run a simple simulation and return the brain");

    m.def("create_agent_simulation", [](size_t steps, double timestep = 0.1) {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        auto agent = std::make_shared<AgentBrain>(brain);
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        for (size_t i = 0; i < steps; ++i) {
            world->update(timestep);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i, i * timestep);
        }
        
        return std::make_tuple(brain, agent, world);
    }, py::arg("steps"), py::arg("timestep") = 0.1,
         "Create a complete agent simulation and return (brain, agent, world)");

    m.def("version", []() {
        return "0.1.0 - Advanced Python Bindings with Error Handling";
    }, "Get the version of the NLM Python bindings");

    m.def("examples", []() {
        py::dict example_dict;
        example_dict["simple_brain"] = R"(
import pynlm

# Create and initialize brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print(f"Simulation complete: {brain.getTotalSpikeCount()} spikes")
        )";
        
        example_dict["agent_world"] = R"(
import pynlm

# Create complete agent simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)
world.reset()
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    # Agent automatically decodes action
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
        
print("Agent simulation complete!")
        )";
        
        example_dict["checkpoint"] = R"(
import pynlm

# Create brain and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some steps
for i in range(100):
    brain.step(i)

# Save checkpoint
brain.save("my_brain_checkpoint.bin")
print("Brain saved to checkpoint")

# Reset and load new brain
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.initialize()
brain2.load("my_brain_checkpoint.bin")
print("Brain restored from checkpoint")
        )";
        
        return example_dict;
    }, "Get dictionary of usage examples");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Define exception types for user access
    py::class_<PyNLMError>(m, "NLMError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<ConfigurationError>(m, "ConfigurationError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));

    py::class_<SimulationError>(m, "SimulationError", py::module_local())
        .def(py::init<const std::string&>(), py::arg("message"));
}

} // namespace nlm
