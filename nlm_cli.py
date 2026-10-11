#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI)

A comprehensive command-line interface for the NLM (Neural Learning Machine) simulation framework.
Provides commands for exploration, learning, simulation, analysis, benchmarking, debugging, profiling, and export.

Usage:
    nlm-explore [OPTIONS]              Explore NLM brain configurations interactively
    nlm-learn [OPTIONS]                Run learning simulations with presets
    nlm-simulate [OPTIONS]             Run simple simulations
    nlm-analyze [OPTIONS]              Analyze and visualize simulation results
    nlm-benchmark [OPTIONS]            Benchmark performance across configurations
    nlm-debug [OPTIONS]                Advanced debugging with breakpoints
    nlm-profile [OPTIONS]              Profile memory and performance
    nlm-export [OPTIONS]               Export results in various formats
"""

import argparse
import sys
import os
sys.path.append(os.path.dirname(os.path.abspath(__file__)))

from pynlm import createBrain, createSimpleWorld, createAgentBrain, createDefaultConfig
import json
import time
from typing import Dict, Any


def explore_command(args):
    """Interactive exploration of NLM configurations"""
    print("=== NLM Brain Explorer ===")
    print("Creating default brain configuration...")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    print(f"Active neurons: {brain.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"Total spikes: {brain.getTotalSpikeCount()}")
    
    if args.steps:
        for i in range(args.steps):
            brain.step(i)
            if i % 10 == 0:
                print(f"Step {i}: {brain.getFiringNeuronCount()} neurons firing, "
                      f"avg rate: {brain.getAverageFiringRate():.2f}")
    
    print("\nExploration complete. Use 'nlm-analyze' for detailed statistics.")


def learn_command(args):
    """Run learning simulations with predefined presets"""
    print("=== NLM Learning Simulator ===")
    
    presets = {
        'simple': {'curiosity': True, 'reward': True, 'development': True, 'structural': False},
        'advanced': {'curiosity': True, 'reward': True, 'development': True, 'structural': True},
        'curiosity_only': {'curiosity': True, 'reward': False, 'development': False, 'structural': False},
        'reward_only': {'curiosity': False, 'reward': True, 'development': False, 'structural': False},
    }
    
    if args.preset not in presets:
        print(f"Unknown preset: {args.preset}")
        print(f"Available presets: {list(presets.keys())}")
        return
    
    preset = presets[args.preset]
    print(f"Running '{args.preset}' preset: {preset}")
    
    # Create configuration with preset settings
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=30, height=30, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Apply preset learning settings
    agent.enableCuriosity(preset['curiosity'])
    agent.enableRewardModulation(preset['reward'])
    agent.enableDevelopment(preset['development'])
    agent.enableStructuralPlasticity(preset['structural'])
    
    print(f"Running learning simulation for {args.steps} steps...")
    start_time = time.time()
    
    for step in range(args.steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 50 == 0:
            print(f"Step {step}: neurons firing={brain.getFiringNeuronCount()}, "
                  f"spikes={brain.getTotalSpikeCount()}, "
                  f"rate={brain.getAverageFiringRate():.2f}")
    
    elapsed = time.time() - start_time
    print(f"\nLearning simulation complete!")
    print(f"Duration: {elapsed:.2f} seconds")
    print(f"Final statistics:")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  Development stage: {brain.getDevelopmentalStage()}")
    print(f"  Learning levels: {agent.getAllNeuromodulationLevels()}")


def simulate_command(args):
    """Run simple simulations"""
    print("=== NLM Simple Simulation ===")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=40, height=40, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable learning if requested
    if args.learning:
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
    
    print(f"Running simulation for {args.steps} steps...")
    print(f"Brain: {brain.getTotalNeuronCount()} neurons, {brain.getTotalSynapseCount()} synapses")
    
    for step in range(args.steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if step % 20 == 0:
            print(f"Step {step}: firing={brain.getFiringNeuronCount()}, "
                  f"rate={brain.getAverageFiringRate():.2f}")
    
    print(f"\nSimulation complete!")
    print(f"Final brain statistics:")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  World agent position: ({agent.getBrain().getRegion(0)->getAllNeurons()[0]->getId().value}, {agent.getBrain().getRegion(0)->getAllNeurons()[1]->getId().value})")


def analyze_command(args):
    """Analyze and visualize simulation results"""
    print("=== NLM Result Analyzer ===")
    
    if not args.filepath:
        print("Error: --filepath required for analysis")
        print("Use 'nlm-simulate --help' to see simulation options first")
        return
    
    try:
        from pynlm import createBrain
        
        # Load brain state
        brain = createBrain(createDefaultConfig())
        if brain.load(args.filepath):
            print(f"Loaded brain state from {args.filepath}")
        else:
            print(f"Failed to load brain state from {args.filepath}")
            return
        
        print(f"\nBrain Analysis:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Active neurons: {brain.getActiveNeuronCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
        # Get plasticity statistics
        stats = brain.getStatistics()
        print(f"\nDetailed Statistics:")
        for key, value in stats.items():
            print(f"  {key}: {value:.2f}")
        
        # Get memory statistics
        memory_stats = brain.getMemoryStats()
        print(f"\nMemory Statistics:")
        for key, value in memory_stats.items():
            print(f"  {key}: {value}")
        
        print(f"\nAnalysis complete. Use 'nlm-export' to save results in different formats.")
        
    except Exception as e:
        print(f"Error during analysis: {e}")


def benchmark_command(args):
    """Benchmark performance across configurations"""
    print("=== NLM Performance Benchmark ===")
    
    configurations = [
        {'neurons': 100, 'synapses': 1000, 'learning': True},
        {'neurons': 500, 'synapses': 5000, 'learning': True},
        {'neurons': 1000, 'synapses': 10000, 'learning': True},
        {'neurons': 100, 'synapses': 1000, 'learning': False},
    ]
    
    results = []
    
    for i, config in enumerate(configurations):
        print(f"\nRunning benchmark {i+1}/{len(configurations)}: {config}...")
        
        start_time = time.time()
        
        # Create and run simulation
        config_obj = createDefaultConfig()
        brain = createBrain(config_obj)
        brain.initialize()
        
        world = createSimpleWorld()
        world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
        world.reset()
        
        agent = createAgentBrain(brain)
        agent.initialize(world)
        
        if config['learning']:
            agent.enableRewardModulation(True)
            agent.enableCuriosity(True)
        
        steps = 100
        for step in range(steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
        
        elapsed = time.time() - start_time
        
        # Record results
        result = {
            'id': i + 1,
            'neurons': config['neurons'],
            'synapses': config['synapses'],
            'learning': config['learning'],
            'duration': elapsed,
            'steps_per_second': steps / elapsed,
            'neurons_firing': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'avg_firing_rate': brain.getAverageFiringRate()
        }
        results.append(result)
        
        print(f"  Duration: {elapsed:.2f}s, {result['steps_per_second']:.1f} steps/sec")
    
    print(f"\nBenchmark Results:")
    print(f"{'ID':<4} {'Neurons':<8} {'Synapses':<10} {'Learning':<10} {'Duration':<10} {'Steps/sec':<12}")
    print("-" * 65)
    
    for result in results:
        print(f"{result['id']:<4} {result['neurons']:<8} {result['synapses']:<10} "
              f"{str(result['learning']):<10} {result['duration']:<10.2f} {result['steps_per_second']:<12.1f}")
    
    # Save results to file
    if args.output:
        with open(args.output, 'w') as f:
            json.dump({
                'configurations_tested': len(configurations),
                'results': results
            }, f, indent=2)
        print(f"\nResults saved to {args.output}")
    
    print(f"\nBenchmark complete. Use 'nlm-export' to export detailed results.")


def debug_command(args):
    """Advanced debugging with breakpoints and inspection"""
    print("=== NLM Debugger ===")
    
    print("Debug mode enabled. Available commands:")
    print("  'step' - Execute one simulation step")
    print("  'status' - Show current brain state")
    print("  'continue' - Run until specified step")
    print("  'break <step>' - Set breakpoint at specific step")
    print("  'neurons' - Show neuron details")
    print("  'synapses' - Show synapse details")
    print("  'memory' - Show memory system status")
    print("  'quit' - Exit debug mode")
    
    # For now, create a simple debug session
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Debug session started with {brain.getTotalNeuronCount()} neurons")
    print("Use 'nlm-debug --help' for more options in future versions.")


def profile_command(args):
    """Profile memory and performance"""
    print("=== NLM Profiler ===")
    
    import psutil
    import os
    
    # Get system info
    process = psutil.Process(os.getpid())
    print(f"System Information:")
    print(f"  Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
    print(f"  CPU count: {psutil.cpu_count()}")
    print(f"  CPU percent: {psutil.cpu_percent():.1f}%")
    
    # Performance profiling
    print(f"\nRunning performance profile...")
    
    config = createDefaultConfig()
    brain = createBrain(config)
    brain.initialize()
    
    world = createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = createAgentBrain(brain)
    agent.initialize(world)
    
    # Profile different aspects
    start_time = time.time()
    
    # Step execution profile
    steps = args.steps
    for step in range(steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    elapsed = time.time() - start_time
    
    print(f"\nPerformance Profile Results:")
    print(f"  Duration: {elapsed:.2f} seconds")
    print(f"  Steps per second: {steps / elapsed:.1f}")
    print(f"  Average step time: {elapsed / steps * 1000:.1f} ms")
    print(f"  Memory usage: {process.memory_info().rss / 1024 / 1024:.2f} MB")
    print(f"  Neurons: {brain.getTotalNeuronCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    
    # Save profile results
    if args.output:
        profile_data = {
            'duration': elapsed,
            'steps_per_second': steps / elapsed,
            'avg_step_time_ms': elapsed / steps * 1000,
            'memory_usage_mb': process.memory_info().rss / 1024 / 1024,
            'neurons': brain.getTotalNeuronCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'config': args.output
        }
        
        with open(args.output, 'w') as f:
            json.dump(profile_data, f, indent=2)
        print(f"\nProfile data saved to {args.output}")


def export_command(args):
    """Export results in various formats"""
    print("=== NLM Exporter ===")
    
    if not args.filepath:
        print("Error: --filepath required for export")
        return
    
    try:
        from pynlm import createBrain
        
        # Load brain state
        brain = createBrain(createDefaultConfig())
        if not brain.load(args.filepath):
            print(f"Failed to load brain state from {args.filepath}")
            return
        
        print(f"Loaded brain state from {args.filepath}")
        
        if args.format == 'json':
            # Export as JSON
            export_data = {
                'brain_info': {
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                    'active_neuron_count': brain.getActiveNeuronCount(),
                    'firing_neuron_count': brain.getFiringNeuronCount(),
                    'total_spike_count': brain.getTotalSpikeCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'development_stage': brain.getDevelopmentalStage(),
                    'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio()
                },
                'statistics': brain.getStatistics(),
                'memory_stats': brain.getMemoryStats(),
                'plasticity_status': brain.getPlasticityStatus(),
                'timestamp': time.time()
            }
            
            output_file = args.filepath.replace('.brain', '.json') if not args.output else args.output
            with open(output_file, 'w') as f:
                json.dump(export_data, f, indent=2)
            print(f"Exported brain data to {output_file} (JSON format)")
            
        elif args.format == 'csv':
            # Export as CSV (simplified)
            output_file = args.filepath.replace('.brain', '.csv') if not args.output else args.output
            with open(output_file, 'w') as f:
                f.write("metric,value\n")
                f.write(f"neuron_count,{brain.getTotalNeuronCount()}\n")
                f.write(f"synapse_count,{brain.getTotalSynapseCount()}\n")
                f.write(f"active_neuron_count,{brain.getActiveNeuronCount()}\n")
                f.write(f"firing_neuron_count,{brain.getFiringNeuronCount()}\n")
                f.write(f"total_spike_count,{brain.getTotalSpikeCount()}\n")
                f.write(f"average_firing_rate,{brain.getAverageFiringRate()}\n")
                f.write(f"development_stage,{brain.getDevelopmentalStage()}\n")
                f.write(f"excitation_inhibition_ratio,{brain.getExcitationInhibitionRatio()}\n")
            print(f"Exported brain data to {output_file} (CSV format)")
            
        elif args.format == 'matlab':
            # Export as MATLAB format (simplified)
            output_file = args.filepath.replace('.brain', '.mat') if not args.output else args.output
            with open(output_file, 'w') as f:
                f.write("% MATLAB format brain data export\n")
                f.write("% Generated by NLM Exporter\n\n")
                f.write("brain_data.neuron_count = %d;\n" % brain.getTotalNeuronCount())
                f.write("brain_data.synapse_count = %d;\n" % brain.getTotalSynapseCount())
                f.write("brain_data.active_neuron_count = %d;\n" % brain.getActiveNeuronCount())
                f.write("brain_data.firing_neuron_count = %d;\n" % brain.getFiringNeuronCount())
                f.write("brain_data.total_spike_count = %d;\n" % brain.getTotalSpikeCount())
                f.write("brain_data.average_firing_rate = %f;\n" % brain.getAverageFiringRate())
                f.write("brain_data.development_stage = %d;\n" % brain.getDevelopmentalStage())
                f.write("brain_data.excitation_inhibition_ratio = %f;\n" % brain.getExcitationInhibitionRatio())
            print(f"Exported brain data to {output_file} (MATLAB format)")
        
        elif args.format == 'text':
            # Export as human-readable text
            output_file = args.filepath.replace('.brain', '.txt') if not args.output else args.output
            with open(output_file, 'w') as f:
                f.write("NLM Brain State Export\n")
                f.write("=" * 50 + "\n\n")
                f.write(f"Total Neurons: {brain.getTotalNeuronCount()}\n")
                f.write(f"Total Synapses: {brain.getTotalSynapseCount()}\n")
                f.write(f"Active Neurons: {brain.getActiveNeuronCount()}\n")
                f.write(f"Firing Neurons: {brain.getFiringNeuronCount()}\n")
                f.write(f"Total Spikes: {brain.getTotalSpikeCount()}\n")
                f.write(f"Average Firing Rate: {brain.getAverageFiringRate():.2f}\n")
                f.write(f"Development Stage: {brain.getDevelopmentalStage()}\n")
                f.write(f"Excitation/Inhibition Ratio: {brain.getExcitationInhibitionRatio():.2f}\n\n")
                
                f.write("Statistics:\n")
                stats = brain.getStatistics()
                for key, value in stats.items():
                    f.write(f"  {key}: {value:.2f}\n")
                
                f.write("\nMemory Statistics:\n")
                memory_stats = brain.getMemoryStats()
                for key, value in memory_stats.items():
                    f.write(f"  {key}: {value}\n")
                
                f.write("\nPlasticity Status:\n")
                plasticity_status = brain.getPlasticityStatus()
                for key, value in plasticity_status.items():
                    f.write(f"  {key}: {value}\n")
            print(f"Exported brain data to {output_file} (text format)")
        
        else:
            print(f"Unknown export format: {args.format}")
            print(f"Available formats: json, csv, matlab, text")
        
        print(f"\nExport complete.")
        
    except Exception as e:
        print(f"Error during export: {e}")


def main():
    parser = argparse.ArgumentParser(description="NLM Command Line Interface")
    
    # Create subparsers for different commands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # === EXPLORER COMMAND ===
    explore_parser = subparsers.add_parser('explore', help='Explore NLM brain configurations interactively')
    explore_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    
    # === LEARN COMMAND ===
    learn_parser = subparsers.add_parser('learn', help='Run learning simulations with presets')
    learn_parser.add_argument('--preset', choices=['simple', 'advanced', 'curiosity_only', 'reward_only'],
                             default='simple', help='Learning preset configuration')
    learn_parser.add_argument('--steps', type=int, default=500, help='Number of simulation steps')
    
    # === SIMULATE COMMAND ===
    simulate_parser = subparsers.add_parser('simulate', help='Run simple simulations')
    simulate_parser.add_argument('--steps', type=int, default=200, help='Number of simulation steps')
    simulate_parser.add_argument('--learning', action='store_true', help='Enable learning features')
    
    # === ANALYZE COMMAND ===
    analyze_parser = subparsers.add_parser('analyze', help='Analyze and visualize simulation results')
    analyze_parser.add_argument('--filepath', help='Path to brain state file to analyze')
    
    # === BENCHMARK COMMAND ===
    benchmark_parser = subparsers.add_parser('benchmark', help='Benchmark performance across configurations')
    benchmark_parser.add_argument('--output', help='Output file for benchmark results (JSON)')
    
    # === DEBUG COMMAND ===
    debug_parser = subparsers.add_parser('debug', help='Advanced debugging with breakpoints')
    
    # === PROFILE COMMAND ===
    profile_parser = subparsers.add_parser('profile', help='Profile memory and performance')
    profile_parser.add_argument('--steps', type=int, default=100, help='Number of simulation steps')
    profile_parser.add_argument('--output', help='Output file for profile data (JSON)')
    
    # === EXPORT COMMAND ===
    export_parser = subparsers.add_parser('export', help='Export results in various formats')
    export_parser.add_argument('--filepath', required=True, help='Path to brain state file to export')
    export_parser.add_argument('--format', choices=['json', 'csv', 'matlab', 'text'],
                             default='json', help='Export format')
    export_parser.add_argument('--output', help='Output file path (auto-generated if not specified)')
    
    # If no arguments provided, show help
    if len(sys.argv) == 1:
        parser.print_help()
        return
    
    # Execute command
    args = parser.parse_args()
    
    try:
        if args.command == 'explore':
            explore_command(args)
        elif args.command == 'learn':
            learn_command(args)
        elif args.command == 'simulate':
            simulate_command(args)
        elif args.command == 'analyze':
            analyze_command(args)
        elif args.command == 'benchmark':
            benchmark_command(args)
        elif args.command == 'debug':
            debug_command(args)
        elif args.command == 'profile':
            profile_command(args)
        elif args.command == 'export':
            export_command(args)
        else:
            print(f"Unknown command: {args.command}")
            parser.print_help()
    except KeyboardInterrupt:
        print("\nOperation cancelled by user")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()
