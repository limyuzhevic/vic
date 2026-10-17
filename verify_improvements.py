#!/usr/bin/env python3

"""
Simple test to verify NLM code improvements.
This test checks that key improvements are working correctly.
"""

import os
import sys
import subprocess
import re
from pathlib import Path

def check_file_has_header_guard(filepath):
    """Check if a .cpp file has a header guard."""
    with open(filepath, 'r') as f:
        content = f.read(100)  # Read first 100 bytes
        return '#pragma once' in content

def check_constants_usage(filepath, constant_name):
    """Check if a constant name is used in a file."""
    with open(filepath, 'r') as f:
        content = f.read()
        # Simple check for constant usage (not perfect)
        return constant_name in content

def check_spike_history_optimization(filepath):
    """Check if spike history optimization is present."""
    with open(filepath, 'r') as f:
        content = f.read()
        # Check for addSpikeRecord method or similar optimization
        return 'addSpikeRecord' in content or 'addPostSpikeRecord' in content

def main():
    print("=== NLM Code Improvements Verification ===\n")
    
    # Get the project root directory
    project_root = Path(__file__).parent
    
    # Test files to check
    test_files = [
        project_root / 'src' / 'brain' / 'Neuron.cpp',
        project_root / 'src' / 'brain' / 'Synapse.cpp',
        project_root / 'src' / 'brain' / 'NeuralRegion.cpp',
    ]
    
    print("1. Checking Header Guards:")
    header_guard_issues = []
    for filepath in test_files:
        if filepath.exists():
            has_guard = check_file_has_header_guard(filepath)
            status = "✓" if has_guard else "✗"
            print(f"  {status} {filepath.name}: {'Has' if has_guard else 'Missing'} header guard")
            if not has_guard:
                header_guard_issues.append(str(filepath))
        else:
            print(f"  ✗ {filepath.name}: File not found")
            header_guard_issues.append(str(filepath))
    
    print("\n2. Checking Constants Usage:")
    constants_to_check = [
        ('Neuron.cpp', 'MEMBRANE_POTENTIAL_BOUNDS_MIN'),
        ('Neuron.cpp', 'DEFAULT_THRESHOLD'),
        ('Synapse.cpp', 'MIN_WEIGHT'),
        ('Synapse.cpp', 'EXCITATORY_WEIGHT_MEAN'),
    ]
    
    constant_issues = []
    for filename, constant in constants_to_check:
        found_file = None
        for filepath in test_files:
            if filepath.name == filename:
                found_file = filepath
                break
        
        if found_file:
            has_constant = check_constants_usage(found_file, constant)
            status = "✓" if has_constant else "✗"
            print(f"  {status} {filename}: {'Uses' if has_constant else 'Missing'} {constant}")
            if not has_constant:
                constant_issues.append(f"{filename}: {constant}")
        else:
            print(f"  ✗ {filename}: File not found")
            constant_issues.append(f"{filename}: {constant}")
    
    print("\n3. Checking Spike History Optimizations:")
    spike_history_issues = []
    for filepath in test_files:
        if filepath.exists():
            has_optimization = check_spike_history_optimization(filepath)
            status = "✓" if has_optimization else "✗"
            print(f"  {status} {filepath.name}: {'Optimized' if has_optimization else 'Not optimized'}")
            if not has_optimization and 'Neuron' in filepath.name or 'Synapse' in filepath.name:
                spike_history_issues.append(str(filepath))
        else:
            print(f"  ✗ {filepath.name}: File not found")
    
    print("\n4. Checking TODO Comments:")
    todo_files = []
    todo_issues = []
    
    # Look for TODO comments in the codebase
    for root, dirs, files in os.walk(project_root / 'src'):
        for file in files:
            if file.endswith('.hpp'):
                filepath = Path(root) / file
                try:
                    with open(filepath, 'r') as f:
                        content = f.read()
                        if 'TODO' in content:
                            todo_files.append(str(filepath))
                            # Check if it's a Phase 2 TODO
                            if 'PHASE 2' in content:
                                todo_issues.append(str(filepath))
                except:
                    pass
    
    print(f"  Found {len(todo_files)} files with TODO comments")
    if todo_issues:
        print(f"  {len(todo_issues)} files with Phase 2 TODOs (should be implemented)")
        for filepath in todo_issues[:3]:  # Show first 3
            print(f"    - {os.path.basename(filepath)}")
        if len(todo_issues) > 3:
            print(f"    ... and {len(todo_issues) - 3} more")
    else:
        print("  ✓ No Phase 2 TODOs found (or they were implemented)")
    
    print("\n=== Summary ===")
    all_issues = len(header_guard_issues) + len(constant_issues) + len(spike_history_issues) + len(todo_issues)
    
    if all_issues == 0:
        print("✓ All improvements verified successfully!")
        print("\nKey improvements confirmed:")
        print("- Header guards added to .cpp files")
        print("- Named constants implemented")
        print("- Spike history optimization in place")
        print("- TODO comments reviewed")
        return 0
    else:
        print(f"✗ {all_issues} issues found")
        if header_guard_issues:
            print("- Missing header guards:", len(header_guard_issues))
        if constant_issues:
            print("- Missing constants:", len(constant_issues))
        if spike_history_issues:
            print("- Missing spike history optimizations:", len(spike_history_issues))
        if todo_issues:
            print("- Phase 2 TODOs to implement:", len(todo_issues))
        return 1

if __name__ == '__main__':
    sys.exit(main())