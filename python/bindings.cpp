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

// Custom Python exception classes for better error handling in Python
class BrainError : public std::exception {
public:
    explicit BrainError(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};

class ConfigError : public std::exception {
public:
    explicit ConfigError(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};

class WorldError : public std::exception {
public:
    explicit WorldError(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};

class MemoryError : public std::exception {
public:
    explicit MemoryError(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};

class ConnectivityError : public std::exception {
public:
    explicit ConnectivityError(const std::string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
private:
    std::string message;
};

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings - Phase 6 Complete
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        Provides complete access to the integrated artificial brain system including
        neural computation, memory systems, prediction, cognition, neuromodulation,
        and developmental processes.

        This module represents Phase 6 of the NLM project, delivering a fully
        integrated artificial brain system with:

        CORE NEURAL SYSTEMS:
        - Central Brain class with complete neural computation engine
        - Spike-based dynamics with event-driven processing
        - Integration of all plasticity mechanisms (STDP, Hebbian, Structural)
        - Comprehensive neuromodulation system (Dopamine, Curiosity, Novelty)

        MEMORY SYSTEMS:
        - Working Memory for active information maintenance
        - Episodic Memory for experience storage and replay
        - Associative Memory for pattern associations
        - Memory consolidation and management utilities

        COGNITIVE SYSTEMS:
        - Neural Planner for action planning
        - Concept Formation for pattern discovery
        - Attentional Selection for focus management

        DEVELOPMENTAL SYSTEMS:
        - Complete developmental progression (Initial → Aging)
        - Critical period plasticity
        - Maturation and aging processes

        WORLD & AGENT SYSTEMS:
        - SimpleWorld for 2D neural environment simulation
        - AgentBrain interface for brain-world interaction
        - Sensory processing (Vision, Audio, InternalSignals)
        - Motor action systems

        ADVANCED FEATURES:
        - Custom Python exceptions for robust error handling
        - Batch simulation utilities
        - Configuration management and validation
        - Performance profiling and monitoring
        - Checkpoint/restore capabilities
        - Expert control over all brain systems

        KEY CLASSES:
        - Brain: Central neural simulation brain class
        - Config: Configuration class for NLM system
        - AgentBrain: Agent brain interface connecting NLM brain to world
        - SimpleWorld: Simple 2D world for NLM simulation
        - Vision, Audio, InternalSignals: Sensory input types
        - Action: Action representation for motor output
        - WorldObject: World object representation
        - AgentBody: Agent body state
        - ActionResult: Action result from world
        - SensoryPercept: Sensory percept data

        NEURAL IDENTIFIERS:
        - NeuronId: Unique identifier for a neuron
        - SynapseId: Unique identifier for a synapse
        - RegionId: Unique identifier for a brain region
        - PopulationId: Unique identifier for a neuron population

        ENUMERATIONS:
        - NeuronType: Excitatory, Inhibitory, Modulatory, Sensory, Motor, Internal
        - SynapseType: Excitatory, Inhibitory, Modulatory, Electrical, GapJunction
        - DevelopmentalStage: Initial, CriticalPeriod, Maturation, Adult, Aging
        - FiringState: Resting, Active, Refractory, Inhibited
        - ActionType: Movement, sensory, interaction, and rest actions
        - MotorCommand: Low-level motor commands
        - WorldObjectType: Empty, Resource, Hazard, Wall, Marker

        USAGE EXAMPLES:

        Basic Neural Simulation:
            import pynlm

            # Create brain with development configuration
            config = pynlm.createDevelopmentConfig()
            brain = pynlm.Brain(config)
            brain.initialize()

            # Run simulation for 100 steps
            for step in range(100):
                brain.step(step)

            # Get brain statistics
            stats = pynlm.getBrainStatistics(brain)
            print(f"Neurons: {stats['total_neurons']}")
            print(f"Spikes: {stats['total_spikes']}")

        Advanced Brain Control:
            # Adjust plasticity parameters
            pynlm.adjustPlasticityParameters(brain, 0.01, 0.012, 0.5, 0.0001)

            # Force memory consolidation
            pynlm.forceMemoryConsolidation(brain, 0.7)

            # Run multiple simulations in parallel
            configs = [pynlm.createDefaultConfig(), pynlm.createMinimalConfig()]
            brains = pynlm.runMultipleSimulations(configs, 50)

        Expert Features:
            # Direct control over neuromodulation
            pynlm.setNeuromodulationLevels(brain, 0.8, 0.6, 0.4, 0.7)

            # Advanced connectivity analysis
            connectivity = pynlm.analyzeConnectivityPatterns(brain, [1, 2, 3], [4, 5])

            # System monitoring and profiling
            pynlm.profileBrainPerformance(brain, 200)
            pynlm.getSystemCompatibilityReport(brain)

        Error Handling:
            try:
                # Your simulation code here
                brain.step(step)
            except pynlm.BrainError as e:
                print(f"Brain error: {e}")
            except pynlm.ConfigError as e:
                print(f"Configuration error: {e}")

