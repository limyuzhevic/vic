#!/usr/bin/env python3

"""
Advanced script to find inconsistencies between header files and implementation files.
"""

import os
import re
import sys
from pathlib import Path

# Pattern to extract method signatures from .hpp files (public methods)
HEADER_METHOD_PATTERNS = [
    # Pattern for: return_type className::methodName(parameters)
    r'(\w+(?:\s*\<[^>]+\>)?\s+\w+::\w+\([^)]*\))',
    # Pattern for: return_type methodName(parameters) inside class
    r'(\w+(?:\s*\<[^>]+\>)?\s+\w+\([^)]*\))(?=\s*[;{]|$)',
    # Pattern for: virtual return_type methodName(parameters)
    r'(\bvirtual\s+\w+(?:\s*\<[^>]+\>)?\s+\w+\([^)]*\))(?=\s*[;]|$)',
    # Pattern for: static return_type methodName(parameters)
    r'(\bstatic\s+\w+(?:\s*\<[^>]+\>)?\s+\w+\([^)]*\))(?=\s*[;]|$)',
]

# Pattern to extract method definitions from .cpp files
def extract_header_methods(filepath, classname):
    """Extract method signatures from .hpp file"""
    methods = []
    
    if not os.path.exists(filepath):
        return methods
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Remove comments
    content = re.sub(r'//.*', '', content)
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    
    # Extract methods from class definition
    # Look for patterns like: return_type methodName(parameters)
    method_pattern = r'(\w+(?:\s*\<[^>]+\>)?\s+\w+\([^)]*\))'
    
    for match in re.finditer(method_pattern, content):
        method_sig = match.group(1).strip()
        if method_sig and 'this' not in method_sig and '::' not in method_sig:
            methods.append(method_sig)
    
    return list(set(methods))

def extract_cpp_methods(filepath, classname):
    """Extract method definitions from .cpp file"""
    methods = []
    
    if not os.path.exists(filepath):
        return methods
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Remove comments
    content = re.sub(r'//.*', '', content)
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    
    # Pattern for: className::methodName(parameters)
    method_pattern = r'(\w+::\w+\([^)]*\))'
    
    for match in re.finditer(method_pattern, content):
        method_sig = match.group(1).strip()
        if method_sig:
            methods.append(method_sig)
    
    return list(set(methods))

def analyze_file_pair(header_file, cpp_file, classname):
    """Analyze a header-cpp file pair for inconsistencies"""
    issues = []
    
    if not os.path.exists(header_file) or not os.path.exists(cpp_file):
        return issues
    
    # Extract methods
    header_methods = extract_header_methods(header_file, classname)
    cpp_methods = extract_cpp_methods(cpp_file, classname)
    
    print(f"\n  Header methods: {len(header_methods)}")
    for m in sorted(header_methods):
        print(f"    - {m}")
    
    print(f"\n  Implementation methods: {len(cpp_methods)}")
    for m in sorted(cpp_methods):
        print(f"    - {m}")
    
    # Compare
    header_simplified = set()
    for method in header_methods:
        # Remove return type, keep methodName(params)
        simplified = re.sub(r'^\w+(?:\s*<[^>]+>)?\s+', '', method)
        header_simplified.add(simplified)
    
    cpp_simplified = set()
    for method in cpp_methods:
        # Remove className::, keep methodName(params)
        simplified = re.sub(r'^\w+::', '', method)
        cpp_simplified.add(simplified)
    
    # Check 1: Methods in header but not in implementation
    missing_in_cpp = header_simplified - cpp_simplified
    if missing_in_cpp:
        for method in sorted(missing_in_cpp):
            issues.append(f"  1. Method declared in header but not implemented: {method}")
    
    # Check 2: Methods in implementation but not in header
    missing_in_header = cpp_simplified - header_simplified
    if missing_in_header:
        for method in sorted(missing_in_header):
            if '(' in method and ')' in method:  # Filter for actual methods, not just patterns
                issues.append(f"  2. Method implemented but not declared in header: {method}")
    
    return issues

def main():
    print("=" * 80)
    print("ADVANCED INCONSISTENCY ANALYSIS")
    print("=" * 80)
    
    checks = [
        ("src/agent/AgentBrain.hpp", "src/agent/AgentBrain.cpp", "AgentBrain"),
        ("src/brain/Brain.hpp", "src/brain/Brain.cpp", "Brain"),
        ("src/world/SimpleWorld.hpp", "src/world/SimpleWorld.cpp", "SimpleWorld"),
        ("src/experiments/Phase6IntegratedExperiment.hpp", "src/experiments/Phase6IntegratedExperiment.cpp", "Phase6IntegratedExperiment"),
    ]
    
    all_issues = []
    
    for header_path, cpp_path, classname in checks:
        print(f"\n{'='*60}")
        print(f"ANALYZING {classname}")
        print(f"Header: {header_path}")
        print(f"Implementation: {cpp_path}")
        print(f"{'='*60}")
        
        # Check if files exist
        header_file = Path(header_path)
        cpp_file = Path(cpp_path)
        
        if not header_file.exists():
            print(f"WARNING: Header file not found: {header_file}")
            continue
        
        if not cpp_file.exists():
            print(f"WARNING: Implementation file not found: {cpp_file}")
            continue
        
        # Analyze
        issues = analyze_file_pair(str(header_file), str(cpp_file), classname)
        
        if issues:
            print(f"\n  FOUND {len(issues)} INCONSISTENCIES:")
            all_issues.extend(issues)
        else:
            print(f"\n  ✓ No inconsistencies found!")
    
    # Special check for Phase6Demo references
    print(f"\n{'='*80}")
    print("SPECIAL CHECK: Phase6Demo INCONSISTENCIES")
    print(f"{'='*80}")
    
    phase6demo_file = Path("src/experiments/Phase6Demo.cpp")
    if phase6demo_file.exists():
        print(f"\nChecking {phase6demo_file} for method calls that don't exist...")
        
        with open(phase6demo_file, 'r') as f:
            content = f.read()
        
        # Look for method calls
        method_calls = re.findall(r'\w+\.\w+\([^)]*\)', content)
        
        # Known missing methods based on header-cpp analysis
        potential_missing = [
            ('observe(', 'SimpleWorld::observe method not found in header'),
            ('applyAction(', 'SimpleWorld::applyAction method not found in header'),
            ('computeReward(', 'SimpleWorld::computeReward method not found in header'),
        ]
        
        for pattern, description in potential_missing:
            if pattern in content:
                print(f"  ✗ {description}")
                all_issues.append(f"  Phase6Demo.cpp: References {description}")
    
    # Final summary
    print(f"\n{'='*80}")
    print("FINAL SUMMARY")
    print(f"{'='*80}")
    
    if all_issues:
        print(f"\nTOTAL ISSUES FOUND: {len(all_issues)}")
        print("\nDetailed issues:")
        for i, issue in enumerate(all_issues, 1):
            print(f"{i}. {issue}")
        
        return 1
    else:
        print("\n✓ No inconsistencies found!")
        return 0

if __name__ == "__main__":
    sys.exit(main())
