# NLM TUTORIALS

## Overview

This section contains tutorials for users at different skill levels, from complete beginners to advanced users. Each tutorial builds upon the previous one, allowing users to progressively master NLM.

## BEGINNER: Getting Started with NLM

### Tutorial 1: Your First Brain

#### Learning Objectives
- Understand NLM architecture and components
- Create and initialize a basic brain
- Run a simple simulation
- Interpret basic brain statistics

#### What You'll Build
A minimal working brain that simulates neural activity without any external interaction.

#### Step-by-Step Instructions

**Step 1: Setup**
```python
import pynlm

print("=== NLM Brain Simulation Tutorial ===")
print("This tutorial creates a simple brain and runs a simulation.")
print()
```

**Step 2: Create Configuration**
```python
# Create default brain configuration
config = pynlm.createDefaultConfig()

# Optional: Customize configuration
# config.set("brain.neuron_count", 500)
# config.set("brain.v_thresh", -50.0)
```

**Step 3: Create and Initialize Brain**
```python
# Create the brain
brain = pynlm.createBrain(config)

# Initialize the brain (required before any simulation)
brain.initialize()

print(f"✓ Brain created with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Brain has {brain.getTotalSynapseCount()} synapses")
```

**Step 4: Run Simulation**
```python
print("\n=== Running Simulation ===")
print("Simulating 100 time steps...")

for step in range(100):
    brain.step(step)
    
    # Print progress every 20 steps
    if step % 20 == 0:
        firing = brain.getFiringNeuronCount()
        avg_rate = brain.getAverageFiringRate()
        print(f"  Step {step:3d}: {firing:4d} neurons firing, {avg_rate:6.2f} avg firing rate")
```

**Step 5: Analyze Results**
```python
print("\n=== Simulation Results ===")
print(f"Final neuron count: {brain.getTotalNeuronCount()}")
print(f"Final synapse count: {brain.getTotalSynapseCount()}")
print(f"Total spikes during simulation: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")

# Try reset
brain.reset()
print("✓ Brain reset completed")
```

#### Complete Beginner Code
```python
import pynlm

# Create and initialize
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for i in range(100):
    brain.step(i)

print("✅ Tutorial 1 complete!")
print(f"Your brain thought {brain.getTotalSpikeCount()} times")
```

### Tutorial 2: Brains in a World

#### Learning Objectives
- Understand the AgentBrain component
- Create and configure a world
- Simulate agent-environment interaction
- Enable learning subsystems

#### What You'll Build
A complete agent that interacts with a simulated world, learns from rewards, and explores its environment.

#### Step-by-Step Instructions

**Step 1: Setup Components**
```python
import pynlm

print("=== Tutorial 2: Agent in World ===")
```

**Step 2: Create Brain and World**
```python
# Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

print(f"✓ Created world ({world.getWidth()}x{world.getHeight()} cells)")
print(f"✓ Agent starting position: (10.0, 10.0)")
```

**Step 3: Create Agent Interface**
```python
# Create agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

print("✓ Agent interface created")
```

**Step 4: Enable Learning Features**
```python
# Enable learning subsystems (try different combinations!)
agent.enableRewardModulation(True)    # Learn from rewards
agent.enableStructuralPlasticity(True)  # Grow new connections
agent.enableDevelopment(True)        # Brain matures over time
agent.enableCuriosity(True)          # Explore new things

print("✓ Learning subsystems enabled")
```

**Step 5: Run Simulation Loop**
```python
print("\n=== Running Agent Simulation ===")
print("Simulating 200 steps of interaction...")

for step in range(200):
    # Update world state
    world.update(0.1)
    
    # Get what the agent sees
    percept = world.getSensoryPercept()
    
    # Tell brain what it perceives
    agent.processSensoryInput(percept)
    
    # Brain processes information
    brain.step(step)
    
    # Get brain's decision
    action = agent.decodeMotorCommand()
    
    # Execute action in world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation (brain learns from experience)
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 50 == 0:
        print(f"  Step {step:3d}:")
        print(f"    Firing neurons: {brain.getFiringNeuronCount():4d}")
        print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"    Dev Stage: {brain.getDevelopmentalStage()}")
```