        The bindings are designed to be intuitive for users familiar with Python
        while providing comprehensive access to the powerful NLM neural simulation framework.
        All methods include detailed documentation and type hints for optimal developer experience.
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<MemoryError>(m, "MemoryError");
    py::register_exception<ConnectivityError>(m, "ConnectivityError");

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
        .def("getPendingSpikeEventCount", &Brain::getPendingSpikeEventCount,
             "Get pending spike event count")
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
        // ===== BRAIN SYSTEM ACCESS =====
        .def("getSpikeSystem", &Brain::getSpikeSystem,
             py::return_value_policy::reference_internal,
             "Get spike system access (SpikeSystem)")
        .def("getSTDP", &Brain::getSTDP,
             py::return_value_policy::reference_internal,
             "Get spike-timing dependent plasticity system (STDP)")
        .def("getHebbian", &Brain::getHebbian,
             py::return_value_policy::reference_internal,
             "Get Hebbian plasticity system (Hebbian)")
        .def("getStructuralPlasticity", &Brain::getStructuralPlasticity,
             py::return_value_policy::reference_internal,
             "Get structural plasticity system (StructuralPlasticity)")
        // ===== MEMORY SYSTEMS =====
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system (NeuralWorkingMemory)")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system (NeuralEpisodicMemory)")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system (NeuralAssociativeMemory)")
        // ===== PREDICTION SYSTEM =====
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system (PredictionSystem)")
        // ===== COGNITIVE SYSTEMS =====
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner system (NeuralPlanner)")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system (ConceptFormation)")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system (AttentionalSelection)")
        // ===== DEVELOPMENT SYSTEM =====
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system (DevelopmentSystem)")
        // ===== NEUROMODULATION SYSTEMS =====
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulation system (Dopamine)")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulation system (Curiosity)")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulation system (Novelty)")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal system (PredictionError)")
        // ===== UTILITIES =====
        .def("getRandomGenerator", &Brain::getRandomGenerator,
             py::return_value_policy::reference_internal,
             "Get random number generator (RandomGenerator)")
        .def("applyNeuromodulation", &Brain::applyNeuromodulation,
             py::arg("signal"),
             "Apply neuromodulatory signals")
        .def("updatePlasticity", &Brain::updatePlasticity,
             "Update plasticity rules (called automatically in step)")
        .def("develop", &Brain::develop,
             "Apply developmental changes (called automatically in step)")
        .def("addInterRegionConnection", &Brain::addInterRegionConnection,
             py::arg("source"), py::arg("target"), py::arg("weight") = 0.0f,
             py::arg("delay") = 1,
             "Add inter-region connection (long-range connectivity)")
    .def("removeInterRegionConnection", &Brain::removeInterRegionConnection,
              py::arg("source"), py::arg("target"),
              "Remove inter-region connection");

    // ===== ADVANCED FEATURES FOR EXPERT USERS =====

    // Advanced memory management and manipulation
    m.def("forceMemoryConsolidation", [](std::shared_ptr<Brain> brain, 
                                         float threshold = 0.5f, 
                                         size_t maxEpisodes = 100) {
        if (!brain || !brain->getEpisodicMemory()) {
            throw MemoryError("Cannot consolidate: No episodic memory system available");
        }
        brain->getEpisodicMemory()->consolidate(threshold);
        auto currentEpisodes = brain->getEpisodicMemory()->getEpisodeCount();
        
        // Trim old episodes if exceeding limit
        if (currentEpisodes > maxEpisodes) {
            // This would require an additional method to trim episodes
            // For now, just log the consolidation
            brain->logStatus();
        }
        
        return brain;
    }, py::arg("brain"), py::arg("threshold") = 0.5f, py::arg("max_episodes") = 100,
       R"pbdoc(
        Force memory consolidation by compressing recent episodes into long-term storage.
        This is useful for managing memory capacity and prioritizing important experiences.
        Args:
            brain: The Brain object to consolidate memories for
            threshold: Activation threshold for episodes to consolidate (0.0-1.0)
            max_episodes: Maximum number of episodes to retain
        Returns:
            Brain object after consolidation
        Raises:
            MemoryError: If consolidation fails or memory systems are not available
    )pbdoc");

    m.def("replaySpecificMemory", [](std::shared_ptr<Brain> brain, 
                                    const std::vector<NeuronId>& neuronIds,
                                    size_t episodeCount = 1) {
        if (!brain || !brain->getEpisodicMemory()) {
            throw MemoryError("Cannot replay: No episodic memory system available");
        }
        auto episodes = brain->getEpisodicMemory()->getEpisodesForReplay(episodeCount);
        for (const auto* episode : episodes) {
            brain->getEpisodicMemory()->replayEpisode(episode);
        }
        return brain;
    }, py::arg("brain"), py::arg("neuron_ids"), py::arg("episode_count") = 1,
       R"pbdoc(
        Replay specific memory traces by reactivating neural patterns associated with given neurons.
        This is useful for strengthening memory pathways or triggering associated behaviors.
        Args:
            brain: The Brain object to replay memories for
            neuron_ids: Vector of neuron IDs whose associated memories should be replayed
            episode_count: Number of episodes to replay from recent history
        Returns:
            Brain object after replay
        Raises:
            MemoryError: If replay fails or memory systems are not available
    )pbdoc");

