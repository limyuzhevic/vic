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
}

// =============================================================================
// ADVANCED FUNCTIONALITY FOR EXPERT USERS
// =============================================================================

// Memory system access methods
m.def("getWorkingMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralWorkingMemory> {
    return brain->getWorkingMemory();
}, py::arg("brain"), "Access working memory system");

m.def("getEpisodicMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralEpisodicMemory> {
    return brain->getEpisodicMemory();
}, py::arg("brain"), "Access episodic memory system");

m.def("getAssociativeMemory", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralAssociativeMemory> {
    return brain->getAssociativeMemory();
}, py::arg("brain"), "Access associative memory system");

// Prediction system access methods
m.def("getPredictionSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PredictionSystem> {
    return brain->getPredictionSystem();
}, py::arg("brain"), "Access prediction system");

m.def("getPlanner", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<NeuralPlanner> {
    return brain->getPlanner();
}, py::arg("brain"), "Access neural planner");

m.def("getConceptFormation", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<ConceptFormation> {
    return brain->getConceptFormation();
}, py::arg("brain"), "Access concept formation system");

m.def("getAttention", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AttentionalSelection> {
    return brain->getAttention();
}, py::arg("brain"), "Access attentional selection system");

// Neuromodulation system access
m.def("getDopamine", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Dopamine> {
    return brain->getDopamine();
}, py::arg("brain"), "Access dopamine neuromodulator");

m.def("getCuriosity", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Curiosity> {
    return brain->getCuriosity();
}, py::arg("brain"), "Access curiosity neuromodulator");

m.def("getNovelty", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Novelty> {
    return brain->getNovelty();
}, py::arg("brain"), "Access novelty neuromodulator");

m.def("getPredictionErrorSignal", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<PredictionError> {
    return brain->getPredictionErrorSignal();
}, py::arg("brain"), "Access prediction error signal");

// Plasticity system access
m.def("getSTDP", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<STDP> {
    return brain->getSTDP();
}, py::arg("brain"), "Access STDP plasticity system");

m.def("getHebbian", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<Hebbian> {
    return brain->getHebbian();
}, py::arg("brain"), "Access Hebbian plasticity system");

m.def("getStructuralPlasticity", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<StructuralPlasticity> {
    return brain->getStructuralPlasticity();
}, py::arg("brain"), "Access structural plasticity system");

// Development system access
m.def("getDevelopmentSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<DevelopmentSystem> {
    return brain->getDevelopmentSystem();
}, py::arg("brain"), "Access development system");

// Spike system access
m.def("getSpikeSystem", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<SpikeSystem> {
    return brain->getSpikeSystem();
}, py::arg("brain"), "Access spike system for advanced spike manipulation");

// Direct system control
m.def("applyNeuromodulation", [](std::shared_ptr<Brain> brain, const Neuromodulator& signal) {
    brain->applyNeuromodulation(signal);
}, py::arg("brain"), py::arg("signal"), "Apply neuromodulatory signal directly to brain");

m.def("updatePlasticity", [](std::shared_ptr<Brain> brain) {
    brain->updatePlasticity();
}, py::arg("brain"), "Update plasticity rules directly");

m.def("develop", [](std::shared_ptr<Brain> brain) {
    brain->develop();
}, py::arg("brain"), "Apply developmental changes directly");

// Connection management
m.def("addInterRegionConnection", [](std::shared_ptr<Brain> brain, RegionId source, RegionId target, 
                                      float weight = 0.0f, Delay delay = 1) {
    brain->addInterRegionConnection(source, target, weight, delay);
}, py::arg("brain"), py::arg("source"), py::arg("target"), 
      py::arg("weight") = 0.0f, py::arg("delay") = 1,
      "Create connection between brain regions");

m.def("removeInterRegionConnection", [](std::shared_ptr<Brain> brain, RegionId source, RegionId target) {
    brain->removeInterRegionConnection(source, target);
}, py::arg("brain"), py::arg("source"), py::arg("target"), "Remove connection between brain regions");

