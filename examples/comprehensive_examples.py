#!/usr/bin/env python3
"""
Comprehensive examples for the improved NLM Python bindings.

This file demonstrates the enhanced Python API for the NLM (Neural Learning Machine) framework.
It showcases the improved error handling, convenience methods, and advanced features
that make the Python bindings more powerful and user-friendly.

To run these examples, ensure you have the pynlm module installed:
    pip install -e .  # or pip install .
"""

import pynlm
import time
import numpy as np

def example_1_basic_usage():
    """Example 1: Basic usage of the improved Python bindings."""
    print("=" * 60)
    print("Example 1: Basic Usage")
    print("=" * 60)
    
    # Create a brain with default configuration
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    
    # Initialize the brain (always needed before use)
    brain.initialize()
    
    # Get basic statistics
    print(f"Created brain with {brain.get_total_neuron_count()} neurons")
    print(f"Expected synapses: ~{brain.get_total_synapse_count()}")
    
    # Run a simple simulation
    for step in range(10):
        brain.step(step)
        print(f"Step {step}: {brain.get_firing_neuron_count()} neurons firing")
    
    print("✓ Basic usage example completed\n")

def example_2_enhanced_config_api():
    """Example 2: Enhanced Config API with factory methods."""
    print("=" * 60)
    print("Example 2: Enhanced Config API")
    print("=" * 60)
    
    # Create configuration using the improved API
    config = pynlm.create_default_config()
    
    # Use the dictionary-based config creation
    config_dict = {
        "brain.neuron_count": 500,
        "brain.connection_probability": 0.1,
        "plasticity.stdp.enable": True,
        "neuromod.dopamine.scale": 1.0
    }
    
    enhanced_config = pynlm.create_config_from_dict(config_dict)
    
    # Configure brain
    brain = pynlm.create_brain(enhanced_config)
    brain.initialize()
    
    # Print configuration summary
    summary = enhanced_config.summary()
    print("Configuration summary:")
    print(summary)
    
    # Verify configuration values
    if enhanced_config.has("brain.neuron_count"):
        neuron_count = enhanced_config.get("brain.neuron_count")
        print(f"Neuron count: {neuron_count}")
    
    print("✓ Enhanced Config API example completed\n")

def example_3_enhanced_action_api():
    """Example 3: Enhanced Action API with helper methods."""
    print("=" * 60)
    print("Example 3: Enhanced Action API")
    print("=" * 60)
    
    # Create action with different constructors
    action1 = pynlm.Action(pynlm.ActionType.MoveForward)
    action2 = pynlm.Action(pynlm.ActionType.Look, [0.5, 0.5])
    action3 = pynlm.Action(pynlm.ActionType.Custom, [1.0, 2.0, 3.0])
    
    # Use enhanced action methods
    print(f"Action 1 type: {action1.get_type()} - is_move: {action1.is_move_action()}")
    print(f"Action 2 type: {action2.get_type()} - is_look: {action2.is_look_action()}")
    print(f"Action 3 type: {action3.get_type()} - is_interact: {action3.is_interact_action()}")
    
    # Clone actions
    cloned_action = action1.clone()
    print(f"Cloned action type: {cloned_action.get_type()}")
    
    # Create actions from strings
    action_from_name = pynlm.Action.from_name("MoveForward")
    print(f"Action from name: {action_from_name.get_name()}")
    
    print("✓ Enhanced Action API example completed\n")

def example_4_enhanced_sensory_api():
    """Example 4: Enhanced Sensory API with visualization helpers."""
    print("=" * 60)
    print("Example 4: Enhanced Sensory API")
    print("=" * 60)
    
    # Create vision input
    vision = pynlm.Vision(width=10, height=10, channels=3)
    
    # Generate sample data (simulating an image)
    np.random.seed(42)
    data = np.random.rand(10 * 10 * 3).astype(np.float32)
    vision.set_data(data)
    
    # Use enhanced vision methods
    print(f"Vision dimensions: {vision.get_dimensions()}")
    print(f"Vision width: {vision.get_width()}, height: {vision.get_height()}")
    print(f"Vision channels: {vision.get_channels()}")
    
    # Get data as NumPy array (if available)
    try:
        image_array = vision.get_data_as_array()
        print(f"Data shape: {image_array.shape}")
        print(f"Data normalized: min={image_array.min():.3f}, max={image_array.max():.3f}")
    except AttributeError:
        print("get_data_as_array() not available (using C++ API)")
    
    # Normalize vision data
    vision.normalize(0.0, 255.0)
    
    # Create audio input
    audio = pynlm.Audio(sampleRate=44100, numSamples=44100)  # 1 second of audio
    audio_data = np.sin(2 * np.pi * 440 * np.arange(44100) / 44100).astype(np.float32)
    audio.set_data(audio_data)
    
    print(f"Audio duration: {audio.get_duration():.3f} seconds")
    print(f"Audio sample rate: {audio.get_sample_rate()}")
    
    print("✓ Enhanced Sensory API example completed\n")

