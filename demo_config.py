#!/usr/bin/env python3

# Demo script showing different configuration formats

import json
import yaml
import toml
import configparser

# Sample configuration in different formats

# JSON format
json_config = {
    "core": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000
    },
    "plasticity": {
        "enable_plasticity": True,
        "stdp_ltp_weight": 0.02
    }
}

print("JSON Configuration:")
print(json.dumps(json_config, indent=2))
print()

# YAML format  
yaml_config = {
    "core": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000
    },
    "plasticity": {
        "enable_plasticity": True,
        "stdp_ltp_weight": 0.02
    }
}

print("YAML Configuration:")
print(yaml.dump(yaml_config, default_flow_style=False))
print()

# TOML format
toml_config = {
    "core": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000
    },
    "plasticity": {
        "enable_plasticity": True,
        "stdp_ltp_weight": 0.02
    }
}

print("TOML Configuration:")
print(toml.dumps(toml_config))
print()

# INI format
ini_config = configparser.ConfigParser()
ini_config['CORE'] = {
    'random_seed': '42',
    'simulation_timestep': '0.001',
    'neuron_count': '1000'
}
ini_config['PLASTICITY'] = {
    'enable_plasticity': 'true',
    'stdp_ltp_weight': '0.02'
}

print("INI Configuration:")
ini_config.write(open("temp.ini", 'w'))
with open("temp.ini", 'r') as f:
    print(f.read())
print()

# Clean up
import os
if os.path.exists("temp.ini"):
    os.remove("temp.ini")

print("Configuration formats demonstrated successfully!")
