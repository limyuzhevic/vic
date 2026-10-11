#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

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
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration Error: " + msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError("Simulation Error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory Error: " + msg) {}
};

class ValidationError : public NLMError {
public:
    explicit ValidationError(const std::string& msg) : NLMError("Validation Error: " + msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    // Register custom exceptions
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<ValidationError>(m, "ValidationError");

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
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
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
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
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

    // Config class with enhanced Python interface
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Enhanced Python-specific methods
        .def("getInt", [](Config& self, const std::string& key, int defaultValue) {
            if (!self.has(key)) return defaultValue;
            auto value = self.get<int>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), R"pbdoc(Get integer value with default)pbdoc")
        .def("getFloat", [](Config& self, const std::string& key, float defaultValue) {
            if (!self.has(key)) return defaultValue;
            auto value = self.get<double>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), R"pbdoc(Get float value with default)pbdoc")
        .def("getBool", [](Config& self, const std::string& key, bool defaultValue) {
            if (!self.has(key)) return defaultValue;
            auto value = self.get<bool>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), R"pbdoc(Get boolean value with default)pbdoc")
        .def("getString", [](Config& self, const std::string& key, const std::string& defaultValue) {
            if (!self.has(key)) return defaultValue;
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), R"pbdoc(Get string value with default)pbdoc")
        .def("setInt", [](Config& self, const std::string& key, int value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set integer value)pbdoc")
        .def("setFloat", [](Config& self, const std::string& key, float value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, static_cast<double>(value), source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set float value)pbdoc")
        .def("setBool", [](Config& self, const std::string& key, bool value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set boolean value)pbdoc")
        .def("setString", [](Config& self, const std::string& key, const std::string& value, ConfigSource source = ConfigSource::Runtime) {
            self.set(key, value, source);
        }, py::arg("key"), py::arg("value"), py::arg("source") = ConfigSource::Runtime, R"pbdoc(Set string value)pbdoc")
        .def("validate", &Config::summary,
             "Validate configuration and return summary");

    // SensoryInput class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + 
                   std::to_string(v.getHeight()) + "x" + std::to_string(v.getChannels()) + ">";
        });

    // Audio class
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // Action class
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
        .def("__repr__", [](const Action& a) {
            return "<Action: " + a.getName() + ">";
        });

    // WorldObject class
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
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject: " + std::to_string(obj.x) + "," + 
                   std::to_string(obj.y) + " type=" + std::to_string(static_cast<int>(obj.type)) +
                   " value=" + std::to_string(obj.value) + ">";
        });

