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

// Include memory systems
#include "../src/memory/Memory.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/SemanticMemory.hpp"
#include "../src/memory/ProceduralMemory.hpp"

// Include prediction system
#include "../src/prediction/PredictionSystem.hpp"

// Include cognition systems
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/SelfModel.hpp"
#include "../src/cognition/SocialLearning.hpp"

// Include enhanced neuromodulation systems
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Acetylcholine.hpp"
#include "../src/neuromodulation/Norepinephrine.hpp"
#include "../src/neuromodulation/Serotonin.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Reward.hpp"
#include "../src/neuromodulation/novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

// Include development system
#include "../src/development/DevelopmentSystem.hpp"

// Include persistence systems
#include "../src/persistence/CheckpointSystem.hpp"

// Include advanced simulation features
#include "../src/advanced/AdvancedTools.hpp"
#include "../src/advanced/AdvancedSimulation.hpp"

// Include enhanced statistics and utilities
#include "../src/utils/EnhancedStatistics.hpp"
#include "../src/utils/NetworkTopology.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive classes for:
        - Core types (NeuronId, SynapseId, RegionId, PopulationId, etc.)
        - Configuration management (Config class)
        - Sensory processing (Vision, Audio, InternalSignals)
        - Motor actions and control (Action, WorldObject, AgentBody)
        - World simulation (SimpleWorld)
        - Perception and integration (SensoryPercept, AgentBrain)
        - **ADVANCED SYSTEMS**:
          - Memory systems (WorkingMemory, EpisodicMemory, SemanticMemory, ProceduralMemory)
          - Prediction systems (PredictionSystem)
          - Cognitive systems (NeuralPlanner, ConceptFormation, SelfModel, SocialLearning)
          - Neuromodulation (Dopamine, Acetylcholine, Norepinephrine, Serotonin, Curiosity, Novelty, PredictionError)
          - Development system (DevelopmentSystem)
          - Persistence (CheckpointSystem)
          - Advanced simulation tools (AdvancedTools, AdvancedSimulation)
          - Enhanced statistics (EnhancedStatistics, NetworkTopology)

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

    // === Memory Systems ===
    
    py::class_<WorkingMemory, std::shared_ptr<WorkingMemory>>(m, "WorkingMemory", R"pbdoc(Short-term working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &WorkingMemory::store, py::arg("key"), py::arg("value"),
             "Store a value in working memory")
        .def("retrieve", &WorkingMemory::retrieve, py::arg("key"),
             "Retrieve a value from working memory")
        .def("has", &WorkingMemory::has, py::arg("key"),
             "Check if a key exists in working memory")
        .def("erase", &WorkingMemory::erase, py::arg("key"),
             "Erase a key from working memory")
        .def("clear", &WorkingMemory::clear,
             "Clear all working memory")
        .def("size", &WorkingMemory::size,
             "Get the number of items in working memory")
        .def("getKeys", &WorkingMemory::getKeys,
             "Get all keys in working memory")
        .def("update", &WorkingMemory::update, py::arg("key"), py::arg("value"),
             "Update an existing value in working memory")
        .def("getTTL", &WorkingMemory::getTTL, py::arg("key"),
             "Get time-to-live for a key in working memory");

    py::class_<EpisodicMemory, std::shared_ptr<EpisodicMemory>>(m, "EpisodicMemory", R"pbdoc(Long-term episodic memory system)pbdoc")
        .def(py::init<>())
        .def("record", &EpisodicMemory::record, py::arg("event"), py::arg("context"),
             "Record an episodic event")
        .def("recall", &EpisodicMemory::recall, py::arg("pattern"),
             "Recall events matching a pattern")
        .def("getRecent", &EpisodicMemory::getRecent, py::arg("count") = 10,
             "Get recent episodic events")
        .def("search", &EpisodicMemory::search, py::arg("query"), py::arg("context"),
             "Search for episodic events")
        .def("addContext", &EpisodicMemory::addContext, py::arg("eventId"), py::arg("context"),
             "Add context to an event")
        .def("removeContext", &EpisodicMemory::removeContext, py::arg("eventId"), py::arg("contextKey"),
             "Remove context from an event")
        .def("getEventCount", &EpisodicMemory::getEventCount,
             "Get total number of events")
        .def("getContextCount", &EpisodicMemory::getContextCount,
             "Get total number of context entries");

    py::class_<SemanticMemory, std::shared_ptr<SemanticMemory>>(m, "SemanticMemory", R"pbdoc(Knowledge graph semantic memory system)pbdoc")
        .def(py::init<>())
        .def("storeFact", &SemanticMemory::storeFact, py::arg("subject"), py::arg("predicate"), py::arg("object"),
             "Store a semantic fact")
        .def("retrieveFact", &SemanticMemory::retrieveFact, py::arg("subject"), py::arg("predicate"), py::arg("object"),
             "Retrieve a semantic fact")
        .def("query", &SemanticMemory::query, py::arg("pattern"),
             "Query semantic knowledge graph")
        .def("getAllFacts", &SemanticMemory::getAllFacts,
             "Get all semantic facts")
        .def("clear", &SemanticMemory::clear,
             "Clear all semantic knowledge")
        .def("factCount", &SemanticMemory::factCount,
             "Get number of facts")
        .def("getSubjects", &SemanticMemory::getSubjects,
             "Get all subjects")
        .def("getPredicates", &SemanticMemory::getPredicates,
             "Get all predicates")
        .def("getObjects", &SemanticMemory::getObjects,
             "Get all objects")
        .def("infer", &SemanticMemory::infer, py::arg("subject"), py::arg("predicate"),
             "Infer missing object from subject-predicate pair");

    py::class_<ProceduralMemory, std::shared_ptr<ProceduralMemory>>(m, "ProceduralMemory", R"pbdoc(Procedural knowledge memory system)pbdoc")
        .def(py::init<>())
        .def("storeProcedure", &ProceduralMemory::storeProcedure, py::arg("name"), py::arg("steps"),
             "Store a procedural sequence")
        .def("executeProcedure", &ProceduralMemory::executeProcedure, py::arg("name"),
             "Execute a stored procedure")
        .def("retrieveProcedure", &ProceduralMemory::retrieveProcedure, py::arg("name"),
             "Retrieve a stored procedure")
        .def("deleteProcedure", &ProceduralMemory::deleteProcedure, py::arg("name"),
             "Delete a stored procedure")
        .def("listProcedures", &ProceduralMemory::listProcedures,
             "List all stored procedures")
        .def("clear", &ProceduralMemory::clear,
             "Clear all procedures")
        .def("procedureCount", &ProceduralMemory::procedureCount,
             "Get number of procedures")
        .def("getStepCount", &ProceduralMemory::getStepCount, py::arg("name"),
             "Get number of steps in a procedure")
        .def("addStep", &ProceduralMemory::addStep, py::arg("name"), py::arg("step"), py::arg("position"),
             "Add a step to a procedure");

    // === Prediction System ===
    
    py::class_<PredictionSystem, std::shared_ptr<PredictionSystem>>(m, "PredictionSystem", R"pbdoc(Prediction and error-based learning system)pbdoc")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predict, py::arg("input"), py::arg("horizon"),
             "Make a prediction from input")
        .def("calculateError", &PredictionSystem::calculateError, py::arg("predicted"), py::arg("actual"),
             "Calculate prediction error")
        .def("updateModel", &PredictionSystem::updateModel, py::arg("input"), py::arg("target"),
             "Update prediction model with error signal")
        .def("getPredictions", &PredictionSystem::getPredictions,
             "Get recent predictions")
        .def("getPredictionCount", &PredictionSystem::getPredictionCount,
             "Get number of predictions")
        .def("clear", &PredictionSystem::clear,
             "Clear prediction history")
        .def("setLearningRate", &PredictionSystem::setLearningRate, py::arg("rate"),
             "Set learning rate for prediction model")
        .def("getLearningRate", &PredictionSystem::getLearningRate,
             "Get current learning rate")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get current prediction error")
        .def("isConverged", &PredictionSystem::isConverged,
             "Check if prediction model has converged");

    // === Cognition Systems ===
    
    py::class_<NeuralPlanner, std::shared_ptr<NeuralPlanner>>(m, "NeuralPlanner", R"pbdoc(Neural planning and decision-making system)pbdoc")
        .def(py::init<>())
        .def("plan", &NeuralPlanner::plan, py::arg("goal"), py::arg("constraints"),
             "Plan a sequence of actions to achieve a goal")
        .def("getPlan", &NeuralPlanner::getPlan,
             "Get the current plan")
        .def("executePlan", &NeuralPlanner::executePlan,
             "Execute the current plan")
        .def("cancelPlan", &NeuralPlanner::cancelPlan,
             "Cancel the current plan")
        .def("isPlanning", &NeuralPlanner::isPlanning,
             "Check if currently planning")
        .def("setGoal", &NeuralPlanner::setGoal, py::arg("goal"),
             "Set the planning goal")
        .def("getGoal", &NeuralPlanner::getGoal,
             "Get the current goal")
        .def("getConstraints", &NeuralPlanner::getConstraints,
             "Get current constraints")
        .def("addConstraint", &NeuralPlanner::addConstraint, py::arg("constraint"),
             "Add a constraint to planning")
        .def("clearConstraints", &NeuralPlanner::clearConstraints,
             "Clear all constraints")
        .def("getPlanningSteps", &NeuralPlanner::getPlanningSteps,
             "Get planning step count");

    py::class_<ConceptFormation, std::shared_ptr<ConceptFormation>>(m, "ConceptFormation", R"pbdoc(Concept abstraction and categorization system)pbdoc")
        .def(py::init<>())
        .def("learnConcept", &ConceptFormation::learnConcept, py::arg("pattern"), py::arg("label"),
             "Learn a new concept from a pattern")
        .def("recognize", &ConceptFormation::recognize, py::arg("input"),
             "Recognize a concept from input")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("label"),
             "Get concept details")
        .def("getConcepts", &ConceptFormation::getConcepts,
             "Get all learned concepts")
        .def("deleteConcept", &ConceptFormation::deleteConcept, py::arg("label"),
             "Delete a concept")
        .def("clear", &ConceptFormation::clear,
             "Clear all concepts")
        .def("conceptCount", &ConceptFormation::conceptCount,
             "Get number of concepts")
        .def("getPatternCount", &ConceptFormation::getPatternCount, py::arg("label"),
             "Get number of patterns for a concept")
        .def("addPattern", &ConceptFormation::addPattern, py::arg("label"), py::arg("pattern"),
             "Add a pattern to a concept");

    py::class_<SelfModel, std::shared_ptr<SelfModel>>(m, "SelfModel", R"pbdoc(Self-representation and identity system)pbdoc")
        .def(py::init<>())
        .def("update", &SelfModel::update, py::arg("sensoryInput"), py::arg("action"), py::arg("result"),
             "Update self-model based on interaction")
        .def("getSelfState", &SelfModel::getSelfState,
             "Get current self state")
        .def("getCapabilities", &SelfModel::getCapabilities,
             "Get current capabilities")
        .def("getPreferences", &SelfModel::getPreferences,
             "Get current preferences")
        .def("getGoals", &SelfModel::getGoals,
             "Get current goals")
        .def("addGoal", &SelfModel::addGoal, py::arg("goal"),
             "Add a goal")
        .def("removeGoal", &SelfModel::removeGoal, py::arg("goal"),
             "Remove a goal")
        .def("getActionHistory", &SelfModel::getActionHistory,
             "Get action history")
        .def("getExperience", &SelfModel::getExperience,
             "Get accumulated experience")
        .def("getNovelty", &SelfModel::getNovelty,
             "Get novelty level")
        .def("getCuriosity", &SelfModel::getCuriosity,
             "Get curiosity level")
        .def("reset", &SelfModel::reset,
             "Reset self-model");

    py::class_<SocialLearning, std::shared_ptr<SocialLearning>>(m, "SocialLearning", R"pbdoc(Social learning and imitation system)pbdoc")
        .def(py::init<>())
        .def("observe", &SocialLearning::observe, py::arg("agent"), py::arg("behavior"),
             "Observe another agent's behavior")
        .def("imitate", &SocialLearning::imitate, py::arg("agentId"),
             "Imitate another agent's behavior")
        .def("getObservedAgents", &SocialLearning::getObservedAgents,
             "Get list of observed agents")
        .def("getBehaviorHistory", &SocialLearning::getBehaviorHistory, py::arg("agentId"),
             "Get behavior history of an agent")
        .def("getImitationSuccessRate", &SocialLearning::getImitationSuccessRate, py::arg("agentId"),
             "Get imitation success rate")
        .def("clearHistory", &SocialLearning::clearHistory,
             "Clear observation history")
        .def("setLearningRate", &SocialLearning::setLearningRate, py::arg("rate"),
             "Set social learning rate")
        .def("getLearningRate", &SocialLearning::getLearningRate,
             "Get current social learning rate")
        .def("getObservationCount", &SocialLearning::getObservationCount,
             "Get number of observations");

    // === Enhanced Neuromodulation Systems ===
    
    py::class_<Dopamine, std::shared_ptr<Dopamine>>(m, "Dopamine", R"pbdoc(Dopamine neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("processPredictionError", &Dopamine::processPredictionError, py::arg("error"), py::arg("timeStep"),
             "Process prediction error for dopamine modulation")
        .def("getModulationLevel", &Dopamine::getModulationLevel, py::arg("timeStep"),
             "Get dopamine modulation level at time step")
        .def("getAverageLevel", &Dopamine::getAverageLevel,
             "Get average dopamine level")
        .def("setBaseline", &Dopamine::setBaseline, py::arg("baseline"),
             "Set baseline dopamine level")
        .def("getBaseline", &Dopamine::getBaseline,
             "Get baseline dopamine level")
        .def("reset", &Dopamine::reset,
             "Reset dopamine system");

    py::class_<Acetylcholine, std::shared_ptr<Acetylcholine>>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("processAttention", &Acetylcholine::processAttention, py::arg("sensoryInput"), py::arg("salience"),
             "Process attention signal")
        .def("getAttentionLevel", &Acetylcholine::getAttentionLevel, py::arg("timeStep"),
             "Get attention level at time step")
        .def("getAverageAttention", &Acetylcholine::getAverageAttention,
             "Get average attention level")
        .def("setFocus", &Acetylcholine::setFocus, py::arg("focusArea"),
             "Set focus area")
        .def("getFocus", &Acetylcholine::getFocus,
             "Get current focus area")
        .def("reset", &Acetylcholine::reset,
             "Reset acetylcholine system");

    py::class_<Norepinephrine, std::shared_ptr<Norepinephrine>>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("processAlertness", &Norepinephrine::processAlertness, py::arg("stimulus"), py::arg("intensity"),
             "Process alertness signal")
        .def("getAlertnessLevel", &Norepinephrine::getAlertnessLevel, py::arg("timeStep"),
             "Get alertness level at time step")
        .def("getPeakAlertness", &Norepinephrine::getPeakAlertness,
             "Get peak alertness level")
        .def("setArousal", &Norepinephrine::setArousal, py::arg("arousal"),
             "Set arousal level")
        .def("getArousal", &Norepinephrine::getArousal,
             "Get current arousal level")
        .def("reset", &Norepinephrine::reset,
             "Reset norepinephrine system");

    py::class_<Serotonin, std::shared_ptr<Serotonin>>(m, "Serotonin", R"pbdoc(Serotonin neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("processMood", &Serotonin::processMood, py::arg("context"), py::arg("social"),
             "Process mood signal")
        .def("getMoodLevel", &Serotonin::getMoodLevel, py::arg("timeStep"),
             "Get mood level at time step")
        .def("getAverageMood", &Serotonin::getAverageMood,
             "Get average mood level")
        .def("setTone", &Serotonin::setTone, py::arg("tone"),
             "Set mood tone")
        .def("getTone", &Serotonin::getTone,
             "Get current mood tone")
        .def("reset", &Serotonin::reset,
             "Reset serotonin system");

    // Legacy neuromodulation classes for backward compatibility
    py::class_<Neuromodulator, std::shared_ptr<Neuromodulator>>(m, "Neuromodulator", R"pbdoc(Generic neuromodulator base class)pbdoc")
        .def(py::init<>())
        .def("update", &Neuromodulator::update, py::arg("input"), py::arg("timeStep"),
             "Update neuromodulator state")
        .def("getLevel", &Neuromodulator::getLevel, py::arg("timeStep"),
             "Get modulation level at time step")
        .def("reset", &Neuromodulator::reset,
             "Reset neuromodulator");

    py::class_<PredictionError, std::shared_ptr<PredictionError>>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("calculate", &PredictionError::calculate, py::arg("predicted"), py::arg("actual"),
             "Calculate prediction error")
        .def("getError", &PredictionError::getError,
             "Get current error value")
        .def("getSquaredError", &PredictionError::getSquaredError,
             "Get squared error value")
        .def("getAbsoluteError", &PredictionError::getAbsoluteError,
             "Get absolute error value")
        .def("reset", &PredictionError::reset,
             "Reset error state");

    py::class_<Curiosity, std::shared_ptr<Curiosity>>(m, "Curiosity", R"pbdoc(Curiosity drive system)pbdoc")
        .def(py::init<>())
        .def("processNovelty", &Curiosity::processNovelty, py::arg("novelty"), py::arg("timeStep"),
             "Process novelty signal")
        .def("getCuriosityLevel", &Curiosity::getCuriosityLevel, py::arg("timeStep"),
             "Get curiosity level at time step")
        .def("getAverageCuriosity", &Curiosity::getAverageCuriosity,
             "Get average curiosity level")
        .def("setSensitivity", &Curiosity::setSensitivity, py::arg("sensitivity"),
             "Set curiosity sensitivity")
        .def("getSensitivity", &Curiosity::getSensitivity,
             "Get current sensitivity")
        .def("reset", &Curiosity::reset,
             "Reset curiosity system");

    py::class_<Reward, std::shared_ptr<Reward>>(m, "Reward", R"pbdoc(Reward signal system)pbdoc")
        .def(py::init<>())
        .def("processFeedback", &Reward::processFeedback, py::arg("feedback"), py::arg("timeStep"),
             "Process feedback signal")
        .def("getRewardLevel", &Reward::getRewardLevel, py::arg("timeStep"),
             "Get reward level at time step")
        .def("getAverageReward", &Reward::getAverageReward,
             "Get average reward level")
        .def("setMagnitude", &Reward::setMagnitude, py::arg("magnitude"),
             "Set reward magnitude")
        .def("getMagnitude", &Reward::getMagnitude,
             "Get current magnitude")
        .def("reset", &Reward::reset,
             "Reset reward system");

    py::class_<Novelty, std::shared_ptr<Novelty>>(m, "Novelty", R"pbdoc(Novelty detection system)pbdoc")
        .def(py::init<>())
        .def("processInput", &Novelty::processInput, py::arg("input"), py::arg("timeStep"),
             "Process input for novelty detection")
        .def("getNoveltyLevel", &Novelty::getNoveltyLevel, py::arg("timeStep"),
             "Get novelty level at time step")
        .def("getAverageNovelty", &Novelty::getAverageNovelty,
             "Get average novelty level")
        .def("setThreshold", &Novelty::setThreshold, py::arg("threshold"),
             "Set novelty threshold")
        .def("getThreshold", &Novelty::getThreshold,
             "Get current threshold")
        .def("reset", &Novelty::reset,
             "Reset novelty system");

    // === Development System ===
    
    py::class_<DevelopmentSystem, std::shared_ptr<DevelopmentSystem>>(m, "DevelopmentSystem", R"pbdoc(Developmental and evolutionary system)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("brain"), py::arg("timestep"),
             "Update developmental state")
        .def("getDevelopmentalStage", &DevelopmentSystem::getDevelopmentalStage,
             "Get current developmental stage")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             "Set developmental stage")
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress,
             "Get maturation progress")
        .def("getCriticalPeriodActive", &DevelopmentSystem::getCriticalPeriodActive,
             "Check if critical period is active")
        .def("enableCriticalPeriod", &DevelopmentSystem::enableCriticalPeriod,
             "Enable critical period")
        .def("disableCriticalPeriod", &DevelopmentSystem::disableCriticalPeriod,
             "Disable critical period")
        .def("getSynaptogenesisRate", &DevelopmentSystem::getSynaptogenesisRate,
             "Get synaptogenesis rate")
        .def("setSynaptogenesisRate", &DevelopmentSystem::setSynaptogenesisRate, py::arg("rate"),
             "Set synaptogenesis rate")
        .def("getPruningRate", &DevelopmentSystem::getPruningRate,
             "Get pruning rate")
        .def("setPruningRate", &DevelopmentSystem::setPruningRate, py::arg("rate"),
             "Set pruning rate")
        .def("reset", &DevelopmentSystem::reset,
             "Reset development system");

    // === Persistence System ===
    
    py::class_<CheckpointSystem, std::shared_ptr<CheckpointSystem>>(m, "CheckpointSystem", R"pbdoc(Persistence and checkpoint system)pbdoc")
        .def(py::init<>())
        .def("createCheckpoint", &CheckpointSystem::createCheckpoint, py::arg("name") = "",
             "Create a new checkpoint")
        .def("saveCheckpoint", &CheckpointSystem::saveCheckpoint, py::arg("name"), py::arg("filepath"),
             "Save a checkpoint to file")
        .def("loadCheckpoint", &CheckpointSystem::loadCheckpoint, py::arg("name"), py::arg("filepath"),
             "Load a checkpoint from file")
        .def("listCheckpoints", &CheckpointSystem::listCheckpoints,
             "List all checkpoints")
        .def("deleteCheckpoint", &CheckpointSystem::deleteCheckpoint, py::arg("name"),
             "Delete a checkpoint")
        .def("getCheckpointNames", &CheckpointSystem::getCheckpointNames,
             "Get list of checkpoint names")
        .def("getCheckpointCount", &CheckpointSystem::getCheckpointCount,
             "Get number of checkpoints")
        .def("clearCheckpoints", &CheckpointSystem::clearCheckpoints,
             "Clear all checkpoints")
        .def("restore", &CheckpointSystem::restore, py::arg("name"),
             "Restore to a checkpoint")
        .def("setAutoSave", &CheckpointSystem::setAutoSave, py::arg("enabled"), py::arg("interval"),
             "Enable/disable auto-save")
        .def("isAutoSaveEnabled", &CheckpointSystem::isAutoSaveEnabled,
             "Check if auto-save is enabled")
        .def("getAutoSaveInterval", &CheckpointSystem::getAutoSaveInterval,
             "Get auto-save interval");

    // === Enhanced Statistics and Utilities ===
    
    py::class_<EnhancedStatistics, std::shared_ptr<EnhancedStatistics>>(m, "EnhancedStatistics", R"pbdoc(Enhanced network and brain statistics)pbdoc")
        .def(py::init<>())
        .def("computeNetworkTopology", &EnhancedStatistics::computeNetworkTopology,
             "Compute network topology metrics")
        .def("detectCommunities", &EnhancedStatistics::detectCommunities,
             "Detect communities in network")
        .def("computeBetweennessCentrality", &EnhancedStatistics::computeBetweennessCentrality,
             "Compute betweenness centrality")
        .def("computeClusteringCoefficient", &EnhancedStatistics::computeClusteringCoefficient,
             "Compute clustering coefficient")
        .def("getNetworkDensity", &EnhancedStatistics::getNetworkDensity,
             "Get network density")
        .def("getAveragePathLength", &EnhancedStatistics::getAveragePathLength,
             "Get average path length")
        .def("getAverageShortestPath", &EnhancedStatistics::getAverageShortestPath,
             "Get average shortest path length")
        .def("getAssortativity", &EnhancedStatistics::getAssortativity,
             "Get network assortativity")
        .def("getModularity", &EnhancedStatistics::getModularity,
             "Get modularity score")
        .def("getCommunityStructure", &EnhancedStatistics::getCommunityStructure,
             "Get community structure")
        .def("getNodeCentrality", &EnhancedStatistics::getNodeCentrality, py::arg("neuronId"),
             "Get centrality of a specific node")
        .def("getAllCentrality", &EnhancedStatistics::getAllCentrality,
             "Get centrality for all nodes")
        .def("reset", &EnhancedStatistics::reset,
             "Reset statistics")
        .def("getComputationTime", &EnhancedStatistics::getComputationTime,
             "Get last computation time");

    py::class_<NetworkTopology, std::shared_ptr<NetworkTopology>>(m, "NetworkTopology", R"pbdoc(Advanced network topology analysis)pbdoc")
        .def(py::init<>())
        .def("analyze", &NetworkTopology::analyze, py::arg("brain"),
             "Analyze brain network topology")
        .def("getRegionConnections", &NetworkTopology::getRegionConnections,
             "Get connections between regions")
        .def("getDegreeDistribution", &NetworkTopology::getDegreeDistribution,
             "Get degree distribution")
        .def("getClusteringDistribution", &NetworkTopology::getClusteringDistribution,
             "Get clustering distribution")
        .def("getComponentSizes", &NetworkTopology::getComponentSizes,
             "Get connected component sizes")
        .def("getSmallestComponentSize", &NetworkTopology::getSmallestComponentSize,
             "Get smallest component size")
        .def("getLargestComponentSize", &NetworkTopology::getLargestComponentSize,
             "Get largest component size")
        .def("isConnected", &NetworkTopology::isConnected,
             "Check if network is connected")
        .def("getConnectionCount", &NetworkTopology::getConnectionCount,
             "Get total connection count")
        .def("getRegionCount", &NetworkTopology::getRegionCount,
             "Get number of regions")
        .def("getNeuronCount", &NetworkTopology::getNeuronCount,
             "Get total neuron count")
        .def("getSynapseCount", &NetworkTopology::getSynapseCount,
             "Get total synapse count")
        .def("reset", &NetworkTopology::reset,
             "Reset topology analysis");

    // === Neuron Lookup Utilities ===
    
    m.def("findNeuronById", &findNeuronById, py::arg("brain"), py::arg("neuronId"),
          "Find a neuron by its ID")
        .def("findNeuronsByType", &findNeuronsByType, py::arg("brain"), py::arg("type"),
             "Find all neurons of a specific type")
        .def("findNeuronsByRegion", &findNeuronsByRegion, py::arg("brain"), py::arg("regionId"),
             "Find all neurons in a region")
        .def("findSynapseById", &findSynapseById, py::arg("brain"), py::arg("synapseId"),
             "Find a synapse by its ID")
        .def("findConnections", &findConnections, py::arg("brain"), py::arg("fromId"), py::arg("toId"),
             "Find connections between two neurons");

    // === Enhanced Simulation Control ===
    
    py::class_<AdvancedTools, std::shared_ptr<AdvancedTools>>(m, "AdvancedTools", R"pbdoc(Advanced simulation tools)pbdoc")
        .def(py::init<>())
        .def("enableMultiThreading", &AdvancedTools::enableMultiThreading, py::arg("threads"),
             "Enable multi-threaded simulation")
        .def("disableMultiThreading", &AdvancedTools::disableMultiThreading,
             "Disable multi-threaded simulation")
        .def("isMultiThreadingEnabled", &AdvancedTools::isMultiThreadingEnabled,
             "Check if multi-threading is enabled")
        .def("getThreadCount", &AdvancedTools::getThreadCount,
             "Get number of threads")
        .def("enableEventDrivenMode", &AdvancedTools::enableEventDrivenMode,
             "Enable event-driven simulation mode")
        .def("disableEventDrivenMode", &AdvancedTools::disableEventDrivenMode,
             "Disable event-driven simulation mode")
        .def("isEventDrivenModeEnabled", &AdvancedTools::isEventDrivenModeEnabled,
             "Check if event-driven mode is enabled")
        .def("setTimeStep", &AdvancedTools::setTimeStep, py::arg("timestep"),
             "Set simulation time step")
        .def("getTimeStep", &AdvancedTools::getTimeStep,
             "Get current time step")
        .def("getFrameTime", &AdvancedTools::getFrameTime,
             "Get time since last frame")
        .def("resetFrameTime", &AdvancedTools::resetFrameTime,
             "Reset frame time counter");

    py::class_<AdvancedSimulation, std::shared_ptr<AdvancedSimulation>>(m, "AdvancedSimulation", R"pbdoc(Advanced simulation controller)pbdoc")
        .def(py::init<>())
        .def("start", &AdvancedSimulation::start,
             "Start advanced simulation")
        .def("stop", &AdvancedSimulation::stop,
             "Stop simulation")
        .def("isRunning", &AdvancedSimulation::isRunning,
             "Check if simulation is running")
        .def("pause", &AdvancedSimulation::pause,
             "Pause simulation")
        .def("resume", &AdvancedSimulation::resume,
             "Resume simulation")
        .def("step", &AdvancedSimulation::step,
             "Advance simulation by one step")
        .def("setSpeed", &AdvancedSimulation::setSpeed, py::arg("speed"),
             "Set simulation speed")
        .def("getSpeed", &AdvancedSimulation::getSpeed,
             "Get current simulation speed")
        .def("getElapsedTime", &AdvancedSimulation::getElapsedTime,
             "Get elapsed simulation time")
        .def("getFPS", &AdvancedSimulation::getFPS,
             "Get frames per second")
        .def("enableLogging", &AdvancedSimulation::enableLogging,
             "Enable logging")
        .def("disableLogging", &AdvancedSimulation::disableLogging,
             "Disable logging")
        .def("isLoggingEnabled", &AdvancedSimulation::isLoggingEnabled,
             "Check if logging is enabled")
        .def("setLogLevel", &AdvancedSimulation::setLogLevel, py::arg("level"),
             "Set log level")
        .def("getLogLevel", &AdvancedSimulation::getLogLevel,
             "Get current log level");

    // === Factory Functions for New Systems ===
    
    m.def("createWorkingMemory", []() -> std::shared_ptr<WorkingMemory> {
        return std::make_shared<WorkingMemory>();
    }, "Create a new working memory system");

    m.def("createEpisodicMemory", []() -> std::shared_ptr<EpisodicMemory> {
        return std::make_shared<EpisodicMemory>();
    }, "Create a new episodic memory system");

    m.def("createSemanticMemory", []() -> std::shared_ptr<SemanticMemory> {
        return std::make_shared<SemanticMemory>();
    }, "Create a new semantic memory system");

    m.def("createProceduralMemory", []() -> std::shared_ptr<ProceduralMemory> {
        return std::make_shared<ProceduralMemory>();
    }, "Create a new procedural memory system");

    m.def("createPredictionSystem", []() -> std::shared_ptr<PredictionSystem> {
        return std::make_shared<PredictionSystem>();
    }, "Create a new prediction system");

    m.def("createNeuralPlanner", []() -> std::shared_ptr<NeuralPlanner> {
        return std::make_shared<NeuralPlanner>();
    }, "Create a new neural planner");

    m.def("createConceptFormation", []() -> std::shared_ptr<ConceptFormation> {
        return std::make_shared<ConceptFormation>();
    }, "Create a new concept formation system");

    m.def("createSelfModel", []() -> std::shared_ptr<SelfModel> {
        return std::make_shared<SelfModel>();
    }, "Create a new self model system");

    m.def("createSocialLearning", []() -> std::shared_ptr<SocialLearning> {
        return std::make_shared<SocialLearning>();
    }, "Create a new social learning system");

    m.def("createDopamineSystem", []() -> std::shared_ptr<Dopamine> {
        return std::make_shared<Dopamine>();
    }, "Create a new dopamine system");

    m.def("createAcetylcholineSystem", []() -> std::shared_ptr<Acetylcholine> {
        return std::make_shared<Acetylcholine>();
    }, "Create a new acetylcholine system");

    m.def("createNorepinephrineSystem", []() -> std::shared_ptr<Norepinephrine> {
        return std::make_shared<Norepinephrine>();
    }, "Create a new norepinephrine system");

    m.def("createSerotoninSystem", []() -> std::shared_ptr<Serotonin> {
        return std::make_shared<Serotonin>();
    }, "Create a new serotonin system");

    m.def("createDevelopmentSystem", []() -> std::shared_ptr<DevelopmentSystem> {
        return std::make_shared<DevelopmentSystem>();
    }, "Create a new development system");

    m.def("createCheckpointSystem", []() -> std::shared_ptr<CheckpointSystem> {
        return std::make_shared<CheckpointSystem>();
    }, "Create a new checkpoint system");

    m.def("createEnhancedStatistics", []() -> std::shared_ptr<EnhancedStatistics> {
        return std::make_shared<EnhancedStatistics>();
    }, "Create a new enhanced statistics system");

    m.def("createNetworkTopology", []() -> std::shared_ptr<NetworkTopology> {
        return std::make_shared<NetworkTopology>();
    }, "Create a new network topology analyzer");

    m.def("createAdvancedTools", []() -> std::shared_ptr<AdvancedTools> {
        return std::make_shared<AdvancedTools>();
    }, "Create advanced simulation tools");

    m.def("createAdvancedSimulation", []() -> std::shared_ptr<AdvancedSimulation> {
        return std::make_shared<AdvancedSimulation>();
    }, "Create advanced simulation controller");

    // === Constants ===
    
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
