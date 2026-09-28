#!/usr/bin/env python3
"""
NLM Configuration Management System

Advanced configuration management and parameter tuning for NLM simulations.
This module provides sophisticated configuration loading, validation, 
parameter optimization, and simulation tuning capabilities.
"""

import json
import yaml
import os
import sys
import time
from typing import Dict, Any, List, Optional, Union, Callable
from dataclasses import dataclass, field
from enum import Enum
import pynlm

class ConfigScope(Enum):
    """Configuration scope levels."""
    GLOBAL = "global"
    BRAIN = "brain"
    NEUROMODULATION = "neuromodulation"
    PLASTICITY = "plasticity"
    MEMORY = "memory"
    DEVELOPMENT = "development"
    WORLD = "world"
    AGENT = "agent"

class ParameterType(Enum):
    """Parameter data types."""
    INTEGER = "integer"
    FLOAT = "float"
    BOOLEAN = "boolean"
    STRING = "string"
    ENUM = "enum"
    VECTOR = "vector"
    MATRIX = "matrix"

@dataclass
class ParameterConstraint:
    """Constraints for configuration parameters."""
    min_value: Optional[float] = None
    max_value: Optional[float] = None
    allowed_values: Optional[List[Any]] = None
    constraint_function: Optional[Callable[[Any], bool]] = None
    description: str = ""

@dataclass
class ConfigurationParameter:
    """Definition of a configuration parameter."""
    name: str
    type: ParameterType
    default_value: Any
    scope: ConfigScope
    description: str
    constraints: ParameterConstraint = field(default_factory=ParameterConstraint)
    is_required: bool = False
    unit: str = ""

class ParameterValidator:
    """Validates configuration parameters."""
    
    @staticmethod
    def validate_parameter(param: ConfigurationParameter, value: Any) -> List[str]:
        """Validate a parameter value."""
        errors = []
        
        # Type validation
        if not ParameterValidator._check_type(param, value):
            errors.append(f"Parameter '{param.name}' must be {param.type.value}, got {type(value).__name__}")
        
        # Constraints validation
        if not ParameterValidator._check_constraints(param, value):
            errors.append(f"Parameter '{param.name}' violates constraints")
        
        return errors
    
    @staticmethod
    def _check_type(param: ConfigurationParameter, value: Any) -> bool:
        """Check if value matches parameter type."""
        type_checks = {
            ParameterType.INTEGER: isinstance(value, int) and not isinstance(value, bool),
            ParameterType.FLOAT: isinstance(value, (int, float)),
            ParameterType.BOOLEAN: isinstance(value, bool),
            ParameterType.STRING: isinstance(value, str),
            ParameterType.ENUM: value in param.constraints.allowed_values if param.constraints.allowed_values else False,
            ParameterType.VECTOR: isinstance(value, list),
            ParameterType.MATRIX: isinstance(value, list) and all(isinstance(row, list) for row in value)
        }
        
        return type_checks.get(param.type, True)
    
    @staticmethod
    def _check_constraints(param: ConfigurationParameter, value: Any) -> bool:
        """Check if value satisfies constraints."""
        if not param.constraints:
            return True
        
        # Check min/max values for numeric types
        if param.type in [ParameterType.INTEGER, ParameterType.FLOAT]:
            if param.constraints.min_value is not None and value < param.constraints.min_value:
                return False
            if param.constraints.max_value is not None and value > param.constraints.max_value:
                return False
        
        # Check allowed values
        if param.constraints.allowed_values is not None and value not in param.constraints.allowed_values:
            return False
        
        # Check custom constraint function
        if param.constraints.constraint_function is not None:
            if not param.constraints.constraint_function(value):
                return False
        
        return True

