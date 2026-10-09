#!/usr/bin/env python3
"""
Configuration Presets and Utilities for NLM Python API

This module provides advanced configuration management utilities for the NLM Python API,
including configuration presets, comparison tools, analysis utilities, and export/import capabilities.
"""

import json
import yaml
import csv
import os
import sys
import numpy as np
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass
from enum import Enum
import hashlib
import copy

# Try to import pynlm
import pynlm

class ConfigurationPreset(Enum):
    """Predefined configuration presets"""
    
    # Task-specific presets
    REINFORCEMENT_LEARNING = "reinforcement_learning"
    DEVELOPMENTAL_STUDY = "developmental_study"
    PERCEPTION_EXPERIMENT = "perception_experiment"
    MOTOR_CONTROL = "motor_control"
    COGNITIVE_TASK = "cognitive_task"
    
    # Network size presets
    SMALL = "small"
    MEDIUM = "medium"
    LARGE = "large"
    EXPERT = "expert"
    
    # Research domain presets
    NEUROSCIENCE = "neuroscience"
    ROBOTICS = "robotics"
    ARTIFICIAL_INTELLIGENCE = "artificial_intelligence"
    PSYCHOLOGY = "psychology"
    
    # Performance optimization presets
    SPEED = "speed"
    ACCURACY = "accuracy"
    BALANCE = "balance"

@dataclass
class ConfigurationDiff:
    """Configuration difference analysis"""
    
    added_keys: List[str]
    removed_keys: List[str]
    changed_keys: Dict[str, Any]
    similarity_score: float
    compatibility_score: float

@dataclass
class ConfigurationAnalysis:
    """Configuration analysis results"""
    
    complexity_score: float
    parameter_dependencies: List[str]
    sensitivity_analysis: Dict[str, float]
    performance_score: float
    recommended_optimizations: List[str]

