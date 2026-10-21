# NLM - Enhanced Usage Guide for Beginners and Advanced Users

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

## Advanced Usage Features

NLM now includes advanced features for power users and researchers. Let's explore the enhanced capabilities:

### 1. Enhanced Brain API

```python
import pynlm

# Create and initialize brain with advanced features
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Get comprehensive brain information
brain_info = brain.getBrainInfo()
print(f"Brain has {brain_info['total_neurons']} neurons and {brain_info['total_synapses']} synapses")

# Advanced step with timing
for i in range(100):
    brain.stepWithTiming(i, i * 0.001)  # Step with explicit timing

# Set random seed for reproducible results
brain.setRandomSeed(42)

# Get simulation statistics
sim_time = brain.getSimulationTime()
sim_step = brain.getSimulationStep()
print(f"Simulation time: {sim_time}s, Step: {sim_step}")
```

### 2. Enhanced Agent API

```python
import pynlm

# Create advanced agent with all systems enabled
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Create agent with advanced configuration
agent = pynlm.createAdvancedAgent(brain, world)

# Run simulation with advanced features
for step in range(500):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
    
    if step % 100 == 0:
        agent_info = agent.getBrainInfo()
        print(f"Step {step}: Curiosity={agent_info['curiosity_level']:.3f}, "
              f"Novelty={agent_info['novelty_level']:.3f}")
```

### 3. Configuration Management

```python
import pynlm
import json

# Create configuration
config = pynlm.createDefaultConfig()

# Use advanced configuration with dictionary
advanced_config = {
    "random_seed": 12345,
    "simulation_timestep": 0.001f,
    "neuron_count": 2000,
    "region_count": 3,
    "connection_probability": 0.1f,
    "plasticity.stdp.enable": True,
    "plasticity.stdp.learning_rate": 0.001f,
    "plasticity.hebbian.enable": True,
    "neuromod.dopamine.scale": 1.0f,
    "neuromod.curiosity.enable": True,
    "neuromod.novelty.enable": True,
    "neuromod.prediction_error.enable": True
}

# Create brain with advanced configuration
brain = pynlm.createBrain(config)
# Configure with dictionary (requires advanced API or direct config methods)
config.set("random_seed", 12345)
config.set("neuron_count", 2000)

brain.initialize()
```

### 4. Performance Benchmarking

```python
import pynlm

# Create brain for benchmarking
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run performance benchmark
metrics = ["firing_rate", "memory_usage", "connection_density"]
results = pynlm.benchmarkBrainPerformance(brain, 1000, metrics)

print("=== Performance Benchmark Results ===")
print(f"Steps completed: {results['steps_completed']}")
print(f"Duration: {results['duration_seconds']:.2f} seconds")
print(f"Steps per second: {results['steps_per_second']:.1f}")
print(f"Spikes generated: {results['spikes_generated']}")
print(f"Final firing rate: {results['final_firing_rate']:.3f}")
print(f"Connection density: {results['connection_density']:.6f}")
```

### 5. Simulation Utilities

```python
import pynlm

# Create brain and run simulation until target spikes
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation to achieve target spike count
target_spikes = 5000
result = pynlm.simulateToTargetSpikes(brain, target_spikes)

print(f"Reached target of {target_spikes} spikes in {result['steps_taken']} steps")
print(f"Final simulation step: {result['final_step']}")
```

### 6. Configuration Templates

```python
import pynlm

# Get predefined configuration templates
templates = ["minimal", "balanced", "high_performance"]

for template in templates:
    config = pynlm.getConfigurationTemplate(template)
    print(f"\n=== {template.title()} Template ===")
    for key, value in config.items():
        print(f"{key}: {value}")
```

### 7. System Information

```python
import pynlm

# Get detailed system information
system_info = pynlm.getNLMInfo()
print("=== NLM System Information ===")
print(f"Version: {system_info['version']}")
print(f"Build Date: {system_info['build_date']}")
print(f"Compiler: {system_info['compiler']}")
print(f"Features: {len(system_info['features'])} available")

print("\n=== Key Features ===")
for feature in system_info['features']:
    print(f"✓ {feature}")
```