class ParameterRegistry:
    """Registry of all configuration parameters."""
    
    def __init__(self):
        self.parameters: Dict[str, ConfigurationParameter] = {}
        self.parameter_groups: Dict[ConfigScope, List[str]] = {}
    
    def register_parameter(self, param: ConfigurationParameter):
        """Register a configuration parameter."""
        self.parameters[param.name] = param
        
        if param.scope not in self.parameter_groups:
            self.parameter_groups[param.scope] = []
        self.parameter_groups[param.scope].append(param.name)
    
    def get_parameter(self, name: str) -> Optional[ConfigurationParameter]:
        """Get a parameter by name."""
        return self.parameters.get(name)
    
    def get_parameters_by_scope(self, scope: ConfigScope) -> List[ConfigurationParameter]:
        """Get all parameters for a specific scope."""
        return [self.parameters[name] for name in self.parameter_groups.get(scope, [])]
    
    def get_all_parameters(self) -> List[ConfigurationParameter]:
        """Get all registered parameters."""
        return list(self.parameters.values())
    
    def get_required_parameters(self) -> List[ConfigurationParameter]:
        """Get all required parameters."""
        return [p for p in self.parameters.values() if p.is_required]

class AdvancedConfigBuilder:
    """Builds and manages advanced configurations."""
    
    def __init__(self, registry: ParameterRegistry):
        self.registry = registry
        self.config_history: List[Dict[str, Any]] = []
    
    def create_configuration(self, name: str, values: Dict[str, Any]) -> Dict[str, Any]:
        """Create a configuration from parameter values."""
        # Validate all parameters
        errors = []
        for param_name, value in values.items():
            param = self.registry.get_parameter(param_name)
            if param:
                param_errors = ParameterValidator.validate_parameter(param, value)
                errors.extend(param_errors)
            else:
                errors.append(f"Unknown parameter: {param_name}")
        
        if errors:
            raise ValueError(f"Configuration validation failed: {errors}")
        
        # Create configuration
        config = {
            'name': name,
            'created_at': time.time(),
            'parameters': values,
            'scope': self._determine_scope(values)
        }
        
        self.config_history.append(config)
        return config
    
    def _determine_scope(self, values: Dict[str, Any]) -> ConfigScope:
        """Determine the primary scope of the configuration."""
        # Simple heuristic based on parameter names
        scope_indicators = {
            ConfigScope.BRAIN: ['brain.', 'neuron', 'synapse', 'v_', 'tau_'],
            ConfigScope.NEUROMODULATION: ['neuromod', 'dopamine', 'curiosity', 'novelty'],
            ConfigScope.PLASTICITY: ['plasticity', 'stdp', 'hebbian', 'structural'],
            ConfigScope.DEVELOPMENT: ['development', 'developmental', 'plasticity'],
            ConfigScope.WORLD: ['world', 'width', 'height', 'vision'],
            ConfigScope.AGENT: ['agent', 'curiosity', 'novelty', 'reward']
        }
        
        for param_name in values.keys():
            for scope, indicators in scope_indicators.items():
                if any(indicator in param_name.lower() for indicator in indicators):
                    return scope
        
        return ConfigScope.GLOBAL

