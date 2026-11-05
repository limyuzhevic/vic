#!/usr/bin/env python3
"""
NLM - Example 1: Basic Usage

This example demonstrates the basic usage of NLM for spiking neural network
simulation. It's designed to be simple and easy to understand for beginners.
"""

import matplotlib.pyplot as plt
from nlm import EventType

# Setup logging to see what's happening
import logging
setup_logging(logging.INFO)

def basic_example():
    """
    Example 1: Basic NLM usage with minimal configuration.
    """
    print("=" * 60)
    print("Example 1: Basic NLM Usage")
    print("=" * 60)
    
    # Step 1: Create brain with minimal configuration
    print("\n1. Creating brain with minimal configuration...")
    config = get_minimal_config()
    brain = AdvancedNLM(config)
    
    print(f"   - Neuron count: {brain.brain.getTotalNeuronCount()}")
    print(f"   - Region count: {brain.brain.getRegionCount()}")
    print(f"   - Connection probability: {config.connection_probability}")
    
    # Step 2: Run basic simulation
    print("\n2. Running basic simulation...")
    results = brain.run_experiment("basic_test", 200)
    
    print(f"   - Steps completed: {results['steps_completed']}")
    print(f"   - Final firing rate: {results['final_brain_state']['firing_rate']:.2f} Hz")
    print(f"   - Total spikes: {results['final_brain_state']['total_spikes']}")
    print(f"   - Firing neurons: {results['final_brain_state']['firing_neurons']}")
    
    # Step 3: Visualize results
    print("\n3. Visualizing results...")
    plot_basic_results(brain, "basic_example_results.png")
    
    # Step 4: Demonstrate learning capabilities
    print("\n4. Demonstrating learning capabilities...")
    brain.brain.enableRewardModulation(True)
    brain.brain.enableStructuralPlasticity(True)
    
    learning_results = brain.run_experiment("learning_test", 500)
    
    print(f"   - Learning simulation completed")
    print(f"   - Final learning rate: {learning_results['plasticity_summary']['stdp_learning_rate']:.4f}")
    
    # Step 5: Show simulation status
    print("\n5. Final simulation status...")
    simulation = brain.create_simulation("final_status", max_steps=1000)
    progress = simulation.get_progress()
    
    print(f"   - Simulation status: {progress['status']}")
    print(f"   - Current step: {progress['current_step']}")
    print(f"   - Progress: {progress['progress']:.1f}%")
    
    print("\n" + "=" * 60)
    print("Example 1 completed successfully!")
    print("=" * 60)
    
    return brain, results

def plot_basic_results(brain, output_path):
    """
    Plot and save basic simulation results.
    
    Args:
        brain: AdvancedNLM instance
        output_path: Output file path
    """
    # Create simulation
    simulation = brain.create_simulation("basic_plot", max_steps=200)
    
    # Get metrics
    metrics = simulation.get_metrics_range(0, 199)
    
    # Extract data
    steps = [m.steps_completed for m in metrics]
    firing_rates = [m.average_firing_rate for m in metrics]
    spike_counts = [m.total_spikes for m in metrics]
    firing_neurons = [m.firing_neurons for m in metrics]
    e_i_ratios = [m.excitation_inhibition_ratio for m in metrics]
    
    # Create plots
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    fig.suptitle("NLM Basic Simulation Results", fontsize=16)
    
    # Plot 1: Firing rate over time
    axes[0, 0].plot(steps, firing_rates, 'b-', linewidth=2, label='Firing Rate')
    axes[0, 0].set_xlabel('Simulation Steps')
    axes[0, 0].set_ylabel('Firing Rate (Hz)')
    axes[0, 0].set_title('Firing Rate Over Time')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Spike count over time
    axes[0, 1].plot(steps, spike_counts, 'r-', linewidth=2, label='Total Spikes')
    axes[0, 1].set_xlabel('Simulation Steps')
    axes[0, 1].set_ylabel('Spike Count')
    axes[0, 1].set_title('Spike Count Over Time')
    axes[0, 1].legend()
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Firing neurons over time
    axes[1, 0].plot(steps, firing_neurons, 'g-', linewidth=2, label='Firing Neurons')
    axes[1, 0].set_xlabel('Simulation Steps')
    axes[1, 0].set_ylabel('Number of Neurons')
    axes[1, 0].set_title('Active Neurons Over Time')
    axes[1, 0].legend()
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: E/I ratio over time
    axes[1, 1].plot(steps, e_i_ratios, 'orange', linewidth=2, label='E/I Ratio')
    axes[1, 1].axhline(y=1.0, color='red', linestyle='--', alpha=0.5, label='Balance (E=I)')
    axes[1, 1].set_xlabel('Simulation Steps')
    axes[1, 1].set_ylabel('Excitatory/Inhibitory Ratio')
    axes[1, 1].set_title('Neural Excitation/Inhibition Balance')
    axes[1, 1].legend()
    axes[1, 1].grid(True, alpha=0.3)
    
    # Adjust layout
    plt.tight_layout()
    
    # Save plot
    plt.savefig(output_path, dpi=150, bbox_inches='tight')
    plt.close()
    
    print(f"   Results saved to: {output_path}")

