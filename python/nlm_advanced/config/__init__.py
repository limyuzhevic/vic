"""
Advanced configuration tools for NLM simulations.

Provides configuration templates, parameter search and optimization,
automated hyperparameter tuning, configuration validation and auditing.
"""

import json
import numpy as np
import itertools
import time
from typing import Dict, List, Optional, Any, Callable, Union
from dataclasses import dataclass, field
import yaml
from pathlib import Path
import copy
import warnings

@dataclass
class ConfigTemplate:
    """Configuration template for simulations."""
    name: str
    description: str
    parameters: Dict[str, Any]
    inheritance: List[str] = field(default_factory=list)
    tags: List[str] = field(default_factory=list)
    author: str = ""
    version: str = "1.0"
    created_at: float = field(default_factory=time.time)

@dataclass
class ParameterSearchResult:
    """Result from parameter search."""
    parameter_set: Dict[str, Any]
    metric: float
    fitness_score: float
    metadata: Dict[str, Any] = field(default_factory=dict)

@dataclass
class OptimizationConfig:
    """Configuration for optimization."""
    optimization_method: str = "bayesian"
    n_iterations: int = 100
    budget: float = 1000.0
    tolerance: float = 1e-6
    random_seed: int = 42
    early_stopping: bool = True
    early_stopping_patience: int = 20

class ConfigTemplate:
    """Configuration templates for simulations."""
    
    def __init__(self, template_dir: str = "./templates"):
        self.template_dir = Path(template_dir)
        self.template_dir.mkdir(parents=True, exist_ok=True)
        
        self.templates = {}
        self.default_parameters = {}
    
    def load_template(self, filepath: str, name: str = None) -> ConfigTemplate:
        """Load a configuration template from file."""
        path = Path(filepath)
        
        if path.suffix == '.json':
            with open(path, 'r') as f:
                template_data = json.load(f)
        elif path.suffix in ['.yaml', '.yml']:
            with open(path, 'r') as f:
                template_data = yaml.safe_load(f)
        else:
            raise ValueError(f"Unsupported template format: {path.suffix}")
        
        # Create template object
        template = ConfigTemplate(
            name=name or path.stem,
            description=template_data.get('description', ''),
            parameters=template_data.get('parameters', {}),
            inheritance=template_data.get('inheritance', []),
            tags=template_data.get('tags', []),
            author=template_data.get('author', ''),
            version=template_data.get('version', '1.0')
        )
        
        self.templates[template.name] = template
        
        # Save default parameters if not already set
        if template.name not in self.default_parameters:
            self.default_parameters[template.name] = template.parameters
        
        print(f"Template loaded: {template.name}")
        return template
    
    def create_template(self, name: str, parameters: Dict[str, Any], 
                       description: str = "", tags: List[str] = None) -> ConfigTemplate:
        """Create a new configuration template."""
        template = ConfigTemplate(
            name=name,
            description=description,
            parameters=parameters,
            tags=tags or []
        )
        
        self.templates[name] = template
        self.default_parameters[name] = parameters
        
        print(f"Template created: {name}")
        return template
    
    def save_template(self, name: str, filepath: str = None):
        """Save template to file."""
        if name not in self.templates:
            raise ValueError(f"Template not found: {name}")
        
        template = self.templates[name]
        
        if filepath is None:
            filepath = self.template_dir / f"{name}.json"
        
        template_data = {
            'name': template.name,
            'description': template.description,
            'parameters': template.parameters,
            'inheritance': template.inheritance,
            'tags': template.tags,
            'author': template.author,
            'version': template.version,
            'created_at': template.created_at
        }
        
        with open(filepath, 'w') as f:
            json.dump(template_data, f, indent=2, default=str)
        
        print(f"Template saved: {filepath}")
    
    def get_template(self, name: str) -> ConfigTemplate:
        """Get template by name."""
        return self.templates.get(name)
    
    def list_templates(self) -> List[str]:
        """List all templates."""
        return list(self.templates.keys())
    
    def apply_template(self, name: str, overrides: Dict[str, Any] = None) -> Dict:
        """Apply template with optional overrides."""
        template = self.get_template(name)
        if not template:
            raise ValueError(f"Template not found: {name}")
        
        # Start with template parameters
        config = template.parameters.copy()
        
        # Apply inheritance
        for inherited_name in template.inheritance:
            if inherited_name in self.templates:
                inherited_template = self.templates[inherited_name]
                for key, value in inherited_template.parameters.items():
                    if key not in config:  # Only inherit if not already set
                        config[key] = value
        
        # Apply overrides
        if overrides:
            config.update(overrides)
        
        return config
    
    def validate_template(self, name: str, config: Dict) -> List[str]:
        """Validate configuration against template."""
        template = self.get_template(name)
        if not template:
            return [f"Template not found: {name}"]
        
        errors = []
        
        # Check required parameters
        for key, value in template.parameters.items():
            if isinstance(value, dict) and 'required' in value and value['required']:
                if key not in config:
                    errors.append(f"Missing required parameter: {key}")
        
        # Check parameter types
        for key, value in config.items():
            if key in template.parameters:
                template_value = template.parameters[key]
                if isinstance(template_value, dict) and 'type' in template_value:
                    expected_type = template_value['type']
                    if expected_type == 'int' and not isinstance(value, int):
                        errors.append(f"Parameter {key} must be int, got {type(value).__name__}")
                    elif expected_type == 'float' and not isinstance(value, (int, float)):
                        errors.append(f"Parameter {key} must be numeric, got {type(value).__name__}")
                    elif expected_type == 'bool' and not isinstance(value, bool):
                        errors.append(f"Parameter {key} must be bool, got {type(value).__name__}")
                    elif expected_type == 'string' and not isinstance(value, str):
                        errors.append(f"Parameter {key} must be string, got {type(value).__name__}")
                    elif expected_type == 'list' and not isinstance(value, list):
                        errors.append(f"Parameter {key} must be list, got {type(value).__name__}")
        
        return errors

