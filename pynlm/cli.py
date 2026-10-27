#!/usr/bin/env python3
"""
NLM Command Line Interface

This module provides command-line tools for working with NLM (Neural Learning Machine).
It includes utilities for running simulations, managing configurations, and exploring the system.
"""

import sys
import os
import argparse
import json
from pathlib import Path
from typing import Optional, Dict, Any

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent))

try:
    import pynlm
    import pynlm_enhanced
    HAS_NLM = True
except ImportError as e:
    HAS_NLM = False
    print(f"Warning: NLM Python bindings not available: {e}")
    print("Some CLI commands will not be available.")


def print_banner():
    """Print the NLM banner."""
    print("╔═══════════════════════════════════════════════════════════════╗")
    print("║                                                               ║")
    print("║     NLM — 熙然                                              ║")
    print("║     Neural Learning Machine                                 ║")
    print("║                                                               ║")
    print("║     Command Line Interface                                  ║")
    print("║                                                               ║")
    print("╚═══════════════════════════════════════════════════════════════╝")


def create_brain_from_config(config_file: Optional[str] = None,
                             neuron_count: Optional[int] = None,
                             region_count: Optional[int] = None) -> 'pynlm.Brain':
    """Create a brain with the specified configuration."""
    if not HAS_NLM:
        raise RuntimeError("NLM is not available")
    
    config = pynlm.createDefaultConfig()
    
    if config_file and os.path.exists(config_file):
        config.loadFromFile(config_file)
    
    if neuron_count is not None:
        config.set("neuron_count", neuron_count)
    if region_count is not None:
        config.set("region_count", region_count)
    
    return pynlm.createBrain(config)


def run_simple_simulation(args):
    """Run a simple simulation using the NLM brain."""
    print(f"Running simulation with {args.num_steps} steps...")
    
    brain = create_brain_from_config(
        args.config_file,
        args.neuron_count,
        args.region_count
    )
    
    brain.initialize()
    
    for step in range(args.num_steps):
        brain.step(step)
        
        if args.verbose and step % args.verbose_interval == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} firing neurons, "
                  f"{brain.getTotalSpikeCount()} total spikes")
    
    print(f"\nSimulation complete!")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Firing neurons: {brain.getFiringNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.2f} Hz")
    print(f"  E/I ratio: {brain.getExcitationInhibitionRatio():.2f}")


def run_agent_simulation(args):
    """Run an agent simulation with brain and world."""
    print(f"Running agent simulation with {args.num_steps} steps...")
    
    try:
        # Try to import enhanced API
        import pynlm_enhanced
        
        if args.enhanced:
            # Use enhanced API
            brain, world, agent = pynlm_enhanced.create_simple_agent()
            
            pynlm_enhanced.run_learning_simulation(
                brain, world, agent, args.num_steps,
                enable_reward_modulation=args.reward,
                enable_curiosity=args.curiosity,
                verbose=args.verbose
            )
        else:
            # Use standard API
            brain = create_brain_from_config(
                args.config_file,
                args.neuron_count,
                args.region_count
            )
            
            world = pynlm.createSimpleWorld()
            world.configure(
                width=args.world_width,
                height=args.world_height,
                visionWidth=args.vision_width,
                visionHeight=args.vision_height
            )
            world.reset()
            world.setAgentStart(args.agent_x, args.agent_y)
            
            agent = pynlm.createAgentBrain(brain)
            agent.initialize(world)
            
            if args.reward:
                agent.enableRewardModulation(True)
            if args.curiosity:
                agent.enableCuriosity(True)
            if args.development:
                agent.enableDevelopment(True)
            if args.structural_plasticity:
                agent.enableStructuralPlasticity(True)
            
            for step in range(args.num_steps):
                world.update(0.1)
                
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                
                if args.verbose and step % args.verbose_interval == 0:
                    print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                          f"{agent.getCuriosityLevel():.2f} curiosity, "
                          f"{agent.getNeuromodulationLevel():.2f} neuromod")
        
        print(f"\nAgent simulation complete!")
        
    except ImportError:
        print("Enhanced API not available, falling back to standard API...")
        # Fallback to standard API
        brain = create_brain_from_config(
            args.config_file,
            args.neuron_count,
            args.region_count
        )
        
        world = pynlm.createSimpleWorld()
        world.configure(
            width=args.world_width,
            height=args.world_height,
            visionWidth=args.vision_width,
            visionHeight=args.vision_height
        )
        world.reset()
        world.setAgentStart(args.agent_x, args.agent_y)
        
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        if args.reward:
            agent.enableRewardModulation(True)
        if args.curiosity:
            agent.enableCuriosity(True)
        
        for step in range(args.num_steps):
            world.update(0.1)
            
            percept = world.getSensoryPercept()
            agent.processSensoryInput(percept)
            brain.step(step)
            
            action = agent.decodeMotorCommand()
            world.applyMotorCommand(action, world.getSimulationTime())
            
            if args.verbose and step % args.verbose_interval == 0:
                print(f"Step {step}: {brain.getFiringNeuronCount()} firing, "
                      f"{agent.getCuriosityLevel():.2f} curiosity")


