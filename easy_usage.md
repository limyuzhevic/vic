# NLM - Enhanced Easy Usage Guide

## What is NLM?

NLM (熙然, meaning "serene flow") is an **experimental artificial developmental brain simulator** for computers. It lets you create virtual brains that can:

- **Learn** from experience through synaptic plasticity
- **Remember** patterns through memory systems  
- **Make decisions** through competitive attention and action selection
- **Develop** over time through developmental processes
- **Adapt** to changing environments through neuromodulation

Think of it like this:
- **Neurons** = Brain cells that send electrical signals
- **Synapses** = Connections between brain cells that can strengthen or weaken
- **Learning** = When connections change based on experience (STDP, Hebbian learning)
- **Memory** = The brain storing patterns for later use

## Quick Start (Copy & Paste)

### Example 1: Basic Brain with Learning

```python
import pynlm

# Step 1: Create a brain with enhanced configuration
config = pynlm.createDefaultConfig()
config.set_value("neuron_count", 1000)
config.set_value("random_seed", 42)

brain = pynlm.createBrain(config)
brain.initialize()

# Step 2: Enable advanced learning features
brain.enableRewardModulation(True)
brain.enableCuriosity(True)
brain.enableDevelopment(True)

# Step 3: Run learning simulation
for step in range(1000):
    brain.step(step)
    
print("Learning simulation complete!")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()} Hz")
```

### Example 2: Complete Agent in a World

```python
import pynlm

# Setup the complete agent-environment system
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create the environment
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=10, visionHeight=10)
world.reset()

# Create the agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True) 
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run the complete simulation
for step in range(500):
    world.update(0.1)
    
    # Get what the agent perceives
    percept = world.getSensoryPercept()
    
    # Tell the brain what it perceives
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Brain decides on an action
    action = agent.decodeMotorCommand()
    
    # Execute the action in the world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 50 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Agent simulation complete!")
```

### Example 3: Advanced Batch Processing

```python
import pynlm

# Run multiple experiments efficiently
def run_batch_experiment(config_template, num_experiments=10):
    results = []
    
    for i in range(num_experiments):
        # Create configuration for each experiment
        config = pynlm.createDefaultConfig()
        
        # Modify parameters for this experiment
        config.set_value("neuron_count", config_template.get_value("neuron_count", 1000))
        config.set_value("random_seed", i + 1000)  # Different seed for each
        config.set_value("connection_probability", 0.15)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Enhanced simulation with learning
        for step in range(500):
            brain.step(step)
        
        # Collect results
        result = {
            "experiment_id": i,
            "total_spikes": brain.getTotalSpikeCount(),
            "avg_firing_rate": brain.getAverageFiringRate(),
            "eiratios": brain.getExcitationInhibitionRatio(),
            "development_stage": brain.getDevelopmentalStage(),
            "neuron_count": brain.getTotalNeuronCount()
        }
        results.append(result)
        
        # Save brain state periodically
        if i % 5 == 0:
            brain.save(f"checkpoints/experiment_{i}.bin")
    
    return results

# Run experiments
batch_results = run_batch_experiment({"neuron_count": 2000})

# Analyze results
print("=== Batch Experiment Results ===")
for result in batch_results:
    print(f"Exp {result['experiment_id']}: {result['total_spikes']} spikes, "
          f"{result['avg_firing_rate']:.1f} Hz firing rate")
```

## Advanced Features

### 1. Configuration Management

```python
import pynlm

# Create configuration from dictionary
config_dict = {
    "neuron_count": 5000,
    "region_count": 2,
    "connection_probability": 0.2,
    "random_seed": 123,
    "simulation_timestep": 0.001,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.015
}

brain = pynlm.createBrainFromDict(config_dict)
brain.initialize()
```

### 2. Brain Analysis and Visualization

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation
for i in range(100):
    brain.step(i)

# Analyze brain activity
analysis = brain.analyze_activity()
print("=== Brain Analysis ===")
for key, value in analysis.items():
    print(f"{key}: {value}")

