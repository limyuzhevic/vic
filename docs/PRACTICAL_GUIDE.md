# NLM User Guide: Practical Examples

## Quick Start

This guide provides practical examples for getting started with NLM (Neural Learning Machine). We'll walk through various use cases from simple neuron simulation to complete agent-based world interaction.

## Prerequisites

- NLM built and installed (see README.md for build instructions)
- Basic understanding of neural concepts (neurons, synapses, plasticity)
- Python 3.8+ for examples (NLM has both C++ and Python bindings)

## Section 1: Simple Neuron Simulation

### Example 1: Basic Neuron Activity

This example shows how to create and run a simple neuron simulation to understand basic neural dynamics.

```python
import pynlm

# Create a simple brain configuration
config = pynlm.createDefaultConfig()

# Configure for a small neuron population
config.set("brain.neuron_count", 50)
config.set("brain.synapse_density", 0.1)

# Create and initialize the brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run simulation for 100 steps
for step in range(100):
    brain.step(step)
    
    # Log firing rate
    if step % 20 == 0:
        firing_rate = brain.getAverageFiringRate()
        print(f"Step {step}: Average firing rate = {firing_rate:.3f}")

print("Neuron simulation complete!")
```

### What this teaches:

1. **Basic Setup**: Creating a brain configuration and initializing the neural network
2. **Understanding Parameters**: How neuron count and synapse density affect network properties
3. **Simulation Loop**: The basic step-by-step simulation process
4. **Monitoring**: Tracking neural activity over time

## Section 2: Learning and Plasticity

### Example 2: STDP Learning Experiment

This example demonstrates Spike-Timing-Dependent Plasticity (STDP) learning.

```python
import pynlm
import time

def run_stdp_experiment():
    """Run an STDP learning experiment."""
    
    # Create configuration with STDP enabled
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 200)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print("Starting STDP learning experiment...")
    print("This will demonstrate synaptic strengthening and weakening")
    
    # Enable plasticity on synapses
    if hasattr(brain, 'getRegions'):
        for region in brain.getRegions():
            if hasattr(region, 'getSynapses'):
                for syn in region.getSynapses():
                    syn.enablePlasticity(True, True, False)  # STDP + Hebbian
    
    # Create correlated pre-post activity to trigger LTP
    pre_neuron = None
    post_neuron = None
    
    # Find appropriate neurons for the experiment
    if hasattr(brain, 'getTotalNeuronCount'):
        total_neurons = brain.getTotalNeuronCount()
        if total_neurons >= 2:
            pre_neuron = brain.getNeuronByIndex(0)
            post_neuron = brain.getNeuronByIndex(1)
    
    # Apply STDP learning protocol
    if pre_neuron and post_neuron:
        # Create pre-before-post spike timing for LTP
        for trial in range(50):
            # Pre-synaptic neuron fires first
            pre_neuron.injectCurrent(60.0f)
            brain.step(trial * 2)
            
            # Post-synaptic neuron fires shortly after
            post_neuron.injectCurrent(60.0f)
            brain.step(trial * 2 + 1)
            
            # Log progress
            if trial % 10 == 0:
                print(f"Completed {trial} STDP trials")
                
        print("STDP experiment completed!")
        print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
    
    return brain

# Run the experiment
brain = run_stdp_experiment()
```

### What this teaches:

1. **STDP Mechanism**: How pre-before-post spikes strengthen synapses
2. **Plasticity Control**: Enabling and configuring plasticity rules
3. **Learning Protocols**: Designing experiments to test specific learning phenomena
4. **Measurement**: Quantifying learning through synaptic weight changes

## Section 3: Agent-World Interaction

### Example 3: Complete Agent Simulation

This example shows how to create a complete agent that interacts with a simulated world.

