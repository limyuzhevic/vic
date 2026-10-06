# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**NEW in NLM v0.1.0:** Enhanced Python API with advanced convenience functions and developmental-stage specific configurations!

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

### Example 2: Enhanced Learning with Developmental Stages

```python
import pynlm

# Create a developmentally-appropriate brain for learning
config = pynlm.create_development_config()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a learning simulation
for step in range(1000):
    # Simulate random input
    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 30.0)
    brain.step(step)
    
    if step % 200 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
              f"Avg rate: {brain.getAverageFiringRate():.1f} Hz")

print(f"Learning complete! Final firing rate: {brain.getAverageFiringRate():.1f} Hz")
```

### Example 3: Complete Agent with World and Agent Interface

```python
import pynlm

def run_agent_episode(episode_length=500):
    # Setup with developmentally-appropriate brain
    config = pynlm.create_development_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Initialize agent with world
    agent.initialize(world)
    
    # Enable all subsystems for realistic development
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)  # Brain can grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Brain seeks novel experiences
    
    print(f"Episode {episode_length} steps simulation:")
    
    for step in range(episode_length):
        # Update environment
        world.update(0.1)
        
        # Get what the agent perceives
        percept = world.getSensoryPercept()
        
        # Tell the brain what it senses
        agent.processSensoryInput(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Brain decides what to do
        action = agent.decodeMotorCommand()
        
        # Perform the action in the world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Give feedback for learning (reward)
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
        agent.applyRewardModulation(reward, 0.0)
        
        # Update developmental stage
        agent.updateDevelopment(0.1)
        
        # Show progress periodically
        if step % 100 == 0:
            print(f"  Step {step}:")
            print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"    Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"    Curiosity level: {agent.getCuriosityLevel():.3f}")
            print(f"    Novelty level: {agent.getNoveltyLevel():.3f}")
            print(f"    Development stage: {brain.getDevelopmentalStage()}")
            
            # Show motor activity
            print(f"    Motor command: {action}")
    
    return {
        'brain': brain,
        'world': world,
        'agent': agent,
        'stats': {
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage(),
            'total_steps': episode_length
        }
    }

# Run the complete simulation
episode = run_agent_episode(1000)
print("\n=== Episode Complete ===")
print(f"Total spikes generated: {episode['stats']['total_spikes']}")
print(f"Final development stage: {episode['stats']['development_stage']}")
```

### Example 4: Benchmarking Different Developmental Stages

```python
import pynlm

def compare_developmental_stages():
    """Compare how brains at different developmental stages perform"""
    
    # Create a base configuration
    base_config = pynlm.createDefaultConfig()
    
    # Test different developmental stages
    stage_names = ["Initial (High Plasticity)", "Critical Period", "Maturation", "Adult (Low Plasticity)"]
    
    print("Comparing developmental stages...")
    print("=" * 60)
    
    results = pynlm.compare_developmental_stages(base_config, stage_names, steps_per_stage=200)
    
    print(f"{'Stage':<25} {'Spikes':<10} {'Time (ms)':<12} {'Rate (sp/ms)':<15} {'Firing Rate (Hz)':<15}")
    print("-" * 80)
    
    for stage_name, spikes, time_ms, rate_sp_ms, firing_rate in results:
        print(f"{stage_name:<25} {spikes:<10,} {time_ms:<12} {rate_sp_ms:<15.4f} {firing_rate:<15.2f}")
    
    return results

# Run comparison
stage_results = compare_developmental_stages()
```

### Example 5: Benchmarking Brain Performance

```python
import pynlm

def benchmark_brain_performance():
    """Benchmark brain performance with different step counts"""
    
    # Create test brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Brain Performance Benchmark")
    print("=" * 40)
    
    test_steps = [100, 500, 1000, 5000]
    
    for steps in test_steps:
        result = pynlm.benchmark_brain(brain, steps)
        
        spikes, synapses, firing, duration_ms, spikes_per_ms, neurons_per_ms = result
        
        print(f"\n{steps} steps:")
        print(f"  Total spikes: {spikes:,}")
        print(f"  Total synapses: {synapses:,}")
        print(f"  Final firing neurons: {firing:,}")
        print(f"  Duration: {duration_ms} ms")
        print(f"  Performance: {spikes_per_ms:.2f} spikes/ms, {neurons_per_ms:.2f} neurons/ms")
    
    return test_steps

# Run benchmark
benchmark_results = benchmark_brain_performance()
```

---

## Advanced Usage Patterns

### Pattern 1: Custom Agent Behavior

