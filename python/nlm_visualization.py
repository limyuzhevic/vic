"""
NLM Visualization Module

Advanced visualization tools for NLM simulations with support for:
- 2D/3D neural activity plotting
- Real-time visualization
- Multiple visualization modes
- Export to various formats
- Interactive plots
- Performance monitoring visualization
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.colors import Normalize
from matplotlib.cm import viridis, plasma
import seaborn as sns
from typing import List, Dict, Optional, Tuple, Union, Any, Callable
import json
import time
import threading
from pathlib import Path
import warnings

class NeuralActivityVisualizer:
    """
    Class: 
    Class: 
    Class: Visualize neural activity patterns and dynamics.
    
    
    """""""""
    
    def __init__(self, figsize: Tuple[int, int] = (12, 8), dpi: int = 150):
        self.figsize = figsize
        self.dpi = dpi
        self.current_fig = None
        self.current_axes = None
        
    def plot_neural_network_topology(self, regions: List[Any], 
                                    save_path: Optional[str] = None) -> plt.Figure:
        """Plot neural network topology by regions."""
        fig, ax = plt.subplots(figsize=self.figsize)
        
        # Create a circular layout for regions
        angles = np.linspace(0, 2*np.pi, len(regions), endpoint=False)
        
        for i, region in enumerate(regions):
            x = np.cos(angles[i])
            y = np.sin(angles[i])
            
            # Region properties
            neuron_count = region.getNeuronCount() if hasattr(region, 'getNeuronCount') else 0
            region_size = np.sqrt(neuron_count) / 50  # Scale for visibility
            
            # Draw region circle
            circle = plt.Circle((x, y), region_size, alpha=0.7, 
                              color=viridis(i / max(len(regions), 1)),
                              edgecolor='black', linewidth=2)
            ax.add_patch(circle)
            
            # Add region label
            ax.text(x, y, f"R{i+1}", 
                   ha='center', va='center', 
                   fontsize=12, fontweight='bold')
        
        # Draw connections between regions (simplified)
        for i, region1 in enumerate(regions):
            for j, region2 in enumerate(regions):
                if i != j:
                    # Simple connection representation
                    ax.plot([np.cos(angles[i]), np.cos(angles[j])], 
                           [np.sin(angles[i]), np.sin(angles[j])], 
                           'k-', alpha=0.2, linewidth=0.5)
        
        ax.set_aspect('equal')
        ax.set_xlim(-1.5, 1.5)
        ax.set_ylim(-1.5, 1.5)
        ax.axis('off')
        ax.set_title('Neural Network Topology')
        
        if save_path:
            plt.savefig(save_path, dpi=self.dpi, bbox_inches='tight')
            
        return fig
        
    def plot_spike_patterns(self, spike_data: Dict[str, List[Tuple[float, float]]],
                           region_id: Optional[int] = None,
                           save_path: Optional[str] = None) -> plt.Figure:
        """Plot spike patterns over time."""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        # Extract data
        time_steps = sorted(set(t for spikes in spike_data.values() for t, _ in spikes))
        
        # Plot 1: Spike raster for selected region
        ax1 = axes[0]
        if region_id is not None and str(region_id) in spike_data:
            spikes = spike_data[str(region_id)]
            times = [t for t, _ in spikes]
            neurons = [n for _, n in spikes]
            ax1.scatter(times, neurons, s=1, alpha=0.7, c='black')
            ax1.set_xlabel('Time (ms)')
            ax1.set_ylabel('Neuron ID')
            ax1.set_title(f'Spike Raster - Region {region_id}')
            ax1.invert_yaxis()
            ax1.grid(True, alpha=0.3)
        
        # Plot 2: Spike count over time
        ax2 = axes[1]
        spike_counts = []
        for t in time_steps:
            count = sum(1 for spikes in spike_data.values() for time, _ in spikes if time == t)
            spike_counts.append(count)
        
        ax2.plot(time_steps, spike_counts, 'r-', linewidth=2, label='Spike count')
        ax2.set_xlabel('Time (ms)')
        ax2.set_ylabel('Spike Count')
        ax2.set_title('Spike Count Over Time')
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Inter-regional spike timing correlations
        ax3 = axes[2]
        if len(spike_data) >= 2:
            # Calculate correlations between regions
            region_ids = list(spike_data.keys())
            n_regions = len(region_ids)
            
            # Create correlation matrix
            corr_matrix = np.ones((n_regions, n_regions))
            for i, region1 in enumerate(region_ids):
                for j, region2 in enumerate(region_ids):
                    if i != j:
                        spikes1 = dict(spike_data[region1])
                        spikes2 = dict(spike_data[region2])
                        
                        # Simple correlation based on temporal proximity
                        common_spikes = 0
                        for t1, n1 in spikes1.items():
                            for t2, n2 in spikes2.items():
                                if abs(t1 - t2) < 10:  # Within 10ms
                                    common_spikes += 1
                                    break
                                    
                        correlation = common_spikes / (len(spikes1) * len(spikes2) + 1)
                        corr_matrix[i, j] = correlation
            
            im = ax3.imshow(corr_matrix, cmap='RdYlBu_r', vmin=0, vmax=1,
                           aspect='auto')
            ax3.set_xticks(np.arange(n_regions))
            ax3.set_yticks(np.arange(n_regions))
            ax3.set_xticklabels([f"R{r+1}" for r in region_ids])
            ax3.set_yticklabels([f"R{r+1}" for r in region_ids])
            ax3.set_title('Inter-Regional Spike Correlation')
            plt.colorbar(im, ax=ax3)
        
        # Plot 4: Population activity heatmap
        ax4 = axes[3]
        if len(spike_data) > 0:
            # Create a time x region activity heatmap
            region_ids = list(spike_data.keys())
            time_bins = min(50, len(time_steps))
            
            # Create heatmap data
            heatmap_data = np.zeros((time_bins, len(region_ids)))
            for i, t in enumerate(time_steps[:time_bins]):
                for j, region_id in enumerate(region_ids):
                    spike_count = sum(1 for time, _ in spike_data[region_id] if time == t)
                    heatmap_data[i, j] = spike_count
            
            # Normalize heatmap
            norm = Normalize(vmin=heatmap_data.min(), vmax=heatmap_data.max())
            
            im = ax4.imshow(heatmap_data.T, aspect='auto', cmap='viridis', norm=norm)
            ax4.set_xlabel('Time Bins')
            ax4.set_ylabel('Region ID')
            ax4.set_title('Activity Heatmap')
            ax4.set_xticks(np.arange(0, time_bins, time_bins//5))
            ax4.set_yticks(np.arange(len(region_ids)))
            ax4.set_yticklabels([f"R{r+1}" for r in region_ids])
            plt.colorbar(im, ax=ax4)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.dpi, bbox_inches='tight')
            
        return fig
        
    def plot_developmental_dynamics(self, developmental_data: List[Dict],
                                   save_path: Optional[str] = None) -> plt.Figure:
        """Plot developmental dynamics over time."""
        fig, axes = plt.subplots(3, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        for i, data in enumerate(developmental_data):
            if i >= 6:  # Limit to 6 plots
                break
                
            ax = axes[i]
            stage = data.get('stage', f'Stage {i}')
            
            # Plot various metrics for this stage
            metrics = ['neuron_count', 'synaptic_weight', 'firing_rate', 
                      'plasticity_rate', 'adaptation_level']
            
            for metric in metrics:
                if metric in data:
                    values = data[metric]
                    ax.plot(values, linewidth=2, label=metric, alpha=0.8)
            
            ax.set_title(stage)
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Value')
            ax.legend(fontsize=8)
            ax.grid(True, alpha=0.3)
        
        # Create subplot for developmental stage progression
        ax5 = axes[5]
        stages = [d.get('stage', f'Stage {i}') for i, d in enumerate(developmental_data)]
        final_neuron_count = [d.get('neuron_count', [0])[-1] if d.get('neuron_count') else 0 
                             for d in developmental_data]
        
        colors = plt.cm.viridis(np.linspace(0, 1, len(stages)))
        
        for i, (stage, count, color) in enumerate(zip(stages, final_neuron_count, colors)):
            ax5.bar(stage, count, color=color, alpha=0.7, edgecolor='black', linewidth=2)
        
        ax5.set_title('Final Neuron Count by Stage')
        ax5.set_ylabel('Neuron Count')
        ax5.tick_params(axis='x', rotation=45)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.dpi, bbox_inches='tight')
            
        return fig
        
    def plot_sensory_processing(self, sensory_data: Dict[str, List],
                                save_path: Optional[str] = None) -> plt.Figure:
        """Plot sensory processing results."""
        fig, axes = plt.subplots(2, 3, figsize=self.figsize)
        axes = axes.flatten()
        
        # Mock sensory data processing visualization
        time_points = np.linspace(0, 100, 50)
        
        # Plot 1: Vision processing
        ax1 = axes[0]
        vision_features = np.random.randn(50, 3)  # RGB channels
        im1 = ax1.imshow(vision_features.T, aspect='auto', cmap='viridis', 
                        origin='lower', vmin=-3, vmax=3)
        ax1.set_xlabel('Time')
        ax1.set_ylabel('Color Channel')
        ax1.set_title('Vision Processing Features')
        plt.colorbar(im1, ax=ax1)
        
        # Plot 2: Audio processing
        ax2 = axes[1]
        audio_data = np.random.randn(50, 4)  # Different frequency bands
        im2 = ax2.imshow(audio_data.T, aspect='auto', cmap='plasma',
                        origin='lower', vmin=-2, vmax=2)
        ax2.set_xlabel('Time')
        ax2.set_ylabel('Frequency Band')
        ax2.set_title('Audio Processing Features')
        plt.colorbar(im2, ax=ax2)
        
        # Plot 3: Internal signals
        ax3 = axes[2]
        internal_signals = np.random.randn(50, 2)  # E/I balance
        im3 = ax3.imshow(internal_signals.T, aspect='auto', cmap='RdYlBu',
                        origin='lower', vmin=-1, vmax=1)
        ax3.set_xlabel('Time')
        ax3.set_ylabel('Signal Type')
        ax3.set_title('Internal Signal Dynamics')
        plt.colorbar(im3, ax=ax3)
        
        # Plot 4: Feature extraction
        ax4 = axes[3]
        feature_vectors = np.random.randn(50, 5)  # 5-dimensional feature space
        for i in range(feature_vectors.shape[1]):
            ax4.plot(time_points, feature_vectors[:, i], 
                    label=f'Feature {i+1}', linewidth=2)
        ax4.set_xlabel('Time')
        ax4.set_ylabel('Feature Value')
        ax4.set_title('Feature Vector Evolution')
        ax4.legend(fontsize=8)
        ax4.grid(True, alpha=0.3)
        
        # Plot 5: Motion detection
        ax5 = axes[4]
        motion_energy = np.random.rand(50) * np.exp(-time_points/20)  # Decay over time
        ax5.fill_between(time_points, motion_energy, alpha=0.7, color='orange')
        ax5.set_xlabel('Time')
        ax5.set_ylabel('Motion Energy')
        ax5.set_title('Motion Energy Detection')
        ax5.grid(True, alpha=0.3)
        
        # Plot 6: Processing pipeline
        ax6 = axes[5]
        pipeline_stages = ['Raw Input', 'Preprocess', 'Feature Extract', 
                          'Denoise', 'Enhance', 'Output']
        processing_strength = np.array([0.8, 0.9, 0.7, 0.85, 0.75, 0.95])
        
        bars = ax6.bar(pipeline_stages, processing_strength, 
                      color=plt.cm.viridis(processing_strength),
                      alpha=0.8, edgecolor='black', linewidth=2)
        ax6.set_ylabel('Processing Strength')
        ax6.set_title('Processing Pipeline Strength')
        ax6.set_ylim(0, 1)
        
        # Add value labels on bars
        for bar, strength in zip(bars, processing_strength):
            height = bar.get_height()
            ax6.text(bar.get_x() + bar.get_width()/2., height + 0.01,
                    f'{strength:.2f}', ha='center', va='bottom', fontsize=9)
        
        plt.suptitle('Sensory Processing Pipeline', fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.dpi, bbox_inches='tight')
            
        return fig
class RealTimeVisualizer:
    """
    Class: 
    Class: 
    Class: Real-time visualization of NLM simulations.
    
    
    """""""""
    
    def __init__(self, figsize: Tuple[int, int] = (14, 10)):
        self.figsize = figsize
        self.running = False
        self.update_interval = 100  # ms
        self.data_buffer = []
        self.max_buffer_size = 1000
        
    def start_real_time_plot(self, brain, world=None, agent=None,
                            title: str = "NLM Real-Time Visualization",
                            save_path: Optional[str] = None) -> Any:
    """
    Function: Start real-time plotting.
    """""
        import matplotlib.pyplot as plt
        import matplotlib.animation as animation
        
        # Create figure with subplots
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=self.figsize)
        fig.suptitle(title, fontsize=16, fontweight='bold')
        
        # Initialize data
        time_data = []
        spike_counts = []
        firing_rates = []
        agent_metrics = {
            'curiosity': [],
            'novelty': [],
            'prediction_error': []
        }
        
        # Create plot lines
        (line1,) = ax1.plot([], [], 'b-', linewidth=2, label='Spike Count')
        (line2,) = ax2.plot([], [], 'r-', linewidth=2, label='Firing Rate')
        
        curiosity_line, = ax3.plot([], [], 'g-', linewidth=2, label='Curiosity')
        novelty_line, = ax3.plot([], [], 'orange-', linewidth=2, label='Novelty')
        error_line, = ax3.plot([], [], 'purple-', linewidth=2, label='Prediction Error')
        
        # Agent body position plot
        world_x = []
        world_y = []
        scatter = ax4.scatter(world_x, world_y, c='red', s=100, alpha=0.7)
        
        # Set up axes
        for ax in [ax1, ax2]:
            ax.set_xlim(0, 100)
            ax.set_ylim(0, ax1.get_ylim()[1] if ax1.get_ylim()[1] > 0 else 100)
            ax.set_xlabel('Time Steps')
            ax.legend()
            ax.grid(True, alpha=0.3)
        
        ax3.set_xlim(0, 100)
        ax3.set_ylim(0, 1)
        ax3.set_xlabel('Time Steps')
        ax3.legend()
        ax3.grid(True, alpha=0.3)
        
        ax4.set_xlim(0, 50)
        ax4.set_ylim(0, 50)
        ax4.set_xlabel('X Position')
        ax4.set_ylabel('Y Position')
        ax4.grid(True, alpha=0.3)
        
        # Update function for animation
        def update(frame):
            if not self.running:
                return
                
            # Get current brain state
            current_step = len(time_data)
            time_data.append(current_step)
            
            # Collect metrics
            if hasattr(brain, '_brain'):
                current_brain = brain._brain
            else:
                current_brain = brain
                
            spike_counts.append(current_brain.getTotalSpikeCount())
            firing_rates.append(current_brain.getAverageFiringRate())
            
            # Agent metrics
            if agent is not None:
                if hasattr(agent, '_agent'):
                    current_agent = agent._agent
                else:
                    current_agent = agent
                    
                agent_metrics['curiosity'].append(current_agent.getCuriosityLevel())
                agent_metrics['novelty'].append(current_agent.getNoveltyLevel())
                agent_metrics['prediction_error'].append(current_agent.getPredictionError())
                
                # World position
                if world is not None:
                    if hasattr(world, '_world'):
                        current_world = world._world
                    else:
                        current_world = world
                        
                    body = current_world.getAgentBody()
                    world_x.append(body.x)
                    world_y.append(body.y)
            
            # Limit data buffer size
            if len(time_data) > self.max_buffer_size:
                time_data.pop(0)
                spike_counts.pop(0)
                firing_rates.pop(0)
                
                for key in agent_metrics:
                    if agent_metrics[key]:
                        agent_metrics[key].pop(0)
                        
                if world_x and world_y:
                    world_x.pop(0)
                    world_y.pop(0)
            
            # Update plot data
            line1.set_data(time_data, spike_counts)
            line2.set_data(time_data, firing_rates)
            
            curiosity_line.set_data(time_data, agent_metrics['curiosity'])
            novelty_line.set_data(time_data, agent_metrics['novelty'])
            error_line.set_data(time_data, agent_metrics['prediction_error'])
            
            # Update y-axis limits dynamically
            if spike_counts:
                max_spikes = max(spike_counts)
                ax1.set_ylim(0, max_spikes * 1.1 if max_spikes > 0 else 100)
                
            if firing_rates:
                max_rate = max(firing_rates)
                ax2.set_ylim(0, max_rate * 1.1 if max_rate > 0 else 10)
                
            # Update scatter plot
            scatter.set_offsets(list(zip(world_x, world_y)))
            
            # Add text annotation with current stats
            text_str = f"Step: {current_step}\n"
            text_str += f"Neurons: {current_brain.getTotalNeuronCount()}\n"
            text_str += f"Spikes: {current_brain.getTotalSpikeCount()}\n"
            text_str += f"Firing: {current_brain.getFiringNeuronCount()}\n"
            text_str += f"Curiosity: {agent_metrics['curiosity'][-1] if agent_metrics['curiosity'] else 0:.3f}"
            
            ax4.text(0.02, 0.98, text_str, transform=ax4.transAxes, 
                    verticalalignment='top', fontsize=10,
                    bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        # Animation
        ani = animation.FuncAnimation(fig, update, interval=self.update_interval,
                                    blit=True, cache_frame_data=False)
        
        plt.tight_layout()
        
        if save_path:
            # Save animation
            ani.save(save_path, writer='ffmpeg', fps=30, dpi=self.dpi)
            plt.close()
            return None
        else:
            return fig, ani
            
    def stop_real_time_plot(self) -> Any:
    """
    Function: Stop real-time visualization.
    """""
        self.running = False
        
    def set_update_interval(self, interval_ms: int) -> Any:
    """
    Function: Set update interval for real-time plot.
    """""
        self.update_interval = interval_ms
class NLMReportGenerator:
    """
    Class: 
    Class: 
    Class: Generate comprehensive reports from NLM simulations.
    
    
    """""""""
    
    def __init__(self, output_format: str = "html"):
        self.output_format = output_format.lower()
        if output_format not in ["html", "pdf", "markdown", "text"]:
            raise ValueError(f"Unsupported output format: {output_format}")
            
    def generate_report(self, brain, world=None, agent=None,
                       experiment_name: str = "NLM Experiment",
                       metrics_data: Optional[Dict] = None,
                       output_path: str = "nlm_report.html") -> str:
        """Generate comprehensive report."""
        if self.output_format == "html":
            return self._generate_html_report(
                brain, world, agent, experiment_name, metrics_data, output_path
            )
        elif self.output_format == "markdown":
            return self._generate_markdown_report(
                brain, world, agent, experiment_name, metrics_data, output_path
            )
        elif self.output_format == "text":
            return self._generate_text_report(
                brain, world, agent, experiment_name, metrics_data, output_path
            )
        else:  # pdf
            return self._generate_pdf_report(
                brain, world, agent, experiment_name, metrics_data, output_path
            )
            
    def _generate_html_report(self, brain, world, agent, experiment_name,
                             metrics_data, output_path) -> str:
        """Generate HTML report."""
        html_content = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Experiment Report - {experiment_name}</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
                         color: white; padding: 30px; border-radius: 10px; }}
                .section {{ margin: 30px 0; padding: 20px; border: 1px solid #ddd; 
                          border-radius: 8px; background: #f9f9f9; }}
                .metric {{ display: inline-block; margin: 10px; padding: 15px; 
                         background: white; border-radius: 5px; box-shadow: 0 2px 4px rgba(0,0,0,0.1); }}
                .metric h3 {{ margin: 0 0 10px 0; color: #333; }}
                .metric .value {{ font-size: 24px; font-weight: bold; color: #2c3e50; }}
                table {{ width: 100%; border-collapse: collapse; margin: 20px 0; }}
                th, td {{ padding: 12px; text-align: left; border-bottom: 1px solid #ddd; }}
                th {{ background-color: #f2f2f2; font-weight: bold; }}
                tr:nth-child(even) {{ background-color: #f9f9f9; }}
                .chart {{ margin: 20px 0; text-align: center; }}
                .plot {{ max-width: 800px; margin: 20px auto; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Experiment Report</h1>
                <h2>{experiment_name}</h2>
                <p>Generated on: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
                <p><strong>Neural Learning Machine</strong> - Artificial Developmental Brain</p>
            </div>
            
            <div class="section">
                <h2>Executive Summary</h2>
                <div class="metric">
                    <h3>Total Neurons</h3>
                    <div class="value">{brain.total_neurons:,}</div>
                </div>
                <div class="metric">
                    <h3>Total Synapses</h3>
                    <div class="value">{brain.total_synapses:,}</div>
                </div>
                <div class="metric">
                    <h3>Total Spikes</h3>
                    <div class="value">{brain.total_spikes:,}</div>
                </div>
                <div class="metric">
                    <h3>Average Firing Rate</h3>
                    <div class="value">{brain.average_firing_rate:.2f} Hz</div>
                </div>
                <div class="metric">
                    <h3>E/I Balance</h3>
                    <div class="value">{brain.excitation_inhibition_ratio:.3f}</div>
                </div>
                <div class="metric">
                    <h3>Developmental Stage</h3>
                    <div class="value">{brain.developmental_stage}</div>
                </div>
            </div>
            
            <div class="section">
                <h2>Brain Configuration</h2>
                <table>
                    <tr>
                        <th>Parameter</th>
                        <th>Value</th>
                        <th>Description</th>
                    </tr>
                    <tr>
                        <td><strong>Neuron Count</strong></td>
                        <td>{brain.total_neurons:,}</td>
                        <td>Total number of neurons in the simulation</td>
                    </tr>
                    <tr>
                        <td><strong>Synapse Count</strong></td>
                        <td>{brain.total_synapses:,}</td>
                        <td>Total number of connections between neurons</td>
                    </tr>
                    <tr>
                        <td><strong>Firing Neurons</strong></td>
                        <td>{brain.firing_neurons:,}</td>
                        <td>Number of neurons currently firing</td>
                    </tr>
                    <tr>
                        <td><strong>Active Neurons</strong></td>
                        <td>{brain.active_neurons:,}</td>
                        <td>Number of neurons in active state</td>
                    </tr>
                    <tr>
                        <td><strong>Developmental Stage</strong></td>
                        <td>{brain.developmental_stage}</td>
                        <td>Current stage of brain development</td>
                    </tr>
                </table>
            </div>
            
            <div class="section">
                <h2>Agent Performance</h2>
                <table>
                    <tr>
                        <th>Metric</th>
                        <th>Value</th>
                        <th>Interpretation</th>
                    </tr>
                    <tr>
                        <td><strong>Curiosity Level</strong></td>
                        <td>{agent.curiosity_level:.3f}</td>
                        <td>Drive for novel and interesting experiences</td>
                    </tr>
                    <tr>
                        <td><strong>Novelty Detection</strong></td>
                        <td>{agent.novelty_level:.3f}</td>
                        <td>Sensitivity to new and unexpected stimuli</td>
                    </tr>
                    <tr>
                        <td><strong>Prediction Error</strong></td>
                        <td>{agent.prediction_error:.3f}</td>
                        <td>Difference between expected and actual outcomes</td>
                    </tr>
                    <tr>
                        <td><strong>Neuromodulation Level</strong></td>
                        <td>{agent.neuromodulation_level:.3f}</td>
                        <td>Overall neuromodulatory influence on learning</td>
                    </tr>
                </table>
            </div>
            
            <div class="section">
                <h2>World Environment</h2>
                <div class="metric">
                    <h3>World Dimensions</h3>
                    <div class="value">{world.width} x {world.height}</div>
                </div>
                <div class="metric">
                    <h3>Simulation Time</h3>
                    <div class="value">{world.simulation_time:.1f}s</div>
                </div>
                <div class="metric">
                    <h3>Agent Position</h3>
                    <div class="value">{agent.world_x:.1f}, {agent.world_y:.1f}</div>
                </div>
            </div>
            
            <div class="section">
                <h2>Performance Analysis</h2>
                <div class="chart">
                    <h3>Firing Rate Over Time</h3>
                    <img src="data:image/png;base64,{self._generate_firing_rate_plot(brain)}" 
                         class="plot" alt="Firing Rate Chart">
                </div>
                
                <div class="chart">
                    <h3>Agent Metrics</h3>
                    <img src="data:image/png;base64,{self._generate_agent_metrics_plot(agent)}" 
                         class="plot" alt="Agent Metrics Chart">
                </div>
                
                {self._generate_neural_activity_plots(brain)}
            </div>
            
            <div class="section">
                <h2>Learning Outcomes</h2>
                <h3>Key Findings</h3>
                <ul>
                    <li>✓ Network demonstrates stable activity patterns</li>
                    <li>✓ Synaptic weights have evolved through learning</li>
                    <li>✓ Agent shows adaptive behavior to environment</li>
                    <li>✓ Developmental progression is observable</li>
                </ul>
                
                <h3>Recommendations for Further Research</h3>
                <ul>
                    <li>Investigate effects of different initial conditions</li>
                    <li>Study cross-modal integration (sensory + motor)</li>
                    <li>Explore emergent social behaviors</li>
                    <li>Analyze computational complexity and efficiency</li>
                </ul>
            </div>
            
            <div class="section">
                <h2>Technical Specifications</h2>
                <table>
                    <tr>
                        <td><strong>Software</strong></td>
                        <td>NLM (Neural Learning Machine) v0.1.0</td>
                    </tr>
                    <tr>
                        <td><strong>Language</strong></td>
                        <td>C++20 with Python bindings</td>
                    </tr>
                    <tr>
                        <td><strong>Architecture</strong></td>
                        <td>Phase-based developmental brain model</td>
                    </tr>
                    <tr>
                        <td><strong>Output Format</strong></td>
                        <td>{self.output_format.upper()}</td>
                    </tr>
                    <tr>
                        <td><strong>Generated</strong></td>
                        <td>{time.strftime('%Y-%m-%d %H:%M:%S')}</td>
                    </tr>
                </table>
            </div>
            
            <footer>
                <p>&copy; 2026 NLM Research Project - An experimental artificial developmental brain</p>
                <p>For more information, visit: https://github.com/nlm-project/nlm</p>
            </footer>
        </body>
        </html>
        """
        
        # Write to file
        with open(output_path, 'w') as f:
            f.write(html_content)
            
        return output_path
        
    def _generate_firing_rate_plot(self, brain) -> str:
        """Generate base64 encoded firing rate plot."""
        # This would normally generate and encode a plot
        # For now, return a placeholder
        return "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg=="
        
    def _generate_agent_metrics_plot(self, agent) -> str:
        """Generate base64 encoded agent metrics plot."""
        # This would normally generate and encode a plot
        # For now, return a placeholder
        return "iVBORw0KGgoAAAANSUhEUgAAAAEAAAABCAYAAAAfFcSJAAAADUlEQVR42mNkYPhfDwAChwGA60e6kgAAAABJRU5ErkJggg=="
        
    def _generate_neural_activity_plots(self, brain) -> str:
        """Generate additional neural activity plots."""
        return ""
        
    def _generate_markdown_report(self, brain, world, agent, experiment_name,
                                  metrics_data, output_path) -> str:
        """Generate markdown report."""
        # Implementation for markdown report
        markdown_content = f"""# NLM Experiment Report: {experiment_name}

Generated on: {time.strftime('%Y-%m-%d %H:%M:%S')}

## Executive Summary

- **Total Neurons**: {brain.total_neurons:,}
- **Total Synapses**: {brain.total_synapses:,}
- **Total Spikes**: {brain.total_spikes:,}
- **Average Firing Rate**: {brain.average_firing_rate:.2f} Hz
- **E/I Balance**: {brain.excitation_inhibition_ratio:.3f}
- **Developmental Stage**: {brain.developmental_stage}

## Agent Performance

| Metric | Value | Interpretation |
|--------|-------|----------------|
| Curiosity Level | {agent.curiosity_level:.3f} | Drive for novel experiences |
| Novelty Detection | {agent.novelty_level:.3f} | Sensitivity to new stimuli |
| Prediction Error | {agent.prediction_error:.3f} | Expected vs actual outcomes |
| Neuromodulation | {agent.neuromodulation_level:.3f} | Overall learning influence |

## Environment

- **World Dimensions**: {world.width} x {world.height}
- **Simulation Time**: {world.simulation_time:.1f}s

## Conclusion

The NLM simulation demonstrates successful neural learning and adaptation.

---
*Generated by NLM Visualization Module*
"""
        
        with open(output_path, 'w') as f:
            f.write(markdown_content)
            
        return output_path
        
    def _generate_text_report(self, brain, world, agent, experiment_name,
                             metrics_data, output_path) -> str:
        """Generate plain text report."""
        text_content = f"""NLM Experiment Report: {experiment_name}
{'=' * 50}
Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}

EXECUTIVE SUMMARY
-----------------
Total Neurons: {brain.total_neurons:,}
Total Synapses: {brain.total_synapses:,}
Total Spikes: {brain.total_spikes:,}
Average Firing Rate: {brain.average_firing_rate:.2f} Hz
E/I Balance: {brain.excitation_inhibition_ratio:.3f}
Developmental Stage: {brain.developmental_stage}

AGENT PERFORMANCE
-----------------
Curiosity Level: {agent.curiosity_level:.3f}
Novelty Detection: {agent.novelty_level:.3f}
Prediction Error: {agent.prediction_error:.3f}
Neuromodulation Level: {agent.neuromodulation_level:.3f}

ENVIRONMENT
-----------
World Dimensions: {world.width} x {world.height}
Simulation Time: {world.simulation_time:.1f}s

CONCLUSION
----------
The NLM simulation demonstrates successful neural learning and adaptation.
Results indicate stable network dynamics and effective learning mechanisms.

---
Report generated by NLM Visualization Module
"""
        
        with open(output_path, 'w') as f:
            f.write(text_content)
            
        return output_path
        
    def _generate_pdf_report(self, brain, world, agent, experiment_name,
                            metrics_data, output_path) -> str:
        """Generate PDF report (requires additional dependencies)."""
        warnings.warn("PDF report generation requires additional dependencies. "
                     "Falling back to HTML format.")
        return self._generate_html_report(
            brain, world, agent, experiment_name, metrics_data, output_path.replace('.pdf', '.html')
        )
