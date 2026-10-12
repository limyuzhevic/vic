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

namespace py = pybind11;
namespace nlm {

/**
 * @brief Comprehensive Python bindings for NLM (Neural Learning Machine)
 * 
 * This module provides a complete Python interface to the NLM C++ neural simulation framework.
 * It includes all major components: brain simulation, configuration management, world interaction,
 * sensory processing, motor control, and agent management.
 * 
 * Key features:
 * - Full access to neural simulation capabilities
 * - Comprehensive error handling and validation
 * - Type-safe Python API with extensive documentation
 * - Integration with standard Python ecosystem
 * 
 * Usage examples:
 * 
 * ```python
 * import pynlm
 * 
 * # Create a brain with default configuration
 * brain = pynlm.createBrain(pynlm.createDefaultConfig())
 * 
 * # Initialize and run simulation
 * brain.initialize()
 * for i in range(100):
 *     brain.step(i)
 * 
 * print(f"Simulation completed with {brain.getTotalSpikeCount()} spikes")
 * ```
 */

/**
 * @brief Enhanced ID class bindings with comprehensive documentation
 * 
 * Template function to create robust ID class bindings for all NLM identifier types.
 * Provides consistent behavior across NeuronId, SynapseId, RegionId, and PopulationId.
 * 
 * @tparam IDType The identifier type to bind
 * @param m The Python module to bind to
 * @param name The name of the Python class
 * @param doc The documentation string for the class
 */
template<typename IDType>
void bindIdClass(py::module_& m, const std::string& name, const std::string& doc) {
    py::class_<IDType>(m, name.c_str(), py::doc(doc.c_str()))
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &IDType::value)
        .def("index", &IDType::index)
        .def("__eq__", &IDType::operator==)
        .def("__ne__", &IDType::operator!=)
        .def("__hash__", [](const IDType& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const IDType& id) {
            return "<" + name + ": " + std::to_string(id.value) + ">";
        })
        .doc() = doc.c_str();
}

/**
 * @brief Enhanced enum bindings with comprehensive documentation
 * 
 * Template function to create robust enum bindings for all NLM enumeration types.
 * Provides consistent behavior across all enumeration types with proper documentation.
 * 
 * @tparam EnumType The enumeration type to bind
 * @param m The Python module to bind to
 * @param name The name of the Python enum
 * @param doc The documentation string for the enum
 * @param values Vector of enum value pairs for documentation
 */
