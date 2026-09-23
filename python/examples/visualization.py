"""
NLM Visualization Integration Examples

This example demonstrates various visualization and monitoring integration
techniques for NLM neural simulations.
"""

from pynlm import AdvancedBrain, Config, World, create_simulation
import matplotlib.pyplot as plt
import numpy as np


def basic_visualization():
    """Basic visualization setup and usage."""
    print("=== Basic Visualization Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    brain.configure_simulation(
        max_steps=50,
        enable_visualization=True,
        visualization_backend='matplotlib'
    )
    
    # Setup visualization
    if brain.visualization.setup_plotting('matplotlib'):
        print("✓ Matplotlib visualization backend setup successful")
        
        # Create brain network plot
        network_plot = brain.visualization.create_brain_network_plot(brain)
        if network_plot:
            print("✓ Brain network plot created")
            plt.show()
        
        # Create training monitor
        monitor = brain.visualization.create_training_monitor(brain)
        if monitor:
            print("✓ Training monitoring interface created")
            plt.show()
    else:
        print("✗ Visualization setup failed")
    
    print(f"Monitoring system status: {brain.monitoring.get_performance_stats()}")


def real_time_monitoring():
    """Real-time monitoring during simulation."""
    print("\n=== Real-Time Monitoring Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    brain.configure_simulation(
        max_steps=100,
        enable_visualization=True,
        enable_monitoring=True,
        monitoring_interval=0.2
    )
    
    # Setup plotting
    brain.visualization.setup_plotting('matplotlib')
    
    print("Starting simulation with real-time monitoring...")
    
    # Custom callback to update visualization
    def real_time_callback(state):
        if state.step % 10 == 0:
            print(f"  📊 Step {state.step}: "
                  f"Neurons={state.neurons_active}, "
                  f"Firing={state.neurons_firing}, "
                  f"Reward={state.reward:.2f}")
    
    brain.add_callback('step', real_time_callback)
    
    # Run simulation
    results = brain.run_episode(max_steps=50, verbose=True)
    
    print(f"\nReal-time monitoring completed:")
    print(f"  Total steps: {results['steps_completed']}")
    print(f"  Average reward: {results['total_reward']/results['steps_completed']:.2f}")
    print(f"  Monitoring alerts: {results['metrics']['recent_alerts']} recent")


def comparative_visualization():
    """Comparative visualization across different configurations."""
    print("\n=== Comparative Visualization Example ===\n")
    
    # Test different simulation configurations
    configurations = [
        {'name': 'Fast', 'max_steps': 50, 'simulation_speed': 2.0},
        {'name': 'Medium', 'max_steps': 100, 'simulation_speed': 1.0},
        {'name': 'Slow', 'max_steps': 150, 'simulation_speed': 0.5}
    ]
    
    results_summary = []
    
    for config in configurations:
        print(f"Running {config['name']} simulation...")
        
        brain = AdvancedBrain(verbose=False)
        brain.configure_simulation(
            max_steps=config['max_steps'],
            simulation_speed=config['simulation_speed'],
            enable_monitoring=True
        )
        
        # Setup visualization
        brain.visualization.setup_plotting('matplotlib')
        
        # Run episode
        episode_results = brain.run_episode(
            max_steps=config['max_steps'], 
            verbose=False
        )
        
        results_summary.append({
            'name': config['name'],
            'steps_completed': episode_results['steps_completed'],
            'total_reward': episode_results['total_reward'],
            'success': episode_results['success'],
            'metrics': brain.monitoring.get_performance_stats()
        })
    
    print(f"\nComparative Results:")
    print(f"{'Config':<10} {'Steps':<8} {'Reward':<12} {'Success':<8} {'Time':<8}")
    print(f"{'='*50}")
    
    for result in results_summary:
        time_taken = result['metrics'].get('total_simulation_time', 0)
        print(f"{result['name']:<10} {result['steps_completed']:<8} "
              f"{result['total_reward']:<12.2f} {result['success']:<8} {time_taken:<8.2f}")
    
    # Create comparative plot (would need matplotlib for actual plotting)
    print(f"\nVisualization: Would create comparative plots showing:")
    print(f"  - Reward progression across configurations")
    print(f"  - Performance comparison")
    print(f"  - Monitoring metrics overlay")


