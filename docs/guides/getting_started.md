# NLM Getting Started Guide

## Overview

This guide provides a comprehensive introduction to the NLM (Neural Learning Machine) project. It covers the basics of brain architecture, key concepts, and provides step-by-step examples for getting started with NLM development.

## What is NLM?

NLM (熙然, meaning "serene flow") is a brain-inspired computational system that simulates neural dynamics, learning, and cognition. It implements Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation and multiple plasticity mechanisms.

### Key Features

- **Brain-like Architecture**: Neural populations, synapses, and regions
- **Multiple Learning Systems**: STDP, Hebbian, structural plasticity
- **Cognitive Capabilities**: Memory, prediction, attention, planning
- **Neuromodulation**: Dopamine, curiosity, novelty signals
- **Development**: Stage-based plasticity and maturation
- **Embodiment**: Integration with sensory-motor loops

## Quick Start

### Prerequisites

1. **Install NLM** (see Installation Guide)
2. **Basic understanding** of neural networks and brain concepts
3. **Familiarity** with C++ and/or Python

### First Brain Example

#### Method 1: Using Python (Recommended for beginners)

```python
import pynlm

# 1. Create configuration
config = pynlm.createDefaultConfig()

# 2. Create and initialize brain
brain = pynlm.createBrain(config)
brain.initialize()

# 3. Run simulation
print("Starting NLM simulation...")
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.firing_neuron_count} neurons firing")

# 4. Check results
print(f"\nSimulation complete!")
print(f"Total neurons: {brain.total_neuron_count}")
print(f"Average firing rate: {brain.average_firing_rate:.2f} Hz")
print(f"Excitatory/Inhibitory ratio: {brain.excitation_inhibition_ratio:.2f}")
```

#### Method 2: Using C++

```cpp
#include "brain/Brain.hpp"
#include <iostream>
#include <memory>

int main() {
    // 1. Create configuration
    NLM::Config config;
    config.set("brain.neuron_count", 1000);
    
    // 2. Create brain
    auto brain = std::make_unique<NLM::Brain>(config);
    brain->initialize();
    
    // 3. Run simulation
    std::cout << "Starting NLM simulation..." << std::endl;
    for (NLM::SimulationStep step = 0; step < 100; ++step) {
        brain->step(step);
        if (step % 10 == 0) {
            std::cout << "Step " << step 
                      << ": " << brain->getFiringNeuronCount() 
                      << " neurons firing" << std::endl;
        }
    }
    
    // 4. Check results
    std::cout << "\nSimulation complete!" << std::endl;
    std::cout << "Total neurons: " << brain->getTotalNeuronCount() << std::endl;
    std::cout << "Average firing rate: " << brain->getAverageFiringRate() << " Hz" << std::endl;
    
    return 0;
}
```

### Complete Agent Example

This example demonstrates NLM in a complete sensory-motor loop with an agent.

