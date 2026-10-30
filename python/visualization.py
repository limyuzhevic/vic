"""
NLM Visualization and Debugging Tools
====================================

Visualization and debugging utilities for NLM (Neural Learning Machine).
Provides tools for visualizing neural networks, debugging simulation issues,
and monitoring system performance.
"""

import pynlm
import matplotlib.pyplot as plt
import matplotlib.animation as animation
from matplotlib.patches import Circle, Rectangle
import numpy as np
import time
import json
import sys
from typing import Optional, List, Dict, Any
from enum import Enum

class VisualizationType(Enum):
    """Types of visualizations available."""
    NETWORK_GRAPH = "network_graph"
    NEURON_ACTIVITY = "neuron_activity"
    BRAIN_STATISTICS = "brain_statistics"
    SIMULATION_PROGRESS = "simulation_progress"
    AGENT_BEHAVIOR = "agent_behavior"
    PERFORMANCE_MONITORING = "performance_monitoring"

class Debugger:
    """
    Debugging utilities for NLM simulations.
    """
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None):
        self.brain = brain
        self.world = world
        self.debug_log = []
        self.error_count = 0
        self.warning_count = 0
        
    def log(self, message: str, level: str = "INFO"):
        """Log debug message."""
        timestamp = time.time()
        log_entry = {
            'timestamp': timestamp,
            'level': level,
            'message': message
        }
        self.debug_log.append(log_entry)
        
        if level == "ERROR":
            self.error_count += 1
            print(f"[ERROR] {message}")
        elif level == "WARNING":
            self.warning_count += 1
            print(f"[WARNING] {message}")
        else:
            print(f"[INFO] {message}")
    
    def validate_brain_state(self) -> Dict[str, Any]:
        """Validate brain state and check for issues."""
        issues = []
        warnings = []
        
        # Check neuron count
        neuron_count = self.brain.getTotalNeuronCount()
        if neuron_count < 10:
            warnings.append(f"Low neuron count ({neuron_count}) may affect network dynamics")
        elif neuron_count < 100:
            warnings.append(f"Moderate neuron count ({neuron_count}) - consider increasing for complex tasks")
        
        # Check synapse count
        synapse_count = self.brain.getTotalSynapseCount()
        if synapse_count < 10:
            warnings.append(f"Low synapse count ({synapse_count}) may limit connectivity")
        
        # Check firing activity
        firing_count = self.brain.getFiringNeuronCount()
        total_spikes = self.brain.getTotalSpikeCount()
        
        if firing_count == 0 and total_spikes > 0:
            warnings.append("Neurons have fired but none are currently firing - possible refractory issues")
        
        if firing_count > neuron_count * 0.9:
            warnings.append(f"Very high firing rate ({firing_count}/{neuron_count}) - consider reducing excitability")
        
        # Check developmental stage
        dev_stage = self.brain.getDevelopmentalStage()
        if dev_stage == pynlm.DevelopmentalStage.Initial:
            warnings.append("Brain is in initial developmental stage - may need time to mature")
        
        # Check plasticity
        if self.brain.getTotalSynapseCount() > 0:
            self.log("Plasticity systems initialized")
        
        return {
            'issues': issues,
            'warnings': warnings,
            'neuron_count': neuron_count,
            'synapse_count': synapse_count,
            'firing_count': firing_count,
            'total_spikes': total_spikes,
            'development_stage': dev_stage,
            'error_count': self.error_count,
            'warning_count': self.warning_count
        }
    
    def check_simulation_stability(self, steps: int, expected_firing_rate: float) -> Dict[str, Any]:
        """Check simulation stability over time."""
        firing_rates = []
        
        for step in range(steps):
            self.brain.step(step)
            firing_rate = self.brain.getAverageFiringRate()
            firing_rates.append(firing_rate)
        
        if len(firing_rates) < 10:
            return {
                'stable': False,
                'reason': 'Insufficient data for stability analysis',
                'firing_rates': firing_rates
            }
        
        # Calculate stability metrics
        mean_firing = np.mean(firing_rates)
        std_firing = np.std(firing_rates)
        coefficient_variation = std_firing / mean_firing if mean_firing > 0 else 0
        
        # Check for oscillations
        max_diff = max(firing_rates) - min(firing_rates)
        
        # Stability criteria
        stable = (
            std_firing < 0.1 and  # Low standard deviation
            coefficient_variation < 0.5 and  # Low coefficient of variation
            max_diff < 0.5 and  # Limited variation
            mean_firing > 0.01  # Some activity present
        )
        
        if stable:
            self.log(f"Simulation stable - mean firing rate: {mean_firing:.3f}")
        else:
            self.log(f"Simulation unstable - std: {std_firing:.3f}, CV: {coefficient_variation:.3f}")
        
        return {
            'stable': stable,
            'mean_firing_rate': mean_firing,
            'std_firing_rate': std_firing,
            'coefficient_variation': coefficient_variation,
            'max_difference': max_diff,
            'min_firing_rate': np.min(firing_rates),
            'max_firing_rate': np.max(firing_rates),
            'expected_firing_rate': expected_firing_rate,
            'error_from_expected': abs(mean_firing - expected_firing_rate)
        }
    
    def check_for_memories(self) -> Dict[str, Any]:
        """Check if memory systems are working."""
        memories = {}
        
        # Check working memory
        working_memory = self.brain.getWorkingMemory()
        if working_memory:
            active_traces = working_memory.getActiveTraces()
            memories['working_memory'] = {
                'active_traces': active_traces,
                'status': 'operational' if active_traces > 0 else 'idle'
            }
        else:
            memories['working_memory'] = {'status': 'not_available'}
        
        # Check episodic memory
        episodic_memory = self.brain.getEpisodicMemory()
        if episodic_memory:
            episode_count = episodic_memory.getEpisodeCount()
            memories['episodic_memory'] = {
                'episode_count': episode_count,
                'status': 'operational' if episode_count > 0 else 'empty'
            }
        else:
            memories['episodic_memory'] = {'status': 'not_available'}
        
        # Check associative memory
        associative_memory = self.brain.getAssociativeMemory()
        if associative_memory:
            memory_strength = associative_memory.getMemoryStrength()
            memories['associative_memory'] = {
                'memory_strength': memory_strength,
                'status': 'operational' if memory_strength > 0 else 'weak'
            }
        else:
            memories['associative_memory'] = {'status': 'not_available'}
        
        return memories
    
    def generate_debug_report(self) -> Dict[str, Any]:
        """Generate comprehensive debug report."""
        return {
            'brain_validation': self.validate_brain_state(),
            'memory_systems': self.check_for_memories(),
            'debug_log_length': len(self.debug_log),
            'error_count': self.error_count,
            'warning_count': self.warning_count,
            'latest_logs': self.debug_log[-10:] if self.debug_log else []
        }
