# Improved Python Bindings for NLM (pynlm)

This document describes the enhanced Python bindings for NLM, providing a more Pythonic and user-friendly interface.

## 1. Enhanced Python API Design

### 1.1 Pythonic Interface Design

The improved bindings follow Python best practices:

- **Descriptive naming**: Method names are self-documenting
- **Default parameters**: Sensible defaults reduce boilerplate
- **Context managers**: Resources are automatically managed
- **Exception handling**: Pythonic exception hierarchy
- **Type hints**: Optional type annotations for better IDE support
- **Documentation**: Comprehensive docstrings and examples

### 1.2 Core Class Improvements

#### A. Enhanced Brain Class
```python
class nlm.Brain:
    def __init__(self, config=None):
        """Initialize brain with optional configuration.
        
        Args:
            config (nlm.Config, optional): Configuration object.
                If None, default configuration is used.
        """
        ...
    
    def initialize(self, step_callback=None):
        """Initialize the brain.
        
        Args:
            step_callback (callable, optional): Callback function called
                after each simulation step. Should accept step number.
        
        Returns:
            bool: True if initialization successful, False otherwise.
        """
        ...
    
    def run_simulation(self, steps=100, timestep=0.001, 
                      callback=None, record_metrics=False):
        """Run simulation with optional callbacks and metrics recording.
        
        Args:
            steps (int): Number of simulation steps.
            timestep (float): Time step duration in seconds.
            callback (callable, optional): Function called after each step.
            record_metrics (bool): Whether to record simulation metrics.
        
        Returns:
            dict: Simulation metrics if record_metrics=True.
        """
        ...
    
    def save_brain_state(self, filepath, compression=True):
        """Save brain state to file.
        
        Args:
            filepath (str): Path to save file.
            compression (bool): Whether to compress the checkpoint.
        
        Returns:
            bool: True if save successful, False otherwise.
        """
        ...
    
    def load_brain_state(self, filepath):
        """Load brain state from file.
        
        Args:
            filepath (str): Path to checkpoint file.
        
        Returns:
            bool: True if load successful, False otherwise.
        """
        ...
    
    @contextmanager
    def simulation_context(self, config=None):
        """Context manager for safe simulation execution.
        
        Args:
            config (dict): Configuration for simulation.
        
        Yields:
            nlm.Brain: Configured brain instance.
        
        Example:
            with brain.simulation_context({'neuron_count': 1000}) as brain:
                brain.run_simulation(100)
        """
        ...
```

#### B. Enhanced Config Class
```python
class nlm.Config:
    def __init__(self, config_dict=None):
        """Initialize configuration from dictionary or defaults.
        
        Args:
            config_dict (dict, optional): Configuration dictionary.
        """
        ...
    
    @classmethod
    def from_file(cls, filepath):
        """Load configuration from file.
        
        Args:
            filepath (str): Path to configuration file.
        
        Returns:
            nlm.Config: Configuration object.
        
        Raises:
            FileNotFoundError: If file does not exist.
            ValueError: If configuration is invalid.
        """
        ...
    
    @classmethod
    def from_json(cls, json_string):
        """Load configuration from JSON string.
        
        Args:
            json_string (str): JSON configuration string.
        
        Returns:
            nlm.Config: Configuration object.
        """
        ...
    
    @classmethod
    def from_yaml(cls, yaml_string):
        """Load configuration from YAML string.
        
        Args:
            yaml_string (str): YAML configuration string.
        
        Returns:
            nlm.Config: Configuration object.
        
        Raises:
            ImportError: If PyYAML is not installed.
        """
        ...
    
    def set(self, key, value, source='user'):
        """Set configuration value.
        
        Args:
            key (str): Configuration key.
            value: Configuration value.
            source (str): Configuration source (user, system, default).
        """
        ...
    
    def get(self, key, default=None, value_type=None):
        """Get configuration value with type conversion.
        
        Args:
            key (str): Configuration key.
            default: Default value if key not found.
            value_type (type): Desired value type.
        
        Returns:
            Value or default if key not found.
        """
        ...
    
    def validate(self, schema=None):
        """Validate configuration against schema.
        
        Args:
            schema (dict, optional): Validation schema.
        
        Returns:
            tuple: (bool, list) - (is_valid, error_messages).
        """
        ...
    
    def export(self, format='json', file=None):
        """Export configuration in specified format.
        
        Args:
            format (str): Export format ('json', 'yaml', 'ini').
            file (str, optional): Output file path.
        
        Returns:
            str: Configuration string if file=None.
        """
        ...
```

