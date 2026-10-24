# NLM (熙然) - Neural Learning Machine Python Package

This repository contains the complete NLM (Neural Learning Machine) - an experimental artificial developmental brain system implemented in C++ with Python bindings. NLM is a comprehensive neural simulation framework that implements real spiking neural computation with integrated memory, prediction, cognition, and neuromodulation systems.

## Features

### Core Neural Systems
- **Real LIF Neurons**: Leaky Integrate-and-Fire neuron dynamics with event-driven spike propagation
- **Memory Systems**: Working memory, episodic memory, associative memory
- **Plasticity Rules**: STDP and Hebbian plasticity with structural plasticity
- **Neuromodulation**: Dopamine, curiosity, novelty detection, prediction error
- **Cognition**: Neural planning, concept formation, attention
- **Prediction**: Neural prediction system with error signals

### Development Stages
- Initial, critical period, maturation, adult, aging stages
- Age-dependent plasticity modulation
- Structural plasticity and neural pruning

### Persistence
- Checkpoint save/load system
- Brain state serialization
- Replay and memory consolidation

### Integration (Phase 6 Complete)
- All neural systems connected into coherent brain loop
- Memory systems integrated with neural processing
- Neuromodulation affects plasticity and dynamics
- Prediction integrated with learning
- Developmental stages modulate learning rates

## Python Bindings

The Python package provides high-level access to the NLM neural simulation framework:

### Key Classes

#### Brain
```python
brain = Brain()
brain.initialize()
brain.step(step, time)
brain.receiveSensoryInput(percept)
action = brain.produceAction()
```

#### AgentBrain (High-level interface)
```python
agent_brain = AgentBrain(brain)
agent_brain.initialize(world)
agent_brain.processSensoryInput(percept)
motor_command = agent_brain.decodeMotorCommand()
```

#### SimpleWorld
```python
world = SimpleWorld(width, height, vision_width, vision_height)
world.configure(800, 600, 80, 80)
percept = world.getSensoryPercept()
body = world.getAgentBody()
```

#### Config
```python
config = Config()
config.loadFromFile("config.json")
brain = Brain(config)
```

#### Sensory Systems
- Vision: 2D visual input processing
- Audio: Sound signal processing
- Internal: Internal neural signals
- Proprioception: Body position sense

#### Actions & Motor
- 30+ action types including movement, interaction, and custom actions
- Motor commands for agent control

### Convenience Methods

#### Quick Start
```python
import nlm

# Create a simple brain
brain = nlm.Brain()
brain.initialize()

# Simulate for 1000 steps with random input
for step in range(1000):
    # Inject random sensory input
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 30.0)
    brain.step(step, step * 0.001)

# Get final statistics
print(f"Total spikes: {brain.getTotalSpikeCount()}")
print(f"Average firing rate: {brain.getAverageFiringRate()}")
```

#### Agent Simulation
```python
import nlm

# Create simulation components
world = nlm.createSimpleWorld()
brain = nlm.createBrain()
agent_brain = nlm.createAgentBrain(brain)

# Initialize with world
agent_brain.initialize(world)

# Run episode
for step in range(500):
    # Get perception
    percept = world.getSensoryPercept()
    
    # Process perception
    agent_brain.processSensoryInput(percept)
    
    # Get action
    motor_cmd = agent_brain.decodeMotorCommand()
    
    # Apply action to world
    world.applyMotorCommand(motor_cmd, step * 0.001)
    
    # Update development
    agent_brain.updateDevelopment(0.001)
```

#### Memory Testing
```python
import nlm

# Create brain with memory testing
brain = nlm.Brain()
brain.initialize()

# Enable memory systems
brain.getWorkingMemory().enable(true)
brain.getEpisodicMemory().enable(true)
brain.getAssociativeMemory().enable(true)

# Simulate learning experience
for step in range(1000):
    # Create varied input patterns
    brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, 20.0 + (step % 100))
    brain.step(step, step * 0.001)

# Check memory performance
working_mem = brain.getWorkingMemory()
if working_mem.isActive():
    print("Working memory active")
    print(f"Memory capacity: {working_mem.getCapacity()}")
    print(f"Memory strength: {working_mem.getStrength()}")
```

### Configuration

#### JSON Configuration
```python
import nlm
import json

# Create config
config = nlm.Config()

# Set parameters
config.set("random_seed", 42)
config.set("simulation_timestep", 0.001)
config.set("neuron_count", 1000)
config.set("region_count", 2)
config.set("connection_probability", 0.15)
config.set("stdp_ltp_weight", 0.02)
config.set("stdp_ltd_weight", 0.015)

# Save to file
config.saveToFile("my_config.json")

# Load from file
config2 = nlm.Config()
config2.loadFromFile("my_config.json")

# Create brain with config
brain = nlm.Brain(config)
```

