# Enhanced NLM Python Bindings
// Modern pybind11 API with better error handling, type safety, and convenience methods

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <pybind11/operators.h>
#include <memory>
#include <string>
#include <vector>
#include <sstream>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/sensory/Vision.hpp"
#include "../src/sensory/Audio.hpp"
#include "../src/sensory/InternalSignals.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions
class NLMException : public std::runtime_error {
public:
    explicit NLMException(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMException {
public:
    explicit ConfigurationError(const std::string& msg) : NLMException("Configuration Error: " + msg) {}
};

class InitializationError : public NLMException {
public:
    explicit InitializationError(const std::string& msg) : NLMException("Initialization Error: " + msg) {}
};

class SimulationError : public NLMException {
public:
    explicit SimulationError(const std::string& msg) : NLMException("Simulation Error: " + msg) {}
};

// Helper functions for better Python integration
std::string to_string(const NeuronId& id) {
    return "<NeuronId: " + std::to_string(id.value) + ">";
}

std::string to_string(const SynapseId& id) {
    return "<SynapseId: " + std::to_string(id.value) + ">";
}

std::string to_string(const RegionId& id) {
    return "<RegionId: " + std::to_string(id.value) + ">";
}

std::string to_string(const PopulationId& id) {
    return "<PopulationId: " + std::to_string(id.value) + ">";
}

// Enhanced NeuronId class with Python-friendly interface
void bindNeuronId(py::class_<NeuronId>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) { return to_string(id); })
        .def("__str__", [](const NeuronId& id) { return to_string(id); });
}

// Enhanced SynapseId class with Python-friendly interface
void bindSynapseId(py::class_<SynapseId>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) { return to_string(id); })
        .def("__str__", [](const SynapseId& id) { return to_string(id); });
}

// Enhanced RegionId class with Python-friendly interface
void bindRegionId(py::class_<RegionId>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) { return to_string(id); })
        .def("__str__", [](const RegionId& id) { return to_string(id); });
}

// Enhanced PopulationId class with Python-friendly interface
void bindPopulationId(py::class_<PopulationId>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def(py::self == py::self)
        .def(py::self != py::self)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); });
}

// Enhanced NeuronType enum with Python-friendly access
void bindNeuronType(py::enum_<NeuronType>& e) {
    e.value("EXCITATORY", NeuronType::Excitatory)
       .value("INHIBITORY", NeuronType::Inhibitory)
       .value("MODULATORY", NeuronType::Modulatory)
       .value("SENSORY", NeuronType::Sensory)
       .value("MOTOR", NeuronType::Motor)
       .value("INTERNAL", NeuronType::Internal)
       .export_values()
       .def("__repr__", [](NeuronType t) {
           switch (t) {
               case NeuronType::Excitatory: return "NeuronType.Excitatory";
               case NeuronType::Inhibitory: return "NeuronType.Inhibitory";
               case NeuronType::Modulatory: return "NeuronType.Modulatory";
               case NeuronType::Sensory: return "NeuronType.Sensory";
               case NeuronType::Motor: return "NeuronType.Motor";
               case NeuronType::Internal: return "NeuronType.Internal";
               default: return "NeuronType.Unknown";
           }
       });
}

// Enhanced SynapseType enum with Python-friendly access
void bindSynapseType(py::enum_<SynapseType>& e) {
    e.value("EXCITATORY", SynapseType::Excitatory)
       .value("INHIBITORY", SynapseType::Inhibitory)
       .value("MODULATORY", SynapseType::Modulatory)
       .value("ELECTRICAL", SynapseType::Electrical)
       .value("GAP_JUNCTION", SynapseType::GapJunction)
       .export_values()
       .def("__repr__", [](SynapseType t) {
           switch (t) {
               case SynapseType::Excitatory: return "SynapseType.Excitatory";
               case SynapseType::Inhibitory: return "SynapseType.Inhibitory";
               case SynapseType::Modulatory: return "SynapseType.Modulatory";
               case SynapseType::Electrical: return "SynapseType.Electrical";
               case SynapseType::GapJunction: return "SynapseType.GapJunction";
               default: return "SynapseType.Unknown";
           }
       });
}

