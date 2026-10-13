#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cmath>
#include <nlohmann/json.hpp>

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
        
        Key Features:
        - Event-driven spiking neural networks
        - Integrated memory systems (working, episodic, associative)
        - Neuromodulation (dopamine, curiosity, novelty)
        - Prediction and planning capabilities
        - Developmental plasticity
        - Complex behavior learning
        
        Usage Examples:
        
        >>> import pynlm
        >>> 
        >>> # Basic brain setup
        >>> config = pynlm.createDefaultConfig()
        >>> brain = pynlm.createBrain(config)
        >>> brain.initialize()
        >>> 
        >>> # Simple simulation
        >>> for i in range(100):
        ...     brain.step(i)
        >>> 
        >>> # Get statistics
        >>> neurons = brain.getTotalNeuronCount()
        >>> spikes = brain.getTotalSpikeCount()
        >>> 
        >>> # World simulation
        >>> world = pynlm.createSimpleWorld()
        >>> world.configure(20, 20, 8, 8)
        >>> agent = pynlm.createAgentBrain(brain)
        >>> agent.initialize(world)
        
        >>> # Advanced usage with convenience functions
        >>> config = pynlm.createStandardConfig()
        >>> stats = pynlm.getBrainStatistics(brain)
        >>> print(stats)
        >>> 
        >>> # Training setup
        >>> world, brain, agent = pynlm.createTrainingEnvironment(30, 30, 6, 6)
        >>> config = pynlm.createTrainingConfig(500)
    )pbdoc";

    // Custom exception classes for better error handling
    py::class_<std::runtime_error>(m, "RuntimeError").def(py::init<const char*>());
    
    py::register_exception<nlm::ConfigurationError>(m, "ConfigurationError");
    py::register_exception<nlm::BrainError>(m, "BrainError");
    py::register_exception<nlm::WorldError>(m, "WorldError");
    py::register_exception<nlm::MemoryError>(m, "MemoryError");
    py::register_exception<nlm::ValidationError>(m, "ValidationError");
    py::register_exception<nlm::PlasticityError>(m, "PlasticityError");
    py::register_exception<nlm::DevelopmentError>(m, "DevelopmentError");

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

    // ========== CONVENIENCE FUNCTIONS ==========

    // Helper functions for common configuration presets
    m.def("createStandardConfig", []() {
        auto config = std::make_shared<Config>();
        
        // Standard NLM configuration for general use
        config->set("brain.region.count", 4);
        config->set("brain.region.initialSize", 100);
        config->set("brain.region.connectivity", 0.15f);
        config->set("brain.synapse.stdp.learningRate", 0.01f);
        config->set("brain.synapse.hebbian.strength", 0.005f);
        config->set("brain.plasticity.structural.growthRate", 0.001f);
        config->set("brain.development.initialStage", "CriticalPeriod");
        config->set("brain.development.plasticity.rate", 1.0f);
        config->set("brain.memory.working.capacity", 50);
        config->set("brain.memory.episodic.capacity", 20);
        config->set("brain.neuromodulation.dopamine.baseLevel", 0.1f);
        config->set("brain.neuromodulation.curiosity.sensitivity", 0.5f);
        config->set("brain.neuromodulation.novelty.threshold", 0.3f);
        config->set("brain.prediction.accuracy", 0.8f);
        config->set("brain.cognition.planning.horizons", 5);
        
        return config;
    }, "Create a standard configuration for general NLM use");

    // Helper function for batch neuron current injection
    m.def("injectCurrentToTypeRange", [](std::shared_ptr<Brain> brain, 
                                        NeuronType type, 
                                        MembranePotential current,
                                        const std::vector<NeuronId>& neuronIds) {
        // Inject to all neurons of specified type plus specific IDs
        brain->injectCurrentToNeurons(type, current);
        for (const auto& neuronId : neuronIds) {
            brain->injectCurrent(neuronId, current);
        }
    }, py::arg("brain"), py::arg("type"), py::arg("current"), 
       py::arg("neuronIds"), 
       "Inject current to all neurons of a type and specific neurons");

    // Batch operation to get comprehensive brain statistics
    m.def("getBrainStatistics", [](const std::shared_ptr<Brain>& brain) {
        nlohmann::json stats;
        
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["avg_firing_rate"] = brain->getAverageFiringRate();
        stats["exc_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["region_count"] = brain->getRegionCount();
        
        if (auto* workingMem = brain->getWorkingMemory()) {
            stats["working_memory_capacity"] = workingMem->getCapacity();
            stats["working_memory_utilization"] = workingMem->getUtilization();
        }
        
        if (auto* episodicMem = brain->getEpisodicMemory()) {
            stats["episodic_memory_capacity"] = episodicMem->getCapacity();
            stats["episodic_memory_count"] = episodicMem->getCount();
        }
        
        if (auto* predictionSys = brain->getPredictionSystem()) {
            stats["prediction_accuracy"] = predictionSys->getAccuracy();
            stats["prediction_error"] = predictionSys->getPredictionError();
        }
        
        if (auto* devSystem = brain->getDevelopmentSystem()) {
            stats["developmental_stage"] = static_cast<int>(brain->getDevelopmentalStage());
            stats["development_plasticity"] = devSystem->getPlasticityRate();
        }
        
        return stats.dump();
    }, py::arg("brain"), 
       "Get comprehensive brain statistics as JSON string");

    // Validate configuration for common errors
    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        nlohmann::json errors = nlohmann::json::array();
        
        // Check for required keys
        auto keys = config->getKeys();
        
        // Validate brain region count
        if (!config->has("brain.region.count")) {
            errors.push_back("Missing brain.region.count");
        }
        
        // Validate neuron type distributions
        if (config->has("brain.region.distribution")) {
            auto dist = config->get<std::string>("brain.region.distribution");
            if (dist != "balanced" && dist != "asymmetric" && dist != "focused") {
                errors.push_back("Invalid brain.region.distribution value");
            }
        }
        
        // Validate plasticity parameters
        if (config->has("brain.synapse.stdp.learningRate")) {
            float lr = config->getOr<float>("brain.synapse.stdp.learningRate", 0.0f);
            if (lr < 0.0f || lr > 1.0f) {
                errors.push_back("brain.synapse.stdp.learningRate must be between 0.0 and 1.0");
            }
        }
        
        // Validate memory capacities
        if (config->has("brain.memory.working.capacity")) {
            int capacity = config->getOr<int>("brain.memory.working.capacity", 0);
            if (capacity < 0) {
                errors.push_back("brain.memory.working.capacity must be non-negative");
            }
        }
        
        // Validate neuromodulation levels
        if (config->has("brain.neuromodulation.dopamine.baseLevel")) {
            float level = config->getOr<float>("brain.neuromodulation.dopamine.baseLevel", 0.0f);
            if (level < 0.0f || level > 1.0f) {
                errors.push_back("brain.neuromodulation.dopamine.baseLevel must be between 0.0 and 1.0");
            }
        }
        
        return errors.dump();
    }, py::arg("config"), 
       "Validate configuration and return error messages as JSON string");

    // Safe brain initialization with error checking
    m.def("safeBrainInitialize", [](std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw BrainError("Cannot initialize null brain");
        }
        
        if (!brain->initialize()) {
            throw BrainError("Brain initialization failed");
        }
        
        // Verify brain is in valid state after initialization
        size_t neuronCount = brain->getTotalNeuronCount();
        if (neuronCount == 0) {
            throw BrainError("Brain initialized but has zero neurons");
        }
        
        return true;
    }, py::arg("brain"), 
       "Safely initialize brain with validation and error checking");

    // Simulation progress tracking helper
    m.def("runSimulationWithProgress", 
          [](std::shared_ptr<Brain> brain, 
             std::shared_ptr<SimpleWorld> world,
             std::shared_ptr<AgentBrain> agent,
             size_t numSteps,
             std::function<void(size_t, float)> progressCallback = nullptr) {
              
              if (!brain || !world || !agent) {
                  throw WorldError("Null brain, world, or agent provided");
              }
              
              size_t completed = 0;
              auto startTime = std::chrono::steady_clock::now();
              
              for (size_t step = 0; step < numSteps; ++step) {
                  // Update world
                  world->update(0.1f);
                  
                  // Get sensory input and process
                  auto percept = world->getSensoryPercept();
                  agent->processSensoryInput(percept);
                  
                  // Brain processes
                  brain->step(step);
                  
                  // Get action and apply
                  auto action = agent->decodeMotorCommand();
                  world->applyMotorCommand(action, world->getSimulationTime());
                  
                  completed++;
                  
                  // Calculate progress
                  float progress = (static_cast<float>(completed) / numSteps) * 100.0f;
                  
                  // Call progress callback if provided
                  if (progressCallback) {
                      progressCallback(completed, progress);
                  }
                  
                  // Log every 100 steps
                  if (step % 100 == 0) {
                      brain->logStatus();
                  }
              }
              
              auto endTime = std::chrono::steady_clock::now();
              auto duration = std::chrono::duration_cast<std::chrono::seconds>(endTime - startTime);
              
              return std::make_pair(completed, duration.count());
          }, py::arg("brain"), py::arg("world"), py::arg("agent"), 
             py::arg("numSteps"), py::arg("progressCallback") = nullptr,
             "Run complete simulation loop with progress tracking and error checking");

    // Configuration serialization helper
    m.def("serializeConfig", [](const std::shared_ptr<Config>& config, 
                                const std::string& filepath) {
              if (!config->saveToFile(filepath)) {
                  throw ConfigurationError("Failed to save configuration to file: " + filepath);
              }
              return true;
          }, py::arg("config"), py::arg("filepath"),
          "Save configuration to file with error checking");

    // Configuration loading helper
    m.def("loadConfig", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from file: " + filepath);
        }
        return config;
    }, py::arg("filepath"),
       "Load configuration from file with error checking");

    // Brain checkpoint helper
    m.def("checkpointBrain", [](const std::shared_ptr<Brain>& brain, 
                               const std::string& filepath) {
              if (!brain->save(filepath)) {
                  throw BrainError("Failed to save brain checkpoint: " + filepath);
              }
              return true;
          }, py::arg("brain"), py::arg("filepath"),
          "Save brain state as checkpoint with error checking");

    // Brain restore helper
    m.def("restoreBrain", [](const std::shared_ptr<Brain>& brain, 
                             const std::string& filepath) {
              if (!brain->load(filepath)) {
                  throw BrainError("Failed to load brain checkpoint: " + filepath);
              }
              return true;
          }, py::arg("brain"), py::arg("filepath"),
          "Load brain state from checkpoint with error checking");

    // Create complex world configuration
    m.def("createComplexWorld", [](size_t width, size_t height, 
                                  size_t visionWidth, size_t visionHeight,
                                  float maxEnergy, float energyDecayRate) {
              auto world = std::make_shared<SimpleWorld>();
              world->configure(width, height, visionWidth, visionHeight);
              world->setMaxEnergy(maxEnergy);
              world->setEnergyDecayRate(energyDecayRate);
              return world;
          }, py::arg("width"), py::arg("height"), py::arg("visionWidth"), 
             py::arg("visionHeight"), py::arg("maxEnergy") = 100.0f, 
             py::arg("energyDecayRate") = 0.01f,
             "Create a configured world with custom parameters");

    // Batch action execution helper
    m.def("executeActionsSequence", 
          [](std::shared_ptr<SimpleWorld> world, 
             const std::vector<py::object>& actionTypes,
             float timestep) {
              if (!world) {
                  throw WorldError("Null world provided");
              }
              
              std::vector<py::object> results;
              for (const auto& actionType : actionTypes) {
                  try {
                      // Convert Python action type to MotorCommand
                      std::string actionStr = py::str(actionType);
                      
                      // Create action (simplified - in real implementation would map to ActionType)
                      auto action = std::make_unique<Action>();
                      
                      // Apply action
                      world->applyMotorCommand(action->getType(), timestep);
                      
                      results.push_back(py::bool_(true));
                  } catch (const std::exception& e) {
                      results.push_back(py::str(e.what()));
                  }
              }
              
              return results;
          }, py::arg("world"), py::arg("actionTypes"), py::arg("timestep"),
          "Execute a sequence of actions and return results");

    // Get comprehensive system metrics
    m.def("getSystemMetrics", [](const std::shared_ptr<Brain>& brain,
                                 const std::shared_ptr<SimpleWorld>& world,
                                 const std::shared_ptr<AgentBrain>& agent) {
              nlohmann::json metrics;
              
              // Brain metrics
              metrics["brain"] = nlohmann::json::object();
              auto& brainMetrics = metrics["brain"];
              brainMetrics["neurons"] = brain->getTotalNeuronCount();
              brainMetrics["synapses"] = brain->getTotalSynapseCount();
              brainMetrics["active_neurons"] = brain->getActiveNeuronCount();
              brainMetrics["firing_rate"] = brain->getAverageFiringRate();
              brainMetrics["total_spikes"] = brain->getTotalSpikeCount();
              
              // World metrics
              metrics["world"] = nlohmann::json::object();
              auto& worldMetrics = metrics["world"];
              worldMetrics["width"] = world->getWidth();
              worldMetrics["height"] = world->getHeight();
              worldMetrics["time"] = world->getSimulationTime();
              
              // Agent metrics
              metrics["agent"] = nlohmann::json::object();
              auto& agentMetrics = metrics["agent"];
              agentMetrics["neuromodulation_level"] = agent->getNeuromodulationLevel();
              agentMetrics["curiosity_level"] = agent->getCuriosityLevel();
              agentMetrics["novelty_level"] = agent->getNoveltyLevel();
              agentMetrics["prediction_error"] = agent->getPredictionError();
              agentMetrics["developmental_stage"] = static_cast<int>(agent->getDevelopmentalStage());
              
              return metrics.dump();
          }, py::arg("brain"), py::arg("world"), py::arg("agent"),
          "Get comprehensive system metrics as JSON string");

    // Safe configuration loading with validation
    m.def("loadAndValidateConfig", [](const std::string& filepath) {
        auto config = std::make_shared<Config>();
        
        if (!config->loadFromFile(filepath)) {
            throw ConfigurationError("Failed to load configuration from: " + filepath);
        }
        
        // Basic validation
        if (config->has("brain.region.count")) {
            int regionCount = config->getOr<int>("brain.region.count", 1);
            if (regionCount <= 0) {
                throw ConfigurationError("brain.region.count must be positive");
            }
        }
        
        return config;
    }, py::arg("filepath"),
       "Load and validate configuration with error checking");

    // Enhanced sensory input creation helper
    m.def("createVisionInput", [](size_t width, size_t height, 
                                 const std::vector<float>& data) {
              auto vision = std::make_unique<Vision>(width, height, 3);
              vision->setData(data);
              return vision;
          }, py::arg("width"), py::arg("height"), py::arg("data"),
          "Create vision sensory input with data");

    m.def("createAudioInput", [](size_t sampleRate, 
                                const std::vector<float>& samples) {
              auto audio = std::make_unique<Audio>(sampleRate, samples.size());
              audio->setData(samples);
              return audio;
          }, py::arg("sampleRate"), py::arg("samples"),
          "Create audio sensory input with samples");

    m.def("createInternalSignals", [](const std::vector<float>& signals) {
              auto internal = std::make_unique<InternalSignals>();
              for (float signal : signals) {
                  internal->addSignal(signal);
              }
              return internal;
          }, py::arg("signals"),
          "Create internal signals sensory input with signal data");

    // Configuration comparison helper
    m.def("compareConfigs", [](const std::shared_ptr<Config>& config1,
                               const std::shared_ptr<Config>& config2) {
              nlohmann::json differences = nlohmann::json::array();
              
              auto keys1 = config1->getKeys();
              auto keys2 = config2->getKeys();
              
              // Find keys in config1 not in config2
              for (const auto& key : keys1) {
                  if (!config2->has(key)) {
                      auto val1 = config1->get<ConfigValue>(key);
                      auto val2 = config2->get<ConfigValue>(key);
                      differences.push_back({
                          {"key", key},
                          {"config1_value", val1},
                          {"config2_value", val2},
                          {"status", "missing_in_config2"}
                      });
                  } else {
                      auto val1 = config1->get<ConfigValue>(key);
                      auto val2 = config2->get<ConfigValue>(key);
                      if (val1 != val2) {
                          differences.push_back({
                              {"key", key},
                              {"config1_value", val1},
                              {"config2_value", val2},
                              {"status", "different"}
                          });
                      }
                  }
              }
              
              // Find keys in config2 not in config1
              for (const auto& key : keys2) {
                  if (!config1->has(key)) {
                      auto val1 = config1->get<ConfigValue>(key);
                      auto val2 = config2->get<ConfigValue>(key);
                      differences.push_back({
                          {"key", key},
                          {"config1_value", val1},
                          {"config2_value", val2},
                          {"status", "missing_in_config1"}
                      });
                  }
              }
              
              return differences.dump();
          }, py::arg("config1"), py::arg("config2"),
          "Compare two configurations and return differences as JSON string");

    // Setup standard agent configuration
    m.def("setupStandardAgent", [](std::shared_ptr<Brain> brain,
                                  std::shared_ptr<SimpleWorld> world,
                                  bool enableLearning = true,
                                  bool enableDevelopment = true,
                                  bool enableCuriosity = true) {
              auto agent = std::make_shared<AgentBrain>(brain);
              agent->initialize(world);
              
              if (enableLearning) {
                  agent->enableRewardModulation(true);
              }
              
              if (enableDevelopment) {
                  agent->enableDevelopment(true);
              }
              
              if (enableCuriosity) {
                  agent->enableCuriosity(true);
              }
              
              // Enable structural plasticity by default for development
              agent->enableStructuralPlasticity(true);
              
              return agent;
          }, py::arg("brain"), py::arg("world"),
             py::arg("enableLearning") = true, py::arg("enableDevelopment") = true,
             py::arg("enableCuriosity") = true,
             "Setup a standard agent with common configuration");

    // Batch simulation helper for multiple episodes
    m.def("runMultipleEpisodes", 
          [](std::shared_ptr<Brain> brain,
             std::shared_ptr<SimpleWorld> world,
             std::shared_ptr<AgentBrain> agent,
             size_t numEpisodes,
             size_t stepsPerEpisode,
             bool resetBetweenEpisodes = true) {
              
              std::vector<nlohmann::json> episodeResults;
              
              for (size_t episode = 0; episode < numEpisodes; ++episode) {
                  nlohmann::json episodeData;
                  episodeData["episode"] = episode;
                  
                  // Reset if requested
                  if (resetBetweenEpisodes) {
                      brain->reset();
                      agent->reset();
                      world->reset();
                  }
                  
                  // Initialize for new episode
                  if (!brain->initialize()) {
                      episodeData["status"] = "failed_to_initialize";
                      episodeResults.push_back(episodeData);
                      continue;
                  }
                  
                  // Run episode
                  size_t spikes = 0;
                  float finalFiringRate = 0.0f;
                  
                  for (size_t step = 0; step < stepsPerEpisode; ++step) {
                      world->update(0.1f);
                      
                      auto percept = world->getSensoryPercept();
                      agent->processSensoryInput(percept);
                      
                      brain->step(step);
                      
                      auto action = agent->decodeMotorCommand();
                      world->applyMotorCommand(action, world->getSimulationTime());
                      
                      spikes += brain->getFiringNeuronCount();
                      if (step == stepsPerEpisode - 1) {
                          finalFiringRate = brain->getAverageFiringRate();
                      }
                  }
                  
                  episodeData["status"] = "completed";
                  episodeData["total_spikes"] = spikes;
                  episodeData["final_firing_rate"] = finalFiringRate;
                  episodeData["avg_spikes_per_step"] = static_cast<float>(spikes) / stepsPerEpisode;
                  
                  episodeResults.push_back(episodeData);
              }
              
              return nlohmann::json(episodeResults).dump();
          }, py::arg("brain"), py::arg("world"), py::arg("agent"),
             py::arg("numEpisodes"), py::arg("stepsPerEpisode"),
             py::arg("resetBetweenEpisodes") = true,
             "Run multiple simulation episodes and return results");

    // Create training configuration
    m.def("createTrainingConfig", [](size_t numSteps,
                                     bool enableRewardModulation = true,
                                     bool enableCuriosity = true,
                                     bool enableDevelopment = true,
                                     bool enableStructuralPlasticity = true) {
              auto config = std::make_shared<Config>();
              
              // Basic training setup
              config->set("training.steps", numSteps);
              config->set("training.learning.enabled", enableRewardModulation);
              config->set("training.exploration.enabled", enableCuriosity);
              config->set("training.development.enabled", enableDevelopment);
              config->set("training.plasticity.structural.enabled", enableStructuralPlasticity);
              
              // Learning parameters
              config->set("learning.rate", 0.01f);
              config->set("exploration.rate", 0.1f);
              config->set("development.plasticity.rate", 1.0f);
              
              // Memory settings
              config->set("memory.working.capacity", 100);
              config->set("memory.episodic.capacity", 50);
              
              // Neuromodulation
              config->set("neuromodulation.dopamine.baseLevel", 0.2f);
              config->set("neuromodulation.curiosity.sensitivity", 0.3f);
              
              return config;
          }, py::arg("numSteps"), py::arg("enableRewardModulation") = true,
             py::arg("enableCuriosity") = true, py::arg("enableDevelopment") = true,
             py::arg("enableStructuralPlasticity") = true,
             "Create a training configuration with specified parameters");

    // Safe configuration validation with detailed error reporting
    m.def("validateConfiguration", [](const std::shared_ptr<Config>& config) {
              nlohmann::json validationResult;
              validationResult["valid"] = true;
              validationResult["errors"] = nlohmann::json::array();
              validationResult["warnings"] = nlohmann::json::array();
              
              auto keys = config->getKeys();
              
              // Validate required sections
              std::vector<std::string> requiredSections = {
                  "brain.region.count",
                  "brain.neuromodulation.dopamine.baseLevel",
                  "brain.memory.working.capacity"
              };
              
              for (const auto& key : requiredSections) {
                  if (!config->has(key)) {
                      validationResult["errors"].push_back("Missing required configuration: " + key);
                      validationResult["valid"] = false;
                  }
              }
              
              // Validate parameter ranges
              if (config->has("brain.region.count")) {
                  int count = config->getOr<int>("brain.region.count", 0);
                  if (count <= 0) {
                      validationResult["errors"].push_back("brain.region.count must be positive");
                      validationResult["valid"] = false;
                  } else if (count > 100) {
                      validationResult["warnings"].push_back("brain.region.count is very high (>100), may impact performance");
                  }
              }
              
              if (config->has("brain.neuromodulation.dopamine.baseLevel")) {
                  float level = config->getOr<float>("brain.neuromodulation.dopamine.baseLevel", 0.0f);
                  if (level < 0.0f || level > 1.0f) {
                      validationResult["errors"].push_back("brain.neuromodulation.dopamine.baseLevel must be between 0.0 and 1.0");
                      validationResult["valid"] = false;
                  }
              }
              
              // Validate consistency between parameters
              if (config->has("brain.memory.working.capacity") && 
                  config->has("brain.neuromodulation.dopamine.baseLevel")) {
                  
                  int memCapacity = config->getOr<int>("brain.memory.working.capacity", 0);
                  float dopamineLevel = config->getOr<float>("brain.neuromodulation.dopamine.baseLevel", 0.0f);
                  
                  // Warning if low dopamine with high memory capacity
                  if (memCapacity > 50 && dopamineLevel < 0.1f) {
                      validationResult["warnings"].push_back(
                          "High memory capacity with low dopamine level may reduce learning efficiency");
                  }
              }
              
              return validationResult.dump();
          }, py::arg("config"),
          "Validate configuration with detailed error and warning reporting");

    // Create development stage configuration
    m.def("createDevelopmentConfig", [](const std::string& stage) {
              auto config = std::make_shared<Config>();
              
              if (stage == "initial") {
                  config->set("development.stage", "Initial");
                  config->set("development.plasticity.rate", 2.0f);
                  config->set("development.synaptogenesis.rate", 0.5f);
                  config->set("development.pruning.enabled", false);
                  config->set("development.maturation.enabled", false);
              } else if (stage == "critical") {
                  config->set("development.stage", "CriticalPeriod");
                  config->set("development.plasticity.rate", 1.5f);
                  config->set("development.synaptogenesis.rate", 1.0f);
                  config->set("development.pruning.enabled", true);
                  config->set("development.maturation.enabled", false);
              } else if (stage == "maturation") {
                  config->set("development.stage", "Maturation");
                  config->set("development.plasticity.rate", 1.0f);
                  config->set("development.synaptogenesis.rate", 0.5f);
                  config->set("development.pruning.enabled", true);
                  config->set("development.maturation.enabled", true);
              } else if (stage == "adult") {
                  config->set("development.stage", "Adult");
                  config->set("development.plasticity.rate", 0.5f);
                  config->set("development.synaptogenesis.rate", 0.1f);
                  config->set("development.pruning.enabled", true);
                  config->set("development.maturation.enabled", true);
              } else {
                  throw DevelopmentError("Invalid development stage: " + stage);
              }
              
              return config;
          }, py::arg("stage"),
          "Create development stage configuration");

    // Create development stage configuration
    m.def("createDevelopmentConfig", [](const std::string& stage) {
              auto config = std::make_shared<Config>();
              
              if (stage == "initial") {
                  config->set("development.stage", "Initial");
                  config->set("development.plasticity.rate", 2.0f);
                  config->set("development.synaptogenesis.rate", 0.5f);
                  config->set("development.pruning.enabled", false);
                  config->set("development.maturation.enabled", false);
              } else if (stage == "critical") {
                  config->set("development.stage", "CriticalPeriod");
                  config->set("development.plasticity.rate", 1.5f);
                  config->set("development.synaptogenesis.rate", 1.0f);
                  config->set("development.pruning.enabled", true);
                  config->set("development.maturation.enabled", false);
              } else if (stage == "maturation") {
                  config->set("development.stage", "Maturation");
                  config->set("development.plasticity.rate", 1.0f);
                  config->set("development.synaptogenesis.rate", 0.5f);
                  config->set("development.pruning.enabled", true);
                  config->set("development.maturation.enabled", true);
              } else if (stage == "adult") {
                  config->set("development.stage", "Adult");
                  config->set("development.plasticity.rate", 0.5f);
                  config->set("development.synaptogenesis.rate", 0.1f);
                  config->set("development.pruning.enabled", true);
                  config->set("development.maturation.enabled", true);
              } else {
                  throw DevelopmentError("Invalid development stage: " + stage);
              }
              
              return config;
          }, py::arg("stage"),
          "Create development stage configuration");

    // Create simple training environment helper
    m.def("createTrainingEnvironment", 
          [](size_t width, size_t height,
             size_t visionWidth, size_t visionHeight,
             float learningRate = 0.01f,
             float explorationRate = 0.1f) {
              
              auto world = std::make_shared<SimpleWorld>();
              world->configure(width, height, visionWidth, visionHeight);
              
              auto config = std::make_shared<Config>();
              
              // Set training parameters
              config->set("training.learning.rate", learningRate);
              config->set("training.exploration.rate", explorationRate);
              config->set("brain.neuromodulation.dopamine.baseLevel", 0.2f);
              config->set("brain.neuromodulation.curiosity.sensitivity", explorationRate);
              config->set("brain.prediction.accuracy", 0.9f);
              
              auto brain = std::make_shared<Brain>(config);
              
              if (!brain->initialize()) {
                  throw BrainError("Failed to initialize training brain");
              }
              
              auto agent = std::make_shared<AgentBrain>(brain);
              agent->initialize(world);
              
              return std::make_tuple(world, brain, agent);
          }, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             py::arg("learningRate") = 0.01f,
             py::arg("explorationRate") = 0.1f,
             "Create a complete training environment with brain, world, and agent");

    // Export convenience functions
    m.def("getConvenienceFunctions", []() {
        return nlohmann::json({
            {"name", "createStandardConfig"},
            {"description", "Create standard configuration for general NLM use"},
            {"name", "getBrainStatistics"},
            {"description", "Get comprehensive brain statistics as JSON"},
            {"name", "validateConfig"},
            {"description", "Validate configuration and return error messages"},
            {"name", "safeBrainInitialize"},
            {"description", "Safely initialize brain with validation"},
            {"name", "createTrainingConfig"},
            {"description", "Create training configuration with specified parameters"},
            {"name", "createDevelopmentConfig"},
            {"description", "Create development stage configuration"},
            {"name", "createTrainingEnvironment"},
            {"description", "Create a complete training environment"}
        }).dump();
    }, "Get list of available convenience functions with descriptions");

    // Version information
    m.attr("VERSION") = "1.0.0";
    m.attr("API_VERSION") = "1.0";
    m.attr("BUILD_DATE") = __DATE__;
}

} // namespace nlm
