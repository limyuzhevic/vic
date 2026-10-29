#!/usr/bin/env python3
"""
NLM Python Test Runner

Comprehensive test runner for NLM Python bindings.

This script provides a complete test suite for the NLM Python bindings,
including unit tests, integration tests, performance benchmarks, and error handling tests.
"""

import sys
import os
import argparse
import pytest
import time
from pathlib import Path


def print_banner():
    """Print test suite banner."""
    print("=" * 60)
    print("NLM Python Test Suite")
    print("Neural Learning Machine - Python Bindings")
    print("=" * 60)
    print()


def print_usage():
    """Print usage information."""
    print("Usage: python test_runner.py [OPTIONS]")
    print()
    print("Options:")
    print("  --help, -h              Show this help message")
    print("  --version, -v           Show version information")
    print("  --all, -a              Run all tests")
    print("  --unit, -u             Run unit tests only")
    print("  --integration, -i      Run integration tests only")
    print("  --performance, -p      Run performance tests only")
    print("  --error, -e            Run error handling tests only")
    print("  --config, -c           Run configuration tests only")
    print("  --agent, -g            Run agent tests only")
    print("  --world, -w            Run world tests only")
    print("  --benchmark, -b        Run benchmark tests only")
    print("  --verbose, -V          Run with verbose output")
    print("  --coverage, -C          Generate coverage report")
    print("  --html, -H             Generate HTML report")
    print()
    print("Examples:")
    print("  python test_runner.py --all")
    print("  python test_runner.py --unit --verbose")
    print("  python test_runner.py --integration --benchmark")
    print("  python test_runner.py --all --coverage --html")


def print_version():
    """Print version information."""
    print("NLM Python Test Suite")
    print("Version: 2.0 (Enhanced)")
    print("Date: 2026-10-29")
    print("Author: NLM Development Team")
    print()
    print("Features:")
    print("  - Comprehensive unit testing")
    print("  - Integration testing")
    print("  - Performance benchmarking")
    print("  - Error handling tests")
    print("  - Configuration testing")
    print("  - Agent testing")
    print("  - World testing")
    print("  - Coverage reporting")
    print("  - HTML reporting")


def run_tests(test_patterns, verbose=False, coverage=False, html=False):
    """Run tests with specified patterns."""
    print(f"Running tests: {test_patterns}")
    print(f"Verbose: {verbose}")
    print(f"Coverage: {coverage}")
    print(f"HTML report: {html}")
    print()
    
    # Build pytest command
    cmd = ["pytest"]
    
    if verbose:
        cmd.append("-v")
    
    if coverage:
        cmd.extend([
            "--cov=pynlm",
            "--cov-report=term-missing"
        ])
    
    if html:
        cmd.extend([
            "--cov=pynlm",
            "--cov-report=html"
        ])
    
    cmd.extend(test_patterns)
    
    print(f"Running command: {' '.join(cmd)}")
    print()
    
    # Run tests
    start_time = time.time()
    
    try:
        result = pytest.main(cmd)
    except Exception as e:
        print(f"Error running tests: {e}")
        result = 1
    
    end_time = time.time()
    duration = end_time - start_time
    
    print()
    print(f"Test suite completed in {duration:.2f} seconds")
    print(f"Exit code: {result}")
    
    return result == 0


def get_test_patterns(args):
    """Get test patterns based on arguments."""
    if args.all:
        return [
            "tests/test_basic.py",
            "tests/test_configuration.py",
            "tests/test_agent.py",
            "tests/test_world.py",
            "tests/test_integration.py",
            "tests/test_error_handling.py",
            "tests/test_performance.py"
        ]
    
    patterns = []
    
    if args.unit:
        patterns.extend([
            "tests/test_basic.py",
            "tests/test_configuration.py"
        ])
    
    if args.integration:
        patterns.extend([
            "tests/test_agent.py",
            "tests/test_world.py",
            "tests/test_integration.py"
        ])
    
    if args.performance:
        patterns.append("tests/test_performance.py")
    
    if args.error:
        patterns.append("tests/test_error_handling.py")
    
    if args.config:
        patterns.append("tests/test_configuration.py")
    
    if args.agent:
        patterns.append("tests/test_agent.py")
    
    if args.world:
        patterns.append("tests/test_world.py")
    
    if args.benchmark:
        patterns.append("tests/test_performance.py")
    
    # If no specific patterns selected, run all
    if not patterns:
        return [
            "tests/test_basic.py",
            "tests/test_configuration.py",
            "tests/test_agent.py",
            "tests/test_world.py",
            "tests/test_integration.py",
            "tests/test_error_handling.py",
            "tests/test_performance.py"
        ]
    
    return patterns


def main():
    """Main test runner function."""
    parser = argparse.ArgumentParser(
        description="NLM Python Test Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter
    )
    
    parser.add_argument(
        "--help", "-h",
        action="store_true",
        help="Show this help message"
    )
    
    parser.add_argument(
        "--version", "-v",
        action="store_true",
        help="Show version information"
    )
    
    parser.add_argument(
        "--all", "-a",
        action="store_true",
        help="Run all tests"
    )
    
    parser.add_argument(
        "--unit", "-u",
        action="store_true",
        help="Run unit tests only"
    )
    
    parser.add_argument(
        "--integration", "-i",
        action="store_true",
        help="Run integration tests only"
    )
    
    parser.add_argument(
        "--performance", "-p",
        action="store_true",
        help="Run performance tests only"
    )
    
    parser.add_argument(
        "--error", "-e",
        action="store_true",
        help="Run error handling tests only"
    )
    
    parser.add_argument(
        "--config", "-c",
        action="store_true",
        help="Run configuration tests only"
    )
    
    parser.add_argument(
        "--agent", "-g",
        action="store_true",
        help="Run agent tests only"
    )
    
    parser.add_argument(
        "--world", "-w",
        action="store_true",
        help="Run world tests only"
    )
    
    parser.add_argument(
        "--benchmark", "-b",
        action="store_true",
        help="Run benchmark tests only"
    )
    
    parser.add_argument(
        "--verbose", "-V",
        action="store_true",
        help="Run with verbose output"
    )
    
    parser.add_argument(
        "--coverage", "-C",
        action="store_true",
        help="Generate coverage report"
    )
    
    parser.add_argument(
        "--html", "-H",
        action="store_true",
        help="Generate HTML report"
    )
    
    args = parser.parse_args()
    
    # Print banner
    print_banner()
    
    # Handle help and version
    if args.help:
        print_usage()
        return 0
    
    if args.version:
        print_version()
        return 0
    
    # Get test patterns
    test_patterns = get_test_patterns(args)
    
    # Run tests
    success = run_tests(
        test_patterns,
        verbose=args.verbose,
        coverage=args.coverage,
        html=args.html
    )
    
    return 0 if success else 1


if __name__ == "__main__":
    sys.exit(main())
