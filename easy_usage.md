# NLM - Easy Usage Guide for Beginners

## What is NLM?

NLM is a **brain simulator** for computers. It lets you create virtual brains that can learn, remember things, and make decisions - just like how your brain works!

Think of it like this:
- **Neurons** = Brain cells that send signals to each other
- **Synapses** = Connections between brain cells
- **Learning** = When connections get stronger or weaker based on what happens
- **Memory** = The brain remembering patterns

That's it! NLM simulates all of this.

## Quick Start with New Features

The NLM Python API has been significantly improved with enhanced utilities and better error handling:

```python
import pynlm

# Create and configure brain using the improved API
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Load configuration from dictionary (new feature!)
config = {
    "neuron_count": 500,           # Number of neurons
    "region_count": 1,             # Number of brain regions
    "connection_probability": 0.15, # Synapse density
    "stdp_ltp_weight": 0.01,       # STDP potentiation strength
    "stdp_ltd_weight": 0.012,      # STDP depression strength
    "stdp_tau": 20.0,              # STDP time constant
}

brain.getConfig().setBatch(config)  # New batch config setter
brain.initialize()

# Get brain statistics (new feature!)
stats = brain.getBrainStats()
print(f"Brain initialized: {stats['totalNeurons']} neurons, {stats['totalSynapses']} synapses")

# Run simulation with error handling
for step in range(100):
    try:
        brain.step(step)
    except Exception as e:
        print(f"Error at step {step}: {e}")
        break

print(f"Simulation complete! Total spikes: {brain.getTotalSpikeCount()}")
```

## Advanced Usage with Agent and World

Here's a more complete example using the enhanced agent system:

```python
import pynlm

# Create and configure the complete brain-world-agent system
brain = pynlm.createBrain(pynlm.createDefaultConfig())
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)

# Configure world
world.configure(
    width=20, height=20,
    visionWidth=8, visionHeight=8
)

# Initialize everything
world.setAgentStart(10.0, 10.0)
brain.initialize()
agent.initialize(world)

# Enable advanced features (new API!)
agent.enableAllSystems()  # Enables reward modulation, structural plasticity, development, curiosity

# Run simulation with monitoring
print("Starting agent simulation...")
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process in agent (brain receives sensory data)
    agent.processSensoryInput(percept)
    
    # Brain processes and generates action
    brain.step(step)
    
    # Decode motor command from neural activity
    motor_cmd = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Monitor every 20 steps
    if step % 20 == 0:
        brain_stats = brain.getBrainStats()
        agent_stats = agent.getAgentStats()
        print(f"Step {step}: "
              f"Firing: {brain_stats['firingNeurons']} neurons, "
              f"Curiosity: {agent_stats['curiosity']:.3f}, "
              f"Novelty: {agent_stats['novelty']:.3f}")

# Final status report
print("\n=== Simulation Complete ===")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Final curiosity level: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty level: {agent.getNoveltyLevel():.3f}")
print(f"Final developmental stage: {agent.getDevelopmentalStage()}")

# Get detailed diagnostics
brain_stats = brain.getBrainStats()
agent_stats = agent.getAgentStats()
print(f"\n=== Diagnostics ===")
print(f"Motor system: {agent_stats['visionNeurons']} vision neurons, "
      f"{agent_stats['touchNeurons']} touch neurons")
print(f"Brain regions: {len(brain.getRegionIds())}")

# Save brain state (new error handling!)
try:
    brain.save("agent_brain_checkpoint.bin")
    print("Brain state saved successfully!")
except Exception as e:
    print(f"Save failed: {e}")
```

## The 3 Things You Need to Know

1. **Brain** - The virtual brain that thinks (enhanced with integrated memory, prediction, and cognition systems)
2. **World** - The environment the brain lives in (SimpleWorld with configurable physics)
3. **Agent** - The bridge connecting brain to world (AgentBrain with neuromodulation and learning)

## Configuration Management

The new Python API provides several ways to configure your NLM brain:

### Method 1: Direct Setting
```python
brain.getConfig().set("neuron_count", 1000)
brain.getConfig().set("connection_probability", 0.1)
```

### Method 2: Dictionary (New!)
```python
# Clean, readable configuration
config = {
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity.stdp.enable": True,
    "neuromod.dopamine.scale": 1.0,
    "neuromod.curiosity.enable": True,
}
brain.getConfig().setBatch(config)
```

