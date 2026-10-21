"""
Simulation utilities for NLM.

This module provides simulation utilities for NLM, including
utilities for running simulations, managing steps, and collecting metrics.
"""

import time
import math
from typing import Dict, List, Optional, Callable, Any, Tuple
from .core import Brain, Config
class SimulationStep:
    """Simulation step information."""
    
    def __init__(self, step_number: int, timestamp: float = 0.0):
        self.step_number = step_number
        self.timestamp = timestamp
        self.metrics = {}
    
    def add_metric(self, name: str, value: Any):
        """
        Add a metric for this step.
        
        Args:
            name: Metric name
            value: Metric value
        """
        self.metrics[name] = value
    
    def get_metric(self, name: str, default: Any = None) -> Any:
        """
        Get a metric for this step.
        
        Args:
            name: Metric name
            default: Default value if metric not found
            
        Returns:
            Metric value
        """
        return self.metrics.get(name, default)
class SimulationManager:
    """
    Simulation manager for NLM.
    
    This class provides simulation management functionality,
    including utilities for running simulations, managing steps,
    and collecting metrics.
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize simulation manager.
        
        Args:
            brain: Brain to simulate
        """
        self.brain = brain
        self.steps = []
        self.start_time = 0.0
        self.end_time = 0.0
        self.is_running = False
    
    def start(self):
        """Start simulation."""
        self.start_time = time.time()
        self.is_running = True
        self.steps.clear()
    
    def stop(self):
        """Stop simulation."""
        self.end_time = time.time()
        self.is_running = False
    
    def step(self, step_number: int, timestamp: Optional[float] = None) -> SimulationStep:
        """
        Take a simulation step.
        
        Args:
            step_number: Step number
            timestamp: Optional timestamp (uses current time if not provided)
            
        Returns:
            Simulation step
        """
        if timestamp is None:
            timestamp = time.time() - self.start_time
        
        step = SimulationStep(step_number, timestamp)
        
        # Run brain step
        self.brain.step(step_number)
        
        # Collect metrics
        step.add_metric('spike_count', self.brain.getTotalSpikeCount())
        step.add_metric('firing_neurons', self.brain.getFiringNeuronCount())
        step.add_metric('total_neurons', self.brain.getTotalNeuronCount())
        step.add_metric('firing_rate', self.brain.getAverageFiringRate())
        step.add_metric('exc_inhibition_ratio', self.brain.getExcitationInhibitionRatio())
        step.add_metric('developmental_stage', self.brain.getDevelopmentalStage().value)
        
        self.steps.append(step)
        return step
    
    def run_steps(self, num_steps: int, timestep: float = 0.001,
                  callback: Optional[Callable[[SimulationStep], None]] = None):
        """
        Run multiple simulation steps.
        
        Args:
            num_steps: Number of steps to run
            timestep: Time step between steps
            callback: Optional callback function for each step
        """
        self.start()
        
        try:
            for step_num in range(num_steps):
                timestamp = step_num * timestep
                step = self.step(step_num, timestamp)
                
                if callback:
                    callback(step)
        
        finally:
            self.stop()
    
    def get_metrics(self) -> Dict[str, Any]:
        """
        Get simulation metrics.
        
        Returns:
            Dictionary of metrics
        """
        if not self.steps:
            return {}
        
        metrics = {
            'total_steps': len(self.steps),
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': self.end_time - self.start_time,
            'final_spike_count': self.steps[-1].get_metric('spike_count'),
            'average_firing_rate': self._calculate_average('firing_rate'),
            'total_firing_rate': self._calculate_total('firing_rate'),
            'max_spike_count': self._calculate_max('spike_count'),
            'min_spike_count': self._calculate_min('spike_count'),
            'spike_std': self._calculate_std('spike_count'),
            'stage_transitions': self._count_stage_transitions()
        }
        
        return metrics
    
    def _calculate_average(self, metric_name: str) -> float:
        """Calculate average of metric."""
        values = [step.get_metric(metric_name) for step in self.steps if step.get_metric(metric_name) is not None]
        return sum(values) / len(values) if values else 0.0
    
    def _calculate_total(self, metric_name: str) -> float:
        """Calculate total of metric."""
        values = [step.get_metric(metric_name) for step in self.steps if step.get_metric(metric_name) is not None]
        return sum(values)
    
    def _calculate_max(self, metric_name: str) -> float:
        """Calculate maximum of metric."""
        values = [step.get_metric(metric_name) for step in self.steps if step.get_metric(metric_name) is not None]
        return max(values) if values else 0.0
    
    def _calculate_min(self, metric_name: str) -> float:
        """Calculate minimum of metric."""
        values = [step.get_metric(metric_name) for step in self.steps if step.get_metric(metric_name) is not None]
        return min(values) if values else 0.0
    
    def _calculate_std(self, metric_name: str) -> float:
        """Calculate standard deviation of metric."""
        values = [step.get_metric(metric_name) for step in self.steps if step.get_metric(metric_name) is not None]
        if len(values) < 2:
            return 0.0
        
        mean = sum(values) / len(values)
        variance = sum((x - mean) ** 2 for x in values) / len(values)
        return math.sqrt(variance)
    
    def _count_stage_transitions(self) -> int:
        """Count developmental stage transitions."""
        transitions = 0
        previous_stage = None
        
        for step in self.steps:
            stage = step.get_metric('developmental_stage')
            if previous_stage is not None and stage != previous_stage:
                transitions += 1
            previous_stage = stage
        
        return transitions
    
    def get_stage_timeline(self) -> List[Dict[str, Any]]:
        """
        Get developmental stage timeline.
        
        Returns:
            List of stage transitions with timing
        """
        timeline = []
        previous_stage = None
        previous_step_time = 0.0
        
        for step in self.steps:
            stage = step.get_metric('developmental_stage')
            timestamp = step.timestamp
            
            if previous_stage is not None and stage != previous_stage:
                transition_duration = timestamp - previous_step_time
                timeline.append({
                    'from_stage': previous_stage,
                    'to_stage': stage,
                    'start_time': previous_step_time,
                    'end_time': timestamp,
                    'duration': transition_duration,
                    'step_number': step.step_number
                })
            
            previous_stage = stage
            previous_step_time = timestamp
        
        return timeline
    
    def export_steps(self, filepath: str):
        """
        Export simulation steps to file.
        
        Args:
            filepath: Path to export file
        """
        import json
        
        # Convert steps to serializable format
        steps_data = []
        for step in self.steps:
            steps_data.append({
                'step_number': step.step_number,
                'timestamp': step.timestamp,
                'metrics': step.metrics
            })
        
        data = {
            'steps': steps_data,
            'metrics': self.get_metrics(),
            'stage_timeline': self.get_stage_timeline()
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"Simulation exported to {filepath}")
    
    def import_steps(self, filepath: str):
        """
        Import simulation steps from file.
        
        Args:
            filepath: Path to import file
        """
        import json
        
        with open(filepath, 'r') as f:
            data = json.load(f)
        
        # Clear current steps
        self.steps.clear()
        
        # Import steps
        for step_data in data['steps']:
            step = SimulationStep(
                step_data['step_number'],
                step_data['timestamp']
            )
            
            for metric_name, metric_value in step_data['metrics'].items():
                step.add_metric(metric_name, metric_value)
            
            self.steps.append(step)
        
        print(f"Simulation imported from {filepath}")
