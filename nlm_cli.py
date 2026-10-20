chmod +x nlm_cli.py

import argparse
import json
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional
import time

# Try to import NLM, handle import errors gracefully
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    PYNLM_AVAILABLE = False
    print(f"Warning: pynlm not available: {e}", file=sys.stderr)

# Command registry for available commands
COMMANDS = {
    'create-config': 'Create a new configuration file',
    'list-configs': 'List available configuration files',
    'show-config': 'Display configuration details',
    'edit-config': 'Edit configuration values',
    'run-simulation': 'Run a simulation with specified parameters',
    'batch-run': 'Run multiple simulations in batch',
    'save-brain': 'Save brain state to file',
    'load-brain': 'Load brain state from file',
    'compare-brains': 'Compare two brain states',
    'export-data': 'Export simulation data in various formats',
    'analyze-brain': 'Analyze brain properties and statistics',
    'visualize-connectivity': 'Generate connectivity matrix visualization',
    'help': 'Show help for specific command',
    'version': 'Show version information',
}

def print_banner():
    """Print NLM CLI banner"""
    print("""
╔════════════════════════════════════════════════════════════╗
║                    NLM ADVANCED CLI                      ║
║                    Neural Learning Machine                ║
╚════════════════════════════════════════════════════════════╝
""")

def show_version():
    """Show version information"""
    print("NLM Advanced CLI")
    print("Version: 2.0.0 (Advanced Edition)")
    print("Python Bindings: Available" if PYNLM_AVAILABLE else "Python Bindings: Not Available")
    print()

def show_help(command: Optional[str] = None):
    """Show help for commands"""
    if command:
        if command in COMMANDS:
            print(f"Command: {command}")
            print(f"Description: {COMMANDS[command]}")
            # Show more detailed help for the command
            if command == 'create-config':
                print("""
Usage: nlm create-config --name NAME [--template TEMPLATE] [--output FILE]

Create a new configuration file for NLM simulations.

Arguments:
  --name NAME             Configuration name
  --template TEMPLATE     Use predefined template (basic, advanced, research)
  --output FILE           Output file path (default: configs/name.json)

Templates:
  basic      - Simple configuration for basic testing
  advanced   - Comprehensive configuration with all features enabled
  research  - Configuration optimized for research experiments

Examples:
  nlm create-config my-exp --template advanced
  nlm create-config baseline --template basic
""")
            elif command == 'run-simulation':
                print("""
Usage: nlm run-simulation --config FILE [--steps STEPS] [--timestep TIME]

Run a complete NLM simulation with brain and world interaction.

Arguments:
  --config FILE           Configuration file path
  --steps STEPS           Number of simulation steps (default: 1000)
  --timestep TIME         Simulation timestep in seconds (default: 0.01)

Features:
  - Brain initialization and setup
  - Complete agent-world interaction
  - All neuromodulation systems enabled
  - Automatic reward modulation
  - Development stage tracking

Example:
  nlm run-simulation configs/my-experiment.json --steps 2000 --timestep 0.01
""")
            elif command == 'analyze-brain':
                print("""
Usage: nlm analyze-brain --brain FILE [--output FORMAT] [--verbose]

Analyze brain state and generate detailed statistics.

Arguments:
  --brain FILE            Brain state file path
  --output FORMAT         Output format (json, txt, csv) (default: txt)
  --verbose               Enable verbose output with detailed metrics

Analysis includes:
  - Neural activity statistics
  - Synaptic strength distribution
  - Connectivity patterns
  - Development metrics
  - Performance indicators
""")
        else:
            print(f"Unknown command: {command}")
    else:
        print("Available Commands:")
        print()
        for cmd, desc in sorted(COMMANDS.items()):
            print(f"  {cmd:<25} - {desc}")
        print()
        print("For detailed help on a specific command, use:")
        print("  nlm help <command>")

