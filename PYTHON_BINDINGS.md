# NLM Python Bindings - Comprehensive Documentation

## Overview

NLM (熙然) is an experimental computational brain project that simulates neural systems. The Python bindings (`pynlm`) provide full access to all NLM capabilities through an intuitive Python API.

## Installation and Setup

```bash
# Build the project
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings
pip install -e .
```

## Quick Start Examples

### 1. Basic Brain Creation
```python
import pynlm

# Create a basic brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run a few simulation steps
for step in range(100):
    brain.step(step)

print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. Complete Agent-World Simulation
```python
import pynlm

# Create the complete setup
brain, world, agent = pynlm.createLearningSetup(
    enableCuriosity=True,
    enableRewardModulation=True,
    enableStructuralPlasticity=False
)

# Run simulation
print("Starting simulation...")
agent, world, pynlm.runSimulationSteps(agent, world, 500, showProgress=True)

# Print comprehensive statistics
stats = pynlm.formatSimulationStats(agent, world)
print(stats)
```

### 3. Working Memory Example
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get working memory system
working_memory = brain.getWorkingMemory()
if working_memory:
    # Store some information
    neuron_id = brain.getRegion(0)->getAllNeurons()[0]->getId()
    working_memory.store(neuron_id, 0.75)
    
    # Retrieve information
    value = working_memory.retrieve(neuron_id)
    print(f"Working memory value: {value}")
```

### 4. Phase 6 Advanced Features
```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(20.0f, 20.0f, 16, 16)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning
agent.enableCuriosity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Simulation with advanced features
for step in range(200):
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply learning
    reward = 1.0 if world.getAgentBody().energy > 0 else -0.1
    agent.applyRewardModulation(reward, agent.getPredictionError())
```

## Complete API Reference

### Core Classes

#### Brain
```python
brain = pynlm.createBrain(config)
brain.initialize()
brain.step(step)  # Basic step
brain.step(step, time)  # Step with timestamp
brain.produceAction()  # Get motor action
brain.save("brain.bin")  # Save checkpoint
brain.load("brain.bin")  # Load checkpoint
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getAverageFiringRate()
brain.logStatus()  # Print detailed stats
```

#### Config
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 100)
config.set("connection_probability", 0.1)
config.get("neuron_count")
config.has("connection_probability")
config.getKeys()
```

#### AgentBrain
```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.processSensoryInput(percept)
action = agent.decodeMotorCommand()
agent.applyRewardModulation(reward, predicted)
agent.getThoughtTrace()
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
```

#### SimpleWorld
```python
world = pynlm.createSimpleWorld()
world.configure(width, height, vision_width, vision_height)
world.reset()
world.update(timestep)
percept = world.getSensoryPercept()
action_result = world.applyMotorCommand(action, time)
world.placeResource()  # Convenience method
world.placeHazard()    # Convenience method
world.removeObjectAt(x, y)
world.isOccupied(x, y)
```

### Phase 6 Advanced Systems

#### Working Memory
```python
working_memory = brain.getWorkingMemory()
if working_memory:
    working_memory.store(neuron_id, value)
    retrieved_value = working_memory.retrieve(neuron_id)
    is_stored = working_memory.contains(neuron_id)
    working_memory.getCapacity()
    working_memory.getCurrentSize()
```

#### Episodic Memory
```python
episodic_memory = brain.getEpisodicMemory()
if episodic_memory:
    # Store episode (implementation-specific)
    # Retrieve episodes
    recent = episodic_memory.getRecentEpisodes(count)
    episodic_memory.getEpisodeCount()
```

#### Concept Formation
```python
concept_formation = brain.getConceptFormation()
if concept_formation:
    # Present experience patterns
    # Get matching concepts
    concept_count = concept_formation.getConceptCount()
```

#### Neural Planner
```python
planner = brain.getPlanner()
if planner:
    action = planner.planAction(state, target_reward)
    planner.getPlanningDepth()
    planner.getPlanningConfidence()
```

#### Neuromodulation Systems
```python
# Get neuromodulation signals
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionError()
dopamine_level = agent.getNeuromodulationLevel()
curiosity_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
```

#### Development System
```python
agent.enableDevelopment(True)
development_stage = agent.getDevelopmentalStage()
agent.updateDevelopment(timestep)
```

### Utility Functions

#### Text Encoding (Phase 6)
```python
# Static text encoding
text_pattern = pynlm.encodeText("hello")

# Percept text access
percept_text = percept.getText()
if agent:
    encoded = agent.encodeText(percept_text)
```

#### Convenience Factory Functions
```python
# Example setups
brain, world, agent = pynlm.createExampleSetup()
brain, world, agent = pynlm.createLearningSetup(curiosity=True, reward=True)
brain, world, agent = pynlm.createQuickTestSetup()

# Simulation utility
pynlm.runSimulationSteps(agent, world, 100, showProgress=True)

