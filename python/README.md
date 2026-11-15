# NLM Python Bindings Documentation

This document describes the Python bindings for the NLM (Neural Learning Machine) project.

## Overview

The NLM Python bindings provide a user-friendly interface to the NLM C++ neural simulation framework. The bindings are built using PyBind11 and allow Python users to:

- Create and simulate artificial brains
- Interact with virtual worlds
- Control agent behavior
- Monitor and analyze neural activity
- Save and load brain states
- Configure simulation parameters

## Installation

### Prerequisites

- Python 3.8 or higher
- pip package manager
- Build tools (cmake, make, g++)

### Installing Python Bindings

There are two ways to install the Python bindings:

#### Option 1: Install from pip (recommended for most users)

```bash
# The package should be available on PyPI
pip install pynlm
```

#### Option 2: Build from source

```bash
# Clone the NLM repository
# or navigate to the NLM project root directory

# Build with Python bindings
mkdir build
cd build
cmake .. -DNLM_BUILD_PYTHON_BINDINGS=ON
make

# Install Python package
cd python
pip install .
```

### Alternative: Direct Python Package Installation

If the package is published to PyPI, you can also install directly:

```bash
pip install pynlm
```

## Getting Started

### Basic Example

```python
#!/usr/bin/env python3

import pynlm

# Create default configuration
config = pynlm.createDefaultConfig()

# Create a brain with the configuration
brain = pynlm.createBrain(config)

# Initialize the brain
brain.initialize()

# Print initial status
brain.logStatus()

# Create a simple world
world = pynlm.createSimpleWorld()
world.configure(20, 20, 8, 8)

# Create an agent brain interface
agent = pynlm.createAgentBrain(brain)
agent.initialize(world)

# Enable learning features
agent.enableRewardModulation(True)
agent.enableCuriosity(True)

# Run a simple simulation
for step in range(100):
    # Update world
    world.update(0.1)
    
    # Get sensory percept
    percept = world.getSensoryPercept()
    
    # Process sensory input through agent
    agent.processSensoryInput(percept)
    
    # Brain processes step
    brain.step(step)
    
    # Get action from agent
    action = agent.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(action, step * 0.1)
    
    # Print progress
    if step % 20 == 0:
        print(f"Step {step}: Spikes={brain.getTotalSpikeCount()}, "
              f"Firing={brain.getFiringNeuronCount()}, "
              f"Curiosity={agent.getCuriosityLevel():.3f}")

print("Simulation completed!")
```

### Advanced Example

```python
#!/usr/bin/env python3

import pynlm

def create_experiment():
    """Create a complete simulation experiment."""
    
    # Create configuration with custom parameters
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 1000)
    config.set("region_count", 2)
    config.set("simulation_timestep", 0.001)
    config.set("stdp_ltp_weight", 0.02)
    config.set("stdp_ltd_weight", 0.015)
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    # Configure and initialize
    brain.initialize()
    brain.logStatus()
    
    return config, brain

def run_simulation(config, brain, steps=500):
    """Run a complete simulation."""
    
    # Create world and agent
    world = pynlm.createSimpleWorld()
    world.configure(30, 30, 10, 10)
    world.setAgentStart(15, 15)
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning features
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(steps):
        # World update
        world.update(0.1)
        
        # Sensory processing
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Neural processing
        brain.step(step)
        
        # Action selection and execution
        action = agent.decodeMotorCommand()
        action_result = world.applyMotorCommand(action, step * 0.1)
        
        # Reward-based learning
        agent.applyRewardModulation(action_result.reward, 0.0)
        
        # Development updates
        agent.updateDevelopment(0.1)
        
        # Progress monitoring
        if step % 100 == 0:
            print(f"Step {step}:")
            print(f"  Spikes: {brain.getTotalSpikeCount()}")
            print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
            print(f"  Development stage: {agent.getDevelopmentalStage()}")
            print(f"  Neuromodulation: {agent.getNeuromodulationLevel():.3f}")
            print(f"  Curiosity: {agent.getCuriosityLevel():.3f}")
            print(f"  Novelty: {agent.getNoveltyLevel():.3f}")
            print(f"  Action result: {action_result.message}")
            print()
    
    return brain, agent, world

def save_experiment(brain, filename):
    """Save brain state to file."""
    if brain.save(filename):
        print(f"✓ Brain saved to {filename}")
        return True
    else:
        print(f"✗ Failed to save brain to {filename}")
        return False

def load_experiment(filename):
    """Load brain state from file."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    if brain.load(filename):
        print(f"✓ Brain loaded from {filename}")
        return brain
    else:
        print(f"✗ Failed to load brain from {filename}")
        return None

# Main execution
if __name__ == "__main__":
    print("NLM Python Advanced Example")
    print("=========================")
    
    # Create and run experiment
    config, brain = create_experiment()
    brain, agent, world = run_simulation(config, brain, 200)
    
    # Save results
    save_experiment(brain, "nlm_experiment_state.bin")
    
    # Load and verify
    loaded_brain = load_experiment("nlm_experiment_state.bin")
    
    print("\n✓ Advanced example completed successfully!")