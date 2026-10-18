#!/usr/bin/env python3
"""
NLM Python API Examples
======================

This file demonstrates the enhanced Python API for NLM (Neural Learning Machine).
It showcases the new Pythonic features and provides practical examples for users.

The NLM Python API allows you to create, configure, and run neural simulations
using the NLM C++ framework through Python bindings.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    print("✓ NLM Python API imported successfully")
except ImportError as e:
    print(f"❌ Failed to import NLM Python API: {e}")
    sys.exit(1)

def example_basic_setup():
    """
    Example 1: Basic NLM setup
    Demonstrates creating a brain with default configuration.
    """
    print("\n" + "="*60)
    print("EXAMPLE 1: Basic NLM Setup")
    print("="*60)
    
    # Create a brain with default configuration
    brain = pynlm.create_brain()
    print("✓ Brain created with default configuration")
    
    # Initialize the brain
    brain.initialize()
    print("✓ Brain initialized")
    
    # Get brain statistics
    stats = pynlm.get_simulation_stats(brain)
    print(f"  - Total neurons: {stats.get('total_spikes', 'N/A')}")
    print(f"  - Average firing rate: {stats.get('average_firing_rate', 'N/A')} Hz")
    
    return brain

def example_custom_setup():
    """
    Example 2: Custom configuration
    Demonstrates creating a brain with custom parameters.
    """
    print("\n" + "="*60)
    print("EXAMPLE 2: Custom Configuration")
    print("="*60)
    
    # Create a brain with custom parameters
    brain = pynlm.create_brain(
        neuron_count=2000,
        region_count=2,
        connection_prob=0.15
    )
    print("✓ Brain created with custom parameters:")
    print(f"  - Neuron count: 2000")
    print(f"  - Region count: 2")
    print(f"  - Connection probability: 0.15")
    
    brain.initialize()
    print("✓ Brain initialized")
    
    return brain

def example_world_setup():
    """
    Example 3: World setup
    Demonstrates creating and configuring a world environment.
    """
    print("\n" + "="*60)
    print("EXAMPLE 3: World Setup")
    print("="*60)
    
    # Create and configure a world
    world = pynlm.create_world(
        width=32,
        height=32,
        vision_width=16,
        vision_height=16
    )
    print("✓ World created and configured:")
    print(f"  - World size: 32 x 32 cells")
    print(f"  - Vision size: 16 x 16 cells")
    
    return world

def example_agent_setup():
    """
    Example 4: Agent setup
    Demonstrates creating an agent with brain and world.
    """
    print("\n" + "="*60)
    print("EXAMPLE 4: Agent Setup")
    print("="*60)
    
    # Create brain and world
    brain = pynlm.create_brain()
    world = pynlm.create_world()
    
    # Create agent
    agent = pynlm.create_agent(brain, world)
    print("✓ Agent created with brain and world")
    
    # Configure agent capabilities
    agent.enable_reward_modulation(True)
    agent.enable_curiosity(True)
    agent.enable_development(True)
    print("✓ Agent capabilities enabled:")
    print("  - Reward modulation: ON")
    print("  - Curiosity: ON")
    print("  - Development: ON")
    
    # Get agent metrics
    curiosity = agent.get_curiosity_level()
    novelty = agent.get_novelty_level()
    print(f"✓ Agent metrics: Curiosity={curiosity:.3f}, Novelty={novelty:.3f}")
    
    return agent

def example_sensory_input():
    """
    Example 5: Sensory input
    Demonstrates creating and using sensory inputs.
    """
    print("\n" + "="*60)
    print("EXAMPLE 5: Sensory Input")
    print("="*60)
    
    # Create various sensory inputs
    vision = pynlm.create_vision(64, 64, 3)
    audio = pynlm.create_audio(44100, 1024)
    internal = pynlm.create_internal_signals()
    
    print("✓ Sensory inputs created:")
    print("  - Vision: 64x64x3")
    print("  - Audio: 44100 Hz, 1024 samples")
    print("  - Internal signals")
    
    # Configure vision data
    vision_data = [0.1] * (64 * 64 * 3)
    for i in range(10):
        vision.setData(vision_data)
    
    print("✓ Vision data configured")
    
    # Configure audio data
    audio_data = [0.0] * 1024
    for i in range(10):
        audio.setData(audio_data)
        audio.setSampleRate(44100)
    
    print("✓ Audio data configured")
    
    return vision, audio, internal

def example_action_creation():
    """
    Example 6: Action creation
    Demonstrates creating various actions for the agent.
    """
    print("\n" + "="*60)
    print("EXAMPLE 6: Action Creation")
    print("="*60)
    
    # Create different types of actions
    move_forward = pynlm.create_action(pynlm.ActionType.MoveForward)
    look_right = pynlm.create_action(pynlm.ActionType.TurnRight)
    eat = pynlm.create_action(pynlm.ActionType.Eat)
    
    # Create actions with parameters
    move_with_params = pynlm.create_action(
        pynlm.ActionType.MoveForward,
        [1.0, 0.5]  # x, y movement
    )
    
    look_with_params = pynlm.create_action(
        pynlm.ActionType.Look,
        [45.0]  # angle in degrees
    )
    
    print("✓ Actions created:")
    print("  - Move forward (simple)")
    print("  - Turn right (simple)")
    print("  - Eat (simple)")
    print("  - Move with parameters (x=1.0, y=0.5)")
    print("  - Look with parameters (angle=45°)")
    
    # Create helper actions
    move_action = pynlm.move_action(2.0, 1.0)
    look_action = pynlm.look_action(-30.0)
    
    print("✓ Helper actions created:")
    print("  - Movement action: x=2.0, y=1.0")
    print("  - Look action: angle=-30°")
    
    return move_forward, look_right, eat, move_with_params, look_with_params, move_action, look_action

def example_simulation():
    """
    Example 7: Simulation
    Demonstrates running a simulation with the NLM API.
    """
    print("\n" + "="*60)
    print("EXAMPLE 7: Simulation")
    print("="*60)
    
    # Setup simulation components
    brain = pynlm.create_brain(100, 1000, 0.001)
    world = pynlm.create_world(16, 16, 8, 8)
    agent = pynlm.create_agent(brain, world)
    
    print("✓ Simulation components setup")
    
    # Run simulation steps
    num_steps = 100
    for step in range(num_steps):
        # Get observation from world
        percept = world.observe(brain)
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Run brain step
        pynlm.run_step(brain, step, step * 0.001)
        
        # Get motor command
        action = agent.decodeMotorCommand()
        
        # Apply action to world
        world.applyAction(action, step * 0.001)
        
        # Compute reward
        reward = world.computeReward(agent.getBrain())
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.001)
        
        # Log progress every 20 steps
        if step % 20 == 0:
            firing_rate = agent.getBrain().getAverageFiringRate()
            curiosity = agent.get_curiosity_level()
            print(f"  Step {step}: Firing rate={firing_rate:.2f} Hz, "
                  f"Curiosity={curiosity:.3f}, Reward={reward:.3f}")
    
    # Get final statistics
    stats = pynlm.get_simulation_stats(brain)
    print(f"\n✓ Simulation completed!")
    print(f"  - Total steps: {num_steps}")
    print(f"  - Final firing rate: {stats.get('average_firing_rate', 'N/A')} Hz")
    print(f"  - Total spikes: {stats.get('total_spikes', 'N/A')}")
    print(f"  - Active neurons: {stats.get('active_neurons', 'N/A')}")
    
    return brain, world, agent

def example_checkpoints():
    """
    Example 8: Checkpoints
    Demonstrates checkpoint and replay functionality.
    """
    print("\n" + "="*60)
    print("EXAMPLE 8: Checkpoints and Replay")
    print("="*60)
    
    # Create a brain and run simulation
    brain = pynlm.create_brain(200, 500, 0.001)
    
    # Save checkpoint
    checkpoint_path = "test_checkpoint.bin"
    success = pynlm.export_brain_data(brain, checkpoint_path)
    
    if (success):
        print(f"✓ Checkpoint saved to: {checkpoint_path}")
        
        # Load checkpoint into new brain
        loaded_brain = pynlm.import_brain_data(checkpoint_path)
        
        # Verify loaded brain has similar state
        loaded_stats = pynlm.get_simulation_stats(loaded_brain)
        print(f"✓ Checkpoint loaded successfully")
        print(f"  - Loaded brain neuron count: {loaded_stats.get('neuron_count', 'N/A')}")
        
        # Test replay functionality
        replay_success = pynlm.replayBrainState(checkpoint_path)
        if (replay_success):
            print("✓ Replay functionality working")
        else:
            print("⚠️  Replay functionality not available")
    else:
        print("⚠️  Checkpoint functionality not available")
    
    return brain

def example_configuration():
    """
    Example 9: Configuration
    Demonstrates configuration validation and management.
    """
    print("\n" + "="*60)
    print("EXAMPLE 9: Configuration")
    print("="*60)
    
    # Create and validate configuration
    config = pynlm.createDefaultConfig()
    
    # Validate configuration
    validation_result = pynlm.validate_config(config)
    
    if (validation_result.isValid) {
        print("✓ Configuration is valid")
    } else {
        print("⚠️  Configuration has issues:")
        for error in validation_result.errors:
            print(f"  - {error}")
    
    # Show suggested values
    if (!validation_result.suggestedValues.empty()) {
        print("  Suggested fixes:")
        for key, value : validation_result.suggestedValues.items() {
            print(f"    {key}: {value}")
        }
    }
    
    # Show warnings
    if (!validation_result.warnings.empty()) {
        print("  Warnings:")
        for warning in validation_result.warnings:
            print(f"    - {warning}")
    }
    
    return config

def example_integration():
    """
    Example 10: Full integration
    Demonstrates complete NLM integration using all components.
    """
    print("\n" + "="*60)
    print("EXAMPLE 10: Full Integration")
    print("="*60)
    
    print("Setting up complete NLM integration...")
    
    # Create all components
    brain = pynlm.create_brain()
    world = pynlm.create_world()
    agent = pynlm.create_agent(brain, world)
    
    # Create sensory inputs
    vision = pynlm.create_vision(32, 32, 3)
    audio = pynlm.create_audio(8000, 512)
    
    # Create actions
    move = pynlm.move_action(1.0, 0.0)
    look = pynlm.look_action(0.0)
    
    print("✓ All components integrated successfully")
    
    # Run integrated simulation
    for step in range(50):
        # Get sensor readings
        percept = world.observe(brain)
        agent.processSensoryInput(percept)
        
        # Brain processes input and produces output
        pynlm.run_step(brain, step, step * 0.001)
        
        # Agent makes decisions
        action = agent.decodeMotorCommand()
        world.applyAction(action, step * 0.001)
        
        # Learn from experience
        reward = world.computeReward(agent.getBrain())
        agent.applyRewardModulation(reward, 0.0)
        agent.updateDevelopment(0.001)
    
    # Get final results
    stats = pynlm.get_simulation_stats(brain)
    curiosity = agent.get_curiosity_level()
    novelty = agent.get_novelty_level()
    
    print(f"\n✓ Integration completed successfully!")
    print(f"  - Simulation steps: 50")
    print(f"  - Final firing rate: {stats.get('average_firing_rate', 'N/A')} Hz")
    print(f"  - Curiosity level: {curiosity:.3f}")
    print(f"  - Novelty level: {novelty:.3f}")
    print(f"  - Total reward: {stats.get('total_reward', 'N/A')}")
    
    return brain, world, agent

def print_examples_summary():
    """
    Print summary of all examples
    """
    print("\n" + "="*60)
    print("EXAMPLES SUMMARY")
    print("="*60)
    
    examples = [
        ("Basic Setup", "Creating a brain with default configuration"),
        ("Custom Configuration", "Creating a brain with custom parameters"),
        ("World Setup", "Creating and configuring a world environment"),
        ("Agent Setup", "Creating an agent with brain and world"),
        ("Sensory Input", "Creating and using sensory inputs"),
        ("Action Creation", "Creating various actions for the agent"),
        ("Simulation", "Running a complete simulation"),
        ("Checkpoints", "Checkpoint and replay functionality"),
        ("Configuration", "Configuration validation and management"),
        ("Full Integration", "Complete NLM integration example")
    ]
    
    for i, (name, description) in enumerate(examples, 1):
        print(f"{i:2d}. {name:25s} - {description}")
    
    print(f"\nTotal examples: {len(examples)}")
    print("\nTo run these examples, save this file as 'examples.py' and execute:")
    print("  python examples.py")

def main():
    """
    Main function to run all examples
    """
    print("NLM Python API Examples")
    print("="*60)
    print("This script demonstrates the enhanced Python API for NLM.")
    print("Each example shows a different aspect of the NLM functionality.")
    print()
    
    # Check if examples should be run
    if len(sys.argv) > 1 and sys.argv[1] == "--run":
        try:
            example_basic_setup()
            example_custom_setup()
            example_world_setup()
            example_agent_setup()
            example_sensory_input()
            example_action_creation()
            example_simulation()
            example_checkpoints()
            example_configuration()
            example_integration()
            
            print("\n" + "="*60)
            print("ALL EXAMPLES COMPLETED SUCCESSFULLY")
            print("="*60)
            
        except Exception as e:
            print(f"\n❌ Example failed: {e}")
            import traceback
            traceback.print_exc()
            sys.exit(1)
    else:
        print("Run examples with: python examples.py --run")
        print()
        print("Available options:")
        print("  --run     Run all examples")
        print("  --list    Show examples summary")
        print()
        
        if "--list" in sys.argv:
            print_examples_summary()
    
    return 0

if __name__ == "__main__":
    sys.exit(main())
