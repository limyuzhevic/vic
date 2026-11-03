#!/usr/bin/env python3
"""
Static analysis test script for NLM Phase 6 integration.
Analyzes code quality and integration issues without requiring building.
"""

import os
import re
import sys

def analyze_file(filepath, description):
    """Analyze a file and return issues found"""
    print(f"\n{'='*60}")
    print(f"ANALYSIS: {description}")
    print(f"File: {filepath}")
    print(f"{'='*60}")
    
    issues = []
    
    try:
        with open(filepath, 'r') as f:
            content = f.read()
            lines = content.split('\n')
        
        # Check for C++20 features that should be used
        if 'auto config = std::make_shared<Config>();' in content:
            issues.append("Uses make_shared - consider using unique_ptr for better memory management")
        
        # Check for magic numbers
        magic_numbers = re.findall(r'\b\d+\b', content)
        large_numbers = [num for num in magic_numbers if int(num) > 100 and int(num) < 10000]
        if len(large_numbers) > 5:
            issues.append(f"Uses many magic numbers ({len(large_numbers)} found) - consider defining constants")
        
        # Check for long functions (simple heuristic)
        function_bodies = re.findall(r'int\s+\w+\(.*?^\s*\}', content, re.MULTILINE | re.DOTALL)
        for body in function_bodies:
            lines_in_function = body.count('\n')
            if lines_in_function > 50:
                issues.append(f"Function with {lines_in_function} lines - consider breaking down")
        
        # Check for hardcoded paths
        if '/tmp/' in content:
            issues.append("Uses hardcoded /tmp/ paths - consider making configurable")
        
        # Check for TODO comments
        todo_matches = re.findall(r'//.*TODO|//.*FIXME|//.*HACK', content)
        if todo_matches:
            issues.append(f"Contains {len(todo_matches)} TODO/FIXME/HACK comments")
        
        # Check for include guards in headers (if we had header files)
        
    except Exception as e:
        issues.append(f"Error reading file: {e}")
    
    # Print issues
    if issues:
        print(f"ISSUES FOUND ({len(issues)}):")
        for i, issue in enumerate(issues, 1):
            print(f"  {i}. {issue}")
        return len(issues)
    else:
        print("No issues found")
        return 0

def analyze_cmake_integration():
    """Analyze CMake integration issues"""
    print(f"\n{'='*60}")
    print("ANALYSIS: CMake Integration Issues")
    print(f"{'='*60}")
    
    issues = []
    
    # Check main CMakeLists.txt
    with open("CMakeLists.txt", "r") as f:
        main_cmake = f.read()
    
    # Check if Python bindings are properly integrated
    if "NLM_BUILD_PYTHON_BINDINGS" in main_cmake:
        if "pyproject.toml" in main_cmake:
            issues.append("Python bindings option in CMakeLists.txt conflicts with pyproject.toml approach")
            issues.append("Main CMakeLists.txt should not build Python bindings - use pyproject.toml instead")
    
    # Check for duplicate library definitions
    if "add_library(nlm_core STATIC" in main_cmake and "add_library(nlm_core STATIC" in main_cmake:
        # Count occurrences
        count = main_cmake.count("add_library(nlm_core STATIC")
        if count > 1:
            issues.append(f"nlm_core library defined {count} times - potential conflict")
    
    # Check demo compilation targets
    demo_targets = []
    for line in main_cmake.split('\n'):
        if 'add_executable(' in line and '_demo' in line:
            demo_targets.append(line.strip())
    
    print(f"Found {len(demo_targets)} demo executables:")
    for target in demo_targets:
        print(f"  {target}")
    
    if len(demo_targets) != 5:  # We expect 5: phase3, phase4, phase5, phase6, phase6_text
        issues.append(f"Expected 5 demo executables but found {len(demo_targets)}")
    
    return issues

def analyze_python_bindings():
    """Analyze Python bindings setup"""
    print(f"\n{'='*60}")
    print("ANALYSIS: Python Bindings Setup")
    print(f"{'='*60}")
    
    issues = []
    
    # Check pyproject.toml
    with open("pyproject.toml", "r") as f:
        pyproject = f.read()
    
    # Check if pyproject.toml has proper scikit-build configuration
    if "scikit-build-core>=0.5.0" in pyproject:
        if "pybind11>=2.11.0" in pyproject:
            print("✓ pyproject.toml has proper scikit-build and pybind11 dependencies")
    else:
        issues.append("Missing scikit-build-core or pybind11 in pyproject.toml")
    
    # Check python/CMakeLists.txt
    if os.path.exists("python/CMakeLists.txt"):
        with open("python/CMakeLists.txt", "r") as f:
            python_cmake = f.read()
        
        # Check if python/CMakeLists.txt includes all necessary source files
        source_count = python_cmake.count("${NLM_SOURCE_DIR}/")
        if source_count < 50:  # Should have many source files
            issues.append(f"python/CMakeLists.txt may be missing source files (only {source_count} references found)")
    
    return issues

