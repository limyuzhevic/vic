# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**Key Features:**
- Event-driven spike propagation (biologically realistic)
- Multiple learning mechanisms (STDP, Hebbian, reward-modulated)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages
- Persistent memory with checkpointing
- High-performance with SoA and SIMD optimizations

---

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks
2. **World** - The environment the brain lives in
3. **Agent** - The bridge connecting brain to world

# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

**Key Features:**
- Event-driven spike propagation (biologically realistic)
- Multiple learning mechanisms (STDP, Hebbian, reward-modulated)
- Integrated memory systems (working, episodic, associative)
- Neuromodulation (dopamine, curiosity, novelty)
- Developmental stages
- Persistent memory with checkpointing
- High-performance with SoA and SIMD optimizations

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

### Example 3: Advanced Learning Agent

```python
import pynlm

def run_learning_agent(config, num_steps=1000):
    """Run a complete learning agent with reward modulation."""
    
    # 1. Create and configure brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 4. Initialize agent with world
    agent.initialize(world)
    
    # 5. Enable advanced features
    agent.enableRewardModulation(True)      # Learn from rewards
    agent.enableStructuralPlasticity(True)  # Grow new connections
    agent.enableDevelopment(True)           # Brain matures over time
    agent.enableCuriosity(True)             # Explore new things
    
    # 6. Main simulation loop
    for step in range(num_steps):
        # Update world dynamics
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input and let brain think
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action and execute it
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward-based learning
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development stage
        agent.updateDevelopment(0.1)
        
        # Print progress periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Learning rate: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Exploration: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Development: {brain.getDevelopmentalStage()}")
    
    return {
        'total_steps': num_steps,
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount()
    }

# Advanced configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)           # Larger brain
config.set("brain.synapse_density", 0.08)        # More connections
config.set("plasticity.stdp.enable", True)       # Enable STDP
config.set("plasticity.hebbian.enable", True)    # Enable Hebbian
config.set("neuromod.dopamine.scale", 1.5f)      # Higher dopamine

# Run the advanced learning agent
results = run_learning_agent(config, 500)
print(f"Agent training complete! Final firing rate: {results['final_firing_rate']:.2f} Hz")
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

## Advanced Usage Patterns

### Pattern 1: Running Multiple Independent Brains

```python
import pynlm

def run_multiple_brains(num_brains=5, steps_per_brain=200):
    """Run multiple brains in parallel to compare behaviors."""
    results = []
    
    for brain_id in range(num_brains):
        print(f"=== Brain {brain_id + 1}/{num_brains} ===")
        
        # Create brain with unique seed for diversity
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500 + brain_id * 100)  # Varying sizes
        config.set("brain.initial_weight_mean", 0.5 + brain_id * 0.05)
        config.set("plasticity.stdp.learning_rate", 0.001 * (brain_id + 1))
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run brain independently
        for step in range(steps_per_brain):
            brain.step(step)
            
            if step % 50 == 0:
                print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons")
        
        # Collect results
        results.append({
            'brain_id': brain_id,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount()
        })
    
    print("\n=== Comparison Results ===")
    for result in results:
        print(f"Brain {result['brain_id']}: {result['final_firing_rate']:.2f} Hz, "
              f"{result['total_spikes']} spikes, {result['neuron_count']} neurons")
    
    return results

# Run comparison
brain_results = run_multiple_brains(3, 100)
```

### Pattern 2: Brain Growth and Development

```python
import pynlm

def run_development_experiment():
    """Run a development experiment showing brain maturation."""
    
    # Create brain with developmental features
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("development.initial_plasticity", 1.0)
    config.set("development.plasticity_decay_rate", 0.95)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with development enabled
    agent = pynlm.createAgentBrain(brain)
    
    print("=== Brain Development Experiment ===")
    print("Initial developmental stage:", brain.getDevelopmentalStage())
    
    for step in range(200):
        # Brain thinks
        brain.step(step)
        
        # Apply development
        agent.updateDevelopment(0.1)
        
        # Print developmental milestones
        if step % 50 == 0:
            stage = brain.getDevelopmentalStage()
            plasticity = brain.getDevelopmentSystem().getCurrentPlasticityRate()
            print(f"Step {step}: {stage.name} stage, plasticity: {plasticity:.3f}")
            
            # Check for developmental transitions
            if stage == pynlm.DevelopmentalStage.Maturation:
                print("  -> Brain reaches maturation phase!")
            elif stage == pynlm.DevelopmentalStage.Adult:
                print("  -> Brain becomes adult!")
    
    return {
        'final_stage': brain.getDevelopmentalStage(),
        'final_plasticity': brain.getDevelopmentSystem().getCurrentPlasticityRate()
    }

