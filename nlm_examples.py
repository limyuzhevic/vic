#!/usr/bin/env python3

"""
NLM Improved Configuration Examples and Usage Guide

This script demonstrates the new features and improvements made to NLM.
It shows how to use the enhanced configuration system, validation tools,
and performance monitoring features.

Usage:
    python3 nlm_examples.py [example_name]

Examples:
    python3 nlm_examples.py basic_usage
    python3 nlm_examples.py validation
    python3 nlm_examples.py advanced_config
    python3 nlm_examples.py performance_monitor
"""

import os
import sys
import json
import subprocess
from pathlib import Path

# Add the Python bindings to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

print("NLM Examples and Usage Guide")
print("=" * 35)
def show_basic_usage():
    """Demonstrate basic usage of NLM with new configuration features."""
    print("\n1. Basic Usage Examples")
    print("-" * 25)
    
    print("\nExample 1: Basic configuration loading")
    print("```python")
    print("# Create a default configuration")
    print("import pynlm")
    print("config = pynlm.createDefaultConfig()")
    print("")
    print("# Load configuration from file")
    print("config.loadFromFile('configs/default.cfg')")
    print("")
    print("# Validate configuration")
    print("if config.validateSchema():")
    print("    print('Configuration is valid!')")
    print("else:")
    print("    print('Configuration has issues')")
    print("```")
    
    print("\nExample 2: Command-line configuration")
    print("```bash")
    print("# Command line usage")
    print("./config_validator configs/default.cfg --neuron_count=5000")
    print("```")
    
    print("\nExample 3: Python configuration validator")
    print("```bash")
    print("python3 config_validator.py --config configs/default.cfg --validate-only")
    print("```")
def show_validation_examples():
    """Demonstrate configuration validation features."""
    print("\n2. Configuration Validation Examples")
    print("-" * 35)
    
    print("\nValidation Features:")
    print("✓ Schema validation with required keys")
    print("✓ Value range validation")
    print("✓ Type checking")
    print("✓ Duplicate key detection")
    print("✓ Command-line argument validation")
    
    print("\nValidation Example:")
    print("```python")
    print("# Create and validate configuration")
    print("config = pynlm.createDefaultConfig()")
    print("")
    print("# Load configuration")
    print("config.loadFromFile('configs/default.cfg')")
    print("")
    print("# Check if configuration is valid")
    print("if config.validateSchema():")
    print("    # Use configuration")
    print("    neuron_count = config.get('neuron_count')")
    print("else:")
    print("    # Fix configuration")
    print("    config.set('neuron_count', 1000)")
    print("```")
    
    print("\nValidation Error Examples:")
    print("• Missing required key: 'neuron_count'")
    print("• Invalid value range: neuron_count = 1000000 (too high)")
    print("• Invalid type: setting string as integer")
    print("• Duplicate configuration key")
def show_advanced_config_examples():
    """Demonstrate advanced configuration options."""
    print("\n3. Advanced Configuration Examples")
    print("-" * 35)
    
    print("\nAdvanced Configuration Options:")
    print("```")
    
    # Read and display advanced options from the config file
    config_path = "configs/default.cfg"
    if os.path.exists(config_path):
        with open(config_path, 'r') as f:
            lines = f.readlines()
        
        # Find the advanced section
        in_advanced = False
        for i, line in enumerate(lines):
            if "Advanced configuration options for expert users" in line:
                in_advanced = True
            elif in_advanced and line.strip() and not line.startswith('#'):
                print(line.rstrip())
            elif in_advanced and "# Visualization settings" in line:
                break
    
    print("```")
    
    print("\nAdvanced Usage Example:")
    print("```python")
    print("# Enable advanced features")
    print("config = pynlm.createDefaultConfig()")
    print("")
    print("# Enable SIMD optimization")
    print("config.set('enable_simd', True)")
    print("")
    print("# Set thread count")
    print("config.set('max_threads', 8)")
    print("")
    print("# Enable memory pools")
    print("config.set('use_memory_pools', True)")
    print("")
    print("# Enable structural plasticity")
    print("config.set('enable_structural_plasticity', True)")
    print("")
    print("# Load advanced configuration")
    print("config.loadFromFile('configs/default.cfg')")
    print("")
    print("# Validate and use")
    print("if config.validateSchema():")
    print("    # Create brain with advanced configuration")
    print("    brain = pynlm.createBrain(config)")
    print("```")
