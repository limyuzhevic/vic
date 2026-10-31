#!/usr/bin/env python3
# NLM Configuration Management Tool
# Advanced configuration management for NLM neural simulation framework

import json
import yaml
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional
import argparse

# Default configuration template
DEFAULT_CONFIG = {
    "simulation": {
        "random_seed": 42,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.15,
        "simulation_timestep": 0.001,
        "max_steps": 10000,
        "output_frequency": 100,
    },
    "neurons": {
        "resting_potential": -70.0,
        "threshold": -55.0,
        "reset_potential": -70.0,
        "leak_conductance": 10.0,
        "refractory_period": 5,
        "adaptation_strength": 0.5,
    },
    "plasticity": {
        "stdp_ltp_weight": 0.02,
        "stdp_ltd_weight": 0.015,
        "stdp_tau": 20.0,
        "hebbian_learning_rate": 0.01,
        "hebbian_window": 50.0,
    },
    "neuromodulation": {
        "dopamine_max_level": 1.0,
        "dopamine_decay_rate": 0.99,
        "curiosity_sensitivity": 0.3,
        "novelty_threshold": 0.5,
    },
    "world": {
        "world_width": 20,
        "world_height": 20,
        "vision_width": 8,
        "vision_height": 8,
        "max_energy": 100.0,
    },
}

