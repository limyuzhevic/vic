#!/usr/bin/env python3

"""
NLM Configuration Validation Script

Validates NLM configuration files for:
- Duplicate keys
- Parameter ranges
- Common issues
- Suggests fixes

Usage: python3 validate_config.py [config_file]
"""

import re
import sys
from typing import Dict, List, Tuple, Optional
from pathlib import Path

# Define config parameter ranges and descriptions
CONFIG_SPEC = {
    "random_seed": {
        "type": "int",
        "min": 0,
        "max": 2**31 - 1,
        "description": "Random seed for reproducible experiments"
    },
    "simulation_timestep": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "Simulation timestep in seconds"
    },
    "neuron_count": {
        "type": "int",
        "min": 1,
        "max": 1000000,
        "description": "Number of neurons in the brain"
    },
    "region_count": {
        "type": "int",
        "min": 1,
        "max": 100,
        "description": "Number of brain regions"
    },
    "connection_probability": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "Connection probability for random connectivity"
    },
    "plasticity_learning_rate": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "Plasticity learning rate"
    },
    "stdp_ltp_weight": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "STDP LTP weight"
    },
    "stdp_ltd_weight": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "STDP LTD weight"
    },
    "stdp_time_constant": {
        "type": "float",
        "min": 0.0,
        "max": 1000.0,
        "description": "STDP time constant"
    },
    "development_synaptogenesis_rate": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "Development synaptogenesis rate"
    },
    "development_pruning_rate": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "Development pruning rate"
    },
    "dopamine_baseline": {
        "type": "float",
        "min": 0.0,
        "max": 10.0,
        "description": "Dopamine baseline level"
    },
    "reward_discount_factor": {
        "type": "float",
        "min": 0.0,
        "max": 1.0,
        "description": "Reward discount factor"
    },
    "environment_width": {
        "type": "int",
        "min": 1,
        "max": 1000,
        "description": "Environment width"
    },
    "environment_height": {
        "type": "int",
        "min": 1,
        "max": 1000,
        "description": "Environment height"
    },
    "max_simulation_steps": {
        "type": "int",
        "min": 1,
        "max": 1000000,
        "description": "Maximum simulation steps"
    },
    "simulation_time_limit": {
        "type": "float",
        "min": 0.0,
        "max": 10000.0,
        "description": "Simulation time limit"
    },
    "visualization_update_rate": {
        "type": "float",
        "min": 1.0,
        "max": 1000.0,
        "description": "Visualization update rate"
    }
}

# Boolean values that can appear in config
BOOLEAN_VALUES = {"true", "false", "TRUE", "FALSE", "True", "False"}


def parse_config_file(filepath: Path) -> Tuple[Dict[str, str], List[str], List[str]]:
    """Parse config file and return (entries, duplicate_keys, comments)."""
    entries = {}
    duplicate_keys = []
    comments = []
    
    with open(filepath, 'r') as f:
        for line_num, line in enumerate(f, 1):
            line = line.strip()
            
            # Skip empty lines
            if not line:
                continue
                
            # Preserve comments for later
            if line.startswith('#'):
                comments.append(f"Line {line_num}: {line}")
                continue
                
            # Parse key=value pairs
            if '=' in line:
                key, value = line.split('=', 1)
                key = key.strip()
                value = value.strip()
                
                if key in entries:
                    duplicate_keys.append(f"Line {line_num}: Duplicate key '{key}' found")
                else:
                    entries[key] = value
    
    return entries, duplicate_keys, comments


def validate_config_entries(entries: Dict[str, str]) -> Tuple[List[str], List[str], List[str]]:
    """Validate config entries against spec. Returns (issues, warnings, suggestions)."""
    issues = []
    warnings = []
    suggestions = []
    
    # Check for missing essential keys
    essential_keys = ["neuron_count", "region_count", "connection_probability", 
                     "learning_rate", "dt", "max_steps"]
    for key in essential_keys:
        if key not in entries:
            warnings.append(f"Missing recommended key: '{key}'")
    
    # Validate each entry
    for key, value in entries.items():
        if key not in CONFIG_SPEC:
            warnings.append(f"Unknown configuration key: '{key}'")
            continue
        
        spec = CONFIG_SPEC[key]
        
        # Validate type and range
        try:
            if spec["type"] == "int":
                int_value = int(value)
                if int_value < spec["min"] or int_value > spec["max"]:
                    issues.append(
                        f"Invalid value for '{key}': {value}. "
                        f"Expected {spec['type']} in range [{spec['min']}, {spec['max']}]"
                    )
            elif spec["type"] == "float":
                float_value = float(value)
                if float_value < spec["min"] or float_value > spec["max"]:
                    issues.append(
                        f"Invalid value for '{key}': {value}. "
                        f"Expected {spec['type']} in range [{spec['min']}, {spec['max']}]"
                    )
            elif spec["type"] == "bool":
                if value.lower() not in BOOLEAN_VALUES:
                    issues.append(
                        f"Invalid boolean value for '{key}': {value}. "
                        f"Expected true or false"
                    )
        except ValueError:
            issues.append(f"Invalid value type for '{key}': {value}. Expected {spec['type']}")
    
    # Check consistency
    if "neuron_count" in entries and "region_count" in entries:
        try:
            neuron_count = int(entries["neuron_count"])
            region_count = int(entries["region_count"])
            if neuron_count < region_count:
                suggestions.append(
                    f"neuron_count ({neuron_count}) should be greater than region_count ({region_count})"
                )
        except ValueError:
            pass  # Will be caught in type validation
    
    return issues, warnings, suggestions


def suggest_fixes(issues: List[str], warnings: List[str]) -> List[str]:
    """Suggest fixes for common issues."""
    fixes = []
    
    for issue in issues:
        if "Invalid value" in issue and "range" in issue:
            # Extract key and suggest range
            import re
            match = re.search(r"\'([^\']+)\': ([^\.]+", issue)
            if match:
                key = match.group(1)
                spec = CONFIG_SPEC.get(key, {})
                if spec:
                    fixes.append(
                        f"For '{key}': Try value {spec['min']} (minimum valid)"
                    )
        
        if "Duplicate key" in issue:
            fixes.append(
                "For duplicates: Check config file for repeated keys. "
                "Later occurrences typically override earlier ones."
            )
    
    return fixes


def display_validation_report(
    filepath: Path,
    entries: Dict[str, str],
    duplicate_keys: List[str],
    comments: List[str],
    issues: List[str],
    warnings: List[str],
    suggestions: List[str],
    fixes: List[str]
):
    """Display validation report to user."""
    print("=" * 60)
    print(f"NLM Configuration Validation Report")
    print(f"File: {filepath}")
    print("=" * 60)
    
    if duplicate_keys:
        print("\n❌ DUPLICATE KEYS FOUND:")
        for dup in duplicate_keys:
            print(f"  - {dup}")
    
    if warnings:
        print("\n⚠️  WARNINGS:")
        for warning in warnings:
            print(f"  - {warning}")
    
    if issues:
        print("\n❌ VALIDATION ISSUES:")
        for issue in issues:
            print(f"  - {issue}")
    
    if suggestions:
        print("\n💡 SUGGESTIONS:")
        for suggestion in suggestions:
            print(f"  - {suggestion}")
    
    if fixes:
        print("\n🔧 FIX SUGGESTIONS:")
        for fix in fixes:
            print(f"  - {fix}")
    
    # Show current configuration
    print("\n📋 CURRENT CONFIGURATION:")
    print(f"  Total entries: {len(entries)}")
    for key, value in sorted(entries.items()):
        spec = CONFIG_SPEC.get(key, {"description": "Unknown"})
        print(f"  - {key} = {value}  # {spec['description']}")
    
    # Summary
    total_problems = len(duplicate_keys) + len(issues) + len(warnings)
    if total_problems == 0:
        print("\n✅ Configuration is valid!")
    else:
        print(f"\n⚠️  Configuration has {total_problems} problems")
        print(f"    - {len(duplicate_keys)} duplicates")
        print(f"    - {len(issues)} validation errors")
        print(f"    - {len(warnings)} warnings")


def main():
    """Main function."""
    if len(sys.argv) < 2:
        print("Usage: python3 validate_config.py [config_file]")
        print("       config_file: Path to the configuration file to validate")
        sys.exit(1)
    
    filepath = Path(sys.argv[1])
    
    if not filepath.exists():
        print(f"Error: Config file not found: {filepath}")
        sys.exit(1)
    
    # Parse and validate config
    entries, duplicate_keys, comments = parse_config_file(filepath)
    issues, warnings, suggestions = validate_config_entries(entries)
    fixes = suggest_fixes(issues, warnings)
    
    # Display report
    display_validation_report(
        filepath, entries, duplicate_keys, comments,
        issues, warnings, suggestions, fixes
    )
    
    # Exit with non-zero code if there are issues
    if issues:
        sys.exit(1)


if __name__ == "__main__":
    main()