// Enhanced DevelopmentalStage enum with Python-friendly access
void bindDevelopmentalStage(py::enum_<DevelopmentalStage>& e) {
    e.value("INITIAL", DevelopmentalStage::Initial)
       .value("CRITICAL_PERIOD", DevelopmentalStage::CriticalPeriod)
       .value("MATURATION", DevelopmentalStage::Maturation)
       .value("ADULT", DevelopmentalStage::Adult)
       .value("AGING", DevelopmentalStage::Aging)
       .export_values()
       .def("__repr__", [](DevelopmentalStage s) {
           switch (s) {
               case DevelopmentalStage::Initial: return "DevelopmentalStage.Initial";
               case DevelopmentalStage::CriticalPeriod: return "DevelopmentalStage.CriticalPeriod";
               case DevelopmentalStage::Maturation: return "DevelopmentalStage.Maturation";
               case DevelopmentalStage::Adult: return "DevelopmentalStage.Adult";
               case DevelopmentalStage::Aging: return "DevelopmentalStage.Aging";
               default: return "DevelopmentalStage.Unknown";
           }
       });
}

// Enhanced FiringState enum with Python-friendly access
void bindFiringState(py::enum_<FiringState>& e) {
    e.value("RESTING", FiringState::Resting)
       .value("ACTIVE", FiringState::Active)
       .value("REFRACTORY", FiringState::Refractory)
       .value("INHIBITED", FiringState::Inhibited)
       .export_values()
       .def("__repr__", [](FiringState s) {
           switch (s) {
               case FiringState::Resting: return "FiringState.Resting";
               case FiringState::Active: return "FiringState.Active";
               case FiringState::Refractory: return "FiringState.Refractory";
               case FiringState::Inhibited: return "FiringState.Inhibited";
               default: return "FiringState.Unknown";
           }
       });
}

// Enhanced ActionType enum with Python-friendly access
void bindActionType(py::enum_<ActionType>& e) {
    e.value("MOVE_FORWARD", ActionType::MoveForward)
       .value("MOVE_BACKWARD", ActionType::MoveBackward)
       .value("MOVE_LEFT", ActionType::MoveLeft)
       .value("MOVE_RIGHT", ActionType::MoveRight)
       .value("TURN_LEFT", ActionType::TurnLeft)
       .value("TURN_RIGHT", ActionType::TurnRight)
       .value("LOOK", ActionType::Look)
       .value("LOOK_UP", ActionType::LookUp)
       .value("LOOK_DOWN", ActionType::LookDown)
       .value("INTERACT", ActionType::Interact)
       .value("EAT", ActionType::Eat)
       .value("DRINK", ActionType::Drink)
       .value("REST", ActionType::Rest)
       .value("WAIT", ActionType::Wait)
       .value("CUSTOM", ActionType::Custom)
       .export_values()
       .def("__repr__", [](ActionType a) {
           switch (a) {
               case ActionType::MoveForward: return "ActionType.MoveForward";
               case ActionType::MoveBackward: return "ActionType.MoveBackward";
               case ActionType::MoveLeft: return "ActionType.MoveLeft";
               case ActionType::MoveRight: return "ActionType.MoveRight";
               case ActionType::TurnLeft: return "ActionType.TurnLeft";
               case ActionType::TurnRight: return "ActionType.TurnRight";
               case ActionType::Look: return "ActionType.Look";
               case ActionType::LookUp: return "ActionType.LookUp";
               case ActionType::LookDown: return "ActionType.LookDown";
               case ActionType::Interact: return "ActionType.Interact";
               case ActionType::Eat: return "ActionType.Eat";
               case ActionType::Drink: return "ActionType.Drink";
               case ActionType::Rest: return "ActionType.Rest";
               case ActionType::Wait: return "ActionType.Wait";
               case ActionType::Custom: return "ActionType.Custom";
               default: return "ActionType.Unknown";
           }
       });
}

// Enhanced MotorCommand enum with Python-friendly access
void bindMotorCommand(py::enum_<MotorCommand>& e) {
    e.value("MOVE_FORWARD", MotorCommand::MoveForward)
       .value("MOVE_BACKWARD", MotorCommand::MoveBackward)
       .value("TURN_LEFT", MotorCommand::TurnLeft)
       .value("TURN_RIGHT", MotorCommand::TurnRight)
       .value("LOOK_LEFT", MotorCommand::LookLeft)
       .value("LOOK_RIGHT", MotorCommand::LookRight)
       .value("INTERACT", MotorCommand::Interact)
       .value("WAIT", MotorCommand::Wait)
       .export_values()
       .def("__repr__", [](MotorCommand m) {
           switch (m) {
               case MotorCommand::MoveForward: return "MotorCommand.MoveForward";
               case MotorCommand::MoveBackward: return "MotorCommand.MoveBackward";
               case MotorCommand::TurnLeft: return "MotorCommand.TurnLeft";
               case MotorCommand::TurnRight: return "MotorCommand.TurnRight";
               case MotorCommand::LookLeft: return "MotorCommand.LookLeft";
               case MotorCommand::LookRight: return "MotorCommand.LookRight";
               case MotorCommand::Interact: return "MotorCommand.Interact";
               case MotorCommand::Wait: return "MotorCommand.Wait";
               default: return "MotorCommand.Unknown";
           }
       });
}

