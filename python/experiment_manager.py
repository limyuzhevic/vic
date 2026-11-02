# ExperimentManager - Comprehensive experiment management and analysis

This module provides tools for managing experiments, running parameter sweeps,
analyzing results, and generating reports.
"""

import time
import threading
from typing import Optional, List, Dict, Any, Callable, Union, Tuple
from dataclasses import dataclass, field
import numpy as np
from pathlib import Path

from .enhanced_api import NLMSystem, SimulationStats

@dataclass
class ExperimentConfig:
    """Configuration for running experiments"""
    name: str = "experiment"
    description: str = ""
    brain_config: Any = None
    simulation_steps: int = 1000
    parameter_sets: List[Dict[str, Any]] = field(default_factory=list)
    parallel_execution: bool = False
    max_workers: int = 4
    output_directory: str = "./experiments"
    save_intermediate_results: bool = False
    monitor_callback: Optional[Callable] = None
    progress_callback: Optional[Callable] = None

@dataclass
class ExperimentResult:
    """Results from a single experiment run"""
    experiment_id: str
    config: Dict[str, Any]
    brain: Any
    system: NLMSystem
    simulation_stats: List[SimulationStats]
    total_reward: float = 0.0
    learning_curve: List[Dict[str, Any]] = field(default_factory=list)
    behavior_trace: List[Dict[str, Any]] = field(default_factory=list)
    performance_metrics: Dict[str, Any] = field(default_factory=dict)
    start_time: float = 0.0
    end_time: float = 0.0

class ExperimentManager:
    """
    Comprehensive experiment management system for NLM simulations.
    
    This class provides tools for:
    - Managing experiment configurations
    - Running single experiments and parameter sweeps
    - Analyzing results and generating reports
    - Saving and loading experiment data
    - Parallel execution of experiments
    
    Examples:
        >>> # Single experiment
        >>> manager = nlm.ExperimentManager()
        >>> result = manager.run_experiment("test", config, steps=1000)
        >>> 
        >>> # Parameter sweep
        >>> manager = nlm.ExperimentManager()
        >>> manager.add_parameter_set({"brain.neuron_count": 100})
        >>> manager.add_parameter_set({"brain.neuron_count": 200})
        >>> results = manager.run_parameter_sweep("neuron_count_test")
        >>> 
        >>> # Analysis
        >>> manager = nlm.ExperimentManager()
        >>> analysis = manager.analyze_results(results, metric="firing_rate")
        >>> manager.generate_report(analysis, "experiment_report.html")
    """
    
    def __init__(self, config: Optional[ExperimentConfig] = None):
        """
        Initialize the experiment manager.
        
        Args:
            config: Optional experiment configuration
        """
        self.config = config or ExperimentConfig()
        self.results: List[ExperimentResult] = []
        self._lock = threading.Lock()
        self._active_experiments: Dict[str, threading.Thread] = {}
        
        # Create output directory
        Path(self.config.output_directory).mkdir(parents=True, exist_ok=True)
    
    def add_parameter_set(self, parameters: Dict[str, Any]) -> 'ExperimentManager':
        """
        Add a parameter set for parameter sweep experiments.
        
        Args:
            parameters: Dictionary of configuration parameters
            
        Returns:
            Self for method chaining
        """
        self.config.parameter_sets.append(parameters)
        return self
    
    def remove_parameter_set(self, index: int) -> 'ExperimentManager':
        """
        Remove a parameter set by index.
        
        Args:
            index: Index of parameter set to remove
            
        Returns:
            Self for method chaining
        """
        if 0 <= index < len(self.config.parameter_sets):
            self.config.parameter_sets.pop(index)
        return self
    
    def clear_parameter_sets(self) -> 'ExperimentManager':
        """Clear all parameter sets."""
        self.config.parameter_sets.clear()
        return self
    
    def run_experiment(self, experiment_name: Optional[str] = None, 
                      config: Optional[ExperimentConfig] = None) -> ExperimentResult:
        """
        Run a single experiment.
        
        Args:
            experiment_name: Name of the experiment
            config: Optional experiment configuration override
            
        Returns:
            Experiment results
            
        Raises:
            Exception: If experiment fails
        """
        try:
            # Use provided config or current config
            exp_config = config or self.config
            name = experiment_name or exp_config.name
            
            # Create unique experiment ID
            experiment_id = f"{name}_{int(time.time())}_{len(self.results)}"
            
            # Create brain and system
            import pynlm as nlm
            brain_config = exp_config.brain_config or nlm.createDefaultConfig()
            brain = nlm.createBrain(brain_config)
            system = NLMSystem(brain)
            
            # Initialize system
            start_time = time.time()
            system.initialize()
            
            # Run simulation
            simulation_stats = system.run_simulation(exp_config.simulation_steps)
            
            # Collect metrics
            learning_curve = self._extract_learning_curve(simulation_stats)
            behavior_trace = self._collect_behavior_trace(system, simulation_stats)
            performance_metrics = self._calculate_performance_metrics(simulation_stats)
            
            end_time = time.time()
            
            # Create result
            result = ExperimentResult(
                experiment_id=experiment_id,
                config={
                    'name': name,
                    'description': exp_config.description,
                    'simulation_steps': exp_config.simulation_steps,
                    'brain_config': brain_config,
                    'parameter_sets': exp_config.parameter_sets
                },
                brain=brain,
                system=system,
                simulation_stats=simulation_stats,
                learning_curve=learning_curve,
                behavior_trace=behavior_trace,
                performance_metrics=performance_metrics,
                start_time=start_time,
                end_time=end_time
            )
            
            # Save if configured
            if exp_config.save_intermediate_results:
                self._save_experiment_result(result)
            
            # Add to results
            with self._lock:
                self.results.append(result)
            
            # Call progress callback
            if exp_config.progress_callback:
                progress = len(self.results) / max(1, len(exp_config.parameter_sets))
                exp_config.progress_callback(name, progress, result)
            
            return result
            
        except Exception as e:
            raise Exception(f"Experiment '{name}' failed: {e}")
    
    def run_parameter_sweep(self, experiment_name: str, 
                           parameter_name: str, 
                           parameter_values: List[Any]) -> List[ExperimentResult]:
        """
        Run a parameter sweep experiment.
        
        Args:
            experiment_name: Name of the parameter sweep
            parameter_name: Name of parameter to vary
            parameter_values: List of values to test
            
        Returns:
            List of experiment results for each parameter value
        """
        try:
            # Clear existing parameter sets
            self.clear_parameter_sets()
            
            # Add parameter sets
            for value in parameter_values:
                self.add_parameter_set({parameter_name: value})
            
            # Run experiments
            results = []
            for i, param_set in enumerate(self.config.parameter_sets):
                exp_name = f"{experiment_name}_{parameter_name}_{i}"
                
                # Create experiment config with current parameters
                exp_config = ExperimentConfig(
                    name=exp_name,
                    brain_config=self.config.brain_config,
                    simulation_steps=self.config.simulation_steps,
                    save_intermediate_results=self.config.save_intermediate_results,
                    monitor_callback=self.config.monitor_callback,
                    progress_callback=self.config.progress_callback
                )
                
                result = self.run_experiment(exp_name, exp_config)
                results.append(result)
            
            return results
            
        except Exception as e:
            raise Exception(f"Parameter sweep failed: {e}")
    
    def run_parallel_experiments(self, experiment_names: List[str]) -> List[ExperimentResult]:
        """
        Run multiple experiments in parallel.
        
        Args:
            experiment_names: List of experiment names to run
            
        Returns:
            List of experiment results
        """
        try:
            if not self.config.parallel_execution:
                # Run sequentially if parallel execution disabled
                results = []
                for name in experiment_names:
                    result = self.run_experiment(name)
                    results.append(result)
                return results
            
            # Run in parallel
            threads = []
            results = [None] * len(experiment_names)
            
            def run_experiment_with_index(index: int, name: str):
                try:
                    result = self.run_experiment(name)
                    results[index] = result
                except Exception as e:
                    print(f"Experiment '{name}' failed: {e}")
            
            # Start threads
            for i, name in enumerate(experiment_names):
                thread = threading.Thread(
                    target=run_experiment_with_index,
                    args=(i, name)
                )
                threads.append(thread)
                thread.start()
                
                # Limit number of concurrent threads
                if len(self._active_experiments) >= self.config.max_workers:
                    # Wait for any thread to complete
                    for t in threads:
                        if not t.is_alive():
                            threads.remove(t)
                            break
            
            # Wait for all threads to complete
            for thread in threads:
                thread.join()
            
            return [r for r in results if r is not None]
            
        except Exception as e:
            raise Exception(f"Parallel experiment execution failed: {e}")
    
    def analyze_results(self, results: List[ExperimentResult], 
                       metric: str = "firing_rate", 
                       analysis_type: str = "statistical") -> Dict[str, Any]:
        """
        Analyze experiment results.
        
        Args:
            results: List of experiment results to analyze
            metric: Metric to analyze
            analysis_type: Type of analysis ('statistical', 'comparative', 'trend')
            
        Returns:
            Dictionary containing analysis results
        """
        try:
            if not results:
                return {'error': 'No results to analyze'}
            
            # Extract data for the specified metric
            data_points = []
            for result in results:
                if metric == "firing_rate":
                    values = [stats.firing_rate for stats in result.simulation_stats]
                elif metric == "learning_rate":
                    values = [stats.learning_rate for stats in result.simulation_stats]
                elif metric == "total_spikes":
                    values = [stats.total_spikes for stats in result.simulation_stats]
                else:
                    values = []
                
                if values:
                    data_points.append({
                        'experiment_id': result.experiment_id,
                        'parameter_set': result.config.get('parameter_sets', [{}])[0],
                        'values': values,
                        'mean': np.mean(values),
                        'std': np.std(values),
                        'min': np.min(values),
                        'max': np.max(values),
                        'median': np.median(values)
                    })
            
            if analysis_type == "statistical":
                return self._statistical_analysis(data_points)
            elif analysis_type == "comparative":
                return self._comparative_analysis(data_points)
            elif analysis_type == "trend":
                return self._trend_analysis(data_points)
            else:
                return {'error': f'Unknown analysis type: {analysis_type}'}
                
        except Exception as e:
            return {'error': f'Analysis failed: {e}'}
    
    def generate_report(self, analysis: Dict[str, Any], 
                       report_type: str = "text") -> str:
        """
        Generate a report from analysis results.
        
        Args:
            analysis: Analysis results
            report_type: Type of report ('text', 'html', 'json')
            
        Returns:
            Report content as string
        """
        try:
            if report_type == "text":
                return self._generate_text_report(analysis)
            elif report_type == "html":
                return self._generate_html_report(analysis)
            elif report_type == "json":
                return self._generate_json_report(analysis)
            else:
                raise ValueError(f"Unknown report type: {report_type}")
                
        except Exception as e:
            return f"Report generation failed: {e}"
    
    def get_experiment_summary(self) -> Dict[str, Any]:
        """
        Get summary of all completed experiments.
        
        Returns:
            Summary statistics
        """
        try:
            if not self.results:
                return {'message': 'No experiments completed'}
            
            total_steps = sum(len(r.simulation_stats) for r in self.results)
            total_reward = sum(r.total_reward for r in self.results)
            
            return {
                'total_experiments': len(self.results),
                'total_simulation_steps': total_steps,
                'average_steps_per_experiment': total_steps / len(self.results),
                'total_reward': total_reward,
                'average_reward_per_experiment': total_reward / len(self.results),
                'experiment_ids': [r.experiment_id for r in self.results],
                'parameter_variations': self._get_parameter_variations()
            }
            
        except Exception as e:
            return {'error': f'Failed to get experiment summary: {e}'}
    
    def save_results(self, filepath: str) -> bool:
        """
        Save all experiment results to file.
        
        Args:
            filepath: Path to output file
            
        Returns:
            True if save successful, False otherwise
        """
        try:
            import json
            
            results_data = []
            for result in self.results:
                results_data.append({
                    'experiment_id': result.experiment_id,
                    'config': result.config,
                    'performance_metrics': result.performance_metrics,
                    'start_time': result.start_time,
                    'end_time': result.end_time,
                    'duration': result.end_time - result.start_time
                })
            
            with open(filepath, 'w') as f:
                json.dump(results_data, f, indent=2)
            
            return True
            
        except Exception as e:
            print(f"Failed to save results: {e}")
            return False
    
    def load_results(self, filepath: str) -> bool:
        """
        Load experiment results from file.
        
        Args:
            filepath: Path to input file
            
        Returns:
            True if load successful, False otherwise
        """
        try:
            import json
            
            with open(filepath, 'r') as f:
                results_data = json.load(f)
            
            # Note: This would need to be implemented with actual data loading
            # For now, just load metadata
            self.results.clear()
            
            return True
            
        except Exception as e:
            print(f"Failed to load results: {e}")
            return False
    
    def clear_results(self) -> 'ExperimentManager':
        """Clear all experiment results."""
        with self._lock:
            self.results.clear()
        return self
    
    def _extract_learning_curve(self, simulation_stats: List[SimulationStats]) -> List[Dict[str, Any]]:
        """Extract learning curve from simulation statistics."""
        return [
            {
                'step': i,
                'firing_rate': stats.firing_rate,
                'learning_rate': stats.learning_rate,
                'plasticity_effect': stats.plasticity_effect,
                'total_spikes': stats.total_spikes
            }
            for i, stats in enumerate(simulation_stats)
        ]
    
    def _collect_behavior_trace(self, system: NLMSystem, 
                               simulation_stats: List[SimulationStats]) -> List[Dict[str, Any]]:
        """Collect behavior trace from system."""
        # This would need to be implemented based on available system capabilities
        return []
    
    def _calculate_performance_metrics(self, simulation_stats: List[SimulationStats]) -> Dict[str, Any]:
        """Calculate performance metrics from simulation statistics."""
        if not simulation_stats:
            return {}
        
        firing_rates = [stats.firing_rate for stats in simulation_stats]
        learning_rates = [stats.learning_rate for stats in simulation_stats]
        
        return {
            'average_firing_rate': np.mean(firing_rates),
            'std_firing_rate': np.std(firing_rates),
            'peak_firing_rate': np.max(firing_rates),
            'average_learning_rate': np.mean(learning_rates),
            'total_simulation_steps': len(simulation_stats),
            'learning_efficiency': np.mean(learning_rates) / max(1, np.mean(firing_rates))
        }
    
    def _statistical_analysis(self, data_points: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Perform statistical analysis."""
        if not data_points:
            return {}
        
        return {
            'metric': 'statistical',
            'data_points_count': len(data_points),
            'parameter_variations': [dp['parameter_set'] for dp in data_points],
            'summary_statistics': {
                'mean_values': [dp['mean'] for dp in data_points],
                'std_values': [dp['std'] for dp in data_points],
                'range': [dp['max'] - dp['min'] for dp in data_points]
            }
        }
    
    def _comparative_analysis(self, data_points: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Perform comparative analysis."""
        if not data_points:
            return {}
        
        # Compare each data point with the first
        baseline = data_points[0] if data_points else None
        comparisons = []
        
        if baseline:
            for dp in data_points[1:]:
                comparisons.append({
                    'parameter_set': dp['parameter_set'],
                    'vs_baseline_mean': dp['mean'] - baseline['mean'],
                    'vs_baseline_percent': ((dp['mean'] - baseline['mean']) / baseline['mean'] * 100) if baseline['mean'] != 0 else 0,
                    'improvement': 'better' if dp['mean'] > baseline['mean'] else 'worse'
                })
        
        return {
            'metric': 'comparative',
            'baseline': baseline['parameter_set'] if baseline else None,
            'comparisons': comparisons
        }
    
    def _trend_analysis(self, data_points: List[Dict[str, Any]]) -> Dict[str, Any]:
        """Perform trend analysis."""
        if not data_points:
            return {}
        
        # Calculate trend for each parameter variation
        trends = []
        for dp in data_points:
            if len(dp['values']) > 1:
                x = list(range(len(dp['values'])))
                y = dp['values']
                slope = np.polyfit(x, y, 1)[0] if len(x) > 1 else 0
                trends.append({
                    'parameter_set': dp['parameter_set'],
                    'trend_slope': slope,
                    'trend_direction': 'increasing' if slope > 0.01 else 'decreasing' if slope < -0.01 else 'stable',
                    'stability_score': 1.0 / (1.0 + abs(slope))
                })
        
        return {
            'metric': 'trend',
            'trends': trends
        }
    
    def _generate_text_report(self, analysis: Dict[str, Any]) -> str:
        """Generate text report."""
        return f"Analysis Report:\n{analysis}"
    
    def _generate_html_report(self, analysis: Dict[str, Any]) -> str:
        """Generate HTML report."""
        return f"<html><body>Analysis Report:<pre>{analysis}</pre></body></html>"
    
    def _generate_json_report(self, analysis: Dict[str, Any]) -> str:
        """Generate JSON report."""
        import json
        return json.dumps(analysis, indent=2)
    
    def _get_parameter_variations(self) -> List[Dict[str, Any]]:
        """Get parameter variations from results."""
        variations = []
        for result in self.results:
            params = result.config.get('parameter_sets', [])
            if params:
                variations.append(params[0])
        return variations
    
    def _save_experiment_result(self, result: ExperimentResult):
        """Save experiment result to file."""
        try:
            filename = f"{self.config.output_directory}/{result.experiment_id}.json"
            import json
            
            with open(filename, 'w') as f:
                json.dump({
                    'experiment_id': result.experiment_id,
                    'config': result.config,
                    'performance_metrics': result.performance_metrics
                }, f, indent=2)
                
        except Exception as e:
            print(f"Failed to save experiment result: {e}")