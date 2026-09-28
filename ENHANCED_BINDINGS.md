# NLM Python Bindings - Enhanced Documentation

This document provides comprehensive information about the enhanced Python bindings for the NLM (Neural Learning Machine) framework.

## Overview

The NLM Python bindings have been completely enhanced to provide comprehensive access to all components of the neural simulation framework. This represents a significant improvement over the original bindings, expanding from a basic set of classes to a complete, feature-rich interface.

## What's New

### Major Enhancements

1. **Comprehensive Core Neural Components**
   - `Neuron` - Individual neuron model with LIF dynamics
   - `Synapse` - Synaptic connections with delay and plasticity
   - `NeuralRegion` - Container for neural populations
   - `NeuralPopulation` - Collection of neurons with shared properties

2. **Advanced Plasticity Rules**
   - `STDP` - Spike-Timing-Dependent Plasticity
   - `HebbianRule` - Hebbian learning ("neurons that fire together, wire together")
   - `RewardModulatedSTDP` - Reward-modulated STDP
   - `TripletSTDP` - STDP with triplet interactions
   - `StructuralPlasticity` - Synapse creation and elimination

3. **Complete Memory Systems**
   - `NeuralWorkingMemory` - Short-term working memory with competitive decay
   - `NeuralEpisodicMemory` - Long-term episodic memory with replay capability

4. **Full Neuromodulation Suite**
   - `Reward` - Dopamine-based reward modulator
   - `Curiosity` - Exploration-driven curiosity module
   - `Novelty` - Detects novel sensory patterns
   - `PredictionError` - Prediction error signal for learning

5. **Development and Aging Mechanisms**
   - `DevelopmentSystem` - Manages developmental stages and plasticity changes
   - `Maturation` - Neural maturation and synaptic strengthening
   - `Synaptogenesis` - Formation of new synapses during development
   - `Pruning` - Abolition of weak or unused synapses

6. **Performance Optimizations**
   - `MemoryPool` - Efficient memory allocation for neural data
   - `EventQueue` - Event-driven computation system
   - `CheckpointSystem` - Persistence and checkpoint management
   - `SparseConnectivity` - Efficient sparse neural connectivity
   - `OptimizedBrain` - Performance-optimized brain implementation

7. **Advanced Cognitive Systems**
   - `NeuralPlanner` - Neural planning and action sequence evaluation
   - `ConceptFormation` - Concept formation from experience patterns

8. **Enhanced Sensory/Motor Systems**
   - `Vision` - Vision sensory input
   - `Audio` - Audio sensory input
   - `InternalSignals` - Internal neural signals
   - `Action` - Action representation for motor output
   - `SensoryPercept` - Sensory percept data
   - `AgentBody` - Agent body state
   - `WorldObject` - World object representation

9. **Environment and Experiment Framework**
   - `Environment` - Abstract base class for environments
   - `Experiment` - Abstract base class for experiments
   - `ExperimentRunner` - Runs experiments and collects metrics

10. **Utility Classes**
    - `SimulationClock` - Accurate simulation timing
    - `RandomGenerator` - Random number generation for simulations
    - `Logger` - Logging and debugging utilities

## Key Improvements Over Original Bindings

### Before (Original)

```python
# Original bindings were limited to these classes:
from pynlm import (
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action,
    SensoryPercept, AgentBody, WorldObject,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
)
```

### After (Enhanced)

```python
# Enhanced bindings provide comprehensive access:
from pynlm import (
    # Core neural components
    Neuron, Synapse, NeuralRegion, NeuralPopulation,
    
    # Advanced plasticity rules
    PlasticityRule, HebbianRule, STDP, RewardModulatedSTDP,
    TripletSTDP, StructuralPlasticity,
    
    # Memory systems
    Memory, NeuralWorkingMemory, NeuralEpisodicMemory,
    
    # Neuromodulation
    Neuromodulator, Reward, Curiosity, Novelty, PredictionError,
    
    # Development
    DevelopmentSystem, Maturation, Synaptogenesis, Pruning,
    
    # Performance
    MemoryPool, EventQueue, CheckpointSystem, SparseConnectivity,
    OptimizedBrain,
    
    # Cognition
    NeuralPlanner, ConceptFormation,
    
    # All original classes plus much more
    Config, Brain, AgentBrain, SimpleWorld,
    Vision, Audio, InternalSignals, Action,
    SensoryPercept, AgentBody, WorldObject,
    NeuronId, SynapseId, RegionId, PopulationId,
    NeuronType, SynapseType, DevelopmentalStage, FiringState,
    ActionType, MotorCommand, WorldObjectType,
)
```

## Factory Functions for Easy Creation

The enhanced bindings provide factory functions for easy creation of components:

### Plasticity Rule Factories

