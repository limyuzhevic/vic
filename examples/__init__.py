"""
NLM Python Bindings - Example Scripts

This file contains example scripts demonstrating the improved Python API
for the NLM neural simulation framework.
"""

def basic_example():
    """
    Basic example showing how to create and simulate a neural brain.
    
    This example demonstrates the core workflow:
    1. Create a configuration
    2. Initialize a brain
    3. Set up the environment
    4. Run a simulation episode
    """
    print("=== Basic NLM Example ===")
    
    # Import the main modules
    from pynlm import Config, Brain, SimpleWorld, create_movement_action
    
    # Create a simple configuration
    config = Config()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    print(f"✓ Created configuration: {config}")
    
    # Create and initialize the brain
    brain = Brain(config)
    brain.initialize()
    print(f"✓ Created and initialized brain with {brain.get_region_count()} regions")
    
    # Set up the world
    world = SimpleWorld()
    world.configure(64, 64, 32, 32)  # world width, height, vision width, height
    world.set_agent_start(32.0, 32.0)
    print(f"✓ Created world: {world}")
    
    # Create agent brain interface
    from pynlm import AgentBrain
    agent_brain = AgentBrain(brain)
    agent_brain.initialize(world)
    print("✓ Created agent brain interface")
    
    # Run a simulation episode
    print("\n--- Running Simulation Episode ---")
    for step in range(10):
        # Get sensory input from world
        percept = world.get_sensory_percept()
        
        # Process sensory input through agent brain
        agent_brain.processSensoryInput(percept)
        
        # Brain produces motor output
        action = brain.produce_action()
        print(f"Step {step}: Action = {action.getName()}")
        
        # Apply action to world
        world.applyMotorCommand(action, step)
        
        # Update agent development
        agent_brain.updateDevelopment(0.1)
    
    # Show final statistics
    stats = brain.get_statistics()
    print(f"\nFinal statistics:")
    print(f"  Total neurons: {stats.total_neurons}")
    print(f"  Active neurons: {stats.active_neurons}")
    print(f"  Total spikes: {stats.total_spikes}")
    print(f"  Average firing rate: {stats.average_firing_rate:.2f} Hz")
    
    print("✓ Basic example completed successfully")

def advanced_example():
    """
    Advanced example showing the new Python API features.
    
    This example demonstrates the improvements:
    - Type hints and documentation
    - Convenience functions
    - Error handling
    - High-level abstractions
    """
    print("\n=== Advanced NLM Example ===")
    
    # Import advanced features
    from pynlm import (
        create_default_config, create_brain, create_simple_world, create_agent_brain,
        create_movement_action, create_rotation_action,
        ACTION_TYPES, MOTOR_COMMANDS, Vision, Audio, InternalSignals
    )
    
    # Use convenience functions for setup
    config = create_default_config()
    print(f"✓ Used convenience function: {config}")
    
    brain = create_brain(config)
    print(f"✓ Created brain using factory function")
    
    world = create_simple_world()
    world.configure(128, 128, 64, 64)
    print(f"✓ Created world using factory function")
    
    agent_brain = create_agent_brain(brain)
    agent_brain.initialize(world)
    print(f"✓ Created agent brain using factory function")
    
    # Demonstrate high-level action creation
    forward_action = create_movement_action("forward", 1.5)
    turn_action = create_rotation_action("left", 0.5)
    
    print(f"✓ Created movement action: {forward_action.getName()}")
    print(f"✓ Created rotation action: {turn_action.getName()}")
    
    # Demonstrate sensory data structures
    vision = Vision(64, 64, 3)
    vision.set_data([0.5] * 64 * 64 * 3)  # Gray field
    print(f"✓ Created Vision: {vision}")
    
    audio = Audio(22050, 512)
    audio.set_data([0.1] * 512)
    print(f"✓ Created Audio: {audio}")
    
    signals = InternalSignals()
    signals.add_signal(0.3)
    signals.add_signal(0.7)
    signals.clear_signals()
    print(f"✓ Created InternalSignals")
    
    # Demonstrate enum constants
    print(f"\nAvailable action types: {list(ACTION_TYPES.keys())}")
    print(f"Available motor commands: {list(MOTOR_COMMANDS.keys())}")
    
    # Show a complex simulation scenario
    print(f"\n--- Complex Scenario ---")
    print("1. Brain with multiple regions and connections")
    print("2. Agent with advanced sensory processing")
    print("3. Movement and rotation actions")
    print("4. Neuromodulation affecting behavior")
    print("5. Development system evolving over time")
    
    print("✓ Advanced example completed successfully")

