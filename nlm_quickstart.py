#!/usr/bin/env python3
"""
NLM Phase 2 Quick Start Guide

This script demonstrates the enhanced NLM Phase 2 system with all improvements implemented.

Usage:
    python3 nlm_quickstart.py [options]

Options:
    --test MODE      Run specific test (connectivity, plasticity, stdp, all)
    --neurons N      Number of neurons (default: 500)
    --steps S        Number of simulation steps (default: 1000)
    --help           Show this help message
"""

import subprocess
import sys
import argparse

def run_command(cmd):
    """Run a shell command and return output."""
    try:
        result = subprocess.run(cmd, shell=True, capture_output=True, text=True)
        return result.returncode, result.stdout, result.stderr
    except Exception as e:
        return -1, "", str(e)

def print_banner():
    print("=" * 60)
    print("NLM Phase 2: Real Neural Computation - Quick Start")
    print("=" * 60)
    print()

def print_help():
    print("NLM Phase 2 Quick Start Guide")
    print()
    print("This script demonstrates the enhanced NLM Phase 2 system with:")
    print("✓ Fixed memory system integration")
    print("✓ Enhanced checkpoint save/load")
    print("✓ Memory leak prevention")
    print("✓ Thread safety in plasticity experiments")
    print("✓ Advanced command-line interface")
    print("✓ Cognitive system integration")
    print()
    print("USAGE:")
    print("    python3 nlm_quickstart.py --test MODE [options]")
    print()
    print("MODES:")
    print("    connectivity    Run basic connectivity test")
    print("    plasticity     Run plasticity learning experiment")
    print("    stdp           Run STDP verification")
    print("    all            Run all tests (default)")
    print()
    print("EXAMPLES:")
    print("    python3 nlm_quickstart.py --test connectivity")
    print("    python3 nlm_quickstart.py --test plasticity --neurons 1000")
    print("    python3 nlm_quickstart.py --test stdp --steps 500")
    print()
    print("For more information, see:")
    print("    NLM_Phase2_Documentation.md")
    print()

def run_nlm_test(test_mode, neurons, steps):
    """Run NLM with specified test mode and parameters."""
    print(f"Running NLM test: {test_mode}")
    print(f"Configuration: {neurons} neurons, {steps} steps")
    print()
    
    # Build command
    cmd = f"./{test_mode}"
    if neurons != 500:
        cmd += f" --neurons {neurons}"
    if steps != 1000:
        cmd += f" --steps {steps}"
    
    print(f"Executing: {cmd}")
    print("-" * 60)
    
    returncode, stdout, stderr = run_command(cmd)
    
    if returncode == 0:
        print("Test completed successfully!")
        print(stdout)
    else:
        print("Test failed with error:")
        print(stderr)
    
    return returncode == 0

def main():
    parser = argparse.ArgumentParser(description="NLM Phase 2 Quick Start")
    parser.add_argument("--test", choices=["connectivity", "plasticity", "stdp", "all"], 
                       default="all", help="Test mode to run")
    parser.add_argument("--neurons", type=int, default=500, help="Number of neurons")
    parser.add_argument("--steps", type=int, default=1000, help="Number of simulation steps")
    parser.add_argument("--help", action="store_true", help="Show help message")
    
    args = parser.parse_args()
    
    if args.help or len(sys.argv) == 1:
        print_help()
        return
    
    print_banner()
    
    success = True
    
    if args.test == "all":
        # Run all tests
        success = run_nlm_test("connectivity", args.neurons, args.steps)
        if success:
            success = run_nlm_test("plasticity", args.neurons, args.steps)
        if success:
            success = run_nlm_test("stdp", args.neurons, args.steps)
    else:
        # Run specific test
        success = run_nlm_test(args.test, args.neurons, args.steps)
    
    print()
    print("=" * 60)
    if success:
        print("All tests completed successfully!")
        print("NLM Phase 2 system is working correctly.")
    else:
        print("Some tests failed. Please check the output above.")
        print("See NLM_Phase2_Documentation.md for troubleshooting.")
    print("=" * 60)
    
    return 0 if success else 1

if __name__ == "__main__":
    sys.exit(main())
