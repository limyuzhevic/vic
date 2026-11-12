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

## Advanced Examples for Power Users

### 1. Custom Configuration with Advanced Settings

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Optimize for learning
config.set("brain.neuron_count", 2000)  # More neurons for complex behavior
config.set("brain.connection_probability", 0.15)  # Higher connectivity
config.set("plasticity.stdp.learning_rate", 0.002)  # Stronger STDP
config.set("plasticity.hebbian.enable", True)  # Enable Hebbian learning
config.set("neuromod.dopamine.scale", 2.0)  # Stronger reward signals
config.set("development.enable", True)  # Enable brain development
config.set("development.synaptogenesis_rate", 0.0005)  # Faster synapse formation

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### 2. Learning Experiment with Reward and Memory

```python
import pynlm

def run_learning_experiment(num_steps=2000):
    """Run a comprehensive learning experiment."""
    
    # Setup agent with all learning systems
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePredictionError(True)
    
    # Track learning metrics
    learning_data = {
        'total_reward': 0.0,
        'episodes_learned': 0,
        'curiosity_levels': [],
        'novelty_levels': [],
        'development_stages': [],
        'working_memory_traces': []
    }
    
    print("Starting comprehensive learning experiment...")
    print("Step | Reward | Curiosity | Novelty | Dev Stage | WM Traces | Episodes")
    print("-----|--------|-----------|---------|-----------|-----------|----------")
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get and apply action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        reward = agent.getNeuromodulationLevel()  # Current dopamine level as proxy
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        if step % 100 == 0:
            curiosity = agent.getCuriosityLevel()
            novelty = agent.getNoveltyLevel()
            dev_stage = brain.getDevelopmentalStage()
            wm_traces = brain.getWorkingMemory() and brain.getWorkingMemory().getActiveTraces() or 0
            
            learning_data['total_reward'] += reward
            learning_data['curiosity_levels'].append(curiosity)
            learning_data['novelty_levels'].append(novelty)
            learning_data['development_stages'].append(dev_stage)
            learning_data['working_memory_traces'].append(wm_traces)
            
            print(f"{step:4d} | {reward:6.3f} | {curiosity:9.3f} | {novelty:7.3f} | "
                  f"{dev_stage.name:10s} | {wm_traces:10d} | {learning_data['episodes_learned']}")
    
    print(f"\nExperiment completed!")
    print(f"Total reward collected: {learning_data['total_reward']:.3f}")
    print(f"Average curiosity: {sum(learning_data['curiosity_levels'])/len(learning_data['curiosity_levels']):.3f}")
    print(f"Average novelty: {sum(learning_data['novelty_levels'])/len(learning_data['novelty_levels']):.3f}")
    print(f"Final development stage: {learning_data['development_stages'][-1].name}")
    print(f"Final working memory traces: {learning_data['working_memory_traces'][-1]}")
    
    return learning_data

# Run the experiment
learning_results = run_learning_experiment(1000)
```

### 3. Checkpointing and Persistence

```python
import pynlm
import os

def run_checkpointing_demo():
    """Demonstrate checkpoint saving and loading."""
    
    # Create and initialize brain
    config = pynlm.createDefaultConfig()
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent1 = pynlm.createAgentBrain(brain1)
    agent1.initialize(world)
    agent1.enableRewardModulation(True)
    
    print("Running simulation for first checkpoint...")
    for step in range(200):
        world.update(0.1)
        agent1.processSensoryInput(world.getSensoryPercept())
        brain1.step(step)
        world.applyMotorCommand(agent1.decodeMotorCommand(), world.getSimulationTime())
    
    # Save checkpoint
    checkpoint_path = "./test_checkpoint.bin"
    if brain1.save(checkpoint_path):
        print(f"✓ Checkpoint saved to {checkpoint_path}")
    else:
        print("✗ Failed to save checkpoint")
        return
    
    # Check checkpoint properties
    initial_spikes = brain1.getTotalSpikeCount()
    print(f"Initial brain state: {initial_spikes} total spikes")
    
    # Create new brain and load checkpoint
    print("\nLoading checkpoint into new brain...")
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    
    if brain2.load(checkpoint_path):
        print("✓ Checkpoint loaded successfully")
        
        # Verify loaded state
        loaded_spikes = brain2.getTotalSpikeCount()
        print(f"Loaded brain state: {loaded_spikes} total spikes")
        
        # Note: Due to neural dynamics, exact spike counts may differ,
        # but the network structure should be preserved
        if loaded_spikes > 0:
            print("✓ Loaded brain has activity (structure preserved)")
        else:
            print("ℹ Loaded brain is quiet (normal for fresh simulation)")
            
    else:
        print("✗ Failed to load checkpoint")
    
    # Cleanup
    if os.path.exists(checkpoint_path):
        os.remove(checkpoint_path)
        print(f"\nCleaned up checkpoint file")

# Run checkpointing demo
run_checkpointing_demo()
```

