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
agent.enableCuriosity(True)              # Explore new things
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

### Project 3: Complete Agent (NEW - Advanced Features)

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

# Enable learning (NEW)
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

### Project 4: Advanced Configuration (NEW - Advanced Features)

```python
import pynlm
import pynlm_advanced as nlm_adv

# Create advanced configuration
config = nlm_adv.createDefaultAdvancedConfig()
brain = pynlm.createBrain(config.get_config())
brain.initialize()

# Enable advanced features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)

print("Advanced configuration loaded!")
print(f"Neurons: {brain.getTotalNeuronCount()}")
print(f"Development stage: {brain.getDevelopmentalStage()}")
```

### Project 5: Performance Profiling (NEW - Advanced Features)

```python
import pynlm_advanced as nlm_adv

# Create performance profiler
profiler = nlm_adv.createPerformanceProfiler()

# Profile different operations
profiler.start_profile("brain_step")
# ... run brain simulation ...
profiler.end_profile()

profiler.start_profile("memory_update")
# ... run memory update ...
profiler.end_profile()

# Get profiling results
summary = profiler.get_profile_summary()
for operation, data in summary.items():
    print(f"{operation}: avg {data['avg_time_ms']:.2f}ms")
```

### Project 6: Experiment Runner (NEW - Advanced Features)

```python
import pynlm_advanced as nlm_adv

# Setup with advanced config
config = nlm_adv.createLearningConfig()
brain = nlm_adv.create_brain(config.get_config())
world = nlm_adv.create_simple_world()
agent = nlm_adv.create_agent_brain(brain)
agent.initialize(world)

# Create experiment runner
experiment_runner = nlm_adv.create_advanced_experiment_runner(brain, world, agent)

# Run ablation study
system_disabling = {"reward_modulation": True, "curiosity": False}
results = experiment_runner.run_ablation_study(system_disabling)

print("Experiment results:", results)
```

---

## Quick Reference

| What you want | Code |
|--------------|-------|
| Create brain | `pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `pynlm.createSimpleWorld()` |
| Create agent | `pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getFiringNeuronCount()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |

---

## Mini Projects - Advanced Features

### Advanced Project 1: Configuration Builder

```python
import pynlm_advanced as nlm_adv

# Create custom configuration
config = (nlm_adv.AdvancedConfigBuilder()
          .set_neural_network(5000, 2)
          .set_connectivity(0.05, 0.5, 0.1)
          .set_stdp(0.02, 0.018, 20.0)
          .enable_plasticity(True)
          .enable_reward_modulation(True)
          .enable_curiosity(True)
          .enable_development(True)
          .set_memory_capacity(500, 2500))

brain = nlm_adv.create_brain(config.get_config())
```

### Advanced Project 2: Simulation Runner

```python
import pynlm_advanced as nlm_adv

# Setup with advanced features
config = nlm_adv.create_high_performance_config(3000)
brain = nlm_adv.create_brain(config.get_config())
world = nlm_adv.create_simple_world()
agent = nlm_adv.create_agent_brain(brain)

# Create advanced runner
runner = nlm_adv.create_advanced_runner(brain, world, agent)

# Run advanced simulation
runner.run_advanced(2000, mode="balanced")

# Get comprehensive statistics
stats = runner.get_advanced_statistics()
print(f"Learning completed: {stats['total_spikes']} spikes, {stats['curiosity_level']:.2f} curiosity")
```

### Advanced Project 3: Performance Profiling

```python
import pynlm_advanced as nlm_adv

# Create performance profiler
profiler = nlm_adv.create_performance_profiler()

# Profile brain operations
profiler.start_profile("neural_dynamics")
# ... run brain dynamics ...
profiler.end_profile()

profiler.start_profile("plasticity")
# ... run plasticity updates ...
profiler.end_profile()

# Get detailed profiling results
summary = profiler.get_profile_summary()
for operation, data in summary.items():
    print(f"{operation}: avg {data['avg_time_ms']:.2f}ms, " +
          f"min {data['min_time_ms']:.2f}ms, max {data['max_time_ms']:.2f}ms")
```

### Advanced Project 4: Experiment Runner

```python
import pynlm_advanced as nlm_adv

# Setup for experiment
config = nlm_adv.create_learning_config()
brain = nlm_adv.create_brain(config.get_config())
world = nlm_adv.create_simple_world()
agent = nlm_adv.create_agent_brain(brain)

# Create experiment runner
experiment_runner = nlm_adv.create_advanced_experiment_runner(brain, world, agent)

# Run ablation study to test individual components
system_disabling = {
    "reward_modulation": True,
    "curiosity": False,
    "structural_plasticity": True,
    "development": False
}

results = experiment_runner.run_ablation_study(system_disabling)
print("Ablation study results:", results)

# Run parameter sweep to optimize configuration
parameters = {
    "connection_probability": [0.05, 0.1, 0.15],
    "stdp_ltp_weight": [0.01, 0.02, 0.03]
}

sweep_results = experiment_runner.run_parameter_sweep(parameters, steps=500)
print("Parameter sweep results:", sweep_results)
```

### Advanced Project 5: Multi-Episode Learning

