# NLM Command-Line Utilities for Experimentation

## Overview

This module provides comprehensive command-line utilities for running, monitoring, and managing NLM experiments. It includes experiment launchers, configuration management, benchmarking tools, and experiment comparison utilities.

---

## 1. Core Experiment CLI

### 1.1 Experiment Runner

```python
import pynlm
import argparse
import time
import json
import os
from pathlib import Path
from typing import Optional, Dict, Any, List
from enum import Enum

class ExperimentMode(Enum):
    SINGLE_RUN = "single"
    CONTINUOUS = "continuous"
    BENCHMARK = "benchmark"
    REPLAY = "replay"

class ExperimentCLI:
    """Command-line interface for running NLM experiments"""
    
    def __init__(self):
        self.setup_argument_parser()
        self.setup_configuration()
        self.setup_logging()
    
    def setup_argument_parser(self):
        """Setup command-line argument parser"""
        self.parser = argparse.ArgumentParser(
            description="NLM Experiment Runner - Comprehensive neural network simulation platform",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Examples:
  python nlm_cli.py experiment --brain-config default.cfg --world-config simple.json
  python nlm_cli.py benchmark --neuron-count 1000 --timesteps 500
  python nlm_cli.py replay --checkpoint brain_checkpoint.bin
  python nlm_cli.py compare --experiment1 run1.json --experiment2 run2.json

Configuration files:
  - brain-config: NLM configuration file (default.cfg)
  - world-config: World configuration in JSON format
  - experiment-config: Experiment-specific settings
"""
        )
        
        # Main commands
        main_subparsers = self.parser.add_subparsers(dest='command', help='Main commands')
        
        # Experiment command
        exp_parser = main_subparsers.add_parser('experiment', help='Run a single experiment')
        exp_parser.add_argument('--brain-config', default='default.cfg',
                              help='Brain configuration file')
        exp_parser.add_argument('--world-config', help='World configuration file')
        exp_parser.add_argument('--experiment-config', help='Experiment configuration file')
        exp_parser.add_argument('--steps', type=int, help='Number of simulation steps')
        exp_parser.add_argument('--duration', type=float, help='Duration in seconds')
        exp_parser.add_argument('--output', default='experiment_results.json',
                              help='Output file for results')
        exp_parser.add_argument('--seed', type=int, help='Random seed for reproducibility')
        exp_parser.add_argument('--verbose', action='store_true', help='Verbose output')
        
        # Benchmark command
        bench_parser = main_subparsers.add_parser('benchmark', help='Run benchmark tests')
        bench_parser.add_argument('--neuron-counts', nargs='+', type=int,
                                help='List of neuron counts to test')
        bench_parser.add_argument('--timesteps', type=int, default=100,
                                help='Number of steps per benchmark')
        bench_parser.add_argument('--repeats', type=int, default=3,
                                help='Number of repetitions per configuration')
        bench_parser.add_argument('--output', default='benchmark_results.json',
                                help='Output file for benchmark results')
        bench_parser.add_argument('--configs', nargs='+', help='Additional config files')
        
        # Replay command
        replay_parser = main_subparsers.add_parser('replay', help='Replay a saved experiment')
        replay_parser.add_argument('--checkpoint', required=True,
                                 help='Checkpoint file to load')
        replay_parser.add_argument('--steps', type=int, help='Number of steps to replay')
        replay_parser.add_argument('--output', default='replay_results.json',
                                 help='Output file for replay results')
        replay_parser.add_argument('--speed', type=float, default=1.0,
                                 help='Replay speed multiplier')
        
        # Compare command
        compare_parser = main_subparsers.add_parser('compare', help='Compare multiple experiments')
        compare_parser.add_argument('--experiments', nargs='+', required=True,
                                  help='Experiment result files to compare')
        compare_parser.add_argument('--metric', choices=['reward', 'performance', 'spikes', 'memory', 'all'],
                                  default='all', help='Metric to compare')
        compare_parser.add_argument('--output', default='comparison_report.html',
                                  help='Output HTML report file')
        
        # List command
        list_parser = main_subparsers.add_parser('list', help='List available experiments/configurations')
        list_parser.add_argument('--configs', action='store_true', help='List configuration files')
        list_parser.add_argument('--experiments', action='store_true', help='List experiment files')
        
        # Monitor command
        monitor_parser = main_subparsers.add_parser('monitor', help='Start monitoring mode')
        monitor_parser.add_argument('--port', type=int, default=8080,
                                  help='Port for web monitor (default: 8080)')
        monitor_parser.add_argument('--host', default='localhost',
                                  help='Host for web monitor (default: localhost)')
    
    def setup_configuration(self):
        """Setup default configuration"""
        self.default_config = {
            'simulation': {
                'timestep': 0.001,
                'max_steps': 1000,
                'neuron_count': 500,
                'region_count': 1,
                'connection_probability': 0.1
            },
            'learning': {
                'enable_reward_modulation': True,
                'enable_curiosity': True,
                'enable_development': True,
                'enable_structural_plasticity': True,
                'stdp_ltp_weight': 0.01,
                'stdp_ltd_weight': 0.012
            },
            'environment': {
                'width': 20,
                'height': 20,
                'vision_width': 8,
                'vision_height': 8
            }
        }
    
    def setup_logging(self):
        """Setup logging configuration"""
        import logging
        self.logger = logging.getLogger('nlm_cli')
        self.logger.setLevel(logging.INFO)
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setLevel(logging.INFO)
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        console_handler.setFormatter(formatter)
        self.logger.addHandler(console_handler)
        
        # File handler for detailed logs
        file_handler = logging.FileHandler('nlm_cli.log')
        file_handler.setLevel(logging.DEBUG)
        file_handler.setFormatter(formatter)
        self.logger.addHandler(file_handler)
    
    def run_experiment(self, args):
        """Run a single experiment"""
        self.logger.info("Starting experiment with config: %s", args.brain_config)
        
        try:
            # Load configuration
            config = self.load_config(args.brain_config)
            
            # Override with command line arguments
            if args.steps:
                config['simulation']['max_steps'] = args.steps
            if args.duration:
                config['simulation']['max_steps'] = int(args.duration / config['simulation']['timestep'])
            if args.seed:
                config['simulation']['random_seed'] = args.seed
            
            # Setup world if config provided
            world = None
            if args.world_config:
                world = self.setup_world(args.world_config, config)
            
            # Create brain
            brain = self.create_brain(config)
            
            # Setup experiment monitoring
            experiment_monitor = self.setup_experiment_monitor(config)
            
            # Run experiment
            results = self.run_simulation(brain, world, config, args.verbose)
            
            # Save results
            self.save_results(results, args.output)
            
            self.logger.info("Experiment completed successfully. Results saved to: %s", args.output)
            
            return results
            
        except Exception as e:
            self.logger.error("Experiment failed: %s", str(e))
            raise
    
    def run_benchmark(self, args):
        """Run benchmark tests"""
        self.logger.info("Starting benchmark with configs: %s", args.neuron_counts)
        
        benchmark_results = {
            'benchmark_config': {
                'neuron_counts': args.neuron_counts,
                'timesteps': args.timesteps,
                'repeats': args.repeats
            },
            'results': [],
            'summary': {}
        }
        
        for neuron_count in args.neuron_counts:
            self.logger.info("Benchmarking with %d neurons", neuron_count)
            
            config = self.default_config.copy()
            config['simulation']['neuron_count'] = neuron_count
            config['simulation']['max_steps'] = args.timesteps
            
            # Run multiple repeats for statistical significance
            repeat_results = []
            for repeat in range(args.repeats):
                self.logger.info("  Repeat %d/%d", repeat + 1, args.repeats)
                
                brain = self.create_brain(config)
                
                # Run simulation
                results = self.run_simulation(brain, None, config, False)
                repeat_results.append(results)
            
            # Aggregate repeat results
            aggregated_results = self.aggregate_benchmark_results(repeat_results)
            benchmark_results['results'].append({
                'neuron_count': neuron_count,
                'individual_results': repeat_results,
                'aggregated_results': aggregated_results
            })
        
        # Calculate summary statistics
        benchmark_results['summary'] = self.calculate_benchmark_summary(benchmark_results['results'])
        
        # Save results
        self.save_results(benchmark_results, args.output)
        
        self.logger.info("Benchmark completed. Results saved to: %s", args.output)
        
        return benchmark_results
    
    def run_replay(self, args):
        """Replay a saved experiment"""
        self.logger.info("Replaying experiment from checkpoint: %s", args.checkpoint)
        
        try:
            # Load brain from checkpoint
            brain = self.load_brain_from_checkpoint(args.checkpoint)
            
            # Determine replay steps
            steps = args.steps if args.steps else 1000
            
            # Run replay simulation
            results = self.run_simulation(brain, None, self.default_config, args.verbose)
            
            # Save replay results
            self.save_results(results, args.output)
            
            self.logger.info("Replay completed successfully. Results saved to: %s", args.output)
            
            return results
            
        except Exception as e:
            self.logger.error("Replay failed: %s", str(e))
            raise
    
    def run_compare(self, args):
        """Compare multiple experiments"""
        self.logger.info("Comparing experiments: %s", args.experiments)
        
        # Load experiment results
        experiment_data = []
        for exp_file in args.experiments:
            with open(exp_file, 'r') as f:
                data = json.load(f)
                experiment_data.append({
                    'file': exp_file,
                    'data': data
                })
        
        # Generate comparison report
        comparison_report = self.generate_comparison_report(experiment_data, args.metric)
        
        # Save report
        with open(args.output, 'w') as f:
            f.write(comparison_report)
        
        self.logger.info("Comparison report saved to: %s", args.output)
        
        return comparison_report
    
    def load_config(self, config_file: str) -> Dict[str, Any]:
        """Load configuration from file"""
        try:
            with open(config_file, 'r') as f:
                # Handle different config formats
                if config_file.endswith('.json'):
                    return json.load(f)
                elif config_file.endswith('.cfg') or config_file.endswith('.txt'):
                    # Parse NLM-style config file
                    config = self.default_config.copy()
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            
                            # Try to parse value
                            if value.isdigit():
                                value = int(value)
                            elif value.replace('.', '').isdigit():
                                value = float(value)
                            elif value.lower() in ['true', 'false']:
                                value = value.lower() == 'true'
                            
                            # Set nested configuration
                            self.set_nested_config(config, key, value)
                    
                    return config
                else:
                    # Assume default config
                    return self.default_config.copy()
                    
        except FileNotFoundError:
            self.logger.warning("Config file not found: %s, using defaults", config_file)
            return self.default_config.copy()
        except json.JSONDecodeError:
            self.logger.error("Invalid JSON in config file: %s", config_file)
            raise
        except Exception as e:
            self.logger.error("Error loading config file %s: %s", config_file, str(e))
            raise
    
    def set_nested_config(self, config: Dict, key: str, value: Any):
        """Set nested configuration key"""
        if '.' in key:
            parts = key.split('.')
            current = config
            for part in parts[:-1]:
                if part not in current:
                    current[part] = {}
                current = current[part]
            current[parts[-1]] = value
        else:
            config[key] = value
    
    def setup_world(self, world_config_file: str, brain_config: Dict[str, Any]):
        """Setup world from configuration"""
        try:
            if world_config_file.endswith('.json'):
                with open(world_config_file, 'r') as f:
                    world_config = json.load(f)
            else:
                world_config = {}
            
            # Create world
            world = pynlm.createSimpleWorld()
            
            # Configure world based on brain config and world config
            world_width = world_config.get('width', brain_config['environment']['width'])
            world_height = world_config.get('height', brain_config['environment']['height'])
            vision_width = world_config.get('vision_width', brain_config['environment']['vision_width'])
            vision_height = world_config.get('vision_height', brain_config['environment']['vision_height'])
            
            world.configure(world_width, world_height, vision_width, vision_height)
            
            # Set agent start position if specified
            if 'agent_start' in world_config:
                start = world_config['agent_start']
                world.setAgentStart(start['x'], start['y'])
            
            return world
            
        except Exception as e:
            self.logger.error("Error setting up world: %s", str(e))
            raise
    
    def create_brain(self, config: Dict[str, Any]) -> pynlm.Brain:
        """Create brain from configuration"""
        try:
            # Create config object
            brain_config = pynlm.createDefaultConfig()
            
            # Set configuration values
            sim_config = config['simulation']
            brain_config.set("neuron_count", sim_config.get('neuron_count', 500))
            brain_config.set("region_count", sim_config.get('region_count', 1))
            brain_config.set("connection_probability", sim_config.get('connection_probability', 0.1))
            brain_config.set("simulation_timestep", sim_config.get('timestep', 0.001))
            brain_config.set("random_seed", sim_config.get('random_seed', 42))
            
            # Set learning parameters
            learning_config = config['learning']
            brain_config.set("stdp_ltp_weight", learning_config.get('stdp_ltp_weight', 0.01))
            brain_config.set("stdp_ltd_weight", learning_config.get('stdp_ltd_weight', 0.012))
            
            # Create and return brain
            brain = pynlm.createBrain(brain_config)
            brain.initialize()
            
            return brain
            
        except Exception as e:
            self.logger.error("Error creating brain: %s", str(e))
            raise
    
    def setup_experiment_monitor(self, config: Dict[str, Any]):
        """Setup experiment monitoring"""
        # TODO: Implement experiment monitoring
        return None
    
    def run_simulation(self, brain: pynlm.Brain, world: Optional[pynlm.SimpleWorld],
                      config: Dict[str, Any], verbose: bool) -> Dict[str, Any]:
        """Run simulation with brain and world"""
        max_steps = config['simulation']['max_steps']
        timestep = config['simulation']['timestep']
        
        # Initialize metrics tracking
        metrics = {
            'steps_completed': 0,
            'total_spikes': 0,
            'firing_rate': 0.0,
            'active_neurons': 0,
            'reward_total': 0.0,
            'time_elapsed': 0.0,
            'start_time': time.time(),
            'agent_actions': 0,
            'memory_usage': 0.0,
            'cpu_usage': 0.0
        }
        
        self.logger.info("Starting simulation: %d steps, %.6f timestep", max_steps, timestep)
        
        for step in range(max_steps):
            step_start_time = time.time()
            
            # Update world if provided
            if world:
                world.update(timestep)
                
                # Get sensory input
                percept = world.getSensoryPercept()
                
                # Process sensory input through brain
                if hasattr(brain, 'receiveSensoryInput'):
                    brain.receiveSensoryInput(percept)
                elif hasattr(brain, 'injectCurrentToNeurons'):
                    brain.injectCurrentToNeurons(pynlm.NeuronType.Sensory, 1.0)
            
            # Run brain step
            brain.step(step, step * timestep)
            
            # Get action if world exists
            action = None
            if world:
                # Simple action selection for demo
                action = pynlm.MotorCommand.MoveForward
                
                # Apply action in world
                world.applyMotorCommand(action, step * timestep)
                metrics['agent_actions'] += 1
                
                # Get reward if available
                if hasattr(world, 'applyAction'):
                    result = world.applyAction(action, step * timestep)
                    metrics['reward_total'] += result.reward
            
            # Update metrics
            metrics['steps_completed'] = step + 1
            metrics['total_spikes'] = brain.getTotalSpikeCount()
            metrics['firing_rate'] = brain.getAverageFiringRate()
            metrics['active_neurons'] = brain.getActiveNeuronCount()
            metrics['time_elapsed'] = time.time() - metrics['start_time']
            
            # Log progress
            if verbose or (step + 1) % 100 == 0:
                self.logger.info(
                    "Step %d/%d - Spikes: %d - Rate: %.3f Hz - "
                    "Actions: %d - Reward: %.3f",
                    step + 1, max_steps,
                    metrics['total_spikes'],
                    metrics['firing_rate'],
                    metrics['agent_actions'],
                    metrics['reward_total']
                )
            
            # Check for early termination
            if metrics['time_elapsed'] > config['simulation'].get('max_duration', float('inf')):
                self.logger.info("Time limit reached. Stopping simulation.")
                break
        
        # Calculate final metrics
        metrics['final_spikes'] = brain.getTotalSpikeCount()
        metrics['final_firing_rate'] = brain.getAverageFiringRate()
        metrics['final_active_neurons'] = brain.getActiveNeuronCount()
        metrics['simulation_efficiency'] = metrics['steps_completed'] / metrics['time_elapsed']
        
        self.logger.info(
            "Simulation completed: %d steps in %.2f seconds - "
            "%.2f steps/second",
            metrics['steps_completed'],
            metrics['time_elapsed'],
            metrics['simulation_efficiency']
        )
        
        return {
            'metrics': metrics,
            'brain': brain,
            'world': world,
            'config': config
        }
    
    def aggregate_benchmark_results(self, repeat_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Aggregate benchmark results from multiple repeats"""
        if not repeat_results:
            return {}
        
        # Extract metrics from all results
        steps = [r['metrics']['steps_completed'] for r in repeat_results]
        spikes = [r['metrics']['total_spikes'] for r in repeat_results]
        rates = [r['metrics']['firing_rate'] for r in repeat_results]
        rewards = [r['metrics']['reward_total'] for r in repeat_results]
        
        # Calculate statistics
        aggregated = {
            'steps': {
                'mean': sum(steps) / len(steps),
                'std': (sum((x - sum(steps)/len(steps))**2 for x in steps) / len(steps))**0.5,
                'min': min(steps),
                'max': max(steps)
            },
            'spikes': {
                'mean': sum(spikes) / len(spikes),
                'std': (sum((x - sum(spikes)/len(spikes))**2 for x in spikes) / len(spikes))**0.5,
                'min': min(spikes),
                'max': max(spikes)
            },
            'firing_rate': {
                'mean': sum(rates) / len(rates),
                'std': (sum((x - sum(rates)/len(rates))**2 for x in rates) / len(rates))**0.5,
                'min': min(rates),
                'max': max(rates)
            },
            'reward': {
                'mean': sum(rewards) / len(rewards),
                'std': (sum((x - sum(rewards)/len(rewards))**2 for x in rewards) / len(rewards))**0.5,
                'min': min(rewards),
                'max': max(rewards)
            },
            'repeats': len(repeat_results)
        }
        
        return aggregated
    
    def calculate_benchmark_summary(self, benchmark_results: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Calculate benchmark summary statistics"""
        if not benchmark_results:
            return {}
        
        # Analyze performance across different neuron counts
        neuron_counts = [r['neuron_count'] for r in benchmark_results]
        
        summary = {
            'neuron_counts_tested': len(neuron_counts),
            'min_neurons': min(neuron_counts),
            'max_neurons': max(neuron_counts),
            'scales_tested': [f"{n//100}k" if n >= 1000 else str(n) for n in neuron_counts]
        }
        
        # Add performance metrics for each neuron count
        for result in benchmark_results:
            neuron_count = result['neuron_count']
            aggregated = result['aggregated_results']
            
            summary[f'neuron_count_{neuron_count}'] = {
                'steps_per_second': aggregated['steps'] / aggregated['steps']['std'] if aggregated['steps']['std'] > 0 else aggregated['steps']['mean'],
                'efficiency_score': aggregated['firing_rate']['mean'] / aggregated['spikes']['mean'] if aggregated['spikes']['mean'] > 0 else 0,
                'reward_efficiency': aggregated['reward']['mean'] / aggregated['steps']['mean'] if aggregated['steps']['mean'] > 0 else 0
            }
        
        return summary
    
    def save_results(self, results: Dict[str, Any], output_file: str):
        """Save experiment results to file"""
        try:
            # Create output directory if it doesn't exist
            output_path = Path(output_file)
            output_path.parent.mkdir(parents=True, exist_ok=True)
            
            # Save results based on file extension
            if output_file.endswith('.json'):
                with open(output_file, 'w') as f:
                    json.dump(results, f, indent=2, default=str)
            elif output_file.endswith('.txt') or output_file.endswith('.log'):
                with open(output_file, 'w') as f:
                    f.write(self.format_text_results(results))
            elif output_file.endswith('.csv'):
                self.save_results_csv(results, output_file)
            else:
                # Default to JSON
                json_file = output_file + '.json'
                with open(json_file, 'w') as f:
                    json.dump(results, f, indent=2, default=str)
                self.logger.info("Results saved as JSON: %s", json_file)
            
            self.logger.info("Results saved to: %s", output_file)
            
        except Exception as e:
            self.logger.error("Error saving results: %s", str(e))
            raise
    
    def format_text_results(self, results: Dict[str, Any]) -> str:
        """Format results as text for human-readable output"""
        output = []
        output.append("NLM Experiment Results")
        output.append("=" * 50)
        
        if 'metrics' in results:
            metrics = results['metrics']
            output.append(f"Simulation Summary:")
            output.append(f"  Steps Completed: {metrics['steps_completed']}")
            output.append(f"  Total Spikes: {metrics['total_spikes']}")
            output.append(f"  Average Firing Rate: {metrics['firing_rate']:.3f} Hz")
            output.append(f"  Active Neurons: {metrics['active_neurons']}")
            output.append(f"  Total Reward: {metrics['reward_total']:.3f}")
            output.append(f"  Simulation Time: {metrics['time_elapsed']:.2f}s")
            output.append(f"  Efficiency: {metrics['simulation_efficiency']:.2f} steps/s")
        
        output.append("")
        output.append("Configuration:")
        for key, value in results.get('config', {}).items():
            output.append(f"  {key}: {value}")
        
        return "\n".join(output)
    
    def save_results_csv(self, results: Dict[str, Any], csv_file: str):
        """Save results in CSV format"""
        try:
            import csv
            
            with open(csv_file, 'w', newline='') as f:
                writer = csv.writer(f)
                
                # Write header
                writer.writerow(['Metric', 'Value'])
                
                # Write metrics if available
                if 'metrics' in results:
                    metrics = results['metrics']
                    for key, value in metrics.items():
                        writer.writerow([key, value])
                
                # Write configuration if available
                if 'config' in results:
                    writer.writerow([])  # Empty row
                    writer.writerow(['Configuration'])
                    for key, value in results['config'].items():
                        writer.writerow([key, value])
            
        except Exception as e:
            self.logger.error("Error saving CSV results: %s", str(e))
            raise
    
    def load_brain_from_checkpoint(self, checkpoint_file: str) -> pynlm.Brain:
        """Load brain from checkpoint file"""
        try:
            # Try to load using brain.save/load methods
            # Note: This assumes the brain has save/load capabilities
            # For demo purposes, we'll create a new brain
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            self.logger.warning(
                "Checkpoint loading not fully implemented. Creating new brain instead. "
                "To enable full checkpoint support, implement save/load methods."
            )
            
            return brain
            
        except Exception as e:
            self.logger.error("Error loading checkpoint: %s", str(e))
            raise
    
    def generate_comparison_report(self, experiment_data: List[Dict[str, Any]], 
                                 metric: str) -> str:
        """Generate HTML comparison report"""
        html_template = self.load_comparison_template()
        
        # Prepare comparison data
        comparison_data = {
            'experiments': [],
            'metric': metric,
            'summary': self.summarize_experiments(experiment_data),
            'detailed_comparison': self.create_detailed_comparison(experiment_data, metric)
        }
        
        # Add experiment data
        for exp_data in experiment_data:
            comparison_data['experiments'].append({
                'file': exp_data['file'],
                'data': exp_data['data']
            })
        
        # Fill template
        html = html_template.format(**comparison_data)
        
        return html
    
    def load_comparison_template(self) -> str:
        """Load HTML comparison template"""
        return '''
<!DOCTYPE html>
<html>
<head>
    <title>NLM Experiment Comparison Report</title>
    <style>
        body {{ font-family: Arial, sans-serif; margin: 20px; }}
        .header {{ background-color: #f0f0f0; padding: 20px; border-radius: 5px; }}
        .experiment {{ border: 1px solid #ddd; margin: 10px; padding: 10px; }}
        .metric {{ background-color: #f9f9f9; padding: 5px; margin: 5px 0; }}
        .comparison-table {{ width: 100%; border-collapse: collapse; }}
        .comparison-table th, .comparison-table td {{ border: 1px solid #ddd; padding: 8px; text-align: left; }}
        .comparison-table th {{ background-color: #f2f2f2; }}
        .best {{ background-color: #d4edda; font-weight: bold; }}
        .worst {{ background-color: #f8d7da; }}
    </style>
</head>
<body>
    <div class="header">
        <h1>NLM Experiment Comparison Report</h1>
        <p>Generated on {{datetime}}</p>
    </div>
    
    <h2>Summary</h2>
    <div class="summary">
        {{summary}}
    </div>
    
    <h2>Detailed Comparison</h2>
    {{detailed_comparison}}
    
    <h2>Experiment Results</h2>
    {% for experiment in experiments %}
    <div class="experiment">
        <h3>Experiment: {{ experiment.file }}</h3>
        <div class="metric">
            <h4>Key Metrics</h4>
            {{ experiment.data }}
        </div>
    </div>
    {% endfor %}
</body>
</html>
        '''
    
    def summarize_experiments(self, experiment_data: List[Dict[str, Any]]) -> str:
        """Summarize experiments for comparison"""
        if not experiment_data:
            return "<p>No experiments to compare.</p>"
        
        summary_html = []
        summary_html.append("<table class='comparison-table'>")
        summary_html.append("<tr><th>Experiment</th><th>Steps</th><th>Spikes</th><th>Firing Rate</th><th>Reward</th></tr>")
        
        for exp_data in experiment_data:
            data = exp_data['data']
            if 'metrics' in data:
                metrics = data['metrics']
                row_class = self.get_experiment_row_class(exp_data['file'], experiment_data)
                summary_html.append(f"""
                <tr class='{row_class}'>
                    <td>{exp_data['file']}</td>
                    <td>{metrics.get('steps_completed', 'N/A')}</td>
                    <td>{metrics.get('total_spikes', 'N/A')}</td>
                    <td>{metrics.get('firing_rate', 'N/A'):.3f}</td>
                    <td>{metrics.get('reward_total', 'N/A'):.3f}</td>
                </tr>
                """)
        
        summary_html.append("</table>")
        return "\n".join(summary_html)
    
    def get_experiment_row_class(self, filename: str, all_experiments: List[Dict[str, Any]]) -> str:
        """Determine CSS class for experiment row based on performance"""
        if len(all_experiments) <= 1:
            return ""
        
        # Get metrics for this experiment
        exp_data = next((exp for exp in all_experiments if exp['file'] == filename), None)
        if not exp_data or 'metrics' not in exp_data['data']:
            return ""
        
        # Compare to best/worst
        metrics = exp_data['data']['metrics']
        best_steps = max(exp['data']['metrics']['steps_completed'] 
                        for exp in all_experiments if 'metrics' in exp['data'])
        worst_steps = min(exp['data']['metrics']['steps_completed'] 
                         for exp in all_experiments if 'metrics' in exp['data'])
        
        if metrics['steps_completed'] == best_steps:
            return "best"
        elif metrics['steps_completed'] == worst_steps:
            return "worst"
        else:
            return ""
    
    def create_detailed_comparison(self, experiment_data: List[Dict[str, Any]], 
                                  metric: str) -> str:
        """Create detailed comparison based on selected metric"""
        if metric == 'single':
            return "<p>Single experiment analysis. No comparison performed.</p>"
        
        comparison_html = []
        
        if metric in ['reward', 'spikes', 'memory', 'performance', 'all']:
            comparison_html.append(self.create_metric_comparison(experiment_data, metric))
        
        return "\n".join(comparison_html)
    
    def create_metric_comparison(self, experiment_data: List[Dict[str, Any]], metric: str) -> str:
        """Create comparison for specific metric"""
        metric_html = f"""
        <h3>Comparison by {metric.title()}</h3>
        <table class='comparison-table'>
            <tr><th>Experiment</th><th>{metric.title()}</th><th>Rank</th></tr>
        """
        
        # Calculate metric values
        metric_values = []
        for exp_data in experiment_data:
            if 'metrics' in exp_data['data']:
                value = self.extract_metric(exp_data['data']['metrics'], metric)
                metric_values.append((exp_data['file'], value))
        
        if not metric_values:
            return "<p>No metric data available for comparison.</p>"
        
        # Sort by metric value
        metric_values.sort(key=lambda x: x[1], reverse=True)
        
        # Create table rows
        for i, (filename, value) in enumerate(metric_values):
            rank = "1st" if i == 0 else f"{i + 1}th"
            row_class = "best" if i == 0 else ("worst" if i == len(metric_values) - 1 else "")
            
            metric_html += f"""
            <tr class='{row_class}'>
                <td>{filename}</td>
                <td>{value:.3f if isinstance(value, float) else value}</td>
                <td>{rank}</td>
            </tr>
            """
        
        metric_html += "</table>"
        return metric_html
    
    def extract_metric(self, metrics: Dict[str, Any], metric: str) -> float:
        """Extract metric value from metrics dictionary"""
        metric_map = {
            'reward': 'reward_total',
            'spikes': 'total_spikes',
            'memory': 'active_neurons',
            'performance': 'firing_rate',
            'steps': 'steps_completed'
        }
        
        key = metric_map.get(metric, metric)
        return float(metrics.get(key, 0))
    
    def run(self, args):
        """Main entry point for CLI"""
        try:
            if args.command == 'experiment':
                self.run_experiment(args)
            elif args.command == 'benchmark':
                self.run_benchmark(args)
            elif args.command == 'replay':
                self.run_replay(args)
            elif args.command == 'compare':
                self.run_compare(args)
            elif args.command == 'list':
                self.run_list(args)
            elif args.command == 'monitor':
                self.run_monitor(args)
            else:
                self.parser.error(f"Unknown command: {args.command}")
                
        except KeyboardInterrupt:
            self.logger.info("Experiment interrupted by user")
            return 1
        except Exception as e:
            self.logger.error("Experiment failed: %s", str(e))
            return 1
        
        return 0
    
    def run_list(self, args):
        """List experiments and configurations"""
        print("NLM Experiment and Configuration Listings")
        print("=" * 50)
        
        if args.configs:
            self.list_configurations()
        
        if args.experiments:
            self.list_experiments()
        
        if not args.configs and not args.experiments:
            print("Available commands:")
            print("  --configs    List configuration files")
            print("  --experiments List experiment result files")
    
    def list_configurations(self):
        """List available configuration files"""
        config_dir = Path("configs")
        if config_dir.exists():
            print("\nConfiguration files:")
            for config_file in config_dir.glob("*.cfg"):
                print(f"  {config_file.name}")
            for config_file in config_dir.glob("*.json"):
                print(f"  {config_file.name}")
        else:
            print("\nNo configuration directory found.")
    
    def list_experiments(self):
        """List experiment result files"""
        result_dir = Path(".")
        experiment_files = []
        
        # Find experiment result files
        for ext in ['*.json', '*.txt', '*.csv']:
            experiment_files.extend(result_dir.glob(f"experiment_results{ext}"))
            experiment_files.extend(result_dir.glob(f"benchmark_results{ext}"))
            experiment_files.extend(result_dir.glob(f"replay_results{ext}"))
        
        if experiment_files:
            print("\nExperiment result files:")
            for exp_file in sorted(experiment_files):
                print(f"  {exp_file.name}")
        else:
            print("\nNo experiment result files found.")
    
    def run_monitor(self, args):
        """Start monitoring mode"""
        self.logger.info("Starting web monitor on port %d", args.port)
        self.logger.info("Monitor URL: http://%s:%d", args.host, args.port)
        
        # TODO: Implement web monitoring
        print(f"Web monitoring would start on http://{args.host}:{args.port}")
        print("Note: Web monitoring implementation not yet complete.")
        print("For now, use file-based logging and analysis tools.")

# Example usage and entry point
if __name__ == "__main__":
    cli = ExperimentCLI()
    
    # Parse arguments
    args = cli.parser.parse_args()
    
    # Run the appropriate command
    exit_code = cli.run(args)
    exit(exit_code)
```

