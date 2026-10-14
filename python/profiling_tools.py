# Python integration module for NLM Performance Profiling and Benchmarking

import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from datetime import datetime
import json
import sys
import os

# Add the src directory to Python path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'src'))

try:
    import pybind11
    import profiling_tools
    HAVE_PROFILING = True
except ImportError:
    HAVE_PROFILING = False
    print("Warning: profiling_tools module not available. Using basic Python implementations.")

class NLMProfiler:
    """Python wrapper for NLM performance profiling and benchmarking tools."""
    
    def __init__(self):
        if HAVE_PROFILING:
            self.profiler = profiling_tools.getProfiler()
            self.benchmark_suite = profiling_tools.getBenchmarkSuite()
            self.memory_analyzer = profiling_tools.getMemoryAnalyzer()
            self.cpu_memory_profiler = profiling_tools.getCPUMemoryProfiler()
            self.output_formatter = profiling_tools.getOutputFormatter()
        else:
            # Fallback Python implementation
            self.benchmark_results = []
            self.memory_usage_history = []
            self.cpu_usage_history = []
    
    def start_session(self, name):
        """Start a profiling session."""
        if HAVE_PROFILING:
            self.profiler.startSession(name)
        else:
            print(f"Starting session: {name}")
    
    def end_session(self):
        """End the current profiling session."""
        if HAVE_PROFILING:
            self.profiler.endSession()
        else:
            print("Ending session")
    
    def record_timer(self, name, duration):
        """Record a timer measurement."""
        if HAVE_PROFILING:
            self.profiler.recordTimer(name, duration)
        else:
            print(f"Timer '{name}': {duration}s")
    
    def add_benchmark(self, name, benchmark_func, category="general", parameters=None):
        """Add a benchmark to the suite."""
        if parameters is None:
            parameters = {}
        
        if HAVE_PROFILING:
            self.benchmark_suite.addBenchmark(name, benchmark_func, category, parameters)
        else:
            self.benchmark_results.append({
                'name': name,
                'category': category,
                'parameters': parameters,
                'benchmark_func': benchmark_func
            })
    
    def run_benchmarks(self):
        """Run all registered benchmarks."""
        if HAVE_PROFILING:
            return self.benchmark_suite.runBenchmarks([])
        else:
            results = []
            for benchmark in self.benchmark_results:
                start_time = datetime.now()
                result = benchmark['benchmark_func']()
                end_time = datetime.now()
                duration = (end_time - start_time).total_seconds()
                
                results.append({
                    'name': benchmark['name'],
                    'category': benchmark['category'],
                    'parameters': benchmark['parameters'],
                    'duration': duration,
                    'result': result
                })
            return results
    
    def export_results(self, filename, format="csv"):
        """Export benchmark results."""
        if HAVE_PROFILING:
            self.benchmark_suite.exportResults(filename, format)
        else:
            if format == "csv":
                df = pd.DataFrame(self.benchmark_results)
                df.to_csv(filename, index=False)
            elif format == "json":
                with open(filename, 'w') as f:
                    json.dump(self.benchmark_results, f, indent=2)
            else:
                raise ValueError(f"Unsupported format: {format}")
    
    def start_memory_tracking(self, session_name="default"):
        """Start memory tracking."""
        if HAVE_PROFILING:
            self.memory_analyzer.startTracking(session_name)
        else:
            print(f"Starting memory tracking for session: {session_name}")
    
    def stop_memory_tracking(self):
        """Stop memory tracking."""
        if HAVE_PROFILING:
            self.memory_analyzer.stopTracking()
        else:
            print("Stopping memory tracking")
    
    def profile_operation(self, operation, name="operation", measure_memory=True):
        """Profile a specific operation."""
        if HAVE_PROFILING:
            return self.cpu_memory_profiler.profileOperation(operation, name, measure_memory)
        else:
            start_time = datetime.now()
            if measure_memory:
                start_memory = self._get_memory_usage()
            
            operation()
            
            end_time = datetime.now()
            duration = (end_time - start_time).total_seconds()
            
            if measure_memory:
                end_memory = self._get_memory_usage()
                memory_used = end_memory - start_memory
            else:
                memory_used = 0
            
            return {
                'operation': name,
                'cpu_time': duration,
                'memory_used': memory_used,
                'timestamp': start_time.isoformat()
            }
    
    def _get_memory_usage(self):
        """Get current memory usage (platform-dependent)."""
        try:
            import psutil
            return psutil.virtual_memory().used / (1024 * 1024)  # MB
        except ImportError:
            return 0
    
    def generate_report(self, filename, format="html"):
        """Generate a comprehensive performance report."""
        if HAVE_PROFILING:
            self.benchmark_suite.generateComprehensiveReport(filename, format)
        else:
            if format == "html":
                self._generate_html_report(filename)
            elif format == "json":
                self._generate_json_report(filename)
            else:
                raise ValueError(f"Unsupported format: {format}")
    
    def _generate_html_report(self, filename):
        """Generate an HTML report."""
        html_content = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Performance Report</title>
            <style>
                body { font-family: Arial, sans-serif; margin: 40px; }
                .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
                .benchmark { margin: 20px 0; padding: 15px; border: 1px solid #ddd; }
                .regression { background-color: #ffcccc; }
                table { border-collapse: collapse; width: 100%; }
                th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }
                th { background-color: #f2f2f2; }
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Performance Report</h1>
                <p>Generated on: {timestamp}</p>
            </div>
        """.format(timestamp=datetime.now().isoformat())
        
        for benchmark in self.benchmark_results:
            html_content += f"""
            <div class="benchmark">
                <h2>{benchmark['name']}</h2>
                <p><strong>Category:</strong> {benchmark['category']}</p>
                <p><strong>Duration:</strong> {benchmark['duration']:.4f}s</p>
                <p><strong>Result:</strong> {benchmark['result']}</p>
            </div>
            """
        
        html_content += """
        </body>
        </html>
        """
        
        with open(filename, 'w') as f:
            f.write(html_content)
    
    def _generate_json_report(self, filename):
        """Generate a JSON report."""
        report = {
            'timestamp': datetime.now().isoformat(),
            'benchmarks': self.benchmark_results,
            'summary': self._generate_summary()
        }
        
        with open(filename, 'w') as f:
            json.dump(report, f, indent=2)
    
    def _generate_summary(self):
        """Generate a summary of benchmark results."""
        if not self.benchmark_results:
            return {}
        
        total_duration = sum(b['duration'] for b in self.benchmark_results)
        avg_duration = total_duration / len(self.benchmark_results)
        
        return {
            'total_benchmarks': len(self.benchmark_results),
            'total_duration': total_duration,
            'average_duration': avg_duration,
            'categories': list(set(b['category'] for b in self.benchmark_results))
        }

class BrainPerformanceProfiler:
    """Python integration for brain system performance profiling."""
    
    def __init__(self):
        if HAVE_PROFILING:
            self.profiler = profiling_tools.getProfiler()
        else:
            pass
    
    def profile_brain_step(self, brain, steps=1, enable_profiler=False):
        """Profile a brain simulation step."""
        if HAVE_PROFILING:
            return profiling_tools.profileBrainStep(brain, steps, enable_profiler)
        else:
            return {
                'neurons': brain.get_total_neuron_count() if hasattr(brain, 'get_total_neuron_count') else 0,
                'synapses': brain.get_total_synapse_count() if hasattr(brain, 'get_total_synapse_count') else 0,
                'spikes': brain.get_total_spike_count() if hasattr(brain, 'get_total_spike_count') else 0,
                'steps': steps
            }

class PerformanceComparison:
    """Python wrapper for performance comparison and regression detection."""
    
    def __init__(self):
        if HAVE_PROFILING:
            self.comparator = profiling_tools.getPerformanceComparator()
        else:
            pass
    
    def add_baseline(self, name, results):
        """Add baseline results."""
        if HAVE_PROFILING:
            self.comparator.addBaseline(name, results)
        else:
            print(f"Adding baseline '{name}' with {len(results)} results")
    
    def add_current(self, name, results):
        """Add current results."""
        if HAVE_PROFILING:
            self.comparator.addCurrent(name, results)
        else:
            print(f"Adding current '{name}' with {len(results)} results")
    
    def compare_all(self):
        """Compare all baseline and current results."""
        if HAVE_PROFILING:
            return self.comparator.compareAll()
        else:
            # Simple comparison for fallback
            comparisons = []
            baseline_data = self._load_baseline_data()
            current_data = self._load_current_data()
            
            for baseline in baseline_data:
                for current in current_data:
                    if baseline['name'] == current['name']:
                        ratio = (current['duration'] / baseline['duration']) * 100
                        is_regression = ratio > 110  # 10% threshold
                        
                        comparisons.append({
                            'testName': baseline['name'],
                            'baselineName': 'baseline',
                            'currentName': 'current',
                            'baselineTime': baseline['duration'],
                            'currentTime': current['duration'],
                            'timeRatio': ratio,
                            'isRegression': is_regression,
                            'confidence': 0.95
                        })
            
            return comparisons
    
    def _load_baseline_data(self):
        """Load baseline data (placeholder)."""
        # This would load from a file or database
        return []
    
    def _load_current_data(self):
        """Load current data (placeholder)."""
        # This would load from a file or database
        return []
    
    def export_comparisons(self, filename, format="csv"):
        """Export comparison results."""
        if HAVE_PROFILING:
            self.comparator.exportComparison(filename, format)
        else:
            comparisons = self.compareAll()
            if format == "csv":
                df = pd.DataFrame(comparisons)
                df.to_csv(filename, index=False)
            elif format == "json":
                with open(filename, 'w') as f:
                    json.dump(comparisons, f, indent=2)
    
    def generate_regression_report(self, filename):
        """Generate a regression report."""
        if HAVE_PROFILING:
            self.comparator.generateRegressionReport(filename)
        else:
            comparisons = self.compareAll()
            regressions = [c for c in comparisons if c['isRegression']]
            
            with open(filename, 'w') as f:
                f.write("Performance Regression Report\n")
                f.write("=============================\n\n")
                f.write(f"Total comparisons: {len(comparisons)}\n")
                f.write(f"Regressions detected: {len(regressions)}\n\n")
                
                for comparison in comparisons:
                    f.write(f"- {comparison['testName']}: {comparison['baselineTime']:.4f}s -> {comparison['currentTime']:.4f}s (ratio: {comparison['timeRatio']:.1f}%, regression: {'YES' if comparison['isRegression'] else 'NO'})\n")

# Factory functions for easy access
def get_profiler():
    """Get the global profiler instance."""
    return NLMProfiler()

def get_brain_profiler():
    """Get the brain performance profiler instance."""
    return BrainPerformanceProfiler()

def get_performance_comparison():
    """Get the performance comparison instance."""
    return PerformanceComparison()

# Convenience functions
def run_benchmark_suite(benchmarks, output_file="benchmark_results.csv"):
    """Run a suite of benchmarks and save results."""
    profiler = NLMProfiler()
    for name, func in benchmarks.items():
        profiler.add_benchmark(name, func)
    
    results = profiler.run_benchmarks()
    profiler.export_results(output_file)
    return results

def profile_neural_dynamics(neural_dynamics, steps=100):
    """Profile neural dynamics simulation."""
    profiler = NLMProfiler()
    profiler.start_session("neural_dynamics")
    
    for i in range(steps):
        profiler.record_timer(f"step_{i}", 0.001)  # Mock timing
    
    profiler.end_session()
    return profiler

# Version and metadata
__version__ = "1.0.0"
__author__ = "NLM Performance Team"

# Example usage
def example_usage():
    """Example usage of the profiling tools."""
    print("NLM Performance Profiling and Benchmarking Tools")
    print("=" * 50)
    
    # Create profiler
    profiler = NLMProfiler()
    
    # Define some example benchmarks
    def memory_benchmark():
        import random
        data = [random.random() for _ in range(100000)]
        return sum(data)
    
    def cpu_benchmark():
        import math
        result = sum(math.sqrt(i) for i in range(10000))
        return result
    
    # Add benchmarks
    profiler.add_benchmark("memory_test", memory_benchmark, "memory")
    profiler.add_benchmark("cpu_test", cpu_benchmark, "cpu")
    
    # Run benchmarks
    results = profiler.run_benchmarks()
    
    # Generate report
    profiler.generate_report("performance_report.html")
    
    print(f"Benchmark results: {len(results)} benchmarks completed")
    print("Report generated: performance_report.html")

if __name__ == "__main__":
    example_usage()
