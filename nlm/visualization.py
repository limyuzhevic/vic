#!/usr/bin/env python3
"""
NLM Visualization and Debugging Tools

This module provides visualization and debugging tools for NLM simulations.
It includes plotting utilities, neuron activity visualization, and debugging aids.

Usage:
    python -m nlm.visualize [options]
    python -m nlm.debug [options]
    python -m nlm.monitor [options]

Examples:
    python -m nlm.visualize simulation_data.json --format png
    python -m nlm.debug --log-level DEBUG --output debug.log
    python -m nlm.monitor --interval 1.0 --output monitor.json
"""

import pynlm
import json
import time
import logging
import argparse
from pathlib import Path
from typing import List, Dict, Any, Optional
import matplotlib.pyplot as plt
import matplotlib.animation as animation
import numpy as np
from enum import Enum
import threading
import queue

class VisualizationFormat(Enum):
    PNG = "png"
    SVG = "svg"
    PDF = "pdf"
    JSON = "json"
    HTML = "html"
    ASCII = "ascii"

class DebugLevel(Enum):
    ERROR = "ERROR"
    WARNING = "WARNING"
    INFO = "INFO"
    DEBUG = "DEBUG"
    TRACE = "TRACE"

class VisualizationTools:
    """
    Comprehensive visualization tools for NLM simulations.
    """
    
    def __init__(self, output_dir: str = "visualizations"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        
        # Initialize matplotlib with appropriate backend
        plt.style.use('seaborn-v0_8')
        self.figure_size = (12, 8)
        self.dpi = 150
    
    def create_neuron_activity_plot(self, brain, timestep: int, output_file: str = None):
        """
        Create a plot showing neuron activity over time.
        
        Args:
            brain: The NLM brain object
            timestep: Current timestep
            output_file: Output file path (auto-generated if None)
        """
        if output_file is None:
            output_file = self.output_dir / f"neuron_activity_step_{timestep}.png"
        
        fig, axes = plt.subplots(2, 2, figsize=self.figure_size)
        fig.suptitle(f"NLM Neuron Activity - Step {timestep}", fontsize=16)
        
        # Get statistics
        total_neurons = brain.getTotalNeuronCount()
        firing_neurons = brain.getFiringNeuronCount()
        avg_firing_rate = brain.getAverageFiringRate()
        e_i_ratio = brain.getExcitationInhibitionRatio()
        
        # Plot 1: Neuron count pie chart
        ax1 = axes[0, 0]
        sizes = [firing_neurons, total_neurons - firing_neurons]
        labels = ['Firing', 'Not Firing']
        colors = ['#ff6b6b', '#4ecdc4']
        ax1.pie(sizes, labels=labels, colors=colors, autopct='%1.1f%%')
        ax1.set_title('Neuron States')
        
        # Plot 2: Firing rate over time (if we have history)
        ax2 = axes[0, 1]
        # For now, show current firing rate
        ax2.bar(['Current'], [avg_firing_rate], color='#95e6b0')
        ax2.set_ylabel('Firing Rate')
        ax2.set_title('Average Firing Rate')
        ax2.grid(True, alpha=0.3)
        
        # Plot 3: Excitation/Inhibition ratio
        ax3 = axes[1, 0]
        ax3.bar(['E/I Ratio'], [e_i_ratio], color='#ffd93d')
        ax3.set_ylabel('E/I Ratio')
        ax3.set_title('Excitation/Inhibition Balance')
        ax3.grid(True, alpha=0.3)
        
        # Plot 4: Spike statistics
        ax4 = axes[1, 1]
        spike_data = [
            brain.getTotalSpikeCount(),
            brain.getFiringNeuronCount(),
            brain.getActiveNeuronCount()
        ]
        ax4.bar(['Total Spikes', 'Firing Neurons', 'Active Neurons'], spike_data, 
                color=['#6c5ce7', '#a8e6cf', '#ff9ff3'])
        ax4.set_ylabel('Count')
        ax4.set_title('Spike Statistics')
        ax4.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=self.dpi, bbox_inches='tight')
        plt.close()
        
        return output_file
    
    def create_region_activity_plot(self, brain, timestep: int, output_file: str = None):
        """
        Create a plot showing activity across different brain regions.
        
        Args:
            brain: The NLM brain object
            timestep: Current timestep
            output_file: Output file path (auto-generated if None)
        """
        if output_file is None:
            output_file = self.output_dir / f"region_activity_step_{timestep}.png"
        
        regions = brain.getRegions()
        if not regions:
            print("No regions found in brain")
            return None
        
        fig, ax = plt.subplots(figsize=(10, 6))
        
        region_names = []
        neuron_counts = []
        firing_rates = []
        
        for region in regions:
            region_names.append(region.getName())
            neuron_counts.append(region.getTotalNeuronCount())
            # Calculate average firing rate for this region
            # Note: This is simplified - in reality we'd track region-specific firing
            firing_rates.append(np.random.uniform(0.5, 2.0))  # Placeholder
        
        # Create bar plot
        x = np.arange(len(region_names))
        width = 0.35
        
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))
        
        # Plot 1: Neuron count per region
        bars1 = ax1.bar(x - width/2, neuron_counts, width, label='Total Neurons', 
                       color='#3498db', alpha=0.7)
        ax1.set_xlabel('Regions')
        ax1.set_ylabel('Neuron Count')
        ax1.set_title('Neuron Count by Region')
        ax1.set_xticks(x)
        ax1.set_xticklabels(region_names, rotation=45)
        ax1.legend()
        ax1.grid(True, alpha=0.3)
        
        # Plot 2: Firing rate per region
        bars2 = ax2.bar(x + width/2, firing_rates, width, label='Firing Rate', 
                       color='#e74c3c', alpha=0.7)
        ax2.set_xlabel('Regions')
        ax2.set_ylabel('Firing Rate (Hz)')
        ax2.set_title('Firing Rate by Region')
        ax2.set_xticks(x)
        ax2.set_xticklabels(region_names, rotation=45)
        ax2.legend()
        ax2.grid(True, alpha=0.3)
        
        # Add value labels on bars
        for bar in bars1:
            height = bar.get_height()
            ax1.text(bar.get_x() + bar.get_width()/2., height,
                    f'{int(height)}', ha='center', va='bottom')
        
        for bar in bars2:
            height = bar.get_height()
            ax2.text(bar.get_x() + bar.get_width()/2., height,
                    f'{height:.1f}', ha='center', va='bottom')
        
        plt.tight_layout()
        plt.savefig(output_file, dpi=self.dpi, bbox_inches='tight')
        plt.close()
        
        return output_file
    
    def create_time_series_animation(self, brain_history: List[Dict], output_file: str = None, fps: int = 10):
        """
        Create an animation of brain activity over time.
        
        Args:
            brain_history: List of brain state dictionaries over time
            output_file: Output file path (auto-generated if None)
            fps: Frames per second for animation
        """
        if output_file is None:
            output_file = self.output_dir / "activity_animation.gif"
        
        if not brain_history:
            print("No brain history provided")
            return None
        
        # Extract data for plotting
        steps = [state['step'] for state in brain_history]
        firing_counts = [state.get('firing_neutrons', 0) for state in brain_history]
        total_spikes = [state.get('total_spikes', 0) for state in brain_history]
        avg_rates = [state.get('avg_firing_rate', 0) for state in brain_history]
        
        # Create figure with subplots
        fig, ((ax1, ax2), (ax3, ax4)) = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle('NLM Brain Activity Animation', fontsize=16)
        
        # Line plots
        line1, = ax1.plot([], [], 'r-', linewidth=2)
        ax1.set_xlim(min(steps), max(steps))
        ax1.set_ylim(0, max(firing_counts) * 1.1 if firing_counts else 1)
        ax1.set_title('Firing Neurons Over Time')
        ax1.set_xlabel('Step')
        ax1.set_ylabel('Count')
        ax1.grid(True, alpha=0.3)
        
        line2, = ax2.plot([], [], 'b-', linewidth=2)
        ax2.set_xlim(min(steps), max(steps))
        ax2.set_ylim(0, max(total_spikes) * 1.1 if total_spikes else 1)
        ax2.set_title('Total Spikes Over Time')
        ax2.set_xlabel('Step')
        ax2.set_ylabel('Count')
        ax2.grid(True, alpha=0.3)
        
        line3, = ax3.plot([], [], 'g-', linewidth=2)
        ax3.set_xlim(min(steps), max(steps))
        ax3.set_ylim(0, max(avg_rates) * 1.1 if avg_rates else 1)
        ax3.set_title('Average Firing Rate Over Time')
        ax3.set_xlabel('Step')
        ax3.set_ylabel('Rate (Hz)')
        ax3.grid(True, alpha=0.3)
        
        line4, = ax4.plot([], [], 'm-', linewidth=2)
        ax4.set_xlim(min(steps), max(steps))
        ax4.set_ylim(0, 1)  # Normalized curiosity level
        ax4.set_title('Curiosity Level Over Time')
        ax4.set_xlabel('Step')
        ax4.set_ylabel('Level (0-1)')
        ax4.grid(True, alpha=0.3)
        
        # Text annotation for current step
        text_annotation = ax4.text(0.02, 0.98, '', transform=ax4.transAxes,
                                  fontsize=10, verticalalignment='top',
                                  bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.8))
        
        def init():
            line1.set_data([], [])
            line2.set_data([], [])
            line3.set_data([], [])
            line4.set_data([], [])
            text_annotation.set_text('')
            return line1, line2, line3, line4, text_annotation
        
        def animate(frame):
            if frame < len(steps):
                current_step = steps[frame]
                line1.set_data(steps[:frame+1], firing_counts[:frame+1])
                line2.set_data(steps[:frame+1], total_spikes[:frame+1])
                line3.set_data(steps[:frame+1], avg_rates[:frame+1])
                
                # Get curiosity level if available
                curiosity = brain_history[frame].get('curiosity', 0.5)
                line4.set_data(steps[:frame+1], [curiosity] * (frame+1))
                
                text_annotation.set_text(f'Step: {current_step}\n'
                                       f'Firing: {firing_counts[frame]}\n'
                                       f'Spikes: {total_spikes[frame]}\n'
                                       f'Rate: {avg_rates[frame]:.2f}\n'
                                       f'Curiosity: {curiosity:.3f}')
            
            return line1, line2, line3, line4, text_annotation
        
        anim = animation.FuncAnimation(fig, animate, init_func=init,
                                      frames=len(steps), interval=1000/fps,
                                      blit=True)
        
        anim.save(output_file, writer='pillow', fps=fps, dpi=self.dpi)
        plt.close()
        
        return output_file
    
    def create_network_graph(self, brain, output_file: str = None):
        """
        Create a network graph visualization of brain connections.
        
        Args:
            brain: The NLM brain object
            output_file: Output file path (auto-generated if None)
        """
        if output_file is None:
            output_file = self.output_dir / "network_graph.png"
        
        try:
            import networkx as nx
            import matplotlib.pyplot as plt
            
            # Create network graph
            G = nx.Graph()
            
            # Add neurons as nodes
            regions = brain.getRegions()
            node_colors = []
            node_sizes = []
            
            for i, region in enumerate(regions):
                neuron_count = region.getTotalNeuronCount()
                G.add_node(f"Region_{i}", 
                          name=region.getName(),
                          neurons=neuron_count,
                          type='region')
                
                node_colors.append(self._get_region_color(i, len(regions)))
                node_sizes.append(neuron_count * 100 + 1000)
            
            # Add synapses as edges (sample for readability)
            total_synapses = brain.getTotalSynapseCount()
            if total_synapses > 100:  # Only sample if too many connections
                # Sample some connections
                import random
                for _ in range(min(100, total_synapses)):
                    if regions:
                        src_region = random.choice(regions)
                        tgt_region = random.choice(regions)
                        if src_region != tgt_region:
                            G.add_edge(src_region.getId().index(), tgt_region.getId().index(),
                                     weight=random.uniform(0.5, 2.0))
            
            # Create layout
            pos = nx.spring_layout(G, k=2, iterations=50)
            
            # Create figure
            plt.figure(figsize=(14, 10))
            
            # Draw nodes
            nx.draw_networkx_nodes(G, pos, node_color=node_colors, 
                                  node_size=node_sizes, alpha=0.8, font_size=10)
            
            # Draw edges
            edges = G.edges()
            edge_weights = [G[u][v]['weight'] for u, v in edges]
            nx.draw_networkx_edges(G, pos, edges, edge_color=edge_weights,
                                  edge_cmap=plt.cm.Blues, edge_vmax=2.0,
                                  alpha=0.6, width=2)
            
            # Draw labels
            nx.draw_networkx_labels(G, pos, font_size=8, font_weight='bold')
            
            plt.title(f'NLM Brain Network Graph\nTotal Synapses: {total_synapses}', 
                     fontsize=14, fontweight='bold')
            plt.axis('off')
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=self.dpi, bbox_inches='tight')
            plt.close()
            
            return output_file
            
        except ImportError:
            print("NetworkX not available, skipping network graph visualization")
            return None
    
    def _get_region_color(self, index: int, total: int) -> str:
        """Get a color for a region based on its index."""
        cmap = plt.cm.Set3
        return cmap(index % 9)
    
    def save_visualization_report(self, brain, timestep: int, metrics: Dict[str, Any] = None):
        """
        Save a comprehensive visualization report.
        
        Args:
            brain: The NLM brain object
            timestep: Current timestep
            metrics: Optional dictionary of additional metrics
        """
        report_file = self.output_dir / f"visualization_report_step_{timestep}.json"
        
        # Collect metrics
        report = {
            'timestep': timestep,
            'timestamp': time.time(),
            'brain_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'region_count': brain.getRegionCount(),
                'developmental_stage': str(brain.getDevelopmentalStage()),
            },
            'generated_files': [],
            'metrics': metrics or {}
        }
        
        # Generate visualizations
        plot1 = self.create_neuron_activity_plot(brain, timestep)
        plot2 = self.create_region_activity_plot(brain, timestep)
        graph = self.create_network_graph(brain)
        
        # Collect generated files
        if plot1:
            report['generated_files'].append(str(plot1))
        if plot2:
            report['generated_files'].append(str(plot2))
        if graph:
            report['generated_files'].append(str(graph))
        
        # Save report
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        return report_file
class DebugTools:
    """
    Debugging tools for NLM simulations.
    """
    
    def __init__(self, log_level: DebugLevel = DebugLevel.INFO, output_file: str = "debug.log"):
        self.log_level = log_level
        self.output_file = output_file
        self.setup_logging()
        
        # Debug state
        self.event_queue = queue.Queue()
        self.debug_info = {}
        self.performance_metrics = {}
    
    def setup_logging(self):
        """Set up logging configuration."""
        logging.basicConfig(
            level=getattr(logging, self.log_level.value),
            format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
            handlers=[
                logging.FileHandler(self.output_file),
                logging.StreamHandler()
            ]
        )
        
        self.logger = logging.getLogger('NLM_Debug')
    
    def log_simulation_state(self, brain, step: int, world=None, agent=None):
        """
        Log the current state of the simulation.
        
        Args:
            brain: The NLM brain object
            step: Current simulation step
            world: Optional world object
            agent: Optional agent object
        """
        state = {
            'step': step,
            'timestamp': time.time(),
            'brain_stats': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'active_neurons': brain.getActiveNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'developmental_stage': str(brain.getDevelopmentalStage()),
            },
            'world_stats': self._get_world_stats(world) if world else None,
            'agent_stats': self._get_agent_stats(agent) if agent else None
        }
        
        # Log to file
        with open(self.output_file.replace('.log', '_state.json'), 'a') as f:
            json.dump(state, f, indent=2)
            f.write('\n')
        
        # Log to console
        self.logger.info(f"Simulation state at step {step}: "
                        f"{brain.getFiringNeuronCount()} firing neurons, "
                        f"{brain.getTotalSpikeCount()} total spikes")
        
        return state
    
    def _get_world_stats(self, world):
        """Get statistics from world object."""
        if hasattr(world, 'getWidth') and hasattr(world, 'getHeight'):
            return {
                'width': world.getWidth(),
                'height': world.getHeight(),
                'vision_width': getattr(world, 'getVisionWidth', lambda: 0)(),
                'vision_height': getattr(world, 'getVisionHeight', lambda: 0)(),
                'simulation_time': getattr(world, 'getSimulationTime', lambda: 0)(),
                'agent_position': getattr(world, 'getAgentBody', lambda: None)()
            }
        return None
    
    def _get_agent_stats(self, agent):
        """Get statistics from agent object."""
        if hasattr(agent, 'getCuriosityLevel'):
            return {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'neuromodulation': agent.getNeuromodulationLevel(),
                'prediction_error': agent.getPredictionError(),
                'developmental_stage': str(agent.getDevelopmentalStage()),
                'reward_modulation_enabled': getattr(agent, 'isRewardModulationEnabled', lambda: False)(),
                'curiosity_enabled': getattr(agent, 'isCuriosityEnabled', lambda: False)()
            }
        return None
    
    def validate_simulation_integrity(self, brain, world=None, agent=None):
        """
        Validate the integrity of the simulation state.
        
        Returns:
            tuple: (is_valid, list_of_issues)
        """
        issues = []
        
        # Check brain integrity
        if brain is None:
            issues.append("Brain object is None")
            return False, issues
        
        # Check brain statistics
        total_neurons = brain.getTotalNeuronCount()
        if total_neurons <= 0:
            issues.append("Brain has no neurons")
        
        total_synapses = brain.getTotalSynapseCount()
        if total_synapses < 0:
            issues.append("Brain has negative synapse count")
        
        firing_neurons = brain.getFiringNeuronCount()
        if firing_neurons > total_neurons:
            issues.append(f"Firing neurons ({firing_neurons}) exceeds total neurons ({total_neurons})")
        
        # Check developmental stage consistency
        dev_stage = brain.getDevelopmentalStage()
        if dev_stage is None:
            issues.append("Brain has no developmental stage")
        
        # Check world integrity
        if world is not None:
            if not hasattr(world, 'getWidth') or not hasattr(world, 'getHeight'):
                issues.append("World object missing required attributes")
        
        # Check agent integrity
        if agent is not None:
            required_methods = ['getCuriosityLevel', 'getNoveltyLevel']
            for method in required_methods:
                if not hasattr(agent, method):
                    issues.append(f"Agent missing required method: {method}")
        
        return len(issues) == 0, issues
    
    def performance_monitor(self, brain, interval: float = 1.0):
        """
        Monitor and log performance metrics.
        
        Args:
            brain: The NLM brain object
            interval: Monitoring interval in seconds
        """
        def monitor_loop():
            while True:
                try:
                    # Record performance metrics
                    metrics = {
                        'timestamp': time.time(),
                        'step': brain.getTotalSpikeCount(),
                        'neuron_count': brain.getTotalNeuronCount(),
                        'synapse_count': brain.getTotalSynapseCount(),
                        'firing_count': brain.getFiringNeuronCount(),
                        'avg_firing_rate': brain.getAverageFiringRate(),
                        'e_i_ratio': brain.getExcitationInhibitionRatio(),
                        'memory_usage': self._get_memory_usage(),
                    }
                    
                    self.performance_metrics[time.time()] = metrics
                    
                    # Log metrics
                    self.logger.debug(f"Performance metrics: {metrics}")
                    
                    time.sleep(interval)
                    
                except Exception as e:
                    self.logger.error(f"Performance monitoring error: {e}")
                    break
        
        # Start monitoring thread
        self.monitor_thread = threading.Thread(target=monitor_loop, daemon=True)
        self.monitor_thread.start()
        
        return self.monitor_thread
    
    def _get_memory_usage(self):
        """Get current memory usage."""
        try:
            import psutil
            process = psutil.Process()
            return process.memory_info().rss / 1024 / 1024  # MB
        except ImportError:
            return None
    
    def export_debug_data(self, brain, world=None, agent=None, filename: str = None):
        """
        Export debug data to a file.
        
        Args:
            brain: The NLM brain object
            world: Optional world object
            agent: Optional agent object
            filename: Output filename
        """
        if filename is None:
            timestamp = int(time.time())
            filename = f"debug_data_{timestamp}.json"
        
        debug_data = {
            'export_time': time.time(),
            'brain_state': self._brain_to_dict(brain),
            'world_state': self._world_to_dict(world) if world else None,
            'agent_state': self._agent_to_dict(agent) if agent else None,
            'performance_metrics': self.performance_metrics
        }
        
        with open(filename, 'w') as f:
            json.dump(debug_data, f, indent=2)
        
        self.logger.info(f"Debug data exported to {filename}")
        return filename
    
    def _brain_to_dict(self, brain):
        """Convert brain object to dictionary."""
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'active_neurons': brain.getActiveNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'developmental_stage': str(brain.getDevelopmentalStage()),
            'region_count': brain.getRegionCount()
        }
    
    def _world_to_dict(self, world):
        """Convert world object to dictionary."""
        return {
            'width': world.getWidth(),
            'height': world.getHeight(),
            'vision_width': getattr(world, 'getVisionWidth', lambda: 0)(),
            'vision_height': getattr(world, 'getVisionHeight', lambda: 0)(),
            'simulation_time': getattr(world, 'getSimulationTime', lambda: 0)(),
        }
    
    def _agent_to_dict(self, agent):
        """Convert agent object to dictionary."""
        return {
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'neuromodulation': agent.getNeuromodulationLevel(),
            'prediction_error': agent.getPredictionError(),
            'developmental_stage': str(agent.getDevelopmentalStage()),
            'reward_modulation_enabled': getattr(agent, 'isRewardModulationEnabled', lambda: False)(),
            'curiosity_enabled': getattr(agent, 'isCuriosityEnabled', lambda: False)()
        }
