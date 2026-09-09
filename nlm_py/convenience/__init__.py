"""
NLM Python Convenience Functions

This module contains convenience functions and utilities for the NLM Python bindings.
"""

import json
import logging
from typing import Dict, List, Any, Callable, Optional, Union, Tuple
from enum import Enum

import numpy as np

# Configure logging
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

class SimulationPreset:
    """Preset simulation configurations."""
    
    @staticmethod
    def create_simple_brain(size: int = 100, density: float = 0.1) -> Dict[str, Any]:
        """Create a simple brain configuration."""
        return {
            'brain_size': size,
            'region_count': 1,
            'population_per_region': size // 2,
            'connection_probability': density,
            'mean_weight': 0.5,
            'weight_variance': 0.1,
            'development_stage': 'Initial',
            'enable_plasticity': True,
            'enable_neuromodulation': True,
            'enable_curiosity': True
        }
    
    @staticmethod
    def create_complex_brain(size: int = 1000, density: float = 0.05) -> Dict[str, Any]:
        """Create a complex brain configuration."""
        return {
            'brain_size': size,
            'region_count': 3,
            'population_per_region': size // 3,
            'connection_probability': density,
            'mean_weight': 0.3,
            'weight_variance': 0.15,
            'development_stage': 'CriticalPeriod',
            'enable_plasticity': True,
            'enable_neuromodulation': True,
            'enable_curiosity': True,
            'enable_structural_plasticity': True,
            'enable_prediction': True
        }
    
    @staticmethod
    def create_agent_brain(config: Dict[str, Any]) -> Dict[str, Any]:
        """Create an agent brain configuration from brain config."""
        return {
            'agent_mode': 'active',
            'sensory_input_size': 24,  # 8x8x3 for vision
            'motor_output_size': 10,
            'learning_rate': 0.01,
            'reward_discount': 0.9,
            'enable_reward_modulation': True,
            'enable_curiosity': True,
            'novelty_threshold': 0.5,
            'prediction_error_weight': 0.5
        }
class MetricsCollector:
    """Collect and format metrics during simulation."""
    
    def __init__(self):
        self.metrics = {}
        self.current_step = 0
    
    def add_metric(self, name: str, value: Any):
        """Add a metric value."""
        if name not in self.metrics:
            self.metrics[name] = []
        self.metrics[name].append(value)
    
    def get_average(self, name: str) -> float:
        """Get average value of a metric."""
        if name not in self.metrics:
            return 0.0
        values = self.metrics[name]
        return sum(values) / len(values) if values else 0.0
    
    def get_latest(self, name: str) -> Any:
        """Get the latest value of a metric."""
        if name not in self.metrics:
            return None
        values = self.metrics[name]
        return values[-1] if values else None
    
    def get_history(self, name: str) -> List[Any]:
        """Get history of a metric."""
        return self.metrics.get(name, [])
    
    def clear(self):
        """Clear all metrics."""
        self.metrics.clear()
    
    def format_report(self) -> str:
        """Format a report of collected metrics."""
        report = "Simulation Metrics Report:\n"
        for name, values in self.metrics.items():
            if values:
                avg = self.get_average(name)
                latest = self.get_latest(name)
                report += f"  {name}: avg={avg:.4f}, latest={latest}\n"
        return report
class StateLogger:
    """Log simulation state for analysis."""
    
    def __init__(self, filepath: str):
        self.filepath = filepath
        self.data = []
    
    def log_step(self, step: int, **kwargs):
        """Log a simulation step."""
        log_entry = {
            'step': step,
            'timestamp': step * 0.1,  # Assuming 0.1s timestep
            **kwargs
        }
        self.data.append(log_entry)
    
    def save(self):
        """Save logged data to file."""
        with open(self.filepath, 'w') as f:
            json.dump(self.data, f, indent=2)
    
    def load(self) -> List[Dict[str, Any]]:
        """Load logged data from file."""
        with open(self.filepath, 'r') as f:
            self.data = json.load(f)
        return self.data
    
    def get_range(self, start_step: int, end_step: int) -> List[Dict[str, Any]]:
        """Get data for a range of steps."""
        return [entry for entry in self.data 
                if start_step <= entry['step'] <= end_step]
class Debugger:
    """Debug NLM simulations."""
    
    def __init__(self):
        self.breakpoints = []
        self.watches = []
        self.variables = {}
    
    def add_breakpoint(self, condition: Callable[[int], bool]):
        """Add a breakpoint condition."""
        self.breakpoints.append(condition)
    
    def add_watch(self, name: Callable[[], Any]):
        """Add a watch variable."""
        self.watches.append(name)
    
    def evaluate_watches(self) -> Dict[str, Any]:
        """Evaluate all watch variables."""
        results = {}
        for watch in self.watches:
            try:
                results[watch.__name__] = watch()
            except Exception as e:
                results[watch.__name__] = f"Error: {e}"
        return results
    
    def check_breakpoints(self, step: int) -> bool:
        """Check if any breakpoints are triggered."""
        for bp in self.breakpoints:
            if bp(step):
                return True
        return False
def configure_logging(level: str = "INFO", format_string: Optional[str] = None):
    """Configure logging for NLM."""
    if format_string is None:
        format_string = '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    
    logging.basicConfig(
        level=getattr(logging, level.upper()),
        format=format_string
    )
class CallbackManager:
    """Manage callbacks for simulation events."""
    
    def __init__(self):
        self.callbacks = {
            'on_step': [],
            'on_brain_reset': [],
            'on_action_taken': [],
            'on_reward_received': [],
            'on_development_update': []
        }
    
    def add_callback(self, event: str, callback: Callable):
        """Add a callback for an event."""
        if event in self.callbacks:
            self.callbacks[event].append(callback)
        else:
            raise ValueError(f"Unknown event: {event}")
    
    def remove_callback(self, event: str, callback: Callable):
        """Remove a callback for an event."""
        if event in self.callbacks:
            if callback in self.callbacks[event]:
                self.callbacks[event].remove(callback)
    
    def trigger_callbacks(self, event: str, **kwargs):
        """Trigger all callbacks for an event."""
        if event in self.callbacks:
            for callback in self.callbacks[event]:
                try:
                    callback(**kwargs)
                except Exception as e:
                    logger.error(f"Callback error in {event}: {e}")
class BrainProfiler:
    """Profile brain performance."""
    
    def __init__(self):
        self.profiles = {}
        self.start_time = 0.0
    
    def start_profile(self, name: str):
        """Start profiling a function or operation."""
        import time
        self.profiles[name] = {"start": time.time(), "samples": []}
        self.start_time = time.time()
    
    def end_profile(self, name: str) -> float:
        """End profiling and return duration."""
        import time
        if name in self.profiles:
            duration = time.time() - self.profiles[name]["start"]
            self.profiles[name]["total_time"] = duration
            return duration
        return 0.0
    
    def add_sample(self, name: str, value: float):
        """Add a sample to a profile."""
        if name in self.profiles:
            self.profiles[name]["samples"].append(value)
    
    def get_profile_stats(self, name: str) -> Dict[str, float]:
        """Get statistics for a profile."""
        if name not in self.profiles:
            return {}
        
        profile = self.profiles[name]
        if "samples" in profile:
            samples = profile["samples"]
            if samples:
                return {
                    "mean": sum(samples) / len(samples),
                    "min": min(samples),
                    "max": max(samples),
                    "std": (sum((x - sum(samples) / len(samples)) ** 2 for x in samples) / len(samples)) ** 0.5
                }
        return {"total_time": profile.get("total_time", 0.0)}
    
    def get_overall_stats(self) -> Dict[str, float]:
        """Get overall profiling statistics."""
        import time
        total_time = time.time() - self.start_time
        stats = {}
        for name, profile in self.profiles.items():
            duration = profile.get("total_time", 0.0)
            stats[name] = duration / total_time * 100 if total_time > 0 else 0.0
        return stats
