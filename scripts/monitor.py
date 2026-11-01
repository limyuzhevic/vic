#!/usr/bin/env python3
"""
NLM Performance Monitor
Real-time performance monitoring and profiling for NLM simulations
"""

import sys
import os
import time
import threading
import statistics
from typing import Dict, List, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import json

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

import pynlm

class MetricType(Enum):
    COUNTER = "counter"
    GAUGE = "gauge"
    HISTOGRAM = "histogram"
    TIMER = "timer"

@dataclass
class Metric:
    name: str
    type: MetricType
    value: float = 0.0
    count: int = 1
    min_value: float = float('inf')
    max_value: float = 0.0
    timestamp: float = 0.0

@dataclass
class PerformanceProfile:
    name: str
    start_time: float
    end_time: float
    metrics: Dict[str, Metric] = field(default_factory=dict)
    brain_snapshots: List[Dict] = field(default_factory=list)
    thread_id: int = 0

class PerformanceMonitor:
    """Real-time performance monitor for NLM simulations"""
    
    def __init__(self, name: str = "NLM_Monitor"):
        self.name = name
        self.is_running = False
        self.current_profile: Optional[PerformanceProfile] = None
        self.profiles: List[PerformanceProfile] = []
        self.monitor_thread: Optional[threading.Thread] = None
        self.collection_interval = 0.1  # seconds
        
        # Core metrics
        self.metrics: Dict[str, Metric] = {
            'simulation_steps': Metric('simulation_steps', MetricType.COUNTER),
            'total_spikes': Metric('total_spikes', MetricType.COUNTER),
            'active_neurons': Metric('active_neurons', MetricType.GAUGE),
            'firing_rate': Metric('firing_rate', MetricType.GAUGE),
            'simulation_time': Metric('simulation_time', MetricType.TIMER),
            'neuron_count': Metric('neuron_count', MetricType.GAUGE),
            'synapse_count': Metric('synapse_count', MetricType.GAUGE),
            'memory_usage': Metric('memory_usage', MetricType.GAUGE),
        }
    
    def start_profile(self, profile_name: str) -> PerformanceProfile:
        """Start a new performance profile"""
        if self.current_profile:
            self.stop_profile()
        
        self.current_profile = PerformanceProfile(
            name=profile_name,
            start_time=time.time()
        )
        self.is_running = True
        
        # Start monitoring thread
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        return self.current_profile
    
    def stop_profile(self) -> Optional[PerformanceProfile]:
        """Stop current performance profile"""
        if not self.current_profile:
            return None
        
        self.is_running = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
        
        self.current_profile.end_time = time.time()
        
        # Move to completed profiles
        self.profiles.append(self.current_profile)
        current = self.current_profile
        self.current_profile = None
        
        return current
    
    def record_metric(self, name: str, value: float, metric_type: Optional[MetricType] = None):
        """Record a metric value"""
        if name not in self.metrics:
            if metric_type is None:
                metric_type = MetricType.GAUGE
            self.metrics[name] = Metric(name, metric_type)
        
        metric = self.metrics[name]
        metric.value = value
        metric.timestamp = time.time()
        
        if metric.type == MetricType.COUNTER:
            metric.value += value
            metric.count += 1
        elif metric.type == MetricType.GAUGE:
            metric.value = value
        elif metric.type == MetricType.HISTOGRAM:
            metric.min_value = min(metric.min_value, value)
            metric.max_value = max(metric.max_value, value)
        
        if self.current_profile:
            self.current_profile.metrics[name] = metric
    
    def record_brain_snapshot(self, brain: pynlm.Brain, step: int):
        """Record a snapshot of brain state"""
        if not self.current_profile:
            return
        
        snapshot = {
            'step': step,
            'timestamp': time.time(),
            'total_spikes': brain.getTotalSpikeCount(),
            'active_neurons': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'development_stage': brain.getDevelopmentalStage(),
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
        }
        
        self.current_profile.brain_snapshots.append(snapshot)
    
    def _monitor_loop(self):
        """Background monitoring loop"""
        while self.is_running:
            # Record core metrics if we have a brain to monitor
            # This would be populated by the actual simulation
            time.sleep(self.collection_interval)