### 4. Multi-Agent Coordination

```python
import pynlm

def run_multi_agent_scenario():
    """Demonstrate multiple agents coordinating in the same world."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=16, visionHeight=16)
    world.reset()
    
    # Create multiple brains with different personalities
    config1 = pynlm.createDefaultConfig()
    config1.set("brain.neuron_count", 800)
    config1.set("neuromod.dopamine.scale", 1.0)  # Conservative
    brain1 = pynlm.createBrain(config1)
    brain1.initialize()
    
    config2 = pynlm.createDefaultConfig()
    config2.set("brain.neuron_count", 800)
    config2.set("neuromod.dopamine.scale", 3.0)  # Risk-seeking
    brain2 = pynlm.createBrain(config2)
    brain2.initialize()
    
    # Create agents for each brain
    agent1 = pynlm.createAgentBrain(brain1)
    agent2 = pynlm.createAgentBrain(brain2)
    
    agent1.initialize(world)
    agent2.initialize(world)
    
    # Position agents in different parts of world
    world.setAgentStart(10.0, 10.0)
    agent1.initialize(world)
    
    world.setAgentStart(40.0, 40.0)
    agent2.initialize(world)
    
    print("Multi-agent coordination scenario:")
    print(f"Agent 1 (conservative): Dopamine scale = {config1.getOr<float>('neuromod.dopamine.scale', 1.0)}")
    print(f"Agent 2 (risk-seeking): Dopamine scale = {config2.getOr<float>('neuromod.dopamine.scale', 3.0)}")
    print("\nStep | Agent1 Action | Agent2 Action | World Resources")
    print("-----|---------------|---------------|----------------")
    
    # Run coordinated simulation
    for step in range(100):
        world.update(0.1)
        
        # Both agents sense the same world but may act differently
        percept = world.getSensoryPercept()
        
        agent1.processSensoryInput(percept)
        agent2.processSensoryInput(percept)
        
        brain1.step(step)
        brain2.step(step)
        
        action1 = agent1.decodeMotorCommand()
        action2 = agent2.decodeMotorCommand()
        
        world.applyMotorCommand(action1, world.getSimulationTime())
        world.applyMotorCommand(action2, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"{step:4d} | {str(action1):15s} | {str(action2):15s} | {world.getAgentBody().energy:10.1f}")
    
    # Compare agent behaviors
    curiosity1 = agent1.getCuriosityLevel()
    curiosity2 = agent2.getCuriosityLevel()
    
    print(f"\nAgent comparison:")
    print(f"Agent 1 curiosity: {curiosity1:.3f} (conservative)")
    print(f"Agent 2 curiosity: {curiosity2:.3f} (risk-seeking)")
    
    # Different dopamine scales should lead to different exploration behaviors
    if abs(curiosity1 - curiosity2) > 0.5:
        print("✓ Agents show different behavioral styles due to different dopamine settings")

# Run multi-agent demo
run_multi_agent_scenario()
```

### 5. Advanced Memory System Exploration

