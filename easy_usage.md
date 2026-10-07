# NLM - Easy Usage Guide for Beginners and Advanced Users

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

## Advanced Usage - Integrated Systems

### Example 3: Using Brain Memory Systems

```python
import pynlm

# Create and initialize the brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

print("Working memory capacity:", working_memory.getCapacity())
print("Episodic memory entries:", episodic_memory.getEntryCount())
print("Associative memory patterns:", associative_memory.getPatternCount())
```

### Example 4: Neuromodulation and Learning

```python
import pynlm

# Setup complete agent with all learning systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Access neuromodulation signals
print("Neuromodulation level:", agent.getNeuromodulationLevel())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())

# Access brain-level neuromodulators
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
```

### Example 5: Brain State Management

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Save and reload brain state
brain.save("my_brain_checkpoint.json")
print("Brain saved!")

# Simulate some steps
for i in range(100):
    brain.step(i)

# Load saved state (simulate different episode)
brain.load("my_brain_checkpoint.json")
print("Brain reloaded with", brain.getTotalNeuronCount(), "neurons")

# Continue simulation
for i in range(50, 150):
    brain.step(i)
print("Simulation complete!")
```

### Example 6: Cognitive Systems Integration

```python
import pynlm

# Create brain with all cognitive systems
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Access cognitive systems
prediction_system = brain.getPredictionSystem()
neural_planner = brain.getPlanner()
concept_formation = brain.getConceptFormation()
attentional_selection = brain.getAttention()

print("Prediction system active:", prediction_system.isActive())
print("Planner available:", neural_planner != None)
print("Concept formation initialized:", concept_formation != None)
print("Attention system ready:", attentional_selection != None)

# Run a simple cognitive task
for step in range(200):
    brain.step(step)
    if step % 50 == 0:
        print(f"Step {step}: Firing rate = {brain.getAverageFiringRate():.2f}")
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

# Memory system stats
print("Working memory usage:", working_memory.getCurrentLoad())
print("Episodic memory size:", episodic_memory.getMemorySize())

# Neuromodulation stats
print("Dopamine level:", agent.getNeuromodulationLevel())
print("Curiosity level:", agent.getCuriosityLevel())
print("Novelty level:", agent.getNoveltyLevel())
print("Prediction error:", agent.getPredictionError())
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
| What you want | Code |
|--------------|------|
| Access working memory | `brain.getWorkingMemory()` |
| Access episodic memory | `brain.getEpisodicMemory()` |
| Access associative memory | `brain.getAssociativeMemory()` |
| Access prediction system | `brain.getPredictionSystem()` |
| Access neural planner | `brain.getPlanner()` |
| Access concept formation | `brain.getConceptFormation()` |
| Access attention system | `brain.getAttention()` |
| Access development system | `brain.getDevelopmentSystem()` |
| Get dopamine signal | `brain.getDopamine()` |
| Get curiosity signal | `brain.getCuriosity()` |
| Get novelty signal | `brain.getNovelty()` |
| Get prediction error | `brain.getPredictionErrorSignal()` |

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

### "Memory system not working"
- Check that memory systems are enabled in config
- Try running longer simulations for memory consolidation

### "Neuromodulation not affecting behavior"
- Ensure reward modulation is enabled
- Check that development is running

---

## Next Steps

When you're comfortable:

1. **Explore memory systems**: Try using working memory, episodic memory, and associative memory together
2. **Experiment with neuromodulation**: See how dopamine, curiosity, and novelty affect learning
3. **Try cognitive integration**: Use prediction, planning, and attention systems together
4. **Performance optimization**: See the performance examples below
5. Read `HOW_TO_USE.md` for more technical details
6. Read `docs/ARCHITECTURE.md` to understand how it all works
7. Experiment with different configurations!

---

## Performance Optimization Examples

### Example 7: Efficient Simulation Loop

