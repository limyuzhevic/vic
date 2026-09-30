# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

NLM is NOT:
- A transformer or LLM
- A chatbot
- A deep learning model
- A pretrained AI system

NLM IS intended to become:
- A neural system that learns from experience
- A brain-inspired architecture with neurons and synapses
- A system that develops and adapts over time
- A system where cognition emerges from neural dynamics

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

### Example 3: Enhanced Brain with Advanced Features

```python
import pynlm

# Create a brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("random_seed", 42)

brain = pynlm.createBrain(config)
brain.initialize()

# Enable advanced learning features
agent = pynlm.createAgentBrain(brain)

# Enable all advanced learning mechanisms
agent.enableRewardModulation(True)    # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Add some resources to collect
pynlm.addResourceToWorld(world, 5.0, 5.0, 10.0)
pynlm.addResourceToWorld(world, 15.0, 10.0, 5.0)
pynlm.addHazardToWorld(world, 2.0, 8.0, -3.0)

# Run a learning simulation
print("Running learning simulation...")
for episode in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(episode)
    
    action = agent.decodeMotorCommand()
    result = world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward with prediction
    agent.applyRewardModulation(result.reward, agent.getPredictionError())
    
    if episode % 20 == 0:
        print(f"Episode {episode}: {brain.getFiringNeuronCount()} neurons firing, Reward: {result.reward}")

print("Learning simulation complete!")
```

---

## Advanced Usage Examples

### Pattern 1: Running Multiple Simulations

```python
def run_multiple_simulations(num_simulations, steps_per_simulation):
    results = []
    
    for sim in range(num_simulations):
        # Create fresh brain for each simulation
        brain = pynlm.createDemoBrain()
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for step in range(steps_per_simulation):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        # Collect statistics
        stats = brain.getStatistics()
        results.append({
            'simulation': sim,
            'steps': steps_per_simulation,
            'total_spikes': stats['total_spikes'],
            'neurons_firing': stats['active_neurons'],
            'learning_progress': sim * 0.1
        })
    
    return results

# Run multiple simulations
simulation_results = run_multiple_simulations(5, 200)
for result in simulation_results:
    print(f"Simulation {result['simulation']}: {result['total_spikes']} spikes, "
          f"{result['neurons_firing']} active neurons")
```

### Pattern 2: Training with Rewards

```python
def train_with_rewards(num_episodes, learning_rate=0.01):
    # Create agent with reward learning enabled
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Add training resources
    pynlm.addResourceToWorld(world, 3.0, 3.0, 20.0)
    pynlm.addResourceToWorld(world, 7.0, 7.0, 15.0)
    
    training_history = []
    
    for episode in range(num_episodes):
        episode_reward = 0.0
        
        for step in range(50):  # 5 seconds at 0.1s per step
            # Get current observation
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Make decision
            action = agent.decodeMotorCommand()
            
            # Execute action and get reward
            result = world.applyMotorCommand(action, world.getSimulationTime())
            
            # Learn from reward
            predicted_reward = agent.getPredictionError()
            agent.applyRewardModulation(result.reward, predicted_reward)
            
            episode_reward += result.reward
            
            if step % 10 == 0:
                print(f"  Step {step}: Action {action}, Reward {result.reward}")
        
        # Record episode
        training_history.append({
            'episode': episode,
            'total_reward': episode_reward,
            'average_reward': episode_reward / 50.0,
            'neuromodulation_level': agent.getNeuromodulationLevel(),
            'curiosity_level': agent.getCuriosityLevel()
        })
        
        print(f"Episode {episode}: Total reward = {episode_reward:.2f}")
    
    return training_history

# Run training
training_data = train_with_rewards(20)
print(f"Training complete! Average reward: {sum(h['average_reward'] for h in training_data) / len(training_data):.3f}")
```

### Pattern 3: Configuration Management