// Enhanced WorldObjectType enum with Python-friendly access
void bindWorldObjectType(py::enum_<WorldObjectType>& e) {
    e.value("EMPTY", WorldObjectType::Empty)
       .value("RESOURCE", WorldObjectType::Resource)
       .value("HAZARD", WorldObjectType::Hazard)
       .value("WALL", WorldObjectType::Wall)
       .value("MARKER", WorldObjectType::Marker)
       .export_values()
       .def("__repr__", [](WorldObjectType t) {
           switch (t) {
               case WorldObjectType::Empty: return "WorldObjectType.Empty";
               case WorldObjectType::Resource: return "WorldObjectType.Resource";
               case WorldObjectType::Hazard: return "WorldObjectType.Hazard";
               case WorldObjectType::Wall: return "WorldObjectType.Wall";
               case WorldObjectType::Marker: return "WorldObjectType.Marker";
               default: return "WorldObjectType.Unknown";
           }
       });
}

// Enhanced Config class with Pythonic interface
void bindConfig(py::class_<Config>& cls) {
    cls
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
        .def("__str__", [](const Config& cfg) {
            return cfg.summary();
        });
    
    // Add type-safe get methods
    cls.def("get_int", [](const Config& self, const std::string& key, int64_t default_val) {
        return self.getOr<int64_t>(key, default_val);
    }, py::arg("key"), py::arg("default_val"),
        "Get an integer configuration value");
    
    cls.def("get_float", [](const Config& self, const std::string& key, float default_val) {
        return self.getOr<float>(key, default_val);
    }, py::arg("key"), py::arg("default_val"),
        "Get a float configuration value");
    
    cls.def("get_double", [](const Config& self, const std::string& key, double default_val) {
        return self.getOr<double>(key, default_val);
    }, py::arg("key"), py::arg("default_val"),
        "Get a double configuration value");
    
    cls.def("get_string", [](const Config& self, const std::string& key, const std::string& default_val) {
        return self.getOr<std::string>(key, default_val);
    }, py::arg("key"), py::arg("default_val"),
        "Get a string configuration value");
    
    cls.def("set", [](Config& self, const std::string& key, int64_t value) {
        self.set(key, value, ConfigSource::Default);
    }, py::arg("key"), py::arg("value"),
        "Set an integer configuration value");
    
    cls.def("set", [](Config& self, const std::string& key, float value) {
        self.set(key, value, ConfigSource::Default);
    }, py::arg("key"), py::arg("value"),
        "Set a float configuration value");
    
    cls.def("set", [](Config& self, const std::string& key, const std::string& value) {
        self.set(key, value, ConfigSource::Default);
    }, py::arg("key"), py::arg("value"),
        "Set a string configuration value");
}

// Enhanced SensoryInput class
void bindSensoryInput(py::class_<SensoryInput>& cls) {
    cls
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");
}

// Enhanced Vision class
void bindVision(py::class_<Vision, SensoryInput>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);
}

// Enhanced Audio class
void bindAudio(py::class_<Audio, SensoryInput>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);
}

// Enhanced InternalSignals class
void bindInternalSignals(py::class_<InternalSignals, SensoryInput>& cls) {
    cls
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);
}

// Enhanced Action class
void bindAction(py::class_<Action>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);
}

// Enhanced WorldObject class
void bindWorldObject(py::class_<WorldObject>& cls) {
    cls
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
}

// Enhanced AgentBody class
void bindAgentBody(py::class_<AgentBody>& cls) {
    cls
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
}

// Enhanced ActionResult class
void bindActionResult(py::class_<ActionResult>& cls) {
    cls
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);
}

// Enhanced SensoryPercept class
void bindSensoryPercept(py::class_<SensoryPercept>& cls) {
    cls
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
}

// Enhanced SimpleWorld class
void bindSimpleWorld(py::class_<SimpleWorld>& cls) {
    cls
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);
}

// Enhanced Brain class with modern Pythonic interface
void bindBrain(py::class_<Brain>& cls) {
    cls
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
             "Log brain status");
}

// Enhanced AgentBrain class with modern Pythonic interface
void bindAgentBrain(py::class_<AgentBrain>& cls) {
    cls
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
}