```python
import pynlm

def explore_memory_systems():
    """Deep dive into working and episodic memory systems."""
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Memory System Exploration:")
    print("=" * 50)
    
    # 1. Check working memory initialization
    working_memory = brain.getWorkingMemory()
    if working_memory:
        print(f"✓ Working memory system: ACTIVE")
        print(f"  - Initial active traces: {working_memory.getActiveTraces()}")
        print(f"  - Max capacity: {working_memory.getMaxTraces()}")
        print(f"  - Current load: {working_memory.getLoad()}")
    else:
        print("✗ Working memory system: NOT AVAILABLE")
    
    # 2. Check episodic memory
    episodic_memory = brain.getEpisodicMemory()
    if episodic_memory:
        print(f"\n✓ Episodic memory system: ACTIVE")
        print(f"  - Initial episode count: {episodic_memory.getEpisodeCount()}")
        print(f"  - Max episodes: {episodic_memory.getMaxEpisodes()}")
        print(f"  - Replay queue size: {episodic_memory.getReplayQueueSize()}")
    else:
        print("\n✗ Episodic memory system: NOT AVAILABLE")
    
    # 3. Run simulation and track memory changes
    print(f"\nRunning simulation to observe memory dynamics...")
    for step in range(300):
        brain.step(step)
        
        if step % 50 == 0:
            if working_memory:
                wm_traces = working_memory.getActiveTraces()
                print(f"Step {step:3d} | Working Memory: {wm_traces:4d} active traces")
            
            if episodic_memory:
                episodes = episodic_memory.getEpisodeCount()
                print(f"Step {step:3d} | Episodic Memory: {episodes:4d} stored episodes")
    
    # 4. Demonstrate memory replay
    if episodic_memory and episodic_memory.getEpisodeCount() > 0:
        print(f"\nMemory Replay Demonstration:")
        episodes_for_replay = episodic_memory.getEpisodesForReplay(2)
        print(f"✓ Retrieved {len(episodes_for_replay)} episodes for replay")
        
        # Replay could be used to:
        # - Consolidate important memories
        # - Extract patterns
        # - Influence current behavior
        print("  Replay can be used for memory consolidation and pattern extraction")
    
    # 5. Check memory integration indicators
    print(f"\nMemory Integration Status:")
    print(f"✓ Working memory: {'CONNECTED' if working_memory else 'DISCONNECTED'}")
    print(f"✓ Episodic memory: {'CONNECTED' if episodic_memory else 'DISCONNECTED'}")
    print(f"✓ Memory systems are part of the integrated brain loop")

# Run memory exploration
explore_memory_systems()
```

### 6. Development and Maturation Tracking

```python
import pynlm

def track_development_process():
    """Track brain development over time."""
    
    config = pynlm.createDefaultConfig()
    config.set("development.enable", True)
    config.set("development.synaptogenesis_rate", 0.001)
    config.set("development.pruning_rate", 0.0001)
    config.set("development.maturation_rate", 0.01)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain Development Tracking:")
    print("=" * 60)
    print(f"Initial developmental stage: {brain.getDevelopmentalStage().name}")
    print(f"Initial neuron count: {brain.getTotalNeuronCount()}")
    print(f"Initial synapse count: {brain.getTotalSynapseCount()}")
    print(f"Initial firing rate: {brain.getAverageFiringRate():.3f}")
    
    print(f"\nStep | Dev Stage      | Firing Rate | Synapses | Developmental Changes")
    print(f"-----|----------------|-------------|----------|----------------------")
    
    development_stages = []
    firing_rates = []
    
    for step in range(200):
        brain.step(step)
        
        current_stage = brain.getDevelopmentalStage()
        firing_rate = brain.getAverageFiringRate()
        synapse_count = brain.getTotalSynapseCount()
        
        development_stages.append(current_stage)
        firing_rates.append(firing_rate)
        
        if step % 20 == 0:
            print(f"{step:4d} | {current_stage.name:16s} | {firing_rate:11.3f} | {synapse_count:8d} | {'Active' if current_stage != development_stages[-2] if step > 0 else 'N/A'}")
    
    # Analyze development patterns
    print(f"\nDevelopment Analysis:")
    unique_stages = set(development_stages)
    print(f"Development stages experienced: {len(unique_stages)}")
    
    if len(unique_stages) > 1:
        print(f"✓ Brain underwent {len(unique_stages)} developmental stages")
        print(f"  Stages: {[s.name for s in unique_stages]}")
    
    # Check final development state
    final_stage = development_stages[-1]
    avg_firing_rate = sum(firing_rates) / len(firing_rates)
    
    print(f"\nFinal Development State:")
    print(f"  Stage: {final_stage.name}")
    print(f"  Average firing rate: {avg_firing_rate:.3f}")
    print(f"  Final synapse count: {brain.getTotalSynapseCount()}")
    
    if final_stage.name == "Adult":
        print(f"✓ Brain reached full maturity!")
    elif final_stage.name == "CriticalPeriod":
        print(f"ℹ Brain in critical learning period")
    else:
        print(f"📈 Brain is still developing")

# Track development process
 track_development_process()
```