class PerformanceProfiler:
    """
    Performance profiler for NLM.
    
    This class provides performance profiling functionality,
    including timing of operations and performance analysis.
    """
    
    def __init__(self):
        """Initialize performance profiler."""
        self.timings = {}
        self.memory_usage = []
        self.current_operation = None
    
    def start_operation(self, operation_name: str):
        """
        Start timing an operation.
        
        Args:
            operation_name: Name of operation to time
        """
        self.current_operation = operation_name
        self.timings[operation_name] = {
            'start_time': time.time(),
            'duration': 0.0,
            'call_count': 0
        }
    
    def end_operation(self, operation_name: str = None):
        """
        End timing an operation.
        
        Args:
            operation_name: Name of operation to end (uses current operation if not provided)
        """
        if operation_name is None:
            operation_name = self.current_operation
        
        if operation_name in self.timings:
            end_time = time.time()
            start_time = self.timings[operation_name]['start_time']
            
            self.timings[operation_name]['duration'] += end_time - start_time
            self.timings[operation_name]['call_count'] += 1
            
            self.current_operation = None
    
    def get_operation_time(self, operation_name: str) -> float:
        """
        Get total time for an operation.
        
        Args:
            operation_name: Name of operation
            
        Returns:
            Total time in seconds
        """
        if operation_name in self.timings:
            return self.timings[operation_name]['duration']
        return 0.0
    
    def get_operation_count(self, operation_name: str) -> int:
        """
        Get call count for an operation.
        
        Args:
            operation_name: Name of operation
            
        Returns:
            Number of calls
        """
        if operation_name in self.timings:
            return self.timings[operation_name]['call_count']
        return 0
    
    def get_operation_avg_time(self, operation_name: str) -> float:
        """
        Get average time per call for an operation.
        
        Args:
            operation_name: Name of operation
            
        Returns:
            Average time in seconds
        """
        count = self.get_operation_count(operation_name)
        if count == 0:
            return 0.0
        
        return self.get_operation_time(operation_name) / count
    
    def get_operation_stats(self, operation_name: str) -> Dict[str, Any]:
        """
        Get statistics for an operation.
        
        Args:
            operation_name: Name of operation
            
        Returns:
            Dictionary of statistics
        """
        if operation_name not in self.timings:
            return {}
        
        stats = self.timings[operation_name]
        return {
            'total_time': stats['duration'],
            'call_count': stats['call_count'],
            'avg_time': self.get_operation_avg_time(operation_name),
            'min_time': self._get_min_time(operation_name),
            'max_time': self._get_max_time(operation_name),
            'std_time': self._get_std_time(operation_name)
        }
    
    def _get_min_time(self, operation_name: str) -> float:
        """Get minimum time for an operation (placeholder)."""
        return 0.0  # Would need to store all timings
    
    def _get_max_time(self, operation_name: str) -> float:
        """Get maximum time for an operation (placeholder)."""
        return 0.0  # Would need to store all timings
    
    def _get_std_time(self, operation_name: str) -> float:
        """Get standard deviation for an operation (placeholder)."""
        return 0.0  # Would need to store all timings
    
    def get_all_stats(self) -> Dict[str, Dict[str, Any]]:
        """
        Get statistics for all operations.
        
        Returns:
            Dictionary of operation statistics
        """
        return {
            name: self.get_operation_stats(name) for name in self.timings
        }
    
    def reset(self):
        """Reset profiler."""
        self.timings.clear()
        self.current_operation = None
    
    def generate_report(self) -> str:
        """
        Generate performance report.
        
        Returns:
            Performance report as string
        """
        stats = self.get_all_stats()
        
        if not stats:
            return "No performance data available."
        
        report = "NLM Performance Report\n"
        report += "=" * 50 + "\n\n"
        
        for operation_name, stat in stats.items():
            report += f"Operation: {operation_name}\n"
            report += f"  Total time: {stat['total_time']:.4f}s\n"
            report += f"  Call count: {stat['call_count']}\n"
            report += f"  Average time: {stat['avg_time']:.6f}s\n"
            report += f"\n"
        
        return report
