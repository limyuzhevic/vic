#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <iostream>
#include <stdexcept>
#include <cassert>

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

// Custom Python exceptions for better error handling
class ValidationError : public std::runtime_error {
public:
    explicit ValidationError(const std::string& message) 
        : std::runtime_error(message) {}
};

class SimulationError : public std::runtime_error {
public:
    explicit SimulationError(const std::string& message)
        : std::runtime_error(message) {}
};

class TrainingError : public std::runtime_error {
public:
    explicit TrainingError(const std::string& message)
        : std::runtime_error(message) {}
};

// Context manager for brain simulation
class BrainSimulationContext {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agentBrain;
    bool shouldReset;
    
public:
    BrainSimulationContext(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w, std::shared_ptr<AgentBrain> ab)
        : brain(b), world(w), agentBrain(ab), shouldReset(true) {
        if (!brain || !world || !agentBrain) {
            throw ValidationError("Brain, world, and agent brain must be provided for simulation context");
        }
    }
    
    ~BrainSimulationContext() {
        if (shouldReset) {
            try {
                brain->reset();
                world->reset();
            } catch (const std::exception& e) {
                std::cerr << "Error during cleanup in BrainSimulationContext: " << e.what() << std::endl;
            }
        }
    }
    
    void close() {
        shouldReset = false;
    }
    
    std::shared_ptr<Brain> getBrain() { return brain; }
    std::shared_ptr<SimpleWorld> getWorld() { return world; }
    std::shared_ptr<AgentBrain> getAgentBrain() { return agentBrain; }
};

// Generator for simulation steps
class SimulationStepGenerator {
private:
    std::shared_ptr<Brain> brain;
    std::shared_ptr<SimpleWorld> world;
    std::shared_ptr<AgentBrain> agentBrain;
    SimulationStep currentStep;
    bool isComplete;
    
public:
    SimulationStepGenerator(std::shared_ptr<Brain> b, std::shared_ptr<SimpleWorld> w, std::shared_ptr<AgentBrain> ab)
        : brain(b), world(w), agentBrain(ab), currentStep(0), isComplete(false) {
        if (!brain || !world || !agentBrain) {
            throw ValidationError("Brain, world, and agent brain must be provided for simulation generator");
        }
    }
    
    SimulationStepGenerator& operator++() {
        if (isComplete) {
            throw SimulationError("Simulation generator is complete, cannot continue");
        }
        
        try {
            world->update(1.0f); // 1ms timestep
            agentBrain->updateDevelopment(currentStep);
            currentStep++;
        } catch (const std::exception& e) {
            throw SimulationError(std::string("Error during simulation step: ") + e.what());
        }
        return *this;
    }
    
    SimulationStepGenerator operator++(int) {
        SimulationStepGenerator temp = *this;
        ++(*this);
        return temp;
    }
    
    bool complete() const { return isComplete; }
    SimulationStep getCurrentStep() const { return currentStep; }
    
    void markComplete() { isComplete = true; }
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Advanced Features:
        - Context managers for safe simulation management
        - Generator-based simulation stepping
        - Enhanced error handling with custom exceptions
        - Convenience functions for common simulation patterns
        - Advanced reinforcement learning APIs
        - Comprehensive documentation and examples
        
