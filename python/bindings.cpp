#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/functional.h>
#include <pybind11/chrono.h>
#include <memory>
#include <string>
#include <vector>
#include <functional>
#include <cmath>

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
        
        This module provides Python-friendly interfaces for simulating artificial brains
        with spiking neurons, memory systems, neuromodulation, and cognitive functions.
        
        Key features:
        - Type hints and comprehensive docstrings
        - Pythonic property access
        - Convenience functions for common patterns
        - Enhanced error handling
        - Built-in configuration presets
        - Complete simulation loop helpers
        
        For comprehensive documentation, see docs/PYTHON_API.md
    )pbdoc";

    // Custom exception for Python users
    py::register_exception<std::runtime_error>(m, "RuntimeError", PyExc_RuntimeError);
    py::register_exception<std::runtime_error>(m, "BrainError", PyExc_RuntimeError);
    
    // Type conversion functions
    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system
        
        The Config class manages all simulation parameters including neural,
        plasticity, neuromodulation, and environmental settings.
        
        Configuration can be loaded from JSON files, command line arguments,
        or created programmatically.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             R"pbdoc(Load configuration from a JSON file
             
             Args:
                 filepath: Path to the JSON configuration file
                 
             Returns:
                 True if configuration was loaded successfully, False otherwise
             )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments\n\n"
           "Parses --config key=value pairs and standard CLI options\n\n"
           "Args:\n"
           "    argc: Argument count\n"
           "    argv: Argument vector\n\n"
           "Returns:\n"
           "    True if configuration was loaded, False otherwise")
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

    // Global convenience functions for configuration
    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, "Create a default configuration with sensible values for beginners");

    m.def("createMinimalConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 100);
        config->set("region_count", 1);
        config->set("connection_probability", 0.1f);
        config->set("simulation_timestep", 0.001);
        return config;
    }, "Create a minimal configuration for quick testing");

    m.def("createResearchConfig", []() -> std::shared_ptr<Config> {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", 2000);
        config->set("region_count", 2);
        config->set("connection_probability", 0.15f);
        config->set("simulation_timestep", 0.001);
        config->set("plasticity.stdp.enable", true);
        config->set("plasticity.hebbian.enable", true);
        config->set("neuromod.dopamine.scale", 2.0);
        config->set("neuromod.curiosity.enable", true);
        config->set("neuromod.novelty.enable", true);
        config->set("development.enable", true);
        config->set("memory.working.enable", true);
        config->set("memory.episodic.enable", true);
        config->set("prediction.enable", true);
        config->set("cognition.planner.enable", true);
        config->set("cognition.concept.enable", true);
        config->set("cognition.attention.enable", true);
        return config;
    }, "Create a research-oriented configuration for advanced experiments");

    m.def("createQuickBrain", [](std::shared_ptr<Config> config = nullptr) -> std::shared_ptr<Brain> {
        if (!config) {
            config = std::make_shared<Config>();
            config->set("neuron_count", 500);
            config->set("region_count", 1);
            config->set("connection_probability", 0.05);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config") = nullptr,
       "Create a brain with sensible defaults for quick testing\n\n"
       "Args:\n"
       "    config: Optional configuration (uses defaults if not provided)\n\n"
       "Returns:\n"
       "    Brain instance ready for simulation");

    m.def("createAgentSetup", [](std::shared_ptr<SimpleWorld> world, 
                                 std::shared_ptr<Config> config = nullptr) {
        auto brain = createQuickBrain(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        return std::make_tuple(brain, agent);
    }, py::arg("world"), py::arg("config") = nullptr,
       "Create a complete agent setup with world for simulation\n\n"
       "Args:\n"
       "    world: SimpleWorld instance for the agent to interact with\n"
       "    config: Optional brain configuration (uses defaults if not provided)\n\n"
       "Returns:\n"
       "    Tuple of (brain, agent) ready for simulation");

    m.def("run_episode", [](std::shared_ptr<Brain> brain,
                           std::shared_ptr<AgentBrain> agent,
                           std::shared_ptr<SimpleWorld> world,
                           int num_steps,
                           bool verbose = false) {
        world->reset();
        agent->reset();
        
        float total_reward = 0.0f;
        size_t max_firing = 0;
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, world->getSimulationTime());
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = world->computeReward(agent->getBrain()->getRegions()[0].get());
            total_reward += reward;
            agent->applyRewardModulation(reward, 0.0f);
            
            if (verbose && step % 50 == 0) {
                std::cout << "Step " << step << ": "
                         << "Reward: " << (total_reward / (step + 1))
                         << " | Firing: " << brain->getAverageFiringRate()
                         << " | Curios: " << agent->getCuriosityLevel()
                         << std::endl;
            }
            
            max_firing = std::max(max_firing, brain->getFiringNeuronCount());
        }
        
        return std::make_tuple(total_reward / num_steps, max_firing);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), 
          py::arg("num_steps") = 100, py::arg("verbose") = false,
       "Run a complete simulation episode\n\n"
       "Args:\n"
       "    brain: The neural brain\n"
       "    agent: The agent interface\n"
       "    world: The environment\n"
       "    num_steps: Number of simulation steps\n"
       "    verbose: Print progress information\n\n"
       "Returns:\n"
       "    Tuple of (average_reward, max_firing_neurons)");

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

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input
        
        SensoryInput represents information received from the environment or internal
        state of the brain. Subclasses include Vision (visual input), Audio (sound input),
        and InternalSignals (internal brain state).
        
        Attributes:
            type: The type of sensory input
            timestamp: When this sensory data was created
        
        Methods:
            getType() -> NeuronType: Get the input type
            getData() -> List[float]: Get raw data values
            getDimensions() -> List[int]: Get data dimensions
            getTimestamp() -> float: Get timestamp
            setTimestamp(timestamp: float): Set timestamp
    )pbdoc")
        .def("getType", &SensoryInput::getType,
             "Get the type of sensory input\n\n"
             "Returns:\n"
             "    NeuronType: The type of sensory input")
        .def("getData", &SensoryInput::getData,
             "Get the raw data as a vector of floats\n\n"
             "Returns:\n"
             "    List[float]: Vector of sensor readings")
        .def("getDimensions", &SensoryInput::getDimensions,
             "Get the dimensionality of the sensory data\n\n"
             "Returns:\n"
             "    List[int]: Dimensions of the sensory data")
        .def("getTimestamp", &SensoryInput::getTimestamp,
             "Get the timestamp when this sensory data was created\n\n"
             "Returns:\n"
             "    float: Simulation time when data was generated")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp for this sensory data\n\n"
             "Args:\n"
             "    timestamp: Simulation time to set");

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Visual sensory input
        
        Vision represents visual information from the environment, typically as
        pixel values from a camera or simulated vision system.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (typically 3 for RGB)
        
        Methods:
            __init__(width=0, height=0, channels=3) -> None
            setData(data) -> None: Set pixel data
            getWidth() -> int: Get image width
            getHeight() -> int: Get image height
            getChannels() -> int: Get number of channels
        
        Example:
            >>> vision = Vision(width=8, height=8, channels=3)
            >>> vision.setData([0.0] * 192)  # 8x8x3 = 192 pixels
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Create a Vision sensory input\n\n"
             "Args:\n"
             "    width: Image width in pixels\n"
             "    height: Image height in pixels\n"
             "    channels: Number of color channels (default: 3 for RGB)")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set pixel data for the vision image\n\n"
             "Args:\n"
             "    data: Vector of pixel values (width × height × channels)")
        .def("getWidth", &Vision::getWidth,
             "Get the image width in pixels\n\n"
             "Returns:\n"
             "    int: Image width in pixels")
        .def("getHeight", &Vision::getHeight,
             "Get the image height in pixels\n\n"
             "Returns:\n"
             "    int: Image height in pixels")
        .def("getChannels", &Vision::getChannels,
             "Get the number of color channels\n\n"
             "Returns:\n"
             "    int: Number of color channels");

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input
        
        Audio represents sound information, typically as a time series of samples
        from an audio sensor or generated by the brain.
        
        Attributes:
            sampleRate: Audio sampling rate in Hz
            numSamples: Number of audio samples
        
        Methods:
            __init__(sampleRate=0, numSamples=0) -> None
            setData(data) -> None: Set audio samples
            setSampleRate(sampleRate) -> None: Set sampling rate
            getSampleRate() -> int: Get sampling rate
            getNumSamples() -> int: Get sample count
        
        Example:
            >>> audio = Audio(sampleRate=44100, numSamples=1000)
            >>> audio.setData([0.0] * 1000)  # 1 second of silence
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Create Audio sensory input\n\n"
             "Args:\n"
             "    sampleRate: Sampling rate in Hz\n"
             "    numSamples: Number of audio samples")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set audio samples\n\n"
             "Args:\n"
             "    data: Vector of audio sample values")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set the audio sampling rate\n\n"
             "Args:\n"
             "    sampleRate: Sampling rate in Hz")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get the audio sampling rate\n\n"
             "Returns:\n"
             "    int: Sampling rate in Hz")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get the number of audio samples\n\n"
             "Returns:\n"
             "    int: Number of audio samples");

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input
        
        InternalSignals represents the brain's internal state, such as motor commands,
        intentions, or other internal representations that can be used as sensory input.
        
        Methods:
            __init__() -> None
            addSignal(value) -> None: Add a signal value
            clearSignals() -> None: Clear all signals
        
        Example:
            >>> internal = InternalSignals()
            >>> internal.addSignal(0.5)  # Add an internal signal
            >>> internal.addSignal(0.8)  # Add another signal
        )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals\n\n"
             "Args:\n"
             "    value: Signal value to add")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output
        
        Action represents a motor command or behavior that the brain can produce,
        such as moving forward, turning, or interacting with the environment.
        
        Attributes:
            type: The type of action to perform
            parameters: Additional parameters for the action (e.g., speed, distance)
        
        Methods:
            __init__(type=ActionType.Wait) -> None
            __init__(type, parameters) -> None
            getType() -> ActionType: Get the action type
            setType(type) -> None: Set the action type
            getParameters() -> List[float]: Get action parameters
            setParameters(parameters) -> None: Set action parameters
            getName() -> str: Get human-readable action name
            clone() -> Action: Create a copy of this action
        
        Example:
            >>> action = Action(ActionType.MoveForward, [1.0, 0.0, 0.0])
            >>> action.getName()
            'move_forward'
            >>> action.getParameters()
            [1.0, 0.0, 0.0]
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType,
             "Get the action type\n\n"
             "Returns:\n"
             "    ActionType: The type of action")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type\n\n"
             "Args:\n"
             "    type: The action type to set")
        .def("getParameters", &Action::getParameters,
             "Get the action parameters\n\n"
             "Returns:\n"
             "    List[float]: Action parameters")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set the action parameters\n\n"
             "Args:\n"
             "    params: List of action parameters")
        .def("getName", &Action::getName,
             "Get the human-readable name of this action\n\n"
             "Returns:\n"
             "    str: Human-readable action name")
        .def("clone", &Action::clone,
             "Create a copy of this action\n\n"
             "Returns:\n"
             "    Action: Copy of this action");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation
        
        WorldObject represents an object in the simulated world, such as a resource,
        hazard, wall, or marker that the agent can interact with.
        
        Attributes:
            x: X position in world coordinates
            y: Y position in world coordinates
            type: Type of world object
            value: Numerical value (e.g., resource quantity)
            radius: Interaction radius
            active: Whether the object is currently active
        
        Methods:
            __init__(x=0.0, y=0.0, type=WorldObjectType.Empty, value=0.0, radius=0.5) -> None
        
        Example:
            >>> resource = WorldObject(x=5.0, y=5.0, type=WorldObjectType.Resource, value=10.0)
            >>> resource.x
            5.0
            >>> resource.type
            <WorldObjectType.Resource: 4>
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Create a world object\n\n"
             "Args:\n"
             "    x: X position in world coordinates\n"
             "    y: Y position in world coordinates\n"
             "    type: Type of world object\n"
             "    value: Numerical value (default: 0.0)\n"
             "    radius: Interaction radius (default: 0.5)")
        .def_readwrite("x", &WorldObject::x, "X position in world coordinates")
        .def_readwrite("y", &WorldObject::y, "Y position in world coordinates")
        .def_readwrite("radius", &WorldObject::radius, "Interaction radius")
        .def_readwrite("type", &WorldObject::type, "Type of world object")
        .def_readwrite("value", &WorldObject::value, "Numerical value")
        .def_readwrite("active", &WorldObject::active, "Whether object is active");

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state
        
        AgentBody represents the physical state of the agent in the world, including
        position, velocity, orientation, energy, health, and age.
        
        Attributes:
            x: X position
            y: Y position
            orientation: Facing direction in radians
            velocityX: X velocity component
            velocityY: Y velocity component
            angularVelocity: Rotational velocity
            energy: Current energy level
            health: Health status (0-100)
            age: Agent age in simulation steps
            isMoving: Whether agent is currently moving
            isTurning: Whether agent is currently turning
            lastActionTime: Time since last action
        
        Methods:
            __init__() -> None
            reset() -> None: Reset body state to defaults
        
        Example:
            >>> body = AgentBody()
            >>> body.x = 10.0
            >>> body.y = 5.0
            >>> body.energy = 50.0
            >>> body.isMoving = True
    )pbdoc")
        .def(py::init<>())
        .def_readwrite("x", &AgentBody::x, "X position")
        .def_readwrite("y", &AgentBody::y, "Y position")
        .def_readwrite("orientation", &AgentBody::orientation, "Facing direction (radians)")
        .def_readwrite("velocityX", &AgentBody::velocityX, "X velocity component")
        .def_readwrite("velocityY", &AgentBody::velocityY, "Y velocity component")
        .def_readwrite("angularVelocity", &AgentBody::angularVelocity, "Rotational velocity")
        .def_readwrite("energy", &AgentBody::energy, "Current energy level")
        .def_readwrite("health", &AgentBody::health, "Health status (0-100)")
        .def_readwrite("age", &AgentBody::age, "Agent age in simulation steps")
        .def_readwrite("isMoving", &AgentBody::isMoving, "Whether agent is moving")
        .def_readwrite("isTurning", &AgentBody::isTurning, "Whether agent is turning")
        .def_readwrite("lastActionTime", &AgentBody::lastActionTime, "Time since last action")
        .def("reset", &AgentBody::reset, "Reset body state to initial values");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world
        
        ActionResult represents the outcome of an agent's action in the world,
        including reward, success status, and a descriptive message.
        
        Attributes:
            reward: Numerical reward value
            success: Whether the action was successful
            message: Descriptive message about the action result
        
        Methods:
            __init__(reward=0.0, success=False, message="") -> None
        
        Example:
            >>> result = ActionResult(reward=10.0, success=True, message="Found resource")
            >>> result.reward
            10.0
            >>> result.success
            True
            >>> result.message
            'Found resource'
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Create an action result\n\n"
             "Args:\n"
             "    reward: Numerical reward value\n"
             "    success: Whether action was successful\n"
             "    message: Descriptive message (default: empty)")
        .def_readwrite("reward", &ActionResult::reward, "Reward value")
        .def_readwrite("success", &ActionResult::success, "Whether action succeeded")
        .def_readwrite("message", &ActionResult::message, "Result message");

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data
        
        SensoryPercept represents the complete set of sensory information available
        to the agent at a given moment, including visual, tactile, internal, and audio data.
        
        The percept serves as the interface between the world and the agent,
        containing all sensory modalities that the agent can perceive.
        
        Attributes:
            vision: Visual input data
            touch: Tactile/somatosensory input
            internal: Internal body signals
            proprioception: Body position/awareness
            audio: Audio input data
        
        Methods:
            __init__() -> None
            getVision() -> Vision: Get visual input
            setVision(vision) -> None: Set visual input
            getVisionWidth() -> int: Get vision width
            getVisionHeight() -> int: Get vision height
            getTouch() -> InternalSignals: Get touch input
            setTouch(touch) -> None: Set touch input
            getInternal() -> InternalSignals: Get internal signals
            setInternal(internal) -> None: Set internal signals
            getProprioception() -> InternalSignals: Get proprioception
            setProprioception(proprioception) -> None: Set proprioception
            getAudio() -> Audio: Get audio input
            setAudio(audio) -> None: Set audio input
            getAllSignals() -> List[float]: Get all signal values
            getTimestamp() -> float: Get percept timestamp
            setTimestamp(timestamp) -> None: Set percept timestamp
        
        Example:
            >>> percept = SensoryPercept()
            >>> percept.setVision(Vision(width=8, height=8))
            >>> percept.getVisionWidth()
            8
            >>> percept.getTouch().addSignal(0.5)
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             "Get the visual component of this percept\n\n"
             "Returns:\n"
             "    Vision: Visual input data")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set the visual component of this percept\n\n"
             "Args:\n"
             "    vision: Visual input data")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get the width of the visual field\n\n"
             "Returns:\n"
             "    int: Vision width in pixels")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get the height of the visual field\n\n"
             "Returns:\n"
             "    int: Vision height in pixels")
        .def("getTouch", &SensoryPercept::getTouch,
             "Get the tactile/somatosensory input\n\n"
             "Returns:\n"
             "    InternalSignals: Touch input data")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set the tactile input\n\n"
             "Args:\n"
             "    touch: Touch input data")
        .def("getInternal", &SensoryPercept::getInternal,
             "Get the internal signals\n\n"
             "Returns:\n"
             "    InternalSignals: Internal signal data")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set the internal signals\n\n"
             "Args:\n"
             "    internal: Internal signal data")
        .def("getProprioception", &SensoryPercept::getProprioception,
             "Get the proprioceptive (body awareness) input\n\n"
             "Returns:\n"
             "    InternalSignals: Proprioception data")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set the proprioceptive input\n\n"
             "Args:\n"
             "    proprioception: Proprioception data")
        .def("getAudio", &SensoryPercept::getAudio,
             "Get the audio input\n\n"
             "Returns:\n"
             "    Audio: Audio input data")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set the audio input\n\n"
             "Args:\n"
             "    audio: Audio input data")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all signal values from all sensory modalities\n\n"
             "Returns:\n"
             "    List[float]: All signal values")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get the timestamp of this percept\n\n"
             "Returns:\n"
             "    float: Percept timestamp")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set the timestamp of this percept\n\n"
             "Args:\n"
             "    timestamp: Percept timestamp");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation
        
        SimpleWorld represents a 2D environment where the NLM brain operates, providing
        the interface between the agent and the world. It handles world state, physics,
        object management, and sensory processing.
        
        Attributes:
            width: World width in cells
            height: World height in cells
            visionWidth: Width of agent's visual field
            visionHeight: Height of agent's visual field
        
        Methods:
            __init__() -> None
            configure(width, height, visionWidth, visionHeight) -> None
            reset() -> None: Reset world to initial state
            setAgentStart(x, y) -> None: Set agent's starting position
            update(timestep) -> None: Update world physics
            applyMotorCommand(cmd, currentTime) -> ActionResult: Apply action
            getSensoryPercept() -> SensoryPercept: Get agent's sensory input
            getAgentBody() -> AgentBody: Get agent's physical state
            addObject(obj) -> None: Add object to world
            removeObject(x, y) -> None: Remove object at position
            isValidPosition(x, y) -> bool: Check if position is valid
            getWidth() -> int: Get world width
            getHeight() -> int: Get world height
            getMaxEnergy() -> float: Get maximum energy
            setMaxEnergy(e) -> None: Set maximum energy
            getEnergyDecayRate() -> float: Get energy decay rate
            setEnergyDecayRate(r) -> None: Set energy decay rate
            getSimulationTime() -> float: Get current simulation time
            setRandomSeed(seed) -> None: Set random seed
            getRandomSeed() -> int: Get random seed
        
        Example:
            >>> world = SimpleWorld()
            >>> world.configure(20, 20, 8, 8)
            >>> world.reset()
            >>> world.setAgentStart(10, 10)
            >>> world.update(0.1)
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure,
             py::arg("width"), py::arg("height"),
             py::arg("visionWidth"), py::arg("visionHeight"),
             "Configure the world dimensions and agent vision\n\n"
             "Args:\n"
             "    width: World width in cells\n"
             "    height: World height in cells\n"
             "    visionWidth: Width of agent's visual field\n"
             "    visionHeight: Height of agent's visual field")
        .def("reset", &SimpleWorld::reset,
             "Reset the world to its initial state")
        .def("setAgentStart", &SimpleWorld::setAgentStart,
             py::arg("x"), py::arg("y"),
             "Set the agent's starting position\n\n"
             "Args:\n"
             "    x: X coordinate\n"
             "    y: Y coordinate")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world physics and state\n\n"
             "Args:\n"
             "    timestep: Time step duration in seconds")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
             py::arg("cmd"), py::arg("currentTime"),
             "Apply a motor command to the world\n\n"
             "Args:\n"
             "    cmd: Motor command to apply\n"
             "    currentTime: Current simulation time\n\n"
             "Returns:\n"
             "    ActionResult: Result of the action")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
             "Get the agent's sensory percept from the current world state\n\n"
             "Returns:\n"
             "    SensoryPercept: Complete sensory input")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
             "Get the agent's current physical state\n\n"
             "Returns:\n"
             "    AgentBody: Agent's position, velocity, and state")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add a world object\n\n"
             "Args:\n"
             "    obj: World object to add")
        .def("removeObject", &SimpleWorld::removeObject,
             py::arg("x"), py::arg("y"),
             "Remove an object from the world\n\n"
             "Args:\n"
             "    x: X coordinate\n"
             "    y: Y coordinate")
        .def("isValidPosition", &SimpleWorld::isValidPosition,
             py::arg("x"), py::arg("y"),
             "Check if a position is valid (within bounds and not occupied)\n\n"
             "Args:\n"
             "    x: X coordinate\n"
             "    y: Y coordinate\n\n"
             "Returns:\n"
             "    bool: True if position is valid")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get the world width\n\n"
             "Returns:\n"
             "    int: World width in cells")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get the world height\n\n"
             "Returns:\n"
             "    int: World height in cells")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get the maximum energy level\n\n"
             "Returns:\n"
             "    float: Maximum energy")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set the maximum energy level\n\n"
             "Args:\n"
             "    e: Maximum energy value")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get the energy decay rate\n\n"
             "Returns:\n"
             "    float: Energy decay rate")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set the energy decay rate\n\n"
             "Args:\n"
             "    r: Energy decay rate")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get the current simulation time\n\n"
             "Returns:\n"
             "    float: Current simulation time in seconds")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set the random seed for reproducible simulations\n\n"
             "Args:\n"
             "    seed: Random seed value")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get the current random seed\n\n"
             "Returns:\n"
             "    int: Current random seed");

    py::class_<Brain>(m, "Brain", R"pbdoc(Central neural simulation brain class
        
        The Brain class represents the core neural system in NLM, implementing real spiking
        neural computation with event-driven dynamics and integrated memory, prediction,
        cognition, and neuromodulation systems.
        
        The brain simulates biological neural activity using Leaky Integrate-and-Fire (LIF)
        neurons, with spike-based communication and plastic synaptic connections.
        
        Attributes:
            config: Configuration object for this brain
            neuron_count: Total number of neurons in the brain
            synapse_count: Total number of synapses in the brain
            firing_neuron_count: Number of currently firing neurons
            average_firing_rate: Average firing rate across all neurons
            total_spike_count: Total number of spikes generated
            developmental_stage: Current developmental stage
        
        Methods:
            __init__(config) -> None
            initialize() -> None: Initialize the brain
            step(step, time=0.0) -> None: Perform simulation step
            receiveSensoryInput(input) -> None: Inject sensory input
            injectCurrent(neuron, current) -> None: Inject current to specific neuron
            injectCurrentToNeurons(type, current) -> None: Inject to neuron type
            produceAction() -> Action: Produce motor action
            reset() -> None: Reset brain state
            save(filepath) -> bool: Save brain state to file
            load(filepath) -> bool: Load brain state from file
            addRegion(name="") -> RegionId: Add neural region
            getRegion(id) -> NeuralRegion: Get region by ID
            getRegionCount() -> int: Get number of regions
            getRegionIds() -> List[RegionId]: Get all region IDs
            getTotalNeuronCount() -> int: Get total neuron count
            getTotalSynapseCount() -> int: Get total synapse count
            getActiveNeuronCount() -> int: Get active neuron count
            getFiringNeuronCount() -> int: Get firing neuron count
            getAverageFiringRate() -> float: Get average firing rate
            getExcitationInhibitionRatio() -> float: Get E/I balance
            getTotalSpikeCount() -> int: Get total spike count
            getDevelopmentalStage() -> DevelopmentalStage: Get developmental stage
            setDevelopmentalStage(stage) -> None: Set developmental stage
            getConfig() -> Config: Get brain configuration
            logStatus() -> None: Log brain status and statistics
        
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> brain.step(0)
            >>> print(f"Brain has {brain.neuron_count} neurons")
            >>> print(f"Spike rate: {brain.average_firing_rate:.2f} Hz")
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"),
             "Create a new brain with configuration\n\n"
             "Args:\n"
             "    config: Configuration object for the brain\n\n"
             "Returns:\n"
             "    Brain instance ready for initialization")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration\n\n"
             "This method must be called before any simulation steps.\n\n"
             "Raises:\n"
             "    RuntimeError: If brain initialization fails")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step\n\n"
             "Args:\n"
             "    currentStep: Current simulation step number")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime") = 0.0,
             "Perform a simulation step with timestamp\n\n"
             "Args:\n"
             "    currentStep: Current simulation step number\n"
             "    currentTime: Current simulation time (default: 0.0)")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput, py::arg("input"),
             "Inject sensory input into the brain\n\n"
             "Args:\n"
             "    input: Sensory input to inject\n\n"
             "Raises:\n"
             "    RuntimeError: If input is invalid")
        .def("injectCurrent", &Brain::injectCurrent, py::arg("neuron"), py::arg("current"),
             "Inject current directly into a specific neuron\n\n"
             "Args:\n"
             "    neuron: Target neuron ID\n"
             "    current: Current amount to inject (positive: excitatory, negative: inhibitory)")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type\n\n"
             "Args:\n"
             "    type: Neuron type (e.g., NeuronType.Sensory, NeuronType.Internal)\n"
             "    current: Current amount to inject")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity\n\n"
             "Returns:\n"
             "    Action: Motor action based on neural activity\n\n"
             "Raises:\n"
             "    RuntimeError: If no action can be produced")
        .def("reset", &Brain::reset,
             "Reset brain state to initial conditions")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file\n\n"
             "Args:\n"
             "    filepath: Path to save file\n\n"
             "Returns:\n"
             "    bool: True if save successful, False otherwise")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file\n\n"
             "Args:\n"
             "    filepath: Path to load file\n\n"
             "Returns:\n"
             "    bool: True if load successful, False otherwise")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region to the brain\n\n"
             "Args:\n"
             "    name: Optional name for the region\n\n"
             "Returns:\n"
             "    RegionId: ID of the newly created region")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a neural region by ID\n\n"
             "Args:\n"
             "    id: Region ID to retrieve\n\n"
             "Returns:\n"
             "    NeuralRegion: The requested region")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of neural regions\n\n"
             "Returns:\n"
             "    int: Number of regions")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs\n\n"
             "Returns:\n"
             "    List[RegionId]: All region IDs")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions\n\n"
             "Returns:\n"
             "    int: Total number of neurons")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions\n\n"
             "Returns:\n"
             "    int: Total number of synapses")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons\n\n"
             "Returns:\n"
             "    int: Number of active neurons")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons\n\n"
             "Returns:\n"
             "    int: Number of firing neurons")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons\n\n"
             "Returns:\n"
             "    float: Average firing rate in Hz")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio\n\n"
             "Returns:\n"
             "    float: Excitation/inhibition ratio (higher = more excitation)")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count\n\n"
             "Returns:\n"
             "    int: Total number of spikes generated")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage\n\n"
             "Returns:\n"
             "    DevelopmentalStage: Current developmental stage")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage\n\n"
             "Args:\n"
             "    stage: Developmental stage to set")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration for this brain\n\n"
             "Returns:\n"
             "    Config: Brain configuration object")
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics to console")
        // Python property accessors
        .def_property_readonly("neuron_count", &Brain::getTotalNeuronCount,
                              "Total number of neurons in the brain")
        .def_property_readonly("synapse_count", &Brain::getTotalSynapseCount,
                              "Total number of synapses in the brain")
        .def_property_readonly("firing_neuron_count", &Brain::getFiringNeuronCount,
                              "Number of currently firing neurons")
        .def_property_readonly("average_firing_rate", &Brain::getAverageFiringRate,
                              "Average firing rate across all neurons (Hz)")
        .def_property_readonly("total_spike_count", &Brain::getTotalSpikeCount,
                              "Total number of spikes generated")
        .def_property_readonly("developmental_stage", &Brain::getDevelopmentalStage,
                              "Current developmental stage");

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world
        
        AgentBrain serves as the interface between the NLM brain and the external world,
        handling sensory processing, action decoding, and neuromodulation. It connects
        the neural system to environmental interaction, enabling goal-directed behavior.
        
        This class manages the complete perception-action loop:
        1. Receives sensory input from the world
        2. Processes input through the brain
        3. Decodes motor commands from brain activity
        4. Applies reward and updates learning
        
        Attributes:
            brain: The underlying NLM brain
            curiosity_level: Current curiosity (exploration motivation)
            novelty_level: Current novelty detection level
            neuromodulation_level: Current neuromodulation strength
            prediction_error: Current prediction error signal
        
        Methods:
            __init__(brain) -> None
            initialize(world) -> None: Initialize with world
            getSensoryInputSize() -> int: Get expected sensory input size
            getMotorOutputSize() -> int: Get expected motor output size
            processSensoryInput(percept) -> None: Process sensory input
            decodeMotorCommand() -> MotorCommand: Decode action from brain
            applyRewardModulation(reward, predictedReward) -> None: Apply reward learning
            updateDevelopment(timestep) -> None: Update developmental system
            getDevelopmentalStage() -> DevelopmentalStage: Get developmental stage
            getNeuromodulationLevel() -> float: Get neuromodulation level
            getCuriosityLevel() -> float: Get curiosity level
            getNoveltyLevel() -> float: Get novelty level
            getPredictionError() -> float: Get prediction error
            reset() -> None: Reset agent for new episode
            getBrain() -> Brain: Get the underlying brain
            enableRewardModulation(enable) -> None: Enable/disable reward learning
            enableStructuralPlasticity(enable) -> None: Enable/disable structural plasticity
            enableDevelopment(enable) -> None: Enable/disable development
            enableCuriosity(enable) -> None: Enable/disable curiosity
            isRewardModulationEnabled() -> bool: Check if reward modulation enabled
            isStructuralPlasticityEnabled() -> bool: Check if structural plasticity enabled
            isDevelopmentEnabled() -> bool: Check if development enabled
            isCuriosityEnabled() -> bool: Check if curiosity enabled
        
        Example:
            >>> config = pynlm.createDefaultConfig()
            >>> brain = pynlm.createBrain(config)
            >>> brain.initialize()
            >>> world = pynlm.createSimpleWorld()
            >>> world.configure(20, 20, 8, 8)
            >>> agent = pynlm.createAgentBrain(brain)
            >>> agent.initialize(world)
            >>> agent.enableRewardModulation(True)
            >>> agent.enableCuriosity(True)
        
        Complete simulation loop:
            >>> for step in range(100):
            ...     world.update(0.1)
            ...     percept = world.getSensoryPercept()
            ...     agent.processSensoryInput(percept)
            ...     brain.step(step)
            ...     action = agent.decodeMotorCommand()
            ...     world.applyMotorCommand(action, world.getSimulationTime())
            ...     reward = world.computeReward(brain.getRegions()[0].get())
            ...     agent.applyRewardModulation(reward, 0.0)
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Create an agent brain interface\n\n"
             "Args:\n"
             "    brain: The underlying NLM brain to interface with")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize the agent brain with a world\n\n"
             "Args:\n"
             "    world: World instance for interaction")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size\n\n"
             "Returns:\n"
             "    int: Expected sensory input size")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size\n\n"
             "Returns:\n"
             "    int: Expected motor output size")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain\n\n"
             "Args:\n"
             "    percept: Sensory percept to process")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command\n\n"
             "Returns:\n"
             "    MotorCommand: Motor command based on brain activity")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
             py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation\n\n"
             "Args:\n"
             "    reward: Actual reward received\n"
             "    predictedReward: Predicted reward\n\n"
             "Returns:\n"
             "    float: Update in neuromodulation level")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
             "Update development system\n\n"
             "Args:\n"
             "    timestep: Time step duration")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage\n\n"
             "Returns:\n"
             "    DevelopmentalStage: Current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level\n\n"
             "Returns:\n"
             "    float: Current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level\n\n"
             "Returns:\n"
             "    float: Current curiosity level (exploration motivation)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level\n\n"
             "Returns:\n"
             "    float: Current novelty detection level")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error\n\n"
             "Returns:\n"
             "    float: Current prediction error")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             py::return_value_policy::reference_internal,
             "Get the underlying brain\n\n"
             "Returns:\n"
             "    Brain: The underlying NLM brain")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             py::arg("enable"),
             "Enable or disable reward modulation\n\n"
             "Args:\n"
             "    enable: Whether to enable reward modulation")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             py::arg("enable"),
             "Enable or disable structural plasticity\n\n"
             "Args:\n"
             "    enable: Whether to enable structural plasticity")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             py::arg("enable"),
             "Enable or disable development\n\n"
             "Args:\n"
             "    enable: Whether to enable development")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             py::arg("enable"),
             "Enable or disable curiosity-driven exploration\n\n"
             "Args:\n"
             "    enable: Whether to enable curiosity")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled\n\n"
             "Returns:\n"
             "    bool: True if reward modulation enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled\n\n"
             "Returns:\n"
             "    bool: True if structural plasticity enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled\n\n"
             "Returns:\n"
             "    bool: True if development enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled\n\n"
             "Returns:\n"
             "    bool: True if curiosity enabled")
        // Python property accessors
        .def_property_readonly("curiosity_level", &AgentBrain::getCuriosityLevel,
                              "Current curiosity level (exploration motivation)")
        .def_property_readonly("novelty_level", &AgentBrain::getNoveltyLevel,
                              "Current novelty detection level")
        .def_property_readonly("neuromodulation_level", &AgentBrain::getNeuromodulationLevel,
                              "Current neuromodulation level")
        .def_property_readonly("prediction_error", &AgentBrain::getPredictionError,
                              "Current prediction error signal")
        .def_property_readonly("developmental_stage", &AgentBrain::getDevelopmentalStage,
                              "Current developmental stage");

    // Global convenience functions for common patterns
    
    m.def("createQuickBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        if (!config) {
            config = std::make_shared<Config>();
            config->set("neuron_count", 500);
            config->set("region_count", 1);
            config->set("connection_probability", 0.05);
            config->set("stdp_ltp_weight", 0.01f);
            config->set("stdp_ltd_weight", 0.012f);
            config->set("synaptogenesis_rate", 0.0001f);
            config->set("pruning_rate", 0.00001f);
        }
        
        auto brain = std::make_shared<Brain>(config);
        brain->initialize();
        return brain;
    }, py::arg("config") = nullptr,
       "Create a brain with sensible defaults for quick testing\n\n"
       "Args:\n"
       "    config: Optional configuration (uses defaults if not provided)\n\n"
       "Returns:\n"
       "    Brain: Brain instance ready for simulation");

    m.def("createAgentSetup", [](std::shared_ptr<SimpleWorld> world, 
                                 std::shared_ptr<Config> config) {
        auto brain = createQuickBrain(config);
        auto agent = std::make_shared<AgentBrain>(brain);
        agent->initialize(world);
        agent->enableRewardModulation(true);
        agent->enableCuriosity(true);
        return std::make_tuple(brain, agent);
    }, py::arg("world"), py::arg("config") = nullptr,
       "Create a complete agent setup with world for simulation\n\n"
       "Args:\n"
       "    world: SimpleWorld instance for the agent to interact with\n"
       "    config: Optional brain configuration (uses defaults if not provided)\n\n"
       "Returns:\n"
       "    Tuple of (brain, agent) ready for simulation");

    m.def("run_episode", [](std::shared_ptr<Brain> brain,
                           std::shared_ptr<AgentBrain> agent,
                           std::shared_ptr<SimpleWorld> world,
                           int num_steps,
                           bool verbose) {
        world->reset();
        agent->reset();
        
        float total_reward = 0.0f;
        size_t max_firing = 0;
        
        for (int step = 0; step < num_steps; ++step) {
            world->update(0.1f);
            
            auto percept = world->getSensoryPercept();
            agent->processSensoryInput(percept);
            brain->step(step, world->getSimulationTime());
            
            auto action = agent->decodeMotorCommand();
            world->applyMotorCommand(action, world->getSimulationTime());
            
            float reward = world->computeReward(agent->getBrain()->getRegions()[0].get());
            total_reward += reward;
            agent->applyRewardModulation(reward, 0.0f);
            
            if (verbose && step % 50 == 0) {
                std::cout << "Step " << step << ": "
                         << "Reward: " << (total_reward / (step + 1))
                         << " | Firing: " << brain->getAverageFiringRate()
                         << " | Curios: " << agent->getCuriosityLevel()
                         << std::endl;
            }
            
            max_firing = std::max(max_firing, brain->getFiringNeuronCount());
        }
        
        return std::make_tuple(total_reward / num_steps, max_firing);
    }, py::arg("brain"), py::arg("agent"), py::arg("world"), 
          py::arg("num_steps") = 100, py::arg("verbose") = false,
       "Run a complete simulation episode\n\n"
       "Args:\n"
       "    brain: The neural brain\n"
       "    agent: The agent interface\n"
       "    world: The environment\n"
       "    num_steps: Number of simulation steps\n"
       "    verbose: Print progress information\n\n"
       "Returns:\n"
       "    Tuple of (average_reward, max_firing_neurons)");

} // namespace nlm
