# AdvancedConfigurator - Advanced configuration management

This module provides advanced configuration management tools for NLM,
including parameter optimization, validation, and advanced settings.
"""

import numpy as np
from typing import Optional, List, Dict, Any, Union
from dataclasses import dataclass, field
import json

from .enhanced_api import NLMSystem

@dataclass
class AdvancedConfig:
    """Advanced configuration settings for NLM"""
    # Brain configuration
    neuron_count: int = 1000
    synapse_density: float = 0.1
    connection_probability: float = 0.05
    initial_weight_mean: float = 0.5
    initial_weight_std: float = 0.1
    
    # Neuron parameters
    v_thresh: float = -50.0
    v_rest: float = -70.0
    v_reset: float = -75.0
    tau_mem: float = 20.0
    tau_ref: float = 2.0
    
    # Plasticity configuration
    stdp_enable: bool = True
    stdp_learning_rate: float = 0.001
    stdp_tau_plus: float = 20.0
    stdp_tau_minus: float = 20.0
    hebbian_enable: bool = True
    hebbian_learning_rate: float = 0.01
    structural_plasticity_enable: bool = True
    synaptogenesis_rate: float = 0.0001
    pruning_rate: float = 0.00001
    
    # Neuromodulation
    dopamine_enable: bool = True
    dopamine_scale: float = 1.0
    curiosity_enable: bool = True
    curiosity_weight: float = 0.5
    novelty_enable: bool = True
    prediction_error_enable: bool = True
    
    # Memory systems
    working_memory_capacity: int = 100
    episodic_memory_max_episodes: int = 1000
    associative_memory_strength: float = 0.8
    
    # Prediction system
    prediction_enabled: bool = True
    prediction_horizon: int = 10
    confidence_threshold: float = 0.5
    
    # Cognitive systems
    planning_depth: int = 5
    concept_formation_threshold: float = 0.7
    attention_inhibition_strength: float = 0.5
    attention_excitation_strength: float = 1.5
    
    # Development
    developmental_stages: List[str] = field(default_factory=lambda: ["Initial", "CriticalPeriod", "Maturation", "Adult"])
    initial_plasticity: float = 1.0
    critical_period_plasticity: float = 0.8
    maturation_plasticity: float = 0.5
    adult_plasticity: float = 0.2
    
    # Simulation parameters
    simulation_timestep: float = 0.001
    max_steps: int = 10000
    energy_decay_rate: float = 0.01
    max_energy: float = 100.0
    
    # Advanced options
    checkpoint_enabled: bool = True
    checkpoint_interval: int = 1000
    replay_enabled: bool = True
    replay_interval: int = 100
    consolidation_enabled: bool = True
    consolidation_threshold: float = 0.3

class ParameterSpace:
    """Parameter space for optimization"""
    
    def __init__(self):
        self.parameters = {}
        self.bounds = {}
        self.distributions = {}
    
    def add_parameter(self, name: str, value: Any, bounds: Tuple[float, float] = None,
                     distribution: str = "uniform", min_value: float = None, max_value: float = None):
        """Add a parameter to the parameter space."""
        self.parameters[name] = value
        
        if bounds:
            self.bounds[name] = bounds
        elif min_value is not None and max_value is not None:
            self.bounds[name] = (min_value, max_value)
        else:
            # Default bounds based on parameter type
            if isinstance(value, int):
                self.bounds[name] = (0, 1000)
            elif isinstance(value, float):
                self.bounds[name] = (0.0, 1.0)
            elif isinstance(value, bool):
                self.bounds[name] = (0.0, 1.0)
            else:
                raise ValueError(f"Cannot determine bounds for parameter {name}")
        
        self.distributions[name] = distribution
    
    def sample_random_configuration(self) -> Dict[str, Any]:
        """Sample a random configuration from the parameter space."""
        config = {}
        
        for name, value in self.parameters.items():
            if name in self.bounds:
                min_val, max_val = self.bounds[name]
                if self.distributions[name] == "uniform":
                    config[name] = np.random.uniform(min_val, max_val)
                elif self.distributions[name] == "loguniform":
                    config[name] = np.exp(np.random.uniform(np.log(min_val), np.log(max_val)))
                elif self.distributions[name] == "normal":
                    mean = (min_val + max_val) / 2
                    std = (max_val - min_val) / 6
                    config[name] = np.random.normal(mean, std)
                    # Clip to bounds
                    config[name] = np.clip(config[name], min_val, max_val)
                else:
                    config[name] = value
            else:
                config[name] = value
        
        return config

class AdvancedConfigurator:
    """
    Advanced configuration management for NLM systems.
    
    This class provides tools for:
    - Parameter optimization and tuning
    - Configuration validation and compatibility checking
    - Preset configurations for common use cases
    - Configuration comparison and merging
    - Automated parameter search
    
    Examples:
        >>> # Create advanced configurator
        >>> configurator = nlm.AdvancedConfigurator()
        >>> 
        >>> # Define parameter space
        >>> configurator.add_parameter("brain.neuron_count", 500, (100, 2000), "uniform")
        >>> configurator.add_parameter("learning.plasticity_rate", 0.1, (0.01, 0.5), "uniform")
        >>> configurator.add_parameter("neuromodulation.curiosity_weight", 0.5, (0.1, 1.0), "uniform")
        >>> 
        >>> # Sample random configuration
        >>> config = configurator.sample_random_configuration()
        >>> 
        >>> # Optimize configuration
        >>> optimized_config = configurator.optimize_configuration(
        ...     brain_function, 
        ...     initial_config,
        ...     optimization_method="bayesian"
        ... )
    """
    
    def __init__(self, config: Optional[AdvancedConfig] = None):
        """
        Initialize the advanced configurator.
        
        Args:
            config: Optional initial configuration
        """
        self.config = config or AdvancedConfig()
        self.parameter_space = ParameterSpace()
        self.optimization_history = []
    
    def add_parameter(self, name: str, value: Any, bounds: Tuple[float, float] = None,
                     distribution: str = "uniform", min_value: float = None, max_value: float = None):
        """
        Add a parameter to the configurator.
        
        Args:
            name: Parameter name
            value: Default value
            bounds: Tuple of (min, max) for the parameter
            distribution: Distribution type ('uniform', 'loguniform', 'normal')
            min_value: Minimum value (alternative to bounds)
            max_value: Maximum value (alternative to bounds)
        """
        self.parameter_space.add_parameter(name, value, bounds, distribution, min_value, max_value)
    
    def create_advanced_config(self, name: str, description: str = "") -> AdvancedConfig:
        """
        Create an advanced configuration from the current parameter space.
        
        Args:
            name: Name for the configuration
            description: Description of the configuration
            
        Returns:
            AdvancedConfig instance
        """
        try:
            # Create a new config instance
            advanced_config = AdvancedConfig()
            
            # Copy current configuration values
            for param_name, param_value in self.parameter_space.parameters.items():
                # Set nested parameters based on dot notation
                keys = param_name.split('.')
                target = advanced_config
                
                for key in keys[:-1]:
                    if not hasattr(target, key):
                        setattr(target, key, type(None)())
                    target = getattr(target, key)
                
                setattr(target, keys[-1], param_value)
            
            return advanced_config
            
        except Exception as e:
            raise Exception(f"Failed to create advanced config: {e}")
    
    def optimize_configuration(self, objective_function: Callable[[AdvancedConfig], float],
                              initial_config: AdvancedConfig,
                              optimization_method: str = "random_search",
                              iterations: int = 100,
                              population_size: int = 10) -> AdvancedConfig:
        """
        Optimize configuration using the specified method.
        
        Args:
            objective_function: Function that takes a config and returns a score
            initial_config: Initial configuration
            optimization_method: Optimization method ('random_search', 'bayesian', 'grid_search')
            iterations: Number of iterations to run
            population_size: Population size for evolutionary methods
            
        Returns:
            Optimized configuration
        """
        try:
            if optimization_method == "random_search":
                return self._random_search_optimization(objective_function, initial_config, iterations)
            elif optimization_method == "bayesian":
                return self._bayesian_optimization(objective_function, initial_config, iterations)
            elif optimization_method == "grid_search":
                return self._grid_search_optimization(objective_function, initial_config, iterations)
            else:
                raise ValueError(f"Unknown optimization method: {optimization_method}")
                
        except Exception as e:
            raise Exception(f"Configuration optimization failed: {e}")
    
    def validate_configuration(self, config: AdvancedConfig) -> Tuple[bool, List[str]]:
        """
        Validate configuration for compatibility and feasibility.
        
        Args:
            config: Configuration to validate
            
        Returns:
            Tuple of (is_valid, list_of_violations)
        """
        try:
            violations = []
            
            # Check basic constraints
            if config.neuron_count <= 0:
                violations.append("Neuron count must be positive")
            if config.synapse_density <= 0 or config.synapse_density > 1:
                violations.append("Synapse density must be between 0 and 1")
            if config.connection_probability < 0 or config.connection_probability > 1:
                violations.append("Connection probability must be between 0 and 1")
            
            # Check neuron parameter consistency
            if config.v_thresh >= config.v_rest:
                violations.append("Threshold must be below resting potential")
            if config.v_reset > config.v_rest:
                violations.append("Reset potential must be at or below resting potential")
            if config.tau_ref <= 0:
                violations.append("Refractory period must be positive")
            
            # Check plasticity configuration
            if config.stdp_enable and config.stdp_learning_rate <= 0:
                violations.append("STDP learning rate must be positive when enabled")
            if config.hebbian_enable and config.hebbian_learning_rate <= 0:
                violations.append("Hebbian learning rate must be positive when enabled")
            
            # Check neuromodulation configuration
            if config.dopamine_enable and config.dopamine_scale < 0:
                violations.append("Dopamine scale must be non-negative when enabled")
            if config.curiosity_enable and config.curiosity_weight < 0:
                violations.append("Curiosity weight must be non-negative when enabled")
            
            # Check memory configuration
            if config.working_memory_capacity <= 0:
                violations.append("Working memory capacity must be positive")
            if config.episodic_memory_max_episodes <= 0:
                violations.append("Episodic memory max episodes must be positive")
            
            # Check simulation configuration
            if config.simulation_timestep <= 0:
                violations.append("Simulation timestep must be positive")
            if config.max_steps <= 0:
                violations.append("Max steps must be positive")
            
            return (len(violations) == 0, violations)
            
        except Exception as e:
            return (False, [f"Validation failed: {e}"])
    
    def create_preset_configurations(self) -> Dict[str, AdvancedConfig]:
        """
        Create predefined preset configurations for common use cases.
        
        Returns:
            Dictionary of preset configurations
        """
        presets = {}
        
        # Basic configuration
        basic_config = AdvancedConfig()
        basic_config.name = "basic"
        basic_config.neuron_count = 500
        basic_config.synapse_density = 0.1
        basic_config.connection_probability = 0.05
        basic_config.stdp_enable = True
        basic_config.hebbian_enable = True
        basic_config.structural_plasticity_enable = False
        basic_config.curiosity_enable = False
        basic_config.prediction_enabled = False
        presets['basic'] = basic_config
        
        # Learning configuration
        learning_config = AdvancedConfig()
        learning_config.name = "learning"
        learning_config.neuron_count = 1000
        learning_config.synapse_density = 0.15
        learning_config.connection_probability = 0.1
        learning_config.stdp_enable = True
        learning_config.hebbian_enable = True
        learning_config.structural_plasticity_enable = True
        learning_config.curiosity_enable = True
        learning_config.prediction_enabled = True
        learning_config.planning_depth = 3
        learning_config.concept_formation_threshold = 0.8
        presets['learning'] = learning_config
        
        # Exploration configuration
        exploration_config = AdvancedConfig()
        exploration_config.name = "exploration"
        exploration_config.neuron_count = 300
        exploration_config.synapse_density = 0.08
        exploration_config.connection_probability = 0.03
        exploration_config.stdp_enable = True
        learning_config.hebbian_enable = True
        exploration_config.structural_plasticity_enable = True
        exploration_config.curiosity_enable = True
        exploration_config.prediction_enabled = True
        exploration_config.planning_depth = 5
        exploration_config.curiosity_weight = 0.8
        presets['exploration'] = exploration_config
        
        # Stable configuration
        stable_config = AdvancedConfig()
        stable_config.name = "stable"
        stable_config.neuron_count = 200
        stable_config.synapse_density = 0.05
        stable_config.connection_probability = 0.02
        stable_config.stdp_enable = True
        learning_config.hebbian_enable = True
        stable_config.structural_plasticity_enable = True
        stable_config.curiosity_enable = False
        stable_config.prediction_enabled = True
        stable_config.initial_plasticity = 0.8
        stable_config.adult_plasticity = 0.1
        presets['stable'] = stable_config
        
        return presets
    
    def compare_configurations(self, config1: AdvancedConfig, config2: AdvancedConfig) -> Dict[str, Any]:
        """
        Compare two configurations and return differences.
        
        Args:
            config1: First configuration
            config2: Second configuration
            
        Returns:
            Dictionary containing comparison results
        """
        try:
            differences = {}
            
            # Compare all attributes
            config1_dict = vars(config1)
            config2_dict = vars(config2)
            
            all_keys = set(config1_dict.keys()) | set(config2_dict.keys())
            
            for key in all_keys:
                val1 = getattr(config1, key) if hasattr(config1, key) else None
                val2 = getattr(config2, key) if hasattr(config2, key) else None
                
                if val1 != val2:
                    differences[key] = {
                        'config1': val1,
                        'config2': val2,
                        'difference': val2 - val1 if isinstance(val1, (int, float)) and isinstance(val2, (int, float)) else None
                    }
            
            return {
                'differences': differences,
                'total_differences': len(differences),
                'similarity_score': 1.0 - (len(differences) / len(all_keys) if all_keys else 1.0)
            }
            
        except Exception as e:
            return {'error': f"Comparison failed: {e}"}
    
    def merge_configurations(self, *configs: AdvancedConfig) -> AdvancedConfig:
        """
        Merge multiple configurations into one.
        
        Args:
            *configs: Configurations to merge
            
        Returns:
            Merged configuration
        """
        try:
            if not configs:
                return AdvancedConfig()
            
            # Start with the first config
            merged = AdvancedConfig()
            
            # Merge all configs
            for config in configs:
                for key, value in vars(config).items():
                    if value is not None:
                        setattr(merged, key, value)
            
            return merged
            
        except Exception as e:
            raise Exception(f"Configuration merging failed: {e}")
    
    def export_configuration(self, config: AdvancedConfig, filepath: str, 
                           format: str = "json") -> bool:
        """
        Export configuration to file.
        
        Args:
            config: Configuration to export
            filepath: Output file path
            format: Export format ('json', 'python', 'yaml')
            
        Returns:
            True if export successful, False otherwise
        """
        try:
            if format == "json":
                data = vars(config)
                with open(filepath, 'w') as f:
                    json.dump(data, f, indent=2)
                    
            elif format == "python":
                with open(filepath, 'w') as f:
                    f.write(f"""
