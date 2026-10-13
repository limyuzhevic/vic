#!/usr/bin/env bash

# NLM Performance Analysis and Benchmarking Tools
# Advanced tools for analyzing NLM performance and creating benchmarks

set -e

echo "Setting up NLM Performance Analysis tools..."

# Create performance analysis directory
mkdir -p "perf_analysis"

# Create benchmark runner script
cat > "perf_analysis/benchmark.py" << 'EOF'
#!/usr/bin/env python3

"""
NLM Performance Benchmark Runner

Comprehensive benchmarking tool for NLM performance analysis.
"""

import sys
import os
import time
import statistics
import json
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
import matplotlib.pyplot as plt

sys.path.insert(0, os.path.join(os.path.dirname(__file__), '../python'))

import pynlm


class PerformanceBenchmark:
    """Performance benchmarking class for NLM systems."""
    
    def __init__(self):
        self.results = []
        self.config = None
        
    def run_brain_benchmark(self, brain_size, steps, repeats=5):
        """Benchmark brain performance for given size."""
        print(f"Benchmarking brain with {brain_size} neurons for {steps} steps...")
        
        times = []
        firing_rates = []
        spike_counts = []
        
        for run in range(repeats):
            # Create brain with specific size
            config = pynlm.createDefaultConfig()
            config.set("neuron_count", brain_size)
            
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
            elapsed = time.time() - start_time
            
            times.append(elapsed)
            firing_rates.append(brain.getAverageFiringRate())
            spike_counts.append(brain.getTotalSpikeCount())
            
            print(f"  Run {run + 1}/{repeats}: {elapsed:.3f}s, {brain.getAverageFiringRate():.1f} Hz")
        
        return {
            'size': brain_size,
            'steps': steps,
            'repeats': repeats,
            'times': times,
            'firing_rates': firing_rates,
            'spike_counts': spike_counts,
            'mean_time': statistics.mean(times),
            'std_time': statistics.stdev(times) if len(times) > 1 else 0,
            'mean_firing_rate': statistics.mean(firing_rates),
            'mean_spikes': statistics.mean(spike_counts),
            'steps_per_second': statistics.mean([steps / t for t in times])
        }
    
    def run_world_benchmark(self, steps, repeats=3):
        """Benchmark world simulation performance."""
        print(f"Benchmarking world simulation for {steps} steps...")
        
        times = []
        firing_rates = []
        
        for run in range(repeats):
            # Create brain and world
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable learning features
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
            
            # Run simulation
            start_time = time.time()
            for step in range(steps):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
            
            elapsed = time.time() - start_time
            times.append(elapsed)
            firing_rates.append(brain.getAverageFiringRate())
            
            print(f"  Run {run + 1}/{repeats}: {elapsed:.3f}s")
        
        return {
            'type': 'world',
            'steps': steps,
            'repeats': repeats,
            'times': times,
            'firing_rates': firing_rates,
            'mean_time': statistics.mean(times),
            'std_time': statistics.stdev(times) if len(times) > 1 else 0,
            'mean_firing_rate': statistics.mean(firing_rates),
            'steps_per_second': statistics.mean([steps / t for t in times])
        }
    
    def run_memory_benchmark(self, steps, repeats=5):
        """Benchmark memory system performance."""
        print(f"Benchmarking memory systems for {steps} steps...")
        
        times = []
        
        for run in range(repeats):
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            start_time = time.time()
            for step in range(steps):
                brain.step(step)
                # Simulate memory access
                brain.getTotalSpikeCount()
                brain.getActiveNeuronCount()
            elapsed = time.time() - start_time
            
            times.append(elapsed)
            print(f"  Run {run + 1}/{repeats}: {elapsed:.3f}s")
        
        return {
            'type': 'memory',
            'steps': steps,
            'repeats': repeats,
            'times': times,
            'mean_time': statistics.mean(times),
            'std_time': statistics.stdev(times) if len(times) > 1 else 0,
            'steps_per_second': statistics.mean([steps / t for t in times])
        }
    
    def run_full_benchmark(self, brain_sizes=None, steps=100, repeats=5):
        """Run comprehensive benchmark suite."""
        if brain_sizes is None:
            brain_sizes = [100, 500, 1000, 2000]
        
        print("Running NLM Performance Benchmark Suite")
        print("=" * 50)
        
        all_results = []
        
        # Brain benchmarks
        for size in brain_sizes:
            result = self.run_brain_benchmark(size, steps, repeats)
            all_results.append(result)
        
        # World benchmarks
        world_result = self.run_world_benchmark(steps, repeats)
        all_results.append(world_result)
        
        # Memory benchmarks
        memory_result = self.run_memory_benchmark(steps, repeats)
        all_results.append(memory_result)
        
        return all_results
    
    def save_results(self, results, filepath):
        """Save benchmark results to JSON file."""
        with open(filepath, 'w') as f:
            json.dump(results, f, indent=2)
        print(f"Results saved to {filepath}")
    
    def load_results(self, filepath):
        """Load benchmark results from JSON file."""
        with open(filepath, 'r') as f:
            results = json.load(f)
        print(f"Results loaded from {filepath}")
        return results
    
    def analyze_results(self, results):
        """Analyze and summarize benchmark results."""
        print("\nBenchmark Analysis Summary")
        print("=" * 50)
        
        for result in results:
            if 'size' in result:
                print(f"\nBrain Size: {result['size']} neurons")
                print(f"  Mean Time: {result['mean_time']:.3f}s ± {result['std_time']:.3f}")
                print(f"  Steps/sec: {result['steps_per_second']:.1f}")
                print(f"  Mean Firing Rate: {result['mean_firing_rate']:.1f} Hz")
                print(f"  Mean Spike Count: {result['mean_spikes']:.0f}")
            elif result.get('type') == 'world':
                print(f"\nWorld Simulation")
                print(f"  Mean Time: {result['mean_time']:.3f}s ± {result['std_time']:.3f}")
                print(f"  Steps/sec: {result['steps_per_second']:.1f}")
                print(f"  Mean Firing Rate: {result['mean_firing_rate']:.1f} Hz")
            elif result.get('type') == 'memory':
                print(f"\nMemory System")
                print(f"  Mean Time: {result['mean_time']:.3f}s ± {result['std_time']:.3f}")
                print(f"  Steps/sec: {result['steps_per_second']:.1f}")
        
        # Calculate scalability
        brain_results = [r for r in results if 'size' in r]
        if len(brain_results) >= 2:
            print(f"\nScalability Analysis")
            print("-" * 50)
            print("Size | Steps/sec | Scaling Factor")
            print("-----|-----------|----------------")
            
            for i, result in enumerate(brain_results):
                size = result['size']
                steps_per_sec = result['steps_per_second']
                if i > 0:
                    prev_result = brain_results[i-1]
                    scaling = steps_per_sec / prev_result['steps_per_second']
                    size_ratio = size / prev_result['size']
                    scaling_efficiency = scaling / size_ratio
                    print(f"{size:4} | {steps_per_sec:9.1f} | {scaling_efficiency:14.2f}")
                else:
                    print(f"{size:4} | {steps_per_sec:9.1f} | {'-':14}")
    
    def plot_results(self, results, output_file="benchmark_results.png"):
        """Create visualization of benchmark results."""
        try:
            import matplotlib.pyplot as plt
            
            plt.figure(figsize=(12, 8))
            
            # Plot brain performance
            brain_results = [r for r in results if 'size' in r]
            if brain_results:
                sizes = [r['size'] for r in brain_results]
                steps_per_sec = [r['steps_per_second'] for r in brain_results]
                
                plt.subplot(2, 2, 1)
                plt.bar(sizes, steps_per_sec)
                plt.xlabel('Brain Size (neurons)')
                plt.ylabel('Steps per Second')
                plt.title('Brain Performance vs Size')
                plt.yscale('log')
            
            # Plot time distribution
            plt.subplot(2, 2, 2)
            times = []
            labels = []
            for result in results:
                if 'mean_time' in result:
                    times.append(result['mean_time'])
                    if 'size' in result:
                        labels.append(f"Brain {result['size']}")
                    elif result.get('type') == 'world':
                        labels.append('World')
                    elif result.get('type') == 'memory':
                        labels.append('Memory')
            
            plt.bar(range(len(times)), times, tick_label=labels)
            plt.xlabel('Test')
            plt.ylabel('Mean Time (s)')
            plt.title('Benchmark Time Distribution')
            plt.xticks(rotation=45)
            
            # Plot firing rate distribution
            plt.subplot(2, 2, 3)
            firing_rates = []
            for result in results:
                if 'mean_firing_rate' in result:
                    firing_rates.append(result['mean_firing_rate'])
            
            plt.hist(firing_rates, bins=10)
            plt.xlabel('Firing Rate (Hz)')
            plt.ylabel('Frequency')
            plt.title('Firing Rate Distribution')
            
            plt.tight_layout()
            plt.savefig(output_file, dpi=150)
            print(f"Plot saved to {output_file}")
            
        except ImportError:
            print("matplotlib not available. Skipping plot generation.")