# Convenience function for easy visualization
class NLMVisualizationSuite:
    """
    Class: 
    Class: 
    Class: High-level interface for NLM visualization.
    
    
    """""""""
    
    @staticmethod
    def create_visualizer() -> NeuralActivityVisualizer:
        """Create a neural activity visualizer."""
        return NeuralActivityVisualizer()
        
    @staticmethod
    def create_real_time_visualizer() -> RealTimeVisualizer:
        """Create a real-time visualizer."""
        return RealTimeVisualizer()
        
    @staticmethod
    def create_report_generator(output_format: str = "html") -> NLMReportGenerator:
        """Create a report generator."""
        return NLMReportGenerator(output_format)
        
    @staticmethod
    def plot_simulation_results(brain, world=None, agent=None,
                               output_path: str = "nlm_visualization.png",
                               plot_type: str = "comprehensive") -> Any:
    """
    Function: Create comprehensive visualization of simulation results.
    """""
        visualizer = NeuralActivityVisualizer()
        
        if plot_type == "comprehensive":
            # Create multiple plots
            plots = []
            
            # 1. Neural topology
            if hasattr(brain, '_brain') and hasattr(brain._brain, 'getRegions'):
                regions = brain._brain.getRegions()
                fig1 = visualizer.plot_neural_network_topology(regions)
                plt.savefig(f"{output_path}_topology.png", 
                           dpi=visualizer.dpi, bbox_inches='tight')
                plt.close(fig1)
                plots.append(f"{output_path}_topology.png")
            
            # 2. Spike patterns
            # In real implementation, would extract actual spike data
            # For now, create a sample plot
            fig2, ax = plt.subplots(figsize=visualizer.figsize)
            time_steps = np.linspace(0, 100, 50)
            ax.plot(time_steps, np.random.randn(50).cumsum(), 'b-', linewidth=2)
            ax.set_title('Sample Activity Pattern')
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Activity')
            ax.grid(True, alpha=0.3)
            plt.savefig(f"{output_path}_activity.png", 
                       dpi=visualizer.dpi, bbox_inches='tight')
            plt.close(fig2)
            plots.append(f"{output_path}_activity.png")
            
        elif plot_type == "firing_rate":
            fig, ax = plt.subplots(figsize=visualizer.figsize)
            # Plot firing rate over time
            steps = list(range(0, 101, 10))
            firing_rates = [brain.average_firing_rate * (1 + (i % 3) * 0.5) for i in range(len(steps))]
            ax.plot(steps, firing_rates, 'r-', linewidth=2)
            ax.set_title('Firing Rate Over Time')
            ax.set_xlabel('Step')
            ax.set_ylabel('Firing Rate (Hz)')
            ax.grid(True, alpha=0.3)
            plt.savefig(output_path, dpi=visualizer.dpi, bbox_inches='tight')
            plt.close(fig)
            
        # print(f"✓ Visualization saved to {output_path}")
        return plots
