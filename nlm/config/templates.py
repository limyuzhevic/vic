#!/usr/bin/env python3
"""
NLM Config Templates - Configuration Templates

This module provides template-based configuration management.
"""

__version__ = "1.0.0"
__author__ = "NLM Team"

from typing import Dict, List, Optional, Any, Union
import json
import os
import time
from pathlib import Path
from nlm.config.manager import ConfigManager, ConfigTemplate

# Re-export config template classes
__all__ = [
    "ConfigManager",
    "ConfigTemplate",
    "ConfigError",
    "__version__",
    "__author__"
]

# Template definitions
STANDARD_TEMPLATES = {
    "small_neural_net": {
        "name": "Small Neural Network",
        "description": "A small neural network with 1K neurons",
        "template": {
            "neuron_count": 1000,
            "connection_probability": 0.1,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "tau_mem": 20.0,
            "dt": 0.1,
            "simulation_steps": 1000,
            "random_seed": 42
        }
    },
    "medium_neural_net": {
        "name": "Medium Neural Network",
        "description": "A medium neural network with 10K neurons",
        "template": {
            "neuron_count": 10000,
            "connection_probability": 0.1,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "tau_mem": 20.0,
            "dt": 0.1,
            "simulation_steps": 5000,
            "random_seed": 42
        }
    },
    "large_neural_net": {
        "name": "Large Neural Network",
        "description": "A large neural network with 100K neurons",
        "template": {
            "neuron_count": 100000,
            "connection_probability": 0.05,
            "v_thresh": -55.0,
            "v_rest": -70.0,
            "tau_mem": 25.0,
            "dt": 0.05,
            "simulation_steps": 10000,
            "random_seed": 42
        }
    },
    "activity_detection": {
        "name": "Activity Detection",
        "description": "Optimized for detecting neural activity patterns",
        "template": {
            "neuron_count": 5000,
            "connection_probability": 0.15,
            "v_thresh": -40.0,
            "v_rest": -60.0,
            "tau_mem": 15.0,
            "dt": 0.1,
            "simulation_steps": 2000,
            "random_seed": 123,
            "enable_activity_detection": True,
            "activity_threshold": 5.0
        }
    },
    "learning_optimization": {
        "name": "Learning Optimization",
        "description": "Optimized for learning and plasticity",
        "template": {
            "neuron_count": 8000,
            "connection_probability": 0.08,
            "v_thresh": -50.0,
            "v_rest": -70.0,
            "tau_mem": 18.0,
            "dt": 0.1,
            "simulation_steps": 8000,
            "random_seed": 456,
            "plasticity_rate": 0.01,
            "learning_rate": 0.05
        }
    }
}

# Helper functions for template management

def create_standard_template(name: str) -> ConfigTemplate:
    """Create a standard template by name.
    
    Args:
        name: Template name (small_neural_net, medium_neural_net, etc.)
        
    Returns:
        ConfigTemplate object
    """
    if name not in STANDARD_TEMPLATES:
        raise ValueError(f"Unknown template: {name}")
    
    template_info = STANDARD_TEMPLATES[name]
    
    # Create template
    template = ConfigTemplate()
    
    # Set variables from template configuration
    config = template_info["template"]
    for key, value in config.items():
        template.set_variable(key, value, f"{template_info['name']} parameter: {key}")
    
    # Add key parameters
    key_params = ["neuron_count", "connection_probability", "v_thresh", "v_rest"]
    for param in key_params:
        if param in config:
            template.add_parameter(param, config[param], f"Parameter {param}")
    
    return template

def get_available_templates() -> List[str]:
    """Get list of available standard templates.
    
    Returns:
        List of template names
    """
    return list(STANDARD_TEMPLATES.keys())

