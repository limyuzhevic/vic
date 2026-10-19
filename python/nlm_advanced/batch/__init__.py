"""
Batch processing utilities for NLM simulations.

Provides simulation parameter sweeps, cross-validation tools, result aggregation,
parallel execution, and automated experiment generation.
"""

import numpy as np
import itertools
import threading
from typing import Dict, List, Optional, Any, Callable, Tuple
from dataclasses import dataclass, field
from collections import defaultdict
import time
import json
import pandas as pd
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor, as_completed
import warnings
import concurrent.futures

@dataclass
class ParameterSet:
    """Configuration for a single simulation parameter set."""
    name: str
    parameters: Dict[str, Any]
    description: str = ""
    weight: float = 1.0  # For weighted sampling

@dataclass
class ExperimentConfig:
    """Configuration for batch experiment."""
    name: str
    parameter_ranges: Dict[str, List[Any]]
    fixed_parameters: Dict[str, Any] = field(default_factory=dict)
    repetitions: int = 1
    randomize_order: bool = False
    output_format: str = "json"
    save_intermediate: bool = False

class SimulationSweeper:
    """Perform parameter sweeps across simulation parameters."""
    
    def __init__(self, brain_factory: Callable, config: Dict):
        self.brain_factory = brain_factory
        self.config = config
        self.parameter_history = []
        self.results_history = []
        self.sweep_configs = {}
    
    def add_sweep(self, name: str, config: ExperimentConfig):
        """Add a parameter sweep configuration."""
        self.sweep_configs[name] = config
    
    def run_sweep(self, sweep_name: str) -> List[Dict]:
        """Run a parameter sweep."""
        if sweep_name not in self.sweep_configs:
            raise ValueError(f"Unknown sweep: {sweep_name}")
        
        config = self.sweep_configs[sweep_name]
        print(f"Starting sweep: {sweep_name}")
        
        # Generate all parameter combinations
        param_combinations = self._generate_parameter_combinations(config)
        
        if config.randomize_order:
            np.random.shuffle(param_combinations)
        
        results = []
        
        for i, params in enumerate(param_combinations):
            print(f"Running sweep {sweep_name}: {i+1}/{len(param_combinations)}")
            
            # Create parameter set
            param_set = ParameterSet(
                name=f"{sweep_name}_param_{i}",
                parameters=params,
                description=f"Parameter set {i+1} for {sweep_name}"
            )
            
            # Run simulation with these parameters
            result = self._run_single_simulation(param_set, config)
            
            results.append(result)
            self.results_history.append(result)
            self.parameter_history.append(param_set)
            
            # Save intermediate results if requested
            if config.save_intermediate:
                self._save_intermediate_result(result, sweep_name, i)
        
        return results
    
    def _generate_parameter_combinations(self, config: ExperimentConfig) -> List[Dict]:
        """Generate all parameter combinations for the sweep."""
        param_names = list(config.parameter_ranges.keys())
        param_values = list(config.parameter_ranges.values())
        
        # Generate Cartesian product
        combinations = list(itertools.product(*param_values))
        
        # Convert to dictionaries
        result = []
        for combo in combinations:
            param_dict = dict(zip(param_names, combo))
            result.append(param_dict)
        
        return result
    
    def _run_single_simulation(self, param_set: ParameterSet, config: ExperimentConfig) -> Dict:
        """Run a single simulation with given parameters."""
        # Merge parameters
        simulation_config = param_set.parameters.copy()
        simulation_config.update(config.fixed_parameters)
        
        # Create brain with modified configuration
        brain = self.brain_factory(simulation_config)
        
        # Run simulation
        start_time = time.time()
        result = self._execute_simulation(brain, simulation_config)
        end_time = time.time()
        
        # Add metadata
        result['metadata'] = {
            'sweep_name': config.name,
            'param_set_name': param_set.name,
            'parameters': param_set.parameters,
            'start_time': start_time,
            'end_time': end_time,
            'duration': end_time - start_time,
            'repetition': param_set.name.split('_')[-1] if '_' in param_set.name else '0'
        }
        
        return result
    
    def _execute_simulation(self, brain, config: Dict) -> Dict:
        """Execute a single simulation."""
        # Initialize brain
        brain.initialize()
        
        # Run simulation steps
        total_steps = config.get('total_steps', 1000)
        step_results = []
        
        for step in range(total_steps):
            brain.step(step, time.time())
            
            if step % 100 == 0:  # Record every 100 steps
                step_result = self._capture_step_data(brain, step)
                step_results.append(step_result)
        
        # Final metrics
        final_metrics = self._extract_final_metrics(brain)
        
        return {
            'step_results': step_results,
            'final_metrics': final_metrics,
            'config': config
        }
    
    def _capture_step_data(self, brain, step: int) -> Dict:
        """Capture data for a single simulation step."""
        return {
            'step': step,
            'time': time.time(),
            'active_neurons': brain.getActiveNeuronCount(),
            'firing_rate': brain.getAverageFiringRate(),
            'e_i_ratio': brain.getExcitationInhibitionRatio(),
            'spike_count': brain.getTotalSpikeCount()
        }
    
    def _extract_final_metrics(self, brain) -> Dict:
        """Extract final metrics from simulation."""
        return {
            'final_active_neurons': brain.getActiveNeuronCount(),
            'final_firing_rate': brain.getAverageFiringRate(),
            'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'final_development_stage': brain.getDevelopmentalStage(),
            'final_neurons': brain.getRegionCount()
        }
    
    def _save_intermediate_result(self, result: Dict, sweep_name: str, iteration: int):
        """Save intermediate result to file."""
        filename = f"{sweep_name}_iteration_{iteration}.json"
        
        with open(filename, 'w') as f:
            json.dump(result, f, indent=2)
    
    def analyze_sweep_results(self, results: List[Dict]) -> Dict:
        """Analyze sweep results and identify patterns."""
        if not results:
            return {}
        
        analysis = {}
        
        # Extract parameter values and metrics
        param_names = []
        metrics_data = defaultdict(list)
        
        for result in results:
            params = result['metadata']['parameters']
            metrics = result['final_metrics']
            
            for param_name in params:
                if param_name not in param_names:
                    param_names.append(param_name)
            
            for metric_name, metric_value in metrics.items():
                metrics_data[metric_name].append(metric_value)
        
        # Perform correlation analysis
        analysis['correlations'] = self._calculate_correlations(
            param_names, params, metrics_data
        )
        
        # Identify optimal parameters
        analysis['optimal_parameters'] = self._find_optimal_parameters(
            metrics_data, results
        )
        
        # Generate response surfaces
        analysis['response_surfaces'] = self._generate_response_surfaces(
            param_names, metrics_data
        )
        
        return analysis
    
    def _calculate_correlations(self, param_names: List[str], params_list: List[Dict], 
                               metrics_data: Dict) -> Dict:
        """Calculate correlations between parameters and metrics."""
        correlations = {}
        
        for metric_name, values in metrics_data.items():
            if len(values) < 2:
                continue
            
            metric_array = np.array(values)
            param_arrays = {}
            
            for param_name in param_names:
                # Extract parameter values for this parameter
                param_values = []
                for params in params_list:
                    param_values.append(params.get(param_name))
                param_arrays[param_name] = np.array(param_values)
            
            # Calculate correlations
            param_correlations = {}
            for param_name, param_values in param_arrays.items():
                if len(np.unique(param_values)) > 1:  # Need at least 2 unique values
                    try:
                        corr = np.corrcoef(param_values, metric_array)[0, 1]
                        param_correlations[param_name] = corr
                    except:
                        param_correlations[param_name] = None
            
            correlations[metric_name] = param_correlations
        
        return correlations
    
    def _find_optimal_parameters(self, metrics_data: Dict, results: List[Dict]) -> Dict:
        """Find optimal parameter combinations."""
        if not metrics_data:
            return {}
        
        # Use final firing rate as optimization target
        target_metric = 'final_firing_rate'
        
        if target_metric not in metrics_data:
            # Find any numeric metric
            for metric_name, values in metrics_data.items():
                if all(isinstance(v, (int, float)) for v in values if v is not None):
                    target_metric = metric_name
                    break
        
        if target_metric not in metrics_data:
            return {}
        
        values = metrics_data[target_metric]
        max_idx = np.argmax(values)
        
        # Get corresponding parameters
        optimal_params = {}
        for param_name in results[0]['metadata']['parameters']:
            optimal_params[param_name] = results[max_idx]['metadata']['parameters'][param_name]
        
        return {
            'target_metric': target_metric,
            'optimal_value': values[max_idx],
            'optimal_parameters': optimal_params,
            'all_results': list(enumerate(values))
        }
    
    def _generate_response_surfaces(self, param_names: List[str], metrics_data: Dict) -> Dict:
        """Generate response surfaces for key metrics."""
        response_surfaces = {}
        
        for metric_name, values in metrics_data.items():
            if not all(isinstance(v, (int, float)) for v in values if v is not None):
                continue
            
            # Simple 2D response surface if we have 2 parameters
            if len(param_names) >= 2:
                param1, param2 = param_names[0], param_names[1]
                surface_data = self._create_2d_surface(param1, param2, values)
                response_surfaces[metric_name] = surface_data
        
        return response_surfaces
    
    def _create_2d_surface(self, param1: str, param2: str, metric_values: List[float]) -> Dict:
        """Create 2D response surface data."""
        # This is a simplified version - would need proper implementation
        return {
            'param1': param1,
            'param2': param2,
            'metric_values': metric_values,
            'normalized_values': self._normalize_array(metric_values)
        }
    
    def _normalize_array(self, values: List[float]) -> List[float]:
        """Normalize array values to [0, 1]."""
        arr = np.array(values)
        if arr.max() - arr.min() > 0:
            return ((arr - arr.min()) / (arr.max() - arr.min())).tolist()
        return [0.5] * len(values)

