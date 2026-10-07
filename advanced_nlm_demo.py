#!/usr/bin/env python3
"""
Advanced NLM Demo Script

This script demonstrates the advanced features of the NLM Python API,
including advanced brain simulation, experiment management, and analysis.
"""

import advanced_nlm
import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns
from typing import Dict, Any

def main_demo():
    """Run the main advanced NLM demo."""
    print("=" * 60)
    print("Advanced NLM Demo")
    print("=" * 60)
    
    # Create advanced brain
    print("\n1. Creating Advanced Brain...")
    brain = advanced_nlm.createAdvancedBrain()
    
    # Enable advanced features
    print("2. Enabling Advanced Features...")
    brain.enable_advanced_features(
        reward_modulation=True,
        structural_plasticity=True,
        development=True,
        curiosity=True,
        prediction=True
    )
    
    # Run basic dynamics
    print("3. Running Basic Dynamics...")
    metrics = brain.run_dynamics(500, collect_metrics=True)
    
    print(f"   Total spikes: {brain.get_stats()['total_spikes']}")
    print(f"   Firing neurons: {brain.get_stats()['firing_neurons']}")
    print(f"   Average firing rate: {brain.get_stats()['average_firing_rate']:.2f} Hz")
    
    # Analyze activity patterns
    print("\n4. Analyzing Activity Patterns...")
    patterns = brain.get_activity_pattern(100)
    print(f"   Stability index: {patterns.get('stability_index', 0):.3f}")
    print(f"   Exploration index: {patterns.get('exploration_index', 0):.3f}")
    print(f"   Learning trend: {patterns.get('learning_curve', []).count('increasing')} increasing, {patterns.get('learning_curve', []).count('decreasing')} decreasing")
    
    # Create experiment manager
    print("\n5. Setting up Experiment Manager...")
    experiment_manager = advanced_nlm.ExperimentManager()
    
    # Create parameter sweep experiment
    print("6. Creating Parameter Sweep Experiment...")
    sweep_config = experiment_manager.create_parameter_sweep(
        'brain.neuron_count',
        [500, 1000, 1500],
        steps=100
    )
    
    # Run experiment (this would normally run multiple simulations)
    print("7. Running Experiment...")
    # For demo purposes, we'll just create the sweep config
    print("   Parameter sweep configured for: neuron_count values 500, 1000, 1500")
    
    # Data collection demo
    print("\n8. Demonstrating Data Collection...")
    data_collector = advanced_nlm.DataCollector()
    
    # Collect data from current brain
    data_collector.collect_data(brain, "advanced_demo", {
        'simulation_type': 'advanced_demo',
        'features_enabled': ['reward_modulation', 'structural_plasticity']
    })
    
    # Apply filter
    data_collector.apply_filter(lambda x: x['brain_stats']['total_spikes'])
    
    # Process data
    data_collector.process_data()
    
    summary = data_collector.get_summary_statistics()
    print(f"   Total experiments: {summary.get('total_experiments', 0)}")
    print(f"   Average spikes per experiment: {summary.get('avg_spikes_per_experiment', 0):.1f}")
    
    # Visualization demo (if matplotlib is available)
    print("\n9. Creating Visualizations...")
    try:
        # Spike histogram
        spike_histogram = advanced_nlm.VisualProfiler.create_spike_histogram(
            list(range(0, 1000, 50)),  # Sample spike data
            bins=10
        )
        print(f"   Spike histogram created with {len(spike_histogram['bins'])} bins")
        
        # Activity pattern analysis
        activity_analysis = advanced_nlm.VisualProfiler.analyze_activity_patterns(metrics)
        print(f"   Activity analysis: mean firing rate = {activity_analysis.get('mean_firing_rate', 0):.2f} Hz")
        
    except ImportError:
        print("   Skipping visualizations (matplotlib not available)")
    
    # Machine learning integration demo
    print("\n10. Demonstrating ML Integration...")
    ml_integration = advanced_nlm.MLIntegration()
    
    # Create sample data for ML demonstration
    np.random.seed(42)
    X = np.random.randn(100, 5)  # 100 samples, 5 features
    y = np.random.randint(0, 2, 100)  # Binary labels
    
    # Train model
    ml_model = ml_integration.train_supervised_model(X, y, 'random_forest')
    print(f"   ML model trained: {ml_model}")
    
    # Make predictions
    predictions = ml_integration.predict_with_model('random_forest', X[:5])
    print(f"   Sample predictions: {predictions}")
    
    print("\n" + "=" * 60)
    print("Advanced NLM Demo Complete!")
    print("=" * 60)
    
    print("\nKey Features Demonstrated:")
    print("✅ Advanced Brain Interface with comprehensive metrics")
    print("✅ Experiment Management and Parameter Sweeps")
    print("✅ Data Collection and Processing")
    print("✅ Machine Learning Integration")
    print("✅ Activity Pattern Analysis")
    print("✅ Visualization Capabilities")
    
    print("\nFor more advanced usage, refer to:")
    print("- advanced_nlm.py module documentation")
    print("- ExperimentManager for complex experimental setups")
    print("- MLIntegration for machine learning applications")
    print("- DataCollector for comprehensive data analysis")

if __name__ == "__main__":
    main_demo()
