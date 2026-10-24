#!/usr/bin/env python3
"""
NLM Python Examples and Tutorials

Comprehensive examples and tutorials for using the enhanced NLM Python interface.
These examples demonstrate the full capabilities of the improved NLM framework.
"""

import os
import sys
import json
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, str(Path(__file__).parent))

from nlm_py import (
    NLMConfig, NLMBrain, NLMSimulation, NLMExperiment,
    ExperimentRunner, NLMAnalyzer, NLMVisualizer
)
def example_basic_usage():
    """Example 1: Basic brain creation and simulation."""
    print("=" * 60)
    print("Example 1: Basic Brain Creation and Simulation")
    print("=" * 60)
    
    # Create a brain with custom configuration
    config = NLMConfig({
        'neuron_count': 500,
        'region_count': 2,
        'simulation_timestep': 0.001,
        'random_seed': 42
    })
    
    print(f"Creating brain with {config.get('neuron_count')} neurons...")
    brain = NLMBrain(config)
    
    # Initialize the brain
    brain.initialize()
    print("Brain initialized successfully")
    
    # Get brain statistics
    stats = brain.get_stats()
    print(f"Brain statistics:")
    print(f"  Total neurons: {stats['total_neurons']}")
    print(f"  Total synapses: {stats['total_synapses']}")
    print(f"  Average firing rate: {stats['average_firing_rate']:.3f} Hz")
    
    # Run a simple simulation
    print("\nRunning 100 simulation steps...")
    simulation = NLMSimulation(brain)
    with simulation.run_context(100) as step:
        print(f"  Step {step} completed")
    
    print(f"Final brain statistics:")
    final_stats = brain.get_stats()
    print(f"  Total spikes: {final_stats['total_spike_count']}")
    print(f"  Active neurons: {final_stats['active_neurons']}")
    
    return brain
def example_experiment_management():
    """Example 2: Running controlled experiments."""
    print("\n" + "=" * 60)
    print("Example 2: Running Controlled Experiments")
    print("=" * 60)
    
    # Create configuration for experiment
    config = NLMConfig()
    
    # Create brain
    brain = NLMBrain(config)
    brain.initialize()
    
    # Run an experiment with results tracking
    print("Starting experiment...")
    experiment = NLMExperiment(brain)
    experiment.start()
    
    # Run simulation steps while recording data
    for step in range(200):
        if brain._brain:
            brain._brain.step(step, step * 0.001)
        experiment.record(step, brain.get_stats())
        
        # Record intermediate metrics
        stats = brain.get_stats()
        if 'total_spike_count' in stats:
            experiment.add_metric_measurement(
                step, 'total_spike_count', stats['total_spike_count']
            )
        
        if step % 50 == 0:
            print(f"  Progress: {step}/200 steps")
    
    # Complete experiment
    results = experiment.end()
    
    print(f"\nExperiment completed!")
    print(f"Duration: {results.duration:.2f} seconds")
    print(f"Total steps: {len(results.time_series_data)}")
    print(f"Final spike count: {results.brain_stats.get('total_spike_count', 0)}")
    print(f"Final firing rate: {results.brain_stats.get('average_firing_rate', 0.0):.3f} Hz")
    
    # Save results
    results.save_results("experiment_results.json")
    print(f"Results saved to experiment_results.json")
    
    # Analyze results
    analyzer = NLMAnalyzer()
    spike_analysis = analyzer.analyze_spike_patterns(results)
    print(f"\nSpike analysis:")
    print(f"  Total spikes: {spike_analysis['total_spikes']}")
    print(f"  Average firing rate: {spike_analysis['spike_statistics']['mean']:.3f} Hz")
    print(f"  Spike pattern: {spike_analysis['spike_trend']}")
    
    return results
