#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>

#include "../src/brain/Brain.hpp"
#include "../src/core/Config/Config.hpp"
#include "../src/core/Types/Types.hpp"
#include "../src/agent/AgentBrain.hpp"
#include "../src/world/SimpleWorld.hpp"
#include "../src/sensory/SensoryInput.hpp"
#include "../src/motor/Action.hpp"
#include "../src/agent/AgentBody.hpp"
#include "../src/agent/SensoryPercept.hpp"
#include "../src/memory/NeuralWorkingMemory.hpp"
#include "../src/memory/NeuralEpisodicMemory.hpp"
#include "../src/cognition/ConceptFormation.hpp"
#include "../src/neuromodulation/Neuromodulator.hpp"
#include "../src/neuromodulation/Curiosity.hpp"
#include "../src/neuromodulation/Novelty.hpp"
#include "../src/neuromodulation/PredictionError.hpp"
#include "../src/cognition/NeuralPlanner.hpp"
#include "../src/development/DevelopmentSystem.hpp"
#include "../src/environment/Environment.hpp"

namespace py = pybind11;
namespace nlm {

// Custom Python exceptions
class NLMError : public std::runtime_error {
public:
    explicit NLMError(const std::string& msg) : std::runtime_error(msg) {}
};

class ConfigurationError : public NLMError {
public:
    explicit ConfigurationError(const std::string& msg) : NLMError("Configuration error: " + msg) {}
};

class MemoryError : public NLMError {
public:
    explicit MemoryError(const std::string& msg) : NLMError("Memory system error: " + msg) {}
};

class PredictionError : public NLMError {
public:
    explicit PredictionError(const std::string& msg) : NLMError("Prediction system error: " + msg) {}
};

// Exception registration
template<typename... Args>
static py::exception<NLMError> register_nlm_exception(py::module_& m, const char* name) {
    return py::register_exception_translator([](std::exception_ptr p) {
        try {
            if (p) std::rethrow_exception(p);
        } catch (const NLMError& e) {
            PyErr_SetString(PyExc_RuntimeError, e.what());
        }
    });
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A comprehensive Python binding for the NLM C++ neural simulation framework.
        
        This enhanced API provides access to all neural systems including memory,
        prediction, cognition, neuromodulation, development, and advanced utilities.
        
        Key Features:
        - Full memory system support (working, episodic, associative)
        - Advanced cognition (planning, concept formation, attention)
        - Neuromodulation (dopamine, curiosity, novelty, prediction error)
        - Developmental systems with age-based plasticity
        - Configuration validation and schema support
        - Real-time monitoring and performance analytics
        - Checkpoint management and batch operations
        - Advanced debugging and visualization tools
        
        Example Usage:
        ```python
        import pynlm as nlm
        
        # Create brain with configuration
        config = nlm.Config()
        config.set("simulation_steps", 1000)
        brain = nlm.Brain(config)
        
        # Initialize all systems
        brain.initialize()
        
        # Access memory systems
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        # Run simulation
        for step in range(1000):
            brain.step(step)
        ```
    )pbdoc";

    // Register custom Python exceptions
    py::register_exception<NLMError>(m, "NLMError", PyExc_RuntimeError);
    py::register_exception<ConfigurationError>(m, "ConfigurationError", PyExc_ValueError);
    py::register_exception<MemoryError>(m, "MemoryError", PyExc_RuntimeError);
    py::register_exception<PredictionError>(m, "PredictionError", PyExc_RuntimeError);

    // ========== BASIC TYPE DEFINITIONS ==========
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

    // ========== CONFIGURATION SYSTEM ==========
    py::class_<Config, std::shared_ptr<Config>>(m, "Config", R"pbdoc(Advanced configuration system for NLM)
        
        The Config class provides a flexible key-value configuration system with
        validation, schema support, and batch operations. It integrates with the
        entire NLM pipeline for simulation and learning.
        
        Features:
        - JSON file loading/saving
        - Command-line argument parsing
        - Type validation and coercion
        - Schema validation
        - Batch operations
        - Configuration templates
        
        Configuration Keys:
        - simulation steps, timestep, max_energy, etc.
        - memory system parameters (capacity, decay rate)
        - learning parameters (learning rate, plasticity)
        - neuromodulation parameters
        - world configuration parameters
        "pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Raises:
                 ConfigurationError: If file cannot be read or parsed
             )pbdoc")
        // Improved Phase 5 fix - cleaner implementation
        .def("loadFromArgs", [](Config& self, const std::vector<std::string>& args) {
            std::vector<char*> argv;
            argv.reserve(args.size());
            for (const auto& a : args) argv.push_back(const_cast<char*>(a.c_str()));
            return self.loadFromArgs(static_cast<int>(argv.size()), argv.data());
        }, py::arg("args"),
             R"pbdoc(Load configuration from command line arguments.
             
             Args:
                 args: List of command line argument strings (without program name)
                 
             Returns:
                 bool: True if arguments were parsed successfully
                 
             Raises:
                 ConfigurationError: If arguments cannot be parsed
             )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(Save configuration to a JSON file.
             
             Args:
                 filepath: Path where to save the configuration
                 
             Raises:
                 ConfigurationError: If file cannot be written
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 bool: True if key exists
             )pbdoc")
        .def("set", &Config::set, py::arg("key"), py::arg("value"),
             R"pbdoc(Set a configuration value.
             
             Args:
                 key: Configuration key
                 value: Configuration value (will be converted to appropriate type)
                 
             Raises:
                 ConfigurationError: If key is invalid or value cannot be coerced
             )pbdoc")
        .def("get", py::overload_cast<const std::string&, float>(&Config::get),
             py::arg("key"), py::arg("defaultValue") = 0.0f,
             R"pbdoc(Get a configuration value with type conversion.
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 float: Configuration value as float
             )pbdoc")
        .def("getInt", py::overload_cast<const std::string&, int>(&Config::get),
             py::arg("key"), py::arg("defaultValue") = 0,
             R"pbdoc(Get a configuration value as integer.
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 int: Configuration value as integer
             )pbdoc")
        .def("getBool", py::overload_cast<const std::string&, bool>(&Config::get),
             py::arg("key"), py::arg("defaultValue") = false,
             R"pbdoc(Get a configuration value as boolean.
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 bool: Configuration value as boolean
             )pbdoc")
        .def("getString", py::overload_cast<const std::string&, const std::string&>(&Config::get),
             py::arg("key"), py::arg("defaultValue") = "",
             R"pbdoc(Get a configuration value as string.
             
             Args:
                 key: Configuration key
                 defaultValue: Default value if key doesn't exist
                 
             Returns:
                 str: Configuration value as string
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(Get all configuration keys.
             
             Returns:
                 list: List of all configuration keys
             )pbdoc")
        .def("validateSchema", &Config::validateSchema,
             R"pbdoc(Validate configuration against schema.
             
             Returns:
                 bool: True if configuration is valid
                 
             Raises:
                 ConfigurationError: If validation fails
             )pbdoc")
        .def("applyTemplate", &Config::applyTemplate, py::arg("templateName"),
             R"pbdoc(Apply a configuration template.
             
             Args:
                 templateName: Name of the template to apply
                 
             Raises:
                 ConfigurationError: If template doesn't exist or application fails
             )pbdoc")
        .def("getSection", &Config::getSection, py::arg("section"),
             R"pbdoc(Get a section of configuration values.
             
             Args:
                 section: Section name (e.g., "memory.working_memory")
                 
             Returns:
                 dict: Dictionary of configuration values in the section
             )pbdoc")
        .def("setSection", &Config::setSection, py::arg("section"), py::arg("values"),
             R"pbdoc(Set multiple configuration values in a section.
             
             Args:
                 section: Section name
                 values: Dictionary of configuration values to set
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(Clear all configuration entries.
             
             Raises:
                 ConfigurationError: If clear operation fails
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(Get a summary string of the configuration.
             
             Returns:
                 str: Human-readable summary of configuration
             )pbdoc")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    // ========== SENSORY SYSTEMS ==========
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

    // ========== MOTOR SYSTEMS ==========
    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone)
        .def("to_dict", [](const Action& a) {
            return py::dict(
                "type", a.getType(),
                "parameters", a.getParameters(),
                "name", a.getName()
            );
        }, "Convert action to dictionary representation");

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
        .def("to_dict", [](const WorldObject& obj) {
            return py::dict(
                "x", obj.x,
                "y", obj.y,
                "radius", obj.radius,
                "type", obj.type,
                "value", obj.value,
                "active", obj.active
            );
        }, "Convert world object to dictionary representation");

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
        .def("reset", &AgentBody::reset)
        .def("to_dict", [](const AgentBody& body) {
            return py::dict(
                "x", body.x,
                "y", body.y,
                "orientation", body.orientation,
                "velocityX", body.velocityX,
                "velocityY", body.velocityY,
                "angularVelocity", body.angularVelocity,
                "energy", body.energy,
                "health", body.health,
                "age", body.age,
                "isMoving", body.isMoving,
                "isTurning", body.isTurning,
                "lastActionTime", body.lastActionTime
            );
        }, "Convert agent body to dictionary representation");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message)
        .def("to_dict", [](const ActionResult& result) {
            return py::dict(
                "reward", result.reward,
                "success", result.success,
                "message", result.message
            );
        }, "Convert action result to dictionary representation");

    // ========== AGENT INTERFACE ==========
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

    // ========== WORLD INTERFACE ==========
    py::class_<SimpleWorld, std::shared_ptr<SimpleWorld>>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)pbdoc")
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
        .def("getRandomSeed", &SimpleWorld::getRandomSeed)
        .def("to_dict", [](const SimpleWorld& world) {
            return py::dict(
                "width", world.getWidth(),
                "height", world.getHeight(),
                "simulationTime", world.getSimulationTime()
            );
        }, "Convert world to dictionary representation");