**Step 6: Analysis**
```python
print("\n=== Final Results ===")
print(f"Total simulation steps: {world.getSimulationTime():.1f}")
print(f"Final firing neurons: {brain.getFiringNeuronCount()}")
print(f"Final curiosity: {agent.getCuriosityLevel():.3f}")
print(f"Final novelty: {agent.getNoveltyLevel():.3f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")
```

#### Complete Tutorial 2 Code
```python
import pynlm

def run_agent_simulation(num_steps=200):
    """Complete agent simulation with brain and world."""
    
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
    
    # 6. Enable subsystems
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

# Run the simulation
run_agent_simulation(200)
print("✅ Tutorial 2 complete!")
```

### Tutorial 3: Building Custom Brains

#### Learning Objectives
- Understand configuration parameters
- Create specialized brain types
- Experiment with different neuron properties
- Save and load brain states

#### What You'll Build
A customized brain with specific characteristics for different simulation scenarios.

#### Step-by-Step Instructions

**Step 1: Explore Configuration Options**
```python
import pynlm

print("=== Tutorial 3: Custom Brain Configuration ===")

# Create configuration
config = pynlm.createDefaultConfig()

# View all available configuration keys
print("Available configuration categories:")
for key in config.getKeys():
    if '.' in key:
        category = key.split('.')[0]
        if category not in locals().get('categories', set()):
            print(f"  - {category}")

# Default configuration includes many parameters
print(f"\nDefault brain has {config.get('brain.neuron_count', 1000)} neurons")
```

**Step 2: Create Specialized Brain**
```python
# Create a brain with specific characteristics
config = pynlm.createDefaultConfig()

# Customize brain parameters
config.set("brain.neuron_count", 2000)                    # More neurons
config.set("brain.v_thresh", -40.0)                      # Lower threshold
config.set("brain.tau_mem", 10.0)                        # Faster membrane time constant
config.set("brain.synapse_density", 0.05)                 # Sparse connectivity
config.set("plasticity.stdp.enable", True)               # Enable STDP
config.set("plasticity.stdp.learning_rate", 0.01)        # Faster learning
config.set("neuromod.dopamine.scale", 2.0)               # Stronger dopamine effect

print("✓ Custom configuration created")
```

**Step 3: Create and Test Brain**
```python
brain = pynlm.createBrain(config)
brain.initialize()

print(f"✓ Custom brain created:")
print(f"  - {brain.getTotalNeuronCount()} neurons")
print(f"  - {brain.getTotalSynapseCount()} synapses")
print(f"  - Threshold: {config.get('brain.v_thresh')} mV")
print(f"  - Learning rate: {config.get('plasticity.stdp.learning_rate')}")
```

**Step 4: Save and Load**
```python
brain.save("my_custom_brain.bin")
print("✓ Brain state saved")

# Create new brain and load saved state
new_brain = pynlm.createBrain(config)
new_brain.load("my_custom_brain.bin")

if new_brain.getTotalNeuronCount() == brain.getTotalNeuronCount():
    print("✓ Brain state loaded successfully")
else:
    print("❌ Failed to load brain state")
```

#### Complete Tutorial 3 Code
```python
import pynlm

# Create specialized brain for cognitive tasks
config = pynlm.createDefaultConfig()

# High-performance cognitive brain parameters
config.set("brain.neuron_count", 3000)                    # Many neurons for cognition
config.set("brain.v_thresh", -45.0)                      # Moderate excitability
config.set("brain.tau_mem", 15.0)                        # Balanced dynamics
config.set("brain.synapse_density", 0.08)                 # Rich connectivity
config.set("plasticity.stdp.learning_rate", 0.005)       # Fine-tuned learning
config.set("plasticity.hebbian.enable", True)            # Complementary learning
config.set("neuromod.dopamine.scale", 1.5)               # Balanced neuromodulation

# Create and initialize
brain = pynlm.createBrain(config)
brain.initialize()

# Run longer simulation for cognitive development
for step in range(500):
    brain.step(step)
    
    if step % 100 == 0:
        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")

# Save the developed brain
brain.save("cognitive_brain.bin")

print("✅ Tutorial 3 complete!")
print(f"Developed brain saved with {brain.getTotalNeuronCount()} neurons")
```

