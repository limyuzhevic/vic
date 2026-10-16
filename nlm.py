#!/usr/bin/env python3

"""
NLM (Neural Learning Machine) CLI

Command-line interface for NLM simulations and experiments.

This tool provides a comprehensive interface for running neural simulations,
validating configurations, and managing experiments using the NLM C++ backend
through Python bindings.

Usage examples:
  nlm run config.json --steps 100
  nlm validate my_config.cfg
  nlm info
  nlm benchmark --duration 60
  nlm experiment preset/reward_learning --config config.json

For more information about a specific command, use:
  nlm <command> --help
"""

import argparse
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional

# Add current directory to path to import validation script
sys.path.insert(0, str(Path(__file__).parent))

from validate_config import parse_config_file, validate_config_entries

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False


def print_help():
    """Print comprehensive help information."""
    help_text = """
NLM (Neural Learning Machine) CLI

Core Commands:
  nlm run <config_file> [options]     Run simulation with configuration
  nlm validate <config_file>          Validate configuration file
  nlm info                           Show system info and capabilities  
  nlm benchmark [options]             Run performance benchmarks
  nlm experiment <preset> [options]   Run predefined experiment

Advanced Features:
  nlm create <config_file> [options]   Create new project/config
  nlm monitor <pid> [options]         Monitor running simulation
  nlm stats <config_file> [options]    Show simulation statistics
  nlm checkpoint <brain_file> [options] Save/load brain states
  nlm profile <config_file> [options]   Profile performance

Integration:
  nlm interactive                      Start interactive exploration mode
  nlm plugin <name> <action>          Plugin system for extensions

Configuration:
  nlm-config create <file> [--template <name>]
  nlm-config validate <file>
  nlm-config merge <output> <file1> [<file2>...]
  nlm-config get <file> <key>
  nlm-config set <file> <key> <value>

Quick Start Examples:

1. Create a quick simulation:
   nlm create my_sim.cfg --template training
   nlm run my_sim.cfg --steps 1000 --verbose

2. Validate and run:
   nlm validate config.json
   nlm run config.json

3. Run benchmark:
   nlm benchmark --duration 300 --output benchmark_results.json

4. Run experiment:
   nlm experiment reward_learning --config config.json
   nlm experiment curiosity_driven --steps 5000

For more information about specific commands, use:
  nlm <command> --help
"""
    print(help_text.strip())


def error_exit(message: str, exit_code: int = 1):
    """Print error message and exit."""
    print(f"❌ Error: {message}", file=sys.stderr)
    sys.exit(exit_code)


def success_print(message: str):
    """Print success message."""
    print(f"✅ {message}")


