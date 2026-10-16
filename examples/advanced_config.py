"""
Advanced Configuration Management Example
========================================

This example demonstrates advanced NLM configuration management techniques.
It shows how to create complex configuration systems, parameter sweeps,
hyperparameter optimization setups, and configuration validation for research.

## What This Example Shows

1. Complex configuration hierarchies
2. Parameter sweeps for hyperparameter optimization
3. Configuration validation and testing
4. Batch configuration processing
5. Configuration export/import for reproducibility
6. Advanced configuration management workflows

## When to Use This Example

- You're running systematic parameter sweeps
- You need to optimize hyperparameters
- You're setting up research experiments with multiple configurations
- You need to share configurations with collaborators
- You're implementing automated configuration testing
- You're managing complex experimental setups
"""

"""
Advanced Configuration Management Example
========================================

This example demonstrates advanced NLM configuration management techniques.
It shows how to create complex configuration systems, parameter sweeps,
hyperparameter optimization setups, and configuration validation for research.

## What This Example Shows

1. Complex configuration hierarchies
2. Parameter sweeps for hyperparameter optimization
3. Configuration validation and testing
4. Batch configuration processing
5. Configuration export/import for reproducibility
6. Advanced configuration management workflows

## When to Use This Example

- You're running systematic parameter sweeps
- You need to optimize hyperparameters
- You're setting up research experiments with multiple configurations
- You need to share configurations with collaborators
- You're implementing automated configuration testing
- You're managing complex experimental setups
"""

import pynlm
from advanced_api import NLMConfigurator
import json
import os
import itertools
from pathlib import Path
from typing import List, Dict, Any

import time
import statistics

def create_parameter_sweep(base_preset: str, parameter_ranges: Dict[str, List[Any]]) -> List[Dict[str, Any]]:
    """
    Create a parameter sweep for hyperparameter optimization.
    
    Args:
        base_preset: Base configuration preset
        parameter_ranges: Dictionary of parameter names and their possible values
    
    Returns:
        List of configuration dictionaries for each parameter combination
    """
    print(f"Creating parameter sweep from {base_preset} preset...")
    print(f"Parameters: {list(parameter_ranges.keys())}")
    
    # Generate all combinations
    param_combinations = list(itertools.product(*parameter_ranges.values()))
    
    configurations = []
    
    for i, combination in enumerate(param_combinations):
        # Create base configuration
        config_dict = NLMConfigurator.create_config(base_preset).getKeys()
        
        # Apply parameter combinations
        for j, (param, values) in enumerate(parameter_ranges.items()):
            config_dict.set(param, combination[j])
        
        configurations.append(config_dict)
        
        if (i + 1) % 10 == 0:
            print(f"  Created {i + 1}/{len(param_combinations)} configurations...")
    
    print(f"Parameter sweep complete: {len(configurations)} configurations")
    return configurations

def create_configuration_grid(grid_parameters: Dict[str, List[Any]]) -> List[Dict[str, Any]]:
    """
    Create a configuration grid for systematic testing.
    
    Args:
        grid_parameters: Dictionary of parameter names and grid values
    
    Returns:
        List of configuration dictionaries for grid testing
    """
    print("Creating configuration grid...")
    print(f"Grid parameters: {list(grid_parameters.keys())}")
    
    # Calculate grid size
    grid_size = 1
    for values in grid_parameters.values():
        grid_size *= len(values)
    
    print(f"Grid size: {grid_size} configurations")
    
    configurations = []
    
    # Create all combinations
    param_names = list(grid_parameters.keys())
    param_values = list(grid_parameters.values())
    
    for combination in itertools.product(*param_values):
        config_dict = NLMConfigurator.create_config('research')
        
        for name, value in zip(param_names, combination):
            config_dict.set(name, value)
        
        configurations.append(config_dict)
    
    print(f"Configuration grid complete: {len(configurations)} configurations")
    return configurations

