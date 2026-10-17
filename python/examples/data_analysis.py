#!/usr/bin/env python3
"""
NLM Python Data Analysis Examples

This script demonstrates data analysis capabilities with NLM Python bindings,
including statistical analysis, visualization, and machine learning integration.
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import pandas as pd
from scipy import stats
from datetime import datetime

def example_neural_activity_analysis():
    """Example: Analyze neural activity patterns"""
    print("=" * 60)
    print("Neural Activity Analysis")
    print("=" * 60)
    
    # Create brain and run simulation
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 1500)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation to collect activity data
    print("Running simulation to collect activity data...")
    for i in range(3000):
        brain.step(i)
    
    # Collect activity statistics over time
    print("Collecting activity statistics...")
    
    # Simulate collecting time-series data (in real implementation, this would come from brain)
    time_steps = 100
    time_points = np.linspace(0, 3000, time_steps)
    
    # Generate synthetic activity data (in real usage, this would be from brain)
    firing_rates = []
    total_spikes = []
    
    for i in range(time_steps):
        # In real implementation, this would be actual brain data
        # For demonstration, we'll use simulated data
        firing_rate = 50 + 30 * np.sin(i * 0.1) + np.random.normal(0, 5)
        spike_count = int(firing_rate * 0.1) + np.random.poisson(10)
        
        firing_rates.append(firing_rate)
        total_spikes.append(spike_count)
    
    # Create DataFrame for analysis
    activity_df = pd.DataFrame({
        'time_step': time_points,
        'firing_rate': firing_rates,
        'spike_count': total_spikes
    })
    
    # Statistical analysis
    print("\nStatistical Analysis:")
    print(f"  Mean firing rate: {activity_df['firing_rate'].mean():.2f} Hz")
    print(f"  Median firing rate: {activity_df['firing_rate'].median():.2f} Hz")
    print(f"  Firing rate std: {activity_df['firing_rate'].std():.2f} Hz")
    print(f"  Max firing rate: {activity_df['firing_rate'].max():.2f} Hz")
    print(f"  Min firing rate: {activity_df['firing_rate'].min():.2f} Hz")
    
    # Normal distribution fit
    z_scores = stats.zscore(activity_df['firing_rate'])
    outliers = np.sum(np.abs(z_scores) > 2)
    print(f"  Potential outliers (z > 2): {outliers}")
    
    # Calculate burst detection
    burst_threshold = activity_df['firing_rate'].quantile(0.95)
    bursts = activity_df[activity_df['firing_rate'] > burst_threshold]
    print(f"  Burst threshold (95th percentile): {burst_threshold:.2f} Hz")
    print(f"  Number of burst events: {len(bursts)}")
    
    # Create visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 10))
    
    # Plot 1: Firing rate over time
    axes[0, 0].plot(activity_df['time_step'], activity_df['firing_rate'], 'b-', linewidth=1, alpha=0.7)
    axes[0, 0].axhline(y=burst_threshold, color='r', linestyle='--', label=f'Burst threshold ({burst_threshold:.1f} Hz)')
    axes[0, 0].set_title('Firing Rate Over Time')
    axes[0, 0].set_xlabel('Time Step')
    axes[0, 0].set_ylabel('Firing Rate (Hz)')
    axes[0, 0].legend()
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Histogram of firing rates
    axes[0, 1].hist(activity_df['firing_rate'], bins=30, alpha=0.7, color='skyblue', edgecolor='black')
    axes[0, 1].axvline(x=activity_df['firing_rate'].mean(), color='r', linestyle='--', label=f'Mean ({activity_df["firing_rate"].mean():.1f} Hz)')
    axes[0, 1].axvline(x=burst_threshold, color='g', linestyle='--', label=f'95th percentile ({burst_threshold:.1f} Hz)')
    axes[0, 1].set_title('Firing Rate Distribution')
    axes[0, 1].set_xlabel('Firing Rate (Hz)')
    axes[0, 1].set_ylabel('Frequency')
    axes[0, 1].legend()
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Spike count over time
    axes[1, 0].plot(activity_df['time_step'], activity_df['spike_count'], 'g-', linewidth=1, alpha=0.7)
    axes[1, 0].set_title('Spike Count Over Time')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Spike Count')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Activity statistics summary
    stats_data = {
        'Metric': ['Mean', 'Median', 'Std', 'Min', 'Max', '95th Percentile', 'Outliers'],
        'Value': [
            f"{activity_df['firing_rate'].mean():.1f} Hz",
            f"{activity_df['firing_rate'].median():.1f} Hz",
            f"{activity_df['firing_rate'].std():.1f} Hz",
            f"{activity_df['firing_rate'].min():.1f} Hz",
            f"{activity_df['firing_rate'].max():.1f} Hz",
            f"{burst_threshold:.1f} Hz",
            f"{outliers}"
        ]
    }
    
    axes[1, 1].axis('off')
    table = axes[1, 1].table(cellText=list(zip(stats_data['Metric'], stats_data['Value'])),
                              colLabels=['Metric', 'Value'], cellLoc='center', loc='center')
    table.auto_set_font_size(False)
    table.set_fontsize(9)
    table.scale(1.2, 1.5)
    axes[1, 1].set_title('Activity Statistics Summary')
    
    plt.tight_layout()
    plt.savefig('neural_activity_analysis.png', dpi=150)
    print(f"\n✓ Visualization saved as 'neural_activity_analysis.png'")
    
    plt.close()
    
    # Save analysis results
    activity_df.to_csv('neural_activity_data.csv', index=False)
    print(f"✓ Data saved as 'neural_activity_data.csv'")
    
    return activity_df

def example_network_topology_analysis():
    """Example: Analyze network topology and connectivity"""
    print("\n" + "=" * 60)
    print("Network Topology Analysis")
    print("=" * 60)
    
    # Create brain for topology analysis
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 2000)
    config.set("brain.region_count", 4)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation
    print("Running simulation for topology analysis...")
    for i in range(2000):
        brain.step(i)
    
    # Get network metrics
    metrics = brain.computeNetworkMetrics()
    
    print("\nNetwork Topology Metrics:")
    print(f"  Connectivity Density: {metrics.connectivity_density:.6f}")
    print(f"  Average Path Length: {metrics.path_length:.6f}")
    print(f"  Clustering Coefficient: {metrics.clustering_coefficient:.6f}")
    print(f"  Small World Coefficient: {metrics.small_world_coefficient:.6f}")
    print(f"  Assortativity: {metrics.assortativity:.6f}")
    print(f"  Modularity: {metrics.modularity:.6f}")
    
    # Calculate derived metrics
    print("\nDerived Metrics:")
    small_world_threshold = 0.3  # Typical threshold for small-world networks
    is_small_world = metrics.small_world_coefficient > small_world_threshold
    print(f"  Is Small-World Network: {is_small_world}")
    print(f"  Network Efficiency: {1.0/metrics.path_length:.6f}")
    print(f"  Global Clustering: {metrics.clustering_coefficient:.6f}")
    print(f"  Expected Clustering (ER): {metrics.connectivity_density * np.log(metrics.connectivity_density) / (2 * np.pi):.6f}")
    
    # Create topology visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Network metrics radar chart
    categories = ['Connectivity', 'Path Length', 'Clustering', 'Small World', 'Assortativity']
    values = [
        metrics.connectivity_density,
        metrics.path_length,
        metrics.clustering_coefficient,
        metrics.small_world_coefficient,
        metrics.assortativity
    ]
    
    # Normalize values for radar chart
    normalized_values = []
    for i, val in enumerate(values):
        if i == 1:  # Path length - lower is better
            norm_val = 1.0 - min(val, 1.0)
        elif i == 3:  # Small world - higher is better
            norm_val = val
        else:  # Other metrics - use as-is or normalize
            norm_val = min(val, 1.0)
        normalized_values.append(norm_val)
    
    angles = np.linspace(0, 2*np.pi, len(categories), closed=False)
    normalized_values += normalized_values[:1]  # Close the radar chart
    
    axes[0, 0].plot(angles, normalized_values, 'b-', linewidth=2, alpha=0.7)
    axes[0, 0].fill(angles, normalized_values, alpha=0.25, color='blue')
    axes[0, 0].set_xticks(angles[:-1])
    axes[0, 0].set_xticklabels(categories)
    axes[0, 0].set_ylim(0, 1)
    axes[0, 0].set_title('Network Topology Radar Chart')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Network metrics comparison
    metric_names = ['Connectivity', 'Path Length', 'Clustering', 'Small World', 'Assortativity']
    metric_values = [metrics.connectivity_density,
                     metrics.path_length,
                     metrics.clustering_coefficient,
                     metrics.small_world_coefficient,
                     metrics.assortativity]
    
    colors = ['skyblue', 'lightgreen', 'salmon', 'gold', 'violet']
    bars = axes[0, 1].bar(metric_names, metric_values, color=colors, alpha=0.7, edgecolor='black')
    axes[0, 1].set_title('Network Metrics Bar Chart')
    axes[0, 1].set_ylabel('Value')
    axes[0, 1].tick_params(axis='x', rotation=45)
    axes[0, 1].grid(True, alpha=0.3, axis='y')
    
    # Add value labels on bars
    for bar, value in zip(bars, metric_values):
        axes[0, 1].text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01,
                        f"{value:.3f}", ha='center', va='bottom', fontsize=8)
    
    # Plot 3: Region connectivity (simulated)
    regions = ['Region 1', 'Region 2', 'Region 3', 'Region 4']
    intra_region = [0.3, 0.4, 0.35, 0.38]
    inter_region = [0.15, 0.12, 0.18, 0.16]
    
    x = np.arange(len(regions))
    width = 0.35
    
    axes[1, 0].bar(x - width/2, intra_region, width, label='Intra-region', alpha=0.7, color='skyblue')
    axes[1, 0].bar(x + width/2, inter_region, width, label='Inter-region', alpha=0.7, color='lightgreen')
    axes[1, 0].set_title('Regional Connectivity')
    axes[1, 0].set_xlabel('Region')
    axes[1, 0].set_ylabel('Connectivity')
    axes[1, 0].set_xticks(x)
    axes[1, 0].set_xticklabels(regions)
    axes[1, 0].legend()
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Network quality assessment
    quality_metrics = {
        'Global Efficiency': 1.0 / metrics.path_length,
        'Modularity': metrics.modularity,
        'Small World': metrics.small_world_coefficient,
        'Clustering Quality': metrics.clustering_coefficient / (metrics.connectivity_density * np.log(metrics.connectivity_density) / (2 * np.pi) + 0.001)
    }
    
    quality_names = list(quality_metrics.keys())
    quality_values = list(quality_metrics.values())
    
    bars2 = axes[1, 1].barh(quality_names, quality_values, alpha=0.7, color=['red', 'blue', 'green', 'orange'])
    axes[1, 1].set_title('Network Quality Assessment')
    axes[1, 1].set_xlabel('Quality Score')
    axes[1, 1].grid(True, alpha=0.3, axis='x')
    
    # Add value labels
    for bar, value in zip(bars2, quality_values):
        axes[1, 1].text(value + 0.01, bar.get_y() + bar.get_height()/2,
                        f"{value:.3f}", ha='left', va='center', fontsize=9)
    
    plt.tight_layout()
    plt.savefig('network_topology_analysis.png', dpi=150)
    print(f"\n✓ Network topology visualization saved as 'network_topology_analysis.png'")
    
    plt.close()
    
    # Save topology data
    topology_data = {
        'metric': list(metric_names),
        'value': metric_values,
        'connectivity_density': metrics.connectivity_density,
        'path_length': metrics.path_length,
        'clustering_coefficient': metrics.clustering_coefficient,
        'small_world_coefficient': metrics.small_world_coefficient,
        'assortativity': metrics.assortativity,
        'modularity': metrics.modularity
    }
    
    pd.DataFrame(topology_data).to_csv('network_topology_data.csv', index=False)
    print(f"✓ Topology data saved as 'network_topology_data.csv'")
    
    return metrics

def example_plasticity_analysis():
    """Example: Analyze plasticity and learning dynamics"""
    print("\n" + "=" * 60)
    print("Plasticity and Learning Analysis")
    print("=" * 60)
    
    # Create brain with plasticity enabled
    config = pynlm.createDefaultConfig()
    config.set("plasticity.stdp.enable", True)
    config.set("plasticity.hebbian.enable", True)
    config.set("plasticity.structural.enable", True)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run simulation with learning
    print("Running plasticity simulation...")
    
    # Collect learning metrics over time
    time_steps = 500
    weight_changes = []
    spike_counts = []
    learning_rates = []
    
    # Simulate learning dynamics (in real usage, this would be actual brain data)
    for i in range(time_steps):
        # Simulate weight changes (in real implementation, this comes from brain)
        base_weight_change = np.random.normal(0, 0.01)
        learning_rate = 0.001 + 0.0005 * (i / time_steps)
        spike_count = np.random.poisson(50) + np.random.normal(0, 10)
        
        weight_changes.append(base_weight_change)
        spike_counts.append(spike_count)
        learning_rates.append(learning_rate)
    
    # Create plasticity DataFrame
    plasticity_df = pd.DataFrame({
        'time_step': range(time_steps),
        'weight_change': weight_changes,
        'spike_count': spike_counts,
        'learning_rate': learning_rates
    })
    
    # Plasticity analysis
    print("\nPlasticity Analysis:")
    print(f"  Average weight change: {plasticity_df['weight_change'].mean():.6f}")
    print(f"  Weight change std: {plasticity_df['weight_change'].std():.6f}")
    print(f"  Total spike count: {plasticity_df['spike_count'].sum()}")
    print(f"  Average spike count: {plasticity_df['spike_count'].mean():.1f}")
    print(f"  Learning rate trend: {learning_rates[-1]:.6f} (final)")
    print(f"  Learning rate trend: {learning_rates[0]:.6f} (initial)")
    
    # Calculate convergence metrics
    first_half = plasticity_df['weight_change'].iloc[:time_steps//2]
    second_half = plasticity_df['weight_change'].iloc[time_steps//2:]
    
    convergence_score = 1.0 - abs(second_half.mean() - first_half.mean()) / abs(first_half.mean() + 0.001)
    stability_score = 1.0 / (1.0 + plasticity_df['weight_change'].std())
    
    print(f"  Convergence Score: {convergence_score:.3f}")
    print(f"  Stability Score: {stability_score:.3f}")
    
    # Plasticity type analysis
    positive_changes = plasticity_df[plasticity_df['weight_change'] > 0]
    negative_changes = plasticity_df[plasticity_df['weight_change'] <= 0]
    
    print(f"  Positive weight changes: {len(positive_changes)}")
    print(f"  Negative weight changes: {len(negative_changes)}")
    print(f"  Plasticity balance: {len(positive_changes) / len(plasticity_df):.3f}")
    
    # Create plasticity visualization
    fig, axes = plt.subplots(2, 2, figsize=(15, 12))
    
    # Plot 1: Weight change over time
    axes[0, 0].plot(plasticity_df['time_step'], plasticity_df['weight_change'], 'b-', linewidth=1, alpha=0.7)
    axes[0, 0].axhline(y=0, color='r', linestyle='--', alpha=0.5)
    axes[0, 0].set_title('Weight Change Dynamics')
    axes[0, 0].set_xlabel('Time Step')
    axes[0, 0].set_ylabel('Weight Change')
    axes[0, 0].grid(True, alpha=0.3)
    
    # Plot 2: Spike count distribution
    axes[0, 1].hist(plasticity_df['spike_count'], bins=30, alpha=0.7, color='skyblue', edgecolor='black')
    axes[0, 1].set_title('Spike Count Distribution')
    axes[0, 1].set_xlabel('Spike Count')
    axes[0, 1].set_ylabel('Frequency')
    axes[0, 1].grid(True, alpha=0.3)
    
    # Plot 3: Learning rate over time
    axes[1, 0].plot(plasticity_df['time_step'], plasticity_df['learning_rate'], 'g-', linewidth=2)
    axes[1, 0].set_title('Learning Rate Over Time')
    axes[1, 0].set_xlabel('Time Step')
    axes[1, 0].set_ylabel('Learning Rate')
    axes[1, 0].grid(True, alpha=0.3)
    
    # Plot 4: Plasticity quality assessment
    quality_data = {
        'Convergence': convergence_score,
        'Stability': stability_score,
        'Learning Rate': learning_rates[-1],
        'Plasticity Balance': len(positive_changes) / len(plasticity_df)
    }
    
    quality_names = list(quality_data.keys())
    quality_values = list(quality_data.values())
    
    colors = ['red', 'blue', 'green', 'orange']
    bars = axes[1, 1].barh(quality_names, quality_values, alpha=0.7, color=colors)
    axes[1, 1].set_title('Plasticity Quality Assessment')
    axes[1, 1].set_xlabel('Quality Score')
    axes[1, 1].grid(True, alpha=0.3, axis='x')
    
    # Add value labels
    for bar, value in zip(bars, quality_values):
        axes[1, 1].text(value + 0.01, bar.get_y() + bar.get_height()/2,
                        f"{value:.3f}", ha='left', va='center', fontsize=9)
    
    plt.tight_layout()
    plt.savefig('plasticity_analysis.png', dpi=150)
    print(f"\n✓ Plasticity visualization saved as 'plasticity_analysis.png'")
    
    plt.close()
    
    # Save plasticity data
    plasticity_df.to_csv('plasticity_data.csv', index=False)
    print(f"✓ Plasticity data saved as 'plasticity_data.csv'")
    
    return plasticity_df

def example_comprehensive_report():
    """Example: Generate comprehensive analysis report"""
    print("\n" + "=" * 60)
    print("Comprehensive Analysis Report")
    print("=" * 60)
    
    # Create brain for comprehensive analysis
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", 3000)
    config.set("plasticity.stdp.enable", True)
    config.set("neuromod.curiosity.enable", True)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Run comprehensive simulation
    print("Running comprehensive simulation...")
    for i in range(5000):
        brain.step(i)
    
    # Generate comprehensive report
    print("Generating comprehensive analysis report...")
    
    report = {
        'simulation_parameters': {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'region_count': brain.getRegionCount(),
            'connection_probability': config.getOr<float>("brain.connection_probability", 0.1),
            'plasticity_enabled': config.getOr<bool>("plasticity.stdp.enable", True),
            'neuromodulation_enabled': config.getOr<bool>("neuromod.curiosity.enable", True)
        },
        'activity_statistics': {
            'total_spikes': brain.getTotalSpikeCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_neuron_percentage': (brain.getFiringNeuronCount() / brain.getTotalNeuronCount()) * 100,
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': brain.getDevelopmentalStage()
        },
        'network_metrics': brain.computeNetworkMetrics().__dict__ if hasattr(brain.computeNetworkMetrics(), '__dict__') else brain.computeNetworkMetrics(),
        'synchrony_metrics': brain.getSynchronyMetrics().__dict__ if hasattr(brain.getSynchronyMetrics(), '__dict__') else brain.getSynchronyMetrics(),
        'analysis_timestamp': datetime.now().isoformat()
    }
    
    # Save comprehensive report
    with open("comprehensive_analysis_report.txt", "w") as f:
        f.write("COMPREHENSIVE NLM ANALYSIS REPORT\n")
        f.write("=" * 60 + "\n\n")
        
        f.write("SIMULATION PARAMETERS:\n")
        for key, value in report['simulation_parameters'].items():
            f.write(f"  {key.replace('_', ' ').title()}: {value}\n")
        
        f.write("\nACTIVITY STATISTICS:\n")
        for key, value in report['activity_statistics'].items():
            f.write(f"  {key.replace('_', ' ').title()}: {value}\n")
        
        f.write("\nNETWORK METRICS:\n")
        if hasattr(report['network_metrics'], '__dict__'):
            metrics_dict = report['network_metrics'].__dict__
            for key, value in metrics_dict.items():
                f.write(f"  {key.replace('_', ' ').title()}: {value}\n")
        else:
            for key, value in report['network_metrics'].items():
                f.write(f"  {key.replace('_', ' ').title()}: {value}\n")
        
        f.write("\nSYNCHRONY METRICS:\n")
        if hasattr(report['synchrony_metrics'], '__dict__'):
            synchrony_dict = report['synchrony_metrics'].__dict__
            for key, value in synchrony_dict.items():
                f.write(f"  {key.replace('_', ' ').title()}: {value}\n")
        else:
            for key, value in report['synchrony_metrics'].items():
                f.write(f"  {key.replace('_', ' ').title()}: {value}\n")
        
        f.write(f"\nReport generated: {report['analysis_timestamp']}\n")
        f.write("=" * 60 + "\n")
        f.write("ANALYSIS COMPLETE\n")
    
    print(f"✓ Comprehensive report saved as 'comprehensive_analysis_report.txt'")
    
    # Create summary statistics table
    summary_data = {
        'Metric': ['Total Neurons', 'Total Synapses', 'Firing Neurons', 'Total Spikes',
                  'Firing %', 'Avg Firing Rate', 'E/I Ratio'],
        'Value': [
            report['simulation_parameters']['neuron_count'],
            report['simulation_parameters']['synapse_count'],
            report['activity_statistics']['firing_neurons'],
            report['activity_statistics']['total_spikes'],
            f"{report['activity_statistics']['firing_neuron_percentage']:.1f}%",
            f"{report['activity_statistics']['average_firing_rate']:.2f}",
            f"{report['activity_statistics']['excitement_inhibition_ratio']:.3f}"
        ]
    }
    
    summary_df = pd.DataFrame(summary_data)
    print(f"\nActivity Summary:")
    print(summary_df.to_string(index=False))
    
    return report

def main():
    """Run all analysis examples"""
    print("NLM Python Data Analysis Examples")
    print("=" * 60)
    print("This script demonstrates comprehensive data analysis with NLM Python bindings.")
    print()
    
    # Run all analysis examples
    example_neural_activity_analysis()
    example_network_topology_analysis()
    example_plasticity_analysis()
    example_comprehensive_report()
    
    print("\n" + "=" * 60)
    print("All analysis examples completed successfully!")
    print("=" * 60)
    print("\nKey features demonstrated:")
    print("1. ✓ Neural activity pattern analysis and visualization")
    print("2. ✓ Network topology analysis and radar charts")
    print("3. ✓ Plasticity and learning dynamics analysis")
    print("4. ✓ Comprehensive reporting and data export")
    print("\nGenerated files:")
    print("  - neural_activity_analysis.png")
    print("  - network_topology_analysis.png")
    print("  - plasticity_analysis.png")
    print("  - neural_activity_data.csv")
    print("  - network_topology_data.csv")
    print("  - plasticity_data.csv")
    print("  - comprehensive_analysis_report.txt")
    print("\nAll data has been saved for further analysis!")

if __name__ == "__main__":
    main()