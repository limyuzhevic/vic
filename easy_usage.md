# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

---

## Quick Start (Copy & Paste)

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

### Example 2: Brain in a Simple World

```python
import pynlm

# Create everything
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    percept = world.getSensoryPercept()  # What does the agent see?
    agent.processSensoryInput(percept)  # Brain sees it
    brain.step(step)  # Brain thinks
    action = agent.decodeMotorCommand()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Firing neurons:", brain.getFiringNeuronCount())
```

---

## Simple Analogy

| Real World | NLM Code |
|------------|----------|
| You see something | `percept = world.getSensoryPercept()` |
| Your brain processes it | `agent.processSensoryInput(percept)` |
| You decide to act | `action = agent.decodeMotorCommand()` |
| You do the action | `world.applyMotorCommand(action, time)` |
| Your brain learns from results | `agent.applyRewardModulation(reward, predicted)` |

---

## The Simplest Possible Example

```python
import pynlm

# One line to create a virtual brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize it
brain.initialize()

# Make it active
brain.step(0)

print("Your brain has", brain.getTotalNeuronCount(), "neurons!")
```

---

## Common Patterns

### Pattern 1: Run a Simulation

```python
def run_simulation(brain, world, agent, num_steps):
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())

# Use it like this:
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Get Brain Stats

```python
# Just add these inside your loop:
print("Neurons firing:", brain.getFiringNeuronCount())
print("Total spikes:", brain.getTotalSpikeCount())
print("Avg firing rate:", brain.getAverageFiringRate())
print("Development stage:", brain.getDevelopmentalStage())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things
```

---

## What Each Part Does

### `pynlm.createBrain(config)`
Creates a virtual brain with neurons and synapses.
- `config` = settings for the brain (use `createDefaultConfig()` for simple setup)

### `brain.initialize()`
Starts up the brain. Always call this before using the brain!

### `brain.step(step_number)`
Makes the brain process one moment in time. The brain:
- Checks each neuron
- Sends signals between connected neurons
- Updates connections based on learning rules

### `world.update(time)`
Updates the virtual world by `time` seconds.

### `agent.processSensoryInput(percept)`
Gives sensory information (vision, touch, etc.) to the brain.

### `agent.decodeMotorCommand()`
Reads the brain's motor neurons to decide what action to take.

---

## Mini Projects

### Project 1: Silent Brain (Just Neurons)

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for i in range(10):
    brain.step(i)

print("Silent brain test done!")
```

### Project 2: Brain Watching a World

```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Watch the world for 30 steps
for i in range(30):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(i)
    
print("Watched world for 30 steps")
print("Firing rate:", brain.getAverageFiringRate())
```

### Project 3: Complete Agent

```python
import pynlm

# Setup
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run agent
for step in range(100):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

---

## Advanced Mini Projects

### Project 4: Learning and Forgetting

```python
import pynlm

# Create brain with enhanced configuration
config = pynlm.createAdvancedConfig()
config.set("plasticity_learning_rate", 0.05)  # Higher learning rate
config.set("neuron_count", 500)  # Smaller brain for faster testing

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
world.reset()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable advanced learning features
agent.enableStructuralPlasticity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)

print("=== Learning and Forgetting Experiment ===")
print("This experiment demonstrates learning over time with structural changes")

# Track learning over time
learning_data = []

for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    
    # Record learning metrics every 20 steps
    if step % 20 == 0:
        spike_count = brain.getTotalSpikeCount()
        firing_rate = brain.getAverageFiringRate()
        learning_progress = brain.getLearningProgress()
        
        learning_data.append({
            'step': step,
            'spike_count': spike_count,
            'firing_rate': firing_rate,
            'learning_progress': learning_progress
        })
        
        print(f"Step {step:3d}: Spikes={spike_count:4d}, "
              f"Firing Rate={firing_rate:6.2f} Hz, "
              f"Learning Progress={learning_progress:5.2f}")

