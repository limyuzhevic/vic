#!/usr/bin/env python3
"""
NLM Configuration Utility

This script provides command-line tools for working with NLM configuration files,
including validation, conversion, and comparison operations.

Features:
- Validate configuration files (JSON/YAML format)
- Convert between configuration formats
- Merge configurations
- Compare configurations
- Profile configuration validation performance
"""

import argparse
import json
import sys
import time
import os
from pathlib import Path
from typing import Dict, Any, Optional

def validate_config(file_path: str) -> bool:
    """Validate a configuration file"""
    try:
        with open(file_path, 'r') as f:
            content = f.read()
        
        # Try to parse as JSON
        try:
            config_data = json.loads(content)
            print(f"✓ Valid JSON configuration file: {file_path}")
            print(f"  Contains {len(config_data)} top-level keys")
            return True
        except json.JSONDecodeError:
            pass
        
        # Try to parse as simple key=value format
        with open(file_path, 'r') as f:
            lines = f.readlines()
            valid_lines = 0
            for line in lines:
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    valid_lines += 1
            
            if valid_lines > 0:
                print(f"✓ Valid configuration file (key=value format): {file_path}")
                print(f"  Contains {valid_lines} configuration entries")
                return True
            else:
                print(f"✗ Invalid configuration file: {file_path}")
                return False
                
    except Exception as e:
        print(f"✗ Error reading configuration file {file_path}: {e}")
        return False

def convert_config(input_file: str, output_file: str, target_format: str) -> bool:
    """Convert configuration file to a different format"""
    try:
        # Parse the input configuration
        with open(input_file, 'r') as f:
            content = f.read()
        
        config_data = {}
        
        # Try to parse as JSON first
        try:
            config_data = json.loads(content)
            print(f"Loaded JSON configuration from: {input_file}")
        except json.JSONDecodeError:
            # Parse as key=value format
            for line in content.splitlines():
                line = line.strip()
                if line and not line.startswith('#') and '=' in line:
                    key, value = line.split('=', 1)
                    key = key.strip()
                    value = value.strip().strip('"\'')
                    
                    # Try to convert value to appropriate type
                    if value.lower() == 'true':
                        config_data[key] = True
                    elif value.lower() == 'false':
                        config_data[key] = False
                    elif value.replace('.', '', 1).isdigit():
                        if '.' in value:
                            config_data[key] = float(value)
                        else:
                            config_data[key] = int(value)
                    else:
                        config_data[key] = value
            
            print(f"Loaded key=value configuration from: {input_file}")
        
        # Write to target format
        with open(output_file, 'w') as f:
            if target_format.lower() == 'json':
                json.dump(config_data, f, indent=2)
                print(f"✓ Converted configuration to JSON: {output_file}")
            elif target_format.lower() == 'yaml':
                try:
                    import yaml
                    yaml.dump(config_data, f, default_flow_style=False)
                    print(f"✓ Converted configuration to YAML: {output_file}")
                except ImportError:
                    print("✗ YAML conversion requires PyYAML package. Install with: pip install PyYAML")
                    return False
            elif target_format.lower() == 'env':
                for key, value in config_data.items():
                    if isinstance(value, bool):
                        f.write(f"{key}={str(value).lower()}\n")
                    elif isinstance(value, (int, float)):
                        f.write(f"{key}={value}\n")
                    else:
                        f.write(f'{key}="{value}"\n')
                print(f"✓ Converted configuration to environment file: {output_file}")
            else:
                print(f"✗ Unsupported target format: {target_format}")
                return False
        
        return True
        
    except Exception as e:
        print(f"✗ Error during conversion: {e}")
        return False

def merge_configs(config_files: list, output_file: str) -> bool:
    """Merge multiple configuration files"""
    try:
        merged_config = {}
        
        for file_path in config_files:
            with open(file_path, 'r') as f:
                content = f.read()
            
            # Try to parse as JSON
            try:
                file_config = json.loads(content)
                merged_config.update(file_config)
                print(f"✓ Merged configuration from: {file_path}")
            except json.JSONDecodeError:
                # Parse as key=value format
                for line in content.splitlines():
                    line = line.strip()
                    if line and not line.startswith('#') and '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip().strip('"\'')
                        merged_config[key] = value
                
                print(f"✓ Merged configuration from: {file_path} (key=value format)")
        
        # Write merged configuration
        with open(output_file, 'w') as f:
            json.dump(merged_config, f, indent=2)
            print(f"✓ Merged configuration saved to: {output_file}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error during merge: {e}")
        return False

