# NLM API Reference

## Overview

The NLM API Reference provides comprehensive documentation for all public APIs in the NLM framework. This reference covers the core neural network interfaces, Python bindings, and development utilities for creating advanced neural simulations.

## Core NLM APIs

### Brain Class

#### Overview
The Brain class is the central component of the NLM framework, representing a complete neural network simulation system. It provides interfaces for neural computation, sensory processing, and action generation.

#### Class Definition
```cpp
class Brain {
public:
    // Constructors and Destructor
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Copy operations (deleted)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Move operations
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core simulation methods
    bool initialize();
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Input processing
    void receiveSensoryInput(const SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Output generation
    std::unique_ptr<Action> produceAction();
    
    // State management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    void reset();
    
    // Information access
    size_t get_total_neuron_count() const;
    size_t get_total_synapse_count() const;
    size_t get_active_neuron_count() const;
    size_t get_firing_neuron_count() const;
    float get_average_firing_rate() const;
    float get_excitation_inhibition_ratio() const;
    size_t get_total_spike_count() const;
    Timestamp get_simulation_time() const;
    DevelopmentalStage get_developmental_stage() const;
    
    // System components
    SpikeSystem* get_spike_system();
    const SpikeSystem* get_spike_system() const;
    STDP* get_stdp();
    Hebbian* get_hebbian();
    StructuralPlasticity* get_structural_plasticity();
    
    // Development system
    void apply_neuromodulation(const Neuromodulator& signal);
    void update_plasticity();
    void develop();
    
    // Configuration access
    const std::shared_ptr<Config>& get_config() const;
    
    // Development controls
    void enable_development(bool enable);
    void enable_synaptic_plasticity(bool enable);
    void enable_structural_plasticity(bool enable);
    bool is_development_enabled() const;
    bool is_synaptic_plasticity_enabled() const;
    bool is_structural_plasticity_enabled() const;
};
```

#### Detailed Methods

##### Constructor
```cpp
explicit Brain(std::shared_ptr<Config> config);

Parameters:
- `config`: Configuration object defining neural network parameters
  (neuron count, region count, connection probability, etc.)

Throws:
- std::invalid_argument if configuration is invalid
- std::runtime_error if configuration cannot be processed
```

##### initialize()
```cpp
bool initialize();

Initializes the neural network with the configured parameters. This method:
- Allocates memory for neurons and regions
- Creates synaptic connections based on configuration
- Initializes neural dynamics
- Sets up plasticity mechanisms

Returns:
- true if initialization succeeded
- false if initialization failed (check getLastError())

Throws:
- std::runtime_error if initialization fails due to system constraints

Example:
```cpp
auto config = ConfigPresets::create_default_config();
config.set("neuron_count", 1000);
config.set("region_count", 10);

Brain brain(config);
if (!brain.initialize()) {
    std::cerr << "Brain initialization failed: " << brain.getLastError() << std::endl;
    return 1;
}
```

##### step(SimulationStep)
```cpp
void step(SimulationStep currentStep);

Advances the neural network simulation by one time step.

Parameters:
- `currentStep`: Current simulation step number

Process:
1. Update neuron membrane potentials
2. Process synaptic inputs
3. Generate neural outputs and spikes
4. Update synaptic weights (plasticity)
5. Apply neuromodulation signals
6. Update developmental stage if enabled

Note:
- Must be called after initialize() and before reset()
- Thread safety: Not thread-safe. Use external synchronization
```

##### step(SimulationStep, Timestamp)
```cpp
void step(SimulationStep currentStep, Timestamp currentTime);

Advances the neural network simulation with precise timing control.

Parameters:
- `currentStep`: Current simulation step number
- `currentTime`: Current simulation timestamp (seconds)

This method provides temporal precision for time-sensitive simulations.
```

##### receiveSensoryInput()
```cpp
void receiveSensoryInput(const SensoryInput& input);

Injects sensory input into the neural network.

Parameters:
- `input`: Sensory input containing stimulus patterns and timing

The input is converted to neural current and injected into sensory neurons
based on the input configuration.
```

##### injectCurrent()
```cpp
void injectCurrent(NeuronId neuron, MembranePotential current);

Injects current directly into a specific neuron.

Parameters:
- `neuron`: Target neuron ID
- `current`: Current to inject (nA)

Note:
- Neuron ID must be valid (see NeuronId class)
- Current can be positive (excitatory) or negative (inhibitory)
```

##### injectCurrentToNeurons()
```cpp
void injectCurrentToNeurons(NeuronType type, MembranePotential current);

Injects current to all neurons of a specified type.

Parameters:
- `type`: Neuron type (Excitatory, Inhibitory, etc.)
- `current`: Current to inject (nA)

This method is useful for:
- Applying global modulatory signals
- Testing specific neuron populations
- Implementing external stimulation protocols
```

##### produceAction()
```cpp
std::unique_ptr<Action> produceAction();

Generates a motor action based on current neural activity.

Returns:
- std::unique_ptr to Action object
- nullptr if no action could be produced

