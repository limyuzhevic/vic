#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <optional>
#include <sstream>

// Forward declarations for helper classes
class PyConfigContext;
class PyBrainContext;
class PyWorldContext;
class PySimulationRunner;

// Include all NLM headers
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

// ===== PYTHONIC EXCEPTIONS =====

// Custom exception for Python users
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& message) : std::runtime_error(message) {}
};

class ConfigError : public NLMException {
public:
    explicit ConfigError(const std::string& message) : NLMException("Config error: " + message) {}
};

class BrainError : public NLMException {
public:
    explicit BrainError(const std::string& message) : NLMException("Brain error: " + message) {}
};

class WorldError : public NLMException {
public:
    explicit WorldError(const std::string& message) : NLMException("World error: " + message) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& message) : NLMException("Simulation error: " + message) {}
};

// ===== CORE TYPES & IDENTIFIERS WITH PYTHONIC FEATURES =====

void defineNeuronId(py::module_& m) {
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value, R"pbdoc(Get or set the neuron ID value)pbdoc")
        .def("index", &NeuronId::index, R"pbdoc(Get the neuron index)pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        // Pythonic properties
        .def_property_readonly("is_valid", [](const NeuronId& id) { return id.value != UINT64_MAX; });
}

void defineSynapseId(py::module_& m) {
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
}

void defineRegionId(py::module_& m) {
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
}

void definePopulationId(py::module_& m) {
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);
}

// ===== ENUMERATIONS WITH PYTHONIC HELPERS =====

void defineNeuronTypes(py::module_& m) {
    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values()
        .def("is_sensory_or_motor", [](NeuronType type) {
            return type == NeuronType::Sensory || type == NeuronType::Motor;
        }, R"pbdoc(Check if neuron type is sensory or motor)pbdoc")
        .def("is_excitatory_or_inhibitory", [](NeuronType type) {
            return type == NeuronType::Excitatory || type == NeuronType::Inhibitory;
        }, R"pbdoc(Check if neuron type is excitatory or inhibitory)pbdoc");
}

void defineSynapseTypes(py::module_& m) {
    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("is_significant", [](SynapseType type) {
            return type == SynapseType::Excitatory || type == SynapseType::Inhibitory || type == SynapseType::Modulatory;
        }, R"pbdoc(Check if synapse type is significant for neural dynamics)pbdoc");
}

void defineDevelopmentalStages(py::module_& m) {
    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("is_developing", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Initial || stage == DevelopmentalStage::CriticalPeriod || stage == DevelopmentalStage::Maturation;
        }, R"pbdoc(Check if in developmental phase)pbdoc")
        .def("is_mature", [](DevelopmentalStage stage) {
            return stage == DevelopmentalStage::Adult || stage == DevelopmentalStage::Aging;
        }, R"pbdoc(Check if mature adult)pbdoc");
}

void defineFiringStates(py::module_& m) {
    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("is_active_state", [](FiringState state) {
            return state == FiringState::Active;
        }, R"pbdoc(Check if neuron is currently firing)pbdoc")
        .def("is_passive_state", [](FiringState state) {
            return state == FiringState::Resting || state == FiringState::Inhibited;
        }, R"pbdoc(Check if neuron is in passive state)pbdoc");
}

void defineActionTypes(py::module_& m) {
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
        .export_values()
        .def("is_movement", [](ActionType type) {
            return type == ActionType::MoveForward || type == ActionType::MoveBackward ||
                   type == ActionType::MoveLeft || type == ActionType::MoveRight ||
                   type == ActionType::TurnLeft || type == ActionType::TurnRight;
        }, R"pbdoc(Check if action is a movement)pbdoc")
        .def("is_visual", [](ActionType type) {
            return type == ActionType::Look || type == ActionType::LookUp || type == ActionType::LookDown;
        }, R"pbdoc(Check if action is visual)pbdoc");
}

void defineMotorCommands(py::module_& m) {
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
}

void defineWorldObjectTypes(py::module_& m) {
    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("is_resource_or_hazard", [](WorldObjectType type) {
            return type == WorldObjectType::Resource || type == WorldObjectType::Hazard;
        }, R"pbdoc(Check if object is a resource or hazard)pbdoc")
        .def("is_passable", [](WorldObjectType type) {
            return type == WorldObjectType::Empty || type == WorldObjectType::Resource || type == WorldObjectType::Marker;
        }, R"pbdoc(Check if object is passable)pbdoc");
}

