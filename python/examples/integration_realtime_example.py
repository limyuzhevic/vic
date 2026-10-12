#!/usr/bin/env python3
"""
Integration and Real-Time Monitoring Example for Enhanced NLM Python Bindings

This script demonstrates real-time monitoring and integration capabilities
of the enhanced NLM Python bindings, including live visualization and data collection.
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm
import pandas as pd
import numpy as np
import time
import threading
from queue import Queue, Empty
import matplotlib.pyplot as plt

def data_collector(brain_manager, data_queue, stop_event, interval=0.1):
    """
    Background thread for collecting data from brain manager.
    
    Args:
        brain_manager: BrainManager instance
        data_queue: Queue for storing collected data
        stop_event: Event to signal thread termination
        interval: Collection interval in seconds
    """
    step = 0
    while not stop_event.is_set():
        # Collect metrics
        metrics = brain_manager.get_metrics()
        metrics['step'] = step
        metrics['timestamp'] = time.time()
        
        # Add to queue
        data_queue.put(metrics)
        
        # Increment step
        step += 1
        
        # Sleep for interval
        time.sleep(interval)
    
    print("   Data collector thread terminated")
def real_time_monitor(brain_manager, data_queue, stop_event, plot_interval=1.0):
    """
    Background thread for real-time monitoring and visualization.
    
    Args:
        brain_manager: BrainManager instance
        data_queue: Queue for data to display
        stop_event: Event to signal thread termination
        plot_interval: Plot update interval in seconds
    """
    last_update = time.time()
    
    while not stop_event.is_set():
        current_time = time.time()
        
        # Check if it's time to update plot
        if current_time - last_update >= plot_interval:
            try:
                # Create real-time plot
                fig, axes = plt.subplots(2, 2, figsize=(12, 8))
                
                # Collect data from queue
                data_points = []
                while True:
                    try:
                        data_points.append(data_queue.get_nowait())
                    except Empty:
                        break
                
                if data_points:
                    # Convert to DataFrame
                    df = pd.DataFrame(data_points)
                    
                    # Plot 1: Spike count over time
                    axes[0, 0].plot(df['step'], df['total_spike_count'], label='Total Spike Count', color='blue')
                    axes[0, 0].set_title('Spike Count Over Time')
                    axes[0, 0].set_xlabel('Step')
                    axes[0, 0].set_ylabel('Spike Count')
                    axes[0, 0].legend()
                    axes[0, 0].grid(True, alpha=0.3)
                    
                    # Plot 2: Active neurons over time
                    axes[0, 1].plot(df['step'], df['active_neuron_count'], label='Active Neurons', color='green')
                    axes[0, 1].set_title('Active Neurons Over Time')
                    axes[0, 1].set_xlabel('Step')
                    axes[0, 1].set_ylabel('Neuron Count')
                    axes[0, 1].legend()
                    axes[0, 1].grid(True, alpha=0.3)
                    
                    # Plot 3: Firing rate over time
                    axes[1, 0].plot(df['step'], df['average_firing_rate'], label='Average Firing Rate', color='red')
                    axes[1, 0].set_title('Firing Rate Over Time')
                    axes[1, 0].set_xlabel('Step')
                    axes[1, 0].set_ylabel('Firing Rate')
                    axes[1, 0].legend()
                    axes[1, 0].grid(True, alpha=0.3)
                    
                    # Plot 4: Neurons vs Synapses ratio
                    if 'total_synapse_count' in df.columns:
                        axes[1, 1].plot(df['step'], df['total_spike_count'] / (df['total_synapse_count'] + 1), 
                                      label='Spike/Synapse Ratio', color='purple')
                        axes[1, 1].set_title('Spike/Synapse Ratio Over Time')
                        axes[1, 1].set_xlabel('Step')
                        axes[1, 1].set_ylabel('Ratio')
                        axes[1, 1].legend()
                        axes[1, 1].grid(True, alpha=0.3)
                    
                    # Adjust layout
                    plt.tight_layout()
                    
                    # Save plot
                    plt.savefig('real_time_monitor.png', dpi=100, bbox_inches='tight')
                    
                    # Display plot
                    plt.show()
                    
                    # Clear plot to avoid memory issues
                    plt.clf()
                    plt.close(fig)
                    
                    print("   Real-time plot updated")
                
                last_update = current_time
                
            except Exception as e:
                print(f"   Error in real-time monitor: {e}")
                time.sleep(0.5)
    
    print("   Real-time monitor thread terminated")
def research_session_manager(brain_manager, session_configs):
    """
    Manage multiple research sessions.
    
    Args:
        brain_manager: BrainManager instance
        session_configs: List of session configuration dictionaries
    """
    sessions = []
    
    for config in session_configs:
        # Create research session
        session = pynlm.ResearchSession(brain_manager)
        
        # Configure session
        session.setSaveOnExit(config.get('save_on_exit', False))
        session.setExitPath(config.get('exit_path', f"session_{len(sessions)}.bin"))
        
        # Start session
        session.__enter__()
        sessions.append(session)
        
        print(f"   ✓ Session {len(sessions)} started")
    
    # Run sessions
    for i, session in enumerate(sessions):
        print(f"   Running session {i + 1}...")
        
        for step in range(config.get('steps', 100)):
            session.run_step(step)
            
            # Collect metrics every 10 steps
            if step % 10 == 0:
                metrics = session.collect_metrics()
                print(f"     Session {i + 1}, Step {step}: "
                      f"active_neurons={metrics.get('active_neuron_count', 0)}, "
                      f"firing_rate={metrics.get('average_firing_rate', 0):.3f}")
        
        # End session
        session.__exit__(None, None, None)
        print(f"   ✓ Session {i + 1} completed")
    
    return sessions
def main():
    print("=== Enhanced NLM Python Bindings - Integration & Real-Time Monitoring Example ===\n")
    
    # Example 1: Research Session Manager
    print("1. Research Session Manager...")
    
    # Create brain manager
    brain_manager = pynlm.BrainManager()
    brain_manager.set_learning_rate(0.01)
    brain_manager.set_neuron_count(1000)
    
    # Define session configurations
    session_configs = [
        {'steps': 100, 'save_on_exit': True, 'exit_path': 'session1.bin'},
        {'steps': 200, 'save_on_exit': False, 'exit_path': 'session2.bin'},
        {'steps': 150, 'save_on_exit': True, 'exit_path': 'session3.bin'}
    ]
    
    # Manage research sessions
    sessions = research_session_manager(brain_manager, session_configs)
    print(f"   ✓ All {len(sessions)} sessions completed")
    
    # Example 2: Real-Time Monitoring
    print("\n2. Real-Time Monitoring...")
    
    # Create new brain manager for real-time monitoring
    real_time_brain = pynlm.BrainManager()
    real_time_brain.set_learning_rate(0.02)
    real_time_brain.set_neuron_count(500)
    
    # Initialize real-time brain
    real_time_brain.initialize()
    
    # Create data queue and stop event
    data_queue = Queue()
    stop_event = threading.Event()
    
    # Start data collector thread
    collector_thread = threading.Thread(
        target=data_collector,
        args=(real_time_brain, data_queue, stop_event, 0.5),
        daemon=True
    )
    collector_thread.start()
    print("   ✓ Data collector thread started")
    
    # Start real-time monitor thread
    monitor_thread = threading.Thread(
        target=real_time_monitor,
        args=(real_time_brain, data_queue, stop_event, 2.0),
        daemon=True
    )
    monitor_thread.start()
    print("   ✓ Real-time monitor thread started")
    
    # Run simulation with real-time monitoring
    print("   Running simulation with real-time monitoring...")
    for step in range(100):
        real_time_brain.runExperiment(steps=1, record_metrics=False)
        time.sleep(0.5)  # Small delay to control simulation speed
        
        if step % 20 == 0:
            print(f"     Step {step} completed")
    
    # Stop monitoring
    stop_event.set()
    collector_thread.join(timeout=5)
    monitor_thread.join(timeout=5)
    
    print("   ✓ Real-time monitoring completed")
    
    # Example 3: Integration with Experiment Manager
    print("\n3. Integration with Experiment Manager...")
    
    # Create experiment manager
    exp_manager = pynlm.ExperimentManager(real_time_brain)
    
    # Run multiple experiments
    experiments = []
    
    for i in range(3):
        print(f"   Running experiment {i + 1}...")
        
        # Configure experiment
        exp_name = f"integration_test_{i + 1}"
        steps = 200 + i * 100
        learning_rate = 0.01 * (i + 1)
        
        # Run experiment
        result = exp_manager.conduct_learning_experiment(
            experiment_name=exp_name,
            steps=steps,
            learning_rate=learning_rate,
            neuron_count=1000 + i * 500,
            record_step_data=True
        )
        
        experiments.append(result)
        print(f"     ✓ {exp_name} completed: {result.experiment_name}")
    
    # Compare experiment results
    print("\n   Experiment Comparison:")
    for i, result in enumerate(experiments):
        metrics = result.metrics
        print(f"     Experiment {i + 1}: total_spikes={metrics.get('total_spike_count', 0)}, "
              f"avg_firing_rate={metrics.get('average_firing_rate', 0):.3f}")
    
    # Example 4: Real-Time Data Export
    print("\n4. Real-Time Data Export...")
    
    # Create exporter
    exporter = pynlm.create_pandas_exporter()
    
    # Generate sample data for export
    export_data = pd.DataFrame({
        'step': range(100),
        'total_spike_count': np.random.randint(100, 1000, 100),
        'active_neuron_count': np.random.randint(50, 500, 100),
        'average_firing_rate': np.random.uniform(0, 1, 100),
        'learning_rate': np.random.uniform(0.001, 0.1, 100),
        'neuron_count': np.full(100, 1000),
        'memory_utilization': np.random.uniform(0.5, 1.0, 100),
        'energy_consumption': np.random.uniform(0.3, 0.9, 100),
        'stability': np.random.uniform(0.6, 1.0, 100)
    })
    
    # Export data
    exporter.export_to_csv(export_data, 'real_time_data.csv')
    exporter.export_to_json(export_data, 'real_time_data.json')
    exporter.export_to_excel(export_data, 'real_time_data.xlsx')
    
    print("   ✓ Real-time data exported to CSV, JSON, and Excel")
    
    # Example 5: Statistical Analysis of Real-Time Data
    print("\n5. Statistical Analysis of Real-Time Data...")
    
    # Create statistical analysis
    analysis = pynlm.create_statistical_analysis(export_data)
    
    # Describe data
    print("   Data Description:")
    describe_results = analysis.describe()
    for column in ['total_spike_count', 'active_neuron_count', 'average_firing_rate']:
        stats = describe_results[column]
        print(f"     {column}: mean={stats.mean:.1f}, std={stats.std:.1f}, min={stats.min:.0f}, max={stats.max:.0f}")
    
    # Perform correlation analysis
    print("\n   Correlation Analysis:")
    for var1, var2 in [('learning_rate', 'average_firing_rate'),
                      ('neuron_count', 'memory_utilization'),
                      ('energy_consumption', 'stability')]:
        corr_results = analysis.correlation_analysis(export_data[var1], export_data[var2])
        print(f"     {var1} vs {var2}: r={corr_results.correlation:.3f}, p={corr_results.p_value:.3f}")
    
    # Perform t-tests
    print("\n   T-Test Analysis:")
    # Create synthetic groups
    group1 = export_data['learning_rate'].head(50)
    group2 = export_data['learning_rate'].tail(50)
    
    t_test_results = analysis.t_test(group1, group2)
    print(f"     Low vs High Learning Rate: t={t_test_results['t_statistic']:.3f}, p={t_test_results['p_value']:.3f}")
    print(f"     Significant difference: {t_test_results['significant']}")
    
    # Generate report
    print("\n   Statistical Report:")
    report = analysis.generate_report()
    print(report[:1000] + "...")
    
    print("\n=== Integration & Real-Time Monitoring Example Completed Successfully ===\n")
    
    return True
if __name__ == "__main__":
    try:
        success = main()
        if success:
            print("\nIntegration and real-time monitoring example completed successfully!")
            sys.exit(0)
        else:
            print("\nIntegration and real-time monitoring example failed!")
            sys.exit(1)
    except Exception as e:
        print(f"\nError: {e}")
        sys.exit(1)