# Advanced NLM Configuration
# Generated by AdvancedConfigurator

config = {{
""")
                    for key, value in vars(config).items():
                        if isinstance(value, dict):
                            f.write(f"    '{key}': {json.dumps(value)},\n")
                        elif isinstance(value, list):
                            f.write(f"    '{key}': {json.dumps(value)},\n")
                        else:
                            f.write(f"    '{key}': {repr(value)},\n")
                    f.write("}")
                    
            else:
                raise ValueError(f"Unsupported export format: {format}")
            
            return True
            
        except Exception as e:
            print(f"Failed to export configuration: {e}")
            return False
    
    def import_configuration(self, filepath: str, format: str = "json") -> AdvancedConfig:
        """
        Import configuration from file.
        
        Args:
            filepath: Input file path
            format: Import format ('json', 'python', 'yaml')
            
        Returns:
            Imported configuration
        """
        try:
            if format == "json":
                with open(filepath, 'r') as f:
                    data = json.load(f)
                
                config = AdvancedConfig()
                for key, value in data.items():
                    setattr(config, key, value)
                    
            elif format == "python":
                # Import from Python module
                import importlib.util
                spec = importlib.util.spec_from_file_location("imported_config", filepath)
                module = importlib.util.module_from_spec(spec)
                spec.loader.exec_module(module)
                
                config = module.config
                
            else:
                raise ValueError(f"Unsupported import format: {format}")
            
            return config
            
        except Exception as e:
            raise Exception(f"Failed to import configuration: {e}")
    
    def _random_search_optimization(self, objective_function: Callable[[AdvancedConfig], float],
                                    initial_config: AdvancedConfig, iterations: int) -> AdvancedConfig:
        """Random search optimization."""
        try:
            best_config = initial_config
            best_score = objective_function(initial_config)
            
            for i in range(iterations):
                # Generate random configuration
                random_config = AdvancedConfig()
                
                # Modify random parameters
                for param_name, param_value in self.parameter_space.parameters.items():
                    if param_name in self.parameter_space.bounds:
                        min_val, max_val = self.parameter_space.bounds[param_name]
                        modified_value = np.random.uniform(min_val, max_val)
                        
                        # Set the parameter (this would need to be implemented)
                        # For now, just create a new config with default values
                        pass
                
                # Evaluate objective function
                score = objective_function(random_config)
                
                # Update best if improved
                if score > best_score:
                    best_score = score
                    best_config = random_config
                
                # Store in history
                self.optimization_history.append({
                    'iteration': i,
                    'score': score,
                    'config': vars(random_config)
                })
            
            return best_config
            
        except Exception as e:
            raise Exception(f"Random search optimization failed: {e}")
    
    def _bayesian_optimization(self, objective_function: Callable[[AdvancedConfig], float],
                               initial_config: AdvancedConfig, iterations: int) -> AdvancedConfig:
        """Bayesian optimization."""
        try:
            # Placeholder for Bayesian optimization
            # This would typically use libraries like scikit-optimize or similar
            
            best_config = initial_config
            best_score = objective_function(initial_config)
            
            for i in range(iterations):
                # Generate candidate configuration using random sampling
                candidate_config = AdvancedConfig()
                
                # Evaluate objective function
                score = objective_function(candidate_config)
                
                # Update best if improved
                if score > best_score:
                    best_score = score
                    best_config = candidate_config
                
                # Store in history
                self.optimization_history.append({
                    'iteration': i,
                    'score': score,
                    'config': vars(candidate_config)
                })
            
            return best_config
            
        except Exception as e:
            raise Exception(f"Bayesian optimization failed: {e}")
    
    def _grid_search_optimization(self, objective_function: Callable[[AdvancedConfig], float],
                                   initial_config: AdvancedConfig, iterations: int) -> AdvancedConfig:
        """Grid search optimization."""
        try:
            # Placeholder for grid search optimization
            
            best_config = initial_config
            best_score = objective_function(initial_config)
            
            for i in range(min(iterations, 100)):  # Limit grid size
                # Generate grid point
                grid_config = AdvancedConfig()
                
                # Evaluate objective function
                score = objective_function(grid_config)
                
                # Update best if improved
                if score > best_score:
                    best_score = score
                    best_config = grid_config
                
                # Store in history
                self.optimization_history.append({
                    'iteration': i,
                    'score': score,
                    'config': vars(grid_config)
                })
            
            return best_config
            
        except Exception as e:
            raise Exception(f"Grid search optimization failed: {e}")