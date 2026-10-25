#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>
#include <sstream>
#include <iomanip>

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
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

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
        .export_values()
        .def("__str__", [](NeuronType type) {
            switch(type) {
                case NeuronType::Excitatory: return "Excitatory";
                case NeuronType::Inhibitory: return "Inhibitory";
                case NeuronType::Modulatory: return "Modulatory";
                case NeuronType::Sensory: return "Sensory";
                case NeuronType::Motor: return "Motor";
                case NeuronType::Internal: return "Internal";
                default: return "Unknown";
            }
        });

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values()
        .def("__str__", [](SynapseType type) {
            switch(type) {
                case SynapseType::Excitatory: return "Excitatory";
                case SynapseType::Inhibitory: return "Inhibitory";
                case SynapseType::Modulatory: return "Modulatory";
                case SynapseType::Electrical: return "Electrical";
                case SynapseType::GapJunction: return "GapJunction";
                default: return "Unknown";
            }
        });

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values()
        .def("__str__", [](DevelopmentalStage stage) {
            switch(stage) {
                case DevelopmentalStage::Initial: return "Initial";
                case DevelopmentalStage::CriticalPeriod: return "CriticalPeriod";
                case DevelopmentalStage::Maturation: return "Maturation";
                case DevelopmentalStage::Adult: return "Adult";
                case DevelopmentalStage::Aging: return "Aging";
                default: return "Unknown";
            }
        });

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values()
        .def("__str__", [](FiringState state) {
            switch(state) {
                case FiringState::Resting: return "Resting";
                case FiringState::Active: return "Active";
                case FiringState::Refractory: return "Refractory";
                case FiringState::Inhibited: return "Inhibited";
                default: return "Unknown";
            }
        });

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
        .def("__str__", [](ActionType action) {
            switch(action) {
                case ActionType::MoveForward: return "MoveForward";
                case ActionType::MoveBackward: return "MoveBackward";
                case ActionType::MoveLeft: return "MoveLeft";
                case ActionType::MoveRight: return "MoveRight";
                case ActionType::TurnLeft: return "TurnLeft";
                case ActionType::TurnRight: return "TurnRight";
                case ActionType::Look: return "Look";
                case ActionType::LookUp: return "LookUp";
                case ActionType::LookDown: return "LookDown";
                case ActionType::Interact: return "Interact";
                case ActionType::Eat: return "Eat";
                case ActionType::Drink: return "Drink";
                case ActionType::Rest: return "Rest";
                case ActionType::Wait: return "Wait";
                case ActionType::Custom: return "Custom";
                default: return "Unknown";
            }
        });

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values()
        .def("__str__", [](MotorCommand cmd) {
            switch(cmd) {
                case MotorCommand::MoveForward: return "MoveForward";
                case MotorCommand::MoveBackward: return "MoveBackward";
                case MotorCommand::TurnLeft: return "TurnLeft";
                case MotorCommand::TurnRight: return "TurnRight";
                case MotorCommand::LookLeft: return "LookLeft";
                case MotorCommand::LookRight: return "LookRight";
                case MotorCommand::Interact: return "Interact";
                case MotorCommand::Wait: return "Wait";
                default: return "Unknown";
            }
        });

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values()
        .def("__str__", [](WorldObjectType type) {
            switch(type) {
                case WorldObjectType::Empty: return "Empty";
                case WorldObjectType::Resource: return "Resource";
                case WorldObjectType::Hazard: return "Hazard";
                case WorldObjectType::Wall: return "Wall";
                case WorldObjectType::Marker: return "Marker";
                default: return "Unknown";
            }
        });

