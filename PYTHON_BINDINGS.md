# Python Bindings for NLM - Enhancement Guide

## Overview

This document describes the enhancements made to the Python bindings for the NLM (Neural Learning Machine) project. It provides comprehensive documentation for the Python API, including examples, migration guides, and best practices for using NLM from Python.

## Current State

### Existing Python Bindings

The current Python bindings are located in:
- `src/python/bindings.cpp` - Main Python bindings implementation
- `pyproject.toml` - Python project configuration

### Current API Coverage

The current Python bindings provide basic functionality:
1. **Core Configuration** - Basic configuration management
2. **Brain Core** - Access to basic brain functionality
3. **Neurons and Synapses** - Access to neural components
4. **Simple World Interface** - Basic world interaction

### Current Limitations

The current Python bindings have several limitations:
1. **Limited Memory System Access** - Limited access to advanced memory systems
2. **No Neuromodulation Access** - No access to neuromodulation systems
3. **Limited Planning and Cognition** - Limited access to cognitive systems
4. **No Advanced Features** - Missing advanced features like development, prediction, etc.
5. **Poor Documentation** - Insufficient documentation and examples
6. **Inconsistent API** - Inconsistent naming and patterns

## Enhanced Python API

### Core Functionality

#### Configuration Management

```python
"""
Configuration management for NLM.
"""

class NLMConfig:
    """
    NLM configuration manager.
    
    This class provides a Python interface to NLM configuration management,
    including loading from files, command line, and runtime settings.
    """
    
    def __init__(self, config_file=None):
        """
        Initialize NLM configuration.
        
        Args:
            config_file (str): Path to configuration file
        """
        pass
    
    def set_value(self, key, value):
        """
        Set a configuration value.
        
        Args:
            key (str): Configuration key
            value: Configuration value
        """
        pass
    
    def get_value(self, key, default=None):
        """
        Get a configuration value.
        
        Args:
            key (str): Configuration key
            default: Default value if key not found
            
        Returns:
            Configuration value
        """
        pass
    
    def load_from_file(self, config_file):
        """
        Load configuration from file.
        
        Args:
            config_file (str): Path to configuration file
            
        Returns:
            bool: True if configuration loaded successfully
        """
        pass
    
    def validate(self):
        """
        Validate configuration.
        
        Returns:
            bool: True if configuration is valid
        """
        pass
    
    def save(self, config_file):
        """
        Save configuration to file.
        
        Args:
            config_file (str): Path to configuration file
            
        Returns:
            bool: True if configuration saved successfully
        """
        pass

# Example usage
config = NLMConfig("configs/default.cfg")
config.set_value("neuron_count", 1000)
config.set_value("simulation_timestep", 0.001)
neurons = config.get_value("neuron_count", 500)
print(f"Number of neurons: {neurons}")
```

#### Brain Core

```python
"""
NLM brain core functionality.
"""

class NLMBrain:
    """
    NLM brain core functionality.
    
    This class provides access to the core brain functionality,
    including neural computation, plasticity, and basic dynamics.
    """
    
    def __init__(self, config=None):
        """
        Initialize NLM brain.
        
        Args:
            config (NLMConfig): Configuration for the brain
        """
        pass
    
    def initialize(self):
        """
        Initialize the brain with current configuration.
        
        Returns:
            bool: True if initialization successful
        """
        pass
    
    def step(self, step, time):
        """
        Advance the brain simulation by one step.
        
        Args:
            step (int): Current simulation step
            time (float): Current simulation time
            
        Returns:
            dict: Brain state information
        """
        pass
    
    def inject_current(self, neuron_id, current):
        """
        Inject current into a specific neuron.
        
        Args:
            neuron_id (int): ID of neuron to inject current
            current (float): Current value to inject
        """
        pass
    
    def get_neuron_count(self):
        """
        Get the total number of neurons.
        
        Returns:
            int: Number of neurons
        """
        pass
    
    def get_synapse_count(self):
        """
        Get the total number of synapses.
        
        Returns:
            int: Number of synapses
        """
        pass
    
    def get_total_spike_count(self):
        """
        Get the total number of spikes.
        
        Returns:
            int: Total number of spikes
        """
        pass
    
    def get_average_firing_rate(self):
        """
        Get the average firing rate.
        
        Returns:
            float: Average firing rate in Hz
        """
        pass

# Example usage
brain = NLMBrain(config)
if brain.initialize():
    for step in range(1000):
        brain.step(step, step * 0.001)
        if step % 100 == 0:
            print(f"Step {step}: {brain.get_total_spike_count()} spikes")
```