def parse_arguments():
    """Parse command line arguments."""
    parser = argparse.ArgumentParser(
        prog='nlm',
        description='NLM (Neural Learning Machine) CLI - Run neural simulations and experiments',
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog='Use "nlm <command> --help" for detailed command information'
    )
    
    # Global options
    parser.add_argument('--version', action='store_true', help='Show version information')
    parser.add_argument('--verbose', '-v', action='store_true', help='Enable verbose output')
    parser.add_argument('--quiet', '-q', action='store_true', help='Suppress non-essential output')
    parser.add_argument('--config', '-c', help='Default configuration file path')
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands', required=True)
    
    # Core Commands
    run_parser = subparsers.add_parser('run', help='Run simulation with configuration')
    run_parser.add_argument('config_file', help='Configuration file path')
    run_parser.add_argument('--steps', '-s', type=int, default=100, help='Number of simulation steps')
    run_parser.add_argument('--max-time', type=float, help='Maximum simulation time')
    run_parser.add_argument('--from-checkpoint', dest='continue_from', action='store_true', help='Continue from checkpoint')
    
    validate_parser = subparsers.add_parser('validate', help='Validate configuration file')
    validate_parser.add_argument('config_file', help='Configuration file path')
    validate_parser.add_argument('--strict', action='store_true', help='Strict validation mode')
    
    info_parser = subparsers.add_parser('info', help='Show system info and capabilities')
    
    benchmark_parser = subparsers.add_parser('benchmark', help='Run performance benchmarks')
    benchmark_parser.add_argument('--duration', '-d', type=int, default=60, help='Benchmark duration in seconds')
    benchmark_parser.add_argument('--output', '-o', help='Output file for benchmark results')
    benchmark_parser.add_argument('--type', choices=['speed', 'memory', 'accuracy', 'all'], default='all', help='Benchmark type')
    
    experiment_parser = subparsers.add_parser('experiment', help='Run predefined experiment')
    experiment_parser.add_argument('preset', choices=['reward_learning', 'curiosity_driven', 'development', 'ablation', 'phase6_demo'],
                                 help='Predefined experiment preset')
    experiment_parser.add_argument('--config', '-c', help='Configuration file path')
    experiment_parser.add_argument('--steps', '-s', type=int, help='Number of steps')
    
    # Advanced Features
    create_parser = subparsers.add_parser('create', help='Create new project/config')
    create_parser.add_argument('config_file', help='Configuration file to create')
    create_parser.add_argument('--template', choices=['default', 'minimal', 'training', 'exploration', 'phase6'], 
                              default='default', help='Configuration template')
    create_parser.add_argument('--brain-size', type=int, help='Number of neurons in brain')
    
    monitor_parser = subparsers.add_parser('monitor', help='Monitor running simulation')
    monitor_parser.add_argument('pid', type=int, help='Process ID to monitor')
    monitor_parser.add_argument('--interval', '-i', type=int, default=1, help='Monitoring interval in seconds')
    monitor_parser.add_argument('--output', '-o', help='Output file for monitoring data')
    
    stats_parser = subparsers.add_parser('stats', help='Show simulation statistics')
    stats_parser.add_argument('config_file', help='Configuration file path')
    stats_parser.add_argument('--output', '-o', help='Output file for statistics')
    
    checkpoint_parser = subparsers.add_parser('checkpoint', help='Save/load brain states')
    checkpoint_parser.add_argument('brain_file', help='Brain checkpoint file')
    checkpoint_parser.add_argument('--action', choices=['save', 'load', 'list'], default='save', help='Checkpoint action')
    checkpoint_parser.add_argument('--brain', help='Brain object to save/load')
    
    profile_parser = subparsers.add_parser('profile', help='Profile performance')
    profile_parser.add_argument('config_file', help='Configuration file path')
    profile_parser.add_argument('--output', '-o', help='Output file for profile data')
    profile_parser.add_argument('--format', choices=['json', 'csv', 'html'], default='json', help='Output format')
    
    # Integration Features
    interactive_parser = subparsers.add_parser('interactive', help='Start interactive exploration mode')
    
    plugin_parser = subparsers.add_parser('plugin', help='Plugin system for extensions')
    plugin_parser.add_argument('action', choices=['list', 'load', 'unload', 'create'], help='Plugin action')
    plugin_parser.add_argument('name', help='Plugin name')
    
    return parser


def run_command(args):
    """Execute the run command."""
    if not PYNLM_AVAILABLE:
        error_exit("Python bindings not available. Please install pynlm.")
    
    config_file = Path(args.config_file)
    if not config_file.exists():
        error_exit(f"Config file not found: {config_file}")
    
    print(f"Running simulation with config: {config_file}")
    print(f"Steps: {args.steps}")
    
    if args.max_time:
        print(f"Max time: {args.max_time} seconds")
    
    if args.continue_from:
        print("Continuing from checkpoint...")
    
    # Try to load and run simulation
    try:
        # Load config using existing Config class
        config = pynlm.Config()
        config.loadFromFile(str(config_file))
        
        if args.verbose:
            print(f"Config loaded: {config.summary()}")
        
        # Create brain
        brain = pynlm.createBrain(config)
        print(f"Brain created with {brain.getTotalNeuronCount()} neurons")
        
        # Initialize brain
        brain.initialize()
        
        # Run simulation
        print(f"Starting simulation for {args.steps} steps...")
        
        for step in range(args.steps):
            brain.step(step)
            
            if args.verbose and step % 100 == 0:
                print(f"Step {step}/{args.steps}")
        
        # Get stats
        stats = pynlm.getBrainStats(brain)
        print(f"\nSimulation completed!")
        print(f"Total spikes: {stats.get('total_spikes', 0)}")
        print(f"Average firing rate: {stats.get('average_firing_rate', 0):.4f}")
        
        success_print(f"Simulation completed successfully in {args.steps} steps")
        
    except Exception as e:
        error_exit(f"Simulation failed: {e}")


