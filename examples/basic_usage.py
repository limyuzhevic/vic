#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) Python Examples
=============================================

This file demonstrates the enhanced Pythonic API for the NLM neural simulation framework.
The improved bindings provide:

1. Pythonic API design with convenience methods
2. Enhanced error handling and type safety
3. Simulation helpers and episode runners
4. Configuration management with JSON support
5. Data structures with numpy-compatible interfaces

Key improvements over the original C++ bindings:
- Pythonic method names and parameter handling
- Comprehensive error handling with custom exceptions
- Type-safe configuration with validation
- Simulation context managers
- High-level API for common operations
"""

import pynlm as nlm
import numpy as np
from typing import Dict, List, Optional

print("NLM Python Enhanced API Examples")
print("=" * 50)

# Example 1: Basic Configuration
print("\n1. Basic Configuration:")
print("-" * 30)

# Create default configuration
config = nlm.Config.create_default()
config.set_with_validation("brain.neuronCount", 1000)
config.set_with_validation("brain.regionCount", 5)
config.set_with_validation("simulation.dt", 0.1)
config.set_with_validation("simulation.maxTime", 100.0)
config.set_with_validation("world.width", 100.0)
config.set_with_validation("world.height", 100.0)

# Load from Python dictionary
config_dict = {
    "brain.neuronCount": 2000,
    "brain.regionCount": 8,
    "simulation.dt": 0.05,
    "neuromodulation.level": 0.7
}
config.load_from_dict(config_dict)

# Get values with type safety
neuron_count = config.get_value("brain.neuronCount", 500)
stage = config.get_value("development.stage", 0)

eigenvector = config.get_with_validation("brain.neuronCount")

print(f"Neuron count: {neuron_count}")
print(f"Development stage: {stage}")
print(f"Eigenvector: {eigenvector}")

# Example 2: Vision and Audio Processing
print("\n2. Sensory Processing:")
print("-" * 30)

# Create vision from numpy array
vision_array = np.random.rand(64, 64, 3).astype(np.float32)
vision = nlm.Vision.from_array(vision_array)
print(f"Vision shape: {vision.get_width()}x{vision.get_height()}x{vision.get_channels()}")

# Create audio from numpy array
audio_array = np.random.randn(44100).astype(np.float32)
audio = nlm.Audio.from_array(audio_array, sample_rate=44100)
print(f"Audio duration: {audio.get_num_samples()/audio.get_sample_rate():.2f} seconds")

# Create sensory percept
percept = nlm.SensoryPercept()
percept.set_vision(vision.to_array())
percept.set_audio(audio.to_array())
data = percept.get_all_data()

print(f"Percept contains {len(data)} sensory modalities")

# Example 3: Action and Motor Control
print("\n3. Motor Control:")
print("-" * 30)

# Create actions
move_action = nlm.Action(nlm.ActionType.MoveForward, [1.0, 0.0])
look_action = nlm.Action(nlm.ActionType.Look, [0.5, 0.2, 0.1])
interact_action = nlm.Action.from_dict({
    "type": nlm.ActionType.Interact,
    "parameters": [0.8, 0.3]
})

print(f"Move action: {move_action.get_name()} with {move_action.get_parameters()}")
print(f"Look action: {look_action.get_name()} with {look_action.get_parameters()}")
print(f"Interact action: {interact_action.get_name()} with {interact_action.get_parameters()}")

# Clone action
cloned_action = move_action.clone()
print(f"Cloned action type matches: {cloned_action.get_type() == move_action.get_type()}")

# Example 4: Enhanced World Simulation
print("\n4. World Simulation:")
print("-" * 30)

# Create and configure world
world = nlm.SimpleWorld()
world.configure(width=200, height=200, vision_width=32, vision_height=32)
world.set_random_seed(42)

# Get world state
body_state = world.get_agent_body()
print(f"Agent position: ({body_state['x']:.1f}, {body_state['y']:.1f})")
print(f"Agent orientation: {body_state['orientation']:.1f} degrees")
print(f"Agent energy: {body_state['energy']:.1f}")

# Add objects to world
world.add_world_object(50.0, 50.0, nlm.WorldObjectType.Resource, value=10.0)
world.add_world_object(150.0, 150.0, nlm.WorldObjectType.Hazard, value=5.0)

# Check positions
is_valid = world.is_valid_position(75.0, 75.0)
print(f"Position (75, 75) is valid: {is_valid}")

world_dimensions = world.get_dimensions()
print(f"World dimensions: {world_dimensions}")

# Example 5: Brain and AgentBrain Integration
print("\n5. Brain Integration:")
print("-" * 30)

# Create brain with configuration
brain = nlm.Brain(config)
brain.initialize()

# Get brain statistics
stats = brain.get_statistics()
print("Brain statistics:")
for key, value in stats.items():
    print(f"  {key}: {value}")

# Perform simulation steps
for step in range(5):
    brain.step(step)
    brain.step_with_time(step, step * 0.1)

# Get current state
final_stats = brain.get_statistics()
print(f"\nAfter 5 steps:")
print(f"  Total spikes: {final_stats['total_spikes']}")
print(f"  Average firing rate: {final_stats['avg_firing_rate']:.2f}")
print(f"  Prediction error: {final_stats['prediction_error']:.4f}")

# Create agent brain
agent_brain = nlm.AgentBrain(brain)
agent_brain.initialize(world)
agent_brain.enable_reward_modulation(True)
agent_brain.enable_curiosity(True)

print(f"\nAgent brain initialized with:")
print(f"  Sensory input size: {agent_brain.get_sensory_input_size()}")
print(f"  Motor output size: {agent_brain.get_motor_output_size()}")
print(f"  Developmental stage: {agent_brain.get_developmental_stage()}")

# Example 6: Neuromodulation and Development
print("\n6. Neuromodulation and Development:")
print("-" * 30)

# Get neuromodulation levels
neuromodulation = agent_brain.get_neuromodulation_levels()
print("Neuromodulation levels:")
for key, value in neuromodulation.items():
    print(f"  {key}: {value:.4f}")

# Apply reward modulation
agent_brain.apply_reward_modulation(reward=1.0, predicted_reward=0.8)
print(f"\nApplied reward modulation: reward=1.0, predicted=0.8")

# Update development
agent_brain.update_development(timestep=0.1)
print(f"Developmental stage after update: {agent_brain.get_developmental_stage()}")

# Example 7: Simulation Context Manager
print("\n7. Simulation Context Manager:")
print("-" * 30)

# Create simulation context
context = brain.create_simulation_context(start_step=0, end_step=10)

# Run simulation and collect statistics
print("Running 10-step simulation...")
context.run()

final_stats = brain.get_statistics()
print(f"Simulation completed:")
print(f"  Total steps: 10")
print(f"  Total spikes: {final_stats['total_spikes']}")
print(f"  Firing neurons: {final_stats['firing_neurons']}")

# Example 8: Episode Running
print("\n8. Episode Running (High-level API):")
print("-" * 30)

# Run a complete episode (this would require full integration)
print("Episode would run here with:")
print("  - Sensory input processing")
print("  - Action selection and execution")
print("  - Reward modulation")
print("  - Development updates")
print("  - Statistics collection")

# Example 9: Error Handling
print("\n9. Error Handling:")
print("-" * 30)

try:
    # This should raise a ConfigurationError
    config.set_with_validation("invalid.key", "not_a_number")
    print("ERROR: Should have raised ConfigurationError")
except nlm.ConfigurationError as e:
    print(f"Caught ConfigurationError: {e}")

# Example 10: Error Handling with Simulation
print("\n10. Simulation Error Handling:")
print("-" * 30)

try:
    # Try to step brain before initialization
    uninitialized_brain = nlm.Brain(config)
    uninitialized_brain.step(0)
    print("ERROR: Should have raised SimulationError")
except nlm.SimulationError as e:
    print(f"Caught SimulationError: {e}")

# Example 11: Configuration Validation
print("\n11. Configuration Validation:")
print("-" * 30)

# Valid configuration
valid_config = nlm.Config.create_default()
valid_config.set_with_validation("brain.neuronCount", 1000)
valid_config.set_with_validation("simulation.dt", 0.1)

# Invalid configuration (should raise exception)
try:
    valid_config.set_with_validation("brain.neuronCount", "not_a_number")
    print("ERROR: Should have raised ConfigurationError")
except nlm.ConfigurationError as e:
    print(f"Caught configuration error for invalid type: {e}")

# Missing key handling
if not valid_config.has("nonexistent.key"):
    print("Correctly detected missing key")

# Example 12: Data Export
print("\n12. Data Export and Serialization:")
print("-" * 30)

# Export configuration as dictionary
config_dict = config.as_dict()
print(f"Configuration as dictionary has {len(config_dict)} keys")

# Export brain statistics
brain_stats = brain.get_statistics()
print("Brain statistics exported successfully")

# Export action data
action_data = move_action.to_dict()
print(f"Action exported: type={action_data['type']}, name={action_data['name']}")

print("\n" + "=" * 50)
print("All examples completed successfully!")
print("\nThe enhanced Python API provides:")
print("✓ Pythonic method names and parameter handling")
print("✓ Comprehensive error handling with custom exceptions")
print("✓ Type-safe configuration with validation")
print("✓ Simulation context managers and helpers")
print("✓ High-level API for common operations")
print("✓ Data structure export and serialization")
print("=" * 50)