class ConfigurationManager:
    """Main configuration manager for NLM simulations."""
    
    def __init__(self, config_file: Optional[str] = None):
        self.registry = ParameterRegistry()
        self.builder = AdvancedConfigBuilder(self.registry)
        self.current_config = None
        self.configs: Dict[str, Dict[str, Any]] = {}
        
        # Initialize parameter registry
        self._initialize_parameters()
        
        # Load configuration file if provided
        if config_file:
            self.load_configuration(config_file)
    
    def _initialize_parameters(self):
        """Initialize the parameter registry with all NLM parameters."""
        
        # Brain parameters
        self.registry.register_parameter(ConfigurationParameter(
            name="brain.neuron_count",
            type=ParameterType.INTEGER,
            default_value=1000,
            scope=ConfigScope.BRAIN,
            description="Total number of neurons in the brain",
            constraints=ParameterConstraint(min_value=100, max_value=10000),
            is_required=True,
            unit="neurons"
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="brain.synapse_density",
            type=ParameterType.FLOAT,
            default_value=0.1,
            scope=ConfigScope.BRAIN,
            description="Synapse connectivity density (0.0-1.0)",
            constraints=ParameterConstraint(min_value=0.0, max_value=1.0),
            unit="density"
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="brain.v_thresh",
            type=ParameterType.FLOAT,
            default_value=-50.0,
            scope=ConfigScope.BRAIN,
            description="Neuron threshold potential (mV)",
            constraints=ParameterConstraint(min_value=-100.0, max_value=0.0),
            unit="mV"
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="brain.v_rest",
            type=ParameterType.FLOAT,
            default_value=-70.0,
            scope=ConfigScope.BRAIN,
            description="Resting potential (mV)",
            constraints=ParameterConstraint(min_value=-80.0, max_value=0.0),
            unit="mV"
        ))
        
        # Plasticity parameters
        self.registry.register_parameter(ConfigurationParameter(
            name="plasticity.stdp.enable",
            type=ParameterType.BOOLEAN,
            default_value=True,
            scope=ConfigScope.PLASTICITY,
            description="Enable Spike-Timing-Dependent Plasticity",
            unit=""
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="plasticity.stdp.learning_rate",
            type=ParameterType.FLOAT,
            default_value=0.001,
            scope=ConfigScope.PLASTICITY,
            description="STDP learning rate",
            constraints=ParameterConstraint(min_value=0.0, max_value=0.1),
            unit="rate"
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="plasticity.hebbian.enable",
            type=ParameterType.BOOLEAN,
            default_value=True,
            scope=ConfigScope.PLASTICITY,
            description="Enable Hebbian learning",
            unit=""
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="plasticity.structural.enable",
            type=ParameterType.BOOLEAN,
            default_value=True,
            scope=ConfigScope.PLASTICITY,
            description="Enable structural plasticity",
            unit=""
        ))
        
        # Neuromodulation parameters
        self.registry.register_parameter(ConfigurationParameter(
            name="neuromod.dopamine.scale",
            type=ParameterType.FLOAT,
            default_value=1.0,
            scope=ConfigScope.NEUROMODULATION,
            description="Dopamine modulation scale",
            constraints=ParameterConstraint(min_value=0.0, max_value=5.0),
            unit="scale"
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="neuromod.curiosity.enable",
            type=ParameterType.BOOLEAN,
            default_value=True,
            scope=ConfigScope.NEUROMODULATION,
            description="Enable curiosity-driven exploration",
            unit=""
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="neuromod.novelty.enable",
            type=ParameterType.BOOLEAN,
            default_value=True,
            scope=ConfigScope.NEUROMODULATION,
            description="Enable novelty detection",
            unit=""
        ))
        
        # World parameters
        self.registry.register_parameter(ConfigurationParameter(
            name="world.width",
            type=ParameterType.INTEGER,
            default_value=20,
            scope=ConfigScope.WORLD,
            description="World width in cells",
            constraints=ParameterConstraint(min_value=5, max_value=100),
            unit="cells"
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="world.height",
            type=ParameterType.INTEGER,
            default_value=20,
            scope=ConfigScope.WORLD,
            description="World height in cells",
            constraints=ParameterConstraint(min_value=5, max_value=100),
            unit="cells"
        ))
        
        self.registry.register_parameter(ConfigurationParameter(
            name="world.vision_width",
            type=ParameterType.INTEGER,
            default_value=8,
            scope=ConfigScope.WORLD,
            description="Vision field width",
            constraints=ParameterConstraint(min_value=1, max_value=32),
            unit="cells"
        ))
        
        # Agent parameters
        self.registry.register_parameter(ConfigurationParameter(
            name="agent.max_energy",
            type=ParameterType.FLOAT,
            default_value=100.0,
            scope=ConfigScope.AGENT,
            description="Maximum agent energy",
            constraints=ParameterConstraint(min_value=0.0, max_value=1000.0),
            unit="energy"
        ))
        
        # Development parameters
        self.registry.register_parameter(ConfigurationParameter(
            name="development.plasticity_rate",
            type=ParameterType.FLOAT,
            default_value=1.0,
            scope=ConfigScope.DEVELOPMENT,
            description="Plasticity rate during development",
            constraints=ParameterConstraint(min_value=0.0, max_value=10.0),
            unit="rate"
        ))
    
    def load_configuration(self, filepath: str) -> Dict[str, Any]:
        """Load configuration from a file."""
        if not os.path.exists(filepath):
            raise FileNotFoundError(f"Configuration file not found: {filepath}")
        
        # Detect file format
        if filepath.endswith('.json'):
            with open(filepath, 'r') as f:
                config_data = json.load(f)
        elif filepath.endswith(('.yaml', '.yml')):
            with open(filepath, 'r') as f:
                config_data = yaml.safe_load(f)
        else:
            raise ValueError(f"Unsupported configuration file format: {filepath}")
        
        # Convert to our format
        name = config_data.get('name', 'loaded_config')
        parameters = config_data.get('parameters', config_data)
        
        config = self.builder.create_configuration(name, parameters)
        self.configs[name] = config['parameters']
        self.current_config = config['parameters']
        
        return self.current_config
    
    def save_configuration(self, filepath: str, config_name: str = None):
        """Save configuration to a file."""
        if config_name is None:
            config_name = f"config_{int(time.time())}"
        
        if config_name not in self.configs:
            raise ValueError(f"Configuration '{config_name}' not found")
        
        config_data = {
            'name': config_name,
            'created_at': time.time(),
            'parameters': self.configs[config_name]
        }
        
        # Determine file format
        if filepath.endswith('.json'):
            with open(filepath, 'w') as f:
                json.dump(config_data, f, indent=2)
        elif filepath.endswith(('.yaml', '.yml')):
            with open(filepath, 'w') as f:
                yaml.dump(config_data, f, default_flow_style=False)
        else:
            # Default to JSON
            json_path = filepath + '.json'
            with open(json_path, 'w') as f:
                json.dump(config_data, f, indent=2)
            print(f"Configuration saved as {json_path}")
    
    def create_brain_from_config(self, config_name: str = None) -> pynlm.Brain:
        """Create a brain using the specified configuration."""
        if config_name is None:
            config_name = 'default'
        
        config = self.configs.get(config_name, {})
        
        # Create default configuration
        pyn_config = pynlm.createDefaultConfig()
        
        # Apply parameters based on scope
        for param_name, value in config.items():
            if param_name.startswith('brain.'):
                brain_param = param_name.replace('brain.', '')
                # Handle different brain parameters
                if brain_param == 'neuron_count':
                    # Note: This is a simplified approach - actual config may not support this
                    pass
            elif param_name.startswith('plasticity.'):
                plasticity_param = param_name.replace('plasticity.', '')
                # Handle plasticity parameters
                pass
            elif param_name.startswith('neuromod.'):
                neuromod_param = param_name.replace('neuromod.', '')
                # Handle neuromodulation parameters
                pass
            elif param_name.startswith('world.'):
                world_param = param_name.replace('world.', '')
                # Handle world parameters
                pass
        
        # Create and return brain
        brain = pynlm.createBrain(pyn_config)
        brain.initialize()
        return brain
    
    def get_configuration(self, name: str = None) -> Dict[str, Any]:
        """Get configuration by name."""
        if name is None:
            return self.current_config
        return self.configs.get(name, {})
    
    def list_configurations(self) -> List[str]:
        """List all saved configurations."""
        return list(self.configs.keys())
    
    def create_parameter_tuning_experiment(self, base_config_name: str, 
                                           parameter_ranges: Dict[str, List[Any]],
                                           num_trials: int = 10) -> List[Dict[str, Any]]:
        """Create parameter tuning experiment using random sampling."""
        if base_config_name not in self.configs:
            raise ValueError(f"Base configuration '{base_config_name}' not found")
        
        base_config = self.configs[base_config_name]
        experiments = []
        
        for trial in range(num_trials):
            # Create random variation of base configuration
            trial_config = base_config.copy()
            
            for param_name, value_range in parameter_ranges.items():
                if param_name in trial_config:
                    trial_config[param_name] = random.choice(value_range)
                else:
                    # Add new parameter
                    trial_config[param_name] = random.choice(value_range)
            
            # Create experiment configuration
            experiment_name = f"{base_config_name}_trial_{trial}"
            experiment_config = self.builder.create_configuration(experiment_name, trial_config)
            
            experiments.append({
                'name': experiment_name,
                'config': trial_config,
                'parameters': experiment_config['parameters']
            })
        
        return experiments
    
    def get_parameter_validation_report(self) -> Dict[str, Any]:
        """Get validation report for all parameters."""
        report = {
            'total_parameters': len(self.registry.get_all_parameters()),
            'parameters_by_scope': {},
            'validation_status': {}
        }
        
        # Group by scope
        for scope in ConfigScope:
            params = self.registry.get_parameters_by_scope(scope)
            report['parameters_by_scope'][scope.value] = len(params)
        
        # Validate parameter types
        validation_issues = []
        for param in self.registry.get_all_parameters():
            if param.type == ParameterType.ENUM and param.constraints.allowed_values:
                # Check if all allowed values are valid for enum
                validation_issues.append(f"Parameter '{param.name}' has enum values: {param.constraints.allowed_values}")
        
        report['validation_issues'] = validation_issues
        
        return report