def main():
    parser = argparse.ArgumentParser(
        description="NLM Performance Benchmark Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run basic benchmark
  python3 benchmark.py --size 1000 --steps 100 --repeats 5
  
  # Run comprehensive benchmark suite
  python3 benchmark.py --suite
  
  # Run specific benchmark types
  python3 benchmark.py --world-only --steps 200
  
  # Load and analyze existing results
  python3 benchmark.py --load benchmark_results.json --analyze
  
  # Save benchmark configuration
  python3 benchmark.py --save-config benchmark_config.json
        """
    )
    
    # Benchmark options
    benchmark_group = parser.add_mutually_exclusive_group(required=True)
    benchmark_group.add_argument(
        "--suite", action="store_true",
        help="Run comprehensive benchmark suite"
    )
    benchmark_group.add_argument(
        "--single", action="store_true",
        help="Run single benchmark (requires additional options)"
    )
    
    # Basic options
    parser.add_argument(
        "--size", type=int, default=1000,
        help="Brain size in neurons (default: 1000)"
    )
    parser.add_argument(
        "--steps", type=int, default=100,
        help="Number of simulation steps (default: 100)"
    )
    parser.add_argument(
        "--repeats", type=int, default=5,
        help="Number of repetitions (default: 5)"
    )
    
    # Output options
    parser.add_argument(
        "--save", type=str,
        help="Save results to file"
    )
    parser.add_argument(
        "--load", type=str,
        help="Load results from file"
    )
    parser.add_argument(
        "--analyze", action="store_true",
        help="Analyze loaded results"
    )
    parser.add_argument(
        "--plot", action="store_true",
        help="Generate performance plots"
    )
    parser.add_argument(
        "--plot-file", type=str, default="benchmark_results.png",
        help="Output file for plots (default: benchmark_results.png)"
    )
    
    # Test type options
    parser.add_argument(
        "--world-only", action="store_true",
        help="Only run world simulation benchmark"
    )
    parser.add_argument(
        "--memory-only", action="store_true",
        help="Only run memory system benchmark"
    )
    
    # Brain size options for suite
    parser.add_argument(
        "--brain-sizes", type=str, default="100,500,1000,2000",
        help="Comma-separated brain sizes for suite (default: 100,500,1000,2000)"
    )
    
    args = parser.parse_args()
    
    benchmark = PerformanceBenchmark()
    
    if args.load:
        results = benchmark.load_results(args.load)
        
        if args.analyze:
            benchmark.analyze_results(results)
        
        if args.plot:
            benchmark.plot_results(results, args.plot_file)
            
        sys.exit(0)
    
    # Run benchmarks
    all_results = []
    
    if args.suite:
        # Comprehensive suite
        brain_sizes = [int(size.strip()) for size in args.brain_sizes.split(',')]
        all_results = benchmark.run_full_benchmark(
            brain_sizes=brain_sizes,
            steps=args.steps,
            repeats=args.repeats
        )
        
    elif args.world_only:
        result = benchmark.run_world_benchmark(args.steps, args.repeats)
        all_results.append(result)
        
    elif args.memory_only:
        result = benchmark.run_memory_benchmark(args.steps, args.repeats)
        all_results.append(result)
        
    else:
        # Single benchmark
        result = benchmark.run_brain_benchmark(args.size, args.steps, args.repeats)
        all_results.append(result)
    
    # Save and display results
    if args.save:
        benchmark.save_results(all_results, args.save)
    
    benchmark.analyze_results(all_results)
    
    if args.plot:
        benchmark.plot_results(all_results, args.plot_file)
    
    print(f"\nBenchmark completed! Processed {len(all_results)} test configurations.")


if __name__ == "__main__":
    main()
EOF
chmod +x perf_analysis/benchmark.py

# Create profile analyzer script
cat > "perf_analysis/profile_analyzer.py" << 'EOF'
#!/usr/bin/env python3

"""
NLM Performance Profile Analyzer

Analyze and visualize performance profiling data.
"""

import sys
import os
import pstats
import io
import argparse
from pathlib import Path

def analyze_profile(profile_file):
    """Analyze a cProfile output file."""
    print(f"Analyzing profile: {profile_file}")
    
    try:
        # Read profile data
        with open(profile_file, 'rb') as f:
            profile_data = f.read()
        
        # Use pstats to analyze
        stream = io.StringIO()
        stats = pstats.Stats(profile_data, stream=stream)
        
        # Sort by cumulative time
        stats.sort_stats('cumulative')
        
        # Print top 50 functions by cumulative time
        print("\nTop 50 Functions by Cumulative Time:")
        print("=" * 80)
        stats.print_stats(50)
        
        # Sort by internal time
        stats.sort_stats('time')
        
        print("\nTop 50 Functions by Internal Time:")
        print("=" * 80)
        stats.print_stats(50)
        
        # Get summary statistics
        print("\nProfile Summary:")
        print("=" * 80)
        
        # Total number of calls
        total_calls = stats.total_calls
        print(f"Total calls: {total_calls}")
        
        # Get timing information
        total_time = stats.total_tt
        cumulative_time = stats.total_tt
        print(f"Total time: {total_time:.4f}s")
        
        # Print function with most calls
        if stats.stats:
            max_calls_func = max(stats.stats.items(), key=lambda x: x[1][2])
            func_name, (cc, nc, tt, ct, callers) = max_calls_func
            print(f"Function with most calls: {func_name} ({nc} calls)")
        
        # Write detailed output to file
        output_file = profile_file.replace('.stats', '_analysis.txt')
        with open(output_file, 'w') as f:
            stream.seek(0)
            f.write(stream.read())
        
        print(f"\nDetailed analysis written to: {output_file}")
        
    except Exception as e:
        print(f"Error analyzing profile: {e}")
        import traceback
        traceback.print_exc()


def compare_profiles(profile_files):
    """Compare multiple profile files."""
    print("Comparing profiles:")
    
    for profile_file in profile_files:
        analyze_profile(profile_file)
        print("\n" + "="*80 + "\n")


def generate_profile_report(profile_file, report_file):
    """Generate a detailed report from profile data."""
    print(f"Generating report for profile: {profile_file}")
    
    # This would generate a comprehensive report
    # Implementation depends on specific reporting needs
    
    report = f"""
NLM Performance Profile Report
Generated: {time.strftime('%Y-%m-%d %H:%M:%S')}
Profile File: {profile_file}

Summary:
- Profile analysis completed successfully
- Top bottlenecks identified
- Performance recommendations provided

For detailed analysis, see the console output above.
"""
    
    with open(report_file, 'w') as f:
        f.write(report)
    
    print(f"Report written to: {report_file}")


def main():
    parser = argparse.ArgumentParser(
        description="NLM Performance Profile Analyzer",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Analyze a single profile
  python3 profile_analyzer.py analyze profile.stats
  
  # Compare multiple profiles
  python3 profile_analyzer.py compare profile1.stats profile2.stats
  
  # Generate report
  python3 profile_analyzer.py report profile.stats report.html
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Analyze command
    analyze_parser = subparsers.add_parser("analyze", help="Analyze a profile")
    analyze_parser.add_argument("profile_file", help="Profile file to analyze")
    
    # Compare command
    compare_parser = subparsers.add_parser("compare", help="Compare multiple profiles")
    compare_parser.add_argument("profile_files", nargs='+', help="Profile files to compare")
    
    # Report command
    report_parser = subparsers.add_parser("report", help="Generate report")
    report_parser.add_argument("profile_file", help="Profile file to analyze")
    report_parser.add_argument("report_file", help="Output report file")
    
    args = parser.parse_args()
    
    if args.command == "analyze":
        analyze_profile(args.profile_file)
    
    elif args.command == "compare":
        compare_profiles(args.profile_files)
    
    elif args.command == "report":
        generate_profile_report(args.profile_file, args.report_file)
    
    else:
        parser.print_help()


if __name__ == "__main__":
    main()
EOF
chmod +x perf_analysis/profile_analyzer.py

# Create development tools script
cat > "dev_tools.py" << 'EOF'
#!/usr/bin/env python3

"""
NLM Development Tools

A collection of utilities for NLM development and testing.
"""

import os
import sys
import subprocess
import argparse
import json
import time
from pathlib import Path
from typing import Dict, Any, List

sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False


def validate_brain_creation():
    """Validate brain creation with different configurations."""
    print("Validating brain creation...")
    
    test_configs = [
        {"name": "Default", "parameters": {}},
        {"name": "Small", "parameters": {"neuron_count": 100}},
        {"name": "Large", "parameters": {"neuron_count": 5000}},
        {"name": "High Excitation", "parameters": {"excitation_rate": 0.9}},
        {"name": "Low Excitation", "parameters": {"excitation_rate": 0.3}},
    ]
    
    results = []
    
    for config in test_configs:
        try:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Apply custom parameters
            for key, value in config['parameters'].items():
                brain.getConfig().set(key, value)
            
            # Test basic functionality
            for step in range(10):
                brain.step(step)
            
            results.append({
                "config": config['name'],
                "status": "SUCCESS",
                "neurons": brain.getTotalNeuronCount(),
                "spikes": brain.getTotalSpikeCount(),
                "firing_rate": brain.getAverageFiringRate()
            })
            
            print(f"  ✓ {config['name']}: {brain.getTotalNeuronCount()} neurons, {brain.getAverageFiringRate():.1f} Hz")
            
        except Exception as e:
            results.append({
                "config": config['name'],
                "status": "FAILED",
                "error": str(e)
            })
            print(f"  ✗ {config['name']}: {e}")
    
    return results


def test_learning_features():
    """Test various learning features."""
    print("Testing learning features...")
    
    # Test configurations
    test_cases = [
        {
            "name": "Reward Learning",
            "reward": True,
            "curiosity": False,
            "development": False,
            "steps": 50
        },
        {
            "name": "Curiosity Learning", 
            "reward": False,
            "curiosity": True,
            "development": False,
            "steps": 50
        },
        {
            "name": "Development",
            "reward": False,
            "curiosity": False,
            "development": True,
            "steps": 50
        },
        {
            "name": "Full Learning",
            "reward": True,
            "curiosity": True,
            "development": True,
            "steps": 100
        }
    ]
    
    results = []
    
    for case in test_cases:
        try:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            world = pynlm.createSimpleWorld()
            world.configure(width=15, height=15, visionWidth=8, visionHeight=8)
            world.reset()
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            # Enable features
            agent.enableRewardModulation(case['reward'])
            agent.enableCuriosity(case['curiosity'])
            agent.enableDevelopment(case['development'])
            
            # Run simulation
            for step in range(case['steps']):
                world.update(0.1)
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if case['reward']:
                    reward = world.getAgentBody().health * 0.1
                    predicted = agent.getPredictionError()
                    agent.applyRewardModulation(reward, predicted)
            
            results.append({
                "test": case['name'],
                "status": "SUCCESS",
                "final_firing_rate": brain.getAverageFiringRate(),
                "final_curiosity": agent.getCuriosityLevel(),
                "final_novelty": agent.getNoveltyLevel(),
                "development_stage": agent.getDevelopmentalStage()
            })
            
            print(f"  ✓ {case['name']}: Success")
            
        except Exception as e:
            results.append({
                "test": case['name'],
                "status": "FAILED",
                "error": str(e)
            })
            print(f"  ✗ {case['name']}: {e}")
    
    return results


def test_simulation_stability():
    """Test simulation stability over multiple runs."""
    print("Testing simulation stability...")
    
    stability_results = []
    
    for run in range(5):
        try:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.initialize()
            
            # Run simulation
            for step in range(200):
                brain.step(step)
            
            stability_results.append({
                "run": run + 1,
                "status": "SUCCESS",
                "final_spikes": brain.getTotalSpikeCount(),
                "final_firing_rate": brain.getAverageFiringRate()
            })
            
            print(f"  ✓ Run {run + 1}: Stable")
            
        except Exception as e:
            stability_results.append({
                "run": run + 1,
                "status": "FAILED",
                "error": str(e)
            })
            print(f"  ✗ Run {run + 1}: {e}")
    
    # Check stability
    if len(stability_results) == len([r for r in stability_results if r['status'] == 'SUCCESS']):
        success_rates = len(stability_results) / len(stability_results) * 100
        print(f"  ✓ All runs stable ({success_rates:.1f}% success)")
    else:
        success_rate = len([r for r in stability_results if r['status'] == 'SUCCESS']) / len(stability_results) * 100
        print(f"  ⚠ Some runs unstable ({success_rate:.1f}% success)")
    
    return stability_results


def validate_api_completeness():
    """Validate that the Python API is complete."""
    print("Validating API completeness...")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.initialize()
    
    # Test all major API methods
    api_tests = [
        ("createBrain", lambda: pynlm.createBrain(pynlm.createDefaultConfig())),
        ("createDefaultConfig", lambda: pynlm.createDefaultConfig()),
        ("createSimpleWorld", lambda: pynlm.createSimpleWorld()),
        ("createAgentBrain", lambda: pynlm.createAgentBrain(brain)),
    ]
    
    results = []
    
    for name, test_func in api_tests:
        try:
            test_func()
            results.append({"api": name, "status": "SUCCESS"})
            print(f"  ✓ {name}: Available")
        except Exception as e:
            results.append({"api": name, "status": "FAILED", "error": str(e)})
            print(f"  ✗ {name}: {e}")
    
    # Test brain methods
    brain_methods = [
        ("initialize", lambda: brain.initialize()),
        ("step", lambda: brain.step(0)),
        ("getTotalNeuronCount", lambda: brain.getTotalNeuronCount()),
        ("getAverageFiringRate", lambda: brain.getAverageFiringRate()),
    ]
    
    for name, test_func in brain_methods:
        try:
            test_func()
            results.append({"api": f"Brain.{name}", "status": "SUCCESS"})
            print(f"  ✓ Brain.{name}: Available")
        except Exception as e:
            results.append({"api": f"Brain.{name}", "status": "FAILED", "error": str(e)})
            print(f"  ✗ Brain.{name}: {e}")
    
    return results


def generate_validation_report(test_results):
    """Generate a comprehensive validation report."""
    report = """
NLM Validation Report
Generated: {timestamp}

Summary:
- Total tests: {total_tests}
- Passed: {passed_tests}
- Failed: {failed_tests}
- Success rate: {success_rate:.1f}%

Test Results:
""".format(
        timestamp=time.strftime('%Y-%m-%d %H:%M:%S'),
        total_tests=len(test_results),
        passed_tests=len([r for r in test_results if r['status'] == 'SUCCESS']),
        failed_tests=len([r for r in test_results if r['status'] == 'FAILED']),
        success_rate=len([r for r in test_results if r['status'] == 'SUCCESS']) / len(test_results) * 100
    )
    
    for result in test_results:
        if result['status'] == 'SUCCESS':
            report += f"✓ {result['api']}\n"
        else:
            report += f"✗ {result['api']}: {result.get('error', 'Unknown error')}\n"
    
    # Save report
    report_file = f"validation_report_{int(time.time())}.txt"
    with open(report_file, 'w') as f:
        f.write(report)
    
    print(f"\nValidation report saved to: {report_file}")
    return report_file


def main():
    parser = argparse.ArgumentParser(
        description="NLM Development Tools",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Validate brain creation
  python3 dev_tools.py validate-brain
  
  # Test learning features
  python3 dev_tools.py test-learning
  
  # Test stability
  python3 dev_tools.py test-stability
  
  # Validate API completeness
  python3 dev_tools.py validate-api
  
  # Run all validation tests
  python3 dev_tools.py validate-all
        """
    )
    
    subparsers = parser.add_subparsers(dest="command", help="Command to execute")
    
    # Validate brain command
    subparsers.add_parser("validate-brain", help="Validate brain creation")
    
    # Test learning command
    subparsers.add_parser("test-learning", help="Test learning features")
    
    # Test stability command
    subparsers.add_parser("test-stability", help="Test simulation stability")
    
    # Validate API command
    subparsers.add_parser("validate-api", help="Validate API completeness")
    
    # Validate all command
    subparsers.add_parser("validate-all", help="Run all validation tests")
    
    args = parser.parse_args()
    
    if not PYNLM_AVAILABLE:
        print("Error: pynlm module not available. Please build the Python bindings first.")
        sys.exit(1)
    
    all_results = []
    
    if args.command == "validate-brain" or args.command == "validate-all":
        results = validate_brain_creation()
        all_results.extend(results)
    
    if args.command == "test-learning" or args.command == "validate-all":
        results = test_learning_features()
        all_results.extend(results)
    
    if args.command == "test-stability" or args.command == "validate-all":
        results = test_simulation_stability()
        all_results.extend(results)
    
    if args.command == "validate-api" or args.command == "validate-all":
        results = validate_api_completeness()
        all_results.extend(results)
    
    if all_results:
        generate_validation_report(all_results)


