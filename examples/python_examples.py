"""
Example usage of the enhanced NLM Python bindings.

This file demonstrates the new features and improved user experience
provided by the enhanced Python bindings.
"""

import pynlm


def example_quick_start():
    """Quick start example - the simplest way to use NLM."""
    print("=== Quick Start Example ===")
    
    # Create a brain with default configuration
    print("Creating a brain with default configuration...")
    brain = pynlm.Brain.createDefault()
    brain.initialize()
    
    # Run a simple simulation
    print("Running 100 simulation steps...")
    brain.run_steps(100)
    
    # Get quick statistics
    print("Getting brain statistics...")
    stats = brain.get_stats()
    print(f"  Total neurons: {stats['total_neuron_count']}")
    print(f"  Total spikes: {stats['total_spike_count']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.2f} Hz")
    
    print("✓ Quick start completed successfully!\n")


def example_context_manager():
    """Example using context manager for automatic cleanup."""
    print("=== Context Manager Example ===")
    
    # Use context manager for automatic cleanup
    print("Using context manager for automatic cleanup...")
    with pynlm.Brain.createDefault() as brain:
        brain.initialize()
        brain.run_steps(50)
    
    print("✓ Context manager example completed (automatic cleanup handled)\n")


def example_configuration_presets():
    """Example using configuration presets."""
    print("=== Configuration Presets Example ===")
    
    presets = ["development", "production", "research", "demo"]
    
    for preset_name in presets:
        print(f"Creating brain with '{preset_name}' preset...")
        config = pynlm.Config.from_preset(preset_name)
        
        # Validate configuration
        validation = config.validate()
        print(f"  Validation result:\n  {validation}")
        
        # Create brain
        brain = pynlm.Brain(config)
        brain.initialize()
        
        # Run a few steps
        brain.run_steps(10)
        
        # Get stats
        stats = brain.get_stats()
        print(f"  Stats: {stats['total_neuron_count']} neurons, {stats['total_spike_count']} spikes\n")
    
    print("✓ Configuration presets example completed\n")


def example_world_interactions():
    """Example using world interactions."""
    print("=== World Interactions Example ===")
    
    # Create a world with objects
    print("Creating a standard world...")
    world = pynlm.SimpleWorld.create_standard(100, 100)
    
    # Add an agent
    print("Adding agent at position (10, 10)...")
    world.add_agent(10, 10)
    
    # Add various objects
    print("Adding resources and hazards...")
    world.add_resource(50, 50, 20.0)  # Resource with value 20
    world.add_hazard(75, 75, 10.0)    # Hazard with damage 10
    
    # Create actions
    print("Creating actions...")
    move_action = pynlm.Action.create_move("forward", 1.0)
    look_action = pynlm.Action.create_look(0.1, 0.0)
    interact_action = pynlm.Action.create_interact()
    
    print(f"  Move action type: {move_action.get_type()}")
    print(f"  Look action type: {look_action.get_type()}")
    print(f"  Interact action type: {interact_action.get_type()}")
    
    # Show world state
    body = world.get_agent_body()
    print(f"  Agent position: ({body.x}, {body.y})")
    print(f"  Agent energy: {body.energy}")
    
    print("✓ World interactions example completed\n")


def example_advanced_usage():
    """Example of advanced usage patterns."""
    print("=== Advanced Usage Example ===")
    
    # Create a world with objects
    world = pynlm.create_demo_world()
    
    # Create an agent brain for the world
    agent_brain = pynlm.create_agent_for_world(world)
    
    # Get the underlying brain
    brain = agent_brain.get_brain()
    
    # Test convergence waiting
    print("Waiting for brain to reach target firing rate...")
    converged = brain.wait_for_convergence(10.0, 2.0)
    if converged:
        print("  ✓ Brain reached target firing rate")
    else:
        print("  ⚠ Timeout reached (this is normal in demo mode)")
    
    # Simulate an episode
    print("Simulating an episode...")
    reward, steps = pynlm.simulate_episode(brain, world, 200, 50.0)
    print(f"  Episode completed: {steps} steps, {reward:.2f} reward")
    
    # Validate the setup
    validation = pynlm.validate_setup(brain, world)
    print(f"  Setup validation:\n  {validation}")
    
    print("✓ Advanced usage example completed\n")