### 2.1 Experiment Launcher Script

```python
#!/usr/bin/env python3
"""
NLM Experiment Launcher - Convenience script for running experiments
"""

import pynlm
import argparse
import json
import time
import os
from pathlib import Path
from datetime import datetime

class NLMExperimentLauncher:
    """High-level interface for running NLM experiments with presets"""
    
    def __init__(self):
        self.setup_argument_parser()
    
    def setup_argument_parser(self):
        """Setup argument parser for experiment launcher"""
        parser = argparse.ArgumentParser(
            description="NLM Experiment Launcher - Quick experiment setup and execution",
            formatter_class=argparse.ArgumentDefaultsHelpFormatter
        )
        
        # Preset configurations
        preset_group = parser.add_argument_group('Preset Configurations')
        preset_group.add_argument('--preset', choices=['quick', 'standard', 'research', 'production'],
                                 default='standard', help='Preset configuration')
        
        # Brain configuration
        brain_group = parser.add_argument_group('Brain Configuration')
        brain_group.add_argument('--neurons', type=int, help='Number of neurons')
        brain_group.add_argument('--regions', type=int, help='Number of brain regions')
        brain_group.add_argument('--timestep', type=float, help='Simulation timestep')
        
        # Learning configuration
        learning_group = parser.add_argument_group('Learning Configuration')
        learning_group.add_argument('--enable-reward', action='store_true', help='Enable reward modulation')
        learning_group.add_argument('--enable-curiosity', action='store_true', help='Enable curiosity')
        learning_group.add_argument('--enable-development', action='store_true', help='Enable development')
        learning_group.add_argument('--enable-structural', action='store_true', help='Enable structural plasticity')
        
        # Environment configuration
        env_group = parser.add_argument_group('Environment Configuration')
        env_group.add_argument('--world-width', type=int, default=20, help='World width')
        env_group.add_argument('--world-height', type=int, default=20, help='World height')
        env_group.add_argument('--vision-width', type=int, default=8, help='Vision width')
        env_group.add_argument('--vision-height', type=int, default=8, help='Vision height')
        
        # Experiment configuration
        exp_group = parser.add_argument_group('Experiment Configuration')
        exp_group.add_argument('--steps', type=int, help='Number of simulation steps')
        exp_group.add_argument('--duration', type=float, help='Duration in seconds')
        exp_group.add_argument('--save-config', help='Save configuration to file')
        exp_group.add_argument('--load-config', help='Load configuration from file')
        
        # Output configuration
        out_group = parser.add_argument_group('Output Configuration')
        out_group.add_argument('--output', default='experiment_results.json', help='Output file for results')
        out_group.add_argument('--verbose', action='store_true', help='Enable verbose output')
        out_group.add_argument('--quiet', action='store_true', help='Minimal output')
        
        # Special modes
        mode_group = parser.add_argument_group('Special Modes')
        mode_group.add_argument('--replay', help='Replay from checkpoint file')
        mode_group.add_argument('--benchmark', action='store_true', help='Run benchmark test')
        mode_group.add_argument('--interactive', action='store_true', help='Start interactive mode')
        
        self.parser = parser
    
    def get_preset_config(self, preset: str) -> dict:
        """Get configuration for preset"""
        presets = {
            'quick': {
                'neurons': 100,
                'timestep': 0.01,
                'enable_reward': True,
                'enable_curiosity': False,
                'enable_development': False,
                'enable_structural': False
            },
            'standard': {
                'neurons': 500,
                'timestep': 0.001,
                'enable_reward': True,
                'enable_curiosity': True,
                'enable_development': True,
                'enable_structural': False
            },
            'research': {
                'neurons': 2000,
                'timestep': 0.0005,
                'enable_reward': True,
                'enable_curiosity': True,
                'enable_development': True,
                'enable_structural': True
            },
            'production': {
                'neurons': 5000,
                'timestep': 0.001,
                'enable_reward': True,
                'enable_curiosity': True,
                'enable_development': True,
                'enable_structural': True
            }
        }
        
        return presets.get(preset, presets['standard'])
    
    def load_configuration(self, config_file: str) -> dict:
        """Load configuration from file"""
        try:
            with open(config_file, 'r') as f:
                if config_file.endswith('.json'):
                    return json.load(f)
                else:
                    # Parse simple config file
                    config = {}
                    for line in f:
                        line = line.strip()
                        if line and not line.startswith('#') and '=' in line:
                            key, value = line.split('=', 1)
                            key = key.strip()
                            value = value.strip()
                            config[key] = value
                    return config
        except Exception as e:
            print(f"Error loading config file: {e}")
            return {}
    
    def save_configuration(self, config: dict, config_file: str):
        """Save configuration to file"""
        try:
            with open(config_file, 'w') as f:
                json.dump(config, f, indent=2)
            print(f"Configuration saved to: {config_file}")
        except Exception as e:
            print(f"Error saving config file: {e}")
    
    def setup_brain_config(self, preset_config: dict, args) -> pynlm.Config:
        """Setup brain configuration from preset and command line arguments"""
        # Create default config
        config = pynlm.createDefaultConfig()
        
        # Override with preset
        if 'neurons' in preset_config:
            config.set("neuron_count", preset_config['neurons'])
        if 'timestep' in preset_config:
            config.set("simulation_timestep", preset_config['timestep'])
        
        # Override with command line arguments
        if args.neurons:
            config.set("neuron_count", args.neurons)
        if args.timestep:
            config.set("simulation_timestep", args.timestep)
        
        # Set learning parameters
        learning_config = {
            'reward_modulation': preset_config.get('enable_reward', False),
            'curiosity': preset_config.get('enable_curiosity', False),
            'development': preset_config.get('enable_development', False),
            'structural_plasticity': preset_config.get('enable_structural', False)
        }
        
        # Note: These would need to be set via brain configuration
        # For now, we'll use the brain's initialization methods
        
        return config
    
    def setup_world_config(self, args) -> pynlm.SimpleWorld:
        """Setup world configuration"""
        world = pynlm.createSimpleWorld()
        
        # Configure world
        world.configure(
            args.world_width,
            args.world_height,
            args.vision_width,
            args.vision_height
        )
        
        # Set agent start position
        world.setAgentStart(args.world_width // 2, args.world_height // 2)
        
        return world
    
    def run_experiment(self, args):
        """Run experiment with launcher configuration"""
        print(f"Starting NLM experiment with preset: {args.preset}")
        
        # Get preset configuration
        preset_config = self.get_preset_config(args.preset)
        
        # Load additional configuration if specified
        if args.load_config:
            loaded_config = self.load_configuration(args.load_config)
            # Merge with preset
            preset_config.update(loaded_config)
        
        # Setup brain configuration
        brain_config = self.setup_brain_config(preset_config, args)
        
        # Create brain
        brain = pynlm.createBrain(brain_config)
        brain.initialize()
        
        # Setup world if not in replay mode
        world = None
        if not args.replay:
            world = self.setup_world_config(args)
        
        # Determine number of steps
        steps = args.steps
        if not steps and args.duration:
            steps = int(args.duration / brain_config.getOr("simulation_timestep", 0.001))
        
        if not steps:
            steps = preset_config.get('steps', 100)
        
        # Run experiment
        print(f"Experiment configuration:")
        print(f"  Neurons: {brain_config.getOr('neuron_count', 'N/A')}")
        print(f"  Regions: {brain_config.getOr('region_count', 'N/A')}")
        print(f"  Steps: {steps}")
        print(f"  Timestep: {brain_config.getOr('simulation_timestep', 0.001)}")
        print(f"  Duration: {steps * brain_config.getOr('simulation_timestep', 0.001):.2f}s")
        print()
        
        # Run simulation
        start_time = time.time()
        
        for step in range(steps):
            # Update world
            world.update(brain_config.getOr("simulation_timestep", 0.001))
            
            # Get sensory input
            percept = world.getSensoryPercept()
            
            # Process sensory input (if world exists)
            if hasattr(brain, 'receiveSensoryInput'):
                brain.receiveSensoryInput(percept)
            
            # Run brain step
            brain.step(step, step * brain_config.getOr("simulation_timestep", 0.001))
            
            # Get action
            action = pynlm.MotorCommand.MoveForward
            
            # Apply action
            world.applyMotorCommand(action, step * brain_config.getOr("simulation_timestep", 0.001))
            
            # Print progress
            if args.verbose or (step + 1) % 50 == 0:
                elapsed = time.time() - start_time
                print(f"Step {step + 1}/{steps} - "
                      f"Firing: {brain.getFiringNeuronCount()} - "
                      f"Rate: {brain.getAverageFiringRate():.3f} Hz - "
                      f"Time: {elapsed:.2f}s")
        
        # Calculate final metrics
        end_time = time.time()
        elapsed = end_time - start_time
        
        results = {
            'experiment_info': {
                'preset': args.preset,
                'start_time': datetime.now().isoformat(),
                'end_time': datetime.now().isoformat(),
                'duration_seconds': elapsed,
                'steps_completed': steps,
                'neuron_count': brain_config.getOr("neuron_count", 0),
                'region_count': brain_config.getOr("region_count", 0)
            },
            'performance_metrics': {
                'total_spikes': brain.getTotalSpikeCount(),
                'average_firing_rate': brain.getAverageFiringRate(),
                'active_neurons': brain.getActiveNeuronCount(),
                'firing_neurons': brain.getFiringNeuronCount(),
                'efficiency': steps / elapsed if elapsed > 0 else 0
            },
            'configuration': preset_config,
            'brain': brain,
            'world': world
        }
        
        # Save results
        if args.save_config:
            self.save_configuration(preset_config, args.save_config)
        
        # Save results
        with open(args.output, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"\nExperiment completed!")
        print(f"Results saved to: {args.output}")
        print(f"Duration: {elapsed:.2f}s ({steps/elapsed:.1f} steps/sec)")
        print(f"Total spikes: {results['performance_metrics']['total_spikes']}")
        print(f"Average firing rate: {results['performance_metrics']['average_firing_rate']:.3f} Hz")
        
        return results
    
    def run(self):
        """Main entry point"""
        args = self.parser.parse_args()
        
        try:
            if args.replay:
                print("Replay mode not yet implemented in launcher")
                return
            
            self.run_experiment(args)
            
        except KeyboardInterrupt:
            print("\nExperiment interrupted by user")
            return 1
        except Exception as e:
            print(f"Error: {e}")
            return 1

if __name__ == "__main__":
    launcher = NLMExperimentLauncher()
    launcher.run()
```

