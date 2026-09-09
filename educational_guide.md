# NLM Educational Guide

## Learning Path Overview

This guide is designed to help you learn the NLM (Neural Learning Machine) framework systematically. Whether you're a complete beginner or have some programming experience, this guide will take you from basic concepts to advanced usage.

### What You'll Learn

1. **Foundations** - Neural networks, brain simulation, and core concepts
2. **Getting Started** - Setting up your environment and running your first simulation
3. **Core Components** - Understanding the brain, world, and agent systems
4. **Working Examples** - Step-by-step tutorials and mini-projects
5. **Advanced Topics** - Deep dive into learning and customization

## Prerequisites

### Before You Begin

- Basic Python programming knowledge (recommended)
- Understanding of neural networks is helpful but not required
- No prior experience with NLM needed!

### Required Software

```bash
# Create a Python virtual environment (recommended)
python -m venv nlm_env
source nlm_env/bin/activate  # On Windows: nlm_env\Scripts\activate

# Install NLM and dependencies
pip install numpy pytest
pip install .  # Install NLM from current directory
```

## Module 1: Understanding the Brain

### 1.1 What is a Brain?

Think of a biological brain as a collection of neurons that communicate through electrical signals called spikes. NLM creates a simulated brain with:

- **Neurons**: Cell-like units that process information
- **Synapses**: Connections between neurons that can strengthen or weaken
- **Spiking dynamics**: Realistic electrical activity patterns
- **Learning**: Automatic adjustment of connections based on experience

### 1.2 Key Terms

| Term | Description |
|------|-------------|
| **Neuron** | Basic processing unit that receives signals and can fire |
| **Synapse** | Connection between neurons that transmits signals |
| **Spike** | Electrical signal sent by a neuron |
| **Weight** | Strength of a synaptic connection |
| **LIF Model** | Leaky Integrate-and-Fire neuron model (realistic neurons) |

### 1.3 Analogy

```
Biological Brain                              Simulated Brain
---------------------------------           -------------------------------
Neurons                                 ->   Python classes in src/brain/
Synapses                               ->   Connection objects
Axons/Dendrites                        ->   Signal transmission pathways
Neurotransmitters                      ->   Synaptic modulation
Learning (Synaptic Plasticity)         ->   STDP, Hebbian, Structural Plasticity
```

## Module 2: Setting Up Your Environment

### 2.1 Installation Steps

Copy and paste this into your terminal:

```python
# Step 1: Import NLM
import pynlm

# Step 2: Create configuration
config = pynlm.createDefaultConfig()

# Step 3: Create brain
brain = pynlm.createBrain(config)

# Step 4: Initialize brain
brain.initialize()

print("Brain created and initialized!")
```

### 2.2 Troubleshooting Common Issues

**Issue**: "ModuleNotFoundError: No module named 'pynlm'"
**Solution**: Run `pip install .` or `pip install pynlm`

**Issue**: "Segmentation fault"
**Solution**: Always call `brain.initialize()` before `brain.step()`

**Issue**: "Brain isn't doing anything"
**Solution**: 
- Check that `brain.initialize()` was called
- Try more simulation steps
- Check that sensory input is connected

## Module 3: Your First Simulation

### 3.1 Exercise 1: Silent Brain

This creates a brain that just exists without any external input:

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Make it think for a few steps
for step in range(10):
    brain.step(step)

print(f"Brain has {brain.getTotalNeuronCount()} neurons")
print(f"Total spikes: {brain.getTotalSpikeCount()}")
```

**Learning Points**:
- Brain creation and initialization
- Basic simulation loop
- Accessing brain statistics

### 3.2 Exercise 2: Brain with Sensory Input

This creates a brain connected to a simple world:

```python
import pynlm

# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Create world and agent
world = pynlm.createSimpleWorld()
world.configure(width=10, height=10, visionWidth=8, visionHeight=8)
world.reset()

agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Run one simulation step
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(0)

print("Simulation step completed")
print(f"Agent curiosity level: {agent.getCuriosityLevel():.3f}")
```

**Learning Points**:
- Creating world and agent systems
- Sensory processing pipeline
- Motor output decoding

## Module 4: Pattern Recognition Exercise

### 4.1 Project: Simple Pattern Recognition

This project trains a brain to recognize patterns in a controlled environment:

```python
def run_pattern_recognition_simulation():
    """Train brain to recognize and respond to patterns"""
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enableRewardModulation(True)
    agent.enableCuriosity(True)
    
    # Training loop
    for step in range(1000):
        # 1. Update world
        world.update(0.1)
        
        # 2. Get what agent sees
        percept = world.getSensoryPercept()
        
        # 3. Tell brain what it sees
        agent.processSensoryInput(percept)
        
        # 4. Brain thinks
        brain.step(step)
        
        # 5. Get action from brain
        action = agent.decodeMotorCommand()
        
        # 6. Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # 7. Give brain feedback
        reward = calculate_reward(world, action)
        agent.applyRewardModulation(reward, 0.0)
        
        # Print progress
        if step % 100 == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons")
            print(f"Curiosity: {agent.getCuriosityLevel():.3f}")
    
    return brain, world, agent