def demonstrate_event_monitoring(brain):
    """
    Demonstrate event monitoring capabilities.
    
    Args:
        brain: AdvancedNLM instance
    """
    print("\n" + "=" * 60)
    print("Event Monitoring Demonstration")
    print("=" * 60)
    
    # Create simulation
    simulation = brain.create_simulation("event_demo", max_steps=100)
    
    # Define event callbacks
    def on_step_completed(event):
        if event.step % 25 == 0:  # Only print every 25 steps
            print(f"   Step {event.step}: Firing rate={event.data.get('firing_rate', 0):.2f}Hz, "
                  f"Spikes={event.data.get('spike_count', 0)}")
    
    def on_simulation_started(event):
        print(f"   Simulation started with {event.data['num_steps']} steps")
    
    def on_simulation_stopped(event):
        print(f"   Simulation stopped after {event.data['total_steps']} steps")
        print(f"   Duration: {event.data['elapsed_time']:.2f} seconds")
        print(f"   Speed: {event.data['steps_per_second']:.1f} steps/second")
    
    # Add event listeners
    simulation.add_event_listener(EventType.STEP_COMPLETED, on_step_completed)
    simulation.add_event_listener(EventType.SIMULATION_STARTED, on_simulation_started)
    simulation.add_event_listener(EventType.SIMULATION_STOPPED, on_simulation_stopped)
    
    # Run simulation with monitoring
    print("\n   Running simulation with event monitoring...")
    simulation.run(100)
    
    print("\n   Event monitoring demonstration completed!")

def demonstrate_learning_simulation(brain):
    """
    Demonstrate learning simulation capabilities.
    
    Args:
        brain: AdvancedNLM instance
    """
    print("\n" + "=" * 60)
    print("Learning Simulation Demonstration")
    print("=" * 60)
    
    # Configure brain for learning
    brain.brain.enableRewardModulation(True)
    brain.brain.enableStructuralPlasticity(True)
    brain.brain.enableDevelopment(True)
    brain.brain.enableCuriosity(True)
    
    print("\n   Configured brain with learning systems:")
    print(f"   - Reward modulation: {'Enabled' if brain.brain.isRewardModulationEnabled() else 'Disabled'}")
    print(f"   - Structural plasticity: {'Enabled' if brain.brain.isStructuralPlasticityEnabled() else 'Disabled'}")
    print(f"   - Development: {'Enabled' if brain.brain.isDevelopmentEnabled() else 'Disabled'}")
    print(f"   - Curiosity: {'Enabled' if brain.brain.isCuriosityEnabled() else 'Disabled'}")
    
    # Run learning simulation
    print("\n   Running learning simulation...")
    learning_results = brain.run_experiment("learning_demo", 300)
    
    # Analyze learning results
    print("\n   Learning analysis:")
    print(f"   - Total steps: {learning_results['steps_completed']}")
    print(f"   - Final firing rate: {learning_results['final_brain_state']['firing_rate']:.2f} Hz")
    print(f"   - Final spike count: {learning_results['final_brain_state']['total_spikes']}")
    print(f"   - Learning rate: {learning_results['plasticity_summary']['stdp_learning_rate']:.4f}")
    
    # Get plasticity summary
    plasticity = learning_results['plasticity_summary']
    print("\n   Plasticity systems:")
    for system, enabled in plasticity.items():
        status = 'Enabled' if enabled else 'Disabled'
        print(f"   - {system}: {status}")
    
    print("\n   Learning simulation demonstration completed!")

