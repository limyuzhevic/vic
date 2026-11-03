#!/usr/bin/env python3
"""
NLM Advanced Configuration File

Advanced configuration settings for NLM simulations and debugging.
This file provides extended configuration options for advanced users.
"""

import json
from typing import Dict, Any, Optional
from pathlib import Path

class AdvancedConfig:
    def __init__(self, config_path: Optional[str] = None):
        self.config_path = config_path
        self.settings = self._load_default_settings()
        if config_path:
            self.load(config_path)
    
    def _load_default_settings(self) -> Dict[str, Any]:
        """Load default advanced settings"""
        return {
            # Neural simulation settings
            "neural": {
                "simulation_fps": 1000,
                "max_neurons": 10000,
                "max_synapses": 50000,
                "dt": 0.001,
                "time_step": 0.001,
                "enable_plasticity": True,
                "plasticity_rate": 0.01,
                "learning_enabled": True,
                "reward_enabled": True,
                "curiosity_enabled": True,
                "novelty_enabled": True
            },
            
            # Visualization settings
            "visualization": {
                "enabled": True,
                "backend": "opengl",
                "width": 1920,
                "height": 1080,
                "framerate": 60,
                "show_neurons": True,
                "show_synapses": True,
                "show_spikes": True,
                "show_weights": True,
                "color_scheme": "default"
            },
            
            # Advanced debugging
            "debugging": {
                "enable_breakpoints": True,
                "enable_watchpoints": True,
                "enable_profiling": True,
                "enable_memory_tracking": True,
                "enable_timing_tracking": True,
                "log_level": "DEBUG",
                "output_format": "json"
            },
            
            # Performance optimization
            "performance": {
                "enable_multithreading": True,
                "thread_count": 8,
                "enable_simd": True,
                "enable_cache_optimization": True,
                "memory_pool_size": 1024,
                "event_queue_size": 8192
            },
            
            # Advanced features
            "advanced_features": {
                "enable_structural_plasticity": True,
                "enable_developmental_system": True,
                "enable_prediction_system": True,
                "enable_cognitive_planning": True,
                "enable_social_learning": True,
                "enable_self_model": True,
                "enable_text_processing": True
            },
            
            # Experimental settings
            "experiments": {
                "current_phase": "6",
                "enable_checkpointing": True,
                "checkpoint_interval": 100,
                "enable_replay": True,
                "replay_interval": 10,
                "enable_ablation": False,
                "ablation_mode": "none"
            }
        }
    
    def load(self, config_path: str):
        """Load configuration from JSON file"""
        try:
            with open(config_path, 'r') as f:
                file_config = json.load(f)
            self._merge_config(file_config)
        except Exception as e:
            print(f"Warning: Could not load config file {config_path}: {e}")
    
    def _merge_config(self, file_config: Dict[str, Any]):
        """Merge file configuration with current settings"""
        def merge_dict(original: Dict[str, Any], override: Dict[str, Any]):
            for key, value in override.items():
                if key in original and isinstance(original[key], dict) and isinstance(value, dict):
                    merge_dict(original[key], value)
                else:
                    original[key] = value
        
        merge_dict(self.settings, file_config)
    
    def get(self, key_path: str, default=None):
        """Get configuration value using dot notation"""
        keys = key_path.split('.')
        value = self.settings
        for key in keys:
            if isinstance(value, dict) and key in value:
                value = value[key]
            else:
                return default
        return value
    
    def set(self, key_path: str, value: Any):
        """Set configuration value using dot notation"""
        keys = key_path.split('.')
        current = self.settings
        for key in keys[:-1]:
            if key not in current:
                current[key] = {}
            current = current[key]
        current[keys[-1]] = value
    
    def save(self, config_path: str):
        """Save configuration to JSON file"""
        with open(config_path, 'w') as f:
            json.dump(self.settings, f, indent=2)
    
    def print_config(self, section: Optional[str] = None):
        """Print configuration to console"""
        if section:
            if section in self.settings:
                print(f"=== {section.upper()} ===")
                print(json.dumps(self.settings[section], indent=2))
            else:
                print(f"Section '{section}' not found")
        else:
            print("=== NLM Advanced Configuration ===")
            print(json.dumps(self.settings, indent=2))
    
    def create_advanced_config_file(self, filepath: str):
        """Create an example advanced configuration file"""
        self.save(filepath)
        print(f"Created advanced configuration file: {filepath}")

def main():
    import argparse
    
    parser = argparse.ArgumentParser(
        description="NLM Advanced Configuration Manager",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_config --create-file advanced_config.json
  nlm_config --load advanced_config.json
  nlm_config --save my_config.json
  nlm_config --get neural.simulation_fps
  nlm_config --set neural.simulation_fps 2000
  nlm_config --print neural
  nlm_config --print experimental
        """
    )
    
    parser.add_argument(
        "--create-file",
        help="Create an example advanced configuration file"
    )
    
    parser.add_argument(
        "--load",
        help="Load configuration from file"
    )
    
    parser.add_argument(
        "--save",
        help="Save configuration to file"
    )
    
    parser.add_argument(
        "--get",
        help="Get configuration value using dot notation (e.g., neural.simulation_fps)"
    )
    
    parser.add_argument(
        "--set",
        nargs=2,
        help="Set configuration value using dot notation (e.g., neural.simulation_fps 2000)"
    )
    
    parser.add_argument(
        "--print",
        help="Print configuration section (neural, visualization, debugging, performance, advanced_features, experiments)"
    )
    
    parser.add_argument(
        "--config",
        help="Configuration file path (default: advanced_config.json)"
    )
    
    args = parser.parse_args()
    
    config_file = args.config or "advanced_config.json"
    config = AdvancedConfig()
    
    if args.create_file:
        config.create_advanced_config_file(args.create_file)
    elif args.load:
        config = AdvancedConfig(args.load)
        print(f"Loaded configuration from {args.load}")
    elif args.save:
        config.save(args.save)
        print(f"Saved configuration to {args.save}")
    elif args.get:
        value = config.get(args.get)
        print(f"{args.get} = {value}")
    elif args.set:
        key, value = args.set
        # Try to convert numeric values
        try:
            if '.' in value:
                value = float(value)
            else:
                value = int(value)
        except ValueError:
            pass
        config.set(key, value)
        print(f"Set {key} = {value}")
    elif args.print:
        config.print_config(args.print)
    else:
        parser.print_help()

if __name__ == "__main__":
    main()
