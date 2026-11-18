"""
NLM Python Bindings - Batch Operations and Performance Monitoring

This module provides batch operations and performance monitoring tools for the NLM
(Neural Learning Machine) Python bindings, enabling efficient processing of large
scale simulations and comprehensive performance analysis.
"""

import time
import numpy as np
from typing import Any, Dict, List, Optional, Union, Callable, Tuple
from dataclasses import dataclass, field
from collections import defaultdict, deque
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import threading
import json
import warnings

from .nlm_errors import (
    NLMError,
    SimulationError,
    MemoryError,
    PlasticityError,
    DevelopmentError,
    NeuromodulationError,
    get_recovery_manager,
    SafeNLMOperation
)

@dataclass
class PerformanceMetrics:
    """Performance metrics container for monitoring NLM operations."""
    
    # Timing metrics
    operation_times: Dict[str, List[float]] = field(default_factory=dict)
    simulation_time: float = 0.0
    batch_processing_time: float = 0.0
    
    # Memory metrics
    memory_usage: Dict[str, float] = field(default_factory=dict)
    neuron_memory: float = 0.0
    synapse_memory: float = 0.0
    
    # Performance metrics
    operations_per_second: float = 0.0
    neurons_per_second: float = 0.0
    spikes_per_second: float = 0.0
    
    # Resource metrics
    cpu_usage: float = 0.0
    memory_efficiency: float = 0.0
    
    # Tracking
    operation_count: Dict[str, int] = field(default_factory=lambda: defaultdict(int))
    start_time: float = 0.0
    end_time: float = 0.0
    
    def __post_init__(self):
        if self.start_time == 0.0:
            self.start_time = time.time()
    
    def record_operation_time(self, operation: str, duration: float) -> None:
        """Record operation timing."""
        if operation not in self.operation_times:
            self.operation_times[operation] = []
        self.operation_times[operation].append(duration)
        self.operation_count[operation] += 1
    
    def get_average_time(self, operation: str) -> float:
        """Get average time for a specific operation."""
        if operation not in self.operation_times or not self.operation_times[operation]:
            return 0.0
        return sum(self.operation_times[operation]) / len(self.operation_times[operation])
    
    def calculate_performance_metrics(self) -> None:
        """Calculate derived performance metrics."""
        if self.end_time == 0.0:
            self.end_time = time.time()
        
        total_duration = self.end_time - self.start_time
        if total_duration > 0:
            self.operations_per_second = sum(self.operation_count.values()) / total_duration
            self.neurons_per_second = 0  # Would need actual neuron data
            self.spikes_per_second = 0  # Would need actual spike data
        
        # Calculate memory efficiency
        total_memory = sum(self.memory_usage.values())
        if total_memory > 0:
            self.memory_efficiency = (self.neuron_memory + self.synapse_memory) / total_memory
