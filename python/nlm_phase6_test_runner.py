#!/usr/bin/env python3
"""
NLM Phase 6 Integration Test Runner

Main test runner for NLM Phase 6 integration components.
This file orchestrates all Phase 6 integration tests for the NLM framework.

Usage:
    python nlm_phase6_test_runner.py [options]

Options:
    --help                 Show help message
    --version              Show version information
    --test-dir PATH        Specify test directory (default: ./tests)
    --output-dir PATH      Specify output directory (default: ./test_results)
    --verbose              Enable verbose output
    --quiet                Suppress non-essential output
    --performance          Run performance tests
    --integration         Run integration tests
    --all                 Run all tests (default)
    --help-examples        Show examples of test usage

Examples:
    python nlm_phase6_test_runner.py
    python nlm_phase6_test_runner.py --performance --integration
    python nlm_phase6_test_runner.py --verbose --test-dir ../tests
    python nlm_phase6_test_runner.py --help-examples
"""

import argparse
import sys
import os
import time
import json
import tempfile
import shutil
from pathlib import Path
from typing import Dict, Any, List, Optional

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import test modules
from nlm_phase6_test_suite import (
    create_nlm_test_suite,
    create_test_fixtures,
    create_test_documentation,
    create_neuromodulation_test_suite,
    create_prediction_test_suite,
    create_development_test_suite,
    create_cognition_test_suite,
    create_integration_test_suite,
    create_performance_test_suite
)

# Version information
VERSION = "2.0.0"
DESCRIPTION = "NLM Phase 6 Integration Test Runner"

