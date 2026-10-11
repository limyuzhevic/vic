#!/usr/bin/env python3
"""
NLM Performance Monitoring and Profiling Tools

This module provides comprehensive performance monitoring and profiling capabilities
for NLM simulations, including memory tracking, timing analysis, and performance optimization.
"""

import time
import tracemalloc
import psutil
import sys
from typing import Dict, List, Optional, Callable, Any
from dataclasses import dataclass, field
from contextlib import contextmanager
import threading
import json
from pathlib import Path
import numpy as np
from enum import Enum

class ProfilingLevel(Enum):
    """Profiling levels for different types of measurements."""
    MINIMAL = "minimal"
    STANDARD = "standard"
    DETAILED = "detailed"
    COMPREHENSIVE = "comprehensive"

@dataclass
class MemoryProfile:
    """Memory profiling data."""
    timestamp: float
    memory_mb: float
    memory_percent: float
    memory_info: Dict
    cpu_percent: float
    cpu_count: int

@dataclass
class TimeProfile:
    """Time profiling data."""
    timestamp: float
    step_number: int
    step_duration: float
    cumulative_time: float
    fps: float
    time_per_step_ms: float

@dataclass
class PerformanceProfile:
    """Complete performance profile."""
    profiling_level: ProfilingLevel
    sampling_interval: float
    memory_samples: List[MemoryProfile] = field(default_factory=list)
    time_samples: List[TimeProfile] = field(default_factory=list)
    step_times: List[float] = field(default_factory=list)
    memory_usage: List[float] = field(default_factory=list)
    cpu_usage: List[float] = field(default_factory=list)
    start_time: float = 0.0
    end_time: float = 0.0
    steps_completed: int = 0

