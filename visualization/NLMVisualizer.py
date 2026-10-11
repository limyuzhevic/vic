#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools

This module provides visualization and debugging capabilities for NLM simulations,
including neural network visualization, activity monitoring, and debugging tools.
"""

import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
import json
from typing import Dict, List, Optional, Tuple, Any
from dataclasses import dataclass, field
from pathlib import Path
import os

@dataclass
class NeuronData:
    """Neuron visualization data."""
    id: int
    x: float
    y: float
    type: str
    radius: float
    color: str
    activity: float = 0.0
    max_activity: float = 1.0
    spike_count: int = 0
    connections: List[int] = field(default_factory=list)
    properties: Dict[str, Any] = field(default_factory=dict)

@dataclass
class SynapseData:
    """Synapse visualization data."""
    pre_neuron: int
    post_neuron: int
    weight: float
    active: bool = True
    delay: float = 0.0
    properties: Dict[str, Any] = field(default_factory=dict)

@dataclass
class RegionData:
    """Brain region visualization data."""
    id: int
    x: float
    y: float
    width: float
    height: float
    neurons: List[NeuronData] = field(default_factory=list)
    synapses: List[SynapseData] = field(default_factory=list)
    properties: Dict[str, Any] = field(default_factory=dict)

@dataclass
class SimulationFrame:
    """Single frame of simulation data for animation."""
    step: int
    timestamp: float
    neurons: List[Dict]
    regions: List[Dict]
    synapses: List[Dict]
    metrics: Dict
    metadata: Dict = field(default_factory=dict)

class NLMVisualizer:
    """Visualization tools for NLM simulations."""
    
    def __init__(self, output_dir: str = "visualizations"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.regions = []
        self.neurons = []
        self.synapses = []
        
        # Setup plotting style
        plt.style.use('seaborn-v0_8')
        self.setup_plot_style()
    
    def setup_plot_style(self):
        """Setup consistent plotting style."""
        self.figsize = (12, 8)
        self.dpi = 150
        self.neuron_colors = {
            'excitatory': '#FF6B6B',
            'inhibitory': '#4ECDC4',
            'sensory': '#45B7D1',
            'motor': '#96CEB4',
            'internal': '#FFEAA7',
            'modulatory': '#DDA0DD'
        }
        self.region_colors = {
            'cortex': '#E8F4FD',
            'hippocampus': '#FFF5F5',
            'amygdala': '#F0FFF0',
            'default': '#F9F9F9'
        }
    
    def create_brain_layout(self, config: Dict) -> List[RegionData]:
        """Create initial brain layout based on configuration."""
        regions = []
        
        # Create regions based on configuration
        region_count = config.get('region_count', 2)
        width, height = 800, 600  # Canvas size
        
        for i in range(region_count):
            # Calculate region position (simple grid layout)
            cols = int(np.sqrt(region_count))
            row = i // cols
            col = i % cols
            
            x = col * (width / cols) + (width / cols) * 0.1
            y = row * (height / (cols + 1)) + (height / (cols + 1)) * 0.1
            w = width / cols * 0.8
            h = height / (cols + 1) * 0.8
            
            region = RegionData(
                id=i,
                x=x, y=y, width=w, height=h,
                properties={
                    'neuron_type': 'mixed',
                    'connection_density': config.get('connection_probability', 0.1),
                    'plasticity_enabled': config.get('enable_plasticity', True)
                }
            )
            
            # Create neurons within region
            neuron_count = config.get('neuron_count', 1000) // region_count
            self._create_neurons_in_region(region, neuron_count, x, y, w, h)
            
            regions.append(region)
        
        self.regions = regions
        return regions
    
    def _create_neurons_in_region(self, region: RegionData, neuron_count: int, 
                                 canvas_x: float, canvas_y: float, 
                                 canvas_w: float, canvas_h: float):
        """Create neurons within a specific region."""
        neuron_types = ['excitatory', 'inhibitory', 'sensory', 'motor', 'internal']
        
        for i in range(neuron_count):
            # Random position within region
            x = canvas_x + np.random.uniform(0.1, 0.9) * canvas_w
            y = canvas_y + np.random.uniform(0.1, 0.9) * canvas_h
            
            # Determine neuron type based on region and random chance
            if region.properties.get('neuron_type') == 'sensory' or np.random.random() < 0.2:
                neuron_type = 'sensory'
            elif region.properties.get('neuron_type') == 'motor' or np.random.random() < 0.1:
                neuron_type = 'motor'
            else:
                neuron_type = np.random.choice(neuron_types)
            
            # Calculate neuron radius based on type
            radius = self._get_neuron_radius(neuron_type)
            
            neuron = NeuronData(
                id=len(self.neurons),
                x=x, y=y,
                type=neuron_type,
                radius=radius,
                color=self.neuron_colors.get(neuron_type, '#CCCCCC'),
                properties={
                    'region_id': region.id,
                    'position_in_region': (i, neuron_count),
                    'initialized': True
                }
            )
            
            self.neurons.append(neuron)
            region.neurons.append(neuron)
    
    def _get_neuron_radius(self, neuron_type: str) -> float:
        """Get appropriate radius for neuron type."""
        radii = {
            'excitatory': 3.0,
            'inhibitory': 4.0,
            'sensory': 2.5,
            'motor': 3.5,
            'internal': 2.0
        }
        return radii.get(neuron_type, 2.0)
    
    def create_connections(self, config: Dict) -> List[SynapseData]:
        """Create synaptic connections based on configuration."""
        synapses = []
        
        # Create connections based on connection probability
        connection_probability = config.get('connection_probability', 0.1)
        
        for pre_neuron in self.neurons:
            for post_neuron in self.neurons:
                if pre_neuron.id == post_neuron.id:
                    continue  # No self-connections
                
                # Check if connection should exist
                if np.random.random() < connection_probability:
                    # Calculate weight
                    weight = np.random.uniform(0.1, 1.0)
                    
                    # Adjust weight based on neuron types
                    if pre_neuron.type == 'excitatory' and post_neuron.type == 'inhibitory':
                        weight *= 0.5  # Stronger inhibition
                    elif pre_neuron.type == 'inhibitory' and post_neuron.type == 'excitatory':
                        weight *= 0.8
                    
                    synapse = SynapseData(
                        pre_neuron=pre_neuron.id,
                        post_neuron=post_neuron.id,
                        weight=weight,
                        properties={
                            'connection_type': 'static',
                            'plasticity_enabled': config.get('enable_plasticity', True)
                        }
                    )
                    
                    synapses.append(synapse)
                    pre_neuron.connections.append(post_neuron.id)
        
        self.synapses = synapses
        return synapses
    
    def create_simulation_frames(self, steps: int, brain_data: Dict) -> List[SimulationFrame]:
        """Create simulation frames for animation."""
        frames = []
        
        for step in range(steps):
            # Create frame data for this step
            frame = SimulationFrame(
                step=step,
                timestamp=step * brain_data.get('timestep', 0.001),
                neurons=[{
                    'id': n.id,
                    'x': n.x,
                    'y': n.y,
                    'type': n.type,
                    'radius': n.radius,
                    'color': n.color,
                    'activity': min(n.activity, 1.0),  # Clamp activity
                    'spike_count': n.spike_count,
                    'connections': n.connections,
                    'properties': n.properties
                } for n in self.neurons],
                regions=[{
                    'id': r.id,
                    'x': r.x, 'y': r.y,
                    'width': r.width, 'height': r.height,
                    'neurons': [n.id for n in r.neurons],
                    'synapses': [s.pre_neuron for s in r.synapses if s.pre_neuron in [n.id for n in r.neurons]],
                    'properties': r.properties
                } for r in self.regions],
                synapses=[{
                    'pre_neuron': s.pre_neuron,
                    'post_neuron': s.post_neuron,
                    'weight': s.weight,
                    'active': s.active,
                    'delay': s.delay,
                    'properties': s.properties
                } for s in self.synapses],
                metrics=self._get_simulation_metrics(step, brain_data),
                metadata={'step': step, 'total_steps': steps}
            )
            
            frames.append(frame)
        
        return frames
    
    def _get_simulation_metrics(self, step: int, brain_data: Dict) -> Dict:
        """Get simulation metrics for a specific step."""
        # Mock metrics based on step number
        return {
            'total_spikes': step * 50 + np.random.randint(0, 100),
            'firing_rate': min(20.0, step * 0.04 + np.random.uniform(0, 5)),
            'average_weight': 0.5 + np.random.uniform(-0.2, 0.2),
            'active_synapses': int(100 * (1 - step / 1000)),
            'neuron_activity': np.random.uniform(0, 1)
        }
    
    def create_static_visualizations(self, frames: List[SimulationFrame], 
                                    output_prefix: str = "nlm_viz"):
        """Create static visualizations from simulation frames."""
        print(f"Creating static visualizations from {len(frames)} frames...")
        
        # Create brain connectivity map
        self._create_connectivity_map(frames[0], f"{output_prefix}_connectivity.png")
        
        # Create activity distribution plot
        self._create_activity_distribution(frames, f"{output_prefix}_activity_distribution.png")
        
        # Create time series plots
        self._create_time_series_plots(frames, f"{output_prefix}_time_series.png")
        
        # Create region activity plots
        self._create_region_activity_plots(frames, f"{output_prefix}_region_activity.png")
        
        print("Static visualizations created successfully.")
    
    def _create_connectivity_map(self, frame: SimulationFrame, filename: str):
        """Create brain connectivity map visualization."""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=self.figsize)
        
        # Get neuron positions
        neuron_x = [n['x'] for n in frame.neurons]
        neuron_y = [n['y'] for n in frame.neurons]
        neuron_sizes = [n['radius'] * 20 for n in frame.neurons]
        neuron_colors = [n['color'] for n in frame.neurons]
        
        # Draw neurons
        ax1.scatter(neuron_x, neuron_y, s=neuron_sizes, c=neuron_colors, 
                   alpha=0.6, edgecolors='black', linewidth=0.5)
        
        # Draw synapses
        syn_x, syn_y = [], []
        syn_w, syn_c = [], []
        
        for syn in frame.synapses:
            pre_neuron = next(n for n in frame.neurons if n['id'] == syn['pre_neuron'])
            post_neuron = next(n for n in frame.neurons if n['id'] == syn['post_neuron'])
            
            syn_x.extend([pre_neuron['x'], post_neuron['x']])
            syn_y.extend([pre_neuron['y'], post_neuron['y']])
            syn_w.append(syn['weight'])
            syn_c.append('green' if syn['active'] else 'red')
        
        ax1.plot(syn_x, syn_y, syn_c, linewidth=0.5, alpha=0.3)
        
        # Draw regions
        for region in frame.regions:
            rect = plt.Rectangle((region['x'], region['y']), 
                               region['width'], region['height'],
                               linewidth=2, edgecolor='black', 
                               facecolor='none', linestyle='--', alpha=0.5)
            ax1.add_patch(rect)
        
        ax1.set_title('Brain Connectivity Map')
        ax1.set_xlabel('X Position')
        ax1.set_ylabel('Y Position')
        ax1.set_aspect('equal')
        ax1.grid(True, alpha=0.3)
        
        # Draw activity histogram
        activities = [n['activity'] for n in frame.neurons]
        ax2.hist(activities, bins=20, alpha=0.7, color='skyblue', edgecolor='black')
        ax2.set_title('Neuron Activity Distribution')
        ax2.set_xlabel('Activity Level')
        ax2.set_ylabel('Count')
        ax2.set_xlim(0, 1)
        
        plt.tight_layout()
        plt.savefig(filename, dpi=self.dpi, bbox_inches='tight')
        plt.close()
    
    def _create_activity_distribution(self, frames: List[SimulationFrame], filename: str):
        """Create activity distribution over time."""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        # Prepare data
        time_points = [frame.step for frame in frames]
        spike_counts = [frame.metrics['total_spikes'] for frame in frames]
        firing_rates = [frame.metrics['firing_rate'] for frame in frames]
        avg_weights = [frame.metrics['average_weight'] for frame in frames]
        active_synapses = [frame.metrics['active_synapses'] for frame in frames]
        
        # Spike count over time
        axes[0].plot(time_points, spike_counts, linewidth=2)
        axes[0].set_title('Total Spikes Over Time')
        axes[0].set_xlabel('Simulation Step')
        axes[0].set_ylabel('Spike Count')
        axes[0].grid(True, alpha=0.3)
        
        # Firing rate over time
        axes[1].plot(time_points, firing_rates, linewidth=2, color='orange')
        axes[1].set_title('Average Firing Rate Over Time')
        axes[1].set_xlabel('Simulation Step')
        axes[1].set_ylabel('Firing Rate (Hz)')
        axes[1].grid(True, alpha=0.3)
        
        # Average synaptic weight over time
        axes[2].plot(time_points, avg_weights, linewidth=2, color='green')
        axes[2].set_title('Average Synaptic Weight Over Time')
        axes[2].set_xlabel('Simulation Step')
        axes[2].set_ylabel('Average Weight')
        axes[2].grid(True, alpha=0.3)
        
        # Active synapses over time
        axes[3].plot(time_points, active_synapses, linewidth=2, color='red')
        axes[3].set_title('Active Synapses Over Time')
        axes[3].set_xlabel('Simulation Step')
        axes[3].set_ylabel('Active Synapse Count')
        axes[3].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(filename, dpi=self.dpi, bbox_inches='tight')
        plt.close()
    
    def _create_time_series_plots(self, frames: List[SimulationFrame], filename: str):
        """Create comprehensive time series plots."""
        fig, (ax1, ax2) = plt.subplots(2, 1, figsize=(15, 10))
        
        time_points = [frame.step for frame in frames]
        
        # Primary metrics
        spikes = [frame.metrics['total_spikes'] for frame in frames]
        firing_rates = [frame.metrics['firing_rate'] for frame in frames]
        activities = [frame.metrics['neuron_activity'] for frame in frames]
        
        # Plot on primary axis
        ax1_twin = ax1.twinx()
        
        line1 = ax1.plot(time_points, spikes, linewidth=2, label='Total Spikes', color='blue')
        line2 = ax1.plot(time_points, firing_rates, linewidth=2, label='Firing Rate', color='orange')
        line3 = ax1.plot(time_points, activities, linewidth=1, label='Avg Neuron Activity', color='green')
        
        ax1_twin.plot(time_points, activities, linewidth=1, linestyle='--', alpha=0.5, color='red')
        
        ax1.set_xlabel('Simulation Step')
        ax1.set_ylabel('Spikes / Firing Rate', color='black')
        ax1_twin.set_ylabel('Neuron Activity', color='red')
        
        ax1.set_title('Comprehensive Time Series Analysis')
        ax1.legend(loc='upper left')
        ax1.grid(True, alpha=0.3)
        
        # Secondary metrics (normalized)
        max_spikes = max(spikes) if spikes else 1
        max_firing_rate = max(firing_rates) if firing_rates else 1
        
        norm_spikes = [s / max_spikes for s in spikes]
        norm_firing_rates = [f / max_firing_rate for f in firing_rates]
        
        ax2.plot(time_points, norm_spikes, linewidth=2, alpha=0.7)
        ax2.plot(time_points, norm_firing_rates, linewidth=2, alpha=0.7)
        ax2.set_xlabel('Simulation Step')
        ax2.set_ylabel('Normalized Values')
        ax2.set_title('Normalized Activity Over Time')
        ax2.legend(['Normalized Spikes', 'Normalized Firing Rate'])
        ax2.grid(True, alpha=0.3)
        ax2.set_ylim(0, 1.1)
        
        plt.tight_layout()
        plt.savefig(filename, dpi=self.dpi, bbox_inches='tight')
        plt.close()
    
    def _create_region_activity_plots(self, frames: List[SimulationFrame], filename: str):
        """Create region-specific activity plots."""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        # Get region information
        num_regions = len(frames[0].regions) if frames else 1
        
        if num_regions <= 2:
            # Simple layout for 2 regions
            region_ids = [0, 1]
        else:
            # For more regions, sample them
            region_ids = np.linspace(0, num_regions - 1, min(4, num_regions), dtype=int)
        
        for i, region_id in enumerate(region_ids):
            if i >= len(axes):
                break
                
            # Get metrics for this region
            region_spikes = []
            region_firing = []
            time_points = []
            
            for frame in frames:
                region = next((r for r in frame.regions if r['id'] == region_id), None)
                if region:
                    # Mock metrics based on region
                    region_spikes.append(region.get('spike_count', frame.metrics['total_spikes'] * 0.5))
                    region_firing.append(frame.metrics['firing_rate'] * 0.8)
                    time_points.append(frame.step)
            
            ax = axes[i]
            ax.plot(time_points, region_spikes, linewidth=2, label='Spikes', color='blue')
            ax.plot(time_points, region_firing, linewidth=2, label='Firing Rate', color='orange')
            
            ax.set_xlabel('Simulation Step')
            ax.set_ylabel('Count / Rate')
            ax.set_title(f'Region {region_id} Activity')
            ax.legend()
            ax.grid(True, alpha=0.3)
        
        # Hide unused subplots
        for i in range(len(region_ids), len(axes)):
            axes[i].set_visible(False)
        
        plt.tight_layout()
        plt.savefig(filename, dpi=self.dpi, bbox_inches='tight')
        plt.close()
    
    def create_animation(self, frames: List[SimulationFrame], filename: str, 
                         fps: int = 30) -> bool:
        """Create animation from simulation frames."""
        try:
            print(f"Creating animation from {len(frames)} frames...")
            
            # Create figure and axis
            fig, ax = plt.subplots(figsize=(12, 8))
            
            # Initialize plot elements
            neuron_scatter = ax.scatter([], [], s=100, alpha=0.6, edgecolors='black', linewidth=0.5)
            region_patches = []
            connection_lines = []
            
            # Add regions
            for region in frames[0].regions:
                rect = plt.Rectangle((region['x'], region['y']), 
                                   region['width'], region['height'],
                                   linewidth=2, edgecolor='black', 
                                   facecolor='none', linestyle='--', alpha=0.5)
                ax.add_patch(rect)
                region_patches.append(rect)
            
            # Animation update function
            def update(frame_idx):
                frame = frames[frame_idx]
                
                # Update neuron positions and properties
                neuron_x = [n['x'] for n in frame.neurons]
                neuron_y = [n['y'] for n in frame.neurons]
                neuron_sizes = [n['radius'] * 20 for n in frame.neurons]
                neuron_colors = [n['color'] for n in frame.neurons]
                
                neuron_scatter.set_offsets(list(zip(neuron_x, neuron_y)))
                neuron_scatter.set_sizes(neuron_sizes)
                neuron_scatter.set_color(neuron_colors)
                
                # Update connections
                if connection_lines:
                    for line in connection_lines:
                        line.remove()
                    connection_lines.clear()
                
                # Draw active synapses
                for syn in frame.synapses:
                    if syn['active']:
                        pre_neuron = next(n for n in frame.neurons if n['id'] == syn['pre_neuron'])
                        post_neuron = next(n for n in frame.neurons if n['id'] == syn['post_neuron'])
                        
                        line = ax.plot([pre_neuron['x'], post_neuron['x']],
                                     [pre_neuron['y'], post_neuron['y']],
                                     linewidth=syn['weight'] * 2, alpha=0.6, color='green')[0]
                        connection_lines.append(line)
                
                # Update title
                ax.set_title(f"NLM Simulation - Step {frame.step} (Activity: {frame.metrics['neuron_activity']:.3f})")
                
                return [neuron_scatter] + region_patches + connection_lines
            
            # Create animation
            anim = animation.FuncAnimation(fig, update, frames=len(frames), 
                                         interval=1000/fps, blit=True)
            
            # Save animation
            anim.save(filename, writer='ffmpeg', fps=fps, 
                     extra_args=['-vcodec', 'libx264'], dpi=self.dpi)
            
            plt.close(fig)
            
            print(f"Animation saved: {filename}")
            return True
            
        except Exception as e:
            print(f"Failed to create animation: {e}")
            return False
    
    def create_debug_report(self, frames: List[SimulationFrame], 
                           brain_data: Dict, filename: str):
        """Create detailed debugging report."""
        print(f"Creating debug report from {len(frames)} frames...")
        
        debug_data = {
            'summary': {
                'total_frames': len(frames),
                'simulation_duration': frames[-1].timestamp - frames[0].timestamp,
                'average_firing_rate': np.mean([f.metrics['firing_rate'] for f in frames]),
                'max_activity': max([f.metrics['neuron_activity'] for f in frames]),
                'min_activity': min([f.metrics['neuron_activity'] for f in frames]),
            },
            'time_series': {
                'steps': [f.step for f in frames],
                'spikes': [f.metrics['total_spikes'] for f in frames],
                'firing_rates': [f.metrics['firing_rate'] for f in frames],
                'activities': [f.metrics['neuron_activity'] for f in frames],
                'weights': [f.metrics['average_weight'] for f in frames],
                'active_synapses': [f.metrics['active_synapses'] for f in frames]
            },
            'region_analysis': self._analyze_regions(frames),
            'neuron_statistics': self._analyze_neurons(frames),
            'synapse_statistics': self._analyze_synapses(frames),
            'brain_configuration': brain_data
        }
        
        # Save debug report
        with open(filename, 'w') as f:
            json.dump(debug_data, f, indent=2, default=str)
        
        # Create summary plots
        self._create_debug_plots(debug_data, filename.replace('.json', '_plots.png'))
        
        print(f"Debug report saved: {filename}")
    
    def _analyze_regions(self, frames: List[SimulationFrame]) -> Dict:
        """Analyze region activity over time."""
        region_analysis = {}
        
        for frame in frames:
            for region in frame.regions:
                region_id = region['id']
                if region_id not in region_analysis:
                    region_analysis[region_id] = {
                        'steps': [],
                        'activity_levels': [],
                        'spike_counts': []
                    }
                
                region_analysis[region_id]['steps'].append(frame.step)
                region_analysis[region_id]['activity_levels'].append(frame.metrics['neuron_activity'])
                region_analysis[region_id]['spike_counts'].append(frame.metrics['total_spikes'])
        
        return region_analysis
    
    def _analyze_neurons(self, frames: List[SimulationFrame]) -> Dict:
        """Analyze neuron activity statistics."""
        # Mock neuron analysis based on frame data
        return {
            'total_neurons': len(frames[0].neurons) if frames else 0,
            'activity_range': {
                'min': min([n['activity'] for f in frames for n in f.neurons]) if frames else 0,
                'max': max([n['activity'] for f in frames for n in f.neurons]) if frames else 0,
                'mean': np.mean([n['activity'] for f in frames for n in f.neurons]) if frames else 0
            }
        }
    
    def _analyze_synapses(self, frames: List[SimulationFrame]) -> Dict:
        """Analyze synapse activity statistics."""
        # Mock synapse analysis based on frame data
        return {
            'total_synapses': len(frames[0].synapses) if frames else 0,
            'active_synapse_percentage': np.mean([f.metrics['active_synapses'] for f in frames]) / (len(frames[0].synapses) if frames else 1) * 100,
            'weight_distribution': {
                'min': min([s['weight'] for f in frames for s in f.synapses]) if frames else 0,
                'max': max([s['weight'] for f in frames for s in f.synapses]) if frames else 0,
                'mean': np.mean([s['weight'] for f in frames for s in f.synapses]) if frames else 0
            }
        }
    
    def _create_debug_plots(self, debug_data: Dict, filename: str):
        """Create debugging plots."""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        axes = axes.flatten()
        
        # Time series plots
        time_series = debug_data['time_series']
        
        axes[0].plot(time_series['steps'], time_series['spikes'], linewidth=2)
        axes[0].set_title('Spike Count Over Time')
        axes[0].set_xlabel('Step')
        axes[0].set_ylabel('Count')
        axes[0].grid(True, alpha=0.3)
        
        axes[1].plot(time_series['steps'], time_series['firing_rates'], linewidth=2)
        axes[1].set_title('Firing Rate Over Time')
        axes[1].set_xlabel('Step')
        axes[1].set_ylabel('Rate (Hz)')
        axes[1].grid(True, alpha=0.3)
        
        # Activity plots
        axes[2].plot(time_series['steps'], time_series['activities'], linewidth=2)
        axes[2].set_title('Neuron Activity Over Time')
        axes[2].set_xlabel('Step')
        axes[2].set_ylabel('Activity')
        axes[2].grid(True, alpha=0.3)
        
        axes[3].plot(time_series['steps'], time_series['weights'], linewidth=2)
        axes[3].set_title('Average Synaptic Weight Over Time')
        axes[3].set_xlabel('Step')
        axes[3].set_ylabel('Weight')
        axes[3].grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(filename, dpi=self.dpi, bbox_inches='tight')
        plt.close()

# Example usage and demonstration functions
def demonstrate_visualization(config: Dict = None, steps: int = 100) -> bool:
    """
    Demonstrate NLM visualization capabilities.
    
    Args:
        config: Brain configuration
        steps: Number of simulation steps to visualize
        
    Returns:
        True if successful, False otherwise
    """
    if config is None:
        config = {
            'neuron_count': 500,
            'region_count': 3,
            'connection_probability': 0.1,
            'simulation_timestep': 0.001,
            'enable_plasticity': True
        }
    
    try:
        # Create visualizer
        visualizer = NLMVisualizer("demo_visualizations")
        
        # Create brain layout
        regions = visualizer.create_brain_layout(config)
        
        # Create connections
        synapses = visualizer.create_connections(config)
        
        # Create mock brain data
        brain_data = {
            'timestep': config['simulation_timestep'],
            'neuron_count': config['neuron_count'],
            'region_count': config['region_count']
        }
        
        # Create simulation frames
        frames = visualizer.create_simulation_frames(steps, brain_data)
        
        # Create static visualizations
        visualizer.create_static_visualizations(frames, "demo_nlm")
        
        # Create animation (if ffmpeg is available)
        animation_file = "demo_nlm_animation.mp4"
        visualizer.create_animation(frames, animation_file)
        
        # Create debug report
        debug_file = "demo_debug_report.json"
        visualizer.create_debug_report(frames, brain_data, debug_file)
        
        print("=== Visualization Demonstration Complete ===")
        print(f"Static visualizations: demo_nlm_*.png")
        print(f"Animation: {animation_file}")
        print(f"Debug report: {debug_file}")
        
        return True
        
    except Exception as e:
        print(f"Visualization demonstration failed: {e}")
        return False

def create_debug_session(brain, frames: int = 50) -> Dict:
    """
    Create a debug session with real brain data.
    
    Args:
        brain: NLM brain object
        frames: Number of frames to capture
        
    Returns:
        Dictionary with debug information
    """
    try:
        # Create visualizer
        visualizer = NLMVisualizer("debug_sessions")
        
        # Create configuration from brain
        config = {
            'neuron_count': brain.getTotalNeuronCount(),
            'region_count': brain.getRegionCount(),
            'connection_probability': 0.1,
            'simulation_timestep': 0.001,
            'enable_plasticity': True
        }
        
        # Create brain layout
        regions = visualizer.create_brain_layout(config)
        
        # Create connections (simplified)
        synapses = visualizer.create_connections(config)
        
        # Create simulation frames with real data
        brain_data = {
            'timestep': config['simulation_timestep'],
            'neuron_count': config['neuron_count'],
            'region_count': config['region_count']
        }
        
        frames_list = visualizer.create_simulation_frames(frames, brain_data)
        
        # Update with real brain data
        for i, frame in enumerate(frames_list):
            # Update with real brain metrics
            frame.metrics['total_spikes'] = brain.getTotalSpikeCount()
            frame.metrics['firing_rate'] = brain.getAverageFiringRate()
            frame.metrics['neuron_activity'] = brain.getFiringNeuronCount() / max(1, brain.getTotalNeuronCount())
        
        # Create debug report with real data
        debug_file = f"debug_session_{int(time.time())}.json"
        visualizer.create_debug_report(frames_list, brain_data, debug_file)
        
        return {
            'success': True,
            'frames_created': len(frames_list),
            'debug_file': debug_file,
            'brain_metrics': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'firing_rate': brain.getAverageFiringRate(),
                'neurons_firing': brain.getFiringNeuronCount()
            }
        }
        
    except Exception as e:
        return {
            'success': False,
            'error': str(e)
        }

if __name__ == "__main__":
    # Run demonstration
    print("=== NLM Visualization and Debugging Tools ===")
    print("Running demonstration...")
    
    success = demonstrate_visualization()
    
    if success:
        print("\n=== Demonstration completed successfully ===")
    else:
        print("\n=== Demonstration failed ===")
        exit(1)