#### Memory Systems

```python
"""
NLM memory systems.
"""

class NLMMemory:
    """
    NLM memory systems.
    
    This class provides access to NLM memory systems,
    including working memory, episodic memory, and associative memory.
    """
    
    def __init__(self, brain):
        """
        Initialize memory systems.
        
        Args:
            brain (NLMBrain): Reference to brain instance
        """
        pass
    
    def get_working_memory(self):
        """
        Get working memory instance.
        
        Returns:
            WorkingMemory: Working memory instance
        """
        pass
    
    def get_episodic_memory(self):
        """
        Get episodic memory instance.
        
        Returns:
            EpisodicMemory: Episodic memory instance
        """
        pass
    
    def get_associative_memory(self):
        """
        Get associative memory instance.
        
        Returns:
            AssociativeMemory: Associative memory instance
        """
        pass
    
    def store_episode(self, episode):
        """
        Store an episode in episodic memory.
        
        Args:
            episode (dict): Episode data
        """
        pass
    
    def retrieve_similar_episodes(self, query_pattern, max_results=5):
        """
        Retrieve similar episodes.
        
        Args:
            query_pattern (list): Query pattern
            max_results (int): Maximum number of results
            
        Returns:
            list: Similar episodes
        """
        pass

# Example usage
memory = NLMMemory(brain)
working_memory = memory.get_working_memory()
episodic_memory = memory.get_episodic_memory()
associative_memory = memory.get_associative_memory()

# Store a pattern in working memory
working_memory.store([0.1, 0.5, 0.8, 0.3, 0.9], 1.0)

# Retrieve similar episodes
query = [0.2, 0.6, 0.4, 0.1, 0.7]
similar = episodic_memory.retrieve_similar_episodes(query, 3)
print(f"Found {len(similar)} similar episodes")
```

#### Neuromodulation

```python
"""
NLM neuromodulation systems.
"""

class NLMNeuromodulation:
    """
    NLM neuromodulation systems.
    
    This class provides access to NLM neuromodulation systems,
    including dopamine, curiosity, novelty, and prediction error.
    """
    
    def __init__(self, brain):
        """
        Initialize neuromodulation systems.
        
        Args:
            brain (NLMBrain): Reference to brain instance
        """
        pass
    
    def get_dopamine(self):
        """
        Get dopamine system.
        
        Returns:
            Dopamine: Dopamine system
        """
        pass
    
    def get_curiosity(self):
        """
        Get curiosity system.
n        
        Returns:
            Curiosity: Curiosity system
        """
        pass
    
    def get_novelty(self):
        """
        Get novelty system.
        
        Returns:
            Novelty: Novelty system
        """
        pass
    
    def get_prediction_error(self):
        """
        Get prediction error system.
        
        Returns:
            PredictionError: Prediction error system
        """
        pass
    
    def apply_reward(self, reward, predicted_reward):
        """
        Apply reward modulation.
        
        Args:
            reward (float): Reward value
            predicted_reward (float): Predicted reward value
        """
        pass
    
    def get_neuromodulation_level(self):
        """
        Get current neuromodulation level.
        
        Returns:
            float: Neuromodulation level
        """
        pass

# Example usage
neuromod = NLMNeuromodulation(brain)
neuromod.apply_reward(10.0, 8.0)
level = neuromod.get_neuromodulation_level()
print(f"Neuromodulation level: {level}")
```

#### Cognition Systems

