# API Reference Documentation

This document provides comprehensive API reference for the NLM Python bindings.
It covers all classes, methods, and functions available in the `pynlm` module.

## Module Structure

The NLM Python bindings are organized into the following modules:

- `pynlm`: Main module with exports and factory functions
- `pynlm.brain`: Brain class and simulation management
- `pynlm.neurons`: Neuron dynamics and LIF model
- `pynlm.memory`: Memory systems (working, episodic, associative)
- `pynlm.plasticity`: Plasticity rules (STDP, Hebbian, structural)
- `pynlm.neuromodulation`: Neuromodulation systems
- `pynlm.cognition`: Cognitive systems (planning, concept formation, attention)
- `pynlm.prediction`: Prediction system and forward models
- `pynlm.development`: Development system and critical periods
- `pynlm.config`: Configuration management
- `pynlm.experiment`: Experiment management
- `pynlm.visualizer`: Visualization utilities

## Class Documentation

### Brain

The central neural simulation brain with manager-based architecture.

#### Constructor
```python
def __init__(self, config: Config = None)
```

**Parameters:**
- `config (Config, optional)`: Configuration object. Creates default if not provided.

#### Methods

##### initialize
```python
def initialize(self, config_overrides: Dict[str, Any] = None) -> bool
```

Initialize brain with simulation parameters.

**Parameters:**
- `config_overrides (Dict[str, Any], optional)`: Configuration parameter overrides.

**Returns:**
- `bool`: True if initialization successful.

##### step
```python
def step(self, step: int, dt: float = 0.001)
```

Perform one simulation step.

**Parameters:**
- `step (int)`: Simulation step number
- `dt (float)`: Time step duration in seconds

##### get_stats
```python
def get_stats(self) -> Dict[str, Any]
```

Get current simulation statistics.

**Returns:**
- `Dict[str, Any]`: Dictionary of statistics.

##### reset
```python
def reset(self)
```

Reset brain state to initial configuration.

##### save
```python
def save(self, filepath: str)
```

Save brain state to file.

**Parameters:**
- `filepath (str)`: Path to save file

##### load
```python
def load(self, filepath: str)
```

Load brain state from file.

**Parameters:**
- `filepath (str)`: Path to load file

##### Property Accessors

###### config
```python
@property
def config(self) -> Config
```

Get brain configuration.

###### memory
```python
@property
def memory(self) -> MemorySystem
```

Get memory system.

###### plasticity
```python
@property
def plasticity(self) -> PlasticityRule
```

Get plasticity system.

###### neuromodulation
```python
@property
def neuromodulation(self) -> Neuromodulator
```

Get neuromodulation system.

###### cognition
```python
@property
def cognition(self) -> CognitiveSystem
```

Get cognitive system.

###### prediction
```python
@property
def prediction(self) -> PredictionSystem
```

Get prediction system.

###### development
```python
@property
def development(self) -> DevelopmentSystem
```

Get development system.

### Neuron

Leaky Integrate-and-Fire (LIF) neuron model.

#### Constructor
```python
def __init__(self, neuron_id: int, neuron_type: str = "Internal")
```

Initialize neuron.

**Parameters:**
- `neuron_id (int)`: Unique neuron identifier
- `neuron_type (str)`: Neuron type

#### Methods

##### set_parameters
```python
def set_parameters(self, params: Dict[str, Any])
```

Set neuron parameters.

**Parameters:**
- `params (Dict[str, Any])`: Dictionary of neuron parameters

##### update_parameters
```python
def update_parameters(self, dt: float, current_time: float)
```

Update neuron state for one time step.

**Parameters:**
- `dt (float)`: Time step duration
- `current_time (float)`: Current simulation time

##### receive_excitatory_input
```python
def receive_excitatory_input(self, amplitude: float)
```

Receive excitatory synaptic input.

**Parameters:**
- `amplitude (float)`: Input amplitude

##### receive_inhibitory_input
```python
def receive_inhibitory_input(self, amplitude: float)
```

Receive inhibitory synaptic input.

**Parameters:**
- `amplitude (float)`: Input amplitude

##### inject_current
```python
def inject_current(self, current: float)
```

Inject external current into neuron.

**Parameters:**
- `current (float)`: Current amplitude

##### enable_plasticity
```python
def enable_plasticity(self, hebbian: bool = True, stdp: bool = True, 
                      reward_modulated: bool = False)
```

Enable plasticity mechanisms.

**Parameters:**
- `hebbian (bool)`: Enable Hebbian plasticity
- `stdp (bool)`: Enable STDP
- `reward_modulated (bool)`: Enable reward-modulated plasticity

