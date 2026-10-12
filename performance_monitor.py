#!/usr/bin/env python3

"""
NLM Performance Monitoring and Profiling Tool

A Python interface for monitoring and profiling NLM simulations.
This tool provides basic performance monitoring capabilities
to help optimize NLM configurations and identify performance bottlenecks.

Usage:
    python3 performance_monitor.py [options]

Options:
    --config FILE          Configuration file to load (default: configs/default.cfg)
    --duration SECONDS     Duration to run monitoring (default: 60)
    --output FILE          Output file for metrics (default: metrics.json)
    --help                 Show help message
    --version              Show version information

This is a basic performance monitoring framework. For advanced profiling,
consider using external tools like:
    • valgrind
    • perf
    • gprof
    • vtune
"""

import json
import os
import sys
import time
import threading
import psutil
from datetime import datetime
from typing import Dict, Any, List, Optional

# Add the Python bindings to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Performance monitoring will be limited.")

class PerformanceMonitor:
    """Basic performance monitoring for NLM simulations."""
    
    def __init__(self):
        self.version = "1.0.0"
        self.default_config_file = "configs/default.cfg"
        self.monitoring = False
        self.metrics = {
            'start_time': None,
            'end_time': None,
            'cpu_usage': [],
            'memory_usage': [],
            'thread_count': [],
            'nlm_metrics': {}
        }
    
    def load_config(self, filepath: str) -> bool:
        """Load configuration from file."""
        print(f"Loading configuration: {filepath}")
        
        if not os.path.exists(filepath):
            print(f"ERROR: Configuration file not found: {filepath}")
            return False
        
        if PYNLM_AVAILABLE:
            try:
                self.config = pynlm.createDefaultConfig()
                if not self.config.loadFromFile(filepath):
                    print(f"ERROR: Failed to load configuration file: {filepath}")
                    return False
                
                print(f"✓ Configuration loaded from: {filepath}")
                return True
                
            except Exception as e:
                print(f"ERROR: {str(e)}")
                return False
        else:
            print("✗ pynlm not available. Cannot load configuration.")
            return False
    
    def collect_system_metrics(self) -> Dict[str, Any]:
        """Collect current system metrics."""
        process = psutil.Process(os.getpid())
        
        metrics = {
            'timestamp': datetime.now().isoformat(),
            'cpu_percent': process.cpu_percent(),
            'memory_rss': process.memory_info().rss / 1024 / 1024,  # MB
            'memory_vms': process.memory_info().vms / 1024 / 1024,  # MB
            'memory_percent': process.memory_percent(),
            'num_threads': process.num_threads(),
            'num_fds': process.num_fds() if hasattr(process, 'num_fds') else 0,
            'create_time': process.create_time()
        }
        
        return metrics
    
    def monitor_performance(self, duration: int = 60):
        """Monitor performance for specified duration."""
        print(f"Starting performance monitoring for {duration} seconds...")
        print("Press Ctrl+C to stop monitoring early")
        
        self.monitoring = True
        self.metrics['start_time'] = datetime.now().isoformat()
        
        try:
            start_time = time.time()
            
            while self.monitoring and (time.time() - start_time) < duration:
                # Collect metrics
                metrics = self.collect_system_metrics()
                
                # Store metrics
                self.metrics['cpu_usage'].append(metrics['cpu_percent'])
                self.metrics['memory_usage'].append(metrics['memory_rss'])
                self.metrics['thread_count'].append(metrics['num_threads'])
                
                # Print progress
                elapsed = time.time() - start_time
                print(f"  {elapsed:5.1f}s | CPU: {metrics['cpu_percent']:5.1f}% | "
                      f"Memory: {metrics['memory_rss']:8.1f} MB | "
                      f"Threads: {metrics['num_threads']:3d}", end='\r')
                
                time.sleep(1)
            
            self.monitoring = False
            self.metrics['end_time'] = datetime.now().isoformat()
            
            print("\nMonitoring completed.")
            
        except KeyboardInterrupt:
            print("\nMonitoring interrupted by user.")
            self.monitoring = False
        
        self.generate_report()
    
    def generate_report(self) -> Dict[str, Any]:
        """Generate performance report."""
        print("Generating performance report...")
        
        report = {
            'summary': {},
            'metrics': {},
            'recommendations': []
        }
        
        # Calculate summary statistics
        if self.metrics['cpu_usage']:
            report['summary']['cpu'] = {
                'average': sum(self.metrics['cpu_usage']) / len(self.metrics['cpu_usage']),
                'maximum': max(self.metrics['cpu_usage']),
                'minimum': min(self.metrics['cpu_usage']),
                'samples': len(self.metrics['cpu_usage'])
            }
        
        if self.metrics['memory_usage']:
            report['summary']['memory'] = {
                'average': sum(self.metrics['memory_usage']) / len(self.metrics['memory_usage']),
                'maximum': max(self.metrics['memory_usage']),
                'minimum': min(self.metrics['memory_usage']),
                'samples': len(self.metrics['memory_usage'])
            }
        
        if self.metrics['thread_count']:
            report['summary']['threads'] = {
                'average': sum(self.metrics['thread_count']) / len(self.metrics['thread_count']),
                'maximum': max(self.metrics['thread_count']),
                'minimum': min(self.metrics['thread_count']),
                'samples': len(self.metrics['thread_count'])
            }
        
        # Performance recommendations
        if report['summary'].get('cpu', {}).get('average', 0) > 80:
            report['recommendations'].append(
                "High CPU usage detected (>80%). Consider optimizing algorithms or increasing thread count."
            )
        
        if report['summary'].get('memory', {}).get('average', 0) > 500:
            report['recommendations'].append(
                "High memory usage detected (>500MB). Consider enabling memory pools or compression."
            )
        
        if report['summary'].get('threads', {}).get('average', 1) > 4:
            report['recommendations'].append(
                "High thread count detected. Check for thread contention or consider reducing parallelism."
            )
        
        # Add configuration recommendations if available
        if hasattr(self, 'config') and self.config:
            neuron_count = self.config.getOr("neuron_count", 0)
            connection_prob = self.config.getOr("connection_probability", 0.0)
            max_steps = self.config.getOr("max_simulation_steps", 0)
            
            if neuron_count > 10000:
                report['recommendations'].append(
                    f"Large neuron count ({neuron_count}). Enable memory pools and SIMD optimization."
                )
            
            if connection_prob > 0.5:
                report['recommendations'].append(
                    "High connection probability. Enable SIMD optimization and consider spatial partitioning."
                )
            
            if max_steps > 10000:
                report['recommendations'].append(
                    f"Large simulation steps ({max_steps}). Consider parallel processing and checkpointing."
                )
        
        # Store report in metrics
        self.metrics['report'] = report
        
        # Save to file
        output_file = os.path.join(os.path.dirname(__file__), 'performance_metrics.json')
        try:
            with open(output_file, 'w') as f:
                json.dump(self.metrics, f, indent=2, default=str)
            
            print(f"Report saved to: {output_file}")
        except Exception as e:
            print(f"Warning: Could not save report: {str(e)}")
        
        # Print summary
        print("\nPerformance Summary:")
        print("-" * 20)
        
        for category, stats in report['summary'].items():
            print(f"{category.title()}:")
            for metric, value in stats.items():
                if isinstance(value, float):
                    print(f"  {metric}: {value:.2f}")
                else:
                    print(f"  {metric}: {value}")
        
        print("\nRecommendations:")
        print("-" * 20)
        for rec in report['recommendations']:
            print(f"• {rec}")
        
        return report
    
    def show_help(self):
        """Show help information."""
        help_text = """
NLM Performance Monitoring Tool
===============================

This tool monitors system performance during NLM simulations.

Usage:
    python3 performance_monitor.py [options]

Options:
    --config FILE          Configuration file to load (default: configs/default.cfg)
    --duration SECONDS     Duration to run monitoring (default: 60)
    --output FILE          Output file for metrics (default: metrics.json)
    --help                 Show this help message
    --version              Show version information

Features:
    • Real-time CPU usage monitoring
    • Memory usage tracking
    • Thread count monitoring
    • Performance recommendations
    • Report generation in JSON format

Performance Recommendations:
    • High CPU (>80%): Optimize algorithms or increase thread count
    • High memory (>500MB): Enable memory pools or compression
    • High threads (>4): Check for thread contention

Advanced Configuration Options:
    • enable_simd: Enable SIMD optimization
    • max_threads: Set maximum thread count
    • enable_profiling: Enable performance profiling
    • enable_memory_pools: Enable memory pool optimization
    • enable_swapping: Enable memory swapping

For advanced profiling, use external tools:
    • valgrind
    • perf
    • gprof
    • Intel VTune
"""
        print(help_text)
    
    def show_version(self):
        """Show version information."""
        print(f"NLM Performance Monitor v{self.version}")
        print("Basic performance monitoring framework")
        print(f"Python bindings available: {'Yes' if PYNLM_AVAILABLE else 'No'}")
        if PYNLM_AVAILABLE:
            import platform
            print(f"Platform: {platform.system()} {platform.release()}")
            print(f"Python version: {platform.python_version()}")