The action is generated by integrating activity from motor neurons and
applying decision-making algorithms based on the current network state.
```

##### save()
```cpp
bool save(const std::string& filepath) const;

Saves the current brain state to a file for checkpointing.

Parameters:
- `filepath`: Path to the output file

Returns:
- true if save succeeded
- false if save failed

Saved state includes:
- Neuron states (membrane potentials, gates)
- Synaptic weights and delays
- Network topology
- Simulation parameters
- Development state

Example:
```cpp
bool saveBrain(const std::string& filename) {
    Brain brain(config);
    brain.initialize();
    
    // Run simulation...
    for (int step = 0; step < 1000; ++step) {
        brain.step(step);
        
        // Save checkpoint every 100 steps
        if (step % 100 == 0) {
            std::string checkpoint = "checkpoint_" + std::to_string(step) + ".nlm";
            if (!brain.save(checkpoint)) {
                std::cerr << "Failed to save checkpoint: " << checkpoint << std::endl;
            }
        }
    }
    
    // Save final state
    return brain.save("final_state.nlm");
}
```

##### load()
```cpp
bool load(const std::string& filepath);

Loads brain state from a file.

Parameters:
- `filepath`: Path to the input file

Returns:
- true if load succeeded
- false if load failed

The loaded state can be used to:
- Resume interrupted simulations
- Replay recorded behavior
- Initialize from pre-trained networks
```

##### reset()
```cpp
void reset();

Resets the brain to its initial state.

This method:
- Resets all neuron states to initial values
- Resets synaptic weights to initial configuration
- Resets simulation counters
- Resets development state if applicable

Note:
- Does not require re-initialization after reset
- Can be called multiple times
```

##### Statistics Access Methods

All statistics access methods are thread-safe and can be called concurrently:

###### get_total_neuron_count()
```cpp
size_t get_total_neuron_count() const;
Returns the total number of neurons in the network.
```

###### get_total_synapse_count()
```cpp
size_t get_total_synapse_count() const;
Returns the total number of synapses in the network.
```

###### get_active_neuron_count()
```cpp
size_t get_active_neuron_count() const;
Returns the number of neurons currently active (membrane potential > threshold).
```

###### get_firing_neuron_count()
```cpp
size_t get_firing_neuron_count() const;
Returns the number of neurons that have fired in the current time step.
```

###### get_average_firing_rate()
```cpp
float get_average_firing_rate() const;
Returns the average firing rate (Hz) across all neurons.
```

###### get_excitation_inhibition_ratio()
```cpp
float get_excitation_inhibition_ratio() const;
Returns the ratio of excitatory to inhibitory activity.
```

###### get_total_spike_count()
```cpp
size_t get_total_spike_count() const;
Returns the total number of spikes generated since initialization.
```

###### get_simulation_time()
```cpp
Timestamp get_simulation_time() const;
Returns the current simulation time in seconds.
```

###### get_developmental_stage()
```cpp
DevelopmentalStage get_developmental_stage() const;
Returns the current developmental stage of the neural network.
```

### Config Class

#### Overview
The Config class manages neural network configuration parameters, including neuron counts, connection probabilities, and simulation settings.

#### Class Definition
```cpp
class Config {
public:
    // Constructor and Destructor
    Config();
    ~Config();
    
    // Copy and move operations
    Config(const Config&) = default;
    Config& operator=(const Config&) = default;
    Config(Config&&) = default;
    Config& operator=(Config&&) = default;
    
    // Configuration management
    bool set(const std::string& key, const std::any& value);
    std::any get(const std::string& key) const;
    template<typename T> T get(const std::string& key) const;
    template<typename T> T getOr(const std::string& key, T default_value) const;
    
    // Validation and checking
    bool is_valid() const;
    std::vector<std::string> get_valid_keys() const;
    std::vector<std::string> get_invalid_keys() const;
    
    // Configuration presets
    static Config create_default_config();
    static Config create_config_from_file(const std::string& filepath);
    
    // Configuration conversion
    std::string to_string() const;
    bool from_string(const std::string& config_string);
    
    // Configuration utilities
    bool save_to_file(const std::string& filepath) const;
    bool load_from_file(const std::string& filepath);
    
    // Configuration comparison
    bool equals(const Config& other) const;
    
    // Configuration hashing
    size_t hash() const;
};
```

#### Configuration Keys

