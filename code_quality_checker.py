#!/usr/bin/env python3
"""
NLM Code Quality and Bug Detection Script

This script analyzes the NLM codebase to identify potential bugs,
performance issues, and areas for improvement.
"""

import os
import re
import sys
from pathlib import Path

class NLMCodeQualityChecker:
    def __init__(self, project_root):
        self.project_root = Path(project_root)
        self.issues = []
        self.warnings = []
        self.suggestions = []
    
    def check_python_files(self):
        """Check Python files for quality issues"""
        python_files = list(self.project_root.rglob("*.py"))
        
        for py_file in python_files:
            self.analyze_python_file(py_file)
    
    def analyze_python_file(self, py_file):
        """Analyze a single Python file"""
        try:
            with open(py_file, 'r', encoding='utf-8') as f:
                content = f.read()
                lines = content.split('\n')
            
            # Check for basic Python anti-patterns
            self.check_basic_patterns(py_file, content, lines)
            self.check_imports(py_file, content)
            self.check_error_handling(py_file, content)
            self.check_documentation(py_file, content)
            
        except Exception as e:
            self.issues.append(f"Error analyzing {py_file}: {e}")
    
    def check_basic_patterns(self, py_file, content, lines):
        """Check for basic Python code quality issues"""
        # Check for long lines (> 100 characters)
        long_lines = []
        for i, line in enumerate(lines, 1):
            if len(line) > 100:
                long_lines.append((i, len(line)))
        
        if long_lines:
            self.warnings.append(
                f"{py_file}: {len(long_lines)} lines exceed 100 characters "
                f"(max: {max(ll[1] for ll in long_lines)} chars)"
            )
        
        # Check for TODO comments
        todo_patterns = [
            r'# TODO:',
            r'# FIXME:',
            r'# XXX:',
            r'# HACK:'
        ]
        
        for pattern in todo_patterns:
            matches = re.finditer(pattern, content)
            for match in matches:
                line_num = content[:match.start()].count('\n') + 1
                self.warnings.append(f"{py_file}:{line_num}: {pattern.strip(':')} comment found")
    
    def check_imports(self, py_file, content):
        """Check import statements"""
        lines = content.split('\n')
        
        # Check for import ordering (standard library first)
        std_imports = []
        third_party_imports = []
        local_imports = []
        
        for line in lines:
            stripped = line.strip()
            if stripped.startswith('import ') and not stripped.startswith('import .'):
                if 'pynlm' not in stripped and 'nlm' not in stripped:
                    third_party_imports.append(line)
                else:
                    local_imports.append(line)
            elif stripped.startswith('from ') and 'import' in stripped:
                if 'pynlm' not in stripped and 'nlm' not in stripped:
                    third_party_imports.append(line)
                else:
                    local_imports.append(line)
        
        # Check for mixed import styles
        mixed_import_patterns = re.findall(r'import.*\n.*from', content)
        if mixed_import_patterns:
            self.warnings.append(f"{py_file}: Mixed import styles detected")
    
    def check_error_handling(self, py_file, content):
        """Check error handling patterns"""
        # Check for bare except statements
        bare_except_pattern = r'except\s*:\s*pass'
        if re.search(bare_except_pattern, content):
            self.warnings.append(f"{py_file}: Bare except statement detected")
        
        # Check for try-except without specific exception
        try_except_pattern = r'try:.*?except.*?:'
        if re.search(try_except_pattern, content, re.DOTALL):
            self.issues.append(f"{py_file}: Generic exception handling detected")
    
    def check_documentation(self, py_file, content):
        """Check documentation quality"""
        # Check for docstring quality
        if '"""' in content or "'''" in content:
            # Extract docstrings
            docstrings = re.findall(r'""".*?"""', content, re.DOTALL)
            docstrings += re.findall(r"'''.*?'''", content, re.DOTALL)
            
            for docstring in docstrings:
                if len(docstring.strip()) < 20:
                    self.warnings.append(f"{py_file}: Potentially insufficient docstring")
    
    def check_cpp_files(self):
        """Check C++ files for quality issues"""
        cpp_files = list(self.project_root.rglob("*.cpp"))
        cpp_files += list(self.project_root.rglob("*.hpp"))
        
        for cpp_file in cpp_files:
            self.analyze_cpp_file(cpp_file)
    
    def analyze_cpp_file(self, cpp_file):
        """Analyze a single C++ file"""
        try:
            with open(cpp_file, 'r', encoding='utf-8') as f:
                content = f.read()
            
            # Check for potential issues
            self.check_cpp_patterns(cpp_file, content)
            self.check_memory_management(cpp_file, content)
            
        except Exception as e:
            self.issues.append(f"Error analyzing {cpp_file}: {e}")
    
    def check_cpp_patterns(self, cpp_file, content):
        """Check for C++ code quality issues"""
        # Check for missing includes
        if '#include' not in content and cpp_file.suffix == '.cpp':
            self.warnings.append(f"{cpp_file}: Missing include guards or namespace")
        
        # Check for long lines
        lines = content.split('\n')
        long_lines = []
        for i, line in enumerate(lines, 1):
            if len(line) > 120:
                long_lines.append((i, len(line)))
        
        if long_lines:
            self.warnings.append(
                f"{cpp_file}: {len(long_lines)} lines exceed 120 characters"
            )
        
        # Check for magic numbers
        magic_numbers = re.findall(r'\b\d+\b', content)
        magic_numbers = [n for n in magic_numbers if len(n) > 2]
        if len(magic_numbers) > 10:
            self.suggestions.append(
                f"{cpp_file}: {len(magic_numbers)} magic numbers found, consider using constants"
            )
    
    def check_memory_management(self, cpp_file, content):
        """Check for potential memory management issues"""
        # Check for raw pointers without smart pointers
        raw_ptr_patterns = [
            r'new\s+\w+\s*\(',
            r'delete\s+\w+\s*\('
        ]
        
        for pattern in raw_ptr_patterns:
            if re.search(pattern, content):
                self.suggestions.append(
                    f"{cpp_file}: Consider using smart pointers for {pattern}"
                )
    
    def run_checks(self):
        """Run all code quality checks"""
        print("Running NLM Code Quality Checks...")
        print("=" * 50)
        
        # Check Python files
        print("Checking Python files...")
        self.check_python_files()
        
        # Check C++ files
        print("Checking C++ files...")
        self.check_cpp_files()
        
        # Report results
        self.report_results()
    
    def report_results(self):
        """Report the results of code quality checks"""
        print(f"\nCode Quality Check Results:")
        print(f"  Total files analyzed: {len(list(self.project_root.rglob('*.py'))) + len(list(self.project_root.rglob('*.cpp'))) + len(list(self.project_root.rglob('*.hpp')))}")
        
        if self.issues:
            print(f"\nISSUES ({len(self.issues)}):")
            for issue in self.issues:
                print(f"  ❌ {issue}")
        
        if self.warnings:
            print(f"\nWARNINGS ({len(self.warnings)}):")
            for warning in self.warnings:
                print(f"  ⚠️  {warning}")
        
        if self.suggestions:
            print(f"\nSUGGESTIONS ({len(self.suggestions)}):")
            for suggestion in self.suggestions:
                print(f"  💡 {suggestion}")
        
        if not self.issues and not self.warnings and not self.suggestions:
            print("\n✅ No issues found. Code quality is good!")
        
        return len(self.issues) == 0

def main():
    """Main function"""
    project_root = Path(__file__).parent
    
    checker = NLMCodeQualityChecker(project_root)
    success = checker.run_checks()
    
    if not success:
        print(f"\nCode quality checks completed with issues found.")
        print("Review the issues and warnings above for potential improvements.")
        sys.exit(1)
    else:
        print(f"\n✅ Code quality checks completed successfully.")
        sys.exit(0)

if __name__ == "__main__":
    main()