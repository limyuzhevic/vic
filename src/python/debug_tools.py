"""
Basic Visualization and Debugging Tools for NLM

This module provides essential visualization and debugging utilities for NLM (Neural Learning Machine)
simulations, including:

1. Simple graph plotting for brain state
2. Step-by-step simulation debugging
3. Neural activity visualization
4. Configuration validation tools
5. Performance profiling utilities
"""

import pynlm
import numpy as np
import matplotlib.pyplot as plt
import matplotlib
matplotlib.use('Agg')  # Non-interactive backend
import json
import time
import logging
from typing import List, Dict, Optional, Tuple, Any
from dataclasses import dataclass
from enum import Enum

logger = logging.getLogger(__name__)

class VisualizationFormat(Enum):
    PNG = "png"
    SVG = "svg"
    JSON = "json"
    HTML = "html"

@dataclass
class SimulationSnapshot:
    """Snapshot of brain state at a specific time."""
    step: int
    time: float
    firing_neurons: int
    total_spikes: int
    average_firing_rate: float
    e_i_ratio: float
    neuronal_activity: List[float]
    synaptic_weights: List[float]
    region_stats: Dict[str, Dict[str, Any]]

class NLMBrainVisualizer:
    """Basic visualization tools for NLM brain state."""
    
    def __init__(self, figsize: Tuple[int, int] = (12, 8), dpi: int = 100):
        self.figsize = figsize
        self.dpi = dpi
        self.snapshots: List[SimulationSnapshot] = []
        self.colors = {
            'excitatory': '#FF6B6B',
            'inhibitory': '#4ECDC4',
            'sensory': '#45B7D1',
            'motor': '#96CEB4',
            'internal': '#FFEAA7',
            'active': '#FF9F43',
            'background': '#FFFFFF'
        }
    
    def capture_brain_state(self, brain: pynlm.Brain, step: int, time: float) -> SimulationSnapshot:
        """Capture current brain state as a snapshot."""
        neuron_activity = self._get_neuron_activity(brain)
        synaptic_weights = self._get_synaptic_weights(brain)
        region_stats = self._get_region_stats(brain)
        
        return SimulationSnapshot(
            step=step,
            time=time,
            firing_neurons=brain.getFiringNeuronCount(),
            total_spikes=brain.getTotalSpikeCount(),
            average_firing_rate=brain.getAverageFiringRate(),
            e_i_ratio=brain.getExcitationInhibitionRatio(),
            neuronal_activity=neuron_activity,
            synaptic_weights=synaptic_weights,
            region_stats=region_stats
        )
    
    def _get_neuron_activity(self, brain: pynlm.Brain) -> List[float]:
        """Get activity levels for all neurons."""
        activity = []
        for region in brain.getRegions():
            for pop in region.getPopulations():
                for neuron in pop.getNeurons():
                    # Simple activity measure based on membrane potential deviation
                    state = neuron.getState()
                    activity_level = abs(state.membranePotential - state.restingPotential) / 100.0
                    activity.append(min(activity_level, 1.0))
        return activity
    
    def _get_synaptic_weights(self, brain: pynlm.Brain) -> List[float]:
        """Get synaptic weights across the network."""
        weights = []
        for region in brain.getRegions():
            for synapse in region.getSynapses():
                weights.append(synapse.getWeight())
        return weights
    
    def _get_region_stats(self, brain: pynlm.Brain) -> Dict[str, Dict[str, Any]]:
        """Get statistics for each brain region."""
        stats = {}
        for region in brain.getRegions():
            neurons = region.getTotalNeuronCount()
            synapses = region.getSynapseCount()
            avg_weight = 0.0
            if synapses > 0:
                # Calculate average synaptic weight
                total_weight = 0.0
                for synapse in region.getSynapses():
                    total_weight += abs(synapse.getWeight())
                avg_weight = total_weight / synapses
            
            stats[str(region.getId())] = {
                'name': region.getName(),
                'neuron_count': neurons,
                'synapse_count': synapses,
                'average_weight': avg_weight,
                'neuron_types': self._count_neuron_types(region)
            }
        return stats
    
    def _count_neuron_types(self, region: nlm.NeuralRegion) -> Dict[str, int]:
        """Count neuron types in a region."""
        counts = {'excitatory': 0, 'inhibitory': 0, 'sensory': 0, 
                 'motor': 0, 'internal': 0}
        
        for pop in region.getPopulations():
            ntype = pop.getNeuronType()
            type_name = type(ntype).name.lower()
            if type_name in counts:
                counts[type_name] += pop.getNeuronCount()
        
        return counts
    
    def create_activity_plot(self, snapshots: List[SimulationSnapshot], 
                           output_file: str, format: VisualizationFormat = VisualizationFormat.PNG):
        """Create a plot showing neural activity over time."""
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        fig.suptitle('NLM Brain Activity Visualization', fontsize=16, fontweight='bold')
        
        steps = [s.step for s in snapshots]
        
        # Plot 1: Firing neurons over time
        ax = axes[0, 0]
        ax.plot(steps, [s.firing_neurons for s in snapshots], 
               color=self.colors['active'], linewidth=2, marker='o', markersize=3)
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Number of Firing Neurons')
        ax.set_title('Active Neuron Count')
        ax.grid(True, alpha=0.3)
        ax.set_ylim(bottom=0)
        
        # Plot 2: Total spikes over time
        ax = axes[0, 1]
        ax.plot(steps, [s.total_spikes for s in snapshots], 
               color=self.colors['excitatory'], linewidth=2)
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Total Spike Count')
        ax.set_title('Cumulative Spikes')
        ax.grid(True, alpha=0.3)
        
        # Plot 3: Average firing rate over time
        ax = axes[1, 0]
        ax.plot(steps, [s.average_firing_rate for s in snapshots], 
               color=self.colors['inhibitory'], linewidth=2)
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('Average Firing Rate (Hz)')
        ax.set_title('Network Firing Rate')
        ax.grid(True, alpha=0.3)
        ax.set_ylim(bottom=0)
        
        # Plot 4: E/I ratio over time
        ax = axes[1, 1]
        ei_ratios = [s.e_i_ratio if s.e_i_ratio != np.inf else 100 for s in snapshots]
        ax.plot(steps, ei_ratios, 
               color=self.colors['sensory'], linewidth=2)
        ax.set_xlabel('Simulation Step')
        ax.set_ylabel('E/I Ratio')
        ax.set_title('Excitation/Inhibition Balance')
        ax.grid(True, alpha=0.3)
        
        plt.tight_layout()
        plt.savefig(output_file, format=format.value, dpi=self.dpi, bbox_inches='tight')
        plt.close()
        
        logger.info(f"Created activity plot saved to: {output_file}")
    
    def create_neural_distribution_plot(self, brain: pynlm.Brain, 
                                       output_file: str, format: VisualizationFormat = VisualizationFormat.PNG):
        """Create a plot showing neural distribution by type."""
        region_stats = self._get_region_stats(brain)
        
        fig, axes = plt.subplots(2, 2, figsize=self.figsize)
        fig.suptitle('Neural Distribution by Region', fontsize=16, fontweight='bold')
        
        regions = list(region_stats.keys())
        
        # Plot 1: Neuron count by region and type
        ax = axes[0, 0]
        neuron_types = ['excitatory', 'inhibitory', 'sensory', 'motor', 'internal']
        
        for region_id in regions:
            region_data = region_stats[region_id]
            type_counts = region_data['neuron_types']
            x = np.arange(len(neuron_types))
            widths = [type_counts[nt] for nt in neuron_types]
            
            bars = ax.bar(x + len(regions) * 0.1, widths, 
                        width=0.8 / len(regions), 
                        label=f"Region {region_id}")
            
            # Color bars by neuron type
            for i, nt in enumerate(neuron_types):
                bars[i].set_color(self.colors.get(nt, '#CCCCCC'))
        
        ax.set_xlabel('Neuron Type')
        ax.set_ylabel('Neuron Count')
        ax.set_title('Neurons by Type and Region')
        ax.set_xticks(np.arange(len(neuron_types)))
        ax.set_xticklabels([nt.capitalize() for nt in neuron_types])
        ax.legend()
        ax.grid(True, alpha=0.3, axis='y')
        
        # Plot 2: Synaptic weight distribution
        ax = axes[0, 1]
        all_weights = []
        for region_data in region_stats.values():
            all_weights.extend([w for w in region_data.get('all_weights', []) if w > 0])
        
        if all_weights:
            n, bins, patches = ax.hist(all_weights, bins=20, alpha=0.7, 
                                    color=self.colors['excitatory'], edgecolor='black')
            ax.set_xlabel('Synaptic Weight')
            ax.set_ylabel('Frequency')
            ax.set_title('Synaptic Weight Distribution')
            ax.grid(True, alpha=0.3)
        
        # Plot 3: Region size comparison
        ax = axes[1, 0]
        neuron_counts = [region_stats[rid]['neuron_count'] for rid in regions]
        colors = [self.colors['sensory'] if i % 2 == 0 else self.colors['motor'] 
                 for i in range(len(regions))]
        
        bars = ax.bar(range(len(regions)), neuron_counts, color=colors, edgecolor='black')
        ax.set_xlabel('Region ID')
        ax.set_ylabel('Neuron Count')
        ax.set_title('Neurons by Region')
        ax.set_xticks(range(len(regions)))
        ax.grid(True, alpha=0.3, axis='y')
        
        # Add value labels on bars
        for bar, count in zip(bars, neuron_counts):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{count}', ha='center', va='bottom')
        
        # Plot 4: Connection density
        ax = axes[1, 1]
        connection_densities = []
        for region_data in region_stats.values():
            neurons = region_data['neuron_count']
            synapses = region_data['synapse_count']
            density = synapses / neurons if neurons > 0 else 0
            connection_densities.append(density)
        
        bars = ax.bar(range(len(regions)), connection_densities, 
                    color=self.colors['internal'], alpha=0.7, edgecolor='black')
        ax.set_xlabel('Region ID')
        ax.set_ylabel('Connection Density')
        ax.set_title('Connectivity Density by Region')
        ax.set_xticks(range(len(regions)))
        ax.grid(True, alpha=0.3, axis='y')
        
        # Add value labels
        for bar, density in zip(bars, connection_densities):
            height = bar.get_height()
            ax.text(bar.get_x() + bar.get_width()/2., height,
                   f'{density:.3f}', ha='center', va='bottom')
        
        plt.tight_layout()
        plt.savefig(output_file, format=format.value, dpi=self.dpi, bbox_inches='tight')
        plt.close()
        
        logger.info(f"Created neural distribution plot saved to: {output_file}")
    
    def save_all_snapshots(self, output_file: str):
        """Save all snapshots to a JSON file."""
        snapshots_data = []
        for snapshot in self.snapshots:
            snapshots_data.append({
                'step': snapshot.step,
                'time': snapshot.time,
                'firing_neurons': snapshot.firing_neurons,
                'total_spikes': snapshot.total_spikes,
                'average_firing_rate': snapshot.average_firing_rate,
                'e_i_ratio': snapshot.e_i_ratio,
                'neuronal_activity': snapshot.neuronal_activity,
                'synaptic_weights': snapshot.synaptic_weights,
                'region_stats': snapshot.region_stats
            })
        
        with open(output_file, 'w') as f:
            json.dump(snapshots_data, f, indent=2)
        
        logger.info(f"Saved {len(self.snapshots)} snapshots to: {output_file}")