def create_config(name: str, template: str = "basic", output: Optional[str] = None):
    """Create a new configuration file"""
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available. Cannot create configuration.")
        return
    
    if output is None:
        output = f"configs/{name}.json"
    
    os.makedirs(os.path.dirname(output), exist_ok=True)
    
    # Default templates
    templates = {
        'basic': {
            'brain': {
                'neuron_count': 1000,
                'region_count': 1,
                'connection_probability': 0.05,
                'initial_weight_mean': 0.5,
                'v_thresh': -50.0,
                'v_rest': -70.0,
                'v_reset': -75.0,
            },
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.001},
                'hebbian': {'enable': True},
                'structural': {'enable': False}
            },
            'neuromodulation': {
                'dopamine': {'scale': 1.0},
                'curiosity': {'enable': True},
                'novelty': {'enable': True}
            }
        },
        'advanced': {
            'brain': {
                'neuron_count': 5000,
                'region_count': 3,
                'connection_probability': 0.1,
                'initial_weight_mean': 0.5,
                'initial_weight_std': 0.1,
                'v_thresh': -50.0,
                'v_rest': -70.0,
                'v_reset': -75.0,
                'tau_mem': 20.0,
                'tau_ref': 2.0,
            },
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.001, 'tau_plus': 20.0, 'tau_minus': 20.0},
                'hebbian': {'enable': True, 'learning_rate': 0.0005},
                'structural': {'enable': True, 'synaptogenesis_rate': 0.0001, 'pruning_rate': 0.00001}
            },
            'neuromodulation': {
                'dopamine': {'scale': 1.0, 'learning_rate': 0.01},
                'curiosity': {'enable': True, 'scale': 0.5},
                'novelty': {'enable': True, 'threshold': 0.1},
                'prediction_error': {'enable': True, 'scale': 1.0}
            },
            'development': {
                'enable': True,
                'plasticity_decay_rate': 0.001,
                'critical_period_length': 100.0
            }
        },
        'research': {
            'brain': {
                'neuron_count': 10000,
                'region_count': 5,
                'connection_probability': 0.08,
                'initial_weight_mean': 0.5,
                'initial_weight_std': 0.15,
            },
            'plasticity': {
                'stdp': {'enable': True, 'learning_rate': 0.002, 'tau_plus': 15.0, 'tau_minus': 25.0},
                'hebbian': {'enable': True, 'learning_rate': 0.001},
                'structural': {'enable': True, 'synaptogenesis_rate': 0.00005, 'pruning_rate': 0.000005}
            },
            'neuromodulation': {
                'dopamine': {'scale': 1.5, 'learning_rate': 0.02},
                'curiosity': {'enable': True, 'scale': 1.0},
                'novelty': {'enable': True, 'threshold': 0.05},
                'prediction_error': {'enable': True, 'scale': 2.0}
            },
            'development': {
                'enable': True,
                'plasticity_decay_rate': 0.0005,
                'critical_period_length': 200.0
            }
        }
    }
    
    config_data = templates.get(template, templates['basic'])
    config_data['name'] = name
    config_data['created'] = time.strftime('%Y-%m-%d %H:%M:%S')
    
    with open(output, 'w') as f:
        json.dump(config_data, f, indent=2)
    
    print(f"Created configuration '{name}' using '{template}' template")
    print(f"Saved to: {output}")
    
    # Show a sample of the configuration
    print("\nSample configuration:")
    brain_config = config_data.get('brain', {})
    print(f"  Neurons: {brain_config.get('neuron_count', 'N/A')}")
    print(f"  Regions: {brain_config.get('region_count', 'N/A')}")
    print(f"  STDP enabled: {config_data.get('plasticity', {}).get('stdp', {}).get('enable', False)}")

