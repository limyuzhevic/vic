# NLM Python API Reference

## Overview

The NLM Python API provides high-level access to the brain simulation with Pythonic interfaces for rapid development and experimentation.

## Module Structure

```python
import pynlm

# Core classes
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Enumerations and types
pynlm.NeuronType
pynlm.DevelopmentalStage
pynlm.ActionType
pynlm.WorldObjectType
pynlm.ConfigScope
```

## Core Classes

### Brain Class

```python
class Brain:
    def __init__(self, config):
        """Create a new brain instance.
        
        Args:
            config: Configuration object or dict
        """
        
    def initialize(self):
        """Initialize the brain with neurons and synapses."""
        
    def step(self, step_number, time=None):
        """Run one simulation step.
        
        Args:
            step_number: Current simulation step (for logging/indexing)
            time: Simulation time in milliseconds (optional)
        """
        
    def reset(self):
        """Reset brain to initial state."""
        
    # Properties and Statistics
    @property
    def total_neuron_count(self):
        """Total number of neurons in the brain."""
        
    @property
    def total_synapse_count(self):
        """Total number of synaptic connections."""
        
    @property
    def firing_neuron_count(self):
        """Number of neurons currently firing."""
        
    @property
    def average_firing_rate(self):
        """Average firing rate (spikes/second)."""
        
    @property
    def excitation_inhibition_ratio(self):
        """E/I balance ratio."""
        
    @property
    def developmental_stage(self):
        """Current developmental stage (0.0 to 1.0)."""
        
    # Sensory-Motor Interface
    def receive_sensory_input(self, sensory_input):
        """Receive sensory data from environment."""
        
    def produce_action(self):
        """Convert neural activity to motor command."""
        
    # State Management
    def save(self, filepath):
        """Save brain state to file."""
        
    def load(self, filepath):
        """Load brain state from file."""
        
    # Memory Systems (Phase 2)
    def get_working_memory(self):
        """Access working memory system."""
        
    def get_episodic_memory(self):
        """Access episodic memory system."""
        
    def get_semantic_memory(self):
        """Access semantic memory system."""
        
    def get_procedural_memory(self):
        """Access procedural memory system."""
        
    def get_prediction_system(self):
        """Access prediction system."""
        
    # Development
    def update_development(self, time_step):
        """Advance developmental stage."""
        
    def add_region(self, name):
        """Add a new neural region."""
        
    def get_region(self, region_id):
        """Get neural region by ID."""
```

### Config Class

```python
class Config:
    def __init__(self, source=None):
        """Create configuration from dict, file, or defaults.
        
        Args:
            source: Configuration source (None for defaults)
        """
        
    def has(self, key):
        """Check if configuration key exists."""
        
    def get_keys(self, prefix=None):
        """Get all configuration keys, optionally with prefix."""
        
    def get(self, key, default=None):
        """Get configuration value."""
        
    def set(self, key, value):
        """Set configuration value."""
        
    def load_from_file(self, filepath):
        """Load configuration from file."""
        
    def save_to_file(self, filepath):
        """Save configuration to file."""
        
    # Special methods for nested access
    def __getitem__(self, key):
        """Dict-like access: config['brain.neuron_count']"""
        
    def __setitem__(self, key, value):
        """Dict-like setting: config['brain.neuron_count'] = 1000"""
```

### AgentBrain Class

```python
class AgentBrain:
    def __init__(self, brain):
        """Create agent-brain interface.
        
        Args:
            brain: Brain instance to interface with
        """
        
    def initialize(self, world):
        """Initialize agent with world interface."""
        
    def process_sensory_input(self, percept):
        """Process sensory input from world."""
        
    def decode_motor_command(self):
        """Convert neural activity to motor command."""
        
    def apply_reward_modulation(self, reward, predicted_reward):
        """Apply reward signal to plasticity."""
        
    def update_development(self, time_step):
        """Update developmental state."""
        
    # Subsystem Control
    def enable_reward_modulation(self, enabled):
        """Enable/disable reward modulation."""
        
    def enable_structural_plasticity(self, enabled):
        """Enable/disable structural plasticity."""
        
    def enable_development(self, enabled):
        """Enable/disable development."""
        
    def enable_curiosity(self, enabled):
        """Enable/disable curiosity system."""
        
    # Statistics and Monitoring
    def get_curiosity_level(self):
        """Get current curiosity level."""
        
    def get_novelty_level(self):
        """Get current novelty level."""
        
    def get_prediction_error(self):
        """Get prediction error signal."""
        
    def get_neuromodulation_level(self):
        """Get current neuromodulation level."""
```

