#!/usr/bin/env python3
"""
Advanced Python Configuration Management

This example demonstrates advanced Python configuration management techniques
for NLM, including configuration validation, parameter optimization, and
custom configuration profiles.
"""

import pynlm
import json
import yaml
import numpy as np
from typing import Dict, List, Any, Optional

class NLMConfigurator:
    """Advanced NLM configuration manager."""
    
    def __init__(self):
        self.config_templates = {}
        self.validation_rules = {}
        self.optimization_history = []
    
    def create_configuration_profile(self, name: str, parameters: Dict[str, Any]) -> pynlm.Config:
        """Create a configuration profile from parameter dictionary."""
        
        print(f"Creating configuration profile: {name}")
        
        config = pynlm.createDefaultConfig()
        
        # Apply parameters with validation
        for key, value in parameters.items():
            try:
                self._validate_parameter(key, value)
                config.set(key, value)
                print(f"  ✓ Set {key} = {value}")
            except ValueError as e:
                print(f"  ✗ Error setting {key}: {e}")
                raise
        
        return config
    
    def _validate_parameter(self, key: str, value: Any):
        """Validate parameter value."""
        validation_rules = {
            'brain.neuron_count': lambda v: isinstance(v, int) and 100 <= v <= 10000,
            'brain.synapse_density': lambda v: isinstance(v, (int, float)) and 0.01 <= v <= 0.3,
            'brain.v_thresh': lambda v: isinstance(v, (int, float)) and -100 <= v <= 0,
            'brain.tau_mem': lambda v: isinstance(v, (int, float)) and 1 <= v <= 100,
            'plasticity.stdp.learning_rate': lambda v: isinstance(v, (int, float)) and 0.00001 <= v <= 0.1,
            'neuromod.curiosity.enable': lambda v: isinstance(v, bool),
            'working.memory.capacity': lambda v: isinstance(v, int) and 10 <= v <= 500
        }
        
        if key in validation_rules:
            if not validation_rules[key](value):
                raise ValueError(f"Invalid value {value} for parameter {key}")
    
    def load_configuration_from_file(self, filepath: str, format: str = 'json') -> pynlm.Config:
        """Load configuration from file with format detection."""
        
        print(f"Loading configuration from {filepath} ({format})")
        
        if format.lower() == 'json':
            with open(filepath, 'r') as f:
                params = json.load(f)
        elif format.lower() == 'yaml':
            with open(filepath, 'r') as f:
                params = yaml.safe_load(f)
        elif format.lower() == 'toml':
            try:
                import tomli
                with open(filepath, 'rb') as f:
                    params = tomli.load(f)
            except ImportError:
                raise ImportError("tomli package required for TOML files")
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        return self.create_configuration_profile("loaded_config", params)
    
    def save_configuration_to_file(self, config: pynlm.Config, filepath: str, format: str = 'json'):
        """Save configuration to file in specified format."""
        
        print(f"Saving configuration to {filepath} ({format})")
        
        params = self._config_to_dict(config)
        
        if format.lower() == 'json':
            with open(filepath, 'w') as f:
                json.dump(params, f, indent=2)
        elif format.lower() == 'yaml':
            with open(filepath, 'w') as f:
                yaml.dump(params, f, default_flow_style=False)
        elif format.lower() == 'toml':
            try:
                import tomli_w
                with open(filepath, 'wb') as f:
                    tomli_w.dump(params, f)
            except ImportError:
                raise ImportError("tomli-w package required for TOML files")
        else:
            raise ValueError(f"Unsupported format: {format}")
        
        print(f"  ✓ Configuration saved to {filepath}")
    
    def _config_to_dict(self, config: pynlm.Config) -> Dict[str, Any]:
        """Convert configuration to dictionary."""
        params = {}
        for key in config.getKeys():
            params[key] = config.get(key)
        return params
    
    def optimize_configuration(self, target_performance: float, trials: int = 100) -> Dict[str, Any]:
        """Optimize configuration using simple random search."""
        
        print(f"Optimizing configuration for target performance: {target_performance}")
        print(f"Running {trials} optimization trials...")
        
        best_params = {}
        best_score = 0.0
        
        for trial in range(trials):
            # Generate random configuration
            params = {
                'brain.neuron_count': np.random.choice([500, 1000, 2000, 5000]),
                'brain.synapse_density': np.random.uniform(0.05, 0.2),
                'plasticity.stdp.learning_rate': np.random.uniform(0.001, 0.01),
                'neuromod.curiosity.enable': np.random.choice([True, False]),
                'working.memory.capacity': np.random.choice([100, 200, 500, 1000])
            }
            
            # Create and test configuration
            config = self.create_configuration_profile(f"trial_{trial}", params)
            
            # Simulate performance (simplified model)
            performance = self._evaluate_configuration_performance(config, params)
            
            if performance > best_score:
                best_score = performance
                best_params = params.copy()
                print(f"    Trial {trial+1}: Score {performance:.3f} (new best)")
            
            self.optimization_history.append({
                'trial': trial,
                'params': params.copy(),
                'performance': performance
            })
        
        print(f"Optimization complete!")
        print(f"  Best score: {best_score:.3f}")
        print(f"  Best parameters: {best_params}")
        
        return best_params
    
    def _evaluate_configuration_performance(self, config: pynlm.Config, params: Dict[str, Any]) -> float:
        """Evaluate configuration performance (simplified model)."""
        
        # Simple performance model based on parameters
        neuron_score = min(params['brain.neuron_count'] / 5000, 1.0)
        synapse_score = params['brain.synapse_density']
        learning_score = params['plasticity.stdp.learning_rate'] / 0.01
        curiosity_score = 1.0 if params['neuromod.curiosity.enable'] else 0.5
        memory_score = min(params['working.memory.capacity'] / 1000, 1.0)
        
        # Combined performance score
        performance = (
            neuron_score * 0.2 +
            synapse_score * 0.2 +
            learning_score * 0.2 +
            curiosity_score * 0.2 +
            memory_score * 0.2
        )
        
        # Add some randomness for realism
        performance *= np.random.uniform(0.8, 1.2)
        
        return performance
    
    def compare_configurations(self, config1: pynlm.Config, config2: pynlm.Config, name1: str, name2: str) -> Dict[str, Any]:
        """Compare two configurations and show differences."""
        
        print(f"Comparing configurations: {name1} vs {name2}")
        
        params1 = self._config_to_dict(config1)
        params2 = self._config_to_dict(config2)
        
        all_keys = set(params1.keys()) | set(params2.keys())
        differences = {}
        
        for key in sorted(all_keys):
            val1 = params1.get(key, 'N/A')
            val2 = params2.get(key, 'N/A')
            
            if val1 != val2:
                differences[key] = {'value1': val1, 'value2': val2}
                print(f"  {key}:")
                print(f"    {name1}: {val1}")
                print(f"    {name2}: {val2}")
        
        if not differences:
            print("  ✓ No differences found")
        
        return differences

