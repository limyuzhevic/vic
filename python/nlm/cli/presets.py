"""
Presets utilities for NLM CLI
"""

import json
import yaml
from pathlib import Path
from typing import Dict, Any, List, Optional
from datetime import datetime

class NLMPresets:
    def __init__(self, presets_dir: str = "presets"):
        self.presets_dir = Path(presets_dir)
        self.presets_dir.mkdir(parents=True, exist_ok=True)
        self.presets = {}
    
    def load_preset(self, name: str) -> Optional[Dict[str, Any]]:
        """Load a preset configuration"""
        preset_file = self.presets_dir / f"{name}.json"
        
        if not preset_file.exists():
            print(f"Preset not found: {name}")
            return None
        
        try:
            with open(preset_file, 'r') as f:
                preset = json.load(f)
            
            self.presets[name] = preset
            print(f"Loaded preset: {name}")
            return preset
        except Exception as e:
            print(f"Error loading preset {name}: {e}")
            return None
    
    def save_preset(self, name: str, config: Dict[str, Any], description: str = "") -> bool:
        """Save a preset configuration"""
        try:
            preset = {
                "name": name,
                "description": description,
                "timestamp": datetime.now().isoformat(),
                "config": config
            }
            
            preset_file = self.presets_dir / f"{name}.json"
            with open(preset_file, 'w') as f:
                json.dump(preset, f, indent=2)
            
            self.presets[name] = preset
            print(f"Saved preset: {name}")
            return True
        except Exception as e:
            print(f"Error saving preset {name}: {e}")
            return False
    
    def list_presets(self) -> List[Dict[str, Any]]:
        """List all available presets"""
        presets = []
        
        for preset_file in self.presets_dir.glob("*.json"):
            try:
                with open(preset_file, 'r') as f:
                    preset = json.load(f)
                presets.append({
                    "name": preset_file.stem,
                    "description": preset.get("description", ""),
                    "timestamp": preset.get("timestamp", ""),
                    "config_keys": list(preset.get("config", {}).keys())
                })
            except Exception as e:
                print(f"Error reading preset file {preset_file}: {e}")
        
        return sorted(presets, key=lambda x: x["timestamp"], reverse=True)
    
    def delete_preset(self, name: str) -> bool:
        """Delete a preset"""
        preset_file = self.presets_dir / f"{name}.json"
        
        if not preset_file.exists():
            print(f"Preset not found: {name}")
            return False
        
        try:
            preset_file.unlink()
            if name in self.presets:
                del self.presets[name]
            print(f"Deleted preset: {name}")
            return True
        except Exception as e:
            print(f"Error deleting preset {name}: {e}")
            return False
    
    def export_preset(self, name: str, output_file: str, format: str = "json") -> bool:
        """Export a preset to a file"""
        preset = self.load_preset(name)
        if not preset:
            return False
        
        try:
            Path(output_file).parent.mkdir(parents=True, exist_ok=True)
            
            if format.lower() == 'json':
                with open(output_file, 'w') as f:
                    json.dump(preset, f, indent=2)
            elif format.lower() == 'yaml':
                with open(output_file, 'w') as f:
                    yaml.dump(preset, f, default_flow_style=False)
            else:
                raise ValueError(f"Unsupported format: {format}")
            
            print(f"Exported preset {name} to {output_file}")
            return True
        except Exception as e:
            print(f"Error exporting preset: {e}")
            return False
    
    def import_preset(self, filepath: str, name: str = None) -> bool:
        """Import a preset from a file"""
        try:
            if filepath.endswith('.json'):
                with open(filepath, 'r') as f:
                    preset = json.load(f)
            elif filepath.endswith(('.yaml', '.yml')):
                with open(filepath, 'r') as f:
                    preset = yaml.safe_load(f)
            else:
                raise ValueError("Unsupported file format")
            
            # Use provided name or extract from preset
            if name is None:
                name = preset.get('name', Path(filepath).stem)
            
            # Save the preset
            success = self.save_preset(name, preset.get("config", {}), preset.get("description", ""))
            
            if success:
                print(f"Imported preset from {filepath} as '{name}'")
            
            return success
        except Exception as e:
            print(f"Error importing preset: {e}")
            return False