# Helper function
def calculate_reward(world, action):
    """Calculate reward based on action in world"""
    # Simple reward: positive for certain actions, negative for others
    if action == pynlm.ActionType.MoveForward:
        return 0.5
    elif action == pynlm.ActionType.Interact:
        return 1.0
    elif action == pynlm.ActionType.TurnLeft:
        return 0.1
    else:
        return -0.1
```

### 4.2 Exercise Answers

Check your understanding with these concepts:

1. **What does `brain.step(0)` do?**
   - Updates all neurons based on current inputs and connections
   - Processes spikes and updates synaptic weights
   - Updates brain statistics and developmental stage

2. **What's the purpose of `agent.processSensoryInput()`?**
   - Translates world perception into neural signals
   - Injects current into sensory neurons based on visual input
   - Stores percept in working memory

3. **Why do we need both `world.update()` and `brain.step()`?**
   - World update changes environment state
   - Brain step processes neural computation based on that environment

## Module 5: Core Concepts Deep Dive

### 5.1 The Three-System Architecture

NLM uses a clean separation of concerns:

```
WORLD LAYER                           BRAIN LAYER                           AGENT LAYER
 ┌─────────────────┐                ┌─────────────────┐                ┌─────────────────┐
 │   Environment   │◄──────────────►│    Neural       │◄──────────────►│   Interface     │
 │   Simulation    │   Sensory      │    Processing   │   Motor        │
 │                 │   Input       │    Core         │   Output       │
 │  - Physics      │   - Vision     │  - Neurons      │  - Actions     │
 │  - Resources    │  - Touch       │  - Synapses      │  - Decisions   │
 │  - Agents       │  - Internal    │  - Plasticity   │               │
 │                 │  - Proprioception│  - Memory      │               │
 └─────────────────┘                └─────────────────┘                └─────────────────┘
```

### 5.2 Learning Mechanisms

NLM implements multiple learning systems:

1. **STDP (Spike-Timing-Dependent Plasticity)**
   - Changes synapse strength based on spike timing
   - More precise than Hebbian learning

2. **Hebbian Learning**
   - "Neurons that fire together, wire together"
   - Simpler but effective

3. **Structural Plasticity**
   - Creates and removes synapses
   - Allows the brain to grow and adapt

4. **Reward Modulation**
   - Learns from outcomes
   - Guides exploration vs. exploitation

### 5.3 Memory Systems

NLM has multiple memory types:

- **Working Memory**: Short-term storage of current information
- **Episodic Memory**: Sequence of experiences
- **Semantic Memory**: Abstract knowledge about the world
- **Procedural Memory**: Skills and habits

## Module 6: Advanced Topics

### 6.1 Customization

#### Custom Configuration

```python
# Create custom brain configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 2000)
config.set("plasticity.stdp.learning_rate", 0.01)
config.set("neuromod.dopamine.scale", 2.0)

