#!/usr/bin/env python3
"""
CLI Examples and Demonstrations for NLM

Contains comprehensive examples and demonstrations for NLM CLI commands.
"""

import json
import time
from pathlib import Path
from typing import Dict, Any, List
from datetime import datetime

import click
from rich.console import Console
from rich.table import Table
from rich.syntax import Syntax

from nlm_cli import NLMCLI

console = Console()

class CliExamples:
    """Handle CLI examples and demonstrations."""
    
    def __init__(self, nlm_cli: NLMCLI):
        self.nlm_cli = nlm_cli
    
    def handle(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Handle CLI examples."""
        command = args.get('command')
        
        handlers = {
            'basic_usage': self.show_basic_usage,
            'configuration_examples': self.show_configuration_examples,
            'brain_examples': self.show_brain_examples,
            'simulation_examples': self.show_simulation_examples,
            'experiment_examples': self.show_experiment_examples,
            'analysis_examples': self.show_analysis_examples,
            'plugin_examples': self.show_plugin_examples,
            'system_examples': self.show_system_examples,
            'advanced_examples': self.show_advanced_examples,
        }
        
        if command in handlers:
            return handlers[command](args)
        else:
            raise click.ClickException(f"Unknown examples command: {command}")
    
    def show_basic_usage(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show basic NLM CLI usage examples."""
        examples = {
            'title': 'Basic NLM CLI Usage',
            'examples': [
                {
                    'title': 'Getting Started',
                    'description': 'Basic commands to get started with NLM',
                    'commands': [
                        'nlm-cli --help',
                        'nlm-cli config list',
                        'nlm-cli brain create --name mybrain',
                        'nlm-cli simulation start --name test_sim --steps 100'
                    ]
                },
                {
                    'title': 'Configuration Management',
                    'description': 'Common configuration tasks',
                    'commands': [
                        'nlm-cli config create --name default_config --description "Default configuration"',
                        'nlm-cli config edit --name default_config --parameters "{\"neurons\": 1000}"',
                        'nlm-cli config backup --name default_config --output backup.json',
                        'nlm-cli config restore --name default_config --backup backup.json'
                    ]
                },
                {
                    'title': 'Brain Management',
                    'description': 'Brain creation and management',
                    'commands': [
                        'nlm-cli brain create --name experimental_brain --config default_config',
                        'nlm-cli brain load --name loaded_brain --filepath mybrain.json',
                        'nlm-cli brain inspect --name experimental_brain',
                        'nlm-cli brain compare --brain1 experimental_brain --brain2 loaded_brain'
                    ]
                },
                {
                    'title': 'Simulation Control',
                    'description': 'Running simulations',
                    'commands': [
                        'nlm-cli simulation start --name main_experiment --steps 500 --auto_step',
                        'nlm-cli simulation pause --name main_experiment',
                        'nlm-cli simulation resume --name main_experiment',
                        'nlm-cli simulation checkpoint --name main_experiment --filepath checkpoint.json',
                        'nlm-cli simulation restore --name main_experiment --filepath checkpoint.json'
                    ]
                },
                {
                    'title': 'Experiment Management',
                    'description': 'Creating and managing experiments',
                    'commands': [
                        'nlm-cli experiment create --name first_exp --description "First experiment"',
                        'nlm-cli experiment run --name first_exp --steps 100 --time_scale 2.0',
                        'nlm-cli experiment list',
                        'nlm-cli experiment compare --name1 first_exp --name2 second_exp'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Basic NLM CLI usage examples displayed.",
            'examples': examples
        }
    
    def show_configuration_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show configuration management examples."""
        examples = {
            'title': 'Configuration Management Examples',
            'examples': [
                {
                    'title': 'Creating Configurations',
                    'description': 'Different ways to create configurations',
                    'commands': [
                        '# Create simple config
nlm-cli config create --name simple_config',
                        
                        '# Create with JSON parameters
nlm-cli config create --name detailed_config --parameters "{\"neurons\": 1000, \"synapses\": 10000, \"region_count\": 4}"',
                        
                        '# Create from file
nlm-cli config create --name file_config --parameters config_params.json'
                    ]
                },
                {
                    'title': 'Working with Configurations',
                    'description': 'Common configuration tasks',
                    'commands': [
                        '# List all configurations
nlm-cli config list --all',
                        
                        '# Show specific configuration
nlm-cli config show --name detailed_config',
                        
                        '# Validate configuration
nlm-cli config validate --name detailed_config',
                        
                        '# Edit configuration
nlm-cli config edit --name detailed_config --parameters "{\"neurons\": 2000}"'
                    ]
                },
                {
                    'title': 'Configuration Backups',
                    'description': 'Backup and restore configurations',
                    'commands': [
                        '# Backup configuration
nlm-cli config backup --name detailed_config --output detailed_config_backup.json',
                        
                        '# Restore configuration
nlm-cli config restore --name detailed_config --backup detailed_config_backup.json --force',
                        
                        '# Export configuration
nlm-cli config export --name detailed_config --output exported_config.json',
                        
                        '# Import configuration
nlm-cli config import --input exported_config.json --name imported_config'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Configuration management examples displayed.",
            'examples': examples
        }
    
    def show_brain_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show brain management examples."""
        examples = {
            'title': 'Brain Management Examples',
            'examples': [
                {
                    'title': 'Creating Brains',
                    'description': 'Creating brain instances with different configurations',
                    'commands': [
                        '# Create brain with default config
nlm-cli brain create --name basic_brain',
                        
                        '# Create brain with custom config
nlm-cli brain create --name experimental_brain --config detailed_config',
                        
                        '# Create brain with parameters
nlm-cli brain create --name custom_brain --parameters "{\"neuron_type\": 0, \"region_count\": 8}"',
                        
                        '# Create brain with debug mode
nlm-cli brain create --name debug_brain --debug'
                    ]
                },
                {
                    'title': 'Brain Operations',
                    'description': 'Common brain operations',
                    'commands': [
                        '# Load brain from file
nlm-cli brain load --name saved_brain --filepath mybrain.json',
                        
                        '# Save brain to file
nlm-cli brain save --name saved_brain --filepath export_brain.json',
                        
                        '# Inspect brain statistics
nlm-cli brain inspect --name saved_brain',
                        
                        '# Compare brain models
nlm-cli brain compare --brain1 basic_brain --brain2 experimental_brain',
                        
                        '# Export brain
nlm-cli brain export --name basic_brain --output basic_brain_export.json',
                        
                        '# Import brain
nlm-cli brain import --input basic_brain_export.json --name imported_brain'
                    ]
                },
                {
                    'title': 'Brain Statistics',
                    'description': 'Getting brain statistics and information',
                    'commands': [
                        '# Get brain stats
nlm-cli brain stats --name basic_brain',
                        
                        '# Reset brain
nlm-cli brain reset --name basic_brain',
                        
                        '# List available brain files
nlm-cli brain list --directory /path/to/brain/files'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Brain management examples displayed.",
            'examples': examples
        }
    
    def show_simulation_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show simulation control examples."""
        examples = {
            'title': 'Simulation Control Examples',
            'examples': [
                {
                    'title': 'Starting Simulations',
                    'description': 'Different ways to start simulations',
                    'commands': [
                        '# Run simulation with default settings
nlm-cli simulation start --name test_sim --steps 100',
                        
                        '# Run with progress bar
nlm-cli simulation start --name sim_with_progress --steps 500',
                        
                        '# Run with step mode
nlm-cli simulation start --name step_sim --steps 200 --auto_step',
                        
                        '# Run simulation with time scale
nlm-cli simulation start --name accelerated_sim --steps 100 --time_scale 2.0'
                    ]
                },
                {
                    'title': 'Simulation Control',
                    'description': 'Controlling running simulations',
                    'commands': [
                        '# Pause simulation
nlm-cli simulation pause --name running_sim',
                        
                        '# Resume simulation
nlm-cli simulation resume --name running_sim',
                        
                        '# Step through simulation
nlm-cli simulation step --name running_sim --step_number 50',
                        
                        '# Stop simulation
nlm-cli simulation stop --name running_sim',
                        
                        '# Get simulation status
nlm-cli simulation status --name running_sim',
                        
                        '# Get simulation metrics
nlm-cli simulation metrics --name running_sim'
                    ]
                },
                {
                    'title': 'Checkpoints and Recovery',
                    'description': 'Managing simulation checkpoints',
                    'commands': [
                        '# Create checkpoint
nlm-cli simulation checkpoint --name running_sim --filepath checkpoint.json',
                        
                        '# List checkpoints
nlm-cli simulation checkpoint_list',
                        
                        '# Restore from checkpoint
nlm-cli simulation checkpoint_restore --name running_sim --filepath checkpoint.json',
                        
                        '# Set simulation speed
nlm-cli simulation speed --name running_sim --speed 1.5',
                        
                        '# Enable step mode
nlm-cli simulation step_mode --name running_sim --enabled true'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Simulation control examples displayed.",
            'examples': examples
        }
    
    def show_experiment_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show experiment management examples."""
        examples = {
            'title': 'Experiment Management Examples',
            'examples': [
                {
                    'title': 'Creating Experiments',
                    'description': 'Creating and configuring experiments',
                    'commands': [
                        '# Create simple experiment
nlm-cli experiment create --name basic_exp --description "Basic experiment"',
                        
                        '# Create experiment with brain
nlm-cli experiment create --name brain_exp --brain_name mybrain',
                        
                        '# Create experiment with config
nlm-cli experiment create --name config_exp --config_name detailed_config',
                        
                        '# Create experiment with parameters
nlm-cli experiment create --name param_exp --parameters "{\"steps\": 100, \"time_scale\": 2.0}"'
                    ]
                },
                {
                    'title': 'Running Experiments',
                    'description': 'Running experiments with different configurations',
                    'commands': [
                        '# Run experiment
nlm-cli experiment run --name basic_exp --steps 100',
                        
                        '# Run experiment with time scale
nlm-cli experiment run --name accelerated_exp --steps 100 --time_scale 2.0',
                        
                        '# Run experiment in step mode
nlm-cli experiment run --name step_exp --steps 50 --auto_step',
                        
                        '# List experiments
nlm-cli experiment list --all',
                        
                        '# Show experiment details
nlm-cli experiment show --name basic_exp'
                    ]
                },
                {
                    'title': 'Experiment Results',
                    'description': 'Managing experiment results and comparisons',
                    'commands': [
                        '# Compare experiments
nlm-cli experiment compare --name1 exp1 --name2 exp2',
                        
                        '# Get experiment results
nlm-cli experiment results --name basic_exp',
                        
                        '# Export experiment results
nlm-cli experiment export --name basic_exp --output results.json',
                        
                        '# Import experiment results
nlm-cli experiment import --input results.json --name imported_exp',
                        
                        '# Pause experiment
nlm-cli experiment pause --name running_exp',
                        
                        '# Resume experiment
nlm-cli experiment resume --name paused_exp'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Experiment management examples displayed.",
            'examples': examples
        }
    
    def show_analysis_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show analysis examples."""
        examples = {
            'title': 'Analysis Examples',
            'examples': [
                {
                    'title': 'Data Analysis',
                    'description': 'Analyzing simulation data',
                    'commands': [
                        '# Analyze simulation data
nlm-cli analysis analyze_data --simulation_name my_sim',
                        
                        '# Generate performance report
nlm-cli analysis generate_report --simulation_name my_sim --format json',
                        
                        '# Visualize results
nlm-cli analysis visualize --simulation_name my_sim --plot_type comprehensive',
                        
                        '# Find patterns and anomalies
nlm-cli analysis find_patterns --simulation_name my_sim --pattern_type all'
                    ]
                },
                {
                    'title': 'Pattern Detection',
                    'description': 'Detecting patterns and anomalies',
                    'commands': [
                        '# Detect temporal patterns
nlm-cli analysis find_patterns --simulation_name my_sim --pattern_type temporal',
                        
                        '# Detect neural patterns
nlm-cli analysis find_patterns --simulation_name my_sim --pattern_type neural',
                        
                        '# Detect activity patterns
nlm-cli analysis find_patterns --simulation_name my_sim --pattern_type activity',
                        
                        '# Compare analysis results
nlm-cli analysis compare_results --simulation1 sim1 --simulation2 sim2'
                    ]
                },
                {
                    'title': 'System Monitoring',
                    'description': 'Monitoring system resources and performance',
                    'commands': [
                        '# Get system statistics
nlm-cli system status',
                        
                        '# Monitor system resources
nlm-cli system resources',
                        
                        '# Check system health
nlm-cli system check_system_health',
                        
                        '# Get system alerts
nlm-cli system alerts',
                        
                        '# Monitor system logs
nlm-cli system logs --lines 100'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Analysis examples displayed.",
            'examples': examples
        }
    
    def show_plugin_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show plugin management examples."""
        examples = {
            'title': 'Plugin Management Examples',
            'examples': [
                {
                    'title': 'Plugin Discovery',
                    'description': 'Finding and listing available plugins',
                    'commands': [
                        '# List all plugins
nlm-cli plugin list',
                        
                        '# List only local plugins
nlm-cli plugin list --source local',
                        
                        '# Search for plugins
nlm-cli plugin search --query visualization',
                        
                        '# Get plugin information
nlm-cli plugin info --name visualization_plugin'
                    ]
                },
                {
                    'title': 'Plugin Installation',
                    'description': 'Installing and configuring plugins',
                    'commands': [
                        '# Install plugin from PyPI
nlm-cli plugin install --name visualization --source pypi',
                        
                        '# Install plugin from local source
nlm-cli plugin install --name custom_plugin --source local --source_path ./plugin',
                        
                        '# Install plugin from Git
nlm-cli plugin install --name git_plugin --source git --git_url https://github.com/example/plugin',
                        
                        '# Install specific version
nlm-cli plugin install --name visualization --version 2.0.0'
                    ]
                },
                {
                    'title': 'Plugin Management',
                    'description': 'Enabling, disabling, and configuring plugins',
                    'commands': [
                        '# Enable plugin
nlm-cli plugin enable --name visualization_plugin',
                        
                        '# Disable plugin
nlm-cli plugin disable --name visualization_plugin',
                        
                        '# Configure plugin
nlm-cli plugin configure --name visualization_plugin',
                        
                        '# Check plugin status
nlm-cli plugin check --name visualization_plugin',
                        
                        '# Update plugin
nlm-cli plugin update --name visualization_plugin',
                        
                        '# Uninstall plugin
nlm-cli plugin uninstall --name visualization_plugin --force'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Plugin management examples displayed.",
            'examples': examples
        }
    
    def show_system_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show system management examples."""
        examples = {
            'title': 'System Management Examples',
            'examples': [
                {
                    'title': 'System Monitoring',
                    'description': 'Monitoring system resources and status',
                    'commands': [
                        '# Get overall system status
nlm-cli system status',
                        
                        '# Get system resources
nlm-cli system resources',
                        
                        '# Check system health
nlm-cli system check_system_health',
                        
                        '# Get system alerts
nlm-cli system alerts',
                        
                        '# Monitor system processes
nlm-cli system processes --top_n 10',
                        
                        '# Get CPU information
nlm-cli system cpu_info',
                        
                        '# Get memory information
nlm-cli system memory_info',
                        
                        '# Get disk information
nlm-cli system disk_info',
                        
                        '# Get network information
nlm-cli system network_info'
                    ]
                },
                {
                    'title': 'System Maintenance',
                    'description': 'System maintenance and configuration',
                    'commands': [
                        '# Check dependencies
nlm-cli system check_dependencies',
                        
                        '# Install dependencies
nlm-cli system install_dependencies --dependencies psutil matplotlib numpy',
                        
                        '# Setup monitoring
nlm-cli system setup_monitoring --duration 3600 --interval 60',
                        
                        '# Get system logs
nlm-cli system logs --log_file /var/log/nlm.log --lines 50',
                        
                        '# Manage plugins
nlm-cli system manage_plugins --action list',
                        
                        '# Update system plugins
nlm-cli system manage_plugins --action update'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "System management examples displayed.",
            'examples': examples
        }
    
    def show_advanced_examples(self, args: Dict[str, Any]) -> Dict[str, Any]:
        """Show advanced examples."""
        examples = {
            'title': 'Advanced NLM CLI Examples',
            'examples': [
                {
                    'title': 'Automated Experiment Runs',
                    'description': 'Running automated experiment sequences',
                    'commands': [
                        '# Run experiment with logging
nlm-cli experiment run --name auto_exp --steps 100 2>&1 | tee auto_exp.log',
                        
                        '# Run multiple experiments with different parameters
for steps in [50, 100, 200]:
  nlm-cli experiment run --name exp_$steps --steps $steps
',
                        
                        '# Create and run experiment in one command
nlm-cli experiment create --name quick_exp --description "Quick test" && nlm-cli experiment run --name quick_exp --steps 10'
                    ]
                },
                {
                    'title': 'Batch Processing',
                    'description': 'Processing multiple files and configurations',
                    'commands': [
                        '# Process multiple brain files
for brain_file in brains/*.json; do
  nlm-cli brain load --name $(basename $brain_file .json) --filepath $brain_file
  nlm-cli brain inspect --name $(basename $brain_file .json)
done
',
                        
                        '# Backup all configurations
nlm-cli config backup --name all_configs --output all_configs_backup.json'
                    ]
                },
                {
                    'title': 'Scripting and Automation',
                    'description': 'Creating scripts and automated workflows',
                    'commands': [
                        '# Create NLM workflow script
#!/bin/bash
# Create configuration
nlm-cli config create --name workflow_config --parameters "{\"neurons\": 500}"

# Create brain
nlm-cli brain create --name workflow_brain --config workflow_config

# Run experiment
nlm-cli experiment run --name workflow_exp --steps 100

# Analyze results
nlm-cli analysis generate_report --simulation_name workflow_exp
',
                        
                        '# Set up automated monitoring
while true; do
  nlm-cli system check_system_health
  sleep 300
done
'
                    ]
                }
            ]
        }
        
        self._display_examples(examples)
        
        return {
            'success': True,
            'message': "Advanced examples displayed.",
            'examples': examples
        }
    
    def _display_examples(self, examples: Dict[str, Any]):
        """Display examples in a formatted way."""
        console.print(f"\n{examples['title']}")
        console.print("=" * 60)
        
        if 'description' in examples:
            console.print(f"\nDescription: {examples['description']}")
        
        for example in examples['examples']:
            console.print(f"\n{'='*60}")
            console.print(f"Example: {example['title']}")
            console.print(f"Description: {example['description']}")
            
            if 'commands' in example:
                console.print("\nCommands:")
                for cmd in example['commands']:
                    syntax = Syntax(cmd, 'bash', theme='default')
                    console.print(syntax)