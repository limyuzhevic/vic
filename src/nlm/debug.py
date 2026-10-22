"""
Debug module for NLM Python bindings

This module provides debugging and profiling functionality for neural network simulations.
"""

import json
import time
import psutil
import os
from typing import Dict, Any, List, Optional, Union
from enum import Enum
import numpy as np

class DebugLevel(Enum):
    NONE = "none"
    BASIC = "basic"
    DETAILED = "detailed"
    VERBOSE = "verbose"

class PerformanceMetrics:
    """Container for performance metrics."""
    
    def __init__(self, operation_name: str):
        self.operation_name = operation_name
        self.start_time = time.time()
        self.end_time = None
        self.duration = None
        self.memory_usage = 0
        self.cpu_usage = 0
        self.thread_id = os.getpid()
    
    def stop(self):
        """Stop timing the operation."""
        self.end_time = time.time()
        self.duration = self.end_time - self.start_time
        self.memory_usage = psutil.virtual_memory().used
        self.cpu_usage = psutil.cpu_percent()
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'operation_name': self.operation_name,
            'start_time': self.start_time,
            'end_time': self.end_time,
            'duration': self.duration,
            'memory_usage': self.memory_usage,
            'cpu_usage': self.cpu_usage,
            'thread_id': self.thread_id
        }

class MemoryTracker:
    """Track memory allocations and deallocations."""
    
    def __init__(self):
        self.allocations = []
        self.deallocations = []
        self.current_usage = 0
    
    def track_allocation(self, size: int, allocation_site: str = ""):
        """Track memory allocation."""
        allocation = {
            'timestamp': time.time(),
            'size': size,
            'allocation_site': allocation_site,
            'thread_id': os.getpid()
        }
        self.allocations.append(allocation)
        self.current_usage += size
    
    def track_deallocation(self, size: int):
        """Track memory deallocation."""
        deallocation = {
            'timestamp': time.time(),
            'size': size,
            'thread_id': os.getpid()
        }
        self.deallocations.append(deallocation)
        self.current_usage -= size
    
    def get_total_allocated(self) -> int:
        """Get total allocated memory."""
        return sum(allocation['size'] for allocation in self.allocations)
    
    def get_current_usage(self) -> int:
        """Get current memory usage."""
        return self.current_usage
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'allocations': self.allocations,
            'deallocations': self.deallocations,
            'current_usage': self.current_usage,
            'total_allocated': self.get_total_allocated()
        }

class SpikeProfiler:
    """Profile spike activity and STDP events."""
    
    def __init__(self):
        self.spike_events = []
        self.pre_post_pairs = []
        self.is_profiling = False
        self.session_start = time.time()
        self.session_id = f"spike_profile_{int(self.session_start)}"
    
    def start_profiling(self):
        """Start profiling."""
        self.is_profiling = True
        self.session_start = time.time()
        self.session_id = f"spike_profile_{int(self.session_start)}"
    
    def stop_profiling(self):
        """Stop profiling."""
        self.is_profiling = False
    
    def record_spike(self, neuron_id: int, timestamp: float, step: int, spike_type: str = "regular"):
        """Record a spike event."""
        spike_event = {
            'neuron_id': neuron_id,
            'timestamp': timestamp,
            'step': step,
            'spike_type': spike_type,
            'session_id': self.session_id
        }
        self.spike_events.append(spike_event)
    
    def record_pre_post_spike(self, synapse_id: int, pre_neuron_id: int, post_neuron_id: int, 
                              pre_time: float, post_time: float):
        """Record pre-post spike pair for STDP analysis."""
        time_difference = post_time - pre_time
        pair = {
            'synapse_id': synapse_id,
            'pre_neuron_id': pre_neuron_id,
            'post_neuron_id': post_neuron_id,
            'pre_time': pre_time,
            'post_time': post_time,
            'time_difference': time_difference,
            'session_id': self.session_id
        }
        self.pre_post_pairs.append(pair)
    
    def generate_spike_report(self) -> Dict[str, Any]:
        """Generate spike activity report."""
        if not self.spike_events:
            return {'total_spikes': 0, 'firing_rate': 0.0}
        
        session_duration = time.time() - self.session_start
        
        # Analyze spike times
        spike_times = [event['timestamp'] for event in self.spike_events]
        
        # Calculate statistics
        spike_count_by_neuron = {}
        for event in self.spike_events:
            neuron_id = event['neuron_id']
            if neuron_id not in spike_count_by_neuron:
                spike_count_by_neuron[neuron_id] = 0
            spike_count_by_neuron[neuron_id] += 1
        
        # Calculate firing rates
        firing_rates = {}
        for neuron_id, count in spike_count_by_neuron.items():
            firing_rates[neuron_id] = count / session_duration if session_duration > 0 else 0.0
        
        return {
            'session_id': self.session_id,
            'session_duration': session_duration,
            'total_spikes': len(self.spike_events),
            'unique_neurons': len(spike_count_by_neuron),
            'firing_rates': firing_rates,
            'spike_count_by_neuron': spike_count_by_neuron,
            'spike_types': self._analyze_spike_types()
        }
    
    def generate_stdp_report(self) -> Dict[str, Any]:
        """Generate STDP analysis report."""
        if not self.pre_post_pairs:
            return {'total_pairs': 0, 'valid_pairs': 0}
        
        # Filter valid pre-post pairs
        valid_pairs = []
        for pair in self.pre_post_pairs:
            if pair['time_difference'] > 0:  # Positive time difference (pre before post)
                valid_pairs.append(pair)
        
        # Analyze timing
        if valid_pairs:
            time_differences = [pair['time_difference'] for pair in valid_pairs]
            
            # Categorize timing for STDP
            early_ltp = sum(1 for td in time_differences if td < 10.0)  # Early post (within 10ms)
            late_ltp = sum(1 for td in time_differences if 10.0 <= td < 50.0)  # Late post
            early_ltd = sum(1 for td in time_differences if td >= 50.0)  # Long time difference
            
            return {
                'session_id': self.session_id,
                'total_pairs': len(self.pre_post_pairs),
                'valid_pairs': len(valid_pairs),
                'early_ltp': early_ltp,
                'late_ltp': late_ltp,
                'early_ltd': early_ltd,
                'average_time_difference': np.mean(time_differences) if time_differences else 0.0,
                'time_difference_std': np.std(time_differences) if time_differences else 0.0
            }
        else:
            return {
                'session_id': self.session_id,
                'total_pairs': len(self.pre_post_pairs),
                'valid_pairs': 0
            }
    
    def _analyze_spike_types(self) -> Dict[str, int]:
        """Analyze spike types."""
        spike_types = {}
        for event in self.spike_events:
            spike_type = event['spike_type']
            if spike_type not in spike_types:
                spike_types[spike_type] = 0
            spike_types[spike_type] += 1
        return spike_types
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'session_id': self.session_id,
            'is_profiling': self.is_profiling,
            'session_start': self.session_start,
            'spike_events': self.spike_events,
            'pre_post_pairs': self.pre_post_pairs
        }

class ConfigurationProfiler:
    """Profile configuration and performance."""
    
    def __init__(self, config: Dict[str, Any]):
        self.config = config
        self.performance_history = []
        self.session_id = f"config_profile_{int(time.time())}"
    
    def profile_performance(self, brain_results: Dict[str, Any], description: str = ""):
        """Profile performance of a brain simulation."""
        # Calculate performance metrics
        metrics = {
            'session_id': self.session_id,
            'description': description,
            'timestamp': time.time(),
            'simulation_duration': brain_results.get('simulation_time', 0.0),
            'total_spikes': brain_results.get('total_spikes', 0),
            'total_neurons': brain_results.get('total_neurons', 0),
            'total_synapses': brain_results.get('total_synapses', 0),
            'average_firing_rate': brain_results.get('average_firing_rate', 0.0),
            'weight_change_magnitude': brain_results.get('weight_change_magnitude', 0.0),
            'memory_footprint': brain_results.get('memory_footprint', 0),
            'configuration': self.config
        }
        
        self.performance_history.append(metrics)
    
    def generate_configuration_report(self) -> Dict[str, Any]:
        """Generate configuration profile report."""
        if not self.performance_history:
            return {'total_runs': 0}
        
        # Calculate summary statistics
        total_runs = len(self.performance_history)
        
        # Aggregate metrics
        total_spikes = sum(h['total_spikes'] for h in self.performance_history)
        total_neurons = sum(h['total_neurons'] for h in self.performance_history)
        total_synapses = sum(h['total_synapses'] for h in self.performance_history)
        
        # Calculate averages
        avg_firing_rate = total_spikes / (total_runs * 1000)  # Assuming 1 second per run
        avg_memory_footprint = total_neurons * 64  # Approximate
        
        return {
            'session_id': self.session_id,
            'total_runs': total_runs,
            'average_firing_rate': avg_firing_rate,
            'average_memory_footprint': avg_memory_footprint,
            'configuration': self.config,
            'performance_history': self.performance_history
        }
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert to dictionary."""
        return {
            'session_id': self.session_id,
            'config': self.config,
            'performance_history': self.performance_history
        }

class DebugManager:
    """Main debug manager for coordinating debugging and profiling."""
    
    def __init__(self):
        self.debug_level = DebugLevel.DETAILED
        self.performance_metrics = []
        self.memory_tracker = MemoryTracker()
        self.spike_profiler = SpikeProfiler()
        self.config_profiler = None
        self.is_initialized = False
        self.session_id = f"debug_session_{int(time.time())}"
    
    def initialize(self, level: Union[str, DebugLevel] = DebugLevel.DETAILED):
        """
        Initialize debug manager.
        
        Args:
            level: Debug level ("none", "basic", "detailed", "verbose")
        """
        if isinstance(level, str):
            self.debug_level = DebugLevel(level.lower())
        else:
            self.debug_level = level
        
        self.is_initialized = True
        self.spike_profiler.start_profiling()
        
        print(f"Debug manager initialized with level: {self.debug_level.value}")
    
    def shutdown(self):
        """Shutdown debug manager."""
        if self.is_initialized:
            self.spike_profiler.stop_profiling()
            self.is_initialized = False
            print("Debug manager shutdown")
    
    def set_debug_level(self, level: Union[str, DebugLevel]):
        """Set debug level."""
        if isinstance(level, str):
            self.debug_level = DebugLevel(level.lower())
        else:
            self.debug_level = level
    
    def get_debug_level(self) -> DebugLevel:
        """Get current debug level."""
        return self.debug_level
    
    def start_performance_tracking(self, operation_name: str) -> PerformanceMetrics:
        """Start performance tracking."""
        if self.debug_level == DebugLevel.NONE:
            return None
        
        metrics = PerformanceMetrics(operation_name)
        self.performance_metrics.append(metrics)
        return metrics
    
    def stop_performance_tracking(self, metrics: PerformanceMetrics):
        """Stop performance tracking."""
        if metrics:
            metrics.stop()
    
    def get_memory_tracker(self) -> MemoryTracker:
        """Get memory tracker."""
        return self.memory_tracker
    
    def get_spike_profiler(self) -> SpikeProfiler:
        """Get spike profiler."""
        return self.spike_profiler
    
    def get_configuration_profiler(self, config: Dict[str, Any]) -> ConfigurationProfiler:
        """Get configuration profiler."""
        if self.config_profiler is None:
            self.config_profiler = ConfigurationProfiler(config)
        return self.config_profiler
    
    def log_debug(self, message: str, level: Union[str, DebugLevel] = DebugLevel.DETAILED):
        """
        Log debug message.
        
        Args:
            message: Debug message
            level: Message level
        """
        if isinstance(level, str):
            message_level = DebugLevel(level.lower())
        else:
            message_level = level
        
        # Only log messages at the current debug level or higher
        if message_level.value >= self.debug_level.value:
            timestamp = time.time()
            print(f"[{timestamp}] {message_level.value.upper()}: {message}")
    
    def log_performance(self, category: str, operation: str, time_ms: float):
        """
        Log performance measurement.
        
        Args:
            category: Performance category
            operation: Operation name
            time_ms: Execution time in milliseconds
        """
        self.log_debug(f"Performance: {category}.{operation} = {time_ms}ms", DebugLevel.BASIC)
    
    def log_memory(self, category: str, allocated_bytes: int):
        """
        Log memory usage.