class BatchProcessor:
    """Efficient batch processing for NLM operations."""
    
    def __init__(self, max_workers: int = 4, use_processes: bool = False):
        """
        Initialize batch processor.
        
        Args:
            max_workers: Maximum number of worker threads/processes
            use_processes: Whether to use processes instead of threads
        """
        self.max_workers = max_workers
        self.use_processes = use_processes
        self._executor = None
        self._metrics = PerformanceMetrics()
        self._lock = threading.Lock()
    
    def __enter__(self):
        """Context manager entry."""
        executor_type = ProcessPoolExecutor if self.use_processes else ThreadPoolExecutor
        self._executor = executor_type(max_workers=self.max_workers)
        return self
    
    def __exit__(self, exc_type, exc_val, exc_tb):
        """Context manager exit."""
        if self._executor:
            self._executor.shutdown(wait=True)
            self._executor = None
    
    def batch_step(self, brain, steps: List[int], times: Optional[List[float]] = None) -> List[Any]:
        """
        Execute multiple simulation steps in batch.
        
        Args:
            brain: NLM brain object
            steps: List of step numbers
            times: Optional list of corresponding times
            
        Returns:
            List of step results
            
        Raises:
            SimulationError: If batch execution fails
        """
        if times is None:
            times = [float(step) for step in steps]
        
        start_time = time.time()
        results = []
        
        try:
            for i, (step, time_val) in enumerate(zip(steps, times)):
                step_start = time.time()
                
                try:
                    if time_val is not None:
                        brain.step(step, time_val)
                    else:
                        brain.step(step)
                    
                    results.append({
                        'step': step,
                        'time': time_val,
                        'success': True,
                        'error': None
                    })
                except Exception as e:
                    results.append({
                        'step': step,
                        'time': time_val,
                        'success': False,
                        'error': str(e)
                    })
                
                step_end = time.time()
                self._metrics.record_operation_time('step', step_end - step_start)
        
        except Exception as e:
            raise SimulationError(
                f"Batch step execution failed: {str(e)}",
                step_number=steps[-1] if steps else 0,
                component='batch_step'
            )
        
        batch_end = time.time()
        self._metrics.record_operation_time('batch_step', batch_end - start_time)
        
        return results
    
    def batch_inject_current(self, brain, neuron_groups: List[List[Any]], currents: List[float]) -> None:
        """
        Inject current into multiple neuron groups in batch.
        
        Args:
            brain: NLM brain object
            neuron_groups: List of neuron group identifiers (NeuronId, type, etc.)
            currents: List of current values
            
        Raises:
            MemoryError: If memory bounds are exceeded
        """
        if len(neuron_groups) != len(currents):
            raise MemoryError("Neuron groups and currents must have same length")
        
        start_time = time.time()
        
        try:
            for group, current in zip(neuron_groups, currents):
                if isinstance(group, tuple) and group[0] == 'type':
                    # Inject to all neurons of a type
                    neuron_type = group[1]
                    brain.injectCurrentToNeurons(neuron_type, current)
                elif isinstance(group, tuple) and group[0] == 'id':
                    # Inject to specific neuron
                    neuron_id = group[1]
                    brain.injectCurrent(neuron_id, current)
                else:
                    raise MemoryError(f"Invalid neuron group format: {group}")
                
                self._metrics.record_operation_time('neuron_injection', 0.0)  # Individual timing handled elsewhere
        
        except Exception as e:
            raise MemoryError(
                f"Batch current injection failed: {str(e)}",
                neuron_id=str(neuron_groups[-1]) if neuron_groups else None
            )
        
        batch_end = time.time()
        self._metrics.record_operation_time('batch_inject_current', batch_end - start_time)
    
    def batch_update_synapses(self, brain, synaptic_updates: List[Tuple[Any, float]]) -> None:
        """
        Update multiple synapses in batch.
        
        Args:
            brain: NLM brain object
            synaptic_updates: List of (synapse_id, weight) tuples
            
        Raises:
            PlasticityError: If synaptic updates fail
        """
        start_time = time.time()
        
        try:
            # Note: This assumes access to internal synapse manipulation APIs
            # In a real implementation, this would interface with the C++ layer
            for synapse_id, weight in synaptic_updates:
                # Implementation depends on available C++ APIs
                pass
            
            batch_end = time.time()
            self._metrics.record_operation_time('batch_update_synapses', batch_end - start_time)
        
        except Exception as e:
            raise PlasticityError(
                f"Batch synaptic update failed: {str(e)}",
                plasticity_rule='batch_update'
            )
    
    def batch_region_operations(self, brain, operations: List[Tuple[str, Any]]) -> List[Any]:
        """
        Perform multiple region operations in batch.
        
        Args:
            brain: NLM brain object
            operations: List of (operation_name, args) tuples
            
        Returns:
            List of operation results
            
        Raises:
            DevelopmentError: If region operations fail
        """
        start_time = time.time()
        results = []
        
        try:
            for operation_name, args in operations:
                if operation_name == 'add_region':
                    region_name = args if len(args) == 1 else args[0]
                    region_id = brain.addRegion(region_name)
                    results.append(region_id)
                elif operation_name == 'get_region':
                    region_id = args if len(args) == 1 else args[0]
                    region = brain.getRegion(region_id)
                    results.append(region)
                elif operation_name == 'get_region_count':
                    count = brain.getRegionCount()
                    results.append(count)
                else:
                    raise DevelopmentError(f"Unknown region operation: {operation_name}")
                
                self._metrics.record_operation_time('region_operation', 0.0)
            
            batch_end = time.time()
            self._metrics.record_operation_time('batch_region_operations', batch_end - start_time)
        
        except Exception as e:
            raise DevelopmentError(
                f"Batch region operation failed: {str(e)}",
                current_stage='batch_processing'
            )
        
        return results
    
    def get_metrics(self) -> PerformanceMetrics:
        """Get performance metrics."""
        return self._metrics
    
    def reset_metrics(self) -> None:
        """Reset performance metrics."""
        self._metrics = PerformanceMetrics()
