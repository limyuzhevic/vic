"""
Python interface for NLM (Neural Learning Machine)

This module provides the main Python interface for the NLM framework,
including command-line interfaces and higher-level APIs.
"""

import sys
import argparse
import json
from typing import List, Dict, Any

from .brain import Brain
from .visualization import (
    visualize_neural_network,
    visualize_spike_activity,
    visualize_network_topology,
    create_interactive_dashboard,
)

def run_command(args):
    """Run a simulation command."""
    try:
        # Load configuration if specified
        config = None
        if args.config:
            with open(args.config, 'r') as f:
                config = json.load(f)
        
        # Create brain
        if config:
            brain = Brain.from_config(config)
        else:
            brain = Brain(
                neuron_count=args.neuron_count,
                region_count=args.region_count,
                connection_probability=args.connection_probability,
                random_seed=args.random_seed
            )
        
        # Initialize and run
        brain.initialize()
        
        output_file = args.output or f"simulation_results_{int(__import__('time').time())}.json"
        
        print(f"Running simulation with {args.steps} steps...")
        result = brain.run(args.steps, output_file)
        
        print(f"Simulation completed!")
        print(f"Results saved to: {output_file}")
        print(f"Total spikes: {result['total_spikes']}")
        print(f"Total neurons: {result['total_neurons']}")
        print(f"Total synapses: {result['total_synapses']}")
        
        # Generate visualization if requested
        if args.visualize:
            print("Generating visualization...")
            visualize_neural_network(brain, "simulation_viz.html")
            print("Visualization saved to: simulation_viz.html")
        
        return result
        
    except Exception as e:
        print(f"Error: {e}")
        return None

def analyze_command(args):
    """Analyze simulation results."""
    try:
        # Load results
        with open(args.input, 'r') as f:
            results = json.load(f)
        
        # Create brain from results
        brain = Brain.from_results(results)
        
        print("Analyzing simulation results...")
        analysis = brain.analyze()
        
        output_file = args.output or "analysis_results.json"
        
        with open(output_file, 'w') as f:
            json.dump(analysis, f, indent=2)
        
        print(f"Analysis saved to: {output_file}")
        
        # Generate visualization if requested
        if args.visualize:
            print("Generating analysis visualization...")
            create_interactive_dashboard(brain, "analysis_dashboard.html")
            print("Dashboard saved to: analysis_dashboard.html")
        
        return analysis
        
    except Exception as e:
        print(f"Error: {e}")
        return None

def compare_command(args):
    """Compare multiple simulations."""
    try:
        # Load multiple result files
        results_list = []
        for input_file in args.input:
            with open(input_file, 'r') as f:
                results = json.load(f)
                results_list.append(results)
        
        # Create brains from results
        brains = [Brain.from_results(results) for results in results_list]
        
        print("Comparing simulations...")
        comparison = brain.compare(brains, args.metrics)
        
        output_file = args.output or "comparison_results.json"
        
        with open(output_file, 'w') as f:
            json.dump(comparison, f, indent=2)
        
        print(f"Comparison saved to: {output_file}")
        
        return comparison
        
    except Exception as e:
        print(f"Error: {e}")
        return None

def batch_command(args):
    """Run batch simulations."""
    try:
        # Load configuration file
        with open(args.config, 'r') as f:
            config_template = json.load(f)
        
        # Generate parameter combinations
        param_combinations = []
        for param_name, param_values in config_template.get('parameter_sweep', {}).items():
            for value in param_values:
                param_combination = config_template.copy()
                param_combination[param_name] = value
                param_combinations.append(param_combination)
        
        # Run simulations
        results = []
        for i, config in enumerate(param_combinations):
            print(f"Running simulation {i+1}/{len(param_combinations)}")
            
            brain = Brain.from_config(config)
            brain.initialize()
            
            output_file = f"{args.output_dir}/simulation_{i}.json"
            result = brain.run(config.get('steps', 100), output_file)
            results.append(result)
        
        print(f"Batch simulation completed. Results saved to: {args.output_dir}")
        return results
        
    except Exception as e:
        print(f"Error: {e}")
        return None

def report_command(args):
    """Generate reports."""
    try:
        # Load results
        with open(args.input, 'r') as f:
            results = json.load(f)
        
        # Create brain from results
        brain = Brain.from_results(results)
        
        print(f"Generating {args.report_type} report...")
        report = brain.generate_report(args.report_type)
        
        output_file = args.output or f"report_{int(__import__('time').time())}.md"
        
        with open(output_file, 'w') as f:
            f.write(report)
        
        print(f"Report saved to: {output_file}")
        
        return report
        
    except Exception as e:
        print(f"Error: {e}")
        return None

def main():
    """Main entry point for command-line interface."""
    parser = argparse.ArgumentParser(
        description="NLM Python Interface for Neural Network Simulation",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Command to execute')
    
    # Run command
    run_parser = subparsers.add_parser('run', help='Run a simulation')
    run_parser.add_argument('--config', '-c', help='Configuration file')
    run_parser.add_argument('--neuron-count', '-n', type=int, default=100, help='Number of neurons')
    run_parser.add_argument('--region-count', type=int, default=2, help='Number of regions')
    run_parser.add_argument('--connection-probability', type=float, default=0.15, help='Connection probability')
    run_parser.add_argument('--random-seed', type=int, help='Random seed')
    run_parser.add_argument('--steps', '-s', type=int, default=100, help='Number of simulation steps')
    run_parser.add_argument('--output', '-o', help='Output file')
    run_parser.add_argument('--visualize', action='store_true', help='Generate visualization')
    
    # Analyze command
    analyze_parser = subparsers.add_parser('analyze', help='Analyze simulation results')
    analyze_parser.add_argument('--input', '-i', required=True, help='Input results file')
    analyze_parser.add_argument('--output', '-o', help='Output file')
    analyze_parser.add_argument('--visualize', action='store_true', help='Generate visualization')
    
    # Compare command
    compare_parser = subparsers.add_parser('compare', help='Compare multiple simulations')
    compare_parser.add_argument('--input', '-i', nargs='+', required=True, help='Input results files')
    compare_parser.add_argument('--output', '-o', help='Output file')
    compare_parser.add_argument('--metrics', nargs='+', default=['firing_rate', 'weight_change'], help='Metrics to compare')
    
    # Batch command
    batch_parser = subparsers.add_parser('batch', help='Run batch simulations')
    batch_parser.add_argument('--config', '-c', required=True, help='Configuration file with parameter sweep')
    batch_parser.add_argument('--output-dir', '-o', default='batch_results', help='Output directory')
    
    # Report command
    report_parser = subparsers.add_parser('report', help='Generate reports')
    report_parser.add_argument('--input', '-i', required=True, help='Input results file')
    report_parser.add_argument('--report-type', '-t', default='comprehensive', help='Report type')
    report_parser.add_argument('--output', '-o', help='Output file')
    
    args = parser.parse_args()
    
    if args.command == 'run':
        run_command(args)
    elif args.command == 'analyze':
        analyze_command(args)
    elif args.command == 'compare':
        compare_command(args)
    elif args.command == 'batch':
        batch_command(args)
    elif args.command == 'report':
        report_command(args)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
