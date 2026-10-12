//!/usr/bin/env python3
"""
Code Quality Analysis and Improvement Plan for NLM Project

This script analyzes the NLM (Neural Learning Machine) project structure
and provides recommendations for improvements based on the code review.
"""

import os
import re
import json
from pathlib import Path
from typing import List, Dict, Any, Tuple

class NLMCodeAnalyzer:
    def __init__(self, project_path: str):
        self.project_path = Path(project_path)
        self.cpp_files = []
        self.total_lines = 0
        self.large_files = []
        self.complexity_metrics = {}
        
    def analyze_project(self) -> Dict[str, Any]:
        """Analyze the entire project structure"""
        print("🔍 Analyzing NLM Project Structure...")
        
        # Find all C++ files
        self.cpp_files = list(self.project_path.rglob("*.cpp"))
        self.cpp_files.extend(list(self.project_path.rglob("*.hpp")))
        
        # Analyze file sizes and complexity
        for file_path in self.cpp_files:
            if file_path.is_file():
                lines = self.count_lines(file_path)
                self.total_lines += lines
                
                if lines > 100:
                    self.large_files.append((str(file_path), lines))
                    
                # Analyze complexity patterns
                complexity = self.analyze_complexity(file_path)
                self.complexity_metrics[str(file_path)] = complexity
        
        return self.generate_report()
    
    def count_lines(self, file_path: Path) -> int:
        """Count lines in a file, excluding comments and empty lines"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Remove comments and count actual code lines
            # Remove single-line comments
            content = re.sub(r'//.*', '', content)
            # Remove multi-line comments
            content = re.sub(r'/\*.*?\*/', '', content, flags=re.DOTALL)
            # Count non-empty lines
            lines = [line for line in content.split('\n') if line.strip()]
            return len(lines)
        except Exception as e:
            print(f"Error reading {file_path}: {e}")
            return 0
    
    def analyze_complexity(self, file_path: Path) -> Dict[str, Any]:
        """Analyze code complexity metrics"""
        try:
            with open(file_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Count control structures
            if_count = len(re.findall(r'\bif\s*\(', content))
            for_count = len(re.findall(r'\bfor\s*\(', content))
            while_count = len(re.findall(r'\bwhile\s*\(', content))
            switch_count = len(re.findall(r'\bswitch\s*\(', content))
            
            # Count function definitions
            function_matches = re.findall(r'\b(?:bool|void|double|float|void|\w+)\s+\w*\s*\(', content)
            function_count = len(function_matches)
            
            # Count template usages
            template_count = len(re.findall(r'template<', content))
            
            return {
                'if_statements': if_count,
                'for_loops': for_count,
                'while_loops': while_count,
                'switch_statements': switch_count,
                'function_count': function_count,
                'template_count': template_count,
                'total_lines': self.count_lines(file_path)
            }
        except Exception as e:
            print(f"Error analyzing {file_path}: {e}")
            return {}
    
    def generate_report(self) -> Dict[str, Any]:
        """Generate comprehensive analysis report"""
        print("\n📊 PROJECT ANALYSIS REPORT")
        print("=" * 50)
        
        # Basic metrics
        print(f"📁 Total C++/Header files: {len(self.cpp_files)}")
        print(f"📄 Total lines of code: {self.total_lines:,}")
        print(f"📊 Average file size: {self.total_lines / max(1, len(self.cpp_files)):.1f} lines")
        
        # Large files report
        print(f"\n⚠️  LARGE FILES (>100 lines):")
        for file_path, lines in sorted(self.large_files, key=lambda x: x[1], reverse=True)[:10]:
            print(f"   {file_path.split('/')[-2:]}")
            print(f"      {lines:,} lines of code")
        
        # Most complex files
        print(f"\n🧩 MOST COMPLEX FILES:")
        sorted_complexity = sorted(self.complexity_metrics.items(), 
                                  key=lambda x: x[1].get('if_statements', 0) + x[1].get('for_loops', 0), 
                                  reverse=True)
        for file_path, metrics in sorted_complexity[:5]:
            if metrics:
                complexity_score = metrics.get('if_statements', 0) + metrics.get('for_loops', 0)
                print(f"   {file_path.split('/')[-2:]}")
                print(f"      Complexity score: {complexity_score}")
                print(f"      Functions: {metrics.get('function_count', 0)}")
        
        return {
            'total_files': len(self.cpp_files),
            'total_lines': self.total_lines,
            'large_files': [(fp, lines) for fp, lines in self.large_files],
            'complexity_metrics': self.complexity_metrics
        }

def create_improvement_plan() -> Dict[str, Any]:
    """Create a comprehensive improvement plan"""
    print("\n🔧 IMPROVEMENT PLAN")
    print("=" * 50)
    
    improvements = [
        {
            'category': 'Code Structure & Organization',
            'improvements': [
                'Refactor Brain.cpp to use smaller, focused classes',
                'Implement proper namespace organization',
                'Create abstract base classes for neural components',
                'Add comprehensive header documentation'
            ]
        },
        {
            'category': 'Configuration System',
            'improvements': [
                'Replace key=value parser with JSON/YAML support',
                'Add type-safe configuration values',
                'Implement configuration validation',
                'Add configuration inheritance and merging'
            ]
        },
        {
            'category': 'Memory Management',
            'improvements': [
                'Replace manual memory management with smart pointers',
                'Implement RAII patterns for all resources',
                'Add deep copy support for checkpointing',
                'Simplify cleanup and reset operations'
            ]
        },
        {
            'category': 'Command Line Interface',
            'improvements': [
                'Add comprehensive CLI options for all systems',
                'Implement configuration file loading/saving',
                'Add profiling and debugging tools',
                'Implement command history and scripting'
            ]
        },
        {
            'category': 'Code Quality & Best Practices',
            'improvements': [
                'Add unit tests for all major components',
                'Implement static analysis and linting',
                'Add code documentation generation',
                'Implement performance profiling tools'
            ]
        },
        {
            'category': 'Advanced Features',
            'improvements': [
                'Add user interface for brain visualization',
                'Implement experiment management system',
                'Add machine learning integration',
                'Implement plugin architecture for extensions'
            ]
        }
    ]
    
    for i, improvement in enumerate(improvements, 1):
        print(f"\n{i}. {improvement['category']}:")
        for imp in improvement['improvements']:
            print(f"   ✅ {imp}")
    
    return improvements

def analyze_specific_files(project_path: str) -> Dict[str, Any]:
    """Analyze specific key files mentioned in the review"""
    print("\n🔬 DETAILED FILE ANALYSIS")
    print("=" * 50)
    
    files_to_analyze = [
        ('src/brain/Brain.cpp', 'Main Brain Implementation'),
        ('src/agent/AgentBrain.cpp', 'Agent Interface'),
        ('src/main.cpp', 'Main Demo'),
        ('src/core/Config/Config.cpp', 'Configuration System'),
        ('src/experiments/Phase6Demo.cpp', 'Phase 6 Integration')
    ]
    
    analysis_results = {}
    
    for file_path, description in files_to_analyze:
        full_path = os.path.join(project_path, file_path)
        if os.path.exists(full_path):
            print(f"\n📖 {description} ({file_path}):")
            
            # Read and analyze the file
            with open(full_path, 'r', encoding='utf-8', errors='ignore') as f:
                content = f.read()
            
            # Basic metrics
            lines = len(content.split('\n'))
            non_empty_lines = len([line for line in content.split('\n') if line.strip()])
            print(f"   📄 Total lines: {lines:,}")
            print(f"   📝 Non-empty lines: {non_empty_lines:,}")
            
            # Class/function count
            class_count = len(re.findall(r'\bclass\s+\w+', content))
            function_count = len(re.findall(r'^\s*(?:bool|void|double|float|\w+)\s+\w*\s*\(', content, re.MULTILINE))
            template_count = len(re.findall(r'template<', content))
            
            print(f"   🏛️  Classes: {class_count}")
            print(f"   🔧 Functions: {function_count}")
            print(f"   📦 Templates: {template_count}")
            
            # Identify patterns needing improvement
            issues = []
            if lines > 500:
                issues.append("Extremely large file - needs refactoring")
            if class_count > 1:
                issues.append("Multiple classes in single file - consider separation")
            if template_count > 3:
                issues.append("Many templates - complex dependencies")
            
            if issues:
                print(f"   ⚠️  Issues identified:")
                for issue in issues:
                    print(f"      • {issue}")
            
            analysis_results[file_path] = {
                'lines': lines,
                'non_empty_lines': non_empty_lines,
                'classes': class_count,
                'functions': function_count,
                'templates': template_count,
                'issues': issues
            }
        else:
            print(f"\n❌ File not found: {file_path}")
            analysis_results[file_path] = {'error': 'File not found'}
    
    return analysis_results

def main():
    """Main analysis function"""
    project_path = "/workspace/app"
    
    print("🚀 NLM Project Code Quality Analysis")
    print("=" * 60)
    
    # Initialize analyzer
    analyzer = NLMCodeAnalyzer(project_path)
    
    # Run analysis
    basic_report = analyzer.analyze_project()
    
    # Create improvement plan
    improvement_plan = create_improvement_plan()
    
    # Analyze specific files
    detailed_analysis = analyze_specific_files(project_path)
    
    # Generate summary
    print("\n🎯 EXECUTIVE SUMMARY")
    print("=" * 50)
    print("✅ Project successfully analyzed")
    print(f"📊 Analysis completed on {len(analyzer.cpp_files)} files")
    print(f"🔧 Improvement plan with {len(improvement_plan)} major categories")
    print(f"📋 Detailed analysis of {len(detailed_analysis)} key files")
    
    # Save analysis results
    results = {
        'basic_report': basic_report,
        'improvement_plan': improvement_plan,
        'detailed_analysis': detailed_analysis,
        'timestamp': str(__import__('datetime').datetime.now())
    }
    
    # Save to JSON file
    with open('/workspace/app/analysis_results.json', 'w') as f:
        json.dump(results, f, indent=2, default=str)
    
    print(f"\n💾 Analysis saved to: /workspace/app/analysis_results.json")
    
    return results

if __name__ == "__main__":
    main()