| Key | Type | Description | Valid Range | Default |
|-----|------|-------------|-------------|---------|
| `neuron_count` | size_t | Total number of neurons | [1, 1000000] | 100 |
| `region_count` | size_t | Number of neural regions | [1, 1000] | 3 |
| `connection_probability` | float | Probability of synapse formation | [0.0, 1.0] | 0.1 |
| `learning_rate` | float | Synaptic plasticity learning rate | [0.0, 1.0] | 0.01 |
| `plasticity_enabled` | bool | Enable synaptic plasticity | - | true |
| `development_enabled` | bool | Enable neural development | - | false |
| `structural_plasticity_enabled` | bool | Enable structural plasticity | - | false |
| `num_threads` | size_t | Number of simulation threads | [1, 64] | 1 |
| `simulation_time_step` | float | Time step duration (seconds) | [1e-6, 1.0] | 0.01 |
| `spike_threshold` | float | Neuron firing threshold | [0.0, 50.0] | 20.0 |
| `membrane_decay` | float | Membrane potential decay rate | [0.0, 1.0] | 0.9 |
| `sensory_neurons` | size_t | Number of sensory neurons | [0, neuron_count] | 10 |
| `motor_neurons` | size_t | Number of motor neurons | [0, neuron_count] | 5 |

#### Configuration Methods

##### set()
```cpp
bool set(const std::string& key, const std::any& value);

Sets a configuration parameter.

Parameters:
- `key`: Configuration parameter name
- `value`: Parameter value

Returns:
- true if setting succeeded
- false if key is invalid or value type is wrong
```

##### get()
```cpp
std::any get(const std::string& key) const;
template<typename T> T get(const std::string& key) const;

Gets a configuration parameter.

Parameters:
- `key`: Configuration parameter name

Returns:
- std::any: Generic value (type depends on parameter)
- T: Typed value (template specialization)

Throws:
- std::out_of_range if key doesn't exist
- std::bad_any_cast if type mismatch
```

##### getOr()
```cpp
template<typename T> T getOr(const std::string& key, T default_value) const;

Gets a configuration parameter with default value.

Parameters:
- `key`: Configuration parameter name
- `default_value`: Value to return if key doesn't exist

Returns:
- T: Parameter value or default if key doesn't exist
```

##### is_valid()
```cpp
bool is_valid() const;

Validates all configuration parameters.

Returns:
- true if all parameters are valid
- false if any parameter is invalid

Checks:
- Parameter types and ranges
- Parameter dependencies
- System resource requirements
```

##### create_default_config()
```cpp
static Config create_default_config();

Creates a configuration with default values.

Returns:
- Config object with default settings

Default values are optimized for general-purpose neural network simulation.
```

### AgentBrain Class

#### Overview
The AgentBrain class extends the Brain class to provide agent-specific functionality, including sensory processing, motor action generation, and behavioral control.

#### Class Definition
```cpp
class AgentBrain {
public:
    // Constructor and Destructor
    explicit AgentBrain(std::shared_ptr<Brain> brain);
    ~AgentBrain();
    
    // Copy operations (deleted)
    AgentBrain(const AgentBrain&) = delete;
    AgentBrain& operator=(const AgentBrain&) = delete;
    
    // Move operations
    AgentBrain(AgentBrain&&) noexcept;
    AgentBrain& operator=(AgentBrain&&) noexcept;
    
    // Core agent methods
    bool initialize(std::shared_ptr<SimpleWorld> world);
    void processSensoryInput(const SensoryInput& input);
    std::unique_ptr<Action> decodeMotorCommand();
    void updateWorld(double time_step);
    
    // Neuromodulation
    void enable_reward_modulation(bool enable);
    void enable_curiosity(bool enable);
    void enable_novelty(bool enable);
    
    // State access
    double get_neuromodulation_level() const;
    double get_curiosity_level() const;
    double get_novelty_level() const;
    float get_reward_signal() const;
    
    // World interaction
    void set_world(std::shared_ptr<SimpleWorld> world);
    std::shared_ptr<SimpleWorld> get_world() const;
    
    // Statistics
    size_t get_steps_processed() const;
    double get_total_reward() const;
    size_t get_action_count(ActionType type) const;
};
```

#### Agent-Specific Features

##### initialize()
```cpp
bool initialize(std::shared_ptr<SimpleWorld> world);

Initializes the agent with a world environment.

Parameters:
- `world`: Pointer to the environment world

Returns:
- true if initialization succeeded
- false if initialization failed

Initializes agent-specific systems:
- Sensory processing systems
- Motor control systems
- Neuromodulation circuits
- World interaction protocols
```

##### processSensoryInput()
```cpp
void processSensoryInput(const SensoryInput& input);

Processes sensory input from the environment.

Parameters:
- `input`: Sensory input containing environmental data

The method:
1. Preprocesses sensory data
2. Encodes input for neural processing
3. Injects processed input into brain sensory neurons
4. Updates internal sensory representations
```

##### decodeMotorCommand()
```cpp
std::unique_ptr<Action> decodeMotorCommand();

Generates a motor command based on current brain state.

Returns:
- std::unique_ptr to Action object
- nullptr if no action can be generated

The action is generated by:
1. Integrating motor neuron activity
2. Applying decision thresholds
3. Selecting appropriate action type
4. Setting action parameters
```

##### updateWorld()
```cpp
void updateWorld(double time_step);

Updates the world environment based on agent actions.

Parameters:
- `time_step`: Time step duration for world update

The method:
1. Applies agent's motor actions to the world
2. Updates environmental state
3. Generates new sensory input
4. Updates world statistics
```