// Enhanced AgentBrain accessors for neuromodulation state
m.def("getDopamineLevel", [](std::shared_ptr<AgentBrain> agent) -> float {
    return agent->getDopamineLevel();
}, py::arg("agent"), "Get current dopamine level from agent");

m.def("getCuriosityLevel", [](std::shared_ptr<AgentBrain> agent) -> float {
    return agent->getCuriosityLevel();
}, py::arg("agent"), "Get current curiosity level from agent");

m.def("getNoveltyLevel", [](std::shared_ptr<AgentBrain> agent) -> float {
    return agent->getNoveltyLevel();
}, py::arg("agent"), "Get current novelty level from agent");

m.def("getPredictionError", [](std::shared_ptr<AgentBrain> agent) -> float {
    return agent->getPredictionError();
}, py::arg("agent"), "Get current prediction error from agent");

m.def("getNeuromodulationLevel", [](std::shared_ptr<AgentBrain> agent) -> float {
    return agent->getNeuromodulationLevel();
}, py::arg("agent"), "Get overall neuromodulation level from agent");

// Configuration helper functions
m.def("createConfigWithDefaults", []() -> std::shared_ptr<Config> {
    auto config = std::make_shared<Config>();
    // Set reasonable defaults for simulation
    config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("brain.synapse_density", 0.05f, ConfigSource::Default);
    config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
    config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
    config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
    config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
    config->set("brain.v_rest", -70.0f, ConfigSource::Default);
    config->set("brain.v_reset", -75.0f, ConfigSource::Default);
    config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
    config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
    config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
    config->set("simulation_timestep", 0.001, ConfigSource::Default);
    
    // Enable core plasticity rules
    config->set("plasticity.stdp.enable", true, ConfigSource::Default);
    config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_plus", 20.0f, ConfigSource::Default);
    config->set("plasticity.stdp.tau_minus", 20.0f, ConfigSource::Default);
    config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
    config->set("plasticity.hebbian.learning_rate", 0.0005f, ConfigSource::Default);
    config->set("plasticity.structural.enable", true, ConfigSource::Default);
    config->set("plasticity.structural.synaptogenesis_rate", 0.0001f, ConfigSource::Default);
    config->set("plasticity.structural.pruning_rate", 0.00001f, ConfigSource::Default);
    
    // Enable neuromodulation
    config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
    config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
    config->set("neuromod.curiosity.exploration_rate", 0.1f, ConfigSource::Default);
    config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    config->set("neuromod.novelty.sensitivity", 0.5f, ConfigSource::Default);
    
    return config;
}, "Create configuration with sensible defaults for simulation");

m.def("createWorldWithDefaults", []() -> std::shared_ptr<SimpleWorld> {
    auto world = std::make_shared<SimpleWorld>();
    world->configure(width=20, height=20, visionWidth=8, visionHeight=8);
    world->setMaxEnergy(100.0f);
    world->setEnergyDecayRate(0.01f);
    return world;
}, "Create world with default configuration");

// Error handling and validation
m.def("validateBrainState", [](std::shared_ptr<Brain> brain) -> bool {
    // Basic validation checks
    bool valid = true;
    
    // Check neuron count
    if (brain->getTotalNeuronCount() == 0) {
        std::cerr << "WARNING: Brain has no neurons!" << std::endl;
        valid = false;
    }
    
    // Check for regions
    if (brain->getRegionCount() == 0) {
        std::cerr << "WARNING: Brain has no regions!" << std::endl;
        valid = false;
    }
    
    // Check spike system
    auto spikeSystem = brain->getSpikeSystem();
    if (spikeSystem->getPendingSpikeEventCount() > 1000) {
        std::cerr << "WARNING: High number of pending spike events (" 
                  << spikeSystem->getPendingSpikeEventCount() << ")" << std::endl;
        valid = false;
    }
    
    return valid;
}, py::arg("brain"), "Validate brain state and return true if configuration is valid");

