#!/usr/bin/env python3

"""
NLM Enhanced Components Test Suite Runner

This module provides the main test runner for NLM enhanced components testing,
integrating all test suites and providing comprehensive reporting and analysis.
"""

import sys
import os
import argparse
import time
import json
import logging
from datetime import datetime
from typing import Dict, List, Any, Optional
from pathlib import Path

# Add the current directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_test_suite.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Import test suites
from tests.comprehensive.core.test_framework import NLMTestRunner, TestType
from tests.comprehensive.core.test_core import TestConfigurationEnhanced, TestBrainEnhanced, TestMemoryEnhanced, TestNeuromodulationEnhanced, TestErrorHandlingEnhanced
from tests.comprehensive.integration.test_integration import TestCrossComponentIntegration, TestPerformanceIntegration, TestCompatibilityIntegration, TestRealWorldScenarios
from tests.comprehensive.performance.test_performance import TestMemoryPerformance, TestBrainPerformance, TestNeuromodulationPerformance, TestMemoryStressTesting, TestRealTimePerformance, TestConcurrentPerformance
from tests.comprehensive.security.test_security import TestInputValidationSecurity, TestPluginSystemSecurity, TestMemorySafety, TestErrorRecovery, TestResourceManagement
from tests.comprehensive.cli.test_cli import TestCLIArgumentParsing, TestCLICommandExecution, TestCLICapabilityValidation, TestCLISecurityValidation
from tests.comprehensive.visualization.test_visualization import TestVisualizationRendering, TestVisualizationDataExport, TestVisualizationPerformance