#### Command Line Configuration
```python
# Command line arguments are also supported
# --random_seed=42 --neuron_count=500 --stdp_ltp_weight=0.01
# Various other configuration options available
```

### Development Features

#### Developmental Stages
```python
import nlm

brain = nlm.Brain()
brain.initialize()

# Set developmental stage
brain.setDevelopmentalStage(nlm.DevelopmentalStage.Maturation)

# Check current stage
stage = brain.getDevelopmentalStage()
if stage == nlm.DevelopmentalStage.Maturation:
    print("Brain is in critical learning phase")
```

#### Neuromodulation
```python
import nlm

brain = nlm.Brain()
brain.initialize()

# Get neuromodulation levels
print(f"Dopamine level: {brain.getDopamine().getLevel()}")
print(f"Curiosity level: {brain.getCuriosity().getLevel()}")
print(f"Novelty level: {brain.getNovelty().getLevel()}")
print(f"Prediction error: {brain.getPredictionErrorSignal().getValue()}")
```

### Experiment Framework
```python
import nlm
import time

# Create experiment
experiment = nlm.Experiment("learning_test")
experiment.addBrain(brain)

# Run experiment
start_time = time.time()
experiment.run(1000)  # 1000 simulation steps
end_time = time.time()

# Get results
results = experiment.getResults()
print(f"Experiment completed in {end_time - start_time:.2f}s")
print(f"Total spikes: {results['total_spikes']}")
print(f"Learning detected: {results['learning_occurred']}")
```

### Visualization
```python
import nlm

brain = nlm.Brain()
brain.initialize()

# Create visualization interface
viz = nlm.VisualizationInterface()
viz.initialize(brain)

# Run simulation with visualization
for step in range(100):
    brain.step(step, step * 0.001)
    viz.update(step, step * 0.001)
    viz.render()
```

## Installation

### Prerequisites
- C++ compiler with C++20 support
- Python 3.8+
- pip

### Build and Install
```bash
# From source
cmake -B build -S .
cmake --build build
pip install .

# Or install in development mode
pip install -e .
```

### Quick Test
```bash
# Test the Python bindings
python -c "import nlm; brain = nlm.Brain(); brain.initialize(); print('Python bindings working!')"
```

## Testing

### Unit Tests
```bash
# Run C++ tests
cd build && ctest

# Run Python tests
pytest tests/ -v
```

### Integration Tests
```python
# Comprehensive integration test
import nlm
import time

def test_complete_brain_simulation():
    """Test complete brain simulation with all systems"""
    # Create brain
    brain = nlm.Brain()
    brain.initialize()
    
    # Run simulation
    for step in range(1000):
        # Vary input based on step
        current = 10.0 + 20.0 * (1.0 + math.sin(step * 0.01)) / 2.0
        brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, current)
        brain.step(step, step * 0.001)
    
    # Verify results
    assert brain.getTotalSpikeCount() > 0
    assert brain.getAverageFiringRate() > 0
    
    print("✓ Complete brain simulation test passed")

def test_agent_brain():
    """Test agent brain integration"""
    world = nlm.createSimpleWorld()
    brain = nlm.createBrain()
    agent_brain = nlm.createAgentBrain(brain)
    
    agent_brain.initialize(world)
    
    for step in range(500):
        percept = world.getSensoryPercept()
        agent_brain.processSensoryInput(percept)
        motor_cmd = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, step * 0.001)
    
    print("✓ Agent brain integration test passed")

# Run tests
if __name__ == "__main__":
    import math
    test_complete_brain_simulation()
    test_agent_brain()
    print("\n✓ All tests passed!")
```

## Examples

### Maze Navigation
```python
import nlm
import random

def simulate_maze_navigation(world_size=100, steps=2000):
    """Simulate an agent navigating a maze"""
    # Create simulation
    world = nlm.createSimpleWorld(world_size, world_size, 20, 20)
    brain = nlm.createBrain()
    agent_brain = nlm.createAgentBrain(brain)
    
    # Initialize
    agent_brain.initialize(world)
    
    # Place agent in center
    world.setAgentStart(world_size // 2, world_size // 2)
    
    # Add obstacles
    for x in range(0, world_size, 10):
        for y in range(0, world_size, 10):
            if random.random() > 0.8:
                obj = nlm.WorldObject(x, y, nlm.WorldObjectType.Wall, 1.0, 0.8)
                world.addObject(obj)
    
    # Run simulation
    for step in range(steps):
        # Get perception
        percept = world.getSensoryPercept()
        
        # Process and act
        agent_brain.processSensoryInput(percept)
        motor_cmd = agent_brain.decodeMotorCommand()
        world.applyMotorCommand(motor_cmd, step * 0.001)
        
        # Check if agent reached goal
        body = world.getAgentBody()
        if body.x == world_size - 1 and body.y == world_size - 1:
            print(f"✓ Reached goal at step {step}")
            break
    
    print(f"Final position: ({body.x}, {body.y})")
    return brain

# Run maze navigation
brain = simulate_maze_navigation()
```