```python
"""
NLM cognition systems.
"""

class NLMCognition:
    """
    NLM cognition systems.
    
    This class provides access to NLM cognition systems,
    including planning, concept formation, and attention.
    """
    
    def __init__(self, brain):
        """
        Initialize cognition systems.
        
        Args:
            brain (NLMBrain): Reference to brain instance
        """
        pass
    
    def get_planner(self):
        """
        Get planner system.
        
        Returns:
            NeuralPlanner: Planner system
        """
        pass
    
    def get_concept_formation(self):
        """
        Get concept formation system.
        
        Returns:
            ConceptFormation: Concept formation system
        """
        pass
    
    def get_attention(self):
        """
        Get attention system.
        
        Returns:
            AttentionalSelection: Attention system
        """
        pass
    
    def plan_action(self, sensory_input, motor_history):
        """
        Plan an action based on sensory input and motor history.
        
        Args:
            sensory_input (list): Sensory input pattern
            motor_history (list): Motor command history
            
        Returns:
            int: Planned action
        """
        pass
    
    def form_concepts(self, sensory_pattern, action, reward):
        """
        Form concepts from experience.
        
        Args:
            sensory_pattern (list): Sensory pattern
            action (int): Action taken
            reward (float): Reward received
        """
        pass

# Example usage
 cognition = NLMCognition(brain)
planner = cognition.get_planner()
concept_formation = cognition.get_concept_formation()

# Plan an action
sensory = [0.1, 0.5, 0.3, 0.8, 0.2]
motor_history = [0, 1, 0, 1, 0]
action = planner.plan_action(sensory, motor_history)
print(f"Planned action: {action}")
```

### Advanced Features

#### Development System

```python
"""
NLM development system.
"""

class NLMDevelopment:
    """
    NLM development system.
    
    This class provides access to NLM development system,
    including developmental stages and plasticity modulation.
    """
    
    def __init__(self, brain):
        """
        Initialize development system.
        
        Args:
            brain (NLMBrain): Reference to brain instance
        """
        pass
    
    def get_developmental_stage(self):
        """
        Get current developmental stage.
        
        Returns:
            str: Developmental stage
        """
        pass
    
    def update_development(self, timestep):
        """
        Update development system.
        
        Args:
            timestep (float): Time step
        """
        pass
    
    def get_plasticity_modifier(self):
        """
        Get current plasticity modifier.
        
        Returns:
            float: Plasticity modifier
        """
        pass

# Example usage
development = NLMDevelopment(brain)
stage = development.get_developmental_stage()
print(f"Developmental stage: {stage}")
development.update_development(0.001)
plasticity_mod = development.get_plasticity_modifier()
print(f"Plasticity modifier: {plasticity_mod}")
```

#### Prediction System

```python
"""
NLM prediction system.
"""

class NLMPrediction:
    """
    NLM prediction system.
    
    This class provides access to NLM prediction system,
    including prediction and prediction error computation.
    """
    
    def __init__(self, brain):
        """
        Initialize prediction system.
        
        Args:
            brain (NLMBrain): Reference to brain instance
        """
        pass
    
    def predict(self, sensory_input):
        """
        Make a prediction based on sensory input.
        
        Args:
            sensory_input (list): Sensory input pattern
            
        Returns:
            float: Predicted value
        """
        pass
    
    def get_prediction_error(self, actual, predicted):
        """
        Get prediction error.
        
        Args:
            actual (float): Actual value
            predicted (float): Predicted value
            
        Returns:
            float: Prediction error
        """
        pass
    
    def update_prediction(self, sensory_input, reward):
        """
        Update prediction based on reward.
        
        Args:
            sensory_input (list): Sensory input pattern
            reward (float): Reward received
        """
        pass

# Example usage
prediction = NLMPrediction(brain)
sensory = [0.1, 0.5, 0.3, 0.8, 0.2]
predicted = prediction.predict(sensory)
print(f"Predicted value: {predicted}")
actual = 0.7
error = prediction.get_prediction_error(actual, predicted)
print(f"Prediction error: {error}")
```

### World Interaction

