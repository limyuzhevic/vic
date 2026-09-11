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

### Example 1.5: Interactive Python Session

```python
import pynlm

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
# Configure brain parameters (uncomment to customize)
# config.set("brain.neuron_count", 500)
# config.set("brain.synapse_density", 0.05)
# config.set("plasticity.stdp.enable", True)

brain = pynlm.createBrain(config)
brain.initialize()

print("Brain created successfully!")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run a simulation
for step in range(50):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
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

### Example 2.5: Advanced Configuration Example

```python
import pynlm

# Create a brain with custom configuration
config = pynlm.createDefaultConfig()

# Configure specific parameters for better learning
config.set("brain.neuron_count", 2000)
config.set("brain.synapse_density", 0.05)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.structural.enable", True)
config.set("neuromod.dopamine.scale", 2.0)
config.set("neuromod.curiosity.enable", True)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=16, visionHeight=16)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

print("Advanced brain created with custom configuration!")
print(f"Neuron count: {brain.getTotalNeuronCount()}")
print(f"Initial development stage: {brain.getDevelopmentalStage()}")
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
    """Run a complete agent simulation with brain and world."""
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

### Project 3: Complete Agent with Learning

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

### Project 4: Developmental Learning

```python
import pynlm
import time

# Create brain with developmental system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all developmental features
agent.enableDevelopment(True)
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableCuriosity(True)

print("=== Developmental Learning Simulation ===")
print(f"Initial stage: {brain.getDevelopmentalStage()}")

# Run simulation with developmental tracking
development_stages = []
for step in range(200):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Track development
    if step % 20 == 0:
        current_stage = brain.getDevelopmentalStage()
        development_stages.append(current_stage)
        print(f"Step {step}: Stage {current_stage}, "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}")

print("=== Development Complete ===")
print(f"Stage progression: {development_stages}")
print(f"Final stage: {brain.getDevelopmentalStage()}")
print(f"Memory episodes: {brain.getEpisodicMemory().getEpisodeCount() if brain.getEpisodicMemory() else 0}")
```

---

## Performance Optimization Tips

### Pattern 7: Performance Monitoring

```python
def monitor_performance(brain, world, agent, num_steps, log_interval=100):
    """Monitor and log performance metrics during simulation."""
    import time
    
    start_time = time.time()
    metrics = {
        'steps': [],
        'firing_rates': [],
        'spike_counts': [],
        'curiosity_levels': [],
        'novelty_levels': [],
        'rewards': []
    }
    
    for step in range(num_steps):
        step_start = time.time()
        
        # Run simulation step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log reward
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Collect metrics
        metrics['steps'].append(step)
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        metrics['spike_counts'].append(brain.getTotalSpikeCount())
        metrics['curiosity_levels'].append(agent.getCuriosityLevel())
        metrics['novelty_levels'].append(agent.getNoveltyLevel())
        metrics['rewards'].append(reward)
        
        # Log performance summary
        if step % log_interval == 0 or step == num_steps - 1:
            step_time = time.time() - step_start
            avg_step_time = (time.time() - start_time) / (step + 1)
            
            print(f"Step {step}/{num_steps}:")
            print(f"  Performance: {step_time*1000:.2f}ms/step, "
                  f"{avg_step_time*1000:.2f}ms avg")
            print(f"  Neural stats: {brain.getFiringNeuronCount()} firing, "
                  f"{brain.getAverageFiringRate():.3f} Hz avg")
            print(f"  Behavior: Curiosity {agent.getCuriosityLevel():.3f}, "
                  f"Novelty {agent.getNoveltyLevel():.3f}")
            print(f"  Reward: {reward:.3f}")
            print()
    
    # Calculate summary statistics
    total_time = time.time() - start_time
    metrics_summary = {
        'total_time': total_time,
        'steps_per_second': num_steps / total_time,
        'avg_firing_rate': sum(metrics['firing_rates']) / len(metrics['firing_rates']),
        'max_firing_rate': max(metrics['firing_rates']),
        'total_reward': sum(metrics['rewards']),
        'avg_reward': sum(metrics['rewards']) / len(metrics['rewards']),
        'final_curiosity': metrics['curiosity_levels'][-1],
        'final_novelty': metrics['novelty_levels'][-1]
    }
    
    print("=== Performance Summary ===")
    print(f"Total simulation time: {total_time:.2f}s")
    print(f"Steps per second: {metrics_summary['steps_per_second']:.2f}")
    print(f"Average firing rate: {metrics_summary['avg_firing_rate']:.3f} Hz")
    print(f"Maximum firing rate: {metrics_summary['max_firing_rate']:.3f} Hz")
    print(f"Total reward: {metrics_summary['total_reward']:.3f}")
    print(f"Average reward per step: {metrics_summary['avg_reward']:.3f}")
    
    return metrics, metrics_summary
```