// ===== CONFIG SYSTEM WITH FLUENT INTERFACE =====

void defineConfig(py::module_& m) {
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
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // Fluent interface methods for Config
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration");

    // Configuration helper functions
    m.def("load_config_from_json", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigError("Failed to load config from " + filepath);
        }
        return config;
    }, py::arg("filepath"), "Create and load configuration from JSON file");

    m.def("create_config_with_defaults", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set some sensible defaults
        config->set("brain.region_count", 1);
        config->set("brain.initial_neurons_per_region", 100);
        config->set("simulation.max_steps", 1000);
        config->set("world.width", 20.0f);
        config->set("world.height", 20.0f);
        return config;
    }, "Create configuration with sensible defaults");
}

// ===== SENSORY SYSTEM WITH CONTEXT MANAGER =====

void defineSensoryInput(py::module_& m) {
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
}

void defineVision(py::module_& m) {
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
        // Pythonic properties
        .def_property_readonly("shape", [](const Vision& self) {
            return std::vector<size_t>{self.getHeight(), self.getWidth(), self.getChannels()};
        }, R"pbdoc(Get vision data shape as (height, width, channels))pbdoc");
}

void defineAudio(py::module_& m) {
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
}

void defineInternalSignals(py::module_& m) {
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);
}

// ===== MOTOR SYSTEM WITH PROPERTIES =====

void defineAction(py::module_& m) {
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
        // Pythonic properties
        .def_property("type", &Action::getType, &Action::setType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        .def_property_readonly("is_movement", [](const Action& self) {
            return self.getType() == ActionType::MoveForward ||
                   self.getType() == ActionType::MoveBackward ||
                   self.getType() == ActionType::MoveLeft ||
                   self.getType() == ActionType::MoveRight ||
                   self.getType() == ActionType::TurnLeft ||
                   self.getType() == ActionType::TurnRight;
        }, R"pbdoc(Check if action is a movement)pbdoc")
        .def("__repr__", [](const Action& action) {
            return "<Action: " + action.getName() + " parameters=" + std::to_string(action.getParameters().size()) + ">";
        });
}

void defineWorldObject(py::module_& m) {
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
        // Pythonic properties
        .def_property_readonly("is_resource", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Resource;
        }, R"pbdoc(Check if object is a resource)pbdoc")
        .def_property_readonly("is_hazard", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Hazard;
        }, R"pbdoc(Check if object is a hazard)pbdoc")
        .def_property_readonly("is_passable", [](const WorldObject& obj) {
            return obj.type == WorldObjectType::Empty || obj.type == WorldObjectType::Resource || obj.type == WorldObjectType::Marker;
        }, R"pbdoc(Check if object can be traversed)pbdoc")
        .def("distance_to", [](const WorldObject& obj, float x, float y) {
            return std::hypot(obj.x - x, obj.y - y);
        }, py::arg("x"), py::arg("y"), R"pbdoc(Calculate Euclidean distance to position)pbdoc");
}

void defineAgentBody(py::module_& m) {
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

    // Speed-related properties
    m.def("calculate_speed", [](const AgentBody& body) {
        return std::hypot(body.velocityX, body.velocityY);
    }, R"pbdoc(Calculate current speed)pbdoc");

    m.def("calculate_direction", [](const AgentBody& body) {
        return std::atan2(body.velocityY, body.velocityX) * 180.0 / M_PI;
    }, R"pbdoc(Calculate current direction in degrees)pbdoc");
}

void defineActionResult(py::module_& m) {
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Pythonic properties
        .def_property_readonly("is_success", [](const ActionResult& result) {
            return result.success;
        }, R"pbdoc(Check if action was successful)pbdoc")
        .def_property_readonly("has_reward", [](const ActionResult& result) {
            return result.reward != 0.0f;
        }, R"pbdoc(Check if action produced reward)pbdoc");
}

// ===== SENSORY PERCEPT SYSTEM =====