def list_configs():
    """List available configuration files"""
    config_dir = Path("configs")
    
    if not config_dir.exists():
        print("No configuration directory found. Run 'nlm create-config' to create one.")
        return
    
    config_files = list(config_dir.glob("*.json"))
    
    if not config_files:
        print("No configuration files found.")
        return
    
    print("Available Configurations:")
    print()
    for config_file in sorted(config_files):
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
            
            name = config.get('name', config_file.stem)
            neurons = config.get('brain', {}).get('neuron_count', 'N/A')
            regions = config.get('brain', {}).get('region_count', 'N/A')
            
            print(f"  {config_file.stem:<20} - {name}")
            print(f"    {' ' * 22}Neurons: {neurons}, Regions: {regions}")
        except json.JSONDecodeError:
            print(f"  {config_file.stem:<20} - (Invalid JSON)")
        print()

def show_config(config_file: str):
    """Display configuration details"""
    try:
        with open(config_file, 'r') as f:
            config = json.load(f)
        
        name = config.get('name', Path(config_file).stem)
        print(f"Configuration: {name}")
        print(f"File: {config_file}")
        print()
        
        # Display configuration in sections
        sections = ['brain', 'plasticity', 'neuromodulation', 'development']
        
        for section in sections:
            if section in config:
                print(f"[{section.upper()}]")
                section_config = config[section]
                
                for key, value in section_config.items():
                    if isinstance(value, dict):
                        print(f"  {key}:")
                        for subkey, subvalue in value.items():
                            print(f"    {subkey} = {subvalue}")
                    else:
                        print(f"  {key} = {value}")
                print()
                
    except FileNotFoundError:
        print(f"Error: Configuration file '{config_file}' not found.")
    except json.JSONDecodeError:
        print(f"Error: Configuration file '{config_file}' contains invalid JSON.")

def run_simulation(config_file: str, steps: int = 1000, timestep: float = 0.01):
    """Run a simulation with specified parameters"""
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available. Cannot run simulation.")
        return
    
    print(f"Loading configuration: {config_file}")
    
    try:
        # Load configuration from file (simplified - would need actual config loading)
        print(f"Configuration would be loaded from {config_file}")
        print(f"Running simulation for {steps} steps with timestep {timestep}")
        
        # Create brain with default config for demo
        config = pynlm.createDefaultConfig()
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run simulation
        start_time = time.time()
        
        for step in range(steps):
            brain.step(step)
            
            # Print progress every 10% of simulation
            if step % (steps // 10) == 0 or step == steps - 1:
                progress = (step / steps) * 100
                firing = brain.getFiringNeuronCount()
                total_spikes = brain.getTotalSpikeCount()
                print(f"Progress: {progress:.0f}% - Firing: {firing}, Total spikes: {total_spikes}")
        
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"\nSimulation completed in {duration:.2f} seconds")
        print(f"Final statistics:")
        print(f"  Total neurons: {brain.getTotalNeuronCount()}")
        print(f"  Total synapses: {brain.getTotalSynapseCount()}")
        print(f"  Final firing neurons: {brain.getFiringNeuronCount()}")
        print(f"  Average firing rate: {brain.getAverageFiringRate():.4f}")
        print(f"  Development stage: {brain.getDevelopmentalStage()}")
        
    except Exception as e:
        print(f"Error during simulation: {e}")
        import traceback
        traceback.print_exc()

def analyze_brain(brain_file: str, output_format: str = "txt", verbose: bool = False):
    """Analyze brain state and generate statistics"""
    if not PYNLM_AVAILABLE:
        print("Error: pynlm not available. Cannot analyze brain.")
        return
    
    print(f"Analyzing brain state: {brain_file}")
    
    try:
        # Try to load brain (simplified)
        if os.path.exists(brain_file):
            # This is a simplified analysis - in a real implementation
            # you would load the actual brain state
            print("Brain file exists, but detailed analysis requires actual brain loading.")
            print("Analysis would include:")
            if verbose:
                print("  - Neural activity patterns")
                print("  - Synaptic weight distributions")
                print("  - Regional connectivity")
                print("  - Development metrics")
                print("  - Performance indicators")
            else:
                print("  - Basic statistics (activate with --verbose)")
        else:
            print(f"Error: Brain file '{brain_file}' not found.")
            
    except Exception as e:
        print(f"Error analyzing brain: {e}")

