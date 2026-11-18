# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental computational brain project that simulates artificial intelligence through neural network principles. It provides a Python interface to create virtual brains that can learn, think, and interact with environments.

Think of it like this:
- **Neurons** = Brain cells that process information
- **Synapses** = Connections between brain cells
- **Learning** = When connections strengthen or weaken based on experience
- **Memory** = The brain remembering patterns and experiences

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks and processes
2. **World** - The environment the brain lives in and interacts with
3. **Agent** - The bridge connecting brain to world actions

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain

```python
import nlm

# Step 1: Make a brain
brain = nlm.PythonBrainWrapper(nlm.PythonFactory.createBrain(nlm.PythonFactory.createDefaultConfig()))

# Step 2: Turn it on
brain.initialize()

# Step 3: Make it think for 100 steps
for i in range(100):
    brain.step(i)  # One step of thinking

print("Done! Your brain thought", brain.getStats()["total_spikes"], "times")
```

### Example 2: Brain in a Simple World

```python
import nlm

# Create everything using factory helpers
config = nlm.PythonFactory.createDefaultConfig()
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=20, height=20, visionWidth=8, visionHeight=8)
agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableLearning(True)

# Run for 50 steps
for step in range(50):
    world.update(0.1)  # Update world
    agent.processObservations([("vision", [0.5, 0.5, 0.8])])  # What does the agent see?
    brain.step(step)  # Brain thinks
    action = agent.generateAction()  # Brain decides action
    world.applyMotorCommand(action, world.getSimulationTime())  # Do action

print("Simulation finished!")
print("Neurons firing:", brain.getStats()["firing_neurons"])
```

### Example 3: Advanced Agent with Learning

```python
import nlm
import random

# Setup with enhanced configuration
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=500, regionCount=3, connectionProbability=0.15)
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=30, height=30, visionWidth=10, visionHeight=10)
agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning subsystems
agent.enableLearning(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run complex agent simulation
for step in range(200):
    # Generate sensory observations
    observations = []
    if step % 5 == 0:
        observations.append(("vision", [random.random() for _ in range(30)]))
    if step % 10 == 0:
        observations.append(("internal", [random.gauss(0, 1) for _ in range(5)]))

    # Process and think
    agent.processObservations(observations)
    brain.step(step)
    action = agent.generateAction("exploratory")
    world.applyMotorCommand(action, world.getSimulationTime())

    # Learn from experience
    reward = 1.0 if "resource" in action.lower() else 0.0
    agent.learnFromExperience(reward, step * 0.1)

    if step % 50 == 0:
        print(f"Step {step}: {brain.getStats()[\"firing_neurons\"]} neurons firing")
        print(f"State: curiosity={agent.getState()[\"curiosity\"]:.2f}, novelty={agent.getState()[\"novelty\"]:.2f}")

print("Advanced agent simulation complete!")
print("Final stats:", brain.getStats())
```

## Enhanced API Features

### 1. PythonBrainWrapper - High-Level Interface

The `PythonBrainWrapper` class provides a more intuitive Python interface:

```python
brain = nlm.PythonBrainWrapper(brain)  # Wrap existing brain
brain.initialize()                      # Start the brain
brain.step(5, 0.1)                      # Step with time
brain.getStats()                        # Get brain stats as dict
brain.getFiringRate()                   # Convenience method
brain.getExcitatoryInhibitionRatio()   # E/I balance
brain.saveCheckpoint("model.pkl")       # Save with metadata
```

### 2. Enhanced AgentBrain

```python
agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)

# Process observations from dictionary
observations = {
    "vision": [0.1, 0.5, 0.9],
    "internal": [0.0, 0.1, -0.2],
    "audio": [0.3, 0.7]
}
agent.processObservations(observations)

# Generate action with strategy
action = agent.generateAction("conservative")

# Get comprehensive state
state = agent.getState()
```

### 3. SimulationRunner - Advanced Simulation

