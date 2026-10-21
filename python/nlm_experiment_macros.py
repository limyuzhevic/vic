#!/usr/bin/env python3
"""
NLM Experiment Macros and Presets

This module provides experiment macros and presets for NLM,
enabling users to quickly set up common experiment configurations
and automation macros.
"""

import json
import os
import sys
from typing import Dict, List, Any, Optional
from dataclasses import dataclass, asdict
from datetime import datetime
import argparse

@dataclass
class ExperimentMacro:
    """Definition of an experiment macro"""
    name: str
    description: str
    category: str
    parameters: Dict[str, Any]
    commands: List[str]
    author: str = "NLM Team"
    version: str = "1.0.0"
    date_created: str = None
    
    def __post_init__(self):
        if self.date_created is None:
            self.date_created = datetime.now().strftime('%Y-%m-%d')

@dataclass
class ExperimentPreset:
    """Predefined experiment preset"""
    name: str
    description: str
    template: str
    parameters: Dict[str, Any]
    tags: List[str]
    difficulty: str = "intermediate"
    estimated_duration: float = 10.0
    required_memory: str = "medium"

class NLMExperimentMacroGenerator:
    """Generate experiment macros and presets for NLM"""
    
    def __init__(self, macro_dir: str = "macros", preset_dir: str = "presets"):
        self.macro_dir = macro_dir
        self.preset_dir = preset_dir
        self.experiment_macros = self.load_experiment_macros()
        self.experiment_presets = self.load_experiment_presets()
        self.ensure_directories()
    
    def ensure_directories(self):
        """Ensure directories exist"""
        for directory in [self.macro_dir, self.preset_dir]:
            if not os.path.exists(directory):
                os.makedirs(directory)
    
    def load_experiment_macros(self) -> Dict[str, Any]:
        """Load experiment macro definitions"""
        return {
            "plasticity_learning": {
                "name": "Plasticity Learning Experiment",
                "description": "Comprehensive study of synaptic plasticity mechanisms",
                "category": "neuroscience",
                "parameters": {
                    "duration": 50.0,
                    "timestep": 0.0005,
                    "neuron_count": 500,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "stimulation_pattern": "periodic",
                    "recording_frequency": "high",
                    "plasticity_rules": ["stdp", "hebbian", "structural"]
                },
                "commands": [
                    "python simulate_plasticity.py",
                    "nlm-analyze --type plasticity",
                    "nlm-plot --plasticity",
                    "nlm-benchmark --detailed"
                ],
                "author": "NLM Research Team",
                "version": "2.1.0"
            },
            "memory_capacity": {
                "name": "Memory Capacity Study",
                "description": "Investigation of memory system capacity and limits",
                "category": "cognition",
                "parameters": {
                    "duration": 30.0,
                    "timestep": 0.001,
                    "neuron_count": 200,
                    "working_memory_enabled": True,
                    "episodic_memory_enabled": True,
                    "semantic_memory_enabled": True,
                    "memory_load": "high",
                    "recall_test": True,
                    "memory_tracking": "continuous"
                },
                "commands": [
                    "python simulate_memory.py",
                    "nlm-analyze --type memory",
                    "nlm-plot --memory",
                    "nlm-benchmark --neurons 200"
                ],
                "author": "NLM Cognitive Team",
                "version": "1.8.0"
            },
            "neuromodulation_study": {
                "name": "Neuromodulation Effects Investigation",
                "description": "Study of dopamine, curiosity, and novelty effects",
                "category": "neurobiology",
                "parameters": {
                    "duration": 40.0,
                    "timestep": 0.0005,
                    "neuron_count": 300,
                    "neuromodulation_enabled": True,
                    "dopamine_baseline": 0.5,
                    "curiosity_baseline": 0.3,
                    "novelty_baseline": 0.2,
                    "reward_structure": "sparse",
                    "modulation_effects": ["dopamine", "curiosity", "novelty"],
                    "behavioral_correspondence": True
                },
                "commands": [
                    "python simulate_neuromod.py",
                    "nlm-analyze --type neuromodulation",
                    "nlm-plot --neuromodulation",
                    "nlm-benchmark --neurons 300"
                ],
                "author": "NLM Neurobiology Team",
                "version": "2.0.0"
            },
            "developmental_simulation": {
                "name": "Neural Development Simulation",
                "description": "Simulation of neural development and maturation process",
                "category": "development",
                "parameters": {
                    "duration": 100.0,
                    "timestep": 0.001,
                    "neuron_count": 100,
                    "development_enabled": True,
                    "initial_neurons": 50,
                    "synaptogenesis_rate": 0.001,
                    "pruning_rate": 0.0001,
                    "maturation_rate": 0.01,
                    "age_effects": True,
                    "stage_by_stage": True,
                    "neural_growth": True
                },
                "commands": [
                    "python simulate_development.py",
                    "nlm-analyze --type development",
                    "nlm-plot --development",
                    "nlm-benchmark --neurons 100"
                ],
                "author": "NLM Development Team",
                "version": "1.5.0"
            },
            "benchmark_suite": {
                "name": "Comprehensive Benchmark Suite",
                "description": "Performance benchmarking across multiple configurations",
                "category": "performance",
                "parameters": {
                    "neuron_counts": [100, 500, 1000, 5000],
                    "simulation_steps": [100, 500, 1000, 5000],
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "parallel_processing": True,
                    "output_format": "json",
                    "performance_analysis": True,
                    "benchmark_name": "NLM Comprehensive Benchmark"
                },
                "commands": [
                    "python run_benchmarks.py",
                    "nlm-benchmark --detailed",
                    "nlm-plot --benchmark",
                    "nlm-analyze --type performance"
                ],
                "author": "NLM Performance Team",
                "version": "3.0.0"
            },
            "integration_experiment": {
                "name": "System Integration Test",
                "description": "Comprehensive integration test of all NLM components",
                "category": "integration",
                "parameters": {
                    "duration": 60.0,
                    "timestep": 0.001,
                    "neuron_count": 200,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "development_enabled": True,
                    "environment_type": "IntegrationWorld",
                    "comprehensive_testing": True,
                    "system_validation": True
                },
                "commands": [
                    "python simulate_integration.py",
                    "nlm-validate --all",
                    "nlm-analyze --all",
                    "nlm-benchmark --detailed --neurons 200"
                ],
                "author": "NLM Integration Team",
                "version": "2.5.0"
            }
        }
    
    def load_experiment_presets(self) -> Dict[str, Any]:
        """Load experiment preset definitions"""
        return {
            "quick_test": {
                "name": "Quick Test",
                "description": "Fast test of basic NLM functionality",
                "template": "basic_simulation",
                "parameters": {
                    "duration": 5.0,
                    "timestep": 0.001,
                    "neuron_count": 50,
                    "plasticity_enabled": True,
                    "memory_enabled": False
                },
                "tags": ["quick", "test", "basic"],
                "difficulty": "beginner",
                "estimated_duration": 5.0,
                "required_memory": "low"
            },
            "research_prototype": {
                "name": "Research Prototype",
                "description": "Prototype for research experiments",
                "template": "plasticity_study",
                "parameters": {
                    "duration": 30.0,
                    "timestep": 0.0005,
                    "neuron_count": 500,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "data_collection": "continuous",
                    "analysis": "comprehensive"
                },
                "tags": ["research", "prototype", "advanced"],
                "difficulty": "intermediate",
                "estimated_duration": 30.0,
                "required_memory": "high"
            },
            "production_experiment": {
                "name": "Production Experiment",
                "description": "Production-ready experiment configuration",
                "template": "integration_experiment",
                "parameters": {
                    "duration": 120.0,
                    "timestep": 0.001,
                    "neuron_count": 1000,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "development_enabled": True,
                    "environment_type": "ProductionWorld",
                    "comprehensive_logging": True,
                    "error_handling": "robust",
                    "backup_system": True
                },
                "tags": ["production", "stable", "reliable"],
                "difficulty": "advanced",
                "estimated_duration": 120.0,
                "required_memory": "very_high"
            },
            "educational_demo": {
                "name": "Educational Demo",
                "description": "Demonstration for educational purposes",
                "template": "basic_simulation",
                "parameters": {
                    "duration": 15.0,
                    "timestep": 0.001,
                    "neuron_count": 100,
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "visualization_enabled": True,
                    "interactive_mode": True,
                    "teach_mode": True
                },
                "tags": ["educational", "demo", "interactive"],
                "difficulty": "beginner",
                "estimated_duration": 15.0,
                "required_memory": "low"
            },
            "large_scale_experiment": {
                "name": "Large Scale Experiment",
                "description": "Large-scale experiment with many neurons",
                "template": "benchmark_suite",
                "parameters": {
                    "neuron_counts": [1000, 5000, 10000],
                    "simulation_steps": [500, 2000, 5000],
                    "plasticity_enabled": True,
                    "memory_enabled": True,
                    "neuromodulation_enabled": True,
                    "parallel_processing": True,
                    "distributed_computing": True,
                    "output_format": "csv",
                    "performance_analysis": True
                },
                "tags": ["large_scale", "distributed", "advanced"],
                "difficulty": "expert",
                "estimated_duration": 300.0,
                "required_memory": "extreme"
            },
            "rapid_prototype": {
                "name": "Rapid Prototype",
                "description": "Quick prototype for concept validation",
                "template": "plasticity_study",
                "parameters": {
                    "duration": 10.0,
                    "timestep": 0.001,
                    "neuron_count": 200,
                    "plasticity_enabled": True,
                    "memory_enabled": False,
                    "neuromodulation_enabled": False,
                    "minimal_logging": True,
                    "quick_validation": True
                },
                "tags": ["rapid", "prototype", "quick"],
                "difficulty": "beginner",
                "estimated_duration": 10.0,
                "required_memory": "low"
            }
        }
    
    def create_experiment_macro(self, macro_name: str, output_file: str = None) -> bool:
        """
        Create an experiment macro
        
        Args:
            macro_name: Name of the macro
            output_file: Output file path
            
        Returns:
            True if successful, False otherwise
        """
        if macro_name not in self.experiment_macros:
            print(f"Error: Unknown macro '{macro_name}'")
            return False
        
        macro = self.experiment_macros[macro_name]
        target_file = output_file or f"{self.macro_dir}/{macro_name}.json"
        
        try:
            macro_data = {
                "macro_info": {
                    "name": macro["name"],
                    "description": macro["description"],
                    "category": macro["category"],
                    "author": macro["author"],
                    "version": macro["version"],
                    "date_created": datetime.now().strftime('%Y-%m-%d'),
                    "file_version": "1.0"
                },
                "parameters": macro["parameters"],
                "commands": macro["commands"],
                "metadata": {
                    "generator": "NLM Experiment Macro Generator",
                    "generated_at": datetime.now().isoformat(),
                    "template_used": macro_name
                }
            }
            
            with open(target_file, 'w') as f:
                json.dump(macro_data, f, indent=2)
            
            print(f"Experiment macro generated: {target_file}")
            print(f"Macro name: {macro['name']}")
            print(f"Description: {macro['description']}")
            return True
            
        except Exception as e:
            print(f"Error creating experiment macro: {e}")
            return False
    
    def create_experiment_preset(self, preset_name: str, output_file: str = None) -> bool:
        """
        Create an experiment preset
        
        Args:
            preset_name: Name of the preset
            output_file: Output file path
            
        Returns:
            True if successful, False otherwise
        """
        if preset_name not in self.experiment_presets:
            print(f"Error: Unknown preset '{preset_name}'")
            return False
        
        preset = self.experiment_presets[preset_name]
        target_file = output_file or f"{self.preset_dir}/{preset_name}.json"
        
        try:
            preset_data = {
                "preset_info": {
                    "name": preset["name"],
                    "description": preset["description"],
                    "template": preset["template"],
                    "tags": preset["tags"],
                    "difficulty": preset["difficulty"],
                    "estimated_duration": preset["estimated_duration"],
                    "required_memory": preset["required_memory"],
                    "file_version": "1.0"
                },
                "parameters": preset["parameters"],
                "metadata": {
                    "generator": "NLM Experiment Preset Generator",
                    "generated_at": datetime.now().isoformat(),
                    "preset_type": "predefined"
                }
            }
            
            with open(target_file, 'w') as f:
                json.dump(preset_data, f, indent=2)
            
            print(f"Experiment preset generated: {target_file}")
            print(f"Preset name: {preset['name']}")
            print(f"Description: {preset['description']}")
            return True
            
        except Exception as e:
            print(f"Error creating experiment preset: {e}")
            return False
    
    def generate_macro_collection(self, output_dir: str = "macro_collection"):
        """
        Generate a complete collection of experiment macros and presets
        
        Args:
            output_dir: Output directory
        """
        if not os.path.exists(output_dir):
            os.makedirs(output_dir)
        
        # Create macros directory
        macros_dir = os.path.join(output_dir, "macros")
        os.makedirs(macros_dir)
        
        # Create presets directory
        presets_dir = os.path.join(output_dir, "presets")
        os.makedirs(presets_dir)
        
        # Generate all macros
        for macro_name in self.experiment_macros:
            self.create_experiment_macro(macro_name, 
                                        os.path.join(macros_dir, f"{macro_name}.json"))
        
        # Generate all presets
        for preset_name in self.experiment_presets:
            self.create_experiment_preset(preset_name,
                                        os.path.join(presets_dir, f"{preset_name}.json"))
        
        # Generate index file
        index_file = os.path.join(output_dir, "index.json")
        self.generate_index_file(index_file, macros_dir, presets_dir)
        
        # Generate README
        readme_file = os.path.join(output_dir, "README.md")
        self.generate_readme(readme_file, macros_dir, presets_dir)
        
        print(f"\nMacro collection generated in: {output_dir}")
        print(f"Macros: {len(self.experiment_macros)} files")
        print(f"Presets: {len(self.experiment_presets)} files")
    
    def generate_index_file(self, index_path: str, macros_dir: str, presets_dir: str):
        """Generate index file for macro collection"""
        index_data = {
            "collection_info": {
                "name": "NLM Experiment Macros and Presets",
                "version": "1.0.0",
                "generated": datetime.now().isoformat(),
                "generator": "NLM Experiment Macro Generator"
            },
            "macros": [],
            "presets": []
        }
        
        # Add macros
        for macro_name in self.experiment_macros:
            macro_file = os.path.join(macros_dir, f"{macro_name}.json")
            if os.path.exists(macro_file):
                with open(macro_file, 'r') as f:
                    macro_data = json.load(f)
                index_data["macros"].append({
                    "name": macro_name,
                    "display_name": macro_data["macro_info"]["name"],
                    "description": macro_data["macro_info"]["description"],
                    "category": macro_data["macro_info"],
                    "file": f"macros/{macro_name}.json"
                })
        
        # Add presets
        for preset_name in self.experiment_presets:
            preset_file = os.path.join(presets_dir, f"{preset_name}.json")
            if os.path.exists(preset_file):
                with open(preset_file, 'r') as f:
                    preset_data = json.load(f)
                index_data["presets"].append({
                    "name": preset_name,
                    "display_name": preset_data["preset_info"]["name"],
                    "description": preset_data["preset_info"]["description"],
                    "tags": preset_data["preset_info"]["tags"],
                    "difficulty": preset_data["preset_info"]["difficulty"],
                    "file": f"presets/{preset_name}.json"
                })
        
        with open(index_path, 'w') as f:
            json.dump(index_data, f, indent=2)
    
    def generate_readme(self, readme_path: str, macros_dir: str, presets_dir: str):
        """Generate README file for macro collection"""
        readme_content = '''# NLM Experiment Macros and Presets

This directory contains experiment macros and presets for the Neural Learning Machine (NLM).

## Macros

Experiment macros are predefined configurations for specific experiments. They include:

- Parameters for neural networks, simulation settings, and analysis options
- Recommended commands for running the experiments
- Metadata about the experiment type and requirements

### Available Macros

'''
        
        # List macros
        for macro_name in self.experiment_macros:
            macro = self.experiment_macros[macro_name]
            readme_content += f'''- **{macro_name}**: {macro['name']}
  - Description: {macro['description']}
  - Category: {macro['category']}
  - File: `macros/{macro_name}.json`
  - Parameters: {', '.join(list(macro['parameters'].keys())[:3])}...

''
        
        readme_content += '''
## Presets

Experiment presets are quick configurations for common experimental needs. They include:

- Template configurations with optimized parameters
- Difficulty levels and estimated durations
- Memory requirements and technical specifications
- Tags for easy categorization

### Available Presets

'''
        
        # List presets
        for preset_name in self.experiment_presets:
            preset = self.experiment_presets[preset_name]
            readme_content += f'''- **{preset_name}**: {preset['name']}
  - Description: {preset['description']}
  - Difficulty: {preset['difficulty']}
  - Duration: {preset['estimated_duration']} seconds
  - Tags: {', '.join(preset['tags'])}
  - File: `presets/{preset_name}.json`

''
        
        readme_content += '''
## Usage

To use an experiment macro or preset:

1. Copy the configuration file to your experiment directory
2. Modify parameters as needed
3. Run the recommended commands

### Examples

To use the plasticity learning macro:
```bash
# Copy the macro configuration
cp macros/plasticity_learning.json ./config.json

# Run the recommended commands
python simulate_plasticity.py
nlm-analyze --type plasticity
nlm-plot --plasticity
nlm-benchmark --detailed
```

To use the memory capacity preset:
```bash
# Copy the preset configuration
cp presets/memory_capacity.json ./config.json

# Run the experiment
python simulate_memory.py
```

## Technical Details

- Generated by: NLM Experiment Macro Generator
- Version: 1.0.0
- Generated: {generation_time}
- Total Macros: {macro_count}
- Total Presets: {preset_count}

## Custom Macros

You can create custom macros by modifying existing ones or creating new ones from scratch:

1. Duplicate an existing macro file
2. Modify the JSON structure as needed
3. Update the metadata

## Customization

To customize a macro or preset, edit the JSON file and adjust:

- Parameters according to your experimental needs
- Commands to match your workflow
- Metadata as needed for documentation

This collection provides a comprehensive set of tools for conducting experiments with the NLM system.
'''.format(
            generation_time=datetime.now().strftime('%Y-%m-%d %H:%M:%S'),
            macro_count=len(self.experiment_macros),
            preset_count=len(self.experiment_presets)
        )
        
        with open(readme_path, 'w') as f:
            f.write(readme_content)
    
    def list_macros(self):
        """List available experiment macros"""
        print("=== Available Experiment Macros ===")
        
        for macro_name in self.experiment_macros:
            macro = self.experiment_macros[macro_name]
            print(f"\n{macro_name.upper()}:")
            print(f"  Name: {macro['name']}")
            print(f"  Description: {macro['description']}")
            print(f"  Category: {macro['category']}")
            print(f"  Author: {macro['author']}")
            print(f"  Version: {macro['version']}")
            print(f"  Key Parameters: {', '.join(list(macro['parameters'].keys())[:5])}")
            print(f"  Commands: {len(macro['commands'])}")
    
    def list_presets(self):
        """List available experiment presets"""
        print("=== Available Experiment Presets ===")
        
        for preset_name in self.experiment_presets:
            preset = self.experiment_presets[preset_name]
            print(f"\n{preset_name.upper()}:")
            print(f"  Name: {preset['name']}")
            print(f"  Description: {preset['description']}")
            print(f"  Difficulty: {preset['difficulty']}")
            print(f"  Duration: {preset['estimated_duration']} seconds")
            print(f"  Memory: {preset['required_memory']}")
            print(f"  Tags: {', '.join(preset['tags'])}")
    
    def print_usage(self):
        """Print usage information"""
        print("\nNLM Experiment Macros and Presets")
        print("=" * 40)
        print("\nCommands:")
        print("  generate-macro <name> [output_file]    Generate a specific experiment macro")
        print("  generate-preset <name> [output_file]    Generate a specific experiment preset")
        print("  generate-collection [output_dir]        Generate complete collection of macros and presets")
        print("  list-macros                               List all available macros")
        print("  list-presets                              List all available presets")
        print("\nExamples:")
        print("  nlm-macros generate-macro plasticity_learning")
        print("  nlm-macros generate-preset quick_test")
        print("  nlm-macros generate-collection my_experiment_collection")
        print("\nFor more information, refer to the documentation.")