print("\n=== Learning Analysis ===")
if learning_data:
    # Calculate learning statistics
    initial_learning = learning_data[0]['learning_progress']
    final_learning = learning_data[-1]['learning_progress']
    learning_improvement = final_learning - initial_learning
    
    print(f"Initial learning progress: {initial_learning:.3f}")
    print(f"Final learning progress: {final_learning:.3f}")
    print(f"Learning improvement: {learning_improvement:.3f}")
    
    if learning_improvement > 0.1:
        print("✓ Significant learning occurred")
    elif learning_improvement > 0.05:
        print("✓ Moderate learning occurred")
    else:
        print("⚠ Limited learning detected")

print("\n=== Experiment Complete ===")
```

### Project 5: Curiosity-Driven Exploration

```python
import pynlm

# Create brain with curiosity settings
config = pynlm.createAdvancedConfig()
config.set("neuron_count", 300)
config.set("plasticity_learning_rate", 0.02)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=8, height=8, visionWidth=6, visionHeight=6)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all advanced features including curiosity
agent.enableStructuralPlasticity(True)
agent.enableRewardModulation(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("=== Curiosity-Driven Exploration Experiment ===")
print("This experiment demonstrates curiosity-driven exploration and learning")

# Track exploration vs. exploitation
exploration_data = []

for step in range(300):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    
    # Get agent's decision
    action = agent.decodeMotorCommand()
    curiosity_level = agent.getCuriosityLevel()
    novelty_level = agent.getNoveltyLevel()
    
    # Determine if exploration or exploitation
    if curiosity_level > 0.5 or novelty_level > 0.3:
        behavior = "EXPLORATION"
    else:
        behavior = "EXPLOITATION"
        
    exploration_data.append({
        'step': step,
        'action': action,
        'curiosity': curiosity_level,
        'novelty': novelty_level,
        'behavior': behavior
    })
    
    if step % 50 == 0:
        print(f"Step {step:3d}: Behavior={behavior:12s}, "
              f"Curiosity={curiosity_level:5.2f}, "
              f"Novelty={novelty_level:5.2f}")

print("\n=== Exploration Analysis ===")
if exploration_data:
    # Count exploration vs. exploitation
    exploration_count = sum(1 for d in exploration_data if d['behavior'] == "EXPLORATION")
    exploitation_count = sum(1 for d in exploration_data if d['behavior'] == "EXPLOITATION")
    
    print(f"Exploration steps: {exploration_count}")
    print(f"Exploitation steps: {exploitation_count}")
    print(f"Exploration ratio: {exploration_count/len(exploration_data):.2%}")
    
    # Calculate average curiosity and novelty
    avg_curiosity = sum(d['curiosity'] for d in exploration_data) / len(exploration_data)
    avg_novelty = sum(d['novelty'] for d in exploration_data) / len(exploration_data)
    
    print(f"Average curiosity level: {avg_curiosity:.3f}")
    print(f"Average novelty level: {avg_novelty:.3f}")
    
    # Check if curiosity is driving exploration
    curiosity_driven = sum(1 for d in exploration_data 
                          if d['curiosity'] > 0.5 and d['behavior'] == "EXPLORATION")
    
    print(f"Curiosity-driven exploration: {curiosity_driven} steps")

print("\n=== Curiosity Experiment Complete ===")
```

### Project 6: Development and Maturation

```python
import pynlm

# Create brain with developmental settings
config = pynlm.createAdvancedConfig()
config.set("neuron_count", 400)
config.set("plasticity_learning_rate", 0.03)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=12, height=12, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable development
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)

print("=== Development and Maturation Experiment ===")
print("This experiment demonstrates developmental stages and maturation")

# Track developmental changes
development_data = []

