#!/usr/bin/env python3
"""
Benchmarking and performance testing for NLM

Provides tools for benchmarking NLM performance, comparing configurations,
and analyzing brain simulation speed and efficiency.
"""

import time
import statistics
from typing import List, Dict, Any, Optional
import sys
import os

# Add the Python module to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

try:
    import pynlm
except ImportError as e:
    print(f"Error: Could not import pynlm: {e}")
    sys.exit(1)


class NLMBenchmark:
    """Benchmark suite for NLM performance testing."""
    
    def __init__(self):
        self.results = []
    
    def benchmark_configuration(self, neurons: int, steps: int, 
                              regions: int = 1, variance: float = 0.1,
                              description: str = "") -> Dict[str, Any]:
        """Benchmark a specific NLM configuration.
        
        Args:
            neurons: Number of neurons
            steps: Number of simulation steps
            regions: Number of brain regions
            variance: Connection probability
            description: Description of the configuration
            
        Returns:
            Dictionary with benchmark results
        """
        print(f"Benchmarking: {description or f'{neurons} neurons, {steps} steps'}")
        
        # Create configuration
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neurons)
        config.set("brain.region_count", regions)
        config.set("brain.connection_probability", variance)
        
        # Create brain
        start_time = time.time()
        brain = pynlm.createBrain(config)
        brain_initialize_time = time.time() - start_time
        
        if not brain.initialize():
            raise NLMBenchmarkError("Failed to initialize brain")
        
        # Run simulation
        start_time = time.time()
        for step in range(steps):
            brain.step(step)
        simulation_time = time.time() - start_time
        
        # Collect statistics
        total_spikes = brain.getTotalSpikeCount()
        total_steps = steps
        spikes_per_step = total_spikes / total_steps if total_steps > 0 else 0
        
        # Calculate derived metrics
        steps_per_second = total_steps / simulation_time if simulation_time > 0 else 0
        spikes_per_second = total_spikes / simulation_time if simulation_time > 0 else 0
        average_firing_rate = brain.getAverageFiringRate()
        
        result = {
            'description': description,
            'neurons': neurons,
            'regions': regions,
            'variance': variance,
            'steps': steps,
            'initialize_time': brain_initialize_time,
            'simulation_time': simulation_time,
            'total_spikes': total_spikes,
            'spikes_per_step': spikes_per_step,
            'steps_per_second': steps_per_second,
            'spikes_per_second': spikes_per_second,
            'average_firing_rate': average_firing_rate,
            'efficiency': spikes_per_step / (brain.getTotalSynapseCount() / neurons) if neurons > 0 else 0,
        }
        
        self.results.append(result)
        
        print(f"  Initialization: {brain_initialize_time:.3f}s")
        print(f"  Simulation: {simulation_time:.3f}s ({steps_per_second:.1f} steps/sec)")
        print(f"  Total spikes: {total_spikes} ({spikes_per_step:.3f} per step)")
        
        return result
    
    def compare_configurations(self, configs: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Compare multiple configurations.
        
        Args:
            configs: List of configuration dictionaries with keys:
                - neurons: Number of neurons
                - steps: Number of simulation steps
                - regions: Number of regions (optional)
                - variance: Connection probability (optional)
                - description: Description (optional)
                
        Returns:
            Comparison results
        """
        print("Comparing configurations...")
        print("=" * 80)
        
        comparison_results = []
        
        for config in configs:
            result = self.benchmark_configuration(**config)
            comparison_results.append(result)
        
        print("\n" + "=" * 80)
        print("COMPARISON SUMMARY")
        print("=" * 80)
        
        # Sort by speed (steps per second)
        sorted_results = sorted(comparison_results, key=lambda x: x['steps_per_second'], reverse=True)
        
        for i, result in enumerate(sorted_results):
            print(f"{i+1}. {result['description']}")
            print(f"   Neurons: {result['neurons']:,} | Steps: {result['steps']:,}")
            print(f"   Speed: {result['steps_per_second']:,.1f} steps/sec")
            print(f"   Spikes/sec: {result['spikes_per_second']:,.1f}")
            print(f"   Avg firing rate: {result['average_firing_rate']:.3f}")
            print()
        
        # Calculate statistics
        if comparison_results:
            avg_speed = statistics.mean([r['steps_per_second'] for r in comparison_results])
            avg_spikes_per_second = statistics.mean([r['spikes_per_second'] for r in comparison_results])
            avg_efficiency = statistics.mean([r['efficiency'] for r in comparison_results])
            
            print("STATISTICS (averages):")
            print(f"  Average speed: {avg_speed:,.1f} steps/sec")
            print(f"  Average spikes/sec: {avg_spikes_per_second:,.1f}")
            print(f"  Average efficiency: {avg_efficiency:.6f}")
            
            comparison_stats = {
                'configurations_tested': len(comparison_results),
                'average_speed': avg_speed,
                'average_spikes_per_second': avg_spikes_per_second,
                'average_efficiency': avg_efficiency,
                'results': sorted_results,
            }
            
            return comparison_stats
        
        return {'configurations_tested': 0, 'results': []}
    
    def generate_report(self, output_file: Optional[str] = None) -> str:
        """Generate a benchmark report.
        
        Args:
            output_file: File path to save report (optional)
            
        Returns:
            Report string
        """
        if not self.results:
            return "No benchmark results available."
        
        report_lines = []
        report_lines.append("NLM BENCHMARK REPORT")
        report_lines.append("=" * 80)
        report_lines.append(f"Total configurations tested: {len(self.results)}")
        report_lines.append(f"Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}")
        report_lines.append()
        
        # Summary statistics
        speeds = [r['steps_per_second'] for r in self.results]
        spike_rates = [r['spikes_per_second'] for r in self.results]
        efficiencies = [r['efficiency'] for r in self.results]
        
        report_lines.append("PERFORMANCE SUMMARY")
        report_lines.append("-" * 80)
        report_lines.append(f"Speed: {statistics.mean(speeds):,.1f} ± {statistics.stdev(speeds):,.1f} steps/sec")
        report_lines.append(f"Spike rate: {statistics.mean(spike_rates):,.1f} ± {statistics.stdev(spike_rates):,.1f} spikes/sec")
        report_lines.append(f"Efficiency: {statistics.mean(efficiencies):.6f} ± {statistics.stdev(efficiencies):.6f}")
        report_lines.append()
        
        # Detailed results
        report_lines.append("DETAILED RESULTS")
        report_lines.append("-" * 80)
        report_lines.append(f"{'Description':<30} {'Neurons':>10} {'Steps':>10} {'Time(s)':>10} {'Speed':>15} {'Spikes/sec':>15}")
        report_lines.append("-" * 80)
        
        for result in self.results:
            report_lines.append(f"{result['description']:<30} "
                             f"{result['neurons']:>10,} "
                             f"{result['steps']:>10,} "
                             f"{result['simulation_time']:>10.3f} "
                             f"{result['steps_per_second']:>15,.1f} "
                             f"{result['spikes_per_second']:>15,.1f}")
        
        report = "\n".join(report_lines)
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report)
            print(f"Report saved to: {output_file}")
        
        return report
    
    def clear_results(self):
        """Clear all benchmark results."""
        self.results.clear()


class NLMBenchmarkError(Exception):
    """Exception raised by NLM benchmarking."""
    pass


def quick_benchmark():
    """Quick benchmark with common configurations."""
    print("Running quick benchmark...")
    print("This will test various NLM configurations and provide performance metrics.")
    print()
    
    benchmark = NLMBenchmark()
    
    # Common configurations to test
    configs = [
        {'neurons': 100, 'steps': 100, 'regions': 1, 'variance': 0.1, 
         'description': 'Small: 100 neurons'},
        {'neurons': 1000, 'steps': 1000, 'regions': 1, 'variance': 0.1, 
         'description': 'Medium: 1K neurons'},
        {'neurons': 5000, 'steps': 1000, 'regions': 4, 'variance': 0.05, 
         'description': 'Large: 5K neurons, 4 regions'},
        {'neurons': 10000, 'steps': 1000, 'regions': 8, 'variance': 0.02, 
         'description': 'Very Large: 10K neurons, 8 regions'},
    ]
    
    try:
        comparison = benchmark.compare_configurations(configs)
        
        # Generate report
        report = benchmark.generate_report("nlm_benchmark_report.txt")
        
        print(f"\nBenchmark complete! {comparison['configurations_tested']} configurations tested.")
        print("Report saved to: nlm_benchmark_report.txt")
        
        return benchmark
        
    except Exception as e:
        print(f"Benchmark failed: {e}")
        return None


if __name__ == "__main__":
    """Command-line interface for benchmarking."""
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Benchmarking and Performance Testing",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python nlm_benchmark.py                    # Quick benchmark with common configs
  python nlm_benchmark.py --custom-configs    # Use custom configurations
  python nlm_benchmark.py --report report.txt  # Generate report to file
        """
    )
    
    parser.add_argument("--custom-configs", action="store_true",
                       help="Use custom configurations instead of quick benchmark")
    parser.add_argument("--report", help="Save report to specified file")
    
    args = parser.parse_args()
    
    if args.custom_configs:
        print("Custom configuration mode")
        print("To use this, create a configuration file or specify parameters directly.")
        print("This feature is under development.")
    else:
        benchmark = quick_benchmark()
        if benchmark and args.report:
            report = benchmark.generate_report(args.report)
            print(f"\nReport also saved to: {args.report}")