```python
def createOptimizedConfig(neurons, regions, learning_rate):
    """Create a configuration optimized for specific tasks"""
    config = {
        "neuron_count": neurons,
        "region_count": regions,
        "connection_probability": 0.15,
        "random_seed": 42,
        "plasticity_learning_rate": learning_rate,
        "stdp_ltp_weight": learning_rate * 0.8,
        "stdp_ltd_weight": learning_rate * 0.9,
        "simulation_timestep": 0.001,
        "max_simulation_steps": 10000
    }
    
    return config

def createConfigFromEnvironment(env_type="urban", complexity="medium"):
    """Create configuration based on environment type"""
    base_config = pynlm.createDefaultConfig()
    
    if env_type == "urban":
        base_config.set("neuron_count", 2000)
        base_config.set("region_count", 3)
        base_config.set("connection_probability", 0.2)
    elif env_type == "rural":
        base_config.set("neuron_count", 500)
        base_config.set("region_count", 1)
        base_config.set("connection_probability", 0.1)
    elif env_type == "space":
        base_config.set("neuron_count", 1500)
        base_config.set("region_count", 2)
        base_config.set("connection_probability", 0.12)
    
    if complexity == "high":
        base_config.set("plasticity_learning_rate", 0.05)
    elif complexity == "low":
        base_config.set("plasticity_learning_rate", 0.005)
    
    return base_config

# Usage examples
urban_config = createConfigFromEnvironment("urban", "high")
rural_config = createConfigFromEnvironment("rural", "medium")
space_config = createConfigFromEnvironment("space", "low")

print("Environment configurations created successfully!")
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
print("Neuromodulation level:", agent.getNeuromodulationLevel())
print("Curiosity level:", agent.getCuriosityLevel())
```

### Pattern 3: Enable Brain Features

```python
# These are all optional:
agent.enableRewardModulation(True)      # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)           # Brain matures over time
agent.enableCuriosity(True)             # Explore new things

# Enable specific plasticity mechanisms
# brain.getSTDP().enablePlasticity(True)
# brain.getHebbian().enablePlasticity(True)
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

### `brain.getStatistics()`
Gets comprehensive brain statistics as a dictionary with detailed metrics.

### `pynlm.createDemoBrain()`
Creates a pre-configured brain suitable for demonstrations and testing.

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
print(f"Brain has {brain.getTotalNeuronCount()} neurons")
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

# Add training resources
pynlm.addResourceToWorld(world, 5.0, 5.0, 10.0)
pynlm.addResourceToWorld(world, 10.0, 10.0, 15.0)

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
print(f"Final stats: {brain.getTotalSpikeCount()} total spikes")
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
| Get brain stats | `brain.getStatistics()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Add resource | `pynlm.addResourceToWorld(world, x, y)` |
| Add hazard | `pynlm.addHazardToWorld(world, x, y)` |
| Get advanced config | `pynlm.createDemoBrain()` |

---

## Troubleshooting

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if the brain has enough neurons and regions
- Enable reward modulation for learning behavior

**"The agent isn't moving"**
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify the agent has motor neurons
- Make sure the agent is in a valid state

**"Everything is 0"**
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if you need to enable specific features like reward modulation

**"My simulation is too slow"**
- Try reducing the number of neurons
- Use `pynlm.createTestBrain()` for smaller, faster simulations
- Consider using the command-line interface for performance benchmarking
- Check if you can reduce simulation steps

**"Learning isn't working"**
- Make sure `enableRewardModulation(True)` is called
- Check that `enableCuriosity(True)` is set for exploration
- Verify that rewards are being applied correctly
- Try adjusting learning rates

---

## Next Steps

When you're comfortable:
1. Read the source code to understand how NLM works
2. Experiment with different configurations!
3. Try the command-line interface for advanced control
4. Look at the documentation in the `docs/` folder
5. Create your own simulation scenarios
6. Contribute to the NLM project!

That's it! You're now ready to use NLM with all its enhanced features.