class NLMTestSuiteRunner:
    """
    Comprehensive test suite runner for NLM enhanced components.
    
    This class provides advanced test execution capabilities including:
    - Integration of all test suites
    - Comprehensive test configuration
    - Performance benchmarking
    - Test result aggregation
    - Detailed reporting
    - CI/CD integration
    - Custom test selection
    """
    
    def __init__(self):
        self.test_runner = NLMTestRunner()
        self.test_suite_config = {}
        self.test_execution_results = {}
        self.performance_benchmarks = {}
        self.test_suite_metrics = {}
        
        # Initialize test registration
        self._initialize_test_registration()
    
    def _initialize_test_registration(self):
        """Initialize test registration with all test suites."""
        logger.info("Initializing test registration...")
        
        # Register core tests
        self._register_core_tests()
        
        # Register integration tests
        self._register_integration_tests()
        
        # Register performance tests
        self._register_performance_tests()
        
        # Register security tests
        self._register_security_tests()
        
        # Register CLI tests
        self._register_cli_tests()
        
        # Register visualization tests
        self._register_visualization_tests()
        
        # Register Python binding tests
        self._register_python_binding_tests()
    
    def _register_core_tests(self):
        """Register core component tests."""
        logger.info("Registering core component tests...")
        
        test_classes = [
            (TestConfigurationEnhanced, TestType.CONFIGURATION),
            (TestBrainEnhanced, TestType.BRAIN),
            (TestMemoryEnhanced, TestType.MEMORY),
            (TestNeuromodulationEnhanced, TestType.NEUROSIMULATION),
            (TestErrorHandlingEnhanced, TestType.ERROR_HANDLING),
        ]
        
        for test_class, test_type in test_classes:
            for method_name in dir(test_class):
                if method_name.startswith('test_'):
                    method = getattr(test_class, method_name)
                    self.test_runner.register_test(
                        method, 
                        test_type, 
                        test_name=f"{test_class.__name__}.{method_name}",
                        tags=["core", "unit", test_class.__name__.lower()]
                    )
    
    def _register_integration_tests(self):
        """Register integration tests."""
        logger.info("Registering integration tests...")
        
        test_classes = [
            (TestCrossComponentIntegration, TestType.INTEGRATION),
            (TestPerformanceIntegration, TestType.PERFORMANCE),
            (TestCompatibilityIntegration, TestType.COMPATIBILITY),
            (TestRealWorldScenarios, TestType.NEUROSIMULATION),
        ]
        
        for test_class, test_type in test_classes:
            for method_name in dir(test_class):
                if method_name.startswith('test_'):
                    method = getattr(test_class, method_name)
                    self.test_runner.register_test(
                        method,
                        test_type,
                        test_name=f"{test_class.__name__}.{method_name}",
                        tags=["integration", "system", test_class.__name__.lower()]
                    )
    
    def _register_performance_tests(self):
        """Register performance tests."""
        logger.info("Registering performance tests...")
        
        test_classes = [
            (TestMemoryPerformance, TestType.PERFORMANCE),
            (TestBrainPerformance, TestType.PERFORMANCE),
            (TestNeuromodulationPerformance, TestType.PERFORMANCE),
            (TestMemoryStressTesting, TestType.PERFORMANCE),
            (TestRealTimePerformance, TestType.PERFORMANCE),
            (TestConcurrentPerformance, TestType.CONCURRENT),
        ]
        
        for test_class, test_type in test_classes:
            for method_name in dir(test_class):
                if method_name.startswith('test_'):
                    method = getattr(test_class, method_name)
                    self.test_runner.register_test(
                        method,
                        test_type,
                        test_name=f"{test_class.__name__}.{method_name}",
                        tags=["performance", "benchmark", test_class.__name__.lower()]
                    )
    
    def _register_security_tests(self):
        """Register security tests."""
        logger.info("Registering security tests...")
        
        test_classes = [
            (TestInputValidationSecurity, TestType.SECURITY),
            (TestPluginSystemSecurity, TestType.PLUGIN),
            (TestMemorySafety, TestType.SECURITY),
            (TestErrorRecovery, TestType.ERROR_HANDLING),
            (TestResourceManagement, TestType.SECURITY),
        ]
        
        for test_class, test_type in test_classes:
            for method_name in dir(test_class):
                if method_name.startswith('test_'):
                    method = getattr(test_class, method_name)
                    self.test_runner.register_test(
                        method,
                        test_type,
                        test_name=f"{test_class.__name__}.{method_name}",
                        tags=["security", "validation", test_class.__name__.lower()]
                    )
    
    def _register_cli_tests(self):
        """Register CLI tests."""
        logger.info("Registering CLI tests...")
        
        test_classes = [
            (TestCLIArgumentParsing, TestType.CLI),
            (TestCLICommandExecution, TestType.CLI),
            (TestCLICapabilityValidation, TestType.CLI),
            (TestCLISecurityValidation, TestType.SECURITY),
        ]
        
        for test_class, test_type in test_classes:
            for method_name in dir(test_class):
                if method_name.startswith('test_'):
                    method = getattr(test_class, method_name)
                    self.test_runner.register_test(
                        method,
                        test_type,
                        test_name=f"{test_class.__name__}.{method_name}",
                        tags=["cli", "command_line", test_class.__name__.lower()]
                    )
    
    def _register_visualization_tests(self):
        """Register visualization tests."""
        logger.info("Registering visualization tests...")
        
        test_classes = [
            (TestVisualizationRendering, TestType.VISUAL),
            (TestVisualizationDataExport, TestType.VISUAL),
            (TestVisualizationPerformance, TestType.VISUAL),
        ]
        
        for test_class, test_type in test_classes:
            for method_name in dir(test_class):
                if method_name.startswith('test_'):
                    method = getattr(test_class, method_name)
                    self.test_runner.register_test(
                        method,
                        test_type,
                        test_name=f"{test_class.__name__}.{method_name}",
                        tags=["visualization", "rendering", test_class.__name__.lower()]
                    )
    
    def _register_python_binding_tests(self):
        """Register Python binding tests."""
        logger.info("Registering Python binding tests...")
        
        # Python binding tests would be registered here
        # For now, we'll create placeholder registrations
        
        def dummy_python_binding_test():
            """Dummy test for Python bindings."""
            from pynlm import Config, create_default_config
            config = create_default_config()
            assert isinstance(config, Config)
            return True
        
        self.test_runner.register_test(
            dummy_python_binding_test,
            TestType.CONFIGURATION,
            test_name="Python_Binding_Tests.Compatibility_Test",
            tags=["python_bindings", "integration", "compatibility"]
        )
        
        def dummy_python_memory_test():
            """Dummy test for Python memory bindings."""
            from pynlm.memory import WorkingMemory, EpisodicMemory, AssociativeMemory
            assert WorkingMemory is not None
            assert EpisodicMemory is not None
            assert AssociativeMemory is not None
            return True
        
        self.test_runner.register_test(
            dummy_python_memory_test,
            TestType.MEMORY,
            test_name="Python_Binding_Tests.Memory_Tests",
            tags=["python_bindings", "memory", "memory_system"]
        )
    
    def configure_test_suite(self, config_file: Optional[str] = None):
        """
        Configure the test suite with configuration options.
        
        Args:
            config_file: Path to configuration file (JSON/YAML)
        """
        if config_file and os.path.exists(config_file):
            # Load configuration from file
            if config_file.endswith('.json'):
                with open(config_file, 'r') as f:
                    self.test_suite_config = json.load(f)
            elif config_file.endswith(('.yml', '.yaml')):
                import yaml
                with open(config_file, 'r') as f:
                    self.test_suite_config = yaml.safe_load(f)
        
        # Set default configuration if not provided
        if not self.test_suite_config:
            self.test_suite_config = {
                "test_filter": [],
                "parallel_execution": True,
                "max_workers": 4,
                "verbose_output": True,
                "performance_benchmarking": True,
                "security_testing": True,
                "integration_testing": True,
                "compatibility_testing": True,
                "generate_reports": True,
                "export_results": True,
                "report_format": "json",
                "performance_metrics": True,
                "resource_monitoring": True
            }
        
        logger.info(f"Test suite configured with: {self.test_suite_config}")
    
    def run_test_suite(self, test_filter: Optional[List[TestType]] = None,
                      config_file: Optional[str] = None,
                      output_file: Optional[str] = None) -> Dict[str, Any]:
        """
        Run the comprehensive NLM test suite.
        
        Args:
            test_filter: Optional list of test types to run
            config_file: Optional configuration file path
            output_file: Optional output file path for results
            
        Returns:
            Dictionary containing test suite results and metrics
        """
        start_time = time.time()
        start_datetime = datetime.now()
        
        # Configure test suite if not already configured
        if config_file or not self.test_suite_config:
            self.configure_test_suite(config_file)
        
        logger.info("Starting NLM Enhanced Components Test Suite")
        logger.info(f"Test filter: {test_filter or 'All tests'}")
        logger.info(f"Parallel execution: {self.test_suite_config.get('parallel_execution', True)}")
        logger.info(f"Max workers: {self.test_suite_config.get('max_workers', 4)}")
        
        # Run tests
        config = self.test_suite_config.get('test_filter', test_filter)
        parallel = self.test_suite_config.get('parallel_execution', True)
        max_workers = self.test_suite_config.get('max_workers', 4)
        
        test_metrics = self.test_runner.run_all_tests(
            test_filter=config,
            max_workers=max_workers,
            parallel=parallel
        )
        
        end_time = time.time()
        end_datetime = datetime.now()
        
        # Calculate total execution time
        total_duration = end_time - start_time
        
        # Generate comprehensive report
        test_results = self.test_runner.get_test_results()
        suite_metrics = self.test_runner.get_suite_metrics()
        
        # Prepare comprehensive results
        results = {
            "test_suite_info": {
                "name": "NLM Enhanced Components Test Suite",
                "version": "1.0.0",
                "description": "Comprehensive testing suite for NLM enhanced components",
                "start_time": start_datetime.isoformat(),
                "end_time": end_datetime.isoformat(),
                "total_duration_seconds": total_duration,
                "configuration": self.test_suite_config
            },
            "test_metrics": {
                "total_tests": suite_metrics.total_tests if suite_metrics else 0,
                "passed_tests": suite_metrics.passed_tests if suite_metrics else 0,
                "failed_tests": suite_metrics.failed_tests if suite_metrics else 0,
                "skipped_tests": suite_metrics.skipped_tests if suite_metrics else 0,
                "pass_rate": suite_metrics.pass_rate if suite_metrics else 0.0,
                "performance_benchmarks": suite_metrics.performance_benchmarks if suite_metrics else {}
            },
            "test_results": [],
            "performance_benchmarks": self.test_runner.performance_benchmarks,
            "test_categories": {}
        }
        
        # Add detailed test results
        for test_result in test_results:
            results["test_results"].append({
                "test_name": test_result.test_name,
                "test_type": test_result.test_type,
                "status": test_result.status,
                "duration": test_result.duration,
                "memory_usage": test_result.memory_usage,
                "performance_score": test_result.performance_score,
                "error": test_result.error_message,
                "timestamp": test_result.timestamp.isoformat(),
                "tags": test_result.tags
            })
        
        # Add performance benchmarks
        if self.test_suite_config.get('performance_benchmarking', True):
            performance_results = self._run_performance_benchmarks()
            results["performance_benchmarks"].update(performance_results)
        
        # Add test category analysis
        test_categories = self._analyze_test_categories(test_results)
        results["test_categories"] = test_categories
        
        # Generate output file if specified
        if output_file:
            self._write_results(output_file, results)
        
        # Generate additional reports
        if self.test_suite_config.get('generate_reports', True):
            self._generate_additional_reports(results)
        
        # Log completion summary
        logger.info("NLM Enhanced Components Test Suite completed")
        logger.info(f"Total tests: {results['test_metrics']['total_tests']}")
        logger.info(f"Passed: {results['test_metrics']['passed_tests']}")
        logger.info(f"Failed: {results['test_metrics']['failed_tests']}")
        logger.info(f"Pass rate: {results['test_metrics']['pass_rate']:.1f}%")
        logger.info(f"Total duration: {total_duration:.2f}s")
        
        return results
    
    def _run_performance_benchmarks(self) -> Dict[str, Any]:
        """Run performance benchmarks."""
        benchmarks = {}
        
        # Run system performance benchmark
        system_start = time.time()
        
        # Simulate system operations
        config = create_default_config()
        
        with Brain(config) as brain:
            working_mem = WorkingMemory(brain)
            neuromodulation = NeuromodulationSystem(brain)
            
            for i in range(100):
                brain.step(i)
                working_mem.store([0.1, 0.2, 0.3], strength=0.5)
                neuromodulation.apply_dopamine(reward=0.5)
                neuromodulation.apply_curiosity(novelty=0.3)
                working_mem.update(0.01)
                neuromodulation.update(0.01)
        
        system_end = time.time()
        system_duration = system_end - system_start
        
        benchmarks["system_performance"] = {
            "duration_seconds": system_duration,
            "operations_per_second": 500 / system_duration if system_duration > 0 else 0,
            "timestamp": datetime.now().isoformat()
        }
        
        return benchmarks
    
    def _analyze_test_categories(self, test_results: List[Any]) -> Dict[str, Any]:
        """Analyze test results by category."""
        categories = {}
        
        for test_result in test_results:
            test_type = test_result.test_type
            if test_type not in categories:
                categories[test_type] = {
                    "total": 0,
                    "passed": 0,
                    "failed": 0,
                    "skipped": 0,
                    "pass_rate": 0.0
                }
            
            category = categories[test_type]
            category["total"] += 1
            
            if test_result.status == "PASSED":
                category["passed"] += 1
            elif test_result.status == "FAILED":
                category["failed"] += 1
            elif test_result.status == "SKIPPED":
                category["skipped"] += 1
            
            # Calculate pass rate
            if category["total"] > 0:
                category["pass_rate"] = (category["passed"] / category["total"]) * 100
        
        return categories
    
    def _write_results(self, output_file: str, results: Dict[str, Any]):
        """Write results to file."""
        try:
            with open(output_file, 'w') as f:
                json.dump(results, f, indent=2, default=str)
            
            logger.info(f"Results written to: {output_file}")
        except Exception as e:
            logger.error(f"Failed to write results to {output_file}: {e}")
    
    def _generate_additional_reports(self, results: Dict[str, Any]):
        """Generate additional reports."""
        timestamp = datetime.now().strftime('%Y%m%d_%H%M%S')
        
        # Generate HTML report
        html_report = self._generate_html_report(results)
        html_file = f"nlm_test_report_{timestamp}.html"
        try:
            with open(html_file, 'w') as f:
                f.write(html_report)
            logger.info(f"HTML report written to: {html_file}")
        except Exception as e:
            logger.error(f"Failed to write HTML report: {e}")
        
        # Generate summary report
        summary_report = self._generate_summary_report(results)
        summary_file = f"nlm_test_summary_{timestamp}.txt"
        try:
            with open(summary_file, 'w') as f:
                f.write(summary_report)
            logger.info(f"Summary report written to: {summary_file}")
        except Exception as e:
            logger.error(f"Failed to write summary report: {e}")
    
    def _generate_html_report(self, results: Dict[str, Any]) -> str:
        """Generate HTML report."""
        html = """
        <!DOCTYPE html>
        <html>
        <head>
            <title>NLM Enhanced Components Test Report</title>
            <style>
                body { font-family: Arial, sans-serif; margin: 20px; }
                .header { background-color: #f0f0f0; padding: 20px; border-radius: 5px; }
                .metrics { display: grid; grid-template-columns: repeat(4, 1fr); gap: 10px; }
                .metric { background-color: #e9ecef; padding: 15px; border-radius: 3px; }
                .metric h3 { margin: 0; color: #495057; }
                .metric p { margin: 5px 0; font-size: 24px; font-weight: bold; }
                .test-results { margin-top: 20px; }
                .test-result { border: 1px solid #ddd; padding: 10px; margin: 5px 0; border-radius: 3px; }
                .passed { background-color: #d4edda; border-color: #c3e6cb; }
                .failed { background-color: #f8d7da; border-color: #f5c6cb; }
                .skipped { background-color: #fff3cd; border-color: #ffeaa7; }
            </style>
        </head>
        <body>
            <div class="header">
                <h1>NLM Enhanced Components Test Report</h1>
                <p>Generated on: {}</p>
                <p>Total Tests: {} | Passed: {} | Failed: {} | Pass Rate: {:.1f}%</p>
            </div>
            
            <h2>Test Metrics</h2>
            <div class="metrics">
                <div class="metric">
                    <h3>Total Tests</h3>
                    <p>{}</p>
                </div>
                <div class="metric">
                    <h3>Passed</h3>
                    <p style="color: green;">{}</p>
                </div>
                <div class="metric">
                    <h3>Failed</h3>
                    <p style="color: red;">{}</p>
                </div>
                <div class="metric">
                    <h3>Pass Rate</h3>
                    <p style="color: #007bff;">{:.1f}%</p>
                </div>
            </div>
            
            <h2>Test Categories</h2>
            <table border="1" style="width: 100%; border-collapse: collapse;">
                <tr>
                    <th>Test Type</th>
                    <th>Total</th>
                    <th>Passed</th>
                    <th>Failed</th>
                    <th>Skipped</th>
                    <th>Pass Rate</th>
                </tr>
                {}
            </table>
            
            <h2>Detailed Test Results</h2>
            <div class="test-results">
                {}
            </div>
            
            <h2>Performance Benchmarks</h2>
            <pre>{}</pre>
        </body>
        </html>
        """.format(
            results["test_suite_info"]["start_time"],
            results["test_metrics"]["total_tests"],
            results["test_metrics"]["passed_tests"],
            results["test_metrics"]["failed_tests"],
            results["test_metrics"]["pass_rate"],
            results["test_metrics"]["total_tests"],
            results["test_metrics"]["passed_tests"],
            results["test_metrics"]["failed_tests"],
            results["test_metrics"]["pass_rate"],
            # Test categories table
            "\n".join([f"<tr><td>{k}</td><td>{v['total']}</td><td>{v['passed']}</td><td>{v['failed']}</td><td>{v['skipped']}</td><td>{v['pass_rate']:.1f}%</td></tr>" 
                      for k, v in results["test_categories"].items()]),
            # Test results
            "\n".join([f'<div class="test-result {result["status"].lower()}"><strong>{result["test_name"]}</strong> ({result["test_type"]}) - {result["status"]} ({result["duration"]:.3f}s)</div>' 
                      for result in results["test_results"]]),
            # Performance benchmarks
            json.dumps(results["performance_benchmarks"], indent=2)
        )
        
        return html
    
    def _generate_summary_report(self, results: Dict[str, Any]) -> str:
        """Generate summary report."""
        summary = f"""
NLM Enhanced Components Test Suite Summary
=========================================
Generated: {results["test_suite_info"]["start_time"]}
Duration: {results["test_suite_info"]["total_duration_seconds"]:.2f} seconds

Test Metrics:
- Total Tests: {results["test_metrics"]["total_tests"]}
- Passed: {results["test_metrics"]["passed_tests"]}
- Failed: {results["test_metrics"]["failed_tests"]}
- Skipped: {results["test_metrics"]["skipped_tests"]}
- Pass Rate: {results["test_metrics"]["pass_rate"]:.1f}%

Test Categories:
"""
        
        for category, stats in results["test_categories"].items():
            summary += f"- {category}: {stats['total']} total, {stats['passed']} passed, {stats['failed']} failed, {stats['skipped']} skipped ({stats['pass_rate']:.1f}%)