def validate_configuration_set(configurations: List[Dict[str, Any]]) -> Dict[str, Any]:
    """
    Validate a set of configurations and report issues.
    
    Args:
        configurations: List of configuration dictionaries
    
    Returns:
        Dictionary with validation statistics
    """
    print("Validating configuration set...")
    print(f"Validating {len(configurations)} configurations...")
    
    validation_stats = {
        'total': len(configurations),
        'passed': 0,
        'warnings': 0,
        'errors': 0,
        'issue_types': {}
    }
    
    for i, config in enumerate(configurations):
        # Basic validation
        issues = []
        
        # Check for required parameters
        if not config.has('brain.neuron_count'):
            issues.append("Missing required parameter: brain.neuron_count")
        
        # Check for reasonable ranges
        if config.has('brain.neuron_count'):
            neuron_count = config.get('brain.neuron_count')
            if neuron_count < 10:
                issues.append(f"Unreasonable neuron count: {neuron_count}")
            elif neuron_count > 10000:
                issues.append(f"High memory requirement: {neuron_count} neurons")
        
        # Check plasticity settings
        if config.has('plasticity.stdp.enable') and not config.get('plasticity.stdp.enable'):
            if config.has('plasticity.hebbian.enable') and not config.get('plasticity.hebbian.enable'):
                issues.append("No plasticity mechanisms enabled - simulation may be trivial")
        
        # Count issues
        if issues:
            validation_stats['errors'] += 1
            for issue in issues:
                issue_type = issue.split(':')[0]
                validation_stats['issue_types'][issue_type] = validation_stats['issue_types'].get(issue_type, 0) + 1
        else:
            validation_stats['passed'] += 1
        
        if (i + 1) % 50 == 0:
            print(f"  Validated {i + 1}/{len(configurations)} configurations...")
    
    validation_stats['warnings'] = validation_stats['errors'] * 0.2  # Estimate
    
    print(f"Validation complete:")
    print(f"  Total configurations: {validation_stats['total']}")
    print(f"  Valid: {validation_stats['passed']} ({validation_stats['passed']/validation_stats['total']*100:.1f}%)")
    print(f"  Issues found: {validation_stats['errors']}")
    
    return validation_stats

def export_configurations(configurations: List[Dict[str, Any]], output_path: str):
    """
    Export configurations to JSON file for sharing.
    
    Args:
        configurations: List of configuration dictionaries
        output_path: Path to output JSON file
    """
    print(f"Exporting {len(configurations)} configurations to {output_path}...")
    
    # Convert to serializable format
    export_data = {
        'configurations': [],
        'metadata': {
            'exported_at': time.strftime('%Y-%m-%d %H:%M:%S'),
            'total_configurations': len(configurations),
            'source_preset': 'research'
        }
    }
    
    for config in configurations:
        config_dict = {}
        for key, value in config.getKeys():
            config_dict[key] = value
        
        export_data['configurations'].append(config_dict)
    
    # Write to file
    with open(output_path, 'w') as f:
        json.dump(export_data, f, indent=2)
    
    print(f"Exported to {output_path}")
    print(f"File size: {os.path.getsize(output_path) / 1024:.1f} KB")

def import_configurations(file_path: str) -> List[Dict[str, Any]]:
    """
    Import configurations from JSON file.
    
    Args:
        file_path: Path to input JSON file
    
    Returns:
        List of configuration dictionaries
    """
    print(f"Importing configurations from {file_path}...")
    
    with open(file_path, 'r') as f:
        import_data = json.load(f)
    
    configurations = []
    for config_dict in import_data['configurations']:
        config = NLMConfigurator.create_config('research')
        
        for key, value in config_dict.items():
            config.set(key, value)
        
        configurations.append(config)
    
    print(f"Imported {len(configurations)} configurations")
    return configurations

