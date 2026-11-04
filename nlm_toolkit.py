#!/usr/bin/env python3
"""
NLM Performance Optimization and Profiling Tools
===============================================

Performance monitoring, profiling, and optimization tools for NLM simulations.
"""

import pynlm
import time
import cProfile
import pstats
import io
import json
import tempfile
import os
import sys
from pathlib import Path
from typing import Dict, List, Any, Optional, Callable
from dataclasses import dataclass, field
from enum import Enum
import threading
import queue

class PerformanceMetric(Enum):
    """Performance metrics"""
    NEURON_COUNT = "neuron_count"
    SYNAPSE_COUNT = "synapse_count"
    FIRING_RATE = "firing_rate"
    SIMULATION_TIME = "simulation_time"
    MEMORY_USAGE = "memory_usage"
    CPU_USAGE = "cpu_usage"
    INTERACTION_TIME = "interaction_time"
    LEARNING_RATE = "learning_rate"

@dataclass
class PerformanceSample:
    """Single performance sample"""
    timestamp: float
    step: int
    metric_values: Dict[PerformanceMetric, float] = field(default_factory=dict)
    additional_data: Dict[str, Any] = field(default_factory=dict)

class PerformanceMonitor:
    """Performance monitoring and profiling for NLM simulations"""
    
    def __init__(self, sampling_interval: float = 0.1):
        self.sampling_interval = sampling_interval
        self.samples: List[PerformanceSample] = []
        self.is_monitoring = False
        self.monitor_thread: Optional[threading.Thread] = None
        self.data_queue = queue.Queue()
    
    def start_monitoring(self):
        """Start performance monitoring"""
        if self.is_monitoring:
            return
        
        self.is_monitoring = True
        self.monitor_thread = threading.Thread(target=self._monitor_loop)
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
    
    def stop_monitoring(self):
        """Stop performance monitoring"""
        self.is_monitoring = False
        if self.monitor_thread:
            self.monitor_thread.join(timeout=1.0)
            self.monitor_thread = None
    
    def _monitor_loop(self):
        """Monitoring loop"""
        while self.is_monitoring:
            time.sleep(self.sampling_interval)
            sample = self._collect_sample()
            if sample:
                self.samples.append(sample)
    
    def _collect_sample(self) -> Optional[PerformanceSample]:
        """Collect a performance sample"""
        try:
            sample = PerformanceSample(
                timestamp=time.time(),
                step=len(self.samples)
            )
            
            # Collect basic metrics
            sample.metric_values[PerformanceMetric.INTERACTION_TIME] = time.time()
            
            return sample
        except Exception:
            return None
    
    def add_custom_metric(self, step: int, metric_name: str, value: float):
        """Add custom metric to current sample"""
        # This would need to be integrated with the actual monitoring
        pass
    
    def get_statistics(self, metric: PerformanceMetric) -> Dict[str, float]:
        """Get statistics for a specific metric"""
        if not self.samples:
            return {}
        
        values = []
        for sample in self.samples:
            if metric in sample.metric_values:
                values.append(sample.metric_values[metric])
        
        if not values:
            return {}
        
        return {
            'min': min(values),
            'max': max(values),
            'mean': sum(values) / len(values),
            'median': sorted(values)[len(values) // 2],
            'count': len(values)
        }
    
    def save_profile(self, filename: str):
        """Save performance profile to file"""
        profile_data = {
            'samples': [],
            'statistics': {}
        }
        
        for sample in self.samples:
            profile_data['samples'].append({
                'timestamp': sample.timestamp,
                'step': sample.step,
                'metric_values': {m.value: v for m, v in sample.metric_values.items()},
                'additional_data': sample.additional_data
            })
        
        # Calculate statistics
        for metric in PerformanceMetric:
            stats = self.get_statistics(metric)
            if stats:
                profile_data['statistics'][metric.value] = stats
        
        with open(filename, 'w') as f:
            json.dump(profile_data, f, indent=2)
        
        print(f"Performance profile saved to {filename}")
class PerformanceProfiler:
    """Advanced profiling with cProfile"""
    
    def __init__(self):
        self.profile_data = {}
        self.current_session = None
    
    def start_profile(self, session_name: str):
        """Start profiling a specific session"""
        self.current_session = session_name
        self.profile_data[session_name] = {
            'start_time': time.time(),
            'profile': cProfile.Profile()
        }
        
        self.profile_data[session_name]['profile'].enable()
    
    def stop_profile(self, session_name: str):
        """Stop profiling a session"""
        if session_name not in self.profile_data:
            return
        
        session = self.profile_data[session_name]
        session['profile'].disable()
        session['end_time'] = time.time()
        
        # Generate statistics
        s = io.StringIO()
        ps = pstats.Stats(session['profile'], stream=s).sort_stats('cumulative')
        ps.print_stats()
        
        session['stats'] = s.getvalue()
        session['duration'] = session['end_time'] - session['start_time']
    
    def get_profile_summary(self, session_name: str) -> Dict[str, Any]:
        """Get profile summary for a session"""
        if session_name not in self.profile_data:
            return {}
        
        session = self.profile_data[session_name]
        
        # Parse top 20 functions
        s = io.StringIO()
        ps = pstats.Stats(session['profile'], stream=s).sort_stats('cumulative')
        ps.print_stats(20)
        
        return {
            'session_name': session_name,
            'duration': session['duration'],
            'top_functions': s.getvalue(),
            'total_calls': ps.total_calls,
            'total_time': ps.total_tt
        }
    
    def save_all_profiles(self, directory: str):
        """Save all profiles to directory"""
        os.makedirs(directory, exist_ok=True)
        
        for session_name, session_data in self.profile_data.items():
            filename = os.path.join(directory, f"{session_name}.json")
            
            with open(filename, 'w') as f:
                json.dump(session_data, f, indent=2)
            
            print(f"Profile saved: {filename}")
class SimulationOptimizer:
    """Optimization utilities for NLM simulations"""
    
    def __init__(self):
        self.optimization_history = []
    
    def optimize_brain_configuration(self, config_template: pynlm.Config, 
                                      performance_target: float) -> pynlm.Config:
        """Optimize brain configuration for performance target"""
        print(f"Optimizing brain configuration for {performance_target} performance target...")
        
        # Try different configurations
        configurations = [
            {'neuron_count': 500, 'region_count': 1, 'synapse_density': 0.05},
            {'neuron_count': 1000, 'region_count': 2, 'synapse_density': 0.1},
            {'neuron_count': 2000, 'region_count': 3, 'synapse_density': 0.15},
            {'neuron_count': 3000, 'region_count': 4, 'synapse_density': 0.2},
        ]
        
        best_config = config_template
        best_performance = 0.0
        
        for params in configurations:
            # Create configuration
            config = pynlm.createConfigWithSettings(
                neuron_count=params['neuron_count'],
                region_count=params['region_count'],
                synapse_density=params['synapse_density']
            )
            
            # Test performance
            performance = self._evaluate_configuration(config, performance_target)
            
            if performance > best_performance:
                best_performance = performance
                best_config = config
            
            self.optimization_history.append({
                'parameters': params,
                'performance': performance,
                'target_met': performance >= performance_target
            })
        
        print(f"Best configuration: {best_performance:.2f} performance")
        return best_config
    
    def _evaluate_configuration(self, config: pynlm.Config, target: float) -> float:
        """Evaluate configuration performance"""
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(50):
            brain.step(step)
        end_time = time.time()
        
        # Calculate performance metric
        neurons = brain.getTotalNeuronCount()
        spikes = brain.getTotalSpikeCount()
        duration = end_time - start_time
        
        # Performance = spikes per second per neuron (normalized)
        performance = (spikes / duration) / (neurons / 1000.0) if neurons > 0 else 0
        
        return performance
    
    def optimize_world_parameters(self, config: pynlm.Config,
                                  base_world_config: Dict) -> Dict:
        """Optimize world parameters for agent performance"""
        print("Optimizing world parameters...")
        
        # Try different world configurations
        world_configs = [
            {'width': 10, 'height': 10, 'vision_width': 4, 'vision_height': 4},
            {'width': 20, 'height': 20, 'vision_width': 8, 'vision_height': 8},
            {'width': 30, 'height': 30, 'vision_width': 12, 'vision_height': 12},
            {'width': 40, 'height': 40, 'vision_width': 16, 'vision_height': 16},
        ]
        
        best_config = base_world_config
        best_performance = 0.0
        
        for params in world_configs:
            # Create and test
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(**params)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Run simulation
            for step in range(30):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
            
            # Calculate performance
            performance = agent.getCuriosityLevel() * brain.getAverageFiringRate()
            
            if performance > best_performance:
                best_performance = performance
                best_config = params
            
            self.optimization_history.append({
                'world_params': params,
                'performance': performance,
                'curiosity': agent.getCuriosityLevel(),
                'firing_rate': brain.getAverageFiringRate()
            })
        
        print(f"Best world config: {best_performance:.2f} performance")
        return best_config
    
    def get_optimization_history(self) -> List[Dict[str, Any]]:
        """Get optimization history"""
        return self.optimization_history
    
    def plot_optimization_results(self):
        """Plot optimization results (placeholder)"""
        print("Optimization results would be plotted here")
        print("Consider using matplotlib for detailed visualization")
class MemoryProfiler:
    """Memory profiling for NLM simulations"""
    
    def __init__(self):
        self.memory_samples = []
        try:
            import psutil
            self.psutil = psutil
            self.has_psutil = True
        except ImportError:
            self.psutil = None
            self.has_psutil = False
    
    def start_memory_profiling(self, interval: float = 1.0):
        """Start memory profiling"""
        if not self.has_psutil:
            print("Warning: psutil not installed. Memory profiling disabled.")
            return
        
        self.is_profiling = True
        self.memory_thread = threading.Thread(target=self._memory_monitor)
        self.memory_thread.daemon = True
        self.memory_thread.start()
    
    def _memory_monitor(self):
        """Memory monitoring loop"""
        while hasattr(self, 'is_profiling') and self.is_profiling:
            sample = self._collect_memory_sample()
            if sample:
                self.memory_samples.append(sample)
            time.sleep(interval)
    
    def _collect_memory_sample(self) -> Optional[Dict[str, float]]:
        """Collect memory sample"""
        try:
            import psutil
            process = psutil.Process()
            
            return {
                'timestamp': time.time(),
                'memory_rss': process.memory_info().rss / 1024 / 1024,  # MB
                'memory_vms': process.memory_info().vms / 1024 / 1024,  # MB
                'cpu_percent': process.cpu_percent(),
                'num_threads': process.num_threads()
            }
        except Exception:
            return None
    
    def stop_memory_profiling(self):
        """Stop memory profiling"""
        self.is_profiling = False
        if hasattr(self, 'memory_thread'):
            self.memory_thread.join(timeout=1.0)
    
    def get_memory_statistics(self) -> Dict[str, float]:
        """Get memory statistics"""
        if not self.memory_samples:
            return {}
        
        rss_values = [sample['memory_rss'] for sample in self.memory_samples]
        vms_values = [sample['memory_vms'] for sample in self.memory_samples]
        cpu_values = [sample['cpu_percent'] for sample in self.memory_samples]
        
        return {
            'memory_rss': {
                'min': min(rss_values),
                'max': max(rss_values),
                'mean': sum(rss_values) / len(rss_values),
                'count': len(rss_values)
            },
            'memory_vms': {
                'min': min(vms_values),
                'max': max(vms_values),
                'mean': sum(vms_values) / len(vms_values),
                'count': len(vms_values)
            },
            'cpu_percent': {
                'min': min(cpu_values),
                'max': max(cpu_values),
                'mean': sum(cpu_values) / len(cpu_values),
                'count': len(cpu_values)
            }
        }
    
    def save_memory_profile(self, filename: str):
        """Save memory profile to file"""
        profile_data = {
            'samples': self.memory_samples,
            'statistics': self.get_memory_statistics()
        }
        
        with open(filename, 'w') as f:
            json.dump(profile_data, f, indent=2)
        
        print(f"Memory profile saved to {filename}")
class NLPAnalyzer:
    """Comprehensive NLM analysis and reporting"""
    
    def __init__(self):
        self.analyzer_config = {
            'include_neural_metrics': True,
            'include_learning_metrics': True,
            'include_behavioral_metrics': True,
            'include_performance_metrics': True
        }
    
    def analyze_brain(self, brain: pynlm.Brain, simulation_steps: int = 100) -> Dict[str, Any]:
        """Analyze brain performance and characteristics"""
        print(f"Analyzing brain with {simulation_steps} simulation steps...")
        
        # Run simulation if not already done
        for step in range(simulation_steps):
            brain.step(step)
        
        analysis = {
            'neural_metrics': {},
            'learning_metrics': {},
            'behavioral_metrics': {}
        }
        
        if self.analyzer_config['include_neural_metrics']:
            analysis['neural_metrics'] = self._analyze_neural_metrics(brain)
        
        if self.analyzer_config['include_learning_metrics']:
            analysis['learning_metrics'] = self._analyze_learning_metrics(brain)
        
        if self.analyzer_config['include_behavioral_metrics']:
            analysis['behavioral_metrics'] = self._analyze_behavioral_metrics(brain)
        
        return analysis
    
    def _analyze_neural_metrics(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Analyze neural metrics"""
        return {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitement_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'regional_distribution': self._get_regional_distribution(brain)
        }
    
    def _get_regional_distribution(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Get neuron distribution across regions"""
        distribution = {}
        for region_id in brain.getRegionIds():
            region = brain.getRegion(region_id)
            if region:
                # Count neurons in this region
                neuron_count = 0
                for pop in region.getPopulations():
                    neuron_count += pop.getNeurons().size()
                distribution[f"region_{region_id.getValue()}"] = neuron_count
        
        return distribution
    
    def _analyze_learning_metrics(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Analyze learning metrics (if available)"""
        return {
            'developmental_stage': brain.getDevelopmentalStage(),
            'plasticity_modifier': 1.0,  # Placeholder
            'learning_capacity': 'estimated'
        }
    
    def _analyze_behavioral_metrics(self, brain: pynlm.Brain) -> Dict[str, Any]:
        """Analyze behavioral metrics"""
        return {
            'activity_level': brain.getAverageFiringRate(),
            'stability': 'estimated',  # Would need more analysis
            'complexity': 'estimated'
        }
    
    def generate_report(self, analysis: Dict[str, Any], output_format: str = 'text') -> str:
        """Generate comprehensive analysis report"""
        if output_format == 'text':
            return self._generate_text_report(analysis)
        elif output_format == 'json':
            return json.dumps(analysis, indent=2)
        elif output_format == 'html':
            return self._generate_html_report(analysis)
        else:
            raise ValueError(f"Unsupported output format: {output_format}")
    
    def _generate_text_report(self, analysis: Dict[str, Any]) -> str:
        """Generate text report"""
        report = "NLM Brain Analysis Report\n"
        report += "=" * 40 + "\n\n"
        
        neural = analysis.get('neural_metrics', {})
        report += "Neural Metrics:\n"
        for key, value in neural.items():
            if key != 'regional_distribution':
                report += f"  {key}: {value}\n"
        
        regions = neural.get('regional_distribution', {})
        if regions:
            report += "  Regional Distribution:\n"
            for region, count in regions.items():
                report += f"    {region}: {count} neurons\n"
        
        return report
    
    def _generate_html_report(self, analysis: Dict[str, Any]) -> str:
        """Generate HTML report"""
        html = """
        <html>
        <head><title>NLM Brain Analysis Report</title></head>
        <body>
            <h1>NLM Brain Analysis Report</h1>
            <pre>
"""
        html += self._generate_text_report(analysis)
        html += """
            </pre>
        </body>
        </html>
        """
        return html
class SimulationProfiler:
    """Advanced simulation profiling"""
    
    def __init__(self):
        self.profiler = PerformanceProfiler()
        self.monitor = PerformanceMonitor()
        self.analyzer = NLPAnalyzer()
    
    def profile_complete_simulation(self, world_config: Dict, 
                                    agent_config: Dict, steps: int) -> Dict[str, Any]:
        """Profile a complete simulation"""
        print(f"Starting complete simulation profiling...")
        
        # Start monitoring
        self.monitor.start_monitoring()
        
        # Start profiling
        self.profiler.start_profile("complete_simulation")
        
        # Run simulation
        start_time = time.time()
        
        # Create configuration
        config = pynlm.createBrain(pynlm.createConfigWithSettings(**agent_config))
        config.initialize()
        
        # Create world
        world = pynlm.createSimpleWorld()
        world.configure(**world_config)
        world.reset()
        
        # Create agent
        agent = pynlm.createAgentBrain(config)
        agent.initialize(world)
        
        # Enable learning if requested
        if agent_config.get('enable_learning', False):
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            agent.enableDevelopment(True)
        
        # Run simulation steps
        for step in range(steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            config.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            reward = percept.getInternal()[0] if percept.getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            agent.updateDevelopment(0.1)
        
        end_time = time.time()
        
        # Stop profiling
        self.profiler.stop_profile("complete_simulation")
        self.monitor.stop_monitoring()
        
        # Collect profiling data
        profile_data = {
            'simulation_time': end_time - start_time,
            'steps_completed': steps,
            'steps_per_second': steps / (end_time - start_time),
            'final_brain_stats': {
                'neurons': config.getTotalNeuronCount(),
                'synapses': config.getTotalSynapseCount(),
                'spikes': config.getTotalSpikeCount(),
                'firing_rate': config.getAverageFiringRate()
            },
            'final_agent_stats': {
                'curiosity': agent.getCuriosityLevel(),
                'novelty': agent.getNoveltyLevel(),
                'prediction_error': agent.getPredictionError()
            },
            'performance_profile': self.profiler.get_profile_summary("complete_simulation"),
            'monitoring_data': self.monitor.samples
        }
        
        # Analyze brain
        brain_analysis = self.analyzer.analyze_brain(config, steps)
        
        print(f"Simulation complete! Duration: {profile_data['simulation_time']:.2f}s")
        print(f"Performance: {profile_data['steps_per_second']:.1f} steps/sec")
        
        return {
            'profile_data': profile_data,
            'brain_analysis': brain_analysis
        }
    
    def generate_comprehensive_report(self, profile_results: Dict[str, Any]) -> str:
        """Generate comprehensive profiling report"""
        profile_data = profile_results['profile_data']
        brain_analysis = profile_results['brain_analysis']
        
        report = """NLM Performance Profiling Report
===================================

Simulation Performance:
  Total Time: {simulation_time:.2f} seconds
  Steps Completed: {steps_completed}
  Steps per Second: {steps_per_second:.1f}

Final Brain Statistics:
  Neurons: {neurons}
  Synapses: {synapses}
  Total Spikes: {spikes}
  Average Firing Rate: {firing_rate:.2f} Hz

Final Agent Statistics:
  Curiosity Level: {curiosity:.3f}
  Novelty Level: {novelty:.3f}
  Prediction Error: {prediction_error:.3f}

Top Performance Bottlenecks:
{performance_bottlenecks}

Neural Analysis:
{neural_analysis}

Recommendations:
{recommendations}
""".format(
            simulation_time=profile_data['simulation_time'],
            steps_completed=profile_data['steps_completed'],
            steps_per_second=profile_data['steps_per_second'],
            neurons=profile_data['final_brain_stats']['neurons'],
            synapses=profile_data['final_brain_stats']['synapses'],
            spikes=profile_data['final_brain_stats']['spikes'],
            firing_rate=profile_data['final_brain_stats']['firing_rate'],
            curiosity=profile_data['final_agent_stats']['curiosity'],
            novelty=profile_data['final_agent_stats']['novelty'],
            prediction_error=profile_data['final_agent_stats']['prediction_error'],
            performance_bottlenecks="Major bottlenecks would be identified here",
            neural_analysis="Neural analysis would be detailed here",
            recommendations="Optimization recommendations would be provided here"
        )
        
        return report
    
    def save_profile_results(self, results: Dict[str, Any], directory: str):
        """Save profiling results to directory"""
        os.makedirs(directory, exist_ok=True)
        
        # Save profile data
        profile_file = os.path.join(directory, "profile_data.json")
        with open(profile_file, 'w') as f:
            json.dump(results['profile_data'], f, indent=2)
        
        # Save brain analysis
        analysis_file = os.path.join(directory, "brain_analysis.json")
        with open(analysis_file, 'w') as f:
            json.dump(results['brain_analysis'], f, indent=2)
        
        # Save comprehensive report
        report_file = os.path.join(directory, "profile_report.txt")
        with open(report_file, 'w') as f:
            f.write(self.generate_comprehensive_report(results))
        
        print(f"Profile results saved to {directory}")
class NLPOptimizer:
    """Main NLM optimization interface"""
    
    def __init__(self):
        self.monitor = PerformanceMonitor()
        self.profiler = PerformanceProfiler()
        self.memory_profiler = MemoryProfiler()
        self.optimizer = SimulationOptimizer()
        self.analyzer = NLPAnalyzer()
        self.profiler_instance = SimulationProfiler()
    
    def run_optimization_study(self, config_template: pynlm.Config, 
                               world_config: Dict, agent_config: Dict,
                               steps: int, output_dir: str = "optimization_results"):
        """Run complete optimization study"""
        print("Starting NLM Optimization Study...")
        print("=" * 50)
        
        os.makedirs(output_dir, exist_ok=True)
        
        # Run multiple simulations with different configurations
        configurations = [
            {'name': 'Baseline', 'neurons': 1000, 'regions': 2},
            {'name': 'High Performance', 'neurons': 2000, 'regions': 3},
            {'name': 'Low Resource', 'neurons': 500, 'regions': 1},
        ]
        
        results = {}
        
        for config_name in configurations:
            print(f"\nRunning {config_name['name']} configuration...")
            
            # Create configuration
            test_config = pynlm.createConfigWithSettings(
                neuron_count=config_name['neurons'],
                region_count=config_name['regions'],
                synapse_density=0.1
            )
            
            # Run simulation
            simulation_results = self.profiler_instance.profile_complete_simulation(
                world_config, {**agent_config, **config_name}, steps
            )
            
            results[config_name['name']] = simulation_results
            
            # Save intermediate results
            config_dir = os.path.join(output_dir, config_name['name'])
            self.profiler_instance.save_profile_results(simulation_results, config_dir)
        
        # Analyze optimization results
        optimization_report = self._generate_optimization_report(results)
        
        # Save optimization report
        report_file = os.path.join(output_dir, "optimization_report.txt")
        with open(report_file, 'w') as f:
            f.write(optimization_report)
        
        print(f"\nOptimization study complete!")
        print(f"Results saved to {output_dir}")
        
        return results, optimization_report
    
    def _generate_optimization_report(self, results: Dict[str, Any]) -> str:
        """Generate optimization report"""
        report = """NLM Optimization Study Report
=================================

Configuration Comparison:
"""
        
        for config_name, result in results.items():
            profile = result['profile_data']
            report += f"\n{config_name}:
  Simulation Time: {profile['simulation_time']:.2f}s
  Steps/sec: {profile['steps_per_second']:.1f}
  Final Neurons: {profile['final_brain_stats']['neurons']}
  Final Spikes: {profile['final_brain_stats']['spikes']}
  Curiosity Level: {profile['final_agent_stats']['curiosity']:.3f}
  Novelty Level: {profile['final_agent_stats']['novelty']:.3f}
""
        
        report += "\n" + "=" * 50 + "\n"
        report += "\nRecommendations:\n"
        report += "Based on the results above, consider the following optimizations:\n"
        
        # Simple recommendation logic
        if len(results) >= 2:
            fastest = min(results.keys(), key=lambda x: results[x]['profile_data']['simulation_time'])
            most_curious = max(results.keys(), key=lambda x: results[x]['profile_data']['final_agent_stats']['curiosity'])
            
            report += f"1. For fastest performance: Use {fastest} configuration\n"
            report += f"2. For highest curiosity (exploration): Use {most_curious} configuration\n"
        
        report += "\nConsider trade-offs between simulation speed and behavioral complexity.\n"
        
        return report
    
    def run_performance_benchmark(self, world_config: Dict, agent_config: Dict, steps: int,
                                 output_file: str = "benchmark_results.json"):
        """Run performance benchmark"""
        print(f"Running performance benchmark...")
        
        # Run with memory profiling
        self.memory_profiler.start_memory_profiling()
        
        try:
            # Run simulation
            config = pynlm.createBrain(pynlm.createConfigWithSettings(**agent_config))
            config.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(**world_config)
            world.reset()
            
            agent = pynlm.createAgentBrain(config)
            agent.initialize(world)
            
            if agent_config.get('enable_learning', False):
                agent.enableRewardModulation(True)
                agent.enableCuriosity(True)
                agent.enableDevelopment(True)
            
            # Time the simulation
            start_time = time.time()
            
            for step in range(steps):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                config.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                reward = percept.getInternal()[0] if percept.getInternal() else 0.0
                agent.applyRewardModulation(reward, 0.0)
                agent.updateDevelopment(0.1)
            
            end_time = time.time()
            
            # Collect benchmark data
            benchmark_data = {
                'world_config': world_config,
                'agent_config': agent_config,
                'steps': steps,
                'total_time': end_time - start_time,
                'steps_per_second': steps / (end_time - start_time),
                'final_brain_stats': {
                    'neurons': config.getTotalNeuronCount(),
                    'synapses': config.getTotalSynapseCount(),
                    'spikes': config.getTotalSpikeCount(),
                    'firing_rate': config.getAverageFiringRate()
                },
                'final_agent_stats': {
                    'curiosity': agent.getCuriosityLevel(),
                    'novelty': agent.getNoveltyLevel(),
                    'prediction_error': agent.getPredictionError()
                },
                'memory_stats': self.memory_profiler.get_memory_statistics()
            }
            
            # Save results
            with open(output_file, 'w') as f:
                json.dump(benchmark_data, f, indent=2)
            
            print(f"Benchmark complete!")
            print(f"Time: {benchmark_data['total_time']:.2f}s")
            print(f"Speed: {benchmark_data['steps_per_second']:.1f} steps/sec")
            print(f"Results saved to {output_file}")
            
            return benchmark_data
            
        finally:
            self.memory_profiler.stop_memory_profiling()
class NLPToolkit:
    """Complete NLM toolkit with all optimization and profiling tools"""
    
    def __init__(self):
        self.optimizer = NLPOptimizer()
    
    def create_optimization_project(self, project_name: str, config_template: pynlm.Config,
                                    world_config: Dict, agent_config: Dict, steps: int,
                                    output_dir: str = None):
        """Create a new optimization project"""
        if output_dir is None:
            output_dir = f"nlm_optimization_{project_name}_{int(time.time())}"
        
        print(f"Creating optimization project: {project_name}")
        print(f"Output directory: {output_dir}")
        
        # Run optimization study
        results, report = self.optimizer.run_optimization_study(
            config_template, world_config, agent_config, steps, output_dir
        )
        
        return results, report, output_dir
    
    def run_benchmark_suite(self, world_config: Dict, agent_config: Dict, 
                           benchmark_steps: int, output_file: str = "benchmark_results.json"):
        """Run complete benchmark suite"""
        print("Running benchmark suite...")
        
        return self.optimizer.run_performance_benchmark(
            world_config, agent_config, benchmark_steps, output_file
        )
    
    def analyze_performance(self, simulation_results: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze performance results"""
        return self.optimizer.profiler_instance.generate_comprehensive_report(simulation_results)

# Convenience functions for common use cases
def quick_benchmark(config_name: str = "medium", steps: int = 100) -> Dict[str, Any]:
    """Quick benchmark with predefined configurations"""
    configs = {
        'simple': {'neuron_count': 500, 'regions': 1},
        'medium': {'neuron_count': 1000, 'regions': 2},
        'complex': {'neuron_count': 2000, 'regions': 3}
    }
    
    world_config = {'width': 20, 'height': 20, 'vision_width': 8, 'vision_height': 8}
    agent_config = {
        'enable_learning': True,
        **configs[config_name]
    }
    
    toolkit = NLPToolkit()
    return toolkit.run_benchmark_suite(world_config, agent_config, steps)

def analyze_brain_performance(brain: pynlm.Brain, simulation_steps: int = 100) -> str:
    """Analyze brain performance and return report"""
    analyzer = NLPAnalyzer()
    analysis = analyzer.analyze_brain(brain, simulation_steps)
    return analyzer.generate_report(analysis, 'text')

def profile_simulation(world_config: Dict, agent_config: Dict, steps: int) -> Dict[str, Any]:
    """Profile a simulation with comprehensive profiling"""
    profiler = SimulationProfiler()
    return profiler.profile_complete_simulation(world_config, agent_config, steps)

def optimize_configuration(base_config: pynlm.Config, world_config: Dict, 
                         agent_config: Dict, steps: int) -> pynlm.Config:
    """Optimize configuration for best performance"""
    optimizer = SimulationOptimizer()
    return optimizer.optimize_brain_configuration(base_config, 1.0)
# Example usage and demonstration functions
def demonstrate_profiling():
    """Demonstrate profiling capabilities"""
    print("NLM Profiling Demonstration")
    print("=" * 30)
    
    # Create configuration
    config = pynlm.createConfigWithSettings(
        neuron_count=1000,
        region_count=2,
        synapse_density=0.1
    )
    
    world_config = {'width': 15, 'height': 15, 'vision_width': 8, 'vision_height': 8}
    agent_config = {'enable_learning': True}
    
    # Run profiling
    profiler = SimulationProfiler()
    results = profiler.profile_complete_simulation(world_config, agent_config, 50)
    
    # Generate report
    report = profiler.generate_comprehensive_report(results)
    print(report)
    
    return results

def demonstrate_optimization():
    """Demonstrate optimization capabilities"""
    print("NLM Optimization Demonstration")
    print("=" * 35)
    
    # Create base configuration
    base_config = pynlm.createDefaultConfig()
    
    world_config = {'width': 20, 'height': 20, 'vision_width': 10, 'vision_height': 10}
    agent_config = {'enable_learning': True}
    
    # Run optimization
    optimizer = NLPOptimizer()
    
    results, report = optimizer.run_optimization_study(
        base_config, world_config, agent_config, 30
    )
    
    print(report)
    
    return results, report

def main():
    """Main entry point with example demonstrations"""
    print("NLM Performance Optimization and Profiling Toolkit")
    print("=" * 55)
    print()
    
    print("Available demonstrations:")
    print("  1. Quick benchmark")
    print("  2. Brain performance analysis")
    print("  3. Simulation profiling")
    print("  4. Configuration optimization")
    print("  5. Complete profiling study")
    print()
    
    print("Note: This toolkit provides advanced performance monitoring")
    print("      and optimization tools for NLM simulations.")
    print()
    
    # Example usage (uncomment to run)
    # try:
    #     print("Running quick benchmark...")
    #     quick_benchmark("medium", 50)
    #     
    #     print("\nRunning profiling demonstration...")
    #     demonstrate_profiling()
    #     
    # except Exception as e:
    #     print(f"Error in demonstration: {e}")
    
    print("To use the toolkit, import the main classes:")
    print("  from nlm_toolkit import NLPOptimizer, SimulationProfiler, NLPToolkit")
    print()
    print("Or use convenience functions:")
    print("  from nlm_toolkit import quick_benchmark, profile_simulation")
    print()
    print("For more information, see the documentation.")

if __name__ == "__main__":
    main()