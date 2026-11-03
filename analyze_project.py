#!/usr/bin/env python3
"""
Project Analysis Script for NLM (熙然)
Analyzes the neural simulation framework to identify areas for improvement
"""

import os
import json
import re
from pathlib import Path
from typing import Dict, List, Set, Tuple
import networkx as nx
from collections import defaultdict

def analyze_project_structure(root_path: Path) -> Dict:
    """Analyze the overall project structure"""
    analysis = {
        'total_files': 0,
        'cpp_files': 0,
        'hpp_files': 0,
        'directories': [],
        'phase_integrations': [],
        'build_files': []
    }
    
    for item in root_path.iterdir():
        analysis['total_files'] += 1
        if item.is_file():
            if item.suffix == '.cpp':
                analysis['cpp_files'] += 1
            elif item.suffix == '.hpp':
                analysis['hpp_files'] += 1
            elif item.suffix == '.cmake' or item.name == 'CMakeLists.txt':
                analysis['build_files'].append(item.name)
        else:
            analysis['directories'].append(item.name)
            
        # Check for phase integration files
        if 'phase' in item.name.lower():
            analysis['phase_integrations'].append(item.name)
    
    return analysis

def analyze_cpp_file(filepath: Path) -> Dict:
    """Analyze a C++ file for code quality metrics"""
    with open(filepath, 'r') as f:
        content = f.read()
    
    analysis = {
        'lines': len(content.splitlines()),
        'classes': len(re.findall(r'class\s+\w+', content)),
        'functions': len(re.findall(r'^\s*(\w+\s+\w+)\s*\([^;]*\)\s*{', content, re.MULTILINE)),
        'includes': len(re.findall(r'#include\s+[<"].*?[>"]', content)),
        'comments': len(re.findall(r'//.*', content)),
        'complexity_issues': []
    }
    
    # Check for potential issues
    if analysis['lines'] > 100:
        analysis['complexity_issues'].append('File too long (>100 lines)')
    
    if analysis['includes'] > 10:
        analysis['complexity_issues'].append('Too many includes (>10)')
    
    if analysis['functions'] > 20:
        analysis['complexity_issues'].append('Too many functions (>20)')
    
    return analysis

def analyze_experiment_system(root_path: Path) -> Dict:
    """Analyze the experiment system for improvements"""
    experiment_dir = root_path / 'src' / 'experiments'
    
    if not experiment_dir.exists():
        return {'error': 'Experiment directory not found'}
    
    analysis = {
        'experiment_types': [],
        'integration_points': [],
        'config_usage': defaultdict(int),
        'repetitive_patterns': []
    }
    
    for exp_file in experiment_dir.glob('*.cpp'):
        filename = exp_file.name
        
        # Extract phase name from filename
        phase_match = re.match(r'(Phase\d+)', filename)
        if phase_match:
            analysis['experiment_types'].append(phase_match.group(1))
        
        with open(exp_file, 'r') as f:
            content = f.read()
        
        # Check for repetitive patterns
        if 'std::make_shared<Config>()' in content:
            analysis['config_usage']['boilerplate'] += 1
        
        if 'brain->initialize()' in content and 'brain->step(' in content:
            analysis['repetitive_patterns'].append(filename)
        
        # Check for integration points
        if 'verifyIntegration()' in content or 'testIntegration()' in content:
            analysis['integration_points'].append(filename)
    
    return analysis

def analyze_configuration_system(root_path: Path) -> Dict:
    """Analyze the configuration system"""
    config_files = list(root_path.glob('**/*config*.hpp')) + list(root_path.glob('**/*config*.cpp'))
    
    analysis = {
        'config_classes': [],
        'key_value_pairs': [],
        'configuration_patterns': []
    }
    
    for config_file in config_files:
        with open(config_file, 'r') as f:
            content = f.read()
        
        # Extract class names
        classes = re.findall(r'class\s+(\w+Config|\w+Config\s*{)', content)
        analysis['config_classes'].extend(classes)
        
        # Look for key-value configurations
        key_values = re.findall(r'set\("([^"]+)",\s*[^)]+\)', content)
        analysis['key_value_pairs'].extend(key_values)
    
    return analysis

