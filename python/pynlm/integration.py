pynlm/integration.py
Name: Integration module
Summary: Integration with popular Python libraries and environments

from pynlm.interface import ScriptingInterface
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass
import json
import pickle
import sys


class NumPyIntegration:
    """NumPy integration for NLM brain data"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.data_cache = {}
        self.array_operations = {}
    
    def get_numpy_arrays(self, data_keys: List[str]) -> Dict[str, Any]:
        """Get brain data as NumPy arrays"""
        arrays = {}
        
        for key in data_keys:
            if key == 'brain_state':
                state = self.interface.get_state()
                arrays['brain_state'] = pd.DataFrame([state])
            
            elif key == 'region_data':
                state = self.interface.get_state()
                if 'regions' in state:
                    arrays['region_data'] = pd.DataFrame(state['regions'])
            
            elif key == 'neuron_activity':
                if self.interface.context.brain:
                    regions = self.interface.context.brain.getRegions()
                    activity_data = []
                    
                    for region in regions:
                        region_id = region.getId()
                        neuron_count = region.getNeuronCount()
                        active_count = region.getActiveCount()
                        
                        activity_data.append({
                            'region_id': region_id,
                            'neuron_count': neuron_count,
                            'active_count': active_count,
                            'activity_rate': active_count / max(neuron_count, 1)
                        })
                    
                    arrays['neuron_activity'] = pd.DataFrame(activity_data)
            
            elif key == 'synaptic_connections':
                if self.interface.context.brain:
                    regions = self.interface.context.brain.getRegions()
                    connections = []
                    
                    for region in regions:
                        region_id = region.getId()
                        for neuron in region.getNeurons():
                            for synapse in neuron.getSynapses():
                                connections.append({
                                    'source_neuron': neuron.getId(),
                                    'target_neuron': synapse.getTargetId(),
                                    'weight': synapse.getStrength(),
                                    'active': synapse.getActive(),
                                    'type': synapse.getType()
                                })
                    
                    arrays['synaptic_connections'] = pd.DataFrame(connections)
            
            elif key == 'spike_trains':
                if self.interface.context.brain:
                    regions = self.interface.context.brain.getRegions()
                    spike_data = []
                    
                    for region in regions:
                        region_id = region.getId()
                        total_spikes = region.getTotalSpikeCount()
                        
                        spike_data.append({
                            'region_id': region_id,
                            'total_spikes': total_spikes,
                            'spike_rate': total_spikes / max(region.getSimulationTime(), 1)
                        })
                    
                    arrays['spike_trains'] = pd.DataFrame(spike_data)
        
        return arrays
    
    def compute_neural_statistics(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Compute neural statistics from NumPy arrays"""
        stats = {}
        
        if 'region_data' in data:
            region_df = data['region_data']
            stats['region_stats'] = {
                'mean_neurons': region_df['neuron_count'].mean(),
                'std_neurons': region_df['neuron_count'].std(),
                'max_neurons': region_df['neuron_count'].max(),
                'min_neurons': region_df['neuron_count'].min(),
                'total_neurons': region_df['neuron_count'].sum()
            }
        
        if 'neuron_activity' in data:
            activity_df = data['neuron_activity']
            stats['activity_stats'] = {
                'mean_activity': activity_df['activity_rate'].mean(),
                'std_activity': activity_df['activity_rate'].std(),
                'max_activity': activity_df['activity_rate'].max(),
                'min_activity': activity_df['activity_rate'].min(),
                'coefficient_of_variation': (
                    activity_df['activity_rate'].std() / 
                    activity_df['activity_rate'].mean()
                ) if activity_df['activity_rate'].mean() > 0 else 0
            }
        
        if 'synaptic_connections' in data:
            syn_df = data['synaptic_connections']
            stats['synaptic_stats'] = {
                'total_connections': len(syn_df),
                'mean_weight': syn_df['weight'].mean(),
                'std_weight': syn_df['weight'].std(),
                'max_weight': syn_df['weight'].max(),
                'min_weight': syn_df['weight'].min(),
                'active_connections': syn_df['active'].sum(),
                'connection_density': len(syn_df) / (len(syn_df.index.unique()) ** 2)
            }
        
        if 'spike_trains' in data:
            spike_df = data['spike_trains']
            stats['spike_stats'] = {
                'total_spikes': spike_df['total_spikes'].sum(),
                'mean_spike_rate': spike_df['spike_rate'].mean(),
                'std_spike_rate': spike_df['spike_rate'].std(),
                'max_spike_rate': spike_df['spike_rate'].max(),
                'min_spike_rate': spike_df['spike_rate'].min()
            }
        
        return stats
    
    def export_to_numpy(self, data: Dict[str, Any], filepath: str, 
                       compression: bool = False) -> bool:
        """Export data to NumPy file format"""
        try:
            np.savez(filepath, **data)
            
            if compression:
                # Compress with gzip (not directly supported by np.savez)
                # For simplicity, we'll just use the regular save
                pass
            
            logger = __import__('logging').getLogger(__name__)
            logger.info(f"Data exported to NumPy: {filepath}")
            
            return True
        except Exception as e:
            logger = __import__('logging').getLogger(__name__)
            logger.error(f"Failed to export to NumPy: {e}")
            return False
    
    def load_from_numpy(self, filepath: str) -> Dict[str, Any]:
        """Load data from NumPy file format"""
        try:
            data = np.load(filepath, allow_pickle=True)
            return {key: data[key].item() if hasattr(data[key], 'item') else data[key]
                   for key in data.files}
        except Exception as e:
            logger = __import__('logging').getLogger(__name__)
            logger.error(f"Failed to load from NumPy: {e}")
            return {}


