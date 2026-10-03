#!/usr/bin/env python3
"""
NLM Test Coverage - Test coverage reporting tool for the Neural Learning Machine system.

This script provides comprehensive test coverage reporting with support for:
- Source code coverage analysis
- Test coverage metrics
- Coverage recommendations
- Coverage gap analysis
- Coverage trend tracking
"""

import os
import sys
import argparse
from datetime import datetime
from pathlib import Path
from typing import List, Dict, Any, Optional, Tuple
import json
import statistics

class NLMTestCoverage:
    def __init__(self, source_dir: str = "", test_results: str = ""):
        self.source_dir = source_dir
        self.test_results = test_results
        self.source_files = []
        self.coverage_data = {}
        self.test_results_data = []
        
    def load_source_files(self, source_dir: str = ""):
        """Load source code files for coverage analysis."""
        if source_dir:
            self.source_dir = source_dir
        
        if not os.path.exists(self.source_dir):
            print(f"Source directory not found: {self.source_dir}")
            return False
        
        # Find C++ source files
        for root, dirs, files in os.walk(self.source_dir):
            # Skip common exclusion directories
            dirs[:] = [d for d in dirs if not d.startswith('.') and d not in ['build', 'cmake-build', '__pycache__']]
            
            for file in files:
                if file.endswith(('.cpp', '.cxx', '.cc', '.c')):
                    file_path = os.path.join(root, file)
                    self.source_files.append(file_path)
        
        print(f"Found {len(self.source_files)} source files for coverage analysis")
        return True
    
    def load_test_results(self, test_results: str = ""):
        """Load test results for coverage analysis."""
        if test_results:
            self.test_results = test_results
        
        if not self.test_results:
            # Find the most recent results file
            results_dir = Path(__file__).parent / "results"
            if not results_dir.exists():
                results_dir = Path(__file__).parent
            
            result_files = list(results_dir.glob("test_results_*.json"))
            if result_files:
                # Sort by modification time, newest first
                result_files.sort(key=os.path.getmtime, reverse=True)
                self.test_results = str(result_files[0])
            else:
                print("No results files found")
                return False
        
        if not os.path.exists(self.test_results):
            print(f"Results file not found: {self.test_results}")
            return False
        
        with open(self.test_results, 'r') as f:
            self.test_results_data = json.load(f)
        
        print(f"Loaded test results from: {self.test_results}")
        return True
    
    def analyze_coverage(self):
        """Analyze test coverage for source files."""
        print("\n=== Test Coverage Analysis ===")
        
        if not self.source_files:
            print("No source files to analyze")
            return
        
        # For each source file, calculate basic coverage metrics
        self.coverage_data = {}
        
        for file_path in self.source_files:
            file_coverage = self.calculate_file_coverage(file_path)
            if file_coverage:
                self.coverage_data[file_path] = file_coverage
        
        # Generate summary statistics
        self.generate_coverage_summary()
        
        # Identify high-risk files
        self.identify_high_risk_files()
        
        # Generate coverage recommendations
        self.generate_coverage_recommendations()
    
    def calculate_file_coverage(self, file_path: str) -> Optional[Dict[str, Any]]:
        """Calculate coverage metrics for a single source file."""
        try:
            with open(file_path, 'r') as f:
                content = f.read()
            
            # Extract function definitions (simplified)
            functions = self.extract_functions(content)
            
            # Extract test calls (simplified)
            test_calls = self.extract_test_calls(content)
            
            # Calculate coverage metrics
            total_functions = len(functions)
            covered_functions = self.count_covered_functions(functions, test_calls)
            coverage_percentage = (covered_functions / total_functions * 100) if total_functions > 0 else 0
            
            return {
                'file_path': file_path,
                'total_functions': total_functions,
                'covered_functions': covered_functions,
                'coverage_percentage': coverage_percentage,
                'test_calls': test_calls,
                'functions': functions
            }
            
        except Exception as e:
            print(f"Error analyzing file {file_path}: {e}")
            return None
    
    def extract_functions(self, content: str) -> List[str]:
        """Extract function definitions from C++ source code."""
        import re
        
        # Simple regex-based function extraction
        # This is a simplified version - in production, use a proper C++ parser
        function_pattern = r'(?:\w+::)?\s*\w+\s+\w+\s*\([^)]*\)\s*(?:const\s*)?(?:noexcept\s*)?(?:=\s*0)?\s*{'
        
        functions = re.findall(function_pattern, content)
        return functions
    
    def extract_test_calls(self, content: str) -> List[str]:
        """Extract test function calls from source code."""
        import re
        
        # Extract function calls that might be test calls
        # This is a simplified version
        test_call_pattern = r'test[A-Za-z_]*\s*\([^)]*\)'
        
        test_calls = re.findall(test_call_pattern, content)
        return test_calls
    
    def count_covered_functions(self, functions: List[str], test_calls: List[str]) -> int:
        """Count how many functions are covered by test calls."""
        if not test_calls:
            return 0
        
        # Simplified coverage calculation
        # In a real implementation, this would map test calls to functions
        covered = 0
        for func in functions:
            # Extract function name (last part before parentheses)
            func_name = func.split('::')[-1].split('(')[0]
            
            # Check if there's a test call that might cover this function
            # This is a very simplified version
            if any(func_name.lower() in call.lower() for call in test_calls):
                covered += 1
        
        return covered
    
    def generate_coverage_summary(self):
        """Generate summary statistics for coverage analysis."""
        if not self.coverage_data:
            print("No coverage data available")
            return
        
        total_files = len(self.coverage_data)
        total_functions = sum(data['total_functions'] for data in self.coverage_data.values())
        total_covered = sum(data['covered_functions'] for data in self.coverage_data.values())
        
        if total_files > 0:
            overall_coverage = total_covered / total_functions * 100 if total_functions > 0 else 0
        else:
            overall_coverage = 0
        
        print(f"Total Files Analyzed: {total_files}")
        print(f"Total Functions: {total_functions}")
        print(f"Total Covered Functions: {total_covered}")
        print(f"Overall Coverage: {overall_coverage:.1f}%")
        
        # Calculate average coverage per file
        if total_files > 0:
            avg_coverage_per_file = overall_coverage
            print(f"Average Coverage per File: {avg_coverage_per_file:.1f}%")
    
    def identify_high_risk_files(self):
        """Identify files with high coverage risk (low coverage)."""
        print("\n=== High-Risk Files (Low Coverage) ===")
        
        if not self.coverage_data:
            print("No coverage data available")
            return
        
        # Sort files by coverage percentage (ascending)
        sorted_files = sorted(self.coverage_data.items(), 
                            key=lambda x: x[1]['coverage_percentage'])
        
        # Identify files with very low coverage (< 50%)
        low_coverage_files = [(path, data) for path, data in sorted_files 
                           if data['coverage_percentage'] < 50]
        
        if low_coverage_files:
            print(f"Files with coverage < 50%: {len(low_coverage_files)}")
            for file_path, data in low_coverage_files[:10]:  # Show top 10
                file_name = os.path.basename(file_path)
                print(f"  {file_name}: {data['coverage_percentage']:.1f}% "
                      f"({data['covered_functions']}/{data['total_functions']} functions)")
        else:
            print("No files with very low coverage (< 50%)")
        
        # Identify files with very high coverage (> 90%)
        high_coverage_files = [(path, data) for path, data in sorted_files 
                            if data['coverage_percentage'] > 90]
        
        if high_coverage_files:
            print(f"\nFiles with coverage > 90%: {len(high_coverage_files)}")
            for file_path, data in high_coverage_files[:10]:  # Show top 10
                file_name = os.path.basename(file_path)
                print(f"  {file_name}: {data['coverage_percentage']:.1f}%")
    
    def generate_coverage_recommendations(self):
        """Generate coverage improvement recommendations."""
        print("\n=== Coverage Improvement Recommendations ===")
        
        if not self.coverage_data:
            print("No coverage data available to generate recommendations")
            return
        
        # Recommendations based on coverage analysis
        print("1. Focus on files with low coverage (< 50%)")
        print("2. Add unit tests for critical utility functions")
        print("3. Implement integration tests for complex components")
        print("4. Consider adding parameterized tests for multiple scenarios")
        print("5. Implement edge case testing for boundary conditions")
        print("6. Add performance and stress tests for critical paths")
        print("7. Implement cross-component integration tests")
        
        # File-specific recommendations
        print("\nFile-specific recommendations:")
        for file_path, data in self.coverage_data.items():
            if data['coverage_percentage'] < 50:
                file_name = os.path.basename(file_path)
                print(f"  - Review and add tests for: {file_name}")
    
    def generate_coverage_report(self, report_file: str = ""):
        """Generate comprehensive coverage report."""
        print("\n=== Generating Coverage Report ===")
        
        if not report_file:
            report_file = f"coverage_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        
        report_data = {
            'timestamp': datetime.now().isoformat(),
            'source_directory': self.source_dir,
            'test_results_file': self.test_results,
            'summary': self.generate_summary_data(),
            'file_coverage': self.coverage_data,
            'recommendations': self.generate_recommendations_data()
        }
        
        with open(report_file, 'w') as f:
            json.dump(report_data, f, indent=2)
        
        print(f"Coverage report generated: {report_file}")
    
    def generate_summary_data(self) -> Dict[str, Any]:
        """Generate summary data for the report."""
        if not self.coverage_data:
            return {
                'total_files': 0,
                'total_functions': 0,
                'total_covered': 0,
                'overall_coverage': 0
            }
        
        total_files = len(self.coverage_data)
        total_functions = sum(data['total_functions'] for data in self.coverage_data.values())
        total_covered = sum(data['covered_functions'] for data in self.coverage_data.values())
        
        overall_coverage = total_covered / total_functions * 100 if total_functions > 0 else 0
        
        return {
            'total_files': total_files,
            'total_functions': total_functions,
            'total_covered': total_covered,
            'overall_coverage': overall_coverage
        }
    
    def generate_recommendations_data(self) -> List[str]:
        """Generate recommendations for the report."""
        recommendations = [
            "Focus on files with coverage below 50% to improve overall coverage",
            "Add unit tests for critical utility and helper functions",
            "Implement integration tests for cross-component interactions",
            "Consider adding parameterized tests for multiple test scenarios",
            "Implement edge case testing for boundary conditions and error handling"
        ]
        
        return recommendations

def main():
    parser = argparse.ArgumentParser(description="NLM Test Coverage - Analyze test coverage for the Neural Learning Machine system")
    parser.add_argument("source", help="Source code directory path", default="src")
    parser.add_argument("results", help="Test results file path", default="")
    parser.add_argument("--report", help="Coverage report file path")
    parser.add_argument("--verbose", action='store_true', help="Enable verbose output")
    
    args = parser.parse_args()
    
    # Create coverage analyzer
    coverage_analyzer = NLMTestCoverage(args.source, args.results)
    
    # Load source files
    if not coverage_analyzer.load_source_files(args.source):
        print("Failed to load source files. Exiting.")
        return 1
    
    # Load test results
    if not coverage_analyzer.load_test_results(args.results):
        print("Failed to load test results. Continuing with coverage analysis.")
    
    # Perform coverage analysis
    if args.verbose:
        coverage_analyzer.analyze_coverage()
    
    # Generate report
    report_file = args.report
    if not report_file:
        report_file = f"coverage_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
    
    coverage_analyzer.generate_coverage_report(report_file)
    
    print(f"\nCoverage analysis complete. Report saved to: {report_file}")
    return 0

if __name__ == "__main__":
    exit(main())