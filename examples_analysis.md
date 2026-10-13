# NLM - Analysis Examples

**Date:** 2026-10-13
**Audience:** Intermediate to advanced users
**Level:** Advanced

---

## What You'll Learn

This section provides **analysis examples** demonstrating how to use NLM's comprehensive analysis tools to understand brain activity, performance, and research findings.

1. **Brain Activity Analysis** - Comprehensive analysis of neural dynamics
2. **Network Topology Analysis** - Understanding brain connectivity patterns
3. **Memory System Analysis** - Analyzing memory storage and retrieval
4. **Neuromodulation Analysis** - Understanding regulatory mechanisms
5. **Development Stage Analysis** - Tracking brain development over time
6. **Statistical Analysis** - Quantitative analysis of simulation results

---

## Example 1: Comprehensive Brain Activity Analysis

### What It Does
This example demonstrates comprehensive analysis of brain activity using Advanced API tools.

```python
import pynlm
from pynlm.advanced_api import (
    createBrainProfiler,
    createNeuronActivityMonitor,
    createNetworkConnectivityAnalyzer,
    createMemorySystemProfiler,
    createNeuromodulationAnalyzer,
    createDevelopmentStageAnalyzer
)
import numpy as np
import matplotlib.pyplot as plt
from scipy import stats
import pandas as pd

def comprehensive_brain_analysis(brain, duration_steps=1000):
    """Perform comprehensive analysis of brain activity"""
    
    print("=== Comprehensive Brain Activity Analysis ===")
    
    # Initialize analysis tools
    print("1. Initializing analysis tools...")
    
    brain_profiler = createBrainProfiler(brain)
    neuron_monitor = createNeuronActivityMonitor(brain)
    network_analyzer = createNetworkConnectivityAnalyzer(brain)
    memory_profiler = createMemorySystemProfiler(brain)
    neuromod_analyzer = createNeuromodulationAnalyzer(brain)
    development_analyzer = createDevelopmentStageAnalyzer(brain)
    
    # Data collection containers
    analysis_data = {
        'time_series': [],
        'neural_dynamics': [],
        'memory_analysis': [],
        'neuromodulation': [],
        'development': [],
        'network_topology': []
    }
    
    # PART 1: Run simulation and collect data
    print("2. Running simulation and collecting data...")
    
    brain_profiler.start()
    
    for step in range(duration_steps):
        brain.step(step)
        
        # Collect time series data
        if step % 10 == 0:  # Collect every 10 steps
            time_series_data = {
                'step': step,
                'time': step * 0.1,  # Assuming dt = 0.1
                'firing_rate': brain.getAverageFiringRate(),
                'firing_count': brain.getFiringNeuronCount(),
                'total_neurons': brain.getTotalNeuronCount(),
                'development_stage': brain.getDevelopmentalStage(),
                'dopamine': brain.getDopamineLevel(),
                'curiosity': brain.getCuriosityLevel(),
                'novelty': brain.getNoveltyLevel(),
                'prediction_error': brain.getPredictionError()
            }
            analysis_data['time_series'].append(time_series_data)
        
        # Collect neural dynamics
        if step % 50 == 0:
            neural_dynamics = brain_profiler.get_neural_dynamics(step)
            analysis_data['neural_dynamics'].append(neural_dynamics)
        
        # Collect memory analysis
        if step % 100 == 0:
            memory_analysis = memory_profiler.get_memory_summary()
            analysis_data['memory_analysis'].append(memory_analysis)
        
        # Collect neuromodulation data
        if step % 100 == 0:
            neuromodulation_data = neuromod_analyzer.get_modulation_summary()
            analysis_data['neuromodulation'].append(neuromodulation_data)
        
        # Collect development data
        if step % 200 == 0:
            development_data = development_analyzer.get_development_summary()
            analysis_data['development'].append(development_data)
        
        # Collect network topology
        if step % 300 == 0:
            network_data = network_analyzer.get_connectivity_summary()
            analysis_data['network_topology'].append(network_data)
    
    brain_profiler.stop()
    
    # PART 2: Statistical analysis
    print("3. Performing statistical analysis...")
    
    statistical_analysis = perform_comprehensive_statistical_analysis(analysis_data)
    
    # PART 3: Pattern detection
    print("4. Detecting patterns...")
    
    pattern_analysis = detect_patterns_in_data(analysis_data)
    
    # PART 4: Visualization
    print("5. Creating visualizations...")
    
    visualizations = create_comprehensive_visualizations(analysis_data, statistical_analysis, pattern_analysis)
    
    # PART 5: Generate comprehensive report
    print("6. Generating comprehensive report...")
    
    comprehensive_report = create_comprehensive_analysis_report(
        analysis_data, statistical_analysis, pattern_analysis, visualizations
    )
    
    return analysis_data, statistical_analysis, pattern_analysis, visualizations, comprehensive_report

def perform_comprehensive_statistical_analysis(data):
    """Perform comprehensive statistical analysis on brain data"""
    
    print("   Performing statistical analysis...")
    
    stats = {
        'time_series_stats': {},
        'neural_dynamics_stats': {},
        'memory_stats': {},
        'neuromodulation_stats': {},
        'development_stats': {},
        'network_stats': {}
    }
    
    # Time series statistics
    if data['time_series']:
        time_series_df = pd.DataFrame(data['time_series'])
        
        stats['time_series_stats'] = {
            'firing_rate': {
                'mean': time_series_df['firing_rate'].mean(),
                'std': time_series_df['firing_rate'].std(),
                'median': time_series_df['firing_rate'].median(),
                'min': time_series_df['firing_rate'].min(),
                'max': time_series_df['firing_rate'].max(),
                'skewness': stats.skew(time_series_df['firing_rate']),
                'kurtosis': stats.kurtosis(time_series_df['firing_rate']),
                'confidence_interval': calculate_confidence_interval(time_series_df['firing_rate'])
            },
            
            'development_stage': {
                'distribution': time_series_df['development_stage'].value_counts().to_dict(),
                'progression_rate': calculate_development_progression(time_series_df)
            },
            
            'neuromodulators': {
                'dopamine': {
                    'mean': time_series_df['dopamine'].mean(),
                    'std': time_series_df['dopamine'].std(),
                    'coefficient_of_variation': time_series_df['dopamine'].std() / time_series_df['dopamine'].mean() if time_series_df['dopamine'].mean() > 0 else 0
                },
                'curiosity': {
                    'mean': time_series_df['curiosity'].mean(),
                    'std': time_series_df['curiosity'].std(),
                    'threshold_analysis': analyze_threshold_crossings(time_series_df['curiosity'], threshold=0.5)
                },
                'novelty': {
                    'mean': time_series_df['novelty'].mean(),
                    'std': time_series_df['novelty'].std()
                }
            }
        }
    
    # Neural dynamics statistics
    if data['neural_dynamics']:
        neural_dynamics_df = pd.DataFrame(data['neural_dynamics'])
        
        stats['neural_dynamics_stats'] = {
            'spike_frequency_distribution': {
                'mean': neural_dynamics_df['spike_frequency'].mean(),
                'std': neural_dynamics_df['spike_frequency'].std(),
                'histogram': calculate_histogram(neural_dynamics_df['spike_frequency']),
                'outliers': detect_outliers(neural_dynamics_df['spike_frequency'])
            },
            
            'burst_analysis': {
                'burst_rate': neural_dynamics_df['burst_rate'].mean(),
                'burst_duration': neural_dynamics_df['burst_duration'].mean(),
                'inter_burst_interval': neural_dynamics_df['inter_burst_interval'].mean()
            }
        }
    
    # Memory statistics
    if data['memory_analysis']:
        memory_df = pd.DataFrame(data['memory_analysis'])
        
        stats['memory_stats'] = {
            'working_memory': {
                'load': memory_df['working_memory_load'].mean(),
                'capacity': memory_df['working_memory_capacity'].mean(),
                'efficiency': memory_df['working_memory_efficiency'].mean()
            },
            'episodic_memory': {
                'retention': memory_df['episodic_retention'].mean(),
                'consolidation_rate': memory_df['episodic_consolidation_rate'].mean(),
                'replay_frequency': memory_df['episodic_replay_frequency'].mean()
            },
            'semantic_memory': {
                'access_time': memory_df['semantic_access_time'].mean(),
                'search_efficiency': memory_df['semantic_search_efficiency'].mean()
            }
        }
    
    # Neuromodulation statistics
    if data['neuromodulation']:
        neuromodulation_df = pd.DataFrame(data['neuromodulation'])
        
        stats['neuromodulation_stats'] = {
            'dopamine_dynamics': {
                'stability': calculate_stability(neuromodulation_df['dopamine_level']),
                'regulatory_effectiveness': calculate_regulatory_effectiveness(neuromodulation_df)
            },
            'curiosity_dynamics': {
                'exploration_rate': neuromodulation_df['curiosity_level'].mean(),
                'novelty_response': neuromodulation_df['novelty_response'].mean()
            }
        }
    
    # Development statistics
    if data['development']:
        development_df = pd.DataFrame(data['development'])
        
        stats['development_stats'] = {
            'plasticity': {
                'rate': development_df['plasticity_rate'].mean(),
                'critical_period_progress': development_df['critical_period_progress'].mean()
            },
            'maturation': {
                'stage_transitions': calculate_stage_transitions(development_df),
                'maturation_rate': calculate_maturation_rate(development_df)
            }
        }
    
    # Network statistics
    if data['network_topology']:
        network_df = pd.DataFrame(data['network_topology'])
        
        stats['network_stats'] = {
            'topology': {
                'clustering_coefficient': network_df['clustering_coefficient'].mean(),
                'path_length': network_df['characteristic_path_length'].mean(),
                'small_worldness': network_df['small_worldness'].mean()
            },
            'connectivity': {
                'connection_density': network_df['connection_density'].mean(),
                'hub_stability': calculate_hub_stability(network_df)
            }
        }
    
    return stats

def detect_patterns_in_data(data):
    """Detect patterns in brain activity data"""
    
    print("   Detecting patterns...")
    
    patterns = {
        'development_patterns': {},
        'neural_patterns': {},
        'memory_patterns': {},
        'neuromodulation_patterns': {},
        'network_patterns': {},
        'temporal_patterns': {}
    }
    
    # Development patterns
    if data['development']:
        development_df = pd.DataFrame(data['development'])
        patterns['development_patterns'] = {
            'critical_period_transition': detect_critical_period_transition(development_df),
            'plasticity_phases': identify_plasticity_phases(development_df),
            'maturation_trajectory': calculate_maturation_trajectory(development_df)
        }
    
    # Neural patterns
    if data['neural_dynamics']:
        neural_df = pd.DataFrame(data['neural_dynamics'])
        patterns['neural_patterns'] = {
            'burst_patterns': detect_burst_patterns(neural_df),
            'firing_rate_patterns': detect_firing_rate_patterns(neural_df),
            'coherence_patterns': detect_coherence_patterns(neural_df)
        }
    
    # Memory patterns
    if data['memory_analysis']:
        memory_df = pd.DataFrame(data['memory_analysis'])
        patterns['memory_patterns'] = {
            'consolidation_patterns': detect_consolidation_patterns(memory_df),
            'retrieval_patterns': detect_retrieval_patterns(memory_df),
            'interference_patterns': detect_interference_patterns(memory_df)
        }
    
    # Neuromodulation patterns
    if data['neuromodulation']:
        neuromodulation_df = pd.DataFrame(data['neuromodulation'])
        patterns['neuromodulation_patterns'] = {
            'dopamine_regulation': analyze_dopamine_regulation(neuromodulation_df),
            'curiosity_drives': analyze_curiosity_drives(neuromodulation_df),
            'novelty_response': analyze_novelty_response(neuromodulation_df)
        }
    
    # Network patterns
    if data['network_topology']:
        network_df = pd.DataFrame(data['network_topology'])
        patterns['network_patterns'] = {
            'community_structure': analyze_community_structure(network_df),
            'hub_dynamics': analyze_hub_dynamics(network_df),
            'flow_patterns': analyze_flow_patterns(network_df)
        }
    
    # Temporal patterns
    if data['time_series']:
        time_series_df = pd.DataFrame(data['time_series'])
        patterns['temporal_patterns'] = {
            'rhythms': detect_temporal_rhythms(time_series_df),
            'cycles': detect_cycles(time_series_df),
            'transients': detect_transients(time_series_df)
        }
    
    return patterns

def create_comprehensive_visualizations(data, stats, patterns):
    """Create comprehensive visualizations from analysis results"""
    
    print("   Creating visualizations...")
    
    visualizations = {}
    
    if data['time_series']:
        time_series_df = pd.DataFrame(data['time_series'])
        
        # Create multi-panel visualization
        fig, axes = plt.subplots(3, 3, figsize=(15, 12))
        fig.suptitle('Comprehensive Brain Activity Analysis', fontsize=16, fontweight='bold')
        
        # Panel 1: Firing rate over time
        ax = axes[0, 0]
        ax.plot(time_series_df['step'], time_series_df['firing_rate'], 'b-', linewidth=2, label='Firing Rate')
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Neural Activity Over Time')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Panel 2: Development stages
        ax = axes[0, 1]
        development_counts = time_series_df['development_stage'].value_counts().sort_index()
        ax.bar(development_counts.index, development_counts.values, alpha=0.7, color='green')
        ax.set_xlabel('Development Stage')
        ax.set_ylabel('Duration')
        ax.set_title('Time Spent in Development Stages')
        ax.grid(True, alpha=0.3)
        
        # Panel 3: Neuromodulator levels
        ax = axes[0, 2]
        ax.plot(time_series_df['step'], time_series_df['dopamine'], 'r-', linewidth=2, label='Dopamine')
        ax.plot(time_series_df['step'], time_series_df['curiosity'], 'orange', linewidth=2, label='Curiosity')
        ax.plot(time_series_df['step'], time_series_df['novelty'], 'purple', linewidth=2, label='Novelty')
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Level')
        ax.set_title('Neuromodulator Levels Over Time')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Panel 4: Development vs Firing Rate
        ax = axes[1, 0]
        scatter = ax.scatter(time_series_df['development_stage'], 
                           time_series_df['firing_rate'], 
                           c=time_series_df['dopamine'], 
                           cmap='viridis', alpha=0.6, s=20)
        ax.set_xlabel('Development Stage')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Development Stage vs Neural Activity')
        plt.colorbar(scatter, ax=ax, label='Dopamine Level')
        ax.grid(True, alpha=0.3)
        
        # Panel 5: Statistical distribution
        ax = axes[1, 1]
        for name, color in [('Firing Rate', 'blue'), ('Dopamine', 'red'), ('Curiosity', 'orange')]:
            data = time_series_df[name.lower()]
            ax.hist(data, bins=30, alpha=0.5, color=color, density=True, label=name)
        ax.set_xlabel('Value')
        ax.set_ylabel('Probability Density')
        ax.set_title('Statistical Distributions')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Panel 6: Pattern detection
        ax = axes[1, 2]
        # Plot key patterns from stats and patterns
        if 'time_series_stats' in stats:
            firing_rate_stats = stats['time_series_stats']['firing_rate']
            ci_lower = firing_rate_stats['confidence_interval']['lower']
            ci_upper = firing_rate_stats['confidence_interval']['upper']
            
            ax.axhline(y=firing_rate_stats['mean'], color='red', linestyle='-', label=f"Mean: {firing_rate_stats['mean']:.3f}")
            ax.axhline(y=ci_lower, color='orange', linestyle='--', label=f"95% CI: [{ci_lower:.3f}, {ci_upper:.3f}]")
            ax.axhline(y=ci_upper, color='orange', linestyle='--')
            
            # Add data points
            ax.scatter(time_series_df['step'], time_series_df['firing_rate'], 
                      alpha=0.3, s=5, color='blue')
        
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Firing Rate (Hz)')
        ax.set_title('Statistical Analysis Overlay')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Panel 7: Network properties (if available)
        ax = axes[2, 0]
        if 'network_stats' in stats:
            network_stats = stats['network_stats']
            topology = network_stats['topology']
            
            metrics = ['clustering_coefficient', 'path_length', 'small_worldness']
            values = [topology[m] for m in metrics]
            
            bars = ax.bar(metrics, values, alpha=0.7, color=['skyblue', 'lightgreen', 'lightcoral'])
            ax.set_ylabel('Value')
            ax.set_title('Network Topology Metrics')
            ax.grid(True, alpha=0.3)
            
            # Add value labels on bars
            for bar, value in zip(bars, values):
                ax.text(bar.get_x() + bar.get_width()/2, bar.get_height() + 0.01, 
                       f'{value:.3f}', ha='center', va='bottom', fontsize=8)
        
        # Panel 8: Pattern summary
        ax = axes[2, 1]
        if patterns['development_patterns']:
            critical_period_progress = patterns['development_patterns']['critical_period_transition']
            ax.bar(['Critical Period'], [critical_period_progress], alpha=0.7, color='gold')
            ax.set_ylabel('Progress (0-1)')
            ax.set_title('Critical Period Progress')
            ax.set_ylim(0, 1)
            ax.grid(True, alpha=0.3)
        
        # Pattern indicators
        pattern_text = []
        if patterns['development_patterns'].get('critical_period_transition', False):
            pattern_text.append('✅ Critical period detected')
        
        if patterns['neural_patterns'].get('burst_patterns', {}).get('bursts_present', False):
            pattern_text.append('✅ Burst activity detected')
        
        if patterns['memory_patterns'].get('consolidation_patterns', {}).get('active_consolidation', False):
            pattern_text.append('✅ Active memory consolidation')
        
        if patterns['neuromodulation_patterns'].get('dopamine_regulation', {}).get('stable', False):
            pattern_text.append('✅ Stable dopamine regulation')
        
        ax.text(0.5, 0.5, '\n'.join(pattern_text), 
               transform=ax.transAxes, fontsize=10, 
               verticalalignment='center', horizontalalignment='center',
               bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.axis('off')
        ax.set_title('Pattern Detection Summary')
        
        # Panel 9: Statistical summary
        ax = axes[2, 2]
        ax.axis('off')
        
        # Create summary statistics table
        summary_text = []
        if 'time_series_stats' in stats:
            ts_stats = stats['time_series_stats']
            fr_stats = ts_stats['firing_rate']
            
            summary_text.extend([
                f"Firing Rate Statistics:",
                f"  Mean: {fr_stats['mean']:.3f} Hz",
                f"  Std: {fr_stats['std']:.3f} Hz",
                f"  95% CI: [{fr_stats['confidence_interval']['lower']:.3f}, {fr_stats['confidence_interval']['upper']:.3f}]",
                f"",
                f"Development Stages: {time_series_df['development_stage'].nunique()} unique stages",
                f"Time steps analyzed: {len(time_series_df)}",
                f"",
                f"Key Metrics:",
                f"  Stability Score: {calculate_stability_score(time_series_df):.3f}",
                f"  Development Progress: {calculate_development_progress(time_series_df):.3f}",
                f"  Neuromodulation Stability: {calculate_neuromodulation_stability(time_series_df):.3f}"
            ])
        
        ax.text(0.1, 0.9, '\n'.join(summary_text), 
               transform=ax.transAxes, fontsize=9, 
               verticalalignment='top', fontfamily='monospace',
               bbox=dict(boxstyle='round', facecolor='white', alpha=0.9, edgecolor='gray'))
        ax.set_title('Summary Statistics')
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        
        plt.tight_layout()
        visualizations['comprehensive_analysis'] = fig
    
    return visualizations

def create_comprehensive_analysis_report(data, stats, patterns, visualizations):
    """Create comprehensive analysis report"""
    
    print("   Generating comprehensive report...")
    
    report = {
        'metadata': {
            'analysis_timestamp': datetime.now().isoformat(),
            'simulation_parameters': {
                'duration_steps': len(data['time_series']) if data['time_series'] else 0,
                'total_neurons': data['time_series'][0]['total_neurons'] if data['time_series'] else 0,
                'simulation_time': calculate_simulation_time(data['time_series']) if data['time_series'] else 0
            }
        },
        
        'statistical_analysis': stats,
        
        'pattern_analysis': patterns,
        
        'visualization_summary': {
            'figures_created': list(visualizations.keys()),
            'figure_details': {
                name: {
                    'type': 'comprehensive_analysis' if name == 'comprehensive_analysis' else 'unknown',
                    'panels': 9 if name == 'comprehensive_analysis' else 0,
                    'data_points': len(data['time_series']) if data['time_series'] else 0
                }
                for name in visualizations.keys()
            }
        },
        
        'key_findings': {
            'neural_dynamics': extract_neural_dynamics_findings(stats, patterns),
            'memory_systems': extract_memory_findings(stats, patterns),
            'neuromodulation': extract_neuromodulation_findings(stats, patterns),
            'development': extract_development_findings(stats, patterns),
            'network_topology': extract_network_findings(stats, patterns)
        },
        
        'recommendations': generate_analysis_recommendations(stats, patterns),
        
        'quality_assessment': {
            'data_completeness': calculate_data_completeness(data),
            'statistical_reliability': calculate_statistical_reliability(stats),
            'pattern_confidence': calculate_pattern_confidence(patterns),
            'visualization_quality': calculate_visualization_quality(visualizations)
        },
        
        'future_research_directions': generate_future_research_directions(stats, patterns)
    }
    
    return report

# Helper functions
def calculate_confidence_interval(data, confidence=0.95):
    """Calculate confidence interval for data"""
    if len(data) < 2:
        return {'lower': data[0] if data else 0, 'upper': data[0] if data else 0, 'width': 0}
    
    mean = np.mean(data)
    std = np.std(data)
    sem = std / np.sqrt(len(data))
    
    from scipy import stats
    t_value = stats.t.ppf((1 + confidence) / 2, len(data) - 1)
    
    margin = t_value * sem
    
    return {
        'mean': mean,
        'std': std,
        'margin': margin,
        'lower': mean - margin,
        'upper': mean + margin,
        'width': 2 * margin
    }

def calculate_development_progression(development_df):
    """Calculate development progression metrics"""
    
    if len(development_df) < 2:
        return {'transitions': 0, 'progression_rate': 0}
    
    # Count stage transitions
    transitions = sum(1 for i in range(1, len(development_df)) 
                     if development_df.iloc[i]['development_stage'] != development_df.iloc[i-1]['development_stage'])
    
    progression_rate = transitions / (len(development_df) - 1)
    
    return {
        'transitions': transitions,
        'progression_rate': progression_rate,
        'final_stage': development_df.iloc[-1]['development_stage']
    }

def analyze_threshold_crossings(data, threshold):
    """Analyze threshold crossings in time series"""
    
    crossings = []
    above_threshold = False
    
    for i, value in enumerate(data):
        if not above_threshold and value > threshold:
            crossings.append({
                'time': i,
                'value': value,
                'direction': 'up'
            })
            above_threshold = True
        elif above_threshold and value < threshold:
            crossings.append({
                'time': i,
                'value': value,
                'direction': 'down'
            })
            above_threshold = False
    
    return {
        'crossings': crossings,
        'upward_crossings': len([c for c in crossings if c['direction'] == 'up']),
        'downward_crossings': len([c for c in crossings if c['direction'] == 'down']),
        'crossings_per_step': len(crossings) / len(data) if len(data) > 0 else 0
    }

def calculate_histogram(data, bins=30):
    """Calculate histogram of data"""
    
    counts, bin_edges = np.histogram(data, bins=bins)
    
    return {
        'counts': counts.tolist(),
        'bin_edges': bin_edges.tolist(),
        'bin_centers': (bin_edges[:-1] + bin_edges[1:]) / 2,
        'mean': np.mean(data),
        'std': np.std(data),
        'min': np.min(data),
        'max': np.max(data)
    }

def detect_outliers(data, method='zscore', threshold=3):
    """Detect outliers in data"""
    
    if method == 'zscore':
        z_scores = np.abs((data - np.mean(data)) / np.std(data))
        outliers = np.where(z_scores > threshold)[0]
        
        return {
            'outlier_indices': outliers.tolist(),
            'outlier_values': data[outliers].tolist(),
            'outlier_count': len(outliers),
            'total_points': len(data)
        }
    
    elif method == 'iqr':
        Q1 = np.percentile(data, 25)
        Q3 = np.percentile(data, 75)
        IQR = Q3 - Q1
        
        lower_bound = Q1 - 1.5 * IQR
        upper_bound = Q3 + 1.5 * IQR
        
        outliers = np.where((data < lower_bound) | (data > upper_bound))[0]
        
        return {
            'outlier_indices': outliers.tolist(),
            'outlier_values': data[outliers].tolist(),
            'outlier_count': len(outliers),
            'total_points': len(data),
            'bounds': {'lower': lower_bound, 'upper': upper_bound}
        }
    
    return {'outliers': [], 'outlier_count': 0, 'total_points': len(data)}

def calculate_stability(data):
    """Calculate stability of time series data"""
    
    if len(data) < 2:
        return 1.0
    
    mean_val = np.mean(data)
    std_val = np.std(data)
    
    return 1.0 - (std_val / abs(mean_val)) if mean_val != 0 else 0.0

def calculate_regulatory_effectiveness(neuromodulation_df):
    """Calculate effectiveness of neuromodulation regulation"""
    
    dopamine_levels = neuromodulation_df['dopamine_level']
    
    # Calculate stability metrics
    mean_dopamine = np.mean(dopamine_levels)
    std_dopamine = np.std(dopamine_levels)
    
    # Calculate coefficient of variation
    cv = std_dopamine / mean_dopamine if mean_dopamine > 0 else 0
    
    # Effectiveness is inverse of variability
    effectiveness = 1.0 / (1.0 + cv)
    
    return {
        'coefficient_of_variation': cv,
        'regulatory_effectiveness': effectiveness,
        'stability_score': 1.0 - min(cv, 1.0)
    }

def calculate_hub_stability(network_df):
    """Calculate stability of hub nodes in network"""
    
    if 'hub_degree' not in network_df.columns:
        return {'stability': 'unknown', 'variability': 0}
    
    hub_degrees = network_df['hub_degree']
    
    # Calculate coefficient of variation
    mean_degree = np.mean(hub_degrees)
    std_degree = np.std(hub_degrees)
    
    cv = std_degree / mean_degree if mean_degree > 0 else 0
    
    stability = 1.0 - min(cv, 1.0)
    
    return {
        'stability': stability if stability > 0.5 else 'unstable',
        'variability': cv,
        'mean_degree': mean_degree,
        'std_degree': std_degree
    }

def calculate_stage_transitions(development_df):
    """Calculate developmental stage transitions"""
    
    stages = development_df['stage']
    
    if len(stages) < 2:
        return {'transitions': 0, 'transition_matrix': {}}
    
    # Create transition matrix
    unique_stages = sorted(stages.unique())
    transition_matrix = {}
    
    for current_stage in unique_stages:
        next_stages = []
        for i in range(len(stages) - 1):
            if stages.iloc[i] == current_stage:
                next_stages.append(stages.iloc[i + 1])
        
        transition_counts = {}
        for next_stage in unique_stages:
            count = next_stages.count(next_stage)
            if count > 0:
                transition_counts[next_stage] = count / len(next_stages)
        
        transition_matrix[current_stage] = transition_counts
    
    return {
        'transitions': sum(len(transitions) for transitions in transition_matrix.values()),
        'transition_matrix': transition_matrix
    }

def calculate_maturation_rate(development_df):
    """Calculate maturation rate over time"""
    
    stages = development_df['stage']
    
    if len(stages) < 2:
        return {'rate': 0, 'trajectory': list(stages)}
    
    # Calculate time to reach final stage
    final_stage = stages.iloc[-1]
    stage_transitions = []
    
    for i in range(len(stages) - 1):
        if stages.iloc[i] != stages.iloc[i + 1]:
            stage_transitions.append({
                'from': stages.iloc[i],
                'to': stages.iloc[i + 1],
                'step': i + 1
            })
    
    return {
        'rate': len(stage_transitions) / (len(stages) - 1),
        'transitions': stage_transitions,
        'trajectory': list(stages),
        'final_stage': final_stage
    }

def calculate_stability_score(time_series_df):
    """Calculate overall stability score"""
    
    metrics = []
    
    # Firing rate stability
    fr_stability = calculate_stability(time_series_df['firing_rate'])
    metrics.append(fr_stability)
    
    # Development stability
    if 'development_stage' in time_series_df.columns:
        dev_stability = calculate_stability(time_series_df['development_stage'].astype(float))
        metrics.append(dev_stability)
    
    # Neuromodulator stability
    for modulator in ['dopamine', 'curiosity', 'novelty']:
        if modulator in time_series_df.columns:
            mod_stability = calculate_stability(time_series_df[modulator])
            metrics.append(mod_stability)
    
    return np.mean(metrics) if metrics else 0.0

def calculate_development_progress(time_series_df):
    """Calculate development progress"""
    
    if 'development_stage' not in time_series_df.columns:
        return 0.0
    
    stages = time_series_df['development_stage'].astype(float)
    final_stage = stages.iloc[-1]
    initial_stage = stages.iloc[0]
    
    max_possible_progress = final_stage - initial_stage
    if max_possible_progress > 0:
        return (final_stage - initial_stage) / max_possible_progress
    else:
        return 0.0

def calculate_neuromodulation_stability(time_series_df):
    """Calculate neuromodulation stability"""
    
    stabilities = []
    
    for modulator in ['dopamine', 'curiosity', 'novelty']:
        if modulator in time_series_df.columns:
            modulator_stability = calculate_stability(time_series_df[modulator])
            stabilities.append(modulator_stability)
    
    return np.mean(stabilities) if stabilities else 0.0

def calculate_simulation_time(time_series_data):
    """Calculate total simulation time"""
    
    if not time_series_data:
        return 0.0
    
    # Assuming dt = 0.1 for NLM
    return len(time_series_data) * 0.1

def extract_neural_dynamics_findings(stats, patterns):
    """Extract key findings from neural dynamics analysis"""
    
    findings = []
    
    if 'neural_dynamics_stats' in stats:
        nd_stats = stats['neural_dynamics_stats']
        
        if 'spike_frequency_distribution' in nd_stats:
            sfd = nd_stats['spike_frequency_distribution']
            if sfd['mean'] > 0:
                findings.append(f"Average spike frequency: {sfd['mean']:.2f} Hz")
            
            if 'outliers' in sfd and sfd['outliers']['outlier_count'] > 0:
                findings.append(f"Found {sfd['outliers']['outlier_count']} outlier spike frequencies")
        
        if 'burst_analysis' in nd_stats:
            ba = nd_stats['burst_analysis']
            findings.append(f"Burst rate: {ba['burst_rate']:.2f} bursts per second")
            findings.append(f"Average burst duration: {ba['burst_duration']:.2f} seconds")
    
    if 'neural_patterns' in patterns:
        npatterns = patterns['neural_patterns']
        if npatterns.get('burst_patterns', {}).get('bursts_present', False):
            findings.append("Burst activity detected in neural dynamics")
        
        if npatterns.get('firing_rate_patterns', {}).get('stable_periods', False):
            findings.append("Stable firing rate periods detected")
    
    return findings

def extract_memory_findings(stats, patterns):
    """Extract key findings from memory analysis"""
    
    findings = []
    
    if 'memory_stats' in stats:
        ms = stats['memory_stats']
        
        if 'working_memory' in ms:
            wm = ms['working_memory']
            findings.append(f"Working memory efficiency: {wm['efficiency']:.2%}")
        
        if 'episodic_memory' in ms:
            em = ms['episodic_memory']
            findings.append(f"Episodic memory retention: {em['retention']:.2%}")
    
    if 'memory_patterns' in patterns:
        mp = patterns['memory_patterns']
        if mp.get('consolidation_patterns', {}).get('active_consolidation', False):
            findings.append("Active memory consolidation detected")
    
    return findings

def extract_neuromodulation_findings(stats, patterns):
    """Extract key findings from neuromodulation analysis"""
    
    findings = []
    
    if 'neuromodulation_stats' in stats:
        ns = stats['neuromodulation_stats']
        
        if 'dopamine_dynamics' in ns:
            dd = ns['dopamine_dynamics']
            findings.append(f"Dopamine regulatory effectiveness: {dd['regulatory_effectiveness']:.2%}")
            findings.append(f"Dopamine stability score: {dd['stability_score']:.2%}")
    
    return findings

def extract_development_findings(stats, patterns):
    """Extract key findings from development analysis"""
    
    findings = []
    
    if 'development_stats' in stats:
        ds = stats['development_stats']
        
        if 'plasticity' in ds:
            p = ds['plasticity']
            findings.append(f"Plasticity rate: {p['rate']:.2f}")
            findings.append(f"Critical period progress: {p['critical_period_progress']:.2%}")
    
    if 'development_patterns' in patterns:
        dp = patterns['development_patterns']
        if dp.get('critical_period_transition', False):
            findings.append("Critical period transition detected")
    
    return findings

def extract_network_findings(stats, patterns):
    """Extract key findings from network analysis"""
    
    findings = []
    
    if 'network_stats' in stats:
        ns = stats['network_stats']
        
        if 'topology' in ns:
            t = ns['topology']
            findings.append(f"Network small-worldness: {t['small_worldness']:.2f}")
            findings.append(f"Clustering coefficient: {t['clustering_coefficient']:.3f}")
            findings.append(f"Characteristic path length: {t['path_length']:.3f}")
    
    if 'network_patterns' in patterns:
        np = patterns['network_patterns']
        if np.get('community_structure', {}).get('communities_present', False):
            findings.append("Community structure detected in network")
    
    return findings

def generate_analysis_recommendations(stats, patterns):
    """Generate recommendations based on analysis results"""
    
    recommendations = []
    
    # General recommendations based on findings
    if 'time_series_stats' in stats:
        ts_stats = stats['time_series_stats']
        fr_stats = ts_stats['firing_rate']
        
        if fr_stats['std'] > fr_stats['mean'] * 0.5:
            recommendations.append("High variability in firing rates detected. Consider stabilizing neuromodulation.")
        
        if fr_stats['kurtosis'] > 3:
            recommendations.append("High kurtosis detected. Investigate burst activity patterns.")
    
    # Development recommendations
    if 'development_stats' in stats:
        ds = stats['development_stats']
        
        if 'plasticity' in ds and ds['plasticity']['critical_period_progress'] > 0.8:
            recommendations.append("Critical period nearing completion. Consider transitioning to advanced learning tasks.")
    
    # Memory recommendations
    if 'memory_stats' in stats:
        ms = stats['memory_stats']
        
        if 'working_memory' in ms and ms['working_memory']['efficiency'] < 0.5:
            recommendations.append("Working memory efficiency below optimal. Consider enhancing recurrent connectivity.")
    
    # Neuromodulation recommendations
    if 'neuromodulation_stats' in stats:
        ns = stats['neuromodulation_stats']
        
        if 'dopamine_dynamics' in ns:
            dd = ns['dopamine_dynamics']
            if dd['stability_score'] < 0.7:
                recommendations.append("Dopamine regulation instability detected. Consider neuromodulation enhancement.")
    
    return recommendations

def calculate_data_completeness(data):
    """Calculate completeness of collected data"""
    
    if not data['time_series']:
        return 0.0
    
    # Check what data types were collected
    expected_types = ['time_series', 'neural_dynamics', 'memory_analysis', 
                     'neuromodulation', 'development', 'network_topology']
    
    collected_types = [data_type for data_type in expected_types if data[data_type]]
    
    return len(collected_types) / len(expected_types)

def calculate_statistical_reliability(stats):
    """Calculate reliability of statistical analysis"""
    
    if not stats:
        return 0.0
    
    # Check sample sizes for statistical tests
    reliability_scores = []
    
    if 'time_series_stats' in stats:
        ts_stats = stats['time_series_stats']
        confidence_interval = ts_stats['firing_rate']['confidence_interval']
        
        # Wider confidence intervals indicate lower reliability
        interval_width = confidence_interval['width']
        reliability = max(0, 1 - min(interval_width / 10, 1))  # Normalize to 0-1
        reliability_scores.append(reliability)
    
    return np.mean(reliability_scores) if reliability_scores else 0.0

def calculate_pattern_confidence(patterns):
    """Calculate confidence in detected patterns"""
    
    pattern_types = ['development_patterns', 'neural_patterns', 'memory_patterns', 
                    'neuromodulation_patterns', 'network_patterns', 'temporal_patterns']
    
    confidence_scores = []
    
    for pattern_type in pattern_types:
        if pattern_type in patterns:
            # Simple confidence calculation based on pattern characteristics
            pattern_data = patterns[pattern_type]
            
            # Count significant patterns
            significant_patterns = 0
            for key, value in pattern_data.items():
                if isinstance(value, dict):
                    if value.get('present', False) or (isinstance(value.get('value', 0), (int, float)) and abs(value['value']) > 0.1):
                        significant_patterns += 1
                elif isinstance(value, (list, tuple)) and len(value) > 0:
                    significant_patterns += len(value)
            
            confidence = min(significant_patterns / 5, 1.0)  # Normalize to max 5 patterns
            confidence_scores.append(confidence)
    
    return np.mean(confidence_scores) if confidence_scores else 0.0

def calculate_visualization_quality(visualizations):
    """Calculate quality of created visualizations"""
    
    if not visualizations:
        return 0.0
    
    # Simple quality assessment based on figure count and types
    quality_score = 0.0
    
    for name, fig in visualizations.items():
        if name == 'comprehensive_analysis':
            # Comprehensive analysis gets high score
            quality_score += 1.0
        else:
            # Other figures get partial score
            quality_score += 0.5
    
    # Normalize to maximum possible score (9 panels in comprehensive analysis)
    return min(quality_score / 9.0, 1.0)

def generate_future_research_directions(stats, patterns):
    """Generate future research directions based on analysis"""
    
    directions = []
    
    # Generate directions based on findings
    if 'development_stats' in stats:
        ds = stats['development_stats']
        
        if 'plasticity' in ds and ds['plasticity']['critical_period_progress'] > 0.7:
            directions.append("Investigating critical period extension mechanisms")
            directions.append("Developing interventions to prolong critical periods")
    
    if 'memory_stats' in stats:
        ms = stats['memory_stats']
        
        if 'working_memory' in ms and ms['working_memory']['efficiency'] < 0.5:
            directions.append("Enhancing working memory capacity through connectivity optimization")
            directions.append("Developing memory training protocols based on consolidation patterns")
    
    if 'neuromodulation_stats' in stats:
        ns = stats['neuromodulation_stats']
        
        if 'dopamine_dynamics' in ns:
            dd = ns['dopamine_dynamics']
            if dd['stability_score'] < 0.7:
                directions.append("Understanding dopamine regulation mechanisms")
                directions.append("Developing pharmacological interventions for neuromodulation")
    
    # Add general research directions
    directions.extend([
        "Multi-region brain network interactions",
        "Real-time adaptive learning algorithms",
        "Cross-species comparative studies",
        "Clinical applications of NLM findings"
    ])
    
    return directions

# Run comprehensive brain analysis
print("Starting comprehensive brain activity analysis...")
print("This will analyze neural dynamics, memory systems, neuromodulation, and development.")
print("Please wait while data is collected and analyzed...\n")

# Create test brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Run comprehensive analysis
analysis_data, statistical_analysis, pattern_analysis, visualizations, comprehensive_report = comprehensive_brain_analysis(brain, 1000)

print("\n=== Comprehensive Brain Activity Analysis Complete ===")
print(f"Total data points collected: {len(analysis_data['time_series'])}")
print(f"Statistical categories analyzed: {len(statistical_analysis)}")
print(f"Pattern types detected: {len(pattern_analysis)}")
print(f"Visualizations created: {len(visualizations)}")

print("\n=== Key Findings ===")
for category, findings in comprehensive_report['key_findings'].items():
    print(f"{category.replace('_', ' ').title()}:")
    for finding in findings:
        print(f"  • {finding}")

print(f"\n=== Quality Assessment ===")
qa = comprehensive_report['quality_assessment']
print(f"Data completeness: {qa['data_completeness']:.1%}")
print(f"Statistical reliability: {qa['statistical_reliability']:.1%}")
print(f"Pattern confidence: {qa['pattern_confidence']:.1%}")
print(f"Visualization quality: {qa['visualization_quality']:.1%}")

print(f"\n=== Analysis Recommendations ===")
for recommendation in comprehensive_report['recommendations']:
    print(f"• {recommendation}")

print(f"\n=== Future Research Directions ===")
for direction in comprehensive_report['future_research_directions'][:3]:
    print(f"• {direction}")

print("\n✅ Comprehensive brain activity analysis complete!")
print("The analysis provides deep insights into neural dynamics, memory systems, and brain development.")
```