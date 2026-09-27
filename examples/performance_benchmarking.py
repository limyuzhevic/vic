"""
NLM Performance Benchmarking Example
====================================

Comprehensive demonstration of NLM performance benchmarking
and optimization techniques for neural simulations.

This example shows:
1. Performance measurement and profiling
2. Optimization strategies and techniques
3. Comparative analysis across different configurations
4. Automated hyperparameter optimization
5. Performance monitoring and reporting

Prerequisites:
- NLM C++ bindings must be built and installed
- Python 3.8+ with pynlm module
- Performance analysis tools
"""

import pynlm as nlm
import time
import json
import statistics
import matplotlib.pyplot as plt
from typing import Dict, List, Any, Tuple
from datetime import datetime
import numpy as np

class PerformanceBenchmarker:
    """Comprehensive performance benchmarking for NLM simulations."""
    
    def __init__(self):
        self.benchmark_results = {}
        self.performance_metrics = {}
        self.optimization_history = []
        
    def setup_benchmark_environment(self, config: Dict[str, Any] = None) -> Dict[str, Any]:
        """Setup benchmarking environment with performance monitoring."""
        print("1. Setting up performance benchmarking environment...")
        
        # Create brain configurations for comparison
        brain_configs = {
            'small': {
                'neuron_count': 500,
                'complexity': 0.4,
                'description': 'Small scale simulation'
            },
            'medium': {
                'neuron_count': 1000,
                'complexity': 0.6,
                'description': 'Medium scale simulation'
            },
            'large': {
                'neuron_count': 2000,
                'complexity': 0.8,
                'description': 'Large scale simulation'
            }
        }
        
        # Merge with user config
        if config:
            for key, user_config in config.items():
                if key in brain_configs:
                    brain_configs[key].update(user_config)
        
        print(f"   Configured {len(brain_configs)} benchmark scales")
        for scale, settings in brain_configs.items():
            print(f"     {scale:6s}: {settings['description']} ({settings['neuron_count']} neurons)")
        
        return {
            'brain_configs': brain_configs,
            'benchmark_settings': config.get('benchmark_settings', {}),
            'measurement_tools': self.setup_measurement_tools()
        }
    
    def setup_measurement_tools(self) -> Dict[str, Any]:
        """Setup comprehensive measurement tools."""
        print("2. Setting up measurement tools...")
        
        measurement_tools = {
            'time_profiler': TimeProfiler(),
            'memory_profiler': MemoryProfiler(),
            'cpu_profiler': CPUProfiler(),
            'simd_profiler': SIMDProfiler(),
            'cache_profiler': CacheProfiler(),
            'throughput_monitor': ThroughputMonitor(),
            'stability_analyzer': StabilityAnalyzer()
        }
        
        print(f"   ✓ Time profiler: {measurement_tools['time_profiler'].__class__.__name__}")
        print(f"   ✓ Memory profiler: {measurement_tools['memory_profiler'].__class__.__name__}")
        print(f"   ✓ CPU profiler: {measurement_tools['cpu_profiler'].__class__.__name__}")
        print(f"   ✓ SIMD profiler: {measurement_tools['simd_profiler'].__class__.__name__}")
        print(f"   ✓ Cache profiler: {measurement_tools['cache_profiler'].__class__.__name__}")
        print(f"   ✓ Throughput monitor: {measurement_tools['throughput_monitor'].__class__.__name__}")
        print(f"   ✓ Stability analyzer: {measurement_tools['stability_analyzer'].__class__.__name__}")
        
        return measurement_tools
    
    def run_benchmark_suite(self, environment: Dict[str, Any]) -> Dict[str, Any]:
        """Run comprehensive benchmark suite across different configurations."""
        print("3. Running benchmark suite...")
        
        brain_configs = environment['brain_configs']
        measurement_tools = environment['measurement_tools']
        benchmark_settings = environment['benchmark_settings']
        
        # Get benchmark parameters
        steps = benchmark_settings.get('steps', 100)
        iterations = benchmark_settings.get('iterations', 3)
        warmup_steps = benchmark_settings.get('warmup_steps', 20)
        
        benchmark_results = {}
        
        for scale, config in brain_configs.items():
            print(f"\n   Running benchmark for {scale.upper()} scale...")
            
            scale_results = {
                'scale': scale,
                'config': config,
                'iterations': iterations,
                'steps': steps,
                'warmup_steps': warmup_steps,
                'trials': [],
                'performance_metrics': {},
                'efficiency_score': 0.0
            }
            
            for trial in range(iterations):
                print(f"     Trial {trial + 1}/{iterations}...")
                
                # Create brain with specific configuration
                brain = nlm.NLMBrain(
                    neuron_count=config['neuron_count'],
                    complexity=config['complexity']
                )
                brain.initialize()
                
                # Run warmup
                for warmup_step in range(warmup_steps):
                    brain.step(warmup_step)
                
                # Run benchmark with measurements
                trial_results = self.run_benchmark_trial(
                    brain, measurement_tools, steps, warmup_steps
                )
                
                scale_results['trials'].append(trial_results)
                
                # Small delay between trials
                time.sleep(0.1)
            
            # Analyze trial results
            scale_results['performance_metrics'] = self.analyze_benchmark_results(scale_results['trials'])
            scale_results['efficiency_score'] = self.calculate_efficiency_score(scale_results['performance_metrics'])
            
            benchmark_results[scale] = scale_results
            
            print(f"     ✓ {scale.upper()} benchmark completed")
        
        print(f"\n   ✓ Benchmark suite completed: {len(benchmark_results)} scales tested")
        
        return benchmark_results
    
    def run_benchmark_trial(self, brain, measurement_tools: Dict[str, Any], steps: int, warmup_steps: int) -> Dict[str, Any]:
        """Run a single benchmark trial with comprehensive measurements."""
        trial_results = {
            'step_data': [],
            'performance': {},
            'memory_usage': [],
            'cpu_usage': [],
            'simd_usage': [],
            'cache_misses': [],
            'throughput': [],
            'stability': []
        }
        
        # Reset brain for fresh trial
        brain.reset()
        brain.initialize()
        
        # Start all measurement tools
        for name, tool in measurement_tools.items():
            tool.start()
        
        # Run benchmark steps
        for step in range(steps):
            # Start step measurement
            for name, tool in measurement_tools.items():
                tool.start_step(step)
            
            # Run simulation step
            brain.step(step)
            
            # Collect measurements
            trial_results['step_data'].append({
                'step': step,
                'timestamp': time.time()
            })
            
            # Get measurements from tools
            trial_results['memory_usage'].append(measurement_tools['memory_profiler'].get_current_memory())
            trial_results['cpu_usage'].append(measurement_tools['cpu_profiler'].get_current_cpu())
            trial_results['simd_usage'].append(measurement_tools['simd_profiler'].get_simd_usage())
            trial_results['cache_misses'].append(measurement_tools['cache_profiler'].get_cache_misses())
            trial_results['throughput'].append(measurement_tools['throughput_monitor'].get_throughput(step))
            trial_results['stability'].append(measurement_tools['stability_analyzer'].get_stability(step))
            
            # Stop step measurement
            for name, tool in measurement_tools.items():
                tool.stop_step()
            
            # Print progress
            if step % 20 == 0:
                firing_rate = brain.get_average_firing_rate()
                print(f"     Step {step:3d}/{steps}: FR={firing_rate:6.3f} Hz")
        
        # Stop all measurement tools
        for name, tool in measurement_tools.items():
            tool.stop()
        
        # Aggregate measurements
        trial_results['performance'] = self.aggregate_measurements(trial_results)
        
        return trial_results
    
    def analyze_benchmark_results(self, trials: List[Dict[str, Any]]) -> Dict[str, Any]:n:
        """Analyze benchmark results across multiple trials."""
        if not trials:
            return {}
        
        # Aggregate across trials
        all_memory = []
        all_cpu = []
        all_simd = []
        all_cache = []
        all_throughput = []
        all_stability = []
        all_firing_rates = []
        
        for trial in trials:
            all_memory.extend(trial['memory_usage'])
            all_cpu.extend(trial['cpu_usage'])
            all_simd.extend(trial['simd_usage'])
            all_cache.extend(trial['cache_misses'])
            all_throughput.extend(trial['throughput'])
            all_stability.extend(trial['stability'])
            
            # Extract firing rates from step data
            for step_data in trial['step_data']:
                firing_rate = step_data.get('firing_rate', 0)
                if firing_rate > 0:
                    all_firing_rates.append(firing_rate)
        
        # Calculate statistics
        analysis = {
            'memory': {
                'mean': statistics.mean(all_memory) if all_memory else 0,
                'median': statistics.median(all_memory) if all_memory else 0,
                'std': statistics.stdev(all_memory) if len(all_memory) > 1 else 0,
                'min': min(all_memory) if all_memory else 0,
                'max': max(all_memory) if all_memory else 0,
                'total_samples': len(all_memory)
            },
            'cpu': {
                'mean': statistics.mean(all_cpu) if all_cpu else 0,
                'median': statistics.median(all_cpu) if all_cpu else 0,
                'std': statistics.stdev(all_cpu) if len(all_cpu) > 1 else 0
            },
            'simd': {
                'mean': statistics.mean(all_simd) if all_simd else 0,
                'median': statistics.median(all_simd) if all_simd else 0,
                'utilization': sum(all_simd) / len(all_simd) if all_simd else 0
            },
            'cache': {
                'miss_rate': statistics.mean(all_cache) / len(all_cache) if all_cache else 0,
                'total_misses': sum(all_cache),
                'samples': len(all_cache)
            },
            'throughput': {
                'mean': statistics.mean(all_throughput) if all_throughput else 0,
                'median': statistics.median(all_throughput) if all_throughput else 0,
                'std': statistics.stdev(all_throughput) if len(all_throughput) > 1 else 0
            },
            'stability': {
                'mean': statistics.mean(all_stability) if all_stability else 0,
                'median': statistics.median(all_stability) if all_stability else 0,
                'consistency': 1.0 - statistics.stdev(all_stability) / statistics.mean(all_stability) if all_stability and statistics.mean(all_stability) > 0 else 0
            },
            'firing_rates': {
                'mean': statistics.mean(all_firing_rates) if all_firing_rates else 0,
                'median': statistics.median(all_firing_rates) if all_firing_rates else 0,
                'std': statistics.stdev(all_firing_rates) if len(all_firing_rates) > 1 else 0,
                'range': max(all_firing_rates) - min(all_firing_rates) if all_firing_rates else 0
            }
        }
        
        return analysis
    
    def calculate_efficiency_score(self, performance: Dict[str, Any]) -> float:
        """Calculate overall efficiency score from performance metrics."""
        if not performance:
            return 0.0
        
        # Efficiency factors (normalized 0-1)
        efficiency_factors = []
        
        # Memory efficiency (lower memory usage is better)
        memory_mean = performance.get('memory', {}).get('mean', 0)
        memory_efficiency = max(0, 1.0 - memory_mean / 1000.0)  # Normalize to 0-1
        efficiency_factors.append(memory_efficiency)
        
        # CPU efficiency (moderate CPU usage is optimal)
        cpu_mean = performance.get('cpu', {}).get('mean', 0)
        cpu_efficiency = max(0, 1.0 - abs(cpu_mean - 50.0) / 100.0)  # Centered around 50%
        efficiency_factors.append(cpu_efficiency)
        
        # SIMD utilization
        simd_utilization = performance.get('simd', {}).get('utilization', 0)
        efficiency_factors.append(simd_utilization)
        
        # Cache efficiency (lower miss rate is better)
        cache_miss_rate = performance.get('cache', {}).get('miss_rate', 0)
        cache_efficiency = max(0, 1.0 - cache_miss_rate)
        efficiency_factors.append(cache_efficiency)
        
        # Throughput
        throughput_mean = performance.get('throughput', {}).get('mean', 0)
        throughput_efficiency = min(throughput_mean / 1000.0, 1.0)  # Scale up to 1.0
        efficiency_factors.append(throughput_efficiency)
        
        # Stability
        stability_consistency = performance.get('stability', {}).get('consistency', 0)
        efficiency_factors.append(stability_consistency)
        
        # Firing rate consistency (lower std is better for stability)
        firing_rate_std = performance.get('firing_rates', {}).get('std', 0)
        firing_rate_efficiency = max(0, 1.0 - firing_rate_std / 50.0)
        efficiency_factors.append(firing_rate_efficiency)
        
        # Calculate weighted average
        weights = [0.15, 0.10, 0.15, 0.10, 0.15, 0.15, 0.10]  # Total = 1.0
        
        if len(efficiency_factors) != len(weights):
            # Adjust if mismatch
            weights = [1.0 / len(efficiency_factors)] * len(efficiency_factors)
        
        efficiency_score = sum(factor * weight for factor, weight in zip(efficiency_factors, weights))
        
        return efficiency_score
    
    def aggregate_measurements(self, trial_results: Dict[str, Any]) -> Dict[str, Any]:
        """Aggregate measurements from a single trial."""
        # This is a placeholder for measurement aggregation
        # In a real implementation, this would combine data from all measurement tools
        return {
            'total_steps': len(trial_results['step_data']),
            'total_time': trial_results['step_data'][-1]['timestamp'] - trial_results['step_data'][0]['timestamp'],
            'memory_samples': len(trial_results['memory_usage']),
            'cpu_samples': len(trial_results['cpu_usage']),
            'simd_samples': len(trial_results['simd_usage']),
            'cache_samples': len(trial_results['cache_misses']),
            'throughput_samples': len(trial_results['throughput']),
            'stability_samples': len(trial_results['stability'])
        }
    
    def generate_optimization_recommendations(self, benchmark_results: Dict[str, Any]) -> List[str]:
        """Generate optimization recommendations based on benchmark results."""
        recommendations = []
        
        # Find best performing configuration
        best_scale = None
        best_efficiency = -1
        
        for scale, results in benchmark_results.items():
            efficiency = results.get('efficiency_score', 0)
            if efficiency > best_efficiency:
                best_efficiency = efficiency
                best_scale = scale
        
        if best_scale:
            recommendations.append(f"✓ Use {best_scale.upper()} scale as optimal baseline")
            
            # Analyze specific optimizations
            best_config = benchmark_results[best_scale]['config']
            
            # Memory optimization
            memory_efficiency = benchmark_results[best_scale]['performance_metrics']['memory']['mean']
            if memory_efficiency > 500:
                recommendations.append("📉 Memory optimization needed: Consider reducing memory footprint")
            elif memory_efficiency < 200:
                recommendations.append("✅ Good memory efficiency")
            
            # CPU optimization
            cpu_efficiency = benchmark_results[best_scale]['performance_metrics']['cpu']['mean']
            if cpu_efficiency > 80:
                recommendations.append("📉 CPU optimization needed: Reduce CPU usage")
            elif cpu_efficiency < 30:
                recommendations.append("✅ Good CPU utilization")
            
            # SIMD optimization
            simd_utilization = benchmark_results[best_scale]['performance_metrics']['simd']['utilization']
            if simd_utilization < 0.7:
                recommendations.append("📉 SIMD optimization needed: Increase SIMD vectorization")
            else:
                recommendations.append("✅ Good SIMD utilization")
            
            # Cache optimization
            cache_miss_rate = benchmark_results[best_scale]['performance_metrics']['cache']['miss_rate']
            if cache_miss_rate > 0.2:
                recommendations.append("📉 Cache optimization needed: Improve cache locality")
            else:
                recommendations.append("✅ Good cache efficiency")
        
        return recommendations
    
    def create_benchmark_report(self, benchmark_results: Dict[str, Any], output_file: str = None):
        """Create comprehensive benchmark report."""
        print("4. Creating benchmark report...")
        
        # Generate benchmark summary
        benchmark_summary = {
            'benchmark_info': {
                'timestamp': datetime.now().isoformat(),
                'tool_version': 'NLM Performance Benchmark v1.0',
                'total_scales_tested': len(benchmark_results),
                'status': 'completed'
            },
            'results_summary': {},
            'performance_analysis': {},
            'optimization_recommendations': self.generate_optimization_recommendations(benchmark_results)
        }
        
        # Add results for each scale
        for scale, results in benchmark_results.items():
            benchmark_summary['results_summary'][scale] = {
                'neuron_count': results['config']['neuron_count'],
                'complexity': results['config']['complexity'],
                'efficiency_score': results['efficiency_score'],
                'memory_usage': results['performance_metrics']['memory'],
                'cpu_usage': results['performance_metrics']['cpu'],
                'simd_utilization': results['performance_metrics']['simd']['utilization'],
                'cache_efficiency': 1.0 - results['performance_metrics']['cache']['miss_rate'],
                'throughput': results['performance_metrics']['throughput'],
                'stability': results['performance_metrics']['stability']
            }
        
        # Calculate overall performance rankings
        performance_ranking = []
        for scale, results in benchmark_results.items():
            efficiency = results['efficiency_score']
            performance_ranking.append({
                'scale': scale,
                'efficiency_score': efficiency,
                'neuron_count': results['config']['neuron_count'],
                'performance_rank': len(performance_ranking) + 1  # Will be updated
            })
        
        # Sort by efficiency score
        performance_ranking.sort(key=lambda x: x['efficiency_score'], reverse=True)
        
        # Update ranks
        for i, item in enumerate(performance_ranking):
            item['performance_rank'] = i + 1
        
        benchmark_summary['performance_ranking'] = performance_ranking
        
        # Create analysis
        benchmark_summary['performance_analysis'] = {
            'best_efficiency': max(item['efficiency_score'] for item in performance_ranking),
            'worst_efficiency': min(item['efficiency_score'] for item in performance_ranking),
            'average_efficiency': statistics.mean(item['efficiency_score'] for item in performance_ranking),
            'efficiency_variance': statistics.variance(item['efficiency_score'] for item in performance_ranking) if len(performance_ranking) > 1 else 0,
            'scale_relationship': self.analyze_scale_relationship(benchmark_results)
        }
        
        # Save benchmark report
        if output_file:
            with open(output_file, 'w') as f:
                json.dump(benchmark_summary, f, indent=2)
            print(f"   ✓ Benchmark report saved to {output_file}")
        
        return benchmark_summary
    
    def analyze_scale_relationship(self, benchmark_results: Dict[str, Any]) -> Dict[str, Any]:
        """Analyze relationship between scale and performance."""
        if len(benchmark_results) < 2:
            return {
                'relationship': 'insufficient_data',
                'insights': ['Need at least 2 scales for relationship analysis']
            }
        
        # Extract data for analysis
        scales = []
        efficiencies = []
        neuron_counts = []
        
        for scale, results in benchmark_results.items():
            scales.append(scale)
            efficiencies.append(results['efficiency_score'])
            neuron_counts.append(results['config']['neuron_count'])
        
        # Calculate correlation between neuron count and efficiency
        if len(neuron_counts) > 1:
            correlation = self.calculate_correlation(neuron_counts, efficiencies)
        else:
            correlation = 0.0
        
        # Determine relationship type
        if abs(correlation) < 0.3:
            relationship = 'weak'
            insights = ['Weak correlation between scale and efficiency']
        elif abs(correlation) < 0.7:
            relationship = 'moderate'
            insights = ['Moderate correlation between scale and efficiency']
        else:
            relationship = 'strong'
            insights = ['Strong correlation between scale and efficiency']
        
        # Add scaling insights
        if correlation > 0:
            insights.append('✓ Larger scales show better efficiency')
            insights.append('⚠ Consider scalability limits for very large scales')
        else:
            insights.append('✗ Larger scales show worse efficiency')
            insights.append('⚠ Consider resource constraints for larger scales')
        
        return {
            'relationship': relationship,
            'correlation_coefficient': correlation,
            'insights': insights,
            'recommendations': self.generate_scaling_recommendations(correlation, scales)
        }
    
    def calculate_correlation(self, x: List[float], y: List[float]) -> float:
        """Calculate Pearson correlation coefficient."""
        if len(x) != len(y) or len(x) < 2:
            return 0.0
        
        x_mean = statistics.mean(x)
        y_mean = statistics.mean(y)
        
        numerator = sum((xi - x_mean) * (yi - y_mean) for xi, yi in zip(x, y))
        denominator = (sum((xi - x_mean) ** 2 for xi in x) * 
                       sum((yi - y_mean) ** 2 for yi in y)) ** 0.5
        
        if denominator == 0:
            return 0.0
        
        return numerator / denominator
    
    def generate_scaling_recommendations(self, correlation: float, scales: List[str]) -> List[str]:
        """Generate scaling recommendations based on correlation analysis."""
        recommendations = []
        
        if correlation > 0.5:
            recommendations.extend([
                'Scale up to larger sizes for better performance',
                'Consider resource-intensive optimizations for large scales',
                'Plan for scalability in production deployments'
            ])
        elif correlation > 0.2:
            recommendations.extend([
                'Moderate scaling benefits available',
                'Balance scale with optimization efforts',
                'Consider hybrid scaling strategies'
            ])
        elif correlation > -0.2:
            recommendations.extend([
                'Scale has minimal impact on performance',
                'Focus on algorithmic optimizations over scale',
                'Consider other optimization dimensions'
            ])
        else:
            recommendations.extend([
                'Scale negatively impacts performance',
                'Consider smaller scale optimizations',
                'Focus on efficiency rather than size'
            ])
        
        return recommendations
    
    def run_complete_benchmark_suite(self, config: Dict[str, Any] = None) -> Dict[str, Any]:
        """Run complete benchmark suite with all analyses."""
        print("=" * 80)
        print("NLM Performance Benchmarking Suite")
        print("=" * 80)
        print("This comprehensive benchmark suite evaluates NLM performance")
        print("across different scales and provides optimization recommendations.")
        print("=" * 80)
        
        # 1. Setup benchmark environment
        environment = self.setup_benchmark_environment(config)
        
        # 2. Run benchmark suite
        benchmark_results = self.run_benchmark_suite(environment)
        
        # 3. Create benchmark report
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        report_file = f"benchmark_results/benchmark_report_{timestamp}.json"
        benchmark_summary = self.create_benchmark_report(benchmark_results, report_file)
        
        # 4. Generate visualization data
        visualization_data = self.create_visualization_data(benchmark_results)
        
        # 5. Create analysis summary
        analysis_summary = self.create_analysis_summary(benchmark_summary)
        
        print("\n" + "=" * 80)
        print("BENCHMARK SUITE COMPLETED")
        print("=" * 80)
        
        self.display_benchmark_summary(benchmark_summary, analysis_summary)
        
        return {
            'benchmark_summary': benchmark_summary,
            'visualization_data': visualization_data,
            'analysis_summary': analysis_summary,
            'timestamp': timestamp,
            'success': True
        }
    
    def create_visualization_data(self, benchmark_results: Dict[str, Any]) -> Dict[str, Any]:
        """Create visualization-ready data."""
        print("Creating visualization data...")
        
        visualization_data = {
            'efficiency_chart': {
                'scales': [],
                'efficiencies': [],
                'error_bars': []
            },
            'memory_usage_chart': {
                'scales': [],
                'memory_usage': [],
                'error_bars': []
            },
            'throughput_chart': {
                'scales': [],
                'throughput': [],
                'error_bars': []
            },
            'radar_chart': {
                'scales': [],
                'metrics': {
                    'memory_efficiency': [],
                    'cpu_efficiency': [],
                    'simd_utilization': [],
                    'cache_efficiency': [],
                    'throughput_efficiency': [],
                    'stability': []
                }
            }
        }
        
        for scale, results in benchmark_results.items():
            perf = results['performance_metrics']
            
            visualization_data['efficiency_chart']['scales'].append(scale)
            visualization_data['efficiency_chart']['efficiencies'].append(results['efficiency_score'])
            
            visualization_data['memory_usage_chart']['scales'].append(scale)
            visualization_data['memory_usage_chart']['memory_usage'].append(perf['memory']['mean'])
            
            visualization_data['throughput_chart']['scales'].append(scale)
            visualization_data['throughput_chart']['throughput'].append(perf['throughput']['mean'])
            
            visualization_data['radar_chart']['scales'].append(scale)
            visualization_data['radar_chart']['metrics']['memory_efficiency'].append(1.0 - perf['memory']['mean'] / 1000.0)
            visualization_data['radar_chart']['metrics']['cpu_efficiency'].append(1.0 - abs(perf['cpu']['mean'] - 50.0) / 100.0)
            visualization_data['radar_chart']['metrics']['simd_utilization'].append(perf['simd']['utilization'])
            visualization_data['radar_chart']['metrics']['cache_efficiency'].append(1.0 - perf['cache']['miss_rate'])
            visualization_data['radar_chart']['metrics']['throughput_efficiency'].append(min(perf['throughput']['mean'] / 1000.0, 1.0))
            visualization_data['radar_chart']['metrics']['stability'].append(perf['stability']['consistency'])
        
        # Save visualization data
        vis_file = f"benchmark_results/visualization_data_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(vis_file, 'w') as f:
            json.dump(visualization_data, f, indent=2)
        
        print(f"   ✓ Visualization data saved to {vis_file}")
        return visualization_data
    
    def create_analysis_summary(self, benchmark_summary: Dict[str, Any]) -> Dict[str, Any]:
        """Create comprehensive analysis summary."""
        print("Creating analysis summary...")
        
        efficiency_scores = [item['efficiency_score'] for item in benchmark_summary['performance_ranking']]
        
        analysis_summary = {
            'statistical_analysis': {
                'efficiency_mean': statistics.mean(efficiency_scores),
                'efficiency_std': statistics.stdev(efficiency_scores) if len(efficiency_scores) > 1 else 0,
                'efficiency_median': statistics.median(efficiency_scores),
                'efficiency_range': max(efficiency_scores) - min(efficiency_scores)
            },
            'performance_insights': self.generate_performance_insights(benchmark_summary),
            'optimization_opportunities': self.identify_optimization_opportunities(benchmark_summary),
            'scaling_recommendations': benchmark_summary['performance_analysis']['scaling_recommendations']
        }
        
        # Save analysis summary
        analysis_file = f"benchmark_results/analysis_summary_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(analysis_file, 'w') as f:
            json.dump(analysis_summary, f, indent=2)
        
        print(f"   ✓ Analysis summary saved to {analysis_file}")
        return analysis_summary
    
    def generate_performance_insights(self, benchmark_summary: Dict[str, Any]) -> List[str]:
        """Generate insights about performance."""
        insights = []
        
        efficiency_scores = [item['efficiency_score'] for item in benchmark_summary['performance_ranking']]
        
        if len(efficiency_scores) >= 3:
            # Check for consistency across scales
            std_dev = statistics.stdev(efficiency_scores)
            if std_dev < 0.1:
                insights.append("✓ High consistency across scales - stable performance")
            elif std_dev < 0.3:
                insights.append("⚠ Moderate variability across scales")
            else:
                insights.append("✗ High variability across scales - need optimization")
        
        # Identify best and worst performers
        if benchmark_summary['performance_ranking']:
            best = benchmark_summary['performance_ranking'][0]
            worst = benchmark_summary['performance_ranking'][-1]
            
            insights.append(f"🏆 Best performer: {best['scale']} scale (efficiency: {best['efficiency_score']:.3f})")
            insights.append(f"📊 Worst performer: {worst['scale']} scale (efficiency: {worst['efficiency_score']:.3f})")
            
            efficiency_gap = best['efficiency_score'] - worst['efficiency_score']
            if efficiency_gap > 0.3:
                insights.append("📈 Significant performance gap - investigate scaling issues")
            elif efficiency_gap > 0.1:
                insights.append("📉 Moderate performance gap - consider optimization")
            else:
                insights.append("✅ Good performance consistency across scales")
        
        return insights
    
    def identify_optimization_opportunities(self, benchmark_summary: Dict[str, Any]) -> List[str]:
        """Identify optimization opportunities based on benchmark results."""
        opportunities = []
        
        # Check memory optimization
        for scale, results in benchmark_summary['results_summary'].items():
            memory_efficiency = 1.0 - results['memory_usage']['mean'] / 1000.0
            if memory_efficiency < 0.5:
                opportunities.append(f"📉 Memory optimization for {scale.upper()} scale")
            elif memory_efficiency < 0.8:
                opportunities.append(f"⚠ Memory improvement possible for {scale.upper()} scale")
        
        # Check CPU optimization
        for scale, results in benchmark_summary['results_summary'].items():
            cpu_efficiency = 1.0 - abs(results['cpu_usage']['mean'] - 50.0) / 100.0
            if cpu_efficiency < 0.5:
                opportunities.append(f"📉 CPU optimization for {scale.upper()} scale")
            elif cpu_efficiency < 0.8:
                opportunities.append(f"⚠ CPU improvement possible for {scale.upper()} scale")
        
        # Check SIMD optimization
        for scale, results in benchmark_summary['results_summary'].items():
            simd_utilization = results['simd_utilization']
            if simd_utilization < 0.7:
                opportunities.append(f"📉 SIMD optimization for {scale.upper()} scale")
            elif simd_utilization < 0.9:
                opportunities.append(f"⚠ SIMD improvement possible for {scale.upper()} scale")
        
        return opportunities
    
    def display_benchmark_summary(self, benchmark_summary: Dict[str, Any], analysis_summary: Dict[str, Any]):
        """Display comprehensive benchmark summary."""
        print("\n" + "=" * 80)
        print("BENCHMARK SUMMARY")
        print("=" * 80)
        
        print(f"Benchmark Suite Results:")
        print(f"  • Scales tested: {len(benchmark_summary['results_summary'])}")
        print(f"  • Overall efficiency: {benchmark_summary['performance_analysis']['best_efficiency']:.3f} (max)")
        print(f"  • Average efficiency: {benchmark_summary['performance_analysis']['best_efficiency']:.3f}")
        print(f"  • Efficiency variance: {benchmark_summary['performance_analysis']['efficiency_variance']:.3f}")
        
        print(f"\nPerformance Ranking:")
        for i, item in enumerate(benchmark_summary['performance_ranking'][:3]):  # Top 3
            medal = ["🥇", "🥈", "🥉"][i]
            print(f"  {i+1}. {item['scale']:8s} - Rank {item['performance_rank']:2d} "
                  f"(Efficiency: {item['efficiency_score']:.3f}, "
                  f"Neurons: {item['neuron_count']})")
        
        print(f"\nKey Insights:")
        for insight in benchmark_summary['performance_analysis']['insights'][:3]:  # Top 3 insights
            print(f"  {insight}")
        
        print(f"\nOptimization Opportunities:")
        for opportunity in benchmark_summary['optimization_recommendations'][:3]:  # Top 3 opportunities
            print(f"  {opportunity}")
        
        print(f"\nNext Steps:")
        print(f"  1. Review performance analysis report")
        print(f"  2. Implement optimization recommendations")
        print(f"  3. Fine-tune parameters based on best performing scale")
        print(f"  4. Run additional benchmarks for validation")
        print(f"  5. Integrate optimized configuration into production")