def analyze_experiment_structure():
    """Analyze experiment structure for consistency"""
    print(f"\n{'='*60}")
    print("ANALYSIS: Experiment Structure Consistency")
    print(f"{'='*60}")
    
    issues = []
    
    phases = ["Phase3", "Phase4", "Phase5", "Phase6"]
    
    for phase in phases:
        cpp_file = f"src/experiments/{phase}Demo.cpp"
        experiment_cpp = f"src/experiments/{phase}Experiment.cpp"
        
        if os.path.exists(cpp_file):
            with open(cpp_file, "r") as f:
                content = f.read()
            
            # Check for proper structure
            if "int main(" not in content:
                issues.append(f"{phase}Demo.cpp missing main() function")
            
            if "namespace nlm" not in content:
                issues.append(f"{phase}Demo.cpp not in nlm namespace")
            
            # Check for argument parsing
            if "argc" not in content or "argv" not in content:
                issues.append(f"{phase}Demo.cpp doesn't parse command line arguments")
    
    return issues

def generate_suggestions(issues):
    """Generate suggestions for fixing issues"""
    print(f"\n{'='*60}")
    print("SUGGESTIONS FOR IMPROVEMENT")
    print(f"{'='*60}")
    
    suggestions = [
        "1. Refactor long functions into smaller, focused functions",
        "2. Define constants for magic numbers instead of hardcoded values",
        "3. Make hardcoded paths configurable via command line or config file",
        "4. Replace TODO comments with actual fixes or documented reasons",
        "5. Use RAII (smart pointers) instead of raw pointers where possible",
        "6. Remove duplicate library definitions in CMake",
        "7. Ensure consistent argument parsing across all demos",
        "8. Add proper error handling for file operations",
        "9. Consider using dependency injection for configuration objects",
        "10. Add unit tests for core functionality"
    ]
    
    for suggestion in suggestions:
        print(f"  {suggestion}")

def main():
    print("NLM Phase 6 Integration Static Analysis")
    print("=" * 60)
    
    total_issues = 0
    
    # Analyze demo files
    demo_files = [
        ("Phase 3 Demo", "src/experiments/Phase3Demo.cpp"),
        ("Phase 4 Demo", "src/experiments/Phase4Demo.cpp"),
        ("Phase 5 Demo", "src/experiments/Phase5Demo.cpp"),
        ("Phase 6 Demo", "src/experiments/Phase6Demo.cpp"),
        ("Phase 6 Text Demo", "src/experiments/Phase6TextDemo.cpp"),
    ]
    
    for desc, filepath in demo_files:
        if os.path.exists(filepath):
            issues = analyze_file(filepath, desc)
            total_issues += issues
        else:
            print(f"\nERROR: {filepath} not found!")
    
    # Analyze CMake integration
    cmake_issues = analyze_cmake_integration()
    total_issues += len(cmake_issues)
    for issue in cmake_issues:
        print(f"  {issue}")
    
    # Analyze Python bindings
    python_issues = analyze_python_bindings()
    total_issues += len(python_issues)
    for issue in python_issues:
        print(f"  {issue}")
    
    # Analyze experiment structure
    structure_issues = analyze_experiment_structure()
    total_issues += len(structure_issues)
    for issue in structure_issues:
        print(f"  {issue}")
    
    # Generate suggestions
    generate_suggestions(total_issues)
    
    # Summary
    print(f"\n{'='*60}")
    print("SUMMARY")
    print(f"{'='*60}")
    print(f"Total issues identified: {total_issues}")
    
    if total_issues > 50:
        print("❌ CRITICAL ISSUES FOUND - Major refactoring needed!")
        return 1
    elif total_issues > 20:
        print("⚠️  MODERATE ISSUES FOUND - Improvements recommended")
        return 0
    else:
        print("✅ Few issues found - Good baseline code quality")
        return 0

if __name__ == "__main__":
    sys.exit(main())