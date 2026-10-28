# NLM Visualization Utilities

This module provides advanced visualization utilities for NLM (Neural Learning Machine) simulations, making it easier to create, customize, and manage visual representations of neural activity and simulation data.

## Overview

The visualization system provides:

1. **Brain visualization** with multiple rendering modes
2. **Activity visualization** showing neural dynamics over time
3. **Weight matrix visualization** for synaptic connectivity patterns
4. **Time series visualization** for simulation metrics
5. **Comparison tools** for multiple simulation runs
6. **Animation utilities** for creating animated visualizations
7. **Export functionality** for sharing and documentation

## Key Features

### 1. Brain Visualization

#### Network Graph Visualization

```python
import matplotlib.pyplot as plt
import networkx as nx
import numpy as np
from nlm.utils import VisualizationUtils

class BrainVisualizer:
    """Advanced brain visualization utilities."""
    
    def __init__(self):
        self.default_colors = {
            'sensory': '#FF6B6B',      # Red
            'motor': '#4ECDC4',       # Teal
            'internal': '#45B7D1',    # Blue
            'excitatory': '#96CEB4',  # Green
            'inhibitory': '#FFE5B4',  # Yellow
            'modulatory': '#FF9F43'   # Orange
        }
    
    def create_neural_network_graph(self, brain: pynlm.Brain, 
                                   region_id: Optional[int] = None,
                                   max_neurons: int = 100) -> plt.Figure:
        """Create a neural network graph visualization."""
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        axes = axes.flatten()
        
        # Get brain regions
        regions = brain.getRegions() if region_id is None else [brain.getRegion(RegionId(region_id))]
        
        for i, region in enumerate(regions[:4]):  # Limit to 4 regions for clarity
            ax = axes[i]
            
            # Get neurons and synapses from this region
            neurons = region.getAllNeurons()
            synapses = region.getSynapses()
            
            # Limit number of neurons for visualization
            neurons = neurons[:min(max_neurons, len(neurons))]
            
            # Create networkx graph
            G = nx.Graph()
            
            # Add neurons as nodes
            for neuron in neurons:
                neuron_id = neuron.getId().index()
                neuron_type = str(neuron.getType())
                
                G.add_node(
                    neuron_id,
                    type=neuron_type,
                    color=self.default_colors.get(neuron_type, '#CCCCCC'),
                    size=100 + neuron.getFiringRate() * 500,
                    label=str(neuron_id)
                )
            
            # Add synapses as edges
            for synapse in synapses[:min(200, len(synapses))]:  # Limit edges for clarity
                source_id = synapse.getSourceNeuron().index()
                target_id = synapse.getDestinationNeuron().index()
                weight = synapse.getWeight()
                
                if source_id in G.nodes() and target_id in G.nodes():
                    G.add_edge(
                        source_id, target_id,
                        weight=weight,
                        width=min(abs(weight) * 10, 5)
                    )
            
            # Create layout
            if len(G.nodes()) < 50:
                # Use force-directed layout for smaller networks
                pos = nx.spring_layout(G, seed=42, k=1, iterations=50)
            else:
                # Use circular layout for larger networks
                pos = nx.circular_layout(G, seed=42)
            
            # Draw nodes
            node_colors = [G.nodes[node]['color'] for node in G.nodes()]
            node_sizes = [G.nodes[node]['size'] for node in G.nodes()]
            
            nx.draw_networkx_nodes(G, pos, node_color=node_colors, 
                                  node_size=node_sizes, alpha=0.8, ax=ax)
            
            # Draw edges
            edge_widths = [G[u][v]['width'] for u, v in G.edges()]
            edge_colors = [G[u][v]['weight'] for u, v in G.edges()]
            
            nx.draw_networkx_edges(G, pos, width=edge_widths, 
                                  edge_color=edge_colors, cmap='RdBu', 
                                  vmin=-1, vmax=1, alpha=0.6, ax=ax)
            
            # Draw labels (sample for large networks)
            if len(G.nodes()) <= 20:
                nx.draw_networkx_labels(G, pos, ax=ax, font_size=8)
            
            ax.set_title(f"Region {i + 1}: {len(G.nodes())} neurons, {len(G.edges())} synapses")
            ax.axis('off')
        
        plt.suptitle("Neural Network Visualization", fontsize=16, fontweight='bold')
        plt.tight_layout()
        return fig
    
    def create_spike_raster_plot(self, brain: pynlm.Brain, 
                                region_id: Optional[int] = None,
                                time_window: Tuple[float, float] = (0, 1000),
                                neuron_samples: int = 50) -> plt.Figure:
        """Create spike raster plot showing neural activity over time."""
        fig, axes = plt.subplots(2, 2, figsize=(16, 12))
        axes = axes.flatten()
        
        # Get brain regions
        regions = brain.getRegions() if region_id is None else [brain.getRegion(RegionId(region_id))]
        
        for i, region in enumerate(regions[:4]):
            ax = axes[i]
            
            # Get neurons from region
            neurons = region.getAllNeurons()
            
            # Sample neurons for visualization
            if len(neurons) > neuron_samples:
                sample_indices = np.random.choice(len(neurons), neuron_samples, replace=False)
                neurons = [neurons[idx] for idx in sample_indices]
            
            # Collect spike times for sampled neurons
            spike_data = []
            neuron_ids = []
            
            for neuron in neurons:
                neuron_id = neuron.getId().index()
                
                # Get spike history (simplified for demonstration)
                spike_history = neuron.getSpikeHistory() if hasattr(neuron, 'getSpikeHistory') else []
                
                # Filter spikes within time window
                filtered_spikes = [s for s in spike_history 
                                 if time_window[0] <= s <= time_window[1]]
                
                if filtered_spikes:
                    spike_data.append(filtered_spikes)
                    neuron_ids.append(neuron_id)
            
            if spike_data:
                # Create spike raster
                for spike_times, neuron_id in zip(spike_data, neuron_ids):
                    ax.vlines(spike_times, neuron_id - 0.5, neuron_id + 0.5, 
                             color='black', linewidth=0.5, alpha=0.7)
                
                # Set up axes
                ax.set_xlim(time_window)
                ax.set_ylim(-0.5, len(neurons) - 0.5)
                ax.set_xlabel('Time (simulation steps)')
                ax.set_ylabel('Neuron ID')
                ax.set_title(f'Spike Raster - Region {i + 1}')
                
                # Add statistics
                total_spikes = sum(len(spikes) for spikes in spike_data)
                firing_rate = total_spikes / (time_window[1] - time_window[0]) / len(neuron_ids) if neuron_ids else 0
                
                ax.text(0.95, 0.95, f'Total spikes: {total_spikes}\nFiring rate: {firing_rate:.2f} Hz',
                       transform=ax.transAxes, fontsize=10, verticalalignment='top',
                       bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        plt.suptitle('Spike Raster Plots - Neural Activity Visualization', fontsize=16, fontweight='bold')
        plt.tight_layout()
        return fig
    
    def create_weight_matrix_heatmap(self, brain: pynlm.Brain,
                                     region_id: Optional[int] = None,
                                     max_neurons: int = 50) -> plt.Figure:
        """Create weight matrix heatmap showing synaptic connectivity."""
        fig, axes = plt.subplots(2, 2, figsize=(15, 12))
        axes = axes.flatten()
        
        # Get brain regions
        regions = brain.getRegions() if region_id is None else [brain.getRegion(RegionId(region_id))]
        
        for i, region in enumerate(regions[:4]):
            ax = axes[i]
            
            # Get neurons and synapses
            neurons = region.getAllNeurons()
            synapses = region.getSynapses()
            
            # Limit neurons for matrix size
            neurons = neurons[:min(max_neurons, len(neurons))]
            
            # Create weight matrix
            n_neurons = len(neurons)
            weight_matrix = np.zeros((n_neurons, n_neurons))
            
            # Fill weight matrix
            for synapse in synapses:
                source_neuron = synapse.getSourceNeuron()
                target_neuron = synapse.getDestinationNeuron()
                weight = synapse.getWeight()
                
                # Find indices
                try:
                    source_idx = next(idx for idx, n in enumerate(neurons) 
                                    if n.getId() == source_neuron)
                    target_idx = next(idx for idx, n in enumerate(neurons) 
                                    if n.getId() == target_neuron)
                    
                    # Fill matrix (consider direction)
                    weight_matrix[source_idx, target_idx] = weight
                except StopIteration:
                    continue
            
            # Create heatmap
            im = ax.imshow(weight_matrix, cmap='RdBu', vmin=-1, vmax=1, aspect='auto')
            
            # Add colorbar
            plt.colorbar(im, ax=ax, shrink=0.8)
            
            # Set labels
            ax.set_title(f'Weight Matrix - Region {i + 1}')
            ax.set_xlabel('Target Neuron')
            ax.set_ylabel('Source Neuron')
            
            # Add statistics
            non_zero_weights = np.count_nonzero(weight_matrix)
            max_weight = np.max(np.abs(weight_matrix)) if non_zero_weights > 0 else 0
            mean_weight = np.mean(weight_matrix) if non_zero_weights > 0 else 0
            
            stats_text = f'Non-zero: {non_zero_weights}\n'
            stats_text += f'Max: {max_weight:.3f}\n'
            stats_text += f'Mean: {mean_weight:.3f}'
            
            ax.text(0.95, 0.05, stats_text, transform=ax.transAxes, fontsize=9,
                   verticalalignment='bottom', horizontalalignment='right',
                   bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        
        plt.suptitle('Synaptic Weight Matrix Visualization', fontsize=16, fontweight='bold')
        plt.tight_layout()
        return fig
class ActivityVisualizer:
    """Visualizer for tracking and displaying neural activity over time."""
    
    def __init__(self):
        self.recording_buffer = {}
        self.time_series_data = {}
    
    def record_simulation_data(self, brain: pynlm.Brain, agent: pynlm.AgentBrain,
                               world: pynlm.SimpleWorld, step: int,
                               record_frequency: int = 10) -> None:
        """Record simulation data for later visualization."""
        if step % record_frequency != 0:
            return
        
        # Create unique key for this recording
        key = f"step_{step}"
        
        # Record data
        self.recording_buffer[key] = {
            'step': step,
            'timestamp': world.getSimulationTime(),
            'brain_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': str(brain.getDevelopmentalStage())
            },
            'agent_stats': {
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'curiosity_level': agent.getCuriosityLevel(),
                'novelty_level': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError(),
                'developmental_stage': str(agent.getDevelopmentalStage())
            },
            'world_stats': {
                'agent_position': {'x': agent.getBrain().getAgentBody().x, 
                                 'y': agent.getBrain().getAgentBody().y},
                'agent_energy': agent.getBrain().getAgentBody().energy,
                'agent_health': agent.getBrain().getAgentBody().health,
                'world_time': world.getSimulationTime()
            }
        }
        
        # Store in time series
        self.time_series_data[key] = self.recording_buffer[key]
    
    def create_activity_dashboard(self, steps_to_show: int = 100) -> plt.Figure:
        """Create comprehensive activity dashboard."""
        fig, axes = plt.subplots(3, 3, figsize=(18, 15))
        axes = axes.flatten()
        
        # Get recent data
        recent_keys = sorted(self.time_series_data.keys(), key=lambda x: int(x.split('_')[1]))[-steps_to_show:]
        recent_data = [self.time_series_data[key] for key in recent_keys]
        
        if not recent_data:
            fig.text(0.5, 0.5, 'No recording data available', 
                    horizontalalignment='center', verticalalignment='center',
                    transform=fig.transFigure, fontsize=16)
            return fig
        
        # Extract data series
        steps = [data['step'] for data in recent_data]
        times = [data['timestamp'] for data in recent_data]
        spike_counts = [data['brain_stats']['total_spikes'] for data in recent_data]
        firing_rates = [data['brain_stats']['average_firing_rate'] for data in recent_data]
        neuromod_levels = [data['agent_stats']['neuromodulation_level'] for data in recent_data]
        curiosity_levels = [data['agent_stats']['curiosity_level'] for data in recent_data]
        prediction_errors = [data['agent_stats']['prediction_error'] for data in recent_data]
        agent_energies = [data['world_stats']['agent_energy'] for data in recent_data]
        agent_healths = [data['world_stats']['agent_health'] for data in recent_data]
        
        # 1. Spike count over time
        axes[0].plot(steps, spike_counts, linewidth=2, color='red')
        axes[0].set_title('Total Spike Count Over Time')
        axes[0].set_xlabel('Step')
        axes[0].set_ylabel('Spike Count')
        axes[0].grid(True, alpha=0.3)
        
        # 2. Firing rate over time
        axes[1].plot(steps, firing_rates, linewidth=2, color='blue')
        axes[1].set_title('Average Firing Rate')
        axes[1].set_xlabel('Step')
        axes[1].set_ylabel('Firing Rate (Hz)')
        axes[1].grid(True, alpha=0.3)
        
        # 3. Neuromodulation levels
        axes[2].plot(steps, neuromod_levels, linewidth=2, color='green')
        axes[2].set_title('Neuromodulation Level')
        axes[2].set_xlabel('Step')
        axes[2].set_ylabel('Level')
        axes[2].grid(True, alpha=0.3)
        
        # 4. Curiosity levels
        axes[3].plot(steps, curiosity_levels, linewidth=2, color='purple')
        axes[3].set_title('Curiosity Level')
        axes[3].set_xlabel('Step')
        axes[3].set_ylabel('Level')
        axes[3].grid(True, alpha=0.3)
        
        # 5. Prediction error
        axes[4].plot(steps, prediction_errors, linewidth=2, color='orange')
        axes[4].set_title('Prediction Error')
        axes[4].set_xlabel('Step')
        axes[4].set_ylabel('Error')
        axes[4].grid(True, alpha=0.3)
        
        # 6. Agent energy
        axes[5].plot(steps, agent_energies, linewidth=2, color='brown')
        axes[5].set_title('Agent Energy')
        axes[5].set_xlabel('Step')
        axes[5].set_ylabel('Energy')
        axes[5].grid(True, alpha=0.3)
        
        # 7. Agent health
        axes[6].plot(steps, agent_healths, linewidth=2, color='cyan')
        axes[6].set_title('Agent Health')
        axes[6].set_xlabel('Step')
        axes[6].set_ylabel('Health')
        axes[6].grid(True, alpha=0.3)
        
        # 8. Multi-metric overlay
        ax = axes[7]
        
        # Create twin axes for different metrics
        ax2 = ax.twinx()
        
        # Plot different metrics
        line1 = ax.plot(steps, spike_counts, linewidth=1, color='red', alpha=0.7, label='Spikes')
        line2 = ax2.plot(steps, firing_rates, linewidth=1, color='blue', alpha=0.7, label='Firing Rate')
        
        ax.set_title('Multi-Metric Activity Dashboard')
        ax.set_xlabel('Step')
        ax.set_ylabel('Spike Count', color='red')
        ax2.set_ylabel('Firing Rate (Hz)', color='blue')
        
        # Combine legends
        lines = line1 + line2
        labels = [l.get_label() for l in lines]
        ax.legend(lines, labels, loc='upper left')
        
        # 9. Activity summary statistics
        axes[8].axis('off')
        
        stats_text = f"Simulation Statistics:\n\n"
        stats_text += f"Duration: {len(steps)} steps\n"
        stats_text += f"Total spikes: {sum(spike_counts):,}\n"
        stats_text += f"Average firing rate: {np.mean(firing_rates):.2f} Hz\n"
        stats_text += f"Firing rate std: {np.std(firing_rates):.2f} Hz\n"
        stats_text += f"Neuromodulation range: {min(neuromod_levels):.3f} - {max(neuromod_levels):.3f}\n"
        stats_text += f"Curiosity range: {min(curiosity_levels):.3f} - {max(curiosity_levels):.3f}\n"
        stats_text += f"Prediction error range: {min(prediction_errors):.3f} - {max(prediction_errors):.3f}\n"
        stats_text += f"Energy range: {min(agent_energies):.3f} - {max(agent_energies):.3f}\n"
        stats_text += f"Health range: {min(agent_healths):.3f} - {max(agent_healths):.3f}\n"
        
        axes[8].text(0.1, 0.5, stats_text, fontsize=11, fontfamily='monospace',
                    verticalalignment='center', bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.8))
        
        plt.suptitle('NLM Simulation Activity Dashboard', fontsize=16, fontweight='bold')
        plt.tight_layout()
        return fig
    
    def create_time_series_animation(self, steps_per_frame: int = 10,
                                    output_file: Optional[str] = None) -> Any:
        """Create animated time series visualization."""
        try:
            import matplotlib.animation as animation
            
            if not self.time_series_data:
                print("No recording data available for animation")
                return None
            
            # Prepare data for animation
            steps = sorted([int(key.split('_')[1]) for key in self.time_series_data.keys()])
            times = [self.time_series_data[f'step_{s}']['timestamp'] for s in steps]
            
            # Create figure and axes
            fig, axes = plt.subplots(2, 2, figsize=(14, 10))
            axes = axes.flatten()
            
            # Initialize data lines
            lines = []
            data_series = {}
            
            for i, (metric, key) in enumerate([
                ('spike_count', 'brain_stats.total_spikes'),
                ('firing_rate', 'brain_stats.average_firing_rate'),
                ('neuromodulation', 'agent_stats.neuromodulation_level'),
                ('curiosity', 'agent_stats.curiosity_level')
            ]):
                ax = axes[i]
                line, = ax.plot([], [], linewidth=2)
                lines.append(line)
                axes[i].set_title(metric.replace('_', ' ').title())
                axes[i].set_xlabel('Step')
                
                if 'spike' in metric:
                    axes[i].set_ylabel('Count')
                elif 'firing' in metric:
                    axes[i].set_ylabel('Rate (Hz)')
                else:
                    axes[i].set_ylabel('Level')
                
                axes[i].grid(True, alpha=0.3)
                data_series[metric] = line
            
            # Animation update function
            def update(frame):
                # Get current frame step
                current_step = frame * steps_per_frame
                
                # Get data up to current step
                step_indices = [s for s in steps if s <= current_step]
                
                for i, (metric, key_path) in enumerate([
                    ('spike_count', 'brain_stats.total_spikes'),
                    ('firing_rate', 'brain_stats.average_firing_rate'),
                    ('neuromodulation', 'agent_stats.neuromodulation_level'),
                    ('curiosity', 'agent_stats.curiosity_level')
                ]):
                    # Extract nested data
                    data = self.time_series_data
                    for part in key_path.split('.'):
                        data = data[part]
                    
                    # Get data up to current step
                    x_data = [s for s in steps if s <= current_step]
                    y_data = [self._get_nested_value(self.time_series_data, f'step_{s}', key_path) 
                             for s in x_data]
                    
                    lines[i].set_data(x_data, y_data)
                    
                    # Set axis limits
                    if x_data:
                        axes[i].set_xlim(min(x_data), max(x_data))
                        if y_data:
                            y_min, y_max = min(y_data), max(y_data)
                            axes[i].set_ylim(y_min - 0.1 * abs(y_min), y_max + 0.1 * abs(y_max))
                
                # Update title
                fig.suptitle(f'NLM Activity Animation - Step {current_step}', 
                           fontsize=14, fontweight='bold')
                
                return lines
            
            # Create animation
            num_frames = len(steps) // steps_per_frame + 1
            anim = animation.FuncAnimation(
                fig, update, frames=num_frames, 
                interval=200, blit=False, repeat=True
            )
            
            if output_file:
                # Save animation
                anim.save(output_file, writer='pillow', fps=5, dpi=100)
                print(f"Animation saved to {output_file}")
            
            return anim
            
        except ImportError:
            print("Matplotlib animation not available")
            return None
        except Exception as e:
            print(f"Error creating animation: {e}")
            return None
    
    def _get_nested_value(self, data: Dict, key: str, path: str) -> Any:
        """Get nested value from dictionary."""
        value = data.get(key, {})
        
        for part in path.split('.'):
            if isinstance(value, dict) and part in value:
                value = value[part]
            else:
                return None
        
        return value
class ComparisonVisualizer:
    """Visualizer for comparing multiple simulation runs or configurations."""
    
    def __init__(self):
        self.comparison_data = {}
    
    def add_comparison_run(self, name: str, episode_data: List[Dict[str, Any]]):
        """Add a simulation run for comparison."""
        self.comparison_data[name] = {
            'data': episode_data,
            'summary': self._calculate_run_summary(episode_data),
            'timestamp': time.time()
        }
    
    def _calculate_run_summary(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate summary statistics for a simulation run."""
        if not episode_data:
            return {}
        
        # Extract metrics from episode data
        total_rewards = [step['total_reward'] for step in episode_data]
        episode_lengths = [step['episode_length'] for step in episode_data]
        spike_counts = [step['brain_stats']['total_spikes'] for step in episode_data]
        firing_rates = [step['brain_stats']['average_firing_rate'] for step in episode_data]
        brain_healths = [step['final_brain_health'] for step in episode_data]
        
        return {
            'total_reward': sum(total_rewards),
            'average_reward': np.mean(total_rewards),
            'reward_std': np.std(total_rewards),
            'reward_range': [min(total_rewards), max(total_rewards)],
            'average_episode_length': np.mean(episode_lengths),
            'episode_length_std': np.std(episode_lengths),
            'total_spikes': sum(spike_counts),
            'average_firing_rate': np.mean(firing_rates),
            'firing_rate_std': np.std(firing_rates),
            'average_brain_health': np.mean(brain_healths),
            'brain_health_std': np.std(brain_healths),
            'num_episodes': len(episode_data)
        }
    
    def create_comparison_dashboard(self, metrics_to_compare: Optional[List[str]] = None) -> plt.Figure:
        """Create comparison dashboard for multiple simulation runs."""
        if not self.comparison_data:
            fig.text(0.5, 0.5, 'No comparison data available', 
                    horizontalalignment='center', verticalalignment='center',
                    transform=fig.transFigure, fontsize=16)
            return fig
        
        fig, axes = plt.subplots(3, 3, figsize=(18, 15))
        axes = axes.flatten()
        
        # Get comparison data
        runs = list(self.comparison_data.keys())
        
        if metrics_to_compare is None:
            # Default metrics to compare
            metrics_to_compare = ['total_reward', 'average_reward', 'episode_length', 
                                'average_firing_rate', 'average_brain_health', 
                                'firing_rate_std', 'brain_health_std']
        
        # Create subplots for each metric
        for i, metric in enumerate(metrics_to_compare[:9]):  # Limit to 9 metrics
            if i >= len(runs):
                axes[i].axis('off')
                continue
            
            ax = axes[i]
            
            # Get metric values for all runs
            metric_values = []
            for run_name in runs:
                summary = self.comparison_data[run_name]['summary']
                if metric in summary:
                    metric_values.append(summary[metric])
                else:
                    metric_values.append(None)
            
            # Create bar chart
            valid_values = [(val, run_name) for val, run_name in zip(metric_values, runs) if val is not None]
            if not valid_values:
                axes[i].text(0.5, 0.5, f'No data for {metric}',
                           horizontalalignment='center', verticalalignment='center')
                continue
            
            values, run_names = zip(*valid_values)
            
            bars = ax.bar(run_names, values, alpha=0.7, color=plt.cm.Set3(np.linspace(0, 1, len(runs))))
            
            # Add value labels on bars
            for bar, value in zip(bars, values):
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height,
                       f'{value:.3f}', ha='center', va='bottom', fontsize=9)
            
            ax.set_title(metric.replace('_', ' ').title())
            ax.set_ylabel('Value')
            ax.tick_params(axis='x', rotation=45)
            ax.grid(True, alpha=0.3, axis='y')
        
        # Hide unused subplots
        for i in range(len(metrics_to_compare), len(axes)):
            axes[i].axis('off')
        
        plt.suptitle('NLM Simulation Comparison Dashboard', fontsize=16, fontweight='bold')
        plt.tight_layout()
        return fig
    
    def create_pairwise_comparison(self, metric_pairs: List[Tuple[str, str]]) -> plt.Figure:
        """Create pairwise comparison plots (scatter plots, correlation matrices, etc.)."""
        if len(self.comparison_data) < 2:
            fig.text(0.5, 0.5, 'Need at least 2 runs for pairwise comparison', 
                    horizontalalignment='center', verticalalignment='center',
                    transform=fig.transFigure, fontsize=16)
            return fig
        
        fig, axes = plt.subplots(2, 2, figsize=(14, 12))
        axes = axes.flatten()
        
        runs = list(self.comparison_data.keys())
        
        for i, (metric1, metric2) in enumerate(metric_pairs[:4]):  # Limit to 4 pairs
            ax = axes[i]
            
            # Get data for both metrics
            x_values = []
            y_values = []
            run_names = []
            
            for run_name in runs:
                summary = self.comparison_data[run_name]['summary']
                
                val1 = summary.get(metric1)
                val2 = summary.get(metric2)
                
                if val1 is not None and val2 is not None:
                    x_values.append(val1)
                    y_values.append(val2)
                    run_names.append(run_name)
            
            if len(x_values) < 2:
                ax.text(0.5, 0.5, f'Insufficient data for {metric1} vs {metric2}',
                       horizontalalignment='center', verticalalignment='center')
                continue
            
            # Create scatter plot
            colors = plt.cm.Set3(np.linspace(0, 1, len(runs)))
            
            for j, (x, y, name, color) in enumerate(zip(x_values, y_values, run_names, colors)):
                ax.scatter(x, y, c=[color], s=100, alpha=0.7, label=name)
                ax.annotate(name, (x, y), xytext=(5, 5), textcoords='offset points', fontsize=8)
            
            # Add correlation line
            if len(x_values) >= 2:
                z = np.polyfit(x_values, y_values, 1)
                p = np.poly1d(z)
                x_range = np.linspace(min(x_values), max(x_values), 100)
                ax.plot(x_range, p(x_range), 'r--', alpha=0.8, linewidth=2)
                
                # Calculate correlation coefficient
                correlation = np.corrcoef(x_values, y_values)[0, 1]
                ax.text(0.05, 0.95, f'Correlation: {correlation:.3f}',
                       transform=ax.transAxes, fontsize=10,
                       bbox=dict(boxstyle='round', facecolor='white', alpha=0.8))
            
            ax.set_xlabel(metric1.replace('_', ' '))
            ax.set_ylabel(metric2.replace('_', ' '))
            ax.set_title(f'{metric1.title()} vs {metric2.title()}')
            ax.grid(True, alpha=0.3)
            ax.legend()
        
        # Hide unused subplots
        for i in range(len(metric_pairs), len(axes)):
            axes[i].axis('off')
        
        plt.suptitle('Pairwise Comparison of Simulation Runs', fontsize=16, fontweight='bold')
        plt.tight_layout()
        return fig
    
    def export_comparison_data(self, filepath: str, format: str = 'json'):
        """Export comparison data to file."""
        try:
            if format.lower() == 'json':
                with open(filepath, 'w') as f:
                    json.dump(self.comparison_data, f, indent=2, default=str)
            elif format.lower() == 'csv':
                # Convert to CSV format
                csv_data = {}
                for run_name, data in self.comparison_data.items():
                    csv_data[run_name] = data['summary']
                
                df = pd.DataFrame.from_dict(csv_data, orient='index')
                df.to_csv(filepath)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Comparison data exported to {filepath}")
            return True
            
        except Exception as e:
            print(f"Failed to export comparison data: {e}")
            return False
class ExportManager:
    """Manager for exporting visualizations and data."""
    
    def __init__(self):
        self.export_history = []
    
    def save_visualization(self, fig: plt.Figure, filename: str, 
                          dpi: int = 300, format: str = 'png',
                          metadata: Optional[Dict[str, Any]] = None) -> str:
        """Save visualization with metadata."""
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        
        # Generate filename if not provided
        if not filename:
            filename = f"visualization_{timestamp}.{format}"
        
        # Ensure output directory exists
        output_dir = "exports"
        os.makedirs(output_dir, exist_ok=True)
        
        filepath = os.path.join(output_dir, filename)
        
        try:
            # Save figure
            fig.savefig(filepath, dpi=dpi, format=format, 
                       bbox_inches='tight', metadata=metadata)
            
            # Record export
            export_info = {
                'filename': filename,
                'filepath': filepath,
                'timestamp': timestamp,
                'format': format,
                'dpi': dpi,
                'metadata': metadata or {}
            }
            
            self.export_history.append(export_info)
            
            print(f"Visualization saved to {filepath}")
            return filepath
            
        except Exception as e:
            print(f"Failed to save visualization: {e}")
            return ""
    
    def create_visualization_package(self, visualizations: List[Tuple[str, plt.Figure]],
                                    package_name: str = "visualization_package",
                                    metadata: Optional[Dict[str, Any]] = None) -> str:
        """Create a package with multiple visualizations."""
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        
        package_dir = os.path.join("exports", f"{package_name}_{timestamp}")
        os.makedirs(package_dir, exist_ok=True)
        
        # Save individual visualizations
        saved_files = []
        for filename, fig in visualizations:
            filepath = self.save_visualization(fig, filename, metadata=metadata)
            if filepath:
                saved_files.append(os.path.basename(filepath))
        
        # Create manifest file
        manifest = {
            'package_name': package_name,
            'timestamp': timestamp,
            'files': saved_files,
            'metadata': metadata or {},
            'description': 'Package of NLM simulation visualizations'
        }
        
        manifest_path = os.path.join(package_dir, 'manifest.json')
        with open(manifest_path, 'w') as f:
            json.dump(manifest, f, indent=2, default=str)
        
        print(f"Visualization package created: {package_dir}")
        return package_dir
    
    def list_export_history(self) -> List[Dict[str, Any]]:
        """List export history."""
        return self.export_history.copy()
    
    def cleanup_old_exports(self, keep_last_n: int = 20):
        """Clean up old export files."""
        exports_dir = "exports"
        
        if not os.path.exists(exports_dir):
            return
        
        # Get all export files
        all_files = []
        for filename in os.listdir(exports_dir):
            filepath = os.path.join(exports_dir, filename)
            if os.path.isfile(filepath):
                stat = os.stat(filepath)
                all_files.append((stat.st_mtime, filepath, filename))
        
        # Sort by modification time (newest first)
        all_files.sort(key=lambda x: x[0], reverse=True)
        
        # Remove oldest files
        if len(all_files) > keep_last_n:
            files_to_remove = all_files[keep_last_n:]
            
            for _, filepath, filename in files_to_remove:
                try:
                    os.remove(filepath)
                    print(f"Removed old export: {filename}")
                except Exception as e:
                    print(f"Error removing export {filename}: {e}")
class VisualizationToolkit:
    """Main toolkit providing access to all NLM visualization utilities."""
    
    def __init__(self):
        self.brain_visualizer = BrainVisualizer()
        self.activity_visualizer = ActivityVisualizer()
        self.comparison_visualizer = ComparisonVisualizer()
        self.export_manager = ExportManager()
    
    def create_comprehensive_brain_visualization(self, brain: pynlm.Brain,
                                                 include_raster: bool = True,
                                                 include_heatmap: bool = True,
                                                 region_id: Optional[int] = None) -> plt.Figure:
        """Create comprehensive brain visualization with multiple views."""
        fig, axes = plt.subplots(2, 3, figsize=(20, 14))
        axes = axes.flatten()
        
        # 1. Neural network graph
        if region_id is None:
            graph_fig = self.brain_visualizer.create_neural_network_graph(brain, max_neurons=100)
        else:
            graph_fig = self.brain_visualizer.create_neural_network_graph(brain, region_id, max_neurons=100)
        
        # Place network graph in first subplot
        axes[0].imshow(plt.imread(io.BytesIO(graph_fig.canvas.tostring_rgb())))
        axes[0].axis('off')
        
        # 2. Spike raster plot (if enabled)
        if include_raster:
            raster_fig = self.brain_visualizer.create_spike_raster_plot(brain, region_id, 
                                                                      (0, 1000), 50)
            axes[1].imshow(plt.imread(io.BytesIO(raster_fig.canvas.tostring_rgb())))
            axes[1].axis('off')
        
        # 3. Weight matrix heatmap (if enabled)
        if include_heatmap:
            heatmap_fig = self.brain_visualizer.create_weight_matrix_heatmap(brain, region_id, 50)
            axes[2].imshow(plt.imread(io.BytesIO(heatmap_fig.canvas.tostring_rgb())))
            axes[2].axis('off')
        
        # 4. Create activity dashboard (simplified)
        axes[3].text(0.5, 0.5, 'Activity Dashboard\n(Dashboard visualization would be created here)',
                   horizontalalignment='center', verticalalignment='center',
                   transform=axes[3].transAxes, fontsize=14,
                   bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        
        # 5. Statistics panel
        axes[4].text(0.5, 0.5, 'Brain Statistics:\n\n'
                   f'Neurons: {brain.getTotalNeuronCount()}\n'
                   f'Regions: {brain.getRegionCount()}\n'
                   f'Synapses: {brain.getTotalSynapseCount()}\n'
                   f'Firing Rate: {brain.getAverageFiringRate():.2f} Hz\n'
                   f'E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n'
                   f'Development: {brain.getDevelopmentalStage()}',
                   horizontalalignment='center', verticalalignment='center',
                   transform=axes[4].transAxes, fontsize=12,
                   bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))
        
        # 6. Configuration info
        config_info = brain.getConfig().summary() if brain.getConfig() else 'No config'
        axes[5].text(0.5, 0.5, f'Configuration:\n\n{config_info[:200]}...',
                   horizontalalignment='center', verticalalignment='center',
                   transform=axes[5].transAxes, fontsize=10,
                   bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.8))
        
        plt.suptitle('Comprehensive NLM Brain Visualization', fontsize=16, fontweight='bold')
        plt.tight_layout()
        return fig
    
    def create_research_figure(self, brain: pynlm.Brain, agent: pynlm.AgentBrain,
                               world: pynlm.SimpleWorld, 
                               save_path: Optional[str] = None) -> plt.Figure:
        """Create a comprehensive research figure suitable for publications."""
        fig = plt.figure(figsize=(20, 12))
        
        # Create subplots
        gs = fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        # Main neural network visualization (spanning 2x2)
        ax1 = fig.add_subplot(gs[0:2, 0:2])
        # Simplified network visualization
        ax1.text(0.5, 0.5, 'Neural Network\nVisualization', 
                horizontalalignment='center', verticalalignment='center',
                transform=ax1.transAxes, fontsize=14,
                bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.8))
        
        # Activity patterns (spanning 1x2)
        ax2 = fig.add_subplot(gs[0:1, 2:4])
        ax2.text(0.5, 0.5, 'Activity Patterns\n(Time Series)', 
                horizontalalignment='center', verticalalignment='center',
                transform=ax2.transAxes, fontsize=14,
                bbox=dict(boxstyle='round', facecolor='lightgreen', alpha=0.8))
        
        # Metrics summary (spanning 2x1)
        ax3 = fig.add_subplot(gs[2:3, 0:2])
        metrics_text = f"NLM Performance Metrics:\n\n"
        metrics_text += f"Neurons: {brain.getTotalNeuronCount():,}\n"
        metrics_text += f"Firing Rate: {brain.getAverageFiringRate():.2f} Hz\n"
        metrics_text += f"E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n"
        metrics_text += f"Total Spikes: {brain.getTotalSpikeCount():,}\n"
        metrics_text += f"Development: {brain.getDevelopmentalStage()}\n"
        metrics_text += f"Neuromodulation: {agent.getNeuromodulationLevel():.3f}\n"
        metrics_text += f"Curiosity: {agent.getCuriosityLevel():.3f}"
        
        ax3.text(0.1, 0.5, metrics_text, fontsize=12,
                verticalalignment='center',
                bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.8))
        
        # Configuration (spanning 1x1)
        ax4 = fig.add_subplot(gs[2:3, 3:4])
        config_info = brain.getConfig().summary() if brain.getConfig() else 'No config'
        ax4.text(0.5, 0.5, f'Configuration\nParameters:\n\n{config_info[:150]}...',
                horizontalalignment='center', verticalalignment='center',
                transform=ax4.transAxes, fontsize=10,
                bbox=dict(boxstyle='round', facecolor='lightcoral', alpha=0.8))
        
        plt.suptitle('NLM Research Visualization - Publication Quality Figure',
                    fontsize=18, fontweight='bold')
        
        # Add timestamp
        timestamp = datetime.datetime.now().strftime('%Y-%m-%d %H:%M')
        fig.text(0.98, 0.01, f'Generated: {timestamp}', fontsize=9, 
                ha='right', style='italic')
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight',
                       facecolor='white', edgecolor='none')
            print(f"Research figure saved to {save_path}")
        
        return fig
    
    def export_research_package(self, brain: pynlm.Brain, agent: pynlm.AgentBrain,
                               world: pynlm.SimpleWorld, package_name: str) -> str:
        """Export a complete research package with multiple visualizations."""
        timestamp = datetime.datetime.now().strftime("%Y%m%d_%H%M%S")
        package_dir = os.path.join("research_packages", f"{package_name}_{timestamp}")
        os.makedirs(package_dir, exist_ok=True)
        
        # Create research figure
        research_fig = self.create_research_figure(brain, agent, world, 
                                                  save_path=os.path.join(package_dir, 
                                                                        f"research_figure_{timestamp}.png"))
        
        # Create brain visualization
        brain_fig = self.create_comprehensive_brain_visualization(
            brain, include_raster=True, include_heatmap=True
        )
        brain_fig.savefig(os.path.join(package_dir, f"brain_visualization_{timestamp}.png"),
                         dpi=300, bbox_inches='tight')
        
        # Create metadata
        metadata = {
            'package_name': package_name,
            'timestamp': timestamp,
            'brain_info': {
                'neuron_count': brain.getTotalNeuronCount(),
                'region_count': brain.getRegionCount(),
                'synapse_count': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'developmental_stage': str(brain.getDevelopmentalStage())
            },
            'agent_info': {
                'neuromodulation_level': agent.getNeuromodulationLevel(),
                'curiosity_level': agent.getCuriosityLevel(),
                'developmental_stage': str(agent.getDevelopmentalStage())
            },
            'world_info': {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'simulation_time': world.getSimulationTime()
            }
        }
        
        # Save metadata
        with open(os.path.join(package_dir, 'metadata.json'), 'w') as f:
            json.dump(metadata, f, indent=2, default=str)
        
        # Create README
        readme_content = f"""
NLM Research Package
===================

Package Name: {package_name}
Generated: {timestamp}

DESCRIPTION
-----------
This package contains visualizations and data from an NLM (Neural Learning Machine) simulation.

VISUALIZATIONS
--------------
- research_figure.png: Main research figure suitable for publications
- brain_visualization.png: Comprehensive brain network visualization

DATA
----
- metadata.json: Configuration and simulation parameters

INTERPRETATION
--------------
This simulation demonstrates the NLM's ability to learn and adapt through neural dynamics,
neuromodulation, and developmental processes.

For more information about NLM, visit: https://github.com/nlm-project/nlm
"""
        
        with open(os.path.join(package_dir, 'README.md'), 'w') as f:
            f.write(readme_content)
        
        print(f"Research package created: {package_dir}")
        return package_dir
# Global visualization toolkit instance
_viz_toolkit = VisualizationToolkit()

# Convenience functions
def create_brain_visualization(brain: pynlm.Brain, 
                               include_raster: bool = True,
                               include_heatmap: bool = True) -> plt.Figure:
    """Create comprehensive brain visualization."""
    return _viz_toolkit.create_comprehensive_brain_visualization(brain, include_raster, include_heatmap)

def create_research_figure(brain: pynlm.Brain, agent: pynlm.AgentBrain,
                           world: pynlm.SimpleWorld) -> plt.Figure:
    """Create publication-quality research figure."""
    return _viz_toolkit.create_research_figure(brain, agent, world)

def export_research_package(brain: pynlm.Brain, agent: pynlm.AgentBrain,
                           world: pynlm.SimpleWorld, package_name: str) -> str:
    """Export complete research package."""
    return _viz_toolkit.export_research_package(brain, agent, world, package_name)

# Module information
__version__ = "2.0.0"
__author__ = "NLM Visualization Team"
__description__ = "Advanced visualization toolkit for NLM simulations"

# Export public API
__all__ = [
    'BrainVisualizer',
    'ActivityVisualizer',
    'ComparisonVisualizer',
    'ExportManager',
    'VisualizationToolkit',
    'create_brain_visualization',
    'create_research_figure',
    'export_research_package'
]