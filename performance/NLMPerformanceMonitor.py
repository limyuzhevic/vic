#!/usr/bin/env python3
"""
NLM Performance Optimization Tools

This module provides performance monitoring and optimization tools for NLM simulations.
It includes memory tracking, timing analysis, and performance profiling capabilities.
"""

import time
import tracemalloc
import psutil
import sys
from typing import Dict, List, Optional, Callable
from dataclasses import dataclass, field
from contextlib import contextmanager
import threading
import json
from pathlib import Path

@dataclass
class PerformanceMetrics:
    """Performance metrics for NLM simulation."""
    steps_completed: int = 0
    total_time: float = 0.0
    average_step_time: float = 0.0
    steps_per_second: float = 0.0
    peak_memory_mb: float = 0.0
    average_memory_mb: float = 0.0
    max_firing_rate: float = 0.0
    min_firing_rate: float = 0.0
    average_firing_rate: float = 0.0
    neuron_count: int = 0
    synapse_count: int = 0
    start_time: float = 0.0
    end_time: float = 0.0
    memory_samples: List[float] = field(default_factory=list)
    step_time_samples: List[float] = field(default_factory=list)
    firing_rate_samples: List[float] = field(default_factory=list)

@dataclass
class BenchmarkResult:
    """Benchmark test result."""
    test_name: str
    metrics: PerformanceMetrics
    configuration: Dict
    timestamp: float
    success: bool
    error_message: Optional[str] = None

