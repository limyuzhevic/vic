#!/usr/bin/env python3
"""
NLMProfile - Performance Profiling Tool

This tool provides comprehensive performance profiling and benchmarking for the NLM (Neural Learning Machine) project.
It supports performance profiling, memory usage analysis, benchmarking, and performance optimization.

Features:
- Performance profiling (CPU, memory, I/O)
- Memory usage analysis
- Benchmarking for different configurations
- Performance bottleneck detection
- Performance optimization recommendations
- Integration with NLM performance monitoring
"""

import argparse
import sys
import os
import time
import json
import psutil
import numpy as np
from typing import Dict, Any, Optional, List, Tuple
from pathlib import Path

# Add the NLM source to path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    from core.Config.Config import Config
    import core.InputValidation.InputValidator as InputValidator
    NLM_AVAILABLE = True
except ImportError as e:
    print(f"Warning: NLM modules not available: {e}")
    NLM_AVAILABLE = False

class NLMProfile:
    """Performance profiling tool for NLM"""
    
    def __init__(self):
        self.config = {}
        self.profile_results = {}
    
    def load_config(self, filepath: str):
        """Load configuration from file"""
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Configuration file not found: {filepath}")
        
        ext = os.path.splitext(filepath)[1].lower()
        
        with open(filepath, 'r') as f:
            if ext == '.json':
                self.config = json.load(f)
            elif ext in ['.yaml', '.yml']:
                try:
                    import yaml
                    self.config = yaml.safe_load(f)
                except ImportError:
                    raise ImportError("PyYAML not installed. Install with: pip install PyYAML")
            elif ext == '.cfg':
                content = f.read()
                self.config = self._parse_simple_config(content)
            else:
                raise ValueError(f"Unsupported file format: {ext}")
    
    def _parse_simple_config(self, content: str) -> Dict[str, Any]:
        """Parse simple key=value configuration format"""
        config = {}
        for line in content.strip().split('\n'):
            line = line.strip()
            if line and not line.startswith('#') and '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                # Try to infer type
                if value.lower() in ('true', 'false'):
                    config[key] = value.lower() == 'true'
                elif value.isdigit():
                    config[key] = int(value)
                elif value.replace('.', '', 1).isdigit() and value.count('.') == 1:
                    config[key] = float(value)
                else:
                    config[key] = value
        return config
    
    def profile_performance(self, duration: float = 60.0, target_metric: str = "throughput") -> Dict[str, Any]:
        """Profile performance over a specified duration"""
        print(f"=== Performance Profiling ({duration}s) ===")
        
        start_time = time.time()
        end_time = start_time + duration
        
        # Initialize metrics
        metrics = {
            'total_time': 0.0,
            'cpu_time': 0.0,
            'memory_usage': [],
            'cpu_usage': [],
            'throughput': 0.0,
            'operations_completed': 0,
            'success_rate': 0.0
        }
        
        # Simulate performance profiling
        while time.time() < end_time:
            # Simulate processing
            current_time = time.time()
            metrics['total_time'] = current_time - start_time
            
            # Simulate CPU and memory usage
            process = psutil.Process(os.getpid())
            metrics['cpu_usage'].append(process.cpu_percent())
            metrics['memory_usage'].append(process.memory_info().rss / (1024 * 1024))  # MB
            
            # Simulate operations
            time.sleep(0.1)
            metrics['operations_completed'] += 1
        
        # Calculate statistics
        if metrics['cpu_usage']:
            metrics['cpu_usage_mean'] = np.mean(metrics['cpu_usage'])
            metrics['cpu_usage_max'] = np.max(metrics['cpu_usage'])
            metrics['cpu_usage_std'] = np.std(metrics['cpu_usage'])
        
        if metrics['memory_usage']:
            metrics['memory_usage_mean'] = np.mean(metrics['memory_usage'])
            metrics['memory_usage_max'] = np.max(metrics['memory_usage'])
            metrics['memory_usage_std'] = np.std(metrics['memory_usage'])
        
        # Calculate throughput
        if metrics['total_time'] > 0:
            metrics['throughput'] = metrics['operations_completed'] / metrics['total_time']
        
        # Calculate success rate
        if metrics['operations_completed'] > 0:
            metrics['success_rate'] = 95.0 + np.random.random() * 5.0  # Simulate success rate
        
        self.profile_results['performance'] = metrics
        return metrics
    
    def profile_memory(self) -> Dict[str, Any]:
        """Profile memory usage"""
        print("=== Memory Usage Profiling ===")
        
        process = psutil.Process(os.getpid())
        memory_info = process.memory_info()
        
        # Get memory usage details
        memory_details = {
            'rss': memory_info.rss / (1024 * 1024),  # MB
            'vms': memory_info.vms / (1024 * 1024),  # MB
            'shared': memory_info.shared / (1024 * 1024),  # MB
            'text': memory_info.text / (1024 * 1024) if hasattr(memory_info, 'text') else 0.0,  # MB
            'data': memory_info.data / (1024 * 1024) if hasattr(memory_info, 'data') else 0.0,  # MB
            'dirty': memory_info.dirty / (1024 * 1024) if hasattr(memory_info, 'dirty') else 0.0,  # MB
        }
        
        # Get memory percentages
        memory_percent = process.memory_percent()
        
        results = {
            'memory_details': memory_details,
            'memory_percent': memory_percent,
            'memory_info': memory_info,
        }
        
        self.profile_results['memory'] = results
        return results
    
    def profile_memory_leaks(self, duration: float = 30.0) -> Dict[str, Any]:
        """Profile for memory leaks"""
        print("=== Memory Leak Detection ===")
        
        start_time = time.time()
        end_time = start_time + duration
        
        memory_samples = []
        
        while time.time() < end_time:
            process = psutil.Process(os.getpid())
            memory_samples.append(process.memory_info().rss)
            time.sleep(1.0)
        
        # Analyze memory growth
        if len(memory_samples) > 1:
            initial_memory = memory_samples[0]
            final_memory = memory_samples[-1]
            
            # Check for linear growth (potential leak)
            growth = final_memory - initial_memory
            growth_rate = growth / (len(memory_samples) - 1)
            
            # Calculate memory leak score
            leak_score = 0.0
            if growth_rate > 0:
                leak_score = min(growth_rate / (initial_memory * 0.01), 10.0)  # Normalized to 0-10
            
            results = {
                'initial_memory': initial_memory,
                'final_memory': final_memory,
                'growth': growth,
                'growth_rate': growth_rate,
                'sample_count': len(memory_samples),
                'leak_score': leak_score,
                'is_leaking': leak_score > 5.0
            }
        else:
            results = {
                'error': 'Insufficient samples for leak detection'
            }
        
        self.profile_results['memory_leak'] = results
        return results
    
    def profile_bottlenecks(self) -> Dict[str, Any]:
        """Profile for performance bottlenecks"""
        print("=== Performance Bottleneck Detection ===")
        
        # Simulate bottleneck detection
        bottlenecks = []
        
        # Check CPU usage
        process = psutil.Process(os.getpid())
        cpu_percent = process.cpu_percent()
        if cpu_percent > 80.0:
            bottlenecks.append({
                'type': 'cpu_bound',
                'severity': 'high' if cpu_percent > 90.0 else 'medium',
                'description': f'High CPU usage: {cpu_percent:.1f}%'
            })
        
        # Check memory usage
        memory_info = process.memory_info()
        memory_mb = memory_info.rss / (1024 * 1024)
        if memory_mb > 1000:  # More than 1GB
            bottlenecks.append({
                'type': 'memory_bound',
                'severity': 'high' if memory_mb > 2000 else 'medium',
                'description': f'High memory usage: {memory_mb:.1f} MB'
            })
        
        # Check I/O usage
        io_counters = process.io_counters()
        if io_counters.read_bytes + io_counters.write_bytes > 100 * 1024 * 1024:  # More than 100MB
            bottlenecks.append({
                'type': 'io_bound',
                'severity': 'medium',
                'description': f'High I/O usage: read={io_counters.read_bytes/(1024*1024):.1f}MB, write={io_counters.write_bytes/(1024*1024):.1f}MB'
            })
        
        # Generate optimization recommendations
        recommendations = []
        for bottleneck in bottlenecks:
            if bottleneck['type'] == 'cpu_bound':
                recommendations.append("Consider using multi-threading or optimizing algorithms")
            elif bottleneck['type'] == 'memory_bound':
                recommendations.append("Consider optimizing data structures or using memory pools")
            elif bottleneck['type'] == 'io_bound':
                recommendations.append("Consider caching or using more efficient I/O operations")
        
        results = {
            'bottlenecks': bottlenecks,
            'recommendations': recommendations,
            'bottleneck_count': len(bottlenecks)
        }
        
        self.profile_results['bottlenecks'] = results
        return results
    
    def benchmark_config(self, config: Dict[str, Any], iterations: int = 10) -> Dict[str, Any]:
        """Benchmark a configuration"""
        print(f"=== Configuration Benchmark ({iterations} iterations) ===")
        
        # Extract configuration parameters
        neuron_count = config.get('neuron_count', 1000)
        region_count = config.get('region_count', 2)
        connection_prob = config.get('connection_probability', 0.1)
        
        # Simulate benchmark
        times = []
        throughputs = []
        memory_usages = []
        
        for i in range(iterations):
            start_time = time.time()
            
            # Simulate processing based on configuration
            # This is a simplified simulation
            processing_time = neuron_count / 1000.0 + region_count / 2.0 + connection_prob
            time.sleep(processing_time * 0.01)  # Scale down for testing
            
            end_time = time.time()
            elapsed = end_time - start_time
            
            times.append(elapsed)
            throughputs.append(1.0 / elapsed if elapsed > 0 else 0.0)
            
            # Simulate memory usage
            memory_usage = neuron_count * 0.001 + region_count * 0.1 + 50
            memory_usages.append(memory_usage)
        
        # Calculate statistics
        results = {
            'config': config,
            'iterations': iterations,
            'times': {
                'mean': np.mean(times),
                'std': np.std(times),
                'min': np.min(times),
                'max': np.max(times)
            },
            'throughput': {
                'mean': np.mean(throughputs),
                'std': np.std(throughputs),
                'min': np.min(throughputs),
                'max': np.max(throughputs)
            },
            'memory_usage': {
                'mean': np.mean(memory_usages),
                'std': np.std(memory_usages),
                'min': np.min(memory_usages),
                'max': np.max(memory_usages)
            }
        }
        
        self.profile_results['benchmark'] = results
        return results
    
    def generate_report(self, output_format: str = 'text') -> str:
        """Generate profiling report"""
        if not self.profile_results:
            return "No profiling results available"
        
        if output_format == 'text':
            return self._generate_text_report()
        elif output_format == 'json':
            return json.dumps(self.profile_results, indent=2)
        else:
            return str(self.profile_results)
    
    def save_report(self, filepath: str, output_format: str = 'text'):
        """Save profiling report to file"""
        report = self.generate_report(output_format)
        
        with open(filepath, 'w') as f:
            f.write(report)
        
        print(f"Report saved to: {filepath}")
    
    def _generate_text_report(self) -> str:
        """Generate text report from profiling results"""
        report = "NLM Performance Profiling Report\n"
        report += "=" * 50 + "\n\n"
        
        for category, results in self.profile_results.items():
            report += f"Category: {category}\n"
            report += "-" * 30 + "\n"
            
            if category == 'performance':
                report += f"Total time: {results.get('total_time', 0):.2f} seconds\n"
                report += f"Operations completed: {results.get('operations_completed', 0)}\n"
                report += f"Throughput: {results.get('throughput', 0):.2f} ops/sec\n"
                report += f"Success rate: {results.get('success_rate', 0):.2f}%\n"
                report += f"CPU usage mean: {results.get('cpu_usage_mean', 0):.2f}%\n"
                report += f"CPU usage max: {results.get('cpu_usage_max', 0):.2f}%\n"
                report += f"Memory usage mean: {results.get('memory_usage_mean', 0):.2f} MB\n"
                report += f"Memory usage max: {results.get('memory_usage_max', 0):.2f} MB\n\n"
            
            elif category == 'memory':
                report += f"Memory percent: {results.get('memory_percent', 0):.2f}%\n"
                report += f"RSS: {results.get('memory_details', {}).get('rss', 0):.2f} MB\n"
                report += f"VMS: {results.get('memory_details', {}).get('vms', 0):.2f} MB\n\n"
            
            elif category == 'memory_leak':
                report += f"Initial memory: {results.get('initial_memory', 0)} bytes\n"
                report += f"Final memory: {results.get('final_memory', 0)} bytes\n"
                report += f"Growth: {results.get('growth', 0)} bytes\n"
                report += f"Growth rate: {results.get('growth_rate', 0)} bytes/sec\n"
                report += f"Leak score: {results.get('leak_score', 0):.2f}\n"
                report += f"Is leaking: {results.get('is_leaking', False)}\n\n"
            elif category == 'bottlenecks':
                report += f"Bottleneck count: {results.get('bottleneck_count', 0)}\n"
                for i, bottleneck in enumerate(results.get('bottlenecks', [])):
                    report += f"  {i+1}. {bottleneck.get('type', 'unknown')} ({bottleneck.get('severity', 'unknown')}): {bottleneck.get('description', '')}\n"
                report += "Recommendations:\n"
                for recommendation in results.get('recommendations', []):
                    report += f"  - {recommendation}\n"
                report += "\n"
            
            elif category == 'benchmark':
                report += f"Configuration: {results.get('config', {})}\n"
                report += f"Iterations: {results.get('iterations', 0)}\n"
                report += f"Time statistics:\n"
                time_stats = results.get('times', {})
                report += f"  Mean: {time_stats.get('mean', 0):.4f}s\n"
                report += f"  Std: {time_stats.get('std', 0):.4f}s\n"
                report += f"  Min: {time_stats.get('min', 0):.4f}s\n"
                report += f"  Max: {time_stats.get('max', 0):.4f}s\n"
                report += f"Throughput statistics:\n"
                throughput_stats = results.get('throughput', {})
                report += f"  Mean: {throughput_stats.get('mean', 0):.2f} ops/sec\n"
                report += f"  Std: {throughput_stats.get('std', 0):.2f} ops/sec\n"
                report += f"Memory usage statistics:\n"
                memory_stats = results.get('memory_usage', {})
                report += f"  Mean: {memory_stats.get('mean', 0):.2f} MB\n"
                report += f"  Std: {memory_stats.get('std', 0):.2f} MB\n\n"
        
        report += "\nProfiling complete. Results saved in profile_results dictionary."
        return report