class NLMConfigManager:
    def __init__(self, config_file: Optional[str] = None):
        self.config_file = config_file
        self.config = DEFAULT_CONFIG.copy()
        
        if config_file and os.path.exists(config_file):
            self.load_config(config_file)
        
        self.config_dir = Path("configs")
        self.config_dir.mkdir(exist_ok=True)
    
    def load_config(self, filepath: str) -> bool:
        """Load configuration from file (JSON, YAML, or NLM config format)"""
        try:
            path = Path(filepath)
            
            if path.suffix == '.json':
                with open(path, 'r') as f:
                    loaded_config = json.load(f)
                    self._merge_config(loaded_config)
            elif path.suffix == '.yaml' or path.suffix == '.yml':
                with open(path, 'r') as f:
                    loaded_config = yaml.safe_load(f)
                    self._merge_config(loaded_config)
            elif path.suffix == '.cfg':
                # Parse NLM config file format (simple key=value)
                loaded_config = self._parse_nlm_config(path)
                self._merge_config(loaded_config)
            else:
                print(f"Unsupported file format: {path.suffix}")
                return False
            
            self.config_file = filepath
            print(f"Loaded configuration from: {filepath}")
            return True
            
        except Exception as e:
            print(f"Error loading configuration from {filepath}: {e}")
            return False
    
    def save_config(self, filepath: Optional[str] = None, format: str = "nlm") -> bool:
        """Save configuration to file"""
        output_file = filepath or self.config_file or "nlm_config.cfg"
        
        try:
            path = Path(output_file)
            
            if format == "json":
                with open(path, 'w') as f:
                    json.dump(self.config, f, indent=2)
            elif format == "yaml":
                with open(path, 'w') as f:
                    yaml.dump(self.config, f, default_flow_style=False)
            elif format == "nlm":
                self._save_nlm_config(path)
            else:
                print(f"Unsupported output format: {format}")
                return False
            
            print(f"Configuration saved to: {output_file}")
            return True
            
        except Exception as e:
            print(f"Error saving configuration to {output_file}: {e}")
            return False
    
    def _parse_nlm_config(self, filepath: Path) -> Dict[str, Any]:
        """Parse NLM configuration file format"""
        config = {}
        current_section = ""
        
        with open(filepath, 'r') as f:
            for line in f:
                line = line.strip()
                if not line or line.startswith('#'):
                    continue
                
                # Check for section headers [section]
                if line.startswith('[') and line.endswith(']'):
                    current_section = line[1:-1]
                    config[current_section] = {}
                    continue
                
                # Parse key = value pairs
                if '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip()
                    
                    # Try to convert numeric values
                    try:
                        if '.' in value:
                            value = float(value)
                        else:
                            value = int(value)
                    except ValueError:
                        pass  # Keep as string
                    
                    if current_section:
                        config[current_section][key] = value
                    else:
                        config[key] = value
        
        return config
    
    def _save_nlm_config(self, filepath: Path) -> None:
        """Save configuration in NLM config format"""
        with open(filepath, 'w') as f:
            f.write("# NLM Configuration File\n")
            f.write("# Generated by NLM Configuration Manager\n\n")
            
            for section, values in self.config.items():
                f.write(f"[{section}]\n")
                for key, value in values.items():
                    if isinstance(value, str):
                        f.write(f"{key} = \"{value}\"\n")
                    else:
                        f.write(f"{key} = {value}\n")
                f.write("\n")
    
    def _merge_config(self, new_config: Dict[str, Any]) -> None:
        """Merge new configuration into current config"""
        for section, values in new_config.items():
            if section in self.config:
                if isinstance(self.config[section], dict) and isinstance(values, dict):
                    self.config[section].update(values)
                else:
                    self.config[section] = values
            else:
                self.config[section] = values
    
    def set_value(self, key_path: str, value: Any) -> None:
        """Set configuration value using dot notation"""
        keys = key_path.split('.')
        current = self.config
        
        for i, key in enumerate(keys[:-1]):
            if key not in current:
                current[key] = {}
            current = current[key]
        
        # Handle the last key
        last_key = keys[-1]
        if last_key in current and not isinstance(current[last_key], dict):
            # Convert to appropriate type if possible
            try:
                if isinstance(current[last_key], str) and '.' in current[last_key]:
                    current[last_key] = float(current[last_key])
                elif isinstance(current[last_key], str):
                    current[last_key] = int(current[last_key])
            except ValueError:
                pass
        
        current[last_key] = value
    
    def get_value(self, key_path: str, default: Any = None) -> Any:
        """Get configuration value using dot notation"""
        keys = key_path.split('.')
        current = self.config
        
        for key in keys:
            if key in current:
                current = current[key]
            else:
                return default
        
        return current
    
    def list_sections(self) -> list:
        """List all configuration sections"""
        return list(self.config.keys())
    
    def export_config(self, output_format: str = "json", output_file: Optional[str] = None) -> bool:
        """Export configuration to specified format"""
        return self.save_config(output_file, output_format)
    
    def create_preset(self, preset_name: str, description: str = "") -> None:
        """Create a preset configuration"""
        preset_file = self.config_dir / f"{preset_name}.nlm"
        
        # Save current config as preset
        self.save_config(str(preset_file), "nlm")
        
        # Create preset metadata file
        metadata_file = self.config_dir / f"{preset_name}.json"
        with open(metadata_file, 'w') as f:
            preset_info = {
                "name": preset_name,
                "description": description,
                "created": str(Path.cwd()),
                "config_file": preset_name + ".nlm"
            }
            json.dump(preset_info, f, indent=2)
        
        print(f"Created preset: {preset_name}")
    
    def load_preset(self, preset_name: str) -> bool:
        """Load a preset configuration"""
        preset_file = self.config_dir / f"{preset_name}.nlm"
        
        if preset_file.exists():
            return self.load_config(str(preset_file))
        else:
            print(f"Preset not found: {preset_name}")
            return False
    
    def compare_configs(self, other_config_manager: 'NLMConfigManager') -> Dict[str, Any]:
        """Compare two configurations and return differences"""
        differences = {
            "added": {},
            "removed": {},
            "modified": {}
        }
        
        # Compare all sections
        all_sections = set(self.config.keys()) | set(other_config_manager.config.keys())
        
        for section in all_sections:
            if section not in self.config:
                differences["added"][section] = other_config_manager.config[section]
            elif section not in other_config_manager.config:
                differences["removed"][section] = self.config[section]
            else:
                # Compare section values
                self_section = self.config[section]
                other_section = other_config_manager.config[section]
                
                if isinstance(self_section, dict) and isinstance(other_section, dict):
                    sec_diff = self._compare_dicts(self_section, other_section)
                    if sec_diff:
                        differences["modified"][section] = sec_diff
        
        return differences
    
    def _compare_dicts(self, dict1: Dict, dict2: Dict) -> Dict[str, Any]:
        """Compare two dictionaries and return differences"""
        differences = {}
        
        all_keys = set(dict1.keys()) | set(dict2.keys())
        
        for key in all_keys:
            if key not in dict1:
                differences[key] = {"from": None, "to": dict2[key]}
            elif key not in dict2:
                differences[key] = {"from": dict1[key], "to": None}
            elif dict1[key] != dict2[key]:
                differences[key] = {"from": dict1[key], "to": dict2[key]}
        
        return differences