def demonstrate_configuration_management(brain):
    """
    Demonstrate configuration management capabilities.
    
    Args:
        brain: AdvancedNLM instance
    """
    print("\n" + "=" * 60)
    print("Configuration Management Demonstration")
    print("=" * 60)
    
    # Get current configuration
    config = brain.brain.getConfig()
    print(f"\n   Current configuration:")
    print(f"   - Neuron count: {config.neuron_count}")
    print(f"   - Region count: {config.region_count}")
    print(f"   - Connection probability: {config.connection_probability}")
    print(f"   - Simulation timestep: {config.simulation_timestep} s")
    
    # Create config manager
    from nlm import ConfigManager
    config_manager = ConfigManager(config)
    
    # Validate configuration
    is_valid = config_manager.validate()
    print(f"\n   Configuration validation: {'Valid' if is_valid else 'Invalid'}")
    
    if not is_valid:
        errors = config_manager.get_validation_errors()
        print(f"   Validation errors: {errors}")
    
    # Update configuration
    print("\n   Updating configuration...")
    config_manager.update_config(
        neuron_count=500,
        connection_probability=0.15
    )
    
    updated_config = config_manager.get_config()
    print(f"   - Updated neuron count: {updated_config.neuron_count}")
    print(f"   - Updated connection probability: {updated_config.connection_probability}")
    
    # Save configuration
    config_manager.save_config("demo_config.json")
    print(f"\n   Configuration saved to: demo_config.json")
    
    # Load configuration
    from nlm import get_default_config
    new_config = get_default_config()
    new_config_manager = ConfigManager(new_config)
    new_config_manager.load_config("demo_config.json")
    
    print(f"\n   Loaded configuration:")
    print(f"   - Neuron count: {new_config_manager.get_config().neuron_count}")
    print(f"   - Region count: {new_config_manager.get_config().region_count}")
    
    print("\n   Configuration management demonstration completed!")

def demonstrate_performance_monitoring(brain):
    """
    Demonstrate performance monitoring capabilities.
    
    Args:
        brain: AdvancedNLM instance
    """
    print("\n" + "=" * 60)
    print("Performance Monitoring Demonstration")
    print("=" * 60)
    
    # Monitor memory usage
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    memory_info = process.memory_info()
    
    print(f"\n   Memory usage:")
    print(f"   - RSS: {memory_info.rss/1024/1024:.1f} MB")
    print(f"   - VMS: {memory_info.vms/1024/1024:.1f} MB")
    print(f"   - CPU percent: {process.cpu_percent()}%")
    
    # Get brain performance stats
    performance_stats = brain.enhanced_brain.get_performance_stats()
    print(f"\n   Brain performance statistics:")
    for metric, value in performance_stats.items():
        print(f"   - {metric}: {value}")
    
    # Run simulation and monitor performance
    print("\n   Running simulation while monitoring performance...")
    
    simulation = brain.create_simulation("performance_test", max_steps=100)
    
    def monitor_performance(step, brain):
        # Get current performance metrics
        current_memory = process.memory_info().rss / 1024 / 1024
        current_cpu = process.cpu_percent()
        
        if step % 25 == 0:
            print(f"   Step {step}: Memory={current_memory:.1f}MB, CPU={current_cpu:.1f}%")
    
    simulation.run(100, callback=monitor_performance)
    
    # Analyze stability
    print("\n   Stability analysis:")
    stability_analysis = brain.enhanced_brain.analyze_stability(100)
    print(f"   - Stability: {stability_analysis['stability']}")
    print(f"   - Recommendations: {stability_analysis['recommendations']}")
    
    print("\n   Performance monitoring demonstration completed!")