### 7. Performance Benchmarking

```python
import pynlm
import time

def benchmark_brain_performance():
    """Benchmark brain performance with different configurations."""
    
    def run_benchmark(neuron_count, description):
        print(f"\nBenchmark: {description}")
        print("-" * 40)
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neuron_count)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        start_steps = brain.getTotalSpikeCount()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        end_time = time.time()
        end_steps = brain.getTotalSpikeCount()
        
        elapsed_time = end_time - start_time
        spikes_generated = end_steps - start_steps
        steps_per_second = 100 / elapsed_time
        spikes_per_second = spikes_generated / elapsed_time
        
        print(f"Neurons: {neuron_count}")
        print(f"Time: {elapsed_time:.3f}s")
        print(f"Steps processed: 100")
        print(f"Steps/sec: {steps_per_second:.1f}")
        print(f"Spikes generated: {spikes_generated}")
        print(f"Spikes/sec: {spikes_per_second:.1f}")
        print(f"Efficiency (spikes/step): {spikes_generated/100:.1f}")
        
        return {
            'neurons': neuron_count,
            'time': elapsed_time,
            'steps_per_sec': steps_per_second,
            'spikes_per_sec': spikes_per_second,
            'efficiency': spikes_generated / 100
        }
    
    print("Brain Performance Benchmark")
    print("=" * 40)
    
    # Run benchmarks with different brain sizes
    benchmarks = []
    
    for neuron_count in [100, 500, 1000, 2000]:
        benchmark = run_benchmark(neuron_count, f"{neuron_count} Neurons")
        benchmarks.append(benchmark)
    
    # Compare results
    print(f"\nPerformance Comparison:")
    print(f"{'Neurons':>8} | {'Time (s)':>10} | {'Steps/sec':>12} | {'Spikes/sec':>12} | {'Efficiency':>12}")
    print(f"{'-' * 8} | {'-' * 10} | {'-' * 12} | {'-' * 12} | {'-' * 12}")
    
    for b in benchmarks:
        print(f"{b['neurons']:>8} | {b['time']:10.3f} | {b['steps_per_sec']:12.1f} | "
              f"{b['spikes_per_sec']:12.1f} | {b['efficiency']:12.1f}")
    
    # Find optimal configuration
    best_efficiency = max(b['efficiency'] for b in benchmarks)
    best_benchmark = next(b for b in benchmarks if b['efficiency'] == best_efficiency)
    
    print(f"\nOptimal Configuration:")
    print(f"Best efficiency ({best_efficiency:.1f} spikes/step) with {best_benchmark['neurons']} neurons")

# Run performance benchmark
benchmark_brain_performance()
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

### Advanced Commands

| Feature | Method | Description |
|---------|--------|-------------|
| Custom config | `config.set("key", value)` | Set configuration parameters |
| Memory access | `brain.getWorkingMemory()` | Access working memory system |
| Development tracking | `brain.getDevelopmentalStage()` | Check brain development stage |
| Neuromodulation | `agent.getCuriosityLevel()` | Get neuromodulation levels |
| Prediction | `agent.getPredictionError()` | Access prediction system |
| Checkpoint | `brain.save("path")` | Save brain state |
| Load state | `brain.load("path")` | Restore brain state |
| Multiple agents | `agent1.processSensoryInput()` | Share world between agents |

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check that sensory input is connected

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify motor neurons are active

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check configuration parameters

### "Memory systems not working"
- Verify the brain has been running long enough for memory formation
- Check that learning systems are enabled
- Memory may be sparse in simple environments

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for more details**
   - Deep dive into configuration options
   - Learn about advanced building procedures

2. **Read `docs/ARCHITECTURE.md` to understand how it all works**
   - System architecture overview
   - Component interactions

3. **Experiment with different configurations!**
   - Try different neuron counts and connection probabilities
   - Experiment with different neuromodulation settings
   - Test various developmental parameters

4. **Contribute to the project**
   - Share your advanced examples
   - Report issues and suggest improvements
   - Help document the system

---

That's it! You're now ready to use NLM at both beginner and advanced levels.

