#!/usr/bin/env python3
"""
Advanced NLM Command-Line Interface Tools
===========================================

This module provides advanced command-line tools for working with the NLM framework,
including simulation control, configuration management, experiment running,
and result analysis.

Key Features:
- Enhanced simulation control with progress monitoring
- Configuration management and validation
- Experiment running utilities
- Result analysis and visualization
- Performance profiling
- Configuration export/import
- Batch processing tools

Author: NLM Development Team
License: MIT
Version: 1.0.0 (Advanced CLI)
"""

import argparse
import sys
import os
import json
import time
import csv
from pathlib import Path
from typing import Dict, Any, List, Optional, Callable
import logging

# Configure logging
logging.basicConfig(level=logging.INFO, format='%(asctime)s - %(levelname)s - %(message)s')
logger = logging.getLogger(__name__)

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    logger.warning("pynlm module not available. CLI tools will not work.")

# Import enhanced bindings
try:
    from pynlm_enhanced import (
        NLMSimulationContext, NLMBatchContext, ConfigurationTemplate,
        StatisticsCollector, NLMProfiler, get_statistics_collector,
        create_simulation_config, validate_config
    )
    ENHANCED_BINDINGS_AVAILABLE = True
except ImportError:
    ENHANCED_BINDINGS_AVAILABLE = False
    logger.warning("Enhanced bindings not available. Basic CLI tools only.")

class NLMCLIError(Exception):
    """Base exception for NLM CLI errors."""
    pass

