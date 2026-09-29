# NLM - Enhanced Easy Usage Guide for Beginners

## What is NLM?

NLM (熙然, meaning "serene flow") is an experimental artificial developmental brain project. The long-term goal is to create a neural system that begins in a primitive developmental state and acquires increasingly complex abilities through interaction with an environment.

## What Makes NLM Different Now

The Python bindings have been significantly improved with:
- **Error handling**: Better exception messages and type safety
- **Context managers**: Automatic cleanup with `with` statements
- **Convenience functions**: Simplified common operations
- **Performance monitoring**: Built-in profiling capabilities
- **Advanced utilities**: ID creation, configuration helpers, and more
- **Module version**: Easy access to version information

---

## Quick Start (Copy & Paste)

### Example 1: Simplest Brain with Context Manager

```python
import pynlm

# Step 1: Make a brain with context manager for automatic cleanup
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    
    # Step 2: Make it think for 100 steps
    for i in range(100):
        brain.step(i)
    
    print("Done! Your brain thought", brain.getTotalSpikeCount(), "times")
```

### Example 2: Complete Agent with Performance Monitoring

```python
import pynlm

# Create and initialize with world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
agent.initialize(world)

# Enable all learning subsystems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run simulation with performance monitoring
print("Starting simulation...")
for step in range(100):
    world.update(0.1)
    
    # Get sensory input and process
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Brain thinks
    brain.step(step)
    
    # Get action and execute
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)

print("Simulation complete!")
print("Firing neurons:", brain.getFiringNeuronCount())
print("Performance metrics:", pynlm.get_performance_stats())
```

### Example 3: High-Level Convenience Functions

```python
import pynlm

# Create brain and world
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete episode with all subsystems
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Use high-level convenience function for simulation
episode_data = pynlm.run_episode(brain, world, steps=500, verbose=True)

# Get comprehensive brain information
brain_info = pynlm.get_brain_info(brain)
print(f"Brain has {brain_info['neurons']} neurons with {brain_info['total_spikes']} total spikes")

# Use advanced step iterator
step_data = pynlm.simulate_steps(brain, world, num_steps=100)
print(f"Simulation ran {len(step_data)} steps")
```

---

## New Pythonic Features

### 1. Context Managers (Automatic Cleanup)

```python
# Old way: Manual reset
brain.initialize()
brain.step(0)
brain.reset()  # Manual cleanup

# New way: Automatic cleanup with context manager
with pynlm.createBrain(pynlm.createDefaultConfig()) as brain:
    brain.initialize()
    for i in range(10):
        brain.step(i)
    # brain.reset() called automatically here
```

### 2. Enhanced Error Handling

```python
import pynlm

config = pynlm.createDefaultConfig()

# Load configuration with error handling
try:
    config.loadFromFile("my_config.json")
    print("Config loaded successfully")
except Exception as e:
    print(f"Config loading failed: {e}")

# Save configuration with error handling
try:
    brain.save("checkpoint.bin")
    print("Checkpoint saved")
except Exception as e:
    print(f"Failed to save checkpoint: {e}")
```

### 3. Configuration Helpers

```python
import pynlm

# Create configuration with custom settings
my_settings = {
    "int": {"brain.neuron_count": 500},
    "float": {"brain.connection_probability": 0.05},
    "string": {"brain.name": "My Brain"}
}

config = pynlm.create_default_config_with_settings(my_settings)
brain = pynlm.createBrain(config)
brain.initialize()
```

---

## Convenience Functions

### run_episode()

Run a complete simulation episode with automatic error handling:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

world = pynlm.createSimpleWorld()
world.configure(width=10, height=10)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Single function call runs complete episode
episode_results = pynlm.run_episode(brain, world, steps=100)
print(f"Episode completed with {len(episode_results)} recorded steps")
```

### simulate_steps()

Use step-by-step simulation with detailed results:

```python
import pynlm

# Advanced step-by-step simulation with detailed results
step_results = pynlm.simulate_steps(brain, world, num_steps=50)

for i, (step_num, simulation_step, percept, action, result) in enumerate(step_results):
    print(f"Step {step_num}: Firing neurons={brain.getFiringNeuronCount()}")
    print(f"  Action: {action}, Success: {result.success}, Reward: {result.reward}")
```

### get_brain_info()

Get comprehensive brain information in dictionary format:

```python
import pynlm

brain_info = pynlm.get_brain_info(brain)
print(f"Brain Statistics:")
print(f"  Neurons: {brain_info['neurons']}")
print(f"  Synapses: {brain_info['synapses']}")
print(f"  Firing Neurons: {brain_info['firing_neurons']}")
print(f"  Total Spikes: {brain_info['total_spikes']}")
print(f"  Development Stage: {brain_info['dev_stage']}")
```

---

## Advanced Usage for Power Users

### Performance Benchmarking

```python
import pynlm

# Setup
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run benchmark with warmup
benchmark_results = pynlm.benchmark_agent(agent, world, steps=1000, warmup_steps=100)
print(f"Benchmark Results:")
print(f"  Warmup Time: {benchmark_results['warmup_time']:.2f}s")
print(f"  Average Step Time: {benchmark_results['average_step_time']:.4f}s")
print(f"  Throughput: {benchmark_results['throughput']:.1f} steps/s")
print(f"  Peak Memory: {benchmark_results['memory_peak']} MB")
```

### ID Creation Helpers

```python
import pynlm