void defineSensoryPercept(py::module_& m) {
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
        // Pythonic properties
        .def_property_readonly("vision_shape", [](const SensoryPercept& self) {
            return std::vector<size_t>{self.getVisionHeight(), self.getVisionWidth()};
        }, R"pbdoc(Get vision shape as (height, width))pbdoc")
        .def_property_readonly("has_vision", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, R"pbdoc(Check if percept has vision data)pbdoc")
        .def("get_normalized_vision", [](const SensoryPercept& self) {
            auto vision = self.getVision();
            if (vision.empty()) return std::vector<float>{};
            float min_val = *std::min_element(vision.begin(), vision.end());
            float max_val = *std::max_element(vision.begin(), vision.end());
            if (max_val - min_val < 1e-6) return std::vector<float>(vision.size(), 0.0f);
            std::vector<float> normalized(vision.size());
            for (size_t i = 0; i < vision.size(); ++i) {
                normalized[i] = (vision[i] - min_val) / (max_val - min_val);
            }
            return normalized;
        }, R"pbdoc(Get normalized vision data (0-1))pbdoc");
}

// ===== WORLD SYSTEM WITH CONTEXT MANAGER =====

void defineSimpleWorld(py::module_& m) {
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
        .def("getObjectAt", &SimpleWorld::getObjectAt, py::arg("x"), py::arg("y"),
             py::return_value_policy::reference_internal)
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: " + std::to_string(world.getWidth()) + "x" + std::to_string(world.getHeight()) + ">";
        });

    // World helper functions
    m.def("create_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new world");

    m.def("configure_world", [](std::shared_ptr<SimpleWorld> world, float width, float height,
                               size_t visionWidth, size_t visionHeight) {
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("world"), py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"),
       "Configure an existing world or create a new one");

    m.def("add_resource_to_world", [](std::shared_ptr<SimpleWorld> world, float x, float y, float value = 1.0f) {
        WorldObject obj(x, y, WorldObjectType::Resource, value);
        world->addObject(obj);
        return world;
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value") = 1.0f,
       "Add a resource to the world");

    m.def("add_hazard_to_world", [](std::shared_ptr<SimpleWorld> world, float x, float y, float value = -1.0f) {
        WorldObject obj(x, y, WorldObjectType::Hazard, value);
        world->addObject(obj);
        return world;
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("value") = -1.0f,
       "Add a hazard to the world");

    m.def("add_wall_to_world", [](std::shared_ptr<SimpleWorld> world, float x, float y, float radius = 0.5f) {
        WorldObject obj(x, y, WorldObjectType::Wall, 0.0f, radius);
        world->addObject(obj);
        return world;
    }, py::arg("world"), py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f,
       "Add a wall to the world");
}

// ===== BRAIN SYSTEM WITH CONTEXT MANAGER =====

void defineBrain(py::module_& m) {
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
        // Pythonic properties
        .def_property_readonly("region_count", &Brain::getRegionCount)
        .def_property_readonly("total_neuron_count", &Brain::getTotalNeuronCount)
        .def_property_readonly("total_synapse_count", &Brain::getTotalSynapseCount)
        .def_property_readonly("active_neuron_count", &Brain::getActiveNeuronCount)
        .def_property_readonly("firing_neuron_count", &Brain::getFiringNeuronCount)
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate)
        .def_property_readonly("excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio)
        .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount)
        .def_property_readonly("is_initialized", [](const Brain& brain) {
            // This would need a new method in Brain class
            return false; // Placeholder
        }, "Check if brain is initialized")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: " + std::to_string(brain.getTotalNeuronCount()) + " neurons, " +
                   std::to_string(brain.getTotalSynapseCount()) + " synapses>";
        });

    // Brain helper functions
    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");

    m.def("create_brain_with_defaults", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.region_count", 1);
        config->set("brain.initial_neurons_per_region", 100);
        return std::make_shared<Brain>(config);
    }, "Create a brain with sensible defaults");

    // Brain simulation helpers
    m.def("run_brain_step", [](std::shared_ptr<Brain> brain, SimulationStep step, std::optional<Timestamp> time = std::nullopt) {
        if (time.has_value()) {
            brain->step(step, time.value());
        } else {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("step"), py::arg("time") = std::nullopt,
       "Run a single brain simulation step");
}

// ===== AGENT BRAIN SYSTEM =====

void defineAgentBrain(py::module_& m) {
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // AgentBrain helper functions
    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("initialize_agent", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
        agent->initialize(*world);
        return agent;
    }, py::arg("agent"), py::arg("world"),
       "Initialize an agent brain with a world");
}

// ===== GLOBAL CONSTANTS AND FACTORIES =====

void defineGlobalConstants(py::module_& m) {
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Pythonic constants
    m.attr("PI") = 3.14159265358979323846;
    m.attr("EPSILON") = 1e-6;
}