#### C. Enhanced AgentBrain Class
```python
class nlm.AgentBrain:
    def __init__(self, brain):
        """Initialize agent brain interface.
        
        Args:
            brain (nlm.Brain): Underlying brain instance.
        """
        ...
    
    def connect(self, world, episode_timeout=3600):
        """Connect agent to world.
        
        Args:
            world (nlm.SimpleWorld): World to connect to.
            episode_timeout (float): Maximum episode duration in seconds.
        """
        ...
    
    def run_episode(self, max_steps=None, reward_threshold=None,
                    exploration_callback=None):
        """Run a single episode with learning.
        
        Args:
            max_steps (int, optional): Maximum steps per episode.
            reward_threshold (float, optional): Episode termination reward.
            exploration_callback (callable, optional): Function called
                during exploration for custom behavior.
        
        Returns:
            dict: Episode statistics including rewards, steps, etc.
        """
        ...
    
    def train(self, num_episodes, max_steps_per_episode=None,
              checkpoint_interval=None, callback=None):
        """Train agent over multiple episodes.
        
        Args:
            num_episodes (int): Number of training episodes.
            max_steps_per_episode (int, optional): Maximum steps.
            checkpoint_interval (int, optional): Save checkpoint every N episodes.
            callback (callable, optional): Progress callback.
        
        Returns:
            list: Training statistics for each episode.
        """
        ...
    
    def evaluate(self, num_episodes, max_steps_per_episode=None):
        """Evaluate trained agent.
        
        Args:
            num_episodes (int): Number of evaluation episodes.
            max_steps_per_episode (int, optional): Maximum steps.
        
        Returns:
            dict: Evaluation results and statistics.
        """
        ...
```

## 2. Enhanced Type System

### 2.1 Pythonic Type Representations
```python
# Enhanced type definitions with Pythonic interfaces
class nlm.NeuronType:
    """Neuron type enumeration with descriptive methods."""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    SENSORY = 3
    MOTOR = 4
    INTERNAL = 5
    
    @classmethod
    def is_excitatory(cls, neuron_type):
        """Check if neuron type is excitatory."""
        return neuron_type in (cls.EXCITATORY, cls.INTERNAL)
    
    @classmethod
    def is_inhibitory(cls, neuron_type):
        """Check if neuron type is inhibitory."""
        return neuron_type == cls.INHIBITORY
    
    @classmethod
    def is_sensory(cls, neuron_type):
        """Check if neuron type is sensory."""
        return neuron_type == cls.SENSORY
    
    @classmethod
    def is_motor(cls, neuron_type):
        """Check if neuron type is motor."""
        return neuron_type == cls.MOTOR

class nlm.SynapseType:
    """Synapse type enumeration with descriptive methods."""
    EXCITATORY = 0
    INHIBITORY = 1
    MODULATORY = 2
    ELECTRICAL = 3
    GAP_JUNCTION = 4
    
    @classmethod
    def is_excitatory(cls, synapse_type):
        """Check if synapse type is excitatory."""
        return synapse_type in (cls.EXCITATORY, cls.ELECTRICAL)
    
    @classmethod
    def is_inhibitory(cls, synapse_type):
        """Check if synapse type is inhibitory."""
        return synapse_type in (cls.INHIBITORY, cls.MODULATORY)
```

### 2.2 Enhanced ID Classes
```python
class nlm.NeuronId:
    """Enhanced neuron ID with Pythonic interface."""
    def __init__(self, value):
        self.value = value
    
    def index(self):
        """Get the index representation."""
        return self.value
    
    def __repr__(self):
        return f"<NeuronId: {self.value}>"
    
    def __hash__(self):
        return hash(self.value)
    
    def __eq__(self, other):
        if isinstance(other, NeuronId):
            return self.value == other.value
        return False
    
    @property
    def is_valid(self):
        """Check if ID is valid."""
        return self.value > 0

# Similar enhanced classes for SynapseId, RegionId, PopulationId
```

## 3. Advanced Python Features

### 3.1 Context Managers and Resource Management
```python
# Brain context manager for safe resource management
with nlm.Brain(config) as brain:
    brain.initialize()
    result = brain.run_simulation(100)
    # Brain automatically cleaned up on exit

# World context manager
with nlm.SimpleWorld(width=80, height=60) as world:
    world.configure()
    agent = nlm.AgentBrain(brain)
    agent.connect(world)
    # World automatically cleaned up on exit
```

