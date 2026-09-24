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

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more details
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Read `docs/PHASE6_FINAL_AUDIT.md` for Phase 6 integration details
4. Read `docs/SCIENCE.md` for scientific background
5. Experiment with different configurations!

## Advanced Features You Can Explore

### 1. Memory Systems
```python
# Working memory - temporary storage
brain.getWorkingMemory()

# Episodic memory - experience replay  
brain.getEpisodicMemory()

# Concept formation - pattern discovery
brain.getConceptFormation()
```

### 2. Neuromodulation
```python
# Reward prediction
brain.getDopamine()
brain.getPredictionErrorSignal()

# Curiosity and exploration
brain.getCuriosity()
brain.getNovelty()
```

### 3. Development and Plasticity
```python
# Brain development stages
brain.getDevelopmentalStage()
brain.setDevelopmentalStage(stage)

# Synaptic plasticity
brain.getSTDP()
brain.getStructuralPlasticity()
```

### 4. Advanced Agent Behaviors
```python
# Planning and decision making
brain.getPlanner()

# Attention and focus
brain.getAttention()

# Social learning
# (Implemented in the full NLM framework)
```

### 5. Performance Monitoring
```python
# Get brain statistics
brain.getTotalNeuronCount()
brain.getTotalSynapseCount()
brain.getActiveNeuronCount()
brain.getTotalSpikeCount()
brain.getAverageFiringRate()
```

## Complete Example: Learning Agent

Here's a comprehensive example that demonstrates all major features:

```python
import pynlm

def run_learning_agent(config=None, num_steps=1000):
    """
    Complete example of a learning agent using all NLM features.
    
    This agent:
    - Uses sensory perception (vision, touch, internal signals)
    - Makes decisions through neural planning
    - Learns through reward prediction error
    - Explores through curiosity
    - Maintains working and episodic memory
    """
    
    # Create brain with configuration
    if config is None:
        config = pynlm.createDefaultConfig()
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=12, visionHeight=12)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable advanced learning features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # Set development stage for enhanced learning
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)
    
    print(f"Starting learning simulation with {num_steps} steps")
    print(f"Brain has {brain.getTotalNeuronCount()} neurons")
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get sensory input
        percept = world.getSensoryPercept()
        
        # 3. Process through agent (inject into brain)
        agent.processSensoryInput(percept)
        
        # 4. Brain computation
        brain.step(step)
        
        # 5. Get action from neural planning
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward modulation (learning)
        # In a real scenario, you'd calculate reward based on world state
        # For now, use basic curiosity-based reward
        reward = brain.getNoveltyLevel() * 0.1
        predicted = brain.getPredictionError()
        agent.applyRewardModulation(reward, predicted)
        
        # 8. Monitor performance periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
            print(f"  Development stage: {brain.getDevelopmentalStage()}")
            print(f"  Neuromodulation levels:")
            if brain.getDopamine():
                print(f"    Dopamine: {brain.getDopamine().getLevel():.3f}")
            if brain.getCuriosity():
                print(f"    Curiosity: {brain.getCuriosity().getLevel():.3f}")
            print()
    
    print("Learning simulation complete!")
    
    # Final brain status
    brain.logStatus()
    
    return brain, world, agent

# Usage example
if __name__ == "__main__":
    # Run with default configuration
    brain, world, agent = run_learning_agent(num_steps=500)
    
    # Save brain state for later use
    brain.save("trained_brain.nml")
    
    # Load and continue learning
    new_brain = pynlm.createBrain(pynlm.createDefaultConfig())
    new_brain.load("trained_brain.nml")
```

## Configuration File Examples

### Basic Configuration (JSON)

```json
{
  "neuron_count": 1000,
  "region_count": 2,
  "connection_probability": 0.1,
  "simulation_timestep": 0.001,
  "random_seed": 12345,
  
  "stdp_ltp_weight": 0.01,
  "stdp_ltd_weight": 0.012,
  "stdp_tau": 20.0,
  
  "synaptogenesis_rate": 0.0001,
  "pruning_rate": 0.00001,
  
  "replay_interval": 100,
  "consolidation_interval": 1000,
  
  "checkpoint_dir": "./checkpoints",
  "checkpoint_interval": 1000
}
```