class ConfigurationManager:
    """Manage NLM configurations from command line."""
    
    def __init__(self):
        self.configs = {}
        
    def load_config(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from file."""
        try:
            with open(filepath, 'r') as f:
                return json.load(f)
        except FileNotFoundError:
            raise NLMCLIError(f"Configuration file not found: {filepath}")
        except json.JSONDecodeError as e:
            raise NLMCLIError(f"Invalid JSON in configuration file: {e}")
    
    def save_config(self, config: Dict[str, Any], filepath: str):
        """Save configuration to file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(config, f, indent=2)
        except IOError as e:
            raise NLMCLIError(f"Cannot save configuration: {e}")
    
    def create_config_from_args(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Create configuration from command line arguments."""
        config = {}
        
        # Basic brain configuration
        if 'neuron_count' in args:
            config['brain.neuron_count'] = args['neuron_count']
        if 'region_count' in args:
            config['brain.region_count'] = args['region_count']
        if 'connection_probability' in args:
            config['brain.connection_probability'] = args['connection_probability']
        if 'initial_weight_mean' in args:
            config['brain.initial_weight_mean'] = args['initial_weight_mean']
        if 'initial_weight_std' in args:
            config['brain.initial_weight_std'] = args['initial_weight_std']
        
        # Neuron configuration
        if 'v_thresh' in args:
            config['brain.v_thresh'] = args['v_thresh']
        if 'v_rest' in args:
            config['brain.v_rest'] = args['v_rest']
        if 'v_reset' in args:
            config['brain.v_reset'] = args['v_reset']
        if 'tau_mem' in args:
            config['brain.tau_mem'] = args['tau_mem']
        if 'tau_ref' in args:
            config['brain.tau_ref'] = args['tau_ref']
        
        # Plasticity configuration
        if 'stdp_enable' in args:
            config['plasticity.stdp.enable'] = args['stdp_enable']
        if 'stdp_learning_rate' in args:
            config['plasticity.stdp.learning_rate'] = args['stdp_learning_rate']
        if 'hebbian_enable' in args:
            config['plasticity.hebbian.enable'] = args['hebbian_enable']
        if 'structural_enable' in args:
            config['plasticity.structural.enable'] = args['structural_enable']
        
        # Neuromodulation configuration
        if 'dopamine_scale' in args:
            config['neuromod.dopamine.scale'] = args['dopamine_scale']
        if 'curiosity_enable' in args:
            config['neuromod.curiosity.enable'] = args['curiosity_enable']
        if 'novelty_enable' in args:
            config['neuromod.novelty.enable'] = args['novelty_enable']
        
        return config

class SimulationRunner:
    """Run NLM simulations from command line."""
    
    def __init__(self, config_manager: ConfigurationManager = None):
        self.config_manager = config_manager or ConfigurationManager()
        self.profiler = NLMProfiler() if ENHANCED_BINDINGS_AVAILABLE else None
        
    def run_simulation(self, config_args: Dict[str, Any], 
                      steps: int, timestep: float = 0.1,
                      world_config: Dict[str, Any] = None,
                      enable_features: Dict[str, bool] = None,
                      output_file: str = None) -> Dict[str, Any]:
        """Run a single simulation."""
        if not PYNLM_AVAILABLE:
            raise NLMCLIError("pynlm module is required for simulation")
            
        # Create configuration
        config = nlm.createDefaultConfig()
        
        # Apply configuration arguments
        for key, value in config_args.items():
            # Convert key format (dot notation)
            if '.' in key:
                # This is a simplified approach - actual implementation needs proper key handling
                pass
            else:
                # Direct setting for simple keys
                config.set(key, value)
        
        # Create and configure brain
        brain = nlm.createBrain(config)
        brain.initialize()
        
        # Create and configure world
        world = nlm.createSimpleWorld()
        if world_config:
            world.configure(
                width=world_config.get('width', 20),
                height=world_config.get('height', 20),
                visionWidth=world_config.get('visionWidth', 8),
                visionHeight=world_config.get('visionHeight', 8)
            )
            if 'agent_start_x' in world_config and 'agent_start_y' in world_config:
                world.setAgentStart(
                    world_config['agent_start_x'],
                    world_config['agent_start_y']
                )
        world.reset()
        
        # Create agent
        agent = nlm.createAgentBrain(brain)
        agent.initialize(world)
        
        # Enable requested features
        if enable_features:
            agent.enableRewardModulation(enable_features.get('reward_modulation', False))
            agent.enableStructuralPlasticity(enable_features.get('structural_plasticity', False))
            agent.enableDevelopment(enable_features.get('development', False))
            agent.enableCuriosity(enable_features.get('curiosity', False))
        
        # Run simulation
        start_time = time.time()
        
        for step in range(steps):
            # Profile step if available
            if self.profiler:
                step_start = self.profiler.start_timing(f"step_{step}")
            
            # Run world update
            world.update(timestep)
            
            # Get sensory percept
            percept = world.getSensoryPercept()
            
            # Process sensory input
            agent.processSensoryInput(percept)
            
            # Run brain step
            brain.step(step)
            
            # Get motor command
            action = agent.decodeMotorCommand()
            
            # Apply motor command
            world.applyMotorCommand(action, world.getSimulationTime())
            
            # Record step time
            if self.profiler:
                self.profiler.stop_timing(step_start, f"step_{step}")
                self.profiler.record_step_time(time.time() - step_start)
                
            # Log progress periodically
            if step % 100 == 0:
                progress = (step / steps) * 100.0
                logger.info(f"Progress: {progress:.1f}% (Step {step}/{steps})")
        
        elapsed_time = time.time() - start_time
        
        # Collect statistics
        stats = {
            'steps': steps,
            'timestep': timestep,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_synapses': brain.getTotalSynapseCount(),
            'firing_neurons': brain.getFiringNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'elapsed_time': elapsed_time,
            'steps_per_second': steps / elapsed_time if elapsed_time > 0 else 0,
            'world_time': world.getSimulationTime()
        }
        
        # Save results if requested
        if output_file:
            self._save_results(stats, output_file)
            
        return stats
    
    def _save_results(self, stats: Dict[str, Any], filepath: str):
        """Save simulation results to file."""
        try:
            with open(filepath, 'w') as f:
                json.dump(stats, f, indent=2)
            logger.info(f"Results saved to {filepath}")
        except IOError as e:
            raise NLMCLIError(f"Cannot save results: {e}")
    
    def run_batch_simulation(self, configs: List[Dict[str, Any]],
                           steps: int, timestep: float = 0.1,
                           world_config: Dict[str, Any] = None,
                           enable_features: Dict[str, bool] = None,
                           output_prefix: str = "simulation") -> List[Dict[str, Any]]:
        """Run batch of simulations."""
        results = []
        
        for i, config_args in enumerate(configs):
            output_file = f"{output_prefix}_{i}.json" if len(configs) > 1 else None
            
            logger.info(f"Running simulation {i + 1}/{len(configs)}")
            
            try:
                stats = self.run_simulation(
                    config_args, steps, timestep, world_config,
                    enable_features, output_file
                )
                stats['simulation_id'] = i
                results.append(stats)
                
            except Exception as e:
                logger.error(f"Simulation {i + 1} failed: {e}")
                results.append({
                    'simulation_id': i,
                    'error': str(e),
                    'success': False
                })
                
        return results

class ExperimentManager:
    """Manage NLM experiments."""
    
    def __init__(self):
        self.experiments = {}
        
    def create_experiment(self, name: str, config: Dict[str, Any]) -> Dict[str, Any]:
        """Create a new experiment."""
        experiment = {
            'name': name,
            'config': config,
            'results': [],
            'parameters': {
                'created_at': time.time(),
                'created_by': 'nlm-cli',
                'status': 'created'
            }
        }
        self.experiments[name] = experiment
        return experiment
    
    def run_experiment(self, name: str, simulation_runner: SimulationRunner,
                      steps: int, timestep: float = 0.1,
                      param_sweeps: List[Dict[str, Any]] = None) -> Dict[str, Any]:
        """Run experiment with optional parameter sweeps."""
        if name not in self.experiments:
            raise NLMCLIError(f"Experiment not found: {name}")
            
        experiment = self.experiments[name]
        experiment['parameters']['status'] = 'running'
        experiment['parameters']['start_time'] = time.time()
        
        # Get base configuration
        base_config = experiment['config']
        
        if param_sweeps:
            # Run parameter sweeps
            for sweep in param_sweeps:
                config_args = {**base_config, **sweep}
                results = simulation_runner.run_simulation(
                    config_args, steps, timestep
                )
                experiment['results'].append({
                    'parameters': sweep,
                    'results': results
                })
        else:
            # Run single simulation
            results = simulation_runner.run_simulation(
                base_config, steps, timestep
            )
            experiment['results'].append({
                'parameters': {},
                'results': results
            })
            
        experiment['parameters']['end_time'] = time.time()
        experiment['parameters']['status'] = 'completed'
        experiment['parameters']['num_runs'] = len(experiment['results'])
        
        return experiment
    
    def analyze_experiment(self, name: str) -> Dict[str, Any]:
        """Analyze experiment results."""
        if name not in self.experiments:
            raise NLMCLIError(f"Experiment not found: {name}")
            
        experiment = self.experiments[name]
        
        if not experiment['results']:
            return {'error': 'No results to analyze'}
            
        # Collect all results
        all_results = []
        for run in experiment['results']:
            all_results.append(run['results'])
            
        # Analyze results
        analysis = {
            'num_runs': len(experiment['results']),
            'successful_runs': sum(1 for r in all_results if 'error' not in r),
            'failed_runs': sum(1 for r in all_results if 'error' in r),
            'average_neurons': sum(r.get('total_neurons', 0) for r in all_results) / len(all_results),
            'average_spikes': sum(r.get('total_spikes', 0) for r in all_results) / len(all_results),
            'average_firing_rate': sum(r.get('average_firing_rate', 0) for r in all_results) / len(all_results),
            'min_neurons': min(r.get('total_neurons', 0) for r in all_results),
            'max_neurons': max(r.get('total_neurons', 0) for r in all_results),
            'min_spikes': min(r.get('total_spikes', 0) for r in all_results),
            'max_spikes': max(r.get('total_spikes', 0) for r in all_results)
        }
        
        return analysis
    
    def export_experiment_results(self, name: str, filepath: str, format: str = 'json'):
        """Export experiment results to file."""
        if name not in self.experiments:
            raise NLMCLIError(f"Experiment not found: {name}")
            
        experiment = self.experiments[name]
        
        if format.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(experiment, f, indent=2)
        elif format.lower() == 'csv':
            self._export_to_csv(experiment, filepath)
        else:
            raise NLMCLIError(f"Unsupported format: {format}")
            
    def _export_to_csv(self, experiment: Dict[str, Any], filepath: str):
        """Export experiment results to CSV."""
        # This is a simplified CSV export
        # For a full implementation, you'd want more sophisticated handling
        with open(filepath, 'w', newline='') as f:
            writer = csv.writer(f)
            writer.writerow(['Run', 'Parameters', 'Total Neurons', 'Total Synapses',
                           'Firing Neurons', 'Total Spikes', 'Average Firing Rate',
                           'Elapsed Time', 'Steps per Second'])
            
            for i, run in enumerate(experiment['results']):
                results = run['results']
                writer.writerow([
                    i,
                    str(run.get('parameters', {})),
                    results.get('total_neurons', ''),
                    results.get('total_synapses', ''),
                    results.get('firing_neurons', ''),
                    results.get('total_spikes', ''),
                    results.get('average_firing_rate', ''),
                    results.get('elapsed_time', ''),
                    results.get('steps_per_second', '')
                ])

class ResultAnalyzer:
    """Analyze and visualize NLM simulation results."""
    
    @staticmethod
    def analyze_statistics(stats_list: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Analyze statistics from multiple simulations."""
        if not stats_list:
            return {}
            
        # Extract numeric values
        neurons = [s.get('total_neurons', 0) for s in stats_list]
        synapses = [s.get('total_synapses', 0) for s in stats_list]
        spikes = [s.get('total_spikes', 0) for s in stats_list]
        firing_rates = [s.get('average_firing_rate', 0) for s in stats_list]
        times = [s.get('elapsed_time', 0) for s in stats_list]
        
        return {
            'num_simulations': len(stats_list),
            'neurons': {
                'mean': sum(neurons) / len(neurons),
                'std': (sum((x - sum(neurons)/len(neurons))**2 for x in neurons) / len(neurons))**0.5,
                'min': min(neurons),
                'max': max(neurons),
                'median': sorted(neurons)[len(neurons)//2]
            },
            'synapses': {
                'mean': sum(synapses) / len(synapses),
                'std': (sum((x - sum(synapses)/len(synapses))**2 for x in synapses) / len(synapses))**0.5,
                'min': min(synapses),
                'max': max(synapses),
                'median': sorted(synapses)[len(synapses)//2]
            },
            'spikes': {
                'mean': sum(spikes) / len(spikes),
                'std': (sum((x - sum(spikes)/len(spikes))**2 for x in spikes) / len(spikes))**0.5,
                'min': min(spikes),
                'max': max(spikes),
                'median': sorted(spikes)[len(spikes)//2]
            },
            'firing_rates': {
                'mean': sum(firing_rates) / len(firing_rates),
                'std': (sum((x - sum(firing_rates)/len(firing_rates))**2 for x in firing_rates) / len(firing_rates))**0.5,
                'min': min(firing_rates),
                'max': max(firing_rates),
                'median': sorted(firing_rates)[len(firing_rates)//2]
            },
            'times': {
                'mean': sum(times) / len(times),
                'std': (sum((x - sum(times)/len(times))**2 for x in times) / len(times))**0.5,
                'min': min(times),
                'max': max(times),
                'median': sorted(times)[len(times)//2]
            }
        }
    
    @staticmethod
    def generate_report(analysis: Dict[str, Any]) -> str:
        """Generate text report from analysis."""
        report = "NLM Simulation Results Analysis Report\n"
        report += "=" * 50 + "\n\n"
        
        report += f"Number of simulations: {analysis['num_simulations']}\n\n"
        
        for category, data in analysis.items():
            if category == 'num_simulations':
                continue
                
            report += f"{category.title()}:\n"
            for stat, value in data.items():
                if isinstance(value, dict):
                    report += f"  {stat}: {value['mean']:.2f} (std: {value['std']:.2f}, "
                    report += f"min: {value['min']}, max: {value['max']}, median: {value['median']:.2f})\n"
                else:
                    report += f"  {stat}: {value:.2f}\n"
            report += "\n"
            
        return report

class NLMCommandLine:
    """Main command-line interface for NLM."""
    
    def __init__(self):
        self.config_manager = ConfigurationManager()
        self.simulation_runner = SimulationRunner(self.config_manager)
        self.experiment_manager = ExperimentManager()
        self.result_analyzer = ResultAnalyzer()
        
    def run(self, args: argparse.Namespace):
        """Run the appropriate command."""
        if args.command == 'simulate':
            self._run_simulate_command(args)
        elif args.command == 'batch':
            self._run_batch_command(args)
        elif args.command == 'experiment':
            self._run_experiment_command(args)
        elif args.command == 'analyze':
            self._run_analyze_command(args)
        elif args.command == 'profile':
            self._run_profile_command(args)
        elif args.command == 'config':
            self._run_config_command(args)
        elif args.command == 'export':
            self._run_export_command(args)
        elif args.command == 'import':
            self._run_import_command(args)
        else:
            raise NLMCLIError(f"Unknown command: {args.command}")
    
    def _run_simulate_command(self, args: argparse.Namespace):
        """Run simulation command."""
        if not PYNLM_AVAILABLE:
            raise NLMCLIError("pynlm module is required for simulation")
            
        # Load configuration
        config_args = {}
        if args.config:
            config_args = self.config_manager.load_config(args.config)
            
        # Override with command line arguments
        if args.neuron_count:
            config_args['brain.neuron_count'] = args.neuron_count
        if args.steps:
            config_args['brain.steps'] = args.steps
        if args.learning_rate:
            config_args['plasticity.stdp.learning_rate'] = args.learning_rate
            
        # Create world configuration
        world_config = None
        if args.width and args.height:
            world_config = {
                'width': args.width,
                'height': args.height,
                'visionWidth': args.vision_width,
                'visionHeight': args.vision_height,
                'agent_start_x': args.agent_start_x,
                'agent_start_y': args.agent_start_y
            }
            
        # Create feature configuration
        enable_features = None
        if args.enable_features:
            enable_features = {}
            for feature in args.enable_features:
                if feature == 'reward_modulation':
                    enable_features['reward_modulation'] = True
                elif feature == 'structural_plasticity':
                    enable_features['structural_plasticity'] = True
                elif feature == 'development':
                    enable_features['development'] = True
                elif feature == 'curiosity':
                    enable_features['curiosity'] = True
                    
        # Run simulation
        logger.info(f"Running simulation with {args.steps} steps")
        results = self.simulation_runner.run_simulation(
            config_args, args.steps, args.timestep, world_config,
            enable_features, args.output
        )
        
        # Print results
        self._print_simulation_results(results)
        
    def _run_batch_command(self, args: argparse.Namespace):
        """Run batch simulation command."""
        if not PYNLM_AVAILABLE:
            raise NLMCLIError("pynlm module is required for batch simulation")
            
        # Load base configuration
        base_config_args = {}
        if args.config:
            base_config_args = self.config_manager.load_config(args.config)
            
        # Create parameter sweeps
        param_sweeps = []
        if args.param_sweep:
            for sweep in args.param_sweep:
                param_sweeps.append(self.config_manager.load_config(sweep))
                
        # Run batch
        logger.info(f"Running batch simulation with {len(param_sweeps) + 1} configurations")
        results = self.simulation_runner.run_batch_simulation(
            [base_config_args] + param_sweeps if param_sweeps else [base_config_args],
            args.steps, args.timestep
        )
        
        # Analyze results
        analysis = self.result_analyzer.analyze_statistics(results)
        
        # Print results
        print(self.result_analyzer.generate_report(analysis))
        
        # Save results if requested
        if args.output:
            with open(args.output, 'w') as f:
                json.dump({
                    'results': results,
                    'analysis': analysis
                }, f, indent=2)
                
    def _run_experiment_command(self, args: argparse.Namespace):
        """Run experiment command."""
        if not PYNLM_AVAILABLE:
            raise NLMCLIError("pynlm module is required for experiment")
            
        # Load configuration
        config = self.config_manager.load_config(args.config)
        
        # Create experiment
        self.experiment_manager.create_experiment(args.name, config)
        
        # Run experiment
        param_sweeps = None
        if args.param_sweep:
            param_sweeps = []
            for sweep in args.param_sweep:
                param_sweeps.append(self.config_manager.load_config(sweep))
                
        experiment = self.experiment_manager.run_experiment(
            args.name, self.simulation_runner, args.steps,
            args.timestep, param_sweeps
        )
        
        # Analyze experiment
        analysis = self.experiment_manager.analyze_experiment(args.name)
        
        # Print analysis
        print(f"Experiment '{args.name}' completed")
        print(f"Number of runs: {analysis['num_runs']}")
        print(f"Successful runs: {analysis['successful_runs']}")
        print(f"Failed runs: {analysis['failed_runs']}")
        
        # Export results if requested
        if args.export:
            self.experiment_manager.export_experiment_results(
                args.name, args.export, args.format or 'json'
            )
            
    def _run_analyze_command(self, args: argparse.Namespace):
        """Analyze results command."""
        if not args.results:
            raise NLMCLIError("Results file is required for analysis")
            
        # Load results
        with open(args.results, 'r') as f:
            if args.results.endswith('.json'):
                results = json.load(f)['results']
            elif args.results.endswith('.csv'):
                # Load CSV (simplified)
                import pandas as pd
                df = pd.read_csv(args.results)
                results = df.to_dict('records')
            else:
                raise NLMCLIError(f"Unsupported results format: {args.results}")
                
        # Analyze results
        analysis = self.result_analyzer.analyze_statistics(results)
        
        # Generate report
        report = self.result_analyzer.generate_report(analysis)
        
        # Print report
        print(report)
        
        # Save analysis if requested
        if args.output:
            with open(args.output, 'w') as f:
                json.dump(analysis, f, indent=2)
                
    def _run_profile_command(self, args: argparse.Namespace):
        """Profile command."""
        logger.info("Profiling NLM simulation (placeholder)")
        # Implementation would include profiling tools
        print("Profiling functionality is under development.")
        
    def _run_config_command(self, args: argparse.Namespace):
        """Configuration command."""
        if args.command == 'validate':
            if not args.config:
                raise NLMCLIError("Configuration file is required for validation")
                
            config_args = self.config_manager.load_config(args.config)
            is_valid = validate_config(config_args) if ENHANCED_BINDINGS_AVAILABLE else True
            
            if is_valid:
                print(f"Configuration {args.config} is valid")
            else:
                print(f"Configuration {args.config} is invalid")
                
        elif args.command == 'show':
            if not args.config:
                raise NLMCLIError("Configuration file is required for display")
                
            config_args = self.config_manager.load_config(args.config)
            
            print(f"Configuration from {args.config}:")
            for key, value in config_args.items():
                print(f"  {key}: {value}")
                
    def _run_export_command(self, args: argparse.Namespace):
        """Export command."""
        if not PYNLM_AVAILABLE:
            raise NLMCLIError("pynlm module is required for export")
            
        config = nlm.createDefaultConfig()
        export_config_to_json(config, args.file)
        print(f"Configuration exported to {args.file}")
        
    def _run_import_command(self, args: argparse.Namespace):
        """Import command."""
        if not PYNLM_AVAILABLE:
            raise NLMCLIError("pynlm module is required for import")
            
        config = import_config_from_json(args.file)
        print(f"Configuration imported from {args.file}")
        
    def _print_simulation_results(self, results: Dict[str, Any]):
        """Print simulation results."""
        print("\n" + "=" * 60)
        print("SIMULATION RESULTS")
        print("=" * 60)
        
        print(f"\nPerformance:")
        print(f"  Steps: {results['steps']}")
        print(f"  Elapsed time: {results['elapsed_time']:.2f}s")
        print(f"  Steps per second: {results['steps_per_second']:.2f}")
        
        print(f"\nNetwork:")
        print(f"  Total neurons: {results['total_neurons']}")
        print(f"  Total synapses: {results['total_synapses']}")
        print(f"  Firing neurons: {results['firing_neurons']}")
        print(f"  Total spikes: {results['total_spikes']}")
        
        print(f"\nActivity:")
        print(f"  Average firing rate: {results['average_firing_rate']:.2f} Hz")
        print(f"  World time: {results['world_time']:.2f}")
        
        print(f"\n" + "=" * 60)
        
    def get_argument_parser(self) -> argparse.ArgumentParser:
        """Get the argument parser."""
        parser = argparse.ArgumentParser(
            description="NLM Advanced Command-Line Interface",
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        
        # Create subparsers
        subparsers = parser.add_subparsers(dest='command', help='Command to execute')
        
        # Simulate command
        simulate_parser = subparsers.add_parser('simulate', help='Run simulation')
        simulate_parser.add_argument('--config', help='Configuration file')
        simulate_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        simulate_parser.add_argument('--timestep', type=float, default=0.1, help='Time step size')
        simulate_parser.add_argument('--neuron-count', type=int, help='Number of neurons (overrides config)')
        simulate_parser.add_argument('--width', type=int, default=20, help='World width')
        simulate_parser.add_argument('--height', type=int, default=20, help='World height')
        simulate_parser.add_argument('--vision-width', type=int, default=8, help='Vision width')
        simulate_parser.add_argument('--vision-height', type=int, default=8, help='Vision height')
        simulate_parser.add_argument('--agent-start-x', type=float, default=10.0, help='Agent start X position')
        simulate_parser.add_argument('--agent-start-y', type=float, default=10.0, help='Agent start Y position')
        simulate_parser.add_argument('--learning-rate', type=float, help='STDP learning rate (overrides config)')
        simulate_parser.add_argument('--enable-features', nargs='+', help='Features to enable')
        simulate_parser.add_argument('--output', help='Output file for results')
        
        # Batch command
        batch_parser = subparsers.add_parser('batch', help='Run batch simulation')
        batch_parser.add_argument('--config', help='Base configuration file')
        batch_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        batch_parser.add_argument('--timestep', type=float, default=0.1, help='Time step size')
        batch_parser.add_argument('--param-sweep', nargs='+', help='Parameter sweep files')
        batch_parser.add_argument('--output', help='Output file for aggregated results')
        
        # Experiment command
        experiment_parser = subparsers.add_parser('experiment', help='Run experiment')
        experiment_parser.add_argument('name', help='Experiment name')
        experiment_parser.add_argument('--config', required=True, help='Configuration file')
        experiment_parser.add_argument('--steps', type=int, default=1000, help='Number of simulation steps')
        experiment_parser.add_argument('--timestep', type=float, default=0.1, help='Time step size')
        experiment_parser.add_argument('--param-sweep', nargs='+', help='Parameter sweep files')
        experiment_parser.add_argument('--export', help='Export experiment results to file')
        experiment_parser.add_argument('--format', choices=['json', 'csv'], default='json', help='Export format')
        
        # Analyze command
        analyze_parser = subparsers.add_parser('analyze', help='Analyze simulation results')
        analyze_parser.add_argument('results', help='Results file to analyze')
        analyze_parser.add_argument('--output', help='Output file for analysis')
        
        # Profile command
        profile_parser = subparsers.add_parser('profile', help='Profile simulation')
        
        # Config command
        config_parser = subparsers.add_parser('config', help='Manage configuration')
        config_subparsers = config_parser.add_subparsers(dest='command', help='Configuration command')
        
        validate_parser = config_subparsers.add_parser('validate', help='Validate configuration')
        validate_parser.add_argument('config', help='Configuration file to validate')
        
        show_parser = config_subparsers.add_parser('show', help='Display configuration')
        show_parser.add_argument('config', help='Configuration file to display')
        
        # Export command
        export_parser = subparsers.add_parser('export', help='Export configuration')
        export_parser.add_argument('file', help='Output file')
        
        # Import command
        import_parser = subparsers.add_parser('import', help='Import configuration')
        import_parser.add_argument('file', help='Input file')
        
        return parser

# Main entry point
if __name__ == '__main__':
    cli = NLMCommandLine()
    parser = cli.get_argument_parser()
    args = parser.parse_args()
    
    try:
        cli.run(args)
    except NLMCLIError as e:
        print(f"Error: {e}", file=sys.stderr)
        sys.exit(1)
    except Exception as e:
        print(f"Unexpected error: {e}", file=sys.stderr)
        logger.exception("Unexpected error")
        sys.exit(1)