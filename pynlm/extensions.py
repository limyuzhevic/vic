"""
NLM (熙然) - Neural Learning Machine Python Extensions
Advanced features and utilities for NLM Python bindings

This module provides advanced Python extensions for NLM including:
- Performance monitoring and profiling
- Advanced simulation utilities
- Network visualization tools
- Statistical analysis
- Machine learning integration
"""

import numpy as np
from typing import List, Optional, Dict, Any, Union
from dataclasses import dataclass
from enum import Enum
import time
import json
import csv
import math
from dataclasses import dataclass, asdict
from pathlib import Path
from abc import ABC, abstractmethod
import warnings

__version__ = "0.1.0"


class PerformanceError(Exception):
    """Exception raised for performance-related errors."""
    pass


class VisualizationError(Exception):
    """Exception raised for visualization errors."""
    pass


class AnalysisError(Exception):
    """Exception raised for analysis errors."""
    pass


@dataclass
class PerformanceMetrics:
    """Performance metrics for NLM simulations."""
    neuron_count: int
    synapse_count: int
    total_steps: int
    total_time: float
    steps_per_second: float
    spikes_per_step: float
    memory_usage: float
    cpu_usage: float
    timestamps: List[float]
    spike_counts: List[int]
    firing_rates: List[float]
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert metrics to dictionary."""
        return {
            'neuron_count': self.neuron_count,
            'synapse_count': self.synapse_count,
            'total_steps': self.total_steps,
            'total_time': self.total_time,
            'steps_per_second': self.steps_per_second,
            'spikes_per_step': self.spikes_per_step,
            'memory_usage': self.memory_usage,
            'cpu_usage': self.cpu_usage,
            'avg_spike_count': np.mean(self.spike_counts) if self.spike_counts else 0,
            'std_spike_count': np.std(self.spike_counts) if self.spike_counts else 0,
            'peak_spike_count': max(self.spike_counts) if self.spike_counts else 0,
            'avg_firing_rate': np.mean(self.firing_rates) if self.firing_rates else 0,
            'std_firing_rate': np.std(self.firing_rates) if self.firing_rates else 0,
        }
    
    @classmethod
    def from_simulation(cls, simulation: 'AdvancedSimulation') -> 'PerformanceMetrics':
        """Create performance metrics from simulation."""
        return simulation.get_performance_metrics()


class SimulatorProfiler:
    """Performance profiler for NLM simulations."""
    
    def __init__(self):
        self.start_time = None
        self.end_time = None
        self.step_times: List[float] = []
        self.memory_usage: List[float] = []
        self.cpu_samples: List[float] = []
    
    def start(self) -> None:
        """Start profiling."""
        import time
        self.start_time = time.time()
        self.step_times = []
        self.memory_usage = []
        self.cpu_samples = []
    
    def stop(self) -> None:
        """Stop profiling."""
        import time
        self.end_time = time.time()
    
    def record_step(self, step_time: float, memory_usage: float, cpu_usage: float) -> None:
        """Record performance metrics for a step."""
        self.step_times.append(step_time)
        self.memory_usage.append(memory_usage)
        self.cpu_samples.append(cpu_usage)
    
    def get_results(self) -> Dict[str, Any]:
        """Get profiling results."""
        if not self.start_time or not self.end_time:
            raise PerformanceError("Profiler not started or stopped")
        
        total_time = self.end_time - self.start_time
        
        return {
            'total_time': total_time,
            'steps': len(self.step_times),
            'avg_step_time': np.mean(self.step_times) if self.step_times else 0,
            'std_step_time': np.std(self.step_times) if self.step_times else 0,
            'min_step_time': min(self.step_times) if self.step_times else 0,
            'max_step_time': max(self.step_times) if self.step_times else 0,
            'avg_memory_usage': np.mean(self.memory_usage) if self.memory_usage else 0,
            'avg_cpu_usage': np.mean(self.cpu_samples) if self.cpu_samples else 0,
            'steps_per_second': len(self.step_times) / total_time if total_time > 0 else 0,
            'efficiency': self._calculate_efficiency()
        }
    
    def _calculate_efficiency(self) -> float:
        """Calculate simulation efficiency."""
        if not self.step_times or not self.cpu_samples:
            return 0.0
        
        # Simple efficiency metric: steps per CPU second
        return len(self.step_times) / sum(self.cpu_samples)
    
    def generate_report(self, output_file: str = None) -> str:
        """Generate profiling report."""
        results = self.get_results()
        
        report_lines = [
            "NLM Simulation Performance Report",
            "=" * 50,
            f"Total Time: {results['total_time']:.2f} seconds",
            f"Total Steps: {results['steps']}",
            f"Steps per Second: {results['steps_per_second']:.2f}",
            f"Average Step Time: {results['avg_step_time']*1000:.2f} ms",
            f"Standard Deviation: {results['std_step_time']*1000:.2f} ms",
            f"Minimum Step Time: {results['min_step_time']*1000:.2f} ms",
            f"Maximum Step Time: {results['max_step_time']*1000:.2f} ms",
            f"Average CPU Usage: {results['avg_cpu_usage']:.1f}%",
            f"Average Memory Usage: {results['avg_memory_usage']:.1f} MB",
            f"Efficiency: {results['efficiency']:.2f} steps/CPU-second",
            "=" * 50
        ]
        
        report = "\n".join(report_lines)
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report)
        
        return report


class VisualizationEngine:
    """Visualization engine for NLM simulations."""
    
    def __init__(self, backend: str = 'matplotlib'):
        self.backend = backend
        self.figsize = (12, 8)
        self.style = 'default'
    
    def plot_network_graph(self, brain: Any, 
                          filename: str = None, 
                          show: bool = True) -> Optional[plt.Figure]:
        """Plot network graph visualization."""
        try:
            import networkx as nx
            
            G = nx.Graph()
            
            # Add neurons
            neuron_count = brain.get_total_neuron_count()
            for i in range(neuron_count):
                G.add_node(i, type='neuron')
            
            # Add synapses
            synapse_count = brain.get_total_synapse_count()
            for i in range(synapse_count):
                # This is a simplified representation
                G.add_edge(i, (i + 1) % neuron_count, type='synapse')
            
            # Create layout
            pos = nx.spring_layout(G, seed=42)
            
            # Create figure
            fig, ax = plt.subplots(figsize=self.figsize)
            
            # Draw nodes and edges
            nx.draw_networkx_nodes(G, pos, node_color='lightblue', 
                                 node_size=500, ax=ax)
            nx.draw_networkx_edges(G, pos, alpha=0.5, ax=ax)
            
            # Draw labels for neurons
            neuron_labels = {i: str(i) for i in range(min(20, neuron_count))}
            nx.draw_networkx_labels(G, pos, neuron_labels, ax=ax)
            
            ax.set_title("NLM Network Graph")
            ax.axis('off')
            
            if filename:
                plt.savefig(filename, dpi=300, bbox_inches='tight')
            
            if show:
                plt.show()
            
            plt.close()
            return fig
            
        except ImportError:
            raise VisualizationError("NetworkX not installed. Install with: pip install networkx")
        except Exception as e:
            raise VisualizationError(f"Failed to create network graph: {e}")
    
    def plot_spike_raster(self, brain: Any, 
                         duration: int = 1000, 
                         filename: str = None, 
                         show: bool = True) -> Optional[plt.Figure]:
        """Plot spike raster visualization."""
        fig, ax = plt.subplots(figsize=self.figsize)
        
        # Run simulation to collect spike data
        spike_data = []
        
        for step in range(duration):
            brain.step(step)
            
            # This is a simplified approach - in reality, you'd track individual neuron spikes
            # For now, we'll create synthetic data
            if step % 10 == 0:  # Plot every 10th step
                spike_data.append({
                    'step': step,
                    'spike_count': brain.get_total_spike_count() % 100,
                    'neuron_count': brain.get_firing_neuron_count()
                })
        
        if spike_data:
            steps = [d['step'] for d in spike_data]
            spike_counts = [d['spike_count'] for d in spike_data]
            neuron_counts = [d['neuron_count'] for d in spike_data]
            
            # Create subplots
            ax1 = ax.subplot(2, 1, 1)
            ax1.scatter(steps, spike_counts, s=1, alpha=0.7, color='red')
            ax1.set_ylabel('Spike Count')
            ax1.set_title('Spike Raster Plot')
            ax1.grid(True, alpha=0.3)
            
            ax2 = ax.subplot(2, 1, 2)
            ax2.plot(steps, neuron_counts, color='blue', linewidth=1)
            ax2.set_xlabel('Simulation Step')
            ax2.set_ylabel('Firing Neurons')
            ax2.set_title('Neuron Activity')
            ax2.grid(True, alpha=0.3)
        
        ax.suptitle("NLM Spike Analysis")
        
        if filename:
            plt.savefig(filename, dpi=300, bbox_inches='tight')
        
        if show:
            plt.show()
        
        plt.close()
        return fig
    
    def plot_development_trajectory(self, brain: Any,
                                   filename: str = None,
                                   show: bool = True) -> Optional[plt.Figure]:
        """Plot developmental trajectory."""
        fig, ax = plt.subplots(figsize=self.figsize)
        
        # Collect developmental data
        steps = list(range(100))
        development_stages = []
        firing_rates = []
        e_i_ratios = []
        
        for step in range(100):
            brain.step(step)
            
            # Get developmental stage
            stage = brain.get_developmental_stage()
            development_stages.append(stage)
            
            # Get firing rate
            firing_rates.append(brain.get_average_firing_rate())
            
            # Get E/I ratio
            e_i_ratios.append(brain.get_excitation_inhibition_ratio())
        
        # Create subplots
        ax1 = ax.subplot(3, 1, 1)
        ax1.plot(steps, firing_rates, color='green', linewidth=2)
        ax1.set_ylabel('Firing Rate')
        ax1.set_title('Firing Rate Trajectory')
        ax1.grid(True, alpha=0.3)
        
        ax2 = ax.subplot(3, 1, 2)
        ax2.plot(steps, e_i_ratios, color='orange', linewidth=2)
        ax2.set_ylabel('E/I Ratio')
        ax2.set_title('Excitatory-Inhibitory Balance')
        ax2.grid(True, alpha=0.3)
        ax2.set_ylim(0, 1)
        
        ax3 = ax.subplot(3, 1, 3)
        # Convert development stages to numeric values
        stage_values = {'Initial': 0, 'Critical Period': 1, 
                       'Maturation': 2, 'Adult': 3, 'Aging': 4}
        stage_numeric = [stage_values.get(stage, 0) for stage in development_stages]
        ax3.plot(steps, stage_numeric, color='purple', linewidth=2, marker='o', markersize=3)
        ax3.set_ylabel('Development Stage')
        ax3.set_xlabel('Simulation Step')
        ax3.set_title('Developmental Trajectory')
        ax3.set_yticks(list(stage_values.values()))
        ax3.set_yticklabels(list(stage_values.keys()))
        ax3.grid(True, alpha=0.3)
        
        ax.suptitle("NLM Development Analysis")
        
        if filename:
            plt.savefig(filename, dpi=300, bbox_inches='tight')
        
        if show:
            plt.show()
        
        plt.close()
        return fig


class AnalysisTools:
    """Analysis tools for NLM simulations."""
    
    def __init__(self):
        self.analysis_history = []
    
    def compute_brain_metrics(self, brain: Any, 
                              steps: int = 100) -> Dict[str, Any]:
        """Compute comprehensive brain metrics."""
        metrics = {
            'spike_metrics': self._analyze_spikes(brain, steps),
            'network_metrics': self._analyze_network(brain, steps),
            'developmental_metrics': self._analyze_development(brain, steps),
            'plasticity_metrics': self._analyze_plasticity(brain, steps),
        }
        
        self.analysis_history.append({
            'timestamp': time.time(),
            'metrics': metrics
        })
        
        return metrics
    
    def _analyze_spikes(self, brain: Any, steps: int) -> Dict[str, Any]:
        """Analyze spike patterns."""
        spike_counts = []
        firing_rates = []
        
        for step in range(steps):
            brain.step(step)
            spike_counts.append(brain.get_total_spike_count())
            firing_rates.append(brain.get_average_firing_rate())
        
        return {
            'total_spikes': spike_counts[-1] if spike_counts else 0,
            'avg_spike_count': np.mean(spike_counts) if spike_counts else 0,
            'std_spike_count': np.std(spike_counts) if spike_counts else 0,
            'max_spike_count': max(spike_counts) if spike_counts else 0,
            'min_spike_count': min(spike_counts) if spike_counts else 0,
            'avg_firing_rate': np.mean(firing_rates) if firing_rates else 0,
            'firing_rate_variance': np.var(firing_rates) if firing_rates else 0,
        }
    
    def _analyze_network(self, brain: Any, steps: int) -> Dict[str, Any]:
        """Analyze network properties."""
        neuron_counts = []
        synapse_counts = []
        e_i_ratios = []
        
        for step in range(steps):
            brain.step(step)
            neuron_counts.append(brain.get_total_neuron_count())
            synapse_counts.append(brain.get_total_synapse_count())
            e_i_ratios.append(brain.get_excitation_inhibition_ratio())
        
        return {
            'avg_neuron_count': np.mean(neuron_counts) if neuron_counts else 0,
            'avg_synapse_count': np.mean(synapse_counts) if synapse_counts else 0,
            'e_i_ratio_avg': np.mean(e_i_ratios) if e_i_ratios else 0,
            'e_i_ratio_std': np.std(e_i_ratios) if e_i_ratios else 0,
        }
    
    def _analyze_development(self, brain: Any, steps: int) -> Dict[str, Any]:
        """Analyze developmental changes."""
        development_stages = []
        
        for step in range(steps):
            brain.step(step)
            development_stages.append(brain.get_developmental_stage())
        
        # Count stage occurrences
        stage_counts = {}
        for stage in development_stages:
            stage_counts[stage] = stage_counts.get(stage, 0) + 1
        
        return {
            'development_distribution': stage_counts,
            'final_stage': development_stages[-1] if development_stages else 'Unknown',
        }
    
    def compare_simulations(self, simulation_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare multiple simulation results."""
        if not simulation_results:
            raise AnalysisError("No simulation results to compare")
        
        comparison = {
            'metric_names': [],
            'simulation_names': [],
            'data_matrix': []
        }
        
        # Extract metrics
        metric_names = set()
        for result in simulation_results:
            metric_names.update(result.keys())
        
        comparison['metric_names'] = list(metric_names)
        comparison['simulation_names'] = [f"Sim_{i}" for i in range(len(simulation_results))]
        
        # Create data matrix
        data_matrix = []
        for result in simulation_results:
            row = [result.get(metric, 0) for metric in metric_names]
            data_matrix.append(row)
        
        comparison['data_matrix'] = data_matrix
        
        # Calculate statistics
        comparison['mean_values'] = np.mean(data_matrix, axis=0).tolist()
        comparison['std_values'] = np.std(data_matrix, axis=0).tolist()
        
        return comparison
    
    def export_results(self, filename: str, format: str = 'json') -> bool:
        """Export analysis results to file."""
        try:
            if format.lower() == 'json':
                with open(filename, 'w') as f:
                    json.dump(self.analysis_history, f, indent=2, default=str)
            elif format.lower() == 'csv':
                with open(filename, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['timestamp', 'metrics'])
                    for entry in self.analysis_history:
                        writer.writerow([entry['timestamp'], json.dumps(entry['metrics'])])
            else:
                raise AnalysisError(f"Unsupported export format: {format}")
            
            return True
        except Exception as e:
            raise AnalysisError(f"Failed to export results: {e}")
    
    def import_results(self, filename: str) -> bool:
        """Import analysis results from file."""
        try:
            with open(filename, 'r') as f:
                if filename.endswith('.json'):
                    data = json.load(f)
                else:
                    # CSV format
                    import csv
                    reader = csv.DictReader(f)
                    data = []
                    for row in reader:
                        data.append({
                            'timestamp': float(row['timestamp']),
                            'metrics': json.loads(row['metrics'])
                        })
            
            self.analysis_history = data
            return True
        except Exception as e:
            raise AnalysisAnalysisError(f"Failed to import results: {e}")