def example_5_enhanced_world_api():
    """Example 5: Enhanced World API with object management."""
    print("=" * 60)
    print("Example 5: Enhanced World API")
    print("=" * 60)
    
    # Create world
    world = pynlm.create_simple_world()
    
    # Configure world
    world.configure(width=50, height=50, visionWidth=8, visionHeight=8)
    
    # Set agent start position
    world.set_agent_start(25.0, 25.0)
    
    # Reset world
    world.reset()
    
    # Create and add objects
    resource_obj = pynlm.WorldObject(
        x=10.0, y=10.0, 
        type=pynlm.WorldObjectType.Resource, 
        value=1.0
    )
    
    hazard_obj = pynlm.WorldObject(
        x=40.0, y=40.0, 
        type=pynlm.WorldObjectType.Hazard, 
        value=0.5
    )
    
    wall_obj = pynlm.WorldObject(
        x=30.0, y=5.0, 
        type=pynlm.WorldObjectType.Wall, 
        value=2.0, radius=1.0
    )
    
    world.add_object(resource_obj)
    world.add_object(hazard_obj)
    world.add_object(wall_obj)
    
    # Check object positions
    objects_near_agent = world.get_objects_near(25.0, 25.0, 15.0)
    print(f"Objects near agent (radius 15): {len(objects_near_agent)}")
    
    # Run simulation
    agent = pynlm.create_simple_agent(brain=None)  # Will set brain later
    agent.initialize(world)
    
    for step in range(5):
        world.update(0.1)
        percept = world.get_sensory_percept()
        
        print(f"Step {step}:")
        print(f"  Simulation time: {world.get_simulation_time():.3f}s")
        print(f"  Max energy: {world.get_max_energy()}")
        print(f"  Simulation time: {world.get_simulation_time():.3f}s")
        
        if step == 0:  # Just for demo
            agent.process_sensory_input(percept)
    
    print("✓ Enhanced World API example completed\n")

def example_6_advanced_agent_integration():
    """Example 6: Complete agent integration with advanced features."""
    print("=" * 60)
    print("Example 6: Advanced Agent Integration")
    print("=" * 60)
    
    # Create brain with custom configuration
    config = pynlm.create_default_config()
    
    # Customize configuration
    config.set("brain.neuron_count", 200)
    config.set("brain.region_count", 2)
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("neuromod.curiosity.enable", True)
    
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Create agent with world
    agent = pynlm.create_agent_brain(brain)
    world = pynlm.create_simple_world()
    world.configure(width=30, height=30, visionWidth=6, visionHeight=6)
    world.reset()
    
    # Initialize agent
    agent.initialize(world)
    
    # Enable all subsystems
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # Run simulation with logging
    print("Starting simulation with all subsystems enabled:")
    
    for step in range(20):
        # Update world
        world.update(0.1)
        
        # Get sensory input
        percept = world.get_sensory_percept()
        
        # Process in brain
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Decode action
        action = agent.decode_motor_command()
        
        # Apply action
        world.apply_motor_command(action, world.get_simulation_time())
        
        # Apply reward modulation (simplified)
        reward = 0.1 if step % 10 == 0 else 0.0
        agent.apply_reward_modulation(reward, reward * 0.5)
        
        # Update development
        agent.update_development(0.1)
        
        # Print status periodically
        if step % 5 == 0:
            print(f"\nStep {step}:")
            print(f"  Firing neurons: {brain.get_firing_neuron_count()}")
            print(f"  Total spikes: {brain.get_total_spike_count()}")
            print(f"  Curiosity: {agent.get_curiosity_level():.3f}")
            print(f"  Novelty: {agent.get_novelty_level():.3f}")
            print(f"  Prediction error: {agent.get_prediction_error():.3f}")
            print(f"  Development stage: {brain.get_developmental_stage()}")
            print(f"  Action: {action.get_name()}")
    
    print("\n✓ Advanced agent integration example completed\n")

