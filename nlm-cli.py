#!/usr/bin/env python3
"""
NLM Advanced Command Line Interface (CLI)

A command-line tool for working with NLM (Neural Learning Machine) that provides
advanced functionality for power users and researchers.
"""

import argparse
import json
import sys
import time
from pathlib import Path

# Add the source directory to path to import NLM
sys.path.insert(0, str(Path(__file__).parent / "src"))

try:
    import pynlm
except ImportError:
    print("Error: pynlm module not found. Please install NLM first.")
    print("Run: pip install .")
    sys.exit(1)


def create_brain(config_file=None, neurons=1000, density=0.1):
    """Create and configure a brain."""
    config = pynlm.createDefaultConfig()
    
    if config_file:
        try:
            config.loadFromFile(config_file)
            print(f"Loaded configuration from {config_file}")
        except Exception as e:
            print(f"Warning: Could not load config file: {e}")
    
    # Override with command-line parameters
    config.set("brain.neuron_count", neurons)
    config.set("brain.synapse_density", density)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Created brain with {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
    return brain


def run_simulation(brain, steps, learning=True):
    """Run a simulation with optional learning."""
    print(f"Running simulation for {steps} steps...")
    
    start_time = time.time()
    
    for step in range(steps):
        brain.step(step)
        
        if step % 100 == 0:
            print(f"  Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"avg rate {brain.getAverageFiringRate():.2f}")
    
    end_time = time.time()
    
    print(f"Simulation complete in {end_time - start_time:.2f} seconds")
    print(f"Final stats: {brain.getTotalSpikeCount()} total spikes")


def save_brain(brain, filepath):
    """Save brain state to file."""
    try:
        brain.save(filepath)
        print(f"Brain saved to {filepath}")
        return True
    except Exception as e:
        print(f"Error saving brain: {e}")
        return False


def load_brain(filepath, neurons=1000):
    """Load brain state from file."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    try:
        brain.load(filepath)
        print(f"Brain loaded from {filepath}")
        print(f"Loaded brain has {brain.getTotalNeuronCount()} neurons and {brain.getTotalSynapseCount()} synapses")
        return brain
    except Exception as e:
        print(f"Error loading brain: {e}")
        return None


def compare_brains(file1, file2, neurons=1000):
    """Compare two saved brains."""
    config = pynlm.createDefaultConfig()
    config.set("brain.neuron_count", neurons)
    
    brain1 = pynlm.createBrain(config)
    brain1.initialize()
    brain1.load(file1)
    
    brain2 = pynlm.createBrain(config)
    brain2.initialize()
    brain2.load(file2)
    
    print("Brain Comparison:")
    print(f"  Brain 1: {brain1.getTotalNeuronCount()} neurons, {brain1.getTotalSynapseCount()} synapses")
    print(f"  Brain 2: {brain2.getTotalNeuronCount()} neurons, {brain2.getTotalSynapseCount()} synapses")
    
    # Compare spike counts
    spikes1 = brain1.getTotalSpikeCount()
    spikes2 = brain2.getTotalSpikeCount()
    print(f"  Total spikes: Brain 1 = {spikes1}, Brain 2 = {spikes2}")
    
    if spikes1 > 0 and spikes2 > 0:
        diff = abs(spikes1 - spikes2) / max(spikes1, spikes2) * 100
        print(f"  Spike count difference: {diff:.1f}%")
    
    return brain1, brain2


def analyze_brain(brain, output_file=None):
    """Analyze brain and save statistics."""
    stats = {
        'total_neurons': brain.getTotalNeuronCount(),
        'total_synapses': brain.getTotalSynapseCount(),
        'firing_neurons': brain.getFiringNeuronCount(),
        'total_spikes': brain.getTotalSpikeCount(),
        'average_firing_rate': brain.getAverageFiringRate(),
        'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
        'developmental_stage': brain.getDevelopmentalStage(),
    }
    
    print("Brain Analysis:")
    for key, value in stats.items():
        print(f"  {key}: {value}")
    
    if output_file:
        with open(output_file, 'w') as f:
            json.dump(stats, f, indent=2)
        print(f"Analysis saved to {output_file}")
    
    return stats


def list_available_configs():
    """List available configuration files."""
    config_dir = Path("configs")
    if not config_dir.exists():
        print("No configs directory found.")
        return
    
    config_files = list(config_dir.glob("*.cfg"))
    if not config_files:
        print("No configuration files found.")
        return
    
    print("Available configuration files:")
    for config_file in sorted(config_files):
        print(f"  - {config_file.name}")


def benchmark_brain(neurons_list=[100, 500, 1000, 2000], steps=1000):
    """Benchmark brain performance."""
    print("NLM Performance Benchmark")
    print("=" * 50)
    
    results = []
    
    for neurons in neurons_list:
        print(f"\nBenchmarking {neurons} neurons...")
        
        config = pynlm.createDefaultConfig()
        config.set("brain.neuron_count", neurons)
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        
        total_time = end_time - start_time
        steps_per_sec = steps / total_time
        
        result = {
            'neurons': neurons,
            'steps': steps,
            'total_time': total_time,
            'steps_per_second': steps_per_sec,
            'spikes_per_step': brain.getTotalSpikeCount() / steps,
            'synapses_per_neuron': brain.getTotalSynapseCount() / neurons,
        }
        
        results.append(result)
        
        print(f"  Time: {total_time:.2f}s")
        print(f"  Steps/sec: {steps_per_sec:.0f}")
        print(f"  Spikes/step: {result['spikes_per_step']:.2f}")
        print(f"  Synapses/neuron: {result['synapses_per_neuron']:.2f}")
    
    print("\n" + "=" * 50)
    print("Benchmark Summary:")
    for result in results:
        print(f"  {result['neurons']:5d} neurons: {result['steps_per_second']:8.0f} steps/sec")
    
    return results


def main():
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli create --neurons 2000 --steps 500
  nlm-cli save brain.bin
  nlm-cli load brain.bin --neurons 1000
  nlm-cli benchmark
  nlm-cli analyze brain.bin
  nlm-cli compare brain1.bin brain2.bin
        ""
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create a new brain')
    create_parser.add_argument('--config', '-c', help='Configuration file')
    create_parser.add_argument('--neurons', '-n', type=int, default=1000, help='Number of neurons')
    create_parser.add_argument('--density', '-d', type=float, default=0.1, help='Synapse density')
    
    # Simulate command
    sim_parser = subparsers.add_parser('simulate', help='Run a simulation')
    sim_parser.add_argument('--steps', '-s', type=int, required=True, help='Number of steps')
    sim_parser.add_argument('--learning', action='store_true', help='Enable learning')
    sim_parser.add_argument('--brain-file', help='Brain file to load (for simulation)')
    
    # Save command
    save_parser = subparsers.add_parser('save', help='Save brain state')
    save_parser.add_argument('filepath', help='Filepath to save brain')
    
    # Load command
    load_parser = subparsers.add_parser('load', help='Load brain state')
    load_parser.add_argument('filepath', help='Filepath to load brain from')
    load_parser.add_argument('--neurons', '-n', type=int, default=1000, help='Number of neurons')
    
    # Compare command
    compare_parser = subparsers.add_parser('compare', help='Compare two brain files')
    compare_parser.add_argument('file1', help='First brain file')
    compare_parser.add_argument('file2', help='Second brain file')
    compare_parser.add_argument('--neurons', '-n', type=int, default=1000, help='Number of neurons')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain state')
    analyze_parser.add_argument('filepath', help='Brain file to analyze')
    analyze_parser.add_argument('--output', '-o', help='Output file for analysis')
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmark')
    benchmark_parser.add_argument('--neurons', '-n', nargs='+', type=int, default=[100, 500, 1000, 2000], help='Neuron counts to benchmark')
    benchmark_parser.add_argument('--steps', '-s', type=int, default=1000, help='Steps per benchmark')
    
    # List configs command
    list_parser = subparsers.add_parser('list-configs', help='List available configuration files')
    
    # Status command
    status_parser = subparsers.add_parser('status', help='Show current system status')
    
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    try:
        if args.command == 'create':
            brain = create_brain(args.config, args.neurons, args.density)
            print("Brain created successfully!")
            
        elif args.command == 'simulate':
            if args.brain_file:
                brain = load_brain(args.brain_file, args.neurons)
                if not brain:
                    return
            else:
                brain = create_brain(None, args.neurons)
            
            run_simulation(brain, args.steps, args.learning)
            
        elif args.command == 'save':
            # Load brain first
            brain = create_brain(None, 1000)
            save_brain(brain, args.filepath)
            
        elif args.command == 'load':
            brain = load_brain(args.filepath, args.neurons)
            if brain:
                analyze_brain(brain)
                
        elif args.command == 'compare':
            compare_brains(args.file1, args.file2, args.neurons)
            
        elif args.command == 'analyze':
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1000)
            brain = pynlm.createBrain(config)
            brain.initialize()
            brain.load(args.filepath)
            analyze_brain(brain, args.output)
            
        elif args.command == 'benchmark':
            benchmark_brain(args.neurons, args.steps)
            
        elif args.command == 'list-configs':
            list_available_configs()
            
        elif args.command == 'status':
            print("NLM CLI Status")
            print("=" * 30)
            print(f"Python version: {sys.version}")
            print(f"NLM version: {getattr(pynlm, '__version__', 'Unknown')}")
            
            # Test basic functionality
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            print(f"Basic functionality test: {brain.getTotalNeuronCount()} neurons created")
            
    except KeyboardInterrupt:
        print("\nOperation interrupted by user")
    except Exception as e:
        print(f"Error: {e}")
        import traceback
        traceback.print_exc()
        sys.exit(1)


if __name__ == "__main__":
    main()
