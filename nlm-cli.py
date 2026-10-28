#!/usr/bin/env python3
"""
NLM Command Line Interface Tool

Advanced command-line tool for controlling NLM simulations,
brain management, and configuration.

Usage:
    nlm-cli [command] [options]

Commands:
    run          Run a simulation
    config       Configuration management
    brain        Brain state management
    stats        Get brain statistics
    test         Run predefined test scenarios
    help         Show help
"""

import sys
import argparse
import json
import os
from typing import Optional, Dict, Any

# Add the current directory to the path so we can import pynlm
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm


def create_parser():
    """Create the main argument parser"""
    parser = argparse.ArgumentParser(
        description="NLM Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    # Add subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    run_parser = subparsers.add_parser('run', help='Run a simulation')
    run_parser.add_argument('--config', default='configs/default.cfg', 
                           help='Configuration file path')
    run_parser.add_argument('--steps', type=int, default=1000,
                           help='Number of simulation steps')
    run_parser.add_argument('--time', type=float, default=1.0,
                           help='Total simulation time in seconds')
    run_parser.add_argument('--world-width', type=int, default=20,
                           help='Width of the simple world')
    run_parser.add_argument('--world-height', type=int, default=20,
                           help='Height of the simple world')
    run_parser.add_argument('--output', help='Output file for brain state')
    run_parser.add_argument('--no-world', action='store_true',
                           help='Run brain-only simulation')
    
    # Configuration command
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_subparsers = config_parser.add_subparsers(dest='config_action', 
                                                     help='Configuration actions')
    
    # List configuration
    list_parser = config_subparsers.add_parser('list', help='List all configuration keys')
    list_parser.add_argument('--config', default='configs/default.cfg',
                            help='Configuration file')
    
    # Get configuration value
    get_parser = config_subparsers.add_parser('get', help='Get configuration value')
    get_parser.add_argument('--config', default='configs/default.cfg',
                           help='Configuration file')
    get_parser.add_argument('key', help='Configuration key')
    
    # Set configuration value
    set_parser = config_subparsers.add_parser('set', help='Set configuration value')
    set_parser.add_argument('--config', default='configs/default.cfg',
                           help='Configuration file')
    set_parser.add_argument('key', help='Configuration key')
    set_parser.add_argument('value', help='Configuration value')
    
    # Brain management command
    brain_parser = subparsers.add_parser('brain', help='Brain state management')
    brain_subparsers = brain_parser.add_subparsers(dest='brain_action',
                                                   help='Brain actions')
    
    # Save brain state
    save_parser = brain_subparsers.add_parser('save', help='Save brain state to file')
    save_parser.add_argument('filepath', help='File path to save brain state')
    save_parser.add_argument('--config', default='configs/default.cfg',
                           help='Configuration file')
    
    # Load brain state
    load_parser = brain_subparsers.add_parser('load', help='Load brain state from file')
    load_parser.add_argument('filepath', help='File path to load brain state from')
    load_parser.add_argument('--config', default='configs/default.cfg',
                           help='Configuration file')
    
    # Stats command
    stats_parser = subparsers.add_parser('stats', help='Get brain statistics')
    stats_parser.add_argument('--config', default='configs/default.cfg',
                             help='Configuration file')
    stats_parser.add_argument('--format', choices=['json', 'text', 'yaml'],
                             default='text', help='Output format')
    
    # Test command
    test_parser = subparsers.add_parser('test', help='Run predefined test scenarios')
    test_parser.add_argument('scenario', choices=['basic', 'plasticity', 'stdp', 'curiosity', 'integration'],
                            help='Test scenario to run')
    test_parser.add_argument('--steps', type=int, default=500,
                            help='Number of simulation steps')
    test_parser.add_argument('--output', help='Output file for results')
    
    return parser


def run_simulation(args):
    """Run a simulation with the given arguments"""
    print("Initializing NLM simulation...")
    
    # Create configuration
    config = pynlm.createAdvancedConfig()
    
    # Load additional configuration from file if specified
    if os.path.exists(args.config):
        try:
            config.loadFromFile(args.config)
            print(f"Loaded configuration from {args.config}")
        except Exception as e:
            print(f"Warning: Could not load configuration from {args.config}: {e}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    print(f"Brain initialized with {brain.getTotalNeuronCount()} neurons")
    
    # Create world if requested
    if not args.no_world:
        print("Setting up world environment...")
        world = pynlm.createSimpleWorld()
        world.configure(
            width=args.world_width,
            height=args.world_height,
            visionWidth=8,
            visionHeight=8
        )
        world.reset()
        world.setAgentStart(args.world_width // 2, args.world_height // 2)
        
        # Create agent
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable learning subsystems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        print("World and agent initialized")
        
        # Run simulation
        timestep = args.time / args.steps
        for step in range(args.steps):
            # Update world
            world.update(timestep)
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get action
            action = agent.decodeMotorCommand()
            
            # Apply action
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward modulation
            reward = world.getSensoryPercept().getInternal()[0] if world.getSensoryPercept().getInternal() else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            # Update development
            agent.updateDevelopment(timestep)
            
            # Print progress
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    else:
        # Brain-only simulation
        print("Running brain-only simulation...")
        for step in range(args.steps):
            brain.step(step)
            
            if step % 100 == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                      f"Total spikes: {brain.getTotalSpikeCount()}")
    
    print("Simulation completed!")
    
    # Save brain state if requested
    if args.output:
        print(f"Saving brain state to {args.output}")
        brain.save(args.output)
    
    # Print final statistics
    print("\nFinal Statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
    print(f"  Developmental stage: {brain.getDevelopmentalStage()}")


def config_command(args):
    """Handle configuration management commands"""
    if args.config_action == 'list':
        config = pynlm.createDefaultConfig()
        if os.path.exists(args.config):
            config.loadFromFile(args.config)
        
        print("Configuration keys:")
        for key in config.getKeys():
            print(f"  {key}")
    
    elif args.config_action == 'get':
        config = pynlm.createDefaultConfig()
        if os.path.exists(args.config):
            config.loadFromFile(args.config)
        
        value = config.get(args.key)
        if value is not None:
            print(f"{args.key} = {value}")
        else:
            print(f"Key '{args.key}' not found")
    
    elif args.config_action == 'set':
        config = pynlm.createDefaultConfig()
        
        # Try to determine value type
        try:
            # Try integer
            int_val = int(args.value)
            config.set(args.key, int_val)
        except ValueError:
            try:
                # Try float
                float_val = float(args.value)
                config.set(args.key, float_val)
            except ValueError:
                try:
                    # Try boolean
                    if args.value.lower() == 'true':
                        config.set(args.key, True)
                    elif args.value.lower() == 'false':
                        config.set(args.key, False)
                    else:
                        config.set(args.key, args.value)
                except ValueError:
                    config.set(args.key, args.value)
        
        # Save configuration
        if os.path.exists(args.config):
            config.saveToFile(args.config)
        
        print(f"Set {args.key} = {args.value}")


def brain_command(args):
    """Handle brain management commands"""
    if args.brain_action == 'save':
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Saving brain state to {args.filepath}")
        brain.save(args.filepath)
        
    elif args.brain_action == 'load':
        # Create brain
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        print(f"Loading brain state from {args.filepath}")
        brain.load(args.filepath)


def stats_command(args):
    """Get brain statistics"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if args.format == 'json':
        stats = {
            'neuron_count': brain.getTotalNeuronCount(),
            'synapse_count': brain.getTotalSynapseCount(),
            'firing_neuron_count': brain.getFiringNeuronCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'excitation_inhibition_ratio': brain.getExcitationInhibitionRatio(),
            'total_spike_count': brain.getTotalSpikeCount(),
            'developmental_stage': str(brain.getDevelopmentalStage())
        }
        print(json.dumps(stats, indent=2))
    
    elif args.format == 'yaml':
        print("## Brain Statistics")
        print(f"neuron_count: {brain.getTotalNeuronCount()}")
        print(f"synapse_count: {brain.getTotalSynapseCount()}")
        print(f"firing_neuron_count: {brain.getFiringNeuronCount()}")
        print(f"average_firing_rate: {brain.getAverageFiringRate():.2f}")
        print(f"excitation_inhibition_ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"total_spike_count: {brain.getTotalSpikeCount()}")
        print(f"developmental_stage: {brain.getDevelopmentalStage()}")
    
    else:  # text
        print("Brain Statistics:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.2f}")
        print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.3f}")
        print(f"  Total spikes: {brain.getTotalSpikeCount()}")
        print(f"  Developmental stage: {brain.getDevelopmentalStage()}")


def test_command(args):
    """Run predefined test scenarios"""
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    print(f"Running test scenario: {args.scenario}")
    
    if args.scenario == 'basic':
        # Basic connectivity test
        for step in range(args.steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 100 == 0:
                print(f"Step {step}: Firing neurons: {brain.getFiringNeuronCount()}")
    
    elif args.scenario == 'plasticity':
        # Plasticity test with learning
        agent.enableRewardModulation(True)
        
        for step in range(args.steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward
            reward = 0.1 if step % 50 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            if step % 100 == 0:
                print(f"Step {step}: Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}")
    
    elif args.scenario == 'stdp':
        # STDP verification test
        agent.enableRewardModulation(True)
        
        for step in range(args.steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Simple reward pattern for STDP
            reward = 1.0 if step % 100 == 0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            if step % 200 == 0:
                print(f"Step {step}: Total spikes: {brain.getTotalSpikeCount()}")
    
    elif args.scenario == 'curiosity':
        # Curiosity-driven exploration test
        agent.enableCuriosity(True)
        
        for step in range(args.steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if step % 100 == 0:
                print(f"Step {step}: Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"Novelty: {agent.getNoveltyLevel():.3f}, Action: {action}")
    
    elif args.scenario == 'integration':
        # Full integration test with all systems
        agent.enableRewardModulation(True)
        agent.enableCuriosity(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        
        for step in range(args.steps):
            world.update(0.1)
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Apply reward based on action success
            reward = 0.1 if action != pynlm.MotorCommand.Wait else 0.0
            agent.applyRewardModulation(reward, 0.0)
            
            agent.updateDevelopment(0.1)
            
            if step % 100 == 0:
                print(f"Step {step}: Firing: {brain.getFiringNeuronCount()}, "
                      f"Curiosity: {agent.getCuriosityLevel():.3f}, "
                      f"DevStage: {brain.getDevelopmentalStage()}")
    
    print(f"Test scenario '{args.scenario}' completed!")
    
    # Save results if requested
    if args.output:
        print(f"Saving test results to {args.output}")
        with open(args.output, 'w') as f:
            f.write(f"Test scenario: {args.scenario}\n")
            f.write(f"Total steps: {args.steps}\n")
            f.write(f"Final neuron count: {brain.getTotalNeuronCount()}\n")
            f.write(f"Final synapse count: {brain.getTotalSynapseCount()}\n")
            f.write(f"Total spikes: {brain.getTotalSpikeCount()}\n")


def main():
    """Main entry point"""
    parser = create_parser()
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    try:
        if args.command == 'run':
            run_simulation(args)
        elif args.command == 'config':
            config_command(args)
        elif args.command == 'brain':
            brain_command(args)
        elif args.command == 'stats':
            stats_command(args)
        elif args.command == 'test':
            test_command(args)
        elif args.command == 'help':
            parser.print_help()
    except KeyboardInterrupt:
        print("\nInterrupted by user")
    except Exception as e:
        print(f"Error: {e}")
        sys.exit(1)


if __name__ == '__main__':
    main()
