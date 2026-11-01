#!/usr/bin/env python3
"""
NLM (Neural Learning Machine) - Main Command Line Interface
A comprehensive tool for working with NLM neural simulations
"""

import argparse
import sys
import os
from pathlib import Path
import json
import time
import logging
from typing import Dict, Any, List, Optional
import numpy as np
import matplotlib.pyplot as plt

# Import our tool modules
sys.path.insert(0, str(Path(__file__).parent))

from tools import NLMCli, create_config, run_brain_simulation, visualize_brain_state, interactive_explore
from visualization import BrainVisualizer, SimplePlotter, GraphRenderer
from experiments import ExperimentRunner, Phase6Experiment

# Setup logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.StreamHandler(sys.stdout),
        logging.FileHandler('nlm_cli.log')
    ]
)

logger = logging.getLogger(__name__)

class NLMCLI:
    """Main NLM command line interface with all tools integrated"""
    
    def __init__(self):
        self.cli = NLMCli()
        self.visualizer = BrainVisualizer()
        self.plotter = SimplePlotter()
        self.graph_renderer = GraphRenderer()
        self.experiment_runner = ExperimentRunner()
        self.phase6_experiment = Phase6Experiment()
        
    def run(self, args):
        """Run the appropriate command"""
        try:
            if args.command == 'create-config':
                self._handle_create_config(args)
            elif args.command == 'run':
                self._handle_run(args)
            elif args.command == 'visualize':
                self._handle_visualize(args)
            elif args.command == 'benchmark':
                self._handle_benchmark(args)
            elif args.command == 'explore':
                self._handle_explore(args)
            elif args.command == 'experiments':
                self._handle_experiments(args)
            elif args.command == 'visualize-dashboard':
                self._handle_visualize_dashboard(args)
            elif args.command == 'create-graph':
                self._handle_create_graph(args)
            elif args.command == 'run-phase6':
                self._handle_phase6(args)
            elif args.command == 'list-experiments':
                self._handle_list_experiments(args)
            elif args.command == 'analyze-experiment':
                self._handle_analyze_experiment(args)
            else:
                self._show_help()
                
        except Exception as e:
            logger.error(f"Error executing command {args.command}: {e}")
            print(f"Error: {e}")
            sys.exit(1)
    
    def _handle_create_config(self, args):
        """Handle create-config command"""
        logger.info("Creating brain configuration")
        self.cli.create_config_command(args)
        print("Configuration created successfully")
    
    def _handle_run(self, args):
        """Handle run command"""
        logger.info(f"Running brain simulation with {args.steps} steps")
        self.cli.run_command(args)
    
    def _handle_visualize(self, args):
        """Handle visualize command"""
        logger.info(f"Visualizing brain state from {args.checkpoint}")
        self.cli.visualize_command(args)
    
    def _handle_benchmark(self, args):
        """Handle benchmark command"""
        logger.info(f"Running benchmark with {args.neurons} neurons")
        self.cli.benchmark_command(args)
    
    def _handle_explore(self, args):
        """Handle explore command"""
        logger.info("Starting interactive exploration")
        self.cli.explore_command(args)
    
    def _handle_experiments(self, args):
        """Handle experiments command"""
        if args.subcommand == 'create':
            self._handle_create_experiment(args)
        elif args.subcommand == 'run':
            self._handle_run_experiment(args)
        elif args.subcommand == 'list':
            self._handle_list_experiments(args)
        else:
            self._show_experiments_help()
    
    def _handle_visualize_dashboard(self, args):
        """Handle visualize-dashboard command"""
        logger.info("Creating comprehensive brain dashboard")
        
        # This would normally load a brain and create visualization
        # For now, we'll create a sample dashboard
        try:
            # Simulate dashboard creation
            dashboard_file = args.output if hasattr(args, 'output') else "brain_dashboard.png"
            
            # Create a simple test visualization
            fig, ax = plt.subplots(figsize=(10, 6))
            
            # Generate sample data
            x = np.linspace(0, 10, 100)
            y1 = np.sin(x) * np.exp(-x/5)
            y2 = np.cos(x) * np.exp(-x/3)
            
            ax.plot(x, y1, label='Neural Activity', linewidth=2)
            ax.plot(x, y2, label='Plasticity Change', linewidth=2)
            ax.set_xlabel('Time Step')
            ax.set_ylabel('Amplitude')
            ax.set_title('NLM Brain Dashboard - Sample Data')
            ax.legend()
            ax.grid(True, alpha=0.3)
            
            plt.tight_layout()
            plt.savefig(dashboard_file, dpi=150, bbox_inches='tight')
            plt.close()
            
            print(f"Dashboard saved to: {dashboard_file}")
            
        except Exception as e:
            print(f"Error creating dashboard: {e}")
            sys.exit(1)
    
    def _handle_create_graph(self, args):
        """Handle create-graph command"""
        logger.info(f"Creating graph visualization: {args.graph_type}")
        
        try:
            graph_file = args.output if hasattr(args, 'output') else "brain_graph.png"
            self.graph_renderer.render_graph_image(None, args.graph_type, graph_file)
            print(f"Graph saved to: {graph_file}")
            
        except Exception as e:
            print(f"Error creating graph: {e}")
            sys.exit(1)
    
    def _handle_phase6(self, args):
        """Handle phase6 command"""
        logger.info("Running Phase 6 integration experiment")
        
        try:
            result = self.phase6_experiment.run_integration_test()
            
            if result.success:
                print(f"Phase 6 experiment completed successfully!")
                print(f"Duration: {result.end_time - result.start_time}")
                print(f"Checkpoints saved: {len(result.checkpoints)}")
            else:
                print(f"Phase 6 experiment failed: {result.error_message}")
                
        except Exception as e:
            print(f"Error running Phase 6 experiment: {e}")
            sys.exit(1)
    
    def _handle_create_experiment(self, args):
        """Handle create-experiment subcommand"""
        logger.info(f"Creating experiment: {args.name}")
        
        experiment_config = {
            "name": args.name,
            "description": args.description,
            "brain_config": args.brain_config or {},
            "world_config": args.world_config or {},
            "duration": args.duration,
            "steps_per_iteration": args.steps_per_iteration,
            "save_checkpoints": args.save_checkpoints,
            "checkpoint_interval": args.checkpoint_interval,
            "metrics_to_record": args.metrics or [],
            "parallel_runs": args.parallel_runs,
            "max_memory_usage": args.max_memory
        }
        
        if self.experiment_runner.create_experiment(experiment_config):
            print(f"Experiment '{args.name}' created successfully")
        else:
            print(f"Failed to create experiment '{args.name}'")
    
    def _handle_run_experiment(self, args):
        """Handle run-experiment subcommand"""
        logger.info(f"Running experiment: {args.name}")
        
        result = self.experiment_runner.run_experiment(args.name)
        
        if result.success:
            print(f"Experiment '{args.name}' completed successfully")
            print(f"Duration: {result.end_time - result.start_time}")
            print(f"Metrics collected: {len(result.metrics.get('metrics', {}))}")
        else:
            print(f"Experiment '{args.name}' failed: {result.error_message}")
    
    def _handle_list_experiments(self, args):
        """Handle list-experiments subcommand"""
        logger.info("Listing experiments")
        
        experiments = self.experiment_runner.experiments
        results = self.experiment_runner.results_history
        
        print(f"\nTotal Experiments: {len(experiments)}")
        print(f"Total Results: {len(results)}\n")
        
        for name, config in experiments.items():
            result = None
            for r in results:
                if r.experiment_name == name:
                    result = r
                    break
            
            status = "Completed" if result and result.success else "Not Started"
            duration = f"{result.end_time - result.start_time}" if result else "N/A"
            
            print(f"Experiment: {name}")
            print(f"  Description: {config.description}")
            print(f"  Duration: {config.duration} iterations")
            print(f"  Status: {status}")
            print(f"  Runtime: {duration}")
            print(f"  Checkpoints: {len(result.checkpoints) if result else 0}")
            print()
    
    def _handle_analyze_experiment(self, args):
        """Handle analyze-experiment subcommand"""
        logger.info(f"Analyzing experiment: {args.name}")
        
        analysis = self.experiment_runner.analyze_results(args.name, args.analysis_type)
        
        print(f"\nAnalysis for experiment '{args.name}':")
        for key, value in analysis.items():
            print(f"  {key}: {value}")
    
    def _show_help(self):
        """Show general help"""
        print("""
NLM (Neural Learning Machine) Command Line Interface

Usage: nlm <command> [options]

Commands:
  create-config    Create a new brain configuration
  run             Run a brain simulation
  visualize       Visualize brain state from checkpoint
  benchmark       Run performance benchmark
  explore         Start interactive brain exploration
  experiments     Manage experiments (create, run, list, analyze)
  visualize-dashboard Create comprehensive brain dashboard
  create-graph    Create graph visualization
  run-phase6      Run Phase 6 integration experiment
  list-experiments List all experiments
  analyze-experiment Analyze experiment results

Use 'nlm <command> --help' for detailed help on specific commands
        """)
    
    def _show_experiments_help(self):
        """Show experiments help"""
        print("""
Experiment Management:

Subcommands:
  create   Create a new experiment
  run      Run an experiment
  list     List all experiments
  analyze  Analyze experiment results

Usage for 'create':
  nlm experiments create --name <name> --description <desc>
                         --duration <steps> --brain-config <json>
                         --world-config <json>

Usage for 'run':
  nlm experiments run --name <name>

Usage for 'list':
  nlm experiments list

Usage for 'analyze':
  nlm experiments analyze --name <name> --type <basic|comprehensive|performance|trends>
        """)