class NLMDebugSession:
    """Debugging session for NLM simulations with step-by-step control."""
    
    def __init__(self, brain: pynlm.Brain):
        self.brain = brain
        self.session_log: List[Dict[str, Any]] = []
        self.breakpoints: List[int] = []
        self.variables: Dict[str, Any] = {}
        self.is_running = False
        self.current_step = 0
    
    def add_breakpoint(self, step: int):
        """Add a breakpoint at a specific step."""
        self.breakpoints.append(step)
        logger.info(f"Breakpoint added at step {step}")
    
    def remove_breakpoint(self, step: int):
        """Remove a breakpoint."""
        if step in self.breakpoints:
            self.breakpoints.remove(step)
            logger.info(f"Breakpoint removed at step {step}")
    
    def set_variable(self, name: str, value: Any):
        """Set a debugging variable."""
        self.variables[name] = value
        logger.debug(f"Variable '{name}' set to: {value}")
    
    def run_with_debug(self, steps: int, timestep: float = 0.001, 
                      auto_step: bool = True) -> Dict[str, Any]:
        """Run simulation with debugging capabilities."""
        logger.info(f"Starting debug session with {steps} steps")
        
        self.is_running = True
        self.session_log.clear()
        self.current_step = 0
        
        try:
            for step in range(steps):
                if not self.is_running:
                    logger.info("Debug session stopped by user")
                    break
                
                # Check for breakpoints
                if step in self.breakpoints:
                    logger.info(f"Breakpoint reached at step {step}")
                    self._handle_breakpoint(step)
                    
                    # Wait for user input to continue
                    if auto_step:
                        logger.info("Continuing automatically (pause not supported in auto mode)")
                    else:
                        input("Press Enter to continue...")
                
                # Log current state
                state_info = {
                    'step': step,
                    'time': step * timestep,
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'average_firing_rate': self.brain.getAverageFiringRate(),
                    'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                    'variables': dict(self.variables)
                }
                self.session_log.append(state_info)
                
                # Execute step
                self.brain.step(step)
                self.current_step = step
                
                # Log after step execution
                post_step_info = {
                    'step': step,
                    'time': step * timestep,
                    'firing_neurons': self.brain.getFiringNeuronCount(),
                    'total_spikes': self.brain.getTotalSpikeCount(),
                    'average_firing_rate': self.brain.getAverageFiringRate(),
                    'e_i_ratio': self.brain.getExcitationInhibitionRatio(),
                    'delta_spikes': state_info['total_spikes'] - (self.brain.getTotalSpikeCount() - 
                        (state_info['total_spikes'] if step > 0 else 0)),
                    'variables': dict(self.variables)
                }
                self.session_log.append(post_step_info)
                
                logger.debug(f"Step {step} completed")
            
            logger.info("Debug session completed")
            
            return {
                'success': True,
                'steps_completed': steps,
                'total_spikes': self.brain.getTotalSpikeCount(),
                'session_log': self.session_log,
                'breakpoints_hit': [log for log in self.session_log if log.get('is_breakpoint')]
            }
            
        except Exception as e:
            logger.error(f"Debug session failed: {e}")
            return {
                'success': False,
                'error': str(e),
                'steps_completed': self.current_step,
                'session_log': self.session_log[:self.current_step * 2 + 1]
            }
    
    def _handle_breakpoint(self, step: int):
        """Handle a breakpoint hit."""
        log_entry = {
            'step': step,
            'event': 'breakpoint_hit',
            'is_breakpoint': True
        }
        self.session_log.append(log_entry)
    
    def pause(self):
        """Pause the debug session."""
        self.is_running = False
        logger.info("Debug session paused")
    
    def resume(self):
        """Resume the debug session."""
        self.is_running = True
        logger.info("Debug session resumed")
    
    def get_session_stats(self) -> Dict[str, Any]:
        """Get statistics about the debug session."""
        if not self.session_log:
            return {}
        
        steps = [log['step'] for log in self.session_log if 'step' in log]
        firing_neurons = [log['firing_neurons'] for log in self.session_log if 'firing_neurons' in log]
        total_spikes = [log['total_spikes'] for log in self.session_log if 'total_spikes' in log]
        
        return {
            'total_logs': len(self.session_log),
            'steps_ran': len(steps) if steps else 0,
            'max_firing_neurons': max(firing_neurons) if firing_neurons else 0,
            'total_spikes_final': total_spikes[-1] if total_spikes else 0,
            'breakpoints_count': len(self.breakpoints),
            'duration_steps': max(steps) - min(steps) if steps else 0
        }
    
    def save_session_log(self, output_file: str):
        """Save the debug session log to a file."""
        with open(output_file, 'w') as f:
            json.dump(self.session_log, f, indent=2)
        
        logger.info(f"Debug session log saved to: {output_file}")

