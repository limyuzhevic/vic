#!/usr/bin/env python3
"""
NLM Performance Profiler
Advanced profiling and debugging tools for NLM brain simulations

This script provides:
- Real-time performance monitoring
- Bottleneck identification
- Memory usage tracking
- Profile analysis
- Visualization output export
"""

import time
import sys
import json
import csv
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Optional, Any
from dataclasses import dataclass, field
from collections import defaultdict, deque
import threading
import queue
import math

@dataclass
class PerformanceMetric:
    """Container for performance metrics"""
    timestamp: float
    step: int
    neurons_active: int
    neurons_firing: int
    total_spikes: int
    total_synapses: int
    simulation_time: float
    cpu_usage: float
    memory_usage: float
    plasticity_updates: int
    visualization_fps: float

@dataclass
class ProfileData:
    """Collection of profile data"""
    metrics: List[PerformanceMetric] = field(default_factory=list)
    config: Dict[str, Any] = field(default_factory=dict)
    start_time: float = 0.0
    end_time: float = 0.0
    
    def add_metric(self, metric: PerformanceMetric):
        self.metrics.append(metric)
    
    def get_average_spike_rate(self) -> float:
        if not self.metrics:
            return 0.0
        total_spikes = sum(m.total_spikes for m in self.metrics)
        total_time = self.end_time - self.start_time
        return total_spikes / total_time if total_time > 0 else 0.0
    
    def get_max_active_neurons(self) -> int:
        return max((m.neurons_active for m in self.metrics), default=0)
    
    def get_efficiency_score(self) -> float:
        """Calculate a performance efficiency score"""
        if not self.metrics:
            return 0.0
        
        # Higher spike rate and efficiency = better
        spike_rate = self.get_average_spike_rate()
        active_ratio = sum(m.neurons_active for m in self.metrics) / (len(self.metrics) * 1000)  # Normalized
        
        return min((spike_rate * active_ratio) / 100.0, 10.0)

