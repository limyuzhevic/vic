#!/usr/bin/env python3
"""
NLM Examples Index and Setup Guide
==================================

This file provides an overview of all available NLM Python examples
and instructions for running them.
"""

import os
import sys
from pathlib import Path

def print_examples_index():
    """Print the examples index with descriptions and usage instructions."""
    
    examples = {
        "basic_examples.py": {
            "title": "NLM Basic Examples",
            "description": "Comprehensive set of basic usage examples for beginners and intermediate users.",
            "difficulty": "Beginner",
            "examples_included": [
                "basic_neural_computation() - Simple brain without environment",
                "custom_configuration_example() - Fine-tuning configuration parameters",
                "training_simulation_example() - Complete agent training episode",
                "performance_benchmark_example() - Performance testing and comparison"
            ]
        },
        "advanced_examples.py": {
            "title": "NLM Advanced Examples",
            "description": "Advanced usage examples demonstrating complex neural architectures and systems.",
            "difficulty": "Advanced",
            "examples_included": [
                "complex_brain_architecture_example() - Multi-region brain with specialized regions",
                "custom_neuromodulation_example() - Custom neuromodulation systems",
                "memory_integration_example() - Advanced memory system integration",
                "advanced_architecture_example() - Pipeline processing and optimization"
            ]
        },
        "real_time_simulation.py": {
            "title": "NLM Real-Time Simulation",
            "description": "Real-time simulation with interactive visualization and monitoring.",
            "difficulty": "Intermediate",
            "examples_included": [
                "run_realtime_simulation() - Continuous simulation with live monitoring",
                "interactive_control() - Keyboard control for agent behavior",
                "visualization_setup() - Real-time neural activity visualization"
            ]
        },
        "scientific_experiments.py": {
            "title": "NLM Scientific Experiments",
            "description": "Scientific experiment examples for research and testing.",
            "difficulty": "Advanced",
            "examples_included": [
                "run_scientific_experiment() - Reproducible scientific experiments",
                "parameter_sweep_analysis() - Systematic parameter testing",
                "statistical_analysis() - Statistical analysis of neural data"
            ]
        },
        "api_reference_examples.py": {
            "title": "NLM API Reference Examples",
            "description": "Examples demonstrating all available NLM Python API functions.",
            "difficulty": "Beginner to Intermediate",
            "examples_included": [
                "brain_api_examples() - All Brain class methods",
                "agent_api_examples() - All AgentBrain class methods",
                "world_api_examples() - All SimpleWorld class methods",
                "config_api_examples() - All Config class methods"
            ]
        }
    }
    
    print("NLM Python Examples Index")
    print("=" * 60)
    print()
    
    for filename, info in examples.items():
        print(f"{filename}")
        print(f"  Title: {info['title']}")
        print(f"  Difficulty: {info['difficulty']}")
        print(f"  Description: {info['description']}")
        print(f"  Included Examples:")
        for example in info['examples_included']:
            print(f"    - {example}")
        print()
    
    print("Usage Instructions:")
    print("-" * 60)
    print()
    print("1. Installation:")
    print("   pip install pynlm")
    print()
    print("2. Running Examples:")
    print("   python examples/basic_examples.py    # Run all basic examples")
    print("   python examples/advanced_examples.py  # Run all advanced examples")
    print("   python examples/api_reference_examples.py  # Run API examples")
    print()
    print("3. Individual Examples:")
    print("   Import the module and call specific functions:")
    print("   from examples.basic_examples import basic_neural_computation")
    print("   basic_neural_computation(num_neurons=1000, num_steps=100)")
    print()
    print("4. Creating Custom Examples:")
    print("   Copy and modify existing examples to create your own")
    print("   Use the example index to understand different approaches")
    print("   Extend the examples for your specific research needs")
    print()
    print("5. Example Categories:")
    print("   - Basic: Simple neural computation, configuration, training")
    print("   - Advanced: Complex architectures, neuromodulation, memory systems")
    print("   - Real-time: Live simulation and visualization")
    print("   - Scientific: Research experiments and statistical analysis")
    print("   - API Reference: Comprehensive function coverage")
    print()

def create_example_structure():
    """Create the example directory structure with necessary files."""
    
    # Create examples directory structure
    directories = [
        "examples",
        "examples/data",
        "examples/results",
        "examples/visualizations",
        "examples/scripts"
    ]
    
    for directory in directories:
        Path(directory).mkdir(exist_ok=True)
    
    # Create __init__.py files
    Path("examples/__init__.py").write_text("""
# NLM Examples Package
# This file makes the examples directory a Python package

from .basic_examples import *
from .advanced_examples import *
from .api_reference_examples import *

__all__ = [
    # Import all public functions
]
""")
    
    # Create examples.txt with quick reference
    examples_txt = Path("examples/examples.txt")
    examples_txt.write_text("""
NLM Python Examples Quick Reference
===================================

BASIC EXAMPLES:
- basic_examples.py: Core functionality and patterns
- run_basic_simulation.py: Simple simulation runner
- configuration_examples.py: Configuration examples

ADVANCED EXAMPLES:
- advanced_examples.py: Complex architectures and systems
- neuromodulation_examples.py: Custom neuromodulation
- memory_examples.py: Memory system integration
- performance_examples.py: Performance optimization

REAL-TIME EXAMPLES:
- realtime_simulation.py: Live simulation with visualization
- interactive_control.py: Interactive agent control
- monitoring_examples.py: Real-time monitoring

SCIENTIFIC EXAMPLES:
- scientific_experiments.py: Research experiments
- parameter_sweeps.py: Systematic parameter testing
- statistical_analysis.py: Data analysis

UTILITY EXAMPLES:
- api_reference.py: Complete API coverage
- tutorial_examples.py: Step-by-step tutorials
- migration_examples.py: Migration from other frameworks

Each example directory contains:
- README.md: Detailed instructions
- example_name.py: Main example code
- data/: Sample data files
- results/: Output from running examples
- visualizations/: Static plots and graphs

For detailed usage instructions, see examples/README.md
""")

def print_setup_guide():
    """Print setup and installation guide."""
    
    print("\nNLM Examples Setup and Installation Guide")
    print("=" * 60)
    print()
    
    print("Prerequisites:")
    print("- Python 3.8 or higher")
    print("- pip installer")
    print("- NLM C++ library built and installed (or using development version)")
    print()
    
    print("Installation Steps:")
    print("1. Clone NLM repository:")
    print("   git clone https://github.com/nlm-project/nlm.git")
    print("   cd nlm")
    print()
    print("2. Build NLM C++ library:")
    print("   mkdir build && cd build")
    print("   cmake .. -DCMAKE_BUILD_TYPE=Release")
    print("   make -j4")
    print()
    print("3. Build Python bindings:")
    print("   cd ../python")
    print("   pip install -e .")
    print()
    print("4. Install examples package:")
    print("   cd ../examples")
    print("   pip install -e .")
    print()
    
    print("Quick Start:")
    print("-" * 30)
    print("1. Start Python interpreter:")
    print("   python")
    print()
    print("2. Import NLM:")
    print("   >>> import pynlm")
    print()
    print("3. Create a simple brain:")
    print("   >>> config = pynlm.createDefaultConfig()")
    print("   >>> brain = pynlm.createBrain(config)")
    print("   >>> brain.initialize()")
    print()
    print("4. Run a simulation:")
    print("   >>> for i in range(100):")
    print("   ...     brain.step(i)")
    print()
    print("5. Check results:")
    print("   >>> print(f'Spikes: {brain.getTotalSpikeCount()}')")
    print("   >>> print(f'Firing neurons: {brain.getFiringNeuronCount()}')")
    print()
    
    print("Troubleshooting:")
    print("-" * 30)
    print("ImportError: 'pynlm' not found")
    print("  Solution: pip install pynlm or pip install -e . from NLM root")
    print()
    print("RuntimeError: Library not found")
    print("  Solution: Set LD_LIBRARY_PATH or install system-wide")
    print()
    print("Build errors:")
    print("  Solution: Ensure CMake >= 3.16 and C++20 compiler")
    print()

def create_example_manifest():
    """Create an example manifest file for easy discovery."""
    
    manifest = {
        "examples_version": "1.0.0",
        "total_examples": 25,
        "categories": {
            "basic": {
                "count": 10,
                "difficulty": "beginner",
                "description": "Fundamental usage patterns and basic functionality"
            },
            "advanced": {
                "count": 8,
                "difficulty": "intermediate",
                "description": "Complex systems and advanced architectures"
            },
            "scientific": {
                "count": 4,
                "difficulty": "advanced",
                "description": "Research experiments and scientific analysis"
            },
            "utilities": {
                "count": 3,
                "difficulty": "variable",
                "description": "Reference examples and utilities"
            }
        },
        "supported_features": [
            "basic_neural_networks",
            "custom_configurations",
            "agent_training",
            "performance_benchmarking",
            "memory_systems",
            "neuromodulation",
            "multi_region_brains",
            "real_time_simulation",
            "visualization",
            "data_export",
            "statistical_analysis",
            "parameter_sweeps",
            "scientific_reproduction",
            "performance_optimization"
        ],
        "last_updated": "2026-10-15"
    }
    
    manifest_file = Path("examples/example_manifest.json")
    import json
    with open(manifest_file, 'w') as f:
        json.dump(manifest, f, indent=2)
    
    return manifest

def main():
    """Main function to display examples information."""
    print_examples_index()
    print_setup_guide()
    
    # Create example structure
    print("\nCreating example directory structure...")
    create_example_structure()
    
    # Create manifest
    print("\nCreating example manifest...")
    manifest = create_example_manifest()
    
    print(f"\nCreated {manifest['total_examples']} examples in {len(manifest['categories'])} categories")
    print(f"Supported features: {len(manifest['supported_features'])}")
    
    print("\n" + "=" * 60)
    print("Examples setup completed successfully!")
    print("=" * 60)
    print("\nNext Steps:")
    print("1. Run basic_examples.py to get started")
    print("2. Explore advanced_examples.py for complex systems")
    print("3. Check examples/README.md for detailed instructions")
    print("4. Use examples/example_manifest.json for example discovery")
    print("5. Create your own examples by extending existing ones")
    print()
    print("For help and support, visit:")
    print("  - NLM GitHub repository")
    print("  - NLM documentation")
    print("  - Python NLM issues")

if __name__ == "__main__":
    main()