### Learning-Focused Configuration

```json
{
  "neuron_count": 2000,
  "region_count": 3,
  "connection_probability": 0.15,
  
  "stdp_ltp_weight": 0.02,    // Stronger learning
  "stdp_ltd_weight": 0.018,
  "stdp_tau": 30.0,           // Longer memory trace
  
  "synaptogenesis_rate": 0.0005,   // More structural growth
  "pruning_rate": 0.00005,         // Less pruning
  
  "replay_interval": 50,      // More frequent replay
  "consolidation_interval": 500,
  
  "development_stage": "CriticalPeriod",
  "plasticity_factor": 1.5
}
```

## Troubleshooting and Common Issues

### Issue: Brain produces no spikes

**Problem**: Neurons are not firing.

**Causes**:
1. Not calling `brain.initialize()`
2. No sensory input connected
3. Configuration has too few neurons
4. Development stage is "Adult" (low plasticity)

**Solution**:
```python
# Always initialize
brain.initialize()

# Ensure development allows learning
brain.setDevelopmentalStage(pynlm.DevelopmentalStage.CriticalPeriod)

# Add sensory input
agent.processSensoryInput(percept)  # Call this every step
```

### Issue: Agent doesn't explore

**Problem**: Agent gets stuck in local optima.

**Causes**:
1. Curiosity disabled
2. No novelty detection
3. Reward function too simple

**Solution**:
```python
# Enable exploration mechanisms
agent.enableCuriosity(True)
agent.enableNoveltyDetection(True)

# Use curiosity-driven exploration
if brain.getCuriosity().getLevel() > 0.5:
    # Force exploration action
    action = pynlm.Action(pynlm.ActionType.Look)
    world.applyMotorCommand(action, world.getSimulationTime())
```

### Issue: Memory systems not working

**Problem**: Memory doesn't persist or replay properly.

**Causes**:
1. Memory systems not initialized
2. Not enough storage capacity
3. Replay intervals too long

**Solution**:
```python
# Check memory systems
if brain.getWorkingMemory():
    print(f"Working memory capacity: {brain.getWorkingMemory().getCapacity()}")

if brain.getEpisodicMemory():
    print(f"Episodes stored: {brain.getEpisodicMemory().getEpisodeCount()}")

# Configure memory properly
brain.getWorkingMemory().setCapacity(neuron_count // 10)
brain.getEpisodicMemory().setMaxEpisodes(1000)
```

## Performance Optimization Tips

### 1. Configure for Your Use Case

```python
# For real-time applications
config = pynlm.createDefaultConfig()
config.setInt("neuron_count", 500)  # Smaller brain
config.setFloat("simulation_timestep", 0.01)  # Faster updates

# For high-fidelity simulation
config.setInt("neuron_count", 5000)  # Larger brain
config.setInt("region_count", 5)     # More regions
config.setFloat("simulation_timestep", 0.001)  # Slower, more accurate
```

### 2. Use Checkpointing

```python
# Save brain state periodically to resume later
if step % 1000 == 0:
    brain.save(f"checkpoint_{step//1000}.nml")
    print(f"Saved checkpoint at step {step}")
```

### 3. Monitor Performance

```python
import time

start_time = time.time()
for step in range(num_steps):
    # ... simulation steps ...
    
    # Log performance every 100 steps
    if step % 100 == 0:
        current_time = time.time()
        steps_per_second = 100 / (current_time - start_time)
        print(f"Performance: {steps_per_second:.1f} steps/second")
        start_time = current_time
```

## Python-Specific Tips and Best Practices

### 1. Use Context Managers for Clean Resource Management