class NLMProfiler:
    """Comprehensive profiler for NLM simulations."""
    
    def __init__(self, profiling_level: ProfilingLevel = ProfilingLevel.STANDARD,
                 sampling_interval: float = 0.1):
        self.profiling_level = profiling_level
        self.sampling_interval = sampling_interval
        self.profile = PerformanceProfile(
            profiling_level=profiling_level,
            sampling_interval=sampling_interval
        )
        self.process = psutil.Process()
        self.monitoring = False
        self.monitor_thread = None
        self.step_times = []
        self.step_start_time = None
        
    def start_profiling(self):
        """Start performance profiling."""
        tracemalloc.start()
        self.profile.start_time = time.time()
        self.step_start_time = self.profile.start_time
        self.monitoring = True
        
        # Start background thread for continuous monitoring
        self.monitor_thread = threading.Thread(target=self._monitoring_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
    def stop_profiling(self):
        """Stop performance profiling."""
        self.monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
        
        self.profile.end_time = time.time()
        self._finalize_profile()
        
    def _monitoring_loop(self):
        """Background monitoring loop."""
        while self.monitoring:
            self._collect_metrics()
            time.sleep(self.sampling_interval)
    
    def _collect_metrics(self):
        """Collect current metrics."""
        timestamp = time.time()
        
        # Get memory info
        memory_info = self.process.memory_info()
        memory_mb = memory_info.rss / 1024 / 1024
        memory_percent = self.process.memory_percent()
        
        # Get CPU info
        cpu_percent = self.process.cpu_percent()
        cpu_count = psutil.cpu_count()
        
        # Create memory profile
        memory_profile = MemoryProfile(
            timestamp=timestamp,
            memory_mb=memory_mb,
            memory_percent=memory_percent,
            memory_info={
                'rss': memory_info.rss,
                'vms': memory_info.vms,
                'shared': memory_info.shared,
                'text': memory_info.text,
                'data': memory_info.data,
                'dirty': memory_info.dirty
            },
            cpu_percent=cpu_percent,
            cpu_count=cpu_count
        )
        
        # Add to profile based on profiling level
        self.profile.memory_samples.append(memory_profile)
        self.profile.memory_usage.append(memory_mb)
        self.profile.cpu_usage.append(cpu_percent)
        
        # For detailed and comprehensive profiling, also collect time samples
        if self.profiling_level in [ProfilingLevel.DETAILED, ProfilingLevel.COMPREHENSIVE]:
            time_profile = TimeProfile(
                timestamp=timestamp,
                step_number=self.profile.steps_completed,
                step_duration=0.0,  # Will be updated after step
                cumulative_time=timestamp - self.profile.start_time,
                fps=1.0 / self.sampling_interval if self.sampling_interval > 0 else 0.0,
                time_per_step_ms=0.0
            )
            self.profile.time_samples.append(time_profile)
    
    def _finalize_profile(self):
        """Calculate final profile statistics."""
        if self.profile.memory_samples:
            # Calculate statistics
            peak_memory = max(self.profile.memory_usage)
            avg_memory = np.mean(self.profile.memory_usage)
            std_memory = np.std(self.profile.memory_usage)
            
            peak_cpu = max(self.profile.cpu_usage)
            avg_cpu = np.mean(self.profile.cpu_usage)
            
            # Store in profile
            self.profile.peak_memory_mb = peak_memory
            self.profile.average_memory_mb = avg_memory
            self.profile.memory_std_mb = std_memory
            self.profile.peak_cpu_percent = peak_cpu
            self.profile.average_cpu_percent = avg_cpu
        
        if self.profile.time_samples:
            # Calculate time statistics
            durations = [tp.step_duration for tp in self.profile.time_samples]
            if durations:
                avg_duration = np.mean(durations)
                std_duration = np.std(durations)
                
                self.profile.average_step_time_ms = avg_duration * 1000
                self.profile.step_time_std_ms = std_duration * 1000
                self.profile.steps_per_second = 1.0 / avg_duration if avg_duration > 0 else 0.0
    
    def start_step(self):
        """Mark the start of a simulation step."""
        self.step_start_time = time.time()
    
    def end_step(self):
        """Mark the end of a simulation step and record timing."""
        if self.step_start_time is not None:
            step_duration = time.time() - self.step_start_time
            self.step_times.append(step_duration)
            self.profile.steps_completed += 1
            
            # Update time samples for detailed profiling
            if self.profiling_level in [ProfilingLevel.DETAILED, ProfilingLevel.COMPREHENSIVE]:
                timestamp = time.time()
                time_profile = TimeProfile(
                    timestamp=timestamp,
                    step_number=self.profile.steps_completed,
                    step_duration=step_duration,
                    cumulative_time=timestamp - self.profile.start_time,
                    fps=1.0 / step_duration if step_duration > 0 else 0.0,
                    time_per_step_ms=step_duration * 1000
                )
                self.profile.time_samples.append(time_profile)
            
            self.step_start_time = None
    
    def get_profile(self) -> PerformanceProfile:
        """Get current performance profile."""
        return self.profile
    
    def export_profile(self, filename: str):
        """Export performance profile to JSON file."""
        data = {
            'profile': self._serialize_profile(),
            'timestamp': time.time()
        }
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
    
    def _serialize_profile(self) -> Dict:
        """Serialize profile data for JSON export."""
        profile_dict = {
            'profiling_level': self.profile.profiling_level.value,
            'sampling_interval': self.profile.sampling_interval,
            'start_time': self.profile.start_time,
            'end_time': self.profile.end_time,
            'steps_completed': self.profile.steps_completed,
            'total_time': self.profile.end_time - self.profile.start_time if self.profile.end_time > 0 else 0
        }
        
        # Add memory data
        if self.profile.memory_samples:
            profile_dict['memory_data'] = [{
                'timestamp': mp.timestamp,
                'memory_mb': mp.memory_mb,
                'memory_percent': mp.memory_percent,
                'cpu_percent': mp.cpu_percent
            } for mp in self.profile.memory_samples]
        
        # Add time data for detailed/complete profiling
        if self.profile.time_samples:
            profile_dict['time_data'] = [{
                'timestamp': tp.timestamp,
                'step_number': tp.step_number,
                'step_duration': tp.step_duration,
                'cumulative_time': tp.cumulative_time,
                'fps': tp.fps,
                'time_per_step_ms': tp.time_per_step_ms
            } for tp in self.profile.time_samples]
        
        # Add performance statistics
        stats = self._calculate_performance_statistics()
        profile_dict['performance_stats'] = stats
        
        return profile_dict
    
    def _calculate_performance_statistics(self) -> Dict:
        """Calculate performance statistics."""
        stats = {}
        
        if self.profile.step_times:
            step_times = self.profile.step_times
            stats['step_time'] = {
                'mean_ms': np.mean(step_times) * 1000,
                'median_ms': np.median(step_times) * 1000,
                'std_ms': np.std(step_times) * 1000,
                'min_ms': np.min(step_times) * 1000,
                'max_ms': np.max(step_times) * 1000,
                'percentile_95_ms': np.percentile(step_times, 95) * 1000,
                'percentile_99_ms': np.percentile(step_times, 99) * 1000
            }
            
            # Calculate performance metrics
            avg_step_time = np.mean(step_times)
            if avg_step_time > 0:
                stats['performance'] = {
                    'steps_per_second': 1.0 / avg_step_time,
                    'steps_per_minute': 60.0 / avg_step_time,
                    'estimated_hours_to_complete': (self.profile.steps_completed * avg_step_time) / 3600.0,
                    'estimated_days_to_complete': (self.profile.steps_completed * avg_step_time) / 86400.0
                }
        
        if self.profile.memory_usage:
            stats['memory'] = {
                'peak_mb': self.profile.peak_memory_mb,
                'average_mb': self.profile.average_memory_mb,
                'std_mb': self.profile.memory_std_mb,
                'min_mb': np.min(self.profile.memory_usage),
                'max_mb': np.max(self.profile.memory_usage)
            }
        
        if self.profile.cpu_usage:
            stats['cpu'] = {
                'peak_percent': self.profile.peak_cpu_percent,
                'average_percent': self.profile.average_cpu_percent,
                'min_percent': np.min(self.profile.cpu_usage),
                'max_percent': np.max(self.profile.cpu_usage)
            }
        
        return stats
    
    def generate_report(self, filename: str = "profile_report.html"):
        """Generate HTML report of performance profile."""
        try:
            html_content = self._create_html_report()
            with open(filename, 'w') as f:
                f.write(html_content)
            print(f"Profile report generated: {filename}")
        except Exception as e:
            print(f"Failed to generate report: {e}")
    
    def _create_html_report(self) -> str:
        """Create HTML report content."""
        stats = self._calculate_performance_statistics()
        
        html = f"""
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Performance Profile Report</title>
            <style>
                body {{ font-family: Arial, sans-serif; margin: 20px; }}
                .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
                .section {{ margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }}
                .metric-grid {{ display: grid; grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); gap: 10px; }}
                .metric {{ background-color: #f8f9fa; padding: 10px; border-radius: 3px; text-align: center; }}
                .metric h3 {{ margin: 0 0 10px 0; color: #495057; }}
                .metric p {{ margin: 5px 0; font-size: 1.2em; font-weight: bold; }}
                .chart {{ margin: 20px 0; text-align: center; }}
                .timeline {{ display: flex; justify-content: space-between; margin: 20px 0; }}
                .timeline-item {{ text-align: center; padding: 10px; background-color: #e9ecef; border-radius: 3px; }}
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Performance Profile Report</h1>
                <p>Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}</p>
                <p>Profiling Level: {self.profile.profiling_level.value}</p>
                <p>Sampling Interval: {self.profile.sampling_interval}s</p>
            </div>
        """
        
        # Add performance summary
        html += f"""
        <div class="section">
            <h2>Performance Summary</h2>
            <div class="metric-grid">
                <div class="metric">
                    <h3>Total Steps</h3>
                    <p>{self.profile.steps_completed}</p>
                </div>
                <div class="metric">
                    <h3>Total Time</h3>
                    <p>{stats.get('time', {}).get('total_seconds', 0):.2f}s</p>
                </div>
                <div class="metric">
                    <h3>Steps per Second</h3>
                    <p>{stats.get('performance', {}).get('steps_per_second', 0):.2f}</p>
                </div>
                <div class="metric">
                    <h3>Peak Memory</h3>
                    <p>{stats.get('memory', {}).get('peak_mb', 0):.1f} MB</p>
                </div>
            </div>
        </div>
        """
        
        # Add step time statistics
        if 'step_time' in stats:
            html += f"""
            <div class="section">
                <h2>Step Time Statistics</h2>
                <div class="metric-grid">
                    <div class="metric">
                        <h3>Average</h3>
                        <p>{stats['step_time']['mean_ms']:.2f} ms</p>
                    </div>
                    <div class="metric">
                        <h3>Median</h3>
                        <p>{stats['step_time']['median_ms']:.2f} ms</p>
                    </div>
                    <div class="metric">
                        <h3>Standard Deviation</h3>
                        <p>{stats['step_time']['std_ms']:.2f} ms</p>
                    </div>
                    <div class="metric">
                        <h3>P95</h3>
                        <p>{stats['step_time']['percentile_95_ms']:.2f} ms</p>
                    </div>
                    <div class="metric">
                        <h3>P99</h3>
                        <p>{stats['step_time']['percentile_99_ms']:.2f} ms</p>
                    </div>
                    <div class="metric">
                        <h3>Min</h3>
                        <p>{stats['step_time']['min_ms']:.2f} ms</p>
                    </div>
                    <div class="metric">
                        <h3>Max</h3>
                        <p>{stats['step_time']['max_ms']:.2f} ms</p>
                    </div>
                </div>
            </div>
            """
        
        # Add memory statistics
        if 'memory' in stats:
            html += f"""
            <div class="section">
                <h2>Memory Statistics</h2>
                <div class="metric-grid">
                    <div class="metric">
                        <h3>Peak</h3>
                        <p>{stats['memory']['peak_mb']:.1f} MB</p>
                    </div>
                    <div class="metric">
                        <h3>Average</h3>
                        <p>{stats['memory']['average_mb']:.1f} MB</p>
                    </div>
                    <div class="metric">
                        <h3>Standard Deviation</h3>
                        <p>{stats['memory'].get('std_mb', 0):.1f} MB</p>
                    </div>
                    <div class="metric">
                        <h3>Minimum</h3>
                        <p>{stats['memory']['min_mb']:.1f} MB</p>
                    </div>
                </div>
            </div>
            """
        
        # Add CPU statistics
        if 'cpu' in stats:
            html += f"""
            <div class="section">
                <h2>CPU Statistics</h2>
                <div class="metric-grid">
                    <div class="metric">
                        <h3>Peak</h3>
                        <p>{stats['cpu']['peak_percent']:.1f}%</p>
                    </div>
                    <div class="metric">
                        <h3>Average</h3>
                        <p>{stats['cpu']['average_percent']:.1f}%</p>
                    </div>
                    <div class="metric">
                        <h3>Minimum</h3>
                        <p>{stats['cpu']['min_percent']:.1f}%</p>
                    </div>
                    <div class="metric">
                        <h3>Maximum</h3>
                        <p>{stats['cpu']['max_percent']:.1f}%</p>
                    </div>
                </div>
            </div>
            """
        
        # Add estimated completion times
        if 'performance' in stats:
            perf = stats['performance']
            html += f"""
            <div class="section">
                <h2>Estimated Completion Times</h2>
                <div class="metric-grid">
                    <div class="metric">
                        <h3>Hours to Complete Current Steps</h3>
                        <p>{perf.get('estimated_hours_to_complete', 0):.2f} h</p>
                    </div>
                    <div class="metric">
                        <h3>Days to Complete Current Steps</h3>
                        <p>{perf.get('estimated_days_to_complete', 0):.2f} d</p>
                    </div>
                </div>
            </div>
            """
        
        html += """
        </body>
        </html>
        """
        
        return html

class NLMProfilerSuite:
    """Suite of profilers for different simulation scenarios."""
    
    def __init__(self):
        self.profilers: List[NLMProfiler] = []
        self.results: List[Dict] = []
    
    def create_minimal_profiler(self) -> NLMProfiler:
        """Create a minimal profiler for basic profiling."""
        profiler = NLMProfiler(ProfilingLevel.MINIMAL, 1.0)
        self.profilers.append(profiler)
        return profiler
    
    def create_standard_profiler(self) -> NLMProfiler:
        """Create a standard profiler for regular simulations."""
        profiler = NLMProfiler(ProfilingLevel.STANDARD, 0.1)
        self.profilers.append(profiler)
        return profiler
    
    def create_detailed_profiler(self, sampling_interval: float = 0.05) -> NLMProfiler:
        """Create a detailed profiler for in-depth analysis."""
        profiler = NLMProfiler(ProfilingLevel.DETAILED, sampling_interval)
        self.profilers.append(profiler)
        return profiler
    
    def create_comprehensive_profiler(self) -> NLMProfiler:
        """Create a comprehensive profiler for thorough analysis."""
        profiler = NLMProfiler(ProfilingLevel.COMPREHENSIVE, 0.01)
        self.profilers.append(profiler)
        return profiler
    
    def run_profiling_scenario(self, profiler: NLMProfiler, brain_factory: Callable,
                              config: Dict, steps: int) -> Dict:
        """Run a profiling scenario with the given profiler."""
        try:
            # Create brain
            brain = brain_factory(config)
            
            # Start profiling
            profiler.start_profiling()
            
            # Run simulation
            for step in range(steps):
                profiler.start_step()
                brain.step(step)
                profiler.end_step()
            
            # Stop profiling
            profiler.stop_profiling()
            
            # Export profile
            filename = f"profile_{profiler.profiling_level.value}_steps_{steps}.json"
            profiler.export_profile(filename)
            
            # Generate report
            report_filename = f"report_{profiler.profiling_level.value}_steps_{steps}.html"
            profiler.generate_report(report_filename)
            
            result = {
                'success': True,
                'profiler': profiler,
                'config': config,
                'steps': steps,
                'profile_file': filename,
                'report_file': report_filename
            }
            
        except Exception as e:
            result = {
                'success': False,
                'error': str(e),
                'profiler': profiler
            }
        
        self.results.append(result)
        return result
    
    def run_comparison_suite(self, brain_factory: Callable, config: Dict, steps: int):
        """Run comparison suite with different profiling levels."""
        print("=== NLM Profiling Suite ===")
        print(f"Running comparison with {steps} steps...")
        
        # Create profilers
        profilers = [
            self.create_minimal_profiler(),
            self.create_standard_profiler(),
            self.create_detailed_profiler(),
            self.create_comprehensive_profiler()
        ]
        
        # Run scenarios
        for i, profiler in enumerate(profilers):
            print(f"\n--- Profiler {i+1}/{len(profilers)}: {profiler.profiling_level.value} ---")
            result = self.run_profiling_scenario(profiler, brain_factory, config, steps)
            
            if result['success']:
                print(f"  Completed: {result['steps']} steps in {profiler.get_profile().end_time - profiler.get_profile().start_time:.2f}s")
            else:
                print(f"  Failed: {result['error']}")
    
    def compare_results(self) -> Dict:
        """Compare results from all profilers."""
        if not self.results:
            return {}
        
        comparison = {}
        
        for result in self.results:
            if result['success']:
                profiler = result['profiler']
                profile = profiler.get_profile()
                
                comparison[profiler.profiling_level.value] = {
                    'steps_per_second': profile.steps_per_second,
                    'peak_memory_mb': profile.peak_memory_mb,
                    'average_memory_mb': profile.average_memory_mb,
                    'average_step_time_ms': profile.average_step_time_ms,
                    'total_time': profile.end_time - profile.start_time,
                    'profiling_overhead': (profile.end_time - profile.start_time) / result['steps'] if result['steps'] > 0 else 0
                }
        
        return comparison

def profile_simulation(brain_factory: Callable, config: Dict, steps: int,
                      profiling_level: ProfilingLevel = ProfilingLevel.STANDARD) -> Dict:
    """
    Profile a single NLM simulation.
    
    Args:
        brain_factory: Function that creates brain from config
        config: Simulation configuration
        steps: Number of simulation steps
        profiling_level: Level of profiling detail
        
    Returns:
        Dictionary containing profiling results
    """
    profiler = NLMProfiler(profiling_level)
    
    try:
        # Create brain
        brain = brain_factory(config)
        
        # Start profiling
        profiler.start_profiling()
        
        # Run simulation
        for step in range(steps):
            profiler.start_step()
            brain.step(step)
            profiler.end_step()
        
        # Stop profiling
        profiler.stop_profiling()
        
        # Export profile
        filename = f"profile_{profiling_level.value}.json"
        profiler.export_profile(filename)
        
        # Generate report
        report_filename = f"report_{profiling_level.value}.html"
        profiler.generate_report(report_filename)
        
        result = {
            'success': True,
            'profiler': profiler,
            'config': config,
            'steps': steps,
            'profile_file': filename,
            'report_file': report_filename,
            'performance_stats': profiler._calculate_performance_statistics()
        }
        
    except Exception as e:
        result = {
            'success': False,
            'error': str(e)
        }
    
    return result

if __name__ == "__main__":
    print("=== NLM Performance Profiling Tools ===")
    print("This module provides comprehensive performance profiling for NLM simulations.")
    print("\nAvailable classes:")
    print("  - NLMProfiler: Basic performance profiler")
    print("  - NLMProfilerSuite: Suite of profilers for comparison")
    print("  - ProfilingLevel: Enum for profiling levels")
    print("\nUsage examples:")
    print("  # Basic profiling")
    print("  from nlm_profiling import NLMProfiler, ProfilingLevel")
    print("  profiler = NLMProfiler(ProfilingLevel.STANDARD)")
    print("  profiler.start_profiling()")
    print("  # Run simulation...")
    print("  profiler.end_step()  # after each step")
    print("  profiler.stop_profiling()")
    print("\nFor help, see documentation in the module docstrings.")

if __name__ == "__main__":
    print("Error: This script should not be run directly. Import the classes for use.")
    sys.exit(1)
