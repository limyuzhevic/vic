#!/usr/bin/env python3
"""
Example 2: Config validation and schema checking
"""

import subprocess
import tempfile
import os

def example_validation():
    """Demonstrate config validation"""
    print("=== Example 2: Config Validation ===")
    
    # Create a config with validation schema
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write("""
neuron_count: 100
region_count: 2
simulation_timestep: 0.01
""")
        config_file = f.name
    
    try:
        # Validate config
        print("\n1. Validating config:")
        result = subprocess.run(['python3', 'scripts/config_validation.py', 'validate', config_file],
                               capture_output=True, text=True)
        print(result.stdout)
        
        # Validate with detailed error reporting
        print("\n2. Detailed validation:")
        result = subprocess.run(['python3', 'scripts/config_validation.py', 'validate', '--verbose', config_file],
                               capture_output=True, text=True)
        print(result.stdout)
        
        # Compare with default config
        print("\n3. Comparing with default config:")
        result = subprocess.run(['python3', 'scripts/config_validation.py', 'compare', config_file, 'templates/default_config.yaml'],
                               capture_output=True, text=True)
        print(result.stdout)
        
    finally:
        os.unlink(config_file)

if __name__ == "__main__":
    example_validation()