def main():
    """Run the advanced configuration management example"""
    print("=== Advanced Configuration Management Example ===")
    print()
    
    # 1. Create parameter sweep
    print("Step 1: Creating parameter sweep...")
    
    parameter_ranges = {
        'brain.neuron_count': [100, 500, 1000],
        'brain.synapse_density': [0.05, 0.1, 0.15],
        'plasticity.stdp.learning_rate': [0.001, 0.01, 0.1],
        'neuromod.curiosity.enable': [True, False]
    }
    
    sweep_configs = create_parameter_sweep('research', parameter_ranges)
    print(f"Created {len(sweep_configs)} configurations for parameter sweep")
    print()
    
    # 2. Create configuration grid
    print("Step 2: Creating configuration grid...")
    
    grid_parameters = {
        'brain.neuron_count': [50, 200, 500],
        'plasticity.stdp.enable': [True],
        'neuromod.curiosity.enable': [True, False]
    }
    
    grid_configs = create_configuration_grid(grid_parameters)
    print(f"Created {len(grid_configs)} configurations for grid testing")
    print()
    
    # 3. Validate configurations
    print("Step 3: Validating configuration sets...")
    
    # Validate sweep configurations (sample)
    print("Validating parameter sweep (sample of 100)...")
    sweep_sample = sweep_configs[:100]
    sweep_stats = validate_configuration_set(sweep_sample)
    
    print()
    print("Validating full configuration grid...")
    grid_stats = validate_configuration_set(grid_configs)
    
    print()
    print("Validation Summary:")
    print(f"  Parameter sweep: {sweep_stats['passed']}/{sweep_stats['total']} valid ({sweep_stats['passed']/sweep_stats['total']*100:.1f}%)")
    print(f"  Configuration grid: {grid_stats['passed']}/{grid_stats['total']} valid ({grid_stats['passed']/grid_stats['total']*100:.1f}%)")
    print(f"  Total issues found: {sweep_stats['errors'] + grid_stats['errors']}")
    
    # 4. Export configurations
    print()
    print("Step 4: Exporting configurations...")
    
    # Export sweep configurations
    sweep_export_path = Path("exports/sweep_configurations.json")
    sweep_export_path.parent.mkdir(exist_ok=True)
    export_configurations(sweep_configs[:50], str(sweep_export_path))
    
    # Export grid configurations  
    grid_export_path = Path("exports/grid_configurations.json")
    export_configurations(grid_configs[:50], str(grid_export_path))
    
    # 5. Import and test configurations
    print()
    print("Step 5: Testing imported configurations...")
    
    # Import a sample
    imported_configs = import_configurations(str(sweep_export_path))
    print(f"Imported and processed {len(imported_configs)} configurations")
    
    # Test imported configurations
    valid_imported = 0
    for config in imported_configs[:10]:  # Test first 10
        brain = pynlm.createBrain(config)
        if brain.initialize():
            valid_imported += 1
    
    print(f"Successfully initialized {valid_imported}/10 imported configurations")
    
    # 6. Configuration analysis
    print()
    print("Step 6: Configuration analysis...")
    
    # Analyze parameter sweep
    neuron_counts = [100, 500, 1000]
    curiosity_settings = [True, False]
    
    print("Parameter sweep analysis:")
    for neurons in neuron_counts:
        for curiosity in curiosity_settings:
            matching_configs = [c for c in sweep_configs 
                              if c.get('brain.neuron_count') == neurons 
                              and c.get('neuromod.curiosity.enable') == curiosity]
            if matching_configs:
                avg_firing = statistics.mean([bc.getAverageFiringRate() for bc in 
                                            [pynlm.createBrain(c) for c in matching_configs[:5]]])
                print(f"  {neurons} neurons, curiosity={curiosity}: ~{avg_firing:.3f} firing rate")
    
    print()
    print("Configuration management best practices:")
    print("  1. Use parameter sweeps for hyperparameter optimization")
    print("  2. Create configuration grids for systematic testing")
    print("  3. Always validate configurations before use")
    print("  4. Export configurations for reproducibility")
    print("  5. Use JSON for configuration sharing")
    print("  6. Keep configuration naming consistent")
    
    print()
    print("=== Advanced Configuration Management Complete ===")
    print()
    print("For production use:")
    print("  1. Start with parameter sweeps for optimization")
    print("  2. Validate all configurations before deployment")
    print("  3. Export successful configurations for future use")
    print("  4. Document parameter ranges and rationale")
    print("  5. Set up automated configuration testing")

if __name__ == "__main__":
    import time
    import statistics
    main()