## INTERMEDIATE: Advanced NLM Usage

### Tutorial 4: Research Experiment Setup

#### Learning Objectives
- Understand experiment framework
- Set up controlled experiments
- Record and analyze experimental data
- Implement experiment best practices

#### What You'll Build
A structured research experiment that can be replicated and analyzed.

### Tutorial 5: Performance Optimization

#### Learning Objectives
- Understand performance bottlenecks
- Configure for optimal performance
- Monitor and profile simulation performance
- Scale simulations for large networks

#### What You'll Build
A brain simulation optimized for performance and scalability.

### Tutorial 6: Custom Plasticity Rules

#### Learning Objectives
- Understand plasticity rule architecture
- Implement custom learning rules
- Experiment with novel plasticity mechanisms
- Compare different plasticity strategies

#### What You'll Build
A brain with custom learning rules different from the standard STDP and Hebbian learning.

## ADVANCED: Expert NLM Usage

### Tutorial 7: Advanced Simulation Techniques

#### Learning Objectives
- Master advanced simulation techniques
- Implement complex interaction patterns
- Use advanced configuration options
- Debug and optimize complex simulations

#### What You'll Build
A sophisticated agent capable of complex behaviors and learning strategies.

### Tutorial 8: Integration with External Systems

#### Learning Objectives
- Understand NLM extensibility
- Integrate with external data sources
- Create custom world implementations
- Connect to external analysis tools

#### What You'll Build
An NLM brain integrated with external systems for enhanced functionality.

## Quick Reference

### Key Functions

| Function | Description | Example |
|----------|-------------|---------|
| `createBrain(config)` | Create brain instance | `brain = pynlm.createBrain(pynlm.createDefaultConfig())` |
| `brain.initialize()` | Initialize brain | `brain.initialize()` |
| `brain.step(step)` | Run simulation step | `brain.step(step_number)` |
| `brain.reset()` | Reset brain state | `brain.reset()` |
| `createSimpleWorld()` | Create world instance | `world = pynlm.createSimpleWorld()` |
| `createAgentBrain(brain)` | Create agent interface | `agent = pynlm.createAgentBrain(brain)` |
| `brain.getTotalNeuronCount()` | Get neuron count | `brain.getTotalNeuronCount()` |
| `brain.getFiringNeuronCount()` | Get firing neurons | `brain.getFiringNeuronCount()` |

### Common Patterns

#### Pattern 1: Basic Simulation Loop
```python
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(num_steps):
    brain.step(step)
```

#### Pattern 2: Agent Simulation
```python
# Create components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

world = pynlm.createSimpleWorld()
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Configure features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run simulation
for step in range(num_steps):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, world.getSimulationTime())
```

#### Pattern 3: Configuration Experiment
```python
# Experiment with different parameters
for neuron_count in [1000, 5000, 10000]:
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neuron_count)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation and collect metrics
    results = run_simulation(brain, 100)
    print(f"Results for {neuron_count} neurons: {results}")
```

### Error Handling Best Practices

#### Common Errors and Solutions

1. **Brain not initialized before stepping**
```python
# ❌ Incorrect
brain.step(0)

# ✅ Correct
brain.initialize()
brain.step(0)
```

2. **World not configured**
```python
# ❌ Incorrect
world = pynlm.createSimpleWorld()
world.update(0.1)  # Error: world not configured

# ✅ Correct
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.update(0.1)
```

3. **Missing reward for neuromodulation**
```python
# ✅ Correct with default
reward = 0.0  # Use default if no reward
agent.applyRewardModulation(reward, 0.0)

# ✅ Correct with check
if percept.getInternal():
    reward = percept.getInternal()[0]
else:
    reward = 0.0
agent.applyRewardModulation(reward, 0.0)
```

### Performance Tips

1. **Reuse configuration objects**
```python
# Good: Reuse configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 5000)

brain1 = pynlm.createBrain(config)
brain2 = pynlm.createBrain(config)  # Same config
```

2. **Monitor performance**
```python
import time

start_time = time.time()
for step in range(num_steps):
    brain.step(step)
    
end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")
```