def export_brain(brain, filepath: str):
    """Export brain state to file."""
    print(f"Exporting brain state to {filepath}...")
    
    success = brain.save(filepath)
    
    if success:
        print(f"Brain state successfully exported to {filepath}")
    else:
        print(f"Failed to export brain state to {filepath}")


def import_brain(filepath: str):
    """Import brain state from file."""
    print(f"Importing brain state from {filepath}...")
    
    if not os.path.exists(filepath):
        print(f"File not found: {filepath}")
        return None
    
    brain = create_brain_from_config()
    
    success = brain.load(filepath)
    
    if success:
        print(f"Brain state successfully imported from {filepath}")
        return brain
    else:
        print(f"Failed to import brain state from {filepath}")
        return None


def create_config_from_python_dict(args):
    """Create configuration from Python dictionary."""
    if not args.config_dict:
        return
    
    try:
        config_dict = json.loads(args.config_dict)
        config = pynlm.createDefaultConfig()
        
        for key, value in config_dict.items():
            config.set(key, value)
        
        config_file = args.config_dict_path or "config.json"
        config.saveToFile(config_file)
        
        print(f"Configuration saved to {config_file}")
        
    except json.JSONDecodeError as e:
        print(f"Invalid JSON in config dictionary: {e}")
    except Exception as e:
        print(f"Error creating configuration: {e}")


def explore_features(args):
    """Explore NLM features and capabilities."""
    print("Exploring NLM features...")
    
    if not HAS_NLM:
        print("NLM not available for exploration")
        return
    
    brain = create_brain_from_config(neuron_count=100, region_count=2)
    brain.initialize()
    
    print(f"Created test brain with {brain.getTotalNeuronCount()} neurons")
    print(f"Brain can export status: {brain.getTotalSynapseCount()} synapses")
    
    # Test different developmental stages
    for stage_name in ['Initial', 'CriticalPeriod', 'Maturation', 'Adult']:
        print(f"Testing {stage_name} developmental stage simulation...")
        brain.step(0)
    
    print("\nFeature exploration complete!")
    print("\nAvailable features:")
    print("  - Advanced error handling with custom exception types")
    print("  - Enhanced configuration management")
    print("  - Performance monitoring utilities")
    print("  - Convenience methods for common operations")
    print("  - Context managers for resource management")
    print("  - Learning simulation helpers")


