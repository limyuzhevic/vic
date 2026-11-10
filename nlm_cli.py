#!/usr/bin/env python3
"""
NLM Command Line Interface

A comprehensive CLI for the NLM (Neural Learning Machine) framework.

Usage:
    nlm-cli [OPTIONS] COMMAND [ARGS]...

Commands:
    run            Run a simulation or experiment
    demo           Run a specific demo program
    explore        Explore and test individual components
    monitor        Monitor an ongoing simulation
    config         Configuration management
    help           Show help information
    version        Show version information

Options:
    --help, -h     Show this help message
    --verbose, -v  Enable verbose output
    --version      Show version information
    --debug        Enable debug output

Environment:
    NLM_HOME       Path to NLM installation directory
    NLM_LOG_LEVEL  Log level (trace, debug, info, warn, error)
    NLM_CONFIG     Path to configuration file
"""

import sys
import os
import argparse
from pathlib import Path
from typing import Dict, Any, Optional

# Add NLM modules to path
nlm_home = os.environ.get('NLM_HOME', os.path.dirname(os.path.abspath(__file__)))
if nlm_home not in sys.path:
    sys.path.insert(0, nlm_home)

def print_banner():
    """Print NLM CLI banner"""
    banner = """
    ╔═══════════════════════════════════════════════════════════════╗
    ║                                                               ║
    ║     NLM — 熙然                                                ║
    ║     Neural Learning Machine                                   ║
    ║                                                               ║
    ║     An experimental artificial developmental brain.            ║
    ║                                                               ║
    ╚═══════════════════════════════════════════════════════════════╝
    """
    print(banner)

def handle_run(args: Dict[str, Any]) -> int:
    """Handle run command"""
    print(f"Running NLM experiment: {args['experiment']}")
    
    if args['experiment'] == 'learning':
        print("Learning experiment configuration:")
        print(f"  Steps: {args['steps']}")
        print(f"  Neuron count: {args['neuron_count']}")
        print(f"  Region count: {args['region_count']}")
        
        # TODO: Actually run the learning experiment
        print("To be implemented: Run learning experiment")
        
    elif args['experiment'] == 'plasticity':
        print("Plasticity experiment configuration:")
        print(f"  Steps: {args['steps']}")
        print(f"  Neuron count: {args['neuron_count']}")
        print(f"  STDP enabled: {args['stdp']}")
        
        # TODO: Actually run the plasticity experiment
        print("To be implemented: Run plasticity experiment")
        
    elif args['experiment'] == 'connectivity':
        print("Connectivity experiment configuration:")
        print(f"  Steps: {args['steps']}")
        print(f"  Neuron count: {args['neuron_count']}")
        
        # TODO: Actually run the connectivity experiment
        print("To be implemented: Run connectivity experiment")
        
    elif args['experiment'] == 'full':
        print("Full integration experiment configuration:")
        print(f"  Duration: {args['duration']}s")
        print(f"  Neuron count: {args['neuron_count']}")
        print(f"  Region count: {args['region_count']}")
        
        # TODO: Actually run the full integration experiment
        print("To be implemented: Run full integration experiment")
        
    else:
        print(f"Unknown experiment: {args['experiment']}")
        return 1
    
    return 0

def handle_demo(args: Dict[str, Any]) -> int:
    """Handle demo command"""
    print(f"Running demo: {args['demo']}")
    
    if args['demo'] == 'phase2':
        print("Phase 2: Real Neural Computation Demo")
        print("This demo shows:")
        print("  - LIF neuron dynamics")
        print("  - Spike propagation")
        print("  - STDP and Hebbian plasticity")
        print("  - Structural plasticity")
        
    elif args['demo'] == 'phase6':
        print("Phase 6: Final Integration Demo")
        print("This demo shows:")
        print("  - All systems integrated")
        print("  - Memory systems working")
        print("  - Neuromodulation effects")
        print("  - Development stages")
        
    else:
        print(f"Unknown demo: {args['demo']}")
        return 1
    
    return 0

