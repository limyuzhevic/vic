#!/usr/bin/env python3
"""
NLM Performance Profiler CLI - Benchmarking and profiling brain performance.

This tool provides command-line access to performance benchmarking,
profiling, and analysis of NLM brain performance characteristics.
"""

import argparse
import json
import logging
import time
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional

# Add parent directory to path for pynlm import
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import pynlm
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    sys.exit(1)


def benchmark_creation(args: argparse.Namespace) -> None:
    """Benchmark brain creation performance."""
    logger = logging.getLogger("performance_profiler")
    
    # Parse benchmark parameters
    num_brains = int(args.num_brains) if args.num_brains else 5
    warmup_steps = int(args.warmup_steps) if args.warmup_steps else 10
    
    logger.info(f"Benchmarking brain creation: {num_brains} brains, {warmup_steps} warm-up steps")
    
    creation_times = []
    initialization_times = []
    step_times = []
    
    for i in range(num_brains):
        start_time = time.time()
        config = pynlm.createDefaultConfig()
        create_end_time = time.time()
        creation_times.append(create_end_time - start_time)
        
        brain = pynlm.createBrain(config)
        init_start_time = time.time()
        brain.initialize()
        init_end_time = time.time()
        initialization_times.append(init_end_time - init_start_time)
        
        # Warm-up steps
        for j in range(warmup_steps):
            brain.step(j)
        
        # Benchmark step performance
        step_start_time = time.time()
        for j in range(args.benchmark_steps if args.benchmark_steps else 100):
            brain.step(j + warmup_steps)
        step_end_time = time.time()
        step_times.append(step_end_time - step_start_time)
        
        logger.debug(f"Brain {i+1}/{num_brains} completed")
    
    # Calculate statistics
    def calc_stats(times):
        return {
            "mean": sum(times) / len(times),
            "median": sorted(times)[len(times)//2],
            "min": min(times),
            "max": max(times),
            "stddev": (sum((x - sum(times)/len(times))**2 for x in times) / len(times))**0.5,
            "total": sum(times)
        }
    
    creation_stats = calc_stats(creation_times)
    init_stats = calc_stats(initialization_times)
    step_stats = calc_stats(step_times)
    
    benchmark_results = {
        "benchmark_type": "brain_creation_and_performance",
        "parameters": {
            "num_brains": num_brains,
            "warmup_steps": warmup_steps,
            "benchmark_steps": args.benchmark_steps if args.benchmark_steps else 100
        },
        "results": {
            "creation": creation_stats,
            "initialization": init_stats,
            "step_performance": step_stats
        },
        "summary": {
            "total_time_seconds": creation_stats["total"] + init_stats["total"] + step_stats["total"],
            "creation_per_brain_ms": creation_stats["mean"] * 1000,
            "initialization_per_brain_ms": init_stats["mean"] * 1000,
            "steps_per_second": num_brains * (args.benchmark_steps if args.benchmark_steps else 100) / step_stats["total"]
        }
    }
    
    if args.json:
        json.dump(benchmark_results, sys.stdout, indent=2)
    else:
        print(f"\n=== Brain Creation Benchmark Results ===")
        print(f"Benchmark Type: Brain Creation and Performance")
        print(f"Total Time: {benchmark_results['summary']['total_time_seconds']:.2f}s")
        print(f"Creation Time (mean): {creation_stats['mean']*1000:.2f}ms per brain")
        print(f"Initialization Time (mean): {init_stats['mean']*1000:.2f}ms per brain")
        print(f"Step Performance: {benchmark_results['summary']['steps_per_second']:.1f} steps/sec")
        print(f"Creation Time (stddev): {creation_stats['stddev']*1000:.2f}ms")
        print(f"Step Time (mean): {step_stats['mean']/args.benchmark_steps*1000 if args.benchmark_steps else step_stats['mean']*1000:.2f}ms per step")


def profile_memory_usage(args: argparse.Namespace) -> None:
    """Profile memory usage of brain operations."""
    logger = logging.getLogger("performance_profiler")
    
    # Parse profile parameters
    num_steps = int(args.steps) if args.steps else 100
    num_runs = int(args.runs) if args.runs else 3
    
    logger.info(f"Profiling memory usage: {num_runs} runs, {num_steps} steps each")
    
    memory_profiles = []
    
    for run in range(num_runs):
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Memory usage tracking
        initial_memory = brain.getTotalNeuronCount()
        initial_synapses = brain.getTotalSynapseCount()
        
        for step in range(num_steps):
            brain.step(step)
        
        final_memory = brain.getTotalNeuronCount()
        final_synapses = brain.getTotalSynapseCount()
        
        memory_profile = {
            "run": run + 1,
            "initial_neurons": initial_memory,
            "initial_synapses": initial_synapses,
            "final_neurons": final_memory,
            "final_synapses": final_synapses,
            "neuron_growth": final_memory - initial_memory,
            "synapse_growth": final_synapses - initial_synapses
        }
        
        memory_profiles.append(memory_profile)
        logger.debug(f"Memory profile run {run+1} completed")
    
    # Calculate statistics
    total_neuron_growth = sum(p["neuron_growth"] for p in memory_profiles)
    total_synapse_growth = sum(p["synapse_growth"] for p in memory_profiles)
    avg_neuron_growth = total_neuron_growth / len(memory_profiles)
    avg_synapse_growth = total_synapse_growth / len(memory_profiles)
    
    profile_results = {
        "profile_type": "memory_usage_tracking",
        "parameters": {
            "steps": num_steps,
            "runs": num_runs
        },
        "results": {
            "individual_profiles": memory_profiles,
            "statistics": {
                "average_neuron_growth": avg_neuron_growth,
                "average_synapse_growth": avg_synapse_growth,
                "total_neuron_growth": total_neuron_growth,
                "total_synapse_growth": total_synapse_growth,
                "max_neuron_growth": max(p["neuron_growth"] for p in memory_profiles),
                "max_synapse_growth": max(p["synapse_growth"] for p in memory_profiles)
            }
        }
    }
    
    if args.json:
        json.dump(profile_results, sys.stdout, indent=2)
    else:
        print(f"\n=== Memory Usage Profile Results ===")
        print(f"Profile Type: Memory Usage Tracking")
        print(f"Average Neuron Growth: {avg_neuron_growth:.1f}")
        print(f"Average Synapse Growth: {avg_synapse_growth:.1f}")
        print(f"Total Neuron Growth: {total_neuron_growth:.1f}")
        print(f"Total Synapse Growth: {total_synapse_growth:.1f}")
        print(f"Maximum Neuron Growth: {max(p['neuron_growth'] for p in memory_profiles):.1f}")
        print(f"Maximum Synapse Growth: {max(p['synapse_growth'] for p in memory_profiles):.1f}")


def profile_brain_stats(args: argparse.Namespace) -> None:
    """Profile detailed brain statistics."""
    logger = logging.getLogger("performance_profiler")
    
    # Parse profile parameters
    num_steps = int(args.steps) if args.steps else 1000
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    logger.info(f"Profiling brain statistics over {num_steps} steps")
    
    # Collect statistics over time
    time_series_data = []
    spike_accumulator = 0
    firing_rate_accumulator = 0
    
    for step in range(num_steps):
        brain.step(step)
        
        step_data = {
            "step": step,
            "neuron_count": brain.getTotalNeuronCount(),
            "synapse_count": brain.getTotalSynapseCount(),
            "active_neuron_count": brain.getActiveNeuronCount(),
            "firing_neuron_count": brain.getFiringNeuronCount(),
            "average_firing_rate": brain.getAverageFiringRate(),
            "total_spike_count": brain.getTotalSpikeCount(),
            "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
            "developmental_stage": str(brain.getDevelopmentalStage())
        }
        
        time_series_data.append(step_data)
        
        spike_accumulator += step_data["total_spike_count"]
        firing_rate_accumulator += step_data["average_firing_rate"]
        
        if args.verbose and step % 100 == 0:
            logger.info(f"Step {step}: {step_data['active_neuron_count']} active neurons, "
                       f"{step_data['firing_neuron_count']} firing neurons")
    
    # Calculate final statistics
    profile_results = {
        "profile_type": "brain_statistics_over_time",
        "parameters": {
            "steps": num_steps
        },
        "results": {
            "time_series": time_series_data,
            "summary_statistics": {
                "final_neuron_count": brain.getTotalNeuronCount(),
                "final_synapse_count": brain.getTotalSynapseCount(),
                "final_spike_count": brain.getTotalSpikeCount(),
                "final_firing_rate": brain.getAverageFiringRate(),
                "final_ei_ratio": brain.getExcitationInhibitionRatio(),
                "final_development_stage": str(brain.getDevelopmentalStage()),
                "average_firing_rate": firing_rate_accumulator / num_steps,
                "average_neuron_count": sum(d["neuron_count"] for d in time_series_data) / num_steps,
                "max_active_neurons": max(d["active_neuron_count"] for d in time_series_data),
                "max_firing_neurons": max(d["firing_neuron_count"] for d in time_series_data),
                "min_ei_ratio": min(d["excitatory_inhibitory_ratio"] for d in time_series_data),
                "max_ei_ratio": max(d["excitatory_inhibitory_ratio"] for d in time_series_data)
            }
        }
    }
    
    if args.json:
        json.dump(profile_results, sys.stdout, indent=2)
    else:
        print(f"\n=== Brain Statistics Profile Results ===")
        summary = profile_results["results"]["summary_statistics"]
        print(f"Final Neuron Count: {summary['final_neuron_count']}")
        print(f"Final Synapse Count: {summary['final_synapse_count']}")
        print(f"Final Spike Count: {summary['final_spike_count']}")
        print(f"Average Firing Rate: {summary['average_firing_rate']:.4f}")
        print(f"Average Neuron Count: {summary['average_neuron_count']:.1f}")
        print(f"Max Active Neurons: {summary['max_active_neurons']}")
        print(f"Max Firing Neurons: {summary['max_firing_neurons']}")
        print(f"E/I Ratio Range: {summary['min_ei_ratio']:.3f} - {summary['max_ei_ratio']:.3f}")


def main(args: argparse.Namespace) -> int:
    """Main entry point for Performance Profiler CLI."""
    logger = logging.getLogger("performance_profiler")
    
    if args.verbose:
        setup_logging(logging.DEBUG)
    else:
        setup_logging(logging.INFO)
    
    # Execute command based on subcommand
    if args.command == 'benchmark':
        benchmark_creation(args)
    elif args.command == 'memory':
        profile_memory_usage(args)
    elif args.command == 'stats':
        profile_brain_stats(args)
    else:
        logger.error(f"Unknown command: {args.command}")
        return 1
    
    return 0


def setup_logging(level: int = logging.INFO) -> None:
    """Setup logging configuration."""
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )


def create_parser() -> argparse.ArgumentParser:
    """Create argument parser for Performance Profiler CLI."""
    parser = argparse.ArgumentParser(
        prog='nlm performance',
        description='NLM Performance Profiler - Benchmarking and profiling brain performance',
        epilog='Examples:\n'
              '  nlm performance benchmark --num-brains 5 --benchmark-steps 100\n'
              '  nlm performance memory --steps 1000 --runs 3\n'
              '  nlm performance stats --steps 1000'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Enable verbose logging'
    )
    
    parser.add_argument(
        '--json',
        action='store_true',
        help='Output results as JSON'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Benchmark brain creation and performance')
    benchmark_parser.add_argument('--num-brains', type=int, default=5, help='Number of brains to benchmark')
    benchmark_parser.add_argument('--warmup-steps', type=int, default=10, help='Warm-up steps for each brain')
    benchmark_parser.add_argument('--benchmark-steps', type=int, default=100, help='Number of benchmark steps')
    benchmark_parser.set_defaults(func=benchmark_creation)
    
    # Memory profile command
    memory_parser = subparsers.add_parser('memory', help='Profile memory usage')
    memory_parser.add_argument('--steps', type=int, default=100, help='Number of steps to profile')
    memory_parser.add_argument('--runs', type=int, default=3, help='Number of profiling runs')
    memory_parser.set_defaults(func=profile_memory_usage)
    
    # Stats profile command
    stats_parser = subparsers.add_parser('stats', help='Profile brain statistics')
    stats_parser.add_argument('--steps', type=int, default=1000, help='Number of steps to profile')
    stats_parser.set_defaults(func=profile_brain_stats)
    
    return parser


if __name__ == '__main__':
    parser = create_parser()
    args = parser.parse_args()
    
    # Setup logging
    setup_logging(logging.DEBUG if args.verbose else logging.INFO)
    
    # Execute command
    try:
        if hasattr(args, 'func'):
            sys.exit(args.func(args))
        else:
            parser.print_help()
            sys.exit(0)
    except Exception as e:
        logging.error(f"Command failed: {e}")
        if args.json:
            json.dump({"success": False, "error": str(e)}, sys.stdout, indent=2)
        else:
            print(f"ERROR: {e}")
        sys.exit(1)