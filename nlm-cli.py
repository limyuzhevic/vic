#!/usr/bin/env python3

"""
NLM (熙然) - Neural Learning Machine Command Line Interface

This is the main command-line interface for the NLM brain simulation framework.
It provides easy access to demos, simulations, and analysis tools.

Usage examples:
  nlm run --demo phase3 --episodes 10 --steps 200    # Run Phase 3 demo
  nlm run --demo phase6 --quick                          # Quick integration test
  nlm brain create --neurons 1000 --output demo.brain   # Create and save a brain
  nlm brain load demo.brain --steps 500                 # Load and simulate a brain
  nlm analyze --input demo.brain --output stats.json   # Analyze brain metrics
  nlm config create --output config.json                # Create standard config
  nlm help                                                  # Show help
"""

import argparse
import json
import sys
import os
import subprocess
from pathlib import Path

# Add the python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not found. Some commands may not work.", file=sys.stderr)
def create_parser():
    """Create the main argument parser."""
    parser = argparse.ArgumentParser(
        description="NLM (熙然) - Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm run --demo phase3 --episodes 10 --steps 200
  nlm run --demo phase6 --quick
  nlm brain create --neurons 1000 --output demo.brain
  nlm brain load demo.brain --steps 500
  nlm analyze --input demo.brain --output stats.json
  nlm config create --output config.json
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run demo commands
    add_run_commands(subparsers)
    
    # Brain management commands
    add_brain_commands(subparsers)
    
    # Analysis commands
    add_analyze_commands(subparsers)
    
    # Configuration commands
    add_config_commands(subparsers)
    
    # Utility commands
    add_utility_commands(subparsers)
    
    return parser
def add_run_commands(subparsers):
    """Add run demo commands."""
    run_parser = subparsers.add_parser(
        'run', 
        aliases=['r'],
        help='Run demos and simulations',
        description='Run NLM demos and simulations with various configurations'
    )
    
    run_subparsers = run_parser.add_subparsers(dest='demo', help='Demo type')
    
    # Phase 3 demo
    phase3_parser = run_subparsers.add_parser(
        'phase3', 
        help='Phase 3: World Interaction Demo',
        description='Run Phase 3 demo - experience -> neural activity -> action -> consequence -> learning'
    )
    phase3_parser.add_argument('--episodes', '-e', type=int, default=5,
                              help='Number of episodes (default: 5)')
    phase3_parser.add_argument('--steps', '-s', type=int, default=200,
                              help='Steps per episode (default: 200)')
    phase3_parser.add_argument('--neurons', type=int, default=500,
                              help='Number of neurons (default: 500)')
    phase3_parser.add_argument('--regions', type=int, default=2,
                              help='Number of brain regions (default: 2)')
    phase3_parser.add_argument('--verbose', '-v', action='store_true',
                              help='Enable verbose output')
    phase3_parser.add_argument('--output', '-o', help='Output file for results')
    
    # Phase 4 demo (research mode)
    phase4_parser = run_subparsers.add_parser(
        'phase4', 
        help='Phase 4: Research Demo',
        description='Run Phase 4 demo - advanced research capabilities'
    )
    phase4_parser.add_argument('--episodes', '-e', type=int, default=3,
                              help='Number of episodes (default: 3)')
    phase4_parser.add_argument('--steps', '-s', type=int, default=1000,
                              help='Steps per episode (default: 1000)')
    phase4_parser.add_argument('--neurons', type=int, default=2000,
                              help='Number of neurons (default: 2000)')
    phase4_parser.add_argument('--curiosity', type=float, default=0.5,
                              help='Curiosity factor (0-1, default: 0.5)')
    phase4_parser.add_argument('--novelty', type=float, default=0.3,
                              help='Novelty factor (0-1, default: 0.3)')
    phase4_parser.add_argument('--rewards', type=float, default=1.0,
                              help='Reward magnitude (default: 1.0)')
    
    # Phase 6 demo (integration test)
    phase6_parser = run_subparsers.add_parser(
        'phase6', 
        aliases=['integration'],
        help='Phase 6: Integration Demo',
        description='Run Phase 6 integration demo - verify all systems work together'
    )
    phase6_parser.add_argument('--quick', '-q', action='store_true',
                              help='Quick integration test (fewer neurons)')
    phase6_parser.add_argument('--neurons', type=int, default=500,
                              help='Number of neurons (default: 500 for quick, 2000 for full)')
    phase6_parser.add_argument('--verbose', '-v', action='store_true',
                              help='Enable verbose output')
    
    # Custom simulation
    custom_parser = run_subparsers.add_parser(
        'custom', 
        help='Custom simulation',
        description='Run a custom simulation with specified parameters'
    )
    custom_parser.add_argument('--config', '-c', help='Configuration file path')
    custom_parser.add_argument('--steps', '-s', type=int, default=1000,
                              help='Number of simulation steps (default: 1000)')
    custom_parser.add_argument('--neurons', type=int, default=1000,
                              help='Number of neurons (default: 1000)')
    custom_parser.add_argument('--episodes', '-e', type=int, default=1,
                              help='Number of episodes (default: 1)')
    
    # Research mode
    research_parser = run_subparsers.add_parser(
        'research', 
        help='Research mode - advanced experimentation',
        description='Run research mode with advanced features enabled'
    )
    research_parser.add_argument('--enable-development', action='store_true',
                                default=True, help='Enable development system')
    research_parser.add_argument('--enable-curiosity', action='store_true',
                                default=True, help='Enable curiosity system')
    research_parser.add_argument('--enable-novelty', action='store_true',
                                default=True, help='Enable novelty detection')
    research_parser.add_argument('--enable-structural-plasticity', action='store_true',
                                default=True, help='Enable structural plasticity')
    research_parser.add_argument('--steps', '-s', type=int, default=5000,
                              help='Number of steps (default: 5000)')
    research_parser.add_argument('--output', '-o', help='Output file for results')

def add_brain_commands(subparsers):
    """Add brain management commands."""
    brain_parser = subparsers.add_parser(
        'brain',
        aliases=['b'],
        help='Brain management commands',
        description='Create, load, save, and manage brain states'
    )
    
    brain_subparsers = brain_parser.add_subparsers(dest='brain_action', help='Brain action')
    
    # Create brain
    create_parser = brain_subparsers.add_parser(
        'create', 
        help='Create a new brain',
        description='Create a new brain with specified configuration'
    )
    create_parser.add_argument('--neurons', '-n', type=int, default=1000,
                              help='Number of neurons (default: 1000)')
    create_parser.add_argument('--regions', '-r', type=int, default=2,
                              help='Number of brain regions (default: 2)')
    create_parser.add_argument('--output', '-o', required=True,
                              help='Output file path for brain checkpoint')
    create_parser.add_argument('--config', '-c', help='Configuration file path')
    
    # Load brain
    load_parser = brain_subparsers.add_parser(
        'load', 
        help='Load a brain from file',
        description='Load a brain from a checkpoint file'
    )
    load_parser.add_argument('input', help='Input brain checkpoint file')
    load_parser.add_argument('--steps', '-s', type=int, default=100,
                            help='Number of simulation steps (default: 100)')
    load_parser.add_argument('--output', '-o', help='Output file for results')
    
    # Save brain
    save_parser = brain_subparsers.add_parser(
        'save', 
        help='Save current brain to file',
        description='Save current brain state to a checkpoint file'
    )
    save_parser.add_argument('output', help='Output brain checkpoint file')
    
    # Brain analysis
    analyze_parser = brain_subparsers.add_parser(
        'analyze', 
        help='Analyze brain state',
        description='Analyze current brain state and generate metrics'
    )
    analyze_parser.add_argument('--output', '-o', required=True,
                              help='Output JSON file for analysis results')
    analyze_parser.add_argument('--format', choices=['json', 'text', 'csv'],
                              default='json', help='Output format (default: json)')

def add_analyze_commands(subparsers):
    """Add analysis commands."""
    analyze_parser = subparsers.add_parser(
        'analyze',
        aliases=['a'],
        help='Analyze brain data',
        description='Analyze brain data from files or simulations'
    )
    
    analyze_subparsers = analyze_parser.add_subparsers(dest='analysis_type', help='Analysis type')
    
    # Brain metrics
    metrics_parser = analyze_subparsers.add_parser(
        'metrics', 
        help='Analyze brain metrics',
        description='Analyze brain metrics and generate statistics'
    )
    metrics_parser.add_argument('--input', '-i', required=True,
                               help='Input brain checkpoint file')
    metrics_parser.add_argument('--output', '-o', required=True,
                               help='Output JSON file for metrics')
    metrics_parser.add_argument('--time-series', action='store_true',
                               help='Include time-series data in output')
    
    # Learning curve
    curve_parser = analyze_subparsers.add_parser(
        'curve', 
        help='Generate learning curve',
        description='Generate learning curve from simulation data'
    )
    curve_parser.add_argument('--input', '-i', required=True,
                             help='Input simulation data file')
    curve_parser.add_argument('--output', '-o', required=True,
                             help='Output learning curve plot file')
    curve_parser.add_argument('--metric', choices=['reward', 'firing_rate', 'energy', 'curiosity'],
                             default='reward', help='Metric to plot (default: reward)')

def add_config_commands(subparsers):
    """Add configuration commands."""
    config_parser = subparsers.add_parser(
        'config',
        aliases=['c'],
        help='Configuration commands',
        description='Create and manage configuration files'
    )
    
    config_subparsers = config_parser.add_subparsers(dest='config_action', help='Configuration action')
    
    # Create config
    create_parser = config_subparsers.add_parser(
        'create', 
        help='Create configuration file',
        description='Create a configuration file with specified settings'
    )
    create_parser.add_argument('--output', '-o', required=True,
                              help='Output configuration file')
    create_parser.add_argument('--type', choices=['minimal', 'standard', 'research'],
                              default='standard', help='Configuration type (default: standard)')
    create_parser.add_argument('--neurons', type=int, help='Number of neurons')
    create_parser.add_argument('--steps', type=int, help='Number of simulation steps')
    
    # Convert config
    convert_parser = config_subparsers.add_parser(
        'convert', 
        help='Convert configuration format',
        description='Convert configuration file between formats'
    )
    convert_parser.add_argument('input', help='Input configuration file')
    convert_parser.add_argument('--output', '-o', required=True,
                               help='Output configuration file')
    convert_parser.add_argument('--format', choices=['json', 'ini', 'yaml'],
                               default='json', help='Output format (default: json)')

def add_utility_commands(subparsers):
    """Add utility commands."""
    util_parser = subparsers.add_parser(
        'util',
        aliases=['u'],
        help='Utility commands',
        description='Miscellaneous utility commands'
    )
    
    util_subparsers = util_parser.add_subparsers(dest='util_action', help='Utility action')
    
    # Version info
    version_parser = util_subparsers.add_parser(
        'version', 
        help='Show version information',
        description='Show version and feature information'
    )
    
    # Help command
    help_parser = util_subparsers.add_parser(
        'help', 
        help='Show help',
        description='Show help information'
    )
    help_parser.add_argument('topic', nargs='?', help='Help topic (brain, demo, config, etc.)')
    
    # Quick test
    test_parser = util_subparsers.add_parser(
        'test', 
        help='Run quick tests',
        description='Run quick tests of system components'
    )
    test_parser.add_argument('--type', choices=['brain', 'bindings', 'integration'],
                            default='brain', help='Test type (default: brain)')
    
    # Demo manager
    demo_parser = util_subparsers.add_parser(
        'demo-manager', 
        help='Demo manager',
        description='Manage and run multiple demos'
    )
    demo_parser.add_argument('action', choices=['list', 'run', 'save'],
                            help='Action to perform')
    demo_parser.add_argument('--config', '-c', help='Demo configuration file')


def main():
    """Main entry point."""
    parser = create_parser()
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    try:
        if args.command in ['run', 'r']:
            handle_run_command(args)
        elif args.command in ['brain', 'b']:
            handle_brain_command(args)
        elif args.command in ['analyze', 'a']:
            handle_analyze_command(args)
        elif args.command in ['config', 'c']:
            handle_config_command(args)
        elif args.command in ['util', 'u']:
            handle_util_command(args)
        else:
            print(f"Unknown command: {args.command}", file=sys.stderr)
            return 1
            
    except Exception as e:
        print(f"Error: {e}", file=sys.stderr)
        if '--verbose' in sys.argv or '-v' in sys.argv:
            import traceback
            traceback.print_exc()
        return 1
    
    return 0
def handle_run_command(args):
    """Handle run demo commands."""
    if PYNLM_AVAILABLE:
        if args.demo == 'phase3':
            run_phase3_demo(args)
        elif args.demo == 'phase4':
            run_phase4_demo(args)
        elif args.demo in ['phase6', 'integration']:
            run_phase6_demo(args)
        elif args.demo == 'custom':
            run_custom_simulation(args)
        elif args.demo == 'research':
            run_research_mode(args)
        else:
            raise ValueError(f"Unknown demo type: {args.demo}")
    else:
        print("Error: pynlm module not available. Cannot run demos.", file=sys.stderr)
        return 1
def run_phase3_demo(args):
    """Run Phase 3 demo."""
    print(f"Running Phase 3 demo with {args.episodes} episodes, {args.steps} steps each")
    print(f"Configuration: {args.neurons} neurons, {args.regions} regions")
    
    if PYNLM_AVAILABLE:
        # Create config
        config = pynlm.createDefaultConfig()
        config.loadFromArgs(['--neuron-count', str(args.neurons),
                            '--region-count', str(args.regions),
                            '--random-seed', '42'])
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Create world and agent
        world = pynlm.createSimpleWorld()
        world.configure(20, 20, 16, 16)
        agent = pynlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Run simulation
        for episode in range(args.episodes):
            print(f"Episode {episode + 1}/{args.episodes}")
            world.reset()
            agent.reset()
            brain.reset()
            
            for step in range(args.steps):
                percept = world.getSensoryPercept()
                agent.processSensoryInput(percept)
                brain.step(step)
                action = agent.decodeMotorCommand()
                world.applyMotorCommand(action, world.getSimulationTime())
                world.update(0.01)
        
        print("Phase 3 demo completed successfully!")
        
        if args.output:
            brain.save(args.output)
            print(f"Brain saved to {args.output}")
    else:
        print("Error: pynlm module not available", file=sys.stderr)
        return 1
def run_phase4_demo(args):
    """Run Phase 4 demo."""
    print(f"Running Phase 4 demo with curiosity={args.curiosity}, novelty={args.novelty}")
    # Implementation similar to Phase 3 but with advanced features
    print("Phase 4 demo would run here...")
def run_phase6_demo(args):
    """Run Phase 6 demo."""
    neurons = 2000 if not args.quick else 500
    print(f"Running Phase 6 integration demo with {neurons} neurons")
    
    if PYNLM_AVAILABLE:
        config = pynlm.createDefaultConfig()
        config.loadFromArgs(['--neuron-count', str(neurons),
                            '--region-count', '2'])
        
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a quick test
        for step in range(100):
            brain.step(step)
        
        print("Phase 6 demo completed successfully!")
    else:
        print("Error: pynlm module not available", file=sys.stderr)
        return 1
def run_custom_simulation(args):
    """Run custom simulation."""
    print("Running custom simulation...")
    print(f"Steps: {args.steps}, Neurons: {args.neurons}, Episodes: {args.episodes}")
    # Implementation would load config file and run simulation
    print("Custom simulation would run here...")
def run_research_mode(args):
    """Run research mode."""
    print("Running research mode...")
    print(f"Steps: {args.steps}")
    print(f"Enabled features: Development={args.enable_development}, "
          f"Curiosity={args.enable_curiosity}, Novelty={args.enable_novelty}, "
          f"StructuralPlasticity={args.enable_structural_plasticity}")
    print("Research mode would run here...")
def handle_brain_command(args):
    """Handle brain management commands."""
    if args.brain_action == 'create':
        create_brain(args)
    elif args.brain_action == 'load':
        load_brain(args)
    elif args.brain_action == 'save':
        save_brain(args)
    elif args.brain_action == 'analyze':
        analyze_brain(args)
    else:
        raise ValueError(f"Unknown brain action: {args.brain_action}")
def create_brain(args):
    """Create a brain."""
    if not PYNLM_AVAILABLE:
        raise ImportError("pynlm module not available")
    
    config = pynlm.createDefaultConfig()
    config.loadFromArgs(['--neuron-count', str(args.neurons),
                        '--region-count', str(args.regions)])
    
    brain = pynlm.createBrain(config)
    brain.initialize()
    brain.save(args.output)
    print(f"Brain created and saved to {args.output}")
def load_brain(args):
    """Load a brain."""
    if not PYNLM_AVAILABLE:
        raise ImportError("pynlm module not available")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.load(args.input)
    
    # Run some steps
    for step in range(args.steps):
        brain.step(step)
    
    print(f"Brain loaded from {args.input} and simulated for {args.steps} steps")
    
    if args.output:
        brain.save(args.output)
        print(f"Brain saved to {args.output}")
def save_brain(args):
    """Save brain (placeholder - would need current brain state)."""
    print(f"Saving brain to {args.output}")
    # Implementation would need current brain state
    print("Note: Would need to save current brain state")
def analyze_brain(args):
    """Analyze brain."""
    if not PYNLM_AVAILABLE:
        raise ImportError("pynlm module not available")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.load(args.input)
    
    # Simple analysis
    diagnostics = pynlm.run_diagnostic(brain)
    
    if args.format == 'json':
        with open(args.output, 'w') as f:
            json.dump(diagnostics, f, indent=2)
    elif args.format == 'text':
        with open(args.output, 'w') as f:
            for key, value in diagnostics.items():
                f.write(f"{key}: {value}\n")
    
    print(f"Analysis completed and saved to {args.output}")
def handle_analyze_command(args):
    """Handle analysis commands."""
    if args.analysis_type == 'metrics':
        analyze_metrics(args)
    elif args.analysis_type == 'curve':
        analyze_curve(args)
    else:
        raise ValueError(f"Unknown analysis type: {args.analysis_type}")
def analyze_metrics(args):
    """Analyze brain metrics."""
    if not PYNLM_AVAILABLE:
        raise ImportError("pynlm module not available")
    
    brain = pynlm.createBrain(pynlm.createDefaultConfig())
    brain.load(args.input)
    
    diagnostics = pynlm.run_diagnostic(brain)
    
    with open(args.output, 'w') as f:
        json.dump(diagnostics, f, indent=2)
    
    print(f"Metrics analysis completed and saved to {args.output}")
if __name__ == '__main__':
    sys.exit(main())