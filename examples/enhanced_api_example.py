"""
Example usage of the enhanced NLM Python bindings.

This file demonstrates the Pythonic API improvements and new convenience features
for working with neural network simulations.
"""

# Basic imports
import pynlm
import json
import time

def basic_usage_example():
    """
    Example 1: Basic usage with new Pythonic API.
    
    This demonstrates how the new bindings provide a more Pythonic interface
    with context managers, properties, and convenience methods.
    """
    print("=== Basic Usage Example ===")
    
    # Create a brain using the convenience factory function
    brain = pynlm.create_brain(pynlm.create_default_config())
    print(f"Created brain: {brain}")
    
    # Use brain as a context manager for safe resource management
    with pynlm.brain_context(brain) as ctx:
        if ctx:
            print("Brain initialized successfully")
            
            # Perform a simulation step
            brain.step(1, time.time())
            
            # Access brain statistics via properties
            print(f"Total neurons: {brain.total_neuron_count}")
            print(f"Total synapses: {brain.total_synapse_count}")
            print(f"Average firing rate: {brain.average_firing_rate:.2f}")
        else:
            print("Failed to initialize brain")

def quick_start_example():
    """
    Example 2: Using the QuickStart wizard for common use cases.
    
    The QuickStart wizard creates a complete setup with brain, world,
    and agent brain ready for experimentation.
    """
    print("\n=== QuickStart Example ===")
    
    # Quick start with default parameters
    runner = pynlm.quick_start(width=30, height=30, vision_width=7, vision_height=7)
    print(f"Created experiment runner: {runner}")
    
    # Run a quick learning episode
    print("Running learning episode...")
    runner.run_episode(50, timestep=0.1)
    
    # Get statistics
    stats = runner.get_stats()
    print(f"Simulation stats: {json.dumps(stats, indent=2)}")

def advanced_api_example():
    """
    Example 3: Advanced usage with Pythonic wrapper classes.
    
    This demonstrates the _ConfigWrapper and _BrainWrapper classes that
    provide enhanced Pythonic interfaces.
    """
    print("\n=== Advanced API Example ===")
    
    # Create a Pythonic config wrapper
    config = pynlm.Config()
    config.set("network_size", 1000)
    config.set("learning_rate", 0.01)
    config.set("vision_enabled", True)
    
    print(f"Config: {config.get_summary()}")
    
    # Load additional configuration from file
    try:
        config.load_from_file("experiment_config.json")
        print("Loaded config from file")
    except Exception as e:
        print(f"Could not load config file: {e}")
    
    # Get values with defaults
    network_size = config.get("network_size", 500)
    print(f"Network size (with fallback): {network_size}")
    
    # Create brain with enhanced wrapper
    brain_wrapper = pynlm.Brain(pynlm.create_brain(config.get_config()))
    with brain_wrapper:
        brain_wrapper.step(1)
        print(f"Brain initialized with {brain_wrapper.total_neuron_count} neurons")

def experiment_runner_example():
    """
    Example 4: Using ExperimentRunner for automated testing.
    
    The ExperimentRunner class provides high-level APIs for running
    controlled experiments and collecting metrics.
    """
    print("\n=== Experiment Runner Example ===")
    
    # Create experiment runner with custom setup
    runner = pynlm.ExperimentRunner(pynlm.create_brain(pynlm.create_default_config()))
    
    # Setup the experiment
    runner.setup_quick(width=50, height=50, vision_width=10, vision_height=10)
    
    # Run multiple steps with data collection
    print("Running experiment steps...")
    for step in range(10):
        time_step = runner.run_step(step, timestep=0.05)
        if step % 5 == 0:
            stats = runner.get_stats()
            print(f"Step {step}: {stats['active_neurons']} active neurons")
    
    print("Experiment completed successfully!")

def serialization_example():
    """
    Example 5: Serialization utilities for saving and loading experiments.
    
    The bindings provide easy save/load functionality for both
    configuration and brain state.
    """
    print("\n=== Serialization Example ===")
    
    # Create and configure a brain
    config = pynlm.Config()
    config.set("description", "Serialization test")
    
    brain = pynlm.create_brain(config)
    with pynlm.brain_context(brain):
        brain.step(1)
        
        # Save brain state
        brain.save("brain_checkpoint.pkl")
        print("Brain state saved to 'brain_checkpoint.pkl'")
        
        # Save config
        brain_config = brain.get_config()
        # Create a Pythonic wrapper for the config
        from pynlm import Config as ConfigWrapper
        config_wrapper = ConfigWrapper(brain_config)
        config_wrapper.save_to_file("config_saved.json")
        print("Config saved to 'config_saved.json'")
    
    # Load the saved brain
    loaded_brain = pynlm.create_brain(pynlm.create_default_config())
    if loaded_brain.load("brain_checkpoint.pkl"):
        print("Brain state loaded successfully")
    
    print("Serialization example completed")

def error_handling_example():
    """
    Example 6: Improved error handling and recovery.
    
    The enhanced bindings provide better error messages and recovery
    suggestions to help users diagnose issues.
    """
    print("\n=== Error Handling Example ===")
    
    try:
        # Try to create a brain with invalid configuration
        config = pynlm.Config()
        config.load_from_file("nonexistent_file.json")
        print("This should not print - file doesn't exist")
    except pynlm.ConfigError as e:
        print(f"Caught ConfigError: {e}")
        print("Suggested recovery: Check if the file exists and is accessible")
    
    try:
        # Try to use null brain
        null_brain = None
        if null_brain:
            null_brain.step(1)
    except Exception as e:
        print(f"Caught exception with helpful message: {e}")

def performance_tips_example():
    """
    Example 7: Performance tips and optimizations.
    
    This demonstrates ways to improve performance when using the bindings.
    """
    print("\n=== Performance Tips Example ===")
    
    # Tip 1: Reuse configurations instead of recreating them
    default_config = pynlm.create_default_config()
    
    brains = []
    for i in range(5):
        # Reuse the same configuration
        brain = pynlm.create_brain(default_config)
        brains.append(brain)
        print(f"Created brain {i+1} using reused config")
    
    # Tip 2: Use context managers for automatic cleanup
    print("Using context managers for safe resource management...")
    for i, brain in enumerate(brains):
        with pynlm.brain_context(brain):
            brain.step(i+1)
    
    print("All brains processed efficiently")

def main():
    """
    Main function demonstrating all the enhanced features.
    """
    print("NLM Python Bindings - Enhanced Examples")
    print("=" * 50)
    
    # Run all examples
    basic_usage_example()
    quick_start_example()
    advanced_api_example()
    experiment_runner_example()
    serialization_example()
    error_handling_example()
    performance_tips_example()
    
    print("\n" + "=" * 50)
    print("All examples completed successfully!")
    print("\nKey improvements demonstrated:")
    print("• Pythonic API with snake_case methods")
    print("• Context managers for resource management")
    print("• Properties for easy access to statistics")
    print("• QuickStart wizard for common use cases")
    print("• Enhanced error handling with helpful messages")
    print("• Serialization utilities")
    print("• Performance optimization tips")

if __name__ == "__main__":
    main()
