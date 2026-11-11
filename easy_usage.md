# NLM Python Wrapper Library

This module provides a Python-friendly interface to the NLM (Neural Learning Machine) neural simulation framework. It offers high-level abstractions and convenience methods to make working with complex neural computations more intuitive and accessible for Python users.

## Key Features

### High-Level Abstractions
- Simplified brain creation and configuration
- Batch operations for common tasks
- Memory management helpers
- Performance monitoring utilities

### Enhanced Error Handling
- Pythonic exception classes
- Detailed error messages with context
- Graceful fallback behaviors
- Validation of inputs

### Scientific Computing Integration
- NumPy array support for neural data
- Pandas DataFrame for population statistics
- Matplotlib integration for visualization
- Jupyter notebook support

### Configuration Management
- JSON configuration with defaults
- Command-line argument parsing
- Environment variable support
- Configuration validation

### Development Tools
- Brain debugging utilities
- Step-by-step execution control
- Visualization helpers
- Performance profiling

### Advanced Features
- Parallel processing capabilities
- GPU acceleration integration
- Distributed computing support
- Custom middleware plugins

## Installation

```bash
pip install pynlm

# Or install from source
pip install -e .
```

## Basic Usage

```python
from pynlm import NeuralBrain, NeuralWorld, create_brain

# Create a brain with default configuration
brain = NeuralBrain()
brain.initialize()

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)

# Get statistics
stats = brain.get_statistics()
print(f"Average firing rate: {stats['average_firing_rate']:.2f} Hz")
print(f"Total spikes: {stats['total_spikes']}")

# Save brain state
brain.save("my_brain.nlm")
```

## Advanced Usage

```python
import numpy as np
from pynlm import NeuralWorld, Vision, ActionType
import matplotlib.pyplot as plt

# Create a world with vision
world = NeuralWorld(width=640, height=480)

# Create a neural population
population = brain.create_population(
    neuron_type="sensory",
    size=100,
    params={
        "receptor_density": 0.8,
        "response_threshold": -45.0
    }
)

# Run learning experiment
experiment = LearningExperiment(brain, world)
experiment.run(
    duration=1000,
    stimulus=Vision(width=32, height=32, channels=3),
    learning_rate=0.01,
    reward_schedule="exponential"
)

# Analyze results
results = experiment.get_results()
plt.figure(figsize=(12, 4))
plt.subplot(1, 3, 1)
plt.plot(results['firing_rates'])
plt.title("Firing Rate Dynamics")

plt.subplot(1, 3, 2)
plt.hist(results['synaptic_weights'], bins=50)
plt.title("Synaptic Weight Distribution")

plt.subplot(1, 3, 3)
plt.plot(results['prediction_errors'])
plt.title("Prediction Error")
plt.tight_layout()
plt.show()
```

## Configuration

### Default Configuration
```python
from pynlm import Config

config = Config()
config.set("brain.neuron_count", 500)
config.set("brain.region_count", 2)
config.set("brain.connection_probability", 0.15)
config.set("plasticity.stdp_enabled", True)
config.set("plasticity.hebbian_enabled", True)
config.set("plasticity.reward_modulated_enabled", True)
```

### Environment Variables
```bash
export NLM_NEURON_COUNT=1000
export NLM_SIMULATION_STEPS=5000
export NLM_RANDOM_SEED=42
```

## Machine Learning Integration

```python
from pynlm import NeuralBrain, create_brain, LearningExperiment
import sklearn.metrics

# Create and train a brain
brain = create_brain(neuron_count=500, regions=2)

# Define training data
X_train = np.random.randn(100, 10)  # 100 samples, 10 features
Y_train = np.random.randn(100)      # Target values

# Train using neural simulation
experiment = LearningExperiment(brain)
for epoch in range(100):
    for i in range(len(X_train)):
        # Convert input to neural stimulus
        stimulus = Vision(width=10, height=10, channels=1)
        stimulus.set_data(X_train[i].reshape(10, 10, 1))
        
        # Run neural computation
        brain.receive_sensory_input(stimulus)
        action = brain.produce_action()
        
        # Calculate reward (simplified)
        reward = -abs(Y_train[i] - action.get_parameter())
        brain.apply_reward_modulation(reward, 0.0)
    
    # Evaluate performance
    if epoch % 10 == 0:
        error = sklearn.metrics.mean_squared_error(Y_train, predictions)
        print(f"Epoch {epoch}, MSE: {error:.4f}")
```

## Development and Debugging