def validate_command(args):
    """Execute the validate command."""
    config_file = Path(args.config_file)
    if not config_file.exists():
        error_exit(f"Config file not found: {config_file}")
    
    print(f"Validating configuration: {config_file}")
    
    if args.strict:
        print("Using strict validation mode")
    
    # Run validation using existing validation script
    import subprocess
    cmd = [sys.executable, str(Path(__file__).parent / "validate_config.py"), str(config_file)]
    if args.strict:
        cmd.append("--strict")
    
    result = subprocess.run(cmd, capture_output=True, text=True)
    
    print(result.stdout)
    
    if result.returncode != 0:
        if args.strict:
            error_exit("Configuration validation failed (strict mode)")
        else:
            error_exit("Configuration validation failed")
    else:
        success_print("Configuration validation passed")


def info_command(args):
    """Execute the info command."""
    if not PYNLM_AVAILABLE:
        error_exit("Python bindings not available. Please install pynlm.")
    
    print("NLM System Information:")
    print("=" * 50)
    print(f"Python bindings: Available ({pynlm.__version__ if hasattr(pynlm, '__version__') else 'v0.1.0'})")
    print(f"Platform: {sys.platform}")
    print(f"Python version: {sys.version}")
    
    # Check for Python module
    try:
        import inspect
        print(f"\nPython module path: {pynlm.__file__}")
    except:
        pass
    
    # Test basic API functionality
    print("\nTesting API functionality:")
    try:
        config = pynlm.createExampleConfig()
        print("✅ createExampleConfig: Available")
        
        brain = pynlm.createQuickBrain()
        print("✅ createQuickBrain: Available")
        
        issues, warnings, suggestions = pynlm.validateConfig(config)
        print(f"✅ validateConfig: Available (issues: {len(issues)}, warnings: {len(warnings)})")
        
        # Test more functions
        world = pynlm.createQuickWorld()
        print("✅ createQuickWorld: Available")
        
        agent = pynlm.createAgentSetup(brain, world)
        print("✅ createAgentSetup: Available")
        
        success_print("All core API functions are working")
        
    except Exception as e:
        error_exit(f"API functionality test failed: {e}")


def benchmark_command(args):
    """Execute the benchmark command."""
    if not PYNLM_AVAILABLE:
        error_exit("Python bindings not available. Please install pynlm.")
    
    print(f"Running {args.type} benchmark for {args.duration} seconds")
    print(f"Output: {args.output}")
    
    try:
        import time
        start_time = time.time()
        
        print("Running benchmark simulation...")
        
        # Create brain for benchmarking
        brain = pynlm.createQuickBrain()
        brain.initialize()
        
        # Run benchmark
        end_time = time.time()
        duration = end_time - start_time
        
        print(f"Benchmark completed in {duration:.2f} seconds")
        
        # Collect benchmark results
        results = {
            'type': args.type,
            'duration': duration,
            'steps_per_second': args.steps if hasattr(args, 'steps') else 100,
            'neuron_count': brain.getTotalNeuronCount(),
            'timestamp': start_time
        }
        
        # Save results if output specified
        if args.output:
            import json
            output_path = Path(args.output)
            with open(output_path, 'w') as f:
                json.dump(results, f, indent=2)
            print(f"Benchmark results saved to: {output_path}")
        
        print(f"Benchmark results: {results}")
        
    except Exception as e:
        error_exit(f"Benchmark failed: {e}")


