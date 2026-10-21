#!/usr/bin/env python3
"""
NLM Command Line Interface

Advanced command-line interface for NLM (Neural Learning Machine).
Provides tools for running simulations, managing configurations, and debugging.
"""

import argparse
import json
import sys
import os
from pathlib import Path

# Add parent directory to path so we can import pynlm
sys.path.insert(0, str(Path(__file__).parent))

import pynlm

# Version information
NLM_VERSION = "0.1.0"

def print_version():
    """Print version information."""
    print("NLM Command Line Interface")
    print(f"Version: {NLM_VERSION}")
    print("Phase 6: Final Integration")
    print("Built with Python bindings for NLM")

def create_brain(args):
    """Create and initialize a brain with specified configuration."""
    config = pynlm.createDefaultConfig()
    
    if args.neurons:
        config.set("brain.neuron_count", args.neurons)
    if args.density:
        config.set("brain.synapse_density", args.density)
    
    print(f"Creating brain with {args.neurons} neurons and synapse density {args.density}...")
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    if args.validate:
        is_valid, errors = pynlm.validateBrainConfiguration(config)
        if is_valid:
            print("Configuration is valid!")
        else:
            print("Configuration errors:")
            for error in errors:
                print(f"  - {error}")
    
    return brain

def run_simulation(args):
    """Run a simulation with the brain and world."""
    brain = create_brain(args)
    
    world = pynlm.createSimpleWorld()
    world.configure(width=20, height=20, visionWidth=8, visionHeight=8)
    world.reset()
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(world)
    
    # Enable features if requested
    if args.reward:
        agent.enableRewardModulation(True)
    if args.curiosity:
        agent.enableCuriosity(True)
    if args.development:
        agent.enableDevelopment(True)
    if args.plasticity:
        agent.enableStructuralPlasticity(True)
    
    print(f"Running simulation for {args.steps} steps...")
    
    for step in range(args.steps):
        world.update(0.1)
        percept = world.getSensoryPercept()
        agent.processSensoryInput(percept)
        brain.step(step)
        action = agent.decodeMotorCommand()
        world.applyMotorCommand(action, world.getSimulationTime())
        
        if args.verbose and step % args.verbose == 0:
            print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing, "
                  f"curiosity: {agent.getCuriosityLevel():.3f}")
    
    print("Simulation complete!")
    print(f"Final statistics:")
    print(f"  Total neurons: {brain.getTotalNeuronCount()}")
    print(f"  Total synapses: {brain.getTotalSynapseCount()}")
    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
    print(f"  Firing neurons (last step): {brain.getFiringNeuronCount()}")
    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
    print(f"  Development stage: {agent.getDevelopmentalStage()}")
    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
    
    if args.save:
        brain.save(args.save)
        print(f"Brain state saved to {args.save}")
    
    return brain

def test_system(args):
    """Run system test and show results."""
    print("Running NLM system test...")
    test_result = pynlm.runQuickTest()
    print("Test results:")
    print(test_result)
    
    if args.export:
        try:
            with open(args.export, 'w') as f:
                json.dump({
                    "test_result": test_result,
                    "timestamp": str(sys.time.time()),
                    "version": NLM_VERSION
                }, f, indent=2)
            print(f"Test results exported to {args.export}")
        except Exception as e:
            print(f"Error exporting results: {e}")
            return 1
    
    return 0

def config_commands(args):
    """Handle configuration commands."""
    if args.config_dict:
        try:
            with open(args.config_dict, 'r') as f:
                config_dict = json.load(f)
            
            config = pynlm.createConfigFromDict(config_dict)
            print("Configuration loaded from dictionary")
            
            if args.output:
                try:
                    with open(args.output, 'w') as f:
                        json.dump(pynlm.convertToDict(config), f, indent=2)
                    print(f"Configuration saved to {args.output}")
                except Exception as e:
                    print(f"Error saving configuration: {e}")
                    return 1
            else:
                print("Configuration:", pynlm.convertToDict(config))
                
        except FileNotFoundError:
            print(f"Error: Configuration file {args.config_dict} not found")
            return 1
        except json.JSONDecodeError as e:
            print(f"Error: Invalid JSON in {args.config_dict}: {e}")
            return 1
        except Exception as e:
            print(f"Error loading configuration: {e}")
            return 1
    
    return 0

