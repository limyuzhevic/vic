# Advanced features for NLM power users
from typing import Optional, List, Dict, Any, Callable, Union
import time
import json
import threading
from enum import Enum

class ProfilingLevel(Enum):
    """Profiling levels for performance monitoring."""
    OFF = 0
    BASIC = 1
    DETAILED = 2
    TRACING = 3

class PerformanceMonitor:
    """
    Performance monitoring for NLM simulations.
    
    Provides detailed performance statistics, profiling capabilities,
    and bottleneck analysis for power users.
    """
    
    def __init__(self):
        self._start_time = 0.0
        self._step_times = []
        self._memory_usage = []
        self._spike_times = []
        self._neuron_states = []
        self._profiling_level = ProfilingLevel.BASIC
        self._enabled = True
        self._lock = threading.RLock()
    
    def start(self) -> None:
        """Start performance monitoring."""
        with self._lock:
            self._start_time = time.time()
            self._step_times.clear()
            self._spike_times.clear()
            self._neuron_states.clear()
    
    def stop(self) -> Dict[str, Any]:
        """
        Stop performance monitoring and return statistics.
        
        Returns:
            Dict containing performance statistics
        """
        with self._lock:
            total_time = time.time() - self._start_time
            stats = {
                'total_time': total_time,
                'step_count': len(self._step_times),
                'steps_per_second': len(self._step_times) / total_time if total_time > 0 else 0,
                'avg_step_time': sum(self._step_times) / len(self._step_times) if self._step_times else 0,
                'min_step_time': min(self._step_times) if self._step_times else 0,
                'max_step_time': max(self._step_times) if self._step_times else 0,
                'spike_count': len(self._spike_times),
                'spike_frequency': len(self._spike_times) / total_time if total_time > 0 else 0,
                'neuron_state_count': len(self._neuron_states)
            }
            return stats
    
    def record_step_time(self, step_time: float) -> None:
        """Record step execution time."""
        if self._enabled:
            with self._lock:
                self._step_times.append(step_time)
    
    def record_spike(self, timestamp: float) -> None:
        """Record a spike event."""
        if self._enabled:
            with self._lock:
                self._spike_times.append(timestamp)
    
    def record_neuron_state(self, state: Dict[str, Any]) -> None:
        """Record neuron state snapshot."""
        if self._enabled and self._profiling_level != ProfilingLevel.OFF:
            with self._lock:
                if self._profiling_level == ProfilingLevel.TRACING:
                    self._neuron_states.append(state)
    
    def set_profiling_level(self, level: ProfilingLevel) -> None:
        """Set profiling level."""
        self._profiling_level = level
    
    def enable(self, enable: bool) -> None:
        """Enable or disable performance monitoring."""
        self._enabled = enable
    
    def get_summary(self) -> str:
        """Get performance summary as formatted string."""
        stats = self.stop()
        lines = [
            "=== NLM Performance Summary ===",
            f"Total time: {stats['total_time']:.3f}s",
            f"Steps per second: {stats['steps_per_second']:.1f}",
            f"Average step time: {stats['avg_step_time']*1000:.2f}ms",
            f"Min/Max step time: {stats['min_step_time']*1000:.2f}ms / {stats['max_step_time']*1000:.2f}ms",
            f"Spike count: {stats['spike_count']}",
            f"Spike frequency: {stats['spike_frequency']:.1f} Hz",
            "==============================="
        ]
        return "\n".join(lines)

