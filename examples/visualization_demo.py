"""Visualization NLM Demo - Advanced Neural Visualization

This example demonstrates advanced neural visualization capabilities including
real-time visualization, complex plotting, and interactive data analysis.
"""
import nlm
from nlm import expert
import time
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sns

def run_visualization_demo():
    """Run visualization NLM demonstration."""
    print("=== Visualization NLM Demo ===")
    print("This example demonstrates advanced neural visualization capabilities.")
    print("Features:")
    print("  - Advanced plotting and visualization")
    print("  - Real-time visualization pipelines")
    print("  - Complex visualization workflows")
    print("  - Multiple visualization formats")
    print("  - Interactive visualization analysis")
    print()
    
    # Setup
    print("1. Setting up visualization systems...")
    config = nlm.create_default_config()
    world = nlm.create_simple_world()
    world.configure(20, 20, 64, 64)
    world.setAgentStart(10, 10)
    
    brain = nlm.create_brain(config)
    brain.initialize(world)
    
    agent_brain = nlm.create_agent_brain(brain)
    agent_brain.initialize(brain, world)
    
    # Create visualization pipeline
    print("2. Creating visualization pipeline...")
    pipeline = expert.VisualizationPipeline()
    real_time_viz = expert.RealTimeVisualizer(pipeline)
    
    # Create visualization components
    print("3. Setting up visualization components...")
    
    # Data collection for visualization
    visualization_data = {
        'neuron_activity': [],
        'connection_density': [],
        'population_activity': [],
        'plasticity_events': [],
        'neuromodulation_effects': [],
        'simulation_steps': list(range(200))
    }
    
    # Create advanced plotter
    advanced_plotter = expert.AdvancedPlotting()
    
    # Create data exporter
    data_exporter = expert.DataExporter("hdf5")
    
    print("   - Visualization pipeline: initialized")
    print("   - Real-time visualizer: initialized")
    print("   - Advanced plotter: initialized")
    print("   - Data exporter: initialized")
    print()
    
    # Run simulation with comprehensive visualization
    print("4. Running simulation with visualization...")
    
    for step in range(200):
        # Get sensory input
        percept = world.getSensoryPercept()
        
        # Process with agent brain
        agent_brain.processSensoryInput(percept)
        action = agent_brain.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, step)
        world.update(0.001)
        
        # Collect data for visualization
        neurons_active = brain.getActiveNeuronCount()
        
        # Generate connection density plot
        density_plot = nlm.generate_connection_density_plot(brain)
        
        # Collect visualization data
        visualization_data['neuron_activity'].append(neurons_active)
        visualization_data['connection_density'].append(np.mean(density_plot))
        visualization_data['plasticity_events'].append(len(expert.CustomPlasticityRules()))
        visualization_data['neuromodulation_effects'].append(np.random.uniform(0.7, 0.9))
        
        # Add data to visualization pipeline
        pipeline.addStep("neuron_activity", visualization_data['neuron_activity'])
        pipeline.addStep("connection_density", visualization_data['connection_density'])
        pipeline.addStep("plasticity_events", visualization_data['plasticity_events'])
        
        # Progress output
        if step % 50 == 0:
            body = world.getAgentBody()
            print(f"   - Step {step}: {neurons_active} active neurons")
    
    print("\n5. Creating comprehensive visualizations...")
    
    # Create multi-panel visualization
    fig = plt.figure(figsize=(20, 16))
    
    # Create subplots
    axes = []
    for i in range(3, 10):
        ax = fig.add_subplot(3, 3, i)
        axes.append(ax)
    
    # Neuron activity over time
    axes[0].plot(visualization_data['neuron_activity'])
    axes[0].set_title('Neuron Activity Over Time')
    axes[0].set_xlabel('Step')
    axes[0].set_ylabel('Active Neurons')
    axes[0].grid(True, alpha=0.3)
    
    # Connection density over time
    axes[1].plot(visualization_data['connection_density'])
    axes[1].set_title('Connection Density Over Time')
    axes[1].set_xlabel('Step')
    axes[1].set_ylabel('Density')
    axes[1].grid(True, alpha=0.3)
    
    # Plasticity events over time
    axes[2].plot(visualization_data['plasticity_events'])
    axes[2].set_title('Plasticity Events Over Time')
    axes[2].set_xlabel('Step')
    axes[2].set_ylabel('Event Count')
    axes[2].grid(True, alpha=0.3)
    
    # Neuromodulation effects over time
    axes[3].plot(visualization_data['neuromodulation_effects'])
    axes[3].set_title('Neuromodulation Effects Over Time')
    axes[3].set_xlabel('Step')
    axes[3].set_ylabel('Strength')
    axes[3].grid(True, alpha=0.3)
    
    # Correlation matrix
    correlation_matrix = np.corrcoef([
        visualization_data['neuron_activity'],
        visualization_data['connection_density'],
        visualization_data['plasticity_events'],
        visualization_data['neuromodulation_effects']
    ])
    
    im = axes[4].imshow(correlation_matrix, cmap='coolwarm', vmin=-1, vmax=1)
    axes[4].set_title('Correlation Matrix')
    axes[4].set_xlabel('Variables')
    axes[4].set_ylabel('Variables')
    
    # Add colorbar
    cbar = plt.colorbar(im, ax=axes[4])
    cbar.set_label('Correlation Coefficient')
    
    # Heatmap of neural activity
    activity_heatmap = np.array(visualization_data['neuron_activity']).reshape(20, 10)
    im2 = axes[5].imshow(activity_heatmap, cmap='viridis', aspect='auto')
    axes[5].set_title('Neural Activity Heatmap')
    axes[5].set_xlabel('Time Block')
    axes[5].set_ylabel('Neuron Group')
    plt.colorbar(im2, ax=axes[5])
    
    # 3D visualization
    from mpl_toolkits.mplot3d import Axes3D
    ax3d = fig.add_subplot(3, 3, 6, projection='3d')
    
    x = np.arange(len(visualization_data['neuron_activity']))
    y = visualization_data['connection_density']
    z = visualization_data['neuromodulation_effects']
    
    scatter = ax3d.scatter(x, y, z, c=visualization_data['neuron_activity'], cmap='plasma', alpha=0.7)
    ax3d.set_title('3D Neural Activity Visualization')
    ax3d.set_xlabel('Time Step')
    ax3d.set_ylabel('Connection Density')
    ax3d.set_zlabel('Neuromodulation Strength')
    
    plt.colorbar(scatter, ax=ax3d)
    
    # Network graph visualization
    axes[7].plot(visualization_data['neuron_activity'], visualization_data['connection_density'], 
                marker='o', alpha=0.7, linewidth=2)
    axes[7].set_title('Neuron Activity vs Connection Density')
    axes[7].set_xlabel('Neuron Activity')
    axes[7].set_ylabel('Connection Density')
    axes[7].grid(True, alpha=0.3)
    
    # Time series forecast
    from sklearn.preprocessing import MinMaxScaler
    from sklearn.linear_model import LinearRegression
    
    # Prepare data for forecasting
    X = np.array(visualization_data['neuron_activity']).reshape(-1, 1)
    y = np.array(visualization_data['neuron_activity'])
    
    # Simple forecast (next 10 steps)
    forecast_steps = 10
    future_x = np.array(range(len(visualization_data['neuron_activity']), 
                                 len(visualization_data['neuron_activity']) + forecast_steps)).reshape(-1, 1))
    
    scaler = MinMaxScaler()
    X_scaled = scaler.fit_transform(X)
    future_x_scaled = scaler.transform(future_x)
    
    model = LinearRegression()
    model.fit(X_scaled, y)
    forecast = model.predict(future_x_scaled)
    
    axes[8].plot(visualization_data['neuron_activity'], label='Historical')
    axes[8].plot(range(len(visualization_data['neuron_activity']), len(visualization_data['neuron_activity']) + forecast_steps),
               forecast, 'r--', label='Forecast')
    axes[8].set_title('Neuron Activity Forecast')
    axes[8].set_xlabel('Step')
    axes[8].set_ylabel('Neuron Activity')
    axes[8].legend()
    axes[8].grid(True, alpha=0.3)
    
    # Advanced plotting with annotations
    axes[0].annotate(f'Max: {max(visualization_data["neuron_activity"])}',
                    xy=(np.argmax(visualization_data['neuron_activity']), max(visualization_data['neuron_activity'])),
                    xytext=(10, 10), textcoords='offset points',
                    arrowprops=dict(arrowstyle='->'))
    
    axes[1].annotate(f'Min: {min(visualization_data["connection_density"]):.3f}',
                    xy=(np.argmin(visualization_data['connection_density']), min(visualization_data['connection_density'])),
                    xytext=(10, 10), textcoords='offset points',
                    arrowprops=dict(arrowstyle='->'))
    
    plt.suptitle('Advanced Neural Visualization Demo', fontsize=16, fontweight='bold')
    plt.tight_layout()
    
    # Save visualization
    plt.savefig('visualization_demo_results.png', dpi=150, bbox_inches='tight')
    plt.show()
    
    print("   - Advanced visualization saved as 'visualization_demo_results.png'")
    print()
    
    # Create specialized visualization types
    print("6. Creating specialized visualizations...")
    
    # Time-frequency analysis
    from scipy import signal
    
    # Compute power spectral density
    fs = 1.0  # Sampling frequency
    f, Pxx = signal.welch_gtgr(visualization_data['neuron_activity'], fs)
    
    fig2, axes2 = plt.subplots(2, 2, figsize=(14, 10))
    
    # Power spectral density
    axes2[0, 0].semilogy(f, Pxx)
    axes2[0, 0].set_title('Power Spectral Density')
    axes2[0, 0].set_xlabel('Frequency (Hz)')
    axes2[0, 0].set_ylabel('PSD')
    axes2[0, 0].grid(True, alpha=0.3)
    
    # Wavelet transform
    scales = np.arange(1, 32)
    coefficients = signal.cwt(visualization_data['neuron_activity'], signal.ricker, scales)
    
    axes2[0, 1].imshow(coefficients, extent=[0, len(visualization_data['neuron_activity']), scales[0], scales[-1]],
                     aspect='auto', cmap='RdBu_r')
    axes2[0, 1].set_title('Wavelet Transform')
    axes2[0, 1].set_xlabel('Time')
    axes2[0, 1].set_ylabel('Scale')
    
    # Entropy calculation
    from scipy import entropy
    entropy_values = []
    
    for window in np.lib.stride_tricks(visualization_data['neuron_activity'], 10):
        entropy_values.append(entropy(window))
    
    axes2[1, 0].plot(entropy_values)
    axes2[1, 0].set_title('Entropy Over Time')
    axes2[1, 0].set_xlabel('Window')
    axes2[1, 0].set_ylabel('Entropy')
    axes2[1, 0].grid(True, alpha=0.3)
    
    # Complex plot
    complex_data = visualization_data['neuron_activity'] + 1j * visualization_data['connection_density']
    magnitude = np.abs(complex_data)
    phase = np.angle(complex_data)
    
    axes2[1, 1].plot(magnitude, label='Magnitude')
    axes2[1, 1].plot(phase, label='Phase')
    axes2[1, 1].set_title('Complex Signal Analysis')
    axes2[1, 1].set_xlabel('Time')
    axes2[1, 1].set_ylabel('Value')
    axes2[1, 1].legend()
    axes2[1, 1].grid(True, alpha=0.3)
    
    plt.tight_layout()
    plt.savefig('visualization_advanced.png', dpi=150)
    plt.show()
    
    print("   - Advanced visualization saved as 'visualization_advanced.png'")
    print()
    
    # Data export
    print("7. Exporting visualization data...")
    export_filename = f"visualization_data_{int(time.time())}.h5"
    data_exporter.exportData({
        'neuron_activity': visualization_data['neuron_activity'],
        'connection_density': visualization_data['connection_density'],
        'plasticity_events': visualization_data['plasticity_events'],
        'neuromodulation_effects': visualization_data['neuromodulation_effects'],
        'simulation_steps': visualization_data['simulation_steps']
    }, export_filename)
    
    print(f"   - Data exported to: {export_filename}")
    print()
    
    # Visualization summary
    print("8. Visualization Summary:")
    print(f"   - Neuron activity range: {min(visualization_data['neuron_activity'])} - {max(visualization_data['neuron_activity'])}")
    print(f"   - Connection density range: {min(visualization_data['connection_density']):.3f} - {max(visualization_data['connection_density']):.3f}")
    print(f"   - Plasticity events: {len(visualization_data['plasticity_events'])}")
    print(f"   - Neuromodulation range: {min(visualization_data['neuromodulation_effects']):.3f} - {max(visualization_data['neuromodulation_effects']):.3f}")
    print(f"   - Visualization components: Neuron Activity, Connection Density, Plasticity, Neuromodulation")
    print(f"   - Export format: HDF5")
    print(f"   - Specialized visualizations: Spectral analysis, Wavelet transform, Entropy analysis")
    print()
    
    return {
        'visualization_data': visualization_data,
        'export_filename': export_filename,
        'specialized_visualizations': ['spectral_analysis', 'wavelet_transform', 'entropy_analysis', 'complex_analysis'],
        'visualization_components': ['time_series', 'correlation_matrix', 'heatmap', '3d_visualization', 'forecast'],
        'data_export': 'HDF5 format'
    }

if __name__ == "__main__":
    print("NLM Visualization Demo - Advanced Neural Visualization")
    print("=" * 75)
    print()
    
    results = run_visualization_demo()
    
    print("Visualization demo completed successfully!")
    print("\nResults are available in:")
    print("  - 'visualization_demo_results.png' - Multi-panel visualization")
    print("  - 'visualization_advanced.png' - Advanced visualization techniques")
    print("  - 'visualization_data_*.h5' - Exported visualization data")
    print("\nFor more examples, see:")
    print("  - examples/basic_demo.py - Basic neural network simulation")
    print("  - examples/expert_demo.py - Advanced expert features")
    print("  - examples/performance_demo.py - Performance optimization")
    print("  - examples/research_demo.py - Research applications")
    print("  - examples/production_demo.py - Production deployment")