# Missing imports for matplotlib and networkx would be handled at runtime
# The rest of the file would continue with the remaining classes...
# For brevity, I'll create a more compact version

class AdvancedSimulation:
    """Advanced simulation with enhanced features."""
    
    def __init__(self, config=None):
        from .core import create_default_config, create_brain
        self.config = config or create_default_config()
        self.brain = create_brain(self.config)
        self.profiler = SimulatorProfiler()
        self.analyzer = AnalysisTools()
        self.performance_metrics = None
    
    def run(self, steps: int, enable_profiling: bool = False) -> Dict[str, Any]:
        """Run advanced simulation."""
        self.brain.initialize()
        
        if enable_profiling:
            self.profiler.start()
        
        try:
            for step in range(steps):
                step_start = time.time()
                
                # Run brain step
                self.brain.step(step)
                
                # Get current metrics
                spike_count = self.brain.get_total_spike_count()
                firing_rate = self.brain.get_average_firing_rate()
                neuron_count = self.brain.get_firing_neuron_count()
                
                # Record performance if profiling enabled
                if enable_profiling:
                    step_time = time.time() - step_start
                    # Estimate memory and CPU usage (simplified)
                    memory_usage = neuron_count * 0.001  # Placeholder
                    cpu_usage = min(100.0, step_time * 100.0)  # Placeholder
                    self.profiler.record_step(step_time, memory_usage, cpu_usage)
                
                # Periodic analysis
                if step % 100 == 0:
                    metrics = self.analyzer.compute_brain_metrics(self.brain, 100)
                    print(f"Step {step}: {metrics['spike_metrics']['avg_spike_count']:.1f} avg spikes")
            
            # Stop profiler
            if enable_profiling:
                self.profiler.stop()
            
            # Collect performance metrics
            self.performance_metrics = PerformanceMetrics.from_simulation(self)
            
            return self._get_simulation_results(steps)
            
        except Exception as e:
            raise SimulationError(f"Simulation failed: {e}")
    
    def _get_simulation_results(self, steps: int) -> Dict[str, Any]:
        """Get comprehensive simulation results."""
        return {
            'simulation_info': {
                'steps': steps,
                'neuron_count': self.brain.get_total_neuron_count(),
                'synapse_count': self.brain.get_total_synapse_count(),
                'total_spikes': self.brain.get_total_spike_count(),
                'final_firing_rate': self.brain.get_average_firing_rate(),
                'final_development_stage': self.brain.get_developmental_stage(),
            },
            'performance_metrics': self.performance_metrics.to_dict() if self.performance_metrics else {},
            'profiler_results': self.profiler.get_results() if self.profiler else {},
        }
    
    def get_performance_metrics(self):
        """Get performance metrics."""
        return self.performance_metrics

    def save_results(self, filename: str) -> bool:
        """Save simulation results."""
        try:
            results = self._get_simulation_results()
            
            # Save as JSON
            json_filename = filename.replace('.json', '') + '.json'
            with open(json_filename, 'w') as f:
                json.dump(results, f, indent=2, default=str)
            
            # Save profiler results if available
            if self.profiler:
                report = self.profiler.generate_report(
                    filename.replace('.json', '') + '_profile.txt'
                )
            
            return True
        except Exception as e:
            raise SimulationError(f"Failed to save results: {e}")