```python
import pynlm
import time

def optimized_simulation(num_steps=1000):
    """Optimized simulation with performance monitoring."""
    
    # Pre-configure for performance
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)  # More neurons for richer behavior
    config.set("brain.synapse_density", 0.15)
    
    # Create systems
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    agent = pynlm.createAgentBrain(brain)
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=16, visionHeight=16)
    world.reset()
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Performance monitoring
    start_time = time.time()
    spike_milestones = [0, num_steps // 4, num_steps // 2, 3 * num_steps // 4, num_steps]
    
    for step in range(num_steps):
        # Optimized world update
        world.update(0.05)  # Larger timestep for efficiency
        
        # Get percept once per step
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step with timing info
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation every 10 steps
        if step % 10 == 0:
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development periodically
        if step % 20 == 0:
            agent.updateDevelopment(0.05)
        
        # Log performance at milestones
        if step in spike_milestones:
            elapsed = time.time() - start_time
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                  f"{brain.getTotalSpikeCount()} total spikes, "
                  f"{elapsed:.1f}s elapsed")
    
    # Print final statistics
    elapsed = time.time() - start_time
    print(f"\n=== Simulation Complete ===")
    print(f"Duration: {elapsed:.2f} seconds")
    print(f"Average speed: {num_steps/elapsed:.0f} steps/second")
    print(f"Final firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"Development stage: {brain.getDevelopmentalStage()}")
    print(f"Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"Novelty level: {agent.getNoveltyLevel():.3f}")
    
    return brain, agent, world

# Run optimized simulation
brain, agent, world = optimized_simulation(1000)
```

### Example 8: Batch Processing Multiple Brains

```python
import pynlm
import threading
import time

def run_batch_of_brains(num_brains=5, steps_per_brain=200):
    """Run multiple brains in parallel for comparison testing."""
    
    brains = []
    agents = []
    worlds = []
    
    print(f"Creating {num_brains} brains with different configurations...")
    
    for i in range(num_brains):
        # Different configurations for each brain
        config = pynlm.createDefaultConfig()
        
        # Vary parameters
        neuron_count = 500 + i * 300  # 500, 800, 1100, 1400, 1700
        config.set("brain.neuron_count", neuron_count)
        config.set("brain.synapse_density", 0.05 + i * 0.02)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        agent.initialize(world)
        
        # Enable all learning
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(i % 2 == 0)  # Vary structural plasticity
        agent.enableDevelopment(True)
        agent.enableCuriosity(i % 3 == 0)  # Vary curiosity
        
        brains.append(brain)
        agents.append(agent)
        worlds.append(world)
        
        print(f"  Brain {i+1}: {neuron_count} neurons, "
              f"structural plastic: {agent.isStructuralPlasticityEnabled()}, "
              f"curiosity: {agent.isCuriosityEnabled()}")
    
    # Run all brains
    print(f"\nRunning {num_brains} simulations in parallel...")
    start_time = time.time()
    
    for brain_idx, (brain, agent, world) in enumerate(zip(brains, agents, worlds)):
        print(f"\n=== Brain {brain_idx + 1} ===")
        
        for step in range(steps_per_brain):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 50 == 0:
                reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
        
        # Print results
        print(f"Results: {brain.getTotalNeuronCount()} neurons, "
              f"{brain.getTotalSpikeCount()} spikes, "
              f"{brain.getAverageFiringRate():.3f} firing rate, "
              f"{agent.getCuriosityLevel():.3f} curiosity")
    
    total_time = time.time() - start_time
    print(f"\n=== Batch Complete ===")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Average per brain: {total_time/num_brains:.2f} seconds")
    
    return brains, agents, worlds

# Run batch processing
brains, agents, worlds = run_batch_of_brains(3, 100)
```

### Example 9: Memory Benchmarking

