#!/usr/bin/env python3
"""
Example 1: Basic Config CLI usage
"""

import subprocess
import tempfile
import os

def example_basic_usage():
    """Demonstrate basic config operations"""
    print("=== Example 1: Basic Config Usage ===")
    
    # Create a temporary config file
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        f.write("""
neuron_count: 1000
region_count: 3
connection_probability: 0.1
""")
        config_file = f.name
    
    try:
        # List all config keys
        print("\n1. Listing config keys:")
        result = subprocess.run(['python3', 'scripts/nlm-config', 'list', '--file', config_file], 
                               capture_output=True, text=True)
        print(result.stdout)
        
        # Get a specific value
        print("\n2. Getting config value:")
        result = subprocess.run(['python3', 'scripts/nlm-config', 'get', '--file', config_file, 'neuron_count'],
                               capture_output=True, text=True)
        print(result.stdout)
        
        # Set a value
        print("\n3. Setting config value:")
        result = subprocess.run(['python3', 'scripts/nlm-config', 'set', '--file', config_file, 'neuron_count', '2000'],
                               capture_output=True, text=True)
        print(result.stdout)
        
        # Verify the value was set
        result = subprocess.run(['python3', 'scripts/nlm-config', 'get', '--file', config_file, 'neuron_count'],
                               capture_output=True, text=True)
        print("\n4. Verifying set value:")
        print(result.stdout)
        
        # Save to different format
        print("\n5. Saving to JSON:")
        json_file = config_file.replace('.yaml', '.json')
        result = subprocess.run(['python3', 'scripts/nlm-config', 'save', '--file', config_file, '--output', json_file, '--format', 'json'],
                               capture_output=True, text=True)
        print(result.stdout)
        
    finally:
        # Clean up
        os.unlink(config_file)
        if os.path.exists(json_file):
            os.unlink(json_file)

if __name__ == "__main__":
    example_basic_usage()