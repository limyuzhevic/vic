# NLM Phase 6 Python Bindings Usage

This document provides examples of using the NLM Phase 6 integration with Python bindings.

## Installation

```bash
# Build the C++ library first
cd /path/to/nlm
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Install Python bindings (if using setuptools/pip)
# Or simply use the compiled library directly
```

## Basic Usage Examples

### Example 1: Simple Brain Simulation

```python
import pynlm

# Create a brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run the brain for 100 steps
for i in range(100):
    brain.step(i)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f}")
```

### Example 2: Complete Agent Simulation

```python
import pynlm

# Initialize brain and world
config = pynlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 1)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run simulation
for step in range(1000):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: Firing neurons = {brain.getFiringNeuronCount()}, "
              f"Curiosity = {agent.getCuriosityLevel():.3f}, "
              f"Novelty = {agent.getNoveltyLevel():.3f}")

print("Agent simulation complete!")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
```

### Example 3: Phase 6 Integration Demo

```python
import pynlm

# Create experiment configuration
class Phase6Config:
    def __init__(self):
        self.neuronCount = 200
        self.maxSteps = 500
        self.enableCheckpointing = False
        self.enableReplay = False
        self.enableDevelopment = True

# Note: The full Phase 6 integration is available in the C++ demo
# This shows the key components used in Phase 6

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=16, height=16, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all Phase 6 systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Run integration test
for step in range(config.maxSteps):
    world.update(0.1)
    
    # Phase 6 systems work together
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)  # Sensory processing
    
    brain.step(step)  # Neural processing
    
    action = agent.decodeMotorCommand()  # Decision making
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Neuromodulation affects learning
    reward = world.computeReward(brain.getRegions()[0].get())
    agent.applyRewardModulation(reward, 0.0)

print("Phase 6 integration test completed!")
print(f"Brain integration: Working memory active = {brain.getWorkingMemory() is not None}")
print(f"Memory integration: Episodic memory = {brain.getEpisodicMemory() is not None}")
print(f"Neuromodulation: Dopamine system = {brain.getDopamine() is not None}")
```

### Example 4: Memory Systems Demonstration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check memory systems
print("Memory System Status:")
print(f"  Working Memory: {'Active' if brain.getWorkingMemory() else 'Inactive'}")
print(f"  Episodic Memory: {'Active' if brain.getEpisodicMemory() else 'Inactive'}")
print(f"  Semantic Memory: {'Active' if brain.getSemanticMemory() else 'Inactive'}")

# Run simulation to populate memory
for i in range(200):
    brain.step(i)
    
    # Inject some activity to strengthen memory traces
    if i % 10 == 0:
        brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 10.0)

print(f"\nAfter simulation:")
if brain.getWorkingMemory():
    print(f"  Working Memory active traces: {brain.getWorkingMemory().getActiveTraces()}")
if brain.getEpisodicMemory():
    print(f"  Episodic Memory episodes: {brain.getEpisodicMemory().getEpisodeCount()}")

# Show development progress
print(f"  Developmental stage: {brain.getDevelopmentalStage()}")
```

### Example 5: Neuromodulation Demonstration

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check neuromodulation systems
print("Neuromodulation System Status:")
print(f"  Dopamine: {'Active' if brain.getDopamine() else 'Inactive'}")
print(f"  Curiosity: {'Active' if brain.getCuriosity() else 'Inactive'}")
print(f"  Novelty: {'Active' if brain.getNovelty() else 'Inactive'}")

# Create agent with neuromodulation
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation with reward learning
for step in range(300):
    world.update(0.1)
    
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    brain.step(step)
    
    # Simulate reward-based learning
    if step % 50 == 0:
        reward = 1.0 if (step // 50) % 2 == 0 else 0.0
        agent.applyRewardModulation(reward, 0.5)
    
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())

print(f"\nNeuromodulation levels:")
print(f"  Dopamine: {agent.getNeuromodulationLevel():.3f}")
print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
```

## Advanced Usage

### Direct Component Access

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Access specific brain regions
regions = brain.getRegions()
for region in regions:
    print(f"Region neurons: {region.getTotalNeuronCount()}")
    print(f"Region populations: {region.getPopulationCount()}")

# Get neural statistics
total_neurons = brain.getTotalNeuronCount()
active_neurons = brain.getActiveNeuronCount()
firing_rate = brain.getAverageFiringRate()

print(f"\nNeural Statistics:")
print(f"  Total neurons: {total_neurons}")
print(f"  Active neurons: {active_neurons}")
print(f"  Firing rate: {firing_rate:.2f} Hz")
```

## Configuration Options

```python
import pynlm

config = pynlm.createDefaultConfig()

# Modify configuration parameters
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.2f)
config.set("stdp_ltp_weight", 0.02f)
config.set("stdp_ltd_weight", 0.018f)
config.set("synaptogenesis_rate", 0.0001)
config.set("pruning_rate", 0.00001)

# Save configuration
config.saveToFile("custom_config.json")
```

## Error Handling

```python
try:
    brain = pynlm.createBrain(config)
    brain.initialize()
    print("Brain initialized successfully")
except Exception as e:
    print(f"Error initializing brain: {e}")
    # Fallback to simpler configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
```

## Key Methods to Remember

### Brain Class
- `initialize()`: Start the brain simulation
- `step(step)`: Advance simulation by one step
- `injectCurrent(neuron, current)`: Add neural input
- `produceAction()`: Generate motor output
- `save(filepath)`: Save brain state
- `load(filepath)`: Load brain state

### AgentBrain Class
- `processSensoryInput(percept)`: Process sensory information
- `decodeMotorCommand()`: Convert neural activity to actions
- `applyRewardModulation(reward, predicted)`: Learn from rewards
- `updateDevelopment(timestep)`: Track brain development

### SimpleWorld Class
- `update(dt)`: Advance world simulation
- `applyMotorCommand(cmd, time)`: Execute actions
- `getSensoryPercept()`: Get current sensory input
- `computeReward(brain)`: Calculate reward signals

## Performance Tips

1. **Batch operations**: Process multiple steps before checking status
2. **Memory management**: Save brain state periodically to avoid memory buildup
3. **Configuration tuning**: Adjust neuron count and connection probability for desired performance
4. **Parallel processing**: Multiple brains can run in parallel for testing

## Troubleshooting

### "Brain not doing anything"
- Ensure `brain.initialize()` was called
- Check that `brain.step()` is being called
- Verify configuration has appropriate neuron counts

### "Agent not moving"
- Ensure `world.update()` is called before `decodeMotorCommand()`
- Check that `world.applyMotorCommand()` is called
- Verify motor neurons are properly connected

### "Memory not working"
- Ensure memory systems are enabled
- Check that brain has been running long enough
- Verify configuration supports memory features

## Next Steps

1. **Read C++ documentation**: `docs/ARCHITECTURE.md` for technical details
2. **Run Phase 6 demo**: Use `nlm_phase6_demo` binary for full integration test
3. **Experiment with configurations**: Try different neuron counts and connection probabilities
4. **Add visualization**: Use `nlm_phase6_demo` or create custom visualization tools
5. **Explore advanced features**: Prediction systems, neuromodulation, structural plasticity

## Resources

- **Documentation**: `docs/` directory contains detailed technical documentation
- **Phase 6 Demo**: `src/experiments/Phase6Demo.cpp` shows complete integration
- **API Reference**: Generated from Doxygen comments in source code
- **Examples**: More examples in `examples/` directory

The Python bindings provide a convenient interface to NLM's powerful neural simulation capabilities while maintaining access to all the advanced Phase 6 features.