class ConfigurationManager:
    """
    Advanced configuration management for NLM Python API
    """
    
    def __init__(self):
        self.config_history = []
        self.config_cache = {}
    
    def get_preset_configuration(self, preset: ConfigurationPreset) -> Dict[str, Any]:
        """
        Get a predefined configuration preset
        
        Args:
            preset: Configuration preset type
            
        Returns:
            Dictionary with configuration parameters
        """
        presets = {
            ConfigurationPreset.SMALL: {
                "brain.neuron_count": 500,
                "brain.synapse_density": 0.05,
                "plasticity.stdp.learning_rate": 0.005,
                "plasticity.hebbian.enable": False,
                "plasticity.structural.enable": False,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True
            },
            
            ConfigurationPreset.MEDIUM: {
                "brain.neuron_count": 1000,
                "brain.synapse_density": 0.1,
                "plasticity.stdp.learning_rate": 0.01,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": False,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "plasticity.stdp.tau_plus": 20.0,
                "plasticity.stdp.tau_minus": 20.0
            },
            
            ConfigurationPreset.LARGE: {
                "brain.neuron_count": 2000,
                "brain.synapse_density": 0.15,
                "plasticity.stdp.learning_rate": 0.02,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.5,
                "plasticity.stdp.tau_plus": 25.0,
                "plasticity.stdp.tau_minus": 25.0
            },
            
            ConfigurationPreset.EXPERT: {
                "brain.neuron_count": 5000,
                "brain.synapse_density": 0.2,
                "plasticity.stdp.learning_rate": 0.05,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 2.0,
                "plasticity.stdp.tau_plus": 30.0,
                "plasticity.stdp.tau_minus": 30.0,
                "plasticity.structural.synaptogenesis_rate": 0.0001,
                "plasticity.structural.pruning_rate": 0.00001,
                "neuromod.novelty.threshold": 0.3,
                "neuromod.curiosity.exploration_rate": 0.4
            },
            
            ConfigurationPreset.REINFORCEMENT_LEARNING: {
                "brain.neuron_count": 1500,
                "plasticity.stdp.learning_rate": 0.03,
                "neuromod.dopamine.scale": 1.5,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "plasticity.stdp.tau_plus": 20.0,
                "plasticity.stdp.tau_minus": 20.0,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.exploration_rate": 0.3
            },
            
            ConfigurationPreset.DEVELOPMENTAL_STUDY: {
                "brain.neuron_count": 1000,
                "plasticity.stdp.learning_rate": 0.02,
                "plasticity.structural.enable": True,
                "plasticity.structural.synaptogenesis_rate": 0.0002,
                "plasticity.structural.pruning_rate": 0.00002,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.curiosity.exploration_rate": 0.5,
                "neuromod.novelty.threshold": 0.4,
                "brain.initial_weight_mean": 0.6,
                "brain.initial_weight_std": 0.15
            },
            
            ConfigurationPreset.PERCEPTION_EXPERIMENT: {
                "brain.neuron_count": 800,
                "brain.synapse_density": 0.12,
                "plasticity.stdp.learning_rate": 0.015,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.novelty.threshold": 0.25,
                "plasticity.stdp.tau_plus": 15.0,
                "plasticity.stdp.tau_minus": 15.0
            },
            
            ConfigurationPreset.MOTOR_CONTROL: {
                "brain.neuron_count": 1200,
                "brain.synapse_density": 0.18,
                "plasticity.stdp.learning_rate": 0.025,
                "neuromod.dopamine.scale": 1.2,
                "neuromod.curiosity.enable": True,
                "plasticity.stdp.tau_plus": 22.0,
                "plasticity.stdp.tau_minus": 22.0
            },
            
            ConfigurationPreset.COGNITIVE_TASK: {
                "brain.neuron_count": 1500,
                "brain.synapse_density": 0.14,
                "plasticity.stdp.learning_rate": 0.02,
                "plasticity.hebbian.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "plasticity.stdp.tau_plus": 18.0,
                "plasticity.stdp.tau_minus": 18.0,
                "plasticity.hebbian.learning_rate": 0.01
            },
            
            ConfigurationPreset.NEUROSCIENCE: {
                "brain.neuron_count": 2000,
                "brain.synapse_density": 0.16,
                "plasticity.stdp.learning_rate": 0.01,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.0,
                "plasticity.stdp.tau_plus": 20.0,
                "plasticity.stdp.tau_minus": 20.0
            },
            
            ConfigurationPreset.ROBOTICS: {
                "brain.neuron_count": 1800,
                "brain.synapse_density": 0.17,
                "plasticity.stdp.learning_rate": 0.03,
                "plasticity.hebbian.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.3,
                "neuromod.curiosity.exploration_rate": 0.4,
                "plasticity.stdp.tau_plus": 25.0,
                "plasticity.stdp.tau_minus": 25.0
            },
            
            ConfigurationPreset.ARTIFICIAL_INTELLIGENCE: {
                "brain.neuron_count": 3000,
                "brain.synapse_density": 0.18,
                "plasticity.stdp.learning_rate": 0.04,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.5,
                "neuromod.curiosity.exploration_rate": 0.5,
                "plasticity.stdp.tau_plus": 30.0,
                "plasticity.stdp.tau_minus": 30.0
            },
            
            ConfigurationPreset.PSYCHOLOGY: {
                "brain.neuron_count": 1200,
                "brain.synapse_density": 0.12,
                "plasticity.stdp.learning_rate": 0.01,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.curiosity.exploration_rate": 0.3,
                "neuromod.novelty.threshold": 0.35,
                "plasticity.stdp.tau_plus": 18.0,
                "plasticity.stdp.tau_minus": 18.0
            },
            
            ConfigurationPreset.SPEED: {
                "brain.neuron_count": 800,
                "brain.synapse_density": 0.08,
                "plasticity.stdp.learning_rate": 0.008,
                "plasticity.hebbian.enable": False,
                "plasticity.structural.enable": False,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "plasticity.stdp.tau_plus": 15.0,
                "plasticity.stdp.tau_minus": 15.0
            },
            
            ConfigurationPreset.ACCURACY: {
                "brain.neuron_count": 3000,
                "brain.synapse_density": 0.25,
                "plasticity.stdp.learning_rate": 0.06,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 2.0,
                "neuromod.curiosity.exploration_rate": 0.2,
                "plasticity.stdp.tau_plus": 35.0,
                "plasticity.stdp.tau_minus": 35.0
            },
            
            ConfigurationPreset.BALANCE: {
                "brain.neuron_count": 1500,
                "brain.synapse_density": 0.15,
                "plasticity.stdp.learning_rate": 0.02,
                "plasticity.hebbian.enable": True,
                "plasticity.structural.enable": True,
                "neuromod.curiosity.enable": True,
                "neuromod.novelty.enable": True,
                "neuromod.dopamine.scale": 1.2,
                "neuromod.curiosity.exploration_rate": 0.3,
                "neuromod.novelty.threshold": 0.3,
                "plasticity.stdp.tau_plus": 20.0,
                "plasticity.stdp.tau_minus": 20.0
            }
        }
        
        return presets.get(preset, {})
    
    def compare_configurations(self, config1: Dict[str, Any], config2: Dict[str, Any]) -> ConfigurationDiff:
        """
        Compare two configurations and return differences
        
        Args:
            config1: First configuration
            config2: Second configuration
            
        Returns:
            ConfigurationDiff object with differences
        """
        keys1 = set(config1.keys())
        keys2 = set(config2.keys())
        
        added_keys = list(keys2 - keys1)
        removed_keys = list(keys1 - keys2)
        
        changed_keys = {}
        common_keys = keys1 & keys2
        
        for key in common_keys:
            if config1[key] != config2[key]:
                changed_keys[key] = {
                    "old_value": config1[key],
                    "new_value": config2[key]
                }
        
        # Calculate similarity score
        all_keys = keys1 | keys2
        if len(all_keys) == 0:
            similarity_score = 1.0
        else:
            common_common = len(common_keys | changed_keys.keys())
            similarity_score = common_common / len(all_keys)
        
        # Calculate compatibility score
        compatibility_score = 0.0
        if config1 and config2:
            compatibility_score = len(common_keys) / max(len(keys1), len(keys2))
        
        return ConfigurationDiff(
            added_keys=added_keys,
            removed_keys=removed_keys,
            changed_keys=changed_keys,
            similarity_score=similarity_score,
            compatibility_score=compatibility_score
        )
    
    def analyze_configuration(self, config: Dict[str, Any]) -> ConfigurationAnalysis:
        """
        Analyze configuration and provide optimization recommendations
        
        Args:
            config: Configuration to analyze
            
        Returns:
            ConfigurationAnalysis object with analysis results
        """
        # Calculate complexity score based on number of parameters
        complexity_score = min(len(config) / 100.0, 1.0)  # Normalize to 0-1
        
        # Identify parameter dependencies
        dependencies = []
        for key in config.keys():
            if "plasticity" in key or "neuromod" in key:
                dependencies.append(key)
        
        # Analyze parameter sensitivity (simplified)
        sensitivity_analysis = {}
        for key, value in config.items():
            if isinstance(value, (int, float)):
                sensitivity = abs(value) / (abs(value) + 1.0)  # Normalized sensitivity
                sensitivity_analysis[key] = sensitivity
        
        # Calculate performance score based on key parameters
        performance_score = 0.0
        if config.get("brain.neuron_count", 0) > 0:
            neuron_factor = min(config["brain.neuron_count"] / 5000.0, 1.0)
            performance_score += neuron_factor * 0.3
        
        if config.get("plasticity.stdp.learning_rate", 0) > 0:
            learning_rate_factor = min(config["plasticity.stdp.learning_rate"] / 0.1, 1.0)
            performance_score += learning_rate_factor * 0.3
        
        if config.get("neuromod.curiosity.enable", False):
            performance_score += 0.2
        
        if config.get("neuromod.novelty.enable", False):
            performance_score += 0.2
        
        # Generate optimization recommendations
        optimizations = []
        
        if config.get("brain.neuron_count", 0) > 2000:
            optimizations.append("Consider reducing neuron count for better performance")
        
        if config.get("brain.synapse_density", 0) > 0.2:
            optimizations.append("High synapse density may cause performance issues")
        
        if config.get("plasticity.stdp.learning_rate", 0) > 0.05:
            optimizations.append("High learning rate may cause instability")
        
        if not config.get("neuromod.curiosity.enable", False):
            optimizations.append("Enable curiosity for better exploration")
        
        if not config.get("neuromod.novelty.enable", False):
            optimizations.append("Enable novelty detection for better learning")
        
        return ConfigurationAnalysis(
            complexity_score=complexity_score,
            parameter_dependencies=dependencies,
            sensitivity_analysis=sensitivity_analysis,
            performance_score=performance_score,
            recommended_optimizations=optimizations
        )
    
    def merge_configurations(self, configs: List[Dict[str, Any]], strategy: str = "override") -> Dict[str, Any]:
        """
        Merge multiple configurations
        
        Args:
            configs: List of configurations to merge
            strategy: Merging strategy ("override", "average", "conservative")
            
        Returns:
            Merged configuration
        """
        if not configs:
            return {}
        
        if strategy == "override":
            # Last configuration overrides previous ones
            merged = configs[-1].copy()
            for config in reversed(configs[:-1]):
                for key, value in config.items():
                    if key not in merged:
                        merged[key] = value
        
        elif strategy == "average":
            # Calculate average for numeric values
            merged = {}
            for key in set().union(*[config.keys() for config in configs]):
                values = [config.get(key) for config in configs if key in config]
                if not values:
                    continue
                
                # Average numeric values
                numeric_values = [v for v in values if isinstance(v, (int, float))]
                if numeric_values:
                    merged[key] = sum(numeric_values) / len(numeric_values)
                else:
                    # Use first non-numeric value
                    merged[key] = values[0]
        
        elif strategy == "conservative":
            # Most conservative values
            merged = {}
            for key in set().union(*[config.keys() for config in configs]):
                values = [config.get(key) for config in configs if key in config]
                if not values:
                    continue
                
                # For booleans, use True if any is True
                if all(isinstance(v, bool) for v in values):
                    merged[key] = any(values)
                else:
                    # Use first value
                    merged[key] = values[0]
        
        return merged
    
    def save_configuration_to_file(self, config: Dict[str, Any], filename: str, format: str = "json"):
        """
        Save configuration to file in specified format
        
        Args:
            config: Configuration to save
            filename: Output filename
            format: Output format ("json", "yaml", "csv", "xml", "toml")
        """
        if format == "json":
            with open(filename, 'w') as f:
                json.dump(config, f, indent=2)
        
        elif format == "yaml":
            try:
                import yaml
                with open(filename, 'w') as f:
                    yaml.dump(config, f, default_flow_style=False)
            except ImportError:
                print("YAML module not available, skipping YAML export")
        
        elif format == "csv":
            with open(filename, 'w', newline='') as f:
                writer = csv.writer(f)
                writer.writerow(["parameter", "value"])
                for key, value in config.items():
                    writer.writerow([key, value])
        
        elif format == "xml":
            # Simplified XML export
            xml_content = '<?xml version="1.0" encoding="UTF-8"?>\n'
            xml_content += '<configuration>\n'
            for key, value in config.items():
                xml_content += f'  <{key}>{value}</{key}>\n'
            xml_content += '</configuration>'
            
            with open(filename, 'w') as f:
                f.write(xml_content)
        
        elif format == "toml":
            try:
                import tomli_w
                with open(filename, 'wb') as f:
                    tomli_w.dump(config, f)
            except ImportError:
                print("TOML module not available, skipping TOML export")
        
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        print(f"Configuration saved to {filename}")
    
    def load_configuration_from_file(self, filename: str) -> Dict[str, Any]:
        """
        Load configuration from file
        
        Args:
            filename: Input filename
            
        Returns:
            Loaded configuration
        """
        file_ext = os.path.splitext(filename)[1].lower()
        
        if file_ext == '.json':
            with open(filename, 'r') as f:
                return json.load(f)
        
        elif file_ext == '.yaml' or file_ext == '.yml':
            try:
                import yaml
                with open(filename, 'r') as f:
                    return yaml.safe_load(f)
            except ImportError:
                print("YAML module not available, skipping YAML import")
                return {}
        
        elif file_ext == '.csv':
            with open(filename, 'r') as f:
                reader = csv.DictReader(f)
                return {row['parameter']: row['value'] for row in reader}
        
        elif file_ext == '.xml':
            # Simplified XML import
            import xml.etree.ElementTree as ET
            tree = ET.parse(filename)
            root = tree.getroot()
            
            config = {}
            for child in root:
                config[child.tag] = child.text
            
            return config
        
        else:
            raise ValueError(f"Unsupported file format: {file_ext}")
    
    def validate_configuration(self, config: Dict[str, Any]) -> Dict[str, Any]:
        """
        Validate configuration and return validation results
        
        Args:
            config: Configuration to validate
            
        Returns:
            Dictionary with validation results
        """
        validation_results = {
            "is_valid": True,
            "errors": [],
            "warnings": [],
            "suggestions": []
        }
        
        # Check for required keys
        required_keys = ["brain.neuron_count"]
        for key in required_keys:
            if key not in config:
                validation_results["errors"].append(f"Missing required parameter: {key}")
                validation_results["is_valid"] = False
        
        # Validate parameter ranges
        if "brain.neuron_count" in config:
            neuron_count = config["brain.neuron_count"]
            if not isinstance(neuron_count, (int, float)):
                validation_results["errors"].append("brain.neuron_count must be a number")
                validation_results["is_valid"] = False
            elif neuron_count <= 0:
                validation_results["warnings"].append("brain.neuron_count should be positive")
        
        if "brain.synapse_density" in config:
            density = config["brain.synapse_density"]
            if not isinstance(density, (int, float)):
                validation_results["errors"].append("brain.synapse_density must be a number")
                validation_results["is_valid"] = False
            elif density < 0 or density > 1:
                validation_results["warnings"].append("brain.synapse_density should be between 0 and 1")
        
        # Check for incompatible combinations
        if config.get("plasticity.hebbian.enable", False) and config.get("plasticity.stdp.enable", False):
            validation_results["suggestions"].append("Both Hebbian and STDP plasticity enabled - may cause interference")
        
        # Check for performance warnings
        if config.get("brain.neuron_count", 0) > 5000:
            validation_results["warnings"].append("Large number of neurons may cause performance issues")
        
        if config.get("brain.synapse_density", 0) > 0.3:
            validation_results["warnings"].append("High synapse density may cause memory issues")
        
        return validation_results


