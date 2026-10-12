#!/usr/bin/env python3
"""
NLM Experiment Runner Script
Runs complex experimental workflows for NLM simulations.

This script provides comprehensive support for running experimental workflows,
including parameter variations, different simulation types, and automated
analysis of experimental results.
"""

import argparse
import subprocess
import json
import os
import time
import csv
from pathlib import Path
from typing import List, Dict, Any, Optional
from dataclasses import dataclass
from enum import Enum
import itertools

class ExperimentType(Enum):
    PLASTICITY = "plasticity"
    DEVELOPMENT = "development"
    COGNITIVE = "cognitive"
    SOCIAL = "social"
    MEMORY = "memory"
    ATTENTION = "attention"
    ALL = "all"

@dataclass
class ExperimentConfig:
    """Configuration for a single experiment."""
    name: str
    type: str
    neurons: int
    regions: int
    steps: int
    preset: str
    parameters: Dict[str, Any]
    dependencies: List[str] = None

@dataclass
class ExperimentResult:
    """Result of a completed experiment."""
    config: ExperimentConfig
    success: bool
    execution_time: float
    metrics: Dict[str, Any]
    output_files: List[str]
    error_message: Optional[str] = None

class NLMExperimentRunner:
    """Runner for NLM experimental workflows."""
    
    def __init__(self, output_dir: str = "experiment_results"):
        self.output_dir = Path(output_dir)
        self.output_dir.mkdir(exist_ok=True)
        self.configs = {}
    
    def load_configurations(self, config_dir: str = "configs"):
        """Load configuration presets from directory."""
        config_dir = Path(config_dir)
        
        if config_dir.exists():
            for config_file in config_dir.glob("*.cfg"):
                try:
                    config_name = config_file.stem
                    with open(config_file, 'r') as f:
                        config = {}
                        for line in f:
                            line = line.strip()
                            if line and not line.startswith('#'):
                                if '=' in line:
                                    key, value = line.split('=', 1)
                                    config[key.strip()] = value.strip()
                    self.configs[config_name] = config
                    print(f"Loaded configuration: {config_name}")
                except Exception as e:
                    print(f"Warning: Could not load config {config_file}: {e}")
    
    def create_experiment_config(self, name: str, exp_type: str, 
                                neurons: int = None, regions: int = None,
                                steps: int = None, preset: str = None,
                                parameters: Dict[str, Any] = None) -> ExperimentConfig:
        """Create an experiment configuration."""
        config = ExperimentConfig(
            name=name,
            type=exp_type,
            neurons=neurons or 1000,
            regions=regions or 2,
            steps=steps or 1000,
            preset=preset or exp_type,
            parameters=parameters or {}
        )
        return config
    
    def get_experiment_types(self, include_all: bool = True) -> List[str]:
        """Get list of available experiment types."""
        types = [e.value for e in ExperimentType if e != ExperimentType.ALL]
        
        if include_all:
            types.append(ExperimentType.ALL.value)
        
        return types
    
    def get_presets(self) -> List[str]:
        """Get list of available configuration presets."""
        presets = ["basic", "plasticity", "development", "cognitive", 
                  "social", "memory", "attention"]
        return presets
    
    def run_experiment(self, config: ExperimentConfig, verbose: bool = False) -> ExperimentResult:
        """Run a single experiment with specified configuration."""
        start_time = time.time()
        output_files = []
        
        # Create output directory for this experiment
        exp_output_dir = self.output_dir / config.name
        exp_output_dir.mkdir(exist_ok=True)
        
        # Build NLM command
        cmd = ["nlm", "experiment"]
        
        # Add experiment type
        cmd.extend(["--type", config.type])
        
        # Add parameters
        cmd.extend(["--steps", str(config.steps)])
        cmd.extend(["--neurons", str(config.neurons)])
        cmd.extend(["--regions", str(config.regions)])
        
        if config.preset:
            cmd.extend(["--preset", config.preset])
        
        # Add additional parameters from config
        if config.parameters:
            for key, value in config.parameters.items():
                if isinstance(value, bool):
                    value = "true" if value else "false"
                cmd.extend([f"--{key}", str(value)])
        
        # Add output options
        cmd.extend(["--output", str(exp_output_dir / "results.json")])
        
        if not verbose:
            cmd.extend(["--verbose", "quiet"])
        
        print(f"Running experiment: {config.name}")
        print(f"Command: {' '.join(cmd)}")
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=600,  # 10 minute timeout
                cwd=str(self.output_dir)
            )
            
            execution_time = time.time() - start_time
            
            # Collect output files
            for file_pattern in ["*.json", "*.csv", "*.txt", "*.log"]:
                for output_file in exp_output_dir.glob(file_pattern):
                    output_files.append(str(output_file))
            
            if result.returncode == 0:
                # Parse results (simplified)
                metrics = self._parse_experiment_output(result.stdout)
                
                success_result = ExperimentResult(
                    config=config,
                    success=True,
                    execution_time=execution_time,
                    metrics=metrics,
                    output_files=output_files
                )
                
                print(f"✓ Experiment completed: {config.name} ({execution_time:.2f}s)")
                return success_result
            else:
                error_msg = result.stderr if result.stderr else f"Exit code: {result.returncode}"
                
                failure_result = ExperimentResult(
                    config=config,
                    success=False,
                    execution_time=execution_time,
                    metrics={},
                    output_files=output_files,
                    error_message=error_msg
                )
                
                print(f"✗ Experiment failed: {config.name} - {error_msg}")
                return failure_result
                
        except subprocess.TimeoutExpired:
            error_msg = "Timeout exceeded"
            execution_time = time.time() - start_time
            
            failure_result = ExperimentResult(
                config=config,
                success=False,
                execution_time=execution_time,
                metrics={},
                output_files=output_files,
                error_message=error_msg
            )
            
            print(f"✗ Experiment timeout: {config.name}")
            return failure_result
        
        except Exception as e:
            error_msg = str(e)
            execution_time = time.time() - start_time
            
            failure_result = ExperimentResult(
                config=config,
                success=False,
                execution_time=execution_time,
                metrics={},
                output_files=output_files,
                error_message=error_msg
            )
            
            print(f"✗ Experiment error: {config.name} - {error_msg}")
            return failure_result
    
    def _parse_experiment_output(self, output: str) -> Dict[str, Any]:
        """Parse experiment output to extract key metrics."""
        metrics = {}
        
        # Extract basic metrics from output
        lines = output.split('\n')
        
        for line in lines:
            if "Spikes generated:" in line:
                try:
                    metrics['spikes'] = float(line.split("Spikes generated:")[1].strip())
                except:
                    pass
            
            elif "Firing neurons:" in line:
                try:
                    metrics['firing_rate'] = float(line.split("Firing neurons:")[1].strip())
                except:
                    pass
            
            elif "Most active neurons recorded:" in line:
                try:
                    metrics['most_active'] = int(line.split("Most active neurons recorded:")[1].strip())
                except:
                    pass
            
            elif "Learning occurred:" in line:
                metrics['learning'] = "yes" in line.lower()
        
        # Default metrics if not found
        if 'spikes' not in metrics:
            metrics['spikes'] = 0.0
        if 'firing_rate' not in metrics:
            metrics['firing_rate'] = 0.0
        if 'most_active' not in metrics:
            metrics['most_active'] = 0
        
        return metrics
    
    def create_experiment_suite(self, experiment_type: str, 
                               neurons_list: List[int] = None,
                               steps_list: List[int] = None,
                               presets: List[str] = None) -> List[ExperimentConfig]:
        """Create a suite of experiments with varying parameters."""
        experiments = []
        
        if experiment_type == ExperimentType.ALL.value:
            types = self.get_experiment_types(include_all=False)
        else:
            types = [experiment_type]
        
        neurons_list = neurons_list or [1000, 1500, 2000]
        steps_list = steps_list or [500, 1000, 2000]
        presets = presets or self.get_presets()
        
        for exp_type in types:
            for neurons in neurons_list:
                for steps in steps_list:
                    for preset in presets:
                        exp_name = f"{exp_type}_{neurons}_{steps}_{preset}"
                        
                        # Skip incompatible combinations
                        if neurons < 500 or steps < 100:
                            continue
                        
                        config = self.create_experiment_config(
                            name=exp_name,
                            type=exp_type,
                            neurons=neurons,
                            regions=2 if exp_type != "memory" else 1,  # Different region counts
                            steps=steps,
                            preset=preset
                        )
                        
                        experiments.append(config)
        
        return experiments
    
    def run_experiment_suite(self, experiments: List[ExperimentConfig], 
                           verbose: bool = False) -> List[ExperimentResult]:
        """Run a suite of experiments and return results."""
        print(f"Starting experiment suite: {len(experiments)} experiments")
        print("-" * 50)
        
        results = []
        total_start = time.time()
        
        for i, config in enumerate(experiments):
            print(f"Experiment {i+1}/{len(experiments)}: {config.name}")
            
            result = self.run_experiment(config, verbose)
            results.append(result)
            
            # Show progress
            progress = (i + 1) / len(experiments) * 100
            print(f"Progress: {progress:.1f}% ({i+1}/{len(experiments)})")
            print(f"  Status: {'✓' if result.success else '✗'}")
            print()
        
        total_time = time.time() - total_start
        
        print("-" * 50)
        print(f"Experiment suite completed: {len(results)} experiments")
        print(f"Total time: {total_time:.2f} seconds")
        
        successful = len([r for r in results if r.success])
        if successful > 0:
            avg_time = total_time / len(results)
            print(f"Average time per experiment: {avg_time:.2f} seconds")
        
        return results
    
    def analyze_suite_results(self, results: List[ExperimentResult]) -> Dict[str, Any]:
        """Analyze experiment suite results."""
        if not results:
            return {}
        
        analysis = {
            'total_experiments': len(results),
            'successful_experiments': len([r for r in results if r.success]),
            'failed_experiments': len([r for r in results if not r.success]),
            'type_distribution': {},
            'preset_distribution': {},
            'neuron_impact': {},
            'step_impact': {},
            'best_performers': []
        }
        
        # Analyze by experiment type
        for result in results:
            exp_type = result.config.type
            analysis['type_distribution'][exp_type] = analysis['type_distribution'].get(exp_type, 0) + 1
        
        # Analyze by preset
        for result in results:
            preset = result.config.preset
            analysis['preset_distribution'][preset] = analysis['preset_distribution'].get(preset, 0) + 1
        
        # Analyze by neuron count
        for result in results:
            neurons = result.config.neurons
            if neurons not in analysis['neuron_impact']:
                analysis['neuron_impact'][neurons] = {'spikes': 0, 'count': 0, 'firing': 0}
            
            if result.success:
                analysis['neuron_impact'][neurons]['spikes'] += result.metrics.get('spikes', 0)
                analysis['neuron_impact'][neurons]['firing'] += result.metrics.get('firing_rate', 0)
                analysis['neuron_impact'][neurons]['count'] += 1
        
        # Calculate averages for neuron impact
        for neurons, data in analysis['neuron_impact'].items():
            if data['count'] > 0:
                data['avg_spikes'] = data['spikes'] / data['count']
                data['avg_firing'] = data['firing'] / data['count']
        
        # Find best performers
        successful_results = [r for r in results if r.success]
        if successful_results:
            # Sort by spikes
            sorted_by_spikes = sorted(successful_results, key=lambda r: r.metrics.get('spikes', 0), reverse=True)
            analysis['best_performers'].extend([
                {
                    'config': r.config.name,
                    'type': r.config.type,
                    'preset': r.config.preset,
                    'neurons': r.config.neurons,
                    'steps': r.config.steps,
                    'spikes': r.metrics.get('spikes', 0),
                    'firing_rate': r.metrics.get('firing_rate', 0),
                    'ranking': 'spikes'
                }
                for r in sorted_by_spikes[:3]
            ])
        
        return analysis
    
    def save_suite_results(self, results: List[ExperimentResult], 
                           analysis: Dict[str, Any], 
                           filename: str = "experiment_suite_results.json"):
        """Save experiment suite results to JSON file."""
        filepath = self.output_dir / filename
        
        # Convert results to serializable format
        serializable_results = []
        for result in results:
            serializable_results.append({
                'name': result.config.name,
                'type': result.config.type,
                'neurons': result.config.neurons,
                'regions': result.config.regions,
                'steps': result.config.steps,
                'preset': result.config.preset,
                'success': result.success,
                'execution_time': result.execution_time,
                'metrics': result.metrics,
                'output_files': result.output_files,
                'error_message': result.error_message
            })
        
        suite_data = {
            'experiments': serializable_results,
            'analysis': analysis
        }
        
        with open(filepath, 'w') as f:
            json.dump(suite_data, f, indent=2)
        
        print(f"Suite results saved to: {filepath}")
    
    def create_experiment_comparison(self, results: List[ExperimentResult], 
                                    output_format: str = "text") -> str:
        """Create a comparison report of experiment results."""
        if not results:
            return "No results to compare"
        
        comparison = []
        comparison.append("EXPERIMENT COMPARISON REPORT")
        comparison.append("=" * 50)
        
        # Summary statistics
        total = len(results)
        successful = len([r for r in results if r.success])
        failed = total - successful
        
        comparison.append(f"\nSummary:")
        comparison.append(f"  Total experiments: {total}")
        comparison.append(f"  Successful: {successful}")
        comparison.append(f"  Failed: {failed}")
        
        # Success rate by type
        if successful > 0:
            comparison.append(f"\nSuccess Rate by Type:")
            type_stats = {}
            for result in results:
                exp_type = result.config.type
                if exp_type not in type_stats:
                    type_stats[exp_type] = {'total': 0, 'success': 0}
                type_stats[exp_type]['total'] += 1
                if result.success:
                    type_stats[exp_type]['success'] += 1
            
            for exp_type, stats in type_stats.items():
                rate = (stats['success'] / stats['total']) * 100
                comparison.append(f"  {exp_type}: {rate:.1f}% ({stats['success']}/{stats['total']})")
        
        # Top performers
        successful_results = [r for r in results if r.success]
        if successful_results:
            comparison.append(f"\nTop Performers (by spikes):")
            sorted_results = sorted(successful_results, 
                                  key=lambda r: r.metrics.get('spikes', 0), 
                                  reverse=True)
            
            for i, result in enumerate(sorted_results[:5]):
                comparison.append(f"  {i+1}. {result.config.name}")
                comparison.append(f"     Type: {result.config.type}, Preset: {result.config.preset}")
                comparison.append(f"     Neurons: {result.config.neurons}, Steps: {result.config.steps}")
                comparison.append(f"     Spikes: {result.metrics.get('spikes', 0):.0f}")
                comparison.append(f"     Firing Rate: {result.metrics.get('firing_rate', 0):.2f}")
                comparison.append(f"     Time: {result.execution_time:.2f}s")
        
        return "\n".join(comparison)


def main():
    parser = argparse.ArgumentParser(
        description="Run NLM experimental workflows",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python experiment_runner.py --type plasticity --neurons 1000 1500 2000 --steps 500 1000
  python experiment_runner.py --type all --preset basic plasticity
  python experiment_runner.py --type cognitive --neurons 2000 --steps 2000 --output-dir cognitive_exp
  python experiment_runner.py --type social --regions 3 --steps 1000 --verbose
        """
    )
    
    parser.add_argument(
        "--type", default="all",
        choices=[e.value for e in ExperimentType],
        help="Experiment type (default: all)"
    )
    
    parser.add_argument(
        "--neurons", nargs="+", type=int, default=[1000, 1500, 2000],
        help="Neuron counts to test (default: 1000 1500 2000)"
    )
    
    parser.add_argument(
        "--steps", nargs="+", type=int, default=[500, 1000, 2000],
        help="Step counts to test (default: 500 1000 2000)"
    )
    
    parser.add_argument(
        "--regions", nargs="+", type=int, default=[2],
        help="Region counts to test (default: 2)"
    )
    
    parser.add_argument(
        "--preset", nargs="+", default=None,
        help="Presets to test (default: all available)"
    )
    
    parser.add_argument(
        "--output-dir", default="experiment_results",
        help="Output directory for experiment results (default: experiment_results)"
    )
    
    parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--save-results", action="store_true", default=True,
        help="Save experiment results (default: True)"
    )
    
    parser.add_argument(
        "--results-file", default="experiment_suite_results.json",
        help="Filename for results JSON file (default: experiment_suite_results.json)"
    )
    
    parser.add_argument(
        "--create-comparison", action="store_true",
        help="Create comparison report"
    )
    
    parser.add_argument(
        "--comparison-file", default="experiment_comparison.txt",
        help="Filename for comparison report (default: experiment_comparison.txt)"
    )
    
    parser.add_argument(
        "--config-dir", default="configs",
        help="Directory containing configuration files (default: configs)"
    )
    
    args = parser.parse_args()
    
    # Create experiment runner
    runner = NLMExperimentRunner(args.output_dir)
    
    # Load configurations
    runner.load_configurations(args.config_dir)
    
    # Create experiment suite
    experiments = runner.create_experiment_suite(
        experiment_type=args.type,
        neurons_list=args.neurons,
        steps_list=args.steps,
        presets=args.preset or runner.get_presets()
    )
    
    print(f"Created experiment suite: {len(experiments)} experiments")
    
    # Run experiment suite
    results = runner.run_experiment_suite(experiments, args.verbose)
    
    # Analyze results
    analysis = runner.analyze_suite_results(results)
    
    # Save results and analysis
    if args.save_results:
        runner.save_suite_results(results, analysis, args.results_file)
    
    # Create comparison report
    if args.create_comparison:
        comparison = runner.create_experiment_comparison(results)
        
        comparison_file = Path(args.output_dir) / args.comparison_file
        with open(comparison_file, 'w') as f:
            f.write(comparison)
        
        print(f"\nComparison report saved to: {comparison_file}")
        print("\n" + comparison)
    
    return 0

if __name__ == "__main__":
    exit(main())
