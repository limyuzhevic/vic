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
3. Experiment with different configurations!

---

## Advanced Usage

This guide has covered the basics. Now you can explore advanced features:

### Using the New Python Interface

The enhanced Python interface provides many convenience methods:

```python
import pynlm

# Create brain with preset configuration
config = pynlm.NLMConfig().load_preset("standard")
brain = pynlm.createBrain(config)

# Initialize and run simple simulation
sim = pynlm.NLMBrainSimulator(config)
sim.initialize()
sim.run(100)
stats = sim.get_stats()
print(f"Simulation complete. Total spikes: {stats['total_spikes']}")

# Run learning experiment with agent
experiment = pynlm.NLMSimulationRunner.run_learning_experiment(
    experiment_name="curiosity_learning",
    preset="learning",
    steps=500,
    enable_agent=True
)

# Create custom configuration
custom_config = (pynlm.NLMConfig()
                .load_preset("high_performance")
                .set("brain.neuron_count", 3000)
                .set("neuromod.dopamine.scale", 1.8))
```

### Using Command-Line Tools

Run simulations using the advanced command-line interface:

```bash
# Run simulation with configuration
nlm-cli run --config=configs/default.cfg --verbose

# Analyze simulation results
nlm-cli analyze --output=simulation_results.log

# Run learning experiment
nlm-cli experiment --name=phase6 --neuron_count=2000 --region_count=5

# Save/load configuration
nlm-cli config-save --file=my_config.json
nlm-cli config-load --file=my_config.json
```

### Working with Memory Systems

The NLM provides comprehensive memory systems that can be accessed programmatically:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)

# Access memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()

# Working memory operations
working_memory.storeToNeuron(neuron_id, activation)
activation = working_memory.getActivation(neuron_id)
neurons = working_memory.getMemoryNeurons()

# Episodic memory operations
episode = pynlm.EpisodicMemoryItem()
episode.positionX = x_position
episode.reward = reward_value
brain.getEpisodicMemory().storeEpisode(episode)

episodes = brain.getEpisodicMemory().getEpisodes()
```

### Neuromodulation and Learning

Control learning through neuromodulation systems:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
agent.initialize(world)

# Enable learning subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

# Access neuromodulation systems
dopamine = agent.getDopamine()
curiosity = agent.getCuriosity()
novelty = agent.getNovelty()
prediction_error = agent.getPredictionError()

# Modify learning rates
agent.setLearningRate(0.01)  # Convenience method

# Monitor learning progress
for step in range(100):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    reward = agent.getCuriosityLevel()
    agent.applyRewardModulation(reward, 0.0)
    
    print(f"Step {step}: Curiosity={agent.getCuriosityLevel():.3f}, Novelty={agent.getNoveltyLevel():.3f}")
```

### Running Different Experiments

Each phase of development provides specialized demos:

```python
# Run specific phase experiments
experiment_modules = {
    "phase3": "Basic neural dynamics",
    "phase4": "Working memory and prediction",
    "phase5": "Performance optimizations",
    "phase6": "Full integration with all systems",
    "phase6_text": "Text association and social learning"
}

for phase_name, description in experiment_modules.items():
    print(f"\n--- Running {phase_name}: {description} ---")
    # Run the specific experiment executable
    import subprocess
    subprocess.run([f"nlm_{phase_name}_demo"], check=True)
```

### Saving and Loading Brain States

Checkpoint your brains for later analysis or continuation:

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run some simulation steps
for i in range(1000):
    brain.step(i)

# Save brain state
brain.save("my_brain_checkpoint.bin")
print("Brain saved!")

# Later, reload and continue
new_brain = pynlm.createBrain(config)
new_brain.initialize()
new_brain.load("my_brain_checkpoint.bin")

# Continue from saved state
for i in range(1000, 2000):
    new_brain.step(i)
    
print("Brain reloaded and simulation continued!")
```

### Advanced Configuration

Create custom configurations for specific research purposes:

```python
import pynlm

# Create advanced configuration
custom_config = pynlm.NLMConfig()

# Set parameters for research experiment
custom_config.set("brain.neuron_count", 5000)
custom_config.set("brain.connection_probability", 0.15)
custom_config.set("plasticity.stdp.enable", True)
custom_config.set("plasticity.hebbian.enable", True)
custom_config.set("plasticity.structural.enable", True)
custom_config.set("neuromod.dopamine.scale", 2.0)
custom_config.set("neuromod.curiosity.enable", True)
custom_config.set("neuromod.novelty.enable", True)
custom_config.set("development.enabled", True)
custom_config.set("development.stage", "adult")
custom_config.set("simulation.timestep", 0.001)
custom_config.set("checkpoint.enabled", True)
custom_config.set("checkpoint.interval", 500)