class ConfigurationOptimizer:
    """
    Configuration optimization utilities
    """
    
    @staticmethod
    def optimize_for_performance(config: Dict[str, Any], target_performance: float = 0.8) -> Dict[str, Any]:
        """
        Optimize configuration for target performance
        
        Args:
            config: Configuration to optimize
            target_performance: Target performance score (0-1)
            
        Returns:
            Optimized configuration
        """
        optimized_config = config.copy()
        
        # Adjust neuron count for target performance
        current_performance = ConfigurationManager().analyze_configuration(config).performance_score
        
        if current_performance < target_performance:
            # Increase complexity
            if optimized_config.get("brain.neuron_count", 0) < 3000:
                optimized_config["brain.neuron_count"] = int(optimized_config.get("brain.neuron_count", 1000) * 1.5)
            
            if not optimized_config.get("plasticity.hebbian.enable", False):
                optimized_config["plasticity.hebbian.enable"] = True
        
        elif current_performance > target_performance + 0.2:
            # Reduce complexity
            if optimized_config.get("brain.neuron_count", 0) > 500:
                optimized_config["brain.neuron_count"] = int(optimized_config.get("brain.neuron_count", 1000) * 0.7)
            
            if optimized_config.get("plasticity.hebbian.enable", False):
                optimized_config["plasticity.hebbian.enable"] = False
        
        return optimized_config
    
    @staticmethod
    def find_optimal_parameter_combination(param_space: Dict[str, List[Any]], 
                                           target_metric: str = "performance") -> Dict[str, Any]:
        """
        Find optimal parameter combination using grid search
        
        Args:
            param_space: Parameter space (parameter name -> list of values)
            target_metric: Target metric for optimization
            
        Returns:
            Optimal parameter combination
        """
        best_combination = {}
        best_score = float('-inf')
        
        # Grid search (simplified)
        param_names = list(param_space.keys())
        param_values = list(param_space.values())
        
        # Generate all combinations (simplified for demonstration)
        from itertools import product
        
        for combination in product(*param_values):
            param_dict = dict(zip(param_names, combination))
            
            # Evaluate combination (simplified evaluation)
            score = ConfigurationOptimizer._evaluate_combination(param_dict, target_metric)
            
            if score > best_score:
                best_score = score
                best_combination = param_dict
        
        return best_combination
    
    @staticmethod
    def _evaluate_combination(combination: Dict[str, Any], target_metric: str) -> float:
        """Evaluate parameter combination (simplified)"""
        # Simplified evaluation logic
        score = 0.0
        
        # Neuron count contribution
        neuron_count = combination.get("brain.neuron_count", 1000)
        score += min(neuron_count / 5000.0, 1.0) * 0.3
        
        # Learning rate contribution
        learning_rate = combination.get("plasticity.stdp.learning_rate", 0.01)
        score += min(learning_rate / 0.1, 1.0) * 0.3
        
        # Feature contributions
        if combination.get("neuromod.curiosity.enable", False):
            score += 0.1
        
        if combination.get("neuromod.novelty.enable", False):
            score += 0.1
        
        if combination.get("plasticity.hebbian.enable", False):
            score += 0.1
        
        if combination.get("plasticity.structural.enable", False):
            score += 0.1
        
        return score