```python
import pynlm

def create_and_run_agent():
    """Create an agent and run a complete simulation."""
    
    # 1. Create the core components
    print("=== Step 1: Creating Core Components ===")
    
    # Create brain configuration
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.05)
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"✓ Brain created: {brain.getTotalNeuronCount()} neurons")
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    
    # Create and configure world
    world = pynlm.createSimpleWorld()
    world.configure(
        width=20,
        height=20,
        visionWidth=8,
        visionHeight=8
    )
    print("✓ World configured: 20x20 grid, 8x8 vision")
    
    # Initialize agent with world
    agent.initialize(world)
    print("✓ Agent initialized with world")
    
    # 2. Enable learning and development subsystems
    print("\n=== Step 2: Enabling Learning Systems ===")
    
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("✓ Reward modulation enabled")
    print("✓ Structural plasticity enabled")
    print("✓ Development enabled")
    print("✓ Curiosity enabled")
    
    # 3. Set starting position and run simulation
    print("\n=== Step 3: Starting Simulation ===")
    
    world.reset()
    world.setAgentStart(10.0, 10.0)
    print(f"✓ Agent positioned at (10, 10)")
    
    # Run simulation loop
    num_steps = 500
    exploration_steps = 200
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Run brain step
        brain.step(step)
        
        # Decode motor command
        motor_cmd = agent.decodeMotorCommand()
        
        # Apply motor command
        world.applyMotorCommand(motor_cmd, world.getSimulationTime())
        
        # Apply reward modulation
        reward = get_internal_reward(world, percept)
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"\n--- Step {step} ---")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Dev Stage: {agent.getDevelopmentalStage()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    print("\n=== Simulation Complete ===")
    print(f"Final stats:")
    print(f"  Total steps: {num_steps}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Final curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Final novelty: {agent.getNoveltyLevel():.3f}")
    print(f"  Final dev stage: {agent.getDevelopmentalStage()}")
    
    return brain, agent, world

def get_internal_reward(world, percept):
    """Compute internal reward from world state."""
    if percept and percept.getInternal():
        internal_signals = percept.getInternal()
        if internal_signals.size() > 0:
            # Reward based on internal energy level
            return internal_signals[0]
    return 0.0

# Run the complete agent simulation
brain, agent, world = create_and_run_agent()
```

### What this teaches:

1. **Integration**: How all NLM components work together as an agent
2. **Sensory Processing**: Converting world percepts to neural signals
3. **Motor Output**: Converting neural activity to actions
4. **Learning**: Reward-modulated plasticity during interaction
5. **Development**: Age-dependent changes in brain properties
6. **Exploration**: Curiosity-driven behavior

## Section 4: Configuration and Customization

### Example 4: Custom Configuration File

Create a custom configuration file for specific experimental conditions.

```python
import pynlm

# Create a specialized configuration for a memory formation experiment
config = pynlm.createDefaultConfig()

# Memory formation parameters
config.set("brain.neuron_count", 300)
config.set("brain.synapse_density", 0.08)
config.set("brain.v_thresh", -50.0)  # Higher threshold for stability
config.set("brain.v_rest", -65.0)    # Typical resting potential
config.set("brain.tau_mem", 25.0)   # Slower membrane time constant

# Memory-specific parameters
config.set("memory.working.capacity", 100)  # Working memory capacity
config.set("memory.episodic.enable", True)  # Enable episodic memory
config.set("memory.associative.enable", True)  # Enable associative memory

# Plasticity settings for memory
config.set("plasticity.hebbian.enable", True)
config.set("plasticity.hebbian.strength", 0.01)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.tau_plus", 30.0)   # LTP time constant
config.set("plasticity.stdp.tau_minus", 30.0) # LTD time constant

# Development for long-term learning
config.set("development.enable", True)
config.set("development.stages", 4)  # 4 developmental stages
config.set("development.critical_period_length", 100.0)  # Strong learning early

# Save the custom configuration
config.saveToFile("memory_experiment.cfg")

print("Custom configuration saved to memory_experiment.cfg")
print("Use this file for reproducible memory experiments.")

# Create brain with custom configuration
custom_brain = pynlm.createBrain(config)
custom_brain.initialize()
```