# Built-in presets
class BuiltInPresets:
    @staticmethod
    def get_quick_test_config() -> Dict[str, Any]:
        """Get a quick test configuration"""
        return {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 100,
            "region_count": 1,
            "connection_probability": 0.2,
            "plasticity_learning_rate": 0.05,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "max_simulation_steps": 1000,
            "visualization_enabled": False
        }
    
    @staticmethod
    def get_research_config() -> Dict[str, Any]:
        """Get a research configuration"""
        return {
            "random_seed": 12345,
            "simulation_timestep": 0.0005,
            "neuron_count": 5000,
            "region_count": 4,
            "connection_probability": 0.15,
            "plasticity_learning_rate": 0.02,
            "stdp_ltp_weight": 0.02,
            "stdp_ltd_weight": 0.018,
            "stdp_time_constant": 25.0,
            "development_synaptogenesis_rate": 0.0005,
            "development_pruning_rate": 0.00005,
            "dopamine_baseline": 0.2,
            "reward_discount_factor": 0.995,
            "environment_name": "AdvancedWorld",
            "environment_width": 20,
            "environment_height": 20,
            "max_simulation_steps": 50000,
            "visualization_enabled": True,
            "log_level": "DEBUG"
        }
    
    @staticmethod
    def get_minimal_config() -> Dict[str, Any]:
        """Get a minimal configuration"""
        return {
            "random_seed": 42,
            "simulation_timestep": 0.001,
            "neuron_count": 10,
            "region_count": 1,
            "connection_probability": 0.5,
            "max_simulation_steps": 100,
            "visualization_enabled": False
        }
    
    @staticmethod
    def get_development_config() -> Dict[str, Any]:
        """Get a development configuration"""
        return {
            "random_seed": 99,
            "simulation_timestep": 0.001,
            "neuron_count": 1000,
            "region_count": 2,
            "connection_probability": 0.1,
            "plasticity_learning_rate": 0.01,
            "stdp_ltp_weight": 0.01,
            "stdp_ltd_weight": 0.012,
            "stdp_time_constant": 20.0,
            "development_synaptogenesis_rate": 0.001,
            "development_pruning_rate": 0.0001,
            "development_maturity_rate": 0.01,
            "development_excitability_change": 0.005,
            "dopamine_baseline": 0.1,
            "reward_discount_factor": 0.99,
            "environment_name": "DevelopmentWorld",
            "environment_width": 15,
            "environment_height": 15,
            "max_simulation_steps": 20000,
            "visualization_enabled": False,
            "log_level": "INFO"
        }
    
    @staticmethod
    def get_performance_config() -> Dict[str, Any]:
        """Get a performance-optimized configuration"""
        return {
            "random_seed": 777,
            "simulation_timestep": 0.002,
            "neuron_count": 2000,
            "region_count": 3,
            "connection_probability": 0.08,
            "plasticity_learning_rate": 0.005,
            "stdp_ltp_weight": 0.008,
            "stdp_ltd_weight": 0.007,
            "stdp_time_constant": 15.0,
            "development_synaptogenesis_rate": 0.0002,
            "development_pruning_rate": 0.00002,
            "dopamine_baseline": 0.05,
            "reward_discount_factor": 0.98,
            "environment_name": "PerformanceWorld",
            "environment_width": 25,
            "environment_height": 25,
            "max_simulation_steps": 10000,
            "visualization_enabled": False,
            "log_level": "WARN"
        }

# Create instance and save built-in presets
def create_builtin_presets():
    """Create the built-in presets directory and files"""
    presets = BuiltInPresets()
    presets_dir = Path("presets")
    presets_dir.mkdir(parents=True, exist_ok=True)
    
    builtin_presets = {
        "quick_test": presets.get_quick_test_config(),
        "research": presets.get_research_config(),
        "minimal": presets.get_minimal_config(),
        "development": presets.get_development_config(),
        "performance": presets.get_performance_config()
    }
    
    descriptions = {
        "quick_test": "Quick test configuration with small brain for rapid testing",
        "research": "Full research configuration with advanced settings",
        "minimal": "Minimal configuration for testing basic functionality",
        "development": "Development configuration with enhanced plasticity and development",
        "performance": "Performance-optimized configuration for benchmarking"
    }
    
    # Save all presets
    for name, config in builtin_presets.items():
        description = descriptions[name]
        success = NLMPresets().save_preset(name, config, description)
        if success:
            print(f"Created built-in preset: {name}")
        else:
            print(f"Failed to create preset: {name}")

# Initialize built-in presets on module import
create_builtin_presets()