# Create neuron and synapse identifiers
neuron_id = pynlm.create_neuron_id(42)
synapse_id = pynynlm.create_synapse_id(123)
region_id = pynlm.create_region_id(7)
population_id = pynlm.create_population_id(99)

print(f"Neuron ID: {neuron_id}")
print(f"Synapse ID: {synapse_id}")
print(f"Region ID: {region_id}")
print(f"Population ID: {population_id}")
```

### Agent Training

```python
import pynlm

# Train agent with different configurations
training_configs = [
    {"episodes": 100, "learning_rate": 0.01},
    {"episodes": 500, "learning_rate": 0.005},
    {"episodes": 1000, "learning_rate": 0.001}
]

for config in training_configs:
    results = pynlm.train_agent(
        agent, world, 
        episodes=config["episodes"], 
        learning_rate=config["learning_rate"]
    )
    print(f"Training {config['episodes']} episodes: Final reward = {results['final_reward']}")
```

---

## Common Patterns (Updated)

### Pattern 1: Robust Simulation Loop

```python
def run_simulation(brain, world, agent, num_steps):
    """Robust simulation loop with error handling."""
    for step in range(num_steps):
        try:
            # Update world
            world.update(0.1)
            
            # Get and process sensory input
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            
            # Brain thinks
            brain.step(step)
            
            # Get and execute action
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
        except Exception as e:
            print(f"Error at step {step}: {e}")
            break
    
    # Get performance metrics
    metrics = pynlm.get_simulation_metrics(brain, world)
    return metrics

# Use it
metrics = run_simulation(brain, world, agent, 1000)
print(f"Simulation completed with efficiency: {metrics['simulation_efficiency']:.2%}")
```

### Pattern 2: Development Monitoring

```python
def monitor_development(brain, world, agent, check_interval=100):
    """Monitor brain development and performance."""
    for step in range(1000):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        if step % check_interval == 0:
            info = pynlm.get_brain_info(brain)
            print(f"Step {step}: {info['dev_stage']} stage, {info['firing_neurons']} neurons firing")
            
            # Log developmental changes
            if brain.getDevelopmentalStage().value > DevelopmentalStage.Initial.value:
                print(f"  Development progressing: {brain.getDevelopmentalStage()}")
```

---

## Quick Reference (Updated)

### Core Functions

| What you want | Code | Notes |
|--------------|------|-------|
| Create brain | `pynlm.createBrain(config)` | Use with `with` for cleanup |
| Start brain | `brain.initialize()` | Always call this first |
| Make brain think | `brain.step(step)` | Can be used in context managers |
| Create world | `pynlm.createSimpleWorld()` | Configure dimensions |
| Create agent | `pynlm.createAgentBrain(brain)` | Connects brain to world |
| Get brain stats | `pynlm.get_brain_info(brain)` | Comprehensive dictionary |
| See world | `world.getSensoryPercept()` | Valid until next world.update() |
| Make action | `agent.decodeMotorCommand()` | Returns action object |
| Run episode | `pynlm.run_episode(brain, world, steps)` | High-level convenience |

### Convenience Functions

| Function | Purpose | Typical Use |
|----------|---------|-------------|
| `run_episode()` | Complete simulation | Agent training loops |
| `simulate_steps()` | Step-by-step simulation | Research experiments |
| `train_agent()` | Agent training | Reinforcement learning |
| `benchmark_agent()` | Performance testing | Optimization |
| `get_brain_info()` | Brain statistics | Monitoring & debugging |
| `create_neuron_id()` | Create neuron IDs | Advanced neuron management |

### Error Handling

| Method | Exception | Typical Situation |
|--------|-----------|-------------------|
| `loadFromFile()` | `KeyError` | Invalid config files |
| `save()` | `KeyError` | File permission issues |
| `initialize()` | `NLMError` | Invalid configuration |

---

## Troubleshooting

### Common Issues and Solutions

**"My context manager isn't working properly"**
- Make sure you're using the `with` statement correctly
- Check that your brain doesn't have nested context managers

**"Performance monitoring isn't working"**
- Call `pynlm.enable_logging()` before starting simulation
- Use `pynlm.get_performance_stats()` to check monitoring status

**"Convenience functions are failing"**
- Check that all components (brain, world, agent) are properly initialized
- Use try-except blocks to catch and handle exceptions

**"Brain development isn't progressing"**
- Ensure all learning subsystems are enabled: reward modulation, curiosity, development
- Check configuration parameters for developmental stage settings

---

## Next Steps

### For Beginners:
1. **Master the basics**: Practice with the examples in this guide
2. **Try different configurations**: Experiment with neuron counts and connection probabilities
3. **Explore the world**: Create custom worlds and test agent behaviors

### For Intermediate Users:
1. **Learn advanced features**: Use context managers and convenience functions
2. **Performance optimization**: Use benchmarking tools to optimize simulations
3. **Custom agents**: Create specialized agent behaviors with advanced control

### For Power Users:
1. **Deep customization**: Use ID creation helpers and advanced training functions
2. **Research applications**: Use step-by-step simulation for controlled experiments
3. **Performance analysis**: Use comprehensive metrics for simulation analysis

### Further Resources:
- **Complete documentation**: Read `HOW_TO_USE.md` for comprehensive API reference
- **Architecture guide**: Read `docs/ARCHITECTURE.md` to understand system design
- **Advanced examples**: Check for more complex examples in the examples/ directory

---

That's it! You're now equipped with the enhanced NLM Python API featuring context managers, error handling, convenience functions, and performance monitoring. The new improvements make the API more robust, user-friendly, and powerful for both beginners and advanced users.