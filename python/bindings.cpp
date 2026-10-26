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

    // Core creation functions
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

    // Convenience functions for easier usage
    m.def("createStandardAgent", []() -> std::tuple<std::shared_ptr<Config>, std::shared_ptr<Brain>, std::shared_ptr<SimpleWorld>, std::shared_ptr<AgentBrain>> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        auto world = std::make_shared<SimpleWorld>();
        auto agent = std::make_shared<AgentBrain>(brain);
        return std::make_tuple(config, brain, world, agent);
    }, "Create a complete agent setup with brain, world, and agent brain interface");

    m.def("createSimulationConfig", [](int neuronCount, float connectionDensity, const std::string& name) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", static_cast<int64_t>(neuronCount), ConfigSource::Default);
        config->set("brain.synapse_density", connectionDensity, ConfigSource::Default);
        config->set("brain.region_count", 1, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        return config;
    }, py::arg("neuronCount"), py::arg("connectionDensity") = 0.1f, py::arg("name") = "simulation", "Create a pre-configured simulation configuration");

    // Configuration presets for different use cases
    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // High exploration, learning-focused configuration
        config->set("brain.neuron_count", 1000, ConfigSource::Default);
        config->set("brain.synapse_density", 0.08f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.08f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.exploration_strength", 0.8f, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.sensitivity", 1.5f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.002f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.synaptogenesis_rate", 0.00015f, ConfigSource::Default);
        config->set("plasticity.structural.pruning_rate", 0.00002f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create an exploration-focused configuration (high curiosity and learning)");

    m.def("createStabilityConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Stability-focused configuration (slow changes, high memory)
        config->set("brain.neuron_count", 500, ConfigSource::Default);
        config->set("brain.synapse_density", 0.03f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.03f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", false, ConfigSource::Default); // Low exploration
        config->set("neuromod.novelty.enable", false, ConfigSource::Default); // Low novelty detection
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.0005f, ConfigSource::Default); // Low learning rate
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.rate", 0.0005f, ConfigSource::Default); // Low learning rate
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.synaptogenesis_rate", 0.00005f, ConfigSource::Default); // Low structural change
        config->set("plasticity.structural.pruning_rate", 0.00001f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a stability-focused configuration (slow, deliberate learning)");

    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Development-stage focused configuration
        config->set("brain.neuron_count", 750, ConfigSource::Default);
        config->set("brain.synapse_density", 0.06f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.06f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.exploration_strength", 0.5f, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.sensitivity", 1.0f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.0015f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.synaptogenesis_rate", 0.00012f, ConfigSource::Default);
        config->set("plasticity.structural.pruning_rate", 0.000015f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(456), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a development-stage focused configuration (balanced learning)");

    m.def("createMemoryConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        // Memory-optimized configuration
        config->set("brain.neuron_count", 1200, ConfigSource::Default);
        config->set("brain.synapse_density", 0.12f, ConfigSource::Default);
        config->set("brain.connection_probability", 0.12f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", false, ConfigSource::Default);
        config->set("neuromod.novelty.enable", false, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learning_rate", 0.001f, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.rate", 0.002f, ConfigSource::Default); // High Hebbian for memory
        config->set("plasticity.structural.enable", false, ConfigSource::Default); // No structural changes
        config->set("random_seed", static_cast<int64_t>(789), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        return config;
    }, "Create a memory-optimized configuration (high stability for memory formation)");
    m.def("runSimpleSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        if (brain) {
            for (size_t i = 0; i < steps; ++i) {
                brain->step(i);
            }
        }
    }, py::arg("brain"), py::arg("steps"), "Run a simple brain simulation");

    m.def("runCompleteEpisode", []() -> bool {
        // Create everything needed for a complete episode
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        // Enable learning
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        agent->enableDevelopment(true);
        
        // Run episode
        for (size_t step = 0; step < 100; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward
            float reward = 0.1f;
            agent->applyRewardModulation(reward, 0.0f);
            
            agent->updateDevelopment(0.1);
        }
        
        return true;
    }, "Run a complete simulation episode with everything set up automatically");

    // Configuration loader/saver utilities
    m.def("saveConfigAsJson", [](const std::string& filepath, const std::shared_ptr<Config>& config) {
        if (config) {
            config->saveToFile(filepath);
        }
    }, py::arg("filepath"), py::arg("config"), "Save configuration to JSON file");

    m.def("loadConfigFromJson", [](const std::string& filepath) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->loadFromFile(filepath);
        return config;
    }, py::arg("filepath"), "Load configuration from JSON file");

    // Statistical utilities
    m.def("computeBrainStats", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> stats;
        if (!brain) {
            stats["error"] = -1.0;
            return stats;
        }
        
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["spike_rate"] = 0.0; // Placeholder - would need simulation time to compute properly
        stats["development_stage"] = static_cast<double>(static_cast<int>(brain->getDevelopmentalStage()));
        
        return stats;
    }, py::arg("brain"), "Compute brain statistics as a dictionary");

    // Statistical utilities
    m.def("computeBrainStats", [](const std::shared_ptr<Brain>& brain) {
        std::map<std::string, double> stats;
        if (!brain) {
            stats["error"] = -1.0;
            return stats;
        }
        
        stats["total_neurons"] = static_cast<double>(brain->getTotalNeuronCount());
        stats["total_synapses"] = static_cast<double>(brain->getTotalSynapseCount());
        stats["firing_neurons"] = static_cast<double>(brain->getFiringNeuronCount());
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = static_cast<double>(brain->getTotalSpikeCount());
        stats["spike_rate"] = 0.0; // Placeholder - would need simulation time to compute properly
        stats["development_stage"] = static_cast<double>(static_cast<int>(brain->getDevelopmentalStage()));
        
        return stats;
    }, py::arg("brain"), "Compute brain statistics as a dictionary");

    // Basic testing utilities for quick validation
    m.def("testBasicConnectivity", []() {
        // Create a simple brain
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 100, ConfigSource::Default);
        config->set("brain.synapse_density", 0.1f, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Basic connectivity test
        auto* region = brain->getRegion(RegionId(1));
        if (!region) {
            throw std::runtime_error("No region found in test");
        }
        
        auto neurons = region->getAllNeurons();
        if (neurons.empty()) {
            throw std::runtime_error("No neurons found in test");
        }
        
        // Inject current to test spike propagation
        size_t initialSpikes = brain->getTotalSpikeCount();
        neurons[0]->injectCurrent(20.0f);
        
        for (int step = 0; step < 50; ++step) {
            brain->step(step, step * 0.001);
        }
        
        size_t spikes = brain->getTotalSpikeCount() - initialSpikes;
        return std::make_tuple(
            true,  // success
            neurons.size(),  // neuron count
            spikes,  // spike count
            brain->getAverageFiringRate()  // average firing rate
        );
    }, "Run a basic connectivity test and return results");

    m.def("testPlasticity", []() {
        // Test STDP and Hebbian plasticity
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 200, ConfigSource::Default);
        config->set("brain.synapse_density", 0.15f, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(123), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Record initial weights
        std::vector<float> initialWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                initialWeights.push_back(syn->getWeight());
            }
        }
        
        size_t initialSynapseCount = brain->getTotalSynapseCount();
        
        // Apply learning stimuli
        for (int step = 0; step < 500; ++step) {
            // Inject current to stimulate plasticity
            brain->injectCurrentToNeurons(NeuronType::Sensory, 15.0f);
            brain->step(step, step * 0.001);
        }
        
        // Record final weights
        std::vector<float> finalWeights;
        if (auto* region = brain->getRegion(RegionId(1))) {
            for (const auto& syn : region->getSynapses()) {
                finalWeights.push_back(syn->getWeight());
            }
        }
        
        size_t finalSynapseCount = brain->getTotalSynapseCount();
        
        // Analyze weight changes
        size_t strengthened = 0, weakened = 0, unchanged = 0;
        float totalChange = 0.0f;
        
        size_t minSize = std::min(initialWeights.size(), finalWeights.size());
        for (size_t i = 0; i < minSize; ++i) {
            float delta = finalWeights[i] - initialWeights[i];
            totalChange += std::abs(delta);
            if (delta > 0.01f) strengthened++;
            else if (delta < -0.01f) weakened++;
            else unchanged++;
        }
        
        bool learningOccurred = (totalChange > 0.001f) || (strengthened > 0) || (weakened > 0);
        
        return std::make_tuple(
            learningOccurred,  // success
            strengthened,     // strengthened synapses
            weakened,         // weakened synapses
            unchanged,        // unchanged synapses
            totalChange       // total weight change
        );
    }, "Test plasticity learning (STDP and Hebbian)");

    m.def("testNeuromodulation", []() {
        // Test neuromodulation systems
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", 150, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("random_seed", static_cast<int64_t>(456), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        auto agent = std::make_shared<AgentBrain>(brain);
        
        // Create a simple world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(10, 10, 8, 8);
        world->reset();
        agent->initialize(*world);
        
        // Run some episodes with reward
        float totalReward = 0.0f;
        float maxCuriosity = 0.0f;
        float maxNovelty = 0.0f;
        
        for (int step = 0; step < 200; ++step) {
            world->update(0.1);
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step);
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Simple reward based on action type
            float reward = 0.0f;
            if (action == MotorCommand::MoveForward) reward = 0.5f;
            else if (action == MotorCommand::Interact) reward = 1.0f;
            
            agent->applyRewardModulation(reward, 0.0f);
            totalReward += reward;
            
            // Track neuromodulation levels
            maxCuriosity = std::max(maxCuriosity, agent->getCuriosityLevel());
            maxNovelty = std::max(maxNovelty, agent->getNoveltyLevel());
            
            agent->updateDevelopment(0.1);
        }
        
        bool neuromodulationActive = (maxCuriosity > 0.0f) || (maxNovelty > 0.0f);
        
        return std::make_tuple(
            neuromodulationActive,  // success
            totalReward,           // total reward received
            maxCuriosity,          // max curiosity level
            maxNovelty,            // max novelty level
            agent->getPredictionError()  // prediction error
        );
    }, "Test neuromodulation systems (dopamine, curiosity, novelty)");

    m.def("runQuickValidation", []() {
        // Run a complete quick validation test
        std::vector<std::string> testResults;
        std::vector<bool> testStatus;
        
        try {
            // Test 1: Basic connectivity
            auto result1 = testBasicConnectivity();
            bool connSuccess = std::get<0>(result1);
            size_t neuronCount = std::get<1>(result1);
            size_t spikeCount = std::get<2>(result1);
            float firingRate = std::get<3>(result1);
            testResults.push_back("Basic connectivity: " + std::to_string(neuronCount) + " neurons, " + std::to_string(spikeCount) + " spikes, " + std::to_string(firingRate) + " Hz");
            testStatus.push_back(connSuccess);
        } catch (...) {
            testResults.push_back("Basic connectivity: FAILED");
            testStatus.push_back(false);
        }
        
        try {
            // Test 2: Plasticity
            auto result2 = testPlasticity();
            bool plastSuccess = std::get<0>(result2);
            size_t strengthened = std::get<1>(result2);
            size_t weakened = std::get<2>(result2);
            size_t unchanged = std::get<3>(result2);
            float totalChange = std::get<4>(result2);
            testResults.push_back("Plasticity: " + std::to_string(strengthened) + " strengthened, " + std::to_string(weakened) + " weakened, " + std::to_string(totalChange) + " total change");
            testStatus.push_back(plastSuccess);
        } catch (...) {
            testResults.push_back("Plasticity: FAILED");
            testStatus.push_back(false);
        }
        
        try {
            // Test 3: Neuromodulation
            auto result3 = testNeuromodulation();
            bool neuromodSuccess = std::get<0>(result3);
            float totalReward = std::get<1>(result3);
            float maxCuriosity = std::get<2>(result3);
            float maxNovelty = std::get<3>(result3);
            float predError = std::get<4>(result3);
            testResults.push_back("Neuromodulation: " + std::to_string(totalReward) + " reward, curiosity: " + std::to_string(maxCuriosity) + ", novelty: " + std::to_string(maxNovelty) + ", pred error: " + std::to_string(predError));
            testStatus.push_back(neuromodSuccess);
        } catch (...) {
            testResults.push_back("Neuromodulation: FAILED");
            testStatus.push_back(false);
        }
        
        bool allPassed = std::all_of(testStatus.begin(), testStatus.end(), [](bool s) { return s; });
        
        return std::make_tuple(
            allPassed,  // overall success
            testResults, // detailed results
            testStatus   // individual test status
        );
    }, "Run a complete quick validation of NLM systems");

} // namespace nlm