// Simulation helper functions
m.def("runTrainingEpisode", 
      [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
         size_t numSteps, float timestep = 0.1f) {
          if (!brain->initialize() || !agent->initialize(*world)) {
              throw std::runtime_error("Failed to initialize brain or agent");
          }
          
          world->reset();
          world->setAgentStart(10.0f, 10.0f);
          
          for (size_t step = 0; step < numSteps; ++step) {
              // Update world
              world->update(timestep);
              
              // Get sensory input
              auto percept = world->getSensoryPercept();
              
              // Process sensory input
              agent->processSensoryInput(percept);
              
              // Run brain step
              brain->step(step, world->getSimulationTime());
              
              // Get action
              auto action = agent->decodeMotorCommand();
              
              // Apply action to world
              world->applyMotorCommand(action, world->getSimulationTime());
              
              // Apply reward modulation based on agent state
              float curiosity = agent->getCuriosityLevel();
              float novelty = agent->getNoveltyLevel();
              float reward = curiosity * 0.1f + novelty * 0.05f;
              agent->applyRewardModulation(reward, 0.0f);
              
              // Update development
              agent->updateDevelopment(timestep);
          }
      },
      py::arg("brain"), py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("timestep") = 0.1f,
      "Run a complete training episode with brain, agent, and world");

m.def("saveBrainWithTimestamp", [](std::shared_ptr<Brain> brain, const std::string& basePath) {
    // Create filename with timestamp
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream filename;
    filename << basePath << "_" << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S") 
             << "_neurons" << brain->getTotalNeuronCount() 
             << "_spikes" << brain->getTotalSpikeCount() << ".bin";
    
    if (!brain->save(filename.str())) {
        throw std::runtime_error("Failed to save brain state");
    }
    
    return filename.str();
}, py::arg("brain"), py::arg("basePath"), "Save brain state with timestamp in filename");

// Convenience functions for common operations
m.def("resetSystem", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world) {
    brain->reset();
    agent->reset();
    world->reset();
    world->setAgentStart(10.0f, 10.0f);
}, py::arg("brain"), py::arg("agent"), py::arg("world"), "Reset all system components");

m.def("getSystemStatistics", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
    py::dict stats;
    stats["neurons"] = brain->getTotalNeuronCount();
    stats["synapses"] = brain->getTotalSynapseCount();
    stats["active_neurons"] = brain->getActiveNeuronCount();
    stats["firing_neurons"] = brain->getFiringNeuronCount();
    stats["spike_count"] = brain->getTotalSpikeCount();
    stats["pending_spikes"] = brain->getPendingSpikeEventCount();
    stats["average_firing_rate"] = brain->getAverageFiringRate();
    stats["e_i_ratio"] = brain->getExcitationInhibitionRatio();
    stats["curiosity"] = agent->getCuriosityLevel();
    stats["novelty"] = agent->getNoveltyLevel();
    stats["prediction_error"] = agent->getPredictionError();
    stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
    stats["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
    return stats;
}, py::arg("brain"), py::arg("agent"), "Get comprehensive system statistics as dictionary");

m.def("printSystemStatus", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent) {
    brain->logStatus();
    std::cout << "[Agent Status]" << std::endl;
    std::cout << "  Curiosity Level: " << agent->getCuriosityLevel() << std::endl;
    std::cout << "  Novelty Level: " << agent->getNoveltyLevel() << std::endl;
    std::cout << "  Prediction Error: " << agent->getPredictionError() << std::endl;
    std::cout << "  Neuromodulation Level: " << agent->getNeuromodulationLevel() << std::endl;
    std::cout << "  Developmental Stage: " << static_cast<int>(agent->getDevelopmentalStage()) << std::endl;
    std::cout << "  Reward Mod. Enabled: " << (agent->isRewardModulationEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  Structural Plasticity Enabled: " << (agent->isStructuralPlasticityEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  Development Enabled: " << (agent->isDevelopmentEnabled() ? "Yes" : "No") << std::endl;
    std::cout << "  Curiosity Enabled: " << (agent->isCuriosityEnabled() ? "Yes" : "No") << std::endl;
}, py::arg("brain"), py::arg("agent"), "Print comprehensive system status");

// =============================================================================
// COMMAND-LINE INTERFACE SUPPORT
// =============================================================================

