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

## Advanced Usage Patterns

### Pattern 4: Advanced Learning with Curiosity

```python
def run_simulation_with_curiosity(brain, world, agent, num_steps, curiosity_threshold=0.5):
    agent.enableCuriosity(True)
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell the brain
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks and explores based on curiosity
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Get reward and learn
        reward = world.getReward() if world.isEpisodeDone() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Curiosity-driven exploration
        if agent.getCuriosityLevel() > curiosity_threshold:
            # Add small random exploration to action
            action = agent.getExplorationAction()
            world.applyMotorCommand(action, world.getSimulationTime())
```

### Pattern 5: Complex World Interaction with Memory

```python
def run_simulation_with_memory(brain, world, agent, num_steps, replay_interval=100):
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what the agent sees
        percept = world.getSensoryPercept()
        
        # 3. Store percept in working memory
        agent.storeSensoryInput(percept)
        
        # 4. Process sensory input and get action
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # 5. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 6. Apply reward and store episode
        reward = world.getReward() if world.isEpisodeDone() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 7. Replay important episodes periodically
        if step % replay_interval == 0 and step > 0:
            agent.replayImportantEpisodes()
```

### Pattern 6: Custom Neural Network Configuration

```python
def create_advanced_brain():
    # Create custom configuration
    config = pynlm.createDefaultConfig()
    
    # Customize neural network parameters
    config.set("neuron_count", 2000)  # More neurons for complex tasks
    config.set("region_count", 3)      # Multiple brain regions
    config.set("connection_probability", 0.25)  # Higher connectivity
    config.set("simulation_timestep", 0.0001)     # Faster simulation
    
    # Enable advanced features
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    config.set("enable_prediction", True)
    
    # Create brain with custom config
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    return brain

def create_complex_agent():
    # Create brain with advanced capabilities
    brain = create_advanced_brain()
    world = pynlm.createSimpleWorld()
    
    # Configure world with rich sensory input
    world.configure(
        width=50, height=50,                # Large world
        visionWidth=16, visionHeight=16,    # Good visual range
        depthPerception=True,               # 3D vision
        auditoryRange=20.0,                 # Sound perception
        olfactoryRange=10.0                 # Smell perception
    )
    world.reset()
    
    # Create agent with all systems enabled
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    
    return brain, world, agent
```

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

### Project 4: Brain with Advanced Learning (Complete AI Agent)

```python
import pynlm

# Create advanced configuration
def create_advanced_config():
    config = pynlm.createDefaultConfig()
    
    # Advanced configuration for research-grade simulation
    config.set("neuron_count", 2000)
    config.set("region_count", 3)
    config.set("connection_probability", 0.15)
    config.set("simulation_timestep", 0.0001)
    
    # Enable all advanced learning systems
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    config.set("enable_prediction", True)
    
    return config

# Create world with advanced features

def create_advanced_world():
    world = pynlm.createSimpleWorld()
    world.configure(
        width=50, height=50,
        visionWidth=16, visionHeight=16,
        depthPerception=True,
        auditoryRange=20.0,
        olfactoryRange=10.0
    )
    return world

# Create sophisticated agent with all systems enabled

def create_advanced_agent(brain, world):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    
    return agent

# Advanced simulation with comprehensive learning metrics

def run_advanced_simulation(brain, world, agent, num_steps=500):
    print("Starting advanced simulation with full cognitive capabilities...")
    
    for step in range(num_steps):
        # World update
        world.update(0.1)
        
        # Sensory input and processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain computation
        brain.step(step)
        
        # Action selection and execution
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Reward-based learning
        reward = world.getReward() if world.isEpisodeDone() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Display progress
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.getAverageFiringRate():.2f} Hz, "
                  f"Memory: {agent.getWorkingMemorySize()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.2f}")
    
    return {
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_reward': brain.getTotalSpikeCount(),  # Using spike count as proxy
        'memory_traces': agent.getWorkingMemorySize(),
        'curiosity_level': agent.getCuriosityLevel()
    }

# Main execution example
if __name__ == "__main__":
    print("=== NLM Advanced AI Agent ===")
    print("Research-grade neural simulation with full cognitive capabilities\n")
    
    # Setup
    config = create_advanced_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = create_advanced_world()
    world.reset()
    
    agent = create_advanced_agent(brain, world)
    
    print(f"Brain initialized: {brain.getTotalNeuronCount()} neurons")
    print(f"World configured: {world.getWidth()}x{world.getHeight()} cells")
    print("All learning systems enabled: Reward, Plasticity, Development, Curiosity")
    print()
    
    # Run simulation
    results = run_advanced_simulation(brain, world, agent, 500)
    
    print("\n=== Advanced Agent Results ===")
    print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
    print(f"Total activity: {results['total_reward']} spike events")
    print(f"Working memory size: {results['memory_traces']} traces")
    print(f"Final curiosity level: {results['curiosity_level']:.2f}")
    
    print("\nAdvanced AI agent simulation completed successfully!")
```

### Project 7: Brain with Advanced Learning (Complete AI Agent)