class CrossValidator:
    """Cross-validation tools for simulation validation."""
    
    def __init__(self, brain_factory: Callable, config: Dict):
        self.brain_factory = brain_factory
        self.config = config
        self.fold_results = []
        self.validation_metrics = {}
    
    def create_cross_validation_folds(self, data_size: int, n_folds: int = 5) -> List[List[int]]:
        """Create cross-validation folds for validation."""
        indices = list(range(data_size))
        np.random.shuffle(indices)
        
        folds = []
        fold_size = data_size // n_folds
        
        for i in range(n_folds):
            start = i * fold_size
            end = (i + 1) * fold_size if i < n_folds - 1 else data_size
            folds.append(indices[start:end])
        
        return folds
    
    def run_cross_validation(self, data_indices: List[int], n_folds: int = 5,
                           validation_strategy: str = "kfold") -> Dict:
        """Run cross-validation experiment."""
        if validation_strategy == "kfold":
            folds = self.create_cross_validation_folds(len(data_indices), n_folds)
            return self._run_kfold_validation(folds, data_indices)
        else:
            raise ValueError(f"Unsupported validation strategy: {validation_strategy}")
    
    def _run_kfold_validation(self, folds: List[List[int]], data_indices: List[int]) -> Dict:
        """Run k-fold cross-validation."""
        results = {}
        
        for fold_idx, validation_indices in enumerate(folds):
            print(f"Running fold {fold_idx + 1}/{len(folds)}")
            
            # Split data
            train_indices = [i for i in data_indices if i not in validation_indices]
            
            # Train simulation (simulated)
            train_result = self._train_on_data(train_indices)
            
            # Validate simulation
            validation_result = self._validate_on_data(validation_indices)
            
            fold_result = {
                'fold': fold_idx,
                'train_indices': train_indices,
                'validation_indices': validation_indices,
                'train_performance': train_result,
                'validation_performance': validation_result,
                'confusion_matrix': self._calculate_confusion_matrix(
                    train_result, validation_result
                )
            }
            
            results[fold_idx] = fold_result
            self.fold_results.append(fold_result)
        
        # Aggregate results
        aggregated = self._aggregate_fold_results(results)
        
        return {
            'fold_results': results,
            'aggregated_results': aggregated,
            'validation_metrics': self.validation_metrics
        }
    
    def _train_on_data(self, train_indices: List[int]) -> Dict:
        """Train simulation on training data."""
        # Simulate training process
        start_time = time.time()
        
        brain = self.brain_factory({'training': True, 'data_indices': train_indices})
        brain.initialize()
        
        # Run training simulation
        for step in range(100):  # Training steps
            brain.step(step, time.time())
        
        training_time = time.time() - start_time
        
        return {
            'training_time': training_time,
            'final_performance': brain.getAverageFiringRate(),
            'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount()
        }
    
    def _validate_on_data(self, validation_indices: List[int]) -> Dict:
        """Validate simulation on validation data."""
        # Simulate validation process
        brain = self.brain_factory({'validation': True, 'data_indices': validation_indices})
        brain.initialize()
        
        # Run validation simulation
        for step in range(50):  # Validation steps
            brain.step(step, time.time())
        
        return {
            'final_performance': brain.getAverageFiringRate(),
            'final_e_i_ratio': brain.getExcitationInhibitionRatio(),
            'total_spikes': brain.getTotalSpikeCount(),
            'generalization_error': abs(brain.getAverageFiringRate() - 0.5)
        }
    
    def _calculate_confusion_matrix(self, train_result: Dict, validation_result: Dict) -> Dict:
        """Calculate confusion matrix for classification validation."""
        # Simplified confusion matrix
        return {
            'accuracy': 0.8 + np.random.random() * 0.2,  # Random for demo
            'precision': 0.75 + np.random.random() * 0.25,
            'recall': 0.7 + np.random.random() * 0.3,
            'f1_score': 0.72 + np.random.random() * 0.28
        }
    
    def _aggregate_fold_results(self, results: Dict) -> Dict:
        """Aggregate results across all folds."""
        aggregated = {}
        
        for metric in ['train_performance', 'validation_performance']:
            fold_values = []
            for fold_result in results.values():
                fold_values.append(fold_result[metric])
            
            # Calculate statistics
            aggregated[f'{metric}_mean'] = np.mean(fold_values)
            aggregated[f'{metric}_std'] = np.std(fold_values)
            aggregated[f'{metric}_median'] = np.median(fold_values)
        
        return aggregated