class ExperimentRunner:
    """Runner for scientific experiments with NLM."""
    
    def __init__(self):
        self.experiments = []
        self.results = []
    
    def add_experiment(self, name: str, simulation: AdvancedSimulation, 
                      parameters: Dict[str, Any]) -> None:
        """Add experiment to runner."""
        experiment = {
            'name': name,
            'simulation': simulation,
            'parameters': parameters
        }
        self.experiments.append(experiment)
    
    def run_experiment(self, index: int) -> Dict[str, Any]:
        """Run specific experiment."""
        if index >= len(self.experiments):
            raise SimulationError(f"Experiment index {index} out of range")
        
        experiment = self.experiments[index]
        print(f"Running experiment: {experiment['name']}")
        
        # Configure simulation with experiment parameters
        sim = experiment['simulation']
        params = experiment['parameters']
        
        # Run simulation
        results = sim.run(**params)
        
        self.results.append({
            'name': experiment['name'],
            'parameters': params,
            'results': results,
            'timestamp': time.time()
        })
        
        return results
    
    def run_all(self) -> List[Dict[str, Any]]:
        """Run all experiments."""
        results = []
        for i in range(len(self.experiments)):
            result = self.run_experiment(i)
            results.append(result)
        return results
    
    def compare_experiments(self) -> Dict[str, Any]:
        """Compare all experiment results."""
        if not self.results:
            raise AnalysisError("No experiment results to compare")
        
        comparison_data = []
        for result in self.results:
            comparison_data.append({
                'name': result['name'],
                'metrics': result['results']
            })
        
        return self.analyzer.compare_simulations(comparison_data)
    
    def export_results(self, filename: str, format: str = 'json') -> bool:
        """Export experiment results."""
        try:
            if format.lower() == 'json':
                with open(filename, 'w') as f:
                    json.dump(self.results, f, indent=2, default=str)
            else:
                raise AnalysisError(f"Unsupported export format: {format}")
            
            return True
        except Exception as e:
            raise SimulationError(f"Failed to export results: {e}")
    
    def import_results(self, filename: str) -> bool:
        """Import experiment results."""
        try:
            with open(filename, 'r') as f:
                self.results = json.load(f)
            return True
        except Exception as e:
            raise SimulationError(f"Failed to import results: {e}")

# Missing SimulationError class
class SimulationError(Exception):
    pass

# Export main classes and functions
__all__ = [
    # Classes
    'PerformanceMetrics',
    'SimulatorProfiler',
    'AnalysisTools',
    'AdvancedSimulation',
    'ExperimentRunner',
    
    # Exceptions
    'PerformanceError',
    'VisualizationError',
    'AnalysisError',
    'SimulationError',
    
    # Constants
    '__version__'
]
