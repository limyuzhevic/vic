#!/usr/bin/env python3
"""
NLM Advanced Experiment Management System

This script provides advanced experiment management capabilities for NLM users,
including experiment tracking, reproducibility, and automated analysis.
"""

import argparse
import sys
import json
import time
import tempfile
import os
import shutil
from pathlib import Path
from datetime import datetime

# Add the python directory to path
sys.path.insert(0, str(Path(__file__).parent / 'python'))

try:
    import pynlm_enhanced
    print("✓ NLM Enhanced bindings available")
except ImportError:
    print("⚠ NLM Enhanced bindings not available")
    sys.exit(1)

class ExperimentManager:
    """Manage experiments with full reproducibility and tracking."""
    
    def __init__(self, base_dir="experiments"):
        self.base_dir = Path(base_dir)
        self.base_dir.mkdir(exist_ok=True)
        self.experiments = {}
        
    def create_experiment(self, name, description, parameters=None):
        """Create a new experiment."""
        experiment_id = f"exp_{int(time.time())}_{name.replace(' ', '_')}"
        
        experiment_dir = self.base_dir / experiment_id
        experiment_dir.mkdir(exist_ok=True)
        
        experiment = {
            'id': experiment_id,
            'name': name,
            'description': description,
            'parameters': parameters or {},
            'created_at': datetime.now().isoformat(),
            'status': 'initialized',
            'results': {},
            'metadata': {}
        }
        
        self.experiments[experiment_id] = experiment
        
        # Save experiment metadata
        with open(experiment_dir / 'metadata.json', 'w') as f:
            json.dump(experiment, f, indent=2)
        
        print(f"✓ Created experiment: {experiment_id}")
        print(f"  - Name: {name}")
        print(f"  - Directory: {experiment_dir}")
        
        return experiment_id
    
    def run_experiment(self, experiment_id, steps=1000):
        """Run an experiment."""
        if experiment_id not in self.experiments:
            print(f"✗ Experiment {experiment_id} not found")
            return None
        
        experiment = self.experiments[experiment_id]
        experiment_dir = self.base_dir / experiment_id
        
        try:
            # Create configuration from experiment parameters
            config = pynlm_enhanced.createEnhancedConfig()
            
            # Set experiment parameters
            for key, value in experiment['parameters'].items():
                config.set(f"experiment.{key}", value)
            
            # Set standard parameters
            config.set("brain.neuron_count", experiment['parameters'].get('neuron_count', 100))
            config.set("brain.synapse_density", experiment['parameters'].get('synapse_density', 0.1))
            
            # Create brain
            brain = pynlm_enhanced.createEnhancedBrain(config)
            brain.initialize()
            
            # Update experiment status
            experiment['status'] = 'running'
            experiment['start_time'] = datetime.now().isoformat()
            
            # Save updated metadata
            with open(experiment_dir / 'metadata.json', 'w') as f:
                json.dump(experiment, f, indent=2)
            
            print(f"✓ Running experiment: {experiment_id}")
            print(f"  - Steps: {steps}")
            print(f"  - Parameters: {experiment['parameters']}")
            
            # Run simulation with monitoring
            brain.startPerformanceMonitoring()
            
            start_time = time.time()
            
            for i in range(steps):
                brain.step(i)
                
                # Save intermediate results if needed
                if steps % 100 == 0:
                    self._save_intermediate_results(experiment_dir, brain, i)
            
            end_time = time.time()
            
            # Stop monitoring
            brain.stopPerformanceMonitoring()
            
            # Collect results
            metrics = brain.getPerformanceMetrics()
            
            experiment['results'] = {
                'simulation_time': end_time - start_time,
                'steps_completed': steps,
                'performance_metrics': {
                    'cpu_usage': metrics.cpuUsage,
                    'memory_usage': metrics.memoryUsage,
                    'spike_count': metrics.spikeCount,
                    'average_firing_rate': metrics.averageFiringRate,
                    'active_neuron_count': metrics.activeNeuronCount
                },
                'brain_stats': {
                    'total_neurons': brain.getBrain().getTotalNeuronCount(),
                    'total_spikes': brain.getBrain().getTotalSpikeCount(),
                    'average_firing_rate': brain.getBrain().getAverageFiringRate(),
                    'excitation_inhibition_ratio': brain.getBrain().getExcitationInhibitionRatio()
                }
            }
            
            experiment['end_time'] = datetime.now().isoformat()
            experiment['status'] = 'completed'
            
            # Save final results
            self._save_experiment_results(experiment_dir, experiment, brain)
            
            print(f"✓ Experiment completed: {experiment_id}")
            print(f"  - Simulation time: {experiment['results']['simulation_time']:.2f}s")
            print(f"  - Results saved to: {experiment_dir}")
            
            return experiment_id
            
        except Exception as e:
            experiment['status'] = 'failed'
            experiment['error'] = str(e)
            experiment['end_time'] = datetime.now().isoformat()
            
            with open(experiment_dir / 'metadata.json', 'w') as f:
                json.dump(experiment, f, indent=2)
            
            print(f"✗ Experiment failed: {experiment_id} - {e}")
            return None
    
    def _save_intermediate_results(self, experiment_dir, brain, step):
        """Save intermediate results."""
        step_dir = experiment_dir / f"step_{step}"
        step_dir.mkdir(exist_ok=True)
        
        # Save brain snapshot
        brain.save(str(step_dir / "brain_state.nlm"))
        
        # Save metrics
        metrics = {
            'step': step,
            'timestamp': datetime.now().isoformat(),
            'brain_stats': {
                'total_neurons': brain.getBrain().getTotalNeuronCount(),
                'total_spikes': brain.getBrain().getTotalSpikeCount(),
                'average_firing_rate': brain.getBrain().getAverageFiringRate()
            }
        }
        
        with open(step_dir / 'metrics.json', 'w') as f:
            json.dump(metrics, f, indent=2)
    
    def _save_experiment_results(self, experiment_dir, experiment, brain):
        """Save complete experiment results."""
        # Save experiment metadata
        with open(experiment_dir / 'metadata.json', 'w') as f:
            json.dump(experiment, f, indent=2)
        
        # Save brain snapshot
        brain.save(str(experiment_dir / 'final_brain_state.nlm'))
        
        # Save performance metrics
        brain.exportMetrics(str(experiment_dir / 'performance_metrics.csv'))
        
        # Generate experiment report
        self._generate_experiment_report(experiment_dir, experiment)
        
        # Save experiment parameters
        with open(experiment_dir / 'parameters.json', 'w') as f:
            json.dump(experiment['parameters'], f, indent=2)
        
        # Save results
        with open(experiment_dir / 'results.json', 'w') as f:
            json.dump(experiment['results'], f, indent=2)
    
    def _generate_experiment_report(self, experiment_dir, experiment):
        """Generate experiment report."""
        report = {
            'title': f"Experiment Report: {experiment['name']}",
            'description': experiment['description'],
            'experiment_id': experiment['id'],
            'created_at': experiment['created_at'],
            'start_time': experiment.get('start_time', experiment['created_at']),
            'end_time': experiment.get('end_time', experiment['created_at']),
            'status': experiment['status'],
            'parameters': experiment['parameters'],
            'results': experiment['results'],
            'error': experiment.get('error')
        }
        
        report_file = experiment_dir / 'report.md'
        
        with open(report_file, 'w') as f:
            f.write(f"# {report['title']}\n\n")
            f.write(f"**Description:** {report['description']}\n\n")
            f.write(f"**Experiment ID:** {report['experiment_id']}\n\n")
            f.write(f"**Created:** {report['created_at']}\n\n")
            f.write(f"**Start Time:** {report['start_time']}\n\n")
            f.write(f"**End Time:** {report['end_time']}\n\n")
            f.write(f"**Status:** {report['status']}\n\n")
            
            if report['error']:
                f.write(f"**Error:** {report['error']}\n\n")
            
            f.write("## Parameters\n\n")
            f.write("```json\n" + json.dumps(report['parameters'], indent=2) + "\n```\n\n")
            
            f.write("## Results\n\n")
            f.write("```json\n" + json.dumps(report['results'], indent=2) + "\n```\n\n")
            
            f.write("## Performance Metrics\n\n")
            if 'performance_metrics' in report['results']:
                perf = report['results']['performance_metrics']
                f.write(f"- **CPU Usage:** {perf['cpu_usage']:.1f}%\n")
                f.write(f"- **Memory Usage:** {perf['memory_usage']:.1f}%\n")
                f.write(f"- **Spike Count:** {perf['spike_count']}\n")
                f.write(f"- **Average Firing Rate:** {perf['average_firing_rate']:.3f} Hz\n")
                f.write(f"- **Active Neuron Count:** {perf['active_neuron_count']}\n")
            
            f.write("\n## Brain Statistics\n\n")
            if 'brain_stats' in report['results']:
                stats = report['results']['brain_stats']
                f.write(f"- **Total Neurons:** {stats['total_neurons']}\n")
                f.write(f"- **Total Spikes:** {stats['total_spikes']}\n")
                f.write(f"- **Average Firing Rate:** {stats['average_firing_rate']:.3f} Hz\n")
                f.write(f"- **E/I Ratio:** {stats['excitation_inhibition_ratio']:.3f}\n")
    
    def list_experiments(self):
        """List all experiments."""
        print(f"\n=== Experiment List ===")
        
        if not self.experiments:
            print("No experiments found.")
            return
        
        for exp_id, experiment in self.experiments.items():
            print(f"\n{exp_id}:")
            print(f"  - Name: {experiment['name']}")
            print(f"  - Description: {experiment['description']}")
            print(f"  - Status: {experiment['status']}")
            print(f"  - Created: {experiment['created_at']}")
            
            if experiment.get('start_time'):
                print(f"  - Start: {experiment['start_time']}")
            
            if experiment.get('end_time'):
                print(f"  - End: {experiment['end_time']}")
            
            if experiment.get('error'):
                print(f"  - Error: {experiment['error']}")
            
            if experiment.get('results'):
                print(f"  - Results: {list(experiment['results'].keys())}")
    
    def get_experiment(self, experiment_id):
        """Get experiment details."""
        if experiment_id not in self.experiments:
            print(f"✗ Experiment {experiment_id} not found")
            return None
        
        experiment = self.experiments[experiment_id]
        print(f"\n=== Experiment Details: {experiment_id} ===")
        print(f"Name: {experiment['name']}")
        print(f"Description: {experiment['description']}")
        print(f"Status: {experiment['status']}")
        print(f"Created: {experiment['created_at']}")
        print(f"Parameters: {json.dumps(experiment['parameters'], indent=2)}")
        
        if experiment.get('results'):
            print(f"\nResults:")
            print(f"  Simulation time: {experiment['results'].get('simulation_time', 'N/A'):.2f}s")
            print(f"  Steps completed: {experiment['results'].get('steps_completed', 'N/A')}")
            
            if 'performance_metrics' in experiment['results']:
                perf = experiment['results']['performance_metrics']
                print(f"  Performance Metrics:")
                print(f"    - CPU Usage: {perf['cpu_usage']:.1f}%")
                print(f"    - Memory Usage: {perf['memory_usage']:.1f}%")
                print(f"    - Spike Count: {perf['spike_count']}")
                print(f"    - Average Firing Rate: {perf['average_firing_rate']:.3f} Hz")
        
        if experiment.get('error'):
            print(f"\nError: {experiment['error']}")
        
        return experiment