class NLMVisualizer:
    """
    Visualization tools for NLM systems.
    """
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None):
        self.brain = brain
        self.world = world
        self.figure_size = (12, 8)
        self.colors = {
            'excitatory': '#FF6B6B',
            'inhibitory': '#4ECDC4',
            'sensory': '#45B7D1',
            'motor': '#96CEB4',
            'internal': '#FFEAA7',
            'modulatory': '#DDA0DD'
        }
        
    def create_network_graph(self, save_path: Optional[str] = None) -> plt.Figure:
        """
        Create a 2D network graph visualization.
        """
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        # Get regions
        regions = self.brain.getRegions()
        if not regions:
            self._create_empty_network_graph(ax)
            return fig
        
        # Position regions
        n_regions = len(regions)
        for i, region in enumerate(regions):
            angle = 2 * np.pi * i / n_regions
            x = np.cos(angle) * 0.8
            y = np.sin(angle) * 0.8
            
            # Draw region circle
            region_circle = Circle((x, y), 0.1, color='steelblue', alpha=0.7, linewidth=2)
            ax.add_patch(region_circle)
            
            # Add region label
            ax.text(x, y, f"Region {i+1}", 
                   ha='center', va='center', fontsize=10, fontweight='bold')
            
            # Add neuron count
            neuron_count = region.getTotalNeuronCount()
            ax.text(x, y - 0.15, f"{neuron_count} neurons", 
                   ha='center', va='center', fontsize=8)
        
        # Draw connections between regions
        for i, region1 in enumerate(regions):
            for j, region2 in enumerate(regions):
                if i < j:
                    angle1 = 2 * np.pi * i / n_regions
                    angle2 = 2 * np.pi * j / n_regions
                    
                    x1, y1 = np.cos(angle1) * 0.8, np.sin(angle1) * 0.8
                    x2, y2 = np.cos(angle2) * 0.8, np.sin(angle2) * 0.8
                    
                    # Calculate connection strength
                    if hasattr(region1, 'getTotalSynapseCount') and hasattr(region2, 'getTotalSynapseCount'):
                        conn_strength = min(region1.getTotalSynapseCount(), region2.getTotalSynapseCount()) / 1000.0
                    else:
                        conn_strength = 0.5
                    
                    # Draw connection line
                    alpha = conn_strength
                    ax.plot([x1, x2], [y1, y2], 'gray', alpha=alpha, linewidth=conn_strength * 2)
        
        ax.set_xlim(-1.2, 1.2)
        ax.set_ylim(-1.2, 1.2)
        ax.set_aspect('equal')
        ax.axis('off')
        ax.set_title('NLM Neural Network Architecture', fontsize=14, fontweight='bold')
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def _create_empty_network_graph(self, ax):
        """Create empty network graph when no regions exist."""
        ax.text(0, 0, "No regions found\nRun brain.initialize() first", 
               ha='center', va='center', fontsize=12)
        ax.text(0, -0.2, "Total neurons: " + str(self.brain.getTotalNeuronCount()), 
               ha='center', va='center', fontsize=10)
        ax.text(0, -0.4, "Total synapses: " + str(self.brain.getTotalSynapseCount()), 
               ha='center', va='center', fontsize=10)
        
    def create_neuron_activity_plot(self, steps: int = 100, save_path: Optional[str] = None) -> plt.Figure:
        """
        Create neuron activity over time plot.
        """
        fig, ax = plt.subplots(figsize=self.figure_size)
        
        firing_rates = []
        spike_counts = []
        
        for step in range(steps):
            self.brain.step(step)
            firing_rate = self.brain.getAverageFiringRate()
            firing_rates.append(firing_rate)
            spike_counts.append(self.brain.getTotalSpikeCount())
        
        # Plot firing rate
        ax.subplot(2, 1, 1)
        ax.plot(firing_rates, 'b-', linewidth=2, label='Average Firing Rate')
        ax.set_ylabel('Firing Rate')
        ax.set_title('Neuron Activity Over Time')
        ax.grid(True, alpha=0.3)
        ax.legend()
        
        # Plot spike count
        ax.subplot(2, 1, 2)
        ax.plot(spike_counts, 'r-', linewidth=2, label='Total Spike Count')
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Spike Count')
        ax.grid(True, alpha=0.3)
        ax.legend()
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def create_brain_statistics_plot(self, save_path: Optional[str] = None) -> plt.Figure:
        """
        Create brain statistics visualization.
        """
        fig, axes = plt.subplots(2, 3, figsize=self.figure_size)
        axes = axes.flatten()
        
        # Get brain statistics
        total_neurons = self.brain.getTotalNeuronCount()
        total_synapses = self.brain.getTotalSynapseCount()
        active_neurons = self.brain.getActiveNeuronCount()
        firing_neurons = self.brain.getFiringNeuronCount()
        average_firing = self.brain.getAverageFiringRate()
        e_i_ratio = self.brain.getExcitationInhibitionRatio()
        total_spikes = self.brain.getTotalSpikeCount()
        development_stage = self.brain.getDevelopmentalStage()
        
        # Statistics
        stats_data = [
            ('Total Neurons', total_neurons, ' Neurons'),
            ('Total Synapses', total_synapses, ' Synapses'),
            ('Active Neurons', active_neurons, ' Neurons'),
            ('Firing Neurons', firing_neurons, ' Neurons'),
            ('Average Firing', average_firing, ' Hz'),
            ('E/I Ratio', e_i_ratio, ''),
            ('Total Spikes', total_spikes, ' Spikes'),
            ('Development', str(development_stage), ''),
        ]
        
        # Create bar charts
        for i, (name, value, unit) in enumerate(stats_data):
            if i >= len(axes):
                break
                
            ax = axes[i]
            
            # Normalize values for better visualization
            if 'Firing' in name or 'Spikes' in name:
                max_val = max(v for _, v, _ in stats_data)
                normalized_value = value / max_val * 100
            else:
                normalized_value = value
            
            bars = ax.bar(0, normalized_value, color='steelblue', alpha=0.7)
            ax.bar(0, normalized_value, color='steelblue', alpha=0.7)
            ax.set_ylabel(f"{value}{unit}" if isinstance(value, (int, float)) else name)
            ax.set_title(name)
            ax.set_xlim(-0.5, 0.5)
            ax.set_xticks([])
            
            # Add value label
            if isinstance(value, (int, float)):
                ax.text(0, normalized_value + 0.05, f"{value:.1f}", 
                       ha='center', va='bottom', fontsize=9)
            else:
                ax.text(0, normalized_value + 0.05, value, 
                       ha='center', va='bottom', fontsize=9)
        
        # Hide unused subplots
        for i in range(len(stats_data), len(axes)):
            axes[i].set_visible(False)
        
        fig.suptitle('Brain Statistics', fontsize=16, fontweight='bold')
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def create_simulation_progress(self, steps: int, update_interval: int = 10, 
                                  save_animation: Optional[str] = None) -> animation.FuncAnimation:
        """
        Create simulation progress animation.
        """
        fig, ax = plt.subplots(figsize=(10, 6))
        
        # Initialize data
        x_data = list(range(0, steps, update_interval))
        y_firing = []
        y_spikes = []
        y_curiosity = []
        
        # Create lines
        line_firing, = ax.plot([], [], 'b-', label='Firing Rate', linewidth=2)
        line_spikes, = ax.plot([], [], 'r-', label='Total Spikes', linewidth=2)
        line_curiosity, = ax.plot([], [], 'g-', label='Curiosity Level', linewidth=2)
        
        # Create text box
        info_text = ax.text(0.02, 0.98, '', transform=ax.transAxes, 
                          fontsize=10, verticalalignment='top',
                          bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        ax.set_xlim(0, steps)
        ax.set_ylim(0, 1)
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Normalized Values')
        ax.set_title('Simulation Progress')
        ax.legend(loc='upper left')
        ax.grid(True, alpha=0.3)
        
        def init():
            line_firing.set_data([], [])
            line_spikes.set_data([], [])
            line_curiosity.set_data([], [])
            info_text.set_text('')
            return line_firing, line_spikes, line_curiosity, info_text
        
        def animate(frame):
            step = frame * update_interval
            if step >= steps:
                return line_firing, line_spikes, line_curiosity, info_text
            
            # Simulate step
            self.brain.step(step)
            
            # Get metrics
            firing_rate = self.brain.getAverageFiringRate()
            total_spikes = self.brain.getTotalSpikeCount()
            
            # Normalize for visualization
            max_firing = 10.0  # Approximate max firing rate
            max_spikes = 1000.0  # Approximate max spikes
            
            # Update data
            y_firing.append(firing_rate / max_firing if max_firing > 0 else 0)
            y_spikes.append(total_spikes / max_spikes if max_spikes > 0 else 0)
            y_curiosity.append(0.5)  # Simplified curiosity
            
            # Update plots
            line_firing.set_data(x_data[:len(y_firing)], y_firing)
            line_spikes.set_data(x_data[:len(y_spikes)], y_spikes)
            line_curiosity.set_data(x_data[:len(y_curiosity)], y_curiosity)
            
            # Update info text
            info_text.set_text(f"Step: {step}\n"
                             f"Firing Rate: {firing_rate:.3f}\n"
                             f"Total Spikes: {total_spikes}\n"
                             f"Development Stage: {self.brain.getDevelopmentalStage()}")
            
            return line_firing, line_spikes, line_curiosity, info_text
        
        anim = animation.FuncAnimation(fig, animate, init_func=init, 
                                      frames=steps // update_interval, interval=100,
                                      blit=True, repeat=False)
        
        if save_animation:
            anim.save(save_animation, writer='pillow', fps=10)
        
        return anim
    
    def create_comprehensive_dashboard(self, save_path: Optional[str] = None) -> plt.Figure:
        """
        Create comprehensive dashboard with multiple visualizations.
        """
        fig = plt.figure(figsize=(16, 12))
        fig.suptitle('NLM System Dashboard', fontsize=20, fontweight='bold')
        
        # Create subplots
        gs = fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        # 1. Network architecture
        ax1 = fig.add_subplot(gs[0, :2])
        self._draw_simple_network(ax1)
        
        # 2. Current statistics
        ax2 = fig.add_subplot(gs[0, 2:])
        self._create_statistics_pie_chart(ax2)
        
        # 3. Activity over time
        ax3 = fig.add_subplot(gs[1, :3])
        self._create_activity_plot(ax3)
        
        # 4. Neuron distribution
        ax4 = fig.add_subplot(gs[1, 3])
        self._create_neuron_distribution(ax4)
        
        # 5. Development stage
        ax5 = fig.add_subplot(gs[2, :2])
        self._create_development_chart(ax5)
        
        # 6. Performance metrics
        ax6 = fig.add_subplot(gs[2, 2:])
        self._create_performance_chart(ax6)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig
    
    def _draw_simple_network(self, ax):
        """Draw simple network diagram."""
        ax.text(0.5, 0.5, 'NLM Network', 
               ha='center', va='center', fontsize=14, fontweight='bold',
               bbox=dict(boxstyle='round', facecolor='lightblue', alpha=0.7))
        
        # Draw neurons
        neuron_pos = [(0.3, 0.3), (0.7, 0.3), (0.3, 0.7), (0.7, 0.7)]
        neuron_types = ['Sensory', 'Internal', 'Motor', 'Modulatory']
        
        for i, (x, y) in enumerate(neuron_pos):
            color = self.colors.get(neuron_types[i], 'gray')
            circle = Circle((x, y), 0.1, color=color, alpha=0.8)
            ax.add_patch(circle)
            ax.text(x, y, neuron_types[i], ha='center', va='center', fontsize=10)
        
        # Draw connections
        for i in range(3):
            ax.plot([neuron_pos[i][0], neuron_pos[i+1][0]], 
                   [neuron_pos[i][1], neuron_pos[i+1][1]], 
                   'black', alpha=0.5, linewidth=1)
        
        ax.set_xlim(0, 1)
        ax.set_ylim(0, 1)
        ax.axis('off')
    
    def _create_statistics_pie_chart(self, ax):
        """Create pie chart of brain statistics."""
        # Get data
        total_neurons = self.brain.getTotalNeuronCount()
        total_synapses = self.brain.getTotalSynapseCount()
        
        # Calculate percentages
        if total_neurons + total_synapses > 0:
            neuron_percent = total_neurons / (total_neurons + total_synapses) * 100
            synapse_percent = total_synapses / (total_neurons + total_synapses) * 100
        else:
            neuron_percent = 50
            synapse_percent = 50
        
        sizes = [neuron_percent, synapse_percent]
        labels = ['Neurons', 'Synapses']
        colors = ['lightcoral', 'lightblue']
        
        wedges, texts, autotext = ax.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%',
                                       startangle=90, textprops={'fontsize': 10})
        ax.set_title('Network Composition', fontsize=12)
    
    def _create_activity_plot(self, ax):
        """Create activity over time plot."""
        steps = 50
        firing_rates = []
        
        for step in range(steps):
            self.brain.step(step)
            firing_rates.append(self.brain.getAverageFiringRate())
        
        ax.plot(firing_rates, 'b-', linewidth=2)
        ax.set_xlabel('Step')
        ax.set_ylabel('Firing Rate')
        ax.set_title('Activity Over Time')
        ax.grid(True, alpha=0.3)
        
        # Add horizontal line for average
        avg_rate = np.mean(firing_rates)
        ax.axhline(y=avg_rate, color='r', linestyle='--', alpha=0.7, label=f'Average: {avg_rate:.3f}')
        ax.legend()
    
    def _create_neuron_distribution(self, ax):
        """Create neuron distribution bar chart."""
        # Simulate distribution (in real implementation, get from brain)
        neuron_types = ['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Internal', 'Modulatory']
        counts = [45, 15, 10, 10, 20, 10]  # Simulated distribution
        colors = [self.colors[nt.lower()] if nt.lower() in self.colors else 'gray' for nt in neuron_types]
        
        bars = ax.bar(neuron_types, counts, color=colors, alpha=0.8)
        ax.set_ylabel('Count')
        ax.set_title('Neuron Distribution')
        ax.tick_params(axis='x', rotation=45)
        
        # Add value labels on bars
        for bar, count in zip(bars, counts):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{count}', ha='center', va='bottom')
    
    def _create_development_chart(self, ax):
        """Create development stage chart."""
        stages = list(pynlm.DevelopmentalStage)
        stage_names = [stage.name for stage in stages]
        
        # Simulate development progress
        progress = [0.1, 0.3, 0.6, 0.8, 0.9, 1.0]  # Normalized progress
        
        ax.barh(stage_names, progress, color='green', alpha=0.7)
        ax.set_xlabel('Development Progress')
        ax.set_title('Development System')
        ax.set_xlim(0, 1.1)
        
        # Add current stage indicator
        current_stage = self.brain.getDevelopmentalStage()
        current_idx = list(pynlm.DevelopmentalStage).index(current_stage)
        ax.axvline(x=progress[current_idx], color='red', linestyle='--', alpha=0.8, label=f'Current: {current_stage}')
        ax.legend()
    
    def _create_performance_chart(self, ax):
        """Create performance metrics chart."""
        # Simulate performance metrics
        metrics = ['Firing Rate', 'Memory', 'Plasticity', 'Curiosity', 'Development']
        values = [self.brain.getAverageFiringRate(), 0.7, 0.8, self.brain.getTotalSpikeCount() / 1000, 0.6]
        
        colors = ['red', 'blue', 'green', 'orange', 'purple']
        
        bars = ax.bar(metrics, values, color=colors, alpha=0.8)
        ax.set_ylabel('Performance (0-1)')
        ax.set_title('System Performance')
        ax.set_ylim(0, 1)
        
        # Add value labels
        for bar, value in zip(bars, values):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height + 0.05,
                   f'{value:.2f}', ha='center', va='bottom', fontsize=8)