```python
import pynlm
import time

def benchmark_memory_systems():
    """Benchmark different memory system configurations."""
    
    configs = [
        {"name": "Light Memory", "neurons": 500, "plasticity": False},
        {"name": "Balanced Memory", "neurons": 1000, "plasticity": True},
        {"name": "Heavy Memory", "neurons": 2000, "plasticity": True},
    ]
    
    results = []
    
    for config_info in configs:
        print(f"\nTesting {config_info['name']}...")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", config_info['neurons'])
        config.set("brain.synapse_density", 0.1)
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        agent.initialize(world)
        
        # Configure based on benchmark
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(config_info['plasticity'])
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
        
        # Run simulation
        start_time = time.time()
        spike_count = 0
        
        for step in range(500):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 10 == 0:
                spike_count += brain.getFiringNeuronCount()
                agent.applyRewardModulation(
                    world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0,
                    0.0
                )
                agent.updateDevelopment(0.1)
        
        elapsed = time.time() - start_time
        
        # Get memory stats
        working_memory = brain.getWorkingMemory()
        episodic_memory = brain.getEpisodicMemory()
        associative_memory = brain.getAssociativeMemory()
        
        result = {
            'name': config_info['name'],
            'neurons': config_info['neurons'],
            'plasticity': config_info['plasticity'],
            'time': elapsed,
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'working_memory_load': working_memory.getCurrentLoad(),
            'episodic_memory_size': episodic_memory.getMemorySize(),
            'associative_patterns': associative_memory.getPatternCount(),
            'final_curiosity': agent.getCuriosityLevel(),
            'final_novelty': agent.getNoveltyLevel(),
        }
        
        results.append(result)
        
        print(f"  Time: {elapsed:.2f}s, Spikes: {brain.getTotalSpikeCount()}, "
              f"Firing rate: {brain.getAverageFiringRate():.3f}")
        print(f"  Memory - Working: {working_memory.getCurrentLoad()}, "
              f"Episodic: {episodic_memory.getMemorySize()}, "
              f"Associative: {associative_memory.getPatternCount()}")
    
    # Print summary
    print(f"\n{'='*60}")
    print(f"{'Memory System Benchmark':^60}")
    print(f"{'='*60}")
    print(f"{'System':<15} {'Neurons':<8} {'Plastic':<8} {'Time':<8} {'Spikes':<8} {'Firing':<8} {'Working':<8} {'Episodic':<8} {'Associative':<10}")
    print(f"{'-'*90}")
    
    for result in results:
        print(f"{result['name']:<15} {result['neurons']:<8} {result['plasticity']:<8} "
              f"{result['time']:<8.2f} {result['total_spikes']:<8} "
              f"{result['avg_firing_rate']:<8.3f} {result['working_memory_load']:<8} "
              f"{result['episodic_memory_size']:<8} {result['associative_patterns']:<10}")
    
    return results

# Run memory benchmark
benchmark_results = benchmark_memory_systems()
```

---

## Performance Best Practices

### 1. Memory Management
- **Reuse objects**: Create and reuse brain/world/agent objects instead of creating new ones
- **Monitor memory usage**: Use working memory capacity to avoid overload
- **Checkpoint frequently**: Save brain state during long simulations

### 2. Timing Optimization
- **Adjust timesteps**: Use larger timesteps (0.05-0.1) for efficiency
- **Batch operations**: Process multiple sensory inputs together when possible
- **Minimize function calls**: Cache frequently accessed values

### 3. Learning System Configuration
- **Enable only needed systems**: Don't enable structural plasticity unless needed
- **Tune neuromodulation**: Adjust curiosity and reward sensitivity
- **Consider developmental stages**: Use appropriate developmental stages for your use case

### 4. Monitoring and Debugging
- **Log key metrics**: Track firing rates, spike counts, memory usage
- **Use checkpoints**: Save and reload states for debugging
- **Profile performance**: Monitor execution time and memory usage

---

## Advanced API Reference

### Memory System Access
```python
# Working memory (short-term active information)
working_memory = brain.getWorkingMemory()
capacity = working_memory.getCapacity()
current_load = working_memory.getCurrentLoad()
active_items = working_memory.getActiveItems()

# Episodic memory (experience storage)
episodic_memory = brain.getEpisodicMemory()
entry_count = episodic_memory.getEntryCount()
memory_size = episodic_memory.getMemorySize()
recent_episodes = episodic_memory.getRecentEpisodes()

# Associative memory (pattern associations)
associative_memory = brain.getAssociativeMemory()
pattern_count = associative_memory.getPatternCount()
associations = associative_memory.getAssociations()
pattern_strength = associative_memory.getPatternStrength(pattern_id)
```