class NLMPhase6TestRunner:
    """Main test runner for NLM Phase 6 integration tests."""
    
    def __init__(self, args: Optional[argparse.Namespace] = None):
        """Initialize test runner with command line arguments."""
        self.args = args or self._parse_arguments()
        self.temp_dir = None
        self.results = {
            "test_suite": DESCRIPTION,
            "version": VERSION,
            "timestamp": time.time(),
            "arguments": vars(self.args) if self.args else {},
            "results": {},
            "summary": {}
        }
        self.passed_tests = 0
        self.failed_tests = 0
        self.total_tests = 0
        
    def _parse_arguments(self) -> argparse.Namespace:
        """Parse command line arguments."""
        parser = argparse.ArgumentParser(
            description=DESCRIPTION,
            formatter_class=argparse.RawDescriptionHelpFormatter
        )
        
        parser.add_argument(
            "--help-examples",
            action="store_true",
            help="Show examples of test usage"
        )
        
        parser.add_argument(
            "--version",
            action="store_true",
            help="Show version information"
        )
        
        parser.add_argument(
            "--test-dir",
            type=str,
            default="./tests",
            help="Specify test directory (default: ./tests)"
        )
        
        parser.add_argument(
            "--output-dir",
            type=str,
            default="./test_results",
            help="Specify output directory (default: ./test_results)"
        )
        
        parser.add_argument(
            "--verbose",
            action="store_true",
            help="Enable verbose output"
        )
        
        parser.add_argument(
            "--quiet",
            action="store_true",
            help="Suppress non-essential output"
        )
        
        group = parser.add_mutually_exclusive_group()
        group.add_argument(
            "--performance",
            action="store_true",
            help="Run performance tests only"
        )
        
        group.add_argument(
            "--integration",
            action="store_true",
            help="Run integration tests only"
        )
        
        group.add_argument(
            "--all",
            action="store_true",
            help="Run all tests (default)"
        )
        
        return parser.parse_args()
    
    def print_help_examples(self):
        """Print examples of test usage."""
        examples = """
# Basic test run
python nlm_phase6_test_runner.py

# Performance tests only
python nlm_phase6_test_runner.py --performance

# Integration tests only
python nlm_phase6_test_runner.py --integration

# Custom test directory
python nlm_phase6_test_runner.py --test-dir ../tests

# Custom output directory
python nlm_phase6_test_runner.py --output-dir ./my_results

# Verbose output
python nlm_phase6_test_runner.py --verbose

# All tests with custom directories
python nlm_phase6_test_runner.py --test-dir ./tests --output-dir ./results

# Quiet mode (minimal output)
python nlm_phase6_test_runner.py --quiet
        """
        print("=== NLM Phase 6 Test Runner Examples ===")
        print(examples)
    
    def print_version(self):
        """Print version information."""
        print(f"{DESCRIPTION} v{VERSION}")
        print("Phase 6 integration test suite for NLM framework")
        print("Copyright (c) 2026 NLM Project")
        print("License: MIT")
    
    def run(self) -> bool:
        """Run all tests and return success status."""
        print("=" * 80)
        print(f"{DESCRIPTION}")
        print("=" * 80)
        
        # Create output directory
        os.makedirs(self.args.output_dir, exist_ok=True)
        
        # Initialize temporary directory for test artifacts
        self.temp_dir = tempfile.mkdtemp(prefix="nlm_phase6_test_")
        print(f"📁 Test artifacts directory: {self.temp_dir}")
        
        try:
            # Run tests based on arguments
            if self.args.help_examples:
                self.print_help_examples()
                return True
            
            if self.args.version:
                self.print_version()
                return True
            
            # Determine which tests to run
            run_performance = self.args.performance
            run_integration = self.args.integration
            run_all = self.args.all or (not run_performance and not run_integration)
            
            if run_all:
                print("🚀 Running ALL Phase 6 integration tests...")
                self.run_all_tests()
            elif run_performance:
                print("⚡ Running Performance tests only...")
                self.run_performance_tests()
            elif run_integration:
                print("🔗 Running Integration tests only...")
                self.run_integration_tests()
            
            # Save results
            self.save_results()
            
            # Print summary
            self.print_summary()
            
            # Cleanup
            self.cleanup()
            
            # Return success status
            success = (self.failed_tests == 0)
            if success:
                print("\n🎉 All tests completed successfully!")
            else:
                print(f"\n⚠️  {self.failed_tests} test(s) failed.")
            
            return success
            
        except Exception as e:
            print(f"❌ Test runner failed with error: {e}")
            import traceback
            if self.args.verbose:
                traceback.print_exc()
            return False
    
    def run_all_tests(self):
        """Run all test suites."""
        print("\n📊 Running all Phase 6 test suites...")
        
        # Create test fixtures
        print("🛠️  Creating test fixtures...")
        fixtures = create_test_fixtures()
        
        # Save fixtures
        with open(os.path.join(self.temp_dir, "TEST_FIXTURES.json"), "w") as f:
            json.dump(fixtures, f, indent=2)
        
        # Run test suites
        test_suites = [
            ("Phase 6 Test Suite", self.run_test_suite),
            ("Memory System Tests", self.run_memory_tests),
            ("Neuromodulation System Tests", self.run_neuromodulation_tests),
            ("Prediction System Tests", self.run_prediction_tests),
            ("Development System Tests", self.run_development_tests),
            ("Cognition System Tests", self.run_cognition_tests),
            ("Integration Tests", self.run_integration_tests),
            ("Performance Tests", self.run_performance_tests),
        ]
        
        for suite_name, suite_func in test_suites:
            print(f"\n📋 Running {suite_name}...")
            suite_func()
    
    def run_test_suite(self):
        """Run the main Phase 6 test suite."""
        print("   🛠️  Creating comprehensive test infrastructure...")
        try:
            success = create_nlm_test_suite()
            if success:
                self.passed_tests += 1
                print("   ✅ Phase 6 Test Suite: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Phase 6 Test Suite: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Phase 6 Test Suite: ERROR - {e}")
    
    def run_memory_tests(self):
        """Run memory system tests."""
        print("   🧠 Running Memory System Tests...")
        try:
            success = run_nlm_phase6_memory_tests()
            if success:
                self.passed_tests += 1
                print("   ✅ Memory System Tests: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Memory System Tests: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Memory System Tests: ERROR - {e}")
    
    def run_neuromodulation_tests(self):
        """Run neuromodulation system tests."""
        print("   ⚡ Running Neuromodulation System Tests...")
        try:
            success = create_neuromodulation_test_suite()
            if success:
                self.passed_tests += 1
                print("   ✅ Neuromodulation System Tests: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Neuromodulation System Tests: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Neuromodulation System Tests: ERROR - {e}")
    
    def run_prediction_tests(self):
        """Run prediction system tests."""
        print("   🔮 Running Prediction System Tests...")
        try:
            success = create_prediction_test_suite()
            if success:
                self.passed_tests += 1
                print("   ✅ Prediction System Tests: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Prediction System Tests: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Prediction System Tests: ERROR - {e}")
    
    def run_development_tests(self):
        """Run development system tests."""
        print("   🌱 Running Development System Tests...")
        try:
            success = create_development_test_suite()
            if success:
                self.passed_tests += 1
                print("   ✅ Development System Tests: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Development System Tests: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Development System Tests: ERROR - {e}")
    
    def run_cognition_tests(self):
        """Run cognition system tests."""
        print("   🧠 Running Cognition System Tests...")
        try:
            success = create_cognition_test_suite()
            if success:
                self.passed_tests += 1
                print("   ✅ Cognition System Tests: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Cognition System Tests: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Cognition System Tests: ERROR - {e}")
    
    def run_integration_tests(self):
        """Run integration tests."""
        print("   🔗 Running Integration Tests...")
        try:
            success = create_integration_test_suite()
            if success:
                self.passed_tests += 1
                print("   ✅ Integration Tests: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Integration Tests: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Integration Tests: ERROR - {e}")
    
    def run_performance_tests(self):
        """Run performance tests."""
        print("   ⚡ Running Performance Tests...")
        try:
            success = create_performance_test_suite()
            if success:
                self.passed_tests += 1
                print("   ✅ Performance Tests: PASSED")
            else:
                self.failed_tests += 1
                print("   ❌ Performance Tests: FAILED")
        except Exception as e:
            self.failed_tests += 1
            print(f"   ❌ Performance Tests: ERROR - {e}")
    
    def save_results(self):
        """Save test results to files."""
        print("\n💾 Saving test results...")
        
        # Save detailed results
        results_file = os.path.join(self.args.output_dir, "nlm_phase6_test_results.json")
        with open(results_file, 'w') as f:
            json.dump(self.results, f, indent=2)
        
        print(f"   📄 Detailed results saved to: {results_file}")
        
        # Save summary
        summary_file = os.path.join(self.args.output_dir, "test_summary.txt")
        with open(summary_file, 'w') as f:
            f.write(self.generate_summary_text())
        
        print(f"   📄 Summary saved to: {summary_file}")
        
        # Create a simple README
        readme_file = os.path.join(self.args.output_dir, "README.md")
        with open(readme_file, 'w') as f:
            f.write(self.generate_readme())
        
        print(f"   📄 README saved to: {readme_file}")
    
    def generate_summary_text(self) -> str:
        """Generate summary text."""
        summary = f"""
NLM Phase 6 Integration Test Summary
=====================================

Test Suite: {self.results['test_suite']}
Version: {self.results['version']}
Timestamp: {time.ctime(self.results['timestamp'])}

Results:
--------
Total Tests: {self.total_tests}
Passed: {self.passed_tests}
Failed: {self.failed_tests}
Success Rate: {(self.passed_tests / max(1, self.total_tests) * 100):.1f}%

Test Results:
-------------
{self.generate_test_results_text()}

Performance Metrics:
-------------------
"""
        
        # Add performance metrics from results
        if "performance_metrics" in self.results:
            metrics = self.results["performance_metrics"]
            for metric_name, value in metrics.items():
                summary += f"{metric_name}: {value:.2f}\n"
        
        return summary
    
    def generate_test_results_text(self) -> str:
        """Generate test results text."""
        if not self.results.get("results"):
            return "No detailed results available."
        
        text = ""
        for test_name, result in self.results["results"].items():
            status = result.get("status", "UNKNOWN")
            duration = result.get("duration", 0)
            details = result.get("details", "")
            text += f"{test_name}: {status} ({duration:.2f}s)\n"
            if details:
                text += f"  {details}\n"
        
        return text
    
    def generate_readme(self) -> str:
        """Generate README file content."""
        readme = f"""# NLM Phase 6 Integration Test Results

## Overview
This directory contains the results of the NLM Phase 6 integration test suite.

## Test Configuration

**Test Suite**: {self.results['test_suite']} v{self.results['version']}

**Test Arguments**:
{chr(10).join([f"- {k}: {v}" for k, v in self.results['arguments'].items() if v])}

**Timestamp**: {time.ctime(self.results['timestamp'])}

## Test Results

### Summary
- **Total Tests**: {self.total_tests}
- **Passed**: {self.passed_tests}
- **Failed**: {self.failed_tests}
- **Success Rate**: {(self.passed_tests / max(1, self.total_tests) * 100):.1f}%

### Detailed Results

See `nlm_phase6_test_results.json` for detailed test results.

### Performance Metrics

See `nlm_phase6_test_results.json` for performance metrics.

## Files

- `nlm_phase6_test_results.json`: Detailed test results and metrics
- `test_summary.txt`: Human-readable test summary
- `README.md`: This file

## Usage

To run tests again, use the NLM Phase 6 Test Runner:

```bash
python nlm_phase6_test_runner.py --test-dir ./tests --output-dir ./test_results
```

For more options, use:

```bash
python nlm_phase6_test_runner.py --help
```

## Notes

- Tests are run in a temporary directory and cleaned up after completion
- Results are saved to the specified output directory
- For more information about NLM Phase 6, visit the NLM documentation
        """
        return readme
    
    def print_summary(self):
        """Print test summary."""
        print("\n" + "=" * 80)
        print("📊 NLM Phase 6 Test Summary")
        print("=" * 80)
        print(f"🧪 Total Tests: {self.total_tests}")
        print(f"✅ Passed: {self.passed_tests}")
        print(f"❌ Failed: {self.failed_tests}")
        print(f"📈 Success Rate: {(self.passed_tests / max(1, self.total_tests) * 100):.1f}%")
        
        if self.args.verbose:
            print("\n" + "=" * 80)
            print("📋 Detailed Test Results:")
            print("=" * 80)
            for test_name, result in self.results.get("results", {}).items():
                status = result.get("status", "UNKNOWN")
                duration = result.get("duration", 0)
                details = result.get("details", "")
                print(f"{test_name}: {status} ({duration:.2f}s)")
                if details:
                    print(f"  {details}")
        
        print("=" * 80)
    
    def cleanup(self):
        """Clean up temporary files."""
        if self.temp_dir and os.path.exists(self.temp_dir):
            print(f"🧹 Cleaning up temporary directory: {self.temp_dir}")
            shutil.rmtree(self.temp_dir)
            print("✅ Cleanup completed")


def main():
    """Main entry point for the test runner."""
    # Check if we're in the right directory
    if not os.path.exists("nlm_phase6_test_suite.py"):
        print("❌ Error: This script should be run from the nlm_phase6_test_suite.py directory")
        print("Please run the script from the correct directory or copy it to your desired location.")
        return 1
    
    # Create test runner and run tests
    runner = NLMPhase6TestRunner()
    
    try:
        success = runner.run()
        return 0 if success else 1
    except KeyboardInterrupt:
        print("\n⚠️  Tests interrupted by user")
        return 130
    except Exception as e:
        print(f"\n❌ Test runner failed: {e}")
        import traceback
        traceback.print_exc()
        return 1


if __name__ == "__main__":
    sys.exit(main())