class NLMExperimentMacroCLI:
    """Command-line interface for NLM experiment macro generator"""
    
    def __init__(self):
        self.generator = NLMExperimentMacroGenerator()
    
    def run_command(self, args):
        """Run command with given arguments"""
        if len(args) < 1:
            print("Error: No command specified")
            return False
        
        command = args[0]
        remaining_args = args[1:]
        
        try:
            if command == "generate-macro":
                self.handle_generate_macro(remaining_args)
            elif command == "generate-preset":
                self.handle_generate_preset(remaining_args)
            elif command == "generate-collection":
                self.handle_generate_collection(remaining_args)
            elif command == "list-macros":
                self.generator.list_macros()
            elif command == "list-presets":
                self.generator.list_presets()
            else:
                print(f"Unknown command: {command}")
                self.print_help()
                return False
                
        except Exception as e:
            print(f"Error: {e}")
            return False
        
        return True
    
    def handle_generate_macro(self, args):
        """Handle generate-macro command"""
        if len(args) < 1:
            print("Error: Missing macro name")
            return False
        
        macro_name = args[0]
        output_file = args[1] if len(args) > 1 else None
        
        print(f"Generating experiment macro: {macro_name}")
        return self.generator.create_experiment_macro(macro_name, output_file)
    
    def handle_generate_preset(self, args):
        """Handle generate-preset command"""
        if len(args) < 1:
            print("Error: Missing preset name")
            return False
        
        preset_name = args[0]
        output_file = args[1] if len(args) > 1 else None
        
        print(f"Generating experiment preset: {preset_name}")
        return self.generator.create_experiment_preset(preset_name, output_file)
    
    def handle_generate_collection(self, args):
        """Handle generate-collection command"""
        output_dir = args[0] if len(args) > 0 else "macro_collection"
        print(f"Generating complete collection of macros and presets in: {output_dir}")
        self.generator.generate_macro_collection(output_dir)
        return True
    
    def print_help(self):
        """Print help information"""
        print("NLM Experiment Macros and Presets")
        print("=" * 40)
        print("\nCommands:")
        print("  generate-macro <name> [output_file]    Generate a specific experiment macro")
        print("  generate-preset <name> [output_file]    Generate a specific experiment preset")
        print("  generate-collection [output_dir]        Generate complete collection of macros and presets")
        print("  list-macros                               List all available macros")
        print("  list-presets                              List all available presets")
        print("\nAvailable Macros:")
        for macro_name in self.generator.experiment_macros:
            print(f"  {macro_name}")
        print("\nAvailable Presets:")
        for preset_name in self.generator.experiment_presets:
            print(f"  {preset_name}")

# Standalone execution
def main():
    """Main function for standalone execution"""
    cli = NLMExperimentMacroCLI()
    
    if len(sys.argv) < 2:
        cli.print_help()
        return 1
    
    success = cli.run_command(sys.argv[1:])
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
