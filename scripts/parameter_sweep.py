#!/usr/bin/env python3
"""
NLM Parameter Sweep Script
Runs simulations with varying parameters to find optimal configurations.

This script provides parameter sweep capabilities for NLM simulations,
enabling automated exploration of parameter spaces to find optimal
or interesting configurations.
"""

import argparse
import subprocess
import json
import csv
import os
import time
from pathlib import Path
from typing import List, Dict, Any, Optional
from dataclasses import dataclass
from enum import Enum

class OptimizationGoal(Enum):
    MAX_SPIKES = "max_spikes"
    MAX_FIRING_RATE = "max_firing_rate"
    MIN_STEPS = "min_steps"
    COST_EFFICIENCY = "cost_efficiency"
    BALANCED = "balanced"

@dataclass
class SweepParameter:
    name: str
    values: List[Any]
    step_size: Optional[float] = None

@dataclass
class SimulationResult:
    parameters: Dict[str, Any]
    neurons: int
    steps: int
    spikes: float
    firing_rate: float
    success: bool
    execution_time: float

class NLMParameterSweep:
    """Parameter sweep for NLM simulations."""
    
    def __init__(self, base_config: Dict[str, Any]):
        self.base_config = base_config
        self.results = []
    
    def parse_parameter_string(self, param_str: str) -> Dict[str, Any]:
        """Parse parameter string like 'neurons=500,1000' into dict."""
        params = {}
        for item in param_str.split(','):
            if '=' in item:
                key, value = item.split('=', 1)
                params[key.strip()] = self._parse_param_value(value.strip())
        return params
    
    def _parse_param_value(self, value: str) -> Any:
        """Parse parameter value based on its type."""
        # Try integer
        try:
            return int(value)
        except ValueError:
            pass
        
        # Try float
        try:
            return float(value)
        except ValueError:
            pass
        
        # Try boolean
        if value.lower() in ['true', 'false']:
            return value.lower() == 'true'
        
        # Return as string
        return value
    
    def generate_parameter_combinations(self, parameters: List[Dict[str, Any]]) -> List[Dict[str, Any]]:
        """Generate all combinations of parameters."""
        if not parameters:
            return [self.base_config]
        
        # Flatten parameters
        param_lists = list(parameters)
        
        # Generate combinations
        combinations = [self.base_config.copy()]
        for param_list in param_lists:
            new_combinations = []
            for param_value in param_list['values']:
                for combo in combinations:
                    new_combo = combo.copy()
                    new_combo[param_list['name']] = param_value
                    new_combinations.append(new_combo)
            combinations = new_combinations
        
        return combinations
    
    def run_simulation(self, config: Dict[str, Any], verbose: bool = False) -> Optional[SimulationResult]:
        """Run a single simulation with specified configuration."""
        # Build command
        cmd = ["nlm", "run"]
        
        # Add simulation type
        sim_type = config.get('type', 'plasticity')
        cmd.extend(["--type", sim_type])
        
        # Add other parameters
        if 'steps' in config:
            cmd.extend(["--steps", str(config['steps'])])
        
        if 'neurons' in config:
            cmd.extend(["--neurons", str(config['neurons'])])
        
        if 'regions' in config:
            cmd.extend(["--regions", str(config['regions'])])
        
        # Add verbosity
        cmd.extend(["--verbose", "quiet" if not verbose else "info"])
        
        print(f"Running: {' '.join(cmd)}")
        
        try:
            start_time = time.time()
            
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout
            )
            
            execution_time = time.time() - start_time
            
            if result.returncode == 0:
                # Parse results (simplified - would need actual parsing of NLM output)
                simulation_result = self._parse_simulation_output(result.stdout)
                
                if simulation_result:
                    simulation_result.parameters = config
                    simulation_result.execution_time = execution_time
                    return simulation_result
                else:
                    print(f"Warning: Could not parse simulation output for config: {config}")
                    return None
            else:
                print(f"✗ Failed: {config}")
                if verbose:
                    print(f"Error: {result.stderr}")
                return None
                
        except subprocess.TimeoutExpired:
            print(f"✗ Timeout: {config}")
            return None
        except Exception as e:
            print(f"✗ Error: {e}")
            return None
    
    def _parse_simulation_output(self, output: str) -> Optional[SimulationResult]:
        """Parse NLM simulation output to extract key metrics."""
        # This is a simplified implementation - actual parsing would depend on NLM's output format
        # For now, we'll create a placeholder result
        
        result = SimulationResult(
            parameters={},
            neurons=0,
            steps=0,
            spikes=0.0,
            firing_rate=0.0,
            success=True,
            execution_time=0.0
        )
        
        # Extract some basic information from output
        lines = output.split('\n')
        
        for line in lines:
            if "Spikes generated:" in line:
                try:
                    result.spikes = float(line.split("Spikes generated:")[1].strip())
                except:
                    pass
            
            elif "Firing neurons:" in line:
                try:
                    result.firing_rate = float(line.split("Firing neurons:")[1].strip())
                except:
                    pass
        
        # Set default values
        result.neurons = 1000  # Would come from config
        result.steps = 1000    # Would come from config
        
        return result
    
    def run_sweep(self, parameters: List[Dict[str, Any]], 
                  verbose: bool = False) -> List[SimulationResult]:
        """Run parameter sweep and return results."""
        # Generate parameter combinations
        combinations = self.generate_parameter_combinations(parameters)
        
        print(f"Starting parameter sweep: {len(combinations)} combinations")
        print(f"Base config: {self.base_config}")
        print("-" * 50)
        
        results = []
        total_start = time.time()
        
        for i, config in enumerate(combinations):
            print(f"Running combination {i+1}/{len(combinations)}")
            
            result = self.run_simulation(config, verbose)
            
            if result:
                results.append(result)
                
                # Show progress
                progress = (i + 1) / len(combinations) * 100
                print(f"Progress: {progress:.1f}% ({i+1}/{len(combinations)})")
                print(f"  Result: {result.spikes} spikes, {result.firing_rate:.2f} firing rate")
            
            print()
        
        total_time = time.time() - total_start
        
        print("-" * 50)
        print(f"Parameter sweep completed: {len(results)}/{len(combinations)} successful")
        print(f"Total time: {total_time:.2f} seconds")
        
        if results:
            avg_time = total_time / len(results)
            print(f"Average time per simulation: {avg_time:.2f} seconds")
        
        return results
    
    def analyze_results(self, results: List[SimulationResult], goal: OptimizationGoal) -> Dict[str, Any]:
        """Analyze sweep results and identify optimal configurations."""
        if not results:
            return {}
        
        analysis = {
            'total_simulations': len(results),
            'successful_simulations': len([r for r in results if r.success]),
            'failed_simulations': len([r for r in results if not r.success]),
            'goal': goal.value,
            'best_config': None,
            'best_value': None,
            'average_metrics': {},
            'parameter_impact': {}
        }
        
        # Calculate metrics for successful simulations
        successful_results = [r for r in results if r.success]
        
        if not successful_results:
            return analysis
        
        # Calculate average metrics
        analysis['average_metrics']['spikes'] = sum(r.spikes for r in successful_results) / len(successful_results)
        analysis['average_metrics']['firing_rate'] = sum(r.firing_rate for r in successful_results) / len(successful_results)
        analysis['average_metrics']['execution_time'] = sum(r.execution_time for r in successful_results) / len(successful_results)
        
        # Find best configuration based on goal
        if goal == OptimizationGoal.MAX_SPIKES:
            best_result = max(successful_results, key=lambda r: r.spikes)
            analysis['best_value'] = best_result.spikes
            analysis['best_config'] = best_result.parameters
        
        elif goal == OptimizationGoal.MAX_FIRING_RATE:
            best_result = max(successful_results, key=lambda r: r.firing_rate)
            analysis['best_value'] = best_result.firing_rate
            analysis['best_config'] = best_result.parameters
        
        elif goal == OptimizationGoal.MIN_STEPS:
            best_result = min(successful_results, key=lambda r: r.execution_time)
            analysis['best_value'] = best_result.execution_time
            analysis['best_config'] = best_result.parameters
        
        elif goal == OptimizationGoal.COST_EFFICIENCY:
            # Cost efficiency = spikes / execution time
            best_result = max(successful_results, key=lambda r: r.spikes / r.execution_time if r.execution_time > 0 else 0)
            analysis['best_value'] = best_result.spikes / best_result.execution_time if best_result.execution_time > 0 else 0
            analysis['best_config'] = best_result.parameters
        
        elif goal == OptimizationGoal.BALANCED:
            # Simple balanced score (normalize and average)
            max_spikes = max(r.spikes for r in successful_results)
            max_firing = max(r.firing_rate for r in successful_results)
            
            best_result = max(successful_results, key=lambda r: 
                            (r.spikes / max_spikes if max_spikes > 0 else 0) * 0.5 +
                            (r.firing_rate / max_firing if max_firing > 0 else 0) * 0.5)
            
            analysis['best_value'] = (best_result.spikes / max_spikes if max_spikes > 0 else 0) * 0.5 + \
                                     (best_result.firing_rate / max_firing if max_firing > 0 else 0) * 0.5
            analysis['best_config'] = best_result.parameters
        
        return analysis
    
    def save_results(self, results: List[SimulationResult], filename: str = "sweep_results.csv"):
        """Save sweep results to CSV file."""
        filepath = Path(filename)
        
        with open(filepath, 'w', newline='') as csvfile:
            fieldnames = ['parameters', 'neurons', 'steps', 'spikes', 'firing_rate', 'success', 'execution_time']
            writer = csv.DictWriter(csvfile, fieldnames=fieldnames)
            writer.writeheader()
            
            for result in results:
                row = {
                    'parameters': json.dumps(result.parameters),
                    'neurons': result.neurons,
                    'steps': result.steps,
                    'spikes': result.spikes,
                    'firing_rate': result.firing_rate,
                    'success': result.success,
                    'execution_time': result.execution_time
                }
                writer.writerow(row)
        
        print(f"Results saved to: {filepath}")
    
    def save_analysis(self, analysis: Dict[str, Any], filename: str = "sweep_analysis.json"):
        """Save analysis results to JSON file."""
        filepath = Path(filename)
        
        with open(filepath, 'w') as f:
            json.dump(analysis, f, indent=2)
        
        print(f"Analysis saved to: {filepath}")