def main():
    """Main entry point for the NLM CLI."""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s simple --num-steps 1000 --neuron-count 500
  %(prog)s agent --num-steps 500 --world-width 20 --world-height 20
  %(prog)s export --filepath brain_state.bin
  %(prog)s import --filepath brain_state.bin
  %(prog)s explore
  %(prog)s config --config-dict '{"neuron_count": 1000, "connection_probability": 0.2}'
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Simple simulation command
    parser_simple = subparsers.add_parser(
        'simple', help='Run a simple simulation with just the brain'
    )
    parser_simple.add_argument('--num-steps', type=int, default=1000,
                               help='Number of simulation steps (default: 1000)')
    parser_simple.add_argument('--neuron-count', type=int, default=500,
                               help='Number of neurons (default: 500)')
    parser_simple.add_argument('--region-count', type=int, default=2,
                               help='Number of brain regions (default: 2)')
    parser_simple.add_argument('--config-file', type=str,
                               help='Path to configuration file')
    parser_simple.add_argument('--verbose', action='store_true',
                               help='Enable verbose output')
    parser_simple.add_argument('--verbose-interval', type=int, default=100,
                               help='Interval between verbose output (default: 100)')
    
    # Agent simulation command
    parser_agent = subparsers.add_parser(
        'agent', help='Run an agent simulation with brain and world'
    )
    parser_agent.add_argument('--num-steps', type=int, default=1000,
                              help='Number of simulation steps (default: 1000)')
    parser_agent.add_argument('--neuron-count', type=int, default=500,
                              help='Number of neurons (default: 500)')
    parser_agent.add_argument('--region-count', type=int, default=2,
                              help='Number of brain regions (default: 2)')
    parser_agent.add_argument('--config-file', type=str,
                              help='Path to configuration file')
    parser_agent.add_argument('--world-width', type=float, default=20.0,
                              help='World width (default: 20.0)')
    parser_agent.add_argument('--world-height', type=float, default=20.0,
                              help='World height (default: 20.0)')
    parser_agent.add_argument('--vision-width', type=int, default=8,
                              help='Vision width (default: 8)')
    parser_agent.add_argument('--vision-height', type=int, default=8,
                              help='Vision height (default: 8)')
    parser_agent.add_argument('--agent-x', type=float, default=10.0,
                              help='Agent starting X position (default: 10.0)')
    parser_agent.add_argument('--agent-y', type=float, default=10.0,
                              help='Agent starting Y position (default: 10.0)')
    parser_agent.add_argument('--reward', action='store_true',
                              help='Enable reward modulation')
    parser_agent.add_argument('--curiosity', action='store_true',
                              help='Enable curiosity-driven exploration')
    parser_agent.add_argument('--development', action='store_true',
                              help='Enable developmental processes')
    parser_agent.add_argument('--structural-plasticity', action='store_true',
                              help='Enable structural plasticity')
    parser_agent.add_argument('--enhanced', action='store_true',
                              help='Use enhanced API (if available)')
    parser_agent.add_argument('--verbose', action='store_true',
                              help='Enable verbose output')
    parser_agent.add_argument('--verbose-interval', type=int, default=100,
                              help='Interval between verbose output (default: 100)')
    
    # Export brain command
    parser_export = subparsers.add_parser(
        'export', help='Export brain state to file'
    )
    parser_export.add_argument('--filepath', type=str, required=True,
                              help='Path to export brain state to')
    parser_export.add_argument('--neuron-count', type=int, default=500,
                              help='Number of neurons (default: 500)')
    parser_export.add_argument('--region-count', type=int, default=2,
                              help='Number of brain regions (default: 2)')
    
    # Import brain command
    parser_import = subparsers.add_parser(
        'import', help='Import brain state from file'
    )
    parser_import.add_argument('--filepath', type=str, required=True,
                              help='Path to import brain state from')
    
    # Config command
    parser_config = subparsers.add_parser(
        'config', help='Create or manage configuration'
    )
    parser_config.add_argument('--config-dict', type=str,
                               help='JSON configuration dictionary')
    parser_config.add_argument('--config-dict-path', type=str, default='config.json',
                               help='Path to save configuration (default: config.json)')
    
    # Explore command
    parser_explore = subparsers.add_parser(
        'explore', help='Explore NLM features and capabilities'
    )
    
    # Version command
    parser_version = subparsers.add_parser(
        'version', help='Show version information'
    )
    
    # Main parsing
    args = parser.parse_args()
    
    if not HAS_NLM:
        print("Error: NLM Python bindings are not available")
        print("Please install the NLM package using pip install pynlm")
        sys.exit(1)
    
    print_banner()
    
    if args.command == 'simple':
        run_simple_simulation(args)
    elif args.command == 'agent':
        run_agent_simulation(args)
    elif args.command == 'export':
        brain = create_brain_from_config(
            None,  # No config file
            args.neuron_count,
            args.region_count
        )
        brain.initialize()
        export_brain(brain, args.filepath)
    elif args.command == 'import':
        import_brain(args.filepath)
    elif args.command == 'config':
        create_config_from_python_dict(args)
    elif args.command == 'explore':
        explore_features(args)
    elif args.command == 'version':
        print(f"NLM CLI Version: 1.0.0")
        print(f"Python bindings: pynlm 0.1.0")
        print(f"Enhanced bindings: pynlm_enhanced 0.1.0")
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == '__main__':
    main()
