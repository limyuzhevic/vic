#!/usr/bin/env python3
"""
Command-line interface for NLM (Neural Learning Machine)

This script provides command-line tools for common NLM operations including:
- Running simulations
- Creating checkpoints
- Loading saved states
- Running experiments
- Analyzing results

Usage examples:
    python nlm_cli.py run --steps 100 --seed 42
    python nlm_cli.py save brain_state.json --steps 50
    python nlm_cli.py load brain_state.json --steps 100
    python nlm_cli.py experiment --type phase6 --duration 60
"""

import pynlm_enhanced as nlm
import json
import argparse
import sys
import os
from pathlib import Path

def setup_simulation(args):
    """Set up a simulation with given arguments"""
    print("Setting up simulation...")
    
    # Create configuration
    config = nlm.createDefaultConfig()
    
    # Set configuration from command line arguments
    if args.seed:
        config.set("random_seed", args.seed)
    
    # Create components
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    # Configure world
    if args.width and args.height:
        world.configure(args.width, args.height, args.vision_width, args.vision_height)
    
    # Set agent start position if specified
    if args.start_x is not None and args.start_y is not None:
        world.setAgentStart(args.start_x, args.start_y)
    
    # Initialize agent
    agent.initialize(world)
    
    return brain, world, agent

def run_simulation(args):
    """Run a simulation with given parameters"""
    print(f"Running simulation for {args.steps} steps...")
    
    brain, world, agent = setup_simulation(args)
    
    # Enable features based on arguments
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
        # Update world
        world.update(args.timestep)
        
        # Get percept and process
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        
        # Brain step
        brain.step(step)
        
        # Get action and apply
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Apply reward modulation
        if args.reward_modulation:
            body = world.getAgentBody()
            reward = 1.0 if body.energy > 50.0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if args.development:
            agent.updateDevelopment(args.timestep)
        
        # Log progress periodically
        if args.log_interval > 0 and step % args.log_interval == 0:
            stats = nlm.formatStats(brain.getBrain())
            body = world.getAgentBody()
            print(f"Step {step}: Pos({body.x:.1f}, {body.y:.1f}), "
                  f"Energy: {body.energy:.1f}, "
                  f"Development: {agent.getDevelopmentalStage()}, "
                  f"Neuromod: {agent.getNeuromodulationLevel():.2f}")
            print(stats)
    
    # Print final statistics
    print(f"\nSimulation complete!")
    print(f"Final position: {world.getAgentBody().x}, {world.getAgentBody().y}")
    print(f"Final energy: {world.getAgentBody().energy}")
    print(f"Final development stage: {agent.getDevelopmentalStage()}")
    print(f"Final neuromodulation level: {agent.getNeuromodulationLevel():.2f}")
    
    print(f"\nFinal statistics:")
    print(nlm.formatStats(brain.getBrain()))
    
    return brain, world, agent

def save_state(args):
    """Save brain state to file"""
    print(f"Loading configuration for state save...")
    
    brain, world, agent = setup_simulation(args)
    brain.initialize()
    
    # Allow some steps to warm up the brain if not specified
    steps = args.steps if args.steps else 10
    
    for step in range(steps):
        world.update(0.1)
        agent.processSensoryInput(world.getSensoryPercept())
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
    
    # Save brain state
    print(f"Saving brain state to {args.output}...")
    success = brain.save(args.output)
    
    if success:
        print("Brain state saved successfully!")
        
        # Also save metadata
        metadata = {
            "steps": steps,
            "width": world.getWidth(),
            "height": world.getHeight(),
            "random_seed": args.seed or nlm.createRandomSeed(),
            "simulation_time": world.getSimulationTime(),
            "neuron_count": brain.getBrain().getTotalNeuronCount(),
            "synapse_count": brain.getBrain().getTotalSynapseCount(),
            "timestamp": str(__import__('datetime').datetime.now())
        }
        
        metadata_path = Path(args.output).with_suffix('.meta.json')
        with open(metadata_path, 'w') as f:
            json.dump(metadata, f, indent=2)
        
        print(f"Metadata saved to {metadata_path}")
    else:
        print(f"Failed to save brain state to {args.output}")
        sys.exit(1)