def show_performance_monitor_examples():
    """Demonstrate performance monitoring features."""
    print("\n4. Performance Monitoring Examples")
    print("-" * 35)
    
    print("\nPerformance Monitoring Features:")
    print("✓ Real-time CPU usage monitoring")
    print("✓ Memory usage tracking")
    print("✓ Thread count monitoring")
    print("✓ Automatic performance recommendations")
    print("✓ Report generation in JSON format")
    
    print("\nPerformance Monitoring Example:")
    print("```bash")
    print("# Basic usage")
    print("python3 performance_monitor.py")
    print("")
    print("# With custom duration")
    print("python3 performance_monitor.py --duration 120")
    print("")
    print("# With custom configuration")
    print("python3 performance_monitor.py --config my_config.cfg --duration 60")
    print("```")
    
    print("\nGenerated Reports:")
    print("• performance_metrics.json - Detailed metrics")
    print("• Contains: CPU usage, memory usage, thread count")
    print("• Includes performance recommendations")
    print("• JSON format for easy integration")
def show_new_tools_examples():
    """Demonstrate new command-line tools."""
    print("\n5. New Command-Line Tools")
    print("-" * 25)
    
    print("\nNew Tools Available:")
    print("✓ config_validator - Configuration validation tool")
    print("✓ performance_monitor - Performance monitoring tool")
    print("✓ nlm_examples - This examples guide")
    
    print("\nTool Usage Examples:")
    print("```bash")
    print("# Configuration validation")
    print("./config_validator configs/default.cfg")
    print("")
    print("# Python configuration validation")
    print("python3 config_validator.py --validate-only")
    print("")
    print("# Performance monitoring")
    print("python3 performance_monitor.py --duration 60")
    print("")
    print("# Help")
    print("./config_validator --help")
    print("python3 config_validator.py --help")
    print("```")
    
    print("\nBuild Instructions:")
    print("```bash")
    print("# Build all tools")
    print("mkdir build && cd build")
    print("cmake ..")
    print("make -j4")
    print("")
    print("# Run config validator")
    print("./build/config_validator")
    print("")
    print("# Run with existing build")
    print("cd build")
    print("./nlm_phase6_demo")
    print("```")
def show_summary():
    """Show summary of improvements made."""
    print("\n6. Summary of Improvements Made")
    print("-" * 32)
    
    print("\nKey Improvements:")
    print("✓ Fixed duplicate dopamine_baseline in config file")
    print("✓ Fixed Config::saveToFile to save actual values")
    print("✓ Enhanced error handling throughout Config class")
    print("✓ Added configuration validation and schema support")
    print("✓ Created config_validator command-line tool")
    print("✓ Created performance_monitor monitoring tool")
    print("✓ Added Python interface for validation")
    print("✓ Added advanced configuration options (40+ new options)")
    print("✓ Fixed misleading documentation in README.md")
    print("✓ Updated Phase 6 status from 'Complete' to 'Work in Progress'")
    
    print("\nBenefits:")
    print("• Better usability for beginners (simplified validation)")
    print("• More features for advanced users (40+ new config options)")
    print("• Improved error handling and recovery")
    print("• Better documentation and guidance")
    print("• Performance monitoring capabilities")
    print("• Configuration validation tools")
    
    print("\nFiles Modified/Created:")
    print("• configs/default.cfg - Added advanced options")
    print("• src/core/Config/Config.hpp - Added validation methods")
    print("• src/core/Config/Config.cpp - Enhanced validation and error handling")
    print("• README.md - Updated Phase 6 status and documentation")
    print("• config_validator.cpp - C++ command-line tool")
    print("• config_validator.py - Python validation tool")
    print("• performance_monitor.py - Performance monitoring tool")
    print("• CMakeLists.txt - Added new tools to build system")
def main():
    """Main entry point."""
    print("NLM Examples and Usage Guide")
    print("=" * 35)
    
    if len(sys.argv) > 1:
        example_name = sys.argv[1]
        
        if example_name == "basic_usage":
            show_basic_usage()
        elif example_name == "validation":
            show_validation_examples()
        elif example_name == "advanced_config":
            show_advanced_config_examples()
        elif example_name == "performance_monitor":
            show_performance_monitor_examples()
        elif example_name == "new_tools":
            show_new_tools_examples()
        elif example_name == "summary":
            show_summary()
        else:
            print(f"Unknown example: {example_name}")
            print("Available examples: basic_usage, validation, advanced_config,")
            print("performance_monitor, new_tools, summary")
            return 1
    else:
        # Show all examples
        show_basic_usage()
        show_validation_examples()
        show_advanced_config_examples()
        show_performance_monitor_examples()
        show_new_tools_examples()
        show_summary()
    
    return 0
if __name__ == "__main__":
    sys.exit(main())