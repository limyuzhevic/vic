#!/usr/bin/env python3
import os
import re
import glob
from pathlib import Path

def find_duplicate_classes(root_dir):
    """Find duplicate class declarations across files"""
    class_pattern = re.compile(r'^\s*(?:class|struct)\s+(\w+)')
    classes_found = {}
    
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.hpp') or file.endswith('.h'):
                filepath = os.path.join(root, file)
                with open(filepath, 'r') as f:
                    content = f.read()
                    
                # Find all class/struct definitions
                for line in content.splitlines():
                    match = class_pattern.match(line.strip())
                    if match:
                        class_name = match.group(1)
                        if class_name not in classes_found:
                            classes_found[class_name] = []
                        classes_found[class_name].append(filepath)
    
    # Filter duplicates
    duplicates = {name: files for name, files in classes_found.items() 
                  if len(files) > 1}
    
    return duplicates

def find_todos_fixes(root_dir):
    """Find TODOs, FIXMEs, XXX, etc."""
    patterns = [
        r'TODO\(|FIXME\(|XXX\(|@@@\(|NOTE\(|HACK\(|',
        r'//\s*TODO\s*:',
        r'//\s*FIXME\s*:',
        r'/\*\s*TODO\s*\*/',
        r'/\*\s*FIXME\s*\*/'
    ]
    
    issues = []
    
    for root, dirs, files in os.walk(root_dir):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.hpp') or file.endswith('.h'):
                filepath = os.path.join(root, file)
                with open(filepath, 'r') as f:
                    content = f.read()
                    
                for pattern in patterns:
                    matches = re.finditer(pattern, content, re.IGNORECASE)
                    for match in matches:
                        # Get context
                        start = max(0, match.start() - 50)
                        end = min(len(content), match.end() + 50)
                        context = content[start:end]
                        issues.append({
                            'file': filepath,
                            'pattern': pattern,
                            'context': context.strip()
                        })
    
    return issues

def check_build_config(root_dir):
    """Check build configuration files"""
    issues = []
    
    # Check CMakeLists.txt
    cmake_path = os.path.join(root_dir, 'CMakeLists.txt')
    if os.path.exists(cmake_path):
        with open(cmake_path, 'r') as f:
            cmake_content = f.read()
            
        # Check for common issues
        if 'add_executable' in cmake_content and 'install' not in cmake_content:
            issues.append('CMakeLists.txt: Missing install directives')
        
        # Check source files referenced in CMake exist
        src_pattern = r'src/[a-zA-Z_][a-zA-Z0-9_]*\.cpp'
        for match in re.finditer(src_pattern, cmake_content):
            src_file = match.group(0)
            full_path = os.path.join(root_dir, src_file)
            if not os.path.exists(full_path):
                issues.append(f'CMakeLists.txt: Referenced source file not found: {src_file}')
    
    # Check pyproject.toml
    pyproject_path = os.path.join(root_dir, 'pyproject.toml')
    if os.path.exists(pyproject_path):
        with open(pyproject_path, 'r') as f:
            pyproject_content = f.read()
            if 'scikit-build-core' not in pyproject_content:
                issues.append('pyproject.toml: Missing scikit-build-core dependency')
    
    return issues

def analyze_project_structure(root_dir):
    """Analyze overall project structure"""
    structure_issues = []
    
    # Expected directories based on README
    expected_dirs = [
        'src/core',
        'src/brain',
        'src/dynamics',
        'src/plasticity',
        'src/development',
        'src/neuromodulation',
        'src/memory',
        'src/prediction',
        'src/cognition',
        'src/sensory',
        'src/motor',
        'src/environment',
        'src/experiments',
        'src/visualization',
        'src/agent',
        'src/world'
    ]
    
    for expected_dir in expected_dirs:
        full_path = os.path.join(root_dir, expected_dir)
        if not os.path.exists(full_path):
            structure_issues.append(f'Missing expected directory: {expected_dir}')
    
    return structure_issues

if __name__ == "__main__":
    root_dir = "/workspace/cf14c76c-4b1f-4d38-9a96-64cd8a400481/sessions/agent_467cb2dc-451d-4d63-a507-c710c6e3509b"
    
    print("=" * 80)
    print("DUPLICATE CLASS DECLARATIONS ANALYSIS")
    print("=" * 80)
    
    duplicates = find_duplicate_classes(root_dir)
    if duplicates:
        print(f"\nFound {len(duplicates)} duplicate class declarations:")
        for class_name, files in sorted(duplicates.items()):
            print(f"\nClass '{class_name}':")
            for file in files:
                print(f"  - {file}")
    else:
        print("\nNo duplicate class declarations found.")
    
    print("\n" + "=" * 80)
    print("TODOs and FIXMEs ANALYSIS")
    print("=" * 80)
    
    todos = find_todos_fixes(root_dir)
    if todos:
        print(f"\nFound {len(todos)} TODO/FIXME-like comments:")
        for issue in todos[:20]:  # Limit to 20 for readability
            print(f"\nFile: {issue['file']}")
            print(f"Context: {issue['context'][:100]}...")
    else:
        print("\nNo TODO/FIXME comments found.")
    
    print("\n" + "=" * 80)
    print("BUILD CONFIGURATION ANALYSIS")
    print("=" * 80)
    
    build_issues = check_build_config(root_dir)
    if build_issues:
        print(f"\nFound {len(build_issues)} build configuration issues:")
        for issue in build_issues:
            print(f"  - {issue}")
    else:
        print("\nNo build configuration issues found.")
    
    print("\n" + "=" * 80)
    print("PROJECT STRUCTURE ANALYSIS")
    print("=" * 80)
    
    structure_issues = analyze_project_structure(root_dir)
    if structure_issues:
        print(f"\nFound {len(structure_issues)} structure issues:")
        for issue in structure_issues:
            print(f"  - {issue}")
    else:
        print("\nNo project structure issues found.")