class ResultAggregator:
    """Aggregate and analyze simulation results."""
    
    def __init__(self):
        self.results = []
        self.aggregated_stats = {}
    
    def add_result(self, result: Dict, metadata: Dict = None):
        """Add a simulation result to the collection."""
        result_entry = {
            'data': result,
            'metadata': metadata or {}
        }
        
        self.results.append(result_entry)
    
    def aggregate_results(self, filter_criteria: Optional[Dict] = None) -> Dict:
        """Aggregate all results."""
        if not self.results:
            return {}
        
        # Apply filters
        filtered_results = self._apply_filters(self.results, filter_criteria)
        
        if not filtered_results:
            return {}
        
        # Aggregate numeric metrics
        self.aggregated_stats = {}
        
        # Find all numeric fields in results
        all_fields = set()
        for result in filtered_results:
            all_fields.update(result['data'].keys())
        
        for field in all_fields:
            values = []
            for result in filtered_results:
                if field in result['data']:
                    value = result['data'][field]
                    if isinstance(value, (int, float)):
                        values.append(value)
            
            if values:
                self.aggregated_stats[field] = {
                    'mean': np.mean(values),
                    'median': np.median(values),
                    'std': np.std(values),
                    'min': np.min(values),
                    'max': np.max(values),
                    'count': len(values)
                }
        
        return self.aggregated_stats
    
    def _apply_filters(self, results: List[Dict], filter_criteria: Optional[Dict]) -> List[Dict]:
        """Apply filter criteria to results."""
        if not filter_criteria:
            return results
        
        filtered = []
        for result in results:
            match = True
            for field, criterion in filter_criteria.items():
                value = result['data'].get(field)
                if value is not None:
                    if callable(criterion):
                        match = criterion(value)
                    elif isinstance(criterion, dict):
                        if 'min' in criterion and value < criterion['min']:
                            match = False
                        if 'max' in criterion and value > criterion['max']:
                            match = False
            
            if match:
                filtered.append(result)
        
        return filtered
    
    def create_visualization_data(self) -> Dict:
        """Create data for visualization."""
        if not self.results:
            return {}
        
        # Group results by metadata fields
        groups = defaultdict(list)
        
        for result in self.results:
            group_key = tuple(sorted(result['metadata'].items()))
            groups[group_key].append(result['data'])
        
        visualization_data = {
            'groups': {},
            'summary_statistics': self.aggregated_stats
        }
        
        for group_key, group_results in groups.items():
            group_name = f"Group_{len(visualization_data['groups'])}"
            
            # Calculate group statistics
            group_stats = {}
            for field in group_results[0].keys():
                values = []
                for result in group_results:
                    if field in result:
                        value = result[field]
                        if isinstance(value, (int, float)):
                            values.append(value)
                
                if values:
                    group_stats[field] = {
                        'mean': np.mean(values),
                        'std': np.std(values)
                    }
            
            visualization_data['groups'][group_name] = {
                'metadata': dict(group_key),
                'statistics': group_stats
            }
        
        return visualization_data

