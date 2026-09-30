# NLM - Enhanced Interactive Documentation

**An Experimental Artificial Developmental Brain**

Welcome to NLM! This is an enhanced version of the documentation with interactive examples and comprehensive API references.

## Quick Start

Let's get you up and running with a practical example:

```python
import pynlm

# Create a default brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run a simple simulation
print("Starting brain simulation...")
for step in range(100):
    brain.step(step)
    if step % 20 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

print("Simulation complete!")
```

## Interactive Examples

### Example 1: Complete Agent Simulation

This example demonstrates a complete agent with world interaction:

```python
import pynlm
import time

def run_agent_simulation():
    """Run a complete agent simulation with learning"""
    
    # 1. Create and configure brain
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # 2. Create agent interface
    agent = pynlm.createAgentBrain(brain)
    
    # 3. Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    agent.initialize(world)
    
    # 4. Enable advanced features
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    agent.enableDevelopment(True)
    
    # 5. Run simulation with monitoring
    episode_reward = 0.0
    curiosity_history = []
    novelty_history = []
    
    for step in range(1000):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process input and get action
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Compute reward
        reward = 0.0
        internal = percept.getInternal()
        if internal:
            reward = internal[0]
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Collect metrics
        episode_reward += reward
        curiosity_history.append(agent.getCuriosityLevel())
        novelty_history.append(agent.getNoveltyLevel())
        
        # Periodic updates
        if step % 100 == 0:
            print(f"Step {step}: Reward={reward:.2f}, "
                  f"Curiosity={agent.getCuriosityLevel():.2f}, "
                  f"Novelty={agent.getNoveltyLevel():.2f}")
    
    # Episode summary
    avg_reward = episode_reward / 1000
    max_curiosity = max(curiosity_history)
    max_novelty = max(novelty_history)
    
    print("\n=== Episode Results ===")
    print(f"Average reward: {avg_reward:.3f}")
    print(f"Max curiosity: {max_curiosity:.3f}")
    print(f"Max novelty: {max_novelty:.3f}")
    print(f"Developmental stage: {brain.getDevelopmentalStage()}")
    
    return {
        'total_reward': episode_reward,
        'avg_reward': avg_reward,
        'max_curiosity': max_curiosity,
        'max_novelty': max_novelty,
        'final_stage': brain.getDevelopmentalStage(),
        'final_dopamine': agent.getNeuromodulationLevel()
    }

# Run the simulation
results = run_agent_simulation()
print(f"Simulation completed with total reward: {results['total_reward']:.2f}")
```

### Example 2: Memory and Development System

Explore the brain development system:

```python
import pynlm

def study_development():
    """Study how brain develops over time"""
    
    # Create brain with development enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent with development
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    agent.enableDevelopment(True)
    
    print("Brain Development Timeline:")
    print("Step | Stage              | Plasticity | Neurons | Synaptic Weight")
    print("-----|-------------------|------------|---------|----------------")
    
    for step in range(0, 1200, 60):
        # Process random input to simulate experience
        from pynlm import Vision, SensoryPercept
        vision = Vision(8, 8, 3)
        for i in range(64):
            vision.setData([i % 256 / 255.0] * 64)
        
        percept = SensoryPercept()
        percept.setVision(vision)
        
        agent.processSensoryInput(percept)
        brain.step(step)
        agent.updateDevelopment(0.1)
        
        stage = brain.getDevelopmentalStage()
        plasticity = 1.0  # This would need to be exposed via agent
        neurons = brain.getTotalNeuronCount()
        
        # Get synaptic weight via config
        config_summary = brain.getConfig().summary()
        
        stage_name = stage.name if hasattr(stage, 'name') else str(stage)
        
        print(f"{step:4d} | {stage_name:19} | {plasticity:11.2f} | {neurons:6d} | {config_summary[:10]}...")
    
    print("\nDevelopment completed!")

study_development()
```

### Example 3: Neuromodulation Systems

Study the interaction between different neuromodulators:

