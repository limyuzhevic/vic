#!/usr/bin/env python3
"""
Code Analysis Report for NLM (Neural Learning Machine)

This script analyzes the NLM codebase for potential improvements,
bugs, and architectural issues.

Author: Code Analysis Assistant
Date: 2026-10-15
"""

import os
import re
import json
from pathlib import Path
from typing import Dict, List, Set, Tuple

class NLMCodeAnalyzer:
    def __init__(self, project_path: str):
        self.project_path = Path(project_path)
        self.src_path = self.project_path / "src"
        self.python_path = self.project_path / "python"
        
        # Code metrics
        self.total_files = 0
        self.total_lines = 0
        self.cpp_files = 0
        self.headers = 0
        self.python_files = 0
        
        # Issues found
        self.issues = []
        self.bugs = []
        self.improvements = []
        self.architectural_issues = []
        
    def analyze_all(self):
        """Run complete analysis"""
        print("=== NLM Code Analysis ===\n")
        
        # File analysis
        self.analyze_files()
        
        # Configuration system analysis
        self.analyze_config_system()
        
        # Python bindings analysis
        self.analyze_python_bindings()
        
        # Architecture analysis
        self.analyze_architecture()
        
        # Code quality analysis
        self.analyze_code_quality()
        
        # Generate report
        self.generate_report()
        
    def analyze_files(self):
        """Analyze file structure and count files"""
        print("1. Analyzing file structure...")
        
        for root, dirs, files in os.walk(self.src_path):
            for file in files:
                if file.endswith(".cpp") or file.endswith(".cc"):
                    self.cpp_files += 1
                elif file.endswith(".hpp") or file.endswith(".h"):
                    self.headers += 1
                    
        for root, dirs, files in os.walk(self.python_path):
            for file in files:
                if file.endswith(".cpp"):
                    self.cpp_files += 1
                
        self.total_files = self.cpp_files + self.headers
        
        print(f"   - C++ files: {self.cpp_files}")
        print(f"   - Header files: {self.headers}")
        print(f"   - Total files: {self.total_files}\n")
        
    def analyze_config_system(self):
        """Analyze configuration system for issues"""
        print("2. Analyzing configuration system...")
        
        config_file = self.src_path / "core" / "Config" / "Config.cpp"
        
        if config_file.exists():
            content = config_file.read_text()
            
            # Check for TODO comments
            todo_matches = re.findall(r'// TODO', content)
            if todo_matches:
                self.issues.append({
                    'file': str(config_file),
                    'line': 'Various',
                    'severity': 'Medium',
                    'description': f"Found {len(todo_matches)} TODO comments",
                    'recommendation': "Complete implementation of TODO items"
                })
            
            # Check saveToFile implementation
            if "PLACEHOLDER_VALUE" in content:
                self.bugs.append({
                    'file': str(config_file),
                    'line': '89',
                    'severity': 'High',
                    'description': "saveToFile writes placeholder values instead of actual data",
                    'impact': "Configuration cannot be saved to file with actual values",
                    'fix': "Implement proper serialization of ConfigValue types"
                })
            
            # Check for hardcoded config access patterns
            if "getOr<int>(\"random_seed\"" in content:
                self.improvements.append({
                    'file': str(config_file),
                    'description': "Hardcoded config key 'random_seed' - should be configurable constant",
                    'recommendation': "Define configuration keys as constants or enum"
                })
        
        print(f"   - Found {len(self.issues)} issues in config system")
        print(f"   - Found {len(self.bugs)} bugs in config system\n")
        
    def analyze_python_bindings(self):
        """Analyze Python bindings for completeness"""
        print("3. Analyzing Python bindings...")
        
        bindings_file = self.project_path / "python" / "bindings.cpp"
        
        if bindings_file.exists():
            content = bindings_file.read_text()
            
            # Check for pybind11 specific patterns
            pybind_patterns = {
                'def\.py\(\)': r'\.def\(py::arg\(\w+\)',
                'missing __repr__': r'def\(\) \{\s*return "<[^>]+>".*\}',
                'missing __str__': r'def\(\) \{\s*return .*\}',
                'missing arithmetic operators': r'operator[+\-*/%]\(\)',
                'missing comparison operators': r'operator==\(\)|operator!=\(\)',
                'missing iterators': r'\.__iter__\(\)|begin\(\)|end\(\)',
                'missing len': r'\.size\(\)'
            }
            
            # Check for common Python interface patterns
            class_patterns = [
                r'py::class_<\w+>',
                r'py::enum_<\w+>',
                r'm\.def\("\w+",'
            ]
            
            classes_found = len(re.findall(r'py::class_<\w+>', content))
            enums_found = len(re.findall(r'py::enum_<\w+>', content))
            
            print(f"   - Found {classes_found} C++ classes exposed to Python")
            print(f"   - Found {enums_found} enums exposed to Python")
            
            # Check for Pythonic interface completeness
            if '\.def\("__len__"' not in content:
                self.improvements.append({
                    'file': str(bindings_file),
                    'description': "Missing Python `__len__` method for size() compatibility",
                    'recommendation': "Add `__len__` method to Python classes for 'len(obj)' support"
                })
            
            # Check for key error handling
            if 'py::register_exception' not in content:
                self.issues.append({
                    'file': str(bindings_file),
                    'severity': 'Low',
                    'description': "Missing custom exception handling for Python bindings",
                    'recommendation': "Register custom exceptions for better Python integration"
                })
        
        print(f"   - Found {len(self.improvements)} improvements for Python bindings\n")
        
    def analyze_architecture(self):
        """Analyze architectural patterns"""
        print("4. Analyzing architecture...")
        
        # Check for tight coupling indicators
        brain_file = self.src_path / "brain" / "Brain.cpp"
        if brain_file.exists():
            content = brain_file.read_text()
            
            # Look for hardcoded dependencies
            if 'std::make_unique<Brain>(config)' in content:
                self.architectural_issues.append({
                    'file': str(brain_file),
                    'pattern': 'Direct brain creation',
                    'severity': 'Medium',
                    'description': "Brain class directly creates its components instead of using factory pattern",
                    'recommendation': "Use dependency injection or factory pattern for better testability"
                })
            
            # Check for hardcoded configuration keys
            config_keys = [
                'neuron_count', 'region_count', 'connection_probability',
                'stdp_ltp_weight', 'stdp_ltd_weight', 'stdp_tau',
                'synaptogenesis_rate', 'pruning_rate', 'simulation_timestep'
            ]
            
            found_keys = 0
            for key in config_keys:
                if f'"{key}"' in content:
                    found_keys += 1
            
            if found_keys > 5:
                self.architectural_issues.append({
                    'file': str(brain_file),
                    'pattern': 'Hardcoded configuration access',
                    'severity': 'High',
                    'description': f"Found {found_keys} hardcoded configuration key accesses",
                    'recommendation': "Extract configuration keys to constants or configuration file"
                })
        
        print(f"   - Found {len(self.architectural_issues)} architectural issues\n")
        
    def analyze_code_quality(self):
        """Analyze code quality metrics"""
        print("5. Analyzing code quality...")
        
        # Analyze Brain.cpp for method lengths
        brain_file = self.src_path / "brain" / "Brain.cpp"
        if brain_file.exists():
            content = brain_file.read_text()
            
            # Count method definitions
            method_pattern = r'\w+\([^)]*\)\s*{'
            methods = re.findall(method_pattern, content)
            
            # Look for very long methods (potential code smell)
            lines = content.split('\n')
            long_methods = []
            current_method = []
            in_method = False
            method_start_line = 0
            
            for i, line in enumerate(lines):
                stripped = line.strip()
                if stripped.startswith('void ') or stripped.startswith('bool '):
                    if in_method and current_method:
                        if len(current_method) > 100:  # More than 100 lines
                            long_methods.append((method_start_line, len(current_method)))
                    
                    in_method = True
                    method_start_line = i + 1
                    current_method = [line]
                elif in_method and (stripped.endswith('{') or stripped.startswith('{')):
                    current_method.append(line)
                elif in_method and stripped == '}':
                    current_method.append(line)
                    if len(current_method) > 100:
                        long_methods.append((method_start_line, len(current_method)))
                    in_method = False
                    current_method = []
            
            if long_methods:
                for start_line, line_count in long_methods[:3]:  # Show first 3
                    self.issues.append({
                        'file': str(brain_file),
                        'line': str(start_line),
                        'severity': 'Medium',
                        'description': f"Method starting at line {start_line} has {line_count} lines",
                        'recommendation': "Break down large methods into smaller, focused methods"
                    })
        
        # Check for magic numbers in source code
        self.check_magic_numbers()
        
        print(f"   - Found {len(self.issues)} code quality issues\n")
        
    def check_magic_numbers(self):
        """Check for magic numbers that should be constants"""
        print("   Checking for magic numbers...")
        
        magic_patterns = [
            r'\*\* \d+',  # Exponents
            r'\+\s*\d+',  # Additions
            r'-\s*\d+',  # Subtractions
            r'\* \d+',   # Multiplications
            r'/ \d+',    # Divisions
        ]
        
        for root, dirs, files in os.walk(self.src_path):
            for file in files:
                if file.endswith(('.cpp', '.hpp')):
                    filepath = os.path.join(root, file)
                    content = open(filepath, 'r').read_text()
                    
                    # Look for specific magic numbers in known contexts
                    magic_numbers = {
                        '0.01f': 'Small learning rate',
                        '0.1f': 'Small weight', 
                        '1.0f': 'Default value',
                        '2.0f': 'Scaling factor',
                        '3.14f': 'PI approximation',
                        'M_PI': 'PI constant'
                    }
                    
                    for num, description in magic_numbers.items():
                        if num in content and f'// {num}' not in content:
                            # Check if it's in a comment first
                            lines = content.split('\n')
                            for i, line in enumerate(lines):
                                if num in line and not line.strip().startswith('//'):
                                    self.improvements.append({
                                        'file': filepath,
                                        'line': str(i + 1),
                                        'description': f"Magic number {num} ({description})",
                                        'recommendation': f"Replace with named constant (e.g., {description.upper()}_CONSTANT)"
                                    })
                                    break
    
    def generate_report(self):
        """Generate comprehensive report"""
        print("=== COMPREHENSIVE ANALYSIS REPORT ===\n")
        
        # Summary
        print("PROJECT SUMMARY:")
        print(f"- Project: NLM (Neural Learning Machine)")
        print(f"- Location: {self.project_path}")
        print(f"- Total source files: {self.total_files}")
        print(f"- C++ implementation files: {self.cpp_files}")
        print(f"- Header files: {self.headers}")
        print()
        
        # Issues by category
        print("ISSUES FOUND:")
        
        if self.bugs:
            print(f"\n🚨 CRITICAL BUGS ({len(self.bugs)}):")
            for i, bug in enumerate(self.bugs, 1):
                print(f"\n{i}. {bug['description']}")
                print(f"   File: {bug['file']}")
                print(f"   Line: {bug.get('line', 'Unknown')}")
                print(f"   Impact: {bug.get('impact', 'N/A')}")
                print(f"   Fix: {bug.get('fix', 'N/A')}")
        
        if self.issues:
            print(f"\n⚠️  ISSUES ({len(self.issues)}):")
            for i, issue in enumerate(self.issues, 1):
                print(f"\n{i}. {issue['description']}")
                print(f"   File: {issue['file']}")
                print(f"   Severity: {issue.get('severity', 'Medium')}")
                print(f"   Recommendation: {issue.get('recommendation', 'N/A')}")
        
        if self.improvements:
            print(f"\n✨ IMPROVEMENTS ({len(self.improvements)}):")
            for i, improvement in enumerate(self.improvements, 1):
                print(f"\n{i}. {improvement['description']}")
                print(f"   File: {improvement['file']}")
                if 'line' in improvement:
                    print(f"   Line: {improvement['line']}")
                print(f"   Recommendation: {improvement.get('recommendation', 'N/A')}")
        
        if self.architectural_issues:
            print(f"\n🏗️  ARCHITECTURAL ISSUES ({len(self.architectural_issues)}):")
            for i, issue in enumerate(self.architectural_issues, 1):
                print(f"\n{i}. {issue['description']}")
                print(f"   File: {issue['file']}")
                print(f"   Severity: {issue.get('severity', 'Medium')}")
                print(f"   Recommendation: {issue.get('recommendation', 'N/A')}")
        
        # Top priorities
        print("\n" + "="*60)
        print("TOP PRIORITIES FOR IMPROVEMENT:")
        print("="*60)
        
        priorities = []
        
        # Bug fixes first
        if self.bugs:
            priorities.append(("FIX CONFIG BUGS", "High", "Configuration system has critical save/load bugs"))
        
        if any(issue.get('severity') == 'High' for issue in self.issues):
            priorities.append(("ADDRESS CODE ISSUES", "High", "Code quality issues detected"))
        
        priorities.append(("ADD PYTHON INTERFACES", "Medium", "Complete Python API with proper error handling"))
        priorities.append(("IMPROVE ARCHITECTURE", "Medium", "Reduce coupling and improve modularity"))
        priorities.append(("ADD CONSTANTS", "Low", "Replace magic numbers with named constants"))
        
        for i, (title, priority, description) in enumerate(priorities, 1):
            print(f"{i}. [{priority}] {title}")
            print(f"   {description}")
        
        # Create JSON report
        report = {
            'summary': {
                'project': 'NLM',
                'total_files': self.total_files,
                'cpp_files': self.cpp_files,
                'headers': self.headers,
                'bugs': len(self.bugs),
                'issues': len(self.issues),
                'improvements': len(self.improvements),
                'architectural_issues': len(self.architectural_issues)
            },
            'bugs': self.bugs,
            'issues': self.issues,
            'improvements': self.improvements,
            'architectural_issues': self.architectural_issues,
            'priorities': priorities
        }
        
        # Save report
        report_file = self.project_path / "code_analysis_report.json"
        with open(report_file, 'w') as f:
            json.dump(report, f, indent=2)
        
        print(f"\n📄 Detailed report saved to: {report_file}")
        
        return report

if __name__ == "__main__":
    analyzer = NLMCodeAnalyzer("/workspace/app")
    analyzer.analyze_all()