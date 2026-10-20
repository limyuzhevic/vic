# NLM - Enhanced Usage Guide for Beginners and Advanced Users

## Quick Reference for All Users

**NLM (Neural Learning Machine)** is a **brain simulator** for computers that implements:
- Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
- Multiple plasticity rules (STDP, Hebbian, structural)
- Working memory, episodic memory, and associative memory systems
- Neuromodulation (dopamine, curiosity, novelty detection)
- Prediction and cognition systems
- Developmental stages and lifespan learning

**Key Components:**

### 1. **Brain** - The virtual brain that thinks
- Contains neurons, synapses, and neural regions
- Implements real spiking neural computation
- Supports learning through plasticity

### 2. **World** - The environment the brain lives in
- Simple 2D physics simulation
- Provides sensory input and reward signals
- Handles agent actions and consequences

### 3. **Agent** - The bridge connecting brain to world
- Processes sensory input from the world
- Decodes neural activity into motor commands
- Applies neuromodulation based on rewards

---

## Beginner Examples

### Example 1: Simplest Brain

```python
import pynlm

# Step 1: Make a brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Basic Agent Simulation (NEW!)

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning systems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
```

---

## Advanced Usage Examples

### Example 3: Checkpoint System (NEW!)

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Simulate for a while
for step in range(500):
    brain.step(step)
    if step % 100 == 0:
        print(f"Step {step}")

# Save checkpoint
print("Saving checkpoint...")
brain.save("my_brain_checkpoint.bin")

# Continue later or create new brain from checkpoint
brain2 = pynlm.createBrain(pynlm.createDefaultConfig())
brain2.load("my_brain_checkpoint.bin")

print("Brain restored from checkpoint!")
print(f"Loaded brain has {brain2.getTotalNeuronCount()} neurons")
print(f"Loaded brain has {brain2.getTotalSynapseCount()} synapses")
```

### Example 4: Memory Integration (NEW!)

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check memory systems
print("Memory System Status:")
print(f"Working memory: {'Available' if brain.getWorkingMemory() else 'Not available'}")
print(f"Episodic memory: {'Available' if brain.getEpisodicMemory() else 'Not available'}")
print(f"Associative memory: {'Available' if brain.getAssociativeMemory() else 'Not available'}")

# Run simulation to populate memory
for step in range(200):
    brain.step(step, step * 0.001)

# Check memory statistics if available
wm = brain.getWorkingMemory()
if wm:
    print(f"Working memory traces: {wm.getActiveTraces()}")

em = brain.getEpisodicMemory()
if em:
    print(f"Episodic memory episodes: {em.getEpisodeCount()}")
```

### Example 5: Neuromodulation Systems (NEW!)

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create agent with world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)

# Run simulation with neuromodulation
for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step, step * 0.001)
    
    # Get action and apply
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Calculate reward based on action result
    reward = 0.1 if action.type == pynlm.ActionType.Interact else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)

# Print neuromodulation statistics
print("Neuromodulation Statistics:")
print(f"Dopamine level: {agent.getNeuromodulationLevel():.3f}")
print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Prediction error: {agent.getPredictionError():.3f}")
```

### Example 6: Performance Benchmarking (NEW!)

```python
import pynlm
import time

# Benchmark different brain sizes
brain_sizes = [500, 1000, 2000, 5000]
results = []

for size in brain_sizes:
    print(f"\nBenchmarking brain with {size} neurons...")
    
    # Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", size)
    config.set("brain.synapse_density", 0.1)
    
    brain = pynlm.createBrain(config)
    
    # Time initialization
    start_time = time.time()
    brain.initialize()
    init_time = time.time() - start_time
    
    # Time simulation
    start_time = time.time()
    for step in range(200):
        brain.step(step, step * 0.001)
    sim_time = time.time() - start_time
    
    # Calculate statistics
    neurons_per_sec = size / init_time if init_time > 0 else 0
    steps_per_sec = 200 / sim_time if sim_time > 0 else 0
    firing_rate = brain.getAverageFiringRate()
    
    result = {
        'size': size,
        'init_time': init_time,
        'sim_time': sim_time,
        'neurons_per_sec': neurons_per_sec,
        'steps_per_sec': steps_per_sec,
        'firing_rate': firing_rate
    }
    results.append(result)
    
    print(f"  Initialization: {init_time:.3f}s ({neurons_per_sec:.0f} neurons/sec)")
    print(f"  Simulation: {sim_time:.3f}s ({steps_per_sec:.0f} steps/sec)")
    print(f"  Average firing rate: {firing_rate:.2f} Hz")

# Print summary
print("\n" + "="*50)
print("PERFORMANCE BENCHMARK SUMMARY")
print("="*50)
print(f"{'Size':<10} {'Init Time':<12} {'Sim Time':<12} {'Neurons/sec':<12} {'Steps/sec':<12} {'Firing Rate':<12}")
print("-"*80)
for r in results:
    print(f"{r['size']:<10} {r['init_time']:<12.3f} {r['sim_time']:<12.3f} "
          f"{r['neurons_per_sec']:<12.0f} {r['steps_per_sec']:<12.0f} "
          f"{r['firing_rate']:<12.2f}")