##### reset
```python
def reset(self)
```

Reset neuron to initial state.

### MemorySystem

Complete memory system integrating all memory subsystems.

#### Methods

##### initialize
```python
def initialize(self)
```

Initialize memory system.

##### update
```python
def update(self, dt: float) -> Dict[str, Any]
```

Update memory system.

**Parameters:**
- `dt (float)`: Time step duration

**Returns:**
- `Dict[str, Any]`: Memory statistics

##### store_working_memory
```python
def store_working_memory(self, neuron_id: int, activation: float, 
                        timestamp: float)
```

Store activation in working memory.

**Parameters:**
- `neuron_id (int)`: Neuron identifier
- `activation (float)`: Activation level
- `timestamp (float)`: Timestamp

##### store_episode
```python
def store_episode(self, sensory_input: List[float], action_taken: Dict[str, Any],
                 reward: float, neural_pattern: List[float], timestamp: float)
```

Store new episode in episodic memory.

**Parameters:**
- `sensory_input (List[float])`: Sensory input
- `action_taken (Dict[str, Any])`: Action taken
- `reward (float)`: Reward received
- `neural_pattern (List[float])`: Neural activation pattern
- `timestamp (float)`: Timestamp

##### get_episode_count
```python
def get_episode_count(self) -> int
```

Get number of episodes in episodic memory.

##### get_working_memory_usage
```python
def get_working_memory_usage(self) -> int
```

Get working memory usage.

### PlasticityRule

Complete plasticity system integrating all plasticity rules.

#### Methods

##### initialize
```python
def initialize(self)
```

Initialize plasticity system.

##### update
```python
def update(self, dt: float)
```

Update plasticity system.

**Parameters:**
- `dt (float)`: Time step duration

##### record_spike_pair
```python
def record_spike_pair(self, synapse: Any, pre_spike: float, post_spike: float)
```

Record spike pair for plasticity computation.

**Parameters:**
- `synapse (Any)`: Synapse object
- `pre_spike (float)`: Pre-synaptic spike time
- `post_spike (float)`: Post-synaptic spike time

##### get_stdp
```python
def get_stdp(self) -> STDP
```

Get STDP rule.

##### get_hebbian
```python
def get_hebbian(self) -> Hebbian
```

Get Hebbian rule.

### Neuromodulator

Neuromodulation system integrating multiple neuromodulators.

#### Methods

##### initialize
```python
def initialize(self)
```

Initialize neuromodulation system.

##### update
```python
def update(self, dt: float) -> Dict[str, Any]
```

Update neuromodulation system.

**Parameters:**
- `dt (float)`: Time step duration

**Returns:**
- `Dict[str, Any]`: Neuromodulation statistics

##### apply_reward
```python
def apply_reward(self, reward: float)
```

Apply reward signal.

**Parameters:**
- `reward (float)`: Reward value

##### update_novelty
```python
def update_novelty(self, sensory_input: List[float])
```

Update novelty detection based on sensory input.

**Parameters:**
- `sensory_input (List[float])`: Sensory input pattern

### CognitiveSystem

Cognitive system integrating planning, concept formation, and attention.

#### Methods

##### initialize
```python
def initialize(self)
```

Initialize cognitive system.

##### update
```python
def update(self, dt: float) -> Dict[str, Any]
```

Update cognitive system.

**Parameters:**
- `dt (float)`: Time step duration

**Returns:**
- `Dict[str, Any]`: Cognitive system statistics

##### set_planning_depth
```python
def set_planning_depth(self, depth: int)
```

Set planning depth.

**Parameters:**
- `depth (int)`: Planning depth

##### set_attention_inhibition
```python
def set_attention_inhibition(self, strength: float)
```

Set attention inhibition strength.

**Parameters:**
- `strength (float)`: Inhibition strength

##### set_attention_excitation
```python
def set_attention_excitation(self, strength: float)
```

Set attention excitation strength.

**Parameters:**
- `strength (float)`: Excitation strength

##### process_competition
```python
def process_competition(self, competitors: List[int])
```

Process competition among neural populations.

**Parameters:**
- `competitors (List[int])`: List of competing neuron IDs

### PredictionSystem

Prediction system for sensory prediction and error computation.

#### Methods

##### initialize
```python
def initialize(self)
```

Initialize prediction system.

##### update
```python
def update(self, dt: float) -> Dict[str, Any]
```

Update prediction system.

**Parameters:**
- `dt (float)`: Time step duration

**Returns:**
- `Dict[str, Any]`: Prediction system statistics

##### process_sensory_input
```python
def process_sensory_input(self, input_data: List[float])
```

