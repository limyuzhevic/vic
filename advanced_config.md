# Advanced Configuration Options and Commands

## Overview

This document provides comprehensive guidance on NLM's advanced configuration options, command-line tools, and programmatic configuration capabilities. It covers everything from basic settings to advanced features for power users.

## Configuration File Formats

### JSON Configuration

```json
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "initial_weight_mean": 0.5,
        "initial_weight_std": 0.1,
        "v_thresh": -50.0,
        "v_rest": -70.0,
        "v_reset": -75.0,
        "tau_mem": 20.0,
        "tau_ref": 2.0
    },
    "plasticity": {
        "stdp": {
            "enable": true,
            "learning_rate": 0.001,
            "tau_plus": 20.0,
            "tau_minus": 20.0
        },
        "hebbian": {
            "enable": true,
            "learning_rate": 0.01
        },
        "structural": {
            "enable": true,
            "synaptogenesis_rate": 0.0001,
            "pruning_rate": 0.00001
        }
    },
    "neuromodulation": {
        "dopamine": {
            "baseline": 0.1,
            "scale": 1.0
        },
        "curiosity": {
            "enable": true,
            "baseline": 0.5
        },
        "novelty": {
            "enable": true,
            "threshold": 0.5
        }
    },
    "development": {
        "initial_plasticity": 1.0,
        "critical_period_end": 100.0,
        "aging_rate": 0.001
    },
    "memory": {
        "working_memory_size": 100,
        "episodic_capacity": 1000,
        "associative_threshold": 0.7
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8,
        "max_energy": 100.0,
        "energy_decay_rate": 0.01
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000,
        "time_limit": 0.0
    },
    "logging": {
        "level": "INFO",
        "to_file": true,
        "filename": "nlm_simulation.log"
    }
}
```

### YAML Configuration (Alternative Format)

```yaml
brain:
  neuron_count: 1000
  region_count: 2
  connection_probability: 0.1
  initial_weight_mean: 0.5
  initial_weight_std: 0.1

plasticity:
  stdp:
    enable: true
    learning_rate: 0.001
    tau_plus: 20.0
    tau_minus: 20.0
  hebbian:
    enable: true
    learning_rate: 0.01
  structural:
    enable: true
    synaptogenesis_rate: 0.0001
    pruning_rate: 0.00001

neuromodulation:
  dopamine:
    baseline: 0.1
    scale: 1.0
  curiosity:
    enable: true
    baseline: 0.5
  novelty:
    enable: true
    threshold: 0.5

world:
  width: 20
  height: 20
  vision_width: 8
  vision_height: 8
  max_energy: 100.0
  energy_decay_rate: 0.01

simulation:
  timestep: 0.001
  max_steps: 10000
  time_limit: 0.0

logging:
  level: INFO
  to_file: true
  filename: nlm_simulation.log
```

## Command Line Interface

### Basic Usage

```bash
# Run with default configuration
./nlm

# Run with custom configuration file
./nlm --config custom.cfg

# Run with command line overrides
./nlm --config base.cfg --neuron_count 2000 --connection_probability 0.2

# Run specific test
./nlm --test phase2

# Run benchmark
./nlm --benchmark
```

### Configuration Command Line Options

```bash
# Core brain settings
--neuron_count N          Number of neurons (default: 1000)
--region_count R          Number of brain regions (default: 2)
--connection_probability P Connection probability (default: 0.1)
--simulation_timestep T   Simulation timestep in seconds (default: 0.001)

# Plasticity settings
--stdp_enable BOOL        Enable STDP (default: true)
--stdp_learning_rate RATE STDP learning rate (default: 0.001)
--hebbian_enable BOOL     Enable Hebbian learning (default: true)
--structural_enable BOOL  Enable structural plasticity (default: true)

# Neuromodulation
--curiosity_enable BOOL   Enable curiosity (default: true)
--novelty_enable BOOL     Enable novelty detection (default: true)
--dopamine_baseline VAL   Dopamine baseline (default: 0.1)

# Development
--development_enable BOOL Enable developmental stages (default: true)
--critical_period_end VAL Critical period end timestep (default: 100.0)

# Memory systems
--working_memory_size SIZE Working memory capacity (default: 100)
--episodic_capacity CAP    Episodic memory capacity (default: 1000)

# World settings
--world_width W           World width (default: 20)
--world_height H          World height (default: 20)
--vision_width VW         Vision width (default: 8)
--vision_height VH        Vision height (default: 8)

# Logging and debugging
--log_level LEVEL         Log level: DEBUG, INFO, WARN, ERROR (default: INFO)
--log_to_file BOOL         Log to file (default: false)
--log_filename FILENAME   Log filename (default: nlm.log)
--debug_mode BOOL          Enable debug mode (default: false)

# Performance
--random_seed SEED         Random seed for reproducibility (default: 42)
--max_simulation_steps S   Maximum simulation steps (default: 10000)
--performance_mode MODE    Performance mode: normal, optimized, debug (default: normal)
```

