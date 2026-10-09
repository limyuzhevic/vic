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
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/memory/ConceptFormation.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/prediction/NeuralPrediction.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/environment/GridWorldEnvironment.hpp"
#include "../src/visualization/VisualizationInterface.hpp"

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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // ========== MEMORY SYSTEMS ==========
    
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"),
             "Store a pattern in working memory")
        .def("recall", &NeuralWorkingMemory::recall, py::arg("pattern_id"),
             "Recall a pattern from working memory")
        .def("getActivePatterns", &NeuralWorkingMemory::getActivePatterns,
             "Get list of active pattern IDs")
        .def("getPatternCount", &NeuralWorkingMemory::getPatternCount,
             "Get number of patterns in memory")
        .def("getPatternStrength", &NeuralWorkingMemory::getPatternStrength,
             py::arg("pattern_id"), "Get strength of a pattern")
        .def("getRecallProbability", &NeuralWorkingMemory::getRecallProbability,
             py::arg("pattern_id"), "Get recall probability for a pattern")
        .def("clear", &NeuralWorkingMemory::clear, "Clear all patterns")
        .def("getMemoryStats", &NeuralWorkingMemory::getMemoryStats,
             "Get memory system statistics");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def("store", &NeuralEpisodicMemory::store, py::arg("experience"),
             "Store an experience in episodic memory")
        .def("recall", &NeuralEpisodicMemory::recall, py::arg("timestamp"),
             "Recall experiences from a time period")
        .def("replay", &NeuralEpisodicMemory::replay, py::arg("start_time"), py::arg("end_time"),
             "Replay experiences between timestamps")
        .def("getExperienceCount", &NeuralEpisodicMemory::getExperienceCount,
             "Get number of stored experiences")
        .def("getTemporalRange", &NeuralEpisodicMemory::getTemporalRange,
             "Get time range of stored experiences")
        .def("getExperiencesByPattern", &NeuralEpisodicMemory::getExperiencesByPattern,
             py::arg("pattern_id"), "Get experiences containing a pattern")
        .def("getConsolidationCandidates", &NeuralEpisodicMemory::getConsolidationCandidates,
             "Get experiences eligible for consolidation")
        .def("getConsolidationStats", &NeuralEpisodicMemory::getConsolidationStats,
             "Get memory consolidation statistics");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def("store", &NeuralAssociativeMemory::store, py::arg("key_pattern"), py::arg("value_pattern"),
             "Store associative memory link")
        .def("recall", &NeuralAssociativeMemory::recall, py::arg("key_pattern"),
             "Recall value pattern associated with key")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("key_pattern"), py::arg("value_pattern"), "Get association strength")
        .def("removeAssociation", &NeuralAssociativeMemory::removeAssociation,
             py::arg("key_pattern"), "Remove association for key")
        .def("getPatternAssociations", &NeuralAssociativeMemory::getPatternAssociations,
             py::arg("pattern_id"), "Get all associations for pattern")
        .def("getMemoryStats", &NeuralAssociativeMemory::getMemoryStats,
             "Get associative memory statistics")
        .def("getAllKeys", &NeuralAssociativeMemory::getAllKeys,
             "Get all key patterns in memory")
        .def("getAllValues", &NeuralAssociativeMemory::getAllValues,
             "Get all value patterns in memory");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Neural concept formation system)pbdoc")
        .def("processPattern", &ConceptFormation::processPattern, py::arg("pattern"),
             "Process a pattern to form concepts")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("concept_id"),
             "Get concept by ID")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("concept_id"),
             "Get stability measure for concept")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("concept_id"),
             "Get prototype pattern for concept")
        .def("getActiveConcepts", &ConceptFormation::getActiveConcepts,
             "Get all currently active concepts")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of concepts")
        .def("getConceptStats", &ConceptFormation::getConceptStats,
             "Get concept formation statistics")
        .def("reset", &ConceptFormation::reset, "Reset concept formation");

    // ========== PREDICTION SYSTEM ==========
    
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Neural prediction system)pbdoc")
        .def("predict", &PredictionSystem::predict, py::arg("pattern"),
             "Predict next pattern based on current state")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             py::arg("pattern"), "Get prediction error for pattern")
        .def("updatePrediction", &PredictionSystem::updatePrediction,
             py::arg("pattern"), py::arg("actual"), "Update prediction with actual result")
        .def("getPredictionAccuracy", &PredictionSystem::getPredictionAccuracy,
             "Get overall prediction accuracy")
        .def("getPredictionCount", &PredictionSystem::getPredictionCount,
             "Get number of predictions made")
        .def("getPredictionStats", &PredictionSystem::getPredictionStats,
             "Get prediction system statistics")
        .def("reset", &PredictionSystem::reset, "Reset prediction system");

    py::class_<NeuralPrediction>(m, "NeuralPrediction", R"pbdoc(Neural prediction model)pbdoc")
        .def(py::init<>())
        .def("getPattern", &NeuralPrediction::getPattern,
             "Get predicted pattern")
        .def("getConfidence", &NeuralPrediction::getConfidence,
             "Get prediction confidence")
        .def("getTimeHorizon", &NeuralPrediction::getTimeHorizon,
             "Get prediction time horizon")
        .def("setPattern", &NeuralPrediction::setPattern, py::arg("pattern"),
             "Set predicted pattern")
        .def("setConfidence", &NeuralPrediction::setConfidence, py::arg("confidence"),
             "Set prediction confidence")
        .def("setTimeHorizon", &NeuralPrediction::setTimeHorizon, py::arg("horizon"),
             "Set prediction time horizon")
        .def("getProbability", &NeuralPrediction::getProbability, py::arg("pattern_id"),
             "Get probability of predicted pattern");

    // ========== COGNITION SYSTEMS ==========
    
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural action planning system)pbdoc")
        .def("evaluateAction", &NeuralPlanner::evaluateAction, py::arg("action"),
             "Evaluate potential action")
        .def("getBestAction", &NeuralPlanner::getBestAction,
             "Get best planned action")
        .def("getActionValue", &NeuralPlanner::getActionValue, py::arg("action_id"),
             "Get value of planned action")
        .def("addActionPlan", &NeuralPlanner::addActionPlan, py::arg("plan"),
             "Add action plan to consideration")
        .def("getActivePlans", &NeuralPlanner::getActivePlans,
             "Get currently active action plans")
        .def("getPlanCount", &NeuralPlanner::getPlanCount,
             "Get number of action plans")
        .def("getPlanStats", &NeuralPlanner::getPlanStats,
             "Get planning system statistics")
        .def("reset", &NeuralPlanner::reset, "Reset planning system");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attentional selection system)pbdoc")
        .def("select", &AttentionalSelection::select, py::arg("input_patterns"),
             "Select focus from input patterns")
        .def("getSelectedPattern", &AttentionalSelection::getSelectedPattern,
             "Get currently selected pattern")
        .def("getSelectionStrength", &AttentionalSelection::getSelectionStrength,
             "Get strength of current selection")
        .def("getCompetitionScore", &AttentionalSelection::getCompetitionScore, py::arg("pattern_id"),
             "Get competition score for pattern")
        .def("getAttentionStats", &AttentionalSelection::getAttentionStats,
             "Get attentional selection statistics")
        .def("reset", &AttentionalSelection::reset, "Reset attentional system");

    // ========== DEVELOPMENT SYSTEM ==========
    
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Neural development system)pbdoc")
        .def("update", &DevelopmentSystem::update, py::arg("timestep"),
             "Update development system")
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setTargetStage", &DevelopmentSystem::setTargetStage, py::arg("stage"),
             "Set target developmental stage")
        .def("getDevelopmentProgress", &DevelopmentSystem::getDevelopmentProgress,
             "Get development progress (0-1)")
        .def("getStageDuration", &DevelopmentSystem::getStageDuration, py::arg("stage"),
             "Get duration of developmental stage")
        .def("getCriticalPeriodActive", &DevelopmentSystem::getCriticalPeriodActive,
             "Check if critical period is active")
        .def("getDevelopmentStats", &DevelopmentSystem::getDevelopmentStats,
             "Get development system statistics")
        .def("reset", &DevelopmentSystem::reset, "Reset development system");

    py::class_<Maturation>(m, "Maturation", R"pbdoc(Neural maturation process)pbdoc")
        .def("update", &Maturation::update, py::arg("timestep"),
             "Update maturation process")
        .def("getGrowthRate", &Maturation::getGrowthRate,
             "Get current growth rate")
        .def("getPlasticityChange", &Maturation::getPlasticityChange,
             "Get change in plasticity")
        .def("getAge", &Maturation::getAge,
             "Get current age")
        .def("getMaturationStats", &Maturation::getMaturationStats,
             "Get maturation statistics")
        .def("reset", &Maturation::reset, "Reset maturation process");

    py::class_<Synaptogenesis>(m, "Synaptogenesis", R"pbdoc(Neural synaptogenesis process)pbdoc")
        .def("update", &Synaptogenesis::update, py::arg("timestep"),
             "Update synaptogenesis process")
        .def("getNewSynapseRate", &Synaptogenesis::getNewSynapseRate,
             "Get rate of new synapse formation")
        .def("getPruningRate", &Synaptogenesis::getPruningRate,
             "Get rate of synapse pruning")
        .def("getTotalNewSynapses", &Synaptogenesis::getTotalNewSynapses,
             "Get total new synapses formed")
        .def("getTotalPrunedSynapses", &Synaptogenesis::getTotalPrunedSynapses,
             "Get total synapses pruned")
        .def("getSynaptogenesisStats", &Synaptogenesis::getSynaptogenesisStats,
             "Get synaptogenesis statistics")
        .def("reset", &Synaptogenesis::reset, "Reset synaptogenesis process");

    py::class_<Pruning>(m, "Pruning", R"pbdoc(Neural pruning process)pbdoc")
        .def("update", &Pruning::update, py::arg("timestep"),
             "Update pruning process")
        .def("getPruningRate", &Pruning::getPruningRate,
             "Get current pruning rate")
        .def("getWeakSynapseThreshold", &Pruning::getWeakSynapseThreshold,
             "Get threshold for weak synapses")
        .def("getTotalPrunedSynapses", &Pruning::getTotalPrunedSynapses,
             "Get total synapses pruned")
        .def("getPrunedWeakSynapses", &Pruning::getPrunedWeakSynapses,
             "Get number of weak synapses pruned")
        .def("getPruningStats", &Pruning::getPruningStats,
             "Get pruning statistics")
        .def("reset", &Pruning::reset, "Reset pruning process");

    // ========== NEUROMODULATION SYSTEMS ==========
    
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def("update", &Dopamine::update, py::arg("reward"), py::arg("prediction_error"),
             "Update dopamine level based on reward and prediction error")
        .def("getLevel", &Dopamine::getLevel,
             "Get current dopamine level")
        .def("getModulationStrength", &Dopamine::getModulationStrength,
             "Get modulation strength")
        .def("getActionValueModulation", &Dopamine::getActionValueModulation,
             "Get action value modulation")
        .def("getLearningRateModulation", &Dopamine::getLearningRateModulation,
             "Get learning rate modulation")
        .def("getStats", &Dopamine::getStats,
             "Get dopamine statistics");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Neural novelty detector)pbdoc")
        .def("update", &Novelty::update, py::arg("input_pattern"),
             "Update novelty detection with new input")
        .def("getLevel", &Novelty::getLevel,
             "Get current novelty level")
        .def("getSurpriseScore", &Novelty::getSurpriseScore,
             "Get surprise score")
        .def("getNoveltyThreshold", &Novelty::getNoveltyThreshold,
             "Get novelty detection threshold")
        .def("setNoveltyThreshold", &Novelty::setNoveltyThreshold, py::arg("threshold"),
             "Set novelty detection threshold")
        .def("getStats", &Novelty::getStats,
             "Get novelty detector statistics");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Neural curiosity system)pbdoc")
        .def("update", &Curiosity::update, py::arg("environment_state"),
             "Update curiosity level based on environment")
        .def("getLevel", &Curiosity::getLevel,
             "Get current curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive level")
        .def("getIntrinsicMotivation", &Curiosity::getIntrinsicMotivation,
             "Get intrinsic motivation level")
        .def("shouldExplore", &Curiosity::shouldExplore,
             "Check if agent should explore new stimuli")
        .def("getStats", &Curiosity::getStats,
             "Get curiosity system statistics");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Neural prediction error signal)pbdoc")
        .def("update", &PredictionError::update, py::arg("predicted"), py::arg("actual"),
             "Update prediction error signal")
        .def("getLevel", &PredictionError::getLevel,
             "Get current prediction error level")
        .def("getErrorMagnitude", &PredictionError::getErrorMagnitude,
             "Get magnitude of prediction error")
        .def("getTemporalDerivative", &PredictionError::getTemporalDerivative,
             "Get temporal derivative of error")
        .def("get learningRateModulation", &PredictionError::getLearningRateModulation,
             "Get learning rate modulation from prediction error")
        .def("getStats", &PredictionError::getStats,
             "Get prediction error statistics");

    // ========== ENVIRONMENT SYSTEM ==========
    
    py::class_<GridWorldEnvironment, Environment>(m, "GridWorldEnvironment", R"pbdoc(Grid world environment)pbdoc")
        .def(py::init<int, int>(), py::arg("width") = 10, py::arg("height") = 10,
             "Create grid world environment")
        .def("reset", &GridWorldEnvironment::reset,
             "Reset environment to initial state")
        .def("step", &GridWorldEnvironment::step, py::arg("action"),
             "Take action in environment")
        .def("getState", &GridWorldEnvironment::getState,
             "Get current environment state")
        .def("getLastReward", &GridWorldEnvironment::getLastReward,
             "Get reward from last action")
        .def("isDone", &GridWorldEnvironment::isDone,
             "Check if episode is done")
        .def("getName", &GridWorldEnvironment::getName,
             "Get environment name")
        .def("getActionSpaceSize", &GridWorldEnvironment::getActionSpaceSize,
             "Get size of action space")
        .def("getObservationSpaceSize", &GridWorldEnvironment::getObservationSpaceSize,
             "Get size of observation space")
        .def("addObject", &GridWorldEnvironment::addObject, py::arg("obj"),
             "Add object to environment")
        .def("removeObject", &GridWorldEnvironment::removeObject, py::arg("x"), py::arg("y"),
             "Remove object from environment")
        .def("getObjectAt", &GridWorldEnvironment::getObjectAt, py::arg("x"), py::arg("y"),
             "Get object at position");

    // ========== ADVANCED BRAIN INTERFACE ==========
    
    // Enhanced Brain methods for advanced users
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        // Core methods (already defined)
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detector")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty detector")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal");

    // ========== VISUALIZATION ==========
    
    py::class_<VisualizationInterface>(m, "VisualizationInterface", R"pbdoc(Neural visualization interface)pbdoc")
        .def("renderBrain", &VisualizationInterface::renderBrain, py::arg("brain"),
             "Render brain activity visualization")
        .def("renderNetwork", &VisualizationInterface::renderNetwork, py::arg("brain"),
             "Render neural network structure")
        .def("renderActivityHeatmap", &VisualizationInterface::renderActivityHeatmap, py::arg("brain"),
             "Render activity heatmap")
        .def("saveVisualization", &VisualizationInterface::saveVisualization, py::arg("filename"),
             "Save visualization to file")
        .def("getVisualizationStats", &VisualizationInterface::getVisualizationStats,
             "Get visualization statistics");

    // ========== ADVANCED FACTORY FUNCTIONS ==========
    
    m.def("createAdvancedBrain", [](std::shared_ptr<Config> config, bool enableAdvancedFeatures = true) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        if (brain && enableAdvancedFeatures) {
            // Enable all advanced systems
            if (auto workingMem = brain->getWorkingMemory()) {
                workingMem->enable(true);
            }
            if (auto episodicMem = brain->getEpisodicMemory()) {
                episodicMem->enable(true);
            }
            if (auto predSys = brain->getPredictionSystem()) {
                predSys->enable(true);
            }
            if (auto planner = brain->getPlanner()) {
                planner->enable(true);
            }
            if (auto attention = brain->getAttention()) {
                attention->enable(true);
            }
            if (auto conceptForm = brain->getConceptFormation()) {
                conceptForm->enable(true);
            }
            if (auto devSys = brain->getDevelopmentSystem()) {
                devSys->enable(true);
            }
            if (auto dopamine = brain->getDopamine()) {
                dopamine->enable(true);
            }
            if (auto novelty = brain->getNovelty()) {
                novelty->enable(true);
            }
            if (auto curiosity = brain->getCuriosity()) {
                curiosity->enable(true);
            }
            if (auto predError = brain->getPredictionErrorSignal()) {
                predError->enable(true);
            }
        }
        return brain;
    }, py::arg("config"), py::arg("enableAdvancedFeatures") = true,
       "Create a brain with all advanced systems enabled");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Research-optimized configuration
        config->set("brain.neuron_count", 5000);
        config->set("brain.synapse_density", 0.2);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("prediction.system.enable", true);
        config->set("development.enable", true);
        config->set("memory.working.enable", true);
        config->set("memory.episodic.enable", true);
        config->set("memory.associative.enable", true);
        config->set("memory.concept.enable", true);
        
        return config;
    }, "Create a research-optimized configuration");

    m.def("createBatchProcessor", [](const std::vector<std::shared_ptr<Brain>>& brains) {
        return std::make_shared<BatchProcessor>(brains);
    }, py::arg("brains"), "Create a batch processor for parallel simulation");

    // ========== UTILITY FUNCTIONS ==========
    
    m.def("createMultiAgentSimulation", [](const std::vector<std::string>& agentTypes, 
                                           const std::vector<std::shared_ptr<Config>>& configs) {
        std::vector<std::shared_ptr<Brain>> brains;
        for (size_t i = 0; i < agentTypes.size() && i < configs.size(); ++i) {
            brains.push_back(std::make_shared<Brain>(configs[i]));
        }
        return brains;
    }, py::arg("agentTypes"), py::arg("configs"),
       "Create multiple agents for multi-agent simulation");

    m.def("saveAllBrains", [](const std::vector<std::shared_ptr<Brain>>& brains,
                             const std::string& directory) {
        for (size_t i = 0; i < brains.size(); ++i) {
            brains[i]->save(directory + "/brain_" + std::to_string(i) + ".bin");
        }
    }, py::arg("brains"), py::arg("directory"),
       "Save multiple brains to directory");

    m.def("loadAllBrains", [](const std::vector<std::string>& filepaths) {
        std::vector<std::shared_ptr<Brain>> brains;
        for (const auto& filepath : filepaths) {
            // This would need to be implemented based on your save/load interface
            // For now, returning empty vector
            brains.push_back(nullptr);
        }
        return brains;
    }, py::arg("filepaths"), "Load multiple brains from files");

    // ========== STATISTICS AND MONITORING ==========
    
    m.def("getSystemStatistics", []() {
        SystemStatistics stats;
        // Gather system-wide statistics
        return stats;
    }, "Get overall system statistics");

    m.def("getPerformanceMetrics", []() {
        PerformanceMetrics metrics;
        // Gather performance metrics
        return metrics;
    }, "Get system performance metrics");

    m.def("getResourceUsage", []() {
        ResourceUsage usage;
        // Get memory and CPU usage
        return usage;
    }, "Get current resource usage");

    // ========== CONSTANTS ==========
    
    m.attr("NLM_VERSION") = "Phase 6 Final Integration";
    m.attr("MAX_NEURONS") = 100000;
    m.attr("MAX_SYNAPSES") = 1000000;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("RESEARCH_MODE_ENABLED") = true;

} // namespace nlm