class NLMDebuggerGUI:
    """
    Simple GUI for NLM debugging and visualization.
    """
    
    def __init__(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None):
        self.brain = brain
        self.world = world
        self.debugger = Debugger(brain, world)
        self.visualizer = NLMVisualizer(brain, world)
        
        # Setup matplotlib for GUI
        plt.ion()
        self.setup_gui()
    
    def setup_gui(self):
        """Setup GUI components."""
        self.fig = plt.figure(figsize=(15, 10))
        self.fig.suptitle('NLM Debugger GUI', fontsize=16, fontweight='bold')
        
        # Create subplots
        self.gs = self.fig.add_gridspec(3, 4, hspace=0.3, wspace=0.3)
        
        # Initialize plot components
        self.setup_plots()
        
        # Create control panel
        self.setup_controls()
        
    def setup_plots(self):
        """Setup visualization plots."""
        # Network graph
        self.ax_network = self.fig.add_subplot(self.gs[0, :2])
        
        # Statistics
        self.ax_stats = self.fig.add_subplot(self.gs[0, 2:])
        
        # Activity plot
        self.ax_activity = self.fig.add_subplot(self.gs[1, :3])
        
        # Debug log
        self.ax_log = self.fig.add_subplot(self.gs[1, 3])
        
        # Controls
        self.ax_controls = self.fig.add_subplot(self.gs[2, :4])
        
    def setup_controls(self):
        """Setup control buttons and sliders."""
        # Create buttons
        control_text = "Controls:\n"
        control_text += "1. Network Graph - Show network architecture\n"
        control_text += "2. Statistics - Show current statistics\n"
        control_text += "3. Activity - Show activity over time\n"
        control_text += "4. Debug - Show debug logs\n"
        control_text += "5. Validate - Validate brain state\n"
        control_text += "6. Progress - Run simulation with updates\n"
        control_text += "7. Save - Save visualizations\n"
        
        self.ax_controls.text(0.1, 0.8, control_text, fontsize=10,
                             verticalalignment='top', horizontalalignment='left')
        
        # Status indicator
        self.status_text = self.ax_controls.text(0.1, 0.3, "Status: Ready", 
                                               fontsize=12, fontweight='bold',
                                               color='green')
    
    def update_plots(self):
        """Update all plots with current brain state."""
        # Update network graph
        self._update_network_plot()
        
        # Update statistics
        self._update_statistics_plot()
        
        # Update activity plot
        self._update_activity_plot()
        
        # Update debug log
        self._update_debug_log()
        
        # Update controls
        self._update_controls()
        
    def _update_network_plot(self):
        """Update network graph plot."""
        self.ax_network.clear()
        self.visualizer._draw_simple_network(self.ax_network)
        self.ax_network.set_title('Network Architecture', fontsize=12)
    
    def _update_statistics_plot(self):
        """Update statistics plot."""
        # Create simple statistics display
        stats = self.debugger.validate_brain_state()
        
        stats_text = f"Statistics:\n"
        stats_text += f"Neurons: {stats['neuron_count']}\n"
        stats_text += f"Synapses: {stats['synapse_count']}\n"
        stats_text += f"Firing Rate: {self.brain.getAverageFiringRate():.3f}\n"
        stats_text += f"Development: {self.brain.getDevelopmentalStage()}\n"
        stats_text += f"Errors: {stats['error_count']}\n"
        stats_text += f"Warnings: {stats['warning_count']}\n"
        
        self.ax_stats.text(0.5, 0.5, stats_text, 
                         ha='center', va='center', fontsize=10,
                         bbox=dict(boxstyle='round', facecolor='lightyellow', alpha=0.7))
        self.ax_stats.set_xlim(0, 1)
        self.ax_stats.set_ylim(0, 1)
        self.ax_stats.axis('off')
    
    def _update_activity_plot(self):
        """Update activity plot."""
        # Run a few simulation steps
        steps = 20
        firing_rates = []
        
        for step in range(steps):
            self.brain.step(step)
            firing_rates.append(self.brain.getAverageFiringRate())
        
        self.ax_activity.clear()
        self.ax_activity.plot(firing_rates, 'b-', linewidth=2)
        self.ax_activity.set_xlabel('Step')
        self.ax_activity.set_ylabel('Firing Rate')
        self.ax_activity.set_title('Activity Over Time')
        self.ax_activity.grid(True, alpha=0.3)
    
    def _update_debug_log(self):
        """Update debug log display."""
        self.ax_log.clear()
        
        log_text = "Debug Log:\n"
        recent_logs = self.debugger.debug_log[-5:] if self.debugger.debug_log else []
        
        for log in recent_logs:
            timestamp = log['timestamp']
            level = log['level']
            message = log['message']
            
            log_text += f"{level}: {message}\n"
        
        self.ax_log.text(0.1, 0.9, log_text, fontsize=8,
                        verticalalignment='top', horizontalalignment='left',
                        bbox=dict(boxstyle='round', facecolor='black', alpha=0.7))
        self.ax_log.set_xlim(0, 1)
        self.ax_log.set_ylim(0, 1)
        self.ax_log.axis('off')
    
    def _update_controls(self):
        """Update control panel."""
        self.ax_controls.clear()
        
        control_text = "Interactive Controls:\n"
        control_text += "1. Network Graph - Show network architecture\n"
        control_text += "2. Statistics - Show current statistics\n"
        control_text += "3. Activity - Show activity over time\n"
        control_text += "4. Debug - Show debug logs\n"
        control_text += "5. Validate - Validate brain state\n"
        control_text += "6. Progress - Run simulation with updates\n"
        control_text += "7. Save - Save visualizations\n"
        control_text += "\nStatus: Active"
        
        self.ax_controls.text(0.1, 0.8, control_text, fontsize=10,
                             verticalalignment='top', horizontalalignment='left')
        self.ax_controls.set_xlim(0, 1)
        self.ax_controls.set_ylim(0, 1)
        self.ax_controls.axis('off')
    
    def run(self):
        """Run the GUI."""
        self.update_plots()
        plt.draw()
        plt.pause(0.1)
        
    def close(self):
        """Close the GUI."""
        plt.close(self.fig)

