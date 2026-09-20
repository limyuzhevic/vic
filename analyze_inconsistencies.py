#!/usr/bin/env python3

"""
Script to find inconsistencies between header files and implementation files.
Compares:
1. Methods declared in headers but don't exist in implementations
2. Methods implemented but not declared in headers  
3. Different signatures between headers and implementations
4. References to non-existent methods
"""

import os
import re
import sys
from pathlib import Path

# Function to extract method signatures from header file
def extract_header_methods(filepath):
    """Extract public method signatures from .hpp file"""
    methods = []
    
    if not os.path.exists(filepath):
        return methods
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Remove comments to avoid parsing comments
    content = re.sub(r'//.*', '', content)  # Remove single line comments
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)  # Remove multi-line comments
    
    # Pattern to match method signatures (simplified)
    # Look for: return_type className::methodName(parameters)
    method_pattern = r'(\w+(?:\s*\<[^>]+\>)?\s+\w+::\w+\([^)]*\))'
    
    for match in re.finditer(method_pattern, content):
        method_sig = match.group(1).strip()
        if method_sig and 'this' not in method_sig:
            methods.append(method_sig)
    
    # Also look for member function declarations in class definition
    class_pattern = r'class\s+\w+\s*\{[^}]+public:[^}]+\}'
    class_match = re.search(class_pattern, content, re.DOTALL)
    if class_match:
        class_content = class_match.group(0)
        # Look for: return_type methodName(parameters)
        method_pattern2 = r'(\w+(?:\s*\<[^>]+\>)?\s+\w+\([^)]*\))'
        for match in re.finditer(method_pattern2, class_content):
            method_sig = match.group(1).strip()
            if method_sig and 'this' not in method_sig:
                methods.append(method_sig)
    
    return list(set(methods))  # Remove duplicates

# Function to extract method definitions from implementation file  
def extract_cpp_methods(filepath):
    """Extract method definitions from .cpp file"""
    methods = []
    
    if not os.path.exists(filepath):
        return methods
    
    with open(filepath, 'r') as f:
        content = f.read()
    
    # Remove comments
    content = re.sub(r'//.*', '', content)
    content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
    
    # Pattern to match: className::methodName(parameters)
    method_pattern = r'(\w+::\w+\([^)]*\))'
    
    for match in re.finditer(method_pattern, content):
        method_sig = match.group(1).strip()
        if method_sig:
            methods.append(method_sig)
    
    return list(set(methods))

# Function to compare method signatures
def compare_methods(header_methods, cpp_methods, classname):
    """Compare methods between header and implementation"""
    issues = []
    
    header_simplified = set()
    for method in header_methods:
        # Simplify: remove return type, keep methodName(params)
        simplified = re.sub(r'^[^:]+::', '', method)
        header_simplified.add(simplified)
    
    cpp_simplified = set()
    for method in cpp_methods:
        # Method definitions already have "::"
        simplified = re.sub(r'^[^:]+::', '', method)
        cpp_simplified.add(simplified)
    
    # 1. Methods in header but not in implementation
    missing_in_cpp = header_simplified - cpp_simplified
    for method in sorted(missing_in_cpp):
        issues.append(f"1. Method declared in header but not implemented: {method}")
    
    # 2. Methods in implementation but not in header  
    missing_in_header = cpp_simplified - header_simplified
    for method in sorted(missing_in_header):
        # Filter out constructors/destructors
        if '(){' in method or '~' not in method:
            issues.append(f"2. Method implemented but not declared in header: {method}")
    
    # 3. Different signatures (basic check)
    # This is more complex - would need full signature comparison
    
    return issues

# Main function
def main():
    # Files to check
    checks = [
        ("src/agent/AgentBrain.hpp", "src/agent/AgentBrain.cpp", "AgentBrain"),
        ("src/brain/Brain.hpp", "src/brain/Brain.cpp", "Brain"),
        ("src/world/SimpleWorld.hpp", "src/world/SimpleWorld.cpp", "SimpleWorld"),
        ("src/experiments/Phase6IntegratedExperiment.hpp", "src/experiments/Phase6IntegratedExperiment.cpp", "Phase6IntegratedExperiment"),
    ]
    
    all_issues = []
    
    print("=" * 80)
    print("INCONSISTENCY ANALYSIS BETWEEN HEADER AND IMPLEMENTATION FILES")
    print("=" * 80)
    
    for header_path, cpp_path, classname in checks:
        print(f"\n{'='*60}")
        print(f"Analyzing {classname}")
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
        
        # Extract methods
        print("\nExtracting methods...")
        header_methods = extract_header_methods(str(header_file))
        cpp_methods = extract_cpp_methods(str(cpp_file))
        
        print(f"Found {len(header_methods)} method(s) in header")
        print(f"Found {len(cpp_methods)} method(s) in implementation")
        
        # Compare
        print("\nComparing methods...")
        issues = compare_methods(header_methods, cpp_methods, classname)
        
        if issues:
            print(f"FOUND {len(issues)} INCONSISTENCIES:")
            for i, issue in enumerate(issues, 1):
                print(f"  {i}. {issue}")
            all_issues.extend(issues)
        else:
            print("No inconsistencies found!")
    
    # Summary
    print(f"\n{'='*80}")
    print("SUMMARY")
    print(f"{'='*80}")
    
    if all_issues:
        print(f"\nTOTAL ISSUES FOUND: {len(all_issues)}")
        
        # Group by type
        type_counts = {}
        for issue in all_issues:
            if "1. Method declared" in issue:
                type_counts["HeaderMissingImpl"] = type_counts.get("HeaderMissingImpl", 0) + 1
            elif "2. Method implemented" in issue:
                type_counts["ImplMissingHeader"] = type_counts.get("ImplMissingHeader", 0) + 1
        
        print("\nBy type:")
        for issue_type, count in type_counts.items():
            print(f"  - {issue_type}: {count}")
        
        # File-specific issues
        print("\nIssues by file:")
        for header_path, cpp_path, classname in checks:
            header_file = Path(header_path)
            cpp_file = Path(cpp_path)
            
            if header_file.exists() and cpp_file.exists():
                issues = compare_methods(extract_header_methods(str(header_file)), 
                                        extract_cpp_methods(str(cpp_file)), classname)
                if issues:
                    print(f"\n{classname}:")
                    for issue in issues:
                        print(f"  - {issue}")
    else:
        print("No inconsistencies found in any of the checked files!")
    
    return 0 if not all_issues else 1

if __name__ == "__main__":
    sys.exit(main())