### Command Line Examples

```bash
# Run with specific configuration
./nlm --config config.json --log_level DEBUG --log_to_file

# Run with custom parameters
./nlm --neuron_count 2000 --region_count 3 --connection_probability 0.15 \
     --curiosity_enable --novelty_enable \
     --stdp_learning_rate 0.002 --hebbian_learning_rate 0.05

# Run benchmark with specific settings
./nlm --benchmark --neuron_count 500 --simulation_timestep 0.002 \
     --random_seed 123 --performance_mode optimized

# Run experiment with environment
./nlm --test phase6_integration --world_width 30 --world_height 30 \
     --log_to_file --log_filename phase6_results.log
```

## Advanced Configuration Classes

### Configuration Builder Pattern

```python
import nlm
from nlm import ConfigBuilder

# Create configuration using builder pattern
config = (ConfigBuilder()
    .brain(1000, 2, 0.1)
    .plasticity(
        stdp=ConfigBuilder.STDP(learning_rate=0.001),
        hebbian=ConfigBuilder.Hebbian(learning_rate=0.01),
        structural=ConfigBuilder.Structural(synaptogenesis_rate=0.0001)
    )
    .neuromodulation(
        curiosity=ConfigBuilder.Curiosity(enable=True, baseline=0.5),
        novelty=ConfigBuilder.Novelty(enable=True, threshold=0.5),
        dopamine=ConfigBuilder.Dopamine(baseline=0.1)
    )
    .development(
        initial_plasticity=1.0,
        critical_period_end=100.0,
        enable=True
    )
    .world(20, 20, vision_width=8, vision_height=8)
    .simulation(timestep=0.001, max_steps=10000)
    .logging(level="INFO", to_file=True, filename="nlm.log")
    .build())

# Use the configuration
brain = nlm.PythonFactory.createBrain(config)
brain.initialize()
```

### Configuration Template System

```python
from nlm import ConfigTemplate

# Define reusable configuration templates
templates = {
    "small_brain": ConfigTemplate(
        neuron_count=100,
        region_count=1,
        connection_probability=0.05,
        simulation_timestep=0.001
    ),
    
    "large_brain": ConfigTemplate(
        neuron_count=5000,
        region_count=5,
        connection_probability=0.15,
        simulation_timestep=0.001
    ),
    
    "high_plasticity": ConfigTemplate(
        neuron_count=1000,
        plasticity=ConfigTemplate.PlasticityConfig(
            stdp=ConfigTemplate.STDPConfig(learning_rate=0.01),
            hebbian=ConfigTemplate.HebbianConfig(learning_rate=0.05),
            structural=ConfigTemplate.StructuralConfig(
                synaptogenesis_rate=0.001,
                pruning_rate=0.0001
            )
        )
    ),
    
    "learning_focused": ConfigTemplate(
        neuron_count=1000,
        neuromodulation=ConfigTemplate.NeuromodulationConfig(
            curiosity=ConfigTemplate.CuriosityConfig(enable=True, baseline=0.8),
            novelty=ConfigTemplate.NoveltyConfig(enable=True, threshold=0.3)
        ),
        plasticity=ConfigTemplate.PlasticityConfig(
            stdp=ConfigTemplate.STDPConfig(learning_rate=0.01),
            hebbian=ConfigTemplate.HebbianConfig(learning_rate=0.02)
        )
    ),
    
    "realistic_brain": ConfigTemplate(
        neuron_count=1000,
        region_count=3,
        connection_probability=0.12,
        plasticity=ConfigTemplate.PlasticityConfig(
            stdp=ConfigTemplate.STDPConfig(learning_rate=0.005, tau_plus=30.0, tau_minus=30.0),
            hebbian=ConfigTemplate.HebbianConfig(learning_rate=0.01)
        ),
        neuromodulation=ConfigTemplate.NeuromodulationConfig(
            dopamine=ConfigTemplate.DopamineConfig(baseline=0.05, scale=0.8),
            curiosity=ConfigTemplate.CuriosityConfig(enable=True, baseline=0.3),
            novelty=ConfigTemplate.NoveltyConfig(enable=True, threshold=0.4)
        ),
        development=ConfigTemplate.DevelopmentConfig(
            initial_plasticity=1.2,
            critical_period_end=50.0,
            enable=True
        ),
        world=ConfigTemplate.WorldConfig(
            width=20,
            height=20,
            vision_width=10,
            vision_height=10,
            max_energy=50.0
        ),
        simulation=ConfigTemplate.SimulationConfig(
            timestep=0.001,
            max_steps=5000
        )
    )
}

# Use template
config = templates["realistic_brain"].create()
config = templates["learning_focused"].modify(config, neuron_count=2000)
```