```python
import pynlm

def run_complete_agent_example():
    """Run a complete agent simulation with all systems."""
    
    print("=== Complete Agent Example ===\n")
    
    # 1. Setup configuration
    print("1. Creating configuration...")
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.synapse_density", 0.15)
    print(f"   - Neuron count: {config.get('brain.neuron_count')}")
    print(f"   - Synapse density: {config.get('brain.synapse_density')}")
    
    # 2. Create brain and initialize
    print("\n2. Creating and initializing brain...")
    brain = pynlm.createBrain(config)
    brain.initialize()
    print(f"   - Brain initialized with {brain.total_neuron_count} neurons")
    
    # 3. Create world
    print("\n3. Creating world simulation...")
    world = pynlm.createSimpleWorld()
    world.configure(
        width=30,
        height=30,
        vision_width=8,
        vision_height=8,
        timestep=0.1,
        max_energy=200.0
    )
    world.reset()
    world.set_agent_start(15.0, 15.0)
    print(f"   - World created: {world.width}x{world.height} cells")
    print(f"   - Agent start position: (15.0, 15.0)")
    
    # 4. Create agent brain interface
    print("\n4. Creating agent interface...")
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    print("   - Agent initialized with world")
    
    # 5. Enable cognitive subsystems
    print("\n5. Enabling cognitive subsystems...")
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    print("   - Reward modulation: ENABLED")
    print("   - Structural plasticity: ENABLED")
    print("   - Development: ENABLED")
    print("   - Curiosity: ENABLED")
    
    # 6. Run simulation loop
    print("\n6. Running simulation loop...")
    num_steps = 200
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process sensory input
        agent.process_sensory_input(percept)
        
        # Run brain step
        brain.step(step)
        
        # Get action from brain
        action = agent.decode_motor_command()
        
        # Apply action to world
        world.apply_motor_command(action, world.simulation_time)
        
        # Apply reward modulation
        reward = (world.get_sensory_percept().get_internal()[0] 
                  if world.get_sensory_percept().get_internal() 
                  else 0.0)
        agent.apply_reward_modulation(reward, 0.0)
        
        # Update development
        agent.update_development(0.1)
        
        # Print progress every 50 steps
        if step % 50 == 0:
            print(f"   Step {step}/{num_steps}:")
            print(f"     - Firing neurons: {brain.firing_neuron_count()}")
            print(f"     - Average firing rate: {brain.average_firing_rate():.2f} Hz")
            print(f"     - Curiosity level: {agent.get_curiosity_level():.3f}")
            print(f"     - Novelty level: {agent.get_novelty_level():.3f}")
            print(f"     - Developmental stage: {brain.developmental_stage:.2f}")
            print(f"     - Agent energy: {world.get_agent_body().energy:.1f}")
    
    # 7. Print final results
    print("\n7. Simulation Results:")
    print(f"   - Total simulation steps: {num_steps}")
    print(f"   - Final firing neurons: {brain.firing_neuron_count()}")
    print(f"   - Average firing rate: {brain.average_firing_rate():.2f} Hz")
    print(f"   - Final developmental stage: {brain.developmental_stage:.2f}")
    print(f"   - Final curiosity level: {agent.get_curiosity_level():.3f}")
    print(f"   - Final novelty level: {agent.get_novelty_level():.3f}")
    print(f"   - E/I balance: {brain.excitation_inhibition_ratio:.2f}")
    
    # 8. Save brain state (optional)
    print("\n8. Saving brain state...")
    try:
        brain.save("agent_simulation_final.bin")
        print("   - Brain state saved successfully")
    except Exception as e:
        print(f"   - Save failed: {e}")
    
    print("\n=== Complete Agent Example Finished ===\n")

if __name__ == "__main__":
    run_complete_agent_example()
```

## Understanding NLM Concepts

### Core Components

#### Brain

The brain is the central processing unit containing:

- **Neurons**: Individual processing units (LIF dynamics)
- **Synapses**: Connections between neurons with adjustable weights
- **Populations**: Groups of neurons with shared properties
- **Regions**: Functional brain regions with specialized roles
- **Memory Systems**: Working memory, episodic memory, etc.

#### World

The world represents the environment containing:

- **Sensory Input**: Vision, touch, proprioception, internal state
- **Motor Output**: Actions and movements
- **Objects**: Resources, hazards, walls, markers
- **Agent**: The brain-environment interface

#### Agent

The agent connects brain and world:

- **Sensory Transduction**: Converts world percepts to neural input
- **Motor Decoding**: Converts neural activity to motor commands
- **Neuromodulation**: Applies reward signals to plasticity
- **Development**: Updates developmental stage
- **Attention**: Selective processing through competition

### Key Terminology

#### Neural Terms

| Term | Description |
|------|-------------|
| **LIF Neuron** | Leaky Integrate-and-Fire neuron model |
| **Spike** | Action potential signal |
| **Synaptic Delay** | Time for signal to propagate |
| **STDP** | Spike-Timing-Dependent Plasticity |
| **Hebbian Learning** | Co-activation strengthening |
| **Structural Plasticity** | Formation/removal of connections |

#### Cognitive Terms

| Term | Description |
|------|-------------|
| **Working Memory** | Temporary storage with persistent activity |
| **Episodic Memory** | Experience encoding in neural patterns |
| **Prediction Error** | Difference between expected and actual outcomes |
| **Curiosity** | Novelty-driven exploration motivation |
| **Novelty Detection** | Identification of new experiences |
| **Attention** | Competitive selection of processing focus |

#### Development Terms

| Term | Description |
|------|-------------|
| **Developmental Stage** | Age-dependent processing mode |
| **Critical Period** | Window of high plasticity |
| **Maturation** | Development of adult capabilities |
| **Synaptogenesis** | Formation of new synapses |
| **Pruning** | Removal of weak connections |

## Basic Tutorials

### Tutorial 1: Simple Brain Simulation

#### Goal
Create and run a minimal brain simulation with basic neural dynamics.

#### Steps

1. **Create Brain**

```python
import pynlm

# Create default brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
```

2. **Initialize Brain**

```python
brain.initialize()
```

3. **Run Simulation**

```python
# Run 100 simulation steps
for step in range(100):
    brain.step(step)
```

4. **Monitor Results**

```python
# Print statistics at intervals
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: {brain.firing_neuron_count()} neurons firing")
```