```python
# Create different types of plasticity rules
stdp_rule = pynlm.create_stdp_rule(ltp_weight=0.01, ltd_weight=0.01, tau=20.0)
hebbian_rule = pynlm.create_hebbian_rule(learning_rate=0.05)
rstdp_rule = pynlm.create_reward_modulated_stdp()

# Or use the generic factory
plasticity_rule = pynlm.create_plasticity_rule("stdp", ltp_weight=0.02)
```

### Memory System Factories

```python
# Create different memory systems
working_memory = pynlm.create_working_memory(capacity=1000)
episodic_memory = pynlm.create_episodic_memory(capacity=100)

# Or use the generic factory
memory_system = pynlm.create_memory_system("working", capacity=2000)
```

### Neuromodulator Factories

```python
# Create different neuromodulators
reward_mod = pynlm.create_reward_modulator(initial_value=0.5)
curiosity_mod = pynlm.create_curiosity_module(initial_level=0.7)
novelty_det = pynlm.Novelty()  # Direct constructor
pred_error = pynlm.PredictionError()  # Direct constructor

# Or use the generic factory
neuromodulator = pynlm.create_neuromodulator("reward", initial_value=0.8)
```

## Advanced Factory Function

The enhanced bindings include a generic factory function that creates any component type:

```python
def create_plasticity_rule(rule_type: str, **kwargs) -> 'PlasticityRule':
    """
    Create a plasticity rule of the specified type.
    
    Args:
        rule_type: Type of plasticity rule ("stdp", "hebbian", 
                  "reward_modulated_stdp", "triplet_stdp", 
                  "structural_plasticity")
        **kwargs: Rule-specific parameters
    
    Returns:
        PlasticityRule instance
    """
    rule_type = rule_type.lower()
    if rule_type == "stdp":
        return create_stdp_rule(**kwargs)
    elif rule_type == "hebbian":
        return create_hebbian_rule(**kwargs)
    elif rule_type == "reward_modulated_stdp":
        return create_reward_modulated_stdp(**kwargs)
    elif rule_type == "triplet_stdp":
        return create_stdp_rule(rule_type="triplet", **kwargs)
    elif rule_type == "structural_plasticity":
        return create_stdp_rule(rule_type="structural", **kwargs)
    else:
        raise ValueError(f"Unknown plasticity rule type: {rule_type}")
```

Similar factory functions exist for memory systems, neuromodulators, development modules, and performance optimizers.

## Usage Examples

### Example 1: Complete Neural Brain Setup

```python
import pynlm

# Create configuration
config = pynlm.create_default_config()
config.set("brain.neuron_count", 1000)
config.set("brain.region_count", 5)

# Create brain
brain = pynlm.create_brain(config)

# Create world
world = pynlm.create_simple_world()
world.configure(100, 100, 50, 50)

# Create agent brain
agent_brain = pynlm.create_agent_brain(brain)

# Initialize components
brain.initialize()
agent_brain.initialize(world)

# Create advanced components
stdp_rule = pynlm.create_stdp_rule(ltp_weight=0.02, ltd_weight=0.02)
working_memory = pynlm.create_working_memory(capacity=2000)
reward_mod = pynlm.create_reward_modulator(initial_value=0.5)
curiosity_mod = pynlm.create_curiosity_module(initial_level=0.7)
```

### Example 2: Simulation Loop with All Systems

```python
import pynlm

# Initialize all components
config = pynlm.create_default_config()
brain = pynlm.create_brain(config)
world = pynlm.create_simple_world()
agent_brain = pynlm.create_agent_brain(brain)

world.configure(50, 50, 25, 25)
world.setAgentStart(25, 25)

brain.initialize()
agent_brain.initialize(world)

# Create specialized components
neural_planner = pynlm.NeuralPlanner()
concept_formation = pynlm.ConceptFormation()
checkpoint_system = pynlm.CheckpointSystem()

# Run simulation
clock = pynlm.SimulationClock()
for step in range(100):
    clock.step(0.1)  # 100ms per step
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process with agent brain
    agent_brain.processSensoryInput(percept)
    
    # Get action
    action = agent_brain.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, clock.getTime())
    
    # Apply neuromodulation
    agent_brain.applyRewardModulation(0.1, 0.15)
    
    # Update development
    agent_brain.updateDevelopment(0.1)
    
    # Brain internal step
    brain.step(clock.getStep(), clock.getTime())

# Save final state
checkpoint_system.save("final_brain_state.json", brain)
```

### Example 3: Working with Memory Systems

