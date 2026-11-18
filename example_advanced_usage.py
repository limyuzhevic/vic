#!/usr/bin/env python3
"""
Example script demonstrating NLM advanced features and improvements.

This script shows how to:
1. Configure and run NLM simulations with advanced options
2. Analyze simulation results
3. Compare different configurations
4. Perform parameter sweeps
"""

import subprocess
import json
import time
import argparse
from pathlib import Path
from typing import Dict, List, Any

class NLMSimulator:
    """NLM simulation manager with advanced features."""
    
    def __init__(self, config_file: str = None):
        self.config_file = config_file
        self.results_dir = Path("nlm_results")
        self.results_dir.mkdir(exist_ok=True)
        
    def run_simulation(self, params: Dict[str, Any]) -> Dict[str, Any]:
        """Run a single NLM simulation with given parameters."""
        print(f"Running NLM simulation with parameters: {params}")
        
        # Build command
        cmd = ["nlm_advanced"]
        
        if self.config_file:
            cmd.extend(["--config", self.config_file])
        
        for key, value in params.items():
            cmd.extend([f"--{key}", str(value)])
        
        # Add output file
        timestamp = int(time.time())
        output_file = self.results_dir / f"simulation_{timestamp}.json"
        cmd.extend(["--json-output", str(output_file)])
        
        # Run simulation
        start_time = time.time()
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=300  # 5 minute timeout
            )
            end_time = time.time()
            
            if result.returncode == 0:
                # Parse results
                analysis = self._analyze_results(str(output_file), timestamp)
                analysis['runtime'] = end_time - start_time
                analysis['params'] = params
                return analysis
            else:
                print(f"Simulation failed: {result.stderr}")
                return {'success': False, 'error': result.stderr}
                
        except subprocess.TimeoutExpired:
            return {'success': False, 'error': 'Simulation timeout'}
        except Exception as e:
            return {'success': False, 'error': str(e)}
    
    def _analyze_results(self, output_file: str, timestamp: int) -> Dict[str, Any]:
        """Analyze simulation results and extract key metrics."""
        try:
            with open(output_file, 'r') as f:
                data = json.load(f)
            
            # Extract key metrics
            analysis = {
                'timestamp': timestamp,
                'output_file': output_file,
                'metrics': self._extract_metrics(data),
                'quality_score': self._calculate_quality_score(data)
            }
            
            return analysis
            
        except Exception as e:
            print(f"Error analyzing results: {e}")
            return {'timestamp': timestamp, 'error': str(e)}
    
    def _extract_metrics(self, data: Dict[str, Any]) -> Dict[str, Any]:
        """Extract key performance metrics from simulation results."""
        metrics = {}
        
        # Basic metrics
        if 'spike_count' in data:
            metrics['total_spikes'] = data['spike_count']
        
        if 'firing_rate' in data:
            metrics['average_firing_rate'] = data['firing_rate']
        
        # Calculate derived metrics
        if 'total_spikes' in metrics and 'steps' in data:
            metrics['spikes_per_step'] = metrics['total_spikes'] / data['steps']
        
        # Quality metrics
        if 'weight_changes' in data:
            metrics['weight_change_mean'] = sum(data['weight_changes']) / len(data['weight_changes'])
            metrics['weight_change_std'] = (sum((x - metrics['weight_change_mean'])**2 for x in data['weight_changes']) / len(data['weight_changes']))**0.5
        
        return metrics
    
    def _calculate_quality_score(self, data: Dict[str, Any]) -> float:
        """Calculate a quality score for the simulation."""
        score = 0.0
        max_score = 10.0
        
        # Spike activity (0-3 points)
        if 'spike_count' in data:
            spike_count = data['spike_count']
            if spike_count > 0:
                score += 3.0
        
        # Stability (0-2 points)
        if 'weight_changes' in data:
            changes = data['weight_changes']
            if len(changes) > 0:
                variance = sum((x - sum(changes)/len(changes))**2 for x in changes) / len(changes)
                if variance < 0.1:
                    score += 2.0
        
        # Learning effectiveness (0-3 points)
        if 'learning_rate' in data:
            if data['learning_rate'] > 0.01:
                score += 3.0
        
        # Resource efficiency (0-2 points)
        if 'runtime' in data and 'spike_count' in data:
            efficiency = data['spike_count'] / data['runtime']
            if efficiency > 100:
                score += 2.0
        
        return min(score, max_score)