def main():
    """Main function demonstrating advanced configuration techniques."""
    
    print("=== Advanced NLM Configuration Management Demo ===\n")
    
    # Create configurator
    configurator = NLMConfigurator()
    
    # Example 1: Create custom configuration
    print("1. Creating custom configuration profile...")
    custom_params = {
        'brain.neuron_count': 2000,
        'brain.synapse_density': 0.08,
        'plasticity.stdp.learning_rate': 0.005,
        'neuromod.curiosity.enable': True,
        'working.memory.capacity': 300
    }
    
    custom_config = configurator.create_configuration_profile("research_config", custom_params)
    print(f"   Created custom config with {custom_config.get('brain.neuron_count')} neurons\n")
    
    # Example 2: Load configuration from JSON
    print("2. Loading configuration from JSON file...")
    
    # Create sample JSON file
    sample_config = {
        'brain.neuron_count': 1500,
        'brain.synapse_density': 0.12,
        'plasticity.stdp.learning_rate': 0.008,
        'neuromod.curiosity.enable': False,
        'working.memory.capacity': 500
    }
    
    with open('sample_config.json', 'w') as f:
        json.dump(sample_config, f, indent=2)
    
    loaded_config = configurator.load_configuration_from_file('sample_config.json', 'json')
    print(f"   Loaded config with {loaded_config.get('brain.neuron_count')} neurons\n")
    
    # Example 3: Save configuration to YAML
    print("3. Saving configuration to YAML file...")
    configurator.save_configuration_to_file(loaded_config, 'saved_config.yaml', 'yaml')
    
    # Example 4: Compare configurations
    print("\n4. Comparing configurations...")
    configurator.compare_configurations(custom_config, loaded_config, "custom", "loaded")
    
    # Example 5: Optimization
    print("\n5. Configuration optimization...")
    optimized_params = configurator.optimize_configuration(target_performance=0.8, trials=10)
    
    # Create optimized configuration
    optimized_config = configurator.create_configuration_profile("optimized_config", optimized_params)
    print(f"   Created optimized config with {optimized_config.get('brain.neuron_count')} neurons\n")
    
    # Example 6: File format conversion
    print("6. Converting configuration file formats...")
    
    # Save to JSON, then load as YAML (demonstrating format conversion)
    configurator.save_configuration_to_file(custom_config, 'converted_config.json', 'json')
    yaml_config = configurator.load_configuration_from_file('converted_config.json', 'json')
    configurator.save_configuration_to_file(yaml_config, 'converted_config.yaml', 'yaml')
    
    print("   ✓ Configuration converted between formats")
    
    print("\n=== Configuration Management Demo Complete ===")
    print("\nThe advanced configuration manager provides:")
    print("  • Validation of configuration parameters")
    print("  • Multiple file format support (JSON, YAML, TOML)")
    print("  • Configuration optimization using random search")
    print("  • Comparison tools for different configurations")
    print("  • Profile management for reuse")
    print("  • Error handling and recovery")

if __name__ == "__main__":
    main()