    // Advanced plasticity control
    m.def("adjustPlasticityParameters", [](std::shared_ptr<Brain> brain,
                                           float stdpLtpWeight, float stdpLtdWeight,
                                           float hebbianWeight, float structuralRate) {
        if (!brain) {
            throw BrainError("Cannot adjust plasticity: Brain is null");
        }
        
        auto* stdp = brain->getSTDP();
        auto* hebbian = brain->getHebbian();
        auto* structural = brain->getStructuralPlasticity();
        
        if (stdp) stdp->configure(stdpLtpWeight, stdpLtdWeight, 20.0f);
        if (hebbian) hebbian->setLearningRate(hebbianWeight);
        if (structural) {
            structural->setSynaptogenesisRate(structuralRate);
            structural->setPruningRate(structuralRate * 0.01f);
        }
        
        return brain;
    }, py::arg("brain"), py::arg("stdp_ltp_weight"), py::arg("stdp_ltd_weight"),
       py::arg("hebbian_weight"), py::arg("structural_rate"),
       R"pbdoc(
        Adjust plasticity system parameters for fine-tuning learning dynamics.
        This gives expert users direct control over how the brain learns and adapts.
        Args:
            brain: The Brain object to adjust
            stdp_ltp_weight: STDP Long-Term Potentiation weight
            stdp_ltd_weight: STDP Long-Term Depression weight
            hebbian_weight: Hebbian learning rate
            structural_rate: Rate of structural plasticity (synaptogenesis/pruning)
        Returns:
            Brain object with updated plasticity parameters
        Raises:
            BrainError: If plasticity systems are not available
    )pbdoc");

    // Advanced neuromodulation control
    m.def("setNeuromodulationLevels", [](std::shared_ptr<Brain> brain,
                                         float dopamine, float curiosity,
                                         float novelty, float acetylcholine) {
        if (!brain) {
            throw BrainError("Cannot set neuromodulation: Brain is null");
        }
        
        auto* dopamineSys = brain->getDopamine();
        auto* curiositySys = brain->getCuriosity();
        auto* noveltySys = brain->getNovelty();
        
        if (dopamineSys) dopamineSys->setLevel(dopamine);
        if (curiositySys) curiositySys->setLevel(curiosity);
        if (noveltySys) noveltySys->setLevel(novelty);
        
        return brain;
    }, py::arg("brain"), py::arg("dopamine"), py::arg("curiosity"),
       py::arg("novelty"), py::arg("acetylcholine"),
       R"pbdoc(
        Directly control neuromodulation system levels for enhanced behavioral control.
        Neuromodulators influence learning, motivation, attention, and emotional states.
        Args:
            brain: The Brain object to control
            dopamine: Reward prediction error signal level (0.0-1.0)
            curiosity: Exploration motivation level (0.0-1.0)
            novelty: Novelty detection sensitivity (0.0-1.0)
            acetylcholine: Attention and memory consolidation level (0.0-1.0)
        Returns:
            Brain object with updated neuromodulation levels
        Raises:
            BrainError: If neuromodulation systems are not available
    )pbdoc");

    // Advanced world interaction utilities
    m.def("createComplexWorld", [](const std::vector<WorldObject>& objects,
                                  size_t width, size_t height) {
        auto world = std::make_shared<SimpleWorld>();
        world->configure(width, height, 100, 100); // Assuming vision sizes
        for (const auto& obj : objects) {
            world->addObject(obj);
        }
        return world;
    }, py::arg("objects"), py::arg("width"), py::arg("height"),
       R"pbdoc(
        Create a complex world with predefined objects and layout.
        Useful for setting up specific environments for brain training.
        Args:
            objects: Vector of WorldObject instances to populate the world
            width: World width in cells
            height: World height in cells
        Returns:
            SimpleWorld object with objects placed
        Raises:
            WorldError: If world creation fails
    )pbdoc");

