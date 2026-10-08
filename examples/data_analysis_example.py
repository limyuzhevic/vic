#!/usr/bin/env python3
"""
NLM Python Example: Data Analysis and Neural Dynamics
=============================================

This example demonstrates the enhanced data analysis capabilities
of the NLM Python API, including neural dynamics analysis,
statistics, and visualization.
"""

import pynlm as nlm
import pynlm.utils as utils
import numpy as np
import time

def main():
    print("NLM Data Analysis Example")
    print("=" * 50)
    
    # Create test configuration
    print("1. Setting Up Test Environment")
    print("-" * 35)
    
    config = utils.ConfigUtils.create_research_config(neuron_count=500)
    brain = nlm.create_brain(config)
    world = utils.WorldUtils.create_empty_world(width=100, height=100)
    
    print(f"   Created brain with {brain.get_total_neuron_count()} neurons")
    print(f"   World size: {world.get_width()}x{world.get_height()}")
    
    # Run simulation and collect data
    print("\n2. Running Simulation and Collecting Data")
    print("-" * 45)
    
    simulation_steps = 50
    
    # Initialize tracking variables
    step_data = {
        'steps': [],
        'neuron_stats': [],
        'brain_stats': [],
        'world_stats': [],
        'time_stamps': []
    }
    
    print(f"\n   Running {simulation_steps} simulation steps...")
    start_time = time.time()
    
    for step in range(simulation_steps):
        # Record data before step
        brain_stats = nlm.BrainUtils.get_neuron_statistics(brain)
        world_stats = world.get_world_status()
        
        # Store current state
        step_data['steps'].append(step)
        step_data['neuron_stats'].append(brain_stats)
        step_data['brain_stats'].append({
            'total_neurons': brain.get_total_neuron_count(),
            'active_neurons': brain.get_active_neuron_count(),
            'firing_neurons': brain.get_firing_neuron_count(),
            'total_spikes': brain.get_total_spike_count(),
            'e_i_ratio': brain.get_excitation_inhibition_ratio(),
            'average_firing_rate': brain.get_average_firing_rate(),
            'regional_activity': [brain.get_region_count()]
        })
        step_data['world_stats'].append(world_stats)
        step_data['time_stamps'].append(step * 1.0)
        
        # Run simulation step
        brain.run_step(step, step * 1.0)
        world.update(step)
        
        # Print progress
        if step % 10 == 0:
            print(f"   Step {step}: {brain_stats['active_neurons']}/{brain_stats['total_neurons']} neurons active")
            print(f"             Firing rate: {brain_stats['average_firing_rate']:.2f} Hz")
    
    end_time = time.time()
    print(f"\n   Simulation completed in {end_time - start_time:.2f} seconds")
    
    # Analyze neural dynamics
    print("\n3. Neural Dynamics Analysis")
    print("-" * 35)
    
    print("\n3.1 Brain Statistics Analysis:")
    
    # Extract time series data
    firing_rates = [stats['average_firing_rate'] for stats in step_data['brain_stats']]
    e_i_ratios = [stats['e_i_ratio'] for stats in step_data['brain_stats']]
    spike_counts = [stats['total_spikes'] for stats in step_data['brain_stats']]
    active_ratios = [stats['active_neurons'] / stats['total_neurons'] 
                    for stats in step_data['brain_stats']]
    
    # Calculate statistics
    print("\n   Statistical Summary:")
    print(f"     Firing rate - Mean: {np.mean(firing_rates):.2f} Hz, Std: {np.std(firing_rates):.2f} Hz")
    print(f"     E/I ratio - Mean: {np.mean(e_i_ratios):.2f}, Std: {np.std(e_i_ratios):.2f}")
    print(f"     Spike count - Mean: {np.mean(spike_counts):.0f}, Max: {np.max(spike_counts)}")
    print(f"     Active ratio - Mean: {np.mean(active_ratios):.3f}, Min: {np.min(active_ratios):.3f}")
    
    # Analyze firing rate dynamics
    print("\n3.2 Firing Rate Dynamics Analysis:")
    
    # Find peaks and troughs
    firing_rate_array = np.array(firing_rates)
    peaks = np.where(np.diff(np.sign(np.diff(firing_rate_array))))[0] + 1
    troughs = np.where(np.diff(np.sign(-np.diff(firing_rate_array))))[0] + 1
    
    print(f"     Number of firing rate peaks: {len(peaks)}")
    print(f"     Number of firing rate troughs: {len(troughs)}")
    
    if len(peaks) > 0:
        peak_values = firing_rate_array[peaks]
        print(f"     Peak firing rate - Max: {np.max(peak_values):.2f} Hz, Mean: {np.mean(peak_values):.2f} Hz")
    
    if len(troughs) > 0:
        trough_values = firing_rate_array[troughs]
        print(f"     Trough firing rate - Min: {np.min(trough_values):.2f} Hz, Mean: {np.mean(trough_values):.2f} Hz")
    
    # Analyze E/I ratio dynamics
    print("\n3.3 Excitation/Inhibition Ratio Analysis:")
    
    e_i_array = np.array(e_i_ratios)
    e_i_std = np.std(e_i_array)
    e_i_trend = np.polyfit(step_data['steps'], e_i_array, 1)[0]  # Linear trend
    
    print(f"     E/I ratio stability - Standard deviation: {e_i_std:.3f}")
    print(f"     E/I ratio trend - Slope: {e_i_trend:.4f}")
    print(f"     E/I ratio range - Min: {np.min(e_i_array):.2f}, Max: {np.max(e_i_array):.2f}")
    
    if abs(e_i_trend) > 0.01:
        trend_direction = "increasing" if e_i_trend > 0 else "decreasing"
        print(f"     E/I ratio shows {trend_direction} trend over time")
    else:
        print(f"     E/I ratio is relatively stable over time")
    
    # Analyze spike count dynamics
    print("\n3.4 Spike Count Dynamics Analysis:")
    
    spike_array = np.array(spike_counts)
    spike_rate = spike_array / (simulation_steps * 0.1)  # spikes per second
    
    print(f"     Spike rate - Mean: {np.mean(spike_rate):.1f} spikes/sec")
    print(f"     Spike rate - Standard deviation: {np.std(spike_rate):.1f} spikes/sec")
    print(f"     Spike rate - Coefficient of variation: {np.std(spike_rate)/np.mean(spike_rate):.3f}")
    
    # Identify high and low activity periods
    spike_rate_threshold = np.mean(spike_rate) + np.std(spike_rate)
    high_activity_steps = [step for step, rate in zip(step_data['steps'], spike_rate) if rate > spike_rate_threshold]
    
    print(f"     High activity periods: {len(high_activity_steps)} out of {simulation_steps} steps")
    if len(high_activity_steps) > 0:
        print(f"     High activity step indices: {high_activity_steps[:10]}{'...' if len(high_activity_steps) > 10 else ''}")
    
    # Analyze active neuron dynamics
    print("\n3.5 Active Neuron Dynamics Analysis:")
    
    active_array = np.array(active_ratios)
    active_mean = np.mean(active_array)
    active_std = np.std(active_array)
    
    print(f"     Active neuron ratio - Mean: {active_mean:.3f}, Std: {active_std:.3f}")
    print(f"     Active neuron ratio - Range: {np.min(active_array):.3f} - {np.max(active_array):.3f}")
    
    # Find correlation between variables
    print("\n3.6 Correlation Analysis:")
    
    variables = {
        'Firing Rate': firing_rate_array,
        'E/I Ratio': e_i_array,
        'Spike Count': spike_array,
        'Active Ratio': active_array
    }
    
    print("     Correlation matrix:")
    correlation_matrix = np.corrcoef(list(variables.values()))
    
    for i, (var1_name, var1_data) in enumerate(variables.items()):
        correlations = []
        for j, (var2_name, var2_data) in enumerate(variables.items()):
            if i != j:
                corr = np.corrcoef(var1_data, var2_data)[0, 1]
                correlations.append((var2_name, corr))
        
        # Sort by absolute correlation
        correlations.sort(key=lambda x: abs(x[1]), reverse=True)
        
        print(f"       {var1_name}:")
        for var2_name, corr in correlations[:3]:  # Show top 3
            direction = "positive" if corr > 0 else "negative"
            print(f"         {var2_name}: {corr:.3f} ({direction})")
    
    # Statistical tests
    print("\n3.7 Statistical Tests:")
    
    # Check for normality (assuming we want to test against normal distribution)
    from scipy import stats
    
    try:
        for name, data in variables.items():
            # Shapiro-Wilk test for normality
            if len(data) >= 3 and len(data) <= 5000:
                _, p_value = stats.shapiro(data)
                if p_value > 0.05:
                    print(f"       {name} appears to be normally distributed (p={p_value:.3f})")
                else:
                    print(f"       {name} does not appear to be normally distributed (p={p_value:.3f})")
    except ImportError:
        print("       SciPy not available, skipping normality tests")
    
    # Time series analysis
    print("\n3.8 Time Series Analysis:")
    
    # Find autocorrelation
    try:
        from scipy.signal import correlate
        
        # Simple autocorrelation for firing rate
        firing_corr = correlate(firing_rate_array - np.mean(firing_rate_array),
                               firing_rate_array - np.mean(firing_rate_array), mode='full')
        firing_corr = firing_corr[len(firing_corr)//2:]  # Take second half
        firing_corr = firing_corr[:len(firing_rate_array)]  # Keep same length
        
        # Normalize autocorrelation
        firing_corr = firing_corr / firing_corr[0]
        
        # Find significant autocorrelation peaks
        autocorr_threshold = 0.5
        autocorr_peaks = [i for i, corr in enumerate(firing_corr) if abs(corr) > autocorr_threshold]
        
        print(f"     Significant autocorrelation peaks at lags: {autocorr_peaks}")
        
        if autocorr_peaks:
            print(f"     Peak at lag {autocorr_peaks[0]}: correlation = {firing_corr[autocorr_peaks[0]]:.3f}")
        
    except ImportError:
        print("       SciPy not available, skipping autocorrelation analysis")
    
    # Create comprehensive report
    print("\n4. Comprehensive Analysis Report")
    print("-" * 40)
    
    report = {
        'simulation_parameters': {
            'neuron_count': config.get('neuron_count'),
            'region_count': config.get('region_count'),
            'connection_probability': config.get('connection_probability'),
            'steps': simulation_steps,
            'duration_seconds': end_time - start_time
        },
        'statistical_summary': {
            'firing_rate': {
                'mean': float(np.mean(firing_rates)),
                'std': float(np.std(firing_rates)),
                'min': float(np.min(firing_rates)),
                'max': float(np.max(firing_rates)),
                'cv': float(np.std(firing_rates) / np.mean(firing_rates)) if np.mean(firing_rates) > 0 else 0
            },
            'e_i_ratio': {
                'mean': float(np.mean(e_i_ratios)),
                'std': float(np.std(e_i_ratios)),
                'min': float(np.min(e_i_ratios)),
                'max': float(np.max(e_i_ratios)),
                'trend': float(np.polyfit(step_data['steps'], e_i_array, 1)[0])
            },
            'spike_count': {
                'mean': float(np.mean(spike_counts)),
                'std': float(np.std(spike_counts)),
                'max': float(np.max(spike_counts)),
                'rate_mean': float(np.mean(spike_rate)),
                'rate_std': float(np.std(spike_rate))
            },
            'active_ratio': {
                'mean': float(np.mean(active_array)),
                'std': float(np.std(active_array)),
                'min': float(np.min(active_array)),
                'max': float(np.max(active_array))
            }
        },
        'dynamic_analysis': {
            'number_of_peaks': len(peaks),
            'number_of_troughs': len(troughs),
            'autocorrelation_peaks': len(autocorr_peaks) if 'autocorr_peaks' in locals() else 0,
            'high_activity_periods': len(high_activity_steps) if 'high_activity_steps' in locals() else 0
        }
    }
    
    # Save report
    print("\n4.1 Saving Analysis Report:")
    utils.DataUtils.save_to_file(report, "neural_dynamics_report.json")
    print("   Analysis report saved to: neural_dynamics_report.json")
    
    # Create visualization data
    print("\n5. Creating Visualization Data:")
    
    visualization_data = {
        'time_series': {
            'steps': step_data['steps'],
            'firing_rates': firing_rates,
            'e_i_ratios': e_i_ratios,
            'spike_counts': spike_counts,
            'active_ratios': active_ratios,
            'time_stamps': step_data['time_stamps']
        },
        'summary_statistics': {
            'final_brain_stats': step_data['brain_stats'][-1],
            'mean_values': {
                'firing_rate': np.mean(firing_rates),
                'e_i_ratio': np.mean(e_i_ratios),
                'spike_count': np.mean(spike_counts),
                'active_ratio': np.mean(active_array)
            },
            'trend_analysis': {
                'firing_rate_trend': np.polyfit(step_data['steps'], firing_rate_array, 1)[0],
                'e_i_ratio_trend': np.polyfit(step_data['steps'], e_i_array, 1)[0],
                'spike_count_trend': np.polyfit(step_data['steps'], spike_array, 1)[0]
            }
        }
    }
    
    utils.DataUtils.save_to_file(visualization_data, "neural_dynamics_viz.json")
    print("   Visualization data saved to: neural_dynamics_viz.json")
    
    # Display final statistics
    print("\n6. Final Statistics")
    print("-" * 30)
    
    final_stats = step_data['brain_stats'][-1]
    print(f"\n   Final Simulation Statistics:")
    print(f"     Total Neurons: {final_stats['total_neurons']}")
    print(f"     Active Neurons: {final_stats['active_neurons']}")
    print(f"     Firing Neurons: {final_stats['firing_neurons']}")
    print(f"     Total Spikes: {final_stats['total_spikes']}")
    print(f"     E/I Ratio: {final_stats['e_i_ratio']:.2f}")
    print(f"     Average Firing Rate: {final_stats['average_firing_rate']:.2f} Hz")
    
    print("\n   Overall Analysis:")
    print(f"     Neural activity is {'stable' if e_i_std < 0.1 else 'dynamic'} (E/I ratio std: {e_i_std:.3f})")
    print(f"     Network shows {'consistent' if len(peaks) < 3 else 'variable'} firing patterns")
    print(f"     Spike generation is {'regular' if np.std(spike_rate) < np.mean(spike_rate) else 'irregular'} (CV: {np.std(spike_rate)/np.mean(spike_rate):.3f})")
    
    # Save brain state
    print("\n7. Saving Brain State")
    print("-" * 30)
    
    brain.save("data_analysis_brain.nlm")
    print("   Brain state saved to: data_analysis_brain.nlm")
    
    print("\nData analysis example completed successfully!")

if __name__ == "__main__":
    main()