### 2.2 Configuration Management Script

```python
#!/usr/bin/env python3
"""
NLM Configuration Manager - Command-line tool for managing NLM configurations
"""

import pynlm
import argparse
import json
import os
import shutil
from pathlib import Path
from typing import Dict, Any, List

class NLMConfigManager:
    """Manage NLM configurations and presets"""
    
    def __init__(self):
        self.setup_argument_parser()
        self.config_dir = Path("configs")
        self.config_dir.mkdir(exist_ok=True)
    
    def setup_argument_parser(self):
        """Setup argument parser for configuration manager"""
        parser = argparse.ArgumentParser(
            description="NLM Configuration Manager - Create and manage NLM configurations",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Commands:
  create     Create a new configuration file
  list       List available configurations
  export     Export configuration to different formats
  import     Import configuration from different formats
  validate   Validate configuration file
  template   Show configuration template
  backup     Backup configuration files
  restore    Restore configuration from backup
"""
        )
        
        # Main commands
        subparsers = parser.add_subparsers(dest='command', help='Commands')
        
        # Create command
        create_parser = subparsers.add_parser('create', help='Create a new configuration')
        create_parser.add_argument('name', help='Name of the configuration')
        create_parser.add_argument('--type', choices=['brain', 'world', 'experiment', 'full'],
                                 default='brain', help='Type of configuration')
        create_parser.add_argument('--output', default='configs/{name}.json',
                                 help='Output file path')
        
        # List command
        list_parser = subparsers.add_parser('list', help='List available configurations')
        list_parser.add_argument('--type', choices=['brain', 'world', 'experiment', 'all'],
                               default='all', help='Type of configurations to list')
        list_parser.add_argument('--show-defaults', action='store_true',
                               help='Show default configuration values')
        
        # Export command
        export_parser = subparsers.add_parser('export', help='Export configuration to different formats')
        export_parser.add_argument('source', help='Source configuration file')
        export_parser.add_argument('--format', choices=['json', 'ini', 'yaml', 'xml'],
                                 default='json', help='Output format')
        export_parser.add_argument('--output', help='Output file path')
        
        # Import command
        import_parser = subparsers.add_parser('import', help='Import configuration from different formats')
        import_parser.add_argument('source', help='Source configuration file')
        import_parser.add_argument('--format', choices=['json', 'ini', 'yaml', 'xml'],
                                 default='json', help='Input format')
        import_parser.add_argument('--output', help='Output file path')
        
        # Validate command
        validate_parser = subparsers.add_parser('validate', help='Validate configuration file')
        validate_parser.add_argument('config_file', help='Configuration file to validate')
        validate_parser.add_argument('--strict', action='store_true', help='Strict validation')
        
        # Template command
        template_parser = subparsers.add_parser('template', help='Show configuration template')
        template_parser.add_argument('--type', choices=['brain', 'world', 'experiment'],
                                   default='brain', help='Type of template')
        
        # Backup command
        backup_parser = subparsers.add_parser('backup', help='Backup configuration files')
        backup_parser.add_argument('--directory', default='configs/backup',
                                 help='Backup directory')
        backup_parser.add_argument('--compress', action='store_true', help='Compress backup')
        
        # Restore command
        restore_parser = subparsers.add_parser('restore', help='Restore configuration from backup')
        restore_parser.add_argument('backup_file', help='Backup file to restore')
        restore_parser.add_argument('--output', default='configs/restored.json',
                                  help='Output file path')
        
        self.parser = parser
    
    def create_configuration(self, args):
        """Create a new configuration file"""
        print(f"Creating {args.type} configuration: {args.name}")
        
        # Get template configuration
        config = self.get_configuration_template(args.type)
        
        # Fill configuration based on type
        if args.type == 'brain':
            config = self.fill_brain_config_template(config)
        elif args.type == 'world':
            config = self.fill_world_config_template(config)
        elif args.type == 'experiment':
            config = self.fill_experiment_config_template(config)
        elif args.type == 'full':
            config = self.create_full_config_template()
        
        # Create output path
        output_path = Path(args.output.format(name=args.name))
        output_path.parent.mkdir(parents=True, exist_ok=True)
        
        # Save configuration
        with open(output_path, 'w') as f:
            json.dump(config, f, indent=2)
        
        print(f"Configuration created: {output_path}")
    
    def list_configurations(self, args):
        """List available configurations"""
        print("Available NLM Configurations")
        print("=" * 50)
        
        if args.type in ['brain', 'all']:
            self.list_brain_configurations(args.show_defaults)
        
        if args.type in ['world', 'all']:
            self.list_world_configurations(args.show_defaults)
        
        if args.type in ['experiment', 'all']:
            self.list_experiment_configurations(args.show_defaults)
    
    def get_configuration_template(self, config_type: str) -> Dict[str, Any]:
        """Get configuration template for type"""
        templates = {
            'brain': {
                'description': 'Brain configuration',
                'parameters': {
                    'neuron_count': 1000,
                    'region_count': 1,
                    'connection_probability': 0.1,
                    'simulation_timestep': 0.001,
                    'random_seed': 42,
                    'stdp_ltp_weight': 0.01,
                    'stdp_ltd_weight': 0.012,
                    'enable_reward_modulation': True,
                    'enable_curiosity': True,
                    'enable_development': True,
                    'enable_structural_plasticity': False
                }
            },
            'world': {
                'description': 'World configuration',
                'parameters': {
                    'width': 20,
                    'height': 20,
                    'vision_width': 8,
                    'vision_height': 8,
                    'max_energy': 100.0,
                    'energy_decay_rate': 0.1,
                    'agent_start_x': 10.0,
                    'agent_start_y': 10.0
                }
            },
            'experiment': {
                'description': 'Experiment configuration',
                'parameters': {
                    'name': 'default_experiment',
                    'description': 'Default NLM experiment',
                    'steps': 1000,
                    'timestep': 0.001,
                    'enable_logging': True,
                    'enable_checkpointing': False,
                    'checkpoint_interval': 100,
                    'results_output': 'experiment_results.json'
                }
            }
        }
        
        return templates.get(config_type, {})
    
    def fill_brain_config_template(self, template: Dict[str, Any]) -> Dict[str, Any]:
        """Fill brain configuration template with actual values"""
        config = {
            'version': '1.0',
            'description': template.get('description', 'Brain configuration'),
            'parameters': template.get('parameters', {}),
            'learning_systems': {
                'plasticity': {
                    'stdp': {
                        'enabled': True,
                        'learning_rate': template['parameters']['stdp_ltp_weight']
                    },
                    'hebbian': {
                        'enabled': True,
                        'learning_rate': 0.01
                    },
                    'structural': {
                        'enabled': template['parameters']['enable_structural_plasticity'],
                        'synaptogenesis_rate': 0.0001f,
                        'pruning_rate': 0.00001f
                    }
                },
                'neuromodulation': {
                    'dopamine': {
                        'enabled': template['parameters']['enable_reward_modulation'],
                        'range': [-1.0, 1.0]
                    },
                    'curiosity': {
                        'enabled': template['parameters']['enable_curiosity'],
                        'exploration_drive': 0.5
                    },
                    'novelty': {
                        'enabled': True,
                        'sensitivity': 0.1
                    }
                },
                'development': {
                    'enabled': template['parameters']['enable_development'],
                    'stages': [
                        {'name': 'Initial', 'duration': 60.0, 'plasticity': 1.0},
                        {'name': 'CriticalPeriod', 'duration': 180.0, 'plasticity': 0.8},
                        {'name': 'Maturation', 'duration': 480.0, 'plasticity': 0.5},
                        {'name': 'Adult', 'duration': float('inf'), 'plasticity': 0.2}
                    ]
                }
            }
        }
        
        return config
    
    def fill_world_config_template(self, template: Dict[str, Any]) -> Dict[str, Any]:
        """Fill world configuration template"""
        params = template.get('parameters', {})
        
        return {
            'version': '1.0',
            'description': template.get('description', 'World configuration'),
            'parameters': params,
            'physics': {
                'gravity': 0.0,
                'friction': 0.1,
                'max_velocity': 10.0,
                'angular_velocity_limit': 3.14
            },
            'objects': {
                'resources': {
                    'count': 10,
                    'value_range': [1.0, 10.0]
                },
                'hazards': {
                    'count': 5,
                    'damage_range': [-5.0, -1.0]
                },
                'walls': {
                    'enabled': True,
                    'thickness': 0.5
                }
            }
        }
    
    def fill_experiment_config_template(self, template: Dict[str, Any]) -> Dict[str, Any]:
        """Fill experiment configuration template"""
        params = template.get('parameters', {})
        
        return {
            'version': '1.0',
            'description': template.get('description', 'Experiment configuration'),
            'parameters': params,
            'metrics': {
                'collect_spikes': True,
                'collect_firing_rates': True,
                'collect_memory_usage': True,
                'collect_reward': True,
                'collect_time_metrics': True,
                'collect_energy_usage': True
            },
            'analysis': {
                'statistical_tests': ['t-test', 'anova'],
                'visualization_types': ['plots', 'charts', 'heatmaps'],
                'report_formats': ['json', 'csv', 'html']
            }
        }
    
    def create_full_config_template(self) -> Dict[str, Any]:
        """Create full configuration template with all systems"""
        return {
            'version': '1.0',
            'description': 'Complete NLM system configuration',
            'timestamp': datetime.now().isoformat(),
            'brain': self.fill_brain_config_template({
                'description': 'Complete brain configuration',
                'parameters': {
                    'neuron_count': 1000,
                    'region_count': 1,
                    'connection_probability': 0.1,
                    'simulation_timestep': 0.001,
                    'random_seed': 42,
                    'stdp_ltp_weight': 0.01,
                    'stdp_ltd_weight': 0.012,
                    'enable_reward_modulation': True,
                    'enable_curiosity': True,
                    'enable_development': True,
                    'enable_structural_plasticity': True
                }
            }),
            'world': self.fill_world_config_template({
                'description': 'Complete world configuration',
                'parameters': {
                    'width': 20,
                    'height': 20,
                    'vision_width': 8,
                    'vision_height': 8,
                    'max_energy': 100.0,
                    'energy_decay_rate': 0.1,
                    'agent_start_x': 10.0,
                    'agent_start_y': 10.0
                }
            }),
            'experiment': self.fill_experiment_config_template({
                'description': 'Complete experiment configuration',
                'parameters': {
                    'name': 'comprehensive_nlm_experiment',
                    'description': 'Comprehensive NLM experiment with all systems',
                    'steps': 1000,
                    'timestep': 0.001,
                    'enable_logging': True,
                    'enable_checkpointing': True,
                    'checkpoint_interval': 100,
                    'results_output': 'comprehensive_results.json'
                }
            }),
            'presets': {
                'quick': {'brain': {'neuron_count': 100}},
                'standard': {'brain': {'neuron_count': 500}},
                'research': {'brain': {'neuron_count': 2000}},
                'production': {'brain': {'neuron_count': 5000}}
            }
        }
    
    def list_brain_configurations(self, show_defaults: bool):
        """List brain configurations"""
        print("\nBrain Configurations:")
        print("-" * 30)
        
        if show_defaults:
            print("\nDefault Configurations:")
            print("1. Quick Test - 100 neurons")
            print("2. Standard - 500 neurons") 
            print("3. Research - 2000 neurons")
            print("4. Production - 5000 neurons")
        
        # List files in configs directory
        config_dir = self.config_dir
        if config_dir.exists():
            print("\nSaved Configurations:")
            for config_file in config_dir.glob("*.json"):
                try:
                    with open(config_file, 'r') as f:
                        config = json.load(f)
                        name = config.get('parameters', {}).get('name', config_file.stem)
                        print(f"  - {name} ({config_file.name})")
                except:
                    print(f"  - {config_file.name} (invalid JSON)")
    
    def list_world_configurations(self, show_defaults: bool):
        """List world configurations"""
        print("\nWorld Configurations:")
        print("-" * 30)
        
        if show_defaults:
            print("\nDefault World Parameters:")
            print("1. Standard - 20x20 world, 8x8 vision")
            print("2. Large - 50x50 world, 16x16 vision")
            print("3. Small - 10x10 world, 4x4 vision")
    
    def list_experiment_configurations(self, show_defaults: bool):
        """List experiment configurations"""
        print("\nExperiment Configurations:")
        print("-" * 30)
        
        if show_defaults:
            print("\nExperiment Settings:")
            print("1. Short - 100 steps")
            print("2. Medium - 1000 steps")
            print("3. Long - 5000 steps")
            print("4. Extended - 10000 steps")

if __name__ == "__main__":
    manager = NLMConfigManager()
    manager.parser.parse_args()
```

### 2.3 Experiment Monitoring Utility

```python
#!/usr/bin/env python3
"""
NLM Experiment Monitor - Real-time monitoring of experiment progress
"""

import pynlm
import time
import threading
import argparse
import json
from pathlib import Path
from typing import Dict, List, Callable
from datetime import datetime
import sys

class NLMExperimentMonitor:
    """Monitor experiment progress in real-time"""
    
    def __init__(self):
        self.setup_argument_parser()
        self.monitoring_data = []
        self.monitoring_active = False
        self.monitor_thread = None
        self.data_file = Path("experiment_monitor.log")
    
    def setup_argument_parser(self):
        """Setup argument parser"""
        parser = argparse.ArgumentParser(
            description="NLM Experiment Monitor - Real-time monitoring of experiment progress",
            formatter_class=argparse.RawDescriptionHelpFormatter,
            epilog="""
Usage:
  nlm_monitor.py run --experiment experiment.json
  nlm_monitor.py web --port 8080 --host localhost
  nlm_monitor.py analyze --log-file experiment_monitor.log
  nlm_monitor.py export --format csv --output results.csv
"""
        )
        
        # Main commands
        subparsers = parser.add_subparsers(dest='command', help='Commands')
        
        # Run monitoring
        run_parser = subparsers.add_parser('run', help='Run experiment with monitoring')
        run_parser.add_argument('experiment', help='Experiment configuration file')
        run_parser.add_argument('--output', default='experiment_monitor.json',
                               help='Output file for monitoring data')
        run_parser.add_argument('--interval', type=float, default=0.5,
                               help='Monitoring interval in seconds')
        run_parser.add_argument('--max-steps', type=int, help='Maximum simulation steps')
        
        # Web interface
        web_parser = subparsers.add_parser('web', help='Start web monitoring interface')
        web_parser.add_argument('--port', type=int, default=8080,
                               help='Port for web interface (default: 8080)')
        web_parser.add_argument('--host', default='localhost',
                               help='Host for web interface (default: localhost)')
        web_parser.add_argument('--config', default='experiment_monitor.json',
                               help='Configuration file for web interface')
        
        # Analyze
        analyze_parser = subparsers.add_parser('analyze', help='Analyze monitoring data')
        analyze_parser.add_argument('--log-file', default='experiment_monitor.log',
                                   help='Log file to analyze')
        analyze_parser.add_argument('--format', choices=['json', 'csv', 'summary'],
                                   default='summary', help='Analysis output format')
        analyze_parser.add_argument('--output', help='Output file for analysis')
        
        # Export
        export_parser = subparsers.add_parser('export', help='Export monitoring data')
        export_parser.add_argument('--source', default='experiment_monitor.json',
                                  help='Source data file')
        export_parser.add_argument('--format', choices=['csv', 'json', 'txt'],
                                  default='csv', help='Export format')
        export_parser.add_argument('--output', required=True, help='Output file path')
        
        self.parser = parser
    
    def start_monitoring(self, experiment_file: str, output_file: str,
                        interval: float, max_steps: Optional[int]):
        """Start experiment monitoring"""
        print(f"Starting experiment monitoring for {experiment_file}")
        print(f"Monitoring data will be saved to: {output_file}")
        
        # Load experiment configuration
        with open(experiment_file, 'r') as f:
            experiment_config = json.load(f)
        
        # Create brain if configured
        brain = None
        if 'brain' in experiment_config:
            try:
                brain = self.create_brain_from_config(experiment_config['brain'])
            except Exception as e:
                print(f"Warning: Could not create brain: {e}")
        
        # Create world if configured
        world = None
        if 'world' in experiment_config:
            try:
                world = self.create_world_from_config(experiment_config['world'])
            except Exception as e:
                print(f"Warning: Could not create world: {e}")
        
        # Setup monitoring
        self.monitoring_data = []
        self.monitoring_active = True
        
        # Start monitoring thread
        self.monitor_thread = threading.Thread(
            target=self._monitoring_loop,
            args=(brain, world, experiment_config, interval, max_steps)
        )
        self.monitor_thread.daemon = True
        self.monitor_thread.start()
        
        print("Monitoring started. Press Ctrl+C to stop.")
        
        # Wait for monitoring to complete (if max_steps specified)
        if max_steps:
            # Simple approach: monitor until completion
            while self.monitoring_active:
                time.sleep(1)
                if self.monitoring_data and self.monitoring_data[-1]['step'] >= max_steps:
                    break
        else:
            # Wait indefinitely until interrupted
            try:
                while self.monitoring_active:
                    time.sleep(1)
            except KeyboardInterrupt:
                print("\nMonitoring stopped by user")
        
        # Save monitoring data
        self.save_monitoring_data(output_file)
        
        print(f"Monitoring data saved to: {output_file}")
    
    def create_brain_from_config(self, brain_config: Dict[str, Any]) -> pynlm.Brain:
        """Create brain from configuration"""
        # Create config
        config = pynlm.createDefaultConfig()
        
        # Set configuration values
        if 'parameters' in brain_config:
            params = brain_config['parameters']
            config.set("neuron_count", params.get('neuron_count', 500))
            config.set("region_count", params.get('region_count', 1))
            config.set("connection_probability", params.get('connection_probability', 0.1))
            config.set("simulation_timestep", params.get('timestep', 0.001))
            config.set("random_seed", params.get('random_seed', 42))
        
        # Create brain
        brain = pynlm.createBrain(config)
        brain.initialize()
        
        return brain
    
    def create_world_from_config(self, world_config: Dict[str, Any]) -> pynlm.SimpleWorld:
        """Create world from configuration"""
        world = pynlm.createSimpleWorld()
        
        # Configure world
        if 'parameters' in world_config:
            params = world_config['parameters']
            world.configure(
                params.get('width', 20),
                params.get('height', 20),
                params.get('vision_width', 8),
                params.get('vision_height', 8)
            )
            
            # Set agent start
            if 'agent_start_x' in params and 'agent_start_y' in params:
                world.setAgentStart(params['agent_start_x'], params['agent_start_y'])
        
        return world
    
    def _monitoring_loop(self, brain: Optional[pynlm.Brain], world: Optional[pynlm.SimpleWorld],
                        experiment_config: Dict[str, Any], interval: float,
                        max_steps: Optional[int]):
        """Main monitoring loop"""
        step = 0
        last_time = time.time()
        
        while self.monitoring_active:
            iteration_start = time.time()
            
            # Collect monitoring data
            data_point = self.collect_monitoring_data(
                brain, world, experiment_config, step, iteration_start
            )
            
            self.monitoring_data.append(data_point)
            
            # Save to log file
            self.save_log_entry(data_point)
            
            # Check if experiment should stop
            if max_steps and step >= max_steps:
                break
            
            # Advance simulation
            if brain:
                brain.step(step, step * experiment_config.get('timestep', 0.001))
            
            # Update world
            if world:
                world.update(experiment_config.get('timestep', 0.001))
            
            step += 1
            
            # Calculate sleep time
            elapsed = time.time() - iteration_start
            sleep_time = interval - elapsed
            if sleep_time > 0:
                time.sleep(sleep_time)
    
    def collect_monitoring_data(self, brain: Optional[pynlm.Brain], world: Optional[pynlm.SimpleWorld],
                               config: Dict[str, Any], step: int, timestamp: float) -> Dict[str, Any]:
        """Collect monitoring data for current step"""
        data = {
            'timestamp': datetime.fromtimestamp(timestamp).isoformat(),
            'step': step,
            'time': step * config.get('timestep', 0.001),
        }
        
        # Collect brain metrics
        if brain:
            data.update({
                'brain': {
                    'total_spikes': brain.getTotalSpikeCount(),
                    'average_firing_rate': brain.getAverageFiringRate(),
                    'active_neurons': brain.getActiveNeuronCount(),
                    'firing_neurons': brain.getFiringNeuronCount(),
                    'ei_ratio': brain.getExcitationInhibitionRatio(),
                    'neuron_count': brain.getTotalNeuronCount(),
                    'synapse_count': brain.getTotalSynapseCount(),
                }
            })
            
            # Add neuromodulation data if available
            if hasattr(brain, 'getDopamine') and brain.getDopamine():
                data['brain']['dopamine_level'] = brain.getDopamine().getLevel()
            
            if hasattr(brain, 'getCuriosity') and brain.getCuriosity():
                data['brain']['curiosity_level'] = brain.getCuriosity().getLevel()
            
            if hasattr(brain, 'getDevelopmentSystem') and brain.getDevelopmentSystem():
                data['brain']['developmental_stage'] = int(brain.getDevelopmentalStage())
        
        # Collect world metrics
        if world:
            data.update({
                'world': {
                    'simulation_time': world.getSimulationTime(),
                    'width': world.getWidth(),
                    'height': world.getHeight(),
                    'max_energy': world.getMaxEnergy(),
                    'current_energy': world.getAgentBody().energy if hasattr(world.getAgentBody(), 'energy') else 0.0,
                    'agent_position': {'x': world.getAgentBody().x, 'y': world.getAgentBody().y}
                    if hasattr(world, 'getAgentBody') else {'x': 0, 'y': 0},
                    'agent_orientation': world.getAgentBody().orientation 
                    if hasattr(world.getAgentBody(), 'orientation') else 0.0,
                }
            })
        
        # Add experiment metrics
        data['experiment'] = {
            'memory_usage': self.get_memory_usage(),
            'cpu_usage': self.get_cpu_usage(),
            'process_id': os.getpid(),
        }
        
        return data
    
    def get_memory_usage(self) -> Dict[str, float]:
        """Get memory usage information"""
        try:
            import psutil
            process = psutil.Process(os.getpid())
            memory_info = process.memory_info()
            
            return {
                'rss_mb': memory_info.rss / 1024 / 1024,
                'vms_mb': memory_info.vms / 1024 / 1024,
                'shared_mb': memory_info.shared / 1024 / 1024,
                'text_mb': memory_info.text / 1024 / 1024,
                'data_mb': memory_info.data / 1024 / 1024
            }
        except ImportError:
            return {'error': 'psutil not available'}
    
    def get_cpu_usage(self) -> Dict[str, float]:
        """Get CPU usage information"""
        try:
            import psutil
            return {
                'cpu_percent': psutil.cpu_percent(interval=0.1),
                'cpu_count': psutil.cpu_count(),
                'memory_percent': psutil.virtual_memory().percent,
                'swap_percent': psutil.swap_memory().percent
            }
        except ImportError:
            return {'error': 'psutil not available'}
    
    def save_log_entry(self, data: Dict[str, Any]):
        """Save monitoring data entry to log file"""
        try:
            with open(self.data_file, 'a') as f:
                f.write(json.dumps(data) + '\n')
        except Exception as e:
            print(f"Warning: Could not save log entry: {e}")
    
    def save_monitoring_data(self, output_file: str):
        """Save monitoring data to file"""
        try:
            with open(output_file, 'w') as f:
                json.dump(self.monitoring_data, f, indent=2, default=str)
            print(f"Monitoring data saved to: {output_file}")
        except Exception as e:
            print(f"Warning: Could not save monitoring data: {e}")
    
    def analyze_data(self, log_file: str, output_format: str, output_file: str):
        """Analyze monitoring data"""
        print(f"Analyzing monitoring data from: {log_file}")
        
        # Load monitoring data
        with open(log_file, 'r') as f:
            data = [json.loads(line) for line in f if line.strip()]
        
        # Perform analysis based on output format
        if output_format == 'summary':
            self.generate_summary_analysis(data, output_file)
        elif output_format == 'json':
            self.generate_json_analysis(data, output_file)
        elif output_format == 'csv':
            self.generate_csv_analysis(data, output_file)
        
        print(f"Analysis saved to: {output_file}")
    
    def generate_summary_analysis(self, data: List[Dict[str, Any]], output_file: str):
        """Generate summary analysis"""
        if not data:
            return
        
        summary = {
            'total_steps': len(data),
            'time_range': {
                'start': data[0]['time'] if data else 0,
                'end': data[-1]['time'] if data else 0,
                'duration': (data[-1]['time'] - data[0]['time']) if len(data) > 1 else 0
            },
            'brain_metrics': self.analyze_brain_metrics(data),
            'world_metrics': self.analyze_world_metrics(data),
            'performance_metrics': self.analyze_performance_metrics(data)
        }
        
        with open(output_file, 'w') as f:
            json.dump(summary, f, indent=2)
    
    def analyze_brain_metrics(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze brain metrics"""
        if not data:
            return {}
        
        brain_data = [d for d in data if 'brain' in d]
        
        if not brain_data:
            return {}
        
        metrics = {}
        
        # Spike analysis
        spikes = [b['brain']['total_spikes'] for b in brain_data]
        metrics['spikes'] = {
            'total': sum(spikes),
            'average_per_step': sum(spikes) / len(spikes),
            'max_spikes_per_step': max(spikes) - min(spikes) if len(spikes) > 1 else 0,
            'average_rate': sum(spikes) / data[-1]['time'] if data[-1]['time'] > 0 else 0
        }
        
        # Firing rate analysis
        rates = [b['brain']['average_firing_rate'] for b in brain_data]
        metrics['firing_rate'] = {
            'average': sum(rates) / len(rates),
            'min': min(rates),
            'max': max(rates),
            'std': (sum((x - sum(rates)/len(rates))**2 for x in rates) / len(rates))**0.5
        }
        
        # Active neurons analysis
        active = [b['brain']['active_neurons'] for b in brain_data]
        metrics['active_neurons'] = {
            'average': sum(active) / len(active),
            'min': min(active),
            'max': max(active)
        }
        
        return metrics
    
    def analyze_world_metrics(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze world metrics"""
        if not data:
            return {}
        
        world_data = [d for d in data if 'world' in d]
        
        if not world_data:
            return {}
        
        return {
            'simulation_time': {
                'average': sum(w['world']['simulation_time'] for w in world_data) / len(world_data),
                'min': min(w['world']['simulation_time'] for w in world_data),
                'max': max(w['world']['simulation_time'] for w in world_data)
            },
            'agent_position': {
                'x': [w['world']['agent_position']['x'] for w in world_data],
                'y': [w['world']['agent_position']['y'] for w in world_data]
            }
        }
    
    def analyze_performance_metrics(self, data: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze performance metrics"""
        if not data:
            return {}
        
        return {
            'steps_per_second': len(data) / data[-1]['time'] if data[-1]['time'] > 0 else 0,
            'data_points_per_second': len(data) / (data[-1]['time'] - data[0]['time']) if len(data) > 1 and (data[-1]['time'] - data[0]['time']) > 0 else 0
        }
    
    def generate_json_analysis(self, data: List[Dict[str, Any]], output_file: str):
        """Generate JSON analysis"""
        analysis = {
            'summary': self.analyze_brain_metrics(data),
            'world_metrics': self.analyze_world_metrics(data),
            'performance_metrics': self.analyze_performance_metrics(data),
            'raw_data': data
        }
        
        with open(output_file, 'w') as f:
            json.dump(analysis, f, indent=2)
    
    def generate_csv_analysis(self, data: List[Dict[str, Any]], output_file: str):
        """Generate CSV analysis"""
        try:
            import csv
            
            with open(output_file, 'w', newline='') as f:
                writer = csv.writer(f)
                
                # Write header
                header = ['step', 'time', 'total_spikes', 'average_firing_rate',
                         'active_neurons', 'simulation_time']
                writer.writerow(header)
                
                # Write data
                for entry in data:
                    row = [
                        entry.get('step', 0),
                        entry.get('time', 0),
                        entry.get('brain', {}).get('total_spikes', 0),
                        entry.get('brain', {}).get('average_firing_rate', 0),
                        entry.get('brain', {}).get('active_neurons', 0),
                        entry.get('world', {}).get('simulation_time', 0)
                    ]
                    writer.writerow(row)
                    
        except Exception as e:
            print(f"Error generating CSV analysis: {e}")
            raise
    
    def export_data(self, source_file: str, output_file: str, export_format: str):
        """Export monitoring data to different format"""
        try:
            # Load monitoring data
            with open(source_file, 'r') as f:
                if source_file.endswith('.json'):
                    data = json.load(f)
                else:
                    # Try to parse as JSON lines
                    data = [json.loads(line) for line in f if line.strip()]
            
            # Export based on format
            if export_format == 'csv':
                self.export_csv(data, output_file)
            elif export_format == 'json':
                with open(output_file, 'w') as f:
                    json.dump(data, f, indent=2)
            elif export_format == 'txt':
                self.export_txt(data, output_file)
            
            print(f"Data exported to: {output_file}")
            
        except Exception as e:
            print(f"Error exporting data: {e}")
            raise
    
    def export_csv(self, data: List[Dict[str, Any]], output_file: str):
        """Export data as CSV"""
        try:
            import csv
            
            with open(output_file, 'w', newline='') as f:
                writer = csv.writer(f)
                
                # Determine all possible keys
                all_keys = set()
                for entry in data:
                    all_keys.update(entry.keys())
                
                # Remove nested keys that need special handling
                nested_keys = {'brain', 'world', 'experiment'}
                simple_keys = all_keys - nested_keys
                
                # Flatten nested keys
                flattened_keys = []
                for key in simple_keys:
                    flattened_keys.append(key)
                    if key in ['brain', 'world', 'experiment']:
                        if data and key in data[0]:
                            for sub_key in data[0][key].keys():
                                flattened_keys.append(f"{key}_{sub_key}")
                
                # Write header
                writer.writerow(flattened_keys)
                
                # Write data
                for entry in data:
                    row = []
                    for key in flattened_keys:
                        if '_' in key and key.split('_')[0] in ['brain', 'world', 'experiment']:
                            # Handle nested keys
                            main_key, sub_key = key.split('_', 1)
                            value = entry.get(main_key, {}).get(sub_key, '')
                        else:
                            value = entry.get(key, '')
                        row.append(value)
                    writer.writerow(row)
                    
        except Exception as e:
            print(f"Error exporting CSV: {e}")
            raise
    
    def export_txt(self, data: List[Dict[str, Any]], output_file: str):
        """Export data as human-readable text"""
        with open(output_file, 'w') as f:
            f.write("NLM Experiment Monitoring Data")
            f.write("=" * 50)
            f.write(f"\nTotal data points: {len(data)}")
            f.write(f"\nTime range: {data[0]['time'] if data else 0} - {data[-1]['time'] if data else 0}")
            f.write("\n\nDetailed data:\n")
            
            for i, entry in enumerate(data):
                f.write(f"\nEntry {i}:")
                for key, value in entry.items():
                    f.write(f"\n  {key}: {value}")
    
    def run(self, args):
        """Main entry point"""
        if args.command == 'run':
            self.start_monitoring(args.experiment, args.output, args.interval, args.max_steps)
        elif args.command == 'web':
            self.run_web_interface(args.port, args.host, args.config)
        elif args.command == 'analyze':
            self.analyze_data(args.log_file, args.format, args.output)
        elif args.command == 'export':
            self.export_data(args.source, args.output, args.format)
        else:
            self.parser.error(f"Unknown command: {args.command}")
    
    def run_web_interface(self, port: int, host: str, config_file: str):
        """Run web monitoring interface"""
        print(f"Web monitoring interface would start on http://{host}:{port}")
        print("Note: Web interface implementation not yet complete.")
        print("For now, use command-line analysis and export tools.")

if __name__ == "__main__":
    monitor = NLMExperimentMonitor()
    monitor.parser.parse_args()
```

---

## 2. Experiment Configuration Template

### 2.1 Basic Experiment Configuration

```json
{
  "name": "default_experiment",
  "description": "Default NLM experiment configuration",
  "version": "1.0",
  "timestamp": "2026-09-22T15:37:17.000Z",
  
  "brain": {
    "version": "1.0",
    "description": "Brain configuration",
    "parameters": {
      "neuron_count": 1000,
      "region_count": 1,
      "connection_probability": 0.1,
      "simulation_timestep": 0.001,
      "random_seed": 42,
      "stdp_ltp_weight": 0.01,
      "stdp_ltd_weight": 0.012,
      "enable_reward_modulation": true,
      "enable_curiosity": true,
      "enable_development": true,
      "enable_structural_plasticity": false
    },
    "learning_systems": {
      "plasticity": {
        "stdp": {
          "enabled": true,
          "learning_rate": 0.01
        },
        "hebbian": {
          "enabled": true,
          "learning_rate": 0.01
        },
        "structural": {
          "enabled": false,
          "synaptogenesis_rate": 0.0001,
          "pruning_rate": 0.00001
        }
      },
      "neuromodulation": {
        "dopamine": {
          "enabled": true,
          "range": [-1.0, 1.0]
        },
        "curiosity": {
          "enabled": true,
          "exploration_drive": 0.5
        }
      }
    }
  },
  
  "world": {
    "version": "1.0",
    "description": "World configuration",
    "parameters": {
      "width": 20,
      "height": 20,
      "vision_width": 8,
      "vision_height": 8,
      "max_energy": 100.0,
      "energy_decay_rate": 0.1,
      "agent_start_x": 10.0,
      "agent_start_y": 10.0
    }
  },
  
  "experiment": {
    "version": "1.0",
    "description": "Experiment configuration",
    "parameters": {
      "name": "default_experiment",
      "description": "Default NLM experiment",
      "steps": 1000,
      "timestep": 0.001,
      "enable_logging": true,
      "enable_checkpointing": false,
      "checkpoint_interval": 100,
      "results_output": "experiment_results.json"
    },
    "metrics": {
      "collect_spikes": true,
      "collect_firing_rates": true,
      "collect_memory_usage": true,
      "collect_reward": true,
      "collect_time_metrics": true
    }
  },
  
  "presets": {
    "quick": {"brain": {"neuron_count": 100}},
    "standard": {"brain": {"neuron_count": 500}},
    "research": {"brain": {"neuron_count": 2000}},
    "production": {"brain": {"neuron_count": 5000}}
  }
}
```

---

## 3. Quick Start Examples

### 3.1 Running a Simple Experiment

```bash
# Create a simple experiment
python nlm_cli.py experiment --brain-config default.cfg --steps 1000 --output exp1.json

# Run with custom world
python nlm_cli.py experiment \
  --brain-config brain_config.json \
  --world-config world_config.json \
  --steps 500 \
  --output experiment_results.json

# Run benchmark
python nlm_cli.py benchmark --neuron-counts 100 500 1000 --timesteps 100 --repeats 3

# Replay experiment
python nlm_cli.py replay --checkpoint brain_checkpoint.bin --output replay_results.json

# Compare experiments
python nlm_cli.py compare --experiments exp1.json exp2.json --metric all --output comparison.html

# Start monitoring
python nlm_monitor.py run --experiment experiment.json --output monitor_data.json
```

### 3.2 Using Configuration Manager

```bash
# Create a new configuration
python nlm_cli.py config create experiment my_experiment.json --type full

# List available configurations
python nlm_cli.py config list --type all --show-defaults

# Export configuration
python nlm_cli.py config export configs/my_config.json --format yaml --output exported_config.yaml

# Import configuration
python nlm_cli.py config import external_config.yaml --format yaml --output my_config.json

# Validate configuration
python nlm_cli.py config validate my_config.json

# Create backup
python nlm_cli.py config backup --directory backups/2026-09-22

# Restore configuration
python nlm_cli.py config restore backups/2026-09-22/config.json --output restored_config.json
```

### 3.3 Experiment Monitoring

```bash
# Run experiment with monitoring
python nlm_monitor.py run --experiment experiment_config.json --output monitor_data.json --interval 0.1

# Start web monitor
python nlm_monitor.py web --port 8080 --host localhost

# Analyze monitoring data
python nlm_monitor.py analyze --log-file experiment_monitor.log --format summary

# Export monitoring data
python nlm_monitor.py export --source monitor_data.json --format csv --output results.csv
```

---

## 4. Installation and Setup

### 4.1 Prerequisites

```bash
# Install NLM
pip install pynlm

# Install system dependencies
sudo apt-get update
sudo apt-get install -y cmake build-essential python3-dev python3-pip

# Clone NLM repository
clone https://github.com/nlm-project/nlm.git
cd nlm
```

### 4.2 Building NLM

```bash
# Build NLM from source
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
```

### 4.3 Install Python Bindings

```bash
# Install Python bindings
cd python
cmake .
make
pip install pynlm
```

### 4.4 Installation

```bash
# Install CLI utilities
sudo cp nlm_cli.py /usr/local/bin/
sudo cp nlm_monitor.py /usr/local/bin/
sudo chmod +x /usr/local/bin/nlm_cli.py /usr/local/bin/nlm_monitor.py

# Set up configuration directory
mkdir -p ~/nlm/configs
mkdir -p ~/nlm/backups
mkdir -p ~/nlm/results

# Create sample configuration
python -c "
import pynlm, json, os
config = pynlm.createDefaultConfig()
config.set('neuron_count', 1000)
config.set('region_count', 1)
config.set('random_seed', 42)
with open('~/nlm/configs/default.json', 'w') as f:
    json.dump(config, f, indent=2)
"
```