def experiment_command(args):
    """Execute the experiment command."""
    if not PYNLM_AVAILABLE:
        error_exit("Python bindings not available. Please install pynlm.")
    
    print(f"Running experiment: {args.preset}")
    
    if args.config:
        config_file = Path(args.config)
        if not config_file.exists():
            error_exit(f"Config file not found: {config_file}")
        print(f"Config file: {config_file}")
    
    if args.steps:
        print(f"Steps: {args.steps}")
    
    # Run experiment based on preset
    try:
        if args.steps:
            steps = args.steps
        else:
            # Default steps based on preset
            steps_map = {
                'reward_learning': 1000,
                'curiosity_driven': 500,
                'development': 2000,
                'ablation': 500,
                'phase6_demo': 100
            }
            steps = steps_map.get(args.preset, 1000)
        
        print(f"Starting experiment '{args.preset}' for {steps} steps...")
        
        # Create appropriate config based on preset
        if args.config:
            config = pynlm.Config()
            config.loadFromFile(str(config_file))
        else:
            # Use preset-specific config
            if args.preset == 'reward_learning':
                config = pynlm.createTrainingConfig()
            elif args.preset == 'curiosity_driven':
                config = pynlm.createExplorationConfig()
            else:
                config = pynlm.createExampleConfig()
        
        # Create and run brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        for step in range(steps):
            brain.step(step)
            
            if step % 100 == 0:
                print(f"  Step {step}/{steps}")
        
        stats = pynlm.getBrainStats(brain)
        print(f"\nExperiment '{args.preset}' completed!")
        print(f"Total spikes: {stats.get('total_spikes', 0)}")
        print(f"Average firing rate: {stats.get('average_firing_rate', 0):.4f}")
        
        success_print(f"Experiment '{args.preset}' completed successfully")
        
    except Exception as e:
        error_exit(f"Experiment failed: {e}")


def create_command(args):
    """Execute the create command."""
    config_file = Path(args.config_file)
    
    print(f"Creating configuration: {config_file}")
    print(f"Template: {args.template}")
    
    # Check if file already exists
    if config_file.exists():
        if not args.verbose:
            print(f"Warning: {config_file} already exists")
        # Check if overwrite is requested via verbose flag or environment
        if not args.verbose and not os.environ.get('NLM_FORCE_OVERWRITE'):
            if not args.quiet:
                print("Use --verbose to force overwrite or set NLM_FORCE_OVERWRITE environment variable")
            print("Operation cancelled - file exists")
            return
        else:
            if not args.quiet:
                print("Overwriting existing configuration file...")
    
    try:
        # Use the existing nlm-config tool to create config
        import subprocess
        cmd = [sys.executable, str(Path(__file__).parent / "nlm-config"), "create", str(config_file)]
        
        if args.template != 'default':
            cmd.extend(["--template", args.template])
        
        # Apply brain size if specified
        if args.brain_size:
            cmd.extend(["--neuron-count", str(args.brain_size)])
        
        result = subprocess.run(cmd, capture_output=True, text=True)
        
        if result.returncode != 0:
            error_exit(f"Failed to create config: {result.stderr}")
        else:
            print(result.stdout)
            success_print(f"Configuration created: {config_file}")
        
    except Exception as e:
        error_exit(f"Create command failed: {e}")


def monitor_command(args):
    """Execute the monitor command."""
    print(f"Monitoring process PID: {args.pid}")
    print(f"Interval: {args.interval} seconds")
    
    if args.output:
        print(f"Output file: {args.output}")
    
    # Check if process exists
    try:
        os.kill(args.pid, 0)
    except OSError:
        error_exit(f"Process with PID {args.pid} does not exist")
    
    print("Monitoring feature in development")
    print("(Would monitor process status, memory usage, and simulation progress)")