### SimpleWorld Class

```python
class SimpleWorld:
    def __init__(self):
        """Create a simple 2D world."""
        
    def configure(self, width=20, height=20, vision_width=8, vision_height=8,
                  timestep=0.1, max_energy=100.0, random_seed=None):
        """Configure world parameters.
        
        Args:
            width: World width in cells
            height: World height in cells
            vision_width: Agent vision width
            vision_height: Agent vision height
            timestep: Physics timestep (seconds)
            max_energy: Maximum agent energy
            random_seed: Random seed for reproducibility
        """
        
    def reset(self):
        """Reset world to initial state."""
        
    def update(self, timestep=None):
        """Update world physics."""
        
    def set_agent_start(self, x, y):
        """Set agent starting position."""
        
    def apply_motor_command(self, action, time=None):
        """Apply motor command to world."""
        
    # State Access
    def get_sensory_percept(self):
        """Get current sensory percept."""
        
    def get_agent_body(self):
        """Get agent body state."""
        
    # World Object Management
    def add_object(self, obj):
        """Add object to world."""
        
    def remove_object(self, x, y):
        """Remove object at position."""
        
    def is_valid_position(self, x, y):
        """Check if position is valid."""
        
    # Properties
    @property
    def width(self):
        """World width."""
        
    @property
    def height(self):
        """World height."""
        
    @property
    def simulation_time(self):
        """Current simulation time."""
        
    @property
    def max_energy(self):
        """Maximum energy capacity."""
```

### SensoryPercept Class

```python
class SensoryPercept:
    """Wrapper for sensory input from world."""
    
    @property
    def vision(self):
        """Visual input (2D array)."""
        
    @property
    def proprioception(self):
        """Body position and orientation."""
        
    @property
    def internal(self):
        """Internal state signals."""
        
    @property
    def touch(self):
        """Touch/collision input."""
        
    def get(self, modality, default=None):
        """Get specific modality."""
```

### Action Class

```python
class Action:
    """Represents agent action in world."""
    
    @property
    def action_type(self):
        """Type of action (MoveForward, TurnLeft, etc.)."""
        
    @property
    def parameters(self):
        """Action parameters (position, rotation, etc.)."""
        
    @property
    def strength(self):
        """Action strength/intensity."""
```

### WorldObject Class

```python
class WorldObject:
    """Represents object in world."""
    
    def __init__(self, x, y, obj_type, value=1.0, properties=None):
        """Create world object.
        
        Args:
            x, y: Position
            obj_type: Object type (Resource, Hazard, Wall, etc.)
            value: Object value/usefulness
            properties: Additional properties
        """
```

## Enumerations

### NeuronType

```python
pynlm.NeuronType.Excitatory    # Excitatory neuron
pynlm.NeuronType.Inhibitory    # Inhibitory neuron
pynlm.NeuronType.Sensory       # Sensory neuron
pynlm.NeuronType.Motor         # Motor neuron
pynlm.NeuronType.Modulatory    # Neuromodulatory neuron
pynlm.NeuronType.Internal      # Internal state neuron
```

### DevelopmentalStage

```python
pynlm.DevelopmentalStage.Initial        # Newborn, critical period
pynlm.DevelopmentalStage.CriticalPeriod # High plasticity
pynlm.DevelopmentalStage.Maturation     # Refinement
pynlm.DevelopmentalStage.Adult          # Stable performance
pynlm.DevelopmentalStage.Aging          # Decline (if modeled)
```