```

---

## Advanced Configuration Options

### Custom Brain Configuration

```python
import pynlm

# Create custom configuration
custom_config = pynlm.createDefaultConfig()

# Modify specific parameters
custom_config.set("brain.neuron_count", 5000)  # More neurons
custom_config.set("brain.synapse_density", 0.2)  # More connections
custom_config.set("plasticity.stdp.enable", True)  # Enable STDP
custom_config.set("plasticity.hebbian.enable", True)  # Enable Hebbian
custom_config.set("plasticity.structural.enable", True)  # Enable structural plasticity
custom_config.set("neuromod.dopamine.scale", 1.0)  # Normal dopamine
custom_config.set("neuromod.curiosity.enable", True)  # Enable curiosity
custom_config.set("neuromod.novelty.enable", True)  # Enable novelty detection

# Create brain with custom config
brain = pynlm.createBrain(custom_config)
brain.initialize()
```

### World Configuration

```python
import pynlm

# Create world with custom parameters
world = pynlm.createSimpleWorld()

# Configure world dimensions and sensory capabilities
world.configure(
    width=50,      # World width
    height=50,     # World height  
    visionWidth=16,  # Vision width
    visionHeight=16  # Vision height
)

# Set energy and decay parameters
world.setMaxEnergy(200.0)
world.setEnergyDecayRate(0.005)

# Set random seed for reproducible results
world.setRandomSeed(12345)
world.reset()
```

---

## Error Handling and Validation

NLM provides robust error handling and validation:

### Configuration Validation

```python
import pynlm

# This will throw an exception if config is invalid
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Check if configuration is valid
if config.has("brain.neuron_count"):
    print(f"Brain will have {config.get('brain.neuron_count')} neurons")
```

### Agent Validation

```python
import pynlm

# Create components
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# This will throw an exception if any component is invalid
agent.initialize(world)  # Must be called after brain is initialized

# Check agent status
if agent.isRewardModulationEnabled():
    print("Reward modulation is enabled")
```

### Simulation Validation

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Validate brain before running simulation
if brain.getTotalNeuronCount() > 0 and brain.getTotalSynapseCount() > 0:
    print("Brain is properly initialized")
    
    # Run simulation
    for step in range(100):
        brain.step(step)
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
else:
    print("Error: Brain not properly initialized")
```

---

## Advanced Features

### Developmental Stages

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check developmental stage
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Simulate development
for step in range(1000):
    brain.step(step, step * 0.001)
    
    # Progress through developmental stages
    if step % 200 == 0:
        print(f"Development progress: {step}/1000 steps")
```

### Memory Systems

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check memory systems
wm = brain.getWorkingMemory()
em = brain.getEpisodicMemory()
am = brain.getAssociativeMemory()

print(f"Working memory traces: {wm.getActiveTraces() if wm else 0}")
print(f"Episodic memory episodes: {em.getEpisodeCount() if em else 0}")
print(f"Associative memory associations: {am.getAssociationCount() if am else 0}")
```

### Prediction System

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Check prediction system
prediction_system = brain.getPredictionSystem()
if prediction_system:
    print(f"Prediction error: {prediction_system.getPredictionError():.3f}")
```

---

## Performance Tips

### 1. **Use Larger Timesteps**
```python
# For faster simulation, use larger timesteps
for step in range(1000):
    brain.step(step, step * 0.1)  # 0.1s timesteps instead of 0.001s
```

### 2. **Pre-allocate Memory**
```python
# Create brain with expected size upfront
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
brain = pynlm.createBrain(config)
brain.initialize()  # No reallocation needed
```

### 3. **Use Checkpointing**
```python
# Save checkpoints for long simulations
for step in range(10000):
    brain.step(step)
    
    if step % 1000 == 0:
        brain.save(f"checkpoint_step_{step}.bin")
        print(f"Checkpoint saved at step {step}")
```

### 4. **Enable Performance Optimizations**
```python
# Enable all performance features
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)
brain = pynlm.createBrain(config)
brain.initialize()  # Uses memory pools and SIMD optimizations
```

---

## Troubleshooting

### Common Issues

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if configuration parameters are valid

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify agent is properly initialized

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check that all components are properly initialized

### Debug Information

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Print brain status for debugging
brain.logStatus()

# Check all critical systems
print("\nSystem Health Check:")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Synapses: {brain.getTotalSynapseCount()}")
print(f"Active neurons: {brain.getActiveNeuronCount()}")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

---

## Next Steps

1. **Read `HOW_TO_USE.md`** for more detailed information
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations!**

That's it! You're now ready to use NLM effectively. The enhanced version includes powerful new features while maintaining the simple, beginner-friendly API you need to get started.

**Happy Neural Simulation!** 🎯