#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)
Enhanced command-line interface for NLM users
"""

import sys
import json
import time
import argparse
from pathlib import Path

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install the NLM Python bindings.")
    sys.exit(1)

def create_brain_from_config(config_file=None, neuron_count=1000, region_count=1):
    """Create a brain with specified configuration"""
    brain_config = pynlm.createDefaultConfig()
    
    if config_file:
        try:
            brain_config.loadFromFile(config_file)
            print(f"Loaded configuration from {config_file}")
        except Exception as e:
            print(f"Warning: Could not load config file {config_file}: {e}")
    
    # Override with command line parameters
    brain_config.setInt("neuron_count", neuron_count)
    brain_config.setInt("region_count", region_count)
    
    # Create brain
    brain = pynlm.createBrain(brain_config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons in {brain.getRegionCount()} regions")
    return brain

def create_complete_system(config_file=None):
    """Create a complete NLM system (brain, world, agent)"""
    try:
        brain, world, agent = pynlm.createCompleteSystem(config_file)
        print("Created complete NLM system")
        return brain, world, agent
    except Exception as e:
        print(f"Error creating complete system: {e}")
        return None, None, None

def run_simulation(brain, world, agent, steps, verbose=False):
    """Run a simulation"""
    print(f"Running simulation for {steps} steps...")
    
    start_time = time.time()
    
    for step in range(steps):
        world.update(0.1)  # Standard timestep
        
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        brain.step(step)
        
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if verbose and step % 100 == 0:
            firing_rate = brain.getAverageFiringRate()
            firing_count = brain.getFiringNeuronCount()
            print(f"Step {step}: Firing rate={firing_rate:.3f}, Firing neurons={firing_count}")
    
    end_time = time.time()
    elapsed = end_time - start_time
    
    print(f"Simulation completed in {elapsed:.2f} seconds")
    
    # Get final statistics
    stats = brain.getBrainStats()
    print(f"Final statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    return brain, world, agent

def analyze_brain(brain):
    """Analyze brain network"""
    print("Analyzing brain network...")
    
    # Get basic statistics
    stats = brain.getBrainStats()
    print(f"Basic statistics:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    # Get connectivity statistics
    connectivity = brain.getConnectivityStats()
    print(f"Connectivity statistics:")
    for key, value in connectivity.items():
        print(f"  {key}: {value}")
    
    # Get neuron distribution
    distribution = brain.getNeuronDistribution()
    print(f"Neuron distribution:")
    for key, value in distribution.items():
        print(f"  {key}: {value}")
    
    # Get region statistics
    regions = brain.getRegionStats()
    print(f"Region statistics:")
    for i, region_stats in enumerate(regions):
        print(f"  Region {i}:")
        for key, value in region_stats.items():
            print(f"    {key}: {value}")

def benchmark_brain(neuron_counts, steps_per_test=1000):
    """Benchmark brain performance"""
    print("Benchmarking brain performance...")
    
    results = []
    
    for neuron_count in neuron_counts:
        print(f"Testing with {neuron_count} neurons...")
        
        brain_config = pynlm.createDefaultConfig()
        brain_config.setInt("neuron_count", neuron_count)
        brain_config.setInt("region_count", 1)
        brain_config.setFloat("connection_probability", 0.1f)
        
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        start_time = time.time()
        for step in range(steps_per_test):
            brain.step(step)
        end_time = time.time()
        
        elapsed = end_time - start_time
        steps_per_second = steps_per_test / elapsed
        spikes_per_step = brain.getTotalSpikeCount() / steps_per_test
        
        result = {
            "neurons": neuron_count,
            "steps": steps_per_test,
            "time_ms": elapsed * 1000,
            "steps_per_second": steps_per_second,
            "spikes_per_step": spikes_per_step
        }
        
        results.append(result)
        
        print(f"  Time: {elapsed:.2f}s ({steps_per_second:.0f} steps/s)")
        print(f"  Spikes/step: {spikes_per_step:.2f}")
    
    return results

def save_results(results, filename):
    """Save results to file"""
    with open(filename, 'w') as f:
        json.dump(results, f, indent=2)
    print(f"Results saved to {filename}")

def load_results(filename):
    """Load results from file"""
    try:
        with open(filename, 'r') as f:
            results = json.load(f)
        print(f"Results loaded from {filename}")
        return results
    except Exception as e:
        print(f"Error loading results from {filename}: {e}")
        return None

def create_test_environment(width=20, height=20, resources=5, hazards=3):
    """Create test environment with objects"""
    print(f"Creating test environment ({width}x{height})...")
    
    world = pynlm.createSimpleWorld()
    world.configure(width, height, 8, 8)
    world.reset()
    world.setRandomSeed(int(time.time()))
    
    # Add resources
    for i in range(resources):
        x = float(rand() % width)
        y = float(rand() % height)
        pynlm.WorldObject obj(x, y, pynlm.WorldObjectType.Resource)
        world.addObject(obj)
    
    # Add hazards
    for i in range(hazards):
        x = float(rand() % width)
        y = float(rand() % height)
        pynlm.WorldObject obj(x, y, pynlm.WorldObjectType.Hazard, 0.5f)
        world.addObject(obj)
    
    print(f"Created test environment with {resources} resources and {hazards} hazards")
    return world

def validate_environment(world):
    """Validate environment"""
    print("Validating environment...")
    
    validation = world.validateEnvironment()
    
    for key, value in validation.items():
        status = "✓" if value else "✗"
        print(f"  {status} {key}: {value}")
    
    return validation

def print_help():
    """Print help information"""
    help_text = """
NLM Command Line Interface (CLI)

Usage: python -m nlm.cli [command] [options]

Commands:
  create-brain [options]     Create a brain with specified configuration
  create-system [options]     Create complete NLM system (brain, world, agent)
  run [options]              Run simulation
  analyze [options]          Analyze brain network
  benchmark [options]        Benchmark brain performance
  create-env [options]       Create test environment
  validate [options]         Validate environment
  save-results <file>         Save results to JSON file
  load-results <file>         Load results from JSON file

Options:
  --config-file FILE          Configuration file path
  --neuron-count N            Number of neurons (default: 1000)
  --region-count R            Number of regions (default: 1)
  --steps N                   Number of simulation steps (default: 1000)
  --verbose                   Enable verbose output
  --output FILE               Output file

Examples:
  python -m nlm.cli create-brain --neuron-count 5000
  python -m nlm.cli create-system --config-file my_config.json
  python -m nlm.cli run --steps 5000 --verbose
  python -m nlm.cli benchmark --neurons "1000 5000 10000"
  python -m nlm.cli analyze --output analysis.json
"""
    print(help_text)

def main():
    """Main CLI function"""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python -m nlm.cli create-brain --neuron-count 5000
  python -m nlm.cli create-system
  python -m nlm.cli run --steps 5000 --verbose
  python -m nlm.cli benchmark --neurons 1000,5000,10000
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create brain command
    parser_create_brain = subparsers.add_parser('create-brain', help='Create a brain')
    parser_create_brain.add_argument('--config-file', help='Configuration file path')
    parser_create_brain.add_argument('--neuron-count', type=int, default=1000, help='Number of neurons')
    parser_create_brain.add_argument('--region-count', type=int, default=1, help='Number of regions')
    parser_create_brain.add_argument('--output', help='Output file for brain config')
    
    # Create system command
    parser_create_system = subparsers.add_parser('create-system', help='Create complete NLM system')
    parser_create_system.add_argument('--config-file', help='Configuration file path')
    parser_create_system.add_argument('--output', help='Output file for system config')
    
    # Run simulation command
    parser_run = subparsers.add_parser('run', help='Run simulation')
    parser_run.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    parser_run.add_argument('--verbose', action='store_true', help='Enable verbose output')
    parser_run.add_argument('--output', help='Output file for results')
    
    # Analyze brain command
    parser_analyze = subparsers.add_parser('analyze', help='Analyze brain network')
    parser_analyze.add_argument('--output', help='Output file for analysis')
    
    # Benchmark command
    parser_benchmark = subparsers.add_parser('benchmark', help='Benchmark brain performance')
    parser_benchmark.add_argument('--neurons', type=str, default='1000,5000,10000', 
                                 help='Comma-separated list of neuron counts')
    parser_benchmark.add_argument('--steps', type=int, default=1000, help='Steps per test')
    parser_benchmark.add_argument('--output', help='Output file for benchmark results')
    
    # Create environment command
    parser_create_env = subparsers.add_parser('create-env', help='Create test environment')
    parser_create_env.add_argument('--width', type=int, default=20, help='World width')
    parser_create_env.add_argument('--height', type=int, default=20, help='World height')
    parser_create_env.add_argument('--resources', type=int, default=5, help='Number of resources')
    parser_create_env.add_argument('--hazards', type=int, default=3, help='Number of hazards')
    parser_create_env.add_argument('--output', help='Output file for environment config')
    
    # Validate environment command
    parser_validate = subparsers.add_parser('validate', help='Validate environment')
    
    # Save results command
    parser_save = subparsers.add_parser('save-results', help='Save results to file')
    parser_save.add_argument('file', help='Output file')
    
    # Load results command
    parser_load = subparsers.add_parser('load-results', help='Load results from file')
    parser_load.add_argument('file', help='Input file')
    
    # Help command
    parser_help = subparsers.add_parser('help', help='Show help information')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    # Route to appropriate function
    try:
        if args.command == 'create-brain':
            brain = create_brain_from_config(args.config_file, args.neuron_count, args.region_count)
            if brain and args.output:
                # Save brain configuration
                config = brain.getConfig()
                with open(args.output, 'w') as f:
                    json.dump(config.exportToDict(), f, indent=2)
        
        elif args.command == 'create-system':
            brain, world, agent = create_complete_system(args.config_file)
            if brain and args.output:
                # Save system configuration
                system_config = {
                    'brain': {
                        'neuron_count': brain.getTotalNeuronCount(),
                        'region_count': brain.getRegionCount()
                    },
                    'world': {
                        'width': world.getWidth(),
                        'height': world.getHeight()
                    }
                }
                with open(args.output, 'w') as f:
                    json.dump(system_config, f, indent=2)
        
        elif args.command == 'run':
            # Create system if not provided
            brain, world, agent = create_complete_system(args.config_file)
            if brain and world and agent:
                brain, world, agent = run_simulation(brain, world, agent, args.steps, args.verbose)
                if args.output:
                    # Save simulation results
                    results = {
                        'brain_stats': brain.getBrainStats(),
                        'simulation_steps': args.steps,
                        'final_firing_rate': brain.getAverageFiringRate()
                    }
                    with open(args.output, 'w') as f:
                        json.dump(results, f, indent=2)
        
        elif args.command == 'analyze':
            brain = create_brain_from_config(args.config_file)
            if brain:
                analyze_brain(brain)
                if args.output:
                    with open(args.output, 'w') as f:
                        json.dump({
                            'brain_stats': brain.getBrainStats(),
                            'connectivity': brain.getConnectivityStats(),
                            'distribution': brain.getNeuronDistribution()
                        }, f, indent=2)
        
        elif args.command == 'benchmark':
            neuron_counts = [int(x.strip()) for x in args.neurons.split(',')]
            results = benchmark_brain(neuron_counts, args.steps)
            if args.output:
                save_results(results, args.output)
        
        elif args.command == 'create-env':
            world = create_test_environment(args.width, args.height, args.resources, args.hazards)
            if args.output:
                # Save environment configuration
                env_config = {
                    'width': args.width,
                    'height': args.height,
                    'resources': args.resources,
                    'hazards': args.hazards
                }
                with open(args.output, 'w') as f:
                    json.dump(env_config, f, indent=2)
        
        elif args.command == 'validate':
            world = create_test_environment(20, 20, 5, 3)
            if world:
                validate_environment(world)
        
        elif args.command == 'save-results':
            # This is handled by the save-results function
            print("Please use 'save-results' with a file argument")
        
        elif args.command == 'load-results':
            # This is handled by the load-results function
            print("Please use 'load-results' with a file argument")
        
        elif args.command == 'help':
            print_help()
        
        else:
            print(f"Unknown command: {args.command}")
            parser.print_help()
    
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == '__main__':
    sys.exit(main())