def interactive_mode():
    """Start interactive REPL mode."""
    print("Starting NLM interactive mode...")
    print("Available commands:")
    print("  help          - Show this help")
    print("  quit|exit     - Exit interactive mode")
    print("  brain <n>     - Create brain with n neurons")
    print("  world <w> <h> - Create world with width w and height h")
    print("  agent         - Create agent brain interface")
    print("  run <steps>   - Run simulation for specified steps")
    print("  stats         - Show current system statistics")
    print("  config        - Show current configuration")
    print()
    
    brain = None
    world = None
    agent = None
    
    while True:
        try:
            cmd = input("nlm> ").strip()
            if not cmd:
                continue
                
            parts = cmd.split()
            cmd_type = parts[0].lower()
            
            if cmd_type in ['quit', 'exit']:
                break
            elif cmd_type == 'help':
                print("Available commands:")
                print("  help          - Show this help")
                print("  quit|exit     - Exit interactive mode")
                print("  brain <n>     - Create brain with n neurons")
                print("  world <w> <h> - Create world with width w and height h")
                print("  agent         - Create agent brain interface")
                print("  run <steps>   - Run simulation for specified steps")
                print("  stats         - Show current system statistics")
                print("  config        - Show current configuration")
                print("  validate      - Validate current configuration")
                print("  test          - Run system test")
                print()
                
            elif cmd_type == 'brain':
                if len(parts) < 2:
                    print("Usage: brain <n> - where n is number of neurons")
                    continue
                
                neuron_count = int(parts[1])
                config = pynlm.createDefaultConfig()
                config.set("brain.neuron_count", neuron_count)
                brain = pynlm.createBrain(config)
                brain.initialize()
                print(f"Brain created with {neuron_count} neurons")
                
            elif cmd_type == 'world':
                if len(parts) < 3:
                    print("Usage: world <width> <height>")
                    continue
                
                width = float(parts[1])
                height = float(parts[2])
                
                world = pynlm.createSimpleWorld()
                world.configure(width=width, height=height, visionWidth=8, visionHeight=8)
                world.reset()
                print(f"World created: {width}x{height}")
                
            elif cmd_type == 'agent':
                if not brain:
                    print("Please create a brain first using 'brain <n>'")
                    continue
                
                agent = pynlm.createAgentBrain(brain)
                if world:
                    agent.initialize(world)
                else:
                    print("No world available. Agent created without world.")
                print("Agent brain interface created")
                
            elif cmd_type == 'run':
                if not brain or not agent:
                    print("Please create brain and agent first")
                    continue
                
                if len(parts) < 2:
                    print("Usage: run <steps> - where steps is number of simulation steps")
                    continue
                
                steps = int(parts[1])
                print(f"Running simulation for {steps} steps...")
                
                for step in range(steps):
                    if world:
                        world.update(0.1)
                        percept = world.getSensoryPercept()
                        agent.processSensoryInput(percept)
                    
                    brain.step(step)
                    action = agent.decodeMotorCommand()
                    
                    if world:
                        world.applyMotorCommand(action, world.getSimulationTime())
                    
                    if step % 50 == 0:
                        print(f"Step {step}: {brain.getFiringNeuronCount()} neurons firing")
                
                print("Simulation complete!")
                
            elif cmd_type == 'stats':
                if brain:
                    print(f"Brain stats:")
                    print(f"  Neurons: {brain.getTotalNeuronCount()}")
                    print(f"  Synapses: {brain.getTotalSynapseCount()}")
                    print(f"  Active neurons: {brain.getActiveNeuronCount()}")
                    print(f"  Total spikes: {brain.getTotalSpikeCount()}")
                    print(f"  Average firing rate: {brain.getAverageFiringRate():.3f}")
                
                if agent:
                    print(f"Agent stats:")
                    print(f"  Development stage: {agent.getDevelopmentalStage()}")
                    print(f"  Curiosity level: {agent.getCuriosityLevel():.3f}")
                    print(f"  Novelty level: {agent.getNoveltyLevel():.3f}")
                    print(f"  Neuromod level: {agent.getNeuromodulationLevel():.3f}")
                
                if world:
                    body = world.getAgentBody()
                    print(f"World state:")
                    print(f"  Position: ({body.x}, {body.y})")
                    print(f"  Energy: {body.energy}/{world.getMaxEnergy()}")
                    print(f"  Health: {body.health}")
                
            elif cmd_type == 'config':
                config = brain.getConfig() if brain else pynlm.createDefaultConfig()
                print(f"Current configuration:")
                for key in config.getKeys():
                    if config.has(key):
                        print(f"  {key}: {config.get(key)}")
                
            elif cmd_type == 'validate':
                config = brain.getConfig() if brain else pynlm.createDefaultConfig()
                is_valid, errors = pynlm.validateBrainConfiguration(config)
                if is_valid:
                    print("Configuration is valid!")
                else:
                    print("Configuration errors:")
                    for error in errors:
                        print(f"  - {error}")
                
            elif cmd_type == 'test':
                test_result = pynlm.runQuickTest()
                print("System test results:")
                print(test_result)
                
            else:
                print(f"Unknown command: {cmd_type}")
                print("Type 'help' for available commands")
                
        except KeyboardInterrupt:
            print("\nInterrupted by user")
            break
        except Exception as e:
            print(f"Error: {e}")
    
    print("Exiting interactive mode")

