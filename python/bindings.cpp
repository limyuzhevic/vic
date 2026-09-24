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
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/BCMRule.hpp"
#include "../src/plasticity/AntiHebbian.hpp"
#include "../src/plasticity/RewardModulatedSTDP.hpp"
#include "../src/plasticity/TripletSTDP.hpp"
#include "../src/brain/Neuron.hpp"
#include "../src/brain/Synapse.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/prediction/PredictionSystem.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Dopamine.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides comprehensive bindings for brain simulation,
        plasticity rules, neurons, synapses, memory systems,
        neuromodulation, and agent interfaces.
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

    py::class_<PlasticityRule>(m, "PlasticityRule", R"pbdoc(Abstract base class for plasticity rules)pbdoc")
        .def("isEnabled", &PlasticityRule::isEnabled,
             "Check if plasticity rule is enabled")
        .def("setEnabled", &PlasticityRule::setEnabled, py::arg("enabled"),
             "Enable or disable the plasticity rule")
        .def("getName", &PlasticityRule::getName,
             "Get the name of the plasticity rule");

    py::class_<Hebbian, PlasticityRule>(m, "Hebbian", R"pbdoc(Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("update", &Hebbian::update, py::arg("synapse"), py::arg("preSpikes"),
             py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on spike timing")
        .def("applyWeightChange", &Hebbian::applyWeightChange, py::arg("synapse"),
             py::arg("delta"), "Apply a specific weight change")
        .def("setLearningRate", &Hebbian::setLearningRate, py::arg("rate"),
             "Set Hebbian learning rate")
        .def("getLearningRate", &Hebbian::getLearningRate,
             "Get current learning rate")
        .def("setMaxWeight", &Hebbian::setMaxWeight, py::arg("maxWeight"),
             "Set maximum synaptic weight")
        .def("getMaxWeight", &Hebbian::getMaxWeight,
             "Get maximum weight")
        .def("setMinWeight", &Hebbian::setMinWeight, py::arg("minWeight"),
             "Set minimum synaptic weight")
        .def("getMinWeight", &Hebbian::getMinWeight,
             "Get minimum weight");

    py::class_<AntiHebbian, PlasticityRule>(m, "AntiHebbian", R"pbdoc(Anti-Hebbian learning rule)pbdoc")
        .def(py::init<>())
        .def("update", &AntiHebbian::update, py::arg("synapse"), py::arg("preSpikes"),
             py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on anti-Hebbian learning")
        .def("applyWeightChange", &AntiHebbian::applyWeightChange, py::arg("synapse"),
             py::arg("delta"), "Apply a specific weight change")
        .def("setLearningRate", &AntiHebbian::setLearningRate, py::arg("rate"),
             "Set anti-Hebbian learning rate")
        .def("getLearningRate", &AntiHebbian::getLearningRate,
             "Get current learning rate");

    py::class_<BCMRule, PlasticityRule>(m, "BCMRule", R"pbdoc(BCM learning rule)pbdoc")
        .def(py::init<>())
        .def("update", &BCMRule::update, py::arg("synapse"), py::arg("preSpikes"),
             py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on BCM rule")
        .def("applyWeightChange", &BCMRule::applyWeightChange, py::arg("synapse"),
             py::arg("delta"), "Apply a specific weight change")
        .def("setLearningRate", &BCMRule::setLearningRate, py::arg("rate"),
             "Set BCM learning rate")
        .def("getLearningRate", &BCMRule::getLearningRate,
             "Get current learning rate")
        .def("setThetaPlus", &BCMRule::setThetaPlus, py::arg("thetaPlus"),
             "Set BCM potentiation threshold (θ+)")
        .def("getThetaPlus", &BCMRule::getThetaPlus,
             "Get potentiation threshold")
        .def("setThetaMinus", &BCMRule::setThetaMinus, py::arg("thetaMinus"),
             "Set BCM depression threshold (θ-)")
        .def("getThetaMinus", &BCMRule::getThetaMinus,
             "Get depression threshold")
        .def("setTauTheta", &BCMRule::setTauTheta, py::arg("tauTheta"),
             "Set BCM threshold time constant")
        .def("getTauTheta", &BCMRule::getTauTheta,
             "Get threshold time constant");

    py::class_<STDP, PlasticityRule>(m, "STDP", R"pbdoc(Standard Spike-Timing-Dependent Plasticity)pbdoc")
        .def(py::init<>())
        .def("update", &STDP::update, py::arg("synapse"), py::arg("preSpikes"),
             py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on STDP")
        .def("applyWeightChange", &STDP::applyWeightChange, py::arg("synapse"),
             py::arg("delta"), "Apply a specific weight change")
        .def("setLTPWeight", &STDP::setLTPWeight, py::arg("weight"),
             "Set STDP LTP weight (A+)")
        .def("getLTPWeight", &STDP::getLTPWeight,
             "Get LTP weight")
        .def("setLTDWeight", &STDP::setLTDWeight, py::arg("weight"),
             "Set STDP LTD weight (A-)")
        .def("getLTDWeight", &STDP::getLTDWeight,
             "Get LTD weight")
        .def("setTimeConstant", &STDP::setTimeConstant, py::arg("tau"),
             "Set STDP time constant (τ)")
        .def("getTimeConstant", &STDP::getTimeConstant,
             "Get time constant")
        .def("configure", &STDP::configure, py::arg("ltpWeight"), py::arg("ltdWeight"), py::arg("tau"),
             "Configure STDP parameters from configuration values");

    py::class_<RewardModulatedSTDP, PlasticityRule>(m, "RewardModulatedSTDP", R"pbdoc(Reward-modulated STDP)pbdoc")
        .def(py::init<>())
        .def("update", &RewardModulatedSTDP::update, py::arg("synapse"), py::arg("preSpikes"),
             py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on reward-modulated STDP")
        .def("applyWeightChange", &RewardModulatedSTDP::applyWeightChange, py::arg("synapse"),
             py::arg("delta"), "Apply a specific weight change")
        .def("setBaseLTPWeight", &RewardModulatedSTDP::setBaseLTPWeight, py::arg("weight"),
             "Set base STDP LTP weight")
        .def("getBaseLTPWeight", &RewardModulatedSTDP::getBaseLTPWeight,
             "Get base LTP weight")
        .def("setBaseLTDWeight", &RewardModulatedSTDP::setBaseLTDWeight, py::arg("weight"),
             "Set base STDP LTD weight")
        .def("getBaseLTDWeight", &RewardModulatedSTDP::getBaseLTDWeight,
             "Get base LTD weight")
        .def("setRewardModulationFactor", &RewardModulatedSTDP::setRewardModulationFactor, py::arg("factor"),
             "Set reward modulation factor (γ)")
        .def("getRewardModulationFactor", &RewardModulatedSTDP::getRewardModulationFactor,
             "Get reward modulation factor")
        .def("setBaseTimeConstant", &RewardModulatedSTDP::setBaseTimeConstant, py::arg("tau"),
             "Set base STDP time constant")
        .def("getBaseTimeConstant", &RewardModulatedSTDP::getBaseTimeConstant,
             "Get base time constant")
        .def("setReward", &RewardModulatedSTDP::setReward, py::arg("reward"),
             "Set current reward signal")
        .def("getReward", &RewardModulatedSTDP::getReward,
             "Get current reward signal");

    py::class_<TripletSTDP, PlasticityRule>(m, "TripletSTDP", R"pbdoc(Triplet STDP with triple interactions)pbdoc")
        .def(py::init<>())
        .def("update", &TripletSTDP::update, py::arg("synapse"), py::arg("preSpikes"),
             py::arg("postSpikes"), py::arg("dt"),
             "Update synaptic weights based on triplet STDP")
        .def("applyWeightChange", &TripletSTDP::applyWeightChange, py::arg("synapse"),
             py::arg("delta"), "Apply a specific weight change")
        .def("setLTPWeight", &TripletSTDP::setLTPWeight, py::arg("weight"),
             "Set triplet STDP LTP weight")
        .def("getLTPWeight", &TripletSTDP::getLTPWeight,
             "Get LTP weight")
        .def("setLTDWeight", &TripletSTDP::setLTDWeight, py::arg("weight"),
             "Set triplet STDP LTD weight")
        .def("getLTDWeight", &TripletSTDP::getLTDWeight,
             "Get LTD weight")
        .def("setTimeConstant", &TripletSTDP::setTimeConstant, py::arg("tau"),
             "Set triplet STDP time constant")
        .def("getTimeConstant", &TripletSTDP::getTimeConstant,
             "Get time constant")
        .def("setTraceDecay", &TripletSTDP::setTraceDecay, py::arg("tauPre"), py::arg("tauPost"),
             "Set pre- and post-synaptic trace decay time constants")
        .def("getTraceDecay", &TripletSTDP::getTraceDecay,
             "Get trace decay time constants");

    py::class_<Neuron>(m, "Neuron", R"pbdoc(Individual neuron implementing LIF dynamics)pbdoc")
        .def(py::init<NeuronId>(), py::arg("id"))
        .def("getId", &Neuron::getId, "Get neuron ID")
        .def("getType", &Neuron::getType, "Get neuron type")
        .def("setType", &Neuron::setType, py::arg("type"), "Set neuron type")
        .def("getMembranePotential", &Neuron::getMembranePotential, "Get membrane potential")
        .def("setMembranePotential", &Neuron::setMembranePotential, py::arg("potential"), "Set membrane potential")
        .def("addToMembranePotential", &Neuron::addToMembranePotential, py::arg("delta"), "Add to membrane potential")
        .def("getThreshold", &Neuron::getThreshold, "Get firing threshold")
        .def("setThreshold", &Neuron::setThreshold, py::arg("threshold"), "Set firing threshold")
        .def("isFiring", &Neuron::isFiring, "Check if neuron is firing")
        .def("isRefractory", &Neuron::isRefractory, "Check if neuron is refractory")
        .def("setFiringState", &Neuron::setFiringState, py::arg("state"), "Set firing state")
        .def("getFiringRate", &Neuron::getFiringRate, "Get firing rate")
        .def("setFiringRate", &Neuron::setFiringRate, py::arg("rate"), "Set firing rate")
        .def("setLeakConductance", &Neuron::setLeakConductance, py::arg("conductance"), "Set leak conductance")
        .def("getLeakConductance", &Neuron::getLeakConductance, "Get leak conductance")
        .def("stepLIF", &Neuron::stepLIF, py::arg("currentTime"), py::arg("dt"), "Step LIF dynamics")
        .def("receiveExcitatoryInput", &Neuron::receiveExcitatoryInput, py::arg("amplitude"), "Receive excitatory input")
        .def("receiveInhibitoryInput", &Neuron::receiveInhibitoryInput, py::arg("amplitude"), "Receive inhibitory input")
        .def("receiveModulatoryInput", &Neuron::receiveModulatoryInput, py::arg("amplitude"), "Receive modulatory input")
        .def("injectCurrent", &Neuron::injectCurrent, py::arg("current"), "Inject external current")
        .def("recordSpike", &Neuron::recordSpike, py::arg("timestamp"), "Record spike timestamp")
        .def("getSpikeHistory", &Neuron::getSpikeHistory, "Get spike history")
        .def("clearSpikeHistory", &Neuron::clearSpikeHistory, "Clear spike history")
        .def("getState", &Neuron::getState, py::return_value_policy::reference_internal, "Get neuron state")
        .def("reset", &Neuron::reset, "Reset neuron to initial state")
        .def("initializeRandom", &Neuron::initializeRandom, py::arg("rng"), "Initialize with random parameters");

    py::class_<Synapse>(m, "Synapse", R"pbdoc(Individual synapse connection between neurons)pbdoc")
        .def(py::init<SynapseId, NeuronId, NeuronId>(), py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId, "Get synapse ID")
        .def("getSourceNeuron", &Synapse::getSourceNeuron, "Get source neuron ID")
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron, "Get destination neuron ID")
        .def("getWeight", &Synapse::getWeight, "Get synaptic weight")
        .def("setWeight", &Synapse::setWeight, py::arg("weight"), "Set synaptic weight")
        .def("addToWeight", &Synapse::addToWeight, py::arg("delta"), "Add to synaptic weight")
        .def("getDelay", &Synapse::getDelay, "Get synaptic delay")
        .def("setDelay", &Synapse::setDelay, py::arg("delay"), "Set synaptic delay")
        .def("getType", &Synapse::getType, "Get synapse type")
        .def("setType", &Synapse::setType, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory, "Check if synapse is excitatory")
        .def("isInhibitory", &Synapse::isInhibitory, "Check if synapse is inhibitory")
        .def("recordPreSpike", &Synapse::recordPreSpike, py::arg("timestamp"), "Record pre-synaptic spike")
        .def("recordPostSpike", &Synapse::recordPostSpike, py::arg("timestamp"), "Record post-synaptic spike")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory, "Get pre-synaptic spike history")
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory, "Get post-synaptic spike history")
        .def("clearHistory", &Synapse::clearHistory, "Clear spike history")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace, "Get eligibility trace")
        .def("setEligibilityTrace", &Synapse::setEligibilityTrace, py::arg("trace"), "Set eligibility trace")
        .def("getEfficacy", &Synapse::getEfficacy, "Get synaptic efficacy")
        .def("setEfficacy", &Synapse::setEfficacy, py::arg("efficacy"), "Set synaptic efficacy")
        .def("step", &Synapse::step, py::arg("currentTime"), "Update synapse state")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", &Synapse::initializeRandom, py::arg("rng"), "Initialize with random parameters");

    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Working memory for active information)pbdoc")
        .def(py::init<>())
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuronId"), py::arg("activation"), "Store activation to working memory")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"), "Update working memory")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons, "Get neurons in working memory")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces, "Get number of active traces")
        .def("clear", &NeuralWorkingMemory::clear, "Clear working memory");

    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Episodic memory for experience storage)pbdoc")
        .def(py::init<>())
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"), "Store an episode")
        .def("getEpisodes", &NeuralEpisodicMemory::getEpisodes, "Get episodes")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"), "Replay an episode")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount, "Get episode count")
        .def("clear", &NeuralEpisodicMemory::clear, "Clear episodic memory");

    py::class_<Dopamine>(m, "Dopamine", R"pbdoc(Dopamine neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Dopamine::getLevel, "Get current dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor, "Get plasticity modulation factor")
        .def("update", &Dopamine::update, py::arg("dt"), "Update dopamine level");

    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Curiosity::getLevel, "Get current curiosity level")
        .def("update", &Curiosity::update, py::arg("dt"), "Update curiosity level");

    py::class_<Novelty>(m, "Novelty", R"pbdoc(Novelty detection neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &Novelty::getLevel, "Get current novelty level")
        .def("update", &Novelty::update, py::arg("dt"), "Update novelty level");

    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error neuromodulation system)pbdoc")
        .def(py::init<>())
        .def("getLevel", &PredictionError::getLevel, "Get current prediction error level")
        .def("update", &PredictionError::update, py::arg("dt"), "Update prediction error level");

    py::class_<InterRegionConnection>(m, "InterRegionConnection", R"pbdoc(Connection between brain regions)pbdoc")
        .def(py::init<>())
        .def_readwrite("sourceRegion", &InterRegionConnection::sourceRegion)
        .def_readwrite("targetRegion", &InterRegionConnection::targetRegion)
        .def_readwrite("weight", &InterRegionConnection::weight)
        .def_readwrite("delay", &InterRegionConnection::delay);

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

} // namespace nlm

} // namespace nlm

PYBIND11_MODULE(pynlm, m) {
    // Include all bindings (forward to nlm namespace)
    nlm::bind_pynlm(m);
}