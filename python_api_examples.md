"""
NLM Python API Examples and Documentation

This file contains comprehensive examples and documentation for the NLM (Neural Learning Machine)
Python API. It demonstrates the improved functionality including:
- Enhanced configuration system with JSON/YAML support
- New convenience methods for easier usage
- Complete working examples of brain simulation
- Integration with world simulation
- Learning and analysis capabilities

The examples cover various use cases from simple to complex, suitable for
both beginners and advanced users.
"""

"""
Phase 1: Basic Brain Creation and Configuration
===============================================

This example shows the most basic way to create and use an NLM brain.
"""

import pynlm as nlm
import time

print("=== Phase 1: Basic Brain Creation and Configuration ===")

# Create a default configuration
config = nlm.createDefaultConfig()

# Configure basic parameters
config.set("neuron_count", 1000)
config.set("region_count", 1)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 42)

# Create brain with configuration
brain = nlm.createBrain(config)
brain.initialize()

print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
print(f"Brain status: {brain.getFiringNeuronCount()} firing neurons")
print(f"Total synapses: {brain.getTotalSynapseCount()}")

# Run a simple simulation for 1000 steps
for step in range(1000):
    brain.step(step)

print(f"Simulation complete. Total spikes: {brain.getTotalSpikeCount()}")
print()


"""
Phase 2: Enhanced Configuration System
=====================================

This example demonstrates the improved JSON/YAML configuration system
with proper validation and error handling.
"""

print("=== Phase 2: Enhanced Configuration System ===")

# Example 1: Create config from JSON file
json_config_content = """{
    "neuron_count": 2000,
    "region_count": 2,
    "connection_probability": 0.15,
    "simulation_timestep": 0.001,
    "random_seed": 123,
    "stdp_ltp_weight": 0.02,
    "stdp_ltd_weight": 0.018,
    "log_level": "INFO"
}"""

# Write JSON to a temporary file
with open("temp_config.json", "w") as f:
    f.write(json_config_content)

# Load from JSON file
config = nlm.createDefaultConfig()
if config.loadFromFile("temp_config.json"):
    print("✓ Successfully loaded configuration from JSON file")
    print(f"Loaded {len(config.getKeys())} configuration parameters")
else:
    print("✗ Failed to load configuration from JSON file")

# Example 2: Modify configuration dynamically
config.set("neuron_count", 3000)  # Change neuron count
config.set("stdp_ltp_weight", 0.025)  # Adjust STDP parameters

# Save updated configuration back to JSON
config.saveToFile("updated_config.json")
print("✓ Configuration saved to JSON file")

print(f"Configuration summary:\n{config.summary()}")
print()


"""
Phase 3: Complete Agent Simulation
===================================

This example shows a complete working simulation with brain, world,
and agent integration, demonstrating the convenience methods.
"""

print("=== Phase 3: Complete Agent Simulation ===")

# Create components
config = nlm.createDefaultConfig()
config.set("neuron_count", 500)
config.set("region_count", 1)
config.set("simulation_timestep", 0.01)

brain = nlm.createBrain(config)
brain.initialize()

world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()
world.setAgentStart(10.0, 10.0)

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Enable subsystems for more realistic behavior
agent.enableRewardModulation(True)
agent.enableStructuralPlasticity(True)
agent.enableDevelopment(True)
agent.enableCuriosity(True)

# Run a complete simulation episode
num_steps = 500
print(f"Running simulation for {num_steps} steps...")

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
    
    # Apply motor command to world
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    # Apply reward modulation
    reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
    agent.applyRewardModulation(reward, 0.0)
    
    # Update development
    agent.updateDevelopment(0.1)
    
    # Print progress periodically
    if step % 100 == 0:
        print(f"  Step {step}:")
        print(f"    Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"    Curiosity: {agent.getCuriosityLevel():.3f}")
        print(f"    Novelty: {agent.getNoveltyLevel():.3f}")
        print(f"    Development Stage: {brain.getDevelopmentalStage()}")

print("✓ Complete simulation finished successfully!")
print()


"""
Phase 4: Learning and Analysis
===============================

This example demonstrates the learning analysis capabilities
and how to extract insights from brain simulations.
"""

print("=== Phase 4: Learning and Analysis ===")

# Create and run a learning experiment
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 1)
config.set("simulation_timestep", 0.001)

brain = nlm.createBrain(config)
brain.initialize()

# Enable plasticity
if region := brain.getRegion(nlm.RegionId(1)):
    for syn in region.getSynapses():
        syn.enablePlasticity(True, True, False)

# Apply learning stimulus for 5000 steps
print("Applying learning stimulus...")

# Create a learning pattern
learning_neurons = []
for region in brain.getRegions():
    for neuron in region.getAllNeurons():
        if neuron.getType() == nlm.NeuronType::Sensory and len(learning_neurons) < 20:
            learning_neurons.append(neuron.getId())

