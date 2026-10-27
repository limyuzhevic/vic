#!/usr/bin/env python3
"""
Advanced NLM Command Line Interface for Power Users

This script provides advanced command-line interfaces for NLM users,
including batch processing, configuration management, and performance analysis.
"""

import argparse
import sys
import json
import time
import tempfile
import os
from pathlib import Path

# Add the python directory to path
sys.path.insert(0, str(Path(__file__).parent / 'python'))

try:
    import pynlm_enhanced
    print("✓ NLM Enhanced bindings available")
except ImportError:
    print("⚠ NLM Enhanced bindings not available")
    sys.exit(1)

def create_brain_from_args(config_file=None, neuron_count=1000, synapse_density=0.1):
    """Create a brain from command line arguments."""
    config = pynlm_enhanced.createEnhancedConfig()
    
    # Set configuration from arguments
    config.set("brain.neuron_count", neuron_count)
    config.set("brain.synapse_density", synapse_density)
    
    # Load configuration from file if provided
    if config_file and Path(config_file).exists():
        try:
            config.loadFromJSON(config_file)
            print(f"✓ Loaded configuration from {config_file}")
        except Exception as e:
            print(f"⚠ Could not load configuration from {config_file}: {e}")
    
    # Create brain
    brain = pynlm_enhanced.createEnhancedBrain(config)
    brain.initialize()
    
    print(f"✓ Created brain with {brain.getBrain().getTotalNeuronCount()} neurons")
    print(f"✓ Configuration: {config.summary()}")
    
    return brain, config

def run_simulation(args):
    """Run simulation with given arguments."""
    print(f"\n=== Running Simulation ===")
    print(f"Neurons: {args.neuron_count}, Steps: {args.steps}")
    print(f"Batch size: {args.batch_size}, Checkpoint: {args.checkpoint}")
    
    brain, config = create_brain_from_args(
        args.config_file, args.neuron_count, args.synapse_density
    )
    
    # Start performance monitoring
    brain.startPerformanceMonitoring()
    
    # Run simulation
    start_time = time.time()
    
    if args.batch_size > 1:
        # Run in batches
        for batch in range(0, args.steps, args.batch_size):
            batch_steps = min(args.batch_size, args.steps - batch)
            for i in range(batch_steps):
                brain.step(batch + i)
                
            # Save checkpoint if requested
            if args.checkpoint and (batch + batch_steps) % args.checkpoint == 0:
                checkpoint_file = f"checkpoint_batch_{batch // args.batch_size}.nlm"
                brain.save(checkpoint_file)
                print(f"✓ Saved checkpoint: {checkpoint_file}")
    else:
        # Run single steps
        for i in range(args.steps):
            brain.step(i)
    
    end_time = time.time()
    
    # Stop monitoring
    brain.stopPerformanceMonitoring()
    
    # Generate performance report
    metrics = brain.getPerformanceMetrics()
    report = brain.generatePerformanceReport()
    
    print(f"\n=== Simulation Results ===")
    print(f"Simulation time: {end_time - start_time:.2f}s")
    print(f"Steps completed: {args.steps}")
    print(f"Average step time: {(end_time - start_time) / args.steps:.6f}s")
    print(f"Performance metrics:")
    print(f"  - CPU usage: {metrics.cpuUsage:.1f}%")
    print(f"  - Memory usage: {metrics.memoryUsage:.1f}%")
    print(f"  - Total spikes: {metrics.spikeCount}")
    print(f"  - Active neurons: {metrics.activeNeuronCount}")
    print(f"  - Average firing rate: {metrics.averageFiringRate:.3f} Hz")
    
    # Save metrics to file
    if args.output:
        with open(args.output, 'w') as f:
            json.dump({
                'simulation_time': end_time - start_time,
                'steps_completed': args.steps,
                'average_step_time': (end_time - start_time) / args.steps,
                'performance_metrics': metrics.__dict__ if hasattr(metrics, '__dict__') else vars(metrics),
                'brain_stats': {
                    'neurons': brain.getBrain().getTotalNeuronCount(),
                    'spikes': brain.getBrain().getTotalSpikeCount(),
                    'firing_rate': brain.getBrain().getAverageFiringRate()
                }
            }, f, indent=2)
        print(f"✓ Metrics saved to {args.output}")
    
    # Save performance metrics
    if args.metrics_file:
        brain.exportMetrics(args.metrics_file)
        print(f"✓ Performance metrics saved to {args.metrics_file}")
    
    # Display full performance report
    if args.verbose:
        print(f"\n=== Performance Report ===")
        print(report)
    
    return brain