### ActionType

```python
pynlm.ActionType.MoveForward   # Move forward
pynlm.ActionType.TurnLeft      # Turn left
pynlm.ActionType.TurnRight     # Turn right
pynlm.ActionType.Interact      # Interact with object
pynlm.ActionType.Eat           # Consume resource
# ... more actions
```

### WorldObjectType

```python
pynlm.WorldObjectType.Empty      # Empty cell
pynlm.WorldObjectType.Resource   # Usable resource
pynlm.WorldObjectType.Hazard     # Damaging to agent
pynlm.WorldObjectType.Wall       # Obstacle
pynlm.WorldObjectType.Marker     # Goal/target
```

### ConfigScope

```python
pynlm.ConfigScope.Brain         # Brain parameters
pynlm.ConfigScope.Plasticity    # Plasticity rules
pynlm.ConfigScope.Neuromod      # Neuromodulation
pynlm.ConfigScope.Memory        # Memory systems
pynlm.ConfigScope.Development   # Development
pynlm.ConfigScope.Performance   # Performance settings
```

## Factory Functions

### Core Creation Functions

```python
def createDefaultConfig():
    """Create default configuration."""
    
def createBrain(config=None):
    """Create brain instance."""
    
def createSimpleWorld():
    """Create simple world instance."""
    
def createAgentBrain(brain):
    """Create agent-brain interface."""
    
def createSimpleExperiment(config=None):
    """Create simple experiment runner."""
```

### Configuration Creation

```python
def createDefaultConfig():
    """Create default configuration with all sensible defaults."""
    
def createEmptyConfig():
    """Create empty configuration."""
    
def createConfigFromDict(config_dict):
    """Create configuration from dictionary."""
    
def createConfigFromFile(filepath):
    """Create configuration from file."""
```

### Experiment Creation

```python
def createSimpleExperiment(config=None):
    """Create basic experiment framework."""
    
def createPhase4Experiment(config=None):
    """Create Phase 4 cognitive experiment setup."""
    
def createPhase6Experiment(config=None):
    """Create Phase 6 integration experiment."""
```

## Module Constants

```python
pynlm.__version__           # NLM version string
pynlm.__build_info__        # Build information
pynlm.__simulation_timestep # Default timestep
pynlm.__default_neuron_count # Default neuron count
pynlm.__default_synapse_density # Default synapse density
```

## Working Examples

### Minimal Example

```python
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate
for i in range(100):
    brain.step(i)

print(f"Done! Firing neurons: {brain.firing_neuron_count}")
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.set_agent_start(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process sensory input in brain
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decode_motor_command()
        
        # Apply motor command to world
        world.apply_motor_command(motor_cmd, world.simulation_time)
        
        # Apply reward modulation
        reward = world.get_sensory_percept().get_internal()[0] if world.get_sensory_percept().get_internal() else 0.0
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.firing_neuron_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
            print(f"  Dev Stage: {brain.developmental_stage:.3f}")

# Run the simulation
run_agent_simulation(1000)
```

### Configuration Example

```python
import pynlm
import json

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify specific parameters
config.set("brain.neuron_count", 5000)
config.set("brain.synapse_density", 0.15)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 0.8)
config.set("neuromod.curiosity.enable", True)

# Save to file
config.save_to_file("my_config.json")

# Load from file
config2 = pynlm.createDefaultConfig()
config2.load_from_file("my_config.json")

# Verify configuration
print(f"Neuron count: {config2.get('brain.neuron_count')}")
print(f"Has STDP: {config2.get('plasticity.stdp.enable')}")
```

## Error Handling

### Common Exceptions

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except pynlm.ConfigurationError as e:
    print(f"Configuration error: {e}")
except pynlm.SimulationError as e:
    print(f"Simulation error: {e}")
except pynlm.MemoryError as e:
    print(f"Memory error: {e}")