def find_bottlenecks(root_path: Path) -> List[Dict]:
    """Identify potential performance bottlenecks"""
    bottlenecks = []
    
    # Check for files that might be problematic
    problematic_files = []
    
    for cpp_file in root_path.glob('**/*.cpp'):
        with open(cpp_file, 'r') as f:
            content = f.read()
        
        # Check for known bottlenecks
        if 'for.*step.*brain->step' in content and content.count('for.*step.*brain->step') > 5:
            problematic_files.append(cpp_file)
        
        # Check for nested loops
        loop_count = len(re.findall(r'for\s*\(', content))
        if loop_count > 3:
            bottlenecks.append({
                'file': str(cpp_file),
                'issue': f'Multiple nested loops ({loop_count})',
                'severity': 'medium'
            })
    
    return bottlenecks

def analyze_memory_management(root_path: Path) -> Dict:
    """Analyze memory management patterns"""
    analysis = {
        'smart_ptr_usage': defaultdict(int),
        'raw_pointer_usage': [],
        'potential_leaks': []
    }
    
    for cpp_file in root_path.glob('**/*.cpp'):
        with open(cpp_file, 'r') as f:
            content = f.read()
        
        # Check smart pointer usage
        if 'std::shared_ptr' in content:
            analysis['smart_ptr_usage']['shared_ptr'] += 1
        if 'std::unique_ptr' in content:
            analysis['smart_ptr_usage']['unique_ptr'] += 1
        
        # Check for raw pointers that might be problematic
        raw_ptr_matches = re.findall(r'(\w+)\s*\*\s*(\w+)\s*=', content)
        for match in raw_ptr_matches:
            if match[0] not in ['const', 'auto', 'static']:
                analysis['raw_pointer_usage'].append(f'{match[0]} {match[1]}')
    
    return analysis

def generate_recommendations(analysis: Dict) -> List[str]:
    """Generate improvement recommendations"""
    recommendations = []
    
    # Based on analysis
    if analysis['experiment_analysis']['repetitive_patterns']:
        recommendations.append(
            "Create a generic experiment framework to reduce boilerplate code in Phase 3-6 experiments"
        )
    
    if analysis['configuration_analysis']['config_classes']:
        recommendations.append(
            "Standardize configuration classes with common interfaces and inheritance hierarchy"
        )
    
    if analysis['memory_analysis']['raw_pointer_usage']:
        recommendations.append(
            "Replace identified raw pointers with smart pointers where appropriate"
        )
    
    recommendations.extend([
        "Add comprehensive error handling and validation",
        "Implement unit tests for all major components",
        "Add performance profiling and optimization opportunities",
        "Improve documentation and API consistency",
        "Add command-line interface for easy configuration",
        "Implement logging and monitoring for production use"
    ])
    
    return recommendations

def main():
    """Main analysis function"""
    print("NLM Project Analysis Script")
    print("=" * 50)
    
    # Set up paths
    project_root = Path('/workspace/app')
    
    # Run analyses
    print("\n1. Project Structure Analysis:")
    structure = analyze_project_structure(project_root)
    for key, value in structure.items():
        if isinstance(value, list):
            print(f"  {key}: {len(value)} items")
        else:
            print(f"  {key}: {value}")
    
    print("\n2. Configuration System Analysis:")
    config_analysis = analyze_configuration_system(project_root)
    print(f"  Configuration classes: {len(config_analysis['config_classes'])}")
    print(f"  Configuration patterns: {len(config_analysis['configuration_patterns'])}")
    
    print("\n3. Experiment System Analysis:")
    experiment_analysis = analyze_experiment_system(project_root)
    print(f"  Experiment types: {len(experiment_analysis['experiment_types'])}")
    print(f"  Integration points: {len(experiment_analysis['integration_points'])}")
    print(f"  Repetitive patterns: {len(experiment_analysis['repetitive_patterns'])}")
    
    print("\n4. Performance Bottleneck Analysis:")
    bottlenecks = find_bottlenecks(project_root)
    print(f"  Bottlenecks identified: {len(bottlenecks)}")
    for bottleneck in bottlenecks[:3]:  # Show top 3
        print(f"  - {bottleneck['file']}: {bottleneck['issue']}")
    
    print("\n5. Memory Management Analysis:")
    memory_analysis = analyze_memory_management(project_root)
    print(f"  Smart pointer usage: {dict(memory_analysis['smart_ptr_usage'])}")
    print(f"  Raw pointer patterns: {len(memory_analysis['raw_pointer_usage'])}")
    
    print("\n6. Improvement Recommendations:")
    full_analysis = {
        'experiment_analysis': experiment_analysis,
        'configuration_analysis': config_analysis,
        'memory_analysis': memory_analysis
    }
    recommendations = generate_recommendations(full_analysis)
    for i, rec in enumerate(recommendations, 1):
        print(f"  {i}. {rec}")
    
    print("\nAnalysis complete!")

if __name__ == '__main__':
    main()