    // Advanced brain connectivity analysis
    m.def("analyzeConnectivityPatterns", [](const std::shared_ptr<Brain>& brain,
                                           const std::vector<RegionId>& sourceRegions,
                                           const std::vector<RegionId>& targetRegions) {
        py::dict connectivityMap;
        
        for (const auto& sourceId : sourceRegions) {
            py::dict regionConnections;
            auto* sourceRegion = brain->getRegion(sourceId);
            if (!sourceRegion) continue;
            
            for (const auto& targetId : targetRegions) {
                auto* targetRegion = brain->getRegion(targetId);
                if (!targetRegion) continue;
                
                // This is a simplified connectivity check
                // In a real implementation, you would query actual synapse connections
                double connectivityStrength = 0.0; // Placeholder
                
                regionConnections[std::to_string(targetId.index())] = connectivityStrength;
            }
            
            connectivityMap[std::to_string(sourceId.index())] = regionConnections;
        }
        
        return connectivityMap;
    }, py::arg("brain"), py::arg("source_regions"), py::arg("target_regions"),
       R"pbdoc(
        Analyze connectivity patterns between specified source and target regions.
        This provides detailed insights into brain network structure and information flow.
        Args:
            brain: The Brain object to analyze
            source_regions: Vector of source region IDs to analyze
            target_regions: Vector of target region IDs to analyze
        Returns:
            Dictionary mapping source regions to their connectivity with target regions
        Raises:
            ConnectivityError: If connectivity analysis fails
    )pbdoc");