# Get visualization data for plotting
viz_data = brain.get_visualization_data()
print(f"Visualization data: {len(viz_data)} points")

# Generate human-readable statistics
stats = brain.generate_statistics()
print("\n=== Statistics ===")
print(stats)
```

### 3. Enhanced Command Line Interface

```bash
# Basic usage
./nlm                          # Run standard demo

# With verbose output
./nlm --verbose                # Show detailed progress

# With benchmark mode
./nlm --benchmark              # Optimized for performance testing

# With custom configuration
./nlm --config my_config.cfg    # Use custom config file

# With custom number of steps
./nlm --steps 5000              # Run 5000 simulation steps

# Save output to file
./nlm --save results.txt        # Save results to file

# Help and usage information
./nlm --help                    # Show complete help information
```

## Common Patterns

### Pattern 1: Learning Simulation

```python
def run_learning_simulation(brain, num_steps=1000, verbose=False):
    initial_spikes = brain.getTotalSpikeCount()
    initial_weights = brain.analyze_synapse_weights()
    
    for step in range(num_steps):
        brain.step(step)
        
        if verbose and step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
    
    final_spikes = brain.getTotalSpikeCount()
    final_weights = brain.analyze_synapse_weights()
    
    # Return learning metrics
    return {
        "spike_increase": final_spikes - initial_spikes,
        "weight_change": brain.calculate_weight_change(initial_weights, final_weights),
        "final_firing_rate": brain.getAverageFiringRate(),
        "eiratios": brain.getExcitationInhibitionRatio()
    }
```

### Pattern 2: Agent Training

```python
def train_agent(agent, world, num_episodes=100):
    rewards = []
    steps_per_episode = []
    
    for episode in range(num_episodes):
        episode_reward = 0
        steps = 0
        
        # Reset environment for new episode
        world.reset()
        
        # Run episode
        for step in range(200):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain = agent.getBrain()
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            episode_reward += world.get_last_reward()
            steps += 1
            
            # End episode if goal reached or max steps exceeded
            if world.is_episode_done() or steps >= 200:
                break
        
        rewards.append(episode_reward)
        steps_per_episode.append(steps)
        
        # Save agent periodically
        if episode % 20 == 0:
            agent.save_checkpoint(f"checkpoints/agent_{episode}.bin")
    
    return {
        "average_reward": sum(rewards) / len(rewards),
        "total_episodes": num_episodes,
        "steps_per_episode": steps_per_episode,
        "rewards": rewards
    }
```

### Pattern 3: Experiment Runner

```python
import pynlm
import json
import time

class ExperimentRunner:
    def __init__(self, config_template):
        self.config_template = config_template
        self.results = []
    
    def run_experiment(self, experiment_id, parameters):
        # Create experiment-specific configuration
        config = pynlm.createDefaultConfig()
        
        # Apply template and parameter overrides
        for key, value in self.config_template.items():
            config.set_value(key, value)
        
        for key, value in parameters.items():
            config.set_value(key, value)
        
        # Add experiment identifier
        config.set_value("experiment_id", experiment_id)
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(config.get_value("num_steps", 1000)):
            brain.step(step)
        end_time = time.time()
        
        # Collect results
        result = {
            "experiment_id": experiment_id,
            "config": config.to_dict(),
            "performance": {
                "total_time": end_time - start_time,
                "steps_per_second": config.get_value("num_steps", 1000) / (end_time - start_time),
                "total_spikes": brain.getTotalSpikeCount(),
                "avg_firing_rate": brain.getAverageFiringRate(),
                "final_development_stage": brain.getDevelopmentalStage()
            },
            "analysis": brain.analyze_activity()
        }
        
        self.results.append(result)
        return result
    
    def run_multiple_experiments(self, experiment_configs):
        # Run multiple experiments concurrently
        for exp_config in experiment_configs:
            experiment_id = exp_config["id"]
            parameters = exp_config["parameters"]
            self.run_experiment(experiment_id, parameters)
    
    def save_results(self, filename):
        # Save experiment results to file
        with open(filename, 'w') as f:
            json.dump(self.results, f, indent=2, default=str)
