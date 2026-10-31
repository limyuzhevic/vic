"""
Visualization module for NLM Python bindings.
"""

import time
import math
from typing import Dict, List, Any, Optional, Tuple, Union
from enum import Enum
import matplotlib.pyplot as plt
import numpy as np

try:
    from pynlm.bindings import createBrain, createSimpleWorld
    from pynlm.bindings import createAgentBrain, NeuronType, ActionType
    from pynlm.development import DevelopmentalStage
    BINDINGS_AVAILABLE = True
except ImportError:
    BINDINGS_AVAILABLE = False
    print("Warning: C++ bindings not available. Visualization module limited.")


class VisualizationBackend(Enum):
    """Visualization backends."""
    MATPLOTLIB = "matplotlib"
    PLOTLY = "plotly"
    ASCII = "ascii"
    NONE = "none"


class PlotStyle:
    """Plot styling configuration."""
    
    def __init__(self, backend: VisualizationBackend = VisualizationBackend.MATPLOTLIB):
        """
        Initialize plot style.
        
        Args:
            backend: Visualization backend
        """
        self.backend = backend
        self.color_scheme = {
            'excitatory': '#FF6B6B',
            'inhibitory': '#4ECDC4',
            'sensory': '#45B7D1',
            'motor': '#96CEB4',
            'internal': '#FFEAA7',
            'background': '#FFFFFF',
            'grid': '#E0E0E0',
            'text': '#2D3436',
        }
        self.figsize = (12, 8)
        self.dpi = 100
        self.font_size = 10
        self.line_width = 1.5
        self.marker_size = 6
        self.alpha = 0.7
    
    def set_color(self, element: str, color: str):
        """
        Set color for plot element.
        
        Args:
            element: Element name
            color: Color value
        """
        if element in self.color_scheme:
            self.color_scheme[element] = color
    
    def get_color(self, element: str) -> str:
        """
        Get color for plot element.
        
        Args:
            element: Element name
        
        Returns:
            Color value or default if not found
        """
        return self.color_scheme.get(element, self.color_scheme['text'])
    
    def configure_matplotlib(self):
        """Configure matplotlib settings."""
        if self.backend == VisualizationBackend.MATPLOTLIB:
            plt.rcParams.update({
                'figure.figsize': self.figsize,
                'figure.dpi': self.dpi,
                'font.size': self.font_size,
                'axes.facecolor': self.get_color('background'),
                'axes.edgecolor': self.get_color('grid'),
                'axes.labelcolor': self.get_color('text'),
                'axes.tick_params': {'colors': self.get_color('text')},
                'grid.color': self.get_color('grid'),
                'grid.linestyle': '--',
                'grid.alpha': 0.5,
                'line.color': self.get_color('text'),
                'text.color': self.get_color('text'),
                'legend.facecolor': self.get_color('background'),
                'legend.edgecolor': self.get_color('grid'),
            })


