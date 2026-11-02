# NLM Python Bindings Example

# This example demonstrates the enhanced Python bindings for the NLM framework.
# It showcases the new Python-specific convenience methods and error handling.

import pynlm
import json
import time

def basic_config_example():
    """Example: Basic configuration with Python convenience methods."""
    print("=== Basic Configuration Example ===")
    
    # Create default configuration using Python convenience method
    config = pynlm.createDefaultConfig()
    print(f"Created default config: {config}")
    
    # Update configuration from Python dictionary
    config_data = {
        "int_value": 42,
        "double_value": 3.14,
        "string_value": "hello",
        "bool_value": True,
        "list_values": [1, 2, 3, 4, 5]
    }
    
    config.updateFromDict(config_data)
    print("Updated config from dictionary:")
    print(f"  Has int_value: {config.has('int_value')}")
    print(f"  Has string_value: {config.has('string_value')}")
    
    # Convert to Python dictionary for inspection
    config_dict = config.toDict()
    print(f"Config as dict: {config_dict}")
    
    # Validate configuration
    try:
        config.validate()
        print("Configuration validation passed")
    except Exception as e:
        print(f"Configuration validation failed: {e}")
    
    # Show configuration schema
    schema = config.getSchema()
    print("Configuration schema:")
    for key, description in schema.items():
        print(f"  {key}: {description}")
    
    print()

def brain_convenience_example():
    """Example: Using brain convenience methods."""
    print("=== Brain Convenience Methods Example ===")
    
    # Create configuration and brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Create world for agent brain
    world = pynlm.createSimpleWorld()
    agent_brain = pynlm.createAgentBrain(brain)
    
    # Initialize the system
    brain.initialize()
    agent_brain.initialize(world)
    
    # Set up callbacks for episode monitoring
    def pre_step_callback(step):
        print(f"    Pre-step {step}")
    
    def post_step_callback(step, stats):
        print(f"    Post-step {step}: {stats['active_neurons']} active neurons")
    
    # Enable callbacks
    brain.enableCallbacks(pre_step_callback, post_step_callback)
    
    # Run simulation episode with callback
    print("Running simulation episode...")
    steps_completed = brain.runEpisode(10, post_step_callback)
    print(f"Completed {steps_completed} steps")
    
    # Get brain statistics
    stats = brain.getEpisodeStats()
    print("Brain statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Get developmental stage information
    dev_stage = brain.getDevelopmentStage()
    print(f"Developmental stage: {dev_stage}")
    
    # Set learned preference (personality)
    brain.setLearnedPreference(0.75)
    preference = brain.getLearnedPreference()
    print(f"Learned preference: {preference}")
    
    # Reset for new episode
    brain.resetForNewEpisode()
    print("Reset brain for new episode")
    
    print()

def error_handling_example():
    """Example: Enhanced error handling."""
    print("=== Enhanced Error Handling Example ===")
    
    config = pynlm.createDefaultConfig()
    
    # Demonstrate validation error
    print("Testing configuration validation...")
    try:
        config.validate()
        print("  Validation passed")
    except pynlm.ValidationError as e:
        print(f"  Validation error: {e}")
    except Exception as e:
        print(f"  Unexpected error: {e}")
    
    # Demonstrate file import error
    print("Testing file import error...")
    try:
        brain = pynlm.createBrain(config)
        # Try to load a non-existent file
        success = brain.loadBrain("/nonexistent/file.bin")
        print(f"  Load result: {success}")
    except pynlm.ImportError as e:
        print(f"  Import error: {e}")
    except Exception as e:
        print(f"  Unexpected error: {e}")
    
    print()

def development_example():
    """Example: Development system management."""
    print("=== Development System Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Show current developmental stage
    dev_stage = brain.getDevelopmentStage()
    print(f"Current developmental stage: {dev_stage}")
    
    # Enable development
    brain.enableDevelopment(True)
    print("Development system enabled")
    
    # Set developmental stage
    brain.setDevelopmentalStage(pynlm.DevelopmentalStage.Maturation)
    dev_stage = brain.getDevelopmentStage()
    print(f"Set developmental stage to: {dev_stage}")
    
    # Run a few steps to see development effects
    print("Running simulation to observe development...")
    for i in range(5):
        brain.step(i)
    
    # Check developmental stage after simulation
    dev_stage = brain.getDevelopmentStage()
    print(f"Developmental stage after simulation: {dev_stage}")
    
    print()

def performance_example():
    """Example: Performance monitoring."""
    print("=== Performance Monitoring Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Enable performance monitoring
    brain.enablePerformanceMonitoring()
    
    # Run simulation and collect performance data
    print("Running performance test...")
    start_time = time.time()
    
    for i in range(20):
        brain.step(i)
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    # Get performance statistics
    perf_stats = brain.getPerformanceStats()
    print("Performance statistics:")
    for key, value in perf_stats.items():
        print(f"  {key}: {value}")
    
    print(f"Simulation time: {elapsed:.2f} seconds")
    print(f"Average time per step: {elapsed/20:.4f} seconds")
    
    print()

def serialization_example():
    """Example: Enhanced serialization."""
    print("=== Serialization Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run some simulation steps
    for i in range(10):
        brain.step(i)
    
    # Save brain state
    print("Saving brain state...")
    save_file = "brain_checkpoint.pkl"
    success = brain.saveBrain(save_file)
    if success:
        print(f"  Successfully saved to {save_file}")
        
        # Load brain state back
        print("Loading brain state...")
        load_success = brain.loadBrain(save_file)
        if load_success:
            print("  Successfully loaded from file")
            
            # Verify state by checking stats
            stats = brain.getEpisodeStats()
            print(f"  Total neurons: {stats['total_neurons']}")
            print(f"  Total spikes: {stats['total_spikes']}")
        else:
            print("  Failed to load from file")
    else:
        print("  Failed to save to file")
    
    print()

def visualization_example():
    """Example: Visualization integration."""
    print("=== Visualization Integration Example ===")
    
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.enableVisualization()
    
    # Run simulation steps
    for i in range(15):
        brain.step(i)
    
    # Export data for visualization
    print("Exporting data for visualization...")
    viz_data = brain.exportForVisualization()
    
    print("Visualization data structure:")
    print(f"  Has brain_state: {'brain_state' in viz_data}")
    print(f"  Has timestamp: {'timestamp' in viz_data}")
    
    # Show brain state data
    if 'brain_state' in viz_data:
        state = viz_data['brain_state']
        print(f"  Neurons in state: {state.get('total_neurons', 'N/A')}")
        print(f"  Synapses in state: {state.get('total_synapses', 'N/A')}")
    
    print()

def main():
    """Run all examples."""
    print("NLM Python Bindings Examples")
    print("=" * 50)
    
    basic_config_example()
    brain_convenience_example()
    error_handling_example()
    development_example()
    performance_example()
    serialization_example()
    visualization_example()
    
    print("=" * 50)
    print("All examples completed!")

if __name__ == "__main__":
    main()