for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Record developmental metrics every 50 steps
    if step % 50 == 0:
        stage = brain.getDevelopmentalStage()
        developmental_age = agent.getNeuromodulationLevel()
        plasticity = agent.getCuriosityLevel() * 0.5 + agent.getNoveltyLevel() * 0.5
        firing_rate = brain.getAverageFiringRate()
        
        development_data.append({
            'step': step,
            'stage': stage.name if hasattr(stage, 'name') else str(stage),
            'developmental_age': developmental_age,
            'plasticity': plasticity,
            'firing_rate': firing_rate
        })
        
        print(f"Step {step:3d}: Stage={stage.name if hasattr(stage, 'name') else str(stage):12s}, "
              f"Age={developmental_age:5.1f}, "
              f"Plasticity={plasticity:5.2f}, "
              f"Firing={firing_rate:6.2f} Hz")

print("\n=== Development Analysis ===")
if development_data:
    # Count developmental stages
    stages = {}
    for d in development_data:
        stage = d['stage']
        stages[stage] = stages.get(stage, 0) + 1
    
    print("Developmental stage distribution:")
    for stage, count in stages.items():
        print(f"  {stage}: {count} steps")
        
    # Check developmental progression
    final_stage = development_data[-1]['stage']
    initial_stage = development_data[0]['stage']
    
    print(f"\nDevelopmental progression: {initial_stage} -> {final_stage}")
    
    # Calculate maturation rate
    if initial_stage != final_stage:
        print("✓ Development occurred during simulation")
    else:
        print("⚠ No developmental stage change detected")

print("\n=== Development Experiment Complete ===")
```

### Project 7: Serialization and Checkpointing

```python
import pynlm
import os
import time

print("=== Serialization and Checkpointing Experiment ===")
print("This experiment demonstrates saving and loading brain state")

# Create and run brain
config = pynlm.createAdvancedConfig()
config.set("neuron_count", 200)
config.set("plasticity_learning_rate", 0.01)

brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=6, height=6, visionWidth=4, visionHeight=4)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run simulation with checkpoints
checkpoint_files = []

print("Running simulation with periodic checkpoints...")
for step in range(0, 500, 100):
    # Run simulation step
    for substep in range(100):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step + substep)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save checkpoint
    checkpoint_file = f"checkpoint_step_{step:04d}.npy"
    brain.save(checkpoint_file)
    checkpoint_files.append(checkpoint_file)
    
    print(f"Step {step:3d}: Saved checkpoint to {checkpoint_file}")

print(f"\n=== Checkpoint Analysis ===")
print(f"Total checkpoints saved: {len(checkpoint_files)}")

# Load and analyze a checkpoint
if checkpoint_files:
    checkpoint_to_load = checkpoint_files[0]
    print(f"\nLoading checkpoint from {checkpoint_to_load}")
    
    # Create new brain
    new_brain = pynlm.createBrain(pynlm.createAdvancedConfig())
    new_brain.initialize()
    
    # Load checkpoint
    load_success = new_brain.load(checkpoint_to_load)
    if load_success:
        print("✓ Checkpoint loaded successfully")
        print(f"Loaded state - Steps: {new_brain.getTotalSpikeCount()}, "
              f"Stage: {new_brain.getDevelopmentalStage()}")
    else:
        print("✗ Failed to load checkpoint")

# Clean up checkpoint files
print(f"\nCleaning up checkpoint files...")
for checkpoint_file in checkpoint_files:
    if os.path.exists(checkpoint_file):
        os.remove(checkpoint_file)
        print(f"Removed {checkpoint_file}")

print("\n=== Serialization Experiment Complete ===")
```

### Project 8: Batch Processing

```python
import pynlm

print("=== Batch Processing Experiment ===")
print("This experiment demonstrates batch simulation processing")

# Create brain
brain = pynlm.createBrain(pynlm.createAdvancedConfig())
brain.initialize()

# Create batch of sensory inputs and rewards
print("Creating batch of 50 simulations...")

inputs = []
rewards = []

for i in range(50):
    # Create random sensory percept
    percept = pynlm.SensoryPercept()
    percept.setVision([float(j % 256) / 255.0 for j in range(256)])
    percept.setTouch([float(j % 8) / 7.0 for j in range(8)])
    percept.setInternal([float(j % 4) / 3.0 for j in range(4)])
    percept.setProprioception([float(j % 6) / 5.0 for j in range(6)])
    percept.setTimestamp(i * 0.1)
    
    inputs.append(percept)
    
    # Create corresponding reward
    reward = i * 0.1  # Linearly increasing reward
    rewards.append(reward)

