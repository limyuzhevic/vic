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
    // Add missing Neuromodulator.hpp include
    #include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/memory/NeuralAssociativeMemory.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/development/Synaptogenesis.hpp"
#include "../src/development/Pruning.hpp"
#include "../src/development/Maturation.hpp"
#include "../src/plasticity/StructuralPlasticity.hpp"
#include "../src/plasticity/Hebbian.hpp"
#include "../src/plasticity/STDP.hpp"
#include "../src/plasticity/PlasticityRule.hpp"
#include "../src/performance/CheckpointSystem.hpp"

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

    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator for reward-based learning)pbdoc")
        .def(py::init<>())
        .def("getName", &Dopamine::getName,
             "Get dopamine name")
        .def("getLevel", &Dopamine::getLevel,
             "Get dopamine level")
        .def("setLevel", &Dopamine::setLevel, py::arg("level"),
             "Set dopamine level")
        .def("getPlasticityFactor", &Dopamine::getPlasticityFactor,
             "Get plasticity factor for learning modulation")
        .def("update", &Dopamine::update, py::arg("dt"),
             "Update dopamine level over time")
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             "Signal reward to dopamine system")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             "Signal reward prediction error to dopamine system");

    py::class_<Curiosity, Neuromodulator>(m, "Curiosity", R"pbdoc(Curiosity neuromodulator for exploration motivation)pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Curiosity::getLevel,
             "Get curiosity level")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             "Update curiosity level based on novelty and prediction error")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             "Get exploration drive level")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             "Set weight of novelty in curiosity calculation")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             "Set weight of prediction error in curiosity calculation")
        .def("reset", &Curiosity::reset,
             "Reset curiosity system");

    py::class_<Novelty, Neuromodulator>(m, "Novelty", R"pbdoc(Novelty detection system)pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getLevel", &Novelty::getLevel,
             "Get novelty level")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             "Set novelty level")
        .def("detectNovelty", [](Novelty& self, const std::vector<float>& current,
                                const std::vector<float>& previous) {
            self.detectNovelty(current, previous);
        }, py::arg("currentPattern"), py::arg("previousPattern"),
             "Detect novelty from sensory input patterns")
        .def("update", &Novelty::update, py::arg("dt"),
             "Decay novelty over time")
        .def("getHistory", &Novelty::getHistory,
             "Get novelty history")
        .def("clearHistory", &Novelty::clearHistory,
             "Clear novelty history");

    py::class_<PredictionError, Neuromodulator>(m, "PredictionError", R"pbdoc(Prediction error signal for learning)pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("getError", &PredictionError::getError,
             "Get prediction error value")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             "Compute prediction error from predicted and actual values")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             "Update prediction value")
        .def("getHistory", &PredictionError::getHistory,
             "Get prediction error history")
        .def("clearHistory", &PredictionError::clearHistory,
             "Clear prediction error history")
        .def("getMagnitude", &PredictionError::getMagnitude,
             "Get magnitude of prediction error");

    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attentional selection system)pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             "Initialize with brain reference")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             "Process competing inputs and select winners")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             "Apply attention to a specific brain region")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             "Release attention from current focus")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             "Get currently attended regions")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength,
             py::arg("strength"), "Set inhibition strength")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength,
             py::arg("strength"), "Set excitation strength")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor,
             py::arg("neuron"), "Get inhibition level for a neuron")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor,
             py::arg("neuron"), "Get excitation level for a neuron")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             "Check if a neuron is currently attended")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             "Update attention dynamics")
        .def("reset", &AttentionalSelection::reset,
             "Reset attention system")
        .def("getWinners", &AttentionalSelection::getWinners,
             "Get current attention winners");

    // Convenience functions for common operations
    m.def("getVersion", []() {
        return "NLM 0.1.0";
    }, "Get NLM version information");