### 8. Error Handling and Safety

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Perform operation with error handling
def riskyOperation(brain):
    # Simulate a risky operation
    brain.step(0)
    raise ValueError("Simulated error for demonstration")

# Use safe operation wrapper
result = pynlm.safeBrainOperation(brain, riskyOperation, continueOnError=True)

if result["success"]:
    print("Operation completed successfully")
else:
    print(f"Operation failed with error: {result['error']}")
```

### 9. Batch Processing

```python
import pynlm

# Create multiple brains and worlds for batch processing
brains = []
worlds = []

for i in range(5):
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    world = pynlm.createSimpleWorld()
    world.configure(10, 10, 8, 8)
    world.reset()
    
    brains.append(brain)
    worlds.append(world)

# Run batch simulation
batch_results = pynlm.runBatchSimulation(brains, worlds, 100)

print("=== Batch Simulation Results ===")
print(f"Brains processed: {batch_results['brains_count']}")
print(f"Steps completed: {batch_results['steps_completed']}")
print(f"Total spikes generated: {batch_results['total_spikes']}")
print(f"Total neurons: {batch_results['total_neurons']}")
print(f"Average firing rate: {batch_results['avg_firing_rate']:.3f}")
```

### 10. Advanced Configuration

```python
import pynlm

# Create world with advanced configuration
world = pynlm.createSimpleWorld()

# Advanced world configuration
advanced_world_config = {
    "vision_width": 16,
    "vision_height": 16,
    "max_energy": 100.0f,
    "energy_decay_rate": 0.95f,
    "random_seed": 42
}

# Configure world
advanced_world = pynlm.configureAdvancedWorld(
    world, 30, 30, advanced_world_config
)

print("=== Advanced World Configuration ===")
print(f"World dimensions: {advanced_world.getWidth()}x{advanced_world.getHeight()}")
print(f"Vision: {advanced_world.getVisionWidth()}x{advanced_world.getVisionHeight()}")
print(f"Max energy: {advanced_world.getMaxEnergy()}")
print(f"Energy decay rate: {advanced_world.getEnergyDecayRate()}")
print(f"Random seed: {advanced_world.getRandomSeed()}")
```

### 11. Diagnostic and Debugging

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run diagnostic checks
checks = ["connectivity", "plasticity", "memory", "performance"]
diagnostics = pynlm.diagnosticBrain(brain, checks)

print("=== Brain Diagnostics ===")
for check, result in diagnostics.items():
    print(f"{check}: {result}")
```

### 12. Data Export

```python
import pynlm

# Create brain and simulate
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run some simulation
for i in range(100):
    brain.step(i)

# Export brain data
data_types = ["neural_activity", "synaptic_weights", "spike_patterns"]
export_info = pynlm.exportBrainData(brain, "brain_data.json", data_types)

print(f"=== Export Information ===")
print(f"Export successful: {export_info['success']}")
print(f"File: {export_info['filepath']}")
print(f"Data types requested: {export_info['data_types_requested']}")
print(f"Neurons exported: {export_info['brain_info']['neurons']}")
print(f"Synapses exported: {export_info['brain_info']['synapses']}")
```

---

## Common Patterns and Best Practices

### Pattern 1: Complete Agent Simulation

```python
def run_complete_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create brain and initialize
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 3. Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # 4. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # 5. Initialize agent with world
    agent.initialize(world)
    
    # 6. Enable learning subsystems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    agent.enableStructuralPlasticity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(timestep=0.1)
        
        # Get sensory input from world
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command from brain activity
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command to world
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {brain.getDevelopmentalStage()}")
    
    return brain, agent, world

# Run the simulation
brain, agent, world = run_complete_agent_simulation(1000)
```

### Pattern 2: Performance Monitoring

