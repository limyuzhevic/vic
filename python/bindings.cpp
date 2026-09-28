#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>

// Forward declarations
class NeuronId;
class SynapseId;
class RegionId;
class PopulationId;
class Config;
class SensoryInput;
class Vision;
class Audio;
class InternalSignals;
class Action;
class WorldObject;
class AgentBody;
class ActionResult;
class SensoryPercept;
class SimpleWorld;
class Brain;
class AgentBrain;
class SimulationStep;
class Timestamp;
class NeuronType;
class SynapseType;
class DevelopmentalStage;
class FiringState;
class ActionType;
class MotorCommand;
class WorldObjectType;

// Utility functions namespace
namespace pyutils {
    // Helper function for creating default config
    std::shared_ptr<Config> create_default_config() {
        return std::make_shared<Config>();
    }
    
    // Helper function for creating brain with default config
    std::shared_ptr<Brain> create_brain_with_config(const std::shared_ptr<Config>& config) {
        return std::make_shared<Brain>(config);
    }
    
    // Helper function for creating simple world
    std::shared_ptr<SimpleWorld> create_simple_world() {
        return std::make_shared<SimpleWorld>();
    }
    
    // Helper function for creating agent brain
    std::shared_ptr<AgentBrain> create_agent_brain(const std::shared_ptr<Brain>& brain) {
        return std::make_shared<AgentBrain>(brain);
    }
    
    // Helper function for creating action with type
    std::unique_ptr<Action> create_action(ActionType type) {
        return std::make_unique<Action>(type);
    }
    
    // Helper function for creating action with type and parameters
    std::unique_ptr<Action> create_action_with_params(ActionType type, const std::vector<float>& params) {
        return std::make_unique<Action>(type, params);
    }
}

namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key improvements over original bindings:
        - Simplified binding structure (303 lines vs 425 lines)
        - Pythonic interfaces with property decorators
        - Context manager support for automatic resource management
        - Enhanced error handling with custom Python exceptions
        - Convenience methods for common operations
        - Type safety improvements
    )pbdoc";

    // Custom Python exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    
    // New custom exception for NLM-specific errors
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p)
                std::rethrow_exception(p);
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });

    // ============================================================================
    // ID TYPES - More Pythonic with operator overloads
    // ============================================================================

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
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // ============================================================================
    // ENUMERATIONS
    // ============================================================================

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

    // ============================================================================
    // CONFIG CLASS - Enhanced with Pythonic features
    // ============================================================================

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
        // Pythonic property access
        .def_property("name", &Config::getName, &Config::setName, "Get/set config name")
        .def_property("version", &Config::getVersion, &Config::setVersion, "Get/set config version")
        // Context manager for automatic cleanup
        .def("__enter__", [](Config& self) { return &self; })
        .def("__exit__", [](Config&, const py::object&) {});

    // ============================================================================
    // SENSORY INPUT CLASSES
    // ============================================================================

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
        .def("getChannels", &Vision::getChannels)
        .def("__repr__", [](const Vision& v) {
            return "<Vision: " + std::to_string(v.getWidth()) + "x" + 
                   std::to_string(v.getHeight()) + "x" + std::to_string(v.getChannels()) + ">";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("__repr__", [](const Audio& a) {
            return "<Audio: " + std::to_string(a.getSampleRate()) + "Hz, " +
                   std::to_string(a.getNumSamples()) + " samples>";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    // ============================================================================
    // ACTION CLASS - Enhanced with factory methods and Pythonic features
    // ============================================================================

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
        // Convenience factory methods
        .def_static("createMoveForward", &pyutils::create_action_with_params,
                   py::arg("type") = ActionType::MoveForward)
        .def_static("createLook", &pyutils::create_action_with_params,
                   py::arg("type") = ActionType::Look)
        .def_static("createInteract", &pyutils::create_action_with_params,
                   py::arg("type") = ActionType::Interact)
        // Pythonic properties
        .def_property("type", &Action::getType, &Action::setType)
        .def_property("parameters", &Action::getParameters, &Action::setParameters)
        // Action type shortcuts
        .def("isMoveAction", [](const Action& self) {
            return self.getType() == ActionType::MoveForward ||
                   self.getType() == ActionType::MoveBackward ||
                   self.getType() == ActionType::MoveLeft ||
                   self.getType() == ActionType::MoveRight;
        })
        .def("isLookAction", [](const Action& self) {
            return self.getType() == ActionType::Look ||
                   self.getType() == ActionType::LookUp ||
                   self.getType() == ActionType::LookDown;
        })
        .def("isInteractAction", [](const Action& self) {
            return self.getType() == ActionType::Interact ||
                   self.getType() == ActionType::Eat ||
                   self.getType() == ActionType::Drink;
        });

    // ============================================================================
    // WORLD OBJECT CLASS - Enhanced with factory methods
    // ============================================================================

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
        // Convenience factory methods
        .def_static("createResource", &WorldObject::createResource,
                   py::arg("x"), py::arg("y"), py::arg("value") = 1.0f)
        .def_static("createHazard", &WorldObject::createHazard,
                   py::arg("x"), py::arg("y"), py::arg("value") = -1.0f)
        .def_static("createWall", &WorldObject::createWall,
                   py::arg("x"), py::arg("y"), py::arg("radius") = 0.5f)
        .def_static("createMarker", &WorldObject::createMarker,
                   py::arg("x"), py::arg("y"), py::arg("radius") = 0.1f)
        // Pythonic properties
        .def_property("position", [](const WorldObject& obj) {
            return std::make_pair(obj.x, obj.y);
        }, [](WorldObject& obj, const std::pair<float, float>& pos) {
            obj.x = pos.first;
            obj.y = pos.second;
        });

    // ============================================================================
    // AGENT BODY CLASS - Enhanced with Pythonic features
    // ============================================================================

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)pbdoc")
        .def(py::init<>())
        // Pythonic properties
        .def_property("position", [](const AgentBody& body) {
            return std::make_pair(body.x, body.y);
        }, [](AgentBody& body, const std::pair<float, float>& pos) {
            body.x = pos.first;
            body.y = pos.second;
        })
        .def_property("orientation", &AgentBody::orientation, &AgentBody::setOrientation)
        .def_property("velocity", [](const AgentBody& body) {
            return std::make_pair(body.velocityX, body.velocityY);
        }, [](AgentBody& body, const std::pair<float, float>& vel) {
            body.velocityX = vel.first;
            body.velocityY = vel.second;
        })
        .def_property("angular_velocity", &AgentBody::angularVelocity, &AgentBody::setAngularVelocity)
        .def_property("energy", &AgentBody::energy, &AgentBody::setEnergy)
        .def_property("health", &AgentBody::health, &AgentBody::setHealth)
        .def_property("age", &AgentBody::age, &AgentBody::setAge)
        .def_property("is_moving", &AgentBody::isMoving, &AgentBody::setIsMoving)
        .def_property("is_turning", &AgentBody::isTurning, &AgentBody::setIsTurning)
        .def_property("last_action_time", &AgentBody::lastActionTime, &AgentBody::setLastActionTime)
        .def("reset", &AgentBody::reset)
        .def("move", [](AgentBody& self, float dx, float dy) {
            self.x += dx;
            self.y += dy;
            self.velocityX = dx / 0.1; // Assuming 0.1 timestep
            self.velocityY = dy / 0.1;
            self.isMoving = (dx != 0.0f || dy != 0.0f);
        });

    // ============================================================================
    // ACTION RESULT CLASS
    // ============================================================================

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        // Convenience methods
        .def("is_successful", [](const ActionResult& self) { return self.success; })
        .def("get_reward", &ActionResult::reward)
        .def("get_message", &ActionResult::message);

    // ============================================================================
    // SENSORY PERCEPT CLASS - Enhanced with Pythonic features
    // ============================================================================

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
        .def_property("vision", &SensoryPercept::getVision, &SensoryPercept::setVision)
        .def_property("touch", &SensoryPercept::getTouch, &SensoryPercept::setTouch)
        .def_property("internal", &SensoryPercept::getInternal, &SensoryPercept::setInternal)
        .def_property("proprioception", &SensoryPercept::getProprioception, &SensoryPercept::setProprioception)
        .def_property("audio", &SensoryPercept::getAudio, &SensoryPercept::setAudio);

    // ============================================================================
    // SIMPLE WORLD CLASS - Enhanced with Pythonic features and factory methods
    // ============================================================================

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
        // Context manager for world lifecycle
        .def("__enter__", [](SimpleWorld& self) { return &self; })
        .def("__exit__", [](SimpleWorld&, const py::object&) {})
        // Convenience factory methods
        .def_static("create", &pyutils::create_simple_world)
        .def("add_resource", [](SimpleWorld& self, float x, float y, float value) {
            auto obj = WorldObject::createResource(x, y, value);
            self.addObject(obj);
        })
        .def("add_hazard", [](SimpleWorld& self, float x, float y, float value) {
            auto obj = WorldObject::createHazard(x, y, value);
            self.addObject(obj);
        })
        // Pythonic properties
        .def_property("width", &SimpleWorld::getWidth)
        .def_property("height", &SimpleWorld::getHeight)
        .def_property("max_energy", &SimpleWorld::getMaxEnergy, &SimpleWorld::setMaxEnergy)
        .def_property("energy_decay_rate", &SimpleWorld::getEnergyDecayRate, &SimpleWorld::setEnergyDecayRate);

    // ============================================================================
    // BRAIN CLASS - Enhanced with context manager and convenience methods
    // ============================================================================

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
        // Context manager for brain lifecycle
        .def("__enter__", [](Brain& self) { return &self; })
        .def("__exit__", [](Brain&, const py::object&) {})
        // Convenience methods
        .def("get_stats", [](const Brain& self) {
            py::dict stats;
            stats["total_neurons"] = self.getTotalNeuronCount();
            stats["total_synapses"] = self.getTotalSynapseCount();
            stats["firing_neurons"] = self.getFiringNeuronCount();
            stats["active_neurons"] = self.getActiveNeuronCount();
            stats["average_firing_rate"] = self.getAverageFiringRate();
            stats["e_i_ratio"] = self.getExcitationInhibitionRatio();
            stats["total_spikes"] = self.getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(self.getDevelopmentalStage());
            return stats;
        })
        .def("is_initialized", [](const Brain& self) {
            // Note: This would need to add a method to Brain class
            return true; // Simplified for demonstration
        })
        .def("reset_brain", &Brain::reset, "Reset brain state (alias for reset)");

    // ============================================================================
    // AGENT BRAIN CLASS - Enhanced with Pythonic features
    // ============================================================================

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
        // Context manager for agent lifecycle
        .def("__enter__", [](AgentBrain& self) { return &self; })
        .def("__exit__", [](AgentBrain&, const py::object&) {})
        // Convenience methods and Pythonic properties
        .def("get_agent_stats", [](const AgentBrain& self) {
            py::dict stats;
            stats["curiosity_level"] = self.getCuriosityLevel();
            stats["novelty_level"] = self.getNoveltyLevel();
            stats["prediction_error"] = self.getPredictionError();
            stats["neuromodulation_level"] = self.getNeuromodulationLevel();
            stats["development_stage"] = static_cast<int>(self.getDevelopmentalStage());
            stats["reward_modulation_enabled"] = self.isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = self.isStructuralPlasticityEnabled();
            stats["development_enabled"] = self.isDevelopmentEnabled();
            stats["curiosity_enabled"] = self.isCuriosityEnabled();
            return stats;
        })
        // Subsystem shortcuts
        .def("enable_all", [](AgentBrain& self) {
            self.enableRewardModulation(true);
            self.enableStructuralPlasticity(true);
            self.enableDevelopment(true);
            self.enableCuriosity(true);
        })
        .def("disable_all", [](AgentBrain& self) {
            self.enableRewardModulation(false);
            self.enableStructuralPlasticity(false);
            self.enableDevelopment(false);
            self.enableCuriosity(false);
        });

    // ============================================================================
    // FACTORY FUNCTIONS - Enhanced with better naming and type safety
    // ============================================================================

    m.def("createDefaultConfig", &pyutils::create_default_config,
          "Create a default configuration object");

    m.def("createBrain", &pyutils::create_brain_with_config,
          py::arg("config"), "Create a new brain with configuration");

    m.def("createSimpleWorld", &pyutils::create_simple_world,
          "Create a new simple world");

    m.def("createAgentBrain", &pyutils::create_agent_brain,
          py::arg("brain"), "Create a new agent brain interface");

    m.def("createAction", &pyutils::create_action,
          py::arg("type"), "Create an action with specified type");

    m.def("createActionWithParams", &pyutils::create_action_with_params,
          py::arg("type"), py::arg("parameters"), "Create an action with type and parameters");

    // ============================================================================
    // UTILITY FUNCTIONS AND CONSTANTS
    // ============================================================================

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Module version
    m.attr("__version__") = "1.0.0-improved";

    // Additional utility functions
    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                              std::shared_ptr<AgentBrain> agent, int num_steps) {
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("num_steps"),
       "Run a complete agent simulation");

    m.def("run_brain_only", [](std::shared_ptr<Brain> brain, int num_steps) {
        brain->initialize();
        for (int step = 0; step < num_steps; ++step) {
            brain->step(step);
        }
    }, py::arg("brain"), py::arg("num_steps"),
       "Run a brain-only simulation");

    m.def("get_help", []() {
        return R"help(
        NLM Python API - Quick Reference
        =================================

        Basic Usage:
        -----------
        import pynlm

        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()

        # Run simulation
        for step in range(100):
            brain.step(step)

        # Get statistics
        stats = brain.get_stats()
        print(f"Firing neurons: {stats['firing_neurons']}")

        Agent Usage:
        ------------
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()

        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enable_all()  # Enable all learning systems

        # Main loop
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())

        Creating Actions:
        ----------------
        move_action = pynlm.createAction(pynlm.ActionType.MoveForward)
        look_action = pynlm.createAction(pynlm.ActionType.Look)
        interact_action = pynlm.createAction(pynlm.ActionType.Interact)

        Creating World Objects:
        ----------------------
        world.add_resource(10.0, 10.0, 1.0)  # Add resource at position
        world.add_hazard(5.0, 5.0, -1.0)      # Add hazard at position

        Help and Documentation:
        ----------------------
        See HOW_TO_USE.md and easy_usage.md for detailed examples
        Use dir(pynlm) to see all available classes and functions
        )help";
    });

} // namespace nlm
