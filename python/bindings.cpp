#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <limits>
#include <chrono>

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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs")
        .def("getAllRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add inter-region connection")
        .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove inter-region connection")
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
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled)
        // === BRAIN HELPER METHODS ===
        .def("get_total_neuron_count", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions")
        .def("get_total_synapse_count", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions")
        .def("get_active_neuron_count", &Brain::getActiveNeuronCount,
             "Get count of active neurons")
        .def("get_firing_neuron_count", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons")
        .def("get_average_firing_rate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("get_excitation_inhibition_ratio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("get_total_spike_count", &Brain::getTotalSpikeCount,
             "Get total spike count")
        .def("get_pending_spike_event_count", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
        .def("get_region_ids", &Brain::getRegionIds,
             "Get all region IDs")
        .def("get_all_regions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions")
        .def("get_working_memory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory")
        .def("get_episodic_memory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory")
        .def("get_associative_memory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory")
        .def("get_prediction_system", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        .def("get_planner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("get_concept_formation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation")
        .def("get_attention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection")
        .def("get_development_system", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        .def("get_dopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("get_curiosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("get_novelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("get_prediction_error_signal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        .def("get_random_generator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random generator")
        .def("get_stdp", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get STDP plasticity system")
        .def("get_hebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system")
        .def("get_structural_plasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system")
        .def("add_inter_region_connection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add inter-region connection")
        .def("remove_inter_region_connection", &Brain::removeInterRegionConnection,
             py::arg("source"), py::arg("target"),
             "Remove inter-region connection")
        .def("set_developmental_stage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage")
        .def("get_config", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration")
        .def("log_status", &Brain::logStatus,
             "Log brain status");

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

    // === NEURAL REGION ===
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(Neural brain region containing populations and local connectivity)pbdoc")
        .def(py::init<RegionId>(), py::arg("id"))
        .def(py::init<RegionId, std::string>(), py::arg("id"), py::arg("name"))
        .def("get_id", &NeuralRegion::getId, "Get region ID")
        .def("get_name", &NeuralRegion::getName, "Get region name")
        .def("set_name", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("get_population_count", &NeuralRegion::getPopulationCount,
             "Get number of populations in region")
        .def("get_total_neuron_count", &NeuralRegion::getTotalNeuronCount,
             "Get total neuron count in region")
        .def("get_active_neuron_count", &NeuralRegion::getActiveNeuronCount,
             "Get count of active neurons in region")
        .def("get_firing_neuron_count", &NeuralRegion::getFiringNeuronCount,
             "Get count of firing neurons in region")
        .def("get_average_firing_rate", &NeuralRegion::getAverageFiringRate,
             "Get average firing rate in region")
        .def("get_average_synaptic_weight", &NeuralRegion::getAverageSynapticWeight,
             "Get average synaptic weight in region")
        .def("get_synaptic_density", &NeuralRegion::getSynapticDensity,
             "Get synaptic density in region")
        .def("get_synapse_count", &NeuralRegion::getSynapseCount,
             "Get number of synapses in region")
        .def("get_all_populations", &NeuralRegion::getAllPopulations,
             "Get all populations in region")
        .def("get_population", &NeuralRegion::getPopulation,
             py::return_value_policy::reference_internal,
             py::arg("id"), "Get population by ID")
        .def("add_population", &NeuralRegion::addPopulation,
             py::arg("size"), py::arg("type") = NeuronType::Internal,
             "Add population to region")
        .def("add_synapse", &NeuralRegion::addSynapse,
             py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add synapse between neurons in region")
        .def("get_synapse", &NeuralRegion::getSynapse,
             py::return_value_policy::reference_internal,
             py::arg("id"), "Get synapse by ID")
        .def("get_synapses_from", &NeuralRegion::getSynapsesFrom,
             py::arg("neuron"), "Get synapses originating from neuron")
        .def("get_synapses_to", &NeuralRegion::getSynapsesTo,
             py::arg("neuron"), "Get synapses terminating at neuron")
        .def("get_all_neurons", &NeuralRegion::getAllNeurons,
             "Get all neurons in region")
        .def("step", &NeuralRegion::step,
             py::arg("currentTime"), "Step region dynamics")
        .def("reset", &NeuralRegion::reset,
             "Reset all neurons and synapses in region")
        .def("initialize_random_connectivity", &NeuralRegion::initializeRandomConnectivity,
             py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity within region");

    // === NEURAL POPULATION ===
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(Neural population containing multiple neurons)pbdoc")
        .def(py::init<PopulationId, size_t>(), py::arg("id"), py::arg("size"))
        .def("get_id", &NeuralPopulation::getId, "Get population ID")
        .def("get_size", &NeuralPopulation::getSize, "Get population size")
        .def("is_empty", &NeuralPopulation::isEmpty, "Check if population is empty")
        .def("get_neuron", &NeuralPopulation::getNeuron,
             py::return_value_policy::reference_internal,
             py::arg("index"), "Get neuron by index")
        .def("add_neuron", &NeuralPopulation::addNeuron,
             py::arg("neuron"), "Add neuron to population")
        .def("get_neuron_type", &NeuralPopulation::getNeuronType,
             "Get neuron type for population")
        .def("set_neuron_type", &NeuralPopulation::setNeuronType,
             py::arg("type"), "Set neuron type for population")
        .def("get_active_count", &NeuralPopulation::getActiveCount,
             "Get count of active neurons")
        .def("get_firing_count", &NeuralPopulation::getFiringCount,
             "Get count of firing neurons")
        .def("get_average_firing_rate", &NeuralPopulation::getAverageFiringRate,
             "Get average firing rate")
        .def("get_activity_level", &NeuralPopulation::getActivityLevel,
             "Get activity level (fraction of active neurons)")
        .def("initialize_random", &NeuralPopulation::initializeRandom,
             py::arg("rng"), "Initialize all neurons randomly")
        .def("step", &NeuralPopulation::step,
             py::arg("currentTime"), "Step all neurons")
        .def("reset", &NeuralPopulation::reset,
             "Reset all neurons")
        .def("get_neuron_ids", &NeuralPopulation::getNeuronIds,
             "Get neuron IDs in this population")
        .def("record_activity", &NeuralPopulation::recordActivity,
             "Record current state for statistics")
        .def("get_mean_membrane_potential", &NeuralPopulation::getMeanMembranePotential,
             "Get mean membrane potential")
        .def("get_variance_membrane_potential", &NeuralPopulation::getVarianceMembranePotential,
             "Get variance of membrane potentials");

    // === SYNAPSES ===
    py::class_<Synapse>(m, "Synapse", R"pbdoc(Synaptic connection between neurons)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("get_id", &Synapse::getId, "Get synapse ID")
        .def("get_source_neuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("get_destination_neuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("get_weight", &Synapse::getWeight, "Get synaptic weight")
        .def("set_weight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("add_to_weight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        .def("get_delay", &Synapse::getDelay, "Get synaptic delay")
        .def("set_delay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("get_type", &Synapse::getType, "Get synapse type")
        .def("set_type", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("is_excitatory", &Synapse::isExcitatory, "Check if synapse is excitatory")
        .def("is_inhibitory", &Synapse::isInhibitory, "Check if synapse is inhibitory")
        .def("record_pre_spike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record pre-synaptic spike")
        .def("record_post_spike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record post-synaptic spike")
        .def("get_pre_spike_history", &Synapse::getPreSpikeHistory, "Get pre-synaptic spike history")
        .def("get_post_spike_history", &Synapse::getPostSpikeHistory, "Get post-synaptic spike history")
        .def("clear_history", &Synapse::clearHistory, "Clear spike history")
        .def("get_plasticity_flags", &Synapse::getPlasticityFlags, "Get plasticity flags")
        .def("enable_plasticity", &Synapse::enablePlasticity,
             py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity rules")
        .def("get_eligibility_trace", &Synapse::getEligibilityTrace, "Get eligibility trace")
        .def("set_eligibility_trace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("decay_eligibility_trace", &Synapse::decayEligibilityTrace,
             py::arg("decayRate"), "Decay eligibility trace")
        .def("get_efficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("set_efficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Step synapse dynamics")
        .def("reset", &Synapse::reset, "Reset synapse state")
        .def("initialize_random", &Synapse::initializeRandom, py::arg("rng"), "Initialize synapse randomly");

    // === NEURONS ===
    py::class_<Neuron>(m, "Neuron", R"pbdoc(LIF neuron implementing integrate-and-fire dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("get_id", &Neuron::getId, "Get neuron ID")
        .def("get_type", &Neuron::getType, "Get neuron type")
        .def("set_type", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("get_state", &Neuron::getState, py::return_value_policy::reference_internal, "Get neuron state")
        .def("get_membrane_potential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("set_membrane_potential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("add_to_membrane_potential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("get_threshold", &Neuron::getThreshold, "Get firing threshold")
        .def("set_threshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("is_firing", &Neuron::isFiring, "Check if neuron is firing")
        .def("is_refractory", &Neuron::isRefractory, "Check if neuron is refractory")
        .def("set_firing_state", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("set_refractory_period", &Neuron::setRefractoryPeriod, py::arg("steps"), "Set refractory period")
        .def("decrement_refractory", &Neuron::decrementRefractory, "Decrement refractory counter")
        .def("get_firing_rate", &Neuron::getFiringRate, "Get firing rate")
        .def("set_firing_rate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate")
        .def("set_leak_conductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance")
        .def("get_leak_conductance", &Neuron::getLeakConductance, "Get leak conductance")
        .def("get_refractory_period", &Neuron::getRefractoryPeriod, "Get refractory period")
        .def("set_resting_potential", &Neuron::setRestingPotential, py::arg("potential"), "Set resting potential")
        .def("get_resting_potential", &Neuron::getRestingPotential, "Get resting potential")
        .def("set_reset_potential", &Neuron::setResetPotential, py::arg("potential"), "Set reset potential")
        .def("get_reset_potential", &Neuron::getResetPotential, "Get reset potential")
        .def("check_threshold", &Neuron::checkThreshold, "Check if neuron reaches threshold")
        .def("get_last_spike_time", &Neuron::getLastSpikeTime, "Get last spike time")
        .def("step_lif", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "Step LIF dynamics")
        .def("receive_excitatory_input", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input")
        .def("receive_inhibitory_input", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input")
        .def("receive_modulatory_input", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("inject_current", &Neuron::injectCurrent, py::arg("current"), "Inject external current")
        .def("get_total_current", &Neuron::getTotalCurrent, "Get total injected current")
        .def("clear_total_current", &Neuron::clearTotalCurrent, "Clear injected current")
        .def("record_spike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike event")
        .def("get_spike_history", &Neuron::getSpikeHistory, "Get spike history")
        .def("clear_spike_history", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("add_incoming_synapse", &Neuron::addIncomingSynapse, py::arg("handle"), "Add incoming synapse")
        .def("add_outgoing_synapse", &Neuron::addOutgoingSynapse, py::arg("handle"), "Add outgoing synapse")
        .def("get_incoming_synapses", &Neuron::getIncomingSynapses, "Get incoming synapses")
        .def("get_outgoing_synapses", &Neuron::getOutgoingSynapses, "Get outgoing synapses")
        .def("get_plasticity_flags", &Neuron::getPlasticityFlags, py::return_value_policy::reference_internal, "Get plasticity flags")
        .def("enable_plasticity", &Neuron::enablePlasticity,
             py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"),
             "Enable plasticity rules")
        .def("set_region_id", &Neuron::setRegionId, py::arg("region"), "Set region ID")
        .def("get_region_id", &Neuron::getRegionId, "Get region ID")
        .def("set_population_id", &Neuron::setPopulationId, py::arg("population"), "Set population ID")
        .def("get_population_id", &Neuron::getPopulationId, "Get population ID")
        .def("step", &Neuron::step, py::arg("currentTime"), "Step neuron dynamics")
        .def("reset", &Neuron::reset, "Reset neuron state")
        .def("initialize_random", &Neuron::initializeRandom, py::arg("rng"), "Initialize neuron randomly");

    // === PYTHONIC HELPER FUNCTIONS ===
    
    m.def("validate_brain_config", [](std::shared_ptr<Config> config) {
        // Basic validation of brain configuration
        bool valid = true;
        std::vector<std::string> errors;
        
        if (!config->has("neuron_count")) {
            errors.push_back("Missing neuron_count");
            valid = false;
        } else {
            int64_t neuronCount = config->getOr<int64_t>("neuron_count", 100);
            if (neuronCount <= 0) {
                errors.push_back("neuron_count must be positive");
                valid = false;
            }
        }
        
        if (!config->has("region_count")) {
            errors.push_back("Missing region_count");
            valid = false;
        }
        
        return py::make_tuple(valid, errors);
    }, py::arg("config"), "Validate brain configuration");
    
    m.def("optimize_connectivity", [](std::shared_ptr<NeuralRegion> region,
                                       float targetDensity = 0.15f) {
        // Auto-optimize connectivity for the region
        if (!region) return 0;
        
        size_t totalNeurons = region->getTotalNeuronCount();
        size_t currentSynapses = region->getSynapseCount();
        size_t possibleConnections = totalNeurons * (totalNeurons - 1) / 2;
        float currentDensity = possibleConnections > 0 ? 
            static_cast<float>(currentSynapses) / possibleConnections : 0.0f;
        
        return std::abs(currentDensity - targetDensity);
    }, py::arg("region"), py::arg("targetDensity") = 0.15f,
       "Calculate connectivity optimization needed");
    
    m.def("batch_populate_region", [](std::shared_ptr<NeuralRegion> region,
                                      size_t neuronsPerPopulation,
                                      size_t numPopulations) {
        // Batch populate a region with neurons
        if (!region) return std::vector<PopulationId>();
        
        std::vector<PopulationId> populationIds;
        for (size_t i = 0; i < numPopulations; ++i) {
            PopulationId popId = region->addPopulation(neuronsPerPopulation);
            populationIds.push_back(popId);
        }
        
        return populationIds;
    }, py::arg("region"), py::arg("neuronsPerPopulation"), py::arg("numPopulations"),
       "Batch populate region with neurons");
    
    m.def("region_statistics_summary", [](const std::vector<std::shared_ptr<NeuralRegion>>& regions) {
        // Get comprehensive statistics across multiple regions
        std::map<std::string, double> stats;
        
        size_t totalNeurons = 0;
        size_t totalSynapses = 0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        
        for (const auto& region : regions) {
            totalNeurons += region->getTotalNeuronCount();
            totalSynapses += region->getSynapseCount();
            activeNeurons += region->getActiveNeuronCount();
            firingNeurons += region->getFiringNeuronCount();
        }
        
        stats["total_neurons"] = totalNeurons;
        stats["total_synapses"] = totalSynapses;
        stats["active_neurons"] = activeNeurons;
        stats["firing_neurons"] = firingNeurons;
        stats["excitation_inhibition_ratio"] = 
            activeNeurons > 0 ? static_cast<double>(firingNeurons) / activeNeurons : 0.0;
        
        return stats;
    }, py::arg("regions"), "Get comprehensive statistics across regions");
    
    m.def("normalize_synapse_weights", [](std::shared_ptr<NeuralRegion> region,
                                          float minWeight = -1.0f,
                                          float maxWeight = 1.0f) {
        // Normalize synapse weights in a region to specified range
        if (!region) return 0;
        
        auto& synapses = region->getSynapses();
        if (synapses.empty()) return 0;
        
        float minCurrent = std::numeric_limits<float>::max();
        float maxCurrent = std::numeric_limits<float>::lowest();
        
        for (const auto& syn : synapses) {
            float weight = syn->getWeight();
            minCurrent = std::min(minCurrent, weight);
            maxCurrent = std::max(maxCurrent, weight);
        }
        
        if (maxCurrent == minCurrent) return 0;
        
        for (auto& syn : synapses) {
            float normalized = minWeight + (syn->getWeight() - minCurrent) * 
                              (maxWeight - minWeight) / (maxCurrent - minCurrent);
            syn->setWeight(normalized);
        }
        
        return static_cast<int>(synapses.size());
    }, py::arg("region"), py::arg("minWeight") = -1.0f, py::arg("maxWeight") = 1.0f,
       "Normalize synapse weights to specified range");
    
    m.def("export_neurons_to_json", [](std::shared_ptr<NeuralRegion> region) {
        // Export neuron data as JSON string (simplified)
        if (!region) return std::string("{}");
        
        std::string json = "{\"region_id\": " + std::to_string(region->getId().value) +
                         ", \"neuron_count\": " + std::to_string(region->getTotalNeuronCount()) +
                         ", \"populations\": []}"; // Simplified - just metadata
        return json;
    }, py::arg("region"), "Export region neuron data as JSON");
    
    m.def("simulate_with_logging", [](std::shared_ptr<Brain> brain,
                                      size_t steps,
                                      bool verbose = false) {
        // Run simulation with optional logging
        if (!brain) return false;
        
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            if (verbose && i % 100 == 0) {
                brain->logStatus();
            }
        }
        
        return true;
    }, py::arg("brain"), py::arg("steps"), py::arg("verbose") = false,
       "Run simulation with optional verbose logging");
    
    m.def("benchmark_brain_performance", [](std::shared_ptr<Brain> brain,
                                            size_t steps = 1000) {
        // Benchmark brain performance
        if (!brain) return std::make_tuple(0.0f, 0, 0.0f);
        
        auto start = std::chrono::high_resolution_clock::now();
        
        brain->reset();
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        
        double timePerStep = duration.count() / static_cast<double>(steps);
        size_t spikes = brain->getTotalSpikeCount();
        double spikesPerMicrosecond = spikes / duration.count();
        
        return std::make_tuple(static_cast<float>(timePerStep), spikes, static_cast<float>(spikesPerMicrosecond));
    }, py::arg("brain"), py::arg("steps") = 1000,
       "Benchmark brain performance metrics");
    
    m.def("explain_neuron_dynamics", [](const NeuronState& state) {
        // Generate human-readable explanation of neuron dynamics
        std::string explanation = "Neuron State:\n";
        explanation += "  Membrane Potential: " + std::to_string(state.membranePotential) + " mV\n";
        explanation += "  Threshold: " + std::to_string(state.threshold) + " mV\n";
        explanation += "  Firing State: ";
        
        switch (state.firingState) {
            case FiringState::Resting: explanation += "Resting\n"; break;
            case FiringState::Active: explanation += "Active\n"; break;
            case FiringState::Refractory: explanation += "Refractory\n"; break;
            case FiringState::Inhibited: explanation += "Inhibited\n"; break;
        }
        
        explanation += "  Refractory: " + std::to_string(state.refractoryRemaining) + "/" +
                      std::to_string(state.refractoryPeriod) + " steps\n";
        explanation += "  Adaptation Variable: " + std::to_string(state.adaptationVariable) + "\n";
        
        if (state.lastSpikeTime >= 0) {
            explanation += "  Last Spike: " + std::to_string(state.lastSpikeTime) + "s ago\n";
        } else {
            explanation += "  Last Spike: Never\n";
        }
        
        return explanation;
    }, py::arg("state"), "Generate human-readable explanation of neuron dynamics");
    
    m.def("visualize_stdp", [](std::shared_ptr<Synapse> synapse) {
        // Generate visualization data for STDP effects
        if (!synapse) return std::string("{}");
        
        std::string json = "{\"pre_spike_count\": " + std::to_string(synapse->getPreSpikeHistory().size()) +
                         ", \"post_spike_count\": " + std::to_string(synapse->getPostSpikeHistory().size()) +
                         ", \"weight\": " + std::to_string(synapse->getWeight()) +
                         ", \"eligibility_trace\": " + std::to_string(synapse->getEligibilityTrace()) +
                         "}";
        return json;
    }, py::arg("synapse"), "Generate visualization data for STDP effects");
    
    m.def("create_context_manager", [](std::shared_ptr<Brain> brain) {
        // Create a context manager for automatic brain cleanup
        class BrainContext {
        public:
            explicit BrainContext(std::shared_ptr<Brain> b) : brain_(b) {}
            ~BrainContext() {
                if (brain_) {
                    brain_->reset();
                }
            }
            
            Brain* get() { return brain_.get(); }
            
        private:
            std::shared_ptr<Brain> brain_;
        };
        
        return std::make_shared<BrainContext>(brain);
    }, py::arg("brain"), "Create context manager for automatic brain cleanup");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm"