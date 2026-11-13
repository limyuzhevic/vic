# python/pynlm/utilities.py
"""
Utility functions and helpers for NLM Python bindings
"""

import numpy as np
from typing import Dict, List, Optional, Any, Tuple
import time
import sys

# Import the underlying C++ bindings
try:
    import pynlm
except ImportError:
    pynlm = None

class PerformanceMonitor:
    """Monitor simulation performance metrics."""
    
    def __init__(self):
        self.metrics = {
            'step_times': [],
            'firing_rates': [],
            'spike_counts': [],
            'memory_usage': []
        }
        self.start_time = 0.0
    
    def start_monitoring(self):
        """Start monitoring."""
        self.start_time = time.time()
    
    def record_step(self, firing_rate: float, spike_count: int):
        """Record metrics for a step."""
        step_time = time.time() - self.start_time
        self.metrics['step_times'].append(step_time)
        self.metrics['firing_rates'].append(firing_rate)
        self.metrics['spike_counts'].append(spike_count)
    
    def get_statistics(self) -> Dict[str, Any]:
        """Get performance statistics."""
        if not self.metrics['step_times']:
            return {}
        
        step_times = self.metrics['step_times']
        
        return {
            'total_steps': len(step_times),
            'average_step_time': np.mean(step_times),
            'median_step_time': np.median(step_times),
            'std_step_time': np.std(step_times),
            'min_step_time': np.min(step_times),
            'max_step_time': np.max(step_times),
            'average_firing_rate': np.mean(self.metrics['firing_rates']),
            'average_spike_count': np.mean(self.metrics['spike_counts']),
            'total_spikes': self.metrics['spike_counts'][-1] if self.metrics['spike_counts'] else 0
        }

class BrainAnalyzer:
    """Analyze brain activity and patterns."""
    
    def __init__(self):
        self.spike_history = []
        self.firing_rate_history = []
    
    def record_step(self, brain: Any, step: int):
        """Record brain state for analysis."""
        brain.step(step)
        self.spike_history.append(brain.getTotalSpikeCount())
        self.firing_rate_history.append(brain.getAverageFiringRate())
    
    def get_spike_patterns(self) -> Dict[str, Any]:
        """Get spike pattern analysis."""
        if not self.spike_history:
            return {}
        
        spike_array = np.array(self.spike_history)
        
        return {
            'total_spikes': spike_array[-1],
            'max_spike_rate': np.max(spike_array) / 0.1,  # Convert to Hz
            'min_spike_rate': np.min(spike_array) / 0.1,
            'mean_spike_rate': np.mean(spike_array) / 0.1,
            'std_spike_rate': np.std(spike_array) / 0.1,
            'spike_count_trend': self._calculate_trend(spike_array),
            'coefficient_of_variation': np.std(spike_array) / np.mean(spike_array) if np.mean(spike_array) > 0 else 0
        }
    
    def get_firing_rate_analysis(self) -> Dict[str, Any]:
        """Get firing rate analysis."""
        if not self.firing_rate_history:
            return {}
        
        rates = np.array(self.firing_rate_history)
        
        return {
            'mean_firing_rate': np.mean(rates),
            'median_firing_rate': np.median(rates),
            'std_firing_rate': np.std(rates),
            'min_firing_rate': np.min(rates),
            'max_firing_rate': np.max(rates),
            'firing_rate_trend': self._calculate_trend(rates),
            'stability_index': 1.0 / (1.0 + np.std(rates)) if np.std(rates) > 0 else 1.0
        }
    
    def _calculate_trend(self, data: np.ndarray) -> float:
        """Calculate linear trend coefficient."""
        if len(data) < 2:
            return 0.0
        
        x = np.arange(len(data))
        y = data
        
        # Calculate linear regression slope
        slope = np.cov(x, y)[0, 1] / np.var(x) if np.var(x) > 0 else 0.0
        return slope

class SimulationLogger:
    """Log simulation data for analysis."""
    
    def __init__(self, log_file: str = "simulation_log.txt"):
        self.log_file = log_file
        self.logs = []
    
    def log_step(self, step: int, brain: Any, world: Optional[Any] = None):
        """Log step data."""
        log_entry = {
            'step': step,
            'neuron_count': brain.getTotalNeuronCount(),
            'firing_count': brain.getFiringNeuronCount(),
            'spike_count': brain.getTotalSpikeCount(),
            'firing_rate': brain.getAverageFiringRate()
        }
        
        if world:
            log_entry['world_time'] = world.getSimulationTime()
            log_entry['agent_x'] = world.getAgentBody().x
            log_entry['agent_y'] = world.getAgentBody().y
        
        self.logs.append(log_entry)
    
    def save_to_file(self):
        """Save logs to file."""
        with open(self.log_file, 'w') as f:
            f.write("step,neuron_count,firing_count,spike_count,firing_rate,world_time,agent_x,agent_y\n")
            for entry in self.logs:
                f.write(f"{entry['step']},{entry['neuron_count']},"
                       f"{entry['firing_count']},{entry['spike_count']},"
                       f"{entry['firing_rate']},{entry.get('world_time', 0.0)},"
                       f"{entry.get('agent_x', 0.0)},{entry.get('agent_y', 0.0)}\n")

def benchmark_simulation(brain: Any, num_steps: int, warmup_steps: int = 100) -> Dict[str, Any]:
    """Benchmark simulation performance."""
    start_time = time.time()
    
    # Warmup
    for step in range(warmup_steps):
        brain.step(step)
    
    # Benchmark
    spike_counts = []
    for step in range(num_steps):
        brain.step(warmup_steps + step)
        spike_counts.append(brain.getTotalSpikeCount())
    
    end_time = time.time()
    total_time = end_time - start_time
    
    spike_rates = [count / 0.1 for count in spike_counts]  # Convert to Hz
    
    return {
        'total_time': total_time,
        'steps_per_second': num_steps / total_time,
        'average_spike_rate': np.mean(spike_rates),
        'median_spike_rate': np.median(spike_rates),
        'std_spike_rate': np.std(spike_rates),
        'spike_rate_min': np.min(spike_rates),
        'spike_rate_max': np.max(spike_rates),
        'total_spikes': spike_counts[-1] if spike_counts else 0,
        'warmup_steps': warmup_steps
    }

# Configuration validation
def validate_config(config: Any) -> List[str]:
    """Validate configuration parameters."""
    errors = []
    
    neuron_count = config.getOr('brain.neuron_count', 0)
    if neuron_count <= 0:
        errors.append("Neuron count must be positive")
    elif neuron_count > 10000:
        errors.append("Neuron count too large (max 10000)")
    
    region_count = config.getOr('brain.region_count', 0)
    if region_count <= 0:
        errors.append("Region count must be positive")
    
    conn_prob = config.getOr('brain.connection_probability', 0.0)
    if conn_prob < 0.0 or conn_prob > 1.0:
        errors.append("Connection probability must be between 0.0 and 1.0")
    
    return errors

# Utility functions
def calculate_firing_rate(spike_count: int, time_step: float = 0.1) -> float:
    """Calculate firing rate from spike count."""
    return spike_count / time_step

def calculate_activity_index(firing_rate: float, threshold: float = 10.0) -> float:
    """Calculate activity index based on firing rate."""
    if firing_rate < threshold:
        return 0.0
    return min(1.0, (firing_rate - threshold) / threshold)

# Batch processing utilities
def batch_simulate(configs: List[Any], num_steps: int, 
                  processor: Optional[Callable] = None) -> List[Dict[str, Any]]:
    """Run multiple simulations in batch mode."""
    results = []
    
    for i, config in enumerate(configs):
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(num_steps):
            brain.step(step)
        
        if processor:
            processor(brain, i)
        
        result = {
            'config_index': i,
            'total_spikes': brain.getTotalSpikeCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_stage': brain.getDevelopmentalStage()
        }
        
        results.append(result)
    
    return results