def compare_configurations(args):
    """Compare different configurations."""
    print(f"\n=== Configuration Comparison ===")
    
    configurations = []
    
    # Parse configuration files
    for config_file in args.configs:
        try:
            config = pynlm_enhanced.createEnhancedConfig()
            config.loadFromFile(config_file)
            configurations.append({
                'name': Path(config_file).stem,
                'config': config
            })
            print(f"✓ Loaded configuration: {config_file}")
        except Exception as e:
            print(f"⚠ Could not load {config_file}: {e}")
    
    # Parse inline configurations
    if args.inline_configs:
        for i, inline_config in enumerate(args.inline_configs):
            config = pynlm_enhanced.createEnhancedConfig()
            
            # Parse key=value pairs
            for pair in inline_config.split(','):
                if '=' in pair:
                    key, value = pair.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Try to convert to appropriate type
                    try:
                        if value.lower() == 'true':
                            val = True
                        elif value.lower() == 'false':
                            val = False
                        elif '.' in value:
                            val = float(value)
                        else:
                            val = int(value)
                    except:
                        val = value
                    
                    config.set(key, val)
            
            configurations.append({
                'name': f"inline_{i}",
                'config': config
            })
            print(f"✓ Created inline configuration: {inline_config}")
    
    # Run comparison
    for config_info in configurations:
        print(f"\n--- Testing: {config_info['name']} ---")
        
        brain = pynlm_enhanced.createEnhancedBrain(config_info['config'])
        brain.initialize()
        
        # Run simulation
        brain.startPerformanceMonitoring()
        for i in range(args.steps):
            brain.step(i)
        brain.stopPerformanceMonitoring()
        
        metrics = brain.getPerformanceMetrics()
        
        print(f"  - Neurons: {brain.getBrain().getTotalNeuronCount()}")
        print(f"  - Spikes: {metrics.spikeCount}")
        print(f"  - Simulation time: {metrics.simulationTime:.2f}s")
        print(f"  - Average step time: {metrics.simulationTime / args.steps:.6f}s")
        print(f"  - Firing rate: {metrics.averageFiringRate:.3f} Hz")
    
    print(f"\n=== Comparison Complete ===")
    print(f"Tested {len(configurations)} configurations")

def validate_configuration(args):
    """Validate configuration file."""
    print(f"\n=== Configuration Validation ===")
    
    config = pynlm_enhanced.createEnhancedConfig()
    
    try:
        config.loadFromFile(args.config_file)
        print(f"✓ Loaded configuration from {args.config_file}")
    except Exception as e:
        print(f"✗ Failed to load configuration: {e}")
        return
    
    # Validate configuration
    errors = config.validate()
    
    if errors:
        print(f"⚠ Configuration validation issues:")
        for error in errors:
            print(f"  - {error}")
    else:
        print(f"✓ Configuration validation passed")
    
    # Show configuration summary
    print(f"\nConfiguration summary:")
    print(config.summary())