```python
import pynlm

def study_neuromodulation():
    """Explore neuromodulation systems"""
    
    # Setup with all neuromodulation enabled
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=4, visionHeight=4)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all neuromodulation systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    print("Neuromodulation Dynamics:")
    print("Step | Reward | Dopamine | Curiosity | Novelty | Prediction Error")
    print("-----|--------|----------|-----------|---------|----------------")
    
    # Simulate various scenarios
    scenarios = [
        ("Exploration", 0.1, 0.0),
        ("Success", 1.0, 0.5),
        ("Failure", -1.0, -0.8),
        ("Mixed", 0.3, 0.1)
    ]
    
    for scenario_name, reward, pred_error in scenarios:
        print(f"\n--- Scenario: {scenario_name} (Reward: {reward}) ---")
        
        for step in range(50):
            # Create sensory input
            from pynlm import Vision, SensoryPercept
            vision = Vision(4, 4, 3)
            for i in range(16):
                vision.setData([i % 256 / 255.0] * 16)
            
            percept = SensoryPercept()
            percept.setVision(vision)
            
            agent.processSensoryInput(percept)
            brain.step(step)
            
            # Apply reward with prediction error
            agent.applyRewardModulation(reward, pred_error)
            
            # Record state
            print(f"{step:3d} | {reward:6.1f} | {agent.getNeuromodulationLevel():8.3f} | "
                  f"{agent.getCuriosityLevel():9.3f} | {agent.getNoveltyLevel():6.3f} | "
                  f"{agent.getPredictionError():15.3f}")
    
    print("\nNeuromodulation study completed!")

study_neuromodulation()
```

## API Reference

### Core Classes

#### Brain Class

```python
# Creation and initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Basic operations
brain.step(step_number)                    # Basic simulation step
brain.step(step_number, timestamp)        # Step with timestamp
brain.reset()                              # Reset to initial state

# Input/output
brain.receiveSensoryInput(input)          # Inject sensory data
brain.injectCurrent(neuron, current)       # Direct neuron control
brain.produceAction()                      # Get motor action

# Statistics and monitoring
brain.getTotalNeuronCount()               # Total neurons
brain.getTotalSynapseCount()              # Total synapses
brain.getFiringNeuronCount()              # Currently active
brain.getAverageFiringRate()              # Average firing rate
brain.getExcitationInhibitionRatio()      # E/I balance
brain.getTotalSpikeCount()                # Total spikes fired
brain.getDevelopmentalStage()             # Current developmental stage
brain.getConfig()                          # Access configuration
brain.logStatus()                          # Print status information

# File operations
brain.save("checkpoint.bin")               # Save state
brain.load("checkpoint.bin")               # Load state
brain.addRegion("cortex")                 # Add neural region
brain.getRegion(region_id)                # Get region by ID
```

#### Config Class

```python
# Creation and loading
config = pynlm.createDefaultConfig()
config.loadFromFile("config.json")        # Load from JSON
config.loadFromArgs(args)                 # Load from command line

# Configuration management
config.has("brain.neuron_count")          # Check if key exists
config.getKeys()                          # Get all keys
config.set("brain.neuron_count", 1000)    # Set value
config.clear()                             # Clear all settings
config.summary()                           # Get config summary

# File operations
config.saveToFile("config.json")          # Save to JSON
```

#### AgentBrain Class

```python
# Creation and initialization
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Sensory-motor loop
agent.processSensoryInput(percept)         # Process sensory data
agent.decodeMotorCommand()                 # Get motor command
agent.applyRewardModulation(reward, pred)  # Apply reward learning
agent.updateDevelopment(timestep)          # Update development

# System control
agent.enableRewardModulation(True)        # Enable reward learning
agent.enableStructuralPlasticity(True)    # Enable neural growth
agent.enableDevelopment(True)             # Enable development
agent.enableCuriosity(True)                # Enable exploration

# Monitoring
agent.getCuriosityLevel()                 # Current curiosity
agent.getNoveltyLevel()                   # Current novelty
agent.getPredictionError()                # Prediction accuracy
agent.getNeuromodulationLevel()           # Overall neuromodulation
agent.getDevelopmentalStage()            # Current developmental stage
```

#### SimpleWorld Class

