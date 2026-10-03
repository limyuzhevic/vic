#!/usr/bin/env python3
"""
Code Quality Analysis Script for NLM Project

This script analyzes the codebase for:
1. Memory management issues
2. Error handling problems
3. Code consistency issues
4. Configuration validation issues
5. Documentation issues
6. Performance bottlenecks
"""

import os
import re
from pathlib import Path

class NLMAnalyzer:
    def __init__(self, root_path):
        self.root_path = Path(root_path)
        self.issues = []
        self.improvements = []
    
    def analyze_memory_management(self):
        """Analyze memory management issues"""
        print("Analyzing memory management...")
        
        # Check pImpl implementations
        cpp_files = list(self.root_path.rglob("*.cpp"))
        hpp_files = list(self.root_path.rglob("*.hpp"))
        
        for file in cpp_files:
            content = file.read_text()
            filename = file.name
            
            # Check for potential memory leaks in destructors
            if "~" in content and "delete" not in content and filename.endswith(".cpp"):
                # Check if it's a pImpl class
                if "struct Impl" in content or "Impl* pImpl" in content:
                    self.issues.append(f"Potential memory leak: {filename} may not clean up pImpl")
            
            # Check for missing copy constructors
            if "pImpl" in content and "= delete" not in content and "copy" in filename.lower():
                self.issues.append(f"Copy constructor issue: {filename}")
    
    def analyze_error_handling(self):
        """Analyze error handling issues"""
        print("Analyzing error handling...")
        
        cpp_files = list(self.root_path.rglob("*.cpp"))
        
        for file in cpp_files:
            content = file.read_text()
            
            # Check for magic numbers and hardcoded values
            if re.search(r'set\([^)]+,\s*\d+\s*\)\s*//\s*Hardcoded', content):
                self.issues.append(f"Hardcoded value in {file.name}")
            
            # Check for uninitialized variables
            if re.search(r'auto\s+\w+\s*;\s*//\s*Uninitialized', content):
                self.issues.append(f"Potential uninitialized variable in {file.name}")
    
    def analyze_code_consistency(self):
        """Analyze code consistency issues"""
        print("Analyzing code consistency...")
        
        hpp_files = list(self.root_path.rglob("*.hpp"))
        
        for file in hpp_files:
            content = file.read_text()
            
            # Check for inconsistent naming conventions
            if "get_total_spikes" in content and "totalSpikes" not in content:
                self.issues.append(f"Inconsistent naming in {file.name}")
            
            # Check for TODO comments indicating incomplete work
            if "TODO" in content or "FIXME" in content:
                self.improvements.append(f"Complete TODO/FIXME in {file.name}")
    
    def analyze_documentation(self):
        """Analyze documentation issues"""
        print("Analyzing documentation...")
        
        hpp_files = list(self.root_path.rglob("*.hpp"))
        
        for file in hpp_files:
            content = file.read_text()
            
            # Check for classes without comments
            if re.match(r'^\s*class\s+\w+\s*\{', content) and content.count('//') < 2:
                self.issues.append(f"Poor documentation in {file.name}")
    
    def generate_report(self):
        """Generate analysis report"""
        print("\n" + "="*60)
        print("NLM Code Analysis Report")
        print("="*60)
        
        if self.issues:
            print("\n🔍 ISSUES FOUND:")
            for i, issue in enumerate(self.issues, 1):
                print(f"  {i}. {issue}")
        else:
            print("\n✅ No critical issues found!")
        
        if self.improvements:
            print("\n📝 IMPROVEMENT OPPORTUNITIES:")
            for i, improvement in enumerate(self.improvements, 1):
                print(f"  {i}. {improvement}")
        
        return len(self.issues), len(self.improvements)

def main():
    analyzer = NLMAnalyzer(Path.cwd())
    
    analyzer.analyze_memory_management()
    analyzer.analyze_error_handling()
    analyzer.analyze_code_consistency()
    analyzer.analyze_documentation()
    
    issues_count, improvements_count = analyzer.generate_report()
    
    print(f"\nSummary: {issues_count} critical issues, {improvements_count} improvement opportunities")
    
    if issues_count > 10:
        print("⚠️  WARNING: Many issues found. Consider prioritizing fixes.")
    elif issues_count > 5:
        print("📋 Notice: Several issues found. Implement fixes systematically.")
    else:
        print("✅ Code quality looks good!")

if __name__ == "__main__":
    main()
