#!/usr/bin/env python3
"""
Advanced examples for NLM Python bindings.

This script demonstrates advanced use cases for the NLM Python bindings,
showing how to leverage the full power of the neural simulation framework.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
from pynlm import (
    EnhancedConfig,
    EnhancedBrain,
    EnhancedSimpleWorld,
    EnhancedAgentBrain,
    SensoryPercept,
    Vision,
    Audio,
    Action,
    WorldObject,
)
import numpy as np
import time
import json


def example_complex_configuration():
    """Example 1: Complex configuration with validation."""
    print("=" * 60)
    print("Example 1: Complex Configuration with Validation")
    print("=" * 60)
    
    # Create config with many parameters
    config = EnhancedConfig()
    
    # Brain configuration
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.2)
    config.set("brain.plasticity.stdp.learning_rate", 0.01)
    config.set("brain.plasticity.hebbian.learning_rate", 0.005)
    
    # Sensory configuration
    config.set("sensory.vision.width", 64)
    config.set("sensory.vision.height", 64)
    config.set("sensory.vision.channels", 3)
    config.set("sensory.audio.sample_rate", 44100)
    config.set("sensory.audio.duration", 2.0)
    
    # World configuration
    config.set("world.width", 50)
    config.set("world.height", 50)
    config.set("world.max_energy", 100.0)
    config.set("world.energy_decay_rate", 0.01)
    
    # Development configuration
    config.set("development.stage", "Maturation")
    config.set("development.plasticity_reduction", 0.5)
    
    # Neuromodulation configuration
    config.set("neuromodulation.reward.scale", 1.0)
    config.set("neuromodulation.curiosity.threshold", 0.1)
    config.set("neuromodulation.novelty.decay", 0.95)
    
    print(f"Complex config summary: {config.summary()}")
    
    # Validate configuration
    errors = config.validate()
    if errors:
        print(f"Configuration errors: {errors}")
    else:
        print("✓ Configuration is valid")
    
    # Show some configuration keys
    keys = config.get_keys()
    print(f"\nConfiguration contains {len(keys)} keys including:")
    for key in sorted(keys)[:10]:  # Show first 10 keys
        print(f"  {key}")
    if len(keys) > 10:
        print(f"  ... and {len(keys) - 10} more keys")
    
    print()


def example_learning_systems():
    """Example 2: Advanced learning systems control."""
    print("=" * 60)
    print("Example 2: Advanced Learning Systems Control")
    print("=" * 60)
    
    # Create world and brain
    world = EnhancedSimpleWorld()
    world.configure(width=30, height=30, vision_width=16, vision_height=16)
    world.reset()
    world.setAgentStart(15.0, 15.0)
    
    config = EnhancedConfig()
    config.set("brain.neuron_count", 2000)
    
    brain = EnhancedBrain(config)
    brain.initialize()
    
    agent = EnhancedAgentBrain(brain)
    agent.initialize(world)
    
    # Advanced learning system configuration
    print("Configuring learning systems...")
    
    # Reward modulation with custom settings
    agent.enable_reward_modulation(True)
    
    # Apply reward modulation with actual reward
    agent.applyRewardModulation(reward=10.0, predicted_reward=8.0)
    print(f"✓ Reward modulation applied: reward={agent.reward_level:.2f}")
    
    # Curiosity system with different thresholds
    agent.enable_curiosity(True)
    agent.applyRewardModulation(reward=1.0, predicted_reward=0.5)  # Trigger curiosity
    print(f"✓ Curiosity system triggered: curiosity={agent.curiosity_level:.3f}")
    
    # Development system control
    agent.enable_development(True)
    agent.updateDevelopment(0.1)  # Small timestep for development update
    print(f"✓ Development updated: stage={agent.developmental_stage}")
    
    # Structural plasticity
    agent.enable_structural_plasticity(True)
    print("✓ Structural plasticity enabled")
    
    # Check system status
    print(f"\nLearning system status:")
    print(f"  Reward modulation: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"  Structural plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"  Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"  Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    print(f"  Neuromodulation level: {agent.neuromodulation_level:.3f}")
    print(f"  Novelty level: {agent.novelty_level:.3f}")
    print(f"  Prediction error: {agent.prediction_error:.3f}")
    
    print()


def example_sensory_processing():
    """Example 3: Advanced sensory processing."""
    print("=" * 60)
    print("Example 3: Advanced Sensory Processing")
    print("=" * 60)
    
    # Create vision with specific properties
    vision = Vision(64, 64, 3)
    
    # Create synthetic image (gradient)
    for x in range(64):
        for y in range(64):
            for c in range(3):
                intensity = (x + y + c * 32) / (64 + 32 + 3 * 32)
                vision.setData(vision.to_numpy().flatten().tolist())
    
    # Get processed vision
    vision_data = vision.to_numpy()
    print(f"Vision data shape: {vision_data.shape}")
    print(f"Vision data range: [{vision_data.min():.3f}, {vision_data.max():.3f}]")
    
    # Create audio with specific properties
    audio = Audio(44100, 4410)  # 0.1 seconds of audio
    
    # Create sine wave
    t = np.linspace(0, 0.1, 4410, False)
    audio_data = 0.5 * np.sin(2 * np.pi * 440 * t)  # 440 Hz tone
    audio.setData(audio_data.tolist())
    
    print(f"Audio duration: {audio.duration:.3f} seconds")
    print(f"Audio frequency: {440} Hz (sine wave)")
    print(f"Audio sample rate: {audio.sample_rate} Hz")
    
    # Create internal signals
    internal = EnhancedAgentBrain._InternalSignals()
    for i in range(10):
        internal.addSignal(0.1 * i)  # Increasing signal values
    
    signals = internal.getSignals()
    print(f"Internal signals: {len(signals)} values, range [{min(signals):.2f}, {max(signals):.2f}]")
    
    # Create sensory percept and populate with all sensor types
    percept = SensoryPercept()
    percept.setVision(vision_data.tolist())
    percept.setAudio(audio_data.tolist())
    
    # Add touch data (simulated proximity sensors)
    touch_data = np.random.rand(8).astype(np.float32)
    percept.setTouch(touch_data.tolist())
    
    # Add proprioception (simulated joint angles)
    proprioception_data = np.array([0.1, 0.2, 0.3, 0.2, 0.1], dtype=np.float32)
    percept.setProprioception(proprioception_data.tolist())
    
    # Add internal signals
    internal_data = np.array([0.5, -0.3, 0.8, 0.0, -0.2], dtype=np.float32)
    percept.setInternal(internal_data.tolist())
    
    print(f"\nComplete sensory percept:")
    signals = percept.getAllSignals()
    for signal_type, signal_data in signals.items():
        print(f"  {signal_type}: {len(signal_data)} values, "
              f"range [{min(signal_data):.3f}, {max(signal_data):.3f}]")
    
    print()


def example_neural_network_analysis():
    """Example 4: Neural network analysis and monitoring."""
    print("=" * 60)
    print("Example 4: Neural Network Analysis and Monitoring")
    print("=" * 60)
    
    # Create brain with many neurons for analysis
    config = EnhancedConfig()
    config.set("brain.neuron_count", 10000)
    
    brain = EnhancedBrain(config)
    brain.initialize()
    
    # Run simulation and collect statistics
    print("Running neural simulation for analysis...")
    
    spike_counts = []
    firing_rates = []
    e_i_ratios = []
    region_counts = []
    
    for step in range(50):
        brain.step(step)
        
        spike_counts.append(brain.total_spike_count)
        firing_rates.append(brain.average_firing_rate)
        e_i_ratios.append(brain.excitation_inhibition_ratio)
        region_counts.append(brain.region_count)
        
        if step % 10 == 0:
            print(f"  Step {step}: spikes={brain.total_spike_count}, "
                  f"firing_rate={brain.average_firing_rate:.2f}, "
                  f"E/I_ratio={brain.excitation_inhibition_ratio:.3f}")
    
    # Analyze statistics
    print(f"\nNeural network statistics:")
    print(f"  Total spikes over 50 steps: {sum(spike_counts)}")
    print(f"  Average spikes per step: {np.mean(spike_counts):.1f}")
    print(f"  Average firing rate: {np.mean(firing_rates):.3f} Hz")
    print(f"  Firing rate std: {np.std(firing_rates):.3f} Hz")
    print(f"  E/I ratio range: [{min(e_i_ratios):.3f}, {max(e_i_ratios):.3f}]")
    print(f"  Region count: {region_counts[0]} (constant)")
    
    # Create brain snapshot
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        brain_file = f.name
        brain.save(brain_file)
    
    print(f"Brain state saved to: {brain_file}")
    
    # Load brain snapshot
    loaded_brain = EnhancedBrain(config)
    loaded_brain.load(brain_file)
    
    print(f"✓ Brain state loaded successfully")
    print(f"  Loaded neurons: {loaded_brain.total_neuron_count}")
    print(f"  Loaded regions: {loaded_brain.region_count}")
    
    # Clean up
    os.unlink(brain_file)
    
    print()


def example_world_interaction():
    """Example 5: Complex world interaction with learning."""
    print("=" * 60)
    print("Example 5: Complex World Interaction with Learning")
    print("=" * 60)
    
    # Create rich world with objects
    world = EnhancedSimpleWorld()
    world.configure(width=40, height=40, vision_width=20, vision_height=20)
    world.reset()
    world.setAgentStart(20.0, 20.0)
    
    # Add various objects to the world
    from pynlm import createWorldObject
    
    # Resources
    for x, y in [(5, 5), (10, 15), (25, 10)]:
        resource = createWorldObject(x, y, pynlm.WorldObjectType.Resource, 1.0)
        world.addObject(resource)
    
    # Hazards
    for x, y in [(30, 30), (35, 5)]:
        hazard = createWorldObject(x, y, pynlm.WorldObjectType.Hazard, 2.0)
        world.addObject(hazard)
    
    # Walls
    for x in range(40):
        wall = createWorldObject(x, 0, pynlm.WorldObjectType.Wall)
        world.addObject(wall)
        wall = createWorldObject(x, 39, pynlm.WorldObjectType.Wall)
        world.addObject(wall)
    
    for y in range(40):
        wall = createWorldObject(0, y, pynlm.WorldObjectType.Wall)
        world.addObject(wall)
        wall = createWorldObject(39, y, pynlm.WorldObjectType.Wall)
        world.addObject(wall)
    
    print(f"World setup complete:")
    print(f"  World size: {world.width}x{world.height}")
    print(f"  Agent start: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    print(f"  Objects added: Resources (3), Hazards (2), Walls (158)")
    
    # Create brain and agent
    config = EnhancedConfig()
    config.set("brain.neuron_count", 3000)
    
    brain = EnhancedBrain(config)
    brain.initialize()
    
    agent = EnhancedAgentBrain(brain)
    agent.initialize(world)
    
    # Enable all learning systems
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    agent.enable_structural_plasticity(True)
    
    # Run complex world interaction
    print("\nRunning complex world interaction...")
    
    for step in range(30):
        world.update(0.1)
        
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        action = agent.processSensoryInput(percept)
        
        # Apply action
        result = world.applyMotorCommand(action, world.simulation_time)
        
        # Process result (reward/error)
        reward = result.reward if result.success else -1.0
        predicted_reward = agent.prediction_error
        
        # Apply reward modulation
        agent.applyRewardModulation(reward, predicted_reward)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        # Print progress
        if step % 5 == 0:
            body = world.getAgentBody()
            print(f"  Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                  f"Energy: {body.energy:.1f}, "
                  f"Success: {result.success}, Reward: {reward:.1f}, "
                  f"Curiosity: {agent.curiosity_level:.3f}")
    
    # Check final statistics
    final_body = world.getAgentBody()
    print(f"\nFinal world state:")
    print(f"  Agent position: ({final_body.x:.1f}, {final_body.y:.1f})")
    print(f"  Agent energy: {final_body.energy:.1f}")
    print(f"  Agent health: {final_body.health:.1f}")
    print(f"  Agent age: {final_body.age:.1f}")
    print(f"  Learning systems status:")
    print(f"    Reward: {'Enabled' if agent.isRewardModulationEnabled() else 'Disabled'}")
    print(f"    Curiosity: {'Enabled' if agent.isCuriosityEnabled() else 'Disabled'}")
    print(f"    Development: {'Enabled' if agent.isDevelopmentEnabled() else 'Disabled'}")
    print(f"    Structural Plasticity: {'Enabled' if agent.isStructuralPlasticityEnabled() else 'Disabled'}")
    
    print()


def example_performance_benchmarking():
    """Example 6: Performance benchmarking."""
    print("=" * 60)
    print("Example 6: Performance Benchmarking")
    print("=" * 60)
    
    import time
    
    # Test different configurations
    configurations = [
        {"neurons": 1000, "name": "Small"},
        {"neurons": 5000, "name": "Medium"},
        {"neurons": 10000, "name": "Large"},
    ]
    
    print("Benchmarking brain performance with different configurations...")
    print()
    
    for config_info in configurations:
        print(f"{config_info['name']} configuration (neurons={config_info['neurons']}):")
        
        # Create config
        config = EnhancedConfig()
        config.set("brain.neuron_count", config_info['neurons'])
        
        # Time brain creation
        start_time = time.time()
        brain = EnhancedBrain(config)
        creation_time = time.time() - start_time
        
        # Time brain initialization
        start_time = time.time()
        brain.initialize()
        init_time = time.time() - start_time
        
        # Time simulation steps
        start_time = time.time()
        for step in range(20):
            brain.step(step)
        simulation_time = time.time() - start_time
        
        # Calculate performance metrics
        total_time = creation_time + init_time + simulation_time
        spikes_per_second = brain.total_spike_count / simulation_time if simulation_time > 0 else 0
        
        print(f"  Creation time: {creation_time:.3f}s")
        print(f"  Initialization time: {init_time:.3f}s")
        print(f"  Simulation time (20 steps): {simulation_time:.3f}s")
        print(f"  Total time: {total_time:.3f}s")
        print(f"  Spikes per second: {spikes_per_second:.0f}")
        print(f"  Final spike count: {brain.total_spike_count}")
        print()


def example_error_handling():
    """Example 7: Comprehensive error handling."""
    print("=" * 60)
    print("Example 7: Comprehensive Error Handling")
    print("=" * 60)
    
    # Test various error scenarios
    
    print("Testing error handling scenarios...")
    
    # 1. Configuration validation errors
    print("\n1. Configuration validation:")
    config = EnhancedConfig()
    config.set("brain.neuron_count", -100)  # Invalid negative value
    
    errors = config.validate()
    print(f"   Configuration validation errors: {errors}")
    
    # 2. Brain initialization error
    print("\n2. Brain initialization:")
    config = EnhancedConfig()
    config.set("brain.neuron_count", 0)  # Invalid zero value
    
    brain = EnhancedBrain(config)
    if not brain.initialize():
        print("   ✓ Brain initialization failed as expected (invalid config)")
    
    # 3. Action error
    print("\n3. Action handling:")
    try:
        action = Action(pynlm.ActionType.MoveForward)
        print(f"   ✓ Action created successfully: {action.getName()}")
    except Exception as e:
        print(f"   ✗ Action creation failed: {e}")
    
    # 4. World position validation
    print("\n4. World position validation:")
    world = EnhancedSimpleWorld()
    world.configure(width=10, height=10, vision_width=5, vision_height=5)
    world.reset()
    
    # Try to move outside world bounds
    out_of_bounds = world.isValidPosition(15.0, 15.0)
    print(f"   Position (15, 15) valid: {out_of_bounds} (should be False)")
    
    # Valid position
    in_bounds = world.isValidPosition(5.0, 5.0)
    print(f"   Position (5, 5) valid: {in_bounds} (should be True)")
    
    # 5. Sensory error handling
    print("\n5. Sensory data validation:")
    percept = SensoryPercept()
    
    # Try to set mismatched vision dimensions
    mismatched_vision = [[0.1, 0.2], [0.3, 0.4]]  # 2x2, but percept expects 5x5
    try:
        percept.setVision(mismatched_vision)
        print("   ✓ Vision set (may cause issues in actual use)")
    except Exception as e:
        print(f"   ✗ Vision set failed: {e}")
    
    print("\n✓ Error handling tests completed")
    print()


def example_json_configuration():
    """Example 8: JSON configuration management."""
    print("=" * 60)
    print("Example 8: JSON Configuration Management")
    print("=" * 60)
    
    # Create complex configuration
    config = EnhancedConfig()
    
    # Add nested configuration
    config.set("brain.neuron_count", 5000)
    config.set("brain.synapse_density", 0.15)
    config.set("brain.plasticity.stdp.learning_rate", 0.01)
    config.set("brain.plasticity.hebbian.learning_rate", 0.005)
    config.set("sensory.vision.width", 64)
    config.set("sensory.vision.height", 64)
    config.set("sensory.audio.sample_rate", 44100)
    config.set("world.width", 30)
    config.set("world.height", 30)
    config.set("world.max_energy", 100.0)
    config.set("development.stage", "Adult")
    
    # Convert to JSON-compatible dictionary
    config_dict = {}
    for key in config.get_keys():
        config_dict[key] = config.get(key)
    
    print(f"Configuration as dictionary:")
    print(json.dumps(config_dict, indent=2, default=str))
    
    # Create BrainConfig for comparison
    brain_config_dict = {
        "brain": {
            "neuron_count": 5000,
            "synapse_density": 0.15,
            "plasticity": {
                "stdp": {"learning_rate": 0.01},
                "hebbian": {"learning_rate": 0.005}
            }
        },
        "world": {
            "width": 30,
            "height": 30,
            "max_energy": 100.0
        }
    }
    
    print(f"\nBrain configuration as JSON:")
    print(json.dumps(brain_config_dict, indent=2))
    
    # Save to file
    import tempfile
    import os
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json_file = f.name
        json.dump(brain_config_dict, f, indent=2)
    
    print(f"\nConfiguration saved to: {json_file}")
    
    # Load from file
    with open(json_file, 'r') as f:
        loaded_config_dict = json.load(f)
    
    print(f"Configuration loaded from file: {len(loaded_config_dict)} keys")
    
    # Clean up
    os.unlink(json_file)
    
    print("✓ JSON configuration management completed")
    print()


def run_all_advanced_examples():
    """Run all advanced examples."""
    print("NLM Python Bindings - Advanced Examples")
    print("=" * 60)
    print()
    
    examples = [
        example_complex_configuration,
        example_learning_systems,
        example_sensory_processing,
        example_neural_network_analysis,
        example_world_interaction,
        example_performance_benchmarking,
        example_error_handling,
        example_json_configuration,
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"✗ Example {i} failed: {e}")
            import traceback
            traceback.print_exc()
            print()
    
    print("=" * 60)
    print("All advanced examples completed!")
    print("=" * 60)


if __name__ == "__main__":
    run_all_advanced_examples()
