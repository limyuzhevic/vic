#!/usr/bin/env python3

"""
NLM Python Configuration Validation and Diagnostics Tool

A Python interface for validating and managing NLM configurations.
This provides the same functionality as the C++ config_validator tool
but with a more user-friendly Python API.

Usage:
    python3 config_validator.py [options]

Options:
    --config FILE          Configuration file to validate
    --validate-only        Validate configuration and exit
    --diagnose             Show configuration diagnostics
    --help                 Show help message
    --version              Show version information

Examples:
    python3 config_validator.py --config configs/default.cfg
    python3 config_validator.py --validate-only
    python3 config_validator.py --diagnose
"""

import json
import os
import sys
from typing import Dict, Any, Optional, List

# Add the Python bindings to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False
    print("Warning: pynlm module not available. Some functionality will be limited.")
class ConfigValidator:
    """Configuration validation and diagnostics tool for NLM."""
    
    def __init__(self):
        self.version = "1.0.0"
        self.default_config_file = "configs/default.cfg"
        
        if PYNLM_AVAILABLE:
            self.config = pynlm.createDefaultConfig()
        else:
            self.config = None
    
    def validate_config_file(self, filepath: str) -> bool:
        """Validate a configuration file."""
        print(f"Validating configuration file: {filepath}")
        print("=" * 50)
        
        if not os.path.exists(filepath):
            print(f"ERROR: Configuration file not found: {filepath}")
            return False
        
        if PYNLM_AVAILABLE:
            try:
                if not self.config.loadFromFile(filepath):
                    print(f"ERROR: Failed to load configuration file: {filepath}")
                    return False
                
                print("✓ Configuration file loaded successfully")
                
                # Validate schema
                if self.config.validateSchema():
                    print("✓ Configuration schema validation passed")
                else:
                    print("✗ Configuration schema validation failed")
                    print("  Missing required keys or invalid values")
                    return False
                
                # Show configuration summary
                print("\nConfiguration Summary:")
                print("-" * 30)
                print(self.config.summary())
                
                return True
                
            except Exception as e:
                print(f"ERROR: {str(e)}")
                return False
        else:
            # Fallback: basic file validation
            print(f"✗ pynlm not available. Performing basic file validation...")
            
            try:
                with open(filepath, 'r') as f:
                    content = f.read()
                
                # Check for required keys
                required_keys = ['random_seed', 'simulation_timestep', 'neuron_count', 
                               'region_count', 'connection_probability', 'plasticity_learning_rate',
                               'environment_name', 'max_simulation_steps']
                
                missing_keys = []
                for key in required_keys:
                    if f"{key} =" not in content and f'{key} = "' not in content:
                        missing_keys.append(key)
                
                if missing_keys:
                    print(f"✗ Missing required configuration keys: {', '.join(missing_keys)}")
                    return False
                
                print("✓ Basic file validation passed")
                return True
                
            except Exception as e:
                print(f"ERROR: {str(e)}")
                return False
    
    def validate_command_line(self, argc: int, argv: List[str]) -> bool:
        """Validate command-line configuration."""
        print("Validating command-line configuration...")
        print("=" * 50)
        
        if not PYNLM_AVAILABLE:
            print("✗ pynlm not available. Skipping command-line validation.")
            return False
        
        try:
            # Parse command line arguments
            if not self.config.loadFromArgs(argc, argv):
                print("ERROR: Failed to parse command-line arguments")
                return False
            
            print("✓ Command-line arguments parsed successfully")
            
            # Validate schema
            if self.config.validateSchema():
                print("✓ Command-line configuration schema validation passed")
            else:
                print("✗ Command-line configuration schema validation failed")
                print("  Missing required keys or invalid values")
                return False
            
            # Show configuration summary
            print("\nCommand-Line Configuration Summary:")
            print("-" * 40)
            print(self.config.summary())
            
            return True
            
        except Exception as e:
            print(f"ERROR: {str(e)}")
            return False
    
    def diagnose_config(self) -> bool:
        """Show configuration diagnostics."""
        print("Configuration Diagnostics")
        print("=" * 30)
        
        if PYNLM_AVAILABLE:
            try:
                # Load default configuration
                if not self.config.loadFromFile(self.default_config_file):
                    print(f"Warning: Could not load default configuration: {self.default_config_file}")
                
                # Show diagnostics
                print("\nSystem Configuration Diagnostics:")
                print("-" * 35)
                print(self.config.getDiagnostics())
                
                # Performance recommendations
                print("\nPerformance Recommendations:")
                print("-" * 30)
                
                neuron_count = self.config.getOr("neuron_count", 0)
                connection_prob = self.config.getOr("connection_probability", 0.0)
                max_steps = self.config.getOr("max_simulation_steps", 0)
                
                if neuron_count > 10000:
                    print("  • Large neuron count - consider enabling memory pools")
                
                if connection_prob > 0.5:
                    print("  • High connection probability - consider SIMD optimization")
                
                if max_steps > 10000:
                    print("  • Large simulation steps - consider parallel processing")
                
                # Advanced options
                print("\nAvailable Advanced Options:")
                print("-" * 25)
                print("  • --enable_simd          Enable SIMD optimization")
                print("  • --max_threads=<n>      Set maximum thread count")
                print("  • --enable_profiling     Enable performance profiling")
                print("  • --enable_debug_mode    Enable debug mode")
                print("  • --enable_structural_plasticity Enable structural plasticity")
                print("  • --enable_prediction_system     Enable prediction system")
                print("  • --enable_working_memory       Enable working memory")
                
                return True
                
            except Exception as e:
                print(f"ERROR: {str(e)}")
                return False
        else:
            print("✗ pynlm not available. Cannot generate diagnostics.")
            return False
    
    def show_help(self):
        """Show help information."""
        help_text = """
NLM Configuration Validation Tool
================================

This tool validates and manages NLM configurations.

Usage:
    python3 config_validator.py [options]

Options:
    --config FILE          Configuration file to validate (default: configs/default.cfg)
    --validate-only        Validate configuration and exit
    --diagnose             Show configuration diagnostics
    --help                 Show this help message
    --version              Show version information

Examples:
    python3 config_validator.py --config configs/default.cfg
    python3 config_validator.py --validate-only
    python3 config_validator.py --diagnose

Configuration Validation Rules:
    • Required keys: random_seed, simulation_timestep, neuron_count, region_count,
      connection_probability, plasticity_learning_rate, environment_name,
      max_simulation_steps
    • Value ranges:
      - random_seed: 0 to 2147483647
      - simulation_timestep: 0.0001 to 0.1
      - neuron_count: 1 to 1000000
      - region_count: 1 to 100
      - connection_probability: 0.0 to 1.0
      - plasticity_learning_rate: 0.0 to 1.0
      - dopamine_baseline: 0.0 to 1.0
      - reward_discount_factor: 0.0 to 1.0
      - environment_width/height: 1 to 10000
      - max_simulation_steps: 1 to 1000000

Performance Recommendations:
    • Large neuron count (>10,000): Consider enabling memory pools
    • High connection probability (>0.5): Consider SIMD optimization
    • Large simulation steps (>10,000): Consider parallel processing

Advanced Options Available in configs/default.cfg:
    • enable_simd: Enable SIMD optimization
    • max_threads: Set maximum thread count
    • enable_profiling: Enable performance profiling
    • enable_debug_mode: Enable debug mode
    • enable_structural_plasticity: Enable structural plasticity
    • enable_prediction_system: Enable prediction system
    • enable_working_memory: Enable working memory

For C++ version, use: config_validator executable
"""
        print(help_text)
    
    def show_version(self):
        """Show version information."""
        print(f"NLM Configuration Validator v{self.version}")
        print("Built with Python bindings support")
        if PYNLM_AVAILABLE:
            print("✓ pynlm module available")
        else:
            print("⚠ pynlm module not available")