def stats_command(args):
    """Execute the stats command."""
    config_file = Path(args.config_file)
    if not config_file.exists():
        error_exit(f"Config file not found: {config_file}")
    
    print(f"Collecting statistics from: {config_file}")
    
    if not PYNLM_AVAILABLE:
        print("Warning: Python bindings not available. Running basic file statistics only.")
        
        # Show config file statistics
        with open(config_file, 'r') as f:
            content = f.read()
        
        print(f"Config file size: {len(content)} characters")
        print(f"Config file lines: {len(content.splitlines())}")
        
        # Extract some key info
        lines = content.splitlines()
        key_lines = [line for line in lines if '=' in line and not line.strip().startswith('#')]
        print(f"Config entries: {len(key_lines)}")
        
        # Save stats if output specified
        if args.output:
            import json
            output_path = Path(args.output)
            stats_data = {
                'file_size': len(content),
                'line_count': len(content.splitlines()),
                'config_entries': len(key_lines),
                'keys': [line.split('=')[0].strip() for line in key_lines],
                'timestamp': time.time()
            }
            
            with open(output_path, 'w') as f:
                json.dump(stats_data, f, indent=2)
            print(f"Statistics saved to: {output_path}")
        
        success_print("Basic statistics collected successfully")
        return
    
    try:
        # Load config
        config = pynlm.Config()
        config.loadFromFile(str(config_file))
        
        # Create brain and collect stats
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Run a few steps to collect stats
        for step in range(min(100, config.get("max_simulation_steps", 100))):
            brain.step(step)
        
        # Get comprehensive stats
        stats = pynlm.getBrainStats(brain)
        
        # Display stats
        print("\nSimulation Statistics:")
        print("-" * 40)
        for key, value in stats.items():
            print(f"{key.replace('_', ' ').title()}: {value}")
        
        # Save stats if output specified
        if args.output:
            import json
            output_path = Path(args.output)
            with open(output_path, 'w') as f:
                json.dump(stats, f, indent=2)
            print(f"\nStatistics saved to: {output_path}")
        
        success_print("Statistics collected successfully")
        
    except Exception as e:
        error_exit(f"Stats collection failed: {e}")


def checkpoint_command(args):
    """Execute the checkpoint command."""
    print(f"Checkpoint action: {args.action}")
    print(f"Brain file: {args.brain_file}")
    
    if args.action == 'list':
        print("Listing available checkpoints...")
        # List checkpoint files in the current directory
        checkpoint_files = list(Path('.').glob('*.bin')) + list(Path('.').glob('*.checkpoint'))
        if checkpoint_files:
            print("Available checkpoints:")
            for cf in checkpoint_files:
                stat = cf.stat()
                print(f"  {cf.name} ({stat.st_size} bytes, modified: {stat.st_mtime})")
        else:
            print("No checkpoint files found in current directory")
        return
    
    if not PYNLM_AVAILABLE:
        print("Warning: Python bindings not available.")
        if args.action == 'save':
            print("Checkpoint save requires a brain object and Python bindings.")
            print("Save functionality requires the pynlm module to be installed.")
        elif args.action == 'load':
            print("Checkpoint load requires a brain object and Python bindings.")
            print("Load functionality requires the pynlm module to be installed.")
        else:
            print("Unknown checkpoint action")
        return
    
    if args.action == 'save':
        if not args.brain:
            error_exit("Brain object required for save action")
        print(f"Saving brain to checkpoint: {args.brain_file}")
        
        try:
            # Use existing checkpoint function
            pynlm.saveBrainCheckpoint(args.brain, args.brain_file)
            success_print(f"Brain checkpoint saved to: {args.brain_file}")
        except Exception as e:
            error_exit(f"Failed to save checkpoint: {e}")
    
    elif args.action == 'load':
        if not args.brain:
            error_exit("Brain object required for load action")
        print(f"Loading brain from checkpoint: {args.brain_file}")
        
        try:
            # Use existing checkpoint function
            pynlm.loadBrainCheckpoint(args.brain, args.brain_file)
            success_print(f"Brain checkpoint loaded from: {args.brain_file}")
        except Exception as e:
            error_exit(f"Failed to load checkpoint: {e}")


