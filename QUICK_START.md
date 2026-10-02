# NLM Quick Start Guide

## Overview

This guide provides quick instructions for getting started with the NLM (Neural Learning Machine) framework after the latest improvements.

## Installation and Setup

### Prerequisites

- C++ compiler with C++20 support
- CMake 3.16 or higher
- Python 3.8 or higher
- pybind11 development headers

### Build Instructions

```bash
# Create build directory
mkdir build
cd build

# Configure with CMake
cmake .. -DCMAKE_BUILD_TYPE=Release

# Build the project
make -j4
```

### Build Python Bindings

```bash
cd /path/to/nlm/project
python -m pip install -e .
```

## Basic Usage

### Command Line Interface

The enhanced NLM framework provides a comprehensive command-line interface:

```bash
# Show help information
./nlm --help

# Show version information
./nlm --version

# Run Phase 2 demo (default)
./nlm

# Run with custom configuration
./nlm --neuron 1000 --steps 5000 --config myconfig.cfg

# Run learning experiments
./nlm --mode learning --neuron 500 --steps 2000

# Run benchmarks
./nlm --mode benchmark --timestep 0.001
```

### Python Interface

```python
import pynlm as nlm

# Get system information
info = nlm.getSystemInfo()
print(f"NLM Version: {info['version']}")
print(f"Phase: {info['phase']}")

# Create a brain with default configuration
brain = nlm.createBrain()
brain.initialize()

# Create a world
world = nlm.createSimpleWorld(100, 100, 10, 10)

# Create an agent brain
agent_brain = nlm.createAgentBrain(brain, world)

# Get memory systems
working_memory = brain.getWorkingMemory()
episodic_memory = brain.getEpisodicMemory()
assoc_memory = brain.getAssociativeMemory()

# Get cognitive systems
attention = brain.getAttention()
concept_formation = brain.getConceptFormation()
planner = brain.getPlanner()
```

## Memory Systems

### Working Memory

```python
import pynlm as nlm

# Initialize working memory
brain = nlm.createBrain()
working_memory = brain.getWorkingMemory()
working_memory.initialize(brain.get())

# Store patterns
pattern1 = [0.1, 0.5, 0.9, 0.3, 0.7]
working_memory.store(pattern1, strength=0.8)

# Retrieve patterns
retrieved = working_memory.retrieve()
print(f"Retrieved pattern: {retrieved}")

# Get pattern size
pattern_size = working_memory.getPatternSize()
print(f"Pattern size: {pattern_size}")
```

### Episodic Memory

```python
import pynlm as nlm

brain = nlm.createBrain()
episodic_memory = brain.getEpisodicMemory()

# Store an episode (example)
episode = {
    "timestamp": 100,
    "reward": 0.8,
    "action": nlm.ActionType.MoveForward,
    "position": (10.0, 20.0, 0.0),
    "energy": 0.9,
    "novelty": 0.7
}

# Store episode
episodic_memory.storeEpisode(episode)

# Get recent episodes
recent = episodic_memory.getRecentEpisodes(5)
for i, ep in enumerate(recent):
    print(f"Episode {i}: timestamp={ep['timestamp']}, reward={ep['reward']}")
```

### Associative Memory

```python
import pynlm as nlm

brain = nlm.createBrain()
assoc_memory = brain.getAssociativeMemory()

# Create associations
pattern1 = [0.1, 0.5, 0.9, 0.3, 0.7]
pattern2 = [0.2, 0.6, 0.8, 0.4, 0.6]
pattern3 = [0.3, 0.7, 0.7, 0.5, 0.5]

assoc_memory.createAssociation(pattern1, pattern2, strength=0.8)
assoc_memory.createAssociation(pattern2, pattern3, strength=0.6)

# Retrieve associations
associated = assoc_memory.retrieveAssociation(pattern2, threshold=0.5)
print(f"Pattern associated with pattern2: {associated}")

# Check association strength
strength = assoc_memory.getAssociationStrength(pattern1, pattern2)
print(f"Association strength: {strength}")
```

## Cognitive Systems

### Attentional Selection

```python
import pynlm as nlm

brain = nlm.createBrain()
attention = brain.getAttention()
attention.initialize(brain.get())

# Process sensory input
sensory_signals = [0.1, 0.5, 0.9, 0.3, 0.7, 0.2, 0.8, 0.4, 0.6, 0.1]
available_neurons = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]

selected_focus = attention.processSensoryInput(sensory_signals, available_neurons)
print(f"Selected focus neurons: {selected_focus}")

# Update based on reward
attention.updateBasedOnReward(0.8, selected_focus)

# Get attentional gains
for neuron in selected_focus:
    gain = attention.getAttentionGain(neuron)
    print(f"Neuron {neuron} attention gain: {gain}")
```