def interactive_visualization():
    """Interactive visualization with user input."""
    print("\n=== Interactive Visualization Example ===\n")
    
    brain = AdvancedBrain(verbose=True)
    brain.configure_simulation(
        max_steps=100,
        enable_visualization=True,
        enable_monitoring=True
    )
    
    brain.visualization.setup_plotting('matplotlib')
    
    print("Starting interactive visualization session...")
    print("Use matplotlib controls to:")
    print("  - Zoom in/out on brain network")
    print("  - Pan to explore different regions")
    print("  - Click on neurons to see details")
    print("  - Check performance in real-time\n")
    
    # Interactive callback
    def interactive_callback(state):
        if state.step == 0:
            print("  👋 Interactive mode started")
        elif state.step % 25 == 0:
            print(f"  🎮 Step {state.step}: Monitor neurons and rewards")
        
        # Check for any alerts
        alerts = brain.monitoring.alerts
        if alerts:
            print(f"  ⚠️  {len(alerts)} active alerts detected")
    
    brain.add_callback('step', interactive_callback)
    
    # Run simulation
    results = brain.run_episode(max_steps=40, verbose=True)
    
    print(f"\nInteractive session completed:")
    print(f"  Steps: {results['steps_completed']}")
    print(f"  Success: {results['success']}")
    print(f"  Final monitoring status:")
    
    # Show final visualization state
    if brain.visualization.plotter:
        print(f"  ✓ Visualization ready for further interaction")
    
    # Generate final report
    report = brain.monitoring.generate_report()
    print(f"\nFinal Performance Report:\n{report}")


def visualization_backend_comparison():
    """Compare different visualization backends."""
    print("\n=== Visualization Backend Comparison ===\n")
    
    backends = ['matplotlib', 'plotly', 'bokeh']
    brain_config = Config(max_steps=30, enable_monitoring=True)
    
    results = []
    
    for backend in backends:
        print(f"Testing {backend} backend...")
        
        brain = AdvancedBrain(config=brain_config, verbose=False)
        brain.configure_simulation(
            visualization_backend=backend,
            enable_visualization=True
        )
        
        # Try to setup backend
        success = brain.visualization.setup_plotting(backend)
        
        results.append({
            'backend': backend,
            'setup_successful': success,
            'metrics': brain.monitoring.get_performance_stats() if success else None
        })
        
        if success:
            print(f"  ✓ {backend} setup successful")
        else:
            print(f"  ✗ {backend} setup failed (likely not installed)")
    
    print(f"\nBackend Comparison Results:")
    print(f"{'Backend':<10} {'Setup':<10} {'Status':<15}")
    print(f"{'='*40}")
    
    for result in results:
        status = "✓ Available" if result['setup_successful'] else "✗ Not Available"
        print(f"{result['backend']:<10} {str(result['setup_successful']):<10} {status:<15}")
    
    print(f"\nRecommendation:")
    available_backends = [r['backend'] for r in results if r['setup_successful']]
    if available_backends:
        print(f"  Available backends: {', '.join(available_backends)}")
        print(f"  Consider using: {available_backends[0]}")
    else:
        print(f"  No visualization backends available - install matplotlib, plotly, or bokeh")


def main():
    """Run all visualization examples."""
    print("NLM Visualization Examples")
    print("=" * 50)
    print()
    
    basic_visualization()
    real_time_monitoring()
    comparative_visualization()
    interactive_visualization()
    visualization_backend_comparison()
    
    print("\n" + "=" * 50)
    print("All visualization examples completed!")


if __name__ == "__main__":
    main()