class PandasIntegration:
    """Pandas integration for NLM brain data analysis"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.dataframes = {}
        self.analysis_cache = {}
    
    def get_dataframes(self, data_types: List[str]) -> Dict[str, pd.DataFrame]:
        """Get brain data as Pandas DataFrames"""
        dataframes = {}
        
        for data_type in data_types:
            if data_type == 'simulation_log':
                # Create simulation log DataFrame
                log_data = self._get_simulation_log()
                dataframes['simulation_log'] = pd.DataFrame(log_data)
            
            elif data_type == 'region_time_series':
                # Create region time series DataFrame
                time_series_data = self._get_region_time_series()
                dataframes['region_time_series'] = pd.DataFrame(time_series_data)
            
            elif data_type == 'neuron_behavior':
                # Create neuron behavior DataFrame
                behavior_data = self._get_neuron_behavior_data()
                dataframes['neuron_behavior'] = pd.DataFrame(behavior_data)
            
            elif data_type == 'action_sequence':
                # Create action sequence DataFrame
                action_data = self._get_action_sequence_data()
                dataframes['action_sequence'] = pd.DataFrame(action_data)
        
        return dataframes
    
    def _get_simulation_log(self) -> List[Dict[str, Any]]:
        """Get simulation log data"""
        if not self.interface.context.running:
            return []
        
        log_entries = []
        current_time = time.time()
        
        # Get brain state
        state = self.interface.get_state()
        
        # Get recent events
        events = self.interface.context.event_queue.queue if hasattr(self.interface.context.event_queue, 'queue') else []
        
        log_entries.append({
            'timestamp': current_time,
            'event_type': 'simulation_state',
            'total_neurons': state['total_neurons'],
            'active_neurons': state['active_neurons'],
            'firing_rate': state['average_firing_rate'],
            'simulation_time': current_time - self.interface.context.simulation_start_time
        })
        
        return log_entries
    
    def _get_region_time_series(self) -> List[Dict[str, Any]]:
        """Get region time series data"""
        if not self.interface.context.brain:
            return []
        
        time_series = []
        current_time = time.time()
        
        regions = self.interface.context.brain.getRegions()
        
        for region in regions:
            region_id = region.getId()
            region_state = region.getState()
            
            time_series.append({
                'timestamp': current_time,
                'region_id': region_id,
                'neuron_count': region_state.get('neuron_count', 0),
                'active_count': region_state.get('active_count', 0),
                'firing_rate': region_state.get('firing_rate', 0),
                'excitement_ratio': region_state.get('excitement_ratio', 0)
            })
        
        return time_series
    
    def _get_neuron_behavior_data(self) -> List[Dict[str, Any]]:
        """Get neuron behavior data"""
        if not self.interface.context.brain:
            return []
        
        behavior_data = []
        current_time = time.time()
        
        regions = self.interface.context.brain.getRegions()
        
        for region in regions:
            region_id = region.getId()
            neurons = region.getNeurons()
            
            for neuron in neurons:
                neuron_id = neuron.getId()
                neuron_state = neuron.getState()
                
                behavior_data.append({
                    'timestamp': current_time,
                    'region_id': region_id,
                    'neuron_id': neuron_id,
                    'type': neuron.getType(),
                    'firing_state': neuron.getFiringState(),
                    'potential': neuron.getMembranePotential(),
                    'refractory_time': neuron.getRefractoryTime(),
                    'synaptic_strength': neuron.getSynapticStrength()
                })
        
        return behavior_data
    
    def _get_action_sequence_data(self) -> List[Dict[str, Any]]:
        """Get action sequence data"""
        # This would track actions produced by the brain
        # For this example, we'll create a placeholder
        return []
    
    def analyze_data(self, dataframes: Dict[str, pd.DataFrame]) -> Dict[str, Any]:
        """Perform data analysis on DataFrames"""
        analysis_results = {}
        
        for name, df in dataframes.items():
            if name == 'simulation_log':
                analysis_results['simulation_log'] = {
                    'mean_neurons': df['total_neurons'].mean(),
                    'std_neurons': df['total_neurons'].std(),
                    'neuron_growth_rate': (df['total_neurons'].iloc[-1] - df['total_neurons'].iloc[0]) / 
                                         max(df['simulation_time'].iloc[-1] - df['simulation_time'].iloc[0], 1)
                }
            
            elif name == 'region_time_series':
                if not df.empty:
                    analysis_results['region_time_series'] = {
                        'total_regions': df['region_id'].nunique(),
                        'mean_neuron_count': df['neuron_count'].mean(),
                        'std_neuron_count': df['neuron_count'].std(),
                        'total_activity': df['active_count'].sum(),
                        'average_activity_rate': df['active_count'].sum() / df['neuron_count'].sum()
                    }
            
            elif name == 'neuron_behavior':
                if not df.empty:
                    analysis_results['neuron_behavior'] = {
                        'total_neurons': len(df),
                        'firing_state_distribution': df['firing_state'].value_counts().to_dict(),
                        'mean_potential': df['potential'].mean(),
                        'std_potential': df['potential'].std(),
                        'high_activity_neurons': len(df[df['firing_state'] == 'Active'])
                    }
        
        return analysis_results
    
    def generate_report(self, dataframes: Dict[str, pd.DataFrame], 
                       output_format: str = 'markdown') -> str:
        """Generate analysis report"""
        analysis_results = self.analyze_data(dataframes)
        
        if output_format == 'markdown':
            return self._generate_markdown_report(analysis_results)
        elif output_format == 'csv':
            return self._generate_csv_report(analysis_results)
        elif output_format == 'json':
            return json.dumps(analysis_results, indent=2)
        else:
            raise ValueError(f"Unsupported output format: {output_format}")
    
    def _generate_markdown_report(self, analysis_results: Dict[str, Any]) -> str:
        """Generate markdown report"""
        report = "# NLM Brain Data Analysis Report\n\n"
        
        for analysis_type, results in analysis_results.items():
            report += f"## {analysis_type.replace('_', ' ').title()}\n\n"
            
            for key, value in results.items():
                if isinstance(value, dict):
                    report += f"### {key.replace('_', ' ').title()}\n"
                    for subkey, subvalue in value.items():
                        report += f"- {subkey.replace('_', ' ')}: {subvalue}\n"
                else:
                    report += f"- {key.replace('_', ' ')}: {value}\n"
            
            report += "\n"
        
        return report
    
    def _generate_csv_report(self, analysis_results: Dict[str, Any]) -> str:
        """Generate CSV report"""
        import io
        
        output = io.StringIO()
        
        for analysis_type, results in analysis_results.items():
            output.write(f"{analysis_type},\n")
            
            for key, value in results.items():
                if isinstance(value, dict):
                    for subkey, subvalue in value.items():
                        output.write(f"{key},{subkey},{subvalue}\n")
                else:
                    output.write(f"{key},{value}\n")
            
            output.write("\n")
        
        return output.getvalue()
    
    def export_to_csv(self, dataframes: Dict[str, pd.DataFrame], filepath: str):
        """Export DataFrames to CSV files"""
        for name, df in dataframes.items():
            df.to_csv(f"{filepath}_{name}.csv", index=False)
        
        logger = __import__('logging').getLogger(__name__)
        logger.info(f"Data exported to CSV files from {filepath}")


class MatplotlibIntegration:
    """Matplotlib integration for NLM visualization"""
    
    def __init__(self, interface: ScriptingInterface):
        self.interface = interface
        self.figures = {}
        self.plot_templates = {}
        self.custom_styles = {}
    
    def create_plots(self, plot_types: List[str], data: Dict[str, Any],
                    save_paths: Optional[Dict[str, str]] = None) -> Dict[str, Any]:
        """Create plots from brain data"""
        plots = {}
        
        for plot_type in plot_types:
            if plot_type == 'brain_state':
                fig = self._create_brain_state_plot(data.get('brain_state', {}))
            elif plot_type == 'region_comparison':
                fig = self._create_region_comparison_plot(data.get('regions', []))
            elif plot_type == 'neuron_activity':
                fig = self._create_neuron_activity_plot(data.get('neurons', []))
            elif plot_type == 'spike_patterns':
                fig = self._create_spike_pattern_plot(data.get('spikes', []))
            else:
                raise ValueError(f"Unknown plot type: {plot_type}")
            
            plots[plot_type] = fig
            
            if save_paths and plot_type in save_paths:
                fig.savefig(save_paths[plot_type], dpi=300, bbox_inches='tight')
                logger = __import__('logging').getLogger(__name__)
                logger.info(f"Plot saved: {save_paths[plot_type]}")
        
        return plots
    
    def _create_brain_state_plot(self, state: Dict[str, Any]):
        """Create brain state visualization"""
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        fig.suptitle('NLM Brain State', fontsize=16)
        
        # Plot 1: Neuron count
        axes[0, 0].pie([state['total_neurons'], state['total_synapses']], 
                     labels=['Neurons', 'Synapses'], autopct='%1.1f%%',
                     colors=['lightblue', 'lightcoral'])
        axes[0, 0].set_title('Neural Architecture')
        
        # Plot 2: Activity distribution
        activity_data = [state['active_neurons'], state['firing_neurons'], 
                        state['total_neurons'] - state['active_neurons'] - state['firing_neurons']]
        axes[0, 1].bar(['Active', 'Firing', 'Inactive'], activity_data, 
                      color=['green', 'orange', 'red'], alpha=0.7)
        axes[0, 1].set_title('Neuron Activity States')
        axes[0, 1].set_ylabel('Count')
        
        # Plot 3: Firing rate
        axes[1, 0].bar(['Average Firing Rate'], [state['average_firing_rate']],
                      color='skyblue', alpha=0.8)
        axes[1, 0].set_title('Average Firing Rate')
        axes[1, 0].set_ylabel('Hz')
        axes[1, 0].set_ylim(0, max(100, state['average_firing_rate'] * 1.1))
        
        # Plot 4: Excitement/Inhibition ratio
        ratio = state['excitement_inhibition_ratio']
        axes[1, 1].bar(['Excitement/Inhibition'], [ratio],
                      color='gold' if ratio > 1 else 'lightblue', alpha=0.8)
        axes[1, 1].set_title('Excitement/Inhibition Ratio')
        axes[1, 1].set_ylabel('Ratio')
        axes[1, 1].set_ylim(0, max(10, ratio * 1.1))
        
        plt.tight_layout()
        return fig
    
    def _create_region_comparison_plot(self, regions: List[Dict[str, Any]]):
        """Create region comparison plot"""
        if not regions:
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        fig.suptitle('Region Comparison', fontsize=16)
        
        region_names = [r.get('id', f'Region_{i}') for i, r in enumerate(regions)]
        neuron_counts = [r.get('neuron_count', 0) for r in regions]
        active_counts = [r.get('active_count', 0) for r in regions]
        
        # Plot 1: Bar chart
        x_pos = np.arange(len(region_names))
        width = 0.35
        
        axes[0, 0].bar(x_pos - width/2, neuron_counts, width, 
                       label='Total Neurons', alpha=0.8)
        axes[0, 0].bar(x_pos + width/2, active_counts, width, 
                       label='Active Neurons', alpha=0.8)
        
        axes[0, 0].set_xlabel('Regions')
        axes[0, 0].set_ylabel('Neuron Count')
        axes[0, 0].set_title('Neurons by Region')
        axes[0, 0].set_xticks(x_pos)
        axes[0, 0].set_xticklabels(region_names, rotation=45, ha='right')
        axes[0, 0].legend()
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Activity heatmap
        activity_matrix = np.array([
            [r.get('active_count', 0), r.get('neuron_count', 1) - r.get('active_count', 0)]
            for r in regions
        ])
        
        im = axes[0, 1].imshow(activity_matrix, cmap='RdYlGn', aspect='auto', vmin=0, vmax=1)
        axes[0, 1].set_xticks([0, 1])
        axes[0, 1].set_xticklabels(['Active', 'Inactive'])
        axes[0, 1].set_yticks(range(len(region_names)))
        axes[0, 1].set_yticklabels(region_names)
        axes[0, 1].set_title('Activity Heatmap')
        plt.colorbar(im, ax=axes[0, 1])
        
        # Plot 3: Activity ratio
        activity_ratios = [active / max(count, 1) for active, count in zip(active_counts, neuron_counts)]
        axes[1, 0].bar(range(len(region_names)), activity_ratios, 
                      color='green' if x > 0.5 else 'orange' if x > 0.2 else 'red' 
                      for x in activity_ratios)
        axes[1, 0].set_xlabel('Regions')
        axes[1, 0].set_ylabel('Activity Ratio')
        axes[1, 0].set_title('Activity Ratio by Region')
        axes[1, 0].set_xticks(range(len(region_names)))
        axes[1, 0].set_xticklabels(region_names, rotation=45, ha='right')
        axes[1, 0].set_ylim(0, 1)
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Total spike count
        spike_counts = [r.get('total_spikes', 0) for r in regions]
        axes[1, 1].bar(range(len(region_names)), spike_counts, 
                      color='red' if x > 100 else 'orange' if x > 50 else 'green' 
                      for x in spike_counts)
        axes[1, 1].set_xlabel('Regions')
        axes[1, 1].set_ylabel('Total Spikes')
        axes[1, 1].set_title('Spike Count by Region')
        axes[1, 1].set_xticks(range(len(region_names)))
        axes[1, 1].set_xticklabels(region_names, rotation=45, ha='right')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def _create_neuron_activity_plot(self, neurons: List[Dict[str, Any]]):
        """Create neuron activity plot"""
        if not neurons:
            return None
        
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        fig.suptitle('Neuron Activity Analysis', fontsize=16)
        
        neuron_ids = [n.get('id', i) for i, n in enumerate(neurons)]
        activities = [n.get('activity', n.get('potential', 0)) for n in neurons]
        types = [n.get('type', 'Internal') for n in neurons]
        firing_states = [n.get('firing_state', 'Resting') for n in neurons]
        
        # Plot 1: Activity distribution
        axes[0, 0].hist(activities, bins=20, alpha=0.7, color='blue')
        axes[0, 0].set_xlabel('Activity Level')
        axes[0, 0].set_ylabel('Frequency')
        axes[0, 0].set_title('Activity Distribution')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Neuron types
        type_counts = {}
        for neuron_type in types:
            type_counts[neuron_type] = type_counts.get(neuron_type, 0) + 1
        
        axes[0, 1].pie(type_counts.values(), labels=type_counts.keys(), autopct='%1.1f%%')
        axes[0, 1].set_title('Neuron Type Distribution')
        
        # Plot 3: Activity by type
        type_activities = {}
        for neuron_type, activity in zip(types, activities):
            if neuron_type not in type_activities:
                type_activities[neuron_type] = []
            type_activities[neuron_type].append(activity)
        
        colors = ['red', 'blue', 'green', 'orange', 'purple', 'brown']
        for i, (neuron_type, activity_list) in enumerate(type_activities.items()):
            axes[1, 0].boxplot(activity_list, positions=[i], widths=0.6,
                              labels=[neuron_type])
        
        axes[1, 0].set_xlabel('Neuron Type')
        axes[1, 0].set_ylabel('Activity Level')
        axes[1, 0].set_title('Activity by Neuron Type')
        axes[1, 0].grid(True, alpha=0.3)
        
        # Plot 4: Firing state counts
        state_counts = {}
        for state in firing_states:
            state_counts[state] = state_counts.get(state, 0) + 1
        
        axes[1, 1].bar(state_counts.keys(), state_counts.values(), 
                      color=['red', 'orange', 'yellow', 'green'], alpha=0.7)
        axes[1, 1].set_xlabel('Firing State')
        axes[1, 1].set_ylabel('Count')
        axes[1, 1].set_title('Firing State Distribution')
        axes[1, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def _create_spike_pattern_plot(self, spikes: List[Dict[str, Any]]):
        """Create spike pattern plot"""
        if not spikes:
            return None
        
        fig, axes = plt.subplots(1, 2, figsize=(15, 6))
        fig.suptitle('Spike Pattern Analysis', fontsize=16)
        
        # Plot 1: Spike count histogram
        spike_counts = [s.get('count', 0) for s in spikes]
        axes[0, 0].hist(spike_counts, bins=10, alpha=0.7, color='red')
        axes[0, 0].set_xlabel('Spike Count')
        axes[0, 0].set_ylabel('Frequency')
        axes[0, 0].set_title('Spike Count Distribution')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Plot 2: Spike rate time series
        if 'time' in spikes[0] and 'rate' in spikes[0]:
            times = [s.get('time', i) for i, s in enumerate(spikes)]
            rates = [s.get('rate', 0) for s in spikes]
            
            axes[0, 1].plot(times, rates, linewidth=2, color='red')
            axes[0, 1].set_xlabel('Time')
            axes[0, 1].set_ylabel('Spike Rate')
            axes[0, 1].set_title('Spike Rate Over Time')
            axes[0, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        return fig
    
    def create_dashboard_plot(self, data: Dict[str, Any], 
                            dashboard_type: str = 'comprehensive') -> Any:
        """Create comprehensive dashboard plot"""
        if dashboard_type == 'comprehensive':
            return self._create_comprehensive_dashboard(data)
        elif dashboard_type == 'neural_network':
            return self._create_neural_network_dashboard(data)
        elif dashboard_type == 'simulation_status':
            return self._create_simulation_status_dashboard(data)
        else:
            raise ValueError(f"Unknown dashboard type: {dashboard_type}")
    
    def _create_comprehensive_dashboard(self, data: Dict[str, Any]):
        """Create comprehensive dashboard"""
        fig = plt.figure(figsize=(20, 12))
        fig.suptitle('NLM Brain System Dashboard', fontsize=20)
        
        # Create subplots
        gs = fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        # Brain state overview
        ax1 = fig.add_subplot(gs[0, :2])
        self._plot_brain_state_overview(ax1, data)
        
        # Region activity
        ax2 = fig.add_subplot(gs[0, 2:])
        self._plot_region_activity(ax2, data.get('regions', []))
        
        # Neural activity
        ax3 = fig.add_subplot(gs[1, :2])
        self._plot_neural_activity(ax3, data.get('neurons', []))
        
        # Simulation metrics
        ax4 = fig.add_subplot(gs[1, 2:])
        self._plot_simulation_metrics(ax4, data)
        
        # Resource usage
        ax5 = fig.add_subplot(gs[2, :])
        self._plot_resource_usage(ax5, data)
        
        plt.tight_layout()
        return fig
    
    def _plot_brain_state_overview(self, ax, data: Dict[str, Any]):
        """Plot brain state overview"""
        state = data.get('brain_state', {})
        
        categories = ['Neurons', 'Synapses', 'Active', 'Firing']
        values = [
            state.get('total_neurons', 0),
            state.get('total_synapses', 0),
            state.get('active_neurons', 0),
            state.get('firing_neurons', 0)
        ]
        
        colors = ['skyblue', 'lightcoral', 'lightgreen', 'gold']
        bars = ax.bar(categories, values, color=colors, alpha=0.8)
        
        ax.set_ylabel('Count')
        ax.set_title('Brain Architecture')
        ax.grid(True, alpha=0.3)
        
        # Add value labels on bars
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{value}', ha='center', va='bottom')
    
    def _plot_region_activity(self, ax, regions: List[Dict[str, Any]]):
        """Plot region activity"""
        if not regions:
            ax.text(0.5, 0.5, 'No region data', ha='center', va='center',
                   transform=ax.transAxes)
            return
        
        region_names = [r.get('id', f'R{i}') for i, r in enumerate(regions)]
        neuron_counts = [r.get('neuron_count', 0) for r in regions]
        active_counts = [r.get('active_count', 0) for r in regions]
        
        x_pos = np.arange(len(region_names))
        width = 0.35
        
        ax.bar(x_pos - width/2, neuron_counts, width, label='Total', alpha=0.8)
        ax.bar(x_pos + width/2, active_counts, width, label='Active', alpha=0.8)
        
        ax.set_xlabel('Regions')
        ax.set_ylabel('Neuron Count')
        ax.set_title('Region Activity')
        ax.set_xticks(x_pos)
        ax.set_xticklabels(region_names, rotation=45, ha='right')
        ax.legend()
        ax.grid(True, alpha=0.3)
    
    def _plot_neural_activity(self, ax, neurons: List[Dict[str, Any]]):
        """Plot neural activity"""
        if not neurons:
            ax.text(0.5, 0.5, 'No neuron data', ha='center', va='center',
                   transform=ax.transAxes)
            return
        
        neuron_ids = [n.get('id', i) for i, n in enumerate(neurons)]
        activities = [n.get('activity', n.get('potential', 0)) for n in neurons]
        
        scatter = ax.scatter(neuron_ids, activities, alpha=0.7, s=50,
                           c=activities, cmap='RdYlBu_r', vmin=0, vmax=max(activities) if activities else 1)
        
        ax.set_xlabel('Neuron ID')
        ax.set_ylabel('Activity/Potential')
        ax.set_title('Neural Activity')
        ax.grid(True, alpha=0.3)
        plt.colorbar(scatter, ax=ax)
    
    def _plot_simulation_metrics(self, ax, data: Dict[str, Any]):
        """Plot simulation metrics"""
        metrics = data.get('simulation_metrics', {})
        
        if not metrics:
            ax.text(0.5, 0.5, 'No simulation metrics', ha='center', va='center',
                   transform=ax.transAxes)
            return
        
        categories = list(metrics.keys())
        values = list(metrics.values())
        
        colors = ['green', 'blue', 'orange', 'red', 'purple']
        bars = ax.bar(categories, values, color=colors, alpha=0.8)
        
        ax.set_ylabel('Value')
        ax.set_title('Simulation Metrics')
        ax.grid(True, alpha=0.3)
        
        # Add value labels
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{value:.2f}', ha='center', va='bottom')
    
    def _plot_resource_usage(self, ax, data: Dict[str, Any]):
        """Plot resource usage"""
        resources = data.get('resource_usage', {})
        
        categories = ['Memory', 'CPU', 'Threads', 'Time']
        values = [
            resources.get('memory_mb', 0),
            resources.get('cpu_percent', 0),
            resources.get('thread_count', 0),
            resources.get('execution_time_ms', 0)
        ]
        
        colors = ['red', 'blue', 'green', 'orange']
        bars = ax.bar(categories, values, color=colors, alpha=0.8)
        
        ax.set_ylabel('Usage')
        ax.set_title('Resource Usage')
        ax.grid(True, alpha=0.3)
        
        # Add value labels
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{value:.1f}', ha='center', va='bottom')


# Import statements for missing dependencies
import time
import os

try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False
    np = None

try:
    import pandas as pd
    PANDAS_AVAILABLE = True
except ImportError:
    PANDAS_AVAILABLE = False
    pd = None

try:
    import matplotlib.pyplot as plt
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False
    plt = None

# Export key classes and functions
__all__ = [
    'NumPyIntegration',
    'PandasIntegration',
    'MatplotlibIntegration'
]