# Run development experiment
development_results = run_development_experiment()
print(f"Final development stage: {development_results['final_stage'].name}")
```

### Pattern 3: Memory-Based Learning

```python
import pynlm

def run_memory_learning_experiment():
    """Demonstrate episodic memory and pattern recall."""
    
    # Create configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 800)
    config.set("memory.episodic.capacity", 50)
    
    # Create brain and agent
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create simple world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable memory systems
    agent.enableCuriosity(True)
    agent.enableNoveltyDetection(True)
    
    print("=== Memory Learning Experiment ===")
    
    # Store some experiences
    experiences = []
    
    for episode in range(10):
        print(f"\nEpisode {episode + 1}/10")
        
        # Run episode
        for step in range(50):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Store episodic memory every 10 steps
            if step % 10 == 0:
                agent.storeEpisodicMemory(f"Step {step}, Action {action}")
        
        # Check memory
        memory_count = brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0
        experiences.append(memory_count)
        print(f"  Stored {memory_count} episodes")
    
    # Retrieve and recall patterns
    print(f"\nMemory Statistics:")
    print(f"  Total episodes stored: {sum(experiences)}")
    print(f"  Average per episode: {sum(experiences) / len(experiences):.1f}")
    
    return {
        'total_episodes': sum(experiences),
        'average_per_episode': sum(experiences) / len(experiences)
    }

# Run memory learning experiment
memory_results = run_memory_learning_experiment()
```

---

## Quick Reference

### Brain Creation and Initialization

| What you want | Code |
|--------------|------|
| Create brain | `pynlm.createBrain(config)` |
| Create default config | `pynlm.createDefaultConfig()` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| Save brain state | `brain.save("checkpoint.bin")` |
| Load brain state | `brain.load("checkpoint.bin")` |

### World and Agent Creation

| What you want | Code |
|--------------|------|
| Create world | `pynlm.createSimpleWorld()` |
| Configure world | `world.configure(width, height, visionWidth, visionHeight)` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get world perception | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Apply reward | `agent.applyRewardModulation(reward, predicted)` |

### Advanced Configuration Options

| Feature | Configuration | Example |
|---------|--------------|---------|
| Brain size | `brain.neuron_count` | `config.set("brain.neuron_count", 2000)` |
| Connectivity | `brain.synapse_density` | `config.set("brain.synapse_density", 0.1)` |
| STDP learning | `plasticity.stdp.enable` | `config.set("plasticity.stdp.enable", True)` |
| Reward scale | `neuromod.dopamine.scale` | `config.set("neuromod.dopamine.scale", 1.5f)` |
| Development | `development.enable` | `config.set("development.enable", True)` |

### Neuron and Synapse Types

```python
# Neuron types
pynlm.NeuronType.Excitatory     # Regular excitatory neurons
pynlm.NeuronType.Inhibitory     # Inhibitory neurons
pynlm.NeuronType.Sensory         # Sensory input neurons
pynlm.NeuronType.Motor           # Motor output neurons
pynlm.NeuronType.Modulatory      # Neuromodulatory neurons
pynlm.NeuronType.Internal        # Internal processing neurons

# Developmental stages
pynlm.DevelopmentalStage.Initial        # Newborn brain
pynlm.DevelopmentalStage.CriticalPeriod # Sensitive learning period
pynlm.DevelopmentalStage.Maturation     # Brain matures
pynlm.DevelopmentalStage.Adult         # Fully developed
pynlm.DevelopmentalStage.Aging         # Age-related changes
```

### System Statistics

```python
# These methods provide detailed brain statistics:
brain.getTotalNeuronCount()           # Total number of neurons
brain.getTotalSynapseCount()           # Total number of connections
brain.getFiringNeuronCount()           # Currently active neurons
brain.getAverageFiringRate()           # Average firing frequency
brain.getExcitationInhibitionRatio()  # E/I balance
brain.getTotalSpikeCount()             # Total spikes fired
brain.getPendingSpikeEventCount()      # Events in queue
brain.getDevelopmentalStage()         # Current developmental phase
brain.getLearnedPreference()           # Learned behavioral preferences
```

### Python Module Functions

```python
# Core creation functions
pynlm.createBrain(config)                    # Create neural brain
pynlm.createAgentBrain(brain)                # Create agent interface
pynlm.createSimpleWorld()                    # Create simulation world
pynlm.createDefaultConfig()                  # Create default configuration

# Enumeration constants
pynlm.NeuronType, pynlm.DevelopmentalStage,
pynlm.ActionType, pynlm.WorldObjectType

# Version and metadata
pynlm.__version__                           # NLM version
pynlm.getBuildInfo()                        # Build information
pynlm.getSystemInfo()                       # System capabilities
```

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