class NLMPerformanceMonitor:
    """
    Performance monitoring for NLM simulations.
    """
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.performance_data = []
        self.monitoring = False
        
    def start_monitoring(self, interval: float = 1.0, max_steps: int = 0):
        """
        Start performance monitoring.
        
        Args:
            interval: Time interval between measurements (seconds)
            max_steps: Maximum number of steps to monitor (0 = unlimited)
        """
        self.monitoring = True
        self.start_time = time.time()
        self.step_count = 0
        self.measurement_interval = interval
        self.max_steps = max_steps
        
        self.log(f"Started performance monitoring (interval: {interval}s, max steps: {max_steps})")
    
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring = False
        self.log("Stopped performance monitoring")
    
    def record_measurement(self, step: int, timestamp: float):
        """
        Record performance measurement.
        
        Args:
            step: Current simulation step
            timestamp: Current simulation timestamp
        """
        if not self.monitoring:
            return
        
        # Get performance metrics
        metrics = {
            'step': step,
            'timestamp': timestamp,
            'time_elapsed': time.time() - self.start_time,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'firing_rate': self.brain.getAverageFiringRate(),
            'active_neurons': self.brain.getActiveNeuronCount(),
            'firing_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'development_stage': self.brain.getDevelopmentalStage()
        }
        
        self.performance_data.append(metrics)
        self.step_count += 1
        
        # Check if we've reached max steps
        if self.max_steps > 0 and step >= self.max_steps:
            self.stop_monitoring()
    
    def log(self, message: str):
        """Log monitoring message."""
        timestamp = time.time()
        log_entry = {
            'timestamp': timestamp,
            'message': message
        }
        
        # Print to console
        print(f"[MONITOR] {message}")
        
        # Store in performance data
        if self.performance_data:
            self.performance_data[-1]['log'] = message
    
    def get_performance_report(self) -> Dict[str, Any]:
        """Get performance report."""
        if not self.performance_data:
            return {'error': 'No performance data available'}
        
        # Calculate statistics
        total_time = self.performance_data[-1]['time_elapsed']
        total_steps = self.step_count
        
        if total_time > 0:
            steps_per_second = total_steps / total_time
            avg_time_per_step = total_time / total_steps if total_steps > 0 else 0
        else:
            steps_per_second = 0
            avg_time_per_step = 0
        
        # Get firing rate statistics
        firing_rates = [d['firing_rate'] for d in self.performance_data]
        spike_counts = [d['total_spikes'] for d in self.performance_data]
        
        report = {
            'monitoring_duration': total_time,
            'total_steps': total_steps,
            'steps_per_second': steps_per_second,
            'average_time_per_step': avg_time_per_step,
            'average_firing_rate': statistics.mean(firing_rates) if firing_rates else 0,
            'firing_rate_std': statistics.stdev(firing_rates) if len(firing_rates) > 1 else 0,
            'min_firing_rate': min(firing_rates) if firing_rates else 0,
            'max_firing_rate': max(firing_rates) if firing_rates else 0,
            'total_spikes': sum(spike_counts),
            'spike_rate_per_second': sum(spike_counts) / total_time if total_time > 0 else 0,
            'neuron_count': self.brain.getTotalNeuronCount(),
            'synapse_count': self.brain.getTotalSynapseCount(),
            'measurements_count': len(self.performance_data)
        }
        
        return report
    
    def save_performance_data(self, filename: str):
        """Save performance data to file."""
        with open(filename, 'w') as f:
            json.dump(self.performance_data, f, indent=2)
        
        self.log(f"Saved performance data to {filename}")
    
    def load_performance_data(self, filename: str):
        """Load performance data from file."""
        try:
            with open(filename, 'r') as f:
                self.performance_data = json.load(f)
            
            self.step_count = len(self.performance_data)
            self.log(f"Loaded performance data from {filename}")
            
        except Exception as e:
            self.log(f"Error loading performance data: {e}")
    
    def generate_performance_plot(self, save_path: Optional[str] = None) -> plt.Figure:
        """
        Generate performance plot.
        
        Args:
            save_path: Path to save the plot (optional)
        
        Returns:
            Matplotlib figure
        """
        if not self.performance_data:
            raise ValueError("No performance data available")
        
        fig, axes = plt.subplots(2, 2, figsize=(12, 10))
        axes = axes.flatten()
        
        # 1. Steps per second over time
        steps_data = [d['step'] for d in self.performance_data]
        time_data = [d['time_elapsed'] for d in self.performance_data]
        
        ax1 = axes[0]
        ax1.plot(time_data, steps_data, 'b-', linewidth=2)
        ax1.set_xlabel('Time (s)')
        ax1.set_ylabel('Steps')
        ax1.set_title('Steps Over Time')
        ax1.grid(True, alpha=0.3)
        
        # 2. Firing rate over time
        firing_rates = [d['firing_rate'] for d in self.performance_data]
        
        ax2 = axes[1]
        ax2.plot(time_data, firing_rates, 'r-', linewidth=2)
        ax2.set_xlabel('Time (s)')
        ax2.set_ylabel('Firing Rate')
        ax2.set_title('Firing Rate Over Time')
        ax2.grid(True, alpha=0.3)
        
        # 3. Performance metrics
        perf_metrics = ['Firing Rate', 'Active Neurons', 'Firing Neurons', 'Total Spikes']
        current_values = [
            firing_rates[-1] if firing_rates else 0,
            self.brain.getActiveNeuronCount(),
            self.brain.getFiringNeuronCount(),
            self.brain.getTotalSpikeCount()
        ]
        
        ax3 = axes[2]
        bars = ax3.bar(perf_metrics, current_values, color=['red', 'blue', 'green', 'orange'], alpha=0.7)
        ax3.set_ylabel('Value')
        ax3.set_title('Current Performance')
        ax3.set_ylim(0, max(current_values) * 1.1 if current_values else 1)
        
        # Add value labels on bars
        for bar, value in zip(bars, current_values):
            height = bar.get_height()
            ax3.text(bar.get_x() + bar.get_width()/2., height,
                    f'{value}', ha='center', va='bottom')
        
        # 4. Performance metrics summary
        report = self.get_performance_report()
        
        summary_text = f"Performance Summary:\n"
        summary_text += f"Total Steps: {report['total_steps']}\n"
        summary_text += f"Duration: {report['monitoring_duration']:.2f}s\n"
        summary_text += f"Steps/s: {report['steps_per_second']:.1f}\n"
        summary_text += f"Avg Firing Rate: {report['average_firing_rate']:.3f}\n"
        summary_text += f"Firing Rate Std: {report['firing_rate_std']:.3f}\n"
        summary_text += f"Total Spikes: {report['total_spikes']}\n"
        summary_text += f"Spike Rate/s: {report['spike_rate_per_second']:.1f}"
        
        ax4 = axes[3]
        ax4.text(0.1, 0.9, summary_text, fontsize=10, verticalalignment='top',
                bbox=dict(boxstyle='round', facecolor='lightgray', alpha=0.8))
        ax4.set_xlim(0, 1)
        ax4.set_ylim(0, 1)
        ax4.axis('off')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=300, bbox_inches='tight')
        
        return fig