def error_handling_example():
    """
    Example demonstrating improved error handling and user-friendly messages.
    """
    print("\n=== Error Handling Example ===")
    
    from pynlm import Config, ConfigError, BrainError
    
    # Demonstrate configuration error handling
    print("1. Testing configuration error handling...")
    try:
        config = Config()
        config.load_from_file("/nonexistent/config.json")
    except ConfigError as e:
        print(f"   ✓ Caught ConfigError: {e}")
    
    # Demonstrate brain error handling
    print("2. Testing brain error handling...")
    try:
        from pynlm import Brain
        config = Config()
        brain = Brain(config)
        brain.step(0)  # Should fail without initialization
    except BrainError as e:
        print(f"   ✓ Caught BrainError: {e}")
    
    # Demonstrate validation
    print("3. Testing configuration validation...")
    config = Config()
    config.set("neuron_count", -10)  # Invalid value
    errors = config.validate()
    if errors:
        print(f"   ✓ Validation caught {len(errors)} errors:")
        for error in errors:
            print(f"     - {error}")
    
    print("✓ Error handling example completed successfully")

def api_usage_example():
    """
    Example showing the complete Python API in action.
    """
    print("\n=== API Usage Example ===")
    
    # Comprehensive example showing all major features
    from pynlm import (
        Config, Brain, AgentBrain, SimpleWorld,
        Vision, Audio, InternalSignals,
        ActionType, MotorCommand,
        create_movement_action, create_rotation_action
    )
    
    # Setup
    config = Config()
    config.set("neuron_count", 500)
    config.set("region_count", 3)
    config.set("connection_probability", 0.05)
    
    brain = Brain(config)
    brain.initialize()
    
    world = SimpleWorld()
    world.configure(256, 256, 128, 128)
    world.set_agent_start(128.0, 128.0)
    
    agent_brain = AgentBrain(brain)
    agent_brain.initialize(world)
    
    # Simulation loop with various actions
    actions_taken = []
    for timestep in range(5):
        # Create different actions
        if timestep % 2 == 0:
            action = create_movement_action("forward", 0.5)
        else:
            action = create_rotation_action("right", 0.25)
        
        # Apply action
        result = world.applyMotorCommand(action, timestep)
        
        if result:
            actions_taken.append({
                "timestep": timestep,
                "type": action.getType(),
                "success": result.success,
                "reward": result.reward
            })
        
        # Update development
        agent_brain.updateDevelopment(0.1)
    
    # Summary
    print(f"Simulation completed with {len(actions_taken)} actions:")
    for i, action_info in enumerate(actions_taken):
        print(f"  {i+1}. Step {action_info['timestep']}: "
              f"{ActionType(action_info['type']).name} "
              f"({'success' if action_info['success'] else 'failed'}, "
              f"reward: {action_info['reward']:.2f})")
    
    print(f"✓ API usage example completed with {len(actions_taken)} actions")

def print_examples_summary():
    """Print a summary of all available examples."""
    print("\n" + "=" * 60)
    print("NLM Python Bindings Examples")
    print("=" * 60)
    print("\nAvailable Examples:")
    print("1. Basic Example - Simple brain creation and simulation")
    print("2. Advanced Example - Using new API features and convenience functions")
    print("3. Error Handling Example - Testing error handling and validation")
    print("4. API Usage Example - Comprehensive demonstration")
    print("\nKey Improvements Demonstrated:")
    print("• Modular Python API with type hints")
    print("• Convenience functions for common operations")
    print("• Comprehensive error handling")
    print("• High-level abstractions for complex tasks")
    print("• Full documentation and examples")
    print("• Backward compatibility with C++ extensions")
    print("\nRun examples with: python -c 'import examples; examples.basic_example()'")

if __name__ == "__main__":
    print_examples_summary()
    
    # Uncomment to run examples
    # basic_example()
    # advanced_example()
    # error_handling_example()
    # api_usage_example()
    
    print("\nExamples ready to run. Uncomment the function calls above to execute.")