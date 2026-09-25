# NLM User Guide

## Overview

This guide provides step-by-step instructions for common use cases and workflows using NLM (熙然). It's designed to help users get up and running with NLM quickly, with practical examples for different scenarios.

## Prerequisites

- Python 3.8 or higher
- NLM Python package installed (`pip install pynlm`)
- Basic understanding of neural network concepts (neurons, synapses, learning)

## Installation

### From PyPI (Recommended)
```bash
pip install pynlm
```

### From Source
```bash
pip install -e .
```

### Build Dependencies
For development from source, you need:
```bash
pip install pybind11 scikit-build-core numpy pytest
```

## Quick Start Examples

### Example 1: Silent Brain Simulation

Run a brain simulation without external input to observe internal neural activity.

```python
import pynlm

# Create and configure brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(1000):
    brain.step(step)
    
    # Log every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")

print("Simulation complete!")
```

### Example 2: Brain-World Interaction

Create a complete agent that interacts with an environment.

```python
import pynlm

def run_agent_simulation(num_steps=1000):
    """Run a complete agent simulation with brain and world."""
    
    # 1. Create configuration
    config = pynlm.createDefaultConfig()
    
    # 2. Create and initialize brain
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
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # 7. Run simulation loop
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
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

# Run the simulation
run_agent_simulation(1000)
```

### Example 3: Object Avoidance Agent

Create an agent that learns to avoid hazards in its environment.

```python
import pynlm

def create_avoidance_agent():
    """Create an agent that learns to avoid hazards."""
    
    # Configuration with avoidance-focused parameters
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 500)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    # Create components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=7, visionHeight=7)
    world.reset()
    world.setAgentStart(5.0, 5.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Add a hazard
    hazard = pynlm.WorldObject(25.0, 25.0, pynlm.WorldObjectType.Hazard, value=-10.0, radius=1.0)
    world.addObject(hazard)
    
    # Add a resource
    resource = pynlm.WorldObject(20.0, 20.0, pynlm.WorldObjectType.Resource, value=10.0, radius=0.8)
    world.addObject(resource)
    
    return brain, world, agent

def run_avoidance_training(brain, world, agent, num_steps=500):
    """Train agent to avoid hazards and seek resources."""
    
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward from world
        reward = 0.0
        if percept.getInternal():
            reward = percept.getInternal()[0]
            
        # Apply neuromodulation
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print stats periodically
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Position: {world.getAgentBody().x:.1f}, {world.getAgentBody().y:.1f}")
            print(f"  Energy: {world.getAgentBody().energy:.1f}")
            print(f"  Action: {action}")
            print(f"  Reward: {reward:.2f}")
            print(f"  Firing: {brain.getFiringNeuronCount()}")
    
    print("Avoidance training complete!")

# Run the avoidance training
brain, world, agent = create_avoidance_agent()
run_avoidance_training(brain, world, agent, 500)
```

### Example 4: Developmental Learning

Study how the brain develops over time with different developmental stages.

```python
import pynlm
import time

def simulate_development(duration_steps=2000):
    """Simulate brain development over time."""
    
    # Create agent
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=25, height=25, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(12.5, 12.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all developmental features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    print("Starting developmental simulation...")
    print("Step, Time, Stage, Firing, Curiosity, Novelty, Age")
    
    for step in range(duration_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process in brain
        agent.processSensoryInput(percept)
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply neuromodulation
        reward = percept.getInternal()[0] if percept.getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Log developmental progress
        if step % 100 == 0:
            stage = brain.getDevelopmentalStage()
            age = agent.getDevelopmentalStage()
            print(f"{step}, {world.getSimulationTime():.1f}, {stage.name}, "
                  f"{brain.getFiringNeuronCount()}, {agent.getCuriosityLevel():.3f}, "
                  f"{agent.getNoveltyLevel():.3f}, {age.name}")
    
    print("Developmental simulation complete!")
    return brain, agent, world

# Run developmental simulation
brain, agent, world = simulate_development(2000)
```

## Common Workflows

### Workflow 1: Experiment Setup and Execution

1. **Define Experiment Parameters**
2. **Create Configuration**
3. **Initialize Components**
4. **Run Simulation**
5. **Analyze Results**

```python
def run_experiment(config_path=None, num_steps=1000):
    """Standard experiment workflow."""
    
    # Step 1: Create configuration
    if config_path:
        config = pynlm.createDefaultConfig()
        config.loadFromFile(config_path)
    else:
        config = pynlm.createDefaultConfig()
        # Apply experiment-specific settings
        config.set("brain.neuron_count", 1000)
        config.set("plasticity.stdp.learning_rate", 0.001)
    
    # Step 2: Create and initialize components
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Step 3: Enable required subsystems
    agent.enableRewardModulation(True)
    
    # Step 4: Run experiment
    for step in range(num_steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Record data for analysis
        if step % 100 == 0:
            log_experiment_data(step, brain, world, agent)
    
    # Step 5: Save results
    brain.save(f"experiment_checkpoint_{num_steps}.bin")
    
    return brain, world, agent
```

### Workflow 2: Parameter Tuning

Systematically explore parameter space to optimize performance.

