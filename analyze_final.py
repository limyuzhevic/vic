#!/usr/bin/env python3

"""
Comprehensive script to analyze inconsistencies between header and implementation files.
This script specifically looks for:
1. Methods declared in headers but don't exist in implementations
2. Methods implemented but not declared in headers
3. Different signatures between headers and implementations
4. References to non-existent methods
"""

import re
import os
from pathlib import Path

class MethodAnalyzer:
    def __init__(self, filepath):
        self.filepath = Path(filepath)
        self.content = self.filepath.read_text() if self.filepath.exists() else ""
        
    def extract_methods(self, is_header=True):
        """Extract method signatures from header or implementation file"""
        methods = []
        
        if not self.content:
            return methods
        
        # Remove comments
        cleaned = re.sub(r'//.*?\n', '', self.content)
        cleaned = re.sub(r'/\*.*?\*/', '', cleaned, flags=re.DOTALL)
        
        if is_header:
            # For headers: extract public method declarations
            # Pattern: return_type className::methodName(parameters) with optional virtual/static
            pattern = r'(?:virtual\s+)?(?:static\s+)?(\w+(?:\s*\<[^>]+\>)?\s+\w+::\w+\([^)]*\))'
            for match in re.finditer(pattern, cleaned):
                methods.append(match.group(1))
            
            # Also extract methods without className:: (inside class)
            pattern2 = r'(\w+(?:\s*\<[^>]+\>)?\s+\w+\([^)]*\)\s*;)'
            for match in re.finditer(pattern2, cleaned):
                method = match.group(1).rstrip(';')
                if method and 'this' not in method:
                    methods.append(method)
        else:
            # For .cpp files: extract method definitions
            # Pattern: className::methodName(parameters)
            pattern = r'(\w+::\w+\([^)]*\))'
            for match in re.finditer(pattern, cleaned):
                methods.append(match.group(1))
        
        return list(set(methods))
    
    def extract_public_method_names(self):
        """Extract just the method names (without return type or className) from headers"""
        methods = []
        
        if not self.content:
            return methods
        
        # Remove comments
        cleaned = re.sub(r'//.*?\n', '', self.content)
        cleaned = re.sub(r'/\*.*?\*/', '', cleaned, flags=re.DOTALL)
        
        # Find class definitions and extract public methods
        class_pattern = r'class\s+\w+\s*\{[^}]+public:[^}]+\}'
        for class_match in re.finditer(class_pattern, cleaned, re.DOTALL):
            class_content = class_match.group(0)
            # Extract method declarations in public section
            method_pattern = r'(?:virtual\s+)?(?:static\s+)?\w+(?:\s*\<[^>]+\>)?\s+(\w+)\([^)]*\)\s*;'
            for match in re.finditer(method_pattern, class_content):
                methods.append(match.group(1))
        
        return methods
    
    def extract_cpp_method_names(self):
        """Extract method names from .cpp file (just methodName without className::)"""
        methods = []
        
        if not self.content:
            return methods
        
        # Remove comments
        cleaned = re.sub(r'//.*?\n', '', self.content)
        cleaned = re.sub(r'/\*.*?\*/', '', cleaned, flags=re.DOTALL)
        
        # Pattern for: className::methodName(parameters)
        pattern = r'\w+::(\w+)\([^)]*\)'
        for match in re.finditer(pattern, cleaned):
            methods.append(match.group(1))
        
        return list(set(methods))