def benchmark_config(config_name: str, brain_configs: List[pynlm.Config], 
                    simulation_steps: List[int], iterations: int = 5) -> Dict:
    """Benchmark multiple brain configurations"""
    print(f"Benchmarking configuration: {config_name}")
    
    results = {}
    
    for config in brain_configs:
        config_name = getattr(config, '_config_name', 'Unknown')
        print(f"  Testing {config_name}...")
        
        config_times = {}
        config_spikes = {}
        
        for step_count in simulation_steps:
            times = []
            spikes = []
            
            for _ in range(iterations):
                brain = pynlm.createBrain(config)
                brain.initialize()
                
                start_time = time.perf_counter()
                for step in range(step_count):
                    brain.step(step)
                elapsed = time.perf_counter() - start_time
                
                times.append(elapsed)
                spikes.append(brain.getTotalSpikeCount())
            
            config_times[step_count] = {
                'mean_time': statistics.mean(times),
                'median_time': statistics.median(times),
                'min_time': min(times),
                'max_time': max(times),
                'std_dev': statistics.stdev(times) if len(times) > 1 else 0.0,
                'steps_per_second': step_count / statistics.mean(times)
            }
            
            config_spikes[step_count] = {
                'mean_spikes': statistics.mean(spikes),
                'median_spikes': statistics.median(spikes),
                'min_spikes': min(spikes),
                'max_spikes': max(spikes),
                'spikes_per_second': statistics.mean(spikes) / statistics.mean(times)
            }
        
        results[config_name] = {
            'time_metrics': config_times,
            'spike_metrics': config_spikes
        }
    
    return results

def monitor_simulation(brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld] = None,
                       agent: Optional[pynlm.AgentBrain] = None,
                       duration: int = 100, monitor_interval: float = 0.5) -> PerformanceProfile:
    """Monitor a complete simulation with detailed metrics"""
    print(f"Monitoring simulation for {duration} steps...")
    
    monitor = PerformanceMonitor("Simulation_Monitor")
    profile = monitor.start_profile("Simulation")
    
    try:
        # Initialize monitoring metrics
        monitor.record_metric('simulation_time', 0.0, MetricType.TIMER)
        monitor.record_metric('steps_completed', 0, MetricType.COUNTER)
        
        start_wall_time = time.time()
        
        for step in range(duration):
            step_start = time.perf_counter()
            
            # Record current state
            monitor.record_brain_snapshot(brain, step)
            monitor.record_metric('total_spikes', brain.getTotalSpikeCount())
            monitor.record_metric('active_neurons', brain.getFiringNeuronCount())
            monitor.record_metric('firing_rate', brain.getAverageFiringRate())
            
            # Execute simulation step
            if world and agent:
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
            
            brain.step(step)
            
            if agent:
                action = agent.decodeMotorCommand()
                if world:
                    result = world.applyMotorCommand(action, step * 0.1)
                    monitor.record_metric('total_reward', result.reward)
                
                agent.updateDevelopment(0.1)
            
            # Calculate step time
            step_time = time.perf_counter() - step_start
            monitor.record_metric('step_time', step_time, MetricType.TIMER)
            monitor.record_metric('steps_completed', 1, MetricType.COUNTER)
            
            # Progress reporting
            if step % 20 == 0:
                elapsed = time.time() - start_wall_time
                print(f"  Step {step}/{duration} ({elapsed:.1f}s)")
            
            # Wait for monitor interval
            time.sleep(monitor_interval)
        
        # Calculate final metrics
        total_time = time.time() - start_wall_time
        monitor.record_metric('total_simulation_time', total_time, MetricType.TIMER)
        monitor.record_metric('performance_steps_per_second', duration / total_time, MetricType.GAUGE)
        
    finally:
        profile = monitor.stop_profile()
    
    return profile

def analyze_profile(profile: PerformanceProfile) -> Dict:
    """Analyze and summarize a performance profile"""
    analysis = {
        'profile_name': profile.name,
        'duration_seconds': profile.end_time - profile.start_time,
        'steps_per_second': len(profile.brain_snapshots) / (profile.end_time - profile.start_time) if profile.end_time > profile.start_time else 0,
        'total_spikes': profile.brain_snapshots[-1]['total_spikes'] if profile.brain_snapshots else 0,
        'peak_active_neurons': max(s['active_neurons'] for s in profile.brain_snapshots) if profile.brain_snapshots else 0,
        'average_firing_rate': statistics.mean(s['average_firing_rate'] for s in profile.brain_snapshots) if profile.brain_snapshots else 0,
        'development_stage_changes': sum(1 for s in profile.brain_snapshots if s['development_stage'] != profile.brain_snapshots[0]['development_stage']) if len(profile.brain_snapshots) > 1 else 0,
        'spike_rate_variation': calculate_variation(profile.brain_snapshots, 'total_spikes') if profile.brain_snapshots else 0,
        'stability_score': calculate_stability(profile.brain_snapshots) if profile.brain_snapshots else 0,
    }
    
    return analysis

