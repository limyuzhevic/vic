#!/usr/bin/env python3
"""
Example usage of the NLM CLI for batch processing and automation.

This script demonstrates various commands available in the NLM CLI.
"""

import sys
import os
import json
import time
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

def print_header(title):
    """Print a formatted header."""
    print(f"\n{'=' * 60}")
    print(f"{title}")
    print(f"{'=' * 60}")

def main():
    """Demonstrate NLM CLI capabilities."""
    print_header("NLM CLI - Batch Processing and Automation Demo")
    
    print("\nThis demo shows various NLM CLI commands:")
    print("1. Batch processing with parameter sweeps")
    print("2. Configuration management")
    print("3. Workflow definition and execution")
    print("4. Result analysis and reporting")
    print("5. Experiment management")
    
    # Example 1: Show how to run a parameter sweep
    print_header("Example 1: Parameter Sweep Configuration")
    
    sweep_config = {
        "name": "firing_rate_sweep",
        "parameter_ranges": {
            "target_firing_rate": [1.0, 2.0, 5.0, 10.0, 20.0],
            "network_size": [5000, 10000, 20000],
            "learning_rate": [0.001, 0.01, 0.1]
        },
        "fixed_parameters": {
            "time_step_size": 0.01,
            "total_steps": 1000
        },
        "repetitions": 3,
        "randomize_order": True,
        "save_intermediate": True
    }
    
    print(f"Experiment: {sweep_config['name']}")
    print(f"Parameter ranges: {json.dumps(sweep_config['parameter_ranges'], indent=2)}")
    print(f"Fixed parameters: {json.dumps(sweep_config['fixed_parameters'], indent=2)}")
    print(f"Repetitions: {sweep_config['repetitions']}")
    
    # Example 2: Configuration template
    print_header("Example 2: Configuration Template")
    
    template_config = {
        "name": "neural_network_template",
        "description": "Standard neural network configuration for NLM simulations",
        "parameters": {
            "network_type": "LIF",
            "neuron_count": 10000,
            "learning_rate": 0.01,
            "batch_size": 32,
            "time_step_size": 0.01,
            "max_steps": 1000,
            "use_gpu": True,
            "visualization_enabled": False
        },
        "tags": ["standard", "LIF", "neural_network"],
        "author": "NLM Team",
        "version": "1.0"
    }
    
    print(f"Template: {template_config['name']}")
    print(f"Description: {template_config['description']}")
    print(f"Tags: {', '.join(template_config['tags'])}")
    print(f"Parameters: {json.dumps(template_config['parameters'], indent=2)}")
    
    # Example 3: Workflow definition
    print_header("Example 3: Workflow Definition")
    
    workflow_config = {
        "name": "complete_simulation_workflow",
        "tasks": [
            {
                "name": "config_validation",
                "type": "validation",
                "depends_on": []
            },
            {
                "name": "parameter_sweep",
                "type": "batch",
                "depends_on": ["config_validation"],
                "parameters": {
                    "sweep_name": "firing_rate_sweep",
                    "repetitions": 5
                }
            },
            {
                "name": "result_analysis",
                "type": "analysis",
                "depends_on": ["parameter_sweep"],
                "analysis_type": "statistical"
            },
            {
                "name": "report_generation",
                "type": "reporting",
                "depends_on": ["result_analysis"],
                "report_format": "json"
            }
        ],
        "dependencies": {
            "parameter_sweep": ["config_validation"],
            "result_analysis": ["parameter_sweep"],
            "report_generation": ["result_analysis"]
        }
    }
    
    print(f"Workflow: {workflow_config['name']}")
    print(f"Number of tasks: {len(workflow_config['tasks'])}")
    for task in workflow_config['tasks']:
        print(f"  - {task['name']} ({task['type']}) depends on: {task['depends_on']}")
    
    # Example 4: Expected results analysis
    print_header("Example 4: Expected Results Analysis")
    
    expected_results = {
        "sweep_name": "firing_rate_sweep",
        "total_simulations": 45,  # 5 rates × 3 sizes × 3 learning rates × 3 repetitions
        "expected_metrics": [
            "final_firing_rate",
            "final_e_i_ratio",
            "total_spikes",
            "simulation_duration"
        ],
        "analysis_plan": {
            "statistical_tests": ["ANOVA", "t-test", "correlation analysis"],
            "visualization_types": ["scatter_plot", "heatmap", "response_surface"],
            "output_formats": ["JSON", "CSV", "PDF"],
            "reporting_frequency": "after_each_sweep"
        }
    }
    
    print(f"Sweep analysis for: {expected_results['sweep_name']}")
    print(f"Expected total simulations: {expected_results['total_simulations']}")
    print(f"Metrics to analyze: {', '.join(expected_results['expected_metrics'])}")
    print("\nStatistical tests planned:")
    for test in expected_results['analysis_plan']['statistical_tests']:
        print(f"  - {test}")
    
    # Example 5: CLI command structure
    print_header("Example 5: CLI Command Structure")
    
    cli_examples = [
        {
            "command": "nlm batch run firing_rate_sweep --ranges \"target_firing_rate:[1.0,2.0,5.0,10.0,20.0]\" --repetitions 3",
            "description": "Run a parameter sweep experiment"
        },
        {
            "command": "nlm config create neural_template --parameters \"network_type:LIF,learning_rate:0.01\" --description \"Standard neural network config\"",
            "description": "Create a configuration template"
        },
        {
            "command": "nlm workflow define simulation_workflow --file workflow_config.json",
            "description": "Define a complex workflow from file"
        },
        {
            "command": "nlm analysis run results.json --output-format json",
            "description": "Analyze simulation results"
        },
        {
            "command": "nlm experiment run my_experiment --config experiment_config.json",
            "description": "Run a predefined experiment"
        }
    ]
    
    for example in cli_examples:
        print(f"Command: {example['command']}")
        print(f"Description: {example['description']}\n")
    
    # Example 6: Result expected format
    print_header("Example 6: Expected Result Format")
    
    expected_result_format = {
        "metadata": {
            "sweep_name": "firing_rate_sweep",
            "param_set_name": "firing_rate_sweep_param_0",
            "parameters": {
                "target_firing_rate": 5.0,
                "network_size": 10000,
                "learning_rate": 0.01
            },
            "start_time": "2026-01-01T10:00:00Z",
            "end_time": "2026-01-01T10:05:00Z",
            "duration": 300.0,
            "repetition": 0
        },
        "final_metrics": {
            "final_active_neurons": 8450,
            "final_firing_rate": 4.8,
            "final_e_i_ratio": 1.2,
            "total_spikes": 12345,
            "final_development_stage": 0.85,
            "final_neurons": 10000
        },
        "step_results": [
            {
                "step": 0,
                "time": "2026-01-01T10:00:00Z",
                "active_neurons": 10000,
                "firing_rate": 0.0,
                "e_i_ratio": 1.0,
                "spike_count": 0
            }
        ],
        "config": {
            "target_firing_rate": 5.0,
            "network_size": 10000,
            "time_step_size": 0.01,
            "total_steps": 1000
        }
    }
    
    print("Expected result JSON structure:")
    print(json.dumps(expected_result_format, indent=2))
    
    # Example 7: Analysis output format
    print_header("Example 7: Analysis Output Format")
    
    analysis_output = {
        "sweep_name": "firing_rate_sweep",
        "total_simulations": 45,
        "parameter_names": ["target_firing_rate", "network_size", "learning_rate"],
        "metric_analysis": {
            "final_firing_rate": {
                "mean": 5.2,
                "std": 2.1,
                "min": 1.1,
                "max": 8.3,
                "optimal_value": 7.5,
                "optimal_parameters": {
                    "target_firing_rate": 7.5,
                    "network_size": 15000,
                    "learning_rate": 0.01
                }
            }
        },
        "correlations": {
            "final_firing_rate": {
                "target_firing_rate": 0.85,
                "network_size": 0.3,
                "learning_rate": 0.1
            }
        },
        "response_surfaces": {
            "final_firing_rate": {
                "param1": "target_firing_rate",
                "param2": "network_size",
                "metric_values": [1.2, 3.4, 5.6, 7.8],
                "normalized_values": [0.0, 0.3, 0.6, 1.0]
            }
        }
    }
    
    print("Expected analysis JSON structure:")
    print(json.dumps(analysis_output, indent=2))
    
    # Summary
    print_header("Summary")
    print("\nThe NLM CLI provides comprehensive functionality for:")
    print("✓ Batch processing with parameter sweeps and grid search")
    print("✓ Configuration management with templates and validation")
    print("✓ Workflow definition with dependency management")
    print("✓ Result analysis with statistical tests")
    print("✓ Experiment tracking and monitoring")
    print("✓ Progress tracking and reporting")
    print("✓ Checkpointing and resumption capabilities")
    print("✓ Result storage and organization")
    print("✓ Data export and archiving")
    print("✓ Visualization generation")
    
    print("\nThe CLI integrates seamlessly with all enhanced NLM components:")
    print("• Enhanced Python bindings with type safety")
    print("• Advanced agent capabilities")
    print("• Visualization and debugging tools")
    print("• Configuration management system")
    print("• Performance monitoring tools")
    
    print("\nUsage examples:")
    print("  python nlm_cli.py batch run my_experiment --ranges \"param1:[1,2,3],param2:[4,5,6]\"")
    print("  python nlm_cli.py config create my_template --parameters \"key:value,other:setting\"")
    print("  python nlm_cli.py workflow define my_workflow")
    print("  python nlm_cli.py analysis run results.json")

if __name__ == "__main__":
    main()