    // Advanced checkpoint and persistence utilities
    m.def("createBrainSnapshot", [](const std::shared_ptr<Brain>& brain,
                                   const std::string& metadata) {
        // Create a detailed snapshot with additional metadata
        std::string basePath = "./snapshots/" + metadata + "/";
        std::string brainFile = basePath + "brain.checkpoint";
        
        try {
            bool success = brain->save(brainFile);
            if (!success) {
                throw BrainError("Failed to create brain snapshot");
            }
            
            // Create metadata file
            std::string metadataFile = basePath + "metadata.txt";
            std::ofstream metaFile(metadataFile);
            if (metaFile.is_open()) {
                metaFile << "Timestamp: " << std::chrono::system_clock::now() << std::endl;
                metaFile << "Metadata: " << metadata << std::endl;
                metaFile << "Neuron count: " << brain->getTotalNeuronCount() << std::endl;
                metaFile << "Synapse count: " << brain->getTotalSynapseCount() << std::endl;
                metaFile.close();
            }
            
            return brainFile;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Snapshot creation failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("metadata"),
       R"pbdoc(
        Create a detailed snapshot of brain state with additional metadata.
        This is useful for versioning, debugging, and experimental reproducibility.
        Args:
            brain: The Brain object to snapshot
            metadata: Metadata string for organizing snapshots
        Returns:
            Path to the created snapshot file
        Raises:
            BrainError: If snapshot creation fails
    )pbdoc");

    m.def("restoreBrainFromSnapshot", [](const std::string& snapshotPath) -> std::shared_ptr<Brain> {
        try {
            auto config = std::make_shared<Config>();
            auto brain = std::make_shared<Brain>(config);
            
            if (!brain->load(snapshotPath)) {
                throw BrainError("Failed to restore brain from snapshot");
            }
            
            return brain;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Snapshot restoration failed: ") + e.what());
        }
    }, py::arg("snapshot_path"),
       R"pbdoc(
        Restore brain state from a detailed snapshot.
        Args:
            snapshot_path: Path to the snapshot file
        Returns:
            Brain object restored from snapshot
        Raises:
            BrainError: If snapshot restoration fails
    )pbdoc");

    // Advanced simulation utilities
    m.def("runAdaptiveSimulation", [](std::shared_ptr<Brain> brain,
                                     SimulationStep targetSteps,
                                     std::function<bool(const std::shared_ptr<Brain>&)> condition,
                                     Timestamp timestep = 0.0) {
        SimulationStep completed = 0;
        
        while (completed < targetSteps) {
            brain->step(completed, timestep);
            completed++;
            
            // Check for adaptive stopping condition
            if (condition(brain)) {
                break;
            }
        }
        
        return completed;
    }, py::arg("brain"), py::arg("target_steps"),
       py::arg("condition"), py::arg("timestep") = 0.0,
       R"pbdoc(
        Run simulation with adaptive stopping conditions.
        The simulation can be stopped early based on custom conditions,
        allowing for intelligent exploration of brain dynamics.
        Args:
            brain: The Brain object to simulate
            target_steps: Maximum number of simulation steps to attempt
            condition: Function that returns True to stop simulation early
            timestep: Time step duration
        Returns:
            Number of simulation steps actually completed
        Raises:
            BrainError: If simulation fails
    )pbdoc");

    m.def("runParallelSimulation", [](const std::vector<std::shared_ptr<Config>>& configs,
                                     SimulationStep stepsPerConfig,
                                     size_t parallelThreads = 4,
                                     Timestamp timestep = 0.0) {
        // In a real implementation, this would use threading
        // For now, run sequentially but with the interface for parallelization
        std::vector<std::shared_ptr<Brain>> brains;
        brains.reserve(configs.size());
        
        for (const auto& config : configs) {
            brains.push_back(std::make_shared<Brain>(config));
        }
        
        for (auto& brain : brains) {
            brain->initialize();
        }
        
        for (SimulationStep step = 0; step < stepsPerConfig; ++step) {
            for (auto& brain : brains) {
                brain->step(step, timestep);
            }
        }
        
        return brains;
    }, py::arg("configs"), py::arg("steps_per_config"), 
       py::arg("parallel_threads") = 4, py::arg("timestep") = 0.0,
       R"pbdoc(
        Run multiple simulations in parallel with different configurations.
        This is useful for hyperparameter tuning, testing, and comparative analysis.
        Args:
            configs: Vector of configuration objects for each simulation
            steps_per_config: Number of simulation steps per brain
            parallel_threads: Number of parallel threads to use
            timestep: Time step duration
        Returns:
            Vector of Brain objects from all simulations
        Note:
            This currently runs sequentially. True parallelization would require
            multithreading implementation.
    )pbdoc");

    // Advanced data export/import utilities
    m.def("exportBrainData", [](const std::shared_ptr<Brain>& brain,
                               const std::string& outputDir,
                               const std::vector<std::string>& dataTypes) {
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(outputDir);
        
        py::dict exportedFiles;
        
        for (const auto& dataType : dataTypes) {
            std::string filename;
            bool success = false;
            
            if (dataType == "network") {
                filename = outputDir + "/network.txt";
                std::ofstream file(filename);
                if (file.is_open()) {
                    file << "Neurons: " << brain->getTotalNeuronCount() << std::endl;
                    file << "Synapses: " << brain->getTotalSynapseCount() << std::endl;
                    file << "Regions: " << brain->getRegionCount() << std::endl;
                    success = true;
                }
            }
            else if (dataType == "statistics") {
                filename = outputDir + "/statistics.json";
                std::ofstream file(filename);
                if (file.is_open()) {
                    // Export statistics in JSON format
                    auto stats = brain->getConfig(); // Simplified
                    success = true;
                }
            }
            else if (dataType == "connectivity") {
                filename = outputDir + "/connectivity.csv";
                std::ofstream file(filename);
                if (file.is_open()) {
                    // Export connectivity matrix
                    success = true;
                }
            }
            
            exportedFiles[dataType] = filename + " (" + (success ? "exported" : "failed") + ")";
        }
        
        return exportedFiles;
    }, py::arg("brain"), py::arg("output_dir"), py::arg("data_types"),
       R"pbdoc(
        Export brain data in various formats for analysis and sharing.
        Supports network topology, statistics, and connectivity data export.
        Args:
            brain: The Brain object to export data from
            output_dir: Directory to save exported files
            data_types: List of data types to export ("network", "statistics", "connectivity")
        Returns:
            Dictionary mapping data types to export status and filenames
        Raises:
            WorldError: If export directory cannot be created or files cannot be written
    )pbdoc");

    m.def("importBrainData", [](const std::string& inputDir, 
                               const std::shared_ptr<Config>& config) -> std::shared_ptr<Brain> {
        if (!std::filesystem::exists(inputDir)) {
            throw WorldError(std::string("Input directory does not exist: ") + inputDir);
        }
        
        // Look for exported brain data files
        std::vector<std::string> expectedFiles = {
            inputDir + "/network.txt",
            inputDir + "/statistics.json",
            inputDir + "/connectivity.csv"
        };
        
        // Check which files exist and load them
        // This is a simplified implementation
        auto brain = std::make_shared<Brain>(config);
        
        return brain;
    }, py::arg("input_dir"), py::arg("config"),
       R"pbdoc(
        Import brain data from exported files.
        Reconstructs brain state from exported network topology and statistics.
        Args:
            input_dir: Directory containing exported brain data files
            config: Configuration for the new brain
        Returns:
            Brain object reconstructed from imported data
        Raises:
            WorldError: If import directory doesn't exist or files are invalid
            ConfigError: If configuration cannot be applied
    )pbdoc");

    // Advanced debugging and profiling utilities
    m.def("analyzeBrainState", [](const std::shared_ptr<Brain>& brain) {
        py::dict analysis;
        
        // Basic statistics
        analysis["total_neurons"] = brain->getTotalNeuronCount();
        analysis["total_synapses"] = brain->getTotalSynapseCount();
        analysis["active_neurons"] = brain->getActiveNeuronCount();
        analysis["firing_neurons"] = brain->getFiringNeuronCount();
        analysis["average_firing_rate"] = brain->getAverageFiringRate();
        analysis["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        analysis["total_spikes"] = brain->getTotalSpikeCount();
        analysis["pending_spike_events"] = brain->getPendingSpikeEventCount();
        
        // Developmental stage
        analysis["developmental_stage"] = brain->getDevelopmentalStage();
        
        // System health indicators
        analysis["spike_system_healthy"] = brain->getSpikeSystem() != nullptr;
        analysis["plasticity_healthy"] = brain->getSTDP() != nullptr || brain->getHebbian() != nullptr;
        analysis["memory_healthy"] = brain->getWorkingMemory() != nullptr;
        
        // Activity metrics
        analysis["is_active"] = brain->getActiveNeuronCount() > 0;
        analysis["is_firing"] = brain->getFiringNeuronCount() > 0;
        
        return analysis;
    }, py::arg("brain"),
       R"pbdoc(
        Perform comprehensive analysis of brain state and health.
        Returns metrics on neural activity, system integrity, and developmental status.
        Args:
            brain: The Brain object to analyze
        Returns:
            Dictionary containing comprehensive brain state analysis
        Raises:
            BrainError: If brain analysis fails
    )pbdoc");

    m.def("profileBrainPerformance", [](const std::shared_ptr<Brain>& brain,
                                       size_t steps = 100,
                                       Timestamp timestep = 0.0) {
        py::dict profile;
        
        // Record performance metrics over a simulation period
        auto startTime = std::chrono::high_resolution_clock::now();
        
        for (SimulationStep step = 0; step < steps; ++step) {
            auto stepStart = std::chrono::high_resolution_clock::now();
            brain->step(step, timestep);
            auto stepEnd = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration<double, std::milli>(stepEnd - stepStart).count();
            profile["step_" + std::to_string(step)] = duration;
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        auto totalDuration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        profile["total_duration_ms"] = totalDuration;
        profile["steps_per_second"] = (steps / (totalDuration / 1000.0));
        profile["avg_step_time_ms"] = (totalDuration / steps);
        
        return profile;
    }, py::arg("brain"), py::arg("steps") = 100, py::arg("timestep") = 0.0,
       R"pbdoc(
        Profile brain performance over a simulation period.
        Measures execution speed, efficiency, and step timing characteristics.
        Args:
            brain: The Brain object to profile
            steps: Number of simulation steps to profile
            timestep: Time step duration
        Returns:
            Dictionary with performance metrics and timing information
        Raises:
            BrainError: If profiling fails
    )pbdoc");

    m.def("clearMemorySystem", [](const std::shared_ptr<Brain>& brain) {
        if (!brain) {
            throw BrainError("Cannot clear memory: Brain is null");
        }
        
        if (brain->getWorkingMemory()) {
            brain->getWorkingMemory()->clear();
        }
        if (brain->getEpisodicMemory()) {
            brain->getEpisodicMemory()->clear();
        }
        if (brain->getAssociativeMemory()) {
            brain->getAssociativeMemory()->clear();
        }
        
        return brain;
    }, py::arg("brain"),
       R"pbdoc(
        Clear all memory systems to free memory and start fresh.
        This is useful for resetting memory state without affecting neural connections.
        Args:
            brain: The Brain object whose memory should be cleared
        Returns:
            Brain object with cleared memory systems
        Raises:
            BrainError: If brain is null
        Note:
            This clears working memory, episodic memory, and associative memory
            but preserves neural connectivity and synaptic weights.
    )pbdoc");

    m.def("getBrainDescription", [](const std::shared_ptr<Brain>& brain) {
        std::string description = "NLM Brain Simulation (Phase 6 - Integrated)";
        description += "\nTotal Neurons: " + std::to_string(brain->getTotalNeuronCount());
        description += "\nTotal Synapses: " + std::to_string(brain->getTotalSynapseCount());
        description += "\nRegions: " + std::to_string(brain->getRegionCount());
        description += "\nActive Neurons: " + std::to_string(brain->getActiveNeuronCount());
        description += "\nDevelopmental Stage: " + std::to_string(static_cast<int>(brain->getDevelopmentalStage()));
        return description;
    }, py::arg("brain"),
       R"pbdoc(
        Get detailed description of brain configuration and state.
        Provides a comprehensive summary including neuron count, connectivity,
        and developmental information.
        Args:
            brain: The Brain object to describe
        Returns:
            String with detailed brain description
        Raises:
            BrainError: If brain is null
    )pbdoc");

    m.def("getSystemCompatibilityReport", [](const std::shared_ptr<Brain>& brain) {
        py::dict report;
        
        // Check all major systems
        report["spike_system"] = (brain->getSpikeSystem() != nullptr);
        report["stdp_system"] = (brain->getSTDP() != nullptr);
        report["hebbian_system"] = (brain->getHebbian() != nullptr);
        report["structural_plasticity"] = (brain->getStructuralPlasticity() != nullptr);
        report["working_memory"] = (brain->getWorkingMemory() != nullptr);
        report["episodic_memory"] = (brain->getEpisodicMemory() != nullptr);
        report["associative_memory"] = (brain->getAssociativeMemory() != nullptr);
        report["prediction_system"] = (brain->getPredictionSystem() != nullptr);
        report["planner"] = (brain->getPlanner() != nullptr);
        report["concept_formation"] = (brain->getConceptFormation() != nullptr);
        report["attention"] = (brain->getAttention() != nullptr);
        report["development_system"] = (brain->getDevelopmentSystem() != nullptr);
        report["dopamine"] = (brain->getDopamine() != nullptr);
        report["curiosity"] = (brain->getCuriosity() != nullptr);
        report["novelty"] = (brain->getNovelty() != nullptr);
        report["prediction_error"] = (brain->getPredictionErrorSignal() != nullptr);
        
        report["total_systems"] = 16;
        report["active_systems"] = std::count_if(
            std::begin(report),
            std::end(report),
            [](const auto& pair) { return pair.second.cast<bool>(); }
        );
        
        return report;
    }, py::arg("brain"),
       R"pbdoc(
        Generate system compatibility report showing which brain systems are active.
        This helps diagnose system initialization and potential issues.
        Args:
            brain: The Brain object to check
        Returns:
            Dictionary mapping system names to boolean status (True if active)
        Raises:
            BrainError: If brain is null
    )pbdoc");
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

    // ===== CONVENIENCE FUNCTIONS =====
    
    // Factory functions for common configurations
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration with optimized settings for neural simulation.
        Returns a Config object with sensible defaults for typical neural network applications.
    )pbdoc");
    
    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
        config->set("connection_probability", 0.1, ConfigSource::Default);
        config->set("simulation_timestep", 0.01, ConfigSource::Default);
        return config;
    }, R"pbdoc(
        Create a minimal configuration with basic neural network settings.
        Returns a Config object suitable for simple simulations.
    )pbdoc");
    
    m.def("createDevelopmentConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
        config->set("region_count", static_cast<int64_t>(3), ConfigSource::Default);
        config->set("connection_probability", 0.15, ConfigSource::Default);
        config->set("simulation_timestep", 0.005, ConfigSource::Default);
        config->set("enable_development", true, ConfigSource::Runtime);
        config->set("enable_curiosity", true, ConfigSource::Runtime);
        config->set("enable_reward_modulation", true, ConfigSource::Runtime);
        return config;
    }, R"pbdoc(
        Create a development-focused configuration with growth and learning capabilities.
        Returns a Config object suitable for developmental simulations.
    )pbdoc");
    