// ===== CONTEXT MANAGERS =====

// Configuration Context Manager
class PyConfigContext {
private:
    std::shared_ptr<Config> config_;
    bool auto_save_;
    std::string backup_path_;
public:
    PyConfigContext(std::shared_ptr<Config> config, bool auto_save = false, const std::string& backup_path = "")
        : config_(config), auto_save_(auto_save), backup_path_(backup_path) {}

    ~PyConfigContext() {
        if (auto_save_ && backup_path_ && config_) {
            config_->saveToFile(backup_path_);
        }
    }

    std::shared_ptr<Config> get_config() { return config_; }
    void set_auto_save(bool auto_save) { auto_save_ = auto_save; }
    bool get_auto_save() const { return auto_save_; }
};

// Brain Context Manager
class PyBrainContext {
private:
    std::shared_ptr<Brain> brain_;
    bool auto_save_;
    std::string checkpoint_path_;
    std::chrono::steady_clock::time_point start_time_;
public:
    PyBrainContext(std::shared_ptr<Brain> brain, bool auto_save = false, const std::string& checkpoint_path = "")
        : brain_(brain), auto_save_(auto_save), checkpoint_path_(checkpoint_path) {
        start_time_ = std::chrono::steady_clock::now();
    }

    ~PyBrainContext() {
        if (auto_save_ && checkpoint_path_ && brain_) {
            brain_->save(checkpoint_path_);
        }
    }

    std::shared_ptr<Brain> get_brain() { return brain_; }
    double get_elapsed_time() const {
        auto end = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start_time_);
        return duration.count();
    }
    void set_auto_save(bool auto_save) { auto_save_ = auto_save; }
    bool get_auto_save() const { return auto_save_; }
};

// World Context Manager
class PyWorldContext {
private:
    std::shared_ptr<SimpleWorld> world_;
    bool reset_on_exit_;
public:
    PyWorldContext(std::shared_ptr<SimpleWorld> world, bool reset_on_exit = true)
        : world_(world), reset_on_exit_(reset_on_exit) {}

    ~PyWorldContext() {
        if (reset_on_exit_ && world_) {
            world_->reset();
        }
    }

    std::shared_ptr<SimpleWorld> get_world() { return world_; }
    void set_reset_on_exit(bool reset) { reset_on_exit_ = reset; }
    bool get_reset_on_exit() const { return reset_on_exit_; }
};

// Simulation Runner Helper
class PySimulationRunner {
private:
    std::shared_ptr<Brain> brain_;
    std::shared_ptr<SimpleWorld> world_;
    std::shared_ptr<AgentBrain> agent_;
    std::vector<ActionResult> action_history_;
    std::vector<Timestamp> time_history_;
    size_t max_steps_;
public:
    PySimulationRunner(std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                      std::shared_ptr<AgentBrain> agent, size_t max_steps = 1000)
        : brain_(brain), world_(world), agent_(agent), max_steps_(max_steps) {}

    std::pair<std::vector<ActionResult>, std::vector<Timestamp>> run() {
        for (size_t i = 0; i < max_steps_ && brain_ && world_ && agent_; ++i) {
            // Run a single simulation step
            if (brain_) {
                brain_->step(i, world_->getSimulationTime());
            }
            if (world_) {
                auto percept = world_->getSensoryPercept();
                if (agent_) {
                    agent_->processSensoryInput(percept);
                    auto action = agent_->decodeMotorCommand();
                    auto result = world_->applyMotorCommand(action, world_->getSimulationTime());
                    action_history_.push_back(result);
                    time_history_.push_back(world_->getSimulationTime());
                }
            }
            if (world_) {
                world_->update(0.1); // Fixed timestep
            }
        }
        return {action_history_, time_history_};
    }

    size_t get_steps_completed() const { return action_history_.size(); }
    double get_total_reward() const {
        double total = 0.0;
        for (const auto& result : action_history_) {
            total += result.reward;
        }
        return total;
    }
};

