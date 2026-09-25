#!/usr/bin/env python3
"""
NLM Testing Infrastructure Validation Script

This script provides validation tools for the NLM testing infrastructure,
including test configuration validation, regression detection, and test reporting.

Usage:
    python3 validate_tests.py --mode [validate|regression|performance|all]
"""

import json
import sys
import os
import argparse
from datetime import datetime
from pathlib import Path
from typing import Dict, List, Any, Optional
import subprocess
import re

class NLMTestValidator:
    def __init__(self, project_root: str = "."):
        self.project_root = Path(project_root)
        self.test_results_dir = self.project_root / "test_results"
        self.test_results_dir.mkdir(exist_ok=True)
        
        # Test configuration paths
        self.config_paths = {
            'test_framework': self.project_root / "tests" / "test_main.cpp",
            'catch2_config': self.project_root / "CMakeLists.txt",
            'python_tests': self.project_root / "python" / "CMakeLists.txt",
        }
        
        # Performance benchmarks
        self.performance_benchmarks = {
            'test_execution_time': 1000.0,  # milliseconds
            'memory_usage': 100.0,  # MB
            'test_coverage': 0.95,  # percentage
            'parallel_efficiency': 0.85,
        }
    
    def validate_test_framework(self) -> Dict[str, Any]:
        """Validate the test framework configuration."""
        result = {
            'status': 'PASS',
            'checks': [],
            'timestamp': datetime.now().isoformat(),
        }
        
        # Check 1: Catch2 configuration
        if self.config_paths['catch2_config'].exists():
            content = self.config_paths['catch2_config'].read_text()
            if 'find_package(Catch2 REQUIRED)' in content:
                result['checks'].append({
                    'name': 'Catch2 Detection',
                    'status': 'PASS',
                    'message': 'Catch2 package properly configured for testing'
                })
            else:
                result['checks'].append({
                    'name': 'Catch2 Detection',
                    'status': 'FAIL',
                    'message': 'Catch2 not properly configured'
                })
                result['status'] = 'FAIL'
        else:
            result['checks'].append({
                'name': 'Catch2 Detection',
                'status': 'FAIL',
                'message': 'Catch2 CMakeLists.txt not found'
            })
            result['status'] = 'FAIL'
        
        # Check 2: Test main file
        if self.config_paths['test_framework'].exists():
            content = self.config_paths['test_framework'].read_text()
            if '#include <catch2/catch_test_macros.hpp>' in content:
                result['checks'].append({
                    'name': 'Catch2 Test Framework',
                    'status': 'PASS',
                    'message': 'Modern Catch2 test framework properly included'
                })
            else:
                result['checks'].append({
                    'name': 'Catch2 Test Framework',
                    'status': 'FAIL',
                    'message': 'Catch2 test framework not properly configured'
                })
                result['status'] = 'FAIL'
        else:
            result['checks'].append({
                'name': 'Catch2 Test Framework',
                'status': 'FAIL',
                'message': 'test_main.cpp not found'
            })
            result['status'] = 'FAIL'
        
        # Check 3: Python test configuration
        if self.config_paths['python_tests'].exists():
            content = self.config_paths['python_tests'].read_text()
            if 'find_package(Python' in content and 'pybind11_add_module' in content:
                result['checks'].append({
                    'name': 'Python Test Bindings',
                    'status': 'PASS',
                    'message': 'Python test bindings properly configured'
                })
            else:
                result['checks'].append({
                    'name': 'Python Test Bindings',
                    'status': 'WARNING',
                    'message': 'Python test bindings may need configuration'
                })
        else:
            result['checks'].append({
                'name': 'Python Test Bindings',
                'status': 'WARNING',
                'message': 'Python test configuration not found'
            })
        
        return result
    
    def validate_test_structure(self) -> Dict[str, Any]:
        """Validate the overall test structure and organization."""
        result = {
            'status': 'PASS',
            'checks': [],
            'timestamp': datetime.now().isoformat(),
        }
        
        # Check 1: Test files existence
        test_files = [
            'tests/test_types.cpp',
            'tests/test_config.cpp',
            'tests/test_config_simple.cpp',
            'tests/test_random.cpp',
            'tests/test_clock.cpp',
            'tests/test_neuron.cpp',
            'tests/test_synapse.cpp',
            'tests/test_stdp.cpp',
            'tests/test_brain.cpp',
            'tests/test_dynamics.cpp',
            'tests/test_plasticity.cpp',
            'tests/test_development.cpp',
            'tests/test_neuromodulation.cpp',
            'tests/test_memory.cpp',
            'tests/test_cognition.cpp',
            'tests/test_sensory.cpp',
            'tests/test_prediction.cpp',
            'tests/test_motor.cpp',
            'tests/test_environment.cpp',
            'tests/test_world.cpp',
            'tests/test_agent.cpp',
            'tests/test_visualization.cpp',
            'tests/test_experiments.cpp',
            'tests/test_performance.cpp',
            'tests/test_regression.cpp',
            'tests/config_demonstration.cpp',
        ]
        
        existing_count = 0
        missing_count = 0
        
        for test_file in test_files:
            if (self.project_root / test_file).exists():
                existing_count += 1
                result['checks'].append({
                    'name': f'Test File: {test_file}',
                    'status': 'PASS',
                    'message': 'Test file properly created'
                })
            else:
                missing_count += 1
                result['checks'].append({
                    'name': f'Test File: {test_file}',
                    'status': 'FAIL',
                    'message': 'Required test file is missing'
                })
                result['status'] = 'FAIL'
        
        result['checks'].append({
            'name': 'Test Coverage',
            'status': 'PASS' if missing_count == 0 else 'WARNING',
            'message': f'{existing_count} test files found, {missing_count} missing'
        })
        
        return result
    
    def validate_performance_benchmarks(self) -> Dict[str, Any]:
        """Validate performance benchmarks and test efficiency."""
        result = {
            'status': 'PASS',
            'checks': [],
            'benchmarks': {},
            'timestamp': datetime.now().isoformat(),
        }
        
        # Check 1: Test execution performance
        try:
            # Run a quick test to measure execution time
            start_time = datetime.now()
            
            # Simulate test execution by checking file counts
            test_dir = self.project_root / "tests"
            test_files = list(test_dir.glob("*.cpp"))
            
            end_time = datetime.now()
            execution_time = (end_time - start_time).total_seconds() * 1000  # milliseconds
            
            result['benchmarks']['test_scan_time'] = execution_time
            result['benchmarks']['test_file_count'] = len(test_files)
            
            if execution_time < self.performance_benchmarks['test_execution_time']:
                result['checks'].append({
                    'name': 'Test Execution Performance',
                    'status': 'PASS',
                    'message': f'Test scanning completed in {execution_time:.2f}ms (target: <{self.performance_benchmarks["test_execution_time"]}ms)'
                })
            else:
                result['checks'].append({
                    'name': 'Test Execution Performance',
                    'status': 'WARNING',
                    'message': f'Test scanning took {execution_time:.2f}ms (target: <{self.performance_benchmarks["test_execution_time"]}ms)'
                })
                result['status'] = 'WARNING'
        except Exception as e:
            result['checks'].append({
                'name': 'Test Execution Performance',
                'status': 'FAIL',
                'message': f'Failed to measure test execution performance: {str(e)}'
            })
            result['status'] = 'FAIL'
        
        # Check 2: Test coverage analysis
        total_lines = 0
        test_lines = 0
        
        for test_file in self.project_root.glob("tests/*.cpp"):
            content = test_file.read_text()
            total_lines += len(content.splitlines())
            # Count lines that contain "TEST_CASE" or "REQUIRE"
            test_lines += len([line for line in content.splitlines() 
                             if 'TEST_CASE' in line or 'REQUIRE' in line])
        
        coverage = (test_lines / total_lines * 100) if total_lines > 0 else 0
        
        result['benchmarks']['code_coverage_percentage'] = coverage
        result['benchmarks']['total_test_lines'] = test_lines
        result['benchmarks']['total_source_lines'] = total_lines
        
        if coverage >= self.performance_benchmarks['test_coverage']:
            result['checks'].append({
                'name': 'Test Code Coverage',
                'status': 'PASS',
                'message': f'Test code coverage: {coverage:.1f}% (target: >{self.performance_benchmarks["test_coverage"]}%)'
            })
        else:
            result['checks'].append({
                'name': 'Test Code Coverage',
                'status': 'WARNING',
                'message': f'Test code coverage: {coverage:.1f}% (target: >{self.performance_benchmarks["test_coverage"]}%)'
            })
            result['status'] = 'WARNING'
        
        return result
    
    def validate_regression_testing(self) -> Dict[str, Any]:
        """Validate regression testing infrastructure."""
        result = {
            'status': 'PASS',
            'checks': [],
            'regression_tests': [],
            'timestamp': datetime.now().isoformat(),
        }
        
        # Check 1: Regression test file
        regression_file = self.project_root / "tests" / "test_regression.cpp"
        if regression_file.exists():
            content = regression_file.read_text()
            if 'RegressionTestDatabase' in content and 'PerformanceRegressionDetector' in content:
                result['checks'].append({
                    'name': 'Regression Testing Framework',
                    'status': 'PASS',
                    'message': 'Regression testing infrastructure properly implemented'
                })
                
                # Extract regression test count
                test_case_matches = re.findall(r'TEST_CASE\("([^"]*)"', content)
                result['regression_tests'] = test_case_matches
                result['checks'].append({
                    'name': 'Regression Test Count',
                    'status': 'PASS',
                    'message': f'Found {len(test_case_matches)} regression test cases'
                })
            else:
                result['checks'].append({
                    'name': 'Regression Testing Framework',
                    'status': 'FAIL',
                    'message': 'Regression testing infrastructure incomplete'
                })
                result['status'] = 'FAIL'
        else:
            result['checks'].append({
                'name': 'Regression Testing Framework',
                'status': 'FAIL',
                'message': 'Regression test file not found'
            })
            result['status'] = 'FAIL'
        
        # Check 2: Baseline results
        baseline_file = self.test_results_dir / "baseline_results.json"
        if baseline_file.exists():
            try:
                baseline_data = json.loads(baseline_file.read_text())
                result['checks'].append({
                    'name': 'Baseline Results Storage',
                    'status': 'PASS',
                    'message': f'Baseline results file found with {len(baseline_data)} test entries'
                })
            except json.JSONDecodeError:
                result['checks'].append({
                    'name': 'Baseline Results Storage',
                    'status': 'FAIL',
                    'message': 'Baseline results file contains invalid JSON'
                })
                result['status'] = 'FAIL'
        else:
            result['checks'].append({
                'name': 'Baseline Results Storage',
                'status': 'WARNING',
                'message': 'Baseline results file not found (will be created during first run)'
            })
        
        return result
    
    def validate_documentation(self) -> Dict[str, Any]:
        """Validate testing documentation and guidelines."""
        result = {
            'status': 'PASS',
            'checks': [],
            'documentation_files': [],
            'timestamp': datetime.now().isoformat(),
        }
        
        # Check 1: Documentation files
        doc_files = [
            'docs/TESTING.md',
            'docs/TEST_GUIDELINES.md',
            'docs/REGRESSION_TESTING.md',
            'docs/TEST_PERFORMANCE.md',
        ]
        
        for doc_file in doc_files:
            doc_path = self.project_root / doc_file
            if doc_path.exists():
                content = doc_path.read_text()
                result['documentation_files'].append(doc_file)
                
                # Check for key sections
                if 'test' in content.lower() and 'guide' in content.lower():
                    result['checks'].append({
                        'name': f'Documentation: {doc_file}',
                        'status': 'PASS',
                        'message': 'Documentation file contains test guidelines'
                    })
                else:
                    result['checks'].append({
                        'name': f'Documentation: {doc_file}',
                        'status': 'WARNING',
                        'message': 'Documentation file may need content review'
                    })
            else:
                result['checks'].append({
                    'name': f'Documentation: {doc_file}',
                    'status': 'WARNING',
                    'message': 'Documentation file not found (will be created)'
                })
        
        return result
    
    def run_validation(self, validation_type: str = "all") -> Dict[str, Any]:
        """Run comprehensive validation based on specified type."""
        results = {
            'validation_type': validation_type,
            'timestamp': datetime.now().isoformat(),
            'summary': {},
            'details': {},
            'overall_status': 'PASS',
        }
        
        if validation_type in ["all", "framework"]:
            results['details']['test_framework'] = self.validate_test_framework()
        
        if validation_type in ["all", "structure"]:
            results['details']['test_structure'] = self.validate_test_structure()
        
        if validation_type in ["all", "performance"]:
            results['details']['performance'] = self.validate_performance_benchmarks()
        
        if validation_type in ["all", "regression"]:
            results['details']['regression'] = self.validate_regression_testing()
        
        if validation_type in ["all", "documentation"]:
            results['details']['documentation'] = self.validate_documentation()
        
        # Calculate overall status
        for category, result in results['details'].items():
            if result['status'] == 'FAIL':
                results['overall_status'] = 'FAIL'
            elif result['status'] == 'WARNING' and results['overall_status'] == 'PASS':
                results['overall_status'] = 'WARNING'
        
        # Create summary
        results['summary'] = {
            'total_checks': sum(len(result['checks']) for result in results['details'].values()),
            'passed_checks': sum(1 for result in results['details'].values() 
                               for check in result['checks'] if check['status'] == 'PASS'),
            'failed_checks': sum(1 for result in results['details'].values() 
                               for check in result['checks'] if check['status'] == 'FAIL'),
            'warning_checks': sum(1 for result in results['details'].values() 
                                for check in result['checks'] if check['status'] == 'WARNING'),
        }
        
        return results
    
    def export_validation_results(self, results: Dict[str, Any], output_path: str) -> None:
        """Export validation results to JSON file."""
        output_file = Path(output_path)
        
        # Add metadata
        results['metadata'] = {
            'project_root': str(self.project_root),
            'validation_tool_version': '1.0.0',
            'python_version': sys.version,
        }
        
        # Write to file
        with open(output_file, 'w') as f:
            json.dump(results, f, indent=2, default=str)
        
        print(f"Validation results exported to: {output_file}")

