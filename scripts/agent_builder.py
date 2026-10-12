#!/usr/bin/env python3
"""
Agent Configuration Builder

Advanced agent configuration management with validation, templates, and optimization.

This module provides comprehensive tools for building and optimizing agent
configurations for Neural Learning Machines (NLM) with support for various
architectures and parameter tuning.

Features:
- Agent architecture templates (explorer, planner, learner, etc.)
- Configuration validation and schema enforcement
- Parameter optimization and performance prediction
- Configuration inheritance and customization
- Configuration export/import utilities

Usage:
    python scripts/agent_builder.py --build --type explorer --output config.json
    python scripts/agent_builder.py --optimize --config config.json
"""

import sys
from pathlib import Path
import json
import copy
from typing import Dict, List, Any, Optional, Union, Callable
import numpy as np
import hashlib
import warnings
from dataclasses import dataclass, field
from enum import Enum

# Add parent directory to path for imports
sys.path.insert(0, str(Path(__file__).parent.parent.parent))

from nlm.config import get_default_config, ConfigValidator
from nlm.models import BaseNeuralModel
from nlm.memory import WorkingMemory
from nlm.neuromodulation import NeuromodulationSystem


class ArchitectureType(Enum):
    """Predefined agent architecture types."""
    EXPLORER = "explorer"
    PLANNER = "planner"
    LEARNER = "learner"
    NAVIGATOR = "navigator"
    COLLECTOR = "collector"
    COGNITIVE = "cognitive"
    SOCIAL = "social"
    ADAPTIVE = "adaptive"
    CREATIVE = "creative"
    META = "meta"
class PerformanceMetric(Enum):
    """Performance metrics for agent evaluation."""
    EXPLORATION_EFFICIENCY = "exploration_efficiency"
    LEARNING_SPEED = "learning_speed"
    MEMORY_USAGE = "memory_usage"
    RESPONSE_TIME = "response_time"
    ACCURACY = "accuracy"
    ADAPTATION_RATE = "adaptation_rate"
    EXPLORATION_BREADTH = "exploration_breadth"
    EXPLOITATION_DEPTH = "exploitation_depth"


@dataclass
class AgentTemplate:
    """Template for agent configuration with validation rules."""
    name: str
    description: str
    architecture_type: ArchitectureType
    config_schema: Dict[str, Any]
    validation_rules: Dict[str, Callable] = field(default_factory=dict)
    default_parameters: Dict[str, Any] = field(default_factory=dict)
    constraints: Dict[str, Any] = field(default_factory=dict)
    metadata: Dict[str, Any] = field(default_factory=dict)


@dataclass
class ValidationResult:
    """Result of configuration validation."""
    is_valid: bool
    errors: List[str] = field(default_factory=list)
    warnings: List[str] = field(default_factory=list)
    score: float = 0.0


