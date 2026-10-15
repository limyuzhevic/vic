#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

Command-line tools for NLM operations including training, configuration management,
benchmarking, and analysis.
"""

import argparse
import json
import sys
import os
import time
import threading
import numpy as np
from pathlib import Path
from typing import Dict, List, Optional

# Import NLM modules
import pynlm
from convenience_api import (
    createQuickBrain, 
    createTrainingEnvironment, 
    configureFromPreset,
    BrainManager,
    ExperimentManager,
    PerformanceProfiler,
    runLearningEpisode,
    validateBrainHealth
)

def train_brain(args):
    """Train a brain using various methods."""
    print(f"Training brain with preset: {args.preset}")
    
    # Create brain
    config = configureFromPreset(args.preset)
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Create environment if requested
    if args.mode == 'interactive':
        env = createTrainingEnvironment(args.width, args.height)
        agent = env['agent']
        world = env['world']
    else:
        # Simple training without environment
        agent = None
        world = None
    
    # Training loop
    start_time = time.time()
    
    for step in range(args.steps):
        if args.mode == 'interactive':
            # Interactive training with agent and world
            world.update(0.1)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
        else:
            # Simple brain training
            brain.step(step)
            
            # Inject some input if requested
            if args.inject_sensory:
                # Create a simple sensory input
                from convenience_api import createQuickBrain
                # For now, just create a dummy vision input
                # In a real implementation, this would use actual Vision class
                print("Sensory injection requested (demo mode)")
        
        # Log progress
        if args.log_interval > 0 and (step + 1) % args.log_interval == 0:
            print(f"Step {step + 1}/{args.steps}: "
                  f"Firing: {brain.getFiringNeuronCount()}, "
                  f"Spikes: {brain.getTotalSpikeCount()}, "
                  f"Avg Rate: {brain.getAverageFiringRate():.2f}")
    
    end_time = time.time()
    duration = end_time - start_time
    
    print(f"\nTraining completed in {duration:.2f} seconds")
    print(f"Final Statistics:")
    print(f"  Total Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total Synapses: {brain.getTotalSynapseCount()}")
    print(f"  Total Spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average Firing Rate: {brain.getAverageFiringRate():.2f}")
    print(f"  E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}")
    
    # Save if requested
    if args.output:
        brain.save(args.output)
        print(f"\nBrain saved to: {args.output}")
    
    return brain

def config_commands(args):
    """Handle configuration commands."""
    if args.preset:
        # Show preset configuration
        config = configureFromPreset(args.preset)
        print(f"\nPreset Configuration: {args.preset}")
        print("=" * 50)
        
        # Extract config info
        info = {
            'brain.neuron_count': config.getOr('brain.neuron_count', 'Not set'),
            'brain.connection_probability': config.getOr('brain.connection_probability', 'Not set'),
            'plasticity.stdp.learning_rate': config.getOr('plasticity.stdp.learning_rate', 'Not set'),
            'plasticity.hebbian.learning_rate': config.getOr('plasticity.hebbian.learning_rate', 'Not set'),
            'neuromod.curiosity.enable': config.getOr('neuromod.curiosity.enable', 'Not set'),
            'neuromod.novelty.enable': config.getOr('neuromod.novelty.enable', 'Not set'),
            'neuromod.dopamine.scale': config.getOr('neuromod.dopamine.scale', 'Not set'),
            'simulation_timestep': config.getOr('simulation_timestep', 'Not set'),
            'random_seed': config.getOr('random_seed', 'Not set')
        }
        
        print(json.dumps(info, indent=2))
    
    elif args.validate:
        # Validate configuration file
        try:
            with open(args.validate, 'r') as f:
                config_data = json.load(f)
            
            print(f"\nConfiguration validation: {args.validate}")
            print("Validation passed - valid JSON format")
            
            # Convert to NLM config if possible
            config = pynlm.createDefaultConfig()
            
            for key, value in config_data.items():
                # Simple conversion - assumes standard key format
                if '.' in key:
                    config.set(key, value)
            
            print("Configuration can be loaded into NLM")
            
        except Exception as e:
            print(f"Validation failed: {e}")
            return False
    
    elif args.export:
        # Export configuration to file
        config = configureFromPreset(args.preset or 'quick')
        
        export_data = {
            'preset': args.preset or 'quick',
            'timestamp': time.time(),
            'config': {
                'brain.neuron_count': config.getOr('brain.neuron_count', 500),
                'brain.connection_probability': config.getOr('brain.connection_probability', 0.1),
                'plasticity.stdp.learning_rate': config.getOr('plasticity.stdp.learning_rate', 0.001),
                'plasticity.hebbian.learning_rate': config.getOr('plasticity.hebbian.learning_rate', 0.001),
                'neuromod.curiosity.enable': config.getOr('neuromod.curiosity.enable', True),
                'neuromod.novelty.enable': config.getOr('neuromod.novelty.enable', True),
                'neuromod.dopamine.scale': config.getOr('neuromod.dopamine.scale', 1.0),
                'simulation_timestep': config.getOr('simulation_timestep', 0.001),
                'random_seed': config.getOr('random_seed', 42)
            }
        }
        
        with open(args.export, 'w') as f:
            json.dump(export_data, f, indent=2)
        
        print(f"Configuration exported to: {args.export}")
    
    elif args.import_file:
        # Import configuration from file
        try:
            with open(args.import_file, 'r') as f:
                import_data = json.load(f)
            
            # Convert back to NLM config
            config = pynlm.createDefaultConfig()
            
            if 'config' in import_data:
                for key, value in import_data['config'].items():
                    config.set(key, value)
            
            print(f"Configuration imported from: {args.import_file}")
            print(f"Config preview: {config.getOr('brain.neuron_count', 'N/A')} neurons")
            
        except Exception as e:
            print(f"Import failed: {e}")
            return False
    
    return True

def benchmark_command(args):
    """Run performance benchmarking."""
    print(f"Starting benchmark with {args.neurons} neurons for {args.steps} steps")
    
    # Create brain
    config = configureFromPreset(args.preset)
    config.set("brain.neuron_count", args.neurons)
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Setup profiler
    profiler = PerformanceProfiler()
    
    # Benchmark
    start_time = time.time()
    
    for step in range(args.steps):
        # Profile the step
        profiler.profile_step(brain, f"step_{step}")
        
        # Perform step
        brain.step(step)
        
        # Log progress
        if (step + 1) % args.log_interval == 0:
            print(f"Step {step + 1}/{args.steps} - "
                  f"Duration: {profiler.metrics[-1]['duration']:.4f}s, "
                  f"Spikes/sec: {profiler.metrics[-1]['spikes_per_second']:.0f}")
    
    end_time = time.time()
    total_duration = end_time - start_time
    
    # Generate benchmark results
    results = {
        'benchmark_info': {
            'neurons': args.neurons,
            'steps': args.steps,
            'preset': args.preset,
            'start_time': time.time(),
            'end_time': end_time,
            'total_duration': total_duration,
            'avg_step_duration': np.mean([m['duration'] for m in profiler.metrics]),
            'avg_spikes_per_sec': np.mean([m['spikes_per_second'] for m in profiler.metrics]),
            'total_spikes': sum(m['spikes_generated'] for m in profiler.metrics),
            'memory_usage': 0.0,  # Would need actual memory measurement
        },
        'step_metrics': profiler.metrics,
        'final_brain_stats': {
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'avg_firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    }
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        print(f"Benchmark results saved to: {args.output}")
    
    # Print summary
    print(f"\nBenchmark Results:")
    print(f"  Total Duration: {total_duration:.2f}s")
    print(f"  Average Step Duration: {results['benchmark_info']['avg_step_duration']:.4f}s")
    print(f"  Average Spikes per Second: {results['benchmark_info']['avg_spikes_per_sec']:.0f}")
    print(f"  Total Spikes Generated: {results['benchmark_info']['total_spikes']}")
    print(f"  Final Brain Stats: {results['final_brain_stats']}")
    
    return results

def profile_command(args):
    """Profile a specific brain."""
    if not args.brain:
        print("Error: --brain argument is required for profile command")
        return False
    
    print(f"Profiling brain: {args.brain}")
    
    # Load brain
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.load(args.brain)
    
    # Setup profiler
    profiler = PerformanceProfiler()
    
    # Profile
    start_time = time.time()
    
    for step in range(args.steps):
        profiler.profile_step(brain, f"profile_step_{step}")
        brain.step(step)
        
        if (step + 1) % args.log_interval == 0:
            print(f"Step {step + 1}/{args.steps} - "
                  f"Duration: {profiler.metrics[-1]['duration']:.4f}s")
    
    end_time = time.time()
    
    # Generate profile results
    results = {
        'profile_info': {
            'brain_file': args.brain,
            'steps_profiled': args.steps,
            'detailed': args.detailed,
            'start_time': start_time,
            'end_time': end_time,
            'total_duration': end_time - start_time,
            'final_metrics': {
                'total_neurons': brain.getTotalNeuronCount(),
                'total_synapses': brain.getTotalSynapseCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'avg_firing_rate': brain.getAverageFiringRate(),
                'e_i_ratio': brain.getExcitationInhibitionRatio(),
                'total_spikes': brain.getTotalSpikeCount()
            }
        },
        'step_metrics': profiler.metrics
    }
    
    # Save results if requested
    if args.output:
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        print(f"Profile results saved to: {args.output}")
    
    # Print summary
    print(f"\nProfile Results:")
    print(f"  Total Duration: {results['profile_info']['total_duration']:.2f}s")
    print(f"  Average Step Duration: {np.mean([m['duration'] for m in profiler.metrics]):.4f}s")
    print(f"  Total Spikes Generated: {brain.getTotalSpikeCount()}")
    
    return results

def utils_command(args):
    """Handle utility operations."""
    if args.validate == 'brain':
        if not args.file:
            print("Error: --file argument is required for brain validation")
            return False
        
        try:
            brain = pynlm.createBrain(pynlm.createDefaultConfig())
            brain.load(args.file)
            
            health = validateBrainHealth(brain)
            
            print(f"\nBrain Validation: {args.file}")
            print(f"  Status: {'HEALTHY' if health['is_healthy'] else 'UNHEALTHY'}")
            print(f"  Neurons: {health['neuron_count']}")
            print(f"  Synapses: {health['synapse_count']}")
            print(f"  Firing: {health['firing_count']}")
            print(f"  Avg Firing Rate: {health['avg_firing_rate']:.2f}")
            print(f"  E/I Ratio: {health['e_i_ratio']:.3f}")
            
            if health['warnings']:
                print("\nWarnings:")
                for warning in health['warnings']:
                    print(f"  - {warning}")
            
            if health['errors']:
                print("\nErrors:")
                for error in health['errors']:
                    print(f"  - {error}")
            
        except Exception as e:
            print(f"Validation failed: {e}")
            return False
    
    elif args.validate == 'config':
        if not args.file:
            print("Error: --file argument is required for config validation")
            return False
        
        try:
            # Try to load as JSON
            with open(args.file, 'r') as f:
                json.load(f)
            
            print(f"\nConfiguration validation: {args.file}")
            print("Format: Valid JSON")
            print("Note: To load as NLM config, use 'nlm config --import'")
            
        except Exception as e:
            print(f"Validation failed: {e}")
            return False
    
    elif args.validate == 'all':
        print("Comprehensive validation (not implemented)")
    
    return True

def main():
    """Main CLI entry point."""
    parser = argparse.ArgumentParser(
        description='NLM (Neural Learning Machine) Command Line Interface',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Commands:
  train       Train a brain with different modes and configurations
  config      Configuration management (presets, validation, export/import)
  benchmark   Performance benchmarking
  profile     Brain profiling
  utils       Utility operations (validation, etc.)

Examples:
  nlm train --preset research --steps 1000 --output brain.bin
  nlm config --preset production --export config.json
  nlm benchmark --neurons 2000 --steps 5000
  nlm profile --brain mybrain.bin --steps 100
  nlm utils --validate brain --file mybrain.bin
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Train command
    train_parser = subparsers.add_parser('train', help='Train a brain')
    train_parser.add_argument('--preset', choices=['quick', 'research', 'production', 'debug', 'minimal'],
                            default='quick', help='Configuration preset')
    train_parser.add_argument('--mode', choices=['simple', 'interactive'], default='simple',
                            help='Training mode')
    train_parser.add_argument('--steps', type=int, default=100, help='Number of training steps')
    train_parser.add_argument('--log-interval', type=int, default=50, help='Logging interval')
    train_parser.add_argument('--width', type=int, default=20, help='World width (interactive mode)')
    train_parser.add_argument('--height', type=int, default=20, help='World height (interactive mode)')
    train_parser.add_argument('--inject_sensory', action='store_true', help='Inject sensory input')
    train_parser.add_argument('--output', type=str, help='Output file path')
    
    # Config command
    config_parser = subparsers.add_parser('config', help='Configuration utilities')
    config_parser.add_argument('--preset', choices=['quick', 'research', 'production', 'debug', 'minimal'],
                             help='Show preset configuration')
    config_parser.add_argument('--validate', type=str, help='Validate configuration file')
    config_parser.add_argument('--export', type=str, help='Export configuration to file')
    config_parser.add_argument('--import', type=str, dest='import_file', help='Import configuration from file')
    
    # Benchmark command
    bench_parser = subparsers.add_parser('benchmark', help='Performance benchmarking')
    bench_parser.add_argument('--preset', choices=['quick', 'research', 'production', 'debug', 'minimal'],
                            default='quick', help='Configuration preset')
    bench_parser.add_argument('--neurons', type=int, default=100, help='Number of neurons')
    bench_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    bench_parser.add_argument('--log-interval', type=int, default=100, help='Logging interval')
    bench_parser.add_argument('--output', type=str, help='Output file path')
    
    # Profile command
    profile_parser = subparsers.add_parser('profile', help='Brain profiling')
    profile_parser.add_argument('--brain', type=str, help='Brain file to profile')
    profile_parser.add_argument('--steps', type=int, default=100, help='Number of steps to profile')
    profile_parser.add_argument('--log-interval', type=int, default=100, help='Logging interval')
    profile_parser.add_argument('--output', type=str, help='Output file path')
    profile_parser.add_argument('--detailed', action='store_true', help='Enable detailed profiling')
    
    # Utils command
    utils_parser = subparsers.add_parser('utils', help='Utility operations')
    utils_parser.add_argument('--validate', choices=['brain', 'config', 'all'], help='Validate files or configurations')
    utils_parser.add_argument('--file', type=str, help='File to validate')
    
    args = parser.parse_args()
    
    # Dispatch to appropriate function
    if args.command == 'train':
        return train_brain(args)
    elif args.command == 'config':
        return config_commands(args)
    elif args.command == 'benchmark':
        return benchmark_command(args)
    elif args.command == 'profile':
        return profile_command(args)
    elif args.command == 'utils':
        return utils_command(args)
    else:
        parser.print_help()
        return False

if __name__ == "__main__":
    success = main()
    sys.exit(0 if success else 1)