class ConfigurationSharing:
    """
    Configuration sharing utilities
    """
    
    def __init__(self):
        self.shared_configurations = {}
        self.community_tags = set()
    
    def share_configuration(self, name: str, config: Dict[str, Any], 
                           tags: Optional[List[str]] = None,
                           description: str = ""):
        """
        Share configuration in community
        
        Args:
            name: Configuration name
            config: Configuration to share
            tags: Configuration tags
            description: Description of configuration
        """
        config_id = hashlib.md5(json.dumps(config, sort_keys=True).encode()).hexdigest()
        
        shared_config = {
            "id": config_id,
            "name": name,
            "config": config,
            "tags": tags or [],
            "description": description,
            "created_by": "user",
            "created_at": time.time(),
            "usage_count": 0
        }
        
        self.shared_configurations[config_id] = shared_config
        
        if tags:
            self.community_tags.update(tags)
        
        print(f"Configuration '{name}' shared successfully (ID: {config_id})")
    
    def get_shared_configurations(self, tags: Optional[List[str]] = None) -> List[Dict[str, Any]]:
        """
        Get shared configurations filtered by tags
        
        Args:
            tags: Tags to filter by
            
        Returns:
            List of shared configurations
        """
        if not tags:
            return list(self.shared_configurations.values())
        
        filtered_configs = []
        for config in self.shared_configurations.values():
            if any(tag in config["tags"] for tag in tags):
                filtered_configs.append(config)
        
        return filtered_configs
    
    def import_shared_configuration(self, config_id: str) -> Optional[Dict[str, Any]]:
        """
        Import shared configuration by ID
        
        Args:
            config_id: Configuration ID
            
        Returns:
            Imported configuration or None if not found
        """
        config = self.shared_configurations.get(config_id)
        if config:
            config["usage_count"] += 1
            return config["config"]
        
        return None
    
    def get_community_tags(self) -> List[str]:
        """
        Get all community tags
        
        Returns:
            List of community tags
        """
        return list(self.community_tags)