def main():
    """Run the performance benchmarking example."""
    print("Starting NLM Performance Benchmarking Example...")
    print("This example evaluates NLM performance across different scales")
    print("and provides comprehensive optimization recommendations.")
    print("Features include:")
    print("• Multi-scale benchmarking")
    print("• Comprehensive measurement tools")
    print("• Performance analysis and optimization")
    print("• Visualization data generation")
    print("• Detailed reporting and recommendations")
    
    try:
        benchmarker = PerformanceBenchmarker()
        results = benchmarker.run_complete_benchmark_suite()
        
        print(f"\n✓ Performance Benchmarking completed successfully!")
        print(f"Results saved to: benchmark_results/")
        print(f"Best performing scale: {results['benchmark_summary']['performance_ranking'][0]['scale'].upper()}")
        
        return results
        
    except Exception as e:
        print(f"\nError running performance benchmarking: {e}")
        print("Please ensure all required tools are installed:")
        print("• Time profiling tools")
        print("• Memory profiling tools")
        print("• CPU profiling tools")
        print("• SIMD profiling tools")
        print("• Cache analysis tools")
        return {"error": str(e)}

if __name__ == '__main__':
    try:
        result = main()
        exit(0)
    except Exception as e:
        print(f"\nUnexpected error: {e}")
        import traceback
        traceback.print_exc()
        exit(1)