### Method 3: Command Line Arguments (Enhanced!)
```python
# Support both --key=value and -k value formats
args = ["--neuron-count", "500", "--connection-probability", "0.15", 
        "-s", "30", "-d"]  # Custom key=value pairs
brain.getConfig().loadFromArgs(args)
```

### Method 4: File Loading
```python
brain.getConfig().loadFromFile("my_config.cfg")
```

## Error Handling and Validation

The enhanced Python bindings include comprehensive error handling:

```python
import pynlm

brain = pynlm.createBrain(pynlm.createDefaultConfig())

try:
    brain.initialize()
    print("Brain initialized successfully")
except Exception as e:
    print(f"Initialization failed: {e}")

try:
    # Get configuration with type safety
    neuron_count = brain.getConfig().getOr<int>("neuron_count", 500)
    print(f"Neuron count: {neuron_count}")
except Exception as e:
    print(f"Config access error: {e}")
```

## Utility Functions

The new API includes helpful utility functions:

```python
import pynlm

# Get system information
print(pynlm.version())
print(pynlm.buildInfo())

# Create components with defaults
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
```

## Common Patterns

### Pattern 1: Run a Simulation with Monitoring
```python
def run_simulation(brain, world, agent, num_steps=1000):
    for step in range(num_steps):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 100 == 0:
            print(f"Step {step}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")

# Use it
run_simulation(brain, world, agent, 1000)
```

### Pattern 2: Agent Development Study
```python
def study_development(brain, world, agent, duration=500):
    """Study how the brain develops over time."""
    development_stages = []
    
    for step in range(duration):
        # Apply developmental progression
        agent.updateDevelopment(0.1)
        
        # Run simulation step
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
        
        # Record development stage
        development_stages.append({
            'step': step,
            'stage': agent.getDevelopmentalStage(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel()
        })
        
        if step % 100 == 0:
            print(f"Step {step}, Stage: {agent.getDevelopmentalStage()}, "
                  f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return development_stages

# Run development study
study = study_development(brain, world, agent, 500)
```

### Pattern 3: Learning Experiment
```python
def run_learning_experiment(brain, world, agent, steps=1000):
    """Run an experiment measuring learning over time."""
    # Enable learning systems
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Record initial state
    initial_brain = brain.getBrainStats()
    initial_agent = agent.getAgentStats()
    
    print("Starting learning experiment...")
    
    for step in range(steps):
        # Environment interaction loop
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward and apply neuromodulation
        percept = world.getSensoryPercept()
        reward = 0.0  # In a real experiment, get reward from world
        agent.applyRewardModulation(reward, 0.0)
        
        if step % 100 == 0:
            current_brain = brain.getBrainStats()
            current_agent = agent.getAgentStats()
            
            print(f"Step {step}: "
                  f"Neurons: {current_brain['firingNeurons']}, "
                  f"Curiosity: {current_agent['curiosity']:.3f}, "
                  f"Novelty: {current_agent['novelty']:.3f}")
    
    # Measure learning
    final_brain = brain.getBrainStats()
    final_agent = agent.getAgentStats()
    
    learning_results = {
        'steps': steps,
        'initial_neuron_activity': initial_brain['firingNeurons'],
        'final_neuron_activity': final_brain['firingNeurons'],
        'curiosity_change': final_agent['curiosity'] - initial_agent['curiosity'],
        'novelty_change': final_agent['novelty'] - initial_agent['novelty'],
        'final_stage': agent.getDevelopmentalStage()
    }
    
    return learning_results

# Run learning experiment
results = run_learning_experiment(brain, world, agent, 1000)
print("Learning results:", results)
```

## Performance Monitoring

Monitor your NLM brain in real-time:

```python
import time
import pynlm

# Create and initialize
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Performance monitoring
start_time = time.time()
steps_completed = 0

for step in range(1000):
    brain.step(step)
    steps_completed += 1
    
    # Monitor every 100 steps
    if step % 100 == 0:
        elapsed = time.time() - start_time
        steps_per_second = steps_completed / elapsed if elapsed > 0 else 0
        
        stats = brain.getBrainStats()
        print(f"Step {step}: {steps_per_second:.1f} steps/sec, "
              f"{stats['totalSpikes']} spikes, "
              f"{stats['firingNeurons']} firing neurons")

print(f"Performance: {steps_per_second:.1f} steps/second")
```