```python
def tune_parameters(param_grid, base_config=None):
    """Tune NLM parameters using grid search."""
    
    if base_config is None:
        config = pynlm.createDefaultConfig()
    else:
        config = base_config
    
    results = []
    
    for params in param_grid:
        # Create fresh config for each parameter set
        test_config = pynlm.createDefaultConfig()
        test_config = config  # Apply base config
        
        # Override with tuned parameters
        for key, value in params.items():
            test_config.set(key, value)
        
        # Run test
        brain = pynlm.createBrain(test_config)
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(width=15, height=15, visionWidth=6, visionHeight=6)
        world.reset()
        world.setAgentStart(7.5, 7.5)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agent.enableRewardModulation(True)
        
        # Run test simulation
        final_firing = 0
        for step in range(200):
            world.update(0.1)
            agent.processSensoryInput(world.getSensoryPercept())
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step == 199:
                final_firing = brain.getFiringNeuronCount()
        
        results.append({
            'params': params,
            'final_firing': final_firing,
            'avg_firing': brain.getAverageFiringRate()
        })
        
        print(f"Tested {params}: firing={final_firing}")
    
    return results

# Example parameter grid
param_grid = [
    {"brain.neuron_count": 500, "plasticity.stdp.learning_rate": 0.001},
    {"brain.neuron_count": 1000, "plasticity.stdp.learning_rate": 0.001},
    {"brain.neuron_count": 500, "plasticity.stdp.learning_rate": 0.01},
    {"brain.neuron_count": 1000, "plasticity.stdp.learning_rate": 0.01},
]

# Run parameter tuning
results = tune_parameters(param_grid)
```

### Workflow 3: Multi-Agent Simulation

Run multiple agents in the same environment.

```python
def run_multi_agent_simulation(num_agents=3, steps=500):
    """Run multiple competing agents in shared environment."""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Place agents randomly
    agents = []
    brains = []
    
    for i in range(num_agents):
        # Create unique starting positions
        x = 5 + i * 10
        y = 5 + i * 3
        
        config = pynlm.createDefaultConfig()
        # Different configurations for variety
        config.set("brain.neuron_count", 300 + i * 100)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        brains.append(brain)
    
    print(f"Running {num_agents} agents in shared environment...")
    
    for step in range(steps):
        world.update(0.1)
        
        # Each agent processes sensory input independently
        for agent in agents:
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain = agent.getBrain()
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply agent-specific reward
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        if step % 100 == 0:
            print(f"Step {step}:")
            for i, agent in enumerate(agents):
                brain = agent.getBrain()
                print(f"  Agent {i}: {brain.getFiringNeuronCount()} firing, "
                      f"curiosity={agent.getCuriosityLevel():.2f}")
    
    return agents, brains, world
```

## Component Reference

### Brain Configuration Options

Common brain parameters and their effects:

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `brain.neuron_count` | int | 1000 | Total number of neurons |
| `brain.synapse_density` | float | 0.1 | Synapse connectivity density |
| `brain.v_thresh` | float | -50.0 | Neuron threshold potential (mV) |
| `brain.tau_mem` | float | 20.0 | Membrane time constant (ms) |
| `brain.tau_ref` | float | 2.0 | Refractory period (ms) |

### Plasticity Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `plasticity.stdp.enable` | bool | true | Enable STDP learning |
| `plasticity.stdp.learning_rate` | float | 0.001 | STDP learning rate |
| `plasticity.hebbian.enable` | bool | true | Enable Hebbian learning |
| `plasticity.structural.enable` | bool | true | Enable structural plasticity |

### Neuromodulation Configuration

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `neuromod.dopamine.scale` | float | 1.0 | Dopamine modulation scale |
| `neuromod.curiosity.enable` | bool | true | Enable curiosity-driven exploration |
| `neuromod.novelty.enable` | bool | true | Enable novelty detection |

## Debugging Tips

### Common Issues and Solutions

1. **Brain not firing neurons**
   - Ensure `brain.initialize()` was called
   - Check neuron parameters (threshold, tau_mem)
   - Verify sensory input is providing energy

2. **Agent not moving**
   - Ensure `world.applyMotorCommand()` is called
   - Check motor neuron activity
   - Verify action decoding is working

3. **Zero activity throughout simulation**
   - Neurons may need time to "warm up"
   - Consider increasing simulation steps
   - Check for connectivity issues

4. **High memory usage**
   - Reduce `brain.neuron_count`
   - Decrease `world.width` or `world.height`
   - Monitor with performance monitoring tools

### Debugging Scripts

```python
def debug_simulation(brain, world=None, agent=None, steps=100):
    """Debug simulation with detailed logging."""
    
    print("=== DEBUGGING SIMULATION ===")
    print(f"Initial brain stats:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    
    for step in range(steps):
        brain.step(step)
        
        if step % 20 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Active neurons: {brain.getActiveNeuronCount()}")
            print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
            print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
            
            if agent:
                print(f"  Curiosity level: {agent.getCuriosityLevel():.2f}")
                print(f"  Novelty level: {agent.getNoveltyLevel():.2f}")
            
            if world:
                body = world.getAgentBody()
                print(f"  Agent position: ({body.x:.1f}, {body.y:.1f})")
                print(f"  Agent energy: {body.energy:.1f}")
    
    print("\n=== DEBUG COMPLETE ===")
```

## Further Resources

- `ADVANCED_GUIDE.md`: Advanced usage patterns and techniques
- `EXAMPLES.md`: Comprehensive code examples
- `CONFIG_REFERENCE.md`: Detailed configuration options
- `docs/ARCHITECTURE.md`: System architecture and design
- `docs/SCIENCE.md`: Scientific background and theory

## Learning Path

1. **Beginner**: Start with `easy_usage.md` and basic examples
2. **Intermediate**: Work through `USER_GUIDE.md` and agent workflows
3. **Advanced**: Explore `ADVANCED_GUIDE.md` and parameter tuning
4. **Expert**: Consult `DEV_GUIDE.md` and `TESTING_GUIDE.md` for development

## Community and Support

For questions and community support:
- Check the GitHub repository issues
- Consult the NLM documentation for updates
- Review experiment examples for best practices

This guide will be updated as new features are added and user feedback is collected. 