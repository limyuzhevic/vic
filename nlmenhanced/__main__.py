"""
Command-line interface for Enhanced NLM Python API

This module provides command-line tools for working with the Enhanced NLM Python API,
including running simulations, managing experiments, and generating reports.
"""

import argparse
import sys
import os
from pathlib import Path
import json
import time

# Add parent directory to path to import nlmenhanced
sys.path.insert(0, str(Path(__file__).parent.parent))

import nlmenhanced as nlm
def create_parser():
    """Create the argument parser for the command-line interface."""
    parser = argparse.ArgumentParser(
        description="Enhanced NLM Python API Command-Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  %(prog)s run --steps 100 --profile balanced
  %(prog)s experiment --name "neural_test" --steps 500 --profile research
  %(prog)s analyze --file results.json
  %(prog)s visualize --steps 100 --output viz.png
  %(prog)s create --config config.json

For more information, visit: https://nlm-project.github.io/nlm-python-enhanced/
        """
    )
    
    parser.add_argument(
        '--version', '-v',
        action='version',
        version=f'Enhanced NLM Python API v{nlm.__version__}'
    )
    
    # Global options
    parser.add_argument(
        '--verbose', '-V',
        action='store_true',
        help='Enable verbose output'
    )
    
    parser.add_argument(
        '--quiet', '-q',
        action='store_true',
        help='Suppress output messages'
    )
    
    parser.add_argument(
        '--output-dir', '-o',
        default='output',
        help='Output directory for results and visualizations'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    run_parser = subparsers.add_parser(
        'run',
        help='Run a quick simulation'
    )
    run_parser.add_argument(
        'steps', type=int, default=100,
        help='Number of simulation steps (default: 100)'
    )
    run_parser.add_argument(
        '--profile', '-p',
        choices=[p.value for p in nlm.Profile],
        default='balanced',
        help='Configuration profile (default: balanced)'
    )
    run_parser.add_argument(
        '--brain-config', type=str, default=None,
        help='Path to custom brain configuration file'
    )
    
    # Experiment command
    exp_parser = subparsers.add_parser(
        'experiment',
        help='Run an experiment'
    )
    exp_parser.add_argument(
        '--name', '-n',
        required=True,
        help='Experiment name'
    )
    exp_parser.add_argument(
        '--steps', '-s', type=int, required=True,
        help='Number of simulation steps'
    )
    exp_parser.add_argument(
        '--profile', '-p',
        choices=[p.value for p in nlm.Profile],
        default='balanced',
        help='Configuration profile (default: balanced)'
    )
    exp_parser.add_argument(
        '--brain-config', type=str, default=None,
        help='Path to custom brain configuration file'
    )
    exp_parser.add_argument(
        '--analysis', nargs='+',
        default=['basic', 'advanced'],
        help='Types of analysis to perform (default: basic advanced)'
    )
    exp_parser.add_argument(
        '--visualization', nargs='+',
        default=[],
        help='Types of visualizations to create'
    )
    exp_parser.add_argument(
        '--export-format',
        choices=[fmt.value for fmt in nlm.ExportFormat],
        default='json',
        help='Export format for results (default: json)'
    )
    
    # Create configuration command
    create_parser = subparsers.add_parser(
        'create',
        help='Create configuration'
    )
    create_parser.add_argument(
        'action', choices=['config', 'brain', 'world', 'agent'],
        help='Type of configuration to create'
    )
    create_parser.add_argument(
        '--name', '-n',
        help='Name for the configuration'
    )
    create_parser.add_argument(
        '--profile', '-p',
        choices=[p.value for p in nlm.Profile],
        default='balanced',
        help='Configuration profile (default: balanced)'
    )
    create_parser.add_argument(
        '--output', '-o',
        help='Output file path'
    )
    
    # Analyze command
    analyze_parser = subparsers.add_parser(
        'analyze',
        help='Analyze simulation results'
    )
    analyze_parser.add_argument(
        'file', help='Path to results file'
    )
    analyze_parser.add_argument(
        '--output', '-o',
        help='Output file for analysis results'
    )
    
    # Visualize command
    viz_parser = subparsers.add_parser(
        'visualize',
        help='Create visualizations'
    )
    viz_parser.add_argument(
        '--steps', type=int, default=100,
        help='Number of simulation steps (default: 100)'
    )
    viz_parser.add_argument(
        '--profile', '-p',
        choices=[p.value for p in nlm.Profile],
        default='balanced',
        help='Configuration profile (default: balanced)'
    )
    viz_parser.add_argument(
        '--output', '-o',
        default='visualization.png',
        help='Output visualization file (default: visualization.png)'
    )
    viz_parser.add_argument(
        '--format',
        choices=['png', 'pdf', 'svg'],
        default='png',
        help='Output format (default: png)'
    )
    
    # List available configurations command
    list_parser = subparsers.add_parser(
        'list',
        help='List available configurations and features'
    )
    
    return parser
def load_config_from_file(filepath):
    """Load configuration from file."""
    try:
        with open(filepath, 'r') as f:
            config = json.load(f)
        return config
    except FileNotFoundError:
        print(f"Error: Configuration file '{filepath}' not found")
        return None
    except json.JSONDecodeError as e:
        print(f"Error: Invalid JSON in configuration file: {e}")
        return None
def run_simulation_command(args):
    """Handle the 'run' command."""
    print(f"Running simulation with {args.steps} steps...")
    print(f"Profile: {args.profile}")
    
    # Parse profile
    profile = nlm.Profile(args.profile)
    
    # Create brain configuration
    brain_config = None
    if args.brain_config:
        brain_config = load_config_from_file(args.brain_config)
    
    # Create brain and run simulation
    try:
        brain = nlm.create_brain(profile, brain_config)
        
        if not args.quiet:
            print("Brain created successfully")
        
        # Run simulation
        start_time = time.time()
        result = nlm.run_simulation(brain, args.steps, profile, brain_config)
        end_time = time.time()
        
        print(f"Simulation completed in {end_time - start_time:.2f} seconds")
        print(f"Average firing rate: {result.metrics.get('avg_firing_rate', 0):.2f} Hz")
        print(f"Total spikes: {result.metrics.get('avg_spike_count', 0) * result.total_steps:.0f}")
        
        # Save results
        os.makedirs(args.output_dir, exist_ok=True)
        results_file = os.path.join(args.output_dir, f"simulation_{int(start_time)}.json")
        
        with open(results_file, 'w') as f:
            json.dump({
                'simulation_result': {
                    'steps': result.steps,
                    'metrics': result.metrics,
                    'experiment_name': result.experiment_name,
                    'duration': result.duration,
                    'total_steps': result.total_steps
                },
                'profile': profile.value,
                'steps': args.steps,
                'start_time': start_time,
                'end_time': end_time
            }, f, indent=2)
        
        print(f"Results saved to: {results_file}")
        
        return True
        
    except Exception as e:
        print(f"Error running simulation: {e}")
        return False
def run_experiment_command(args):
    """Handle the 'experiment' command."""
    print(f"Running experiment: {args.name}")
    print(f"Steps: {args.steps}, Profile: {args.profile}")
    
    # Parse profile
    profile = nlm.Profile(args.profile)
    
    # Load brain configuration
    brain_config = None
    if args.brain_config:
        brain_config = load_config_from_file(args.brain_config)
    
    # Create experiment configuration
    config = nlm.ExperimentConfig(
        name=args.name,
        brain_config=brain_config or {},
        simulation_steps=args.steps,
        profile=profile,
        analysis_types=args.analysis,
        visualization_types=args.visualization,
        export_format=nlm.ExportFormat(args.export_format),
        output_dir=args.output_dir
    )
    
    # Run experiment
    try:
        experiment = nlm.create_experiment(config)
        results = experiment.run()
        
        # Generate report
        report = results.generate_report()
        print("\n" + "="*60)
        print("EXPERIMENT REPORT")
        print("="*60)
        print(report)
        
        # Save report
        report_file = os.path.join(args.output_dir, f"{args.name}_report.txt")
        with open(report_file, 'w') as f:
            f.write(report)
        print(f"Report saved to: {report_file}")
        
        return True
        
    except Exception as e:
        print(f"Error running experiment: {e}")
        return False
def create_config_command(args):
    """Handle the 'create' command."""
    print(f"Creating {args.action} configuration...")
    
    profile = nlm.Profile(args.profile)
    
    try:
        if args.action == 'config':
            config = nlm.BrainConfigurator.createQuickStartConfig(profile.value)
            filename = args.output or f"config_{profile.value}_{int(time.time())}.json"
            
            with open(filename, 'w') as f:
                json.dump({
                    'profile': profile.value,
                    'neuron_count': config.get("neuron_count", 1000),
                    'simulation_timestep': config.get("simulation_timestep", 0.001),
                    'connection_probability': config.get("connection_probability", 0.1)
                }, f, indent=2)
            
            print(f"Configuration saved to: {filename}")
            
        elif args.action == 'brain':
            config = nlm.BrainConfigurator.createQuickStartConfig(profile.value)
            brain = nlm.createBrain(config)
            print("Brain configuration created successfully")
            
        elif args.action == 'world':
            world = nlm.createSimpleWorld()
            print("World configuration created successfully")
            
        elif args.action == 'agent':
            brain = nlm.createBrain(nlm.BrainConfigurator.createDefaultConfig())
            agent = nlm.createAgentBrain(brain)
            print("Agent configuration created successfully")
        
        return True
        
    except Exception as e:
        print(f"Error creating configuration: {e}")
        return False
def analyze_command(args):
    """Handle the 'analyze' command."""
    print(f"Analyzing results from: {args.file}")
    
    try:
        with open(args.file, 'r') as f:
            data = json.load(f)
        
        # Create analysis results
        analysis = {
            'file': args.file,
            'steps_analyzed': len(data.get('steps', [])),
            'metrics': data.get('metrics', {}),
            'timestamp': time.time()
        }
        
        # Save analysis
        output_file = args.output or f"analysis_{Path(args.file).stem}.json"
        with open(output_file, 'w') as f:
            json.dump(analysis, f, indent=2)
        
        print(f"Analysis saved to: {output_file}")
        print(f"Steps analyzed: {analysis['steps_analyzed']}")
        
        return True
        
    except Exception as e:
        print(f"Error analyzing results: {e}")
        return False
def visualize_command(args):
    """Handle the 'visualize' command."""
    print(f"Creating visualization with {args.steps} steps...")
    
    try:
        # Create brain and run simulation
        profile = nlm.Profile(args.profile)
        brain = nlm.create_brain(profile)
        result = nlm.run_simulation(brain, args.steps, profile)
        
        # Create visualization
        visualizer = nlm.NLMVisualizer(args.output_dir)
        
        if args.format == 'png':
            plot_path = visualizer.create_dashboard(result, args.output)
        else:
            print(f"Format {args.format} not yet supported")
            plot_path = visualizer.create_dashboard(result)
        
        print(f"Visualization saved to: {plot_path}")
        
        return True
        
    except Exception as e:
        print(f"Error creating visualization: {e}")
        return False
def list_command(args):
    """Handle the 'list' command."""
    print("Available configurations and features:")
    print()
    
    print("Configuration Profiles:")
    for profile in nlm.Profile:
        print(f"  • {profile.value}")
    
    print()
    print("Visualization Types:")
    for viz_type in nlm.VisualizationType:
        print(f"  • {viz_type.value}")
    
    print()
    print("Export Formats:")
    for fmt in nlm.ExportFormat:
        print(f"  • {fmt.value}")
    
    print()
    print("Analysis Types:")
    print("  • basic")
    print("  • advanced")
    
    print()
    print("Available functions:")
    for feature in nlm.get_available_features():
        print(f"  • {feature}")
def main():
    """Main entry point for the command-line interface."""
    parser = create_parser()
    args = parser.parse_args()
    
    # Handle commands
    if args.command == 'run':
        success = run_simulation_command(args)
    elif args.command == 'experiment':
        success = run_experiment_command(args)
    elif args.command == 'create':
        success = create_config_command(args)
    elif args.command == 'analyze':
        success = analyze_command(args)
    elif args.command == 'visualize':
        success = visualize_command(args)
    elif args.command == 'list':
        success = list_command(args)
    else:
        parser.print_help()
        success = False
    
    # Return success status
    sys.exit(0 if success else 1)
if __name__ == "__main__":
    main()