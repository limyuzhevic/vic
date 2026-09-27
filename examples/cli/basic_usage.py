#!/usr/bin/env python3
"""
Basic NLM CLI usage examples
"""

import subprocess
import sys
import os

def run_command(cmd):
    """Run a command and return output"""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr
    except Exception as e:
        return 1, "", str(e)

def main():
    print("NLM CLI Basic Usage Examples")
    print("=" * 40)
    
    examples = [
        ("Help", ["nlm", "--help"]),
        ("Version", ["nlm", "--version"]),
        ("Config List", ["nlm", "config", "list"]),
    ]
    
    for name, cmd in examples:
        print(f"\nExample: {name}")
        print(f"Command: {' '.join(cmd)}")
        returncode, stdout, stderr = run_command(' '.join(cmd))
        
        if returncode == 0:
            print("✓ Command executed successfully")
            if stdout:
                print(f"Output: {stdout[:200]}...")
        else:
            print(f"✗ Command failed (exit code: {returncode})")
            if stderr:
                print(f"Error: {stderr}")

if __name__ == "__main__":
    main()