# Global configuration manager instance
_config_manager = ConfigurationManager()
_config_optimizer = ConfigurationOptimizer()
_config_sharing = ConfigurationSharing()

# Convenience functions
def get_preset(preset: ConfigurationPreset) -> Dict[str, Any]:
    """Get a configuration preset"""
    return _config_manager.get_preset_configuration(preset)

def compare_configs(config1: Dict[str, Any], config2: Dict[str, Any]) -> ConfigurationDiff:
    """Compare two configurations"""
    return _config_manager.compare_configurations(config1, config2)

def analyze_config(config: Dict[str, Any]) -> ConfigurationAnalysis:
    """Analyze configuration"""
    return _config_manager.analyze_configuration(config)

def merge_configs(configs: List[Dict[str, Any]], strategy: str = "override") -> Dict[str, Any]:
    """Merge configurations"""
    return _config_manager.merge_configurations(configs, strategy)

def save_config(config: Dict[str, Any], filename: str, format: str = "json"):
    """Save configuration to file"""
    _config_manager.save_configuration_to_file(config, filename, format)

def load_config(filename: str) -> Dict[str, Any]:
    """Load configuration from file"""
    return _config_manager.load_configuration_from_file(filename)

def validate_config(config: Dict[str, Any]) -> Dict[str, Any]:
    """Validate configuration"""
    return _config_manager.validate_configuration(config)