void defineContextManagers(py::module_& m) {
    // Configuration context manager
    py::class_<PyConfigContext>(m, "ConfigContext")
        .def(py::init<std::shared_ptr<Config>, bool, std::string>(),
             py::arg("config"), py::arg("auto_save") = false, py::arg("backup_path") = "")
        .def("get_config", &PyConfigContext::get_config)
        .def("set_auto_save", &PyConfigContext::set_auto_save)
        .def("get_auto_save", &PyConfigContext::get_auto_save);

    // Brain context manager
    py::class_<PyBrainContext>(m, "BrainContext")
        .def(py::init<std::shared_ptr<Brain>, bool, std::string>(),
             py::arg("brain"), py::arg("auto_save") = false, py::arg("checkpoint_path") = "")
        .def("get_brain", &PyBrainContext::get_brain)
        .def("get_elapsed_time", &PyBrainContext::get_elapsed_time)
        .def("set_auto_save", &PyBrainContext::set_auto_save)
        .def("get_auto_save", &PyBrainContext::get_auto_save);

    // World context manager
    py::class_<PyWorldContext>(m, "WorldContext")
        .def(py::init<std::shared_ptr<SimpleWorld>, bool>(),
             py::arg("world"), py::arg("reset_on_exit") = true)
        .def("get_world", &PyWorldContext::get_world)
        .def("set_reset_on_exit", &PyWorldContext::set_reset_on_exit)
        .def("get_reset_on_exit", &PyWorldContext::get_reset_on_exit);

    // Simulation runner helper
    py::class_<PySimulationRunner>(m, "SimulationRunner")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>, size_t>(),
             py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("max_steps") = 1000)
        .def("run", &PySimulationRunner::run)
        .def("get_steps_completed", &PySimulationRunner::get_steps_completed)
        .def("get_total_reward", &PySimulationRunner::get_total_reward);

    // Python context manager support
    py::class_<PyConfigContext>(m, "config_context")
        .def(py::init<std::shared_ptr<Config>, bool, std::string>(),
             py::arg("config"), py::arg("auto_save") = false, py::arg("backup_path") = "")
        .def("__enter__", &PyConfigContext::get_config)
        .def("__exit__", [](PyConfigContext&, py::object) {
            // Cleanup handled in destructor
        });

    py::class_<PyBrainContext>(m, "brain_context")
        .def(py::init<std::shared_ptr<Brain>, bool, std::string>(),
             py::arg("brain"), py::arg("auto_save") = false, py::arg("checkpoint_path") = "")
        .def("__enter__", &PyBrainContext::get_brain)
        .def("__exit__", [](PyBrainContext&, py::object) {
            // Cleanup handled in destructor
        });

    py::class_<PyWorldContext>(m, "world_context")
        .def(py::init<std::shared_ptr<SimpleWorld>, bool>(),
             py::arg("world"), py::arg("reset_on_exit") = true)
        .def("__enter__", &PyWorldContext::get_world)
        .def("__exit__", [](PyWorldContext&, py::object) {
            // Cleanup handled in destructor
        });
}

// ===== DATA UTILITIES =====