class ParameterOptimizer:
    """Optimize configuration parameters for specific performance goals."""
    
    def __init__(self, config_manager: ConfigurationManager):
        self.config_manager = config_manager
    
    def optimize_for_performance(self, performance_metric: str,
                                target_value: float,
                                initial_config: Dict[str, Any],
                                iterations: int = 100) -> Dict[str, Any]:
        """Optimize configuration for a specific performance metric."""
        
        # Simple random search optimization
        best_config = initial_config.copy()
        best_score = float('inf')
        
        for iteration in range(iterations):
            # Generate random variation
            trial_config = self._random_variation(initial_config, 0.1)
            
            # Run simulation (simplified - would actually run real simulation)
            # For demonstration, we'll use a mock performance calculation
            score = self._mock_performance(trial_config, performance_metric, target_value)
            
            if score < best_score:
                best_score = score
                best_config = trial_config
        
        return {
            'optimized_config': best_config,
            'best_score': best_score,
            'target_value': target_value,
            'performance_metric': performance_metric,
            'iterations': iterations
        }
    
    def _random_variation(self, config: Dict[str, Any], variation_range: float) -> Dict[str, Any]:
        """Generate random variation of configuration."""
        import random
        
        varied_config = config.copy()
        
        for param_name, value in config.items():
            if isinstance(value, (int, float)):
                # Perturb numeric values
                varied_config[param_name] = value * (1 + random.uniform(-variation_range, variation_range))
            elif isinstance(value, bool):
                # Randomly flip boolean values
                varied_config[param_name] = random.choice([True, False])
        
        return varied_config
    
    def _mock_performance(self, config: Dict[str, Any], metric: str, target: float) -> float:
        """Mock performance calculation for demonstration."""
        # This would be replaced with actual simulation performance evaluation
        import random
        
        # Simple mock based on some config parameters
        score = 0.0
        
        if metric == 'learning_speed':
            # Higher learning rate -> better score (but with diminishing returns)
            plasticity_rate = config.get('plasticity.stdp.learning_rate', 0.001)
            score = abs(plasticity_rate - target) * 1000
        
        elif metric == 'firing_rate':
            # Target firing rate around 2-5 Hz
            neuron_count = config.get('brain.neuron_count', 1000)
            expected = 3.0
            score = abs(neuron_count / 1000 * expected - target) * 100
        
        elif metric == 'exploration':
            # Higher curiosity -> better exploration
            curiosity = 1.0 if config.get('neuromod.curiosity.enable', True) else 0.0
            score = abs(curiosity - 1.0) * 100
        
        # Add some randomness
        score += random.uniform(0, 50)
        
        return score