### Debugging Tools
```python
from pynlm import BrainDebugger, create_debug_brain

# Create a debug-enabled brain
debug_brain = create_debug_brain(neuron_count=100)
debug_brain.initialize()

# Set up debugging
debug_brain.enable_neuron_logging(True)
debug_brain.enable_spike_logging(True)
debug_brain.enable_plasticity_logging(True)

# Run with debugging output
for step in range(100):
    debug_brain.step(step)
    if debug_brain.should_stop():
        break

# Get debug information
debug_info = debug_brain.get_debug_info()
print(f"Debug summary: {debug_info['summary']}")
print(f"Spike rate: {debug_info['spike_rate']:.2f} Hz")
print(f"Memory usage: {debug_info['memory_usage']:.2f} MB")
```

### Performance Monitoring
```python
from pynlm import PerformanceProfiler

profiler = PerformanceProfiler(brain)

with profiler.time_section("simulation"):
    for step in range(1000):
        brain.step(step)

with profiler.time_section("analysis"):
    stats = brain.get_statistics()

print("Performance Report:")
print(f"Simulation: {profiler.get_time('simulation'):.2f}s")
print(f"Analysis: {profiler.get_time('analysis'):.2f}s")
print(f"Total time: {profiler.get_total_time():.2f}s")
print(f"Speed: {profiler.get_speed():.2f} steps/s")
```

## Examples and Tutorials

### 1. Simple Brain Training
```python
"""
This example demonstrates how to create and train a simple neural network
using the NLM framework.
"""

from pynlm import NeuralBrain, Vision, create_brain, LearningExperiment
import numpy as np

def simple_training_example():
    # Create a brain
    brain = create_brain(neuron_count=200, regions=2)
    brain.initialize()
    
    # Define a simple pattern to learn
    pattern = np.array([[0, 0, 1, 1, 0, 0],
                        [0, 1, 0, 0, 1, 0],
                        [1, 0, 0, 0, 0, 1],
                        [1, 0, 0, 0, 0, 1],
                        [0, 1, 0, 0, 1, 0],
                        [0, 0, 1, 1, 0, 0]])
    
    # Create vision input
    stimulus = Vision(width=6, height=6, channels=1)
    
    # Run learning experiment
    experiment = LearningExperiment(brain)
    experiment.run(
        duration=500,
        stimulus=stimulus,
        learning_rate=0.01
    )
    
    # Analyze results
    results = experiment.get_results()
    return results

if __name__ == "__main__":
    results = simple_training_example()
    print(f"Training completed in {len(results['steps'])} steps")
    print(f"Final firing rate: {results['firing_rates'][-1]:.2f} Hz")
```

### 2. Motor Control Example
```python
"""
This example demonstrates how to use NLM for motor control tasks.
"""

from pynlm import NeuralWorld, ActionType, create_brain, LearningExperiment
import numpy as np

def motor_control_example():
    # Create world and brain
    world = NeuralWorld(width=100, height=100)
    brain = create_brain(neuron_count=300, regions=3)
    brain.initialize()
    
    # Set up agent
    agent_body = brain.create_agent()
    agent_body.set_position(50, 50)
    
    # Create learning experiment for motor control
    experiment = LearningExperiment(brain, world)
    experiment.run(
        duration=1000,
        goal_position=(80, 80),
        action_space="continuous"
    )
    
    # Analyze motor learning
    results = experiment.get_results()
    trajectory = results['trajectory']
    
    print(f"Started at position {trajectory[0][:2]}")
    print(f"Final position {trajectory[-1][:2]}")
    print(f"Distance to goal: {np.linalg.norm(trajectory[-1][:2] - np.array([80, 80])):.2f}")
    
    return trajectory

if __name__ == "__main__":
    trajectory = motor_control_example()
```

### 3. Cognitive Task Example
```python
"""
This example demonstrates cognitive task solving using NLM.
"""

from pynlm import NeuralBrain, ConceptFormation, create_brain, LearningExperiment
import numpy as np

def cognitive_task_example():
    # Create brain with concept formation
    brain = create_brain(neuron_count=500, regions=4)
    brain.initialize()
    
    # Get concept formation module
    concept_formation = brain.get_concept_formation()
    
    # Create cognitive task: object recognition
    task_data = {
        'objects': {
            'apple': np.random.randn(50, 10),
            'banana': np.random.randn(50, 10),
            'orange': np.random.randn(50, 10)
        },
        'queries': ['apple', 'unknown', 'banana']
    }
    
    # Learn object representations
    experiment = LearningExperiment(brain)
    experiment.run(
        duration=800,
        data=task_data,
        task_type="object_recognition",
        learning_rate=0.02
    )
    
    # Test concept formation
    for query in task_data['queries']:
        if query in task_data['objects']:
            # Test recognition
            test_pattern = task_data['objects'][query]
            
            # Find similar patterns in concept space
            similarities = concept_formation.find_similar(test_pattern)
            best_match = similarities[0] if similarities else None
            
            print(f"Query: {query}")
            print(f"  Best match: {best_match['label'] if best_match else 'None'}")
            print(f"  Similarity: {best_match['similarity']:.3f}")
    
    return experiment.get_results()

if __name__ == "__main__":
    cognitive_task_example()
```