def main():
    parser = argparse.ArgumentParser(
        description="NLM Configuration Management Tool",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  # Create a new configuration file
  nlm_config_manager --create-config
  
  # Load an existing configuration
  nlm_config_manager --load-config my_config.json
  
  # Set a configuration value
  nlm_config_manager --set simulation.neuron_count 2000
  
  # Get a configuration value
  nlm_config_manager --get simulation.random_seed
  
  # Save configuration to different formats
  nlm_config_manager --save-config --format yaml
  
  # Compare two configurations
  nlm_config_manager --compare config1.json config2.json
        """
    )
    
    parser.add_argument(
        "--create-config",
        action="store_true",
        help="Create a new default configuration file"
    )
    
    parser.add_argument(
        "--load-config",
        help="Load configuration from file"
    )
    
    parser.add_argument(
        "--save-config",
        help="Save configuration to file"
    )
    
    parser.add_argument(
        "--format",
        choices=["nlm", "json", "yaml"],
        default="nlm",
        help="Output format (default: nlm)"
    )
    
    parser.add_argument(
        "--set",
        help="Set configuration value using dot notation (e.g., 'simulation.neuron_count')"
    )
    
    parser.add_argument(
        "--get",
        help="Get configuration value using dot notation"
    )
    
    parser.add_argument(
        "--create-preset",
        help="Create a preset configuration"
    )
    
    parser.add_argument(
        "--load-preset",
        help="Load a preset configuration"
    )
    
    parser.add_argument(
        "--compare",
        nargs=2,
        help="Compare two configuration files"
    )
    
    parser.add_argument(
        "--list-presets",
        action="store_true",
        help="List all available presets"
    )
    
    args = parser.parse_args()
    
    # Initialize config manager
    config_file = args.load_config if hasattr(args, 'load_config') else None
    config_manager = NLMConfigManager(config_file)
    
    if args.create_config:
        config_manager.save_config("nlm_config.cfg")
    
    elif args.save_config:
        config_manager.save_config(args.save_config, args.format)
    
    elif args.set and args.set in args.__dict__:
        # Need to parse set argument
        parts = args.set.split('=')
        if len(parts) != 2:
            print("Error: --set argument must be in format 'key=value'")
            return 1
        
        key, value = parts[0], parts[1]
        config_manager.set_value(key, value)
    
    elif args.get:
        value = config_manager.get_value(args.get)
        print(f"{args.get}: {value}")
    
    elif args.create_preset:
        description = input("Enter preset description: ")
        config_manager.create_preset(args.create_preset, description)
    
    elif args.load_preset:
        config_manager.load_preset(args.load_preset)
    
    elif args.compare:
        other_manager = NLMConfigManager(args.compare[0])
        differences = config_manager.compare_configs(other_manager)
        
        print("Configuration Differences:")
        print("=" * 50)
        
        if differences["added"]:
            print("\n[ADDED SECTIONS]")
            for section, values in differences["added"].items():
                print(f"  + {section}")
                for key, value in values.items():
                    print(f"      {key}: {value}")
        
        if differences["removed"]:
            print("\n[REMOVED SECTIONS]")
            for section, values in differences["removed"].items():
                print(f"  - {section}")
                for key, value in values.items():
                    print(f"      {key}: {value}")
        
        if differences["modified"]:
            print("\n[MODIFIED SECTIONS]")
            for section, values in differences["modified"].items():
                print(f"  ~ {section}")
                for key, diff in values.items():
                    print(f"      {key}:")
                    print(f"        from: {diff['from']}")
                    print(f"        to: {diff['to']}")
    
    elif args.list_presets:
        print("Available presets:")
        for preset_file in Path("configs").glob("*.nlm"):
            metadata_file = Path("configs") / (preset_file.stem + ".json")
            if metadata_file.exists():
                with open(metadata_file, 'r') as f:
                    metadata = json.load(f)
                print(f"  - {metadata['name']}: {metadata['description']}")
            else:
                print(f"  - {preset_file.stem}")
    
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
