#!/usr/bin/env python3

"""
Core Testing Framework for NLM Python Bindings

This module provides the testing framework for NLM (Neural Learning Machine) Python bindings,
encompassing comprehensive test coverage for all enhanced components including unit tests,
integration tests, performance tests, security tests, and compatibility tests.

Features:
- Comprehensive test runner with reporting
- Unit test framework for all components
- Integration test suite for cross-component functionality
- Performance benchmarking
- Security and reliability testing
- Coverage analysis
- Test automation tools
- CI/CD integration support
- Property-based testing capabilities
- Advanced testing utilities
- Visual testing support
- Memory testing
- Plugin testing
- Cross-platform testing
"""

import time
import unittest
import sys
import os
import json
import logging
import tempfile
import threading
import multiprocessing
from typing import Dict, List, Any, Optional, Callable, Union
from datetime import datetime
from dataclasses import dataclass, field
from enum import Enum
import traceback
import math
import random
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor

# Optional imports for testing tools
try:
    import numpy as np
    NUMPY_AVAILABLE = True
except ImportError:
    NUMPY_AVAILABLE = False
    np = None

try:
    import pandas as pd
    PANDAS_AVAILABLE = True
except ImportError:
    PANDAS_AVAILABLE = False
    pd = None

try:
    import matplotlib.pyplot as plt
    MATPLOTLIB_AVAILABLE = True
except ImportError:
    MATPLOTLIB_AVAILABLE = False
    plt = None

# Setup logging for test framework
logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(name)s - %(levelname)s - %(message)s',
    handlers=[
        logging.FileHandler('nlm_test_framework.log'),
        logging.StreamHandler()
    ]
)
logger = logging.getLogger(__name__)

# Test result dataclasses
@dataclass
class TestResult:
    """Test execution result with comprehensive metrics."""
    test_name: str
    test_type: str
    status: str  # PASSED, FAILED, SKIPPED
    duration: float
    memory_usage: float
    performance_score: float
    error_message: Optional[str] = None
    error_trace: Optional[str] = None
    timestamp: datetime = field(default_factory=datetime.now)
    metadata: Dict[str, Any] = field(default_factory=dict)
    tags: List[str] = field(default_factory=list)

@dataclass
class PerformanceMetrics:
    """Performance measurement metrics."""
    cpu_usage: float
    memory_usage: float
    execution_time: float
    throughput: float
    peak_memory: float
    avg_memory: float
    min_response_time: float
    max_response_time: float
    median_response_time: float

@dataclass
class TestSuiteMetrics:
    """Test suite execution metrics."""
    total_tests: int
    passed_tests: int
    failed_tests: int
    skipped_tests: int
    total_duration: float
    total_memory_usage: float
    average_performance_score: float
    pass_rate: float
    performance_benchmarks: Dict[str, Any]
    start_time: datetime = field(default_factory=datetime.now)
    end_time: Optional[datetime] = None

class TestStatus(Enum):
    """Test execution status."""
    PASSED = "PASSED"
    FAILED = "FAILED"
    SKIPPED = "SKIPPED"
    ERROR = "ERROR"

class TestType(Enum):
    """Test category types."""
    UNIT = "unit"
    INTEGRATION = "integration"
    PERFORMANCE = "performance"
    SECURITY = "security"
    COMPATIBILITY = "compatibility"
    VISUAL = "visual"
    MEMORY = "memory"
    PLUGIN = "plugin"
    CLI = "cli"
    CONFIGURATION = "configuration"
    ERROR_HANDLING = "error_handling"
    NEUROSIMULATION = "neurosimulation"
    CONCURRENT = "concurrent"
    PROPERTY_BASED = "property_based"