Process sensory input and update prediction.

**Parameters:**
- `input_data (List[float])`: Sensory input data

##### generate_prediction
```python
def generate_prediction(self, state: List[float]) -> List[float]
```

Generate prediction from current state.

**Parameters:**
- `state (List[float])`: Current state

**Returns:**
- `List[float]`: Predicted next state

##### get_prediction_error
```python
def get_prediction_error(self) -> float
```

Get current prediction error.

**Returns:**
- `float`: Prediction error

### DevelopmentSystem

Development system for developmental processes and maturation.

#### Methods

##### initialize
```python
def initialize(self)
```

Initialize development system.

##### update
```python
def update(self, dt: float) -> Dict[str, Any]
```

Update development system.

**Parameters:**
- `dt (float)`: Time step duration

**Returns:**
- `Dict[str, Any]`: Development system statistics

##### set_developmental_stage
```python
def set_developmental_stage(self, stage: str)
```

Set developmental stage.

**Parameters:**
- `stage (str)`: Developmental stage

##### advance_stage
```python
def advance_stage(self)
```

Advance to next developmental stage.

##### get_stage
```python
def get_stage(self) -> str
```

Get current developmental stage.

### Config

Configuration management for NLM systems.

#### Constructor
```python
def __init__(self, schema: Dict[str, Any] = None, 
             initial_config: Dict[str, Any] = None,
             preset: str = None)
```

Initialize configuration.

**Parameters:**
- `schema (Dict[str, Any], optional)`: Configuration schema
- `initial_config (Dict[str, Any], optional)`: Initial configuration
- `preset (str, optional)`: Preset configuration name

#### Methods

##### has
```python
def has(self, key: str) -> bool
```

Check if key exists in configuration.

**Parameters:**
- `key (str)`: Parameter key

**Returns:**
- `bool`: True if key exists

##### get
```python
def get(self, key: str, default: Any = None) -> Any
```

Get parameter value.

**Parameters:**
- `key (str)`: Parameter key
- `default (Any, optional)`: Default value

**Returns:**
- `Any`: Parameter value

##### get_or
```python
def get_or(self, key: str, default: Any) -> Any
```

Get parameter value or return default if not found.

**Parameters:**
- `key (str)`: Parameter key
- `default (Any)`: Default value

**Returns:**
- `Any`: Parameter value or default

##### set
```python
def set(self, key: str, value: Any)
```

Set parameter value.

**Parameters:**
- `key (str)`: Parameter key
- `value (Any)`: Parameter value

##### update
```python
def update(self, config: Dict[str, Any])
```

Update configuration with new values.

**Parameters:**
- `config (Dict[str, Any])`: Configuration updates

##### validate
```python
def validate(self) -> ParameterValidationResult
```

Validate configuration against schema.

**Returns:**
- `ParameterValidationResult`: Validation result

##### save_to_file
```python
def save_to_file(self, filepath: str)
```

Save configuration to file.

**Parameters:**
- `filepath (str)`: File path

##### load_from_file
```python
def load_from_file(self, filepath: str)
```

Load configuration from file.

**Parameters:**
- `filepath (str)`: File path

##### get_keys
```python
def get_keys(self) -> List[str]
```

Get all configuration keys.

**Returns:**
- `List[str]`: List of keys

##### summary
```python
def summary(self) -> str
```

Get configuration summary.

**Returns:**
- `str`: Configuration summary

#### Class Methods

##### create_default
```python
@staticmethod
def create_default() -> 'Config'
```

Create default configuration.

**Returns:**
- `Config`: Configuration instance

##### from_dict
```python
@staticmethod
def from_dict(config_dict: Dict[str, Any]) -> 'Config'
```

Create configuration from dictionary.

**Parameters:**
- `config_dict (Dict[str, Any])`: Configuration dictionary

**Returns:**
- `Config`: Configuration instance

##### from_preset
```python
@staticmethod
def from_preset(preset_name: str) -> 'Config'
```

Create configuration from preset.

**Parameters:**
- `preset_name (str)`: Preset name

**Returns:**
- `Config`: Configuration instance

##### load_from_file
```python
@staticmethod
def load_from_file(filepath: str) -> 'Config'
```

Load configuration from file.

**Parameters:**
- `filepath (str)`: File path

**Returns:**
- `Config`: Configuration instance

### Experiment

Experiment management system for controlled simulations.

#### Constructor
```python
def __init__(self, experiment_id: str, brain: Any = None,
             parameters: Dict[str, Any] = None, data_dir: str = None)
```

Initialize experiment.