def compare_configs(config_files: list) -> bool:
    """Compare multiple configuration files"""
    try:
        configs = []
        
        for file_path in config_files:
            with open(file_path, 'r') as f:
                content = f.read()
            
            # Parse configuration
            config_data = {}
            try:
                config_data = json.loads(content)
            except json.JSONDecodeError:
                # Parse as key=value format
                for line in content.splitlines():
                    line = line.strip()
                    if line and not line.startswith('#') and '=' in line:
                        key, value = line.split('=', 1)
                        key = key.strip()
                        value = value.strip().strip('"\'')
                        config_data[key] = value
            
            configs.append((file_path, config_data))
        
        if len(configs) < 2:
            print("✗ Need at least 2 configuration files to compare")
            return False
        
        print("Configuration Comparison:")
        print("=" * 50)
        
        # Find all unique keys
        all_keys = set()
        for _, config in configs:
            all_keys.update(config.keys())
        
        for key in sorted(all_keys):
            print(f"\nKey: {key}")
            values = []
            for file_path, config in configs:
                if key in config:
                    values.append(f"{file_path}: {config[key]}")
                else:
                    values.append(f"{file_path}: [NOT SET]")
            
            for value in values:
                print(f"  {value}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error during comparison: {e}")
        return False

def profile_validation(file_path: str, iterations: int = 1000) -> bool:
    """Profile configuration validation performance"""
    try:
        with open(file_path, 'r') as f:
            content = f.read()
        
        # Measure validation time
        start_time = time.time()
        
        for _ in range(iterations):
            try:
                json.loads(content)
            except json.JSONDecodeError:
                pass
        
        end_time = time.time()
        total_time = end_time - start_time
        avg_time = (total_time / iterations) * 1000  # Convert to milliseconds
        
        print(f"Configuration Validation Performance:")
        print(f"  File: {file_path}")
        print(f"  Iterations: {iterations}")
        print(f"  Total time: {total_time:.4f} seconds")
        print(f"  Average time per validation: {avg_time:.4f} ms")
        print(f"  Validations per second: {iterations / total_time:.0f}")
        
        return True
        
    except Exception as e:
        print(f"✗ Error during profiling: {e}")
        return False

def main():
    parser = argparse.ArgumentParser(
        description="NLM Configuration Utility",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  nlm_config_utility --validate configs/default.cfg
  nlm_config_utility --convert configs/default.cfg configs/default.json json
  nlm_config_utility --merge config1.json config2.json merged.json
  nlm_config_utility --compare config1.json config2.json
  nlm_config_utility --profile configs/default.cfg
        ""
    )
    
    # Main action group
    action_group = parser.add_mutually_exclusive_group(required=True)
    action_group.add_argument('--validate', action='store_true',
                             help='Validate configuration file')
    action_group.add_argument('--convert', action='store_true',
                             help='Convert configuration to different format')
    action_group.add_argument('--merge', action='store_true',
                             help='Merge multiple configuration files')
    action_group.add_argument('--compare', action='store_true',
                             help='Compare multiple configuration files')
    action_group.add_argument('--profile', action='store_true',
                             help='Profile configuration validation performance')
    
    # Common arguments
    parser.add_argument('input_files', nargs='*',
                       help='Input configuration files')
    parser.add_argument('--output', '-o',
                       help='Output file for conversion/merge operations')
    parser.add_argument('--format', '-f',
                       help='Target format for conversion (json, yaml, env)')
    parser.add_argument('--iterations', type=int, default=1000,
                       help='Number of iterations for profiling (default: 1000)')
    
    args = parser.parse_args()
    
    # Route to appropriate function
    if args.validate:
        if len(args.input_files) != 1:
            print("✗ --validate requires exactly one input file")
            sys.exit(1)
        return validate_config(args.input_files[0])
    
    elif args.convert:
        if len(args.input_files) != 1:
            print("✗ --convert requires exactly one input file")
            sys.exit(1)
        if not args.output:
            print("✗ --convert requires --output option")
            sys.exit(1)
        if not args.format:
            print("✗ --convert requires --format option")
            sys.exit(1)
        return convert_config(args.input_files[0], args.output, args.format)
    
    elif args.merge:
        if len(args.input_files) < 2:
            print("✗ --merge requires at least two input files")
            sys.exit(1)
        if not args.output:
            print("✗ --merge requires --output option")
            sys.exit(1)
        return merge_configs(args.input_files, args.output)
    
    elif args.compare:
        if len(args.input_files) < 2:
            print("✗ --compare requires at least two input files")
            sys.exit(1)
        return compare_configs(args.input_files)
    
    elif args.profile:
        if len(args.input_files) != 1:
            print("✗ --profile requires exactly one input file")
            sys.exit(1)
        return profile_validation(args.input_files[0], args.iterations)
    
    else:
        parser.print_help()
        sys.exit(1)

if __name__ == '__main__':
    success = main()
    sys.exit(0 if success else 1)