class NLMPerformanceMonitor:
    """Monitor NLM simulation performance and resource usage."""
    
    def __init__(self, brain=None):
        self.brain = brain
        self.metrics = PerformanceMetrics()
        self.process = psutil.Process()
        self.monitoring = False
        self.monitor_thread = None
        
    def start_monitoring(self):
        """Start performance monitoring."""
        tracemalloc.start()
        self.metrics.start_time = time.time()
        self.monitoring = True
        
        # Start background thread for continuous monitoring
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def stop_monitoring(self):
        """Stop performance monitoring."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
        
        self.metrics.end_time = time.time()
        self._finalize_metrics()
        
    def _monitor_loop(self):
        """Background monitoring loop."""
        while self.monitoring:
            # Record memory usage
            memory_mb = self.process.memory_info().rss / 1024 / 1024
            self.metrics.memory_samples.append(memory_mb)
            
            # Record CPU usage
            cpu_percent = self.process.cpu_percent()
            
            # Record firing rate if brain is available
            if self.brain:
                firing_rate = self.brain.getAverageFiringRate()
                self.metrics.firing_rate_samples.append(firing_rate)
                
                # Update max/min firing rates
                if firing_rate > self.metrics.max_firing_rate:
                    self.metrics.max_firing_rate = firing_rate
                if self.metrics.min_firing_rate == 0 or firing_rate < self.metrics.min_firing_rate:
                    self.metrics.min_firing_rate = firing_rate
            
            time.sleep(0.1)  # Monitor every 100ms
    
    def _finalize_metrics(self):
        """Calculate final metrics from samples."""
        if self.metrics.memory_samples:
            self.metrics.peak_memory_mb = max(self.metrics.memory_samples)
            self.metrics.average_memory_mb = sum(self.metrics.memory_samples) / len(self.metrics.memory_samples)
        
        if self.metrics.firing_rate_samples:
            self.metrics.average_firing_rate = sum(self.metrics.firing_rate_samples) / len(self.metrics.firing_rate_samples)
        
        if self.metrics.step_time_samples:
            self.metrics.average_step_time = sum(self.metrics.step_time_samples) / len(self.metrics.step_time_samples)
        
        if self.metrics.total_time > 0:
            self.metrics.steps_per_second = self.metrics.steps_completed / self.metrics.total_time
    
    def record_step(self, step_time: float, firing_rate: float):
        """Record metrics for a single simulation step."""
        self.metrics.step_time_samples.append(step_time)
        if firing_rate > 0:
            self.metrics.firing_rate_samples.append(firing_rate)
        self.metrics.steps_completed += 1
    
    def get_metrics(self) -> PerformanceMetrics:
        """Get current performance metrics."""
        return self.metrics
    
    def export_metrics(self, filename: str):
        """Export metrics to JSON file."""
        data = {
            'metrics': self.metrics.__dict__,
            'timestamp': time.time()
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
    def print_summary(self):
        """Print performance summary to console."""
        if self.metrics.start_time == 0:
            print("Performance monitoring not started")
            return
        
        print("=== NLM Performance Summary ===")
        print(f"Total Steps: {self.metrics.steps_completed}")
        print(f"Total Time: {self.metrics.total_time:.2f} seconds")
        print(f"Steps per Second: {self.metrics.steps_per_second:.2f}")
        print(f"Average Step Time: {self.metrics.average_step_time*1000:.2f} ms")
        print(f"Peak Memory Usage: {self.metrics.peak_memory_mb:.1f} MB")
        print(f"Average Memory Usage: {self.metrics.average_memory_mb:.1f} MB")
        print(f"Firing Rate: {self.metrics.average_firing_rate:.2f} ± {self.calculate_std_dev(self.metrics.firing_rate_samples):.2f}")
        print(f"Min/Max Firing Rate: {self.metrics.min_firing_rate:.2f} / {self.metrics.max_firing_rate:.2f}")
        print("==========================")
    
    def calculate_std_dev(self, samples: List[float]) -> float:
        """Calculate standard deviation of samples."""
        if len(samples) < 2:
            return 0.0
        
        mean = sum(samples) / len(samples)
        variance = sum((x - mean) ** 2 for x in samples) / (len(samples) - 1)
        return (variance ** 0.5)

class NLMBenchmarkSuite:
    """Comprehensive benchmark suite for NLM performance testing."""
    
    def __init__(self):
        self.results: List[BenchmarkResult] = []
        self.monitor = None
        
    def create_test_config(self, neuron_count: int = 1000, region_count: int = 2,
                          connection_prob: float = 0.1, timestep: float = 0.001,
                          enable_plasticity: bool = True) -> dict:
        """Create a standardized test configuration."""
        return {
            'neuron_count': neuron_count,
            'region_count': region_count,
            'connection_probability': connection_prob,
            'simulation_timestep': timestep,
            'plasticity_learning_rate': 0.01,
            'stdp_ltp_weight': 0.01,
            'stdp_ltd_weight': 0.012,
            'enable_plasticity': enable_plasticity,
            'enable_curiosity': True,
            'enable_reward_modulation': True,
            'enable_development': True
        }
    
    def benchmark_configuration(self, config: dict, steps: int = 1000, 
                               brain_factory: Callable = None) -> BenchmarkResult:
        """Benchmark a specific configuration."""
        import pynlm
        
        print(f"Benchmarking configuration: {config}")
        
        # Create brain with configuration
        brain = brain_factory(config) if brain_factory else self._create_default_brain(config)
        
        # Create monitor
        self.monitor = NLMPerformanceMonitor(brain)
        
        # Start monitoring
        self.monitor.start_monitoring()
        
        # Run simulation
        try:
            for step in range(steps):
                step_start = time.time()
                
                # Run one simulation step
                brain.step(step)
                
                step_end = time.time()
                step_time = step_end - step_start
                firing_rate = brain.getAverageFiringRate()
                
                # Record metrics
                self.monitor.record_step(step_time, firing_rate)
                
                # Log progress
                if step % 100 == 0:
                    print(f"  Step {step}/{steps}")
            
            # Stop monitoring
            self.monitor.stop_monitoring()
            
            # Create result
            result = BenchmarkResult(
                test_name=f"neuron_{config['neuron_count']}_steps_{steps}",
                metrics=self.monitor.get_metrics(),
                configuration=config,
                timestamp=time.time(),
                success=True
            )
            
        except Exception as e:
            # Handle errors
            if self.monitor:
                self.monitor.stop_monitoring()
            
            result = BenchmarkResult(
                test_name=f"neuron_{config['neuron_count']}_steps_{steps}",
                metrics=self.monitor.get_metrics() if self.monitor else PerformanceMetrics(),
                configuration=config,
                timestamp=time.time(),
                success=False,
                error_message=str(e)
            )
        
        # Store result
        self.results.append(result)
        
        # Print result summary
        self._print_benchmark_result(result)
        
        return result
    
    def _create_default_brain(self, config: dict):
        """Create default NLM brain with configuration."""
        import pynlm
        
        # Create configuration object
        pyconfig = pynlm.createDefaultConfig()
        
        # Set configuration values
        pyconfig.set("brain.neuron_count", config['neuron_count'])
        pyconfig.set("brain.region_count", config['region_count'])
        pyconfig.set("brain.connection_probability", config['connection_probability'])
        pyconfig.set("simulation_timestep", config['simulation_timestep'])
        
        if config.get('enable_plasticity', True):
            pyconfig.set("plasticity.stdp.enable", True)
            pyconfig.set("plasticity.hebbian.enable", True)
        
        # Create brain
        brain = pynlm.createBrain(pyconfig)
        brain.initialize()
        
        return brain
    
    def _print_benchmark_result(self, result: BenchmarkResult):
        """Print benchmark result summary."""
        print(f"\n=== Benchmark Result ===")
        print(f"Test: {result.test_name}")
        print(f"Status: {'PASSED' if result.success else 'FAILED'}")
        print(f"Steps: {result.metrics.steps_completed}")
        print(f"Total Time: {result.metrics.total_time:.2f}s")
        print(f"Speed: {result.metrics.steps_per_second:.2f} steps/s")
        print(f"Memory: {result.metrics.peak_memory_mb:.1f} MB peak")
        print(f"Firing Rate: {result.metrics.average_firing_rate:.2f}")
        
        if result.error_message:
            print(f"Error: {result.error_message}")
    
    def run_benchmark_suite(self, brain_factory: Callable = None):
        """Run complete benchmark suite with different configurations."""
        print("=== NLM Performance Benchmark Suite ===")
        
        # Test configurations
        test_configs = [
            self.create_test_config(100, 1, 0.05, 0.001, True),   # Small, simple
            self.create_test_config(1000, 2, 0.1, 0.001, True),  # Medium, standard
            self.create_test_config(5000, 5, 0.15, 0.001, True),  # Large, complex
            self.create_test_config(1000, 2, 0.05, 0.002, False),  # Same size, slow timestep
            self.create_test_config(1000, 2, 0.1, 0.001, False),   # Same size, no plasticity
        ]
        
        # Run benchmarks
        for i, config in enumerate(test_configs):
            print(f"\n--- Test {i+1}/{len(test_configs)} ---")
            steps = 500 if config['neuron_count'] <= 1000 else 100
            
            result = self.benchmark_configuration(config, steps, brain_factory)
            
            # Export result
            filename = f"benchmark_{result.test_name}.json"
            self.monitor.export_metrics(filename)
            print(f"Results exported to: {filename}")
    
    def compare_results(self, config_names: List[str] = None) -> Dict:
        """Compare results from different configurations."""
        if not config_names:
            config_names = [r.test_name for r in self.results]
        
        comparison = {}
        
        for name in config_names:
            for result in self.results:
                if result.test_name == name:
                    comparison[name] = {
                        'speed': result.metrics.steps_per_second,
                        'memory': result.metrics.peak_memory_mb,
                        'firing_rate': result.metrics.average_firing_rate,
                        'success': result.success
                    }
                    break
        
        return comparison
    
    def generate_report(self, filename: str = "benchmark_report.html"):
        """Generate HTML report of benchmark results."""
        try:
            html_content = self._create_html_report()
            with open(filename, 'w') as f:
                f.write(html_content)
            print(f"Report generated: {filename}")
        except Exception as e:
            print(f"Failed to generate report: {e}")
    
    def _create_html_report(self) -> str:
        """Create HTML report content."""
        html = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Performance Benchmark Report</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .test {{ border: 1px solid #ddd; margin: 10px 0; padding: 15px; }}
                .passed {{ background-color: #d4edda; }}
                .failed {{ background-color: #f8d7da; }}
                .metrics {{ display: flex; flex-wrap: wrap; }}
                .metric {{ margin: 5px; padding: 10px; background-color: #f8f9fa; border-radius: 3px; }}
                .metric h3 {{ margin: 0 0 10px 0; }}
                .metric p {{ margin: 5px 0; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Performance Benchmark Report</h1>
                <p>Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
                <p>Total Tests: {len(self.results)}</p>
                <p>Passed: {sum(1 for r in self.results if r.success)}</p>
                <p>Failed: {sum(1 for r in self.results if not r.success)}</p>
            </div>
        """
        
        for result in self.results:
            status_class = 'passed' if result.success else 'failed'
            status_text = 'PASSED' if result.success else 'FAILED'
            
            html += f"""
            <div class="test {status_class}">
                <h2>{result.test_name}</h2>
                <p><strong>Status:</strong> {status_text}</p>
                <p><strong>Configuration:</strong> {json.dumps(result.configuration, indent=2)}</p>
                <div class="metrics">
                    <div class="metric">
                        <h3>Performance</h3>
                        <p><strong>Speed:</strong> {result.metrics.steps_per_second:.2f} steps/s</p>
                        <p><strong>Average Step Time:</strong> {result.metrics.average_step_time*1000:.2f} ms</p>
                        <p><strong>Total Time:</strong> {result.metrics.total_time:.2f}s</p>
                    </div>
                    <div class="metric">
                        <h3>Memory</h3>
                        <p><strong>Peak Usage:</strong> {result.metrics.peak_memory_mb:.1f} MB</p>
                        <p><strong>Average Usage:</strong> {result.metrics.average_memory_mb:.1f} MB</p>
                    </div>
                    <div class="metric">
                        <h3>Neural Activity</h3>
                        <p><strong>Firing Rate:</strong> {result.metrics.average_firing_rate:.2f}</p>
                        <p><strong>Min/Max:</strong> {result.metrics.min_firing_rate:.2f} / {result.metrics.max_firing_rate:.2f}</p>
                    </div>
                </div>
            </div>
            """
        
        html += """
        </body>
        </html>
        """
        
        return html

