# NLM Python API Documentation

## Overview

This document provides comprehensive documentation for the NLM (Neural Learning Machine) Python API. The NLM Python bindings (`pynlm`) provide access to the complete Phase 6 integrated artificial brain system.

## Module Structure

The `pynlm` module provides access to all major components of the NLM brain:

### Core Classes

- **Brain** - Central neural simulation brain class
- **Config** - Configuration management system
- **AgentBrain** - Interface connecting NLM brain to world
- **SimpleWorld** - Simple 2D world simulation

### Cognitive Classes

- **NeuralPlanner** - Multi-step action planning
- **SelfModel** - Agent's internal model of itself
- **SocialLearning** - Learning from observing other agents
- **ConceptFormation** - Pattern discovery from experience

### Sensory Classes

- **Vision** - Visual sensory input
- **Audio** - Audio sensory input
- **InternalSignals** - Internal neural signals
- **SensoryPercept** - Combined sensory percept

### Motor Classes

- **Action** - Action representation for motor output
- **AgentBody** - Agent body state

### Supporting Classes

- **NeuronId, SynapseId, RegionId, PopulationId** - Strongly-typed identifiers
- **ActionType** - Action type enumeration
- **NeuronType** - Neuron type enumeration
- **DevelopmentalStage** - Developmental stage enumeration

## Quick Start

### Installation

```bash
pip install pynlm
```

### Basic Usage

```python
import pynlm

# Create a brain with default configuration
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### Complete Agent Example

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable subsystems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")

# Run the simulation
run_agent_simulation(1000)
```

## API Reference

### Brain Class

#### Methods

- **initialize()** - Initialize the brain with configuration
- **step(step_number)** - Perform a simulation step
- **step(step_number, current_time)** - Perform step with timestamp
- **receiveSensoryInput(input)** - Inject sensory input into brain
- **injectCurrent(neuron, current)** - Inject current into specific neuron
- **injectCurrentToNeurons(type, current)** - Inject current into neurons of specific type
- **produceAction()** - Produce motor action based on neural activity
- **reset()** - Reset brain state
- **save(filepath)** - Save brain state to file
- **load(filepath)** - Load brain state from file
- **addRegion(name)** - Add a new neural region
- **getRegion(id)** - Get a region by ID
- **getRegionCount()** - Get number of regions
- **getTotalNeuronCount()** - Get total neuron count
- **getTotalSynapseCount()** - Get total synapse count
- **getFiringNeuronCount()** - Get count of currently firing neurons
- **getAverageFiringRate()** - Get average firing rate
- **getExcitationInhibitionRatio()** - Get E/I balance ratio
- **getTotalSpikeCount()** - Get total spike count
- **getDevelopmentalStage()** - Get current developmental stage
- **setDevelopmentalStage(stage)** - Set developmental stage
- **getConfig()** - Get the configuration
- **logStatus()** - Log brain status

#### Properties

- **config** - Brain configuration object
- **spike_system** - Event-driven spike processing system
- **plasticity** - Plasticity mechanisms (STDP, Hebbian)
- **working_memory** - Working memory system
- **episodic_memory** - Episodic memory system
- **prediction_system** - Prediction system
- **neuromodulation** - Neuromodulation signals

### AgentBrain Class

#### Methods

- **initialize(world)** - Initialize with world interface
- **processSensoryInput(percept)** - Process sensory percept and inject into brain
- **decodeMotorCommand()** - Decode brain motor activity into motor command
- **applyRewardModulation(reward, predicted_reward)** - Apply reward-based neuromodulation
- **updateDevelopment(timestep)** - Update development system
- **getThoughtTrace()** - Get formatted thought trace
- **getLearnedPreference()** - Get learned preference trait
- **reset()** - Reset agent for new episode

#### Configuration Methods

- **enableRewardModulation(enable)** - Enable/disable reward modulation
- **enableStructuralPlasticity(enable)** - Enable/disable structural plasticity
- **enableDevelopment(enable)** - Enable/disable development
- **enableCuriosity(enable)** - Enable/disable curiosity

### NeuralPlanner Class

#### Methods

- **planAction(currentState, targetReward)** - Plan next action given state and goal
- **evaluateSequence(actions, startState)** - Evaluate potential action sequence
- **setPlanningDepth(depth)** - Set number of steps to look ahead
- **getPlanningConfidence()** - Get planning confidence
- **setActionQuality(action, quality)** - Set action quality from experience
- **getActionQuality(action)** - Get action quality learned from experience

### SelfModel Class

#### Methods

- **recordSelfAction(action, beforeState, afterState)** - Record sensory changes from own action
- **predictActionConsequence(action, currentState)** - Predict sensory consequence (forward model)
- **getSelfModelConfidence(action)** - Get confidence in self-model for action
- **updateSelfModel(predicted, actual, action)** - Update based on prediction error
- **getCapabilityLevel()** - Get current capability level
- **getBodyAwareness()** - Get how accurately can predict consequences

### SocialLearning Class

#### Methods

- **observeAgentAction(observedAction, observerState, resultingState)** - Record observation of other agent's action
- **canImitate(observedAction)** - Can imitate this observed action?
- **getImitationActionAndSimilarity(currentState)** - Get best imitation with similarity
- **learnCommunicationSignal(signalPattern, signalReward)** - Learn communication signal
- **detectSignal(neuralPattern)** - Detect if agent is signaling
- **getSignalPattern()** - Get learned signal pattern
- **getSignalMeaning()** - Get signal meaning (associated reward)

### ConceptFormation Class