def create_comprehensive_visualization_suite(brain: pynlm.Brain, 
                                               world: Optional[pynlm.SimpleWorld] = None,
                                               save_directory: Optional[str] = None) -> Dict[str, Any]:
    """
    Create comprehensive visualization suite for NLM system.
    
    Args:
        brain: NLM brain instance
        world: NLM world instance (optional)
        save_directory: Directory to save visualizations (optional)
    
    Returns:
        Dictionary with paths to generated visualizations
    """
    results = {}
    
    if save_directory and not os.path.exists(save_directory):
        os.makedirs(save_directory)
    
    # Create debugger and visualizer
    debugger = Debugger(brain, world)
    visualizer = NLMVisualizer(brain, world)
    
    # Run validation
    validation = debugger.validate_brain_state()
    results['validation'] = validation
    
    # Create network graph
    network_fig = visualizer.create_network_graph()
    network_path = os.path.join(save_directory, 'network_graph.png') if save_directory else None
    if network_path:
        network_fig.savefig(network_path, dpi=300, bbox_inches='tight')
    results['network_graph'] = network_path
    
    # Create brain statistics plot
    stats_fig = visualizer.create_brain_statistics_plot()
    stats_path = os.path.join(save_directory, 'brain_statistics.png') if save_directory else None
    if stats_path:
        stats_fig.savefig(stats_path, dpi=300, bbox_inches='tight')
    results['brain_statistics'] = stats_path
    
    # Create activity plot
    activity_fig = visualizer.create_neuron_activity_plot(steps=100)
    activity_path = os.path.join(save_directory, 'activity_over_time.png') if save_directory else None
    if activity_path:
        activity_fig.savefig(activity_path, dpi=300, bbox_inches='tight')
    results['activity_plot'] = activity_path
    
    # Create comprehensive dashboard
    dashboard_fig = visualizer.create_comprehensive_dashboard()
    dashboard_path = os.path.join(save_directory, 'dashboard.png') if save_directory else None
    if dashboard_path:
        dashboard_fig.savefig(dashboard_path, dpi=300, bbox_inches='tight')
    results['dashboard'] = dashboard_path
    
    # Generate debug report
    debug_report = debugger.generate_debug_report()
    results['debug_report'] = debug_report
    
    return results

