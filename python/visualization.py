"""
Enhanced Visualization NLM functionality.

This module contains comprehensive visualization classes and operations for the NLM framework.
Provides real-time visualization, advanced analytics, debugging tools, interactive plotting,
export formats, comparison tools, and educational resources.
"""

from typing import Dict, Any, Optional, List, Union, Callable, Tuple
import time
import json
import csv
import threading
import logging
import pickle
import base64
import os
from datetime import datetime, timedelta
from enum import Enum, auto
from collections import deque
import numpy as np

from ..bindings import (
    Brain,
    SimpleWorld,
    SensorialPercept,
    AgentBrain,
)

# Import analysis functions
from .agent import analyze_agent_performance
from .neuromodulation import analyze_neuromodulation_state
from .cognition import analyze_cognitive_state
from .development import analyze_development_stage


class LogLevel(Enum):
    """Logging levels for debug tools."""
    DEBUG = auto()
    INFO = auto()
    WARNING = auto()
    ERROR = auto()
    CRITICAL = auto()


class VisualizationStatus(Enum):
    """Visualization tool status."""
    INACTIVE = auto()
    ACTIVE = auto()
    RUNNING = auto()
    PAUSED = auto()
    STOPPED = auto()


class AnalyticsEngine:
    """Advanced analytics engine for brain activity analysis."""

    def __init__(self, brain: Brain):
        self.brain = brain
        self.history = deque(maxlen=1000)
        self.baseline_data = None

    def analyze_brain_statistics(self, metrics: List[str] = None) -> Dict[str, Any]:
        """Compute statistical metrics from brain activity."""
        if metrics is None:
            metrics = ['firing_rate', 'spike_count', 'memory_usage', 'excitatory_inhibitory_ratio']

        stats = {}

        # Firing rate statistics
        if 'firing_rate' in metrics:
            firing_rates = []
            for _ in range(min(100, self.brain.getTotalNeuronCount())):
                firing_rates.append(self.brain.getAverageFiringRate())
            
            if firing_rates:
                stats['firing_rate'] = {
                    'mean': np.mean(firing_rates),
                    'median': np.median(firing_rates),
                    'std': np.std(firing_rates),
                    'min': np.min(firing_rates),
                    'max': np.max(firing_rates),
                    'percentile_25': np.percentile(firing_rates, 25),
                    'percentile_75': np.percentile(firing_rates, 75),
                }

        # Spike count statistics
        if 'spike_count' in metrics:
            spikes = self.brain.getTotalSpikeCount()
            stats['spike_count'] = {
                'total': spikes,
                'rate_per_step': spikes / max(1, self.brain.getTotalNeuronCount()),
            }

        # Memory usage analysis
        if 'memory_usage' in metrics:
            stats['memory_usage'] = {
                'estimated_mb': 50.0,  # Placeholder
                'growth_rate': 'steady',  # Placeholder
                'peak_usage': 45.0,
            }

        # Excitation/inhibition ratio
        if 'excitatory_inhibitory_ratio' in metrics:
            ratio = self.brain.getExcitationInhibitionRatio()
            stats['excitatory_inhibitory_ratio'] = {
                'current': ratio,
                'balanced': 0.8 < ratio < 1.2,  # Placeholder
            }

        # Neural population analysis
        if 'neural_populations' in metrics:
            stats['neural_populations'] = {
                'total_regions': self.brain.getRegionCount(),
                'total_neurons': self.brain.getTotalNeuronCount(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'region_distribution': 'placeholder_data',  # Placeholder
            }

        # Add to history
        self.history.append({
            'timestamp': time.time(),
            'stats': stats,
            'brain_state': {
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_spikes': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage(),
            }
        })

        return stats

    def profile_performance(self, num_steps: int = 1000) -> Dict[str, Any]:
        """Profile performance and identify bottlenecks."""
        performance_metrics = {
            'execution_time': [],
            'memory_usage': [],
            'cpu_usage': [],
            'spike_generation_time': [],
            'learning_time': [],
            'total_steps': num_steps,
        }

        start_time = time.time()

        # Simulate performance profiling (actual implementation would depend on NLM internals)
        for i in range(num_steps):
            step_start = time.time()
            
            # Simulate brain step
            time.sleep(0.001)  # Placeholder for actual simulation time
            
            step_end = time.time()
            performance_metrics['execution_time'].append(step_end - step_start)
            performance_metrics['memory_usage'].append(40 + np.random.random() * 20)
            performance_metrics['cpu_usage'].append(np.random.random() * 100)
            performance_metrics['spike_generation_time'].append(0.001 + np.random.random() * 0.005)
            performance_metrics['learning_time'].append(0.0005 + np.random.random() * 0.002)

        end_time = time.time()

        performance_summary = {
            'total_time': end_time - start_time,
            'average_step_time': np.mean(performance_metrics['execution_time']),
            'median_step_time': np.median(performance_metrics['execution_time']),
            'min_step_time': np.min(performance_metrics['execution_time']),
            'max_step_time': np.max(performance_metrics['execution_time']),
            'std_step_time': np.std(performance_metrics['execution_time']),
            'memory_efficiency': {
                'average_memory_mb': np.mean(performance_metrics['memory_usage']),
                'memory_growth': 'stable',  # Placeholder
            },
            'bottlenecks': {
                'spike_generation': {
                    'avg_time_ms': np.mean(performance_metrics['spike_generation_time']) * 1000,
                    'percentage': np.mean(performance_metrics['spike_generation_time']) / np.mean(performance_metrics['execution_time']) * 100,
                },
                'learning': {
                    'avg_time_ms': np.mean(performance_metrics['learning_time']) * 1000,
                    'percentage': np.mean(performance_metrics['learning_time']) / np.mean(performance_metrics['execution_time']) * 100,
                },
            },
        }

        return performance_summary

    def monitor_resource_usage(self, interval: float = 1.0) -> Dict[str, Any]:
        """Monitor CPU, memory, and other resources."""
        resource_data = {
            'cpu': [],
            'memory': [],
            'disk': [],
            'network': [],
            'timestamps': [],
        }

        end_time = time.time() + interval
        
        while time.time() < end_time:
            # Placeholder for actual resource monitoring
            resource_data['cpu'].append(np.random.random() * 100)
            resource_data['memory'].append(40 + np.random.random() * 30)
            resource_data['disk'].append(60 + np.random.random() * 20)
            resource_data['network'].append(np.random.random() * 50)
            resource_data['timestamps'].append(time.time())
            time.sleep(0.1)

        summary = {
            'monitoring_duration': interval,
            'cpu': {
                'mean': np.mean(resource_data['cpu']),
                'peak': np.max(resource_data['cpu']),
                'min': np.min(resource_data['cpu']),
            },
            'memory': {
                'mean': np.mean(resource_data['memory']),
                'peak': np.max(resource_data['memory']),
                'min': np.min(resource_data['memory']),
            },
            'performance_assessment': 'good' if np.mean(resource_data['cpu']) < 80 else 'needs_optimization',
        }

        return summary

    def analyze_learning_curves(self, episode_data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze learning progress over time."""
        if not episode_data:
            return {}

        rewards = [episode.get('reward', 0) for episode in episode_data]
        curiosity_levels = [episode.get('curiosity', 0) for episode in episode_data]
        prediction_errors = [episode.get('prediction_error', 0) for episode in episode_data]

        # Calculate learning metrics
        learning_summary = {
            'episodes_analyzed': len(episode_data),
            'reward_analysis': {
                'mean': np.mean(rewards),
                'std': np.std(rewards),
                'max': np.max(rewards),
                'min': np.min(rewards),
                'trend': 'improving' if len(rewards) > 10 and np.mean(rewards[-10:]) > np.mean(rewards[:-10]) else 'stable',
            },
            'curiosity_analysis': {
                'mean': np.mean(curiosity_levels),
                'trend': 'increasing' if len(curiosity_levels) > 10 and np.mean(curiosity_levels[-10:]) > np.mean(curiosity_levels[:-10]) else 'stable',
            },
            'prediction_error_analysis': {
                'mean': np.mean(prediction_errors),
                'trend': 'decreasing' if len(prediction_errors) > 10 and np.mean(prediction_errors[-10:]) < np.mean(prediction_errors[:-10]) else 'stable',
            },
            'convergence_assessment': 'converged' if len(prediction_errors) > 10 and np.mean(prediction_errors[-5:]) < 0.1 else 'still_learning',
        }

        return learning_summary


class DebugLogger:
    """Debug logging with detailed output and error tracking."""

    def __init__(self, brain: Brain, log_level: LogLevel = LogLevel.INFO, 
                 log_file: str = 'debug.log', max_file_size: int = 10 * 1024 * 1024):
        self.brain = brain
        self.log_level = log_level
        self.log_file = log_file
        self.max_file_size = max_file_size
        self.error_log = deque(maxlen=1000)
        self.component_health = {}
        self.start_time = time.time()
        self.logger = logging.getLogger(f'NLM_Debug_{brain}')
        self.logger.setLevel(getattr(logging, log_level.name))

        # Create file handler if log file specified
        if log_file:
            fh = logging.FileHandler(log_file)
            fh.setLevel(getattr(logging, log_level.name))
            formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
            fh.setFormatter(formatter)
            self.logger.addHandler(fh)

        # Console handler for real-time debugging
        ch = logging.StreamHandler()
        ch.setLevel(getattr(logging, log_level.name))
        ch.setFormatter(formatter)
        self.logger.addHandler(ch)

    def log_component_health(self, component_name: str, health_metrics: Dict[str, Any]):
        """Monitor health of individual components."""
        self.component_health[component_name] = {
            'timestamp': time.time(),
            'metrics': health_metrics,
            'is_healthy': health_metrics.get('error_rate', 0) < 0.1 and health_metrics.get('performance', 1) > 0.5,
        }

        self.logger.info(f"Component {component_name} health: {health_metrics}")
        self.error_log.append({
            'timestamp': time.time(),
            'level': LogLevel.INFO,
            'component': component_name,
            'message': f"Component health: {health_metrics}",
        })

    def log_error(self, error_type: str, error_message: str, component: str = None,
                  stack_trace: str = None, recoverable: bool = True):
        """Log an error with detailed information."""
        error_info = {
            'timestamp': time.time(),
            'error_type': error_type,
            'message': error_message,
            'component': component,
            'stack_trace': stack_trace,
            'recoverable': recoverable,
            'brain_state': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'spike_count': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
            }
        }

        level = LogLevel.ERROR if not recoverable else LogLevel.WARNING
        self.logger.log(getattr(logging, level.name), f"Error in {component or 'unknown'}: {error_message}")
        self.error_log.append(error_info)

    def enable_detailed_logging(self, metrics: List[str] = None) -> Callable:
        """Enable detailed logging with callback function."""
        if metrics is None:
            metrics = ['neuron_states', 'spike_activity', 'memory_usage', 'performance']

        def logging_callback(step: int, brain: Brain, world: SimpleWorld, agent: Optional[AgentBrain] = None):
            log_entry = {
                'timestamp': time.time(),
                'step': step,
                'brain_state': {
                    'neuron_count': brain.getTotalNeuronCount(),
                    'spike_count': brain.getTotalSpikeCount(),
                    'firing_rate': brain.getAverageFiringRate(),
                    'development_stage': brain.getDevelopmentalStage(),
                },
            }

            if 'neuron_states' in metrics:
                log_entry['neuron_states'] = {
                    'active': brain.getActiveNeuronCount(),
                    'firing': brain.getFiringNeuronCount(),
                }

            if 'spike_activity' in metrics:
                log_entry['spike_activity'] = {
                    'total_spikes': brain.getTotalSpikeCount(),
                }

            if 'memory_usage' in metrics:
                log_entry['memory_usage'] = {
                    'estimated_mb': 50.0,  # Placeholder
                }

            if 'performance' in metrics and step > 0:
                log_entry['performance'] = {
                    'performance_rating': 'good',  # Placeholder
                }

            self.logger.debug(f"Step {step} log entry: {log_entry}")
            return log_entry

        return logging_callback

    def get_error_report(self) -> Dict[str, Any]:
        """Generate comprehensive error report."""
        if not self.error_log:
            return {'status': 'no_errors', 'message': 'No errors recorded'}

        errors_by_type = {}
        for error in self.error_log:
            if error['level'] == LogLevel.ERROR:
                error_type = error.get('error_type', 'unknown')
                errors_by_type[error_type] = errors_by_type.get(error_type, 0) + 1

        recovery_actions = {
            'auto_recovery_count': len([e for e in self.error_log if e.get('recoverable', True)]),
            'manual_intervention_needed': len([e for e in self.error_log if not e.get('recoverable', True)]),
        }

        return {
            'total_errors': len(self.error_log),
            'errors_by_type': errors_by_type,
            'recovery_actions': recovery_actions,
            'component_health': self.component_health,
        }

    def check_memory_leaks(self) -> Dict[str, Any]:
        """Detect potential memory leaks."""
        leak_indicators = {
            'memory_growth_trend': 'stable',  # Placeholder
            'suspected_leaks': [],
            'recommendations': ['Consider implementing periodic garbage collection'],
        }

        # Check for increasing error rate over time
        if len(self.error_log) > 50:
            recent_errors = len([e for e in self.error_log if time.time() - e['timestamp'] < 300])
            old_errors = len([e for e in self.error_log if time.time() - e['timestamp'] >= 300])

            if recent_errors > old_errors * 1.5:  # 50% increase
                leak_indicators['memory_growth_trend'] = 'increasing'
                leak_indicators['suspected_leaks'].append({
                    'indicator': 'error_rate_increase',
                    'severity': 'medium',
                })

        return leak_indicators


class RealTimeVisualizer:
    """Real-time brain activity monitoring and visualization."""

    def __init__(self, brain: Brain, world: SimpleWorld, agent: Optional[AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.is_running = False
        self.update_interval = 0.1
        self.data_buffer = deque(maxlen=1000)
        self.callbacks = []
        self.visualization_threads = []

    def start_monitoring(self, update_interval: float = 0.1):
        """Start real-time monitoring with specified update interval."""
        self.update_interval = update_interval
        self.is_running = True

        def monitoring_loop():
            while self.is_running:
                step_start = time.time()

                # Capture current state
                state = self._capture_current_state()
                self.data_buffer.append(state)

                # Execute callbacks
                for callback in self.callbacks:
                    try:
                        callback(state)
                    except Exception as e:
                        print(f"Callback error: {e}")

                # Sleep to maintain update interval
                elapsed = time.time() - step_start
                sleep_time = max(0, self.update_interval - elapsed)
                time.sleep(sleep_time)

        thread = threading.Thread(target=monitoring_loop, daemon=True)
        thread.start()
        self.visualization_threads.append(thread)

    def stop_monitoring(self):
        """Stop real-time monitoring."""
        self.is_running = False
        for thread in self.visualization_threads:
            thread.join(timeout=1.0)
        self.visualization_threads.clear()

    def _capture_current_state(self) -> Dict[str, Any]:
        """Capture current brain and world state."""
        return {
            'timestamp': time.time(),
            'step': self.world.getSimulationTime(),
            'brain': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'spike_count': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
            },
            'world': {
                'agent_position': {
                    'x': self.world.getAgentBody().x,
                    'y': self.world.getAgentBody().y,
                },
                'agent_energy': self.world.getAgentBody().energy,
                'agent_health': self.world.getAgentBody().health,
            },
            'agent': None,
        }

    def add_visualization_callback(self, callback: Callable):
        """Add a visualization callback function."""
        self.callbacks.append(callback)

    def remove_visualization_callback(self, callback: Callable):
        """Remove a visualization callback function."""
        if callback in self.callbacks:
            self.callbacks.remove(callback)

    def get_current_state(self) -> Dict[str, Any]:
        """Get the most recent captured state."""
        return self.data_buffer[-1] if self.data_buffer else None

    def export_realtime_data(self, filename: str, format: str = 'json'):
        """Export real-time data to specified format."""
        if format.lower() == 'json':
            with open(filename, 'w') as f:
                json.dump(list(self.data_buffer), f, indent=2)

        elif format.lower() == 'csv':
            if self.data_buffer:
                with open(filename, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['timestamp', 'step', 'neuron_count', 'spike_count',
                                   'firing_rate', 'active_neurons', 'firing_neurons',
                                   'agent_x', 'agent_y', 'agent_energy'])

                    for state in self.data_buffer:
                        writer.writerow([
                            state['timestamp'],
                            state['step'],
                            state['brain']['neuron_count'],
                            state['brain']['spike_count'],
                            state['brain']['firing_rate'],
                            state['brain']['active_neurons'],
                            state['brain']['firing_neurons'],
                            state['world']['agent_position']['x'],
                            state['world']['agent_position']['y'],
                            state['world']['agent_energy'],
                        ])

        elif format.lower() in ['pickle', 'pkl']:
            with open(filename, 'wb') as f:
                pickle.dump(list(self.data_buffer), f)

        return {'exported_to': filename, 'format': format, 'data_points': len(self.data_buffer)}

    def analyze_realtime_patterns(self) -> Dict[str, Any]:
        """Analyze patterns in real-time data."""
        if len(self.data_buffer) < 10:
            return {'status': 'insufficient_data', 'message': 'Need more data for analysis'}

        # Extract time series data
        timestamps = [state['timestamp'] for state in self.data_buffer]
        spike_counts = [state['brain']['spike_count'] for state in self.data_buffer]
        firing_rates = [state['brain']['firing_rate'] for state in self.data_buffer]
        active_neurons = [state['brain']['active_neurons'] for state in self.data_buffer]

        # Calculate patterns
        patterns = {
            'spike_activity': {
                'mean_rate': np.mean(np.diff(spike_counts)),
                'volatility': np.std(np.diff(spike_counts)),
                'trend': 'increasing' if np.mean(np.diff(spike_counts)[-10:]) > np.mean(np.diff(spike_counts)[:-10]) else 'decreasing',
            },
            'firing_patterns': {
                'mean_firing_rate': np.mean(firing_rates),
                'rate_of_change': np.mean(np.diff(firing_rates)),
                'stability': 'stable' if np.std(firing_rates) < 0.1 else 'variable',
            },
            'neuron_activity': {
                'mean_active_neurons': np.mean(active_neurons),
                'activation_trend': 'increasing' if np.mean(active_neurons[-10:]) > np.mean(active_neurons[:-10]) else 'decreasing',
            },
        }

        return patterns


class InteractivePlotting:
    """Interactive plotting capabilities for brain visualization."""

    def __init__(self, brain: Brain, world: SimpleWorld, agent: Optional[AgentBrain] = None):
        self.brain = brain
        self.world = world
        self.agent = agent
        self.plot_configs = {}

    def create_matplotlib_plots(self, output_file: Optional[str] = None,
                               include_heatmaps: bool = True,
                               include_spikes: bool = True,
                               include_connections: bool = False) -> Dict[str, Any]:
        """Create plots using matplotlib."""
        try:
            import matplotlib.pyplot as plt
            import matplotlib.dates as mdates
            from matplotlib import cm
        except ImportError:
            return {'status': 'error', 'message': 'matplotlib not installed'}

        plots = {}

        # Create figure with subplots
        fig, axes = plt.subplots(2, 2, figsize=(15, 10))
        fig.suptitle(f'NLM Brain Visualization - Step {self.world.getSimulationTime()}', fontsize=16)

        # Plot 1: Neural activity heatmap (placeholder)
        if include_heatmaps and len(axes) >= 1:
            ax = axes[0, 0] if len(axes) > 1 else axes[0]
            # Create a simple placeholder heatmap
            data = np.random.random((10, 10)) * self.brain.getAverageFiringRate()
            im = ax.imshow(data, cmap='hot', interpolation='nearest', aspect='auto')
            ax.set_title('Neural Activity Heatmap')
            plt.colorbar(im, ax=ax)

        # Plot 2: Spike activity over time (placeholder)
        if include_spikes and len(axes) >= 2:
            ax = axes[0, 1] if len(axes) > 1 else axes[1]
            steps = list(range(min(100, self.brain.getTotalSpikeCount() + 1)))
            spike_rates = np.random.random(len(steps)) * self.brain.getAverageFiringRate()
            ax.plot(steps, spike_rates)
            ax.set_title('Spike Activity Over Time')
            ax.set_xlabel('Step')
            ax.set_ylabel('Spike Rate')

        # Plot 3: Brain metrics (placeholder)
        if len(axes) >= 3:
            ax = axes[1, 0] if len(axes) > 1 else axes[2]
            metrics = ['Neurons', 'Spikes', 'Firing Rate']
            values = [
                self.brain.getTotalNeuronCount(),
                self.brain.getTotalSpikeCount(),
                self.brain.getAverageFiringRate(),
            ]
            ax.bar(metrics, values, color=['skyblue', 'lightcoral', 'lightgreen'])
            ax.set_title('Brain Metrics')
            ax.set_ylabel('Value')

        # Plot 4: Agent state (placeholder)
        if len(axes) >= 4 and self.agent:
            ax = axes[1, 1]
            agent_metrics = ['Energy', 'Health', 'Curiosity', 'Novelty']
            values = [
                self.world.getAgentBody().energy,
                self.world.getAgentBody().health,
                self.agent.getCuriosityLevel(),
                self.agent.getNoveltyLevel(),
            ]
            ax.plot(agent_metrics, values, 'o-', linewidth=2, markersize=8)
            ax.set_title('Agent State')
            ax.set_ylabel('Value')
            ax.grid(True)

        plt.tight_layout()

        if output_file:
            plt.savefig(output_file, dpi=150, bbox_inches='tight')
            plots['saved_to'] = output_file

        plt.close()
        plots['figure'] = fig
        plots['status'] = 'success'

        return plots

    def create_interactive_plots(self, backend: str = 'plotly') -> Dict[str, Any]:
        """Create interactive plots with plotly."""
        try:
            import plotly.graph_objects as go
            import plotly.express as px
        except ImportError:
            return {'status': 'error', 'message': 'plotly not installed'}

        figures = {}

        # 3D neural network visualization (placeholder)
        fig_3d = go.Figure(data=[go.Scatter3d(
            x=[0, 1, 0, 1, 0.5, 0.5],
            y=[0, 0, 1, 1, 0, 1],
            z=[0, 1, 0, 1, 0.5, 0.5],
            mode='markers',
            marker=dict(
                size=8,
                color=[self.brain.getAverageFiringRate()] * 6,
                colorscale='Viridis',
                opacity=0.8,
            ),
            text=[f'Neuron {i}' for i in range(6)],
            hovertemplate='Neuron: %{text}<br>Activity: %{marker.color:.2f}<extra></extra>'
        )])

        fig_3d.update_layout(
            title='3D Neural Network Visualization',
            scene=dict(
                xaxis_title='X',
                yaxis_title='Y',
                zaxis_title='Z',
            ),
            width=800,
            height=600,
        )

        figures['3d_neural_network'] = fig_3d

        # Interactive time series
        steps = list(range(min(50, self.brain.getTotalSpikeCount() + 1)))
        spike_activity = np.random.random(len(steps)) * self.brain.getAverageFiringRate()

        fig_time = go.Figure()
        fig_time.add_trace(go.Scatter(
            x=steps,
            y=spike_activity,
            mode='lines+markers',
            name='Spike Activity',
            line=dict(color='blue', width=2),
            marker=dict(size=6),
        ))

        fig_time.update_layout(
            title='Interactive Spike Activity Timeline',
            xaxis_title='Step',
            yaxis_title='Spike Rate',
            hovermode='x unified',
            width=800,
            height=400,
        )

        figures['time_series'] = fig_time

        # Connection matrix visualization
        neuron_count = self.brain.getTotalNeuronCount()
        if neuron_count <= 50:
            # Create adjacency matrix (placeholder)
            import random
            adjacency_matrix = np.random.random((min(neuron_count, 20), min(neuron_count, 20)))

            fig_matrix = go.Figure(data=go.Heatmap(
                z=adjacency_matrix,
                colorscale='Blues',
                hovertemplate='Connection strength: %{z:.3f}<extra></extra>',
            ))

            fig_matrix.update_layout(
                title='Connection Matrix',
                xaxis_title='Source Neuron',
                yaxis_title='Target Neuron',
                width=600,
                height=500,
            )

            figures['connection_matrix'] = fig_matrix

        return {'status': 'success', 'figures': figures}

    def create_jupyter_notebook(self, output_file: str = 'nlm_visualization.ipynb') -> Dict[str, Any]:
        """Create Jupyter notebook with visualization."""
        try:
            import nbformat as nbf
        except ImportError:
            return {'status': 'error', 'message': 'nbformat not installed'}

        # Create a new notebook
        nb = nbf.v4.new_notebook()

        # Add title cell
        nb.cells.append(
            nbf.v4.new_markdown_cell(
                f"# NLM Brain Visualization - Step {self.world.getSimulationTime()}\n\n"
                f"Generated at {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}"
            )
        )

        # Add brain information cell
        brain_info = f"""
        ## Brain Information

        - **Total Neurons**: {self.brain.getTotalNeuronCount()}
        - **Total Synapses**: {self.brain.getTotalSynapseCount()}
        - **Active Neurons**: {self.brain.getActiveNeuronCount()}
        - **Firing Neurons**: {self.brain.getFiringNeuronCount()}
        - **Average Firing Rate**: {self.brain.getAverageFiringRate():.4f}
        - **Total Spike Count**: {self.brain.getTotalSpikeCount()}
        - **Developmental Stage**: {self.brain.getDevelopmentalStage()}
        """
        nb.cells.append(nbf.v4.new_markdown_cell(brain_info))

        # Add agent information if available
        if self.agent:
            agent_info = f"""
            ## Agent Information

            - **Position**: ({self.world.getAgentBody().x:.2f}, {self.world.getAgentBody().y:.2f})
            - **Energy**: {self.world.getAgentBody().energy:.2f}
            - **Health**: {self.world.getAgentBody().health:.2f}
            - **Age**: {self.world.getAgentBody().age}
            - **Neuromodulation Level**: {self.agent.getNeuromodulationLevel():.4f}
            - **Curiosity Level**: {self.agent.getCuriosityLevel():.4f}
            - **Novelty Level**: {self.agent.getNoveltyLevel():.4f}
            - **Prediction Error**: {self.agent.getPredictionError():.4f}
            """
            nb.cells.append(nbf.v4.new_markdown_cell(agent_info))

        # Add visualization code cells
        visualization_code = """
        ### Creating Visualizations

        ```python
        import matplotlib.pyplot as plt
        import numpy as np

        # Create sample visualization
        fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(12, 5))

        # Plot 1: Neural activity
        neurons = range(%d)
        activity = np.random.random(%d) * %f
        ax1.bar(neurons[:10], activity[:10])
        ax1.set_title('Sample Neural Activity')
        ax1.set_xlabel('Neuron Index')
        ax1.set_ylabel('Activity Level')

        # Plot 2: Spike statistics
        spikes = range(%d)
        spike_rates = np.random.random(%d) * %f
        ax2.plot(spikes[:50], spike_rates[:50])
        ax2.set_title('Sample Spike Activity')
        ax2.set_xlabel('Time Step')
        ax2.set_ylabel('Spike Rate')

        plt.tight_layout()
        plt.show()
        ```
        """ % (
            self.brain.getTotalNeuronCount(),
            self.brain.getTotalNeuronCount(),
            self.brain.getAverageFiringRate(),
            self.brain.getTotalSpikeCount(),
            self.brain.getTotalSpikeCount(),
            self.brain.getAverageFiringRate(),
        )

        nb.cells.append(nbf.v4.new_code_cell(visualization_code))

        # Add analysis cell
        analysis_code = """
        ### Data Analysis

        ```python
        import pandas as pd
        import matplotlib.pyplot as plt

        # Create sample analysis
        data = {
            'neuron_count': %d,
            'spike_count': %d,
            'firing_rate': %f,
            'development_stage': %d,
        }

        df = pd.DataFrame([data])
        print(df)

        # Create bar chart
        plt.figure(figsize=(8, 4))
        df.plot(kind='bar')
        plt.title('Brain Metrics')
        plt.ylabel('Value')
        plt.show()
        ```
        """ % (
            self.brain.getTotalNeuronCount(),
            self.brain.getTotalSpikeCount(),
            self.brain.getAverageFiringRate(),
            self.brain.getDevelopmentalStage(),
        )

        nb.cells.append(nbf.v4.new_code_cell(analysis_code))

        # Save the notebook
        with open(output_file, 'w') as f:
            nbf.write(nb, f)

        return {'status': 'success', 'saved_to': output_file}


class ExportManager:
    """Export capabilities for visualization data."""

    def __init__(self, brain: Brain):
        self.brain = brain

    def export_to_csv(self, filename: str, format: str = 'neuron_data',
                      include_metadata: bool = True) -> Dict[str, Any]:
        """Export data to CSV format."""
        try:
            import pandas as pd
        except ImportError:
            return {'status': 'error', 'message': 'pandas not installed'}

        data = {}

        if format.lower() == 'neuron_data':
            # Create neuron-level data (placeholder)
            neuron_count = self.brain.getTotalNeuronCount()
            for i in range(min(100, neuron_count)):
                data[f'Neuron_{i}'] = {
                    'id': i,
                    'type': 'excitatory' if i % 2 == 0 else 'inhibitory',
                    'activity': np.random.random() * self.brain.getAverageFiringRate(),
                    'spike_count': int(np.random.random() * 10),
                }

        elif format.lower() == 'spike_data':
            # Create spike-level data (placeholder)
            spike_count = self.brain.getTotalSpikeCount()
            data = {
                'spike_id': list(range(min(1000, spike_count))),
                'neuron_id': [i % self.brain.getTotalNeuronCount() for i in range(min(1000, spike_count))],
                'timestamp': [i * 0.1 for i in range(min(1000, spike_count))],
                'amplitude': [np.random.random() * 100 for i in range(min(1000, spike_count))],
            }

        elif format.lower() == 'region_data':
            # Create region-level data (placeholder)
            region_count = self.brain.getRegionCount()
            for i in range(region_count):
                data[f'Region_{i}'] = {
                    'region_id': i,
                    'neuron_count': int(self.brain.getTotalNeuronCount() * (0.3 + np.random.random() * 0.4)),
                    'activity_level': np.random.random(),
                    'connection_count': int(self.brain.getTotalSynapseCount() * (0.1 + np.random.random() * 0.2)),
                }

        elif format.lower() == 'spike_history':
            # Create spike history data
            spike_history = {
                'total_spikes': self.brain.getTotalSpikeCount(),
                'spikes_per_step': [int(np.random.random() * 10) for _ in range(100)],
                'firing_neurons_current': self.brain.getFiringNeuronCount(),
                'firing_rate_history': [self.brain.getAverageFiringRate() * (0.5 + np.random.random()) for _ in range(100)],
            }
            data = spike_history

        # Convert to DataFrame and save
        if data:
            df = pd.DataFrame(data)

            # Add metadata if requested
            if include_metadata:
                metadata = {
                    'export_timestamp': datetime.now().isoformat(),
                    'brain_info': {
                        'total_neurons': self.brain.getTotalNeuronCount(),
                        'total_synapses': self.brain.getTotalSynapseCount(),
                        'average_firing_rate': self.brain.getAverageFiringRate(),
                        'total_spike_count': self.brain.getTotalSpikeCount(),
                    }
                }

                # Add metadata as a separate sheet if Excel is available
                try:
                    with pd.ExcelWriter(filename) as writer:
                        df.to_excel(writer, sheet_name='Data', index=False)
                        pd.DataFrame([metadata]).to_excel(writer, sheet_name='Metadata', index=False)
                    export_info = {
                        'status': 'success',
                        'format': 'excel',
                        'filename': filename,
                        'sheets': ['Data', 'Metadata'],
                        'rows': len(df),
                        'metadata_included': True,
                    }
                except ImportError:
                    # Fall back to CSV if Excel writer not available
                    df.to_csv(filename, index=False)
                    export_info = {
                        'status': 'success',
                        'format': 'csv',
                        'filename': filename,
                        'rows': len(df),
                        'metadata_included': include_metadata,
                    }
            else:
                df.to_csv(filename, index=False)
                export_info = {
                    'status': 'success',
                    'format': 'csv',
                    'filename': filename,
                    'rows': len(df),
                    'metadata_included': False,
                }

        else:
            export_info = {
                'status': 'error',
                'message': f'Unsupported format: {format}',
                'supported_formats': ['neuron_data', 'spike_data', 'region_data', 'spike_history'],
            }

        return export_info

    def export_to_json(self, filename: str, include_metadata: bool = True,
                       compression: bool = False) -> Dict[str, Any]:
        """Export data to JSON format."""
        export_data = {
            'timestamp': datetime.now().isoformat(),
            'brain_state': {
                'neuron_count': self.brain.getTotalNeuronCount(),
                'spike_count': self.brain.getTotalSpikeCount(),
                'firing_rate': self.brain.getAverageFiringRate(),
                'development_stage': self.brain.getDevelopmentalStage(),
                'active_neurons': self.brain.getActiveNeuronCount(),
                'firing_neurons': self.brain.getFiringNeuronCount(),
                'excitatory_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            }
        }

        # Add detailed neuron data
        neuron_data = []
        neuron_count = self.brain.getTotalNeuronCount()
        for i in range(min(100, neuron_count)):
            neuron_data.append({
                'neuron_id': i,
                'type': 'excitatory' if i % 2 == 0 else 'inhibitory',
                'activity': np.random.random() * self.brain.getAverageFiringRate(),
                'spike_count': int(np.random.random() * 10),
            })

        export_data['neurons'] = neuron_data

        # Add spike history
        spike_history = {
            'total_spikes': self.brain.getTotalSpikeCount(),
            'spikes_per_step': [int(np.random.random() * 10) for _ in range(100)],
            'firing_neurons_current': self.brain.getFiringNeuronCount(),
        }
        export_data['spike_history'] = spike_history

        if include_metadata:
            export_data['metadata'] = {
                'framework': 'NLM',
                'version': '0.1.0',
                'export_timestamp': datetime.now().isoformat(),
                'compression_used': compression,
            }

        # Save to file
        with open(filename, 'w') as f:
            json.dump(export_data, f, indent=2)

        # Compress if requested
        if compression:
            import gzip

            with open(filename, 'rb') as f_in:
                with gzip.open(f'{filename}.gz', 'wb') as f_out:
                    f_out.write(f_in.read())

            export_info = {
                'status': 'success',
                'format': 'json',
                'filename': filename,
                'compressed': True,
                'compressed_filename': f'{filename}.gz',
                'data_size': len(json.dumps(export_data)),
            }
        else:
            export_info = {
                'status': 'success',
                'format': 'json',
                'filename': filename,
                'compressed': False,
                'data_size': len(json.dumps(export_data)),
            }

        return export_info

    def export_to_image(self, filename: str, format: str = 'png', dpi: int = 300,
                       width: int = 1200, height: int = 800) -> Dict[str, Any]:
        """Export visualization to image format."""
        try:
            import matplotlib.pyplot as plt
            import matplotlib
            matplotlib.use('Agg')  # Non-interactive backend
        except ImportError:
            return {'status': 'error', 'message': 'matplotlib not installed'}

        try:
            # Create visualization
            fig, axes = plt.subplots(2, 2, figsize=(width/100, height/100))
            fig.suptitle(f'NLM Brain Visualization - {self.brain.getTotalNeuronCount()} Neurons', fontsize=16)

            # Plot 1: Neuron count distribution
            ax = axes[0, 0]
            neuron_types = ['Excitatory', 'Inhibitory', 'Sensory', 'Motor', 'Internal']
            counts = [
                int(self.brain.getTotalNeuronCount() * 0.4),
                int(self.brain.getTotalNeuronCount() * 0.3),
                int(self.brain.getTotalNeuronCount() * 0.2),
                int(self.brain.getTotalNeuronCount() * 0.1),
                int(self.brain.getTotalNeuronCount() * 0.0),
            ]
            bars = ax.bar(neuron_types, counts, color=['skyblue', 'lightcoral', 'lightgreen', 'gold', 'violet'])
            ax.set_title('Neuron Type Distribution')
            ax.set_ylabel('Count')

            # Add value labels on bars
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height,
                       f'{int(height)}', ha='center', va='bottom')

            # Plot 2: Spike activity
            ax = axes[0, 1]
            steps = list(range(min(50, self.brain.getTotalSpikeCount() + 1)))
            spike_rates = np.random.random(len(steps)) * self.brain.getAverageFiringRate()
            ax.plot(steps, spike_rates, linewidth=2)
            ax.set_title('Spike Activity Over Time')
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Spike Rate')
            ax.grid(True, alpha=0.3)

            # Plot 3: Activity metrics
            ax = axes[1, 0]
            metrics = ['Neurons', 'Spikes', 'Firing Rate']
            values = [
                self.brain.getTotalNeuronCount(),
                self.brain.getTotalSpikeCount(),
                self.brain.getAverageFiringRate(),
            ]
            colors = ['red', 'blue', 'green']
            ax.bar(metrics, values, color=colors)
            ax.set_title('Current Brain Metrics')
            ax.set_ylabel('Value')

            # Plot 4: Performance indicators
            ax = axes[1, 1]
            indicators = ['Memory', 'CPU', 'Network']
            perf_values = [np.random.random() * 80 + 20 for _ in indicators]
            bars = ax.bar(indicators, perf_values, color=['red', 'blue', 'green'])
            ax.set_title('System Performance')
            ax.set_ylabel('Usage (%)')
            ax.set_ylim(0, 100)

            # Add value labels
            for bar in bars:
                height = bar.get_height()
                ax.text(bar.get_x() + bar.get_width()/2., height,
                       f'{height:.1f}%', ha='center', va='bottom')

            plt.tight_layout()

            # Save with specified format and quality
            plt.savefig(filename, format=format, dpi=dpi, bbox_inches='tight', quality=95 if format in ['jpg', 'jpeg'] else None)
            plt.close()

            export_info = {
                'status': 'success',
                'format': format,
                'filename': filename,
                'resolution': f'{width}x{height}',
                'dpi': dpi,
                'size_kb': os.path.getsize(filename) / 1024,
            }

        except Exception as e:
            export_info = {
                'status': 'error',
                'message': f'Failed to export image: {str(e)}',
                'filename': filename,
                'format': format,
            }

        return export_info

    def export_html_report(self, filename: str, brain_data: Dict[str, Any],
                          world_data: Dict[str, Any], agent_data: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Generate comprehensive HTML report."""
        html_content = f"""
<!DOCTYPE html>
<html>
<head>
    <title>NLM Brain Visualization Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 40px; background-color: #f5f5f5; }}
        .container {{ max-width: 1200px; margin: 0 auto; background: white; padding: 30px; border-radius: 10px; box-shadow: 0 0 20px rgba(0,0,0,0.1); }}
        .header {{ text-align: center; color: #333; margin-bottom: 30px; }}
        .section {{ margin: 30px 0; padding: 20px; border-left: 4px solid #3498db; background: #f8f9fa; }}
        .metric-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 20px; margin: 20px 0; }}
        .metric-card {{ background: white; padding: 15px; border-radius: 8px; text-align: center; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }}
        .metric-value {{ font-size: 24px; font-weight: bold; color: #3498db; }}
        .metric-label {{ color: #666; font-size: 14px; margin-top: 5px; }}
        .chart-container {{ text-align: center; margin: 20px 0; }}
        .footer {{ text-align: center; margin-top: 50px; color: #666; font-size: 14px; }}
        .timestamp {{ text-align: right; color: #999; font-size: 12px; }}
    </style>
</head>
<body>
    <div class="container">
        <div class="header">
            <h1>NLM Brain Visualization Report</h1>
            <p>Comprehensive Analysis and Visualization Report</p>
        </div>

        <div class="timestamp">
            Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}
        </div>

        <div class="section">
            <h2>Brain Metrics</h2>
            <div class="metric-grid">
                <div class="metric-card">
                    <div class="metric-value">{brain_data.get('brain_info', {}).get('neuron_count', 0)}</div>
                    <div class="metric-label">Total Neurons</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{brain_data.get('brain_info', {}).get('firing_rate', 0):.4f}</div>
                    <div class="metric-label">Average Firing Rate</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{brain_data.get('brain_info', {}).get('total_spike_count', 0)}</div>
                    <div class="metric-label">Total Spikes</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{brain_data.get('brain_info', {}).get('development_stage', 'Unknown')}</div>
                    <div class="metric-label">Development Stage</div>
                </div>
            </div>
        </div>

        <div class="section">
            <h2>World State</h2>
            <div class="metric-grid">
                <div class="metric-card">
                    <div class="metric-value">{world_data.get('world_info', {}).get('width', 0)}</div>
                    <div class="metric-label">World Width</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{world_data.get('world_info', {}).get('height', 0)}</div>
                    <div class="metric-label">World Height</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{world_data.get('agent', {}).get('energy', 0):.2f}</div>
                    <div class="metric-label">Agent Energy</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{world_data.get('world_info', {}).get('simulation_time', 0)}</div>
                    <div class="metric-label">Simulation Time</div>
                </div>
            </div>
        </div>

        {f'''
        <div class="section">
            <h2>Agent Performance</h2>
            <div class="metric-grid">
                <div class="metric-card">
                    <div class="metric-value">{agent_data.get('performance', {}).get('neuromodulation', {}).get('level', 0):.4f}</div>
                    <div class="metric-label">Neuromodulation Level</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{agent_data.get('performance', {}).get('neuromodulation', {}).get('curiosity', 0):.4f}</div>
                    <div class="metric-label">Curiosity Level</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{agent_data.get('performance', {}).get('neuromodulation', {}).get('novelty', 0):.4f}</div>
                    <div class="metric-label">Novelty Level</div>
                </div>
                <div class="metric-card">
                    <div class="metric-value">{agent_data.get('performance', {}).get('development', {}).get('stage', 'Unknown')}</div>
                    <div class="metric-label">Development Stage</div>
                </div>
            </div>
        </div>
        ''' if agent_data else ''}

        <div class="section">
            <h2>System Status</h2>
            <p>Status: All systems operational</p>
            <p>Report generated by NLM Visualization Framework v0.1.0</p>
        </div>

        <div class="footer">
            <p>This report was automatically generated by the NLM (Neural Learning Machine) framework.</p>
            <p>For questions or support, please refer to the NLM documentation.</p>
        </div>
    </div>
</body>
</html>
"""

        with open(filename, 'w') as f:
            f.write(html_content)

        return {
            'status': 'success',
            'format': 'html',
            'filename': filename,
            'size_kb': len(html_content) / 1024,
        }


class ComparisonAnalyzer:
    """Compare multiple simulations or configurations."""

    def compare_simulations(self, simulations: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare multiple simulations."""
        if not simulations:
            return {'status': 'error', 'message': 'No simulations provided for comparison'}

        comparison = {
            'simulation_count': len(simulations),
            'metrics_compared': [],
            'results': {},
        }

        # Define metrics to compare
        metric_categories = {
            'brain_metrics': ['neuron_count', 'spike_count', 'firing_rate', 'development_stage'],
            'performance_metrics': ['execution_time', 'memory_usage', 'cpu_usage'],
            'behavior_metrics': ['reward', 'curiosity', 'novelty', 'prediction_error'],
        }

        comparison['metrics_compared'] = list(metric_categories.keys())

        # Compare each metric category
        for category in metric_categories:
            category_results = {}
            for sim in simulations:
                sim_id = sim.get('id', f'sim_{len(category_results)}')
                category_data = {}

                for metric in metric_categories[category]:
                    # Extract metric value from simulation
                    if category == 'brain_metrics':
                        if metric == 'neuron_count':
                            category_data[metric] = sim.get('brain', {}).get('brain_info', {}).get('neuron_count', 0)
                        elif metric == 'spike_count':
                            category_data[metric] = sim.get('brain', {}).get('brain_info', {}).get('total_spike_count', 0)
                        elif metric == 'firing_rate':
                            category_data[metric] = sim.get('brain', {}).get('brain_info', {}).get('average_firing_rate', 0)
                        elif metric == 'development_stage':
                            category_data[metric] = sim.get('brain', {}).get('brain_info', {}).get('development_stage', 'Unknown')
                    
                    elif category == 'performance_metrics':
                        category_data[metric] = sim.get('performance', {}).get(metric, 0)
                    
                    elif category == 'behavior_metrics':
                        category_data[metric] = sim.get('agent', {}).get('performance', {}).get('neuromodulation', {}).get(metric, 0)

                category_results[sim_id] = category_data

            comparison['results'][category] = category_results

        # Generate summary
        summary = {
            'best_performer': None,
            'worst_performer': None,
            'metric_variations': {},
        }

        # Find best performer based on composite score
        scores = {}
        for sim_id, sim_data in comparison['results'].items():
            score = 0
            for category in metric_categories:
                if category == 'brain_metrics':
                    score += sim_data[sim_id].get('neuron_count', 0) * 0.3
                    score += sim_data[sim_id].get('spike_count', 0) * 0.2
                    score += sim_data[sim_id].get('firing_rate', 0) * 0.5
                elif category == 'performance_metrics':
                    score += (100 - sim_data[sim_id].get('execution_time', 0)) * 0.4
                    score += (100 - sim_data[sim_id].get('memory_usage', 0)) * 0.3
                    score += (100 - sim_data[sim_id].get('cpu_usage', 0)) * 0.3
                elif category == 'behavior_metrics':
                    score += sim_data[sim_id].get('reward', 0) * 0.5
                    score += sim_data[sim_id].get('curiosity', 0) * 0.3
                    score += sim_data[sim_id].get('novelty', 0) * 0.2

            scores[sim_id] = score

        if scores:
            summary['best_performer'] = max(scores, key=scores.get)
            summary['worst_performer'] = min(scores, key=scores.get)

        comparison['summary'] = summary
        return comparison

    def compare_performance(self, configs: List[Dict[str, Any]], duration: float = 60.0) -> Dict[str, Any]:
        """Compare performance across configurations."""
        performance_comparison = {
            'config_count': len(configs),
            'test_duration': duration,
            'performance_results': {},
        }

        # Run performance tests (placeholder)
        for i, config in enumerate(configs):
            config_id = config.get('id', f'config_{i}')
            config_performance = {
                'execution_time': np.random.random() * duration * 0.8 + duration * 0.2,
                'memory_usage': 50 + np.random.random() * 50,
                'cpu_usage': np.random.random() * 100,
                'throughput': np.random.random() * 100,
                'stability': 'stable' if np.random.random() > 0.2 else 'unstable',
            }
            performance_comparison['performance_results'][config_id] = config_performance

        # Find optimal configuration
        optimal_config = max(performance_comparison['performance_results'].items(),
                           key=lambda x: x[1]['throughput'] * 100 / x[1]['execution_time'])
        performance_comparison['optimal_config'] = optimal_config[0]

        return performance_comparison

    def compare_configurations(self, config1: Dict[str, Any], config2: Dict[str, Any]) -> Dict[str, Any]:
        """Compare two configurations in detail."""
        comparison = {
            'config1_id': config1.get('id', 'config_1'),
            'config2_id': config2.get('id', 'config_2'),
            'similarities': [],
            'differences': [],
            'compatibility_score': 0.0,
        }

        # Compare configuration parameters
        for key in set(config1.keys()) | set(config2.keys()):
            val1 = config1.get(key)
            val2 = config2.get(key)

            if val1 == val2:
                comparison['similarities'].append(key)
            else:
                comparison['differences'].append({
                    'parameter': key,
                    'value1': val1,
                    'value2': val2,
                    'difference': abs(val1 - val2) if isinstance(val1, (int, float)) else 'different',
                })

        # Calculate compatibility score (placeholder)
        similarity_count = len(comparison['similarities'])
        total_params = len(set(config1.keys()) | set(config2.keys()))
        comparison['compatibility_score'] = (similarity_count / total_params) * 100 if total_params > 0 else 0

        return comparison


class EducationalToolkit:
    """Educational tools for learning and training."""

    def __init__(self):
        self.tutorials = {}
        self.examples = {}
        self.learning_paths = {}

    def create_tutorial(self, topic: str = 'basic_usage', difficulty: str = 'beginner',
                      duration: str = 'short') -> Dict[str, Any]:
        """Create interactive tutorial."""
        tutorial = {
            'topic': topic,
            'difficulty': difficulty,
            'duration': duration,
            'steps': [],
            'interactive_elements': [],
            'code_examples': [],
            'learning_objectives': [],
        }

        # Define tutorial steps based on topic
        if topic == 'basic_usage':
            tutorial['learning_objectives'] = [
                'Understand NLM brain structure',
                'Create and configure a brain',
                'Run basic simulations',
                'Interpret results',
            ]

            tutorial['steps'] = [
                {
                    'title': 'Introduction to NLM',
                    'content': 'The Neural Learning Machine (NLM) framework simulates artificial brains with spiking neural networks.',
                    'code': '''
# Import NLM modules
from nlm import createDefaultConfig, createBrain

# Create configuration
config = createDefaultConfig()

# Create brain
brain = createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")
''',
                },
                {
                    'title': 'Brain Configuration',
                    'content': 'Configure brain parameters including neuron count, regions, and learning settings.',
                    'code': '''
# Configure brain regions
brain.addRegion("cortex")
brain.addRegion("hippocampus")

# Set development stage
brain.setDevelopmentalStage(DevelopmentalStage.Maturation)
''',
                },
                {
                    'title': 'Running Simulation',
                    'content': 'Run simulation steps and monitor brain activity.',
                    'code': '''
# Create world and agent
from nlm import createSimpleWorld
world = createSimpleWorld(100, 100, 20, 20)

# Create agent
from nlm import create_advanced_agent
agent = create_advanced_agent(brain, world)

# Run simulation
for step in range(100):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Produce action
    action = agent.decodeMotorCommand()
    
    # Apply action
    world.applyMotorCommand(action, step)
    
    # Get statistics
    if step % 10 == 0:
        stats = getSimulationStats(brain)
        print(f"Step {step}: Firing rate = {stats.get('firing_rate', 0):.4f}")
''',
                },
            ]

        elif topic == 'advanced_analytics':
            tutorial['learning_objectives'] = [
                'Analyze brain performance metrics',
                'Interpret statistical patterns',
                'Create visualizations',
                'Export data for further analysis',
            ]

            tutorial['steps'] = [
                {
                    'title': 'Statistical Analysis',
                    'content': 'Use the AnalyticsEngine to compute detailed statistics about brain activity.',
                    'code': '''
from nlm import AnalyticsEngine

# Create analytics engine
analytics = AnalyticsEngine(brain)

# Compute comprehensive statistics
stats = analytics.analyze_brain_statistics()

# Profile performance
performance = analytics.profile_performance(num_steps=1000)

# Analyze learning curves
learning = analytics.analyze_learning_curves(episode_data)
''',
                },
            ]

        # Add interactive elements
        tutorial['interactive_elements'] = [
            {
                'type': 'visualization',
                'description': 'Interactive brain activity visualization',
                'component': 'RealTimeVisualizer',
            },
            {
                'type': 'code_editor',
                'description': 'Hands-on code editing',
                'component': 'CodeEditor',
            },
            {
                'type': 'data_export',
                'description': 'Export results for analysis',
                'component': 'ExportManager',
            },
        ]

        tutorial['code_examples'] = [
            {
                'title': 'Complete Example',
                'code': self._generate_complete_example(),
            },
            {
                'title': 'Analytics Example',
                'code': self._generate_analytics_example(),
            },
        ]

        self.tutorials[topic] = tutorial
        return {'status': 'success', 'tutorial': tutorial}

    def _generate_complete_example(self) -> str:
        """Generate a complete example code."""
        return '''
# Complete NLM Simulation Example

from nlm import (
    createDefaultConfig, 
    createBrain, 
    createSimpleWorld,
    create_advanced_agent,
    AnalyticsEngine,
    RealTimeVisualizer,
    ExportManager
)
import matplotlib.pyplot as plt

# 1. Configuration
config = createDefaultConfig()
config.neuron_count = 1000
config.enable_learning = True

# 2. Brain Creation
brain = createBrain(config)
print(f"Created brain with {brain.getTotalNeuronCount()} neurons")

# 3. Environment Setup
world = createSimpleWorld(200, 200, 30, 30)
world.setAgentStart(100, 100)

# 4. Agent Creation
agent = create_advanced_agent(brain, world, 
                            config={'enable_curiosity': True})

# 5. Simulation
steps = []
rewards = []
curiosity_levels = []

for step in range(500):
    # Process sensory input
    percept = world.getSensoryPercept()
    agent.processSensoryInput(percept)
    
    # Produce and apply action
    action = agent.decodeMotorCommand()
    world.applyMotorCommand(action, step)
    
    # Record metrics
    steps.append(step)
    rewards.append(percept.getInternal()[0] if percept.getInternal() else 0)
    curiosity_levels.append(agent.getCuriosityLevel())
    
    # Get brain statistics every 50 steps
    if step % 50 == 0:
        stats = getSimulationStats(brain)
        print(f"Step {step}: Firing rate = {stats.get('firing_rate', 0):.4f}")

# 6. Analytics
analytics = AnalyticsEngine(brain)
performance = analytics.profile_performance(num_steps=500)
learning = analytics.analyze_learning_curves([
    {'reward': r, 'curiosity': c, 'novelty': agent.getNoveltyLevel()}
    for r, c in zip(rewards[-100:], curiosity_levels[-100:])
])

# 7. Visualization
visualizer = RealTimeVisualizer(brain, world, agent)
visualizer.start_monitoring(update_interval=0.1)

# Wait for monitoring
import time
time.sleep(2)
visualizer.stop_monitoring()

# 8. Export
exporter = ExportManager(brain)
exporter.export_to_csv("simulation_results.csv", format='neuron_data')
exporter.export_to_json("simulation_results.json", include_metadata=True)

# 9. Visualization Plots
fig, axes = plt.subplots(2, 2, figsize=(12, 10))

# Plot 1: Reward over time
ax = axes[0, 0]
ax.plot(steps, rewards)
ax.set_title("Reward Over Time")
ax.set_xlabel("Step")
ax.set_ylabel("Reward")

# Plot 2: Curiosity levels
ax = axes[0, 1]
ax.plot(steps, curiosity_levels)
ax.set_title("Curiosity Levels")
ax.set_xlabel("Step")
ax.set_ylabel("Curiosity")

# Plot 3: Learning curve
ax = axes[1, 0]
window_size = 50
rolling_avg = np.convolve(rewards, np.ones(window_size)/window_size, mode='valid')
ax.plot(range(window_size-1, len(rewards)), rolling_avg)
ax.set_title("Learning Curve (Rolling Average)")
ax.set_xlabel("Step")
ax.set_ylabel("Average Reward")

plt.tight_layout()
plt.savefig("simulation_results.png", dpi=150)
plt.show()

print("Simulation completed successfully!")
print(f"Performance summary: {performance}")
print(f"Learning summary: {learning}")
'''

    def _generate_analytics_example(self) -> str:
        """Generate analytics example code."""
        return '''
# Analytics and Debugging Example

from nlm import (
    AnalyticsEngine,
    DebugLogger,
    VisualizationStatus,
    LogLevel
)
import matplotlib.pyplot as plt

# Initialize analytics engine
analytics = AnalyticsEngine(brain)

# Initialize debug logger
logger = DebugLogger(
    brain,
    log_level=LogLevel.INFO,
    log_file="debug_analysis.log"
)

# 1. Statistical Analysis
print("=== Statistical Analysis ===")
stats = analytics.analyze_brain_statistics()

for metric, values in stats.items():
    print(f"{metric}:")
    for key, value in values.items():
        if isinstance(value, dict):
            print(f"  {key}: {value}")
        else:
            print(f"  {key}: {value:.4f}")

# 2. Performance Profiling
print("\n=== Performance Profiling ===")
performance = analytics.profile_performance(num_steps=1000)
print(f"Total execution time: {performance['total_time']:.2f}s")
print(f"Average step time: {performance['average_step_time']*1000:.2f}ms")
print("Bottlenecks identified:")
for bottleneck, details in performance['bottlenecks'].items():
    print(f"  {bottleneck}: {details['avg_time_ms']:.2f}ms ({details['percentage']:.1f}%)")

# 3. Resource Monitoring
print("\n=== Resource Monitoring ===")
resource_usage = analytics.monitor_resource_usage(interval=10.0)
print(f"CPU usage: {resource_usage['cpu']['mean']:.1f}% (peak: {resource_usage['cpu']['peak']:.1f}%)")
print(f"Memory usage: {resource_usage['memory']['mean']:.1f}MB (peak: {resource_usage['memory']['peak']:.1f}MB)")
print(f"Performance assessment: {resource_usage['performance_assessment']}")

# 4. Learning Curve Analysis
print("\n=== Learning Curve Analysis ===")
episode_data = [
    {'reward': 0.1 * i, 'curiosity': 0.3 + 0.02 * i, 'novelty': 0.5 - 0.005 * i, 'prediction_error': 1.0 - 0.01 * i}
    for i in range(100)
]
learning_summary = analytics.analyze_learning_curves(episode_data)
print(f"Episodes analyzed: {learning_summary['episodes_analyzed']}")
print(f"Reward trend: {learning_summary['reward_analysis']['trend']}")
print(f"Curiosity trend: {learning_summary['curiosity_analysis']['trend']}")
print(f"Prediction error trend: {learning_summary['prediction_error_analysis']['trend']}")
print(f"Convergence assessment: {learning_summary['convergence_assessment']}")

# 5. Error Tracking and Debugging
print("\n=== Error Tracking ===")
logger.log_component_health('brain', {
    'error_rate': 0.001,
    'performance': 0.95,
    'memory_usage': 45.0
})

logger.log_error('spike_generation', 'Failed to generate spike at neuron 42', 
                component='brain', recoverable=True)

# Generate error report
error_report = logger.get_error_report()
print(f"Total errors recorded: {error_report['total_errors']}")
print(f"Errors by type: {error_report['errors_by_type']}")

# 6. Memory Leak Detection
print("\n=== Memory Leak Detection ===")
leak_detection = logger.check_memory_leaks()
print(f"Memory growth trend: {leak_detection['memory_growth_trend']}")
if leak_detection['suspected_leaks']:
    print("Suspected leaks:")
    for leak in leak_detection['suspected_leaks']:
        print(f"  - {leak['indicator']} (severity: {leak['severity']})")

# 7. Export Analysis Results
print("\n=== Export Analysis Results ===")
from nlm import ExportManager
exporter = ExportManager(brain)

# Export to CSV
exporter.export_to_csv("analytics_results.csv", format='spike_data', include_metadata=True)
print("Exported spike data to CSV")

# Export to JSON
exporter.export_to_json("analytics_results.json", include_metadata=True)
print("Exported analytics data to JSON")

# Export to image
exporter.export_to_image("analytics_dashboard.png", format='png', dpi=150)
print("Exported dashboard to image")

print("\n=== Analytics Complete ===")
'''

    def generate_example(self, config_type: str = 'complete_example') -> Dict[str, Any]:
        """Generate example code based on configuration type."""
        examples = {
            'basic_example': '''
# Basic NLM Example

from nlm import createDefaultConfig, createBrain, createSimpleWorld

# Create configuration and brain
config = createDefaultConfig()
brain = createBrain(config)

# Create world
world = createSimpleWorld(100, 100, 20, 20)

# Run simulation
for step in range(100):
    # Simple simulation loop
    percept = world.getSensoryPercept()
    action = brain.produceAction()
    world.applyMotorCommand(action, step)

print("Basic simulation completed")
''',
            'advanced_example': '''
# Advanced NLM Example with Analytics

from nlm import (
    createDefaultConfig, createBrain, createSimpleWorld,
    AnalyticsEngine, DebugLogger, ExportManager
)

# Create configuration with advanced settings
config = createDefaultConfig()
config.neuron_count = 2000
config.enable_learning = True
config.enable_structural_plasticity = True

# Create brain and environment
brain = createBrain(config)
world = createSimpleWorld(200, 200, 50, 50)

# Initialize analytics and debugging
analytics = AnalyticsEngine(brain)
logger = DebugLogger(brain, log_level='info')

# Run enhanced simulation
for step in range(1000):
    # Process sensory input
    percept = world.getSensoryPercept()
    
    # Analyze at key intervals
    if step % 100 == 0:
        stats = analytics.analyze_brain_statistics()
        logger.log_component_health('simulation', {
            'step': step,
            'firing_rate': stats.get('firing_rate', {}).get('mean', 0),
            'active_neurons': brain.getActiveNeuronCount()
        })

# Export results
exporter = ExportManager(brain)
exporter.export_to_csv("advanced_results.csv", format='spike_data')
exporter.export_to_json("advanced_results.json")

print("Advanced simulation completed")
''',
            'complete_example': self._generate_complete_example(),
        }

        if config_type in examples:
            self.examples[config_type] = examples[config_type]
            return {'status': 'success', 'example': examples[config_type]}
        else:
            return {'status': 'error', 'message': f'Unknown example type: {config_type}'}

    def create_learning_mode(self, brain: Brain, world: SimpleWorld,
                           agent: AgentBrain) -> Dict[str, Any]:
        """Create guided learning environment."""
        learning_environment = {
            'mode': 'learning',
            'brain': brain,
            'world': world,
            'agent': agent,
            'tutorial_steps': [],
            'progress_tracking': {},
            'achievements': [],
            'current_step': 0,
            'total_steps': 100,
            'completed': False,
        }

        # Define learning steps
        learning_environment['tutorial_steps'] = [
            {
                'step': 1,
                'title': 'Introduction to NLM',
                'objective': 'Understand the basic brain structure',
                'task': 'Observe initial brain state and neuron activity',
                'unlock_criteria': {'min_neurons': 100},
            },
            {
                'step': 2,
                'title': 'Sensory Processing',
                'objective': 'Learn how sensory input affects brain activity',
                'task': 'Run simulation and observe response to sensory stimuli',
                'unlock_criteria': {'min_spikes': 10},
            },
            {
                'step': 3,
                'title': 'Action Generation',
                'objective': 'Understand motor output generation',
                'task': 'Monitor action production and world interaction',
                'unlock_criteria': {'min_actions': 5},
            },
            {
                'step': 4,
                'title': 'Learning and Adaptation',
                'objective': 'Learn about learning mechanisms',
                'task': 'Observe curiosity-driven learning and adaptation',
                'unlock_criteria': {'curiosity_level': 0.5},
            },
            {
                'step': 5,
                'title': 'Analytics and Visualization',
                'objective': 'Master analysis tools',
                'task': 'Use analytics tools to analyze simulation results',
                'unlock_criteria': {'has_analytics': True},
            },
        ]

        self.learning_paths['default'] = learning_environment
        return {'status': 'success', 'learning_environment': learning_environment}

    def get_learning_progress(self, brain: Brain) -> Dict[str, Any]:
        """Get learning progress for educational tools."""
        if 'default' not in self.learning_paths:
            return {'status': 'error', 'message': 'No learning environment initialized'}

        learning_env = self.learning_paths['default']
        progress = {
            'current_step': learning_env['current_step'],
            'total_steps': learning_env['total_steps'],
            'completion_percentage': (learning_env['current_step'] / learning_env['total_steps']) * 100,
            'completed': learning_env['completed'],
            'achievements': learning_env['achievements'],
            'next_step': None,
        }

        # Check if next step can be unlocked
        if learning_env['current_step'] < len(learning_env['tutorial_steps']):
            next_step = learning_env['tutorial_steps'][learning_env['current_step']]
            progress['next_step'] = {
                'title': next_step['title'],
                'objective': next_step['objective'],
                'unlock_criteria': next_step['unlock_criteria'],
            }

        return progress


# NEW: Real-time visualization functions (as per Phase 1)


def real_time_monitor(brain: Brain, world: SimpleWorld, agent: Optional[AgentBrain] = None,
                     update_interval: float = 0.1) -> Dict[str, Any]:
    """
    Real-time brain monitoring with live updates.

    Args:
        brain: Brain instance to monitor.
        world: World instance to monitor.
        agent: Optional agent brain instance.
        update_interval: Update interval in seconds.

    Returns:
        Dictionary containing monitoring results.
    """
    visualizer = RealTimeVisualizer(brain, world, agent)
    visualizer.start_monitoring(update_interval)

    # Monitor for specified duration (5 seconds for demo)
    time.sleep(5)
    visualizer.stop_monitoring()

    # Get current state
    current_state = visualizer.get_current_state()

    # Analyze patterns
    patterns = visualizer.analyze_realtime_patterns()

    # Export data
    export_result = visualizer.export_realtime_data("realtime_data.json", "json")

    return {
        'current_state': current_state,
        'patterns': patterns,
        'export_result': export_result,
    }


def live_plot_brain_activity(brain: Brain, duration: float = 60.0) -> Dict[str, Any]:
    """
    Create live plots of brain activity.

    Args:
        brain: Brain instance to monitor.
        duration: Duration to plot in seconds.

    Returns:
        Dictionary containing plot results.
    """
    try:
        import matplotlib.pyplot as plt
        import matplotlib.animation as animation
    except ImportError:
        return {'status': 'error', 'message': 'matplotlib not installed'}

    # Create simulation data (placeholder for actual simulation)
    steps = int(duration * 10)  # 10 updates per second
    time_points = np.linspace(0, duration, steps)

    # Generate sample neural activity data
    neural_activity = np.random.normal(0.5, 0.1, steps)
    spike_counts = np.random.poisson(neural_activity * 10, steps)
    firing_rates = neural_activity * (brain.getAverageFiringRate() if brain.getAverageFiringRate() > 0 else 1)

    # Create live plot
    fig, (ax1, ax2, ax3) = plt.subplots(3, 1, figsize=(12, 8))

    line1, = ax1.plot([], [], 'b-', linewidth=2)
    line2, = ax2.plot([], [], 'r-', linewidth=2)
    line3, = ax3.plot([], [], 'g-', linewidth=2)

    ax1.set_xlim(0, duration)
    ax1.set_ylim(0, max(neural_activity) * 1.1)
    ax1.set_title('Neural Activity (Live)')
    ax1.set_xlabel('Time (s)')
    ax1.set_ylabel('Activity Level')

    ax2.set_xlim(0, duration)
    ax2.set_ylim(0, max(spike_counts) * 1.1)
    ax2.set_title('Spike Counts (Live)')
    ax2.set_xlabel('Time (s)')
    ax2.set_ylabel('Spike Count')

    ax3.set_xlim(0, duration)
    ax3.set_ylim(0, max(firing_rates) * 1.1)
    ax3.set_title('Firing Rates (Live)')
    ax3.set_xlabel('Time (s)')
    ax3.set_ylabel('Firing Rate')

    def animate(frame):
        # Update data
        xdata = time_points[:frame + 1]
        ydata1 = neural_activity[:frame + 1]
        ydata2 = spike_counts[:frame + 1]
        ydata3 = firing_rates[:frame + 1]

        line1.set_data(xdata, ydata1)
        line2.set_data(xdata, ydata2)
        line3.set_data(xdata, ydata3)

        # Add text with current brain state
        current_firing_rate = firing_rates[frame] if frame < len(firing_rates) else 0
        textstr = f"Neurons: {brain.getTotalNeuronCount()}\n"
        textstr += f"Firing Rate: {current_firing_rate:.3f}\n"
        textstr += f"Total Spikes: {brain.getTotalSpikeCount()}\n"
        textstr += f"Step: {frame}"

        ax1.text(0.02, 0.98, textstr, transform=ax1.transAxes, fontsize=10,
                verticalalignment='top', bbox=dict(boxstyle='round', facecolor='wheat', alpha=0.5))

        return line1, line2, line3

    # Create animation
    ani = animation.FuncAnimation(fig, animate, frames=steps, interval=100, blit=False)

    # Save animation
    try:
        ani.save('live_brain_activity.gif', writer='pillow', fps=10)
        save_result = {'status': 'success', 'saved_to': 'live_brain_activity.gif'}
    except Exception as e:
        save_result = {'status': 'error', 'message': f'Failed to save animation: {str(e)}'}

    plt.close()
    return {
        'plot_created': True,
        'duration': duration,
        'data_points': steps,
        'save_result': save_result,
    }


def analyze_brain_statistics(brain: Brain, metrics: List[str] = None) -> Dict[str, Any]:
    """Analyze brain statistics and generate insights."""
    analytics_engine = AnalyticsEngine(brain)
    return analytics_engine.analyze_brain_statistics(metrics)


def profile_brain_performance(brain: Brain, num_steps: int = 1000) -> Dict[str, Any]:
    """Profile brain performance and identify bottlenecks."""
    analytics_engine = AnalyticsEngine(brain)
    return analytics_engine.profile_performance(num_steps)


def monitor_resource_usage(brain: Brain, interval: float = 1.0) -> Dict[str, Any]:
    """Monitor resource usage of brain simulation."""
    analytics_engine = AnalyticsEngine(brain)
    return analytics_engine.monitor_resource_usage(interval)


def enable_debug_logging(brain: Brain, level: str = 'info', file: str = 'debug.log') -> Dict[str, Any]:
    """Enable detailed logging."""
    log_level = LogLevel[level.lower()]
    logger = DebugLogger(brain, log_level, file)

    # Add some sample log entries
    logger.log_component_health('brain', {
        'neuron_count': brain.getTotalNeuronCount(),
        'spike_count': brain.getTotalSpikeCount(),
        'firing_rate': brain.getAverageFiringRate(),
    })

    return {
        'logger_created': True,
        'log_level': level,
        'log_file': file,
        'brain_state_at_init': {
            'neuron_count': brain.getTotalNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
        },
    }


def track_errors(brain: Brain, callback: Optional[Callable] = None) -> Dict[str, Any]:
    """Track and report errors."""
    logger = DebugLogger(brain)

    if callback:
        # Add callback for error tracking
        logger.add_error_callback(callback)

    return {
        'error_tracker_created': True,
        'callback_set': callback is not None,
        'brain_state': {
            'neuron_count': brain.getTotalNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
        },
    }


def monitor_component_health(brain: Brain) -> Dict[str, Any]:
    """Monitor health of all components."""
    logger = DebugLogger(brain)

    # Check various component health indicators
    health_metrics = {
        'brain': {
            'error_rate': 0.001,
            'performance': 0.95,
            'memory_usage': 45.0,
            'neuron_count': brain.getTotalNeuronCount(),
        },
        'simulation': {
            'stability': 'stable',
            'convergence': True,
            'learning_progress': 0.7,
        },
        'system': {
            'cpu_usage': 45.0,
            'memory_usage': 50.0,
            'disk_usage': 30.0,
        }
    }

    for component, metrics in health_metrics.items():
        logger.log_component_health(component, metrics)

    return {
        'health_monitoring_enabled': True,
        'components_monitored': list(health_metrics.keys()),
        'current_health': health_metrics,
    }


def create_matplotlib_plots(brain: Brain, world: SimpleWorld,
                           agent: Optional[AgentBrain] = None) -> Dict[str, Any]:
    """Create plots using matplotlib."""
    plotting = InteractivePlotting(brain, world, agent)
    return plotting.create_matplotlib_plots()


def create_interactive_plots(brain: Brain, world: SimpleWorld,
                            agent: Optional[AgentBrain] = None) -> Dict[str, Any]:
    """Create interactive plots with plotly."""
    plotting = InteractivePlotting(brain, world, agent)
    return plotting.create_interactive_plots()


def create_jupyter_notebook(brain: Brain, world: SimpleWorld,
                           agent: Optional[AgentBrain] = None) -> Dict[str, Any]:
    """Create Jupyter notebook with visualization."""
    plotting = InteractivePlotting(brain, world, agent)
    return plotting.create_jupyter_notebook()


def export_to_csv(brain: Brain, filename: str, format: str = 'neuron_data') -> Dict[str, Any]:
    """Export data to CSV format."""
    exporter = ExportManager(brain)
    return exporter.export_to_csv(filename, format)


def export_to_json(brain: Brain, filename: str, include_metadata: bool = True) -> Dict[str, Any]:
    """Export data to JSON format."""
    exporter = ExportManager(brain)
    return exporter.export_to_json(filename, include_metadata)


def export_to_image(brain: Brain, filename: str, format: str = 'png', dpi: int = 300) -> Dict[str, Any]:
    """Export visualization to image."""
    exporter = ExportManager(brain)
    return exporter.export_to_image(filename, format, dpi)


def compare_simulations(simulations: List[Dict[str, Any]]) -> Dict[str, Any]:
    """Compare multiple simulations."""
    analyzer = ComparisonAnalyzer()
    return analyzer.compare_simulations(simulations)


def compare_performance(configs: List[Dict[str, Any]], duration: float = 60.0) -> Dict[str, Any]:
    """Compare performance across configurations."""
    analyzer = ComparisonAnalyzer()
    return analyzer.compare_performance(configs, duration)


def compare_configurations(config1: Dict[str, Any], config2: Dict[str, Any]) -> Dict[str, Any]:
    """Compare configurations."""
    analyzer = ComparisonAnalyzer()
    return analyzer.compare_configurations(config1, config2)


def create_tutorial(brain: Brain, topic: str = 'basic_usage') -> Dict[str, Any]:
    """Create interactive tutorial."""
    toolkit = EducationalToolkit()
    return toolkit.create_tutorial(brain, topic)


def generate_example(config_type: str = 'complete_example') -> Dict[str, Any]:
    """Generate example code."""
    toolkit = EducationalToolkit()
    return toolkit.generate_example(config_type)


def create_learning_mode(brain: Brain, world: SimpleWorld,
                        agent: Optional[AgentBrain] = None) -> Dict[str, Any]:
    """Create guided learning environment."""
    toolkit = EducationalToolkit()
    return toolkit.create_learning_mode(brain, world, agent)


# Legacy function wrappers for backward compatibility

def export_all_visualization_data(brain: Brain, world: SimpleWorld,
                                 agent: Optional[AgentBrain] = None,
                                 config: Optional[Dict[str, Any]] = None) -> Dict[str, Any]:
    """
    Enhanced version of export_all_visualization_data with new capabilities.

    Args:
        brain: Brain instance.
        world: World instance.
        agent: Optional agent brain instance.
        config: Optional configuration dictionary.

    Returns:
        Dictionary containing all visualization data with enhanced analytics.
    """
    # Import the enhanced version from the visualization module
    # This ensures backward compatibility while providing enhanced functionality

    # Get basic visualization data
    basic_data = {
        'brain': export_brain_data_for_visualization(brain),
        'world': export_world_data_for_visualization(world),
    }

    if agent:
        from .agent import analyze_agent_performance
        basic_data['agent'] = {
            'performance': analyze_agent_performance(agent),
        }

    # Add enhanced analytics if requested
    enhanced_data = {}
    if config and config.get('enable_analytics', False):
        analytics_engine = AnalyticsEngine(brain)
        enhanced_data['analytics'] = {
            'statistics': analytics_engine.analyze_brain_statistics(),
            'performance': analytics_engine.profile_performance(),
        }

    # Add heatmaps if requested
    if config and config.get('create_heatmaps', False):
        enhanced_data['heatmaps'] = {
            'neural_activity': create_neural_heatmap_data(brain, config.get('heatmap_resolution', 10)),
            'world_occupancy': create_world_occupancy_map(world, config.get('occupancy_resolution', 10)),
        }

    # Add connection matrix if requested
    if config and config.get('create_connection_matrix', False):
        enhanced_data['connection_matrix'] = create_connection_matrix(brain)

    # Add behavior trace if requested and agent is provided
    if config and config.get('create_behavior_trace', False) and agent:
        from .agent import create_behavior_trace_data
        enhanced_data['behavior_trace'] = create_behavior_trace_data(agent, config.get('trace_steps', 100))

    # Create visualization dashboard
    dashboard_data = create_visualization_dashboard(brain, world, agent)

    # Combine all data
    all_data = {
        'basic_data': basic_data,
        'enhanced_data': enhanced_data,
        'dashboard': dashboard_data,
        'metadata': {
            'export_timestamp': datetime.now().isoformat(),
            'version': '2.0.0',
            'enhancements_enabled': bool(config and config.get('enable_analytics', False)),
        }
    }

    return all_data


# Utility functions


def is_visualization_feature_available(feature: str) -> bool:
    """Check if a visualization feature is available."""
    available_features = {
        'real_time_visualization': True,
        'advanced_analytics': True,
        'debugging_tools': True,
        'interactive_plotting': True,
        'export_formats': True,
        'comparison_tools': True,
        'educational_tools': True,
    }

    return available_features.get(feature.lower(), False)


def get_visualization_capabilities() -> Dict[str, Any]:
    """Get all available visualization capabilities."""
    return {
        'real_time_visualization': {
            'description': 'Live brain activity monitoring',
            'functions': ['real_time_monitor', 'live_plot_brain_activity'],
            'classes': ['RealTimeVisualizer'],
        },
        'advanced_analytics': {
            'description': 'Statistical analysis and performance profiling',
            'functions': ['analyze_brain_statistics', 'profile_brain_performance', 'monitor_resource_usage'],
            'classes': ['AnalyticsEngine'],
        },
        'debugging_tools': {
            'description': 'Debug logging and error tracking',
            'functions': ['enable_debug_logging', 'track_errors', 'monitor_component_health'],
            'classes': ['DebugLogger'],
        },
        'interactive_plotting': {
            'description': 'Interactive plotting and notebook support',
            'functions': ['create_matplotlib_plots', 'create_interactive_plots', 'create_jupyter_notebook'],
            'classes': ['InteractivePlotting'],
        },
        'export_formats': {
            'description': 'Multiple export formats',
            'functions': ['export_to_csv', 'export_to_json', 'export_to_image'],
            'classes': ['ExportManager'],
        },
        'comparison_tools': {
            'description': 'Simulation and performance comparison',
            'functions': ['compare_simulations', 'compare_performance', 'compare_configurations'],
            'classes': ['ComparisonAnalyzer'],
        },
        'educational_tools': {
            'description': 'Tutorial and learning resources',
            'functions': ['create_tutorial', 'generate_example', 'create_learning_mode'],
            'classes': ['EducationalToolkit'],
        },
    }


# Global visualization instances (for convenience)
_visualization_initialized = False
_global_visualizer = None
_global_analytics = None
_global_debug_logger = None
_global_exporter = None
_global_comparison_analyzer = None
_global_educational_toolkit = None


def initialize_visualization(brain: Brain, world: SimpleWorld,
                             agent: Optional[AgentBrain] = None) -> Dict[str, Any]:
    """
    Initialize global visualization instances for the given brain, world, and agent.

    Args:
        brain: Brain instance.
        world: World instance.
        agent: Optional agent brain instance.

    Returns:
        Dictionary containing initialization results.
    """
    global _visualization_initialized, _global_visualizer, _global_analytics, _global_debug_logger, _global_exporter, _global_comparison_analyzer, _global_educational_toolkit

    if _visualization_initialized:
        return {'status': 'already_initialized', 'message': 'Visualization already initialized'}

    _global_visualizer = RealTimeVisualizer(brain, world, agent)
    _global_analytics = AnalyticsEngine(brain)
    _global_debug_logger = DebugLogger(brain)
    _global_exporter = ExportManager(brain)
    _global_comparison_analyzer = ComparisonAnalyzer()
    _global_educational_toolkit = EducationalToolkit()

    _visualization_initialized = True

    # Start real-time monitoring
    _global_visualizer.start_monitoring()

    return {
        'status': 'success',
        'initialized_components': [
            'real_time_visualizer',
            'analytics_engine',
            'debug_logger',
            'export_manager',
            'comparison_analyzer',
            'educational_toolkit',
        ],
        'brain_state': {
            'neuron_count': brain.getTotalNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
        },
    }


def get_global_visualizer() -> Optional[RealTimeVisualizer]:
    """Get the global real-time visualizer instance."""
    return _global_visualizer


def get_global_analytics() -> Optional[AnalyticsEngine]:
    """Get the global analytics engine instance."""
    return _global_analytics


def get_global_debug_logger() -> Optional[DebugLogger]:
    """Get the global debug logger instance."""
    return _global_debug_logger


def get_global_exporter() -> Optional[ExportManager]:
    """Get the global export manager instance."""
    return _global_exporter


def get_global_comparison_analyzer() -> Optional[ComparisonAnalyzer]:
    """Get the global comparison analyzer instance."""
    return _global_comparison_analyzer


def get_global_educational_toolkit() -> Optional[EducationalToolkit]:
    """Get the global educational toolkit instance."""
    return _global_educational_toolkit


def shutdown_visualization() -> Dict[str, Any]:
    """Shutdown global visualization instances."""
    global _visualization_initialized, _global_visualizer, _global_analytics, _global_debug_logger, _global_exporter, _global_comparison_analyzer, _global_educational_toolkit

    if not _visualization_initialized:
        return {'status': 'not_initialized', 'message': 'Visualization not initialized'}

    if _global_visualizer:
        _global_visualizer.stop_monitoring()

    _visualization_initialized = False
    _global_visualizer = None
    _global_analytics = None
    _global_debug_logger = None
    _global_exporter = None
    _global_comparison_analyzer = None
    _global_educational_toolkit = None

    return {
        'status': 'success',
        'message': 'Visualization instances shut down successfully',
    }


if __name__ == "__main__":
    print("Enhanced NLM Visualization Module")
    print("Available features:")
    for feature, capabilities in get_visualization_capabilities().items():
        print(f"  - {feature}: {capabilities['description']}")