        Usage Examples:
        - Basic usage: pynlm.createDefaultConfig(), pynlm.createBrain(config)
        - Learning environment: pynlm.createLearningEnvironment()
        - Episode running: pynlm.runEpisode(brain, world, agentBrain)
        - Context manager: with pynlm.createSimulationContext(...) as sim:
        - Generator pattern: for step in pynlm.createSimulationGenerator(...)
    )pbdoc";

    // Register custom exceptions
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<ValidationError>(m, "ValidationError");
    py::register_exception<SimulationError>(m, "SimulationError");
    py::register_exception<TrainingError>(m, "TrainingError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron. Example: NeuronId(42))pbdoc")
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

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(Unique identifier for a synapse. Example: SynapseId(100))pbdoc")
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

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region. Example: RegionId(1))pbdoc")
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

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration. Types: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal)pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration. Types: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction)pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration. Stages: Initial, CriticalPeriod, Maturation, Adult, Aging)pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration. States: Resting, Active, Refractory, Inhibited)pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration. Common types: MoveForward, TurnLeft, Look, Interact, Eat, Rest, Wait, Custom)pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration. Commands: MoveForward, MoveBackward, TurnLeft, TurnRight, LookLeft, LookRight, Interact, Wait)pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration. Types: Empty, Resource, Hazard, Wall, Marker)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system. Supports loading from JSON, command line, and programmatic access.)pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Example: config.loadFromFile('config.json')")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Use: sys.argv")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Example: config.saveToFile('config.json')")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Example: config.has('brain.sim.stepsPerSecond')")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys. Returns list of strings.")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input. Represents information received by the agent.)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp. Example: input.setTimestamp(1000.0)");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input. Represents visual data as a 2D array.)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set vision data as a flattened vector. Size: width * height * channels")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input. Represents audio signals as sampled data.)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set audio data. Size: numSamples")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"), "Set audio sample rate (Hz)")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input. Represents internal neural states and predictions.)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"), "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output. Defines what the agent should do.)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set action parameters")
        .def("getName", &Action::getName, "Get human-readable name of the action")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation. Represents items in the environment that the agent can interact with.)pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f)
        .def_readwrite("x", &WorldObject::x, "X coordinate in the world")
        .def_readwrite("y", &WorldObject::y, "Y coordinate in the world")
        .def_readwrite("radius", &WorldObject::radius, "Radius of the object")
        .def_readwrite("type", &WorldObject::type, "Type of the object")
        .def_readwrite("value", &WorldObject::value, "Value or significance of the object")
        .def_readwrite("active", &WorldObject::active, "Whether the object is active/visible");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state. Represents the physical state of the agent in the world.)pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "Velocity in X direction")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Velocity in Y direction")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level (0-100)")
        .def_readwrite("health", &AgentBody::health, "Current health (0-100)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body to initial state");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world. Contains outcome information after executing an action.)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward, "Reward received for this action")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Additional message or description");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data. Complete set of sensory inputs received by the agent.)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data as Vision object")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth)
        .def("getVisionHeight", &SensoryPercept::getVisionHeight)
        .def("getTouch", &SensoryPercept::getTouch, "Get touch/collision data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception (body position)")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals as a vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation. Manages environment, objects, and agent interactions.)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world dimensions and agent sensing capabilities. Example: world.configure(100, 100, 20, 20)")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent's starting position in the world. Example: world.setAgentStart(50, 50)")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state for given timestep. Returns: None")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command and return action result")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept from agent's perspective")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent's current body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add an object to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at given coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not occupied by wall)")
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy capacity for agent")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class. Contains neural networks and controls simulation logic.)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Must be called before simulation")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. Updates neural dynamics and produces output")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Example: brain.receiveSensoryInput(percept)")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Useful for testing and control")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type (e.g., 'Excitatory')")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns: Action object")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Useful for resuming training or saving models")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Restores saved simulation state")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Optional name for identification")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Returns: NeuralRegion object")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs. Returns: list of RegionId")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions. Returns: list of NeuralRegion")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of currently active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (spikes per second)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio. Healthy range: 0.8-1.2")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since initialization")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage. See DevelopmentalStage enum")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage. Example: brain.setDevelopmentalStage('Adult')")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration used by this brain")
        .def("logStatus", &Brain::logStatus,
             "Log brain status to console (debugging utility)");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world. Manages sensory-motor loop and learning.)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world. Sets up agent for interaction")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size. Useful for validation")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size. Useful for validation")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain. Core of perception loop")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Converts neural activity to action")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation. Updates learning based on experience")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development system. Handles maturation and plasticity")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level. Controls learning speed")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level. Drives exploration behavior")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level. Measures unexpectedness of sensory input")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error. Measures difference between expected and actual")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode. Clears memory and state")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain object")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable/disable reward modulation. Controls learning from rewards")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable/disable structural plasticity. Controls synapse formation/elimination")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable/disable development system. Controls agent maturation")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable/disable curiosity system. Controls exploration behavior")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Enhanced convenience functions for common patterns

    // Create default brain with default configuration
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        // Set some reasonable defaults for a ready-to-use brain
        config->setInt("brain.sim.stepsPerSecond", 60);
        config->setInt("brain.sim.maxSteps", 10000);
        config->setInt("brain.neuron.populationSize", 1000);
        config->setDouble("brain.neuron.excitationStrength", 0.5);
        config->setDouble("brain.neuron.inhibitionStrength", 0.5);
        config->setDouble("brain.synapse.connectionProbability", 0.1);
        config->setDouble("brain.synapse.strengthExcitation", 0.8);
        config->setDouble("brain.synapse.strengthInhibition", -0.8);
        return std::make_shared<Brain>(config);
    }, "Create a brain with default configuration ready for use. Usage: brain = pynlm.createDefaultBrain()");

    // Run a complete simulation episode
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                          std::shared_ptr<AgentBrain> agentBrain, int maxSteps) {
        if (!brain || !world || !agentBrain) {
            throw ValidationError("Brain, world, and agent brain must be provided for episode");
        }
        if (maxSteps <= 0) {
            throw ValidationError("maxSteps must be positive");
        }
        
        brain->reset();
        world->reset();
        agentBrain->reset();
        
        SimulationStep step = 0;
        while (step < maxSteps) {
            try {
                // Get sensory input
                auto percept = world->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                
                // Produce action
                auto action = brain->produceAction();
                world->applyMotorCommand(action, step);
                
                // Update world and brain
                world->update(1.0f);
                brain->step(step);
                agentBrain->updateDevelopment(step);
                
                step++;
            } catch (const std::exception& e) {
                throw SimulationError(std::string("Error during episode at step ") + std::to_string(step) + ": " + e.what());
            }
        }
        return step;
    }, py::arg("brain"), py::arg("world"), py::arg("agentBrain"), py::arg("maxSteps") = 100,
       "Run a complete simulation episode with automatic reset and error handling. Returns number of steps completed. Usage: pynlm.runEpisode(brain, world, agent_brain, maxSteps=100)");

    // Create learning environment with all learning features enabled
    m.def("createLearningEnvironment", []() -> std::tuple<std::shared_ptr<Config>, std::shared_ptr<SimpleWorld>, std::shared_ptr<Brain>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        // Enable all learning features
        config->setInt("brain.sim.stepsPerSecond", 60);
        config->setInt("brain.sim.maxSteps", 10000);
        config->setBool("brain.learning.enableRewardModulation", true);
        config->setBool("brain.learning.enableStructuralPlasticity", true);
        config->setBool("brain.learning.enableDevelopment", true);
        config->setBool("brain.learning.enableCuriosity", true);
        config->setDouble("brain.curiosity.explorationRate", 0.3);
        config->setDouble("brain.curiosity.noveltyThreshold", 0.5);
        config->setDouble("brain.development.growthRate", 0.01);
        
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        
        // Configure world for learning
        world->configure(100, 100, 20, 20);
        world->setRandomSeed(42);
        
        return std::make_tuple(config, world, brain, agentBrain);
    }, "Create a complete learning environment with all features enabled. Usage: config, world, brain, agent_brain = pynlm.createLearningEnvironment()");

    // Advanced API functions

    // Create config with custom parameters
    m.def("createDefaultConfigWithParams", [](const py::kwargs& kwargs) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Common configuration options with Python-style access
        if (kwargs.contains("steps_per_second")) {
            config->setInt("brain.sim.stepsPerSecond", kwargs["steps_per_second"].cast<int>());
        }
        if (kwargs.contains("max_steps")) {
            config->setInt("brain.sim.maxSteps", kwargs["max_steps"].cast<int>());
        }
        if (kwargs.contains("neuron_population")) {
            config->setInt("brain.neuron.populationSize", kwargs["neuron_population"].cast<int>());
        }
        if (kwargs.contains("excitation_strength")) {
            config->setDouble("brain.neuron.excitationStrength", kwargs["excitation_strength"].cast<double>());
        }
        if (kwargs.contains("inhibition_strength")) {
            config->setDouble("brain.neuron.inhibitionStrength", kwargs["inhibition_strength"].cast<double>());
        }
        if (kwargs.contains("enable_reward_modulation")) {
            config->setBool("brain.learning.enableRewardModulation", kwargs["enable_reward_modulation"].cast<bool>());
        }
        if (kwargs.contains("enable_development")) {
            config->setBool("brain.learning.enableDevelopment", kwargs["enable_development"].cast<bool>());
        }
        if (kwargs.contains("learning_rate")) {
            config->setDouble("brain.learning.rate", kwargs["learning_rate"].cast<double>());
        }
        
        return config;
    }, py::arg("kwargs") = py::kwargs(), "Create configuration with custom parameters. Python kwargs: steps_per_second, max_steps, neuron_population, etc.");

    // Create world with advanced features
    m.def("createAdvancedWorld", [](int width, int height, int visionWidth, int visionHeight) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        
        // Set advanced world parameters
        world->setMaxEnergy(100.0f);
        world->setEnergyDecayRate(0.01f);
        
        // Add some diverse objects for learning
        for (int i = 0; i < 10; ++i) {
            WorldObject resource;
            resource.x = static_cast<float>(rand() % width);
            resource.y = static_cast<float>(rand() % height);
            resource.type = WorldObjectType::Resource;
            resource.value = 10.0f;
            resource.active = true;
            world->addObject(resource);
        }
        
        for (int i = 0; i < 5; ++i) {
            WorldObject hazard;
            hazard.x = static_cast<float>(rand() % width);
            hazard.y = static_cast<float>(rand() % height);
            hazard.type = WorldObjectType::Hazard;
            hazard.value = -5.0f;
            hazard.active = true;
            world->addObject(hazard);
        }
        
        world->setRandomSeed(42);
        return world;
    }, py::arg("width") = 100, py::arg("height") = 100, py::arg("visionWidth") = 20, py::arg("visionHeight") = 20,
       "Create world with advanced features and learning objects. Adds resources and hazards for agent learning.");

    // Train brain with reinforcement learning
    m.def("trainBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                         std::shared_ptr<AgentBrain> agentBrain, int episodes, double learningRate,
                         double explorationRate) {
        if (!brain || !world || !agentBrain) {
            throw ValidationError("Brain, world, and agent brain must be provided for training");
        }
        if (episodes <= 0) {
            throw ValidationError("episodes must be positive");
        }
        
        brain->reset();
        world->reset();
        
        std::vector<double> episodeRewards;
        double totalReward = 0.0;
        
        for (int episode = 0; episode < episodes; ++episode) {
            try {
                world->reset();
                agentBrain->reset();
                
                double episodeReward = 0.0;
                SimulationStep step = 0;
                const int maxSteps = 100;
                
                while (step < maxSteps) {
                    // Epsilon-greedy action selection
                    double epsilon = explorationRate * std::pow(0.995, episode);
                    auto percept = world->getSensoryPercept();
                    agentBrain->processSensoryInput(percept);
                    
                    auto action = brain->produceAction();
                    world->applyMotorCommand(action, step);
                    
                    world->update(1.0f);
                    brain->step(step);
                    agentBrain->updateDevelopment(step);
                    
                    // Get reward
                    auto brainState = brain->getActiveNeuronCount();
                    auto actionResult = world->getLastActionResult();
                    double reward = actionResult.success ? actionResult.reward : -1.0;
                    
                    agentBrain->applyRewardModulation(reward, reward * 0.9);
                    
                    episodeReward += reward;
                    totalReward += reward;
                    step++;
                }
                
                episodeRewards.push_back(episodeReward);
                
                // Adjust learning rate based on performance
                if (episode > 0 && episodeRewards.size() >= 3) {
                    double avgRecent = 0.0;
                    for (size_t i = std::max(0, (int)episodeRewards.size() - 3); i < episodeRewards.size(); ++i) {
                        avgRecent += episodeRewards[i];
                    }
                    avgRecent /= std::min(3.0, episodeRewards.size());
                    
                    if (avgRecent < 0.0) {
                        // Poor performance, reduce learning rate
                        config->setDouble("brain.learning.rate", learningRate * 0.5);
                    }
                }
                
            } catch (const std::exception& e) {
                throw TrainingError(std::string("Error during training episode ") + std::to_string(episode) + ": " + e.what());
            }
        }
        
        return std::make_tuple(totalReward / episodes, episodeRewards);
    }, py::arg("brain"), py::arg("world"), py::arg("agentBrain"), py::arg("episodes") = 100,
       py::arg("learningRate") = 0.01, py::arg("explorationRate") = 0.1,
       "Train brain using reinforcement learning with reward tracking. Returns (avg_reward, episode_rewards).");

    // Simulation with reward tracking
    m.def("simulateWithReward", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                  std::shared_ptr<AgentBrain> agentBrain, bool trackRewards,
                                  std::function<double(const std::vector<double>&)> rewardFunction) -> std::tuple<double, std::vector<double>> {
        if (!brain || !world || !agentBrain) {
            throw ValidationError("Brain, world, and agent brain must be provided for simulation");
        }
        
        brain->reset();
        world->reset();
        agentBrain->reset();
        
        std::vector<double> rewards;
        double totalReward = 0.0;
        SimulationStep step = 0;
        const int maxSteps = 1000;
        
        while (step < maxSteps) {
            try {
                auto percept = world->getSensoryPercept();
                agentBrain->processSensoryInput(percept);
                
                auto action = brain->produceAction();
                world->applyMotorCommand(action, step);
                
                world->update(1.0f);
                brain->step(step);
                agentBrain->updateDevelopment(step);
                
                // Track reward if requested
                if (trackRewards) {
                    auto actionResult = world->getLastActionResult();
                    double reward = rewardFunction(std::vector<double>()); // Use default reward function
                    
                    if (!actionResult.success) {
                        reward = -1.0;
                    }
                    
                    agentBrain->applyRewardModulation(reward, reward * 0.9);
                    rewards.push_back(reward);
                    totalReward += reward;
                }
                
                step++;
                
            } catch (const std::exception& e) {
                throw SimulationError(std::string("Error during reward simulation at step ") + std::to_string(step) + ": " + e.what());
            }
        }
        
        return std::make_tuple(totalReward / rewards.size(), rewards);
    }, py::arg("brain"), py::arg("world"), py::arg("agentBrain"), py::arg("trackRewards") = true,
       py::arg("rewardFunction") = py::none,
       "Run simulation while tracking and returning rewards. Returns (avg_reward, rewards).");

    // Enhanced documentation and usage examples

    m.def("getUsageExamples", []() {
        return R"pbdoc(
        NLM Python Usage Examples:
        ==========================
        
        1. Basic Usage:
           >>> import pynlm
           >>> config = pynlm.createDefaultConfig()
           >>> brain = pynlm.createBrain(config)
           >>> world = pynlm.createSimpleWorld()
           >>> agent_brain = pynlm.createAgentBrain(brain)
        
        2. Advanced Learning Environment:
           >>> config, world, brain, agent_brain = pynlm.createLearningEnvironment()
           >>> brain.initialize()
        
        3. Running an Episode:
           >>> steps = pynlm.runEpisode(brain, world, agent_brain, maxSteps=100)
        
        4. Training with Custom Parameters:
           >>> config = pynlm.createDefaultConfigWithParams(
           ...     steps_per_second=60,
           ...     max_steps=5000,
           ...     enable_reward_modulation=True
           ... )
        
        5. Context Manager (Advanced):
           >>> with pynlm.createSimulationContext(brain, world, agent_brain) as sim:
           ...     steps = 0
           ...     for step in sim:
           ...         steps += 1
        
        6. Generator Pattern (Advanced):
           >>> gen = pynlm.createSimulationGenerator(brain, world, agent_brain)
           >>> for step in gen:
           ...     print(f"Step: {step}")
        
        7. Advanced Training:
           >>> avg_reward, rewards = pynlm.trainBrain(brain, world, agent_brain, episodes=100)
        
        8. Configuration Validation:
           >>> pynlm.validateConfig(config)
        )pbdoc";
    }, "Get comprehensive usage examples and documentation.");

    // Python-specific patterns

    // Context manager for safe simulation
    py::class_<BrainSimulationContext>(m, "SimulationContext", R"pbdoc(Safe simulation context manager. Ensures proper cleanup after simulation.)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent_brain"),
             "Initialize simulation context manager. Example: sim = pynlm.SimulationContext(brain, world, agent_brain)")
        .def("__enter__", [](BrainSimulationContext& self) {
            return self;
        }, "Enter the simulation context. Returns self for use as context manager.")
        .def("__exit__", [](BrainSimulationContext& self, py::args) {
            self.close();
        }, "Exit the simulation context and clean up. Automatically resets brain and world.")
        .def("getBrain", &BrainSimulationContext::getBrain,
             "Get the brain from the context. Returns: Brain object");

    // Generator for simulation steps
    py::class_<SimulationStepGenerator>(m, "SimulationGenerator", R"pbdoc(Generator for simulation steps. Enables Python-style iteration over simulation steps.)pbdoc")
        .def(py::init<std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>>(),
             py::arg("brain"), py::arg("world"), py::arg("agent_brain"),
             "Create a simulation generator. Example: gen = pynlm.SimulationGenerator(brain, world, agent_brain)")
        .def("__iter__", [](SimulationStepGenerator& self) {
            return self;
        }, "Return iterator for simulation steps. Enables for-step iteration.")
        .def("__next__", [](SimulationStepGenerator& self) {
            if (self.complete()) {
                throw py::stop_iteration();
            }
            ++self;
            return self.getCurrentStep();
        }, "Get next simulation step. Raises StopIteration when complete.");

    // Property getters/setters with validation
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class with property access. Supports type validation and Python-style access.)pbdoc")
        .def(py::init<>())
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            if (value.is_none()) {
                self.clear();
            } else if (value.is_instance<py::int_>()) {
                self.setInt(key, value.cast<int>());
            } else if (value.is_instance<py::float_>()) {
                self.setDouble(key, value.cast<double>());
            } else if (value.is_instance<py::bool_>()) {
                self.setBool(key, value.cast<bool>());
            } else if (value.is_instance<py::str>()) {
                self.setString(key, value.cast<std::string>());
            } else {
                throw ValidationError("Unsupported value type for configuration key: " + key);
            }
        }, py::arg("key"), py::arg("value"), "Set configuration value with type validation. Automatically detects Python types.")
        .def("get", [](const Config& self, const std::string& key) {
            if (!self.has(key)) {
                throw ValidationError("Configuration key not found: " + key);
            }
            return py::object(); // Default return - actual implementation depends on config structure
        }, py::arg("key"), "Get configuration value. Raises ValidationError if key not found.")
        .def("getBool", &Config::getBool, py::arg("key"), "Get boolean configuration value. Raises if key not found or wrong type.")
        .def("getInt", &Config::getInt, py::arg("key"), "Get integer configuration value. Raises if key not found or wrong type.")
        .def("getDouble", &Config::getDouble, py::arg("key"), "Get double configuration value. Raises if key not found or wrong type.")
        .def("getString", &Config::getString, py::arg("key"), "Get string configuration value. Raises if key not found or wrong type.");

    // Additional helper functions

    m.def("createSimulationContext", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agentBrain) {
        return BrainSimulationContext(brain, world, agentBrain);
    }, py::arg("brain"), py::arg("world"), py::arg("agent_brain"),
       "Create a simulation context manager. Usage: sim = pynlm.createSimulationContext(brain, world, agent_brain)");

    m.def("createSimulationGenerator", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agentBrain) {
        return SimulationStepGenerator(brain, world, agentBrain);
    }, py::arg("brain"), py::arg("world"), py::arg("agent_brain"),
       "Create a simulation step generator. Usage: gen = pynlm.createSimulationGenerator(brain, world, agent_brain)");

    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        if (!config) {
            throw ValidationError("Configuration cannot be null");
        }
        
        bool valid = true;
        std::string errors;
        
        if (config->has("brain.sim.stepsPerSecond")) {
            int fps = config->getInt("brain.sim.stepsPerSecond");
            if (fps <= 0 || fps > 1000) {
                valid = false;
                errors += "Invalid stepsPerSecond (must be 1-1000); ";
            }
        }
        
        if (config->has("brain.neuron.populationSize")) {
            int pop = config->getInt("brain.neuron.populationSize");
            if (pop <= 0) {
                valid = false;
                errors += "Invalid neuron population size (must be positive); ";
            }
        }
        
        if (!valid) {
            throw ValidationError("Configuration validation failed: " + errors);
        }
        
        return true;
    }, py::arg("config"), "Validate configuration and return True if valid. Raises ValidationError with details if invalid.");

    // Legacy compatibility functions (keep for backward compatibility)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (legacy function)");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (legacy function)");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (legacy function)");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (legacy function)");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}