def run_performance_monitoring(brain: pynlm.Brain, 
                               duration: float = 60.0,
                               steps_per_interval: int = 10) -> Dict[str, Any]:
    """
    Run performance monitoring for specified duration.
    
    Args:
        brain: NLM brain instance
        duration: Monitoring duration in seconds
        steps_per_interval: Steps between measurements
    
    Returns:
        Performance monitoring results
    """
    monitor = NLMPerformanceMonitor(brain)
    
    # Start monitoring
    monitor.start_monitoring(interval=1.0, max_steps=0)
    
    # Run simulation for specified duration
    start_time = time.time()
    interval_count = int(duration)
    
    for interval in range(interval_count):
        # Run steps for this interval
        for step in range(steps_per_interval):
            current_step = interval * steps_per_interval + step
            monitor.record_measurement(current_step, current_step * 0.001)
        
        # Print progress
        print(f"Progress: {((interval + 1) / interval_count) * 100:.1f}%")
    
    # Stop monitoring
    monitor.stop_monitoring()
    
    # Generate report
    report = monitor.get_performance_report()
    
    # Save performance data
    monitor.save_performance_data('performance_monitoring_data.json')
    
    # Generate performance plot
    plot_path = 'performance_monitoring.png'
    monitor.generate_performance_plot(plot_path)
    
    results = {
        'report': report,
        'performance_data_path': 'performance_monitoring_data.json',
        'plot_path': plot_path
    }
    
    return results
