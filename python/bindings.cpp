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

    // Register all relevant standard exception types
    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");
    py::register_exception<std::domain_error>(m, "ValueError");
    py::register_exception<std::length_error>(m, "ValueError");
    py::register_exception<std::range_error>(m, "ValueError");
    py::register_exception<std::overflow_error>(m, "ValueError");

    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(Unique identifier for a neuron)pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("get_index", &NeuronId::index)
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
        .def("get_index", &SynapseId::index)
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
        .def("get_index", &RegionId::index)
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
        .def("get_index", &PopulationId::index)
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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system

    Provides access to NLM configuration parameters including neuron counts,
    connection probabilities, plasticity weights, and simulation settings.
    Configuration can be loaded from files or command line arguments,
    and saved back to files.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON or key=value file.
             
             Args:
                 filepath: Path to the configuration file
                 
             Returns:
                 True if successful, False on file error
                 
             Raises:
                 RuntimeError: If file cannot be read or parsed
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command line arguments.
           
           Supports --key=value format and -key value format.
           
           Args:
               argc: Argument count
               argv: Argument array
               
           Returns:
               True if successful
               
           Raises:
               RuntimeError: If parsing fails
             )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a key=value format file.
             
             Args:
                 filepath: Path to save the configuration file
                 
             Returns:
                 True if successful, False on file error
                 
             Raises:
                 RuntimeError: If file cannot be written
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check
                 
             Returns:
                 True if key exists, False otherwise
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 List of all configuration keys
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             Note: This operation cannot be undone.
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 Summary string with key configuration parameters
             )pbdoc")
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
            if (data.size() > 1e9) {  // Sanity check to prevent excessive memory usage
                throw py::value_error("Vision data too large");
            }
            self.setData(data);
        }, py::arg("data"))
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            if (data.size() > 1e9) {  // Sanity check to prevent excessive memory usage
                throw py::value_error("Audio data too large");
            }
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
        .def("setVision", [](SensoryPercept& self, const Vision& vision) {
            // Validate vision object before setting
            if (vision.getWidth() == 0 || vision.getHeight() == 0) {
                throw py::value_error("Invalid vision dimensions");
            }
            self.setVision(vision);
        }, py::arg("vision"))
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation

    Represents a 2D environment where the NLM brain interacts through sensory
    perception and motor actions. The world manages agent state, objects,
    and provides the interface between brain and external environment.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure the world dimensions and sensory capabilities.
             
             Args:
                 width: World width in arbitrary units
                 height: World height in arbitrary units  
                 visionWidth: Width of agent's visual field
                 visionHeight: Height of agent's visual field
                 
             Note: Configure must be called before reset or any operations
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
             Reset the world to initial state.
             
             This clears all objects, resets agent position to start location,
             and resets simulation time to zero.
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set the agent's starting position.
             
             Args:
                 x: Starting x-coordinate
                 y: Starting y-coordinate
                 
             Note: Position is relative to world coordinate system
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
             Update world state for one timestep.
             
             Args:
                 timestep: Time increment in seconds
                 
             Note: Updates physics, decay processes, and world dynamics
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply a motor command from the agent to the world.
             
             Args:
                 cmd: Motor command (Action object)
                 currentTime: Current simulation time
                 
             Returns:
                 ActionResult with reward and success status
                 
             Raises:
                 ValueError: If command is invalid
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             R"pbdoc(
             Get the agent's current sensory perception.
             
             Returns:
                 SensoryPercept object containing vision, touch, internal, and audio data
                 
             Note: Perception is updated during world::update()
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             R"pbdoc(
             Get the agent's current body state.
             
             Returns:
                 AgentBody object with position, velocity, and physiological state
                 
             Note: Updated continuously during simulation
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add an object to the world.
             
             Args:
                 obj: WorldObject to add
                 
             Raises:
                 ValueError: If object coordinates are invalid
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove an object at specific coordinates.
             
             Args:
                 x: X-coordinate
                 y: Y-coordinate
                 
             Note: Removes first object found at those coordinates
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if a position is valid for object placement.
             
             Args:
                 x: X-coordinate
                 y: Y-coordinate
                 
             Returns:
                 True if position is valid, False otherwise
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
             Get world width.
             
             Returns:
                 World width in arbitrary units
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
             Get world height.
             
             Returns:
                 World height in arbitrary units
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
             Get maximum agent energy.
             
             Returns:
                 Maximum energy value
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
             Set maximum agent energy.
             
             Args:
                 e: New maximum energy value
                 
             Note: Affects agent survival and stamina mechanics
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
             Get agent energy decay rate.
             
             Returns:
                 Energy decay rate per second
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
             Set agent energy decay rate.
             
             Args:
                 r: Energy decay rate per second
                 
             Note: Higher values cause faster energy depletion
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
             Get current simulation time.
             
             Returns:
                 Current simulation time in seconds
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set random seed for reproducible world behavior.
             
             Args:
                 seed: Random seed value
                 
             Note: Ensures reproducible world behavior across runs
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
             Get current random seed.
             
             Returns:
                 Current random seed value
             )pbdoc");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class

    Implements a complete artificial brain with spiking neurons, integrated memory
    systems, neuromodulation, prediction, and cognition. The brain processes
    sensory input, produces motor actions, and learns from experience through
    plasticity mechanisms.

    Key Features:
    - Real spiking neural computation with LIF neuron dynamics
    - Event-driven spike propagation
    - Integrated memory systems (working, episodic, associative)
    - Neuromodulation (dopamine, curiosity, novelty detection)
    - Prediction and cognitive planning
    - Developmental stages affecting plasticity
    - Checkpoint save/load for persistence
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Create a new brain with configuration.
             
             Args:
                 config: Configuration object specifying brain parameters
                 
             Raises:
                 ValueError: If config is invalid
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration parameters.
             
             This method sets up neural populations, connections, and internal
             systems based on the configuration.
             
             Returns:
                 True if initialization succeeded
                 
             Raises:
                 RuntimeError: If initialization fails due to invalid parameters
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a single simulation step.
             
             Args:
                 currentStep: Current simulation step number
                 
             Note: This performs one timestep of neural computation
             "pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a single simulation step with timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
                 
             Note: This performs one timestep of neural computation
             "pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             Args:
                 input: Sensory input data (Vision, Audio, or InternalSignals)
                 
             Raises:
                 ValueError: If input is None or invalid
                 RuntimeError: If injection fails
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current directly into a specific neuron.
             
             Args:
                 neuron: Target neuron ID
                 current: Current to inject (positive for depolarization)
                 
             Raises:
                 ValueError: If neuron ID is invalid
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type to target
                 current: Current to inject
                 
             Raises:
                 ValueError: If neuron type is invalid
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor/action output based on neural activity.
             
             Returns:
                 Action object representing the motor command
                 
             Note: Action is generated from motor neuron activity patterns
             "pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state to initial conditions.
             
             This clears all neural activity, memory contents, and internal
             state, effectively restarting the simulation.
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to a file for persistence.
             
             Args:
                 filepath: Path to save the brain state
                 
             Returns:
                 True if save succeeded
                 
             Raises:
                 RuntimeError: If save fails due to I/O error
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from a saved file.
             
             Args:
                 filepath: Path to load the brain state from
                 
             Returns:
                 True if load succeeded
                 
             Raises:
                 RuntimeError: If load fails due to file error or corruption
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region to the brain.
             
             Args:
                 name: Optional name for the region (for debugging)
                 
             Returns:
                 Region ID of the newly created region
                 
             Note: New regions start empty and can have neurons added later
             "pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             R"pbdoc(
             Get a region by its ID.
             
             Args:
                 id: Region ID to retrieve
                 
             Returns:
                 Region object
                 
             Raises:
                 IndexError: If region ID does not exist
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions in the brain.
             
             Returns:
                 Number of regions
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs.
             
             Returns:
                 List of all region IDs
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             R"pbdoc(
             Get all regions in the brain.
             
             Returns:
                 List of all region objects
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all regions.
             
             Returns:
                 Total number of neurons in the brain
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count across all regions.
             
             Returns:
                 Total number of synapses in the brain
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get count of active neurons.
             
             Returns:
                 Number of neurons currently firing
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons.
             
             Returns:
                 Number of neurons that spiked in the last step
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons.
             
             Returns:
                 Average firing rate in Hz
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio.
             
             Returns:
                 Ratio of excitatory to inhibitory activity (higher = more excitation)
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count since brain initialization.
             
             Returns:
                 Total number of spikes generated
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 Current developmental stage enum value
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
             Set developmental stage.
             
             Args:
                 stage: New developmental stage
                 
             Note: Developmental stage affects plasticity rates and learning capabilities
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             R"pbdoc(
             Get the configuration object.
             
             Returns:
                 Configuration object
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log current brain status to the log system.
             
             Useful for debugging and monitoring brain state.
             )pbdoc");

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
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config"), "Create and initialize a new brain with configuration");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, "Create a new simple world");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), "Create a new agent brain interface");

    m.def("create_preset_config", [](const std::string& preset) {
        auto config = std::make_shared<Config>();
        if (preset == "minimal") {
            config->set("neuron_count", 100);
            config->set("region_count", 1);
            config->set("random_seed", 42);
        } else if (preset == "standard") {
            config->set("neuron_count", 1000);
            config->set("region_count", 1);
            config->set("random_seed", 42);
            config->set("connection_probability", 0.1f);
        } else if (preset == "complex") {
            config->set("neuron_count", 10000);
            config->set("region_count", 4);
            config->set("random_seed", 42);
            config->set("connection_probability", 0.2f);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
        } else {
            throw py::value_error("Unknown preset configuration: " + preset);
        }
        return config;
    }, py::arg("preset"),
       R"pbdoc(
       Create a preset configuration for quick setup.
       
       Provides pre-configured brain settings for different complexity levels.
       
       Args:
           preset: Configuration preset name ("minimal", "standard", or "complex")
           
       Returns:
           Configuration object with preset settings
           
       Raises:
           ValueError: If preset name is unknown
           
       Examples:
           >>> import pynlm
           >>> config = pynlm.create_preset_config("minimal")
           >>> config = pynlm.create_preset_config("standard") 
           >>> config = pynlm.create_preset_config("complex")
       )pbdoc");

    // Type conversion helpers
    m.def("to_vision", [](py::list width, py::list height, py::list channels = py::list()) {
        size_t w = width.empty() ? 0 : py::cast<size_t>(width[0]);
        size_t h = height.empty() ? 0 : py::cast<size_t>(height[0]);
        size_t c = channels.empty() ? 3 : py::cast<size_t>(channels[0]);
        return Vision(w, h, c);
    }, R"pbdoc(Helper to create Vision from Python lists or scalars)pbdoc");

    m.def("to_audio", [](py::list sampleRate, py::list numSamples) {
        size_t sr = sampleRate.empty() ? 0 : py::cast<size_t>(sampleRate[0]);
        size_t ns = numSamples.empty() ? 0 : py::cast<size_t>(numSamples[0]);
        return Audio(sr, ns);
    }, R"pbdoc(Helper to create Audio from Python lists or scalars)pbdoc");

    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