def main():
    """Main entry point."""
    print("NLM Python Configuration Validator")
    print("=" * 35)
    
    validator = ConfigValidator()
    
    if len(sys.argv) == 1:
        validator.show_help()
        return 0
    
    try:
        arg1 = sys.argv[1]
        
        if arg1 == "--help" or arg1 == "-h":
            validator.show_help()
            return 0
        elif arg1 == "--version":
            validator.show_version()
            return 0
        elif arg1 == "--validate-only":
            success = validator.validate_config_file(validator.default_config_file)
            return 0 if success else 1
        elif arg1 == "--diagnose":
            success = validator.diagnose_config()
            return 0 if success else 1
        elif arg1 == "--config":
            if len(sys.argv) > 2:
                config_file = sys.argv[2]
                success = validator.validate_config_file(config_file)
                return 0 if success else 1
            else:
                print("ERROR: --config option requires a filename")
                return 1
        else:
            # Check if first argument is a file that exists
            if os.path.exists(arg1):
                # It's a file, validate config file
                if len(sys.argv) > 2:
                    # Parse additional key=value pairs
                    success = validator.validate_command_line(len(sys.argv), sys.argv)
                    return 0 if success else 1
                else:
                    success = validator.validate_config_file(arg1)
                    return 0 if success else 1
            else:
                # It's not a file, assume it's a key=value pair or command-line config
                success = validator.validate_command_line(len(sys.argv), sys.argv)
                return 0 if success else 1
                
    except Exception as e:
        print(f"ERROR: {str(e)}")
        return 1
if __name__ == "__main__":
    sys.exit(main())