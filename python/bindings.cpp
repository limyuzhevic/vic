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

    // Python package version
    m.attr("__version__") = "0.1.0";

    // Configuration management utilities
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible defaults");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set minimal working configuration
        config->set("brain.neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.05f, ConfigSource::Default);
        config->set("brain.initial_weight_mean", 0.5f, ConfigSource::Default);
        config->set("brain.initial_weight_std", 0.1f, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0f, ConfigSource::Default);
        config->set("brain.v_rest", -70.0f, ConfigSource::Default);
        config->set("brain.v_reset", -75.0f, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0f, ConfigSource::Default);
        config->set("brain.tau_ref", 2.0f, ConfigSource::Default);
        return config;
    }, "Create a minimal working configuration for basic simulations");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Set configuration suitable for developmental simulations
        config->set("brain.neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("brain.synapse_density", 0.15f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.08f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("development.stage", static_cast<uint64_t>(0), ConfigSource::Default);
        return config;
    }, "Create a development-oriented configuration for agent learning");

    // Convenience configuration builders
    m.def("createSimulationConfig", [](int neuronCount = 1000, float synapseDensity = 0.1f,
                                     float connectionProbability = 0.05f,
                                     float learningRate = 0.001f) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("brain.synapse_density", synapseDensity, ConfigSource::Default);
        config->set("brain.connection_probability", connectionProbability, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", learningRate, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        return config;
    }, "Create a simulation-ready configuration with customizable parameters");

    m.def("createResearchConfig", [](int neuronCount = 5000, bool enableAllSystems = true) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("brain.synapse_density", 0.2f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.1f, ConfigSource::Default);
        
        if (enableAllSystems) {
            config->set("plasticity.stdp.enable", true, ConfigSource::Default);
            config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
            config->set("plasticity.structural.enable", true, ConfigSource::Default);
            config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
            config->set("neuromod.novelty.enable", true, ConfigSource::Default);
            config->set("prediction.enable", true, ConfigSource::Default);
            config->set("cognition.enable", true, ConfigSource::Default);
            config->set("development.stage", static_cast<uint64_t>(0), ConfigSource::Default);
        }
        return config;
    }, "Create a research-oriented configuration with all systems enabled");

    // Enhanced Config methods
    m.def("configMerge", [](std::shared_ptr<Config> config, const std::string& key, 
                          const std::string& value) {
        config->set(key, value, ConfigSource::Runtime);
        return config;
    }, py::arg("config"), py::arg("key"), py::arg("value"), "Merge a key-value pair into config");

    m.def("configMergeNumber", [](std::shared_ptr<Config> config, const std::string& key, 
                                double value) {
        config->set(key, value, ConfigSource::Runtime);
        return config;
    }, py::arg("config"), py::arg("key"), py::arg("value"), "Merge a numeric key-value pair into config");

    m.def("configMergeBool", [](std::shared_ptr<Config> config, const std::string& key, 
                              bool value) {
        config->set(key, value, ConfigSource::Runtime);
        return config;
    }, py::arg("config"), py::arg("key"), py::arg("value"), "Merge a boolean key-value pair into config");

    m.def("configValidate", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("brain.neuron_count") || 
            config->getOr<int>("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be a positive integer");
        }
        
        if (config->has("brain.synapse_density")) {
            auto density = config->get<float>("brain.synapse_density");
            if (density && (*density < 0.0 || *density > 1.0)) {
                errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
            }
        }
        
        if (config->has("brain.connection_probability")) {
            auto prob = config->get<float>("brain.connection_probability");
            if (prob && (*prob < 0.0 || *prob > 1.0)) {
                errors.push_back("brain.connection_probability must be between 0.0 and 1.0");
            }
        }
        
        if (config->has("brain.v_thresh") && config->has("brain.v_reset")) {
            auto thresh = config->get<float>("brain.v_thresh");
            auto reset = config->get<float>("brain.v_reset");
            if (thresh && reset && *reset >= *thresh) {
                errors.push_back("brain.v_reset must be less than brain.v_thresh");
            }
        }
        
        return errors;
    }, "Validate configuration and return error messages if any");

    m.def("configToDict", [](std::shared_ptr<Config> config) {
        std::map<std::string, py::object> dict;
        auto keys = config->getKeys();
        
        for (const auto& key : keys) {
            if (config->has(key)) {
                auto value = config->get<std::string>(key);
                if (value.has_value()) {
                    dict[key] = *value;
                } else {
                    auto intVal = config->get<int64_t>(key);
                    if (intVal.has_value()) {
                        dict[key] = *intVal;
                    } else {
                        auto doubleVal = config->get<double>(key);
                        if (doubleVal.has_value()) {
                            dict[key] = *doubleVal;
                        } else {
                            auto boolVal = config->get<bool>(key);
                            if (boolVal.has_value()) {
                                dict[key] = *boolVal;
                            }
                        }
                    }
                }
            }
        }
        
        return dict;
    }, "Convert configuration to Python dictionary");

    m.def("configFromDict", [](std::map<std::string, py::object> dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        for (const auto& pair : dict) {
            const std::string& key = pair.first;
            const py::object& value = pair.second;
            
            if (value.is(py::bool_())) {
                config->set(key, value.cast<bool>(), ConfigSource::Runtime);
            } else if (value.is(py::int_())) {
                config->set(key, value.cast<int64_t>(), ConfigSource::Runtime);
            } else if (value.is(py::float_())) {
                config->set(key, value.cast<double>(), ConfigSource::Runtime);
            } else if (value.is(py::str())) {
                config->set(key, value.cast<std::string>(), ConfigSource::Runtime);
            } else {
                throw std::invalid_argument("Unsupported type in configuration dictionary for key: " + key);
            }
        }
        
        return config;
    }, "Create configuration from Python dictionary");

    // Simulation management utilities
    m.def("runEpisode", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                          std::shared_ptr<AgentBrain> agent, size_t numSteps, bool enableLearning) {
        if (!brain || !world || !agent) {
            throw std::invalid_argument("All simulation components must be provided");
        }
        
        brain->initialize();
        world->reset();
        agent->initialize(*world);
        
        if (enableLearning) {
            agent->enableRewardModulation(true);
            agent->enableStructuralPlasticity(true);
            agent->enableDevelopment(true);
            agent->enableCuriosity(true);
        }
        
        for (size_t step = 0; step < numSteps; ++step) {
            // Update world
            world->update(0.1);
            
            // Get sensory input
            SensoryPercept percept = world->getSensoryPercept();
            
            // Process sensory input
            agent->processSensoryInput(percept);
            
            // Brain step
            brain->step(static_cast<SimulationStep>(step));
            
            // Get action
            MotorCommand action = agent->decodeMotorCommand();
            
            // Apply action
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Apply reward modulation if learning enabled
            if (enableLearning) {
                auto internalSignals = percept.getInternal();
                float reward = internalSignals.empty() ? 0.0f : internalSignals[0];
                agent->applyRewardModulation(reward, 0.0f);
                agent->updateDevelopment(0.1);
            }
        }
        
        // Return simulation statistics
        return std::map<std::string, py::object>{
            {"steps_completed", numSteps},
            {"final_firing_rate", brain->getAverageFiringRate()},
            {"total_spikes", brain->getTotalSpikeCount()},
            {"active_neurons", brain->getActiveNeuronCount()},
            {"curiosity_level", agent->getCuriosityLevel()},
            {"novelty_level", agent->getNoveltyLevel()},
            {"developmental_stage", static_cast<int>(brain->getDevelopmentalStage())},
            {"reward_modulation_enabled", agent->isRewardModulationEnabled()},
            {"structural_plasticity_enabled", agent->isStructuralPlasticityEnabled()},
            {"development_enabled", agent->isDevelopmentEnabled()},
            {"curiosity_enabled", agent->isCuriosityEnabled()}
        };
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
          py::arg("numSteps") = 100, py::arg("enableLearning") = false,
          "Run a complete agent episode with world interaction and learning");

    m.def("saveBrainSnapshot", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::invalid_argument("Brain must not be null");
        }
        bool success = brain->save(filepath);
        if (!success) {
            throw std::runtime_error("Failed to save brain snapshot to " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Save brain state to file (checkpoint)");

    m.def("loadBrainSnapshot", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw std::invalid_argument("Brain must not be null");
        }
        bool success = brain->load(filepath);
        if (!success) {
            throw std::runtime_error("Failed to load brain snapshot from " + filepath);
        }
        return success;
    }, py::arg("brain"), py::arg("filepath"), "Load brain state from file (checkpoint)");

    m.def("createDefaultAgentBrain", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
        if (!brain || !world) {
            throw std::invalid_argument("Brain and world must not be null");
        }
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        return agent;
    }, py::arg("brain"), py::arg("world"), "Create and initialize an agent brain with world");

    // World configuration utilities
    m.def("configureStandardWorld", [](std::shared_ptr<SimpleWorld> world, size_t width = 20, 
                                     size_t height = 20, size_t visionWidth = 8, 
                                     size_t visionHeight = 8) {
        if (!world) {
            throw std::invalid_argument("World must not be null");
        }
        world->configure(width, height, visionWidth, visionHeight);
        return world;
    }, py::arg("world"), py::arg("width") = 20, py::arg("height") = 20,
          py::arg("visionWidth") = 8, py::arg("visionHeight") = 8,
          "Configure a standard world with given dimensions and agent vision");

    // Simulation statistics and monitoring
    m.def("getSimulationStats", [](std::shared_ptr<Brain> brain, std::shared_ptr<AgentBrain> agent,
                                   std::shared_ptr<SimpleWorld> world) {
        std::map<std::string, py::object> stats;
        
        if (brain) {
            stats["total_neurons"] = brain->getTotalNeuronCount();
            stats["total_synapses"] = brain->getTotalSynapseCount();
            stats["active_neurons"] = brain->getActiveNeuronCount();
            stats["firing_neurons"] = brain->getFiringNeuronCount();
            stats["total_spikes"] = brain->getTotalSpikeCount();
            stats["average_firing_rate"] = brain->getAverageFiringRate();
            stats["excitatory_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        }
        
        if (agent) {
            stats["curiosity_level"] = agent->getCuriosityLevel();
            stats["novelty_level"] = agent->getNoveltyLevel();
            stats["prediction_error"] = agent->getPredictionError();
            stats["neuromodulation_level"] = agent->getNeuromodulationLevel();
            stats["reward_modulation_enabled"] = agent->isRewardModulationEnabled();
            stats["structural_plasticity_enabled"] = agent->isStructuralPlasticityEnabled();
            stats["development_enabled"] = agent->isDevelopmentEnabled();
            stats["curiosity_enabled"] = agent->isCuriosityEnabled();
        }
        
        if (world) {
            stats["world_width"] = world->getWidth();
            stats["world_height"] = world->getHeight();
            stats["simulation_time"] = world->getSimulationTime();
            stats["max_energy"] = world->getMaxEnergy();
        }
        
        return stats;
    }, py::arg("brain"), py::arg("agent"), py::arg("world"),
          "Get comprehensive simulation statistics from all components");

    // Action utilities
    m.def("createAction", [](ActionType type, std::vector<float> parameters) {
        return std::make_unique<Action>(type, parameters);
    }, py::arg("type"), py::arg("parameters") = std::vector<float>(),
          "Create an action with specified type and parameters");

    m.def("createActionWithType", &Action::getName,
          "Get the name of an action type (utility function)");

    // Visualization utilities
    m.def("validateWorldState", [](std::shared_ptr<SimpleWorld> world, std::shared_ptr<Brain> brain,
                                   std::shared_ptr<AgentBrain> agent) {
        std::vector<std::string> warnings;
        
        if (world) {
            if (world->getWidth() == 0 || world->getHeight() == 0) {
                warnings.push_back("World dimensions are zero");
            }
            if (world->getAgentStart() == nullptr) {
                warnings.push_back("No agent start position set");
            }
        }
        
        if (brain && brain->getTotalNeuronCount() == 0) {
            warnings.push_back("Brain has no neurons initialized");
        }
        
        if (agent) {
            if (agent->getSensoryInputSize() == 0) {
                warnings.push_back("Agent brain has no sensory input configured");
            }
            if (agent->getMotorOutputSize() == 0) {
                warnings.push_back("Agent brain has no motor output configured");
            }
        }
        
        return warnings;
    }, "Validate simulation state and return warnings if any issues found");

    // Convenience functions for common patterns
    m.def("initializeCompleteAgent", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                       std::shared_ptr<AgentBrain> agent) {
        if (!brain || !world || !agent) {
            throw std::invalid_argument("All components must be provided");
        }
        
        brain->initialize();
        world->reset();
        world->configure(world->getWidth(), world->getHeight(), 8, 8);
        agent->initialize(*world);
        
        // Enable all learning subsystems
        agent->enableRewardModulation(true);
        agent->enableStructuralPlasticity(true);
        agent->enableDevelopment(true);
        agent->enableCuriosity(true);
        
        return true;
    }, "Initialize all components with default settings for a complete agent setup");

    m.def("getDefaultWorldConfig", []() {
        return std::map<std::string, py::object>{
            {"width", 20},
            {"height", 20},
            {"visionWidth", 8},
            {"visionHeight", 8},
            {"maxEnergy", 100.0},
            {"energyDecayRate", 0.1},
            {"simulationTimestep", 0.1}
        };
    }, "Get default world configuration parameters");

    m.def("getDefaultBrainConfig", []() {
        return std::map<std::string, py::object>{
            {"neuronCount", 1000},
            {"synapseDensity", 0.1},
            {"connectionProbability", 0.05},
            {"vThresh", -50.0},
            {"vRest", -70.0},
            {"vReset", -75.0},
            {"tauMem", 20.0},
            {"tauRef", 2.0}
        };
    }, "Get default brain configuration parameters");

    // Enhanced Config methods
    m.def("configMerge", [](std::shared_ptr<Config> config, const std::string& key, 
                          const std::string& value) {
        config->set(key, value, ConfigSource::Runtime);
        return config;
    }, py::arg("config"), py::arg("key"), py::arg("value"), "Merge a key-value pair into config");

    m.def("configValidate", [](std::shared_ptr<Config> config) {
        std::vector<std::string> errors;
        
        if (!config->has("brain.neuron_count") || 
            config->getOr<int>("brain.neuron_count", 0) <= 0) {
            errors.push_back("brain.neuron_count must be a positive integer");
        }
        
        if (config->has("brain.synapse_density")) {
            auto density = config->get<float>("brain.synapse_density");
            if (density && (*density < 0.0 || *density > 1.0)) {
                errors.push_back("brain.synapse_density must be between 0.0 and 1.0");
            }
        }
        
        return errors;
    }, "Validate configuration and return error messages if any");

    // Neuron utilities
    m.def("createTestNeurons", [](std::shared_ptr<Brain> brain, size_t count) {
        if (!brain) {
            throw std::invalid_argument("Brain must not be null");
        }
        // This is a placeholder for creating test neurons
        // In a real implementation, this would use the brain's internal API
        return std::vector<std::string>{"Test neurons created"};
    }, "Create test neurons for experimentation");

    // Improved Region management
    m.def("createDefaultRegions", [](std::shared_ptr<Brain> brain, const std::vector<std::string>& regionNames) {
        if (!brain) {
            throw std::invalid_argument("Brain must not be null");
        }
        std::vector<RegionId> createdRegions;
        for (const auto& name : regionNames) {
            createdRegions.push_back(brain->addRegion(name));
        }
        return createdRegions;
    }, "Create multiple regions with given names");

    // Performance monitoring
    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain, std::chrono::steady_clock::time_point startTime) {
        auto endTime = std::chrono::steady_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
        
        std::map<std::string, py::object> metrics;
        metrics["simulation_time_ms"] = static_cast<int64_t>(duration.count());
        metrics["time_per_step_ms"] = brain ? (duration.count() / (brain->getTotalSpikeCount() + 1)) : 0;
        
        return metrics;
    }, "Get performance metrics for timing analysis");

    // Enhanced error handling
    py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const std::invalid_argument& e) {
            PyErr_SetString(PyExc_ValueError, e.what());
        } catch (const std::runtime_error& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        } catch (...) {
            PyErr_SetString(PyExc_RuntimeError, "Unknown error occurred");
        }
    });
}

} // namespace nlm
