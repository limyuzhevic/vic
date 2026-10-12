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
#include "../src/brain/Synapse.hpp"
#include "../src/brain/NeuralPopulation.hpp"
#include "../src/brain/NeuralRegion.hpp"

namespace py = pybind11;
namespace nlm {

PYBIND11_MODULE(pynlm, m) {
    m.doc() = R"pbdoc(
        NLM (熙然) Python Bindings
        ---------------------------
        A Python binding for the NLM C++ neural simulation framework.
        
        This module provides comprehensive Python bindings for the NLM
        (Neural Learning Machine) brain simulation system, enabling
        researchers and developers to create, control, and analyze
        artificial neural networks using Python.
        
        **Core Components:**
        
        **Configuration Management:**
          - Config: Load/save configuration from JSON files
          - createDefaultConfig(): Create default configuration
          
        **Neural Architecture:**
          - Brain: Central neural simulation engine with integrated systems
          - NeuralRegion: Brain regions with populations and connectivity
          - NeuralPopulation: Groups of neurons with shared properties
          - Neuron: Individual spiking neurons (LIF dynamics)
          - Synapse: Synaptic connections with plasticity mechanisms
          
        **Sensory-Motor Interface:**
          - Vision, Audio, InternalSignals: Sensory input modalities
          - SensoryPercept: Agent's perception of the environment
          - AgentBody: Physical properties and state representation
          - SimpleWorld: 2D simulation environment with physics
          - Action: Motor commands for world interaction
          
        **Advanced Learning Systems:**
          - AgentBrain: Connects brain to world with developmental learning
          - Reward modulation, structural plasticity, curiosity-driven learning
          
        **Usage Examples:**
        
        Basic Brain Simulation:
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> config.set("neuron_count", 1000)
        >>> config.set("connection_probability", 0.1)
        >>> brain = pynlm.Brain(config)
        >>> brain.initialize()
        >>> 
        >>> for step in range(100):
        ...     # Get sensory input
        ...     percept = world.getSensoryPercept()
        ...     # Process sensory input
        ...     agent.processSensoryInput(percept)
        ...     # Run brain computation
        ...     brain.step(step)
        ...     # Generate motor command
        ...     action = agent.decodeMotorCommand()
        ...     # Apply action to world
        ...     world.applyMotorCommand(action, world.getSimulationTime())
        
        Complete Agent Simulation:
        >>> import pynlm
        >>> 
        >>> def run_agent_simulation(num_steps=1000):
        ...     # 1. Create configuration
        ...     config = pynlm.Config()
        ...     
        ...     # 2. Create brain and initialize
        ...     brain = pynlm.Brain(config)
        ...     brain.initialize()
        ...     
        ...     # 3. Create agent brain interface
        ...     agent = pynlm.AgentBrain(brain)
        ...     
        ...     # 4. Create and configure world
        ...     world = pynlm.SimpleWorld()
        ...     world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        ...     world.reset()
        ...     world.setAgentStart(10.0, 10.0)
        ...     
        ...     # 5. Initialize agent with world
        ...     agent.initialize(world)
        ...     
        ...     # 6. Enable learning subsystems
        ...     agent.enableRewardModulation(True)
        ...     agent.enableStructuralPlasticity(True)
        ...     agent.enableDevelopment(True)
        ...     agent.enableCuriosity(True)
        ...     
        ...     # 7. Run simulation loop
        ...     for step in range(num_steps):
        ...         # Update world
        ...         world.update(timestep=0.1)
        ...         
        ...         # Get sensory input from world
        ...         percept = world.getSensoryPercept()
        ...         
        ...         # Process sensory input in brain
        ...         agent.processSensoryInput(percept)
        ...         
        ...         # Run brain step
        ...         brain.step(step)
        ...         
        ...         # Decode motor command from brain activity
        ...         motor_cmd = agent.decodeMotorCommand()
        ...         
        ...         # Apply motor command to world
        ...         world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        ...         
        ...         # Apply reward modulation (if implemented)
        ...         # reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        ...         # agent.applyRewardModulation(reward, 0.0)
        ...         
        ...         # Update development (if enabled)
        ...         # agent.updateDevelopment(0.1)
        ...         
        ...         # Print progress
        ...         if step % 100 == 0:
        ...             print(f"Step {step}:")
        ...             print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        ...             print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
        ...             print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
        ...             print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
        ...     
        ...     # Run the simulation
        ...     run_agent_simulation(1000)
        
        Advanced Synapse Operations:
        >>> synapse = pynlm.Synapse(source_neuron_id, destination_neuron_id)
        >>> synapse.setWeight(0.5)
        >>> synapse.setDelay(2)
        >>> synapse.enablePlasticity(True, True, False)
        >>> 
        >>> # Record spike activity for plasticity
        >>> synapse.recordPreSpike(timestamp)
        >>> synapse.recordPostSpike(timestamp + 0.001)
        
        Neural Network Construction:
        >>> config = pynlm.Config()
        >>> brain = pynlm.Brain(config)
        >>> brain.initialize()
        >>> 
        >>> # Add regions and populations
        >>> region_id = brain.addRegion("cortex")
        >>> region = brain.getRegion(region_id)
        >>> population_id = region.addPopulation(100, pynlm.NeuronType.Excitatory)
        >>> population = region.getPopulation(population_id)
        >>> 
        >>> # Create synapses between neurons
        >>> source_neuron_id = NeuronId(0)
        >>> dest_neuron_id = NeuronId(1)
        >>> synapse_id = region.addSynapse(source_neuron_id, dest_neuron_id)
        >>> 
        >>> # Configure synapse properties
        >>> synapse = region.getSynapse(synapse_id)
        >>> synapse.setWeight(0.3)
        >>> synapse.setType(pynlm.SynapseType.Excitatory)
        
        For detailed documentation, see: https://github.com/your-org/nlm
    )pbdoc";

    py::register_exception<std::runtime_error>(m, "RuntimeError");
    py::register_exception<std::invalid_argument>(m, "ValueError");
    py::register_exception<std::out_of_range>(m, "IndexError");

    // Core type bindings with improved documentation

    // NeuronId: Unique identifier for neurons with Pythonic interface
    py::class_<NeuronId>(m, "NeuronId", R"pbdoc(
        Unique identifier for a neuron with Pythonic interface.
        
        NeuronId provides a strongly-typed neuron identifier with
        comparison operators, hashing, and string representation.
        
        Examples:
          >>> neuron_id = pynlm.NeuronId(123)
          >>> print(neuron_id)
          <NeuronId: 123>
          >>> neuron_id2 = pynlm.NeuronId(456)
          >>> neuron_id < neuron_id2
          True
          >>> hash(neuron_id)  # For use in dictionaries/sets
          123
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &NeuronId::value)
        .def("index", &NeuronId::index)
        .def("__eq__", &NeuronId::operator==)
        .def("__ne__", &NeuronId::operator!=)
        .def("__lt__", &NeuronId::operator<)
        .def("__le__", [](const NeuronId& a, const NeuronId& b) { return a.value <= b.value; })
        .def("__gt__", [](const NeuronId& a, const NeuronId& b) { return a.value > b.value; })
        .def("__ge__", [](const NeuronId& a, const NeuronId& b) { return a.value >= b.value; })
        .def("__hash__", [](const NeuronId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const NeuronId& id) {
            return "<NeuronId: " + std::to_string(id.value) + ">";
        })
        .def("__str__", [](const NeuronId& id) {
            return std::to_string(id.value);
        })
        .def(py::self == py::self)
        .def(py::self != py::self);