def main():
    parser = argparse.ArgumentParser(
        description="NLM - Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_cli.py run --neurons 1000 --density 0.1 --steps 100
  nlm_cli.py test --export test_results.json
  nlm_cli.py config --config_dict my_config.json
  nlm_cli.py run --save brain_checkpoint.bin --verbose 10
  nlm_cli.py interactive

For more information, visit https://github.com/nlm-project/nlm
        """
    )
    
    parser.add_argument('--version', action='store_true',
                       help='Show version information and exit')
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    run_parser = subparsers.add_parser('run', help='Run a simulation')
    run_parser.add_argument('--neurons', type=int, default=1000,
                           help='Number of neurons (default: 1000)')
    run_parser.add_argument('--density', type=float, default=0.1,
                           help='Synapse density (0.0-1.0, default: 0.1)')
    run_parser.add_argument('--steps', type=int, default=100,
                           help='Number of simulation steps (default: 100)')
    run_parser.add_argument('--reward', action='store_true',
                           help='Enable reward modulation')
    run_parser.add_argument('--curiosity', action='store_true',
                           help='Enable curiosity-driven exploration')
    run_parser.add_argument('--development', action='store_true',
                           help='Enable developmental processes')
    run_parser.add_argument('--plasticity', action='store_true',
                           help='Enable structural plasticity')
    run_parser.add_argument('--validate', action='store_true',
                           help='Validate configuration before running')
    run_parser.add_argument('--save', help='Save brain state to file')
    run_parser.add_argument('--verbose', type=int, default=0,
                           help='Print verbose output every N steps')
    
    # Test system command
    test_parser = subparsers.add_parser('test', help='Test system functionality')
    test_parser.add_argument('--export', help='Export test results to JSON file')
    
    # Configuration command
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_parser.add_argument('--config_dict', help='Load configuration from JSON file')
    config_parser.add_argument('--output', help='Save configuration to file')
    
    # Interactive mode
    interactive_parser = subparsers.add_parser('interactive', help='Start interactive REPL mode')
    
    # Version command
    version_parser = subparsers.add_parser('version', help='Show version information')
    
    args = parser.parse_args()
    
    if args.version:
        print_version()
        return 0
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        if args.command == 'run':
            run_simulation(args)
        elif args.command == 'test':
            test_system(args)
        elif args.command == 'config':
            config_commands(args)
        elif args.command == 'interactive':
            interactive_mode()
        elif args.command == 'version':
            print_version()
        
        return 0
        
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        if '--verbose' in sys.argv or '--verbose' in ' '.join(sys.argv):
            import traceback
            traceback.print_exc()
        return 1

if __name__ == '__main__':
    sys.exit(main())
