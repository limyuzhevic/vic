#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI) - Improved Version
===================================================

A command-line interface for interacting with NLM brains, providing
convenient access to simulation utilities and analysis tools.

This CLI demonstrates the new features added to improve NLM usability:
- Simplified brain creation and configuration
- Simulation utilities and monitoring
- Performance benchmarking
- Brain comparison and analysis
- Debug and diagnostic tools
"""

import sys
import os
import argparse
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

import pynlm
import time

def create_simple_brain(neuron_count: int = 100, region_count: int = 1) -> pynlm.Brain:
    """Create a simple brain with reduced parameters for quick testing."""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    # Configure the brain using new convenience methods
    brain.getConfig().setNeuronCount(neuron_count)
    brain.getConfig().setRegionCount(region_count)
    brain.getConfig().setConnectionProbability(0.1)
    
    return brain

def simulate_command(args):
    """Run simulation command."""
    print(f"Running simulation with {args.steps} steps...")
    brain = create_simple_brain()
    
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return 1
    
    for step in range(args.steps):
        brain.step(step)
    
    print("\nSimulation Results:")
    print(f"  Steps: {args.steps}")
    print(f"  Spike Count: {brain.getTotalSpikeCount()}")
    print(f"  Average Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Regions: {brain.getRegionCount()}")
    return 0

def benchmark_command(args):
    """Run benchmark command."""
    print(f"Benchmarking brain with {args.steps} steps...")
    brain = create_simple_brain()
    
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return 1
    
    start_time = time.time()
    
    for step in range(args.steps):
        brain.step(step)
    
    end_time = time.time()
    duration_ms = (end_time - start_time) * 1000
    steps_per_sec = args.steps / (duration_ms / 1000)
    
    print("\nBenchmark Results:")
    print(f"  Steps: {args.steps}")
    print(f"  Duration: {duration_ms:.2f} ms")
    print(f"  Steps/sec: {steps_per_sec:.2f}")
    print(f"  Spike Count: {brain.getTotalSpikeCount()}")
    print(f"  Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
    return 0

def analyze_command(args):
    """Run analyze command."""
    print("Analyzing brain configuration...")
    brain = create_simple_brain()
    
    if not brain.initialize():
        print("Error: Failed to initialize brain")
        return 1
    
    print("\nBrain Analysis:")
    print(f"  Neuron Count: {brain.getTotalNeuronCount()}")
    print(f"  Region Count: {brain.getRegionCount()}")
    print(f"  Synapse Count: {brain.getTotalSynapseCount()}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  Developmental Stage: {brain.getDevelopmentalStage()}")
    return 0

def create_command(args):
    """Run create command."""
    print(f"Creating brain with {args.neurons} neurons and {args.regions} regions...")
    brain = create_simple_brain(args.neurons, args.regions)
    
    if brain.initialize():
        print("Brain created successfully!")
        print(f"  Neurons: {brain.getTotalNeuronCount()}")
        print(f"  Regions: {brain.getRegionCount()}")
    else:
        print("Error: Failed to initialize brain")
        return 1
    return 0

def learn_command(args):
    """Run learning command."""
    print(f"Running learning simulation with {args.steps} steps...")
    brain = create_simple_brain()
    
    # Enable plasticity for learning
    brain.getConfig().setPlasticityEnabled(True)
    
    # Get neurons and apply input for learning
    if brain.getRegionCount() > 0:
        region = brain.getRegion(brain.getRegionIds()[0])
        neurons = region.getAllNeurons()
        
        # Apply input to some neurons
        if neurons:
            for i in range(min(5, len(neurons))):
                neurons[i].injectCurrent(30.0f)
    
    if brain.initialize():
        for step in range(args.steps):
            brain.step(step)
        
        print("\nLearning Simulation Results:")
        print(f"  Steps: {args.steps}")
        print(f"  Spike Count: {brain.getTotalSpikeCount()}")
        print(f"  Firing Rate: {brain.getAverageFiringRate():.2f} Hz")
        print(f"  Plasticity Enabled: {brain.getConfig().has('plasticity_enabled') and brain.getConfig().getOr<bool>('plasticity_enabled', False)}")
    else:
        print("Error: Failed to initialize brain")
        return 1
    return 0

def config_command(args):
    """Run config command."""
    print("Configuration presets:")
    print("  simple: Small brain for testing (100 neurons, 1 region)")
    print("  learning: Brain configured for learning (500 neurons, 2 regions)")
    print("  inference: Brain configured for inference (200 neurons, 1 region)")
    return 0

def version_command(args):
    """Run version command."""
    print("NLM CLI Version 1.0 - Improved with new Python bindings")
    print("Features added:")
    print("  - Convenience configuration methods")
    print("  - Simulation utilities and monitoring")
    print("  - Performance benchmarking")
    print("  - Brain comparison tools")
    print("  - Debug and diagnostic capabilities")
    return 0

def examples_command(args):
    """Run examples command."""
    print("NLM CLI Usage Examples:")
    print()
    print("Basic Usage:")
    print("  python nlm_cli.py simulate --steps 500")
    print("  python nlm_cli.py benchmark --steps 1000")
    print("  python nlm_cli.py analyze")
    print()
    print("Create Custom Brain:")
    print("  python nlm_cli.py create --neurons 200 --regions 2")
    print("  python nlm_cli.py config simple")
    print()
    print("Learning Examples:")
    print("  python nlm_cli.py learn --steps 500")
    print()
    print("All commands support --help for more details.")
    return 0

def debug_command(args):
    """Run debug command."""
    print("Debug mode enabled - detailed analysis")
    brain = create_simple_brain()
    
    if brain.initialize():
        print(f"Brain initialized successfully")
        print(f"  Neuron Count: {brain.getTotalNeuronCount()}")
        print(f"  Region Count: {brain.getRegionCount()}")
        print(f"  Synapse Count: {brain.getTotalSynapseCount()}")
        print(f"  Config: {brain.getConfig().summary()}")
    else:
        print("Error: Failed to initialize brain")
        return 1
    return 0

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface - Improved Version",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s simulate --steps 500                    # Run a simulation
  %(prog)s benchmark --steps 1000                   # Benchmark brain performance
  %(prog)s analyze                                  # Analyze brain in detail
  %(prog)s create --neurons 200 --regions 2        # Create brain with custom config
  %(prog)s learn --steps 500                        # Run learning simulation
        """)
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands', required=True)
    
    # Simulate command
    sim_parser = subparsers.add_parser('simulate', help='Run a simulation and print statistics')
    sim_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    
    # Benchmark command
    bench_parser = subparsers.add_parser('benchmark', help='Benchmark brain performance')
    bench_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    
    # Analyze command
    subparsers.add_parser('analyze', help='Analyze brain in detail')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create a brain with configuration')
    create_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')
    create_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    
    # Learn command
    learn_parser = subparsers.add_parser('learn', help='Run learning simulation with plasticity')
    learn_parser.add_argument('--steps', type=int, default=500, help='Number of simulation steps')
    
    # Config presets
    config_parser = subparsers.add_parser('config', help='Configuration presets')
    config_subparsers = config_parser.add_subparsers(dest='config_command', help='Configuration commands')
    
    simple_parser = config_subparsers.add_parser('simple', help='Simple brain configuration')
    simple_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')
    simple_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    
    learning_parser = config_subparsers.add_parser('learning', help='Learning brain configuration')
    learning_parser.add_argument('--neurons', type=int, default=500, help='Number of neurons')
    learning_parser.add_argument('--regions', type=int, default=2, help='Number of regions')
    learning_parser.add_argument('--plasticity', action='store_true', help='Enable plasticity')
    
    inference_parser = config_subparsers.add_parser('inference', help='Inference brain configuration')
    inference_parser.add_argument('--neurons', type=int, default=200, help='Number of neurons')
    inference_parser.add_argument('--regions', type=int, default=1, help='Number of regions')
    
    # Version command
    subparsers.add_parser('version', help='Show version information')
    
    # Examples command
    subparsers.add_parser('examples', help='Show usage examples')
    
    # Debug command
    subparsers.add_parser('debug', help='Enable debug output and detailed analysis')
    
    args = parser.parse_args()
    
    # Command dispatch
    try:
        if args.command == 'simulate':
            return simulate_command(args)
        elif args.command == 'benchmark':
            return benchmark_command(args)
        elif args.command == 'analyze':
            return analyze_command(args)
        elif args.command == 'create':
            return create_command(args)
        elif args.command == 'learn':
            return learn_command(args)
        elif args.command == 'config':
            return config_command(args)
        elif args.command == 'version':
            return version_command(args)
        elif args.command == 'examples':
            return examples_command(args)
        elif args.command == 'debug':
            return debug_command(args)
        else:
            print(f"Unknown command: {args.command}")
            return 1
    
    except Exception as e:
        print(f"Error: {e}")
        return 1

if __name__ == "__main__":
    sys.exit(main())