def example_error_handling():
    """Example of error handling."""
    print("=== Error Handling Example ===")
    
    # Test error handling for invalid presets
    print("Testing invalid preset error handling...")
    try:
        config = pynlm.Config.from_preset("invalid_preset")
    except pynlm.ConfigValidationError as e:
        print(f"  ✓ Caught ConfigValidationError: {e}")
    
    # Test error handling for invalid actions
    print("\nTesting invalid action error handling...")
    try:
        action = pynlm.Action.create_move("invalid_direction")
    except pynlm.ActionValidationError as e:
        print(f"  ✓ Caught ActionValidationError: {e}")
    
    print("✓ Error handling example completed\n")


def example_type_conversions():
    """Example of type conversions."""
    print("=== Type Conversions Example ===")
    
    # Create config
    config = pynlm.Config()
    
    # Set values using Python types
    print("Setting configuration values using Python types...")
    config.update_from_dict({
        "brain.neuron_count": 100,
        "brain.simulation_time_step": 0.01,
        "world.width": 50.0,
        "world.height": 50.0,
        "reward.gamma": 0.95,
        "enable_development": True,
        "mode": "research"
    })
    
    # Convert to dictionary
    config_dict = config.to_dict()
    print("\nConfiguration as Python dictionary:")
    for key, value in config_dict.items():
        print(f"  {key}: {value} (type: {type(value).__name__})")
    
    # Get with default
    width = config.get_with_default("world.width", 100.0)
    height = config.get_with_default("world.height", 100.0)
    custom = config.get_with_default("custom.key", "not_found")
    
    print(f"\nRetrieved values with defaults:")
    print(f"  width: {width}")
    print(f"  height: {height}")
    print(f"  custom (not found): {custom}")
    
    print("✓ Type conversions example completed\n")


def example_preset_constants():
    """Example using preset constants."""
    print("=== Preset Constants Example ===")
    
    print("Using preset constants for validation...")
    
    # Validate preset names
    valid_presets = [pynlm.PRESET_DEVELOPMENT, pynlm.PRESET_PRODUCTION, 
                     pynlm.PRESET_RESEARCH, pynlm.PRESET_DEMO]
    
    for preset in valid_presets:
        print(f"  Valid preset: {preset}")
    
    # Use in conditionals
    print("\nConditional preset handling...")
    if pynlm.PRESET_DEMO == "demo":
        print("  ✓ Demo preset detected")
    
    print("✓ Preset constants example completed\n")


def example_brain_utilities():
    """Example of brain utility methods."""
    print("=== Brain Utilities Example ===")
    
    brain = pynlm.create_demo_brain()
    
    # Check if brain is ready
    if brain.is_ready():
        print("Brain is ready for simulation")
        
        # Get comprehensive stats
        stats = brain.get_stats()
        print("\nBrain statistics:")
        for key, value in stats.items():
            print(f"  {key}: {value}")
        
        # Test run_steps
        print("\nRunning 50 simulation steps...")
        brain.run_steps(50)
        
        # Get updated stats
        updated_stats = brain.get_stats()
        print(f"After simulation: {updated_stats['total_spike_count']} total spikes")
    
    print("✓ Brain utilities example completed\n")


def main():
    """Run all examples."""
    print("=" * 70)
    print("Enhanced NLM Python Bindings - Usage Examples")
    print("=" * 70)
    
    examples = [
        ("Quick Start", example_quick_start),
        ("Context Manager", example_context_manager),
        ("Configuration Presets", example_configuration_presets),
        ("World Interactions", example_world_interactions),
        ("Advanced Usage", example_advanced_usage),
        ("Error Handling", example_error_handling),
        ("Type Conversions", example_type_conversions),
        ("Preset Constants", example_preset_constants),
        ("Brain Utilities", example_brain_utilities),
    ]
    
    for title, example_func in examples:
        print(f"\n{'=' * 70}")
        print(f"{title}")
        print(f"{'=' * 70}")
        try:
            example_func()
        except Exception as e:
            print(f"Example failed: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 70)
    print("All examples completed!")
    print("=" * 70)


if __name__ == "__main__":
    main()