```python
# Creation and configuration
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# World control
world.reset()                              # Reset to initial state
world.update(timestep)                     # Advance simulation
setAgentStart(x, y)                        # Set agent position
world.applyMotorCommand(action, time)      # Apply action

# Sensing and perception
percept = world.getSensoryPercept()        # Get sensory percept
body = world.getAgentBody()                # Get agent body state

# Object management
world.addObject(object)                   # Add world object
world.removeObject(x, y)                  # Remove object
world.isValidPosition(x, y)               # Check position validity

# Properties
world.getWidth()                          # World width
world.getHeight()                         # World height
world.getSimulationTime()                 # Current simulation time
```

### Enumerations

#### NeuronType
```python
pynlm.NeuronType.Excitatory    # Excitatory neurons
pynlm.NeuronType.Inhibitory    # Inhibitory neurons
pynlm.NeuronType.Sensory       # Sensory neurons
pynlm.NeuronType.Motor         # Motor neurons
pynlm.NeuronType.Modulatory    # Neuromodulatory neurons
pynlm.NeuronType.Internal      # Internal neurons
```

#### ActionType
```python
pynlm.ActionType.MoveForward
pynlm.ActionType.MoveBackward
pynlm.ActionType.MoveLeft
pynlm.ActionType.MoveRight
pynlm.ActionType.TurnLeft
pynlm.ActionType.TurnRight
pynlm.ActionType.Look
pynlm.ActionType.Interact
pynlm.ActionType.Eat
pynlm.ActionType.Wait
# ... and more
```

#### DevelopmentalStage
```python
pynlm.DevelopmentalStage.Initial
pynlm.DevelopmentalStage.CriticalPeriod
pynlm.DevelopmentalStage.Maturation
pynlm.DevelopmentalStage.Adult
pynlm.DevelopmentalStage.Aging
```

## Advanced Features

### 1. Simulation Control

The Python API provides enhanced control over simulation parameters:

```python
# Custom timestep
for step in range(1000):
    world.update(0.05)  # Custom timestep
    brain.step(step, step * 0.05)  # Synchronized timestep

# Multiple world updates
for _ in range(10):
    world.update(0.1)
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)

# Variable timesteps
time = 0.0
while time < 10.0:
    dt = min(0.1, 10.0 - time)  # Reduce timestep near end
    world.update(dt)
    brain.step(time, time)
    time += dt
```

### 2. Error Handling and Debugging

```python
try:
    # Create and run simulation
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        raise RuntimeError("Brain initialization failed")
    
    # Run simulation with error handling
    for step in range(1000):
        brain.step(step)
        
        # Check for potential issues
        if brain.getFiringNeuronCount() == 0:
            print(f"Warning: No neurons firing at step {step}")
            
except pynlm.RuntimeError as e:
    print(f"NLM runtime error: {e}")
except Exception as e:
    print(f"Unexpected error: {e}")
    import traceback
    traceback.print_exc()
```

### 3. Performance Monitoring

```python
import time
import pynlm

def run_monitored_simulation():
    """Run simulation with performance monitoring"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create agent and world
    world = pynlm.createSimpleWorld()
    world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Performance monitoring
    start_time = time.time()
    steps_completed = 0
    wall_time_per_step = []
    
    for step in range(1000):
        step_start = time.time()
        
        # Run step
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        step_end = time.time()
        wall_time = step_end - step_start
        wall_time_per_step.append(wall_time)
        steps_completed += 1
        
        # Log performance periodically
        if step % 100 == 0:
            avg_wall_time = sum(wall_time_per_step[-100:]) / min(len(wall_time_per_step), 100)
            print(f"Step {step}: {1.0/avg_wall_time:.1f} Hz, "
                  f"Memory: {len(wall_time_per_step[-100:])} steps")
    
    # Calculate performance metrics
    total_wall_time = time.time() - start_time
    avg_steps_per_second = steps_completed / total_wall_time
    avg_wall_time = sum(wall_time_per_step) / len(wall_time_per_step) if wall_time_per_step else 0
    
    print(f"\n=== Performance Summary ===")
    print(f"Total steps: {steps_completed}")
    print(f"Total wall time: {total_wall_time:.2f}s")
    print(f"Average steps/second: {avg_steps_per_second:.1f}")
    print(f"Average wall time per step: {avg_wall_time*1000:.2f}ms")
    print(f"Speed: {1.0/avg_wall_time:.1f} Hz")
    
    return {
        'total_steps': steps_completed,
        'total_wall_time': total_wall_time,
        'avg_steps_per_second': avg_steps_per_second,
        'avg_wall_time': avg_wall_time
    }

# Run monitored simulation
metrics = run_monitored_simulation()
```