print(f"Created batch of {len(inputs)} simulations")
print(f"Input dimensions: Vision={inputs[0].getVisionWidth()}x{inputs[0].getVisionHeight()}, "
      f"Touch={len(inputs[0].getTouch())}, "
      f"Internal={len(inputs[0].getInternal())}, "
      f"Proprioception={len(inputs[0].getProprioception())}")
print(f"Expected reward range: {min(rewards):.2f} to {max(rewards):.2f}")

# Run batch simulation
print("\nRunning batch simulation...")
start_time = time.time()

# Note: The actual batch processing method may vary based on implementation
# This is a conceptual example
batch_results = []
for i in range(50):
    # Run individual simulation for this batch item
    brain.reset()
    brain.initialize()
    
    # Process input
    brain.receiveSensoryInput(inputs[i])
    
    # Run steps
    for step in range(20):
        brain.step(step)
    
    # Get result
    result = {
        'simulation_id': i,
        'spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'final_stage': brain.getDevelopmentalStage(),
        'expected_reward': rewards[i]
    }
    batch_results.append(result)

end_time = time.time()

print(f"Batch simulation completed in {end_time - start_time:.2f} seconds")
print(f"Average simulation time: {(end_time - start_time) / 50:.3f} seconds")

# Analyze batch results
if batch_results:
    avg_spikes = sum(r['spikes'] for r in batch_results) / len(batch_results)
    avg_firing_rate = sum(r['firing_rate'] for r in batch_results) / len(batch_results)
    success_rate = sum(1 for r in batch_results if r['firing_rate'] > 0.1) / len(batch_results)
    
    print(f"\n=== Batch Analysis ===")
    print(f"Average spikes per simulation: {avg_spikes:.1f}")
    print(f"Average firing rate: {avg_firing_rate:.2f} Hz")
    print(f"Success rate (>0.1 Hz): {success_rate:.1%}")
    print(f"Simulation efficiency: {len(batch_results) / (end_time - start_time):.1f} simulations/sec")

print("\n=== Batch Processing Experiment Complete ===")
```

---

## Advanced Configuration Examples

### 1. Performance-Optimized Configuration

```python
import pynlm

# Configuration optimized for high performance
performance_config = pynlm.createAdvancedConfig()
performance_config.set("neuron_count", 1000)  # Large brain
performance_config.set("plasticity_learning_rate", 0.01)  # Moderate learning
performance_config.set("stdp_ltp_weight", 0.02)
performance_config.set("stdp_ltd_weight", 0.022)
performance_config.set("environment_name", "GridWorld")
performance_config.set("log_level", "WARNING")  # Reduce log noise
performance_config.set("visualization_enabled", False)  # Disable visualization for performance

print("Performance-optimized configuration created")
```

### 2. Learning-Focused Configuration

```python
import pynlm

# Configuration optimized for learning
learning_config = pynlm.createAdvancedConfig()
learning_config.set("neuron_count", 500)  # Medium brain
learning_config.set("plasticity_learning_rate", 0.05)  # High learning rate
learning_config.set("stdp_ltp_weight", 0.03)  # Strong potentiation
learning_config.set("stdp_ltd_weight", 0.028)  # Strong depression
learning_config.set("synaptogenesis_rate", 0.002)  # High synaptogenesis
learning_config.set("pruning_rate", 0.0005)  # Moderate pruning
learning_config.set("environment_name", "LearningWorld")
learning_config.set("log_level", "INFO")

print("Learning-focused configuration created")
```

### 3. Memory-Constrained Configuration

```python
import pynlm

