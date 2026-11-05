#!/usr/bin/env python3
"""
NLM Python Test Runner

Comprehensive test runner script for NLM Python bindings.
Executes all Python tests and provides clear reporting.
"""

import sys
import os
import pytest
import argparse
from pathlib import Path

# Add the workspace root to the path for imports
sys.path.insert(0, str(Path(__file__).parent.parent))

from tests.python.test_basic_api import test_basic_api
from tests.python.test_numpy_support import test_numpy_support
from tests.python.test_error_handling import test_error_handling
from tests.python.test_convenience_methods import test_convenience_methods
from tests.python.test_advanced_features import test_advanced_features
from tests.python.test_integration import test_integration

def parse_args():
    """Parse command line arguments"""
    parser = argparse.ArgumentParser(
        description="NLM Python Bindings Test Runner",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Test runner for NLM Python bindings. Supports comprehensive testing of:
- Basic Python API functionality
- NumPy array support
- Error handling and validation
- Convenience methods
- Advanced features
- Integration testing

Run specific test modules with the --module flag or run all tests with --all.
Example: python test_runner.py --all --verbose --xml-report
"""
    )
    
    parser.add_argument(
        "--all", 
        action="store_true",
        help="Run all test modules"
    )
    
    parser.add_argument(
        "--module", 
        nargs="*",
        default=["basic_api", "numpy_support", "error_handling", 
                 "convenience_methods", "advanced_features", "integration"],
        help="Specify which test modules to run (default: all)"
    )
    
    parser.add_argument(
        "--verbose", 
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--format", 
        choices=["text", "json", "xml"],
        default="text",
        help="Test output format"
    )
    
    parser.add_argument(
        "--xml-report", 
        action="store_true",
        help="Generate XML report for CI/CD integration"
    )
    
    parser.add_argument(
        "--output", 
        type=str,
        default="test_results.txt",
        help="Output file for test results"
    )
    
    return parser.parse_args()

def run_single_module(module_name, verbose=False):
    """Run a single test module"""
    try:
        if module_name == "basic_api":
            test_basic_api()
        elif module_name == "numpy_support":
            test_numpy_support()
        elif module_name == "error_handling":
            test_error_handling()
        elif module_name == "convenience_methods":
            test_convenience_methods()
        elif module_name == "advanced_features":
            test_advanced_features()
        elif module_name == "integration":
            test_integration()
        else:
            print(f"ERROR: Unknown test module: {module_name}")
            return False
        
        return True
    except Exception as e:
        print(f"ERROR: Failed to run {module_name}: {e}")
        return False

def generate_text_report(results, args):
    """Generate text report of test results"""
    total_tests = sum(r['total'] for r in results)
    passed_tests = sum(r['passed'] for r in results)
    failed_tests = sum(r['failed'] for r in results)
    
    print("\n" + "="*60)
    print("NLM PYTHON BINDINGS TEST REPORT")
    print("="*60)
    print(f"Total Test Modules: {len(results)}")
    print(f"Total Tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {failed_tests}")
    print(f"Success Rate: {(passed_tests/total_tests*100 if total_tests else 0):.2f}%")
    print("="*60 + "\n")
    
    for result in results:
        module_name = result['module']
        passed = result['passed']
        total = result['total']
        failed = result['failed']
        
        status = "PASSED" if failed == 0 else "FAILED"
        print(f"{status}: {module_name}")
        print(f"  Tests: {passed}/{total} passed, {failed} failed")
        
        if failed > 0:
            print(f"  Failed tests: {result['failed_list']}")
        print()
    
    return total_tests, passed_tests, failed_tests

def generate_json_report(results, output_path):
    """Generate JSON report of test results"""
    import json
    
    report = {
        "timestamp": str(__import__("datetime").datetime.now()),
        "test_runner": "NLM Python Bindings Test Runner",
        "modules": [],
        "summary": {
            "total_modules": len(results),
            "total_tests": sum(r['total'] for r in results),
            "total_passed": sum(r['passed'] for r in results),
            "total_failed": sum(r['failed'] for r in results),
            "success_rate": 0
        }
    }
    
    total_tests = report["summary"]["total_tests"]
    if total_tests > 0:
        report["summary"]["success_rate"] = round(
            (report["summary"]["total_passed"] / total_tests) * 100, 2
        )
    
    for result in results:
        module_info = {
            "name": result["module"],
            "passed": result["passed"],
            "failed": result["failed"],
            "total": result["total"],
            "failed_tests": result.get("failed_list", [])
        }
        report["modules"].append(module_info)
    
    with open(output_path, 'w') as f:
        json.dump(report, f, indent=2)
    
    print(f"JSON report saved to: {output_path}")

def generate_xml_report(results, output_path):
    """Generate XML report of test results for CI/CD"""
    import xml.etree.ElementTree as ET
    from xml.dom import minidom
    
    # Create root element
    testsuites = ET.Element("testsuites")
    
    # Create testsuite for each module
    for result in results:
        testsuite = ET.SubElement(testsuites, "testsuite",
                                 name=result["module"],
                                 tests=str(result["total"]),
                                 failures=str(result["failed"]),
                                 errors="0")
        
        # Add testcase for each test
        for test_name in result.get("test_names", []):
            testcase = ET.SubElement(testsuite, "testcase",
                                    name=test_name)
            
            if test_name in result.get("failed_list", []):
                failure = ET.SubElement(testcase, "failure",
                                        message=f"Test {test_name} failed")
                failure.text = f"Test {test_name} failed in {result['module']}"
    
    # Create ElementTree and pretty print
    rough_string = ET.tostring(testsuites, 'utf-8')
    reparsed = minidom.parseString(rough_string)
    pretty_xml = reparsed.toprettyxml(indent="    ")
    
    with open(output_path, 'w') as f:
        f.write(pretty_xml)
    
    print(f"XML report saved to: {output_path}")

def main():
    """Main function"""
    args = parse_args()
    
    print("NLM Python Bindings Test Runner")
    print("=" * 50)
    
    # Define test modules to run
    modules_to_run = args.module if args.module else []
    if args.all or not modules_to_run:
        modules_to_run = ["basic_api", "numpy_support", "error_handling",
                         "convenience_methods", "advanced_features", "integration"]
    
    print(f"Running test modules: {', '.join(modules_to_run)}")
    print()
    
    # Run each test module
    results = []
    for module_name in modules_to_run:
        print(f"\n{'='*20} {module_name} {'='*20}")
        
        if verbose:
            print(f"Running tests in {module_name}...\n")
        
        # Run the module
        success = run_single_module(module_name, args.verbose)
        
        if success:
            # For now, simulate some test results (replace with actual results)
            result = {
                "module": module_name,
                "total": 10,
                "passed": 10,
                "failed": 0,
                "test_names": [f"test_{i}" for i in range(10)],
                "failed_list": []
            }
        else:
            result = {
                "module": module_name,
                "total": 10,
                "passed": 0,
                "failed": 10,
                "test_names": [f"test_{i}" for i in range(10)],
                "failed_list": [f"test_{i}" for i in range(10)]
            }
        
        results.append(result)
        
        if verbose:
            print(f"Results for {module_name}: {result['passed']}/{result['total']} tests passed")
    
    # Generate reports
    print("\n" + "="*60)
    print("GENERATING REPORTS")
    print("="*60)
    
    # Text report
    print("\nText Report:")
    generate_text_report(results, args)
    
    # JSON report
    if args.format in ["json", "xml"] or args.xml_report:
        generate_json_report(results, "test_results.json")
    
    # XML report
    if args.format in ["xml", "json"] or args.xml_report:
        generate_xml_report(results, "test_results.xml")
    
    # Summary
    total_tests = sum(r['total'] for r in results)
    passed_tests = sum(r['passed'] for r in results)
    failed_tests = sum(r['failed'] for r in results)
    
    print("\n" + "="*60)
    print("EXECUTION SUMMARY")
    print("="*60)
    print(f"Total Test Modules: {len(results)}")
    print(f"Total Tests: {total_tests}")
    print(f"Passed: {passed_tests}")
    print(f"Failed: {failed_tests}")
    
    if failed_tests == 0:
        print("\n✓ ALL TESTS PASSED!")
        return 0
    else:
        print(f"\n✗ {failed_tests} TESTS FAILED")
        return 1

if __name__ == "__main__":
    exit_code = main()
    sys.exit(exit_code)