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

## Advanced Usage: Complete Agent with Learning

### Advanced Example 1: Learning with Memory Systems

```python
import pynlm
import time

def run_advanced_agent_simulation(num_steps=5000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration with all systems enabled
    config = pynlm.createAdvancedConfig(
        neuronCount=3000,
        regionCount=3,
        connectionProb=0.08,
        enableAllSystems=True
    )
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print(f"Starting advanced simulation with {num_steps} steps...")
    print("Systems enabled:")
    print("  ✓ Reward modulation (dopamine learning)")
    print("  ✓ Structural plasticity (synaptogenesis/pruning)")
    print("  ✓ Development (brain maturation)")
    print("  ✓ Curiosity (exploration motivation)")
    print("  ✓ Memory systems (working/episodic)")
    
    # 7. Run simulation loop with periodic status updates
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
        
        # Get reward from world for learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 500 steps
        if step % 500 == 0:
            metrics = pynlm.getPerformanceMetrics(brain)
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
            print(f"  Working Memory: {metrics['working_memory_traces']} traces")
            print(f"  Episodic Memory: {metrics['episodic_memory_episodes']} episodes")
    
    print("\nAdvanced simulation complete!")
    return brain, agent, world

# Run the advanced simulation
brain, agent, world = run_advanced_agent_simulation(5000)

# Save brain state for later use
print("\nSaving brain state...")
pynlm.exportBrainState(brain, "advanced_brain_checkpoint.bin")
print("Brain state saved successfully!")
```

### Advanced Example 2: Performance Benchmarking

```python
import pynlm
import time

def benchmarkBrainPerformance():
    """Benchmark NLM brain performance with different configurations."""
    
    # Test 1: Minimal configuration (baseline)
    print("Benchmark Test 1: Minimal Configuration")
    config1 = pynlm.createDefaultConfig()
    config1.set("neuron_count", 500, pynlm.ConfigSource.User)
    config1.set("region_count", 1, pynlm.ConfigSource.User)
    
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    start = time.time()
    for i in range(2000):
        brain1.step(i, i * 0.001)
    duration1 = time.time() - start
    
    # Test 2: Advanced configuration (all systems)
    print("\nBenchmark Test 2: Advanced Configuration (All Systems)")
    config2 = pynlm.createAdvancedConfig(
        neuronCount=1000,
        regionCount=2,
        connectionProb=0.05,
        enableAllSystems=True
    )
    
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    start = time.time()
    for i in range(2000):
        brain2.step(i, i * 0.001)
    duration2 = time.time() - start
    
    # Test 3: High-performance configuration
    print("\nBenchmark Test 3: High-Performance Configuration")
    config3 = pynlm.createAdvancedConfig(
        neuronCount=2000,
        regionCount=3,
        connectionProb=0.08,
        enableAllSystems=True
    )
    
    brain3 = pynlm.createBrain(config3)
    brain3.initialize()
    
    start = time.time()
    for i in range(2000):
        brain3.step(i, i * 0.001)
    duration3 = time.time() - start
    
    # Display results
    print("\nBenchmark Results:")
    print("=" * 60)
    print(f"{'Config':<25} {'Neurons':<10} {'Time (ms)':<12} {'Steps/s':<12} {'Status':<15}")
    print("-" * 60)
    
    def format_ms(time_ms):
        return f"{time_ms:.1f}"
    
    def format_steps_per_sec(steps, time_ms):
        if time_ms > 0:
            return f"{steps / (time_ms/1000):.0f}"
        return "N/A"
    
    print(f"{'Minimal':<25} {config1.getOr<int64_t>('neuron_count', 500):<10} " +
          f"{format_ms(duration1 * 1000):<12} {format_steps_per_sec(2000, duration1 * 1000):<12} {'Basic':<15}")
    print(f"{'Advanced':<25} {config2.getOr<int64_t>('neuron_count', 1000):<10} " +
          f"{format_ms(duration2 * 1000):<12} {format_steps_per_sec(2000, duration2 * 1000):<12} {'All Systems':<15}")
    print(f"{'High-Performance':<25} {config3.getOr<int64_t>('neuron_count', 2000):<10} " +
          f"{format_ms(duration3 * 1000):<12} {format_steps_per_sec(2000, duration3 * 1000):<12} {'Optimized':<15}")
    
    print("\nPerformance Metrics:")
    metrics1 = pynlm.getPerformanceMetrics(brain1)
    metrics2 = pynlm.getPerformanceMetrics(brain2)
    metrics3 = pynlm.getPerformanceMetrics(brain3)
    
    print(f"\n{'Config':<15} {'Total Spikes':<15} {'Avg Firing Rate':<18} {'E/I Ratio':<12} {'Dev Stage':<12}")
    print("-" * 70)
    print(f"{'Minimal':<15} {metrics1['total_spikes']:<15} {metrics1['avg_firing_rate']:<18.3f} " +
          f"{metrics1['e_i_ratio']:<12.2f} {metrics1['developmental_stage']:<12}")
    print(f"{'Advanced':<15} {metrics2['total_spikes']:<15} {metrics2['avg_firing_rate']:<18.3f} " +
          f"{metrics2['e_i_ratio']:<12.2f} {metrics2['developmental_stage']:<12}")
    print(f"{'High-Performance':<15} {metrics3['total_spikes']:<15} {metrics3['avg_firing_rate']:<18.3f} " +
          f"{metrics3['e_i_ratio']:<12.2f} {metrics3['developmental_stage']:<12}")

# Run benchmark
benchmarkBrainPerformance()
```