### SimpleWorld Class

#### Overview
The SimpleWorld class represents a simple, grid-based environment for neural agent simulation.

#### Class Definition
```cpp
class SimpleWorld {
public:
    // Constructor and Destructor
    SimpleWorld();
    ~SimpleWorld();
    
    // Configuration
    bool configure(size_t width, size_t height, 
                  size_t vision_width = 8, size_t vision_height = 8);
    
    // State management
    void reset();
    void update(double time_step);
    
    // Input/output processing
    SensoryInput getSensoryPercept() const;
    void applyMotorCommand(const Action& action, double time_step);
    
    // Information access
    size_t get_width() const;
    size_t get_height() const;
    size_t get_vision_width() const;
    size_t get_vision_height() const;
    double get_simulation_time() const;
    
    // Statistics
    size_t get_active_agent_count() const;
    size_t get_total_action_count() const;
    double get_average_reward() const;
    
    // Environment objects
    void add_world_object(const WorldObject& object);
    void remove_world_object(const WorldObjectId& id);
    const WorldObject* get_world_object(const WorldObjectId& id) const;
    std::vector<WorldObjectId> get_visible_objects(const Position& observer) const;
    
    // World statistics
    double get_terrain_complexity() const;
    size_t get_resource_density() const;
    bool has_weather_effect() const;
};
```

#### World Configuration

##### configure()
```cpp
bool configure(size_t width, size_t height,
              size_t vision_width = 8, size_t vision_height = 8);

Configures the world environment.

Parameters:
- `width`: World width in cells
- `height`: World height in cells
- `vision_width`: Sensory range width
- `vision_height`: Sensory range height

Returns:
- true if configuration succeeded
- false if configuration failed

Configures:
- Grid-based world topology
- Sensor ranges for perception
- Initial world state
- Resource distribution
```

### Type Definitions

#### Fundamental Types

| Type | Description | Typical Values |
|------|-------------|----------------|
| `NeuronId` | Unique identifier for neurons | [0, 2^64-1] |
| `RegionId` | Unique identifier for regions | [0, 2^32-1] |
| `SynapseId` | Unique identifier for synapses | [0, 2^64-1] |
| `PopulationId` | Unique identifier for neuron populations | [0, 2^16-1] |
| `SimulationStep` | Simulation step counter | [0, 2^32-1] |
| `Timestamp` | Time value in seconds | [0.0, ∞) |
| `MembranePotential` | Neuron membrane potential | [-100.0, 50.0] |
| `ActionType` | Motor action type | Discrete enum values |
| `WorldObjectType` | Environment object type | Discrete enum values |

#### NeuronType Enum

```cpp
enum class NeuronType {
    Excitatory,
    Inhibitory,
    Sensory,
    Motor,
    Modulatory
};
```

#### DevelopmentalStage Enum

```cpp
enum class DevelopmentalStage {
    Initial,
    Growth,
    Maturation,
    Stabilization,
    Degradation
};
```

### Helper Functions

#### Python Binding Utilities

```cpp
// Python binding helper functions
namespace pybindings {
    
    // Type conversion utilities
    py::array_t<float> to_numpy_array(const std::vector<float>& data);
    std::vector<float> to_vector(const py::array_t<float>& array);
    
    // Configuration utilities
    py::dict config_to_dict(const Config& config);
    Config config_from_dict(const py::dict& config_dict);
    
    // Brain wrapper utilities
    py::dict brain_to_dict(const Brain& brain);
    Brain brain_from_dict(const py::dict& brain_dict);
    
    // Recording utilities
    bool record_state(const Brain& brain, const std::string& filepath);
    bool playback_state(Brain& brain, const std::string& filepath);
    
    // Visualization utilities
    py::dict create_visualization_data(const Brain& brain);
    
    // Analysis utilities
    py::dict analyze_network(const Brain& brain);
    
    // Error handling utilities
    std::string get_last_error();
    void clear_last_error();
    
    // Performance monitoring utilities
    double get_simulation_throughput();
    double get_memory_usage();
    
} // namespace pybindings
```

## Python API Reference

### Module Overview

The NLM Python bindings provide a comprehensive interface to the NLM C++ neural simulation framework. The bindings are organized into several key modules:

#### Core Modules

- `pynlm` - Main module with all NLM functionality
- `pynlm.brain` - Brain-specific operations
- `pynlm.config` - Configuration management
- `pynlm.agent` - Agent system interface
- `pynlm.world` - World simulation interface

#### Helper Modules

- `pynlm.utils` - Utility functions
- `pynlm.exceptions` - Custom exceptions
- `pynlm.types` - Type definitions and enums

### Main pynlm Module

#### Brain Class