m.def("create_python_script_from_template", []() {
    return R"PYTHON_SCRIPT(
#!/usr/bin/env python3
"""Automatically generated NLM Python script from template."""

import pynlm
import sys
import os

def run_brain_simulation(config_path=None, num_steps=1000, output_file=None):
    """Run a brain simulation with the given configuration."""
    
    try:
        # Create configuration
        if config_path and os.path.exists(config_path):
            config = pynlm.createDefaultConfig()
            config.loadFromFile(config_path)
        else:
            config = pynlm.createConfigWithDefaults()
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Starting simulation with {brain.getTotalNeuronCount()} neurons")
        print(f"Target steps: {num_steps}")
        
        # Run simulation
        for step in range(num_steps):
            brain.step(step)
            
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"{brain.getTotalSpikeCount()} total spikes")
                
                # Print additional stats
                if step % 500 == 0:
                    stats = pynlm.getSystemStatistics(brain, None)
                    print(f"  Average firing rate: {stats['average_firing_rate']:.3f}")
                    print(f"  E/I ratio: {stats['e_i_ratio']:.3f}")
        
        # Save brain state if requested
        if output_file:
            brain.save(output_file)
            print(f"Brain state saved to {output_file}")
        
        print("Simulation completed successfully!")
        
    except Exception as e:
        print(f"Error during simulation: {e}")
        sys.exit(1)

if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser(description="NLM Brain Simulation")
    parser.add_argument("--config", help="Configuration file path")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--output", help="Output file for brain state")
    
    args = parser.parse_args()
    run_brain_simulation(args.config, args.steps, args.output)
)PYTHON_SCRIPT";
}, "Generate a complete Python script template for running simulations");

m.def("create_jupyter_notebook_template", []() {
    return R"JUPYTER_NOTEBOOK(
{
 "cells": [
  {
   "cell_type": "markdown",
   "metadata": {},
   "source": [
    "# NLM Brain Simulation in Jupyter Notebook",
    "",
    "This notebook demonstrates how to use the NLM Python bindings to create and run neural simulations."
   ]
  },
  {
   "cell_type": "code",
   "execution_count": null,
   "metadata": {},
   "outputs": [],
   "source": [
    "import pynlm",
    "import matplotlib.pyplot as plt",
    "import numpy as np",
    "",
    "# Create a brain with default configuration",
    "config = pynlm.createDefaultConfig()",
    "brain = pynlm.createBrain(config)",
    "brain.initialize()",
    "",
    "print(f\"Created brain with {brain.getTotalNeuronCount()} neurons\")"
   ]
  },
  {
   "cell_type": "code",
   "execution_count": null,
   "metadata": {},
   "outputs": [],
   "source": [
    "# Run a short simulation",
    "num_steps = 500",
    "firing_rates = []",
    "spike_counts = []",
    "",
    "for step in range(num_steps):",
    "    brain.step(step)",
    "    ",
    "    if step % 50 == 0:",
    "        firing_rate = brain.getFiringNeuronCount() / brain.getTotalNeuronCount()",
    "        firing_rates.append(firing_rate)
    "        spike_counts.append(brain.getTotalSpikeCount())",
    "",
    "# Plot results",
    "fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(10, 8))",
    "",
    "ax1.plot(firing_rates)",
    "ax1.set_xlabel('Step / 50')",
    "ax1.set_ylabel('Firing Rate')",
    "ax1.set_title('Neuron Firing Rate Over Time')",
    "ax1.grid(True)",
    "",
    "ax2.plot(spike_counts)",
    "ax2.set_xlabel('Step / 50')",
    "ax2.set_ylabel('Total Spike Count')",
    "ax2.set_title('Cumulative Spike Count Over Time')",
    "ax2.grid(True)",
    "",
    "plt.tight_layout()",
    "plt.show()"
   ]
  }
 ],
 "metadata": {
  "kernelspec": {
   "display_name": "Python 3",
   "language": "python",
   "name": "python3"
  },
  "language_info": {
   "name": "python",
   "version": "3.8+"
  }
 },
 "nbformat": 4,
 "nbformat_minor": 4
}
)JUPYTER_NOTEBOOK";
}, "Generate a Jupyter notebook template for interactive exploration");