def example_7_text_and_concept_processing():
    """Example 7: Text encoding and concept formation utilities."""
    print("=" * 60)
    print("Example 7: Text Encoding and Concept Formation")
    print("=" * 60)
    
    # Create brain and agent
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    agent = pynlm.create_agent_brain(brain)
    
    # Text encoding examples
    texts = ["hello", "world", "test", "example"]
    
    print("Text encoding examples:")
    for text in texts:
        try:
            pattern = pynlm.encode_text(text)
            print(f"  '{text}' -> pattern with {len(pattern)} neurons")
        except Exception as e:
            print(f"  '{text}' -> error: {e}")
    
    # Concept formation example
    print("\nConcept formation example:")
    
    # Create sample patterns
    pattern1 = [0.1, 0.2, 0.3, 0.4, 0.5]
    pattern2 = [0.15, 0.25, 0.35, 0.45, 0.55]  # Similar to pattern1
    pattern3 = [0.9, 0.8, 0.7, 0.6, 0.5]      # Different from pattern1
    
    # Compute similarities
    try:
        sim_same = pynlm.compute_concept_similarity(pattern1, pattern1)
        sim_similar = pynlm.compute_concept_similarity(pattern1, pattern2)
        sim_different = pynlm.compute_concept_similarity(pattern1, pattern3)
        
        print(f"  Pattern 1 vs Pattern 1 (same): {sim_same:.3f}")
        print(f"  Pattern 1 vs Pattern 2 (similar): {sim_similar:.3f}")
        print(f"  Pattern 1 vs Pattern 3 (different): {sim_different:.3f}")
        
        if sim_same > 0.9 and sim_similar > 0.7 and sim_different < 0.7:
            print("  ✓ Concept formation working correctly")
        else:
            print("  ! Concept formation may have issues")
            
    except Exception as e:
        print(f"  Concept formation error: {e}")
    
    print("✓ Text and concept processing example completed\n")

def example_8_error_handling_and_exceptions():
    """Example 8: Enhanced error handling and exception management."""
    print("=" * 60)
    print("Example 8: Enhanced Error Handling")
    print("=" * 60)
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Test error handling
    print("Testing error handling:")
    
    # Valid operation
    try:
        brain.step(0)
        print("  ✓ Valid step operation succeeded")
    except Exception as e:
        print(f"  ✗ Valid step operation failed: {e}")
    
    # Test parameter validation
    vision = pynlm.Vision(10, 10, 3)
    
    # Correct size data
    correct_data = [0.1] * (10 * 10 * 3)
    try:
        vision.set_data(correct_data)
        print("  ✓ Correct data size accepted")
    except Exception as e:
        print(f"  ✗ Correct data size rejected: {e}")
    
    # Incorrect size data (should fail)
    incorrect_data = [0.1] * 100  # Wrong size for 10x10x3
    try:
        vision.set_data(incorrect_data)
        print("  ✗ Incorrect data size accepted (should have failed)")
    except Exception as e:
        print(f"  ✓ Incorrect data size rejected: {e}")
    
    # Test action validation
    action = pynlm.Action(pynlm.ActionType.MoveForward)
    print(f"  ✓ Action created: {action.get_name()}")
    
    # Test world operations
    world = pynlm.create_simple_world()
    world.configure(20, 20, 8, 8)
    world.reset()
    
    # Valid position
    if world.is_valid_position(10.0, 10.0):
        print("  ✓ Valid position accepted")
    
    # Invalid position (outside bounds)
    if not world.is_valid_position(-5.0, -5.0):
        print("  ✓ Invalid position rejected")
    
    print("✓ Error handling example completed\n")