class ComparisonTool:
    """Compare results from multiple simulations."""
    
    def __init__(self):
        self.results = []
    
    def add_result(self, result: Dict[str, Any]):
        """Add a simulation result to the comparison."""
        self.results.append(result)
    
    def compare_configurations(self) -> Dict[str, Any]:
        """Compare results from different configurations."""
        if not self.results:
            return {'error': 'No results to compare'}
        
        comparison = {
            'total_simulations': len(self.results),
            'successful_simulations': len([r for r in self.results if r.get('success', True)]),
            'configuration_comparison': self._compare_configurations(),
            'performance_comparison': self._compare_performance(),
            'recommendations': self._generate_recommendations()
        }
        
        return comparison
    
    def _compare_configurations(self) -> Dict[str, Any]:
        """Compare different parameter configurations."""
        configs = {}
        
        for result in self.results:
            if result.get('success', True):
                params = result['params']
                config_id = f"{params.get('neurons', 0)} neurons, {params.get('steps', 0)} steps"
                
                if config_id not in configs:
                    configs[config_id] = []
                
                configs[config_id].append(result)
        
        return configs
    
    def _compare_performance(self) -> Dict[str, Any]:
        """Compare performance metrics."""
        performance = {
            'spike_rate_comparison': [],
            'learning_effectiveness_comparison': [],
            'stability_comparison': []
        }
        
        for config_id, results in self._compare_configurations().items():
            spike_rates = [r['metrics'].get('spikes_per_step', 0) for r in results if 'metrics' in r]
            learning_rates = [r['quality_score'] for r in results if 'quality_score' in r]
            
            performance['spike_rate_comparison'].append({
                'config': config_id,
                'mean': sum(spike_rates) / len(spike_rates) if spike_rates else 0,
                'std': (sum((x - sum(spike_rates)/len(spike_rates))**2 for x in spike_rates) / len(spike_rates))**0.5 if spike_rates else 0
            })
            
            performance['learning_effectiveness_comparison'].append({
                'config': config_id,
                'mean': sum(learning_rates) / len(learning_rates) if learning_rates else 0,
                'std': (sum((x - sum(learning_rates)/len(learning_rates))**2 for x in learning_rates) / len(learning_rates))**0.5 if learning_rates else 0
            })
        
        return performance
    
    def _generate_recommendations(self) -> List[str]:
        """Generate recommendations based on comparison."""
        recommendations = []
        
        if not self.results:
            return recommendations
        
        # Find best performing configuration
        best_result = max([r for r in self.results if 'quality_score' in r], 
                         key=lambda x: x['quality_score'], default=None)
        
        if best_result:
            recommendations.append(f"Best configuration: {best_result['params']}")
            recommendations.append(f"Quality score: {best_result['quality_score']:.2f}")
        
        return recommendations

class ParameterSweep:
    """Perform parameter sweeps for research."""
    
    def __init__(self, sweep_params: Dict[str, List[Any]]):
        self.sweep_params = sweep_params
        self.results = []
    
    def run_sweep(self, base_params: Dict[str, Any]) -> List[Dict[str, Any]]:
        """Run parameter sweep with given base parameters."""
        import itertools
        
        # Generate all combinations
        param_combinations = list(itertools.product(*self.sweep_params.values()))
        param_names = list(self.sweep_params.keys())
        
        print(f"Running parameter sweep with {len(param_combinations)} combinations...")
        
        for i, combination in enumerate(param_combinations):
            params = base_params.copy()
            
            for name, value in zip(param_names, combination):
                params[name] = value
            
            # Run simulation
            simulator = NLMSimulator()
            result = simulator.run_simulation(params)
            
            if result.get('success', True):
                result['combination_id'] = i
                self.results.append(result)
                print(f"Completed combination {i+1}/{len(param_combinations)}")
        
        return self.results
    
    def analyze_sweep(self) -> Dict[str, Any]:
        """Analyze parameter sweep results."""
        if not self.results:
            return {'error': 'No results to analyze'}
        
        # Analyze parameter effects
        analysis = {
            'total_combinations': len(self.results),
            'successful_combinations': len([r for r in self.results if r.get('success', True)]),
            'parameter_effects': self._analyze_parameter_effects(),
            'optimal_parameters': self._find_optimal_parameters(),
            'response_surfaces': self._generate_response_surfaces()
        }
        
        return analysis
    
    def _analyze_parameter_effects(self) -> Dict[str, Any]:
        """Analyze how parameters affect results."""
        effects = {}
        
        for param_name in self.sweep_params.keys():
            param_values = {}
            
            for result in self.results:
                if param_name in result['params'] and 'quality_score' in result:
                    value = result['params'][param_name]
                    score = result['quality_score']
                    
                    if value not in param_values:
                        param_values[value] = []
                    
                    param_values[value].append(score)
            
            effects[param_name] = {}
            
            for value, scores in param_values.items():
                effects[param_name][str(value)] = {
                    'mean_score': sum(scores) / len(scores),
                    'std_score': (sum((x - sum(scores)/len(scores))**2 for x in scores) / len(scores))**0.5
                }
        
        return effects
    
    def _find_optimal_parameters(self) -> Dict[str, Any]:
        """Find optimal parameter combinations."""
        if not self.results:
            return {}
        
        # Find best combination for each parameter
        optimal = {}
        
        for param_name in self.sweep_params.keys():
            param_performance = {}
            
            for result in self.results:
                if param_name in result['params'] and 'quality_score' in result:
                    value = result['params'][param_name]
                    score = result['quality_score']
                    
                    if value not in param_performance:
                        param_performance[value] = []
                    
                    param_performance[value].append(score)
            
            # Find best value for each parameter
            best_value = None
            best_mean = -1
            
            for value, scores in param_performance.items():
                mean_score = sum(scores) / len(scores)
                if mean_score > best_mean:
                    best_mean = mean_score
                    best_value = value
            
            optimal[param_name] = {
                'optimal_value': best_value,
                'mean_score': best_mean,
                'num_trials': len(param_performance.get(best_value, []))
            }
        
        return optimal
    
    def _generate_response_surfaces(self) -> Dict[str, Any]:
        """Generate response surfaces for parameter analysis."""
        surfaces = {}
        
        # Simple 2D response surfaces for first two parameters
        if len(self.sweep_params) >= 2:
            param_names = list(self.sweep_params.keys())[:2]
            
            for param1 in self.sweep_params[param_names[0]]:
                for param2 in self.sweep_params[param_names[1]]:
                    key = f"{param_names[0]}_{param_names[1]}"
                    
                    scores = []
                    for result in self.results:
                        if (result['params'].get(param_names[0]) == param1 and 
                            result['params'].get(param_names[1]) == param2 and
                            'quality_score' in result):
                            scores.append(result['quality_score'])
                    
                    surfaces[key] = {
                        'param1_value': param1,
                        'param2_value': param2,
                        'mean_score': sum(scores) / len(scores) if scores else 0,
                        'num_trials': len(scores)
                    }
        
        return surfaces

