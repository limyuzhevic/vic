This guide covers the basic NLM usage. For more advanced features, see `HOW_TO_USE.md`.

## Quick Reference Guide

| Function | Description | Example |
|----------|-------------|---------|
| `pynlm.createBrain(config)` | Create a neural brain | `brain = pynlm.createBrain(pynlm.createDefaultConfig())` |
| `brain.initialize()` | Initialize brain subsystems | `brain.initialize()` |
| `brain.step(step)` | Run simulation step | `brain.step(step)` |
| `pynlm.createSimpleWorld()` | Create simulation world | `world = pynlm.createSimpleWorld()` |
| `agent.processSensoryInput(percept)` | Process sensory input | `agent.processSensoryInput(percept)` |
| `agent.decodeMotorCommand()` | Get motor action | `action = agent.decodeMotorCommand()` |
| `brain.getTotalNeuronCount()` | Get neuron count | `neurons = brain.getTotalNeuronCount()` |

## Common Patterns

### Pattern 1: Simulation with Agent

```python
import pynlm

def run_agent_episode(config, num_steps=1000):
    """Run a complete agent episode with world interaction."""
    
    # 1. Create brain and world
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    # 2. Configure and initialize
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # 3. Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # 4. Run simulation
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Optional: Get reward and apply neuromodulation
        # reward = get_reward_from_environment(percept)
        # agent.applyRewardModulation(reward, 0.0)
    
    return brain, world, agent

# Usage
config = pynlm.createDefaultConfig()
brain, world, agent = run_agent_episode(config, num_steps=500)
print(f"Simulation complete! Neurons: {brain.getTotalNeuronCount()}")
```

### Pattern 2: Brain Development Monitoring

```python
import pynlm

def monitor_brain_development(config, num_steps=1000):
    """Run simulation while monitoring brain development."""
    
    brain = pynlm.createBrain(config)
    agent = pynlm.createAgentBrain(brain)
    
    # Track development
    development_log = []
    
    brain.initialize()
    
    for step in range(num_steps):
        brain.step(step)
        
        # Log development status
        dev_stage = brain.getDevelopmentalStage()
        firing_rate = brain.getAverageFiringRate()
        neurons_active = brain.getFiringNeuronCount()
        
        development_log.append({
            'step': step,
            'development_stage': dev_stage,
            'firing_rate': firing_rate,
            'neurons_active': neurons_active
        })
        
        # Print progress every 100 steps
        if step % 100 == 0:
            print(f"Step {step}: Stage {dev_stage}, "
                  f"Firing Rate: {firing_rate:.2f}")
    
    return development_log

# Usage
config = pynlm.createDefaultConfig()
log = monitor_brain_development(config, num_steps=1000)
print(f"Development monitored over {len(log)} steps")
```

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

### Pattern 3: Learning with Curiosity

```python
import pynlm

def run_curiosity_driven_learning(config, num_steps=1000):
    """Run simulation with curiosity-driven exploration."""
    
    brain = pynlm.createBrain(config)
    world = pynlm.createSimpleWorld()
    agent = pynlm.createAgentBrain(brain)
    
    world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
    world.reset()
    agent.initialize(world)
    
    # Enable curiosity for exploration
    agent.enableCuriosity(True)
    agent.enableRewardModulation(True)
    
    curiosity_scores = []
    
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Curiosity affects exploration
        curiosity = agent.getCuriosityLevel()
        curiosity_scores.append(curiosity)
        
        motor_cmd = agent.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        if step % 200 == 0:
            print(f"Step {step}: Curiosity = {curiosity:.3f}")
    
    avg_curiosity = sum(curiosity_scores) / len(curiosity_scores)
    print(f"Average curiosity level: {avg_curiosity:.3f}")
    
    return curiosity_scores

# Usage
config = pynlm.createDefaultConfig()
curiosity_scores = run_curiosity_driven_learning(config, num_steps=1000)
```

## Advanced Features

NLM supports several advanced features for complex simulations:

### 1. Multiple Regions
```python
# Add different brain regions for specialized functions
region1 = brain.addRegion("SensoryCortex")
region2 = brain.addRegion("MotorCortex")
region3 = brain.addRegion("AssociationCortex")
```

### 2. Memory Systems
```python
# Access memory systems for cognitive modeling
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
associative_memory = brain.getAssociativeMemory()

# Store and retrieve experiences
working_memory.storeToNeuron(neuron_id, activation_value)
memory_neuron = working_memory.getMemoryNeurons()
```