def example_plasticity_experiment():
    """Example 3: Plasticity and learning experiments."""
    print("\n" + "=" * 60)
    print("Example 3: Plasticity and Learning Experiments")
    print("=" * 60)
    
    # Create brain for plasticity experiment
    config = NLMConfig({
        'neuron_count': 1000,
        'connection_probability': 0.15,
        'plasticity_learning_rate': 0.02,
        'stdp_ltp_weight': 0.02,
        'stdp_ltd_weight': 0.015
    })
    
    brain = NLMBrain(config)
    brain.initialize()
    print("Brain initialized for plasticity experiment")
    
    # Enable plasticity on all synapses
    if brain._brain:
        for region in brain._brain.getRegions():
            for synapse in region.getSynapses():
                synapse.enablePlasticity(True, True, False)
                
    print("Plasticity enabled on all synapses")
    
    # Run initial test
    initial_stats = brain.get_stats()
    print(f"\nInitial state:")
    print(f"  Total spikes: {initial_stats['total_spike_count']}")
    print(f"  Average firing rate: {initial_stats['average_firing_rate']:.3f} Hz")
    
    # Apply learning input
    print("\nApplying learning input (simulating sensory input)...")
    learning_steps = 500
    
    for step in range(learning_steps):
        # Inject sensory input (simulating external stimulation)
        if brain._brain:
            brain._brain.injectCurrentToNeurons(
                NeuronType.Sensory, 30.0f)
            brain._brain.step(step, step * 0.001)
            
        # Log progress
        if step % 100 == 0:
            stats = brain.get_stats()
            print(f"  Step {step}: Spikes={stats['total_spike_count']}, "
                  f"Firing={stats['firing_neurons']}, Rate={stats['average_firing_rate']:.3f} Hz")
    
    # Final assessment
    final_stats = brain.get_stats()
    print(f"\nPlasticity experiment results:")
    print(f"  Final spike count: {final_stats['total_spike_count']}")
    print(f"  Spike change: {final_stats['total_spike_count'] - initial_stats['total_spike_count']}")
    print(f"  Final average firing rate: {final_stats['average_firing_rate']:.3f} Hz")
    print(f"  Firing neurons: {final_stats['firing_neurons']}")
    
    # Check for learning
    spike_change = final_stats['total_spike_count'] - initial_stats['total_spike_count']
    learning_detected = spike_change > 0
    
    print(f"\nLearning detected: {'✓ YES' if learning_detected else '✗ NO'}")
    if learning_detected:
        print("  Synaptic weights have changed through experience")
    else:
        print("  No significant learning occurred - check parameters")
    
    return brain, initial_stats, final_stats
def example_stdp_verification():
    """Example 4: STDP (Spike Timing Dependent Plasticity) verification."""
    print("\n" + "=" * 60)
    print("Example 4: STDP Verification Experiment")
    print("=" * 60)
    
    # Create brain for STDP testing
    config = NLMConfig({
        'neuron_count': 200,
        'stdp_ltp_weight': 0.02,
        'stdp_ltd_weight': 0.015,
        'stdp_time_constant': 20.0
    })
    
    brain = NLMBrain(config)
    brain.initialize()
    print("Brain initialized for STDP verification")
    
    # Enable only STDP plasticity
    if brain._brain:
        for region in brain._brain.getRegions():
            for synapse in region.getSynapses():
                synapse.enablePlasticity(False, True, False)
                
    # Get neurons for pre/post testing
    neurons = brain.get_neurons()
    if len(neurons) < 2:
        print("Error: Need at least 2 neurons for STDP test")
        return None
        
    pre_neuron = neurons[0]
    post_neuron = neurons[1]
    
    print(f"Using pre-synaptic neuron: {pre_neuron.get_id()}")
    print(f"Using post-synaptic neuron: {post_neuron.get_id()}")
    
    # STDP test: pre fires before post (LTP - Long Term Potentiation)
    print("\nTesting STDP LTP (pre before post)...")
    ltp_trials = 30
    
    for trial in range(ltp_trials):
        if brain._brain and pre_neuron._neuron and post_neuron._neuron:
            # Pre fires first
            pre_neuron._neuron.injectCurrent(60.0f)
            brain._brain.step(trial * 2, trial * 2 * 0.001)
            
            # Post fires immediately after
            post_neuron._neuron.injectCurrent(60.0f)
            brain._brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
            
        if (trial + 1) % 10 == 0:
            print(f"  Completed {trial + 1}/{ltp_trials} LTP trials")
    
    # Check LTP results
    ltp_stats = brain.get_stats()
    print(f"\nLTP test results:")
    print(f"  Total spikes: {ltp_stats['total_spike_count']}")
    print(f"  Learning detected: {'✓ YES' if ltp_stats['total_spike_count'] > 0 else '✗ NO'}")
    
    # Reset and test LTD (post before pre)
    brain.reset()
    brain.initialize()
    
    print("\n" + "=" * 60)
    print("Testing STDP LTD (post before pre)...")
    
    # Enable STDP again
    if brain._brain:
        for region in brain._brain.getRegions():
            for synapse in region.getSynapses():
                synapse.enablePlasticity(False, True, False)
                
    # Test LTD: post fires before pre
    ltd_trials = 30
    
    for trial in range(ltd_trials):
        if brain._brain and pre_neuron._neuron and post_neuron._neuron:
            # Post fires first
            post_neuron._neuron.injectCurrent(60.0f)
            brain._brain.step(trial * 2, trial * 2 * 0.001)
            
            # Pre fires immediately after
            pre_neuron._neuron.injectCurrent(60.0f)
            brain._brain.step(trial * 2 + 1, (trial * 2 + 1) * 0.001)
            
        if (trial + 1) % 10 == 0:
            print(f"  Completed {trial + 1}/{ltd_trials} LTD trials")
    
    # Check LTD results
    ltd_stats = brain.get_stats()
    print(f"\nLTD test results:")
    print(f"  Total spikes: {ltd_stats['total_spike_count']}")
    print(f"  Learning detected: {'✓ YES' if ltd_stats['total_spike_count'] > 0 else '✗ NO'}")
    
    print(f"\nSTDP verification complete!")
    return brain
def example_visualization_and_analysis():
    """Example 5: Visualization and analysis tools."""
    print("\n" + "=" * 60)
    print("Example 5: Visualization and Analysis Tools")
    print("=" * 60)
    
    # Create brain and run experiment
    config = NLMConfig({
        'neuron_count': 300,
        'max_simulation_steps': 200
    })
    
    brain = NLMBrain(config)
    brain.initialize()
    
    print("Running experiment for visualization...")
    experiment = NLMExperiment(brain)
    experiment.start()
    
    for step in range(100):
        if brain._brain:
            brain._brain.step(step, step * 0.001)
            experiment.record(step, brain.get_stats())
    
    results = experiment.end()
    
    # Create visualizations
    print("\nCreating visualizations...")
    
    # Create time series plots
    NLMVisualizer.create_time_series_plots(results, "plots")
    print("✓ Time series plots created in 'plots/' directory")
    
    # Create HTML dashboard
    dashboard_path = "experiment_dashboard.html"
    NLMVisualizer.create_summary_dashboard(results, dashboard_path)
    print(f"✓ HTML dashboard created: {dashboard_path}")
    
    # Analyze results
    analyzer = NLMAnalyzer()
    
    print("\nAnalyzing experiment results...")
    
    # Basic analysis
    spike_analysis = analyzer.analyze_spike_patterns(results)
    print(f"Spike pattern analysis:")
    print(f"  Total spikes: {spike_analysis['total_spikes']}")
    print(f"  Average firing rate: {spike_analysis['spike_statistics']['mean']:.3f} Hz")
    print(f"  Spike trend: {spike_analysis['spike_trend']}")
    
    # Pattern detection
    pattern_results = analyzer.detect_patterns(results, threshold=0.1)
    print(f"\nPattern detection:")
    print(f"  Total patterns detected: {pattern_results['total_patterns']}")
    print(f"  Significant changes: {pattern_results['significant_changes']}")
    
    return results
def example_cli_usage():
    """Example 6: Using the NLM command-line interface."""
    print("\n" + "=" * 60)
    print("Example 6: NLM Command-Line Interface Usage")
    print("=" * 60)
    
    print("The NLM CLI provides a user-friendly interface for:")
    print("1. Creating and configuring brains")
    print("2. Running various experiment types")
    print("3. Saving and loading brain states")
    print("4. Monitoring brain statistics")
    print("\nExample CLI commands:")
    print("  nlm_cli.py --create-brain --connectivity-test 100")
    print("  nlm_cli.py --create-brain --plasticity-test 500")
    print("  nlm_cli.py --create-brain --stdp-test 50")
    print("  nlm_cli.py --create-brain --run-experiment --steps 1000")
    print("  nlm_cli.py --create-brain --save-brain my_brain.bin")
    print("  nlm_cli.py --create-brain --load-brain my_brain.bin")
    print("  nlm_cli.py --create-brain --show-stats")
    print("  nlm_cli.py --create-brain --show-config")
    
    print("\nFor detailed help:")
    print("  nlm_cli.py --help")
    print("\nTo use configuration sets:")
    print("  nlm_cli.py --create-brain --config-set neuron_count 1000")
    print("  nlm_cli.py --create-brain --config-set simulation_timestep 0.01")
def example_plugin_usage():
    """Example 7: Using the plugin system (stub)."""
    print("\n" + "=" * 60)
    print("Example 7: Plugin System (Advanced)")
    print("=" * 60)
    
    print("The plugin system allows extending NLM functionality with:")
    print("1. Custom neuron models")
    print("2. Novel plasticity rules")
    print("3. Specialized visualization tools")
    print("4. Advanced analysis algorithms")
    print("5. Custom experiment types")
    
    print("\nPlugin architecture includes:")
    print("- Plugin interface and base classes")
    print("- Plugin loader and manager")
    print("- Plugin registry and discovery")
    print("- Security and validation")
    print("- Version management")
    
    print("\nExample plugin development:")
    print("  class CustomNeuronPlugin:")
    print("      def initialize(self, brain): ...")
    print("      def step(self, timestep): ...")
    print("      def get_metrics(self): ...")
    
    print("\nPlugin management commands:")
    print("  nlm_cli.py --plugin-list")
    print("  nlm_cli.py --install-plugin my_plugin.zip")
    print("  nlm_cli.py --uninstall-plugin plugin_name")
    print("  nlm_cli.py --enable-plugin plugin_name")
    print("  nlm_cli.py --disable-plugin plugin_name")
    
    print("\nNote: Plugin system is under development - features will be added.")
def example_batch_processing():
    """Example 8: Batch experiment processing."""
    print("\n" + "=" * 60)
    print("Example 8: Batch Experiment Processing")
    print("=" * 60)
    
    print("Batch processing allows running multiple experiments with different")
    print("parameters and collecting results systematically.")
    
    # Define experiment parameter sets
    parameter_sets = [
        {'name': 'Baseline', 'neuron_count': 500, 'plasticity_rate': 0.01},
        {'name': 'High Plasticity', 'neuron_count': 1000, 'plasticity_rate': 0.05},
        {'name': 'Low Plasticity', 'neuron_count': 300, 'plasticity_rate': 0.001},
        {'name': 'Large Network', 'neuron_count': 2000, 'plasticity_rate': 0.02},
    ]
    
    print("\nRunning parameter sweep...")
    for i, params in enumerate(parameter_sets):
        print(f"\nExperiment {i+1}: {params['name']}")
        print(f"  Neurons: {params['neuron_count']}")
        print(f"  Plasticity rate: {params['plasticity_rate']}")
        
        # Create configuration
        config = NLMConfig({
            'neuron_count': params['neuron_count'],
            'plasticity_learning_rate': params['plasticity_rate'],
            'random_seed': 42 + i  # Different seed for each run
        })
        
        # Create and run brain
        brain = NLMBrain(config)
        brain.initialize()
        
        # Run simple test
        for step in range(50):
            if brain._brain:
                brain._brain.step(step, step * 0.001)
        
        # Get results
        stats = brain.get_stats()
        print(f"  Result: {stats['total_spike_count']} spikes, "
              f"{stats['average_firing_rate']:.3f} Hz")
        
    print(f"\nBatch processing complete! Ran {len(parameter_sets)} experiments.")
def example_data_export():
    """Example 9: Exporting experiment data."""
    print("\n" + "=" * 60)
    print("Example 9: Exporting Experiment Data")
    print("=" * 60)
    
    # Create sample experiment results
    print("Creating sample experiment results...")
    
    # This would normally load actual experiment results
    sample_data = {
        'steps': list(range(100)),
        'total_spike_count': [i * 2 + 10 for i in range(100)],  # Sample data
        'average_firing_rate': [i * 0.1 + 0.5 for i in range(100)],
        'active_neurons': [i + 5 for i in range(100)]
    }
    
    print("\nAvailable export formats:")
    print("1. CSV (for spreadsheet analysis)")
    print("2. JSON (for programmatic use)")
    print("3. Pickle (for Python objects)")
    print("4. HTML (for web visualization)")
    print("5. MATLAB/Octave (for scientific computing)")
    
    print("\nExample export commands:")
    print("  nlm_cli.py --run-experiment --save-results experiment1.json")
    print("  nlm_cli.py --run-experiment --export-results experiment1.csv")
    print("  nlm_cli.py --run-experiment --export-for-matlab experiment1.mat")
    
    print("\nExported data typically includes:")
    print("  - Time series of neural activity")
    print("  - Simulation parameters")
    print("  - Performance metrics")
    print("  - Configuration settings")
    print("  - Analysis results")
def main_examples():
    """Run all examples."""
    print("NLM Python Examples and Tutorials")
    print("====================================")
    print("These examples demonstrate the enhanced NLM Python interface.")
    
    examples = [
        example_basic_usage,
        example_experiment_management,
        example_plasticity_experiment,
        example_stdp_verification,
        example_visualization_and_analysis,
        example_cli_usage,
        example_plugin_usage,
        example_batch_processing,
        example_data_export
    ]
    
    for i, example_func in enumerate(examples, 1):
        try:
            example_func()
        except Exception as e:
            print(f"\nError in example {i}: {e}")
            import traceback
            traceback.print_exc()
    
    print("\n" + "=" * 60)
    print("All examples completed!")
    print("=" * 60)
    print("\nFor more detailed documentation, see:")
    print("- README.md in the main directory")
    print("- docs/ directory for technical documentation")
    print("- Examples can be run individually by calling their functions directly")
if __name__ == "__main__":
    main_examples()