---

## 5. Usage Examples

### 5.1 Complete Experiment Pipeline

```bash
#!/bin/bash
# NLM Complete Experiment Pipeline Script

set -e

EXPERIMENT_NAME="nlm_experiment_$(date +%Y%m%d_%H%M%S)"
OUTPUT_DIR="results/$EXPERIMENT_NAME"
mkdir -p "$OUTPUT_DIR"

# Configuration
BRAIN_CONFIG="configs/experiment_config.json"
WORLD_CONFIG="configs/world_config.json"
EXPERIMENT_CONFIG="configs/experiment_settings.json"

# Step 1: Validate configurations
echo "Validating configurations..."
if [ ! -f "$BRAIN_CONFIG" ]; then
    echo "Error: Brain configuration not found: $BRAIN_CONFIG"
    exit 1
fi

if [ ! -f "$WORLD_CONFIG" ]; then
    echo "Warning: World configuration not found: $WORLD_CONFIG"
fi

# Step 2: Run experiment
echo "Running experiment: $EXPERIMENT_NAME"
python nlm_cli.py experiment \
  --brain-config "$BRAIN_CONFIG" \
  --world-config "$WORLD_CONFIG" \
  --steps 1000 \
  --output "$OUTPUT_DIR/experiment_results.json" \
  --verbose

# Step 3: Run benchmark
echo "Running benchmark..."
python nlm_cli.py benchmark \
  --neuron-counts 100 500 1000 \
  --timesteps 200 \
  --repeats 2 \
  --output "$OUTPUT_DIR/benchmark_results.json"

# Step 4: Analyze results
echo "Analyzing results..."
python nlm_cli.py analyze \
  --log-file "experiment_monitor.log" \
  --format json \
  --output "$OUTPUT_DIR/analysis.json"

# Step 5: Generate report
echo "Generating experiment report..."
python -c "
import pynlm, json, os
results_file = '$OUTPUT_DIR/experiment_results.json'
if os.path.exists(results_file):
    with open(results_file, 'r') as f:
        results = json.load(f)
    print(f'Experiment completed successfully!')
    print(f'Total spikes: {results.get(\"metrics\", {}).get(\"total_spikes\", 0)}')
    print(f'Final firing rate: {results.get(\"metrics\", {}).get(\"firing_rate\", 0):.3f} Hz')
    print(f'Efficiency: {results.get(\"metrics\", {}).get(\"efficiency\", 0):.2f} steps/sec')
else:
    print('Results file not found')
"

echo "Experiment pipeline completed successfully!"
echo "Results saved to: $OUTPUT_DIR"
```

### 5.2 Web-Based Monitoring

```bash
# Start web monitoring (requires additional dependencies)
# pip install flask matplotlib plotly

python nlm_monitor.py web --port 8080 --host localhost

# Access web interface at http://localhost:8080
```

---

## 6. Troubleshooting

### 6.1 Common Issues

**Issue**: Experiment fails to start
**Solution**: Check configuration files and ensure NLM is properly built

**Issue**: Monitoring data not saved
**Solution**: Check disk space and file permissions

**Issue**: Web interface not accessible
**Solution**: Check port availability and firewall settings

**Issue**: Large dataset causes memory issues
**Solution**: Increase system memory or reduce experiment size

### 6.2 Error Codes

- `0`: Success
- `1`: General error
- `2`: Configuration error
- `3`: Experiment error
- `4`: Monitoring error

---

## 7. Development Guidelines

### 7.1 Contribution Guidelines

1. **Code Style**: Follow PEP 8 for Python code
2. **Documentation**: Update README.md and docstrings
3. **Testing**: Add unit tests for new functionality
4. **Performance**: Profile and optimize hot paths
5. **Compatibility**: Maintain backward compatibility

### 7.2 Testing Commands

```bash
# Run unit tests
python -m pytest tests/ -v

# Run integration tests
python -m pytest integration_tests/ -v

# Run performance tests
python benchmark_runner.py

# Run smoke tests
python smoke_tests.py
```

### 7.3 Building from Source

```bash
# Clone repository
git clone https://github.com/nlm-project/nlm.git
cd nlm

# Build NLM core
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4

# Build Python bindings
cd python
cmake .
make

# Install Python package
pip install -e .
```

---

## 8. License and Attribution

This software is licensed under the MIT License. See LICENSE file for details.

## 9. Contact and Support

For questions and support:
- GitHub Issues: https://github.com/nlm-project/nlm/issues
- Documentation: https://nlm.readthedocs.io
- Discord: https://discord.gg/nlm
- Email: support@nlm-project.org

---

**Last Updated**: 2026-09-22
**Version**: 1.0.0
**Author**: NLM Project Team

*This documentation is continuously updated. Check the repository for the latest version.*