class AgentConfigurationBuilder:
    """Advanced agent configuration builder with validation and optimization."""

    def __init__(self):
        """Initialize the configuration builder."""
        self.templates = self._load_default_templates()
        self.config_history = []
        self.validation_rules = self._load_default_validation_rules()

    def _load_default_templates(self) -> Dict[str, AgentTemplate]:
        """Load default agent templates."""
        templates = {}
        
        # Explorer template
        explorer_schema = {
            "learning_rate": {"type": "float", "min": 0.001, "max": 0.1},
            "curiosity_weight": {"type": "float", "min": 0.1, "max": 1.0},
            "exploration_ratio": {"type": "float", "min": 0.3, "max": 0.9},
            "memory_capacity": {"type": "int", "min": 100, "max": 10000},
            "novelty_threshold": {"type": "float", "min": 0.1, "max": 0.5},
            "adaptation_rate": {"type": "float", "min": 0.001, "max": 0.1}
        }
        
        templates["explorer"] = AgentTemplate(
            name="Explorer Agent",
            description="Agent focused on exploration and discovery with curiosity-driven learning",
            architecture_type=ArchitectureType.EXPLORER,
            config_schema=explorer_schema,
            default_parameters={
                "learning_rate": 0.01,
                "curiosity_weight": 0.5,
                "exploration_ratio": 0.7,
                "memory_capacity": 1000,
                "novelty_threshold": 0.3,
                "adaptation_rate": 0.01
            }
        )
        
        # Planner template
        planner_schema = {
            "planning_horizon": {"type": "int", "min": 5, "max": 100},
            "goal_valuation_weight": {"type": "float", "min": 0.1, "max": 1.0},
            "uncertainty_tolerance": {"type": "float", "min": 0.0, "max": 1.0},
            "action_selection_temperature": {"type": "float", "min": 0.1, "max": 2.0},
            "replan_frequency": {"type": "int", "min": 1, "max": 50},
            "cognitive_load": {"type": "int", "min": 1, "max": 10}
        }
        
        templates["planner"] = AgentTemplate(
            name="Planner Agent",
            description="Agent specialized in sequential planning and goal achievement",
            architecture_type=ArchitectureType.PLANNER,
            config_schema=planner_schema,
            default_parameters={
                "planning_horizon": 20,
                "goal_valuation_weight": 0.7,
                "uncertainty_tolerance": 0.5,
                "action_selection_temperature": 1.0,
                "replan_frequency": 10,
                "cognitive_load": 3
            }
        )
        
        # Learner template
        learner_schema = {
            "learning_rate": {"type": "float", "min": 0.001, "max": 0.1},
            "forgetting_rate": {"type": "float", "min": 0.0, "max": 0.5},
            "consolidation_threshold": {"type": "float", "min": 0.1, "max": 1.0},
            "pattern_recognition_depth": {"type": "int", "min": 1, "max": 10},
            "generalization_factor": {"type": "float", "min": 0.1, "max": 1.0}
        }
        
        templates["learner"] = AgentTemplate(
            name="Learner Agent",
            description="Agent focused on efficient learning and memory consolidation",
            architecture_type=ArchitectureType.LEARNER,
            config_schema=learner_schema,
            default_parameters={
                "learning_rate": 0.01,
                "forgetting_rate": 0.05,
                "consolidation_threshold": 0.8,
                "pattern_recognition_depth": 3,
                "generalization_factor": 0.7
            }
        )
        
        return templates

    def _load_default_validation_rules(self) -> Dict[str, Callable]:
        """Load default configuration validation rules."""
        return {
            "arch_consistency": self._validate_architecture_consistency,
            "parameter_constraints": self._validate_parameter_constraints,
            "performance_predictor": self._predict_performance,
            "resource_check": self._check_resource_requirements
        }
    def build_configuration(
        self,
        architecture_type: Union[str, ArchitectureType],
        custom_params: Optional[Dict[str, Any]] = None,
        inheritance_path: Optional[List[str]] = None
    ) -> Dict[str, Any]:
        """Build a custom agent configuration.
        
        Args:
            architecture_type: Type of architecture to build
            custom_params: Custom parameters to override defaults
            inheritance_path: List of template names to inherit from
            
        Returns:
            Complete agent configuration
            
        Raises:
            ValueError: If architecture type is not found or validation fails
        """
        # Convert to ArchitectureType if string
        if isinstance(architecture_type, str):
            try:
                arch_type = ArchitectureType(architecture_type.lower())
            except ValueError:
                available_types = [t.value for t in ArchitectureType]
                raise ValueError(f"Unknown architecture type: {architecture_type}. "
                               f"Available types: {available_types}")
        else:
            arch_type = architecture_type
        
        # Find template
        template_name = arch_type.value
        if template_name not in self.templates:
            raise ValueError(f"Template not found for architecture: {arch_type}")
        
        template = self.templates[template_name]
        
        # Start with default parameters
        config = copy.deepcopy(template.default_parameters)
        config["architecture_type"] = template_name
        config["name"] = template.name
        config["description"] = template.description
        
        # Apply inheritance if specified
        if inheritance_path:
            config = self._apply_inheritance(config, inheritance_path)
        
        # Apply custom parameters
        if custom_params:
            config = self._apply_custom_parameters(config, custom_params)
        
        # Validate configuration
        validation_result = self.validate_configuration(config, template)
        if not validation_result.is_valid:
            errors = "; ".join(validation_result.errors)
            raise ValueError(f"Configuration validation failed: {errors}")
        
        # Run performance prediction
        performance_score = validation_result.score
        config["performance_score"] = performance_score
        
        # Generate config hash for tracking
        config_hash = self._generate_config_hash(config)
        config["config_hash"] = config_hash
        
        # Store in history
        self.config_history.append(config)
        
        return config

    def _apply_inheritance(
        self,
        base_config: Dict[str, Any],
        inheritance_path: List[str]
    ) -> Dict[str, Any]:
        """Apply inheritance from other templates.
        
        Args:
            base_config: Base configuration to inherit into
            inheritance_path: List of template names to inherit from
            
        Returns:
            Configuration with inheritance applied
        """
        config = copy.deepcopy(base_config)
        
        for template_name in inheritance_path:
            if template_name in self.templates:
                parent_template = self.templates[template_name]
                # Merge parent defaults with current config
                for key, value in parent_template.default_parameters.items():
                    if key not in config or self._should_override(key, template_name):
                        config[key] = value
            else:
                warnings.warn(f"Template not found for inheritance: {template_name}")
        
        return config

    def _should_override(self, key: str, template_name: str) -> bool:
        """Determine if a parameter should be overridden by child template."""
        # Child templates should generally override parent parameters
        return True

    def _apply_custom_parameters(
        self,
        config: Dict[str, Any],
        custom_params: Dict[str, Any]
    ) -> Dict[str, Any]:
        """Apply custom parameters to configuration.
        
        Args:
            config: Current configuration
            custom_params: Custom parameters to apply
            
        Returns:
            Configuration with custom parameters applied
        """
        updated_config = copy.deepcopy(config)
        
        for key, value in custom_params.items():
            if key in updated_config:
                updated_config[key] = value
            else:
                warnings.warn(f"Custom parameter '{key}' not in template schema. Adding anyway.")
                updated_config[key] = value
        
        return updated_config

    def validate_configuration(
        self,
        config: Dict[str, Any],
        template: Optional[AgentTemplate] = None
    ) -> ValidationResult:
        """Validate a configuration.
        
        Args:
            config: Configuration to validate
            template: Template to validate against (uses config's template if None)
            
        Returns:
            Validation result with errors and warnings
        """
        result = ValidationResult()
        
        if not template and "architecture_type" in config:
            template_name = config["architecture_type"]
            if template_name in self.templates:
                template = self.templates[template_name]
        
        if not template:
            result.errors.append("No template provided and architecture_type not found")
            return result
        
        # Run validation rules
        for rule_name, rule_func in self.validation_rules.items():
            try:
                rule_result = rule_func(config, template)
                if isinstance(rule_result, ValidationResult):
                    if not rule_result.is_valid:
                        result.errors.extend(rule_result.errors)
                    result.warnings.extend(rule_result.warnings)
                    result.score = max(result.score, rule_result.score)
            except Exception as e:
                result.errors.append(f"Validation rule '{rule_name}' failed: {str(e)}")
        
        result.is_valid = len(result.errors) == 0
        return result

    def _validate_architecture_consistency(
        self,
        config: Dict[str, Any],
        template: AgentTemplate
    ) -> ValidationResult:
        """Validate architectural consistency of configuration."""
        result = ValidationResult()
        
        # Check for required parameters
        required_params = template.default_parameters.keys()
        for param in required_params:
            if param not in config:
                result.errors.append(f"Missing required parameter: {param}")
            elif config[param] is None:
                result.warnings.append(f"Parameter {param} is None")
        
        # Validate parameter ranges
        for param, schema in template.config_schema.items():
            if param in config:
                value = config[param]
                if not self._validate_parameter_value(value, schema):
                    result.errors.append(
                        f"Parameter '{param}' value {value} violates schema: {schema}"
                    )
        
        return result

    def _validate_parameter_constraints(
        self,
        config: Dict[str, Any],
        template: AgentTemplate
    ) -> ValidationResult:
        """Validate parameter constraints."""
        result = ValidationResult()
        
        # Template-specific constraints
        constraints = template.constraints
        for param, constraint in constraints.items():
            if param in config:
                value = config[param]
                if isinstance(constraint, dict):
                    if "range" in constraint:
                        min_val, max_val = constraint["range"]
                        if not (min_val <= value <= max_val):
                            result.errors.append(
                                f"Parameter '{param}' ({value}) outside valid range [{min_val}, {max_val}]"
                            )
                elif isinstance(constraint, (list, tuple)):
                    if value not in constraint:
                        result.errors.append(
                            f"Parameter '{param}' ({value}) not in allowed values: {constraint}"
                        )
        
        return result

    def _validate_parameter_value(self, value: Any, schema: Dict[str, Any]) -> bool:
        """Validate a single parameter value against schema."""
        param_type = schema.get("type")
        
        if param_type == "float":
            return isinstance(value, (int, float)) and schema.get("min", float("-inf")) <= value <= schema.get("max", float("inf"))
        elif param_type == "int":
            return isinstance(value, int) and schema.get("min", float("-inf")) <= value <= schema.get("max", float("inf"))
        elif param_type == "str":
            return isinstance(value, str)
        elif param_type == "bool":
            return isinstance(value, bool)
        elif param_type == "list":
            return isinstance(value, list)
        elif param_type == "dict":
            return isinstance(value, dict)
        else:
            # Type not specified, just check value exists
            return value is not None

    def _predict_performance(self, config: Dict[str, Any], template: AgentTemplate) -> ValidationResult:
        """Predict agent performance based on configuration.
        
        This is a simplified performance prediction model.
        """
        result = ValidationResult()
        
        # Simple heuristic-based performance prediction
        performance = 1.0
        
        # Adjust based on parameter values
        for param, schema in template.config_schema.items():
            if param in config and param in template.default_parameters:
                default_val = template.default_parameters[param]
                current_val = config[param]
                
                # Calculate deviation from optimal (assuming middle of range is optimal)
                if "min" in schema and "max" in schema:
                    optimal = (schema["min"] + schema["max"]) / 2
                    deviation = abs(current_val - optimal)
                    range_size = schema["max"] - schema["min"]
                    if range_size > 0:
                        performance -= (deviation / range_size) * 0.2
        
        # Ensure performance stays in valid range
        result.score = max(0.0, min(1.0, performance))
        
        return result

    def _check_resource_requirements(self, config: Dict[str, Any], template: AgentTemplate) -> ValidationResult:
        """Check if configuration meets resource requirements."""
        result = ValidationResult()
        
        # Check memory requirements
        memory_capacity = config.get("memory_capacity", 1000)
        if memory_capacity > 10000:
            result.warnings.append(f"High memory capacity ({memory_capacity}) may impact performance")
        
        # Check learning rate
        learning_rate = config.get("learning_rate", 0.01)
        if learning_rate > 0.1:
            result.warnings.append(f"High learning rate ({learning_rate}) may cause unstable training")
        
        return result

    def optimize_configuration(
        self,
        config: Dict[str, Any],
        optimization_criteria: Optional[List[PerformanceMetric]] = None,
        iterations: int = 100,
        population_size: int = 20
    ) -> Dict[str, Any]:
        """Optimize configuration parameters using evolutionary algorithm.
        
        Args:
            config: Base configuration to optimize
            optimization_criteria: Metrics to optimize
            iterations: Number of optimization iterations
            population_size: Population size for evolutionary algorithm
            
        Returns:
            Optimized configuration
        """
        if optimization_criteria is None:
            optimization_criteria = [
                PerformanceMetric.EXPLORATION_EFFICIENCY,
                PerformanceMetric.LEARNING_SPEED,
                PerformanceMetric.ADAPTATION_RATE
            ]
        
        # Get template for this config
        template_name = config.get("architecture_type")
        if template_name not in self.templates:
            raise ValueError(f"Unknown architecture type: {template_name}")
        template = self.templates[template_name]
        
        # Initialize population
        population = self._initialize_population(config, template, population_size)
        
        # Evolution loop
        best_config = config
        best_score = float("inf")
        
        for iteration in range(iterations):
            # Evaluate population
            fitness_scores = []
            for individual in population:
                score = self._evaluate_individual(individual, optimization_criteria)
                fitness_scores.append(score)
                
                if score < best_score:
                    best_score = score
                    best_config = individual
            
            # Selection and reproduction
            population = self._evolve_population(population, fitness_scores, template)
            
            # Log progress
            if iteration % 10 == 0:
                avg_score = sum(fitness_scores) / len(fitness_scores)
                print(f"Iteration {iteration}, Average score: {avg_score:.4f}, Best score: {best_score:.4f}")
        
        return best_config
    def _initialize_population(
        self,
        base_config: Dict[str, Any],
        template: AgentTemplate,
        population_size: int
    ) -> List[Dict[str, Any]]:
        """Initialize population for evolutionary optimization."""
        population = []
        
        for _ in range(population_size):
            individual = copy.deepcopy(base_config)
            
            # Apply random mutations
            for param, schema in template.config_schema.items():
                if param in individual and isinstance(schema.get("min"), (int, float)):
                    # Add random variation (5% of parameter range)
                    if "min" in schema and "max" in schema:
                        range_size = schema["max"] - schema["min"]
                        mutation = np.random.normal(0, range_size * 0.05)
                        mutated_value = individual[param] + mutation
                        
                        # Clip to valid range
                        individual[param] = max(schema["min"], min(schema["max"], mutated_value))
            
            population.append(individual)
        
        return population

    def _evaluate_individual(
        self,
        individual: Dict[str, Any],
        criteria: List[PerformanceMetric]
    ) -> float:
        """Evaluate individual configuration based on criteria.
        
        Args:
            individual: Configuration to evaluate
            criteria: Performance metrics to optimize
            
        Returns:
            Fitness score (lower is better)
        """
        # Use performance prediction as a proxy for fitness
        template_name = individual.get("architecture_type")
        if template_name in self.templates:
            template = self.templates[template_name]
            validation_result = self._predict_performance(individual, template)
            
            # Weight criteria based on importance
            score = 1.0 - validation_result.score  # Invert since higher performance = lower score
            return score
        
        return float("inf")

    def _evolve_population(
        self,
        population: List[Dict[str, Any]],
        fitness_scores: List[float],
        template: AgentTemplate
    ) -> List[Dict[str, Any]]:
        """Evolve population through selection and reproduction."""
        population_size = len(population)
        new_population = []
        
        # Selection (roulette wheel)
        total_fitness = sum(fitness_scores)
        if total_fitness == 0:
            # If all fitness scores are zero or negative, select randomly
            selected_indices = np.random.choice(population_size, population_size, replace=True)
        else:
            # Normalize fitness scores for roulette wheel
            normalized_fitness = [f / total_fitness for f in fitness_scores]
            selected_indices = np.random.choice(population_size, population_size, p=normalized_fitness)
        
        # Crossover and mutation for new population
        for i in range(population_size):
            parent1_idx = selected_indices[i]
            parent2_idx = selected_indices[(i + 1) % population_size]
            
            # 50% chance of crossover
            if np.random.random() < 0.5:
                child = self._crossover(population[parent1_idx], population[parent2_idx], template)
            else:
                child = copy.deepcopy(population[parent1_idx])
            
            # Mutation
            child = self._mutate(child, template)
            
            new_population.append(child)
        
        return new_population

    def _crossover(
        self,
        parent1: Dict[str, Any],
        parent2: Dict[str, Any],
        template: AgentTemplate
    ) -> Dict[str, Any]:
        """Crossover between two parent configurations."""
        child = {}
        
        for param in parent1.keys():
            if param in parent2:
                # 50% chance to take from parent1, 50% from parent2
                if np.random.random() < 0.5:
                    child[param] = parent1[param]
                else:
                    child[param] = parent2[param]
            elif param in template.default_parameters:
                # Use template default if parameter not in parent2
                child[param] = parent1[param]
        
        return child

    def _mutate(
        self,
        individual: Dict[str, Any],
        template: AgentTemplate
    ) -> Dict[str, Any]:
        """Apply mutation to an individual."""
        mutated = copy.deepcopy(individual)
        
        for param, schema in template.config_schema.items():
            if param in mutated and isinstance(schema.get("min"), (int, float)):
                # Mutation rate decreases over time (simulated via random chance)
                if np.random.random() < 0.1:  # 10% mutation rate
                    if "min" in schema and "max" in schema:
                        range_size = schema["max"] - schema["min"]
                        mutation = np.random.normal(0, range_size * 0.02)  # Smaller mutations during evolution
                        mutated_value = mutated[param] + mutation
                        
                        # Clip to valid range
                        mutated[param] = max(schema["min"], min(schema["max"], mutated_value))
        
        return mutated

    def _generate_config_hash(self, config: Dict[str, Any]) -> str:
        """Generate hash for configuration tracking."""
        # Create deterministic string representation
        config_str = json.dumps(config, sort_keys=True, default=str)
        return hashlib.sha256(config_str.encode()).hexdigest()[:16]

    def export_configuration(self, config: Dict[str, Any], file_path: str) -> None:
        """Export configuration to file.
        
        Args:
            config: Configuration to export
            file_path: Path to export file
        """
        export_data = {
            "config": config,
            "exported_at": np.datetime64("now").astype(str),
            "builder_version": "1.0.0"
        }
        
        with open(file_path, 'w') as f:
            json.dump(export_data, f, indent=2)

    def import_configuration(self, file_path: str) -> Dict[str, Any]:
        """Import configuration from file.
        
        Args:
            file_path: Path to import file
            
        Returns:
            Imported configuration
            
        Raises:
            FileNotFoundError: If file does not exist
            ValueError: If file format is invalid
        """
        if not Path(file_path).exists():
            raise FileNotFoundError(f"Configuration file not found: {file_path}")
        
        with open(file_path, 'r') as f:
            data = json.load(f)
        
        if "config" not in data:
            raise ValueError("Invalid configuration file format: missing 'config' key")
        
        config = data["config"]
        self.config_history.append(config)
        return config

    def get_configuration_history(self) -> List[Dict[str, Any]]:
        """Get history of generated configurations."""
        return copy.deepcopy(self.config_history)

    def clear_history(self) -> None:
        """Clear configuration history."""
        self.config_history = []