```python
runner = nlm.SimulationRunner()
runner.initialize(brain, world, agent)
runner.setLearningMode(True)

results = runner.runWithLearning(1000, verbose=True)
print(f"Average reward: {results['average_reward']}")
print(f"Average firing rate: {results['average_firing_rate']}")
```

### 4. VisualizationHelper - Brain Analysis

```python
brainGraph = nlm.VisualizationHelper.createBrainGraph(brain)
# Returns dict with neurons and synapses for visualization
```

## Configuration Management

### Using Factory Functions

```python
# Create and configure with factory
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=1000, regionCount=2)

# Or configure manually
config.set("neuron_count", 1000)
config.set("connection_probability", 0.1)
config.set("plasticity_learning_rate", 0.01)
```

### Environment Setup

```python
world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=20, height=20, visionWidth=8, visionHeight=8)
```

## Common Patterns

### Pattern 1: Learning Environment Simulation

```python
def run_learning_simulation(config, world_config, num_steps=1000):
    # Setup
    config = nlm.PythonFactory.createDefaultConfig()
    nlm.PythonFactory.configureBrain(config, neuronCount=500)
    brain = nlm.PythonFactory.createBrain(config)
    brain.initialize()

    world = nlm.PythonFactory.createWorld()
    nlm.PythonFactory.setupEnvironment(world, **world_config)
    agent = nlm.PythonFactory.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableLearning(True)

    # Training loop
    for step in range(num_steps):
        # Get observations
        observations = get_observations(world, step)
        
        # Process and act
        agent.processObservations(observations)
        brain.step(step)
        action = agent.generateAction()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Learn
        reward = calculate_reward(world, action)
        agent.learnFromExperience(reward, step * 0.1)
        
        # Monitor progress
        if step % 100 == 0:
            stats = brain.getStats()
            print(f"Step {step}: Reward={reward:.3f}, Firing={stats['firing_neurons']}")
    
    return brain, agent, world
```

### Pattern 2: Model Checkpointing

```python
# Save model with metadata
def save_model(brain, path, metadata=None):
    brain_wrapper = nlm.PythonBrainWrapper(brain)
    brain_wrapper.saveCheckpoint(path, metadata or "Training model")

# Load model
def load_model(path):
    # Implementation would load the brain
    pass

# Experiment tracking
def run_experiment(experiment_name, config, num_runs=10):
    results = []
    for run in range(num_runs):
        brain, agent, world = run_learning_simulation(config)
        brain_wrapper = nlm.PythonBrainWrapper(brain)
        results.append({
            "run": run,
            "final_spikes": brain.getStats()["total_spikes"],
            "final_firing_rate": brain.getStats()["average_firing_rate"],
            "mean_reward": calculate_mean_reward(agent)
        })
        save_model(brain, f"models/{experiment_name}_run{run}.pkl")
    
    return results
```

### Pattern 3: Multiple Environments

```python
# Train agent in different environments
def train_multienv(agent_config, environments, num_steps_per_env=500):
    agent_results = {}
    
    for env_name, env_config in environments.items():
        print(f"Training in {env_name}...")
        
        # Setup environment
        world = nlm.PythonFactory.createWorld()
        nlm.PythonFactory.setupEnvironment(world, **env_config)
        
        # Create agent for this environment
        brain = nlm.PythonFactory.createBrain(agent_config)
        brain.initialize()
        agent = nlm.PythonFactory.createAgentBrain(brain)
        agent.initialize(world)
        
        # Train
        for step in range(num_steps_per_env):
            # Environment-specific observations
            observations = generate_env_observations(env_name, world, step)
            
            # Learn
            agent.processObservations(observations)
            brain.step(step)
            action = agent.generateAction()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Get reward
            reward = get_env_reward(env_name, world, action)
            agent.learnFromExperience(reward, step * 0.1)
        
        agent_results[env_name] = {
            "final_stats": brain.getStats(),
            "final_agent_state": agent.getState()
        }
    
    return agent_results
```

## Mini Projects