```python
"""
NLM world interaction.
"""

class NLMWorld:
    """
    NLM world interaction.
    
    This class provides access to NLM world interaction,
    including sensory perception and motor output.
    """
    
    def __init__(self, brain):
        """
        Initialize world interaction.
        
        Args:
            brain (NLMBrain): Reference to brain instance
        """
        pass
    
    def get_sensory_input(self):
        """
        Get current sensory input.
        
        Returns:
            list: Sensory input
        """
        pass
    
    def apply_motor_command(self, command):
        """
        Apply motor command.
        
        Args:
            command (int): Motor command
            
        Returns:
            dict: Action result
        """
        pass
    
    def update(self, timestep):
        """
        Update world state.
        
        Args:
            timestep (float): Time step
        """
        pass

# Example usage
world = NLMWorld(brain)
while True:
    sensory = world.get_sensory_input()
    brain.process_sensory_input(sensory)
    action = brain.decode_motor_command()
    result = world.apply_motor_command(action)
    brain.step(step, step * 0.001)
    step += 1
    if step % 100 == 0:
        print(f"Step {step}, reward: {result['reward']}")
```

### Python API Usage Examples

#### Example 1: Simple Brain Simulation

```python
"""
Simple example of NLM brain simulation using Python bindings.
"""

from nlm import NLMBrain, NLMConfig

def main():
    # Create configuration
    config = NLMConfig("configs/default.cfg")
    config.set_value("neuron_count", 1000)
    config.set_value("simulation_timestep", 0.001)
    
    # Create brain
    brain = NLMBrain(config)
    
    # Initialize brain
    if not brain.initialize():
        print("Failed to initialize brain")
        return 1
    
    print("Brain initialized successfully")
    
    # Run simulation
    for step in range(1000):
        time = step * 0.001
        brain.step(step, time)
        
        if step % 100 == 0:
            neurons = brain.get_neuron_count()
            synapses = brain.get_synapse_count()
            spikes = brain.get_total_spike_count()
            rate = brain.get_average_firing_rate()
            
            print(f"Step {step}: {neurons} neurons, {synapses} synapses, "
                  f"{spikes} spikes, {rate:.2f} Hz average firing rate")
    
    print("Simulation completed successfully")

if __name__ == "__main__":
    main()
```

#### Example 2: Memory-Enhanced Brain

```python
"""
Example of NLM brain with enhanced memory systems.
"""

from nlm import NLMBrain, NLMConfig, NLMMemory

def main():
    # Create configuration
    config = NLMConfig("configs/default.cfg")
    config.set_value("neuron_count", 2000)
    
    # Create brain
    brain = NLMBrain(config)
    
    # Initialize brain
    if not brain.initialize():
        print("Failed to initialize brain")
        return 1
    
    # Initialize memory systems
    memory = NLMMemory(brain)
    
    print("Brain initialized with memory systems")
    
    # Simulate with memory
    for step in range(500):
        time = step * 0.001
        
        # Get sensory input (example: random pattern)
        import random
        sensory = [random.random() for _ in range(16)]
        
        # Process sensory input
        brain.process_sensory_input(sensory)
        
        # Get motor command
        action = brain.decode_motor_command()
        
        # Store sensory pattern in working memory
        memory.get_working_memory().store(sensory, 1.0)
        
        # Step brain
        brain.step(step, time)
        
        if step % 100 == 0:
            print(f"Step {step}: action {action}")
    
    print("Memory-enhanced simulation completed successfully")

if __name__ == "__main__":
    main()
```

#### Example 3: Learning with Neuromodulation

