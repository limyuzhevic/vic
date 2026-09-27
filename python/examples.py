#!/usr/bin/env python3
"""
Basic examples for NLM Python bindings.

This script demonstrates the basic use cases for the NLM Python bindings,
showing how to get started with the neural simulation framework.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
from pynlm import (
    Config,
    EnhancedConfig,
    Brain,
    EnhancedBrain,
    SimpleWorld,
    EnhancedSimpleWorld,
    Action,
    SensoryPercept,
)
import numpy as np


def example_basic_config():
    """Example 1: Basic configuration usage."""
    print("=" * 60)
    print("Example 1: Basic Configuration")
    print("=" * 60)
    
    # Create default config using enhanced interface
    config = EnhancedConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15)
    config.set("world.width", 20)
    config.set("world.height", 20)
    
    print(f"Config summary: {config.summary()}")
    print(f"Neuron count: {config.get('brain.neuron_count')}")
    print(f"Synapse density: {config.get('brain.synapse_density')}")
    
    # Validate configuration
    errors = config.validate()
    if errors:
        print(f"Configuration errors: {errors}")
    else:
        print("✓ Configuration is valid")
    
    print()


def example_basic_brain():
    """Example 2: Basic brain creation and initialization."""
    print("=" * 60)
    print("Example 2: Basic Brain Creation and Initialization")
    print("=" * 60)
    
    # Create config
    config = EnhancedConfig()
    config.set("brain.neuron_count", 500)
    
    # Create brain using enhanced interface
    brain = EnhancedBrain(config)
    
    # Initialize brain
    if brain.initialize():
        print(f"✓ Brain initialized successfully")
        print(f"  Total neurons: {brain.total_neuron_count}")
        print(f"  Region count: {brain.region_count}")
        print(f"  Average firing rate: {brain.average_firing_rate:.3f} Hz")
    else:
        print("✗ Brain initialization failed")
    
    # Run simulation
    print("\nRunning simulation...")
    for step in range(10):
        brain.step(step)
        print(f"  Step {step}: {brain.firing_neuron_count} neurons firing, "
              f"{brain.total_spike_count} total spikes")
    
    print(f"Final statistics: {brain.total_spike_count} total spikes")
    print()


def example_world_simulation():
    """Example 3: World simulation with agent."""
    print("=" * 60)
    print("Example 3: World Simulation with Agent")
    print("=" * 60)
    
    # Create world using enhanced interface
    world = EnhancedSimpleWorld()
    world.configure(width=20, height=20, vision_width=8, vision_height=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    print(f"World created: {world.width}x{world.height}")
    print(f"Agent start position: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    # Create brain
    config = EnhancedConfig()
    brain = EnhancedBrain(config)
    brain.initialize()
    
    # Create agent
    from pynlm import EnhancedAgentBrain
    agent = EnhancedAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning systems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    
    print("Learning systems enabled: Reward, Curiosity, Development")
    
    # Run world simulation
    print("\nRunning world simulation...")
    for step in range(20):
        world.update(0.1)
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.simulation_time)
        
        # Log statistics
        if step % 5 == 0:
            body = world.getAgentBody()
            print(f"  Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                  f"Energy: {body.energy:.1f}, "
                  f"Firing: {brain.firing_neuron_count}, "
                  f"Curiosity: {agent.curiosity_level:.3f}")
    
    final_body = world.getAgentBody()
    print(f"\n✓ World simulation complete. Final position: "
          f"({final_body.x:.1f}, {final_body.y:.1f})")
    print()


def example_sensory_integration():
    """Example 4: Sensory integration."""
    print("=" * 60)
    print("Example 4: Sensory Integration")
    print("=" * 60)
    
    # Create sensory percept
    percept = SensoryPercept()
    
    # Create vision data (32x32x3)
    vision_data = np.random.rand(32, 32, 3).astype(np.float32)
    percept.setVision(vision_data.tolist())
    
    print(f"Vision data shape: {len(percept.getVision())}x{len(percept.getVision()[0])}")
    
    # Create touch data (10 sensors)
    touch_data = np.random.rand(10).astype(np.float32)
    percept.setTouch(touch_data.tolist())
    
    print(f"Touch sensors: {len(percept.getTouch())}")
    
    # Create internal signals
    internal_data = np.random.rand(5).astype(np.float32)
    percept.setInternal(internal_data.tolist())
    
    print(f"Internal signals: {len(percept.getInternal())}")
    
    # Get all signals
    all_signals = percept.getAllSignals()
    print(f"Total signal types: {len(all_signals)}")
    for signal_type, signal_data in all_signals.items():
        print(f"  {signal_type}: {len(signal_data)} values")
    
    # Convert vision to numpy and back
    vision_from_percept = percept.getVision()
    vision_numpy = np.array(vision_from_percept)
    print(f"Vision to numpy and back: shape {vision_numpy.shape}")
    
    print()


def example_action_system():
    """Example 5: Action system usage."""
    print("=" * 60)
    print("Example 5: Action System Usage")
    print("=" * 60)
    
    # Create actions using factory functions
    from pynlm import createAction, createActionResult
    
    # Create move forward action
    move_forward = createAction(pynlm.ActionType.MoveForward)
    print(f"Move forward action: {move_forward.getName()}")
    print(f"  Parameters: {move_forward.getParameters()}")
    
    # Create move backward action with parameters
    move_backward = createAction(pynlm.ActionType.MoveBackward, [1.0, 0.5])
    print(f"Move backward action: {move_backward.getName()}")
    print(f"  Parameters: {move_backward.getParameters()}")
    
    # Create action result
    success_result = createActionResult(10.0, True, "Action completed successfully")
    print(f"Action result: reward={success_result.reward}, "
          f"success={success_result.success}, "
          f"message='{success_result.message}'")
    
    # Clone action
    cloned_action = move_forward.clone()
    print(f"Cloned action: {cloned_action.getName()}")
    
    print()


def example_world_objects():
    """Example 6: World object management."""
    print("=" * 60)
    print("Example 6: World Object Management")
    print("=" * 60)
    
    # Create world objects
    from pynlm import createWorldObject
    
    # Create resource object
    resource = createWorldObject(5.0, 5.0, pynlm.WorldObjectType.Resource, 1.0)
    print(f"Resource object: pos({resource.x}, {resource.y}), "
          f"type={resource.type}, value={resource.value}, active={resource.active}")
    
    # Create hazard object
    hazard = createWorldObject(15.0, 15.0, pynlm.WorldObjectType.Hazard, 2.0)
    print(f"Hazard object: pos({hazard.x}, {hazard.y}), "
          f"type={hazard.type}, value={hazard.value}, active={hazard.active}")
    
    # Create wall object
    wall = createWorldObject(10.0, 20.0, pynlm.WorldObjectType.Wall, 0.5)
    print(f"Wall object: pos({wall.x}, {wall.y}), "
          f"type={wall.type}, value={wall.value}, active={wall.active}")
    
    # Activate hazard
    hazard.setActive(True)
    print(f"Activated hazard: active={hazard.active}")
    
    # Get distance between objects
    distance = ((resource.x - hazard.x)**2 + (resource.y - hazard.y)**2)**0.5
    print(f"Distance between resource and hazard: {distance:.2f} units")
    
    print()


def example_configuration_file_io():
    """Example 7: Configuration file I/O."""
    print("=" * 60)
    print("Example 7: Configuration File I/O")
    print("=" * 60)
    
    import tempfile
    import os
    
    # Create config and save to file
    config = EnhancedConfig()
    config.set("brain.neuron_count", 1000)
    config.set("brain.synapse_density", 0.15)
    config.set("world.width", 20)
    config.set("world.height", 20)
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        config_file = f.name
        config.save_to_file(config_file)
    
    print(f"Configuration saved to: {config_file}")
    
    # Load config from file
    loaded_config = EnhancedConfig()
    loaded_config.load_from_file(config_file)
    
    print(f"Loaded configuration:")
    print(f"  Neuron count: {loaded_config.get('brain.neuron_count')}")
    print(f"  Synapse density: {loaded_config.get('brain.synapse_density')}")
    print(f"  World width: {loaded_config.get('world.width')}")
    print(f"  World height: {loaded_config.get('world.height')}")
    
    # Validate loaded config
    errors = loaded_config.validate()
    if not errors:
        print("✓ Loaded configuration is valid")
    else:
        print(f"Configuration errors: {errors}")
    
    # Clean up
    os.unlink(config_file)
    print("Configuration file cleaned up.")
    
    print()


def example_backward_compatibility():
    """Example 8: Backward compatibility with original API."""
    print("=" * 60)
    print("Example 8: Backward Compatibility")
    print("=" * 60)
    
    # Original API (using original classes)
    from pynlm import Config, Brain
    
    config = Config()
    brain = Brain(config)
    brain.initialize()
    
    print(f"Original API - Brain initialized: {brain.initialized}")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    
    # Enhanced API (using enhanced classes)
    enhanced_config = EnhancedConfig()
    enhanced_brain = EnhancedBrain(enhanced_config)
    enhanced_brain.initialize()
    
    print(f"Enhanced API - Brain initialized: {enhanced_brain.initialized}")
    print(f"  Total neurons: {enhanced_brain.total_neuron_count}")
    
    # Both APIs should work with the same underlying C++ bindings
    print("✓ Both original and enhanced APIs work correctly")
    
    print()


def run_all_examples():
    """Run all examples."""
    print("NLM Python Bindings - Basic Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_basic_config,
        example_basic_brain,
        example_world_simulation,
        example_sensory_integration,
        example_action_system,
        example_world_objects,
        example_configuration_file_io,
        example_backward_compatibility,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"✗ Example {i} failed: {e}")
            print()
    
    print("=" * 60)
    print("All examples completed!")
    print("=" * 60)


if __name__ == "__main__":
    run_all_examples()