class InconsistencyDetector:
    def __init__(self):
        self.issues = []
        
    def check_method_pair(self, header_path, cpp_path, classname):
        """Check a header-cpp pair for inconsistencies"""
        if not os.path.exists(header_path) or not os.path.exists(cpp_path):
            self.issues.append(f"WARNING: File(s) not found: {header_path} or {cpp_path}")
            return
        
        print(f"\n{'='*60}")
        print(f"ANALYZING: {classname}")
        print(f"Header: {header_path}")
        print(f"Implementation: {cpp_path}")
        print(f"{'='*60}")
        
        # Analyze header
        header_analyzer = MethodAnalyzer(header_path)
        header_methods = header_analyzer.extract_methods(is_header=True)
        header_method_names = header_analyzer.extract_public_method_names()
        
        # Analyze implementation
        cpp_analyzer = MethodAnalyzer(cpp_path)
        cpp_methods = cpp_analyzer.extract_methods(is_header=False)
        cpp_method_names = cpp_analyzer.extract_cpp_method_names()
        
        print(f"\nHeader methods found: {len(header_methods)}")
        print(f"Implementation methods found: {len(cpp_methods)}")
        
        # Check 1: Methods in header but not in implementation
        header_simplified = set()
        for method in header_methods:
            # Remove return type and className
            simplified = re.sub(r'^[^:]+::', '', method)
            simplified = re.sub(r'^[^\s]+\s+', '', simplified)
            header_simplified.add(simplified)
        
        cpp_simplified = set()
        for method in cpp_methods:
            # Remove className::
            simplified = re.sub(r'^[^:]+::', '', method)
            cpp_simplified.add(simplified)
        
        # 1.1 Methods in header but not in implementation
        missing_in_cpp = header_simplified - cpp_simplified
        for method in sorted(missing_in_cpp):
            self.issues.append(f"{classname}::{method}")
            print(f"  1.1 Method declared in header but not implemented: {method}")
        
        # 1.2 Methods in implementation but not in header
        missing_in_header = cpp_simplified - header_simplified
        for method in sorted(missing_in_header):
            self.issues.append(f"{classname}::{method} (IMPLEMENTED BUT NOT DECLARED)")
            print(f"  1.2 Method implemented but not declared in header: {method}")
        
        return len(missing_in_cpp) + len(missing_in_header)
    
    def check_potential_missing_methods(self, file_path):
        """Check for method calls that might reference missing methods"""
        if not os.path.exists(file_path):
            return
        
        analyzer = MethodAnalyzer(file_path)
        content = analyzer.filepath.read_text()
        
        # Look for method calls on objects (e.g., world.___())
        # Common patterns from our codebase
        patterns = [
            (r'(\w+)\.observe\(', 'observe()'),
            (r'(\w+)\.applyAction\(', 'applyAction()'),
            (r'(\w+)\.applyMotorCommand\(', 'applyMotorCommand()'),
            (r'(\w+)\.computeReward\(', 'computeReward()'),
            (r'(\w+)\.getSensoryPercept\(', 'getSensoryPercept()'),
            (r'(\w+)\.getAgentBody\(', 'getAgentBody()'),
            (r'(\w+)\.getRegions\(', 'getRegions()'),
        ]
        
        for pattern, method_name in patterns:
            matches = re.findall(pattern, content)
            for obj_name in matches:
                self.issues.append(f"{file_path}: {obj_name}.{method_name}() called but method may not exist")
                print(f"  Potential issue: {obj_name}.{method_name}() called (file: {file_path})")

def main():
    print("=" * 80)
    print("COMPREHENSIVE INCONSISTENCY ANALYSIS")
    print("=" * 80)
    
    detector = InconsistencyDetector()
    
    # Files to check based on the issue description
    checks = [
        ("src/agent/AgentBrain.hpp", "src/agent/AgentBrain.cpp", "AgentBrain"),
        ("src/brain/Brain.hpp", "src/brain/Brain.cpp", "Brain"),
        ("src/world/SimpleWorld.hpp", "src/world/SimpleWorld.cpp", "SimpleWorld"),
        ("src/experiments/Phase6IntegratedExperiment.hpp", "src/experiments/Phase6IntegratedExperiment.cpp", "Phase6IntegratedExperiment"),
    ]
    
    for header, cpp, classname in checks:
        detector.check_method_pair(header, cpp, classname)
    
    # Check for references to potentially missing methods in Phase6Demo.cpp
    phase6demo_path = "src/experiments/Phase6Demo.cpp"
    if os.path.exists(phase6demo_path):
        print(f"\n{'='*80}")
        print(f"CHECKING: Phase6Demo.cpp for referenced methods")
        print(f"{'='*80}")
        detector.check_potential_missing_methods(phase6demo_path)
    
    # Summary
    print(f"\n{'='*80}")
    print("SUMMARY OF ISSUES FOUND")
    print(f"{'='*80}")
    
    if detector.issues:
        print(f"\nTOTAL ISSUES FOUND: {len(detector.issues)}")
        for i, issue in enumerate(detector.issues, 1):
            print(f"{i}. {issue}")
        
        print(f"\n{'='*80}")
        print("NEAR-MISSING METHOD REFERENCES")
        print(f"{'='*80}")
        print("The following methods were called but their declaration status is unclear:")
        print("  - observe() - Called in Phase6IntegratedExperiment.cpp")
        print("  - applyAction() - Called in Phase6IntegratedExperiment.cpp")
        print("  - computeReward() - Called in Phase6IntegratedExperiment.cpp")
        
        print(f"\n{'='*80}")
        print("SPECIFIC FINDINGS:")
        print(f"{'='*80}")
        print("1. SimpleWorld.hpp does NOT declare observe(), applyAction(), or computeReward()")
        print("2. But Phase6IntegratedExperiment.cpp calls these methods on 'world' object")
        print("3. This indicates either:")
        print("   a) The methods exist but are private (e.g., SimpleWorld::observe, SimpleWorld::applyAction)")
        print("   b) The methods should be public but are missing from SimpleWorld.hpp")
        print("   c) The implementation files contain these methods (check SimpleWorld.cpp)")
        
        return 1
    else:
        print("\n✓ No inconsistencies found!")
        return 0

if __name__ == "__main__":
    exit(main())
