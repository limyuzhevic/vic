# NLM USER GUIDES

## Overview

This section contains comprehensive user guides for different skill levels and use cases. The guides are organized to help users progressively master NLM from basic concepts to advanced applications.

## Getting Started Guide

### 1. Quick Start Tutorial

#### Learning Objectives
- Install and configure NLM
- Run your first brain simulation
- Understand basic brain statistics
- Explore NLM interfaces

#### What You'll Build
A complete working brain simulation that demonstrates NLM capabilities.

#### Step 1: Installation

**Install NLM**
```bash
# Install from PyPI
pip install pynlm

# Install from source
pip install -e .

# Verify installation
python3 -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

**Installation Options**
```bash
# Install with additional dependencies
pip install pynlm[dev]

# Install development version
pip install -e .[dev]

# Build from source (if needed)
python3 -m pip install build
python3 -m build
pip install dist/*.whl
```

#### Step 2: Create Your First Brain

**Option 1: Minimal Brain**
```python
import pynlm

# Create brain with default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"✓ Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Brain has {brain.getTotalSynapseCount()} synapses")
```

**Option 2: Custom Brain**
```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -45.0)
config.set("plasticity.stdp.learning_rate", 0.001)

# Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

print(f"✓ Created custom brain with {brain.getTotalNeuronCount()} neurons")
print(f"✓ Brain threshold: {config.get('brain.v_thresh')} mV")
```

#### Step 3: Run First Simulation

**Basic Simulation**
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
print("Running simulation...")
for step in range(100):
    brain.step(step)
    
    if step % 20 == 0:
        firing = brain.getFiringNeuronCount()
        rate = brain.getAverageFiringRate()
        print(f"  Step {step:3d}: {firing:4d} firing, {rate:6.2f} Hz")
```

**Simulation with Statistics**
```python
import pynlm
import time

# Setup and run
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Measure performance
start_time = time.time()
for step in range(1000):
    brain.step(step)
end_time = time.time()

# Display results
print("\n=== Simulation Results ===")
print(f"Total steps: 1000")
print(f"Wall time: {end_time - start_time:.2f} seconds")
print(f"Average speed: {(end_time - start_time) / 1000 * 1000:.1f} ms/step")
print(f"Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
```

**Step 4: Explore Brain Statistics**
```python
import pynlm

# Setup
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(500):
    brain.step(step)

# Explore statistics
print("\n=== Brain Statistics ===")
print(f"Total neurons: {brain.getTotalNeuronCount()}")
print(f"Total synapses: {brain.getTotalSynapseCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"Firing neurons: {brain.getFiringNeuronCount()}")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Developmental stage: {brain.getDevelopmentalStage()}")

# Save brain state (if available)
try:
    brain.save("first_brain.bin")
    print("✓ Brain state saved")
except:
    print("ℹ Brain save not available (expected for basic version)")
```

#### Step 5: Try Agent and World

**Basic Agent Simulation**
```python
import pynlm

# Setup brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

print("✓ Agent and world created")
print(f"  World size: {world.getWidth()}x{world.getHeight()}")
print(f"  Agent start: (10.0, 10.0)")
print(f"  Vision range: {world.getVisionWidth()}x{world.getVisionHeight()}")
```

**Agent Simulation Loop**
```python
import pynlm

# Setup all components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

print("Starting agent simulation...")

# Run simulation
for step in range(200):
    # Update world
    world.update(0.1)
    
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processes
    brain.step(step)
    
    # Get action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    if step % 50 == 0:
        print(f"  Step {step}: "
              f"Firing: {brain.getFiringNeuronCount()}, "
              f"Curiosity: {agent.getCuriosityLevel():.3f}, "
              f"Novelty: {agent.getNoveltyLevel():.3f}")

print("✓ Agent simulation complete!")
```

#### Step 6: Save and Load

**Save Configuration and State**
```python
import pynlm

# Create and run brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

for step in range(100):
    brain.step(step)

# Save configuration
config.saveToFile("my_config.json")
print("✓ Configuration saved to my_config.json")

# Save brain state (if available)
try:
    brain.save("my_brain.bin")
    print("✓ Brain state saved to my_brain.bin")
except:
    print("ℹ Brain save not available")

# Save statistics to file
with open("simulation_results.txt", 'w') as f:
    f.write(f"Neurons: {brain.getTotalNeuronCount()}\n")
    f.write(f"Firing rate: {brain.getAverageFiringRate()}\n")
    f.write(f"Total spikes: {brain.getTotalSpikeCount()}\n")
print("✓ Statistics saved to simulation_results.txt")
```

**Load Configuration and State**
```python
import pynlm

# Load configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("my_config.json")
print("✓ Configuration loaded")

# Load brain state (if available)
try:
    new_brain = pynlm.createBrain(config)
    new_brain.load("my_brain.bin")
    print("✓ Brain state loaded")
except:
    print("ℹ Brain load not available")

# Load statistics
with open("simulation_results.txt", 'r') as f:
    content = f.read()
    print(f"\nLoaded statistics:\n{content}")
```

#### Step 7: Troubleshooting Common Issues

**Common Problems and Solutions**

1. **"Brain isn't doing anything"**
```python
# Problem: Brain not initialized
# Solution: Call brain.initialize() before any operations
brain = pynlm.createBrain(config)
brain.initialize()  # Must be called first!
brain.step(0)  # Now this works
```

2. **"ImportError: No module named 'pynlm'"**
```bash
# Install pynlm
pip install pynlm

# Or install from source
pip install -e .
```

3. **"Agent isn't moving"**
```python
# Problem: Missing world update and action application
# Solution: Ensure both calls are made
world.update(0.1)           # Update world state
agent.decodeMotorCommand()   # Get action from brain
world.applyMotorCommand(action, time)  # Apply action to world
```

4. **"Everything is 0"**
```python
# Problem: Network needs time to warm up
# Solution: Run more steps or check initial conditions
for step in range(500):  # More steps
    brain.step(step)
```

#### Step 8: Next Steps

**Continue Learning**
```python
# Read more documentation
# - HOW_TO_USE.md: Detailed API documentation
# - docs/ARCHITECTURE.md: System architecture
# - docs/SCIENCE.md: Scientific background
# - docs/PERFORMANCE.md: Performance optimization

# Explore examples
# - examples/ directory: Advanced use cases
# - performance_benchmarks.md: Performance testing
# - development_experiment.md: Research experiments
```

**Connect with Community**
```python
# Join NLM community
# - GitHub repository: https://github.com/nlm-project/nlm
# - Documentation: https://docs.nlm.org
# - Discussion forums: https://discuss.nlm.org
# - Slack/ Discord: NLM community channels
```

### Summary

You now have:
- ✅ NLM installed and configured
- ✅ Basic brain creation and initialization
- ✅ Running simulations with statistics
- ✅ Agent-world interaction
- ✅ Configuration and state management
- ✅ Troubleshooting skills

You're ready to explore NLM further! Next steps include reading detailed documentation, trying advanced examples, and contributing to the NLM community.

---

*Guide Version: 1.0*
*Last Updated: 2026-10-07*
*Status: Getting Started*

---

## Beginner: Basic Operations

### 1. Brain Creation and Initialization

#### Learning Objectives
- Create brain instances
- Initialize brain state
- Understand brain parameters
- Run basic simulations

#### Detailed Explanation

**Brain Creation**
```python
import pynlm

# Method 1: Use default configuration
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Method 2: Custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.v_thresh", -50.0)
config.set("brain.synapse_density", 0.1)
brain = pynlm.createBrain(config)
```

**Key Brain Parameters**
- `brain.neuron_count`: Total number of neurons
- `brain.synapse_density`: Connection probability between neurons
- `brain.v_thresh`: Neuron firing threshold (mV)
- `brain.tau_mem`: Membrane time constant (ms)
- `brain.tau_ref`: Refractory period (ms)

**Brain Initialization**
```python
# Initialize brain (required before simulation)
brain.initialize()

# Common initialization tasks
# - Allocate memory for neurons and synapses
# - Set initial membrane potentials
# - Initialize plasticity rules
# - Set up spike generation mechanisms
```

#### Code Examples

**Example 1: Simple Brain Creation**
```python
import pynlm

# Create brain with default settings
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain has {brain.getTotalSynapseCount()} synapses")
```

**Example 2: Custom Brain with Parameters**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Modify key parameters
config.set("brain.neuron_count", 2000)           # More neurons
config.set("brain.synapse_density", 0.05)        # Sparse connectivity
config.set("brain.v_thresh", -45.0)              # Lower threshold
config.set("brain.tau_mem", 15.0)                # Slower dynamics
config.set("plasticity.stdp.learning_rate", 0.001)  # Fine learning

# Create and initialize
brain = pynlm.createBrain(config)
brain.initialize()

print(f"Custom brain created:")
print(f"  - Neurons: {brain.getTotalNeuronCount()}")
print(f"  - Synapses: {brain.getTotalSynapseCount()}")
print(f"  - Threshold: {config.get('brain.v_thresh')} mV")
print(f"  - Learning rate: {config.get('plasticity.stdp.learning_rate')}")
```

**Example 3: Brain Statistics and Monitoring**
```python
import pynlm

# Setup and run simulation
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

for step in range(500):
    brain.step(step)
    
    # Collect statistics every 100 steps
    if step % 100 == 0:
        print(f"Step {step}:")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Excitatory/Inhibitory ratio: {brain.getExcitationInhibitionRatio():.2f}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")

# Final statistics
print(f"\nFinal Statistics:")
print(f"  Total neurons: {brain.getTotalNeuronCount()}")
print(f"  Total synapses: {brain.getTotalSynapseCount()}")
print(f"  Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
print(f"  Total spikes: {brain.getTotalSpikeCount()}")
```

### 2. World and Agent Operations

#### Learning Objectives
- Create and configure world environments
- Understand agent-brain interfaces
- Simulate agent-environment interaction
- Enable and use learning subsystems

#### Detailed Explanation

**World Creation and Configuration**
```python
import pynlm

# Create world
world = pynlm.createSimpleWorld()

# Configure world parameters
world.configure(
    width=20,              # World width (cells)
    height=20,             # World height (cells)
    visionWidth=8,         # Agent vision width
    visionHeight=8         # Agent vision height
)

# Reset world to initial state
world.reset()

# Set agent starting position
world.setAgentStart(x=10.0, y=10.0)
```

**Agent Creation and Initialization**
```python
import pynlm

# Create brain for agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Create agent interface
agent = pynlm.createAgentBrain(brain)

# Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Initialize agent with world
agent.initialize(world)
```

#### Code Examples

**Example 1: Basic Agent Setup**
```python
import pynlm

# Create brain and agent
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

# Set agent starting position
world.setAgentStart(10.0, 10.0)

# Initialize agent
agent.initialize(world)

print("✓ Agent and world setup complete")
print(f"  World: {world.getWidth()}x{world.getHeight()} cells")
print(f"  Vision: {world.getVisionWidth()}x{world.getVisionHeight()} cells")
```

**Example 2: Complete Agent Simulation**
```python
import pynlm

# Setup simulation components
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

agent = pynlm.createAgentBrain(brain)
world = pynlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(15.0, 15.0)

agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)
agent.enableDevelopment(True)
agent.enableStructuralPlasticity(True)

print("Starting simulation...")

# Run simulation loop
for step in range(200):
    # Update world state
    world.update(0.1)
    
    # Get sensory input
    percept = world.getSensoryPercept()
    
    # Process sensory input
    agent.processSensoryInput(percept)
    
    # Brain processes information
    brain.step(step)
    
    # Get motor command
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress
    if step % 50 == 0:
        print(f"  Step {step}:")
        print(f"    Firing neurons: {brain.getFiringNeuronCount():4d}")
        print(f"    Agent position: ({world.getAgentBody().getX():.1f}, {world.getAgentBody().getY():.1f})")
        print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"    Development stage: {brain.getDevelopmentalStage()}")

print("✓ Simulation complete!")
```

**Example 3: Multi-Agent Simulation**
```python
import pynlm

def create_agent_world(num_agents=3):
    """Create world with multiple agents"""
    
    # Create shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=50, height=50, visionWidth=10, visionHeight=10)
    world.reset()
    
    # Create agents with different behaviors
    agents = []
    brains = []
    
    for i in range(num_agents):
        # Create unique configuration for each agent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 1000)
        
        # Vary parameters for diversity
        config.set("brain.v_thresh", -50.0 + i * 2.0)
        config.set("plasticity.stdp.learning_rate", 0.001 * (i + 1))
        
        # Create brain and agent
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Set different starting positions
        x = 5 + i * 15
        y = 5 + i * 15
        world.setAgentStart(x, y)
        
        agents.append(agent)
        brains.append(brain)
    
    return world, agents, brains

# Run multi-agent simulation
world, agents, brains = create_agent_world(3)

# Simulation loop
for step in range(500):
    world.update(0.1)
    
    # Each agent perceives the same world
    percept = world.getSensoryPercept()
    
    # Each agent processes independently
    for i, agent in enumerate(agents):
        agent.processSensoryInput(percept)
        brains[i].step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())

print("✓ Multi-agent simulation complete!")
```

### 3. Configuration Management

#### Learning Objectives
- Understand configuration system
- Modify brain parameters
- Save and load configurations
- Use parameter validators

#### Detailed Explanation

**Configuration System**
```python
import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Available configuration methods
# config.set(key, value)      # Set parameter
# config.get(key, default)    # Get parameter
# config.has(key)             # Check if key exists
# config.loadFromFile(path)   # Load from file
# config.saveToFile(path)     # Save to file
# config.getKeys()            # Get all keys
```

**Common Configuration Categories**
- `brain.*`: Brain parameters (neuron count, thresholds, etc.)
- `plasticity.*`: Plasticity rule settings
- `neuromod.*`: Neuromodulation parameters
- `development.*`: Developmental stage settings
- `logger.*`: Logging configuration

#### Code Examples

**Example 1: Configuration with File I/O**
```python
import pynlm

# Create and customize configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 3000)
config.set("brain.v_thresh", -45.0)
config.set("brain.synapse_density", 0.08)
config.set("plasticity.stdp.learning_rate", 0.001)

# Save configuration to file
config.saveToFile("my_config.json")
print("✓ Configuration saved to my_config.json")

# Create new brain from saved config
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("my_config.json")
print("✓ Configuration loaded")

# Use loaded configuration
brain = pynlm.createBrain(new_config)
brain.initialize()
```

**Example 2: Configuration Validation**
```python
import pynlm

# Create configuration
config = pynlm.createDefaultConfig()

# Test parameter ranges
print("Configuration validation:")

# Valid parameters
print(f"✓ Neuron count: {config.get('brain.neuron_count')}")
print(f"✓ Synapse density: {config.get('brain.synapse_density')}")
print(f"✓ Threshold: {config.get('brain.v_thresh')} mV")

# Check if configuration has required keys
required_keys = ["brain.neuron_count", "brain.synapse_density"]
for key in required_keys:
    if config.has(key):
        print(f"✓ Has {key}")
    else:
        print(f"✗ Missing {key}")

# Get all configuration keys
all_keys = config.getKeys()
print(f"\nTotal configuration keys: {len(all_keys)}")
print("Sample keys:", all_keys[:5])
```

**Example 3: Performance Configuration**
```python
import pynlm

# Create performance-optimized configuration
config = pynlm.createDefaultConfig()

# High-performance settings
config.set("brain.neuron_count", 5000)                    # Large network
config.set("brain.synapse_density", 0.02)                 # Sparse connectivity
config.set("brain.v_thresh", -40.0)                      # Higher excitability
config.set("brain.tau_mem", 10.0)                        # Faster dynamics
config.set("plasticity.stdp.learning_rate", 0.005)       # Faster learning
config.set("neuromod.dopamine.scale", 1.5)               # Strong neuromodulation

# Create brain with performance configuration
brain = pynlm.createBrain(config)
brain.initialize()

print("✓ High-performance brain created")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Synapses: {brain.getTotalSynapseCount()}")
```

**Example 4: Development-Specific Configuration**
```python
import pynlm

# Create developmentally-sensitive configuration
config = pynlm.createDefaultConfig()

# Critical period configuration
config.set("brain.neuron_count", 2000)
config.set("brain.v_thresh", -50.0)
config.set("plasticity.stdp.learning_rate", 0.01)  # High learning
config.set("neuromod.dopamine.scale", 2.0)          # Strong modulation

# Create brain for critical period studies
brain = pynlm.createBrain(config)
brain.initialize()

print("✓ Critical period brain created")
print(f"  Neurons: {brain.getTotalNeuronCount()}")
print(f"  Learning rate: {config.get('plasticity.stdp.learning_rate')}")
```

### 4. State Management

#### Learning Objectives
- Save brain state
- Load brain state
- Use checkpoint systems
- Manage simulation history

#### Detailed Explanation

**Brain State Management**
```python
import pynlm

# Save and load brain state
# brain.save(filepath)  # Save current state
# brain.load(filepath)  # Load saved state
```

**Checkpoint Systems**
```python
# Advanced checkpoint management
# - Multiple checkpoints
# - Automatic saving
# - Checkpoint validation
# - Checkpoint compression
```

#### Code Examples

**Example 1: Basic State Management**
```python
import pynlm

# Create and run brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Save state (if available)
try:
    brain.save("simulation_state.bin")
    print("✓ Brain state saved to simulation_state.bin")
except:
    print("ℹ Brain save not available")

# Create new brain and load state (if available)
try:
    new_brain = pynlm.createBrain(config)
    new_brain.load("simulation_state.bin")
    print("✓ Brain state loaded")
except:
    print("ℹ Brain load not available")
```

**Example 2: Configuration and State Combined**
```python
import pynlm
import json

# Create brain with custom configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
config.set("brain.v_thresh", -50.0)

brain = pynlm.createBrain(config)
brain.initialize()

# Run simulation
for step in range(200):
    brain.step(step)

# Save everything
config.saveToFile("simulation_config.json")
print("✓ Configuration saved")

# Save statistics
stats = {
    'final_neuron_count': brain.getTotalNeuronCount(),
    'final_firing_rate': brain.getAverageFiringRate(),
    'total_spikes': brain.getTotalSpikeCount(),
    'simulation_steps': 200
}

with open("simulation_stats.json", 'w') as f:
    json.dump(stats, f, indent=2)
print("✓ Statistics saved")

# Save brain state (if available)
try:
    brain.save("simulation_checkpoint.bin")
    print("✓ Brain checkpoint saved")
except:
    print("ℹ Brain checkpoint not available")
```

**Example 3: Load and Continue**
```python
import pynlm

# Load configuration
config = pynlm.createDefaultConfig()
config.loadFromFile("simulation_config.json")
print("✓ Configuration loaded")

# Load statistics
with open("simulation_stats.json", 'r') as f:
    loaded_stats = json.load(f)
    print(f"Loaded stats: {loaded_stats}")

# Create brain with loaded configuration
brain = pynlm.createBrain(config)

# Load brain state (if available)
try:
    brain.load("simulation_checkpoint.bin")
    print("✓ Brain checkpoint loaded")
    
    # Continue simulation
    for step in range(loaded_stats['simulation_steps'], loaded_stats['simulation_steps'] + 100):
        brain.step(step)
        
    print(f"✓ Continued simulation from step {loaded_stats['simulation_steps']}")
    print(f"  Final firing rate: {brain.getAverageFiringRate():.2f} Hz")
    
except:
    print("ℹ Brain checkpoint not available, running fresh simulation")
    brain.initialize()
    
    for step in range(300):  # Run full simulation
        brain.step(step)
```

### 5. Error Handling and Troubleshooting

#### Learning Objectives
- Identify common errors
- Fix configuration issues
- Debug simulation problems
- Handle edge cases

#### Common Issues and Solutions

**Issue 1: Brain Not Initialized**
```python
# Problem: Brain not initialized before simulation
# Solution: Always call brain.initialize() first

# ❌ Wrong
brain = pynlm.createBrain(config)
brain.step(0)  # Error!

# ✅ Correct
brain = pynlm.createBrain(config)
brain.initialize()  # Required!
brain.step(0)  # Now works
```

**Issue 2: Import Error**
```bash
# Problem: Cannot import pynlm
# Solution: Check installation

# Install pynlm
pip install pynlm

# Or install from source
pip install -e .

# Verify installation
python3 -c "import pynlm; print('NLM version:', pynlm.__version__)"
```

**Issue 3: Agent Not Moving**
```python
# Problem: Agent not responding to world
# Solution: Check simulation loop

# Ensure proper simulation sequence
world.update(0.1)                    # Update world state
agent.processSensoryInput(percept)    # Process input
brain.step(step)                      # Brain processes
action = agent.decodeMotorCommand()   # Get decision
world.applyMotorCommand(action, time) # Execute action
```

**Issue 4: High Memory Usage**
```python
# Problem: Memory usage too high
# Solution: Optimize configuration

# Create memory-efficient configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)              # Fewer neurons
config.set("brain.synapse_density", 0.01)           # Sparse connectivity
config.set("plasticity.stdp.learning_rate", 0.0001) # Slower learning

brain = pynlm.createBrain(config)
brain.initialize()
```

**Issue 5: Slow Simulation**
```python
# Problem: Simulation too slow
# Solution: Optimize parameters

# Create high-performance configuration
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 500)               # Smaller network
config.set("brain.synapse_density", 0.05)            # Dense connectivity
config.set("brain.v_thresh", -40.0)                 # Higher threshold
config.set("brain.tau_mem", 5.0)                    # Faster dynamics

brain = pynlm.createBrain(config)
brain.initialize()
```

#### Code Examples

**Example 1: Robust Error Handling**
```python
import pynlm

def safe_simulation(config, steps=100):
    """Run simulation with error handling"""
    
    try:
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(steps):
            brain.step(step)
            
        # Collect results
        results = {
            'steps_completed': steps,
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount(),
            'status': 'success'
        }
        
        return results
        
    except ImportError as e:
        return {
            'steps_completed': 0,
            'error': f"Import error: {e}",
            'status': 'import_error'
        }
    
    except Exception as e:
        return {
            'steps_completed': steps if 'step' in locals() else 0,
            'error': str(e),
            'status': 'simulation_error'
        }

# Test with different configurations
test_configs = [
    ("Default", pynlm.createDefaultConfig()),
    ("Custom", pynlm.createDefaultConfig())
]

for name, config in test_configs:
    config.set("brain.neuron_count", 1000 if name == "Default" else 500)
    
    results = safe_simulation(config, 50)
    print(f"{name}: {results['status']}")
    if 'error' in results:
        print(f"  Error: {results['error']}")
    else:
        print(f"  Steps: {results['steps_completed']}")
        print(f"  Final firing rate: {results['final_firing_rate']:.2f} Hz")
```

**Example 2: Configuration Validation**
```python
import pynlm

def validate_configuration(config):
    """Validate configuration for common issues"""
    
    issues = []
    warnings = []
    
    # Check neuron count
    neuron_count = config.get("brain.neuron_count", 1000)
    if neuron_count < 100:
        warnings.append("Very small network may not exhibit interesting dynamics")
    elif neuron_count > 100000:
        warnings.append("Very large network may be slow to simulate")
    
    # Check synapse density
    synapse_density = config.get("brain.synapse_density", 0.1)
    if synapse_density < 0.001:
        warnings.append("Very sparse connectivity may isolate neurons")
    elif synapse_density > 0.5:
        warnings.append("Dense connectivity may lead to synchronization")
    
    # Check threshold
    threshold = config.get("brain.v_thresh", -50.0)
    if threshold < -70:
        warnings.append("Very low threshold may cause excessive firing")
    elif threshold > -40:
        warnings.append("Very high threshold may cause little activity")
    
    # Check learning rate
    learning_rate = config.get("plasticity.stdp.learning_rate", 0.001)
    if learning_rate < 0.00001:
        warnings.append("Very low learning rate may cause little adaptation")
    elif learning_rate > 0.1:
        warnings.append("Very high learning rate may cause instability")
    
    return {
        'issues': issues,
        'warnings': warnings,
        'is_valid': len(issues) == 0
    }

# Test validation
config = pynlm.createDefaultConfig()
validation = validate_configuration(config)

print("Configuration Validation:")
if validation['is_valid']:
    print("✓ Configuration is valid")
else:
    print("✗ Configuration issues found:")
    for issue in validation['issues']:
        print(f"  - {issue}")

if validation['warnings']:
    print("Warnings:")
    for warning in validation['warnings']:
        print(f"  - {warning}")
```

**Example 3: Simulation Recovery**
```python
import pynlm

def robust_simulation(config, steps=100, checkpoint_file="checkpoint.bin"):
    """Run simulation with checkpoint recovery"""
    
    try:
        # Try to load checkpoint
        try:
            brain = pynlm.createBrain(config)
            brain.load(checkpoint_file)
            print(f"✓ Loaded checkpoint from {checkpoint_file}")
            start_step = 1000  # Assume we saved step number
        except:
            # Create new brain
            brain = pynlm.createBrain(config)
            brain.initialize()
            print("✓ Created new brain")
            start_step = 0
        
        # Run simulation
        for step in range(start_step, start_step + steps):
            brain.step(step)
            
            # Save checkpoint periodically
            if step % 100 == 0:
                try:
                    brain.save(checkpoint_file)
                    print(f"✓ Checkpoint saved at step {step}")
                except:
                    print("ℹ Checkpoint save not available")
        
        return {
            'steps_completed': steps,
            'checkpoint_saved': True,
            'status': 'success'
        }
        
    except Exception as e:
        return {
            'steps_completed': start_step if 'start_step' in locals() else 0,
            'error': str(e),
            'status': 'error'
        }

# Test robust simulation
config = pynlm.createDefaultConfig()
results = robust_simulation(config, steps=50)

print(f"\nSimulation Results:")
print(f"Status: {results['status']}")
print(f"Steps completed: {results['steps_completed']}")
if 'error' in results:
    print(f"Error: {results['error']}")
```

### 6. Performance Monitoring

#### Learning Objectives
- Monitor simulation performance
- Track resource usage
- Identify performance bottlenecks
- Optimize for efficiency

#### Detailed Explanation

**Performance Monitoring**
```python
import time
import psutil
import pynlm

# Monitor system resources
process = psutil.Process()
```

**Key Performance Metrics**
- **Simulation speed**: Steps per second
- **Memory usage**: MB consumed
- **CPU usage**: Percentage utilization
- **Firing rate**: Neural activity level
- **Network statistics**: Connectivity measures

#### Code Examples

**Example 1: Performance Monitoring**
```python
import time
import psutil
import pynlm

def monitor_performance(brain, steps=100):
    """Monitor simulation performance"""
    
    # Get current process
    process = psutil.Process()
    
    # Track performance metrics
    metrics = {
        'start_time': time.time(),
        'start_memory': process.memory_info().rss / 1024 / 1024,  # MB
        'start_cpu': process.cpu_percent(),
        
        'steps_completed': 0,
        'steps_per_second': 0,
        'memory_usage': [],
        'cpu_usage': [],
        'firing_rates': []
    }
    
    # Run simulation with monitoring
    for step in range(steps):
        # Record current usage
        metrics['memory_usage'].append(process.memory_info().rss / 1024 / 1024)
        metrics['cpu_usage'].append(process.cpu_percent())
        metrics['firing_rates'].append(brain.getAverageFiringRate())
        
        # Run brain step
        brain.step(step)
        metrics['steps_completed'] += 1
        
        # Print progress
        if step % 20 == 0:
            elapsed = time.time() - metrics['start_time']
            print(f"Step {step}: {elapsed:.2f}s elapsed, "
                  f"{metrics['steps_completed']/elapsed:.1f} steps/s")
    
    # Calculate final metrics
    elapsed = time.time() - metrics['start_time']
    metrics['total_time'] = elapsed
    metrics['steps_per_second'] = metrics['steps_completed'] / elapsed
    metrics['final_memory'] = process.memory_info().rss / 1024 / 1024
    metrics['final_cpu'] = process.cpu_percent()
    
    return metrics

# Run performance monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

performance = monitor_performance(brain, 200)

print(f"\nPerformance Summary:")
print(f"Total time: {performance['total_time']:.2f} seconds")
print(f"Steps per second: {performance['steps_per_second']:.1f}")
print(f"Average memory: {sum(performance['memory_usage'])/len(performance['memory_usage']):.1f} MB")
print(f"Average CPU: {sum(performance['cpu_usage'])/len(performance['cpu_usage']):.1f}%")
print(f"Final firing rate: {performance['firing_rates'][-1]:.2f} Hz")
```

**Example 2: Performance Comparison**
```python
import pynlm
import matplotlib.pyplot as plt

def compare_performance(configurations, steps=100):
    """Compare performance of different configurations"""
    
    results = {}
    
    for name, config in configurations.items():
        print(f"Testing {name}...")
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Time the simulation
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        end_time = time.time()
        
        # Collect metrics
        results[name] = {
            'time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'final_firing_rate': brain.getAverageFiringRate(),
            'total_spikes': brain.getTotalSpikeCount()
        }
        
        print(f"  {results[name]['steps_per_second']:.1f} steps/s")
    
    return results

# Compare different configurations
configs = {
    'Small': pynlm.createDefaultConfig(),
    'Medium': pynlm.createDefaultConfig(),
    'Large': pynlm.createDefaultConfig()
}

# Customize configurations
configs['Small'].set("brain.neuron_count", 500)
configs['Medium'].set("brain.neuron_count", 2000)
configs['Large'].set("brain.neuron_count", 5000)

# Run comparison
comparison_results = compare_performance(configs, 100)

# Plot results
plt.figure(figsize=(10, 6))

plt.subplot(2, 2, 1)
plt.bar([r['name'] for r in comparison_results.values()], 
        [r['steps_per_second'] for r in comparison_results.values()])
plt.title('Steps per Second')
plt.ylabel('Steps/s')

plt.subplot(2, 2, 2)
plt.bar([r['name'] for r in comparison_results.values()], 
        [r['final_firing_rate'] for r in comparison_results.values()])
plt.title('Final Firing Rate')
plt.ylabel('Hz')

plt.subplot(2, 2, 3)
plt.bar([r['name'] for r in comparison_results.values()], 
        [r['total_spikes'] for r in comparison_results.values()])
plt.title('Total Spikes')
plt.ylabel('Count')

plt.tight_layout()
plt.show()
```

**Example 3: Real-time Performance**
```python
import pynlm
import time
import psutil

def real_time_monitor(brain, duration=60):
    """Monitor simulation in real-time"""
    
    process = psutil.Process()
    start_time = time.time()
    
    print(f"Running real-time simulation for {duration} seconds...")
    print(f"{'Step':>6} {'Time':>8} {'Speed':>10} {'Memory':>10} {'CPU':>8} {'Firing Rate':>12}")
    print("-" * 70)
    
    step = 0
    while time.time() - start_time < duration:
        # Record current state
        current_time = time.time() - start_time
        memory_mb = process.memory_info().rss / 1024 / 1024
        cpu_percent = process.cpu_percent()
        firing_rate = brain.getAverageFiringRate()
        
        # Print status
        print(f"{step:>6} {current_time:>8.1f} {1/current_time*1000:>9.1f} "
              f"{memory_mb:>9.1f} {cpu_percent:>7.1f} {firing_rate:>12.2f}")
        
        # Run simulation step
        brain.step(step)
        step += 1
        
        # Small delay to simulate real-time
        time.sleep(0.1)
    
    print(f"\nReal-time simulation complete!")
    print(f"Total steps: {step}")
    print(f"Average speed: {step / duration:.1f} steps/s")

# Run real-time monitoring
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

real_time_monitor(brain, duration=30)
```

### Summary

This chapter covers essential user guide concepts for working with NLM:

1. **Getting Started Guide**: Complete tutorial for new users
2. **Basic Operations**: Core functionality and operations
3. **Brain Operations**: Creating and initializing brains
4. **World and Agent Operations**: Environment and agent handling
5. **Configuration Management**: Parameter and setting management
6. **State Management**: Saving and loading simulation state
7. **Error Handling**: Troubleshooting and debugging
8. **Performance Monitoring**: Performance tracking and optimization

The guides provide comprehensive coverage of NLM functionality while remaining accessible to users at all skill levels. Each section includes practical examples and clear explanations to help users master NLM effectively.

---

*User Guide Version: 1.0*
*Last Updated: 2026-10-07*
*Status: Complete*