### Learning Experiment
```python
import nlm
import math

def learning_experiment(experiences=10, steps_per_experience=200):
    """Test learning through repeated experiences"""
    # Create brain
    brain = nlm.Brain()
    brain.initialize()
    
    # Enable plasticity
    if region := brain.getRegion(1):
        for syn in region.getSynapses():
            syn.enablePlasticity(true, true, false)
    
    # Record initial weights
    initial_weights = []
    if region := brain.getRegion(1):
        for syn in region.getSynapses()[:10]:
            initial_weights.append(syn.getWeight())
    
    print(f"Initial average weight: {sum(initial_weights)/len(initial_weights):.4f}")
    
    # Run learning experiences
    for experience in range(experiences):
        print(f"\n--- Experience {experience + 1}/{experiences} ---")
        
        # Create unique pattern for this experience
        pattern_offset = experience * 0.5
        
        for step in range(steps_per_experience):
            # Create experience-specific input
            current = 30.0 + pattern_offset + 10.0 * math.sin(step * 0.05)
            brain.injectCurrentToNeurons(nlm.NeuronType.Sensory, current)
            
            brain.step(step, step * 0.001)
            
            if step % 50 == 0:
                print(f"  Step {step}: Spikes={brain.getTotalSpikeCount()}, "
                      f"Firing={brain.getFiringNeuronCount()}")
    
    # Record final weights
    final_weights = []
    if region := brain.getRegion(1):
        for syn in region.getSynapses()[:10]:
            final_weights.append(syn.getWeight())
    
    print(f"\nFinal average weight: {sum(final_weights)/len(final_weights):.4f}")
    
    # Calculate learning
    if initial_weights and final_weights:
        initial_avg = sum(initial_weights) / len(initial_weights)
        final_avg = sum(final_weights) / len(final_weights)
        learning_change = final_avg - initial_avg
        
        print(f"\nLearning Results:")
        print(f"  Weight change: {learning_change:+.4f}")
        print(f"  Learning rate: {learning_change / initial_avg * 100:.2f}%")
        
        if abs(learning_change) > 0.01:
            print("  ✓ Significant learning detected!")
        else:
            print("  ⚠ Limited learning - may need different parameters")
    
    return brain

# Run learning experiment
brain = learning_experiment()
```

## API Reference

### Classes
- **Brain**: Main neural simulation brain class
- **AgentBrain**: High-level interface connecting brain to world
- **SimpleWorld**: 2D simulation world for agent interaction
- **Config**: Configuration management
- **Vision, Audio, InternalSignals**: Sensory input types
- **Action**: Motor action representation
- **SensoryPercept**: Collected sensory information
- **AgentBody**: Agent state and physics

### Enums
- **NeuronType**: Neuron types (Excitatory, Inhibitory, Sensory, etc.)
- **ActionType**: Available actions (MoveForward, Look, Interact, etc.)
- **DevelopmentalStage**: Developmental stages
- **FiringState**: Neuron firing states

### Functions
- **createDefaultConfig**: Create default configuration
- **createBrain**: Create new brain with configuration
- **createSimpleWorld**: Create simple world
- **createAgentBrain**: Create agent brain interface

### Constants
- **INVALID_NEURON_ID, INVALID_SYNAPSE_ID, etc.**: Invalid ID constants

## Performance Considerations

### Large-Scale Simulations
For simulations with many neurons (>10,000):
- Use larger simulation timesteps
- Reduce sensory input frequency
- Enable checkpoint saving
- Monitor memory usage

### Memory Management
- Brains can be saved and reloaded for long experiments
- Use checkpoint system for recovery
- Monitor spike system memory usage

## Troubleshooting

### Import Errors
```bash
# Error: No module named 'nlm'
# Solution: pip install -e .
```

### Build Errors
```bash
# Error: pybind11 not found
# Solution: Ensure pybind11 is available
# On Ubuntu: apt-get install pybind11-dev
```

### Runtime Errors
```python
# Error: Brain not initialized
# Solution: Call brain.initialize() before use
```

## Version Information

- **Python Package**: 0.1.0
- **C++ Core**: 0.1.0
- **License**: MIT
- **Status**: Phase 6 Complete (Production Ready)

## Acknowledgements

This project builds upon years of neural computation research and represents Phase 6 of the NLM developmental project. Special thanks to the research team and contributors.