class ConfigurationVisualizer:
    """Visualize configuration data and optimization results."""
    
    def __init__(self):
        try:
            import matplotlib.pyplot as plt
            import seaborn as sns
            self.plt = plt
            self.sns = sns
            self.visualization_available = True
        except ImportError:
            self.visualization_available = False
            print("Warning: matplotlib/seaborn not available for visualization")
    
    def plot_parameter_distribution(self, config_manager: ConfigurationManager,
                                   parameter_name: str, output_file: str = None):
        """Plot distribution of a parameter across configurations."""
        if not self.visualization_available:
            return
        
        # Collect parameter values from all configurations
        values = []
        for config_name, config in config_manager.configs.items():
            if parameter_name in config:
                values.append(config[parameter_name])
        
        if not values:
            print(f"No values found for parameter '{parameter_name}'")
            return
        
        # Create plot
        self.plt.figure(figsize=(10, 6))
        self.sns.histplot(values, kde=True)
        self.plt.title(f'Distribution of {parameter_name}')
        self.plt.xlabel(parameter_name)
        self.plt.ylabel('Frequency')
        
        if output_file:
            self.plt.savefig(output_file, dpi=300, bbox_inches='tight')
        
        self.plt.show()
    
    def plot_optimization_results(self, optimization_result: Dict[str, Any], output_file: str = None):
        """Plot optimization results."""
        if not self.visualization_available:
            return
        
        fig, axes = self.plt.subplots(2, 2, figsize=(12, 10))
        fig.suptitle(f"Optimization Results: {optimization_result['performance_metric']}", fontsize=16)
        
        # Parameter history (simplified - would need actual iteration history)
        axes[0, 0].plot([1, 2, 3, 4, 5], [100, 80, 60, 40, 20], 'o-')
        axes[0, 0].set_title('Mock Iteration History')
        axes[0, 0].set_xlabel('Iteration')
        axes[0, 0].set_ylabel('Performance Score')
        
        # Parameter value changes
        if 'optimized_config' in optimization_result:
            param_names = list(optimization_result['optimized_config'].keys())
            param_values = list(optimization_result['optimized_config'].values())
            
            axes[0, 1].barh(param_names[:5], param_values[:5])  # Limit to 5 parameters
            axes[0, 1].set_title('Optimized Parameter Values')
        
        # Score distribution
        axes[1, 0].hist([20, 40, 60, 80, 100], bins=10, alpha=0.7, edgecolor='black')
        axes[1, 0].set_title('Score Distribution')
        axes[1, 0].set_xlabel('Score')
        axes[1, 0].set_ylabel('Frequency')
        
        # Convergence plot
        iterations = list(range(1, 21))
        scores = [100 - i * 4 for i in iterations]  # Mock decreasing scores
        axes[1, 1].plot(iterations, scores, 'r-', linewidth=2)
        axes[1, 1].set_title('Convergence Progress')
        axes[1, 1].set_xlabel('Iteration')
        axes[1, 1].set_ylabel('Best Score')
        axes[1, 1].grid(True, alpha=0.3)
        
        self.plt.tight_layout()
        
        if output_file:
            self.plt.savefig(output_file, dpi=300, bbox_inches='tight')
        
        self.plt.show()
    
    def generate_configuration_report(self, config_manager: ConfigurationManager, 
                                     output_file: str = None) -> str:
        """Generate comprehensive configuration report."""
        report = """# NLM Configuration Report

## Configuration Overview
"""
        
        report += f"Total Configurations: {len(config_manager.configs)}\n"
        report += f"Total Parameters: {len(config_manager.registry.get_all_parameters())}\n\n"
        
        # Parameter breakdown by scope
        report += "## Parameter Breakdown by Scope\n\n"
        for scope in ConfigScope:
            params = config_manager.registry.get_parameters_by_scope(scope)
            if params:
                report += f"### {scope.value.upper()}\n"
                for param in params:
                    report += f"- **{param.name}**: {param.description} (default: {param.default_value})\n"
                report += "\n"
        
        # Current configuration
        if config_manager.current_config:
            report += "## Current Configuration\n\n"
            for param_name, value in config_manager.current_config.items():
                param = config_manager.registry.get_parameter(param_name)
                if param:
                    report += f"- **{param.name}**: {value} {param.unit}\n"
                else:
                    report += f"- **{param_name}**: {value}\n"
        
        # Validation report
        validation_report = config_manager.get_parameter_validation_report()
        report += "\n## Parameter Validation Report\n\n"
        report += f"Total Parameters Validated: {validation_report['total_parameters']}\n"
        report += "Parameters by Scope:\n"
        for scope, count in validation_report['parameters_by_scope'].items():
            report += f"  - {scope}: {count}\n"
        
        if validation_report['validation_issues']:
            report += "\nValidation Issues:\n"
            for issue in validation_report['validation_issues']:
                report += f"  - {issue}\n"
        
        # Save report
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report)
            print(f"Configuration report saved to {output_file}")
        
        return report

