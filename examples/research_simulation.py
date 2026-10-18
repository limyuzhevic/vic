#!/usr/bin/env python3
"""
Research Simulation Example
===========================

This example demonstrates research-specific configuration validation and
parameter management for scientific experiments. It showcases advanced
configuration management, parameter history, and reproducibility.

Key Features Demonstrated:
- Research configuration with validation
- Custom parameter settings for scientific experiments
- Parameter history and reproducibility
- Advanced configuration management

File: examples/research_simulation.py
"""

import json
import sys
import os
import hashlib
from datetime import datetime

# Add the python bindings directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), '..', 'python'))

try:
    import pynlm as nlm
    PYNLM_AVAILABLE = True
except ImportError as e:
    print(f"Error: pynlm module not available: {e}")
    sys.exit(1)

class ValidationError(Exception):
    """Custom validation error for research simulations."""
    pass

class ResearchConfigManager:
    """Manages research configurations with validation and reproducibility."""
    
    def __init__(self):
        self.config_history = []
        self.parameter_definitions = {}
        self.reproducibility_hashes = []
        
        # Initialize with base configuration
        self.config = nlm.createDefaultConfig()
        
        # Define research parameter types
        self._setup_parameter_definitions()
    
    def _setup_parameter_definitions(self):
        """Define parameter types and constraints for research."""
        # Brain parameters
        self.parameter_definitions["brain.neuron_count"] = {
            "type": int,
            "range": (100, 50000),
            "required": True,
            "units": "neurons"
        }
        
        self.parameter_definitions["brain.synapse_density"] = {
            "type": float,
            "range": (0.01, 0.2),
            "required": False,
            "units": "connections/neuron^2"
        }
        
        self.parameter_definitions["brain.region_count"] = {
            "type": int,
            "range": (1, 20),
            "required": False,
            "units": "regions"
        }
        
        # Plasticity parameters
        self.parameter_definitions["plasticity.stdp.learning_rate"] = {
            "type": float,
            "range": (0.0001, 0.1),
            "required": False,
            "units": "learning_rate"
        }
        
        self.parameter_definitions["plasticity.stdp.time_constant"] = {
            "type": float,
            "range": (5.0, 100.0),
            "required": False,
            "units": "ms"
        }
        
        self.parameter_definitions["plasticity.stdp.enable"] = {
            "type": bool,
            "required": False,
            "units": "enabled"
        }
        
        # Neuromodulation parameters
        self.parameter_definitions["neuromod.dopamine.scale"] = {
            "type": float,
            "range": (0.1, 5.0),
            "required": False,
            "units": "modulation_strength"
        }
        
        self.parameter_definitions["neuromod.serotonin.modulation"] = {
            "type": float,
            "range": (0.0, 1.0),
            "required": False,
            "units": "modulation_strength"
        }
        
        self.parameter_definitions["neuromod.curiosity.enable"] = {
            "type": bool,
            "required": False,
            "units": "enabled"
        }
        
        # Development parameters
        self.parameter_definitions["development.enable"] = {
            "type": bool,
            "required": False,
            "units": "enabled"
        }
        
        self.parameter_definitions["development.critical_period_start"] = {
            "type": float,
            "range": (0.0, 1000.0),
            "required": False,
            "units": "time_step"
        }
        
        self.parameter_definitions["development.critical_period_end"] = {
            "type": float,
            "range": (0.0, 1000.0),
            "required": False,
            "units": "time_step"
        }
    
    def validate_consistency(self) -> bool:
        """
        Validate configuration consistency.
        
        Returns:
            bool: True if configuration is consistent, False otherwise
        """
        issues = []
        
        # Check required parameters
        for param, definition in self.parameter_definitions.items():
            if definition.get("required", False):
                if not self.config.has(param):
                    issues.append(f"Missing required parameter: {param}")
                else:
                    # Validate parameter type and range
                    value = self.config.get(param)
                    if "range" in definition:
                        if definition["type"] == int:
                            if not (definition["range"][0] <= value <= definition["range"][1]):
                                issues.append(
                                    f"Parameter {param}: value {value} outside range "
                                    f"{definition['range']}"
                                )
                        elif definition["type"] == float:
                            if not (definition["range"][0] <= value <= definition["range"][1]):
                                issues.append(
                                    f"Parameter {param}: value {value} outside range "
                                    f"{definition['range']}"
                                )
        
        # Check parameter relationships
        if self.config.has("development.critical_period_end"):
            end_val = self.config.get("development.critical_period_end")
            if self.config.has("development.critical_period_start"):
                start_val = self.config.get("development.critical_period_start")
                if end_val < start_val:
                    issues.append(
                        "Development critical period end before start"
                    )
        
        # Check STDP consistency
        if not self.config.has("plasticity.stdp.enable") or \
           self.config.get("plasticity.stdp.enable"):
            if not self.config.has("plasticity.stdp.learning_rate"):
                issues.append(
                    "STDP enabled but missing learning rate"
                )
        
        if issues:
            print("Configuration consistency issues found:")
            for issue in issues:
                print(f"  - {issue}")
            return False
        
        return True
    
    def validate_completeness(self) -> bool:
        """
        Validate configuration completeness.
        
        Returns:
            bool: True if configuration is complete, False otherwise
        """
        # Check if all required parameters have reasonable defaults
        required_params = ["brain.neuron_count"]
        
        for param in required_params:
            if not self.config.has(param):
                print(f"Missing essential parameter: {param}")
                return False
        
        # Check if essential groups have at least one parameter set
        plasticity_set = (
            self.config.has("plasticity.stdp.enable") or
            self.config.has("plasticity.stdp.learning_rate")
        )
        
        neuromodulation_set = (
            self.config.has("neuromod.dopamine.scale") or
            self.config.has("neuromod.curiosity.enable")
        )
        
        development_set = (
            self.config.has("development.enable") or
            self.config.has("development.critical_period_start")
        )
        
        if not (plasticity_set and neuromodulation_set and development_set):
            print("Warning: Some advanced feature groups are not fully configured")
            print("  - Plasticity: enabled or learning_rate set")
            print("  - Neuromodulation: dopamine scale or curiosity enabled")
            print("  - Development: enable or critical period set")
        
        return True
    
    def validate_against_schema(self):
        """
        Validate configuration against research schema.
        
        Returns:
            dict: Validation result with passed flag and issues
        """
        result = {
            "passed": True,
            "issues": [],
            "warnings": []
        }
        
        # Schema validation checks
        if self.config.has("brain.neuron_count"):
            neuron_count = self.config.get("brain.neuron_count")
            if neuron_count > 10000:  # Large networks
                result["warnings"].append(
                    f"Large network with {neuron_count} neurons may require "
                    "significant computational resources"
                )
        
        # Check for impossible combinations
        if self.config.has("plasticity.stdp.enable") and \
           self.config.get("plasticity.stdp.enable"):
            if not self.config.has("plasticity.stdp.learning_rate"):
                result["issues"].append(
                    "STDP enabled but learning rate not specified"
                )
                result["passed"] = False
        
        # Check neuromodulation consistency
        if self.config.has("neuromod.curiosity.enable") and \
           self.config.get("neuromod.curiosity.enable"):
            if not self.config.has("neuromod.dopamine.scale"):
                result["warnings"].append(
                    "Curiosity enabled but dopamine scale not specified"
                )
        
        if result["issues"] or result["warnings"]:
            print("Schema validation results:")
            if result["warnings"]:
                print("Warnings:")
                for warning in result["warnings"]:
                    print(f"  - {warning}")
            if result["issues"]:
                print("Errors:")
                for issue in result["issues"]:
                    print(f"  - {issue}")
        
        return result
    
    def set_parameter(self, param: str, value, units=None):
        """
        Set a research parameter with validation.
        
        Args:
            param: Parameter name (e.g., "brain.neuron_count")
            value: Parameter value
            units: Parameter units (optional)
        """
        # Validate parameter exists in definitions
        if param not in self.parameter_definitions:
            print(f"Warning: Unknown parameter '{param}' - not in definitions")
        
        # Validate value type
        if param in self.parameter_definitions:
            expected_type = self.parameter_definitions[param]["type"]
            if not isinstance(value, expected_type):
                try:
                    # Try to convert
                    if expected_type == int:
                        value = int(value)
                    elif expected_type == float:
                        value = float(value)
                    else:
                        raise ValueError(f"Type mismatch: expected {expected_type}")
                except:
                    raise ValueError(
                        f"Type mismatch for {param}: "
                        f"expected {expected_type}, got {type(value)}"
                    )
            
            # Validate range if specified
            if "range" in self.parameter_definitions[param]:
                range_min, range_max = self.parameter_definitions[param]["range"]
                if value < range_min or value > range_max:
                    raise ValueError(
                        f"Value {value} for {param} outside valid range "
                        f"[{range_min}, {range_max}]"
                    )
        
        # Set the parameter
        self.config.set(param, value)
        
        # Record in history
        self.config_history.append({
            "timestamp": datetime.now().isoformat(),
            "parameter": param,
            "value": value,
            "units": units or self.parameter_definitions.get(param, {}).get("units", "")
        })
        
        # Update reproducibility hash
        self._update_reproducibility_hash()
    
    def _update_reproducibility_hash(self):
        """Update the reproducibility hash for this configuration."""
        # Create a hash of the configuration state
        config_string = json.dumps(dict(self.config.getKeys()), sort_keys=True)
        hash_obj = hashlib.sha256(config_string.encode())
        self.reproducibility_hashes.append(hash_obj.hexdigest())
    
    def get_reproducibility_hash(self) -> str:
        """
        Get the current reproducibility hash.
        
        Returns:
            str: Hash string for reproducibility
        """
        if not self.reproducibility_hashes:
            self._update_reproducibility_hash()
        return self.reproducibility_hashes[-1] if self.reproducibility_hashes else ""
    
    def save_to_file(self, filepath: str):
        """
        Save configuration to file with validation.
        
        Args:
            filepath: Path to save configuration
        """
        if not self.validate_completeness():
            raise ValidationError("Configuration is incomplete - cannot save")
        
        if not self.validate_consistency():
            raise ValidationError("Configuration is inconsistent - cannot save")
        
        # Create metadata
        metadata = {
            "timestamp": datetime.now().isoformat(),
            "description": "Research configuration saved via NLM Python API",
            "reproducibility_hash": self.get_reproducibility_hash(),
            "parameter_history": self.config_history[-10:]  # Last 10 changes
        }
        
        # Save config with metadata
        full_config = {
            "metadata": metadata,
            "parameters": {}
        }
        
        for key in self.config.getKeys():
            full_config["parameters"][key] = self.config.get(key)
        
        with open(filepath, 'w') as f:
            json.dump(full_config, f, indent=2)
        
        print(f"Configuration saved to {filepath}")
        print(f"Reproducibility hash: {self.get_reproducibility_hash()[:16]}...")
    
    def load_from_file(self, filepath: str):
        """
        Load configuration from file with validation.
        
        Args:
            filepath: Path to load configuration from
        """
        try:
            with open(filepath, 'r') as f:
                full_config = json.load(f)
            
            # Check metadata
            if "metadata" in full_config:
                metadata = full_config["metadata"]
                print(f"Loaded configuration saved: {metadata.get('timestamp', 'unknown')}")
                print(f"Original hash: {metadata.get('reproducibility_hash', 'N/A')[:16]}...")
            
            # Load parameters
            parameters = full_config.get("parameters", {})
            
            # Clear current config
            self.config.clear()
            
            # Load parameters
            for key, value in parameters.items():
                self.config.set(key, value)
            
            # Update history
            self.config_history.append({
                "timestamp": datetime.now().isoformat(),
                "action": "load",
                "source_file": filepath
            })
            
            print(f"Configuration loaded from {filepath}")
            print(f"Loaded {len(parameters)} parameters")
            
        except Exception as e:
            raise ValidationError(f"Failed to load configuration: {e}")
    
    def create_fallback_config(self):
        """
        Create a fallback configuration when primary config fails.
        
        Returns:
            Config: A safe, working configuration
        """
        fallback_config = nlm.createDefaultConfig()
        
        # Set minimal working configuration
        fallback_config.set("brain.neuron_count", 1000)
        fallback_config.set("brain.region_count", 2)
        fallback_config.set("brain.connection_probability", 0.05)
        fallback_config.set("plasticity.stdp.enable", True)
        fallback_config.set("plasticity.stdp.learning_rate", 0.01)
        fallback_config.set("neuromod.dopamine.scale", 1.0)
        fallback_config.set("neuromod.curiosity.enable", True)
        fallback_config.set("development.enable", True)
        
        self.config_history.append({
            "timestamp": datetime.now().isoformat(),
            "action": "created_fallback"
        })
        
        print("Created fallback configuration")
        return fallback_config
    
    def get_config_summary(self) -> str:
        """
        Get a summary of the current configuration.
        
        Returns:
            str: Configuration summary
        """
        lines = ["Research Configuration Summary:"]
        lines.append(f"  Parameters set: {len(self.config.getKeys())}")
        lines.append(f"  Parameter history size: {len(self.config_history)}")
        lines.append(f"  Reproducibility hashes: {len(self.reproducibility_hashes)}")
        
        if self.config.has("brain.neuron_count"):
            lines.append(f"  Neuron count: {self.config.get('brain.neuron_count')}")
        
        if self.config.has("plasticity.stdp.enable"):
            lines.append(f"  STDP enabled: {self.config.get('plasticity.stdp.enable')}")
        
        if self.config.has("neuromod.dopamine.scale"):
            lines.append(f"  Dopamine scale: {self.config.get('neuromod.dopamine.scale')}")
        
        if self.config.has("development.enable"):
            lines.append(f"  Development enabled: {self.config.get('development.enable')}")
        
        # Configuration quality assessment
        consistency = self.validate_consistency()
        completeness = self.validate_completeness()
        
        lines.append(f"\nConfiguration Quality:")
        lines.append(f"  Consistent: {'✓' if consistency else '✗'}")
        lines.append(f"  Complete: {'✓' if completeness else '✗'}")
        
        return "\n".join(lines)
    
    def compare_with(self, other_config_manager):
        """
        Compare this configuration with another.
        
        Args:
            other_config_manager: Another ResearchConfigManager instance
            
        Returns:
            dict: Comparison results
        """
        self_keys = set(self.config.getKeys())
        other_keys = set(other_config_manager.config.getKeys())
        
        differences = {}
        
        # Find differences
        for key in self_keys:
            if key in other_keys:
                if self.config.get(key) != other_config_manager.config.get(key):
                    differences[key] = {
                        "self": self.config.get(key),
                        "other": other_config_manager.config.get(key)
                    }
        
        for key in other_keys:
            if key not in self_keys:
                differences[key] = {
                    "self": "N/A",
                    "other": other_config_manager.config.get(key)
                }
        
        return {
            "total_parameters": len(self_keys),
            "differences": len(differences),
            "difference_details": differences
        }