#### Complete Code

```python
import pynlm

# Create and initialize brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run simulation
print("Simple Brain Simulation")
print("=======================")

for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        firing = brain.firing_neuron_count()
        rate = brain.average_firing_rate()
        print(f"Step {step:3d}: {firing:4d} firing, {rate:6.2f} Hz avg")

print("\nFinal Statistics:")
print(f"Total neurons: {brain.total_neuron_count}")
print(f"Total synapses: {brain.total_synapse_count}")
print(f"Average firing rate: {brain.average_firing_rate():.2f} Hz")
print(f"E/I ratio: {brain.excitation_inhibition_ratio:.2f}")
```

### Tutorial 2: Agent in World Environment

#### Goal
Create an agent that perceives the world, makes decisions, and takes actions.

#### Steps

1. **Setup Brain and World**

```python
import pynlm

# Create brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)
brain = pynlm.createBrain(config)
brain.initialize()

# Create world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
```

2. **Create Agent Interface**

```python
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)
```

3. **Run Perception-Action Loop**

```python
for step in range(50):
    # 1. Update world
    world.update(0.1)
    
    # 2. Get sensory percept
    percept = world.get_sensory_percept()
    
    # 3. Process sensory input
    agent.process_sensory_input(percept)
    
    # 4. Brain computation
    brain.step(step)
    
    # 5. Get motor command
    action = agent.decode_motor_command()
    
    # 6. Apply action
    world.apply_motor_command(action, world.simulation_time)
```

#### Complete Code

```python
import pynlm

# 1. Create brain
config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# 2. Create and configure world
world = pynlm.createSimpleWorld()
world.configure(width=20, height=20, vision_width=8, vision_height=8)
world.reset()
world.set_agent_start(10.0, 10.0)

# 3. Create agent interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# 4. Run simulation loop
print("Agent in World Simulation")
print("==========================")

for step in range(50):
    # Update world
    world.update(0.1)
    
    # Get sensory input
    percept = world.get_sensory_percept()
    
    # Process sensory input
    agent.process_sensory_input(percept)
    
    # Brain computation
    brain.step(step)
    
    # Get motor command
    action = agent.decode_motor_command()
    
    # Apply action
    world.apply_motor_command(action, world.simulation_time)
    
    # Print status every 10 steps
    if step % 10 == 0:
        print(f"Step {step}:")
        print(f"  Vision: {percept.vision.shape[0]}x{percept.vision.shape[1]} pixels")
        print(f"  Firing neurons: {brain.firing_neuron_count()}")
        print(f"  Action: {action.action_type}")
        print(f"  Agent energy: {world.get_agent_body().energy:.1f}")
        print()

print("Simulation complete!")
```

### Tutorial 3: Learning with Neuromodulation

#### Goal
Demonstrate how reward and curiosity drive learning in the brain.

#### Steps

1. **Enable Learning Systems**

```python
agent.enable_reward_modulation(True)
agent.enable_curiosity(True)
```

2. **Run Simulation with Learning**

```python
for step in range(200):
    world.update(0.1)
    
    # Get reward from world
    percept = world.get_sensory_percept()
    reward = percept.get_internal()[0] if percept.get_internal() else 0.0
    
    # Process and learn
    agent.process_sensory_input(percept)
    brain.step(step)
    agent.apply_reward_modulation(reward, 0.0)
    
    # Get action
    action = agent.decode_motor_command()
    world.apply_motor_command(action, world.simulation_time)
    
    # Print learning metrics
    if step % 20 == 0:
        print(f"Step {step}: Reward={reward:.2f}, "
              f"Curiosity={agent.get_curiosity_level():.3f}, "
              f"Novelty={agent.get_novelty_level():.3f}")
```

#### Complete Code

