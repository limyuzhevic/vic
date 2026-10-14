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

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(Neuron type enumeration)
        
        Defines the different types of neurons in the NLM system,
        each with specific functions and properties.
        "pbdoc")
        .value("Excitatory", NeuronType::Excitatory, R"pbdoc(Excitatory neurons - promote firing in other neurons)pbdoc")
        .value("Inhibitory", NeuronType::Inhibitory, R"pbdoc(Inhibitory neurons - suppress firing in other neurons)pbdoc")
        .value("Modulatory", NeuronType::Modulatory, R"pbdoc(Modulatory neurons - affect plasticity and neuromodulation)pbdoc")
        .value("Sensory", NeuronType::Sensory, R"pbdoc(Sensory neurons - receive external input from environment)pbdoc")
        .value("Motor", NeuronType::Motor, R"pbdoc(Motor neurons - produce actions based on brain activity)pbdoc")
        .value("Internal", NeuronType::Internal, R"pbdoc(Internal neurons - internal brain processing)pbdoc")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
        
        Defines the different types of synaptic connections between neurons,
        each with distinct functional properties.
        "pbdoc")
        .value("Excitatory", SynapseType::Excitatory, R"pbdoc(Excitatory synapses - increase postsynaptic neuron firing)pbdoc")
        .value("Inhibitory", SynapseType::Inhibitory, R"pbdoc(Inhibitory synapses - decrease postsynaptic neuron firing)pbdoc")
        .value("Modulatory", SynapseType::Modulatory, R"pbdoc(Modulatory synapses - affect plasticity and neuromodulation)pbdoc")
        .value("Electrical", SynapseType::Electrical, R"pbdoc(Electrical synapses - direct current flow between neurons)pbdoc")
        .value("GapJunction", SynapseType::GapJunction, R"pbdoc(Gap junction synapses - electrical coupling between neurons)pbdoc")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
        
        Represents different stages of neural development, each with distinct
        plasticity characteristics and learning capabilities.
        "pbdoc")
        .value("Initial", DevelopmentalStage::Initial, R"pbdoc(Initial stage - high plasticity, rapid learning)pbdoc")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, R"pbdoc(Critical period - enhanced learning sensitivity)pbdoc")
        .value("Maturation", DevelopmentalStage::Maturation, R"pbdoc(Maturation - balanced development and learning)pbdoc")
        .value("Adult", DevelopmentalStage::Adult, R"pbdoc(Adult stage - stable, refined neural connections)pbdoc")
        .value("Aging", DevelopmentalStage::Aging, R"pbdoc(Aging stage - declining plasticity with accumulated experience)pbdoc")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
        
        Represents the current firing state of a neuron during simulation,
        indicating whether the neuron is active, resting, or in a special state.
        "pbdoc")
        .value("Resting", FiringState::Resting, R"pbdoc(Neuron is at rest, not firing)pbdoc")
        .value("Active", FiringState::Active, R"pbdoc(Neuron is currently firing)pbdoc")
        .value("Refractory", FiringState::Refractory, R"pbdoc(Neuron is in refractory period after firing)pbdoc")
        .value("Inhibited", FiringState::Inhibited, R"pbdoc(Neuron is inhibited by inhibitory input)pbdoc")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
        
        Defines the types of actions that can be performed by the agent.
        These represent the basic building blocks of behavior.
        "pbdoc")
        .value("MoveForward", ActionType::MoveForward, R"pbdoc(Move forward in the world)pbdoc")
        .value("MoveBackward", ActionType::MoveBackward, R"pbdoc(Move backward in the world)pbdoc")
        .value("MoveLeft", ActionType::MoveLeft, R"pbdoc(Move left in the world)pbdoc")
        .value("MoveRight", ActionType::MoveRight, R"pbdoc(Move right in the world)pbdoc")
        .value("TurnLeft", ActionType::TurnLeft, R"pbdoc(Turn left by rotating)pbdoc")
        .value("TurnRight", ActionType::TurnRight, R"pbdoc(Turn right by rotating)pbdoc")
        .value("Look", ActionType::Look, R"pbdoc(Look in current direction)pbdoc")
        .value("LookUp", ActionType::LookUp, R"pbdoc(Look upward)pbdoc")
        .value("LookDown", ActionType::LookDown, R"pbdoc(Look downward)pbdoc")
        .value("Interact", ActionType::Interact, R"pbdoc(Interact with nearby object)pbdoc")
        .value("Eat", ActionType::Eat, R"pbdoc(Eat food or resource)pbdoc")
        .value("Drink", ActionType::Drink, R"pbdoc(Drink water or resource)pbdoc")
        .value("Rest", ActionType::Rest, R"pbdoc(Enter resting state)pbdoc")
        .value("Wait", ActionType::Wait, R"pbdoc(Wait without doing anything)pbdoc")
        .value("Custom", ActionType::Custom, R"pbdoc(Custom action with additional parameters)pbdoc")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)
        
        Defines the basic motor commands that can be executed by the agent.
        These are the atomic actions that translate neural activity into behavior.
        "pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, R"pbdoc(Forward movement)pbdoc")
        .value("MoveBackward", MotorCommand::MoveBackward, R"pbdoc(Backward movement)pbdoc")
        .value("TurnLeft", MotorCommand::TurnLeft, R"pbdoc(Turn left)pbdoc")
        .value("TurnRight", MotorCommand::TurnRight, R"pbdoc(Turn right)pbdoc")
        .value("LookLeft", MotorCommand::LookLeft, R"pbdoc(Look left)pbdoc")
        .value("LookRight", MotorCommand::LookRight, R"pbdoc(Look right)pbdoc")
        .value("Interact", MotorCommand::Interact, R"pbdoc(Interact with object)pbdoc")
        .value("Wait", MotorCommand::Wait, R"pbdoc(Wait/stay still)pbdoc")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
        
        Defines the types of objects that can exist in the simulated world.
        These determine how objects interact with the agent and environment.
        "pbdoc")
        .value("Empty", WorldObjectType::Empty, R"pbdoc(Empty space/cell)pbdoc")
        .value("Resource", WorldObjectType::Resource, R"pbdoc(Useful resource (e.g., food, water))pbdoc")
        .value("Hazard", WorldObjectType::Hazard, R"pbdoc(Hazardous object (e.g., poison, obstacle))pbdoc")
        .value("Wall", WorldObjectType::Wall, R"pbdoc(Wall/blocking structure)pbdoc")
        .value("Marker", WorldObjectType::Marker, R"pbdoc(Marker/landmark)pbdoc")
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        Contains configuration parameters for the NLM system including brain,
        plasticity, neuromodulation, and simulation settings.
        "pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
              R"pbdoc(Load configuration from a file)pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
              R"pbdoc(Load configuration from command line arguments)pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
              R"pbdoc(Save configuration to a file)pbdoc")
        .def("has", &Config::has, py::arg("key"),
              R"pbdoc(Check if a configuration key exists)pbdoc")
        .def("getKeys", &Config::getKeys,
              R"pbdoc(Get all configuration keys)pbdoc")
        .def("clear", &Config::clear,
              R"pbdoc(Clear all configuration entries)pbdoc")
        .def("summary", &Config::summary,
              R"pbdoc(Get a summary string of the configuration)pbdoc")
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

