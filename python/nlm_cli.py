#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI) Tool

A command-line interface for the NLM neural simulation framework.
Provides easy access to brain simulation, experiments, and analysis.
"""

import sys
import os
import json
import argparse
import logging
from pathlib import Path
from typing import Optional, List
from datetime import datetime

# Add the python directory to the path so we can import our modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    from pynlm_enhanced import (
        BrainSimulator, NLMEnvironment, NLMConfigurator, 
        SimulationMetrics, NLMError, InitializationError,
        ConfigurationError, SimulationError
    )
except ImportError as e:
    print(f"Error: Failed to import NLM modules: {e}")
    print("Please ensure pynlm_enhanced.py is available in the python directory.")
    sys.exit(1)

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s',
    datefmt='%Y-%m-%d %H:%M:%S'
)
logger = logging.getLogger(__name__)


def create_parser():
    """Create the main argument parser."""
    parser = argparse.ArgumentParser(
        prog='nlm',
        description='NLM Neural Simulation Framework CLI',
        epilog='For more information, visit https://github.com/nlm-project/nlm'
    )
    
    parser.add_argument(
        '--version', 
        action='version', 
        version='NLM CLI v2.0.0',
        help='Show version information and exit'
    )
    
    # Global logging options
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose logging'
    )
    
    parser.add_argument(
        '--quiet', '-q',
        action='store_true',
        help='Suppress non-essential output'
    )
    
    # Main subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create brain command
    create_brain_parser(subparsers)
    
    # Run simulation command
    run_sim_parser(subparsers)
    
    # Environment command
    env_parser(subparsers)
    
    # Configuration command
    config_parser(subparsers)
    
    # Experiment command
    exp_parser(subparsers)
    
    # Export command
    export_parser(subparsers)
    
    # Benchmark command
    bench_parser(subparsers)
    
    return parser


def create_brain_parser(subparsers):
    """Create the brain creation subparser."""
    parser = subparsers.add_parser(
        'create-brain',
        aliases=['brain'],
        help='Create and initialize a brain'
    )
    
    parser.add_argument(
        '--config', '-c',
        help='Path to configuration file'
    )
    
    parser.add_argument(
        '--neurons', '-n',
        type=int,
        default=1000,
        help='Number of neurons (default: 1000)'
    )
    
    parser.add_argument(
        '--regions', '-r',
        type=int,
        default=1,
        help='Number of brain regions (default: 1)'
    )
    
    parser.add_argument(
        '--steps', '-s',
        type=int,
        default=100,
        help='Number of simulation steps (default: 100)'
    )
    
    parser.add_argument(
        '--output', '-o',
        help='Output file for brain state'
    )


def run_sim_parser(subparsers):
    """Create the run simulation subparser."""
    parser = subparsers.add_parser(
        'run-sim',
        aliases=['sim', 'run'],
        help='Run a brain simulation'
    )
    
    parser.add_argument(
        '--config', '-c',
        help='Path to configuration file'
    )
    
    parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Number of simulation steps (default: 1000)'
    )
    
    parser.add_argument(
        '--timestep', '-t',
        type=float,
        default=0.001,
        help='Simulation timestep (default: 0.001)'
    )
    
    parser.add_argument(
        '--output', '-o',
        help='Output file for results'
    )
    
    parser.add_argument(
        '--export-metrics',
        action='store_true',
        help='Export simulation metrics'
    )
    
    parser.add_argument(
        '--format',
        choices=['json', 'csv'],
        default='json',
        help='Export format (default: json)'
    )


def env_parser(subparsers):
    """Create the environment subparser."""
    parser = subparsers.add_parser(
        'env',
        help='Manage world/environment simulation'
    )
    
    subparsers_env = parser.add_subparsers(dest='env_command', help='Environment commands')
    
    # Configure environment
    config_env_parser = subparsers_env.add_parser(
        'configure',
        help='Configure environment'
    )
    
    config_env_parser.add_argument('--width', type=int, default=20, help='World width')
    config_env_parser.add_argument('--height', type=int, default=20, help='World height')
    config_env_parser.add_argument('--vision-width', type=int, default=8, help='Vision sensor width')
    config_env_parser.add_argument('--vision-height', type=int, default=8, help='Vision sensor height')
    
    # Reset environment
    subparsers_env.add_parser('reset', help='Reset environment')
    
    # Update environment
    update_parser = subparsers_env.add_parser('update', help='Update environment')
    update_parser.add_argument('--timestep', type=float, default=0.1, help='Timestep')
    
    # Set agent start
    start_parser = subparsers_env.add_parser('start', help='Set agent start position')
    start_parser.add_argument('--x', type=float, required=True, help='X coordinate')
    start_parser.add_argument('--y', type=float, required=True, help='Y coordinate')


def config_parser(subparsers):
    """Create the configuration subparser."""
    parser = subparsers.add_parser(
        'config',
        help='Manage configuration'
    )
    
    subparsers_config = parser.add_subparsers(dest='config_command', help='Configuration commands')
    
    # Load configuration
    load_parser = subparsers_config.add_parser('load', help='Load configuration from file')
    load_parser.add_argument('filepath', help='Path to configuration file')
    
    # Save configuration
    save_parser = subparsers_config.add_parser('save', help='Save configuration to file')
    save_parser.add_argument('filepath', help='Path to save configuration file')
    
    # Get configuration value
    get_parser = subparsers_config.add_parser('get', help='Get configuration value')
    get_parser.add_argument('key', help='Configuration key')
    get_parser.add_argument('--default', help='Default value if key not found')
    
    # Set configuration value
    set_parser = subparsers_config.add_parser('set', help='Set configuration value')
    set_parser.add_argument('key', help='Configuration key')
    set_parser.add_argument('value', help='Configuration value')


def exp_parser(subparsers):
    """Create the experiment subparser."""
    parser = subparsers.add_parser(
        'experiment',
        aliases=['exp'],
        help='Run a complete experiment'
    )
    
    parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Number of simulation steps (default: 1000)'
    )
    
    parser.add_argument(
        '--output', '-o',
        help='Output directory for experiment results'
    )
    
    parser.add_argument(
        '--name',
        help='Experiment name'
    )
    
    parser.add_argument(
        '--description',
        help='Experiment description'
    )


def export_parser(subparsers):
    """Create the export subparser."""
    parser = subparsers.add_parser(
        'export',
        help='Export simulation data'
    )
    
    parser.add_argument(
        'source',
        choices=['brain', 'metrics', 'config'],
        help='Source to export'
    )
    
    parser.add_argument(
        '--output', '-o',
        required=True,
        help='Output file path'
    )
    
    parser.add_argument(
        '--format',
        choices=['json', 'csv', 'pickle'],
        default='json',
        help='Export format (default: json)'
    )


def bench_parser(subparsers):
    """Create the benchmark subparser."""
    parser = subparsers.add_parser(
        'benchmark',
        help='Run performance benchmarks'
    )
    
    parser.add_argument(
        '--steps', '-s',
        type=int,
        default=100,
        help='Number of steps for benchmark (default: 100)'
    )
    
    parser.add_argument(
        '--repeats',
        type=int,
        default=5,
        help='Number of benchmark repetitions (default: 5)'
    )
    
    parser.add_argument(
        '--output', '-o',
        help='Output file for benchmark results'
    )


def main():
    """Main CLI entry point."""
    parser = create_parser()
    args = parser.parse_args()
    
    # Configure logging based on verbosity
    if args.verbose:
        logger.setLevel(logging.DEBUG)
    elif args.quiet:
        logger.setLevel(logging.WARNING)
    
    # Handle subcommands
    if args.command is None:
        parser.print_help()
        return 0
    
    try:
        if args.command in ['create-brain', 'brain']:
            return handle_create_brain(args)
        elif args.command in ['run-sim', 'sim', 'run']:
            return handle_run_sim(args)
        elif args.command == 'env':
            return handle_env(args)
        elif args.command == 'config':
            return handle_config(args)
        elif args.command in ['experiment', 'exp']:
            return handle_experiment(args)
        elif args.command == 'export':
            return handle_export(args)
        elif args.command == 'benchmark':
            return handle_benchmark(args)
        else:
            logger.error(f"Unknown command: {args.command}")
            return 1
            
    except KeyboardInterrupt:
        logger.info("Operation cancelled by user")
        return 130
    except Exception as e:
        logger.error(f"Command failed: {e}")
        if args.verbose:
            import traceback
            traceback.print_exc()
        return 1


def handle_create_brain(args):
    """Handle create-brain command."""
    logger.info("Creating brain...")
    
    # Create configuration
    config = None
    if args.config:
        from pynlm_enhanced import NLMConfigurator
        configurator = NLMConfigurator()
        if not configurator.load(args.config):
            raise ConfigurationError(f"Failed to load configuration from {args.config}")
        config = configurator.config
    
    # Create brain simulator
    from pynlm_enhanced import BrainSimulator
    brain_sim = BrainSimulator(config)
    
    # Initialize with custom parameters
    init_kwargs = {}
    if args.neurons:
        init_kwargs['brain'] = {'neuron_count': args.neurons}
    if args.regions:
        init_kwargs['brain'] = init_kwargs.get('brain', {})
        init_kwargs['brain']['region_count'] = args.regions
    
    brain_sim.initialize(**init_kwargs)
    
    # Run simulation
    logger.info(f"Running simulation: {args.steps} steps")
    brain_sim.run(args.steps)
    
    # Save if requested
    if args.output:
        brain_sim.save(args.output)
        logger.info(f"Brain state saved to {args.output}")
    
    # Print summary
    metrics = brain_sim.get_metrics_summary()
    logger.info(f"Simulation complete: {metrics.get('total_steps', 0)} steps")
    
    return 0


def handle_run_sim(args):
    """Handle run-sim command."""
    logger.info("Starting simulation...")
    
    # Create brain simulator
    from pynlm_enhanced import BrainSimulator
    brain_sim = BrainSimulator()
    
    # Initialize brain
    brain_sim.initialize()
    
    # Run simulation with custom timestep
    logger.info(f"Running {args.steps} steps with timestep {args.timestep}")
    
    def progress_callback(metrics):
        if metrics.step % 100 == 0:
            logger.info(f"Step {metrics.step}: {metrics.active_neurons} firing neurons, "
                       f"{metrics.firing_rate:.2f} Hz average")
    
    # Run simulation
    metrics = brain_sim.run(args.steps, 
                           timestep=args.timestep,
                           callback=progress_callback)
    
    # Save brain state
    if args.output:
        brain_sim.save(args.output)
        logger.info(f"Brain state saved to {args.output}")
    
    # Export metrics if requested
    if args.export_metrics:
        metrics_file = f"{args.output}_metrics.{args.format}" if args.output else f"simulation_metrics.{args.format}"
        brain_sim.export_metrics(metrics_file, args.format)
        logger.info(f"Metrics exported to {metrics_file}")
    
    # Print final summary
    summary = brain_sim.get_metrics_summary()
    logger.info("Simulation complete!")
    logger.info(f"Total steps: {summary.get('total_steps', 0)}")
    logger.info(f"Max firing neurons: {summary.get('max_firing_neurons', 0)}")
    logger.info(f"Average firing rate: {summary.get('avg_firing_rate', 0):.2f} Hz")
    
    return 0


def handle_env(args):
    """Handle environment command."""
    if args.env_command is None:
        logger.error("Environment command required. Use --help for available commands.")
        return 1
    
    env = NLMEnvironment()
    
    if args.env_command == 'configure':
        logger.info("Configuring environment...")
        env.configure(
            width=args.width,
            height=args.height,
            vision_width=args.vision_width,
            vision_height=args.vision_height
        )
    
    elif args.env_command == 'reset':
        logger.info("Resetting environment...")
        env.reset()
    
    elif args.env_command == 'update':
        logger.info(f"Updating environment with timestep {args.timestep}...")
        result = env.update(args.timestep)
        logger.info(f"Environment updated: {result['timestamp']}")
    
    elif args.env_command == 'start':
        logger.info(f"Setting agent start position to ({args.x}, {args.y})")
        env.set_agent_start(args.x, args.y)
    
    else:
        logger.error(f"Unknown environment command: {args.env_command}")
        return 1
    
    return 0


def handle_config(args):
    """Handle configuration command."""
    from pynlm_enhanced import NLMConfigurator
    
    if args.config_command is None:
        logger.error("Configuration command required. Use --help for available commands.")
        return 1
    
    configurator = NLMConfigurator()
    
    if args.config_command == 'load':
        logger.info(f"Loading configuration from {args.filepath}...")
        if configurator.load(args.filepath):
            logger.info("Configuration loaded successfully")
        else:
            raise ConfigurationError(f"Failed to load configuration from {args.filepath}")
    
    elif args.config_command == 'save':
        logger.info(f"Saving configuration to {args.filepath}...")
        configurator.save(args.filepath)
    
    elif args.config_command == 'get':
        logger.info(f"Getting configuration value for key '{args.key}'...")
        value = configurator.get(args.key, args.default)
        if value is not None:
            print(f"{args.key} = {value}")
        else:
            print(f"{args.key} = (not set)")
    
    elif args.config_command == 'set':
        logger.info(f"Setting configuration '{args.key}' = {args.value}...")
        configurator.set(args.key, args.value)
    
    else:
        logger.error(f"Unknown configuration command: {args.config_command}")
        return 1
    
    return 0


def handle_experiment(args):
    """Handle experiment command."""
    logger.info("Starting experiment...")
    
    # Create experiment with custom name/description
    from pynlm_enhanced import run_experiment
    
    try:
        results = run_experiment(
            brain_config=None,
            world_config={
                'width': 20,
                'height': 20,
                'vision_width': 8,
                'vision_height': 8
            },
            num_steps=args.steps
        )
        
        logger.info("Experiment completed successfully!")
        
        # Save results if output directory specified
        if args.output:
            os.makedirs(args.output, exist_ok=True)
            
            # Save brain state
            brain_sim = results['brain_simulator']
            brain_path = os.path.join(args.output, 'brain.bin')
            brain_sim.save(brain_path)
            
            # Save metrics
            metrics_path = os.path.join(args.output, 'metrics.json')
            brain_sim.export_metrics(metrics_path, 'json')
            
            # Save world state
            env = results['environment']
            # Note: NLM doesn't have a direct world save method, so we just save metadata
            with open(os.path.join(args.output, 'experiment_info.json'), 'w') as f:
                json.dump({
                    'name': args.name or 'experiment',
                    'description': args.description or '',
                    'timestamp': datetime.now().isoformat(),
                    'steps': args.steps
                }, f, indent=2)
            
            logger.info(f"Experiment results saved to {args.output}")
        
        return 0
        
    except Exception as e:
        logger.error(f"Experiment failed: {e}")
        return 1


def handle_export(args):
    """Handle export command."""
    logger.info(f"Exporting {args.source} data...")
    
    # Note: This is a simplified example. In a real implementation,
    # we would handle different export sources
    
    if args.source == 'config':
        from pynlm_enhanced import NLMConfigurator
        configurator = NLMConfigurator()
        
        if args.format == 'json':
            config_data = {}
            for key in configurator.config.getKeys():
                config_data[key] = configurator.config.get("string")(key)
            
            with open(args.output, 'w') as f:
                json.dump(config_data, f, indent=2)
        
        logger.info(f"Configuration exported to {args.output}")
    
    elif args.source == 'metrics':
        logger.info("Metrics export requires a brain simulator instance.")
        logger.info("Use 'run-sim' command first to generate metrics.")
        return 1
    
    elif args.source == 'brain':
        logger.info("Brain state export requires a brain simulator instance.")
        logger.info("Use 'create-brain' or 'run-sim' command first.")
        return 1
    
    return 0


def handle_benchmark(args):
    """Handle benchmark command."""
    logger.info(f"Running benchmark: {args.repeats} repeats of {args.steps} steps each")
    
    from pynlm_enhanced import BrainSimulator
    import time
    
    times = []
    
    for repeat in range(args.repeats):
        logger.info(f"Benchmark run {repeat + 1}/{args.repeats}...")
        
        brain_sim = BrainSimulator()
        brain_sim.initialize()
        
        start_time = time.time()
        brain_sim.run(args.steps)
        end_time = time.time()
        
        duration = end_time - start_time
        times.append(duration)
        
        logger.info(f"  Completed in {duration:.2f} seconds")
    
    # Calculate statistics
    avg_time = sum(times) / len(times)
    min_time = min(times)
    max_time = max(times)
    
    results = {
        'repeats': args.repeats,
        'steps_per_repeat': args.steps,
        'total_steps': args.repeats * args.steps,
        'times': times,
        'avg_time_per_repeat': avg_time,
        'min_time_per_repeat': min_time,
        'max_time_per_repeat': max_time,
        'steps_per_second': (args.repeats * args.steps) / avg_time if avg_time > 0 else 0
    }
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2)
        logger.info(f"Benchmark results saved to {args.output}")
    
    # Print summary
    logger.info("\nBenchmark Results:")
    logger.info(f"  Total steps: {results['total_steps']}")
    logger.info(f"  Average time per repeat: {results['avg_time_per_repeat']:.2f} seconds")
    logger.info(f"  Min time per repeat: {results['min_time_per_repeat']:.2f} seconds")
    logger.info(f"  Max time per repeat: {results['max_time_per_repeat']:.2f} seconds")
    logger.info(f"  Steps per second: {results['steps_per_second']:.0f}")
    
    return 0


if __name__ == '__main__':
    sys.exit(main())