# Statistics formatting
stats = pynlm.formatSimulationStats(agent, world)
print(stats)
```

#### Sensory Input Creation
```python
# Create various sensory inputs
vision = pynlm.createVisionInput(16, 16, 3, vision_data)
audio = pynlm.createAudioInput(44100, 1024, audio_data)
internal = pynlm.createInternalSignalsInput(signals_data)
```

#### Motor Command Creation
```python
# Create motor commands with ease
move_forward = pynlm.createMoveCommand("forward", 0.8)
interact_action = pynlm.createMoveCommand("interact")
```

## Configuration Examples

### Basic Configuration
```python
config = pynlm.createConfigWithDefaults(neuronCount=200, regionCount=3, connectionProb=0.15)
```

### Advanced Configuration
```python
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 5)
config.set("connection_probability", 0.2)
config.set("random_seed", 42)
```

## Error Handling

```python
import pynlm

try:
    brain = pynlm.createBrain(config)
    brain.initialize()
except Exception as e:
    print(f"Error creating brain: {e}")

try:
    result = world.applyMotorCommand(action, time)
    if not result.success:
        print(f"Action failed: {result.message}")
except pynlm.RuntimeError as e:
    print(f"NLM runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
```

## Best Practices

1. **Always initialize your brain**: Call `brain.initialize()` before using it
2. **Use configuration objects**: Create config objects and modify them before creating brains
3. **Enable learning features early**: Enable curiosity, reward modulation, etc. before running simulations
4. **Use the example setups**: Start with `createExampleSetup()` for testing
5. **Monitor statistics**: Use `formatSimulationStats()` to track system health
6. **Handle errors gracefully**: Always wrap NLM operations in try-catch blocks
7. **Use Phase 6 features**: Enable advanced systems for full functionality

## Advanced Usage Examples

### Full Brain Integration
```python
import pynlm
import numpy as np

# Create learning setup
brain, world, agent = pynlm.createLearningSetup(
    enableCuriosity=True,
    enableRewardModulation=True,
    enableDevelopment=True,
    enableStructuralPlasticity=True
)

# Create custom resources
for i in range(5):
    x = 5.0 + i * 3.0
    y = 5.0 + i * 3.0
    resource = pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, 20.0f, 0.3f)
    world.addObject(resource)

# Run simulation
for step in range(1000):
    world.update(0.1)
    
    # Get current perception
    percept = world.getSensoryPercept()
    
    # Process with agent
    agent.processSensoryInput(percept)
    brain.step(step)
    
    # Act
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from outcome
    reward = result.reward if result.success else -0.5
    predicted = agent.getPredictionError()
    agent.applyRewardModulation(reward, predicted)
    
    # Show progress
    if step % 100 == 0:
        print(f"Step {step}: Energy={world.getAgentBody().energy:.1f}, "
              f"Health={world.getAgentBody().health:.1f}")
        
# Final statistics
stats = pynlm.formatSimulationStats(agent, world)
print("\n=== FINAL SIMULATION STATS ===")
print(stats)

# Save brain state
if agent.getBrain():
    agent.getBrain().save("final_brain.bin")
    print("\nBrain saved to final_brain.bin")
```

### Phase 6 Text Learning
```python
import pynlm

# Setup
brain, world, agent = pynlm.createExampleSetup()

# Create text percept
class TextPercept:
    def __init__(self, text):
        self.text = text
        self.vision = []
        
    def getText(self):
        return self.text

# Simulate text learning
text_words = ["food", "water", "danger", "home", "friend"]
for step, word in enumerate(text_words):
    world.update(0.1)
    
    # Create text percept
    text_percept = TextPercept(word)
    text_percept.vision = [0.1] * 16  # Some basic vision
    
    # Process text
    agent.processSensoryInput(text_percept)
    brain.step(step)
    
    print(f"Step {step}: Encoded text '{word}' -> "
          f"pattern {[i-16 for i in agent.encodeText(word)]}")
```

## Troubleshooting

### Common Issues and Solutions

1. **Brain doesn't fire neurons**:
   ```python
   # Solution: Ensure sensory input is connected
   brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0f)
   ```

2. **Agent doesn't move**:
   ```python
   # Solution: Check world collision and agent energy
   if world.getAgentBody().energy <= 0:
       print("Agent out of energy!")
   ```

3. **Simulation crashes**:
   ```python
   # Solution: Add error handling and check null pointers
   try:
       result = world.applyMotorCommand(action, time)
   except Exception as e:
       print(f"Error in simulation: {e}")
   ```

4. **Memory usage high**:
   ```python
   # Solution: Use simpler configurations
   brain, world, agent = pynlm.createQuickTestSetup()
   ```

## Performance Tips

1. **Use fewer neurons for testing**: QuickTestSetup is optimized for testing
2. **Batch process**: Process multiple steps at once when possible
3. **Monitor memory usage**: Use built-in statistics to track system health
4. **Save checkpoints**: Regularly save brain state to resume simulations
5. **Limit simulation length**: Start with shorter simulations for debugging

## Migration Guide

### From Basic to Advanced Usage

**Basic (current):**
```python
import pynlm
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
brain.step(0)
```

**Advanced (recommended):**
```python
import pynlm
brain, world, agent = pynlm.createLearningSetup()
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

## Future Enhancements

The Python bindings are continuously being improved. Future enhancements may include:

1. **Web-based visualization**: Real-time brain simulation visualization
2. **Parallel processing**: Multi-threaded simulation capabilities
3. **Remote execution**: Run simulations on remote servers
4. **Cloud integration**: Save and share simulation results
5. **Enhanced API**: More Pythonic interfaces and convenience methods

## License

This project is licensed under the MIT License.
