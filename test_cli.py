#!/usr/bin/env python3
# Simple Python CLI tester

import subprocess
import sys

def test_cli():
    """Test the NLM CLI functionality"""
    print("Testing NLM CLI...")
    
    # Test help command
    try:
        result = subprocess.run(['bash', '-c', 'cd /workspace/app && ./nlm_cli.sh help'], 
                              capture_output=True, text=True, timeout=10)
        if result.returncode == 0:
            print("✓ Help command test passed")
        else:
            print(f"✗ Help command failed: {result.stderr}")
    except subprocess.TimeoutExpired:
        print("✗ Help command timed out")
    except Exception as e:
        print(f"✗ Error running help command: {e}")

if __name__ == "__main__":
    test_cli()