**Parameters:**
- `experiment_id (str)`: Experiment identifier
- `brain (Any, optional)`: Brain instance
- `parameters (Dict[str, Any], optional)`: Experiment parameters
- `data_dir (str, optional)`: Data directory

#### Methods

##### run
```python
def run(self, run_id: str = None) -> ExperimentResult
```

Run experiment.

**Parameters:**
- `run_id (str, optional)`: Run identifier

**Returns:**
- `ExperimentResult`: Experiment result

##### run_parameter_sweep
```python
def run_parameter_sweep(self) -> List[ExperimentResult]
```

Run parameter sweep experiment.

**Returns:**
- `List[ExperimentResult]`: All experiment results

##### add_parameter_sweep
```python
def add_parameter_sweep(self, param_name: str, values: List[Any])
```

Add parameter sweep.

**Parameters:**
- `param_name (str)`: Parameter name
- `values (List[Any])`: Parameter values

##### compare_experiments
```python
def compare_experiments(self, experiment_results: List[ExperimentResult],
                       metric: str = "brain_stats") -> Dict[str, Any]
```

Compare multiple experiment results.

**Parameters:**
- `experiment_results (List[ExperimentResult])`: Experiment results
- `metric (str)`: Metric to compare

**Returns:**
- `Dict[str, Any]`: Comparison results

##### save_results
```python
def save_results(self, filepath: Optional[str] = None)
```

Save experiment results.

**Parameters:**
- `filepath (str, optional)`: File path

##### load_results
```python
def load_results(self, filepath: str)
```

Load experiment results.

**Parameters:**
- `filepath (str)`: File path

### Visualizer

Main visualization system integrating all visualization modules.

#### Methods

##### plot_all
```python
def plot_all(self, brain: Any, experiment: Any = None, 
             output_prefix: str = "visualizations/all")
```

Plot all visualizations.

**Parameters:**
- `brain (Any)`: Brain instance
- `experiment (Any, optional)`: Experiment instance
- `output_prefix (str)`: Output file prefix

##### plot_activity
```python
def plot_activity(self, output_file: Optional[str] = None)
```

Plot neural activity.

**Parameters:**
- `output_file (str, optional)`: Output file path

##### plot_learning_curve
```python
def plot_learning_curve(self, experiment: Any, output_file: Optional[str] = None,
                       metric: str = "firing_rate", compare_runs: bool = False)
```

Plot learning curve.

**Parameters:**
- `experiment (Any)`: Experiment instance
- `output_file (str, optional)`: Output file path
- `metric (str)`: Metric to plot
- `compare_runs (bool)`: Whether to compare multiple runs

##### plot_memory_traces
```python
def plot_memory_traces(self, brain: Any, output_file: Optional[str] = None)
```

Plot memory traces.

**Parameters:**
- `brain (Any)`: Brain instance
- `output_file (str, optional)`: Output file path

##### plot_action_selection
```python
def plot_action_selection(self, brain: Any, output_file: Optional[str] = None)
```

Plot action selection.

**Parameters:**
- `brain (Any)`: Brain instance
- `output_file (str, optional)`: Output file path

### Factory Functions

#### create_brain
```python
def create_brain(config: Config = None) -> Brain
```

Create brain with configuration.

**Parameters:**
- `config (Config, optional)`: Configuration object

**Returns:**
- `Brain`: Brain instance

#### create_default_config
```python
def create_default_config() -> Config
```

Create default configuration.

**Returns:**
- `Config`: Configuration instance

## Data Types

### NeuronType

Neuron type enumeration.

Values:
- "Excitatory"
- "Inhibitory"
- "Modulatory"
- "Sensory"
- "Motor"
- "Internal"

### FiringState

Neuron firing state enumeration.

Values:
- "Resting"
- "Active"
- "Refractory"
- "Inhibited"

### NeuronState

Neuron state for LIF dynamics.

Attributes:
- `membrane_potential`: Membrane potential (mV)
- `resting_potential`: Resting potential (mV)
- `threshold`: Firing threshold (mV)
- `reset_potential`: Post-spike reset value (mV)
- `leak_conductance`: Leak conductance (nS)
- `firing_rate`: Current firing rate (Hz)
- `firing_state`: Current firing state
- `refractory_remaining`: Steps remaining in refractory period
- `refractory_period`: Total refractory period (steps)
- `adaptation_variable`: For spike-frequency adaptation
- `last_spike_time`: Last spike timestamp
- `synaptic_input`: Accumulated synaptic input
- `total_current`: Total injected current

### SynapseType

Synapse type enumeration.

