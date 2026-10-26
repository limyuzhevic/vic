#!/usr/bin/env python3
"""
NLM Performance Benchmark Suite

This script provides benchmarking capabilities for the NLM framework,
allowing performance measurement across different scales and configurations.
"""

import argparse
import json
import os
import sys
import time
import yaml
import csv
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional

# Import NLM modules
import nlm
from nlm import Config, Brain, createBrain, createDefaultConfig

class NLMBenchmark:
    """Main benchmarking class for NLM performance testing."""
    
    def __init__(self, config_file: str = "benchmark_config.yaml"):
        self.config_file = config_file
        self.benchmark_config = self._load_config()
        self.results = {}
        self.start_time = None
        self.end_time = None
    
    def _load_config(self) -> Dict:
        """Load benchmark configuration from YAML file."""
        if os.path.exists(self.config_file):
            with open(self.config_file, 'r') as f:
                return yaml.safe_load(f)
        else:
            # Default configuration
            return self._get_default_config()
    
    def _get_default_config(self) -> Dict:
        """Get default benchmark configuration."""
        return {
            'benchmarks': {
                'micro': {
                    'neurons': 1000,
                    'steps': 10000,
                    'timestep': 0.001,
                    'sparsity': 0.1,
                    'connectivity': 'random'
                },
                'small': {
                    'neurons': 10000,
                    'steps': 20000,
                    'timestep': 0.001,
                    'sparsity': 0.05,
                    'connectivity': 'random'
                },
                'medium': {
                    'neurons': 100000,
                    'steps': 50000,
                    'timestep': 0.001,
                    'sparsity': 0.01,
                    'connectivity': 'distance'
                },
                'large': {
                    'neurons': 1000000,
                    'steps': 100000,
                    'timestep': 0.001,
                    'sparsity': 0.005,
                    'connectivity': 'distance'
                }
            },
            'performance_metrics': {
                'measure': [
                    'neurons_per_second',
                    'spikes_per_second',
                    'real_time_factor',
                    'memory_usage',
                    'energy_efficiency'
                ],
                'targets': {
                    'micro': 10000,
                    'small': 5000,
                    'medium': 1000,
                    'large': 100
                }
            },
            'hardware': {
                'cores': os.cpu_count() or 8,
                'memory_limit_gb': 16,
                'benchmark_timeout_minutes': 60
            }
        }
    
    def run_benchmark(self, scale: str, output_dir: str = "results") -> Dict:
        """Run a benchmark for the specified scale."""
        print(f"\n=== Running NLM Benchmark: {scale.upper()} ===")
        
        if scale not in self.benchmark_config['benchmarks']:
            raise ValueError(f"Unknown benchmark scale: {scale}")
        
        scale_config = self.benchmark_config['benchmarks'][scale]
        print(f"Configuration: {scale_config}")
        
        # Create benchmark directory
        benchmark_dir = Path(output_dir) / scale
        benchmark_dir.mkdir(parents=True, exist_ok=True)
        
        # Initialize results
        results = {
            'scale': scale,
            'neurons': scale_config['neurons'],
            'steps': scale_config['steps'],
            'timestep': scale_config['timestep'],
            'sparsity': scale_config['sparsity'],
            'connectivity': scale_config['connectivity'],
            'timestamp': datetime.now().isoformat(),
            'config': scale_config
        }
        
        # Start timing
        self.start_time = time.time()
        
        try:
            # Run the actual benchmark
            benchmark_results = self._execute_benchmark(scale_config)
            results.update(benchmark_results)
            
        except Exception as e:
            print(f"Benchmark failed: {e}")
            results['error'] = str(e)
            results['success'] = False
            
        # Calculate end time and finalize results
        self.end_time = time.time()
        results['wall_clock_time'] = self.end_time - self.start_time
        results['success'] = True
        
        # Save results
        self._save_results(benchmark_dir, results)
        
        # Print summary
        self._print_summary(results)
        
        return results
    
    def _execute_benchmark(self, config: Dict) -> Dict:
        """Execute the actual NLM benchmark."""
        print("Creating configuration...")
        
        # Create configuration
        cfg = createDefaultConfig()
        cfg.set("brain.neuron_count", config['neurons'])
        cfg.set("brain.synapse_density", config['sparsity'])
        cfg.set("brain.connection_probability", config['sparsity'])
        
        print("Creating and initializing brain...")
        
        # Create brain
        brain = createBrain(cfg)
        brain.initialize()
        
        # Initialize metrics
        metrics = {
            'neurons_processed': 0,
            'spikes_processed': 0,
            'steps_completed': 0,
            'total_reward': 0.0,
            'avg_firing_rate': 0.0,
            'firing_neuron_count': 0,
            'avg_synaptic_weight': 0.0,
            'memory_episodes_stored': 0,
            'novelty_level': 0.0,
            'curiosity_level': 0.0,
            'dopamine_level': 0.0
        }
        
        # Run simulation
        print(f"Running simulation for {config['steps']} steps...")
        
        for step in range(config['steps']):
            # Record step start time
            step_start = time.time()
            
            # Run brain step
            brain.step(step)
            
            # Collect metrics
            metrics['steps_completed'] = step + 1
            metrics['neurons_processed'] += brain.getTotalNeuronCount()
            metrics['spikes_processed'] += brain.getTotalSpikeCount()
            metrics['total_reward'] += brain.getAverageFiringRate()
            metrics['firing_neuron_count'] = brain.getFiringNeuronCount()
            
            # Calculate running averages
            metrics['avg_firing_rate'] = metrics['total_reward'] / metrics['steps_completed']
            metrics['avg_synaptic_weight'] = 0.5  # Placeholder
            
            # Memory and neuromodulation (if available)
            if hasattr(brain, 'getEpisodicMemory'):
                episodic_mem = brain.getEpisodicMemory()
                if episodic_mem:
                    metrics['memory_episodes_stored'] = episodic_mem.getEpisodeCount()
            
            if hasattr(brain, 'getCuriosity'):
                curiosity = brain.getCuriosity()
                if curiosity:
                    metrics['curiosity_level'] = curiosity.getCuriosityLevel()
            
            if hasattr(brain, 'getNovelty'):
                novelty = brain.getNovelty()
                if novelty:
                    metrics['novelty_level'] = novelty.getNoveltyLevel()
            
            if hasattr(brain, 'getDopamine'):
                dopamine = brain.getDopamine()
                if dopamine:
                    metrics['dopamine_level'] = dopamine.getLevel()
            
            # Log progress every 1000 steps
            if (step + 1) % 1000 == 0:
                print(f"  Step {step + 1}/{config['steps']}: "
                      f"{brain.getFiringNeuronCount()} firing neurons, "
                      f"{brain.getTotalSpikeCount()} spikes")
        
        # Calculate final performance metrics
        performance = self._calculate_performance_metrics(metrics, config)
        
        return {
            'metrics': metrics,
            'performance': performance,
            'hardware_info': self._get_hardware_info()
        }
    
    def _calculate_performance_metrics(self, metrics: Dict, config: Dict) -> Dict:
        """Calculate derived performance metrics."""
        wall_time = self.end_time - self.start_time
        
        performance = {
            'wall_clock_time': wall_time,
            'steps_per_second': metrics['steps_completed'] / wall_time if wall_time > 0 else 0,
            'neurons_per_second': metrics['neurons_processed'] / wall_time if wall_time > 0 else 0,
            'spikes_per_second': metrics['spikes_processed'] / wall_time if wall_time > 0 else 0,
            'real_time_factor': wall_time / (config['steps'] * config['timestep']) if config['steps'] * config['timestep'] > 0 else 0,
            'memory_efficiency': config['neurons'] / (metrics['neurons_processed'] / 1024 / 1024) if metrics['neurons_processed'] > 0 else 0,
            'firing_rate_avg': metrics['avg_firing_rate'],
            'firing_rate_max': metrics['firing_neuron_count'] / config['neurons'] if config['neurons'] > 0 else 0,
            'synapse_efficiency': config['neurons'] * config['sparsity'] * config['steps'] / wall_time if wall_time > 0 else 0
        }
        
        return performance
    
    def _get_hardware_info(self) -> Dict:
        """Get hardware information."""
        import platform
        return {
            'platform': platform.platform(),
            'python_version': platform.python_version(),
            'cpu_count': os.cpu_count(),
            'memory_available': self._get_available_memory()
        }
    
    def _get_available_memory(self) -> float:
        """Get available memory in GB."""
        try:
            import psutil
            return psutil.virtual_memory().available / (1024**3)
        except ImportError:
            return 0.0
    
    def _save_results(self, benchmark_dir: Path, results: Dict):
        """Save benchmark results to files."""
        # Save JSON results
        json_file = benchmark_dir / "results.json"
        with open(json_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        # Save CSV metrics
        csv_file = benchmark_dir / "metrics.csv"
        with open(csv_file, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['metric', 'value'])
            
            # Add performance metrics
            if 'performance' in results:
                for key, value in results['performance'].items():
                    writer.writerow([f'perf_{key}', value])
            
            # Add simulation metrics
            if 'metrics' in results:
                for key, value in results['metrics'].items():
                    writer.writerow([f'sim_{key}', value])
        
        # Save summary report
        summary_file = benchmark_dir / "summary.txt"
        with open(summary_file, 'w') as f:
            f.write(self._generate_summary(results))
    
    def _generate_summary(self, results: Dict) -> str:
        """Generate a human-readable summary of benchmark results."""
        summary = []
        summary.append("=== NLM Performance Benchmark Summary ===")
        summary.append(f"Scale: {results['scale']}")
        summary.append(f"Neurons: {results.get('neurons', 'N/A')}")
        summary.append(f"Steps: {results.get('steps', 'N/A')}")
        summary.append(f"Wall Clock Time: {results.get('wall_clock_time', 0):.2f} seconds")
        
        if 'performance' in results:
            perf = results['performance']
            summary.append("\nPerformance Metrics:")
            summary.append(f"  Steps/sec: {perf.get('steps_per_second', 0):.0f}")
            summary.append(f"  Neurons/sec: {perf.get('neurons_per_second', 0):.0f}")
            summary.append(f"  Spikes/sec: {perf.get('spikes_per_second', 0):.0f}")
            summary.append(f"  Real-time factor: {perf.get('real_time_factor', 0):.2f}")
            summary.append(f"  Memory efficiency: {perf.get('memory_efficiency', 0):.2f}")
        
        if 'error' in results:
            summary.append(f"\nERROR: {results['error']}")
        
        return "\n".join(summary)
    
    def _print_summary(self, results: Dict):
        """Print benchmark summary to console."""
        print("\n=== Benchmark Results ===")
        
        if 'performance' in results:
            perf = results['performance']
            print(f"Wall Clock Time: {results.get('wall_clock_time', 0):.2f} seconds")
            print(f"Steps/sec: {perf.get('steps_per_second', 0):.0f}")
            print(f"Neurons/sec: {perf.get('neurons_per_second', 0):.0f}")
            print(f"Spikes/sec: {perf.get('spikes_per_second', 0):.0f}")
            print(f"Real-time factor: {perf.get('real_time_factor', 0):.2f}")
        
        print(f"Results saved to: results/{results['scale']}/")
    
    def run_all_benchmarks(self, scales: List[str] = None, output_dir: str = "results") -> Dict:
        """Run all benchmarks for specified scales."""
        if scales is None:
            scales = list(self.benchmark_config['benchmarks'].keys())
        
        all_results = {}
        
        for scale in scales:
            try:
                result = self.run_benchmark(scale, output_dir)
                all_results[scale] = result
            except Exception as e:
                print(f"Failed to run benchmark for {scale}: {e}")
                all_results[scale] = {'error': str(e), 'success': False}
        
        # Generate comparison report
        self._generate_comparison_report(all_results, output_dir)
        
        return all_results
    
    def _generate_comparison_report(self, all_results: Dict, output_dir: str):
        """Generate a comparison report across all benchmarks."""
        report_file = Path(output_dir) / "comparison_report.md"
        
        with open(report_file, 'w') as f:
            f.write("# NLM Performance Benchmark Comparison Report\n\n")
            f.write(f"Generated: {datetime.now().isoformat()}\n\n")
            
            for scale, result in all_results.items():
                f.write(f"## {scale.upper()} Benchmark\n\n")
                
                if 'error' in result:
                    f.write(f"**ERROR**: {result['error']}\n\n")
                    continue
                
                f.write(f"- **Neurons**: {result.get('neurons', 'N/A')}\n")
                f.write(f"- **Steps**: {result.get('steps', 'N/A')}\n")
                f.write(f"- **Wall Clock Time**: {result.get('wall_clock_time', 0):.2f} seconds\n")
                
                if 'performance' in result:
                    perf = result['performance']
                    f.write(f"- **Steps/sec**: {perf.get('steps_per_second', 0):.0f}\n")
                    f.write(f"- **Neurons/sec**: {perf.get('neurons_per_second', 0):.0f}\n")
                    f.write(f"- **Real-time factor**: {perf.get('real_time_factor', 0):.2f}\n\n")
        
        print(f"Comparison report saved to: {report_file}")


def main():
    """Main function for command-line interface."""
    parser = argparse.ArgumentParser(
        description="NLM Performance Benchmark Suite"
    )
    
    parser.add_argument(
        'action',
        choices=['run', 'all', 'profile'],
        help='Benchmark action to perform'
    )
    
    parser.add_argument(
        '--scale',
        choices=['micro', 'small', 'medium', 'large'],
        help='Specific scale to benchmark'
    )
    
    parser.add_argument(
        '--output-dir',
        default='results',
        help='Output directory for benchmark results'
    )
    
    parser.add_argument(
        '--config',
        default='benchmark_config.yaml',
        help='Benchmark configuration file'
    )
    
    parser.add_argument(
        '--scales',
        nargs='+',
        choices=['micro', 'small', 'medium', 'large'],
        help='Scales to run (for --action all)'
    )
    
    args = parser.parse_args()
    
    # Initialize benchmark suite
    benchmark = NLMBenchmark(args.config)
    
    # Run specified action
    if args.action == 'run':
        if not args.scale:
            parser.error("--scale is required for --action run")
        
        result = benchmark.run_benchmark(args.scale, args.output_dir)
        sys.exit(0 if result.get('success', False) else 1)
    
    elif args.action == 'all':
        scales = args.scales or ['micro', 'small', 'medium']
        results = benchmark.run_all_benchmarks(scales, args.output_dir)
        
        # Check if all benchmarks succeeded
        success = all(result.get('success', False) for result in results.values())
        sys.exit(0 if success else 1)
    
    elif args.action == 'profile':
        print("Profiling mode not yet implemented")
        sys.exit(1)


if __name__ == '__main__':
    main()
