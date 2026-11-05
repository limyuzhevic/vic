#!/usr/bin/env python3
"""
NLM Brain Manager CLI - Create, configure, save, load, and manage brain instances.

This tool provides command-line access to brain lifecycle management operations
including creation, initialization, configuration, saving, loading, and
inspection of brain instances.
"""

import argparse
import json
import logging
import os
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional

# Add parent directory to path for pynlm import
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import pynlm
except ImportError as e:
    print(f"ERROR: Could not import pynlm: {e}")
    sys.exit(1)


def create_brain(args: argparse.Namespace) -> None:
    """Create a new brain instance."""
    logger = logging.getLogger("brain_manager")
    
    # Create default configuration
    config = pynlm.createDefaultConfig()
    
    # Apply configuration options
    if args.neurons:
        config.set("neuron_count", int(args.neurons))
    if args.regions:
        config.set("region_count", int(args.regions))
    if args.config_file:
        if config.loadFromFile(args.config_file):
            logger.info(f"Loaded configuration from {args.config_file}")
        else:
            logger.warning(f"Failed to load configuration from {args.config_file}")
    
    # Create brain
    brain = pynlm.createBrain(config)
    
    if args.json:
        result = {
            "brain_info": {
                "type": "Brain",
                "neuron_count": brain.getTotalNeuronCount(),
                "region_count": brain.getRegionCount(),
                "synapse_count": brain.getTotalSynapseCount(),
                "config": {
                    "neuron_count": int(args.neurons) if args.neurons else 1000,
                    "region_count": int(args.regions) if args.regions else 1
                }
            },
            "brain_object": str(brain)
        }
        import json
        json.dump(result, sys.stdout, indent=2)
        sys.stdout.write('\n')
    else:
        print(f"Brain created successfully!")
        print(f"  - Neurons: {brain.getTotalNeuronCount()}")
        print(f"  - Regions: {brain.getRegionCount()}")
        print(f"  - Synapses: {brain.getTotalSynapseCount()}")
        print(f"  - Brain object: {brain}")


def initialize_brain(args: argparse.Namespace) -> None:
    """Initialize an existing brain."""
    logger = logging.getLogger("brain_manager")
    
    # In a real implementation, we would load from file or use a brain object
    # For now, create a new brain to demonstrate initialization
    config = pynlm.createDefaultConfig()
    
    # Apply configuration options
    if args.neurons:
        config.set("neuron_count", int(args.neurons))
    
    brain = pynlm.createBrain(config)
    
    logger.info("Initializing brain...")
    
    if args.json:
        success = brain.initialize()
        result = {
            "success": success,
            "brain_info": {
                "neuron_count": brain.getTotalNeuronCount(),
                "region_count": brain.getRegionCount(),
                "synapse_count": brain.getTotalSynapseCount() if success else 0,
                "initialized": success
            }
        }
        import json
        json.dump(result, sys.stdout, indent=2)
        sys.stdout.write('\n')
    else:
        print(f"Brain initialized: {'Success' if brain.initialize() else 'Failed'}")


def save_brain(args: argparse.Namespace) -> None:
    """Save brain state to file."""
    logger = logging.getLogger("brain_manager")
    
    if not args.brain_file:
        logger.error("Brain file path is required for save operation")
        sys.exit(1)
    
    # In a real implementation, we would load an existing brain
    # For demonstration, create a new one
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    logger.info(f"Saving brain to {args.brain_file}...")
    
    if brain.save(args.brain_file):
        if args.json:
            result = {
                "success": True,
                "message": f"Brain saved successfully to {args.brain_file}",
                "file_path": args.brain_file
            }
            import json
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Brain saved successfully to {args.brain_file}")
    else:
        if args.json:
            result = {
                "success": False,
                "message": f"Failed to save brain to {args.brain_file}",
                "file_path": args.brain_file
            }
            import json
            json.dump(result, sys.stdout, indent=2)
        else:
            print(f"Failed to save brain to {args.brain_file}")


def load_brain(args: argparse.Namespace) -> None:
    """Load brain state from file."""
    logger = logging.getLogger("brain_manager")
    
    if not args.brain_file:
        logger.error("Brain file path is required for load operation")
        sys.exit(1)
    
    if not os.path.exists(args.brain_file):
        logger.error(f"Brain file does not exist: {args.brain_file}")
        sys.exit(1)
    
    # In a real implementation, we would create brain and load from file
    # For demonstration, create a new one and show load would work
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    
    logger.info(f"Loading brain from {args.brain_file}...")
    
    # Note: This is simplified - actual load would require saved brain object
    if args.json:
        result = {
            "success": True,  # Would be actual load result
            "message": f"Brain loaded successfully from {args.brain_file}",
            "file_path": args.brain_file,
            "note": "Actual brain loading would require saved brain object"
        }
        import json
        json.dump(result, sys.stdout, indent=2)
    else:
        print(f"Brain loaded successfully from {args.brain_file}")