def optimize_config(config: Dict[str, Any], target_performance: float = 0.8) -> Dict[str, Any]:
    """Optimize configuration"""
    return _config_optimizer.optimize_for_performance(config, target_performance)

def find_optimal_config(param_space: Dict[str, List[Any]], 
                        target_metric: str = "performance") -> Dict[str, Any]:
    """Find optimal configuration"""
    return _config_optimizer.find_optimal_parameter_combination(param_space, target_metric)

def share_config(name: str, config: Dict[str, Any], tags: Optional[List[str]] = None, 
                 description: str = ""):
    """Share configuration"""
    _config_sharing.share_configuration(name, config, tags, description)

def get_shared_configs(tags: Optional[List[str]] = None) -> List[Dict[str, Any]]:
    """Get shared configurations"""
    return _config_sharing.get_shared_configurations(tags)

def import_shared_config(config_id: str) -> Optional[Dict[str, Any]]:
    """Import shared configuration"""
    return _config_sharing.import_shared_configuration(config_id)

def get_tags() -> List[str]:
    """Get community tags"""
    return _config_sharing.get_community_tags()

# Export presets for easy access
EXPRESS_PRESET = ConfigurationPreset.SMALL
STANDARD_PRESET = ConfigurationPreset.MEDIUM
ADVANCED_PRESET = ConfigurationPreset.LARGE
EXPERT_PRESET = ConfigurationPreset.EXPERT