```python
class pynlm.Brain:
    """
    Neural network simulation engine.
    
    The Brain class represents a complete neural network simulation with
    multiple regions, neurons, and synaptic connections. It provides
    interfaces for neural computation, sensory processing, and action
    generation.
    
    Attributes:
        config (dict): Brain configuration parameters
        neuron_count (int): Total number of neurons
        region_count (int): Number of neural regions
        total_spike_count (int): Total number of spikes generated
    """
    
    def __init__(self, config=None):
        """
        Initialize a neural network brain.
        
        Args:
            config (dict, optional): Configuration parameters.
                If None, default configuration is used.
                
        Raises:
            ValueError: If config is invalid
            RuntimeError: If brain initialization fails
        """
        pass
    
    def initialize(self):
        """
        Initialize the neural network.
        
        This method sets up the neural network structure, allocates
        memory for neurons and regions, and initializes synaptic
        connections.
        
        Returns:
            bool: True if initialization succeeded, False otherwise
            
        Raises:
            RuntimeError: If initialization fails
        """
        pass
    
    def step(self, step):
        """
        Advance the neural network simulation by one time step.
        
        Args:
            step (int): Current simulation step number
            
        Raises:
            RuntimeError: If brain is not initialized
            ValueError: If step is invalid
        """
        pass
    
    def get_total_neuron_count(self):
        """
        Get the total number of neurons in the network.
        
        Returns:
            int: Total number of neurons
        """
        pass
    
    def get_total_synapse_count(self):
        """
        Get the total number of synapses in the network.
        
        Returns:
            int: Total number of synapses
        """
        pass
    
    def get_active_neuron_count(self):
        """
        Get the number of neurons currently active.
        
        Returns:
            int: Number of active neurons
        """
        pass
    
    def get_firing_neuron_count(self):
        """
        Get the number of neurons that have fired in the current step.
        
        Returns:
            int: Number of firing neurons
        """
        pass
    
    def get_average_firing_rate(self):
        """
        Get the average firing rate across all neurons.
        
        Returns:
            float: Average firing rate in Hz
        """
        pass
    
    def get_total_spike_count(self):
        """
        Get the total number of spikes generated since initialization.
        
        Returns:
            int: Total spike count
        """
        pass
    
    def get_simulation_time(self):
        """
        Get the current simulation time in seconds.
        
        Returns:
            float: Current simulation time
        """
        pass
    
    def get_developmental_stage(self):
        """
        Get the current developmental stage.
        
        Returns:
            DevelopmentalStage: Current developmental stage
        """
        pass
    
    def save(self, filepath):
        """
        Save the current brain state to a file.
        
        Args:
            filepath (str): Path to the output file
            
        Returns:
            bool: True if save succeeded, False otherwise
        """
        pass
    
    def load(self, filepath):
        """
        Load brain state from a file.
n        
        Args:
            filepath (str): Path to the input file
            
        Returns:
            bool: True if load succeeded, False otherwise
        """
        pass
    
    def reset(self):
        """
        Reset the brain to its initial state.
        
        This method resets all neuron states, synaptic weights,
        and simulation counters to their initial values.
        """
        pass
```

#### Config Class

```python
class pynlm.Config:
    """
    Neural network configuration manager.
    
    The Config class manages neural network configuration parameters,
    including neuron counts, connection probabilities, and simulation
    settings. It provides validation and conversion utilities.
    
    Attributes:
        valid_keys (list): List of valid configuration keys
        invalid_keys (list): List of invalid configuration keys
    """
    
    def __init__(self):
        """Initialize an empty configuration."""
        pass
    
    def set(self, key, value):
        """
        Set a configuration parameter.
        
        Args:
            key (str): Configuration parameter name
            value: Parameter value
            
        Returns:
            bool: True if setting succeeded, False otherwise
            
        Raises:
            ValueError: If key is invalid or value type is wrong
        """
        pass
    
    def get(self, key, default_value=None):
        """
        Get a configuration parameter.
        
        Args:
            key (str): Configuration parameter name
            default_value: Default value if key doesn't exist
            
        Returns:
            Parameter value or default if key doesn't exist
            
        Raises:
            KeyError: If key doesn't exist and no default is provided
        """
        pass
    
    def get_keys(self):
        """
        Get all valid configuration keys.
        
        Returns:
            list: List of valid configuration keys
        """
        pass
    
    def is_valid(self):
        """
        Validate configuration parameters.
        
        Returns:
            bool: True if all parameters are valid, False otherwise
        """
        pass
    
    @classmethod
    def create_default_config(cls):
        """
        Create a configuration with default values.
        
        Returns:
            Config: Configuration object with default settings
        """
        pass
    
    @classmethod
    def create_config_from_dict(cls, config_dict):
        """
        Create configuration from dictionary.
        
        Args:
            config_dict (dict): Configuration dictionary
            
        Returns:
            Config: Configuration object
        """
        pass
```

#### AgentBrain Class

