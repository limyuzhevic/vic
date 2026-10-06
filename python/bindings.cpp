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
#include "../src/experiments/Phase6IntegratedExperiment.hpp"

namespace py = pybind11;
namespace nlm {

// Helper function to get memory system data
py::dict getMemorySystemInfo(py::object brain) {
    // This would need to be implemented with actual brain object
    // For now, we'll create a comprehensive bindings file that includes all Phase 6 features
    return py::dict("working_memory"_a = true, "episodic_memory"_a = true);
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Enhanced Phase 6 Integration Edition with complete brain system access.
        
        NEW in Phase 6:
        - Advanced memory system management (working, episodic, associative)
        - Neuromodulation control (dopamine, curiosity, novelty, reward)
        - Prediction system with error tracking and confidence
        - Cognitive functions (neural planning, concept formation, attention)
        - Development system with stage management
        - Checkpoint and replay systems
        - Phase 6 Integration Experiment runner
        - Advanced agent capabilities
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic Types (existing functionality)
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

    // Enumerations (expanded for Phase 6)
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

    // Enhanced Config with Phase 6 settings
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM Phase 6 system)pbdoc")
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
        })
        // Phase 6 specific configuration methods
        .def("setNeuronalParameters", [](Config& self, double tauMem, double vRest, double vThresh) {
            self.set("neuron_tau_membrane", tauMem, ConfigSource::User);
            self.set("neuron_v_rest", vRest, ConfigSource::User);
            self.set("neuron_v_threshold", vThresh, ConfigSource::User);
        }, py::arg("tauMembrane"), py::arg("vRest"), py::arg("vThresh"),
           "Set neuronal membrane parameters for advanced tuning")
        .def("setPlasticityParameters", [](Config& self, double stdpLTP, double stdpLTD, double weightMax) {
            self.set("stdp_ltp_weight", stdpLTP, ConfigSource::User);
            self.set("stdp_ltd_weight", stdpLTD, ConfigSource::User);
            self.set("synaptic_weight_max", weightMax, ConfigSource::User);
        }, py::arg("stdpLTP"), py::arg("stdpLTD"), py::arg("weightMax"),
           "Set synaptic plasticity parameters")
        .def("enableAdvancedFeatures", [](Config& self) {
            self.set("enable_advanced_memory", true, ConfigSource::User);
            self.set("enable_neuromodulation", true, ConfigSource::User);
            self.set("enable_prediction_system", true, ConfigSource::User);
            self.set("enable_cognitive_functions", true, ConfigSource::User);
            self.set("enable_development_system", true, ConfigSource::User);
        }, "Enable all Phase 6 advanced features");

    // Enhanced SensoryInput with additional methods
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp")
        .def("getConfidence", &SensoryInput::getConfidence,
             "Get sensory input confidence (Phase 6)")
        .def("setConfidence", &SensoryInput::setConfidence, py::arg("confidence"),
             "Set sensory input confidence");

    // Enhanced Vision and Audio classes
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Advanced vision sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels)
        .def("getFeatures", &Vision::getFeatures,
             "Extract visual features (Phase 6 advanced)")
        .def("detectObjects", &Vision::detectObjects,
             "Object detection for agent planning");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Advanced audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"))
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples)
        .def("getFrequency", &Audio::getFrequency,
             "Get audio frequency features (Phase 6)")
        .def("detectPatterns", &Audio::detectPatterns,
             "Audio pattern detection for learning");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals)
        .def("getEmotionalState", &InternalSignals::getEmotionalState,
             "Get emotional state (Phase 6 neuromodulation)")
        .def("setEmotionalState", &InternalSignals::setEmotionalState,
             py::arg("state"));

    // Enhanced Action class
    py::class_<Action>(m, "Action", R"pbdoc(Advanced action representation with planning)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("getSequence", &Action::getSequence,
             "Get action sequence for planning (Phase 6)")
        .def("estimateOutcome", &Action::estimateOutcome,
             "Estimate action outcome using prediction system");

    // Enhanced WorldObject with AI features
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object with AI awareness)pbdoc")
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
        .def("getAIIteraction", &WorldObject::getAIIteraction,
             "Get AI interaction value (Phase 6 enhanced)")
        .def("updateWithAIFeatures", &WorldObject::updateWithAIFeatures,
             "Update object with AI-derived features");

    // Enhanced AgentBody with development tracking
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Evolved agent body with developmental state)pbdoc")
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
        .def("getDevelopmentalMetrics", &AgentBody::getDevelopmentalMetrics,
             "Get body development metrics (Phase 6)")
        .def("updateWithDevelopment", &AgentBody::updateWithDevelopment,
             py::arg("timestep"), "Update body state based on development");

    // Enhanced ActionResult with prediction
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result with prediction and confidence)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("getPredictionConfidence", &ActionResult::getPredictionConfidence,
             "Get action prediction confidence (Phase 6)")
        .def("setPredictionConfidence", &ActionResult::setPredictionConfidence,
             py::arg("confidence"), "Set action prediction confidence");

    // Enhanced SensoryPercept with prediction
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept with prediction and attention)pbdoc")
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
        .def("getAttentionScore", &SensoryPercept::getAttentionScore,
             "Get attention score (Phase 6)")
        .def("getPredictionError", &SensoryPercept::getPredictionError,
             "Get prediction error for learning (Phase 6)")
        .def("getNoveltyScore", &SensoryPercept::getNoveltyScore,
             "Get novelty detection score (Phase 6)");

    // Enhanced SimpleWorld with AI integration
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Evolved world with AI integration)pbdoc")
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
        // Phase 6 enhanced world features
        .def("getPerceivedObjects", &SimpleWorld::getPerceivedObjects,
             "Get perceived objects with AI analysis")
        .def("getEnvironmentalComplexity", &SimpleWorld::getEnvironmentalComplexity,
             "Get environmental complexity score (Phase 6)")
        .def("enableDynamicEnvironment", &SimpleWorld::enableDynamicEnvironment,
             "Enable dynamic environment evolution (Phase 6)");

    // Enhanced Brain class with Phase 6 systems
    py::class_<Brain>(m, "Brain", R"pbdoc(Advanced neural brain with integrated Phase 6 systems)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with advanced configuration")
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
        // Phase 6 enhanced brain methods
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system (Phase 6)")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system (Phase 6)")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system (Phase 6)")
        .def("getSemanticMemory", &Brain::getSemanticMemory,
             py::return_value_policy::reference_internal,
             "Get semantic memory system (Phase 6)")
        .def("getProceduralMemory", &Brain::getProceduralMemory,
             py::return_value_policy::reference_internal,
             "Get procedural memory system (Phase 6)")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system (Phase 6)")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner (Phase 6)")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system (Phase 6)")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attention system (Phase 6)")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system (Phase 6)")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system (Phase 6)")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulation system (Phase 6)")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detection system (Phase 6)")
        .def("getReward", &Brain::getReward,
             py::return_value_policy::reference_internal,
             "Get reward system (Phase 6)")
        .def("getPredictionError", &Brain::getPredictionError,
             "Get prediction error for learning (Phase 6)")
        .def("getAverageSynapticWeight", &Brain::getAverageSynapticWeight,
             "Get average synaptic weight (Phase 6)")
        .def("getNetworkEfficiency", &Brain::getNetworkEfficiency,
             "Get network efficiency metric (Phase 6)")
        .def("getCognitiveLoad", &Brain::getCognitiveLoad,
             "Get cognitive load assessment (Phase 6)")
        .def("getIntegrationStatus", &Brain::getIntegrationStatus,
             "Get complete integration status (Phase 6)")
        .def("enableAdvancedMemory", &Brain::enableAdvancedMemory,
             "Enable advanced memory systems (Phase 6)")
        .def("enableNeuromodulation", &Brain::enableNeuromodulation,
             "Enable neuromodulation systems (Phase 6)")
        .def("enablePredictionSystem", &Brain::enablePredictionSystem,
             "Enable prediction system (Phase 6)")
        .def("enableCognitiveFunctions", &Brain::enableCognitiveFunctions,
             "Enable cognitive functions (Phase 6)")
        .def("enableDevelopmentSystem", &Brain::enableDevelopmentSystem,
             "Enable development system (Phase 6)")
        .def("isMemoryIntegrated", &Brain::isMemoryIntegrated,
             "Check if memory systems are integrated (Phase 6)")
        .def("isNeuromodulationIntegrated", &Brain::isNeuromodulationIntegrated,
             "Check if neuromodulation systems are integrated (Phase 6)")
        .def("isPredictionIntegrated", &Brain::isPredictionIntegrated,
             "Check if prediction system is integrated (Phase 6)")
        .def("isDevelopmentIntegrated", &Brain::isDevelopmentIntegrated,
             "Check if development system is integrated (Phase 6)");

    // Enhanced AgentBrain with Phase 6 capabilities
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with Phase 6 advanced capabilities)pbdoc")
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
        // Phase 6 enhanced AgentBrain methods
        .def("enableAdvancedLearning", &AgentBrain::enableAdvancedLearning,
             "Enable advanced learning capabilities (Phase 6)")
        .def("enableCuriosityDrivenExploration", &AgentBrain::enableCuriosityDrivenExploration,
             "Enable curiosity-driven exploration (Phase 6)")
        .def("enablePredictivePlanning", &AgentBrain::enablePredictivePlanning,
             "Enable predictive planning (Phase 6)")
        .def("enableConceptLearning", &AgentBrain::enableConceptLearning,
             "Enable concept learning (Phase 6)")
        .def("enableAttentionManagement", &AgentBrain::enableAttentionManagement,
             "Enable attention management (Phase 6)")
        .def("enableSocialLearning", &AgentBrain::enableSocialLearning,
             "Enable social learning (Phase 6)")
        .def("enableContinualLearning", &AgentBrain::enableContinualLearning,
             "Enable continual learning (Phase 6)")
        .def("isAdvancedLearningEnabled", &AgentBrain::isAdvancedLearningEnabled,
             "Check if advanced learning is enabled (Phase 6)")
        .def("isCuriosityDrivenExplorationEnabled", &AgentBrain::isCuriosityDrivenExplorationEnabled,
             "Check if curiosity-driven exploration is enabled (Phase 6)")
        .def("isPredictivePlanningEnabled", &AgentBrain::isPredictivePlanningEnabled,
             "Check if predictive planning is enabled (Phase 6)")
        .def("getLearningEfficiency", &AgentBrain::getLearningEfficiency,
             "Get learning efficiency metric (Phase 6)")
        .def("getExplorationRate", &AgentBrain::getExplorationRate,
             "Get exploration rate (Phase 6)")
        .def("getPlanningHorizon", &AgentBrain::getPlanningHorizon,
             "Get planning horizon (Phase 6)")
        .def("getConceptComplexity", &AgentBrain::getConceptComplexity,
             "Get concept complexity (Phase 6)")
        .def("getAttentionSpan", &AgentBrain::getAttentionSpan,
             "Get attention span (Phase 6)")
        .def("getSocialLearningCapacity", &AgentBrain::getSocialLearningCapacity,
             "Get social learning capacity (Phase 6)")
        .def("getContinualLearningRate", &AgentBrain::getContinualLearningRate,
             "Get continual learning rate (Phase 6)");

    // Phase 6 Integration Experiment runner
    py::class_<Phase6IntegratedExperiment>(m, "Phase6IntegratedExperiment", R"pbdoc(Phase 6 integration experiment runner)pbdoc")
        .def(py::init<>())
        .def("run", &Phase6IntegratedExperiment::run,
             py::arg("config"), "Run the complete integration test")
        .def("verifyIntegration", &Phase6IntegratedExperiment::verifyIntegration,
             "Run a simple integration verification")
        .def("testMemoryIntegration", &Phase6IntegratedExperiment::testMemoryIntegration,
             "Test memory integration")
        .def("testNeuromodulationIntegration", &Phase6IntegratedExperiment::testNeuromodulationIntegration,
             "Test neuromodulation integration")
        .def("testCheckpointing", &Phase6IntegratedExperiment::testCheckpointing,
             "Test checkpoint save/load")
        .def("testReplay", &Phase6IntegratedExperiment::testReplay,
             "Test replay system");

    // Phase 6 Configuration
    py::class_<Phase6Config>(m, "Phase6Config", R"pbdoc(Configuration for Phase 6 integration experiment)pbdoc")
        .def(py::init<>())
        .def_readwrite("maxSteps", &Phase6Config::maxSteps)
        .def_readwrite("neuronCount", &Phase6Config::neuronCount)
        .def_readwrite("regionCount", &Phase6Config::regionCount)
        .def_readwrite("connectionProbability", &Phase6Config::connectionProbability)
        .def_readwrite("enableCheckpointing", &Phase6Config::enableCheckpointing)
        .def_readwrite("enableReplay", &Phase6Config::enableReplay)
        .def_readwrite("enableDevelopment", &Phase6Config::enableDevelopment)
        .def_readwrite("checkpointPath", &Phase6Config::checkpointPath);

    // Phase 6 Integration Result
    py::class_<Phase6IntegrationResult>(m, "Phase6IntegrationResult", R"pbdoc(Result of Phase 6 integration experiment)pbdoc")
        .def(py::init<>())
        .def_readonly("memoryWorkingMemoryIntegrated", &Phase6IntegrationResult::memoryWorkingMemoryIntegrated)
        .def_readonly("memoryEpisodicMemoryIntegrated", &Phase6IntegrationResult::memoryEpisodicMemoryIntegrated)
        .def_readonly("neuromodulationIntegrated", &Phase6IntegrationResult::neuromodulationIntegrated)
        .def_readonly("predictionIntegrated", &Phase6IntegrationResult::predictionIntegrated)
        .def_readonly("developmentIntegrated", &Phase6IntegrationResult::developmentIntegrated)
        .def_readonly("checkpointingWorks", &Phase6IntegrationResult::checkpointingWorks)
        .def_readonly("replayWorks", &Phase6IntegrationResult::replayWorks)
        .def_readonly("totalReward", &Phase6IntegrationResult::totalReward)
        .def_readonly("avgFiringRate", &Phase6IntegrationResult::avgFiringRate)
        .def_readonly("avgSynapticWeight", &Phase6IntegrationResult::avgSynapticWeight)
        .def_readonly("memoryEpisodesStored", &Phase6IntegrationResult::memoryEpisodesStored)
        .def_readonly("noveltyLevel", &Phase6IntegrationResult::noveltyLevel)
        .def_readonly("curiosityLevel", &Phase6IntegrationResult::curiosityLevel)
        .def_readonly("dopamineLevel", &Phase6IntegrationResult::dopamineLevel)
        .def_readonly("totalWallClockTime", &Phase6IntegrationResult::totalWallClockTime);

    // Factory functions (Phase 6 enhanced)
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("enable_advanced_memory", true, ConfigSource::Default);
        config->set("enable_neuromodulation", true, ConfigSource::Default);
        config->set("enable_prediction_system", true, ConfigSource::Default);
        config->set("enable_cognitive_functions", true, ConfigSource::Default);
        config->set("enable_development_system", true, ConfigSource::Default);
        return config;
    }, "Create a default Phase 6 configuration with all advanced features enabled");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new Phase 6 brain with advanced configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->enableDynamicEnvironment();
        return world;
    }, "Create a new Phase 6 world with dynamic environment");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->enableAdvancedLearning();
        agent->enableCuriosityDrivenExploration();
        agent->enablePredictivePlanning();
        return agent;
    }, py::arg("brain"), "Create a new Phase 6 agent brain with advanced capabilities");

    // Phase 6 Integration Experiment factory
    m.def("createPhase6Experiment", []() -> std::shared_ptr<Phase6IntegratedExperiment> {
        return std::make_shared<Phase6IntegratedExperiment>();
    }, "Create a Phase 6 integration experiment runner");

    // Constants (existing and Phase 6 additions)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Phase 6 specific constants
    m.attr("PHASE6_ADVANCED_FEATURES_ENABLED") = true;
    m.attr("PHASE6_DEFAULT_NEURON_COUNT") = 1000;
    m.attr("PHASE6_DEFAULT_MAX_STEPS") = 10000;
    m.attr("PHASE6_DEVELOPMENT_ENABLED") = true;
    m.attr("PHASE6_CHECKPOINTING_ENABLED") = true;
    m.attr("PHASE6_REPLAY_ENABLED") = true;
}

} // namespace nlm