### Configuration Validation

```python
from nlm import ConfigValidator

class AdvancedConfigValidator(ConfigValidator):
    def validate_config(self, config):
        errors = []
        
        # Validate brain parameters
        neuron_count = config.get("brain", "neuron_count", 0)
        if neuron_count < 10 or neuron_count > 100000:
            errors.append("Neuron count must be between 10 and 100,000")
        
        connection_prob = config.get("brain", "connection_probability", 0.0)
        if connection_prob < 0.0 or connection_prob > 1.0:
            errors.append("Connection probability must be between 0.0 and 1.0")
        
        # Validate plasticity settings
        if config.get("plasticity", "stdp", {}).get("enable", False):
            stdp_lr = config.get("plasticity", "stdp", "learning_rate", 0.0)
            if stdp_lr <= 0.0:
                errors.append("STDP learning rate must be positive")
        
        # Validate world dimensions
        width = config.get("world", "width", 0)
        height = config.get("world", "height", 0)
        if width < 1 or height < 1:
            errors.append("World dimensions must be positive")
        
        # Validate simulation parameters
        timestep = config.get("simulation", "timestep", 0.0)
        if timestep <= 0.0:
            errors.append("Simulation timestep must be positive")
        
        return errors

# Use validator
validator = AdvancedConfigValidator()
config = nlm.PythonFactory.createDefaultConfig()
errors = validator.validate_config(config)
if errors:
    print("Configuration errors:", errors)
else:
    print("Configuration is valid")
```

## Configuration Management Tools

### Configuration CLI Tool

