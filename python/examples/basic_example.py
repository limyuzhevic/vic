#!/usr/bin/env python3
"""
Basic Usage Example for Enhanced NLM Python Bindings

This script demonstrates the basic usage of the enhanced NLM Python bindings,
showing how to create brain managers, run experiments, and visualize results.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import time
import pandas as pd
import matplotlib.pyplot as plt

def main():
    print("=== Enhanced NLM Python Bindings - Basic Usage Example ===\n")
    
    # Example 1: Basic Brain Manager Usage
    print("1. Creating Brain Manager...")
    brain = pynlm.BrainManager()
    print("   ✓ BrainManager created")
    
    # Configure brain
    brain.set_learning_rate(0.01)
    brain.set_neuron_count(1000)
    print("   ✓ Brain configured (learning_rate=0.01, neuron_count=1000)")
    
    # Example 2: Running Experiment
    print("\n2. Running Experiment...")
    start_time = time.time()
    results = brain.run_experiment(steps=500)
    end_time = time.time()
    
    print(f"   ✓ Experiment completed in {end_time - start_time:.2f} seconds")
    print(f"   ✓ Total spikes: {results.metrics.get('total_spike_count', 'N/A')}")
    print(f"   ✓ Active neurons: {results.metrics.get('active_neuron_count', 'N/A')}")
    
    # Example 3: Visualization
    print("\n3. Creating Visualizations...")
    
    # Create visualizer
    visualizer = pynlm.create_matplotlib_visualizer(brain)
    
    # Generate plots
    visualizer.plot_neural_activity()
    visualizer.plot_memory_metrics()
    visualizer.plot_learning_progress()
    
    print("   ✓ Neural activity plot created")
    print("   ✓ Memory metrics plot created")
    print("   ✓ Learning progress plot created")
    
    # Example 4: Save Results
    print("\n4. Saving Results...")
    
    # Save brain state
    brain.save_state("experiment_results.bin")
    print("   ✓ Brain state saved to experiment_results.bin")
    
    # Create sample data for analysis
    sample_data = pd.DataFrame({
        'experiment': ['test1', 'test2', 'test1', 'test2'] * 250,
        'performance': [0.8, 0.9, 0.75, 0.85] * 250,
        'memory_usage': [0.6, 0.7, 0.65, 0.75] * 250,
        'learning_rate': [0.01, 0.05, 0.01, 0.05] * 250
    })
    
    # Export data
    exporter = pynlm.create_pandas_exporter()
    exporter.export_to_csv(sample_data, "sample_data.csv")
    print("   ✓ Sample data exported to sample_data.csv")
    
    # Example 5: Statistical Analysis
    print("\n5. Statistical Analysis...")
    
    # Create statistical analysis
    analysis = pynlm.create_statistical_analysis(sample_data)
    
    # Describe data
    print("   Data Summary:")
    describe_results = analysis.describe()
    for column, stats in describe_results.items():
        print(f"     {column}: mean={stats.mean:.3f}, std={stats.std:.3f}")
    
    # Perform correlation analysis
    corr_results = analysis.correlation_analysis(sample_data['learning_rate'], sample_data['performance'])
    print(f"   Learning Rate vs Performance correlation: {corr_results.correlation:.3f}")
    
    # Example 6: Research Session
    print("\n6. Research Session Management...")
    
    # Create research session
    with pynlm.create_research_session(brain) as session:
        for step in range(100):
            session.run_step(step)
            
            if step % 25 == 0:
                metrics = session.collect_metrics()
                print(f"     Step {step}: active_neurons={metrics.get('active_neuron_count', 0)}, firing_rate={metrics.get('average_firing_rate', 0):.3f}")
        
        # Get results
        results = session.get_results()
        print(f"     Final step count: {results['step_count']}")
        print(f"     Session active: {results['session_active']}")
    
    print("\n=== Example Completed Successfully ===\n")
    
    # Display plots
    print("Displaying plots...")
    visualizer.show_all_plots()
    
    return True
if __name__ == "__main__":
    try:
        success = main()
        if success:
            print("\nAll examples completed successfully!")
            sys.exit(0)
        else:
            print("\nExample failed!")
            sys.exit(1)
    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)