def example_9_performance_monitoring():
    """Example 9: Performance monitoring and simulation utilities."""
    print("=" * 60)
    print("Example 9: Performance Monitoring")
    print("=" * 60)
    
    config = pynlm.create_default_config()
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # Performance monitoring
    start_time = time.time()
    steps_to_run = 100
    
    for step in range(steps_to_run):
        brain.step(step)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"Performance metrics for {steps_to_run} steps:")
    print(f"  Total time: {elapsed:.3f} seconds")
    print(f"  Average time per step: {elapsed/steps_to_run*1000:.3f} ms")
    print(f"  Steps per second: {steps_to_run/elapsed:.1f}")
    
    # Memory usage monitoring
    total_spikes = brain.get_total_spike_count()
    firing_neurons = brain.get_firing_neuron_count()
    avg_firing_rate = brain.get_average_firing_rate()
    
    print(f"\nBrain state metrics:")
    print(f"  Total spikes: {total_spikes}")
    print(f"  Currently firing neurons: {firing_neurons}")
    print(f"  Average firing rate: {avg_firing_rate:.3f} Hz")
    print(f"  Simulation efficiency: {total_spikes/max(firing_neurons, 1):.1f} spikes per firing neuron")
    
    print("✓ Performance monitoring example completed\n")

def example_10_complete_workflow():
    """Example 10: Complete workflow with all components."""
    print("=" * 60)
    print("Example 10: Complete Workflow")
    print("=" * 60)
    
    print("Setting up complete NLM workflow...")
    
    # 1. Configuration
    print("1. Creating configuration...")
    config = pynlm.create_default_config()
    
    # 2. Brain creation
    print("2. Creating and initializing brain...")
    brain = pynlm.create_brain(config)
    brain.initialize()
    
    # 3. World creation
    print("3. Creating and configuring world...")
    world = pynlm.create_simple_world()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    world.reset()
    
    # 4. Agent creation
    print("4. Creating and initializing agent...")
    agent = pynlm.create_agent_brain(brain)
    agent.initialize(world)
    
    # 5. Enable subsystems
    print("5. Enabling all subsystems...")
    agent.enable_reward_modulation(True)
    agent.enable_structural_plasticity(True)
    agent.enable_development(True)
    agent.enable_curiosity(True)
    
    # 6. Run simulation
    print("6. Running simulation...")
    
    for episode in range(3):
        print(f"  Episode {episode + 1}:")
        
        # Reset world for new episode
        world.reset()
        world.set_agent_start(20.0, 20.0)
        
        # Run episode
        for step in range(30):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.get_sensory_percept()
            
            # Process input
            agent.process_sensory_input(percept)
            brain.step(step)
            
            # Get action and apply
            action = agent.decode_motor_command()
            world.apply_motor_command(action, world.get_simulation_time())
            
            # Apply learning
            reward = 0.1
            agent.apply_reward_modulation(reward, reward * 0.5)
            agent.update_development(0.1)
            
            # Log periodically
            if step % 10 == 0:
                print(f"    Step {step}: {brain.get_firing_neuron_count()} firing neurons")
        
        # Log episode summary
        print(f"    Episode {episode + 1} complete:")
        print(f"      Total spikes: {brain.get_total_spike_count()}")
        print(f"      Avg firing rate: {brain.get_average_firing_rate():.3f} Hz")
        print(f"      Curiosity: {agent.get_curiosity_level():.3f}")
        print(f"      Novelty: {agent.get_novelty_level():.3f}")
    
    print("\n✓ Complete workflow example completed\n")
    print("Summary:")
    print("  Successfully demonstrated all major NLM Python APIs")
    print("  Included configuration, brain, world, agent integration")
    print("  Showed advanced features (neuromodulation, development, curiosity)")
    print("  Provided comprehensive error handling and performance monitoring")

if __name__ == "__main__":
    print("NLM Python Bindings - Comprehensive Examples")
    print("=" * 60)
    print()
    
    # Run all examples
    example_1_basic_usage()
    example_2_enhanced_config_api()
    example_3_enhanced_action_api()
    example_4_enhanced_sensory_api()
    example_5_enhanced_world_api()
    example_6_advanced_agent_integration()
    example_7_text_and_concept_processing()
    example_8_error_handling_and_exceptions()
    example_9_performance_monitoring()
    example_10_complete_workflow()
    
    print("\n" + "=" * 60)
    print("All examples completed successfully!")
    print("=" * 60)