### 3. Neuromodulation
```python
# Monitor neuromodulation systems
dopamine = brain.getDopamine()
curiosity = brain.getCuriosity()
novelty = brain.getNovelty()

print(f"Dopamine: {dopamine.getLevel():.3f}")
print(f"Curiosity: {curiosity.getLevel():.3f}")
print(f"Novelty: {novelty.getLevel():.3f}")
```

### 4. Checkpointing
```python
# Save and load brain state
brain.save("checkpoint_brain.nlm")

# Later...
brain.load("checkpoint_brain.nlm")
```

## Troubleshooting

### Common Issues and Solutions

**Issue: Brain doesn't fire neurons**
- **Solution**: Ensure `brain.initialize()` is called before simulation
- **Check**: Verify brain configuration has enough neurons

**Issue: Agent doesn't move**
- **Solution**: Check that `world.applyMotorCommand()` is called
- **Check**: Verify world simulation is being updated with `world.update()`

**Issue: Memory systems not working**
- **Solution**: Enable memory systems via configuration
- **Check**: Verify brain has sufficient neuron count for memory

**Issue: High memory usage**
- **Solution**: Reduce brain size or adjust memory system capacity
- **Check**: Monitor neuron and synapse counts

### Debug Commands

```python
# Check brain configuration
brain_config = brain.getConfig()
print(brain_config.summary())

# Get comprehensive status
brain.logStatus()

# Check memory usage
print(f"Working Memory Active Traces: {working_memory.getActiveTraces()}")
print(f"Episodic Memory Episodes: {episodic_memory.getEpisodeCount()}")
```

## Further Learning

Once comfortable with the basics:

1. **Explore HOW_TO_USE.md** - Comprehensive API documentation and examples
2. **Study ARCHITECTURE.md** - Deep dive into NLM design patterns
3. **Read SCIENCE.md** - Scientific background and theoretical foundations
4. **Experiment with configurations** - Different brain sizes and connection patterns
5. **Build custom environments** - Extend beyond SimpleWorld
6. **Implement learning algorithms** - Custom plasticity rules

## Version Information

This is Phase 6 (FINAL INTEGRATION) of the NLM project. The Python API is designed to be intuitive while exposing the full power of the C++ neural simulation engine.

---

*For technical documentation, see `HOW_TO_USE.md`
For scientific background, see `docs/SCIENCE.md`
For architecture details, see `docs/ARCHITECTURE.md`*

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

### Common Issues and Solutions

**Issue: Brain doesn't fire neurons**
- **Solution**: Ensure `brain.initialize()` is called before simulation
- **Check**: Verify brain configuration has enough neurons

**Issue: Agent doesn't move**
- **Solution**: Check that `world.applyMotorCommand()` is called
- **Check**: Verify world simulation is being updated with `world.update()`

**Issue: Memory systems not working**
- **Solution**: Enable memory systems via configuration
- **Check**: Verify brain has sufficient neuron count for memory

**Issue: High memory usage**
- **Solution**: Reduce brain size or adjust memory system capacity
- **Check**: Monitor neuron and synapse counts

### Debug Commands

```python
# Check brain configuration
brain_config = brain.getConfig()
print(brain_config.summary())

# Get comprehensive status
brain.logStatus()

# Check memory usage
print(f"Working Memory Active Traces: {working_memory.getActiveTraces()}")
print(f"Episodic Memory Episodes: {episodic_memory.getEpisodeCount()}")
```

## Further Learning

Once comfortable with the basics:

1. **Explore HOW_TO_USE.md** - Comprehensive API documentation and examples
2. **Study ARCHITECTURE.md** - Deep dive into NLM design patterns
3. **Read SCIENCE.md** - Scientific background and theoretical foundations
4. **Experiment with configurations** - Different brain sizes and connection patterns
5. **Build custom environments** - Extend beyond SimpleWorld
6. **Implement learning algorithms** - Custom plasticity rules

## Version Information

This is Phase 6 (FINAL INTEGRATION) of the NLM project. The Python API is designed to be intuitive while exposing the full power of the C++ neural simulation engine.

---

*For technical documentation, see `HOW_TO_USE.md`
For scientific background, see `docs/SCIENCE.md`
For architecture details, see `docs/ARCHITECTURE.md`*
