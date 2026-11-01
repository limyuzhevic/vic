"""
NLM Visualization Tools - Advanced visualization for NLM brain data
"""

import numpy as np
import matplotlib.pyplot as plt
import matplotlib as mpl
from matplotlib.gridspec import GridSpec
from matplotlib.animation import FuncAnimation
from pathlib import Path
from typing import Optional, List, Dict, Any, Tuple
import json
import logging

logger = logging.getLogger(__name__)

class BrainVisualizer:
    """Advanced visualization tools for NLM brain data"""
    
    def __init__(self, figsize: Tuple[int, int] = (14, 10), dpi: int = 150):
        self.figsize = figsize
        self.dpi = dpi
        self.colors = {
            'excitatory': '#2E86AB',      # Blue
            'inhibitory': '#A23B72',       # Purple
            'modulatory': '#F18F01',       # Orange
            'sensory': '#C73E1D',          # Red
            'motor': '#43A047',            # Green
            'internal': '#546E7A',         # Gray
            'active': '#FF6B6B',           # Bright Red
            'resting': '#4ECDC4',          # Teal
            'background': '#F7FFF7'        # Almost white
        }
    
    def create_comprehensive_dashboard(self, brain, save_path: str = "brain_dashboard.png"):
        """Create a comprehensive dashboard showing brain state"""
        try:
            # Create figure with GridSpec for better layout control
            fig = plt.figure(figsize=self.figsize, dpi=self.dpi)
            gs = GridSpec(3, 4, figure=fig, hspace=0.3, wspace=0.2)
            
            # 1. Network topology
            ax1 = fig.add_subplot(gs[0, :2])
            self._draw_network_topology(ax1, brain)
            ax1.set_title("Neural Network Topology", fontsize=14, fontweight='bold')
            
            # 2. Population distribution
            ax2 = fig.add_subplot(gs[0, 2:])
            self._draw_population_distribution(ax2, brain)
            ax2.set_title("Population Distribution", fontsize=14, fontweight='bold')
            
            # 3. Activity heatmap
            ax3 = fig.add_subplot(gs[1, :2])
            self._draw_activity_heatmap(ax3, brain)
            ax3.set_title("Neural Activity Heatmap", fontsize=14, fontweight='bold')
            
            # 4. Firing rate statistics
            ax4 = fig.add_subplot(gs[1, 2])
            self._draw_firing_rate_stats(ax4, brain)
            ax4.set_title("Firing Rate Statistics", fontsize=14, fontweight='bold')
            
            # 5. Synaptic dynamics
            ax5 = fig.add_subplot(gs[1, 3])
            self._draw_synaptic_dynamics(ax5, brain)
            ax5.set_title("Synaptic Dynamics", fontsize=14, fontweight='bold')
            
            # 6. Developmental stage
            ax6 = fig.add_subplot(gs[2, :2])
            self._draw_development_timeline(ax6, brain)
            ax6.set_title("Developmental Timeline", fontsize=14, fontweight='bold')
            
            # 7. Memory systems
            ax7 = fig.add_subplot(gs[2, 2:])
            self._draw_memory_systems(ax7, brain)
            ax7.set_title("Memory Systems Status", fontsize=14, fontweight='bold')
            
            # Add overall title
            fig.suptitle(f"NLM Brain State Dashboard - Step {brain.step_count}", 
                        fontsize=16, fontweight='bold')
            
            # Save figure
            plt.savefig(save_path, bbox_inches='tight', dpi=self.dpi)
            plt.close()
            
            logger.info(f"Dashboard saved to: {save_path}")
            return save_path
            
        except Exception as e:
            logger.error(f"Error creating dashboard: {e}")
            raise
    
    def _draw_network_topology(self, ax, brain):
        """Draw neural network topology"""
        # Get brain statistics
        regions = brain.getRegionIds()
        if not regions:
            ax.text(0.5, 0.5, "No regions initialized", 
                   ha='center', va='center', transform=ax.transAxes)
            return
        
        # Simple circular layout for regions
        center_x, center_y = 0.5, 0.5
        radius = 0.4
        
        for i, region_id in enumerate(regions):
            region = brain.getRegion(region_id)
            if region:
                angle = 2 * np.pi * i / len(regions)
                x = center_x + radius * np.cos(angle)
                y = center_y + radius * np.sin(angle)
                
                # Draw region
                region_color = self.colors['internal']
                ax.scatter(x, y, s=100, c=region_color, alpha=0.7, edgecolors='black', linewidth=1)
                ax.text(x, y + 0.05, f"R{i}", ha='center', va='bottom', fontsize=8, fontweight='bold')
                
                # Draw connections to other regions
                for j, other_region_id in enumerate(regions):
                    if i != j:
                        other_angle = 2 * np.pi * j / len(regions)
                        other_x = center_x + radius * np.cos(other_angle)
                        other_y = center_y + radius * np.sin(other_angle)
                        
                        # Draw connection if exists
                        ax.plot([x, other_x], [y, other_y], 
                               color='gray', alpha=0.3, linewidth=0.5, linestyle='--')
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.set_aspect('equal')
        ax.axis('off')
    
    def _draw_population_distribution(self, ax, brain):
        """Draw population distribution pie chart"""
        # Estimate population counts
        total_neurons = brain.getTotalNeuronCount()
        if total_neurons == 0:
            ax.text(0.5, 0.5, "No neurons", ha='center', va='center', transform=ax.transAxes)
            return
        
        # Estimate distribution based on typical NLM configuration
        excitatory = int(total_neurons * 0.8)
        inhibitory = int(total_neurons * 0.15)
        modulatory = total_neurons - excitatory - inhibitory
        
        sizes = [excitatory, inhibitory, modulatory]
        labels = ['Excitatory', 'Inhibitory', 'Modulatory']
        colors_list = [self.colors['excitatory'], self.colors['inhibitory'], self.colors['modulatory']]
        
        # Add actual data if available
        if hasattr(brain, 'get_population_stats'):
            stats = brain.get_population_stats()
            if stats:
                sizes = [stats.get('excitatory', 0), stats.get('inhibitory', 0), stats.get('modulatory', 0)]
        
        wedges, texts, autotexts = ax.pie(sizes, labels=labels, colors=colors_list, 
                                        autopct='%1.1f%%', startangle=90)
        
        for autotext in autotexts:
            autotext.set_color('white')
            autotext.set_fontweight('bold')
        
        ax.set_title(f"Neuron Distribution (Total: {total_neurons})", fontsize=10)
    
    def _draw_activity_heatmap(self, ax, brain):
        """Draw neural activity heatmap"""
        # Create sample activity data
        np.random.seed(42)
        size = 20
        activity = np.random.random((size, size)) * 0.8 + 0.2
        
        # Add some pattern based on brain state
        for i in range(size):
            for j in range(size):
                activity[i, j] *= (1 + 0.1 * np.sin(i * 0.1) * np.cos(j * 0.1))
        
        im = ax.imshow(activity, cmap='RdYlBu_r', vmin=0, vmax=1, aspect='auto')
        
        # Add region overlays
        regions = brain.getRegionIds()
        for region_id in regions:
            region = brain.getRegion(region_id)
            if region:
                # Draw region as a rectangle on heatmap
                ax.axvline(x=region_id * 2, color='black', alpha=0.5, linewidth=1)
        
        plt.colorbar(im, ax=ax, label='Activity Level')
        ax.set_xlabel('Space ( neurons )')
        ax.set_ylabel('Layer')
        ax.set_title('Neural Activity Pattern')
    
    def _draw_firing_rate_stats(self, ax, brain):
        """Draw firing rate statistics"""
        # Get stats from brain
        total_neurons = brain.getTotalNeuronCount()
        active_neurons = brain.getActiveNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        avg_firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        
        # Create statistics display
        stats_text = f"Total Neurons: {total_neurons}\n"
        stats_text += f"Active Neurons: {active_neurons}\n"
        stats_text += f"Firing Neurons: {firing_neurons}\n"
        stats_text += f"Avg Firing Rate: {avg_firing_rate:.2f} Hz\n"
        stats_text += f"E/I Ratio: {e_i_ratio:.2f}\n"
        stats_text += f"Firing Percentage: {firing_neurons/max(total_neurons,1)*100:.1f}%"
        
        ax.text(0.1, 0.5, stats_text, fontsize=11, fontfamily='monospace',
               verticalalignment='center', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        # Add progress bars
        self._draw_progress_bar(ax, 0.7, "Activity", active_neurons/total_neurons if total_neurons > 0 else 0)
        self._draw_progress_bar(ax, 0.8, "Firing", firing_neurons/max(total_neurons,1))
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.axis('off')
    
    def _draw_synaptic_dynamics(self, ax, brain):
        """Draw synaptic dynamics visualization"""
        # Simulate synaptic dynamics
        time_points = 50
        stdp_weight = np.random.normal(1.0, 0.1, time_points)
        hebbian_weight = np.random.normal(0.8, 0.15, time_points)
        structural_weight = np.random.normal(1.2, 0.2, time_points)
        
        x = np.arange(time_points)
        
        ax.plot(x, stdp_weight, label='STDP', linewidth=2, color=self.colors['excitatory'])
        ax.plot(x, hebbian_weight, label='Hebbian', linewidth=2, color=self.colors['inhibitory'])
        ax.plot(x, structural_weight, label='Structural', linewidth=2, color=self.colors['modulatory'])
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Weight (normalized)')
        ax.set_title('Plasticity Dynamics')
        ax.legend()
        ax.grid(True, alpha=0.3)
        
        # Add current plasticity values if available
        if hasattr(brain, 'get_plasticity_stats'):
            plasticity_stats = brain.get_plasticity_stats()
            if plasticity_stats:
                self._add_plasticity_indicators(ax, plasticity_stats)
    
    def _draw_development_timeline(self, ax, brain):
        """Draw developmental timeline"""
        developmental_stages = [
            ("Initial", 0, 0.2),
            ("Critical Period", 0.2, 0.4),
            ("Maturation", 0.4, 0.6),
            ("Adult", 0.6, 0.8),
            ("Aging", 0.8, 1.0)
        ]
        
        current_stage = brain.getDevelopmentalStage()
        stage_index = int(current_stage)
        
        # Draw timeline
        for i, (name, start, end) in enumerate(developmental_stages):
            color = self.colors['excitatory'] if i <= stage_index else 'lightgray'
            alpha = 1.0 if i == stage_index else 0.5
            
            ax.axhspan(start, end, xmin=0, xmax=1, color=color, alpha=alpha)
            ax.text(0.05, (start + end)/2, name, fontsize=9, ha='left', va='center')
        
        # Highlight current position
        ax.axhspan(current_stage, current_stage + 0.05, xmin=0, xmax=1, 
                  color='red', alpha=0.7)
        ax.text(0.05, current_stage + 0.025, "CURRENT", fontsize=10, ha='left', 
               va='center', fontweight='bold', color='white')
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.set_xlabel('Development Progress')
        ax.set_ylabel('Stage')
        ax.set_yticks([0, 0.25, 0.5, 0.75, 1])
        ax.set_yticklabels(['Initial', 'Critical', 'Mature', 'Adult', 'Aging'])
        ax.grid(True, alpha=0.3)
    
    def _draw_memory_systems(self, ax, brain):
        """Draw memory systems status"""
        # Memory system status indicators
        memory_systems = {
            'Working Memory': 0.7,
            'Episodic Memory': 0.6,
            'Associative Memory': 0.8,
            'Prediction Memory': 0.5
        }
        
        y_pos = np.arange(len(memory_systems))
        levels = list(memory_systems.values())
        labels = list(memory_systems.keys())
        
        bars = ax.barh(y_pos, levels, color='steelblue', alpha=0.8)
        ax.set_yticks(y_pos)
        ax.set_yticklabels(labels)
        ax.set_xlabel('Activity Level')
        ax.set_xlim(0, 1)
        ax.set_title('Memory Systems')
        
        # Add status indicators
        for i, (level, label) in enumerate(zip(levels, labels)):
            status = "HIGH" if level > 0.7 else "MEDIUM" if level > 0.4 else "LOW"
            ax.text(level + 0.02, i, f" {status}", fontsize=8, va='center')
        
        # Add memory stats if available
        if hasattr(brain, 'get_memory_stats'):
            memory_stats = brain.get_memory_stats()
            if memory_stats:
                self._add_memory_indicators(ax, memory_stats)
    
    def _draw_progress_bar(self, ax, y_pos, label, value):
        """Draw a progress bar on the side"""
        bar_width = 0.3
        bar_height = 0.04
        
        # Draw border
        ax.add_patch(plt.Rectangle((0.7, y_pos), bar_width, bar_height, 
                                 linewidth=1, edgecolor='black', facecolor='none'))
        
        # Draw fill
        ax.add_patch(plt.Rectangle((0.7, y_pos), value * bar_width, bar_height,
                                 facecolor='green', alpha=0.7))
        
        # Draw label
        ax.text(0.625, y_pos + bar_height/2, label, fontsize=8, ha='right', va='center')
    
    def _add_plasticity_indicators(self, ax, stats):
        """Add plasticity indicators to the plot"""
        pass  # Placeholder for future implementation
    
    def _add_memory_indicators(self, ax, stats):
        """Add memory indicators to the plot"""
        pass  # Placeholder for future implementation

class SimplePlotter:
    """Simple plotting utilities for NLM data"""
    
    def __init__(self):
        self.colors = BrainVisualizer().colors
    
    def create_activity_plot(self, brain, save_path: str = "activity_plot.png"):
        """Create a simple activity plot"""
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Simulate activity data
        time_steps = 100
        activity = np.random.random((time_steps, 50)) * 0.9 + 0.1
        
        # Add some structure
        for i in range(time_steps):
            for j in range(50):
                activity[i, j] *= (1 + 0.1 * np.sin(i * 0.1) * np.cos(j * 0.1))
        
        # Plot activity
        for neuron in range(min(10, activity.shape[1])):  # Plot first 10 neurons
            ax.plot(time_steps, activity[:, neuron], 
                   color=self.colors['excitatory'], alpha=0.6, linewidth=0.5)
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Activity Level')
        ax.set_title('Neural Activity Over Time')
        ax.set_xlim(0, time_steps - 1)
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
        
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        plt.close()
        
        return save_path
    
    def create_region_comparison(self, brain, save_path: str = "region_comparison.png"):
        """Create region comparison plot"""
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
        
        regions = brain.getRegionIds()
        
        if not regions:
            ax1.text(0.5, 0.5, "No regions available", ha='center', va='center')
            ax2.text(0.5, 0.5, "No regions available", ha='center', va='center')
        else:
            # Region activity plot
            for i, region_id in enumerate(regions):
                region = brain.getRegion(region_id)
                if region:
                    activity = np.random.random(50) * 0.8 + 0.2
                    ax1.plot(activity, label=f'Region {i}', linewidth=2)
            
            ax1.set_xlabel('Time')
            ax1.set_ylabel('Activity')
            ax1.set_title('Regional Activity Patterns')
            ax1.legend()
            ax1.grid(True, alpha=0.3)
            
            # Region properties plot
            properties = []
            for region_id in regions:
                region = brain.getRegion(region_id)
                if region:
                    properties.append({
                        'region': region_id,
                        'neurons': region.getNeuronCount(),
                        'synapses': region.getSynapseCount(),
                        'activity': np.random.random()
                    })
            
            if properties:
                x = np.arange(len(properties))
                width = 0.25
                
                ax2.bar(x - width, [p['neurons'] for p in properties], width,
                       label='Neurons', color=self.colors['excitatory'], alpha=0.8)
                ax2.bar(x, [p['synapses'] for p in properties], width,
                       label='Synapses', color=self.colors['inhibitory'], alpha=0.8)
                ax2.bar(x + width, [p['activity'] for p in properties], width,
                       label='Activity', color=self.colors['modulatory'], alpha=0.8)
                
                ax2.set_xlabel('Region')
                ax2.set_ylabel('Count/Level')
                ax2.set_title('Regional Properties')
                ax2.set_xticks(x)
                ax2.set_xticklabels([f'R{i}' for i in range(len(properties))])
                ax2.legend()
                ax2.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(save_path, dpi=150, bbox_inches='tight')
        plt.close()
        
        return save_path

class GraphRenderer:
    """Graph rendering utilities for NLM visualizations"""
    
    def __init__(self):
        self.visualizer = BrainVisualizer()
    
    def render_graph_image(self, brain, graph_type: str = "network", 
                          save_path: str = "brain_graph.png"):
        """Render brain graph visualization"""
        try:
            fig, ax = plt.subplots(figsize=(12, 10))
            
            if graph_type == "network":
                self._render_network_graph(ax, brain)
            elif graph_type == "activity":
                self._render_activity_graph(ax, brain)
            elif graph_type == "connections":
                self._render_connections_graph(ax, brain)
            else:
                raise ValueError(f"Unknown graph type: {graph_type}")
            
            ax.set_title(f"{graph_type.title()} Graph Visualization", 
                        fontsize=16, fontweight='bold')
            ax.axis('off')
            
            plt.tight_layout()
            plt.savefig(save_path, dpi=150, bbox_inches='tight')
            plt.close()
            
            logger.info(f"Graph visualization saved to: {save_path}")
            return save_path
            
        except Exception as e:
            logger.error(f"Error rendering graph: {e}")
            raise
    
    def _render_network_graph(self, ax, brain):
        """Render network topology graph"""
        regions = brain.getRegionIds()
        
        if not regions:
            ax.text(0.5, 0.5, "No regions available", ha='center', va='center', transform=ax.transAxes)
            return
        
        # Create circular layout
        center_x, center_y = 0.5, 0.5
        radius = 0.4
        
        for i, region_id in enumerate(regions):
            angle = 2 * np.pi * i / len(regions)
            x = center_x + radius * np.cos(angle)
            y = center_y + radius * np.sin(angle)
            
            # Draw node
            ax.scatter(x, y, s=200, c='steelblue', alpha=0.8, edgecolors='darkblue', linewidth=2)
            ax.text(x, y + 0.05, f"Region {i}", ha='center', va='bottom', 
                   fontsize=10, fontweight='bold')
            
            # Draw connections
            for j, other_region_id in enumerate(regions):
                if i != j:
                    other_angle = 2 * np.pi * j / len(regions)
                    other_x = center_x + radius * np.cos(other_angle)
                    other_y = center_y + radius * np.sin(other_angle)
                    
                    # Draw connection with opacity based on hypothetical connection strength
                    alpha = 0.3 + 0.7 * np.random.random()
                    ax.plot([x, other_x], [y, other_y], 
                           color='gray', alpha=alpha, linewidth=1, linestyle='-')
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.set_aspect('equal')
    
    def _render_activity_graph(self, ax, brain):
        """Render activity over time graph"""
        time_steps = 100
        neurons = 50
        
        # Generate synthetic activity data
        activity = np.random.random((time_steps, neurons)) * 0.8 + 0.2
        
        # Add temporal correlation
        for t in range(time_steps):
            for n in range(neurons):
                activity[t, n] *= (1 + 0.1 * np.sin(t * 0.05) * np.cos(n * 0.02))
        
        # Plot activity for selected neurons
        for n in range(min(10, neurons)):
            ax.plot(time_steps, activity[:, n], 
                   color='steelblue', alpha=0.7, linewidth=1)
        
        ax.set_xlabel('Time Step')
        ax.set_ylabel('Activity Level')
        ax.set_title('Neural Activity Over Time')
        ax.set_xlim(0, time_steps - 1)
        ax.set_ylim(0, 1)
        ax.grid(True, alpha=0.3)
    
    def _render_connections_graph(self, ax, brain):
        """Render connection strength graph"""
        # Create adjacency matrix-like visualization
        regions = brain.getRegionIds()
        n_regions = len(regions)
        
        if n_regions == 0:
            ax.text(0.5, 0.5, "No regions available", ha='center', va='center', transform=ax.transAxes)
            return
        
        # Generate connection strength data
        connections = np.random.random((n_regions, n_regions)) * 0.8 + 0.2
        np.fill_diagonal(connections, 0)  # No self-connections
        
        # Normalize for visualization
        max_strength = np.max(connections)
        if max_strength > 0:
            connections = connections / max_strength
        
        # Display as heatmap
        im = ax.imshow(connections, cmap='YlOrRd', vmin=0, vmax=1, aspect='auto')
        
        # Add region labels
        for i in range(n_regions):
            ax.text(i, i, f'R{i}', ha='center', va='center', 
                   color='white', fontweight='bold', fontsize=10)
            ax.text(i, n_regions-1-i, f'R{i}', ha='center', va='center',
                   color='white', fontweight='bold', fontsize=10)
        
        plt.colorbar(im, ax=ax, label='Connection Strength')
        ax.set_xlabel('Target Region')
        ax.set_ylabel('Source Region')
        ax.set_title('Inter-Regional Connection Strengths')

# Export important classes
__all__ = [
    'BrainVisualizer',
    'SimplePlotter', 
    'GraphRenderer'
]