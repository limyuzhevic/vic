#!/usr/bin/env python3

import json
import os

def create_basic_template():
    """Create a basic configuration template"""
    template = {
        "description": "Basic NLM configuration template",
        "version": "2.0",
        "parameters": {
            "random_seed": {
                "type": "integer64",
                "default": 42,
                "description": "Random seed for reproducible experiments"
            },
            "simulation_timestep": {
                "type": "float",
                "default": 0.001,
                "description": "Simulation timestep in seconds"
            },
            "neuron_count": {
                "type": "integer64",
                "default": 500,
                "description": "Number of neurons in the brain"
            },
            "region_count": {
                "type": "integer64",
                "default": 1,
                "description": "Number of brain regions"
            },
            "connection_probability": {
                "type": "float",
                "default": 0.15,
                "description": "Connection probability for random connectivity"
            }
        }
    }
    
    with open('configs/templates/basic.json', 'w') as f:
        json.dump(template, f, indent=2)
    
    print("Created basic.json template")

def create_advanced_template():
    """Create an advanced configuration template"""
    template = {
        "description": "Advanced NLM configuration template with plasticity",
        "version": "2.0",
        "parameters": {
            "random_seed": {
                "type": "integer64",
                "default": 12345,
                "description": "Random seed for reproducible experiments"
            },
            "simulation_timestep": {
                "type": "float",
                "default": 0.001,
                "description": "Simulation timestep in seconds"
            },
            "neuron_count": {
                "type": "integer64",
                "default": 2000,
                "description": "Number of neurons in the brain"
            },
            "region_count": {
                "type": "integer64",
                "default": 2,
                "description": "Number of brain regions"
            },
            "connection_probability": {
                "type": "float",
                "default": 0.2,
                "description": "Connection probability for random connectivity"
            },
            "stdp_ltp_weight": {
                "type": "float",
                "default": 0.02,
                "description": "STDP LTP weight"
            },
            "stdp_ltd_weight": {
                "type": "float",
                "default": 0.015,
                "description": "STDP LTD weight"
            },
            "stdp_tau": {
                "type": "float",
                "default": 20.0,
                "description": "STDP time constant"
            },
            "synaptogenesis_rate": {
                "type": "float",
                "default": 0.0001,
                "description": "Synaptogenesis rate"
            },
            "pruning_rate": {
                "type": "float",
                "default": 0.00001,
                "description": "Pruning rate"
            },
            "environment_name": {
                "type": "string",
                "default": "GridWorld",
                "description": "Environment name"
            },
            "environment_width": {
                "type": "integer64",
                "default": 20,
                "description": "Environment width"
            },
            "environment_height": {
                "type": "integer64",
                "default": 20,
                "description": "Environment height"
            },
            "visualization_enabled": {
                "type": "boolean",
                "default": false,
                "description": "Enable visualization"
            },
            "visualization_update_rate": {
                "type": "float",
                "default": 60.0,
                "description": "Visualization update rate in Hz"
            }
        }
    }
    
    with open('configs/templates/advanced.json', 'w') as f:
        json.dump(template, f, indent=2)
    
    print("Created advanced.json template")

def create_debug_template():
    """Create a debug configuration template"""
    template = {
        "description": "Debug NLM configuration template for testing",
        "version": "2.0",
        "parameters": {
            "random_seed": {
                "type": "integer64",
                "default": 999,
                "description": "Random seed for reproducible experiments"
            },
            "simulation_timestep": {
                "type": "float",
                "default": 0.01,
                "description": "Simulation timestep in seconds"
            },
            "neuron_count": {
                "type": "integer64",
                "default": 100,
                "description": "Number of neurons in the brain"
            },
            "region_count": {
                "type": "integer64",
                "default": 1,
                "description": "Number of brain regions"
            },
            "connection_probability": {
                "type": "float",
                "default": 0.3,
                "description": "Connection probability for random connectivity"
            },
            "visualization_enabled": {
                "type": "boolean",
                "default": true,
                "description": "Enable visualization for debugging"
            },
            "log_level": {
                "type": "string",
                "default": "Debug",
                "description": "Logging level (Debug, Info, Warning, Error)"
            },
            "max_simulation_steps": {
                "type": "integer64",
                "default": 1000,
                "description": "Maximum simulation steps"
            }
        }
    }
    
    with open('configs/templates/debug.json', 'w') as f:
        json.dump(template, f, indent=2)
    
    print("Created debug.json template")

def create_preset_examples():
    """Create example preset configuration files"""
    
    # Quick test preset
    quick_test = {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 100,
        "region_count": 1,
        "connection_probability": 0.1,
        "stdp_ltp_weight": 0.01,
        "stdp_ltd_weight": 0.012,
        "stdp_tau": 10.0,
        "visualization_enabled": false
    }
    
    with open('configs/presets/quick_test.json', 'w') as f:
        json.dump(quick_test, f, indent=2)
    
    # Full simulation preset
    full_sim = {
        "random_seed": 12345,
        "simulation_timestep": 0.001,
        "neuron_count": 5000,
        "region_count": 3,
        "connection_probability": 0.15,
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "synaptogenesis_rate": 0.0001,
        "pruning_rate": 0.00001,
        "environment_name": "ComplexWorld",
        "environment_width": 50,
        "environment_height": 50,
        "visualization_enabled": true,
        "visualization_update_rate": 30.0,
        "log_level": "Info",
        "log_to_file": true,
        "log_filename": "nlm_simulation.log",
        "max_simulation_steps": 100000,
        "simulation_time_limit": 0.0
    }
    
    with open('configs/presets/full_simulation.json', 'w') as f:
        json.dump(full_sim, f, indent=2)
    
    # Research experiment preset
    research = {
        "random_seed": 999,
        "simulation_timestep": 0.0005,
        "neuron_count": 2000,
        "region_count": 2,
        "connection_probability": 0.2,
        "stdp_ltp_weight": 0.025,
        "stdp_ltd_weight": 0.01,
        "stdp_tau": 15.0,
        "synaptogenesis_rate": 0.00005,
        "pruning_rate": 0.00002,
        "environment_name": "CorticalColumns",
        "environment_width": 30,
        "environment_height": 30,
        "visualization_enabled": false,
        "log_level": "Debug",
        "log_to_file": true,
        "log_filename": "nlm_research.log",
        "max_simulation_steps": 50000,
        "simulation_time_limit": 1000.0
    }
    
    with open('configs/presets/research.json', 'w') as f:
        json.dump(research, f, indent=2)
    
    print("Created preset configuration files")

def main():
    # Create directories
    os.makedirs('configs/templates', exist_ok=True)
    os.makedirs('configs/presets', exist_ok=True)
    
    # Create templates
    create_basic_template()
    create_advanced_template()
    create_debug_template()
    
    # Create preset examples
    create_preset_examples()
    
    print("\nConfiguration templates have been created:")
    print("  - configs/templates/basic.json")
    print("  - configs/templates/advanced.json")
    print("  - configs/templates/debug.json")
    print("  - configs/presets/quick_test.json")
    print("  - configs/presets/full_simulation.json")
    print("  - configs/presets/research.json")
    
    print("\nEach template includes parameter descriptions and validation rules.")
    print("The presets can be used as starting points for specific use cases.")

if __name__ == "__main__":
    main()
