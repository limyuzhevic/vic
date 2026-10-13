#!/usr/bin/env python3
"""
Educational Example Scripts for NLM (Neural Learning Machine)

This module provides comprehensive educational examples demonstrating how to use
the NLM Python bindings. Each example is designed to teach specific concepts
and showcase different features of the library.
"""

import sys
import os
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..'))

try:
    import pynlm as nlm
    from pynlm import createDefaultConfig, createLearningConfig, createFastConfig
    from pynlm import createBrain, createWorld, createAgentBrain
    from pynlm import ExperimentSession
except ImportError as e:
    print(f"Error importing pynlm: {e}")
    print("Please ensure pynlm is properly built and installed.")
    sys.exit(1)

print("=== NLM Educational Example Scripts ===")
print("This module provides comprehensive examples for learning NLM.\n")

# Example 1: Basic NLM Usage
def example_1_basic_usage():
    """
    Example 1: Basic NLM Usage
    
    This example demonstrates the fundamental steps for creating and using an NLM brain:
    1. Creating a configuration
    2. Initializing a brain
    3. Running a simulation
    4. Observing results
    """
    print("Example 1: Basic NLM Usage")
    print("-" * 40)
    
    # Create a basic configuration
    config = createDefaultConfig()
    print(f"Created default configuration with {config.get('neuron_count')} neurons")
    
    # Create a brain
    brain = createBrain(config)
    print("Brain created successfully")
    
    # Initialize the brain
    success = brain.initialize()
    print(f"Brain initialization: {'Success' if success else 'Failed'}")
    
    # Run a simple simulation
    for step in range(100):
        brain.step(step)
    
    # Print statistics
    print(f"\nSimulation Results:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Excitation/Inhibition ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    print("\n" + "=" * 60 + "\n")

# Example 2: Configuration Presets
def example_2_configuration_presets():
    """
    Example 2: Configuration Presets
    
    This example demonstrates the different configuration presets available
    in NLM and how to use them for different simulation scenarios.
    """
    print("Example 2: Configuration Presets")
    print("-" * 40)
    
    presets = ["basic", "learning", "fast", "research", "demo"]
    
    for preset in presets:
        print(f"\n{preset.upper()} Preset:")
        print(f"  Description: {nlm.get_preset_description(preset)}")
        
        # Get preset values
        preset_values = nlm.get_preset_values(preset)
        for key, value in preset_values.items():
            print(f"    {key}: {value}")
    
    print("\n" + "=" * 60 + "\n")

# Example 3: Learning Experiment
def example_3_learning_experiment():
    """
    Example 3: Learning Experiment
    
    This example demonstrates how to set up and run a learning experiment
    using NLM's plasticity mechanisms.
    """
    print("Example 3: Learning Experiment")
    print("-" * 40)
    
    # Create a learning-focused configuration
    config = createLearningConfig()
    brain = createBrain(config)
    
    # Initialize brain
    brain.initialize()
    
    # Run learning experiment using context manager
    with ExperimentSession(brain, steps=1000) as session:
        # The session automatically enables plasticity
        # and runs the experiment with stimulus injection
        pass
    
    # Get experiment metrics
    metrics = session.get_metrics()
    print(f"Learning Experiment Results:")
    print(f"  Weight change: {metrics['weight_change']:.4f}")
    print(f"  Strengthened synapses: {metrics['strengthened']}")
    print(f"  Weakened synapses: {metrics['weakened']}")
    print(f"  Unchanged synapses: {metrics['unchanged']}")
    print(f"  Learning detected: {metrics['learning_detected']}")
    
    print("\n" + "=" * 60 + "\n")

# Example 4: Visualization and Data Export
def example_4_visualization_and_data_export():
    """
    Example 4: Visualization and Data Export
    
    This example demonstrates how to export simulation data for visualization
    and analysis using NLM's data export capabilities.
    """
    print("Example 4: Visualization and Data Export")
    print("-" * 40)
    
    # Create configuration and brain
    config = createDefaultConfig()
    brain = createBrain(config)
    
    # Initialize and run simulation
    brain.initialize()
    
    for step in range(500):
        brain.step(step)
    
    # Export network statistics
    print("Exporting network statistics...")
    success = nlm.export_network_data(brain, "network_stats.txt")
    print(f"  Network data exported: {'Success' if success else 'Failed'}")
    
    # Export spike data
    print("Exporting spike data...")
    success = nlm.export_spike_data(brain, "spike_data.txt")
    print(f"  Spike data exported: {'Success' if success else 'Failed'}")
    
    # Export activity data
    print("Exporting activity data...")
    success = nlm.export_activity_data(brain, "activity_data.txt")
    print(f"  Activity data exported: {'Success' if success else 'Failed'}")
    
    # Export weight data
    print("Exporting weight data...")
    success = nlm.export_weight_data(brain, "weight_data.txt")
    print(f"  Weight data exported: {'Success' if success else 'Failed'}")
    
    # Create comprehensive visualization
    print("Creating comprehensive visualization...")
    success = nlm.create_comprehensive_visualization(brain, "visualization_")
    print(f"  Visualization created: {'Success' if success else 'Failed'}")
    
    print("\n" + "=" * 60 + "\n")

# Example 5: Agent-Based Simulation
def example_5_agent_based_simulation():
    """
    Example 5: Agent-Based Simulation
    
    This example demonstrates how to create an agent-based simulation using
    NLM's agent brain interface and world system.
    """
    print("Example 5: Agent-Based Simulation")
    print("-" * 40)
    
    # Create brain and world
    config = createFastConfig()
    brain = createBrain(config)
    world = createWorld(width=100, height=100, visionWidth=7, visionHeight=7)
    
    # Create agent brain
    agent_brain = createAgentBrain(brain, world)
    print("Agent brain created successfully")
    
    # Initialize world
    world.setAgentStart(50, 50)
    print("Agent positioned at center of world")
    
    # Run simulation loop
    for step in range(200):
        # Get sensory percept
        percept = world.getSensoryPercept()
        
        # Process sensory input
        agent_brain.processSensoryInput(percept)
        
        # Get motor command
        action = agent_brain.decodeMotorCommand()
        
        # Apply action in world
        result = world.applyMotorCommand(action, step * 0.001)
        
        # Log result
        if result.success:
            print(f"  Step {step}: Action '{action.getName()}' successful (reward: {result.reward})")
        else:
            print(f"  Step {step}: Action '{action.getName()}' failed")
        
        # Update world
        world.update(0.001)
    
    # Print final statistics
    brain.logStatus()
    print(f"\nAgent simulation completed in {step + 1} steps")
    
    print("\n" + "=" * 60 + "\n")

# Example 6: Advanced Configuration and Validation
def example_6_advanced_configuration():
    """
    Example 6: Advanced Configuration and Validation
    
    This example demonstrates advanced configuration techniques, including
    validation, preset application, and configuration comparison.
    """
    print("Example 6: Advanced Configuration and Validation")
    print("-" * 40)
    
    # Create configuration from dictionary
    dict_config = {
        "neuron_count": "200",
        "region_count": "3",
        "simulation_timestep": "0.001",
        "connection_probability": "0.2",
        "stdp_ltp_weight": "0.03",
        "stdp_ltd_weight": "0.025"
    }
    
    config = nlm.dict_to_config(dict_config)
    print(f"Created configuration from dictionary: {config.get('neuron_count')} neurons")
    
    # Apply preset
    config.apply_preset("learning")
    print("Applied 'learning' preset")
    
    # Validate configuration
    errors = nlm.validate_config(config)
    if errors:
        print("Configuration validation errors:")
        for error in errors:
            print(f"  - {error}")
    else:
        print("Configuration validation: PASSED")
    
    # Get configuration statistics
    stats = nlm.get_config_stats(config)
    print(f"\nConfiguration Statistics:")
    print(f"  Total entries: {stats.total_entries}")
    print(f"  Validation score: {stats.validation_score:.2f}")
    print(f"  Available presets: {len(stats.available_presets)}")
    
    print("\n" + "=" * 60 + "\n")

# Example 7: Multiple Scenario Comparison
def example_7_multiple_scenario_comparison():
    """
    Example 7: Multiple Scenario Comparison
    
    This example demonstrates how to compare different simulation scenarios
    using the same configuration with different parameters.
    """
    print("Example 7: Multiple Scenario Comparison")
    print("-" * 40)
    
    scenarios = ["basic", "learning", "fast", "research"]
    brains = []
    
    for scenario in scenarios:
        print(f"\nRunning '{scenario}' scenario...")
        config = nlm.get_config(scenario)
        brain = createBrain(config)
        brain.initialize()
        
        # Run simulation
        for step in range(100):
            brain.step(step)
        
        brains.append(brain)
        
        # Record statistics
        stats = nlm.get_network_stats(brain)
        print(f"  Neurons: {stats.neuron_count}, Spikes: {stats.total_spikes}")
    
    # Compare results
    print("\n" + "=" * 60)
    print("Scenario Comparison:")
    print("=" * 60)
    
    for i, scenario in enumerate(scenarios):
        print(f"\n{scenario.upper()}:")
        print(f"  Neurons: {brains[i].getTotalNeuronCount()}")
        print(f"  Synapses: {brains[i].getTotalSynapseCount()}")
        print(f"  Total Spikes: {brains[i].getTotalSpikeCount()}")
        print(f"  Firing Rate: {brains[i].getAverageFiringRate():.2f} Hz")
    
    print("\n" + "=" * 60 + "\n")

# Example 8: Performance and Scalability Testing
def example_8_performance_and_scalability():
    """
    Example 8: Performance and Scalability Testing
    
    This example demonstrates how to test NLM performance with different
    configuration sizes and measure the impact on simulation speed.
    """
    print("Example 8: Performance and Scalability Testing")
    print("-" * 40)
    
    configurations = [
        ("Small", 50),
        ("Medium", 200),
        ("Large", 500)
    ]
    
    print("\nPerformance Testing:")
    print("-" * 40)
    print(f"{'Size':<10} {'Neurons':<10} {'Time (s)':<10} {'Spikes/s':<10}")
    print("-" * 40)
    
    for name, neuron_count in configurations:
        import time
        
        config = createFastConfig(neuron_count)
        brain = createBrain(config)
        
        start_time = time.time()
        brain.initialize()
        
        for step in range(50):
            brain.step(step)
        
        end_time = time.time()
        duration = end_time - start_time
        
        spikes_per_sec = brain.getTotalSpikeCount() / duration
        
        print(f"{name:<10} {neuron_count:<10} {duration:<10.3f} {spikes_per_sec:<10.1f}")
    
    print("\n" + "=" * 60 + "\n")

def run_all_examples():
    """
    Run all educational examples
    """
    examples = [
        example_1_basic_usage,
        example_2_configuration_presets,
        example_3_learning_experiment,
        example_4_visualization_and_data_export,
        example_5_agent_based_simulation,
        example_6_advanced_configuration,
        example_7_multiple_scenario_comparison,
        example_8_performance_and_scalability
    ]
    
    for i, example in enumerate(examples, 1):
        try:
            example()
        except Exception as e:
            print(f"Error in example {i}: {e}")
            import traceback
            traceback.print_exc()
            print("\n" + "=" * 60 + "\n")

if __name__ == "__main__":
    print("NLM Educational Examples")
    print("=======================\n")
    
    if len(sys.argv) > 1:
        # Run specific example
        example_num = int(sys.argv[1])
        examples = [
            example_1_basic_usage,
            example_2_configuration_presets,
            example_3_learning_experiment,
            example_4_visualization_and_data_export,
            example_5_agent_based_simulation,
            example_6_advanced_configuration,
            example_7_multiple_scenario_comparison,
            example_8_performance_and_scalability
        ]
        
        if 1 <= example_num <= len(examples):
            examples[example_num - 1]()
        else:
            print(f"Invalid example number: {example_num}")
            print(f"Please choose a number between 1 and {len(examples)}")
    else:
        # Run all examples
        run_all_examples()
    
    print("\n" + "=" * 60)
    print("All examples completed!")
    print("For more information, refer to the NLM documentation.")
    print("=" * 60)