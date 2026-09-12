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

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        A configuration manager for NLM (Neural Learning Machine) that handles
        neural network parameters, simulation settings, and behavior rules.
        Manages key-value pairs of configuration data with JSON file support.
        )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file\n"
             "Args:\n"
             "    filepath: Path to the JSON configuration file\n"
             "Returns:\n"
             "    True if configuration was loaded successfully, False otherwise")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments\n"
           "Args:\n"
           "    argc: Number of command line arguments\n"
           "    argv: Array of command line argument strings\n"
           "Returns:\n"
           "    True if configuration was loaded successfully, False otherwise")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file\n"
             "Args:\n"
             "    filepath: Path where the JSON configuration will be saved")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists\n"
             "Args:\n"
             "    key: Configuration key to check\n"
             "Returns:\n"
             "    True if the key exists, False otherwise")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys\n"
             "Returns:\n"
             "    List of all configuration keys")
        .def("clear", &Config::clear,
             "Clear all configuration entries\n"
             "This removes all configuration data from the instance")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration\n"
             "Returns:\n"
             "    Formatted string containing configuration details")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        Abstract base class representing sensory information received from the world.
        Provides interface for accessing different types of sensory data including
        vision, audio, and internal neural signals. Each sensory modality provides
        different kinds of information for the brain to process.
        )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input\n"
             "Returns:\n"
             "    Sensory input type enum value")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector\n"
             "Returns:\n"
             "    Vector of float values representing sensor readings")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data\n"
             "Returns:\n"
             "    Number of dimensions in the sensory data")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp of when this sensory input was received\n"
             "Returns:\n"
             "    Timestamp (seconds since simulation start)")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory input\n"
             "Args:\n"
             "    timestamp: Timestamp to set (seconds since simulation start)");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        Visual sensory input representing camera-like observations from the agent's perspective.
        Contains 2D image data with RGB color channels (or grayscale if single channel).
        Used for object recognition, navigation, and visual scene understanding.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create vision sensory input with specified dimensions\n"
             "Args:\n"
             "    width: Image width in pixels\n"
             "    height: Image height in pixels\n"
             "    channels: Number of color channels (1 for grayscale, 3 for RGB)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the raw image data\n"
             "Args:\n"
             "    data: Flattened array of pixel values (width * height * channels)")
        .def("getWidth", &Vision::getWidth,
             "Get image width in pixels\n"
             "Returns:\n"
             "    Image width")
        .def("getHeight", &Vision::getHeight,
             "Get image height in pixels\n"
             "Returns:\n"
             "    Image height")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels\n"
             "Returns:\n"
             "    Number of color channels (1 or 3)");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        Audio sensory input representing sound data from the environment.
        Contains acoustic signal data sampled at a specific rate, useful for
        sound source localization, speech recognition, and environmental awareness.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create audio sensory input with specified parameters\n"
             "Args:\n"
             "    sampleRate: Audio sampling rate in Hz (e.g., 44100 for CD quality)\n"
             "    numSamples: Number of audio samples to store")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set the raw audio data\n"
             "Args:\n"
             "    data: Array of audio samples (numSamples elements)")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sampling rate\n"
             "Args:\n"
             "    sampleRate: New sampling rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the audio sampling rate\n"
             "Returns:\n"
             "    Sampling rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples\n"
             "Returns:\n"
             "    Number of audio samples stored");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        Internal neural signals representing the brain's own activity and state.
        Contains values like firing rates, membrane potentials, and other
        physiological measurements from neurons within the brain itself.
        Used for proprioception and internal monitoring.
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a neural signal value\n"
             "Args:\n"
             "    value: Neural signal value to add")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals\n"
             "This removes all neural signal data from the internal signals buffer");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        Motor action command that an agent can execute in the environment.
        Represents the lowest-level motor commands that can be sent to the world
        simulation, including movements, interactions, and other agent behaviors.
        Actions can have parameters for fine-grained control (e.g., rotation angles).
        )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Create action with just a type\n"
             "Args:\n"
             "    type: Action type for the motor command")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Create action with type and parameters\n"
             "Args:\n"
             "    type: Action type for the motor command\n"
             "    parameters: Vector of float parameters for the action")
        .def("getType", &Action::getType,
             "Get the action type\n"
             "Returns:\n"
             "    ActionType enum value")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type\n"
             "Args:\n"
             "    type: New action type")
        .def("getParameters", &Action::getParameters,
             "Get the action parameters\n"
             "Returns:\n"
             "    Vector of float parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters\n"
             "Args:\n"
             "    params: New parameters vector")
        .def("getName", &Action::getName,
             "Get human-readable name of the action\n"
             "Returns:\n"
             "    String name of the action")
        .def("clone", &Action::clone,
             "Create a copy of this action\n"
             "Returns:\n"
             "    New Action object identical to this one");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        Physical object in the 2D simulation world with position, type, and properties.
        Represents entities that can interact with the agent, including resources,
        hazards, walls, markers, or empty spaces. Objects have collision detection
        and can have associated values (e.g., resource quantity, hazard damage).
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object with position and properties\n"
             "Args:\n"
             "    x: X-coordinate position\n"
             "    y: Y-coordinate position\n"
             "    type: Type of object (Empty, Resource, Hazard, Wall, Marker)\n"
             "    value: Object value (e.g., resource quantity or hazard damage)\n"
             "    radius: Collision radius (default: 0.5)")
        .def_readwrite("x", &WorldObject::x,
                       "X-coordinate position of the object")
        .def_readwrite("y", &WorldObject::y,
                       "Y-coordinate position of the object")
        .def_readwrite("radius", &WorldObject::radius,
                       "Collision radius of the object")
        .def_readwrite("type", &WorldObject::type,
                       "Type of the object (Empty, Resource, Hazard, Wall, Marker)")
        .def_readwrite("value", &WorldObject::value,
                       "Numeric value associated with the object")
        .def_readwrite("active", &WorldObject::active,
                       "Whether the object is currently active in the simulation");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        Physical state representation of the agent in the 2D world simulation.
        Contains position, orientation, movement parameters, energy, health, and
        age information. Tracks kinematic properties and agent condition for
        interaction with the environment and world dynamics.
        )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
                       "X-coordinate position")
        .def_readwrite("y", &AgentBody::y,
                       "Y-coordinate position")
        .def_readwrite("orientation", &AgentBody::orientation,
                       "Orientation angle in degrees (0 = east, 90 = north)")
        .def_readwrite("velocityX", &AgentBody::velocityX,
                       "X-component of linear velocity")
        .def_readwrite("velocityY", &AgentBody::velocityY,
                       "Y-component of linear velocity")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
                       "Angular velocity (radians per timestep)")
        .def_readwrite("energy", &AgentBody::energy,
                       "Current energy level (0.0 to 1.0)")
        .def_readwrite("health", &AgentBody::health,
                       "Current health level (0.0 to 1.0)")
        .def_readwrite("age", &AgentBody::age,
                       "Age in simulation timesteps")
        .def_readwrite("isMoving", &AgentBody::isMoving,
                       "Whether the agent is currently moving")
        .def_readwrite("isTurning", &AgentBody::isTurning,
                       "Whether the agent is currently turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
                       "Time since last action (timesteps)")
        .def("reset", &AgentBody::reset,
             "Reset agent body state to initial conditions\n"
             "Resets position, velocity, orientation, energy, health, and age to default values");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        Result of executing a motor action in the world simulation.
        Contains reward signal, success status, and descriptive message about
        what happened during the action execution. Used by the agent brain to
        learn from outcomes and update its internal models.
        )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result\n"
             "Args:\n"
             "    reward: Reward signal from the action (positive for good outcomes)\n"
             "    success: Whether the action succeeded\n"
             "    message: Optional human-readable description of the outcome")
        .def_readwrite("reward", &ActionResult::reward,
                       "Reward value received from executing this action")
        .def_readwrite("success", &ActionResult::success,
                       "Whether the action was successful")
        .def_readwrite("message", &ActionResult::message,
                       "Human-readable message describing the action result");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        Container for all sensory information received by the agent in a timestep.
        Aggregates vision, audio, touch, internal signals, and proprioception
        into a single percept object that can be passed to the agent brain.
        Provides access to all sensory modalities in a unified interface.
        )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get vision sensory data\n"
             "Returns:\n"
             "    Vision object containing visual observations")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision sensory data\n"
             "Args:\n"
             "    vision: Vision object to set")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision image width\n"
             "Returns:\n"
             "    Width in pixels")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision image height\n"
             "Returns:\n"
             "    Height in pixels")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get touch/internal signals data\n"
             "Returns:\n"
             "    InternalSignals object containing touch data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch/internal signals data\n"
             "Args:\n"
             "    touch: InternalSignals object to set")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get internal neural signals\n"
             "Returns:\n"
             "    InternalSignals object containing internal signals")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal neural signals\n"
             "Args:\n"
             "    internal: InternalSignals object to set")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get proprioception data\n"
             "Returns:\n"
             "    InternalSignals object containing proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data\n"
             "Args:\n"
             "    proprioception: InternalSignals object to set")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get audio sensory data\n"
             "Returns:\n"
             "    Audio object containing sound data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio sensory data\n"
             "Args:\n"
             "    audio: Audio object to set")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as a list\n"
             "Returns:\n"
             "    Vector of all sensory signals")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get timestamp of this percept\n"
             "Returns:\n"
             "    Simulation timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp of this percept\n"
             "Args:\n"
             "    timestamp: Simulation timestamp to set");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        Discrete-time 2D simulation environment where agents interact with the world
        and each other. Manages world state including objects, physics simulation,
        and agent sensing. Provides methods for world configuration, object
        management, and state querying for the agent brain.
        )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent sensing parameters\n"
             "Args:\n"
             "    width: World width in simulation units\n"
             "    height: World height in simulation units\n"
             "    visionWidth: Width of agent's vision field in pixels\n"
             "    visionHeight: Height of agent's vision field in pixels")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state\n"
             "Clears all objects, resets simulation time, and reinitializes agent")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set the agent's starting position\n"
             "Args:\n"
             "    x: Starting X-coordinate\n"
             "    y: Starting Y-coordinate")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Advance world simulation by one timestep\n"
             "Args:\n"
             "    timestep: Simulation timestep duration")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command from the agent to the world\n"
             "Args:\n"
             "    cmd: Action to execute\n"
             "    currentTime: Current simulation timestamp")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             "Get the agent's current sensory percept\n"
             "Returns:\n"
             "    SensoryPercept object containing all sensory data")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             "Get the agent's current body state\n"
             "Returns:\n"
             "    AgentBody object representing agent state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object to the simulation\n"
             "Args:\n"
             "    obj: WorldObject to add")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove world object at position\n"
             "Args:\n"
             "    x: X-coordinate of object to remove\n"
             "    y: Y-coordinate of object to remove")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if a position is valid for the agent\n"
             "Args:\n"
             "    x: X-coordinate to check\n"
             "    y: Y-coordinate to check\n"
             "Returns:\n"
             "    True if position is within bounds and not colliding with obstacles")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width\n"
             "Returns:\n"
             "    World width in simulation units")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height\n"
             "Returns:\n"
             "    World height in simulation units")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum agent energy\n"
             "Returns:\n"
             "    Maximum energy level (0.0 to 1.0)")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum agent energy\n"
             "Args:\n"
             "    e: Maximum energy level (0.0 to 1.0)")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate per timestep\n"
             "Returns:\n"
             "    Energy decay rate (0.0 to 1.0)")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate per timestep\n"
             "Args:\n"
             "    r: Energy decay rate (0.0 to 1.0)")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time\n"
             "Returns:\n"
             "    Simulation time in seconds")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations\n"
             "Args:\n"
             "    seed: Random seed value")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get current random seed\n"
             "Returns:\n"
             "    Current random seed value");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        The core neural network simulation engine for NLM. Manages neural regions,
        neurons, and synapses to perform cognitive computations and produce actions
        based on sensory input. Handles neural dynamics, learning, and development.
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a brain instance with configuration\n"
             "Args:\n"
             "    config: Shared pointer to the configuration object")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration and prepare neural regions\n"
             "This method sets up neural structures based on configuration parameters")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step without timestamp\n"
             "Args:\n"
             "    currentStep: Current simulation step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp\n"
             "Args:\n"
             "    currentStep: Current simulation step number\n"
             "    currentTime: Current simulation timestamp (seconds)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain for processing\n"
             "Args:\n"
             "    input: Sensory input data (Vision, Audio, or InternalSignals)")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron\n"
             "Args:\n"
             "    neuron: Target neuron ID\n"
             "    current: Current value to inject (microamps)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type\n"
             "Args:\n"
             "    type: Neuron type to target\n"
             "    current: Current value to inject (microamps)")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity\n"
             "Returns:\n"
             "    Action object representing the generated motor command")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions\n"
             "This clears all neural activity and resets to brain initialization state")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file\n"
             "Args:\n"
             "    filepath: Path where brain state will be saved")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file\n"
             "Args:\n"
             "    filepath: Path to the saved brain state file")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region to the brain\n"
             "Args:\n"
             "    name: Optional name for the new region\n"
             "Returns:\n"
             "    Region ID of the newly created region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID\n"
             "Args:\n"
             "    id: Region ID to retrieve\n"
             "Returns:\n"
             "    Neural region object (raises exception if not found)")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions in the brain\n"
             "Returns:\n"
             "    Number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs\n"
             "Returns:\n"
             "    List of all region IDs")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all neural regions\n"
             "Returns:\n"
             "    List of all region objects")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions\n"
             "Returns:\n"
             "    Total number of neurons")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions\n"
             "Returns:\n"
             "    Total number of synapses")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of neurons currently active\n"
             "Returns:\n"
             "    Number of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of neurons currently firing\n"
             "Returns:\n"
             "    Number of firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons\n"
             "Returns:\n"
             "    Average firing rate (spikes per second)")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio\n"
             "Returns:\n"
             "    E/I ratio (1.0 = balanced, >1.0 = excitation dominant, <1.0 = inhibition dominant)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count across all neurons\n"
             "Returns:\n"
             "    Total number of spikes since brain initialization")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage of the brain\n"
             "Returns:\n"
             "    Current developmental stage enum value")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage of the brain\n"
             "Args:\n"
             "    stage: Developmental stage to set")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration object used by this brain\n"
             "Returns:\n"
             "    Configuration object")
        .def("logStatus", &Brain::logStatus,
             "Log brain status information to the configured log\n"
             "This outputs neural activity statistics and diagnostic information");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        High-level interface that connects a brain instance to a world simulation,
        providing sensory processing, motor command decoding, and behavioral control.
        Manages the interaction between neural computation and environmental interaction,
        including reward modulation, development, and curiosity-driven exploration.
        )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface\n"
             "Args:\n"
             "    brain: Shared pointer to the brain instance to control")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with a world simulation\n"
             "Args:\n"
             "    world: Shared pointer to the world instance")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get the expected size of sensory input data\n"
             "Returns:\n"
             "    Number of sensory input features expected")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get the expected size of motor output data\n"
             "Returns:\n"
             "    Number of motor output features expected")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             "Process sensory percept and inject into brain\n"
             "Args:\n"
             "    percept: SensoryPercept object containing all sensory modalities")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command\n"
             "Returns:\n"
             "    Action object representing the decoded motor command")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation to influence learning\n"
             "Args:\n"
             "    reward: Actual reward received from environment\n"
             "    predictedReward: Expected reward predicted by brain")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             "Update developmental system state\n"
             "Args:\n"
             "    timestep: Current simulation timestep")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage\n"
             "Returns:\n"
             "    Current developmental stage enum value")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level\n"
             "Returns:\n"
             "    Neuromodulation strength (0.0 to 1.0)")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level for exploration\n"
             "Returns:\n"
             "    Curiosity level (0.0 to 1.0)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level for surprise detection\n"
             "Returns:\n"
             "    Novelty level (0.0 to 1.0)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error for learning\n"
             "Returns:\n"
             "    Prediction error magnitude")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode\n"
             "This resets all agent state including neuromodulation, curiosity, and development")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain instance\n"
             "Returns:\n"
             "    Shared pointer to the brain instance")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation\n"
             "Args:\n"
             "    enable: True to enable, False to disable")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity\n"
             "Args:\n"
             "    enable: True to enable, False to disable")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable developmental processes\n"
             "Args:\n"
             "    enable: True to enable, False to disable")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration\n"
             "Args:\n"
             "    enable: True to enable, False to disable")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled\n"
             "Returns:\n"
             "    True if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled\n"
             "Returns:\n"
             "    True if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled\n"
             "Returns:\n"
             "    True if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled\n"
             "Returns:\n"
             "    True if curiosity is enabled");

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
}

} // namespace nlm
