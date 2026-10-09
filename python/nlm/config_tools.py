"""
Advanced configuration management for NLM Python bindings.
"""

from typing import Dict, Any, Optional, List
from dataclasses import dataclass, field
from enum import Enum
import json
import yaml
import os

class ConfigFormat(Enum):
    JSON = "json"
    YAML = "yaml"
    INI = "ini"
    TOML = "toml"

@dataclass
class ExperimentConfig:
    """Configuration for scientific experiments."""
    name: str
    description: str
    parameters: Dict[str, Any] = field(default_factory=dict)
    results_file: str = ""
    metadata: Dict[str, Any] = field(default_factory=dict)
    
    def to_dict(self) -> Dict[str, Any]:
        return {
            "name": self.name,
            "description": self.description,
            "parameters": self.parameters,
            "results_file": self.results_file,
            "metadata": self.metadata
        }

@dataclass
class BenchmarkConfig:
    """Configuration for performance benchmarking."""
    name: str
    description: str
    num_iterations: int = 5
    warmup_iterations: int = 1
    output_file: str = ""
    metrics: List[str] = field(default_factory=lambda: ["time", "memory", "cpu"])
    parameters: Dict[str, Any] = field(default_factory=dict)

class ValidationTools:
    """Validation tools for NLM configurations and simulations."""
    
    @staticmethod
    def validate_config(config: Dict[str, Any]) -> List[str]:
        """Validate a configuration dictionary."""
        errors = []
        
        # Check required fields
        required_fields = ["neuron_count", "region_count"]
        for field in required_fields:
            if field not in config:
                errors.append(f"Missing required field: {field}")
        
        # Validate parameter ranges
        if "neuron_count" in config and config["neuron_count"] <= 0:
            errors.append("neuron_count must be positive")
            
        if "connection_probability" in config and not (0.0 <= config["connection_probability"] <= 1.0):
            errors.append("connection_probability must be between 0.0 and 1.0")
            
        return errors
    
    @staticmethod
    def validate_simulation(brain, max_steps: int = 10000) -> Dict[str, Any]:
        """Validate simulation stability and performance."""
        validation_result = {
            "stable": True,
            "warnings": [],
            "errors": [],
            "performance_metrics": {}
        }
        
        # Check for excessive spiking
        if brain.getTotalSpikeCount() > max_steps * 100:  # More than 100 spikes per step
            validation_result["warnings"].append(
                f"High spike rate: {brain.getTotalSpikeCount()} spikes"
            )
        
        # Check for memory issues
        if brain.getTotalNeuronCount() > 10000:
            validation_result["warnings"].append(
                f"Large network: {brain.getTotalNeuronCount()} neurons"
            )
        
        return validation_result

class PerformanceProfiler:
    """Performance profiling and optimization tools."""
    
    def __init__(self):
        self.timings = {}
        self.memory_usage = []
        self.cpu_usage = []
    
    def profile_brain_initialization(self, brain):
        """Profile brain initialization performance."""
        import time
        
        start_time = time.time()
        brain.initialize()
        end_time = time.time()
        
        self.timings["initialization"] = end_time - start_time
        return self.timings["initialization"]
    
    def profile_simulation(self, brain, steps: int):
        """Profile simulation performance."""
        import time
        import psutil
        import os
        
        process = psutil.Process(os.getpid())
        
        start_time = time.time()
        start_memory = process.memory_info().rss / 1024 / 1024  # MB
        start_cpu = process.cpu_percent()
        
        for step in range(steps):
            brain.step(step, step * 0.001)
        
        end_time = time.time()
        end_memory = process.memory_info().rss / 1024 / 1024
        end_cpu = process.cpu_percent()
        
        self.timings["simulation"] = end_time - start_time
        self.memory_usage.append((end_memory - start_memory))
        self.cpu_usage.append((end_cpu - start_cpu))
        
        return {
            "time": self.timings["simulation"],
            "steps_per_second": steps / self.timings["simulation"],
            "memory_delta_mb": end_memory - start_memory,
            "cpu_delta_percent": end_cpu - start_cpu
        }
    
    def generate_report(self) -> str:
        """Generate a performance report."""
        report = "Performance Profile Report\n"
        report += "=" * 50 + "\n"
        
        for operation, timing in self.timings.items():
            report += f"{operation}: {timing:.4f}s\n"
        
        if self.memory_usage:
            avg_memory = sum(self.memory_usage) / len(self.memory_usage)
            report += f"Average memory delta: {avg_memory:.2f} MB\n"
        
        if self.cpu_usage:
            avg_cpu = sum(self.cpu_usage) / len(self.cpu_usage)
            report += f"Average CPU delta: {avg_cpu:.2f}%\n"
        
        return report