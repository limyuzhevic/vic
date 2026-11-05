#!/usr/bin/env python3
"""
NLM Experiment Runner CLI - Run different experiment types with various parameters.

This tool provides command-line access to running various NLM experiments
including simulation runs, parameter sweeps, and different experimental protocols.
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


def run_experiment(args: argparse.Namespace) -> None:
    """Run a simple NLM experiment."""
    logger = logging.getLogger("experiment_runner")
    
    # Create brain and world
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create world
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    world.setAgentStart(10.0, 10.0)
    
    # Create agent brain interface
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable comprehensive learning capabilities
    agent.enableRewardModulation(True)
    agent.enableStructuralPlasticity(True)
    agent.enableDevelopment(True)
    agent.enableCuriosity(True)
    
    # Parse experiment parameters
    max_steps = int(args.max_steps) if args.max_steps else 1000
    start_step = int(args.start_step) if args.start_step else 0
    
    logger.info(f"Running experiment with max_steps={max_steps}, start_step={start_step}")
    
    # Run simulation
    episode_reward = 0.0
    start_time = time.time()
    
    for step in range(start_step, start_step + max_steps):
        # Update world with time passing
        world.update(0.1)
        
        # Get what the agent sees
        percept = world.getSensoryPercept()
        
        # Process sensory input in brain
        agent.processSensoryInput(percept)
        
        # Brain thinks and learns
        brain.step(step)
        
        # Get action from brain's motor neurons
        action = agent.decodeMotorCommand()
        
        # Do action in world
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Get reward for agent's actions
        reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
        agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        agent.updateDevelopment(0.1)
        
        episode_reward += reward
        
        # Log progress
        if args.verbose and step % 100 == 0:
            logger.info(f"Step {step}: Reward={reward:.2f}, "
                       f"Curiosity={agent.getCuriosityLevel():.3f}, "
                       f"Novelty={agent.getNoveltyLevel():.3f}")
    
    end_time = time.time()
    elapsed_time = end_time - start_time
    
    # Collect experiment results
    experiment_results = {
        "experiment_type": "simple_agent",
        "parameters": {
            "max_steps": max_steps,
            "start_step": start_step,
            "world_dimensions": "20x20",
            "vision_dimensions": "8x8"
        },
        "results": {
            "total_reward": episode_reward,
            "total_spikes": brain.getTotalSpikeCount(),
            "final_curiosity": agent.getCuriosityLevel(),
            "final_novelty": agent.getNoveltyLevel(),
            "final_prediction_error": agent.getPredictionError(),
            "final_development_stage": str(brain.getDevelopmentalStage()),
            "memory_capacity_used": brain.getFiringNeuronCount(),
            "total_neurons": brain.getTotalNeuronCount(),
            "total_synapses": brain.getTotalSynapseCount(),
            "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio()
        },
        "performance": {
            "elapsed_time_seconds": elapsed_time,
            "steps_per_second": max_steps / elapsed_time if elapsed_time > 0 else 0,
            "spikes_per_second": brain.getTotalSpikeCount() / elapsed_time if elapsed_time > 0 else 0
        }
    }
    
    # Output results
    if args.json:
        json.dump(experiment_results, sys.stdout, indent=2)
        sys.stdout.write('\n')
    else:
        print(f"\n=== Experiment Results ===")
        print(f"Experiment Type: {experiment_results['experiment_type']}")
        print(f"Total Reward: {experiment_results['results']['total_reward']:.2f}")
        print(f"Total Spikes: {experiment_results['results']['total_spikes']}")
        print(f"Final Curiosity: {experiment_results['results']['final_curiosity']:.3f}")
        print(f"Final Novelty: {experiment_results['results']['final_novelty']:.3f}")
        print(f"Memory Capacity Used: {experiment_results['results']['memory_capacity_used']}/{experiment_results['results']['total_neurons']}")
        print(f"E/I Balance: {experiment_results['results']['excitatory_inhibitory_ratio']:.3f}")
        print(f"Elapsed Time: {experiment_results['performance']['elapsed_time_seconds']:.2f}s")
        print(f"Performance: {experiment_results['performance']['steps_per_second']:.1f} steps/sec")


def run_parameter_sweep(args: argparse.Namespace) -> None:
    """Run parameter sweep experiment."""
    logger = logging.getLogger("experiment_runner")
    
    # Parse parameter sweep configuration
    param_configs = []
    if args.param_values:
        try:
            # Parse comma-separated key=value pairs
            for param in args.param_values:
                key, value = param.split('=')
                param_configs.append({
                    "key": key.strip(),
                    "value": float(value) if '.' in value else int(value)
                })
        except ValueError:
            logger.error(f"Invalid parameter format: {param}. Use key=value format")
            sys.exit(1)
    
    if args.json:
        results = {
            "sweep_type": "parameter_sweep",
            "parameters": param_configs,
            "message": "Parameter sweep execution completed (simulated)",
            "note": "Full parameter sweep implementation would iterate through all configurations"
        }
        json.dump(results, sys.stdout, indent=2)
    else:
        print(f"Parameter sweep configuration: {param_configs}")
        print("Parameter sweep completed (simulated)")


def run_benchmark(args: argparse.Namespace) -> None:
    """Run performance benchmark."""
    logger = logging.getLogger("experiment_runner")
    
    # Parse benchmark parameters
    num_steps = int(args.steps) if args.steps else 100
    warmup_steps = int(args.warmup) if args.warmup else 10
    
    # Create brain
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Warm-up phase
    for i in range(warmup_steps):
        brain.step(i)
    
    # Benchmark phase
    start_time = time.time()
    for i in range(num_steps):
        brain.step(i + warmup_steps)
    end_time = time.time()
    
    elapsed_time = end_time - start_time
    
    benchmark_results = {
        "benchmark_type": "brain_step_performance",
        "parameters": {
            "warmup_steps": warmup_steps,
            "benchmark_steps": num_steps
        },
        "results": {
            "total_time_seconds": elapsed_time,
            "steps_per_second": num_steps / elapsed_time if elapsed_time > 0 else 0,
            "final_neuron_count": brain.getTotalNeuronCount(),
            "final_spike_count": brain.getTotalSpikeCount(),
            "final_firing_rate": brain.getAverageFiringRate()
        }
    }
    
    if args.json:
        json.dump(benchmark_results, sys.stdout, indent=2)
    else:
        print(f"\n=== Benchmark Results ===")
        print(f"Benchmark Type: Brain Step Performance")
        print(f"Total Time: {benchmark_results['results']['total_time_seconds']:.2f}s")
        print(f"Steps/sec: {benchmark_results['results']['steps_per_second']:.1f}")
        print(f"Spikes/sec: {benchmark_results['results']['final_spike_count'] / elapsed_time:.1f}")


def main(args: argparse.Namespace) -> int:
    """Main entry point for Experiment Runner CLI."""
    logger = logging.getLogger("experiment_runner")
    
    if args.verbose:
        setup_logging(logging.DEBUG)
    else:
        setup_logging(logging.INFO)
    
    # Execute command based on subcommand
    if args.command == 'run':
        run_experiment(args)
    elif args.command == 'sweep':
        run_parameter_sweep(args)
    elif args.command == 'benchmark':
        run_benchmark(args)
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
    """Create argument parser for Experiment Runner CLI."""
    parser = argparse.ArgumentParser(
        prog='nlm experiment',
        description='NLM Experiment Runner - Run different experiment types with various parameters',
        epilog='Examples:\n'
              '  nlm experiment run --max-steps 1000 --start-step 0\n'
              '  nlm experiment sweep --param-values "neuron_count=500" "region_count=3"\n'
              '  nlm experiment benchmark --steps 100 --warmup 10'
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
    
    # Run experiment command
    run_parser = subparsers.add_parser('run', help='Run an NLM experiment')
    run_parser.add_argument('--max-steps', type=int, default=1000, help='Maximum simulation steps')
    run_parser.add_argument('--start-step', type=int, default=0, help='Starting step')
    run_parser.set_defaults(func=run_experiment)
    
    # Parameter sweep command
    sweep_parser = subparsers.add_parser('sweep', help='Run parameter sweep experiment')
    sweep_parser.add_argument('--param-values', nargs='+', help='Parameter values in key=value format')
    sweep_parser.set_defaults(func=run_parameter_sweep)
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmark')
    benchmark_parser.add_argument('--steps', type=int, default=100, help='Number of benchmark steps')
    benchmark_parser.add_argument('--warmup', type=int, default=10, help='Number of warm-up steps')
    benchmark_parser.set_defaults(func=run_benchmark)
    
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