## API Reference

### Core Classes

#### NeuralBrain
The main brain interface with simplified methods for common operations.

```python
class NeuralBrain:
    def __init__(self, config=None):
        # Initialize with optional configuration
    
    def initialize(self, config=None):
        # Initialize the brain with configuration
    
    def step(self, step_number, timestep=None):
        # Perform one simulation step
    
    def receive_sensory_input(self, stimulus):
        # Inject sensory input into the brain
    
    def produce_action(self):
        # Produce motor action based on neural activity
    
    def get_statistics(self):
        # Get brain statistics and metrics
    
    def save(self, filepath):
        # Save brain state to file
    
    def load(self, filepath):
        # Load brain state from file
    
    def reset(self):
        # Reset brain to initial state
    
    def enable_logging(self, enable=True):
        # Enable/disable logging
    
    def get_debug_info(self):
        # Get debug information
```

#### NeuralWorld
Environment simulation interface.

```python
class NeuralWorld:
    def __init__(self, width, height):
        # Initialize world with dimensions
    
    def reset(self):
        # Reset world to initial state
    
    def update(self, timestep):
        # Update world state
    
    def apply_action(self, agent_id, action):
        # Apply motor action to agent
    
    def get_percept(self, agent_id):
        # Get sensory percept for agent
    
    def get_agent_body(self, agent_id):
        # Get agent body state
    
    def add_object(self, obj_type, position, params):
        # Add object to world
    
    def remove_object(self, position):
        # Remove object from world
```

### Configuration

#### Config
Python-friendly configuration interface.

```python
class Config:
    def __init__(self, config_dict=None):
        # Initialize with optional dictionary
    
    def set(self, key, value):
        # Set configuration value
    
    def get(self, key, default=None):
        # Get configuration value
    
    def has(self, key):
        # Check if key exists
    
    def load_from_file(self, filepath):
        # Load configuration from JSON file
    
    def save_to_file(self, filepath):
        # Save configuration to JSON file
    
    def validate(self, schema):
        # Validate configuration against schema
```

### Utility Classes

#### LearningExperiment
Helper class for running learning experiments.

```python
class LearningExperiment:
    def __init__(self, brain, world=None):
        # Initialize experiment
    
    def run(self, duration, **kwargs):
        # Run experiment for given duration
    
    def get_results(self):
        # Get experiment results
    
    def get_performance_metrics(self):
        # Get performance metrics
```

#### PerformanceProfiler
Performance monitoring utilities.

```python
class PerformanceProfiler:
    def time_section(self, name):
        # Context manager for timing a section
    
    def get_time(self, name):
        # Get elapsed time for section
    
    def get_total_time(self):
        # Get total profiling time
    
    def get_speed(self):
        # Get processing speed (steps/second)
```

### Helper Functions

#### Factory Functions
```python
def create_brain(neuron_count=100, regions=1, **kwargs):
    # Create a brain with specified parameters

def create_world(width, height, **kwargs):
    # Create a neural world with specified dimensions

def create_agent(brain, position=(0, 0), **kwargs):
    # Create an agent for the brain
```

#### Data Conversion Functions
```python
def to_numpy(neural_data):
    # Convert neural data to NumPy array

def from_numpy(array):
    # Convert NumPy array to neural data

def create_stimulus_from_array(data):
    # Create sensory stimulus from array data

def create_array_from_stimulus(stimulus):
    # Convert stimulus to NumPy array
```

### Exception Classes

#### NLMError
Base exception class for all NLM errors.

```python
class NLMError(Exception):
    def __init__(self, message, error_code=None):
        # Initialize error with message and optional error code
```

#### ConfigurationError
Configuration-related errors.

```python
class ConfigurationError(NLMError):
    pass
```

#### NeuralError
Neural computation-related errors.