```bash
#!/bin/bash
# config_tool.sh - Advanced NLM configuration management

usage() {
    echo "Usage: $0 <command> [options]"
    echo "Commands:"
    echo "  create <name>                    Create new configuration"
    echo "  modify <name> [options]          Modify existing configuration"
    echo "  validate <name>                  Validate configuration"
    echo "  export <name> <file>             Export configuration to file"
    echo "  import <file> <name>             Import configuration from file"
    echo "  list                             List all configurations"
    echo "  delete <name>                    Delete configuration"
    echo "  compare <name1> <name2>          Compare two configurations"
    echo "  merge <name1> <name2> <output>   Merge two configurations"
}

# Configuration creation with validation
create_config() {
    local name=$1
    local config_file="configs/${name}.json"
    
    if [ -f "$config_file" ]; then
        echo "Error: Configuration '$name' already exists"
        return 1
    fi
    
    echo "Creating configuration '$name'..."
    cat > "$config_file" << EOF
{
    "brain": {
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": { "enable": true, "learning_rate": 0.001 },
        "hebbian": { "enable": true, "learning_rate": 0.01 }
    },
    "neuromodulation": {
        "curiosity": { "enable": true, "baseline": 0.5 },
        "novelty": { "enable": true, "threshold": 0.5 }
    },
    "world": {
        "width": 20,
        "height": 20,
        "vision_width": 8,
        "vision_height": 8
    },
    "simulation": {
        "timestep": 0.001,
        "max_steps": 10000
    }
}
EOF
    echo "Configuration '$name' created at $config_file"
}

# Configuration modification
modify_config() {
    local name=$1
    local config_file="configs/${name}.json"
    
    if [ ! -f "$config_file" ]; then
        echo "Error: Configuration '$name' not found"
        return 1
    fi
    
    echo "Modifying configuration '$name'..."
    echo "Available modifications:"
    echo "  1. brain.neuron_count"
    echo "  2. brain.region_count" 
    echo "  3. brain.connection_probability"
    echo "  4. plasticity.stdp.learning_rate"
    echo "  5. plasticity.hebbian.learning_rate"
    echo "  6. neuromodulation.curiosity.baseline"
    echo "  7. world.width"
    echo "  8. world.height"
    echo "  9. simulation.timestep"
    
    read -p "Enter modification (1-9): " choice
    
    case $choice in
        1) read -p "Enter neuron count: " count; sed -i "s/\"neuron_count\": .*/\"neuron_count\": $count,/" "$config_file" ;;
        2) read -p "Enter region count: " count; sed -i "s/\"region_count\": .*/\"region_count\": $count,/" "$config_file" ;;
        3) read -p "Enter connection probability: " prob; sed -i "s/\"connection_probability\": .*/\"connection_probability\": $prob,/" "$config_file" ;;
        4) read -p "Enter STDP learning rate: " lr; sed -i "s/\"learning_rate\": .*/\"learning_rate\": $lr,/" "$config_file" ;;
        5) read -p "Enter Hebbian learning rate: " lr; sed -i "s/\"learning_rate\": .*/\"learning_rate\": $lr,/" "$config_file" ;;
        6) read -p "Enter curiosity baseline: " base; sed -i "s/\"baseline\": .*/\"baseline\": $base,/" "$config_file" ;;
        7) read -p "Enter world width: " width; sed -i "s/\"width\": .*/\"width\": $width,/" "$config_file" ;;
        8) read -p "Enter world height: " height; sed -i "s/\"height\": .*/\"height\": $height,/" "$config_file" ;;
        9) read -p "Enter simulation timestep: " ts; sed -i "s/\"timestep\": .*/\"timestep\": $ts,/" "$config_file" ;;
        *) echo "Invalid choice" ;;
    esac
    
    echo "Configuration modified"
}

# Validate configuration
validate_config() {
    local name=$1
    local config_file="configs/${name}.json"
    
    if [ ! -f "$config_file" ]; then
        echo "Error: Configuration '$name' not found"
        return 1
    fi
    
    echo "Validating configuration '$name'..."
    python3 -c "
import json
import sys

with open('$config_file', 'r') as f:
    config = json.load(f)

errors = []
warnings = []

# Validate required sections
required_sections = ['brain', 'plasticity', 'neuromodulation', 'world', 'simulation']
for section in required_sections:
    if section not in config:
        errors.append(f'Missing required section: {section}')

# Validate brain parameters
if 'brain' in config:
    brain = config['brain']
    if 'neuron_count' not in brain:
        warnings.append('brain.neuron_count not specified (using default)')
    if 'region_count' not in brain:
        warnings.append('brain.region_count not specified (using default)')
    if 'connection_probability' in brain:
        prob = brain['connection_probability']
        if prob < 0.0 or prob > 1.0:
            errors.append('brain.connection_probability must be between 0.0 and 1.0')

# Validate plasticity
if 'plasticity' in config:
    plastic = config['plasticity']
    if 'stdp' in plastic and plastic['stdp'].get('enable', False):
        if 'learning_rate' not in plastic['stdp']:
            warnings.append('plasticity.stdp.learning_rate not specified (using default)')
        elif plastic['stdp']['learning_rate'] <= 0.0:
            errors.append('plasticity.stdp.learning_rate must be positive')

# Validate world dimensions
if 'world' in config:
    world = config['world']
    for dim in ['width', 'height']:
        if dim not in world:
            warnings.append(f'world.{dim} not specified (using default)')

# Output results
if warnings:
    print('Warnings:')
    for warning in warnings:
        print(f'  - {warning}')

if errors:
    print('Errors:')
    for error in errors:
        print(f'  - {error}')
else:
    print('Configuration is valid!')
" || echo "Error: Python validation failed"
}

# Export configuration
export_config() {
    local name=$1
    local output_file=$2
    local config_file="configs/${name}.json"
    
    if [ ! -f "$config_file" ]; then
        echo "Error: Configuration '$name' not found"
        return 1
    fi
    
    cp "$config_file" "$output_file"
    echo "Configuration exported to $output_file"
}

# List configurations
list_configs() {
    echo "Available configurations:"
    ls -la configs/*.json | sed 's|^|  |'
}

# Main command handling
case "$1" in
    create) create_config "$2" ;;
    modify) modify_config "$2" ;;
    validate) validate_config "$2" ;;
    export) export_config "$2" "$3" ;;
    list) list_configs ;;
    *) usage ;;
esac
```

### Configuration Management Python Module