```python
import pynlm
import random

# Create advanced configuration
def create_research_config():
    config = pynlm.createDefaultConfig()
    
    # Research-grade parameters
    config.set("neuron_count", 5000)           # Large brain for complex tasks
    config.set("region_count", 4)              # Multiple specialized regions
    config.set("connection_probability", 0.15)  # Balanced connectivity
    config.set("simulation_timestep", 0.0001)   # High precision
    
    # Learning systems
    config.set("enable_structural_plasticity", True)
    config.set("enable_development", True)
    config.set("enable_curiosity", True)
    config.set("enable_prediction", True)
    config.set("enable_working_memory", True)
    config.set("enable_episodic_memory", True)
    
    # Neuromodulation
    config.set("enable_dopamine", True)
    config.set("enable_prediction_error", True)
    config.set("enable_novelty", True)
    
    # Plasticity parameters
    config.set("stdp_ltp_weight", 0.02f)
    config.set("stdp_ltd_weight", 0.015f)
    config.set("stdp_tau", 30.0f)
    config.set("synaptogenesis_rate", 0.00005f)
    config.set("pruning_rate", 0.000005f)
    
    return config

# Create world with complex environment

def create_complex_world():
    world = pynlm.createSimpleWorld()
    world.configure(
        width=100, height=100,              # Large arena
        visionWidth=20, visionHeight=20,    # Good visual range
        depthPerception=True,               # 3D vision
        auditoryRange=50.0,                 # Long-range sound
        olfactoryRange=25.0,                # Medium-range smell
        tactileRange=15.0,                  # Touch perception
        thermalRange=20.0,                  # Temperature sensing
        magneticRange=100.0                # Magnetoreception
    )
    world.reset()
    return world

# Create sophisticated agent
def create_advanced_agent(brain, world):
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all advanced learning systems
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    agent.enablePrediction(True)
    agent.enableWorkingMemory(True)
    agent.enableEpisodicMemory(True)
    agent.enableDopamine(True)
    agent.enablePredictionError(True)
    agent.enableNovelty(True)
    
    return agent

# Advanced simulation with learning metrics

def run_advanced_simulation(brain, world, agent, num_steps=1000):
    # Performance tracking
    firing_rates = []
    reward_history = []
    memory_growth = []
    curiosity_levels = []
    
    for step in range(num_steps):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get current observation
        percept = world.getSensoryPercept()
        
        # 3. Process sensory input and store in memory
        agent.processSensoryInput(percept)
        agent.storeSensoryInput(percept)  # Working memory
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action
        action = agent.decodeMotorCommand()
        
        # 6. Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Apply reward and learn
        reward = world.getReward() if world.isEpisodeDone() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # 8. Apply curiosity-driven exploration
        if agent.getCuriosityLevel() > 0.7:
            exploration_action = agent.getExplorationAction()
            world.applyMotorCommand(exploration_action, world.getSimulationTime())
        
        # 9. Store episode if significant
        if reward != 0.0 or step % 100 == 0:
            agent.storeEpisodicMemory(reward, step)
        
        # 10. Periodic replay of important episodes
        if step % 500 == 0 and step > 0:
            agent.replayImportantEpisodes()
        
        # 11. Collect performance metrics
        firing_rates.append(brain.getAverageFiringRate())
        reward_history.append(reward)
        memory_growth.append(agent.getWorkingMemorySize())
        curiosity_levels.append(agent.getCuriosityLevel())
        
        # 12. Print progress every 100 steps
        if step % 100 == 0:
            avg_firing = sum(firing_rates[-100:]) / 100
            avg_reward = sum(reward_history[-100:]) / 100
            print(f"Step {step}:")
            print(f"  Firing rate: {avg_firing:.2f} Hz")
            print(f"  Avg reward: {avg_reward:.4f}")
            print(f"  Working memory: {agent.getWorkingMemorySize()} traces")
            print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
    
    # Return performance summary
    return {
        'final_firing_rate': brain.getAverageFiringRate(),
        'total_reward': sum(reward_history),
        'avg_reward_per_step': sum(reward_history) / num_steps,
        'max_memory_traces': max(memory_growth),
        'final_curiosity': curiosity_levels[-1],
        'episodes_stored': agent.getEpisodicMemoryCount()
    }

# Main advanced agent example
if __name__ == "__main__":
    print("=== NLM Advanced AI Agent ===")
    print("Research-grade neural simulation with full cognitive capabilities\n")
    
    # Create everything
    config = create_research_config()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = create_complex_world()
    
    agent = create_advanced_agent(brain, world)
    
    print("Starting advanced simulation...")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons")
    print(f"World: {world.getWidth()}x{world.getHeight()} cells")
    print("Learning systems enabled: Reward, Structural Plasticity, Development,")
    print("                           Curiosity, Prediction, Memory Systems")
    print()
    
    # Run simulation
    results = run_advanced_simulation(brain, world, agent, 1000)
    
    print("\n=== Simulation Results ===")
    print(f"Final firing rate: {results['final_firing_rate']:.2f} Hz")
    print(f"Total reward: {results['total_reward']:.4f}")
    print(f"Average reward per step: {results['avg_reward_per_step']:.6f}")
    print(f"Maximum memory traces: {results['max_memory_traces']}")
    print(f"Final curiosity level: {results['final_curiosity']:.2f}")
    print(f"Episodic memories stored: {results['episodes_stored']}")
    
    print("\nAdvanced AI agent simulation complete!")
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
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