def main():
    parser = argparse.ArgumentParser(
        description="Run parameter sweep for NLM simulations",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python parameter_sweep.py --param neurons=500,1000 --param steps=100,500
  python parameter_sweep.py --param neurons=500,1000 --param steps=100,500 --verbose
  python parameter_sweep.py --param neurons=500,1000 --param steps=100,500 --goal cost_efficiency
        """
    )
    
    parser.add_argument(
        "--param", nargs="+", action='append',
        help="Parameter values to test (format: key=value1,value2,...)"
    )
    
    parser.add_argument(
        "--output", default="parameter_sweep.csv",
        help="Output CSV file for sweep results (default: parameter_sweep.csv)"
    )
    
    parser.add_argument(
        "--analysis-file", default="sweep_analysis.json",
        help="Filename for analysis JSON file (default: sweep_analysis.json)"
    )
    
    parser.add_argument(
        "--base-config-file", default="configs/default.cfg",
        help="Base configuration file (default: configs/default.cfg)"
    )
    
    parser.add_argument(
        "--verbose", action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--goal", choices=[e.value for e in OptimizationGoal], default=OptimizationGoal.BALANCED.value,
        help="Optimization goal for parameter selection (default: balanced)"
    )
    
    parser.add_argument(
        "--save-results", action="store_true", default=True,
        help="Save sweep results (default: True)"
    )
    
    parser.add_argument(
        "--save-analysis", action="store_true", default=True,
        help="Save analysis results (default: True)"
    )
    
    args = parser.parse_args()
    
    # Parse base configuration
    base_config = {}
    if os.path.exists(args.base_config_file):
        # Try to parse config file (simplified)
        try:
            with open(args.base_config_file, 'r') as f:
                content = f.read()
                # Very basic parsing - real implementation would need proper config parsing
                for line in content.split('\n'):
                    if '=' in line and not line.startswith('#'):
                        key, value = line.split('=', 1)
                        base_config[key.strip()] = value.strip()
        except Exception as e:
            print(f"Warning: Could not parse config file {args.base_config_file}: {e}")
    
    # Parse parameter values
    parameters = []
    if args.param:
        for param_list in args.param:
            for param_str in param_list:
                param_dict = {k: v for k, v in [item.split('=') for item in param_str.split(',')]}
                if param_dict:
                    name = list(param_dict.keys())[0]
                    values = param_dict[name]
                    
                    # Parse values
                    parsed_values = []
                    for val in values.split(','):
                        parsed_values.append(param_dict[name])
                    
                    parameters.append({
                        'name': name,
                        'values': parsed_values
                    })
    
    # Create parameter sweep
    sweep = NLMParameterSweep(base_config)
    
    # Run parameter sweep
    results = sweep.run_sweep(parameters, args.verbose)
    
    # Analyze results
    goal = OptimizationGoal(args.goal)
    analysis = sweep.analyze_results(results, goal)
    
    # Save results and analysis
    if args.save_results:
        sweep.save_results(results, args.output)
    
    if args.save_analysis:
        sweep.save_analysis(analysis, args.analysis_file)
    
    # Print analysis summary
    if analysis:
        print("\n" + "="*50)
        print("SWEEP ANALYSIS SUMMARY")
        print("="*50)
        print(f"Total simulations: {analysis['total_simulations']}")
        print(f"Successful: {analysis['successful_simulations']}")
        print(f"Failed: {analysis['failed_simulations']}")
        print(f"Optimization goal: {analysis['goal']}")
        
        if analysis['best_config']:
            print(f"\nBest configuration:")
            for key, value in analysis['best_config'].items():
                print(f"  {key}: {value}")
            print(f"  Score: {analysis['best_value']}")
        
        print(f"\nAverage metrics:")
        for key, value in analysis['average_metrics'].items():
            print(f"  {key}: {value:.2f}")
    
    return 0

if __name__ == "__main__":
    exit(main())