3. **Use appropriate step sizes**
```python
# For fast dynamics, use smaller steps
brain.step(step, 0.01)  # 10 ms steps

# For slow dynamics, use larger steps
brain.step(step, 1.0)   # 1000 ms steps
```

### Advanced Configuration

#### Brain Configuration Examples
```python
# High-performance brain for research
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 10000)
config.set("brain.synapse_density", 0.01)
config.set("plasticity.stdp.enable", True)
config.set("plasticity.stdp.learning_rate", 0.001)

# Fast learning brain for quick experiments
config.set("brain.v_thresh", -35.0)      # Higher excitability
config.set("brain.tau_mem", 5.0)        # Faster dynamics
config.set("plasticity.hebbian.enable", True)  # Complementary learning

# Stable brain for long simulations
config.set("brain.v_thresh", -55.0)      # Lower excitability
config.set("brain.tau_ref", 5.0)        # Longer refractory period
config.set("neuromod.dopamine.scale", 0.5)  # Weaker neuromodulation
```

### Multi-Agent Experiments

#### Simple Multi-Agent Setup
```python
import pynlm

def create_agent_world(num_agents=3):
    """Create world with multiple agents."""
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create brains for all agents
    agents = []
    brains = []
    
    for i in range(num_agents):
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        agents.append(agent)
        brains.append(brain)
        
        # Set different starting positions
        x = 5 + i * 15
        y = 5 + i * 15
        world.setAgentStart(x, y)
    
    return world, agents, brains

# Run multi-agent simulation
world, agents, brains = create_agent_world(3)

for step in range(1000):
    world.update(0.1)
    
    # Each agent perceives the same world
    percept = world.getSensoryPercept()
    
    # Each agent processes independently
    for agent in agents:
        agent.processSensoryInput(percept)
        brain = agents.index(agent)  # Find corresponding brain
        brains[brain].step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

print("✅ Multi-agent simulation complete!")
```

## Troubleshooting Guide

### Common Issues and Solutions

1. **"Brain isn't doing anything"**
- **Cause**: Brain not initialized or too few steps
- **Solution**: Call `brain.initialize()` before stepping, run more steps

2. **"Agent isn't moving"**
- **Cause**: Motor command not applied or world not updated
- **Solution**: Ensure both `world.update()` and `world.applyMotorCommand()` are called

3. **"Everything is 0"**
- **Cause**: Network needs time to "warm up"
- **Solution**: Run more simulation steps, check sensory input connections

4. **"Memory systems return None"**
- **Cause**: Advanced memory systems not yet fully integrated
- **Solution**: Use basic brain functionality or wait for Phase 6 integration

### Performance Issues

#### High Memory Usage
- **Solution**: Reduce `brain.neuron_count`, use `brain.synapse_density = 0.01`

#### Slow Simulation
- **Solution**: Increase step size, reduce neuron count, enable SIMD if available

### Debugging Tips

1. **Enable verbose logging**
```python
# Add logging configuration
config.set("logger.level", "DEBUG")
```

2. **Print neuron statistics**
```python
print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
print(f"Average rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

3. **Check world state**
```python
percept = world.getSensoryPercept()
print(f"Vision size: {len(percept.getVision())}")
print(f"Touch sensors: {len(percept.getTouch())}")
print(f"Internal signals: {len(percept.getInternal()) if percept.getInternal() else 0}")
```

## References

- NLM Documentation: https://nlm.readthedocs.io
- Source code: https://github.com/nlm-project/nlm
- Examples and more tutorials: Check the examples/ directory

## Further Reading

- [NLM Architecture Documentation](docs/ARCHITECTURE.md)
- [NLM Scientific Background](docs/SCIENCE.md)
- [NLM Performance Guide](docs/PERFORMANCE.md)
- [NLM Research Methods](docs/RESEARCH_METHODS.md)
- [NLM Experiment Guide](docs/EXPERIMENTS.md)

## Getting Help

If you need help:
1. Check the NLM GitHub repository issues
2. Join the NLM Discord community
3. Email: help@nlm.org
4. Documentation: https://nlm.readthedocs.io

---

*Last updated: 2026-10-07*
*Version: 0.1.0*]