### Concept Formation

```python
import pynlm as nlm

brain = nlm.createBrain()
concept_formation = brain.getConceptFormation()
concept_formation.initialize(brain.get())

# Present experiences
pattern = [0.1, 0.5, 0.9, 0.3, 0.7]
features = [0.8, 0.6, 0.4]
reward = 0.9

concept_id = concept_formation.presentExperience(pattern, features, reward, 100)
print(f"Concept ID: {concept_id}")

# Get concepts
concepts = concept_formation.getConcepts()
print(f"Total concepts: {len(concepts)}")

# Find matching concept
matching_id = concept_formation.getMatchingConcept(pattern, similarityThreshold=0.7)
print(f"Matching concept ID: {matching_id}")
```

## Neural Dynamics

### Basic Neural Simulation

```python
import pynlm as nlm
import time

# Create brain
brain = nlm.createBrain()
brain.initialize()

# Create sensory input
vision = nlm.createVision(10, 10, 3)

# Create action
action = nlm.createAction(nlm.ActionType.MoveForward, [1.0, 0.5])

# Run simulation
print("Starting neural simulation...")
start_time = time.time()

for step in range(100):
    # Process sensory input
    brain.receiveSensoryInput(vision)
    
    # Produce action
    produced_action = brain.produceAction()
    
    # Step simulation
    brain.step(step, step * 0.001)
    
    # Update development
    brain.updateDevelopment(0.001)

end_time = time.time()
print(f"Simulation completed in {end_time - start_time:.2f} seconds")

# Get final statistics
brain.logStatus()
```

## Configuration

### Command Line Configuration

```bash
# Set number of neurons
./nlm --neuron 1000

# Set simulation steps
./nlm --steps 5000

# Set timestep
./nlm --timestep 0.001

# Set brain regions
./nlm --regions 2

# Load configuration file
./nlm --config myconfig.cfg

# Enable debug mode
./nlm --debug

# Quiet mode (reduced output)
./nlm --quiet
```

### Configuration File Format

Create a file named `config.cfg`:

```ini
# NLM Configuration File
random_seed = 12345
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
max_steps = 1000
connection_probability = 0.15

# STDP Parameters
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0

# Structural Plasticity Parameters
synaptogenesis_rate = 0.0001
pruning_rate = 0.00001
```

### Python Configuration

```python
import pynlm as nlm

# Create default configuration
config = nlm.createDefaultConfig()

# Modify configuration
config.setInt("neuron_count", 1000)
config.setFloat("simulation_timestep", 0.001)
config.setString("random_seed", "12345")

# Create brain with custom configuration
brain = nlm.createBrain(config)
```

## Error Handling

### Common Errors and Solutions

#### Invalid Pattern Size

```python
import pynlm as nlm

# Ensure pattern has correct size
pattern = [0.1, 0.5, 0.9]
expected_size = 5

if len(pattern) != expected_size:
    print(f"Error: Pattern size {len(pattern)} != expected {expected_size}")
    # Pad or truncate pattern
    pattern = pattern + [0.0] * (expected_size - len(pattern))
    if len(pattern) > expected_size:
        pattern = pattern[:expected_size]
```

#### Configuration File Errors

```python
# Handle configuration file errors
try:
    brain = nlm.createBrain()
except Exception as e:
    print(f"Error loading configuration: {e}")
    # Load default configuration
    brain = nlm.createDefaultBrain()
```

#### Memory Errors

```python
# Monitor memory usage
import psutil
import os

process = psutil.Process(os.getpid())
memory_info = process.memory_info()
print(f"Memory usage: {memory_info.rss / 1024 / 1024:.2f} MB")
```

## Performance Monitoring

### Basic Performance Monitoring

```python
import time
import pynlm as nlm

# Time neural network operations
start_time = time.time()

# Create brain and world
brain = nlm.createBrain()
world = nlm.createSimpleWorld(50, 50, 5, 5)

end_time = time.time()

print(f"Setup time: {end_time - start_time:.3f} seconds")
```

### System Information

```python
import pynlm as nlm

# Get system information
info = nlm.getSystemInfo()
print("NLM System Information:")
for key, value in info.items():
    if key != "features":
        print(f"  {key}: {value}")

print("\nKey Features:")
for feature in info["features"]:
    print(f"  - {feature}")
```

## Examples

### Complete Example: Learning Agent

