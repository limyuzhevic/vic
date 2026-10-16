#!/usr/bin/env python3
"""
Quick Start Example for NLM Python Bindings

This script demonstrates the basic usage of the NLM Python bindings
and serves as a getting started guide for new users.
"""

import sys
import os

# Add the current directory to the path so we can import nlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import nlm
    NLM_AVAILABLE = True
except ImportError:
    NLM_AVAILABLE = False
    print("Error: NLM Python bindings not found.")
    print("Please build the project first:")
    print("  mkdir build")
    print("  cd build")
    print("  cmake ..")
    print("  make pynlm")
    sys.exit(1)
def print_header():
    """Print the example header."""
    print("=" * 70)
    print("NLM Python Bindings - Quick Start Example")
    print("=" * 70)
    print()
    print("This example demonstrates the enhanced NLM Python bindings")
    print("with the new Pythonic APIs, convenience methods, and")
    print("advanced functionality added to the bindings.")
    print()
def create_basic_brain():
    """Create a basic brain example."""
    print("1. Creating a Basic Brain")
    print("-" * 40)
    
    # Create default configuration
    config = nlm.create_default_config()
    print(f"Created default configuration with {len(config.getKeys())} keys")
    
    # Create brain using the new convenience function
    brain = nlm.createBrain(config)
    print("Created brain using createBrain()")
    
    # Initialize brain
    brain.initialize()
    print("Brain initialized")
    
    return brain, config
def explore_configuration(config):
    """Explore configuration using Pythonic APIs."""
    print("\n2. Configuration Exploration")
    print("-" * 40)
    
    # Show initial configuration
    print("Initial configuration values:")
    important_keys = [
        "brain.neuron_count",
        "brain.simulation_steps",
        "world.width",
        "world.height",
    ]
    
    for key in important_keys:
        if config.has(key):
            if key.endswith("neuron_count") or key.endswith("steps"):
                value = config.get_int(key, 0)
                print(f"  {key}: {value}")
            elif key.endswith("width") or key.endswith("height"):
                value = config.get_float(key, 0.0)
                print(f"  {key}: {value}")
            else:
                value = config.get(key, None)
                print(f"  {key}: {value}")
    
    # Modify configuration using Pythonic APIs
    print("\nModifying configuration...")
    config.set_int("brain.neuron_count", 2000)
    config.set_float("world.width", 50.0)
    config.set_bool("simulation.verbose", True)
    
    # Show modified configuration
    print("\nModified configuration values:")
    for key in important_keys:
        if config.has(key):
            if key.endswith("neuron_count") or key.endswith("steps"):
                value = config.get_int(key, 0)
                print(f"  {key}: {value}")
            elif key.endswith("width") or key.endswith("height"):
                value = config.get_float(key, 0.0)
                print(f"  {key}: {value}")
            else:
                value = config.get(key, None)
                print(f"  {key}: {value}")
    
    # Validate configuration
    print("\nValidating configuration...")
    errors = config.validate(["brain.neuron_count", "world.width"])
    
    if not errors:
        print("✓ Configuration is valid!")
    else:
        print(f"✗ Configuration has {len(errors)} errors:")
        for error in errors:
            print(f"  - {error}")
    
    return config
def create_world_and_simulation(brain, config):
    """Create world and run simulation."""
    print("\n3. World Creation and Simulation")
    print("-" * 40)
    
    # Create world using the new convenience function
    world = nlm.create_simple_demo_world()
    print("Created world using create_simple_demo_world()")
    
    # Configure world based on modified configuration
    world.configure(
        config.get_float("world.width", 100.0),
        config.get_float("world.height", 100.0),
        config.get_int("world.vision_width", 10),
        config.get_int("world.vision_height", 10)
    )
    
    print(f"World configured: {world.getWidth()}x{world.getHeight()}")
    print(f"Vision range: {world.getVisionWidth()}x{world.getVisionHeight()}")
    
    # Run simulation using the new run_simulation function
    print(f"\nRunning simulation for 100 steps...")
    result = nlm.run_simulation(brain, world, 100, config.get_float("simulation_timestep", 0.01))
    
    print(f"Simulation completed!")
    print(f"  Status: {result['status']}")
    print(f"  Total steps: {result['total_steps']}")
    print(f"  Final step: {result['final_step']}")
    
    return brain, world, result
def demonstrate_actions():
    """Demonstrate action creation and manipulation."""
    print("\n4. Action Creation and Manipulation")
    print("-" * 40)
    
    # Create different action types
    action_types = [
        ("Move Forward", nlm.ACTION_TYPE_MOVE_FORWARD),
        ("Move Backward", nlm.ACTION_TYPE_MOVE_BACKWARD),
        ("Turn Left", nlm.ACTION_TYPE_TURN_LEFT),
        ("Turn Right", nlm.ACTION_TYPE_TURN_RIGHT),
    ]
    
    for name, action_type in action_types:
        print(f"\n{name} Action:")
        
        # Create action using Pythonic APIs
        action = nlm.Action(action_type)
        action.add_param(1.0)  # Distance
        action.add_param(0.5)  # Speed
        
        # Modify action using convenience methods
        action.set_param(0, 2.0)  # Change distance to 2.0
        
        # Convert to dictionary for analysis
        action_dict = action.to_dict()
        print(f"  Type: {name}")
        print(f"  Parameters: {action_dict['parameters']}")
        print(f"  Name: {action_dict['name']}")
        
        # Show action statistics
        print(f"  Action clone test: {action.clone().getType() == action.getType()}")
    
    return True