// === ADVANCED BRAIN CREATION ===
    m.def("createBrainAdvanced", [](std::shared_ptr<Config> config, 
                                    bool enableRewardModulation = true,
                                    bool enableCuriosity = true,
                                    bool enableDevelopment = true,
                                    bool enableStructuralPlasticity = true) {
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        if (enableRewardModulation) {
            agent->enableRewardModulation(true);
        }
        if (enableCuriosity) {
            agent->enableCuriosity(true);
        }
        if (enableDevelopment) {
            agent->enableDevelopment(true);
        }
        if (enableStructuralPlasticity) {
            agent->enableStructuralPlasticity(true);
        }
        
        return std::make_tuple(brain, agent);
    }, py::arg("config"), py::arg("enableRewardModulation") = true, 
         py::arg("enableCuriosity") = true, py::arg("enableDevelopment") = true,
         py::arg("enableStructuralPlasticity") = true,
         "Create brain with advanced configuration and return (brain, agent)");

    // === BRAIN STATE MANAGEMENT ===
    m.def("saveBrainSnapshot", [](std::shared_ptr<Brain> brain, const std::string& directory) {
        std::string filepath = directory + "/brain_snapshot_" + 
            std::to_string(brain->getTotalSpikeCount()) + ".bin";
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("directory"),
         "Save brain snapshot to file with timestamp in filename");

    m.def("createBrainCheckpoint", [](std::shared_ptr<Config> config, 
                                       const std::string& checkpointDir,
                                       size_t maxCheckpoints = 10) {
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        // Configure checkpoint system
        brain->getConfig()->set<std::string>("checkpoint_dir", checkpointDir);
        brain->getConfig()->set<size_t>("max_checkpoints", maxCheckpoints);
        
        return std::make_tuple(brain, agent);
    }, py::arg("config"), py::arg("checkpointDir"), py::arg("maxCheckpoints") = 10,
         "Create brain with checkpoint system enabled");

    // === ADVANCED SIMULATION FEATURES ===
    m.def("createEpisodeSimulation", [](std::shared_ptr<Config> config,
                                        size_t numSteps,
                                        bool enableMemory = true,
                                        bool enablePlasticity = true) {
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        // Configure for episodic simulation
        if (enableMemory) {
            agent->enableRewardModulation(true);
        }
        if (enablePlasticity) {
            agent->enableStructuralPlasticity(true);
        }
        
        // Return simulation info object (conceptual)
        struct EpisodeInfo {
            std::shared_ptr<Brain> brain;
            std::shared_ptr<AgentBrain> agent;
            size_t numSteps;
            double startTime;
            
            EpisodeInfo(auto b, auto a, size_t steps) 
                : brain(b), agent(a), numSteps(steps), startTime(0.0) {}
        };
        
        return EpisodeInfo(brain, agent, numSteps);
    }, py::arg("config"), py::arg("numSteps"), py::arg("enableMemory") = true,
         py::arg("enablePlasticity") = true,
         "Create an episodic simulation with specified parameters");

    // === UTILITY FUNCTIONS FOR ADVANCED USERS ===
    m.def("generateBrainConfig", [](size_t neuronCount = 1000,
                                    size_t regionCount = 1,
                                    float connectionProbability = 0.1f,
                                    float initialWeightMean = 0.2f,
                                    float initialWeightStd = 0.1f) {
        auto config = createDefaultConfig();
        
        config->set<size_t>("neuron_count", neuronCount);
        config->set<size_t>("region_count", regionCount);
        config->set<float>("connection_probability", connectionProbability);
        config->set<float>("initial_weight_mean", initialWeightMean);
        config->set<float>("initial_weight_std", initialWeightStd);
        
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
         py::arg("connectionProbability") = 0.1f,
         py::arg("initialWeightMean") = 0.2f, py::arg("initialWeightStd") = 0.1f,
         "Generate a custom brain configuration");

    m.def("analyzeBrainConnectivity", [](std::shared_ptr<Brain> brain) {
        // Return connectivity analysis
        struct ConnectivityAnalysis {
            size_t totalNeurons;
            size_t totalSynapses;
            float excitationInhibitionRatio;
            size_t activeNeuronCount;
            float averageFiringRate;
        };
        
        ConnectivityAnalysis analysis;
        analysis.totalNeurons = brain->getTotalNeuronCount();
        analysis.totalSynapses = brain->getTotalSynapseCount();
        analysis.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        analysis.activeNeuronCount = brain->getActiveNeuronCount();
        analysis.averageFiringRate = brain->getAverageFiringRate();
        
        return analysis;
    }, py::arg("brain"),
         "Analyze and return brain connectivity statistics");

    m.def("exportBrainStats", [](std::shared_ptr<Brain> brain, 
                                 const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "=== NLM BRAIN STATISTICS ===" << std::endl;
        file << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        file << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        file << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        file << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        file << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
        
        file.close();
        return true;
    }, py::arg("brain"), py::arg("filename"),
         "Export brain statistics to a text file");

    m.def("simulateStep", [](std::shared_ptr<Brain> brain, 
                             size_t stepNumber,
                             const std::vector<float>& sensoryInput = std::vector<float>()) {
        if (!sensoryInput.empty()) {
            // Create a simple sensory input from vector
            // This is a simplified version - real implementation would create proper SensoryInput
            brain->receiveSensoryInput(sensoryInput);
        }
        
        brain->step(stepNumber);
        return brain->getFiringNeuronCount();
    }, py::arg("brain"), py::arg("stepNumber"),
         py::arg("sensoryInput") = std::vector<float>(),
         "Run a single simulation step and return firing neuron count");

    m.def("runFullSimulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t numSteps) {
        for (size_t i = 0; i < numSteps; ++i) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(i);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
         "Run a complete simulation with world updates and actions");

    // === CONFIGURATION UTILITIES ===

    // === DOCUMENTATION ENHANCEMENT ===
    // Update module documentation with comprehensive information
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides access to the complete NLM system including:
        - Brain simulation with spiking neurons and plasticity
        - Integrated memory systems (working, episodic, associative)
        - Prediction and cognitive systems
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Motor control and action systems
        - Environmental simulation
        - Developmental systems
        
        Key Features:
        * Real-time neural computation with event-driven dynamics
        * Multi-system integration (memory, prediction, cognition)
        * Neuromodulated learning (reward, curiosity, novelty)
        * Developmental plasticity and maturation
        * Checkpoint/restore for long-running simulations
        * Comprehensive statistics and monitoring
        
        Usage Examples:
        >>> import pynlm
        >>> 
        >>> # Create a simple brain
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Run a simulation step
        >>> brain.step(0)
        >>> 
        >>> # Get statistics
        >>> neurons = brain.getTotalNeuronCount()
        >>> spikes = brain.getTotalSpikeCount()
        >>> 
        >>> # Advanced usage with agent and world
        >>> world = pynlm.createSimpleWorld()
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        >>> 
        >>> # Run complete episode
        >>> for step in range(100):
        ...     world.update(0.1)
        ...     percept = world.getSensoryPercept()
        ...     agent.processSensoryInput(percept)
        ...     brain.step(step)
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
        
        For more information, see:
        * docs/ARCHITECTURE.md - Technical architecture
        * docs/PHASE6_FINAL_AUDIT.md - Implementation details
        * easy_usage.md - Quick start guide
        * The NLM project documentation
        
        Advanced users can access:
        * Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
        * Memory systems (working, episodic, associative)
        * Cognitive systems (planner, concept formation, attention)
        * Development system for brain maturation
        * Plasticity rules (STDP, Hebbian, structural)
        * Checkpoint/restore for simulation persistence
        * Advanced configuration generation and analysis
        
        The module supports:
        * Real-time simulation with configurable timestep
        * Multi-step episodes with memory consolidation
        * Reward-based learning and exploration
        * Development stages from initial to aging
        * Complex environmental interactions
        * Statistical monitoring and analysis
    )pbdoc";

    // === IMPROVED CLASS DOCUMENTATION ===
    // Add comprehensive documentation for all classes
    
    // NeuronId class
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron in the brain network.
        
        NeuronId provides a stable, immutable identifier for neurons that persist
        across simulation steps and checkpoints. Used throughout the API to
        reference specific neurons for sensory input, current injection, and
        connection management.
        
        Attributes:
            value (uint64_t): Internal integer identifier for the neuron
            
        Example:
            >>> import pynlm
            >>> # Neuron IDs are created internally during brain initialization
            >>> # They can be used to reference specific neurons
            >>> brain = pynlm.createBrain(config)
            >>> # Access neurons through regions and populations
    )pbdoc")
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

    // SynapseId class
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synaptic connection between neurons.
        
        SynapseId identifies specific connections in the neural network,
        allowing precise control over synaptic weights, delays, and
        plasticity rules. Essential for advanced connection management
        and targeted plasticity experiments.
        
        Attributes:
            value (uint64_t): Internal integer identifier for the synapse
            
        Example:
            >>> import pynlm
            >>> # Synapse IDs are used for connection management
            >>> # and plasticity experiments
            >>> # Connections are created automatically during initialization
            >>> # but can be modified through advanced APIs
    )pbdoc")
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

    // RegionId class
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a neural region in the brain.
        
        RegionId identifies anatomical or functional regions of the brain,
        allowing regional-specific operations like attention focusing,
        regional development, and localized plasticity experiments.
        
        Attributes:
            value (uint64_t): Internal integer identifier for the region
            
        Example:
            >>> import pynlm
            >>> # Regions are created during brain initialization
            >>> # They organize neurons into functional groups
            >>> region_id = brain.addRegion("Visual_Cortex")
    )pbdoc")
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

    // PopulationId class
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population within a region.
        
        PopulationId identifies groups of neurons that share similar
        properties (type, function, or connectivity patterns). Populations
        are fundamental organizational units in NLM, allowing control over
        large groups of neurons simultaneously.
        
        Attributes:
            value (uint64_t): Internal integer identifier for the population
            
        Example:
            >>> import pynlm
            >>> # Populations organize neurons by type and function
            >>> # Common types: Sensory, Internal, Motor
            >>> sensory_pop = region.addPopulation(100, NeuronType.Sensory)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    // Enhanced documentation for existing classes
    // (Continuing from previous bindings with improved documentation)
    
    // Config class
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration object for NLM brain simulation parameters.
        
        Config provides a flexible key-value store for simulation parameters
        that control brain architecture, learning dynamics, and system behavior.
        Supports various data types (integers, floats, strings, vectors)
        with type-safe access methods and validation.
        
        Common Configuration Parameters:
        * neuron_count: Total number of neurons in the brain
        * region_count: Number of brain regions
        * connection_probability: Probability of initial connectivity
        * simulation_timestep: Time step duration for simulation
        * random_seed: Random number generator seed
        
        Methods:
        * loadFromFile: Load configuration from JSON file
        * loadFromArgs: Parse configuration from command-line arguments
        * saveToFile: Export configuration to JSON file
        * getKeys: List all configuration keys
        * has: Check if configuration key exists
        * summary: Get human-readable configuration summary
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("neuron_count", 2000)
            >>> config.set("region_count", 2)
            >>> brain = pynlm.createBrain(config)
    )pbdoc")
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

    // Brain class with enhanced documentation
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain with spiking neurons and plasticity.
        
        Brain implements the core neural computation engine of NLM, featuring:
        * Event-driven spiking neuron dynamics
        * Multiple plasticity rules (STDP, Hebbian)
        * Structural plasticity (synaptogenesis, pruning)
        * Integrated memory systems
        * Neuromodulated learning
        * Developmental plasticity
        
        The brain operates in discrete time steps, simulating neural activity
        through leaky integrate-and-fire neuron models and synaptic transmission.
        
        Key Methods:
        * initialize(): Setup brain with configuration
        * step(): Advance simulation by one time step
        * receiveSensoryInput(): Inject sensory signals
        * produceAction(): Generate motor commands
        * save(): Checkpoint brain state
        * load(): Restore brain state
        
        Statistics:
        * getTotalNeuronCount(): Total neurons in brain
        * getTotalSynapseCount(): Total synapses
        * getActiveNeuronCount(): Currently active neurons
        * getFiringNeuronCount(): Neurons firing this step
        * getTotalSpikeCount(): Cumulative spikes
        * getAverageFiringRate(): Population firing rate
        * getExcitationInhibitionRatio(): Network balance
        
        Development:
        * getDevelopmentalStage(): Current development phase
        * setDevelopmentalStage(): Manually control development
        
        Integration Systems:
        * Memory systems (working, episodic, associative)
        * Prediction system for forward models
        * Cognitive systems (planner, concept formation)
        * Neuromodulation (dopamine, curiosity, novelty)
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
            >>> neurons = brain.getTotalNeuronCount()
            >>> spikes = brain.getTotalSpikeCount()
    )pbdoc")
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
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
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
             "Get novelty detector")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal");

    // AgentBrain class with enhanced documentation
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to world environment.
        
        AgentBrain provides the bridge between the internal neural brain and
        the external world, handling sensory processing and motor output.
        Implements the perception-action cycle fundamental to intelligent
        behavior in NLM.
        
        The agent processes sensory input from the environment and translates
        neural activity into appropriate actions, implementing embodied cognition
        through the interaction with the world.
        
        Key Methods:
        * initialize(): Setup agent with world
        * processSensoryInput(): Receive and process environmental input
        * decodeMotorCommand(): Translate neural activity to actions
        * applyRewardModulation(): Learn from environmental feedback
        * updateDevelopment(): Update developmental state
        * enableRewardModulation(): Enable/disable reward learning
        * enableCuriosity(): Enable/disable exploration motivation
        * enableDevelopment(): Enable/disable developmental processes
        * enableStructuralPlasticity(): Enable/disable connection growth
        
        Sensory-Motor Loop:
        1. World updates (time progression)
        2. Get sensory percept (what agent perceives)
        3. Process sensory input (brain receives input)
        4. Brain thinks (neural computation)
        5. Decode motor command (brain decides action)
        6. Apply action in world (agent acts)
        7. Apply reward modulation (learning from consequences)
        
        Example:
            >>> import pynlm
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(20, 20, 8, 8)
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> 
            >>> # Run simulation
            >>> for step in range(100):
            ...     world.update(0.1)
            ...     percept = world.getSensoryPercept()
            ...     agent.processSensoryInput(percept)
            ...     brain.step(step)
            ...     action = agent.decodeMotorCommand()
            ...     world.applyMotorCommand(action, world.getSimulationTime())
    )pbdoc")
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

    // === ADVANCED SIMULATION FEATURES (continued) ===
    m.def("createBrainAdvanced", [](std::shared_ptr<Config> config, 
                                    bool enableRewardModulation = true,
                                    bool enableCuriosity = true,
                                    bool enableDevelopment = true,
                                    bool enableStructuralPlasticity = true) {
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        if (enableRewardModulation) {
            agent->enableRewardModulation(true);
        }
        if (enableCuriosity) {
            agent->enableCuriosity(true);
        }
        if (enableDevelopment) {
            agent->enableDevelopment(true);
        }
        if (enableStructuralPlasticity) {
            agent->enableStructuralPlasticity(true);
        }
        
        return std::make_tuple(brain, agent);
    }, py::arg("config"), py::arg("enableRewardModulation") = true, 
         py::arg("enableCuriosity") = true, py::arg("enableDevelopment") = true,
         py::arg("enableStructuralPlasticity") = true,
         "Create brain with advanced configuration and return (brain, agent)");

    m.def("saveBrainSnapshot", [](std::shared_ptr<Brain> brain, const std::string& directory) {
        std::string filepath = directory + "/brain_snapshot_" + 
            std::to_string(brain->getTotalSpikeCount()) + ".bin";
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("directory"),
         "Save brain snapshot to file with timestamp in filename");

    m.def("createBrainCheckpoint", [](std::shared_ptr<Config> config, 
                                       const std::string& checkpointDir,
                                       size_t maxCheckpoints = 10) {
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        // Configure checkpoint system
        brain->getConfig()->set<std::string>("checkpoint_dir", checkpointDir);
        brain->getConfig()->set<size_t>("max_checkpoints", maxCheckpoints);
        
        return std::make_tuple(brain, agent);
    }, py::arg("config"), py::arg("checkpointDir"), py::arg("maxCheckpoints") = 10,
         "Create brain with checkpoint system enabled");

    m.def("generateBrainConfig", [](size_t neuronCount = 1000,
                                    size_t regionCount = 1,
                                    float connectionProbability = 0.1f,
                                    float initialWeightMean = 0.2f,
                                    float initialWeightStd = 0.1f) {
        auto config = createDefaultConfig();
        
        config->set<size_t>("neuron_count", neuronCount);
        config->set<size_t>("region_count", regionCount);
        config->set<float>("connection_probability", connectionProbability);
        config->set<float>("initial_weight_mean", initialWeightMean);
        config->set<float>("initial_weight_std", initialWeightStd);
        
        return config;
    }, py::arg("neuronCount") = 1000, py::arg("regionCount") = 1,
         py::arg("connectionProbability") = 0.1f,
         py::arg("initialWeightMean") = 0.2f, py::arg("initialWeightStd") = 0.1f,
         "Generate a custom brain configuration");

    m.def("analyzeBrainConnectivity", [](std::shared_ptr<Brain> brain) {
        // Return connectivity analysis
        struct ConnectivityAnalysis {
            size_t totalNeurons;
            size_t totalSynapses;
            float excitationInhibitionRatio;
            size_t activeNeuronCount;
            float averageFiringRate;
        };
        
        ConnectivityAnalysis analysis;
        analysis.totalNeurons = brain->getTotalNeuronCount();
        analysis.totalSynapses = brain->getTotalSynapseCount();
        analysis.excitationInhibitionRatio = brain->getExcitationInhibitionRatio();
        analysis.activeNeuronCount = brain->getActiveNeuronCount();
        analysis.averageFiringRate = brain->getAverageFiringRate();
        
        return analysis;
    }, py::arg("brain"),
         "Analyze and return brain connectivity statistics");

    m.def("exportBrainStats", [](std::shared_ptr<Brain> brain, 
                                 const std::string& filename) {
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot open file for writing: " + filename);
        }
        
        file << "=== NLM BRAIN STATISTICS ===" << std::endl;
        file << "Total Neurons: " << brain->getTotalNeuronCount() << std::endl;
        file << "Total Synapses: " << brain->getTotalSynapseCount() << std::endl;
        file << "Active Neurons: " << brain->getActiveNeuronCount() << std::endl;
        file << "Firing Neurons: " << brain->getFiringNeuronCount() << std::endl;
        file << "Total Spikes: " << brain->getTotalSpikeCount() << std::endl;
        file << "Average Firing Rate: " << brain->getAverageFiringRate() << std::endl;
        file << "Excitation/Inhibition Ratio: " << brain->getExcitationInhibitionRatio() << std::endl;
        file << "Developmental Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << std::endl;
        file << "Pending Spike Events: " << brain->getPendingSpikeEventCount() << std::endl;
        
        file.close();
        return true;
    }, py::arg("brain"), py::arg("filename"),
         "Export brain statistics to a text file");

    m.def("simulateStep", [](std::shared_ptr<Brain> brain, 
                             size_t stepNumber,
                             const std::vector<float>& sensoryInput = std::vector<float>()) {
        if (!sensoryInput.empty()) {
            // Create a simple sensory input from vector
            // This is a simplified version - real implementation would create proper SensoryInput
            brain->receiveSensoryInput(sensoryInput);
        }
        
        brain->step(stepNumber);
        return brain->getFiringNeuronCount();
    }, py::arg("brain"), py::arg("stepNumber"),
         py::arg("sensoryInput") = std::vector<float>(),
         "Run a single simulation step and return firing neuron count");

    m.def("runFullSimulation", [](std::shared_ptr<Brain> brain,
                                   std::shared_ptr<SimpleWorld> world,
                                   std::shared_ptr<AgentBrain> agent,
                                   size_t numSteps) {
        for (size_t i = 0; i < numSteps; ++i) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            auto percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain thinks
            brain->step(i);
            
            // Get action
            auto action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
        }
        
        return brain->getTotalSpikeCount();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"),
         "Run a complete simulation with world updates and actions");

    // === ADVANCED BRAIN CREATION ===
    m.def("createBrainAdvanced", [](std::shared_ptr<Config> config, 
                                    bool enableRewardModulation = true,
                                    bool enableCuriosity = true,
                                    bool enableDevelopment = true,
                                    bool enableStructuralPlasticity = true) {
        auto brain = createBrain(config);
        auto agent = createAgentBrain(brain);
        
        if (enableRewardModulation) {
            agent->enableRewardModulation(true);
        }
        if (enableCuriosity) {
            agent->enableCuriosity(true);
        }
        if (enableDevelopment) {
            agent->enableDevelopment(true);
        }
        if (enableStructuralPlasticity) {
            agent->enableStructuralPlasticity(true);
        }
        
        return std::make_tuple(brain, agent);
    }, py::arg("config"), py::arg("enableRewardModulation") = true, 
         py::arg("enableCuriosity") = true, py::arg("enableDevelopment") = true,
         py::arg("enableStructuralPlasticity") = true,
         "Create brain with advanced configuration and return (brain, agent)");
}