class NLMTestRunner:
    """
    Comprehensive test runner for NLM Python bindings.
    
    This class provides advanced test execution capabilities including:
    - Test categorization and filtering
    - Performance benchmarking
    - Resource monitoring
    - Parallel execution
    - Test result aggregation
    - Reporting and visualization
    """
    
    def __init__(self):
        self.test_results: List[TestResult] = []
        self.suite_metrics: Optional[TestSuiteMetrics] = None
        self.test_registry: Dict[str, List[Any]] = {}
        self.performance_benchmarks: Dict[str, Any] = {}
        self.test_config: Dict[str, Any] = {}
        
        # Initialize test registry
        self._initialize_test_registry()
    
    def _initialize_test_registry(self):
        """Initialize the test registry with test categories."""
        self.test_registry = {
            'unit_tests': [],
            'integration_tests': [],
            'performance_tests': [],
            'security_tests': [],
            'compatibility_tests': [],
            'visual_tests': [],
            'memory_tests': [],
            'plugin_tests': [],
            'cli_tests': [],
            'configuration_tests': [],
            'error_handling_tests': [],
            'neurosimulation_tests': [],
            'concurrent_tests': [],
            'property_based_tests': []
        }
    
    def register_test(self, test_func: Callable, test_type: TestType, 
                     test_name: Optional[str] = None, tags: Optional[List[str]] = None):
        """
        Register a test function with the test runner.
        
        Args:
            test_func: The test function to register
            test_type: Category of the test
            test_name: Optional name for the test (defaults to function name)
            tags: Optional tags for categorization
        """
        name = test_name or test_func.__name__
        if tags is None:
            tags = []
        
        test_info = {
            'function': test_func,
            'type': test_type,
            'name': name,
            'tags': tags,
            'registered_at': datetime.now()
        }
        
        category = f"{test_type.value}_tests"
        if category in self.test_registry:
            self.test_registry[category].append(test_info)
        else:
            self.test_registry[test_type.value] = [test_info]
        
        logger.info(f"Registered test: {name} ({test_type.value})")
    
    def run_all_tests(self, test_filter: Optional[List[TestType]] = None,
                     max_workers: int = 4, parallel: bool = False) -> TestSuiteMetrics:
        """
        Run all registered tests with comprehensive reporting.
        
        Args:
            test_filter: Optional list of test types to run
            max_workers: Maximum number of workers for parallel execution
            parallel: Whether to run tests in parallel
            
        Returns:
            TestSuiteMetrics containing comprehensive execution metrics
        """
        start_time = datetime.now()
        
        # Determine which tests to run
        test_categories = []
        if test_filter:
            for test_type in test_filter:
                category = f"{test_type.value}_tests"
                if category in self.test_registry:
                    test_categories.extend(self.test_registry[category])
        else:
            # Run all tests
            for category in self.test_registry.values():
                test_categories.extend(category)
        
        logger.info(f"Running {len(test_categories)} tests")
        
        if parallel and len(test_categories) > 1:
            results = self._run_parallel_tests(test_categories, max_workers)
        else:
            results = self._run_sequential_tests(test_categories)
        
        end_time = datetime.now()
        
        # Calculate suite metrics
        total_duration = (end_time - start_time).total_seconds()
        
        suite_metrics = TestSuiteMetrics(
            total_tests=len(test_categories),
            passed_tests=sum(1 for r in results if r.status == TestStatus.PASSED),
            failed_tests=sum(1 for r in results if r.status == TestStatus.FAILED),
            skipped_tests=sum(1 for r in results if r.status == TestStatus.SKIPPED),
            total_duration=total_duration,
            total_memory_usage=sum(r.memory_usage for r in results),
            average_performance_score=sum(r.performance_score for r in results) / len(results) if results else 0.0,
            pass_rate=(sum(1 for r in results if r.status == TestStatus.PASSED) / len(results) * 100) if results else 0.0,
            performance_benchmarks=self.performance_benchmarks,
            start_time=start_time,
            end_time=end_time
        )
        
        self.suite_metrics = suite_metrics
        self.test_results = results
        
        # Generate comprehensive report
        self._generate_test_report()
        
        return suite_metrics
    
    def _run_sequential_tests(self, test_categories: List[Dict]) -> List[TestResult]:
        """Run tests sequentially."""
        results = []
        
        for test_info in test_categories:
            result = self._execute_single_test(test_info)
            results.append(result)
            
            # Log progress
            status = "✓" if result.status == TestStatus.PASSED else "✗"
            logger.info(f"{status} {test_info['name']} - {result.duration:.3f}s")
        
        return results
    
    def _run_parallel_tests(self, test_categories: List[Dict], max_workers: int) -> List[TestResult]:
        """Run tests in parallel using ThreadPoolExecutor."""
        results = []
        results_lock = threading.Lock()
        
        def run_single_test(test_info):
            result = self._execute_single_test(test_info)
            with results_lock:
                results.append(result)
            
            # Log progress
            status = "✓" if result.status == TestStatus.PASSED else "✗"
            logger.info(f"{status} {test_info['name']} - {result.duration:.3f}s")
        
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = [executor.submit(run_single_test, test_info) for test_info in test_categories]
            for future in futures:
                future.result()  # Wait for completion and propagate exceptions
        
        return results
    
    def _execute_single_test(self, test_info: Dict) -> TestResult:
        """Execute a single test with comprehensive metrics collection."""
        test_func = test_info['function']
        test_name = test_info['name']
        test_type = test_info['type']
        
        start_time = time.time()
        
        # Monitor memory usage
        import resource
        initial_memory = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        
        status = TestStatus.ERROR
        error_message = None
        error_trace = None
        performance_score = 0.0
        
        try:
            # Execute the test
            if test_func():
                status = TestStatus.PASSED
                performance_score = self._calculate_performance_score(test_name, test_type)
            else:
                status = TestStatus.FAILED
                error_message = "Test returned False"
        except Exception as e:
            status = TestStatus.FAILED
            error_message = str(e)
            error_trace = traceback.format_exc()
        
        end_time = time.time()
        duration = end_time - start_time
        
        # Calculate final metrics
        final_memory = resource.getrusage(resource.RUSAGE_SELF).ru_maxrss
        memory_usage = final_memory - initial_memory
        
        return TestResult(
            test_name=test_name,
            test_type=test_type.value,
            status=status.value,
            duration=duration,
            memory_usage=memory_usage,
            performance_score=performance_score,
            error_message=error_message,
            error_trace=error_trace,
            metadata={'test_info': test_info}
        )
    
    def _calculate_performance_score(self, test_name: str, test_type: TestType) -> float:
        """Calculate performance score for a test."""
        # Base performance score
        base_score = 1.0
        
        # Adjust based on test type
        type_adjustments = {
            TestType.UNIT: 1.0,
            TestType.INTEGRATION: 0.8,
            TestType.PERFORMANCE: 0.9,
            TestType.SECURITY: 0.7,
            TestType.COMPATIBILITY: 0.85,
            TestType.VISUAL: 0.6,
            TestType.MEMORY: 0.7,
            TestType.PLUGIN: 0.8,
            TestType.CLI: 0.9,
            TestType.CONFIGURATION: 0.8,
            TestType.ERROR_HANDLING: 0.9,
            TestType.NEUROSIMULATION: 0.7,
            TestType.CONCURRENT: 0.6,
            TestType.PROPERTY_BASED: 0.8
        }
        
        adjustment = type_adjustments.get(test_type, 1.0)
        
        # Special handling for performance tests
        if test_type == TestType.PERFORMANCE:
            adjustment = 1.2  # Performance tests get higher scores for good performance
        
        return base_score * adjustment
    
    def _generate_test_report(self):
        """Generate comprehensive test report."""
        if not self.test_results:
            logger.warning("No test results to report")
            return
        
        # Generate summary report
        report = {
            'summary': {
                'total_tests': self.suite_metrics.total_tests,
                'passed_tests': self.suite_metrics.passed_tests,
                'failed_tests': self.suite_metrics.failed_tests,
                'skipped_tests': self.suite_metrics.skipped_tests,
                'pass_rate': self.suite_metrics.pass_rate,
                'total_duration': self.suite_metrics.total_duration,
                'total_memory_usage': self.suite_metrics.total_memory_usage,
                'average_performance_score': self.suite_metrics.average_performance_score
            },
            'test_details': [],
            'performance_benchmarks': self.performance_benchmarks,
            'test_categories': {}
        }
        
        # Add test details
        for result in self.test_results:
            test_detail = {
                'name': result.test_name,
                'type': result.test_type,
                'status': result.status,
                'duration': result.duration,
                'memory_usage': result.memory_usage,
                'performance_score': result.performance_score,
                'error': result.error_message,
                'timestamp': result.timestamp.isoformat()
            }
            report['test_details'].append(test_detail)
        
        # Group tests by category
        for test_type in TestType:
            category_key = f"{test_type.value}_tests"
            if category_key in self.test_registry:
                report['test_categories'][test_type.value] = {
                    'total': len(self.test_registry[category_key]),
                    'passed': sum(1 for r in self.test_results 
                                if r.test_type == test_type.value and r.status == TestStatus.PASSED.value),
                    'failed': sum(1 for r in self.test_results 
                                if r.test_type == test_type.value and r.status == TestStatus.FAILED.value),
                    'skipped': sum(1 for r in self.test_results 
                                 if r.test_type == test_type.value and r.status == TestStatus.SKIPPED.value)
                }
        
        # Write report to file
        report_path = f"nlm_test_report_{datetime.now().strftime('%Y%m%d_%H%M%S')}.json"
        with open(report_path, 'w') as f:
            json.dump(report, f, indent=2, default=str)
        
        logger.info(f"Test report written to: {report_path}")
        
        # Print summary
        print("\n" + "="*80)
        print("NLM TEST SUITE EXECUTION SUMMARY")
        print("="*80)
        print(f"Total Tests: {self.suite_metrics.total_tests}")
        print(f"Passed: {self.suite_metrics.passed_tests}")
        print(f"Failed: {self.suite_metrics.failed_tests}")
        print(f"Skipped: {self.suite_metrics.skipped_tests}")
        print(f"Pass Rate: {self.suite_metrics.pass_rate:.1f}%")
        print(f"Total Duration: {self.suite_metrics.total_duration:.2f}s")
        print(f"Average Performance Score: {self.suite_metrics.average_performance_score:.2f}")
        print("="*80)
    
    def add_performance_benchmark(self, benchmark_name: str, metrics: PerformanceMetrics):
        """Add performance benchmark results."""
        self.performance_benchmarks[benchmark_name] = {
            'cpu_usage': metrics.cpu_usage,
            'memory_usage': metrics.memory_usage,
            'execution_time': metrics.execution_time,
            'throughput': metrics.throughput,
            'peak_memory': metrics.peak_memory,
            'avg_memory': metrics.avg_memory,
            'min_response_time': metrics.min_response_time,
            'max_response_time': metrics.max_response_time,
            'median_response_time': metrics.median_response_time,
            'timestamp': datetime.now().isoformat()
        }
    
    def get_test_results(self) -> List[TestResult]:
        """Get all test results."""
        return self.test_results
    
    def get_suite_metrics(self) -> Optional[TestSuiteMetrics]:
        """Get suite metrics."""
        return self.suite_metrics

# Import all test modules
try:
    from . import test_core
    from . import test_memory
    from . import test_neuromodulation
    from . import test_config
    from . import test_cli
    from . import test_visualization
    from . import test_plugin_system
    from . import test_python_bindings
    from . import test_performance
    from . import test_security
    from . import test_comprehensive_integration
except ImportError as e:
    logger.warning(f"Could not import test modules: {e}")

# Global test runner instance
_test_runner = NLMTestRunner()

def get_test_runner() -> NLMTestRunner:
    """Get the global test runner instance."""
    return _test_runner

if __name__ == "__main__":
    # Run comprehensive test suite
    test_runner = get_test_runner()
    
    # Run tests with filtering (can be customized)
    test_filter = [
        TestType.UNIT,
        TestType.INTEGRATION,
        TestType.PERFORMANCE,
        TestType.SECURITY,
        TestType.COMPATIBILITY
    ]
    
    metrics = test_runner.run_all_tests(test_filter=test_filter, parallel=True)
    
    # Exit with appropriate code
    sys.exit(0 if metrics.failed_tests == 0 else 1)