py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        Vision is a 2D visual input representing what the agent sees.
        Each cell represents a pixel with intensity between 0.0 and 1.0.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
              py::arg("channels") = 3,
              R"pbdoc(Initialize a vision input with specified dimensions
              
              Args:
                  width: Number of pixels horizontally
                  height: Number of pixels vertically  
                  channels: Number of color channels (default: 3)
              )pbdoc")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
              R"pbdoc(Set vision data
              
              Args:
                  data: Vector of pixel values (0.0 to 1.0)
              )pbdoc")
        .def("getWidth", &Vision::getWidth,
              R"pbdoc(Get vision width in pixels)pbdoc")
        .def("getHeight", &Vision::getHeight,
              R"pbdoc(Get vision height in pixels)pbdoc")
        .def("getChannels", &Vision::getChannels,
              R"pbdoc(Get number of color channels)pbdoc")
        .def("__repr__", [](const Vision& v) {
            return "<Vision width=" + std::to_string(v.getWidth()) +
                   " height=" + std::to_string(v.getHeight()) +
                   " channels=" + std::to_string(v.getChannels()) + ">";
        });

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        Audio input representing sound or auditory signals.
        Sample rate and number of samples define the audio characteristics.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
              R"pbdoc(Initialize audio input
              
              Args:
                  sampleRate: Number of samples per second (Hz)
                  numSamples: Total number of audio samples
              )pbdoc")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
              R"pbdoc(Set audio data samples
              
              Args:
                  data: Vector of audio sample values
              )pbdoc")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
              R"pbdoc(Set sample rate in Hz)pbdoc")
        .def("getSampleRate", &Audio::getSampleRate,
              R"pbdoc(Get sample rate in Hz)pbdoc")
        .def("getNumSamples", &Audio::getNumSamples,
              R"pbdoc(Get number of audio samples)pbdoc")
        .def("__repr__", [](const Audio& a) {
            return "<Audio sampleRate=" + std::to_string(a.getSampleRate()) +
                   " samples=" + std::to_string(a.getNumSamples()) + ">";
        });

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
        .def("clearSignals", &InternalSignals::clearSignals);

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        Represents a motor command or action that the brain can produce.
        Actions include movement commands, interactions, and other behaviors.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
              R"pbdoc(Create an action with a specific type
              
              Args:
                  type: The type of action to create
              )pbdoc")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
              R"pbdoc(Create an action with type and parameters
              
              Args:
                  type: The type of action
                  parameters: Additional action parameters (e.g., strength, direction)
              )pbdoc")
        .def("getType", &Action::getType,
              R"pbdoc(Get the action type)pbdoc")
        .def("setType", &Action::setType, py::arg("type"),
              R"pbdoc(Set the action type)pbdoc")
        .def("getParameters", &Action::getParameters,
              R"pbdoc(Get action parameters)pbdoc")
        .def("setParameters", &Action::setParameters, py::arg("params"),
              R"pbdoc(Set action parameters)pbdoc")
        .def("getName", &Action::getName,
              R"pbdoc(Get human-readable name of the action)pbdoc")
        .def("clone", &Action::clone,
              R"pbdoc(Create a copy of this action)pbdoc")
        .def("__repr__", [](const Action& a) {
            return "<Action type=" + std::to_string(static_cast<int>(a.getType())) +
                   " name=\"" + a.getName() + "\">";
        });

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        Represents an object in the simulated world such as resources, hazards, walls, or markers.
        Objects have position, type, and physical properties.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
              py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
              py::arg("radius") = 0.5f,
              R"pbdoc(Create a world object
              
              Args:
                  x: X coordinate in world space
                  y: Y coordinate in world space
                  type: Type of object (Resource, Hazard, Wall, etc.)
                  value: Object value (e.g., energy content)
                  radius: Physical radius of the object
              )pbdoc")
        .def_readwrite("x", &WorldObject::x,
              R"pbdoc(Get/set X coordinate)pbdoc")
        .def_readwrite("y", &WorldObject::y,
              R"pbdoc(Get/set Y coordinate)pbdoc")
        .def_readwrite("radius", &WorldObject::radius,
              R"pbdoc(Get/set object radius)pbdoc")
        .def_readwrite("type", &WorldObject::type,
              R"pbdoc(Get/set object type)pbdoc")
        .def_readwrite("value", &WorldObject::value,
              R"pbdoc(Get/set object value)pbdoc")
        .def_readwrite("active", &WorldObject::active,
              R"pbdoc(Get/set whether object is active)pbdoc")
        .def("__repr__", [](const WorldObject& obj) {
            return "<WorldObject type=" + std::to_string(static_cast<int>(obj.type)) +
                   " x=" + std::to_string(obj.x) +
                   " y=" + std::to_string(obj.y) +
                   " active=" + std::to_string(obj.active) + ">";
        });

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        Represents the physical state of an agent in the simulated world.
        Contains position, orientation, velocity, and other body properties.
        "pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x,
              R"pbdoc(Get/set X position)pbdoc")
        .def_readwrite("y", &AgentBody::y,
              R"pbdoc(Get/set Y position)pbdoc")
        .def_readwrite("orientation", &AgentBody::orientation,
              R"pbdoc(Get/set orientation in radians)pbdoc")
        .def_readwrite("velocityX", &AgentBody::velocityX,
              R"pbdoc(Get/set X velocity)pbdoc")
        .def_readwrite("velocityY", &AgentBody::velocityY,
              R"pbdoc(Get/set Y velocity)pbdoc")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity,
              R"pbdoc(Get/set angular velocity (radians/sec))pbdoc")
        .def_readwrite("energy", &AgentBody::energy,
              R"pbdoc(Get/set energy level)pbdoc")
        .def_readwrite("health", &AgentBody::health,
              R"pbdoc(Get/set health level)pbdoc")
        .def_readwrite("age", &AgentBody::age,
              R"pbdoc(Get/set age in simulation steps)pbdoc")
        .def_readwrite("isMoving", &AgentBody::isMoving,
              R"pbdoc(Get/set whether agent is moving)pbdoc")
        .def_readwrite("isTurning", &AgentBody::isTurning,
              R"pbdoc(Get/set whether agent is turning)pbdoc")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime,
              R"pbdoc(Get/set time since last action)pbdoc")
        .def("reset", &AgentBody::reset,
              R"pbdoc(Reset agent body to initial state)pbdoc")
        .def("__repr__", [](const AgentBody& body) {
            return "<AgentBody x=" + std::to_string(body.x) +
                   " y=" + std::to_string(body.y) +
                   " orientation=" + std::to_string(body.orientation) +
                   " energy=" + std::to_string(body.energy) + ">";
        });

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        
        Represents the result of an action performed in the world,
        including reward value, success status, and descriptive message.
        "pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
              py::arg("reward"), py::arg("success"), py::arg("message") = "",
              R"pbdoc(Create an action result
              
              Args:
                  reward: Reward value (positive = good, negative = bad)
                  success: Whether the action succeeded
                  message: Descriptive message about the result
              )pbdoc")
        .def_readwrite("reward", &ActionResult::reward,
              R"pbdoc(Get/set reward value)pbdoc")
        .def_readwrite("success", &ActionResult::success,
              R"pbdoc(Get/set whether action succeeded)pbdoc")
        .def_readwrite("message", &ActionResult::message,
              R"pbdoc(Get/set result message)pbdoc")
        .def("__repr__", [](const ActionResult& result) {
            return "<ActionResult reward=" + std::to_string(result.reward) +
                   " success=" + std::to_string(result.success) +
                   " message=\"" + result.message + "\">";
        });

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        Contains sensory input data from different modalities:
        vision (what the agent sees), touch (collisions), internal (internal state),
        proprioception (body position), and audio signals.
        "pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
              R"pbdoc(Get vision input data (2D array of pixel intensities))pbdoc")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
              R"pbdoc(Set vision input data)pbdoc")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
              R"pbdoc(Get width of vision data in pixels)pbdoc")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
              R"pbdoc(Get height of vision data in pixels)pbdoc")
        .def("getTouch", &SensoryPercept::getTouch,
              R"pbdoc(Get touch input data (collision signals))pbdoc")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
              R"pbdoc(Set touch input data)pbdoc")
        .def("getInternal", &SensoryPercept::getInternal,
              R"pbdoc(Get internal signals (internal state))pbdoc")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
              R"pbdoc(Set internal signals)pbdoc")
        .def("getProprioception", &SensoryPercept::getProprioception,
              R"pbdoc(Get proprioception data (body position))pbdoc")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
              R"pbdoc(Set proprioception data)pbdoc")
        .def("getAudio", &SensoryPercept::getAudio,
              R"pbdoc(Get audio input data)pbdoc")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
              R"pbdoc(Set audio input data)pbdoc")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
              R"pbdoc(Get all sensory signals as a combined dictionary)")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
              R"pbdoc(Get timestamp of this sensory percept)pbdoc")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
              R"pbdoc(Set timestamp for this sensory percept)pbdoc")
        .def("__repr__", [](const SensoryPercept& percept) {
            return "<SensoryPercept vision=" + std::to_string(percept.getVision().size()) +
                   " touch=" + std::to_string(percept.getTouch().size()) +
                   " internal=" + std::to_string(percept.getInternal().size()) + ">";
        });

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        A 2D world where agents live, interact, and learn.
        Contains terrain, objects, and physics simulation.
        "pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"),
              R"pbdoc(Configure world dimensions
              
              Args:
                  width: World width in cells
                  height: World height in cells
                  visionWidth: Agent's visual field width
                  visionHeight: Agent's visual field height
              )pbdoc")
        .def("reset", &SimpleWorld::reset,
              R"pbdoc(Reset world to initial state)pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
              R"pbdoc(Set agent starting position)pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
              R"pbdoc(Update world by timestep seconds)pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
              py::arg("cmd"), py::arg("currentTime"),
              R"pbdoc(Apply a motor command to the agent)pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
              py::return_value_policy::reference_internal,
              R"pbdoc(Get agent's current sensory perception)")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
              py::return_value_policy::reference_internal,
              R"pbdoc(Get agent's current body state)pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
              R"pbdoc(Add an object to the world)pbdoc")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
              R"pbdoc(Remove object at position)pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
              R"pbdoc(Check if position is valid (not blocked))pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
              R"pbdoc(Get world width in cells)pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
              R"pbdoc(Get world height in cells)pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
              R"pbdoc(Get maximum agent energy)pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
              R"pbdoc(Set maximum agent energy)pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
              R"pbdoc(Get energy decay rate per timestep)pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
              R"pbdoc(Set energy decay rate per timestep)pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
              R"pbdoc(Get current simulation time)pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
              R"pbdoc(Set random seed for reproducible simulations)pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
              R"pbdoc(Get current random seed)pbdoc")
        .def("__repr__", [](const SimpleWorld& world) {
            return "<SimpleWorld width=" + std::to_string(world.getWidth()) +
                   " height=" + std::to_string(world.getHeight()) +
                   " time=" + std::to_string(world.getSimulationTime()) + ">";
        });

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class)
        
        The main brain class that implements Leaky Integrate-and-Fire (LIF) neurons
        with event-driven spike propagation and multiple plasticity mechanisms.
        Coordinates all neural computation and integrated systems.
        "pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
              R"pbdoc(Create a brain with configuration
              
              Args:
                  config: Brain configuration
              )pbdoc")
        .def("initialize", &Brain::initialize,
              R"pbdoc(Initialize the brain with configuration)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
              py::arg("currentStep"),
              R"pbdoc(Perform a simulation step)pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
              py::arg("currentStep"), py::arg("currentTime"),
              R"pbdoc(Perform a simulation step with timestamp)pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
              py::arg("input"),
              R"pbdoc(Inject sensory input into the brain)pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
              py::arg("neuron"), py::arg("current"),
              R"pbdoc(Inject current into a specific neuron)pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
              py::arg("type"), py::arg("current"),
              R"pbdoc(Inject current into all neurons of a specific type)pbdoc")
        .def("produceAction", &Brain::produceAction,
              R"pbdoc(Produce motor action based on neural activity)pbdoc")
        .def("reset", &Brain::reset,
              R"pbdoc(Reset brain state)pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
              R"pbdoc(Save brain state to file)pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
              R"pbdoc(Load brain state from file)pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
              R"pbdoc(Add a new neural region)pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
              py::return_value_policy::reference_internal,
              R"pbdoc(Get a region by ID)pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
              R"pbdoc(Get the number of regions)pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
              R"pbdoc(Get all region IDs)pbdoc")
        .def("getRegions", &Brain::getRegions,
              py::return_value_policy::reference_internal,
              R"pbdoc(Get all regions)pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
              R"pbdoc(Get total neuron count across all regions)pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
              R"pbdoc(Get total synapse count across all regions)pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
              R"pbdoc(Get count of active neurons)pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
              R"pbdoc(Get count of currently firing neurons)pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
              R"pbdoc(Get average firing rate across all neurons)pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
              R"pbdoc(Get excitation/inhibition balance ratio)pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
              R"pbdoc(Get total spike count)pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
              R"pbdoc(Get current developmental stage)pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
              py::arg("stage"),
              R"pbdoc(Set developmental stage)pbdoc")
        .def("getConfig", &Brain::getConfig,
              py::return_value_policy::reference_internal,
              R"pbdoc(Get the configuration)pbdoc")
        .def("logStatus", &Brain::logStatus,
              R"pbdoc(Log brain status to console)pbdoc")
        .def("__repr__", [](const Brain& brain) {
            return "<Brain regions=" + std::to_string(brain.getRegionCount()) +
                   " neurons=" + std::to_string(brain.getTotalNeuronCount()) +
                   " synapses=" + std::to_string(brain.getTotalSynapseCount()) + ">";
        });

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        Connects the NLM brain to the world simulation by handling sensory
transduction and motor decoding. Enables the brain to interact with environments.
        "pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
              R"pbdoc(Create an agent brain interface
              
              Args:
                  brain: The underlying brain to interface
              )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
              R"pbdoc(Initialize with world)pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
              R"pbdoc(Get expected sensory input size)pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
              R"pbdoc(Get expected motor output size)pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
              py::arg("percept"),
              R"pbdoc(Process sensory percept and inject into brain)pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
              R"pbdoc(Decode brain motor activity into motor command)pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
              py::arg("reward"), py::arg("predictedReward"),
              R"pbdoc(Apply reward-based neuromodulation)pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
              py::arg("timestep"),
              R"pbdoc(Update development system)pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
              R"pbdoc(Get current developmental stage)pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
              R"pbdoc(Get current neuromodulation level)pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
              R"pbdoc(Get curiosity level)pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
              R"pbdoc(Get novelty level)pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
              R"pbdoc(Get prediction error)pbdoc")
        .def("reset", &AgentBrain::reset,
              R"pbdoc(Reset agent for new episode)pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
              py::return_value_policy::reference_internal,
              R"pbdoc(Get the underlying brain)pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
              py::arg("enable"),
              R"pbdoc(Enable or disable reward modulation)pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
              py::arg("enable"),
              R"pbdoc(Enable or disable structural plasticity)pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
              py::arg("enable"),
              R"pbdoc(Enable or disable development)pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
              py::arg("enable"),
              R"pbdoc(Enable or disable curiosity)pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
              R"pbdoc(Check if reward modulation is enabled)pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
              R"pbdoc(Check if structural plasticity is enabled)pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
              R"pbdoc(Check if development is enabled)pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
              R"pbdoc(Check if curiosity is enabled)pbdoc")
        .def("__repr__", [](const AgentBrain& agent) {
            std::string brainInfo = agent.getBrain() ? "<AgentBrain brain>" : "<AgentBrain no-brain>";
            return brainInfo;
        });

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(Create a default configuration with sensible default values
              
              Returns:
                  A new Config object with default settings
              )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        return std::make_shared<Brain>(config);
    }, py::arg("config"), R"pbdoc(Create a new brain with configuration
              
              Args:
                  config: Brain configuration
                  
              Returns:
                  A new Brain object
              )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(Create a new simple world for simulation
              
              Returns:
                  A new SimpleWorld object
              )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        return std::make_shared<AgentBrain>(brain);
    }, py::arg("brain"), R"pbdoc(Create a new agent brain interface
              
              Args:
                  brain: The underlying brain to interface with
                  
              Returns:
                  A new AgentBrain object
              )pbdoc");

    // Module-level constants for convenience
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
}

} // namespace nlm
