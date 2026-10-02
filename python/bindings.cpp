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
        .def_readwrite("active", &WorldObject::active)
        .def("to_dict", [](const WorldObject& self) {
            return py::dict("x"_a = self.x,
                           "y"_a = self.y,
                           "radius"_a = self.radius,
                           "type"_a = static_cast<int>(self.type),
                           "value"_a = self.value,
                           "active"_a = self.active);
        }, "Convert WorldObject to dictionary")
        .def("__repr__", [](const WorldObject& self) {
            return "<WorldObject: x=" + std::to_string(self.x) +
                   ", y=" + std::to_string(self.y) +
                   ", type=" + std::to_string(static_cast<int>(self.type)) + ">";
        });

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
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& self) {
            return py::dict("reward"_a = self.reward,
                           "success"_a = self.success,
                           "message"_a = self.message);
        }, "Convert ActionResult to dictionary")
        .def("__repr__", [](const ActionResult& self) {
            return "<ActionResult: reward=" + std::to_string(self.reward) +
                   ", success=" + std::to_string(self.success) +
                   ", message=\"" + self.message + "\">";
        });

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
    }, "Create a default configuration with sensible NLM defaults");
    
    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), "Create a new brain with configuration");
    
    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world environment");
    
    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");
    
    // Factory function for common configurations
    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Set NLM Phase 6 development configuration
        config->set("neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.1f, ConfigSource::Default);
        
        // Enable all plasticity systems
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        // Neuromodulation settings
        config->set("neuromod.dopamine.scale", 1.0f, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Development parameters
        config->set("synaptogenesis_rate", 0.0002f, ConfigSource::Default);
        config->set("pruning_rate", 0.00002f, ConfigSource::Default);
        
        return config;
    }, "Create a development configuration optimized for NLM Phase 6");
    
    // Factory function for curiosity-driven exploration
    m.def("createExplorationConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Basic configuration
        config->set("neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("connection_probability", 0.08f, ConfigSource::Default);
        
        // Enable curiosity for exploration
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        // Higher novelty threshold for more exploration
        config->set("neuromod.curiosity.threshold", 0.5f, ConfigSource::Default);
        config->set("neuromod.novelty.threshold", 0.3f, ConfigSource::Default);
        
        return config;
    }, "Create an exploration configuration optimized for curiosity-driven learning");
    
    // Factory function for rapid testing
    m.def("createQuickTestConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        // Small brain for quick testing
        config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.05f, ConfigSource::Default);
        
        // Minimal plasticity for speed
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", false, ConfigSource::Default);
        config->set("plasticity.structural.enable", false, ConfigSource::Default);
        
        return config;
    }, "Create a quick test configuration with small brain size");
    
    // Validation and utility functions
    m.def("validateConfig", [](const Config& config) {
        std::vector<std::string> errors;
        
        if (!config.has("neuron_count")) {
            errors.push_back("Missing 'neuron_count' in config");
        } else {
            int64_t neuronCount = config.getOr<int64_t>("neuron_count", 0);
            if (neuronCount < 10 || neuronCount > 10000) {
                errors.push_back("neuron_count should be between 10 and 10000");
            }
        }
        
        if (config.has("connection_probability")) {
            float prob = config.getOr<float>("connection_probability", 0.0f);
            if (prob < 0.0f || prob > 1.0f) {
                errors.push_back("connection_probability should be between 0.0 and 1.0");
            }
        }
        
        return errors;
    }, py::arg("config"), "Validate configuration and return list of errors (empty if valid)");
    
    // Configuration utilities
    m.def("mergeConfigs", [](const Config& config1, const Config& config2) {
        auto merged = std::make_shared<Config>();
        
        // Copy config1
        for (const auto& key : config1.getKeys()) {
            if (config1.has<std::string>(key)) {
                merged->set(key, config1.get<std::string>(key));
            } else if (config1.has<int64_t>(key)) {
                merged->set(key, config1.get<int64_t>(key));
            } else if (config1.has<double>(key)) {
                merged->set(key, config1.get<double>(key));
            } else if (config1.has<float>(key)) {
                merged->set(key, config1.get<float>(key));
            }
        }
        
        // Override with config2
        for (const auto& key : config2.getKeys()) {
            if (config2.has<std::string>(key)) {
                merged->set(key, config2.get<std::string>(key));
            } else if (config2.has<int64_t>(key)) {
                merged->set(key, config2.get<int64_t>(key));
            } else if (config2.has<double>(key)) {
                merged->set(key, config2.get<double>(key));
            } else if (config2.has<float>(key)) {
                merged->set(key, config2.get<float>(key));
            }
        }
        
        return merged;
    }, py::arg("config1"), py::arg("config2"), "Merge two configurations (config2 overrides config1)");
    
    m.def("configToDict", [](const Config& config) {
        py::dict dict;
        
        for (const auto& key : config.getKeys()) {
            if (config.has<std::string>(key)) {
                dict[key] = config.get<std::string>(key);
            } else if (config.has<int64_t>(key)) {
                dict[key] = config.get<int64_t>(key);
            } else if (config.has<double>(key)) {
                dict[key] = config.get<double>(key);
            } else if (config.has<float>(key)) {
                dict[key] = config.get<float>(key);
            } else if (config.has<bool>(key)) {
                dict[key] = config.get<bool>(key);
            }
        }
        
        return dict;
    }, py::arg("config"), "Convert configuration to dictionary");
    
    m.def("dictToConfig", [](py::dict dict) -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        
        for (auto item : dict) {
            std::string key = py::str(item.first);
            
            if (py::isinstance(item.second, py::int_())) {
                int64_t value = item.second.cast<int64_t>();
                config->set(key, value);
            } else if (py::isinstance(item.second, py::float_())) {
                float value = item.second.cast<float>();
                config->set(key, value);
            } else if (py::isinstance(item.second, py::str())) {
                std::string value = item.second.cast<std::string>();
                config->set(key, value);
            } else if (py::isinstance(item.second, py::bool_())) {
                bool value = item.second.cast<bool>();
                config->set(key, value);
            }
        }
        
        return config;
    }, py::arg("dict"), "Create configuration from dictionary");
    
    // Brain utilities
    m.def("compareBrains", [](const Brain& brain1, const Brain& brain2) {
        std::unordered_map<std::string, std::string> differences;
        
        // Compare neuron counts
        size_t neurons1 = brain1.getTotalNeuronCount();
        size_t neurons2 = brain2.getTotalNeuronCount();
        if (neurons1 != neurons2) {
            differences["neuron_count"] = "brain1: " + std::to_string(neurons1) +
                                         ", brain2: " + std::to_string(neurons2);
        }
        
        // Compare synapse counts
        size_t synapses1 = brain1.getTotalSynapseCount();
        size_t synapses2 = brain2.getTotalSynapseCount();
        if (synapses1 != synapses2) {
            differences["synapse_count"] = "brain1: " + std::to_string(synapses1) +
                                          ", brain2: " + std::to_string(synapses2);
        }
        
        // Compare spike counts
        size_t spikes1 = brain1.getTotalSpikeCount();
        size_t spikes2 = brain2.getTotalSpikeCount();
        if (spikes1 != spikes2) {
            differences["total_spikes"] = "brain1: " + std::to_string(spikes1) +
                                          ", brain2: " + std::to_string(spikes2);
        }
        
        return differences;
    }, py::arg("brain1"), py::arg("brain2"), "Compare two brains and return differences");
    
    m.def("analyzeBrainState", [](const Brain& brain) {
        py::dict analysis;
        
        // Basic statistics
        analysis["total_neurons"] = brain.getTotalNeuronCount();
        analysis["total_synapses"] = brain.getTotalSynapseCount();
        analysis["active_neurons"] = brain.getActiveNeuronCount();
        analysis["firing_neurons"] = brain.getFiringNeuronCount();
        analysis["total_spikes"] = brain.getTotalSpikeCount();
        analysis["average_firing_rate"] = brain.getAverageFiringRate();
        analysis["e_i_ratio"] = brain.getExcitationInhibitionRatio();
        analysis["region_count"] = brain.getRegionCount();
        analysis["developmental_stage"] = static_cast<int>(brain.getDevelopmentalStage());
        
        // Calculate derived metrics
        if (brain.getTotalNeuronCount() > 0) {
            float firing_percentage = (static_cast<float>(brain.getFiringNeuronCount()) / 
                                     brain.getTotalNeuronCount()) * 100.0f;
            analysis["firing_percentage"] = firing_percentage;
        }
        
        if (brain.getTotalSynapseCount() > 0) {
            float avg_synapse_weight = brain.getRegion(RegionId(1)) ? 
                brain.getRegion(RegionId(1))->getAverageSynapticWeight() : 0.0f;
            analysis["average_synapse_weight"] = avg_synapse_weight;
        }
        
        return analysis;
    }, py::arg("brain"), "Analyze brain state and return statistics dictionary");
    
    // Performance monitoring
    m.def("profileBrainSimulation", [](std::shared_ptr<Brain> brain, size_t steps) {
        std::chrono::high_resolution_clock::time_point start = 
            std::chrono::high_resolution_clock::now();
        
        for (size_t step = 0; step < steps; ++step) {
            brain->step(step);
        }
        
        std::chrono::high_resolution_clock::time_point end = 
            std::chrono::high_resolution_clock::now();
        
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        
        py::dict profile;
        profile["total_steps"] = steps;
        profile["total_time_ms"] = static_cast<double>(duration.count());
        profile["steps_per_second"] = (steps * 1000.0) / duration.count();
        profile["final_spikes"] = brain->getTotalSpikeCount();
        profile["final_firing"] = brain->getFiringNeuronCount();
        profile["average_step_time_ms"] = duration.count() / static_cast<double>(steps);
        
        return profile;
    }, py::arg("brain"), py::arg("steps"), "Profile brain simulation performance");
    
    // Batch processing utilities
    m.def("runMultipleSimulations", [](const std::vector<std::shared_ptr<Config>>& configs, 
                                       size_t stepsPerConfig) {
        std::vector<py::dict> results;
        
        for (size_t i = 0; i < configs.size(); ++i) {
            auto brain = std::make_shared<Brain>(configs[i]);
            brain->initialize();
            
            for (size_t step = 0; step < stepsPerConfig; ++step) {
                brain->step(step);
            }
            
            py::dict result;
            result["config_index"] = static_cast<int64_t>(i);
            result["total_spikes"] = brain->getTotalSpikeCount();
            result["firing_neurons"] = brain->getFiringNeuronCount();
            result["average_firing_rate"] = brain->getAverageFiringRate();
            result["final_neurons"] = brain->getTotalNeuronCount();
            
            results.push_back(result);
        }
        
        return results;
    }, py::arg("configs"), py::arg("stepsPerConfig"), "Run multiple brain simulations with different configurations");
    
    // Debugging utilities
    m.def("debugBrain", [](const Brain& brain) {
        std::stringstream debug;
        
        debug << "=== BRAIN DEBUG INFO ===" << std::endl;
        debug << "Total neurons: " << brain.getTotalNeuronCount() << std::endl;
        debug << "Total synapses: " << brain.getTotalSynapseCount() << std::endl;
        debug << "Active neurons: " << brain.getActiveNeuronCount() << std::endl;
        debug << "Firing neurons: " << brain.getFiringNeuronCount() << std::endl;
        debug << "Total spikes: " << brain.getTotalSpikeCount() << std::endl;
        debug << "E/I ratio: " << brain.getExcitationInhibitionRatio() << std::endl;
        debug << "Region count: " << brain.getRegionCount() << std::endl;
        debug << "Developmental stage: " << static_cast<int>(brain.getDevelopmentalStage()) << std::endl;
        
        if (brain.getRegionCount() > 0) {
            auto* region = brain.getRegion(RegionId(1));
            if (region) {
                debug << "Region 1 neurons: " << region->getTotalNeuronCount() << std::endl;
                debug << "Region 1 synapses: " << region->getSynapseCount() << std::endl;
                debug << "Region 1 average weight: " << region->getAverageSynapticWeight() << std::endl;
            }
        }
        
        return debug.str();
    }, py::arg("brain"), "Generate debug information about brain state");
    
    // Version and system information
    m.attr("__version__") = "0.1.0";
    m.attr("__nlm_version__") = "Phase 6 Final Integration";
    m.attr("__python_version__") = PYBIND11_VERSION;
    m.attr("__system_info__") = std::string("NLM Python Bindings v0.1.0 - Phase 6 Final Integration");
    
    // Performance constants
    m.attr("DEFAULT_NEURON_COUNT") = 1000;
    m.attr("DEFAULT_REGION_COUNT") = 1;
    m.attr("DEFAULT_CONNECTION_PROBABILITY") = 0.1f;
    m.attr("MAX_NEURON_COUNT") = 10000;
    m.attr("MIN_NEURON_COUNT") = 10;
    m.attr("DEFAULT_SIMULATION_STEPS") = 1000;
    m.attr("DEFAULT_WORLD_WIDTH") = 20;
    m.attr("DEFAULT_WORLD_HEIGHT") = 20;
    m.attr("DEFAULT_VISION_SIZE") = 8;
    
    // Configuration presets
    m.def("getQuickExperimentConfig", []() {
        auto config = std::make_shared<Config>();
        
        config->set("neuron_count", static_cast<int64_t>(200));
        config->set("region_count", static_cast<int64_t>(1));
        config->set("connection_probability", 0.05f);
        config->set("simulation_timestep", 0.001);
        
        return config;
    }, "Get quick experiment configuration");
    
    m.def("getDevelopmentConfig", []() {
        auto config = std::make_shared<Config>();
        
        config->set("neuron_count", static_cast<int64_t>(2000));
        config->set("region_count", static_cast<int64_t>(3));
        config->set("connection_probability", 0.1f);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("plasticity.structural.enable", true);
        
        return config;
    }, "Get development configuration");
    
    m.def("getExplorationConfig", []() {
        auto config = std::make_shared<Config>();
        
        config->set("neuron_count", static_cast<int64_t>(1500));
        config->set("region_count", static_cast<int64_t>(2));
        config->set("connection_probability", 0.08f);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        
        return config;
    }, "Get exploration configuration");
    
    } // namespace nlm
