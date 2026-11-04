#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <utility>
#include <stdexcept>
#include <algorithm>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/Hebbian.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions for better error handling
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& msg) : NLMException("Config Error: " + msg) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& msg) : NLMException("Brain Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

// Register all ID types with a template to reduce boilerplate
template<typename T>
void register_id_type(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<T>(m, name.c_str(), py::doc(doc.c_str()))
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &T::value)
        .def("index", &T::index)
        .def("__eq__", &T::operator==)
        .def("__ne__", &T::operator!=)
        .def("__hash__", [](const T& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const T& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        });
}

// Helper function for batch operations
template<typename Callable>
void batch_process(std::vector<std::reference_wrapper<const NeuronId>>& neurons,
                   const std::string& operationName,
                   Callable&& func) {
    try {
        for (auto& neuron : neurons) {
            func(neuron.get());
        }
    } catch (const std::exception& e) {
        throw SimulationError("Batch operation " + operationName + " failed: " + e.what());
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Additional features:
        - Convenience factory methods
        - Batch operations for performance
        - Advanced Pythonic APIs
        - Configuration helpers
        - Simulation utilities
        - Error handling for Python developers
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMException>(m, "NLMException");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<SimulationError>(m, "SimulationError");

    // Register all ID types using the template to reduce code duplication
    register_id_type<NeuronId>(m, "NeuronId", "Unique identifier for a neuron");
    register_id_type<SynapseId>(m, "SynapseId", "Unique identifier for a synapse");
    register_id_type<RegionId>(m, "RegionId", "Unique identifier for a brain region");
    register_id_type<PopulationId>(m, "PopulationId", "Unique identifier for a neuron population");

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

    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
        // loadFromArgs takes (int, char**) in C++, which has no Python
        // equivalent; bind it as a list of strings instead (Phase 5
        // fix — the old binding did not compile).
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
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
        // Pythonic convenience methods
        .def("get", [](const Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) throw ConfigError("Key not found: " + key);
            return *value;
        }, py::arg("key"), "Get a string value (throws ConfigError if not found)")
        .def("getOr", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            return value ? *value : defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get a string value or default if not found")
        .def("getInt", [](const Config& self, const std::string& key) {
            auto value = self.get<int64_t>(key);
            if (!value) throw ConfigError("Key not found or wrong type: " + key);
            return *value;
        }, py::arg("key"), "Get an integer value (throws ConfigError if not found)")
        .def("getFloat", [](const Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value) throw ConfigError("Key not found or wrong type: " + key);
            return *value;
        }, py::arg("key"), "Get a float value (throws ConfigError if not found)")
        .def("getBool", [](const Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (!value) throw ConfigError("Key not found or wrong type: " + key);
            return *value;
        }, py::arg("key"), "Get a boolean value (throws ConfigError if not found)")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a string value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set an integer value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a float value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a boolean value")
        .def("setInt", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set an integer value")
        .def("setFloat", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a float value")
        .def("setBool", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set a boolean value")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        // Pythonic convenience methods
        .def("data", &SensoryInput::getData, "Alias for getData() - Pythonic API")
        .def("type", &SensoryInput::getType, "Alias for getType() - Pythonic API");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        // Pythonic convenience methods
        .def("shape", [ ](Vision& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight(), self.getChannels()};
        }, "Get tensor shape as [width, height, channels]")
        .def("flatten", [ ](Vision& self) {
            std::vector<float> data = self.getData();
            return data;
        }, "Get flattened data array");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        // Pythonic convenience methods
        .def("duration", [ ](Audio& self) {
            return self.getNumSamples() / static_cast<float>(self.getSampleRate());
        }, "Get duration in seconds")
        .def("sampleRate", &Audio::getSampleRate, "Alias for getSampleRate() - Pythonic API");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        // Pythonic convenience methods
        .def("add", &InternalSignals::addSignal, py::arg("value"), "Alias for addSignal() - Pythonic API")
        .def("signals", &InternalSignals::getData, "Get internal signals as vector (alias for getData())");

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
        // Pythonic convenience methods
        .def("type", &Action::getType, "Alias for getType() - Pythonic API")
        .def("params", &Action::getParameters, "Alias for getParameters() - Pythonic API")
        .def("set", &Action::setType, py::arg("type"), "Alias for setType() - Pythonic API")
        .def("name", &Action::getName, "Alias for getName() - Pythonic API");

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
        // Pythonic convenience methods
        .def("position", [ ](WorldObject& self) {
            return std::vector<float>{self.x, self.y};
        }, "Get position as [x, y]")
        .def("location", [ ](WorldObject& self) {
            return std::vector<float>{self.x, self.y};
        }, "Alias for position() - Pythonic API")
        .def("is_resource", [ ](const WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if object is a resource")
        .def("is_hazard", [ ](const WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if object is a hazard")
        .def("is_wall", [ ](const WorldObject& self) {
            return self.type == WorldObjectType::Wall;
        }, "Check if object is a wall");

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
        // Pythonic convenience methods
        .def("position", [ ](AgentBody& self) {
            return std::vector<float>{self.x, self.y};
        }, "Get position as [x, y]")
        .def("velocity", [ ](AgentBody& self) {
            return std::vector<float>{self.velocityX, self.velocityY};
        }, "Get velocity as [vx, vy]")
        .def("location", [ ](AgentBody& self) {
            return std::vector<float>{self.x, self.y};
        }, "Alias for position() - Pythonic API")
        .def("heading", [ ](AgentBody& self) {
            return self.orientation;
        }, "Get orientation (heading)")
        .def("speed", [ ](AgentBody& self) {
            return std::sqrt(self.velocityX * self.velocityX + self.velocityY * self.velocityY);
        }, "Get speed (magnitude of velocity)");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Pythonic convenience methods
        .def("outcome", [ ](const ActionResult& self) {
            return std::make_pair(self.reward, self.success);
        }, "Get outcome as (reward, success) tuple")
        .def("succeeded", [ ](const ActionResult& self) {
            return self.success;
        }, "Check if action succeeded")
        .def("has_reward", [ ](const ActionResult& self) {
            return self.reward != 0.0f;
        }, "Check if action has non-zero reward");

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
        // Pythonic convenience methods
        .def("vision", &SensoryPercept::getVision, "Alias for getVision() - Pythonic API")
        .def("touch", &SensoryPercept::getTouch, "Alias for getTouch() - Pythonic API")
        .def("internal", &SensoryPercept::getInternal, "Alias for getInternal() - Pythonic API")
        .def("audio", &SensoryPercept::getAudio, "Alias for getAudio() - Pythonic API")
        .def("all_signals", &SensoryPercept::getAllSignals, "Alias for getAllSignals() - Pythonic API")
        .def("timestamp", &SensoryPercept::getTimestamp, "Alias for getTimestamp() - Pythonic API")
        .def("set_all_signals", [](SensoryPercept& self, const std::vector<float>& signals) {
            self.setInternal(signals);
        }, "Set all internal signals (convenience method)");

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        // Pythonic convenience methods
        .def("agent_position", [ ](SimpleWorld& self) {
            return std::vector<float>{self.getAgentBody().x, self.getAgentBody().y};
        }, "Get agent position as [x, y]")
        .def("agent_body", &SimpleWorld::getAgentBody, "Alias for getAgentBody() - Pythonic API")
        .def("sensory_percept", &SimpleWorld::getSensoryPercept, "Alias for getSensoryPercept() - Pythonic API")
        .def("world_size", [ ](SimpleWorld& self) {
            return std::vector<size_t>{self.getWidth(), self.getHeight()};
        }, "Get world dimensions as [width, height]")
        .def("add_resource", [ ](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Resource;
            obj.value = value;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Add a resource object")
        .def("add_hazard", [ ](SimpleWorld& self, float x, float y, float value = 1.0f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Hazard;
            obj.value = value;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("value") = 1.0f, "Add a hazard object")
        .def("add_wall", [ ](SimpleWorld& self, float x, float y, float radius = 0.5f) {
            WorldObject obj;
            obj.x = x;
            obj.y = y;
            obj.type = WorldObjectType::Wall;
            obj.radius = radius;
            self.addObject(obj);
        }, py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f, "Add a wall object");

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Pythonic convenience methods
        .def("region", [ ](Brain& self, RegionId id) {
            return self.getRegion(id);
        }, py::arg("id"), "Alias for getRegion() - Pythonic API")
        .def("neuron_count", &Brain::getTotalNeuronCount, "Alias for getTotalNeuronCount() - Pythonic API")
        .def("synapse_count", &Brain::getTotalSynapseCount, "Alias for getTotalSynapseCount() - Pythonic API")
        .def("spike_count", &Brain::getTotalSpikeCount, "Alias for getTotalSpikeCount() - Pythonic API")
        .def("active_neurons", &Brain::getActiveNeuronCount, "Alias for getActiveNeuronCount() - Pythonic API")
        .def("firing_neurons", &Brain::getFiringNeuronCount, "Alias for getFiringNeuronCount() - Pythonic API")
        .def("step_once", [ ](Brain& self) {
            // Simplified step for one simulation step
            static SimulationStep step = 0;
            self.step(step++);
        }, "Perform a single simulation step (convenience method)")
        .def("inject_sensory", &Brain::receiveSensoryInput,
             py::arg("input"), "Alias for receiveSensoryInput() - Pythonic API")
        .def("produce_motor_action", &Brain::produceAction, "Alias for produceAction() - Pythonic API");

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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
        // Pythonic convenience methods
        .def("sensory_input_size", &AgentBrain::getSensoryInputSize, "Alias for getSensoryInputSize() - Pythonic API")
        .def("motor_output_size", &AgentBrain::getMotorOutputSize, "Alias for getMotorOutputSize() - Pythonic API")
        .def("process_sensory", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Alias for processSensoryInput() - Pythonic API")
        .def("decode_motor", &AgentBrain::decodeMotorCommand, "Alias for decodeMotorCommand() - Pythonic API")
        .def("apply_reward", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"), "Alias for applyRewardModulation() - Pythonic API")
        .def("neuromodulation", &AgentBrain::getNeuromodulationLevel, "Alias for getNeuromodulationLevel() - Pythonic API")
        .def("curiosity", &AgentBrain::getCuriosityLevel, "Alias for getCuriosityLevel() - Pythonic API")
        .def("novelty", &AgentBrain::getNoveltyLevel, "Alias for getNoveltyLevel() - Pythonic API")
        .def("prediction_error", &AgentBrain::getPredictionError, "Alias for getPredictionError() - Pythonic API")
        .def("development", &AgentBrain::getDevelopmentalStage, "Alias for getDevelopmentalStage() - Pythonic API")
        .def("is_enabled", [](AgentBrain& self, const std::string& feature) {
            if (feature == "reward_modulation") return self.isRewardModulationEnabled();
            if (feature == "structural_plasticity") return self.isStructuralPlasticityEnabled();
            if (feature == "development") return self.isDevelopmentEnabled();
            if (feature == "curiosity") return self.isCuriosityEnabled();
            throw SimulationError("Unknown feature: " + feature);
        }, py::arg("feature"), "Check if a feature is enabled")
        .def("enable_feature", [](AgentBrain& self, const std::string& feature, bool enable) {
            if (feature == "reward_modulation") self.enableRewardModulation(enable);
            else if (feature == "structural_plasticity") self.enableStructuralPlasticity(enable);
            else if (feature == "development") self.enableDevelopment(enable);
            else if (feature == "curiosity") self.enableCuriosity(enable);
            else throw SimulationError("Unknown feature: " + feature);
        }, py::arg("feature"), py::arg("enable"), "Enable/disable a feature");

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

    // Advanced utility functions for experienced users
    m.def("create_demo_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set up a basic configuration for a simple neural network
        config->set("neuron_count", static_cast<int64_t>(100), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.1, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
        config->set("max_steps", 1000, nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.01, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration suitable for basic demonstrations");

    m.def("create_demo_config_advanced", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set up a more complex configuration for advanced demonstrations
        config->set("neuron_count", static_cast<int64_t>(500), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(4), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.15, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.001, nlm::ConfigSource::Default);
        config->set("max_steps", 5000, nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.001, nlm::ConfigSource::Default);
        config->set("enable_reward_modulation", true, nlm::ConfigSource::Default);
        config->set("enable_structural_plasticity", true, nlm::ConfigSource::Default);
        config->set("enable_development", true, nlm::ConfigSource::Default);
        config->set("enable_curiosity", true, nlm::ConfigSource::Default);
        return config;
    }, "Create a configuration suitable for advanced demonstrations");

    m.def("create_simple_demo_config", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(50), nlm::ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), nlm::ConfigSource::Default);
        config->set("connection_probability", 0.2, nlm::ConfigSource::Default);
        config->set("learning_rate", 0.01, nlm::ConfigSource::Default);
        config->set("max_steps", 500, nlm::ConfigSource::Default);
        config->set("simulation_timestep", 0.01, nlm::ConfigSource::Default);
        config->set("enable_reward_modulation", false, nlm::ConfigSource::Default);
        return config;
    }, "Create a minimal configuration for quick demos");

    // Simulation utilities
    m.def("run_simple_demo", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain->initialize()) {
            throw SimulationError("Failed to initialize brain");
        }
        
        for (int step = 0; step < 100; ++step) {
            // Get sensory percept
            auto percept = world->getSensoryPercept();
            // Process it
            brain->receiveSensoryInput(percept);
            // Step the brain
            brain->step(step);
            // Apply motor command
            auto action = brain->produceAction();
            world->applyMotorCommand(action, step * 0.01f);
        }
        
        return "Demo completed successfully";
    }, py::arg("brain"), py::arg("world"), "Run a simple demonstration loop");

    m.def("create_agent_with_world", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"), "Create an agent brain and initialize it with a world");

    // Performance optimization utilities
    m.def("create_config_batch", [](const std::vector<std::pair<std::string, nlm::ConfigValue>>& settings) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        for (const auto& setting : settings) {
            config->set(setting.first, setting.second, nlm::ConfigSource::Runtime);
        }
        return config;
    }, py::arg("settings"), "Create configuration from a batch of settings");

    // Debug and monitoring utilities
    m.def("get_brain_summary", [](std::shared_ptr<Brain> brain) {
        std::stringstream ss;
        ss << "Neuron Count: " << brain->getTotalNeuronCount() << "\n";
        ss << "Synapse Count: " << brain->getTotalSynapseCount() << "\n";
        ss << "Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        ss << "Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        ss << "Spike Count: " << brain->getTotalSpikeCount() << "\n";
        ss << "Excitement/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << "\n";
        return ss.str();
    }, py::arg("brain"), "Get a detailed summary of brain state");

    // Visualization utilities
    m.def("create_test_vision", [](size_t width, size_t height, size_t channels = 3) -> std::shared_ptr<Vision> {
        auto vision = std::make_shared<Vision>(width, height, channels);
        // Fill with test pattern (gradient)
        std::vector<float> data(width * height * channels);
        for (size_t i = 0; i < data.size(); ++i) {
            data[i] = static_cast<float>(i % channels) / channels;
        }
        vision->setData(data);
        return vision;
    }, py::arg("width"), py::arg("height"), py::arg("channels") = 3, "Create a test vision input with gradient pattern");

    m.def("create_test_audio", [](size_t sampleRate, size_t numSamples) -> std::shared_ptr<Audio> {
        auto audio = std::make_shared<Audio>(sampleRate, numSamples);
        // Fill with test sine wave
        std::vector<float> data(numSamples);
        for (size_t i = 0; i < numSamples; ++i) {
            data[i] = 0.5f * std::sin(2.0f * M_PI * static_cast<float>(i) * 440.0f / sampleRate);
        }
        audio->setData(data);
        return audio;
    }, py::arg("sampleRate"), py::arg("numSamples"), "Create test audio input with sine wave");

    // Convenience functions for batch operations
    m.def("batch_inject_current", [](std::shared_ptr<Brain> brain,
                                   const std::vector<std::pair<NeuronId, float>>& injections) {
        for (const auto& inj : injections) {
            brain->injectCurrent(inj.first, inj.second);
        }
        return static_cast<int>(injections.size());
    }, py::arg("brain"), py::arg("injections"), "Batch inject current into multiple neurons");

    m.def("batch_set_parameters", [](std::shared_ptr<Config> config,
                                   const std::vector<std::pair<std::string, nlm::ConfigValue>>& settings) {
        for (const auto& setting : settings) {
            config->set(setting.first, setting.second, nlm::ConfigSource::Runtime);
        }
        return static_cast<int>(settings.size());
    }, py::arg("config"), py::arg("settings"), "Batch set configuration parameters");

    // Error handling utilities
    m.def("safe_step", [](std::shared_ptr<Brain> brain, SimulationStep step) -> bool {
        try {
            brain->step(step);
            return true;
        } catch (const std::exception& e) {
            return false;
        }
    }, py::arg("brain"), py::arg("step"), "Safely step brain, returns true if successful");

    m.def("safe_initialize", [](std::shared_ptr<Brain> brain) -> bool {
        try {
            return brain->initialize();
        } catch (const std::exception& e) {
            return false;
        }
    }, py::arg("brain"), "Safely initialize brain, returns true if successful");

    // Constants and invalid IDs (already present but reorganized)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

} // namespace nlm