### Advanced Example 3: Event-Driven Simulation with Memory Integration

```python
import pynlm
import random

def runMemoryRichSimulation():
    """Simulation that emphasizes memory integration and learning."""
    
    # Create brain with emphasis on memory systems
    config = pynlm.createAdvancedConfig(
        neuronCount=2500,
        regionCount=2,
        connectionProb=0.06,
        enableAllSystems=True
    )
    
    # Set memory-specific parameters
    config.set("memory_working_memory_capacity", 500, pynlm.ConfigSource.User)
    config.set("memory_episodic_max_episodes", 2000, pynlm.ConfigSource.User)
    config.set("replay_interval", 50, pynlm.ConfigSource.User)  # More frequent replay
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all systems for memory-rich learning
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Memory-Rich Simulation Started")
    print("Focus: Memory integration, replay, and consolidation")
    print("Expected behaviors:")
    print("  - Working memory stores recent sensory patterns")
    print("  - Episodic memory accumulates experiences")
    print("  - Replay reinforces important memories")
    print("  - Curiosity drives exploration of novel patterns")
    print("  - Development modulates learning rates")
    
    # Custom condition for early termination
    def interestingEventCondition(brain_obj):
        # Check for interesting learning events
        working_mem = pynlm.getWorkingMemory(brain_obj)
        if working_mem and working_mem.getActiveTraces() > 50:
            return True
        
        episodic_mem = pynlm.getEpisodicMemory(brain_obj)
        if episodic_mem and episodic_mem.getEpisodeCount() > 100:
            return True
        
        return False
    
    # Run event-driven simulation with memory focus
    print(f"\nRunning simulation (condition: memory threshold reached)...")
    
    for step in range(20000):  # Up to 20k steps
        # Update world with some dynamics
        if random.random() < 0.1:  # 10% chance of event
            # Create some world change to trigger learning
            # In real implementation, would call world-specific methods
            pass
        
        world.update(0.1)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Step brain
        brain.step(step)
        
        # Decode action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Check for termination condition
        if interestingEventCondition(brain) and step > 500:
            print(f"Interesting event detected at step {step} - terminating")
            break
        
        # Print status every 2000 steps
        if step % 2000 == 0:
            metrics = pynlm.getPerformanceMetrics(brain)
            print(f"Step {step}:")
            print(f"  Memory traces: {metrics.get('working_memory_traces', 0)}")
            print(f"  Episodes stored: {metrics.get('episodic_memory_episodes', 0)}")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    # Final memory analysis
    print("\n=== Final Memory Analysis ===")
    episodic_mem = pynlm.getEpisodicMemory(brain)
    if episodic_mem:
        print(f"Total episodes stored: {episodic_mem.getEpisodeCount()}")
        print(f"Replay enabled: {episodic_mem.isReplayEnabled()}")
        
        # Get some statistics
        recent_episodes = episodic_mem.getRecentEpisodes(10)
        print(f"Recent episodes (last 10): {len(recent_episodes)}")
        
        # Find average reward
        avg_reward = episodic_mem.getAverageReward()
        print(f"Average episode reward: {avg_reward:.3f}")
    
    # Save memory-rich brain
    pynlm.exportBrainState(brain, "memory_rich_brain.bin")
    print("\nMemory-rich simulation saved successfully!")

# Run memory-rich simulation
runMemoryRichSimulation()
```

