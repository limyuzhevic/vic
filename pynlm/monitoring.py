"""
pynlm - Real-time Monitoring and Visualization

Real-time monitoring, visualization, and data collection for NLM simulations.
"""

import threading
import time
import queue
import json
from typing import List, Dict, Any, Optional, Callable
from datetime import datetime
import numpy as np
from collections import deque

# Import main classes
import pynlm as nlm_core
class MonitoringSystem:
    """Real-time monitoring system for NLM simulations."""
    
    def __init__(self, brain: Any, agent: Any, world: Any):
        self.brain = brain
        self.agent = agent
        self.world = world
        self.monitoring_active = False
        self.data_queue = queue.Queue(maxsize=1000)
        self.thread: Optional[threading.Thread] = None
        self.sampling_interval = 0.1  # seconds
        self.max_data_points = 10000
        
        # Monitoring data storage
        self.data_points: List[Dict[str, Any]] = []
        self.current_metrics: Dict[str, Any] = {}
        
        # Callback system
        self.data_callbacks: List[Callable] = []
        self.event_callbacks: Dict[str, List[Callable]] = {
            'threshold_exceeded': [],
            'critical_event': [],
            'state_change': [],
            'data_collected': []
        }
        
    def start(self):
        """Start real-time monitoring."""
        if self.monitoring_active:
            return False
            
        self.monitoring_active = True
        self.thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.thread.start()
        return True
    
    def stop(self):
        """Stop real-time monitoring."""
        if not self.monitoring_active:
            return False
            
        self.monitoring_active = False
        if self.thread:
            self.thread.join(timeout=1.0)
            self.thread = None
        return True
    
    def set_sampling_interval(self, interval: float):
        """Set sampling interval in seconds."""
        self.sampling_interval = max(0.01, interval)  # Minimum 10ms
    
    def add_data_callback(self, callback: Callable):
        """Add callback for new data points."""
        self.data_callbacks.append(callback)
    
    def remove_data_callback(self, callback: Callable):
        """Remove data callback."""
        if callback in self.data_callbacks:
            self.data_callbacks.remove(callback)
    
    def add_event_callback(self, event_type: str, callback: Callable):
        """Add callback for specific events."""
        if event_type in self.event_callbacks:
            self.event_callbacks[event_type].append(callback)
    
    def _monitor_loop(self):
        """Main monitoring loop running in separate thread."""
        while self.monitoring_active:
            try:
                # Collect current metrics
                data_point = self._collect_metrics()
                
                # Add to queue
                if self.data_queue.full():
                    self.data_queue.get()  # Remove oldest item
                self.data_queue.put(data_point)
                
                # Update local storage
                self.data_points.append(data_point)
                if len(self.data_points) > self.max_data_points:
                    self.data_points.pop(0)
                
                # Notify callbacks
                self._notify_callbacks(data_point)
                
                # Sleep for sampling interval
                time.sleep(self.sampling_interval)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(self.sampling_interval)
    
    def _collect_metrics(self) -> Dict[str, Any]:
        """Collect current simulation metrics."""
        timestamp = time.time()
        
        # Basic brain metrics
        brain_metrics = {
            'total_neurons': self.brain.getTotalNeuronCount(),
            'active_neurons': self.brain.getFiringNeuronCount(),
            'total_spikes': self.brain.getTotalSpikeCount(),
            'average_firing_rate': self.brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': self.brain.getExcitationInhibitionRatio(),
            'developmental_stage': self.brain.getDevelopmentalStage()
        }
        
        # Agent metrics
        agent_metrics = {
            'curiosity_level': self.agent.getCuriosityLevel(),
            'novelty_level': self.agent.getNoveltyLevel(),
            'prediction_error': self.agent.getPredictionError(),
            'neuromodulation_level': self.agent.getNeuromodulationLevel()
        }
        
        # World metrics
        body = self.world.getAgentBody()
        world_metrics = {
            'agent_x': body.x,
            'agent_y': body.y,
            'agent_orientation': body.orientation,
            'agent_velocity_x': body.velocityX,
            'agent_velocity_y': body.velocityY,
            'agent_angular_velocity': body.angularVelocity,
            'agent_energy': body.energy,
            'agent_health': body.health,
            'agent_age': body.age,
            'agent_is_moving': body.isMoving,
            'agent_is_turning': body.isTurning,
            'simulation_time': self.world.getSimulationTime(),
            'world_width': self.world.getWidth(),
            'world_height': self.world.getHeight()
        }
        
        # Combine all metrics
        data_point = {
            'timestamp': timestamp,
            'brain': brain_metrics,
            'agent': agent_metrics,
            'world': world_metrics,
            'percept': self._extract_percept_data()
        }
        
        return data_point
    
    def _extract_percept_data(self) -> Dict[str, Any]:
        """Extract and format percept data."""
        percept = self.world.getSensoryPercept()
        
        if not percept:
            return {}
        
        data = {}
        
        # Vision data
        vision = percept.getVision()
        if vision:
            data['vision'] = {
                'width': vision.getWidth(),
                'height': vision.getHeight(),
                'channels': vision.getChannels(),
                'has_data': True
            }
        
        # Audio data
        audio = percept.getAudio()
        if audio:
            data['audio'] = {
                'sample_rate': audio.getSampleRate(),
                'num_samples': audio.getNumSamples(),
                'has_data': True
            }
        
        # Internal signals
        internal = percept.getInternal()
        if internal:
            data['internal_signals'] = {
                'signal_count': len(internal),
                'signal_values': internal,
                'has_data': True
            }
        
        return data
    
    def _notify_callbacks(self, data_point: Dict[str, Any]):
        """Notify all registered callbacks of new data."""
        
        # Notify data callbacks
        for callback in self.data_callbacks:
            try:
                callback(data_point)
            except Exception as e:
                print(f"Data callback error: {e}")
        
        # Check for threshold events
        self._check_threshold_events(data_point)
    
    def _check_threshold_events(self, data_point: Dict[str, Any]):
        """Check if any thresholds have been exceeded."""
        
        brain = data_point['brain']
        agent = data_point['agent']
        world = data_point['world']
        
        # Check brain metrics thresholds
        if brain['average_firing_rate'] > 100.0:
            self._trigger_event('critical_event', {
                'type': 'high_firing_rate',
                'value': brain['average_firing_rate'],
                'threshold': 100.0,
                'data_point': data_point
            })
        
        if brain['active_neurons'] > brain['total_neurons'] * 0.8:
            self._trigger_event('threshold_exceeded', {
                'type': 'high_neuron_activity',
                'value': brain['active_neurons'],
                'threshold': brain['total_neurons'] * 0.8,
                'data_point': data_point
            })
        
        # Check agent metrics thresholds
        if agent['curiosity_level'] < 0.1:
            self._trigger_event('threshold_exceeded', {
                'type': 'low_curiosity',
                'value': agent['curiosity_level'],
                'threshold': 0.1,
                'data_point': data_point
            })
        
        if agent['prediction_error'] > 1.0:
            self._trigger_event('critical_event', {
                'type': 'high_prediction_error',
                'value': agent['prediction_error'],
                'threshold': 1.0,
                'data_point': data_point
            })
        
        # Check world metrics thresholds
        if world['agent_health'] < 0.2:
            self._trigger_event('critical_event', {
                'type': 'low_health',
                'value': world['agent_health'],
                'threshold': 0.2,
                'data_point': data_point
            })
        
        if world['agent_energy'] < 0.1:
            self._trigger_event('threshold_exceeded', {
                'type': 'low_energy',
                'value': world['agent_energy'],
                'threshold': 0.1,
                'data_point': data_point
            })
    
    def _trigger_event(self, event_type: str, event_data: Dict[str, Any]):
        """Trigger an event notification."""
        for callback in self.event_callbacks.get(event_type, []):
            try:
                callback(event_data)
            except Exception as e:
                print(f"Event callback error: {e}")
    
    def get_current_metrics(self) -> Dict[str, Any]:
        """Get current metrics without collecting new data."""
        return self.current_metrics.copy()
    
    def get_recent_data(self, count: int = 100) -> List[Dict[str, Any]]:
        """Get recent data points."""
        return self.data_points[-count:]
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get statistical summary of monitoring data."""
        if not self.data_points:
            return {}
        
        # Calculate statistics for numeric fields
        stats = {}
        
        # Sample first data point to get field names
        first_point = self.data_points[0]
        numeric_fields = []
        
        def extract_numeric_fields(obj, prefix=''):
            if isinstance(obj, dict):
                for key, value in obj.items():
                    if isinstance(value, (int, float)):
                        numeric_fields.append(f"{prefix}{key}")
                    else:
                        extract_numeric_fields(value, f"{prefix}{key}.")
        
        extract_numeric_fields(first_point)
        
        # Calculate statistics for each numeric field
        for field in numeric_fields:
            values = []
            for data_point in self.data_points:
                # Navigate through nested structure
                parts = field.split('.')
                current = data_point
                for part in parts:
                    if isinstance(current, dict) and part in current:
                        current = current[part]
                    else:
                        current = None
                        break
                
                if current is not None and isinstance(current, (int, float)):
                    values.append(current)
            
            if values:
                stats[field] = {
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'median': np.median(values),
                    'count': len(values)
                }
        
        return stats
    
    def export_data(self, filename: str, format: str = 'json'):
        """Export monitoring data to file."""
        if format == 'json':
            with open(filename, 'w') as f:
                json.dump(self.data_points, f, indent=2)
        elif format == 'csv':
            import csv
            if not self.data_points:
                return
            
            # Flatten data structure for CSV
            with open(filename, 'w', newline='') as csvfile:
                writer = csv.writer(csvfile)
                
                # Write header
                headers = []
                first_point = self.data_points[0]
                
                def get_headers(obj, prefix=''):
                    if isinstance(obj, dict):
                        for key, value in obj.items():
                            if isinstance(value, (int, float)):
                                headers.append(f"{prefix}{key}")
                            else:
                                get_headers(value, f"{prefix}{key}.")
                
                get_headers(first_point)
                writer.writerow(headers)
                
                # Write data rows
                for data_point in self.data_points:
                    row = []
                    for header in headers:
                        parts = header.split('.')
                        current = data_point
                        for part in parts:
                            if isinstance(current, dict) and part in current:
                                current = current[part]
                            else:
                                current = ''
                                break
                        row.append(current)
                    
                    writer.writerow(row)
class VisualizationHelper:
    """Helper functions for data visualization."""
    
    @staticmethod
    def plot_time_series(data_points: List[Dict[str, Any]], field: str, 
                        title: str = "Time Series") -> None:
        """Plot time series data."""
        try:
            import matplotlib.pyplot as plt
            
            # Extract timestamps and field values
            timestamps = []
            values = []
            
            for data_point in data_points:
                # Navigate through nested structure
                parts = field.split('.')
                current = data_point
                for part in parts:
                    if isinstance(current, dict) and part in current:
                        current = current[part]
                    else:
                        current = None
                        break
                
                if current is not None and isinstance(current, (int, float)):
                    timestamps.append(data_point['timestamp'])
                    values.append(current)
            
            if not values:
                return
            
            # Convert timestamps to matplotlib dates
            timestamps = [datetime.fromtimestamp(ts) for ts in timestamps]
            
            # Create plot
            plt.figure(figsize=(12, 6))
            plt.plot(timestamps, values, linewidth=2)
            plt.title(title)
            plt.xlabel('Time')
            plt.ylabel(field.replace('.', ' ').title())
            plt.grid(True, alpha=0.3)
            plt.xticks(rotation=45)
            plt.tight_layout()
            plt.show()
            
        except ImportError:
            print("matplotlib not available. Install with: pip install matplotlib")
    
    @staticmethod
    def create_dashboard(data_points: List[Dict[str, Any]], 
                        metrics_to_display: List[str] = None) -> Dict[str, Any]:
        """Create a visualization dashboard from monitoring data."""
        
        if not data_points:
            return {}
        
        if metrics_to_display is None:
            # Default metrics to display
            metrics_to_display = [
                'brain.average_firing_rate',
                'brain.active_neurons',
                'agent.curiosity_level',
                'agent.novelty_level',
                'world.agent_energy',
                'world.agent_health'
            ]
        
        dashboard = {
            'timestamp': datetime.now().isoformat(),
            'data_points_count': len(data_points),
            'metrics': {},
            'summary': {}
        }
        
        # Extract metrics data
        for metric in metrics_to_display:
            values = []
            timestamps = []
            
            for data_point in data_points:
                parts = metric.split('.')
                current = data_point
                for part in parts:
                    if isinstance(current, dict) and part in current:
                        current = current[part]
                    else:
                        current = None
                        break
                
                if current is not None and isinstance(current, (int, float)):
                    values.append(current)
                    timestamps.append(data_point['timestamp'])
            
            if values:
                dashboard['metrics'][metric] = {
                    'values': values,
                    'timestamps': timestamps,
                    'mean': np.mean(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'current': values[-1]
                }
        
        # Calculate summary statistics
        dashboard['summary'] = {
            'total_monitoring_time': (max(dp['timestamp'] for dp in data_points) - 
                                    min(dp['timestamp'] for dp in data_points)) / 60,  # minutes
            'average_firing_rate': np.mean([dp['brain']['average_firing_rate'] 
                                           for dp in data_points]),
            'average_curiosity': np.mean([dp['agent']['curiosity_level'] 
                                        for dp in data_points]),
            'energy_trend': 'stable' if abs(np.mean([dp['world']['agent_energy'] 
                                                   for dp in data_points]) - 1.0) < 0.1 else 'decreasing'
        }
        
        return dashboard
    
    @staticmethod
    def create_real_time_plotter(monitoring_system: MonitoringSystem):
        """Create a real-time plotting function for monitoring system."""
        
        def plot_updates(interval: float = 1.0):
            """Plot updates in real-time."""
            try:
                import matplotlib.pyplot as plt
                
                # Create figure with subplots
                fig, axes = plt.subplots(2, 3, figsize=(15, 10))
                fig.suptitle('NLM Real-Time Monitoring Dashboard', fontsize=16)
                
                # Flatten axes for easy indexing
                axes = axes.flatten()
                
                # Get recent data
                recent_data = monitoring_system.get_recent_data(300)  # Last 30 seconds at 10Hz
                
                if not recent_data:
                    return
                
                metrics = monitoring_system.get_statistics()
                
                # Plot 1: Firing rate
                firing_rates = [dp['brain']['average_firing_rate'] for dp in recent_data]
                axes[0].plot(firing_rates, linewidth=2, color='blue')
                axes[0].set_title('Average Firing Rate')
                axes[0].set_ylabel('Hz')
                axes[0].grid(True, alpha=0.3)
                
                # Plot 2: Neuron activity
                active_neurons = [dp['brain']['active_neurons'] for dp in recent_data]
                total_neurons = recent_data[0]['brain']['total_neurons']
                axes[1].plot(active_neurons, linewidth=2, color='green')
                axes[1].set_title('Active Neurons')
                axes[1].set_ylabel('Count')
                axes[1].set_ylim(0, total_neurons)
                axes[1].grid(True, alpha=0.3)
                
                # Plot 3: Curiosity level
                curiosities = [dp['agent']['curiosity_level'] for dp in recent_data]
                axes[2].plot(curiosities, linewidth=2, color='orange')
                axes[2].set_title('Curiosity Level')
                axes[2].set_ylabel('Level')
                axes[2].set_ylim(0, 1)
                axes[2].grid(True, alpha=0.3)
                
                # Plot 4: Novelty level
                novelties = [dp['agent']['novelty_level'] for dp in recent_data]
                axes[3].plot(novelties, linewidth=2, color='purple')
                axes[3].set_title('Novelty Level')
                axes[3].set_ylabel('Level')
                axes[3].set_ylim(0, 1)
                axes[3].grid(True, alpha=0.3)
                
                # Plot 5: Agent energy
                energies = [dp['world']['agent_energy'] for dp in recent_data]
                axes[4].plot(energies, linewidth=2, color='red')
                axes[4].set_title('Agent Energy')
                axes[4].set_ylabel('Energy')
                axes[4].set_ylim(0, 1)
                axes[4].grid(True, alpha=0.3)
                
                # Plot 6: Agent health
                healths = [dp['world']['agent_health'] for dp in recent_data]
                axes[5].plot(healths, linewidth=2, color='brown')
                axes[5].set_title('Agent Health')
                axes[5].set_ylabel('Health')
                axes[5].set_ylim(0, 1)
                axes[5].grid(True, alpha=0.3)
                
                plt.tight_layout()
                plt.pause(0.1)
                
            except ImportError:
                print("matplotlib not available. Install with: pip install matplotlib")
        
        return plot_updates
    
    @staticmethod
    def format_number(number: float, precision: int = 2) -> str:
        """Format a number with appropriate precision."""
        if number >= 1000 or number <= -1000:
            return f"{number/1000:.{precision}f}K"
        elif number >= 1000000 or number <= -1000000:
            return f"{number/1000000:.{precision}f}M"
        else:
            return f"{number:.{precision}f}"
    
    @staticmethod
    def format_time(seconds: float) -> str:
        """Format time in human-readable format."""
        if seconds < 60:
            return f"{seconds:.1f}s"
        elif seconds < 3600:
            minutes = seconds // 60
            secs = seconds % 60
            return f"{int(minutes)}m {secs:.1f}s"
        else:
            hours = seconds // 3600
            minutes = (seconds % 3600) // 60
            secs = seconds % 60
            return f"{int(hours)}h {int(minutes)}m {secs:.1f}s"
class ExperimentMonitor:
    """Monitor and analyze NLM experiments."""
    
    def __init__(self):
        self.experiments: List[Dict[str, Any]] = []
        self.current_experiment: Optional[Dict[str, Any]] = None
        
    def start_experiment(self, name: str, description: str = "", 
                        config: Dict[str, Any] = None):
        """Start monitoring a new experiment."""
        if self.current_experiment:
            print("An experiment is already running. Stop it first.")
            return False
        
        self.current_experiment = {
            'name': name,
            'description': description,
            'config': config or {},
            'start_time': time.time(),
            'end_time': None,
            'monitoring_system': None,
            'data_points': [],
            'events': []
        }
        
        self.experiments.append(self.current_experiment)
        return True
    
    def stop_experiment(self):
        """Stop the current experiment."""
        if not self.current_experiment:
            return False
        
        self.current_experiment['end_time'] = time.time()
        experiment = self.current_experiment.copy()
        self.current_experiment = None
        
        # Calculate experiment duration
        duration = experiment['end_time'] - experiment['start_time']
        
        # Add to experiments list with duration
        experiment['duration_seconds'] = duration
        return experiment
    
    def get_current_experiment(self) -> Optional[Dict[str, Any]]:
        """Get current experiment info."""
        return self.current_experiment.copy() if self.current_experiment else None
    
    def get_all_experiments(self) -> List[Dict[str, Any]]:
        """Get all experiments."""
        return [exp.copy() for exp in self.experiments]
    
    def export_experiment_data(self, experiment_id: int, filename: str,
                              format: str = 'json'):
        """Export experiment data to file."""
        if experiment_id >= len(self.experiments):
            raise ValueError(f"Experiment ID {experiment_id} not found")
        
        experiment = self.experiments[experiment_id]
        
        if format == 'json':
            with open(filename, 'w') as f:
                json.dump(experiment, f, indent=2)
        elif format == 'csv':
            import csv
            
            if not experiment.get('data_points'):
                return
            
            # Flatten data structure for CSV
            with open(filename, 'w', newline='') as csvfile:
                writer = csv.writer(csvfile)
                
                # Write header based on first data point
                if experiment['data_points']:
                    first_point = experiment['data_points'][0]
                    headers = list(first_point.keys())
                    writer.writerow(headers)
                    
                    # Write data rows
                    for data_point in experiment['data_points']:
                        row = []
                        for header in headers:
                            if isinstance(data_point.get(header), dict):
                                # For nested dicts, convert to string representation
                                row.append(str(data_point[header]))
                            else:
                                row.append(data_point.get(header))
                        writer.writerow(row)
    
    def generate_experiment_report(self, experiment_id: int) -> str:
        """Generate a report for a specific experiment."""
        if experiment_id >= len(self.experiments):
            return "Experiment not found."
        
        experiment = self.experiments[experiment_id]
        
        if not experiment.get('data_points'):
            return f"No data collected for experiment '{experiment['name']}'."
        
        # Calculate statistics
        data_points = experiment['data_points']
        
        # Calculate average metrics
        brain_metrics = {}
        agent_metrics = {}
        world_metrics = {}
        
        for data_point in data_points:
            # Brain metrics
            for key, value in data_point.get('brain', {}).items():
                if isinstance(value, (int, float)):
                    if key not in brain_metrics:
                        brain_metrics[key] = []
                    brain_metrics[key].append(value)
            
            # Agent metrics
            for key, value in data_point.get('agent', {}).items():
                if isinstance(value, (int, float)):
                    if key not in agent_metrics:
                        agent_metrics[key] = []
                    agent_metrics[key].append(value)
            
            # World metrics
            for key, value in data_point.get('world', {}).items():
                if isinstance(value, (int, float)):
                    if key not in world_metrics:
                        world_metrics[key] = []
                    world_metrics[key].append(value)
        
        # Calculate averages
        def calculate_stats(values):
            if not values:
                return {}
            return {
                'mean': np.mean(values),
                'std': np.std(values),
                'min': np.min(values),
                'max': np.max(values),
                'median': np.median(values)
            }
        
        brain_stats = {k: calculate_stats(v) for k, v in brain_metrics.items()}
        agent_stats = {k: calculate_stats(v) for k, v in agent_metrics.items()}
        world_stats = {k: calculate_stats(v) for k, v in world_metrics.items()}
        
        # Generate report
        report = f"""