py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Custom Python exception for NLM errors
    py::register_exception<std::runtime_error>(m, "NLMError", PyExc_RuntimeError);

    // Add Python-style error checking and validation to key methods

    // Enhanced Config methods with validation
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](Config& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw std::runtime_error("Filepath cannot be empty");
            }
            return self.loadFromFile(filepath);
        }, py::arg("filepath"),
             "Load configuration from a JSON file")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            if (argc <= 0) {
                throw std::runtime_error("argc must be positive");
            }
            if (!argv) {
                throw std::runtime_error("argv cannot be null");
            }
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments")
        .def("saveToFile", [](Config& self, const std::string& filepath) {
            if (filepath.empty()) {
                throw std::runtime_error("Filepath cannot be empty");
            }
            return self.saveToFile(filepath);
        }, py::arg("filepath"),
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
        .def("__repr__", [](const Action& action) {
            std::string params_str = "[]";
            if (!action.getParameters().empty()) {
                params_str = "[";
                for (size_t i = 0; i < action.getParameters().size(); ++i) {
                    if (i > 0) params_str += ", ";
                    params_str += std::to_string(action.getParameters()[i]);
                }
                params_str += "]";
            }
            return "<Action: " + action.getName() + " params=" + params_str + ">";
        });

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
            return "<WorldObject: type=" + std::to_string(static_cast<int>(obj.type)) +
                   " x=" + std::to_string(obj.x) + " y=" + std::to_string(obj.y) +
                   " radius=" + std::to_string(obj.radius) + ">";
        });

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
            return "<AgentBody: x=" + std::to_string(body.x) + " y=" + std::to_string(body.y) +
                   " energy=" + std::to_string(body.energy) + " health=" + std::to_string(body.health) +
                   " age=" + std::to_string(body.age) + " moving=" + std::to_string(body.isMoving) +
                   " turning=" + std::to_string(body.isTurning) + ">";
        });

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
                   " message=\"" + result.message + \">";
        });

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
            std::string vision_str = "[empty]";
            if (!percept.getVision().empty()) {
                vision_str = "[";
                for (size_t i = 0; i < percept.getVision().size(); ++i) {
                    if (i > 0) vision_str += ", ";
                    vision_str += std::to_string(percept.getVision()[i]);
                }
                vision_str += "]";
            }
            return "<SensoryPercept: vision=" + vision_str + " width=" + std::to_string(percept.getVisionWidth()) +
                   " height=" + std::to_string(percept.getVisionHeight()) + " internal=" + std::to_string(percept.getInternal().size()) +
                   " timestamp=" + std::to_string(percept.getTimestamp()) + ">";
        });

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
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld: width=" + std::to_string(world.getWidth()) +
                   " height=" + std::to_string(world.getHeight()) +
                   " time=" + std::to_string(world.getSimulationTime()) + ">";
        });

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
        .def("__repr__", [](const Brain& brain) {
            return "<Brain: neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) +
                   " firing=" + std::to_string(brain.getFiringNeuronCount()) +
                   " rate=" + std::to_string(brain.getAverageFiringRate()) +
                   " dev_stage=" + std::to_string(static_cast<int>(brain.getDevelopmentalStage())) + ">";
        });

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
        .def("__repr__", [](const AgentBrain& agent) {
            return "<AgentBrain: brain_ref=" + std::to_string(reinterpret_cast<uintptr_t>(agent.getBrain())) +
                   " curiosity=" + std::to_string(agent.getCuriosityLevel()) +
                   " novelty=" + std::to_string(agent.getNoveltyLevel()) + ">";
        });

    // Add convenience methods for common operations

    // Add world helper functions
    m.def("createWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        return world;
    }, "Create a new simple world");

    m.def("createAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"),
        "Create an agent brain and initialize it with a world");

    m.def("runSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                            std::shared_ptr<AgentBrain> agent, int numSteps) {
        if (!brain || !world || !agent) {
            throw std::runtime_error("Brain, world, and agent must not be null");
        }
        if (numSteps <= 0) {
            throw std::runtime_error("Number of steps must be positive");
        }
        
        brain->initialize();
        world->reset();
        
        for (int step = 0; step < numSteps; ++step) {
            world->update(0.1);  // Default timestep
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto motorCmd = agent->decodeMotorCommand();
            world->applyMotorCommand(motorCmd, world->getSimulationTime());
            
            // Optional: Apply reward modulation if enabled
            if (agent->isRewardModulationEnabled()) {
                auto internalSignals = percept.getInternal();
                float reward = internalSignals.empty() ? 0.0f : internalSignals[0];
                agent->applyRewardModulation(reward, 0.0f);
            }
            
            // Optional: Update development if enabled
            if (agent->isDevelopmentEnabled()) {
                agent->updateDevelopment(0.1);
            }
        }
        
        return std::make_tuple(brain->getTotalSpikeCount(),
                               brain->getFiringNeuronCount(),
                               brain->getAverageFiringRate());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps") = 1000,
        "Run a complete agent simulation");

    m.def("resetAndInitialize", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        if (!world) {
            throw std::runtime_error("World cannot be null");
        }
        
        brain->reset();
        world->reset();
        brain->initialize();
        return true;
    }, py::arg("brain"), py::arg("world"),
        "Reset brain and world, then initialize brain");

    m.def("getBrainStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["region_count"] = brain->getRegionCount();
        stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return stats;
    }, py::arg("brain"),
        "Get comprehensive brain statistics as a dictionary");

    m.def("configureStandardWorld", [](std::shared_ptr<SimpleWorld> world, 
                                     int width = 20, int height = 20,
                                     int visionWidth = 8, int visionHeight = 8) {
        if (!world) {
            throw std::runtime_error("World cannot be null");
        }
        if (width <= 0 || height <= 0 || visionWidth <= 0 || visionHeight <= 0) {
            throw std::runtime_error("World and vision dimensions must be positive");
        }
        
        world->configure(width, height, visionWidth, visionHeight);
        world->setAgentStart(width / 2, height / 2);
        return world;
    }, py::arg("world"), py::arg("width") = 20, py::arg("height") = 20,
        py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
        "Configure a standard world with sensible defaults");

    // Add integration with Python time module

    // Python time integration functions
    m.def("getCurrentTime", []() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return milliseconds;
    }, "Get current time in milliseconds since epoch");

    m.def("getTimestamp", []() {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        return static_cast<double>(milliseconds) / 1000.0;  // Return as seconds
    }, "Get current time in seconds since epoch");

    m.def("formatTime", [](double timestamp) {
        auto timePoint = std::chrono::system_clock::time_point(
            std::chrono::duration<double>(timestamp)
        );
        auto timeT = std::chrono::system_clock::to_time_t(timePoint);
        std::stringstream ss;
        ss << std::put_time(std::gmtime(&timeT), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }, py::arg("timestamp"),
        "Format a timestamp as a readable string");

    m.def("addTime", [](double timestamp, double seconds) {
        return timestamp + seconds;
    }, py::arg("timestamp"), py::arg("seconds"),
        "Add seconds to a timestamp");

    m.def("timeDiff", [](double timestamp1, double timestamp2) {
        return std::abs(timestamp1 - timestamp2);
    }, py::arg("timestamp1"), py::arg("timestamp2"),
        "Calculate absolute difference between two timestamps in seconds");

    m.def("elapsedTime", [](double startTime) {
        auto now = std::chrono::system_clock::now();
        auto duration = now.time_since_epoch();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
        double currentTime = static_cast<double>(milliseconds) / 1000.0;
        return currentTime - startTime;
    }, py::arg("startTime"),
        "Calculate elapsed time since a given timestamp in seconds");

    // Enhanced step methods with Python time integration
    py::overload_cast<SimulationStep>(&Brain::step).def(py::arg("currentStep"),
        "Perform a simulation step (simple version)");
    py::overload_cast<SimulationStep, Timestamp>(&Brain::step).def(py::arg("currentStep"), py::arg("currentTime"),
        "Perform a simulation step with timestamp (Python-enhanced version)");

    // Add advanced features for power users

    // Advanced brain manipulation and analysis functions
    m.def("batchProcessNeurons", [](std::shared_ptr<Brain> brain, 
                                 std::vector<NeuronId> neuronIds,
                                 std::function<void(NeuronId, float)> callback) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        for (const auto& neuronId : neuronIds) {
            callback(neuronId, brain->getMembranePotential(neuronId));
        }
    }, py::arg("brain"), py::arg("neuronIds"), py::arg("callback"),
        "Process multiple neurons with a callback function");

    m.def("getRegionStatistics", [](std::shared_ptr<Brain> brain, RegionId regionId) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        auto* region = brain->getRegion(regionId);
        if (!region) {
            throw std::runtime_error("Region not found");
        }
        
        py::dict stats;
        stats["neuron_count"] = region->getNeuronCount();
        stats["synapse_count"] = region->getSynapseCount();
        stats["active_neurons"] = region->getActiveNeuronCount();
        stats["firing_rate"] = region->getAverageFiringRate();
        stats["excitatory_ratio"] = region->getExcitationInhibitionRatio();
        
        return stats;
    }, py::arg("brain"), py::arg("regionId"),
        "Get detailed statistics for a specific brain region");

    m.def("findSimilarNeurons", [](std::shared_ptr<Brain> brain,
                                 const std::vector<float>& pattern,
                                 float threshold = 0.8) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        std::vector<NeuronId> similarNeurons;
        for (size_t i = 0; i < brain->getTotalNeuronCount(); ++i) {
            NeuronId neuronId(i);
            auto potential = brain->getMembranePotential(neuronId);
            if (potential > 0.0) {  // Simple similarity check
                similarNeurons.push_back(neuronId);
            }
        }
        
        return similarNeurons;
    }, py::arg("brain"), py::arg("pattern"), py::arg("threshold") = 0.8,
        "Find neurons similar to a given pattern");

    m.def("exportBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        if (filename.empty()) {
            throw std::runtime_error("Filename cannot be empty");
        }
        
        return brain->save(filename);
    }, py::arg("brain"), py::arg("filename"),
        "Export brain state to a file (alias for save())");

    m.def("importBrainState", [](std::shared_ptr<Brain> brain, const std::string& filename) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        if (filename.empty()) {
            throw std::runtime_error("Filename cannot be empty");
        }
        
        return brain->load(filename);
    }, py::arg("brain"), py::arg("filename"),
        "Import brain state from a file (alias for load())");

    m.def("createComplexConfig", [](const std::string& configJson) {
        auto config = std::make_shared<Config>();
        // For now, just create default config
        // In future, could parse JSON config
        return config;
    }, py::arg("configJson"),
        "Create a configuration from JSON string (future feature)");

    m.def("benchmarkBrain", [](std::shared_ptr<Brain> brain, int numSteps, double timestep) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        if (numSteps <= 0) {
            throw std::runtime_error("Number of steps must be positive");
        }
        
        auto startTime = std::chrono::high_resolution_clock::now();
        brain->initialize();
        
        for (int step = 0; step < numSteps; ++step) {
            brain->step(step, timestep * step);
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        py::dict results;
        results["steps_performed"] = numSteps;
        results["elapsed_time_ms"] = static_cast<double>(duration.count());
        results["steps_per_second"] = static_cast<double>(numSteps) * 1000.0 / duration.count();
        results["neurons"] = brain->getTotalNeuronCount();
        results["spikes"] = brain->getTotalSpikeCount();
        
        return results;
    }, py::arg("brain"), py::arg("numSteps"), py::arg("timestep") = 0.1,
        "Benchmark brain performance and return metrics");

    m.def("getAllRegionIds", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        return brain->getRegionIds();
    }, py::arg("brain"),
        "Get all region IDs from brain (convenience function)");

    m.def("setAgentConfig", [](std::shared_ptr<AgentBrain> agent, 
                             bool rewardModulation = true,
                             bool structuralPlasticity = true,
                             bool development = true,
                             bool curiosity = true) {
        if (!agent) {
            throw std::runtime_error("Agent cannot be null");
        }
        
        agent->enableRewardModulation(rewardModulation);
        agent->enableStructuralPlasticity(structuralPlasticity);
        agent->enableDevelopment(development);
        agent->enableCuriosity(curiosity);
        
        return true;
    }, py::arg("agent"), py::arg("rewardModulation") = true,
        py::arg("structuralPlasticity") = true, py::arg("development") = true,
        py::arg("curiosity") = true,
        "Configure agent with multiple subsystems at once");

    m.def("getComprehensiveStats", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw std::runtime_error("Brain cannot be null");
        }
        
        py::dict allStats;
        
        // Basic stats
        allStats["basic"] = py::dict(
            "total_neurons" = brain->getTotalNeuronCount(),
            "total_synapses" = brain->getTotalSynapseCount(),
            "active_neurons" = brain->getActiveNeuronCount(),
            "firing_neurons" = brain->getFiringNeuronCount(),
            "total_spikes" = brain->getTotalSpikeCount(),
            "average_firing_rate" = brain->getAverageFiringRate(),
            "excitation_inhibition_ratio" = brain->getExcitationInhibitionRatio(),
            "developmental_stage" = static_cast<int>(brain->getDevelopmentalStage())
        );
        
        // Region stats
        py::list regionStats;
        auto regionIds = brain->getRegionIds();
        for (const auto& regionId : regionIds) {
            auto* region = brain->getRegion(regionId);
            if (region) {
                regionStats.append(py::dict(
                    "id" = regionId.value,
                    "neuron_count" = region->getNeuronCount(),
                    "synapse_count" = region->getSynapseCount(),
                    "active_neurons" = region->getActiveNeuronCount(),
                    "firing_rate" = region->getAverageFiringRate(),
                    "excitation_inhibition_ratio" = region->getExcitationInhibitionRatio()
                ));
            }
        }
        allStats["regions"] = regionStats;
        
        return allStats;
    }, py::arg("brain"),
        "Get comprehensive statistics for brain and all regions");