```python
"""
Advanced configuration management for NLM.
Provides comprehensive configuration creation, modification, validation,
and management capabilities.
"""

import json
import copy
from typing import Dict, List, Any, Optional, Union
from dataclasses import dataclass, field
import os

@dataclass
class ConfigError:
    """Configuration error with severity and message."""
    message: str
    severity: str = "error"  # error, warning, info

class ConfigManager:
    """Advanced configuration manager for NLM."""
    
    def __init__(self, config_dir: str = "configs"):
        self.config_dir = config_dir
        self.configs = {}
        self.load_configs()
    
    def load_configs(self):
        """Load all configuration files from directory."""
        if not os.path.exists(self.config_dir):
            os.makedirs(self.config_dir)
        
        for filename in os.listdir(self.config_dir):
            if filename.endswith('.json'):
                name = filename[:-5]  # Remove .json extension
                with open(os.path.join(self.config_dir, filename), 'r') as f:
                    self.configs[name] = json.load(f)
    
    def save_config(self, name: str, config: Dict[str, Any]):
        """Save configuration to file."""
        filename = os.path.join(self.config_dir, f"{name}.json")
        with open(filename, 'w') as f:
            json.dump(config, f, indent=2)
        self.configs[name] = copy.deepcopy(config)
    
    def get_config(self, name: str) -> Optional[Dict[str, Any]]:
        """Get configuration by name."""
        return self.configs.get(name)
    
    def create_config(self, name: str, template: str = "default") -> Dict[str, Any]:
        """Create new configuration from template."""
        if template == "default":
            config = {
                "brain": {
                    "neuron_count": 1000,
                    "region_count": 2,
                    "connection_probability": 0.1,
                    "initial_weight_mean": 0.5,
                    "initial_weight_std": 0.1
                },
                "plasticity": {
                    "stdp": {
                        "enable": True,
                        "learning_rate": 0.001,
                        "tau_plus": 20.0,
                        "tau_minus": 20.0
                    },
                    "hebbian": {
                        "enable": True,
                        "learning_rate": 0.01
                    },
                    "structural": {
                        "enable": True,
                        "synaptogenesis_rate": 0.0001,
                        "pruning_rate": 0.00001
                    }
                },
                "neuromodulation": {
                    "dopamine": {
                        "baseline": 0.1,
                        "scale": 1.0
                    },
                    "curiosity": {
                        "enable": True,
                        "baseline": 0.5
                    },
                    "novelty": {
                        "enable": True,
                        "threshold": 0.5
                    }
                },
                "world": {
                    "width": 20,
                    "height": 20,
                    "vision_width": 8,
                    "vision_height": 8,
                    "max_energy": 100.0,
                    "energy_decay_rate": 0.01
                },
                "simulation": {
                    "timestep": 0.001,
                    "max_steps": 10000,
                    "time_limit": 0.0
                },
                "logging": {
                    "level": "INFO",
                    "to_file": False,
                    "filename": "nlm.log"
                }
            }
        elif template == "learning_focused":
            config = self.create_config("temp", "default")
            config["plasticity"]["stdp"]["learning_rate"] = 0.01
            config["plasticity"]["hebbian"]["learning_rate"] = 0.02
            config["neuromodulation"]["curiosity"]["baseline"] = 0.8
            config["neuromodulation"]["novelty"]["threshold"] = 0.3
        elif template == "optimized":
            config = self.create_config("temp", "default")
            config["brain"]["neuron_count"] = 500
            config["brain"]["region_count"] = 1
            config["plasticity"]["structural"]["enable"] = False
            config["world"]["width"] = 10
            config["world"]["height"] = 10
            config["world"]["vision_width"] = 4
            config["world"]["vision_height"] = 4
        else:
            raise ValueError(f"Unknown template: {template}")
        
        self.save_config(name, config)
        return config
    
    def modify_config(self, name: str, modifications: Dict[str, Any]):
        """Modify existing configuration."""
        config = self.get_config(name)
        if not config:
            raise ValueError(f"Configuration '{name}' not found")
        
        def deep_modify(base: Dict[str, Any], modifications: Dict[str, Any]):
            for key, value in modifications.items():
                if key in base and isinstance(base[key], dict) and isinstance(value, dict):
                    deep_modify(base[key], value)
                else:
                    base[key] = value
        
        deep_modify(config, modifications)
        self.save_config(name, config)
    
    def validate_config(self, name: str) -> List[ConfigError]:
        """Validate configuration and return list of errors."""
        config = self.get_config(name)
        if not config:
            return [ConfigError(f"Configuration '{name}' not found", "error")]
        
        errors = []
        warnings = []
        
        # Validate required sections
        required_sections = ['brain', 'plasticity', 'neuromodulation', 'world', 'simulation']
        for section in required_sections:
            if section not in config:
                warnings.append(ConfigError(f"Missing optional section: {section}", "warning"))
        
        # Validate brain parameters
        if 'brain' in config:
            brain = config['brain']
            if 'neuron_count' not in brain:
                warnings.append(ConfigError("brain.neuron_count not specified (using default)", "warning"))
            else:
                neuron_count = brain['neuron_count']
                if neuron_count < 10:
                    errors.append(ConfigError("Neuron count must be at least 10", "error"))
                elif neuron_count > 100000:
                    warnings.append(ConfigError("Neuron count is very high (performance impact)", "warning"))
            
            if 'connection_probability' in brain:
                prob = brain['connection_probability']
                if prob < 0.0 or prob > 1.0:
                    errors.append(ConfigError("Connection probability must be between 0.0 and 1.0", "error"))
        
        # Validate plasticity settings
        if 'plasticity' in config:
            plastic = config['plasticity']
            if 'stdp' in plastic and plastic['stdp'].get('enable', False):
                if 'learning_rate' not in plastic['stdp']:
                    warnings.append(ConfigError("plasticity.stdp.learning_rate not specified (using default)", "warning"))
                else:
                    stdp_lr = plastic['stdp']['learning_rate']
                    if stdp_lr <= 0.0:
                        errors.append(ConfigError("plasticity.stdp.learning_rate must be positive", "error"))
                    elif stdp_lr > 1.0:
                        warnings.append(ConfigError("plasticity.stdp.learning_rate is very high (instability risk)", "warning"))
        
        # Validate world dimensions
        if 'world' in config:
            world = config['world']
            for dim in ['width', 'height']:
                if dim not in world:
                    warnings.append(ConfigError(f"world.{dim} not specified (using default)", "warning"))
                elif world[dim] < 1:
                    errors.append(ConfigError(f"world.{dim} must be at least 1", "error"))
        
        # Validate simulation parameters
        if 'simulation' in config:
            sim = config['simulation']
            if 'timestep' not in sim:
                warnings.append(ConfigError("simulation.timestep not specified (using default)", "warning"))
            else:
                timestep = sim['timestep']
                if timestep <= 0.0:
                    errors.append(ConfigError("simulation.timestep must be positive", "error"))
                elif timestep > 1.0:
                    warnings.append(ConfigError("simulation.timestep is large (performance impact)", "warning"))
        
        return errors + warnings
    
    def compare_configs(self, name1: str, name2: str) -> Dict[str, Any]:
        """Compare two configurations and show differences."""
        config1 = self.get_config(name1)
        config2 = self.get_config(name2)
        
        if not config1 or not config2:
            return {"error": "One or both configurations not found"}
        
        def compare_dicts(dict1: Dict[str, Any], dict2: Dict[str, Any], path: str = ""):
            differences = {}
            
            all_keys = set(dict1.keys()) | set(dict2.keys())
            for key in all_keys:
                current_path = f"{path}.{key}" if path else key
                
                if key not in dict1:
                    differences[current_path] = {"value1": None, "value2": dict2[key], "change": "added"}
                elif key not in dict2:
                    differences[current_path] = {"value1": dict1[key], "value2": None, "change": "removed"}
                elif dict1[key] != dict2[key]:
                    differences[current_path] = {"value1": dict1[key], "value2": dict2[key], "change": "modified"}
            
            return differences
        
        return compare_dicts(config1, config2)
    
    def merge_configs(self, names: List[str], output_name: str) -> Dict[str, Any]:
        """Merge multiple configurations."""
        if len(names) < 2:
            raise ValueError("Need at least 2 configurations to merge")
        
        merged = {}
        for name in names:
            config = self.get_config(name)
            if not config:
                raise ValueError(f"Configuration '{name}' not found")
            
            # Merge with precedence (later configs override earlier ones)
            self._deep_merge(merged, config)
        
        self.save_config(output_name, merged)
        return merged
    
    def _deep_merge(self, base: Dict[str, Any], override: Dict[str, Any]):
        """Deep merge two configuration dictionaries."""
        for key, value in override.items():
            if key in base and isinstance(base[key], dict) and isinstance(value, dict):
                self._deep_merge(base[key], value)
            else:
                base[key] = value
    
    def list_configs(self) -> List[str]:
        """Get list of all configuration names."""
        return list(self.configs.keys())
    
    def delete_config(self, name: str):
        """Delete configuration."""
        if name in self.configs:
            filename = os.path.join(self.config_dir, f"{name}.json")
            if os.path.exists(filename):
                os.remove(filename)
            del self.configs[name]
        else:
            raise ValueError(f"Configuration '{name}' not found")

# Factory functions for common configurations
def create_config_from_file(filepath: str) -> Dict[str, Any]:
    """Create configuration from file."""
    with open(filepath, 'r') as f:
        return json.load(f)

def create_config_from_args(args: List[str]) -> Dict[str, Any]:
    """Create configuration from command line arguments."""
    config = {
        "brain": {"neuron_count": 1000, "region_count": 2, "connection_probability": 0.1},
        "plasticity": {"stdp": {"enable": True, "learning_rate": 0.001}},
        "neuromodulation": {"curiosity": {"enable": True, "baseline": 0.5}},
        "world": {"width": 20, "height": 20},
        "simulation": {"timestep": 0.001}
    }
    
    i = 0
    while i < len(args):
        arg = args[i]
        if arg == "--neuron_count" and i + 1 < len(args):
            config["brain"]["neuron_count"] = int(args[i + 1])
            i += 2
        elif arg == "--region_count" and i + 1 < len(args):
            config["brain"]["region_count"] = int(args[i + 1])
            i += 2
        elif arg == "--connection_probability" and i + 1 < len(args):
            config["brain"]["connection_probability"] = float(args[i + 1])
            i += 2
        elif arg == "--world_width" and i + 1 < len(args):
            config["world"]["width"] = int(args[i + 1])
            i += 2
        elif arg == "--world_height" and i + 1 < len(args):
            config["world"]["height"] = int(args[i + 1])
            i += 2
        elif arg == "--timestep" and i + 1 < len(args):
            config["simulation"]["timestep"] = float(args[i + 1])
            i += 2
        elif arg == "--curiosity_enable":
            config["neuromodulation"]["curiosity"]["enable"] = True
            i += 1
        elif arg == "--novelty_enable":
            config["neuromodulation"]["novelty"]["enable"] = True
            i += 1
        else:
            i += 1
    
    return config

# Global configuration manager instance
_config_manager = ConfigManager()

# Convenience functions
def get_config_manager() -> ConfigManager:
    """Get the global configuration manager instance."""
    return _config_manager

def create_config(name: str, template: str = "default") -> Dict[str, Any]:
    """Create configuration using global manager."""
    return _config_manager.create_config(name, template)

def validate_config(name: str) -> List[ConfigError]:
    """Validate configuration using global manager."""
    return _config_manager.validate_config(name)

def list_available_configs() -> List[str]:
    """List available configurations using global manager."""
    return _config_manager.list_configs()
"""

# Example usage
if __name__ == "__main__":
    # Create manager
    manager = ConfigManager()
    
    # List available configs
    print("Available configurations:", manager.list_configs())
    
    # Create a new configuration
    config = manager.create_config("my_experiment", "learning_focused")
    
    # Validate it
    errors = manager.validate_config("my_experiment")
    if errors:
        print("Validation errors:", [e.message for e in errors])
    
    # Modify it
    manager.modify_config("my_experiment", {
        "brain": {"neuron_count": 2000},
        "world": {"width": 30, "height": 30}
    })
    
    # Compare with default
    comparison = manager.compare_configs("my_experiment", "temp")
    print("Differences:", comparison)
    
    # Merge with another config
    merged = manager.merge_configs(["my_experiment", "temp"], "merged_config")
    print("Merged configuration created")
```