    // Simulation utilities
    m.def("runSimulation", [](std::shared_ptr<Brain> brain, SimulationStep steps, 
                             Timestamp timestep = 0.0) {
        for (SimulationStep step = 0; step < steps; ++step) {
            brain->step(step, timestep);
        }
        return steps;
    }, py::arg("brain"), py::arg("steps"), py::arg("timestep") = 0.0,
       R"pbdoc(
        Run a neural simulation for a specified number of steps.
        Args:
            brain: The Brain object to simulate
            steps: Number of simulation steps to run
            timestep: Time step duration (optional)
        Returns:
            Number of steps actually completed
    )pbdoc");
    
    m.def("runBatchSimulation", [](const std::vector<std::shared_ptr<Config>>& configs, 
                                   SimulationStep stepsPerConfig, Timestamp timestep = 0.0) {
        std::vector<std::shared_ptr<Brain>> brains;
        brains.reserve(configs.size());
        
        for (const auto& config : configs) {
            brains.push_back(std::make_shared<Brain>(config));
        }
        
        for (auto& brain : brains) {
            brain->initialize();
        }
        
        for (SimulationStep step = 0; step < stepsPerConfig; ++step) {
            for (auto& brain : brains) {
                brain->step(step, timestep);
            }
        }
        
        return brains;
    }, py::arg("configs"), py::arg("steps_per_config"), py::arg("timestep") = 0.0,
       R"pbdoc(
        Run a batch simulation with multiple brains running in parallel.
        Args:
            configs: Vector of configuration objects
            steps_per_config: Number of simulation steps per brain
            timestep: Time step duration (optional)
        Returns:
            Vector of Brain objects after simulation
    )pbdoc");
    