def calculate_variation(data: List[Dict], key: str) -> float:
    """Calculate variation coefficient for data series"""
    if not data:
        return 0.0
    
    values = [d[key] for d in data]
    mean = statistics.mean(values)
    std_dev = statistics.stdev(values) if len(values) > 1 else 0.0
    
    return (std_dev / mean) * 100 if mean > 0 else 0.0

def calculate_stability(data: List[Dict]) -> float:
    """Calculate stability score based on firing rate consistency"""
    if not data:
        return 0.0
    
    firing_rates = [d['average_firing_rate'] for d in data]
    mean_rate = statistics.mean(firing_rates)
    
    # Calculate coefficient of variation
    if len(firing_rates) > 1:
        std_dev = statistics.stdev(firing_rates)
        cv = std_dev / mean_rate if mean_rate > 0 else 0.0
        # Convert to stability score (0-100, lower CV = higher stability)
        stability = max(0.0, 100.0 * (1.0 - cv))
        return stability
    else:
        return 100.0 if mean_rate > 0 else 0.0

def generate_report(profile: PerformanceProfile, analysis: Dict) -> str:
    """Generate a comprehensive performance report"""
    report = f"""
=== NLM Performance Analysis Report ===

Profile: {profile.name}
Duration: {analysis['duration_seconds']:.2f} seconds

Simulation Performance:
  Steps per second: {analysis['steps_per_second']:.1f}
  Total simulation time: {analysis['duration_seconds']:.2f}s

Neural Activity:
  Total spikes generated: {analysis['total_spikes']:,}
  Peak active neurons: {analysis['peak_active_neurons']}
  Average firing rate: {analysis['average_firing_rate']:.2f} Hz

Stability Analysis:
  Development stage changes: {analysis['development_stage_changes']}
  Spike rate variation: {analysis['spike_rate_variation']:.1f}%
  Stability score: {analysis['stability_score']:.1f}/100

Brain State Evolution:
"""
    
    if profile.brain_snapshots:
        report += f"  Initial state - Spikes: {profile.brain_snapshots[0]['total_spikes']}, "
        report += f"Firing rate: {profile.brain_snapshots[0]['average_firing_rate']:.2f} Hz\n"
        
        report += f"  Final state - Spikes: {profile.brain_snapshots[-1]['total_spikes']}, "
        report += f"Firing rate: {profile.brain_snapshots[-1]['average_firing_rate']:.2f} Hz\n"
        
        if len(profile.brain_snapshots) > 1:
            growth = (profile.brain_snapshots[-1]['total_spikes'] - profile.brain_snapshots[0]['total_spikes']) / profile.brain_snapshots[0]['total_spikes'] * 100 if profile.brain_snapshots[0]['total_spikes'] > 0 else 0
            report += f"  Spike growth: {growth:+.1f}%\n"
    
    report += f"\nConclusion: "
    if analysis['stability_score'] > 80:
        report += "The simulation shows high stability with consistent neural activity."
    elif analysis['stability_score'] > 60:
        report += "The simulation shows moderate stability with some fluctuations."
    else:
        report += "The simulation shows low stability with significant fluctuations."
    
    report += f"\n\n=== End of Report ==="
    
    return report