class NLMPerformanceProfiler:
    """Basic performance profiling for NLM simulations."""
    
    def __init__(self):
        self.profiles: Dict[str, Dict[str, Any]] = {}
    
    def start_profile(self, name: str):
        """Start profiling a simulation."""
        self.profiles[name] = {
            'start_time': time.time(),
            'end_time': None,
            'steps_completed': 0,
            'time_per_step': [],
            'brain_stats': []
        }
        logger.info(f"Started profiling '{name}'")
    
    def step_complete(self, brain: pynlm.Brain, step_time: float):
        """Record completion of a step."""
        current_profile = list(self.profiles.values())[-1] if self.profiles else None
        if current_profile:
            current_profile['steps_completed'] += 1
            current_profile['time_per_step'].append(step_time)
            current_profile['brain_stats'].append({
                'step': current_profile['steps_completed'],
                'firing_neurons': brain.getFiringNeuronCount(),
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate()
            })
    
    def end_profile(self, name: str):
        """End profiling for a simulation."""
        if name in self.profiles:
            profile = self.profiles[name]
            profile['end_time'] = time.time()
            
            total_time = profile['end_time'] - profile['start_time']
            avg_step_time = np.mean(profile['time_per_step']) if profile['time_per_step'] else 0
            
            profile['total_time'] = total_time
            profile['average_step_time'] = avg_step_time
            profile['steps_per_second'] = profile['steps_completed'] / total_time if total_time > 0 else 0
            
            logger.info(f"Ended profiling '{name}' - "
                       f"Total time: {total_time:.2f}s, "
                       f"Average step time: {avg_step_time:.4f}s, "
                       f"Steps/sec: {profile['steps_per_second']:.1f}")
    
    def get_profile_summary(self, name: str) -> Dict[str, Any]:
        """Get summary of a profile."""
        return self.profiles.get(name, {})
    
    def compare_profiles(self, names: List[str]) -> Dict[str, Any]:
        """Compare multiple profiles."""
        comparison = {}
        for name in names:
            if name in self.profiles:
                comparison[name] = self.get_profile_summary(name)
        
        return comparison

