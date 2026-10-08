#!/usr/bin/env python3
"""
NLM Basic Usage Examples

These examples demonstrate the enhanced NLM Python interface for common
tasks and simulation scenarios.
"""

from nlm import (
    BrainSimulation,
    NLMConfigManager,
    quick_demo,
    run_phase4_demo,
    create_default_config,
    create_small_config,
    create_medium_config,
    create_large_config,
)


def example_basic_usage():
    """Example 1: Basic brain simulation"""
    print("=== Basic Usage Example ===")
    
    # Create a brain simulation with custom configuration
    config = create_medium_config()
    simulation = BrainSimulation(config)
    
    # Initialize the brain
    print("Initializing brain...")
    simulation.initialize()
    
    # Run a quick simulation
    print("Running simulation...")
    results = simulation.run_simulation(100)
    
    # Display results
    print(f"Simulation completed!")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Active neurons: {results['active_neurons']}")
    print(f"Average firing rate: {results['average_firing_rate']:.3f} Hz")
    
    # Save the brain state
    simulation.save_simulation("basic_example.checkpoint")
    print("Brain state saved to basic_example.checkpoint")
    
    print()


def example_configuration_management():
    """Example 2: Configuration management"""
    print("=== Configuration Management Example ===")
    
    # Create config manager
    config_manager = NLMConfigManager()
    
    # Create a custom configuration
    custom_config = {
        "simulation_timestep": 0.001,
        "neuron_count": 500,
        "region_count": 1,
        "connection_probability": 0.15,
        "random_seed": 123,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.018,
    }
    
    print("Creating custom configuration...")
    config_file = config_manager.create_config("custom_medium", custom_config)
    print(f"Configuration saved to: {config_file}")
    
    # Load and display the configuration
    print("\nLoading and displaying configuration...")
    config_manager.show_config("custom_medium")
    
    # Validate configuration
    print("\nValidating configuration...")
    is_valid = config_manager.validate_config(custom_config)
    print(f"Configuration valid: {is_valid}")
    
    # Export configuration in different formats
    print("\nExporting configuration in different formats...")
    json_format = config_manager.export_config("custom_medium", "json")
    print(f"JSON format (first 200 chars): {json_format[:200]}...")
    
    print()


def example_quick_demo():
    """Example 3: Quick demo function"""
    print("=== Quick Demo Example ===")
    
    print("Running quick demonstration with minimal configuration...")
    results = quick_demo(neurons=200, steps=100)
    
    print(f"Quick demo completed!")
    print(f"Total steps: {results['total_steps']}")
    print(f"Total spikes: {results['total_spikes']}")
    print(f"Average firing rate: {results['average_firing_rate']:.3f} Hz")
    
    print()


def example_simulation_with_input():
    """Example 4: Simulation with sensory input"""
    print("=== Simulation with Sensory Input Example ===")
    
    simulation = BrainSimulation()
    simulation.initialize()
    
    print("Simulating with visual input...")
    
    # Create visual-like input (simulating a simple visual stimulus)
    visual_input = [0.1, 0.5, 0.9, 0.7, 0.3] * 20  # Repeating pattern
    
    # Run a few steps with sensory input
    for step in range(50):
        if step % 10 == 0:
            print(f"Step {step}: Injecting visual input")
            simulation.inject_sensory_input(visual_input, "vision")
        
        simulation.run_simulation(10)
        state = simulation.get_brain_state()
        print(f"  Step {step}: Active neurons = {state['active_neurons']}")
    
    print()


def example_configuration_comparison():
    """Example 5: Comparing different configuration sizes"""
    print("=== Configuration Comparison Example ===")
    
    config_sizes = {
        "Small (quick test)": create_small_config(),
        "Medium (standard)": create_medium_config(),
        "Large (research)": create_large_config(),
    }
    
    for name, config in config_sizes.items():
        print(f"\n{name}:")
        print(f"  Neurons: {config['neuron_count']}")
        print(f"  Regions: {config['region_count']}")
        print(f"  Connection probability: {config['connection_probability']}")
        print(f"  Simulation timestep: {config['simulation_timestep']}s")
        
        if 'stdp_ltp_weight' in config:
            print(f"  STDP LTP weight: {config['stdp_ltp_weight']}")
        if 'synaptogenesis_rate' in config:
            print(f"  Synaptogenesis rate: {config['synaptogenesis_rate']}")


def example_error_handling():
    """Example 6: Error handling"""
    print("=== Error Handling Example ===")
    
    try:
        # Create invalid configuration
        invalid_config = {
            "neuron_count": -100,  # Invalid: negative
            "region_count": 0,     # Invalid: zero
            "connection_probability": 1.5,  # Invalid: > 1.0
        }
        
        config_manager = NLMConfigManager()
        
        print("Attempting to validate invalid configuration...")
        is_valid = config_manager.validate_config(invalid_config)
        print(f"Validation result: {is_valid}")
        
        # This should raise an error when trying to load
        print("\nAttempting to load non-existent configuration...")
        config_manager.load_config("nonexistent")
        
    except FileNotFoundError as e:
        print(f"File not found error (expected): {e}")
    except Exception as e:
        print(f"Other error: {e}")
    
    print()


def example_save_load():
    """Example 7: Save and load simulation state"""
    print("=== Save and Load Example ===")
    
    # Create and run a simulation
    simulation = BrainSimulation(create_small_config())
    simulation.initialize()
    
    print("Running initial simulation...")
    simulation.run_simulation(50)
    
    # Save the simulation state
    simulation.save_simulation("example_save.checkpoint")
    print("Simulation state saved to example_save.checkpoint")
    
    # Create a new simulation and load the saved state
    print("\nCreating new simulation and loading saved state...")
    new_simulation = BrainSimulation(create_small_config())
    new_simulation.initialize()
    
    if new_simulation.load_simulation("example_save.checkpoint"):
        print("Simulation state loaded successfully")
        print(f"Loaded step: {new_simulation.current_step}")
    else:
        print("Failed to load simulation state")
    
    print()


if __name__ == "__main__":
    """
    Run all examples in sequence.
    
    This script demonstrates the enhanced NLM interface features.
    Uncomment any examples you'd like to run.
    """
    
    print("NLM Python Interface Examples")
    print("=" * 50)
    print()
    
    # Run examples (uncomment to run specific examples)
    example_basic_usage()
    example_configuration_management()
    example_quick_demo()
    example_simulation_with_input()
    example_configuration_comparison()
    example_error_handling()
    example_save_load()
    
    print("All examples completed!")
    print("\nFor more complex examples, see the examples/ directory for scripts that demonstrate")
    print("specific use cases like experiment management, visualization, and advanced features.")