class NLMTools:
    """
    Main toolkit providing all visualization and debugging utilities.
    """
    
    def __init__(self):
        self.debugger = None
        self.visualizer = None
        self.performance_monitor = None
    
    def setup(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None):
        """Setup tools for NLM system."""
        self.debugger = Debugger(brain, world)
        self.visualizer = NLMVisualizer(brain, world)
        self.performance_monitor = NLMPerformanceMonitor(brain)
    
    def get_debugger(self) -> Debugger:
        """Get debugger instance."""
        if self.debugger is None:
            raise RuntimeError("Debugger not initialized. Call setup() first.")
        return self.debugger
    
    def get_visualizer(self) -> NLMVisualizer:
        """Get visualizer instance."""
        if self.visualizer is None:
            raise RuntimeError("Visualizer not initialized. Call setup() first.")
        return self.visualizer
    
    def get_performance_monitor(self) -> NLMPerformanceMonitor:
        """Get performance monitor instance."""
        if self.performance_monitor is None:
            raise RuntimeError("Performance monitor not initialized. Call setup() first.")
        return self.performance_monitor
    
    def create_visualization_suite(self, save_directory: Optional[str] = None) -> Dict[str, Any]:
        """Create complete visualization suite."""
        if self.visualizer is None or self.debugger is None:
            raise RuntimeError("Tools not initialized. Call setup() first.")
        
        return create_comprehensive_visualization_suite(self.debugger.brain, 
                                                       self.debugger.world, 
                                                       save_directory)
    
    def run_performance_monitoring(self, duration: float = 60.0) -> Dict[str, Any]:
        """Run performance monitoring."""
        if self.performance_monitor is None:
            raise RuntimeError("Performance monitor not initialized. Call setup() first.")
        
        return run_performance_monitoring(self.performance_monitor.brain, duration)
    
    def validate_system(self) -> Dict[str, Any]:
        """Validate NLM system."""
        if self.debugger is None:
            raise RuntimeError("Debugger not initialized. Call setup() first.")
        
        return self.debugger.validate_brain_state()