#### Methods

- **presentExperience(pattern, features, reward, currentTime)** - Present experience to concept system
- **getMatchingConcept(pattern)** - Get concept that best matches pattern
- **getConcept(conceptId)** - Get specific concept by ID

### SimpleWorld Class

#### Methods

- **configure(width, height, visionWidth, visionHeight)** - Configure world dimensions
- **reset()** - Reset world to initial state
- **setAgentStart(x, y)** - Set agent starting position
- **update(timestep)** - Update the world
- **applyMotorCommand(cmd, currentTime)** - Apply motor command to world
- **getSensoryPercept()** - Get sensory percept from world
- **getAgentBody()** - Get agent body state

### Utility Functions

#### Factory Functions

- **createDefaultConfig()** - Create default configuration
- **createBrain(config)** - Create new brain with configuration
- **createSimpleWorld()** - Create new simple world
- **createAgentBrain(brain)** - Create agent brain interface

#### Convenience Functions

- **runAgentSimulation(brain, world, agent, num_steps)** - Run complete agent simulation
- **createCompleteAgentSetup(config, worldWidth, worldHeight, visionWidth, visionHeight)** - Create complete agent setup

### Type Enumerations

#### Neuron Types
- **NeuronType.Excitatory** - Excitatory neuron
- **NeuronType.Inhibitory** - Inhibitory neuron
- **NeuronType.Modulatory** - Modulatory neuron
- **NeuronType.Sensory** - Sensory neuron
- **NeuronType.Motor** - Motor neuron
- **NeuronType.Internal** - Internal neuron

#### Action Types
- **ActionType.MoveForward** - Move forward
- **ActionType.MoveBackward** - Move backward
- Kapolres
- **ActionType.TurnLeft** - Turn left
- **ActionType.TurnRight** - Turn right
- **ActionType.Look** - Look
- **ActionType.LookUp** - Look up
- **ActionType.LookDown** - Look down
- **ActionType.Interact** - Interact
- **ActionType.Eat** - Eat
- **ActionType.Drink** - Drink
- **ActionType.Rest** - Rest
- **ActionType.Wait** - Wait
- **ActionType.Custom** - Custom action

#### Developmental Stages
- **DevelopmentalStage.Initial** - Initial stage
- **DevelopmentalStage.CriticalPeriod** - Critical period
- **DevelopmentalStage.Maturation** - Maturation
- **DevelopmentalStage.Adult** - Adult stage
- **DevelopmentalStage.Aging** - Aging stage

## Configuration Options

### Brain Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.connection_probability` | float | 0.05 | Probability of connection between neurons |
| `brain.initial_weight_mean` | float | 0.5 | Mean initial synaptic weight |
| `brain.initial_weight_std` | float | 0.1 | Standard deviation of initial weights |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential |
| `brain.v_rest` | float | -70.0 | Resting potential |
| `brain.v_reset` | float | -75.0 | Reset potential after spike |
| `brain.tau_mem` | float | 20.0 | Membrane time constant |
| `brain.tau_ref` | float | 2.0 | Refractory period |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.stdp.tau_plus` | float | 20.0 | STDP time constant |
| `plasticity.stdp.tau_minus` | float | 20.0 | STDP time constant |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Best Practices

### 1. Initialize Before Use

```python
brain = pynlm.createBrain(config)
brain.initialize()  # Never skip this!
```

### 2. Use Context Managers (Future)

```python
# When context managers are supported
with pynlm.createBrain(config) as brain:
    brain.initialize()
    for step in range(100):
        brain.step(step)
```

### 3. Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    for step in range(1000):
        brain.step(step)
except pynlm.RuntimeError as e:
    print(f"Runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

### 4. Memory Management

```python
# Brain and agent objects are automatically managed
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)

# No need to manually delete - Python's GC handles it
```

### 5. Performance Optimization

```python
# Configure for performance
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.001)
```

### 6. State Persistence

```python
# Save brain state for later restoration
brain.save("checkpoint.bin")

# Later, restore brain state
brain.load("checkpoint.bin")
```

## Examples Directory

The NLM package includes an `examples/` directory with practical examples:

- **hello_world.py** - Basic brain simulation
- **agent_example.py** - Complete agent with world interaction
- **cognitive_example.py** - Advanced cognitive functions
- **memory_example.py** - Working with memory systems
- **plasticity_example.py** - Demonstrating learning mechanisms

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"
**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"
**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"ImportError: No module named 'pynlm'"
**
```bash
pip install --force-reinstall pynlm
```

**"Segmentation fault"
**
- Check that `initialize()` is called before `step()`
- Ensure `reset()` is called before re-running simulation

### Debugging Tips

```python
# Enable verbose logging
config.set("log_level", "DEBUG")

# Get detailed status
brain.logStatus()

# Monitor key metrics
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio()}")
```

## Migration Guide

### From NLM 0.0.0 to NLM 2.0.0 (Enhanced)

1. **New Classes Added**: SelfModel, SocialLearning, NeuralPlanner, ConceptFormation
2. **Enhanced AgentBrain**: More comprehensive cognitive integration
3. **Better Error Handling**: More descriptive error messages
4. **Documentation**: Comprehensive docstrings and examples
5. **API Consistency**: More uniform naming conventions

## License

MIT License. See the LICENSE file for details.

## Authors

NLM Research Team. This is an experimental artificial developmental brain project.

## Acknowledgments

- Thanks to all NLM contributors for their work on the C++ core
- Special thanks to the pybind11 team for enabling Python bindings
- Appreciation for the open-source community