def main():
    """Main entry point."""
    print("NLM Performance Monitoring Tool")
    print("=" * 30)
    
    monitor = PerformanceMonitor()
    
    if len(sys.argv) == 1:
        monitor.show_help()
        return 0
    
    try:
        arg1 = sys.argv[1]
        
        if arg1 == "--help" or arg1 == "-h":
            monitor.show_help()
            return 0
        elif arg1 == "--version":
            monitor.show_version()
            return 0
        elif arg1 == "--duration":
            if len(sys.argv) > 2:
                try:
                    duration = int(sys.argv[2])
                    monitor.load_config(monitor.default_config_file)
                    monitor.monitor_performance(duration)
                    return 0
                except ValueError:
                    print("ERROR: Invalid duration value")
                    return 1
            else:
                print("ERROR: --duration option requires a value")
                return 1
        elif arg1 == "--config":
            if len(sys.argv) > 2:
                config_file = sys.argv[2]
                if len(sys.argv) > 3 and sys.argv[3] == "--duration":
                    if len(sys.argv) > 4:
                        try:
                            duration = int(sys.argv[4])
                            if monitor.load_config(config_file):
                                monitor.monitor_performance(duration)
                            return 0
                        except ValueError:
                            print("ERROR: Invalid duration value")
                            return 1
                    else:
                        print("ERROR: --duration option requires a value")
                        return 1
                else:
                    if monitor.load_config(config_file):
                        monitor.monitor_performance(60)
                    return 0
            else:
                print("ERROR: --config option requires a filename")
                return 1
        else:
            # Assume default behavior
            if monitor.load_config(monitor.default_config_file):
                monitor.monitor_performance(60)
            return 0
            
    except Exception as e:
        print(f"ERROR: {str(e)}")
        return 1
if __name__ == "__main__":
    sys.exit(main())