def handle_explore(args: Dict[str, Any]) -> int:
    """Handle explore command"""
    print(f"Exploring component: {args['component']}")
    
    if args['component'] == 'neurons':
        print("Neuron exploration configuration:")
        print(f"  Neuron count: {args['neuron_count']}")
        print(f"  Neuron type: {args['neuron_type']}")
        
    elif args['component'] == 'synapses':
        print("Synapse exploration configuration:")
        print(f"  Synapse count: {args['synapse_count']}")
        print(f"  Plasticity: {args['plasticity']}")
        
    elif args['component'] == 'memory':
        print("Memory exploration configuration:")
        print(f"  Memory type: {args['memory_type']}")
        print(f"  Capacity: {args['capacity']}")
        
    else:
        print(f"Unknown component: {args['component']}")
        return 1
    
    return 0

def handle_config(args: Dict[str, Any]) -> int:
    """Handle config command"""
    if args['action'] == 'show':
        print("Current configuration:")
        print("  neuron_count = 500")
        print("  region_count = 1")
        print("  connection_probability = 0.15")
        
    elif args['action'] == 'set':
        print(f"Setting {args['key']} = {args['value']}")
        
    elif args['action'] == 'get':
        print(f"Getting {args['key']} = ?")
        
    elif args['action'] == 'load':
        print(f"Loading configuration from: {args['file']}")
        
    elif args['action'] == 'save':
        print(f"Saving configuration to: {args['file']}")
        
    else:
        print(f"Unknown config action: {args['action']}")
        return 1
    
    return 0

def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM — Neural Learning Machine Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm-cli demo phase2
  nlm-cli run learning --steps 1000
  nlm-cli explore neurons --neuron-count 100
  nlm-cli config show
        """
    )
    
    # Global options
    parser.add_argument('--verbose', '-v', action='store_true',
                       help='Enable verbose output')
    parser.add_argument('--version', action='store_true',
                       help='Show version information')
    parser.add_argument('--debug', action='store_true',
                       help='Enable debug output')
    
    # Create subparsers for commands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run a simulation or experiment')
    run_parser.add_argument('experiment', choices=['learning', 'plasticity', 'connectivity', 'full'],
                           help='Experiment type')
    run_parser.add_argument('--steps', type=int, default=1000,
                           help='Number språk:en.completion'
    
    # Demo command
    demo_parser = subparsers.add_parser('demo', help='Run a specific demo program')
    demo_parser.add_argument('demo', choices=['phase2', 'phase6', 'quick', 'integration'],
                            help='Demo program')
    
    # Explore command
    explore_parser = subparsers.add_parser('explore', help='Explore and test individual components')
    explore_parser.add_argument('component', choices=['neurons', 'synapses', 'memory', 'plasticity', 'cognition'],
                               help='Component to explore')
    
    # Config command
    config_parser = subparsers.add_parser('config', help='Configuration management')
    config_parser.add_argument('action', choices=['show', 'set', 'get', 'load', 'save'],
                              help='Configuration action')
    config_parser.add_argument('key', nargs='?', help='Configuration key')
    config_parser.add_argument('value', nargs='?', help='Configuration value')
    config_parser.add_argument('--file', '-f', help='Configuration file')
    
    # Parse arguments
    args = parser.parse_args()
    
    # Handle version
    if args.version:
        print_banner()
        print("NLM CLI v0.1.0")
        return 0
    
    # Handle help
    if not args.command:
        parser.print_help()
        return 0
    
    # Handle commands
    try:
        if args.command == 'run':
            result = handle_run(vars(args))
            return result
        elif args.command == 'demo':
            result = handle_demo(vars(args))
            return result
        elif args.command == 'explore':
            result = handle_explore(vars(args))
            return result
        elif args.command == 'config':
            result = handle_config(vars(args))
            return result
        else:
            print(f"Unknown command: {args.command}")
            return 1
    except KeyboardInterrupt:
        print("\nInterrupted by user")
        return 130
    except Exception as e:
        print(f"Error: {e}")
        if args.debug:
            import traceback
            traceback.print_exc()
        return 1

if __name__ == '__main__':
    sys.exit(main())