def load_robust_config(filepath: str):
    """
    Robust configuration loading with comprehensive error handling.
    
    Args:
        filepath: Path to configuration file
        
    Returns:
        Config: Loaded configuration or fallback
    """
    print(f"\n=== Loading Research Configuration ===")
    print(f"File: {filepath}")
    
    try:
        config_manager = ResearchConfigManager()
        
        # Validate file path
        if not config_manager.config.validateFilePath(filepath):
            raise ValidationError(f"Invalid file path: {filepath}")
            
        # Load with validation
        config_manager.load_from_file(filepath)
        
        # Validate content
        if not config_manager.validateCompleteness():
            raise ValidationError("Configuration is incomplete")
            
        # Validate consistency
        if not config_manager.validateConsistency():
            raise ValidationError("Configuration is inconsistent")
        
        print("✓ Configuration loaded successfully")
        print(f"✓ Quality: {config_manager.get_config_summary()}")
        return config_manager
        
    except ValidationError as e:
        print(f"✗ Configuration error: {e}")
        return create_fallback_config_manager()
    except Exception as e:
        print(f"✗ Unexpected error: {e}")
        return create_fallback_config_manager()


def create_fallback_config_manager():
    """
    Create a fallback configuration manager.
    
    Returns:
        ResearchConfigManager: A safe, working configuration manager
    """
    print("Creating fallback configuration...")
    config_manager = ResearchConfigManager()
    
    # Set safe defaults
    config_manager.set_parameter("brain.neuron_count", 5000)
    config_manager.set_parameter("brain.region_count", 2)
    config_manager.set_parameter("brain.connection_probability", 0.05)
    config_manager.set_parameter("plasticity.stdp.enable", True)
    config_manager.set_parameter("plasticity.stdp.learning_rate", 0.01)
    config_manager.set_parameter("neuromod.dopamine.scale", 1.0)
    config_manager.set_parameter("neuromod.curiosity.enable", True)
    config_manager.set_parameter("development.enable", True)
    
    return config_manager