```python
"""
Example of NLM brain with neuromodulation and learning.
"""

from nlm import NLMBrain, NLMConfig, NLMNeuromodulation

def main():
    # Create configuration
    config = NLMConfig("configs/default.cfg")
    config.set_value("neuron_count", 1500)
    
    # Create brain
    brain = NLMBrain(config)
    
    # Initialize brain
    if not brain.initialize():
        print("Failed to initialize brain")
        return 1
    
    # Initialize neuromodulation systems
    neuromod = NLMNeuromodulation(brain)
    
    print("Brain initialized with neuromodulation systems")
    
    # Simulate with neuromodulation
    for step in range(300):
        time = step * 0.001
        
        # Get sensory input
        import random
        sensory = [random.random() for _ in range(16)]
        
        # Process sensory input
        brain.process_sensory_input(sensory)
        
        # Get motor command
        action = brain.decode_motor_command()
        
        # Apply reward modulation based on action
        reward = 1.0 if action == 0 else 0.0  # Simple reward
        neuromod.apply_reward(reward, 0.0)
        
        # Step brain
        brain.step(step, time)
        
        if step % 50 == 0:
            level = neuromod.get_neuromodulation_level()
            print(f"Step {step}: neuromodulation level {level:.3f}, action {action}")
    
    print("Neuromodulation-enhanced simulation completed successfully")

if __name__ == "__main__":
    main()
```

### Python API Best Practices

#### 1. Configuration Management

```python
# Always validate configuration
config = NLMConfig("config.cfg")
if not config.validate():
    errors = config.get_validation_errors()
    for error in errors:
        print(f"Configuration error: {error}")
    raise ValueError("Invalid configuration")
```

#### 2. Error Handling

```python
try:
    brain = NLMBrain(config)
    if not brain.initialize():
        raise RuntimeError("Failed to initialize brain")
    
    # Run simulation
    for step in range(1000):
        brain.step(step, step * 0.001)
        
except RuntimeError as e:
    print(f"Runtime error: {e}")
except ValueError as e:
    print(f"Configuration error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

#### 3. Performance Optimization

```python
# Use efficient data structures
brain = NLMBrain(config)
memory = NLMMemory(brain)

# Pre-allocate large data structures
sensory_pattern = [0.0] * 256
for step in range(1000):
    # Use efficient vector operations
    if step % 10 == 0:
        brain.process_sensory_input(sensory_pattern)
    
    brain.step(step, step * 0.001)
```

#### 4. Resource Management

```python
try:
    brain = NLMBrain(config)
    if brain.initialize():
        print("Brain initialized successfully")
        
        # Use brain...
        for step in range(1000):
            brain.step(step, step * 0.001)
            
finally:
    # Clean up resources
    if 'brain' in locals():
        brain.cleanup()
```

### API Compatibility

The enhanced Python bindings maintain backward compatibility with the existing API while adding new functionality:

#### Backward Compatibility

```python
# Old API (still supported)
from nlm import NLMBrain, NLMConfig

config = NLMConfig()
brain = NLMBrain(config)
brain.initialize()

# New API (recommended)
from nlm import NLMBrain, NLMConfig, NLMMemory, NLMNeuromodulation, NLMCognition

config = NLMConfig("config.cfg")
brain = NLMBrain(config)
memory = NLMMemory(brain)
neuromod = NLMNeuromodulation(brain)
cognition = NLMCognition(brain)

if brain.initialize():
    # Use all systems...
```

### Installation and Setup

#### Requirements

```bash
# Install NLM Python bindings
pip install pynlm

# Or install from source
pip install -e .
```

#### Setup Instructions

```bash
# Clone NLM repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build Python bindings
mkdir build
cd build
cmake .. -DPYTHON_EXECUTABLE=$(which python3)
make -j4

# Install Python bindings
pip install .
```

#### Testing

```bash
# Run Python bindings tests
pytest tests/test_python_bindings.py

# Run all tests
pytest tests/
```

## Conclusion

The enhanced Python bindings provide a comprehensive, user-friendly interface to NLM's advanced neural computation capabilities. Key features include:

1. **Comprehensive API Coverage**: Access to all NLM systems including memory, neuromodulation, cognition, and development.
2. **Backward Compatibility**: Maintains compatibility with existing Python code while adding new functionality.
3. **Performance Optimized**: Designed for efficient performance with minimal overhead.
4. **User-Friendly**: Intuitive API with comprehensive documentation and examples.
5. **Flexible**: Supports various configuration and runtime options.
6. **Well-Documented**: Extensive documentation with examples and best practices.

The enhanced Python bindings make it easy for researchers and developers to leverage NLM's advanced neural computation capabilities from Python, opening up new possibilities for experimentation, research, and application development.