```

## Troubleshooting Guide

### Common Issues and Solutions

**"My brain isn't doing anything"**
- Did you call `brain.initialize()`?
- Try increasing the number of steps (the brain needs time to warm up)
- Check that sensory input is being provided

**"The agent isn't learning"**
- Ensure reward modulation is enabled: `agent.enableRewardModulation(True)`
- Check that curiosity is enabled: `agent.enableCuriosity(True)`
- Verify development is enabled: `agent.enableDevelopment(True)`
- Make sure structural plasticity is enabled: `agent.enableStructuralPlasticity(True)`

**"Everything is 0"**
- Brains need time to "warm up" - try more steps (at least 100-200)
- Some neurons need input to fire - make sure sensory input is connected
- Check that plasticity rules (STDP, Hebbian) are enabled
- Verify that the brain has sufficient neurons to generate activity

**"Memory systems not working"**
- Check that episodic memory is enabled
- Verify that working memory capacity is set appropriately
- Ensure that the brain is receiving sufficient input to store in memory

**"Performance too slow"**
- Reduce neuron count: `config.set_value("neuron_count", 500)`
- Decrease simulation steps
- Use smaller world dimensions
- Disable some learning systems if not needed

**"Configuration errors"**
- Check that all numeric values are properly typed
- Verify that file paths exist
- Ensure required configuration keys are present

## Advanced Usage Tips

### 1. Optimizing Performance

```python
# For faster execution
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.getConfig().set_value("neuron_count", 500)
brain.getConfig().set_value("region_count", 1)
brain.initialize()

# Run fewer steps
for step in range(500):  # Reduced from 1000
    brain.step(step)
```

### 2. Complex Experiments

```python
# Custom experiment with multiple configurations
experiments = []
for neurons in [500, 1000, 2000]:
    for connectivity in [0.1, 0.15, 0.2]:
        config = pynlm.createDefaultConfig()
        config.set_value("neuron_count", neurons)
        config.set_value("connection_probability", connectivity)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(200):
            brain.step(step)
        
        experiments.append({
            "neurons": neurons,
            "connectivity": connectivity,
            "performance": brain.getAverageFiringRate()
        })
```

### 3. Integration with Other Tools

```python
# Save brain for later analysis
brain.save("brain_checkpoints/my_brain.bin")

# Load brain later
loaded_brain = pynlm.createBrain(pynlm.createDefaultConfig())
loaded_brain.load("brain_checkpoints/my_brain.bin")

# Use for multiple simulations
for i in range(10):
    # Reset brain state
    loaded_brain.reset()
    loaded_brain.initialize()
    
    # Run simulation
    for step in range(100):
        loaded_brain.step(step)
    
    # Collect metrics
    print(f"Run {i+1}: {loaded_brain.getTotalSpikeCount()} spikes")
```

## Quick Reference

| Concept | Code | Description |
|---------|------|-------------|
| Create basic brain | `brain = pynlm.createBrain(pynlm.createDefaultConfig())` | Simple brain setup |
| Initialize brain | `brain.initialize()` | Must be called before use |
| Make brain think | `brain.step(step)` | One simulation step |
| Get stats | `brain.getTotalSpikeCount()` | Total spikes |
| Get firing rate | `brain.getAverageFiringRate()` | Average Hz |
| Create world | `world = pynlm.createSimpleWorld()` | Environment setup |
| Create agent | `agent = pynlm.createAgentBrain(brain)` | Agent interface |
| Get percept | `percept = world.getSensoryPercept()` | What agent sees |
| Get action | `action = agent.decodeMotorCommand()` | Brain's decision |
| Apply action | `world.applyMotorCommand(action, time)` | Execute action |

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md` for advanced topics** - Deep dive into configuration, optimization, and advanced features
2. **Explore `docs/ARCHITECTURE.md`** - Understand the complete brain architecture and system components
3. **Experiment with different configurations** - Try various neuron counts, connectivity patterns, and learning parameters
4. **Join the research community** - Share your findings and collaborate with other researchers

## License

MIT

Enjoy experimenting with artificial developmental brains!