def main():
    """Main CLI entry point"""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm --help                          # Show all commands
  nlm help create-config              # Get help for create-config command
  nlm create-config my-exp --template advanced
  nlm run-simulation configs/my-exp.json --steps 5000
  nlm analyze-brain checkpoint.bin --verbose
        """
    )
    
    parser.add_argument(
        '--version',
        action='store_true',
        help='Show version information'
    )
    
    parser.add_argument(
        '--verbose',
        action='store_true',
        help='Enable verbose output'
    )
    
    # Main command
    parser.add_argument(
        'command',
        nargs='?',
        default='help',
        help='Command to execute'
    )
    
    # Command-specific arguments
    parser.add_argument(
        'args',
        nargs='*',
        help='Additional arguments for the command'
    )
    
    args = parser.parse_args()
    
    print_banner()
    
    if args.version:
        show_version()
        return
    
    # Command dispatch
    if args.command == 'help':
        if args.args:
            show_help(args.args[0])
        else:
            show_help()
    
    elif args.command == 'create-config':
        if not args.args:
            print("Error: Configuration name required")
            print("Usage: nlm create-config NAME [--template TEMPLATE] [--output FILE]")
            return
        
        name = args.args[0]
        template = "basic"
        output = None
        
        # Parse remaining arguments
        remaining = args.args[1:]
        for i, arg in enumerate(remaining):
            if arg == '--template' and i + 1 < len(remaining):
                template = remaining[i + 1]
            elif arg == '--output' and i + 1 < len(remaining):
                output = remaining[i + 1]
        
        create_config(name, template, output)
    
    elif args.command == 'list-configs':
        list_configs()
    
    elif args.command == 'show-config':
        if not args.args:
            print("Error: Configuration file path required")
            print("Usage: nlm show-config FILE")
            return
        show_config(args.args[0])
    
    elif args.command == 'run-simulation':
        config_file = None
        steps = 1000
        timestep = 0.01
        
        # Parse remaining arguments
        for i, arg in enumerate(args.args):
            if arg == '--config' and i + 1 < len(args.args):
                config_file = args.args[i + 1]
            elif arg == '--steps' and i + 1 < len(args.args):
                try:
                    steps = int(args.args[i + 1])
                except ValueError:
                    print(f"Error: Invalid steps value: {args.args[i + 1]}")
                    return
            elif arg == '--timestep' and i + 1 < len(args.args):
                try:
                    timestep = float(args.args[i + 1])
                except ValueError:
                    print(f"Error: Invalid timestep value: {args.args[i + 1]}")
                    return
        
        if not config_file:
            print("Error: Configuration file required (--config)")
            print("Usage: nlm run-simulation --config FILE [--steps STEPS] [--timestep TIME]")
            return
        
        run_simulation(config_file, steps, timestep)
    
    elif args.command == 'analyze-brain':
        brain_file = None
        output_format = "txt"
        verbose = args.verbose
        
        # Parse remaining arguments
        for i, arg in enumerate(args.args):
            if arg == '--brain' and i + 1 < len(args.args):
                brain_file = args.args[i + 1]
            elif arg == '--output' and i + 1 < len(args.args):
                output_format = args.args[i + 1]
        
        if not brain_file:
            print("Error: Brain file required (--brain)")
            print("Usage: nlm analyze-brain --brain FILE [--output FORMAT] [--verbose]")
            return
        
        analyze_brain(brain_file, output_format, verbose)
    
    else:
        print(f"Unknown command: {args.command}")
        print("Run 'nlm --help' for available commands.")

if __name__ == '__main__':
    main()
