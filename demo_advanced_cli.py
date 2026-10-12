#!/usr/bin/env python3
"""
Example script demonstrating the enhanced NLM advanced CLI functionality.
This script shows practical usage of all the new features and improvements.
"""

import os
import sys
from pathlib import Path

# Add the current directory to the path for imports
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

from advanced_cli import NLMAdvancedCLI
def main():
    """Main demonstration function."""
    print("=" * 60)
    print("NLM Advanced CLI Demonstration")
    print("=" * 60)
    print()
    
    # Initialize the advanced CLI
    cli = NLMAdvancedCLI()
    
    # Example 1: Display help information
    print("1. Displaying help information:")
    print("-" * 40)
    cli.show_help()
    print()
    
    # Example 2: Create a custom configuration
    print("2. Creating a custom configuration example:")
    print("-" * 40)
    
    custom_config = {
        'brain': {
            'neuron_count': 5000,
            'simulation_steps': 100000,
            'timestep': 0.01,
            'enable_neuromodulation': True,
            'enable_development': True,
            'enable_curiosity': True,
            'neuromodulation_preset': 'balanced',
            'plasticity_preset': 'adaptive',
        },
        'world': {
            'width': 200.0,
            'height': 200.0,
            'vision_width': 16,
            'vision_height': 16,
            'enable_vision': True,
            'enable_audio': False,
        },
        'agent': {
            'energy_decay_rate': 0.0001,
            'health_threshold': 0.1,
            'enable_structural_plasticity': True,
            'memory_capacity': 500.0,
            'curiosity_threshold': 0.3,
        },
        'experiments': {
            'duration': 60.0,
            'repetitions': 3,
            'checkpoint_interval': 10.0,
            'save_checkpoints': True,
        }
    }
    
    # Create the configs directory if it doesn't exist
    configs_dir = Path("configs")
    configs_dir.mkdir(exist_ok=True)
    
    # Save the custom configuration
    import yaml
    config_path = configs_dir / "custom_research_config.yaml"
    with open(config_path, 'w') as f:
        yaml.dump(custom_config, f, default_flow_style=False, indent=2)
    
    print(f"Created custom configuration: {config_path}")
    print(f"Configuration includes:")
    print(f"  - {custom_config['brain']['neuron_count']} neurons")
    print(f"  - {custom_config['brain']['simulation_steps']} simulation steps")
    print(f"  - World size: {custom_config['world']['width']}x{custom_config['world']['height']}")
    print(f"  - Agent memory capacity: {custom_config['agent']['memory_capacity']}")
    print()
    
    # Example 3: Demonstrate validation
    print("3. Demonstrating configuration validation:")
    print("-" * 40)
    
    try:
        validated_config = cli.load_config(str(config_path))
        if validated_config:
            print("✓ Configuration loaded and validated successfully")
            print(f"✓ Loaded {len(validated_config)} configuration sections")
        else:
            print("✗ Configuration validation failed")
    except Exception as e:
        print(f"✗ Validation error: {e}")
    
    print()
    
    # Example 4: Show available export formats
    print("4. Available data export formats:")
    print("-" * 40)
    export_formats = ['json', 'csv', 'hdf5', 'pickle']
    for i, fmt in enumerate(export_formats, 1):
        print(f"  {i}. {fmt.upper()} - {get_format_description(fmt)}")
    print()
    
    # Example 5: Show advanced features
    print("5. Advanced CLI features available:")
    print("-" * 40)
    features = [
        "Custom experiment configuration management",
        "Advanced data export in multiple formats",
        "Real-time experiment monitoring",
        "Performance benchmarking tools",
        "Automated analysis and recommendations",
        "Checkpoint management",
        "Parameter sweep and optimization",
        "Reproducibility tracking",
        "Configuration validation",
        "Error handling and recovery"
    ]
    
    for i, feature in enumerate(features, 1):
        print(f"  {i:2d}. {feature}")
    print()
    
    # Example 6: Show usage examples
    print("6. Usage examples:")
    print("-" * 40)
    examples = [
        ("Run experiment", "./advanced_cli.py run configs/custom_research_config.yaml --duration 120 --checkpoint 30"),
        ("Create configuration", "./advanced_cli.py create \"Research Experiment\" configs/custom_research_config.yaml"),
        ("Analyze results", "./advanced_cli.py analyze results/experiment_1.json"),
        ("Export data", "./advanced_cli.py export data/results.json json output/scientific_data.json"),
        ("Benchmark", "./advanced_cli.py benchmark --duration 30"),
        ("Show help", "./advanced_cli.py help")
    ]
    
    for desc, cmd in examples:
        print(f"  {desc:25} {cmd}")
    print()
    
    # Example 7: Show configuration templates
    print("7. Configuration templates:")
    print("-" * 40)
    templates = [
        ("development.yaml", "Detailed debugging and logging"),
        ("production.yaml", "Optimized for performance"),
        ("research.yaml", "Maximum flexibility and control"),
        ("debug.yaml", "Intensive debugging mode"),
        ("optimization.yaml", "Performance-focused settings")
    ]
    
    for template, description in templates:
        print(f"  {template:20} - {description}")
    print()
    
    print("=" * 60)
    print("Demonstration complete!")
    print("=" * 60)
    print()
    print("Next steps:")
    print("1. Explore the advanced_cli.py source code")
    print("2. Create your own configurations in the configs/ directory")
    print("3. Run experiments using the advanced CLI")
    print("4. Analyze results and export data for research")
    print("5. Use the validation framework to ensure experiment quality")
    print()
    print("For more information, see:")
    print("- ADVANCED_IMPROVEMENTS.md (complete feature documentation)")
    print("- advanced_cli/README.md (CLI usage guide)")
    print("- NLM_IMPROVEMENTS_SUMMARY.md (comprehensive summary)")

def get_format_description(format_name):
    """Get description for export format."""
    descriptions = {
        'json': 'Standard scientific data format with metadata',
        'csv': 'Spreadsheet-compatible for data analysis',
        'hdf5': 'High-performance for large datasets',
        'pickle': 'Python object serialization for analysis'
    }
    return descriptions.get(format_name, 'Unknown format')
if __name__ == '__main__':
    main()