m.def("create_advanced_agent_example", []() {
    return R"PYTHON_ADVANCED(
#!/usr/bin/env python3
"""Advanced NLM agent example with complete world interaction."""

import pynlm
import math
import random

class NLMAgent:
    """Advanced NLM agent with learning and exploration capabilities."""
    
    def __init__(self, config_file=None):
        # Create configuration
        if config_file:
            self.config = pynlm.createDefaultConfig()
            self.config.loadFromFile(config_file)
        else:
            self.config = pynlm.createConfigWithDefaults()
        
        # Create brain
        self.brain = pynlm.createBrain(self.config)
        self.brain.initialize()
        
        # Create agent brain interface
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Create world
        self.world = pynlm.createWorldWithDefaults()
        
        # Initialize agent
        self.agent.initialize(self.world)
        
        # Enable learning systems
        self.agent.enableRewardModulation(True)
        self.agent.enableStructuralPlasticity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableCuriosity(True)
        
        # Agent state
        self.episode_reward = 0.0
        self.step_count = 0
        self.observation_history = []
        
        print(f"Initialized advanced agent with {self.brain.getTotalNeuronCount()} neurons")
    
    def get_observation(self):
        """Get current sensory observation from world."""
        return self.world.getSensoryPercept()
    
    def take_action(self, action):
        """Take an action in the world and return result."""
        # Apply action to world
        self.world.applyMotorCommand(action, self.world.getSimulationTime())
        
        # Get reward from agent's internal state
        curiosity = self.agent.getCuriosityLevel()
        novelty = self.agent.getNoveltyLevel()
        reward = curiosity * 0.1f + novelty * 0.05f
        
        # Get next observation
        next_observation = self.get_observation()
        
        # Store experience
        self.observation_history.append({
            'action': action,
            'reward': reward,
            'next_observation': next_observation,
            'step': self.step_count
        })
        
        self.episode_reward += reward
        self.step_count += 1
        
        return reward, next_observation
    
    def run_episode(self, max_steps=1000, timestep=0.1f):
        """Run a complete episode of agent-world interaction."""
        print(f"Running episode (max {max_steps} steps)...")
        
        # Reset world
        self.world.reset()
        self.world.setAgentStart(10.0f, 10.0f)
        
        # Reset agent
        self.agent.reset()
        self.episode_reward = 0.0
        self.step_count = 0
        self.observation_history = []
        
        for step in range(max_steps):
            # Update world
            self.world.update(timestep)
            
            # Get current observation
            observation = self.get_observation()
            
            # Process observation through agent
            self.agent.processSensoryInput(observation)
            
            # Brain computation
            self.brain.step(step, self.world.getSimulationTime())
            
            # Get action from agent
            action = self.agent.decodeMotorCommand()
            
            # Take action and get reward
            reward, next_observation = self.take_action(action)
            
            # Apply reward modulation
            self.agent.applyRewardModulation(reward, 0.0f)
            
            # Update development
            self.agent.updateDevelopment(timestep)
            
            # Print progress
            if step % 100 == 0:
                print(f"  Step {step}: Reward={reward:.3f}, "
                      f"Curiosity={self.agent.getCuriosityLevel():.3f}, "
                      f"Novelty={self.agent.getNoveltyLevel():.3f}, "
                      f"Total Episode Reward={self.episode_reward:.3f}")
        
        print(f"Episode completed after {self.step_count} steps")
        print(f"Total episode reward: {self.episode_reward:.3f}")
        
        return self.episode_reward, self.observation_history
    
    def get_brain_stats(self):
        """Get comprehensive brain statistics."""
        stats = {
            'neurons': self.brain.getTotalNeuronCount(),
            'synapses': self.brain.getTotalSynapseCount(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'spike_count': self.brain.getTotalSpikeCount(),
            'pending_spikes': self.brain.getPendingSpikeEventCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
            'curiosity': self.agent.getCuriosityLevel(),
            'novelty': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel(),
            'developmental_stage': int(self.agent.getDevelopmentalStage()),
            'reward_mod_enabled': self.agent.isRewardModulationEnabled(),
            'structural_plasticity_enabled': self.agent.isStructuralPlasticityEnabled(),
            'development_enabled': self.agent.isDevelopmentEnabled(),
            'curiosity_enabled': self.agent.isCuriosityEnabled(),
        }
        return stats
    
    def save_brain_state(self, filepath):
        """Save brain state to file."""
        self.brain.save(filepath)
        print(f"Brain state saved to {filepath}")
    
    def load_brain_state(self, filepath):
        """Load brain state from file."""
        if self.brain.load(filepath):
            print(f"Brain state loaded from {filepath}")
        else:
            print(f"Warning: Failed to load brain state from {filepath}")
    
    def print_status(self):
        """Print comprehensive agent status."""
        print("=" * 60)
        print("NLM AGENT STATUS REPORT")
        print("=" * 60)
        
        print(f"\n[BRAIN STATISTICS]")
        print(f"  Neurons: {self.brain.getTotalNeuronCount()}")
        print(f"  Synapses: {self.brain.getTotalSynapseCount()}")
        print(f"  Active Neurons: {self.brain.getActiveNeuronCount()}")
        print(f"  Firing Neurons: {self.brain.getFiringNeuronCount()}")
        print(f"  Spike Count: {self.brain.getTotalSpikeCount()}")
        print(f"  Average Firing Rate: {self.brain.getAverageFiringRate():.3f}")
        print(f"  E/I Ratio: {self.brain.getExcitationInhibitionRatio():.3f}")
        
        print(f"\n[AGENT STATISTICS]")
        print(f"  Curiosity Level: {self.agent.getCuriosityLevel():.3f}")
        print(f"  Novelty Level: {self.agent.getNoveltyLevel():.3f}")
        print(f"  Prediction Error: {self.agent.getPredictionError():.3f}")
        print(f"  Neuromodulation Level: {self.agent.getNeuromodulationLevel():.3f}")
        print(f"  Developmental Stage: {int(self.agent.getDevelopmentalStage())}")
        
        print(f"\n[SYSTEM ENABLED]")
        print(f"  Reward Modulation: {'Yes' if self.agent.isRewardModulationEnabled() else 'No'}")
        print(f"  Structural Plasticity: {'Yes' if self.agent.isStructuralPlasticityEnabled() else 'No'}")
        print(f"  Development: {'Yes' if self.agent.isDevelopmentEnabled() else 'No'}")
        print(f"  Curiosity: {'Yes' if self.agent.isCuriosityEnabled() else 'No'}")
        
        print(f"\n[EPISODE INFO]")
        print(f"  Steps: {self.step_count}")
        print(f"  Total Reward: {self.episode_reward:.3f}")
        print(f"  Observations: {len(self.observation_history)}")
        
        print("=" * 60)

# Example usage function
def example_advanced_usage():
    """Example of advanced NLM agent usage."""
    print("Example: Advanced NLM Agent with Complete World Interaction")
    print("=" * 60)
    
    # Create agent
    agent = NLMAgent()
    
    # Print initial status
    agent.print_status()
    
    # Run multiple episodes
    for episode in range(3):
        print(f"\n--- Episode {episode + 1} ---")
        reward, history = agent.run_episode(max_steps=500)
        
        # Save brain state after each episode
        agent.save_brain_state(f"agent_episode_{episode + 1}.bin")
        
        # Print final stats
        stats = agent.get_brain_stats()
        print(f"\nEpisode {episode + 1} Statistics:")
        print(f"  Final Reward: {reward:.3f}")
        print(f"  Total Synapses: {stats['synapses']}")
        print(f"  Spike Count: {stats['spike_count']}")
        print(f"  Average Firing Rate: {stats['average_firing_rate']:.3f}")
    
    print("\nAdvanced agent example completed successfully!")

if __name__ == "__main__":
    example_advanced_usage()
)PYTHON_ADVANCED";
}, "Generate advanced agent example with complete functionality");

m.attr("ADVANCED_PYTHON_TEMPLATE") = "agent_example.py";
m.attr("JUPYTER_NOTEBOOK_TEMPLATE") = "nlm_analysis.ipynb";
m.attr("SCRIPT_TEMPLATE") = "nlm_simulation.py";
}

} // namespace nlm