```python
class pynlm.AgentBrain:
    """
    Neural agent with sensory and motor capabilities.
    
    The AgentBrain class extends the Brain class to provide agent-specific
    functionality, including sensory processing, motor action generation,
    and behavioral control in an environment.
    
    Attributes:
        world (SimpleWorld): Environment world
        neuromodulation_level (float): Neuromodulation level
        curiosity_level (float): Curiosity level
        novelty_level (float): Novelty level
    """
    
    def __init__(self, brain=None):
        """
        Initialize an agent brain.
        
        Args:
            brain (Brain, optional): Brain instance. If None, creates new brain.
        """
        pass
    
    def initialize(self, world):
        """
        Initialize the agent with a world environment.
        
        Args:
            world (SimpleWorld): Environment world
            
        Returns:
            bool: True if initialization succeeded, False otherwise
        """
        pass
    
    def processSensoryInput(self, percept):
        """
        Process sensory input from the environment.
        
        Args:
            percept (SensoryInput): Sensory input data
        """
        pass
    
    def decodeMotorCommand(self):
        """
        Generate a motor command based on current brain state.
        
        Returns:
            Action: Motor action object
        """
        pass
    
    def updateWorld(self, time_step):
        """
        Update the world environment based on agent actions.
        
        Args:
            time_step (float): Time step duration
        """
        pass
    
    def enable_reward_modulation(self, enable):
        """
        Enable or disable reward modulation.
        
        Args:
            enable (bool): True to enable, False to disable
        """
        pass
    
    def enable_curiosity(self, enable):
        """
        Enable or disable curiosity system.
        
        Args:
            enable (bool): True to enable, False to disable
        """
        pass
    
    def enable_novelty(self, enable):
        """
        Enable or disable novelty detection.
        
        Args:
            enable (bool): True to enable, False to disable
        """
        pass
    
    def get_neuromodulation_level(self):
        """
        Get current neuromodulation level.
        
        Returns:
            float: Neuromodulation level
        """
        pass
    
    def get_curiosity_level(self):
        """
        Get current curiosity level.
        
        Returns:
            float: Curiosity level
        """
        pass
    
    def get_novelty_level(self):
        """
        Get current novelty level.
        
        Returns:
            float: Novelty level
        """
        pass
    
    def get_reward_signal(self):
        """
        Get current reward signal.
        
        Returns:
            float: Reward signal
        """
        pass
```

#### SimpleWorld Class

```python
class pynlm.SimpleWorld:
    """
    Simple grid-based environment for neural agent simulation.
    
    The SimpleWorld class represents a simple, grid-based environment
    where neural agents can interact with objects and learn behaviors.
    
    Attributes:
        width (int): World width in cells
        height (int): World height in cells
        vision_width (int): Sensory range width
        vision_height (int): Sensory range height
    """
    
    def __init__(self):
        """Initialize an empty world."""
        pass
    
    def configure(self, width, height, vision_width=8, vision_height=8):
        """
        Configure the world environment.
        
        Args:
            width (int): World width in cells
            height (int): World height in cells
            vision_width (int): Sensory range width
            vision_height (int): Sensory range height
            
        Returns:
            bool: True if configuration succeeded, False otherwise
        """
        pass
    
    def reset(self):
        """
        Reset the world to its initial state.
        """
        pass
    
    def update(self, time_step):
        """
        Update the world environment.
        
        Args:
            time_step (float): Time step duration
        """
        pass
    
    def getSensoryPercept(self):
        """
        Get sensory percept from the environment.
        
        Returns:
            SensoryInput: Sensory input data
        """
        pass
    
    def applyMotorCommand(self, action, time_step):
        """
        Apply motor command to the world.
        
        Args:
            action (Action): Motor action to apply
            time_step (float): Time step duration
        """
        pass
    
    def get_width(self):
        """
        Get world width.
        
        Returns:
            int: World width
        """
        pass
    
    def get_height(self):
        """
        Get world height.
        
        Returns:
            int: World height
        """
        pass
    
    def get_simulation_time(self):
        """
        Get current simulation time.
        
        Returns:
            float: Current simulation time
        """
        pass
```

### Type Definitions

#### Enum Types

```python
# Neuron types
class NeuronType(Enum):
    EXCITATORY = 1
    INHIBITORY = 2
    SENSORY = 3
    MOTOR = 4
    MODULATORY = 5

# Developmental stages
class DevelopmentalStage(Enum):
    INITIAL = 1
    GROWTH = 2
    MATURATION = 3
    STABILIZATION = 4
    DEGRADATION = 5

# Action types
class ActionType(Enum):
    MOVE_FORWARD = 1
    MOVE_BACKWARD = 2
    TURN_LEFT = 3
    TURN_RIGHT = 4
    PICKUP = 5
    DROP = 6
    USE = 7
    ATTACK = 8
    DEFEND = 9

# World object types
class WorldObjectType(Enum):
    AGENT = 1
    RESOURCE = 2
    TERRAIN = 3
    OBSTACLE = 4
    PREDATOR = 5
    PREY = 6
```

#### Exception Classes

```python
# Base exception for all NLM errors
class NLMError(Exception):
    """Base exception for NLM errors."""
    pass

# Configuration validation errors
class ValidationError(NLMError):
    """Exception for configuration validation errors."""
    pass

# Simulation runtime errors
class SimulationError(NLMError):
    """Exception for simulation runtime errors."""
    pass

# Python interface errors
class PythonError(NLMError):
    """Exception for Python interface errors."""
    pass
```