# Save configuration
config.saveToFile("custom_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.loadFromFile("custom_config.json")
```

#### Enabling Learning Features

```python
# Enable all learning systems
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# The brain will automatically:
# - Adapt based on rewards
# - Grow new connections
# - Mature over time
# - Explore novel situations
```

### 6.2 Performance Optimization

#### For Larger Simulations

```python
# Use more neurons for complex tasks
config.set("brain.neuron_count", 5000)

# Adjust synapse density for better connectivity
config.set("brain.synapse_density", 0.15)

# Longer simulation for learning
for step in range(10000):
    world.update(0.05)  # Smaller timestep for accuracy
    brain.step(step)
```

## Module 7: Project Gallery

### 7.1 Project Ideas

Here are some project ideas to practice with NLM:

1. **Maze Navigation**: Train brain to navigate a maze
2. **Pattern Association**: Link visual patterns to actions
3. **Memory Span**: Test working memory capacity
4. **Social Learning**: Share knowledge between agents
5. **Development Simulation**: Watch brain maturation

### 7.2 Complete Example: Multi-Agent System

```python
import pynlm
import numpy as np

def create_multi_agent_system(num_agents=3):
    """Create a system with multiple interacting agents"""
    
    # Shared world
    world = pynlm.createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agents = []
    
    for i in range(num_agents):
        # Create unique brain for each agent
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", 500 + i * 200)  # Different sizes
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        
        agents.append(agent)
        
        # Place agent at different starting positions
        start_x = 5 + i * 8
        start_y = 5 + i * 6
        world.setAgentStart(start_x, start_y)
    
    return world, agents

# Run simulation
world, agents = create_multi_agent_system()

for step in range(2000):
    # Update world
    world.update(0.1)
    
    # Get percept for all agents
    percept = world.getSensoryPercept()
    
    # Each agent processes input
    for agent in agents:
        agent.processSensoryInput(percept)
    
    # All brains think simultaneously
    for i, agent in enumerate(agents):
        agents[i].getBrain().step(step)
    
    # Each agent decides on action
    for i, agent in enumerate(agents):
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Give feedback
        reward = calculate_social_reward(world, agents, i)
        agent.applyRewardModulation(reward, 0.0)

print(f"Multi-agent simulation completed with {len(agents)} agents")
```

## Module 8: Troubleshooting Guide

### 8.1 Common Errors and Solutions

**Error 1: "Brain state save not implemented"**

```python
# Instead of returning false, implement save:
bool Brain::save(const std::string& filepath) const {
    // Save neural states, synapses, memory, etc.
    // Return true on success
}
```

**Error 2: Memory "not used" warnings**

```cpp
// Fix by actually instantiating and using memory systems:
brain.pImpl->workingMemory = std::make_unique<NeuralWorkingMemory>();
brain.pImpl->workingMemory->initialize();
```

**Error 3: Dead neurons (no spikes)**

```python
# Solution: Add more input or lower thresholds
config.set("brain.v_thresh", -40.0)  # Lower threshold
world.setAgentStart(5, 5)  # Start near resources
```

### 8.2 Debug Tips

1. **Use logging**: Enable verbose logging to track brain state
2. **Check initialization**: Always verify `brain.initialize()` is called
3. **Inspect percepts**: Print sensory input to debug input flow
4. **Monitor development**: Watch developmental stage progression
5. **Visualize**: Use NLM's visualization tools (Phase 6+)

## Module 9: Next Steps

### 9.1 Advanced Learning

After mastering the basics:

1. **Read the Architecture Documentation** (`docs/ARCHITECTURE.md`)
2. **Study the Science Papers** (`docs/SCIENCE.md`)
3. **Explore Phase 6+ Features** (`docs/PHASE6_FINAL_AUDIT.md`)
4. **Contribute to the Codebase** - Help connect the disconnected systems!

### 9.2 Development Path

```
Beginner -> Easy Usage Guide -> HOW_TO_USE.md -> Architecture Documentation
     -> Phase 6 Final Audit -> Advanced Configuration -> Research Papers
```

### 9.3 Research Opportunities

The NLM codebase has many research opportunities:

- **Connect disconnected systems** (memories, cognition, prediction)
- **Improve learning algorithms** (novelty, curiosity, reward)
- **Add biological realism** (ion channels, detailed neurons)
- **Create benchmarks** for measuring learning performance
- **Develop visualization tools** for understanding brain dynamics

## Appendices

### A.1 API Reference

Quick lookup of key NLM classes and methods:

#### Brain Class
- `createBrain(config)` - Create a new brain
- `initialize()` - Initialize brain state
- `step(step_number)` - Process one simulation step
- `getTotalNeuronCount()` - Get number of neurons
- `getTotalSpikeCount()` - Get total spikes fired
- `save/load()` - Persist brain state

#### AgentBrain Class
- `createAgentBrain(brain)` - Create agent interface
- `initialize(world)` - Connect to world
- `processSensoryInput(percept)` - Handle sensory input
- `decodeMotorCommand()` - Get motor action
- `applyRewardModulation()` - Learn from rewards

#### SimpleWorld Class
- `createSimpleWorld()` - Create world
- `configure()` - Set world dimensions
- `update(timestep)` - Advance time
- `getSensoryPercept()` - Get what agent sees
- `applyMotorCommand()` - Execute action

### A.2 Common Patterns

**Pattern 1: Basic Simulation Loop**
```python
world.update(0.1)
percept = world.getSensoryPercept()
agent.processSensoryInput(percept)
brain.step(step)
action = agent.decodeMotorCommand()
world.applyMotorCommand(action, time)
```

**Pattern 2: Learning Loop**
```python
# Enable learning first
agent.enableRewardModulation(True)

# In your main loop
reward = calculate_reward()
agent.applyRewardModulation(reward, predicted_reward)
```

**Pattern 3: Monitoring**
```python
# Track progress
if step % 100 == 0:
    print(f"Step {step}:")
    print(f"  Neurons firing: {brain.getFiringNeuronCount()}")
    print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
```

## Learning Commitment

This educational guide is designed to be your companion on the NLM learning journey. Take it step by step, experiment with the examples, and don't be afraid to try things on your own.

Remember:
- **Start simple**: Begin with the examples and build up
- **Iterate**: Modify and improve the code
- **Debug**: Troubleshoot is a core skill
- **Explore**: The code has many hidden features

Good luck on your neural learning journey!

---

*This guide is part of the NLM project and is continuously being improved. Suggest improvements at: https://github.com/nlm-project/nlm*