def load_state(args):
    """Load brain state from file"""
    print(f"Loading brain state from {args.input}...")
    
    # Check if files exist
    if not os.path.exists(args.input):
        print(f"Error: State file '{args.input}' not found")
        sys.exit(1)
    
    # Try to load metadata if available
    metadata_path = Path(args.input).with_suffix('.meta.json')
    if os.path.exists(metadata_path):
        with open(metadata_path, 'r') as f:
            metadata = json.load(f)
        print(f"Loaded metadata: {metadata}")
    
    # Setup simulation components
    config = nlm.createDefaultConfig()
    if args.seed:
        config.set("random_seed", args.seed)
    
    brain = nlm.createBrain(config)
    world = nlm.createSimpleWorld()
    agent = nlm.createAgentBrain(brain)
    
    # Configure world from metadata or arguments
    if metadata_path.exists():
        world.configure(metadata["width"], metadata["height"])
    else:
        world.configure(args.width or 10, args.height or 10)
    
    # Load brain state
    print("Loading brain state...")
    success = brain.load(args.input)
    
    if success:
        print("Brain state loaded successfully!")
        
        # Initialize agent
        agent.initialize(world)
        
        # Allow some warm-up steps if not loading from checkpoint
        if not brain.getBrain().getTotalSynapseCount() > 0:
            for step in range(10):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
        
        # Run additional steps if specified
        if args.additional_steps:
            print(f"Running {args.additional_steps} additional steps...")
            for step in range(args.additional_steps):
                world.update(0.1)
                agent.processSensoryInput(world.getSensoryPercept())
                brain.step(step + 10)  # Continue step numbering
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if args.log_interval > 0 and step % args.log_interval == 0:
                    print(f"Loaded simulation step {step + 10}")
    else:
        print(f"Failed to load brain state from {args.input}")
        sys.exit(1)
    
    return brain, world, agent

def run_experiment(args):
    """Run an experiment"""
    print(f"Running {args.experiment_type} experiment for {args.duration} seconds...")
    
    brain, world, agent = setup_simulation(args)
    brain.initialize()
    
    # Configure based on experiment type
    if args.experiment_type == "phase6":
        world.configure(20, 20, 12, 12)
        agent.enableRewardModulation(True)
        agent.enableStructuralPlasticity(True)
        agent.enableDevelopment(True)
        agent.enableCuriosity(True)
    
    # Run experiment
    start_time = world.getSimulationTime()
    step = 0
    
    while world.getSimulationTime() - start_time < args.duration:
        world.update(0.05)
        
        # Process sensory input
        agent.processSensoryInput(world.getSensoryPercept())
        
        # Brain step
        brain.step(step)
        
        # Get action
        action = agent.decodeMotorCommand()
        
        # Apply action
        world.applyMotorCommand(action, world.getSimulationTime())
        
        # Update neuromodulation
        if args.reward_modulation:
            body = world.getAgentBody()
            reward = 1.0 if body.energy > 50.0 else 0.0
            agent.applyRewardModulation(reward, 0.0)
        
        # Update development
        if args.development:
            agent.updateDevelopment(0.05)
        
        # Log progress
        if args.log_interval > 0 and step % args.log_interval == 0:
            stats = nlm.formatStats(brain.getBrain())
            print(f"Experiment step {step} (t={world.getSimulationTime():.1f}s)")
            print(stats)
        
        step += 1
    
    print(f"Experiment completed!")
    print(f"Ran {step} steps in {world.getSimulationTime() - start_time:.1f} seconds")
    
    return brain, world, agent

def analyze_results(args):
    """Analyze and visualize results"""
    print("Analyzing results...")
    
    if not args.input:
        print("Error: No input file specified for analysis")
        sys.exit(1)
    
    # Load state
    brain, world, agent = load_state(args)
    
    # Generate analysis report
    print("\n=== Analysis Report ===")
    print(f"File: {args.input}")
    
    brain_obj = brain.getBrain()
    print(f"Neurons: {brain_obj.getTotalNeuronCount()}")
    print(f"Synapses: {brain_obj.getTotalSynapseCount()}")
    print(f"Active neurons: {brain_obj.getActiveNeuronCount()}")
    print(f"Firing neurons: {brain_obj.getFiringNeuronCount()}")
    print(f"Total spikes: {brain_obj.getTotalSpikeCount()}")
    print(f"Average firing rate: {brain_obj.getAverageFiringRate():.2f} Hz")
    print(f"Excitation/inhibition ratio: {brain_obj.getExcitationInhibitionRatio():.3f}")
    print(f"Development stage: {agent.getDevelopmentalStage()}")
    
    # Get world statistics
    body = world.getAgentBody()
    print(f"Final position: ({body.x}, {body.y})")
    print(f"Final energy: {body.energy}")
    print(f"Final health: {body.health}")
    print(f"Age: {body.age}")
    
    # Save analysis report
    if args.output:
        report_path = Path(args.input).with_suffix('.analysis.json')
        with open(report_path, 'w') as f:
            analysis_data = {
                "file": args.input,
                "neurons": brain_obj.getTotalNeuronCount(),
                "synapses": brain_obj.getTotalSynapseCount(),
                "active_neurons": brain_obj.getActiveNeuronCount(),
                "firing_neurons": brain_obj.getFiringNeuronCount(),
                "total_spikes": brain_obj.getTotalSpikeCount(),
                "average_firing_rate": brain_obj.getAverageFiringRate(),
                "excitation_inhibition_ratio": brain_obj.getExcitationInhibitionRatio(),
                "development_stage": agent.getDevelopmentalStage(),
                "final_position": {"x": body.x, "y": body.y},
                "final_energy": body.energy,
                "final_health": body.health,
                "age": body.age,
                "timestamp": str(__import__('datetime').datetime.now())
            }
            json.dump(analysis_data, f, indent=2)
        
        print(f"Analysis report saved to {report_path}")
    
    print("\nAnalysis complete!")

