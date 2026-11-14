#!/usr/bin/env python3
"""
NLM Performance Benchmarking Tool
=================================

Performance benchmarking utilities for the NLM neural simulation framework.
Provides comprehensive benchmarking for different aspects of the brain simulation.

Usage:
    python3 nlm_benchmark.py [options]
"""

import nlm
import time
import json
import logging
import argparse
from typing import Dict, Any
from nlm_test import BenchmarkSuite

logger = logging.getLogger(__name__)

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(name)s - %(levelname)s - %(message)s')

parser = argparse.ArgumentParser(description='NLM Performance Benchmarking Tool')

parser.add_argument('--brain-config', default='configs/default.cfg', help='Path to brain configuration file')
parser.add_argument('--world-width', type=int, default=20, help='World width')
parser.add_argument('--world-height', type=int, default=20, help='World height')
parser.add_argument('--vision-width', type=int, default=8, help='World vision width')
parser.add_argument('--vision-height', type=int, default=8, help='World vision height')
parser.add_argument('--steps', type=int, default=100, help='Simulation steps per benchmark')
parser.add_argument('--iterations', type=int, default=3, help='Number of benchmark iterations')
parser.add_argument('--output', help='Output JSON file for benchmark results')

args = parser.parse_args()

# Create benchmark suite
benchmark = BenchmarkSuite()

# Load configuration
config = nlm.createDefaultConfig()
if config.loadFromFile(args.brain_config):
    logger.info(f"Loaded configuration from {args.brain_config}")
else:
    logger.warning(f"Failed to load configuration from {args.brain_config}, using defaults")

# Configure world size
world = nlm.createSimpleWorld()
world.configure(args.world_width, args.world_height, args.vision_width, args.vision_height)

# Run benchmarks
logger.info("Running NLM performance benchmarks...")

# Benchmark brain initialization
logger.info("Benchmarking brain initialization...")
init_results = benchmark.benchmark_brain_initialization(config, args.iterations)
logger.info(f"Brain initialization: {init_results['mean_time']:.3f}s mean")

# Create brain and world for integration tests
brain = nlm.createBrain(config)
brain.initialize()

agent = nlm.createAgentBrain(brain)
agent.initialize(world)

# Benchmark simulation
logger.info("Benchmarking simulation performance...")
sim_results = benchmark.benchmark_simulation(brain, world, agent, args.steps, args.iterations)
logger.info(f"Simulation: {sim_results['mean_time_per_step']:.6f}s per step")

# Benchmark memory usage
logger.info("Benchmarking memory usage...")
mem_results = benchmark.benchmark_memory_usage(brain, world, agent, min(args.steps, 50))
logger.info(f"Memory: {mem_results['peak_memory_mb']:.2f}MB peak")

# Combine results
results = {
    'benchmark_info': {
        'brain_config': args.brain_config,
        'world_config': {
            'width': args.world_width,
            'height': args.world_height,
            'vision_width': args.vision_width,
            'vision_height': args.vision_height
        },
        'steps': args.steps,
        'iterations': args.iterations,
        'timestamp': time.time()
    },
    'brain_initialization': init_results,
    'simulation': sim_results,
    'memory': mem_results
}

# Calculate summary statistics
performance_score = {
    'init_speed': 1.0 / init_results['mean_time'] if init_results['mean_time'] > 0 else 0,
    'sim_speed': sim_results['steps_per_second'] if sim_results['steps_per_second'] > 0 else 0,
    'memory_efficiency': mem_results['steps_per_mb'] if mem_results['steps_per_mb'] > 0 else 0
}

results['performance_score'] = performance_score

# Log results
logger.info("\nBenchmark Results Summary:")
logger.info(f"  Brain initialization: {init_results['mean_time']:.3f}s mean")
logger.info(f"  Simulation speed: {sim_results['steps_per_second']:.0f} steps/second")
logger.info(f"  Memory usage: {mem_results['peak_memory_mb']:.2f}MB peak")
logger.info(f"  Performance score: {performance_score}")

# Export results
if args.output:
    with open(args.output, 'w') as f:
        json.dump(results, f, indent=2, default=str)
    logger.info(f"Benchmark results exported to {args.output}")
else:
    # Print JSON output
    print(json.dumps(results, indent=2, default=str))

# Generate recommendation based on results
logger.info("\nPerformance Recommendations:")
if performance_score['sim_speed'] < 10:
    logger.warning("Simulation speed is low (< 10 steps/second)")
    logger.info("Consider increasing neuron_count or reducing world dimensions")
elif performance_score['sim_speed'] > 100:
    logger.info("Excellent simulation speed (> 100 steps/second)")
else:
    logger.info("Good simulation speed (10-100 steps/second)")

if performance_score['memory_efficiency'] < 1000:
    logger.warning("Memory efficiency is low (< 1000 steps/MB)")
    logger.info("Consider optimizing memory usage or reducing simulation complexity")
else:
    logger.info(f"Good memory efficiency ({performance_score['memory_efficiency']:.0f} steps/MB)")

if performance_score['init_speed'] < 0.5:
    logger.warning("Brain initialization is slow (< 0.5s)")
    logger.info("Consider using a simpler configuration or larger neuron_count")
else:
    logger.info(f"Good initialization speed ({performance_score['init_speed']:.1f} init/second)")

logger.info("\nBenchmark completed successfully!")