// Convenience factory functions
void bindConvenienceFunctions(py::module_& m) {
    // Default configuration factory
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration", py::call_guard<py::gil_scoped_release>());
    
    // Brain factory with common configurations
    m.def("createBrain", [](const std::string& config_name) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (config_name == "small") {
            config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        } else if (config_name == "medium") {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        } else if (config_name == "large") {
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        } else {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        }
        auto brain = std::make_shared<Brain>(config);
        if (!brain->initialize()) {
            throw InitializationError("Failed to initialize brain");
        }
        return brain;
    }, py::arg("config_name") = "medium",
        "Create a brain with a predefined configuration");
    
    // Simple world factory with convenience methods
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        return world;
    }, "Create a simple world with default settings");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, int steps) {
        for (int i = 0; i < steps; ++i) {
            brain->step(i, i * 0.001);
        }
    }, py::arg("brain"), py::arg("steps"),
        "Run a simulation for a specified number of steps");
    
    // Configuration helpers
    m.def("createConfigWithDefaults", [](py::kwargs kwargs) {
        auto config = std::make_shared<Config>();
        if (kwargs.contains("neuron_count")) {
            config->set("neuron_count", static_cast<int64_t>(kwargs["neuron_count"].cast<int64_t>()), ConfigSource::Default);
        }
        if (kwargs.contains("region_count")) {
            config->set("region_count", static_cast<int64_t>(kwargs["region_count"].cast<int64_t>()), ConfigSource::Default);
        }
        if (kwargs.contains("connection_probability")) {
            config->set("connection_probability", kwargs["connection_probability"].cast<float>(), ConfigSource::Default);
        }
        return config;
    }, py::arg("kwargs") = py::kwargs(),
        "Create a configuration with customizable parameters");
}

PYBIND11_MODULE(pynlm, m) {
    // Module docstring
    m.doc() = R"pbdoc(
        Enhanced NLM (Neural Learning Machine) Python Bindings
        --------------------------------------------------------
        A modern Python binding for the NLM C++ neural simulation framework.
        
        Key features:
        - Enhanced error handling with custom exceptions
        - Type-safe configuration API
        - Convenience factory functions
        - Modern Pythonic interface with type hints
        - Better documentation and examples
        
        Classes: Brain, Config, AgentBrain, SimpleWorld, Vision, Audio, InternalSignals
        Data types: NeuronId, SynapseId, RegionId, PopulationId
        Enums: NeuronType, SynapseType, DevelopmentalStage, FiringState, ActionType, MotorCommand, WorldObjectType
        
        For more information, see the NLM documentation.
    )pbdoc";
    
    // Register custom exceptions
    py::register_exception<NLMException>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError");
    py::register_exception<InitializationError>(m, "InitializationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    
    // Bind identifier classes
    bindNeuronId(m.class_<NeuronId>(m, "NeuronId"));
    bindSynapseId(m.class_<SynapseId>(m, "SynapseId"));
    bindRegionId(m.class_<RegionId>(m, "RegionId"));
    bindPopulationId(m.class_<PopulationId>(m, "PopulationId"));
    
    // Bind enums
    bindNeuronType(m.enum_<NeuronType>(m, "NeuronType"));
    bindSynapseType(m.enum_<SynapseType>(m, "SynapseType"));
    bindDevelopmentalStage(m.enum_<DevelopmentalStage>(m, "DevelopmentalStage"));
    bindFiringState(m.enum_<FiringState>(m, "FiringState"));
    bindActionType(m.enum_<ActionType>(m, "ActionType"));
    bindMotorCommand(m.enum_<MotorCommand>(m, "MotorCommand"));
    bindWorldObjectType(m.enum_<WorldObjectType>(m, "WorldObjectType"));
    
    // Bind classes
    bindConfig(m.class_<Config>(m, "Config"));
    bindSensoryInput(m.class_<SensoryInput>(m, "SensoryInput"));
    bindVision(m.class_<Vision, SensoryInput>(m, "Vision"));
    bindAudio(m.class_<Audio, SensoryInput>(m, "Audio"));
    bindInternalSignals(m.class_<InternalSignals, SensoryInput>(m, "InternalSignals"));
    bindAction(m.class_<Action>(m, "Action"));
    bindWorldObject(m.class_<WorldObject>(m, "WorldObject"));
    bindAgentBody(m.class_<AgentBody>(m, "AgentBody"));
    bindActionResult(m.class_<ActionResult>(m, "ActionResult"));
    bindSensoryPercept(m.class_<SensoryPercept>(m, "SensoryPercept"));
    bindSimpleWorld(m.class_<SimpleWorld>(m, "SimpleWorld"));
    bindBrain(m.class_<Brain>(m, "Brain"));
    bindAgentBrain(m.class_<AgentBrain>(m, "AgentBrain"));
    
    // Bind convenience functions
    bindConvenienceFunctions(m);
}

} // namespace nlm