def main():
    """Example usage of AgentConfigurationBuilder."""
    import argparse
    
    parser = argparse.ArgumentParser(description="Agent Configuration Builder")
    parser.add_argument("--build", action="store_true", help="Build a new configuration")
    parser.add_argument("--type", dest="arch_type", choices=[t.value for t in ArchitectureType],
                       help="Architecture type to build")
    parser.add_argument("--output", help="Output file path for configuration")
    parser.add_argument("--optimize", action="store_true", help="Optimize configuration")
    parser.add_argument("--iterations", type=int, default=100, help="Optimization iterations")
    parser.add_argument("--population", type=int, default=20, help="Evolution population size")
    parser.add_argument("--import", dest="import_file", help="Import configuration from file")
    parser.add_argument("--export", dest="export_file", help="Export configuration to file")
    parser.add_argument("--custom", nargs="*", help="Custom parameters as key=value pairs")
    
    args = parser.parse_args()
    
    builder = AgentConfigurationBuilder()
    
    if args.import_file:
        print(f"Importing configuration from {args.import_file}...")
        config = builder.import_configuration(args.import_file)
        print(f"✓ Configuration imported successfully")
        print(f"Configuration hash: {config.get('config_hash', 'N/A')}")
    
    if args.build and args.arch_type:
        print(f"Building {args.arch_type} agent configuration...")
        
        # Parse custom parameters
        custom_params = {}
        if args.custom:
            for param in args.custom:
                if "=" in param:
                    key, value = param.split("=", 1)
                    try:
                        custom_params[key] = json.loads(value)
                    except json.JSONDecodeError:
                        custom_params[key] = value
        
        # Build configuration
        config = builder.build_configuration(
            architecture_type=args.arch_type,
            custom_params=custom_params
        )
        
        print(f"✓ Configuration built successfully")
        print(f"Configuration hash: {config.get('config_hash', 'N/A')}")
        print(f"Performance score: {config.get('performance_score', 0):.2f}")
        print("\nConfiguration details:")
        for key, value in config.items():
            if key not in ["config_hash", "performance_score", "architecture_type", "name", "description"]:
                print(f"  {key}: {value}")
        
        # Export if requested
        if args.output:
            builder.export_configuration(config, args.output)
            print(f"\n✓ Configuration exported to {args.output}")
    
    if args.optimize and args.arch_type:
        print(f"Optimizing {args.arch_type} agent configuration...")
        
        # Build base configuration
        config = builder.build_configuration(architecture_type=args.arch_type)
        
        # Optimize
        optimized_config = builder.optimize_configuration(
            config,
            iterations=args.iterations,
            population_size=args.population
        )
        
        print(f"✓ Configuration optimized successfully")
        print(f"Original performance score: {config.get('performance_score', 0):.2f}")
        print(f"Optimized performance score: {optimized_config.get('performance_score', 0):.2f}")
        
        # Show improved parameters
        print("\nImproved parameters:")
        for key, value in optimized_config.items():
            if key in config and key not in ["config_hash", "performance_score"]:
                if value != config[key]:
                    print(f"  {key}: {config[key]:.4f} -> {value:.4f}")
    
    if args.export_file and "config" in locals():
        builder.export_configuration(config, args.export_file)
        print(f"\n✓ Configuration exported to {args.export_file}")


if __name__ == "__main__":
    main()
