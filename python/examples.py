"""
Example usage of the improved NLM Python API.

This script demonstrates the new convenience functions and improved API
designed to make it easy for beginners while maintaining full control for advanced users.
"""

import pynlm

# ============================================================================
# EXAMPLE 1: Basic Usage - Easy for Beginners
# ============================================================================

def example_basic_usage():
    """
    Simple example showing how to get started with NLM.
    This is much simpler than the manual setup shown in HOW_TO_USE.md.
    """
    print("=== Example 1: Basic Usage ===")
    
    # Create a brain with default settings (sensible defaults)
    brain = pynlm.create_default_brain()
    
    # Run a simulation
    brain = pynlm.run_complete_simulation(
        steps=100,
        enable_learning=True
    )
    
    # Get statistics
    stats = pynlm.collect_brain_stats(brain)
    print(f"Simulation completed: {stats['neuron_count']} neurons, "
          f"{stats['firing_neurons']} firing neurons")
    
    print()

# ============================================================================
# EXAMPLE 2: Advanced Configuration
# ============================================================================

def example_advanced_configuration():
    """
    Example showing advanced configuration options.
    """
    print("=== Example 2: Advanced Configuration ===")
    
    # Create brain with custom configuration
    brain = pynlm.create_brain_with_config(
        neuron_count=500,
        synapse_density=0.05,
        plasticity_enable=True,
        neuromod_enable=True,
        learning_rate=0.001
    )
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
    print()

# ============================================================================
# EXAMPLE 3: Agent with World Integration
# ============================================================================

def example_agent_with_world():
    """
    Example showing complete agent setup with world integration.
    """
    print("=== Example 3: Agent with World Integration ===")
    
    # Create complete agent setup in one call
    brain, agent, world = pynlm.create_simple_agent_setup()
    
    # Run a training episode
    training_helper = pynlm.SimpleTrainingHelper(brain, agent, world)
    training_helper.run_episode(max_steps=50, reward_threshold=1.0)
    
    # Get performance metrics
    metrics = training_helper.get_performance_metrics()
    print(f"Training metrics: {metrics}")
    
    print()

# ============================================================================
# EXAMPLE 4: World Integration Test
# ============================================================================

def example_world_integration_test():
    """
    Example showing world integration test with custom configuration.
    """
    print("=== Example 4: World Integration Test ===")
    
    # Create a custom configuration
    config = pynlm.createDefaultConfig()
    config.set("world.width", 30.0f)
    config.set("world.height", 20.0f)
    
    # Run world test
    world, agent, brain = pynlm.run_world_test(config)
    
    print(f"World test completed: {world.getWidth()}x{world.getHeight()} world")
    print(f"Agent started at: ({world.getAgentBody().x}, {world.getAgentBody().y})")
    
    print()

# ============================================================================
# EXAMPLE 5: Progress Monitoring
# ============================================================================

def example_progress_monitoring():
    """
    Example showing progress monitoring during long simulations.
    """
    print("=== Example 5: Progress Monitoring ===")
    
    brain = pynlm.create_default_brain()
    
    # Run simulation with progress monitoring
    brain = pynlm.run_simulation_with_progress(
        brain=brain,
        steps=200,
        timestep=0.1
    )
    
    print("Simulation completed with progress monitoring!")
    print()

# ============================================================================
# EXAMPLE 6: Results Export
# ============================================================================

def example_results_export():
    """
    Example showing how to export simulation results.
    """
    print("=== Example 6: Results Export ===")
    
    brain = pynlm.create_default_brain()
    world = pynlm.createSimpleWorld()
    
    # Run a quick simulation
    for step in range(50):
        brain.step(step)
    
    # Export results
    brain = pynlm.export_simulation_results(
        brain=brain,
        world=world,
        filepath="simulation_results.txt",
        metadata={
            "model": "NLM-P6",
            "training_episodes": "1",
            "steps_per_episode": "50"
        }
    )
    
    print("Results exported to simulation_results.txt")
    print()

# ============================================================================
# MAIN FUNCTION - Run all examples
# ============================================================================

def main():
    """
    Run all example functions to demonstrate the improved API.
    """
    print("NLM Python API Examples")
    print("=" * 50)
    print()
    
    try:
        example_basic_usage()
        example_advanced_configuration()
        example_agent_with_world()
        example_world_integration_test()
        example_progress_monitoring()
        example_results_export()
        
        print("All examples completed successfully!")
        
    except Exception as e:
        print(f"Error running examples: {e}")
        import traceback
        traceback.print_exc()

if __name__ == "__main__":
    main()