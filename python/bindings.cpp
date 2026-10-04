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
        // Advanced Brain Configuration
        .def("setBrainType", &Config::setBrainType, py::arg("type"),
             "Set brain type (e.g., 'Phase6_Integrated')")
        .def("setNeuronCount", &Config::setNeuronCount, py::arg("count"),
             "Set number of neurons")
        .def("setRegionCount", &Config::setRegionCount, py::arg("count"),
             "Set number of brain regions")
        .def("setConnectionProbability", &Config::setConnectionProbability,
             py::arg("prob"), "Set connection probability (0.0-1.0)")
        // Advanced Memory Configuration
        .def("setWorkingMemoryCapacity", &Config::setWorkingMemoryCapacity,
             py::arg("capacity"), "Set working memory capacity")
        .def("setWorkingMemoryDecayRate", &Config::setWorkingMemoryDecayRate,
             py::arg("rate"), "Set working memory decay rate")
        .def("setEpisodicMemoryMaxEpisodes", &Config::setEpisodicMemoryMaxEpisodes,
             py::arg("max"), "Set maximum number of episodic memory episodes")
        .def("setEpisodicMemoryConsolidationThreshold",
             &Config::setEpisodicMemoryConsolidationThreshold,
             py::arg("threshold"), "Set episodic memory consolidation threshold")
        // Advanced Prediction System Configuration
        .def("setPredictionDepth", &Config::setPredictionDepth,
             py::arg("depth"), "Set prediction system depth")
        .def("setPredictionConfidenceThreshold",
             &Config::setPredictionConfidenceThreshold,
             py::arg("threshold"), "Set prediction confidence threshold")
        .def("setPredictionErrorSmoothing", &Config::setPredictionErrorSmoothing,
             py::arg("smoothing"), "Set prediction error smoothing factor")
        // Advanced Cognition Configuration
        .def("setPlannerPlanningDepth", &Config::setPlannerPlanningDepth,
             py::arg("depth"), "Set planner planning depth")
        .def("setConceptFormationStabilityThreshold",
             &Config::setConceptFormationStabilityThreshold,
             py::arg("threshold"), "Set concept formation stability threshold")
        .def("setAttentionInhibitionStrength",
             &Config::setAttentionInhibitionStrength,
             py::arg("strength"), "Set attention inhibition strength")
        .def("setAttentionExcitationStrength",
             &Config::setAttentionExcitationStrength,
             py::arg("strength"), "Set attention excitation strength")
        // Advanced Neuromodulation Configuration
        .def("setDopaminePlasticityFactor", &Config::setDopaminePlasticityFactor,
             py::arg("factor"), "Set dopamine plasticity factor")
        .def("setNoveltyDetectionThreshold",
             &Config::setNoveltyDetectionThreshold,
             py::arg("threshold"), "Set novelty detection threshold")
        .def("setCuriosityExplorationWeight",
             &Config::setCuriosityExplorationWeight,
             py::arg("weight"), "Set curiosity exploration weight")
        .def("setSerotoninMoodInfluence", &Config::setSerotoninMoodInfluence,
             py::arg("influence"), "Set serotonin mood influence")
        .def("setNorepinephrineArousalThreshold",
             &Config::setNorepinephrineArousalThreshold,
             py::arg("threshold"), "Set norepinephrine arousal threshold")
        .def("setAcetylcholineAttentionGain", &Config::setAcetylcholineAttentionGain,
             py::arg("gain"), "Set acetylcholine attention gain")
        // Advanced Development Configuration
        .def("setDevelopmentStageTransitions",
             &Config::setDevelopmentStageTransitions,
             py::arg("transitionRate"), "Set development stage transition rate")
        .def("setDevelopmentalPlasticityMultiplier",
             &Config::setDevelopmentalPlasticityMultiplier,
             py::arg("multiplier"), "Set developmental plasticity multiplier")
        .def("setMaturationRate", &Config::setMaturationRate,
             py::arg("rate"), "Set maturation rate")
        // Advanced Checkpoint Configuration
        .def("setCheckpointPath", &Config::setCheckpointPath,
             py::arg("path"), "Set checkpoint file path")
        .def("setCheckpointCompressionLevel",
             &Config::setCheckpointCompressionLevel,
             py::arg("level"), "Set checkpoint compression level")
        .def("setCheckpointMaxFiles", &Config::setCheckpointMaxFiles,
             py::arg("max"), "Set maximum number of checkpoint files")
        .def("setCheckpointAutoSave", &Config::setCheckpointAutoSave,
             py::arg("autoSave"), "Enable or disable automatic checkpoint saving")
        // Advanced Performance Configuration
        .def("setMemoryPoolSize", &Config::setMemoryPoolSize,
             py::arg("size"), "Set memory pool size")
        .def("setEventQueueCapacity", &Config::setEventQueueCapacity,
             py::arg("capacity"), "Set event queue capacity")
        .def("setParallelProcessingEnabled", &Config::setParallelProcessingEnabled,
             py::arg("enabled"), "Enable or disable parallel processing")
        .def("setSIMDVectorizationEnabled", &Config::setSIMDVectorizationEnabled,
             py::arg("enabled"), "Enable or disable SIMD vectorization")
        // Advanced Replay Configuration
        .def("setReplayMemoryCount", &Config::setReplayMemoryCount,
             py::arg("count"), "Set replay memory count")
        .def("setReplayConfidenceThreshold",
             &Config::setReplayConfidenceThreshold,
             py::arg("threshold"), "Set replay confidence threshold")
        .def("setReplayTimeStepInterval", &Config::setReplayTimeStepInterval,
             py::arg("interval"), "Set replay time step interval")
        // Advanced Rest Configuration
        .def("setRestPeriodDuration", &Config::setRestPeriodDuration,
             py::arg("duration"), "Set rest period duration")
        .def("setRestNeuromodulatorThresholds",
             &Config::setRestNeuromodulatorThresholds,
             py::arg("sero"), py::arg("dopa"), py::arg("norepi"), py::arg("ach"),
             "Set neuromodulator thresholds for rest periods")
        .def("setRestDevelopmentAcceleration",
             &Config::setRestDevelopmentAcceleration,
             py::arg("accelerated"), "Enable or disable development acceleration during rest")
        // Advanced Event System Configuration
        .def("setEventQueueMaxSize", &Config::setEventQueueMaxSize,
             py::arg("size"), "Set maximum event queue size")
        .def("setEventProcessingInterval", &Config::setEventProcessingInterval,
             py::arg("interval"), "Set event processing interval")
        .def("setEventPriorityLevel", &Config::setEventPriorityLevel,
             py::arg("level"), "Set event priority level")
        // Advanced Memory Consolidation Configuration
        .def("setMemoryConsolidationThreshold",
             &Config::setMemoryConsolidationThreshold,
             py::arg("threshold"), "Set memory consolidation threshold")
        .def("setMemoryConsolidationDecay", &Config::setMemoryConsolidationDecay,
             py::arg("decay"), "Set memory consolidation decay rate")
        .def("setMemoryReplayStrength", &Config::setMemoryReplayStrength,
             py::arg("strength"), "Set memory replay strength")
        // Advanced Configuration Methods
        .def("hasAdvancedFeatures", &Config::hasAdvancedFeatures,
             "Check if advanced features are enabled")
        .def("getAdvancedConfigSummary", &Config::getAdvancedConfigSummary,
             "Get a summary of all advanced configuration parameters")
        .def("getAllAdvancedKeys", &Config::getAllAdvancedKeys,
             "Get list of all advanced configuration keys")
        .def("loadAdvancedConfigFromFile", &Config::loadAdvancedConfigFromFile,
             py::arg("filepath"), "Load advanced configuration from file")
        .def("saveAdvancedConfigToFile", &Config::saveAdvancedConfigToFile,
             py::arg("filepath"), "Save advanced configuration to file")
        .def("setAdvanced", &Config::setAdvanced<std::string>, py::arg("key"), py::arg("value"),
             "Set advanced configuration value (template method)")
        .def("getOr", &Config::getOr<int>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or default (integer template)")
        .def("getOr", &Config::getOr<double>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or default (float template)")
        .def("getOr", &Config::getOr<bool>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or default (boolean template)")
        .def("getOr", &Config::getOr<std::string>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or default (string template)")
        .def("getOr", &Config::getOr<size_t>, py::arg("key"), py::arg("defaultValue"),
             "Get configuration value or default (size_t template)")
        .def("validateAdvancedSetting", &Config::validateAdvancedSetting,
             py::arg("key"), py::arg("value"),
             "Validate advanced configuration setting")
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

    // ========== ADVANCED BRAIN SYSTEM BINDINGS ==========

    // Enhance Brain class with advanced system access
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class with advanced features)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        // ... existing bindings ...
        // Add advanced bindings:
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attention system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine system")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty system")
        .def("getPredictionError", &Brain::getPredictionError,
             py::return_value_policy::reference_internal,
             "Get prediction error system")
        .def("getSerotonin", &Brain::getSerotonin,
             py::return_value_policy::reference_internal,
             "Get serotonin system")
        .def("getNorepinephrine", &Brain::getNorepinephrine,
             py::return_value_policy::reference_internal,
             "Get norepinephrine system")
        .def("getAcetylcholine", &Brain::getAcetylcholine,
             py::return_value_policy::reference_internal,
             "Get acetylcholine system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"), "Set developmental stage")
        // Add rest/sleep control:
        .def("enterRestPeriod", &Brain::enterRestPeriod,
             "Enter rest period for memory consolidation")
        .def("leaveRestPeriod", &Brain::leaveRestPeriod,
             "Leave rest period")
        .def("isResting", &Brain::isResting,
             "Check if brain is in rest period")
        // Add checkpoint control:
        .def("triggerCheckpoint", &Brain::triggerCheckpoint,
             py::arg("description"), "Trigger manual checkpoint")
        // Add replay control:
        .def("triggerReplay", &Brain::triggerReplay,
             py::arg("count", 5), "Trigger replay of recent memories")
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("logStatus", &Brain::logStatus,
             "Log brain status");

    // ========== ADVANCED NEURAL WORKING MEMORY BINDINGS ==========

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory",
         R"pbdoc(Neural working memory with persistent activity and competition)pbdoc")
        .def("initialize", &NeuralWorkingMemory::initialize,
             py::arg("brain"), "Initialize working memory")
        .def("store", &NeuralWorkingMemory::store,
             py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store pattern in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"),
             "Store activation to specific neuron")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             "Get working memory capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity,
             py::arg("cap"), "Set working memory capacity")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"), "Get neuron activation")
        .def("update", &NeuralWorkingMemory::update,
             py::arg("dt"), "Update working memory")
        .def("clear", &NeuralWorkingMemory::clear,
             "Clear working memory")
        .def("reinforce", &NeuralWorkingMemory::reinforceDuringRest,
             py::arg("factor"), "Reinforce memory traces")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    // ========== ADVANCED EPISODIC MEMORY BINDINGS ==========

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory",
         R"pbdoc(Neural episodic memory storing experiences as neural patterns)pbdoc")
        .def("initialize", &NeuralEpisodicMemory::initialize,
             py::arg("brain"), "Initialize episodic memory")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode,
             py::arg("episode"), "Store episode")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get number of stored episodes")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes,
             py::arg("count"), "Get recent episodes")
        .def("retrieveEpisode", &NeuralEpisodicMemory::retrieveEpisode,
             py::arg("index"), "Retrieve episode by index")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay,
             py::arg("count"), "Get episodes for replay")
        .def("consolidateMemory", &NeuralEpisodicMemory::consolidateRestEnhanced,
             "Consolidate memories")
        .def("consolidate", &NeuralEpisodicMemory::consolidate,
             py::arg("threshold"), "Consolidate with threshold")
        .def("clear", &NeuralEpisodicMemory::clear,
             "Clear episodic memory")
        .def("setResting", &NeuralEpisodicMemory::setResting,
             py::arg("resting"), "Set resting state")
        .def("isResting", &NeuralEpisodicMemory::isResting,
             "Check if episodic memory is resting")
        .def("incrementRestPeriod", &NeuralEpisodicMemory::incrementRestPeriod,
             "Increment rest period count")
        .def("getRestPeriodCount", &NeuralEpisodicMemory::getRestPeriodCount,
             "Get rest period count");

    // ========== PREDICTION SYSTEM BINDINGS ==========

    py::class_<PredictionSystem>(m, "PredictionSystem",
         R"pbdoc(Prediction system for sensory prediction and error computation)pbdoc")
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentState"), "Predict next sensory state")
        .def("updatePredictions", &PredictionSystem::updatePredictions,
             py::arg("predicted"), py::arg("actual"), "Update predictions with actual observation")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("getErrorHistory", &PredictionSystem::getErrorHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionSystem::clearHistory,
             "Clear prediction history")
        .def("train", &PredictionSystem::train,
             py::arg("observation"), "Train prediction model");

    // ========== NEURAL PLANNER BINDINGS ==========

    py::class_<NeuralPlanner>(m, "NeuralPlanner",
         R"pbdoc(Neural planner for action planning and decision making)pbdoc")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentStep"), py::arg("currentState"),
             py::arg("totalSpikeCount"), py::arg("predictionData"),
             py::arg("neuromodulationData"), "Plan next action")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"), "Evaluate action sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get planning depth")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth,
             py::arg("depth"), "Set planning depth")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             "Get planning confidence")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update plan quality based on outcomes")
        .def("updatePlanningParameters", &NeuralPlanner::updatePlanningParameters,
             py::arg("predictionError"), py::arg("rewardPrediction"),
             "Update planner parameters")
        .def("updateFromExperience", &NeuralPlanner::updateFromExperience,
             py::arg("episodes"), "Update from episodic memory")
        .def("clearCache", &NeuralPlanner::clearCache,
             "Clear planning cache")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal,
             py::arg("goal"), "Set current planning goal")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             "Get current planning goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent plan was successful")
        .def("setActionQuality", &NeuralPlanner::setActionQuality,
             py::arg("action"), py::arg("quality"), "Set action quality");

    // ========== CONCEPT FORMATION BINDINGS ==========

    py::class_<ConceptFormation>(m, "ConceptFormation",
         R"pbdoc(Concept formation for pattern discovery and categorization)pbdoc")
        .def("initialize", &ConceptFormation::initialize,
             py::arg("brain"), "Initialize concept formation")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Present new experience to concept formation")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Get concept that best matches pattern")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts discovered")
        .def("getConcepts", &ConceptFormation::getConcepts,
             py::return_value_policy::reference_internal,
             "Get all concepts")
        .def("getConcept", &ConceptFormation::getConcept,
             py::arg("conceptId"), "Get concept by ID")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype,
             py::arg("conceptId"), "Get concept prototype")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances,
             py::arg("conceptId"), "Get concept instances")
        .def("getConceptStability", &ConceptFormation::getConceptStability,
             py::arg("conceptId"), "Get concept stability")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts,
             py::arg("conceptA"), py::arg("conceptB"), "Merge two concepts")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             "Update concept with new instance")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity,
             py::arg("a"), py::arg("b"), "Compute pattern similarity")
        .def("isNovel", &ConceptFormation::isNovel,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             "Check if pattern is novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern,
             py::arg("pattern"), "Find concept for pattern")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility,
             py::arg("conceptId"), "Get generalization ability")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold,
             py::arg("threshold"), "Set formation threshold")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow,
             py::arg("window"), "Set stability window");

    // ========== ATTENTION SYSTEM BINDINGS ==========

    py::class_<AttentionalSelection>(m, "AttentionalSelection",
         R"pbdoc(Neural attention through competition and selection)pbdoc")
        .def("initialize", &AttentionalSelection::initialize,
             py::arg("brain"), "Initialize attention system")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competition between inputs")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion,
             py::arg("region"), "Focus attention on region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention to all regions")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             py::return_value_policy::reference_internal,
             "Get attended regions")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
             py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
             py::arg("strength"), "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold,
             py::arg("threshold"), "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor,
             py::arg("neuron"), "Get inhibition for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor,
             py::arg("neuron"), "Get excitation for neuron")
        .def("update", &AttentionalSelection::update,
             py::arg("dt"), "Update attention dynamics")
        .def("isAttended", &AttentionalSelection::isAttended,
             py::arg("neuron"), "Check if neuron is attended")
        .def("getWinners", &AttentionalSelection::getWinners,
             py::return_value_policy::reference_internal,
             "Get attention winners")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias,
             py::arg("neuron"), py::arg("biasStrength"), "Apply top-down bias")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience,
             py::arg("neuron"), py::arg("salienceStrength"), "Apply bottom-up salience")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention system");

    // ========== DEVELOPMENT SYSTEM BINDINGS ==========

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem",
         R"pbdoc(Development system for neural maturation and plasticity)pbdoc")
        .def("getStage", &DevelopmentSystem::getStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage,
             py::arg("stage"), "Set developmental stage")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             "Advance developmental stage")
        .def("getStageName", &DevelopmentSystem::getStageName,
             "Get stage name")
        .def("update", &DevelopmentSystem::update,
             py::arg("brain"), py::arg("currentStep"), "Update development (simple)")
        .def("updateFull", &DevelopmentSystem::update,
             py::arg("brain"), py::arg("rng"), py::arg("dt"), "Update development with parameters")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             "Get plasticity modifier for current stage")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             "Check if in critical period")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             "Get critical period progress")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             "Get developmental age")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge,
             py::arg("age"), "Set developmental age")
        // Rest-based development:
        .def("accelerateDevelopment", &DevelopmentSystem::accelerateDevelopment,
             "Accelerate development during rest")
        .def("isDevelopmentAccelerated", &DevelopmentSystem::isDevelopmentAccelerated,
             "Check if development is accelerated")
        .def("setDevelopmentAccelerated", &DevelopmentSystem::setDevelopmentAccelerated,
             py::arg("accelerated"), "Set development acceleration");

    // ========== NEUROMODULATION BINDINGS ==========

    // Base neuromodulator class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base for neuromodulatory signals)pbdoc")
        .def("getName", &Neuromodulator::getName,
             "Get neuromodulator name")
        .def("getLevel", &Neuromodulator::getLevel,
             "Get current level")
        .def("setLevel", &Neuromodulator::setLevel,
             py::arg("level"), "Set neuromodulator level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             "Get plasticity factor")
        .def("update", &Neuromodulator::update,
             py::arg("dt"), "Update neuromodulator state");

    // Dopamine system
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Reward and reinforcement learning signal)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"))
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"))
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"))
        .def("recordReward", &Dopamine::recordReward, py::arg("reward"))
        .def("getAverageReward", &Dopamine::getAverageReward)
        .def("getRewardCount", &Dopamine::getRewardCount);

    // Serotonin system
    py::class_<Serotonin>(m, "Serotonin", R"pbdoc(Mood, impulsivity, and social behavior modulation)pbdoc")
        .def(py::init<>())
        .def("getName", &Serotonin::getName)
        .def("getLevel", &Serotonin::getLevel)
        .def("setLevel", &Serotonin::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Serotonin::getPlasticityFactor)
        .def("update", &Serotonin::update, py::arg("dt"))
        .def("signalSocialFeedback", &Serotonin::signalSocialFeedback, py::arg("feedback"))
        .def("modulateImpulsivity", &Serotonin::modulateImpulsivity, py::arg("impulsivity"))
        .def("regulateMood", &Serotonin::regulateMood, py::arg("mood"))
        .def("getSocialModulation", &Serotonin::getSocialModulation)
        .def("getBehavioralControl", &Serotonin::getBehavioralControl);

    // Norepinephrine system
    py::class_<Norepinephrine>(m, "Norepinephrine", R"pbdoc(Arousal, vigilance, and stress response modulation)pbdoc")
        .def(py::init<>())
        .def("getName", &Norepinephrine::getName)
        .def("getLevel", &Norepinephrine::getLevel)
        .def("setLevel", &Norepinephrine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Norepinephrine::getPlasticityFactor)
        .def("update", &Norepinephrine::update, py::arg("dt"))
        .def("signalStress", &Norepinephrine::signalStress, py::arg("stress"))
        .def("enhanceVigilance", &Norepinephrine::enhanceVigilance, py::arg("isThreat"))
        .def("getArousalLevel", &Norepinephrine::getArousalLevel)
        .def("getVigilanceModulation", &Norepinephrine::getVigilanceModulation);

    // Acetylcholine system
    py::class_<Acetylcholine>(m, "Acetylcholine", R"pbdoc(Attention and memory consolidation modulation)pbdoc")
        .def(py::init<>())
        .def("getName", &Acetylcholine::getName)
        .def("getLevel", &Acetylcholine::getLevel)
        .def("setLevel", &Acetylcholine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Acetylcholine::getPlasticityFactor)
        .def("update", &Acetylcholine::update, py::arg("dt"))
        .def("signalNovelty", &Acetylcholine::signalNovelty, py::arg("novelty"))
        .def("enhanceAttentionDuringNovelty", &Acetylcholine::enhanceAttentionDuringNovelty, py::arg("isNovel"))
        .def("getAttentionModulation", &Acetylcholine::getAttentionModulation);

    // Novelty system
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection and processing)pbdoc")
        .def(py::init<>())
        .def("getName", &Novelty::getName)
        .def("getLevel", &Novelty::getLevel)
        .def("setLevel", &Novelty::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Novelty::getPlasticityFactor)
        .def("update", &Novelty::update, py::arg("dt"));

    // Curiosity system
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity and exploration motivation)pbdoc")
        .def(py::init<>())
        .def("getName", &Curiosity::getName)
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Curiosity::getPlasticityFactor)
        .def("update", &Curiosity::update, py::arg("dt"));

    // Prediction error system
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("getName", &PredictionError::getName)
        .def("getLevel", &PredictionError::getLevel)
        .def("setLevel", &PredictionError::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &PredictionError::getPlasticityFactor)
        .def("update", &PredictionError::update, py::arg("dt"));

    // Reward system
    py::class_<Reward>(m, "Reward", R"pbdoc(Reward signaling)pbdoc")
        .def(py::init<>())
        .def("getName", &Reward::getName)
        .def("getLevel", &Reward::getLevel)
        .def("setLevel", &Reward::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Reward::getPlasticityFactor)
        .def("update", &Reward::update, py::arg("dt"));

    // ========== CHECKPOINT AND PERFORMANCE BINDINGS ==========

    // Checkpoint system
    py::class_<CheckpointSystem>(m, "CheckpointSystem",
         R"pbdoc(Checkpointer for brain state serialization and checkpointing)pbdoc")
        .def("save", &CheckpointSystem::save,
             py::arg("filepath"), "Save checkpoint to file")
        .def("load", &CheckpointSystem::load,
             py::arg("filepath"), "Load checkpoint from file")
        .def("create", &CheckpointSystem::create,
             py::arg("name"), "Create new checkpoint")
        .def("delete", &CheckpointSystem::delete,
             py::arg("name"), "Delete checkpoint")
        .def("list", &CheckpointSystem::list,
             "List all checkpoints")
        .def("getState", &CheckpointSystem::getState,
             "Get current checkpoint state")
        .def("validate", &CheckpointSystem::validate,
             "Validate checkpoint integrity")
        .def("getStatus", &CheckpointSystem::getStatus,
             "Get checkpoint system status");

    // Event queue system
    py::class_<EventQueue>(m, "EventQueue",
         R"pbdoc(Event queue for time-scheduled events)pbdoc")
        .def("push", &EventQueue::push,
             py::arg("event"), "Push event to queue")
        .def("pop", &EventQueue::pop,
             "Pop event from queue")
        .def("getEvents", &EventQueue::getEvents,
             "Get all events in queue")
        .def("clear", &EventQueue::clear,
             "Clear event queue")
        .def("getSize", &EventQueue::getSize,
             "Get queue size")
        .def("isEmpty", &EventQueue::isEmpty,
             "Check if queue is empty");

    // Performance monitoring
    py::class_<Performance>(m, "Performance",
         R"pbdoc(Performance monitoring and metrics collection)pbdoc")
        .def("getMetrics", &Performance::getMetrics,
             "Get performance metrics")
        .def("getSimTime", &Performance::getSimTime,
             "Get simulation time")
        .def("getStepCount", &Performance::getStepCount,
             "Get step count")
        .def("getNeuronCount", &Performance::getNeuronCount,
             "Get neuron count")
        .def("getSynapseCount", &Performance::getSynapseCount,
             "Get synapse count")
        .def("getMemoryUsage", &Performance::getMemoryUsage,
             "Get memory usage")
        .def("getCPUUsage", &Performance::getCPUUsage,
             "Get CPU usage")
        .def("resetMetrics", &Performance::resetMetrics,
             "Reset performance metrics");

    // ========== ADVANCED BRAIN CONTROL BINDINGS ==========

    // Rest/sleep control
    m.def("enterRestPeriod", &Brain::enterRestPeriod,
         py::arg("description"), "Enter rest period")
        .def("leaveRestPeriod", &Brain::leaveRestPeriod,
             "Leave rest period")
        .def("isResting", &Brain::isResting,
             "Check if brain is resting")
        .def("getRestPeriodCount", &Brain::getRestPeriodCount,
             "Get rest period count");

    // Checkpoint control
    m.def("triggerCheckpoint", &Brain::triggerCheckpoint,
         py::arg("description"), "Trigger manual checkpoint")
        .def("getCheckpointState", &Brain::getCheckpointState,
             "Get current checkpoint state")
        .def("listCheckpoints", &Brain::listCheckpoints,
             "List available checkpoints");

    // Replay control
    m.def("triggerReplay", &Brain::triggerReplay,
         py::arg("count", 5), "Trigger replay of recent memories")
        .def("getReplayState", &Brain::getReplayState,
             "Get replay system state")
        .def("stopReplay", &Brain::stopReplay,
             "Stop replay system");

    // Performance monitoring
    m.def("getPerformanceMetrics", &Brain::getPerformanceMetrics,
         "Get performance metrics")
        .def("getBenchmark", &Brain::getBenchmark,
             "Get performance benchmark")
        .def("getStatistics", &Brain::getStatistics,
             "Get brain statistics");

    // Event-driven processing
    m.def("getEventQueue", &Brain::getEventQueue,
         py::return_value_policy::reference_internal,
         "Get event queue")
        .def("processEvents", &Brain::processEvents,
             "Process pending events")
        .def("scheduleEvent", &Brain::scheduleEvent,
             py::arg("delay"), py::arg("event"), "Schedule event")
        .def("cancelEvent", &Brain::cancelEvent,
             py::arg("eventId"), "Cancel scheduled event");

    // Memory management
    m.def("consolidateMemory", &Brain::consolidateMemory,
         py::arg("threshold"), "Consolidate memories")
        .def("reinforceWorkingMemory", &Brain::reinforceWorkingMemory,
             py::arg("factor"), "Reinforce working memory")
        .def("updateEpisodicMemory", &Brain::updateEpisodicMemory,
             "Update episodic memory consolidation");

    // Configuration access
    m.def("getBrainConfig", &Brain::getBrainConfig,
         py::return_value_policy::reference_internal,
         "Get brain configuration")
        .def("setBrainConfig", &Brain::setBrainConfig,
             py::arg("config"), "Set brain configuration")
        .def("getSystemStatus", &Brain::getSystemStatus,
             "Get system status");

    // System control
    m.def("resetSystems", &Brain::resetSystems,
         "Reset all systems")
        .def("pauseSimulation", &Brain::pauseSimulation,
             "Pause simulation")
        .def("resumeSimulation", &Brain::resumeSimulation,
             "Resume simulation")
        .def("shutdown", &Brain::shutdown,
             "Shutdown brain system");

    // Helper functions
    m.def("createAdvancedConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set advanced defaults
        config->set("brain.restPeriodEnabled", true);
        config->set("brain.checkpointInterval", 10000);
        config->set("brain.replayEnabled", true);
        config->set("brain.developmentEnabled", true);
        config->set("brain.performanceMonitoring", true);
        config->set("brain.eventDrivenEnabled", true);
        config->set("brain.advancedMemory", true);
        config->set("brain.neuromodulationEnabled", true);
        config->set("brain.cognitionEnabled", true);
        return config;
    }, "Create advanced configuration");

    m.def("createOptimizedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create optimized brain with advanced features");

    m.attr("ADVANCED_FEATURES_ENABLED") = true;
    m.attr("PHASE6_INTEGRATION") = true;

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm

