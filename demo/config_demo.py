#!/usr/bin/env python3
"""
Configuration System Demonstration Script
NLM Configuration System - Phase 6 Improvements

This script demonstrates the enhanced configuration system features:
1. Configuration file format support (INI, JSON)
2. Configuration validation
3. Configuration import/export
4. Configuration comparison and merging
5. Template usage
6. Command line interface
"""

import sys
import os
import json
import argparse
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

def print_header(title):
    """Print a formatted header"""
    print("\n" + "=" * 80)
    print(f" {title}")
    print("=" * 80)

def print_example(name, description, example):
    """Print an example with name, description, and code"""
    print(f"\n{name}:")
    print(f"  {description}")
    print(f"  {example}")

def demo_config_format_support():
    """Demonstrate configuration file format support"""
    print_header("Configuration File Format Support")
    
    print("\nThe NLM configuration system supports multiple file formats:")
    
    print_example(
        "INI-style (.cfg)",
        "The primary configuration format used by NLM - simple key-value pairs",
        """
        random_seed = 42
simulation_timestep = 0.001
neuron_count = 1000
region_count = 1
connection_probability = 0.1
stdp_ltp_weight = 0.02
stdp_ltd_weight = 0.015
stdp_tau = 20.0
working_memory_capacity = 100
max_episodes = 1000
        """.strip()
    )
    
    print_example(
        "JSON (.json)",
        "Hierarchical, type-aware configuration format",
        """
        {
    "name": "Phase 6 Configuration",
    "version": "6.0.0",
    "basic": {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 1,
        "connection_probability": 0.1
    },
    "plasticity": {
        "stdp": {
            "ltp_weight": 0.02,
            "ltd_weight": 0.015,
            "tau": 20.0
        }
    },
    "memory": {
        "working_memory": {
            "capacity": 100
        }
    }
}
        """.strip()
    )

def demo_config_validation():
    """Demonstrate configuration validation"""
    print_header("Configuration Validation")
    
    print("\nConfiguration validation ensures:")
    
    print("\n1. Type Validation:")
    print("   - Values must match expected types")
    print("   - Example: neuron_count must be an integer")
    print("   - Example: simulation_timestep must be a floating point number")
    
    print("\n2. Range Validation:")
    print("   - Values must be within valid ranges")
    print("   - Example: neuron_count > 0")
    print("   - Example: simulation_timestep between 0.0001 and 1.0")
    
    print("\n3. Presence Validation:")
    print("   - Required keys must be present")
    print("   - Example: random_seed, neuron_count, simulation_timestep")
    
    print("\n4. Logical Validation:")
    print("   - Values must be logically consistent")
    print("   - Example: neuron_count * region_count should be reasonable")
    
    print("\nExample validation errors:")
    print("  - 'Missing required configuration key: neuron_count'")
    print("  - 'neuron_count must be positive'")
    print("  - 'simulation_timestep must be between 0 and 1.0 seconds'")

def demo_config_import_export():
    """Demonstrate configuration import/export"""
    print_header("Configuration Import/Export")
    
    print("\nConfiguration import/export supports multiple formats:")
    
    print("\n1. Export Formats:")
    print("   - JSON: Hierarchical, type-aware format")
    print("   - INI: Flat key-value format")
    print("   - YAML: Human-readable format (planned)")
    
    print("\n2. Import Formats:")
    print("   - JSON: Import from JSON strings or files")
    print("   - INI: Import from INI-style configuration files")
    print("   - YAML: Import from YAML files (planned)")
    
    print("\nExample usage:")
    print("""
# Create configuration
config = Config()

# Load from file
if config.loadFromFile("configs/default.cfg"):
    print("Loaded configuration from file")

# Export to JSON
json_str = config.exportToJSON()
print("Configuration as JSON:")
print(json_str)

# Import from JSON
bool success = config.importFromJSON(json_str)
if success:
    print("Successfully imported configuration from JSON")

# Validate configuration
if !config.validateAll():
    errors = config.getValidationErrors()
    print("Configuration validation errors:")
    for error in errors:
        print(f"  - {error}")
    """)

def demo_config_comparison():
    """Demonstrate configuration comparison and merging"""
    print_header("Configuration Comparison and Merging")
    
    print("\nConfiguration comparison and merging:")
    
    print("\n1. Configuration Comparison:")
    print("   - Check if two configurations have the same values")
    print("   - Used for testing and validation")
    
    print("\n2. Configuration Merging:")
    print("   - Merge configuration values")
    print("   - Used for extending configurations")
    
    print("\nExample usage:")
    print("""
# Create two configurations
Config config1 = new Config()
Config config2 = new Config()

# Load configuration from files
config1.loadFromFile("configs/base.cfg")
config2.loadFromFile("configs/custom.cfg")

# Compare configurations
boolean haveSameValues = config1.hasSameValues(config2)
if (haveSameValues) {
    System.out.println("Configurations have the same values");
} else {
    System.out.println("Configurations have different values");
}

# Merge configurations
config1.merge(config2, ConfigSource::CommandLine);
System.out.println("Merged configurations");
    """)

def demo_template_usage():
    """Demonstrate template usage"""
    print_header("Template Usage")
    
    print("\nTemplates provide pre-defined configuration sets for common use cases:")
    
    print("\n1. Available Templates:")
    print("   - fast: Optimized for speed with fewer neurons")
    print("   - memory_research: Optimized for memory system research")
    print("   - plasticity_research: Optimized for plasticity rule research")
    print("   - full_integration: Complete Phase 6 configuration")
    print("   - custom: Template for creating custom configurations")
    
    print("\n2. Loading Templates:")
    print("""
# Load a built-in template
Config config = new Config();
if (config.loadFromTemplate("fast")) {
    System.out.println("Loaded fast template");
}

# Load a custom template
if (config.loadFromTemplate("my_custom_template.cfg")) {
    System.out.println("Loaded custom template");
}
    """)
    
    print("\n3. Template Categories:")
    print("   - By Use Case: Research, Performance, Development, Education")
    print("   - By Complexity: Simple, Standard, Advanced, Custom")
    print("   - By Domain: Memory Research, Plasticity Research, Integration Research")
    
    print("\n4. Template Best Practices:")
    print("   - Document your template with clear comments")
    print("   - Use consistent formatting")
    print("   - Include validation rules")
    print("   - Provide examples")
    print("   - Test your template thoroughly")

def demo_command_line():
    """Demonstrate command line interface"""
    print_header("Command Line Interface")
    
    print("\nThe NLM configuration system automatically parses command line arguments:")
    
    print("\n1. Key-Value Format:")
    print("   ./nlm --random_seed=42 --neuron_count=2000 --simulation_timestep=0.002")
    
    print("\n2. Short Format:")
    print("   ./nlm -seed 123 -neurons 500 -timestep 0.001")
    
    print("\n3. Configuration File:")
    print("   ./nlm --config configs/fast_config.cfg")
    
    print("\n4. Template Usage:")
    print("   ./nlm --template fast")
    print("   ./nlm --template memory_research")
    
    print("\n5. Template Management:")
    print("   ./nlm --list-templates")
    print("   ./nlm --template-info fast")
    print("   ./nlm --validate-template fast")
    
    print("\nCommand Line Precedence:")
    print("   1. Command line arguments")
    print("   2. Configuration files")
    print("   3. Default values")

def main():
    """Main function"""
    print_header("NLM Configuration System Demonstration")
    print("\nThis script demonstrates the enhanced configuration system features")
    print("implemented in NLM Phase 6.")
    
    # Parse command line arguments
    parser = argparse.ArgumentParser(description="NLM Configuration System Demonstration")
    parser.add_argument("--demo", choices=["all", "format", "validation", "import_export", "comparison", "templates", "command_line"], default="all", help="Which demonstration to run")
    parser.add_argument("--output", help="Output file for demonstrations")
    
    args = parser.parse_args()
    
    # Run demonstrations
    if args.demo in ["all", "format"]:
        demo_config_format_support()
    
    if args.demo in ["all", "validation"]:
        demo_config_validation()
    
    if args.demo in ["all", "import_export"]:
        demo_config_import_export()
    
    if args.demo in ["all", "comparison"]:
        demo_config_comparison()
    
    if args.demo in ["all", "templates"]:
        demo_template_usage()
    
    if args.demo in ["all", "command_line"]:
        demo_command_line()
    
    print("\n" + "=" * 80)
    print("Demonstration Complete")
    print("=" * 80)
    
    # Write output to file if specified
    if args.output:
        with open(args.output, "w") as f:
            f.write("NLM Configuration System Demonstration\n")
            f.write("================================\n")
            f.write("This file contains the output of the configuration system demonstration.\n")
            f.write("\n")
            f.write("See the main script for detailed demonstrations.\n")
        print(f"\nDemonstration written to: {args.output}")
    
    return 0

if __name__ == "__main__":
    sys.exit(main())