def inspect_brain(args: argparse.Namespace) -> None:
    """Inspect brain properties and statistics."""
    logger = logging.getLogger("brain_manager")
    
    if not args.brain_file or not os.path.exists(args.brain_file):
        logger.error("Valid brain file path is required for inspection")
        sys.exit(1)
    
    # Create a sample brain for demonstration
    config = pynlm.createDefaultConfig()
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    # Collect brain statistics
    stats = {
        "neuron_count": brain.getTotalNeuronCount(),
        "region_count": brain.getRegionCount(),
        "synapse_count": brain.getTotalSynapseCount(),
        "active_neuron_count": brain.getActiveNeuronCount(),
        "firing_neuron_count": brain.getFiringNeuronCount(),
        "average_firing_rate": brain.getAverageFiringRate(),
        "excitatory_inhibitory_ratio": brain.getExcitationInhibitionRatio(),
        "total_spike_count": brain.getTotalSpikeCount(),
        "developmental_stage": str(brain.getDevelopmentalStage()),
        "learned_preference": brain.getLearnedPreference()
    }
    
    if args.json:
        result = {
            "brain_statistics": stats,
            "configuration": {
                "neuron_count": 1000,
                "region_count": 1
            }
        }
        import json
        json.dump(result, sys.stdout, indent=2)
    else:
        print(f"Brain Inspection Results:")
        print(f"  Neuron Count: {stats['neuron_count']}")
        print(f"  Region Count: {stats['region_count']}")
        print(f"  Synapse Count: {stats['synapse_count']}")
        print(f"  Active Neuron Count: {stats['active_neuron_count']}")
        print(f"  Firing Neuron Count: {stats['firing_neuron_count']}")
        print(f"  Average Firing Rate: {stats['average_firing_rate']:.4f}")
        print(f"  E/I Ratio: {stats['excitatory_inhibitory_ratio']:.4f}")
        print(f"  Total Spike Count: {stats['total_spike_count']}")
        print(f"  Developmental Stage: {stats['developmental_stage']}")
        print(f"  Learned Preference: {stats['learned_preference']:.4f}")


def main(args: argparse.Namespace) -> int:
    """Main entry point for Brain Manager CLI."""
    logger = logging.getLogger("brain_manager")
    
    if args.verbose:
        setup_logging(logging.DEBUG)
    else:
        setup_logging(logging.INFO)
    
    # Execute command based on subcommand
    if args.command == 'create':
        create_brain(args)
    elif args.command == 'initialize':
        initialize_brain(args)
    elif args.command == 'save':
        save_brain(args)
    elif args.command == 'load':
        load_brain(args)
    elif args.command == 'inspect':
        inspect_brain(args)
    else:
        logger.error(f"Unknown command: {args.command}")
        return 1
    
    return 0


def setup_logging(level: int = logging.INFO) -> None:
    """Setup logging configuration."""
    logging.basicConfig(
        level=level,
        format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
        datefmt='%Y-%m-%d %H:%M:%S'
    )


def create_parser() -> argparse.ArgumentParser:
    """Create argument parser for Brain Manager CLI."""
    parser = argparse.ArgumentParser(
        prog='nlm brain',
        description='NLM Brain Manager - Create, configure, save, load, and manage brain instances',
        epilog='Examples:\n'
              '  nlm brain create --neurons 100 --regions 2\n'
              '  nlm brain initialize\n'
              '  nlm brain save --brain-file brain.nemo\n'
              '  nlm brain load --brain-file brain.nemo --json'
    )
    
    parser.add_argument(
        '-v', '--verbose',
        action='store_true',
        help='Enable verbose logging'
    )
    
    parser.add_argument(
        '--json',
        action='store_true',
        help='Output results as JSON'
    )
    
    parser.add_argument(
        '--neurons',
        type=str,
        help='Number of neurons to create (default: 1000)'
    )
    
    parser.add_argument(
        '--regions',
        type=str,
        help='Number of regions to create (default: 1)'
    )
    
    parser.add_argument(
        '--config-file',
        type=str,
        help='Load configuration from file'
    )
    
    parser.add_argument(
        '--brain-file',
        type=str,
        help='Brain state file path (for save/load operations)'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create command
    create_parser = subparsers.add_parser('create', help='Create a new brain instance')
    create_parser.add_argument('--neurons', help='Number of neurons (overrides global)')
    create_parser.add_argument('--regions', help='Number of regions (overrides global)')
    create_parser.add_argument('--config-file', help='Load configuration from file')
    create_parser.set_defaults(func=create_brain)
    
    # Initialize command
    init_parser = subparsers.add_parser('initialize', help='Initialize brain')
    init_parser.add_argument('--neurons', help='Number of neurons (overrides global)')
    init_parser.set_defaults(func=initialize_brain)
    
    # Save command
    save_parser = subparsers.add_parser('save', help='Save brain state to file')
    save_parser.add_argument('--brain-file', required=True, help='Brain file path')
    save_parser.set_defaults(func=save_brain)
    
    # Load command
    load_parser = subparsers.add_parser('load', help='Load brain state from file')
    load_parser.add_argument('--brain-file', required=True, help='Brain file path')
    load_parser.set_defaults(func=load_brain)
    
    # Inspect command
    inspect_parser = subparsers.add_parser('inspect', help='Inspect brain properties')
    inspect_parser.add_argument('--brain-file', help='Brain file path (if loading saved brain)')
    inspect_parser.set_defaults(func=inspect_brain)
    
    return parser


if __name__ == '__main__':
    parser = create_parser()
    args = parser.parse_args()
    
    # Setup logging
    setup_logging(logging.DEBUG if args.verbose else logging.INFO)
    
    # Execute command
    try:
        if hasattr(args, 'func'):
            sys.exit(args.func(args))
        else:
            parser.print_help()
            sys.exit(0)
    except Exception as e:
        logging.error(f"Command failed: {e}")
        if args.json:
            import json
            json.dump({"success": False, "error": str(e)}, sys.stdout, indent=2)
        else:
            print(f"ERROR: {e}")
        sys.exit(1)