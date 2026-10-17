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

// Template function for creating ID type bindings
template<typename IDType, typename IDDoc>
void bindIdType(py::class_<IDType> &cls, IDDoc docString) {
    cls.def(py::init<>(), "Create an invalid ID (default)")
       .def(py::init<uint64_t>(), py::arg("value"),
            "Create ID from uint64_t value")
       .def_readwrite("value", &IDType::value, "Get or set the ID value")
       .def("index", &IDType::index, "Get the index of the ID")
       .def("__eq__", &IDType::operator==, "Check equality with another ID")
       .def("__ne__", &IDType::operator!=, "Check inequality with another ID")
       .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); },
            "Get hash value for the ID")
       .def("__repr__", [](const IDType& id) {
            return "<" + std::string(docString).substr(0, std::string(docString).find(">")) +
                   ": " + std::to_string(id.value) + ">";
        }, "Get string representation of ID");
    
    // Python validation for ID values
    cls.def(py::init<uint64_t>(), py::arg("value").noconvert(),
            "Create ID with validation",
            [](uint64_t v) {
                if (v >= UINT64_MAX) {
                    throw py::value_error("ID value cannot be UINT64_MAX");
                }
                return IDType(v);
            });
}

// Constants extracted from magic numbers
namespace {
    constexpr float DEFAULT_WIDTH = 100.0f;
    constexpr float DEFAULT_HEIGHT = 100.0f;
    constexpr float DEFAULT_VISION_WIDTH = 20;
    constexpr float DEFAULT_VISION_HEIGHT = 20;
    constexpr float DEFAULT_RADIUS = 0.5f;
    constexpr float DEFAULT_MOVE_SPEED = 3.0f;
    constexpr float DEFAULT_TURN_SPEED = 2.0f;
    constexpr float MAX_SPEED = 5.0f;
    constexpr float MIN_SPEED = 0.1f;
    constexpr float MAX_TURN_RATE = 3.0f;
    constexpr float MIN_TURN_RATE = 0.01f;
    constexpr float ENERGY_DECAY_RATE = 0.01f;
    constexpr float MAX_ENERGY = 100.0f;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Use template function to reduce duplication for ID types
    auto bindNeuronId = [&](auto& cls) {
        cls.def(py::init<>())
           .def(py::init<uint64_t>(), py::arg("value"), "Create ID from uint64_t value")
           .def_readwrite("value", &NeuronId::value, "Get or set the ID value")
           .def("index", &NeuronId::index, "Get the index of the ID")
           .def("__eq__", &NeuronId::operator==, "Check equality with another ID")
           .def("__ne__", &NeuronId::operator!=, "Check inequality with another ID")
           .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
                "Get hash value for the ID")
           .def("__repr__", [](const NeuronId& id) {
               return "<NeuronId: " + std::to_string(id.value) + ">";
           }, "Get string representation of ID");
    };

    py::class_<NeuronId> neuronIdCls(m, "NeuronId", "Unique identifier for a neuron");
    bindNeuronId(neuronIdCls);

    auto bindSynapseId = [&](auto& cls) {
        cls.def(py::init<>())
           .def(py::init<uint64_t>(), py::arg("value"), "Create ID from uint64_t value")
           .def_readwrite("value", &SynapseId::value, "Get or set the ID value")
           .def("index", &SynapseId::index, "Get the index of the ID")
           .def("__eq__", &SynapseId::operator==, "Check equality with another ID")
           .def("__ne__", &SynapseId::operator!=, "Check inequality with another ID")
           .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
                "Get hash value for the ID")
           .def("__repr__", [](const SynapseId& id) {
               return "<SynapseId: " + std::to_string(id.value) + ">";
           }, "Get string representation of ID");
    };

    py::class_<SynapseId> synapseIdCls(m, "SynapseId", "Unique identifier for a synapse");
    bindSynapseId(synapseIdCls);

    auto bindRegionId = [&](auto& cls) {
        cls.def(py::init<>())
           .def(py::init<uint64_t>(), py::arg("value"), "Create ID from uint64_t value")
           .def_readwrite("value", &RegionId::value, "Get or set the ID value")
           .def("index", &RegionId::index, "Get the index of the ID")
           .def("__eq__", &RegionId::operator==, "Check equality with another ID")
           .def("__ne__", &RegionId::operator!=, "Check inequality with another ID")
           .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
                "Get hash value for the ID")
           .def("__repr__", [](const RegionId& id) {
               return "<RegionId: " + std::to_string(id.value) + ">";
           }, "Get string representation of ID");
    };

    py::class_<RegionId> regionIdCls(m, "RegionId", "Unique identifier for a brain region");
    bindRegionId(regionIdCls);

    auto bindPopulationId = [&](auto& cls) {
        cls.def(py::init<>())
           .def(py::init<uint64_t>(), py::arg("value"), "Create ID from uint64_t value")
           .def_readwrite("value", &PopulationId::value, "Get or set the ID value")
           .def("index", &PopulationId::index, "Get the index of the ID")
           .def("__eq__", &PopulationId::operator==, "Check equality with another ID")
           .def("__ne__", &PopulationId::operator!=, "Check inequality with another ID");
    };

    py::class_<PopulationId> populationIdCls(m, "PopulationId", "Unique identifier for a neuron population");
    bindPopulationId(populationIdCls);

    py::enum_<NeuronType>(m, "NeuronType", "Neuron type enumeration")
        .value("Excitatory", NeuronType::Excitatory, "Excitatory neuron type")
        .value("Inhibitory", NeuronType::Inhibitory, "Inhibitory neuron type")
        .value("Modulatory", NeuronType::Modulatory, "Modulatory neuron type")
        .value("Sensory", NeuronType::Sensory, "Sensory neuron type")
        .value("Motor", NeuronType::Motor, "Motor neuron type")
        .value("Internal", NeuronType::Internal, "Internal neuron type")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", "Synapse type enumeration")
        .value("Excitatory", SynapseType::Excitatory, "Excitatory synapse type")
        .value("Inhibitory", SynapseType::Inhibitory, "Inhibitory synapse type")
        .value("Modulatory", SynapseType::Modulatory, "Modulatory synapse type")
        .value("Electrical", SynapseType::Electrical, "Electrical synapse type")
        .value("GapJunction", SynapseType::GapJunction, "Gap junction synapse type")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", "Developmental stage enumeration")
        .value("Initial", DevelopmentalStage::Initial, "Initial developmental stage")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Critical period stage")
        .value("Maturation", DevelopmentalStage::Maturation, "Maturation stage")
        .value("Adult", DevelopmentalStage::Adult, "Adult stage")
        .value("Aging", DevelopmentalStage::Aging, "Aging stage")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", "Neuron firing state enumeration")
        .value("Resting", FiringState::Resting, "Neuron is resting")
        .value("Active", FiringState::Active, "Neuron is actively firing")
        .value("Refractory", FiringState::Refractory, "Neuron is in refractory period")
        .value("Inhibited", FiringState::Inhibited, "Neuron is inhibited")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", "Action type enumeration")
        .value("MoveForward", ActionType::MoveForward, "Move forward action")
        .value("MoveBackward", ActionType::MoveBackward, "Move backward action")
        .value("MoveLeft", ActionType::MoveLeft, "Move left action")
        .value("MoveRight", ActionType::MoveRight, "Move right action")
        .value("TurnLeft", ActionType::TurnLeft, "Turn left action")
        .value("TurnRight", ActionType::TurnRight, "Turn right action")
        .value("Look", ActionType::Look, "Look action")
        .value("LookUp", ActionType::LookUp, "Look up action")
        .value("LookDown", ActionType::LookDown, "Look down action")
        .value("Interact", ActionType::Interact, "Interact action")
        .value("Eat", ActionType::Eat, "Eat action")
        .value("Drink", ActionType::Drink, "Drink action")
        .value("Rest", ActionType::Rest, "Rest action")
        .value("Wait", ActionType::Wait, "Wait action")
        .value("Custom", ActionType::Custom, "Custom action")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", "Low-level motor command enumeration")
        .value("MoveForward", MotorCommand::MoveForward, "Move forward motor command")
        .value("MoveBackward", MotorCommand::MoveBackward, "Move backward motor command")
        .value("TurnLeft", MotorCommand::TurnLeft, "Turn left motor command")
        .value("TurnRight", MotorCommand::TurnRight, "Turn right motor command")
        .value("LookLeft", MotorCommand::LookLeft, "Look left motor command")
        .value("LookRight", MotorCommand::LookRight, "Look right motor command")
        .value("Interact", MotorCommand::Interact, "Interact motor command")
        .value("Wait", MotorCommand::Wait, "Wait motor command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", "World object type enumeration")
        .value("Empty", WorldObjectType::Empty, "Empty world object")
        .value("Resource", WorldObjectType::Resource, "Resource world object")
        .value("Hazard", WorldObjectType::Hazard, "Hazard world object")
        .value("Wall", WorldObjectType::Wall, "Wall world object")
        .value("Marker", WorldObjectType::Marker, "Marker world object")
        .export_values();

    py::class_<Config>(m, "Config", "Configuration class for NLM system")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Filepath must be a valid path.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments. Args should be a list of strings.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Filepath must be a valid path.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Key must be a non-empty string.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        }, "Get string representation of the configuration");

    py::class_<SensoryInput>(m, "SensoryInput", "Base class for sensory input")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp. Timestamp must be a non-negative value.");

    // Vision sensory input with parameter validation
    py::class_<Vision, SensoryInput>(m, "Vision", "Vision sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create Vision input. Width, height, and channels must be positive integers.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Vision data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set vision data. Data must be a non-empty vector of floats.")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio sensory input with parameter validation
    py::class_<Audio, SensoryInput>(m, "Audio", "Audio sensory input")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio input. Sample rate and number of samples must be positive.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.empty()) {
                throw py::value_error("Audio data cannot be empty");
            }
            self.setData(data);
        }, py::arg("data"), "Set audio data. Data must be a non-empty vector of floats.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set sample rate. Sample rate must be positive.")
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals sensory input
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", "Internal signals sensory input")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value. Value should be a finite float.")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all signals");

    // Action class with validation
    py::class_<Action>(m, "Action", "Action representation for motor output")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create Action with type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create Action with type and parameters")
        .def("getType", &Action::getType, "Get the action type")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters. Params should be a vector of floats.")
        .def("getName", &Action::getName, "Get the action name")
        .def("clone", &Action::clone, "Create a copy of this action");

    // WorldObject class with validation
    py::class_<WorldObject>(m, "WorldObject", "World object representation")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create WorldObject. x, y must be finite, type must be valid, radius must be positive.")
        .def_readwrite("x", &WorldObject::x, "Object x position")
        .def_readwrite("y", &WorldObject::y, "Object y position")
        .def_readwrite("radius", &WorldObject::radius, "Object radius")
        .def_readwrite("type", &WorldObject::type, "Object type")
        .def_readwrite("value", &WorldObject::value, "Object value (reward/damage)")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    // AgentBody class
    py::class_<AgentBody>(m, "AgentBody", "Agent body state")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "Agent x position")
        .def_readwrite("y", &AgentBody::y, "Agent y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Agent orientation in radians")
        .def_readwrite("velocityX", &AgentBody::velocityX, "Agent velocity in x direction")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Agent velocity in y direction")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Agent angular velocity")
        .def_readwrite("energy", &AgentBody::energy, "Agent energy level")
        .def_readwrite("health", &AgentBody::health, "Agent health level")
        .def_readwrite("age", &AgentBody::age, "Agent age in steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset agent body to initial state");

    py::class_<ActionResult>(m, "ActionResult", "Action result from world")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create ActionResult. Reward is a float value, success is a boolean.")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action was successful")
        .def_readwrite("message", &ActionResult::message, "Message describing the result");

    py::class_<SensoryPercept>(m, "SensoryPercept", "Sensory percept data")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch data")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception data")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio data")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", "Simple 2D world for NLM simulation")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure world with dimensions. All parameters must be positive.")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position. x and y must be within world bounds.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state. Timestep must be positive.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply motor command. cmd must be a valid MotorCommand.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add object to world. Object must be valid.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid (not wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy. Must be positive.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate. Must be positive.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducibility")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

    // Brain class with improved type annotations and validation
    py::class_<Brain>(m, "Brain", "Central neural simulation brain class")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create brain with configuration. Config must not be null.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. currentStep must be non-negative.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. Both parameters must be non-negative.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. neuron must be valid, current must be finite.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. filepath must be valid.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. filepath must be valid.")
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
             py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status");

    // AgentBrain class with smart pointer return for getBrain()
    py::class_<AgentBrain>(m, "AgentBrain", "Agent brain interface connecting NLM brain to world")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create AgentBrain with brain. Brain must not be null.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"), "Update development system")
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
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"), "Enable or disable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"), "Enable or disable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"), "Enable or disable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"), "Enable or disable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Factory functions
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

    // Invalid ID sentinels
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
