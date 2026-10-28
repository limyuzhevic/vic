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

// Phase 6 System Headers (missing from Python bindings)
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/cognition/AttentionalSelection.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/core/Random/Random.hpp"

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

    // ======== MEMORY SYSTEMS (Phase 6) ========
    
    // NeuralWorkingMemory - Working memory using neural dynamics
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory system)pbdoc")
        .def(py::init<>())
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             "Store information in working memory as neural activity pattern")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             "Retrieve current working memory content as activity levels")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             "Get number of active memory traces")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("cap"),
             "Set working memory capacity")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity, "Get working memory capacity")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             "Set decay rate for memory traces")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate, "Get decay rate")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             "Strengthen working memory representation (rehearsal)")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             "Run competition between memory traces")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             "Get overall memory activity level")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory");

    // NeuralEpisodicMemory - Experience storage
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory system)pbdoc")
        .def(py::init<>())
        .def("storeExperience", [](NeuralEpisodicMemory& self, const std::vector<float>& state,
                                   const std::vector<float>& action, const std::vector<float>& nextState,
                                   float reward) {
            self.store(std::make_tuple(state, action, nextState, reward));
        }, py::arg("state"), py::arg("action"), py::arg("nextState"), py::arg("reward"),
            "Store an experience in episodic memory")
        .def("replayExperience", [](NeuralEpisodicMemory& self, size_t index, std::vector<float>& replayState,
                                    std::vector<float>& replayAction, std::vector<float>& replayNextState) {
            auto experience = self.replay(index);
            replayState = std::get<0>(experience);
            replayAction = std::get<1>(experience);
            replayNextState = std::get<2>(experience);
        }, py::arg("index"), py::arg("replayState"), py::arg("replayAction"), py::arg("replayNextState"),
            "Replay an experience from episodic memory")
        .def("getExperienceCount", &NeuralEpisodicMemory::getExperienceCount,
             "Get number of stored experiences")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear all experiences")
        .def("getConsolidationProgress", &NeuralEpisodicMemory::getConsolidationProgress,
             "Get consolidation progress (0.0-1.0)")
        .def("update", &NeuralEpisodicMemory::update, py::arg("dt"), "Update memory consolidation");

    // NeuralAssociativeMemory - Pattern associations
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory system)pbdoc")
        .def(py::init<>())
        .def("associate", &NeuralAssociativeMemory::associate, py::arg("pattern1"), py::arg("pattern2"), py::arg("strength") = 1.0f,
             "Create association between two patterns")
        .def("recall", &NeuralAssociativeMemory::recall, py::arg("pattern"), py::arg("numAssociates") = 5,
             "Recall associated patterns for given input")
        .def("getStrength", &NeuralAssociativeMemory::getStrength, py::arg("pattern1"), py::arg("pattern2"),
             "Get association strength between two patterns")
        .def("forget", &NeuralAssociativeMemory::forget, py::arg("pattern1"), py::arg("pattern2"),
             "Remove association between patterns")
        .def("getPatternCount", &NeuralAssociativeMemory::getPatternCount,
             "Get number of patterns in memory")
        .def("clear", &NeuralAssociativeMemory::clear, "Clear all associations")
        .def("pruneWeakAssociations", &NeuralAssociativeMemory::pruneWeakAssociations,
             "Remove weak associations below threshold");

    // ======== PREDICTION SYSTEM ======== 
    
    // PredictionSystem - Sensory prediction and error computation
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Neural prediction system)pbdoc")
        .def(py::init<>())
        .def("predict", &PredictionSystem::predict, py::arg("sensoryInput"),
             "Make prediction based on sensory input")
        .def("computeError", &PredictionSystem::computeError, py::arg("actualSensoryInput"),
             "Compute prediction error")
        .def("getPredictionConfidence", &PredictionSystem::getPredictionConfidence,
             "Get prediction confidence (0.0-1.0)")
        .def("getPredictionError", &PredictionSystem::getPredictionError,
             "Get prediction error signal")
        .def("reset", &PredictionSystem::reset, "Reset prediction system")
        .def("update", &PredictionSystem::update, py::arg("dt"), "Update prediction system");

    // ======== COGNITION SYSTEMS ======== 
    
    // NeuralPlanner - Multi-step action planning
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural action planner)pbdoc")
        .def(py::init<>())
        .def("planAction", &NeuralPlanner::planAction, py::arg("currentState"), py::arg("targetReward") = 0.5f,
             "Plan next action given current state and goal")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence, py::arg("actions"), py::arg("startState"),
             "Evaluate potential action sequence")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             "Set number of steps to look ahead")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth, "Get planning depth")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             "Set current goal state")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal, "Get current goal")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality, py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             "Update planning based on actual outcome")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             "Check if recent planning was successful")
        .def("clearCache", &NeuralPlanner::clearCache, "Clear planning cache");

    // ConceptFormation - Pattern discovery
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Concept formation system)pbdoc")
        .def(py::init<>())
        .def("discoverConcept", &ConceptFormation::discoverConcept, py::arg("pattern"),
             "Discover new concept from pattern")
        .def("retrieveConcept", &ConceptFormation::retrieveConcept, py::arg("conceptId"),
             "Retrieve stored concept")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             "Get number of learned concepts")
        .def("getConceptId", &ConceptFormation::getConceptId, py::arg("pattern"),
             "Get concept ID for pattern")
        .def("clearConcepts", &ConceptFormation::clearConcepts, "Clear all concepts")
        .def("update", &ConceptFormation::update, py::arg("dt"), "Update concept formation");

    // AttentionalSelection - Competitive selection
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attentional selection)pbdoc")
        .def(py::init<>())
        .def("processCompetition", &AttentionalSelection::processCompetition, py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing sensory inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to specific region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention (equal processing)")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get current attention focus")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             "Set excitation strength")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             "Set competition threshold")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             "Get inhibition level for neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             "Get excitation level for neuron")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if neuron is currently attended")
        .def("reset", &AttentionalSelection::reset, "Reset attentional selection")
        .def("update", &AttentionalSelection::update, py::arg("dt"), "Update attention dynamics");

    // DevelopmentSystem - Developmental changes
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Brain development system)pbdoc")
        .def(py::init<>())
        .def("update", &DevelopmentSystem::update, py::arg("timestep"), "Update development system")
        .def("getCurrentStage", &DevelopmentSystem::getCurrentStage, "Get current developmental stage")
        .def("setCurrentStage", &DevelopmentStage::value_type, py::arg("stage"), "Set developmental stage")
        .def("getPlasticityRate", &DevelopmentSystem::getPlasticityRate, "Get current plasticity rate")
        .def("getMaturationProgress", &DevelopmentSystem::getMaturationProgress, "Get maturation progress (0.0-1.0)")
        .def("getExperienceAge", &DevelopmentSystem::getExperienceAge, "Get brain experience age in simulation time")
        .def("resetToInitialState", &DevelopmentSystem::resetToInitialState, "Reset brain to initial developmental state");

    // ======== NEUROMODULATION SYSTEMS ======== 
    
    // Curiosity - Exploration motivation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity-driven exploration system)pbdoc")
        .def(py::init<>())
        .def("getCuriosityLevel", &Curiosity::getCuriosityLevel, "Get current curiosity level")
        .def("setCuriosityLevel", &Curiosity::setCuriosityLevel, py::arg("level"), "Set curiosity level")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive, "Get exploration motivation")
        .def("getNoveltySensitivity", &Curiosity::getNoveltySensitivity, "Get sensitivity to novelty")
        .def("triggerExploration", &Curiosity::triggerExploration, "Trigger exploration behavior")
        .def("update", &Curiosity::update, py::arg("dt"), "Update curiosity system");

    // Novelty - Novelty detection
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Neural novelty detection system)pbdoc")
        .def(py::init<>())
        .def("computeNovelty", &Novelty::computeNovelty, py::arg("sensoryInput"), "Compute novelty of sensory input")
        .def("getNoveltyLevel", &Novelty::getNoveltyLevel, "Get current novelty level")
        .def("getNoveltyThreshold", &Novelty::getNoveltyThreshold, "Get novelty threshold")
        .def("setNoveltyThreshold", &Novelty::setNoveltyThreshold, py::arg("threshold"), "Set novelty threshold")
        .def("getRecentActivityPattern", &Novelty::getRecentActivityPattern, "Get recent neural activity pattern")
        .def("reset", &Novelty::reset, "Reset novelty detector")
        .def("update", &Novelty::update, py::arg("dt"), "Update novelty system");

    // PredictionError - Prediction error signal
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Neural prediction error system)pbdoc")
        .def(py::init<>())
        .def("getPredictionError", &PredictionError::getPredictionError, "Get prediction error signal")
        .def("setPredictionError", &PredictionError::setPredictionError, py::arg("error"), "Set prediction error")
        .def("getErrorSensitivity", &PredictionError::getErrorSensitivity, "Get error sensitivity")
        .def("reset", &PredictionError::reset, "Reset prediction error system")
        .def("update", &PredictionError::update, py::arg("dt"), "Update prediction error system");

    // RandomGenerator - Random number generation
    py::class_<RandomGenerator>(m, "RandomGenerator", R"pbdoc(Random number generator)pbdoc")
        .def(py::init<>(), "Create random generator with default seed")
        .def(py::init<uint64_t>(), py::arg("seed"), "Create random generator with specific seed")
        .def("getSeed", &RandomGenerator::getSeed, "Get current seed")
        .def("setSeed", &RandomGenerator::setSeed, py::arg("seed"), "Set random seed")
        .def("randomFloat", &RandomGenerator::randomFloat, "Generate random float in [0,1)")
        .def("randomInt", &RandomGenerator::randomInt, py::arg("min"), py::arg("max"), "Generate random integer")
        .def("randomNormal", &RandomGenerator::randomNormal, py::arg("mean") = 0.0f, py::arg("stddev") = 1.0f, "Generate normal distribution")
        .def("randomBool", &RandomGenerator::randomBool, py::arg("probability") = 0.5f, "Generate random boolean");

    // Add Brain system access methods
    m.def("getBrainWorkingMemory", &Brain::getWorkingMemory, "Get working memory system", py::return_value_policy::reference_internal);
    m.def("getBrainEpisodicMemory", &Brain::getEpisodicMemory, "Get episodic memory system", py::return_value_policy::reference_internal);
    m.def("getBrainAssociativeMemory", &Brain::getAssociativeMemory, "Get associative memory system", py::return_value_policy::reference_internal);
    m.def("getBrainPredictionSystem", &Brain::getPredictionSystem, "Get prediction system", py::return_value_policy::reference_internal);
    m.def("getBrainPlanner", &Brain::getPlanner, "Get neural planner", py::return_value_policy::reference_internal);
    m.def("getBrainConceptFormation", &Brain::getConceptFormation, "Get concept formation system", py::return_value_policy::reference_internal);
    m.def("getBrainAttentionalSelection", &Brain::getAttention, "Get attentional selection system", py::return_value_policy::reference_internal);
    m.def("getBrainDevelopmentSystem", &Brain::getDevelopmentSystem, "Get development system", py::return_value_policy::reference_internal);
    m.def("getBrainDopamine", &Brain::getDopamine, "Get dopamine system", py::return_value_policy::reference_internal);
    m.def("getBrainCuriosity", &Brain::getCuriosity, "Get curiosity system", py::return_value_policy::reference_internal);
    m.def("getBrainNovelty", &Brain::getNovelty, "Get novelty system", py::return_value_policy::reference_internal);
    m.def("getBrainPredictionError", &Brain::getPredictionErrorSignal, "Get prediction error system", py::return_value_policy::reference_internal);
    m.def("getBrainRandomGenerator", &Brain::getRandomGenerator, "Get random generator", py::return_value_policy::reference_internal);
    m.def("getBrainPendingSpikeCount", &Brain::getPendingSpikeEventCount, "Get number of pending spike events");

    m.def("createResearchBrain", [](size_t neuronCount, float learningRate) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", learningRate, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", learningRate * 0.5f, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 2.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount") = 1000, py::arg("learningRate") = 0.001f,
         "Create a research-optimized brain for studying learning dynamics");

    m.def("createDevelopmentBrain", [](size_t neuronCount, bool enableDevelopment) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("development.enable", enableDevelopment, ConfigSource::Default);
        config->set("development.initialPlasticity", 1.0f, ConfigSource::Default);
        config->set("development.plasticDecayRate", 0.1f, ConfigSource::Default);
        config->set("development.stageChangeInterval", 100, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount") = 1000, py::arg("enableDevelopment") = true,
         "Create a brain with developmental plasticity for studying maturation");

    m.def("createPerformanceBrain", [](size_t neuronCount, bool optimizedForSpeed) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("performance.optimization_level", optimizedForSpeed ? 3 : 1, ConfigSource::Default);
        config->set("performance.use_memory_pools", true, ConfigSource::Default);
        config->set("performance.enable_vectorization", true, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount") = 1000, py::arg("optimizedForSpeed") = true,
         "Create a performance-optimized brain for speed testing");

    m.def("createSimpleBrain", [](size_t neuronCount) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", false, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", false, ConfigSource::Default);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount") = 100, "Create a simple brain with minimal configuration");

    // ======== CONFIGURATION HELPERS ======== 
    
    // Configuration presets
    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", 0.0005f, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("prediction.enabled", true, ConfigSource::Default);
        config->set("cognition.planner.enabled", true, ConfigSource::Default);
        config->set("development.enable", false, ConfigSource::Default);
        config->set("development.initialPlasticity", 0.5f, ConfigSource::Default);
        return config;
    }, "Create a research-oriented configuration");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.synapse_density", 0.05f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.learning_rate", 0.001f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("prediction.enabled", true, ConfigSource::Default);
        config->set("cognition.planner.enabled", true, ConfigSource::Default);
        config->set("development.enable", true, ConfigSource::Default);
        config->set("development.initialPlasticity", 1.0f, ConfigSource::Default);
        config->set("development.plasticDecayRate", 0.2f, ConfigSource::Default);
        config->set("development.stageChangeInterval", 50, ConfigSource::Default);
        return config;
    }, "Create a development-oriented configuration");

    m.def("createPerformanceConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.connection_probability", 0.02f, ConfigSource::Default);
        config->set("performance.optimization_level", 3, ConfigSource::Default);
        config->set("performance.use_memory_pools", true, ConfigSource::Default);
        config->set("performance.enable_vectorization", true, ConfigSource::Default);
        config->set("performance.enable_optimization_hints", true, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", false, ConfigSource::Default);
        config->set("development.enable", false, ConfigSource::Default);
        return config;
    }, "Create a performance-optimized configuration");

    m.def("createSimpleConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", false, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", false, ConfigSource::Default);
        config->set("development.enable", false, ConfigSource::Default);
        config->set("prediction.enabled", false, ConfigSource::Default);
        config->set("cognition.enabled", false, ConfigSource::Default);
        return config;
    }, "Create a simple basic configuration");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Add Brain methods for batch operations and performance monitoring
    
    // Batch operations
    m.def("receiveSensoryInputs", [](std::shared_ptr<Brain> brain, const std::vector<std::shared_ptr<SensoryInput>>& inputs,
                                   const std::vector<Timestamp>& timestamps = {}) {
        for (size_t i = 0; i < inputs.size(); ++i) {
            brain->receiveSensoryInput(*inputs[i]);
        }
    }, py::arg("inputs"), py::arg("timestamps") = std::vector<Timestamp>(),
         "Process multiple sensory inputs at once");

    // Performance monitoring and benchmarking
    m.def("benchmark", [](std::shared_ptr<Brain> brain, size_t numSteps, bool detailed) {
        struct BenchmarkResult {
            size_t stepsCompleted;
            float totalSpikes;
            float averageFiringRate;
            float excitationInhibitionRatio;
            float totalTime;
            std::map<std::string, float> subsystemStats;
        };
        
        BenchmarkResult result;
        result.stepsCompleted = numSteps;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < numSteps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
            
            result.totalSpikes += brain->getTotalSpikeCount();
            result.averageFiringRate = result.totalSpikes / (i + 1);
            result.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
            
            if (detailed && i % 100 == 0) {
                // Collect subsystem statistics
                if (auto* workingMemory = brain->getWorkingMemory()) {
                    result.subsystemStats["working_memory_activity"] = workingMemory->getMemoryActivity();
                }
                if (auto* planner = brain->getPlanner()) {
                    result.subsystemStats["planning_confidence"] = planner->getPlanningConfidence();
                }
                if (auto* curiosity = brain->getCuriosity()) {
                    result.subsystemStats["curiosity_level"] = curiosity->getCuriosityLevel();
                }
            }
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        result.totalTime = std::chrono::duration<float, std::chrono::seconds::period>(endTime - startTime).count();
        
        return result;
    }, py::arg("numSteps"), py::arg("detailed") = false,
         "Benchmark brain performance over multiple steps");

    // Memory usage monitoring
    m.def("getMemoryUsage", [](std::shared_ptr<Brain> brain) {
        struct MemoryUsage {
            size_t totalNeurons;
            size_t totalSynapses;
            size_t activeNeurons;
            size_t firingNeurons;
            size_t pendingSpikeEvents;
            float memoryUtilization;
            size_t workingMemoryCapacity;
            size_t episodicMemoryCount;
            size_t associativeMemoryPatterns;
        };
        
        MemoryUsage usage;
        usage.totalNeurons = brain->getTotalNeuronCount();
        usage.totalSynapses = brain->getTotalSynapseCount();
        usage.activeNeurons = brain->getActiveNeuronCount();
        usage.firingNeurons = brain->getFiringNeuronCount();
        usage.pendingSpikeEvents = brain->getPendingSpikeEventCount();
        
        // Calculate memory utilization as ratio of active neurons to total
        usage.memoryUtilization = usage.totalNeurons > 0 ? 
            static_cast<float>(usage.activeNeurons) / usage.totalNeurons : 0.0f;
            
        // Get memory system sizes
        if (auto* workingMemory = brain->getWorkingMemory()) {
            usage.workingMemoryCapacity = workingMemory->getCapacity();
        }
        if (auto* episodicMemory = brain->getEpisodicMemory()) {
            usage.episodicMemoryCount = episodicMemory->getExperienceCount();
        }
        if (auto* associativeMemory = brain->getAssociativeMemory()) {
            usage.associativeMemoryPatterns = associativeMemory->getPatternCount();
        }
        
        return usage;
    }, "Get detailed memory usage statistics");

    // Advanced simulation utilities
    m.def("runForSteps", [](std::shared_ptr<Brain> brain, size_t numSteps, bool logProgress) {
        for (size_t i = 0; i < numSteps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
            if (logProgress && i % 100 == 0) {
                std::cout << "Step " << i << ": " << brain->getFiringNeuronCount() 
                         << " firing neurons, " << brain->getTotalSpikeCount() 
                         << " total spikes" << std::endl;
            }
        }
    }, py::arg("numSteps"), py::arg("logProgress") = false,
         "Run simulation for specified number of steps");

    m.def("runUntil", [](std::shared_ptr<Brain> brain, std::function<bool(std::shared_ptr<Brain>)> condition, 
                        size_t maxSteps, bool logProgress) {
        for (size_t i = 0; i < maxSteps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
            if (logProgress && i % 100 == 0) {
                std::cout << "Step " << i << ": " << brain->getFiringNeuronCount() 
                         << " firing neurons, " << brain->getTotalSpikeCount() 
                         << " total spikes" << std::endl;
            }
            
            if (condition(brain)) {
                std::cout << "Condition met at step " << i << std::endl;
                break;
            }
        }
    }, py::arg("condition"), py::arg("maxSteps") = 10000, py::arg("logProgress") = false,
         "Run simulation until condition is met");

    m.def("saveCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath, 
                              bool createBackup = true) {
        bool success = brain->save(filepath);
        if (success && createBackup) {
            // Create backup filename
            size_t dotPos = filepath.find_last_of('.');
            std::string backupFile = filepath.substr(0, dotPos) + "_backup" + filepath.substr(dotPos);
            brain->save(backupFile);
        }
        return success;
    }, py::arg("filepath"), py::arg("createBackup") = true,
         "Save brain state as checkpoint, optionally creating backup");

    // Validation helpers
    m.def("validateSensoryInput", [](std::shared_ptr<Brain> brain, std::shared_ptr<SensoryInput> input) {
        // Basic validation checks
        if (!input) return false;
        
        int dimensions = input->getDimensions();
        if (dimensions <= 0 || dimensions > 10000) return false; // Sanity check
        
        // Check if brain has corresponding sensory neurons
        auto inputType = input->getType();
        bool hasSensoryNeurons = false;
        
        switch (inputType) {
            case NeuronType::Sensory:
                hasSensoryNeurons = brain->getTotalNeuronCount() > 0;
                break;
            case NeuronType::Internal:
                hasSensoryNeurons = brain->getTotalNeuronCount() > 10;
                break;
            default:
                hasSensoryNeurons = brain->getTotalNeuronCount() > 5;
        }
        
        return hasSensoryNeurons;
    }, py::arg("input"), "Validate sensory input before processing");

    // Configuration validation
    m.def("validateConfig", [](std::shared_ptr<Config> config) {
        bool isValid = true;
        std::vector<std::string> errors;
        
        // Check neuron count
        if (config->has("brain.neuron_count")) {
            int64_t neuronCount = config->getOr<int64_t>("brain.neuron_count", 100);
            if (neuronCount <= 0 || neuronCount > 1000000) {
                errors.push_back("Invalid neuron count: must be between 1 and 1,000,000");
                isValid = false;
            }
        }
        
        // Check connection probability
        if (config->has("brain.connection_probability")) {
            float connProb = config->getOr<float>("brain.connection_probability", 0.1f);
            if (connProb < 0.0f || connProb > 1.0f) {
                errors.push_back("Invalid connection probability: must be between 0.0 and 1.0");
                isValid = false;
            }
        }
        
        // Check timestep
        if (config->has("simulation_timestep")) {
            double timestep = config->getOr<double>("simulation_timestep", 0.001);
            if (timestep <= 0.0 || timestep > 1.0) {
                errors.push_back("Invalid timestep: must be between 0.001 and 1.0");
                isValid = false;
            }
        }
        
        return std::make_pair(isValid, errors);
    }, "Validate configuration and return validation results");
}
}