class BrainVisualizer:
    """Brain visualization system."""
    
    def __init__(self, style: PlotStyle = None):
        """
        Initialize brain visualizer.
        
        Args:
            style: Plot style configuration
        """
        self.style = style or PlotStyle()
        self.history = []
        self.current_view = 'activity'
        self.animation_enabled = False
        self.animation_interval = 100  # milliseconds
        
        if BINDINGS_AVAILABLE:
            self.style.configure_matplotlib()
    
    def plot_neural_activity(self, brain, save_path: str = None, show: bool = False):
        """
        Plot neural activity patterns.
        
        Args:
            brain: Brain object
            save_path: Path to save plot (optional)
            show: Whether to display plot
        """
        if not BINDINGS_AVAILABLE:
            print("Warning: Cannot plot without C++ bindings")
            return
        
        fig, axes = plt.subplots(2, 3, figsize=self.style.figsize)
        fig.suptitle('Neural Activity Analysis', fontsize=16, fontweight='bold')
        
        # Total neuron count
        axes[0, 0].bar(['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Internal'],
                      [brain.getTotalNeuronCount() * 0.6, brain.getTotalNeuronCount() * 0.2,
                       brain.getTotalNeuronCount() * 0.1, brain.getTotalNeuronCount() * 0.08,
                       brain.getTotalNeuronCount() * 0.02],
                      color=[self.style.get_color('excitatory'),
                             self.style.get_color('inhibitory'),
                             self.style.get_color('sensory'),
                             self.style.get_color('motor'),
                             self.style.get_color('internal')])
        axes[0, 0].set_title('Neuron Type Distribution')
        axes[0, 0].set_ylabel('Count')
        
        # Spike rate over time (simplified)
        steps = min(50, brain.getTotalNeuronCount() // 10)
        spike_counts = []
        for step in range(steps):
            spike_counts.append(brain.getFiringNeuronCount())
            if step < steps - 1:
                brain.step(step)
        
        axes[0, 1].plot(spike_counts, color=self.style.get_color('excitatory'), linewidth=self.style.line_width)
        axes[0, 1].set_title('Firing Neuron Count (Sample Steps)')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Count')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Activity heatmap (simplified neuron network)
        neuron_counts = brain.getTotalNeuronCount()
        network_size = min(20, int(math.sqrt(neuron_counts)))
        
        if network_size > 1:
            activity_matrix = np.random.random((network_size, network_size)) * 0.5 + 0.5
            im = axes[0, 2].imshow(activity_matrix, cmap='RdYlBu_r', aspect='auto', alpha=self.style.alpha)
            axes[0, 2].set_title('Neural Activity Heatmap')
            plt.colorbar(im, ax=axes[0, 2])
        
        # Synapse statistics
        axes[1, 0].pie([brain.getTotalSynapseCount() * 0.7, brain.getTotalSynapseCount() * 0.3],
                     labels=['Excitatory', 'Inhibitory'],
                     colors=[self.style.get_color('excitatory'), self.style.get_color('inhibitory')],
                     autopct='%1.1f%%')
        axes[1, 0].set_title('Synapse Distribution')
        
        # E/I ratio
        ei_ratio = brain.getExcitationInhibitionRatio()
        axes[1, 1].bar(['Excitation', 'Inhibition'], [ei_ratio, 1 - ei_ratio],
                      color=[self.style.get_color('excitatory'), self.style.get_color('inhibitory')])
        axes[1, 1].set_title(f'Excitatory/Inhibitory Ratio: {ei_ratio:.3f}')
        axes[1, 1].set_ylabel('Ratio')
        axes[1, 1].set_ylim(0, 1)
        
        # Developmental stage
        dev_stage = brain.getDevelopmentalStage()
        axes[1, 2].bar(['Initial', 'Critical', 'Mature', 'Adult', 'Aging'],
                      [0.2, 0.3, 0.4, dev_stage.value * 0.2 + 0.5, 0.1],
                      color=[self.style.get_color('sensory'), self.style.get_color('motor'),
                             self.style.get_color('internal'), self.style.get_color('excitatory'),
                             self.style.get_color('inhibitory')])
        axes[1, 2].set_title('Developmental Stage')
        axes[1, 2].set_ylabel('Progress')
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.style.dpi, bbox_inches='tight')
        
        if show:
            plt.show()
        
        plt.close()
    
    def plot_brain_evolution(self, brain_history: List[Dict[str, Any]], save_path: str = None, show: bool = False):
        """
        Plot brain evolution over time.
        
        Args:
            brain_history: List of brain states over time
            save_path: Path to save plot (optional)
            show: Whether to display plot
        """
        if not brain_history:
            print("Warning: No brain history to plot")
            return
        
        fig, axes = plt.subplots(3, 2, figsize=self.style.figsize)
        fig.suptitle('Brain Evolution Analysis', fontsize=16, fontweight='bold')
        
        # Neuron count evolution
        epochs = [b.get('step', i) for i, b in enumerate(brain_history)]
        neuron_counts = [b.get('neuron_count', 0) for b in brain_history]
        
        axes[0, 0].plot(epochs, neuron_counts, color=self.style.get_color('excitatory'), 
                       linewidth=self.style.line_width, marker='o', markersize=self.style.marker_size)
        axes[0, 0].set_title('Neuron Count Evolution')
        axes[0, 0].set_xlabel('Step')
        axes[0, 0].set_ylabel('Neuron Count')
        axes[0, 0].grid(True, alpha=0.3)
        
        # Synapse evolution
        synapse_counts = [b.get('synapse_count', 0) for b in brain_history]
        axes[0, 1].plot(epochs, synapse_counts, color=self.style.get_color('inhibitory'),
                       linewidth=self.style.line_width, marker='s', markersize=self.style.marker_size)
        axes[0, 1].set_title('Synapse Count Evolution')
        axes[0, 1].set_xlabel('Step')
        axes[0, 1].set_ylabel('Synapse Count')
        axes[0, 1].grid(True, alpha=0.3)
        
        # Firing rate evolution
        firing_counts = [b.get('firing_count', 0) for b in brain_history]
        axes[1, 0].plot(epochs, firing_counts, color=self.style.get_color('sensory'),
                       linewidth=self.style.line_width, linestyle='--', marker='^', markersize=self.style.marker_size)
        axes[1, 0].set_title('Firing Neuron Count Evolution')
        axes[1, 0].set_xlabel('Step')
        axes[1, 0].set_ylabel('Firing Count')
        axes[1, 0].grid(True, alpha=0.3)
        
        # E/I ratio evolution
        ei_ratios = [b.get('ei_ratio', 0.5) for b in brain_history]
        axes[1, 1].plot(epochs, ei_ratios, color=self.style.get_color('motor'),
                       linewidth=self.style.line_width, marker='d', markersize=self.style.marker_size)
        axes[1, 1].axhline(y=1.0, color='gray', linestyle=':', alpha=0.5)
        axes[1, 1].set_title('Excitatory/Inhibitory Ratio Evolution')
        axes[1, 1].set_xlabel('Step')
        axes[1, 1].set_ylabel('E/I Ratio')
        axes[1, 1].set_ylim(0, 1)
        axes[1, 1].grid(True, alpha=0.3)
        
        # Activity patterns
        avg_activities = [b.get('avg_activity', 0.5) for b in brain_history]
        axes[2, 0].fill_between(epochs, avg_activities, alpha=self.style.alpha,
                               color=self.style.get_color('internal'))
        axes[2, 0].set_title('Average Neural Activity')
        axes[2, 0].set_xlabel('Step')
        axes[2, 0].set_ylabel('Activity Level')
        axes[2, 0].set_ylim(0, 1)
        axes[2, 0].grid(True, alpha=0.3)
        
        # Developmental stage progression
        stages = list(DevelopmentalStage)
        stage_values = [stages.index(DevelopmentalStage[b.get('dev_stage', 'initial')]) for b in brain_history]
        axes[2, 1].step(epochs, stage_values, where='post', linewidth=self.style.line_width)
        axes[2, 1].set_title('Developmental Stage Progression')
        axes[2, 1].set_xlabel('Step')
        axes[2, 1].set_ylabel('Stage Index')
        axes[2, 1].set_yticks(range(len(stages)))
        axes[2, 1].set_yticklabels([s.value for s in stages])
        axes[2, 1].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.style.dpi, bbox_inches='tight')
        
        if show:
            plt.show()
        
        plt.close()
    
    def plot_world_simulation(self, world_history: List[Dict[str, Any]], save_path: str = None, show: bool = False):
        """
        Plot world simulation visualization.
        
        Args:
            world_history: List of world states over time
            save_path: Path to save plot (optional)
            show: Whether to display plot
        """
        if not world_history:
            print("Warning: No world history to plot")
            return
        
        fig, axes = plt.subplots(2, 3, figsize=self.style.figsize)
        fig.suptitle('World Simulation Analysis', fontsize=16, fontweight='bold')
        
        # Agent position trajectory
        if world_history and 'agent_x' in world_history[0]:
            agent_x = [w.get('agent_x', 0) for w in world_history]
            agent_y = [w.get('agent_y', 0) for w in world_history]
            
            axes[0, 0].plot(agent_x, agent_y, color=self.style.get_color('excitatory'),
                           linewidth=self.style.line_width, marker='o', markersize=self.style.marker_size)
            axes[0, 0].set_title('Agent Trajectory')
            axes[0, 0].set_xlabel('X Position')
            axes[0, 0].set_ylabel('Y Position')
            axes[0, 0].grid(True, alpha=0.3)
            axes[0, 0].axis('equal')
        
        # Energy levels
        if world_history and 'agent_energy' in world_history[0]:
            energies = [w.get('agent_energy', 100) for w in world_history]
            axes[0, 1].plot(energies, color=self.style.get_color('inhibitory'),
                           linewidth=self.style.line_width, linestyle='--', marker='s', markersize=self.style.marker_size)
            axes[0, 1].set_title('Agent Energy Levels')
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Energy')
            axes[0, 1].set_ylim(0, 100)
            axes[0, 1].grid(True, alpha=0.3)
        
        # Simulation time
        times = [w.get('simulation_time', i * 0.1) for i, w in enumerate(world_history)]
        axes[0, 2].plot(times, times, color=self.style.get_color('sensory'), linewidth=2)
        axes[0, 2].set_title('Simulation Time')
        axes[0, 2].set_xlabel('Step')
        axes[0, 2].set_ylabel('Simulation Time')
        axes[0, 2].grid(True, alpha=0.3)
        
        # Action distribution
        if world_history and 'actions' in world_history[0]:
            all_actions = []
            for w in world_history:
                if 'actions' in w:
                    all_actions.extend(w['actions'])
            
            if all_actions:
                action_counts = {}
                for action in all_actions:
                    action_counts[action] = action_counts.get(action, 0) + 1
                
                axes[1, 0].bar(action_counts.keys(), action_counts.values(),
                              color=self.style.get_color('motor'), alpha=self.style.alpha)
                axes[1, 0].set_title('Action Distribution')
                axes[1, 0].set_xlabel('Action Type')
                axes[1, 0].set_ylabel('Count')
                plt.setp(axes[1, 0].xaxis.get_majorticklabels(), rotation=45)
        
        # World state changes
        if world_history and 'world_objects' in world_history[0]:
            object_counts = [len(w.get('world_objects', [])) for w in world_history]
            axes[1, 1].plot(object_counts, color=self.style.get_color('internal'),
                           linewidth=self.style.line_width, marker='^', markersize=self.style.marker_size)
            axes[1, 1].set_title('World Objects Count')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Object Count')
            axes[1, 1].grid(True, alpha=0.3)
        
        # Sensory input levels
        if world_history and 'sensory_inputs' in world_history[0]:
            sensory_levels = []
            for w in world_history:
                if 'sensory_inputs' in w:
                    level = sum(w['sensory_inputs'].values()) / max(len(w['sensory_inputs']), 1)
                    sensory_levels.append(level)
                else:
                    sensory_levels.append(0.0)
            
            axes[1, 2].plot(sensory_levels, color=self.style.get_color('excitatory'),
                           linewidth=self.style.line_width, alpha=0.7)
            axes[1, 2].set_title('Sensory Input Levels')
            axes[1, 2].set_xlabel('Step')
            axes[1, 2].set_ylabel('Sensory Level')
            axes[1, 2].set_ylim(0, 1)
            axes[1, 2].grid(True, alpha=0.3)
        
        plt.tight_layout()
        
        if save_path:
            plt.savefig(save_path, dpi=self.style.dpi, bbox_inches='tight')
        
        if show:
            plt.show()
        
        plt.close()
    
    def save_visualization_config(self, filepath: str):
        """Save visualization configuration."""
        config = {
            'backend': self.style.backend.value,
            'color_scheme': self.style.color_scheme,
            'figsize': self.style.figsize,
            'dpi': self.style.dpi,
            'font_size': self.style.font_size,
            'line_width': self.style.line_width,
            'marker_size': self.style.marker_size,
            'alpha': self.style.alpha,
        }
        
        import json
        with open(filepath, 'w') as f:
            json.dump(config, f, indent=2)
    
    def load_visualization_config(self, filepath: str):
        """Load visualization configuration."""
        import json
        
        with open(filepath, 'r') as f:
            config = json.load(f)
        
        self.style.backend = VisualizationBackend(config['backend'])
        self.style.color_scheme = config['color_scheme']
        self.style.figsize = tuple(config['figsize'])
        self.style.dpi = config['dpi']
        self.style.font_size = config['font_size']
        self.style.line_width = config['line_width']
        self.style.marker_size = config['marker_size']
        self.style.alpha = config['alpha']
        
        if BINDINGS_AVAILABLE:
            self.style.configure_matplotlib()
    
    def __repr__(self) -> str:
        return f"BrainVisualizer(backend={self.style.backend.value}, view={self.current_view})"


def create_brain_visualizer(style: PlotStyle = None) -> BrainVisualizer:
    """Create brain visualizer."""
    return BrainVisualizer(style)


def create_plot_style(backend: VisualizationBackend = VisualizationBackend.MATPLOTLIB) -> PlotStyle:
    """Create plot style."""
    return PlotStyle(backend)


def save_plot_data(brain_history: List[Dict[str, Any]], world_history: List[Dict[str, Any]],
                  filepath: str = "nlm_visualization_data.json"):
    """Save plot data to file."""
    import json
    
    data = {
        'brain_history': brain_history,
        'world_history': world_history,
        'timestamp': time.time(),
    }
    
    with open(filepath, 'w') as f:
        json.dump(data, f, indent=2)


def load_plot_data(filepath: str = "nlm_visualization_data.json"):
    """Load plot data from file."""
    import json
    
    with open(filepath, 'r') as f:
        return json.load(f)