### Helper Functions

#### Type Conversion Utilities

```python
# Convert Python list to numpy array
pynlm.to_numpy_array(data: List[float]) -> np.ndarray:

# Convert numpy array to Python list
pynlm.to_vector(array: np.ndarray) -> List[float]:

# Convert neuron type to string
pynlm.neuron_type_to_string(neuron_type: NeuronType) -> str:

# Convert string to neuron type
pynlm.string_to_neuron_type(string: str) -> NeuronType:

# Convert neuron IDs to indices
pynlm.neuron_ids_to_indices(neuron_ids: List[NeuronId]) -> List[int]:

# Convert indices to neuron IDs
pynlm.indices_to_neuron_ids(indices: List[int]) -> List[NeuronId]:
```

#### Configuration Utilities

```python
# Create configuration from dictionary
pynlm.create_config_from_dict(config_dict: Dict[str, Any]) -> Config:

# Get default configuration dictionary
pynlm.get_default_config_dict() -> Dict[str, Any]:

# Create default configuration
pynlm.createDefaultConfig() -> Config:
```

#### Brain Operations

```python
# Create brain with configuration
pynlm.createBrain(config: Config = None) -> Brain:

# Create brain from dictionary
pynlm.createBrainFromDict(config_dict: Dict[str, Any]) -> Brain:

# Create simple world
pynlm.createSimpleWorld() -> SimpleWorld:

# Create agent brain
pynlm.createAgentBrain(brain: Brain) -> AgentBrain:

# Batch current injection
pynlm.batch_inject_currents(brain: Brain, neuron_currents: Dict[NeuronId, float]) -> None:

# Create visualization data
pynlm.create_visualization_data(brain: Brain) -> Dict[str, Any]:

# Record simulation state
pynlm.record_state(brain: Brain, filepath: str) -> bool:

# Playback simulation state
pynlm.playback_state(brain: Brain, filepath: str) -> bool:

# Analyze network
pynlm.analyze_network(brain: Brain) -> Dict[str, Any]:
```

#### Recording and Playback

```python
# Enable recording
brain.enable_recording(filepath: str) -> None:

# Disable recording
brain.disable_recording() -> None:

# Playback state
brain.playback_state(filepath: str) -> bool:

# Record state
brain.record_state(filepath: str) -> bool:
```

#### Visualization

```python
# Enable visualization
brain.enable_visualization() -> None:

# Disable visualization
brain.disable_visualization() -> None:

# Create visualization data
brain.create_visualization_data() -> Dict[str, Any]:
```

#### Error Handling

```python
# Get last error message
pynlm.get_last_error() -> str:

# Clear last error
pynlm.clear_last_error() -> None:

# Get version information
pynlm.get_version() -> str:
```

### Performance Optimization

#### Batch Operations

```python
# Batch current injection for better performance
brain.inject_current_to_type(neuron_type: NeuronType, current: float) -> None:

# Get membrane potentials as numpy array for efficient numerical operations
brain.get_numpy_potentials() -> np.ndarray:

# Convert data types for efficient processing
brain.convert_to_numpy(data: List[float]) -> np.ndarray:
```

#### Context Managers

```python
# Context manager for automatic initialization/reset
with brain.create_context_manager(auto_reset=True) as ctx:
    # Brain is automatically initialized/reset
    brain.step(0)
    action = brain.produce_action()
```

#### Memory Management

```python
# Efficient memory usage with context managers
with brain.create_context_manager(auto_reset=True) as ctx:
    # Perform simulation operations
    brain.step(step)
    action = brain.produce_action()
```

### Example Usage

#### Basic Usage

```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain()
brain.initialize()

# Run simulation steps
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

#### Advanced Features

```python
import pynlm

# Create brain with custom configuration
config = pynlm.create_default_config_dict()
config["neuron_count"] = 500
config["region_count"] = 10

brain = pynlm.createBrain(config)
brain.initialize()

# Get network statistics
stats = brain.get_stats()
print(f"Network stats: {stats}")

# Use context manager
with brain.create_context_manager(auto_reset=True):
    for step in range(100):
        brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Record simulation
brain.enable_recording("simulation.json")
brain.step(0)
brain.disable_recording()

# Playback recorded simulation
brain2 = pynlm.createBrain()
brain2.playback_state("simulation.json")
```

#### Multi-Agent System

```python
import pynlm
import threading

class AgentRunner:
    def __init__(self, agent_id, config):
        self.agent_id = agent_id
        self.brain = pynlm.createBrain(config)
        self.brain.initialize()
        self.world = pynlm.createSimpleWorld()
        self.world.configure(10, 10, 8, 8)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
    
    def run(self, steps):
        for step in range(steps):
            self.world.update(0.1)
            self.agent.processSensoryInput(self.world.getSensoryPercept())
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())