class ResourceManager:
    """Manage system resources for NLM simulations."""
    
    def __init__(self):
        self.memory_usage = []
        self.cpu_usage = []
        self.max_memory = 1024 * 1024 * 1024  # 1GB default
        self.max_cpu = 100.0  # 100% default
    
    def monitor_resources(self):
        """Monitor current resource usage."""
        import psutil
        process = psutil.Process()
        
        # Memory usage
        mem_info = process.memory_info()
        memory_mb = mem_info.rss / (1024 * 1024)
        self.memory_usage.append(memory_mb)
        
        # CPU usage
        cpu_percent = process.cpu_percent()
        self.cpu_usage.append(cpu_percent)
    
    def get_memory_stats(self) -> Dict[str, float]:
        """Get memory usage statistics."""
        if not self.memory_usage:
            return {"current": 0, "max": self.max_memory, "avg": 0}
        
        current = self.memory_usage[-1] * 1024 * 1024  # Convert to bytes
        avg = sum(self.memory_usage) * 1024 * 1024 / len(self.memory_usage)
        max_mb = max(self.memory_usage)
        
        return {
            "current": current,
            "max": self.max_memory,
            "avg": avg,
            "current_mb": self.memory_usage[-1],
            "max_mb": max_mb,
            "avg_mb": sum(self.memory_usage) / len(self.memory_usage)
        }
    
    def get_cpu_stats(self) -> Dict[str, float]:
        """Get CPU usage statistics."""
        if not self.cpu_usage:
            return {"current": 0, "max": self.max_cpu, "avg": 0}
        
        current = self.cpu_usage[-1]
        avg = sum(self.cpu_usage) / len(self.cpu_usage)
        max_cpu = max(self.cpu_usage)
        
        return {
            "current": current,
            "max": self.max_cpu,
            "avg": avg,
            "max_percent": max_cpu
        }
    
    def is_resource_available(self, memory_needed_mb: float = 0, cpu_needed_percent: float = 0) -> bool:
        """Check if enough resources are available."""
        mem_stats = self.get_memory_stats()
        cpu_stats = self.get_cpu_stats()
        
        memory_ok = (memory_needed_mb <= 0) or (mem_stats["current_mb"] + memory_needed_mb <= 100)
        cpu_ok = (cpu_needed_percent <= 0) or (cpu_stats["current"] + cpu_needed_percent <= cpu_stats["max_percent"])
        
        return memory_ok and cpu_ok
    
    def wait_for_resources(self, memory_needed_mb: float = 0, cpu_needed_percent: float = 0, timeout: float = 30.0):
        """Wait for enough resources to be available."""
        import time
        start_time = time.time()
        while not self.is_resource_available(memory_needed_mb, cpu_needed_percent):
            if time.time() - start_time > timeout:
                raise TimeoutError(f"Timeout waiting for resources (needed: {memory_needed_mb}MB, {cpu_needed_percent}%)")
            self.monitor_resources()
            time.sleep(0.1)
def format_spike_statistics(brain) -> str:
    """Format spike statistics for display."""
    return (f"Spikes: {brain.get_total_spike_count()}, "
            f"Active: {brain.get_active_neuron_count()}, "
            f"Firing: {brain.get_firing_neuron_count()}, "
            f"Rate: {brain.get_average_firing_rate():.2f} Hz")


def format_memory_stats(brain) -> str:
    """Format memory statistics for display."""
    # This would need actual memory system access
    return "Memory statistics not available in Python fallback"


def format_prediction_stats(brain) -> str:
    """Format prediction statistics for display."""
    # This would need actual prediction system access
    return "Prediction statistics not available in Python fallback"


def run_simulation(brain, world, agent, num_steps: int):
    """Run a simulation loop."""
    for step in range(num_steps):
        # Update world
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.get_sensory_percept()
        
        # Tell the brain
        agent.process_sensory_input(percept)
        
        # Brain thinks
        brain.step(step)
        
        # Get action from brain
        action = agent.decode_motor_command()
        
        # Do action in world
        world.apply_motor_command(action, world.get_simulation_time())


def save_brain_state(brain, filepath: str) -> bool:
    """Save brain state to file."""
    return brain.save(filepath)


def load_brain_state(brain, filepath: str) -> bool:
    """Load brain state from file."""
    return brain.load(filepath)


def create_simple_brain(size: int = 100) -> Any:
    """Create a simple brain for testing."""
    config = create_default_config()
    brain = create_brain(config)
    brain.initialize()
    return brain


def create_test_world(width: int = 20, height: int = 20) -> Any:
    """Create a test world for testing."""
    world = create_simple_world()
    world.configure(width, height, 8, 8)
    world.reset()
    return world


def create_test_agent(brain) -> Any:
    """Create a test agent for testing."""
    return create_agent_brain(brain)


# Global instances
_callback_manager = CallbackManager()
_profiler = BrainProfiler()
_resource_manager = ResourceManager()

def get_callback_manager() -> CallbackManager:
    """Get the global callback manager."""
    return _callback_manager


def get_profiler() -> BrainProfiler:
    """Get the global profiler."""
    return _profiler


def get_resource_manager() -> ResourceManager:
    """Get the global resource manager."""
    return _resource_manager


# Export public interface
__all__ = [
    # Classes
    'SimulationPreset',
    'MetricsCollector',
    'StateLogger',
    'Debugger',
    'CallbackManager',
    'BrainProfiler',
    'ResourceManager',
    
    # Functions
    'configure_logging',
    'format_spike_statistics',
    'format_memory_stats',
    'format_prediction_stats',
    'run_simulation',
    'save_brain_state',
    'load_brain_state',
    'create_simple_brain',
    'create_test_world',
    'create_test_agent',
    
    # Global managers
    'get_callback_manager',
    'get_profiler',
    'get_resource_manager',
]