## Configuration Best Practices

### 1. Start Simple
- Begin with default configuration
- Only modify parameters as needed
- Use templates for common configurations

### 2. Validate Before Use
- Always validate configuration before running simulations
- Check for warnings and errors
- Ensure all required sections are present

### 3. Use Descriptive Names
- Use meaningful names for configurations
- Include purpose in filename (e.g., `configs/learning_experiment.json`)
- Version configurations (e.g., `configs/experiment_v1.json`)

### 4. Backup Important Configurations
- Keep backup of successful configurations
- Use git to track configuration changes
- Document configuration choices

### 5. Profile and Optimize
- Start with smaller configurations for testing
- Measure performance before scaling up
- Use performance monitoring tools

### 6. Document Configuration Choices
- Keep documentation of why parameters were chosen
- Record observation results
- Document learning curves and outcomes

### 7. Use Configuration Templates
- Create templates for different use cases
- Maintain template repository
- Share templates with team

## Configuration Troubleshooting

### Common Issues and Solutions

**Issue**: High memory usage
**Solution**: Reduce neuron count or increase timestep
```python
config = nlm.PythonFactory.createDefaultConfig()
config.set("brain", "neuron_count", 500)  # Reduce from 1000
config.set("simulation", "timestep", 0.01)  # Increase timestep
```

