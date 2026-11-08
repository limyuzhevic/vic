#!/usr/bin/env python3
"""
Configuration optimization utilities for the NLM Python package.

This module provides tools for automatically optimizing NLM configurations for
better performance, learning capability, and stability.
"""

import pynlm
from pynlm import createDefaultConfig
from typing import Dict, Any, Optional, Callable, List
import numpy as np
import time
from dataclasses import dataclass
@dataclass
class OptimizationResult:
    """Result of an optimization run."""
    config: Dict[str, Any]
    score: float
    metadata: Dict[str, Any]
    convergence_info: Optional[Dict[str, Any]] = None
class ConfigurationOptimizer:
    """
    Optimizes NLM configurations for specific goals.
    
    This class provides methods to optimize NLM configurations for different
    objectives such as learning performance, stability, or raw performance.
    """
    
    def __init__(self, config: Optional[Dict[str, Any]] = None):
        """
        Initialize the configuration optimizer.
        
        Args:
            config: Base configuration to optimize (defaults to createDefaultConfig())
        """
        self.base_config = config or createDefaultConfig()
        self.optimization_history = []
    
    def optimize_for_learning(
        self,
        target_performance: float = 0.8,
        max_iterations: int = 100,
        learning_rate: float = 0.01
    ) -> OptimizationResult:
        """
        Optimize configuration for learning performance.
        
        Args:
            target_performance: Target performance level (0.0 to 1.0)
            max_iterations: Maximum number of optimization iterations
            learning_rate: Learning rate for optimization
            
        Returns:
            Optimization result with best configuration
        """
        print(f"Optimizing for learning performance (target: {target_performance})...")
        
        # Define learning performance metric
        def learning_performance_metric(config: Dict[str, Any]) -> float:
            """Calculate learning performance for a configuration."""
            nlm_config = createDefaultConfig()
            
            # Apply configuration parameters
            for key, value in config.items():
                if key.startswith("brain."):
                    nlm_config.set(key[5:], value)
                elif key.startswith("plasticity."):
                    nlm_config.set(key[7:], value)
                elif key.startswith("neuromod."):
                    nlm_config.set(key[8:], value)
                elif key.startswith("development."):
                    nlm_config.set(key[10:], value)
            
            # Create and test brain
            brain = pynlm.createBrain(nlm_config)
            brain.initialize()
            
            # Run simulation
            for step in range(200):
                brain.step(step)
            
            # Calculate learning metrics
            spike_count = brain.getTotalSpikeCount()
            firing_rate = brain.getAverageFiringRate()
            development_stage = brain.getDevelopmentalStage().value
            
            # Normalize metrics (simple approach)
            spike_score = min(spike_count / 1000.0, 1.0)  # Cap at 1.0
            firing_score = min(firing_rate / 10.0, 1.0)  # Cap at 1.0
            learning_score = development_stage / 5.0  # Max development stage is 4
            
            # Weighted combination
            total_score = spike_score * 0.4 + firing_score * 0.4 + learning_score * 0.2
            
            return total_score
        
        # Initial configuration
        current_config = self._get_config_params(self.base_config)
        best_config = current_config.copy()
        best_score = learning_performance_metric(current_config)
        
        # Optimization loop
        for iteration in range(max_iterations):
            # Generate random perturbations
            perturbed_config = self._apply_perturbation(current_config, learning_rate)
            
            # Evaluate perturbed configuration
            score = learning_performance_metric(perturbed_config)
            
            # Update best configuration if improved
            if score > best_score:
                best_config = perturbed_config.copy()
                best_score = score
                self.optimization_history.append({
                    'iteration': iteration,
                    'score': best_score,
                    'config': best_config.copy()
                })
            
            # Acceptance criterion (simulated annealing-like)
            acceptance_prob = np.exp((score - best_score) / learning_rate) if score > best_score else 1.0
            if np.random.random() < acceptance_prob:
                current_config = perturbed_config.copy()
            
            # Adaptive learning rate
            learning_rate *= 0.99
            
            # Log progress
            if iteration % 20 == 0:
                print(f"  Iteration {iteration}: score = {score:.4f}, best = {best_score:.4f}")
        
        # Create result
        result = OptimizationResult(
            config=best_config,
            score=best_score,
            metadata={
                'target_performance': target_performance,
                'max_iterations': max_iterations,
                'final_learning_rate': learning_rate,
                'convergence_iter': len(self.optimization_history)
            }
        )
        
        print(f"Learning optimization completed. Best score: {best_score:.4f}")
        return result
    
    def optimize_for_stability(
        self,
        metric: Callable[[Dict[str, Any]], float],
        max_iterations: int = 50,
        mutation_rate: float = 0.1
    ) -> OptimizationResult:
        """
        Optimize configuration for network stability.
        
        Args:
            metric: Function that evaluates configuration stability
            max_iterations: Maximum number of optimization iterations
            mutation_rate: Rate of mutation for genetic algorithm
            
        Returns:
            Optimization result with best configuration
        """
        print("Optimizing for network stability...")
        
        # Initialize population
        population_size = 10
        population = []
        
        for _ in range(population_size):
            config = self._get_random_config()
            population.append(config)
        
        # Evaluate initial population
        scores = [metric(config) for config in population]
        best_idx = np.argmax(scores)
        best_config = population[best_idx].copy()
        best_score = scores[best_idx]
        
        # Genetic algorithm
        for generation in range(max_iterations):
            # Create new population
            new_population = []
            
            # Elitism: keep best solution
            new_population.append(best_config.copy())
            
            # Generate offspring
            while len(new_population) < population_size:
                # Select parents
                parent1_idx = np.random.choice(len(population), p=self._selection_probabilities(scores))
                parent2_idx = np.random.choice(len(population), p=self._selection_probabilities(scores))
                
                parent1 = population[parent1_idx]
                parent2 = population[parent2_idx]
                
                # Crossover
                if np.random.random() < 0.8:  # Crossover probability
                    child = self._crossover(parent1, parent2)
                else:
                    child = parent1.copy()
                
                # Mutation
                if np.random.random() < mutation_rate:
                    child = self._mutate(child)
                
                new_population.append(child)
            
            # Replace population
            population = new_population
            
            # Evaluate new population
            scores = [metric(config) for config in population]
            
            # Update best solution
            current_best_idx = np.argmax(scores)
            current_best_score = scores[current_best_idx]
            
            if current_best_score > best_score:
                best_config = population[current_best_idx].copy()
                best_score = current_best_score
                print(f"  Generation {generation}: new best score = {best_score:.4f}")
        
        # Create result
        result = OptimizationResult(
            config=best_config,
            score=best_score,
            metadata={
                'max_iterations': max_iterations,
                'population_size': population_size,
                'final_mutation_rate': mutation_rate
            }
        )
        
        print(f"Stability optimization completed. Best score: {best_score:.4f}")
        return result
    
    def optimize_for_performance(
        self,
        max_iterations: int = 40,
        convergence_threshold: float = 0.001
    ) -> OptimizationResult:
        """
        Optimize configuration for raw performance.
        
        Args:
            max_iterations: Maximum number of optimization iterations
            convergence_threshold: Threshold for convergence
            
        Returns:
            Optimization result with best configuration
        """
        print("Optimizing for raw performance...")
        
        # Use gradient-based optimization
        config = self._get_config_params(self.base_config)
        best_config = config.copy()
        best_score = 0.0
        
        learning_rate = 0.01
        
        for iteration in range(max_iterations):
            # Calculate gradient numerically
            gradient = self._calculate_gradient(config)
            
            # Update configuration
            for key in config:
                config[key] += learning_rate * gradient[key]
                # Clip values to reasonable ranges
                config[key] = max(0.0, min(1.0, config[key]))
            
            # Evaluate current configuration
            current_score = self._evaluate_config(config)
            
            # Update best solution
            if current_score > best_score:
                best_config = config.copy()
                best_score = current_score
                print(f"  Iteration {iteration}: new best score = {best_score:.4f}")
            
            # Check for convergence
            if len(self.optimization_history) > 10:
                recent_scores = [h['score'] for h in self.optimization_history[-10:]]
                if max(recent_scores) - min(recent_scores) < convergence_threshold:
                    print(f"  Converged after {iteration} iterations")
                    break
            
            # Store optimization history
            self.optimization_history.append({
                'iteration': iteration,
                'score': current_score,
                'config': config.copy()
            })
            
            # Reduce learning rate
            learning_rate *= 0.95
        
        # Create result
        result = OptimizationResult(
            config=best_config,
            score=best_score,
            metadata={
                'max_iterations': max_iterations,
                'convergence_threshold': convergence_threshold,
                'final_learning_rate': learning_rate
            }
        )
        
        print(f"Performance optimization completed. Best score: {best_score:.4f}")
        return result
    
    def validate_config(self, config: Dict[str, Any]) -> bool:
        """
        Validate configuration parameters.
        
        Args:
            config: Configuration to validate
            
        Returns:
            True if configuration is valid, False otherwise
        """
        try:
            # Check required parameters
            required_keys = ['brain.neuron_count', 'brain.synapse_density']
            for key in required_keys:
                if key not in config:
                    print(f"Missing required parameter: {key}")
                    return False
            
            # Validate parameter ranges
            neuron_count = config['brain.neuron_count']
            if not (100 <= neuron_count <= 10000):
                print(f"Invalid neuron_count: {neuron_count}. Must be between 100 and 10000")
                return False
            
            synapse_density = config['brain.synapse_density']
            if not (0.01 <= synapse_density <= 0.5):
                print(f"Invalid synapse_density: {synapse_density}. Must be between 0.01 and 0.5")
                return False
            
            # Check plasticity parameters
            if 'plasticity.stdp.learning_rate' in config:
                ltp = config['plasticity.stdp.learning_rate']
                if not (0.0 <= ltp <= 1.0):
                    print(f"Invalid STDP learning rate: {ltp}. Must be between 0.0 and 1.0")
                    return False
            
            return True
            
        except Exception as e:
            print(f"Configuration validation error: {e}")
            return False
    
    def _get_config_params(self, config: Any) -> Dict[str, Any]:
        """Extract configuration parameters from a Config object."""
        params = {}
        
        # Get brain parameters
        params['brain.neuron_count'] = config.getOr[int64_t]("brain.neuron_count", 1000)
        params['brain.synapse_density'] = config.getOr[float]("brain.synapse_density", 0.1)
        params['brain.connection_probability'] = config.getOr[float]("brain.connection_probability", 0.05)
        
        # Get plasticity parameters
        params['plasticity.stdp.learning_rate'] = config.getOr[float]("plasticity.stdp.learning_rate", 0.001)
        params['plasticity.stdp.tau_plus'] = config.getOr[float]("plasticity.stdp.tau_plus", 20.0)
        params['plasticity.stdp.tau_minus'] = config.getOr[float]("plasticity.stdp.tau_minus", 20.0)
        
        params['plasticity.hebbian.learning_rate'] = config.getOr[float]("plasticity.hebbian.learning_rate", 0.01)
        params['plasticity.hebbian.enable'] = config.getOr[bool]("plasticity.hebbian.enable", True)
        
        params['plasticity.structural.enable'] = config.getOr[bool]("plasticity.structural.enable", True)
        params['plasticity.structural.synaptogenesis_rate'] = config.getOr[float]("plasticity.structural.synaptogenesis_rate", 0.0001)
        params['plasticity.structural.pruning_rate'] = config.getOr[float]("plasticity.structural.pruning_rate", 0.00001)
        
        # Get neuromodulation parameters
        params['neuromod.dopamine.scale'] = config.getOr[float]("neuromod.dopamine.scale", 1.0)
        params['neuromod.curiosity.enable'] = config.getOr[bool]("neuromod.curiosity.enable", True)
        params['neuromod.novelty.enable'] = config.getOr[bool]("neuromod.novelty.enable", True)
        
        # Get development parameters
        params['development.initial_plasticity'] = config.getOr[float]("development.initial_plasticity", 1.0)
        params['development.plasticity_decay_rate'] = config.getOr[float]("development.plasticity_decay_rate", 0.01)
        
        return params
    
    def _apply_perturbation(self, config: Dict[str, Any], learning_rate: float) -> Dict[str, Any]:
        """Apply random perturbations to a configuration."""
        perturbed = config.copy()
        
        for key in perturbed:
            if np.random.random() < learning_rate * 10:  # Perturbation probability
                if key.startswith('brain.'):
                    # Perturb brain parameters
                    if 'neuron_count' in key:
                        perturbed[key] = max(100, min(10000, int(perturbed[key] * (1 + np.random.uniform(-0.2, 0.2)))))
                    elif 'synapse_density' in key:
                        perturbed[key] = max(0.01, min(0.5, perturbed[key] * (1 + np.random.uniform(-0.3, 0.3))))
                    elif 'connection_probability' in key:
                        perturbed[key] = max(0.01, min(0.2, perturbed[key] * (1 + np.random.uniform(-0.4, 0.4))))
                
                elif key.startswith('plasticity.'):
                    # Perturb plasticity parameters
                    if 'learning_rate' in key:
                        perturbed[key] = max(0.0, min(1.0, perturbed[key] * (1 + np.random.uniform(-0.5, 0.5))))
                    elif 'tau_' in key:
                        perturbed[key] = max(1.0, min(100.0, perturbed[key] * (1 + np.random.uniform(-0.3, 0.3))))
                
                elif key.startswith('neuromod.'):
                    # Perturb neuromodulation parameters
                    if 'scale' in key:
                        perturbed[key] = max(0.0, min(2.0, perturbed[key] * (1 + np.random.uniform(-0.4, 0.4))))
                
                elif key.startswith('development.'):
                    # Perturb development parameters
                    if 'rate' in key:
                        perturbed[key] = max(0.0, min(1.0, perturbed[key] * (1 + np.random.uniform(-0.5, 0.5))))
        
        return perturbed
    
    def _get_random_config(self) -> Dict[str, Any]:
        """Generate a random configuration."""
        config = {}
        
        # Random brain parameters
        config['brain.neuron_count'] = np.random.choice([100, 500, 1000, 2000, 5000])
        config['brain.synapse_density'] = np.random.uniform(0.05, 0.3)
        config['brain.connection_probability'] = np.random.uniform(0.01, 0.1)
        
        # Random plasticity parameters
        config['plasticity.stdp.learning_rate'] = np.random.uniform(0.0001, 0.01)
        config['plasticity.stdp.tau_plus'] = np.random.uniform(10.0, 50.0)
        config['plasticity.stdp.tau_minus'] = np.random.uniform(10.0, 50.0)
        
        config['plasticity.hebbian.learning_rate'] = np.random.uniform(0.001, 0.1)
        config['plasticity.hebbian.enable'] = np.random.choice([True, False])
        
        config['plasticity.structural.enable'] = np.random.choice([True, False])
        config['plasticity.structural.synaptogenesis_rate'] = np.random.uniform(0.00001, 0.001)
        config['plasticity.structural.pruning_rate'] = np.random.uniform(0.000001, 0.00001)
        
        # Random neuromodulation parameters
        config['neuromod.dopamine.scale'] = np.random.uniform(0.5, 2.0)
        config['neuromod.curiosity.enable'] = np.random.choice([True, False])
        config['neuromod.novelty.enable'] = np.random.choice([True, False])
        
        # Random development parameters
        config['development.initial_plasticity'] = np.random.uniform(0.5, 2.0)
        config['development.plasticity_decay_rate'] = np.random.uniform(0.001, 0.1)
        
        return config
    
    def _crossover(self, parent1: Dict[str, Any], parent2: Dict[str, Any]) -> Dict[str, Any]:
        """Perform crossover between two parent configurations."""
        child = parent1.copy()
        
        # Randomly select which parameters to inherit from parent2
        for key in parent2:
            if np.random.random() < 0.5:  # Crossover probability
                child[key] = parent2[key]
        
        return child
    
    def _mutate(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Apply mutation to a configuration."""
        mutated = config.copy()
        
        for key in mutated:
            if np.random.random() < 0.1:  # Mutation probability
                if key.startswith('brain.'):
                    if 'neuron_count' in key:
                        mutated[key] = max(100, min(10000, int(mutated[key] * (1 + np.random.uniform(-0.5, 0.5)))))
                    elif 'synapse_density' in key:
                        mutated[key] = max(0.01, min(0.5, mutated[key] * (1 + np.random.uniform(-0.5, 0.5))))
                    elif 'connection_probability' in key:
                        mutated[key] = max(0.01, min(0.2, mutated[key] * (1 + np.random.uniform(-0.5, 0.5))))
                
                elif key.startswith('plasticity.'):
                    if 'learning_rate' in key:
                        mutated[key] = max(0.0, min(1.0, mutated[key] * (1 + np.random.uniform(-0.7, 0.7))))
                    elif 'tau_' in key:
                        mutated[key] = max(1.0, min(100.0, mutated[key] * (1 + np.random.uniform(-0.5, 0.5))))
                
                elif key.startswith('neuromod.'):
                    if 'scale' in key:
                        mutated[key] = max(0.0, min(2.0, mutated[key] * (1 + np.random.uniform(-0.7, 0.7))))
                
                elif key.startswith('development.'):
                    if 'rate' in key:
                        mutated[key] = max(0.0, min(1.0, mutated[key] * (1 + np.random.uniform(-0.7, 0.7))))
        
        return mutated
    
    def _selection_probabilities(self, scores: List[float]) -> List[float]:
        """Calculate selection probabilities for genetic algorithm."""
        max_score = max(scores)
        normalized_scores = [s / max_score for s in scores]
        total = sum(normalized_scores)
        
        probabilities = [s / total for s in normalized_scores]
        
        return probabilities
    
    def _calculate_gradient(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """Calculate gradient for gradient-based optimization."""
        gradient = {}
        epsilon = 1e-6
        
        for key in config:
            # Perturb parameter
            original_value = config[key]
            config[key] = original_value + epsilon
            
            # Calculate gradient
            score_plus = self._evaluate_config(config)
            
            config[key] = original_value - epsilon
            score_minus = self._evaluate_config(config)
            
            # Restore original value
            config[key] = original_value
            
            # Calculate gradient
            gradient[key] = (score_plus - score_minus) / (2 * epsilon)
        
        return gradient
    
    def _evaluate_config(self, config: Dict[str, Any]) -> float:
        """Evaluate a configuration (placeholder for actual evaluation)."""
        # This is a placeholder - in practice, this would run a simulation
        # For now, return a simple heuristic score
        score = 0.0
        
        # Add contributions from different aspects
        if 'brain.neuron_count' in config:
            neurons = config['brain.neuron_count']
            score += min(neurons / 1000.0, 1.0) * 0.3
        
        if 'brain.synapse_density' in config:
            density = config['brain.synapse_density']
            score += min(density / 0.3, 1.0) * 0.2
        
        if 'plasticity.stdp.learning_rate' in config:
            ltp = config['plasticity.stdp.learning_rate']
            score += min(ltp / 0.01, 1.0) * 0.2
        
        if 'neuromod.dopamine.scale' in config:
            dopamine = config['neuromod.dopamine.scale']
            score += min(dopamine / 2.0, 1.0) * 0.15
        
        if 'development.initial_plasticity' in config:
            init_plasticity = config['development.initial_plasticity']
            score += min(init_plasticity / 2.0, 1.0) * 0.15
        
        return score
if __name__ == "__main__":
    # Test the optimizer
    print("Testing ConfigurationOptimizer...")
    
    optimizer = ConfigurationOptimizer()
    
    # Test learning optimization
    print("\n1. Testing learning optimization:")
    learning_result = optimizer.optimize_for_learning(target_performance=0.8, max_iterations=50)
    print(f"   Best score: {learning_result.score:.4f}")
    print(f"   Best config: {learning_result.config}")
    
    # Test stability optimization
    print("\n2. Testing stability optimization:")
    
    def stability_metric(config):
        # Simple stability metric based on configuration parameters
        stability = 0.0
        if 'brain.neuron_count' in config:
            stability += min(1.0 - abs(config['brain.neuron_count'] - 1000) / 1000.0, 0.0)
        if 'plasticity.stdp.learning_rate' in config:
            stability += min(config['plasticity.stdp.learning_rate'] / 0.01, 1.0) * 0.3
        return stability
    
    stability_result = optimizer.optimize_for_stability(
        metric=stability_metric,
        max_iterations=30,
        mutation_rate=0.2
    )
    print(f"   Best score: {stability_result.score:.4f}")
    print(f"   Best config: {stability_result.config}")
    
    # Test performance optimization
    print("\n3. Testing performance optimization:")
    performance_result = optimizer.optimize_for_performance(max_iterations=40)
    print(f"   Best score: {performance_result.score:.4f}")
    print(f"   Best config: {performance_result.config}")
    
    print("\nConfiguration optimizer test completed successfully!")