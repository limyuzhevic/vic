#!/usr/bin/env python3
"""
Command-line interface for NLM - Neural Learning Machine

This module provides command-line tools for:
- Running simulations
- Managing brain configurations
- Analyzing brain states
- Running experiments
- Performance optimization
"""

import argparse
import json
import os
import sys
from pathlib import Path
from typing import Optional, List, Dict, Any

# Add the current directory to path so we can import NLM modules
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from src.core.Config.Config import Config
from src.brain.Brain import Brain
from src.agent.AgentBrain import AgentBrain
from src.world.SimpleWorld import SimpleWorld
import pynlm


def create_brain_command(args):
    """Create and initialize a brain"""
    print("Creating NLM brain...")
    
    # Load configuration
    config = pynlm.createDefaultConfig()
    
    # Set custom parameters if provided
    if args.neuron_count:
        config.set("neuron_count", args.neuron_count)
    if args.region_count:
        config.set("region_count", args.region_count)
    if args.connection_prob:
        config.set("connection_probability", args.connection_prob)
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    if not brain.initialize():
        print("ERROR: Failed to initialize brain!")
        return 1
    
    print(f"✓ Brain created successfully:")
    print(f"  - Neurons: {brain.getTotalNeuronCount()}")
    print(f"  - Regions: {brain.getRegionCount()}")
    print(f"  - Synapses: {brain.getTotalSynapseCount()}")
    
    if args.save:
        if brain.save(args.save):
            print(f"✓ Brain state saved to: {args.save}")
        else:
            print(f"ERROR: Failed to save brain to {args.save}")
            return 1
    
    return 0


def run_simulation_command(args):
    """Run a simulation with the brain"""
    print(f"Running simulation with {args.steps} steps...")
    
    # Load brain if provided
    brain = None
    if args.load:
        # Try to load from file
        try:
            brain = load_brain(args.load)
            if not brain:
                print(f"ERROR: Failed to load brain from {args.load}")
                return 1
        except Exception as e:
            print(f"ERROR loading brain: {e}")
            return 1
    else:
        # Create new brain
        config = pynlm.createDefaultConfig()
        if args.neuron_count:
            config.set("neuron_count", args.neuron_count)
        if args.region_count:
            config.set("region_count", args.region_count)
        
        brain = pynlm.createBrain(config)
        if not brain.initialize():
            print("ERROR: Failed to initialize brain!")
            return 1
    
    # Create world if needed
    world = None
    if args.with_world:
        world = pynlm.createSimpleWorld()
        if args.width and args.height:
            world.configure(args.width, args.height, args.vision_width, args.vision_height)
        world.reset()
    
    # Create agent if needed
    agent = None
    if args.with_agent and world:
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable features if requested
        if args.reward_modulation:
            agent.enableRewardModulation(True)
        if args.structural_plasticity:
            agent.enableStructuralPlasticity(True)
        if args.development:
            agent.enableDevelopment(True)
        if args.curiosity:
            agent.enableCuriosity(True)
    
    # Run simulation
    for step in range(args.steps):
        if world:
            world.update(0.1)  # 100ms timestep
            
            if agent:
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
        
        brain.step(step)
        
        if agent:
            action = agent.decodeMotorCommand()
            if world:
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Log progress every 10% or every N steps
        if step % max(1, args.steps // 10) == 0 or step == args.steps - 1:
            print(f"  Step {step}/{args.steps}: "
                  f"Spikes={brain.getTotalSpikeCount()}, "
                  f"Firing={brain.getFiringNeuronCount()}")
    
    print(f"✓ Simulation completed!")
    print(f"  - Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  - Average firing rate: {brain.getAverageFiringRate():.2f}")
    
    if args.save:
        if brain.save(args.save):
            print(f"✓ Brain state saved to: {args.save}")
        else:
            print(f"ERROR: Failed to save brain to {args.save}")
            return 1
    
    return 0


def analyze_brain_command(args):
    """Analyze and report on brain state"""
    print("Analyzing brain state...")
    
    # Load brain
    brain = load_brain(args.file)
    if not brain:
        return 1
    
    # Run analysis
    brain.logStatus()
    
    # Generate report
    if args.output:
        report = generate_brain_report(brain)
        with open(args.output, 'w') as f:
            f.write(report)
        print(f"✓ Analysis report saved to: {args.output}")
    
    return 0


def load_brain(filepath: str) -> Optional[pynlm.Brain]:
    """Load a brain from file"""
    try:
        # Try to use the brain's load method if available
        if hasattr(pynlm.Brain, 'load'):
            # This would require a config first
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            if brain.load(filepath):
                return brain
        else:
            print(f"WARNING: Brain.load method not available")
    except Exception as e:
        print(f"ERROR loading brain: {e}")
    
    return None


def generate_brain_report(brain) -> str:
    """Generate a text report of brain state"""
    report = "NLM Brain Analysis Report\n"
    report += "=" * 50 + "\n\n"
    
    report += f"Total Neurons: {brain.getTotalNeuronCount()}\n"
    report += f"Total Synapses: {brain.getTotalSynapseCount()}\n"
    report += f"Total Spikes: {brain.getTotalSpikeCount()}\n"
    report += f"Active Neurons: {brain.getActiveNeuronCount()}\n"
    report += f"Firing Neurons: {brain.getFiringNeuronCount()}\n"
    report += f"Average Firing Rate: {brain.getAverageFiringRate():.2f}\n"
    report += f"E/I Ratio: {brain.getExcitationInhibitionRatio():.3f}\n"
    
    # Region analysis
    for i, region_id in enumerate(brain.getRegionIds()):
        region = brain.getRegion(region_id)
        if region:
            report += f"\nRegion {i+1} ({region.getName()}):\n"
            report += f"  - Neurons: {region.getTotalNeuronCount()}\n"
            report += f"  - Synapses: {region.getSynapseCount()}\n"
            report += f"  - Avg Weight: {region.getAverageSynapticWeight():.3f}\n"
            report += f"  - Avg Firing Rate: {region.getAverageFiringRate():.2f}\n"
    
    return report


def run_experiment_command(args):
    """Run an experiment"""
    print(f"Running experiment: {args.experiment_name}")
    
    # This would integrate with the experiment framework
    # For now, just show a message
    print("✓ Experiment framework available in C++")
    print(f"  - To run experiments, use: ./nlm_phase6_demo")
    
    return 0


def config_commands_parser(subparsers):
    """Add configuration-related commands"""
    # Create brain command
    create_parser = subparsers.add_parser('create', help='Create and initialize a brain')
    create_parser.add_argument('--neuron-count', type=int, default=1000,
                              help='Number of neurons (default: 1000)')
    create_parser.add_argument('--region-count', type=int, default=2,
                              help='Number of regions (default: 2)')
    create_parser.add_argument('--connection-prob', type=float, default=0.1,
                              help='Connection probability (default: 0.1)')
    create_parser.add_argument('-s', '--save', help='Save brain state to file')
    create_parser.set_defaults(func=create_brain_command)
    
    # Run simulation command
    sim_parser = subparsers.add_parser('simulate', help='Run a simulation')
    sim_parser.add_argument('steps', type=int, help='Number of simulation steps')
    sim_parser.add_argument('--load', help='Load brain state from file')
    sim_parser.add_argument('--neuron-count', type=int, help='Number of neurons')
    sim_parser.add_argument('--region-count', type=int, help='Number of regions')
    sim_parser.add_argument('--with-world', action='store_true', help='Run with world simulation')
    sim_parser.add_argument('--with-agent', action='store_true', help='Run with agent brain')
    sim_parser.add_argument('--reward-modulation', action='store_true', help='Enable reward modulation')
    sim_parser.add_argument('--structural-plasticity', action='store_true', help='Enable structural plasticity')
    sim_parser.add_argument('--development', action='store_true', help='Enable development')
    sim_parser.add_argument('--curiosity', action='store_true', help='Enable curiosity')
    sim_parser.add_argument('--width', type=int, default=10, help='World width (default: 10)')
    sim_parser.add_argument('--height', type=int, default=10, help='World height (default: 10)')
    sim_parser.add_argument('--vision-width', type=int, default=8, help='Vision width (default: 8)')
    sim_parser.add_argument('--vision-height', type=int, default=8, help='Vision height (default: 8)')
    sim_parser.add_argument('-s', '--save', help='Save brain state to file')
    sim_parser.set_defaults(func=run_simulation_command)
    
    # Analyze brain command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze brain state')
    analyze_parser.add_argument('file', help='Brain state file to analyze')
    analyze_parser.add_argument('-o', '--output', help='Output report file')
    analyze_parser.set_defaults(func=analyze_brain_command)
    
    # Run experiment command
    exp_parser = subparsers.add_parser('experiment', help='Run an experiment')
    exp_parser.add_argument('experiment_name', help='Name of experiment to run')
    exp_parser.set_defaults(func=run_experiment_command)


def main():
    """Main entry point"""
    parser = argparse.ArgumentParser(
        description='NLM - Neural Learning Machine Command Line Interface',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm create --neuron-count 2000 --save brain.bin
  nlm simulate 1000 --with-world --with-agent --curiosity
  nlm analyze brain.bin -o analysis.txt
  nlm experiment learning_curves

For more detailed help, use: nlm [command] --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    config_commands_parser(subparsers)
    
    # Version command
    version_parser = subparsers.add_parser('version', help='Show version information')
    version_parser.set_defaults(func=lambda args: print("NLM Phase 6 - Final Integration") or 0)
    
    # Help command
    help_parser = subparsers.add_parser('help', help='Show help information')
    help_parser.add_argument('topic', nargs='?', help='Topic to get help for')
    help_parser.set_defaults(func=lambda args: print("Use 'nlm create --help', 'nlm simulate --help', etc.") or 0)
    
    # Parse arguments
    args = parser.parse_args()
    
    # Execute command
    if hasattr(args, 'func'):
        return args.func(args)
    else:
        parser.print_help()
        return 1


if __name__ == '__main__':
    exit(main())
