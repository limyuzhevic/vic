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

## Advanced Patterns

### Pattern 4: Custom Training Loop with Evaluation

```python
import pynlm
import time

def train_brain(world, agent, num_steps, evaluation_interval=100):
    """Custom training loop with periodic evaluation."""
    
    # Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    start_time = time.time()
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Compute reward from world state
        reward = compute_reward(world, agent)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Evaluate periodically
        if step % evaluation_interval == 0 and step > 0:
            evaluate_brain_performance(world, agent, step)
    
    total_time = time.time() - start_time
    print(f"Training completed in {total_time:.2f} seconds")
    return total_time

def compute_reward(world, agent):
    """Custom reward function for the agent."""
    # Example: reward for collecting resources, penalty for damage
    percept = world.getSensoryPercept()
    reward = 0.0
    
    # Reward for proximity to resources
    if percept.getTouch():
        for x in range(len(percept.getTouch())):
            if percept.getTouch()[x] > 0.5:
                reward += 0.1
    
    # Penalty for staying in hazardous areas
    if percept.getInternal():
        hazard_level = sum(1 for signal in percept.getInternal() if signal > 0.7)
        reward -= hazard_level * 0.2
    
    return max(0.0, reward)
```

### Pattern 5: Episodic Memory and Replay

```python
import pynlm

def run_learning_episode(world, agent, episode_length=500):
    """Run an episode with episodic memory recording."""
    
    # Create a custom world with memory features
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
    
    # Initialize world and agent
    world.reset()
    world.setAgentStart(7.5, 7.5)
    
    # Enable memory features
    # Note: In the current NLM, memory is internal to the brain
    # This example shows how to work with the brain's memory systems
    
    for step in range(episode_length):
        # Store current brain state in brain's episodic memory
        # (automatically handled by brain's internal memory systems)
        
        # Update world
        world.update(0.1)
        
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = compute_reward(world, agent)
        agent.applyRewardModulation(reward, 0.0)
        
        # Log memory statistics periodically
        if step % 100 == 0:
            print(f"Step {step}: Episodes stored = {brain.getEpisodicMemory().getEpisodeCount()}")
    
    # Return final memory statistics
    return {
        'episodes_stored': brain.getEpisodicMemory().getEpisodeCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
        'development_stage': brain.getDevelopmentalStage()
    }
```

### Pattern 6: Multi-Agent Environment

```python
import pynlm
from threading import Thread
import queue

def create_multi_agent_environment(agent_configs):
    """Create an environment with multiple agents."""
    agents = []
    worlds = []
    
    for config in agent_configs:
        # Create shared world for all agents
        world = pynlm.createSimpleWorld()
        world.configure(width=config['world_size'], height=config['world_size'],
                       visionWidth=6, visionHeight=6)
        world.reset()
        world.setRandomSeed(config.get('seed', 42))
        
        # Place agent at random position
        start_x = config['start_x']
        start_y = config['start_y']
        world.setAgentStart(start_x, start_y)
        
        # Create brain
        brain_config = pynlm.createDefaultConfig()
        brain_config.set("brain.neuron_count", config.get('neuron_count', 500))
        brain_config.set("brain.region_count", config.get('region_count', 1))
        
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Configure agent
        agent.enableRewardModulation(config.get('enable_reward_modulation', True))
        agent.enableCuriosity(config.get('enable_curiosity', True))
        agent.enableDevelopment(config.get('enable_development', True))
        
        agents.append(agent)
        worlds.append(world)
    
    return agents, worlds

def run_multi_agent_episode(agents, worlds, num_steps):
    """Run an episode with multiple interacting agents."""
    
    for step in range(num_steps):
        # Update all worlds
        for i, world in enumerate(worlds):
            world.update(0.1)
        
        # Each agent processes its own perception
        for i, (agent, world) in enumerate(zip(agents, worlds)):
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Run brain step
            agent.getBrain().step(step)
            
            # Get action and apply
            motor_cmd = agent.decodeMotorCommand()
            world.applyMotorCommand(motor_cmd, world.getSimulationTime())
            
            # Apply reward (agents can affect each other)
            reward = compute_multi_agent_reward(i, agents, worlds)
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(0.1)
        
        # Log statistics
        if step % 100 == 0:
            print(f"Multi-agent step {step}")
            for i, agent in enumerate(agents):
                brain = agent.getBrain()
                print(f"  Agent {i}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"curiosity={agent.getCuriosityLevel():.3f}")

def compute_multi_agent_reward(agent_idx, agents, worlds):
    """Compute reward considering multi-agent interactions."""
    world = worlds[agent_idx]
    
    # Base reward from resources in the world
    base_reward = 0.0
    percept = world.getSensoryPercept()
    if percept.getTouch():
        base_reward += sum(1 for signal in percept.getTouch() if signal > 0.3)
    
    # Penalty for collisions with other agents
    collision_penalty = 0.0
    agent_body = world.getAgentBody()
    
    # Check proximity to other agents (simplified)
    for i, other_world in enumerate(worlds):
        if i != agent_idx:
            other_body = other_world.getAgentBody()
            distance = ((agent_body.x - other_body.x) ** 2 + 
                       (agent_body.y - other_body.y) ** 2) ** 0.5
            if distance < 1.0:  # Collision threshold
                collision_penalty -= 1.0
    
    # Curiosity bonus for novel experiences
    curiosity_bonus = agents[agent_idx].getCuriosityLevel() * 0.5
    
    return base_reward + collision_penalty + curiosity_bonus
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
3. Experiment with different configurations!

That's it! You're now ready to use NLM.