class AdvancedBrainFeatures:
    """
    Advanced brain features for power users.
    
    Provides enhanced functionality for experimentation,
    debugging, and advanced analysis.
    """
    
    def __init__(self, brain):
        """
        Initialize advanced brain features.
        
        Args:
            brain: Brain object to enhance
        """
        self._brain = brain
        self._performance_monitor = PerformanceMonitor()
        self._experiment_history = []
        self._custom_metrics = {}
    
    def enable_performance_monitoring(self, level: ProfilingLevel = ProfilingLevel.BASIC) -> None:
        """Enable performance monitoring."""
        self._performance_monitor.set_profiling_level(level)
        self._performance_monitor.enable(True)
        self._performance_monitor.start()
    
    def get_performance_stats(self) -> Dict[str, Any]:
        """Get performance statistics."""
        return self._performance_monitor.stop()
    
    def record_custom_metric(self, name: str, value: Any, timestamp: Optional[float] = None) -> None:
        """
        Record a custom metric.
        
        Args:
            name: Metric name
            value: Metric value
            timestamp: Optional timestamp
        """
        if timestamp is None:
            timestamp = time.time()
        
        if name not in self._custom_metrics:
            self._custom_metrics[name] = []
        
        self._custom_metrics[name].append({
            'timestamp': timestamp,
            'value': value,
            'step': getattr(self._brain, '_step_count', 0)
        })
    
    def get_custom_metric(self, name: str) -> List[Dict[str, Any]]:
        """
        Get custom metric data.
        
        Args:
            name: Metric name
            
        Returns:
            List of metric data points
        """
        return self._custom_metrics.get(name, [])
    
    def start_experiment(self, name: str, description: str = "") -> str:
        """
        Start an experiment.
        
        Args:
            name: Experiment name
            description: Experiment description
            
        Returns:
            Experiment ID
        """
        experiment_id = f"exp_{int(time.time())}_{hash(name) & 0xFFFFFFFF}"
        
        experiment = {
            'id': experiment_id,
            'name': name,
            'description': description,
            'start_time': time.time(),
            'brain_snapshot': self._brain.getConfig().getKeys(),
            'metrics': {}
        }
        
        self._experiment_history.append(experiment)
        return experiment_id
    
    def end_experiment(self, experiment_id: str) -> Dict[str, Any]:
        """
        End an experiment.
        
        Args:
            experiment_id: Experiment ID
            
        Returns:
            Experiment results
        """
        for experiment in self._experiment_history:
            if experiment['id'] == experiment_id:
                experiment['end_time'] = time.time()
                experiment['duration'] = experiment['end_time'] - experiment['start_time']
                
                # Collect metrics for this experiment
                results = {
                    'experiment_id': experiment_id,
                    'name': experiment['name'],
                    'duration': experiment['duration'],
                    'brain_config': experiment['brain_snapshot'],
                    'custom_metrics': {}
                }
                
                # Extract metrics relevant to this experiment
                for metric_name, data in self._custom_metrics.items():
                    if data and 'step' in data[0]:
                        results['custom_metrics'][metric_name] = data
                
                return results
        
        return {'error': f"Experiment {experiment_id} not found"}
    
    def get_experiment_history(self) -> List[Dict[str, Any]]:
        """Get experiment history."""
        return self._experiment_history.copy()
    
    def export_metrics(self, filename: str, format: str = "json") -> bool:
        """
        Export metrics to file.
        
        Args:
            filename: Output filename
            format: Export format (json, csv, etc.)
            
        Returns:
            True if export succeeded
        """
        try:
            if format.lower() == 'json':
                data = {
                    'performance_stats': self._performance_monitor.stop(),
                    'custom_metrics': self._custom_metrics,
                    'experiment_history': self._experiment_history,
                    'timestamp': time.time()
                }
                
                with open(filename, 'w') as f:
                    json.dump(data, f, indent=2, default=str)
            
            elif format.lower() == 'csv':
                import csv
                with open(filename, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(['Metric', 'Timestamp', 'Value'])
                    
                    for metric_name, data in self._custom_metrics.items():
                        for point in data:
                            writer.writerow([
                                metric_name,
                                point['timestamp'],
                                str(point['value'])
                            ])
            
            return True
        except Exception as e:
            print(f"Error exporting metrics: {e}")
            return False

class AdvancedWorldFeatures:
    """
    Advanced world features for power users.
    
    Provides enhanced world simulation capabilities.
    """
    
    def __init__(self, world):
        """
        Initialize advanced world features.
        
        Args:
            world: World object to enhance
        """
        self._world = world
        self._sensors = {}
        self._ actuators = {}
    
    def add_sensor(self, name: str, sensor_func: Callable) -> None:
        """
        Add a custom sensor.
        
        Args:
            name: Sensor name
            sensor_func: Function that returns sensor data
        """
        self._sensors[name] = sensor_func
    
    def add_actuator(self, name: str, actuator_func: Callable) -> None:
        """
        Add a custom actuator.
        
        Args:
            name: Actuator name
            actuator_func: Function to apply actuator effect
        """
        self._actuators[name] = actuator_func
    
    def get_sensor_data(self) -> Dict[str, Any]:
        """Get data from all custom sensors."""
        data = {}
        for name, sensor_func in self._sensors.items():
            try:
                data[name] = sensor_func(self._world)
            except Exception as e:
                data[name] = {'error': str(e)}
        return data
    
    def apply_actuator(self, name: str, **kwargs) -> None:
        """
        Apply actuator effect.
        
        Args:
            name: Actuator name
            **kwargs: Actuator parameters
        """
        if name in self._actuators:
            self._actuators[name](self._world, **kwargs)
    
    def create_sensor_wrapper(self, sensor_name: str) -> Callable:
        """
        Create a sensor wrapper function.
        
        Args:
            sensor_name: Sensor name
            
        Returns:
            Wrapper function that returns sensor data
        """
        def sensor_wrapper():
            if sensor_name in self._sensors:
                return self._sensors[sensor_name](self._world)
            return None
        
        return sensor_wrapper
    
    def create_actuator_wrapper(self, actuator_name: str) -> Callable:
        """
        Create an actuator wrapper function.
        
        Args:
            actuator_name: Actuator name
            
        Returns:
            Wrapper function to apply actuator effect
        """
        def actuator_wrapper(**kwargs):
            if actuator_name in self._actuators:
                self._actuators[actuator_name](self._world, **kwargs)
        
        return actuator_wrapper

class ExperimentManager:
    """
    Experiment management for power users.
    
    Provides tools for running and managing NLM experiments.
    """
    
    def __init__(self):
        self._experiments = {}
        self._results = {}
    
    def create_experiment(self, name: str, brain, world, agent,
                         config: Optional[Dict[str, Any]] = None) -> str:
        """
        Create a new experiment.
        
        Args:
            name: Experiment name
            brain: Brain object
            world: World object
            agent: Agent object
            config: Optional experiment configuration
            
        Returns:
            Experiment ID
        """
        experiment_id = f"exp_{int(time.time())}_{hash(name) & 0xFFFFFFFF}"
        
        experiment = {
            'id': experiment_id,
            'name': name,
            'config': config or {},
            'brain': brain,
            'world': world,
            'agent': agent,
            'start_time': None,
            'end_time': None,
            'status': 'created'
        }
        
        self._experiments[experiment_id] = experiment
        return experiment_id
    
    def run_experiment(self, experiment_id: str, num_steps: int) -> Dict[str, Any]:
        """
        Run an experiment.
        
        Args:
            experiment_id: Experiment ID
            num_steps: Number of simulation steps
            
        Returns:
            Experiment results
        """
        if experiment_id not in self._experiments:
            return {'error': f"Experiment {experiment_id} not found"}
        
        experiment = self._experiments[experiment_id]
        experiment['status'] = 'running'
        experiment['start_time'] = time.time()
        
        # Initialize experiment
        brain = experiment['brain']
        world = experiment['world']
        agent = experiment['agent']
        
        # Initialize with configuration if provided
        if 'brain_config' in experiment['config']:
            brain_config = experiment['config']['brain_config']
            # Apply brain configuration
        
        # Run experiment
        for step in range(num_steps):
            # Update world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Decode motor command
            action = agent.decodeMotorCommand()
            
            # Apply action to world
            if action:
                world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record metrics if enabled
            if 'record_metrics' in experiment['config'] and experiment['config']['record_metrics']:
                self._record_experiment_step(experiment_id, step, brain, world, agent)
        
        # End experiment
        experiment['end_time'] = time.time()
        experiment['status'] = 'completed'
        
        # Collect results
        results = self._collect_experiment_results(experiment_id)
        self._results[experiment_id] = results
        
        return results
    
    def get_experiment_results(self, experiment_id: str) -> Optional[Dict[str, Any]]:
        """
        Get experiment results.
        
        Args:
            experiment_id: Experiment ID
            
        Returns:
            Experiment results or None if not available
        """
        return self._results.get(experiment_id)
    
    def list_experiments(self) -> List[Dict[str, Any]]:
        """List all experiments."""
        return list(self._experiments.values())
    
    def delete_experiment(self, experiment_id: str) -> bool:
        """
        Delete an experiment.
        
        Args:
            experiment_id: Experiment ID
            
        Returns:
            True if experiment was deleted
        """
        if experiment_id in self._experiments:
            del self._experiments[experiment_id]
            if experiment_id in self._results:
                del self._results[experiment_id]
            return True
        return False
    
    def _record_experiment_step(self, experiment_id: str, step: int, brain, world, agent) -> None:
        """Record experiment step data."""
        if experiment_id not in self._experiments:
            return
        
        experiment = self._experiments[experiment_id]
        if 'metrics' not in experiment:
            experiment['metrics'] = {}
        
        # Record brain metrics
        brain_metrics = {
            'step': step,
            'timestamp': time.time(),
            'total_neurons': brain.getTotalNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'development_stage': brain.getDevelopmentalStage()
        }
        
        if 'brain_metrics' not in experiment['metrics']:
            experiment['metrics']['brain_metrics'] = []
        
        experiment['metrics']['brain_metrics'].append(brain_metrics)
        
        # Record world metrics
        world_metrics = {
            'step': step,
            'timestamp': time.time(),
            'simulation_time': world.getSimulationTime(),
            'agent_x': world.getAgentBody().x,
            'agent_y': world.getAgentBody().y,
            'agent_energy': world.getAgentBody().energy,
            'agent_health': world.getAgentBody().health
        }
        
        if 'world_metrics' not in experiment['metrics']:
            experiment['metrics']['world_metrics'] = []
        
        experiment['metrics']['world_metrics'].append(world_metrics)
        
        # Record agent metrics
        agent_metrics = {
            'step': step,
            'timestamp': time.time(),
            'curiosity': agent.getCuriosityLevel(),
            'novelty': agent.getNoveltyLevel(),
            'prediction_error': agent.getPredictionError(),
            'neuromodulation': agent.getNeuromodulationLevel()
        }
        
        if 'agent_metrics' not in experiment['metrics']:
            experiment['metrics']['agent_metrics'] = []
        
        experiment['metrics']['agent_metrics'].append(agent_metrics)
    
    def _collect_experiment_results(self, experiment_id: str) -> Dict[str, Any]:
        """Collect experiment results."""
        if experiment_id not in self._experiments:
            return {}
        
        experiment = self._experiments[experiment_id]
        
        results = {
            'experiment_id': experiment_id,
            'name': experiment['name'],
            'status': experiment['status'],
            'duration': experiment['end_time'] - experiment['start_time'] if experiment['end_time'] and experiment['start_time'] else 0,
            'metrics': experiment.get('metrics', {})
        }
        
        return results

# Factory functions for advanced features
def create_performance_monitor() -> PerformanceMonitor:
    """Create a performance monitor."""
    return PerformanceMonitor()

def create_advanced_brain_features(brain) -> AdvancedBrainFeatures:
    """Create advanced brain features."""
    return AdvancedBrainFeatures(brain)

def create_advanced_world_features(world) -> AdvancedWorldFeatures:
    """Create advanced world features."""
    return AdvancedWorldFeatures(world)

def create_experiment_manager() -> ExperimentManager:
    """Create an experiment manager."""
    return ExperimentManager()
