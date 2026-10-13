# NLM - Profiling Examples

**Date:** 2026-10-13
**Audience:** Advanced users
**Level:** Advanced

---

## What You'll Learn

This section provides **profiling examples** demonstrating how to identify and optimize performance bottlenecks in NLM simulations.

1. **Performance Profiling** - Comprehensive profiling of simulation performance
2. **Memory Profiling** - Analyzing memory usage and optimization opportunities
3. **Bottleneck Identification** - Finding and fixing performance issues
4. **Optimization Strategies** - Applying optimization techniques
5. **Scalability Testing** - Testing performance at different scales
6. **Real-World Performance Analysis** - Practical performance optimization

---

## Example 1: Comprehensive Performance Profiling

### What It Does
This example demonstrates comprehensive performance profiling using multiple tools and techniques.

```python
import pynlm
from pynlm.advanced_api import SimulationRunner, SimpleAgent, createBrainProfiler
import time
import cProfile
import pstats
import memory_profiler
import psutil
import numpy as np
import matplotlib.pyplot as plt
from typing import Dict, List, Tuple, Optional

class PerformanceProfiler:
    """Comprehensive performance profiler for NLM simulations"""
    
    def __init__(self, brain):
        self.brain = brain
        self.profiling_data = {
            'cpu_profiling': {},
            'memory_profiling': {},
            'bottleneck_analysis': {},
            'optimization_suggestions': [],
            'performance_metrics': {}
        }
        self.process = psutil.Process()
    
    def start_cpu_profiling(self, duration_steps=10000):
        """Start CPU profiling with detailed metrics"""
        
        print("=== CPU Profiling Started ===")
        
        # Initialize profiler
        profiler = cProfile.Profile()
        profiler.enable()
        
        # Collect system information
        start_time = time.time()
        start_cpu = self.process.cpu_percent()
        start_memory = self.process.memory_info().rss / (1024 * 1024)  # MB
        
        # Run simulation with profiling
        simulation_data = []
        for step in range(duration_steps):
            self.brain.step(step)
            
            # Collect performance metrics every 1000 steps
            if step % 1000 == 0:
                current_time = time.time()
                current_cpu = self.process.cpu_percent()
                current_memory = self.process.memory_info().rss / (1024 * 1024)
                
                metrics = {
                    'step': step,
                    'elapsed_time': current_time - start_time,
                    'cpu_usage': current_cpu,
                    'memory_usage': current_memory,
                    'memory_increase': current_memory - start_memory,
                    'steps_per_second': step / (current_time - start_time) if (current_time - start_time) > 0 else 0
                }
                simulation_data.append(metrics)
        
        # Stop profiler
        profiler.disable()
        end_time = time.time()
        end_memory = self.process.memory_info().rss / (1024 * 1024)
        
        # Analyze profiler results
        stats = pstats.Stats(profiler)
        
        self.profiling_data['cpu_profiling'] = {
            'total_time': end_time - start_time,
            'start_time': start_time,
            'end_time': end_time,
            'cpu_usage_percent': ((start_cpu + self.process.cpu_percent()) / 2),
            'memory_start': start_memory,
            'memory_end': end_memory,
            'memory_increase': end_memory - start_memory,
            'simulation_data': simulation_data,
            'profiler_stats': stats,
            'steps_per_second': duration_steps / (end_time - start_time)
        }
        
        print(f"✅ CPU profiling complete: {self.profiling_data['cpu_profiling']['steps_per_second']:.1f} steps/sec")
    
    @memory_profiler.profile
    def profile_memory_usage(self, duration_steps=10000):
        """Profile memory usage with detailed tracking"""
        
        print("\n=== Memory Profiling Started ===")
        
        # Initial memory snapshot
        initial_memory = self.process.memory_info().rss / (1024 * 1024)
        initial_memory_details = self.process.memory_full_info()
        
        # Memory tracking containers
        memory_samples = []
        garbage_collections = []
        
        # Track memory over time
        for step in range(duration_steps):
            self.brain.step(step)
            
            # Sample memory usage
            if step % 500 == 0:
                current_memory = self.process.memory_info().rss / (1024 * 1024)
                memory_details = self.process.memory_full_info()
                
                memory_sample = {
                    'step': step,
                    'total_memory': current_memory,
                    'memory_increase': current_memory - initial_memory,
                    'memory_percent': (current_memory / self.process.memory_info().vmsize * 100) if self.process.memory_info().vmsize > 0 else 0,
                    
                    # Memory breakdown
                    'rss': memory_details.rss / (1024 * 1024),  # Resident Set Size
                    'vms': memory_details.vms / (1024 * 1024),  # Virtual Memory Size
                    'shared': memory_details.shared / (1024 * 1024),  # Shared Memory
                    'text': memory_details.text / (1024 * 1024),  # Code
                    'data': memory_details.data / (1024 * 1024),  # Data
                    'dirty': memory_details.dirty / (1024 * 1024),  # Dirty Memory
                    
                    # Process information
                    'num_threads': self.process.num_threads(),
                    'num_fds': self.process.num_fds(),
                    'num_ctx_switches': self.process.num_ctx_switches()
                }
                
                memory_samples.append(memory_sample)
        
        # Memory analysis
        final_memory = self.process.memory_info().rss / (1024 * 1024)
        memory_increase = final_memory - initial_memory
        
        self.profiling_data['memory_profiling'] = {
            'initial_memory': initial_memory,
            'final_memory': final_memory,
            'memory_increase': memory_increase,
            'memory_samples': memory_samples,
            'peak_memory': max(sample['total_memory'] for sample in memory_samples) if memory_samples else initial_memory,
            'average_memory': np.mean([sample['total_memory'] for sample in memory_samples]) if memory_samples else initial_memory,
            'memory_growth_rate': memory_increase / duration_steps if duration_steps > 0 else 0,
            'memory_efficiency': self.calculate_memory_efficiency(memory_samples)
        }
        
        print(f"✅ Memory profiling complete: {self.profiling_data['memory_profiling']['peak_memory']:.1f} MB peak, {memory_increase:.1f} MB increase")
    
    def calculate_memory_efficiency(self, memory_samples):
        """Calculate memory efficiency metrics"""
        
        if not memory_samples:
            return {'efficiency': 0, 'utilization': 0}
        
        total_samples = len(memory_samples)
        peak_memory = max(sample['total_memory'] for sample in memory_samples)
        avg_memory = np.mean([sample['total_memory'] for sample in memory_samples])
        
        # Calculate memory efficiency (how much memory is used for actual computation)
        # Simplified calculation based on step count and memory usage
        memory_per_step = avg_memory / total_samples if total_samples > 0 else 0
        
        # Estimate useful work per memory unit (simplified)
        efficiency = 1.0 / (1.0 + memory_per_step)
        
        return {
            'efficiency': efficiency,
            'memory_per_step': memory_per_step,
            'peak_memory_efficiency': peak_memory / avg_memory if avg_memory > 0 else 1.0,
            'memory_growth_rate': (memory_samples[-1]['total_memory'] - memory_samples[0]['total_memory']) / len(memory_samples)
        }
    
    def analyze_bottlenecks(self, profiler_stats):
        """Analyze profiler results to identify bottlenecks"""
        
        print("\n=== Bottleneck Analysis Started ===")
        
        bottlenecks = []
        total_time = profiler_stats.get_stats_profile().total_tt
        
        # Get top time-consuming functions
        func_profiles = profiler_stats.get_stats_profile().func_profiles
        
        for func_name, func_stats in func_profiles.items():
            time_percentage = (func_stats.tt / total_time) * 100
            call_count = func_stats.ncall
            
            # Identify potential bottlenecks
            if time_percentage > 5 and call_count > 100:
                severity = 'high' if time_percentage > 15 else 'medium'
                
                bottlenecks.append({
                    'function': func_name,
                    'time_percentage': time_percentage,
                    'call_count': call_count,
                    'average_time_per_call': func_stats.tt / call_count if call_count > 0 else 0,
                    'severity': severity,
                    'optimization_potential': 'high' if time_percentage > 20 else 'medium'
                })
        
        # Sort by time percentage (descending)
        bottlenecks.sort(key=lambda x: x['time_percentage'], reverse=True)
        
        self.profiling_data['bottleneck_analysis'] = {
            'total_functions_analyzed': len(func_profiles),
            'bottlenecks_found': bottlenecks,
            'top_bottleneck': bottlenecks[0] if bottlenecks else None,
            'total_time_wasted': sum(b['time_percentage'] for b in bottlenecks)
        }
        
        print(f"✅ Bottleneck analysis complete: {len(bottlenecks)} bottlenecks identified")
        for bottleneck in bottlenecks[:5]:  # Show top 5
            print(f"   • {bottleneck['function']}: {bottleneck['time_percentage']:.1f}% (Severity: {bottleneck['severity']})")
    
    def suggest_optimizations(self):
        """Suggest optimization strategies based on profiling results"""
        
        print("\n=== Optimization Suggestions Started ===")
        
        suggestions = []
        
        # CPU optimization suggestions
        if 'cpu_profiling' in self.profiling_data:
            cpu_data = self.profiling_data['cpu_profiling']
            steps_per_second = cpu_data['steps_per_second']
            
            if steps_per_second < 1000:
                suggestions.extend([
                    "Enable parallel processing to improve CPU utilization",
                    "Optimize hot paths in neural computation",
                    "Use SIMD vectorization for neural calculations",
                    "Consider event-driven processing for sparse spikes"
                ])
            elif steps_per_second < 5000:
                suggestions.extend([
                    "Optimize memory access patterns",
                    "Reduce synchronization overhead",
                    "Consider GPU acceleration for large-scale simulations"
                ])
            else:
                suggestions.extend([
                    "Focus on memory optimization",
                    "Improve cache locality",
                    "Consider specialized hardware for peak performance"
                ])
        
        # Memory optimization suggestions
        if 'memory_profiling' in self.profiling_data:
            mem_data = self.profiling_data['memory_profiling']
            
            if mem_data['memory_increase'] > mem_data['initial_memory'] * 0.2:
                suggestions.extend([
                    "Enable memory pooling to reduce allocation overhead",
                    "Implement sparse connectivity to reduce memory footprint",
                    "Add memory leak detection and debugging"
                ])
            
            if mem_data['memory_efficiency']['efficiency'] < 0.5:
                suggestions.extend([
                    "Optimize memory allocation patterns",
                    "Consider pre-allocating memory pools",
                    "Implement memory compression strategies"
                ])
        
        # Bottleneck-based suggestions
        if 'bottleneck_analysis' in self.profiling_data:
            bottlenecks = self.profiling_data['bottleneck_analysis']['bottlenecks_found']
            
            for bottleneck in bottlenecks[:3]:  # Top 3 bottlenecks
                if bottleneck['severity'] == 'high':
                    suggestions.append(
                        f"Optimize {bottleneck['function']}: {bottleneck['time_percentage']:.1f}% CPU time wasted"
                    )
        
        # Remove duplicates and prioritize
        unique_suggestions = list(dict.fromkeys(suggestions))
        
        self.profiling_data['optimization_suggestions'] = unique_suggestions
        
        print(f"✅ Optimization suggestions complete: {len(unique_suggestions)} recommendations")
        for i, suggestion in enumerate(unique_suggestions[:5], 1):  # Show top 5
            print(f"   {i}. {suggestion}")
    
    def generate_performance_report(self):
        """Generate comprehensive performance report"""
        
        print("\n=== Performance Report Generation ===")
        
        report = {
            'performance_summary': {
                'total_time': self.profiling_data.get('cpu_profiling', {}).get('total_time', 0),
                'steps_per_second': self.profiling_data.get('cpu_profiling', {}).get('steps_per_second', 0),
                'peak_memory': self.profiling_data.get('memory_profiling', {}).get('peak_memory', 0),
                'memory_efficiency': self.profiling_data.get('memory_profiling', {}).get('memory_efficiency', {}).get('efficiency', 0)
            },
            
            'bottleneck_analysis': self.profiling_data.get('bottleneck_analysis', {}),
            
            'optimization_opportunities': self.profiling_data.get('optimization_suggestions', []),
            
            'performance_grade': self.calculate_performance_grade(),
            
            'recommendations': self.generate_recommendations()
        }
        
        return report
    
    def calculate_performance_grade(self):
        """Calculate overall performance grade"""
        
        if 'cpu_profiling' not in self.profiling_data:
            return {'grade': 'N/A', 'score': 0}
        
        cpu_data = self.profiling_data['cpu_profiling']
        mem_data = self.profiling_data.get('memory_profiling', {})
        
        # Calculate scores
        cpu_score = min(cpu_data['steps_per_second'] / 1000, 1.0) * 0.6
        
        if mem_data:
            mem_score = mem_data['memory_efficiency']['efficiency'] * 0.4
        else:
            mem_score = 0.4
        
        total_score = cpu_score + mem_score
        
        # Convert to grade
        if total_score >= 0.9:
            grade = 'A+'
        elif total_score >= 0.8:
            grade = 'A'
        elif total_score >= 0.7:
            grade = 'B+'
        elif total_score >= 0.6:
            grade = 'B'
        elif total_score >= 0.5:
            grade = 'C'
        elif total_score >= 0.3:
            grade = 'D'
        else:
            grade = 'F'
        
        return {
            'grade': grade,
            'score': total_score,
            'cpu_score': cpu_score,
            'memory_score': mem_score
        }
    
    def generate_recommendations(self):
        """Generate specific recommendations based on profiling"""
        
        recommendations = []
        
        # Performance recommendations
        if 'cpu_profiling' in self.profiling_data:
            cpu_data = self.profiling_data['cpu_profiling']
            
            if cpu_data['steps_per_second'] < 1000:
                recommendations.extend([
                    "Enable parallel processing to improve throughput",
                    "Optimize neural computation kernels",
                    "Consider reducing simulation resolution for real-time applications",
                    "Implement caching for frequently accessed data"
                ])
            
            if cpu_data['steps_per_second'] > 5000:
                recommendations.extend([
                    "Focus on memory bandwidth optimization",
                    "Consider NUMA-aware memory allocation",
                    "Optimize data layouts for cache locality"
                ])
        
        # Memory recommendations
        if 'memory_profiling' in self.profiling_data:
            mem_data = self.profiling_data['memory_profiling']
            
            if mem_data['memory_increase'] > mem_data['initial_memory'] * 0.1:
                recommendations.extend([
                    "Implement memory pooling to reduce allocation overhead",
                    "Add periodic memory cleanup and garbage collection",
                    "Consider memory-efficient data structures"
                ])
            
            if mem_data['memory_efficiency']['efficiency'] < 0.5:
                recommendations.extend([
                    "Optimize memory access patterns",
                    "Implement pre-allocation strategies",
                    "Consider memory compression for large-scale simulations"
                ])
        
        # Bottleneck recommendations
        if 'bottleneck_analysis' in self.profiling_data:
            bottlenecks = self.profiling_data['bottleneck_analysis']['bottlenecks_found']
            
            for bottleneck in bottlenecks[:3]:
                recommendations.append(
                    f"Optimize {bottleneck['function']} (takes {bottleneck['time_percentage']:.1f}% of CPU time)"
                )
        
        return recommendations

# Run comprehensive performance profiling
print("Starting comprehensive performance profiling...")
print("This will profile CPU usage, memory consumption, and identify bottlenecks.")
print("Please wait while profiling data is collected...\n")

# Create test brain
brain = pynlm.createBrain(pynlm.createDefaultConfig())
brain.initialize()

# Initialize profiler
profiler = PerformanceProfiler(brain)

# Run profiling
profiler.start_cpu_profiling(5000)  # Profile 5000 steps
profiler.profile_memory_usage(5000)  # Profile memory usage
profiler.analyze_bottlenecks(profiler.profiling_data['cpu_profiling']['profiler_stats'])
profiler.suggest_optimizations()

# Generate performance report
performance_report = profiler.generate_performance_report()

print("\n=== Performance Profiling Complete ===")
print(f"Total profiling time: {performance_report['performance_summary']['total_time']:.1f} seconds")
print(f"Performance: {performance_report['performance_summary']['steps_per_second']:.1f} steps/sec")
print(f"Peak memory: {performance_report['performance_summary']['peak_memory']:.1f} MB")
print(f"Memory efficiency: {performance_report['performance_summary']['memory_efficiency']:.1%}")
print(f"Performance grade: {performance_report['performance_grade']['grade']} (score: {performance_report['performance_grade']['score']:.2f})")

print(f"\n=== Top Performance Bottlenecks ===")
if performance_report['bottleneck_analysis']['bottlenecks_found']:
    for i, bottleneck in enumerate(performance_report['bottleneck_analysis']['bottlenecks_found'][:5], 1):
        print(f"{i}. {bottleneck['function']}: {bottleneck['time_percentage']:.1f}% CPU time (Severity: {bottleneck['severity']})")
else:
    print("No significant bottlenecks identified")

print(f"\n=== Optimization Recommendations ===")
for i, recommendation in enumerate(performance_report['optimization_opportunities'][:5], 1):
    print(f"{i}. {recommendation}")

print(f"\n=== Performance Summary ===")
print("✅ Performance profiling complete!")
print("The analysis provides detailed insights into simulation performance:")
print("  • CPU utilization and bottlenecks")
print("  • Memory usage patterns and efficiency")
print("  • Optimization opportunities")
print("  • Performance grade and recommendations")
```