```python
import pynlm as nlm
import time

def run_learning_agent():
    """Run a complete learning agent example"""
    print("=== NLM Learning Agent Example ===")
    
    # Create brain and world
    brain = nlm.createBrain()
    world = nlm.createSimpleWorld(100, 100, 10, 10)
    
    # Create agent brain
    agent_brain = nlm.createAgentBrain(brain, world)
    agent_brain.initialize(world)
    
    # Set up learning
    enable_reward_modulation = True
    enable_structural_plasticity = True
    enable_development = True
    enable_curiosity = True
    
    agent_brain.enableRewardModulation(enable_reward_modulation)
    agent_brain.enableStructuralPlasticity(enable_structural_plasticity)
    agent_brain.enableDevelopment(enable_development)
    agent_brain.enableCuriosity(enable_curiosity)
    
    print(f"Reward modulation enabled: {agent_brain.isRewardModulationEnabled()}")
    print(f"Structural plasticity enabled: {agent_brain.isStructuralPlasticityEnabled()}")
    print(f"Development enabled: {agent_brain.isDevelopmentEnabled()}")
    print(f"Curiosity enabled: {agent_brain.isCuriosityEnabled()}")
    
    # Run simulation
    print("\nStarting learning simulation...")
    start_time = time.time()
    
    for episode in range(5):
        print(f"\n--- Episode {episode + 1} ---")
        
        # Reset for new episode
        agent_brain.reset()
        
        # Run episode
        for step in range(50):
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent_brain.processSensoryInput(percept)
            
            # Decode motor command
            motor_command = agent_brain.decodeMotorCommand()
            
            # Apply motor command
            result = world.applyMotorCommand(motor_command, time.time())
            
            # Apply reward modulation
            if result.success:
                agent_brain.applyRewardModulation(result.reward, result.reward * 0.9)
            
            # Update development
            agent_brain.updateDevelopment(0.001)
            
            # Get current status
            print(f"  Step {step + 1}: Reward = {result.reward:.2f}, Success = {result.success}")
    
    end_time = time.time()
    print(f"\n=== Learning Complete ===")
    print(f"Total time: {end_time - start_time:.2f} seconds")
    
    # Get final status
    agent_brain.getBrain().logStatus()

if __name__ == "__main__":
    run_learning_agent()
```

### Configuration Example

```python
import pynlm as nlm

def create_custom_brain():
    """Create a brain with custom configuration"""
    
    # Create configuration
    config = nlm.createDefaultConfig()
    
    # Modify configuration parameters
    config.setInt("neuron_count", 2000)
    config.setFloat("simulation_timestep", 0.0005)
    config.setInt("random_seed", 12345)
    config.setFloat("connection_probability", 0.2)
    
    # Create brain with custom configuration
    brain = nlm.createBrain(config)
    
    # Initialize
    brain.initialize()
    
    print("Custom brain created:")
    print(f"  Neuron count: {brain.getTotalNeuronCount()}")
    print(f"  Simulation timestep: {brain.getConfig().getFloat('simulation_timestep'):.6f}")
    print(f"  Connection probability: {brain.getConfig().getFloat('connection_probability'):.3f}")
    
    return brain
```

## Quick Commands

```bash
# Show help
./nlm --help

# Show version
./nlm --version

# Run basic demo
./nlm

# Run with custom parameters
./nlm --neuron 1000 --steps 2000 --timestep 0.001

# Run learning experiments
./nlm --mode learning

# Run benchmarks
./nlm --mode benchmark

# Run memory tests
./nlm --mode memory

# Quiet mode
./nlm --quiet

# Debug mode
./nlm --debug
```

## Troubleshooting

### Common Issues

1. **Build Errors**: Ensure you have all dependencies installed
2. **Runtime Errors**: Check for invalid input parameters
3. **Memory Issues**: Monitor memory usage with larger networks
4. **Configuration Errors**: Validate configuration file format

### Getting Help

- Check the documentation in `DOCUMENTATION.md`
- Review the examples in this file
- Use `--help` for command-line options
- Check Python API documentation in the bindings

## Next Steps

1. **Explore Examples**: Run the provided examples to understand the framework
2. **Customize Configuration**: Modify configuration parameters for your use case
3. **Extend Python Interface**: Add custom functions to the Python bindings
4. **Create Custom Experiments**: Design new learning experiments
5. **Performance Optimization**: Profile and optimize your simulations

## Support

For questions and support:

- Check the documentation
- Review the examples
- Use the command-line help
- Consult the source code for implementation details

---

This guide covers the essential functionality of the enhanced NLM framework. Use these examples as starting points for your own neural computation experiments.
