#!/usr/bin/env python3
"""
Advanced Configuration Tool for NLM
=================================

This script provides advanced configuration management for NLM (Neural Learning Machine)
Phase 6 and beyond. It includes configuration validation, schema checking, and
performance optimization.
"""

import json
import argparse
import sys
import os
from pathlib import Path
from typing import Dict, Any, Optional

# Add current directory to path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

import pynlm

class NLMConfigManager:
    """Advanced configuration manager for NLM."""
    
    def __init__(self, config_file: Optional[str] = None):
        self.config = pynlm.createDefaultConfig()
        self.config_file = config_file
        
        if config_file and os.path.exists(config_file):
            self.load_config(config_file)
    
    def load_config(self, config_file: str) -> bool:
        """Load configuration from file."""
        try:
            if self.config.loadFromFile(config_file):
                print(f"✓ Loaded configuration from: {config_file}")
                return True
            else:
                print(f"✗ Could not load configuration from: {config_file}")
                return False
        except Exception as e:
            print(f"✗ Error loading configuration: {e}")
            return False
    
    def save_config(self, config_file: str) -> bool:
        """Save configuration to file."""
        try:
            if self.config.saveToFile(config_file):
                print(f"✓ Saved configuration to: {config_file}")
                return True
            else:
                print(f"✗ Could not save configuration to: {config_file}")
                return False
        except Exception as e:
            print(f"✗ Error saving configuration: {e}")
            return False
    
    def validate_config(self) -> Dict[str, Any]:
        """Validate configuration and return results."""
        results = {
            'valid': True,
            'errors': [],
            'warnings': [],
            'suggestions': []
        }
        
        # Check required configuration
        if not self.config.has("brain.neuron_count"):
            results['warnings'].append("No neuron count specified, using default (1000)")
        
        # Validate neuron count
        if self.config.has("brain.neuron_count"):
            neuron_count = self.config.getOrInt("brain.neuron_count", 1000)
            if neuron_count <= 0:
                results['errors'].append("Neuron count must be positive")
                results['valid'] = False
            elif neuron_count > 100000:
                results['warnings'].append("Neuron count very high, may impact performance")
        
        # Validate connection probability
        if self.config.has("brain.connection_probability"):
            conn_prob = self.config.getOrFloat("brain.connection_probability", 0.1)
            if conn_prob < 0.0 or conn_prob > 1.0:
                results['errors'].append("Connection probability must be between 0.0 and 1.0")
                results['valid'] = False
        
        # Validate timestep
        if self.config.has("simulation_timestep"):
            timestep = self.config.getOrFloat("simulation_timestep", 0.001)
            if timestep <= 0.0 or timestep > 1.0:
                results['errors'].append("Simulation timestep must be positive and less than 1.0")
                results['valid'] = False
        
        # Validate plasticity parameters
        if self.config.has("plasticity.stdp.learning_rate"):
            lrn_rate = self.config.getOrFloat("plasticity.stdp.learning_rate", 0.001)
            if lrn_rate < 0.0:
                results['errors'].append("STDP learning rate must be non-negative")
                results['valid'] = False
        
        # Performance suggestions
        if self.config.has("brain.neuron_count"):
            neuron_count = self.config.getOrInt("brain.neuron_count", 1000)
            if neuron_count > 5000:
                results['suggestions'].append("For large neuron counts, consider enabling memory pools and vectorization")
        
        if self.config.has("brain.connection_probability"):
            conn_prob = self.config.getOrFloat("brain.connection_probability", 0.1)
            if conn_prob > 0.2:
                results['suggestions'].append("High connection probability may impact performance, consider reducing")
        
        return results
    
    def get_config_summary(self) -> str:
        """Get configuration summary."""
        summary = "=== NLM Configuration Summary ===\n"
        summary += f"Config file: {self.config_file or 'default'}\n"
        summary += f"Total configuration keys: {len(self.config.getKeys())}\n\n"
        
        # Group configuration by category
        categories = {
            "brain": [],
            "plasticity": [],
            "neuromod": [],
            "memory": [],
            "cognition": [],
            "development": [],
            "other": []
        }
        
        for key in self.config.getKeys():
            if key.startswith("brain."):
                categories["brain"].append(key)
            elif key.startswith("plasticity."):
                categories["plasticity"].append(key)
            elif key.startswith("neuromod."):
                categories["neuromod"].append(key)
            elif key.startswith("memory."):
                categories["memory"].append(key)
            elif key.startswith("cognition."):
                categories["cognition"].append(key)
            elif key.startswith("development."):
                categories["development"].append(key)
            else:
                categories["other"].append(key)
        
        # Print configuration by category
        for category, keys in categories.items():
            if keys:
                summary += f"\n{category.upper()} Configuration:\n"
                for key in keys:
                    value = self.config.toString(key)
                    summary += f"  {key}: {value}\n"
        
        return summary
    
    def set_value(self, key: str, value: Any) -> bool:
        """Set configuration value."""
        try:
            self.config.set(key, value, pynlm.ConfigSource.Runtime)
            print(f"✓ Set {key} = {value}")
            return True
        except Exception as e:
            print(f"✗ Error setting {key}: {e}")
            return False
    
    def get_value(self, key: str, default=None):
        """Get configuration value."""
        return self.config.getOr(key, default)
    
    def profile_performance(self, steps: int = 1000) -> Dict[str, Any]:
        """Profile performance of current configuration."""
        print(f"Profiling performance with {steps} steps...")
        
        brain = pynlm.createBrain(self.config)
        brain.initialize()
        
        import time
        start_time = time.time()
        
        for i in range(steps):
            brain.step(i)
        
        end_time = time.time()
        
        elapsed = end_time - start_time
        steps_per_second = steps / elapsed
        
        profile_results = {
            'steps': steps,
            'total_time': elapsed,
            'steps_per_second': steps_per_second,
            'total_neurons': brain.getTotalNeuronCount(),
            'total_spikes': brain.getTotalSpikeCount(),
            'average_firing_rate': brain.getAverageFiringRate(),
            'memory_usage': self._get_memory_usage(brain),
            'performance_rating': self._calculate_performance_rating(steps_per_second)
        }
        
        return profile_results
    
    def _get_memory_usage(self, brain) -> Dict[str, Any]:
        """Get memory usage statistics."""
        # This is a simplified version - real implementation would use system tools
        return {
            'estimated_mb': brain.getTotalNeuronCount() * 0.01,  # Rough estimate
            'working_memory_usage': 'medium',
            'episodic_memory_usage': 'low'
        }
    
    def _calculate_performance_rating(self, steps_per_second: float) -> str:
        """Calculate performance rating."""
        if steps_per_second > 1000:
            return "Excellent"
        elif steps_per_second > 500:
            return "Good"
        elif steps_per_second > 100:
            return "Fair"
        else:
            return "Poor"