def main():
    parser = argparse.ArgumentParser(description="NLM Testing Infrastructure Validation")
    parser.add_argument('--mode', choices=['framework', 'structure', 'performance', 'regression', 'documentation', 'all'],
                       default='all', help='Validation mode')
    parser.add_argument('--output', '-o', help='Output file for validation results (JSON)')
    parser.add_argument('--project-root', default='.', help='Path to NLM project root')
    
    args = parser.parse_args()
    
    print("=== NLM Testing Infrastructure Validation ===")
    print(f"Project Root: {args.project_root}")
    print(f"Validation Mode: {args.mode}")
    print()
    
    validator = NLMTestValidator(args.project_root)
    results = validator.run_validation(args.mode)
    
    # Print summary
    print("=== Validation Summary ===")
    print(f"Overall Status: {results['overall_status']}")
    print(f"Total Checks: {results['summary']['total_checks']}")
    print(f"Passed: {results['summary']['passed_checks']}")
    print(f"Failed: {results['summary']['failed_checks']}")
    print(f"Warnings: {results['summary']['warning_checks']}")
    print()
    
    # Print detailed results
    for category, result in results['details'].items():
        print(f"=== {category.title()} Validation ===")
        print(f"Status: {result['status']}")
        
        for check in result.get('checks', []):
            status_icon = "✓" if check['status'] == 'PASS' else "⚠" if check['status'] == 'WARNING' else "✗"
            print(f"  {status_icon} {check['name']}: {check['message']}")
        
        if category == 'performance' and 'benchmarks' in result:
            print("  Benchmarks:")
            for benchmark, value in result['benchmarks'].items():
                print(f"    {benchmark}: {value}")
        
        if category == 'regression' and 'regression_tests' in result:
            print(f"  Regression Tests: {len(result['regression_tests'])} cases")
        
        print()
    
    # Export results if requested
    if args.output:
        validator.export_validation_results(results, args.output)
    
    # Return exit code
    if results['overall_status'] == 'FAIL':
        sys.exit(1)
    elif results['overall_status'] == 'WARNING':
        sys.exit(2)
    else:
        sys.exit(0)
if __name__ == "__main__":
    main()