### What this teaches:

1. **Configuration Management**: Creating and saving custom configurations
2. **Parameter Tuning**: Adjusting parameters for specific experiments
3. **Reproducibility**: Saving experimental conditions for replication
4. **Memory System**: Configuring memory systems for different experiments

## Section 5: Experiment Framework

### Example 5: Running Systematic Experiments

Use NLM's built-in experiment framework to run systematic parameter studies.

```python
import pynlm
from nlm_experiments import create_experiment, run_experiment_batch

# Define experiment parameters
experiment_params = {
    "name": "memory_capacity_study",
    "brain_config": {
        "brain.neuron_count": 200,
        "brain.synapse_density": [0.05, 0.1, 0.15],  # Test different densities
    },
    "plasticity_config": {
        "plasticity.stdp.learning_rate": [0.001, 0.002, 0.005],  # Test different rates
    },
    "memory_config": {
        "memory.working.capacity": [50, 100, 200],  # Test memory capacity
    }
}

# Create experiment
experiment = create_experiment(
    name="memory_capacity_study",
    params=experiment_params,
    description="Study how memory capacity affects learning"
)

# Run experiment batch
print("Running parameter space exploration...")
results = run_experiment_batch(
    experiment,
    max_workers=4,  # Parallel execution
    timeout=300,    # 5 minutes per experiment
    save_results=True,
    results_dir="experiment_results/"
)

# Analyze results
print("\n=== Experiment Results ===")
for i, result in enumerate(results):
    print(f"Experiment {i+1}: {result.get('name', 'Unknown')}")
    print(f"  Parameters: {result.get('parameters', {})}")
    print(f"  Performance: {result.get('performance_score', 0):.3f}")
    print(f"  Duration: {result.get('duration', 0):.2f}s")

print(f"\nTotal experiments completed: {len(results)}")
```

### What this teaches:

1. **Experiment Management**: Organizing and running systematic studies
2. **Parameter Spaces**: Exploring multiple parameter combinations
3. **Batch Processing**: Running multiple experiments efficiently
4. **Result Analysis**: Analyzing and comparing experimental outcomes

## Section 6: Advanced Features

### Example 6: Checkpointing and Recovery

Demonstrate NLM's checkpointing capability for long-running experiments.

```python
import pynlm
import time
import os

def run_experiment_with_checkpointing():
    """Run a long experiment with checkpointing."""
    
    # Create configuration for long experiment
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1000)
    config.set("checkpoint.enabled", True)
    config.set("checkpoint.save_interval", 1000)  # Save every 1000 steps
    
    # Create brain with checkpointing
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    checkpoint_file = "long_experiment_checkpoint.bin"
    start_step = 0
    
    # Check for existing checkpoint
    if os.path.exists(checkpoint_file):
        print(f"Found existing checkpoint: {checkpoint_file}")
        brain.load(checkpoint_file)
        start_step = brain.getCurrentStep() if hasattr(brain, 'getCurrentStep') else 0
        print(f"Resuming from step {start_step}")
    
    print(f"Starting long experiment from step {start_step}")
    
    # Run simulation with periodic checkpointing
    for step in range(start_step, start_step + 5000):
        brain.step(step)
        
        # Save checkpoint periodically
        if step % 1000 == 0 and step > start_step:
            brain.save(f"checkpoint_{step}.bin")
            print(f"Checkpoint saved at step {step}")
        
        # Log progress
        if step % 1000 == 0:
            print(f"Step {step}/{start_step + 5000}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    
    # Save final state
    brain.save(checkpoint_file)
    print(f"Final experiment state saved to {checkpoint_file}")
    
    return brain

# Run the checkpointing experiment
brain = run_experiment_with_checkpointing()
```

### What this teaches:

1. **Checkpointing**: Saving and restoring simulation state
2. **Long-running Experiments**: Managing extended simulation sessions
3. **Reproducibility**: Ensuring experiments can be resumed
4. **Data Management**: Organizing checkpoint files

## Section 7: Python API Reference

### Key Classes and Methods

#### Brain Class
```python
# Creation and initialization
brain = pynlm.createBrain(config)
brain.initialize()

# Core functionality
brain.step(step_number)           # Run one simulation step
brain.reset()                     # Reset brain state
brain.save(filename)              # Save state
brain.load(filename)              # Load state

# Statistics
brain.getTotalNeuronCount()       # Total neurons
brain.getTotalSynapseCount()      # Total synapses
brain.getFiringNeuronCount()      # Currently firing
brain.getAverageFiringRate()       # Average firing rate
brain.getExcitationInhibitionRatio()  # E/I balance
```

#### Config Class
```python
# Create configuration
config = pynlm.createDefaultConfig()

# Query and modify
if config.has("brain.neuron_count"):
    keys = config.getKeys()

config.set("brain.neuron_count", 1000)
config.set("learning.rate", 0.001)

# File I/O
config.loadFromFile("config.json")
config.saveToFile("config.json")
```

#### AgentBrain Class
```python
# Creation
agent = pynlm.createAgentBrain(brain)

# Initialization
agent.initialize(world)

# Sensory processing
agent.processSensoryInput(percept)

# Motor decoding
motor_cmd = agent.decodeMotorCommand()

# Neuromodulation
agent.applyRewardModulation(reward, predicted_reward)

# Development
agent.updateDevelopment(timestep)
agent.getDevelopmentalStage()

# Subsystem enable/disable
agent.enableRewardModulation(True/False)
agent.enableStructuralPlasticity(True/False)
agent.enableDevelopment(True/False)
agent.enableCuriosity(True/False)

# Statistics
agent.getCuriosityLevel()
agent.getNoveltyLevel()
agent.getPredictionError()
agent.getNeuromodulationLevel()
```

#### SimpleWorld Class
```python
# Creation
world = pynlm.createSimpleWorld()

# Configuration
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)

# Control
world.reset()
world.update(timestep)
world.setRandomSeed(42)

# Agent interaction
world.setAgentStart(x, y)
world.applyMotorCommand(motor_cmd, current_time)

# State access
percept = world.getSensoryPercept()
body = world.getAgentBody()

# World objects
world.addObject(pynlm.WorldObject(x, y, pynlm.WorldObjectType.Resource, value=1.0))
world.removeObject(x, y)
world.isValidPosition(x, y)

# Properties
world.getWidth()
world.getHeight()
world.getSimulationTime()
world.getMaxEnergy()
world.setMaxEnergy(100.0)
```

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

## Further Reading

- [NLM Architecture](docs/ARCHITECTURE.md) - Detailed system architecture
- [SCIENCE.md](docs/SCIENCE.md) - Scientific background and theory
- [EXPERIMENTS.md](docs/EXPERIMENTS.md) - Experiment descriptions and protocols
- [README.md](README.md) - Build and installation instructions
- [HOW_TO_USE.md](HOW_TO_USE.md) - Detailed usage guide

## Troubleshooting

### "My brain isn't doing anything"

- Did you call `brain.initialize()`?
- Try increasing the number of steps

### "The agent isn't moving"

- Did you call `world.applyMotorCommand()`?
- Check that `world.update()` is being called

### "Everything is 0"

- Brains need time to "warm up" - try more steps
- Some neurons need input to fire - make sure sensory input is connected

## Next Steps

When you're comfortable:

1. **Read HOW_TO_USE.md** for more detailed instructions
2. **Read docs/ARCHITECTURE.md** to understand how it all works
3. **Experiment with different configurations!**
4. **Try creating your own experiment** using the experiment framework
5. **Combine different patterns** from this guide for custom applications

That's it! You're now ready to use NLM. Happy experimenting!

---

*This guide is a living document. Feel free to contribute examples and corrections.*