# Create and run multiple agents
agents = []
for i in range(3):
    config = pynlm.create_default_config_dict()
    config["neuron_count"] = 100
    agents.append(AgentRunner(i, config))

# Run all agents in parallel
threads = []
for agent in agents:
    thread = threading.Thread(target=agent.run, args=(50,))
    threads.append(thread)
    thread.start()

for thread in threads:
    thread.join()
```

## API Summary

### Core Classes

| Class | Description | Key Methods |
|-------|-------------|-------------|
| Brain | Neural network simulation engine | initialize(), step(), produce_action() |
| Config | Configuration management | set(), get(), is_valid() |
| AgentBrain | Agent with sensory/motor capabilities | processSensoryInput(), decodeMotorCommand() |
| SimpleWorld | Grid-based environment | configure(), update(), getSensoryPercept() |

### Helper Functions

| Function | Description | Parameters |
|----------|-------------|------------|
| to_numpy_array | Convert Python list to numpy array | data: List[float] |
| to_vector | Convert numpy array to Python list | array: np.ndarray |
| createBrain | Create neural network brain | config: Config |
| createSimpleWorld | Create simple environment | None |
| createAgentBrain | Create agent with brain | brain: Brain |
| batch_inject_currents | Batch current injection | brain: Brain, neuron_currents: Dict |

### Exception Classes

| Exception | Description | When Raised |
|-----------|-------------|-------------|
| NLMError | Base exception for all NLM errors | Any NLM error |
| ValidationError | Configuration validation errors | Invalid configuration |
| SimulationError | Simulation runtime errors | Simulation problems |
| PythonError | Python interface errors | Interface issues |

## Version Information

```python
# Get current version
version = pynlm.get_version()

# Version information constants
VERSION = pynlm.VERSION
DEFAULT_NEURON_COUNT = pynlm.DEFAULT_NEURON_COUNT
DEFAULT_REGION_COUNT = pynlm.DEFAULT_REGION_COUNT
DEFAULT_CONNECTION_PROBABILITY = pynlm.DEFAULT_CONNECTION_PROBABILITY
INVALID_NEURON_ID = pynlm.INVALID_NEURON_ID
```

## Installation and Setup

### Requirements

- Python 3.8 or higher
- C++20 compatible compiler
- CMake 3.15 or higher
- pip (Python package manager)

### Installation

```bash
# Install via pip (includes C++ extensions)
pip install nlm

# Install from source
pip install -e .

# Build from source directory
mkdir build
cd build
cmake ..
cmake --build .
```

### Testing

```bash
# Run unit tests
python -m pytest python/test_bindings.py

# Run example usage
python python/example_usage.py

# Run integration tests
./tests/run_tests.sh
```

### Documentation

```bash
# Generate API documentation
python -m pydocstyle python/
python -m mkdocs build --strict
```

## Migration Guide

### From Version 1.x to Version 2.x

The enhanced Python bindings maintain full backward compatibility:

1. **All existing function calls work unchanged**
2. **New features are available through enhanced methods**
3. **Better error messages and type safety**
4. **Additional helper functions for common tasks**

### Example Migration

```python
# Version 1.x code (still works)
import pynlm

brain = pynlm.createBrain()
brain.initialize()
for step in range(100):
    brain.step(step)
print(f"Total spikes: {brain.getTotalSpikeCount()}")

# Version 2.x code (with new features)
import pynlm

brain = pynlm.createBrain()
brain.initialize()

# Use enhanced features
stats = brain.get_stats()
print(f"Network stats: {stats}")

with brain.create_context_manager(auto_reset=True):
    for step in range(100):
        brain.step(step)

print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

## Future Enhancements

### Planned Improvements

1. **Advanced Visualization**
   - Integration with matplotlib/plotly
   - 3D visualization of neural networks
   - Interactive visualization tools

2. **Distributed Computing**
   - Multi-agent system support
   - Parallel processing capabilities
   - Distributed simulation support

3. **Machine Learning Integration**
   - Neural network training
   - Transfer learning capabilities
   - Hybrid system support

4. **Web Interface**
   - REST API for remote simulation
   - Web-based visualization
   - Real-time monitoring

5. **Advanced Plasticity Rules**
   - Custom plasticity rule definitions
   - Meta-learning capabilities
   - Adaptive learning algorithms

## Conclusion

The NLM Python bindings provide a comprehensive, user-friendly interface to the NLM neural simulation framework. With backward compatibility maintained, new features, improved error handling, and extensive documentation, the bindings are suitable for both beginners and advanced users.

Key benefits:

- **Pythonic APIs** with property access and context managers
- **Comprehensive serialization** for state management
- **Powerful analysis tools** for neural network study
- **Robust error handling** with clear messages
- **Extensive documentation** with examples
- **Performance optimizations** for large-scale simulations

The enhanced bindings make it easier than ever to create, simulate, and analyze neural networks using the NLM framework.

For more information, visit the NLM documentation at https://nlm.readthedocs.io.