### Neuromodulation Access
```python
# Global neuromodulation level
neuromodulation_level = agent.getNeuromodulationLevel()

# Specific neuromodulators
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()
prediction_error = brain.getPredictionErrorSignal()

# Neuromodulation metrics
curiosity_level = agent.getCuriosityLevel()
novelty_level = agent.getNoveltyLevel()
prediction_error_value = agent.getPredictionError()

# Neuromodulation control
brain.getDopamine().setLevel(0.5)
brain.getCuriosity().setDrive(0.8)
brain.getNovelty().setSensitivity(0.3)
```

### Cognitive Systems Access
```python
# Prediction system
prediction_system = brain.getPredictionSystem()
prediction_active = prediction_system.isActive()
prediction_confidence = prediction_system.getConfidence()
prediction_error = prediction_system.getPredictionError()

# Neural planner
neural_planner = brain.getPlanner()
planning_active = neural_planner.isPlanning()
planned_actions = neural_planner.getPlannedActions()
planning_horizon = neural_planner.getPlanningHorizon()

# Concept formation
concept_formation = brain.getConceptFormation()
concept_count = concept_formation.getConceptCount()
concepts = concept_formation.getConcepts()
concept_confidence = concept_formation.getConceptConfidence(concept_id)

# Attentional selection
attentional_selection = brain.getAttention()
attention_focus = attentional_selection.getFocus()
attention_strength = attentional_selection.getAttentionStrength()
attentional_resources = attentional_selection.getAvailableResources()
```

### Development System Access
```python
# Developmental stage
development_stage = brain.getDevelopmentalStage()
developmental_age = agent.getDevelopmentalStageAge()
plasticity_modifier = agent.getPlasticityModifier()

# Development system
development_system = brain.getDevelopmentSystem()
current_stage = development_system.getCurrentStage()
development_progress = development_system.getDevelopmentProgress()
next_stage_time = development_system.getNextStageTime()

# Development control
development_system.accelerateDevelopment()
development_system.slowDevelopment()
development_system.setStage(DevelopmentalStage.Adult)
```

---

## Troubleshooting

### Memory System Issues
**Symptoms**: Memory not filling, zero memory usage
**Solutions**:
- Run longer simulations (memory needs time to consolidate)
- Ensure developmental systems are enabled
- Check that brain is receiving adequate sensory input

**Symptoms**: Memory corruption, duplicate entries
**Solutions**:
- Save brain state periodically and reload after issues
- Reduce memory load if approaching capacity limits
- Check neuromodulation levels affecting memory consolidation

### Performance Issues
**Symptoms**: Simulation too slow
**Solutions**:
- Reduce number of neurons
- Use larger timesteps (0.05-0.1 instead of 0.01)
- Disable unnecessary learning systems
- Monitor and optimize memory usage

**Symptoms**: Memory usage growing uncontrollably
**Solutions**:
- Implement memory decay or cleanup mechanisms
- Use episodic memory pruning
- Adjust working memory capacity settings

### Neuromodulation Issues
**Symptoms**: Learning not occurring
**Solutions**:
- Ensure reward modulation is enabled
- Check that development is running
- Verify neuromodulation signals are within valid ranges

**Symptoms**: Behavior too erratic
**Solutions**:
- Reduce curiosity drive
- Check prediction error signals
- Ensure novelty detection is working properly

---

## Getting Started

### Quick Setup
1. **Install NLM**: Follow the installation instructions in HOW_TO_USE.md
2. **Create your first brain**: Use the simplest example above
3. **Add a world**: Create a simple environment
4. **Connect agent**: Set up the agent to interface brain and world
5. **Enable learning**: Add neuromodulation for interesting behavior
6. **Run simulation**: See your brain in action!

### Next Steps
1. **Explore memory systems**: Try working memory, episodic memory, and associative memory
2. **Experiment with neuromodulation**: See how dopamine, curiosity, and novelty affect learning
3. **Try cognitive integration**: Use prediction, planning, and attention systems
4. **Performance optimization**: Use the examples above for efficient simulations
5. **Advanced usage**: Read HOW_TO_USE.md for technical details
6. **Contribute**: Share your findings and improvements!

---

That's it! You're now ready to use NLM with all its advanced features. The documentation now includes comprehensive examples for using memory systems, neuromodulation, cognitive systems, and performance optimization.

Happy brain simulation! 🧠✨
