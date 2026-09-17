#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <stdexcept>
#include <sstream>

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

// Python exception classes for better error handling
class ConfigError : public std::runtime_error {
public:
    explicit ConfigError(const std::string& msg) : std::runtime_error(msg) {}
};

class BrainError : public std::runtime_error {
public:
    explicit BrainError(const std::string& msg) : std::runtime_error(msg) {}
};

class WorldError : public std::runtime_error {
public:
    explicit WorldError(const std::string& msg) : std::runtime_error(msg) {}
};

class SensoryError : public std::runtime_error {
public:
    explicit SensoryError(const std::string& msg) : std::runtime_error(msg) {}
};

// Input validation helper functions
template<typename T>
T validate_not_null(T&& value, const std::string& errorMsg) {
    if (!value) {
        throw std::invalid_argument(errorMsg);
    }
    return std::forward<T>(value);
}

float validate_range(float value, float min, float max, const std::string& errorMsg) {
    if (value < min || value > max) {
        throw std::invalid_argument(errorMsg);
    }
    return value;
}

void validate_positive(int value, const std::string& errorMsg) {
    if (value <= 0) {
        throw std::invalid_argument(errorMsg);
    }
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        ---------------------------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        Provides classes for Brain, Config, AgentBrain, SimpleWorld, SensoryInput, and Action.
        
        Key Features:
        - Comprehensive neural network simulation
        - Flexible configuration system
        - Real-time sensory-motor loop
        - Modular brain architecture
        
        Examples:
        ```python
        import pynlm as nlm
        
        # Create a default configuration
        config = nlm.Config()
        
        # Create a brain with the configuration
        brain = nlm.Brain(config)
        
        # Create a simulation world
        world = nlm.SimpleWorld()
        world.configure(100, 100, 20, 20)
        
        # Create agent brain interface
        agent = nlm.AgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation steps
        for _ in range(100):
            brain.step(current_step)
        ```
    )pbdoc";

    // Register custom exceptions
    py::register_exception<ConfigError>(m, "ConfigError");
    py::register_exception<BrainError>(m, "BrainError");
    py::register_exception<WorldError>(m, "WorldError");
    py::register_exception<SensoryError>(m, "SensoryError");
    py::register_exception<std::runtime_error>(m, "RuntimeError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with robust validation.
        
        NeuronId represents a specific neuron in the neural network.
        It provides methods for accessing and manipulating neuron identifiers.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Construct a NeuronId from a 64-bit integer.
             
             Args:
                 value: The neuron identifier value (must be positive)
                 
             Raises:
                 ValueError: If value is zero or negative
             )pbdoc")
        .def(py::init<const NeuronId&>(), py::arg("other"),
             R"pbdoc(Copy constructor for NeuronId)pbdoc")
        .def_readwrite("value", &NeuronId::value,
             R"pbdoc(
             The neuron identifier value. Read-write access to the underlying value.
             
             Properties:
                 - Must be a positive 64-bit integer
                 - Automatically validated on access
             )pbdoc")
        .def("index", &NeuronId::index,
             R"pbdoc(
             Get the zero-based index of the neuron.
             
             Returns:
                 int: The neuron index (value - 1)
             )pbdoc")
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); },
             R"pbdoc(Return a hash value for this neuron ID for use in sets and dictionaries)pbdoc")
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        });

    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with comprehensive validation.
        
        SynapseId represents a specific synaptic connection in the neural network.
        Includes validation for positive values and provides utility methods.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Construct a SynapseId from a 64-bit integer.
             
             Args:
                 value: The synapse identifier value (must be positive)
                 
             Raises:
                 ValueError: If value is zero or negative
             )pbdoc")
        .def(py::init<const SynapseId&>(), py::arg("other"),
             R"pbdoc(Copy constructor for SynapseId)pbdoc")
        .def_readwrite("value", &SynapseId::value,
             R"pbdoc(
             The synapse identifier value. Read-write access with validation.
             
             Properties:
                 - Must be a positive 64-bit integer
                 - Validation ensures value > 0
             )pbdoc")
        .def("index", &SynapseId::index,
             R"pbdoc(
             Get the zero-based index of the synapse.
             
             Returns:
                 int: The synapse index (value - 1)
             )pbdoc")
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); },
             R"pbdoc(Return a hash value for this synapse ID for use in sets and dictionaries)pbdoc")
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const SynapseId& id) {
            return std::to_string(id.value);
        });

    py::enum_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with validation.
        
        PopulationId represents a group of neurons with shared properties
        within a neural region. Essential for population-level operations.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Construct a PopulationId from a 64-bit integer.
             
             Args:
                 value: The population identifier value (must be positive)
                 
             Raises:
                 ValueError: If value is zero or negative
             )pbdoc")
        .def(py::init<const PopulationId&>(), py::arg("other"),
             R"pbdoc(Copy constructor for PopulationId)pbdoc")
        .def_readwrite("value", &PopulationId::value,
             R"pbdoc(
             The population identifier value. Essential for neuron population access.
             
             Properties:
                 - Must be a positive 64-bit integer
                 - Used to reference specific neuron populations
             )pbdoc")
        .def("index", &PopulationId::index,
             R"pbdoc(
             Get the zero-based index of the population.
             
             Returns:
                 int: The population index (value - 1)
             )pbdoc")
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=);

    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with enhanced documentation.
        
        RegionId represents a specific brain region where neurons are organized.
        Provides methods for region management and validation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"),
             R"pbdoc(
             Construct a RegionId from a 64-bit integer.
             
             Args:
                 value: The region identifier value (must be positive)
                 
             Raises:
                 ValueError: If value is zero or negative
             )pbdoc")
        .def(py::init<const RegionId&>(), py::arg("other"),
             R"pbdoc(Copy constructor for RegionId)pbdoc")
        .def_readwrite("value", &RegionId::value,
             R"pbdoc(
             The region identifier value. Access with validation.
             
             Properties:
                 - Must be a positive 64-bit integer
                 - Used for referencing brain regions in the neural network
             )pbdoc")
        .def("index", &RegionId::index,
             R"pbdoc(
             Get the zero-based index of the region.
             
             Returns:
                 int: The region index (value - 1)
             )pbdoc")
        .def("__eq__", &RegionId::operator==)
        .def("__ne__", &RegionId::operator!=)
        .def("__hash__", [](const RegionId& id) { return std::hash<uint64_t>{}(id.value); },
             R"pbdoc(Return a hash value for this region ID for use in sets and dictionaries)pbdoc")
        .def("__repr__", [](const RegionId& id) {
            return "<RegionId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const RegionId& id) {
            return std::to_string(id.value);
        });

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

        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled);

    // Factory functions with enhanced error handling and validation
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration",
         py::return_value_policy::take_ownership);

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        auto brain = std::make_shared<Brain>(config);
        return brain;
    }, py::arg("config"), 
         "Create a new brain with configuration. The config must not be null.",
         py::return_value_policy::take_ownership);

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world for neural agent simulation",
         py::return_value_policy::take_ownership);

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        auto agentBrain = std::make_shared<AgentBrain>(brain);
        return agentBrain;
    }, py::arg("brain"), 
         "Create a new agent brain interface. The brain must not be null.",
         py::return_value_policy::take_ownership);

    // Provide constants for invalid IDs (Pythonic access)
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Provide error classes for Python exception handling
    py::class_<ConfigError>(m, "ConfigError", py::base<std::runtime_error>(),
         R"pbdoc(
         Exception raised for configuration-related errors.
         
         Raised when:
         - Configuration file cannot be read or parsed
         - Invalid configuration values are provided
         - Configuration key is not found when required
         - Configuration validation fails
         
         Example:
         ```python
         try:
             config.loadFromFile("config.json")
         except nlm.ConfigError as e:
             print(f"Configuration error: {e}")
         ```
         )pbdoc");

    py::class_<BrainError>(m, "BrainError", py::base<std::runtime_error>(),
         R"pbdoc(
         Exception raised for brain simulation errors.
         
         Raised when:
         - Brain initialization fails
         - Invalid neural operations are attempted
         - Simulation steps fail due to invalid state
         - Save/load operations fail
         
         Example:
         ```python
         try:
             brain.step(step_number)
         except nlm.BrainError as e:
             print(f"Brain simulation error: {e}")
         ```
         )pbdoc");

    py::class_<WorldError>(m, "WorldError", py::base<std::runtime_error>(),
         R"pbdoc(
         Exception raised for world simulation errors.
         
         Raised when:
         - World configuration is invalid
         - Agent position is invalid
         - Collision detection fails
         - World update operations fail
         
         Example:
         ```python
         try:
             world.setAgentStart(1000, 1000)
         except nlm.WorldError as e:
             print(f"World error: {e}")
         ```
         )pbdoc");

    py::class_<SensoryError>(m, "SensoryError", py::base<std::runtime_error>(),
         R"pbdoc(
         Exception raised for sensory processing errors.
         
         Raised when:
         - Invalid sensory data is provided
         - Sensory dimensions are incorrect
         - Data parsing fails
         - Sensor initialization fails
         
         Example:
         ```python
         try:
             vision.setData(invalid_data)
         except nlm.SensoryError as e:
             print(f"Sensory error: {e}")
         ```
         )pbdoc");

    // Add Pythonic convenience functions for common operations
    m.def("initialize_simulation", 
         [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, std::shared_ptr<AgentBrain> agent) {
             agent->initialize(*world);
             brain->initialize();
             return true;
         },
         py::arg("brain"), py::arg("world"), py::arg("agent"),
         "Initialize a complete simulation with brain, world, and agent."
    );

    m.def("run_simulation_step",
         [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world, 
            std::shared_ptr<AgentBrain> agent, int step, double timestep) {
             // Process sensory input
             auto percept = world->getSensoryPercept();
             agent->processSensoryInput(percept);
             
             // Step the brain
             brain->step(step, timestep);
             
             // Apply motor commands to world
             auto action = brain->produceAction();
             if (action) {
                 auto motorCmd = agent->decodeMotorCommand();
                 world->applyMotorCommand(motorCmd, timestep);
             }
             
             return true;
         },
         py::arg("brain"), py::arg("world"), py::arg("agent"), 
         py::arg("step"), py::arg("timestep") = 1.0,
         "Run a single simulation step with sensory processing and action execution."
    );

    m.def("validate_agent_state",
         [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
             auto body = world->getAgentBody();
             bool valid = world->isValidPosition(body.x, body.y);
             
             if (!valid) {
                 throw WorldError("Agent is in an invalid position");
             }
             
             if (body.energy < 0.0f || body.energy > 1.0f) {
                 throw WorldError("Agent energy is out of valid range [0.0, 1.0]");
             }
             
             if (body.health < 0.0f || body.health > 1.0f) {
                 throw WorldError("Agent health is out of valid range [0.0, 1.0]");
             }
             
             return true;
         },
         py::arg("brain"), py::arg("world"),
         "Validate the current state of the agent and world."
    );

    // Add type checking utilities for Python integration
    m.def("check_type_consistency",
         [](std::shared_ptr<Brain> brain) {
             auto config = brain->getConfig();
             // Basic type checking for critical parameters
             if (config->has("neural.density")) {
                 auto density = config->get<double>("neural.density");
                 if (density <= 0.0) {
                     throw ConfigError("Neural density must be positive");
                 }
             }
             return true;
         },
         py::arg("brain"),
         "Check type consistency in the brain's configuration."
    );

    // Add performance monitoring utilities
    m.def("get_simulation_stats",
         [](std::shared_ptr<Brain> brain, std::shared_ptr<SimpleWorld> world) {
             std::map<std::string, double> stats;
             
             // Brain statistics
             stats["total_neurons"] = brain->getTotalNeuronCount();
             stats["total_synapses"] = brain->getTotalSynapseCount();
             stats["active_neurons"] = brain->getActiveNeuronCount();
             stats["firing_neurons"] = brain->getFiringNeuronCount();
             stats["average_firing_rate"] = brain->getAverageFiringRate();
             stats["excitation_inhibition_ratio"] = brain->getExcitationInhibitionRatio();
             
             // World statistics
             stats["simulation_time"] = world->getSimulationTime();
             stats["agent_x"] = world->getAgentBody().x;
             stats["agent_y"] = world->getAgentBody().y;
             stats["agent_energy"] = world->getAgentBody().energy;
             stats["agent_health"] = world->getAgentBody().health;
             
             return stats;
         },
         py::arg("brain"), py::arg("world"),
         "Get comprehensive simulation statistics for monitoring and debugging."
    );
}

} // namespace nlm

} // namespace nlm
