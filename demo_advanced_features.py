#!/usr/bin/env python3
"""
Enhanced NLM Advanced Features Demonstration

Demonstration of the advanced features in the Enhanced NLM Python API,
showing how to use the new CLI, advanced analytics, and experiment
management capabilities.
"""

import os
import sys
import time
import json
from pathlib import Path

# Add the project root to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
    from enhanced_nlm import (
        AdvancedNLM,
        ExperimentConfig,
        ExperimentRunner,
        ExperimentDataset,
        create_advanced_nlm,
        create_experiment_runner,
        create_experiment_dataset,
        create_default_config,
        AdvancedAnalytics,
        ExperimentResult,
        LearningMetrics
    )
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: Required modules not available: {e}")
    print("Some demonstrations will be skipped.")

def demonstrate_cli():
    """Demonstrate the advanced CLI functionality."""
    print("=" * 80)
    print("DEMONSTRATION 1: Advanced NLM CLI")
    print("=" * 80)
    print()
    
    if not PYNLM_AVAILABLE:
        print("Skipping CLI demonstration - pynlm not available")
        return
    
    print("The advanced CLI provides a comprehensive interface for managing NLM simulations.")
    print()
    
    print("Key CLI commands:")
    print("  nlm create --neurons 1000 --connection_probability 0.1")
    print("  nlm run --steps 500 --reward-modulation --curiosity")
    print("  nlm experiment --neurons 500 --steps 1000 --checkpoint")
    print("  nlm save --output brain_state.bin")
    print("  nlm load --input brain_state.bin")
    print("  nlm list-config")
    print()
    
    # Show example of CLI usage
    print("Example CLI usage:")
    print("  # Create a brain with 1000 neurons")
    print("  nlm create --neurons 1000 --connection_probability 0.1")
    print()
    print("  # Run a simulation with reward modulation")
    print("  nlm run --steps 500 --reward-modulation --curiosity")
    print()
    print("  # Run an integration experiment")
    print("  nlm experiment --neurons 500 --steps 1000 --checkpoint --replay")
    print()
    
    print("CLI features:")
    print("  ✓ Advanced command-line interface")
    print("  ✓ Configuration management")
    print("  ✓ Experiment running")
    print("  ✓ Checkpoint management")
    print("  ✓ Real-time monitoring")
    print("  ✓ Progress reporting")
    print()

def demonstrate_advanced_nlm():
    """Demonstrate the Advanced NLM functionality."""
    print("=" * 80)
    print("DEMONSTRATION 2: Advanced NLM")
    print("=" * 80)
    print()
    
    if not PYNLM_AVAILABLE:
        print("Skipping Advanced NLM demonstration - pynlm not available")
        return
    
    print("The Advanced NLM provides enhanced functionality with:")
    print()
    
    # Create advanced configuration
    config = ExperimentConfig(
        neuron_count=500,
        connection_probability=0.15,
        enable_stdp=True,
        enable_hebbian=True,
        enable_structural_plasticity=True,
        enable_curiosity=True,
        enable_novelty=True,
        enable_development=True,
        max_steps=1000
    )
    
    print("1. Advanced Configuration:")
    print(f"   Neurons: {config.neuron_count}")
    print(f"   Connection Probability: {config.connection_probability}")
    print(f"   Enable STDP: {config.enable_stdp}")
    print(f"   Enable Curiosity: {config.enable_curiosity}")
    print(f"   Max Steps: {config.max_steps}")
    print()
    
    # Create advanced NLM
    nlm = AdvancedNLM(config)
    
    print("2. Creating Advanced Brain:")
    success = nlm.create_advanced_brain()
    
    if success:
        print("   ✓ Brain created successfully")
        print(f"   - Total Neurons: {nlm.brain.getTotalNeuronCount()}")
        print(f"   - Total Synapses: {nlm.brain.getTotalSynapseCount()}")
    else:
        print("   ✗ Failed to create brain")
        return
    
    print()
    print("3. Creating Advanced World:")
    success = nlm.create_advanced_world(width=20, height=20, vision_width=8, vision_height=8)
    
    if success:
        print("   ✓ World created successfully")
        print(f"   - World Size: 20x20")
        print(f"   - Vision: 8x8")
    else:
        print("   ✗ Failed to create world")
    
    print()
    print("4. Running Advanced Simulation:")
    result = nlm.run_advanced_simulation(max_steps=200)
    
    if result['success']:
        print("   ✓ Simulation completed successfully")
        print(f"   - Wall Clock Time: {result['wall_clock_time']:.2f}s")
        print(f"   - Simulation Time: {result['simulation_time']:.2f}s")
        print(f"   - Final Learning Efficiency: {result['metrics'].learning_efficiency:.3f}")
        
        # Show convergence analysis
        convergence = result.get('convergence', {})
        if convergence:
            print("   - Convergence Analysis:")
            for key, value in convergence.items():
                print(f"     {key}: {value}")
        
        # Save experiment result
        timestamp = time.strftime("%Y%m%d_%H%M%S")
        filepath = f"advanced_nlm_experiment_{timestamp}.json"
        
        nlm.save_experiment_result(filepath, "Advanced NLM Demo Experiment", "Demo of advanced NLM features")
        print(f"   - Result saved to: {filepath}")
    else:
        print(f"   ✗ Simulation failed: {result.get('error', 'Unknown error')}")
    
    print()

def demonstrate_experiment_runner():
    """Demonstrate the Experiment Runner functionality."""
    print("=" * 80)
    print("DEMONSTRATION 3: Experiment Runner")
    print("=" * 80)
    print()
    
    if not PYNLM_AVAILABLE:
        print("Skipping Experiment Runner demonstration - pynlm not available")
        return
    
    print("The Experiment Runner provides sophisticated experiment management:")
    print()
    
    # Create experiment runner
    runner = ExperimentRunner()
    
    # Add multiple experiments
    print("1. Adding Experiments:")
    
    config1 = create_default_config()
    config1.neuron_count = 200
    config1.max_steps = 500
    config1.enable_stdp = True
    
    runner.add_experiment(
        name="Basic Plasticity",
        config=config1,
        description="Test basic STDP and Hebbian learning"
    )
    
    config2 = create_default_config()
    config2.neuron_count = 400
    config2.max_steps = 1000
    config2.enable_curiosity = True
    config2.enable_novelty = True
    
    runner.add_experiment(
        name="Neuromodulation Experiment",
        config=config2,
        description="Test curiosity-driven exploration and novelty detection"
    )
    
    print(f"   - Added {len(runner.experiments)} experiments")
    print(f"   - Experiments: {[exp['name'] for exp in runner.experiments]}")
    print()
    
    print("2. Running Experiments:")
    print("   Note: Running experiments will take time. For demonstration, we'll show the workflow...")
    print()
    
    # Show experiment details
    for i, experiment in enumerate(runner.experiments):
        print(f"   Experiment {i+1}: {experiment['name']}")
        print(f"     Description: {experiment['description']}")
        print(f"     Neurons: {experiment['config'].neuron_count}")
        print(f"     Max Steps: {experiment['config'].max_steps}")
        print(f"     STDP Enabled: {experiment['config'].enable_stdp}")
        print(f"     Curiosity Enabled: {experiment['config'].enable_curiosity}")
        print(f"     Status: {experiment['status']}")
        print()
    
    print("3. Results Summary:")
    summary = runner.get_results_summary()
    print(f"   {summary}")
    
    print()
    print("Experiment Runner Features:")
    print("  ✓ Advanced experiment scheduling")
    print("  ✓ Configuration validation")
    print("  ✓ Comprehensive monitoring")
    print("  ✓ Progress reporting")
    print("  ✓ Results management")
    print()

def demonstrate_experiment_dataset():
    """Demonstrate the Experiment Dataset functionality."""
    print("=" * 80)
    print("DEMONSTRATION 4: Experiment Dataset")
    print("=" * 80)
    print()
    
    print("The Experiment Dataset provides comprehensive experiment management:")
    print()
    
    # Create experiment dataset
    dataset = ExperimentDataset("NLM Advanced Features Demo")
    
    print("1. Creating Dataset:")
    print(f"   - Dataset Name: {dataset.name}")
    print(f"   - Initial Experiments: {len(dataset.experiments)}")
    print()
    
    print("2. Dataset Summary:")
    summary = dataset.get_summary()
    if "error" in summary:
        print(f"   Error: {summary['error']}")
    else:
        print(f"   - Total Experiments: {summary['total_experiments']}")
        print(f"   - Successful Experiments: {summary['successful_experiments']}")
        print(f"   - Failed Experiments: {summary['failed_experiments']}")
        print(f"   - Average Learning Efficiency: {summary['avg_learning_efficiency']:.3f}")
        print(f"   - Average Duration: {summary['avg_duration']:.2f}s")
    
    print()
    print("Experiment Dataset Features:")
    print("  ✓ Experiment storage and management")
    print("  ✓ Best experiment selection")
    print("  ✓ Data export (CSV, JSON)")
    print("  ✓ Summary statistics")
    print("  ✓ Metadata management")
    print()

def demonstrate_advanced_analytics():
    """Demonstrate the Advanced Analytics functionality."""
    print("=" * 80)
    print("DEMONSTRATION 5: Advanced Analytics")
    print("=" * 80)
    print()
    
    print("Advanced Analytics provides sophisticated analysis tools:")
    print()
    
    # Create sample metrics
    metrics = LearningMetrics()
    metrics.reward_history = [1.0, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5]
    metrics.firing_rate_history = [10.0, 15.0, 20.0, 25.0, 30.0, 35.0, 40.0, 45.0, 50.0, 55.0]
    metrics.weight_changes = 15.0
    metrics.total_spikes = 100
    metrics.dopamine_level = 0.8
    metrics.prediction_error = 0.2
    
    print("1. Sample Metrics:")
    print(f"   - Reward History: {len(metrics.reward_history)} samples")
    print(f"   - Firing Rate History: {len(metrics.firing_rate_history)} samples")
    print(f"   - Weight Changes: {metrics.weight_changes}")
    print(f"   - Total Spikes: {metrics.total_spikes}")
    print(f"   - Dopamine Level: {metrics.dopamine_level}")
    print()
    
    # Compute analytics
    analytics = AdvancedAnalytics()
    
    print("2. Convergence Analysis:")
    convergence = analytics.compute_convergence(metrics)
    if convergence:
        for key, value in convergence.items():
            print(f"   - {key}: {value}")
    else:
        print("   - No convergence data available")
    
    print()
    print("3. Performance Analysis:")
    performance = analytics.compute_performance_metrics(metrics)
    if performance:
        for key, value in performance.items():
            print(f"   - {key}: {value}")
    else:
        print("   - No performance data available")
    
    print()
    print("4. Stability Analysis:")
    stability = analytics.compute_stability_analysis(metrics)
    if stability:
        for key, value in stability.items():
            print(f"   - {key}: {value}")
    else:
        print("   - No stability data available")
    
    print()
    print("Advanced Analytics Features:")
    print("  ✓ Convergence analysis")
    print("  ✓ Performance metrics")
    print("  ✓ Stability analysis")
    print("  ✓ Machine learning integration")
    print("  ✓ Statistical analysis")
    print("  ✓ Visualization support")
    print()

def demonstrate_file_io():
    """Demonstrate file I/O capabilities."""
    print("=" * 80)
    print("DEMONSTRATION 6: File I/O Operations")
    print("=" * 80)
    print()
    
    print("File I/O operations for experiment management:")
    print()
    
    # Create sample experiment result
    config = create_default_config()
    
    metrics = LearningMetrics()
    metrics.reward_history = [1.0, 1.5, 2.0, 2.5, 3.0]
    metrics.firing_rate_history = [10.0, 15.0, 20.0, 25.0, 30.0]
    
    result = ExperimentResult(
        config=config,
        metrics=metrics,
        success=True,
        experiment_name="File I/O Demo",
        description="Demonstration of file I/O capabilities"
    )
    
    print("1. Export to JSON:")
    json_file = "sample_experiment.json"
    try:
        # Convert to dict for JSON serialization
        result_dict = {
            'experiment_name': result.experiment_name,
            'success': result.success,
            'config': config.__dict__,
            'metrics': metrics.__dict__,
            'wall_clock_time': result.wall_clock_time,
        }
        
        with open(json_file, 'w') as f:
            json.dump(result_dict, f, indent=2, default=str)
        
        print(f"   ✓ Experiment saved to {json_file}")
        
        # Read back
        with open(json_file, 'r') as f:
            loaded_data = json.load(f)
        
        print(f"   ✓ Experiment loaded from {json_file}")
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
    
    print()
    print("2. CSV Export:")
    csv_file = "sample_experiments.csv"
    try:
        import pandas as pd
        
        # Create DataFrame
        data = [{
            'experiment_id': 'exp_001',
            'name': 'File I/O Demo',
            'success': True,
            'duration': result.wall_clock_time,
            'total_reward': metrics.total_reward,
            'avg_firing_rate': metrics.avg_firing_rate,
            'learning_efficiency': metrics.learning_efficiency,
            'description': result.description,
        }]
        
        df = pd.DataFrame(data)
        df.to_csv(csv_file, index=False)
        print(f"   ✓ Data exported to {csv_file}")
        
    except Exception as e:
        print(f"   ✗ Error: {e}")
    
    print()
    print("File I/O Features:")
    print("  ✓ JSON export/import")
    print("  ✓ CSV export")
    print("  ✓ Configuration saving")
    print("  ✓ Experiment results")
    print("  ✓ Data persistence")
    print()

def main():
    """Main demonstration function."""
    print("Enhanced NLM Advanced Features Demonstration")
    print("=" * 80)
    print()
    print("This demonstration showcases the advanced features of the Enhanced NLM")
    print("Python API, including CLI tools, advanced analytics, experiment management,")
    print("and comprehensive file I/O capabilities.")
    print()
    
    if not PYNLM_AVAILABLE:
        print("Note: Some demonstrations may be limited due to missing dependencies.")
        print()
    
    # Run demonstrations
    demonstrate_cli()
    
    if PYNLM_AVAILABLE:
        demonstrate_advanced_nlm()
        demonstrate_experiment_runner()
        demonstrate_experiment_dataset()
        demonstrate_advanced_analytics()
    else:
        print("Skipping advanced demonstrations - pynlm not available")
    
    demonstrate_file_io()
    
    print("=" * 80)
    print("DEMONSTRATION COMPLETE")
    print("=" * 80)
    print()
    print("Summary of Demonstrated Features:")
    print("  1. Advanced NLM CLI with comprehensive command-line interface")
    print("  2. Advanced NLM with enhanced configuration and monitoring")
    print("  3. Experiment Runner with sophisticated scheduling")
    print("  4. Experiment Dataset with experiment management")
    print("  5. Advanced Analytics with convergence, performance, and stability analysis")
    print("  6. File I/O operations with JSON and CSV export")
    print()
    print("These features provide a comprehensive toolkit for NLM research and experimentation.")
    print("For more information, see the documentation and examples.")

if __name__ == '__main__':
    main()