**Issue**: Slow simulation speed
**Solution**: Optimize world size or disable structural plasticity
```python
config = nlm.PythonFactory.createDefaultConfig()
config.set("world", "width", 10)  # Smaller world
config.set("world", "height", 10)
config.set("plasticity", "structural", "enable", False)  # Disable structural
```

**Issue**: Learning doesn't occur
**Solution**: Check neuromodulation and plasticity settings
```python
config = nlm.PythonFactory.createDefaultConfig()
config.set("neuromodulation", "curiosity", "enable", True)
config.set("neuromodulation", "novelty", "enable", True)
config.set("plasticity", "stdp", "learning_rate", 0.01)  # Increase rate
```

**Issue**: Configuration validation errors
**Solution**: Fix validation issues or use more permissive validation
```python
from nlm import ConfigValidator

class LenientValidator(ConfigValidator):
    def validate_config(self, config):
        warnings = []
        # Only warn about issues, don't error
        return warnings
```

### Debug Configuration Issues

```python
import nlm
import json

# Create and validate configuration
config = nlm.PythonFactory.createDefaultConfig()

# Add validation
from nlm import ConfigValidator
validator = ConfigValidator()
errors = validator.validate_config(config)

if errors:
    print("Configuration issues found:")
    for error in errors:
        print(f"  - {error.message}")
    # Fix issues and validate again
else:
    print("Configuration is valid")

# Debug by printing configuration
print("Current configuration:")
print(json.dumps(config, indent=2))

# Test with minimal configuration
minimal_config = {
    "brain": {"neuron_count": 100, "region_count": 1},
    "world": {"width": 10, "height": 10}
}
```

