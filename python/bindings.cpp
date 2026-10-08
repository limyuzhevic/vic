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

    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system.
        
        The Config class provides a key-value store for NLM system parameters.
        Configuration can be loaded from JSON files, command line arguments,
        or set at runtime. Keys use dot notation to represent nested parameters.
        
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> config.set("brain.region_count", 4)
            >>> config.set("brain.neurons_per_region", 200)
            >>> config.set("memory.working_memory_size", 50)
        
        Configuration is used to initialize Brain, AgentBrain, and other components.
        Common categories include:
        - brain: Neural architecture parameters
        - memory: Memory system settings
        - neuromodulation: Neuromodulator system parameters
        - development: Developmental process configuration
        - plasticity: Synaptic plasticity rules
        - visualization: Visualization and monitoring settings
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(
             Load configuration from a JSON file.
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 bool: True if configuration was loaded successfully, False otherwise
                 
             Raises:
                 RuntimeError: If the file cannot be read or parsed
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> success = config.loadFromFile("my_config.json")
                 >>> if success:
                 ...     print("Configuration loaded!")
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           R"pbdoc(
           Load configuration from command line arguments.
           
           Parses standard command line options in the format:
           --key.subkey value
           
           Args:
               argc: Number of command line arguments
               argv: Array of command line argument strings
               
           Returns:
               bool: True if configuration was loaded successfully, False otherwise
               
           Example:
               >>> import sys
               >>> config = pynlm.createDefaultConfig()
               >>> success = config.loadFromArgs(len(sys.argv), sys.argv)
               >>> # Usage: python script.py --brain.region_count 4 --memory.working_memory_size 50
           )pbdoc")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             R"pbdoc(
             Save configuration to a JSON file.
             
             Args:
                 filepath: Path where the configuration will be saved
                 
             Returns:
                 bool: True if configuration was saved successfully, False otherwise
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> config.set("brain.region_count", 4)
                 >>> config.saveToFile("my_config.json")
             )pbdoc")
        .def("has", &Config::has, py::arg("key"),
             R"pbdoc(
             Check if a configuration key exists.
             
             Args:
                 key: Configuration key to check (uses dot notation for nested keys)
                 
             Returns:
                 bool: True if the key exists in the configuration, False otherwise
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> config.set("brain.region_count", 4)
                 >>> config.has("brain.region_count")
                 True
                 >>> config.has("memory.working_memory_size")
                 False
             )pbdoc")
        .def("getKeys", &Config::getKeys,
             R"pbdoc(
             Get all configuration keys.
             
             Returns:
                 List[str]: List of all configuration keys in the system
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> keys = config.getKeys()
                 >>> print(keys)
                 ['brain.region_count', 'brain.neurons_per_region', 'memory.working_memory_size']
             )pbdoc")
        .def("clear", &Config::clear,
             R"pbdoc(
             Clear all configuration entries.
             
             This removes all key-value pairs from the configuration,
             returning it to a default empty state.
             
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> config.set("brain.region_count", 4)
                 >>> config.clear()
                 >>> config.has("brain.region_count")
                 False
             )pbdoc")
        .def("summary", &Config::summary,
             R"pbdoc(
             Get a summary string of the configuration.
             
             Returns:
                 str: Human-readable summary of configuration keys and values
                 
             Example:
                 >>> config = pynlm.createDefaultConfig()
                 >>> config.set("brain.region_count", 4)
                 >>> config.set("memory.working_memory_size", 50)
                 >>> print(config.summary())
                 Config: brain.region_count=4, brain.neurons_per_region=100, memory.working_memory_size=50
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

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(
        Simple 2D world for NLM simulation.
        
        The SimpleWorld class provides a two-dimensional environment for neural
        network agents to interact with. It supports basic physics simulation,
        collision detection, object placement, and sensory perception.
        
        The world implements a complete physics engine including velocity,
        acceleration, and collision handling. It provides sensory input through
        ray-casting vision and touch sensors to represent the agent's perception
        of its environment.
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             R"pbdoc(
             Configure world parameters.
             
             Args:
                 width: World width in simulation units
                 height: World height in simulation units
                 visionWidth: Width of agent's visual field (in cells)
                 visionHeight: Height of agent's visual field (in cells)
                 
             Note:
                 This method must be called before using the world. It sets up
                 the basic world dimensions and sensor specifications.
             )pbdoc")
        .def("reset", &SimpleWorld::reset,
             R"pbdoc(
             Reset world to initial state.
             
             This method resets the agent to a random starting position and
             regenerates all environmental objects (resources and hazards).
             The agent's energy and other state variables are also reset.
             
             Note:
                 Reset is typically called at the beginning of each new
                 episode or simulation.
             )pbdoc")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             R"pbdoc(
             Set agent starting position.
             
             Args:
                 x: Starting X coordinate
                 y: Starting Y coordinate
                 
             Note:
                 This provides a deterministic starting position for the agent,
                 useful for reproducible experiments and testing.
             )pbdoc")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             R"pbdoc(
             Update world by timestep seconds.
             
             Args:
                 timestep: Time step for simulation update (in seconds)
                 
             Note:
                 This method performs the complete world physics simulation
                 for one time step, including:
                 - Velocity integration
                 - Collision detection and response
                 - Energy decay
                 - Vision sensor generation
                 - Touch sensor updates
                 - State management
             )pbdoc")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             R"pbdoc(
             Apply motor command to world.
             
             Args:
                 cmd: Motor command to execute
                 currentTime: Current simulation time (for action timing)
                 
             Returns:
                 ActionResult: Result of the action including reward and success status
                 
             Note:
                 This method translates abstract motor commands into physical
                 effects in the world, including movement, turning, and interaction
                 with objects. It enforces action cooldown periods and handles
                 boundary conditions.
             )pbdoc")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get sensory percept from world.
             
             Returns:
                 SensoryPercept: Complete sensory perception containing vision,
                                touch, internal state, and proprioception
                 
             Note:
                 This method generates the agent's complete sensory input
                 based on the current world state and agent position. Vision
                 is computed using ray-casting from the agent's position and
                 orientation.
             )pbdoc")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get agent body state.
             
             Returns:
                 AgentBody: Complete agent state including position, velocity,
                          energy, health, and other attributes
                 
             Note:
                 This provides access to the agent's complete state for both
                 the agent brain and external monitoring systems.
             )pbdoc")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             R"pbdoc(
             Add object to world.
             
             Args:
                 obj: WorldObject to add to the world
                 
             Note:
                 Objects can be resources (positive reward), hazards (negative
                 reward), markers (goals), or walls (boundaries). Objects are
                 typically managed automatically by the world reset function.
             )pbdoc")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             R"pbdoc(
             Remove object at position.
             
             Args:
                 x: X coordinate where object should be removed
                 y: Y coordinate where object should be removed
                 
             Note:
                 This method removes any object at the specified position by
                 marking it as inactive. It's useful for dynamic object management
                 in more complex scenarios.
             )pbdoc")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             R"pbdoc(
             Check if position is valid.
             
             Args:
                 x: X coordinate to check
                 y: Y coordinate to check
                 
             Returns:
                 bool: True if position is inside world boundaries, False otherwise
                 
             Note:
                 Valid positions are within the world boundaries [0, width) x [0, height).
                 This is used for collision detection and path planning.
             )pbdoc")
        .def("getWidth", &SimpleWorld::getWidth,
             R"pbdoc(
             Get world width.
             
             Returns:
                 int: World width in simulation units
             )pbdoc")
        .def("getHeight", &SimpleWorld::getHeight,
             R"pbdoc(
             Get world height.
             
             Returns:
                 int: World height in simulation units
             )pbdoc")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             R"pbdoc(
             Get maximum energy.
             
             Returns:
                 float: Maximum energy capacity of the agent
             )pbdoc")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             R"pbdoc(
             Set maximum energy.
             
             Args:
                 e: New maximum energy value
                 
             Note:
                 This changes the agent's energy capacity, affecting how much
                 energy the agent can store and use for actions.
             )pbdoc")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             R"pbdoc(
             Get energy decay rate.
             
             Returns:
                 float: Rate at which agent energy decreases per time step
             )pbdoc")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             R"pbdoc(
             Set energy decay rate.
             
             Args:
                 r: New energy decay rate
                 
             Note:
                 Energy naturally decays over time, representing metabolic costs
                 or other energy expenditure. Lower values mean the agent can
                 sustain activity longer.
             )pbdoc")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             R"pbdoc(
             Get current simulation time.
             
             Returns:
                 float: Current simulation time in seconds
                 
             Note:
                 Simulation time accumulates from the last reset and is used
                 for action timing and state management.
             )pbdoc")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             R"pbdoc(
             Set random seed.
             
             Args:
                 seed: Random seed for reproducible world generation
                 
             Note:
                 Setting a random seed ensures that world generation (object
                 placement, agent starting position) is reproducible across
                 multiple simulation runs.
             )pbdoc")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             R"pbdoc(
             Get random seed.
             
             Returns:
                 int: Current random seed
                 
             Note:
                 The random seed affects the placement of objects and the agent's
                 starting position during world reset.
             )pbdoc");

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class for NLM (Phase 6: Integrated Artificial Brain).
        
        The Brain class represents the complete integrated neural system implementing
        all major NLM components including real spiking neural computation, event-driven
        dynamics, memory systems, prediction, cognition, and neuromodulation. It serves
        as the central coordinator of the artificial brain simulation.
        
        Phase 6 achieves full integration of all previously separate systems into a
        coherent, unified brain architecture that can learn, remember, predict, and
        make decisions through neural dynamics.
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             R"pbdoc(
             Initialize brain with configuration.
             
             Args:
                 config: Shared configuration object containing brain parameters
                 
             Note:
                 The configuration determines the brain's architecture, including
                 number of regions, neurons per region, connection probabilities, and
                 other system parameters. Always call initialize() after creating
                 the brain to set up the neural architecture and integrated systems.
             )pbdoc")
        .def("initialize", &Brain::initialize,
             R"pbdoc(
             Initialize the brain with configuration.
             
             This method performs the complete brain initialization including:
             - Creating neural regions and populations
             - Setting up connectivity patterns
             - Initializing all integrated systems (memory, prediction, cognition,
               neuromodulation, development)
             - Configuring plasticity systems
             - Setting up spike handling and event-driven processing
             - Initializing checkpoint management
             
             Returns:
                 bool: True if initialization was successful, False otherwise
                 
             Note:
                 This method should be called once after creating the brain
                 to set up the complete neural architecture. It implements
                 Phase 6 integration of all NLM systems.
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             R"pbdoc(
             Perform a simulation step with current step number.
             
             Args:
                 currentStep: Current simulation step number
                 
             Note:
                 This method implements one complete brain processing cycle using
                 the simplified timestep approach. It delegates to the main step
                 method with calculated time.
             )pbdoc")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             R"pbdoc(
             Perform a simulation step with precise timestamp.
             
             Args:
                 currentStep: Current simulation step number
                 currentTime: Current simulation time in seconds
                 
             Note:
                 This method implements the complete integrated brain loop (Phase 6):
                 1. Process delayed spike events (deliver synaptic input)
                 2. Update all neurons (LIF dynamics)
                 3. Detect spikes and schedule spike events
                 4. Update working memory (maintenance and competition)
                 5. Apply neuromodulation effects on neural excitability
                 6. Apply plasticity rules (STDP and Hebbian learning)
                 7. Update episodic memory with current experience
                 8. Update prediction system
                 9. Update attention system
                 10. Update concept formation
                 11. Apply structural plasticity
                 12. Replay important memories
                 13. Apply development effects
                 14. Collect statistics
                 
                 This represents the complete artificial brain processing cycle
                 integrating all NLM systems.
             )pbdoc")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             R"pbdoc(
             Inject sensory input into the brain.
             
             Args:
                 input: SensoryInput containing environmental information
                 
             Note:
                 This method converts external sensory data into neural current
                 injection, effectively implementing the sensory transduction
                 stage of the brain-world interface. The input is distributed
                 across sensory neurons based on their population assignments.
             )pbdoc")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             R"pbdoc(
             Inject current directly into a specific neuron.
             
             Args:
                 neuron: Neuron ID to inject current into
                 current: Membrane potential current to inject (in mV)
                 
             Note:
                 This provides direct control over specific neurons, useful for
                 testing, reinforcement learning experiments, or external control
                 of neural activity.
             )pbdoc")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             R"pbdoc(
             Inject current into all neurons of a specific type.
             
             Args:
                 type: Neuron type to target (Excitatory, Inhibitory, etc.)
                 current: Membrane potential current to inject (in mV)
                 
             Note:
                 This method provides bulk control over neuron populations,
                 allowing targeted modulation of neural activity based on neuron type.
             )pbdoc")
        .def("produceAction", &Brain::produceAction,
             R"pbdoc(
             Produce motor/action output based on neural activity.
             
             Returns:
                 Action: Motor action determined by the brain's neural activity
                 
             Note:
                 This method implements action selection by analyzing motor neuron
                 activity and producing the most appropriate action. It represents
                 the output stage of the brain-world interface, converting neural
                 computation into world-directed behavior.
             )pbdoc")
        .def("reset", &Brain::reset,
             R"pbdoc(
             Reset brain state.
             
             This method performs a complete reset of the brain, including:
             - Resetting simulation counters and timing
             - Clearing all neural regions
             - Resetting spike systems
             - Resetting to initial developmental stage
             - Clearing memory systems
             - Resetting attention and cognitive systems
             
             Note:
             This is typically called at the beginning of each new episode
             or when starting a new simulation.
             )pbdoc")
        .def("save", &Brain::save, py::arg("filepath"),
             R"pbdoc(
             Save brain state to file (checkpointing).
             
             Args:
                 filepath: Path where brain state will be saved
                 
             Returns:
                 bool: True if save was successful, False otherwise
                 
             Note:
                 This method saves the complete brain state including:
                 - Neuron membrane potentials and states
                 - Synaptic weights and connections
                 - Memory system contents
                 - Current developmental stage
                 - Simulation counters and timing
                 
                 Checkpointing allows resuming simulations from saved states,
                 supporting long-running experiments and fault tolerance.
             )pbdoc")
        .def("load", &Brain::load, py::arg("filepath"),
             R"pbdoc(
             Load brain state from file.
             
             Args:
                 filepath: Path from which to load brain state
                 
             Returns:
                 bool: True if load was successful, False otherwise
                 
             Note:
                 This method restores brain state from a checkpoint file.
                 It should only be used with files saved by the save() method
                 from the same or compatible brain configuration.
                 
                 Loading allows resuming interrupted simulations or testing
                 with pre-trained brain states.
             )pbdoc")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             R"pbdoc(
             Add a new neural region.
             
             Args:
                 name: Optional name for the region (for identification)
                 
             Returns:
                 RegionId: Unique identifier for the newly created region
                 
             Note:
                 Regions are basic organizational units in the brain architecture.
                 Each region contains its own neural populations and local connectivity.
                 Regions can be used to implement different functional areas or
                 processing modules within the integrated brain system.
             )pbdoc")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get a region by ID.
             
             Args:
                 id: RegionId to retrieve
                 
             Returns:
                 NeuralRegion: Reference to the requested region, or nullptr if not found
                 
             Note:
                 This method provides access to individual regions for inspection,
                 modification, or detailed analysis. Regions can be used to implement
                 modular brain architectures or to study regional differences in function.
             )pbdoc")
        .def("getRegionCount", &Brain::getRegionCount,
             R"pbdoc(
             Get the number of regions.
             
             Returns:
                 int: Total number of regions in the brain
                 
             Note:
                 This provides a quick count of the brain's regional organization.
                 More detailed regional information can be obtained by iterating
                 through getRegions().
             )pbdoc")
        .def("getRegionIds", &Brain::getRegionIds,
             R"pbdoc(
             Get all region IDs.
             
             Returns:
                 List[RegionId]: List of all region identifiers
                 
             Note:
                 This provides a complete list of all region IDs in the brain,
                 which can be used for region management, iteration, or
                 inter-region connection management.
             )pbdoc")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get all regions.
             
             Returns:
                 List[NeuralRegion]: List of references to all brain regions
                 
             Note:
                 This provides access to the complete regional organization
                 of the brain. It can be used to analyze regional differences,
                 implement region-specific operations, or study distributed processing.
             )pbdoc")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             R"pbdoc(
             Get total neuron count across all regions.
             
             Returns:
                 int: Total number of neurons in the brain
                 
             Note:
                 This provides a global measure of brain size and computational
                 capacity. It can be used for resource allocation, performance
                 analysis, or comparing different brain architectures.
             )pbdoc")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             R"pbdoc(
             Get total synapse count across all regions.
             
             Returns:
                 int: Total number of synapses in the brain
                 
             Note:
                 This provides a measure of brain connectivity and complexity.
                 Synaptic count is related to learning capacity, information
                 processing capabilities, and computational power.
             )pbdoc")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             R"pbdoc(
             Get count of active neurons.
             
             Returns:
                 int: Number of neurons currently firing or otherwise active
                 
             Note:
                 Active neuron count indicates the level of ongoing neural
                 processing and can be used as a measure of cognitive load,
                 arousal, or processing intensity.
             )pbdoc")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             R"pbdoc(
             Get count of currently firing neurons.
             
             Returns:
                 int: Number of neurons that are actively firing spikes
                 
             Note:
                 Firing neuron count is a direct measure of ongoing neural
                 activity and can be used to track processing dynamics, learning
                 progress, or cognitive states.
             )pbdoc")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             R"pbdoc(
             Get average firing rate across all neurons.
             
             Returns:
                 float: Average firing rate in Hertz (spikes per second)
                 
             Note:
                 Firing rate is a fundamental measure of neural activity and
                 cognitive processing. It can be used to track learning curves,
                 monitor brain states, or evaluate system performance.
             )pbdoc")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             R"pbdoc(
             Get excitation/inhibition balance ratio.
             
             Returns:
                 float: Ratio of total excitatory to inhibitory synaptic weights
                 
             Note:
                 The E/I ratio is a crucial indicator of brain network stability
                 and function. Healthy E/I balance is important for proper neural
                 dynamics, and imbalances can indicate pathological states or
                 learning-related changes.
             )pbdoc")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             R"pbdoc(
             Get total spike count.
             
             Returns:
                 int: Total number of spikes generated by the brain
                 
             Note:
                 Total spike count provides a measure of overall neural activity
                 and processing. It can be used to track learning progress, compare
                 different experimental conditions, or evaluate system performance.
             )pbdoc")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current stage in the developmental timeline
                 
             Note:
                 Developmental stages (Initial, CriticalPeriod, Maturation, Adult,
                 Aging) affect neural plasticity rates, connectivity patterns, and
                 learning capabilities. This provides insight into the brain's
                 current state in its developmental trajectory.
             )pbdoc")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             R"pbdoc(
             Set developmental stage.
             
             Args:
                 stage: Developmental stage to set
                 
             Note:
                 This method allows direct control over the brain's developmental
                 stage, useful for testing developmental effects, simulating
                 different life stages, or studying age-dependent changes in
                 neural function.
             )pbdoc")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the configuration.
             
             Returns:
                 Config: Reference to the brain's configuration
                 
             Note:
                 This provides access to the brain's configuration parameters,
                 which can be useful for inspection, debugging, or runtime configuration
                 modifications.
             )pbdoc")
        .def("logStatus", &Brain::logStatus,
             R"pbdoc(
             Log brain status.
             
             This method logs comprehensive brain status information including:
             - Neural statistics (neurons, synapses, firing rates)
             - Memory system status
             - Neuromodulation levels
             - Developmental stage
             - Regional details
             
             Note:
             This is primarily useful for debugging, monitoring, and
             understanding brain state during experiments or simulations.
             )pbdoc");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to the world.
        
        The AgentBrain class serves as the bridge between the neural brain and the
        external world. It handles sensory transduction (converting world percepts
        into neural input) and motor decoding (converting neural activity into actions).
        
        This class is essential for agent-environment interaction, managing the
        complete sensory-motor loop of the NLM system.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             R"pbdoc(
             Initialize agent brain with a neural brain.
             
             Args:
                 brain: Shared pointer to the Brain instance to interface with
             )pbdoc")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             R"pbdoc(
             Initialize agent with world interface.
             
             Args:
                 world: Reference to the SimpleWorld instance
                 
             Note:
                 This method must be called before using the agent brain.
                 It sets up the agent's sensory and motor neuron mappings
                 based on the brain's neural architecture.
             )pbdoc")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             R"pbdoc(
             Get expected sensory input size expected by brain.
             
             Returns:
                 int: Size of sensory input vector expected by the brain
                 
             Note:
                 This determines the dimensionality of sensory percepts
                 that can be processed by the agent brain.
             )pbdoc")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             R"pbdoc(
             Get expected motor output size.
             
             Returns:
                 int: Size of motor output vector that the brain expects
                 
             Note:
                 This corresponds to the number of motor actions the brain
                 can produce (typically one per action type).
             )pbdoc")
        .def("processSensoryInput", &AgentBrain::processSensoryInput,
             py::arg("percept"),
             R"pbdoc(
             Process sensory percept and inject into brain.
             
             Args:
                 percept: SensoryPercept containing world information from vision,
                         touch, internal state, and proprioception
                 
             Note:
                 This method converts world perception into neural current
                 injection, effectively implementing the sensory transduction
                 layer of the brain-world interface.
             )pbdoc")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             R"pbdoc(
             Decode brain motor activity into motor command.
             
             Returns:
                 MotorCommand: The decoded action to be executed in the world
                 
             Note:
                 This method analyzes the activity of motor neurons in the brain
                 and selects the most appropriate action for the current context.
                 It implements the motor decoding stage of the interface.
             )pbdoc")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             R"pbdoc(
             Apply reward-based neuromodulation to the agent.
             
             Args:
                 reward: Actual reward received from the environment
                 predictedReward: Predicted reward based on agent's expectations
                 
             Note:
                 This implements reward prediction error (RPE) computation,
                 which is fundamental for reinforcement learning. The difference
                 between actual and predicted reward drives learning through
                 neuromodulatory systems like dopamine.
             )pbdoc")
        .def("updateDevelopment", &AgentBrain::updateDevelopment,
             py::arg("timestep"),
             R"pbdoc(
             Update development system.
             
             Args:
                 timestep: Time increment for developmental updates
                 
             Note:
                 This method handles developmental plasticity and maturation,
                 allowing the agent's neural systems to evolve over time.
             )pbdoc")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             R"pbdoc(
             Get current developmental stage.
             
             Returns:
                 DevelopmentalStage: Current stage in the developmental timeline
                 
             Note:
                 Developmental stages include Initial, CriticalPeriod, Maturation,
                 Adult, and Aging, each with different characteristics for
                 plasticity and learning capabilities.
             )pbdoc")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             R"pbdoc(
             Get current neuromodulation level.
             
             Returns:
                 float: Current neuromodulation level (typically dopamine)
                 
             Note:
                 Neuromodulation levels influence neural excitability, plasticity,
                 and learning rate. Higher levels typically increase plasticity
                 and exploration.
             )pbdoc")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             R"pbdoc(
             Get curiosity level.
             
             Returns:
                 float: Current curiosity level (0.0 to 1.0)
                 
             Note:
                 Curiosity drives exploration and novel experience seeking.
                 Higher curiosity levels increase the likelihood of exploring
                 new actions and environments.
             )pbdoc")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             R"pbdoc(
             Get novelty level.
             
             Returns:
                 float: Current novelty level (0.0 to 1.0)
                 
             Note:
                 Novelty represents the surprise or unexpectedness of sensory input.
                 High novelty indicates encountering something new or unfamiliar,
                 which can trigger learning and exploration responses.
             )pbdoc")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             R"pbdoc(
             Get prediction error.
             
             Returns:
                 float: Current prediction error (actual - predicted reward)
                 
             Note:
                 Prediction error is a fundamental learning signal in
                 reinforcement learning, indicating when outcomes differ from
                 expectations. It drives updating of value estimates and policies.
             )pbdoc")
        .def("reset", &AgentBrain::reset,
             R"pbdoc(
             Reset agent for new episode.
             
             This method resets the agent's internal state, including neuromodulation
             levels, developmental age, and memory traces. It's typically called
             at the beginning of each new episode in episodic tasks.
             )pbdoc")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             R"pbdoc(
             Get the underlying brain.
             
             Returns:
                 Brain: Reference to the brain instance this agent interfaces with
                 
             Note:
                 This provides access to the brain's internal state and systems
                 for advanced agent control and debugging.
             )pbdoc")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable reward modulation.
             
             Args:
                 enable: True to enable, False to disable reward-based learning
                 
             Note:
                 Reward modulation is essential for reinforcement learning. When
                 disabled, the agent will not learn from reward prediction errors
                 or update its value estimates based on experience.
             )pbdoc")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable structural plasticity.
             
             Args:
                 enable: True to enable, False to disable structural changes
                 
             Note:
                 Structural plasticity allows the agent to grow new connections
                 and prune existing ones, enabling adaptation of neural architecture
                 over time based on experience.
             )pbdoc")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable development.
             
             Args:
                 enable: True to enable, False to disable developmental processes
                 
             Note:
                 Development enables age-dependent changes in neural properties,
                 including plasticity rates, connectivity patterns, and learning capabilities.
             )pbdoc")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             R"pbdoc(
             Enable or disable curiosity.
             
             Args:
                 enable: True to enable, False to disable curiosity-driven exploration
                 
             Note:
                 Curiosity enables the agent to explore novel situations and actions,
                 balancing exploration and exploitation in reinforcement learning tasks.
             )pbdoc")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             R"pbdoc(
             Check if reward modulation is enabled.
             
             Returns:
                 bool: True if reward modulation is enabled, False otherwise
             )pbdoc")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             R"pbdoc(
             Check if structural plasticity is enabled.
             
             Returns:
                 bool: True if structural plasticity is enabled, False otherwise
             )pbdoc")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             R"pbdoc(
             Check if development is enabled.
             
             Returns:
                 bool: True if development is enabled, False otherwise
             )pbdoc")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             R"pbdoc(
             Check if curiosity is enabled.
             
             Returns:
                 bool: True if curiosity is enabled, False otherwise
             )pbdoc");

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
