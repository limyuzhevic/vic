#!/usr/bin/env python3
"""
NLM Command Line Interface (CLI) - Advanced scripting and batch processing

This script provides enhanced functionality for running NLM simulations
programmatically with advanced configuration, batch processing, and scripting
capabilities.
"""

import sys
import os
import json
import argparse
from pathlib import Path
from typing import Dict, List, Any, Optional
import subprocess
import time

def main():
    """Main CLI entry point for NLM advanced operations."""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_cli.py run --phase 6 --neurons 2000
  nlm_cli.py batch --iterations 10 --config custom.cfg
  nlm_cli.py script demo.nlm --output results.json
  nlm_cli.py benchmark --duration 300 --metrics full
  nlm_cli.py experiment --template cognitive_learning

For more detailed help, use: nlm_cli.py help [command]
        """)
    
    # Global options
    parser.add_argument(
        '--config', '-c',
        default='configs/default.cfg',
        help='Configuration file path (default: configs/default.cfg)'
    )
    parser.add_argument(
        '--output-dir', '-o',
        default='outputs',
        help='Output directory for results (default: outputs)'
    )
    parser.add_argument(
        '--verbose', '-v',
        action='store_true',
        help='Enable verbose output'
    )
    parser.add_argument(
        '--log-file',
        help='Log output to file'
    )
    
    # Subcommands
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Run simulation command
    run_parser = subparsers.add_parser(
        'run', 
        help='Run a single NLM simulation'
    )
    run_parser.add_argument(
        'phase', 
        type=int, 
        choices=[2, 3, 4, 5, 6, 7],
        help='NLM phase to run (2-6, or 7 for text demo)'
    )
    run_parser.add_argument(
        '--neurons', '-n',
        type=int,
        default=500,
        help='Number of neurons (default: 500)'
    )
    run_parser.add_argument(
        '--steps', '-s',
        type=int,
        default=1000,
        help='Maximum simulation steps (default: 1000)'
    )
    run_parser.add_argument(
        '--enable-checkpoint',
        action='store_true',
        help='Enable checkpointing'
    )
    run_parser.add_argument(
        '--enable-development',
        action='store_true',
        help='Enable developmental stages'
    )
    run_parser.add_argument(
        '--enable-curiosity',
        action='store_true',
        help='Enable curiosity-driven exploration'
    )
    run_parser.add_argument(
        '--enable-reward-modulation',
        action='store_true',
        help='Enable reward modulation'
    )
    run_parser.add_argument(
        '--extra-config',
        action='append',
        help='Additional configuration key=value pairs'
    )
    
    # Batch command
    batch_parser = subparsers.add_parser(
        'batch', 
        help='Run multiple simulations in batch'
    )
    batch_parser.add_argument(
        'iterations', 
        type=int,
        help='Number of iterations to run'
    )
    batch_parser.add_argument(
        '--template', '-t',
        choices=['random', 'progressive', 'expert'],
        default='random',
        help='Parameter variation template (default: random)'
    )
    batch_parser.add_argument(
        '--max-neurons', 
        type=int,
        default=1000,
        help='Maximum neuron count (default: 1000)'
    )
    batch_parser.add_argument(
        '--min-neurons',
        type=int,
        default=100,
        help='Minimum neuron count (default: 100)'
    )
    batch_parser.add_argument(
        '--duration',
        type=int,
        help='Duration of each iteration in seconds'
    )
    
    # Script command
    script_parser = subparsers.add_parser(
        'script', 
        help='Execute a NLM script file'
    )
    script_parser.add_argument(
        'script_file',
        help='Path to script file'
    )
    script_parser.add_argument(
        '--check-only',
        action='store_true',
        help='Only check script syntax, do not execute'
    )
    
    # Benchmark command
    benchmark_parser = subparsers.add_parser(
        'benchmark', 
        help='Run performance benchmarks'
    )
    benchmark_parser.add_argument(
        '--duration', '-d',
        type=int,
        default=60,
        help='Benchmark duration in seconds (default: 60)'
    )
    benchmark_parser.add_argument(
        '--metrics', '-m',
        choices=['basic', 'detailed', 'full'],
        default='detailed',
        help='Metrics collection level (default: detailed)'
    )
    benchmark_parser.add_argument(
        '--phases', '-p',
        nargs='+',
        type=int,
        default=[2, 3, 4, 5, 6],
        help='Phases to benchmark (default: 2 3 4 5 6)'
    )
    
    # Experiment command
    experiment_parser = subparsers.add_parser(
        'experiment', 
        help='Run predefined experiment templates'
    )
    experiment_parser.add_argument(
        'template',
        choices=["cognitive_learning", "social_learning", "sensorimotor", "development", "plasticity"],
        help='Experiment template to run'
    )
    experiment_parser.add_argument(
        '--parameters',
        help='JSON file with experiment parameters'
    )
    
    # Analysis command
    analysis_parser = subparsers.add_parser(
        'analyze', 
        help='Analyze simulation results'
    )
    analysis_parser.add_argument(
        'data_file',
        help='Input data file'
    )
    analysis_parser.add_argument(
        '--output-format',
        choices=['json', 'csv', 'text'],
        default='json',
        help='Output format (default: json)'
    )
    analysis_parser.add_argument(
        '--analysis-type',
        choices=['performance', 'learning', 'memory', 'all'],
        default='all',
        help='Type of analysis (default: all)'
    )
    
    # Help command
    help_parser = subparsers.add_parser(
        'help',
        help='Show help for a specific command'
    )
    help_parser.add_argument(
        'command',
        nargs='?',
        help='Command to get help for'
    )
    
    # Parse arguments
    args = parser.parse_args()
    
    # Route to appropriate handler
    if args.command == 'run':
        run_simulation(args)
    elif args.command == 'batch':
        run_batch(args)
    elif args.command == 'script':
        execute_script(args)
    elif args.command == 'benchmark':
        run_benchmark(args)
    elif args.command == 'experiment':
        run_experiment(args)
    elif args.command == 'analyze':
        analyze_results(args)
    elif args.command == 'help':
        show_help(args.command, parser)
    else:
        parser.print_help()
        sys.exit(1)

def run_simulation(args):
    """Run a single NLM simulation."""
    print(f"Running NLM Phase {args.phase} with {args.neurons} neurons...")
    
    # Build command
    cmd = [
        sys.executable if sys.executable else 'python3',
        '-m', 'pynlm'
    ]
    
    base_args = [
        '--phase', str(args.phase),
        '--neurons', str(args.neurons),
        '--steps', str(args.steps),
        '--config', args.config,
        '--verbose' if args.verbose else ''
    ]
    
    if args.enable_checkpoint:
        base_args.append('--checkpoint')
    if args.enable_development:
        base_args.append('--development')
    if args.enable_curiosity:
        base_args.append('--curiosity')
    if args.enable_reward_modulation:
        base_args.append('--reward-modulation')
    
    # Filter out empty strings
    base_args = [arg for arg in base_args if arg]
    
    full_cmd = cmd + base_args
    
    try:
        print(f"Executing: {' '.join(full_cmd)}")
        result = subprocess.run(
            full_cmd,
            check=True,
            capture_output=not args.verbose
        )
        print(f"Simulation completed successfully (exit code: {result.returncode})")
    except subprocess.CalledProcessError as e:
        print(f"Simulation failed with error: {e}")
        sys.exit(1)
    except FileNotFoundError:
        print("Error: pynlm module not found. Please install NLM Python bindings.")
        sys.exit(1)

def run_batch(args):
    """Run multiple simulations in batch."""
    print(f"Starting batch run with {args.iterations} iterations...")
    
    # Create output directory
    os.makedirs(args.output_dir, exist_ok=True)
    
    results = []
    start_time = time.time()
    
    for i in range(args.iterations):
        print(f"\n--- Iteration {i + 1}/{args.iterations} ---")
        
        # Generate parameters based on template
        params = generate_batch_parameters(args.template, i, args)
        
        # Save parameters to file
        param_file = os.path.join(args.output_dir, f"iteration_{i:04d}.cfg")
        save_parameters(param_file, params)
        
        # Run simulation
        iteration_result = run_single_batch_iteration(i, params, args)
        results.append(iteration_result)
        
        # Wait if duration specified
        if args.duration:
            time.sleep(args.duration)
    
    # Save batch results
    batch_result_file = os.path.join(args.output_dir, 'batch_results.json')
    save_batch_results(results, batch_result_file)
    
    total_time = time.time() - start_time
    print(f"\nBatch completed in {total_time:.2f} seconds")
    print(f"Results saved to {batch_result_file}")

def generate_batch_parameters(template, iteration, args):
    """Generate parameters for batch iteration based on template."""
    if template == 'random':
        neurons = random.randint(args.min_neurons, args.max_neurons)
        steps = random.randint(500, 2000)
    elif template == 'progressive':
        neurons = args.min_neurons + (iteration * (args.max_neurons - args.min_neurons) // args.iterations)
        steps = 500 + (iteration * 1500 // args.iterations)
    elif template == 'expert':
        # Expert template uses specific neuron counts optimized for learning
        expert_counts = [500, 750, 1000, 1500, 2000]
        neurons = expert_counts[min(iteration % len(expert_counts), len(expert_counts) - 1)]
        steps = 2000  # Fixed longer duration for expert runs
    
    return {
        'neurons': neurons,
        'steps': steps,
        'enable_checkpoint': random.random() < 0.3,  # 30% chance of checkpointing
        'enable_development': random.random() < 0.5,  # 50% chance of development
        'enable_curiosity': random.random() < 0.7,   # 70% chance of curiosity
        'enable_reward_modulation': random.random() < 0.8,  # 80% chance of reward modulation
    }

def run_single_batch_iteration(iteration, params, args):
    """Run a single batch iteration."""
    print(f"  Parameters: neurons={params['neurons']}, steps={params['steps']}")
    
    # Build command
    cmd = [
        sys.executable if sys.executable else 'python3',
        '-m', 'pynlm'
    ]
    
    base_args = [
        '--phase', '6',
        '--neurons', str(params['neurons']),
        '--steps', str(params['steps']),
        '--config', args.config,
        '--verbose' if args.verbose else ''
    ]
    
    if params['enable_checkpoint']:
        base_args.append('--checkpoint')
    if params['enable_development']:
        base_args.append('--development')
    if params['enable_curiosity']:
        base_args.append('--curiosity')
    if params['enable_reward_modulation']:
        base_args.append('--reward-modulation')
    
    # Filter out empty strings
    base_args = [arg for arg in base_args if arg]
    
    full_cmd = cmd + base_args
    
    try:
        result = subprocess.run(
            full_cmd,
            check=True,
            capture_output=True,
            text=True
        )
        return {
            'iteration': iteration,
            'status': 'success',
            'neurons': params['neurons'],
            'steps': params['steps'],
            'stdout': result.stdout,
            'stderr': result.stderr
        }
    except subprocess.CalledProcessError as e:
        return {
            'iteration': iteration,
            'status': 'failed',
            'error': str(e),
            'params': params
        }

def save_parameters(filename, params):
    """Save parameters to file."""
    with open(filename, 'w') as f:
        for key, value in params.items():
            f.write(f"{key} = {value}\n")

def save_batch_results(results, filename):
    """Save batch results to JSON file."""
    with open(filename, 'w') as f:
        json.dump({
            'total_iterations': len(results),
            'successful': sum(1 for r in results if r['status'] == 'success'),
            'failed': sum(1 for r in results if r['status'] == 'failed'),
            'results': results
        }, f, indent=2)

def save_script_results(results, filename):
    """Save script execution results."""
    with open(filename, 'w') as f:
        json.dump({
            'timestamp': time.time(),
            'results': results
        }, f, indent=2)

def run_benchmark(args):
    """Run performance benchmarks."""
    print(f"Starting benchmark for {args.duration} seconds...")
    print(f"Metrics level: {args.metrics}")
    
    benchmarks = []
    start_time = time.time()
    
    for phase in args.phases:
        print(f"\nBenchmarking Phase {phase}...")
        phase_start = time.time()
        
        # Run benchmark for this phase
        cmd = [
            sys.executable if sys.executable else 'python3',
            '-m', 'pynlm',
            '--phase', str(phase),
            '--verbose' if args.verbose else ''
        ]
        
        # Add extra arguments based on metrics level
        if args.metrics in ['detailed', 'full']:
            cmd.extend(['--checkpoint'])
        if args.metrics == 'full':
            cmd.extend(['--development', '--curiosity', '--reward-modulation'])
        
        # Filter out empty strings
        cmd = [arg for arg in cmd if arg]
        
        try:
            print(f"  Executing: {' '.join(cmd)}")
            result = subprocess.run(
                cmd,
                check=True,
                capture_output=True,
                text=True
            )
            
            phase_time = time.time() - phase_start
            benchmarks.append({
                'phase': phase,
                'status': 'success',
                'execution_time': phase_time,
                'output': result.stdout if args.verbose else None
            })
            
        except subprocess.CalledProcessError as e:
            benchmarks.append({
                'phase': phase,
                'status': 'failed',
                'error': str(e)
            })
    
    total_time = time.time() - start_time
    
    # Save benchmark results
    benchmark_file = os.path.join(args.output_dir, 'benchmark_results.json')
    with open(benchmark_file, 'w') as f:
        json.dump({
            'total_duration': args.duration,
            'actual_duration': total_time,
            'metrics_level': args.metrics,
            'benchmarks': benchmarks
        }, f, indent=2)
    
    print(f"\nBenchmark completed in {total_time:.2f} seconds")
    print(f"Results saved to {benchmark_file}")

def run_experiment(args):
    """Run predefined experiment template."""
    print(f"Running experiment template: {args.template}")
    
    # Load parameters from JSON if provided
    parameters = {}
    if args.parameters:
        try:
            with open(args.parameters, 'r') as f:
                parameters = json.load(f)
        except Exception as e:
            print(f"Warning: Could not load parameters file: {e}")
    
    # Experiment-specific setup
    experiment_config = get_experiment_config(args.template, parameters)
    
    # Run simulation with experiment configuration
    print("Starting experiment simulation...")
    run_simulation(create_experiment_args(experiment_config))
    
    print("Experiment completed!")

def get_experiment_config(template, parameters):
    """Get configuration for experiment template."""
    base_config = {
        'phase': 6,
        'neurons': 1000,
        'steps': 3000,
        'enable_checkpoint': True,
        'enable_development': True,
        'enable_curiosity': True,
        'enable_reward_modulation': True
    }
    
    # Template-specific overrides
    templates = {
        'cognitive_learning': {
            'neurons': 1500,
            'steps': 5000,
            'enable_curiosity': True,
            'enable_reward_modulation': True
        },
        'social_learning': {
            'neurons': 800,
            'steps': 4000,
            'enable_curiosity': True,
            'enable_reward_modulation': True
        },
        'sensorimotor': {
            'neurons': 1200,
            'steps': 3000,
            'enable_development': True,
            'enable_curiosity': False
        },
        'development': {
            'neurons': 1000,
            'steps': 6000,
            'enable_development': True,
            'enable_curiosity': True
        },
        'plasticity': {
            'neurons': 500,
            'steps': 2000,
            'enable_checkpoint': True,
            'enable_reward_modulation': True
        }
    }
    
    config = base_config.copy()
    if template in templates:
        config.update(templates[template])
    config.update(parameters)
    
    return config

def analyze_results(args):
    """Analyze simulation results."""
    print(f"Analyzing results from {args.data_file}")
    print(f"Analysis type: {args.analysis_type}")
    print(f"Output format: {args.output_format}")
    
    # Load data
    try:
        with open(args.data_file, 'r') as f:
            if args.data_file.endswith('.json'):
                data = json.load(f)
            elif args.data_file.endswith('.csv'):
                # TODO: Parse CSV
                print("CSV parsing not yet implemented")
                return
            else:
                print(f"Unsupported file format: {args.data_file}")
                return
    except Exception as e:
        print(f"Error loading data file: {e}")
        return
    
    # Perform analysis
    analysis_results = perform_analysis(data, args.analysis_type)
    
    # Save results
    output_file = os.path.splitext(args.data_file)[0] + f".{args.output_format}"
    if args.output_format == 'json':
        with open(output_file, 'w') as f:
            json.dump(analysis_results, f, indent=2)
    elif args.output_format == 'csv':
        save_csv_analysis(analysis_results, output_file)
    else:
        save_text_analysis(analysis_results, output_file)
    
    print(f"Analysis saved to {output_file}")

def perform_analysis(data, analysis_type):
    """Perform analysis on simulation data."""
    # Placeholder implementation - would contain actual analysis logic
    return {
        'analysis_type': analysis_type,
        'summary': 'Analysis results placeholder',
        'statistics': {},
        'findings': []
    }

def save_csv_analysis(results, filename):
    """Save analysis results to CSV."""
    # Placeholder implementation
    print(f"CSV output to {filename}")

def save_text_analysis(results, filename):
    """Save analysis results to text file."""
    # Placeholder implementation
    with open(filename, 'w') as f:
        f.write("Analysis Results\n")
        f.write("================\n\n")
        f.write(str(results))

def execute_script(args):
    """Execute a NLM script file."""
    script_path = Path(args.script_file)
    
    if not script_path.exists():
        print(f"Error: Script file not found: {args.script_file}")
        sys.exit(1)
    
    print(f"Executing script: {args.script_file}")
    
    if args.check_only:
        print("Syntax check only - no execution performed")
        return
    
    # Parse script
    try:
        script_content = script_path.read_text()
        script_commands = parse_nlm_script(script_content)
    except Exception as e:
        print(f"Error parsing script: {e}")
        sys.exit(1)
    
    # Execute commands
    results = []
    for command in script_commands:
        print(f"Executing: {command}")
        try:
            result = execute_script_command(command)
            results.append({
                'command': command,
                'status': 'success',
                'result': result
            })
        except Exception as e:
            results.append({
                'command': command,
                'status': 'failed',
                'error': str(e)
            })
    
    # Save script results
    script_result_file = script_path.stem + '_results.json'
    save_script_results(results, script_result_file)
    print(f"Script execution completed. Results saved to {script_result_file}")

def parse_nlm_script(content):
    """Parse NLM script content into commands."""
    commands = []
    lines = content.split('\n')
    
    for line in lines:
        line = line.strip()
        if not line or line.startswith('#'):
            continue
        
        # Simple command parsing
        if line.startswith('run'):
            commands.append(('run', line[4:].strip()))
        elif line.startswith('set'):
            commands.append(('set', line[4:].strip()))
        elif line.startswith('help'):
            commands.append(('help', line[5:].strip()))
        else:
            commands.append(('execute', line))
    
    return commands

def execute_script_command(command):
    """Execute a single script command."""
    # Placeholder implementation
    return f"Executed: {command}"

def create_experiment_args(config):
    """Create arguments object from experiment config."""
    # Create a simple namespace object
    class Args:
        pass
    
    args = Args()
    for key, value in config.items():
        setattr(args, key, value)
    
    return args

def show_help(command, parser):
    """"Show help for specific command."""
    if command:
        print(f"\n=== Help for '{command}' command ===")
        # Find and show help for specific command
        for action in parser._actions:
            if action.dest == command:
                print(action.help)
                break
    else:
        parser.print_help()

if __name__ == '__main__':
    main()