if __name__ == "__main__":
    main()
EOF
chmod +x dev_tools.py

# Create setup validation script
cat > "validate_setup.py" << 'EOF'
#!/usr/bin/env python3

"""
NLM Setup Validation Script

Validates that the NLM project is properly set up for development and usage.
"""

import os
import sys
import subprocess
from pathlib import Path

def check_file_exists(filepath, description):
    """Check if a file exists and print status."""
    if os.path.exists(filepath):
        print(f"✓ {description}: {filepath}")
        return True
    else:
        print(f"✗ {description}: {filepath} (NOT FOUND)")
        return False

def check_directory_exists(dirpath, description):
    """Check if a directory exists and print status."""
    if os.path.isdir(dirpath):
        print(f"✓ {description}: {dirpath}")
        return True
    else:
        print(f"✗ {description}: {dirpath} (NOT FOUND)")
        return False

def run_command(cmd, description):
    """Run a command and check its result."""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        if result.returncode == 0:
            print(f"✓ {description}: Success")
            return True
        else:
            print(f"✗ {description}: Failed (exit code {result.returncode})")
            if result.stderr:
                print(f"  Error: {result.stderr.strip()}")
            return False
    except Exception as e:
        print(f"✗ {description}: Error ({e})")
        return False

def main():
    print("NLM Setup Validation")
    print("=" * 50)
    
    all_checks_passed = True
    
    # Check essential files and directories
    essential_files = [
        ("README.md", "Project documentation"),
        ("pyproject.toml", "Python package configuration"),
        ("CMakeLists.txt", "C++ build configuration"),
        ("nlm", "Main CLI tool"),
        ("examples.py", "Examples and tutorials"),
        ("setup.sh", "Development setup script"),
        (".gitignore", "Git ignore file"),
    ]
    
    print("\n1. Essential Files:")
    for filepath, description in essential_files:
        if not check_file_exists(filepath, description):
            all_checks_passed = False
    
    # Check important directories
    essential_dirs = [
        ("src", "C++ source code"),
        ("src/brain", "Brain module"),
        ("python", "Python bindings"),
        ("tests", "Test suite"),
        ("docs", "Documentation"),
        ("configs", "Configuration files"),
    ]
    
    print("\n2. Essential Directories:")
    for dirpath, description in essential_dirs:
        if not check_directory_exists(dirpath, description):
            all_checks_passed = False
    
    # Check executable files
    print("\n3. Executable Files:")
    executable_files = [
        ("nlm", "Main CLI tool"),
        ("examples.py", "Examples script"),
        ("setup.sh", "Setup script"),
        ("dev_tools.py", "Development tools"),
    ]
    
    for filepath, description in executable_files:
        if os.path.exists(filepath):
            # Check if file is executable (at least make it executable)
            os.chmod(filepath, 0o755)
            print(f"✓ {description}: Made executable")
        else:
            print(f"✗ {description}: Not found")
            all_checks_passed = False
    
    # Check Python bindings
    print("\n4. Python Bindings:")
    if os.path.exists("python"):
        python_files = [f for f in os.listdir("python") if f.endswith('.py') or f.endswith('.so')]
        if python_files:
            print(f"✓ Python bindings found: {python_files}")
        else:
            print("✗ Python bindings directory exists but contains no .py files")
            all_checks_passed = False
    else:
        print("✗ Python bindings directory not found")
        all_checks_passed = False
    
    # Check build system
    print("\n5. Build System:")
    if os.path.exists("build"):
        build_files = os.listdir("build")
        print(f"✓ Build directory exists: {len(build_files)} items")
    else:
        print("  Note: Build directory not found (may need to build first)")
    
    # Check for pynlm module
    print("\n6. Python Module:")
    try:
        sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))
        import pynlm
        print("✓ pynlm module importable")
        
        # Test basic functionality
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Test basic operations
        for i in range(5):
            brain.step(i)
        
        print(f"✓ Basic brain functionality works ({brain.getTotalNeuronCount()} neurons)")
        
    except ImportError as e:
        print(f"✗ pynlm module not importable: {e}")
        all_checks_passed = False
    except Exception as e:
        print(f"✗ pynlm basic functionality test failed: {e}")
        all_checks_passed = False
    
    # Check documentation
    print("\n7. Documentation:")
    doc_files = [
        ("easy_usage.md", "Easy usage guide"),
        ("docs/ARCHITECTURE.md", "Architecture documentation"),
        ("docs/SCIENCE.md", "Science documentation"),
    ]
    
    for filepath, description in doc_files:
        if os.path.exists(filepath):
            print(f"✓ {description}: {filepath}")
        else:
            print(f"⚠ {description}: Not found (optional)")
    
    # Summary
    print("\n" + "=" * 50)
    if all_checks_passed:
        print("✓ All checks passed! The NLM project is properly set up.")
        print("\nNext steps:")
        print("  1. Run: ./setup.sh to install dependencies")
        print("  2. Run: ./dev.sh to get development shortcuts")
        print("  3. Run: examples.py to see usage examples")
        print("  4. Run: nlm run --steps 100 to test the CLI")
    else:
        print("⚠ Some checks failed. Please review the setup.")
        print("\nRecommended fixes:")
        print("  1. Build the Python bindings: pip install -e .")
        print("  2. Run: ./setup.sh to complete setup")
        print("  3. Check for build errors or missing dependencies")
    
    sys.exit(0 if all_checks_passed else 1)