## Troubleshooting

### Common Issues and Solutions

**1. "My brain isn't doing anything"
```python
# Solution: Check initialization and increase steps
brain.initialize()  # Always call initialize first
for step in range(1000):  # More steps
    brain.step(step)
```

**2. "The agent isn't moving"
```python
# Solution: Ensure complete agent loop
world.update(0.1)                    # Update world
percept = world.getSensoryPercept()  # Get sensory input
agent.processSensoryInput(percept)   # Process input
brain.step(step)                     # Brain thinks
action = agent.decodeMotorCommand()  # Get action
world.applyMotorCommand(action, time) # Execute action
```

**3. "Everything is 0"
```python
# Solution: Warm-up and proper input
brain.initialize()                   # Initialize brain
# Add sensory input to neurons
brain.injectCurrentToNeurons(
    pynlm.NeuronType.Sensory, 5.0     # Inject current to sensory neurons
)
# Run some steps
for step in range(100):
    brain.step(step)
```

**4. "Import errors"
```bash
# Fix with force reinstall
pip install --force-reinstall .
```

**5. "Segmentation faults"
```python
# Fix common causes
brain.initialize()    # Always initialize before step
brain.reset()         # Reset before re-running
```

## Getting Help

### Online Resources
- [NLM Documentation](https://nlm.readthedocs.io)
- [GitHub Repository](https://github.com/nlm-project/nlm)
- [Examples and Demos](https://github.com/nlm-project/nlm/tree/main/examples)

### Community Support
- For questions and discussions: Join our Discord community
- For bug reports: Use the GitHub issue tracker
- For feature requests: Submit a GitHub issue or pull request

### Interactive Examples
You can run the examples interactively using Jupyter notebooks:

```python
# Install Jupyter
pip install jupyter

# Start interactive session
jupyter notebook examples/
```

## Migration Guide

If you're migrating from an older version:

### Changes in Version 1.0
- `brain.produceAction()` now returns an `Action` object instead of raw motor commands
- Configuration keys have been standardized (e.g., `neuron_count` instead of `n_count`)
- Agent initialization now requires a `world` parameter
- Error handling now uses specific exception types

### Code Updates

#### Old to New Migration

**Old:**
```python
# Old API
brain = pynlm.createBrain(config)
action = brain.produceAction()
```

**New:**
```python
# New API
brain = pynlm.createBrain(config)
action = brain.produceAction()  # Returns Action object
```

**Old:**
```python
# Old config access
config.set("n_count", 1000)
```

**New:**
```python
# New config access
config.set("brain.neuron_count", 1000)
```

## Advanced Configuration

### Custom Configuration Template

```python
import pynlm
import json

# Create custom configuration
template = {
    "brain": {
        "neuron_count": 2000,
        "region_count": 3,
        "synapse_density": 0.15,
        "v_thresh": -50.0,
        "v_rest": -70.0
    },
    "plasticity": {
        "stdp": {
            "enable": True,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": True,
            "learning_rate": 0.0001f
        }
    },
    "neuromod": {
        "dopamine": {"scale": 1.0},
        "curiosity": {"enable": True},
        "novelty": {"enable": True}
    }
}

# Save to file
with open("custom_config.json", "w") as f:
    json.dump(template, f, indent=2)

# Load and use
config = pynlm.createDefaultConfig()
config.loadFromFile("custom_config.json")

brain = pynlm.createBrain(config)
brain.initialize()
```

## Next Steps

When you're ready to explore deeper:

1. **Read the Architecture Documentation**: `docs/ARCHITECTURE.md`
2. **Study the Science**: `docs/SCIENCE.md`
3. **Explore Experiments**: `docs/EXPERIMENTS.md`
4. **Phase 6 Audit**: `docs/PHASE6_FINAL_AUDIT.md`
5. **Experiment with Configurations**: Modify the template above
6. **Benchmark Your System**: Use the built-in benchmark utilities
7. **Contribute**: Share your findings and improvements

Enjoy building your artificial brain with NLM!