class StateExporter:
    """
    State exporter for NLM.
    
    This class provides state export functionality for NLM,
    including saving and loading simulation states.
    """
    
    def __init__(self, brain: Brain):
        """
        Initialize state exporter.
        
        Args:
            brain: Brain to export state from
        """
        self.brain = brain
    
    def export_state(self, filepath: str, include_config: bool = True,
                    include_metrics: bool = True, include_development: bool = True):
        """
        Export simulation state to file.
        
        Args:
            filepath: Path to export file
            include_config: Whether to include configuration
            include_metrics: Whether to include performance metrics
            include_development: Whether to include development state
        """
        import json
        
        state = {
            'timestamp': time.time(),
            'brain': {}
        }
        
        if include_config and hasattr(self.brain, '_config'):
            state['brain']['config'] = self.brain._config.data if hasattr(self.brain._config, 'data') else {}
        
        if include_metrics:
            state['brain']['metrics'] = {
                'step_count': getattr(self.brain, '_step_count', 0),
                'total_spikes': getattr(self.brain, '_total_spikes', 0),
                'active_neurons': getattr(self.brain, '_active_neurons', 0),
                'current_stage': getattr(self.brain, '_current_stage', 0)
            }
        
        if include_development:
            state['brain']['development'] = {
                'stage': self.brain.getDevelopmentalStage().value,
                'firing_rate': self.brain.getAverageFiringRate(),
                'total_neurons': self.brain.getTotalNeuronCount(),
                'total_synapses': self.brain.getTotalSynapseCount()
            }
        
        with open(filepath, 'w') as f:
            json.dump(state, f, indent=2)
        
        print(f"State exported to {filepath}")
    
    def import_state(self, filepath: str):
        """
        Import simulation state from file.
        
        Args:
            filepath: Path to import file
        """
        import json
        
        with open(filepath, 'r') as f:
            state = json.load(f)
        
        if 'brain' in state:
            print("State import functionality limited - requires C++ implementation")
            print(f"Imported state from {filepath}")
        
        print(f"State imported from {filepath}")

# Global instances
__all__ = [
    'SimulationStep',
    'SimulationManager',
    'PerformanceProfiler',
    'StateExporter'
]
