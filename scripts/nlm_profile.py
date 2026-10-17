#!/usr/bin/env python3
"""
NLM Performance Profiler
Advanced performance profiling tools for Neural Learning Machine

Features:
- Real-time performance monitoring
- Detailed timing analysis
- Resource usage tracking
- Bottleneck identification
- Performance comparison
"""

import time
import sys
import os
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
import statistics
import tracemalloc
from contextlib import contextmanager

class PerformanceProfiler:
    def __init__(self):
        self.profiling_data = {}
        self.current_session = None

    @contextmanager
    def profile(self, name: str):
        """Context manager for profiling a specific operation."""
        start_time = time.perf_counter()
        tracemalloc.start() if 'tracemalloc' in sys.modules else None
        
        try:
            yield
        finally:
            end_time = time.perf_counter()
            duration = end_time - start_time
            
            if self.current_session:
                if name not in self.profiling_data[self.current_session]:
                    self.profiling_data[self.current_session][name] = []
                self.profiling_data[self.current_session][name].append(duration)

    def start_session(self, session_name: str):
        """Start a new profiling session."""
        self.current_session = session_name
        self.profiling_data[session_name] = {}
        print(f"🚀 Started profiling session: {session_name}")

    def end_session(self):
        """End current profiling session and print summary."""
        if not self.current_session:
            print("❌ No active profiling session")
            return

        print(f"\n📊 Profiling Session Summary: {self.current_session}")
        print("=" * 50)
        
        for operation, times in self.profiling_data[self.current_session].items():
            if times:
                avg_time = statistics.mean(times)
                min_time = min(times)
                max_time = max(times)
                
                print(f"{operation}:")
                print(f"  Average: {avg_time*1000:.2f}ms")
                print(f"  Min:     {min_time*1000:.2f}ms")
                print(f"  Max:     {max_time*1000:.2f}ms")
                print(f"  Count:   {len(times)}")
        
        self.current_session = None

    def benchmark_step(self, brain, steps: int, description: str = ""):
        """Benchmark brain stepping performance."""
        if not self.current_session:
            self.start_session("brain_benchmark")
        
        with self.profile(f"step_{description}"):
            # Warm-up
            for i in range(min(100, steps // 10)):
                brain.step(i)
            
            # Actual benchmark
            for i in range(steps):
                brain.step(i)

    def profile_memory(self, brain, iterations: int = 100):
        """Profile memory usage during brain operations."""
        print(f"\n💾 Memory Profiling ({iterations} iterations)...")
        
        try:
            # Start memory tracking
            current, peak = tracemalloc.get_traced_memory()
            
            # Run operations
            for i in range(iterations):
                brain.step(i % 1000)  # Reuse step indices to save memory
            
            current, peak = tracemalloc.get_traced_memory()
            
            print(f"  Current memory usage: {current / 1024 / 1024:.2f} MB")
            print(f"  Peak memory usage:    {peak / 1024 / 1024:.2f} MB")
            
        except Exception as e:
            print(f"❌ Memory profiling error: {e}")

    def compare_configurations(self, configs: List[Dict], steps: int = 1000):
        """Compare performance across different configurations."""
        print("\n🔬 Configuration Performance Comparison")
        print("=" * 60)
        
        results = {}
        
        for i, config in enumerate(configs):
            print(f"\nConfiguration {i+1}:")
            print(f"  Parameters: {config.get('description', 'Unknown')}")
            
            try:
                # Create brain with configuration
                import pynlm
                brain_config = pynlm.createDefaultConfig()
                
                # Apply configuration parameters
                for key, value in config.items():
                    if key != 'description':
                        brain_config.set(key, value)
                
                brain = pynlm.createBrain(brain_config)
                brain.initialize()
                
                # Benchmark
                start_time = time.perf_counter()
                for step in range(steps):
                    brain.step(step)
                end_time = time.perf_counter()
                
                duration = end_time - start_time
                steps_per_sec = steps / duration
                
                results[f"config_{i+1}"] = {
                    'time': duration,
                    'steps_per_sec': steps_per_sec,
                    'neurons': brain.getTotalNeuronCount(),
                    'synapses': brain.getTotalSynapseCount(),
                    'firing': brain.getFiringNeuronCount(),
                    'spikes': brain.getTotalSpikeCount()
                }
                
                print(f"  Time: {duration:.2f}s ({steps_per_sec:.0f} steps/sec)")
                print(f"  Neurons: {brain.getTotalNeuronCount()}")
                print(f"  Synapses: {brain.getTotalSynapseCount()}")
                
            except Exception as e:
                print(f"  ❌ Error: {e}")
                results[f"config_{i+1}"] = {'error': str(e)}
        
        # Print comparison
        print(f"\n📊 Comparison Summary:")
        print(f"{'Config':<10} {'Time (s)':<10} {'Steps/sec':<12} {'Neurons':<10} {'Synapses':<10}")
        print("-" * 65)
        
        for name, result in results.items():
            if 'error' in result:
                print(f"{name:<10} {'ERROR':<10} {'N/A':<12} {'N/A':<10} {'N/A':<10}")
            else:
                print(f"{name:<10} {result['time']:<10.2f} {result['steps_per_sec']:<12.0f} "
                      f"{result['neurons']:<10} {result['synapses']:<10}")

    def export_report(self, filename: str, session_name: str = None):
        """Export profiling report to file."""
        if session_name and session_name in self.profiling_data:
            data = {session_name: self.profiling_data[session_name]}
        else:
            data = self.profiling_data
        
        with open(filename, 'w') as f:
            json.dump(data, f, indent=2)
        
        print(f"✅ Report exported to: {filename}")

class ProfilerManager:
    def __init__(self):
        self.profiler = PerformanceProfiler()

    def run_benchmark(self, args: argparse.Namespace):
        """Run comprehensive benchmark."""
        print("🚀 Running NLM Performance Benchmark")
        print("=" * 50)
        
        try:
            import pynlm
            
            # Use configuration values or defaults
            configs = []
            
            # Configuration 1: Default
            config1 = pynlm.createDefaultConfig()
            configs.append({
                'description': 'Default configuration',
                'neuron_count': config1.getOr('neuron_count', 1000),
                'synapse_density': config1.getOr('synapse_density', 0.1)
            })
            
            # Configuration 2: Small
            config2 = pynlm.createDefaultConfig()
            config2.set("neuron_count", 500)
            configs.append({
                'description': 'Small network (500 neurons)',
                'neuron_count': 500,
                'synapse_density': config2.getOr('synapse_density', 0.1)
            })
            
            # Configuration 3: Large
            config3 = pynlm.createDefaultConfig()
            config3.set("neuron_count", 2000)
            configs.append({
                'description': 'Large network (2000 neurons)',
                'neuron_count': 2000,
                'synapse_density': config3.getOr('synapse_density', 0.1)
            })
            
            self.profiler.compare_configurations(configs, steps=args.steps)
            
        except Exception as e:
            print(f"❌ Error running benchmark: {e}")

    def profile_memory_usage(self, args: argparse.Namespace):
        """Profile memory usage."""
        print("💾 Memory Profiling")
        print("=" * 30)
        
        try:
            import pynlm
            
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            self.profiler.profile_memory(brain, args.iterations)
            
        except Exception as e:
            print(f"❌ Error profiling memory: {e}")

    def start_custom_profile(self, args: argparse.Namespace):
        """Start custom profiling session."""
        if args.session:
            self.profiler.start_session(args.session)
            print(f"✅ Profiling session started: {args.session}")
        else:
            print("❌ Please specify a session name: --session <name>")

    def end_current_profile(self):
        """End current profiling session."""
        self.profiler.end_session()

    def export_profiling_data(self, args: argparse.Namespace):
        """Export profiling data."""
        if not args.file:
            print("❌ Please specify output file: --file <filename>")
            return
        
        session = args.session if args.session else None
        self.profiler.export_report(args.file, session)

def main():
    profiler_manager = ProfilerManager()

    parser = argparse.ArgumentParser(
        description="NLM Performance Profiler",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    # Run benchmark
    benchmark_parser = subparsers.add_parser("benchmark", help="Run performance benchmarks")
    benchmark_parser.add_argument("steps", type=int, nargs="?", default=1000,
                                 help="Number of steps to benchmark (default: 1000)")

    # Profile memory
    memory_parser = subparsers.add_parser("memory", help="Profile memory usage")
    memory_parser.add_argument("iterations", type=int, default=100,
                              help="Number of iterations (default: 100)")

    # Start custom profile
    profile_parser = subparsers.add_parser("start", help="Start custom profiling session")
    profile_parser.add_argument("session", help="Session name")

    # End profile
    subparsers.add_parser("end", help="End current profiling session")

    # Export data
    export_parser = subparsers.add_parser("export", help="Export profiling data")
    export_parser.add_argument("--file", required=True, help="Output filename")
    export_parser.add_argument("--session", help="Specific session to export")

    args = parser.parse_args()

    if args.command == "benchmark":
        profiler_manager.run_benchmark(args)
    elif args.command == "memory":
        profiler_manager.profile_memory_usage(args)
    elif args.command == "start":
        profiler_manager.start_custom_profile(args)
    elif args.command == "end":
        profiler_manager.end_current_profile()
    elif args.command == "export":
        profiler_manager.export_profiling_data(args)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