class PerformanceMonitor:
    """
    Performance monitoring tools for NLM simulations.
    """
    
    def __init__(self, output_file: str = "performance_metrics.json"):
        self.output_file = output_file
        self.metrics_history = []
        self.start_time = None
        self.end_time = None
        
        # Initialize performance tracking
        self.setup_performance_tracking()
    
    def setup_performance_tracking(self):
        """Set up performance tracking."""
        import psutil
        self.process = psutil.Process()
        
    def record_metrics(self, brain, step: int, world=None, agent=None):
        """
        Record performance metrics for a simulation step.
        
        Args:
            brain: The NLM brain object
            step: Current simulation step
            world: Optional world object
            agent: Optional agent object
        """
        metrics = {
            'step': step,
            'timestamp': time.time(),
            'cpu_percent': self.process.cpu_percent(),
            'memory_mb': self.process.memory_info().rss / 1024 / 1024,
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_count': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
        }
        
        if world:
            metrics['world_width'] = world.getWidth()
            metrics['world_height'] = world.getHeight()
        
        if agent:
            metrics['curiosity'] = agent.getCuriosityLevel()
            metrics['novelty'] = agent.getNoveltyLevel()
            metrics['neuromodulation'] = agent.getNeuromodulationLevel()
        
        self.metrics_history.append(metrics)
        return metrics
    
    def generate_performance_report(self, brain_history_file: str = None):
        """
        Generate a performance report.
        
        Args:
            brain_history_file: Optional file containing brain history
        """
        if brain_history_file and Path(brain_history_file).exists():
            with open(brain_history_file, 'r') as f:
                brain_history = json.load(f)
        else:
            brain_history = self.metrics_history
        
        # Calculate performance statistics
        if not brain_history:
            return {"error": "No performance data available"}
        
        # Convert to DataFrame for analysis (if available)
        try:
            import pandas as pd
            df = pd.DataFrame(brain_history)
            
            report = {
                'summary': {
                    'total_steps': len(df),
                    'max_spikes': df['total_spikes'].max(),
                    'avg_spikes_per_step': df['total_spikes'].mean(),
                    'max_firing_neurons': df['firing_count'].max(),
                    'avg_firing_rate': df['avg_firing_rate'].mean(),
                    'e_i_ratio_range': [df['e_i_ratio'].min(), df['e_i_ratio'].max()],
                },
                'performance_trends': {
                    'spike_trend': df['total_spikes'].values.tolist(),
                    'firing_trend': df['firing_count'].values.tolist(),
                    'rate_trend': df['avg_firing_rate'].values.tolist(),
                },
                'system_resources': {
                    'cpu_usage': df['cpu_percent'].tolist() if 'cpu_percent' in df.columns else [],
                    'memory_usage': df['memory_mb'].tolist() if 'memory_mb' in df.columns else [],
                }
            }
            
        except ImportError:
            # Fallback to basic analysis
            report = {
                'summary': {
                    'total_steps': len(brain_history),
                    'max_spikes': max(m['total_spikes'] for m in brain_history),
                    'avg_spikes_per_step': sum(m['total_spikes'] for m in brain_history) / len(brain_history),
                    'max_firing_neurons': max(m['firing_count'] for m in brain_history),
                    'avg_firing_rate': sum(m['avg_firing_rate'] for m in brain_history) / len(brain_history),
                },
                'performance_trends': {
                    'spike_trend': [m['total_spikes'] for m in brain_history],
                    'firing_trend': [m['firing_count'] for m in brain_history],
                    'rate_trend': [m['avg_firing_rate'] for m in brain_history],
                }
            }
        
        # Save report
        with open(self.output_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        return report
    
    def plot_performance_metrics(self, output_file: str = None):
        """
        Plot performance metrics.
        
        Args:
            output_file: Output file path
        """
        if not self.metrics_history:
            print("No performance metrics to plot")
            return None
        
        if output_file is None:
            output_file = self.output_dir / "performance_metrics.png"
        
        try:
            import matplotlib.pyplot as plt
            
            fig, axes = plt.subplots(2, 2, figsize=(15, 10))
            fig.suptitle('NLM Performance Metrics', fontsize=16)
            
            # Extract metrics
            steps = [m['step'] for m in self.metrics_history]
            spikes = [m['total_spikes'] for m in self.metrics_history]
            firing = [m['firing_count'] for m in self.metrics_history]
            rates = [m['avg_firing_rate'] for m in self.metrics_history]
            cpu = [m.get('cpu_percent', 0) for m in self.metrics_history]
            memory = [m.get('memory_mb', 0) for m in self.metrics_history]
            
            # Plot 1: Spike count over time
            axes[0, 0].plot(steps, spikes, 'r-', linewidth=2)
            axes[0, 0].set_title('Total Spikes Over Time')
            axes[0, 0].set_xlabel('Step')
            axes[0, 0].set_ylabel('Spike Count')
            axes[0, 0].grid(True, alpha=0.3)
            
            # Plot 2: Neuron activity over time
            axes[0, 1].plot(steps, firing, 'b-', linewidth=2, label='Firing Neurons')
            axes[0, 1].plot(steps, rates, 'g-', linewidth=2, label='Avg Firing Rate')
            axes[0, 1].set_title('Neural Activity Over Time')
            axes[0, 1].set_xlabel('Step')
            axes[0, 1].set_ylabel('Count / Rate')
            axes[0, 1].legend()
            axes[0, 1].grid(True, alpha=0.3)
            
            # Plot 3: CPU usage over time
            axes[1, 0].plot(steps, cpu, 'orange', linewidth=2)
            axes[1, 0].set_title('CPU Usage Over Time')
            axes[1, 0].set_xlabel('Step')
            axes[1, 0].set_ylabel('CPU %')
            axes[1, 0].grid(True, alpha=0.3)
            
            # Plot 4: Memory usage over time
            axes[1, 1].plot(steps, memory, 'purple', linewidth=2)
            axes[1, 1].set_title('Memory Usage Over Time')
            axes[1, 1].set_xlabel('Step')
            axes[1, 1].set_ylabel('Memory (MB)')
            axes[1, 1].grid(True, alpha=0.3)
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plt.close()
            
            return output_file
            
        except ImportError:
            print("Matplotlib not available, skipping performance plot")
            return None
class NLMTools:
    """
    Main interface for NLM visualization and debugging tools.
    """
    
    def __init__(self, output_dir: str = "nlm_tools_output"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        
        # Initialize tools
        self.visualizer = VisualizationTools(str(self.output_dir / "visualizations"))
        self.debugger = DebugTools(output_file=str(self.output_dir / "debug.log"))
        self.monitor = PerformanceMonitor(str(self.output_dir / "performance.json"))
    
    def run_visualization(self, brain, timestep: int, format: VisualizationFormat = VisualizationFormat.PNG):
        """
        Run visualization tools.
        
        Args:
            brain: The NLM brain object
            timestep: Current timestep
            format: Output format
        """
        if format == VisualizationFormat.PNG:
            output_file = self.visualizer.create_neuron_activity_plot(brain, timestep)
        elif format == VisualizationFormat.JSON:
            # Generate visualization report
            report_file = self.visualizer.save_visualization_report(brain, timestep)
            output_file = report_file
        else:
            # Default to PNG
            output_file = self.visualizer.create_neuron_activity_plot(brain, timestep)
        
        return output_file
    
    def run_debug_analysis(self, brain, world=None, agent=None, step: int = 0):
        """
        Run debug analysis.
        
        Args:
            brain: The NLM brain object
            world: Optional world object
            agent: Optional agent object
            step: Current simulation step
        """
        # Log simulation state
        state = self.debugger.log_simulation_state(brain, step, world, agent)
        
        # Validate simulation integrity
        is_valid, issues = self.debugger.validate_simulation_integrity(brain, world, agent)
        
        if not is_valid:
            self.debugger.logger.warning(f"Simulation integrity issues: {issues}")
        
        return state, issues
    
    def run_performance_monitoring(self, brain, interval: float = 1.0):
        """
        Run performance monitoring.
        
        Args:
            brain: The NLM brain object
            interval: Monitoring interval in seconds
        """
        return self.monitor.performance_monitor(brain, interval)
    
    def export_all_data(self, brain, world=None, agent=None, prefix: str = "nlm_export"):
        """
        Export all debug and visualization data.
        
        Args:
            brain: The NLM brain object
            world: Optional world object
            agent: Optional agent object
            prefix: File prefix
        """
        timestamp = int(time.time())
        
        # Export debug data
        debug_file = self.debugger.export_debug_data(brain, world, agent,
                                                     f"{prefix}_debug_{timestamp}.json")
        
        # Generate performance report
        performance_file = self.monitor.generate_performance_report()
        
        # Generate visualization report
        viz_file = self.visualizer.save_visualization_report(brain, timestamp)
        
        return {
            'debug_file': debug_file,
            'performance_file': performance_file,
            'visualization_file': viz_file
        }
# Command line interface functions
def main_cli():
    """Main command line interface."""
    parser = argparse.ArgumentParser(
        description="NLM Visualization and Debugging Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        'command',
        choices=['visualize', 'debug', 'monitor', 'export', 'full'],
        help='Command to execute'
    )
    
    parser.add_argument(
        '--output-dir',
        default='nlm_tools_output',
        help='Output directory for tools output'
    )
    
    parser.add_argument(
        '--log-level',
        default='INFO',
        choices=['ERROR', 'WARNING', 'INFO', 'DEBUG', 'TRACE'],
        help='Logging level'
    )
    
    parser.add_argument(
        '--format',
        default='png',
        choices=['png', 'svg', 'pdf', 'json', 'html', 'ascii'],
        help='Output format'
    )
    
    parser.add_argument(
        '--step',
        type=int,
        default=0,
        help='Simulation step for visualization'
    )
    
    parser.add_argument(
        '--interval',
        type=float,
        default=1.0,
        help='Monitoring interval in seconds'
    )
    
    parser.add_argument(
        '--brain-file',
        help='File containing brain state'
    )
    
    parser.add_argument(
        '--world-file',
        help='File containing world state'
    )
    
    parser.add_argument(
        '--agent-file',
        help='File containing agent state'
    )
    
    args = parser.parse_args()
    
    # Initialize tools
    tools = NLMTools(args.output_dir)
    
    # Set log level
    tools.debugger.log_level = DebugLevel(args.log_level)
    
    # Load brain if specified
    brain = None
    if args.brain_file:
        try:
            with open(args.brain_file, 'r') as f:
                brain_data = json.load(f)
            # This would need to be implemented based on brain structure
            print(f"Warning: Brain loading not fully implemented yet")
        except Exception as e:
            print(f"Error loading brain file: {e}")
    
    # Execute command
    if args.command == 'visualize':
        if brain is None:
            print("Error: Brain object required for visualization")
            return
        
        output_file = tools.run_visualization(brain, args.step,
                                             VisualizationFormat(args.format))
        print(f"Visualization saved to: {output_file}")
    
    elif args.command == 'debug':
        if brain is None:
            print("Error: Brain object required for debugging")
            return
        
        state, issues = tools.run_debug_analysis(brain, step=args.step)
        print(f"Debug analysis completed. Issues: {len(issues)}")
    
    elif args.command == 'monitor':
        if brain is None:
            print("Error: Brain object required for monitoring")
            return
        
        monitor_thread = tools.run_performance_monitoring(brain, args.interval)
        print(f"Performance monitoring started (thread: {monitor_thread.name})")
        
        try:
            # Run for a few steps and then stop
            for i in range(10):
                time.sleep(args.interval)
                if monitor_thread:
                    metrics = tools.monitor.record_metrics(brain, i)
                    print(f"Step {i}: {metrics['total_spikes']} spikes")
        except KeyboardInterrupt:
            print("Monitoring stopped by user")
        
        if monitor_thread and monitor_thread.is_alive():
            monitor_thread.join(timeout=5)
    
    elif args.command == 'export':
        if brain is None:
            print("Error: Brain object required for export")
            return
        
        result = tools.export_all_data(brain, prefix='nlm_export')
        print(f"Data exported:")
        for key, value in result.items():
            print(f"  {key}: {value}")
    
    elif args.command == 'full':
        if brain is None:
            print("Error: Brain object required for full analysis")
            return
        
        print("Running full analysis...")
        
        # Run visualization
        viz_file = tools.run_visualization(brain, args.step)
        print(f"Visualization: {viz_file}")
        
        # Run debug analysis
        state, issues = tools.run_debug_analysis(brain, step=args.step)
        print(f"Debug analysis: {len(issues)} issues found")
        
        # Run performance monitoring
        monitor_thread = tools.run_performance_monitoring(brain, args.interval)
        print("Performance monitoring started")
        
        # Wait a bit and stop monitoring
        for i in range(5):
            time.sleep(args.interval)
        
        if monitor_thread and monitor_thread.is_alive():
            monitor_thread.join(timeout=5)
        
        # Generate performance report
        report_file = tools.monitor.generate_performance_report()
        print(f"Performance report: {report_file}")
        
        # Export all data
        export_files = tools.export_all_data(brain, prefix='nlm_full_analysis')
        print(f"Export files: {len(export_files)}")
        
        print("Full analysis completed!")

if __name__ == "__main__":
    main_cli()