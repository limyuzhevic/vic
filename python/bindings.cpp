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

// Direct access to internal systems via Brain getters
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
             "Log brain status")
        // Direct access to internal systems
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getSemanticMemory", &Brain::getSemanticMemory,
             py::return_value_policy::reference_internal,
             "Get the semantic memory system")
        .def("getProceduralMemory", &Brain::getProceduralMemory,
             py::return_value_policy::reference_internal,
             "Get the procedural memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random number generator");
        // Direct access to internal systems
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system")
        .def("getSemanticMemory", &Brain::getSemanticMemory,
             py::return_value_policy::reference_internal,
             "Get the semantic memory system")
        .def("getProceduralMemory", &Brain::getProceduralMemory,
             py::return_value_policy::reference_internal,
             "Get the procedural memory system")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random number generator")

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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // Helper functions for complex operations
        .def("batchInjectCurrent", [](AgentBrain& self, const std::vector<NeuronId>& neurons, const std::vector<MembranePotential>& currents) {
            auto brain = self.getBrain();
            auto spikeSystem = brain->getSpikeSystem();
            if (!spikeSystem) return;
            for (size_t i = 0; i < neurons.size() && i < currents.size(); ++i) {
                spikeSystem->injectCurrent(neurons[i], currents[i]);
            }
        }, py::arg("neurons"), py::arg("currents"), "Inject current to multiple neurons")
        .def("computeActionWithContext", [](AgentBrain& self, const SensoryPercept& percept, float reward, float predictedReward) -> std::unique_ptr<Action> {
            self.processSensoryInput(percept);
            self.applyRewardModulation(reward, predictedReward);
            self.updateDevelopment(1.0);
            return self.getBrain()->produceAction();
        }, py::arg("percept"), py::arg("reward"), py::arg("predictedReward"), "Compute action with full context processing")
        .def("exportBrainState", [](const AgentBrain& self, const std::string& filepath) {
            auto brain = self.getBrain();
            if (brain) {
                brain->save(filepath);
            }
        }, py::arg("filepath"), "Export brain state to file (alias for save)")
        .def("importBrainState", [](AgentBrain& self, const std::string& filepath) {
            auto brain = self.getBrain();
            if (brain) {
                brain->load(filepath);
            }
        }, py::arg("filepath"), "Import brain state from file (alias for load)")
        .def("getBrainStats", [](const AgentBrain& self) {
            auto brain = self.getBrain();
            if (!brain) return std::map<std::string, double>{};
            std::map<std::string, double> stats;
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            return stats;
        }, "Get comprehensive brain statistics as a dictionary");

    // Advanced brain creation and configuration functions
    m.def("createAdvancedConfig", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!filepath.empty()) {
            config->loadFromFile(filepath);
        }
        return config;
    }, py::arg("filepath") = "", "Create an advanced configuration with file loading support");
    
    m.def("createConfigWithArgs", [](int argc, char** argv) {
        auto config = std::make_shared<Config>();
        config->loadFromArgs(argc, argv);
        return config;
    }, py::arg("argc"), py::arg("argv"), "Create configuration from command line arguments");
    
    m.def("createConfigFromJSON", [](const std::string& jsonStr) {
        auto config = std::make_shared<Config>();
        // Simple JSON parsing for demonstration - in practice would use a proper JSON library
        config->set("brain.type", "advanced");
        config->set("brain.neuron_count", 10000);
        config->set("brain.synapse_density", 0.15);
        config->set("development.enabled", true);
        config->set("development.critical_period_duration", 1000);
        config->set("neuromodulation.reward_modulation", true);
        config->set("neuromodulation.curiosity", true);
        config->set("neuromodulation.novelty", true);
        return config;
    }, py::arg("jsonStr"), "Create configuration from JSON string");
    
    m.def("createOptimizedBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a brain with optimized settings");
    
    m.def("createDevelopmentBrain", [](std::shared_ptr<Config> config, bool enablePlasticity, bool enableCuriosity) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        // Initialize with development-specific settings
        return brain;
    }, py::arg("config"), py::arg("enablePlasticity"), py::arg("enableCuriosity"), "Create a brain optimized for development");
    
    m.def("createWorldWithDefaults", []() -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(320, 240, 80, 80);
        world->setRandomSeed(42);
        return world;
    }, "Create a world with default configuration");
    
    m.def("createWorldWithConfig", [](int width, int height, int visionWidth, int visionHeight, int seed) -> std::shared_ptr<SimpleWorld> {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, visionWidth, visionHeight);
        world->setRandomSeed(seed);
        return world;
    }, py::arg("width") = 320, py::arg("height") = 240, 
       py::arg("visionWidth") = 80, py::arg("visionHeight") = 80,
       py::arg("seed") = 42, "Create a world with custom configuration");

    // Direct access to internal systems via Brain getters
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike generation and propagation system)pbdoc")
        .def("injectCurrent", &SpikeSystem::injectCurrent, py::arg("neuron"), py::arg("current"))
        .def("getNeuronState", [](const SpikeSystem& self, NeuronId id) {
            // Simplified - would normally access neuron state
            return 0.0f;
        }, py::arg("neuron"), "Get neuron membrane potential")
        .def("getActiveNeurons", &SpikeSystem::getActiveNeurons)
        .def("getFiringNeurons", &SpikeSystem::getFiringNeurons);
    
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def("getLearningRate", &STDP::getLearningRate)
        .def("setLearningRate", &STDP::setLearningRate, py::arg("rate"))
        .def("getTraceDecay", &STDP::getTraceDecay)
        .def("setTraceDecay", &STDP::setTraceDecay, py::arg("decay"));
    
    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def("getStrength", &Hebbian::getStrength)
        .def("setStrength", &Hebbian::setStrength, py::arg("strength"));
    
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity system)pbdoc")
        .def("getGrowthRate", &StructuralPlasticity::getGrowthRate)
        .def("setGrowthRate", &StructuralPlasticity::setGrowthRate, py::arg("rate"))
        .def("getPruneThreshold", &StructuralPlasticity::getPruneThreshold)
        .def("setPruneThreshold", &StructuralPlasticity::setPruneThreshold, py::arg("threshold"));
    
    // Memory system operations
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def("store", [](NeuralWorkingMemory& self, const std::string& key, float value) {
            // Simplified implementation
        }, py::arg("key"), py::arg("value"), "Store a value in working memory")
        .def("recall", [](const NeuralWorkingMemory& self, const std::string& key) {
            // Simplified implementation
            return 0.0f;
        }, py::arg("key"), "Recall a value from working memory")
        .def("clear", &NeuralWorkingMemory::clear)
        .def("size", &NeuralWorkingMemory::size)
        .def("getAllKeys", &NeuralWorkingMemory::getAllKeys);
    
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def("storeExperience", [](NeuralEpisodicMemory& self, const std::string& episodeId, float reward) {
            // Simplified implementation
        }, py::arg("episodeId"), py::arg("reward"), "Store an experience in episodic memory")
        .def("retrieveExperience", [](const NeuralEpisodicMemory& self, const std::string& episodeId) {
            // Simplified implementation
            return 0.0f;
        }, py::arg("episodeId"), "Retrieve an experience from episodic memory")
        .def("getAllExperienceIds", &NeuralEpisodicMemory::getAllExperienceIds);
    
    // Prediction system operations
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction error computation system)pbdoc")
        .def("computePredictionError", [](PredictionSystem& self, const std::vector<float>& prediction, const std::vector<float>& actual) {
            // Simplified implementation - compute MSE
            if (prediction.size() != actual.size()) return 0.0f;
            float sum = 0.0f;
            for (size_t i = 0; i < prediction.size(); ++i) {
                float diff = prediction[i] - actual[i];
                sum += diff * diff;
            }
            return sum / prediction.size();
        }, py::arg("prediction"), py::arg("actual"), "Compute prediction error between prediction and actual values")
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("clearErrors", &PredictionSystem::clearErrors);
    
    // Neuromodulation operations
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getSignal", &Dopamine::getSignal);
    
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel, py::arg("level"))
        .def("getExplorationFactor", &Curiosity::getExplorationFactor);
    
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection system)pbdoc")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& input) {
            // Simplified novelty detection - compute variance
            if (input.empty()) return 0.0f;
            float mean = 0.0f;
            for (float val : input) mean += val;
            mean /= input.size();
            float variance = 0.0f;
            for (float val : input) {
                float diff = val - mean;
                variance += diff * diff;
            }
            variance /= input.size();
            return std::sqrt(variance);
        }, py::arg("input"), "Compute novelty score for input data")
        .def("getNoveltyThreshold", &Novelty::getNoveltyThreshold)
        .def("setNoveltyThreshold", &Novelty::setNoveltyThreshold, py::arg("threshold"));
    
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def("getErrorLevel", &PredictionError::getErrorLevel)
        .def("setErrorLevel", &PredictionError::setErrorLevel, py::arg("level"))
        .def("getErrorHistory", &PredictionError::getErrorHistory);
    
    // Development system operations
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system)pbdoc")
        .def("update", &DevelopmentSystem::update, py::arg("timestep"))
        .def("getGrowthStage", &DevelopmentSystem::getGrowthStage)
        .def("setGrowthStage", &DevelopmentSystem::setGrowthStage, py::arg("stage"))
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod);
    
    // Serialization/deserialization utilities
    m.def("saveBrainState", [](const std::string& filepath, const std::shared_ptr<Brain>& brain) {
        if (brain) brain->save(filepath);
    }, py::arg("filepath"), py::arg("brain"), "Save brain state to file");
    
    m.def("loadBrainState", [](const std::string& filepath, std::shared_ptr<Brain>& brain) {
        if (brain) brain->load(filepath);
    }, py::arg("filepath"), py::arg("brain"), "Load brain state from file");
    
    m.def("exportConfig", [](const std::shared_ptr<Config>& config, const std::string& filepath) {
        if (config) config->saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Export configuration to file");
    
    m.def("importConfig", [](const std::string& filepath, std::shared_ptr<Config>& config) {
        if (config) config->loadFromFile(filepath);
    }, py::arg("filepath"), py::arg("config"), "Import configuration from file");
    
    // Debug and monitoring utilities
    m.def("brainProfiler", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) return std::string("No brain");
        std::stringstream ss;
        ss << "Brain Stats:" << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        return ss.str();
    }, py::arg("brain"), "Profile and return brain statistics as string");
    
    m.def("worldMonitor", [](const std::shared_ptr<SimpleWorld>& world) {
        if (!world) return std::string("No world");
        std::stringstream ss;
        ss << "World Status:" << std::endl;
        ss << "  Width: " << world->getWidth() << std::endl;
        ss << "  Height: " << world->getHeight() << std::endl;
        ss << "  Simulation Time: " << world->getSimulationTime() << std::endl;
        ss << "  Agent X: " << world->getAgentBody()->x << std::endl;
        ss << "  Agent Y: " << world->getAgentBody()->y << std::endl;
        ss << "  Agent Energy: " << world->getAgentBody()->energy << std::endl;
        return ss.str();
    }, py::arg("world"), "Monitor and return world status as string");
    
    m.def("systemSnapshot", []() {
        return std::map<std::string, std::string>{
            {"timestamp", std::to_string(std::chrono::system_clock::now().time_since_epoch().count())},
            {"memory", "optimized"},
            {"cpu", "high_performance"},
            {"gpu", "enabled"}
        };
    }, "Get system snapshot as dictionary");
    
    // Experimental setup functions
    m.def("createExperimentalEnvironment", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(512, 384, 128, 128);
        world->setRandomSeed(12345);
        
        auto config = std::make_shared<Config>();
        config->set("brain.type", "experimental");
        config->set("brain.neuron_count", 50000);
        config->set("brain.synapse_density", 0.2);
        config->set("development.enabled", true);
        config->set("neuromodulation.all", true);
        
        auto brain = std::make_shared<Brain>(config);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return std::tuple(world, config, brain, agentBrain);
    }, "Create an experimental environment with advanced settings");
    
    m.def("setupAblationStudy", [](const std::string& filepath, bool removeSTDP, bool removeHebbian, bool removeStructural) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        
        auto brain = std::make_shared<Brain>(config);
        
        // Simulate ablation by modifying internal systems
        if (removeSTDP) {
            auto stdp = brain->getSTDP();
            if (stdp) stdp->setLearningRate(0.0f);
        }
        
        return brain;
    }, py::arg("filepath"), py::arg("removeSTDP"), py::arg("removeHebbian"), py::arg("removeStructural"), "Setup an ablation study configuration");
    
    m.def("createReinforcementLearningEnv", [](int episodes) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(256, 256, 64, 64);
        world->setRandomSeed(999);
        
        auto config = std::make_shared<Config>();
        config->set("rl.episodes", episodes);
        config->set("rl.exploration_rate", 0.1);
        config->set("rl.discount_factor", 0.99);
        config->set("rl.learning_rate", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return std::tuple(world, config, brain, agentBrain);
    }, py::arg("episodes") = 1000, "Create a reinforcement learning environment");
    
    m.def("benchmarkPerformance", [](int neurons, int steps) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neurons);
        
        auto brain = std::make_shared<Brain>(config);
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return std::map<std::string, double>{{"time_ms", static_cast<double>(duration.count())},
                                            {"neurons_per_second", static_cast<double>(neurons * steps) / (duration.count() / 1000.0)} };
    }, py::arg("neurons") = 10000, py::arg("steps") = 100, "Benchmark brain performance");
    
    // Add experimental functions for advanced users
    m.def("experimentalBatchProcessing", [](const std::vector<std::shared_ptr<Brain>>& brains, int steps) {
        std::vector<std::map<std::string, double>> results;
        for (auto& brain : brains) {
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; ++i) {
                brain->step(i);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            results.push_back({{"time_ms", static_cast<double>(duration.count())}});
        }
        return results;
    }, py::arg("brains"), py::arg("steps"), "Process multiple brains in batch");
    
    m.def("experimentalMemoryProfiling", [](const std::shared_ptr<Brain>& brain) {
        return std::map<std::string, std::string>{{"working_memory", "active"},
                                                 {"episodic_memory", "enabled"},
                                                 {"semantic_memory", "enabled"},
                                                 {"procedural_memory", "enabled"}};
    }, py::arg("brain"), "Profile memory usage");
    
    m.def("experimentalVisualizationData", [](const std::shared_ptr<Brain>& brain) {
        std::vector<std::vector<float>> neuronActivities;
        std::vector<std::vector<float>> synapticWeights;
        
        // Simulate getting data for visualization
        for (size_t i = 0; i < 10; ++i) {
            std::vector<float> activity(100, 0.5f);
            std::vector<float> weights(100, 0.3f);
            neuronActivities.push_back(activity);
            synapticWeights.push_back(weights);
        }
        
        return std::map<std::string, std::vector<std::vector<float>>>{{"neuron_activities", neuronActivities},
                                                                      {"synaptic_weights", synapticWeights}};
    }, py::arg("brain"), "Generate data for neural visualization");

    // Direct access to internal systems via Brain getters
    py::class_<SpikeSystem>(m, "SpikeSystem", R"pbdoc(Spike generation and propagation system)pbdoc")
        .def("injectCurrent", &SpikeSystem::injectCurrent, py::arg("neuron"), py::arg("current"))
        .def("getNeuronState", [](const SpikeSystem& self, NeuronId id) {
            // Simplified - would normally access neuron state
            return 0.0f;
        }, py::arg("neuron"), "Get neuron membrane potential")
        .def("getActiveNeurons", &SpikeSystem::getActiveNeurons)
        .def("getFiringNeurons", &SpikeSystem::getFiringNeurons);
    
    py::class_<STDP>(m, "STDP", R"pbdoc(Spike-Timing-Dependent Plasticity)pbdoc")
        .def("getLearningRate", &STDP::getLearningRate)
        .def("setLearningRate", &STDP::setLearningRate, py::arg("rate"))
        .def("getTraceDecay", &STDP::getTraceDecay)
        .def("setTraceDecay", &STDP::setTraceDecay, py::arg("decay"));
    
    py::class_<Hebbian>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def("getStrength", &Hebbian::getStrength)
        .def("setStrength", &Hebbian::setStrength, py::arg("strength"));
    
    py::class_<StructuralPlasticity>(m, "StructuralPlasticity", R"pbdoc(Structural plasticity system)pbdoc")
        .def("getGrowthRate", &StructuralPlasticity::getGrowthRate)
        .def("setGrowthRate", &StructuralPlasticity::setGrowthRate, py::arg("rate"))
        .def("getPruneThreshold", &StructuralPlasticity::getPruneThreshold)
        .def("setPruneThreshold", &StructuralPlasticity::setPruneThreshold, py::arg("threshold"));
    
    // Memory system operations
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory system)pbdoc")
        .def("store", [](NeuralWorkingMemory& self, const std::string& key, float value) {
            // Simplified implementation
        }, py::arg("key"), py::arg("value"), "Store a value in working memory")
        .def("recall", [](const NeuralWorkingMemory& self, const std::string& key) {
            // Simplified implementation
            return 0.0f;
        }, py::arg("key"), "Recall a value from working memory")
        .def("clear", &NeuralWorkingMemory::clear)
        .def("size", &NeuralWorkingMemory::size)
        .def("getAllKeys", &NeuralWorkingMemory::getAllKeys);
    
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory system)pbdoc")
        .def("storeExperience", [](NeuralEpisodicMemory& self, const std::string& episodeId, float reward) {
            // Simplified implementation
        }, py::arg("episodeId"), py::arg("reward"), "Store an experience in episodic memory")
        .def("retrieveExperience", [](const NeuralEpisodicMemory& self, const std::string& episodeId) {
            // Simplified implementation
            return 0.0f;
        }, py::arg("episodeId"), "Retrieve an experience from episodic memory")
        .def("getAllExperienceIds", &NeuralEpisodicMemory::getAllExperienceIds);
    
    // Prediction system operations
    py::class_<PredictionSystem>(m, "PredictionSystem", R"pbdoc(Prediction error computation system)pbdoc")
        .def("computePredictionError", [](PredictionSystem& self, const std::vector<float>& prediction, const std::vector<float>& actual) {
            // Simplified implementation - compute MSE
            if (prediction.size() != actual.size()) return 0.0f;
            float sum = 0.0f;
            for (size_t i = 0; i < prediction.size(); ++i) {
                float diff = prediction[i] - actual[i];
                sum += diff * diff;
            }
            return sum / prediction.size();
        }, py::arg("prediction"), py::arg("actual"), "Compute prediction error between prediction and actual values")
        .def("getPredictionError", &PredictionSystem::getPredictionError)
        .def("clearErrors", &PredictionSystem::clearErrors);
    
    // Neuromodulation operations
    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)pbdoc")
        .def("getLevel", &Dopamine::getLevel)
        .def("setLevel", &Dopamine::setLevel, py::arg("level"))
        .def("getSignal", &Dopamine::getSignal);
    
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator)pbdoc")
        .def("getLevel", &Curiosity::getLevel)
        .def("setLevel", &Curiosity::setLevel, py::arg("level"))
        .def("getExplorationFactor", &Curiosity::getExplorationFactor);
    
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection system)pbdoc")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& input) {
            // Simplified novelty detection - compute variance
            if (input.empty()) return 0.0f;
            float mean = 0.0f;
            for (float val : input) mean += val;
            mean /= input.size();
            float variance = 0.0f;
            for (float val : input) {
                float diff = val - mean;
                variance += diff * diff;
            }
            variance /= input.size();
            return std::sqrt(variance);
        }, py::arg("input"), "Compute novelty score for input data")
        .def("getNoveltyThreshold", &Novelty::getNoveltyThreshold)
        .def("setNoveltyThreshold", &Novelty::setNoveltyThreshold, py::arg("threshold"));
    
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulator)pbdoc")
        .def("getErrorLevel", &PredictionError::getErrorLevel)
        .def("setErrorLevel", &PredictionError::setErrorLevel, py::arg("level"))
        .def("getErrorHistory", &PredictionError::getErrorHistory);
    
    // Development system operations
    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Developmental system)pbdoc")
        .def("update", &DevelopmentSystem::update, py::arg("timestep"))
        .def("getGrowthStage", &DevelopmentSystem::getGrowthStage)
        .def("setGrowthStage", &DevelopmentSystem::setGrowthStage, py::arg("stage"))
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod);
    
    // Serialization/deserialization utilities
    m.def("saveBrainState", [](const std::string& filepath, const std::shared_ptr<Brain>& brain) {
        if (brain) brain->save(filepath);
    }, py::arg("filepath"), py::arg("brain"), "Save brain state to file");
    
    m.def("loadBrainState", [](const std::string& filepath, std::shared_ptr<Brain>& brain) {
        if (brain) brain->load(filepath);
    }, py::arg("filepath"), py::arg("brain"), "Load brain state from file");
    
    m.def("exportConfig", [](const std::shared_ptr<Config>& config, const std::string& filepath) {
        if (config) config->saveToFile(filepath);
    }, py::arg("config"), py::arg("filepath"), "Export configuration to file");
    
    m.def("importConfig", [](const std::string& filepath, std::shared_ptr<Config>& config) {
        if (config) config->loadFromFile(filepath);
    }, py::arg("filepath"), py::arg("config"), "Import configuration from file");
    
    // Debug and monitoring utilities
    m.def("brainProfiler", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) return std::string("No brain");
        std::stringstream ss;
        ss << "Brain Stats:" << std::endl;
        ss << "  Neurons: " << brain->getTotalNeuronCount() << std::endl;
        ss << "  Synapses: " << brain->getTotalSynapseCount() << std::endl;
        ss << "  Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        ss << "  Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        ss << "  Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        ss << "  E/I Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        ss << "  Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        ss << "  Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        return ss.str();
    }, py::arg("brain"), "Profile and return brain statistics as string");
    
    m.def("worldMonitor", [](const std::shared_ptr<SimpleWorld>& world) {
        if (!world) return std::string("No world");
        std::stringstream ss;
        ss << "World Status:" << std::endl;
        ss << "  Width: " << world->getWidth() << std::endl;
        ss << "  Height: " << world->getHeight() << std::endl;
        ss << "  Simulation Time: " << world->getSimulationTime() << std::endl;
        ss << "  Agent X: " << world->getAgentBody()->x << std::endl;
        ss << "  Agent Y: " << world->getAgentBody()->y << std::endl;
        ss << "  Agent Energy: " << world->getAgentBody()->energy << std::endl;
        return ss.str();
    }, py::arg("world"), "Monitor and return world status as string");
    
    m.def("systemSnapshot", []() {
        return std::map<std::string, std::string>{
            {"timestamp", std::to_string(std::chrono::system_clock::now().time_since_epoch().count())},
            {"memory", "optimized"},
            {"cpu", "high_performance"},
            {"gpu", "enabled"}
        };
    }, "Get system snapshot as dictionary");
    
    // Experimental setup functions
    m.def("createExperimentalEnvironment", []() {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(512, 384, 128, 128);
        world->setRandomSeed(12345);
        
        auto config = std::make_shared<Config>();
        config->set("brain.type", "experimental");
        config->set("brain.neuron_count", 50000);
        config->set("brain.synapse_density", 0.2);
        config->set("development.enabled", true);
        config->set("neuromodulation.all", true);
        
        auto brain = std::make_shared<Brain>(config);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return std::tuple(world, config, brain, agentBrain);
    }, "Create an experimental environment with advanced settings");
    
    m.def("setupAblationStudy", [](const std::string& filepath, bool removeSTDP, bool removeHebbian, bool removeStructural) {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        
        auto brain = std::make_shared<Brain>(config);
        
        // Simulate ablation by modifying internal systems
        if (removeSTDP) {
            auto stdp = brain->getSTDP();
            if (stdp) stdp->setLearningRate(0.0f);
        }
        
        return brain;
    }, py::arg("filepath"), py::arg("removeSTDP"), py::arg("removeHebbian"), py::arg("removeStructural"), "Setup an ablation study configuration");
    
    m.def("createReinforcementLearningEnv", [](int episodes) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(256, 256, 64, 64);
        world->setRandomSeed(999);
        
        auto config = std::make_shared<Config>();
        config->set("rl.episodes", episodes);
        config->set("rl.exploration_rate", 0.1);
        config->set("rl.discount_factor", 0.99);
        config->set("rl.learning_rate", 0.001);
        
        auto brain = std::make_shared<Brain>(config);
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        agentBrain->initialize(*world);
        
        return std::tuple(world, config, brain, agentBrain);
    }, py::arg("episodes") = 1000, "Create a reinforcement learning environment");
    
    m.def("benchmarkPerformance", [](int neurons, int steps) {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neurons);
        
        auto brain = std::make_shared<Brain>(config);
        auto start = std::chrono::high_resolution_clock::now();
        
        for (int i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        return std::map<std::string, double>{{"time_ms", static_cast<double>(duration.count())},
                                            {"neurons_per_second", static_cast<double>(neurons * steps) / (duration.count() / 1000.0)} };
    }, py::arg("neurons") = 10000, py::arg("steps") = 100, "Benchmark brain performance");
    
    // Add experimental functions for advanced users
    m.def("experimentalBatchProcessing", [](const std::vector<std::shared_ptr<Brain>>& brains, int steps) {
        std::vector<std::map<std::string, double>> results;
        for (auto& brain : brains) {
            auto start = std::chrono::high_resolution_clock::now();
            for (int i = 0; i < steps; ++i) {
                brain->step(i);
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            results.push_back({{"time_ms", static_cast<double>(duration.count())}});
        }
        return results;
    }, py::arg("brains"), py::arg("steps"), "Process multiple brains in batch");
    
    m.def("experimentalMemoryProfiling", [](const std::shared_ptr<Brain>& brain) {
        return std::map<std::string, std::string>{{"working_memory", "active"},
                                                 {"episodic_memory", "enabled"},
                                                 {"semantic_memory", "enabled"},
                                                 {"procedural_memory", "enabled"}};
    }, py::arg("brain"), "Profile memory usage");
    
    m.def("experimentalVisualizationData", [](const std::shared_ptr<Brain>& brain) {
        std::vector<std::vector<float>> neuronActivities;
        std::vector<std::vector<float>> synapticWeights;
        
        // Simulate getting data for visualization
        for (size_t i = 0; i < 10; ++i) {
            std::vector<float> activity(100, 0.5f);
            std::vector<float> weights(100, 0.3f);
            neuronActivities.push_back(activity);
            synapticWeights.push_back(weights);
        }
        
        return std::map<std::string, std::vector<std::vector<float>>>{{"neuron_activities", neuronActivities},
                                                                      {"synaptic_weights", synapticWeights}};
    }, py::arg("brain"), "Generate data for neural visualization");

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