Values:
- "Excitatory"
- "Inhibitory"
- "Modulatory"
- "Electrical"
- "GapJunction"

### DevelopmentalStage

Developmental stage enumeration.

Values:
- "Initial"
- "CriticalPeriod"
- "Maturation"
- "Adult"
- "Aging"

### WorldObjectType

World object type enumeration.

Values:
- "Empty"
- "Resource"
- "Hazard"
- "Wall"
- "Marker"

### ActionType

Action type enumeration.

Values:
- "MoveForward"
- "MoveBackward"
- "MoveLeft"
- "MoveRight"
- "TurnLeft"
- "TurnRight"
- "Look"
- "LookUp"
- "LookDown"
- "Interact"
- "Eat"
- "Drink"
- "Rest"
- "Wait"
- "Custom"

### MotorCommand

Motor command enumeration.

Values:
- "MoveForward"
- "MoveBackward"
- "TurnLeft"
- "TurnRight"
- "LookLeft"
- "LookRight"
- "Interact"
- "Wait"

### WorldObjectType

World object type enumeration.

Values:
- "Empty"
- "Resource"
- "Hazard"
- "Wall"
- "Marker"

### ActionType

Action type enumeration.

Values:
- "MoveForward"
- "MoveBackward"
- "MoveLeft"
- "MoveRight"
- "TurnLeft"
- "TurnRight"
- "Look"
- "LookUp"
- "LookDown"
- "Interact"
- "Eat"
- "Drink"
- "Rest"
- "Wait"
- "Custom"

### NeuronId

Neuron identifier.

Attributes:
- `value`: Internal value

### SynapseId

Synapse identifier.

Attributes:
- `value`: Internal value

### RegionId

Region identifier.

Attributes:
- `value`: Internal value

### PopulationId

Population identifier.

Attributes:
- `value`: Internal value

## Examples

The following examples demonstrate the usage of NLM Python bindings:

1. **00_getting_started.py**: Quick start guide
2. **01_basic_simulation.py**: Basic simulation example
3. **02_learning_experiment.py**: Learning experiment example
4. **03_advanced_usage.py**: Advanced usage examples

## Installation

### Prerequisites

- Python 3.8 or higher
- NumPy (optional)
- Matplotlib (optional)
- Seaborn (optional)

### Installation from Source

```bash
pip install -e .
```

### Installation from PyPI

```bash
pip install pynlm
```

### Installation with Development Dependencies

```bash
pip install -e .[dev]
```

## Usage

### Basic Usage

```python
import pynlm

# Create configuration
config = pynlm.Config.create_default()

# Create brain
brain = pynlm.create_brain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)

# Get statistics
stats = brain.get_stats()
print(f"Total spikes: {stats['total_spikes']}")
```

### Advanced Usage

```python
import pynlm

# Create custom configuration
config = pynlm.Config.create_default()
config.set("simulation.neuron_count", 2000)
config.set("plasticity.learning_rate", 0.02)

# Create brain with custom configuration
brain = pynlm.create_brain(config)

# Initialize brain
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Visualize results
visualizer = pynlm.Visualizer(save_plots=True)
visualizer.plot_all(brain)
```

## Troubleshooting

### Common Issues

1. **Python module not found**
   - Ensure the pynlm directory is in your Python path
   - Install the package with `pip install -e .`

2. **Missing dependencies**
   - Install optional dependencies: `pip install pynlm[all]`
   - Install specific dependencies: `pip install matplotlib seaborn`

3. **Import errors**
   - Check that all required files are present
   - Ensure the package is properly installed

4. **Simulation errors**
   - Check that the brain is properly initialized before running simulation
   - Validate configuration parameters
   - Use smaller simulation parameters for debugging

## License

This project is licensed under the MIT License.

## Copyright

Copyright (c) 2024 NLM Project. All rights reserved.

## Index

For detailed documentation, refer to the individual module files:
- `pynlm/__init__.py`: Module documentation and exports
- `pynlm/brain.py`: Brain class documentation
- `pynlm/neurons.py`: Neuron model documentation
- `pynlm/memory.py`: Memory systems documentation
- `pynlm/plasticity.py`: Plasticity rules documentation
- `pynlm/neuromodulation.py`: Neuromodulation documentation
- `pynlm/cognition.py`: Cognition documentation
- `pynlm/prediction.py`: Prediction documentation
- `pynlm/development.py`: Development documentation
- `pynlm/config.py`: Configuration documentation
- `pynlm/experiment.py`: Experiment documentation
- `pynlm/visualizer.py`: Visualization documentation

---

Generated by Kilo AI. Visit https://kilo.ai for more information.