def main():
    """Main function to demonstrate NLM advanced features."""
    parser = argparse.ArgumentParser(
        description="NLM Advanced Features Demonstration",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        '--mode',
        choices=['basic', 'comparison', 'sweep'],
        default='basic',
        help='Operation mode'
    )
    
    parser.add_argument(
        '--config',
        help='Configuration file path'
    )
    
    args = parser.parse_args()
    
    print("NLM Advanced Features Demonstration")
    print("=" * 50)
    
    if args.mode == 'basic':
        # Basic demonstration
        print("Running basic NLM simulation...")
        
        simulator = NLMSimulator(args.config)
        
        params = {
            'neurons': 2000,
            'steps': 1000,
            'param_stdp_ltp_weight': 0.02,
            'param_hebbian_learning_rate': 0.05
        }
        
        result = simulator.run_simulation(params)
        
        if result.get('success', True):
            print(f"Simulation completed successfully!")
            print(f"Quality score: {result['quality_score']:.2f}")
            print(f"Output saved to: {result['output_file']}")
        else:
            print(f"Simulation failed: {result['error']}")
    
    elif args.mode == 'comparison':
        # Comparison demonstration
        print("Running comparison between configurations...")
        
        tool = ComparisonTool()
        
        # Add different configurations
        configs = [
            {'neurons': 1000, 'steps': 1000, 'param_stdp_ltp_weight': 0.01},
            {'neurons': 2000, 'steps': 1000, 'param_stdp_ltp_weight': 0.02},
            {'neurons': 3000, 'steps': 1000, 'param_stdp_ltp_weight': 0.03},
        ]
        
        for config in configs:
            simulator = NLMSimulator(args.config)
            result = simulator.run_simulation(config)
            tool.add_result(result)
            print(f"Added configuration: {config}")
        
        comparison = tool.compare_configurations()
        
        print("\nComparison Results:")
        print(f"Total simulations: {comparison['total_simulations']}")
        print(f"Successful simulations: {comparison['successful_simulations']}")
        
        if 'recommendations' in comparison:
            print("\nRecommendations:")
            for rec in comparison['recommendations']:
                print(f"  - {rec}")
    
    elif args.mode == 'sweep':
        # Parameter sweep demonstration
        print("Running parameter sweep...")
        
        sweep_params = {
            'param_stdp_ltp_weight': [0.01, 0.02, 0.03],
            'param_hebbian_learning_rate': [0.01, 0.02, 0.03],
        }
        
        base_params = {
            'neurons': 1000,
            'steps': 500,
            'simulation_type': 'detailed'
        }
        
        sweep = ParameterSweep(sweep_params)
        results = sweep.run_sweep(base_params)
        
        analysis = sweep.analyze_sweep()
        
        print(f"\nParameter Sweep Results:")
        print(f"Total combinations: {analysis['total_combinations']}")
        print(f"Successful combinations: {analysis['successful_combinations']}")
        
        if 'optimal_parameters' in analysis:
            print("\nOptimal Parameters:")
            for param, info in analysis['optimal_parameters'].items():
                print(f"  {param}: {info['optimal_value']} (score: {info['mean_score']:.2f})")
        
        print("\nParameter Effects:")
        for param, effects in analysis['parameter_effects'].items():
            print(f"\n{param}:")
            for value, metrics in effects.items():
                print(f"  {value}: score={metrics['mean_score']:.2f}, std={metrics['std_score']:.2f}")
    
    print("\n" + "=" * 50)
    print("NLM Advanced Features Demonstration Complete!")

if __name__ == '__main__':
    main()