    // SynapseId: Unique identifier for synapses with Pythonic interface
    py::class_<SynapseId>(m, "SynapseId", R"pbdoc(
        Unique identifier for a synapse with Pythonic interface.
        
        SynapseId provides a strongly-typed synapse identifier with
        comparison operators, hashing, and string representation.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &SynapseId::value)
        .def("index", &SynapseId::index)
        .def("__eq__", &SynapseId::operator==)
        .def("__ne__", &SynapseId::operator!=)
        .def("__lt__", &SynapseId::operator<)
        .def("__hash__", [](const SynapseId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const SynapseId& id) {
            return "<SynapseId: " + std::to_string(id.value) + ">";
        });

    // RegionId: Unique identifier for brain regions with Pythonic interface
    py::class_<RegionId>(m, "RegionId", R"pbdoc(
        Unique identifier for a brain region with Pythonic interface.
    )pbdoc")
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

    // PopulationId: Unique identifier for neuron populations
    py::class_<PopulationId>(m, "PopulationId", R"pbdoc(
        Unique identifier for a neuron population with Pythonic interface.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<uint64_t>(), py::arg("value"))
        .def_readwrite("value", &PopulationId::value)
        .def("index", &PopulationId::index)
        .def("__eq__", &PopulationId::operator==)
        .def("__ne__", &PopulationId::operator!=)
        .def("__hash__", [](const PopulationId& id) { return std::hash<uint64_t>{}(id.value); })
        .def("__repr__", [](const PopulationId& id) {
            return "<PopulationId: " + std::to_string(id.value) + ">";
        });

    // Enumerations with comprehensive documentation

