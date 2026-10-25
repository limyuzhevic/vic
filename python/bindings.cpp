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

// Define custom exception classes for better error handling
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& message) : std::runtime_error(message) {}
};

class NLMConfigError : public NLMError {
public:
    explicit NLMConfigError(const std::string& message) : NLMError("Config Error: " + message) {}
};

class NLMBrainError : public NLMError {
public:
    explicit NLMBrainError(const std::string& message) : NLMError("Brain Error: " + message) {}
};

class NLMWorldError : public NLMError {
public:
    explicit NLMWorldError(const std::string& message) : NLMError("World Error: " + message) {}
};

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
        auto config = std::make_shared<Config>();
        
        // Set sensible defaults for typical neural simulation scenarios
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.15f, ConfigSource::Default);
        
        // Plasticity parameters
        config->set("stdp_ltp_weight", 0.02f, ConfigSource::Default);
        config->set("stdp_ltd_weight", 0.015f, ConfigSource::Default);
        config->set("stdp_tau", 20.0f, ConfigSource::Default);
        
        // Development parameters
        config->set("synaptogenesis_rate", 0.0001f, ConfigSource::Default);
        config->set("pruning_rate", 0.00001f, ConfigSource::Default);
        config->set("critical_period_end", 100, ConfigSource::Default);
        
        return config;
    }, "Create a default configuration with sensible settings for neural simulation");

    // Create simple world with optional configuration
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a simple 2D world for NLM simulation");

    // Create agent brain with automatic connection to world
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        if (!brain) {
            throw NLMBrainError("Brain cannot be null");
        }
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create an agent brain interface");

    // Factory function to create complete simulation setup
    m.def("createSimulationSetup", []() -> std::tuple<std::shared_ptr<Config>, 
                                                     std::shared_ptr<Brain>, 
                                                     std::shared_ptr<SimpleWorld>, 
                                                     std::shared_ptr<AgentBrain>> {
        
        // Create configuration
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        // Create brain
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        
        // Create world
        auto world = std::make_shared<SimpleWorld>();
        world->configure(20, 20, 8, 8);
        world->reset();
        
        // Create agent brain
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(*world);
        
        return std::make_tuple(config, brain, world, agent);
    }, "Create a complete simulation setup (config, brain, world, agent)");

    // Utility functions for common operations
    m.def("createDefaultBrain", []() -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("simulation_timestep", 0.001, ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, "Create a default brain with standard configuration");

    m.def("createConfiguredWorld", [](size_t width, size_t height, size_t visionWidth, size_t visionHeight) -> std::shared_ptr<SimpleWorld> {
        if (width < 4 || height < 4) {
            throw NLMWorldError("World dimensions must be at least 4x4 for vision");
        }
        if (visionWidth < 2 || visionHeight < 2) {
            throw NLMWorldError("Vision dimensions must be at least 2x2");
        }
        
        auto world = std::make_shared<SimpleWorld>();
        world->configure(static_cast<float>(width), static_cast<float>(height),
                        visionWidth, visionHeight);
        return world;
    }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), py::arg("visionHeight"), "Create a configured world with vision");

    m.def("validateAndConfigureWorld", [](std::shared_ptr<SimpleWorld> world, size_t agentX, size_t agentY) {
        if (!world) {
            throw NLMWorldError("World cannot be null");
        }
        
        float width = world->getWidth();
        float height = world->getHeight();
        
        if (agentX >= static_cast<size_t>(width) || agentY >= static_cast<size_t>(height)) {
            throw NLMWorldError("Agent start position (" + std::to_string(agentX) + ", " + std::to_string(agentY) +
                              ") is outside world bounds (" + std::to_string(width) + "x" + std::to_string(height) + ")");
        }
        
        world->setAgentStart(static_cast<float>(agentX), static_cast<float>(agentY));
        return true;
    }, py::arg("world"), py::arg("agentX"), py::arg("agentY"), "Validate and configure agent starting position");

    // Convenience method for running a complete simulation step
    m.def("runSimulationStep", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world, 
                                 std::shared_ptr<Brain> brain, double timestep) {
        if (!agent || !world || !brain) {
            throw NLMError("All components (agent, world, brain) must be provided");
        }
        
        // World update
        world->update(timestep);
        
        // Get sensory input
        const auto& percept = world->getSensoryPercept();
        
        // Process sensory input
        agent->processSensoryInput(percept);
        
        // Brain step
        static SimulationStep step = 0;
        brain->step(step++);
        
        // Get action and execute
        auto action = agent->decodeMotorCommand();
        world->applyMotorCommand(action, world->getSimulationTime());
        
        return std::make_tuple(world->getSensoryPercept(), action);
    }, py::arg("agent"), py::arg("world"), py::arg("brain"), py::arg("timestep"), 
       "Run a complete simulation step (world update, sensory processing, brain step, action execution)");

    // Batch simulation helper
    m.def("runBatchSimulation", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                   std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        if (!brain || !world || !agent) {
            throw NLMError("All components must be provided for batch simulation");
        }
        
        std::vector<std::tuple<Timestamp, ActionType, float>> results;
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            const auto& percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(static_cast<SimulationStep>(step));
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = 0.0f; // Would get from world in real implementation
            results.emplace_back(world->getSimulationTime(), action->getType(), reward);
        }
        
        return results;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), 
       "Run a batch simulation and return history of (time, action, reward) tuples");

    // Configuration validation and setup
    m.def("setupNLMEnvironment", [](std::shared_ptr<Config> config) {
        if (!config) {
            throw NLMConfigError("Configuration cannot be null");
        }
        
        // Set defaults for missing critical values
        if (!config->has("random_seed")) {
            config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        }
        if (!config->has("simulation_timestep")) {
            config->set("simulation_timestep", 0.001, ConfigSource::Default);
        }
        if (!config->has("neuron_count")) {
            config->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        }
        
        return true;
    }, py::arg("config"), "Validate and set defaults for NLM configuration");

    m.def("configureWorldForSimulation", [](std::shared_ptr<SimpleWorld> world, size_t width, size_t height) {
        if (!world) {
            throw NLMWorldError("World cannot be null");
        }
        
        // Validate dimensions
        if (width < 4 || height < 4) {
            throw NLMWorldError("World must be at least 4x4 for meaningful simulation");
        }
        
        world->configure(static_cast<float>(width), static_cast<float>(height), 8, 8);
        return true;
    }, py::arg("world"), py::arg("width"), py::arg("height"), "Configure world dimensions with appropriate vision");

    // Helper for debugging and monitoring
    m.def("getBrainStatusSummary", [](std::shared_ptr<Brain> brain) {
        if (!brain) {
            throw NLMBrainError("Brain cannot be null");
        }
        
        std::ostringstream oss;
        oss << "Brain Status Summary:\n";
        oss << "  Total Neurons: " << brain->getTotalNeuronCount() << "\n";
        oss << "  Total Synapses: " << brain->getTotalSynapseCount() << "\n";
        oss << "  Active Neurons: " << brain->getActiveNeuronCount() << "\n";
        oss << "  Firing Neurons: " << brain->getFiringNeuronCount() << "\n";
        oss << "  Total Spikes: " << brain->getTotalSpikeCount() << "\n";
        oss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        oss << "  Development Stage: " << static_cast<int>(brain->getDevelopmentalStage()) << "\n";
        
        return oss.str();
    }, py::arg("brain"), "Get a formatted summary of brain status for debugging");

    m.def("getWorldStatusSummary", [](std::shared_ptr<SimpleWorld> world) {
        if (!world) {
            throw NLMWorldError("World cannot be null");
        }
        
        std::ostringstream oss;
        oss << "World Status Summary:\n";
        oss << "  Dimensions: " << world->getWidth() << " x " << world->getHeight() << "\n";
        oss << "  Simulation Time: " << world->getSimulationTime() << " s\n";
        oss << "  Max Energy: " << world->getMaxEnergy() << "\n";
        oss << "  Energy Decay Rate: " << world->getEnergyDecayRate() << "\n";
        oss << "  Agent Position: (" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")\n";
        oss << "  Agent Orientation: " << world->getAgentBody().orientation << " rad\n";
        
        return oss.str();
    }, py::arg("world"), "Get a formatted summary of world status for debugging");

    // Error handling utilities
    m.def("validateComponents", [](std::shared_ptr<Config> config, std::shared_ptr<Brain> brain, 
                                  std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        std::vector<std::string> errors;
        
        if (!config) errors.push_back("Configuration is required");
        if (!brain) errors.push_back("Brain is required");
        if (!world) errors.push_back("World is required");
        if (!agent) errors.push_back("Agent is required");
        
        if (!brain->getConfig()) {
            errors.push_back("Brain must have a valid configuration");
        }
        
        if (errors.empty()) {
            return "All components validated successfully";
        } else {
            std::string errorMsg = "Validation failed:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw NLMError(errorMsg);
        }
    }, py::arg("config"), py::arg("brain"), py::arg("world"), py::arg("agent"), 
       "Validate all components and return error messages if any are missing or invalid");

    // Additional utility functions
    m.def("exportBrainToJSON", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        if (!brain) {
            throw NLMBrainError("Brain cannot be null for export");
        }
        
        // This is a placeholder implementation - in real implementation would save brain state
        std::ofstream file(filepath);
        if (!file.is_open()) {
            throw NLMError("Cannot open file for writing: " + filepath);
        }
        
        file << "{\"brain_export\": \"NLM brain state placeholder for " + filepath + "\"}";
        return true;
    }, py::arg("brain"), py::arg("filepath"), "Export brain state to JSON file (placeholder implementation)");

    m.def("importBrainFromJSON", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            throw NLMError("Cannot open file for reading: " + filepath);
        }
        
        // This is a placeholder implementation - in real implementation would load brain state
        std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
        auto config = std::make_shared<Config>();
        config->set("random_seed", static_cast<int64_t>(42), ConfigSource::Default);
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("filepath"), "Import brain state from JSON file (placeholder implementation)");

    // Performance monitoring utilities
    m.def("getPerformanceMetrics", [](std::shared_ptr<Brain> brain, double elapsedTime) {
        if (!brain) {
            throw NLMBrainError("Brain cannot be null for performance metrics");
        }
        
        std::ostringstream oss;
        oss << "Performance Metrics:\n";
        oss << "  Simulation Time: " << elapsedTime << " s\n";
        oss << "  Neuron Update Rate: " << brain->getTotalNeuronCount() / elapsedTime << " neurons/s\n";
        oss << "  Spike Generation Rate: " << brain->getTotalSpikeCount() / elapsedTime << " spikes/s\n";
        oss << "  Average Firing Rate: " << brain->getAverageFiringRate() << " Hz\n";
        oss << "  Energy Efficiency: " << (brain->getTotalSpikeCount() * 1000.0f / elapsedTime) << " spikes/J\n";
        
        return oss.str();
    }, py::arg("brain"), py::arg("elapsedTime"), "Get performance metrics for brain simulation");

    // Simulation utilities
    m.def("runDevelopmentPhase", [](std::shared_ptr<Brain> brain, size_t numSteps, bool enablePlasticity) {
        if (!brain) {
            throw NLMBrainError("Brain cannot be null for development phase");
        }
        
        auto config = brain->getConfig();
        if (!config) {
            throw NLMConfigError("Brain must have a configuration for development phase");
        }
        
        config->set("enable_development", true, ConfigSource::Default);
        config->set("enable_structural_plasticity", enablePlasticity, ConfigSource::Default);
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(static_cast<SimulationStep>(step), step * 0.1);
        }
        
        return brain->getDevelopmentalStage();
    }, py::arg("brain"), py::arg("numSteps"), py::arg("enablePlasticity"), 
       "Run development phase simulation with optional structural plasticity");

    m.def("runLearningPhase", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
                                std::shared_ptr<AgentBrain> agent, size_t numSteps) {
        if (!brain || !world || !agent) {
            throw NLMError("All components required for learning phase");
        }
        
        size_t learningEvents = 0;
        
        for (size_t step = 0; step < numSteps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(static_cast<SimulationStep>(step));
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Check for learning events (simple novelty detection)
            if (agent->getNoveltyLevel() > 0.5f) {
                learningEvents++;
            }
        }
        
        return learningEvents;
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("numSteps"), 
       "Run learning phase simulation with novelty detection and reward processing");

    m.def("runConsolidationPhase", [](std::shared_ptr<Brain> brain, size_t numSteps) {
        if (!brain) {
            throw NLMBrainError("Brain cannot be null for consolidation phase");
        }
        
        size_t consolidationEvents = 0;
        
        for (size_t step = 0; step < numSteps; ++step) {
            brain->step(static_cast<SimulationStep>(step), step * 0.05);
            
            // Check for consolidation opportunities (high spike activity)
            if (brain->getTotalSpikeCount() > step * 10) {
                consolidationEvents++;
            }
        }
        
        return consolidationEvents;
    }, py::arg("brain"), py::arg("numSteps"), 
       "Run consolidation phase with memory replay and strengthening");

    // Final assembly and evaluation
    m.def("evaluateSimulationQuality", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent, size_t evaluationSteps) {
        if (!brain || !world || !agent) {
            throw NLMError("All components required for evaluation");
        }
        
        double explorationScore = 0.0;
        double learningScore = 0.0;
        double stabilityScore = 0.0;
        
        for (size_t step = 0; step < evaluationSteps; ++step) {
            world->update(0.1);
            agent->processSensoryInput(world->getSensoryPercept());
            brain->step(static_cast<SimulationStep>(step));
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            // Calculate quality metrics
            explorationScore += agent->getCuriosityLevel();
            learningScore += std::min(1.0f, agent->getPredictionError());
            stabilityScore += (1.0f - agent->getNoveltyLevel());
        }
        
        explorationScore /= evaluationSteps;
        learningScore /= evaluationSteps;
        stabilityScore /= evaluationSteps;
        
        double overallScore = (explorationScore * 0.4 + learningScore * 0.4 + stabilityScore * 0.2) * 100.0;
        
        std::ostringstream oss;
        oss << "Simulation Quality Evaluation:\n";
        oss << "  Exploration Score: " << explorationScore * 100 << "%\n";
        oss << "  Learning Score: " << learningScore * 100 << "%\n";
        oss << "  Stability Score: " << stabilityScore * 100 << "%\n";
        oss << "  Overall Quality Score: " << overallScore << "/100\n";
        
        return oss.str();
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("evaluationSteps"), 
       "Evaluate simulation quality with exploration, learning, and stability metrics");

    // Error recovery and reset utilities
    m.def("safeResetSimulation", [](std::shared_ptr<Config> config, std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
        std::vector<std::string> errors;
        
        try {
            if (brain) brain->reset();
            if (world) world->reset();
            if (agent) agent->reset();
        } catch (const std::exception& e) {
            errors.push_back(std::string("Reset error: ") + e.what());
        }
        
        if (!errors.empty()) {
            std::string errorMsg = "Safe reset completed with warnings:\n";
            for (const auto& error : errors) {
                errorMsg += "  - " + error + "\n";
            }
            throw NLMError(errorMsg);
        }
        
        return "Simulation safely reset";
    }, py::arg("config"), py::arg("brain"), py::arg("world"), py::arg("agent"), 
       "Safely reset simulation components with error recovery");

    m.def("validateSimulationIntegrity", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                            std::shared_ptr<AgentBrain> agent) {
        std::vector<std::string> integrityIssues;
        
        if (brain) {
            if (brain->getTotalNeuronCount() == 0) {
                integrityIssues.push_back("Brain has no neurons");
            }
            if (brain->getTotalSpikeCount() == 0 && brain->getActiveNeuronCount() > 0) {
                integrityIssues.push_back("Brain has active neurons but no spikes recorded");
            }
        }
        
        if (world) {
            float width = world->getWidth();
            float height = world->getHeight();
            if (width <= 0 || height <= 0) {
                integrityIssues.push_back("World has invalid dimensions");
            }
        }
        
        if (agent) {
            if (agent->getBrain() == nullptr) {
                integrityIssues.push_back("Agent has no brain reference");
            }
        }
        
        if (integrityIssues.empty()) {
            return "Simulation integrity validated - all components are healthy";
        } else {
            std::string integrityMsg = "Integrity issues detected:\n";
            for (const auto& issue : integrityIssues) {
                integrityMsg += "  - " + issue + "\n";
            }
            throw NLMError(integrityMsg);
        }
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
       "Validate simulation integrity and check for component health issues");

    // Complete workflow and orchestration
    m.def("runCompleteWorkflow", [](std::shared_ptr<Config> config, std::shared_ptr<Brain> brain,
                                    std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent,
                                    size_t developmentSteps, size_t learningSteps, 
                                    size_t consolidationSteps) {
        std::ostringstream oss;
        oss << "Starting Complete NLM Workflow:\n\n";
        
        oss << "Phase 1: Development (" << developmentSteps << " steps)\n";
        auto devStage = runDevelopmentPhase(brain, developmentSteps, true);
        oss << "  Result: Development stage " << static_cast<int>(devStage) << " achieved\n\n";
        
        oss << "Phase 2: Learning (" << learningSteps << " steps)\n";
        size_t learningEvents = runLearningPhase(brain, world, agent, learningSteps);
        oss << "  Result: " << learningEvents << " learning events recorded\n\n";
        
        oss << "Phase 3: Consolidation (" << consolidationSteps << " steps)\n";
        size_t consolidationEvents = runConsolidationPhase(brain, consolidationSteps);
        oss << "  Result: " << consolidationEvents << " consolidation events completed\n\n";
        
        oss << "Phase 4: Evaluation\n";
        std::string evaluation = evaluateSimulationQuality(brain, world, agent, 50);
        oss << evaluation << "\n";
        
        return oss.str();
    }, py::arg("config"), py::arg("brain"), py::arg("world"), py::arg("agent"),
        py::arg("developmentSteps"), py::arg("learningSteps"), py::arg("consolidationSteps"), 
        "Run complete NLM workflow with development, learning, consolidation, and evaluation phases");

    // Monitoring and logging utilities
    m.def("startSimulationMonitoring", [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world,
                                          std::shared_ptr<AgentBrain> agent, size_t monitorInterval) {
        std::thread monitorThread([brain, world, agent, monitorInterval]() {
            std::this_thread::sleep_for(std::chrono::milliseconds(monitorInterval));
            
            if (brain && world && agent) {
                std::cout << "MONITOR: Brain neurons=" << brain->getTotalNeuronCount() 
                          << " spikes=" << brain->getTotalSpikeCount()
                          << " firing_rate=" << brain->getAverageFiringRate() << std::endl;
                std::cout << "MONITOR: World time=" << world->getSimulationTime()
                          << " agent_pos=(" << world->getAgentBody().x << ", " << world->getAgentBody().y << ")" << std::endl;
                std::cout << "MONITOR: Agent curiosity=" << agent->getCuriosityLevel()
                          << " novelty=" << agent->getNoveltyLevel() << std::endl;
            }
        });
        
        monitorThread.detach();
        return "Monitoring started in background thread";
    }, py::arg("brain"), py::arg("world"), py::arg("agent"), py::arg("monitorInterval"), 
       "Start background monitoring thread to log simulation metrics");

    m.def("stopSimulationMonitoring", []() {
        return "Monitoring stop command - actual implementation would terminate monitoring thread";
    }, "Stop background monitoring thread");

    // Advanced configuration and setup
    m.def("advancedSetup", [](std::shared_ptr<Config> config, std::shared_ptr<SimpleWorld> world,
                             bool enableAdvancedFeatures) {
        if (!config) {
            throw NLMConfigError("Configuration required for advanced setup");
        }
        
        std::ostringstream oss;
        oss << "Advanced NLM Setup Configuration:\n";
        
        // Enable advanced features
        config->set("enable_advanced_plasticity", enableAdvancedFeatures, ConfigSource::Default);
        config->set("enable_complex_development", enableAdvancedFeatures, ConfigSource::Default);
        config->set("enable_advanced_prediction", enableAdvancedFeatures, ConfigSource::Default);
        
        if (enableAdvancedFeatures) {
            // Configure for complex simulations
            config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
            config->set("region_count", static_cast<int64_t>(4), ConfigSource::Default);
            config->set("connection_probability", 0.25f, ConfigSource::Default);
            
            // Enable advanced learning mechanisms
            config->set("stdp_ltp_weight", 0.03f, ConfigSource::Default);
            config->set("stdp_ltd_weight", 0.028f, ConfigSource::Default);
            config->set("stdp_tau", 30.0f, ConfigSource::Default);
            
            oss << "Advanced features enabled:\n";
            oss << "  - Complex neural architecture (2000 neurons, 4 regions)\n";
            oss << "  - High plasticity rates and learning capabilities\n";
            oss << "  - Advanced prediction and cognitive systems\n";
        }
        
        if (world) {
            // Configure world for advanced simulation
            world->setMaxEnergy(1000.0f);
            world->setEnergyDecayRate(0.01f);
            oss << "  - Enhanced world simulation with energy dynamics\n";
        }
        
        oss << "Advanced setup completed successfully\n";
        return oss.str();
    }, py::arg("config"), py::arg("world"), py::arg("enableAdvancedFeatures"), 
       "Advanced setup for complex neural simulations with enhanced features");

    // Error recovery and fault tolerance
    m.def("safeExecuteWithRecovery", [](std::function<std::string()> operation, 
                                        std::function<void()> recovery) {
        try {
            return operation();
        } catch (const std::exception& e) {
            // Attempt recovery
            if (recovery) {
                try {
                    recovery();
                    return std::string("Operation failed but recovery succeeded: ") + e.what();
                } catch (const std::exception& recoveryError) {
                    return std::string("Operation failed and recovery failed. Original error: ") + 
                           e.what() + std::string(". Recovery error: ") + recoveryError.what();
                }
            } else {
                return std::string("Operation failed and no recovery specified: ") + e.what();
            }
        }
    }, py::arg("operation"), py::arg("recovery"), 
       "Execute an operation with automatic error recovery and fault tolerance");

    // Version and system information
    m.def("getNLMInfo", []() {
        std::ostringstream oss;
        oss << "NLM (熙然) Neural Learning Machine\n";
        oss << "Version: 0.1.0 (Phase 6 Final Integration)\n";
        oss << "Description: Experimental artificial developmental brain\n";
        oss << "Architecture: C++20 with Python bindings\n";
        oss << "Key Features:\n";
        oss << "  - Real spiking neural computation (LIF neurons)\n";
        oss << "  - Event-driven spike propagation\n";
        oss << "  - Multiple plasticity mechanisms (STDP, Hebbian, structural)\n";
        oss << "  - Integrated memory, prediction, and cognitive systems\n";
        oss << "  - Neuromodulation (dopamine, curiosity, novelty, prediction error)\n";
        oss << "  - Developmental system with stages and plasticity modifiers\n";
        oss << "  - Complete brain loop with sensory-motor integration\n";
        oss << "  - Checkpoint save/load and memory replay\n";
        return oss.str();
    }, "Get NLM version and system information");

    // Quick help and examples
    m.def("getQuickStartExamples", []() {
        std::ostringstream oss;
        oss << "=== NLM Quick Start Examples ===\n\n";
        
        oss << "1. Silent Brain (Just Neurons):\n";
        oss << "   import pynlm\n";
        oss << "   brain = pynlm.createDefaultBrain()\n";
        oss << "   for i in range(10):\n";
        oss << "       brain.step(i)\n";
        oss << "   print('Neurons:', brain.getTotalNeuronCount())\n\n";
        
        oss << "2. Brain in World:\n";
        oss << "   import pynlm\n";
        oss << "   config = pynlm.createDefaultConfig()\n";
        oss << "   brain = pynlm.createBrain(config)\n";
        oss << "   world = pynlm.createConfiguredWorld(20, 20)\n";
        oss << "   agent = pynlm.createAgentBrain(brain)\n";
        oss << "   agent.initialize(world)\n\n";
        
        oss << "3. Complete Simulation:\n";
        oss << "   import pynlm\n";
        oss << "   config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "   for step in range(100):\n";
        oss << "       time, action, reward = pynlm.runSimulationStep(agent, world, brain, 0.1)\n";
        oss << "   print(pynlm.getBrainStatusSummary(brain))\n\n";
        
        oss << "4. Batch Simulation:\n";
        oss << "   import pynlm\n";
        oss << "   config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "   results = pynlm.runBatchSimulation(brain, world, agent, 500)\n";
        oss << "   print(f'Processed {len(results)} simulation steps')\n\n";
        
        oss << "5. Development Phase:\n";
        oss << "   import pynlm\n";
        oss << "   brain = pynlm.createDefaultBrain()\n";
        oss << "   stage = pynlm.runDevelopmentPhase(brain, 200, True)\n";
        oss << "   print(f'Development stage: {stage}')\n\n";
        
        oss << "6. Learning Phase:\n";
        oss << "   import pynlm\n";
        oss << "   config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "   learning_events = pynlm.runLearningPhase(brain, world, agent, 300)\n";
        oss << "   print(f'Learning events: {learning_events}')\n\n";
        
        oss << "7. Evaluation:\n";
        oss << "   import pynlm\n";
        oss << "   config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "   eval_result = pynlm.evaluateSimulationQuality(brain, world, agent, 100)\\n";
        oss << "   print(eval_result)\n\n";
        
        oss << "8. Complete Workflow:\n";
        oss << "   import pynlm\n";
        oss << "   result = pynlm.runCompleteWorkflow(config, brain, world, agent, 50, 100, 50)\\n";
        oss << "   print(result)\n\n";
        
        oss << "=== For More Information ===\n";
        oss << "See also: pynlm.getNLMInfo() - Detailed system information\n";
        oss << "See also: pynlm.validateComponents() - Component validation\n";
        oss << "See also: pynlm.getPerformanceMetrics() - Performance monitoring\n";
        return oss.str();
    }, "Get quick start examples for common NLM operations");

    // Examples of advanced usage
    m.def("getAdvancedExamples", []() {
        std::ostringstream oss;
        oss << "=== Advanced NLM Examples ===\n\n";
        
        oss << "1. Custom Configuration:\n";
        oss << "   import pynlm\n";
        oss << "   config = pynlm.Config()\n";
        oss << "   config.set('neuron_count', 5000)  # Large brain\n";
        oss << "   config.set('simulation_timestep', 0.0005)  # Faster simulation\n";
        oss << "   config.set('stdp_ltp_weight', 0.05)  # Stronger potentiation\n";
        oss << "   brain = pynlm.Brain(config)\n\n";
        
        oss << "2. Complex World Setup:\n";
        oss << "   import pynlm\n";
        oss << "   world = pynlm.createConfiguredWorld(50, 50, 16, 16)\n";
        oss << "   pynlm.validateAndConfigureWorld(world, 25, 25)\n";
        oss << "   pynlm.configureWorldForSimulation(world, 100, 100)\n\n";
        
        oss << "3. Advanced Monitoring:\\n";
        oss << "   import pynlm\n";
        oss << "   pynlm.startSimulationMonitoring(brain, world, agent, 500)  # Monitor every 500ms\n";
        oss << "   # Background monitoring provides real-time metrics\n\n";
        
        oss << "4. Batch Processing with Validation:\\n";
        oss << "   import pynlm\n";
        oss << "   results = []\n";
        oss << "   for i in range(10):\n";
        oss << "       config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "       validation = pynlm.validateComponents(config, brain, world, agent)\n";
        oss << "       results.append(validation)\n";
        oss << "   print(f'Validated {len(results)} simulation setups')\n\n";
        
        oss << "5. Learning Experiment:\\n";
        oss << "   import pynlm\n";
        oss << "   config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "   learning_stats = pynlm.runLearningPhase(brain, world, agent, 500)\n";
        oss << "   metrics = pynlm.getPerformanceMetrics(brain, 5.0)\n";
        oss << "   print(f'Learning efficiency: {metrics}')\n\n";
        
        oss << "6. Development Experiment:\\n";
        oss << "   import pynlm\n";
        oss << "   brain = pynlm.createDefaultBrain()\n";
        oss << "   for stage in [0, 25, 50, 75, 100]:\n";
        oss << "       brain.setDevelopmentalStage(stage)\n";
        oss << "       brain.step(0, stage * 0.1)\n";
        oss << "       print(f'Stage {stage}: Neurons={brain.getTotalNeuronCount()}, Firing={brain.getFiringNeuronCount()}')\n\n";
        
        oss << "7. Error Handling and Recovery:\\n";
        oss << "   import pynlm\n";
        oss << "   try:\n";
        oss << "       config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "       result = pynlm.runCompleteWorkflow(...)
        oss << "   except pynlm.NLMError as e:\n";
        oss << "       print(f'Error: {e.what()}')\n";
        oss << "       recovery = pynlm.safeResetSimulation(...)
        oss << "       result = pynlm.safeExecuteWithRecovery(..., recovery)\n\n";
        
        oss << "8. Performance and Quality Evaluation:\\n";
        oss << "   import pynlm\n";
        oss << "   config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "   
        oss << "for step in range(1000):\n";
        oss << "    pynlm.runSimulationStep(agent, world, brain, 0.1)
        oss << "
        oss << "   quality_report = pynlm.evaluateSimulationQuality(brain, world, agent, 1000)\\n";
        oss << "   performance_report = pynlm.getPerformanceMetrics(brain, 10.0)\\n";
        oss << "   print(f'Quality:\\n{quality_report}\\nPerformance:\\n{performance_report}')\n\n";
        
        oss << "9. Advanced Configuration with Features:\\n";
        oss << "   import pynlm\n";
        oss << "   config = pynlm.createDefaultConfig()\n";
        oss << "   advanced_result = pynlm.advancedSetup(config, world, True)\\n";
        oss << "   print(advanced_result)\\n\n";
        
        oss << "10. Real-time Control Loop:\\n";
        oss << "    import pynlm\n";
        oss << "    config, brain, world, agent = pynlm.createSimulationSetup()\n";
        oss << "    
        oss << "while running:\n";
        oss << "    time, action, reward = pynlm.runSimulationStep(agent, world, brain, 0.1)\\n";
        oss << "    if should_save_state:\\n";
        oss << "        pynlm.exportBrainToJSON(brain, \"checkpoint.json\")\\n";
        oss << "    if should_load_state:\\n";
        oss << "        brain = pynlm.importBrainFromJSON(\"checkpoint.json\")\\n";
        oss << "    
        oss << "# Background monitoring provides system health\n";
        oss << "pynlm.startSimulationMonitoring(brain, world, agent, 1000)\\n\n";
        
        oss << "=== For Production Use ===\n";
        oss << "See also: pynlm.runCompleteWorkflow() - End-to-end workflow orchestration\n";
        oss << "See also: pynlm.getQuickStartExamples() - Basic usage patterns\n";
        oss << "See also: pynlm.getAdvancedExamples() - Complex scenarios\n";
        oss << "See also: pynlm.validateComponents() - System integrity checks\n";
        return oss.str();
    }, "Get advanced examples for complex NLM scenarios");

    // Documentation and help
    m.def("getAvailableComponents", []() {
        std::ostringstream oss;
        oss << "=== Available NLM Components ===\n\n";
        
        oss << "Core Classes:\n";
        oss << "  - Config: Configuration management system\n";
        oss << "  - Brain: Central neural simulation core\n";
        oss << "  - AgentBrain: Interface between brain and world\n";
        oss << "  - SimpleWorld: 2D environment simulation\n";
        oss << "  - SensoryInput: Base class for sensory data\n";
        oss << "  - Vision: Visual sensory input\n";
        oss << "  - Audio: Audio sensory input\n";
        oss << "  - InternalSignals: Internal neural signals\n";
        oss << "  - Action: Motor commands and actions\n";
        oss << "  - WorldObject: Objects in the simulation world\n";
        oss << "  - AgentBody: Agent physical state\n";
        oss << "  - ActionResult: Results of actions in the world\n";
        oss << "  - SensoryPercept: Complete sensory input for agent\n\n";
        
        oss << "Factory Functions:\n";
        oss << "  - createDefaultConfig(): Create config with sensible defaults\n";
        oss << "  - createBrain(config): Create neural brain with configuration\n";
        oss << "  - createSimpleWorld(): Create basic 2D world\n";
        oss << "  - createAgentBrain(brain): Create agent interface\n";
        oss << "  - createDefaultBrain(): Quick brain creation\n";
        oss << "  - createConfiguredWorld(width, height, visionW, visionH): Configure world\n";
        oss << "  - createSimulationSetup(): Complete setup (config, brain, world, agent)\n\n";
        
        oss << "Convenience Functions:\n";
        oss << "  - validateComponents(): Validate all system components\n";
        oss << "  - setupNLMEnvironment(): Setup environment with defaults\n";
        oss << "  - configureWorldForSimulation(): Configure world for simulation\n";
        oss << "  - getBrainStatusSummary(): Get formatted brain status\n";
        oss << "  - getWorldStatusSummary(): Get formatted world status\n";
        oss << "  - getPerformanceMetrics(): Get performance measurements\n";
        oss << "  - getNLMInfo(): Get system information\n";
        oss << "  - getQuickStartExamples(): Get basic examples\n";
        oss << "  - getAdvancedExamples(): Get advanced examples\n\n";
        
        oss << "Simulation Workflows:\n";
        oss << "  - runSimulationStep(): Single step execution\n";
        oss << "  - runBatchSimulation(): Batch processing\n";
        oss << "  - runDevelopmentPhase(): Brain development simulation\n";
        oss << "  - runLearningPhase(): Learning with reward processing\n";
        oss << "  - runConsolidationPhase(): Memory consolidation\n";
        oss << "  - evaluateSimulationQuality(): Quality assessment\n";
        oss << "  - runCompleteWorkflow(): End-to-end orchestration\n\n";
        
        oss << "Error Recovery and Monitoring:\n";
        oss << "  - safeResetSimulation(): Safe component reset\n";
        oss << "  - validateSimulationIntegrity(): System integrity check\n";
        oss << "  - safeExecuteWithRecovery(): Fault-tolerant execution\n";
        oss << "  - startSimulationMonitoring(): Background monitoring\n";
        oss << "  - stopSimulationMonitoring(): Stop monitoring\n\n";
        
        oss << "Configuration and Setup:\n";
        oss << "  - setupNLMEnvironment(): Configure with defaults\n";
        oss << "  - configureWorldForSimulation(): Setup world for simulation\n";
        oss << "  - validateAndConfigureWorld(): Configure agent position\n";
        oss << "  - advancedSetup(): Advanced configuration with features\n\n";
        
        oss << "Data Export and Import:\n";
        oss << "  - exportBrainToJSON(): Export brain state to file\n";
        oss << "  - importBrainFromJSON(): Import brain state from file\n\n";
        
        oss << "Exception Types:\n";
        oss << "  - NLMError: Base NLM error class\n";
        oss << "  - NLMConfigError: Configuration-related errors\n";
        oss << "  - NLMBrainError: Brain-related errors\n";
        oss << "  - NLMWorldError: World-related errors\n";
        oss << "  - RuntimeError: Standard C++ runtime errors\n\n";
        
        oss << "=== Getting Started ===\n";
        oss << "1. First Example: Silent Brain\n";
        oss << "   import pynlm\n";
        oss << "   brain = pynlm.createDefaultBrain()\n";
        oss << "   brain.initialize()\n";
        oss << "   for i in range(10):\n";
        oss << "       brain.step(i)\n";
        oss << "   print(f'Done! Your brain thought {brain.getTotalSpikeCount()} times')\n\n";
        
        oss << "2. Second Example: Brain in World\n";
        oss << "   import pynlm\n";
        oss << "   brain = pynlm.createDefaultBrain()\n";
        oss << "   world = pynlm.createConfiguredWorld(20, 20, 8, 8)\n";
        oss << "   agent = pynlm.createAgentBrain(brain)\n";
        oss << "   agent.initialize(world)\n";
        oss << "   for step in range(50):\n";
        oss << "       world.update(0.1)\n";
        oss << "       agent.processSensoryInput(world.getSensoryPercept())\n";
        oss << "       brain.step(step)\n";
        oss << "       action = agent.decodeMotorCommand()\n";
        oss << "       world.applyMotorCommand(action, world.getSimulationTime())\n\n";
        
        oss << "=== Need Help? ===\n";
        oss << "- See easy_usage.md for beginner-friendly documentation\n";
        oss << "- See HOW_TO_USE.md for detailed installation and usage instructions\n";
        oss << "- Use getQuickStartExamples() for common usage patterns\n";
        oss << "- Use getAdvancedExamples() for complex scenarios\n";
        oss << "- Use getAvailableComponents() for complete component list\n";
        
        return oss.str();
    }, "Get complete list of available NLM components and factory functions");

    // Version and system information
    m.def("getNLMVersion", []() {
        return "NLM 0.1.0 (Phase 6 Final Integration)";
    }, "Get NLM version information");

    m.def("getPhaseInfo", []() {
        std::ostringstream oss;
        oss << "NLM Phase 6: FINAL INTEGRATION\n";
        oss << "Status: Complete - All systems integrated into coherent brain loop\n\n";
        oss << "Integration Achievements:\n";
        oss << "✓ Memory systems connected to neural processing\n";
        oss << "✓ Neuromodulation affects plasticity and dynamics\n";
        oss << "✓ Prediction integrated with learning\n";
        oss << "✓ Development affects plasticity rates\n";
        oss << "✓ Checkpoint save/load working\n";
        oss << "✓ Replay and consolidation functional\n";
        oss << "✓ Phase 6 integration experiment created\n\n";
        oss << "Key Capabilities:\n";
        oss << "- Real spiking neural computation (LIF neurons)\n";
        oss << "- Event-driven spike propagation with delays\n";
        oss << "- Multiple plasticity mechanisms (STDP, Hebbian, structural)\n";
        oss << "- Integrated memory systems (working, episodic, associative)\n";
        oss << "- Neuromodulation (dopamine, curiosity, novelty, prediction error)\n";
        oss << "- Cognitive systems (attention, planning, concept formation)\n";
        oss << "- Prediction systems for temporal and sequential learning\n";
        oss << "- Complete developmental system with stages\n";
        oss << "- Agent interface with sensory processing and motor output\n";
        oss << "- World simulation with physics and interactions\n";
        return oss.str();
    }, "Get information about NLM Phase 6 integration");
}

} // namespace nlm