def create_experiment(args):
    """Create and run experiment."""
    print(f"\n=== Creating Experiment ===")
    
    # Create experiment configuration
    config = pynlm_enhanced.createEnhancedConfig()
    
    # Set experiment parameters
    config.set("experiment.name", args.experiment_name)
    config.set("experiment.description", args.description)
    config.set("brain.neuron_count", args.neuron_count)
    config.set("brain.synapse_density", args.synapse_density)
    
    # Create brain
    brain = pynlm_enhanced.createEnhancedBrain(config)
    brain.initialize()
    
    # Start monitoring
    brain.startPerformanceMonitoring()
    
    # Run experiment
    print(f"Running experiment: {args.experiment_name}")
    print(f"Description: {args.description}")
    print(f"Parameters: {args.neuron_count} neurons, {args.steps} steps")
    
    start_time = time.time()
    
    for i in range(args.steps):
        brain.step(i)
        
        # Log progress
        if args.verbose and i % 100 == 0:
            print(f"  Step {i}/{args.steps}")
    
    end_time = time.time()
    
    # Stop monitoring
    brain.stopPerformanceMonitoring()
    
    # Save experiment data
    experiment_data = {
        'name': args.experiment_name,
        'description': args.description,
        'parameters': {
            'neuron_count': args.neuron_count,
            'synapse_density': args.synapse_density,
            'steps': args.steps
        },
        'results': {
            'simulation_time': end_time - start_time,
            'steps_completed': args.steps,
            'performance_metrics': {
                'cpu_usage': brain.getPerformanceMetrics().cpuUsage,
                'memory_usage': brain.getPerformanceMetrics().memoryUsage,
                'spike_count': brain.getPerformanceMetrics().spikeCount,
                'average_firing_rate': brain.getPerformanceMetrics().averageFiringRate
            },
            'brain_stats': {
                'total_neurons': brain.getBrain().getTotalNeuronCount(),
                'total_spikes': brain.getBrain().getTotalSpikeCount(),
                'average_firing_rate': brain.getBrain().getAverageFiringRate()
            }
        }
    }
    
    # Save to JSON file
    output_file = f"experiment_{args.experiment_name}_{int(time.time())}.json"
    with open(output_file, 'w') as f:
        json.dump(experiment_data, f, indent=2)
    
    print(f"✓ Experiment completed and saved to {output_file}")
    
    # Generate report
    if args.verbose:
        print(f"\n=== Experiment Report ===")
        print(f"Experiment: {experiment_data['name']}")
        print(f"Description: {experiment_data['description']}")
        print(f"Simulation time: {experiment_data['results']['simulation_time']:.2f}s")
        print(f"Performance metrics: {json.dumps(experiment_data['results']['performance_metrics'], indent=2)}")
    
    return brain, experiment_data

def main():
    """Main command line interface."""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run a simple simulation
  python3 nlm_cli.py simulate --neuron-count 1000 --steps 500
  
  # Run with batch processing and checkpoints
  python3 nlm_cli.py simulate --neuron-count 500 --steps 1000 --batch-size 100 --checkpoint 100 --output metrics.json
  
  # Compare different configurations
  python3 nlm_cli.py compare config1.json config2.json
  
  # Validate configuration
  python3 nlm_cli.py validate config.json
  
  # Create experiment
  python3 nlm_cli.py experiment "test_experiment" "Test neural network" --neuron-count 200 --steps 1000
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Simulate command
    simulate_parser = subparsers.add_parser('simulate', help='Run simulation')
    simulate_parser.add_argument('--neuron-count', type=int, default=1000, help='Number of neurons')
    simulate_parser.add_argument('--synapse-density', type=float, default=0.1, help='Synapse density (0.0-1.0)')
    simulate_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    simulate_parser.add_argument('--batch-size', type=int, default=1, help='Batch size for parallel execution')
    simulate_parser.add_argument('--checkpoint', type=int, default=0, help='Checkpoint interval (0 to disable)')
    simulate_parser.add_argument('--config-file', help='Configuration file path')
    simulate_parser.add_argument('--output', help='Output file for metrics (JSON format)')
    simulate_parser.add_argument('--metrics-file', help='Output file for performance metrics (CSV format)')
    simulate_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    # Compare command
    compare_parser = subparsers.add_parser('compare', help='Compare configurations')
    compare_parser.add_argument('configs', nargs='+', help='Configuration files to compare')
    compare_parser.add_argument('--inline-config', action='append', help='Inline configuration (key=value pairs)')
    compare_parser.add_argument('--steps', type=int, default=100, help='Number of steps for comparison')
    
    # Validate command
    validate_parser = subparsers.add_parser('validate', help='Validate configuration file')
    validate_parser.add_argument('config-file', help='Configuration file to validate')
    
    # Experiment command
    experiment_parser = subparsers.add_parser('experiment', help='Create and run experiment')
    experiment_parser.add_argument('experiment_name', help='Name of the experiment')
    experiment_parser.add_argument('description', help='Description of the experiment')
    experiment_parser.add_argument('--neuron-count', type=int, default=100, help='Number of neurons')
    experiment_parser.add_argument('--synapse-density', type=float, default=0.1, help='Synapse density')
    experiment_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    experiment_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    try:
        if args.command == 'simulate':
            run_simulation(args)
        elif args.command == 'compare':
            compare_configurations(args)
        elif args.command == 'validate':
            validate_configuration(args)
        elif args.command == 'experiment':
            create_experiment(args)
        
        print(f"\n✓ Command '{args.command}' completed successfully")
        
    except Exception as e:
        print(f"✗ Error: {e}")
        import traceback
        traceback.print_exc()
        return 1
    
    return 0

if __name__ == "__main__":
    sys.exit(main())