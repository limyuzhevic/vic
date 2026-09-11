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
        .def("getWidth", &SimpleWorld::getWidth)
        .def("getHeight", &SimpleWorld::getHeight)
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy)
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"))
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate)
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"))
        .def("getSimulationTime", &SimpleWorld::getSimulationTime)
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"))
        .def("getRandomSeed", &SimpleWorld::getRandomSeed);

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
        
        // ===== ADVANCED SIMULATION CONTROL =====
        .def("setTimestep", &Brain::setTimestep,
             py::arg("timestep"),
             "Set fine-grained simulation timestep")
        .def("getTimestep", &Brain::getTimestep,
             "Get current simulation timestep")
        .def("pause", &Brain::pause,
             "Pause simulation execution")
        .def("resume", &Brain::resume,
             "Resume simulation execution")
        .def("isPaused", &Brain::isPaused,
             "Check if simulation is paused")
        .def("addSpikeHandler", &Brain::addSpikeHandler,
             py::arg("handler"),
             "Add custom spike event handler callback")
        .def("removeSpikeHandler", &Brain::removeSpikeHandler,
             py::arg("handler"),
             "Remove spike event handler")
        .def("batchExecute", &Brain::batchExecute,
             py::arg("operations"),
             "Execute batch operations for performance")
        .def("getSpikeHistory", &Brain::getSpikeHistory,
             py::return_value_policy::reference_internal,
             "Get spike history buffer")
        .def("clearSpikeHistory", &Brain::clearSpikeHistory,
             "Clear spike history buffer")
        
        // ===== MEMORY AND PERFORMANCE MONITORING =====
        .def("getMemoryUsage", &Brain::getMemoryUsage,
             "Get current memory usage (bytes)")
        .def("getPerformanceStats", &Brain::getPerformanceStats,
             "Get performance metrics")
        .def("getResourceUsage", &Brain::getResourceUsage,
             "Get system resource usage")
        .def("profileMemory", &Brain::profileMemory,
             py::arg("duration") = 1.0,
             "Profile memory usage over time")
        .def("startProfiling", &Brain::startProfiling,
             "Start performance profiling")
        .def("stopProfiling", &Brain::stopProfiling,
             "Stop performance profiling")
        .def("getProfileData", &Brain::getProfileData,
             "Get collected profiling data")
        .def("logPerformanceMetrics", &Brain::logPerformanceMetrics,
             "Log detailed performance metrics")
        
        // ===== STATE MANAGEMENT =====
        .def("checkpoint", &Brain::checkpoint,
             py::arg("name"),
             "Create a checkpoint with metadata")
        .def("loadCheckpoint", &Brain::loadCheckpoint,
             py::arg("name"),
             "Load checkpoint by name")
        .def("listCheckpoints", &Brain::listCheckpoints,
             "List all available checkpoints")
        .def("compareStates", &Brain::compareStates,
             py::arg("other"), py::arg("tolerance") = 1e-5,
             "Compare brain state with another brain")
        .def("getStateDiff", &Brain::getStateDiff,
             py::arg("other"),
             "Get state differences from another brain")
        .def("diffWithCheckpoint", &Brain::diffWithCheckpoint,
             py::arg("checkpointName"),
             "Compare current state with checkpoint")
        
        // ===== NEUROMODULATION CONTROL =====
        .def("setDopamineLevel", &Brain::setDopamineLevel,
             py::arg("level"),
             "Set dopamine neuromodulator level")
        .def("setCuriosityLevel", &Brain::setCuriosityLevel,
             py::arg("level"),
             "Set curiosity neuromodulator level")
        .def("setNoveltyLevel", &Brain::setNoveltyLevel,
             py::arg("level"),
             "Set novelty neuromodulator level")
        .def("setPredictionError", &Brain::setPredictionError,
             py::arg("error"),
             "Set prediction error signal")
        .def("applyCustomPlasticity", &Brain::applyCustomPlasticity,
             py::arg("ruleName"), py::arg("parameters"),
             "Apply custom plasticity rule")
        .def("getNeuromodulationLevel", &Brain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("setRewardShaping", &Brain::setRewardShaping,
             py::arg("rewardFunc"),
             "Set custom reward shaping function")
        .def("getNeuromodulators", &Brain::getNeuromodulators,
             "Get current neuromodulator levels")
        .def("resetNeuromodulation", &Brain::resetNeuromodulation,
             "Reset all neuromodulators to baseline")
        
        // ===== DEVELOPMENT AND LEARNING CONTROL =====
        .def("setLearningRate", &Brain::setLearningRate,
             py::arg("rate"),
             "Set global learning rate")
        .def("setPlasticityThreshold", &Brain::setPlasticityThreshold,
             py::arg("threshold"),
             "Set synaptic plasticity threshold")
        .def("setEligibilityTraceDecay", &Brain::setEligibilityTraceDecay,
             py::arg("decay"),
             "Set eligibility trace decay rate")
        .def("applyCustomLearningRule", &Brain::applyCustomLearningRule,
             py::arg("ruleName"), py::arg("params"),
             "Apply custom learning rule")
        .def("enablePatternReplay", &Brain::enablePatternReplay,
             py::arg("patternId"), py::arg("replaySpeed") = 1.0,
             "Enable pattern replay control")
        .def("stopPatternReplay", &Brain::stopPatternReplay,
             "Stop pattern replay")
        .def("getLearningStats", &Brain::getLearningStats,
             "Get learning statistics")
        
        // ===== DIAGNOSTICS AND DEBUGGING =====
        .def("visualizeNetwork", &Brain::visualizeNetwork,
             py::arg("outputFile"),
             "Generate network visualization")
        .def("analyzeConnectivity", &Brain::analyzeConnectivity,
             py::arg("threshold"),
             "Analyze network connectivity")
        .def("getConnectivityMatrix", &Brain::getConnectivityMatrix,
             "Get connectivity matrix")
        .def("profileNeuralActivity", &Brain::profileNeuralActivity,
             py::arg("duration"),
             "Profile neural activity patterns")
        .def("getActivityStats", &Brain::getActivityStats,
             "Get neural activity statistics")
        .def("findRepeats", &Brain::findRepeats,
             py::arg("patternSize"),
             "Find recurring activity patterns")
        .def("detectBursts", &Brain::detectBursts,
             py::arg("threshold"),
             "Detect neural firing bursts")
        .def("getRegionStats", &Brain::getRegionStats,
             py::arg("regionId"),
             "Get statistics for a specific region")
        .def("getAllRegionStats", &Brain::getAllRegionStats,
             "Get statistics for all regions")
        
        // ===== DEVELOPMENT AND LEARNING CONTROL =====
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("advanceDevelopment", &Brain::advanceDevelopment,
             py::arg("timeStep"),
             "Advance development system")
        .def("setLearningRate", &Brain::setLearningRate,
             py::arg("rate"),
             "Set global learning rate")
        .def("setPlasticityThreshold", &Brain::setPlasticityThreshold,
             py::arg("threshold"),
             "Set synaptic plasticity threshold")
        .def("setEligibilityTraceDecay", &Brain::setEligibilityTraceDecay,
             py::arg("decay"),
             "Set eligibility trace decay rate")
        .def("applyCustomLearningRule", &Brain::applyCustomLearningRule,
             py::arg("ruleName"), py::arg("params"),
             "Apply custom learning rule")
        .def("enablePatternReplay", &Brain::enablePatternReplay,
             py::arg("patternId"), py::arg("replaySpeed") = 1.0,
             "Enable pattern replay control")
        .def("stopPatternReplay", &Brain::stopPatternReplay,
             "Stop pattern replay")
        .def("getLearningStats", &Brain::getLearningStats,
             "Get learning statistics")
        
        // ===== DIAGNOSTICS AND DEBUGGING =====
        .def("visualizeNetwork", &Brain::visualizeNetwork,
             py::arg("outputFile"),
             "Generate network visualization")
        .def("analyzeConnectivity", &Brain::analyzeConnectivity,
             py::arg("threshold"),
             "Analyze network connectivity")
        .def("getConnectivityMatrix", &Brain::getConnectivityMatrix,
             "Get connectivity matrix")
        .def("profileNeuralActivity", &Brain::profileNeuralActivity,
             py::arg("duration"),
             "Profile neural activity patterns")
        .def("getActivityStats", &Brain::getActivityStats,
             "Get neural activity statistics")
        .def("findRepeats", &Brain::findRepeats,
             py::arg("patternSize"),
             "Find recurring activity patterns")
        .def("detectBursts", &Brain::detectBursts,
             py::arg("threshold"),
             "Detect neural firing bursts")
        .def("getRegionStats", &Brain::getRegionStats,
             py::arg("regionId"),
             "Get statistics for a specific region")
        .def("getAllRegionStats", &Brain::getAllRegionStats,
             "Get statistics for all regions")
        
        // ===== UTILITY AND STATUS =====
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
        
        // ===== MULTI-AGENT AND ENVIRONMENT CONTROL =====
        .def("batchProcessSensors", &AgentBrain::batchProcessSensors,
             py::arg("percepts"),
             "Process multiple sensory inputs in batch")
        .def("batchExecuteActions", &AgentBrain::batchExecuteActions,
             py::arg("actions"),
             "Execute multiple motor commands in batch")
        .def("coordinateWithAgent", &AgentBrain::coordinateWithAgent,
             py::arg("otherBrain"), py::arg("coordinationType"),
             "Coordinate behavior with another agent")
        .def("synchronizeMemories", &AgentBrain::synchronizeMemories,
             py::arg("otherBrain"), py::arg("memoryType"),
             "Synchronize memory with another agent")
        .def("createEnvironmentScript", &AgentBrain::createEnvironmentScript,
             py::arg("script"),
             "Create and execute environment script")
        .def("batchUpdateWorld", &AgentBrain::batchUpdateWorld,
             py::arg("worldUpdates"),
             "Batch update world state")
        .def("getAllAgentBodies", &AgentBrain::getAllAgentBodies,
             "Get all agent bodies in world")
        .def("getNearbyAgents", &AgentBrain::getNearbyAgents,
             py::arg("x"), py::arg("y"), py::arg("radius"),
             "Get agents within radius")
        .def("setEnvironmentVariable", &AgentBrain::setEnvironmentVariable,
             py::arg("key"), py::arg("value"),
             "Set environment variable")
        .def("getEnvironmentVariable", &AgentBrain::getEnvironmentVariable,
             py::arg("key"),
             "Get environment variable")
        .def("clearEnvironment", &AgentBrain::clearEnvironment,
             "Clear all environment objects")
        .def("spawnEnvironmentObject", &AgentBrain::spawnEnvironmentObject,
             py::arg("type"), py::arg("x"), py::arg("y"),
             "Spawn environment object")
        .def("getEnvironmentStats", &AgentBrain::getEnvironmentStats,
             "Get environment statistics")
        .def("exportEnvironment", &AgentBrain::exportEnvironment,
             py::arg("filename"),
             "Export current environment to file")
        .def("importEnvironment", &AgentBrain::importEnvironment,
             py::arg("filename"),
             "Import environment from file");

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