# Validate configuration
errors = custom_config.validate()
if errors:
    print(f"Configuration errors: {errors}")
else:
    print("Configuration is valid!")
    
# Save for future use
custom_config.save_to_file("research_config.json")
```

### Performance Monitoring

Monitor your NLM simulations in real-time:

```python
import pynlm
import time

# Create brain with performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

start_time = time.time()
last_report = start_time

for step in range(1000):
    brain.step(step)
    
    # Report progress every 100 steps
    if step % 100 == 0:
        current_time = time.time()
        elapsed = current_time - start_time
        
        print(f"Step {step}:")
        print(f"  Time: {elapsed:.2f}s (avg: {elapsed/step:.3f}s per step)")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Avg firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print()

print(f"Total simulation time: {time.time() - start_time:.2f}s")
```

### Creating Custom Projects

Build your own NLM projects:

```python
"""
Example: Create a "Prediction Learning" project
where the agent learns to predict its sensory input
and uses prediction errors to guide behavior.
"""

import pynlm
import numpy as np

class PredictionLearner:
    def __init__(self, config_preset="standard"):
        self.config = pynlm.NLMConfig().load_preset(config_preset)
        self.brain = pynlm.createBrain(self.config)
        self.agent = pynlm.createAgentBrain(self.brain)
        self.world = pynlm.createSimpleWorld()
        self.world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
        self.world.reset()
        
        # Initialize agent
        self.agent.initialize(self.world)
        self.agent.enableCuriosity(True)
        self.agent.enableDevelopment(True)
        self.agent.enableRewardModulation(True)
        
        # Performance tracking
        self.prediction_history = []
        self.reward_history = []
    
    def run_episode(self, max_steps=200):
        """Run a complete episode and return statistics"""
        print(f"Running episode for up to {max_steps} steps...")
        
        for step in range(max_steps):
            # Update world
            self.world.update(0.1)
            
            # Get perception
            percept = self.world.getSensoryPercept()
            
            # Process sensory input
            self.agent.processSensoryInput(percept)
            
            # Brain step
            self.brain.step(step)
            
            # Get action
            action = self.agent.decodeMotorCommand()
            self.world.applyMotorCommand(action, self.world.getSimulationTime())
            
            # Calculate prediction error (simplified)
            curiosity = self.agent.getCuriosityLevel()
            novelty = self.agent.getNoveltyLevel()
            prediction_error = self.agent.getPredictionError()
            
            # Store history
            self.prediction_history.append(prediction_error)
            self.reward_history.append(curiosity * novelty)
            
            # Apply reward modulation
            reward = self.world.getAgentBody().energy
            self.agent.applyRewardModulation(reward, prediction_error)
            
            # Check for episode end
            if self.world.getAgentBody().energy <= 0:
                print(f"Episode ended at step {step} (energy depleted)")
                break
        
        # Return statistics
        return {
            "total_steps": len(self.prediction_history),
            "final_curiosity": self.agent.getCuriosityLevel(),
            "final_novelty": self.agent.getNoveltyLevel(),
            "final_prediction_error": self.agent.getPredictionError(),
            "total_spikes": self.brain.getTotalSpikeCount(),
            "avg_firing_rate": self.brain.getAverageFiringRate(),
            "max_prediction_error": max(self.prediction_history) if self.prediction_history else 0,
            "avg_prediction_error": sum(self.prediction_history) / len(self.prediction_history) if self.prediction_history else 0
        }
    
    def save_results(self, filename):
        """Save experiment results to file"""
        results = {
            "prediction_history": self.prediction_history,
            "reward_history": self.reward_history,
            "final_stats": self.run_episode(0)  # Get current stats
        }
        
        import json
        with open(filename, 'w') as f:
            json.dump(results, f, indent=2)
        
        print(f"Results saved to {filename}")
        return results

# Use the prediction learner
learner = PredictionLearner("learning")
results = learner.run_episode(500)
learner.save_results("prediction_learning_results.json")