```

### Error Codes

- `CONFIG_INVALID`: Invalid configuration value
- `BRAIN_NOT_INITIALIZED`: Brain step called before initialize()
- `MEMORY_EXHAUSTED`: Memory limit exceeded
- `FILE_NOT_FOUND`: Configuration or save file not found
- `SIMULATION_ERROR`: Simulation runtime error

## Performance Tips

### Configuration Optimization

```python
# Use default config for best performance
config = pynlm.createDefaultConfig()

# Pre-compile frequently accessed parameters
neuron_count = config.get("brain.neuron_count")
synapse_density = config.get("brain.synapse_density")
```

### Memory Management

```python
# Reuse brain instance for multiple simulations
brain = pynlm.createBrain(config)
brain.initialize()

# Reset instead of recreating
for episode in range(num_episodes):
    brain.reset()
    # Run simulation
    for step in range(episode_length):
        brain.step(step)
```

### World Optimization

```python
# Configure world once for multiple agents
world = pynlm.createSimpleWorld()
world.configure(width=50, height=50, vision_width=8, vision_height=8)

# Reset instead of recreating
for episode in range(num_episodes):
    world.reset()
    # Run episode
```

## Migration Guide

### From Phase 5 to Phase 6

Key changes in Phase 6:

1. **Memory System Access**: Methods now return objects instead of null pointers
2. **Prediction Integration**: Prediction system automatically processes sensory input
3. **Enhanced Neuromodulation**: All neuromodulators fully integrated
4. **Development Effects**: Development affects plasticity, attention, and memory

Migration examples:

```python
# Phase 5: Memory systems disconnected
wm = brain.get_working_memory()  # Might be None
if wm:
    wm.store(pattern)

# Phase 6: Memory systems integrated  
wm = brain.get_working_memory()  # Always returns working memory
wm.store(pattern, priority=1.0)
```

### Backward Compatibility

- All public APIs remain unchanged
- New features are opt-in via configuration
- Default behavior preserved
- Phase 6 integration is gradual

## Development Guidelines

### Python Conventions

- Use type hints for better documentation
- Follow PEP 8 style guide
- Use context managers for resource management
- Implement proper error handling

### Performance Considerations

- Avoid recreating objects in tight loops
- Use local variables for frequently accessed data
- Pre-allocate arrays when possible
- Monitor memory usage during long simulations

### Testing

```python
# Unit testing
import unittest
import pynlm

class TestBrain(unittest.TestCase):
    def test_initialization(self):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        self.assertGreater(brain.total_neuron_count, 0)
        
    def test_step(self):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        initial_count = brain.firing_neuron_count
        brain.step(0)
        self.assertGreaterEqual(brain.firing_neuron_count, 0)
```

## Contributing

### Adding New Python API

```python
# In pynlm/python/nlm.py

class NewClass:
    """New API class."""
    
    def __init__(self, parameter):
        self.parameter = parameter
        
    def do_something(self):
        """Do something useful."""
        pass

# Register in module __init__.py or factory functions
```

### Documentation Generation

```python
# Use Sphinx for API documentation
# Add to pyproject.toml:

sphinx_apidoc = [
    'docs/api/python',
    'nlm',
    '--module-first',
    '--separate',
    '--quiet'
]
```

## Version Compatibility

### Version Support Matrix

| Python Version | NLM Version | Notes |
|----------------|-------------|-------|
| 3.8+           | 2.0+        | Current stable |
| 3.7            | 1.5         | Legacy support |
| 3.6            | 1.0         | Deprecated |

### Migration Between Versions

```python
# Version 1.x to 2.0 migration
# Old: brain.getWorkingMemory()  # Might return None
# New: brain.get_working_memory()  # Always returns WorkingMemory

# Old: config.setBool("key", True)
# New: config.set("key", True)
```

## Index

- Brain class: 8-45
- Config class: 48-85
- AgentBrain class: 88-130
- SimpleWorld class: 133-180
- Factory functions: 183-195
- Enumerations: 198-215
- Error handling: 218-230
- Performance tips: 233-245
- Migration guide: 248-260
- Development guidelines: 263-280