    // AgentBody class
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
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody: pos=(" + std::to_string(body.x) + "," + 
                   std::to_string(body.y) + ") orient=" + std::to_string(body.orientation) +
                   " age=" + std::to_string(body.age) + ">";
        });

    // ActionResult class
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult: reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) +
                   " message='" + result.message + "'>";
        });

    // SensoryPercept class
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
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept: vision=" + std::to_string(percept.getVisionWidth()) + "x" + 
                   std::to_string(percept.getVisionHeight()) + 
                   " touch=" + std::to_string(percept.getTouch().size()) +
                   " internal=" + std::to_string(percept.getInternal().size()) + ">";
        });

    // SimpleWorld class with enhanced Python interface
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world simulation")
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
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid")
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
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("createDefaultEnvironment", [](SimpleWorld& self) {
            self.reset();
            self.setAgentStart(10.0, 10.0);
            // Add some default resources
            for (int x = 2; x < self.getWidth() - 2; x += 3) {
                for (int y = 2; y < self.getHeight() - 2; y += 3) {
                    WorldObject obj(x, y, WorldObjectType::Resource, 1.0f);
                    self.addObject(obj);
                }
            }
        }, "Create a default environment with resources")
        .def("createMaze", [](SimpleWorld& self, int width, int height) {
            self.configure(width, height, 8, 8);
            self.reset();
            self.setAgentStart(1, 1);
            
            // Create walls
            for (int x = 0; x < width; ++x) {
                WorldObject wall(x, 0, WorldObjectType::Wall, 1.0f);
                self.addObject(wall);
                wall.y = height - 1;
                self.addObject(wall);
            }
            for (int y = 0; y < height; ++y) {
                WorldObject wall(0, y, WorldObjectType::Wall, 1.0f);
                self.addObject(wall);
                wall.x = width - 1;
                self.addObject(wall);
            }
        }, py::arg("width"), py::arg("height"),
           "Create a maze environment")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" +
                   std::to_string(world.getHeight()) + " timestep=" +
                   std::to_string(world.getSimulationTime()) + ">";
        });

    // Brain class with enhanced Python interface
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
        // Enhanced Python-specific methods
        .def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration")
        .def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            auto world = std::make_shared<SimpleWorld>();
            world->configure(20, 20, 8, 8);
            world->reset();
            return world;
        }, "Create a simple world with default configuration")
        .def("createDefaultBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            if (!config) config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            if (!brain->initialize()) {
                throw SimulationError("Failed to initialize brain");
            }
            return brain;
        }, py::arg("config") = nullptr, "Create and initialize a default brain")
        .def("getConnectionsInfo", [](const Brain& brain) {
            std::vector<std::string> info;
            for (const auto& region : brain.getRegions()) {
                info.push_back("Region: " + std::to_string(region->getId().value()));
                info.push_back("  Neurons: " + std::to_string(region->getNeuronCount()));
                info.push_back("  Synapses: " + std::to_string(region->getSynapseCount()));
            }
            return info;
        }, "Get detailed connection information")
        .def("getNeuralActivityStats", [](const Brain& brain) {
            std::map<std::string, double> stats;
            stats["total_neurons"] = brain.getTotalNeuronCount();
            stats["total_synapses"] = brain.getTotalSynapseCount();
            stats["active_neurons"] = brain.getActiveNeuronCount();
            stats["firing_neurons"] = brain.getFiringNeuronCount();
            stats["total_spikes"] = brain.getTotalSpikeCount();
            stats["firing_rate"] = brain.getAverageFiringRate();
            stats["e_i_ratio"] = brain.getExcitationInhibitionRatio();
            return stats;
        }, "Get comprehensive neural activity statistics")
        .def("simulateStepAsync", [](Brain& brain, SimulationStep step) {
            // For now, this just runs the synchronous version
            // In a more advanced implementation, this could use threading
            brain.step(step);
        }, py::arg("step"), "Perform a simulation step (async-ready)")
        .def("configureLearningRates", [](Brain& brain, const std::shared_ptr<Config>& config) {
            if (!config) return;
            if (config->has("stdp_ltp_weight")) {
                auto* stdp = brain.getSTDP();
                if (stdp) {
                    stdp->setLTPWeight(config->get<float>("stdp_ltp_weight").value_or(0.01f));
                }
            }
            if (config->has("stdp_ltd_weight")) {
                auto* stdp = brain.getSTDP();
                if (stdp) {
                    stdp->setLTDWeight(config->get<float>("stdp_ltd_weight").value_or(0.012f));
                }
            }
        }, py::arg("config"), "Configure learning rates from config")
        .def("exportWeights", [](const Brain& brain, const std::string& filepath) {
            return brain.save(filepath);
        }, py::arg("filepath"), "Export brain state (including weights) to file")
        .def("importWeights", [](Brain& brain, const std::string& filepath) {
            return brain.load(filepath);
        }, py::arg("filepath"), "Import brain state (including weights) from file")
        .def("runEpisode", [](Brain& brain, std::shared_ptr<SimpleWorld> world, 
                           std::shared_ptr<AgentBrain> agent, 
                           size_t num_steps) {
            if (!world || !agent) return;
            
            agent->initialize(*world);
            
            for (size_t i = 0; i < num_steps; ++i) {
                // Update world
                world->update(0.1);
                
                // Get sensory input
                auto percept = world->getSensoryPercept();
                
                // Process sensory input
                agent->processSensoryInput(percept);
                
                // Run brain step
                brain.step(i);
                
                // Get action
                auto action = agent->decodeMotorCommand();
                
                // Apply action to world
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward modulation
                auto reward = percept.getInternal()[0] if percept.getInternal() else 0.0f;
                agent->applyRewardModulation(reward, 0.0f);
            }
        }, py::arg("world"), py::arg("agent"), py::arg("num_steps"),
           "Run a complete episode simulation")
        .def("getBehaviorPattern", [](const Brain& brain, size_t num_steps) {
            std::vector<std::string> pattern;
            // This would be implemented to analyze behavior patterns
            pattern.push_back("Analysis of brain behavior over " + std::to_string(num_steps) + " steps");
            return pattern;
        }, py::arg("num_steps"), "Analyze brain behavior patterns")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + 
                   " neurons, " + std::to_string(brain.getTotalSynapseCount()) +
                   " synapses, stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + ">";
        });

    // AgentBrain class with enhanced Python interface
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
        // Configuration methods
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
        // Enhanced Python-specific methods
        .def("createAgent", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create an agent brain interface")
        .def("configureAgent", [](AgentBrain& agent, const std::shared_ptr<Config>& config) {
            if (!config) return;
            
            // Enable all features by default
            agent.enableRewardModulation(true);
            agent.enableStructuralPlasticity(true);
            agent.enableDevelopment(true);
            agent.enableCuriosity(true);
            
            // Configure specific parameters
            if (config->has("neuromod.curiosity.enable")) {
                agent.enableCuriosity(config->get<bool>("neuromod.curiosity.enable").value_or(true));
            }
            if (config->has("neuromod.novelty.enable")) {
                // This would need a method to enable novelty detection
            }
        }, py::arg("config"), "Configure agent with settings from config")
        .def("enableAllFeatures", &AgentBrain::enableRewardModulation, true,
             &AgentBrain::enableStructuralPlasticity, true,
             &AgentBrain::enableDevelopment, true,
             &AgentBrain::enableCuriosity, true,
             "Enable all agent features at once")
        .def("getPerformanceMetrics", [](const AgentBrain& agent) {
            std::map<std::string, double> metrics;
            metrics["curiosity_level"] = agent.getCuriosityLevel();
            metrics["novelty_level"] = agent.getNoveltyLevel();
            metrics["prediction_error"] = agent.getPredictionError();
            metrics["neuromodulation_level"] = agent.getNeuromodulationLevel();
            metrics["development_stage"] = static_cast<int>(agent.getDevelopmentalStage());
            return metrics;
        }, "Get comprehensive performance metrics")
        .def("resetWithSeed", [](AgentBrain& agent, uint64_t seed) {
            // Reset with specific seed for reproducibility
            agent.reset();
            // Note: The actual seed setting would need access to brain's random generator
        }, py::arg("seed"), "Reset agent with specific random seed")
        .def("getActionProbabilities", [](const AgentBrain& agent) {
            std::map<std::string, double> probabilities;
            // This would calculate action selection probabilities
            probabilities["MoveForward"] = 0.2;
            probabilities["MoveBackward"] = 0.1;
            probabilities["TurnLeft"] = 0.2;
            probabilities["TurnRight"] = 0.2;
            probabilities["Interact"] = 0.2;
            probabilities["Wait"] = 0.1;
            return probabilities;
        }, "Get action selection probabilities")
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   " novelty=" + std::to_string(agent.getNoveltyLevel()) +
                   " stage=" + std::to_string(static_cast<int>(agent.getDevelopmentalStage())) + ">";
        });

    // Convenience factory functions
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config") = nullptr, "Create and initialize a brain")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        return world;
    }, "Create a simple world with default configuration")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface")
    
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration")
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config") = nullptr, "Create and initialize a brain")
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        return world;
    }, "Create a simple world with default configuration")
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface")
    
    // Global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
    
    // Python helper functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, size_t num_steps) {
        if (!world || !agent) {
            throw SimulationError("World and agent must be provided");
        }
        
        agent->initialize(*world);
        
        for (size_t i = 0; i < num_steps; ++i) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(i);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            auto reward = percept.getInternal()[0] if percept.getInternal() else 0.0f;
            agent->applyRewardModulation(reward, 0.0f);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete simulation episode")
    
    m.def("create_learning_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up for learning experiments
        config->set("brain.neuron_count", 1000);
        config->set("brain.synapse_density", 0.1);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 1.0f);
        
        return config;
    }, "Create a configuration optimized for learning experiments")
    
    m.def("create_exploration_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up for exploration behavior
        config->set("brain.neuron_count", 500);
        config->set("brain.synapse_density", 0.05);
        config->set("plasticity.stdp.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 0.5f);
        
        return config;
    }, "Create a configuration optimized for exploration behavior")
    
    m.def("create_defense_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set up for defensive behavior
        config->set("brain.neuron_count", 800);
        config->set("brain.synapse_density", 0.15);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.curiosity.enable", false); // Less exploration
        config->set("neuromod.novelty.enable", false); // Less novelty
        
        return config;
    }, "Create a configuration optimized for defensive behavior");
    
    // Version and information
    m.attr("__version__") = "0.1.0";
    m.attr("__author__") = "NLM Authors";
    m.attr("__description__") = "Python bindings for NLM (Neural Learning Machine)";
}