def create_parser():
    """Create the main argument parser"""
    parser = argparse.ArgumentParser(
        description="NLM (Neural Learning Machine) - Command Line Interface",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create a new brain configuration
  nlm create-config --neurons 1000 --regions 4 --excitatory-ratio 0.85
  
  # Run a brain simulation
  nlm run --config my_config.json --steps 500 --checkpoint brain_state.bin
  
  # Visualize brain state
  nlm visualize --checkpoint brain_state.bin --output visualization.png
  
  # Run performance benchmark
  nlm benchmark --neurons 2000 --duration 20 --output benchmark_results.json
  
  # Start interactive exploration
  nlm explore --steps 1000 --auto-save 200
  
  # Create comprehensive dashboard
  nlm visualize-dashboard --output dashboard.png
  
  # Create network graph visualization
  nlm create-graph --graph-type network --output brain_network.png
  
  # Run Phase 6 integration experiment
  nlm run-phase6
  
  # Experiment management
  nlm experiments create --name my_exp --description "My experiment"
  nlm experiments run --name my_exp
  nlm experiments list
  nlm experiments analyze --name my_exp --type comprehensive

For more detailed help on any command, use: nlm <command> --help
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create config command
    config_parser = subparsers.add_parser('create-config', help='Create a new brain configuration')
    config_parser.add_argument('--output', '-o', default='brain_config.json', 
                              help='Output configuration file path (default: brain_config.json)')
    config_parser.add_argument('--neurons', type=int, default=500,
                              help='Number of neurons (default: 500)')
    config_parser.add_argument('--regions', type=int, default=2,
                              help='Number of brain regions (default: 2)')
    config_parser.add_argument('--excitatory-ratio', type=float, default=0.8,
                              help='Ratio of excitatory neurons (default: 0.8)')
    
    # Run brain command
    run_parser = subparsers.add_parser('run', help='Run a brain simulation')
    run_parser.add_argument('--config', '-c', required=True,
                           help='Configuration file path')
    run_parser.add_argument('--steps', '-s', type=int, default=100,
                           help='Number of simulation steps (default: 100)')
    run_parser.add_argument('--checkpoint', '-k', 
                           help='Checkpoint file path for saving state')
    run_parser.add_argument('--load-checkpoint', 
                           help='Load brain state from checkpoint')
    run_parser.add_argument('--verbose', '-v', action='store_true',
                           help='Enable verbose output')
    run_parser.add_argument('--csv-output', 
                           help='Output metrics to CSV file')
    
    # Visualize command
    viz_parser = subparsers.add_parser('visualize', help='Visualize brain state')
    viz_parser.add_argument('--checkpoint', '-k', required=True,
                           help='Checkpoint file path')
    viz_parser.add_argument('--output', '-o', default='brain_viz.png',
                           help='Output image file path')
    viz_parser.add_argument('--width', type=int, default=1200,
                           help='Image width in pixels')
    viz_parser.add_argument('--height', type=int, default=800,
                           help='Image height in pixels')
    viz_parser.add_argument('--show-statistics', action='store_true',
                           help='Include statistics overlay')
    
    # Benchmark command
    bench_parser = subparsers.add_parser('benchmark', help='Run performance benchmark')
    bench_parser.add_argument('--neurons', type=int, default=1000,
                             help='Number of neurons to benchmark (default: 1000)')
    bench_parser.add_argument('--duration', type=int, default=10,
                             help='Benchmark duration in seconds (default: 10)')
    bench_parser.add_argument('--warmup', type=int, default=5,
                             help='Warm-up time in seconds (default: 5)')
    bench_parser.add_argument('--output', '-o', default='benchmark_results.json',
                             help='Output JSON file for results')
    
    # Explore command
    explore_parser = subparsers.add_parser('explore', help='Interactive brain exploration')
    explore_parser.add_argument('--checkpoint', '-k',
                               help='Checkpoint file path (start fresh if not provided)')
    explore_parser.add_argument('--steps', '-s', type=int, default=100,
                               help='Number of steps to run (default: 100)')
    explore_parser.add_argument('--auto-save', type=int, default=100,
                               help='Auto-save checkpoint every N steps (default: 100)')
    
    # Experiments command
    experiments_parser = subparsers.add_parser('experiments', help='Manage experiments')
    exp_subparsers = experiments_parser.add_subparsers(dest='subcommand', 
                                                      help='Experiment subcommands')
    
    # Create experiment
    create_exp_parser = exp_subparsers.add_parser('create', help='Create a new experiment')
    create_exp_parser.add_argument('--name', required=True, help='Experiment name')
    create_exp_parser.add_argument('--description', default='', help='Experiment description')
    create_exp_parser.add_argument('--duration', type=int, default=100, help='Duration in iterations')
    create_exp_parser.add_argument('--steps-per-iteration', type=int, default=10, 
                                  help='Steps per iteration')
    create_exp_parser.add_argument('--brain-config', default='{}', help='Brain configuration JSON')
    create_exp_parser.add_argument('--world-config', default='{}', help='World configuration JSON')
    create_exp_parser.add_argument('--save-checkpoints', action='store_true', help='Save checkpoints')
    create_exp_parser.add_argument('--checkpoint-interval', type=int, default=50, help='Checkpoint interval')
    create_exp_parser.add_argument('--metrics', nargs='+', default=[], help='Metrics to record')
    create_exp_parser.add_argument('--parallel-runs', type=int, default=1, help='Parallel runs')
    create_exp_parser.add_argument('--max-memory', type=float, default=1024, help='Max memory usage in MB')
    
    # Run experiment
    run_exp_parser = exp_subparsers.add_parser('run', help='Run an experiment')
    run_exp_parser.add_argument('--name', required=True, help='Experiment name')
    
    # List experiments
    list_exp_parser = exp_subparsers.add_parser('list', help='List all experiments')
    
    # Analyze experiment
    analyze_exp_parser = exp_subparsers.add_parser('analyze', help='Analyze experiment results')
    analyze_exp_parser.add_argument('--name', required=True, help='Experiment name')
    analyze_exp_parser.add_argument('--type', choices=['basic', 'comprehensive', 'performance', 'trends'],
                                   default='basic', help='Analysis type')
    
    # Visualize dashboard
    dashboard_parser = subparsers.add_parser('visualize-dashboard', help='Create comprehensive brain dashboard')
    dashboard_parser.add_argument('--output', '-o', default='brain_dashboard.png',
                                 help='Output image file path')
    
    # Create graph
    graph_parser = subparsers.add_parser('create-graph', help='Create graph visualization')
    graph_parser.add_argument('--graph-type', choices=['network', 'activity', 'connections'],
                             default='network', help='Type of graph to create')
    graph_parser.add_argument('--output', '-o', default='brain_graph.png',
                             help='Output image file path')
    
    # Phase 6
    phase6_parser = subparsers.add_parser('run-phase6', help='Run Phase 6 integration experiment')
    
    # List experiments (general)
    list_gen_parser = subparsers.add_parser('list-experiments', help='List all experiments')
    
    # Analyze experiment (general)
    analyze_gen_parser = subparsers.add_parser('analyze-experiment', help='Analyze experiment results')
    analyze_gen_parser.add_argument('--name', required=True, help='Experiment name')
    analyze_gen_parser.add_argument('--type', choices=['basic', 'comprehensive', 'performance', 'trends'],
                                   default='basic', help='Analysis type')
    
    return parser

def main():
    """Main entry point"""
    parser = create_parser()
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return
    
    # Create CLI instance and run
    nlm_cli = NLMCLI()
    nlm_cli.run(args)

if __name__ == '__main__':
    main()