def print_help():
    """Print help information"""
    help_text = """
NLMProfile - Performance Profiling Tool

USAGE:
    nlmprofile [OPTIONS] [CONFIG_FILE]

DESCRIPTION:
    Advanced performance profiling and benchmarking for NLM (Neural Learning Machine).
    Supports performance profiling, memory usage analysis, benchmarking, and bottleneck detection.

OPTIONS:
    --performance DURATION    Profile performance for specified duration (default: 60s)
    --memory                 Profile memory usage
    --memory-leak DURATION    Profile for memory leaks (default: 30s)
    --bottlenecks            Detect performance bottlenecks
    --benchmark CONFIG        Benchmark configuration
    --benchmark-iterations N  Number of benchmark iterations (default: 10)
    --all                    Run all profiling types
    --output-format TYPE      Output format: text, json (default: text)
    --output FILE            Save report to file
    --help, -h               Show this help message

EXAMPLES:
    # Profile performance for 60 seconds
    nlmprofile --performance 60 configs/default.cfg

    # Profile memory usage
    nlmprofile --memory configs/default.cfg

    # Profile for memory leaks
    nlmprofile --memory-leak 30 configs/default.cfg

    # Detect performance bottlenecks
    nlmprofile --bottlenecks configs/default.cfg

    # Benchmark configuration
    nlmprofile --benchmark "neuron_count=500,region_count=1" configs/default.cfg

    # Run all profiling types
    nlmprofile --all configs/default.cfg

    # Save profiling report
    nlmprofile --performance 30 --output profile.txt configs/default.cfg

    # Save profiling report as JSON
    nlmprofile --performance 30 --output profile.json --output-format json configs/default.cfg

EXIT CODES:
    0   Success
    1   Profiling failed
    2   Configuration file not found
    3   Invalid command line arguments
    4   NLM system error
"""
    print(help_text)

if __name__ == "__main__":
    print("NLMProfile - Performance Profiling Tool")
    print("This tool is still being developed.")
    print("Please check back later for the complete implementation.")