def run_experiment_cli(args):
    """Run experiment management command."""
    manager = ExperimentManager(args.base_dir)
    
    if args.command == 'create':
        # Create experiment
        experiment_id = manager.create_experiment(
            args.name, args.description,
            {k: v for k, v in vars(args).items() if not k.startswith('_') and k != 'command' and k != 'base_dir' and k != 'name' and k != 'description'}
        )
        
        if experiment_id:
            print(f"✓ Experiment created. Running simulation...")
            run_result = manager.run_experiment(experiment_id, args.steps)
            
            if run_result:
                print(f"✓ Experiment completed successfully")
                return 0
            else:
                print(f"✗ Experiment failed")
                return 1
                
    elif args.command == 'list':
        manager.list_experiments()
        return 0
        
    elif args.command == 'get':
        experiment = manager.get_experiment(args.experiment_id)
        return 0 if experiment else 1
        
    elif args.command == 'run':
        # Run existing experiment
        run_result = manager.run_experiment(args.experiment_id, args.steps)
        return 0 if run_result else 1
    
    print(f"✗ Unknown command: {args.command}")
    return 1

def main():
    """Main experiment management interface."""
    parser = argparse.ArgumentParser(
        description="NLM Experiment Management System",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Commands:
  create     Create and run new experiment
  list      List all experiments
  get       Get experiment details
  run       Run existing experiment

Examples:
  # Create and run experiment
  python3 experiment_manager.py create "test_experiment" "Test neural network" --neuron-count 200 --steps 1000
  
  # List all experiments
  python3 experiment_manager.py list
  
  # Get experiment details
  python3 experiment_manager.py get exp_123456789
  
  # Run existing experiment
  python3 experiment_manager.py run exp_123456789 --steps 2000
        """
    )
    
    subparsers = parser.add_subparsers(dest='command', help='Available commands')
    
    # Create experiment
    create_parser = subparsers.add_parser('create', help='Create and run new experiment')
    create_parser.add_argument('name', help='Name of the experiment')
    create_parser.add_argument('description', help='Description of the experiment')
    create_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    create_parser.add_argument('--base-dir', default='experiments', help='Base directory for experiments')
    create_parser.add_argument('--neuron-count', type=int, default=100, help='Number of neurons')
    create_parser.add_argument('--synapse-density', type=float, default=0.1, help='Synapse density')
    create_parser.add_argument('--verbose', action='store_true', help='Verbose output')
    
    # List experiments
    list_parser = subparsers.add_parser('list', help='List all experiments')
    list_parser.add_argument('--base-dir', default='experiments', help='Base directory for experiments')
    
    # Get experiment details
    get_parser = subparsers.add_parser('get', help='Get experiment details')
    get_parser.add_argument('experiment_id', help='Experiment ID')
    get_parser.add_argument('--base-dir', default='experiments', help='Base directory for experiments')
    
    # Run existing experiment
    run_parser = subparsers.add_parser('run', help='Run existing experiment')
    run_parser.add_argument('experiment_id', help='Experiment ID')
    run_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
    run_parser.add_argument('--base-dir', default='experiments', help='Base directory for experiments')
    
    # Parse arguments
    args = parser.parse_args()
    
    if not args.command:
        parser.print_help()
        return 1
    
    return run_experiment_cli(args)

if __name__ == "__main__":
    sys.exit(main())