    py::enum_<NeuronType>(m, "NeuronType", R"pbdoc(
        Neuron type enumeration for different neuron categories.
        
        Defines the functional type of neurons in the network, affecting
        their behavior, connectivity, and plasticity rules.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory, " Neurons that increase firing probability in postsynaptic neurons")
        .value("Inhibitory", NeuronType::Inhibitory, " Neurons that decrease firing probability in postsynaptic neurons")
        .value("Modulatory", NeuronType::Modulatory, " Neurons that release neuromodulators affecting many other neurons")
        .value("Sensory", NeuronType::Sensory, " Neurons that process external sensory inputs")
        .value("Motor", NeuronType::Motor, " Neurons that generate motor commands")
        .value("Internal", NeuronType::Internal, " Neurons involved in internal processing and storage")
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(
        Synapse type enumeration defining connection characteristics.
        
        Specifies the type of synaptic transmission and plasticity rules.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory, " Classic chemical synapse that increases postsynaptic firing")
        .value("Inhibitory", SynapseType::Inhibitory, " Synapse that decreases postsynaptic firing")
        .value("Modulatory", SynapseType::Modulatory, " Synapse that releases neuromodulators")
        .value("Electrical", SynapseType::Electrical, " Electrical synapse allowing direct current flow")
        .value("GapJunction", SynapseType::GapJunction, " Direct electrical coupling between cells")
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(
        Brain development stage enumeration.
        
        Represents the developmental phase of the brain, affecting
        plasticity rates and learning capabilities.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial, " Initial state with maximum plasticity")
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod, " Sensitive period for skill acquisition")
        .value("Maturation", DevelopmentalStage::Maturation, " Normal development phase")
        .value("Adult", DevelopmentalStage::Adult, " Fully mature with limited plasticity")
        .value("Aging", DevelopmentalStage::Aging, " Age-related changes and decline")
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(
        Neuron firing state enumeration.
        
        Represents the current electrical state of a neuron.
    )pbdoc")
        .value("Resting", FiringState::Resting, " Neuron at rest, below threshold")
        .value("Active", FiringState::Active, " Neuron currently firing action potential")
        .value("Refractory", FiringState::Refractory, " Neuron recovering from spike, unable to fire")
        .value("Inhibited", FiringState::Inhibited, " Neuron actively inhibited by inputs")
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(
        High-level action type enumeration for behavior selection.
        
        Defines possible behavioral actions an agent can take.
    )pbdoc")
        .value("MoveForward", ActionType::MoveForward, " Move forward in the environment")
        .value("MoveBackward", ActionType::MoveBackward, " Move backward in the environment")
        .value("MoveLeft", ActionType::MoveLeft, " Move left in the environment")
        .value("MoveRight", ActionType::MoveRight, " Move right in the environment")
        .value("TurnLeft", ActionType::TurnLeft, " Rotate left by default angle")
        .value("TurnRight", ActionType::TurnRight, " Rotate right by default angle")
        .value("Look", ActionType::Look, " Look in current direction")
        .value("LookUp", ActionType::LookUp, " Look upward")
        .value("LookDown", ActionType::LookDown, " Look downward")
        .value("Interact", ActionType::Interact, " Interact with nearby object")
        .value("Eat", ActionType::Eat, " Consume food resource")
        .value("Drink", ActionType::Drink, " Drink water resource")
        .value("Rest", ActionType::Rest, " Stop moving and rest")
        .value("Wait", ActionType::Wait, " Do nothing for one time step")
        .value("Custom", ActionType::Custom, " Custom action type for extension")
        .export_values();

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(
        Low-level motor command enumeration for motor system.
        
        Defines specific motor commands that can be executed by the agent.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward, " Move forward by velocity")
        .value("MoveBackward", MotorCommand::MoveBackward, " Move backward by velocity")
        .value("TurnLeft", MotorCommand::TurnLeft, " Rotate left by angular velocity")
        .value("TurnRight", MotorCommand::TurnRight, " Rotate right by angular velocity")
        .value("LookLeft", MotorCommand::LookLeft, " Look left by angular velocity")
        .value("LookRight", MotorCommand::LookRight, " Look right by angular velocity")
        .value("Interact", MotorCommand::Interact, " Interact with environment")
        .value("Wait", MotorCommand::Wait, " No movement command")
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(
        World object type enumeration for environment simulation.
        
        Defines the types of objects that can exist in the simulation world.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty, " Empty space with no object")
        .value("Resource", WorldObjectType::Resource, " Consumable resource with value")
        .value("Hazard", WorldObjectType::Hazard, " Dangerous object that causes damage")
        .value("Wall", WorldObjectType::Wall, " Impassable barrier")
        .value("Marker", WorldObjectType::Marker, " Special point for navigation or goals")
        .export_values();

    // Config class with improved error handling
    py::class_<Config>(m, "Config", R"pbdoc(
        Configuration class for NLM system with error handling and validation.
        
        Manages neural network parameters and simulation settings with
        comprehensive error checking and type validation.
        
        Key features:
        - Load/save configuration from JSON files with error handling
        - Type-safe parameter setting with validation
        - Automatic type conversion between Python and C++
        - Comprehensive error messages for debugging
        
        Examples:
          >>> config = pynlm.Config()
          >>> config.loadFromFile("my_config.json")
          >>> config.set("neuron_count", 1000)
          >>> config.set("learning_rate", 0.01)
          >>> config.set("plasticity.enabled", True)
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", [](const std::string& filepath) {
            try {
                Config cfg;
                if (!cfg.loadFromFile(filepath)) {
                    throw std::runtime_error("Failed to load config from file: " + filepath);
                }
                return cfg;
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Config load error: ") + e.what());
            }
        }, py::arg("filepath"), R"pbdoc(
            Load configuration from a JSON file with comprehensive error handling.
            
            Args:
                filepath: Path to JSON configuration file
                
            Returns:
                Config object with loaded settings
                
            Raises:
                RuntimeError: If file cannot be read
                ValueError: If file cannot be parsed or is invalid JSON
                ValueError: If configuration validation fails
                
            Examples:
              >>> config = pynlm.Config()
              >>> config.loadFromFile("config.json")
        )pbdoc")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            try {
                self.loadFromArgs(argc, argv);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Config loadFromArgs error: ") + e.what());
            }
        }, py::arg("argc"), py::arg("argv"),
            "Load configuration from command line arguments with error handling")
        .def("saveToFile", [](const Config& self, const std::string& filepath) {
            try {
                if (!self.saveToFile(filepath)) {
                    throw std::runtime_error("Failed to save config to file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Config save error: ") + e.what());
            }
        }, py::arg("filepath"), "Save configuration to a JSON file with error handling")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists (case-sensitive)")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys in the current configuration")
        .def("clear", &Config::clear,
             "Clear all configuration entries")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration for debugging")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        })
        .def("set", [](Config& self, const std::string& key, const py::object& value) {
            try {
                if (key.empty()) {
                    throw std::invalid_argument("Configuration key cannot be empty");
                }
                if (!value) {
                    throw std::invalid_argument("Configuration value cannot be None");
                }
                
                // Type checking and conversion
                if (py::isinstance<int>(value)) {
                    self.set(key, value.cast<int>());
                } else if (py::isinstance<double>(value)) {
                    self.set(key, value.cast<double>());
                } else if (py::isinstance<bool>(value)) {
                    self.set(key, value.cast<bool>());
                } else if (py::isinstance<std::string>(value)) {
                    self.set(key, value.cast<std::string>());
                } else if (py::isinstance<py::list>(value)) {
                    auto list = value.cast<std::vector<double>>();
                    self.set(key, list);
                } else if (py::isinstance<py::dict>(value)) {
                    auto dict = value.cast<std::unordered_map<std::string, double>>();
                    self.set(key, dict);
                } else {
                    throw std::invalid_argument("Unsupported configuration value type: " + std::string(py::str(value)));
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Config set error for key '":") + key + "': " + e.what());
            }
        }, py::arg("key"), py::arg("value"), R"pbdoc(
            Set configuration value with type validation and error handling.
            
            Args:
                key: Configuration parameter name (e.g., "brain.neuron_count")
                value: Parameter value (automatically converted to appropriate C++ type)
                
            Supported Python types and C++ equivalents:
                - int -> int64_t
                - float -> double  
                - bool -> bool
                - str -> std::string
                - list -> std::vector<double>
                - dict -> std::unordered_map<std::string, double>
                
            Raises:
                ValueError: If key is empty or value type is unsupported
                ValueError: If configuration setting fails (invalid parameter)
                
            Examples:
              >>> config = pynlm.Config()
              >>> config.set("brain.neuron_count", 1000)
              >>> config.set("brain.synapse_density", 0.1)
              >>> config.set("plasticity.stdp.enabled", True)
              >>> config.set("learning.rate", 0.001)
              >>> config.set("neural_population.weights", [0.5, 0.3, 0.8])
        )pbdoc");

    // SensoryInput base class
    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(
        Base class for sensory input from the environment.
        
        Represents sensory data received from the environment, including
        vision, audio, and internal signals.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input")
        .def("getData", &SensoryInput::getData, "Get the raw data as a vector")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"),
             "Set the timestamp");

    // Vision class with error handling
    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(
        Vision sensory input representing visual data from the environment.
        
        Provides 2D visual input with configurable dimensions and color channels.
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (1=gray, 3=RGB)
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3)
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            try {
                if (data.size() != self.getWidth() * self.getHeight() * self.getChannels()) {
                    throw std::invalid_argument(
                        "Data size mismatch: expected " + std::to_string(
                            self.getWidth() * self.getHeight() * self.getChannels()) +
                        " elements, got " + std::to_string(data.size())
                    );
                }
                self.setData(data);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Vision setData error: ") + e.what());
            }
        }, py::arg("data"), R"pbdoc(
            Set vision data with validation.
            
            Args:
                data: Vector of float values representing image pixels
                
            Raises:
                ValueError: If data size doesn't match image dimensions
                
            Note: Data should be formatted as [R,G,B] or grayscale values
                  for each pixel, in row-major order.
        )pbdoc")
        .def("getWidth", &Vision::getWidth)
        .def("getHeight", &Vision::getHeight)
        .def("getChannels", &Vision::getChannels);

    // Audio class with error handling
    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(
        Audio sensory input representing sound data from the environment.
        
        Provides audio input with configurable sample rate and duration.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"))
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            try {
                if (data.size() != self.getNumSamples()) {
                    throw std::invalid_argument(
                        "Audio data size mismatch: expected " + std::to_string(self.getNumSamples()) +
                        " samples, got " + std::to_string(data.size())
                    );
                }
                self.setData(data);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Audio setData error: ") + e.what());
            }
        }, py::arg("data"), "Set audio data with validation")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"))
        .def("getSampleRate", &Audio::getSampleRate)
        .def("getNumSamples", &Audio::getNumSamples);

    // InternalSignals class
    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(
        Internal signals sensory input representing internal brain state.
        
        Provides access to internal signals like reward predictions,
        motivation levels, and other internal brain variables.
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add a signal value to the internal signals")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals");

    // Action class with comprehensive documentation
    py::class_<Action>(m, "Action", R"pbdoc(
        Action representation for motor output with complete parameter handling.
        
        Represents a motor command that can be executed by the agent,
        including action type and associated parameters.
        
        Attributes:
            type: Action type (MoveForward, TurnLeft, etc.)
            parameters: Additional action parameters (velocity, angle, etc.)
        
        Examples:
          >>> # Create action with just type
          >>> action = pynlm.Action(pynlm.ActionType.MoveForward)
          >>> 
          >>> # Create action with type and parameters
          >>> action = pynlm.Action(pynlm.ActionType.TurnLeft, [1.57])
          >>> action.getType()
          <ActionType.TurnLeft: 4>
          >>> action.getParameters()
          [1.57]
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"))
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"))
        .def("getType", &Action::getType)
        .def("setType", &Action::setType, py::arg("type"))
        .def("getParameters", &Action::getParameters)
        .def("setParameters", &Action::setParameters, py::arg("params"))
        .def("getName", &Action::getName)
        .def("clone", &Action::clone);

    // WorldObject class with complete interface
    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(
        World object representation in the simulation environment.
        
        Represents objects in the 2D world including resources, hazards,
        walls, and markers. Objects have position, type, and value attributes.
        
        Attributes:
            x: X coordinate in world space
            y: Y coordinate in world space
            type: Object type (Resource, Hazard, Wall, etc.)
            value: Object value (energy for resources, damage for hazards)
            radius: Object radius for collision detection
            active: Whether the object is currently active
        
        Examples:
          >>> # Create empty space
          >>> empty = pynlm.WorldObject()
          >>> # Create resource
          >>> resource = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, value=10.0)
          >>> # Create wall
          >>> wall = pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Wall, radius=0.5)
    )pbdoc")
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

    // AgentBody class with comprehensive state tracking
    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(
        Agent body state representation for physical simulation.
        
        Tracks the complete physical state of the agent including position,
        orientation, velocity, energy, and health. Used by the world simulation
        to apply physics and track agent status.
        
        Attributes:
            x: X position in world coordinates
            y: Y position in world coordinates
            orientation: Facing angle in radians (0 = east, π/2 = north)
            velocityX: X component of linear velocity
            velocityY: Y component of linear velocity
            angularVelocity: Rotational velocity in radians per timestep
            energy: Current energy level
            health: Current health level
            age: Simulation time since agent creation
            isMoving: Whether agent is currently moving
            isTurning: Whether agent is currently rotating
            lastActionTime: Time since last action was taken
        
        Examples:
          >>> body = pynlm.AgentBody()
          >>> body.x = 10.0
          >>> body.y = 10.0
          >>> body.orientation = 0.0  # Facing east
          >>> body.velocityX = 1.0
          >>> body.velocityY = 0.5
          >>> body.energy = 100.0
          >>> body.health = 100.0
    )pbdoc")
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

    // ActionResult class for action outcomes
    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(
        Result of an action executed in the world environment.
        
        Represents the outcome of an action including reward, success status,
        and any additional information about the action's effects.
        
        Attributes:
            reward: Reward value received from the world
            success: Whether the action was successful
            message: Descriptive message about the action outcome
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    // SensoryPercept class with comprehensive sensor data access
    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(
        Complete sensory percept data from the environment.
        
        Represents all sensory input available to the agent, including
        visual, auditory, proprioceptive, and internal signals. Provides
        methods to access and manipulate all sensory modalities.
        
        Multiple sensory modalities can be set:
        - Vision: Visual scene input
        - Audio: Sound input  
        - Internal: Internal brain signals
        - Proprioception: Body position and movement
        
        Examples:
          >>> percept = pynlm.SensoryPercept()
          >>> # Set visual input
          >>> vision = pynlm.Vision(8, 8, 3)  # 8x8 RGB image
          >>> vision.setData([0.1, 0.2, 0.3, ...])  # Pixel values
          >>> percept.setVision(vision)
          >>> 
          >>> # Set internal signals (e.g., reward prediction)
          >>> internal = pynlm.InternalSignals()
          >>> internal.addSignal(0.5)  # Reward prediction
          >>> internal.addSignal(0.8)  # Motivation level
          >>> percept.setInternal(internal)
          >>> 
          >>> # Access all sensory data
          >>> print(percept.getVisionWidth(), "x", percept.getVisionHeight())
          >>> print("Internal signals:", percept.getInternal())
    )pbdoc")
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

    // Synapse class with comprehensive error handling and documentation
    py::class_<Synapse>(m, "Synapse", R"pbdoc(
        Synaptic connection between neurons with complete functionality.
        
        Represents a biological synapse implementing real synaptic
        transmission with delays, weights, and plasticity mechanisms.
        
        **Core Properties:**
        - Identity: Unique synapse ID, source/destination neurons
        - Connection: Synaptic weight, delay, transmission type
        - Plasticity: STDP, Hebbian, reward-modulated learning
        - Activity: Spike history recording and eligibility traces
        
        **Key Methods:**
        - Weight management: get/set/addToWeight()
        - Delay control: get/setDelay()  
        - Type specification: get/setType()
        - Plasticity control: enablePlasticity()
        - Activity tracking: recordPre/PostSpike()
        - Learning: eligibility trace management
        
        **Plasticity Support:**
        - Spike-Timing-Dependent Plasticity (STDP)
        - Hebbian learning (co-activation)
        - Reward-modulated plasticity
        - Structural plasticity (connection formation/pruning)
        
        Examples:
          >>> # Create synapse between neurons
          >>> synapse = pynlm.Synapse(source_id, dest_id)
          >>> 
          >>> # Configure synaptic properties
          >>> synapse.setWeight(0.5)
          >>> synapse.setDelay(2)
          >>> synapse.setType(pynlm.SynapseType.Excitatory)
          >>> 
          >>> # Enable learning mechanisms
          >>> synapse.enablePlasticity(True, True, False)  # STDP + Hebbian
          >>> 
          >>> # Record spike activity for plasticity
          >>> synapse.recordPreSpike(timestamp)
          >>> synapse.recordPostSpike(timestamp + 0.001)
          >>> 
          >>> # Access synaptic state
          >>> synapse.getWeight()
          >>> synapse.getDelay()
          >>> synapse.getType()
          >>> synapse.isExcitatory()
    )pbdoc")
        .def(py::init<>()) // Default constructor
        .def(py::init<SynapseId, NeuronId, NeuronId>(), 
             py::arg("id"), py::arg("source"), py::arg("destination"))
        .def("getId", &Synapse::getId)
        .def("getSourceNeuron", &Synapse::getSourceNeuron)
        .def("getDestinationNeuron", &Synapse::getDestinationNeuron)
        .def("getWeight", &Synapse::getWeight)
        .def("setWeight", [](Synapse& self, SynapticWeight weight) {
            try {
                self.setWeight(weight);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse setWeight error: ") + e.what());
            }
        }, py::arg("weight"), "Set synaptic weight with error handling")
        .def("addToWeight", [](Synapse& self, SynapticWeight delta) {
            try {
                self.addToWeight(delta);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse addToWeight error: ") + e.what());
            }
        }, py::arg("delta"), "Add delta to current weight")
        .def("getDelay", &Synapse::getDelay)
        .def("setDelay", [](Synapse& self, Delay delay) {
            try {
                if (delay == 0) {
                    throw std::invalid_argument("Synapse delay cannot be zero");
                }
                self.setDelay(delay);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse setDelay error: ") + e.what());
            }
        }, py::arg("delay"), "Set synaptic delay (must be > 0)")
        .def("getType", &Synapse::getType)
        .def("setType", [](Synapse& self, SynapseType type) {
            try {
                self.setType(type);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse setType error: ") + e.what());
            }
        }, py::arg("type"), "Set synapse type")
        .def("isExcitatory", &Synapse::isExcitatory)
        .def("isInhibitory", &Synapse::isInhibitory)
        .def("recordPreSpike", [](Synapse& self, Timestamp timestamp) {
            try {
                if (timestamp < 0) {
                    throw std::invalid_argument("Synapse spike timestamp cannot be negative");
                }
                self.recordPreSpike(timestamp);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse recordPreSpike error: ") + e.what());
            }
        }, py::arg("timestamp"), "Record presynaptic spike with validation")
        .def("recordPostSpike", [](Synapse& self, Timestamp timestamp) {
            try {
                if (timestamp < 0) {
                    throw std::invalid_argument("Synapse spike timestamp cannot be negative");
                }
                self.recordPostSpike(timestamp);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse recordPostSpike error: ") + e.what());
            }
        }, py::arg("timestamp"), "Record postsynaptic spike with validation")
        .def("getPreSpikeHistory", &Synapse::getPreSpikeHistory)
        .def("getPostSpikeHistory", &Synapse::getPostSpikeHistory)
        .def("clearHistory", &Synapse::clearHistory)
        .def("getPlasticityFlags", [](Synapse& self) {
            try {
                return self.getPlasticityFlags();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse getPlasticityFlags error: ") + e.what());
            }
        })
        .def("enablePlasticity", [](Synapse& self, bool hebbian, bool stdp, bool rewardModulated, bool structural = false) {
            try {
                self.enablePlasticity(hebbian, stdp, rewardModulated);
                // Note: structural plasticity flag is not in the C++ interface yet
                // but we keep it for future compatibility
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse enablePlasticity error: ") + e.what());
            }
        }, py::arg("hebbian"), py::arg("stdp"), py::arg("rewardModulated"), py::arg("structural") = false,
             "Enable plasticity rules (Hebbian, STDP, reward-modulated)")
        .def("getEligibilityTrace", &Synapse::getEligibilityTrace)
        .def("setEligibilityTrace", [](Synapse& self, float trace) {
            try {
                if (trace < 0) {
                    throw std::invalid_argument("Eligibility trace cannot be negative");
                }
                self.setEligibilityTrace(trace);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse setEligibilityTrace error: ") + e.what());
            }
        }, py::arg("trace"), "Set eligibility trace value")
        .def("decayEligibilityTrace", [](Synapse& self, float decayRate) {
            try {
                if (decayRate < 0 || decayRate > 1) {
                    throw std::invalid_argument("Decay rate must be between 0 and 1");
                }
                self.decayEligibilityTrace(decayRate);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse decayEligibilityTrace error: ") + e.what());
            }
        }, py::arg("decayRate"), "Decay eligibility trace by rate")
        .def("getEfficacy", &Synapse::getEfficacy)
        .def("setEfficacy", [](Synapse& self, float efficacy) {
            try {
                if (efficacy < 0) {
                    throw std::invalid_argument("Synaptic efficacy cannot be negative");
                }
                self.setEfficacy(efficacy);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse setEfficacy error: ") + e.what());
            }
        }, py::arg("efficacy"), "Set synaptic efficacy (use-dependent modulation)")
        .def("reset", &Synapse::reset, "Reset synapse to initial state")
        .def("initializeRandom", [](Synapse& self, class RandomGenerator& rng) {
            try {
                self.initializeRandom(rng);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Synapse initializeRandom error: ") + e.what());
            }
        }, py::arg("rng"), "Initialize synapse parameters randomly");

    // NeuralPopulation class with comprehensive functionality
    py::class_<NeuralPopulation>(m, "NeuralPopulation", R"pbdoc(
        Group of neurons with shared properties and population-level dynamics.
        
        NeuralPopulation represents a collection of neurons that share common
        properties and can operate with population-level dynamics. This enables
        efficient simulation of large neural populations with shared characteristics.
        
        **Population Properties:**
        - ID: Unique population identifier
        - Size: Number of neurons in population
        - Type: Neuron type (all neurons share same type)
        - Statistics: Population-level statistics and metrics
        
        **Key Methods:**
        - Identity: getId(), setName() for population identification
        - Size management: getSize(), isEmpty(), addNeuron()
        - Neuron access: getNeuron(), getNeurons() for population manipulation
        - Type control: getNeuronType(), setNeuronType() for population properties
        - Statistics: getActiveCount(), getFiringCount(), getAverageFiringRate()
        - Dynamics: step(), reset(), initializeRandom() for population simulation
        - Recording: recordActivity(), getMeanMembranePotential() for analysis
        
        Examples:
          >>> # Create population
          >>> population = pynlm.NeuralPopulation(population_id, 100)
          >>> 
          >>> # Set population properties
          >>> population.setNeuronType(pynlm.NeuronType.Excitatory)
          >>> 
          >>> # Access neurons
          >>> neurons = population.getNeurons()
          >>> neuron = population.getNeuron(0)
          >>> 
          >>> # Get population statistics
          >>> print("Population size:", population.getSize())
          >>> print("Active neurons:", population.getActiveCount())
          >>> print("Firing rate:", population.getAverageFiringRate())
    )pbdoc")
        .def(py::init<>()) // Default constructor
        .def(py::init<PopulationId, size_t>(), 
             py::arg("id"), py::arg("size"))
        .def("getId", &NeuralPopulation::getId)
        .def("getSize", &NeuralPopulation::getSize)
        .def("isEmpty", &NeuralPopulation::isEmpty)
        .def("getNeuron", [](NeuralPopulation& self, NeuronIndex index) {
            try {
                return self.getNeuron(index);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralPopulation getNeuron error: ") + e.what());
            }
        }, py::arg("index"), "Get neuron by index with error handling")
        .def("getNeurons", &NeuralPopulation::getNeurons)
        .def("addNeuron", [](NeuralPopulation& self, Neuron* neuron) {
            try {
                self.addNeuron(neuron);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralPopulation addNeuron error: ") + e.what());
            }
        }, py::arg("neuron"), "Add neuron to population")
        .def("getNeuronType", &NeuralPopulation::getNeuronType)
        .def("setNeuronType", [](NeuralPopulation& self, NeuronType type) {
            try {
                self.setNeuronType(type);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralPopulation setNeuronType error: ") + e.what());
            }
        }, py::arg("type"), "Set neuron type for all neurons in population")
        .def("getActiveCount", &NeuralPopulation::getActiveCount)
        .def("getFiringCount", &NeuralPopulation::getFiringCount)
        .def("getAverageFiringRate", &NeuralPopulation::getAverageFiringRate)
        .def("getActivityLevel", &NeuralPopulation::getActivityLevel)
        .def("initializeRandom", [](NeuralPopulation& self, class RandomGenerator& rng) {
            try {
                self.initializeRandom(rng);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralPopulation initializeRandom error: ") + e.what());
            }
        }, py::arg("rng"), "Initialize all neurons randomly")
        .def("step", [](NeuralPopulation& self, Timestamp currentTime) {
            try {
                self.step(currentTime);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralPopulation step error: ") + e.what());
            }
        }, py::arg("currentTime"), "Step all neurons in population")
        .def("reset", &NeuralPopulation::reset, "Reset all neurons to initial state")
        .def("getNeuronIds", &NeuralPopulation::getNeuronIds)
        .def("recordActivity", &NeuralPopulation::recordActivity,
             "Record current activity for statistics")
        .def("getMeanMembranePotential", &NeuralPopulation::getMeanMembranePotential)
        .def("getVarianceMembranePotential", &NeuralPopulation::getVarianceMembranePotential);

    // NeuralRegion class with comprehensive connectivity management
    py::class_<NeuralRegion>(m, "NeuralRegion", R"pbdoc(
        Brain region containing multiple populations and local connectivity.
        
        NeuralRegion represents a functional brain region that contains
        multiple neural populations and manages local connectivity between
        neurons. This enables modular brain architecture with region-specific
        processing and communication.
        
        **Region Structure:**
        - Identity: Region ID and name for identification
        - Populations: Multiple NeuralPopulation objects
        - Synapses: Local connections within the region
        - Connectivity: Intracranial connections only
        
        **Population Management:**
        - Creation: addPopulation() to add new neural populations
        - Access: getPopulation(), getPopulations() for population manipulation
        - Statistics: getPopulationCount() for regional statistics
        
        **Synapse Management:**
        - Creation: addSynapse() to create local connections
        - Access: getSynapse(), getSynapses() for synapse manipulation
        - Connectivity: getSynapsesFrom(), getSynapsesTo() for connectivity queries
        
        **Regional Statistics:**
        - Scale: getTotalNeuronCount(), getTotalSynapseCount()
        - Activity: getActiveNeuronCount(), getFiringNeuronCount()
        - Performance: getAverageFiringRate(), getAverageSynapticWeight()
        
        **Dynamics and Initialization:**
        - Simulation: step() for regional time integration
        - Reset: reset() for clearing regional state
        - Connectivity: initializeRandomConnectivity() for random wiring
        
        ** neuron Access:**
        - Granular: getAllNeurons() for complete regional access
        
        Examples:
          >>> # Create region
          >>> region = pynlm.NeuralRegion(region_id, "cortex")
          >>> 
          >>> # Add populations
          >>> exc_pop_id = region.addPopulation(100, pynlm.NeuronType.Excitatory)
          >>> inh_pop_id = region.addPopulation(50, pynlm.NeuronType.Inhibitory)
          >>> 
          >>> # Get populations
          >>> exc_pop = region.getPopulation(exc_pop_id)
          >>> inh_pop = region.getPopulation(inh_pop_id)
          >>> 
          >>> # Create synapses (local connections only)
          >>> syn_id = region.addSynapse(exc_neuron_id, inh_neuron_id)
          >>> 
          >>> # Access regional statistics
          >>> print("Total neurons:", region.getTotalNeuronCount())
          >>> print("Total synapses:", region.getSynapseCount())
          >>> print("Firing rate:", region.getAverageFiringRate())
    )pbdoc")
        .def(py::init<>()) // Default constructor
        .def(py::init<RegionId, std::string>(), 
             py::arg("id"), py::arg("name") = "")
        .def("getId", &NeuralRegion::getId)
        .def("getName", &NeuralRegion::getName)
        .def("setName", &NeuralRegion::setName, py::arg("name"), "Set region name")
        .def("addPopulation", [](NeuralRegion& self, size_t size, NeuronType type = NeuronType::Internal) {
            try {
                return self.addPopulation(size, type);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion addPopulation error: ") + e.what());
            }
        }, py::arg("size"), py::arg("type") = NeuronType::Internal, "Add neural population to region")
        .def("getPopulation", [](NeuralRegion& self, PopulationId id) {
            try {
                return self.getPopulation(id);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion getPopulation error: ") + e.what());
            }
        }, py::arg("id"), "Get population by ID")
        .def("getPopulationCount", &NeuralRegion::getPopulationCount)
        .def("getPopulations", &NeuralRegion::getPopulations)
        .def("getAllPopulations", [](NeuralRegion& self) {
            try {
                return self.getAllPopulations();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion getAllPopulations error: ") + e.what());
            }
        })
        .def("addSynapse", [](NeuralRegion& self, NeuronId source, NeuronId destination, 
                           SynapticWeight weight = 0.0f, Delay delay = 1) {
            try {
                if (delay == 0) {
                    throw std::invalid_argument("Synapse delay cannot be zero");
                }
                return self.addSynapse(source, destination, weight, delay);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion addSynapse error: ") + e.what());
            }
        }, py::arg("source"), py::arg("destination"), py::arg("weight") = 0.0f, py::arg("delay") = 1,
             "Add synapse connection within region")
        .def("getSynapse", [](NeuralRegion& self, SynapseId id) {
            try {
                return self.getSynapse(id);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion getSynapse error: ") + e.what());
            }
        }, py::arg("id"), "Get synapse by ID")
        .def("getSynapseCount", &NeuralRegion::getSynapseCount)
        .def("getSynapses", &NeuralRegion::getSynapses)
        .def("getSynapsesFrom", [](NeuralRegion& self, NeuronId neuron) {
            try {
                return self.getSynapsesFrom(neuron);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion getSynapsesFrom error: ") + e.what());
            }
        }, py::arg("neuron"), "Get synapses originating from neuron")
        .def("getSynapsesTo", [](NeuralRegion& self, NeuronId neuron) {
            try {
                return self.getSynapsesTo(neuron);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion getSynapsesTo error: ") + e.what());
            }
        }, py::arg("neuron"), "Get synapses terminating at neuron")
        .def("getTotalNeuronCount", &NeuralRegion::getTotalNeuronCount)
        .def("getActiveNeuronCount", &NeuralRegion::getActiveNeuronCount)
        .def("getFiringNeuronCount", &NeuralRegion::getFiringNeuronCount)
        .def("getAverageFiringRate", &NeuralRegion::getAverageFiringRate)
        .def("getAverageSynapticWeight", &NeuralRegion::getAverageSynapticWeight)
        .def("getSynapticDensity", &NeuralRegion::getSynapticDensity)
        .def("step", [](NeuralRegion& self, Timestamp currentTime) {
            try {
                self.step(currentTime);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion step error: ") + e.what());
            }
        }, py::arg("currentTime"), "Step all neurons and synapses in region")
        .def("reset", &NeuralRegion::reset, "Reset all neurons and synapses to initial state")
        .def("initializeRandomConnectivity", [](NeuralRegion& self, class RandomGenerator& rng, 
                                              float connectionProbability, float meanWeight, float weightVariance) {
            try {
                self.initializeRandomConnectivity(rng, connectionProbability, meanWeight, weightVariance);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion initializeRandomConnectivity error: ") + e.what());
            }
        }, py::arg("rng"), py::arg("connectionProbability"), py::arg("meanWeight"), py::arg("weightVariance"),
             "Initialize random connectivity within region")
        .def("getAllNeurons", [](NeuralRegion& self) {
            try {
                return self.getAllNeurons();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("NeuralRegion getAllNeurons error: ") + e.what());
            }
        }, "Get all neurons in this region");

    // World class (continued from bindings.cpp)
    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain with integrated functionality.
        
        Brain is the main computational engine of the NLM system, implementing
        real spiking neural computation with event-driven dynamics and
        integrated memory, prediction, cognition, and neuromodulation systems.
        
        **Core Capabilities:**
        - Neural Computation: LIF neuron dynamics with spike propagation
        - Memory Systems: Working memory, episodic memory, associative memory
        - Plasticity: STDP, Hebbian, structural synaptic learning
        - Neuromodulation: Dopamine, curiosity, novelty detection
        - Prediction: Forward models and prediction error computation
        - Cognition: Planning, concept formation, attentional selection
        - Development: Age-dependent plasticity and maturation
        
        **Key Interfaces:**
        - Configuration: Load/save brain settings
        - Simulation: Step through time with sensory input/output
        - Memory: Access working memory, episodic memory systems
        - Plasticity: Enable/disable learning mechanisms
        - Development: Control developmental stage and plasticity rates
        - Serialization: Save/load brain state for checkpointing
        
        **Usage Examples:**
        
        Basic Brain Setup:
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> config.set("brain.neuron_count", 1000)
        >>> config.set("brain.synapse_density", 0.1)
        >>> brain = pynlm.Brain(config)
        >>> brain.initialize()
        
        Brain Simulation Loop:
        >>> for step in range(1000):
        ...     # Inject sensory input
        ...     brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)
        ...     
        ...     # Run neural computation
        ...     brain.step(step)
        ...     
        ...     # Get motor output
        ...     action = brain.produceAction()
        ...     
        ...     # Log statistics periodically
        ...     if step % 100 == 0:
        ...         print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        Brain with Learning:
        >>> brain.enablePlasticity(True, True, False)  # STDP + Hebbian
        >>> 
        >>> for step in range(5000):
        ...     # Apply reward modulation
        ...     brain.applyNeuromodulation(reward_signal)
        ...     
        ...     # Run with learning enabled
        ...     brain.step(step)
        ...     
        >>> # After training, check plasticity effects
        >>> brain.logStatus()
        
        Brain State Management:
        >>> # Save trained brain
        >>> brain.save("trained_brain.bin")
        >>> 
        >>> # Load brain for inference
        >>> new_brain = pynlm.Brain(config)
        >>> new_brain.initialize()
        >>> new_brain.load("trained_brain.bin")
    )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"))
        .def("initialize", [](Brain& self) {
            try {
                return self.initialize();
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain initialization error: ") + e.what());
            }
        }, "Initialize the brain with configuration (throws ValueError on failure)")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step with step count")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp")
        .def("receiveSensoryInput", [](Brain& self, const SensoryInput& input) {
            try {
                self.receiveSensoryInput(input);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain receiveSensoryInput error: ") + e.what());
            }
        }, py::arg("input"), "Inject sensory input into brain with error handling")
        .def("injectCurrent", [](Brain& self, NeuronId neuron, MembranePotential current) {
            try {
                self.injectCurrent(neuron, current);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain injectCurrent error: ") + e.what());
            }
        }, py::arg("neuron"), py::arg("current"), "Inject current into specific neuron")
        .def("injectCurrentToNeurons", [](Brain& self, NeuronType type, MembranePotential current) {
            try {
                self.injectCurrentToNeurons(type, current);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain injectCurrentToNeurons error: ") + e.what());
            }
        }, py::arg("type"), py::arg("current"), "Inject current into all neurons of type")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity")
        .def("reset", &Brain::reset, "Reset brain state to initial conditions")
        .def("save", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.save(filepath)) {
                    throw std::runtime_error("Brain save failed for file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain save error: ") + e.what());
            }
        }, py::arg("filepath"), "Save brain state to file with error handling")
        .def("load", [](Brain& self, const std::string& filepath) {
            try {
                if (!self.load(filepath)) {
                    throw std::runtime_error("Brain load failed for file: " + filepath);
                }
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain load error: ") + e.what());
            }
        }, py::arg("filepath"), "Load brain state from file with error handling")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region to the brain")
        .def("getRegion", [](Brain& self, RegionId id) {
            try {
                return self.getRegion(id);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain getRegion error: ") + e.what());
            }
        }, py::arg("id"), "Get region by ID")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions in the brain")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs in the brain")
        .def("getRegions", &Brain::getRegions,
             "Get all regions in the brain")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all brain regions")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all brain regions")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons across all regions")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons across all regions")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count across all neurons")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage of the brain")
        .def("setDevelopmentalStage", [](Brain& self, DevelopmentalStage stage) {
            try {
                self.setDevelopmentalStage(stage);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("Brain setDevelopmentalStage error: ") + e.what());
            }
        }, py::arg("stage"), "Set developmental stage with error handling")
        .def("getConfig", &Brain::getConfig,
             "Get the configuration used to create this brain")
        .def("logStatus", &Brain::logStatus,
             "Log brain status and statistics for debugging");

    // AgentBrain class with comprehensive interface
    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(
        Agent brain interface connecting NLM brain to simulation world.
        
        AgentBrain provides the essential interface for agents to interact
        with their environment through their neural brain. It handles
        sensory processing, motor decoding, and learning integration.
        
        **Core Functionality:**
        
        **Sensory Processing:**
        - Input: processSensoryInput(percept) receives environmental data
        - Output: decodeMotorCommand() generates behavioral actions
        - Integration: Applies sensory input to brain neural network
        
        **Motor Control:**
        - Decoding: Translates neural activity to motor commands
        - Output: Produces Action objects for world interaction
        - Timing: Synchronizes with world simulation timestep
        
        **Learning Integration:**
        - Reward Modulation: applyRewardModulation() for reinforcement learning
        - Development: updateDevelopment() for age-dependent changes
        - Plasticity: Enables/Disables various learning mechanisms
        
        **World Connection:**
        - Initialization: initialize(world) establishes agent-world link
        - Access: getBrain() provides underlying neural engine
        - Monitoring: Curiosity, novelty, prediction error metrics
        
        **Learning Subsystem Control:**
        - enableRewardModulation(bool): Enable reward-based learning
        - enableStructuralPlasticity(bool): Enable connection formation/pruning
        - enableDevelopment(bool): Enable developmental plasticity
        - enableCuriosity(bool): Enable exploration-driven learning
        
        **Usage Examples:**
        
        Basic Agent Setup:
        >>> import pynlm
        >>> config = pynlm.Config()
        >>> brain = pynlm.Brain(config)
        >>> brain.initialize()
        >>> 
        >>> agent = pynlm.AgentBrain(brain)
        >>> world = pynlm.SimpleWorld()
        >>> world.configure(width=20, height=20)
        >>> world.reset()
        >>> 
        >>> agent.initialize(world)
        >>> 
        >>> # Enable learning subsystems
        >>> agent.enableRewardModulation(True)
        >>> agent.enableStructuralPlasticity(True)
        >>> agent.enableDevelopment(True)
        >>> agent.enableCuriosity(True)
        
        Agent Simulation Loop:
        >>> for step in range(500):
        ...     # Update world physics and state
        ...     world.update(0.1)
        ...     
        ...     # Get what the agent perceives
        ...     percept = world.getSensoryPercept()
        ...     
        ...     # Process sensory input in the brain
        ...     agent.processSensoryInput(percept)
        ...     
        ...     # Let the brain compute
        ...     brain.step(step)
        ...     
        ...     # Decode motor command from brain activity
        ...     motor_cmd = agent.decodeMotorCommand()
        ...     
        ...     # Apply action to the world
        ...     world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        Learning Agent:
        >>> for step in range(1000):
        ...     world.update(0.1)
        ...     agent.processSensoryInput(world.getSensoryPercept())
        ...     brain.step(step)
        ...     action = agent.decodeMotorCommand()
        ...     world.applyMotorCommand(action, world.getSimulationTime())
        ...     
        ...     if step % 100 == 0:
        ...         print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}")
        ...         print(f"          Novelty={agent.getNoveltyLevel():.3f}")
        ...         print(f"          Prediction error={agent.getPredictionError():.3f}")
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"))
        .def("initialize", [](AgentBrain& self, SimpleWorld& world) {
            try {
                self.initialize(world);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain initialize error: ") + e.what());
            }
        }, py::arg("world"), "Initialize agent with world environment")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size for this brain configuration")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size for this brain configuration")
        .def("processSensoryInput", [](AgentBrain& self, const SensoryPercept& percept) {
            try {
                self.processSensoryInput(percept);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain processSensoryInput error: ") + e.what());
            }
        }, py::arg("percept"), "Process sensory percept and inject into brain")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command")
        .def("applyRewardModulation", [](AgentBrain& self, float reward, float predictedReward) {
            try {
                self.applyRewardModulation(reward, predictedReward);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain applyRewardModulation error: ") + e.what());
            }
        }, py::arg("reward"), py::arg("predictedReward"),
             "Apply reward-based neuromodulation with validation")
        .def("updateDevelopment", [](AgentBrain& self, float timestep) {
            try {
                self.updateDevelopment(timestep);
            } catch (const std::exception& e) {
                throw py::value_error(std::string("AgentBrain updateDevelopment error: ") + e.what());
            }
        }, py::arg("timestep"), "Update development system with timestep")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get current neuromodulation level")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level (exploration drive)")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level (unexpectedness detection)")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error signal")
        .def("reset", &AgentBrain::reset, "Reset agent for new episode")
        .def("getBrain", &AgentBrain::getBrain,
             "Get the underlying brain object")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation,
             "Enable/disable reward modulation learning")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity,
             "Enable/disable structural plasticity (connection formation)")
        .def("enableDevelopment", &AgentBrain::enableDevelopment,
             "Enable/disable developmental changes")
        .def("enableCuriosity", &AgentBrain::enableCuriosity,
             "Enable/disable curiosity-driven exploration")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
             "Check if reward modulation is enabled")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
             "Check if structural plasticity is enabled")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
             "Check if development is enabled")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
             "Check if curiosity is enabled");

    // Python factory functions with comprehensive documentation

    m.def("createDefaultConfig", []() -> std::shared_ptr<Config> {
        return std::make_shared<Config>();
    }, R"pbdoc(
        Create a default configuration object.
        
        Returns:
            std::shared_ptr<Config>: Shared pointer to default Config instance
            
        This creates a configuration with sensible default values for:
        - Neural network parameters (neuron count, connectivity)
        - Plasticity mechanisms (STDP, Hebbian, structural)
        - Simulation settings (timestep, integration method)
        - Neuromodulation (dopamine, curiosity, novelty)
        
        Examples:
          >>> config = pynlm.createDefaultConfig()
          >>> config.set("brain.neuron_count", 1000)
          >>> config.set("plasticity.stdp.enabled", True)
          >>> 
          >>> brain = pynlm.createBrain(config)
          >>> brain.initialize()
    )pbdoc");

    m.def("createBrain", [](std::shared_ptr<Config> config) -> std::shared_ptr<Brain> {
        try {
            if (!config) {
                throw std::invalid_argument("Configuration cannot be null");
            }
            return std::make_shared<Brain>(config);
        } catch (const std::exception& e) {
            throw py::value_error(std::string("createBrain error: ") + e.what());
        }
    }, py::arg("config"), R"pbdoc(
        Create a new brain with the given configuration.
        
        Args:
            config: Configuration object specifying brain parameters
            
        Returns:
            std::shared_ptr<Brain>: Shared pointer to new Brain instance
            
        Raises:
            ValueError: If configuration is invalid or null
            
        Examples:
          >>> config = pynlm.createDefaultConfig()
          >>> config.set("brain.neuron_count", 500)
          >>> config.set("brain.synapse_density", 0.05)
          >>> 
          >>> brain = pynlm.createBrain(config)
          >>> brain.initialize()
    )pbdoc");

    m.def("createSimpleWorld", []() -> std::shared_ptr<SimpleWorld> {
        return std::make_shared<SimpleWorld>();
    }, R"pbdoc(
        Create a new simple 2D simulation world.
        
        Returns:
            std::shared_ptr<SimpleWorld>: Shared pointer to new SimpleWorld instance
            
        This creates a basic 2D environment with:
        - Grid-based spatial representation
        - Agent movement and physics
        - Sensory input (vision, proprioception)
        - Resource and hazard objects
        
        Examples:
          >>> world = pynlm.createSimpleWorld()
          >>> world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
          >>> world.reset()
          >>> world.setAgentStart(10.0, 10.0)
          >>> 
          >>> # Run simulation
          >>> for step in range(100):
          ...     world.update(0.1)
          ...     percept = world.getSensoryPercept()
          ...     agent.processSensoryInput(percept)
          ...     brain.step(step)
          ...     action = agent.decodeMotorCommand()
          ...     world.applyMotorCommand(action, world.getSimulationTime())
    )pbdoc");

    m.def("createAgentBrain", [](std::shared_ptr<Brain> brain) -> std::shared_ptr<AgentBrain> {
        try {
            if (!brain) {
                throw std::invalid_argument("Brain cannot be null");
            }
            return std::make_shared<AgentBrain>(brain);
        } catch (const std::exception& e) {
            throw py::value_error(std::string("createAgentBrain error: ") + e.what());
        }
    }, py::arg("brain"), R"pbdoc(
        Create a new agent brain interface for a brain object.
        
        Args:
            brain: Brain object to interface with the world
            
        Returns:
            std::shared_ptr<AgentBrain>: Shared pointer to new AgentBrain instance
            
        Raises:
            ValueError: If brain is null
            
        Examples:
          >>> config = pynlm.createDefaultConfig()
          >>> brain = pynlm.createBrain(config)
          >>> brain.initialize()
          >>> 
          >>> agent = pynlm.createAgentBrain(brain)
          >>> 
          >>> # Initialize with world
          >>> world = pynlm.createSimpleWorld()
          >>> world.configure(10, 10, 8, 8)
          >>> agent.initialize(world)
          >>> 
          >>> # Enable learning
          >>> agent.enableRewardModulation(True)
          >>> agent.enableCuriosity(True)
    )pbdoc");

    // Python function overloads with error handling
    m.def("createBrain", [](int neuronCount, float synapseDensity) -> std::shared_ptr<Brain> {
        auto config = std::make_shared<Config>();
        config->set("brain.neuron_count", neuronCount);
        config->set("brain.synapse_density", synapseDensity);
        return std::make_shared<Brain>(config);
    }, py::arg("neuronCount"), py::arg("synapseDensity"), R"pbdoc(
        Create a brain with specified parameters (convenience overload).
        
        Args:
            neuronCount: Number of neurons in the brain
            synapseDensity: Connection density (0.0 to 1.0)
            
        Returns:
            std::shared_ptr<Brain>: Shared pointer to new Brain instance
            
        Examples:
          >>> # Quick brain creation
          >>> brain = pynlm.createBrain(1000, 0.1)
          >>> brain.initialize()
    )pbdoc");

    // Python attributes for invalid ID constants
    m.attr("INVALID_NEURON_ID") = py::cast(INVALID_NEURON_ID);
    m.attr("INVALID_SYNAPSE_ID") = py::cast(INVALID_SYNAPSE_ID);
    m.attr("INVALID_REGION_ID") = py::cast(INVALID_REGION_ID);
    m.attr("INVALID_POPULATION_ID") = py::cast(INVALID_POPULATION_ID);

    // Python version attribute
    m.attr("__version__") = "1.0.0";
    
    // Python module docstring
    m.doc() = R"pbdoc(
        NLM (熙然) - Python Bindings for Neural Learning Machine
        
        A comprehensive Python interface to the NLM brain simulation framework.
        
        This module enables researchers, students, and developers to create,
        control, and analyze artificial neural networks using Python's
        interactive capabilities and extensive scientific computing ecosystem.
        
        Key Features:
        - **Complete API**: Full access to all NLM brain components
        - **Error Handling**: Comprehensive exception handling and validation
        - **Documentation**: Detailed docstrings and examples
        - **Performance**: Efficient C++ backend with Python convenience
        - **Extensible**: Easy to extend with new functionality
        
        Quick Start:
        1. Import the module
        2. Create configuration
        3. Initialize brain and world
        4. Run simulation loop
        5. Analyze results
        
        For detailed documentation, examples, and API reference, visit:
        https://github.com/your-org/nlm
        
        Tutorial Examples:
        - Basic Brain Simulation
        - Agent-Environment Interaction
        - Learning and Plasticity
        - Network Analysis
        
        Installation:
        pip install pynlm
        
        Build from source:
        git clone https://github.com/your-org/nlm.git
        cd nlm
        mkdir build
        cd build
        cmake .. -DCMAKE_BUILD_TYPE=Release
        make -j4
    )pbdoc";
}

} // namespace nlm
