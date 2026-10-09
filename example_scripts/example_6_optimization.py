#!/usr/bin/env python3
"""
Example 6: Configuration Optimization

This example demonstrates configuration optimization techniques for
NLM simulations. It focuses on finding optimal parameter combinations
for specific performance objectives.

Key features demonstrated:
- Multi-objective optimization
- Constraint-based optimization
- Parameter sensitivity analysis
- Trade-off analysis
- Automated configuration search
"""

import pynlm
from advanced_nlm_tools import (
    NLMExperimentRunner,
    ExperimentConfig,
    NLMConfigAnalyzer,
    NLMAnalysisTools
)
import numpy as np
import pandas as pd
import matplotlib.pyplot as plt
from scipy import optimize
from sklearn.preprocessing import StandardScaler
import warnings
warnings.filterwarnings('ignore')

class ConfigOptimizer:
    """
    Advanced configuration optimizer for NLM simulations.
    
    This class provides tools for:
    - Multi-objective optimization
    - Constraint handling
    - Parameter sensitivity analysis
    - Trade-off curve generation
    - Automated configuration discovery
    """
    
    def __init__(self):
        self.optimization_history = []
        self.objective_functions = {}
        self.constraints = {}
        self.parameter_ranges = {}
    
    def add_objective(self, name, func, weight=1.0):
        """
        Add an objective function for optimization.
        
        Args:
            name: Name of the objective
            func: Objective function (takes config parameters, returns score)
            weight: Weight of this objective in multi-objective optimization
        """
        self.objective_functions[name] = {
            'func': func,
            'weight': weight,
            'history': []
        }
    
    def add_constraint(self, name, func, violation_penalty=1000.0):
        """
        Add a constraint function.
        
        Args:
            name: Name of the constraint
            func: Constraint function (takes config parameters, returns violation score)
            violation_penalty: Penalty for constraint violations
        """
        self.constraints[name] = {
            'func': func,
            'violation_penalty': violation_penalty,
            'history': []
        }
    
    def set_parameter_ranges(self, param_ranges):
        """
        Set parameter ranges for optimization.
        
        Args:
            param_ranges: Dictionary of parameter names and ranges
        """
        self.parameter_ranges = param_ranges
    
    def evaluate_configuration(self, params):
        """
        Evaluate a configuration against all objectives and constraints.
        
        Args:
            params: Dictionary of parameter values
            
        Returns:
            dict: Evaluation results
        """
        evaluation = {
            'parameters': params.copy(),
            'objectives': {},
            'constraints': {},
            'total_objective': 0.0,
            'constraint_violations': 0,
            'penalty': 0.0,
            'success': True
        }
        
        try:
            # Evaluate objectives
            for obj_name, obj_info in self.objective_functions.items():
                try:
                    score = obj_info['func'](params)
                    evaluation['objectives'][obj_name] = score
                    evaluation['total_objective'] += obj_info['weight'] * score
                    obj_info['history'].append(score)
                except Exception as e:
                    print(f"Error evaluating objective {obj_name}: {e}")
                    evaluation['success'] = False
            
            # Evaluate constraints
            for constraint_name, constraint_info in self.constraints.items():
                try:
                    violation = constraint_info['func'](params)
                    evaluation['constraints'][constraint_name] = violation
                    
                    if violation > 0:
                        evaluation['constraint_violations'] += 1
                        evaluation['penalty'] += constraint_info['violation_penalty']
                    
                    constraint_info['history'].append(violation)
                except Exception as e:
                    print(f"Error evaluating constraint {constraint_name}: {e}")
                    evaluation['success'] = False
            
            # Calculate final objective (with penalty)
            evaluation['final_objective'] = evaluation['total_objective'] + evaluation['penalty']
            
        except Exception as e:
            print(f"Error evaluating configuration: {e}")
            evaluation['success'] = False
        
        self.optimization_history.append(evaluation)
        return evaluation
    
    def multi_objective_optimization(self, initial_params=None, iterations=100):
        """
        Perform multi-objective optimization using evolutionary strategy.
        
        Args:
            initial_params: Initial parameter values
            iterations: Number of optimization iterations
            
        Returns:
            dict: Optimization results
        """
        print("Running multi-objective optimization...")
        
        if initial_params is None:
            initial_params = {name: np.mean(ranges) 
                            for name, ranges in self.parameter_ranges.items()}
        
        current_params = initial_params.copy()
        best_params = initial_params.copy()
        best_score = float('inf')
        
        print(f"  Starting from: {current_params}")
        print(f"  Running {iterations} iterations...")
        
        for iteration in range(iterations):
            # Generate new parameters (simple mutation)
            new_params = self._mutate_parameters(current_params)
            
            # Evaluate new parameters
            evaluation = self.evaluate_configuration(new_params)
            
            if evaluation['success']:
                score = evaluation['final_objective']
                
                # Update best parameters
                if score < best_score:
                    best_score = score
                    best_params = new_params.copy()
                
                # Accept new parameters (simple hill climbing)
                if score < self.evaluate_configuration(current_params)['final_objective']:
                    current_params = new_params.copy()
                
                if (iteration + 1) % 20 == 0:
                    print(f"    Iteration {iteration + 1}: best score = {best_score:.4f}")
        
        optimization_results = {
            'best_params': best_params,
            'best_score': best_score,
            'optimization_history': self.optimization_history,
            'convergence': self._calculate_convergence(),
            'pareto_front': self._calculate_pareto_front()
        }
        
        return optimization_results
    
    def _mutate_parameters(self, params):
        """Generate new parameters by mutating existing ones."""
        new_params = params.copy()
        
        for name, ranges in self.parameter_ranges.items():
            if isinstance(ranges, tuple) and len(ranges) == 2:
                # Gaussian mutation
                mutation_strength = (ranges[1] - ranges[0]) * 0.1  # 10% of range
                new_params[name] = np.clip(
                    params[name] + np.random.normal(0, mutation_strength),
                    ranges[0], ranges[1]
                )
            elif isinstance(ranges, dict):
                # Discrete choice mutation
                choices = list(ranges.keys())
                new_params[name] = np.random.choice(choices)
        
        return new_params
    
    def _calculate_convergence(self):
        """Calculate convergence metrics."""
        if not self.optimization_history:
            return {}
        
        scores = [eval['final_objective'] for eval in self.optimization_history if eval['success']]
        
        if len(scores) < 2:
            return {'converged': False, 'improvement_rate': 0}
        
        # Calculate improvement rate
        early_scores = scores[:len(scores)//4]
        late_scores = scores[-len(scores)//4:]
        
        if early_scores:
            early_avg = np.mean(early_scores)
            late_avg = np.mean(late_scores)
            improvement_rate = (early_avg - late_avg) / early_avg if early_avg > 0 else 0
        else:
            improvement_rate = 0
        
        return {
            'converged': improvement_rate < 0.01,  # Converged if < 1% improvement
            'improvement_rate': improvement_rate,
            'best_score': np.min(scores) if scores else 0,
            'final_score': scores[-1] if scores else 0
        }
    
    def _calculate_pareto_front(self):
        """Calculate Pareto front for multi-objective optimization."""
        if not self.optimization_history:
            return []
        
        # Extract objective values from history
        pareto_points = []
        
        for eval_result in self.optimization_history:
            if eval_result['success'] and eval_result['objectives']:
                # Collect objective values
                objectives = list(eval_result['objectives'].values())
                pareto_points.append({
                    'parameters': eval_result['parameters'],
                    'objectives': objectives,
                    'final_objective': eval_result['final_objective']
                })
        
        if not pareto_points:
            return []
        
        # Simple Pareto front calculation (for demonstration)
        # In practice, you'd use more sophisticated algorithms
        pareto_front = []
        
        for point in pareto_points:
            dominated = False
            for other in pareto_points:
                if other is not point:
                    # Check if other dominates point
                    if (all(o1 <= o2 for o1, o2 in zip(point['objectives'], other['objectives'])) and
                        any(o1 < o2 for o1, o2 in zip(point['objectives'], other['objectives']))):
                        dominated = True
                        break
            
            if not dominated:
                pareto_front.append(point)
        
        return pareto_front
    
    def sensitivity_analysis(self, iterations=50):
        """
        Perform parameter sensitivity analysis.
        
        Args:
            iterations: Number of iterations for sensitivity analysis
            
        Returns:
            dict: Sensitivity analysis results
        """
        print("Running sensitivity analysis...")
        
        if not self.optimization_history:
            return {'error': 'No optimization history available'}
        
        sensitivity_results = {
            'parameters': list(self.parameter_ranges.keys()),
            'correlation_matrix': np.zeros((len(self.parameter_ranges), len(self.parameter_ranges))),
            'importance_scores': np.zeros(len(self.parameter_ranges)),
            'sensitivity_indices': {}
        }
        
        # Collect parameter values and objective values
        param_values = {name: [] for name in self.parameter_ranges.keys()}
        objective_values = {name: [] for name in self.objective_functions.keys()}
        
        for eval_result in self.optimization_history:
            if eval_result['success']:
                for param_name in self.parameter_ranges.keys():
                    param_values[param_name].append(eval_result['parameters'].get(param_name))
                
                for obj_name in self.objective_functions.keys():
                    if obj_name in eval_result['objectives']:
                        objective_values[obj_name].append(eval_result['objectives'][obj_name])
        
        # Calculate correlation matrix
        for i, param_name in enumerate(sensitivity_results['parameters']):
            for j, obj_name in enumerate(self.objective_functions.keys()):
                if (param_name in param_values and obj_name in objective_values and
                    len(param_values[param_name]) == len(objective_values[obj_name]) and
                    len(param_values[param_name]) > 1):
                    
                    correlation = np.corrcoef(param_values[param_name], 
                                            objective_values[obj_name])[0, 1]
                    sensitivity_results['correlation_matrix'][i, j] = correlation
        
        # Calculate importance scores
        for i, param_name in enumerate(sensitivity_results['parameters']):
            correlations = sensitivity_results['correlation_matrix'][i, :]
            # Exclude self-correlation
            correlations = correlations[~np.eye(len(correlations), dtype=bool)]
            if len(correlations) > 0:
                importance = np.sqrt(np.sum(correlations**2))
            else:
                importance = 0
            sensitivity_results['importance_scores'][i] = importance
        
        # Calculate sensitivity indices
        for param_name in sensitivity_results['parameters']:
            if param_name in param_values and objective_values:
                # Calculate partial correlation
                param_values_array = np.array(param_values[param_name])
                
                for obj_name in self.objective_functions.keys():
                    if obj_name in objective_values:
                        obj_values = np.array(objective_values[obj_name])
                        
                        # Handle NaN values
                        mask = ~(np.isnan(param_values_array) | np.isnan(obj_values))
                        if np.sum(mask) > 2:
                            corr_matrix = np.corrcoef(param_values_array[mask], obj_values[mask])
                            sensitivity = abs(corr_matrix[0, 1])
                            
                            sensitivity_results['sensitivity_indices'][f"{param_name}_{obj_name}"] = sensitivity
        
        return sensitivity_results

def create_configuration_for_objective(neuron_count, timestep=0.001, 
                                      plasticity=True, curiosity=True, 
                                      development=False):
    """Create configuration optimized for specific objective."""
    config = pynlm.createDefaultConfig()
    
    # Set parameters based on objectives
    config.set("brain.neuron_count", neuron_count)
    config.set("simulation_timestep", timestep)
    config.set("random_seed", 42)
    
    # Plasticity configuration
    config.set("plasticity.stdp.enable", plasticity)
    config.set("plasticity.hebbian.enable", plasticity)
    config.set("plasticity.stdp.learning_rate", 0.001)
    config.set("plasticity.hebbian.learning_rate", 0.0005)
    config.set("plasticity.structural.enable", plasticity)
    
    # Neuromodulation
    config.set("neuromod.dopamine.scale", 1.0)
    config.set("neuromod.curiosity.enable", curiosity)
    config.set("neuromod.novelty.enable", True)
    
    # Development
    config.set("development.enable", development)
    
    return config

def create_multi_objective_config(neuron_count, timestep=0.001, 
                                  plasticity=True, curiosity=True, 
                                  development=True, learning_rate=0.001):
    """Create configuration optimized for multiple objectives."""
    config = create_configuration_for_objective(
        neuron_count, timestep, plasticity, curiosity, development
    )
    
    # Additional parameters for multi-objective optimization
    config.set("plasticity.stdp.learning_rate", learning_rate)
    config.set("plasticity.hebbian.learning_rate", learning_rate * 0.5)
    config.set("neuromod.curiosity.threshold", 0.3)
    config.set("neuromod.novelty.sensitivity", 0.5)
    
    return config

def example_objective_functions():
    """Define example objective functions for optimization."""
    
    def objective_fast_performance(params):
        """Objective: Maximize speed (minimize time)."""
        neuron_count = params.get('neuron_count', 1000)
        timestep = params.get('timestep', 0.001)
        
        # Time proportional to neuron_count / timestep
        time_cost = neuron_count / timestep
        
        # Bonus for no plasticity (faster)
        plasticity_bonus = 0 if params.get('plasticity', True) else -100
        
        return time_cost + plasticity_bonus
    
    def objective_learning_effectiveness(params):
        """Objective: Maximize learning effectiveness."""
        neuron_count = params.get('neuron_count', 1000)
        
        # Learning effectiveness increases with neuron count
        learning_score = neuron_count / 1000.0
        
        # Adjust for other parameters
        if params.get('curiosity', True):
            learning_score *= 1.2
        
        if params.get('development', False):
            learning_score *= 1.1
        
        # Learning rate factor
        learning_rate = params.get('learning_rate', 0.001)
        learning_score *= min(learning_rate / 0.001, 2.0)
        
        return -learning_score  # Negative because we minimize
    
    def objective_resource_efficiency(params):
        """Objective: Maximize resource efficiency (neurons per time unit)."""
        neuron_count = params.get('neuron_count', 1000)
        timestep = params.get('timestep', 0.001)
        
        # Efficiency = neurons / (time * resources)
        time_cost = neuron_count / timestep
        efficiency = neuron_count / (time_cost + 1)  # +1 to avoid division by zero
        
        return -efficiency
    
    def objective_balance(params):
        """Objective: Balance between performance and learning."""
        fast = objective_fast_performance(params)
        learning = objective_learning_effectiveness(params)
        
        # Weighted combination
        return 0.6 * fast + 0.4 * learning
    
    return {
        'fast_performance': objective_fast_performance,
        'learning_effectiveness': objective_learning_effectiveness,
        'resource_efficiency': objective_resource_efficiency,
        'balanced': objective_balance
    }

def example_constraints():
    """Define example constraints for optimization."""
    
    def max_memory_constraint(params):
        """Constraint: Maximum memory usage."""
        neuron_count = params.get('neuron_count', 1000)
        # Memory roughly proportional to neuron count
        estimated_memory = neuron_count * 0.001  # 1ms per neuron
        max_memory = 5.0  # 5MB max
        
        return max(0, estimated_memory - max_memory)
    
    def min_performance_constraint(params):
        """Constraint: Minimum performance requirement."""
        neuron_count = params.get('neuron_count', 1000)
        # Performance should be at least 500 neurons
        required_neurons = 500
        
        return max(0, required_neurons - neuron_count)
    
    def stability_constraint(params):
        """Constraint: Stability requirement."""
        timestep = params.get('timestep', 0.001)
        # Timestep should be small enough for stability
        max_timestep = 0.01
        
        return max(0, timestep - max_timestep)
    
    return {
        'max_memory': max_memory_constraint,
        'min_performance': min_performance_constraint,
        'stability': stability_constraint
    }

def example_configuration_optimization():
    """Demonstrate configuration optimization."""
    print("=== Example 6: Configuration Optimization ===\n")
    
    # Create optimizer
    optimizer = ConfigOptimizer()
    
    # Define parameter ranges
    param_ranges = {
        'neuron_count': (500, 5000),
        'timestep': (0.0005, 0.002),
        'plasticity': True,
        'curiosity': True,
        'development': False,
        'learning_rate': (0.0005, 0.005)
    }
    
    optimizer.set_parameter_ranges(param_ranges)
    
    # Add objective functions
    objectives = example_objective_functions()
    
    for name, func in objectives.items():
        weight = 1.0 if name == 'balanced' else 0.5
        optimizer.add_objective(name, func, weight)
    
    # Add constraints
    constraints = example_constraints()
    
    for name, func in constraints.items():
        optimizer.add_constraint(name, func)
    
    # Run optimization
    print("1. Running multi-objective optimization...")
    optimization_results = optimizer.multi_objective_optimization(iterations=50)
    
    print(f"\nOptimization Results:")
    print(f"  Best score: {optimization_results['best_score']:.4f}")
    print(f"  Converged: {optimization_results['convergence']['converged']}")
    print(f"  Improvement rate: {optimization_results['convergence']['improvement_rate']:.4f}")
    print(f"  Pareto front size: {len(optimization_results['pareto_front'])}")
    
    if optimization_results['pareto_front']:
        print(f"\nPareto Front (first 5 points):")
        for i, point in enumerate(optimization_results['pareto_front'][:5]):
            print(f"  Point {i+1}:")
            print(f"    Parameters: {point['parameters']}")
            print(f"    Objectives: {point['objectives']}")
            print(f"    Final Objective: {point['final_objective']:.4f}")
    
    # Run sensitivity analysis
    print("\n2. Running sensitivity analysis...")
    sensitivity_results = optimizer.sensitivity_analysis(iterations=50)
    
    if 'error' not in sensitivity_results:
        print(f"\nSensitivity Analysis Results:")
        print(f"  Parameter importance:")
        for i, param in enumerate(sensitivity_results['parameters']):
            print(f"    {param}: {sensitivity_results['importance_scores'][i]:.3f}")
        
        if sensitivity_results['sensitivity_indices']:
            print(f"\nSensitivity Indices:")
            for param_obj, sensitivity in list(sensitivity_results['sensitivity_indices'].items())[:5]:
                print(f"  {param_obj}: {sensitivity:.3f}")
    
    # Create and test optimized configurations
    print("\n3. Testing optimized configurations...")
    
    # Test best configuration
    best_params = optimization_results['best_params']
    print(f"\nBest configuration: {best_params}")
    
    # Create actual NLM configuration
    config = create_multi_objective_config(**best_params)
    
    # Test with experiment runner
    print("\n4. Validating best configuration...")
    
    # Run quick validation
    brain = pynlm.createBrain(config)
    brain.initialize()
    
    validation_metrics = {
        'neuron_count': brain.getTotalNeuronCount(),
        'initial_firing': brain.getFiringNeuronCount(),
        'learning_enabled': config.getOr<bool>('plasticity.stdp.enable', False),
        'curiosity_enabled': config.getOr<bool>('neuromod.curiosity.enable', False),
        'development_enabled': config.getOr<bool>('development.enable', False)
    }
    
    print(f"Validation metrics:")
    for key, value in validation_metrics.items():
        print(f"  {key}: {value}")
    
    return optimizer, optimization_results, sensitivity_results, validation_metrics

def example_parameter_sensitivity():
    """Demonstrate parameter sensitivity analysis."""
    print("\n=== Parameter Sensitivity Analysis Example ===\n")
    
    # Create simple optimization with sensitivity analysis
    optimizer = ConfigOptimizer()
    
    # Simple parameter ranges
    param_ranges = {
        'neuron_count': (1000, 5000),
        'timestep': (0.001, 0.005),
        'learning_rate': (0.0001, 0.001)
    }
    
    optimizer.set_parameter_ranges(param_ranges)
    
    # Add single objective for sensitivity analysis
    def simple_objective(params):
        neuron_count = params.get('neuron_count', 1000)
        timestep = params.get('timestep', 0.001)
        learning_rate = params.get('learning_rate', 0.001)
        
        # Simple objective combining factors
        score = (neuron_count / 5000) + (timestep / 0.005) + (learning_rate / 0.001)
        return score
    
    optimizer.add_objective('combined_score', simple_objective, weight=1.0)
    
    # Run optimization to generate history
    print("1. Running optimization for sensitivity analysis...")
    optimization_results = optimizer.multi_objective_optimization(iterations=30)
    
    # Run sensitivity analysis
    print("\n2. Running sensitivity analysis...")
    sensitivity_results = optimizer.sensitivity_analysis()
    
    if 'error' not in sensitivity_results:
        print(f"\nSensitivity Analysis:")
        print(f"  Most sensitive parameter: {sensitivity_results['parameters'][np.argmax(sensitivity_results['importance_scores'])]}")
        print(f"  Parameter importance scores:")
        
        for i, param in enumerate(sensitivity_results['parameters']):
            importance = sensitivity_results['importance_scores'][i]
            print(f"    {param}: {importance:.3f}")
            
            # Show correlations with objectives
            correlations = []
            for j, obj_name in enumerate(list(optimizer.objective_functions.keys())[:2]):
                corr = sensitivity_results['correlation_matrix'][i, j]
                if not np.isnan(corr):
                    correlations.append(f"{obj_name}: {corr:.3f}")
            
            if correlations:
                print(f"      Correlations: {', '.join(correlations)}")
    
    return optimizer, sensitivity_results

if __name__ == "__main__":
    print("NLM Advanced Features - Example 6: Configuration Optimization")
    print("=" * 65)
    
    try:
        # Run configuration optimization example
        optimizer, optimization_results, sensitivity_results, validation_metrics = example_configuration_optimization()
        
        # Run parameter sensitivity example
        sensitivity_optimizer, sensitivity_analysis = example_parameter_sensitivity()
        
        print("\n" + "=" * 65)
        print("Example 6 completed successfully!")
        print("Key insights:")
        print("  - Multi-objective optimization finds balanced solutions")
        print("  - Sensitivity analysis identifies critical parameters")
        print("  - Constraints ensure feasible configurations")
        print("  - Pareto front shows trade-offs between objectives")
        print("  - Optimization history helps understand convergence")
        
    except Exception as e:
        print(f"\nError in example 6: {e}")
        import traceback
        traceback.print_exc()