def run_research_simulation():
    """Run a comprehensive research simulation with custom configuration."""
    print("=== Research Simulation Example ===")
    print(f"Started: {datetime.now()}")
    print()
    
    # Method 1: Create configuration from scratch with validation
    print("1. Creating research configuration from scratch...")
    config_manager = ResearchConfigManager()
    
    # Set research-specific parameters
    config_manager.set_parameter("brain.neuron_count", 10000)
    config_manager.set_parameter("brain.synapse_density", 0.05)
    config_manager.set_parameter("brain.region_count", 4)
    config_manager.set_parameter("plasticity.stdp.learning_rate", 0.0005)
    config_manager.set_parameter("plasticity.stdp.time_constant", 30.0)
    config_manager.set_parameter("plasticity.stdp.enable", True)
    config_manager.set_parameter("neuromod.dopamine.scale", 2.0)
    config_manager.set_parameter("neuromod.serotonin.modulation", 0.3)
    config_manager.set_parameter("neuromod.curiosity.enable", True)
    config_manager.set_parameter("neuromod.curiosity.prediction_error_weight", 0.9)
    config_manager.set_parameter("development.enable", True)
    config_manager.set_parameter("development.critical_period_start", 200.0)
    config_manager.set_parameter("development.critical_period_end", 800.0)
    
    print(f"   Configuration quality check:")
    print(f"   - Consistent: {'✓' if config_manager.validate_consistency() else '✗'}")
    print(f"   - Complete: {'✓' if config_manager.validate_completeness() else '✗'}")
    
    # Validate against schema
    schema_validation = config_manager.validate_against_schema()
    print(f"   - Schema validation: {'✓' if schema_validation['passed'] else '✗'}")
    
    # Save configuration for reproducibility
    config_manager.save_to_file("research_config.json")
    
    # Method 2: Load and validate again
    print("\n2. Loading and validating configuration...")
    loaded_config = ResearchConfigManager()
    loaded_config.load_from_file("research_config.json")
    
    validation = loaded_config.validate_against_schema()
    print(f"   Validation passed: {validation['passed']}")
    print(f"   Issues found: {len(validation['issues'])}")
    print(f"   Warnings: {len(validation['warnings'])}")
    
    # Method 3: Robust configuration loading with error handling
    print("\n3. Testing robust configuration loading...")
    robust_config = load_robust_config("research_config.json")
    
    # Show configuration summary
    print("\n=== Final Configuration Summary ===")
    print(robust_config.get_config_summary())
    
    # Demonstrate configuration comparison
    print("\n=== Configuration Comparison ===")
    config_a = ResearchConfigManager()
    config_a.set_parameter("brain.neuron_count", 5000)
    config_a.set_parameter("plasticity.stdp.learning_rate", 0.01)
    
    config_b = ResearchConfigManager()
    config_b.set_parameter("brain.neuron_count", 10000)
    config_b.set_parameter("plasticity.stdp.learning_rate", 0.005)
    
    comparison = config_a.compare_with(config_b)
    print(f"Total parameters compared: {comparison['total_parameters']}")
    print(f"Differences found: {comparison['differences']}")
    if comparison['differences'] > 0:
        print("Key differences:")
        for param, diff in comparison['difference_details'].items():
            print(f"  {param}: {diff['self']} vs {diff['other']}")
    
    print(f"\n=== Research Simulation Complete ===")
    print(f"Completed at: {datetime.now()}")

if __name__ == "__main__":
    run_research_simulation()
