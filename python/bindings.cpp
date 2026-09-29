#include "../src/core/Errors/Errors.hpp"

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

    // Register NLM exception hierarchy
    py::register_exception<NLMError>(m, "NLMError");
    py::register_exception<ConfigurationError>(m, "ConfigurationError", py::module_local());
    py::register_exception<InitializationError>(m, "InitializationError", py::module_local());
    py::register_exception<MemoryError>(m, "MemoryError", py::module_local());
    py::register_exception<ValidationError>(m, "ValidationError", py::module_local());
    py::register_exception<FileError>(m, "FileError", py::module_local());
    py::register_exception<PlasticityError>(m, "PlasticityError", py::module_local());
    py::register_exception<NetworkError>(m, "NetworkError", py::module_local());
    py::register_exception<SimulationError>(m, "SimulationError", py::module_local());
    py::register_exception<SystemError>(m, "SystemError", py::module_local());

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
        
        The NeuronType enumeration defines the different types of neurons in the NLM system.
        Each neuron type has specific characteristics and functions in neural computation.
        
        Usage example:
            import pynlm
            
            # Use neuron types in brain configuration
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("brain.region.neuronTypes", "Excitatory,Inhibitory,Modulatory")
            
            # Check neuron type properties
            neuron_types = [pynlm.NeuronType.Excitatory, pynlm.NeuronType.Inhibitory, 
                           pynlm.NeuronType.Modulatory, pynlm.NeuronType.Sensory,
                           pynlm.NeuronType.Motor, pynlm.NeuronType.Internal]
            
            for neuron_type in neuron_types:
                print(f"Type: {neuron_type}")
                # Use in brain operations
                # brain.injectCurrentToNeurons(neuron_type, current_value)
        
        Values:
            Excitatory: Neurons that increase target neuron firing probability (typical glutamatergic)
            Inhibitory: Neurons that decrease target neuron firing probability (typical GABAergic)
            Modulatory: Neurons that affect neuron state without direct excitation/inhibition (e.g., dopaminergic)
            Sensory: Neurons that process sensory input (afferent)
            Motor: Neurons that generate motor output (efferent)
            Internal: Neurons that maintain internal brain states and homeostatic functions
        
        Raises:
            RuntimeError: If invalid neuron type is used in operations
        
        Note:
            Different neuron types have different electrical and synaptic properties.
            Network dynamics emerge from the balance of excitation and inhibition.
            Sensory neurons receive external input, motor neurons send output.
            Internal neurons maintain brain state and regulate development.
    )pbdoc")
        .value("Excitatory", NeuronType::Excitatory)
        .value("Inhibitory", NeuronType::Inhibitory)
        .value("Modulatory", NeuronType::Modulatory)
        .value("Sensory", NeuronType::Sensory)
        .value("Motor", NeuronType::Motor)
        .value("Internal", NeuronType::Internal)
        .export_values();

    py::enum_<SynapseType>(m, "SynapseType", R"pbdoc(Synapse type enumeration)
        
        The SynapseType enumeration defines the different types of synaptic connections
        in the NLM system. Each synapse type has specific transmission properties
        and plasticity mechanisms.
        
        Usage example:
            import pynlm
            
            # Use synapse types in brain configuration
            brain_config = pynlm.createDefaultConfig()
            brain_config.set("brain.region.synapseTypes", "Excitatory,Inhibitory,Modulatory")
            
            # Create different synapse types
            excitatory_synapse = pynlm.SynapseType.Excitatory
            inhibitory_synapse = pynlm.SynapseType.Inhibitory
            electrical_synapse = pynlm.SynapseType.Electrical
            gap_junction = pynlm.SynapseType.GapJunction
            
            # Check synapse type properties
            synapse_types = [pynlm.SynapseType.Excitatory, pynlm.SynapseType.Inhibitory,
                           pynlm.SynapseType.Modulatory, pynlm.SynapseType.Electrical,
                           pynlm.SynapseType.GapJunction]
            
            for syn_type in synapse_types:
                print(f"Synapse type: {syn_type}")
                # Use in brain operations
                # brain.addSynapse(source, target, syn_type)
        
        Values:
            Excitatory: Chemical synapses that increase target neuron depolarization (positive AMPA-like)
            Inhibitory: Chemical synapses that decrease target neuron depolarization (negative GABA-like)
            Modulatory: Chemical synapses that affect neuron state through G-protein coupled receptors
            Electrical: Gap junctions that allow direct electrical coupling between neurons
            GapJunction: Specialized electrical synapses with fast transmission and low plasticity
        
        Raises:
            RuntimeError: If invalid synapse type is used in connection formation
        
        Note:
            Different synapse types have different transmission speeds, strengths, and plasticity rules.
            Electrical synapses enable synchronized firing patterns across neuron populations.
            Chemical synapses allow for more complex computational operations through neuromodulation.
            Network dynamics depend on the balance and distribution of synapse types.
    )pbdoc")
        .value("Excitatory", SynapseType::Excitatory)
        .value("Inhibitory", SynapseType::Inhibitory)
        .value("Modulatory", SynapseType::Modulatory)
        .value("Electrical", SynapseType::Electrical)
        .value("GapJunction", SynapseType::GapJunction)
        .export_values();

    py::enum_<DevelopmentalStage>(m, "DevelopmentalStage", R"pbdoc(Developmental stage enumeration)
        
        The DevelopmentalStage enumeration defines the different stages of neural
        development in the NLM system. Each stage represents a distinct period
        with specific neural plasticity and learning characteristics.
        
        Usage example:
            import pynlm
            
            # Monitor brain development
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            for step in range(1000):
                brain.step(step)
                
                if step % 100 == 0:
                    stage = brain.getDevelopmentalStage()
                    print(f"Step {step}: Developmental stage = {stage}")
                    
                    # Adapt behavior based on developmental stage
                    if stage == pynlm.DevelopmentalStage.CriticalPeriod:
                        # Enhance learning during critical period
                        print("Critical period: Enhancing learning rate")
                    elif stage == pynlm.DevelopmentalStage.Maturation:
                        # Stabilize learned behaviors
                        print("Maturation: Stabilizing neural connections")
                    elif stage == pynlm.DevelopmentalStage.Aging:
                        # Account for declining performance
                        print("Aging: Adjusting expectations")
        
        Values:
            Initial: Newborn stage with high plasticity and basic connectivity
            CriticalPeriod: Period of rapid learning and experience-dependent wiring
            Maturation: Refinement of connections, stabilization of learned behaviors
            Adult: Mature neural system with balanced plasticity and stability
            Aging: Declining plasticity and efficiency with accumulated experience
        
        Raises:
            RuntimeError: If invalid developmental stage is set
        
        Note:
            Development stages affect plasticity rules, learning rates, and
            neural competition. Critical periods are windows of enhanced learning.
            Development is controlled by the DevelopmentSystem and influenced
            by environmental factors and neuromodulators.
    )pbdoc")
        .value("Initial", DevelopmentalStage::Initial)
        .value("CriticalPeriod", DevelopmentalStage::CriticalPeriod)
        .value("Maturation", DevelopmentalStage::Maturation)
        .value("Adult", DevelopmentalStage::Adult)
        .value("Aging", DevelopmentalStage::Aging)
        .export_values();

    py::enum_<FiringState>(m, "FiringState", R"pbdoc(Neuron firing state enumeration)
        
        The FiringState enumeration defines the different electrical states of neurons
        in the NLM system. These states represent the dynamic behavior of individual
        neurons during simulation.
        
        Usage example:
            import pynlm
            
            # Monitor neuron firing states
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Get statistics about neuron states
            for step in range(100):
                brain.step(step)
                
            active_count = brain.getFiringNeuronCount()
            total_count = brain.getTotalNeuronCount()
            
            print(f"Active neurons: {active_count}/{total_count}")
            print(f"Firing rate: {brain.getAverageFiringRate():.2f} Hz")
            
            # Check firing states indirectly through statistics
            # Individual neuron states would require access to NeuralRegion objects
            # from brain.getRegions()
            for region in brain.getRegions():
                print(f"Region {region.getName()}:")
                print(f"  Active neurons: {region.getActiveNeuronCount()}")
                print(f"  Firing neurons: {region.getFiringNeuronCount()}")
                print(f"  Average firing rate: {region.getAverageFiringRate():.2f} Hz")
        
        Values:
            Resting: Neuron at rest potential, not ready to fire
            Active: Neuron depolarized and capable of firing action potentials
            Refractory: Neuron recovering from firing, unable to fire again immediately
            Inhibited: Neuron suppressed by inhibitory input, unable to fire despite depolarization
        
        Raises:
            RuntimeError: If invalid firing state is encountered
        
        Note:
            Firing states represent the dynamic membrane potential of neurons.
            Transitions between states depend on input currents, ion channels,
            and synaptic inputs. The balance of firing states across the network
            determines overall brain activity and behavior.
    )pbdoc")
        .value("Resting", FiringState::Resting)
        .value("Active", FiringState::Active)
        .value("Refractory", FiringState::Refractory)
        .value("Inhibited", FiringState::Inhibited)
        .export_values();

    py::enum_<ActionType>(m, "ActionType", R"pbdoc(Action type enumeration)
        
        The ActionType enumeration defines the different types of motor actions
        that an agent can perform in the simulated environment. Each action type
        represents a specific behavioral output that can be generated by the brain.
        
        Usage example:
            import pynlm
            
            # Create and use different action types
            move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
            move_backward = pynlm.Action(pynlm.ActionType.MoveBackward)
            turn_left = pynlm.Action(pynlm.ActionType.TurnLeft)
            turn_right = pynlm.Action(pynlm.ActionType.TurnRight)
            look = pynlm.Action(pynlm.ActionType.Look)
            look_up = pynlm.Action(pynlm.ActionType.LookUp)
            look_down = pynlm.Action(pynlm.ActionType.LookDown)
            interact = pynlm.Action(pynlm.ActionType.Interact)
            eat = pynlm.Action(pynlm.ActionType.Eat)
            drink = pynlm.Action(pynlm.ActionType.Drink)
            rest = pynlm.Action(pynlm.ActionType.Rest)
            wait = pynlm.Action(pynlm.ActionType.Wait)
            
            # Categorize actions for agent behavior
            movement_actions = [
                pynlm.ActionType.MoveForward, pynlm.ActionType.MoveBackward,
                pynlm.ActionType.MoveLeft, pynlm.ActionType.MoveRight,
                pynlm.ActionType.TurnLeft, pynlm.ActionType.TurnRight
            ]
            
            sensory_actions = [
                pynlm.ActionType.Look, pynlm.ActionType.LookUp, pynlm.ActionType.LookDown
            ]
            
            interaction_actions = [
                pynlm.ActionType.Interact, pynlm.ActionType.Eat, pynlm.ActionType.Drink
            ]
            
            # Use action type in agent decision making
            if current_state == "hungry":
                action = pynlm.Action(pynlm.ActionType.Eat)
            elif current_state == "thirsty":
                action = pynlm.Action(pynlm.ActionType.Drink)
            elif current_state == "scared":
                action = pynlm.Action(pynlm.ActionType.Rest)
            else:
                action = pynlm.Action(pynlm.ActionType.MoveForward)
        
        Values:
            MoveForward: Move agent forward in current direction
            MoveBackward: Move agent backward
            MoveLeft: Move agent left
            MoveRight: Move agent right
            TurnLeft: Rotate agent counterclockwise
            TurnRight: Rotate agent clockwise
            Look: Look in current direction
            LookUp: Look upward
            LookDown: Look downward
            Interact: Interact with objects in vicinity
            Eat: Consume edible objects
            Drink: Consume drinkable objects
            Rest: Stop moving and performing actions
            Wait: Do nothing for one timestep
            Custom: Custom action with arbitrary parameters
        
        Raises:
            RuntimeError: If invalid action type is used
        
        Note:
            Action types map to motor commands and agent behaviors.
            Basic actions include locomotion (movement and rotation).
            Sensory actions include looking in different directions.
            Interaction actions involve manipulating the environment.
            Survival actions include eating, drinking, and resting.
            Wait is useful for deliberating or avoiding unnecessary actions.
    )pbdoc")
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

    py::enum_<MotorCommand>(m, "MotorCommand", R"pbdoc(Low-level motor command enumeration)
        
        The MotorCommand enumeration defines low-level motor commands that are
        directly executed by the world simulation. These commands are generated
        from Action objects by the AgentBrain's decodeMotorCommand method.
        
        Usage example:
            import pynlm
            
            # Create agent brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            agent_brain = pynlm.createAgentBrain(brain)
            
            # Create different action types
            actions = [
                pynlm.Action(pynlm.ActionType.MoveForward),
                pynlm.Action(pynlm.ActionType.TurnLeft),
                pynlm.Action(pynlm.ActionType.TurnRight),
                pynlm.Action(pynlm.ActionType.Look),
                pynlm.Action(pynlm.ActionType.Interact),
                pynlm.Action(pynlm.ActionType.Wait)
            ]
            
            for action in actions:
                # Decode action to motor command
                motor_cmd = agent_brain.decodeMotorCommand(action)
                print(f"Action {action.getType()} -> Motor command: {motor_cmd}")
                
                # Execute in world
                world = pynlm.createSimpleWorld()
                world.configure(100, 100, 20, 20)
                world.setAgentStart(50, 50)
                world.applyMotorCommand(motor_cmd, step=0)
            
            # Execute multiple motor commands
            motor_commands = [
                pynlm.MotorCommand.MoveForward,
                pynlm.MotorCommand.TurnLeft,
                pynlm.MotorCommand.LookLeft,
                pynlm.MotorCommand.Interact
            ]
            
            for cmd in motor_commands:
                print(f"Executing motor command: {cmd}")
                # world.applyMotorCommand(cmd, currentTime)
        
        Values:
            MoveForward: Move agent forward one unit
            MoveBackward: Move agent backward one unit
            TurnLeft: Rotate agent counterclockwise 90 degrees
            TurnRight: Rotate agent clockwise 90 degrees
            LookLeft: Look left direction
            LookRight: Look right direction
            Interact: Interact with object in current cell
            Wait: Do nothing for one timestep
        
        Raises:
            RuntimeError: If invalid motor command is executed
        
        Note:
            Motor commands are the low-level implementation of actions.
            They are executed directly by the world simulation and
            affect agent position, orientation, and interaction state.
            Motor commands are independent of action types and can be
            used directly for agent control or testing.
    )pbdoc")
        .value("MoveForward", MotorCommand::MoveForward)
        .value("MoveBackward", MotorCommand::MoveBackward)
        .value("TurnLeft", MotorCommand::TurnLeft)
        .value("TurnRight", MotorCommand::TurnRight)
        .value("LookLeft", MotorCommand::LookLeft)
        .value("LookRight", MotorCommand::LookRight)
        .value("Interact", MotorCommand::Interact)
        .value("Wait", MotorCommand::Wait)
        .export_values();

    py::enum_<WorldObjectType>(m, "WorldObjectType", R"pbdoc(World object type enumeration)
        
        The WorldObjectType enumeration defines the different types of entities
        that can exist in the simulated world. Each object type has specific
        behavior, interaction rules, and properties.
        
        Usage example:
            import pynlm
            
            # Create different world object types
            world_objects = [
                pynlm.WorldObjectType.Empty,    # Passable, no effect
                pynlm.WorldObjectType.Resource, # Collectible, provides value
                pynlm.WorldObjectType.Hazard,   # Damaging, reduces agent health
                pynlm.WorldObjectType.Wall,    # Impassable boundary
                pynlm.WorldObjectType.Marker   # Landmark for navigation
            ]
            
            for obj_type in world_objects:
                # Create object of this type
                obj = pynlm.WorldObject(0, 0, obj_type, 1.0, 0.5)
                print(f"Object type: {obj_type}")
                print(f"  Properties: x={obj.x}, y={obj.y}, value={obj.value}, radius={obj.radius}")
                
                # Set additional properties based on type
                if obj_type == pynlm.WorldObjectType.Resource:
                    obj.value = 10.0  # Resource value
                    obj.active = True
                elif obj_type == pynlm.WorldObjectType.Hazard:
                    obj.value = 0.5   # Hazard danger level
                    obj.active = True
                elif obj_type == pynlm.WorldObjectType.Wall:
                    obj.active = True
                elif obj_type == pynlm.WorldObjectType.Marker:
                    obj.value = 123  # Marker identifier
                    obj.active = True
            
            # Add objects to world based on type
            world = pynlm.createSimpleWorld()
            world.configure(100, 100, 20, 20)
            
            # Place a resource
            resource = pynlm.WorldObject(10, 10, pynlm.WorldObjectType.Resource, 50.0)
            world.addObject(resource)
            
            # Place a wall
            wall = pynlm.WorldObject(50, 30, pynlm.WorldObjectType.Wall, 0.0, 0.8)
            world.addObject(wall)
            
            # Place a marker
            marker = pynlm.WorldObject(90, 90, pynlm.WorldObjectType.Marker, 999)
            world.addObject(marker)
        
        Values:
            Empty: Passable, no effect on agent (e.g., floor)
            Resource: Collectible item that provides value to agent (e.g., food, water)
            Hazard: Dangerous object that reduces agent health (e.g., fire, trap)
            Wall: Impassable boundary that blocks agent movement
            Marker: Landmark or target point for navigation
        
        Raises:
            RuntimeError: If invalid world object type is used
        
        Note:
            WorldObjectType determines object behavior in world simulation.
            Resource objects can be collected for positive value.
            Hazard objects cause damage when contacted.
            Wall objects block agent movement and collision.
            Marker objects serve as reference points for navigation.
            Empty objects are passable spaces for agent movement.
            Object type affects AI decision making and agent behavior.
    )pbdoc")
        .value("Empty", WorldObjectType::Empty)
        .value("Resource", WorldObjectType::Resource)
        .value("Hazard", WorldObjectType::Hazard)
        .value("Wall", WorldObjectType::Wall)
        .value("Marker", WorldObjectType::Marker)
        .export_values();

    py::class_<Config>(m, "Config", R"pbdoc(Configuration class for NLM system)
        
        The Config class manages neural simulation parameters and settings.
        It provides a key-value store for configuration values and methods
        to load/save configuration from JSON files or command line arguments.
        
        Usage example:
            import pynlm
            config = pynlm.createDefaultConfig()
            config.set("brain.neuronCount", 1000)
            config.set("brain.visionWidth", 640)
            config.set("brain.visionHeight", 480)
        
        Attributes:
            Keys are accessed via the has() and get() methods.
            Common configuration keys include:
            - brain.neuronCount: Number of neurons in the brain
            - brain.sensory.inputSize: Size of sensory input vector
            - brain.sensory.outputSize: Size of motor output vector
            - brain.simulation.stepRate: Simulation step rate (Hz)
            - brain.simulation.maxSteps: Maximum simulation steps
            - brain.plasticity.enable: Enable plasticity
            - brain.development.enable: Enable developmental processes
        
        Methods:
            loadFromFile(): Load configuration from JSON file
            loadFromArgs(): Load configuration from command line
            saveToFile(): Save configuration to JSON file
            has(): Check if key exists
            getKeys(): Get all configuration keys
            clear(): Clear all configuration entries
            summary(): Get configuration summary
        
        Raises:
            RuntimeError: If file I/O fails or JSON parsing fails
        
        Note:
            Configuration values are type-agnostic and stored as strings.
            Use appropriate type conversion when accessing values.
    )pbdoc")
        .def(py::init<>())
        .def("loadFromFile", &Config::loadFromFile, py::arg("filepath"),
             "Load configuration from a JSON file. Args: filepath: Path to JSON configuration file. Returns: bool indicating success. Raises: RuntimeError if file cannot be read or parsed.")
        .def("loadFromArgs", [](Config& self, int argc, char** argv) {
            return self.loadFromArgs(argc, argv);
        }, py::arg("argc"), py::arg("argv"),
           "Load configuration from command line arguments. Args: argc: Argument count. argv: Argument vector. Returns: bool indicating success. Raises: RuntimeError if argument parsing fails.")
        .def("saveToFile", &Config::saveToFile, py::arg("filepath"),
             "Save configuration to a JSON file. Args: filepath: Path to save JSON file. Returns: bool indicating success. Raises: RuntimeError if file cannot be written.")
        .def("has", &Config::has, py::arg("key"),
             "Check if a configuration key exists. Args: key: Configuration key to check (e.g., 'brain.neuronCount'). Returns: bool indicating if key exists. Raises: RuntimeError if internal error occurs.")
        .def("getKeys", &Config::getKeys,
             "Get all configuration keys. Returns: List of configuration key strings. Raises: RuntimeError if internal error occurs.")
        .def("clear", &Config::clear,
             "Clear all configuration entries. Returns: None. Raises: RuntimeError if internal error occurs.")
        .def("summary", &Config::summary,
             "Get a summary string of the configuration. Returns: Configuration summary string. Raises: RuntimeError if internal error occurs.")
        .def("__repr__", [](const Config& cfg) {
            return "<Config: " + cfg.summary() + ">";
        });

    py::class_<SensoryInput>(m, "SensoryInput", R"pbdoc(Base class for sensory input)
        
        The SensoryInput class represents sensory input from the environment.
        This is an abstract base class for concrete sensory modalities such as
        Vision, Audio, and InternalSignals.
        
        Usage example:
            import pynlm
            
            # Create vision sensory input
            vision = pynlm.Vision(640, 480, 3)  # width, height, channels
            vision.setData([0.1, 0.2, 0.3, ...])  # Flattened image data
            
            # Create audio sensory input  
            audio = pynlm.Audio(44100, 16384)  # sampleRate, numSamples
            audio.setData([0.5, -0.2, 0.1, ...])  # Audio samples
            
            # Create internal signals
            internal = pynlm.InternalSignals()
            internal.addSignal(0.1)  # Add internal signal value
        
        Attributes:
            type: The type of sensory input (Vision, Audio, or InternalSignals)
            data: The sensory data as a list of floats
            dimensions: The dimensionality of the sensory data
            timestamp: The time at which the sensory data was generated
        
        Methods:
            getType(): Get the type of sensory input
            getData(): Get the raw data as a list
            getDimensions(): Get the dimensionality
            getTimestamp(): Get the timestamp
            setTimestamp(): Set the timestamp
        
        Raises:
            RuntimeError: If data is invalid or operations fail
        
        Note:
            Each sensory modality has its own specific implementation and data format.
            See Vision, Audio, and InternalSignals for concrete implementations.
    )pbdoc")
        .def("getType", &SensoryInput::getType, "Get the type of sensory input. Returns: str indicating sensory type (Vision, Audio, or InternalSignals).")
        .def("getData", &SensoryInput::getData, "Get the raw data as a list. Returns: List of float values representing sensory data.")
        .def("getDimensions", &SensoryInput::getDimensions, "Get the dimensionality. Returns: Integer representing data dimensions.")
        .def("getTimestamp", &SensoryInput::getTimestamp, "Get the timestamp. Returns: Float representing time in seconds.")
        .def("setTimestamp", &SensoryInput::setTimestamp, py::arg("timestamp"), "Set the timestamp. Args: timestamp: Time value in seconds.")

    py::class_<Vision, SensoryInput>(m, "Vision", R"pbdoc(Vision sensory input)
        
        The Vision class represents visual sensory input from the environment.
        It contains image data in RGB format and provides methods to access
        image dimensions and manipulate pixel data.
        
        Usage example:
            import pynlm
            
            # Create a 640x480 RGB image (standard camera resolution)
            vision = pynlm.Vision(640, 480, 3)
            
            # Create an image from raw data
            width, height, channels = 640, 480, 3
            pixel_count = width * height * channels
            image_data = [0.0] * pixel_count
            # Fill with your image data here...
            vision.setData(image_data)
            
            # Access image properties
            print(f"Image size: {vision.getWidth()}x{vision.getHeight()}")
            print(f"Channels: {vision.getChannels()}")
            print(f"Dimensions: {vision.getDimensions()}")
            
            # Set timestamp for temporal processing
            vision.setTimestamp(time.time())
        
        Attributes:
            width: Image width in pixels
            height: Image height in pixels  
            channels: Number of color channels (typically 3 for RGB)
            data: List of pixel values normalized to [0, 1] range
        
        Methods:
            __init__(width, height, channels=3): Constructor
            setData(data): Set image data as flattened array
            getWidth(): Get image width
            getHeight(): Get image height
            getChannels(): Get number of channels
            getType(): Inherited from SensoryInput (returns 'Vision')
            getData(): Inherited from SensoryInput
            getDimensions(): Inherited from SensoryInput
            getTimestamp(): Inherited from SensoryInput
            setTimestamp(): Inherited from SensoryInput
        
        Raises:
            RuntimeError: If width/height is zero, negative, or data size mismatch
        
        Note:
            Pixel data should be provided as a flattened array in row-major order:
            [R0,G0,B0, R1,G1,B1, ... R(h-1),G(h-1),B(h-1)]
            Values should be normalized to [0, 1] range for best results.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t, size_t>(), py::arg("width"), py::arg("height"),
             py::arg("channels") = 3,
             "Initialize vision input with dimensions. Args: width: Image width in pixels. height: Image height in pixels. channels: Number of color channels (default 3 for RGB). Returns: Vision object.")
        .def("setData", [](Vision& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set image data as flattened array. Args: data: List of pixel values in row-major order (R0,G0,B0, R1,G1,B1, ...). Expected size: width * height * channels. Raises: RuntimeError if data size mismatch.")
        .def("getWidth", &Vision::getWidth,
             "Get image width. Returns: Integer width in pixels.")
        .def("getHeight", &Vision::getHeight,
             "Get image height. Returns: Integer height in pixels.")
        .def("getChannels", &Vision::getChannels,
             "Get number of color channels. Returns: Integer number of channels (typically 3).")

    py::class_<Audio, SensoryInput>(m, "Audio", R"pbdoc(Audio sensory input)
        
        The Audio class represents auditory sensory input from the environment.
        It contains audio samples as floating-point values and provides methods
        to access audio properties and manipulate audio data.
        
        Usage example:
            import pynlm
            
            # Create audio input with standard CD quality
            audio = pynlm.Audio(44100, 44100)  # sampleRate, numSamples (1 second)
            
            # Create audio from sample data
            sample_count = 44100  # 1 second at 44.1kHz
            audio_data = [0.0] * sample_count
            # Fill with your audio data here...
            audio.setData(audio_data)
            audio.setSampleRate(44100)  # Sample rate in Hz
            
            # Access audio properties
            print(f"Sample rate: {audio.getSampleRate()} Hz")
            print(f"Duration: {audio.getNumSamples() / audio.getSampleRate():.2f} seconds")
            print(f"Samples: {audio.getNumSamples()}")
            print(f"Dimensions: {audio.getDimensions()}")
            
            # Set timestamp for temporal processing
            audio.setTimestamp(time.time())
        
        Attributes:
            sampleRate: Audio sample rate in Hz (e.g., 44100 for CD quality)
            numSamples: Number of audio samples in the buffer
            data: List of audio samples normalized to [-1, 1] range
        
        Methods:
            __init__(): Default constructor
            __init__(sampleRate, numSamples): Constructor with parameters
            setData(data): Set audio data as array
            setSampleRate(rate): Set sample rate
            getSampleRate(): Get sample rate
            getNumSamples(): Get number of samples
            getType(): Inherited from SensoryInput (returns 'Audio')
            getData(): Inherited from SensoryInput
            getDimensions(): Inherited from SensoryInput
            getTimestamp(): Inherited from SensoryInput
            setTimestamp(): Inherited from SensoryInput
        
        Raises:
            RuntimeError: If sampleRate is zero or negative, or data size mismatch
        
        Note:
            Audio samples should be normalized to [-1, 1] range for best results.
            Common sample rates include: 8000, 11025, 16000, 22050, 44100, 48000 Hz.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<size_t, size_t>(), py::arg("sampleRate"), py::arg("numSamples"),
             "Initialize audio input with parameters. Args: sampleRate: Audio sample rate in Hz. numSamples: Number of audio samples. Returns: Audio object.")
        .def("setData", [](Audio& self, const std::vector<float>& data) {
            self.setData(data);
        }, py::arg("data"),
             "Set audio data as array. Args: data: List of audio samples. Expected size: numSamples. Values should be in [-1, 1] range. Raises: RuntimeError if data size mismatch.")
        .def("setSampleRate", &Audio::setSampleRate, py::arg("sampleRate"),
             "Set audio sample rate. Args: sampleRate: Sample rate in Hz.")
        .def("getSampleRate", &Audio::getSampleRate,
             "Get audio sample rate. Returns: Sample rate in Hz.")
        .def("getNumSamples", &Audio::getNumSamples,
             "Get number of audio samples. Returns: Number of samples.")

    py::class_<InternalSignals, SensoryInput>(m, "InternalSignals", R"pbdoc(Internal signals sensory input)
        
        The InternalSignals class represents internal neural states and signals
        from the brain's internal processing systems. This includes neuromodulatory
        signals, prediction errors, curiosity levels, novelty signals, and other
        internal representations.
        
        Usage example:
            import pynlm
            
            # Create internal signals
            internal = pynlm.InternalSignals()
            
            # Add various internal signals
            internal.addSignal(0.1)   # Dopamine level (reward signal)
            internal.addSignal(0.5)   # Curiosity level (exploration drive)
            internal.addSignal(0.2)   # Novelty level (unexpectedness)
            internal.addSignal(-0.1)  # Prediction error
            internal.addSignal(0.3)   # Attention level
            internal.addSignal(0.7)   # Memory activation
            
            # Get all signals
            signals = internal.getData()
            print(f"Number of signals: {len(signals)}")
            for i, signal in enumerate(signals):
                print(f"  Signal {i}: {signal}")
            
            # Clear signals for next time step
            internal.clearSignals()
            
            # Set timestamp for temporal processing
            internal.setTimestamp(time.time())
        
        Attributes:
            signals: List of internal signal values
            timestamp: Time of signal generation
        
        Methods:
            __init__(): Default constructor
            addSignal(value): Add a new internal signal value
            clearSignals(): Clear all signals
            getType(): Inherited from SensoryInput (returns 'InternalSignals')
            getData(): Inherited from SensoryInput
            getDimensions(): Inherited from SensoryInput
            getTimestamp(): Inherited from SensoryInput
            setTimestamp(): Inherited from SensoryInput
        
        Raises:
            RuntimeError: If signal value is invalid or memory allocation fails
        
        Note:
            Internal signals typically represent continuous-valued neural states
            in the range [-1, 1]. Different brain regions and systems produce
            different types of internal signals (dopamine, serotonin, etc.).
    )pbdoc")
        .def(py::init<>())
        .def("addSignal", &InternalSignals::addSignal, py::arg("value"),
             "Add an internal signal value. Args: value: Signal value (typically in [-1, 1] range).")
        .def("clearSignals", &InternalSignals::clearSignals,
             "Clear all internal signals. Returns: None. Raises: RuntimeError if operation fails.")

    py::class_<Action>(m, "Action", R"pbdoc(Action representation for motor output)
        
        The Action class represents motor commands generated by the brain
        and executed by the agent. Actions include predefined types (MoveForward,
        TurnLeft, etc.) and custom action types with arbitrary parameters.
        
        Usage example:
            import pynlm
            
            # Create action from action type
            move_forward = pynlm.Action(pynlm.ActionType.MoveForward)
            print(f"Action type: {move_forward.getType()}")
            print(f"Action name: {move_forward.getName()}")
            
            # Create action with parameters (e.g., custom movement)
            custom_move = pynlm.Action(pynlm.ActionType.Custom, [0.5, 0.3, 0.8])
            print(f"Action type: {custom_move.getType()}")
            print(f"Parameters: {custom_move.getParameters()}")
            
            # Set action properties
            look_action = pynlm.Action(pynlm.ActionType.Look)
            look_action.setType(pynlm.ActionType.LookUp)
            
            # Clone an action
            cloned_action = look_action.clone()
        
        Attributes:
            type: The action type (ActionType enum value)
            parameters: List of action parameters (typically floats)
            name: Human-readable action name
        
        Methods:
            __init__(): Default constructor
            __init__(type): Constructor with action type
            __init__(type, parameters): Constructor with type and parameters
            getType(): Get the action type
            setType(type): Set the action type
            getParameters(): Get action parameters
            setParameters(params): Set action parameters
            getName(): Get human-readable action name
            clone(): Create a copy of the action
        
        Raises:
            RuntimeError: If action type is invalid or parameter size mismatch
        
        Note:
            Action parameters vary by action type. Look at ActionType enumeration
            for predefined action types. Custom actions can have arbitrary parameters.
            Common action types include movement, looking, interacting, and resting.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<ActionType>(), py::arg("type"),
             "Initialize action with type. Args: type: Action type (ActionType enum).")
        .def(py::init<ActionType, std::vector<float>>(), py::arg("type"), py::arg("parameters"),
             "Initialize action with type and parameters. Args: type: Action type. parameters: List of action parameters.")
        .def("getType", &Action::getType,
             "Get the action type. Returns: ActionType enum value.")
        .def("setType", &Action::setType, py::arg("type"),
             "Set the action type. Args: type: Action type (ActionType enum).")
        .def("getParameters", &Action::getParameters,
             "Get action parameters. Returns: List of action parameters.")
        .def("setParameters", &Action::setParameters, py::arg("params"),
             "Set action parameters. Args: params: List of action parameters.")
        .def("getName", &Action::getName,
             "Get human-readable action name. Returns: String name.")
        .def("clone", &Action::clone,
             "Create a copy of the action. Returns: Action object.");

    py::class_<WorldObject>(m, "WorldObject", R"pbdoc(World object representation)
        
        The WorldObject class represents entities in the simulated world environment.
        Objects can be resources (collectible items), hazards (dangerous areas),
        walls (boundaries), markers (landmarks), or empty spaces.
        
        Usage example:
            import pynlm
            
            # Create different types of world objects
            empty_space = pynlm.WorldObject()
            print(f"Empty space type: {empty_space.type}")
            
            resource = pynlm.WorldObject(2.0, 3.0, pynlm.WorldObjectType.Resource, 1.0, 0.3)
            print(f"Resource at ({resource.x}, {resource.y}) with value {resource.value}")
            
            hazard = pynlm.WorldObject(5.0, 5.0, pynlm.WorldObjectType.Hazard, 0.0, 0.5)
            print(f"Hazard at ({hazard.x}, {hazard.y}) with radius {hazard.radius}")
            
            wall = pynlm.WorldObject(1.0, 1.0, pynlm.WorldObjectType.Wall, 0.0, 0.2)
            print(f"Wall at ({wall.x}, {wall.y})")
            
            marker = pynlm.WorldObject(3.0, 3.0, pynlm.WorldObjectType.Marker, 42.0, 0.1)
            print(f"Marker at ({marker.x}, {marker.y}) with ID {marker.value}")
            
            # Access and modify object properties
            resource.active = True
            print(f"Object active: {resource.active}")
        
        Attributes:
            x: X coordinate in world space
            y: Y coordinate in world space
            radius: Physical radius of the object (default 0.5)
            type: Object type (WorldObjectType enum)
            value: Additional value (e.g., resource value, hazard danger level, marker ID)
            active: Whether the object is active/enabled
        
        Methods:
            __init__(): Default constructor
            __init__(x, y, type, value=0.0, radius=0.5): Constructor with parameters
            getX(): Get X coordinate (property x)
            getY(): Get Y coordinate (property y)
            getRadius(): Get radius (property radius)
            getType(): Get type (property type)
            getValue(): Get value (property value)
            getActive(): Get active status (property active)
            setX(x): Set X coordinate (property x)
            setY(y): Set Y coordinate (property y)
            setRadius(radius): Set radius (property radius)
            setType(type): Set type (property type)
            setValue(value): Set value (property value)
            setActive(active): Set active status (property active)
        
        Raises:
            RuntimeError: If coordinates or radius are invalid
        
        Note:
            Radius determines object collision bounds. Type determines object behavior:
            - Empty: Passable, no effect
            - Resource: Collectible, provides value
            - Hazard: Damaging, reduces agent health
            - Wall: Impassable boundary
            - Marker: Landmark for navigation
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, float, WorldObjectType, float, float>(),
             py::arg("x"), py::arg("y"), py::arg("type"), py::arg("value") = 0.0f,
             py::arg("radius") = 0.5f,
             "Initialize world object with parameters. Args: x: X coordinate. y: Y coordinate. type: Object type (WorldObjectType). value: Additional value (default 0). radius: Physical radius (default 0.5).")
        .def_readwrite("x", &WorldObject::x)
        .def_readwrite("y", &WorldObject::y)
        .def_readwrite("radius", &WorldObject::radius)
        .def_readwrite("type", &WorldObject::type)
        .def_readwrite("value", &WorldObject::value)
        .def_readwrite("active", &WorldObject::active);

    py::class_<AgentBody>(m, "AgentBody", R"pbdoc(Agent body state)
        
        The AgentBody class represents the physical state of an agent in the world.
        It tracks position, orientation, motion, energy, health, and age of the agent.
        This is the agent's embodiment in the simulated environment.
        
        Usage example:
            import pynlm
            
            # Create agent body at starting position
            body = pynlm.AgentBody()
            body.x = 5.0
            body.y = 5.0
            body.orientation = 0.0  # radians (0 = facing right)
            
            # Set physical properties
            body.velocityX = 0.5
            body.velocityY = 0.0
            body.angularVelocity = 0.0
            body.energy = 100.0
            body.health = 1.0
            body.age = 0.0
            body.isMoving = True
            body.isTurning = False
            body.lastActionTime = 0.0
            
            # Access body state
            print(f"Position: ({body.x}, {body.y})")
            print(f"Orientation: {body.orientation} rad ({math.degrees(body.orientation)} deg)")
            print(f"Velocity: ({body.velocityX}, {body.velocityY})")
            print(f"Energy: {body.energy}")
            print(f"Health: {body.health}")
            print(f"Age: {body.age}")
            print(f"Moving: {body.isMoving}")
            
            # Reset for new episode
            body.reset()
        
        Attributes:
            x: X coordinate in world space
            y: Y coordinate in world space
            orientation: Orientation angle in radians (0 = facing right, increasing counterclockwise)
            velocityX: Linear velocity in X direction (units/second)
            velocityY: Linear velocity in Y direction (units/second)
            angularVelocity: Angular velocity in radians/second
            energy: Energy level (0-100, 100 = full)
            health: Health status (0.0 = dead, 1.0 = perfect)
            age: Age in simulation steps
            isMoving: Whether agent is currently moving
            isTurning: Whether agent is currently turning
            lastActionTime: Time since last action (simulation steps)
        
        Methods:
            __init__(): Default constructor
            reset(): Reset body to initial state
            getX(): Get X coordinate (property x)
            getY(): Get Y coordinate (property y)
            getOrientation(): Get orientation (property orientation)
            getVelocityX(): Get X velocity (property velocityX)
            getVelocityY(): Get Y velocity (property velocityY)
            getAngularVelocity(): Get angular velocity (property angularVelocity)
            getEnergy(): Get energy (property energy)
            getHealth(): Get health (property health)
            getAge(): Get age (property age)
            getIsMoving(): Get moving status (property isMoving)
            getIsTurning(): Get turning status (property isTurning)
            getLastActionTime(): Get last action time (property lastActionTime)
            setX(x): Set X coordinate (property x)
            setY(y): Set Y coordinate (property y)
            setOrientation(orientation): Set orientation (property orientation)
            setVelocityX(velocityX): Set X velocity (property velocityX)
            setVelocityY(velocityY): Set Y velocity (property velocityY)
            setAngularVelocity(angularVelocity): Set angular velocity (property angularVelocity)
            setEnergy(energy): Set energy (property energy)
            setHealth(health): Set health (property health)
            setAge(age): Set age (property age)
            setIsMoving(isMoving): Set moving status (property isMoving)
            setIsTurning(isTurning): Set turning status (property isTurning)
            setLastActionTime(lastActionTime): Set last action time (property lastActionTime)
        
        Raises:
            RuntimeError: If coordinate or velocity values are invalid
        
        Note:
            Body state updates should be consistent with world physics and
            collision detection. The reset() method restores initial values.
            Coordinates are in world units, velocity in units/second.
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
        .def("reset", &AgentBody::reset,
             "Reset body to initial state. Returns: None. Raises: RuntimeError if reset fails.");

    py::class_<ActionResult>(m, "ActionResult", R"pbdoc(Action result from world)
        
        The ActionResult class represents the result of executing an action in the world.
        It contains the reward received, success status, and any message from the world.
        This is used by the agent to learn from its actions and receive feedback.
        
        Usage example:
            import pynlm
            
            # Create successful action result
            success_result = pynlm.ActionResult(10.0, True, "Collected food item")
            print(f"Reward: {success_result.reward}")
            print(f"Success: {success_result.success}")
            print(f"Message: {success_result.message}")
            
            # Create failure action result
            fail_result = pynlm.ActionResult(-5.0, False, "Hit wall")
            print(f"Reward: {fail_result.reward} (penalty)")
            print(f"Success: {fail_result.success}")
            
            # Default constructor for manual setup
            result = pynlm.ActionResult()
            result.reward = 1.0
            result.success = True
            result.message = "Moved forward successfully"
            
            # Access result attributes
            if result.success:
                print(f"Got reward: {result.reward}")
            else:
                print(f"Penalty: {result.reward}")
        
        Attributes:
            reward: Reward value (positive for good outcomes, negative for penalties)
            success: Boolean indicating if action was successful
            message: Optional descriptive message about action outcome
        
        Methods:
            __init__(): Default constructor
            __init__(reward, success, message=""): Constructor with parameters
            getReward(): Get reward value (property reward)
            getSuccess(): Get success status (property success)
            getMessage(): Get result message (property message)
            setReward(reward): Set reward value (property reward)
            setSuccess(success): Set success status (property success)
            setMessage(message): Set result message (property message)
        
        Raises:
            RuntimeError: If reward value is invalid or parameter types mismatch
        
        Note:
            Reward values should be meaningful to the agent's learning system.
            Positive rewards encourage repetition of successful actions.
            Negative rewards (penalties) discourage failed actions.
            The message provides human-readable explanation of outcomes.
    )pbdoc")
        .def(py::init<>())
        .def(py::init<float, bool, std::string>(),
             py::arg("reward"), py::arg("success"), py::arg("message") = "",
             "Initialize action result with parameters. Args: reward: Reward value. success: Success status. message: Optional message.")
        .def_readwrite("reward", &ActionResult::reward)
        .def_readwrite("success", &ActionResult::success)
        .def_readwrite("message", &ActionResult::message);

    py::class_<SensoryPercept>(m, "SensoryPercept", R"pbdoc(Sensory percept data)
        
        The SensoryPercept class represents processed sensory information
        available to an agent. It contains vision, touch, internal signals,
        proprioception, and audio data captured by the agent's sensory systems.
        This is the primary interface for agents to perceive their environment.
        
        Usage example:
            import pynlm
            
            # Create sensory percept
            percept = pynlm.SensoryPercept()
            
            # Set vision data
            vision = pynlm.Vision(640, 480, 3)
            # ... fill with vision data ...
            percept.setVision(vision)
            print(f"Vision size: {percept.getVisionWidth()}x{percept.getVisionHeight()}")
            
            # Set touch data
            touch = pynlm.Vision(100, 100, 1)  # Single channel for touch
            # ... fill with touch data ...
            percept.setTouch(touch)
            
            # Set internal signals
            internal = pynlm.InternalSignals()
            internal.addSignal(0.5)  # Curiosity level
            internal.addSignal(0.1)  # Novelty level
            percept.setInternal(internal)
            
            # Set proprioception (body position sense)
            proprioception = pynlm.Vision(3, 1, 1)  # x, y, orientation
            # ... fill with proprioception data ...
            percept.setProprioception(proprioception)
            
            # Set audio data
            audio = pynlm.Audio(44100, 4410)  # sampleRate, numSamples
            # ... fill with audio samples ...
            percept.setAudio(audio)
            
            # Get all signals at once
            all_signals = percept.getAllSignals()
            print(f"Total signals available: {len(all_signals)}")
            
            # Set timestamp
            percept.setTimestamp(time.time())
        
        Attributes:
            vision: Vision sensory data
            touch: Touch sensory data  
            internal: Internal signals data
            proprioception: Proprioception (body position) data
            audio: Audio sensory data
            timestamp: Time of percept generation
        
        Methods:
            __init__(): Default constructor
            getVision(): Get vision data
            setVision(vision): Set vision data
            getVisionWidth(): Get vision width
            getVisionHeight(): Get vision height
            getTouch(): Get touch data
            setTouch(touch): Set touch data
            getInternal(): Get internal signals data
            setInternal(internal): Set internal signals data
            getProprioception(): Get proprioception data
            setProprioception(proprioception): Set proprioception data
            getAudio(): Get audio data
            setAudio(audio): Set audio data
            getAllSignals(): Get all sensory signals as list
            getTimestamp(): Get timestamp
            setTimestamp(timestamp): Set timestamp
        
        Raises:
            RuntimeError: If data is invalid or percept is corrupted
        
        Note:
            Vision data typically uses RGB channels (3 channels per pixel).
            Touch data uses single channel intensity.
            Proprioception uses 3 values: [x, y, orientation].
            Internal signals are agent-specific neuromodulatory states.
            Audio data uses normalized samples in range [-1, 1].
    )pbdoc")
        .def(py::init<>())
        .def("getVision", &SensoryPercept::getVision,
             py::return_value_policy::reference_internal,
             "Get vision data. Returns: Vision object.")
        .def("setVision", &SensoryPercept::setVision, py::arg("vision"),
             "Set vision data. Args: vision: Vision sensory input.")
        .def("getVisionWidth", &SensoryPercept::getVisionWidth,
             "Get vision width. Returns: Integer width in pixels.")
        .def("getVisionHeight", &SensoryPercept::getVisionHeight,
             "Get vision height. Returns: Integer height in pixels.")
        .def("getTouch", &SensoryPercept::getTouch,
             py::return_value_policy::reference_internal,
             "Get touch data. Returns: Vision object.")
        .def("setTouch", &SensoryPercept::setTouch, py::arg("touch"),
             "Set touch data. Args: touch: Vision sensory input.")
        .def("getInternal", &SensoryPercept::getInternal,
             py::return_value_policy::reference_internal,
             "Get internal signals data. Returns: InternalSignals object.")
        .def("setInternal", &SensoryPercept::setInternal, py::arg("internal"),
             "Set internal signals data. Args: internal: InternalSignals object.")
        .def("getProprioception", &SensoryPercept::getProprioception,
             py::return_value_policy::reference_internal,
             "Get proprioception data. Returns: Vision object.")
        .def("setProprioception", &SensoryPercept::setProprioception, py::arg("proprioception"),
             "Set proprioception data. Args: proprioception: Vision sensory input.")
        .def("getAudio", &SensoryPercept::getAudio,
             py::return_value_policy::reference_internal,
             "Get audio data. Returns: Audio object.")
        .def("setAudio", &SensoryPercept::setAudio, py::arg("audio"),
             "Set audio data. Args: audio: Audio sensory input.")
        .def("getAllSignals", &SensoryPercept::getAllSignals,
             "Get all sensory signals as list. Returns: List of sensory input objects.")
        .def("getTimestamp", &SensoryPercept::getTimestamp,
             "Get timestamp. Returns: Float time in seconds.")
        .def("setTimestamp", &SensoryPercept::setTimestamp, py::arg("timestamp"),
             "Set timestamp. Args: timestamp: Time value in seconds.");

    py::class_<SimpleWorld>(m, "SimpleWorld", R"pbdoc(Simple 2D world for NLM simulation)
        
        The SimpleWorld class represents a 2D simulated world environment
        for neural agents. It manages world geometry, agent physics, objects,
        and the overall simulation step. The world provides sensory percepts
        and executes agent actions within defined boundaries.
        
        Usage example:
            import pynlm
            
            # Create and configure world
            world = pynlm.createSimpleWorld()
            world.configure(800, 600, 320, 240)  # width, height, vision, vision
            
            # Set up agent starting position
            world.setAgentStart(400, 300)
            
            # Add world objects
            wall = pynlm.WorldObject(100, 100, pynlm.WorldObjectType.Wall, 0.0, 0.2)
            world.addObject(wall)
            
            resource = pynlm.WorldObject(600, 400, pynlm.WorldObjectType.Resource, 10.0, 0.3)
            world.addObject(resource)
            
            # Run simulation steps
            brain = pynlm.createAgentBrain(brain)
            for step in range(1000):
                # Get current agent perception
                percept = world.getSensoryPercept()
                
                # Process percept and get action
                action = brain.processSensoryInput(percept)
                
                # Execute action in world
                world.applyMotorCommand(action, step)
                
                # Get updated world state
                body = world.getAgentBody()
                print(f"Step {step}: Agent at ({body.x:.1f}, {body.y:.1f}), Energy: {body.energy}")
                
                # Update world (apply physics, handle collisions)
                world.update(step)
            
            # Check for valid positions
            for x in range(0, 800, 10):
                for y in range(0, 600, 10):
                    if world.isValidPosition(x, y):
                        # Can place objects here
                        pass
        
        Attributes:
            width: World width in units
            height: World height in units
            maxEnergy: Maximum agent energy capacity
            energyDecayRate: Energy decay per step
            simulationTime: Current simulation time
            randomSeed: Random seed for reproducible simulations
        
        Methods:
            __init__(): Default constructor
            configure(width, height, visionWidth, visionHeight): Configure world dimensions
            reset(): Reset world to initial state
            setAgentStart(x, y): Set agent starting position
            update(timestep): Update world state
            applyMotorCommand(cmd, currentTime): Execute agent action
            getSensoryPercept(): Get agent perception
            getAgentBody(): Get agent body state
            addObject(obj): Add world object
            removeObject(x, y): Remove object at position
            isValidPosition(x, y): Check if position is valid
            getWidth(): Get world width
            getHeight(): Get world height
            getMaxEnergy(): Get maximum energy
            setMaxEnergy(e): Set maximum energy
            getEnergyDecayRate(): Get energy decay rate
            setEnergyDecayRate(r): Set energy decay rate
            getSimulationTime(): Get current simulation time
            setRandomSeed(seed): Set random seed
            getRandomSeed(): Get random seed
        
        Raises:
            RuntimeError: If world is not configured or invalid parameters
        
        Note:
            World coordinates: (0, 0) is bottom-left, (width, height) is top-right.
            Agent starts at configured start position. Objects must be placed
            within valid bounds and not overlapping.
            Energy decays continuously based on energyDecayRate.
            Set randomSeed for reproducible simulations (good for testing).
    )pbdoc")
        .def(py::init<>())
        .def("configure", &SimpleWorld::configure, py::arg("width"), py::arg("height"),
              py::arg("visionWidth"), py::arg("visionHeight"),
              "Configure world dimensions and sensory fields. Args: width: World width in units. height: World height in units. visionWidth: Agent vision width. visionHeight: Agent vision height.")
        .def("reset", &SimpleWorld::reset,
             "Reset world to initial state. Returns: None. Raises: RuntimeError if world not configured.")
        .def("setAgentStart", &SimpleWorld::setAgentStart, py::arg("x"), py::arg("y"),
             "Set agent starting position. Args: x: X coordinate. y: Y coordinate.")
        .def("update", &SimpleWorld::update, py::arg("timestep"),
             "Update world state (physics, collisions, etc.). Args: timestep: Current simulation timestep. Returns: None. Raises: RuntimeError if world not configured.")
        .def("applyMotorCommand", &SimpleWorld::applyMotorCommand,
              py::arg("cmd"), py::arg("currentTime"),
              "Execute agent action in world. Args: cmd: MotorCommand to execute. currentTime: Current simulation time.")
        .def("getSensoryPercept", &SimpleWorld::getSensoryPercept,
              py::return_value_policy::reference_internal,
              "Get agent sensory perception. Returns: SensoryPercept object.")
        .def("getAgentBody", &SimpleWorld::getAgentBody,
              py::return_value_policy::reference_internal,
              "Get agent body state. Returns: AgentBody object.")
        .def("addObject", &SimpleWorld::addObject, py::arg("obj"),
             "Add world object. Args: obj: WorldObject to add. Raises: RuntimeError if object invalid or position occupied.")
        .def("removeObject", &SimpleWorld::removeObject, py::arg("x"), py::arg("y"),
             "Remove object at position. Args: x: X coordinate. y: Y coordinate. Returns: bool indicating success.")
        .def("isValidPosition", &SimpleWorld::isValidPosition, py::arg("x"), py::arg("y"),
             "Check if position is valid for object placement. Args: x: X coordinate. y: Y coordinate. Returns: bool indicating if position is valid.")
        .def("getWidth", &SimpleWorld::getWidth,
             "Get world width. Returns: Integer width in units.")
        .def("getHeight", &SimpleWorld::getHeight,
             "Get world height. Returns: Integer height in units.")
        .def("getMaxEnergy", &SimpleWorld::getMaxEnergy,
             "Get maximum energy. Returns: Float maximum energy.")
        .def("setMaxEnergy", &SimpleWorld::setMaxEnergy, py::arg("e"),
             "Set maximum energy. Args: e: Maximum energy value.")
        .def("getEnergyDecayRate", &SimpleWorld::getEnergyDecayRate,
             "Get energy decay rate. Returns: Float decay rate per timestep.")
        .def("setEnergyDecayRate", &SimpleWorld::setEnergyDecayRate, py::arg("r"),
             "Set energy decay rate. Args: r: Decay rate per timestep.")
        .def("getSimulationTime", &SimpleWorld::getSimulationTime,
             "Get current simulation time. Returns: Float time in seconds.")
        .def("setRandomSeed", &SimpleWorld::setRandomSeed, py::arg("seed"),
             "Set random seed for reproducible simulations. Args: seed: Random seed value.")
        .def("getRandomSeed", &SimpleWorld::getRandomSeed,
             "Get random seed. Returns: Current random seed value.");

    py::class_<Brain>(m, "Brain", R"pbdoc(
        Central neural simulation brain class.
        
        The Brain class represents the core neural computation engine in NLM.
        It simulates spiking neurons, processes sensory input, and generates
        motor commands through complex neural dynamics.
        
        Usage example:
            import pynlm
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            for i in range(100):
                brain.step(i)
        
        Key features:
            - Leaky Integrate-and-Fire (LIF) neurons
            - STDP and Hebbian plasticity
            - Working memory and episodic memory systems
            - Neuromodulation (dopamine, curiosity, novelty)
            - Developmental stages
        
        Attributes:
            neuron_count: Total number of neurons in the brain
            firing_rate: Average firing rate across all neurons
            total_spikes: Total spike count since initialization
        
        Methods:
            step(): Perform one simulation step
            initialize(): Initialize brain with configuration
            reset(): Reset brain to initial state
            receiveSensoryInput(): Process sensory input
            produceAction(): Generate motor action
        
        Raises:
            RuntimeError: If initialization fails or invalid parameters are provided
        
        Note:
            This class is not thread-safe. Do not call multiple methods concurrently.
        )pbdoc")
        .def(py::init<std::shared_ptr<Config>>(), py::arg("config"), 
             "Initialize brain with configuration. Args: config: Brain configuration. Returns: None. Raises: RuntimeError if config is invalid or initialization fails.")
        .def("initialize", &Brain::initialize,
             "Initialize the brain with configuration. Returns: bool indicating success. Raises: RuntimeError if initialization fails.")
        .def("step", static_cast<void (Brain::*)(SimulationStep)>(&Brain::step),
             py::arg("currentStep"),
             "Perform a simulation step. Args: currentStep: Current simulation step (tick count). Returns: None. Raises: RuntimeError if simulation is not initialized.")
        .def("step", static_cast<void (Brain::*)(SimulationStep, Timestamp)>(&Brain::step),
             py::arg("currentStep"), py::arg("currentTime"),
             "Perform a simulation step with timestamp. Args: currentStep: Current simulation step. currentTime: Current simulation time (seconds). Returns: None. Raises: RuntimeError if simulation is not initialized.")
        .def("receiveSensoryInput", &Brain::receiveSensoryInput,
             py::arg("input"),
             "Inject sensory input into the brain. Args: input: SensoryInput object containing environmental input. Returns: None. Raises: RuntimeError if brain is not initialized or input is invalid.")
        .def("injectCurrent", &Brain::injectCurrent,
             py::arg("neuron"), py::arg("current"),
             "Inject current into a specific neuron. Args: neuron: Target neuron ID. current: Current value in pA to inject. Returns: None. Raises: RuntimeError if neuron ID is invalid.")
        .def("injectCurrentToNeurons", &Brain::injectCurrentToNeurons,
             py::arg("type"), py::arg("current"),
             "Inject current into all neurons of a specific type. Args: type: Neuron type (Excitatory, Inhibitory, etc.). current: Current value in pA to inject. Returns: None. Raises: RuntimeError if brain is not initialized.")
        .def("produceAction", &Brain::produceAction,
             "Produce motor action based on neural activity. Returns: Action object containing motor output. Raises: RuntimeError if brain is not initialized or no action generated.")
        .def("reset", &Brain::reset,
             "Reset brain state. Returns: None. Raises: RuntimeError if brain is not initialized.")
        .def("save", &Brain::save, py::arg("filepath"),
             "Save brain state to file. Args: filepath: Path to save file. Returns: bool indicating success. Raises: RuntimeError if save fails.")
        .def("load", &Brain::load, py::arg("filepath"),
             "Load brain state from file. Args: filepath: Path to load file. Returns: bool indicating success. Raises: RuntimeError if load fails.")
        .def("addRegion", &Brain::addRegion, py::arg("name") = "",
             "Add a new neural region. Args: name: Optional region name (defaults to empty). Returns: RegionId for the new region. Raises: RuntimeError if brain is not initialized.")
        .def("getRegion", &Brain::getRegion, py::arg("id"),
             py::return_value_policy::reference_internal,
             "Get a region by ID. Args: id: Region ID to retrieve. Returns: NeuralRegion object. Raises: RuntimeError if region ID not found.")
        .def("getRegionCount", &Brain::getRegionCount,
             "Get the number of regions. Returns: Number of neural regions. Raises: RuntimeError if brain is not initialized.")
        .def("getRegionIds", &Brain::getRegionIds,
             "Get all region IDs. Returns: List of RegionId objects. Raises: RuntimeError if brain is not initialized.")
        .def("getRegions", &Brain::getRegions,
             py::return_value_policy::reference_internal,
             "Get all regions. Returns: List of NeuralRegion objects. Raises: RuntimeError if brain is not initialized.")
        .def("getTotalNeuronCount", &Brain::getTotalNeuronCount,
             "Get total neuron count across all regions. Returns: Total number of neurons. Raises: RuntimeError if brain is not initialized.")
        .def("getTotalSynapseCount", &Brain::getTotalSynapseCount,
             "Get total synapse count across all regions. Returns: Total number of synapses. Raises: RuntimeError if brain is not initialized.")
        .def("getActiveNeuronCount", &Brain::getActiveNeuronCount,
             "Get count of active neurons. Returns: Number of neurons currently active. Raises: RuntimeError if brain is not initialized.")
        .def("getFiringNeuronCount", &Brain::getFiringNeuronCount,
             "Get count of currently firing neurons. Returns: Number of neurons currently firing. Raises: RuntimeError if brain is not initialized.")
        .def("getAverageFiringRate", &Brain::getAverageFiringRate,
             "Get average firing rate across all neurons. Returns: Average firing rate (Hz). Raises: RuntimeError if brain is not initialized.")
        .def("getExcitationInhibitionRatio", &Brain::getExcitationInhibitionRatio,
             "Get excitation/inhibition balance ratio. Returns: Excitation/inhibition ratio. Raises: RuntimeError if brain is not initialized.")
        .def("getTotalSpikeCount", &Brain::getTotalSpikeCount,
             "Get total spike count. Returns: Total number of spikes recorded. Raises: RuntimeError if brain is not initialized.")
        .def("getDevelopmentalStage", &Brain::getDevelopmentalStage,
             "Get current developmental stage. Returns: Current developmental stage (DevelopmentalStage). Raises: RuntimeError if brain is not initialized.")
        .def("setDevelopmentalStage", &Brain::setDevelopmentalStage,
             py::arg("stage"),
             "Set developmental stage. Args: stage: Developmental stage to set. Returns: None. Raises: RuntimeError if brain is not initialized.")
        .def("getConfig", &Brain::getConfig,
             py::return_value_policy::reference_internal,
             "Get the configuration. Returns: Config object. Raises: RuntimeError if brain is not initialized.")
        .def("logStatus", &Brain::logStatus,
             "Log brain status. Returns: None. Raises: RuntimeError if brain is not initialized.")

    py::class_<AgentBrain>(m, "AgentBrain", R"pbdoc(Agent brain interface connecting NLM brain to world)
        
        The AgentBrain class serves as the bridge between the NLM neural brain and the
        simulated world environment. It handles sensory transduction (converting
        perceptual data into neural activity) and motor decoding (converting neural
        activity into motor commands). This class enables agents to interact with
        the world through perception-action loops.
        
        Usage example:
            import pynlm
            
            # Create brain and agent brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            agent_brain = pynlm.createAgentBrain(brain)
            
            # Create world and agent body
            world = pynlm.createSimpleWorld()
            world.configure(200, 200, 40, 40)
            world.setAgentStart(100, 100)
            
            # Initialize agent brain with world
            agent_brain.initialize(world)
            
            # Get agent expectations
            sensory_size = agent_brain.getSensoryInputSize()
            motor_size = agent_brain.getMotorOutputSize()
            print(f"Expected sensory input: {sensory_size} dimensions")
            print(f"Expected motor output: {motor_size} dimensions")
            
            # Main perception-action loop
            for step in range(100):
                # Get current world state
                body = world.getAgentBody()
                percept = world.getSensoryPercept()
                
                # Process sensory input (transduction)
                agent_brain.processSensoryInput(percept)
                
                # Get motor command (decoding)
                motor_cmd = agent_brain.decodeMotorCommand()
                
                # Apply action in world
                world.applyMotorCommand(motor_cmd, step)
                
                # Update world state
                world.update(step)
                
                # Apply reward-based learning
                reward = 0.0  # Calculate from action result
                predicted_reward = agent_brain.getPredictionError()
                agent_brain.applyRewardModulation(reward, predicted_reward)
                
                # Update developmental processes
                agent_brain.updateDevelopment(step)
                
                # Print agent state
                stage = agent_brain.getDevelopmentalStage()
                neuromod = agent_brain.getNeuromodulationLevel()
                curiosity = agent_brain.getCuriosityLevel()
                novelty = agent_brain.getNoveltyLevel()
                
                print(f"Step {step}: Stage={stage}, Neuromod={neuromod:.2f}, "
                      f"Curiosity={curiosity:.2f}, Novelty={novelty:.2f}, "
                      f"Body: ({body.x:.1f}, {body.y:.1f})")
            
            # Reset for new episode
            agent_brain.reset()
        
        Attributes:
            brain: The underlying NLM brain object
            rewardModulationEnabled: Whether reward-based neuromodulation is active
            structuralPlasticityEnabled: Whether structural plasticity is active
            developmentEnabled: Whether developmental processes are active
            curiosityEnabled: Whether curiosity-driven exploration is active
        
        Methods:
            __init__(brain): Constructor with brain object
            initialize(world): Initialize with world environment
            getSensoryInputSize(): Get expected sensory input size
            getMotorOutputSize(): Get expected motor output size
            processSensoryInput(percept): Process sensory input and inject into brain
            decodeMotorCommand(): Decode brain activity into motor command
            applyRewardModulation(reward, predictedReward): Apply reward-based neuromodulation
            updateDevelopment(timestep): Update developmental systems
            getDevelopmentalStage(): Get current developmental stage
            getNeuromodulationLevel(): Get neuromodulation level
            getCuriosityLevel(): Get curiosity level
            getNoveltyLevel(): Get novelty level
            getPredictionError(): Get prediction error signal
            reset(): Reset agent for new episode
            getBrain(): Get underlying brain object
            enableRewardModulation(enable): Enable/disable reward modulation
            enableStructuralPlasticity(enable): Enable/disable structural plasticity
            enableDevelopment(enable): Enable/disable development
            enableCuriosity(enable): Enable/disable curiosity
            isRewardModulationEnabled(): Check if reward modulation is enabled
            isStructuralPlasticityEnabled(): Check if structural plasticity is enabled
            isDevelopmentEnabled(): Check if development is enabled
            isCuriosityEnabled(): Check if curiosity is enabled
        
        Raises:
            RuntimeError: If brain is invalid, world not initialized, or operations fail
        
        Note:
            AgentBrain enables embodied intelligence through perception-action loops.
            It converts raw sensory data into meaningful neural representations
            and translates neural activity into executable actions.
            Neuromodulation systems (dopamine, curiosity, novelty) drive adaptive
            behavior and exploration. Development stages affect learning capabilities.
            The agent is not thread-safe; use sequential method calls.
    )pbdoc")
        .def(py::init<std::shared_ptr<Brain>>(), py::arg("brain"),
             "Initialize agent brain with underlying brain. Args: brain: NLM brain object.")
        .def("initialize", &AgentBrain::initialize, py::arg("world"),
             "Initialize agent brain with world environment. Args: world: SimpleWorld object.")
        .def("getSensoryInputSize", &AgentBrain::getSensoryInputSize,
             "Get expected sensory input size. Returns: Number of sensory dimensions expected.")
        .def("getMotorOutputSize", &AgentBrain::getMotorOutputSize,
             "Get expected motor output size. Returns: Number of motor dimensions expected.")
        .def("processSensoryInput", &AgentBrain::processSensoryInput, py::arg("percept"),
             "Process sensory percept and inject into brain. Args: percept: SensoryPercept object.")
        .def("decodeMotorCommand", &AgentBrain::decodeMotorCommand,
             "Decode brain motor activity into motor command. Returns: MotorCommand object.")
        .def("applyRewardModulation", &AgentBrain::applyRewardModulation,
              py::arg("reward"), py::arg("predictedReward"),
              "Apply reward-based neuromodulation. Args: reward: Actual reward received. predictedReward: Predicted reward.")
        .def("updateDevelopment", &AgentBrain::updateDevelopment, py::arg("timestep"),
              "Update development system. Args: timestep: Simulation timestep.")
        .def("getDevelopmentalStage", &AgentBrain::getDevelopmentalStage,
             "Get current developmental stage. Returns: DevelopmentalStage enum.")
        .def("getNeuromodulationLevel", &AgentBrain::getNeuromodulationLevel,
             "Get neuromodulation level. Returns: Neuromodulation level (0-1).")
        .def("getCuriosityLevel", &AgentBrain::getCuriosityLevel,
             "Get curiosity level. Returns: Curiosity level (0-1).")
        .def("getNoveltyLevel", &AgentBrain::getNoveltyLevel,
             "Get novelty level. Returns: Novelty level (0-1).")
        .def("getPredictionError", &AgentBrain::getPredictionError,
             "Get prediction error. Returns: Prediction error signal (can be negative).")
        .def("reset", &AgentBrain::reset,
             "Reset agent for new episode. Returns: None.")
        .def("getBrain", &AgentBrain::getBrain,
              py::return_value_policy::reference_internal,
              "Get the underlying brain. Returns: Brain object.")
        .def("enableRewardModulation", &AgentBrain::enableRewardModulation, py::arg("enable"),
              "Enable/disable reward modulation. Args: enable: Whether to enable.")
        .def("enableStructuralPlasticity", &AgentBrain::enableStructuralPlasticity, py::arg("enable"),
              "Enable/disable structural plasticity. Args: enable: Whether to enable.")
        .def("enableDevelopment", &AgentBrain::enableDevelopment, py::arg("enable"),
              "Enable/disable development. Args: enable: Whether to enable.")
        .def("enableCuriosity", &AgentBrain::enableCuriosity, py::arg("enable"),
              "Enable/disable curiosity. Args: enable: Whether to enable.")
        .def("isRewardModulationEnabled", &AgentBrain::isRewardModulationEnabled,
              "Check if reward modulation is enabled. Returns: Boolean.")
        .def("isStructuralPlasticityEnabled", &AgentBrain::isStructuralPlasticityEnabled,
              "Check if structural plasticity is enabled. Returns: Boolean.")
        .def("isDevelopmentEnabled", &AgentBrain::isDevelopmentEnabled,
              "Check if development is enabled. Returns: Boolean.")
        .def("isCuriosityEnabled", &AgentBrain::isCuriosityEnabled,
              "Check if curiosity is enabled. Returns: Boolean.");

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
