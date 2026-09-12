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
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/prediction/NeuralPrediction.hpp"

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

    // ========== MEMORY CLASSES ==========
    
    py::class_<Memory, std::shared_ptr<Memory>>(m, "Memory", R"pbdoc(Base class for all memory systems)pbdoc")
        .def("store", &Memory::store, py::arg("key"), py::arg("value"), "Store a value")
        .def("retrieve", &Memory::retrieve, py::arg("key"), "Retrieve a value")
        .def("clear", &Memory::clear, "Clear all memory")
        .def("size", &Memory::size, "Get memory size");
    
    py::class_<WorkingMemory, std::shared_ptr<WorkingMemory>>(m, "WorkingMemory", R"pbdoc(Working memory - transient active information)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("key"), py::arg("value"), "Store information")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("key"), "Retrieve information")
        .def("clear", &WorkingMemory::clear, "Clear working memory")
        .def("size", &WorkingMemory::size, "Get working memory size")
        .def("getLastAccessed", &WorkingMemory::getLastAccessed, "Get last access time");
    
    py::class_<EpisodicMemory, std::shared_ptr<EpisodicMemory>>(m, "EpisodicMemory", R"pbdoc(Episodic memory - experience storage)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &EpisodicMemory::storeEpisode, py::arg("timestamp"), py::arg("event"), "Store an episodic event")
        .def("retrieveEpisode", &EpisodicMemory::retrieveEpisode, py::arg("timestamp"), "Retrieve an episode")
        .def("search", &EpisodicMemory::search, py::arg("keyword"), "Search episodes")
        .def("clear", &EpisodicMemory::clear, "Clear episodic memory")
        .def("size", &EpisodicMemory::size, "Get episode count");
    
    py::class_<SemanticMemory, std::shared_ptr<SemanticMemory>>(m, "SemanticMemory", R"pbdoc(Semantic memory - facts and concepts)pbdoc")
        .def(py::init<>())
        .def("addFact", &SemanticMemory::addFact, py::arg("subject"), py::arg("predicate"), py::arg("object"), "Add a semantic fact")
        .def("query", &SemanticMemory::query, py::arg("subject"), py::arg("predicate"), "Query semantic facts")
        .def("getFacts", &SemanticMemory::getFacts, "Get all facts")
        .def("clear", &SemanticMemory::clear, "Clear semantic memory")
        .def("size", &SemanticMemory::size, "Get fact count");
    
    py::class_<ProceduralMemory, std::shared_ptr<ProceduralMemory>>(m, "ProceduralMemory", R"pbdoc(Procedural memory - skills and habits)pbdoc")
        .def(py::init<>())
        .def("recordSkill", &ProceduralMemory::recordSkill, py::arg("skillId"), py::arg("success"), "Record a skill execution")
        .def("retrieveSkill", &ProceduralMemory::retrieveSkill, py::arg("skillId"), "Retrieve skill data")
        .def("getSuccessRate", &ProceduralMemory::getSuccessRate, py::arg("skillId"), "Get success rate for skill")
        .def("clear", &ProceduralMemory::clear, "Clear procedural memory")
        .def("size", &ProceduralMemory::size, "Get skill count");
    
    py::class_<AssociativeMemory, std::shared_ptr<AssociativeMemory>>(m, "AssociativeMemory", R"pbdoc(Associative memory - pattern associations)pbdoc")
        .def(py::init<>())
        .def("associate", &AssociativeMemory::associate, py::arg("key1"), py::arg("key2"), py::arg("strength"), "Create association")
        .def("recall", &AssociativeMemory::recall, py::arg("key1"), "Recall associated items")
        .def("getStrength", &AssociativeMemory::getStrength, py::arg("key1"), py::arg("key2"), "Get association strength")
        .def("clear", &AssociativeMemory::clear, "Clear associative memory")
        .def("size", &AssociativeMemory::size, "Get association count");

    // ========== NEUROMODULATION CLASSES ==========
    
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulators)pbdoc")
        .def("getName", &Neuromodulator::getName, "Get modulator name")
        .def("getLevel", &Neuromodulator::getLevel, "Get current level")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"), "Set level")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor, "Get plasticity factor")
        .def("update", &Neuromodulator::update, py::arg("dt"), "Update state");
    
    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"), "Signal reward")
        .def("signalError", &Dopamine::signalRewardPredictionError, py::arg("error"), "Signal prediction error")
        .def("getName", &Dopamine::getName)
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor)
        .def("update", &Dopamine::update, py::arg("dt"));
    
    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setLevel", &Curiosity::setLevel, py::arg("level"), "Set curiosity level")
        .def("getLevel", &Curiosity::getLevel)
        .def("getName", &Curiosity::getName)
        .def("getPlasticityFactor", &Curiosity::getPlasticityFactor)
        .def("update", &Curiosity::update, py::arg("dt"));
    
    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty", R"pbdoc(Novelty detection neuromodulator)pbdoc")
        .def(py::init<>())
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level")
        .def("getLevel", &Novelty::getLevel)
        .def("getName", &Novelty::getName)
        .def("getPlasticityFactor", &Novelty::getPlasticityFactor)
        .def("update", &Novelty::update, py::arg("dt"));
    
    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"), "Compute prediction error")
        .def("getName", &PredictionError::getName)
        .def("getLevel", &PredictionError::getLevel)
        .def("setLevel", &PredictionError::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &PredictionError::getPlasticityFactor)
        .def("update", &PredictionError::update, py::arg("dt"));
    
    py::class_<Reward, std::shared_ptr<Reward>>(m, "Reward", R"pbdoc(Reward neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getName", &Reward::getName)
        .def("getLevel", &Reward::getLevel)
        .def("setLevel", &Reward::setLevel, py::arg("level"))
        .def("getPlasticityFactor", &Reward::getPlasticityFactor)
        .def("update", &Reward::update, py::arg("dt"));

    // ========== DEVELOPMENT CLASSES ==========
    
    py::class_<DevelopmentSystem, std::shared_ptr<DevelopmentSystem>>(m, "DevelopmentSystem", R"pbdoc(Development system)pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage)
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"))
        .def("advanceStage", &DevelopmentSystem::advanceStage)
        .def("getStageName", &DevelopmentSystem::getStageName)
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("step"), "Update development")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier)
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod)
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress);
    
    py::class_<Synaptogenesis, std::shared_ptr<Synaptogenesis>>(m, "Synaptogenesis", R"pbdoc(Synaptogenesis)pbdoc")
        .def(py::init<>())
        .def("getFormationRate", &Synaptogenesis::getFormationRate)
        .def("setFormationRate", &Synaptogenesis::setFormationRate, py::arg("rate"))
        .def("getTargetDensity", &Synaptogenesis::getTargetDensity)
        .def("setTargetDensity", &Synaptogenesis::setTargetDensity, py::arg("density"))
        .def("update", &Synaptogenesis::update, py::arg("brain"), py::arg("rng"), "Update synapse formation");
    
    py::class_<Pruning, std::shared_ptr<Pruning>>(m, "Pruning", R"pbdoc(Pruning)pbdoc")
        .def(py::init<>())
        .def("getThreshold", &Pruning::getThreshold)
        .def("setThreshold", &Pruning::setThreshold, py::arg("threshold"))
        .def("getPruningRate", &Pruning::getPruningRate)
        .def("setPruningRate", &Pruning::setPruningRate, py::arg("rate"))
        .def("update", &Pruning::update, py::arg("brain"), py::arg("rng"), "Update pruning")
        .def("pruneSynapse", &Pruning::pruneSynapse, py::arg("synapse"), "Force prune synapse");
    
    py::class_<Maturation, std::shared_ptr<Maturation>>(m, "Maturation", R"pbdoc(Maturation)pbdoc")
        .def(py::init<>())
        .def("getProgress", &Maturation::getProgress)
        .def("setProgress", &Maturation::setProgress, py::arg("progress"))
        .def("update", &Maturation::update, py::arg("brain"), py::arg("step"), "Update maturation")
        .def("getMatureThreshold", &Maturation::getMatureThreshold)
        .def("getMatureRestingPotential", &Maturation::getMatureRestingPotential)
        .def("getMatureTimeConstant", &Maturation::getMatureTimeConstant);

    // ========== COGNITION CLASSES ==========
    
    py::class_<ConceptFormation, std::shared_ptr<ConceptFormation>>(m, "ConceptFormation", R"pbdoc(Concept formation)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"), "Initialize")
        .def("presentExperience", &ConceptFormation::presentExperience, 
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("time"), "Present experience")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept, py::arg("pattern"), py::arg("threshold"), "Get matching concept")
        .def("getConcepts", &ConceptFormation::getConcepts)
        .def("getConcept", &ConceptFormation::getConcept, py::arg("id"), "Get concept")
        .def("getConceptCount", &ConceptFormation::getConceptCount)
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("a"), py::arg("b"), "Merge concepts")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("id"), py::arg("pattern"), py::arg("features"), py::arg("reward"), "Update concept")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"), "Compute similarity")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("threshold"), "Check if novel")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"), "Find concept for pattern")
        .def("clear", &ConceptFormation::clear)
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("id"), "Get generalization ability");
    
    py::class_<NeuralPlanner, std::shared_ptr<NeuralPlanner>>(m, "NeuralPlanner", R"pbdoc(Neural planner)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"), "Initialize")
        .def("planAction", &NeuralPlanner::planAction, py::arg("state"), py::arg("target"), "Plan action")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"), "Evaluate sequence")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth)
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"))
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence)
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("planned"), py::arg("actual"), py::arg("reward"), "Update plan quality")
        .def("clearCache", &NeuralPlanner::clearCache)
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"), "Set action quality")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal)
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful);

    // ========== PREDICTION CLASSES ==========
    
    py::class_<NeuralPrediction, std::shared_ptr<NeuralPrediction>>(m, "NeuralPrediction", R"pbdoc(Neural prediction)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPrediction::initialize, py::arg("brain"), "Initialize")
        .def("recordSensoryState", &NeuralPrediction::recordSensoryState, py::arg("state"), py::arg("step"), "Record sensory state")
        .def("generatePrediction", &NeuralPrediction::generatePrediction, py::arg("step"), "Generate prediction")
        .def("updateWithObservation", &NeuralPrediction::updateWithObservation, py::arg("actual"), py::arg("step"), "Update with observation")
        .def("predictActionConsequence", &NeuralPrediction::predictActionConsequence, 
             py::arg("action"), py::arg("state"), "Predict action consequence")
        .def("getPredictionError", &NeuralPrediction::getPredictionError)
        .def("getPredictionConfidence", &NeuralPrediction::getPredictionConfidence)
        .def("predictMultipleSteps", &NeuralPrediction::predictMultipleSteps, py::arg("step"), py::arg("steps"), "Predict multiple steps")
        .def("recordAction", &NeuralPrediction::recordAction, py::arg("action"), py::arg("step"), "Record action")
        .def("getErrorHistory", &NeuralPrediction::getErrorHistory)
        .def("clearHistory", &NeuralPrediction::clearHistory)
        .def("enableTemporalPrediction", &NeuralPrediction::enableTemporalPrediction, py::arg("enable"), "Enable temporal prediction")
        .def("enableActionConsequencePrediction", &NeuralPrediction::enableActionConsequencePrediction, py::arg("enable"), "Enable action consequence prediction")
        .def("setSequenceMemorySize", &NeuralPrediction::setSequenceMemorySize, py::arg("size"), "Set sequence memory size")
        .def("setPredictionHorizon", &NeuralPrediction::setPredictionHorizon, py::arg("horizon"), "Set prediction horizon")
        .def("getPredictionNeurons", &NeuralPrediction::getPredictionNeurons)
        .def("getSequenceNeurons", &NeuralPrediction::getSequenceNeurons);

    // ========== ENVIRONMENT CLASSES ==========
    
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)")
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

    // ========== ADDITIONAL BRAIN METHODS ==========
    
    // Add additional methods to Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize, "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step), py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step), 
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons, py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction, "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "", "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"), py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount, "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds, "Get all region IDs")
        .def("getRegions", &Brain::getRegions, py::return_value_policy::reference_internal, "Get all regions")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount, "Get total neuron count across all regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount, "Get total synapse count across all regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount, "Get count of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount, "Get count of currently firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate, "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio, "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount, "Get total spike count")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage, "Get current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage, py::arg("stage"), "Set developmental stage")
        .def("getConfig", &Brain::getConfig, py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus, "Log brain status")
        // Added neuromodulation methods
        .def("getDopamine", &Brain::getDopamine, "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity, "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty, "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal, "Get prediction error signal");
    
    // ========== ADDITIONAL AGENTBRAIN METHODS ==========
    
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", &AgentBrain::initialize, py::arg("world"), "Initialize with world")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize, "Get expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize, "Get expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand, "Decode brain motor activity into motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation, py::arg("reward"), py::arg("predictedReward"), "Apply reward-based neuromodulation")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"), "Update development system")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage, "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel, "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel, "Get curiosity level")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel, "Get novelty level")
        .def("getPredictionError", &AgentBrain::getPredictionError, "Get prediction error")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain, py::return_value_policy::reference_internal, "Get the underlying brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"))
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"))
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"))
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"))
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled)
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled)
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled)
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // ========== ADDITIONAL SIMPLEWORLD METHODS ==========
    
    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Configure world")
        .def("reset", &SimpleWorld::reset, "Reset world to initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"), "Set agent starting position")
        .def("update", &SimpleWorld::update, py::arg("timestep"), "Update world state (call each simulation step)")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand, py::arg("cmd"), py::arg("currentTime"), "Apply motor command to agent")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept, py::return_value_policy::reference_internal, "Get current sensory percept for the agent")
        .def("getAgentBody", &SimpleWorld::getAgentBody, py::return_value_policy::reference_internal, "Get agent body state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"), "Add object to world")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"), "Remove object at position")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"), "Check if position is valid (not wall)")
        .def("getWidth", &SimpleWorld::getWidth, "Get world width")
        .def("getHeight", &SimpleWorld::getHeight, "Get world height")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy, "Get maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"), "Set maximum energy")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate, "Get energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"), "Set energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime, "Get simulation time")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"), "Set random seed")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed, "Get random seed");

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