def main():
    """Main entry point."""
    parser = argparse.ArgumentParser(
        description="Advanced Configuration Manager for NLM",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python config_manager.py --load config.json
  python config_manager.py --validate
  python config_manager.py --profile --steps 2000
  python config_manager.py --set brain.neuron_count 2000
  python config_manager.py --save my_config.json
        ""
    )
    
    parser.add_argument('--load', '-l', help='Load configuration from file')
    parser.add_argument('--save', '-s', help='Save configuration to file')
    parser.add_argument('--validate', action='store_true', help='Validate configuration')
    parser.add_argument('--profile', action='store_true', help='Profile performance')
    parser.add_argument('--steps', type=int, default=1000, help='Number of steps for profiling')
    parser.add_argument('--set', nargs=2, metavar=('KEY', 'VALUE'), action='append',
                       help='Set configuration value (key value)')
    parser.add_argument('--get', help='Get configuration value')
    parser.add_argument('--show', action='store_true', help='Show configuration summary')
    
    args = parser.parse_args()
    
    # Initialize configuration manager
    config_manager = NLMConfigManager(args.load)
    
    # Handle commands
    if args.validate:
        results = config_manager.validate_config()
        
        print("=== Configuration Validation Results ===")
        if results['valid']:
            print("✓ Configuration is valid")
        else:
            print("✗ Configuration has errors")
        
        if results['errors']:
            print("\nErrors:")
            for error in results['errors']:
                print(f"  - {error}")
        
        if results['warnings']:
            print("\nWarnings:")
            for warning in results['warnings']:
                print(f"  - {warning}")
        
        if results['suggestions']:
            print("\nSuggestions:")
            for suggestion in results['suggestions']:
                print(f"  - {suggestion}")
        
        return 0 if results['valid'] else 1
    
    if args.save:
        return 0 if config_manager.save_config(args.save) else 1
    
    if args.profile:
        results = config_manager.profile_performance(args.steps)
        
        print("=== Performance Profile ===")
        for key, value in results.items():
            if isinstance(value, dict):
                print(f"{key}:")
                for k, v in value.items():
                    print(f"  {k}: {v}")
            else:
                print(f"{key}: {value}")
        
        return 0
    
    if args.set:
        for key, value in args.set:
            # Try to infer value type
            try:
                if '.' in value:
                    float_value = float(value)
                    config_manager.set_value(key, float_value)
                elif value.lower() in ['true', 'false']:
                    bool_value = value.lower() == 'true'
                    config_manager.set_value(key, bool_value)
                elif value.isdigit():
                    int_value = int(value)
                    config_manager.set_value(key, int_value)
                else:
                    config_manager.set_value(key, value)
            except ValueError:
                config_manager.set_value(key, value)
        
        return 0
    
    if args.get:
        value = config_manager.get_value(args.get)
        if value is not None:
            print(f"{args.get}: {value}")
        else:
            print(f"{args.get}: not set")
        return 0
    
    if args.show:
        print(config_manager.get_config_summary())
        return 0
    
    # Default: show help
    parser.print_help()
    return 1

if __name__ == "__main__":
    sys.exit(main())