# Configuration optimized for memory usage
memory_config = pynlm.createAdvancedConfig()
memory_config.set("neuron_count", 200)  # Small brain
memory_config.set("plasticity_learning_rate", 0.02)  # Moderate learning
memory_config.set("stdp_ltp_weight", 0.015)
memory_config.set("stdp_ltd_weight", 0.018)
memory_config.set("synaptogenesis_rate", 0.0005)  # Low synaptogenesis
memory_config.set("pruning_rate", 0.0001)  # Low pruning
memory_config.set("environment_name", "MemoryWorld")
memory_config.set("log_level", "ERROR")  # Minimal logging
memory_config.set("visualization_enabled", False)  # Disable visualization

print("Memory-constrained configuration created")
```

### 4. Real-Time Configuration

```python
import pynlm

# Configuration optimized for real-time operation
realtime_config = pynlm.createAdvancedConfig()
realtime_config.set("neuron_count", 300)  # Medium brain
realtime_config.set("plasticity_learning_rate", 0.01)  # Moderate learning
realtime_config.set("simulation_timestep", 0.005)  # Faster timestep
realtime_config.set("stdp_ltp_weight", 0.02)
realtime_config.set("stdp_ltd_weight", 0.021)
realtime_config.set("environment_name", "RealTimeWorld")
realtime_config.set("log_level", "WARNING")  # Reduce log overhead

print("Real-time configuration created")
```

---

## Best Practices

### 1. Configuration Management

- **Use environment variables** for production configurations
- **Validate configurations** before use
- **Save important configurations** for reproducibility
- **Use different configurations** for different experiment types

### 2. Performance Monitoring

- **Monitor memory usage** regularly
- **Track learning progress** over time
- **Benchmark performance** periodically
- **Profile bottlenecks** when performance degrades

### 3. Experiment Design

- **Start with small brains** for testing
- **Use checkpoints** to save progress
- **Track metrics** for analysis
- **Compare different configurations**

### 4. Error Handling

- **Check configuration validity** before use
- **Handle load/save errors** gracefully
- **Monitor simulation stability**
- **Implement retry logic** for failed operations

### 5. Resource Management

- **Clean up temporary files**
- **Monitor memory usage**
- **Use efficient data structures**
- **Optimize simulation parameters**

---

## Troubleshooting Common Issues

### 1. "Brain not learning"

**Symptoms:** Brain shows no significant learning over time.

**Solutions:**
1. Check if plasticity is enabled: `agent.enableStructuralPlasticity(True)`
2. Verify learning rate is appropriate: `brain.setLearningRate(0.01)`
3. Ensure sensory input is connected: `agent.processSensoryInput(percept)`
4. Check developmental stage: `brain.getDevelopmentalStage()`
5. Enable reward modulation: `agent.enableRewardModulation(True)`

### 2. "Agent not taking actions"

**Symptoms:** Agent produces no motor commands.

**Solutions:**
1. Ensure sensory processing: `agent.processSensoryInput(percept)`
2. Check motor decoding: `agent.decodeMotorCommand()`
3. Verify brain is firing: `brain.getFiringNeuronCount()`
4. Enable curiosity: `agent.enableCuriosity(True)`

### 3. "Memory usage growing"

**Symptoms:** Memory usage increases during simulation.

**Solutions:**
1. Use checkpointing: Save and reload to free memory
2. Reduce brain size or disable features
3. Force garbage collection periodically
4. Monitor memory usage regularly

### 4. "Checkpoint loading fails"

**Symptoms:** Cannot load saved checkpoints.

**Solutions:**
1. Verify file path is correct
2. Check file format and version compatibility
3. Ensure configuration matches checkpoint
4. Verify file permissions

---

## Next Steps

When you're comfortable with the basics:

1. **Read `HOW_TO_USE.md`** for more detailed information
2. **Read `docs/ARCHITECTURE.md`** to understand how everything works
3. **Experiment with different configurations**
4. **Join the NLM community** for support and discussion
5. **Contribute to the project** by adding new features and documentation

That's it! You're now ready to use NLM at an advanced level. These advanced mini-projects will help you understand the full capabilities of NLM and how to use it effectively for complex tasks.

---

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