n        
        Args:
            category: Memory category
            allocated_bytes: Memory allocated in bytes
        """
        self.log_debug(f"Memory: {category} = {allocated_bytes} bytes", DebugLevel.BASIC)
    
    def generate_debug_report(self, output_dir: str = "debug_reports"):
        """
        Generate debug report.
        
        Args:
            output_dir: Output directory for reports
        """
        import os
        import json
        
        os.makedirs(output_dir, exist_ok=True)
        
        # Generate spike profiler report
        spike_report = self.spike_profiler.generate_spike_report()
        with open(f"{output_dir}/spike_report.json", 'w') as f:
            json.dump(spike_report, f, indent=2)
        
        # Generate STDP report
        stdp_report = self.spike_profiler.generate_stdp_report()
        with open(f"{output_dir}/stdp_report.json", 'w') as f:
            json.dump(stdp_report, f, indent=2)
        
        # Generate performance metrics report
        performance_report = {
            'session_id': self.session_id,
            'performance_metrics': [metrics.to_dict() for metrics in self.performance_metrics],
            'memory_tracker': self.memory_tracker.to_dict()
        }
        with open(f"{output_dir}/performance_report.json", 'w') as f:
            json.dump(performance_report, f, indent=2)
        
        # Generate configuration profiler report if available
        if self.config_profiler:
            config_report = self.config_profiler.generate_configuration_report()
            with open(f"{output_dir}/config_report.json", 'w') as f:
                json.dump(config_report, f, indent=2)
        
        print(f"Debug reports generated in: {output_dir}")
    
    def generate_comprehensive_report(self, output_dir: str = "debug_reports"):
        """
        Generate comprehensive debug report.
        
        Args:
            output_dir: Output directory for reports
        """
        import os
        
        os.makedirs(output_dir, exist_ok=True)
        
        # Generate all reports
        self.generate_debug_report(output_dir)
        
        # Generate summary report
        summary = {
            'session_id': self.session_id,
            'debug_level': self.debug_level.value,
            'total_performance_metrics': len(self.performance_metrics),
            'total_spikes': sum(event['spike_count'] for event in self.spike_profiler.spike_events),
            'total_memory_allocated': self.memory_tracker.get_total_allocated(),
            'current_memory_usage': self.memory_tracker.get_current_usage()
        }
        
        with open(f"{output_dir}/summary_report.json", 'w') as f:
            json.dump(summary, f, indent=2)
        
        # Generate text summary
        text_summary = f"""Debug Summary Report
========================
Session ID: {self.session_id}
Debug Level: {self.debug_level.value}
Total Performance Metrics: {len(self.performance_metrics)}
Total Spikes Recorded: {summary['total_spikes']}
Total Memory Allocated: {summary['total_memory_allocated']} bytes
Current Memory Usage: {summary['current_memory_usage']} bytes

Reports generated:
- spike_report.json: Spike activity analysis
- stdp_report.json: STDP timing analysis
- performance_report.json: Performance metrics
- config_report.json: Configuration profiling (if available)
- summary_report.json: Summary statistics
"""
        
        with open(f"{output_dir}/summary_report.txt", 'w') as f:
            f.write(text_summary)
        
        print(f"Comprehensive debug reports generated in: {output_dir}")
    
    def collect_debug_data(self) -> Dict[str, Any]:
        """Collect current debug data."""
        debug_data = {
            'session_id': self.session_id,
            'debug_level': self.debug_level.value,
            'is_initialized': self.is_initialized,
            'total_performance_metrics': len(self.performance_metrics),
            'memory_tracker': self.memory_tracker.to_dict(),
            'spike_profiler': self.spike_profiler.to_dict()
        }
        
        if self.config_profiler:
            debug_data['config_profiler'] = self.config_profiler.to_dict()
        
        return debug_data

# Global debug manager instance
_global_debug_manager = None

def get_debug_manager() -> DebugManager:
    """Get the global debug manager instance."""
    global _global_debug_manager
    if _global_debug_manager is None:
        _global_debug_manager = DebugManager()
    return _global_debug_manager

def initialize_debug(level: Union[str, DebugLevel] = DebugLevel.DETAILED):
    """
    Initialize global debug manager.
    
    Args:
        level: Debug level
    """
    debug_manager = get_debug_manager()
    debug_manager.initialize(level)

def shutdown_debug():
    """Shutdown global debug manager."""
    debug_manager = get_debug_manager()
    debug_manager.shutdown()

# Version information
__version__ = "0.1.0"

# Check for required packages
def check_requirements():
    """Check if required packages are available."""
    try:
        import psutil
        return True
    except ImportError:
        print("Warning: psutil not available. Install with: pip install psutil")
        return False

# Auto-initialize if requirements are met
if check_requirements():
    initialize_debug()
else:
    print("Debug module requires psutil package for memory tracking.")

# Print module information
print(f"NLM Debug Module v{__version__}")
print("Features:")
print("  - Performance metrics tracking")
print("  - Memory allocation tracking")
print("  - Spike activity profiling")
print("  - STDP analysis")
print("  - Configuration profiling")
print("  - Report generation")
print()