class NLMPerformanceMonitor:
    """Comprehensive performance monitoring for NLM simulations."""
    
    def __init__(self, enable_profiling: bool = True, record_interval: float = 0.1):
        """
        Initialize performance monitor.
        
        Args:
            enable_profiling: Whether to enable detailed profiling
            record_interval: Interval between performance measurements (seconds)
        """
        self.enable_profiling = enable_profiling
        self.record_interval = record_interval
        self._monitoring = False
        self._monitor_thread = None
        self._performance_data = deque(maxlen=1000)
        self._start_time = None
        self._last_record_time = None
        self._lock = threading.Lock()
    
    def start_monitoring(self) -> None:
        """Start performance monitoring."""
        self._monitoring = True
        self._start_time = time.time()
        self._last_record_time = self._start_time
        
        if self.enable_profiling:
            self._monitor_thread = threading.Thread(target=self._monitoring_loop, daemon=True)
            self._monitor_thread.start()
    
    def stop_monitoring(self) -> None:
        """Stop performance monitoring."""
        self._monitoring = False
        if self._monitor_thread:
            self._monitor_thread.join(timeout=1.0)
            self._monitor_thread = None
    
    def _monitoring_loop(self) -> None:
        """Background monitoring loop."""
        while self._monitoring:
            try:
                self._record_performance_data()
                time.sleep(self.record_interval)
            except Exception:
                break  # Exit loop on any error
    
    def _record_performance_data(self) -> None:
        """Record current performance data."""
        current_time = time.time()
        
        # Get current metrics
        perf_data = {
            'timestamp': current_time,
            'elapsed_time': current_time - self._start_time,
            'monitoring_time': current_time - self._last_record_time
        }
        
        # Add process-specific metrics
        process = threading.current_thread()
        perf_data['thread_id'] = process.ident
        perf_data['thread_name'] = process.name
        
        with self._lock:
            self._performance_data.append(perf_data)
            self._last_record_time = current_time
    
    def add_custom_metric(self, name: str, value: Any, timestamp: Optional[float] = None) -> None:
        """
        Add a custom performance metric.
        
        Args:
            name: Metric name
            value: Metric value
            timestamp: Optional timestamp (defaults to current time)
        """
        current_time = timestamp or time.time()
        
        with self._lock:
            metric_data = {
                'timestamp': current_time,
                'name': name,
                'value': value,
                'type': type(value).__name__
            }
            self._performance_data.append(metric_data)
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary statistics."""
        if not self._performance_data:
            return {}
        
        # Group data by type
        timing_data = [d for d in self._performance_data if 'elapsed_time' in d]
        custom_data = [d for d in self._performance_data if 'name' in d]
        
        summary = {
            'total_samples': len(self._performance_data),
            'monitoring_duration': (time.time() - self._start_time) if self._start_time else 0,
            'data_points': {
                'timing': len(timing_data),
                'custom': len(custom_data)
            }
        }
        
        if timing_data:
            elapsed_times = [d['elapsed_time'] for d in timing_data]
            summary['timing_stats'] = {
                'min': min(elapsed_times),
                'max': max(elapsed_times),
                'mean': sum(elapsed_times) / len(elapsed_times),
                'std': np.std(elapsed_times) if len(elapsed_times) > 1 else 0.0
            }
        
        if custom_data:
            # Group by metric name
            metric_groups = defaultdict(list)
            for d in custom_data:
                metric_groups[d['name']].append(d['value'])
            
            summary['custom_metrics'] = {}
            for name, values in metric_groups.items():
                if values and all(isinstance(v, (int, float)) for v in values):
                    summary['custom_metrics'][name] = {
                        'count': len(values),
                        'min': min(values),
                        'max': max(values),
                        'mean': sum(values) / len(values),
                        'std': np.std(values) if len(values) > 1 else 0.0
                    }
                else:
                    summary['custom_metrics'][name] = {
                        'count': len(values),
                        'values': values[:10]  # Show first 10 values
                    }
        
        return summary
    
    def export_performance_data(self, filepath: str) -> None:
        """
        Export performance data to file.
        
        Args:
            filepath: File path to export data
        """
        data = {
            'metadata': {
                'start_time': self._start_time,
                'end_time': time.time(),
                'duration': (time.time() - self._start_time) if self._start_time else 0,
                'total_samples': len(self._performance_data)
            },
            'data': list(self._performance_data)
        }
        
        with open(filepath, 'w') as f:
            json.dump(data, f, indent=2, default=str)
class NLMExperimentManager:
    """Manager for NLM experiments with reproducible workflows."""
    
    def __init__(self, experiment_name: str, config: Optional[Dict[str, Any]] = None):
        """
        Initialize experiment manager.
        
        Args:
            experiment_name: Name of the experiment
            config: Configuration dictionary
        """
        self.experiment_name = experiment_name
        self.config = config or {}
        self.experiment_history: List[Dict[str, Any]] = []
        self.performance_monitor = NLMPerformanceMonitor()
        self.batch_processor = BatchProcessor()
    
    def run_experiment(self, steps: int, description: Optional[str] = None, 
                      use_batch: bool = True) -> Dict[str, Any]:
        """
        Run an NLM experiment.
        
        Args:
            steps: Number of simulation steps
            description: Optional experiment description
            use_batch: Whether to use batch processing
            
        Returns:
            Experiment results dictionary
            """
        experiment_id = f"{self.experiment_name}_{len(self.experiment_history) + 1}"
        start_time = time.time()
        
        # Start performance monitoring
        self.performance_monitor.start_monitoring()
        
        experiment_result = {
            'experiment_id': experiment_id,
            'name': self.experiment_name,
            'description': description or f"Experiment {experiment_id}",
            'config': self.config.copy(),
            'steps': steps,
            'use_batch': use_batch,
            'start_time': start_time,
            'results': {},
            'errors': [],
            'performance': {},
            'metadata': {}
        }
        
        try:
            # Create NLM instance
            from pynlm import NLM
            nlm = NLM(self.config)
            
            # Run simulation
            if use_batch:
                with self.batch_processor as processor:
                    step_batch_size = max(1, steps // 10)  # 10 batches
                    step_indices = list(range(steps))
                    
                    batch_results = []
                    for i in range(0, steps, step_batch_size):
                        batch_steps = step_indices[i:i + step_batch_size]
                        batch_result = processor.batch_step(nlm.brain, batch_steps)
                        batch_results.extend(batch_result)
                    
                    experiment_result['results']['batch_results'] = batch_results
            else:
                # Run single-step simulation
                for step in range(steps):
                    nlm.brain.step(step)
                    if step % 10 == 0:  # Record every 10 steps
                        experiment_result['results'][f'step_{step}'] = {
                            'neuron_count': nlm.brain.get_neuron_count(),
                            'region_count': nlm.brain.get_region_count(),
                            'firing_rate': nlm.brain.get_average_firing_rate()
                        }
            
            # Record final status
            experiment_result['results']['final_status'] = nlm.get_status()
            
            # Get performance metrics
            experiment_result['performance'] = {
                'summary': self.performance_monitor.get_performance_summary(),
                'batch_metrics': self.batch_processor.get_metrics().__dict__
            }
            
            # Add metadata
            experiment_result['metadata'] = {
                'python_version': sys.version,
                'nlm_version': '0.1.0',
                'config_keys': list(self.config.keys()),
                'total_steps': steps,
                'batch_used': use_batch
            }
            
        except Exception as e:
            experiment_result['errors'].append({
                'type': type(e).__name__,
                'message': str(e),
                'timestamp': time.time()
            })
            raise NLMError(f"Experiment {experiment_id} failed: {str(e)}")
        
        finally:
            # Stop monitoring
            self.performance_monitor.stop_monitoring()
            
            # Record experiment
            experiment_result['end_time'] = time.time()
            experiment_result['duration'] = experiment_result['end_time'] - start_time
            self.experiment_history.append(experiment_result)
        
        return experiment_result
    
    def get_experiment_history(self) -> List[Dict[str, Any]]:
        """Get experiment history."""
        return self.experiment_history.copy()
    
    def get_experiment_by_id(self, experiment_id: str) -> Optional[Dict[str, Any]]:
        """
        Get experiment by ID.
        
        Args:
            experiment_id: Experiment identifier
            
        Returns:
            Experiment result or None if not found
        """
        for experiment in self.experiment_history:
            if experiment['experiment_id'] == experiment_id:
                return experiment.copy()
        return None
    
    def export_experiment_results(self, experiment_id: str, filepath: str) -> None:
        """
        Export experiment results to file.
        
        Args:
            experiment_id: Experiment identifier
            filepath: File path to export results
        """
        experiment = self.get_experiment_by_id(experiment_id)
        if experiment:
            with open(filepath, 'w') as f:
                json.dump(experiment, f, indent=2, default=str)
        else:
            raise NLMError(f"Experiment {experiment_id} not found")
# Convenience functions for common workflows

def create_batch_processor(max_workers: int = 4, use_processes: bool = False) -> BatchProcessor:
    """
    Create a batch processor for efficient NLM operations.
    
    Args:
        max_workers: Maximum number of worker threads/processes
        use_processes: Whether to use processes instead of threads
        
    Returns:
        BatchProcessor instance
    """
    return BatchProcessor(max_workers=max_workers, use_processes=use_processes)

def create_performance_monitor(enable_profiling: bool = True, 
                              record_interval: float = 0.1) -> NLMPerformanceMonitor:
    """
    Create a performance monitor for NLM operations.
    
    Args:
        enable_profiling: Whether to enable detailed profiling
        record_interval: Interval between performance measurements
        
    Returns:
        NLMPerformanceMonitor instance
    """
    return NLMPerformanceMonitor(enable_profiling=enable_profiling, 
                                record_interval=record_interval)

def create_experiment_manager(experiment_name: str, config: Optional[Dict[str, Any]] = None) -> NLMExperimentManager:
    """
    Create an experiment manager for NLM experiments.
    
    Args:
        experiment_name: Name of the experiment
        config: Configuration dictionary
        
    Returns:
        NLMExperimentManager instance
    """
    return NLMExperimentManager(experiment_name=experiment_name, config=config)

# Common experiment templates

EXPERIMENT_TEMPLATES = {
    'learning_curve': {
        'description': 'Run learning curve experiment to analyze learning dynamics',
        'steps': 1000,
        'config': {
            'neuron_count': 500,
            'region_count': 2,
            'connection_probability': 0.1
        }
    },
    'plasticity_test': {
        'description': 'Test plasticity rule effects',
        'steps': 500,
        'config': {
            'neuron_count': 200,
            'region_count': 1,
            'connection_probability': 0.2,
            'plasticity_enabled': True
        }
    },
    'development_study': {
        'description': 'Study developmental stage transitions',
        'steps': 800,
        'config': {
            'neuron_count': 300,
            'region_count': 3,
            'connection_probability': 0.05,
            'development_enabled': True
        }
    }
}

def run_experiment_template(template_name: str, custom_config: Optional[Dict[str, Any]] = None) -> NLMExperimentManager:
    """
    Run an experiment using a predefined template.
    
    Args:
        template_name: Name of the experiment template
        custom_config: Optional custom configuration to override template
        
    Returns:
        NLMExperimentManager instance
        
    Raises:
        NLMError: If template not found
    """
    if template_name not in EXPERIMENT_TEMPLATES:
        available_templates = ', '.join(EXPERIMENT_TEMPLATES.keys())
        raise NLMError(f"Template '{template_name}' not found. Available templates: {available_templates}")
    
    template = EXPERIMENT_TEMPLATES[template_name].copy()
    
    # Merge with custom config if provided
    if custom_config:
        template['config'].update(custom_config)
    
    experiment_manager = create_experiment_manager(
        experiment_name=template_name,
        config=template['config']
    )
    
    return experiment_manager
# Performance benchmarks

BENCHMARKS = {
    'step_performance': {
        'description': 'Benchmark single-step performance',
        'function': 'benchmark_step_performance',
        'steps': 100,
        'repeats': 5
    },
    'batch_performance': {
        'description': 'Benchmark batch processing performance',
        'function': 'benchmark_batch_performance',
        'steps': 1000,
        'batch_size': 100,
        'repeats': 3
    },
    'memory_efficiency': {
        'description': 'Benchmark memory efficiency',
        'function': 'benchmark_memory_efficiency',
        'steps': 200,
        'memory_check_interval': 50
    }
}

def run_benchmark(benchmark_name: str, nlm_instance: Any) -> Dict[str, Any]:
    """
    Run a performance benchmark on NLM instance.
    
    Args:
        benchmark_name: Name of the benchmark
        nlm_instance: NLM instance to benchmark
        
    Returns:
        Benchmark results dictionary
        
    Raises:
        NLMError: If benchmark not found or execution fails
    """
    if benchmark_name not in BENCHMARKS:
        available_benchmarks = ', '.join(BENCHMARKS.keys())
        raise NLMError(f"Benchmark '{benchmark_name}' not found. Available benchmarks: {available_benchmarks}")
    
    benchmark_config = BENCHMARKS[benchmark_name].copy()
    benchmark_function = globals().get(benchmark_config['function'])
    
    if not benchmark_function:
        raise NLMError(f"Benchmark function {benchmark_config['function']} not found")
    
    try:
        return benchmark_function(nlm_instance, **{k: v for k, v in benchmark_config.items() if k != 'function' and k != 'description'})
    except Exception as e:
        raise NLMError(f"Benchmark {benchmark_name} failed: {str(e)}")

def benchmark_step_performance(nlm_instance: Any, steps: int = 100, repeats: int = 5) -> Dict[str, Any]:
    """
    Benchmark single-step performance.
    
    Args:
        nlm_instance: NLM instance to benchmark
        steps: Number of steps per benchmark run
        repeats: Number of benchmark repeats
        
    Returns:
        Benchmark results
    """
    times = []
    
    for repeat in range(repeats):
        start_time = time.time()
        
        for step in range(steps):
            nlm_instance.brain.step(step)
        
        end_time = time.time()
        times.append(end_time - start_time)
    
    return {
        'benchmark_name': 'step_performance',
        'steps_per_run': steps,
        'repeats': repeats,
        'total_steps': steps * repeats,
        'times': times,
        'mean_time': sum(times) / len(times),
        'min_time': min(times),
        'max_time': max(times),
        'std_time': np.std(times) if len(times) > 1 else 0.0,
        'steps_per_second': (steps * repeats) / sum(times) if sum(times) > 0 else 0
    }

def benchmark_batch_performance(nlm_instance: Any, steps: int = 1000, 
                              batch_size: int = 100, repeats: int = 3) -> Dict[str, Any]:
    """
    Benchmark batch processing performance.
    
    Args:
        nlm_instance: NLM instance to benchmark
        steps: Total number of steps
        batch_size: Number of steps per batch
        repeats: Number of benchmark repeats
        
    Returns:
        Benchmark results
    """
    from .nlm_errors import BatchProcessor
    
    times = []
    
    for repeat in range(repeats):
        with BatchProcessor() as processor:
            start_time = time.time()
            
            step_indices = list(range(steps))
            batch_results = processor.batch_step(nlm_instance.brain, step_indices)
            
            end_time = time.time()
            times.append(end_time - start_time)
    
    return {
        'benchmark_name': 'batch_performance',
        'total_steps': steps,
        'batch_size': batch_size,
        'repeats': repeats,
        'batches_per_run': (steps + batch_size - 1) // batch_size,
        'times': times,
        'mean_time': sum(times) / len(times),
        'min_time': min(times),
        'max_time': max(times),
        'std_time': np.std(times) if len(times) > 1 else 0.0,
        'steps_per_second': steps / sum(times) if sum(times) > 0 else 0
    }

def benchmark_memory_efficiency(nlm_instance: Any, steps: int = 200, 
                              memory_check_interval: int = 50) -> Dict[str, Any]:
    """
    Benchmark memory efficiency.
    
    Args:
        nlm_instance: NLM instance to benchmark
        steps: Total number of steps
        memory_check_interval: Interval between memory checks
        
    Returns:
        Benchmark results
    """
    import psutil
    import os
    
    process = psutil.Process(os.getpid())
    memory_samples = []
    step_samples = []
    
    start_time = time.time()
    
    for step in range(steps):
        nlm_instance.brain.step(step)
        
        if step % memory_check_interval == 0:
            memory_info = process.memory_info()
            memory_samples.append({
                'step': step,
                'rss': memory_info.rss / 1024 / 1024,  # MB
                'vms': memory_info.vms / 1024 / 1024,  # MB
                'shared': memory_info.shared / 1024 / 1024,  # MB
                'text': memory_info.text / 1024 / 1024,  # MB
                'data': memory_info.data / 1024 / 1024,  # MB
                'dirty': memory_info.dirty / 1024 / 1024   # MB
            })
            step_samples.append(step)
    
    end_time = time.time()
    
    return {
        'benchmark_name': 'memory_efficiency',
        'total_steps': steps,
        'memory_samples': memory_samples,
        'step_samples': step_samples,
        'duration': end_time - start_time,
        'memory_growth': (memory_samples[-1]['rss'] - memory_samples[0]['rss']) if memory_samples else 0,
        'steps_per_second': steps / (end_time - start_time) if (end_time - start_time) > 0 else 0
    }
# Export all public classes and functions

__all__ = [
    # Core classes
    'PerformanceMetrics',
    'BatchProcessor',
    'NLMPerformanceMonitor',
    'NLMExperimentManager',
    
    # Convenience functions
    'create_batch_processor',
    'create_performance_monitor',
    'create_experiment_manager',
    'run_experiment_template',
    'run_benchmark',
    
    # Benchmark functions
    'benchmark_step_performance',
    'benchmark_batch_performance',
    'benchmark_memory_efficiency',
    
    # Templates
    'EXPERIMENT_TEMPLATES',
    'BENCHMARKS'
]