def get_template_info(name: str) -> Dict[str, Any]:
    """Get template information.
    
    Args:
        name: Template name
        
    Returns:
        Template information dictionary
    """
    if name not in STANDARD_TEMPLATES:
        raise ValueError(f"Unknown template: {name}")
    
    return STANDARD_TEMPLATES[name].copy()

def create_custom_template(config: Dict[str, Any], name: str = "custom_template") -> ConfigTemplate:
    """Create a custom template from configuration.
    
    Args:
        config: Configuration dictionary
        name: Template name
        
    Returns:
        Custom ConfigTemplate object
    """
    template = ConfigTemplate()
    
    # Set variables from config
    for key, value in config.items():
        if isinstance(value, (int, float, str, bool)):
            template.set_variable(key, value, f"Custom template parameter: {key}")
    
    # Add key parameters
    key_params = ["neuron_count", "connection_probability", "v_thresh", "v_rest"]
    for param in key_params:
        if param in config:
            template.add_parameter(param, config[param], f"Custom parameter {param}")
    
    return template

def export_template(template: ConfigTemplate, output_file: str):
    """Export template to file.
    
    Args:
        template: Template to export
        output_file: Output file path
    """
    config = template.render()
    
    # Add metadata
    export_data = {
        "name": "exported_template",
        "version": "1.0",
        "created": time.strftime("%Y-%m-%d %H:%M:%S"),
        "config": config,
        "variables": template.get_template_variables(),
        "parameters": template.get_template_parameters()
    }
    
    with open(output_file, 'w') as f:
        json.dump(export_data, f, indent=2)
    
    print(f"Template exported to: {output_file}")

def import_template(input_file: str) -> ConfigTemplate:
    """Import template from file.
    
    Args:
        input_file: Input file path
        
    Returns:
        Imported ConfigTemplate object
    """
    with open(input_file, 'r') as f:
        import_data = json.load(f)
    
    # Create template
    template = ConfigTemplate()
    
    # Load variables
    if "variables" in import_data:
        for var_name, var_info in import_data["variables"].items():
            template.set_variable(var_name, var_info, f"Imported variable: {var_name}")
    
    # Load parameters
    if "parameters" in import_data:
        for param_name, param_info in import_data["parameters"].items():
            template.add_parameter(param_name, param_info, f"Imported parameter: {param_name}")
    
    return template

if __name__ == "__main__":
    # Example usage
    print("Running Config Templates Example")
    
    # Create config manager
    config_manager = ConfigManager()
    
    # Create standard templates
    small_template = create_standard_template("small_neural_net")
    medium_template = create_standard_template("medium_neural_net")
    
    # Get template info
    small_info = get_template_info("small_neural_net")
    print(f"Small template: {small_info['name']} - {small_info['description']}")
    
    # List available templates
    print("Available templates:", get_available_templates())
    
    # Create custom template
    custom_config = {
        "neuron_count": 3000,
        "connection_probability": 0.12,
        "v_thresh": -45.0,
        "v_rest": -65.0,
        "simulation_steps": 3000,
        "plasticity_rate": 0.02
    }
    
    custom_template = create_custom_template(custom_config, "my_custom_template")
    print(f"Created custom template with {len(custom_template.get_template_variables())} variables")
    
    # Render template
    rendered_config = medium_template.render()
    print(f"Rendered template config: {len(rendered_config)} parameters")
    
    # Create config manager instance and use templates
    config_manager = ConfigManager()
    
    # Save templates
    config_manager.save_template("small_template", rendered_config)
    config_manager.save_template("medium_template", rendered_config)
    
    # Load and use templates
    loaded_template = config_manager.load_template("medium_template")
    created_config = config_manager.create_from_template("medium_template", 
                                                         {"neuron_count": 15000})
    
    print(f"Created config from template: {created_config}")
    
    # Export and import template
    export_template(custom_template, "custom_template.json")
    imported_template = import_template("custom_template.json")
    print(f"Imported template has {len(imported_template.get_template_variables())} variables")
    
    print("Config templates example completed!")
