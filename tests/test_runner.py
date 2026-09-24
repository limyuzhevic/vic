#!/usr/bin/env python3
# Comprehensive test runner for NLM

import pytest
import sys
import os
import argparse
import time
from tests import test_helper

class NLMTTestRunner:
    """Test runner for NLM test suite"""
    
    def __init__(self):
        self.test_results = {}
        self.start_time = None
        self.end_time = None
    
    def run_all_tests(self, verbose=False, output_file=None):
        """Run all NLM tests"""
        self.start_time = time.time()
        
        test_categories = [
            'tests.basic_test',
            'tests.memory_test', 
            'tests.agent_test',
            'tests.integration_test',
            'tests.performance_test'
        ]
        
        results = {}
        
        for category in test_categories:
            try:
                if verbose:
                    print(f"\n=== Running {category} ===")
                
                # Run pytest for this category
                exit_code = pytest.main([
                    category,
                    '-v' if verbose else '',
                    '--tb=short'
                ])
                
                results[category] = {
                    'exit_code': exit_code,
                    'status': 'passed' if exit_code == 0 else 'failed'
                }
                
            except Exception as e:
                results[category] = {
                    'exit_code': 1,
                    'status': 'error',
                    'error': str(e)
                }
        
        self.end_time = time.time()
        self.test_results = results
        
        # Generate report
        self.generate_report(output_file)
        
        # Return overall success status
        all_passed = all(r['status'] == 'passed' for r in results.values())
        return all_passed
    
    def generate_report(self, output_file=None):
        """Generate test report"""
        if self.start_time is None or self.end_time is None:
            return
        
        total_duration = self.end_time - self.start_time
        
        report = f"NLM Test Suite Report\n"
        report += f"Generated: {time.ctime()}\n"
        report += f"Total duration: {total_duration:.2f} seconds\n\n"
        
        passed = 0
        failed = 0
        errored = 0
        
        for category, result in self.test_results.items():
            status = result['status']
            if status == 'passed':
                passed += 1
                icon = "✓"
            elif status == 'failed':
                failed += 1
                icon = "✗"
            else:
                errored += 1
                icon = "!"
            
            category_name = category.replace('tests.', '').replace('_test', '').replace('_', ' ').title()
            report += f"{icon} {category_name}: {status}\n"
            
            if status != 'passed' and 'error' in result:
                report += f"  Error: {result['error']}\n"
        
        report += f"\nSummary:\n"
        report += f"  Passed: {passed}\n"
        report += f"  Failed: {failed}\n"
        report += f"  Errors: {errored}\n"
        report += f"  Total: {passed + failed + errored}\n"
        
        print(report)
        
        if output_file:
            with open(output_file, 'w') as f:
                f.write(report)
            print(f"\nReport saved to: {output_file}")
    
    def run_specific_test(self, test_file, verbose=False):
        """Run specific test file"""
        try:
            exit_code = pytest.main([
                test_file,
                '-v' if verbose else '',
                '--tb=short'
            ])
            return exit_code == 0
        except Exception as e:
            print(f"Error running {test_file}: {e}")
            return False

def main():
    """Main test runner function"""
    parser = argparse.ArgumentParser(description='NLM Test Suite Runner')
    parser.add_argument('--category', choices=['basic', 'memory', 'agent', 'integration', 'performance'], 
                       help='Run specific test category')
    parser.add_argument('--verbose', '-v', action='store_true', help='Verbose output')
    parser.add_argument('--output', '-o', help='Output report file')
    parser.add_argument('--help-examples', action='store_true', help='Show example usage')
    
    args = parser.parse_args()
    
    if args.help_examples:
        print("NLM Test Suite Examples:")
        print("  pytest tests/basic_test.py -v")
        print("  pytest tests/memory_test.py -v")
        print("  pytest tests/agent_test.py -v")
        print("  pytest tests/integration_test.py -v")
        print("  pytest tests/performance_test.py -v")
        print("  python -m pytest tests/ -v")
        return
    
    runner = NLMTTestRunner()
    
    # Map category arguments to test files
    category_files = {
        'basic': 'tests/basic_test.py',
        'memory': 'tests/memory_test.py',
        'agent': 'tests/agent_test.py',
        'integration': 'tests/integration_test.py',
        'performance': 'tests/performance_test.py'
    }
    
    if args.category:
        test_file = category_files[args.category]
        print(f"Running {args.category} tests...")
        success = runner.run_specific_test(test_file, args.verbose)
    else:
        print("Running all NLM tests...")
        success = runner.run_all_tests(args.verbose, args.output)
    
    if success:
        print("\n✓ All tests passed!")
        sys.exit(0)
    else:
        print("\n✗ Some tests failed!")
        sys.exit(1)

if __name__ == '__main__':
    main()