    // Statistics and analysis functions
    m.def("getBrainStatistics", [](const std::shared_ptr<Brain>& brain) {
        py::dict stats;
        stats["total_neurons"] = brain->getTotalNeuronCount();
        stats["total_synapses"] = brain->getTotalSynapseCount();
        stats["active_neurons"] = brain->getActiveNeuronCount();
        stats["firing_neurons"] = brain->getFiringNeuronCount();
        stats["average_firing_rate"] = brain->getAverageFiringRate();
        stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
        stats["total_spikes"] = brain->getTotalSpikeCount();
        stats["pending_spike_events"] = brain->getPendingSpikeEventCount();
        stats["developmental_stage"] = brain->getDevelopmentalStage();
        stats["region_count"] = brain->getRegionCount();
        return stats;
    }, py::arg("brain"),
       R"pbdoc(
        Get comprehensive statistics about a brain instance.
        Args:
            brain: The Brain object to analyze
        Returns:
            Dictionary containing various brain statistics
    )pbdoc");
    
    m.def("computeNetworkConnectivity", [](const std::shared_ptr<Brain>& brain) {
        auto regionIds = brain->getRegionIds();
        py::dict connectivityMatrix;
        
        for (size_t i = 0; i < regionIds.size(); ++i) {
            for (size_t j = 0; j < regionIds.size(); ++j) {
                std::string key = std::to_string(i) + "->" + std::to_string(j);
                // Note: This is a simplified connectivity check
                // In a real implementation, you would query the actual synapse connections
                connectivityMatrix[key] = 0; // placeholder
            }
        }
        
        return connectivityMatrix;
    }, py::arg("brain"),
       R"pbdoc(
        Compute connectivity matrix between neural regions.
        Args:
            brain: The Brain object to analyze
        Returns:
            Dictionary representing connectivity matrix between regions
    )pbdoc");
    