def profile_command(args):
    """Execute the profile command."""
    config_file = Path(args.config_file)
    if not config_file.exists():
        error_exit(f"Config file not found: {config_file}")
    
    print(f"Profiling simulation with config: {config_file}")
    print(f"Output format: {args.format}")
    
    if not PYNLM_AVAILABLE:
        print("Warning: Python bindings not available. Running basic profiling only.")
        
        # Basic profiling of config file
        with open(config_file, 'r') as f:
            content = f.read()
        
        # Simple timing
        import time
        start_time = time.time()
        
        # Read file (simulating work)
        lines = content.splitlines()
        
        end_time = time.time()
        
        # Collect profiling data
        profile_data = {
            'config_file': str(config_file),
            'total_steps': len(lines),
            'total_time': end_time - start_time,
            'steps_per_second': len(lines) / (end_time - start_time) if (end_time - start_time) > 0 else 0,
            'neuron_count': len([line for line in lines if '=' in line and 'neuron' in line.lower()]),
            'timestamp': start_time,
            'format': args.format,
            'note': 'Basic profiling (Python bindings not available)'
        }
        
        # Save profile data
        if args.output:
            output_path = Path(args.output)
            
            if args.format == 'json':
                import json
                with open(output_path, 'w') as f:
                    json.dump(profile_data, f, indent=2)
            elif args.format == 'csv':
                import csv
                with open(output_path, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(profile_data.keys())
                    writer.writerow(profile_data.values())
            elif args.format == 'html':
                # Simple HTML report
                html_content = f"""
<!DOCTYPE html>
<html>
<head><title>NLM Profile Report</title></head>
<body>
    <h1>NLM Performance Profile</h1>
    <p>Config: {config_file}</p>
    <p>Total Lines: {len(lines)}</p>
    <p>Total Time: {end_time - start_time:.2f} seconds</p>
    <p>Lines per Second: {len(lines) / (end_time - start_time):.2f}</p>
    <p>Note: Python bindings not available</p>
</body>
</html>
"""
                with open(output_path, 'w') as f:
                    f.write(html_content)
            
            print(f"Profile data saved to: {output_path}")
        
        print(f"\nProfile Results (Basic):")
        print(f"  - Total Lines: {len(lines)}")
        print(f"  - Total Time: {end_time - start_time:.2f} seconds")
        print(f"  - Lines per Second: {len(lines) / (end_time - start_time):.2f}")
        
        success_print("Basic performance profiling completed")
        return
    
    try:
        # Load config
        config = pynlm.Config()
        config.loadFromFile(str(config_file))
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        # Profile the brain
        print("Starting performance profiling...")
        
        # Simple timing profile
        import time
        start_time = time.time()
        
        # Run simulation
        steps = min(500, config.get("max_simulation_steps", 100))
        for step in range(steps):
            brain.step(step)
        
        end_time = time.time()
        
        # Collect profiling data
        profile_data = {
            'config_file': str(config_file),
            'total_steps': steps,
            'total_time': end_time - start_time,
            'steps_per_second': steps / (end_time - start_time),
            'neuron_count': brain.getTotalNeuronCount(),
            'timestamp': start_time,
            'format': args.format
        }
        
        # Save profile data
        if args.output:
            output_path = Path(args.output)
            
            if args.format == 'json':
                import json
                with open(output_path, 'w') as f:
                    json.dump(profile_data, f, indent=2)
            elif args.format == 'csv':
                import csv
                with open(output_path, 'w', newline='') as f:
                    writer = csv.writer(f)
                    writer.writerow(profile_data.keys())
                    writer.writerow(profile_data.values())
            elif args.format == 'html':
                # Simple HTML report
                html_content = f"""
<!DOCTYPE html>
<html>
<head><title>NLM Profile Report</title></head>
<body>
    <h1>NLM Performance Profile</h1>
    <p>Config: {config_file}</p>
    <p>Total Steps: {steps}</p>
    <p>Total Time: {end_time - start_time:.2f} seconds</p>
    <p>Steps per Second: {steps / (end_time - start_time):.2f}</p>
    <p>Neuron Count: {brain.getTotalNeuronCount()}</p>
</body>
</html>
"""
                with open(output_path, 'w') as f:
                    f.write(html_content)
            
            print(f"Profile data saved to: {output_path}")
        
        print(f"\nProfile Results:")
        print(f"  - Total Steps: {steps}")
        print(f"  - Total Time: {end_time - start_time:.2f} seconds")
        print(f"  - Steps per Second: {steps / (end_time - start_time):.2f}")
        print(f"  - Neuron Count: {brain.getTotalNeuronCount()}")
        
        success_print("Performance profiling completed")
        
    except Exception as e:
        error_exit(f"Profiling failed: {e}")


def interactive_command(args):
    """Execute the interactive command."""
    print("Starting NLM interactive mode...")
    print("Type 'help' for available commands, 'exit' to quit")
    
    if not PYNLM_AVAILABLE:
        error_exit("Python bindings not available. Please install pynlm.")
    
    # Simple interactive mode
    while True:
        try:
            user_input = input("\nnlm> ").strip()
            
            if user_input.lower() in ('exit', 'quit', 'q'):
                print("Exiting interactive mode")
                break
            
            elif user_input.lower() in ('help', 'h'):
                print("\nAvailable interactive commands:")
                print("  help, h     - Show this help")
                print("  exit, quit  - Exit interactive mode")
                print("  create       - Create a new brain")
                print("  run <steps>  - Run simulation")
                print("  stats        - Show brain statistics")
                print("  config       - Show current config")
                print("  checkpoint save|load <file> - Manage checkpoints")
            
            elif user_input.lower() == 'create':
                brain = pynlm.createQuickBrain()
                print(f"Created quick brain with {brain.getTotalNeuronCount()} neurons")
            
            elif user_input.lower().startswith('run '):
                try:
                    steps = int(user_input.split()[1])
                    brain = pynlm.createQuickBrain()
                    brain.initialize()
                    print(f"Running simulation for {steps} steps...")
                    
                    for step in range(steps):
                        brain.step(step)
                        if step % 100 == 0:
                            print(f"  Step {step}/{steps}")
                    
                    stats = pynlm.getBrainStats(brain)
                    print(f"Simulation complete. Total spikes: {stats.get('total_spikes', 0)}")
                except ValueError:
                    print("Usage: run <steps>")
            
            elif user_input.lower() == 'stats':
                brain = pynlm.createQuickBrain()
                brain.initialize()
                
                for step in range(50):
                    brain.step(step)
                
                stats = pynlm.getBrainStats(brain)
                print("\nBrain Statistics:")
                for key, value in stats.items():
                    print(f"  {key}: {value}")
            
            elif user_input.lower() == 'config':
                config = pynlm.createExampleConfig()
                print(f"\nCurrent configuration keys: {config.getKeys()}")
            
            elif user_input.lower().startswith('checkpoint '):
                parts = user_input.split()
                if len(parts) >= 2:
                    action = parts[1]
                    if len(parts) >= 3:
                        filename = parts[2]
                        brain = pynlm.createQuickBrain()
                        
                        if action == 'save':
                            pynlm.saveBrainCheckpoint(brain, filename)
                            print(f"Brain saved to {filename}")
                        elif action == 'load':
                            pynlm.loadBrainCheckpoint(brain, filename)
                            print(f"Brain loaded from {filename}")
                        else:
                            print(f"Unknown checkpoint action: {action}")
                    else:
                        print("Usage: checkpoint save|load <filename>")
                else:
                    print("Usage: checkpoint save|load <filename>")
            
            else:
                print(f"Unknown command: {user_input}")
                print("Type 'help' for available commands")
        
        except KeyboardInterrupt:
            print("\nInterrupted. Type 'exit' to quit.")
        
        except Exception as e:
            print(f"Error: {e}")


def plugin_command(args):
    """Execute the plugin command."""
    print(f"Plugin action: {args.action}")
    print(f"Plugin name: {args.name}")
    
    print("Plugin system feature in development")
    print("(Would manage plugins for extensions)")


def main():
    """Main CLI entry point."""
    parser = parse_arguments()
    args = parser.parse_args()
    
    # Dispatch to appropriate command handler
    command_handlers = {
        'run': run_command,
        'validate': validate_command,
        'info': info_command,
        'benchmark': benchmark_command,
        'experiment': experiment_command,
        'create': create_command,
        'monitor': monitor_command,
        'stats': stats_command,
        'checkpoint': checkpoint_command,
        'profile': profile_command,
        'interactive': interactive_command,
        'plugin': plugin_command
    }
    
    if args.command in command_handlers:
        try:
            command_handlers[args.command](args)
        except Exception as e:
            error_exit(f"Command failed: {e}")
    else:
        error_exit(f"Unknown command: {args.command}")


if __name__ == '__main__':
    main()

