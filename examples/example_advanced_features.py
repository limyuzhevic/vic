#!/usr/bin/env python3
"""
Example 3: Config merging and environment variables
"""

import subprocess
import tempfile
import os

def example_advanced_features():
    """Demonstrate advanced config features"""
    print("=== Example 3: Advanced Config Features ===")
    
    # Set environment variables
    print("\n1. Setting environment variables:")
    os.environ['NLM_NEURON_COUNT'] = '500'
    os.environ['NLM_CONNECTION_PROBABILITY'] = '0.2'
    
    # Load from environment
    print("\n2. Loading from environment:")
    result = subprocess.run(['python3', 'scripts/nlm-config', 'env-load'],
                           capture_output=True, text=True)
    print(result.stdout)
    
    # Create two config files
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f1:
        f1.write("""
neuron_count: 100
region_count: 2
""")
        config1_file = f1.name
    
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f2:
        f2.write("""
neuron_count: 200
connection_probability: 0.15
""")
        config2_file = f2.name
    
    try:
        # Merge configs
        print("\n3. Merging two configs:")
        result = subprocess.run(['python3', 'scripts/nlm-config', 'merge', config1_file, config2_file],
                               capture_output=True, text=True)
        print(result.stdout)
        
        # Export to environment
        print("\n4. Exporting to environment variables:")
        result = subprocess.run(['python3', 'scripts/nlm-config', 'env-export', '--file', config1_file, '--prefix', 'MY_APP_'],
                               capture_output=True, text=True)
        print(result.stdout)
        
    finally:
        os.unlink(config1_file)
        os.unlink(config2_file)

if __name__ == "__main__":
    example_advanced_features()