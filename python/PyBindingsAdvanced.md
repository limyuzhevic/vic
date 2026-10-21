# Improved Python Bindings for NLM

This file provides improved Python bindings for the NLM (Neural Learning Machine) framework.

## Key Improvements:

### 1. Modular Structure
- Split bindings into separate header files for better organization
- Easier to maintain and extend

### 2. Enhanced API
- Property-style access for Brain and other objects
- Context managers for proper resource management
- Iterators for easy iteration over collections
- Better error handling with Python exceptions
- Type validation and conversion

### 3. Pythonic Features
- Context managers for simulation sessions
- Property decorators for attribute access
- Iterator protocol support
- Better exception conversion
- Improved documentation

## Usage Examples:

### Basic Brain Management with Context Manager:
```python
import pynlm

# Create and manage a brain with context manager
with pynlm.simulation_session() as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
    print(f"Simulation complete with {brain.getTotalSpikeCount()} spikes")
```

### Property Access:
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access brain properties as attributes
print(f"Neurons: {brain.total_neurons}")
print(f"Synapses: {brain.total_synapses}")
print(f"Firing rate: {brain.average_firing_rate:.2f}")

# Modify properties
brain.timestep = 0.01
brain.set_neuromodulation_level(0.5)
```

### Iteration:
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

print("Regions:")
for region in brain.regions:
    print(f"  {region.name} ({region.neuron_count} neurons)")

print("Motor neurons:")
for i, neuron in enumerate(brain.motor_neurons):
    print(f"  Neuron {i}: {neuron.get_activity():.2f}")
```

### Advanced Memory Control:
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
brain.enable_working_memory(500)
brain.set_episode_limit(1000)
brain.enable_prediction_system(True)
brain.enable_cognition_system(True)

# Get memory statistics
wm = brain.working_memory
total_traces = wm.get_active_traces()
print(f"Working memory traces: {total_traces}")
```

### Neuromodulation Control:
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable neuromodulation systems
brain.enable_reward_modulation(True)
brain.enable_curiosity(True)
brain.enable_novelty_detection(True)

# Control neuromodulators
brain.set_dopamine_level(0.5)
brain.set_curiosity_level(0.3)
brain.set_novelty_threshold(0.2)

# Get neuromodulation state
print(f"Neuromodulation level: {brain.get_neuromodulation_level():.2f}")
print(f"Curiosity level: {brain.get_curiosity_level():.2f}")
print(f"Novelty level: {brain.get_novelty_level():.2f}")
```

### Development Stages:
```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Enable development
brain.enable_development(True)

# Monitor development
for step in range(0, 5000, 1000):
    brain.step(step)
    stage = brain.developmental_stage
    age = brain.development_age
    print(f"Step {step}: Stage={stage}, Age={age:.1f}, Plasticity={brain.plasticity_modifier:.2f}")
```

## Module Structure:

- `pynlm/__init__.py`: Main module exports
- `pynlm/bindings/neuron_bindings.hpp`: Neuron bindings
- `pynlm/bindings/synapse_bindings.hpp`: Synapse bindings
- `pynlm/bindings/brain_bindings.hpp`: Brain bindings
- `pynlm/bindings/agent_bindings.hpp`: Agent bindings
- `pynlm/bindings/world_bindings.hpp`: World bindings
- `pynlm/bindings/config_bindings.hpp`: Config bindings
- `pynlm/bindings/memory_bindings.hpp`: Memory system bindings
- `pynlm/bindings/neuromodulation_bindings.hpp`: Neuromodulation bindings
- `pynlm/bindings/experiment_bindings.hpp`: Experiment bindings

Each binding module provides:
- Basic C++ class bindings
- Property-style access
- Context managers
- Iterators
- Advanced Pythonic features

## Advanced Features:

### 1. Property Access via Descriptors:
```python
class BrainProperty:
    def __init__(self, name, getter, setter=None):
        self.name = name
        self.getter = getter
        self.setter = setter
    
    def __get__(self, obj, objtype=None):
        return self.getter(obj)
    
    def __set__(self, obj, value):
        if self.setter:
            self.setter(obj, value)
```

### 2. Context Manager for Simulation:
```python
class SimulationSession:
    def __init__(self, brain, world=None, agent=None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.original_state = None
    
    def __enter__(self):
        self.original_state = self._capture_state()
        self.brain.initialize()
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        self._restore_state()
        self.brain.reset()
    
    def run(self, num_steps, timestep=0.1):
        for step in range(num_steps):
            if self.world:
                self.world.update(timestep)
                if self.agent:
                    percept = self.world.getSensoryPercept()
                    self.agent.processSensoryInput(percept)
                    action = self.agent.decodeMotorCommand()
                    self.world.applyMotorCommand(action, self.world.getSimulationTime())
            self.brain.step(step)
        return self._get_statistics()
```

### 3. Memory Management Helper:
```python
class MemoryManager:
    def __init__(self, brain):
        self.brain = brain
    
    def optimize_working_memory(self, target_size=1000):
        wm = self.brain.working_memory
        wm.setCapacity(target_size)
        wm.cleanupInactiveTraces()
    
    def consolidate_memories(self, importance_threshold=0.5):
        memory = self.brain.episodic_memory
        memory.consolidate(importance_threshold)
    
    def get_memory_stats(self):
        stats = {}
        if self.brain.working_memory:
            stats['working_memory'] = {
                'traces': self.brain.working_memory.getActiveTraces(),
                'capacity': self.brain.working_memory.getCapacity()
            }
        if self.brain.episodic_memory:
            stats['episodic_memory'] = {
                'episodes': self.brain.episodic_memory.getEpisodeCount(),
                'max_episodes': self.brain.episodic_memory.getMaxEpisodes()
            }
        return stats
```

## Error Handling:

The improved bindings provide better error handling:

1. **Custom Python exceptions** that map to C++ errors
2. **Validation** for invalid operations
3. **Helpful error messages** with context
4. **Graceful degradation** for unsupported features

## Testing:

The improved bindings include comprehensive tests:

```python
def test_property_access():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test property access
    assert brain.total_neurons > 0
    assert brain.timestep > 0
    assert brain.average_firing_rate >= 0
    
    # Test property modification
    original_timestep = brain.timestep
    brain.timestep = 0.01
    assert brain.timestep == 0.01

def test_context_manager():
    with pynlm.simulation_session() as session:
        session.brain.initialize()
        for step in range(10):
            session.brain.step(step)
        assert session.brain.getTotalSpikeCount() > 0

def test_memory_management():
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test memory optimization
    memory_manager = pynlm.MemoryManager(brain)
    memory_manager.optimize_working_memory(500)
    
    # Test memory consolidation
    memory_manager.consolidate_memories(0.5)
```

## Migration Guide:

### Old API:
```python
# Old way - verbose
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
brain.step(1)
neuron_count = brain.getTotalNeuronCount()
```

### New API:
```python
# New way - cleaner
with pynlm.simulation_session() as brain:
    brain.initialize()
    for step in range(2):
        brain.step(step)
    print(f"Neurons: {brain.total_neurons}")
```

## Conclusion:

The improved Python bindings provide:
1. **Better organization** with modular structure
2. **Pythonic features** like context managers and properties
3. **Enhanced usability** with iteration and type safety
4. **Advanced functionality** for complex simulations
5. **Better error handling** with helpful messages
6. **Improved documentation** with examples and guides

These improvements make it easier to:
- Write cleaner Python code
- Work with complex simulations
- Manage memory efficiently
- Debug and test code
- Extend the functionality