```python
class NeuralError(NLMError):
    pass
```

#### EnvironmentError
Environment simulation errors.

```python
class EnvironmentError(NLMError):
    pass
```

## Package Structure

```
pynlm/
├── __init__.py                    # Main package exports
├── core/                          # Core neural computation
│   ├── brain.py                   # Brain interface
│   ├── neuron.py                  # Neuron simulation
│   ├── synapse.py                 # Synapse management
│   └── config.py                  # Configuration system
├── world/                         # Environment simulation
│   ├── neural_world.py            # World interface
│   ├── agent.py                   # Agent interface
│   └── environment.py             # Environment modeling
├── sensory/                       # Sensory processing
│   ├── vision.py                  # Visual input
│   ├── audio.py                   # Auditory input
│   └── internal_signals.py        # Internal neural signals
├── motor/                         # Motor control
│   ├── action.py                  # Action representation
│   ├── movement.py                # Movement control
│   └── motor_system.py            # Motor system
├── memory/                        # Memory systems
│   ├── working_memory.py          # Working memory
│   ├── episodic_memory.py         # Episodic memory
│   └── associative_memory.py      # Associative memory
├── development/                   # Developmental systems
│   ├── development.py             # Development system
│   └── maturation.py              # Neural maturation
├── neuromodulation/               # Neuromodulation
│   ├── reward.py                  # Reward system
│   ├── curiosity.py               # Curiosity system
│   └── novelty.py                 # Novelty detection
├── cognition/                     # Cognitive systems
│   ├── neural_planner.py          # Action planning
│   ├── concept_formation.py       # Concept learning
│   └── attention.py               # Attentional selection
├── prediction/                    # Prediction systems
│   ├── prediction_system.py       # Prediction system
│   └── neural_prediction.py       # Neural prediction
├── plasticity/                    # Plasticity rules
│   ├── stdp.py                    # Spike-Timing Dependent Plasticity
│   ├── hebbian.py                 # Hebbian learning
│   └── structural_plasticity.py   # Structural plasticity
├── experiments/                   # Experiment framework
│   ├── experiment.py              # Base experiment class
│   ├── metrics.py                 # Experiment metrics
│   └── learning.py                # Learning experiments
├── utilities/                     # Utility functions
│   ├── profiling.py               # Performance profiling
│   ├── debugging.py                # Debugging tools
│   └── data_conversion.py          # Data conversion
├── visualization/                 # Visualization
│   ├── plot.py                    # Plotting utilities
│   ├── interactive.py              # Interactive visualization
│   └── viewer.py                  # Brain viewer
└── cli/                           # Command-line interface
    ├── main.py                     # Main CLI entry point
    ├── config_commands.py          # Configuration commands
    ├── simulation_commands.py      # Simulation commands
    └── analysis_commands.py        # Analysis commands
```

## Future Improvements

### Planned Features
1. **GPU Acceleration**: CUDA support for neural computation
2. **Distributed Computing**: Multi-node training and simulation
3. **Advanced Visualization**: 3D neural network visualization
4. **Custom Middleware**: Plugin system for custom neural modules
5. **Real-time Visualization**: Live brain state monitoring
6. **Cloud Integration**: AWS/Azure/GCP support for cloud computing
7. **Web Interface**: REST API for remote brain control
8. **Mobile Support**: iOS/Android apps for brain simulation
9. **Quantum Computing**: Quantum neural computation support
10. **Meta-Learning**: Learning how to learn

### Research Areas
1. **Neuromorphic Computing**: Hardware-software co-design
2. **Neural Architecture Search**: Automated neural network design
3. **Transfer Learning**: Learning across different tasks
4. **Continual Learning**: Learning without forgetting
5. **Neural Interpretability**: Understanding neural computations
6. **Explainable AI**: Making neural decisions interpretable
7. **Safety and Robustness**: Safe AI systems
8. **Value Alignment**: Aligning AI with human values

## Contributing

### Code Contributions
1. Fork the repository
2. Create a feature branch
3. Implement your changes
4. Add tests and documentation
5. Submit a pull request

### Documentation Contributions
1. Fix typos and errors
2. Add examples and tutorials
3. Improve API documentation
4. Translate documentation

### Testing Contributions
1. Add new test cases
2. Improve test coverage
3. Fix existing tests
4. Add performance benchmarks

### Bug Reports
1. Check for existing issues
2. Create new issue with details
3. Provide reproduction steps
4. Suggest fixes or workarounds

## License

MIT License - See LICENSE file for details.