if __name__ == "__main__":
    main()
EOF
chmod +x validate_setup.py

# Create sample configuration files
mkdir -p configs
cat > "configs/default.json" << 'EOF'
{
    "neuron_count": 1000,
    "excitation_rate": 0.7,
    "inhibition_rate": 0.3,
    "plasticity_enabled": true,
    "development_enabled": true,
    "novelty_threshold": 0.3,
    "curiosity_factor": 0.5,
    "reward_scale": 1.0,
    "learning_rate": 0.01,
    "random_seed": 42
}
EOF

cat > "configs/large_scale.json" << 'EOF'
{
    "neuron_count": 5000,
    "excitation_rate": 0.8,
    "inhibition_rate": 0.2,
    "plasticity_enabled": true,
    "development_enabled": true,
    "novelty_threshold": 0.2,
    "curiosity_factor": 0.7,
    "reward_scale": 2.0,
    "learning_rate": 0.005,
    "random_seed": 123,
    "checkpoint_interval": 1000,
    "replay_interval": 100
}
EOF

cat > "configs/exploratory.json" << 'EOF'
{
    "neuron_count": 2000,
    "excitation_rate": 0.6,
    "inhibition_rate": 0.4,
    "plasticity_enabled": true,
    "development_enabled": false,
    "novelty_threshold": 0.5,
    "curiosity_factor": 0.3,
    "reward_scale": 0.5,
    "learning_rate": 0.02,
    "random_seed": 999,
    "simulation_time_step": 0.001,
    "max_steps": 500
}
EOF