=== Experiment Report ===

Experiment: {experiment['name']}
Description: {experiment['description']}
Duration: {experiment.get('duration_seconds', 'N/A')} seconds
Data Points: {len(data_points)}

=== Brain Metrics ===
"""
        
        for metric, stats in brain_stats.items():
            report += f"{metric}: Mean={stats['mean']:.2f}, Std={stats['std']:.2f}, "
            report += f"Min={stats['min']:.2f}, Max={stats['max']:.2f}\n"
        
        report += f"\n=== Agent Metrics ===\n"
        for metric, stats in agent_stats.items():
            report += f"{metric}: Mean={stats['mean']:.2f}, Std={stats['std']:.2f}, "
            report += f"Min={stats['min']:.2f}, Max={stats['max']:.2f}\n"
        
        report += f"\n=== World Metrics ===\n"
        for metric, stats in world_stats.items():
            report += f"{metric}: Mean={stats['mean']:.2f}, Std={stats['std']:.2f}, "
            report += f"Min={stats['min']:.2f}, Max={stats['max']:.2f}\n"
        
        # Find events
        if experiment.get('events'):
            report += f"\n=== Events ===\n"
            for event in experiment['events']:
                report += f"{event.get('timestamp', 'N/A')}: {event.get('type', 'Unknown')} - {event.get('message', '')}\n"
        
        return report
# Re-export commonly used classes and functions
__all__ = [
    'MonitoringSystem', 'VisualizationHelper', 'ExperimentMonitor'
]