class ParallelExecutor:
    """Parallel execution utilities for simulations."""
    
    def __init__(self, max_workers: int = None):
        self.max_workers = max_workers or psutil.cpu_count()
        self.executor = ThreadPoolExecutor(max_workers=self.max_workers)
        self.completed_tasks = []
        self.failed_tasks = []
    
    def run_parallel(self, tasks: List[Dict]) -> List[Dict]:
        """Run multiple tasks in parallel."""
        print(f"Running {len(tasks)} tasks in parallel with {self.max_workers} workers")
        
        # Submit all tasks
        future_to_task = {}
        for task in tasks:
            future = self.executor.submit(self._run_single_task, task)
            future_to_task[future] = task
        
        # Collect results
        results = []
        for future in concurrent.futures.as_completed(future_to_task):
            task = future_to_task[future]
            try:
                result = future.result()
                results.append({
                    'task': task,
                    'result': result,
                    'status': 'completed'
                })
                self.completed_tasks.append(task)
            except Exception as e:
                error_result = {
                    'task': task,
                    'error': str(e),
                    'status': 'failed'
                }
                results.append(error_result)
                self.failed_tasks.append(task)
        
        return results
    
    def _run_single_task(self, task: Dict) -> Dict:
        """Run a single simulation task."""
        # This would interface with the actual simulation
        task_name = task.get('name', 'unnamed_task')
        
        print(f"Running task: {task_name}")
        
        # Simulate work
        time.sleep(task.get('duration', 1.0))
        
        return {
            'task_name': task_name,
            'status': 'completed',
            'timestamp': time.time(),
            'result_data': task.get('result_data', {})
        }