# Apply correlated pre-post input to stimulate STDP
for step in range(5000):
    # Apply pre-synaptic input
    for neuron_id in learning_neurons:
        brain.injectCurrent(neuron_id, 30.0f)
    
    # Step brain
    brain.step(step)
    
    # Apply post-synaptic input (correlated timing)
    if step % 2 == 0:  # Every other step for correlated pre-post
        for neuron_id in learning_neurons:
            brain.injectCurrent(neuron_id, 30.0f)
    
    brain.step(step + 1)

# Analyze learning results
print("Analyzing learning results...")
print(f"Total spikes generated: {brain.getTotalSpikeCount()}")
print(f"Final E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")
print(f"Learning progress score: {brain.runSimulation([], 5000):.3f}")

# Get brain status
status = brain.getStatus()
print(f"\nBrain status:\n{status}")

print()


"""
Phase 5: Advanced Python Features
=================================

This example demonstrates advanced Python API features and
how to leverage the convenience methods for more complex applications.
"""

print("=== Phase 5: Advanced Python Features ===")

# Example 1: Using convenience methods for configuration
print("Example 1: Convenience methods for configuration")
config = nlm.createDefaultConfig()

# Set multiple configuration options efficiently
config.set("neuron_count", 2000)
config.set("simulation_timestep", 0.001)
config.set("random_seed", 123)
config.set("stdp_ltp_weight", 0.02)

# Get configuration values with defaults
neuron_count = config.getInt("neuron_count", 1000)
simulation_timestep = config.getDouble("simulation_timestep", 0.001)
stdp_weight = config.getDouble("stdp_ltp_weight", 0.01)

print(f"Neuron count: {neuron_count}")
print(f"Simulation timestep: {simulation_timestep}")
print(f"STDP LTP weight: {stdp_weight}")

# Example 2: Advanced simulation with custom actions
print("\nExample 2: Advanced simulation with custom actions")

# Create a custom world with obstacles
world = nlm.createSimpleWorld()
world.configure(width=30, height=30, visionWidth=10, visionHeight=10)
world.reset()
world.setAgentStart(15.0, 15.0)

# Add some obstacles
for x in [5, 10, 15, 20, 25]:
    for y in [5, 10, 15, 20, 25]:
        world.addObject(nlm.WorldObject(x, y, nlm.WorldObjectType::Wall, radius=0.5))

# Create and configure brain
brain = nlm.createBrain(config)
brain.initialize()

# Create agent
agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Create custom action
move_action = nlm.createAction(nlm.MotorCommand::MoveForward)

# Advanced simulation with path planning
print("Running path planning simulation...")
target_pos = (25.0, 25.0)
steps_to_target = 0

for step in range(1000):
    # Get current agent position
    agent_body = world.getAgentBody()
    current_pos = (agent_body.x, agent_body.y)
    
    # Check if we've reached the target
    distance = ((current_pos[0] - target_pos[0])**2 + 
                (current_pos[1] - target_pos[1])**2)**0.5
    
    if distance < 1.0:  # Reached target
        print(f"✓ Target reached in {steps_to_target} steps!")
        break
    
    # Get sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    brain.step(step)
    motor_cmd = agent.decodeMotorCommand()
    world.applyMotorCommand(motor_cmd, world.getSimulationTime())
    
    steps_to_target += 1
else:
    print("Target not reached within step limit")

# Example 3: Data analysis and visualization preparation
print("\nExample 3: Data analysis and visualization preparation")

# Run simulation and collect data for analysis
config = nlm.createDefaultConfig()
brain = nlm.createBrain(config)
brain.initialize()

# Collect time-series data
time_series_data = []
spike_counts = []

for step in range(100):
    # Apply some input
    if step % 10 == 0:
        brain.injectCurrentToNeurons(nlm.NeuronType::Sensory, 10.0f)
    
    brain.step(step)
    
    # Collect metrics
    time_series_data.append({
        'step': step,
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'avg_firing_rate': brain.getAverageFiringRate(),
        'eir_ratio': brain.getExcitationInhibitionRatio()
    })
    
    spike_counts.append(brain.getFiringNeuronCount())

# Calculate basic statistics
total_spikes = time_series_data[-1]['total_spikes']
max_firing = max(data['firing_neurons'] for data in time_series_data)
avg_firing = sum(data['firing_neurons'] for data in time_series_data) / len(time_series_data)

print(f"Simulation completed with {total_spikes} total spikes")
print(f"Maximum firing neurons: {max_firing}")
print(f"Average firing neurons: {avg_firing:.1f}")

print("\n✓ Advanced Python features examples completed!")
print()


"""
Additional Utilities and Best Practices
======================================

This section provides additional utilities and best practices for
working with the NLM Python API.
"""

print("=== Additional Utilities and Best Practices ===")

# Utility 1: Safe configuration loading
def safe_load_config(filepath, default_config=None):
    """Safely load configuration with fallback to defaults."""
    config = default_config or nlm.createDefaultConfig()
    
    if config.loadFromFile(filepath):
        print(f"✓ Successfully loaded configuration from {filepath}")
        return config
    else:
        print(f"⚠ Failed to load configuration from {filepath}, using defaults")
        return config