```python
import pynlm_advanced as nlm_adv

# Setup with advanced configuration
config = nlm_adv.create_learning_config()
brain = nlm_adv.create_brain(config.get_config())
world = nlm_adv.create_simple_world()
agent = nlm_adv.create_agent_brain(brain)
agent.initialize(world)

# Create advanced runner
runner = nlm_adv.create_advanced_runner(brain, world, agent)

# Run multiple episodes for learning
runner.run_multi_episode(num_episodes=50, steps_per_episode=200)

# Get final performance statistics
stats = runner.get_advanced_statistics()
print(f"Learning completed over 50 episodes:")
print(f"  Total spikes: {stats['total_spikes']}")
print(f"  Average firing rate: {stats['avg_firing_rate']:.2f}")
print(f"  Final curiosity level: {stats['curiosity_level']:.2f}")
print(f"  Development stage: {stats['development_stage']}")
```

---

## Advanced Configuration Builder

### Standard Presets

```python
import pynlm_advanced as nlm_adv

# Standard configuration
config = nlm_adv.create_default_advanced_config()

# High performance configuration  
brain = nlm_adv.create_brain(nlm_adv.create_high_performance_config(5000).get_config())

# Learning configuration
brain = nlm_adv.create_brain(nlm_adv.create_learning_config(3000).get_config())
```

### Custom Configuration

```python
import pynlm_advanced as nlm_adv

# Build custom configuration
custom_config = (nlm_adv.AdvancedConfigBuilder()
                .set_neural_network(8000, 3)
                .set_connectivity(0.08, 0.6, 0.15)
                .set_stdp(0.025, 0.02, 30.0)
                .enable_plasticity(True)
                .enable_reward_modulation(True)
                .enable_curiosity(True)
                .enable_development(True)
                .set_memory_capacity(800, 4000)
                .set_simulation(0.001, 500.0))

brain = nlm_adv.create_brain(custom_config.get_config())
```

### Load from Dictionary

```python
import pynlm_advanced as nlm_adv

# Configuration as Python dictionary
config_dict = {
    "neuron_count": 4000,
    "region_count": 2,
    "connection_probability": 0.12,
    "stdp_ltp_weight": 0.02,
    "plasticity.stdp.enable": True,
    "development.enable": True,
    "simulation_timestep": 0.002
}

config = nlm_adv.AdvancedConfigBuilder()
config.load_from_dict(config_dict)
brain = nlm_adv.create_brain(config.get_config())
```

### Export Configuration

```python
import pynlm_advanced as nlm_adv

# Export configuration to dictionary
config = nlm_adv.create_default_advanced_config()
config_dict = config.export_to_dict()

# Convert to JSON (simplified)
import json
json_str = json.dumps(config_dict, indent=2)
```

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

**"Performance is slow"**
- Use `create_high_performance_config()` for larger networks
- Enable `performance_profiling` to identify bottlenecks
- Consider using `create_advanced_runner` for optimized simulation

**"Configuration errors"**
- Use `config.validate()` to check for invalid values
- Load configuration from files (`.json`, `.yaml`, `.cfg`)
- Check that all required keys are properly set

---

## Next Steps

When you're comfortable:

1. **Read `HOW_TO_USE.md`** for more technical details
2. **Read `docs/ARCHITECTURE.md`** to understand how it all works
3. **Experiment with different configurations** - the Advanced Configuration Builder makes this easy!
4. **Try the advanced features** - performance profiling, experiment runner, and multi-episode learning
5. **Create your own custom projects** using the advanced Python API

---

## What's New in Phase 6

Phase 6 introduces **Advanced Features** that make NLM much more powerful:

### New Python Module: `pynlm_advanced`

The `pynlm_advanced` module provides:

- **Advanced Configuration Builder** - Easy creation of optimized configurations
- **Advanced Simulation Runner** - Multiple learning modes and comprehensive statistics
- **Performance Profiler** - Detailed performance analysis and optimization
- **Experiment Runner** - Automated ablation studies and parameter sweeps
- **Enhanced Error Handling** - Better error messages and recovery
- **System Utilities** - Configuration export, health checks, and feature detection

### Key Benefits:

1. **Easier Configuration** - Presets and builder pattern
2. **Better Performance** - Optimized simulation modes
3. **Advanced Profiling** - Detailed performance analysis
4. **Research Tools** - Automated experimentation
5. **Error Recovery** - Better error handling and recovery
6. **Backward Compatible** - Existing code continues to work

### Installation:

```bash
pip install pynlm
# pynlm_advanced is included in the main package
```

### Quick Start with Advanced Features:

```python
import pynlm_advanced as nlm_adv

# Create advanced configuration
config = nlm_adv.create_default_advanced_config()

# Create brain with advanced features
brain = nlm_adv.create_brain(config.get_config())

# Create advanced runner
runner = nlm_adv.create_advanced_runner(brain, world, agent)

# Run advanced simulation
runner.run_advanced(1000, mode="balanced")

# Get comprehensive statistics
stats = runner.get_advanced_statistics()
print(f"Advanced simulation complete!")
print(f"Neurons: {stats['neurons']}, Spikes: {stats['total_spikes']}, Curiosity: {stats['curiosity_level']}")
```

That's it! You're now ready to use NLM with all its advanced capabilities. Welcome to Phase 6 - the era of the integrated artificial brain!