```python
import pynlm

def run_learning_simulation():
    """Run simulation demonstrating learning with neuromodulation."""
    
    print("Learning with Neuromodulation")
    print("==============================")
    
    # Setup
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=15, height=15, vision_width=6, vision_height=6)
    world.reset()
    world.set_agent_start(7.5, 7.5)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    
    # Run simulation
    num_steps = 200
    learning_data = []
    
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get sensory input and reward
        percept = world.get_sensory_percept()
        reward = (percept.get_internal()[0] 
                  if percept.get_internal() 
                  else 0.0)
        
        # Process and learn
        agent.process_sensory_input(percept)
        brain.step(step)
        agent.apply_reward_modulation(reward, 0.0)
        
        # Get action
        action = agent.decode_motor_command()
        world.apply_motor_command(action, world.simulation_time)
        
        # Record learning metrics
        learning_data.append({
            'step': step,
            'reward': reward,
            'curiosity': agent.get_curiosity_level(),
            'novelty': agent.get_novelty_level(),
            'firing': brain.firing_neuron_count(),
            'action': action.action_type
        })
        
        # Print progress
        if step % 20 == 0:
            print(f"Step {step:3d}: Reward={reward:6.2f}, "
                  f"Curiosity={agent.get_curiosity_level():5.3f}, "
                  f"Novelty={agent.get_novelty_level():5.3f}, "
                  f"Firing={brain.firing_neuron_count():4d}")
    
    # Print summary
    print("\nLearning Summary:")
    print("=================")
    
    # Calculate statistics
    avg_curiosity = sum(d['curiosity'] for d in learning_data) / len(learning_data)
    avg_novelty = sum(d['novelty'] for d in learning_data) / len(learning_data)
    total_reward = sum(d['reward'] for d in learning_data)
    avg_firing = sum(d['firing'] for d in learning_data) / len(learning_data)
    
    print(f"Total steps: {num_steps}")
    print(f"Average curiosity: {avg_curiosity:.3f}")
    print(f"Average novelty: {avg_novelty:.3f}")
    print(f"Total reward: {total_reward:.2f}")
    print(f"Average firing neurons: {avg_firing:.1f}")
    
    # Show action distribution
    actions = {}
    for d in learning_data:
        action = d['action']
        actions[action] = actions.get(action, 0) + 1
    
    print("\nAction distribution:")
    for action, count in sorted(actions.items()):
        print(f"  {action}: {count} times")
    
    print("\nLearning simulation complete!")

if __name__ == "__main__":
    run_learning_simulation()
```

## Working with NLM

### Monitoring Brain State

#### Basic Statistics

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor statistics
print("Brain Statistics:")
print(f"Total neurons: {brain.total_neuron_count}")
print(f"Total synapses: {brain.total_synapse_count}")
print(f"Current firing: {brain.firing_neuron_count}")
print(f"Average firing rate: {brain.average_firing_rate:.2f} Hz")
print(f"E/I balance: {brain.excitation_inhibition_ratio:.2f}")
print(f"Developmental stage: {brain.developmental_stage:.2f}")
```

#### Memory System Status

```python
# Check memory systems (Phase 2)
if brain.get_working_memory():
    wm = brain.get_working_memory()
    print(f"Working memory size: {wm.size()}")
else:
    print("Working memory not available")

if brain.get_episodic_memory():
    em = brain.get_episodic_memory()
    print(f"Episodic memory capacity: {em.capacity}")
```

### Configuration Management

#### Create Custom Configuration

```python
import pynlm

# Create custom configuration
config = pynlm.createDefaultConfig()

# Modify parameters
config.set("brain.neuron_count", 3000)
config.set("brain.synapse_density", 0.2)
config.set("plasticity.stdp.learning_rate", 0.002)
config.set("neuromod.dopamine.scale", 1.5)

# Verify changes
print(f"Neuron count: {config.get('brain.neuron_count')}")
print(f"STDP enabled: {config.get('plasticity.stdp.enable')}")
```

#### Save and Load Configuration

```python
import pynlm

# Save configuration
config = pynlm.createDefaultConfig()
config.save_to_file("my_config.json")

# Load configuration
new_config = pynlm.createDefaultConfig()
new_config.load_from_file("my_config.json")

# Use loaded configuration
brain = pynlm.createBrain(new_config)
brain.initialize()
```

### Advanced Topics

#### Working with Development

```python
import pynlm

config = pynlm.createDefaultConfig()
brain = pynlm.createBrain(config)
brain.initialize()

# Monitor development
print("Development Progress:")
for stage in range(5):
    print(f"Stage {stage}: {brain.developmental_stage:.2f}")
    brain.update_development(10.0)  # 10ms of development
```

#### Neuron Type Configuration

```python
import pynlm

config = pynlm.createDefaultConfig()

# Configure neuron types
config.set("brain.excitatory_ratio", 0.85)
config.set("brain.inhibitory_ratio", 0.15)
config.set("brain.sensory_ratio", 0.0)
config.set("brain.motor_ratio", 0.0)
config.set("brain.modulatory_ratio", 0.0)
config.set("brain.internal_ratio", 0.0)

# Verify configuration
print("Neuron type distribution:")
print(f"Excitatory: {config.get('brain.excitatory_ratio')*100:.1f}%")
print(f"Inhibitory: {config.get('brain.inhibitory_ratio')*100:.1f}%")
```

## Troubleshooting

### Common Issues

#### "Brain Not Responding"

**Problem:** Brain simulation seems stuck.

**Solution:**

```python
# Check if brain was initialized correctly
brain = pynlm.createBrain(config)
brain.initialize()  # Always call this!