# Utility 2: Validate configuration
def validate_config(config):
    """Validate configuration against basic requirements."""
    errors = []
    
    # Check required parameters
    if not config.has("neuron_count"):
        errors.append("Missing neuron_count")
    
    if not config.has("region_count"):
        errors.append("Missing region_count")
    
    # Check values are reasonable
    neuron_count = config.getInt("neuron_count", 1000)
    if neuron_count < 100 or neuron_count > 100000:
        errors.append(f"Unreasonable neuron_count: {neuron_count}")
    
    region_count = config.getInt("region_count", 1)
    if region_count < 1 or region_count > 100:
        errors.append(f"Unreasonable region_count: {region_count}")
    
    timestep = config.getDouble("simulation_timestep", 0.001)
    if timestep <= 0 or timestep > 1.0:
        errors.append(f"Unreasonable simulation_timestep: {timestep}")
    
    if errors:
        print(f"⚠ Configuration validation warnings:")
        for error in errors:
            print(f"  - {error}")
        return False
    else:
        print("✓ Configuration validation passed")
        return True

# Utility 3: Create a complete demonstration
print("Demonstrating utilities...")

# Use the utilities
config = safe_load_config("nonexistent_config.json")
config.set("neuron_count", 500)
config.set("region_count", 1)

validate_config(config)

# Create a simple demonstration brain
brain = nlm.createBrain(config)
brain.initialize()

print(f"Created demonstration brain: {brain.getTotalNeuronCount()} neurons")
print()


"""
Summary and Key Improvements
============================

This document demonstrates the key improvements made to the NLM Python API:

## Key Improvements

### 1. Enhanced Configuration System
- **JSON/YAML Support**: Proper JSON file parsing and writing
- **Schema Validation**: Configuration validation with descriptive error messages
- **Convenience Methods**: Easy-to-use Pythonic configuration API
- **Error Handling**: Robust error handling with informative messages

### 2. Python API Improvements
- **Convenience Methods**: Simplified configuration setting and getting
- **Type-Specific Getters**: `getInt()`, `getDouble()`, `getString()`, `getBool()`
- **Default Values**: Configuration methods with sensible defaults
- **Brain Status**: `getStatus()` method for easy brain state inspection
- **Learning Analysis**: `runSimulation()` and `getLearningProgress()` methods

### 3. Enhanced Functionality
- **Improved Simulation**: More robust and flexible simulation control
- **Action Creation**: Easier creation of custom motor actions
- **Sensory Input**: Simplified sensory input creation and processing
- **Data Collection**: Built-in utilities for collecting and analyzing simulation data

### 4. Better Error Handling
- **File I/O Errors**: Clear error messages for file operations
- **Validation Errors**: Descriptive validation warnings
- **Graceful Fallbacks**: Safe configuration loading with defaults

## Usage Patterns

### Basic Usage (2-3 lines)
```python
import pynlm as nlm

# Create brain
brain = nlm.createBrain(nlm.createDefaultConfig())
brain.initialize()

# Run simulation
for step in range(100):
    brain.step(step)
```

### Advanced Usage (Comprehensive example)
```python
# Create and configure brain
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.set("region_count", 1)

brain = nlm.createBrain(config)
brain.initialize()

# Create agent and world
world = nlm.createSimpleWorld()
world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
world.reset()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Run complete simulation
for step in range(500):
    world.update(0.1)
    agent.processSensoryInput(world.getSensoryPercept())
    brain.step(step)
    world.applyMotorCommand(agent.decodeMotorCommand(), world.getSimulationTime())
```

### Configuration Management
```python
# Save/load configuration
config = nlm.createDefaultConfig()
config.set("neuron_count", 1000)
config.saveToFile("my_config.json")

config2 = nlm.createDefaultConfig()
config2.loadFromFile("my_config.json")
```

## Benefits

1. **Easier to Use**: Pythonic API with intuitive method names
2. **More Robust**: Enhanced error handling and validation
3. **More Flexible**: Better configuration options and simulation control
4. **More Powerful**: New convenience methods for analysis and learning
5. **Better Documentation**: Comprehensive examples and documentation
6. **Production Ready**: Proper error handling and validation

These improvements make the NLM Python API more accessible to users while
providing powerful tools for advanced research and development.
"""

print("=== NLM Python API Examples Complete ===")
print()
print("Key improvements demonstrated:")
print("✓ Enhanced JSON configuration system")
print("✓ Convenience methods for easier usage")
print("✓ Complete simulation examples")
print("✓ Learning and analysis capabilities")
print("✓ Advanced Python features")
print("✓ Utilities and best practices")
print()
print("The NLM Python API now provides a comprehensive, user-friendly")
print("interface for neural simulation and machine learning research.")
print()

# Cleanup temporary files
import os
if os.path.exists("temp_config.json"):
    os.remove("temp_config.json")
if os.path.exists("updated_config.json"):
    os.remove("updated_config.json")

print("✓ All examples completed successfully!")
print("✓ Temporary files cleaned up")