void defineDataUtilities(py::module_& m) {
    // Vector normalization
    m.def("normalize_vector", [](const std::vector<float>& vec) {
        if (vec.empty()) return std::vector<float>{};
        float min_val = *std::min_element(vec.begin(), vec.end());
        float max_val = *std::max_element(vec.begin(), vec.end());
        if (max_val - min_val < 1e-6) return std::vector<float>(vec.size(), 0.0f);
        std::vector<float> normalized(vec.size());
        for (size_t i = 0; i < vec.size(); ++i) {
            normalized[i] = (vec[i] - min_val) / (max_val - min_val);
        }
        return normalized;
    }, R"pbdoc(Normalize vector to range [0, 1])pbdoc", py::arg("vector"));

    // Convert to numpy-like array (as vector)
    m.def("to_fvec", [](const py::array_t<float>& arr) {
        auto buf = arr.request();
        float* ptr = (float*)buf.ptr;
        return std::vector<float>(ptr, ptr + buf.size);
    }, R"pbdoc(Convert numpy array to vector)pbdoc", py::arg("array"));

    // Vector statistics
    m.def("vector_stats", [](const std::vector<float>& vec) {
        std::map<std::string, double> stats;
        if (vec.empty()) return stats;
        stats["mean"] = std::accumulate(vec.begin(), vec.end(), 0.0) / vec.size();
        float min_val = *std::min_element(vec.begin(), vec.end());
        float max_val = *std::max_element(vec.begin(), vec.end());
        stats["min"] = min_val;
        stats["max"] = max_val;
        stats["range"] = max_val - min_val;
        float sum_sq = 0.0f;
        for (float v : vec) sum_sq += (v - stats["mean"]) * (v - stats["mean"]);
        stats["std"] = std::sqrt(sum_sq / vec.size());
        stats["size"] = vec.size();
        return stats;
    }, R"pbdoc(Calculate vector statistics)pbdoc", py::arg("vector"));

    // Safe conversion functions
    m.def("safe_get", [](const Config& config, const std::string& key, double default_value) {
        auto value = config.get<double>(key);
        return value ? value.value() : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = 0.0,
       "Safely get double value from config with default");

    m.def("safe_get_string", [](const Config& config, const std::string& key, const std::string& default_value) {
        auto value = config.get<std::string>(key);
        return value ? value.value() : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = "",
       "Safely get string value from config with default");

    m.def("safe_get_bool", [](const Config& config, const std::string& key, bool default_value) {
        auto value = config.get<bool>(key);
        return value ? value.value() : default_value;
    }, py::arg("config"), py::arg("key"), py::arg("default_value") = false,
       "Safely get bool value from config with default");

    // Serialization helpers
    m.def("serialize_config", [](const Config& config) {
        std::stringstream ss;
        ss << config.summary();
        return ss.str();
    }, R"pbdoc(Serialize config to string)pbdoc", py::arg("config"));

    // Debug utilities
    m.def("debug_percept", [](const SensoryPercept& percept) {
        std::stringstream ss;
        ss << "Vision: " << percept.getVisionWidth() << "x" << percept.getVisionHeight()
           << ", Touch: " << percept.getTouch().size()
           << ", Internal: " << percept.getInternal().size()
           << ", Proprioception: " << percept.getProprioception().size()
           << ", Audio: " << percept.getAudio().size();
        return ss.str();
    }, R"pbdoc(Debug sensory percept)pbdoc", py::arg("percept"));
}

// ===== MAIN MODULE DEFINITION =====

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python API for the NLM neural simulation framework.

        This module provides:
        - Core neural types and identifiers (NeuronId, SynapseId, etc.)
        - Enumerations for neuron/synapse types, developmental stages, etc.
        - Configuration system with fluent interface
        - Sensory input (Vision, Audio, InternalSignals)
        - Motor output (Action, WorldObject, AgentBody)
        - Action results and sensory percepts
        - World simulation (SimpleWorld)
        - Brain neural computation (Brain)
        - Agent brain interface (AgentBrain)
        - Context managers for resource management
        - Simulation helpers and data utilities
        - Custom exceptions for error handling

        Features:
        * Pythonic properties and getters/setters
        * Context managers for automatic resource cleanup
        * Fluent interface for configuration
        * Data utilities for vector operations
        * Comprehensive error handling
        * Backward compatibility with existing API
    )pbdoc";

    // Register Pythonic exceptions
    py::register_exception<NLMException>(m, "NLMException", PyExc_RuntimeError);
    py::register_exception<ConfigError>(m, "ConfigError", PyExc_RuntimeError);
    py::register_exception<BrainError>(m, "BrainError", PyExc_RuntimeError);
    py::register_exception<WorldError>(m, "WorldError", PyExc_RuntimeError);
    py::register_exception<SimulationError>(m, "SimulationError", PyExc_RuntimeError);

    // Register core types with Pythonic features
    defineNeuronId(m);
    defineSynapseId(m);
    defineRegionId(m);
    definePopulationId(m);

    // Register enumerations with helper methods
    defineNeuronTypes(m);
    defineSynapseTypes(m);
    defineDevelopmentalStages(m);
    defineFiringStates(m);
    defineActionTypes(m);
    defineMotorCommands(m);
    defineWorldObjectTypes(m);

    // Register enhanced Config system
    defineConfig(m);

    // Register sensory system
    defineSensoryInput(m);
    defineVision(m);
    defineAudio(m);
    defineInternalSignals(m);

    // Register motor system with Pythonic properties
    defineAction(m);
    defineWorldObject(m);
    defineAgentBody(m);
    defineActionResult(m);

    // Register sensory percept system
    defineSensoryPercept(m);

    // Register world system with context manager support
    defineSimpleWorld(m);

    // Register brain system with Pythonic properties and context manager
    defineBrain(m);

    // Register agent brain system
    defineAgentBrain(m);

    // Register global constants and factory functions
    defineGlobalConstants(m);

    // Register context managers
    defineContextManagers(m);

    // Register data utilities
    defineDataUtilities(m);

    // Preserve backward compatibility - existing factory functions
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

    // Preserve global constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm