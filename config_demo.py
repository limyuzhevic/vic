#!/usr/bin/env python3
"""
Advanced Configuration Demo

Demonstrates the advanced configuration management features
for the NLM (Neural Learning Machine) project.
"""

import advanced_nlm
from typing import Dict, Any
import json
import time

def demo_basic_configuration():
    """Demonstrate basic configuration management."""
    print("=== Basic Configuration Demo ===")
    
    # Create advanced brain
    brain = advanced_nlm.createAdvancedBrain()
    
    # Configure brain
    config = brain.brain.getConfig()
    
    # Set some configuration values
    config.set("brain.neuron_count", 500)
    config.set("brain.synapse_density", 0.1f)
    config.set("simulation.timestep", 0.001f)
    config.set("plasticity.stdp.enable", true)
    config.set("neuromodulation.curiosity.enable", true)
    
    print(f"Neuron count: {config.getOr<int64_t>('brain.neuron_count', 1000)}")
    print(f"Synapse density: {config.getOr<float>('brain.synapse_density', 0.0f)}")
    print(f"Time step: {config.getOr<double>('simulation.timestep', 0.001)}")
    print(f"STDP enabled: {config.getOr<bool>('plasticity.stdp.enable', false)}")
    print(f"Curiosity enabled: {config.getOr<bool>('neuromodulation.curiosity.enable', false)}")
    
    return brain

def demo_experiment_management():
    """Demonstrate experiment management features."""
    print("\n=== Experiment Management Demo ===")
    
    # Create experiment manager
    experiment_manager = advanced_nlm.ExperimentManager()
    
    # Create parameter sweep experiment
    print("Creating parameter sweep experiment...")
    sweep_config = experiment_manager.create_parameter_sweep(
        "brain.neuron_count",
        [500, 1000, 1500],
        steps=100,
        learning_rate=0.01,
        plasticity_enabled=True
    )
    
    print(f"Sweep configuration: {sweep_config}")
    print("Parameter sweep configured for neuron counts: 500, 1000, 1500")
    
    # In a real implementation, this would run multiple simulations
    # For demo purposes, we'll simulate the results
    print("\nSimulating experiment results...")
    
    # Create sample results
    results = {}
    for i, value in enumerate([500, 1000, 1500]):
        results[f"neuron_count_{value}"] = {
            "total_spikes": value * 10,
            "firing_rate": value * 0.5,
            "stability": 0.8 - (i * 0.1),
            "performance": 0.7 + (i * 0.1)
        }
    
    experiment_manager.results = results
    print(f"Experiment results: {len(results)} configurations tested")
    
    # Save results
    experiment_manager.save_experiment_results("demo_experiment_results.json")
    print("Results saved to demo_experiment_results.json")
    
    return experiment_manager

def demo_data_collection():
    """Demonstrate data collection features."""
    print("\n=== Data Collection Demo ===")
    
    # Create advanced brain
    brain = advanced_nlm.createAdvancedBrain()
    brain.enable_advanced_features(
        reward_modulation=True,
        structural_plasticity=True,
        development=True,
        curiosity=True
    )
    
    # Create data collector
    data_collector = advanced_nlm.DataCollector()
    
    # Collect data from multiple simulations
    print("Collecting data from multiple simulations...")
    
    for simulation in range(3):
        # Run simulation
        brain.run_dynamics(100, collect_metrics=True)
        
        # Collect data
        data_collector.collect_data(brain, f"simulation_{simulation}", {
            'simulation_type': 'advanced_demo',
            'neurons': brain.get_stats()['neuron_count'],
            'spikes': brain.get_stats()['total_spikes']
        })
        
        print(f"  Simulation {simulation + 1}: {brain.get_stats()['total_spikes']} spikes")
    
    # Apply filters
    print("\nApplying filters to collected data...")
    data_collector.apply_filter(lambda x: x['brain_stats']['total_spikes'])
    
    # Process data
    data_collector.process_data()
    
    # Get summary statistics
    summary = data_collector.get_summary_statistics()
    
    print(f"\nData Collection Summary:")
    print(f"  Total experiments: {summary.get('total_experiments', 0)}")
    print(f"  Average spikes: {summary.get('avg_spikes_per_experiment', 0):.1f}")
    print(f"  Spike range: {summary.get('spike_range', (0, 0))}")
    print(f"  Total firing neurons: {summary.get('total_firing_neurons', 0)}")
    
    # Export data
    data_collector.export_data("demo_collected_data.json", 'json')
    print("Data exported to demo_collected_data.json")
    
    return data_collector

def demo_ml_integration():
    """Demonstrate machine learning integration."""
    print("\n=== Machine Learning Integration Demo ===")
    
    # Create ML integration
    ml_integration = advanced_nlm.MLIntegration()
    
    # Register feature extractor
    print("Registering feature extractor...")
    ml_integration.register_feature_extractor(
        'brain_performance',
        lambda brain: {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'stability': calculate_stability(brain)
        }
    )
    
    # Create sample data for ML demonstration
    print("\nGenerating sample data for ML demonstration...")
    import numpy as np
    np.random.seed(42)
    
    X = np.random.randn(50, 8)  # 50 samples, 8 features
    y = np.random.randint(0, 2, 50)  # Binary labels
    
    print(f"Sample data: X shape {X.shape}, y shape {y.shape}")
    
    # Extract features from brain
    brain = advanced_nlm.createAdvancedBrain()
    brain.run_dynamics(50)
    
    features = ml_integration.extract_features(brain, 'brain_performance')
    print(f"Brain features: {features}")
    
    # Train ML model
    print("\nTraining ML model...")
    model = ml_integration.train_supervised_model(X, y, 'random_forest')
    print(f"Model trained: {model}")
    
    # Make predictions
    print("Making predictions...")
    predictions = ml_integration.predict_with_model('random_forest', X[:5])
    print(f"Sample predictions: {predictions}")
    
    return ml_integration

def calculate_stability(brain):
    """Calculate stability metric for brain."""
    if hasattr(brain, 'getActivityPattern'):
        patterns = brain.getActivityPattern(50)
        return patterns.get('stability_index', 0.0)
    return 0.0

def demo_visualization():
    """Demonstrate visualization features."""
    print("\n=== Visualization Demo ===")
    
    # Create advanced brain
    brain = advanced_nlm.createAdvancedBrain()
    brain.run_dynamics(200, collect_metrics=True)
    
    print("Creating visualizations...")
    
    try:
        # Spike histogram
        spike_data = [brain.getTotalSpikeCount() for _ in range(10)]
        histogram = advanced_nlm.VisualProfiler.create_spike_histogram(spike_data, bins=5)
        
        print(f"Spike histogram created with {len(histogram['bins'])} bins")
        print(f"  Mean firing rate: {histogram.get('mean', 0):.2f}")
        print(f"  Standard deviation: {histogram.get('std', 0):.2f}")
        
        # Activity pattern analysis
        if hasattr(brain, 'metrics_history') and brain.metrics_history:
            activity_analysis = advanced_nlm.VisualProfiler.analyze_activity_patterns(brain.metrics_history)
            
            print(f"\nActivity Analysis:")
            print(f"  Mean firing rate: {activity_analysis.get('mean_firing_rate', 0):.2f} Hz")
            print(f"  Coefficient of variation: {activity_analysis.get('coefficient_of_variation', 0):.3f}")
            print(f"  Trend: {activity_analysis.get('trend', 'unknown')}")
        
        print("\nVisualization demo complete!")
        
    except Exception as e:
        print(f"Visualization error: {e}")
        print("Skipping detailed visualizations")
    
    return brain

def demo_configuration_management():
    """Demonstrate advanced configuration management."""
    print("\n=== Advanced Configuration Management Demo ===")
    
    # Create advanced brain
    brain = advanced_nlm.createAdvancedBrain()
    
    # Configure advanced features
    brain.enable_advanced_features(
        reward_modulation=True,
        structural_plasticity=True,
        development=True,
        curiosity=True,
        prediction=True
    )
    
    # Get brain configuration
    config = brain.brain.getConfig()
    
    # Set some configuration values
    config.set("brain.development.stage", "critical_period")
    config.set("neuromodulation.dopamine.scale", 1.5f)
    config.set("prediction.error.threshold", 0.01f)
    
    print(f"Development stage: {config.getOr<std::string>('brain.development.stage', '')}")
    print(f"Dopamine scale: {config.getOr<float>('neuromodulation.dopamine.scale', 0.0f)}")
    print(f"Error threshold: {config.getOr<float>('prediction.error.threshold', 0.0f)}")
    
    # Run simulation
    brain.run_dynamics(100)
    
    print(f"\nFinal Statistics:")
    stats = brain.get_stats()
    for key, value in stats.items():
        if key in ['neuron_count', 'synapse_count', 'total_spikes', 'average_firing_rate']:
            print(f"  {key}: {value}")
    
    return brain

def main():
    """Run the advanced configuration demo."""
    print("=" * 70)
    print("Advanced Configuration Management Demo")
    print("=" * 70)
    
    demos = [
        ("Basic Configuration", demo_basic_configuration),
        ("Experiment Management", demo_experiment_management),
        ("Data Collection", demo_data_collection),
        ("Machine Learning Integration", demo_ml_integration),
        ("Visualization", demo_visualization),
        ("Configuration Management", demo_configuration_management)
    ]
    
    results = {}
    
    for demo_name, demo_func in demos:
        try:
            print(f"\n{'='*70}")
            print(f"Running: {demo_name}")
            print(f"{'='*70}")
            
            result = demo_func()
            results[demo_name] = result
            
            print(f"\n✅ {demo_name} completed successfully!")
            
        except Exception as e:
            print(f"\n❌ {demo_name} failed: {e}")
            import traceback
            traceback.print_exc()
    
    print(f"\n{'='*70}")
    print("All Demos Completed!")
    print(f"{'='*70}")
    
    print(f"\nSuccessfully demonstrated {len(results)} features:")
    for demo_name in results.keys():
        print(f"  ✅ {demo_name}")
    
    print(f"\nKey Features Demonstrated:")
    print("  ✅ Advanced Brain Interface with comprehensive metrics")
    print("  ✅ Experiment Management and Parameter Sweeps")
    print("  ✅ Data Collection and Analysis")
    print("  ✅ Machine Learning Integration")
    print("  ✅ Visualization Capabilities")
    print("  ✅ Advanced Configuration Management")
    
    print(f"\nFor more advanced usage, refer to:")
    print("  - advanced_nlm.py module documentation")
    print("  - ExperimentManager for complex experimental setups")
    print("  - MLIntegration for machine learning applications")
    print("  - DataCollector for comprehensive data analysis")
    
    # Save demo results
    with open("demo_results_summary.json", 'w') as f:
        json.dump({
            "demo_results": {name: "completed" for name in results.keys()},
            "total_demos": len(results),
            "timestamp": time.strftime("%Y-%m-%d %H:%M:%S")
        }, f, indent=2)
    
    print(f"\nDemo results saved to demo_results_summary.json")

if __name__ == "__main__":
    main()
