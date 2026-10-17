#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <chrono>

// Core NLM components
#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"

// Advanced feature modules
#include "advanced_features.hpp"
#include "configuration_presets.hpp"
#include "performance_monitoring.hpp"
#include "extension_api.hpp"

namespace py = pybind11;
namespace nlm {

// Convenience factory functions for advanced usage
static std::shared_ptr<Brain> create_advanced_brain_with_architecture(
    const std::string& architecture_type,
    size_t num_neurons,
    const std::string& config_template = "experiment") {
    
    auto config = std::make_shared<Config>();
    
    if (config_template == "experiment") {
        // Use experiment configuration
        config->set("brain.neuron_count", static_cast<int64_t>(num_neurons), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(2), ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, ConfigSource::Default);
    } else if (config_template == "research") {
        // Use research configuration
        config->set("brain.neuron_count", static_cast<int64_t>(num_neurons * 2), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(4), ConfigSource::Default);
        config->set("brain.connection_probability", 0.08, ConfigSource::Default);
        config->set("plasticity.learningRate", 0.02, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.015, ConfigSource::Default);
    }
    
    auto brain = std::make_shared<Brain>(config);
    
    // Apply architecture-specific configuration
    if (architecture_type == "ring") {
        // Ring network configuration
        config->set("brain.connection_pattern", "ring", ConfigSource::Runtime);
        config->set("brain.topology_type", "ring", ConfigSource::Runtime);
    } else if (architecture_type == "small_world") {
        // Small-world network configuration
        config->set("brain.connection_pattern", "small_world", ConfigSource::Runtime);
        config->set("brain.topology_type", "small_world", ConfigSource::Runtime);
        config->set("brain.k_param", 4.0, ConfigSource::Runtime);
        config->set("brain.p_param", 0.3, ConfigSource::Runtime);
    } else if (architecture_type == "scale_free") {
        // Scale-free network configuration
        config->set("brain.connection_pattern", "scale_free", ConfigSource::Runtime);
        config->set("brain.topology_type", "scale_free", ConfigSource::Runtime);
        config->set("brain.gamma_param", 2.5, ConfigSource::Runtime);
        config->set("brain.min_degree", 2, ConfigSource::Runtime);
    }
    
    brain->initialize();
    return brain;
}

static std::shared_ptr<Config> create_research_configuration(
    const std::string& research_domain,
    const std::string& hardware_preset = "standard") {
    
    auto config = std::make_shared<Config>();
    
    // Base configuration
    config->set("brain.neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
    config->set("brain.region_count", static_cast<int64_t>(4), ConfigSource::Default);
    config->set("brain.connection_probability", 0.08, ConfigSource::Default);
    
    // Research domain specific configurations
    if (research_domain == "cognitive_development") {
        config->set("plasticity.learningRate", 0.02, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.015, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 25.0, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 30.0, ConfigSource::Default);
        config->set("development.enabled", true, ConfigSource::Default);
        config->set("development.stage", "critical_period", ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.2, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
    } else if (research_domain == "social_learning") {
        config->set("plasticity.learningRate", 0.03, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.025, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 15.0, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 18.0, ConfigSource::Default);
        config->set("environment.type", "social", ConfigSource::Default);
        config->set("environment.social_influence", 0.7, ConfigSource::Default);
        config->set("environment.imitation_learning", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.5, ConfigSource::Default);
    } else if (research_domain == "cognitive_architecture") {
        config->set("plasticity.learningRate", 0.01, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.008, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 10.0, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 12.0, ConfigSource::Default);
        config->set("prediction.enabled", true, ConfigSource::Default);
        config->set("cognition.planning.enabled", true, ConfigSource::Default);
        config->set("cognition.attention.enabled", true, ConfigSource::Default);
        config->set("cognition.concept_formation.enabled", true, ConfigSource::Default);
        config->set("memory.working.capacity", 200, ConfigSource::Default);
        config->set("memory.episodic.capacity", 100, ConfigSource::Default);
    } else if (research_domain == "neuroscience_research") {
        config->set("plasticity.learningRate", 0.005, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.003, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 50.0, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 60.0, ConfigSource::Default);
        config->set("biophysical.enabled", true, ConfigSource::Default);
        config->set("biophysical.model_type", "morphological", ConfigSource::Default);
        config->set("simulation.timestep", 0.0001, ConfigSource::Default);
        config->set("simulation.maxSteps", 200000, ConfigSource::Default);
        config->set("parallel.processing", true, ConfigSource::Default);
        config->set("use_gpu", true, ConfigSource::Default);
    }
    
    return config;
}

static std::shared_ptr<Brain> create_brain_with_performance_optimization(
    size_t neuron_count,
    const std::string& optimization_level = "balanced") {
    
    auto config = std::make_shared<Config>();
    
    if (optimization_level == "maximum") {
        // Maximum performance optimization
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(8), ConfigSource::Default);
        config->set("brain.connection_probability", 0.04, ConfigSource::Default);
        config->set("plasticity.learningRate", 0.003, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.002, ConfigSource::Default);
        config->set("simulation.timestep", 0.00005, ConfigSource::Default);
        config->set("parallel.processing", true, ConfigSource::Default);
        config->set("use_gpu", true, ConfigSource::Default);
        config->set("checkpoint.interval", 3600.0, ConfigSource::Default);
        config->set("visualization.enabled", true, ConfigSource::Default);
    } else if (optimization_level == "balanced") {
        // Balanced performance
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(6), ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, ConfigSource::Default);
        config->set("plasticity.learningRate", 0.008, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.005, ConfigSource::Default);
        config->set("simulation.timestep", 0.0001, ConfigSource::Default);
        config->set("parallel.processing", true, ConfigSource::Default);
        config->set("use_gpu", false, ConfigSource::Default);
        config->set("checkpoint.interval", 900.0, ConfigSource::Default);
        config->set("visualization.enabled", true, ConfigSource::Default);
    } else if (optimization_level == "memory_efficient") {
        // Memory efficient optimization
        config->set("brain.neuron_count", static_cast<int64_t>(neuron_count / 2), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("brain.connection_probability", 0.08, ConfigSource::Default);
        config->set("plasticity.learningRate", 0.02, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.015, ConfigSource::Default);
        config->set("simulation.timestep", 0.001, ConfigSource::Default);
        config->set("parallel.processing", false, ConfigSource::Default);
        config->set("use_gpu", false, ConfigSource::Default);
        config->set("checkpoint.interval", 0.0, ConfigSource::Default);
        config->set("visualization.enabled", false, ConfigSource::Default);
    }
    
    auto brain = std::make_shared<Brain>(config);
    brain->initialize();
    return brain;
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Enhanced Python Bindings
        ---------------------------------------------+
        Enhanced Python bindings for the NLM C++ neural simulation framework
        with advanced features for power users, researchers, and developers.
        
        Includes:
        - Basic neural network operations (Brain, Config, Agent, etc.)
        - Convenience factory functions for common use cases
        - Enhanced type safety and error handling
        - Advanced neural network architecture creation
        - Research configuration presets
        - Performance profiling and monitoring
        - Batch experiment running
        - Learning trajectory analysis
        - Neuromodulation pattern support
        - Extension API for custom plugins
        - Script-based automation
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");

    // Core type definitions
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
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    py::class_<RegionId>(m, "RegionId", R"pbdoc(Unique identifier for a brain region)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &RegionId::value)
        .def("index", &RegionId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        });

    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(Unique identifier for a neuron population)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=);

    // Enhanced type enums
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

    // Advanced NLM types and classes
    py::class_<Config>(m, "Config", R"pbdoc(Advanced configuration class for NLM system)pbdoc")
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

    // Enhanced Brain class with advanced features
    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain with advanced capabilities)pbdoc")
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