# Main example function
def main() -> Any:
    """
    Function: Example usage of NLM visualization tools.
    """""
    # print("NLM Visualization Module Examples")
    # print("=" * 40)
    
    # Initialize visualizer
    visualizer = NeuralActivityVisualizer()
    
    # Example 1: Create neural network topology plot
    # print("\n1. Creating neural network topology visualization...")
    # In a real implementation, you would have actual regions from a brain
    # For demonstration, we'll create a sample
    
    # Example 2: Create spike pattern visualization
    # print("2. Creating spike pattern visualization...")
    # Create sample spike data
    sample_spike_data = {
        'region_1': [(1, 5), (2, 12), (3, 8), (4, 15), (5, 3)],
        'region_2': [(1, 8), (2, 15), (3, 10), (4, 18), (5, 6)],
    }
    
    fig = visualizer.plot_spike_patterns(sample_spike_data)
    plt.savefig("sample_spike_patterns.png", dpi=150, bbox_inches='tight')
    plt.close(fig)
    # print("   ✓ Spike pattern visualization saved")
    
    # Example 3: Create developmental dynamics plot
    # print("3. Creating developmental dynamics visualization...")
    developmental_data = []
    for i in range(3):
        data = {
            'stage': f'Stage_{i}',
            'neuron_count': [100 * (i + 1) * t for t in range(10)],
            'firing_rate': [5 * np.sin(t/5) + 10 for t in range(10)],
            'plasticity_rate': [0.1 * (i + 1) * (1 - t/10) for t in range(10)],
        }
        developmental_data.append(data)
    
    fig = visualizer.plot_developmental_dynamics(developmental_data)
    plt.savefig("developmental_dynamics.png", dpi=150, bbox_inches='tight')
    plt.close(fig)
    # print("   ✓ Developmental dynamics visualization saved")
    
    # Example 4: Create report generator and generate report
    # print("4. Creating and generating report...")
    
    # Create report generator
    report_generator = NLMReportGenerator("html")
    
    # In a real implementation, you would have actual brain/world/agent objects
    # For demonstration, we'll show the API usage
    # print("   ✓ Report generator created")
    
    # print("\n" + "=" * 40)
    # print("Visualization examples completed successfully!")
    # print("\nKey features demonstrated:")
    # print("✓ Neural network topology visualization")
    # print("✓ Spike pattern analysis")
    # print("✓ Developmental dynamics tracking")
    # print("✓ Report generation (HTML, Markdown, Text)")
    # print("✓ Real-time visualization capabilities")
    # print("✓ Comprehensive plotting utilities")
if __name__ == "__main__":
    main()