### 3.2 Observer Pattern for Event Handling
```python
class nlm.SimulationObserver:
    """Abstract base class for simulation observers."""
    
    def on_step(self, brain, step, metrics):
        """Called after each simulation step."""
        pass
    
    def on_episode_end(self, agent, episode_stats):
        """Called at the end of an episode."""
        pass
    
    def on_experiment_end(self, experiment, results):
        """Called at the end of an experiment."""
        pass

class nlm.CompositeObserver(nlm.SimulationObserver):
    """Observer that delegates to multiple observers."""
    def __init__(self, *observers):
        self.observers = observers
    
    def on_step(self, brain, step, metrics):
        for observer in self.observers:
            observer.on_step(brain, step, metrics)
    
    # ... other methods

# Usage example
class LoggingObserver(nlm.SimulationObserver):
    def on_step(self, brain, step, metrics):
        print(f"Step {step}: firing_rate={metrics.firing_rate:.2f}")

# Attach observer to brain
brain = nlm.Brain(config)
observer = LoggingObserver()
brain.attach_observer(observer)
```

### 3.3 Strategy Pattern for Behavior Control
```python
class nlm.ExplorationStrategy:
    """Abstract base class for exploration strategies."""
    
    def select_action(self, brain, percept, available_actions):
        """Select action based on current percept."""
        raise NotImplementedError

class nlm.EpsilonGreedyStrategy(nlm.ExplorationStrategy):
    """Epsilon-greedy exploration strategy."""
    
    def __init__(self, epsilon=0.1):
        self.epsilon = epsilon
    
    def select_action(self, brain, percept, available_actions):
        if random.random() < self.epsilon:
            return random.choice(available_actions)
        else:
            return self.greedy_action(brain, percept)

class nlm.UCBStrategy(nlm.ExplorationStrategy):
    """Upper Confidence Bound exploration strategy."""
    
    def __init__(self, c=2.0):
        self.c = c
        self.action_counts = {}
        self.action_values = {}
    
    def select_action(self, brain, percept, available_actions):
        if not available_actions:
            return None
        
        # Calculate UCB values for all actions
        ucb_values = []
        for action in available_actions:
            count = self.action_counts.get(action, 0)
            value = self.action_values.get(action, 0.0)
            if count == 0:
                ucb_values.append(float('inf'))
            else:
                ucb_value = value + self.c * math.sqrt(math.log(sum(self.action_counts.values()) + 1) / count)
                ucb_values.append(ucb_value)
        
        return available_actions[ucb_values.index(max(ucb_values))]
```

## 4. Enhanced Memory and Data Management

### 4.1 Memory Management
```python
class nlm.MemoryManager:
    """Memory management for Python bindings."""
    
    @staticmethod
    def get_memory_usage(brain):
        """Get current memory usage of brain."""
        ...
    
    @staticmethod
    def set_memory_limit(limit_mb):
        """Set memory usage limit in megabytes."""
        ...
    
    @staticmethod
    def cleanup_unused_objects():
        """Clean up unused objects from garbage collection."""
        ...
    
    @staticmethod
    def profile_memory_access():
        """Start memory profiling."""
        ...

# Usage example
nlm.MemoryManager.set_memory_limit(1024)  # 1GB limit
usage = nlm.MemoryManager.get_memory_usage(brain)
print(f"Memory usage: {usage:.2f} MB")
```

### 4.2 Data Management
```python
class nlm.DataManager:
    """Data management for simulation results."""
    
    @staticmethod
    def save_simulation_data(brain, filepath, format='npy'):
        """Save simulation data to file."""
        ...
    
    @staticmethod
    def load_simulation_data(filepath, format='npy'):
        """Load simulation data from file."""
        ...
    
    @staticmethod
    def create_dataset_manager(config_file=None):
        """Create a dataset manager for organizing experiments."""
        ...
```

## 5. Integration with Python Ecosystem

### 5.1 NumPy Integration
```python
class nlm.NumpyIntegration:
    """Integration with NumPy for numerical computing."""
    
    @staticmethod
    def get_neuron_states(brain):
        """Get neuron states as NumPy array."""
        ...
    
    @staticmethod
    def get_synapse_weights(brain):
        """Get synapse weights as NumPy array."""
        ...
    
    @staticmethod
    def get_population_activity(brain, population_id):
        """Get population activity as NumPy array."""
        ...
    
    @staticmethod
    def create_visualization_data(brain):
        """Create visualization-ready data structures."""
        ...
```