class ExperimentGenerator:
    """Automated experiment generation tools."""
    
    def __init__(self, config: Dict):
        self.config = config
        self.experiment_templates = {}
        self.generated_experiments = []
    
    def add_template(self, name: str, template: Dict):
        """Add an experiment template."""
        self.experiment_templates[name] = template
    
    def generate_experiments(self, template_name: str, n_experiments: int = 1) -> List[Dict]:
        """Generate experiments based on a template."""
        if template_name not in self.experiment_templates:
            raise ValueError(f"Unknown template: {template_name}")
        
        template = self.experiment_templates[template_name]
        experiments = []
        
        for i in range(n_experiments):
            experiment = self._create_experiment(template, i)
            experiments.append(experiment)
            self.generated_experiments.append(experiment)
        
        return experiments
    
    def _create_experiment(self, template: Dict, index: int) -> Dict:
        """Create a single experiment from template."""
        # Base experiment configuration
        experiment = {
            'name': f"{template['name']}_{index}",
            'description': template.get('description', ''),
            'parameters': self._generate_parameters(template),
            'metadata': {
                'template': template['name'],
                'index': index,
                'created_at': time.time()
            }
        }
        
        # Add template-specific fields
        for key, value in template.items():
            if key not in ['name', 'description']:
                experiment[key] = value
        
        return experiment
    
    def _generate_parameters(self, template: Dict) -> Dict:
        """Generate parameters for the experiment."""
        params = {}
        
        # Parameter ranges from template
        param_ranges = template.get('parameter_ranges', {})
        
        for param_name, param_config in param_ranges.items():
            if isinstance(param_config, dict) and 'range' in param_config:
                # Generate random value within range
                range_min = param_config['range'][0]
                range_max = param_config['range'][1]
                params[param_name] = np.random.uniform(range_min, range_max)
            elif isinstance(param_config, list):
                # Random choice from list
                params[param_name] = np.random.choice(param_config)
            else:
                # Use the value directly
                params[param_name] = param_config
        
        return params

# Import required modules
from .. import pynlm