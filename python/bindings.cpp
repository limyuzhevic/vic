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
#include "../src/core/PythonBindingsUtils.hpp"

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

    // Additional Python-specific exception types
    py::register_exception<std::invalid_argument>(m, "InvalidArgumentError");
    py::register_exception<std::out_of_range>(m, "OutOfRangeError");

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
             "Log brain status")

        // Advanced Brain methods
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get the spike system for direct neural dynamics control")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get the STDP plasticity rule for advanced synaptic manipulation")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get the Hebbian plasticity rule for association learning")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get the structural plasticity system for neural rewiring")
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get the working memory system for transient information storage")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get the episodic memory system for experience storage")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get the associative memory system for pattern relationships")
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get the prediction system for forward modeling")
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get the neural planner for action planning")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get the concept formation system for pattern discovery")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get the attentional selection system for focus management")
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get the development system for age-dependent plasticity")
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get the dopamine neuromodulation system for reward processing")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get the curiosity neuromodulation system for exploration")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get the novelty detection system for surprise processing")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get the prediction error system for learning signals")
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get the random generator for controlled randomness")
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get count of pending spike events")

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

        // Enhanced AgentBrain interface for power users
        .def("applyNeuromodulator", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply neuromodulator signal (dopamine, acetylcholine, etc.)")
        .def("setDevelopmentParameters", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update development with advanced parameters")
        .def("getAdvancedPredictionError", &AgentBrain::getPredictionError,
             "Get detailed prediction error with variance")
        .def("setCuriosityParameters", [](AgentBrain& self, float explorationWeight, float noveltyThreshold) {
             // This would need to be implemented in AgentBrain for more complex curiosity control
             return 0.0f;
         }, py::arg("explorationWeight"), py::arg("noveltyThreshold"),
             "Set curiosity system parameters for exploration control")
        .def("getNoveltyAndCuriosityCombined", [](const AgentBrain& self) {
             return self.getNoveltyLevel() + self.getCuriosityLevel();
         }, "Get combined novelty and curiosity score for behavior selection")
        .def("setPredictionErrorThreshold", [](AgentBrain& self, float threshold) {
             // Would need implementation for advanced error handling
             return;
         }, py::arg("threshold"),
             "Set prediction error threshold for learning modulation")

    // Advanced configuration utility functions
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Basic validation: check for required keys
        const std::vector<std::string> requiredKeys = {
            "brain.neuronCount",
            "brain.simulation.timestep",
            "brain.plasticity.enable"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                warnings.push_back("Missing recommended configuration key: " + key);
            }
        }
        
        // Check for inconsistent values
        if (config.has("brain.plasticity.stdp.ltpWeight") &&
            config.has("brain.plasticity.stdp.ltdWeight")) {
            auto ltp = config.getOr<double>("brain.plasticity.stdp.ltpWeight", 0.01);
            auto ltd = config.getOr<double>("brain.plasticity.stdp.ltdWeight", 0.02);
            if (std::abs(ltp - ltd) < 0.001) {
                warnings.push_back("LTP and LTD weights are too similar - consider increasing STDP asymmetry");
            }
        }
        
        if (!errors.empty() && !warnings.empty()) {
            py::dict result;
            result["valid"] = false;
            result["errors"] = errors;
            result["warnings"] = warnings;
            return result;
        }
        
        py::dict result;
        result["valid"] = true;
        result["warnings"] = warnings;
        return result;
    }, py::arg("config"), "Validate configuration and return validation results with warnings")
    
    .def("mergeConfigs", [](const Config& baseConfig, const Config& overrideConfig) {
        auto merged = baseConfig;
        const auto overrideKeys = overrideConfig.getKeys();
        for (const auto& key : overrideKeys) {
            auto value = overrideConfig.getOr<ConfigValue>(key, ConfigValue());
            merged.set(key, value);
        }
        return merged;
    }, py::arg("baseConfig"), py::arg("overrideConfig"),
        "Merge two configurations (overrideConfig takes precedence)")
    
    .def("extractConfigSubset", [](const Config& config, const std::vector<std::string>& keys) {
        Config subset;
        for (const auto& key : keys) {
            if (config.has(key)) {
                auto value = config.getOr<ConfigValue>(key, ConfigValue());
                subset.set(key, value);
            }
        }
        return subset;
    }, py::arg("config"), py::arg("keys"), "Extract a subset of configuration keys")
    
    .def("findConflictingConfigs", [](const Config& config) {
        std::vector<std::string> conflicts;
        
        // Check for conflicting plasticity settings
        if (config.has("brain.plasticity.enable") &&
            config.has("brain.plasticity.stdp.enable") &&
            config.has("brain.plasticity.hebbian.enable")) {
            
            auto stdpEnabled = config.getOr<bool>("brain.plasticity.stdp.enable", false);
            auto hebbianEnabled = config.getOr<bool>("brain.plasticity.hebbian.enable", false);
            auto structuralEnabled = config.getOr<bool>("brain.plasticity.structural.enable", false);
            
            int enabledCount = (stdpEnabled ? 1 : 0) + (hebbianEnabled ? 1 : 0) + (structuralEnabled ? 1 : 0);
            if (enabledCount > 2) {
                conflicts.push_back("Multiple plasticity rules enabled - may cause interference");
            }
        }
        
        // Check for incompatible neuromodulation settings
        if (config.has("brain.neuromodulation.dopamine.enable") &&
            config.has("brain.neuromodulation.curiosity.enable") &&
            config.has("brain.neuromodulation.novelty.enable")) {
            
            auto dopWeight = config.getOr<double>("brain.neuromodulation.dopamine.weight", 1.0);
            auto curWeight = config.getOr<double>("brain.neuromodulation.curiosity.weight", 1.0);
            auto novWeight = config.getOr<double>("brain.neuromodulation.novelty.weight", 1.0);
            
            double totalWeight = dopWeight + curWeight + novWeight;
            if (std::abs(totalWeight - 1.0) > 0.1) {
                conflicts.push_back("Neuromodulation weights don't sum to 1.0 - may cause imbalance");
            }
        }
        
        return conflicts;
    }, py::arg("config"), "Find potential configuration conflicts")

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

    // Advanced configuration utility functions
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        std::vector<std::string> warnings;
        
        // Basic validation: check for required keys
        const std::vector<std::string> requiredKeys = {
            "brain.neuronCount",
            "brain.simulation.timestep",
            "brain.plasticity.enable"
        };
        
        for (const auto& key : requiredKeys) {
            if (!config.has(key)) {
                warnings.push_back("Missing recommended configuration key: " + key);
            }
        }
        
        // Check for inconsistent values
        if (config.has("brain.plasticity.stdp.ltpWeight") &&
            config.has("brain.plasticity.stdp.ltdWeight")) {
            auto ltp = config.getOr<double>("brain.plasticity.stdp.ltpWeight", 0.01);
            auto ltd = config.getOr<double>("brain.plasticity.stdp.ltdWeight", 0.02);
            if (std::abs(ltp - ltd) < 0.001) {
                warnings.push_back("LTP and LTD weights are too similar - consider increasing STDP asymmetry");
            }
        }
        
        if (!errors.empty() && !warnings.empty()) {
            py::dict result;
            result["valid"] = false;
            result["errors"] = errors;
            result["warnings"] = warnings;
            return result;
        }
        
        py::dict result;
        result["valid"] = true;
        result["warnings"] = warnings;
        return result;
    }, py::arg("config"), "Validate configuration and return validation results with warnings")
    
    .def("mergeConfigs", [](const Config& baseConfig, const Config& overrideConfig) {
        auto merged = baseConfig;
        const auto overrideKeys = overrideConfig.getKeys();
        for (const auto& key : overrideKeys) {
            auto value = overrideConfig.getOr<ConfigValue>(key, ConfigValue());
            merged.set(key, value);
        }
        return merged;
    }, py::arg("baseConfig"), py::arg("overrideConfig"),
        "Merge two configurations (overrideConfig takes precedence)")
    
    .def("extractConfigSubset", [](const Config& config, const std::vector<std::string>& keys) {
        Config subset;
        for (const auto& key : keys) {
            if (config.has(key)) {
                auto value = config.getOr<ConfigValue>(key, ConfigValue());
                subset.set(key, value);
            }
        }
        return subset;
    }, py::arg("config"), py::arg("keys"), "Extract a subset of configuration keys")
    
    .def("findConflictingConfigs", [](const Config& config) {
        std::vector<std::string> conflicts;
        
        // Check for conflicting plasticity settings
        if (config.has("brain.plasticity.enable") &&
            config.has("brain.plasticity.stdp.enable") &&
            config.has("brain.plasticity.hebbian.enable")) {
            
            auto stdpEnabled = config.getOr<bool>("brain.plasticity.stdp.enable", false);
            auto hebbianEnabled = config.getOr<bool>("brain.plasticity.hebbian.enable", false);
            auto structuralEnabled = config.getOr<bool>("brain.plasticity.structural.enable", false);
            
            int enabledCount = (stdpEnabled ? 1 : 0) + (hebbianEnabled ? 1 : 0) + (structuralEnabled ? 1 : 0);
            if (enabledCount > 2) {
                conflicts.push_back("Multiple plasticity rules enabled - may cause interference");
            }
        }
        
        // Check for incompatible neuromodulation settings
        if (config.has("brain.neuromodulation.dopamine.enable") &&
            config.has("brain.neuromodulation.curiosity.enable") &&
            config.has("brain.neuromodulation.novelty.enable")) {
            
            auto dopWeight = config.getOr<double>("brain.neuromodulation.dopamine.weight", 1.0);
            auto curWeight = config.getOr<double>("brain.neuromodulation.curiosity.weight", 1.0);
            auto novWeight = config.getOr<double>("brain.neuromodulation.novelty.weight", 1.0);
            
            double totalWeight = dopWeight + curWeight + novWeight;
            if (std::abs(totalWeight - 1.0) > 0.1) {
                conflicts.push_back("Neuromodulation weights don't sum to 1.0 - may cause imbalance");
            }
        }
        
        return conflicts;
    }, py::arg("config"), "Find potential configuration conflicts")

    // Advanced simulation state analysis utilities
    m.def("analyzeBrainState", [](const Brain& brain) {
        py::dict analysis;
        
        analysis["total_neurons"] = brain.getTotalNeuronCount();
        analysis["total_synapses"] = brain.getTotalSynapseCount();
        analysis["active_neurons"] = brain.getActiveNeuronCount();
        analysis["firing_neurons"] = brain.getFiringNeuronCount();
        analysis["average_firing_rate"] = brain.getAverageFiringRate();
        analysis["excitation_inhibition_ratio"] = brain.getExcitationInhibitionRatio();
        analysis["total_spike_count"] = brain.getTotalSpikeCount();
        analysis["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        // Get memory system stats if available
        if (auto ws = brain.getWorkingMemory()) {
            analysis["working_memory_capacity"] = ws->getCapacity();
            analysis["working_memory_active_traces"] = ws->getActiveTraces();
            analysis["working_memory_activity"] = ws->getMemoryActivity();
        }
        
        if (auto em = brain.getEpisodicMemory()) {
            analysis["episodic_memory_episodes"] = em->getEpisodeCount();
        }
        
        if (auto am = brain.getAssociativeMemory()) {
            analysis["associative_memory_strength"] = 0.0f; // Would need to compute
        }
        
        if (auto dm = brain.getDevelopmentSystem()) {
            analysis["developmental_age"] = dm->getDevelopmentalAge();
            analysis["critical_period"] = dm->isCriticalPeriod();
            analysis["critical_period_progress"] = dm->getCriticalPeriodProgress();
        }
        
        return analysis;
    }, py::arg("brain"), "Analyze brain state and return comprehensive statistics")
    
    .def("compareBrains", [](const Brain& brain1, const Brain& brain2) {
        py::dict comparison;
        
        comparison["neuron_count_diff"] = static_cast<int>(brain2.getTotalNeuronCount() - brain1.getTotalNeuronCount());
        comparison["synapse_count_diff"] = static_cast<int>(brain2.getTotalSynapseCount() - brain1.getTotalSynapseCount());
        comparison["firing_rate_diff"] = brain2.getAverageFiringRate() - brain1.getAverageFiringRate();
        comparison["spike_count_diff"] = static_cast<int>(brain2.getTotalSpikeCount() - brain1.getTotalSpikeCount());
        comparison["e_i_ratio_diff"] = brain2.getExcitationInhibitionRatio() - brain1.getExcitationInhibitionRatio();
        
        return comparison;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brain states and return differences")
    
    .def("getSystemStatus", [](const Brain& brain) {
        py::dict status;
        
        // System health indicators
        status["healthy"] = true;
        status["simulation_running"] = true; // Would need actual state tracking
        
        // Performance indicators
        float firingRate = brain.getAverageFiringRate();
        status["firing_rate_healthy"] = (firingRate > 0.01 && firingRate < 100.0);
        
        // Memory health
        if (auto ws = brain.getWorkingMemory()) {
            status["working_memory_healthy"] = (ws->getMemoryActivity() < 1.0);
        }
        
        // Development health
        if (auto dm = brain.getDevelopmentSystem()) {
            status["development_healthy"] = (dm->getPlasticityModifier() > 0.0);
        }
        
        return status;
    }, py::arg("brain"), "Get comprehensive system health status")

    // ===================================================================
    // ADVANCED ERROR HANDLING AND VALIDATION FUNCTIONS
    // ===================================================================

    // Input validation helpers
    m.def("validateNeuronId", &validateNeuronId,
          py::arg("neuron_id"), "Validate a neuron ID and return True if valid");
    m.def("validateSynapseId", &validateSynapseId,
          py::arg("synapse_id"), "Validate a synapse ID and return True if valid");
    m.def("validateRegionId", &validateRegionId,
          py::arg("region_id"), "Validate a region ID and return True if valid");
    m.def("validatePopulationId", &validatePopulationId,
          py::arg("population_id"), "Validate a population ID and return True if valid");

    // Configuration validation
    m.def("validateConfig", &validateConfig,
          py::arg("config"), py::arg("warnings") = py::dict(),
          "Validate configuration with optional warnings output");
    m.def("findConflictingConfigs", &findConflictingConfigs,
          py::arg("config"), "Find potential configuration conflicts");

    // Parameter validation for simulation
    m.def("validateSimulationParameters", &validateSimulationParameters,
          py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
          "Validate simulation parameters before running");

    // Safe operation wrappers
    m.def("safeBrainStep", &safeBrainStep,
          py::arg("brain"), py::arg("step"), py::arg("time") = 0.0,
          "Safely execute a brain step with error handling and validation");
    m.def("safeBrainReset", &safeBrainReset,
          py::arg("brain"), "Safely reset brain state with error handling");
    m.def("safeBrainSave", &safeBrainSave,
          py::arg("brain"), py::arg("filepath"), "Safely save brain state with error handling");
    m.def("safeBrainLoad", &safeBrainLoad,
          py::arg("brain"), py::arg("filepath"), "Safely load brain state with error handling");

    // Error information and diagnostics
    m.def("getBrainErrorInfo", &getBrainErrorInfo,
          py::arg("brain"), "Get detailed error information and diagnostic data for brain state");

    // Safe sensory injection
    m.def("safeSensoryInjection", &safeSensoryInjection,
          py::arg("brain"), py::arg("sensory_input"), "Safely inject sensory input with validation");

    // Brain state comparison and analysis
    m.def("analyzeBrainState", &analyzeBrainState,
          py::arg("brain"), "Analyze brain state for health, performance, and issues");
    m.def("compareBrains", &compareBrains,
          py::arg("brain1"), py::arg("brain2"), "Compare two brain states and return differences");

    // Configuration utilities
    m.def("mergeConfigs", &mergeConfigs,
          py::arg("base_config"), py::arg("override_config"),
          "Merge two configurations with override precedence");
    m.def("extractConfigSubset", &extractConfigSubset,
          py::arg("config"), py::arg("keys"), "Extract subset of configuration keys");

    // Simulation control utilities
    m.def("runSafeSimulation", &runSafeSimulation,
          py::arg("brain"), py::arg("num_steps"), py::arg("check_interval") = 100,
          "Run a simulation with periodic state validation and error recovery");

    // ===================================================================
} // namespace nlm
