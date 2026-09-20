#!/usr/bin/env python3
"""
Experiment Management Commands for NLM CLI

Handles experiment creation, running, management, and result comparison.
"""

import json
import time
import csv
import statistics
from pathlib import Path
from typing import Dict, Any, Optional, List
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax
from rich.progress import Progress, SpinnerColumn, TextColumn, BarColumn

from nlm_cli import NLMCLI

console = Console()

class ExperimentCommands:
    """Handle experiment management commands."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle experiment commands."""
        command = args.get('command')
        
        handlers = {
            'create': self.create_experiment,
            'run': self.run_experiment,
            'list': self.list_experiments,
            'show': self.show_experiment,
            'compare': self.compare_experiments,
            'results': self.get_experiment_results,
            'export': self.export_experiment_results,
            'import': self.import_experiment_results,
            'delete': self.delete_experiment,
            'pause': self.pause_experiment,
            'resume': self.resume_experiment,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown experiment command: {command}")
    
    def create_experiment(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Create a new experiment."""
        name = args.get('name')
        description = args.get('description', '')
        brain_name = args.get('brain_name')
        config_name = args.get('config_name')
        parameters = args.get('parameters', {})
        
        # Get brain configuration if specified
        config = {}
        if config_name:
            configs = self.nlm_cli.load_config()
            if config_name in configs:
                config = configs[config_name].get('parameters', {})
            else:
                raise click.ClickException(f"Configuration '{config_name}' not found.")
        
        # Merge with provided parameters
        config = {**config, **parameters}
        
        # Create experiment configuration
        experiment = {
            'name': name,
            'description': description,
            'created_at': datetime.now().isoformat(),
            'last_modified': datetime.now().isoformat(),
            'brain_name': brain_name,
            'config': config,
            'status': 'created',
            'parameters': parameters,
            'results': None,
            'metadata': {}
        }
        
        # Store experiment in context
        if 'experiments' not in self.nlm_cli.context:
            self.nlm_cli.context['experiments'] = {}
        
        self.nlm_cli.context['experiments'][name] = experiment
        
        return {
            'success': True,
            'message': f"Experiment '{name}' created successfully.",
            'experiment': experiment
        }
    
    def run_experiment(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Run an experiment."""
        name = args.get('name')
        steps = args.get('steps', 100)
        time_scale = args.get('time_scale', 1.0)
        auto_step = args.get('auto_step', False)
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name}' not found.")
        
        experiment = self.nlm_cli.context['experiments'][name]
        
        if experiment['status'] != 'created':
            raise click.ClickException(f"Experiment '{name}' is already running or completed.")
        
        # Update status
        experiment['status'] = 'running'
        experiment['start_time'] = datetime.now().isoformat()
        experiment['parameters']['steps'] = steps
        experiment['parameters']['time_scale'] = time_scale
        
        try:
            # Get brain from context
            brain = self.nlm_cli.context.get('brain')
            if not brain:
                raise click.ClickException("No brain loaded. Create or load a brain first.")
            
            # Create world and agent if not present
            if 'world' not in self.nlm_cli.context:
                from pynlm.enhanced_api import create_enhanced_world
                world = create_enhanced_world()
                self.nlm_cli.context['world'] = world
            
            if 'agent' not in self.nlm_cli.context:
                from pynlm.enhanced_api import create_enhanced_agent
                agent = create_enhanced_agent(brain)
                self.nlm_cli.context['agent'] = agent
            
            world = self.nlm_cli.context['world']
            agent = self.nlm_cli.context['agent']
            
            # Update simulation controls
            from pynlm.enhanced_api import SimulationControls
            simulation = SimulationControls(brain, world, agent)
            self.nlm_cli.context['simulation'] = simulation
            
            # Run simulation with progress
            click.echo(f"Running experiment '{name}' for {steps} steps...")
            
            with Progress(
                SpinnerColumn(),
                TextColumn("[progress.description]{task.description}"),
                BarColumn(),
                TextColumn("[progress.percentage]{task.percentage:>3.0f}%"),
                console=console
            ) as progress:
                task = progress.add_task(f"Running {name}...", total=steps)
                
                stats_over_time = []
                
                for step in range(steps):
                    simulation.step(step)
                    
                    # Collect statistics
                    from pynlm.enhanced_api import BrainStats
                    brain_stats = BrainStats(brain)
                    stats_over_time.append({
                        'step': step,
                        'timestamp': datetime.now().isoformat(),
                        'stats': brain_stats.get_summary()
                    })
                    
                    progress.update(task, advance=1)
                    time.sleep(0.01)
            
            # Update experiment with results
            experiment['status'] = 'completed'
            experiment['end_time'] = datetime.now().isoformat()
            experiment['results'] = {
                'steps_completed': steps,
                'stats_over_time': stats_over_time,
                'final_stats': stats_over_time[-1]['stats'] if stats_over_time else {}
            }
            
            # Store experiment in persistent storage
            self.save_experiment(experiment)
            
            return {
                'success': True,
                'message': f"Experiment '{name}' completed successfully.",
                'experiment': experiment
            }
        except Exception as e:
            experiment['status'] = 'failed'
            experiment['error'] = str(e)
            experiment['end_time'] = datetime.now().isoformat()
            
            return {
                'success': False,
                'message': f"Failed to run experiment '{name}': {e}",
                'error': str(e),
                'experiment': experiment
            }
    
    def pause_experiment(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Pause the current experiment."""
        name = args.get('name')
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name}' not found.")
        
        experiment = self.nlm_cli.context['experiments'][name]
        
        if experiment['status'] != 'running':
            raise click.ClickException(f"Experiment '{name}' is not running.")
        
        experiment['status'] = 'paused'
        
        # Pause simulation if running
        simulation = self.nlm_cli.context.get('simulation')
        if simulation:
            simulation.pause()
        
        return {
            'success': True,
            'message': f"Experiment '{name}' paused successfully.",
            'experiment': experiment
        }
    
    def resume_experiment(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Resume the current experiment."""
        name = args.get('name')
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name}' not found.")
        
        experiment = self.nlm_cli.context['experiments'][name]
        
        if experiment['status'] != 'paused':
            raise click.ClickException(f"Experiment '{name}' is not paused.")
        
        experiment['status'] = 'running'
        
        # Resume simulation if paused
        simulation = self.nlm_cli.context.get('simulation')
        if simulation and hasattr(simulation, 'resume'):
            simulation.resume()
        
        return {
            'success': True,
            'message': f"Experiment '{name}' resumed successfully.",
            'experiment': experiment
        }
    
    def list_experiments(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """List experiments."""
        name = args.get('name')
        all_flag = args.get('all', False)
        
        experiments = {}
        
        # Try to load from context first
        if 'experiments' in self.nlm_cli.context:
            experiments = self.nlm_cli.context['experiments']
        
        # Try to load from persistent storage
        if not experiments:
            experiments_file = Path.home() / '.nlm' / 'experiments.json'
            if experiments_file.exists():
                try:
                    with open(experiments_file, 'r') as f:
                        experiments = json.load(f)
                except Exception:
                    experiments = {}
        
        if not experiments:
            return {
                'success': True,
                'message': 'No experiments found.',
                'experiments': []
            }
        
        if name:
            if name in experiments:
                return {
                    'success': True,
                    'message': f"Experiment '{name}' found.",
                    'experiment': experiments[name]
                }
            else:
                return {
                    'success': False,
                    'message': f"Experiment '{name}' not found.",
                    'error': 'Experiment not found'
                }
        
        if all_flag:
            table = Table(title="All Experiments")
            table.add_column("Name", style="cyan")
            table.add_column("Description", style="green")
            table.add_column("Status", style="yellow")
            table.add_column("Created", style="magenta")
            table.add_column("Brain", style="blue")
            
            for exp_name, exp in experiments.items():
                status = exp.get('status', 'unknown')
                created = exp.get('created_at', 'Unknown')[:10]
                brain = exp.get('brain_name', 'None')
                description = exp.get('description', 'No description')[:30] + (
                    '...' if len(exp.get('description', '')) > 30 else '')
                
                table.add_row(exp_name, description, status, created, brain)
            
            console.print(table)
            return {
                'success': True,
                'message': f"Found {len(experiments)} experiments.",
                'experiments': list(experiments.values())
            }
        
        # Simple list
        console.print(f"{'Name':<20} {'Status':<10} {'Created':<12} {'Brain':<15}")
        console.print('-' * 62)
        for exp_name, exp in experiments.items():
            status = exp.get('status', 'unknown')
            created = exp.get('created_at', 'Unknown')[:10]
            brain = exp.get('brain_name', 'None')[:13]
            
            console.print(f"{exp_name:<20} {status:<10} {created:<12} {brain:<15}")
        
        return {
            'success': True,
            'message': f"Found {len(experiments)} experiments.",
            'experiments': list(experiments.values())[:10]  # Limit to first 10 for output
        }
    
    def show_experiment(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show experiment details."""
        name = args.get('name')
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name}' not found.")
        
        experiment = self.nlm_cli.context['experiments'][name]
        
        console.print(f"Experiment: {experiment['name']}")
        console.print("=" * 50)
        console.print(f"Description: {experiment.get('description', 'No description')}")
        console.print(f"Status: {experiment.get('status', 'unknown')}")
        console.print(f"Created: {experiment.get('created_at', 'Unknown')}")
        console.print(f"Last Modified: {experiment.get('last_modified', 'Unknown')}")
        console.print(f"Brain: {experiment.get('brain_name', 'None')}")
        
        if experiment.get('results'):
            results = experiment['results']
            console.print(f"\nResults:")
            console.print(f"  Steps Completed: {results.get('steps_completed', 0)}")
            console.print(f"  Final Neurons: {results.get('final_stats', {}).get('total_neurons', 0)}")
            console.print(f"  Final Synapses: {results.get('final_stats', {}).get('total_synapses', 0)}")
            
            if 'start_time' in experiment:
                start_time = datetime.fromisoformat(experiment['start_time'])
                end_time = datetime.fromisoformat(experiment.get('end_time', datetime.now().isoformat()))
                duration = (end_time - start_time).total_seconds()
                console.print(f"  Duration: {duration:.2f} seconds")
        
        if experiment.get('parameters'):
            console.print(f"\nParameters:")
            syntax = Syntax(
                json.dumps(experiment['parameters'], indent=2),
                'json',
                theme='default'
            )
            console.print(syntax)
        
        return {
            'success': True,
            'message': f"Experiment '{name}' displayed.",
            'experiment': experiment
        }
    
    def compare_experiments(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Compare experiment results."""
        name1 = args.get('name1')
        name2 = args.get('name2')
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name1 not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name1}' not found.")
        
        if name2 not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name2}' not found.")
        
        exp1 = self.nlm_cli.context['experiments'][name1]
        exp2 = self.nlm_cli.context['experiments'][name2]
        
        results1 = exp1.get('results')
        results2 = exp2.get('results')
        
        if not results1:
            raise click.ClickException(f"Experiment '{name1}' has no results.")
        
        if not results2:
            raise click.ClickException(f"Experiment '{name2}' has no results.")
        
        stats1 = results1.get('final_stats', {})
        stats2 = results2.get('final_stats', {})
        
        table = Table(title="Experiment Comparison")
        table.add_column("Metric", style="cyan")
        table.add_column(f"{name1}", style="green")
        table.add_column(f"{name2}", style="yellow")
        table.add_column("Difference", style="magenta")
        
        comparison_metrics = [
            ('Total Neurons', 'total_neurons', stats1.get('total_neurons', 0), stats2.get('total_neurons', 0)),
            ('Total Synapses', 'total_synapses', stats1.get('total_synapses', 0), stats2.get('total_synapses', 0)),
            ('Active Neurons', 'active_neurons', stats1.get('active_neurons', 0), stats2.get('active_neurons', 0)),
            ('Firing Neurons', 'firing_neurons', stats1.get('firing_neurons', 0), stats2.get('firing_neurons', 0)),
            ('Total Spikes', 'total_spikes', stats1.get('total_spikes', 0), stats2.get('total_spikes', 0)),
            ('Firing Rate (Hz)', 'average_firing_rate', stats1.get('average_firing_rate', 0), stats2.get('average_firing_rate', 0)),
            ('E/I Ratio', 'excitation_inhibition_ratio', stats1.get('excitation_inhibition_ratio', 0), stats2.get('excitation_inhibition_ratio', 0)),
        ]
        
        for metric_name, key, value1, value2 in comparison_metrics:
            diff = value2 - value1
            table.add_row(metric_name, str(value1), str(value2), str(diff))
        
        console.print(table)
        
        # Calculate similarity
        similarity = 0
        if stats1 and stats2:
            common_metrics = ['total_neurons', 'total_synapses', 'total_spikes', 'average_firing_rate']
            matched = 0
            for metric in common_metrics:
                val1 = stats1.get(metric, 0)
                val2 = stats2.get(metric, 0)
                if val1 == 0 and val2 == 0:
                    matched += 1
                elif val1 > 0 and val2 > 0:
                    matched += abs(val1 - val2) / max(val1, val2)
            
            similarity = matched / len(common_metrics)
        
        return {
            'success': True,
            'message': f"Comparison between '{name1}' and '{name2}' completed.",
            'comparison': {
                'name1': name1,
                'name2': name2,
                'stats1': stats1,
                'stats2': stats2,
                'similarity': similarity
            },
            'experiments': [exp1, exp2]
        }
    
    def get_experiment_results(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Get experiment results."""
        name = args.get('name')
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name}' not found.")
        
        experiment = self.nlm_cli.context['experiments'][name]
        
        return {
            'success': True,
            'message': f"Results for experiment '{name}' retrieved.",
            'experiment': experiment
        }
    
    def export_experiment_results(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Export experiment results to file."""
        name = args.get('name')
        output_path = args.get('output')
        format = args.get('format', 'json')
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name}' not found.")
        
        experiment = self.nlm_cli.context['experiments'][name]
        
        export_data = {
            'export_timestamp': datetime.now().isoformat(),
            'export_version': '1.0',
            'experiment': experiment,
            'format': 'nlm_experiment'
        }
        
        if not output_path:
            output_path = f"{name}_results.{format}"
        
        output = Path(output_path)
        output.parent.mkdir(parents=True, exist_ok=True)
        
        with open(output, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        return {
            'success': True,
            'message': f"Experiment results for '{name}' exported to {output}.",
            'experiment_name': name,
            'output_path': str(output),
            'format': format
        }
    
    def import_experiment_results(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Import experiment results from file."""
        input_path = args.get('input')
        name = args.get('name')
        force = args.get('force', False)
        
        if not input_path:
            raise click.ClickException("Input file is required for importing experiment results.")
        
        if not Path(input_path).exists():
            raise click.ClickException(f"Input file not found: {input_path}")
        
        with open(input_path, 'r') as f:
            import_data = json.load(f)
        
        # Extract experiment from import data
        if 'experiment' in import_data:
            experiment = import_data['experiment']
        elif 'exp' in import_data:
            experiment = import_data['exp']
        else:
            experiment = import_data
        
        exp_name = name or experiment.get('name', 'imported_experiment')
        
        if 'experiments' not in self.nlm_cli.context:
            self.nlm_cli.context['experiments'] = {}
        
        if exp_name in self.nlm_cli.context['experiments'] and not force:
            if not click.confirm(f"Experiment '{exp_name}' already exists. Overwrite?", default=False):
                return {
                    'success': False,
                    'message': "Operation cancelled."
                }
        
        experiment['last_modified'] = datetime.now().isoformat()
        self.nlm_cli.context['experiments'][exp_name] = experiment
        
        return {
            'success': True,
            'message': f"Experiment results imported from {input_path}.",
            'experiment_name': exp_name,
            'experiment': experiment
        }
    
    def delete_experiment(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Delete an experiment."""
        name = args.get('name')
        
        if 'experiments' not in self.nlm_cli.context:
            raise click.ClickException("No experiments found.")
        
        if name not in self.nlm_cli.context['experiments']:
            raise click.ClickException(f"Experiment '{name}' not found.")
        
        del self.nlm_cli.context['experiments'][name]
        
        # Also remove from persistent storage if exists
        experiments_file = Path.home() / '.nlm' / 'experiments.json'
        if experiments_file.exists():
            try:
                with open(experiments_file, 'r') as f:
                    experiments = json.load(f)
                
                if name in experiments:
                    del experiments[name]
                    
                    with open(experiments_file, 'w') as f:
                        json.dump(experiments, f, indent=2, default=str)
            except Exception:
                pass
        
        return {
            'success': True,
            'message': f"Experiment '{name}' deleted successfully.",
            'deleted_name': name
        }
    
    def save_experiment(self, experiment: Dict[str, Any]):
        """Save experiment to persistent storage."""
        experiments_file = Path.home() / '.nlm' / 'experiments.json'
        
        # Load existing experiments
        if experiments_file.exists():
            try:
                with open(experiments_file, 'r') as f:
                    experiments = json.load(f)
            except Exception:
                experiments = {}
        else:
            experiments = {}
        
        # Add/update experiment
        experiments[experiment['name']] = experiment
        
        # Save back
        experiments_file.parent.mkdir(parents=True, exist_ok=True)
        with open(experiments_file, 'w') as f:
            json.dump(experiments, f, indent=2, default=str)