def advanced_example():
    """
    Advanced example combining multiple NLM features.
    """
    print("\n" + "=" * 60)
    print("Advanced Example: Comprehensive NLM Workflow")
    print("=" * 60)
    
    # Step 1: Create brain with custom configuration
    print("\n1. Creating brain with custom configuration...")
    from nlm import create_config
    
    custom_config = create_config({
        "neuron_count": 200,
        "region_count": 2,
        "connection_probability": 0.1,
        "simulation_timestep": 0.001,
        "plasticity_enabled": True,
        "neuromodulation_enabled": True,
    })
    
    brain = AdvancedNLM(custom_config)
    print(f"   - Custom brain created with {brain.brain.getTotalNeuronCount()} neurons")
    
    # Step 2: Run basic simulation
    print("\n2. Running basic simulation...")
    basic_results = brain.run_experiment("basic_advanced", 100)
    print(f"   - Completed {basic_results['steps_completed']} steps")
    
    # Step 3: Configure advanced learning systems
    print("\n3. Configuring advanced learning systems...")
    brain.brain.enableRewardModulation(True)
    brain.brain.enableStructuralPlasticity(True)
    brain.brain.enableDevelopment(True)
    brain.brain.enableCuriosity(True)
    
    # Step 4: Run learning simulation
    print("\n4. Running learning simulation...")
    learning_results = brain.run_experiment("advanced_learning", 300)
    print(f"   - Learning completed in {learning_results['steps_completed']} steps")
    
    # Step 5: Evaluate performance
    print("\n5. Evaluating performance...")
    performance_stats = brain.enhanced_brain.get_performance_stats()
    print(f"   - Performance: {performance_stats}")
    
    # Step 6: Analyze activity patterns
    print("\n6. Analyzing activity patterns...")
    patterns = brain.enhanced_brain.get_activity_patterns(100)
    
    print(f"   - Activity patterns collected:")
    for pattern_name, data in patterns.items():
        print(f"     * {pattern_name}: {len(data)} data points")
    
    # Step 7: Find active regions
    print("\n7. Finding active regions...")
    active_regions = brain.enhanced_brain.find_active_regions(threshold=0.1)
    print(f"   - Found {len(active_regions)} active regions")
    
    # Step 8: Analyze stability
    print("\n8. Analyzing stability...")
    stability = brain.enhanced_brain.analyze_stability(100)
    print(f"   - Stability: {stability['stability']}")
    
    # Step 9: Save results
    print("\n9. Saving results...")
    brain.save_brain("advanced_example_brain.bin")
    
    simulation = brain.create_simulation("final_advanced", max_steps=1000)
    simulation.export_events("advanced_example_events.json")
    simulation.export_metrics("advanced_example_metrics.json")
    
    print("   - Brain model saved")
    print("   - Events saved")
    print("   - Metrics saved")
    
    print("\n" + "=" * 60)
    print("Advanced example completed successfully!")
    print("=" * 60)

if __name__ == "__main__":
    import time
    
    print("NLM Example 1: Basic Usage")
    print("This example demonstrates the basic usage of NLM for spiking neural network simulation.")
    
    try:
        # Run basic example
        brain, basic_results = basic_example()
        
        # Demonstrate event monitoring
        demonstrate_event_monitoring(brain)
        
        # Demonstrate learning simulation
        demonstrate_learning_simulation(brain)
        
        # Demonstrate configuration management
        demonstrate_configuration_management(brain)
        
        # Demonstrate performance monitoring
        demonstrate_performance_monitoring(brain)
        
        # Run advanced example
        advanced_example()
        
        print("\n" + "=" * 60)
        print("ALL EXAMPLES COMPLETED SUCCESSFULLY!")
        print("=" * 60)
        print("\nExample outputs:")
        print("  - basic_example_results.png: Visualization of basic simulation")
        print("  - advanced_example_brain.bin: Saved brain model")
        print("  - demo_config.json: Configuration file")
        print("  - advanced_example_events.json: Simulation events")
        print("  - advanced_example_metrics.json: Simulation metrics")
        
    except Exception as e:
        print(f"\nError during execution: {e}")
        import traceback
        traceback.print_exc()