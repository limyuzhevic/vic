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
        .def("clone", &Action::clone);

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

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

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
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"));

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
    // Enhanced world configuration and control
    .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("max"),
         "Set the maximum energy level for the world")
    .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("rate"),
         "Set the energy decay rate")
    .def("getEnergy", &SimpleWorld::getEnergy, "Get current energy")
    .def("setVisibilityRange", &SimpleWorld::setVisibilityRange, py::arg("range"),
         "Set the sensory visibility range")
    .def("setObstacleDensity", &SimpleWorld::setObstacleDensity, py::arg("density"),
         "Set the density of obstacles in the world")
    .def("setResourceDensity", &SimpleWorld::setResourceDensity, py::arg("density"),
         "Set the density of resources in the world")
    .def("setHazardDensity", &SimpleWorld::setHazardDensity, py::arg("density"),
         "Set the density of hazards in the world")
    .def("addResource", &SimpleWorld::addResource, py::arg("value"), py::arg("radius") = 0.5f,
         "Add a resource at random position")
    .def("addHazard", &SimpleWorld::addHazard, py::arg("damage"), py::arg("radius") = 0.5f,
         "Add a hazard at random position")
    .def("clearWorld", &SimpleWorld::clearWorld,
         "Clear all objects from the world")
    .def("exportWorld", &SimpleWorld::exportWorld, py::arg("filepath"),
         "Export world configuration to file")
    .def("importWorld", &SimpleWorld::importWorld, py::arg("filepath"),
         "Import world configuration from file");

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
             "Log brain status");

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
    // Advanced brain configuration methods
    .def("configure", &Brain::configure, py::arg("config"),
         "Configure brain with a Config object")
    .def("setNeuronCount", &Brain::setNeuronCount, py::arg("count"),
         "Set the number of neurons in the brain")
    .def("setRegionCount", &Brain::setRegionCount, py::arg("count"),
         "Set the number of regions in the brain")
    .def("setConnectionProbability", &Brain::setConnectionProbability, py::arg("prob"),
         "Set the probability of synaptic connections")
    .def("enablePlasticity", &Brain::enablePlasticity, py::arg("plastic"),
         "Enable or disable synaptic plasticity")
    .def("isPlasticityEnabled", &Brain::isPlasticityEnabled,
         "Check if plasticity is enabled")
    .def("setDevelopmentEnabled", &Brain::setDevelopmentEnabled, py::arg("enabled"),
         "Enable or disable brain development")
    .def("isDevelopmentEnabled", &Brain::isDevelopmentEnabled,
         "Check if development is enabled")
    .def("setCuriosityEnabled", &Brain::setCuriosityEnabled, py::arg("enabled"),
         "Enable or disable curiosity-driven exploration")
    .def("isCuriosityEnabled", &Brain::isCuriosityEnabled,
         "Check if curiosity is enabled")
    .def("enablePredictionError", &Brain::enablePredictionError, py::arg("enabled"),
         "Enable or disable prediction error processing")
    .def("isPredictionErrorEnabled", &Brain::isPredictionErrorEnabled,
         "Check if prediction error is enabled")
    .def("setSimulationTimestep", &Brain::setSimulationTimestep, py::arg("timestep"),
         "Set the simulation timestep (in seconds)")
    .def("getSimulationTimestep", &Brain::getSimulationTimestep,
         "Get the simulation timestep")
    .def("setMaxSteps", &Brain::setMaxSteps, py::arg("steps"),
         "Set the maximum number of simulation steps")
    .def("getMaxSteps", &Brain::getMaxSteps, "Get the maximum number of simulation steps")
    .def("setRandomSeed", &Brain::setRandomSeed, py::arg("seed"),
         "Set the random seed for reproducible simulations")
    .def("getRandomSeed", &Brain::getRandomSeed, "Get the random seed")
    .def("setCheckpointDir", &Brain::setCheckpointDir, py::arg("dir"),
         "Set the checkpoint directory for saving brain states")
    .def("getCheckpointDir", &Brain::getCheckpointDir, "Get the checkpoint directory")
    .def("setMaxCheckpoints", &Brain::setMaxCheckpoints, py::arg("max"),
         "Set the maximum number of checkpoints to keep")
    .def("getMaxCheckpoints", &Brain::getMaxCheckpoints, "Get the maximum number of checkpoints")
    .def("enableMemoryReplay", &Brain::enableMemoryReplay, py::arg("enable"),
         "Enable or disable memory replay during simulation")
    .def("isMemoryReplayEnabled", &Brain::isMemoryReplayEnabled,
         "Check if memory replay is enabled")
    .def("setReplayInterval", &Brain::setReplayInterval, py::arg("interval"),
         "Set the replay interval (in steps)")
    .def("getReplayInterval", &Brain::getReplayInterval, "Get the replay interval")
    .def("setConsolidationInterval", &Brain::setConsolidationInterval, py::arg("interval"),
         "Set the memory consolidation interval (in steps)")
    .def("getConsolidationInterval", &Brain::getConsolidationInterval, "Get the consolidation interval")

        .def("enablePlasticity", &AgentBrain::enablePlasticity, py::arg("plastic"),
             "Enable or disable synaptic plasticity")
        .def("isPlasticityEnabled", &AgentBrain::isPlasticityEnabled,
             "Check if plasticity is enabled")
        .def("setDevelopmentEnabled", &AgentBrain::setDevelopmentEnabled, py::arg("enabled"),
             "Enable or disable brain development")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("setCuriosityEnabled", &AgentBrain::setCuriosityEnabled, py::arg("enabled"),
             "Enable or disable curiosity-driven exploration")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled")
        .def("enablePredictionError", &AgentBrain::enablePredictionError, py::arg("enabled"),
             "Enable or disable prediction error processing")
        .def("isPredictionErrorEnabled", &AgentBrain::isPredictionErrorEnabled,
             "Check if prediction error is enabled")
        .def("setNoveltyWeight", &AgentBrain::setNoveltyWeight, py::arg("weight"),
             "Set the weight of novelty in curiosity calculation")
        .def("getNoveltyWeight", &AgentBrain::getNoveltyWeight,
             "Get the weight of novelty in curiosity calculation")
        .def("setPredictionErrorWeight", &AgentBrain::setPredictionErrorWeight, py::arg("weight"),
             "Set the weight of prediction error in curiosity calculation")
        .def("getPredictionErrorWeight", &AgentBrain::getPredictionErrorWeight,
             "Get the weight of prediction error in curiosity calculation")
        .def("setLearningRate", &AgentBrain::setLearningRate, py::arg("rate"),
             "Set the learning rate for plasticity")
        .def("getLearningRate", &AgentBrain::getLearningRate,
             "Get the learning rate for plasticity")
        .def("setExplorationTemperature", &AgentBrain::setExplorationTemperature, py::arg("temp"),
             "Set the exploration temperature for action selection")
        .def("getExplorationTemperature", &AgentBrain::getExplorationTemperature,
             "Get the exploration temperature for action selection")
        .def("setActionSelectionMethod", &AgentBrain::setActionSelectionMethod, py::arg("method"),
             "Set the action selection method (0: deterministic, 1: stochastic, 2: curiosity-driven)")
        .def("getActionSelectionMethod", &AgentBrain::getActionSelectionMethod,
             "Get the action selection method")
        .def("setSuccessCount", &AgentBrain::setSuccessCount, py::arg("count"),
             "Set the number of consecutive successful actions for skill acquisition")
        .def("getSuccessCount", &AgentBrain::getSuccessCount,
             "Get the number of consecutive successful actions")
        .def("setFailureCount", &AgentBrain::setFailureCount, py::arg("count"),
             "Set the number of consecutive failed actions for punishment")
        .def("getFailureCount", &AgentBrain::getFailureCount,
             "Get the number of consecutive failed actions")
        .def("setRewardThreshold", &AgentBrain::setRewardThreshold, py::arg("threshold"),
             "Set the reward threshold for behavior reinforcement")
        .def("getRewardThreshold", &AgentBrain::getRewardThreshold,
             "Get the reward threshold for behavior reinforcement")
        .def("setPunishmentThreshold", &AgentBrain::setPunishmentThreshold, py::arg("threshold"),
             "Set the punishment threshold for behavior inhibition")
        .def("getPunishmentThreshold", &AgentBrain::getPunishmentThreshold,
             "Get the punishment threshold for behavior inhibition")
        .def("setSkillAcquisitionEnabled", &AgentBrain::setSkillAcquisitionEnabled, py::arg("enabled"),
             "Enable or disable skill acquisition mechanism")
        .def("isSkillAcquisitionEnabled", &AgentBrain::isSkillAcquisitionEnabled,
             "Check if skill acquisition is enabled")
        .def("setMemoryConsolidationEnabled", &AgentBrain::setMemoryConsolidationEnabled, py::arg("enabled"),
             "Enable or disable memory consolidation mechanism")
        .def("isMemoryConsolidationEnabled", &AgentBrain::isMemoryConsolidationEnabled,
             "Check if memory consolidation is enabled")
        .def("setSocialLearningEnabled", &AgentBrain::setSocialLearningEnabled, py::arg("enabled"),
             "Enable or disable social learning mechanism")
        .def("isSocialLearningEnabled", &AgentBrain::isSocialLearningEnabled,
             "Check if social learning is enabled")
        .def("setContinuityLearningEnabled", &AgentBrain::setContinuityLearningEnabled, py::arg("enabled"),
             "Enable or disable continuity learning mechanism")
        .def("isContinuityLearningEnabled", &AgentBrain::isContinuityLearningEnabled,
             "Check if continuity learning is enabled");

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

    // Advanced factory functions
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set advanced defaults
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.15);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("development.enable", true);
        return config;
    }, "Create an advanced configuration with optimal research settings");

    m.def("createResearchWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(50, 50, 16, 16);
        world->setAgentStart(25.0, 25.0);
        // Add research objects
        world->addResource(10.0, 0.5f);  // Resource
        world->addHazard(5.0, 0.3f);     // Hazard
        return world;
    }, "Create a research world with standard setup");

    m.def("createBenchmarkWorld", [](int width, int height) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 8, 8);
        world->setAgentStart(width/2.0f, height/2.0f);
        // Create challenging environment
        for (int i = 0; i < width/4; ++i) {
            for (int j = 0; j < height/4; ++j) {
                if ((i + j) % 3 == 0) {
                    world->addResource(5.0, 0.3f);
                }
                if ((i + j) % 5 == 0) {
                    world->addHazard(2.0, 0.2f);
                }
            }
        }
        return world;
    }, py::arg("width"), py::arg("height"), "Create a benchmark world with obstacles");

    // Utility functions for advanced users
    m.def("runSimulation",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, int numSteps) {
            for (int step = 0; step < numSteps; ++step) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step, step * 0.001f);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
            }
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
        "Run a complete simulation with brain, world, and agent");

    m.def("runLearningExperiment",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, int numSteps, bool enableLearning) {
            if (enableLearning) {
                agent->enableRewardModulation(true);
                agent->enableCuriosity(true);
                agent->enableStructuralPlasticity(true);
                agent->enableDevelopment(true);
            }
            
            for (int step = 0; step < numSteps; ++step) {
                world->update(0.1f);
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step, step * 0.001f);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, world->getSimulationTime());
                
                // Apply reward based on world state
                auto body = world->getAgentBody();
                float reward = body.energy / 100.0f;
                agent->applyRewardModulation(reward, 0.0f);
                
                // Update development
                agent->updateDevelopment(0.001f);
            }
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("numSteps"), py::arg("enableLearning") = true,
        "Run a learning experiment with all advanced features enabled");

    m.def("saveSimulationData",
        [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
           std::shared_ptr<AgentBrain> agent, const std::string& filepath) {
            std::ofstream file(filepath);
            if (!file.is_open()) {
                throw std::runtime_error("Cannot open file for writing: " + filepath);
            }
            
            file << "Simulation Data" << std::endl;
            file << "=" << std::string(50, '=') << std::endl;
            file << "Brain Statistics:" << std::endl;
            file << "  Neuron Count: " << brain->getTotalNeuronCount() << std::endl;
            file << "  Synapse Count: " << brain->getTotalSynapseCount() << std::endl;
            file << "  Firing Rate: " << brain->getAverageFiringRate() << std::endl;
            file << "  Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
            
            file << "\nAgent Statistics:" << std::endl;
            file << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
            file << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
            file << "  Prediction Error: " << agent->getPredictionError() << std::endl;
            file << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
            
            file << "\nWorld Statistics:" << std::endl;
            file << "  World Time: " << world->getSimulationTime() << std::endl;
            file << "  World Size: " << world->getWidth() << " x " << world->getHeight() << std::endl;
            file << "  Max Energy: " << world->getMaxEnergy() << std::endl;
            
            file << "\nSimulation Complete" << std::endl;
            file << std::endl;
        },
        py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("filepath"),
        "Save comprehensive simulation data to file");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