"
        
        summary += f"\nDetailed Test Results:
"
        
        for test_result in results["test_results"]:
            status_icon = "✓" if test_result["status"] == "PASSED" else "✗"
            summary += f"{status_icon} {test_result['test_name']} ({test_result['test_type']}) - {test_result['duration']:.3f}s - {test_result['status']}
"
        
        return summary

# Main execution function
def main():
    """Main function for executing the NLM test suite."""
    parser = argparse.ArgumentParser(
        description="NLM Enhanced Components Test Suite",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog="""
Examples:
  python test_runner.py --all                    # Run all tests
  python test_runner.py --filter core integration   # Run specific test categories
  python test_runner.py --config test_config.json    # Use custom configuration
  python test_runner.py --output results.json        # Save results to file
  python test_runner.py --parallel false             # Run tests sequentially
        """
    )
    
    parser.add_argument(
        "--filter", 
        nargs="+", 
        choices=[t.value for t in TestType],
        help="Filter tests by category"
    )
    
    parser.add_argument(
        "--config",
        help="Path to configuration file"
    )
    
    parser.add_argument(
        "--output",
        help="Path to output file for results"
    )
    
    parser.add_argument(
        "--parallel",
        action="store_true",
        default=True,
        help="Enable parallel test execution"
    )
    
    parser.add_argument(
        "--sequential",
        action="store_false",
        dest="parallel",
        help="Disable parallel test execution"
    )
    
    parser.add_argument(
        "--max-workers",
        type=int,
        default=4,
        help="Maximum number of worker threads for parallel execution"
    )
    
    parser.add_argument(
        "--verbose",
        action="store_true",
        help="Enable verbose output"
    )
    
    parser.add_argument(
        "--version",
        action="version",
        version="NLM Enhanced Components Test Suite 1.0.0"
    )
    
    args = parser.parse_args()
    
    # Create test suite runner
    runner = NLMTestSuiteRunner()
    
    # Configure test suite
    config_file = args.config
    runner.configure_test_suite(config_file)
    
    # Override configuration from command line
    if args.parallel is not None:
        runner.test_suite_config["parallel_execution"] = args.parallel
    
    runner.test_suite_config["max_workers"] = args.max_workers
    runner.test_suite_config["verbose_output"] = args.verbose
    
    # Parse test filter
    test_filter = None
    if args.filter:
        test_filter = [TestType(f.lower()) for f in args.filter]
    
    # Run test suite
    try:
        results = runner.run_test_suite(
            test_filter=test_filter,
            config_file=config_file,
            output_file=args.output
        )
        
        # Print summary
        print("\n" + "="*80)
        print("NLM ENHANCED COMPONENTS TEST SUITE EXECUTION COMPLETE")
        print("="*80)
        print(f"Total Tests: {results['test_metrics']['total_tests']}")
        print(f"Passed: {results['test_metrics']['passed_tests']}")
        print(f"Failed: {results['test_metrics']['failed_tests']}")
        print(f"Pass Rate: {results['test_metrics']['pass_rate']:.1f}%")
        print(f"Total Duration: {results['test_suite_info']['total_duration_seconds']:.2f}s")
        print("="*80)
        
        # Exit with appropriate code
        if results['test_metrics']['failed_tests'] == 0:
            print("SUCCESS: All tests passed!")
            sys.exit(0)
        else:
            print("FAILURE: Some tests failed.")
            sys.exit(1)
            
    except Exception as e:
        logger.error(f"Test suite execution failed: {e}")
        print(f"ERROR: Test suite execution failed: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()