def main():
    import argparse
    
    parser = argparse.ArgumentParser(description="NLM Performance Monitor and Profiler")
    parser.add_argument("--benchmark-configs", action="store_true", help="Benchmark different configurations")
    parser.add_argument("--monitor-simulation", action="store_true", help="Monitor a complete simulation")
    parser.add_argument("--analyze-file", type=str, help="Analyze a saved performance profile from file")
    parser.add_argument("--output-format", choices=['text', 'json', 'csv'], default='text', help="Output format")
    parser.add_argument("--sim-steps", type=int, default=100, help="Simulation steps for monitoring")
    parser.add_argument("--monitor-interval", type=float, default=0.5, help="Monitoring interval in seconds")
    
    args = parser.parse_args()
    
    if args.benchmark_configs:
        # Benchmark different configurations
        print("=== NLM Configuration Benchmark ===")
        
        configs = [
            ("Default", pynlm.createDefaultConfig()),
            ("Performance", pynlm.createPerformanceConfig()),
            ("Research", pynlm.createResearchConfig()),
        ]
        
        steps_to_test = [50, 100, 500, 1000]
        
        all_results = {}
        for name, config in configs:
            all_results[name] = benchmark_config(name, [config], steps_to_test)
        
        # Generate summary
        print("\n=== Benchmark Summary ===")
        for config_name, results in all_results.items():
            print(f"\n{config_name} Configuration:")
            for step_count, time_metrics in results.items()['time_metrics'].items():
                print(f"  {step_count} steps: {time_metrics['steps_per_second']:.1f} steps/sec")
        
        # Save results
        if args.output_format == 'json':
            with open('benchmark_results.json', 'w') as f:
                json.dump(all_results, f, indent=2)
            print("\nResults saved to 'benchmark_results.json'")
    
    elif args.monitor_simulation:
        # Monitor a simulation
        print("=== NLM Simulation Monitor ===")
        
        # Create brain and world
        brain = pynlm.createBrain(pynlm.createDefaultConfig())
        brain.initialize()
        
        world = pynlm.createSimpleWorld()
        world.configure(10, 10, 8, 8)
        world.reset()
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation with monitoring
        profile = monitor_simulation(brain, world, agent, 
                                    duration=args.sim_steps, 
                                    monitor_interval=args.monitor_interval)
        
        # Analyze results
        analysis = analyze_profile(profile)
        
        # Generate report
        report = generate_report(profile, analysis)
        print(report)
        
        # Save if requested
        if args.output_format == 'json':
            with open('simulation_profile.json', 'w') as f:
                json.dump({
                    'profile': {
                        'name': profile.name,
                        'start_time': profile.start_time,
                        'end_time': profile.end_time,
                    },
                    'metrics': {name: {
                        'type': metric.type.value,
                        'value': metric.value,
                        'timestamp': metric.timestamp
                    } for name, metric in profile.metrics.items()},
                    'snapshots': profile.brain_snapshots
                }, f, indent=2)
            print("\nProfile saved to 'simulation_profile.json'")
    
    elif args.analyze_file:
        # Analyze a saved profile
        print(f"=== Analyzing Profile from {args.analyze_file} ===")
        
        try:
            with open(args.analyze_file, 'r') as f:
                data = json.load(f)
            
            # Convert back to PerformanceProfile format
            profile = PerformanceProfile(
                name=data.get('profile', {}).get('name', 'Unknown'),
                start_time=data.get('profile', {}).get('start_time', 0),
                end_time=data.get('profile', {}).get('end_time', 0),
            )
            
            # Restore metrics and snapshots
            for name, metric_data in data.get('metrics', {}).items():
                profile.metrics[name] = Metric(
                    name=name,
                    type=MetricType(metric_data['type']),
                    value=metric_data['value'],
                    timestamp=metric_data['timestamp']
                )
            
            profile.brain_snapshots = data.get('snapshots', [])
            
            # Analyze and report
            analysis = analyze_profile(profile)
            report = generate_report(profile, analysis)
            
            print(report)
            
        except Exception as e:
            print(f"Error analyzing profile: {e}")
    
    else:
        print("=== NLM Performance Monitor ===")
        print("\nAvailable commands:")
        print("  --benchmark-configs    Benchmark different brain configurations")
        print("  --monitor-simulation    Monitor a complete simulation")
        print("  --analyze-file FILE     Analyze a saved performance profile")
        print("  --output-format        Output format (text, json, csv)")
        print("  --sim-steps N          Number of simulation steps (default: 100)")
        print("  --monitor-interval S   Monitoring interval in seconds (default: 0.5)")
        
        print("\nExample usage:")
        print("  python monitor.py --benchmark-configs")
        print("  python monitor.py --monitor-simulation")
        print("  python monitor.py --analyze-file simulation_profile.json")

if __name__ == "__main__":
    main()