```python
class NLMEnvironment:
    """Context manager for NLM simulation lifecycle."""
    
    def __init__(self, config=None):
        self.config = config
        self.brain = None
        self.world = None
        self.agent = None
    
    def __enter__(self):
        self.brain = pynlm.createBrain(self.config or pynlm.createDefaultConfig())
        self.brain.initialize()
        
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=10, height=10)
        self.world.reset()
        
        self.agent = pynlm.createAgentBrain(self.brain)
        self.agent.initialize(self.world)
        
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        # Clean up resources
        if self.agent:
            self.agent.reset()
        if self.brain:
            # Save final state if needed
            pass
    
    def run_steps(self, num_steps):
        """Run simulation for specified number of steps."""
        for step in range(num_steps):
            self.world.update(0.1)
            percept = self.world.getSensoryPercept()
            self.agent.processSensoryInput(percept)
            self.brain.step(step)
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())

# Usage
with NLMEnvironment() as env:
    env.run_steps(1000)
```

### 2. Type Hints for Better Code

```python
import pynlm
from typing import Optional, List, Tuple

def create_brain_with_memory(
    neuron_count: int = 1000,
    enable_plasticity: bool = True,
    development_stage: pynlm.DevelopmentalStage = pynlm.DevelopmentalStage.Initial
) -> pynlm.Brain:
    """Create a brain with optimized memory settings."""
    config = pynlm.createDefaultConfig()
    config.setInt("neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if enable_plasticity:
        # Configure for learning
        brain.getSTDP().configure(0.01, 0.012, 20.0)
        brain.getStructuralPlasticity().configure(0.0001, 0.00001)
    
    brain.setDevelopmentalStage(development_stage)
    return brain
```

### 3. Batch Processing for Multiple Brains

```python
def run_multiple_simulations(
    num_simulations: int,
    steps_per_simulation: int,
    seed: int = 42
) -> List[Tuple[pynlm.Brain, float]]:
    """Run multiple independent simulations."""
    results = []
    
    for sim in range(num_simulations):
        # Create unique config for each simulation
        config = pynlm.createDefaultConfig()
        config.setInt("random_seed", seed + sim)
        
        # Run simulation
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # ... run simulation steps ...
        
        # Calculate performance metric
        final_performance = brain.getAverageFiringRate()
        results.append((brain, final_performance))
        
        print(f"Simulation {sim + 1}/{num_simulations} complete")
    
    return results
```

## API Reference Summary

### Core Classes

| Class | Purpose | Key Methods |
|-------|---------|-------------|
| `Brain` | Neural processing | `initialize()`, `step()`, `produceAction()` |
| `Config` | Configuration management | `loadFromFile()`, `saveToFile()` |
| `AgentBrain` | Agent-world interface | `processSensoryInput()`, `decodeMotorCommand()` |
| `SimpleWorld` | Environment simulation | `update()`, `applyMotorCommand()` |

### Type Enums

- `NeuronType`: Excitatory, Inhibitory, Sensory, Motor, etc.
- `ActionType`: MoveForward, Look, Interact, etc.
- `DevelopmentalStage`: Initial, CriticalPeriod, Maturation, Adult, Aging
- `FiringState`: Resting, Active, Refractory, Inhibited

### Helper Functions

- `createDefaultConfig()`: Create default configuration
- `createBrain()`: Create new brain instance
- `createSimpleWorld()`: Create simulation world
- `createAgentBrain()`: Create agent interface

## Conclusion

NLM provides a powerful framework for creating artificial developmental brains. This guide covers the basics and advanced features needed to build sophisticated learning agents.

Key takeaways:

1. **Always initialize** your brain before use
2. **Enable learning features** early in development
3. **Monitor performance** regularly during long simulations
4. **Use checkpointing** to save progress
5. **Leverage memory systems** for experience replay
6. **Balance exploration and exploitation** through neuromodulation

The Python bindings make it easy to experiment with different brain architectures and learning algorithms. Start with the basic examples, then gradually explore the more advanced features as you become comfortable with the framework.

Happy brain-building! 🧠

That's it! You're now ready to use NLM with advanced features and comprehensive examples.