```python
def custom_agent_simulation(brain, world, agent, custom_behavior_func):
    """Run a custom agent simulation with your own decision logic"""
    
    world.reset()
    agent.initialize(world)
    
    for step in range(1000):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Use your custom decision logic instead of default
        action = custom_behavior_func(agent, world, step)
        
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply standard neuromodulation
        agent.applyRewardModulation(0.0, 0.0)
        agent.updateDevelopment(0.1)
    
    return brain, world, agent

# Example custom behavior function
def exploration_strategy(agent, world, step):
    """Explore randomly when curiosity is high"""
    if agent.getCuriosityLevel() > 0.5:
        # Random exploration
        return pynlm.MotorCommand.Wait
    else:
        # Use agent's normal decision
        return agent.decodeMotorCommand()
```

### Pattern 2: Multi-Episode Training

```python
def train_agent_over_episodes(num_episodes, steps_per_episode):
    """Train an agent over multiple episodes with learning accumulation"""
    
    # Create brain once (weights will persist)
    config = pynlm.create_development_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    training_results = []
    
    for episode in range(num_episodes):
        # Create new agent and world for each episode
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        # Run episode
        for step in range(steps_per_episode):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Extract reward from world state
            reward = 0.0
            if percept.getInternal():
                reward = percept.getInternal()[0]
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
        
        # Record episode statistics
        episode_stats = {
            'episode': episode,
            'spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'curiosity': agent.getCuriosityLevel(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        training_results.append(episode_stats)
        
        # Print progress
        print(f"Episode {episode + 1}/{num_episodes}: "
              f"Firing rate: {episode_stats['firing_rate']:.2f} Hz, "
              f"Curiosity: {episode_stats['curiosity']:.3f}")
    
    return brain, training_results

# Run training
brain, training_history = train_agent_over_episodes(10, 500)
```

---

## Configuration Options

### Developmental Stage Configurations

```python
# For rapid learning (juvenile brain)
juvenile_config = pynlm.create_exploratory_config()

# For stable performance (adult brain)  
adult_config = pynlm.create_steady_state_config()

# Balanced development
balanced_config = pynlm.create_development_config()
```

### Key Configuration Parameters

| Parameter | Description | Default | Range |
|-----------|-------------|---------|-------|
| `brain.neuron_count` | Total neurons in the brain | 500 | 100-5000 |
| `brain.synapse_density` | Connection probability between neurons | 0.05 | 0.01-0.2 |
| `plasticity.stdp.enable` | Spike-Timing Dependent Plasticity | True | True/False |
| `plasticity.hebbian.enable` | Hebbian learning | True | True/False |
| `plasticity.structural.enable` | Structural plasticity (growth) | True | True/False |
| `neuromod.curiosity.enable` | Curiosity-driven exploration | True | True/False |
| `neuromod.novelty.enable` | Novelty detection | True | True/False |

---

## Troubleshooting

### "My brain isn't doing anything"
- **Solution**: Ensure you call `brain.initialize()` before starting simulation
- **Tip**: Use developmental configs with `enableCuriosity(True)` for more activity

### "The agent isn't learning"
- **Solution**: Check that `agent.enableRewardModulation(True)` is called
- **Tip**: Add random sensory input to stimulate learning

### "Everything is 0"
- **Solution**: Brains need time to "warm up" - try more steps
- **Tip**: Use configurations with higher neuron counts for better statistics

---

## Getting Started

1. **Install**: `pip install pynlm`
2. **Try a simple brain**: Use Example 1 above
3. **Explore agent behavior**: Try Example 3 with a complete agent
4. **Experiment with configurations**: Try Examples 2 and 4
5. **Read documentation**: Check `HOW_TO_USE.md` for more details

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

### Project 3: Complete Learning Agent
```python
import pynlm

# Setup with learning enabled
config = pynlm.create_development_config()
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
| Create config | `pynlm.createDefaultConfig()` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Make brain think | `brain.step(step)` |
| Get stats | `brain.getTotalSpikeCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Enable learning | `agent.enableRewardModulation(True)` |

---

## New Features in This Version

- **Enhanced Python API**: Added convenience functions for common patterns
- **Developmental Stage Configs**: `create_development_config()`, `create_steady_state_config()`, `create_exploratory_config()`
- **Benchmarking Tools**: `benchmark_brain()`, `compare_developmental_stages()`
- **Advanced Examples**: Complete agent simulation, multi-episode training
- **Type Aliases**: Better Pythonic type names
- **Constants**: Useful constants for brain modeling
- **Error Handling**: Better error handling in I/O operations
- **Performance Monitoring**: Built-in benchmarking utilities

---

That's it! You're now ready to use NLM with the enhanced Python API.

## Further Reading

When you're comfortable:
1. Read `HOW_TO_USE.md` for more technical details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations and developmental stages!

The enhanced API provides powerful tools for creating, training, and benchmarking neural learning machines with different developmental profiles.