# Global instances for convenience
_visualizer = NLMBrainVisualizer()
_debug_session = None
_profiler = NLMPerformanceProfiler()

# Convenience functions
def create_visualizer(figsize: Tuple[int, int] = (12, 8), dpi: int = 100) -> NLMBrainVisualizer:
    """Create a new visualizer instance."""
    global _visualizer
    _visualizer = NLMBrainVisualizer(figsize, dpi)
    return _visualizer

def get_visualizer() -> NLMBrainVisualizer:
    """Get the global visualizer instance."""
    return _visualizer

def create_debug_session(brain: pynlm.Brain) -> NLMDebugSession:
    """Create a new debug session."""
    global _debug_session
    _debug_session = NLMDebugSession(brain)
    return _debug_session

def get_debug_session() -> Optional[NLMDebugSession]:
    """Get the global debug session instance."""
    return _debug_session

def start_profiling(name: str):
    """Start profiling a simulation."""
    _profiler.start_profile(name)

def step_profile(brain: pynlm.Brain, step_time: float):
    """Profile a simulation step."""
    _profiler.step_complete(brain, step_time)

def end_profiling(name: str):
    """End profiling."""
    _profiler.end_profile(name)

def get_profiler() -> NLMPerformanceProfiler:
    """Get the global profiler instance."""
    return _profiler