---

## Quick Reference

### Basic Commands

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

### Advanced Commands

| Advanced functionality | Code |
|------------------------|------|
| Create advanced config | `pynlm.createAdvancedConfig()` |
| Get working memory | `pynlm.getWorkingMemory(brain)` |
| Get episodic memory | `pynlm.getEpisodicMemory(brain)` |
| Get prediction system | `pynlm.getPredictionSystem(brain)` |
| Get neural planner | `pynlm.getNeuralPlanner(brain)` |
| Get performance metrics | `pynlm.getPerformanceMetrics(brain)` |
| Run integration test | `pynlm.runPhase6IntegrationTest(brain, steps)` |
| Event-driven simulation | `pynlm.runEventDrivenSimulation(brain, condition)` |

### Memory System Commands

| Memory operation | Code |
|------------------|------|
| Store pattern | `workingMem.store(pattern, strength)` |
| Retrieve memory | `workingMem.retrieve()` |
| Store episode | `episodicMem.storeEpisode(episode)` |
| Get similar episodes | `episodicMem.retrieveSimilar(pattern)` |
| Replay episode | `episodicMem.replayEpisode(episode)` |
| Consolidate memory | `episodicMem.consolidate(threshold)` |

### Development Commands

| Development operation | Code |
|----------------------|------|
| Update development | `agent.updateDevelopment(timestep)` |
| Get developmental stage | `brain.getDevelopmentalStage()` |
| Set developmental stage | `brain.setDevelopmentalStage(stage)` |
| Get development system | `pynlm.getDevelopmentSystem(brain)` |

### Neuromodulation Commands

| Neuromodulation operation | Code |
|----------------------------|------|
| Get dopamine level | `brain.getDopamine().getLevel()` |
| Get curiosity level | `agent.getCuriosityLevel()` |
| Get novelty level | `agent.getNoveltyLevel()` |
| Get prediction error | `agent.getPredictionError()` |

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

**"Memory systems not working"**
- Check that all systems are enabled: `agent.enableAllSystems()`
- Make sure the brain is running enough steps for memory consolidation
- Verify that `pynlm.createAdvancedConfig()` is called with `enableAllSystems=True`

**"Performance is slow"**
- Try `pynlm.benchmarkBrain()` to identify bottlenecks
- Use smaller configuration for testing: `pynlm.createAdvancedConfig(neuronCount=500)`
- Monitor performance metrics with `pynlm.getPerformanceMetrics()`

---

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more detailed C++ integration examples
2. Read `docs/ARCHITECTURE.md` to understand how all components work together
3. Experiment with different configurations and explore the advanced API!
4. Run `pynlm.runPhase6IntegrationTest()` to test all integrated systems
5. Use `pynlm.benchmarkBrain()` to optimize performance
6. Explore the memory systems with `pynlm.getWorkingMemory()`, `pynlm.getEpisodicMemory()`, etc.
7. Try `pynlm.runEventDrivenSimulation()` for event-driven simulations
8. Use `pynlm.getPerformanceMetrics()` to monitor and optimize your brains

That's it! You're now ready to use NLM at an advanced level with full access to memory systems, prediction, development, and all the integrated cognitive capabilities.
