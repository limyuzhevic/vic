#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <optional>

// Include all headers
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"

namespace py = pybind11;
namespace nlm {

// Python-friendly exception types
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidArgumentError : public NLMError {
public:
    explicit InvalidArgumentError(const std::string& msg) : NLMError(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError(msg) {}
};

class SimulationError : public NLMError {
public:
    explicit SimulationError(const std::string& msg) : NLMError(msg) {}
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Features:
        - Convenience functions for common operations
        - Type conversions between Python and C++ types
        - Enhanced error handling with Python-style exceptions
        - Helper functions for creating common configurations
        - Advanced API access to internal systems
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<InvalidArgumentError>(m, "InvalidArgumentError", PyExc_ValueError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // ===================== BASIC TYPE BINDINGS =====================

    py::class_<NeuronId>(m, "NeuronId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
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

    py::class_<SynapseId>(m, "SynapseId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
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

    py::class_<RegionId>(m, "RegionId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
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

    py::class_<PopulationId>(m, "PopulationId")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value") = 0)
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // ===================== ENUM BINDINGS =====================

    py::enum_<NeuronType>(m, "NeuronType")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType")
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

    py::enum_<MotorCommand>(m, "MotorCommand")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ===================== CONVENIENCE TYPE BINDINGS =====================

    // Type conversion helpers
    m.def("neuron_id", [](uint64_t value) { return NeuronId(value); },
          "Create a NeuronId from an integer");

    m.def("synapse_id", [](uint64_t value) { return SynapseId(value); },
          "Create a SynapseId from an integer");

    m.def("region_id", [](uint64_t value) { return RegionId(value); },
          "Create a RegionId from an integer");

    m.def("population_id", [](uint64_t value) { return PopulationId(value); },
          "Create a PopulationId from an integer");

    // ===================== ADVANCED CONFIG BINDINGS =====================

    py::class_<Config, std::shared_ptr<Config>>(m, "Config")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file")
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        // Typed getter methods with Pythonic API
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<std::string>(key);
            if (!value) throw InvalidArgumentError("Key not found: " + key);
            return *value;
        }, py::arg("key"), "Get string value (throws if key not found)")
        .def("get", [](Config& self, const std::string& key, const std::string& defaultValue) {
            auto value = self.get<std::string>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get string value with default")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<int>(key);
            if (!value) throw InvalidArgumentError("Key not found or not an integer: " + key);
            return *value;
        }, py::arg("key"), "Get integer value (throws if key not found)")
        .def("get", [](Config& self, const std::string& key, int defaultValue) {
            auto value = self.get<int>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get integer value with default")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<double>(key);
            if (!value) throw InvalidArgumentError("Key not found or not a float: " + key);
            return *value;
        }, py::arg("key"), "Get float value (throws if key not found)")
        .def("get", [](Config& self, const std::string& key, double defaultValue) {
            auto value = self.get<double>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get float value with default")
        .def("get", [](Config& self, const std::string& key) {
            auto value = self.get<bool>(key);
            if (!value) throw InvalidArgumentError("Key not found or not a boolean: " + key);
            return *value;
        }, py::arg("key"), "Get boolean value (throws if key not found)")
        .def("get", [](Config& self, const std::string& key, bool defaultValue) {
            auto value = self.get<bool>(key);
            if (value) return *value;
            return defaultValue;
        }, py::arg("key"), py::arg("defaultValue"), "Get boolean value with default")
        .def("set", [](Config& self, const std::string& key, const std::string& value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set string configuration value")
        .def("set", [](Config& self, const std::string& key, int value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set integer configuration value")
        .def("set", [](Config& self, const std::string& key, double value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set float configuration value")
        .def("set", [](Config& self, const std::string& key, bool value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"), "Set boolean configuration value")
        .def("get_number", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get integer with default (alternative API)")
        .def("get_string", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get string with default (alternative API)")
        .def("get_float", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get float with default (alternative API)");

    // ===================== SENSORY BINDINGS =====================

    py::class_<SensoryInput>(m, "SensoryInput")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("clone", &SensoryInput::clone, "Create a copy of this sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Vision::getData, "Get vision data as a list of floats")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("setWidthHeight", [](Vision& self, size_t width, size_t height) {
            self.setWidthHeight(width, height);
        }, py::arg("width"), py::arg("height"));

    py::class_<Audio, SensoryInput>(m, "Audio")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getData", &Audio::getData, "Get audio data as a list of floats")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getSignals", &InternalSignals::getSignals, "Get all signals as a list");

    // ===================== ACTION BINDINGS =====================

    py::class_<Action>(m, "Action")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& action) {
            return py::dict("type", action.getType(), "parameters", action.getParameters());
        }, "Convert action to dictionary")
        .def("from_dict", [](ActionType type, const py::dict& params) {
            std::vector<float> param_vector;
            if (params.contains("parameters")) {
                param_vector = params["parameters"].cast<std::vector<float>>();
            }
            return Action(type, param_vector);
        }, py::arg("type"), py::arg("parameters"), "Create action from dictionary");

    // ===================== WORLD OBJECT BINDINGS =====================

    py::class_<WorldObject>(m, "WorldObject")
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
            std::string typeStr;
            switch(obj.type) {
                case WorldObjectType::Empty: typeStr = "Empty"; break;
                case WorldObjectType::Resource: typeStr = "Resource"; break;
                case WorldObjectType::Hazard: typeStr = "Hazard"; break;
                case WorldObjectType::Wall: typeStr = "Wall"; break;
                case WorldObjectType::Marker: typeStr = "Marker"; break;
            }
            return "<WorldObject at (" + std::to_string(obj.x) + ", " + std::to_string(obj.y) +
                   ") type=" + typeStr + " value=" + std::to_string(obj.value) + ">";
        });

    // ===================== AGENT BODY BINDINGS =====================

    py::class_<AgentBody>(m, "AgentBody")
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
        .def("to_dict", [](const AgentBody& body) {
            return py::dict("x", body.x, "y", body.y, "orientation", body.orientation,
                           "velocityX", body.velocityX, "velocityY", body.velocityY,
                           "angularVelocity", body.angularVelocity, "energy", body.energy,
                           "health", body.health, "age", body.age, "isMoving", body.isMoving,
                           "isTurning", body.isTurning, "lastActionTime", body.lastActionTime);
        }, "Convert agent body to dictionary")
        .def("from_dict", [](const py::dict& data) {
            AgentBody body;
            body.x = data["x"].cast<float>();
            body.y = data["y"].cast<float>();
            body.orientation = data["orientation"].cast<float>();
            body.velocityX = data["velocityX"].cast<float>();
            body.velocityY = data["velocityY"].cast<float>();
            body.angularVelocity = data["angularVelocity"].cast<float>();
            body.energy = data["energy"].cast<float>();
            body.health = data["health"].cast<float>();
            body.age = data["age"].cast<float>();
            body.isMoving = data["isMoving"].cast<bool>();
            body.isTurning = data["isTurning"].cast<bool>();
            body.lastActionTime = data["lastActionTime"].cast<float>();
            return body;
        }, py::arg("data"), "Create agent body from dictionary");

    // ===================== ACTION RESULT BINDINGS =====================

    py::class_<ActionResult>(m, "ActionResult")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) +
                   " message=\"" + result.message + "\">";
        });

    // ===================== SENSORY PERCEPT BINDINGS =====================

    py::class_<SensoryPercept>(m, "SensoryPercept")
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
        .def("to_dict", [](const SensoryPercept& percept) {
            return py::dict("vision", percept.getVision(), "touch", percept.getTouch(),
                           "internal", percept.getInternal(), "proprioception", percept.getProprioception(),
                           "audio", percept.getAudio(), "timestamp", percept.getTimestamp());
        }, "Convert percept to dictionary")
        .def("from_dict", [](const py::dict& data) {
            SensoryPercept percept;
            if (data.contains("vision")) percept.setVision(data["vision"].cast<std::vector<float>>());
            if (data.contains("touch")) percept.setTouch(data["touch"].cast<std::vector<float>>());
            if (data.contains("internal")) percept.setInternal(data["internal"].cast<std::vector<float>>());
            if (data.contains("proprioception")) percept.setProprioception(data["proprioception"].cast<std::vector<float>>());
            if (data.contains("audio")) percept.setAudio(data["audio"].cast<std::vector<float>>());
            if (data.contains("timestamp")) percept.setTimestamp(data["timestamp"].cast<double>());
            return percept;
        }, py::arg("data"), "Create percept from dictionary");

    // ===================== SIMPLE WORLD BINDINGS =====================

    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth") = 16, py::arg("visionHeight") = 16)
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::return_value_policy::reference_internal,
             py::arg("x"), py::arg("y"), "Get object at position (returns reference)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("to_dict", [](const SimpleWorld& world) {
            py::dict result;
            result["width"] = world.getWidth();
            result["height"] = world.getHeight();
            result["visionWidth"] = world.getVisionWidth();
            result["visionHeight"] = world.getVisionHeight();
            result["maxEnergy"] = world.getMaxEnergy();
            result["energyDecayRate"] = world.getEnergyDecayRate();
            result["simulationTime"] = world.getSimulationTime();
            result["randomSeed"] = world.getRandomSeed();
            return result;
        }, "Convert world to dictionary");

    // ===================== BRAIN BINDINGS =====================

    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain")
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
        // Advanced brain system access
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             "Get spike system (advanced access)")
        .def("getSTDP", &Brain::getSTDP,
             "Get STDP plasticity system (advanced access)")
        .def("getHebbian", &Brain::getHebbian,
             "Get Hebbian plasticity system (advanced access)")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             "Get structural plasticity system (advanced access)")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             "Get working memory system (advanced access)")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             "Get episodic memory system (advanced access)")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             "Get associative memory system (advanced access)")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             "Get prediction system (advanced access)")
        .def("getPlanner", &Brain::getPlanner,
             "Get neural planner system (advanced access)")
        .def("getConceptFormation", &Brain::getConceptFormation,
             "Get concept formation system (advanced access)")
        .def("getAttention", &Brain::getAttention,
             "Get attentional selection system (advanced access)")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             "Get development system (advanced access)")
        .def("getDopamine", &Brain::getDopamine,
             "Get dopamine neuromodulation system (advanced access)")
        .def("getCuriosity", &Brain::getCuriosity,
             "Get curiosity system (advanced access)")
        .def("getNovelty", &Brain::getNovelty,
             "Get novelty detection system (advanced access)")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             "Get prediction error system (advanced access)")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             "Get random number generator (advanced access)")
        .def("getLearnedPreference", &Brain::getLearnedPreference,
             "Get learned preference trait")
        .def("setLearnedPreference", &Brain::setLearnedPreference,
             py::arg("preference"),
             "Set learned preference trait")
        .def("getNeuromodulationLevel", []() { return 0.0f; },  // Placeholder
             "Get current neuromodulation level (advanced access)")
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f,
             py::arg("delay") = 1,
             "Add connection between brain regions (advanced access)")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove connection between brain regions (advanced access)")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events");

    // ===================== AGENT BRAIN BINDINGS =====================

    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain")
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
        // AgentBrain advanced features
        .def("getMotorGroupActivities", &AgentBrain::getMotorGroupActivities,
             "Get detailed motor neuron group activities")
        .def("getThoughtTrace", &AgentBrain::getThoughtTrace,
             "Get cognitive thought trace")
        .def("getLearnedPreference", &AgentBrain::getLearnedPreference,
             "Get learned preference trait")
        .def("encodeText", &AgentBrain::encodeText, py::arg("text"),
             "Encode text as sparse pattern (Phase 6)")
        .def("getInternalFiringRates", &AgentBrain::getInternalFiringRates,
             "Get internal neuron firing rates for recall probe")
        .def("getConceptCount", &AgentBrain::getConceptCount,
             "Get concept count (Phase 6)")
        .def("getSelfModelConfidence", &AgentBrain::getSelfModelConfidence,
             "Get self-model confidence (Phase 6)")
        .def("getSocialObservationCount", &AgentBrain::getSocialObservationCount,
             "Get social observation count (Phase 6)")
        .def("observeOtherAgent", &AgentBrain::observeOtherAgent,
             py::arg("observed"), py::arg("beforeVision"), py::arg("afterVision"),
             "Observe another agent's behavior (Phase 6)");

    // ===================== FACTORY FUNCTIONS =====================

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

    // ===================== CONVENIENCE FACTORY FUNCTIONS =====================

    // Factory for brains with default configuration
    m.def("createBrainWithDefaults", []() {
        auto config = std::make_shared<Config>();
        config->set("brain.regionCount", 2);
        config->set("brain.plasticregionCount", 0);
        config->set("brain.hebbian.regionCount", 0);
        config->set("brain.stdp.enabled", true);
        config->set("brain.stdp.learningRate", 0.01);
        config->set("brain.stdp.ltdWeight", -0.01);
        config->set("brain.stdp.windowSize", 20);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration settings");

    // Factory for world with standard parameters
    m.def("createStandardWorld", [](int width = 100, int height = 100,
                                   int visionWidth = 16, int visionHeight = 16) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height),
                        static_cast<size_t>(visionWidth), static_cast<size_t>(visionHeight));
        return world;
    }, py::arg("width") = 100, py::arg("height") = 100,
       py::arg("visionWidth") = 16, py::arg("visionHeight") = 16,
       "Create a standard world with common parameters");

    // Factory for agent brain with world
    m.def("createAgentBrainWithWorld", [](std::shared_ptr<Brain> brain,
                                          std::shared_ptr<SimpleWorld> world) {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        return agentBrain;
    }, py::arg("brain"), py::arg("world"),
       "Create an agent brain and initialize it with a world");

    // ===================== UTILITY FUNCTIONS =====================

    // Type conversion utilities
    m.def("to_float_list", [](const py::list& pyList) {
        return pyList.cast<std::vector<float>>();
    }, "Convert Python list to float vector");

    m.def("to_int_list", [](const py::list& pyList) {
        return pyList.cast<std::vector<int>>();
    }, "Convert Python list to integer vector");

    m.def("to_string_list", [](const py::list& pyList) {
        return pyList.cast<std::vector<std::string>>();
    }, "Convert Python list to string vector");

    // Error creation utilities
    m.def("create_error", [](const std::string& message) {
        throw NLMError(message);
    }, py::arg("message"), "Create an NLM error (throws)");

    m.def("create_invalid_argument_error", [](const std::string& message) {
        throw InvalidArgumentError(message);
    }, py::arg("message"), "Create an invalid argument error (throws)");

    // Math utilities
    m.def("clamp", [](float value, float min, float max) {
        return std::clamp(value, min, max);
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Clamp a value between min and max");

    m.def("normalize", [](float value, float min, float max) {
        if (max <= min) throw InvalidArgumentError("max must be greater than min");
        return (value - min) / (max - min);
    }, py::arg("value"), py::arg("min"), py::arg("max"), "Normalize a value to [0,1] range");

    // Random utilities
    m.def("random_seed", []() {
        return static_cast<uint64_t>(std::random_device{}());
    }, "Generate a random seed");

    // Simulation utilities
    m.def("run_episode", [](std::shared_ptr<AgentBrain> agentBrain,
                           std::shared_ptr<SimpleWorld> world,
                           int maxSteps) {
        agentBrain->reset();
        float totalReward = 0.0f;
        
        for (int step = 0; step < maxSteps; ++step) {
            // Get current percept
            const auto& percept = world->getSensoryPercept();
            
            // Process sensory input
            agentBrain->processSensoryInput(percept);
            
            // Get action
            MotorCommand cmd = agentBrain->decodeMotorCommand();
            
            // Apply action
            double currentTime = world->getSimulationTime();
            ActionResult result = world->applyMotorCommand(cmd, currentTime);
            
            // Update brain with reward
            agentBrain->applyRewardModulation(result.reward, 0.0f);
            
            totalReward += result.reward;
            
            // Update world
            world->update(1.0); // Fixed timestep
            
            // Check if episode should end
            if (!world->getAgentBody().isMoving && world->getAgentBody().isTurning) {
                break; // Agent stopped
            }
        }
        
        return totalReward;
    }, py::arg("agentBrain"), py::arg("world"), py::arg("maxSteps"),
       "Run a complete simulation episode");

    // ===================== CONSTANTS =====================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Type conversion constants
    m.attr("NEURON_ID_INVALID") = INVALID_NEURON_ID.value;
    m.attr("SYNAPSE_ID_INVALID") = INVALID_SYNAPSE_ID.value;
    m.attr("REGION_ID_INVALID") = INVALID_REGION_ID.value;
    m.attr("POPULATION_ID_INVALID") = INVALID_POPULATION_ID.value;

    // ===================== DOCUMENTATION STRINGS =====================

    m.def("version", []() { return "2.0.0"; },
          "Get the NLM Python bindings version");

    m.def("description", []() {
        return "NLM (Neural Learning Machine) Python Bindings - Enhanced Version 2.0";
    });

    m.def("features", []() {
        return "Convenience functions, advanced API, type conversions, error handling, helpers, documentation, backward compatibility";
    });
}

} // namespace nlm