class PerformanceProfiler:
    """Advanced performance profiler for NLM simulations"""
    
    def __init__(self, output_dir: str = "profiles"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.current_profile: Optional[ProfileData] = None
        self.monitoring = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.metric_queue: queue.Queue = queue.Queue()
        
    def start_profile(self, config: Dict[str, Any]) -> str:
        """Start a new performance profile"""
        self.current_profile = ProfileData(
            config=config,
            start_time=time.time()
        )
        self.monitoring = True
        
        # Start monitoring thread
        self.monitor_thread = threading.Thread(target=self._monitor_loop, daemon=True)
        self.monitor_thread.start()
        
        profile_id = f"profile_{datetime.now().strftime('%Y%m%d_%H%M%S')}"
        print(f"Starting performance profile: {profile_id}")
        return profile_id
    
    def stop_profile(self) -> Optional[ProfileData]:
        """Stop current profile and return data"""
        if not self.current_profile:
            return None
        
        self.monitoring = False
        self.end_time = time.time()
        
        # Wait for monitor thread to finish
        if self.monitor_thread:
            self.monitor_thread.join(timeout=5.0)
        
        # Process remaining metrics
        while not self.metric_queue.empty():
            metric = self.metric_queue.get()
            self.current_profile.add_metric(metric)
        
        print(f"Profile stopped. Collected {len(self.current_profile.metrics)} metrics")
        return self.current_profile
    
    def add_metric(self, metric: PerformanceMetric):
        """Add a metric to the queue"""
        if self.monitoring:
            self.metric_queue.put(metric)
    
    def _monitor_loop(self):
        """Background monitoring loop"""
        while self.monitoring:
            try:
                # Wait for 100ms between updates
                time.sleep(0.1)
                
                # This would normally get data from the simulation
                # For demo purposes, we'll generate synthetic data
                metric = PerformanceMetric(
                    timestamp=time.time(),
                    step=len(self.current_profile.metrics) if self.current_profile else 0,
                    neurons_active=500 + int(300 * math.sin(time.time() * 0.1)),
                    neurons_firing=50 + int(30 * math.sin(time.time() * 0.15)),
                    total_spikes=1000 + int(500 * math.sin(time.time() * 0.05)),
                    total_synapses=5000,
                    simulation_time=0.001,
                    cpu_usage=50.0 + 20.0 * math.sin(time.time() * 0.2),
                    memory_usage=30.0 + 20.0 * math.sin(time.time() * 0.3),
                    plasticity_updates=100,
                    visualization_fps=30.0
                )
                
                self.add_metric(metric)
                
            except Exception as e:
                print(f"Monitoring error: {e}")
                time.sleep(1.0)
    
    def analyze_performance(self, profile: ProfileData) -> Dict[str, Any]:
        """Analyze performance profile and return insights"""
        if not profile.metrics:
            return {}
        
        insights = {}
        
        # Basic statistics
        insights["total_steps"] = len(profile.metrics)
        insights["time_span"] = profile.end_time - profile.start_time
        insights["average_spike_rate"] = profile.get_average_spike_rate()
        insights["max_active_neurons"] = profile.get_max_active_neurons()
        insights["efficiency_score"] = profile.get_efficiency_score()
        
        # Activity patterns
        active_neurons = [m.neurons_active for m in profile.metrics]
        firing_neurons = [m.neurons_firing for m in profile.metrics]
        
        insights["active_neuron_range"] = {
            "min": min(active_neurons),
            "max": max(active_neurons),
            "average": sum(active_neurons) / len(active_neurons)
        }
        
        insights["firing_neuron_range"] = {
            "min": min(firing_neurons),
            "max": max(firing_neurons),
            "average": sum(firing_neurons) / len(firing_neurons)
        }
        
        # Performance bottlenecks
        cpu_usage = [m.cpu_usage for m in profile.metrics]
        memory_usage = [m.memory_usage for m in profile.metrics]
        
        insights["resource_utilization"] = {
            "max_cpu": max(cpu_usage),
            "avg_cpu": sum(cpu_usage) / len(cpu_usage),
            "max_memory": max(memory_usage),
            "avg_memory": sum(memory_usage) / len(memory_usage)
        }
        
        # Plasticity analysis
        plasticity_updates = [m.plasticity_updates for m in profile.metrics]
        insights["plasticity_analysis"] = {
            "avg_updates_per_step": sum(plasticity_updates) / len(plasticity_updates),
            "max_updates": max(plasticity_updates)
        }
        
        return insights
    
    def export_profile(self, profile: ProfileData, format: str = "json"):
        """Export profile data to file"""
        timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
        
        if format == "json":
            filename = self.output_dir / f"profile_{timestamp}.json"
            with open(filename, 'w') as f:
                data = {
                    "config": profile.config,
                    "start_time": profile.start_time,
                    "end_time": profile.end_time,
                    "metrics": [
                        {
                            "timestamp": m.timestamp,
                            "step": m.step,
                            "neurons_active": m.neurons_active,
                            "neurons_firing": m.neurons_firing,
                            "total_spikes": m.total_spikes,
                            "total_synapses": m.total_synapses,
                            "simulation_time": m.simulation_time,
                            "cpu_usage": m.cpu_usage,
                            "memory_usage": m.memory_usage,
                            "plasticity_updates": m.plasticity_updates,
                            "visualization_fps": m.visualization_fps
                        }
                        for m in profile.metrics
                    ]
                }
                json.dump(data, f, indent=2)
        
        elif format == "csv":
            filename = self.output_dir / f"profile_{timestamp}.csv"
            with open(filename, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow([
                    'timestamp', 'step', 'neurons_active', 'neurons_firing',
                    'total_spikes', 'total_synapses', 'simulation_time',
                    'cpu_usage', 'memory_usage', 'plasticity_updates',
                    'visualization_fps'
                ])
                
                for metric in profile.metrics:
                    writer.writerow([
                        metric.timestamp, metric.step, metric.neurons_active,
                        metric.neurons_firing, metric.total_spikes, metric.total_synapses,
                        metric.simulation_time, metric.cpu_usage, metric.memory_usage,
                        metric.plasticity_updates, metric.visualization_fps
                    ])
        
        print(f"Profile exported to: {filename}")
        return filename
    
    def generate_report(self, profile: ProfileData) -> str:
        """Generate a detailed performance report"""
        insights = self.analyze_performance(profile)
        
        report = f"""
# NLM Performance Report
Generated: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}

## Summary
- Total Simulation Steps: {insights.get('total_steps', 0)}
- Time Span: {insights.get('time_span', 0):.2f} seconds
- Average Spike Rate: {insights.get('average_spike_rate', 0):.2f} spikes/second
- Max Active Neurons: {insights.get('max_active_neurons', 0)}
- Efficiency Score: {insights.get('efficiency_score', 0):.2f}

## Neuron Activity
- Active Neurons Range: {insights.get('active_neuron_range', {})}
- Firing Neurons Range: {insights.get('firing_neuron_range', {})}

## Resource Utilization
- CPU Usage: {insights.get('resource_utilization', {}).get('avg_cpu', 0):.1f}% average, {insights.get('resource_utilization', {}).get('max_cpu', 0):.1f}% max
- Memory Usage: {insights.get('resource_utilization', {}).get('avg_memory', 0):.1f}% average, {insights.get('resource_utilization', {}).get('max_memory', 0):.1f}% max

## Plasticity Analysis
- Average Updates Per Step: {insights.get('plasticity_analysis', {}).get('avg_updates_per_step', 0):.1f}
- Maximum Updates Per Step: {insights.get('plasticity_analysis', {}).get('max_updates', 0)}

## Performance Assessment
"""
        
        score = insights.get('efficiency_score', 0)
        if score > 5.0:
            assessment = "Excellent - High efficiency and activity levels"
        elif score > 2.0:
            assessment = "Good - Decent performance with room for improvement"
        elif score > 1.0:
            assessment = "Average - Moderate performance"
        else:
            assessment = "Needs Improvement - Low activity and efficiency"
        
        report += f"\n{assessment}\n"
        
        return report

def main():
    """Command-line interface"""
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Performance Profiler")
    parser.add_argument("--start", action="store_true", help="Start profiling")
    parser.add_argument("--stop", action="store_true", help="Stop profiling")
    parser.add_argument("--export", choices=["json", "csv"], help="Export format")
    parser.add_argument("--report", action="store_true", help="Generate report")
    parser.add_argument("--output-dir", default="profiles", help="Output directory")
    parser.add_argument("--config", help="Configuration file path")
    
    args = parser.parse_args()
    
    if args.start:
        profiler = PerformanceProfiler(args.output_dir)
        config = {}
        if args.config:
            with open(args.config, 'r') as f:
                config = json.load(f)
        
        profile_id = profiler.start_profile(config)
        
        try:
            # Simulate running for 10 seconds
            time.sleep(10)
            
            if args.stop:
                profile = profiler.stop_profile()
                if profile:
                    if args.export:
                        profiler.export_profile(profile, args.export)
                    if args.report:
                        report = profiler.generate_report(profile)
                        print(report)
        except KeyboardInterrupt:
            print("\nInterrupted by user")
            profiler.stop_profile()
    
    elif args.stop:
        print("Use --start to begin profiling first")
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