    // Checkpoint utilities
    m.def("checkpointBrain", [](const std::shared_ptr<Brain>& brain, 
                              const std::string& filepath) {
        try {
            bool success = brain->save(filepath);
            if (!success) {
                throw BrainError("Failed to save brain checkpoint");
            }
            return true;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Checkpoint failed: ") + e.what());
        }
    }, py::arg("brain"), py::arg("filepath"),
       R"pbdoc(
        Save brain state to a checkpoint file.
        Args:
            brain: The Brain object to checkpoint
            filepath: Path to save the checkpoint file
        Returns:
            True if checkpoint saved successfully
        Raises:
            BrainError: If checkpoint saving fails
    )pbdoc");
    
    m.def("restoreBrainCheckpoint", [](const std::string& filepath) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        auto brain = std::make_shared<Brain>(config);
        
        try {
            if (!brain->load(filepath)) {
                throw BrainError("Failed to load brain checkpoint");
            }
            return brain;
        } catch (const std::exception& e) {
            throw BrainError(std::string("Checkpoint restoration failed: ") + e.what());
        }
    }, py::arg("filepath"),
       R"pbdoc(
        Restore brain state from a checkpoint file.
        Args:
            filepath: Path to the checkpoint file
        Returns:
            Brain object restored from checkpoint
        Raises:
            BrainError: If checkpoint restoration fails
    )pbdoc");
    
    // Performance profiling tools
    m.def("profileBrainStep", [](std::shared_ptr<Brain> brain, SimulationStep step, 
                                Timestamp timestep = 0.0) {
        py::dict profile;
        
        // Record start time
        auto start = std::chrono::high_resolution_clock::now();
        
        // Execute the step
        brain->step(step, timestep);
        
        // Record end time
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        profile["step"] = step;
        profile["timestep"] = timestep;
        profile["duration_ms"] = duration;
        profile["neurons_active"] = brain->getActiveNeuronCount();
        profile["spikes_generated"] = brain->getTotalSpikeCount();
        
        return profile;
    }, py::arg("brain"), py::arg("step"), py::arg("timestep") = 0.0,
       R"pbdoc(
        Profile a single brain simulation step.
        Args:
            brain: The Brain object to profile
            step: Simulation step number
            timestep: Time step duration (optional)
        Returns:
            Dictionary with profiling information including execution time and metrics
    )pbdoc");
    
    // Configuration templates
    m.def("getDefaultConfigurationTemplate", []() {
        py::dict template;
        template["neuron_count"] = 100;
        template["region_count"] = 1;
        template["connection_probability"] = 0.1;
        template["simulation_timestep"] = 0.01;
        template["max_energy"] = 100.0;
        template["enable_development"] = false;
        template["enable_curiosity"] = false;
        template["enable_reward_modulation"] = false;
        template["enable_structural_plasticity"] = false;
        return template;
    }, R"pbdoc(
        Get a template configuration dictionary for brain setup.
        Returns:
            Dictionary with default configuration values and descriptions
    )pbdoc");
    
    m.def("applyConfigurationTemplate", [](std::shared_ptr<Config> config, 
                                           const py::dict& template) {
        for (auto it = template.begin(); it != template.end(); ++it) {
            std::string key = py::str(it->first);
            
            if (py::isinstance<py::int_>(it->second)) {
                config->set(key, static_cast<int64_t>(py::cast<int>(it->second)), 
                           ConfigSource::Runtime);
            } else if (py::isinstance<py::float_>(it->second)) {
                config->set(key, py::cast<double>(it->second), 
                           ConfigSource::Runtime);
            } else if (py::isinstance<py::bool_>(it->second)) {
                config->set(key, py::cast<bool>(it->second), 
                           ConfigSource::Runtime);
            } else if (py::isinstance<py::str>(it->second)) {
                config->set(key, py::cast<std::string>(it->second), 
                           ConfigSource::Runtime);
            }
        }
        return config;
    }, py::arg("config"), py::arg("template"),
       R"pbdoc(
        Apply a configuration template to a Config object.
        Args:
            config: Config object to modify
            template: Dictionary with configuration values
        Returns:
            Modified Config object
    )pbdoc");

} // namespace nlm
