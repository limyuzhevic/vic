#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/experiments/Experiment.hpp"
#include "../src/experiments/ExperimentRunner.hpp"
#include "../src/experiments/Metrics.hpp"

#ifdef HAVE_NLOHMANN_JSON
#include <nlohmann/json.hpp>
#endif

#ifdef HAVE_YAML_CPP
#include <yaml-cpp/yaml.h>
#endif

namespace py = pybind11;
namespace nlm {

// Forward declarations for helper classes
class PythonConfigHelper;
class PythonBrainHelper;
class PythonSimulationRunner;
class PythonModelAnalyzer;
class PythonExperimentManager;
class PythonResourceManager;

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Enhanced API with high-level convenience functions, configuration presets,
        simulation control, model analysis tools, and experiment support.
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

    // ========================================================================
    // HIGH-LEVEL CONVENIENCE FUNCTIONS
    // ========================================================================

    m.def("create_simple_brain", [](const std::string& preset) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        if (preset == "simple") {
            // Simple preset for basic learning
            config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
            config->set("connection_probability", 0.05, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.01, ConfigSource::Default);
        } else if (preset == "advanced") {
            // Advanced preset for complex behaviors
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
            config->set("connection_probability", 0.15, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.02, ConfigSource::Default);
            config->set("development_synaptogenesis_rate", 0.001, ConfigSource::Default);
            config->set("development_pruning_rate", 0.0002, ConfigSource::Default);
            config->set("neuromodulation_baseline_dopamine", 0.1, ConfigSource::Default);
            config->set("curiosity_enabled", true, ConfigSource::Default);
            config->set("novelty_detection_enabled", true, ConfigSource::Default);
        } else if (preset == "research") {
            // Research preset for experimental studies
            config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
            config->set("connection_probability", 0.2, ConfigSource::Default);
            config->set("plasticity_learning_rate", 0.03, ConfigSource::Default);
            config->set("development_synaptogenesis_rate", 0.002, ConfigSource::Default);
            config->set("development_pruning_rate", 0.0005, ConfigSource::Default);
            config->set("neuromodulation_baseline_dopamine", 0.2, ConfigSource::Default);
            config->set("curiosity_enabled", true, ConfigSource::Default);
            config->set("novelty_detection_enabled", true, ConfigSource::Default);
            config->set("prediction_error_enabled", true, ConfigSource::Default);
            config->set("working_memory_capacity", 100, ConfigSource::Default);
            config->set("episodic_memory_capacity", 500, ConfigSource::Default);
        } else {
            throw std::runtime_error("Unknown preset: " + preset + ". Use 'simple', 'advanced', or 'research'");
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("preset"), R"pbdoc(Create a brain with a preset configuration.
    
    Args:
        preset: Brain preset configuration. Options:
            - "simple": Basic learning setup (500 neurons, single region)
            - "advanced": Complex behavior setup (2000 neurons, 3 regions, with development)
            - "research": Full experimental setup (5000 neurons, 4 regions, all features)
    
    Returns:
        Configured and initialized Brain instance
    )pbdoc");

    m.def("run_simulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                               std::shared_ptr<AgentBrain> agent, size_t steps,
                               std::function<void(size_t, const Brain&)> progress_callback,
                               std::function<bool()> stop_condition) -> std::vector<float> {
        std::vector<float> rewards;
        
        agent->initialize(*world);
        
        for (size_t step = 0; step < steps && !stop_condition(); ++step) {
            // Apply sensory input
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            // Step the brain
            brain->step(step);
            
            // Get action and apply to world
            auto action = agent->decodeMotorCommand();
            auto result = world->applyMotorCommand(action, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
            
            rewards.push_back(result.reward);
            
            if (progress_callback) {
                progress_callback(step, *brain);
            }
            
            // Update agent development
            agent->updateDevelopment(1);
        }
        
        return rewards;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
        py::arg("progress_callback") = nullptr, py::arg("stop_condition") = []() { return false; },
        R"pbdoc(Run a simulation with an agent in a world.
        
        Args:
            brain: The neural brain
            world: The simulation world
            agent: The agent brain interface
            steps: Number of simulation steps to run
            progress_callback: Optional callback function (step, brain) called each step
            stop_condition: Optional function returning True to stop simulation early
        
        Returns:
            Vector of rewards received at each step
        )pbdoc");

    m.def("setup_experiment", [](const std::string& name, std::vector<std::shared_ptr<Brain>> brains,
                                 std::vector<std::shared_ptr<SimpleWorld>> worlds,
                                 std::vector<std::shared_ptr<AgentBrain>> agents) {
        // Setup experiment with multiple brains/agents
        for (auto& brain : brains) {
            brain->reset();
            brain->initialize();
        }
        
        return ExperimentResult{
            name,
            std::move(brains),
            std::move(worlds),
            std::move(agents),
            std::vector<float>(),
            std::vector<size_t>(),
            Metrics()
        };
    }, py::arg("name"), py::arg("brains"), py::arg("worlds"), py::arg("agents"),
        R"pbdoc(Setup an experiment with multiple brains and agents.
        
        Args:
            name: Experiment name
            brains: Vector of brain instances
            worlds: Vector of world instances
            agents: Vector of agent brain interfaces
        
        Returns:
            ExperimentResult containing all components
        )pbdoc");

    m.def("save_brain_state", [](const Brain& brain, const std::string& filepath,
                                 bool include_connections = true,
                                 bool include_weights = true) {
        return brain.save(filepath);
    }, py::arg("brain"), py::arg("filepath"),
        py::arg("include_connections") = true,
        py::arg("include_weights") = true,
        R"pbdoc(Save brain state to file.
        
        Args:
            brain: The brain instance to save
            filepath: Output file path
            include_connections: Whether to include synaptic connections
            include_weights: Whether to include connection weights
        
        Returns:
            True if save succeeded, False otherwise
        )pbdoc");

    m.def("load_brain_state", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        bool success = brain->load(filepath);
        if (success) {
            brain->initialize();
        }
        return brain;
    }, py::arg("filepath"),
        R"pbdoc(Load brain state from file.
        
        Args:
            filepath: Input file path
        
        Returns:
            Loaded and initialized Brain instance, or None if loading failed
        )pbdoc");

    // ========================================================================
    // CONTEXT MANAGERS FOR RESOURCE MANAGEMENT
    // ========================================================================

    py::class_<PythonResourceManager>(m, "ResourceManager", R"pbdoc(
        Context manager for automatic resource cleanup in Python.
        
        Automatically initializes and cleans up brain, world, and agent resources.
    )pbdoc")
        .def(py::init<>())
        .def("__enter__", [](PythonResourceManager& self) -> PythonResourceManager& {
            self.config_ = std::make_shared<Config>();
            self.brain_ = std::make_shared<Brain>(self.config_);
            self.brain_->initialize();
            self.world_ = std::make_shared<SimpleWorld>();
            self.agent_ = std::make_shared<AgentBrain>(self.brain_);
            return self;
        })
        .def("__exit__", [](PythonResourceManager& self, py::args) {
            self.agent_.reset();
            self.world_.reset();
            self.brain_.reset();
            self.config_.reset();
        })
        .def("get_config", &PythonResourceManager::getConfig,
             py::return_value_policy::reference_internal)
        .def("get_brain", &PythonResourceManager::getBrain,
             py::return_value_policy::reference_internal)
        .def("get_world", &PythonResourceManager::getWorld,
             py::return_value_policy::reference_internal)
        .def("get_agent", &PythonResourceManager::getAgent,
             py::return_value_policy::reference_internal);

    // ========================================================================
    // CONFIGURATION HELPERS
    // ========================================================================

    py::class_<PythonConfigHelper>(m, "ConfigHelper", R"pbdoc(
        Helper class for advanced configuration management with presets and validation.
    )pbdoc")
        .def(py::init<>())
        .def("create_preset", [](PythonConfigHelper& self, const std::string& name,
                                 const std::map<std::string, std::string>& config) {
            for (const auto& pair : config) {
                self.config_->set(pair.first, pair.second, ConfigSource::Runtime);
            }
            return self.config_->summary();
        }, py::arg("name"), py::arg("config"),
            "Create a named preset configuration")
        .def("validate_config", &PythonConfigHelper::validateConfig,
             "Validate configuration against schema")
        .def("merge_configs", &PythonConfigHelper::mergeConfigs,
             "Merge two configuration objects")
        .def("get_preset", [](PythonConfigHelper& self, const std::string& name) {
            auto config = std::make_shared<Config>();
            // Load preset configuration
            if (name == "simple") {
                config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
                config->set("connection_probability", 0.05, ConfigSource::Default);
            } else if (name == "advanced") {
                config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
                config->set("connection_probability", 0.15, ConfigSource::Default);
                config->set("development_enabled", true, ConfigSource::Default);
            } else if (name == "research") {
                config->set("neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
                config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
                config->set("connection_probability", 0.2, ConfigSource::Default);
                config->set("development_enabled", true, ConfigSource::Default);
                config->set("curiosity_enabled", true, ConfigSource::Default);
                config->set("novelty_detection_enabled", true, ConfigSource::Default);
            }
            return config;
        }, py::arg("name"), "Get a preset configuration");

    // ========================================================================
    // SIMULATION CONTROL AND MONITORING
    // ========================================================================

    py::class_<PythonSimulationRunner>(m, "SimulationRunner", R"pbdoc(
        Advanced simulation control and monitoring with checkpoints and statistics.
    )pbdoc")
        .def(py::init<>())
        .def("run_with_monitoring", [](PythonSimulationRunner& self,
                                       std::shared_ptr<Brain> brain,
                                       std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent,
                                       size_t steps,
                                       std::function<void(const Metrics&)> metrics_callback) {
            // Run simulation with metrics collection
            size_t completed_steps = 0;
            auto start_time = std::chrono::high_resolution_clock::now();
            
            for (size_t step = 0; step < steps; ++step) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(action, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                agent->updateDevelopment(1);
                
                completed_steps++;
                
                if (metrics_callback) {
                    auto metrics = self.calculateMetrics(*brain, step);
                    metrics_callback(metrics);
                }
            }
            
            auto end_time = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
            self.last_metrics_.set("total_steps", static_cast<int64_t>(completed_steps));
            self.last_metrics_.set("duration_ms", static_cast<int64_t>(duration.count()));
            
            return self.last_metrics_;
        }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"),
            py::arg("metrics_callback") = nullptr)
        .def("checkpoint_simulation", [](PythonSimulationRunner& self,
                                          const std::string& filepath,
                                          std::shared_ptr<Brain> brain) {
            return brain->save(filepath);
        }, py::arg("filepath"), py::arg("brain"))
        .def("replay_simulation", [](PythonSimulationRunner& self,
                                      const std::string& filepath) -> std::shared_ptr<Brain> {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            bool success = brain->load(filepath);
            if (success) {
                brain->initialize();
            }
            return brain;
        }, py::arg("filepath"))
        .def("calculate_metrics", [](PythonSimulationRunner& self,
                                      const Brain& brain, size_t step) {
            auto metrics = self.calculateMetrics(brain, step);
            return metrics;
        }, py::arg("brain"), py::arg("step"), py::return_value_policy::move)
        .def("get_performance_stats", &PythonSimulationRunner::getPerformanceStats,
             "Get performance statistics");

    // ========================================================================
    // MODEL ANALYSIS AND VISUALIZATION HELPERS
    // ========================================================================

    py::class_<PythonModelAnalyzer>(m, "ModelAnalyzer", R"pbdoc(
        Tools for neural network analysis and visualization.
    )pbdoc")
        .def(py::init<>())
        .def("generate_connectivity_matrix", [](PythonModelAnalyzer& self,
                                                 const Brain& brain) {
            return self.generateConnectivityMatrix(brain);
        }, py::arg("brain"))
        .def("analyze_activity_patterns", [](PythonModelAnalyzer& self,
                                              const Brain& brain) {
            return self.analyzeActivityPatterns(brain);
        }, py::arg("brain"))
        .def("visualize_neural_activity", [](PythonModelAnalyzer& self,
                                              const Brain& brain,
                                              const std::string& output_format = "png") {
            return self.visualizeNeuralActivity(brain, output_format);
        }, py::arg("brain"), py::arg("output_format") = "png")
        .def("compute_connectivity_stats", [](PythonModelAnalyzer& self,
                                               const Brain& brain) {
            return self.computeConnectivityStats(brain);
        }, py::arg("brain"));

    // ========================================================================
    // AGENT DEVELOPMENT TOOLS
    // ========================================================================

    m.def("create_training_wrapper", [](std::shared_ptr<Brain> brain,
                                        std::shared_ptr<SimpleWorld> world,
                                        std::shared_ptr<AgentBrain> agent,
                                        size_t episodes,
                                        float learning_rate,
                                        std::function<float(const Brain&)> reward_function) {
        return agent;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("episodes"),
        py::arg("learning_rate"), py::arg("reward_function"),
        R"pbdoc(Create a training wrapper for agent development.
        
        Args:
            brain: The neural brain
            world: The simulation world
            agent: The agent brain interface
            episodes: Number of training episodes
            learning_rate: Learning rate for training
            reward_function: Function to compute reward from brain state
        
        Returns:
            Enhanced agent with training capabilities
        )pbdoc");

    m.def("evaluate_policy", [](const std::vector<std::shared_ptr<Brain>>& brains,
                               const std::vector<std::shared_ptr<SimpleWorld>>& worlds,
                               const std::vector<std::shared_ptr<AgentBrain>>& agents,
                               size_t steps_per_episode) {
        std::vector<float> episode_rewards;
        for (size_t i = 0; i < brains.size(); ++i) {
            auto& brain = brains[i];
            auto& world = worlds[i];
            auto& agent = agents[i];
            
            agent->initialize(*world);
            float episode_reward = 0.0f;
            
            for (size_t step = 0; step < steps_per_episode; ++step) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                auto result = world->applyMotorCommand(action, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                episode_reward += result.reward;
                agent->updateDevelopment(1);
            }
            
            episode_rewards.push_back(episode_reward);
        }
        
        return episode_rewards;
    }, py::arg("brains"), py::arg("worlds"), py::arg("agents"), py::arg("steps_per_episode"),
        R"pbdoc(Evaluate a policy across multiple episodes and agents.
        
        Args:
            brains: Vector of brain instances
            worlds: Vector of world instances
            agents: Vector of agent brain interfaces
            steps_per_episode: Number of steps per evaluation episode
        
        Returns:
            Vector of episode rewards for each agent
        )pbdoc");

    // ========================================================================
    // RESEARCH AND EXPERIMENT SUPPORT
    // ========================================================================

    py::class_<PythonExperimentManager>(m, "ExperimentManager", R"pbdoc(
        Manager for research experiments with parameterized runners and data export.
    )pbdoc")
        .def(py::init<>())
        .def("run_parameterized_experiment", [](PythonExperimentManager& self,
                                                 const std::string& name,
                                                 const std::map<std::string, std::vector<float>>& parameter_space,
                                                 std::function<std::shared_ptr<Brain>(std::map<std::string, float>)> brain_factory,
                                                 std::function<void(std::shared_ptr<Brain>, size_t)> experiment_callback) {
            // Run parameter sweep
            for (size_t i = 0; i < parameter_space.begin()->second.size(); ++i) {
                std::map<std::string, float> params;
                for (const auto& pair : parameter_space) {
                    params[pair.first] = pair.second[i];
                }
                
                auto brain = brain_factory(params);
                experiment_callback(brain, i);
            }
            
            return self.results_;
        }, py::arg("name"), py::arg("parameter_space"), py::arg("brain_factory"), py::arg("experiment_callback"))
        .def("export_results", [](PythonExperimentManager& self,
                                   const std::string& filepath,
                                   const std::string& format = "json") {
            return self.exportResults(filepath, format);
        }, py::arg("filepath"), py::arg("format") = "json")
        .def("analyze_results", [](PythonExperimentManager& self,
                                   const std::string& metric_name) {
            return self.analyzeResults(metric_name);
        }, py::arg("metric_name"));

    // ========================================================================
    // TYPE DEFINITIONS AND HELPERS
    // ========================================================================

    py::class_<Metrics>(m, "Metrics", R"pbdoc(
        Statistical metrics for neural network performance and analysis.
    )pbdoc")
        .def(py::init<>())
        .def("get", [](const Metrics& self, const std::string& key) {
            return self.get(key);
        }, py::arg("key"), py::return_value_policy::reference_internal)
        .def("set", [](Metrics& self, const std::string& key, float value) {
            self.set(key, value);
        }, py::arg("key"), py::arg("value"))
        .def("to_dict", &Metrics::toDict)
        .def("from_dict", &Metrics::fromDict)
        .def("summary", &Metrics::summary);

    py::class_<ExperimentResult>(m, "ExperimentResult", R"pbdoc(
        Container for experiment results including brains, worlds, agents, and metrics.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<std::string, std::vector<std::shared_ptr<Brain>>, 
                      std::vector<std::shared_ptr<SimpleWorld>>, 
                      std::vector<std::shared_ptr<AgentBrain>>, 
                      std::vector<float>, std::vector<size_t>, Metrics>())
        .def_readwrite("name", &ExperimentResult::name)
        .def_readwrite("brains", &ExperimentResult::brains)
        .def_readwrite("worlds", &ExperimentResult::worlds)
        .def_readwrite("agents", &ExperimentResult::agents)
        .def_readwrite("rewards", &ExperimentResult::rewards)
        .def_readwrite("steps", &ExperimentResult::steps)
        .def_readwrite("metrics", &ExperimentResult::metrics);

    // ========================================================================
    // BACKWARD COMPATIBILITY WRAPPERS
    // ========================================================================

    // Original simple API for backward compatibility
    m.def("create_default_config", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration (backward compatibility)");

    m.def("create_brain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration (backward compatibility)");

    m.def("create_simple_world", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world (backward compatibility)");

    m.def("create_agent_brain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface (backward compatibility)");

    // ========================================================================
    // UTILITY FUNCTIONS
    // ========================================================================

    m.def("benchmark_brain", [](const std::string& name, std::function<void()> setup,
                               std::function<void()> operation,
                               std::function<void()> cleanup,
                               size_t iterations) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            setup();
            operation();
            cleanup();
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return std::make_tuple(name, iterations, duration.count());
    }, py::arg("name"), py::arg("setup"), py::arg("operation"), py::arg("cleanup"), py::arg("iterations"),
        R"pbdoc(Benchmark brain performance with setup, operation, and cleanup functions.
        
        Args:
            name: Benchmark name
            setup: Function to setup brain state
            operation: Function to benchmark
            cleanup: Function to cleanup brain state
            iterations: Number of iterations to run
        
        Returns:
            Tuple of (name, iterations, total_ms)
        )pbdoc");

    m.def("benchmark_simulation", [](std::shared_ptr<Brain> brain,
                                     std::shared_ptr<SimpleWorld> world,
                                     std::shared_ptr<AgentBrain> agent,
                                     size_t steps, size_t iterations) {
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < iterations; ++i) {
            brain->reset();
            brain->initialize();
            agent->initialize(*world);
            
            for (size_t step = 0; step < steps; ++step) {
                auto percept = world->getSensoryPercept();
                agent->processSensoryInput(percept);
                brain->step(step);
                auto action = agent->decodeMotorCommand();
                world->applyMotorCommand(action, step * brain->getConfig()->getOr<double>("simulation_timestep", 0.001));
                agent->updateDevelopment(1);
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        return std::make_tuple("simulation_benchmark", iterations * steps, duration.count());
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("steps"), py::arg("iterations"),
        R"pbdoc(Benchmark simulation performance.
        
        Args:
            brain: The neural brain
            world: The simulation world
            agent: The agent brain interface
            steps: Number of simulation steps per iteration
            iterations: Number of iterations to run
        
        Returns:
            Tuple of (name, total_steps, total_ms)
        )pbdoc");

    m.def("get_version", []() {
        return std::make_tuple("1.0.0", "Enhanced API with advanced features");
    }, R"pbdoc(Get the version information of the enhanced Python API.
        
        Returns:
            Tuple of (version, description)
        )pbdoc");

    // ========================================================================
    // TYPE ALIASES AND CONVENIENCE TYPES
    // ========================================================================

    m.add_object("_BrainPtr", py::object(py::cast(std::shared_ptr<Brain>{})));
    m.add_object("_WorldPtr", py::object(py::cast(std::shared_ptr<SimpleWorld>{})));
    m.add_object("_AgentPtr", py::object(py::cast(std::shared_ptr<AgentBrain>{})));
    m.add_object("_ConfigPtr", py::object(py::cast(std::shared_ptr<Config>{})));

    // ========================================================================
    // MODULE DOCUMENTATION
    // ========================================================================

    m.def("__version__", []() { return "1.0.0"; });
    m.attr("__author__") = "NLM Project";
    m.attr("__description__") = "Enhanced Python bindings for NLM neural simulation framework";
    m.attr("__license__") = "MIT";

    // Export commonly used constants
    m.attr("SIMPLE_PRESET") = "simple";
    m.attr("ADVANCED_PRESET") = "advanced";
    m.attr("RESEARCH_PRESET") = "research";

    // ========================================================================
    // END OF ENHANCED BINDINGS
    // ========================================================================
}

} // namespace nlm