# Global instances for easy access
_config_manager: Optional[ConfigurationManager] = None

def get_config_manager(config_file: Optional[str] = None) -> ConfigurationManager:
    """Get or create the global configuration manager."""
    global _config_manager
    if _config_manager is None:
        _config_manager = ConfigurationManager(config_file)
    return _config_manager

def create_standard_config() -> Dict[str, Any]:
    """Create standard NLM configuration."""
    return {
        'brain.neuron_count': 1000,
        'brain.synapse_density': 0.1,
        'brain.v_thresh': -50.0,
        'brain.v_rest': -70.0,
        'plasticity.stdp.enable': True,
        'plasticity.stdp.learning_rate': 0.001,
        'plasticity.hebbian.enable': True,
        'plasticity.structural.enable': True,
        'neuromod.dopamine.scale': 1.0,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'world.width': 20,
        'world.height': 20,
        'world.vision_width': 8,
        'agent.max_energy': 100.0,
        'development.plasticity_rate': 1.0
    }

def create_research_config() -> Dict[str, Any]:
    """Create research-oriented configuration."""
    return {
        'brain.neuron_count': 2000,
        'brain.synapse_density': 0.15,
        'brain.v_thresh': -45.0,
        'brain.v_rest': -65.0,
        'plasticity.stdp.enable': True,
        'plasticity.stdp.learning_rate': 0.002,
        'plasticity.hebbian.enable': True,
        'plasticity.structural.enable': True,
        'neuromod.dopamine.scale': 1.5,
        'neuromod.curiosity.enable': True,
        'neuromod.novelty.enable': True,
        'world.width': 30,
        'world.height': 30,
        'world.vision_width': 12,
        'agent.max_energy': 200.0,
        'development.plasticity_rate': 1.5
    }