    // ========== CORE BRAIN INTERFACE ==========
    py::class_<Brain, std::shared_ptr<Brain>>(m, "Brain", R"pbdoc(Central neural simulation brain class)pbdoc")
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
        // getRegion is const/non-const overloaded; bind the mutable one.
        .def("getRegion", static_cast<NeuralRegion* (Brain::*)(RegionId)>(&Brain::getRegion),
             py::arg("id"),
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
        // Memory system access
        .def("getWorkingMemory", &Brain::getWorkingMemory,
             py::return_value_policy::reference_internal,
             "Get working memory system")
        .def("getEpisodicMemory", &Brain::getEpisodicMemory,
             py::return_value_policy::reference_internal,
             "Get episodic memory system")
        .def("getAssociativeMemory", &Brain::getAssociativeMemory,
             py::return_value_policy::reference_internal,
             "Get associative memory system")
        // Prediction system access
        .def("getPredictionSystem", &Brain::getPredictionSystem,
             py::return_value_policy::reference_internal,
             "Get prediction system")
        // Cognition system access
        .def("getPlanner", &Brain::getPlanner,
             py::return_value_policy::reference_internal,
             "Get neural planner")
        .def("getConceptFormation", &Brain::getConceptFormation,
             py::return_value_policy::reference_internal,
             "Get concept formation system")
        .def("getAttention", &Brain::getAttention,
             py::return_value_policy::reference_internal,
             "Get attentional selection system")
        // Neuromodulation system access
        .def("getDopamine", &Brain::getDopamine,
             py::return_value_policy::reference_internal,
             "Get dopamine neuromodulator")
        .def("getCuriosity", &Brain::getCuriosity,
             py::return_value_policy::reference_internal,
             "Get curiosity neuromodulator")
        .def("getNovelty", &Brain::getNovelty,
             py::return_value_policy::reference_internal,
             "Get novelty neuromodulator")
        .def("getPredictionErrorSignal", &Brain::getPredictionErrorSignal,
             py::return_value_policy::reference_internal,
             "Get prediction error signal")
        // Development system access
        .def("getDevelopmentSystem", &Brain::getDevelopmentSystem,
             py::return_value_policy::reference_internal,
             "Get development system")
        // AgentBrain interface
        .def("enableRewardModulation", &Brain::applyNeuromodulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation")
        .def("enableStructuralPlasticity", [](Brain& self) {
            if (auto* plasticity = self.getStructuralPlasticity()) {
                plasticity->enable(true);
            }
        }, "Enable structural plasticity")
        .def("enableDevelopment", [](Brain& self) {
            if (auto* dev = self.getDevelopmentSystem()) {
                dev->setStage(DevelopmentalStage::CriticalPeriod);
            }
        }, "Enable development")
        .def("enableCuriosity", [](Brain& self) {
            if (auto* curiosity = self.getCuriosity()) {
                curiosity->setLevel(1.0f);
            }
        }, "Enable curiosity")
        .def("isRewardModulationEnabled", []() { return false; }, "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", [](Brain& self) {
            if (auto* plasticity = self.getStructuralPlasticity()) {
                return plasticity->isEnabled();
            }
            return false;
        }, "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", [](Brain& self) {
            if (auto* dev = self.getDevelopmentSystem()) {
                return dev->getStage() != DevelopmentalStage::Initial;
            }
            return false;
        }, "Check if development is enabled")
        .def("isCuriosityEnabled", [](Brain& self) {
            if (auto* curiosity = self.getCuriosity()) {
                return curiosity->getLevel() > 0.0f;
            }
            return false;
        }, "Check if curiosity is enabled");

    // ========== AGENT INTERFACE ==========
    py::class_<AgentBrain, std::shared_ptr<AgentBrain>>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)pbdoc")
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

    // ========== MEMORY SYSTEMS ==========
    
    // NeuralWorkingMemory with pattern storage and retrieval
    py::class_<NeuralWorkingMemory>(m, "NeuralWorkingMemory", R"pbdoc(Neural working memory with persistent activity)
        
        NeuralWorkingMemory maintains active information through persistent
        neural activity and recurrent connections. It implements attention,
        short-term memory, and information maintenance mechanisms.
        
        Key Features:
        - Pattern-based storage and retrieval
        - Persistent neural activity
        - Recurrent connections for maintenance
        - Competitive selection between traces
        - Decay mechanisms for trace expiration
        
        Example:
        ```python
        working_memory = brain.getWorkingMemory()
        working_memory.store([0.1, 0.5, 0.9, 0.3])  # Store pattern
        retrieved = working_memory.retrieve()  # Get current content
        working_memory.clear()  # Clear memory
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralWorkingMemory::initialize, py::arg("brain"),
             R"pbdoc(Initialize working memory with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("store", &NeuralWorkingMemory::store, py::arg("pattern"), py::arg("strength") = 1.0f,
             R"pbdoc(Store information in working memory.
             
             Args:
                 pattern: Neural activity pattern to store
                 strength: Initial activation strength (0.0-1.0)
             )pbdoc")
        .def("storeToNeuron", &NeuralWorkingMemory::storeToNeuron, py::arg("neuron"), py::arg("activation"),
             R"pbdoc(Store activation directly to a neuron.
             
             Args:
                 neuron: Target neuron ID
                 activation: Activation level to set
             )pbdoc")
        .def("retrieve", &NeuralWorkingMemory::retrieve,
             R"pbdoc(Retrieve current working memory content as activity levels.
             
             Returns:
                 list: Current neural activity patterns
             )pbdoc")
        .def("contains", &NeuralWorkingMemory::contains, py::arg("neuron"),
             R"pbdoc(Check if specific neuron is part of working memory.
             
             Args:
                 neuron: Neuron ID to check
                 
             Returns:
                 bool: True if neuron is in working memory
             )pbdoc")
        .def("getNeuronActivation", &NeuralWorkingMemory::getNeuronActivation, py::arg("neuron"),
             R"pbdoc(Get activation level of a specific neuron in working memory.
             
             Args:
                 neuron: Neuron ID
                 
             Returns:
                 float: Activation level (0.0-1.0)
             )pbdoc")
        .def("update", &NeuralWorkingMemory::update, py::arg("dt"),
             R"pbdoc(Update working memory (maintenance and decay).
             
             Args:
                 dt: Time step duration
             )pbdoc")
        .def("clear", &NeuralWorkingMemory::clear,
             R"pbdoc(Clear working memory completely.
             )pbdoc")
        .def("getActiveTraces", &NeuralWorkingMemory::getActiveTraces,
             R"pbdoc(Get number of active memory traces.
             
             Returns:
                 int: Number of active traces
             )pbdoc")
        .def("getMemoryNeuronCount", &NeuralWorkingMemory::getMemoryNeuronCount,
             R"pbdoc(Get total number of neurons in working memory.
             
             Returns:
                 int: Number of neurons
             )pbdoc")
        .def("getMemoryActivations", &NeuralWorkingMemory::getMemoryActivations,
             R"pbdoc(Get current activation levels of memory neurons.
             
             Returns:
                 list: Activation levels for each neuron
             )pbdoc")
        .def("getMemoryTimestamps", &NeuralWorkingMemory::getMemoryTimestamps,
             R"pbdoc(Get timestamps for memory entries.
             
             Returns:
                 list: Timestamps for each memory entry
             )pbdoc")
        .def("getCapacity", &NeuralWorkingMemory::getCapacity,
             R"pbdoc(Get working memory capacity.
             
             Returns:
                 int: Maximum number of stored patterns
             )pbdoc")
        .def("setCapacity", &NeuralWorkingMemory::setCapacity, py::arg("capacity"),
             R"pbdoc(Set working memory capacity.
             
             Args:
                 capacity: New capacity (must be > 0)
             )pbdoc")
        .def("getDecayRate", &NeuralWorkingMemory::getDecayRate,
             R"pbdoc(Get decay rate for memory traces.
             
             Returns:
                 float: Decay rate per timestep
             )pbdoc")
        .def("setDecayRate", &NeuralWorkingMemory::setDecayRate, py::arg("rate"),
             R"pbdoc(Set decay rate for memory traces.
             
             Args:
                 rate: New decay rate (0.0-1.0)
             )pbdoc")
        .def("getMemoryNeurons", &NeuralWorkingMemory::getMemoryNeurons,
             R"pbdoc(Get neurons currently in working memory.
             
             Returns:
                 list: List of neuron IDs in working memory
             )pbdoc")
        .def("strengthenMemory", &NeuralWorkingMemory::strengthenMemory, py::arg("factor"),
             R"pbdoc(Strengthen working memory representation (for rehearsal).
             
             Args:
                 factor: Multiplicative factor (e.g., 1.5 to strengthen 50%)
             )pbdoc")
        .def("runCompetition", &NeuralWorkingMemory::runCompetition,
             R"pbdoc(Competition between memory traces.
             )pbdoc")
        .def("isWinning", &NeuralWorkingMemory::isWinning, py::arg("neuron"),
             R"pbdoc(Check if neuron is part of the winning population.
             
             Args:
                 neuron: Neuron ID to check
                 
             Returns:
                 bool: True if neuron is in winning population
             )pbdoc")
        .def("getMemoryActivity", &NeuralWorkingMemory::getMemoryActivity,
             R"pbdoc(Get overall memory activity level.
             
             Returns:
                 float: Overall activity level (0.0-1.0)
             )pbdoc")
        .def("createRecurrentConnection", &NeuralWorkingMemory::createRecurrentConnection,
             py::arg("fromNeuron"), py::arg("toNeuron"), py::arg("strength"),
             R"pbdoc(Create a recurrent connection for memory maintenance.
             
             Args:
                 fromNeuron: Source neuron ID
                 toNeuron: Target neuron ID
                 strength: Connection strength
             )pbdoc")
        .def("getMemoryState", [](const NeuralWorkingMemory& wmem) {
            return py::dict(
                "activeTraces", wmem.getActiveTraces(),
                "neuronCount", wmem.getMemoryNeuronCount(),
                "capacity", wmem.getCapacity(),
                "decayRate", wmem.getDecayRate(),
                "activity", wmem.getMemoryActivity()
            );
        }, R"pbdoc(Get current memory state as dictionary.
        
        Returns:
            dict: Memory statistics including active traces,
                  neuron count, capacity, decay rate, and activity level
        )pbdoc");

    // NeuralEpisodicMemory with episode management
    py::class_<NeuralEpisodicMemory>(m, "NeuralEpisodicMemory", R"pbdoc(Neural episodic memory with experience storage)
        
        NeuralEpisodicMemory stores experiences as neural activity patterns
        and supports retrieval through pattern completion, temporal windows,
        and relevance weighting. Episodes can be replayed for consolidation.
        
        Key Features:
        - Pattern-based episode storage and retrieval
        - Temporal indexing and retrieval
        - Spatial and action-based indexing
        - Replay mechanisms for memory consolidation
        - Relevance-weighted recall
        
        Example:
        ```python
        episodic_memory = brain.getEpisodicMemory()
        
        # Create and store an episode
        episode = {
            "timestamp": step,
            "sensoryState": [0.1, 0.5, 0.9],
            "positionX": 10.0,
            "positionY": 5.0,
            "action": nlm.ActionType.Eat,
            "reward": 1.0,
            "energy": 0.8,
            "novelty": 0.3
        }
        
        episodic_memory.storeEpisode(episode)
        
        # Retrieve similar episodes
        similar = episodic_memory.retrieveSimilar([0.1, 0.5, 0.8])
        
        # Get recent episodes
        recent = episodic_memory.getRecentEpisodes(5)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralEpisodicMemory::initialize, py::arg("brain"),
             R"pbdoc(Initialize episodic memory with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("storeEpisode", &NeuralEpisodicMemory::storeEpisode, py::arg("episode"),
             R"pbdoc(Store a new experience episode.
             
             Args:
                 episode: EpisodicMemoryItem containing the experience
             )pbdoc")
        .def("retrieveSimilar", &NeuralEpisodicMemory::retrieveSimilar,
             py::arg("sensoryPattern"), py::arg("maxResults") = 5,
             R"pbdoc(Retrieve episodes similar to a query pattern.
             
             Args:
                 sensoryPattern: Query sensory pattern
                 maxResults: Maximum number of results to return (default: 5)
                 
             Returns:
                 list: List of EpisodicMemoryItem pointers ordered by relevance
             )pbdoc")
        .def("retrieveTemporal", &NeuralEpisodicMemory::retrieveTemporal,
             py::arg("startTime"), py::arg("endTime"), py::arg("maxResults") = 10,
             R"pbdoc(Retrieve episodes from a specific time window.
             
             Args:
                 startTime: Start of time window
                 endTime: End of time window
                 maxResults: Maximum number of results (default: 10)
                 
             Returns:
                 list: List of matching episodes
             )pbdoc")
        .def("retrieveByLocation", &NeuralEpisodicMemory::retrieveByLocation,
             py::arg("x"), py::arg("y"), py::arg("radius"), py::arg("maxResults") = 5,
             R"pbdoc(Retrieve episodes by location.
             
             Args:
                 x: X coordinate
                 y: Y coordinate
                 radius: Search radius
                 maxResults: Maximum number of results (default: 5)
                 
             Returns:
                 list: List of episodes near the location
             )pbdoc")
        .def("retrieveAfterAction", &NeuralEpisodicMemory::retrieveAfterAction,
             py::arg("action"), py::arg("maxResults") = 5,
             R"pbdoc(Retrieve episodes following a specific action.
             
             Args:
                 action: Action type to search for
                 maxResults: Maximum number of results (default: 5)
                 
             Returns:
                 list: List of episodes after the action
             )pbdoc")
        .def("replayEpisode", &NeuralEpisodicMemory::replayEpisode, py::arg("episode"),
             R"pbdoc(Replay an episode - reactivates neural patterns.
             
             Args:
                 episode: Episode to replay
             )pbdoc")
        .def("updateRelevance", &NeuralEpisodicMemory::updateRelevance,
             py::arg("episodeId"), py::arg("relevanceDelta"),
             R"pbdoc(Update episode relevance based on recent use.
             
             Args:
                 episodeId: Index of episode to update
                 relevanceDelta: Change in relevance (can be negative)
             )pbdoc")
        .def("getEpisode", &NeuralEpisodicMemory::getEpisode, py::arg("index"),
             R"pbdoc(Get episode by index.
             
             Args:
                 index: Episode index (0-based)
                 
             Returns:
                 EpisodicMemoryItem: The requested episode
             )pbdoc")
        .def("getEpisodeCount", &NeuralEpisodicMemory::getEpisodeCount,
             R"pbdoc(Get total number of episodes.
             
             Returns:
                 int: Total number of stored episodes
             )pbdoc")
        .def("getEpisodes", &NeuralEpisodicMemory::getEpisodes,
             R"pbdoc(Get all episodes (Phase 5 checkpoint support).
             
             Returns:
                 deque: All stored episodes
             )pbdoc")
        .def("restoreEpisodes", &NeuralEpisodicMemory::restoreEpisodes, py::arg("episodes"),
             R"pbdoc(Restore episodes from a snapshot (Phase 5 checkpoint support).
             
             Args:
                 episodes: List of episodes to restore
             )pbdoc")
        .def("setMaxEpisodes", &NeuralEpisodicMemory::setMaxEpisodes, py::arg("n"),
             R"pbdoc(Set maximum number of episodes to store.
             
             Args:
                 n: Maximum number of episodes
             )pbdoc")
        .def("getRecentEpisodes", &NeuralEpisodicMemory::getRecentEpisodes, py::arg("count"),
             R"pbdoc(Get recent episodes.
             
             Args:
                 count: Number of recent episodes to retrieve
                 
             Returns:
                 list: List of recent episodes
             )pbdoc")
        .def("getAverageReward", &NeuralEpisodicMemory::getAverageReward,
             R"pbdoc(Get average reward from episodes.
             
             Returns:
                 float: Average reward value
             )pbdoc")
        .def("consolidate", &NeuralEpisodicMemory::consolidate, py::arg("threshold"),
             R"pbdoc(Consolidate - clear old episodes below relevance threshold.
             
             Args:
                 threshold: Minimum relevance for episodes to keep
             )pbdoc")
        .def("clear", &NeuralEpisodicMemory::clear,
             R"pbdoc(Clear all episodes.
             )pbdoc")
        .def("enableReplay", &NeuralEpisodicMemory::enableReplay, py::arg("enable"),
             R"pbdoc(Enable or disable replay mechanism.
             
             Args:
                 enable: True to enable replay, False to disable
             )pbdoc")
        .def("isReplayEnabled", &NeuralEpisodicMemory::isReplayEnabled,
             R"pbdoc(Check if replay is enabled.
             
             Returns:
                 bool: True if replay is enabled
             )pbdoc")
        .def("getEpisodesForReplay", &NeuralEpisodicMemory::getEpisodesForReplay, py::arg("count"),
             R"pbdoc(Get episodes for replay (selection based on relevance and recency).
             
             Args:
                 count: Number of episodes to select
                 
             Returns:
                 list: List of episodes selected for replay
             )pbdoc")
        .def("replaySequence", &NeuralEpisodicMemory::replaySequence, py::arg("episodeIds"),
             R"pbdoc(Replay multiple episodes (for sleep-like consolidation).
             
             Args:
                 episodeIds: List of episode indices to replay
             )pbdoc")
        .def("getMemoryState", [](const NeuralEpisodicMemory& mem) {
            return py::dict(
                "episodeCount", mem.getEpisodeCount(),
                "maxEpisodes", mem.maxEpisodes_,
                "replayEnabled", mem.isReplayEnabled(),
                "averageReward", mem.getAverageReward()
            );
        }, R"pbdoc(Get current memory state as dictionary.
        
        Returns:
            dict: Memory statistics including episode count,
                  max episodes, replay status, and average reward
        )pbdoc");

    // NeuralAssociativeMemory for pattern associations
    py::class_<NeuralAssociativeMemory>(m, "NeuralAssociativeMemory", R"pbdoc(Neural associative memory for pattern relationships)
        
        NeuralAssociativeMemory creates and retrieves relationships between
        experiences through synaptic connectivity. Stronger associations form
        with repeated co-activation, enabling spreading activation for recall.
        
        Key Features:
        - Hebbian-style association formation
        - Pattern-based retrieval
        - Spreading activation through association chains
        - Strength-based association updating
        - Pattern completion from partial cues
        
        Example:
        ```python
        associative_memory = brain.getAssociativeMemory()
        
        # Create associations between patterns
        pattern_a = [0.1, 0.5, 0.9]
        pattern_b = [0.2, 0.6, 0.8]
        associative_memory.associate(pattern_a, pattern_b, strength=0.7)
        
        # Retrieve patterns associated with a query
        associated = associative_memory.retrieve(pattern_a, max_results=3)
        
        # Spread activation through association chains
        spread = associative_memory.spreadActivation(pattern_a, steps=2)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralAssociativeMemory::initialize, py::arg("brain"),
             R"pbdoc(Initialize associative memory with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("associate", &NeuralAssociativeMemory::associate,
             py::arg("patternA"), py::arg("patternB"), py::arg("strength") = 1.0f,
             R"pbdoc(Create association between two neural patterns.
             
             Args:
                 patternA: First pattern
                 patternB: Second pattern
                 strength: Association strength (0.0-1.0)
             )pbdoc")
        .def("associateFromExperience", &NeuralAssociativeMemory::associateFromExperience,
             py::arg("episode"),
             R"pbdoc(Create association based on experience.
             
             Args:
                 episode: EpisodicMemoryItem to create associations from
             )pbdoc")
        .def("retrieve", &NeuralAssociativeMemory::retrieve,
             py::arg("queryPattern"), py::arg("maxResults") = 5,
             R"pbdoc(Retrieve patterns associated with a query pattern.
             
             Args:
                 queryPattern: Query pattern
                 maxResults: Maximum number of results (default: 5)
                 
             Returns:
                 list: List of retrieved patterns
             )pbdoc")
        .def("getAssociationStrength", &NeuralAssociativeMemory::getAssociationStrength,
             py::arg("patternA"), py::arg("patternB"),
             R"pbdoc(Get association strength between two patterns.
             
             Args:
                 patternA: First pattern
                 patternB: Second pattern
                 
             Returns:
                 float: Association strength (0.0-1.0)
             )pbdoc")
        .def("updateAssociation", &NeuralAssociativeMemory::updateAssociation,
             py::arg("patternA"), py::arg("patternB"), py::arg("delta"),
             R"pbdoc(Update association based on outcome.
             
             Args:
                 patternA: First pattern
                 patternB: Second pattern
                 delta: Change in strength (can be positive or negative)
             )pbdoc")
        .def("spreadActivation", &NeuralAssociativeMemory::spreadActivation,
             py::arg("cuePattern"), py::arg("steps") = 2,
             R"pbdoc(Spread activation from cue pattern through association chains.
             
             Args:
                 cuePattern: Starting pattern for spreading
                 steps: Number of association steps to propagate (default: 2)
                 
             Returns:
                 list: Patterns activated through association chains
             )pbdoc")
        .def("clear", &NeuralAssociativeMemory::clear,
             R"pbdoc(Clear all associations.
             )pbdoc")
        .def("getAssociationCount", &NeuralAssociativeMemory::getAssociationCount,
             R"pbdoc(Get number of associations.
             
             Returns:
                 int: Number of stored associations
             )pbdoc")
        .def("getMemoryState", [](const NeuralAssociativeMemory& mem) {
            return py::dict(
                "associationCount", mem.getAssociationCount()
            );
        }, R"pbdoc(Get current memory state as dictionary.
        
        Returns:
            dict: Memory statistics including association count
        )pbdoc");

    // ========== COGNITION SYSTEMS ==========

    // NeuralPlanner for action planning
    py::class_<NeuralPlanner>(m, "NeuralPlanner", R"pbdoc(Neural planner for action planning)
        
        NeuralPlanner uses learned predictions to plan multi-step actions
        through predictive neural dynamics rather than explicit search.
        
        Key Features:
        - Predictive action evaluation through neural simulation
        - Multi-step planning with depth control
        - Learning from action outcomes
        - Goal-directed planning
        - Confidence-based action selection
        
        Example:
        ```python
        planner = brain.getPlanner()
        
        # Set planning depth and goal
        planner.setPlanningDepth(5)
        planner.setCurrentGoal([10.0, 5.0, 0.8])
        
        # Plan next action
        current_state = [8.0, 4.0, 0.7]
        action = planner.planAction(current_state)
        
        # Evaluate action sequence
        actions = [ActionType.MoveForward, ActionType.Look]
        candidate = planner.evaluateSequence(actions, current_state)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &NeuralPlanner::initialize, py::arg("brain"),
             R"pbdoc(Initialize planner with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("planAction", &NeuralPlanner::planAction,
             py::arg("currentState"), py::arg("targetReward") = 0.5f,
             R"pbdoc(Plan next action given current state and goal.
             
             Args:
                 currentState: Current neural state vector
                 targetReward: Target reward value (default: 0.5)
                 
             Returns:
                 ActionType: Best action to take now
             )pbdoc")
        .def("evaluateSequence", &NeuralPlanner::evaluateSequence,
             py::arg("actions"), py::arg("startState"),
             R"pbdoc(Evaluate a potential action sequence.
             
             Args:
                 actions: List of actions to evaluate
                 startState: Starting neural state vector
                 
             Returns:
                 PlanningCandidate: Evaluation results with expected reward and confidence
             )pbdoc")
        .def("getPlanningDepth", &NeuralPlanner::getPlanningDepth,
             R"pbdoc(Get number of steps to look ahead.
             
             Returns:
                 int: Planning depth
             )pbdoc")
        .def("setPlanningDepth", &NeuralPlanner::setPlanningDepth, py::arg("depth"),
             R"pbdoc(Set planning depth.
             
             Args:
                 depth: Number of steps to look ahead
             )pbdoc")
        .def("getPlanningConfidence", &NeuralPlanner::getPlanningConfidence,
             R"pbdoc(Get planning confidence.
             
             Returns:
                 float: Planning confidence (0.0-1.0)
             )pbdoc")
        .def("updatePlanQuality", &NeuralPlanner::updatePlanQuality,
             py::arg("plannedActions"), py::arg("actualActions"), py::arg("actualReward"),
             R"pbdoc(Update plans based on actual outcome.
             
             Args:
                 plannedActions: Originally planned action sequence
                 actualActions: Actually executed actions
                 actualReward: Actual reward received
             )pbdoc")
        .def("clearCache", &NeuralPlanner::clearCache,
             R"pbdoc(Clear planning cache.
             )pbdoc")
        .def("setActionQuality", &NeuralPlanner::setActionQuality, py::arg("action"), py::arg("quality"),
             R"pbdoc(Set action quality function learned from experience.
             
             Args:
                 action: Action type
                 quality: Quality score (0.0-1.0)
             )pbdoc")
        .def("getActionQuality", &NeuralPlanner::getActionQuality, py::arg("action"),
             R"pbdoc(Get action quality learned from experience.
             
             Args:
                 action: Action type
                 
             Returns:
                 float: Action quality (0.0-1.0), 0 if never tried
             )pbdoc")
        .def("getCurrentGoal", &NeuralPlanner::getCurrentGoal,
             R"pbdoc(Get current goal.
             
             Returns:
                 list: Current goal state vector
             )pbdoc")
        .def("setCurrentGoal", &NeuralPlanner::setCurrentGoal, py::arg("goal"),
             R"pbdoc(Set current goal.
             
             Args:
                 goal: Goal state vector
             )pbdoc")
        .def("wasRecentPlanSuccessful", &NeuralPlanner::wasRecentPlanSuccessful,
             R"pbdoc(Check if recent planning has been successful.
             
             Returns:
                 bool: True if recent plans were successful
             )pbdoc")
        .def("getPlanningState", [](const NeuralPlanner& planner) {
            return py::dict(
                "depth", planner.getPlanningDepth(),
                "confidence", planner.getPlanningConfidence(),
                "hasCurrentGoal", not planner.getCurrentGoal().empty(),
                "recentSuccess", planner.wasRecentPlanSuccessful()
            );
        }, R"pbdoc(Get current planning state as dictionary.
        
        Returns:
            dict: Planning statistics including depth, confidence,
                  goal status, and recent success rate
        )pbdoc");

    // ConceptFormation for abstraction
    py::class_<ConceptFormation>(m, "ConceptFormation", R"pbdoc(Neural concept formation for abstraction)
        
        ConceptFormation discovers recurring patterns from experience and forms
        abstract concepts through pattern clustering and prototype formation.
        
        Key Features:
        - Pattern clustering based on similarity
        - Prototype formation through averaging
        - Instance tracking for stability measurement
        - Category discovery from shared properties
        - No predefined labels - concepts emerge organically
        
        Example:
        ```python
        concept_formation = brain.getConceptFormation()
        
        # Present new experiences
        for step in range(100):
            pattern = get_sensory_pattern(step)
            features = extract_features(pattern)
            reward = get_reward(step)
            concept_id = concept_formation.presentExperience(pattern, features, reward, step)
        
        # Get discovered concepts
        for concept_id in range(concept_formation.getConceptCount()):
            prototype = concept_formation.getConceptPrototype(concept_id)
            stability = concept_formation.getConceptStability(concept_id)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &ConceptFormation::initialize, py::arg("brain"),
             R"pbdoc(Initialize concept formation with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("presentExperience", &ConceptFormation::presentExperience,
             py::arg("pattern"), py::arg("features"), py::arg("reward"), py::arg("currentTime"),
             R"pbdoc(Present a new experience to the concept formation system.
             
             Args:
                 pattern: Neural activity pattern
                 features: Associated sensory features
                 reward: Reward value
                 currentTime: Current simulation time
                 
             Returns:
                 int: Concept ID if this experience belongs to an existing concept,
                      0 if too early to tell, or new concept ID if novel
             )pbdoc")
        .def("getMatchingConcept", &ConceptFormation::getMatchingConcept,
             py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             R"pbdoc(Get the concept that best matches a pattern.
             
             Args:
                 pattern: Neural activity pattern to match
                 similarityThreshold: Similarity threshold (0.0-1.0, default: 0.7)
                 
             Returns:
                 int: Concept ID of best match, or 0 if no good match
             )pbdoc")
        .def("getConcepts", &ConceptFormation::getConcepts,
             R"pbdoc(Get all discovered concepts.
             
             Returns:
                 list: List of all DiscoveredConcept objects
             )pbdoc")
        .def("getConcept", &ConceptFormation::getConcept, py::arg("conceptId"),
             R"pbdoc(Get a specific concept.
             
             Args:
                 conceptId: Concept identifier
                 
             Returns:
                 DiscoveredConcept: The requested concept
             )pbdoc")
        .def("getConceptPrototype", &ConceptFormation::getConceptPrototype, py::arg("conceptId"),
             R"pbdoc(Get concept prototype.
             
             Args:
                 conceptId: Concept identifier
                 
             Returns:
                 list: Neural pattern representing the concept prototype
             )pbdoc")
        .def("getConceptInstances", &ConceptFormation::getConceptInstances, py::arg("conceptId"),
             R"pbdoc(Get concept instances.
             
             Args:
                 conceptId: Concept identifier
                 
             Returns:
                 list: List of ConceptInstance objects belonging to this concept
             )pbdoc")
        .def("getConceptStability", &ConceptFormation::getConceptStability, py::arg("conceptId"),
             R"pbdoc(Measure concept stability (0-1).
             
             Args:
                 conceptId: Concept identifier
                 
             Returns:
                 float: Stability measure (0.0-1.0)
             )pbdoc")
        .def("getConceptCount", &ConceptFormation::getConceptCount,
             R"pbdoc(Get number of concepts discovered.
             
             Returns:
                 int: Number of concepts
             )pbdoc")
        .def("mergeConcepts", &ConceptFormation::mergeConcepts, py::arg("conceptA"), py::arg("conceptB"),
             R"pbdoc(Merge two concepts (when discovered they are actually the same).
             
             Args:
                 conceptA: First concept ID
                 conceptB: Second concept ID
             )pbdoc")
        .def("updateConcept", &ConceptFormation::updateConcept,
             py::arg("conceptId"), py::arg("newPattern"), py::arg("features"), py::arg("reward"),
             R"pbdoc(Update concept with new instance.
             
             Args:
                 conceptId: Concept identifier
                 newPattern: New pattern instance
                 features: Associated features
                 reward: New reward value
             )pbdoc")
        .def("computeSimilarity", &ConceptFormation::computeSimilarity, py::arg("a"), py::arg("b"),
             R"pbdoc(Compute similarity between two patterns.
             
             Args:
                 a: First pattern
                 b: Second pattern
                 
             Returns:
                 float: Similarity measure (0.0-1.0)
             )pbdoc")
        .def("isNovel", &ConceptFormation::isNovel, py::arg("pattern"), py::arg("similarityThreshold") = 0.7f,
             R"pbdoc(Check if pattern is novel (doesn't belong to any existing concept).
             
             Args:
                 pattern: Neural activity pattern to check
                 similarityThreshold: Similarity threshold (0.0-1.0, default: 0.7)
                 
             Returns:
                 bool: True if pattern is novel
             )pbdoc")
        .def("findConceptForPattern", &ConceptFormation::findConceptForPattern, py::arg("pattern"),
             R"pbdoc(Get concept for a given pattern.
             
             Args:
                 pattern: Neural activity pattern
                 
             Returns:
                 int: Concept ID that best matches the pattern
             )pbdoc")
        .def("clear", &ConceptFormation::clear,
             R"pbdoc(Clear all concepts.
             )pbdoc")
        .def("getGeneralizationAbility", &ConceptFormation::getGeneralizationAbility, py::arg("conceptId"),
             R"pbdoc(Get generalization ability (can concept handle variations?).
             
             Args:
                 conceptId: Concept identifier
                 
             Returns:
                 float: Generalization ability measure
             )pbdoc")
        .def("setFormationThreshold", &ConceptFormation::setFormationThreshold, py::arg("threshold"),
             R"pbdoc(Set formation threshold for new concepts.
             
             Args:
                 threshold: Similarity threshold for concept formation
             )pbdoc")
        .def("setStabilityWindow", &ConceptFormation::setStabilityWindow, py::arg("window"),
             R"pbdoc(Set stability window for concept evaluation.
             
             Args:
                 window: Number of observations to consider for stability
             )pbdoc")
        .def("getConceptFormationState", [](const ConceptFormation& cf) {
            return py::dict(
                "conceptCount", cf.getConceptCount(),
                "formationThreshold", cf.formationThreshold_,
                "stabilityThreshold", cf.stabilityThreshold_,
                "stabilityWindow", cf.stabilityWindow_
            );
        }, R"pbdoc(Get current concept formation state as dictionary.
        
        Returns:
            dict: Concept formation statistics including thresholds
                  and window parameters
        )pbdoc");

    // AttentionalSelection for focus
    py::class_<AttentionalSelection>(m, "AttentionalSelection", R"pbdoc(Neural attentional selection)
        
        AttentionalSelection implements neural attention through competition
        between neural populations, using lateral inhibition and gain modulation
        rather than cognitive focus.
        
        Key Features:
        - Competition between neural populations
        - Salience-based selection
        - Recurrent amplification of winners
        - Inhibitory surround suppression
        - Top-down and bottom-up modulation
        
        Example:
        ```python
        attention = brain.getAttention()
        
        # Process competing sensory inputs
        competitors = [neuron1, neuron2, neuron3]
        winners = attention.processCompetition(competitors, global_inhibition=0.5)
        
        # Focus on a specific region
        attention.focusOnRegion(region_id)
        
        # Check if neuron is attended
        if attention.isAttended(neuron1):
            print("Neuron 1 is receiving focused attention")
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &AttentionalSelection::initialize, py::arg("brain"),
             R"pbdoc(Initialize attentional selection with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("processCompetition", &AttentionalSelection::processCompetition,
             py::arg("competitors"), py::arg("globalInhibition") = 0.5f,
             R"pbdoc(Process competing sensory inputs and select winners.
             
             Args:
                 competitors: List of neuron IDs to compete
                 globalInhibition: Global inhibition strength (0.0-1.0)
                 
             Returns:
                 list: List of winning neuron IDs
             )pbdoc")
        .def("focusOnRegion", &AttentionalSelection::focusOnRegion, py::arg("region"),
             R"pbdoc(Apply attention to a specific region (focus processing there).
             
             Args:
                 region: Region ID to focus on
             )pbdoc")
        .def("releaseAttention", &AttentionalSelection::releaseAttention,
             R"pbdoc(Release attention (allow all regions equal processing).
             )pbdoc")
        .def("getAttendedRegions", &AttentionalSelection::getAttendedRegions,
             R"pbdoc(Get current attention focus.
             
             Returns:
                 list: List of attended region IDs
             )pbdoc")
        .def("setInhibitionStrength", &AttentionalSelection::setInhibitionStrength, py::arg("strength"),
             R"pbdoc(Set inhibition strength.
             
             Args:
                 strength: Inhibition strength (0.0-1.0)
             )pbdoc")
        .def("setExcitationStrength", &AttentionalSelection::setExcitationStrength, py::arg("strength"),
             R"pbdoc(Set excitation strength.
             
             Args:
                 strength: Excitation strength (0.0-1.0)
             )pbdoc")
        .def("setCompetitionThreshold", &AttentionalSelection::setCompetitionThreshold, py::arg("threshold"),
             R"pbdoc(Set competition threshold.
             
             Args:
                 threshold: Competition threshold (0.0-1.0)
             )pbdoc")
        .def("getInhibitionFor", &AttentionalSelection::getInhibitionFor, py::arg("neuron"),
             R"pbdoc(Get inhibition level for a neuron.
             
             Args:
                 neuron: Neuron ID
                 
             Returns:
                 float: Inhibition level (0.0-1.0)
             )pbdoc")
        .def("getExcitationFor", &AttentionalSelection::getExcitationFor, py::arg("neuron"),
             R"pbdoc(Get excitation level for a neuron.
             
             Args:
                 neuron: Neuron ID
                 
             Returns:
                 float: Excitation level (0.0-1.0)
             )pbdoc")
        .def("update", &AttentionalSelection::update, py::arg("dt"),
             R"pbdoc(Update attention dynamics.
             
             Args:
                 dt: Time step duration
             )pbdoc")
        .def("isAttended", &AttentionalSelection::isAttended, py::arg("neuron"),
             R"pbdoc(Check if neuron is currently attended.
             
             Args:
                 neuron: Neuron ID
                 
             Returns:
                 bool: True if neuron is attended
             )pbdoc")
        .def("getWinners", &AttentionalSelection::getWinners,
             R"pbdoc(Get attention winners.
             
             Returns:
                 list: List of winning neuron IDs
             )pbdoc")
        .def("applyTopDownBias", &AttentionalSelection::applyTopDownBias, py::arg("neuron"), py::arg("biasStrength"),
             R"pbdoc(Apply top-down attention (goal-directed).
             
             Args:
                 neuron: Neuron ID
                 biasStrength: Bias strength (positive increases attention)
             )pbdoc")
        .def("applyBottomUpSalience", &AttentionalSelection::applyBottomUpSalience, py::arg("neuron"), py::arg("salienceStrength"),
             R"pbdoc(Apply bottom-up attention (stimulus-driven).
             
             Args:
                 neuron: Neuron ID
                 salienceStrength: Salience strength (positive increases attention)
             )pbdoc")
        .def("reset", &AttentionalSelection::reset,
             R"pbdoc(Reset attention.
             )pbdoc")
        .def("getAttentionalState", [](const AttentionalSelection& attn) {
            return py::dict(
                "inhibitionStrength", attn.inhibitionStrength_,
                "excitationStrength", attn.excitationStrength_,
                "competitionThreshold", attn.competitionThreshold_,
                "hasAttendedRegions", not attn.getAttendedRegions().empty()
            );
        }, R"pbdoc(Get current attentional state as dictionary.
        
        Returns:
            dict: Attention statistics including inhibition/excitation
                  strengths, competition threshold, and focus status
        )pbdoc");

    // ========== NEUROMODULATION SYSTEMS ==========

    // Neuromodulator abstract base class
    py::class_<Neuromodulator>(m, "Neuromodulator", R"pbdoc(Abstract base class for neuromodulatory signals)
        
        Neuromodulators are chemicals that broadly influence neural processing,
        affecting plasticity, attention, motivation, and other global brain states.
        
        Key Features:
        - Abstract base class for all neuromodulators
        - Plasticity factor modulation
        - State management (level, updates)
        - Concrete implementations for specific neurotransmitters
        
        Subclasses:
        - Dopamine: Reward and reinforcement learning signal
        - Acetylcholine: Attention and memory consolidation
        - Norepinephrine: Arousal and vigilance
        - Serotonin: Mood, impulsivity, and social behavior
        "pbdoc")
        .def("getName", &Neuromodulator::getName,
             R"pbdoc(Get neuromodulator name.
             
             Returns:
                 str: Name of the neuromodulator
             )pbdoc")
        .def("getLevel", &Neuromodulator::getLevel,
             R"pbdoc(Get current concentration/level.
             
             Returns:
                 float: Neuromodulator level (0.0-1.0)
             )pbdoc")
        .def("setLevel", &Neuromodulator::setLevel, py::arg("level"),
             R"pbdoc(Set current concentration/level.
             
             Args:
                 level: Neuromodulator level (0.0-1.0)
             )pbdoc")
        .def("getPlasticityFactor", &Neuromodulator::getPlasticityFactor,
             R"pbdoc(Get plasticity factor (0.0-1.0).
             
             Returns:
                 float: Plasticity modulation factor
             )pbdoc")
        .def("update", &Neuromodulator::update, py::arg("dt"),
             R"pbdoc(Update neuromodulator state.
             
             Args:
                 dt: Time step duration
             )pbdoc");

    // Dopamine - reward and reinforcement learning
    py::class_<Dopamine, Neuromodulator>(m, "Dopamine", R"pbdoc(Dopamine neuromodulator)
        
        Dopamine encodes reward prediction error (RPE) and signals the difference
        between expected and actual outcomes. It drives learning and motivation.
        
        Key Features:
        - Reward signaling through prediction error
        - Reinforcement learning updates
        - Motivation and approach behavior
        - Structural plasticity modulation
        
        Example:
        ```python
        dopamine = brain.getDopamine()
        
        # Signal reward prediction error
        dopamine.signalReward(1.0)  # Actual reward
        dopamine.signalRewardPredictionError(0.1)  # RPE: 1.0 - 0.9 predicted
        
        # Get current level
        level = dopamine.getLevel()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("signalReward", &Dopamine::signalReward, py::arg("reward"),
             R"pbdoc(Signal reward (actual outcome).
             
             Args:
                 reward: Actual reward value
             )pbdoc")
        .def("signalRewardPredictionError", &Dopamine::signalRewardPredictionError, py::arg("error"),
             R"pbdoc(Signal reward prediction error.
             
             Args:
                 error: Prediction error (actual - predicted)
             )pbdoc");

    // Acetylcholine - attention and memory consolidation
    py::class_<Acetylcholine, Neuromodulator>(m, "Acetylcholine", R"pbdoc(Acetylcholine neuromodulator)
        
        Acetylcholine modulates attention, arousal, and memory consolidation.
        It enhances signal-to-noise ratio in neural processing.
        
        Key Features:
        - Enhanced attention and signal processing
        - Memory consolidation and strengthening
        - Increased neural plasticity
        - Learning rate modulation
        "pbdoc")
        .def(py::init<>());

    // Norepinephrine - arousal and vigilance
    py::class_<Norepinephrine, Neuromodulator>(m, "Norepinephrine", R"pbdoc(Norepinephrine neuromodulator)
        
        Norepinephrine regulates arousal, vigilance, and stress responses.
        It modulates attention and prepares the brain for action.
        
        Key Features:
        - Arousal and vigilance modulation
        - Stress response regulation
        - Enhanced processing during novel events
        - Attentional resource allocation
        "pbdoc")
        .def(py::init<>());

    // Serotonin - mood, impulsivity, and social behavior
    py::class_<Serotonin, Neuromodulator>(m, "Serotonin", R"pbdoc(Serotonin neuromodulator)
        
        Serotonin regulates mood, impulsivity, social behavior, and sleep.
        It influences decision-making and behavioral inhibition.
        
        Key Features:
        - Mood regulation
        - Behavioral inhibition and impulsivity control
        - Social behavior modulation
        - Sleep-wake cycle regulation
        "pbdoc")
        .def(py::init<>());

    // Curiosity - exploration motivation
    py::class_<Curiosity>(m, "Curiosity", R"pbdoc(Curiosity drive: exploration motivation)
        
        Curiosity computes exploration drive as a weighted combination of novelty
        and prediction error, motivating the agent to explore uncertain environments.
        
        Key Features:
        - Novelty-based exploration
        - Prediction error-driven curiosity
        - Exploration reward signal
        - Adaptive weighting of novelty vs prediction error
        
        Example:
        ```python
        curiosity = brain.getCuriosity()
        level = curiosity.getLevel()  # Current exploration drive (0.0-1.0)
        curiosity.setNoveltyWeight(0.6)  # Weight novelty more than prediction error
        curiosity.setPredictionErrorWeight(0.4)
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &Curiosity::initialize, py::arg("brain"),
             R"pbdoc(Initialize curiosity with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("getLevel", &Curiosity::getLevel,
             R"pbdoc(Get curiosity level.
             
             Returns:
                 float: Current curiosity level (0.0-1.0)
             )pbdoc")
        .def("update", &Curiosity::update, py::arg("novelty"), py::arg("predictionError"), py::arg("dt"),
             R"pbdoc(Update curiosity based on novelty and prediction error.
             
             Args:
                 novelty: Novelty level (0.0-1.0)
                 predictionError: Prediction error (0.0-1.0)
                 dt: Time step duration
             )pbdoc")
        .def("getExplorationDrive", &Curiosity::getExplorationDrive,
             R"pbdoc(Get exploration drive (same as level).
             
             Returns:
                 float: Exploration drive (0.0-1.0)
             )pbdoc")
        .def("setNoveltyWeight", &Curiosity::setNoveltyWeight, py::arg("weight"),
             R"pbdoc(Set novelty weight.
             
             Args:
                 weight: Weight for novelty (0.0-1.0)
             )pbdoc")
        .def("setPredictionErrorWeight", &Curiosity::setPredictionErrorWeight, py::arg("weight"),
             R"pbdoc(Set prediction error weight.
             
             Args:
                 weight: Weight for prediction error (0.0-1.0)
             )pbdoc")
        .def("reset", &Curiosity::reset,
             R"pbdoc(Reset curiosity state.
             )pbdoc")
        .def("getCuriosityState", [](const Curiosity& cur) {
            return py::dict(
                "level", cur.getLevel(),
                "explorationDrive", cur.getExplorationDrive()
            );
        }, R"pbdoc(Get current curiosity state as dictionary.
        
        Returns:
            dict: Curiosity statistics including current level
        )pbdoc");

    // Novelty - novelty detection
    py::class_<Novelty>(m, "Novelty", R"pbdoc(Neural novelty detection signal)
        
        Novelty detects novel events and computes novelty signals for curiosity
        and learning. It compares current observations with past experiences.
        
        Key Features:
        - Comparison with previous observations
        - Pattern-based novelty detection
        - Temporal novelty decay
        - Novelty history tracking
        
        Example:
        ```python
        novelty = brain.getNovelty()
        
        # Detect novelty from observations
        novelty.detectNovelty(current_observation, previous_observation)
        
        # Get current novelty level
        level = novelty.getLevel()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &Novelty::initialize, py::arg("brain"),
             R"pbdoc(Initialize novelty with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("getLevel", &Novelty::getLevel,
             R"pbdoc(Get novelty level.
             
             Returns:
                 float: Current novelty level (0.0-1.0)
             )pbdoc")
        .def("setLevel", &Novelty::setLevel, py::arg("level"),
             R"pbdoc(Set novelty level.
             
             Args:
                 level: Novelty level (0.0-1.0)
             )pbdoc")
        .def("detectNovelty", &Novelty::detectNovelty,
             py::arg("currentPattern"), py::arg("previousPattern"),
             R"pbdoc(Detect novelty from sensory input pattern.
             
             Args:
                 currentPattern: Current sensory pattern
                 previousPattern: Previous sensory pattern
             )pbdoc")
        .def("update", &Novelty::update, py::arg("dt"),
             R"pbdoc(Decay novelty over time.
             
             Args:
                 dt: Time step duration
             )pbdoc")
        .def("getHistory", &Novelty::getHistory,
             R"pbdoc(Get novelty history.
             
             Returns:
                 list: History of novelty values
             )pbdoc")
        .def("clearHistory", &Novelty::clearHistory,
             R"pbdoc(Clear novelty history.
             )pbdoc")
        .def("getNoveltyState", [](const Novelty& nov) {
            return py::dict(
                "level", nov.getLevel(),
                "historySize", nov.getHistory().size()
            );
        }, R"pbdoc(Get current novelty state as dictionary.
        
        Returns:
            dict: Novelty statistics including current level and history
        )pbdoc");

    // PredictionError - prediction error signal
    py::class_<PredictionError>(m, "PredictionError", R"pbdoc(Prediction error signal)
        
        PredictionError computes the difference between predicted and actual values,
        serving as a fundamental teaching signal for learning and adaptation.
        
        Key Features:
        - Prediction error computation
        - Prediction update
        - Error history tracking
        - Magnitude calculation for neuromodulation
        
        Example:
        ```python
        pred_error = brain.getPredictionErrorSignal()
        
        # Compute prediction error
        pred_error.computeError(0.9, 1.0)  # Predicted 0.9, actual 1.0
        
        # Get error magnitude for neuromodulation
        magnitude = pred_error.getMagnitude()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("initialize", &PredictionError::initialize, py::arg("brain"),
             R"pbdoc(Initialize prediction error with brain reference.
             
             Args:
                 brain: Brain instance to connect to
             )pbdoc")
        .def("getError", &PredictionError::getError,
             R"pbdoc(Get error value.
             
             Returns:
                 float: Current prediction error
             )pbdoc")
        .def("computeError", &PredictionError::computeError, py::arg("predicted"), py::arg("actual"),
             R"pbdoc(Compute prediction error.
             
             Args:
                 predicted: Predicted value
                 actual: Actual value
             )pbdoc")
        .def("updatePrediction", &PredictionError::updatePrediction, py::arg("newPrediction"),
             R"pbdoc(Update prediction.
             
             Args:
                 newPrediction: New predicted value
             )pbdoc")
        .def("getHistory", &PredictionError::getHistory,
             R"pbdoc(Get prediction error history.
             
             Returns:
                 list: History of prediction errors
             )pbdoc")
        .def("clearHistory", &PredictionError::clearHistory,
             R"pbdoc(Clear prediction error history.
             )pbdoc")
        .def("getMagnitude", &PredictionError::getMagnitude,
             R"pbdoc(Get error magnitude for neuromodulation.
             
             Returns:
                 float: Error magnitude (absolute value)
             )pbdoc")
        .def("getPredictionErrorState", [](const PredictionError& pe) {
            return py::dict(
                "error", pe.getError(),
                "magnitude", pe.getMagnitude(),
                "historySize", pe.getHistory().size()
            );
        }, R"pbdoc(Get current prediction error state as dictionary.
        
        Returns:
            dict: Prediction error statistics including current error,
                  magnitude, and history size
        )pbdoc");

    // ========== DEVELOPMENT SYSTEM ==========

    py::class_<DevelopmentSystem>(m, "DevelopmentSystem", R"pbdoc(Neural development system)
        
        DevelopmentSystem orchestrates neural development through developmental
        stages, plasticity modulation, and structural changes. It implements
        age-based plasticity and critical period regulation.
        
        Key Features:
        - Developmental stage progression
        - Critical period detection and management
        - Age-based plasticity modulation
        - Synaptogenesis and pruning
        - Maturation processes
        
        Example:
        ```python
        development = brain.getDevelopmentSystem()
        
        # Get current stage
        stage = development.getStage()
        
        # Check if in critical period
        if development.isCriticalPeriod():
            print("Critical period active - high plasticity")
        
        # Advance development
        development.advanceStage()
        
        # Get plasticity modifier
        plasticity = development.getPlasticityModifier()
        ```
    )pbdoc")
        .def(py::init<>())
        .def("getStage", &DevelopmentSystem::getStage,
             R"pbdoc(Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current developmental stage
             )pbdoc")
        .def("setStage", &DevelopmentSystem::setStage, py::arg("stage"),
             R"pbdoc(Set developmental stage.
             
             Args:
                 stage: Target developmental stage
             )pbdoc")
        .def("advanceStage", &DevelopmentSystem::advanceStage,
             R"pbdoc(Advance developmental stage.
             )pbdoc")
        .def("getStageName", &DevelopmentSystem::getStageName,
             R"pbdoc(Get stage name as string.
             
             Returns:
                 str: Human-readable stage name
             )pbdoc")
        .def("update", py::overload_cast<Brain*, SimulationStep>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("currentStep"),
             R"pbdoc(Update development (simple version).
             
             Args:
                 brain: Brain instance
                 currentStep: Current simulation step
             )pbdoc")
        .def("update", py::overload_cast<Brain*, RandomGenerator&, TimestepDuration>(&DevelopmentSystem::update),
             py::arg("brain"), py::arg("rng"), py::arg("dt"),
             R"pbdoc(Update development with full parameters.
             
             Args:
                 brain: Brain instance
                 rng: Random generator for stochastic processes
                 dt: Time step duration
             )pbdoc")
        .def("getPlasticityModifier", &DevelopmentSystem::getPlasticityModifier,
             R"pbdoc(Get plasticity modifier for current stage.
             
             Returns:
                 float: Plasticity modifier (0.0-1.0, higher = more plastic)
             )pbdoc")
        .def("isCriticalPeriod", &DevelopmentSystem::isCriticalPeriod,
             R"pbdoc(Check if in critical period.
             
             Returns:
                 bool: True if in critical period
             )pbdoc")
        .def("getCriticalPeriodProgress", &DevelopmentSystem::getCriticalPeriodProgress,
             R"pbdoc(Get critical period progress (0.0-1.0).
             
             Returns:
                 float: Progress through critical period
             )pbdoc")
        .def("getDevelopmentalAge", &DevelopmentSystem::getDevelopmentalAge,
             R"pbdoc(Get developmental age (in simulation time).
             
             Returns:
                 float: Developmental age
             )pbdoc")
        .def("setDevelopmentalAge", &DevelopmentSystem::setDevelopmentalAge, py::arg("age"),
             R"pbdoc(Set developmental age.
             
             Args:
                 age: Developmental age in simulation time
             )pbdoc")
        .def("getDevelopmentState", [](const DevelopmentSystem& dev) {
            return py::dict(
                "stage", dev.getStage(),
                "age", dev.getDevelopmentalAge(),
                "isCriticalPeriod", dev.isCriticalPeriod(),
                "plasticityModifier", dev.getPlasticityModifier()
            );
        }, R"pbdoc(Get current development state as dictionary.
        
        Returns:
            dict: Development statistics including stage, age,
                  critical period status, and plasticity modifier
        )pbdoc");

    // ========== UTILITY FUNCTIONS ==========

    // Advanced configuration utilities
    m.def("createAdvancedConfig", []() {
        auto config = std::make_shared<Config>();
        config->set("memory.working_memory.capacity", 100);
        config->set("memory.episodic_memory.max_episodes", 1000);
        config->set("cognition.planning.depth", 5);
        config->set("neuromodulation.curiosity.enabled", true);
        config->set("neuromodulation.novelty.enabled", true);
        return config;
    }, "Create an advanced configuration with optimal settings for complex simulations");

    m.def("validateConfig", [](const std::shared_ptr<Config>& config) {
        return config->validateSchema();
    }, py::arg("config"), "Validate configuration against schema");

    // High-level utility functions
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

    // Utility function for batch simulation steps
    m.def("runBatchSimulation", [](std::shared_ptr<Brain> brain, size_t steps, 
                                 std::function<void(std::shared_ptr<Brain>, size_t)> callback) {
        for (size_t i = 0; i < steps; ++i) {
            brain->step(i);
            callback(brain, i);
        }
    }, py::arg("brain"), py::arg("steps"), py::arg("callback"),
        "Run a batch simulation with progress callback");

    // Configuration schema validation utilities
    m.def("getConfigurationSchema", []() {
        return py::dict(
            "version", "1.0",
            "description", "NLM Configuration Schema",
            "type", "object",
            "properties", py::dict(
                ("memory.working_memory.capacity", py::dict(
                    ("type", "integer"),
                    ("minimum", 1),
                    ("maximum", 10000)
                )),
                ("memory.episodic_memory.max_episodes", py::dict(
                    ("type", "integer"),
                    ("minimum", 100),
                    ("maximum", 100000)
                )),
                ("cognition.planning.depth", py::dict(
                    ("type", "integer"),
                    ("minimum", 1),
                    ("maximum", 20)
                )),
                ("neuromodulation.curiosity.weight_novelty", py::dict(
                    ("type", "number"),
                    ("minimum", 0.0),
                    ("maximum", 1.0)
                ))
            ),
            ("required", py::list({
                py::cast("memory.working_memory.capacity"),
                py::cast("memory.episodic_memory.max_episodes")
            }))
        );
    }, "Get the configuration schema for validation");

    // Monitoring and logging utilities
    m.def("monitorSystemState", [](std::shared_ptr<Brain> brain) {
        return py::dict(
            "neuronCount", brain->getTotalNeuronCount(),
            "synapseCount", brain->getTotalSynapseCount(),
            "activeNeuronCount", brain->getActiveNeuronCount(),
            "firingNeuronCount", brain->getFiringNeuronCount(),
            "averageFiringRate", brain->getAverageFiringRate(),
            "excitationInhibitionRatio", brain->getExcitationInhibitionRatio(),
            "totalSpikeCount", brain->getTotalSpikeCount(),
            "developmentalStage", brain->getDevelopmentalStage()
        );
    }, py::arg("brain"), "Monitor and return the current system state");

    // Checkpoint management utilities
    m.def("createCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->save(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Create a checkpoint of brain state");

    m.def("restoreCheckpoint", [](std::shared_ptr<Brain> brain, const std::string& filepath) {
        return brain->load(filepath);
    }, py::arg("brain"), py::arg("filepath"), "Restore brain state from checkpoint");

    // Debugging and analysis utilities
    m.def("analyzeNeuralActivity", [](std::shared_ptr<Brain> brain) {
        auto region = brain->getRegion(brain->getRegionIds().front());
        if (region) {
            return py::dict(
                "regionCount", brain->getRegionCount(),
                "neuronCount", brain->getTotalNeuronCount(),
                "synapseCount", brain->getTotalSynapseCount(),
                "hasWorkingMemory", brain->getWorkingMemory() != nullptr,
                "hasEpisodicMemory", brain->getEpisodicMemory() != nullptr,
                "hasAssociativeMemory", brain->getAssociativeMemory() != nullptr
            );
        }
        return py::dict("error", "No regions found");
    }, py::arg("brain"), "Analyze neural activity and return system statistics");

    // Batch operations utility
    m.def("batchOperation", [](std::shared_ptr<Brain> brain, 
                           std::function<void(std::shared_ptr<Brain>)> operation,
                           size_t batchSize) {
        for (size_t i = 0; i < batchSize; ++i) {
            operation(brain);
        }
    }, py::arg("brain"), py::arg("operation"), py::arg("batchSize"), "Perform a batch operation on brain");

    // Visualization utilities
    m.def("createVisualizationData", [](std::shared_ptr<Brain> brain) {
        return py::dict(
            "neuronCount", brain->getTotalNeuronCount(),
            "activeNeuronCount", brain->getActiveNeuronCount(),
            "firingNeuronCount", brain->getFiringNeuronCount(),
            "excitationInhibitionRatio", brain->getExcitationInhibitionRatio(),
            "totalSpikeCount", brain->getTotalSpikeCount(),
            "hasWorkingMemory", brain->getWorkingMemory() != nullptr,
            "hasEpisodicMemory", brain->getEpisodicMemory() != nullptr,
            "hasAssociativeMemory", brain->getAssociativeMemory() != nullptr
        );
    }, py::arg("brain"), "Create visualization data from brain state");

    // Legacy constants (for backward compatibility)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Advanced constants and enums
    py::enum_<SimulationStep>(m, "SimulationStep", R"pbdoc(Simulation step type)pbdoc")
        .value("Initial", SimulationStep::Initial)
        .value("Step1", SimulationStep::Step1)
        .value("Step2", SimulationStep::Step2)
        .export_values();

    py::enum_<Timestamp>(m, "Timestamp", R"pbdoc(Timestamp type)pbdoc")
        .value("Microsecond", Timestamp::Microsecond)
        .value("Millisecond", Timestamp::Millisecond)
        .value("Second", Timestamp::Second)
        .value("Minute", Timestamp::Minute)
        .value("Hour", Timestamp::Hour)
        .export_values();

    py::enum_<MembranePotential>(m, "MembranePotential", R"pbdoc(Membrane potential type)pbdoc")
        .value("Resting", MembranePotential::Resting)
        .value("Depolarized", MembranePotential::Depolarized)
        .value("Hyperpolarized", MembranePotential::Hyperpolarized)
        .export_values();

    py::enum_<Delay>(m, "Delay", R"pbdoc(Delay type)pbdoc")
        .value("Immediate", Delay::Immediate)
        .value("Fast", Delay::Fast)
        .value("Medium", Delay::Medium)
        .value("Slow", Delay::Slow)
        .value("VerySlow", Delay::VerySlow)
        .export_values();

    py::enum_<PlasticityFlags>(m, "PlasticityFlags", R"pbdoc(Plasticity flags)pbdoc")
        .value("LTP", PlasticityFlags::LTP)
        .value("LTD", PlasticityFlags::LTD)
        .value("STDP", PlasticityFlags::STDP)
        .value("Hebbian", PlasticityFlags::Hebbian)
        .value("Structural", PlasticityFlags::Structural)
        .export_values();

    // Error examples for testing
    m.def("testErrorHandling", []() {
        throw NLMError("Test error for Python exception handling");
    }, "Test error handling");

    m.def("testConfigurationError", []() {
        throw ConfigurationError("Test configuration error");
    }, "Test configuration error handling");

    m.def("testMemoryError", []() {
        throw MemoryError("Test memory system error");
    }, "Test memory error handling");

    m.def("testPredictionError", []() {
        throw PredictionError("Test prediction system error");
    }, "Test prediction error handling");

    // Export version
    m.attr("__version__") = "2.0.0-enhanced";
    m.attr("__api_version__") = "2.0.0";
}

} // namespace nlm