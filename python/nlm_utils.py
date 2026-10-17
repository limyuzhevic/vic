#!/usr/bin/env python3
"""
Utility functions for NLM Python scripts
"""

import json
import time
import os
import sys
from datetime import datetime
from typing import Dict, Any, Optional, List
import numpy as np

def create_default_config() -> Dict[str, Any]:
    """Create a default configuration dictionary"""
    return {
        "simulation": {
            "steps": 1000,
            "vision_width": 16,
            "vision_height": 16,
            "timestep": 0.01
        },
        "world": {
            "width": 20.0,
            "height": 20.0,
            "max_energy": 100.0,
            "energy_decay_rate": 0.01,
            "resource_energy_gain": 30.0
        },
        "neural": {
            "random_seed": 42,
            "development_enabled": True,
            "reward_modulation_enabled": True,
            "structural_plasticity_enabled": True,
            "curiosity_enabled": True,
            "novelty_enabled": True,
            "prediction_enabled": True,
            "planning_enabled": True,
            "attention_enabled": True
        },
        "learning": {
            "learning_rate": 0.01,
            "plasticity_decay": 0.995,
            "hebbian_strength": 0.1,
            "stdp_time_constant": 20.0
        },
        "io": {
            "output_dir": "./output",
            "log_file": "nlm_session.log",
            "checkpoint_interval": 100,
            "save_format": "json"
        }
    }

def load_config_from_file(filepath: str) -> Dict[str, Any]:
    """Load configuration from JSON file"""
    try:
        with open(filepath, 'r') as f:
            config = json.load(f)
        return config
    except FileNotFoundError:
        print(f"Config file not found: {filepath}")
        return create_default_config()
    except json.JSONDecodeError as e:
        print(f"Error parsing config file: {e}")
        return create_default_config()

def save_config_to_file(config: Dict[str, Any], filepath: str) -> bool:
    """Save configuration to JSON file"""
    try:
        with open(filepath, 'w') as f:
            json.dump(config, f, indent=2)
        return True
    except Exception as e:
        print(f"Error saving config file: {e}")
        return False

def create_output_directory(base_dir: str = "./output") -> str:
    """Create output directory with timestamp"""
    timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
    output_dir = f"{base_dir}/session_{timestamp}"
    os.makedirs(output_dir, exist_ok=True)
    return output_dir

def log_message(message: str, level: str = "INFO", log_file: Optional[str] = None) -> None:
    """Log message with timestamp and level"""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    formatted_message = f"[{timestamp}] [{level}] {message}"
    
    print(formatted_message)
    
    if log_file:
        with open(log_file, 'a') as f:
            f.write(formatted_message + "\n")

def calculate_statistics(data: List[float]) -> Dict[str, float]:
    """Calculate basic statistics for a list of numbers"""
    if not data:
        return {"count": 0, "mean": 0.0, "std": 0.0, "min": 0.0, "max": 0.0}
    
    arr = np.array(data)
    return {
        "count": len(arr),
        "mean": float(np.mean(arr)),
        "std": float(np.std(arr)),
        "min": float(np.min(arr)),
        "max": float(np.max(arr)),
        "median": float(np.median(arr))
    }

def format_time(seconds: float) -> str:
    """Format seconds into human-readable time"""
    if seconds < 60:
        return f"{seconds:.2f} seconds"
    elif seconds < 3600:
        minutes = seconds / 60
        return f"{minutes:.2f} minutes"
    else:
        hours = seconds / 3600
        return f"{hours:.2f} hours"

def generate_report(title: str, data: Dict[str, Any]) -> str:
    """Generate a formatted report"""
    report = f"{title}\n"
    report += "=" * len(title) + "\n\n"
    
    for section, content in data.items():
        report += f"{section.upper()}:\n"
        if isinstance(content, dict):
            for key, value in content.items():
                report += f"  {key}: {value}\n"
        else:
            report += f"  {content}\n"
        report += "\n"
    
    return report

def validate_config(config: Dict[str, Any]) -> List[str]:
    """Validate configuration and return list of errors"""
    errors = []
    
    # Required sections
    required_sections = ["simulation", "world", "neural", "learning", "io"]
    for section in required_sections:
        if section not in config:
            errors.append(f"Missing required section: {section}")
    
    # Validate simulation parameters
    if "simulation" in config:
        sim = config["simulation"]
        if "steps" not in sim or sim["steps"] <= 0:
            errors.append("Simulation steps must be positive")
        if "vision_width" not in sim or sim["vision_width"] <= 0:
            errors.append("Vision width must be positive")
        if "vision_height" not in sim or sim["vision_height"] <= 0:
            errors.append("Vision height must be positive")
    
    # Validate world parameters
    if "world" in config:
        world = config["world"]
        if "width" not in world or world["width"] <= 0:
            errors.append("World width must be positive")
        if "height" not in world or world["height"] <= 0:
            errors.append("World height must be positive")
    
    return errors

def merge_configs(base_config: Dict[str, Any], override_config: Dict[str, Any]) -> Dict[str, Any]:
    """Merge two configurations, with override taking precedence"""
    result = base_config.copy()
    
    for section, override_values in override_config.items():
        if section in result and isinstance(result[section], dict) and isinstance(override_values, dict):
            result[section] = {**result[section], **override_values}
        else:
            result[section] = override_values
    
    return result

if __name__ == "__main__":
    # Example usage
    print("Testing NLM utility functions...")
    
    # Create default config
    config = create_default_config()
    print("✓ Created default configuration")
    
    # Save config
    output_dir = create_output_directory()
    config_file = f"{output_dir}/config.json"
    if save_config_to_file(config, config_file):
        print(f"✓ Saved configuration to {config_file}")
    
    # Load config
    loaded_config = load_config_from_file(config_file)
    print("✓ Loaded configuration")
    
    # Validate config
    errors = validate_config(loaded_config)
    if errors:
        print(f"✗ Config validation errors: {errors}")
    else:
        print("✓ Configuration is valid")
    
    # Calculate statistics
    data = [1.0, 2.0, 3.0, 4.0, 5.0]
    stats = calculate_statistics(data)
    print(f"✓ Calculated statistics: {stats}")
    
    # Generate report
    report_data = {
        "experiment": "Test Experiment",
        "parameters": {"steps": 100, "learning_rate": 0.01},
        "results": {"accuracy": 0.85, "loss": 0.15}
    }
    report = generate_report("Experiment Report", report_data)
    print("✓ Generated report")
    
    print("\nAll utility functions working correctly!")
