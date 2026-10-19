#!/usr/bin/env python3
"""
Test script for NLM Enhanced CLI

This script demonstrates the basic functionality of the NLM Enhanced CLI.
"""

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
try:
    from nlm_cli import NLMCLI
    CLI_AVAILABLE = True
except ImportError as e:
    print(f"Error: NLM CLI not available: {e}")
    CLI_AVAILABLE = False

def main():
    """Main function."""
    if not CLI_AVAILABLE:
        print("Error: NLM CLI not available")
        print("Please check that NLM modules are properly installed")
        return 1
    
    # Create CLI instance
    cli = NLMCLI()
    
    # Test basic commands
    test_cases = [
        # Basic help
        ['--help'],
        
        # Config help
        ['config', '--help'],
        
        # Simulation help
        ['simulation', '--help'],
        
        # Analysis help
        ['analysis', '--help'],
        
        # Utility help
        ['utility', '--help'],
        
        # Show version
        ['utility', 'version'],
        
        # Show full version
        ['utility', 'version', '--full'],
    ]
    
    print("Testing NLM Enhanced CLI...")
    print("=" * 50)
    
    for i, test_args in enumerate(test_cases, 1):
        print(f"\nTest {i}: {test_args}")
        try:
            exit_code = cli.run_command(test_args)
            print(f"Exit code: {exit_code}")
        except Exception as e:
            print(f"Error: {e}")
            exit_code = 1
        
        if exit_code != 0:
            print(f"Test {i} failed!")
            return 1
    
    print("\n" + "=" * 50)
    print("All CLI tests completed successfully!")
    return 0
if __name__ == "__main__":
    sys.exit(main())