def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Run a simple simulation
  python nlm_cli.py run --steps 100 --seed 42

  # Save brain state to file
  python nlm_cli.py save brain_state.json --steps 50

  # Load and continue simulation
  python nlm_cli.py load brain_state.json --steps 100

  # Run Phase 6 experiment
  python nlm_cli.py experiment --type phase6 --duration 60

  # Analyze saved state
  python nlm_cli.py analyze brain_state.json

  # Run comprehensive simulation with features
  python nlm_cli.py run --steps 200 --width 30 --height 30 \
        --reward-modulation --development --curiosity
        """
    )
    
    # Global arguments
    parser.add_argument(
        "--seed",
        type=int,
        help="Random seed for reproducible simulations"
    )
    
    parser.add_argument(
        "--width",
        type=int,
        default=10,
        help="World width (default: 10)"
    )
    
    parser.add_argument(
        "--height",
        type=int,
        default=10,
        help="World height (default: 10)"
    )
    
    parser.add_argument(
        "--vision-width",
        type=int,
        default=8,
        help="Vision width (default: 8)"
    )
    
    parser.add_argument(
        "--vision-height",
        type=int,
        default=8,
        help="Vision height (default: 8)"
    )
    
    parser.add_argument(
        "--timestep",
        type=float,
        default=0.1,
        help="Simulation timestep (default: 0.1)"
    )
    
    parser.add_argument(
        "--log-interval",
        type=int,
        default=20,
        help="Log interval in steps (0 to disable, default: 20)"
    )
    
    parser.add_argument(
        "--start-x",
        type=float,
        default=2.0,
        help="Agent start X position (default: 2.0)"
    )
    
    parser.add_argument(
        "--start-y",
        type=float,
        default=2.0,
        help="Agent start Y position (default: 2.0)"
    )
    
    # Create subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run a simulation')
    run_parser.add_argument(
        'steps',
        type=int,
        help='Number of simulation steps'
    )
    run_parser.add_argument(
        '--reward-modulation',
        action='store_true',
        help='Enable reward modulation'
    )
    run_parser.add_argument(
        '--structural-plasticity',
        action='store_true',
        help='Enable structural plasticity'
    )
    run_parser.add_argument(
        '--development',
        action='store_true',
        help='Enable development'
    )
    run_parser.add_argument(
        '--curiosity',
        action='store_true',
        help='Enable curiosity'
    )
    
    # Save command
    save_parser = subparsers.add_parser('save', help='Save brain state to file')
    save_parser.add_argument(
        'output',
        help='Output file path'
    )
    save_parser.add_argument(
        'steps',
        type=int,
        nargs='?',
        default=10,
        help='Number of steps to run before saving (default: 10)'
    )
    
    # Load command
    load_parser = subparsers.add_parser('load', help='Load brain state from file')
    load_parser.add_argument(
        'input',
        help='Input file path'
    )
    load_parser.add_argument(
        'additional_steps',
        type=int,
        nargs='?',
        default=0,
        help='Additional steps to run after loading (default: 0)'
    )
    
    # Experiment command
    experiment_parser = subparsers.add_parser('experiment', help='Run an experiment')
    experiment_parser.add_argument(
        'type',
        choices=['phase3', 'phase4', 'phase6'],
        default='phase6',
        help='Experiment type (default: phase6)'
    )
    experiment_parser.add_argument(
        'duration',
        type=float,
        help='Experiment duration in seconds'
    )
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze results')
    analyze_parser.add_argument(
        'input',
        help='Input file path'
    )
    analyze_parser.add_argument(
        '--output',
        help='Output analysis file path (default: <input>.analysis.json)'
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Route to appropriate command handler
    if args.command == 'run':
        run_simulation(args)
    elif args.command == 'save':
        save_state(args)
    elif args.command == 'load':
        load_state(args)
    elif args.command == 'experiment':
        run_experiment(args)
    elif args.command == 'analyze':
        analyze_results(args)
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == "__main__":
    main()