### Pattern 8: Efficient World Management

```python
def run_efficient_simulation(brain, world, agent, num_steps, use_checkpoints=False):
    """Run efficient simulation with optional checkpointing and optimization."""
    import os
    import time
    
    if use_checkpoints:
        print("Using checkpointing for efficient simulation...")
        # Create checkpoint directory
        os.makedirs('/tmp/nlm_checkpoints', exist_ok=True)
        
        # Run simulation with periodic saving
        checkpoint_interval = max(1, num_steps // 10)
        
        for step in range(num_steps):
            # Check if we need to save checkpoint
            if step % checkpoint_interval == 0 and step > 0:
                checkpoint_path = f'/tmp/nlm_checkpoints/step_{step}.bin'
                brain.save(checkpoint_path)
                print(f"Checkpoint saved: {checkpoint_path}")
            
            # Run simulation (optimized version)
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation (simplified)
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Final checkpoint
        brain.save('/tmp/nlm_checkpoints/final_state.bin')
        print("Final checkpoint saved.")
        
    else:
        print("Running simulation without checkpointing...")
        # Original simulation code
        for step in range(num_steps):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
    
    return brain
```

## Advanced Patterns

### Pattern 4: Multi-agent Simulation

```python
def run_multi_agent_simulation(brains, worlds, agents, num_steps):
    """Run multiple agent simulations in parallel."""
    for step in range(num_steps):
        for i, (brain, world, agent) in enumerate(zip(brains, worlds, agents)):
            # Each agent has its own world
            world.update(0.1)
            
            # Shared perceptual input
            percept = world.getSensoryPercept()
            
            # Each agent processes it differently
            agent.processSensoryInput(percept)
            
            # All brains step together (synchronous)
            brain.step(step)
            
            # Each agent makes its own decision
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Each agent can have different learning rates
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)

# Example usage:
brains = [pynlm.createBrain(pynlm.createDefaultConfig()) for _ in range(3)]
worlds = [pynlm.createSimpleWorld() for _ in range(3)]
agents = [pynlm.createAgentBrain(brain) for brain in brains]

# Configure each world differently
for i, world in enumerate(worlds):
    world.configure(width=15, height=15, visionWidth=10, visionHeight=10)
    world.reset()
    world.setAgentStart(i * 5, i * 5)

for agent in agents:
    agent.initialize(worlds[agents.index(agent)])

run_multi_agent_simulation(brains, worlds, agents, 500)
```

### Pattern 5: Developmental Learning with Checkpointing

```python
def run_developmental_simulation_with_checkpoints(num_episodes=10, steps_per_episode=1000):
    """Run developmental simulation with checkpoint saving and loading."""
    import os
    
    all_checkpoints = []
    
    for episode in range(num_episodes):
        print(f"\n=== Episode {episode + 1}/{num_episodes} ===")
        
        # Create fresh configuration for each episode
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        config.set("brain.synapse_density", 0.05)
        
        # Create brain with development enabled
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=30, height=30, visionWidth=12, visionHeight=12)
        world.reset()
        world.setAgentStart(15.0, 15.0)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable all developmental features
        agent.enableDevelopment(True)
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableCuriosity(True)
        
        # Track development
        development_stages = []
        curiosity_levels = []
        
        # Run episode
        for step in range(steps_per_episode):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
            
            # Record metrics
            if step % 100 == 0:
                development_stages.append(brain.getDevelopmentalStage())
                curiosity_levels.append(agent.getCuriosityLevel())
                print(f"Step {step}: Stage {brain.getDevelopmentalStage()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}")
        
        # Save checkpoint at end of episode
        checkpoint_path = f"/tmp/nlm_episode_{episode}.bin"
        brain.save(checkpoint_path)
        all_checkpoints.append(checkpoint_path)
        
        print(f"Episode {episode + 1} complete. Checkpoint saved to {checkpoint_path}")
    
    # Load and compare last two checkpoints
    if len(all_checkpoints) >= 2:
        print("\n=== Comparing checkpoints ===")
        checkpoint1 = all_checkpoints[-2]
        checkpoint2 = all_checkpoints[-1]
        
        # Load into comparison brains
        config = pynlm.createDefaultConfig()
        brain1 = pynlm.createBrain(config)
        brain1.initialize()
        brain1.load(checkpoint1)
        
        brain2 = pynlm.createBrain(config)
        brain2.initialize()
        brain2.load(checkpoint2)
        
        print(f"Brain1 final stage: {brain1.getDevelopmentalStage()}")
        print(f"Brain2 final stage: {brain2.getDevelopmentalStage()}")
        print(f"Brain1 total spikes: {brain1.getTotalSpikeCount()}")
        print(f"Brain2 total spikes: {brain2.getTotalSpikeCount()}")
    
    return all_checkpoints
```