# Add progress monitoring
for step in range(100):
    brain.step(step)
    if step % 10 == 0:
        print(f"Step {step}: Brain is alive and processing")
```

#### "Too Many Neurons"

**Problem:** System runs out of memory with large brain.

**Solution:**

```python
# Use smaller brain for testing
config.set("brain.neuron_count", 500)

# Or increase system memory
# Check system memory
import psutil
memory = psutil.virtual_memory()
print(f"Available memory: {memory.available / (1024**3):.1f} GB")
```

#### "Configuration Not Applied"

**Problem:** Configuration changes not taking effect.

**Solution:**

```python
# Always configure before creating brain
config = pynlm.createDefaultConfig()
config.set("brain.neuron_count", 1000)

# Create brain with new configuration
brain = pynlm.createBrain(config)
brain.initialize()
```

#### "Python Bindings Not Found"

**Problem:** Cannot import pynlm module.

**Solution:**

```bash
# Install Python bindings correctly
pip install -e .

# Or check Python path
python3 -c "import sys; print(sys.path)"

# Try with Python 3
python3 -c "import pynlm; print(pynlm.__version__)"
```

## Next Steps

### Learn More

1. **Read HOW_TO_USE.md** - Detailed installation and usage guide
2. **Study docs/ARCHITECTURE.md** - System architecture
3. **Explore examples/** - Complete example implementations
4. **Check docs/SCIENCE.md** - Scientific background

### Extend Your Knowledge

1. **Try different configurations** - Experiment with brain parameters
2. **Enable memory systems** - Explore Phase 2 capabilities
3. **Implement custom plasticity** - Create new learning rules
4. **Build custom worlds** - Create specialized environments

### Development

1. **Contribute to NLM** - Add features, fix bugs
2. **Create tutorials** - Help others learn NLM
3. **Write documentation** - Improve existing docs
4. **Create examples** - Share your NLM projects

## Resources

### Documentation

- **Main Documentation**: `/workspace/app/docs/index.md`
- **Installation Guide**: `/workspace/app/docs/guides/installation.md`
- **API References**: `/workspace/app/docs/api/`
- **Configuration Guide**: `/workspace/app/docs/configuration/`

### Examples

- **Basic Examples**: `/workspace/app/examples/`
- **Agent Examples**: `/workspace/app/examples/agent_simulation.md`
- **Learning Examples**: `/workspace/app/examples/learning_example.md`

### Code

- **Source Code**: `/workspace/app/src/`
- **Python Bindings**: `/workspace/app/python/`
- **Tests**: `/workspace/app/tests/`

### Community

- **GitHub Repository**: (if available)
- **Issue Tracker**: (if available)
- **Discussion Forum**: (if available)
- **Slack/Discord**: (if available)

## Quick Reference

### Essential Commands

```python
# Create brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())

# Initialize
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)

# Get statistics
print(f"Firing neurons: {brain.firing_neuron_count()}")
print(f"Average firing rate: {brain.average_firing_rate():.2f} Hz")
```

### Common Patterns

| Pattern | Code |
|---------|------|
| Create brain | `pynlm.createBrain(config)` |
| Initialize brain | `brain.initialize()` |
| Run step | `brain.step(step)` |
| Get firing neurons | `brain.firing_neuron_count()` |
| Get average rate | `brain.average_firing_rate()` |
| Save state | `brain.save("filename")` |
| Load state | `brain.load("filename")` |

### Keyboard Shortcuts

| Action | Shortcut |
|--------|----------|
| Create brain | `Ctrl+N` |
| Run simulation | `F5` |
| Debug | `F9` |
| Open documentation | `F1` |

## Index

- Quick Start: 5-12
  - First Brain Example: 7-24
  - Complete Agent Example: 25-80
- Understanding NLM: 82-139
  - Core Components: 84-96
  - Key Terminology: 98-114
- Basic Tutorials: 116-258
  - Tutorial 1: Simple Brain Simulation: 118-176
  - Tutorial 2: Agent in World: 178-226
  - Tutorial 3: Learning with Neuromodulation: 228-258
- Working with NLM: 260-320
  - Monitoring Brain State: 262-278
  - Configuration Management: 280-304
  - Advanced Topics: 306-320
- Troubleshooting: 322-340
- Next Steps: 342-367
- Resources: 369-408
- Quick Reference: 410-426

---

*NLM Getting Started Guide Version 1.0*
*Phase 6 - Final Integration*
*Last Updated: 2026-09-05*