template<typename EnumType>
void bindEnumClass(py::module_& m, const std::string& name, const std::string& doc, 
                  const std::vector<std::pair<std::string, EnumType>>& values) {
    py::enum_<EnumType> enumClass = py::enum_<EnumType>(m, name.c_str(), doc.c_str());
    
    // Add each enum value with proper documentation
    for (const auto& pair : values) {
        enumClass.value(pair.first.c_str(), pair.second);
    }
    
    enumClass.export_values_from_enum();
}

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (Neural Learning Machine) Python Bindings
        -----------------------------------------------
        
        A complete Python interface to the NLM C++ neural simulation framework.
        
        This module provides access to the full NLM brain simulation capabilities,
        including neural computation, learning, memory, and interaction with simulated environments.
        
        Key Components:
        - Brain simulation with spiking neurons and synapses
        - Configuration management for all system parameters
        - World simulation for agent-environment interaction
        - Sensory processing (vision, touch, audio, internal signals)
        - Motor control and action execution
        - Agent brain interface for integration
        - Neuromodulation and learning systems
        
        The API is designed to be intuitive and Pythonic while maintaining full access
        to NLM's powerful neural simulation capabilities.
        
        Examples:
        
        ```python
        import pynlm
        
        # Basic brain simulation
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        for i in range(100):
            brain.step(i)
        
        print(f"Total spikes: {brain.getTotalSpikeCount()}")
        
        # Advanced agent simulation
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning capabilities
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        print(f"Simulation completed: {brain.getFiringNeuronCount()} neurons firing")
        ```
    )pbdoc";

    try {
        py::register_exception<std::runtime_error>(m, "RuntimeError");
        
        // Bind ID classes using template function
        bindIdClass<NeuronId>(m, "NeuronId", 
            "Unique identifier for a neuron in the NLM brain system.\n\n"
            "Each neuron has a unique ID that can be used to reference it in the brain.\n\n"
            "Attributes:\n"
            "    value: The unique numeric identifier (uint64_t)");
        
        bindIdClass<SynapseId>(m, "SynapseId",
            "Unique identifier for a synapse in the NLM brain system.\n\n"
            "Each synapse has a unique ID that can be used to reference it in the brain.\n\n"
            "Attributes:\n"
            "    value: The unique numeric identifier (uint64_t)");
        
        bindIdClass<RegionId>(m, "RegionId",
            "Unique identifier for a brain region in the NLM system.\n\n"
            "Brain regions contain groups of neurons and synapses.\n\n"
            "Attributes:\n"
            "    value: The unique numeric identifier (uint64_t)");
        
        bindIdClass<PopulationId>(m, "PopulationId",
            "Unique identifier for a neuron population in the NLM system.\n\n"
            "Neurons with the same population ID belong to the same functional group.\n\n"
            "Attributes:\n"
            "    value: The unique numeric identifier (uint64_t)");
        
        // Bind enum classes using template function with comprehensive documentation
        const std::vector<std::pair<std::string, NeuronType>> neuronTypes = {
            {"Excitatory", NeuronType::Excitatory, "Neuron that increases the likelihood of other neurons firing"},
            {"Inhibitory", NeuronType::Inhibitory, "Neuron that decreases the likelihood of other neurons firing"},
            {"Modulatory", NeuronType::Modulatory, "Neuron that influences other neurons' plasticity and excitability"},
            {"Sensory", NeuronType::Sensory, "Neuron that processes external sensory input"},
            {"Motor", NeuronType::Motor, "Neuron that controls movement and action execution"},
            {"Internal", NeuronType::Internal, "Neuron that maintains internal state and homeostasis"}
        };
        bindEnumClass<NeuronType>(m, "NeuronType",
            "Enumeration of neuron types in the NLM system.\n\n"
            "Neurons are classified by their functional role in the brain:\n\n"
            "    Excitatory: Promotes neural firing\n"
            "    Inhibitory: Suppresses neural firing\n"
            "    Modulatory: Affects plasticity and excitability\n"
            "    Sensory: Processes external input\n"
            "    Motor: Controls output actions\n"
            "    Internal: Maintains internal state", neuronTypes);
        
        const std::vector<std::pair<std::string, SynapseType>> synapseTypes = {
            {"Excitatory", SynapseType::Excitatory, "Synapse that increases postsynaptic neuron firing"},
            {"Inhibitory", SynapseType::Inhibitory, "Synapse that decreases postsynaptic neuron firing"},
            {"Modulatory", SynapseType::Modulatory, "Synapse that influences plasticity and excitability"},
            {"Electrical", SynapseType::Electrical, "Direct electrical connection between neurons"},
            {"GapJunction", SynapseType::GapJunction, "Electrical synapse through gap junctions"}
        };
        bindEnumClass<SynapseType>(m, "SynapseType",
            "Enumeration of synapse types in the NLM system.\n\n"
            "Synapses are classified by their functional role:\n\n"
            "    Excitatory: Increases postsynaptic firing\n"
            "    Inhibitory: Decreases postsynaptic firing\n"
            "    Modulatory: Affects plasticity and excitability\n"
            "    Electrical: Direct electrical connection\n"
            "    GapJunction: Electrical coupling through gap junctions", synapseTypes);
        
        const std::vector<std::pair<std::string, DevelopmentalStage>> developmentalStages = {
            {"Initial", DevelopmentalStage::Initial, "Early developmental stage with high plasticity"},
            {"CriticalPeriod", DevelopmentalStage::CriticalPeriod, "Sensitive period for learning"},
            {"Maturation", DevelopmentalStage::Maturation, "Brain maturation phase"},
            {"Adult", DevelopmentalStage::Adult, "Mature, stable brain state"},
            {"Aging", DevelopmentalStage::Aging, "Age-related decline phase"}
        };
        bindEnumClass<DevelopmentalStage>(m, "DevelopmentalStage",
            "Enumeration of developmental stages in the NLM system.\n\n"
            "The brain undergoes developmental stages that affect its learning capabilities:\n\n"
            "    Initial: High plasticity, rapid learning\n"
            "    CriticalPeriod: Sensitive learning window\n"
            "    Maturation: Brain development and refinement\n"
            "    Adult: Mature, stable function\n"
            "    Aging: Age-related changes and decline", developmentalStages);
        
        const std::vector<std::pair<std::string, FiringState>> firingStates = {
            {"Resting", FiringState::Resting, "Neuron is not firing"},
            {"Active", FiringState::Active, "Neuron is currently firing spikes"},
            {"Refractory", FiringState::Refractory, "Neuron is in refractory period after firing"},
            {"Inhibited", FiringState::Inhibited, "Neuron is being inhibited by other neurons"}
        };
        bindEnumClass<FiringState>(m, "FiringState",
            "Enumeration of neuron firing states.\n\n"
            "Neurons can be in one of these states at any given time:\n\n"
            "    Resting: Not firing, membrane potential near resting level\n"
            "    Active: Currently firing action potentials\n"
            "    Refractory: Cannot fire immediately after spike\n"
            "    Inhibited: Actively suppressed by inhibitory inputs", firingStates);
        
        const std::vector<std::pair<std::string, ActionType>> actionTypes = {
            {"MoveForward", ActionType::MoveForward, "Move the agent forward"},
            {"MoveBackward", ActionType::MoveBackward, "Move the agent backward"},
            {"MoveLeft", ActionType::MoveLeft, "Move the agent left"},
            {"MoveRight", ActionType::MoveRight, "Move the agent right"},
            {"TurnLeft", ActionType::TurnLeft, "Turn the agent left"},
            {"TurnRight", ActionType::TurnRight, "Turn the agent right"},
            {"Look", ActionType::Look, "Look in the direction the agent is facing"},
            {"LookUp", ActionType::LookUp, "Look upward"},
            {"LookDown", ActionType::LookDown, "Look downward"},
            {"Interact", ActionType::Interact, "Interact with objects in the world"},
            {"Eat", ActionType::Eat, "Eat food resources"},
            {"Drink", ActionType::Drink, "Drink water resources"},
            {"Rest", ActionType::Rest, "Rest and recover energy"},
            {"Wait", ActionType::Wait, "Do nothing, wait for next step"},
            {"Custom", ActionType::Custom, "Custom action defined by the user"}
        };
        bindEnumClass<ActionType>(m, "ActionType",
            "Enumeration of possible actions an agent can take.\n\n"
            "Actions define what an agent does in the world:\n\n"
            "    MoveForward: Move forward one unit\n"
            "    MoveBackward: Move backward one unit\n"
            "    MoveLeft: Move left one unit\n"
            "    MoveRight: Move right one unit\n"
            "    TurnLeft: Rotate left 90 degrees\n"
            "    TurnRight: Rotate right 90 degrees\n"
            "    Look: Look in current direction\n"
            "    LookUp: Look upward\n"
            "    LookDown: Look downward\n"
            "    Interact: Interact with nearby objects\n"
            "    Eat: Consume food resources\n"
            "    Drink: Consume water resources\n"
            "    Rest: Rest and recover energy\n"
            "    Wait: Do nothing\n"
            "    Custom: User-defined action", actionTypes);
        
        const std::vector<std::pair<std::string, MotorCommand>> motorCommands = {
            {"MoveForward", MotorCommand::MoveForward, "Move forward action"},
            {"MoveBackward", MotorCommand::MoveBackward, "Move backward action"},
            {"TurnLeft", MotorCommand::TurnLeft, "Turn left action"},
            {"TurnRight", MotorCommand::TurnRight, "Turn right action"},
            {"LookLeft", MotorCommand::LookLeft, "Look left action"},
            {"LookRight", MotorCommand::LookRight, "Look right action"},
            {"Interact", MotorCommand::Interact, "Interact action"},
            {"Wait", MotorCommand::Wait, "Wait action (no movement)"}
        };
        bindEnumClass<MotorCommand>(m, "MotorCommand",
            "Enumeration of motor commands produced by the brain.\n\n"
            "Motor commands are the output actions selected by the brain:\n\n"
            "    MoveForward: Move forward\n"
            "    MoveBackward: Move backward\n"
            "    TurnLeft: Turn left\n"
            "    TurnRight: Turn right\n"
            "    LookLeft: Look left\n"
            "    LookRight: Look right\n"
            "    Interact: Interact with objects\n"
            "    Wait: Do nothing", motorCommands);
        
        const std::vector<std::pair<std::string, WorldObjectType>> worldObjectTypes = {
            {"Empty", WorldObjectType::Empty, "Empty space with no object"},
            {"Resource", WorldObjectType::Resource, "A resource that can be collected"},
            {"Hazard", WorldObjectType::Hazard, "A hazardous object that causes damage"},
            {"Wall", WorldObjectType::Wall, "An obstacle that blocks movement"},
            {"Marker", WorldObjectType::Marker, "A marker object with special properties"}
        };
        bindEnumClass<WorldObjectType>(m, "WorldObjectType",
            "Enumeration of world object types.\n\n"
            "Objects in the simulated world can be of different types:\n\n"
            "    Empty: No object\n"
            "    Resource: Collectible resource\n"
            "    Hazard: Dangerous object\n"
            "    Wall: Obstacle\n"
            "    Marker: Special-purpose object", worldObjectTypes);
        
        // Enhanced Config class with comprehensive documentation and examples
        py::class_<Config>(m, "Config", 
            "Configuration class for NLM system.\n\n"
            "The Config class manages all system parameters and settings.\n\n"
            "Key Features:\n"
            "    - Type-safe access to configuration values\n"
            "    - Support for multiple value types (int, double, string, bool, vectors)\n"
            "    - Robust error handling and validation\n"
            "    - File-based configuration loading\n"
            "    - Command-line argument integration\n"
            "    - Summary and debugging capabilities\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    
            "    # Create default configuration\n"
            "    config = pynlm.createDefaultConfig()\n"
            "    \n            "    # Set configuration values\n"
            "    config.set('neuron_count', 1000)\n"
            "    config.set('random_seed', 42)\n"
            "    config.set('simulation_timestep', 0.001)\n"
            "    \n            "    # Get configuration values\n"
            "    neuron_count = config.getOr('neuron_count', 500)\n"
            "    print(f'Neurons: {neuron_count}')\n"
            "    ```\n\n"
            "See the ConfigSource enum for different configuration sources.\n"
        )
            .def(py::init<>())
            .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
                 "Load configuration from a JSON file.\n\n"
                 "Args:\n"
                 "    filepath: Path to the configuration file\n\n"
                 "Returns:\n"
                 "    True if configuration was loaded successfully, False otherwise\n\n"
                 "The file should be in key=value format, with support for:\n"
                 "    - Comments starting with # or /\n"
                 "    - Quoted string values\n"
                 "    - Basic type inference (int, double, string, bool)")
            .def("loadFromArgs", [](Config& self, int argc, char** argv) {
                return self.loadFromArgs(argc, argv);
            }, py::arg("argc"), py::arg("argv"),
                "Load configuration from command line arguments.\n\n"
                "Args:\n"
                "    argc: Number of command line arguments\n"
                "    argv: Array of argument strings\n\n"
                "Supports both --key=value and -key value formats.\n\n"
                "Examples:\n"
                "    --neuron_count=1000\n"
                "    --random_seed=42\n"
                "    -config file.cfg")
            .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
                 "Save configuration to a file.\n\n"
                 "Args:\n"
                 "    filepath: Path where the configuration should be saved\n\n"
                 "Returns:\n"
                 "    True if configuration was saved successfully, False otherwise\n\n"
                 "The saved file can be reloaded using loadFromFile().")
            .def("has", &Config::has, py::arg("key"),
                 "Check if a configuration key exists.\n\n"
                 "Args:\n"
                 "    key: The configuration key to check\n\n"
                 "Returns:\n"
                 "    True if the key exists, False otherwise")
            .def("getKeys", &Config::getKeys,
                 "Get all configuration keys.\n\n"
                 "Returns:\n"
                 "    List of all configuration key names")
            .def("clear", &Config::clear,
                 "Clear all configuration entries.\n\n"
                 "This removes all configuration settings and resets the configuration.")
            .def("summary", &Config::summary,
                 "Get a summary string of the configuration.\n\n"
                 "Returns:\n"
                 "    Formatted summary string showing all configuration entries")
            .def("__repr__", [](const Config& cfg) {
                return "<Config: " + cfg.summary() + ">";
            })
            .doc() = "Configuration management for NLM system.";        
        
        // Enhanced SensoryInput class with comprehensive documentation
        py::class_<SensoryInput>(m, "SensoryInput",
            "Base class for sensory input data.\n\n"
            "SensoryInput represents data received by the agent's sensory systems.
            
            Subclasses include Vision, Audio, and InternalSignals, each providing
            specific types of sensory information for the brain to process.
            
            Attributes:\n"
            "    getType(): Get the type of sensory input\n"
            "    getData(): Get the raw data as a vector\n"
            "    getDimensions(): Get the dimensionality of the data\n"
            "    getTimestamp(): Get the timestamp of the data\n"
            "    setTimestamp(): Set the timestamp of the data\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    
            "    # Get sensory percept from world\n"
            "    percept = world.getSensoryPercept()\n"
            "    \n            "    # Access different sensory modalities\n"
            "    vision = percept.getVision()\n"
            "    touch = percept.getTouch()\n"
            "    audio = percept.getAudio()\n"
            "    ```\n\n"
            "The agent uses sensory input to perceive its environment and guide its actions."
        )
            .def("getType", &SensoryInput::getType, "Get the type of sensory input")
            .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
            .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
            .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
            .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
                 "Set the timestamp");
        
        // Enhanced Vision class with comprehensive documentation
        py::class_<Vision, SensoryInput>(m, "Vision",
            "Vision sensory input representing a 2D visual field.\n\n"
            "Vision provides a grid of intensity values representing what the agent sees.\n\n"
            "Attributes:\n"
            "    width: The width of the vision grid in pixels\n"
            "    height: The height of the vision grid in pixels\n"
            "    channels: The number of color channels (typically 3 for RGB)\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create a vision input (8x8 grayscale)\n"
            "    vision = Vision(8, 8, 1)\n"
            "    \n            "    # Set vision data\n"
            "    data = [0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8]  # Row-major\n"
            "    vision.setData(data)\n"
            "    \n            "    # Access vision properties\n"
            "    width = vision.getWidth()\n"
            "    height = vision.getHeight()\n"
            "    ```\n\n"
            "Vision input is used by the brain to perceive the visual environment."
        )
            .def(py::init<>())
            .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
                 py::arg("channels") = 3)
            .def("setData", [](Vision& self, const std::vector<float>& data) {
                if (data.empty()) {
                    throw std::invalid_argument("Vision data cannot be empty");
                }
                self.setData(data);
            }, py::arg("data"))
            .def("getWidth", &Vision::getWidth)
            .def("getHeight", &Vision::getHeight)
            .def("getChannels", &Vision::getChannels);
        
        // Enhanced Audio class with comprehensive documentation
        py::class_<Audio, SensoryInput>(m, "Audio",
            "Audio sensory input representing sound signals.\n\n"
            "Audio provides a sequence of sound samples that can be processed by the brain.\n\n"
            "Attributes:\n"
            "    sampleRate: The number of samples per second\n"
            "    numSamples: The number of audio samples\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create audio input\n"
            "    audio = Audio(44100, 1024)  # 1 second of audio at 44.1kHz\n"
            "    \n            "    # Set audio data\n"
            "    samples = [0.0, 0.5, 1.0, 0.5, 0.0, -0.5, -1.0, -0.5] * 128\n"
            "    audio.setData(samples)\n"
            "    \n            "    # Access audio properties\n"
            "    sample_rate = audio.getSampleRate()\n"
            "    num_samples = audio.getNumSamples()\n"
            "    ```\n\n"
            "Audio input allows agents to perceive sound in their environment."
        )
            "Audio sensory input with error handling and comprehensive documentation"
            .def(py::init<>())
            .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
            .def("setData", [](Audio& self, const std::vector<float>& data) {
                if (data.empty()) {
                    throw std::invalid_argument("Audio data cannot be empty");
                }
                self.setData(data);
            }, py::arg("data"))
            .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
            .def("getSampleRate", &Audio::getSampleRate)
            .def("getNumSamples", &Audio::getNumSamples);
        
        // Enhanced InternalSignals class
        py::class_<InternalSignals, SensoryInput>(m, "InternalSignals",
            "Internal signals sensory input for agent's internal state.\n\n"
            "InternalSignals represents the agent's internal state, such as hunger,
            energy levels, or other biological states that influence behavior.\n\n"
            "Methods:\n"
            "    addSignal(value): Add a signal value to the internal signals\n"
            "    clearSignals(): Clear all signal values\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create internal signals\n"
            "    signals = InternalSignals()\n"
            "    \n            "    # Add signal values\n"
            "    signals.addSignal(0.7)  # High energy\n"
            "    signals.addSignal(0.3)  # Low hunger\n"
            "    \n            "    # Clear signals\n"
            "    signals.clearSignals()\n"
            "    ```\n\n"
            "Internal signals help the agent maintain homeostasis and respond to needs."
        )
            .def(py::init<>())
            .def("addSignal", &InternalSignals::addSignal, py::arg("value"))
            .def("clearSignals", &InternalSignals::clearSignals);
        
        // Enhanced Action class with comprehensive documentation
        py::class_<Action>(m, "Action",
            "Action representation for motor output.\n\n"
            "Actions represent the agent's intended behavior in the world.\n\n"
            "Attributes:\n"
            "    type: The type of action (ActionType enum)\n"
            "    parameters: Additional action parameters as a vector of floats\n\n"
            "Methods:\n"
            "    getType(): Get the action type\n"
            "    setType(type): Set the action type\n"
            "    getParameters(): Get the action parameters\n"
            "    setParameters(params): Set the action parameters\n"
            "    getName(): Get the human-readable action name\n"
            "    clone(): Create a copy of this action\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create different types of actions\n"
            "    move_forward = Action(ActionType.MoveForward)\n"
            "    turn_left = Action(ActionType.TurnLeft)\n"
            "    look = Action(ActionType.Look)\n"
            "    \n            "    # Actions with parameters\n"
            "    custom_action = Action(ActionType.Custom, [0.5, 0.8, 1.2])\n"
            "    \n            "    # Modify actions\n"
            "    move_forward.setType(ActionType.MoveBackward)\n"
            "    move_forward.setParameters([0.5])  # Half speed\n"
            "    \n            "    # Get action name\n"
            "    name = move_forward.getName()\n"
            "    ```\n\n"
            "Actions are the output of the agent's decision-making process and are
            executed in the world by the agent's motor system."
        )
            .def(py::init<>())
            .def(py::init<ActionType>(), py::arg("type"))
            .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
            .def("getType", &Action::getType)
            .def("setType", &Action::setType, py::arg("type"))
            .def("getParameters", &Action::getParameters)
            .def("setParameters", &Action::setParameters, py::arg("params"))
            .def("getName", &Action::getName)
            .def("clone", &Action::clone);
        
        // Enhanced WorldObject class with comprehensive documentation
        py::class_<WorldObject>(m, "WorldObject",
            "World object representation.\n\n"
            "WorldObject represents entities in the simulated world that the agent can
            interact with. Objects can be resources, hazards, obstacles, markers, or empty space.\n\n"
            "Attributes:\n"
            "    x: X coordinate of the object\n"
            "    y: Y coordinate of the object\n"
            "    radius: Radius of the object (for circular objects)\n"
            "    type: Type of the object (WorldObjectType enum)\n"
            "    value: Additional value (e.g., nutritional value)\n"
            "    active: Whether the object is currently active\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create different types of objects\n"
            "    apple = WorldObject(5.0, 5.0, WorldObjectType.Resource, 10.0, 0.5)\n"
            "    wall = WorldObject(10.0, 10.0, WorldObjectType.Wall, 0.0, 1.0)\n"
            "    hazard = WorldObject(15.0, 15.0, WorldObjectType.Hazard, 5.0, 0.3)\n"
            "    \n            "    # Check object type\n"
            "    if object.type == WorldObjectType.Resource:\n"
            "        print(f'Collectable: {object.value}')\n"
            "    ```\n\n"
            "World objects define the environment in which the agent operates and can be
            collected, avoided, or interacted with by the agent."
        )
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
        
        // Enhanced AgentBody class with validation and documentation
        py::class_<AgentBody>(m, "AgentBody",
            "Agent body state and physical properties.\n\n"
            "AgentBody represents the physical state of the agent in the simulated world,
            including position, orientation, velocity, and physiological states.\n\n"
            "Attributes:\n"
            "    x: X position in world coordinates\n"
            "    y: Y position in world coordinates\n"
            "    orientation: Agent's facing direction in radians\n"
            "    velocityX: X velocity component\n"
            "    velocityY: Y velocity component\n"
            "    angularVelocity: Angular velocity component\n"
            "    energy: Current energy level (0.0 to 1.0)\n"
            "    health: Current health level (0.0 to 1.0)\n"
            "    age: Agent's age in simulation time steps\n"
            "    isMoving: Whether the agent is currently moving\n"
            "    isTurning: Whether the agent is currently turning\n"
            "    lastActionTime: Time since last action was taken\n\n"
            "Methods:\n"
            "    reset(): Reset agent body to initial state\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Access agent body state\n"
            "    body = agent.getAgentBody()\n"
            "    x = body.x\n"
            "    y = body.y\n"
            "    energy = body.energy\n"
            "    health = body.health\n"
            "    \n            "    # Modify agent state\n"
            "    body.x = 10.0\n"
            "    body.energy = 0.8\n"
            "    \n            "    # Reset agent body\n"
            "    body.reset()\n"
            "    ```\n\n"
            "The agent body state tracks the agent's position, movement, and physiological
            conditions, influencing its behavior and capabilities."
        )
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
        
        // Enhanced ActionResult class with comprehensive documentation
        py::class_<ActionResult>(m, "ActionResult",
            "Action result from world interaction.\n\n"
            "ActionResult represents the outcome of an agent's action in the world,
            including reward, success status, and any additional information.\n\n"
            "Attributes:\n"
            "    reward: The reward value received from the action (-1.0 to 1.0)\n"
            "    success: Whether the action was successful\n"
            "    message: Optional message describing the action result\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create action results\n"
            "    success_result = ActionResult(1.0, True, \"Collected food!\")\n"
            "    failure_result = ActionResult(-0.5, False, \"Hit wall!\")\n"
            "    neutral_result = ActionResult(0.0, True, \"Did nothing\")\n"
            "    \n            "    # Check action result\n"
            "    if result.success:\n"
            "        print(f\"Got reward: {result.reward}\")\n"
            "    if result.reward > 0:\n"
            "        print(result.message)\n"
            "    ```\n\n"
            "Action results are used by the agent to learn from its experiences and
            adjust its future behavior through reinforcement learning."
        )
            .def(py::init<>())
            .def(py::init<float, bool, std::string>(),
                 py::arg("reward"), py::arg("success"), py::arg("message") = "")
            .def_readwrite("reward", &ActionResult::reward)
            .def_readwrite("success", &ActionResult::success)
            .def_readwrite("message", &ActionResult::message);
        
        // Enhanced SensoryPercept class with comprehensive documentation
        py::class_<SensoryPercept>(m, "SensoryPercept",
            "Sensory percept data from the world.\n\n"
            "SensoryPercept combines all sensory inputs (vision, touch, audio, internal signals)
            that the agent receives from the world into a single data structure.\n\n"
            "Methods:\n"
            "    getVision(): Get vision data as Vision object\n"
            "    setVision(vision): Set vision data\n"
            "    getVisionWidth(): Get vision grid width\n"
            "    getVisionHeight(): Get vision grid height\n"
            "    getTouch(): Get touch data\n"
            "    setTouch(touch): Set touch data\n"
            "    getInternal(): Get internal signals\n"
            "    setInternal(internal): Set internal signals\n"
            "    getProprioception(): Get proprioception data\n"
            "    setProprioception(proprio): Set proprioception data\n"
            "    getAudio(): Get audio data\n"
            "    setAudio(audio): Set audio data\n"
            "    getAllSignals(): Get all sensory signals as a dict\n"
            "    getTimestamp(): Get the percept timestamp\n"
            "    setTimestamp(timestamp): Set the percept timestamp\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Get sensory percept from world\n"
            "    percept = world.getSensoryPercept()\n"
            "    \n            "    # Access different sensory modalities\n"
            "    vision = percept.getVision()\n"
            "    touch = percept.getTouch()\n"
            "    audio = percept.getAudio()\n"
            "    internal = percept.getInternal()\n"
            "    \n            "    # Modify sensory data\n"
            "    new_vision = Vision(8, 8, 1)\n"
            "    percept.setVision(new_vision)\n"
            "    \n            "    # Get all signals at once\n"
            "    all_signals = percept.getAllSignals()\n"
            "    ```\n\n"
            "SensoryPercept is the interface through which the agent perceives its environment,
            providing the raw data that the agent processes to make decisions and take actions."
        )
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
        
        // Enhanced SimpleWorld class with comprehensive documentation
        py::class_<SimpleWorld>(m, "SimpleWorld",
            "Simple 2D world for NLM simulation.\n\n"
            "SimpleWorld is a simulated environment where the agent interacts with objects
            and experiences consequences of its actions. It provides sensory input,
            motor output, and world dynamics.\n\n"
            "Methods:\n"
            "    configure(width, height, visionWidth, visionHeight): Configure world dimensions\n"
            "    reset(): Reset world to initial state\n"
            "    setAgentStart(x, y): Set agent starting position\n"
            "    update(timestep): Update world dynamics\n"
            "    applyMotorCommand(cmd, time): Apply agent action to world\n"
            "    getSensoryPercept(): Get current sensory input\n"
            "    getAgentBody(): Get current agent state\n"
            "    addObject(obj): Add an object to the world\n"
            "    removeObject(x, y): Remove object at position\n"
            "    isValidPosition(x, y): Check if position is valid\n"
            "    getWidth(): Get world width\n"
            "    getHeight(): Get world height\n"
            "    getMaxEnergy(): Get maximum agent energy\n"
            "    setMaxEnergy(e): Set maximum agent energy\n"
            "    getEnergyDecayRate(): Get energy decay rate\n"
            "    setEnergyDecayRate(r): Set energy decay rate\n"
            "    getSimulationTime(): Get current simulation time\n"
            "    setRandomSeed(seed): Set random seed for reproducibility\n"
            "    getRandomSeed(): Get current random seed\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create and configure world\n"
            "    world = pynlm.createSimpleWorld()\n"
            "    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)\n"
            "    world.reset()\n"
            "    \n            "    # Set agent start position\n"
            "    world.setAgentStart(10, 10)\n"
            "    \n            "# Run simulation loop\n"
            "    for step in range(100):\n"
            "        world.update(0.1)  # Update world\n"
            "        percept = world.getSensoryPercept()  # Get what agent sees\n"
            "        agent.processSensoryInput(percept)  # Brain processes input\n"
            "        brain.step(step)  # Brain thinks\n"
            "        action = agent.decodeMotorCommand()  # Brain decides action\n"
            "        world.applyMotorCommand(action, world.getSimulationTime())  # Do action\n"
            "    ```\n\n"
            "SimpleWorld provides the environment in which the agent operates and learns,
            providing the context for sensory input, motor output, and reward signals."
        )
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
        
        // Enhanced Brain class with comprehensive documentation
        py::class_<Brain>(m, "Brain",
            "Central neural simulation brain class.\n\n"
            "Brain is the core computational engine of NLM, implementing spiking neural
            networks with learning, memory, and decision-making capabilities.\n\n"
            "Key Features:\n"
            "    - Spiking neural network simulation\n"
            "    - Multiple plasticity mechanisms (STDP, Hebbian, structural)\n"
            "    - Memory systems (working, episodic, associative)\n"
            "    - Neuromodulation (dopamine, curiosity, novelty)\n"
            "    - Prediction and planning systems\n"
            "    - Developmental stages\n"
            "    - Checkpoint save/load functionality\n\n"
            "Methods:\n"
            "    initialize(): Initialize the brain with configuration\n"
            "    step(step): Perform a simulation step\n"
            "    step(step, time): Perform a simulation step with timestamp\n"
            "    receiveSensoryInput(input): Inject sensory input\n"
            "    injectCurrent(neuron, current): Inject current into specific neuron\n"
            "    injectCurrentToNeurons(type, current): Inject to all neurons of type\n"
            "    produceAction(): Generate motor action\n"
            "    reset(): Reset brain state\n"
            "    save(filepath): Save brain state to file\n"
            "    load(filepath): Load brain state from file\n"
            "    addRegion(name): Add a new neural region\n"
            "    getRegion(id): Get a region by ID\n"
            "    getTotalNeuronCount(): Get total neuron count\n"
            "    getTotalSynapseCount(): Get total synapse count\n"
            "    getActiveNeuronCount(): Get count of active neurons\n"
            "    getFiringNeuronCount(): Get count of firing neurons\n"
            "    getAverageFiringRate(): Get average firing rate\n"
            "    getExcitationInhibitionRatio(): Get E/I balance\n"
            "    getTotalSpikeCount(): Get total spike count\n"
            "    getDevelopmentalStage(): Get current developmental stage\n"
            "    setDevelopmentalStage(stage): Set developmental stage\n"
            "    getConfig(): Get the configuration\n"
            "    logStatus(): Log brain status\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create brain with default configuration\n"
            "    brain = pynlm.createBrain(pynlm.createDefaultConfig())\n"
            "    \n            "    # Initialize brain\n"
            "    brain.initialize()\n"
            "    \n            "    # Run simulation\n"
            "    for i in range(100):\n"
            "        brain.step(i)\n"
            "    \n            "    # Get brain statistics\n"
            "    print(f'Total neurons: {brain.getTotalNeuronCount()}')\n"
            "    print(f'Total spikes: {brain.getTotalSpikeCount()}')\n"
            "    print(f'Firing rate: {brain.getAverageFiringRate():.2f}')\n"
            "    ```\n\n"
            "The Brain class is the main interface for neural simulation, providing
            methods to control and monitor the neural network's activity and learning."
        )
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
        
        // Enhanced AgentBrain class with comprehensive documentation
        py::class_<AgentBrain>(m, "AgentBrain",
            "Agent brain interface connecting NLM brain to world.\n\n"
            "AgentBrain is the bridge between the neural brain and the simulated world,
            handling sensory transduction and motor decoding. It provides the interface
            for agents to interact with their environment.\n\n"
            "Key Features:\n"
            "    - Sensory input processing and transduction\n"
            "    - Motor output decoding and action selection\n"
            "    - Neuromodulation based on reward\n"
            "    - Developmental system updates\n"
            "    - Curiosity-driven exploration\n"
            "    - Novelty detection and learning\n"
            "    - Prediction error processing\n\n"
            "Methods:\n"
            "    initialize(world): Initialize with world\n"
            "    getSensoryInputSize(): Get expected sensory input size\n"
            "    getMotorOutputSize(): Get expected motor output size\n"
            "    processSensoryInput(percept): Process sensory percept and inject into brain\n"
            "    decodeMotorCommand(): Decode brain motor activity into motor command\n"
            "    applyRewardModulation(reward, predictedReward): Apply reward-based neuromodulation\n"
            "    updateDevelopment(timestep): Update development system\n"
            "    getDevelopmentalStage(): Get current developmental stage\n"
            "    getNeuromodulationLevel(): Get current neuromodulation level\n"
            "    getCuriosityLevel(): Get curiosity level\n"
            "    getNoveltyLevel(): Get novelty level\n"
            "    getPredictionError(): Get prediction error\n"
            "    reset(): Reset agent for new episode\n"
            "    getBrain(): Get the underlying brain\n"
            "    enableRewardModulation/enableStructuralPlasticity/etc.: Enable features\n"
            "    isRewardModulationEnabled/etc.: Check if features are enabled\n\n"
            "Examples:\n\n"
            "    ```python\n"
            "    import pynlm\n"
            "    \n            "    # Create agent brain\n"
            "    config = pynlm.createDefaultConfig()\n"
            "    brain = pynlm.createBrain(config)\n"
            "    agent = pynlm.createAgentBrain(brain)\n"
            "    \n            "    # Initialize with world\n"
            "    world = pynlm.createSimpleWorld()\n"
            "    agent.initialize(world)\n"
            "    \n            "    # Enable learning features\n"
            "    agent.enableRewardModulation(True)\n"
            "    agent.enableCuriosity(True)\n"
            "    \n            "    # Run agent simulation\n"
            "    for step in range(100):\n"
            "        world.update(0.1)\n"
            "        percept = world.getSensoryPercept()\n"
            "        agent.processSensoryInput(percept)\n"
            "        brain.step(step)\n"
            "        action = agent.decodeMotorCommand()\n"
            "        world.applyMotorCommand(action, world.getSimulationTime())\n"
            "    ```\n\n"
            "AgentBrain provides the complete interface for agents to interact with their
            environment, combining neural computation with world interaction."
        )
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
        
        // Factory functions with comprehensive documentation
        m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
            return std::make_shared<Config>();
        }, "Create a default configuration",
             "Creates a new configuration object with default settings.\n\n"
             "The default configuration includes sensible defaults for all NLM parameters.\n\n"
             "Returns:\n"
             "    A new Config object ready to be used with createBrain()\n\n"
             "Example:\n"
             "    config = pynlm.createDefaultConfig()\n"
             "    config.set('neuron_count', 1000)\n"
             "    brain = pynlm.createBrain(config)");
        
        m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
            return std::make_shared<Brain>(config);
        }, py::arg("config"), "Create a new brain with configuration",
             "Creates a new brain with the specified configuration.\n\n"
             "Args:\n"
             "    config: Configuration object for the brain\n\n"
             "Returns:\n"
             "    A new Brain object that can be initialized and simulated\n\n"
             "Example:\n"
             "    config = pynlm.createDefaultConfig()\n"
             "    brain = pynlm.createBrain(config)\n"
             "    brain.initialize()\n"
             "    brain.step(0)");
        
        m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
            return std::make_shared<SimpleWorld>();
        }, "Create a new simple world",
             "Creates a new simple 2D world for NLM simulation.\n\n"
             "The world provides the environment in which the agent operates, including
             sensory input, motor output, and world dynamics.\n\n"
             "Returns:\n"
             "    A new SimpleWorld object\n\n"
             "Example:\n"
             "    world = pynlm.createSimpleWorld()\n"
             "    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)\n"
             "    world.reset()");
        
        m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
            return std::make_shared<AgentBrain>(brain);
        }, py::arg("brain"), "Create a new agent brain interface",
             "Creates a new agent brain interface that connects the neural brain to the world.\n\n"
             "Args:\n"
             "    brain: The neural brain to connect\n\n"
             "Returns:\n"
             "    A new AgentBrain object that can be used to interact with the world\n\n"
             "Example:\n"
             "    config = pynlm.createDefaultConfig()\n"
             "    brain = pynlm.createBrain(config)\n"
             "    agent = pynlm.createAgentBrain(brain)\n"
             "    world = pynlm.createSimpleWorld()\n"
             "    agent.initialize(world)");
        
        // Add module attributes for invalid IDs
        m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
        m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
        m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
        m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);
        
        NLM_LOG_INFO("Python bindings initialization completed successfully");
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Error during Python bindings initialization: " + std::string(e.what()));
        throw;
    }
    
}

} // namespace nlm