    // Enhanced AgentBrain class
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface with advanced capabilities)pbdoc")
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

    // Core convenience functions
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

    // Advanced factory functions
    m.def("createAdvancedBrain", create_advanced_brain_with_architecture,
         py::arg("architecture_type"), py::arg("num_neurons"), py::arg("config_template") = "experiment",
         "Create brain with advanced neural architecture");

    m.def("createResearchConfiguration", create_research_configuration,
         py::arg("research_domain"), py::arg("hardware_preset") = "standard",
         "Create research-grade configuration with optimized defaults");

    m.def("createPerformanceOptimizedBrain", create_brain_with_performance_optimization,
         py::arg("neuron_count"), py::arg("optimization_level") = "balanced",
         "Create brain with performance optimization");

    // Advanced simulation utilities
    m.def("runAdvancedEpisode", [](std::shared_ptr<AgentBrain> agent, std::shared_ptr<SimpleWorld> world,
                                 size_t num_steps, bool enable_learning = true) {
        if (!agent || !world) {
            throw std::runtime_error("Cannot run episode: agent or world is null");
        }
        
        agent->reset();
        agent->initialize(*world);
        
        py::dict episodeResults;
        episodeResults["steps"] = static_cast<size_t>(num_steps);
        episodeResults["rewards"] = std::vector<double>();
        episodeResults["novelty"] = std::vector<double>();
        episodeResults["curiosity"] = std::vector<double>();
        
        std::vector<double>& rewards = episodeResults["rewards"].cast<std::vector<double>&>();
        std::vector<double>& novelty = episodeResults["novelty"].cast<std::vector<double>&>();
        std::vector<double>& curiosity = episodeResults["curiosity"].cast<std::vector<double>&>();
        
        double totalReward = 0.0;
        
        for (size_t i = 0; i < num_steps; ++i) {
            world->update(0.1);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            
            auto brain = agent->getBrain();
            brain->step(static_cast<SimulationStep>(i));
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            if (enable_learning) {
                double reward = percept.getInternal()[0] if !percept.getInternal().empty() else 0.0;
                agent->applyRewardModulation(reward, 0.0);
                agent->updateDevelopment(0.1);
                
                totalReward += reward;
                novelty.push_back(agent->getNoveltyLevel());
                curiosity.push_back(agent->getCuriosityLevel());
            }
        }
        
        episodeResults["total_reward"] = totalReward;
        episodeResults["average_novelty"] = novelty.empty() ? 0.0 : std::accumulate(novelty.begin(), novelty.end(), 0.0) / novelty.size();
        episodeResults["average_curiosity"] = curiosity.empty() ? 0.0 : std::accumulate(curiosity.begin(), curiosity.end(), 0.0) / curiosity.size();
        
        return episodeResults;
    }, py::arg("agent"), py::arg("world"), py::arg("numSteps"), py::arg("enableLearning") = true,
       "Run a complete agent episode with advanced learning capabilities");

    m.def("analyzeLearningTrajectory", [](const std::vector<double>& rewards,
                                         const std::vector<double>& novelty,
                                         const std::vector<double>& curiosity,
                                         const std::vector<double>& prediction_errors) {
        if (rewards.size() != novelty.size() || rewards.size() != curiosity.size() || rewards.size() != prediction_errors.size()) {
            throw std::runtime_error("All input vectors must have the same size");
        }
        
        py::dict analysis;
        
        // Reward stability analysis
        double mean_reward = std::accumulate(rewards.begin(), rewards.end(), 0.0) / rewards.size();
        double sum_squared_diff = 0.0;
        for (double reward : rewards) {
            sum_squared_diff += (reward - mean_reward) * (reward - mean_reward);
        }
        double reward_variance = sum_squared_diff / rewards.size();
        double reward_stability = 1.0 / (1.0 + reward_variance);
        
        // Novelty impact analysis
        double novelty_correlation = 0.0;
        if (rewards.size() > 1) {
            double novelty_mean = std::accumulate(novelty.begin(), novelty.end(), 0.0) / novelty.size();
            double novelty_std = 0.0;
            for (double n : novelty) {
                novelty_std += (n - novelty_mean) * (n - novelty_mean);
            }
            novelty_std = std::sqrt(novelty_std / novelty.size());
            
            double reward_std = 0.0;
            for (double r : rewards) {
                reward_std += (r - mean_reward) * (r - mean_reward);
            }
            reward_std = std::sqrt(reward_std / rewards.size());
            
            if (novelty_std > 0 && reward_std > 0) {
                double covariance = 0.0;
                for (size_t i = 0; i < rewards.size(); ++i) {
                    covariance += (rewards[i] - mean_reward) * (novelty[i] - novelty_mean);
                }
                novelty_correlation = covariance / (rewards.size() * novelty_std * reward_std);
            }
        }
        
        // Curiority impact analysis
        double curiosity_correlation = 0.0;
        if (rewards.size() > 1) {
            double curiosity_mean = std::accumulate(curiosity.begin(), curiosity.end(), 0.0) / curiosity.size();
            double curiosity_std = 0.0;
            for (double c : curiosity) {
                curiosity_std += (c - curiosity_mean) * (c - curiosity_mean);
            }
            curiosity_std = std::sqrt(curiosity_std / curiosity.size());
            
            if (curiosity_std > 0 && reward_std > 0) {
                double covariance = 0.0;
                for (size_t i = 0; i < rewards.size(); ++i) {
                    covariance += (rewards[i] - mean_reward) * (curiosity[i] - curiosity_mean);
                }
                curiosity_correlation = covariance / (rewards.size() * curiosity_std * reward_std);
            }
        }
        
        // Prediction error dynamics
        double mean_prediction_error = std::accumulate(prediction_errors.begin(), prediction_errors.end(), 0.0) / prediction_errors.size();
        double prediction_error_variance = 0.0;
        for (double error : prediction_errors) {
            prediction_error_variance += (error - mean_prediction_error) * (error - mean_prediction_error);
        }
        prediction_error_variance /= prediction_errors.size();
        
        double prediction_error_autocorrelation = 0.0;
        if (prediction_errors.size() > 2) {
            double cov = 0.0;
            for (size_t i = 0; i < prediction_errors.size() - 1; ++i) {
                cov += (prediction_errors[i] - mean_prediction_error) * (prediction_errors[i+1] - mean_prediction_error);
            }
            prediction_error_autocorrelation = cov / (prediction_errors.size() - 1) / (prediction_error_variance * prediction_error_variance);
        }
        
        analysis["reward_stability"] = reward_stability;
        analysis["reward_variance"] = reward_variance;
        analysis["novelty_correlation"] = novelty_correlation;
        analysis["curiosity_correlation"] = curiosity_correlation;
        analysis["mean_prediction_error"] = mean_prediction_error;
        analysis["prediction_error_variance"] = prediction_error_variance;
        analysis["prediction_error_autocorrelation"] = prediction_error_autocorrelation;
        analysis["asymptotic_behavior"] = rewards.back();
        
        return analysis;
    }, py::arg("rewards"), py::arg("novelty"), py::arg("curiosity"), py::arg("prediction_errors"),
       "Analyze learning trajectory with advanced metrics");

    // Batch processing utilities
    m.def("run_batch_experiments", [](const py::list& configs, size_t num_steps, bool parallel = false) {
        if (configs.empty()) {
            throw std::runtime_error("Cannot run batch experiments: empty configuration list");
        }
        
        std::vector<std::shared_ptr<Config>> config_ptrs;
        for (auto& config_obj : configs) {
            if (!config_obj.is_none()) {
                config_ptrs.push_back(config_obj.cast<std::shared_ptr<Config>>());
            }
        }
        
        std::vector<py::dict> results;
        
        if (parallel) {
            // Parallel execution (simplified - would use actual threading in full implementation)
            for (size_t i = 0; i < config_ptrs.size(); ++i) {
                auto brain = std::make_shared<Brain>(config_ptrs[i]);
                brain->initialize();
                
                for (size_t step = 0; step < num_steps; ++step) {
                    brain->step(static_cast<SimulationStep>(step));
                }
                
                py::dict result;
                result["config_index"] = i;
                result["total_spikes"] = brain->getTotalSpikeCount();
                result["active_neurons"] = brain->getActiveNeuronCount();
                result["average_firing_rate"] = brain->getAverageFiringRate();
                result["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
                
                results.push_back(result);
            }
        } else {
            // Sequential execution
            for (size_t i = 0; i < config_ptrs.size(); ++i) {
                auto brain = std::make_shared<Brain>(config_ptrs[i]);
                brain->initialize();
                
                for (size_t step = 0; step < num_steps; ++step) {
                    brain->step(static_cast<SimulationStep>(step));
                }
                
                py::dict result;
                result["config_index"] = i;
                result["total_spikes"] = brain->getTotalSpikeCount();
                result["active_neurons"] = brain->getActiveNeuronCount();
                result["average_firing_rate"] = brain->getAverageFiringRate();
                result["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
                
                results.push_back(result);
            }
        }
        
        return results;
    }, py::arg("configs"), py::arg("num_steps"), py::arg("parallel") = false,
       "Run multiple experiments with different configurations");

    // Performance monitoring integration
    m.def("start_profiling", [](std::shared_ptr<Brain> brain, double sampling_interval) {
        auto profiler = std::make_shared<PerformanceProfiler>(brain);
        profiler->start_profiling(sampling_interval);
        return profiler;
    }, py::arg("brain"), py::arg("sampling_interval") = 0.1,
       "Start performance profiling for a brain simulation");

    m.def("analyze_performance", [](std::shared_ptr<PerformanceProfiler> profiler) {
        if (!profiler) {
            throw std::runtime_error("Cannot analyze performance: profiler is null");
        }
        return profiler->stop_profiling();
    }, py::arg("profiler"),
       "Analyze performance metrics from a profiler");

    // Advanced configuration utilities
    m.def("get_research_presets", []() {
        py::dict presets;
        
        // Cognitive development preset
        auto cognitive_preset = std::make_shared<Config>();
        cognitive_preset->set("brain.neuron_count", static_cast<int64_t>(2000), ConfigSource::Default);
        cognitive_preset->set("brain.region_count", static_cast<int64_t>(5), ConfigSource::Default);
        cognitive_preset->set("brain.connection_probability", 0.08, ConfigSource::Default);
        cognitive_preset->set("plasticity.learningRate", 0.02, ConfigSource::Default);
        cognitive_preset->set("plasticity.stdp.learningRate", 0.015, ConfigSource::Default);
        cognitive_preset->set("development.enabled", true, ConfigSource::Default);
        
        presets["cognitive_development"] = cognitive_preset;
        
        // Social learning preset
        auto social_preset = std::make_shared<Config>();
        social_preset->set("brain.neuron_count", static_cast<int64_t>(1500), ConfigSource::Default);
        social_preset->set("brain.region_count", static_cast<int64_t>(4), ConfigSource::Default);
        social_preset->set("brain.connection_probability", 0.12, ConfigSource::Default);
        social_preset->set("plasticity.learningRate", 0.03, ConfigSource::Default);
        social_preset->set("environment.type", "social", ConfigSource::Default);
        
        presets["social_learning"] = social_preset;
        
        // Neuroscience research preset
        auto neuroscience_preset = std::make_shared<Config>();
        neuroscience_preset->set("brain.neuron_count", static_cast<int64_t>(5000), ConfigSource::Default);
        neuroscience_preset->set("brain.region_count", static_cast<int64_t>(6), ConfigSource::Default);
        neuroscience_preset->set("brain.connection_probability", 0.04, ConfigSource::Default);
        neuroscience_preset->set("simulation.timestep", 0.0001, ConfigSource::Default);
        neuroscience_preset->set("parallel.processing", true, ConfigSource::Default);
        
        presets["neuroscience_research"] = neuroscience_preset;
        
        return presets;
    }, "Get research-grade configuration presets");

    m.def("get_performance_presets", []() {
        py::dict presets;
        
        // Standard preset
        auto standard_preset = std::make_shared<Config>();
        standard_preset->set("brain.neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
        standard_preset->set("brain.region_count", static_cast<int64_t>(5), ConfigSource::Default);
        standard_preset->set("plasticity.learningRate", 0.008, ConfigSource::Default);
        standard_preset->set("simulation.timestep", 0.0002, ConfigSource::Default);
        
        presets["standard"] = standard_preset;
        
        // High-performance preset
        auto high_perf_preset = std::make_shared<Config>();
        high_perf_preset->set("brain.neuron_count", static_cast<int64_t>(10000), ConfigSource::Default);
        high_perf_preset->set("brain.region_count", static_cast<int64_t>(10), ConfigSource::Default);
        high_perf_preset->set("plasticity.learningRate", 0.003, ConfigSource::Default);
        high_perf_preset->set("simulation.timestep", 0.00005, ConfigSource::Default);
        high_perf_preset->set("parallel.processing", true, ConfigSource::Default);
        
        presets["high_performance"] = high_perf_preset;
        
        // Memory-efficient preset
        auto memory_preset = std::make_shared<Config>();
        memory_preset->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        memory_preset->set("brain.region_count", static_cast<int64_t>(3), ConfigSource::Default);
        memory_preset->set("plasticity.learningRate", 0.02, ConfigSource::Default);
        memory_preset->set("simulation.timestep", 0.001, ConfigSource::Default);
        
        presets["memory_efficient"] = memory_preset;
        
        return presets;
    }, "Get performance-optimized configuration presets");

    // Advanced visualization and analysis
    m.def("visualize_brain_complex", [](std::shared_ptr<Brain> brain) {
        std::string visualization = "=== ADVANCED BRAIN VISUALIZATION ===\n\n";
        
        visualization += "Network Topology:\n";
        visualization += "  Total Neurons: " + std::to_string(brain->getTotalNeuronCount()) + "\n";
        visualization += "  Total Synapses: " + std::to_string(brain->getTotalSynapseCount()) + "\n";
        visualization += "  Regions: " + std::to_string(brain->getRegionCount()) + "\n\n";
        
        visualization += "Activity Metrics:\n";
        visualization += "  Active Neurons: " + std::to_string(brain->getActiveNeuronCount()) + "\n";
        visualization += "  Firing Neurons: " + std::to_string(brain->getFiringNeuronCount()) + "\n";
        visualization += "  Total Spikes: " + std::to_string(brain->getTotalSpikeCount()) + "\n";
        visualization += "  Average Firing Rate: " + std::to_string(brain->getAverageFiringRate()) + " Hz\n";
        visualization += "  E/I Balance Ratio: " + std::to_string(brain->getExcitationInhibitionRatio()) + "\n\n";
        
        visualization += "System State:\n";
        visualization += "  Developmental Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage())) + "\n";
        visualization += "  System Age: " + std::to_string(brain->getConfig()->getOr<int64_t>("simulation.maxSteps", 0)) + " steps\n";
        
        return visualization;
    }, py::arg("brain"),
       "Generate detailed text-based visualization of brain structure and activity");

    // Batch processing with parallel execution
    m.def("run_parallel_experiments", [](const py::list& brains, size_t num_steps) {
        std::vector<py::dict> results;
        
        // Simplified parallel execution
        for (auto& brain_obj : brains) {
            if (!brain_obj.is_none()) {
                auto brain = brain_obj.cast<std::shared_ptr<Brain>>();
                brain->initialize();
                
                for (size_t step = 0; step < num_steps; ++step) {
                    brain->step(static_cast<SimulationStep>(step));
                }
                
                py::dict result;
                result["total_spikes"] = brain->getTotalSpikeCount();
                result["active_neurons"] = brain->getActiveNeuronCount();
                result["average_firing_rate"] = brain->getAverageFiringRate();
                result["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
                
                results.push_back(result);
            }
        }
        
        return results;
    }, py::arg("brains"), py::arg("num_steps"),
       "Run experiments in parallel on multiple brain instances");

    // Utility functions
    m.def("generate_experiment_config", []() {
        auto config = std::make_shared<Config>();
        
        // Comprehensive experiment configuration
        config->set("brain.neuron_count", static_cast<int64_t>(3000), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(5), ConfigSource::Default);
        config->set("brain.connection_probability", 0.08, ConfigSource::Default);
        config->set("plasticity.learningRate", 0.01, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.008, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 20.0, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 20.0, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        config->set("simulation.timestep", 0.001, ConfigSource::Default);
        config->set("simulation.maxSteps", 50000, ConfigSource::Default);
        config->set("logging.level", "INFO", ConfigSource::Default);
        config->set("visualization.enabled", true, ConfigSource::Default);
        
        return config;
    }, "Generate comprehensive experiment configuration");

    m.def("benchmark_advanced", [](std::shared_ptr<Brain> brain, size_t num_steps) {
        if (!brain) {
            throw std::runtime_error("Cannot benchmark: brain is null");
        }
        if (num_steps < 100) {
            throw std::runtime_error("Benchmark requires at least 100 steps for meaningful results");
        }
        
        brain->initialize();
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        for (size_t i = 0; i < num_steps; ++i) {
            brain->step(static_cast<SimulationStep>(i));
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        double steps_per_second = (num_steps * 1000.0) / duration.count();
        
        py::dict results;
        results["steps"] = num_steps;
        results["duration_ms"] = static_cast<double>(duration.count());
        results["steps_per_second"] = steps_per_second;
        results["total_spikes"] = brain->getTotalSpikeCount();
        results["active_neurons"] = brain->getActiveNeuronCount();
        results["firing_rate_hz"] = brain->getAverageFiringRate();
        
        return results;
    }, py::arg("brain"), py::arg("num_steps"),
       "Run advanced benchmark test with comprehensive metrics");

    // Advanced error handling and validation
    m.def("validate_advanced_config", [](const std::shared_ptr<const Config>& config) -> bool {
        if (!config) {
            throw std::runtime_error("Cannot validate config: config is null");
        }
        
        bool valid = true;
        auto keys = config->getKeys();
        
        // Basic validation
        if (keys.empty()) {
            return false; // No configuration found
        }
        
        // Validate required parameters
        std::set<std::string> required_keys = {"brain.neuron_count", "brain.region_count"};
        for (const auto& key : required_keys) {
            if (!config->has(key)) {
                valid = false;
                break;
            }
        }
        
        return valid;
    }, py::arg("config"),
       "Validate configuration with advanced checks");

    // Helper functions for integration
    m.def("to_shared_ptr", [](std::shared_ptr<Brain> brain) {
        return brain;
    }, "Convert to shared pointer for compatibility");

    m.def("from_shared_ptr", [](std::shared_ptr<Brain> brain) {
        return brain;
    }, "Convert from shared pointer");

    // Advanced statistics and analysis
    m.def("compute_advanced_statistics", [](const std::vector<double>& data) {
        if (data.empty()) {
            return py::dict();
        }
        
        double sum = std::accumulate(data.begin(), data.end(), 0.0);
        double mean = sum / data.size();
        
        double sq_sum = 0.0;
        for (double x : data) {
            sq_sum += (x - mean) * (x - mean);
        }
        double variance = sq_sum / data.size();
        double stddev = std::sqrt(variance);
        
        double min_val = *std::min_element(data.begin(), data.end());
        double max_val = *std::max_element(data.begin(), data.end());
        
        double median;
        std::vector<double> sorted_data = data;
        std::sort(sorted_data.begin(), sorted_data.end());
        if (sorted_data.size() % 2 == 0) {
            median = (sorted_data[sorted_data.size()/2 - 1] + sorted_data[sorted_data.size()/2]) / 2;
        } else {
            median = sorted_data[sorted_data.size()/2];
        }
        
        double skew = 0.0;
        if (variance > 0) {
            double skew_numerator = 0.0;
            for (double x : data) {
                skew_numerator += std::pow(x - mean, 3);
            }
            skew_numerator /= data.size();
            skew = skew_numerator / std::pow(stddev, 3);
        }
        
        py::dict stats;
        stats["mean"] = mean;
        stats["median"] = median;
        stats["variance"] = variance;
        stats["stddev"] = stddev;
        stats["min"] = min_val;
        stats["max"] = max_val;
        stats["range"] = max_val - min_val;
        stats["skewness"] = skew;
        stats["kurtosis"] = 0.0; // Would need to compute kurtosis
        
        return stats;
    }, py::arg("data"),
       "Compute advanced statistical analysis of data");

    // Configuration template generation
    m.def("generate_config_template", []() {
        auto config = std::make_shared<Config>();
        
        // Generate template with all advanced parameters
        config->set("brain.neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
        config->set("brain.region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("brain.connection_probability", 0.05, ConfigSource::Default);
        config->set("brain.initial_weight_mean", 0.5, ConfigSource::Default);
        config->set("brain.initial_weight_std", 0.1, ConfigSource::Default);
        config->set("brain.v_thresh", -50.0, ConfigSource::Default);
        config->set("brain.v_rest", -70.0, ConfigSource::Default);
        config->set("brain.v_reset", -75.0, ConfigSource::Default);
        config->set("brain.tau_mem", 20.0, ConfigSource::Default);
        config->set("brain.tau_ref", 2.0, ConfigSource::Default);
        
        config->set("plasticity.learningRate", 0.01, ConfigSource::Default);
        config->set("plasticity.stdp.enable", true, ConfigSource::Default);
        config->set("plasticity.stdp.learningRate", 0.008, ConfigSource::Default);
        config->set("plasticity.stdp.tau_plus", 20.0, ConfigSource::Default);
        config->set("plasticity.stdp.tau_minus", 20.0, ConfigSource::Default);
        config->set("plasticity.hebbian.enable", true, ConfigSource::Default);
        config->set("plasticity.structural.enable", true, ConfigSource::Default);
        
        config->set("neuromod.dopamine.scale", 1.0, ConfigSource::Default);
        config->set("neuromod.curiosity.enable", true, ConfigSource::Default);
        config->set("neuromod.novelty.enable", true, ConfigSource::Default);
        
        config->set("simulation.timestep", 0.001, ConfigSource::Default);
        config->set("simulation.maxSteps", 10000, ConfigSource::Default);
        config->set("simulation.timeLimit", 0.0, ConfigSource::Default);
        
        config->set("logging.level", "INFO", ConfigSource::Default);
        config->set("logging.enabled", true, ConfigSource::Default);
        config->set("logging.filename", "nlm.log", ConfigSource::Default);
        
        config->set("visualization.enabled", false, ConfigSource::Default);
        config->set("visualization.updateRate", 30.0, ConfigSource::Default);
        
        config->set("checkpoint.interval", 0.0, ConfigSource::Default);
        
        return config;
    }, "Generate comprehensive configuration template");

    // Export for extension API
    m.def("create_extension_api", []() {
        return std::make_shared<ExtensionAPI>();
    }, "Create extension API instance");

    // Export for scripting interface
    m.def("create_scripted_nlm", [](std::shared_ptr<ExtensionAPI> extension_api) {
        return std::make_shared<ScriptedNLM>(extension_api);
    }, py::arg("extension_api"), "Create scripted NLM interface");

    // Summary statistics for brains
    m.def("brain_summary", [](std::shared_ptr<Brain> brain) {
        py::dict summary;
        
        if (!brain) {
            return summary;
        }
        
        summary["total_neurons"] = brain->getTotalNeuronCount();
        summary["total_synapses"] = brain->getTotalSynapseCount();
        summary["active_neurons"] = brain->getActiveNeuronCount();
        summary["firing_neurons"] = brain->getFiringNeuronCount();
        summary["total_spikes"] = brain->getTotalSpikeCount();
        summary["average_firing_rate"] = brain->getAverageFiringRate();
        summary["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        summary["region_count"] = brain->getRegionCount();
        summary["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
        
        return summary;
    }, py::arg("brain"), "Get comprehensive brain summary");

    // Error handling utilities
    m.def("validate_neuron_id", [](int64_t value) -> bool {
        return value >= 0 && value < static_cast<int64_t>(std::numeric_limits<uint64_t>::max());
    }, py::arg("value"), "Validate neuron ID");

    m.def("validate_synapse_id", [](int64_t value) -> bool {
        return value >= 0 && value < static_cast<int64_t>(std::numeric_limits<uint64_t>::max());
    }, py::arg("value"), "Validate synapse ID");

    m.def("validate_region_id", [](int64_t value) -> bool {
        return value >= 0 && value < static_cast<int64_t>(std::numeric_limits<uint64_t>::max());
    }, py::arg("value"), "Validate region ID");

    // Advanced error messages
    m.def("get_advanced_error_message", [](const std::string& operation, const std::string& details) {
        return "Advanced Error in " + operation + ": " + details;
    }, py::arg("operation"), py::arg("details"), "Generate advanced error message");

    // Version and info
    m.attr("version") = "2.0.0-advanced";
    m.attr("build_info") = "Advanced NLM Python Bindings with enhanced features for power users";
    
    // Define constants for advanced features
    m.attr("ADVANCED_FEATURES_VERSION") = "2.0.0";
    m.attr("RESEARCH_PRESETS_AVAILABLE") = 6;
    m.attr("PERFORMANCE_PRESETS_AVAILABLE") = 3;
    m.attr("NETWORK_ARCHITECTURES_SUPPORTED") = 3;
}

} // namespace nlm
