#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>

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
#include "../src/environment/Environment.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/visualization/VisualizationInterface.hpp"
#include "../src/core/Random/Random.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for: Config, Neuron, Synapse, NeuralPopulation, NeuralRegion,
        NeuronType, SynapseType, DevelopmentalStage, FiringState, ActionType,
        MotorCommand, WorldObjectType, WorldObject, AgentBody, Action, ActionResult,
        SensoryInput, Vision, Audio, InternalSignals, SensoryPercept, SimpleWorld,
        Brain, AgentBrain, Neuromodulator (Dopamine, Acetylcholine, Norepinephrine, Serotonin),
        PlasticityRule (Hebbian, STDP, StructuralPlasticity), DevelopmentSystem,
        NeuralWorkingMemory, AttentionalSelection, VisualizationInterface,
        Environment (GridWorldEnvironment), Observation, and more.
        Provides factory functions for creating neural components and systems.
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
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp")
        .def("clone", &SensoryInput::clone, py::return_value_policy::take_ownership, "Create a copy of this sensory input");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"), py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setDataPtr", [](Vision& self, float* data, size_t size) {
            self.setData(data, size);
        }, py::arg("data"), py::arg("size"), "Set image data from pointer")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("clone", &Vision::clone, py::return_value_policy::take_ownership, "Create a copy of this vision data");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("clone", &Audio::clone, py::return_value_policy::take_ownership, "Create a copy of this audio data");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals")
        .def("clone", &InternalSignals::clone, py::return_value_policy::take_ownership, "Create a copy of this internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone, py::return_value_policy::take_ownership, "Create a copy of this action")
        .def("addParameter", [](Action& self, float param) {
            auto params = self.getParameters();
            params.push_back(param);
            self.setParameters(params);
        }, py::arg("param"), "Add a parameter to the action")
        .def("getParameterCount", [](const Action& self) {
            return self.getParameters().size();
        }, "Get the number of parameters in this action");

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
        // Convenience methods
        .def("getTypeName", [](WorldObject& self) {
            // Get human-readable type name
            static const std::string typeNames[] = {"Empty", "Resource", "Hazard", "Wall", "Marker"};
            auto idx = static_cast<size_t>(self.type);
            if (idx < 5) return typeNames[idx];
            return std::string("Unknown");
        }, "Get human-readable type name")
        .def("isHazard", [](WorldObject& self) {
            return self.type == WorldObjectType::Hazard;
        }, "Check if this is a hazard object")
        .def("isResource", [](WorldObject& self) {
            return self.type == WorldObjectType::Resource;
        }, "Check if this is a resource object")
        .def("isWalkable", [](WorldObject& self) {
            return self.type != WorldObjectType::Wall && self.type != WorldObjectType::Hazard;
        }, "Check if this object is walkable")
        .def("distanceTo", [](WorldObject& self, float x, float y) {
            float dx = self.x - x;
            float dy = self.y - y;
            return std::sqrt(dx*dx + dy*dy);
        }, py::arg("x"), py::arg("y"), "Calculate distance to position (x, y)")
        .def("toString", [](WorldObject& self) {
            std::stringstream ss;
            ss << "WorldObject(type=" << static_cast<int>(self.type)
               << ", x=" << self.x << ", y=" << self.y
               << ", radius=" << self.radius << ", value=" << self.value
               << ", active=" << (self.active ? "true" : "false") << ")";
            return ss.str();
        }, "Get string representation");

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("isPositiveReward", [](const ActionResult& self) {
            return self.reward > 0.0f;
        }, "Check if reward is positive")
        .def("isNegativeReward", [](const ActionResult& self) {
            return self.reward < 0.0f;
        }, "Check if reward is negative")
        .def("hasMessage", [](const ActionResult& self) {
            return !self.message.empty();
        }, "Check if result has a message")
        .def("toString", [](const ActionResult& self) {
            std::stringstream ss;
            ss << "ActionResult(reward=" << self.reward 
               << ", success=" << (self.success ? "true" : "false")
               << ", message=\"" << self.message << "\")";
            return ss.str();
        }, "Get string representation");

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
        // Convenience methods
        .def("hasVision", [](const SensoryPercept& self) {
            return !self.getVision().empty();
        }, "Check if percept has vision data")
        .def("hasTouch", [](const SensoryPercept& self) {
            return !self.getTouch().empty();
        }, "Check if percept has touch data")
        .def("hasAudio", [](const SensoryPercept& self) {
            return !self.getAudio().empty();
        }, "Check if percept has audio data")
        .def("isValid", [](const SensoryPercept& self) {
            return self.getVisionWidth() > 0 && self.getVisionHeight() > 0;
        }, "Check if percept has valid sensor data")
        .def("getSignalCount", [](const SensoryPercept& self) {
            auto& signals = self.getAllSignals();
            return signals.size();
        }, "Get total number of signal channels")
        .def("toString", [](const SensoryPercept& self) {
            std::stringstream ss;
            ss << "SensoryPercept(vision=" << self.getVisionWidth() << "x" << self.getVisionHeight()
               << ", touch=" << self.getTouch().size()
               << ", internal=" << self.getInternal().size()
               << ", audio=" << self.getAudio().size()
               << ", timestamp=" << self.getTimestamp() << ")";
            return ss.str();
        }, "Get string representation");

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
        // Convenience methods
        .def("isAgentAlive", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return body.health > 0.0f && body.energy > 0.0f;
        }, "Check if agent is alive (positive health and energy)")
        .def("getAgentPosition", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return std::make_pair(body.x, body.y);
        }, "Get agent position as (x, y) tuple")
        .def("getAgentOrientation", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return body.orientation;
        }, "Get agent orientation in radians")
        .def("getAgentEnergy", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return body.energy;
        }, "Get agent energy level")
        .def("getAgentHealth", [](SimpleWorld& self) {
            auto& body = self.getAgentBody();
            return body.health;
        }, "Get agent health level")
        .def("getObjectAt", [](SimpleWorld& self, float x, float y) {
            // Helper to find object at position
            for (const auto& obj : self.getObjects()) {
                if (obj.active && std::abs(obj.x - x) < obj.radius && std::abs(obj.y - y) < obj.radius) {
                    return obj;
                }
            }
            WorldObject empty;
            return empty;
        }, py::arg("x"), py::arg("y"), "Get world object at position (x, y)")
        .def("countObjects", [](SimpleWorld& self, WorldObjectType type) {
            int count = 0;
            for (const auto& obj : self.getObjects()) {
                if (obj.active && obj.type == type) {
                    count++;
                }
            }
            return count;
        }, py::arg("type"), "Count objects of a specific type")
        .def("getObjectCount", [](SimpleWorld& self) {
            return self.getObjectCount();
        }, "Get total number of active objects in world")
        .def("isPositionSafe", [](SimpleWorld& self, float x, float y) {
            return self.isValidPosition(x, y);
        }, py::arg("x"), py::arg("y"), "Check if position is safe for agent")
        .def("toString", [](SimpleWorld& self) {
            std::stringstream ss;
            ss << "SimpleWorld(size=" << self.getWidth() << "x" << self.getHeight()
               << ", agent_x=" << self.getAgentBody().x
               << ", agent_y=" << self.getAgentBody().y
               << ", time=" << self.getSimulationTime() << ")";
            return ss.str();
        }, "Get string representation");

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
        // Convenience methods for Python users
        .def("update", [](Brain& self, SimulationStep step, Timestamp time = 0.0) {
            self.step(step, time);
        }, py::arg("step"), py::arg("time") = 0.0, "Update brain for given step and time")
        .def("hasNeuronCount", [](const Brain& self) {
            return self.getTotalNeuronCount() > 0;
        }, "Check if brain has neurons")
        .def("getNetworkHealth", [](const Brain& self) {
            float ratio = self.getExcitationInhibitionRatio();
            size_t active = self.getActiveNeuronCount();
            size_t total = self.getTotalNeuronCount();
            return std::make_tuple(ratio, active, total);
        }, "Get network health metrics (excitation_inhibition_ratio, active_neurons, total_neurons)")
        .def("toString", [](const Brain& self) {
            std::stringstream ss;
            ss << "Brain(neurons=" << self.getTotalNeuronCount()
               << ", synapses=" << self.getTotalSynapseCount()
               << ", regions=" << self.getRegionCount()
               << ", stage=" << static_cast<int>(self.getDevelopmentalStage()) << ")";
            return ss.str();
        }, "Get string representation");

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
        // Convenience methods for Python users
        .def("getActionParameters", [](const AgentBrain& self) {
            // Get default action parameters for each motor command
            std::vector<std::pair<MotorCommand, std::vector<float>>> params;
            
            // Simple parameter mapping based on motor command type
            const std::vector<std::pair<MotorCommand, std::vector<float>>> actionParams = {
                {MotorCommand::MoveForward, {1.0f, 0.0f, 0.0f}},
                {MotorCommand::MoveBackward, {-1.0f, 0.0f, 0.0f}},
                {MotorCommand::TurnLeft, {0.0f, 0.0f, -1.0f}},
                {MotorCommand::TurnRight, {0.0f, 0.0f, 1.0f}},
                {MotorCommand::Look, {0.0f, 1.0f, 0.0f}},
                {MotorCommand::Interact, {0.0f, 0.0f, 0.0f}},
                {MotorCommand::Wait, {0.0f, 0.0f, 0.0f}}
            };
            
            for (const auto& [cmd, param] : actionParams) {
                params.emplace_back(cmd, param);
            }
            
            return params;
        }, "Get default action parameters for motor commands")
        .def("getMotivationalFactors", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "MotivationalFactors(dopamine=" << self.getNeuromodulationLevel()
               << ", curiosity=" << self.getCuriosityLevel()
               << ", novelty=" << self.getNoveltyLevel()
               << ", predictionError=" << self.getPredictionError() << ")";
            return ss.str();
        }, "Get motivational factors as string")
        .def("toString", [](const AgentBrain& self) {
            std::stringstream ss;
            ss << "AgentBrain(stage=" << static_cast<int>(self.getDevelopmentalStage())
               << ", neuromod=" << self.getNeuromodulationLevel()
               << ", curiosity=" << self.getCuriosityLevel()
               << ", novelty=" << self.getNoveltyLevel() << ")";
            return ss.str();
        }, "Get string representation");

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

    // Create convenience factory functions
    m.def("createNeuron", [](NeuronId id) -> std::shared_ptr<Neuron> {
        return std::make_shared<Neuron>(id);
    }, py::arg("id"), "Create a neuron with given ID");

    m.def("createSynapse", [](SynapseId id, NeuronId source, NeuronId destination) -> std::shared_ptr<Synapse> {
        return std::make_shared<Synapse>(id, source, destination);
    }, py::arg("id"), py::arg("source"), py::arg("destination"), "Create a synapse with given IDs");

    m.def("createNeuralPopulation", [](PopulationId id, size_t size) -> std::shared_ptr<NeuralPopulation> {
        return std::make_shared<NeuralPopulation>(id, size);
    }, py::arg("id"), py::arg("size"), "Create a neural population with given ID and size");

    m.def("createNeuralRegion", [](RegionId id, const std::string& name = "") -> std::shared_ptr<NeuralRegion> {
        return std::make_shared<NeuralRegion>(id, name);
    }, py::arg("id"), py::arg("name") = "", "Create a neural region with given ID and name");

    // Create development system factory
    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create a development system");

    // Create neuromodulator factories
    m.def("createDopamine", []() -> std::shared_ptr<Neuromodulator> {
        return std::make_shared<Dopamine>();
    }, "Create a dopamine neuromodulator");

    m.def("createAcetylcholine", []() -> std::shared_ptr<Neuromodulator> {
        return std::make_shared<Acetylcholine>();
    }, "Create an acetylcholine neuromodulator");

    m.def("createNorepinephrine", []() -> std::shared_ptr<Neuromodulator> {
        return std::make_shared<Norepinephrine>();
    }, "Create a norepinephrine neuromodulator");

    m.def("createSerotonin", []() -> std::shared_ptr<Neuromodulator> {
        return std::make_shared<Serotonin>();
    }, "Create a serotonin neuromodulator");

    // Create plasticity rule factories
    m.def("createHebbianRule", []() -> std::shared_ptr<PlasticityRule> {
        return std::make_shared<HebbianRule>();
    }, "Create a Hebbian plasticity rule");

    // Create memory system factories
    m.def("createWorkingMemory", []() -> std::shared_ptr<NeuralWorkingMemory> {
        return std::make_shared<NeuralWorkingMemory>();
    }, "Create a working memory system");

    // Create attention system factory
    m.def("createAttentionalSelection", []() -> std::shared_ptr<AttentionalSelection> {
        return std::make_shared<AttentionalSelection>();
    }, "Create an attentional selection system");

    // Create visualization interface
    m.def("createVisualization", []() -> std::shared_ptr<VisualizationInterface> {
        return std::make_shared<VisualizationInterface>();
    }, "Create a visualization interface");

    // Create environment
    m.def("createEnvironment", []() -> std::shared_ptr<Environment> {
        return std::make_shared<GridWorldEnvironment>();
    }, "Create a grid world environment");

    // Create observation
    m.def("createObservation", [](const SensoryInput& input) -> std::unique_ptr<Observation> {
        return std::make_unique<Observation>(input);
    }, py::arg("input"), "Create an observation from sensory input");

    // Expose constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