### Mini Project 1: Silent Brain Analysis

```python
import nlm
import matplotlib.pyplot as plt

brain = nlm.PythonFactory.createBrain(nlm.PythonFactory.createDefaultConfig())
brain.initialize()

# Record baseline
baseline_stats = brain.getStats()

# Run analysis
for i in range(10):
    brain.step(i)

final_stats = brain.getStats()

print("Silent brain analysis:")
print(f"Initial spikes: {baseline_stats['total_spikes']}")
print(f"Final spikes: {final_stats['total_spikes']}")
print(f"Net spike generation: {final_stats['total_spikes'] - baseline_stats['total_spikes']}")
```

### Mini Project 2: World Viewer

```python
import nlm
import random

# Setup viewing environment
brain = nlm.PythonFactory.createBrain(nlm.PythonFactory.createDefaultConfig())
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=20, height=20, visionWidth=5, visionHeight=5)
agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)

print("Watching world for 30 steps:")
for i in range(30):
    world.update(0.1)
    agent.processObservations([("vision", [random.random() for _ in range(25)])])
    brain.step(i)
    
    if i % 10 == 0:
        print(f"Step {i}: {brain.getStats()['firing_neurons']} neurons firing")
        print(f"Firing rate: {brain.getStats()['average_firing_rate']:.3f}")
```

### Mini Project 3: Complete Learning Agent

```python
import nlm
import random

# Complete setup
config = nlm.PythonFactory.createDefaultConfig()
nlm.PythonFactory.configureBrain(config, neuronCount=300, regionCount=2)
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()

world = nlm.PythonFactory.createWorld()
nlm.PythonFactory.setupEnvironment(world, width=15, height=15, visionWidth=8, visionHeight=8)
agent = nlm.PythonFactory.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning
agent.enableLearning(True)
agent.enableCuriosity(True)

print("Running complete agent for 100 steps:")
for step in range(100):
    # Get sensory input
    vision = [random.random() for _ in range(64)]
    agent.processObservations([("vision", vision)])
    
    # Think and act
    brain.step(step)
    action = agent.generateAction()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Learn from experience
    reward = random.random() * 2.0 - 1.0  # Random reward between -1 and 1
    agent.learnFromExperience(reward, step * 0.1)
    
    # Monitor
    if step % 20 == 0:
        state = agent.getState()
        print(f"Step {step}: {action} | Curiosity={state['curiosity']:.2f}, Novelty={state['novelty']:.2f}")
        print(f"           Reward={reward:.2f}, Firing={brain.getStats()['firing_neurons']}")

print("Agent simulation complete!")
print("Final brain stats:", brain.getStats())
print("Final agent state:", agent.getState())
```

## Quick Reference

| What you want | Enhanced Code |
|--------------|---------------|
| Create brain | `nlm.PythonFactory.createBrain(nlm.PythonFactory.createDefaultConfig())` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `nlm.PythonFactory.createWorld()` |
| Create agent | `nlm.PythonFactory.createAgentBrain(brain)` |
| Get brain stats | `brain.getStats()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.generateAction()` |
| Process observations | `agent.processObservations(observations)` |
| Learn from experience | `agent.learnFromExperience(reward, time)` |
| Save model | `brain.saveCheckpoint("path.pkl", "metadata")` |

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if learning is enabled: `agent.enableLearning(True)`

### "The agent isn't learning"
- Enable learning: `agent.enableLearning(True)`
- Make sure you're calling `agent.learnFromExperience()`
- Check agent's curiosity level: `agent.getState()["curiosity"]`

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if observation data is being provided

## Next Steps

When you're comfortable:
1. Read `HOW_TO_USE.md` for more detailed information
2. Read `docs/ARCHITECTURE.md` to understand how it all works
3. Experiment with different configurations!
4. Try `SimulationRunner` for complex simulations
5. Use `VisualizationHelper` for brain analysis
6. Build custom `Observation` classes for your specific needs

That's it! You're now ready to use NLM with the enhanced Python API.