class ParameterSearcher:
    """Parameter search and optimization tools."""
    
    def __init__(self, brain_factory: Callable, config: Dict):
        self.brain_factory = brain_factory
        self.config = config
        self.search_history = []
        self.optimization_results = {}
        self.objective_function = None
    
    def set_objective_function(self, func: Callable[[Dict], float]):
        """Set objective function for optimization."""
        self.objective_function = func
    
    def manual_search(self, parameter_grid: Dict[str, List[Any]], 
                     brain_factory: Callable = None) -> List[ParameterSearchResult]:
        """Perform manual grid search over parameters."""
        if brain_factory is None:
            brain_factory = self.brain_factory
        
        # Generate all parameter combinations
        param_names = list(parameter_grid.keys())
        param_values = list(parameter_grid.values())
        
        combinations = list(itertools.product(*param_values))
        
        results = []
        
        for i, combination in enumerate(combinations):
            print(f"Testing combination {i + 1}/{len(combinations)}")
            
            # Create parameter dictionary
            params = dict(zip(param_names, combination))
            
            # Run simulation
            brain = brain_factory(params)
            brain.initialize()
            
            # Evaluate objective function
            if self.objective_function:
                metric = self.objective_function(params)
            else:
                # Default metric: average firing rate
                metric = brain.getAverageFiringRate()
            
            # Calculate fitness score (higher is better)
            fitness_score = self._calculate_fitness_score(metric)
            
            # Create result
            result = ParameterSearchResult(
                parameter_set=params,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'combination_index': i}
            )
            
            results.append(result)
            self.search_history.append(result)
        
        return results
    
    def random_search(self, parameter_ranges: Dict[str, tuple], 
                     n_trials: int = 100, brain_factory: Callable = None) -> List[ParameterSearchResult]:
        """Perform random search over parameter space."""
        if brain_factory is None:
            brain_factory = self.brain_factory
        
        results = []
        
        for i in range(n_trials):
            print(f"Random trial {i + 1}/{n_trials}")
            
            # Generate random parameters
            params = {}
            for param_name, (min_val, max_val) in parameter_ranges.items():
                if isinstance(min_val, int) and isinstance(max_val, int):
                    params[param_name] = np.random.randint(min_val, max_val + 1)
                elif isinstance(min_val, float) and isinstance(max_val, float):
                    params[param_name] = np.random.uniform(min_val, max_val)
                else:
                    params[param_name] = np.random.random()
            
            # Run simulation
            brain = brain_factory(params)
            brain.initialize()
            
            # Evaluate objective function
            if self.objective_function:
                metric = self.objective_function(params)
            else:
                metric = brain.getAverageFiringRate()
            
            # Calculate fitness score
            fitness_score = self._calculate_fitness_score(metric)
            
            # Create result
            result = ParameterSearchResult(
                parameter_set=params,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'trial_index': i}
            )
            
            results.append(result)
            self.search_history.append(result)
        
        return results
    
    def bayesian_optimization(self, parameter_bounds: Dict[str, tuple], 
                              n_iterations: int = 50, brain_factory: Callable = None) -> List[ParameterSearchResult]:
        """Perform Bayesian optimization."""
        if brain_factory is None:
            brain_factory = self.brain_factory
        
        # This is a simplified version - in practice would use proper Bayesian optimization libraries
        results = []
        
        # Initialize with random points
        for i in range(min(10, n_iterations)):
            params = {}
            for param_name, (min_val, max_val) in parameter_bounds.items():
                params[param_name] = np.random.uniform(min_val, max_val)
            
            # Run simulation and evaluate
            brain = brain_factory(params)
            brain.initialize()
            
            if self.objective_function:
                metric = self.objective_function(params)
            else:
                metric = brain.getAverageFiringRate()
            
            fitness_score = self._calculate_fitness_score(metric)
            
            result = ParameterSearchResult(
                parameter_set=params,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'iteration': i, 'method': 'bayesian'}
            )
            
            results.append(result)
            self.search_history.append(result)
        
        # Simulated Bayesian optimization iterations
        for iteration in range(10, n_iterations):
            # Simple acquisition function: pick point with highest expected improvement
            # This is a placeholder for proper Bayesian optimization
            params = {}
            for param_name, (min_val, max_val) in parameter_bounds.items():
                params[param_name] = np.random.uniform(min_val, max_val)
            
            brain = brain_factory(params)
            brain.initialize()
            
            if self.objective_function:
                metric = self.objective_function(params)
            else:
                metric = brain.getAverageFiringRate()
            
            fitness_score = self._calculate_fitness_score(metric)
            
            result = ParameterSearchResult(
                parameter_set=params,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'iteration': iteration, 'method': 'bayesian'}
            )
            
            results.append(result)
            self.search_history.append(result)
        
        return results
    
    def genetic_algorithm(self, parameter_ranges: Dict[str, tuple], 
                         population_size: int = 20, generations: int = 50,
                         brain_factory: Callable = None) -> List[ParameterSearchResult]:
        """Perform genetic algorithm optimization."""
        if brain_factory is None:
            brain_factory = self.brain_factory
        
        # Initialize population
        population = []
        for i in range(population_size):
            params = {}
            for param_name, (min_val, max_val) in parameter_ranges.items():
                if isinstance(min_val, int) and isinstance(max_val, int):
                    params[param_name] = np.random.randint(min_val, max_val + 1)
                else:
                    params[param_name] = np.random.uniform(min_val, max_val)
            
            # Evaluate
            brain = brain_factory(params)
            brain.initialize()
            
            if self.objective_function:
                metric = self.objective_function(params)
            else:
                metric = brain.getAverageFiringRate()
            
            fitness_score = self._calculate_fitness_score(metric)
            
            result = ParameterSearchResult(
                parameter_set=params,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'generation': 0, 'method': 'genetic'}
            )
            
            population.append(result)
        
        # Evolution loop
        for generation in range(1, generations):
            # Selection
            population.sort(key=lambda x: x.fitness_score, reverse=True)
            selected = population[:population_size // 2]
            
            # Crossover and mutation
            offspring = []
            while len(offspring) < population_size - len(selected):
                parent1 = np.random.choice(selected)
                parent2 = np.random.choice(selected)
                
                child = self._crossover_and_mutate(parent1, parent2, parameter_ranges)
                
                # Evaluate child
                brain = brain_factory(child.parameter_set)
                brain.initialize()
                
                if self.objective_function:
                    metric = self.objective_function(child.parameter_set)
                else:
                    metric = brain.getAverageFiringRate()
                
                fitness_score = self._calculate_fitness_score(metric)
                
                child_result = ParameterSearchResult(
                    parameter_set=child.parameter_set,
                    metric=metric,
                    fitness_score=fitness_score,
                    metadata={'generation': generation, 'method': 'genetic'}
                )
                
                offspring.append(child_result)
                self.search_history.append(child_result)
            
            population = selected + offspring
            self.optimization_results[f"generation_{generation}"] = population
        
        # Return best result from each generation
        best_per_generation = [max(gen, key=lambda x: x.fitness_score) for gen in self.optimization_results.values()]
        
        return best_per_generation
    
    def _crossover_and_mutate(self, parent1: ParameterSearchResult, parent2: ParameterSearchResult,
                            parameter_ranges: Dict[str, tuple]) -> Dict:
        """Crossover and mutate parameters."""
        child_params = {}
        
        for param_name in parameter_ranges.keys():
            if np.random.random() < 0.5:  # Crossover
                # Simple blend crossover
                val1 = parent1.parameter_set[param_name]
                val2 = parent2.parameter_set[param_name]
                if isinstance(val1, (int, float)) and isinstance(val2, (int, float)):
                    child_params[param_name] = (val1 + val2) / 2.0
                else:
                    child_params[param_name] = val1
            else:  # Mutation
                min_val, max_val = parameter_ranges[param_name]
                if isinstance(min_val, int) and isinstance(max_val, int):
                    child_params[param_name] = np.random.randint(min_val, max_val + 1)
                else:
                    child_params[param_name] = np.random.uniform(min_val, max_val)
        
        return child_params
    
    def _calculate_fitness_score(self, metric: float) -> float:
        """Calculate fitness score from metric."""
        # Higher fitness for higher metric values (for maximization problems)
        # Could be modified for minimization problems
        return 1.0 / (1.0 + np.exp(-metric))  # Sigmoid transformation
    
    def analyze_search_results(self, results: List[ParameterSearchResult]) -> Dict:
        """Analyze parameter search results."""
        if not results:
            return {}
        
        analysis = {}
        
        # Extract metrics and parameters
        metrics = [result.metric for result in results]
        fitness_scores = [result.fitness_score for result in results]
        
        # Calculate statistics
        analysis['summary'] = {
            'best_metric': max(metrics),
            'worst_metric': min(metrics),
            'best_fitness': max(fitness_scores),
            'average_metric': np.mean(metrics),
            'average_fitness': np.mean(fitness_scores),
            'metric_std': np.std(metrics),
            'fitness_std': np.std(fitness_scores),
            'total_evaluations': len(results)
        }
        
        # Find optimal parameter combinations
        best_result = max(results, key=lambda x: x.fitness_score)
        analysis['optimal_configuration'] = {
            'parameters': best_result.parameter_set,
            'metric': best_result.metric,
            'fitness': best_result.fitness_score
        }
        
        # Analyze parameter importance
        analysis['parameter_importance'] = self._analyze_parameter_importance(results)
        
        # Generate recommendations
        analysis['recommendations'] = self._generate_recommendations(results)
        
        return analysis
    
    def _analyze_parameter_importance(self, results: List[ParameterSearchResult]) -> Dict:
        """Analyze importance of each parameter."""
        if not results:
            return {}
        
        parameter_names = list(results[0].parameter_set.keys())
        importance = {}
        
        for param_name in parameter_names:
            # Extract parameter values
            param_values = [result.parameter_set[param_name] for result in results]
            
            # Calculate correlation with metric
            try:
                correlation = np.corrcoef(param_values, [result.metric for result in results])[0, 1]
                importance[param_name] = {
                    'correlation': correlation,
                    'mean_value': np.mean(param_values),
                    'std_value': np.std(param_values)
                }
            except:
                importance[param_name] = {
                    'correlation': None,
                    'mean_value': np.mean(param_values),
                    'std_value': np.std(param_values)
                }
        
        return importance
    
    def _generate_recommendations(self, results: List[ParameterSearchResult]) -> List[str]:
        """Generate recommendations based on search results."""
        recommendations = []
        
        if not results:
            return recommendations
        
        # Check for clear patterns
        best_result = max(results, key=lambda x: x.fitness_score)
        
        # Parameter-based recommendations
        for param_name, value in best_result.parameter_set.items():
            if isinstance(value, float) and value > 0.5:
                recommendations.append(f"Increase {param_name} for better performance")
            elif isinstance(value, float) and value < 0.5:
                recommendations.append(f"Decrease {param_name} for better performance")
        
        # Performance-based recommendations
        average_metric = np.mean([result.metric for result in results])
        if best_result.metric > average_metric * 1.5:
            recommendations.append("The optimal configuration shows significantly better performance than average.")
        elif best_result.metric < average_metric * 0.5:
            recommendations.append("The search may have found an outlier; consider broader exploration.")
        
        return recommendations
    
    def export_search_results(self, filepath: str, results: List[ParameterSearchResult] = None):
        """Export search results to file."""
        if results is None:
            results = self.search_history
        
        # Convert results to serializable format
        export_data = {
            'timestamp': time.time(),
            'results': []
        }
        
        for result in results:
            export_data['results'].append({
                'parameter_set': result.parameter_set,
                'metric': result.metric,
                'fitness_score': result.fitness_score,
                'metadata': result.metadata
            })
        
        with open(filepath, 'w') as f:
            json.dump(export_data, f, indent=2, default=str)
        
        print(f"Search results exported: {filepath}")

class HyperparameterTuner:
    """Automated hyperparameter tuning."""
    
    def __init__(self, brain_factory: Callable, config: Dict):
        self.brain_factory = brain_factory
        self.config = config
        self.tuning_history = []
        self.best_configurations = []
    
    def tune_hyperparameters(self, base_config: Dict, hyperparameter_space: Dict,
                            optimization_method: str = "bayesian",
                            brain_factory: Callable = None) -> Dict:
        """Tune hyperparameters for a configuration."""
        if brain_factory is None:
            brain_factory = self.brain_factory
        
        print(f"Starting hyperparameter tuning with {optimization_method} method")
        
        if optimization_method == "bayesian":
            results = self._bayesian_tuning(base_config, hyperparameter_space, brain_factory)
        elif optimization_method == "random":
            results = self._random_tuning(base_config, hyperparameter_space, brain_factory)
        elif optimization_method == "grid":
            results = self._grid_tuning(base_config, hyperparameter_space, brain_factory)
        elif optimization_method == "genetic":
            results = self._genetic_tuning(base_config, hyperparameter_space, brain_factory)
        else:
            raise ValueError(f"Unsupported optimization method: {optimization_method}")
        
        # Find best configurations
        best_config = max(results, key=lambda x: x.fitness_score)
        
        self.tuning_history.append({
            'timestamp': time.time(),
            'method': optimization_method,
            'results': results,
            'best_configuration': best_config
        })
        
        self.best_configurations.append(best_config)
        
        return {
            'best_configuration': best_config.parameter_set,
            'best_metric': best_config.metric,
            'best_fitness': best_config.fitness_score,
            'all_results': results
        }
    
    def _bayesian_tuning(self, base_config: Dict, hyperparameter_space: Dict,
                         brain_factory: Callable) -> List[ParameterSearchResult]:
        """Bayesian hyperparameter tuning."""
        # Simplified Bayesian tuning
        results = []
        
        for i in range(50):  # Number of iterations
            print(f"Bayesian iteration {i + 1}/50")
            
            # Generate hyperparameters (simplified)
            hyperparameters = {}
            for param_name, (min_val, max_val) in hyperparameter_space.items():
                hyperparameters[param_name] = np.random.uniform(min_val, max_val)
            
            # Create full configuration
            config = {**base_config, **hyperparameters}
            
            # Run simulation
            brain = brain_factory(config)
            brain.initialize()
            
            # Evaluate performance
            metric = brain.getAverageFiringRate()
            fitness_score = 1.0 / (1.0 + np.exp(-metric))  # Sigmoid
            
            result = ParameterSearchResult(
                parameter_set=config,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'method': 'bayesian', 'iteration': i}
            )
            
            results.append(result)
        
        return results
    
    def _random_tuning(self, base_config: Dict, hyperparameter_space: Dict,
                      brain_factory: Callable) -> List[ParameterSearchResult]:
        """Random hyperparameter tuning."""
        results = []
        
        for i in range(100):  # Number of trials
            print(f"Random trial {i + 1}/100")
            
            # Generate random hyperparameters
            hyperparameters = {}
            for param_name, (min_val, max_val) in hyperparameter_space.items():
                if isinstance(min_val, int) and isinstance(max_val, int):
                    hyperparameters[param_name] = np.random.randint(min_val, max_val + 1)
                else:
                    hyperparameters[param_name] = np.random.uniform(min_val, max_val)
            
            # Create full configuration
            config = {**base_config, **hyperparameters}
            
            # Run simulation
            brain = brain_factory(config)
            brain.initialize()
            
            # Evaluate performance
            metric = brain.getAverageFiringRate()
            fitness_score = 1.0 / (1.0 + np.exp(-metric))
            
            result = ParameterSearchResult(
                parameter_set=config,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'method': 'random', 'trial': i}
            )
            
            results.append(result)
        
        return results
    
    def _grid_tuning(self, base_config: Dict, hyperparameter_space: Dict,
                     brain_factory: Callable) -> List[ParameterSearchResult]:
        """Grid hyperparameter tuning."""
        # Generate parameter grid
        param_names = list(hyperparameter_space.keys())
        param_values = list(hyperparameter_space.values())
        
        combinations = list(itertools.product(*param_values))
        
        results = []
        
        for i, combination in enumerate(combinations):
            print(f"Grid combination {i + 1}/{len(combinations)}")
            
            # Create hyperparameters
            hyperparameters = dict(zip(param_names, combination))
            
            # Create full configuration
            config = {**base_config, **hyperparameters}
            
            # Run simulation
            brain = brain_factory(config)
            brain.initialize()
            
            # Evaluate performance
            metric = brain.getAverageFiringRate()
            fitness_score = 1.0 / (1.0 + np.exp(-metric))
            
            result = ParameterSearchResult(
                parameter_set=config,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'method': 'grid', 'combination': i}
            )
            
            results.append(result)
        
        return results
    
    def _genetic_tuning(self, base_config: Dict, hyperparameter_space: Dict,
                        brain_factory: Callable) -> List[ParameterSearchResult]:
        """Genetic algorithm hyperparameter tuning."""
        # Simplified genetic algorithm
        population_size = 20
        generations = 10
        
        # Initialize population
        population = []
        for i in range(population_size):
            hyperparameters = {}
            for param_name, (min_val, max_val) in hyperparameter_space.items():
                hyperparameters[param_name] = np.random.uniform(min_val, max_val)
            
            config = {**base_config, **hyperparameters}
            
            brain = brain_factory(config)
            brain.initialize()
            
            metric = brain.getAverageFiringRate()
            fitness_score = 1.0 / (1.0 + np.exp(-metric))
            
            result = ParameterSearchResult(
                parameter_set=config,
                metric=metric,
                fitness_score=fitness_score,
                metadata={'method': 'genetic', 'generation': 0}
            )
            
            population.append(result)
        
        # Evolution loop
        for generation in range(1, generations):
            # Selection
            population.sort(key=lambda x: x.fitness_score, reverse=True)
            selected = population[:population_size // 2]
            
            # Crossover and mutation
            offspring = []
            while len(offspring) < population_size - len(selected):
                parent1 = np.random.choice(selected)
                parent2 = np.random.choice(selected)
                
                child_hyperparameters = {}
                for param_name in hyperparameter_space.keys():
                    if np.random.random() < 0.5:  # Crossover
                        val1 = parent1.parameter_set[param_name]
                        val2 = parent2.parameter_set[param_name]
                        child_hyperparameters[param_name] = (val1 + val2) / 2.0
                    else:  # Mutation
                        min_val, max_val = hyperparameter_space[param_name]
                        child_hyperparameters[param_name] = np.random.uniform(min_val, max_val)
                
                config = {**base_config, **child_hyperparameters}
                
                brain = brain_factory(config)
                brain.initialize()
                
                metric = brain.getAverageFiringRate()
                fitness_score = 1.0 / (1.0 + np.exp(-metric))
                
                child_result = ParameterSearchResult(
                    parameter_set=config,
                    metric=metric,
                    fitness_score=fitness_score,
                    metadata={'method': 'genetic', 'generation': generation}
                )
                
                offspring.append(child_result)
            
            population = selected + offspring
        
        return population

class ConfigValidator:
    """Configuration validation and auditing."""
    
    def __init__(self):
        self.validation_rules = []
        self.validation_history = []
    
    def add_validation_rule(self, rule: Dict):
        """Add a validation rule."""
        self.validation_rules.append(rule)
    
    def validate_config(self, config: Dict, rules: List[Dict] = None) -> Dict:
        """Validate configuration against rules."""
        validation_rules = rules or self.validation_rules
        
        validation_result = {
            'timestamp': time.time(),
            'valid': True,
            'errors': [],
            'warnings': [],
            'info': []
        }
        
        for rule in validation_rules:
            rule_errors, rule_warnings, rule_info = self._apply_rule(config, rule)
            
            validation_result['errors'].extend(rule_errors)
            validation_result['warnings'].extend(rule_warnings)
            validation_result['info'].extend(rule_info)
        
        validation_result['valid'] = len(validation_result['errors']) == 0
        
        # Record validation
        self.validation_history.append(validation_result)
        
        return validation_result
    
    def _apply_rule(self, config: Dict, rule: Dict) -> tuple:
        """Apply a single validation rule."""
        errors = []
        warnings = []
        info = []
        
        rule_type = rule.get('type', 'custom')
        
        if rule_type == 'range':
            param_name = rule['parameter']
            min_val = rule.get('min')
            max_val = rule.get('max')
            
            if param_name in config:
                value = config[param_name]
                if min_val is not None and value < min_val:
                    errors.append(f"Parameter {param_name} ({value}) is below minimum {min_val}")
                if max_val is not None and value > max_val:
                    warnings.append(f"Parameter {param_name} ({value}) is above maximum {max_val}")
                
                info.append(f"Parameter {param_name}: {value} (valid range: {min_val}-{max_val})")
        
        elif rule_type == 'dependency':
            param_name = rule['parameter']
            dependency_param = rule['depends_on']
            dependency_value = rule['required_value']
            
            if param_name in config and dependency_param in config:
                if config[param_name] and config[dependency_param] != dependency_value:
                    warnings.append(
                        f"Parameter {param_name} is enabled but depends on {dependency_param}={dependency_value}, "
                        f"currently {config[dependency_param]}"
                    )
        
        elif rule_type == 'type':
            param_name = rule['parameter']
            expected_type = rule['type']
            
            if param_name in config:
                value = config[param_name]
                if expected_type == 'int' and not isinstance(value, int):
                    errors.append(f"Parameter {param_name} must be int, got {type(value).__name__}")
                elif expected_type == 'float' and not isinstance(value, (int, float)):
                    errors.append(f"Parameter {param_name} must be numeric, got {type(value).__name__}")
                elif expected_type == 'string' and not isinstance(value, str):
                    errors.append(f"Parameter {param_name} must be string, got {type(value).__name__}")
                elif expected_type == 'bool' and not isinstance(value, bool):
                    errors.append(f"Parameter {param_name} must be bool, got {type(value).__name__}")
                elif expected_type == 'list' and not isinstance(value, list):
                    errors.append(f"Parameter {param_name} must be list, got {type(value).__name__}")
                
                info.append(f"Parameter {param_name}: {value} ({expected_type})")
        
        elif rule_type == 'custom':
            # Custom validation function
            validator = rule.get('validator')
            if validator and callable(validator):
                try:
                    result = validator(config)
                    if isinstance(result, dict):
                        errors.extend(result.get('errors', []))
                        warnings.extend(result.get('warnings', []))
                        info.extend(result.get('info', []))
                    else:
                        info.append(f"Custom validation result: {result}")
                except Exception as e:
                    errors.append(f"Custom validation failed: {e}")
        
        return errors, warnings, info
    
    def get_validation_report(self) -> Dict:
        """Get validation report."""
        if not self.validation_history:
            return {'message': 'No validation history available'}
        
        # Calculate statistics
        total_validations = len(self.validation_history)
        valid_count = sum(1 for v in self.validation_history if v['valid'])
        
        return {
            'total_validations': total_validations,
            'valid_count': valid_count,
            'success_rate': valid_count / total_validations if total_validations > 0 else 0.0,
            'last_validation': self.validation_history[-1] if self.validation_history else None,
            'error_count': sum(len(v['errors']) for v in self.validation_history),
            'warning_count': sum(len(v['warnings']) for v in self.validation_history)
        }
    
    def export_validation_report(self, filepath: str):
        """Export validation report to file."""
        report = self.get_validation_report()
        
        with open(filepath, 'w') as f:
            json.dump(report, f, indent=2, default=str)

class ConfigAuditor:
    """Configuration auditing and compliance checking."""
    
    def __init__(self):
        self.audit_rules = []
        self.audit_history = []
    
    def add_audit_rule(self, rule: Dict):
        """Add an audit rule."""
        self.audit_rules.append(rule)
    
    def audit_config(self, config: Dict, rules: List[Dict] = None) -> Dict:
        """Audit configuration against rules."""
        audit_rules = rules or self.audit_rules
        
        audit_result = {
            'timestamp': time.time(),
            'config_name': config.get('name', 'unnamed'),
            'compliance_score': 1.0,
            'violations': [],
            'recommendations': [],
            'audit_details': {}
        }
        
        for rule in audit_rules:
            violation = self._check_audit_rule(config, rule)
            if violation:
                audit_result['violations'].append(violation)
                audit_result['compliance_score'] *= (1.0 - violation.get('severity', 0.0))
            
            recommendation = self._generate_audit_recommendation(config, rule)
            if recommendation:
                audit_result['recommendations'].append(recommendation)
        
        audit_result['audit_details'] = {
            'rule_count': len(audit_rules),
            'violation_count': len(audit_result['violations']),
            'recommendation_count': len(audit_result['recommendations']),
            'compliance_level': self._get_compliance_level(audit_result['compliance_score'])
        }
        
        # Record audit
        self.audit_history.append(audit_result)
        
        return audit_result
    
    def _check_audit_rule(self, config: Dict, rule: Dict) -> Optional[Dict]:
        """Check a single audit rule."""
        rule_type = rule.get('type', 'custom')
        
        if rule_type == 'required_field':
            field_name = rule['field']
            if field_name not in config or not config[field_name]:
                return {
                    'rule_type': rule_type,
                    'field': field_name,
                    'severity': rule.get('severity', 0.5),
                    'message': f"Required field '{field_name}' is missing or empty"
                }
        
        elif rule_type == 'value_range':
            field_name = rule['field']
            min_val = rule.get('min_value')
            max_val = rule.get('max_value')
            
            if field_name in config:
                value = config[field_name]
                if min_val is not None and value < min_val:
                    return {
                        'rule_type': rule_type,
                        'field': field_name,
                        'value': value,
                        'min_value': min_val,
                        'severity': rule.get('severity', 0.5),
                        'message': f"Field '{field_name}' value {value} is below minimum {min_val}"
                    }
                if max_val is not None and value > max_val:
                    return {
                        'rule_type': rule_type,
                        'field': field_name,
                        'value': value,
                        'max_value': max_val,
                        'severity': rule.get('severity', 0.5),
                        'message': f"Field '{field_name}' value {value} is above maximum {max_val}"
                    }
        
        elif rule_type == 'format_check':
            field_name = rule['field']
            format_pattern = rule.get('format_pattern')
            
            if field_name in config and format_pattern:
                value = config[field_name]
                # Simple format check
                if isinstance(value, str) and format_pattern == 'non_empty' and not value.strip():
                    return {
                        'rule_type': rule_type,
                        'field': field_name,
                        'value': value,
                        'severity': rule.get('severity', 0.5),
                        'message': f"Field '{field_name}' is empty"
                    }
        
        elif rule_type == 'security_check':
            field_name = rule['field']
            
            if field_name in config:
                value = config[field_name]
                # Check for sensitive data
                if 'password' in field_name.lower() and str(value):
                    return {
                        'rule_type': rule_type,
                        'field': field_name,
                        'severity': rule.get('severity', 1.0),
                        'message': f"Field '{field_name}' contains sensitive data and should be encrypted"
                    }
        
        return None
    
    def _generate_audit_recommendation(self, config: Dict, rule: Dict) -> Optional[str]:nlm_advanced/config/__init__.py