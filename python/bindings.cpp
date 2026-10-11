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
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/Reward.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        An experimental artificial developmental brain with neural systems,
        memory, prediction, cognition, and neuromodulation.
        
        Key features:
        - Real spiking neural computation (LIF neurons)
        - Integrated memory systems (working, episodic, associative)
        - Predictive coding and temporal processing
        - Cognitive functions (planning, concept formation, attention)
        - Neuromodulation (reward, curiosity, novelty)
        - Developmental stages and plasticity
        - Checkpointing and persistence
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Basic types
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

    // Enumerations
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

    // Config class
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

    // Basic sensory and motor classes
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

    // Action type enumeration
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

    // Motor command enumeration
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

    // World object classes
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

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    // Agent body state
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

    // Action result
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // Agent brain interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
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

    // Simple world
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

    // Brain class
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"), "Perform a simulation step")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"), "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"), "Inject sensory input into the brain")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"), "Inject current into a specific neuron")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"), "Inject current into all neurons of a specific type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state")
        .def("save", &Brain::save, py::arg("filepath"), "Save brain state to file")
        .def("load", &Brain::load, py::arg("filepath"), "Load brain state from file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal, "Get a region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal, "Get all regions")
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
             py::return_value_policy::reference_internal, "Get the configuration")
        .def("logStatus", &Brain::logStatus,
             "Log brain status")
        .def("__repr__", [](const Brain& b) {
            return "<Brain: neurons=" + std::to_string(b.getTotalNeuronCount()) +
                   ", regions=" + std::to_string(b.getRegionCount()) + ">";
        });

    // Memory systems
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory for transient information)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron,
             py::arg("neuron"), py::arg("activation"), "Store specific neuron activation")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             "Check if specific neuron is part of working memory")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation,
             py::arg("neuron"), "Get activation level of a specific neuron in working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             "Update working memory (maintenance and decay)")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get capacity")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"), "Set capacity")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             "Get neurons currently in working memory")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             "Is this neuron part of the winning population?")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory for experiences)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             "Store a new experience")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             "Retrieve episodes similar to a query pattern")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             "Retrieve episodes from a specific time window")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             "Retrieve episodes by location")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             "Get total number of episodes")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             "Get average reward from episodes");

    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Associative memory for pattern relationships)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             "Create association between two neural patterns")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5,
             "Retrieve patterns associated with a query pattern");

    // Prediction system
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction system for sensory prediction)pbdoc")
        .def(py::init<>())
        .def("predictNextState", &PredictionSystem::predictNextState,
             py::arg("currentPattern"), "Make prediction for next timestep")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error")
        .def("getConfidence", &PredictionSystem::getConfidence,
             "Get prediction confidence")
        .def("trainFromPattern", &PredictionSystem::trainFromPattern,
             py::arg("pattern"), "Train prediction model from neural pattern")
        .def("enable", &PredictionSystem::enable, py::arg("enable"),
             "Enable/disable prediction")
        .def("isEnabled", &PredictionSystem::isEnabled,
             "Check if prediction is enabled")
        .def("getPredictionStrength", &PredictionSystem::getPredictionStrength,
             "Get current prediction strength");

    // Cognitive systems
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth,
             py::arg("depth"), "Set number of steps to look ahead")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             "Get number of steps to look ahead")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal,
             py::arg("goal"), "Set current goal")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Has recent planning been successful?");

    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation for pattern discovery)pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             "Process an experience to form concepts")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.75f,
             "Get concept matching a pattern")
        .def("isConceptStable", &ConceptFormation::isConceptStable,
             py::arg("conceptId"), "Check if concept is stable");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Attentional selection for focus)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Is this neuron currently attended?")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
             py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
             py::arg("strength"), "Set excitation strength");

    // Neuromodulation systems
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel, "Get dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"), "Set dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get dopamine-modulated plasticity factor")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward reception")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError,
             py::arg("error"), "Signal reward prediction error");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel, "Get curiosity level")
        .def("setLevel", &Curiosity::setLevel, py::arg("level"), "Set curiosity level");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection neuromodulator)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel, "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"), "Set novelty level");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel, "Get prediction error level")
        .def("setLevel", &PredictionError::setLevel, py::arg("level"), "Set prediction error level");

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

    // Constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Helper functions
    m.def("getVersion", []() { return "NLM 0.1.0 (Phase 6 - Integrated Artificial Brain)"; },
          "Get NLM version");

    m.def("getSystemInfo", []() {
        return std::map<std::string, std::string>{
            {"status", "Phase 6 - Integrated"},
            {"architecture", "Sophisticated neural computation"},
            {"memory_systems", "Working, Episodic, Associative"},
            {"prediction", "Active predictive coding"},
            {"cognition", "Planning, concept formation, attention"},
            {"neuromodulation", "Dopamine, Curiosity, Novelty"},
            {"development", "Stage-based plasticity"}
        };
    }, "Get system information");
}

} // namespace nlm