print(f"\nPrediction Learning Results:")
print(f"Total steps: {results['total_steps']}")
print(f"Final curiosity: {results['final_curiosity']:.3f}")
print(f"Final prediction error: {results['final_prediction_error']:.3f}")
print(f"Total spikes: {results['total_spikes']}")
print(f"Average firing rate: {results['avg_firing_rate']:.2f} Hz")
```

### Integration with External Tools

Use NLM results in your research pipeline:

```python
# Export results for analysis in other tools
import pynlm
import numpy as np
import pandas as pd

# Run simulation
sim = pynlm.create_brain(2000)
sim.initialize()
sim.run(1000)

# Export spike data
spike_times = []
for step in range(1000):
    # Record spike times (simplified - in real implementation you'd track individual spikes)
    pass

# Create analysis dataframe
data = {
    'step': list(range(1000)),
    'spike_count': [0] * 1000,  # Would be populated with actual spike counts
    'firing_rate': [sim.get_stats()['avg_firing_rate']] * 1000,
    'curiosity': [0] * 1000,  # Would come from agent
    'novelty': [0] * 1000,   # Would come from agent
}

df = pd.DataFrame(data)
df.to_csv('simulation_analysis.csv', index=False)
print("Analysis data exported to simulation_analysis.csv")

# Generate summary report
import matplotlib.pyplot as plt
plt.figure(figsize=(12, 8))

plt.subplot(2, 2, 1)
plt.plot(df['step'], df['spike_count'])
plt.title('Spike Count Over Time')
plt.xlabel('Step')
plt.ylabel('Spike Count')

plt.subplot(2, 2, 2)
plt.plot(df['step'], df['firing_rate'])
plt.title('Firing Rate Over Time')
plt.xlabel('Step')
plt.ylabel('Firing Rate (Hz)')

plt.tight_layout()
plt.savefig('simulation_report.png', dpi=150)
print("Report saved to simulation_report.png")
```

### Troubleshooting Advanced Usage

Common issues and solutions:

**Memory issues:** Large networks may require more memory:
```python
# Configure for memory efficiency
sim = pynlm.create_brain(1000).set_learning_rate(0.001)
```

**Learning stability:** Use curiosity and development:
```python
agent = pynlm.createAgentBrain(brain)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableRewardModulation(True)
```

**Configuration errors:** Always validate:
```python
config = pynlm.NLMConfig().load_preset("standard")
errors = config.validate()
if errors:
    print(f"Configuration errors: {errors}")
```

That's it! You're now ready for advanced NLM usage with all the powerful features available in NLM Phase 6.

---

## Quick Reference for Advanced Users

### Core Classes

| Class | Description |
|-------|-------------|
| `NLMConfig` | Configuration management with presets |
| `NLMBrainSimulator` | High-level brain simulation interface |
| `NLMSimulationRunner` | Experiment management and execution |
| `NLMVisualizer` | Visualization and analysis tools |

### Key Methods

| Method | Class | Description |
|--------|-------|-------------|
| `load_preset()` | `NLMConfig` | Load predefined configuration |
| `run_learning_experiment()` | `NLMSimulationRunner` | Run complete learning experiments |
| `get_stats()` | `NLMBrainSimulator` | Get comprehensive simulation statistics |
| `run_episode()` | `NLMSimulationRunner` | Run single simulation episode |
| `create_summary_plot()` | `NLMVisualizer` | Create analysis visualizations |

### Configuration Presets

| Preset | Description |
|--------|-------------|
| `minimal` | Small brain for testing and prototyping |
| `standard` | Balanced configuration for general use |
| `learning` | Optimized for learning experiments |
| `high_performance` | Large brain for research and analysis |

### Common Patterns

1. **Create and initialize brain:**
   ```python
   sim = pynlm.NLMBrainSimulator().load_preset("standard")
   sim.initialize()
   ```

2. **Run simulation with agent:**
   ```python
   stats = pynlm.NLMSimulationRunner.run_learning_experiment(
       experiment_name="test_learning",
       preset="learning",
       steps=1000
   )
   ```

3. **Get simulation statistics:**
   ```python
   stats = sim.get_stats()
   print(f"Total spikes: {stats['total_spikes']}")
   ```

4. **Visualize results:**
   ```python
   pynlm.NLMVisualizer.create_summary_plot(stats, "results.png")
   ```

That's it! You now have comprehensive access to all NLM features through the enhanced Python interface. Whether you're doing quick tests, running learning experiments, or conducting research, you have the tools to make the most of NLM Phase 6's integrated artificial brain capabilities.

Enjoy experimenting with your new neural learning machine!