# Example usage
if __name__ == "__main__":
    import time
    import random
    
    print("NLM Configuration Management System")
    print("=" * 50)
    
    # Initialize configuration manager
    config_manager = ConfigurationManager()
    
    # Create and save standard configuration
    print("\n1. Creating standard configuration...")
    standard_config = create_standard_config()
    config_manager.configs['standard'] = standard_config
    print("✓ Standard configuration created")
    
    # Create and save research configuration
    print("\n2. Creating research configuration...")
    research_config = create_research_config()
    config_manager.configs['research'] = research_config
    print("✓ Research configuration created")
    
    # Create parameter optimization experiment
    print("\n3. Setting up parameter optimization...")
    optimizer = ParameterOptimizer(config_manager)
    
    parameter_ranges = {
        'brain.neuron_count': [500, 1000, 1500, 2000],
        'brain.synapse_density': [0.05, 0.1, 0.15],
        'neuromod.dopamine.scale': [0.5, 1.0, 1.5, 2.0]
    }
    
    experiments = config_manager.create_parameter_tuning_experiment(
        'standard', parameter_ranges, num_trials=5
    )
    print(f"✓ Created {len(experiments)} experimental configurations")
    
    # Generate configuration report
    print("\n4. Generating configuration report...")
    report = config_manager.get_parameter_validation_report()
    print(f"✓ Validation report generated")
    print(f"  - Total parameters: {report['total_parameters']}")
    print(f"  - Parameters by scope: {report['parameters_by_scope']}")
    
    # Create visualizations
    print("\n5. Creating visualizations...")
    try:
        visualizer = ConfigurationVisualizer()
        
        # Plot parameter distribution
        if 'brain.neuron_count' in standard_config:
            visualizer.plot_parameter_distribution(
                config_manager, 'brain.neuron_count', 'neuron_count_distribution.png'
            )
        
        print("✓ Visualizations created")
    except Exception as e:
        print(f"⚠ Visualization skipped: {e}")
    
    # Demonstrate parameter optimization
    print("\n6. Demonstrating parameter optimization...")
    initial_config = standard_config.copy()
    
    optimization_result = optimizer.optimize_for_performance(
        'firing_rate', 3.0, initial_config, iterations=20
    )
    
    print("✓ Optimization completed")
    print(f"  - Optimized parameter: {optimization_result['performance_metric']}")
    print(f"  - Target value: {optimization_result['target_value']}")
    print(f"  - Best score: {optimization_result['best_score']:.2f}")
    
    # Generate comprehensive report
    print("\n7. Generating comprehensive configuration report...")
    full_report = visualizer.generate_configuration_report(config_manager, 'full_config_report.txt') if visualizer else "Visualization not available"
    print("✓ Comprehensive report generated")
    
    print("\nNLM Configuration Management System Complete!")
    print("\nSummary:")
    print(f"  - Configurations available: {len(config_manager.configs)}")
    print(f"  - Parameter registry size: {len(config_manager.registry.get_all_parameters())}")
    print(f"  - Experiments created: {len(experiments)}")
    print(f"  - Optimization iterations: {optimization_result['iterations']}")