# Global toolkit instance
_toolkit = NLMTools()

def get_toolkit() -> NLMTools:
    """Get global toolkit instance."""
    return _toolkit

def setup_toolkit(brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None):
    """Setup global toolkit."""
    _toolkit.setup(brain, world)

def create_visualization_suite(brain: pynlm.Brain, 
                               world: Optional[pynlm.SimpleWorld] = None,
                               save_directory: Optional[str] = None) -> Dict[str, Any]:
    """Create comprehensive visualization suite."""
    return create_comprehensive_visualization_suite(brain, world, save_directory)

def run_performance_monitoring(brain: pynlm.Brain, 
                               duration: float = 60.0) -> Dict[str, Any]:
    """Run performance monitoring."""
    return run_performance_monitoring(brain, duration)

if __name__ == '__main__':
    print("NLM Visualization and Debugging Tools")
    print("====================================")
    print()
    print("This module provides comprehensive visualization and debugging")
    print("utilities for NLM (Neural Learning Machine) simulations.")
    print()
    print("Key features:")
    print("  - Network graph visualization")
    print("  - Brain statistics and analysis")
    print("  - Activity over time monitoring")
    print("  - Performance monitoring and analysis")
    print("  - Debug validation and error reporting")
    print("  - Comprehensive dashboard")
    print("  - Interactive GUI support")
    print()
    print("Usage example:")
    print("  from nlm import visualization, debugging")
    print("  ")
    print("  # Create NLM system")
    print("  brain = pynlm.createBrain(pynlm.createDefaultConfig())")
    print("  brain.initialize()")
    print("  ")
    print("  # Setup tools")
    print("  tools = visualization.get_toolkit()")
    print("  tools.setup(brain)")
    print("  ")
    print("  # Run simulation with monitoring")
    print("  monitor = tools.get_performance_monitor()")
    print("  monitor.start_monitoring(duration=60.0)")
    print("  ")
    print("  for step in range(1000):")
    print("      brain.step(step)")
    print("  ")
    print("  # Generate visualizations")
    print("  suite = tools.create_visualization_suite()")
    print("  ")
    print("  # Validate system")
    print("  validation = tools.validate_system()")