### 5.2 Pandas Integration
```python
class nlm.PandasIntegration:
    """Integration with pandas for data analysis."""
    
    @staticmethod
    def create_experiment_dataframe(experiments):
        """Create pandas DataFrame from experiment results."""
        ...
    
    @staticmethod
    def create_metrics_time_series(metrics):
        """Create time series DataFrame from metrics."""
        ...
    
    @staticmethod
    def analyze_experiments(dataframe, analysis_type='statistical'):
        """Perform analysis on experiment data."""
        ...
```

### 5.3 Visualization Integration
```python
class nlm.VisualizationIntegration:
    """Integration with visualization libraries."""
    
    @staticmethod
    def create_activity_plots(brain, output_file):
        """Create activity plots and save to file."""
        ...
    
    @staticmethod
    def create_network_visualization(brain, output_file):
        """Create network visualization."""
        ...
    
    @staticmethod
    def create_brain_state_animation(brain, output_file, fps=10):
        """Create animation of brain state evolution."""
        ...
```

## 6. Performance and Profiling

### 6.1 Performance Monitoring
```python
class nlm.PerformanceMonitor:
    """Performance monitoring for Python bindings."""
    
    @staticmethod
    def start_profiling():
        """Start performance profiling."""
        ...
    
    @staticmethod
    def stop_profiling():
        """Stop performance profiling."""
        ...
    
    @staticmethod
    def get_profiling_results():
        """Get profiling results."""
        ...
    
    @staticmethod
    def profile_function(func, *args, **kwargs):
        """Profile a specific function."""
        ...
```

### 6.2 Memory Profiling
```python
class nlm.MemoryProfiler:
    """Memory profiling for Python bindings."""
    
    @staticmethod
    def start_memory_profiling():
        """Start memory profiling."""
        ...
    
    @staticmethod
    def stop_memory_profiling():
        """Stop memory profiling."""
        ...
    
    @staticmethod
    def get_memory_usage():
        """Get current memory usage."""
        ...
    
    @staticmethod
    def analyze_memory_leaks():
        """Analyze potential memory leaks."""
        ...
```

## 7. Documentation and Examples

### 7.1 Enhanced Documentation
```python
"""
NLM Python Bindings Documentation
=================================

This module provides Python bindings for the NLM (Neural Learning Machine) framework,
enabling Python developers to create neural simulations with ease.

Key Features:
------------
- Pythonic interface to C++ NLM framework
- Comprehensive configuration management
- Advanced simulation capabilities
- Integration with Python ecosystem
- Performance monitoring and profiling
- Extensive documentation and examples

Getting Started:
---------------
>>> import nlm
>>> config = nlm.Config({
...     'neuron_count': 1000,
...     'region_count': 2,
...     'connection_probability': 0.1
... })
>>> brain = nlm.Brain(config)
>>> brain.initialize()
>>> result = brain.run_simulation(100)

Examples:
---------
See examples/ directory for comprehensive examples.

API Reference:
--------------
See individual class documentation for detailed API reference.

"""
```