```python
def benchmark_and_monitor(brain, steps):
    """Benchmark brain performance with continuous monitoring."""
    
    print(f"Starting benchmark: {steps} steps")
    print("Step | Firing Rate | Total Spikes | Time (s)")
    print("-" * 50)
    
    import time
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 100 == 0:
            elapsed = time.time() - start_time
            firing_rate = brain.getAverageFiringRate()
            total_spikes = brain.getTotalSpikeCount()
            print(f"{step:4d} | {firing_rate:11.3f} | {total_spikes:12d} | {elapsed:8.2f}")
    
    total_time = time.time() - start_time
    
    # Final benchmark
    metrics = ["firing_rate", "memory_usage"]
    benchmark_results = pynlm.benchmarkBrainPerformance(brain, steps, metrics)
    
    print(f"\n=== Final Benchmark Results ===")
    print(f"Total time: {total_time:.2f} seconds")
    print(f"Steps per second: {benchmark_results['steps_per_second']:.1f}")
    print(f"Final firing rate: {benchmark_results['final_firing_rate']:.3f}")
    
    return benchmark_results
```

### Pattern 3: Experiment Control

```python
def run_experimental_protocol(protocol_name, protocol_config):
    """Run experimental protocol with systematic control."""
    
    # Validate configuration
    validation = pynlm.validateConfiguration(protocol_config)
    if not validation.get("is_valid", False):
        raise ValueError(f"Invalid configuration: {validation}")
    
    print(f"Running experimental protocol: {protocol_name}")
    
    # Create brain with validated configuration
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Apply protocol-specific settings
    for key, value in protocol_config.items():
        if isinstance(value, int):
            brain.getConfig().set(key, value)
        elif isinstance(value, float):
            brain.getConfig().set(key, value)
        elif isinstance(value, bool):
            brain.getConfig().set(key, value)
    
    # Run experimental simulation
    steps = protocol_config.get("steps", 1000)
    for step in range(steps):
        brain.step(step)
        
        # Record data at intervals
        if step % protocol_config.get("record_interval", 100) == 0:
            diagnostic_data = pynlm.diagnosticBrain(brain, ["performance", "plasticity"])
            # Store or analyze diagnostic_data as needed
    
    return brain
```

---

## Troubleshooting

### "My brain isn't doing anything"
- Did you call `brain.initialize()`?
- Try increasing the number of steps
- Check if neurons have enough input to fire

### "The agent isn't moving"
- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called
- Verify that the agent has motor neurons

### "Everything is 0"
- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected
- Check if reward modulation is enabled

### "Performance is slow"
- Reduce the number of neurons in configuration
- Use `benchmarkBrainPerformance()` to identify bottlenecks
- Check if plasticity systems are enabled

### "Configuration errors"
- Use `validateConfiguration()` to check your settings
- Use `getConfigurationTemplate()` for predefined configurations
- Check for typos in configuration keys

---

## Next Steps

When you're comfortable with the basic usage:

1. **Read `HOW_TO_USE.md` for more details** - Comprehensive API reference
2. **Read `docs/ARCHITECTURE.md` to understand how it all works** - Technical architecture
3. **Read `docs/SCIENCE.md` for scientific background** - Research context
4. **Read `docs/EXPERIMENTS.md` for experiment descriptions** - Experimental protocols
5. **Experiment with different configurations** - Try various parameter combinations

## Quick Reference

| What you want | Code |
|--------------|------|
| Create brain | `brain = pynlm.createBrain(config)` |
| Start brain | `brain.initialize()` |
| Make brain think | `brain.step(step)` |
| Create world | `world = pynlm.createSimpleWorld()` |
| Create agent | `agent = pynlm.createAgentBrain(brain)` |
| Get brain stats | `brain.getBrainInfo()` |
| See world | `world.getSensoryPercept()` |
| Make action | `agent.decodeMotorCommand()` |
| Benchmark | `pynlm.benchmarkBrainPerformance(brain, steps)` |
| Validate config | `pynlm.validateConfiguration(config)` |
| Get system info | `pynlm.getNLMInfo()` |

---

That's it! You're now ready to use NLM with advanced capabilities. The enhanced Python API provides powerful tools for research, experimentation, and development of artificial neural systems.

Happy neural simulation!