RL_PRESET = ConfigurationPreset.REINFORCEMENT_LEARNING
DEV_PRESET = ConfigurationPreset.DEVELOPMENTAL_STUDY
PERF_PRESET = ConfigurationPreset.PERCEPTION_EXPERIMENT
MOT_PRESET = ConfigurationPreset.MOTOR_CONTROL
COG_PRESET = ConfigurationPreset.COGNITIVE_TASK

NEUROSCIENCE_PRESET = ConfigurationPreset.NEUROSCIENCE
ROBOTICS_PRESET = ConfigurationPreset.ROBOTICS
AI_PRESET = ConfigurationPreset.ARTIFICIAL_INTELLIGENCE
PSYCHOLOGY_PRESET = ConfigurationPreset.PSYCHOLOGY

SPEED_PRESET = ConfigurationPreset.SPEED
ACCURACY_PRESET = ConfigurationPreset.ACCURACY
BALANCE_PRESET = ConfigurationPreset.BALANCE

def main():
    """Example usage of configuration presets and utilities"""
    print("=== Configuration Presets and Utilities Example ===\n")
    
    # Example 1: Get and use preset configurations
    print("1. Using preset configurations:")
    
    small_config = get_preset(EXPRESS_PRESET)
    print(f"   {EXPRESS_PRESET.value} configuration: {small_config}")
    
    medium_config = get_preset(STANDARD_PRESET)
    print(f"   {STANDARD_PRESET.value} configuration: {medium_config}")
    
    large_config = get_preset(ADVANCED_PRESET)
    print(f"   {ADVANCED_PRESET.value} configuration: {large_config}")
    
    # Example 2: Compare configurations
    print("\n2. Comparing configurations:")
    
    comparison = compare_configs(small_config, large_config)
    print(f"   Similarity score: {comparison.similarity_score:.2f}")
    print(f"   Compatibility score: {comparison.compatibility_score:.2f}")
    print(f"   Added keys: {comparison.added_keys}")
    print(f"   Changed keys: {comparison.changed_keys}")
    
    # Example 3: Analyze configuration
    print("\n3. Analyzing configuration:")
    
    analysis = analyze_config(large_config)
    print(f"   Complexity score: {analysis.complexity_score:.2f}")
    print(f"   Performance score: {analysis.performance_score:.2f}")
    print(f"   Parameter dependencies: {analysis.parameter_dependencies}")
    print(f"   Recommended optimizations: {analysis.recommended_optimizations}")
    
    # Example 4: Merge configurations
    print("\n4. Merging configurations:")
    
    merged_config = merge_configs([small_config, medium_config, large_config], "average")
    print(f"   Merged configuration: {merged_config}")
    
    # Example 5: Validate configuration
    print("\n5. Validating configuration:")
    
    validation = validate_config(large_config)
    print(f"   Is valid: {validation['is_valid']}")
    print(f"   Errors: {validation['errors']}")
    print(f"   Warnings: {validation['warnings']}")
    print(f"   Suggestions: {validation['suggestions']}")
    
    # Example 6: Optimize configuration
    print("\n6. Optimizing configuration:")
    
    optimized_config = optimize_config(large_config, target_performance=0.8)
    print(f"   Optimized configuration: {optimized_config}")
    
    optimized_analysis = analyze_config(optimized_config)
    print(f"   Optimized performance score: {optimized_analysis.performance_score:.2f}")
    
    # Example 7: Save and load configuration
    print("\n7. Saving and loading configuration:")
    
    save_config(optimized_config, "optimized_config.json")
    
    loaded_config = load_config("optimized_config.json")
    print(f"   Loaded configuration: {loaded_config}")
    
    # Example 8: Share configuration
    print("\n8. Sharing configuration:")
    
    share_config("My Optimized Config", optimized_config, ["optimization", "performance", "nlm"], "An optimized configuration for research")
    
    shared_configs = get_shared_configs(["optimization", "performance"])
    print(f"   Shared configurations: {len(shared_configs)}")
    
    # Example 9: Advanced preset usage
    print("\n9. Advanced preset usage:")
    
    rl_config = get_preset(RL_PRESET)
    print(f"   Reinforcement learning configuration: {rl_config}")
    
    dev_config = get_preset(DEV_PRESET)
    print(f"   Developmental study configuration: {dev_config}")
    
    # Example 10: Community features
    print("\n10. Community features:")
    
    tags = get_tags()
    print(f"   Available tags: {tags}")
    
    # Find optimal parameter combination
    param_space = {
        "brain.neuron_count": [1000, 2000, 3000],
        "plasticity.stdp.learning_rate": [0.01, 0.05, 0.1],
        "neuromod.curiosity.enable": [True, False]
    }
    
    optimal_config = find_optimal_config(param_space)
    print(f"   Optimal configuration: {optimal_config}")
    
    print("\n=== Configuration utilities demonstration complete ===")

if __name__ == "__main__":
    main()