### 7.2 Example Scripts
```python
# examples/basic_usage.py
import nlm

# Create configuration
config = nlm.Config({
    'neuron_count': 1000,
    'region_count': 2,
    'connection_probability': 0.1
})

# Create brain
brain = nlm.Brain(config)
brain.initialize()

# Run simulation
result = brain.run_simulation(100)
print(f"Simulation completed. Average firing rate: {result.avg_firing_rate:.2f}")

# Save brain state
brain.save_brain_state("brain_checkpoint.nml")

# Create world and agent
world = nlm.SimpleWorld(width=80, height=60)
agent = nlm.AgentBrain(brain)
agent.connect(world)

# Run episode
episode_stats = agent.run_episode(max_steps=500)
print(f"Episode completed. Total reward: {episode_stats.total_reward:.2f}")

# Train agent
training_stats = agent.train(num_episodes=100, max_steps_per_episode=500)
print(f"Training completed. Average reward: {training_stats.avg_reward:.2f}")

# Evaluate agent
evaluation_stats = agent.evaluate(num_episodes=20)
print(f"Evaluation completed. Success rate: {evaluation_stats.success_rate:.2f}")

# examples/advanced_usage.py
import nlm
import matplotlib.pyplot as plt

# Create sophisticated experiment
config = nlm.Config.from_file("experiment_config.json")

# Use advanced features
with nlm.Brain(config).simulation_context() as brain:
    # Add observers
    logging_observer = nlm.LoggingObserver()
    brain.attach_observer(logging_observer)
    
    # Run experiment with performance monitoring
    with nlm.PerformanceMonitor():
        result = brain.run_simulation(1000, callback=lambda step: print(f"Step {step}"))
    
    # Analyze results
    analysis = nlm.PandasIntegration.analyze_experiments([result])
    print(f"Analysis complete. Mean firing rate: {analysis['mean_firing_rate']:.2f}")

# Create visualizations
nlm.VisualizationIntegration.create_activity_plots(brain, "activity_plots.png")
nlm.VisualizationIntegration.create_network_visualization(brain, "network.png")

# examples/scripting_example.py
import nlm.scripting as nlm_script

# Create a script for automated experiments
script = nlm_script.NLMScript("research_protocol")
script.add_step("initialize_brain", config={'neuron_count': 1000})
script.add_step("run_simulation", steps=100)
script.add_step("save_results", filename="experiment_1")

# Run script
script.run()

# examples/automation_example.py
import nlm.automation as nlm_auto

# Create automated experiment manager
manager = nlm_auto.ExperimentManager()

# Define parameter search space
parameter_space = {
    'neuron_count': [500, 1000, 2000],
    'learning_rate': [0.001, 0.01, 0.1],
    'connection_probability': [0.05, 0.1, 0.2]
}

# Run parameter search
results = manager.run_parameter_sweep(parameter_space, nlm.Script('basic_experiment'))

# Analyze and visualize results
manager.visualize_results(results)
manager.export_results("parameter_search_results.json")
```

## 8. Installation and Setup

### 8.1 Python Package Installation
```bash
# Install from source
pip install -e .

# Install with additional dependencies
pip install -e .[all]

# Install development version
pip install -e .[dev]
```

### 8.2 Environment Setup
```python
# Environment setup script
import nlm.setup

# Create virtual environment
nlm.setup.create_virtual_environment()

# Install package in development mode
nlm.setup.install_in_development_mode()

# Install optional dependencies
nlm.setup.install_optional_dependencies(['numpy', 'pandas', 'matplotlib', 'jupyter'])

# Test installation
nlm.setup.test_installation()
```

## 9. Migration Guide

### 9.1 From Legacy C++ API to Python API
```python
# Legacy C++ API (old)
Brain brain(config);
brain.initialize();
brain.step(step, time);
brain.getTotalSpikeCount();

# New Python API (improved)
import nlm
config = nlm.Config(config_dict)
brain = nlm.Brain(config)
brain.initialize()
brain.run_simulation(steps, timestep)
stats = brain.get_stats()

# Key improvements:
# - Type safety with Python
# - Better error handling
# - Context managers for resource management
# - Enhanced documentation and examples
# - Integration with Python ecosystem
```

## 10. Future Enhancements

### 10.1 Planned Features
- Machine learning integration for experiment optimization
- AutoML for parameter tuning
- Advanced visualization with Plotly
- Real-time monitoring and visualization
- Cloud deployment capabilities
- Distributed computing support
- Enhanced error recovery and fault tolerance

### 10.2 Community Contributions
- Python examples and tutorials
- Contributed scripts and workflows
- Python package extensions
- Performance optimization contributions
- Bug fixes and improvements

## Benefits of Enhanced Python Bindings

1. **Pythonic Interface**: Intuitive, Pythonic API that follows Python conventions
2. **Type Safety**: Better error detection with Python type hints
3. **Resource Management**: Context managers for automatic resource cleanup
4. **Integration**: Seamless integration with Python scientific ecosystem
5. **Documentation**: Comprehensive documentation and examples
6. **Performance**: Optimized performance with minimal overhead
7. **Extensibility**: Easy to extend with new features and functionality
8. **Community**: Active community of Python developers and researchers

These enhanced Python bindings make NLM accessible to Python developers while maintaining the power and flexibility of the C++ implementation. The bindings provide a bridge between the high-performance C++ core and the rich Python ecosystem, enabling powerful neural simulations with ease of use and flexibility.