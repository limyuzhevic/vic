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
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
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
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input representing visual data with width, height, and channels)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3, "Create vision input with specified dimensions")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the visual data vector")
        .def("getData", &Vision::getData, "Get the visual data as a vector")
        .def("getWidth", &Vision::getWidth, "Get image width in pixels")
        .def("getHeight", &Vision::getHeight, "Get image height in pixels")
        .def("getChannels", &Vision::getChannels, "Get number of color channels (1=grayscale, 3=RGB, 4=RGBA)")
        .def("getType", &Vision::getType, "Get the sensory input type (always Vision)")
        .def("getDimensions", &Vision::getDimensions, "Get dimensionality (width, height, channels) as tuple");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input representing sound data with sample rate and samples)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio input with specified sample rate and number of samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"), "Set the audio data vector")
        .def("getData", &Audio::getData, "Get the audio data as a vector")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sample rate (Hz)")
        .def("getSampleRate", &Audio::getSampleRate, "Get the audio sample rate (Hz)")
        .def("getNumSamples", &Audio::getNumSamples, "Get the number of audio samples")
        .def("getType", &Audio::getType, "Get the sensory input type (always Audio)")
        .def("getDimensions", &Audio::getDimensions, "Get dimensionality (sampleRate, numSamples) as tuple");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input representing internal brain states like emotions, motivations)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals vector")
        .def("getData", &InternalSignals::getData, "Get all internal signals as a vector")
        .def("getType", &InternalSignals::getType, "Get the sensory input type (always InternalSignals)")
        .def("getDimensions", &InternalSignals::getDimensions, "Get dimensionality (number of signals)")
        .def("clearSignals", &InternalSignals::clearSignals, "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output with type and parameters)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"), "Create action with specified type")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters vector")
        .def("getType", &Action::getType, "Get the action type (e.g., MoveForward, TurnLeft)")
        .def("setType", &Action::setType, py::arg("type"), "Set the action type")
        .def("getParameters", &Action::getParameters, "Get the action parameters as vector")
        .def("setParameters", &Action::setParameters, py::arg("params"), "Set the action parameters")
        .def("getName", &Action::getName, "Get the human-readable name of the action type")
        .def("clone", &Action::clone, "Create a copy of this action");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Result of an action execution including reward and success status)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and optional message")
        .def_readwrite("reward", &ActionResult::reward, "Numerical reward value (positive = good, negative = bad)")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Human-readable description of the action result");

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
        .def_readwrite("active", &WorldObject::active);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Result of an action execution including reward and success status)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(), py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create action result with reward, success flag, and optional message")
        .def_readwrite("reward", &ActionResult::reward, "Numerical reward value (positive = good, negative = bad)")
        .def_readwrite("success", &ActionResult::success, "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message, "Human-readable description of the action result");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Combined sensory input from all modalities including vision, touch, internal signals, and audio)pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision, "Get vision sensory input component")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"), "Set vision sensory input component")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth, "Get vision image width")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight, "Get vision image height")
        .def("getTouch", &SensoryPercept::getTouch, "Get touch sensory input component")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"), "Set touch sensory input component")
        .def("getInternal", &SensoryPercept::getInternal, "Get internal signals component")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"), "Set internal signals component")
        .def("getProprioception", &SensoryPercept::getProprioception, "Get proprioception component")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"), "Set proprioception component")
        .def("getAudio", &SensoryPercept::getAudio, "Get audio sensory input component")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"), "Set audio sensory input component")
        .def("getAllSignals", &SensoryPercept::getAllSignals, "Get all sensory signals concatenated into a single vector")
        .def("getTimestamp", &SensoryPercept::getTimestamp, "Get the timestamp of this percept")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"), "Set the timestamp of this percept");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world simulation environment for NLM agents with grid-based terrain)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent's sensor range")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state (position agent, clear resources)")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position in the world")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update the world state for one timestep (move agent, decay resources)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply agent's motor command to the world environment")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the agent's current sensory percept from the environment")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's body state (position, velocity, energy, etc.)")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object (resource, hazard, wall, marker) to the world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove any world object at the specified coordinates")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if coordinates are valid (within world bounds and not blocked)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width in cells")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height in cells")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy capacity")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy capacity")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy natural decay rate per timestep")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy natural decay rate per timestep")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get current simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible world dynamics")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get current random seed");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural processing unit implementing spiking neural computation with integrated memory, prediction, and neuromodulation systems)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), "Create brain with configuration")
        .def("initialize", &Brain::initialize,
             "Initialize the brain (create neurons, regions, and internal systems)")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform one simulation step using current timestep (default 1ms)")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform one simulation step with explicit timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory data into sensory neurons (learns patterns from experience)")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current directly into specific neuron (for testing or external input)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of specified type (sensory, motor, internal)")
        .def("produceAction", &Brain::produceAction,
             "Select and produce motor action based on current neural activity")
        .def("reset", &Brain::reset,
             "Reset brain to initial state (clear spikes, working memory, replay)")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to checkpoint file for later restoration")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from checkpoint file to continue simulation")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region (processing area) with optional name")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a neural region by ID for inspection or modification")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions in the brain")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs as a list")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions as a list (internal reference)")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total number of neurons across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total number of synapses across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of neurons that are currently active (potential)")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of neurons that are currently firing spikes")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons (Hz)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get E/I balance (excitatory/inhibitory weights), higher means more excitation")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count since brain initialization")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events (not yet delivered)")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage (Initial, CriticalPeriod, Maturation, Adult, Aging)")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"),
             "Set developmental stage (affects plasticity rates and learning)")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration used to initialize this brain")
        .def("logStatus", &Brain::logStatus,
             "Log current brain state and statistics to console");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world with sensory processing and motor decoding)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"), "Create agent brain interface with underlying brain")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent with world (sets up sensory input pipes, motor output channels)")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected size of sensory input vector for configuration validation")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected size of motor output vector for configuration validation")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject relevant signals into brain (triggers learning)")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Extract motor command from brain's neural activity patterns")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation to plasticity (dopamine signal)")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update developmental system (affects plasticity rates based on age)")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage of the agent's brain")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level (affects neural excitability)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (drives exploration and novelty seeking)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (detects surprising events)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error (difference between expected and actual)")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode (clear working memory, reset development)")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain object (for direct access to neural components)")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
             "Enable or disable reward-based neuromodulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
             "Enable or disable structural synaptic changes")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
             "Enable or disable developmental changes over time")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
             "Enable or disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

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

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