## Configuration File Reference

### Core Configuration Sections

**brain**: Brain parameters
- `neuron_count`: Number of neurons
- `region_count`: Number of brain regions
- `connection_probability`: Connection probability
- `initial_weight_mean`: Initial synaptic weight mean
- `initial_weight_std`: Initial synaptic weight standard deviation

**plasticity**: Plasticity mechanisms
- `stdp`: Spike-timing dependent plasticity settings
- `hebbian`: Hebbian learning settings
- `structural`: Structural plasticity settings

**neuromodulation**: Neuromodulation systems
- `dopamine`: Dopamine modulation
- `curiosity`: Curiosity system
- `novelty`: Novelty detection

**development**: Developmental processes
- `enable`: Whether development is enabled
- `initial_plasticity`: Initial plasticity rate
- `critical_period_end`: Critical period end time

**memory**: Memory systems
- `working_memory_size`: Working memory capacity
- `episodic_capacity`: Episodic memory capacity
- `associative_threshold`: Associative memory threshold

**world**: World simulation
- `width`: World width
- `height`: World height
- `vision_width`: Vision width
- `vision_height`: Vision height
- `max_energy`: Maximum energy
- `energy_decay_rate`: Energy decay rate

**simulation**: Simulation parameters
- `timestep`: Simulation timestep
- `max_steps`: Maximum simulation steps
- `time_limit`: Time limit in seconds

**logging**: Logging configuration
- `level`: Log level
- `to_file`: Whether to log to file
- `filename`: Log filename

## Advanced Configuration Features

### 1. Environment-Specific Configurations

```python
# Development environment
dev_config = nlm.PythonFactory.createDefaultConfig()
dev_config.set("brain", "neuron_count", 100)
dev_config.set("simulation", "max_steps", 1000)

# Production environment
prod_config = nlm.PythonFactory.createDefaultConfig()
prod_config.set("brain", "neuron_count", 5000)
prod_config.set("simulation", "max_steps", 100000)
prod_config.set("logging", "level", "WARN")

# Testing environment
test_config = nlm.PythonFactory.createDefaultConfig()
test_config.set("brain", "neuron_count", 50)
```

### 2. Experimental Configurations

```python
# Experiment A: High curiosity
experiment_a = {
    "brain": {"neuron_count": 1000},
    "neuromodulation": {
        "curiosity": {"baseline": 0.9},
        "novelty": {"threshold": 0.2}
    }
}

# Experiment B: Low curiosity
experiment_b = {
    "brain": {"neuron_count": 1000},
    "neuromodulation": {
        "curiosity": {"baseline": 0.1},
        "novelty": {"threshold": 0.8}
    }
}

# Experiment C: No curiosity
experiment_c = {
    "brain": {"neuron_count": 1000},
    "neuromodulation": {
        "curiosity": {"enable": False},
        "novelty": {"enable": False}
    }
}
```

### 3. Configuration Versioning

```python
# Version 1.0
version1_config = {
    "version": "1.0",
    "brain": {"neuron_count": 1000},
    "neuromodulation": {"curiosity": {"baseline": 0.5}}
}

# Version 2.0 (with new features)
version2_config = {
    "version": "2.0",
    "brain": {"neuron_count": 2000, "region_count": 3},
    "neuromodulation": {
        "curiosity": {"baseline": 0.7, "enable": True},
        "novelty": {"threshold": 0.3, "enable": True}
    },
    "development": {"enable": True, "initial_plasticity": 1.0}
}
```

## Configuration Performance Tips

### Memory Optimization
- Use smaller neuron counts for testing
- Disable structural plasticity for performance
- Limit working memory size
- Use smaller world dimensions

### Speed Optimization
- Increase simulation timestep
- Reduce logging frequency
- Disable debugging features
- Use optimized performance mode

### Quality Optimization
- Use realistic neuromodulation parameters
- Enable appropriate plasticity mechanisms
- Balance exploration and exploitation
- Monitor learning progress

## Conclusion

Advanced configuration management is crucial for effective NLM usage. This guide provides comprehensive coverage of configuration options, command-line tools, and management capabilities. By following best practices and using the provided tools, you can efficiently manage and optimize your NLM simulations.

Key takeaways:
1. Use the factory functions for easy configuration creation
2. Always validate configurations before use
3. Use templates for common configurations
4. Document your configuration choices
5. Profile and optimize based on your specific use case

The advanced configuration system empowers you to:
- Create custom configurations for any use case
- Manage multiple experiment configurations
- Validate and debug configuration issues
- Optimize performance for your specific needs
- Document and share configurations with your team