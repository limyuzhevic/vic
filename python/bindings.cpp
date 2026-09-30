#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <limits>

// Include all the core headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"

namespace py = pybind11;
namespace nlm {

// Custom exception types for Python
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class WorldError : public std::runtime_error {
public:
    explicit WorldError(const std::string& msg) : std::runtime_error(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key improvements:
        - Smart pointer APIs for better memory management
        - Plasticity configuration and neuron parameter access
        - Memory system limits and constraints
        - Enhanced error handling with descriptive exceptions
        - Pythonic properties and method names
        - Comprehensive configuration validation
    )pbdoc";

    // Register custom exceptions
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");

    // Basic type definitions (already present, kept for consistency)
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

    // ========== ENHANCED CONFIG CLASS ==========
    py::class_<Config>(m, "Config", R"pbdoc(Comprehensive configuration class for NLM system)pbdoc")
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
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            throw ConfigError("Configuration key not found: " + key);
        }, py::arg("key"),
             "Get a string configuration value (raises ConfigError if not found)")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (value) return *value;
            throw ConfigError("Configuration key not found: " + key);
        }, py::arg("key"),
             "Get an integer configuration value (raises ConfigError if not found)")
        .def("getDouble", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (value) return *value;
            throw ConfigError("Configuration key not found: " + key);
        }, py::arg("key"),
             "Get a double configuration value (raises ConfigError if not found)")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            throw ConfigError("Configuration key not found: " + key);
        }, py::arg("key"),
             "Get a boolean configuration value (raises ConfigError if not found)")
        .def("getIntOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get integer value or default if not found")
        .def("getDoubleOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get double value or default if not found")
        .def("getBoolOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get boolean value or default if not found")
        .def("getStringOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get string value or default if not found")
        .def("set", &Config::set, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
             "Set a configuration value with source")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a string configuration value")
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set an integer configuration value")
        .def("setDouble", [](Config& self, const std::string& key, double value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a double configuration value")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime,
           "Set a boolean configuration value")
        .def("validate", [](Config& self) {
            // Basic validation: check for required keys
            std::vector<std::string> missingKeys;
            
            // Check for required configuration sections
            if (!self.has("brain.regions.count") || self.getOr<int>("brain.regions.count", 0) <= 0) {
                missingKeys.push_back("brain.regions.count");
            }
            if (!self.has("plasticity.enabled") || !self.getOr<bool>("plasticity.enabled", false)) {
                missingKeys.push_back("plasticity.enabled");
            }
            if (!self.has("memory.working_memory.max_size") || self.getOr<int>("memory.working_memory.max_size", 0) <= 0) {
                missingKeys.push_back("memory.working_memory.max_size");
            }
            if (!self.has("world.width") || self.getOr<double>("world.width", 0.0) <= 0.0) {
                missingKeys.push_back("world.width");
            }
            if (!self.has("world.height") || self.getOr<double>("world.height", 0.0) <= 0.0) {
                missingKeys.push_back("world.height");
            }
            
            if (!missingKeys.empty()) {
                throw ConfigError("Configuration validation failed. Missing required keys: " + 
                                 std::accumulate(missingKeys.begin() + 1, missingKeys.end(), missingKeys[0],
                                 [](const std::string& a, const std::string& b) { return a + ", " + b; }));
            }
            
            return true;
        }, "Validate configuration (raises ConfigError if invalid)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Sensory Input Classes
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

    // Action Class
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

    // World Object Class
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
        .def("toDict", [](const WorldObject& obj) {
            py::dict result;
            result["x"] = obj.x;
            result["y"] = obj.y;
            result["radius"] = obj.radius;
            result["type"] = static_cast<int>(obj.type);
            result["value"] = obj.value;
            result["active"] = obj.active;
            return result;
        }, "Convert world object to Python dictionary");

    // Agent Body Class
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
        .def("distanceTo", [](const AgentBody& self, float x, float y) {
            float dx = x - self.x;
            float dy = y - self.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance to position (x, y)");

    // Action Result Class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Sensory Percept Class
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
        .def("toDict", [](const SensoryPercept& percept) {
            py::dict result;
            auto vision = percept.getVision();
            if (vision) result["vision"] = *vision;
            auto touch = percept.getTouch();
            if (touch) result["touch"] = *touch;
            auto internal = percept.getInternal();
            if (internal) result["internal"] = *internal;
            auto proprioception = percept.getProprioception();
            if (proprioception) result["proprioception"] = *proprioception;
            auto audio = percept.getAudio();
            if (audio) result["audio"] = *audio;
            result["timestamp"] = percept.getTimestamp();
            return result;
        }, "Convert sensory percept to Python dictionary");

    // ========== ENHANCED SIMPLE WORLD ==========
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Advanced 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and vision parameters")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal,
             "Get object at position (if any)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum agent energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate")
        .def("getResourceEnergyGain", &SimpleWorld::getResourceEnergyGain, "Get energy gain from resources")
        .def("setResourceEnergyGain", &SimpleWorld::setResourceEnergyGain, py::arg("gain"),
             "Set energy gain from resources")
        .def("getHazardDamage", &SimpleWorld::getHazardDamage, "Get damage from hazards")
        .def("setHazardDamage", &SimpleWorld::setHazardDamage, py::arg("damage"),
             "Set damage from hazards")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("getSimulationTimeStep", &SimpleWorld::getSimulationTimeStep, "Get time step size")
        .def("setSimulationTimeStep", &SimpleWorld::setSimulationTimeStep, py::arg("dt"),
             "Set time step size")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed")
        .def("getObjectCount", &SimpleWorld::getObjectCount, "Get number of objects in world")
        .def("getObjects", &SimpleWorld::getObjects, py::return_value_policy::reference_internal,
             "Get all objects in the world")
        .def("getAgentDistanceTo", [](const SimpleWorld& self, float x, float y) {
            auto agent = self.getAgentBody();
            float dx = x - agent.x;
            float dy = y - agent.y;
            return std::sqrt(dx * dx + dy * dy);
        }, py::arg("x"), py::arg("y"),
           "Calculate distance from agent to position (x, y)")
        .def("findNearestObject", [](const SimpleWorld& self, float x, float y) {
            auto agent = self.getAgentBody();
            float minDist = std::numeric_limits<float>::max();
            const WorldObject* nearest = nullptr;
            
            for (const auto& obj : self.getObjects()) {
                if (!obj.active) continue;
                float dx = obj.x - agent.x;
                float dy = obj.y - agent.y;
                float dist = std::sqrt(dx * dx + dy * dy);
                if (dist < minDist) {
                    minDist = dist;
                    nearest = &obj;
                }
            }
            
            if (nearest) {
                py::dict result;
                result["distance"] = minDist;
                result["x"] = nearest->x;
                result["y"] = nearest->y;
                result["type"] = static_cast<int>(nearest->type);
                result["value"] = nearest->value;
                result["radius"] = nearest->radius;
                return result;
            }
            throw WorldError("No objects found in world");
        }, py::arg("x"), py::arg("y"),
           "Find nearest object to position (x, y), returns dictionary or raises WorldError");

    // ========== ENHANCED BRAIN CLASS ==========
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration")
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
        .def("getRegion", [](Brain& self, RegionId id) {
            NeuralRegion* region = self.getRegion(id);
            if (!region) {
                throw BrainError("Region not found: " + std::to_string(id.value));
            }
            return region;
        }, py::arg("id"),
             "Get a region by ID (raises BrainError if not found)")
        .def("getRegion", [](Brain& self, RegionId id) -> std::shared_ptr<NeuralRegion> {
            NeuralRegion* region = self.getRegion(id);
            if (!region) {
                throw BrainError("Region not found: " + std::to_string(id.value));
            }
            return std::shared_ptr<NeuralRegion>(region);
        }, py::arg("id"),
             "Get a region by ID as shared_ptr (raises BrainError if not found)")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions (const reference)")
        .def("getRegionsShared", [](const Brain& self) -> std::vector<std::shared_ptr<NeuralRegion>> {
            const auto& regions = self.getRegions();
            std::vector<std::shared_ptr<NeuralRegion>> result;
            result.reserve(regions.size());
            for (const auto& region : regions) {
                result.push_back(std::shared_ptr<NeuralRegion>(region.get()));
            }
            return result;
        }, "Get all regions as shared_ptrs")
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
             "Get number of pending spike events")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getConfigShared", [](const Brain& self) -> std::shared_ptr<const Config> {
            return self.getConfig();
        }, "Get configuration as shared_ptr")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity rule")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity rule")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
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
             "Get dopamine system")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionError", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("getPlasticityStats", [](Brain& self) {
            py::dict stats;
            
            auto stdp = self.getSTDP();
            auto hebbian = self.getHebbian();
            auto structural = self.getStructuralPlasticity();
            
            if (stdp) {
                py::dict stdpStats;
                stdpStats["name"] = stdp->getName();
                stdpStats["ltpWeight"] = stdp->getLTPWeight();
                stdpStats["ltdWeight"] = stdp->getLTDWeight();
                stdpStats["timeConstant"] = stdp->getTimeConstant();
                stats["stdp"] = stdpStats;
            }
            
            if (hebbian) {
                py::dict hebbianStats;
                hebbianStats["name"] = hebbian->getName();
                hebbianStats["learningRate"] = hebbian->getLearningRate();
                hebbianStats["maxWeight"] = hebbian->getMaxWeight();
                stats["hebbian"] = hebbianStats;
            }
            
            if (structural) {
                py::dict structuralStats;
                structuralStats["synaptogenesisRate"] = structural->getSynaptogenesisRate();
                structuralStats["pruningRate"] = structural->getPruningRate();
                stats["structural"] = structuralStats;
            }
            
            return stats;
        }, "Get plasticity system statistics as dictionary")
        .def("getMemoryStats", [](Brain& self) {
            py::dict stats;
            
            auto workingMem = self.getWorkingMemory();
            auto episodicMem = self.getEpisodicMemory();
            auto associativeMem = self.getAssociativeMemory();
            
            if (workingMem) {
                py::dict workingStats;
                workingStats["size"] = workingMem->getSize();
                workingStats["capacity"] = workingMem->getCapacity();
                workingStats["activationThreshold"] = workingMem->getActivationThreshold();
                stats["working"] = workingStats;
            }
            
            if (episodicMem) {
                py::dict episodicStats;
                episodicStats["size"] = episodicMem->getSize();
                episodicStats["capacity"] = episodicMem->getCapacity();
                episodicStats["decayRate"] = episodicMem->getDecayRate();
                stats["episodic"] = episodicStats;
            }
            
            if (associativeMem) {
                py::dict associativeStats;
                associativeMem["patternCount"] = associativeMem->getPatternCount();
                associativeMem["similarityThreshold"] = associativeMem->getSimilarityThreshold();
                stats["associative"] = associativeStats;
            }
            
            return stats;
        }, "Get memory system statistics as dictionary")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals");

    // ========== ENHANCED AGENT BRAIN ==========
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Advanced agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create agent brain interface")
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
        .def("decodeFromMotorNeurons", &AgentBrain::decodeFromMotorNeurons,
             "Direct motor neuron decoding (low-level)")
        .def("selectWithCuriosity", &AgentBrain::selectWithCuriosity,
             py::arg("defaultCmd"),
             "Select action with curiosity/exploration")
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
        .def("getBrainShared", [](AgentBrain& self) -> std::shared_ptr<Brain> {
            return std::shared_ptr<Brain>(self.getBrain());
        }, "Get the underlying brain as shared_ptr")
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

    // ========== PLASTICITY CLASSES ==========
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity rule)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update STDP synapse weights")
        .def("applyWeightChange", &STDP::applyWeightChange,
             py::arg("synapse"), py::arg("delta"),
             "Apply weight change to synapse")
        .def("getName", &STDP::getName, "Get rule name")
        .def("configure", &STDP::configure,
             py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"),
             "Set LTP weight")
        .def("getLTPWeight", &STDP::getLTPWeight, "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"),
             "Set LTD weight")
        .def("getLTDWeight", &STDP::getLTDWeight, "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"),
             "Set STDP time constant")
        .def("getTimeConstant", &STDP::getTimeConstant, "Get STDP time constant");

    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update,
             py::arg("synapse"), py::arg("preSpikes"), py::arg("postSpikes"), py::arg("dt"),
             "Update Hebbian synapse weights")
        .def("applyWeightChange", &Hebbian::applyWeightChange,
             py::arg("synapse"), py::arg("delta"),
             "Apply weight change to synapse")
        .def("getName", &Hebbian::getName, "Get rule name")
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"),
             "Set learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate, "Get learning rate")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"),
             "Set maximum weight")
        .def("getMaxWeight", &Hebbian::getMaxWeight, "Get maximum weight");

    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity system)pbdoc")
        .def(py::init<>())
        .def("createSynapse", &StructuralPlasticity::createSynapse,
             py::arg("brain"), py::arg("source"), py::arg("destination"), py::arg("weight"),
             "Create new synapse (Phase 2)")
        .def("removeSynapse", &StructuralPlasticity::removeSynapse,
             py::arg("brain"), py::arg("synapse"),
             "Remove synapse (Phase 2)")
        .def("createNeuron", &StructuralPlasticity::createNeuron,
             py::arg("brain"), py::arg("type"),
             "Create new neuron (Phase 2)")
        .def("removeNeuron", &StructuralPlasticity::removeNeuron,
             py::arg("brain"), py::arg("neuron"),
             "Remove neuron (Phase 2)")
        .def("getSynaptogenesisRate", &StructuralPlasticity::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        .def("setSynaptogenesisRate", &StructuralPlasticity::setSynaptogenesisRate,
             py::arg("rate"),
             "Set synaptogenesis rate")
        .def("getPruningRate", &StructuralPlasticity::getPruningRate,
             "Get pruning rate")
        .def("setPruningRate", &StructuralPlasticity::setPruningRate,
             py::arg("rate"),
             "Set pruning rate")
        .def("update", &StructuralPlasticity::update,
             py::arg("brain"), py::arg("rng"),
             "Update structural plasticity");

    // ========== FACTORY FUNCTIONS ==========
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

    // Configuration source enum
    py::enum_<ConfigSource>(m, "ConfigSource", R"pbdoc(Configuration source enumeration)pbdoc")
        .value("Default", ConfigSource::Default)
        .value("File", ConfigSource::File)
        .value("CommandLine", ConfigSource::CommandLine)
        .value("Runtime", ConfigSource::Runtime)
        .export_values();

    // Invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic convenience functions
    m.def("createBrainWithDefaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        
        // Set common default values
        config->setString("brain.regions.count", "3");
        config->setBool("plasticity.enabled", true);
        config->setInt("memory.working_memory.max_size", 1000);
        config->setDouble("world.width", 100.0);
        config->setDouble("world.height", 100.0);
        config->setBool("world.wrapping", true);
        
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration");

    m.def("createWorldWithDefaults", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(100.0, 100.0, 5, 5);
        world->setAgentStart(50.0, 50.0);
        world->setMaxEnergy(100.0);
        world->setEnergyDecayRate(0.01);
        world->setRandomSeed(12345);
        return world;
    }, "Create a world with default configuration");

    m.def("createAgentBrainWithDefaults", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain with defaults");

    // Utility functions
    m.def("validateConfig", [](const Config& config) {
        return config.validate();
    }, py::arg("config"), "Validate configuration (raises ConfigError if invalid)");

    m.def("getPythonVersion", []() {
        return "3.8+ (requires pybind11 v2.6+)";
    }, "Get Python version requirement");

    m.def("getVersionInfo", []() {
        py::dict versionInfo;
        versionInfo["nlm"] = "2.0.0";
        versionInfo["python_bindings"] = "enhanced";
        versionInfo["date"] = "2026-09-30";
        versionInfo["features"] = std::vector<std::string>{
            "Smart pointer APIs",
            "Plasticity configuration",
            "Neuron parameter access",
            "Memory limits configuration",
            "Enhanced error handling",
            "Pythonic properties",
            "Configuration validation"
        };
        return versionInfo;
    }, "Get version information");

    // ===== ADVANCED PLACEMENT FEATURE =====
    m.def("initializePlasticityFromConfig", [](Config& config, STDP& stdp, Hebbian& hebbian, StructuralPlasticity& structural) {
        // Configure STDP from config
        if (config.has("plasticity.stdp.ltp_weight")) {
            stdp.setLTPWeight(config.getOr<double>("plasticity.stdp.ltp_weight", 0.01));
        }
        if (config.has("plasticity.stdp.ltd_weight")) {
            stdp.setLTDWeight(config.getOr<double>("plasticity.stdp.ltd_weight", 0.01));
        }
        if (config.has("plasticity.stdp.time_constant")) {
            stdp.setTimeConstant(config.getOr<double>("plasticity.stdp.time_constant", 20.0));
        }
        
        // Configure Hebbian from config
        if (config.has("plasticity.hebbian.learning_rate")) {
            hebbian.setLearningRate(config.getOr<double>("plasticity.hebbian.learning_rate", 0.001));
        }
        if (config.has("plasticity.hebbian.max_weight")) {
            hebbian.setMaxWeight(config.getOr<double>("plasticity.hebbian.max_weight", 1.0));
        }
        
        // Configure structural from config
        if (config.has("plasticity.structural.synaptogenesis_rate")) {
            structural.setSynaptogenesisRate(config.getOr<double>("plasticity.structural.synaptogenesis_rate", 0.001));
        }
        if (config.has("plasticity.structural.pruning_rate")) {
            structural.setPruningRate(config.getOr<double>("plasticity.structural.pruning_rate", 0.0005));
        }
    }, py::arg("config"), py::arg("stdp"), py::arg("hebbian"), py::arg("structural"),
       "Initialize plasticity systems from configuration");
}

} // namespace nlm