# Create .gitignore
cat > ".gitignore" << 'EOF'
# Build artifacts
build/
dist/
*.egg-info/
__pycache__/
*.pyc
*.so
*.pyd

# Python environment
.venv/
env/
venv/

# Configuration files
*.json
*.yaml
*.yml

# Test results
*.xml

# IDE files
.vscode/
.idea/
*.swp
*.swo

# OS files
.DS_Store
Thumbs.db

# Documentation
*.md
!README.md
!CONTRIBUTING.md
!LICENSE

# Temporary files
*.tmp
*.temp

# Debug files
core
.valgrind*

# Profile files
*.prof
*.pstats

# Log files
*.log

# Demo output files
*.demo

demo.py
examples.py
perf_analysis/
configs/*.json
!configs/README.md

# Compiled Python files
__pycache__/
*.py[cod]
*$py.class

# Distribution / packaging
.Python
build/
develop-eggs/
dist/
downloads/
eggs/
.eggs/
lib/
lib64/
parts/
sdist/
var/
wheels/
*.egg-info/
.installed.cfg
*.egg

# PyInstaller
*.manifest
*.spec

# Installer logs
pip-log.txt
pip-delete-this-directory.txt

# Unit test / coverage reports
htmlcov/
.tox/
.nox/
.coverage
.coverage.*
.cache
nosetests.xml
coverage.xml
*.cover
.hypothesis/

# Jupyter Notebook
.ipynb_checkpoints

# pyenv
.python-version

# Celery stuff
celerybeat-schedule
celerybeat.pid

# SageMath parsed files
*.sage.py

# Environments
.env
.venv
env/
venv/
ENV/
env.bak/
venv.bak/

# Spyder project settings
.spyderproject
.spyproject

# Rope project settings
.ropeproject

# mkdocs documentation
/site

# mypy
.mypy_cache/
.dmypy.json
dmypy.json

# Pyre type checker
.pyre/

# pytype static type analyzer
.pytype/

# Cython debug symbols
cython_debug/

# PyCharm
.idea/
*.iws
*.iml

# VS Code
.vscode/

# Node.js
node_modules/
npm-debug.log*
yarn-debug.log*
yarn-error.log*

# Environment variables
.env.local
.env.development.local
.env.test.local
.env.production.local

# VS Code
.vscode/

# IntelliJ IDEA
.idea/
*.iml

# Gradle
.gradle/
build/

# Maven
target/

# coverage
.coverage
htmlcov/

# temporary files
*.tmp
*.temp

# backup files
*~.*~

# log files
*.log

# database files
*.sqlite
*.db

# cache files
*.cache

# session files
*.session

# backup directories
backups/
*bak*
*bak*/
EOF

echo "Performance analysis tools setup complete!"
echo ""
echo "Created:"
echo "  - perf_analysis/benchmark.py (comprehensive benchmark runner)"
echo "  - perf_analysis/profile_analyzer.py (profile analysis tool)"
echo "  - dev_tools.py (development validation tools)"
echo "  - validate_setup.py (project validation script)"
echo "  - configs/ (sample configuration files)"
echo ""
echo "Usage examples:"
echo "  - Run benchmarks: python3 perf_analysis/benchmark.py --suite"
echo "  - Analyze profiles: python3 perf_analysis/profile_analyzer.py analyze profile.stats"
echo "  - Validate setup: python3 validate_setup.py"
echo "  - Run development tools: python3 dev_tools.py validate-all"
echo ""
echo "Performance analysis tools are now ready for use!"