def profile_simulation(config: dict, steps: int = 1000, brain_factory: Callable = None) -> Dict:
    """
    Profile a single NLM simulation and return performance data.
    
    Args:
        config: Simulation configuration
        steps: Number of simulation steps
        brain_factory: Function to create brain from config
        
    Returns:
        Dictionary containing performance metrics and profiling data
    """
    import pynlm
    
    benchmark = NLMBenchmarkSuite()
    
    # Create brain
    brain = brain_factory(config) if brain_factory else benchmark._create_default_brain(config)
    
    # Create monitor
    monitor = NLMPerformanceMonitor(brain)
    
    # Run simulation with profiling
    monitor.start_monitoring()
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        # Record metrics
        firing_rate = brain.getAverageFiringRate()
        monitor.record_step(0.001, firing_rate)  # Use fixed timestep for profiling
    
    monitor.stop_monitoring()
    
    end_time = time.time()
    
    # Get results
    metrics = monitor.get_metrics()
    
    # Clean up
    del brain
    
    return {
        'metrics': metrics.__dict__,
        'config': config,
        'steps': steps,
        'total_time': end_time - start_time,
        'success': True
    }

if __name__ == "__main__":
    # Simple performance test
    print("=== NLM Performance Test ===")
    
    # Create test configuration
    config = {
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.1,
        'simulation_timestep': 0.001,
        'enable_plasticity': True
    }
    
    # Run profile
    result = profile_simulation(config, steps=500)
    
    # Print results
    metrics = result['metrics']
    print(f"\nPerformance Results:")
    print(f"  Steps completed: {metrics['steps_completed']}")
    print(f"  Total time: {result['total_time']:.2f}s")
    print(f"  Steps per second: {metrics['steps_per_second']:.2f}")
    print(f"  Peak memory: {metrics['peak_memory_mb']:.1f} MB")
    print(f"  Average firing rate: {metrics['average_firing_rate']:.2f}")
    
    print("\n=== Performance test completed ===")
        """
        
        return html

if __name__ == "__main__":
    print("=== NLM Performance Test ===")
    
    # Create test configuration
    config = {
        'neuron_count': 1000,
        'region_count': 2,
        'connection_probability': 0.1,
        'simulation_timestep': 0.001,
        'enable_plasticity': True
    }
    
    # Run profile
    result = profile_simulation(config, steps=500)
    
    # Print results
    metrics = result['metrics']
    print(f"\nPerformance Results:")
    print(f"  Steps completed: {metrics['steps_completed']}")
    print(f"  Total time: {result['total_time']:.2f}s")
    print(f"  Steps per second: {metrics['steps_per_second']:.2f}")
    print(f"  Peak memory: {metrics['peak_memory_mb']:.1f} MB")
    print(f"  Average firing rate: {metrics['average_firing_rate']:.2f}")
    
    print("\n=== Performance test completed ===")

if __name__ == "__main__":
    print("Error: This script should not be run directly. Import the classes for use.")
    sys.exit(1)