```python
import pynlm

# Create memory systems
working_memory = pynlm.create_working_memory(capacity=1000)
episodic_memory = pynlm.create_episodic_memory(capacity=100)

# Store experiences in working memory
working_memory.store("current_state", {"position": (25, 25), "energy": 0.8})
working_memory.store("last_action", "move_forward")
working_memory.store("current_reward", 0.5)

# Retrieve and use information
current_state = working_memory.retrieve("current_state")
print(f"Agent position: {current_state['position']}")

# Store experiences in episodic memory
episodic_memory.store("episode_1", {
    "time": 1.0,
    "reward": 0.8,
    "state": "exploration",
    "action": "move_forward"
})

# Replay important experiences
episodic_memory.replay(lambda exp: print(f"Replaying: {exp['action']} at {exp['time']}s"))
```

## Error Handling

The enhanced bindings provide comprehensive error handling:

```python
try:
    # Create invalid component
    rule = pynlm.create_plasticity_rule("invalid_type")
except ValueError as e:
    print(f"Error: {e}")  # "Unknown plasticity rule type: invalid_type"
```

## Performance Features

The enhanced bindings include performance optimization features:

```python
# Memory pool for efficient allocation
memory_pool = pynlm.MemoryPool(2000)
neuron_id = memory_pool.allocateNeuron()

# Event queue for event-driven computation
event_queue = pynlm.EventQueue()
event_queue.pushEvent(some_event)

# Sparse connectivity for large networks
sparse_conn = pynlm.SparseConnectivity(1000000)
sparse_conn.addConnection(source_id, destination_id)

# Checkpoint system for persistence
checkpoint = pynlm.CheckpointSystem()
checkpoint.setAutoSave(100)  # Save every 100 steps
```

## Advanced Features

### 1. Neural Development and Aging

```python
# Create development system
dev_system = pynlm.DevelopmentSystem()
dev_system.setCurrentStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Create maturation module
maturation = pynlm.Maturation()
maturation.setMaturationRate(0.01)

# Create pruning module
pruning = pynlm.Pruning()
pruning.setPruningThreshold(0.1)
```

### 2. Cognitive Systems

```python
# Create neural planner
planner = pynlm.NeuralPlanner()
planner.setPlanningHorizon(10)  # 10 steps ahead

# Create concept formation
concept_learner = pynlm.ConceptFormation()
concept_learner.learnConcept(pattern, "exploration")

# Recognize concept
recognized = concept_learner.recognizeConcept(pattern)
```

### 3. Neuromodulation Integration

```python
# Create and configure neuromodulators
reward = pynlm.create_reward_modulator(initial_value=0.5)
curiosity = pynlm.create_curiosity_module(initial_level=0.7)
novelty = pynlm.Novelty()
novelty.setThreshold(0.3)  # Only detect novel patterns above this threshold

# Update neuromodulators
reward.update(brain, world, dt)
curiosity.update(brain, world, dt)
novelty.update(brain, world, dt)
```

## Building and Installation

The enhanced bindings require the NLM C++ framework to be built. The Python bindings use pybind11 and CMake for compilation.

```bash
# Build the NLM framework and Python bindings
mkdir build
cd build
cmake ..
make -j4

# Install (optional)
sudo make install
```

## Module Information

You can get information about the enhanced bindings using:

```python
import pynlm

# Get version info
print(pynlm.get_version())
print(pynlm.get_info())

# Get all available classes
print(pynlm.__all__)
```

## Testing

A comprehensive test suite is available to verify all functionality:

```bash
python test_improved_bindings.py
```

This test covers:
- Basic types and enums
- Configuration system
- Plasticity rules
- Memory systems
- Neuromodulation
- Brain components
- Simulation loops
- Performance features
- Advanced functionality
- Error handling

## Backward Compatibility

The enhanced bindings maintain full backward compatibility:

```python
# All original code continues to work
from pynlm import Config, Brain, AgentBrain, SimpleWorld, Vision, Audio, etc.

# Original functionality is preserved
brain = pynlm.create_brain(pynlm.create_default_config())
world = pynlm.create_simple_world()
agent_brain = pynlm.create_agent_brain(brain)
```

However, the enhanced bindings now provide access to all the missing and advanced components, making them much more powerful and complete.

## Conclusion

The enhanced NLM Python bindings represent a comprehensive improvement over the original implementation:

1. **Expanded Functionality**: Added 30+ new classes and components
2. **Advanced Features**: Full support for plasticity rules, memory systems, neuromodulation, development, cognition
3. **Performance Optimizations**: Memory pools, event queues, sparse connectivity, checkpoint systems
4. **Easy Creation**: Factory functions for creating all component types
5. **Comprehensive Testing**: Full test suite covering all functionality
6. **Backward Compatibility**: All existing code continues to work
7. **Enhanced Documentation**: Complete API documentation and examples

The enhanced bindings now provide a complete, production-ready Python interface to the NLM neural simulation framework, suitable for research, development, and production use.

---

*For the latest updates, documentation, and examples, visit the NLM project documentation.*