def demonstrate_sensory_processing():
    """Demonstrate sensory data processing."""
    print("\n5. Sensory Data Processing")
    print("-" * 40)
    
    # Create brain and world
    config = nlm.create_default_config()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.create_simple_demo_world()
    
    # Get sensory perception
    percept = world.getSensoryPercept()
    print("Retrieved sensory perception from world")
    
    # Convert to dictionary for easy processing
    percept_dict = percept.to_dict()
    print("\nSensory perception data:")
    print(f"  Vision data shape: {len(percept_dict['vision'])} values")
    print(f"  Touch data: {percept_dict['touch']}")
    print(f"  Internal signals: {len(percept_dict['internal'])} signals")
    print(f"  Timestamp: {percept_dict['timestamp']}")
    
    # Process sensory data
    vision_data = percept_dict['vision']
    if vision_data:
        avg_vision = sum(vision_data) / len(vision_data)
        print(f"  Average vision: {avg_vision:.4f}")
    
    return percept

def demonstrate_agent_simulation():
    """Demonstrate agent-based simulation."""
    print("\n6. Agent-Based Simulation")
    print("-" * 40)
    
    # Create demo brain and world
    config = nlm.create_demo_config()
    brain = nlm.create_simple_demo_brain(config)
    world = nlm.create_simple_demo_world()
    
    # Create agent using the new convenience function
    agent = nlm.create_agent(brain, world)
    agent.initialize(world)
    
    print("Created agent using create_agent()")
    
    # Run agent episode
    print("\nRunning agent episode (50 steps)...")
    for step in range(50):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent.processSensoryInput(percept)
        
        # Simulate world step
        world.update(0.01)
        
        # Get brain action
        action = brain.produceAction()
        
        # Execute action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress
        if step % 10 == 0:
            print(f"  Step {step}: Active neurons = {brain.getActiveNeuronCount()}, "
                  f"Firing neurons = {brain.getFiringNeuronCount()}")
    
    print("\nAgent episode completed successfully!")
    
    # Show agent statistics
    brain_stats = brain.get_statistics()
    print(f"\nAgent statistics:")
    print(f"  Total neurons: {brain_stats['total_neurons']}")
    print(f"  Total spikes: {brain_stats['total_spikes']}")
    print(f"  Firing rate: {brain_stats['avg_firing_rate']:.2f} Hz")
    
    return agent, brain, world
def demonstrate_advanced_features():
    """Demonstrate advanced features."""
    print("\n7. Advanced Features")
    print("-" * 40)
    
    # Create brain for advanced demonstrations
    config = nlm.create_demo_config()
    brain = nlm.createBrain(config)
    brain.initialize()
    
    world = nlm.create_simple_demo_world()
    
    # Run simulation with profiling
    print("Running simulation with performance profiling...")
    profile_result = nlm.profile_brain(brain, 100)
    
    print(f"Performance results:")
    print(f"  Total steps: {profile_result['total_steps']}")
    print(f"  Elapsed time: {profile_result['elapsed_time_ms']:.2f} ms")
    print(f"  Steps per second: {profile_result['steps_per_second']:.2f}")
    print(f"  Average time per step: {profile_result['avg_time_per_step_ms']:.4f} ms")
    
    # Get brain statistics
    stats = brain.get_statistics()
    print(f"\nBrain statistics:")
    print(f"  Excitatory/Inhibitory ratio: {stats['e_i_ratio']:.4f}")
    print(f"  Active neurons: {stats['active_neurons']}")
    print(f"  Firing neurons: {stats['firing_neurons']}")
    
    # Show developmental stage
    developmental_stage = brain.getDevelopmentalStage()
    print(f"  Developmental stage: {developmental_stage}")
    
    return profile_result
def main():
    """Main function."""
    print_header()
    
    # Check if NLM bindings are available
    if not NLM_AVAILABLE:
        print("Error: NLM Python bindings not available.")
        sys.exit(1)
    
    try:
        # Create basic brain
        brain, config = create_basic_brain()
        
        # Explore configuration
        config = explore_configuration(config)
        
        # Create world and run simulation
        brain, world, simulation_result = create_world_and_simulation(brain, config)
        
        # Demonstrate action creation
        demonstrate_actions()
        
        # Demonstrate sensory processing
        demonstrate_sensory_processing()
        
        # Demonstrate agent simulation
        demonstrate_agent_simulation()
        
        # Demonstrate advanced features
        demonstrate_advanced_features()
        
        # Print summary
        print("\n" + "=" * 70)
        print("Quick Start Example Completed Successfully!")
        print("=" * 70)
        print("\nKey improvements demonstrated:")
        print("✓ New Pythonic APIs for configuration management")
        print("✓ Convenience methods for action manipulation")
        print("✓ Enhanced sensory data processing")
        print("✓ Advanced agent-based simulation")
        print("✓ Performance profiling tools")
        print("✓ Comprehensive error handling")
        print("\nThe NLM Python bindings now provide:")
        print("  - Easier configuration management with typed getters/setters")
        print("  - More intuitive action creation and manipulation")
        print("  - Enhanced sensory data handling with dictionary conversion")
        print("  - Advanced simulation utilities and profiling")
        print("  - Better integration between brain, world, and agents")
        print("\nFor more information, visit:")
        print("  - Documentation: docs/PYTHON_USER_GUIDE.md")
        print("  - Examples: examples/README.md")
        print("  - GitHub: https://github.com/nlm-project/nlm")
        
    except Exception as e:
        print(f"\nError during example execution: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)
if __name__ == "__main__":
    main()
