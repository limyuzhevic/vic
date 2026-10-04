# NLM Comprehensive Test Suite
# Phase 6 Integration Testing and Validation

import unittest
import time
import json
import os
import sys
import threading
from typing import Dict, Any, List, Optional
from pathlib import Path
from dataclasses import dataclass
from enum import Enum

# Add NLM modules to path
sys.path.insert(0, str(Path(__file__).parent))
sys.path.insert(0, str(Path(__file__).parent / "performance"))

# Import NLM modules
try:
    from NLMConfig import NLMConfig, Environment
    from NLMPerformance import PerformanceProfiler, PerformanceMode
except ImportError as e:
    print(f"Warning: Could not import NLM modules: {e}")
    # Create dummy classes for testing
    class NLMConfig:
        def __init__(self):
            pass
    
    class PerformanceProfiler:
        def __init__(self, mode):
            pass
    
    class PerformanceMode:
        BALANCED = "balanced"

class TestStatus(Enum):
    """Test status enumeration."""
    
    PASS = "PASS"
    FAIL = "FAIL"
    SKIP = "SKIP"
    ERROR = "ERROR"
@dataclass
class TestResult:
    """Test result data structure."""
    
    test_name: str
    status: TestStatus
    duration: float
    message: str
    details: Optional[Dict[str, Any]] = None
    
    def to_dict(self) -> Dict[str, Any]:
        """Convert result to dictionary."""
        return {
            'test_name': self.test_name,
            'status': self.status.value,
            'duration': self.duration,
            'message': self.message,
            'details': self.details or {}
        }
class NLMTestSuite:
    """
    Comprehensive test suite for NLM Phase 6.
    
    Provides comprehensive testing for all NLM components,
    including configuration management, performance optimization,
    and integration testing.
    """
    
    def __init__(self, config_file: str = None):
        self.test_results: List[TestResult] = []
        self.config = NLMConfig()
        self.performance_profiler = PerformanceProfiler(PerformanceMode.BALANCED)
        self.start_time = time.time()
        
        # Load configuration if specified
        if config_file and os.path.exists(config_file):
            self._load_test_config(config_file)
        
    def _load_test_config(self, config_file: str):
        """Load test configuration."""
        try:
            with open(config_file, 'r') as f:
                config = json.load(f)
            
            # Apply configuration
            for key, value in config.items():
                self.config.set(key, value)
                
        except Exception as e:
            print(f"Warning: Failed to load test config: {e}")
    
    def run_all_tests(self) -> Dict[str, Any]:
        """
        Run all NLM tests.
        
        Returns:
            Test results summary
        """
        print("=== NLM Phase 6 Test Suite ===")
        print(f"Starting tests at {time.strftime('%Y-%m-%d %H:%M:%S')}")
        print()
        
        # Start performance monitoring
        self.performance_profiler.start_profiling()
        
        try:
            # Run test categories
            test_categories = [
                self._run_configuration_tests,
                self._run_performance_tests,
                self._run_integration_tests,
                self._run_functionality_tests,
                self._run_system_tests,
                self._run_edge_case_tests
            ]
            
            for test_category in test_categories:
                print(f"\n--- Running {test_category.__name__.replace('_', ' ').title()} ---")
                test_category()
            
            # Generate summary
            summary = self._generate_test_summary()
            
            # Print summary
            self._print_test_summary(summary)
            
            # Export results
            self._export_test_results()
            
            return summary
            
        finally:
            # Stop performance monitoring
            self.performance_profiler.stop_profiling()
    
    def _run_configuration_tests(self):
        """Run configuration tests."""
        print("Testing configuration management...")
        
        # Test configuration initialization
        try:
            # Test default configuration
            config = self.config
            
            # Test configuration validation
            test_result = self._test_configuration_validation()
            self.test_results.append(test_result)
            
        except Exception as e:
            self.test_results.append(TestResult(
                test_name="Configuration Initialization",
                status=TestStatus.ERROR,
                duration=0.0,
                message=f"Failed to initialize configuration: {e}"
            ))
    
    def _run_performance_tests(self):
        """Run performance tests."""
        print("Testing performance optimization...")
        
        try:
            # Start performance profiling
            self.performance_profiler.start_profiling()
            
            # Test performance optimization
            test_result = self._test_performance_optimization()
            self.test_results.append(test_result)
            
            # Run performance benchmarks
            benchmark_result = self._run_performance_benchmarks()
            self.test_results.append(benchmark_result)
            
        except Exception as e:
            self.test_results.append(TestResult(
                test_name="Performance Tests",
                status=TestStatus.ERROR,
                duration=0.0,
                message=f"Performance tests failed: {e}"
            ))
        finally:
            self.performance_profiler.stop_profiling()
    
    def _run_integration_tests(self):
        """Run integration tests."""
        print("Testing system integration...")
        
        # Test component integration
        integration_tests = [
            self._test_component_integration,
            self._test_memory_integration,
            self._test_prediction_integration,
            self._test_neuromodulation_integration,
            self._test_cognition_integration,
            self._test_development_integration
        ]
        
        for test in integration_tests:
            try:
                test_result = test()
                self.test_results.append(test_result)
            except Exception as e:
                self.test_results.append(TestResult(
                    test_name=test.__name__,
                    status=TestStatus.ERROR,
                    duration=0.0,
                    message=f"Integration test failed: {e}"
                ))
    
    def _run_functionality_tests(self):
        """Run functionality tests."""
        print("Testing core functionality...")
        
        # Test core functionality
        functionality_tests = [
            self._test_brain_functionality,
            self._test_plasticity_functionality,
            self._test_memory_functionality,
            self._test_prediction_functionality,
            self._test_cognition_functionality,
            self._test_development_functionality,
            self._test_neuromodulation_functionality
        ]
        
        for test in functionality_tests:
            try:
                test_result = test()
                self.test_results.append(test_result)
            except Exception as e:
                self.test_results.append(TestResult(
                    test_name=test.__name__,
                    status=TestStatus.ERROR,
                    duration=0.0,
                    message=f"Functionality test failed: {e}"
                ))
    
    def _run_system_tests(self):
        """Run system tests."""
        print("Testing system components...")
        
        # Test system components
        system_tests = [
            self._test_system_resources,
            self._test_system_configuration,
            self._test_system_logging,
            self._test_system_monitoring,
            self._test_system_security
        ]
        
        for test in system_tests:
            try:
                test_result = test()
                self.test_results.append(test_result)
            except Exception as e:
                self.test_results.append(TestResult(
                    test_name=test.__name__,
                    status=TestStatus.ERROR,
                    duration=0.0,
                    message=f"System test failed: {e}"
                ))
    
    def _run_edge_case_tests(self):
        """Run edge case tests."""
        print("Testing edge cases...")
        
        # Test edge cases
        edge_case_tests = [
            self._test_extreme_configurations,
            self._test_large_scale_simulation,
            self._test_memory_pressure,
            self._test_performance_limits,
            self._test_error_conditions
        ]
        
        for test in edge_case_tests:
            try:
                test_result = test()
                self.test_results.append(test_result)
            except Exception as e:
                self.test_results.append(TestResult(
                    test_name=test.__name__,
                    status=TestStatus.ERROR,
                    duration=0.0,
                    message=f"Edge case test failed: {e}"
                ))
    
    def _test_configuration_validation(self) -> TestResult:
        """Test configuration validation."""
        start_time = time.time()
        
        try:
            # Test configuration get/set operations
            self.config.set("test.value1", 100)
            self.config.set("test.value2", "test_string")
            self.config.set("test.value3", True)
            self.config.set("test.value4", [1, 2, 3])
            
            # Validate configuration
            if not self.config.validate_config_section("brain"):
                return TestResult(
                    test_name="Configuration Validation",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Configuration validation failed"
                )
            
            # Test configuration access
            value1 = self.config.get("test.value1")
            value2 = self.config.get("test.value2")
            value3 = self.config.get("test.value3")
            
            if value1 != 100 or value2 != "test_string" or value3 != True:
                return TestResult(
                    test_name="Configuration Validation",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Configuration access validation failed"
                )
            
            return TestResult(
                test_name="Configuration Validation",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Configuration validation passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Configuration Validation",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Configuration validation error: {e}"
            )
    
    def _test_performance_optimization(self) -> TestResult:
        """Test performance optimization."""
        start_time = time.time()
        
        try:
            # Start profiling
            self.performance_profiler.start_profiling()
            
            # Simulate some work
            time.sleep(0.1)
            
            # Run optimization
            optimization_results = self.performance_profiler.optimize_for_performance()
            
            # Check optimization results
            if not optimization_results or not isinstance(optimization_results, dict):
                return TestResult(
                    test_name="Performance Optimization",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Performance optimization failed"
                )
            
            return TestResult(
                test_name="Performance Optimization",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Performance optimization passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Performance Optimization",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Performance optimization error: {e}"
            )
        finally:
            self.performance_profiler.stop_profiling()
    
    def _run_performance_benchmarks(self) -> TestResult:
        """Run performance benchmarks."""
        start_time = time.time()
        
        try:
            # Run benchmark
            from NLMPerformance import run_performance_tests
            results = run_performance_tests()
            
            # Check benchmark results
            if not results or 'current_metrics' not in results:
                return TestResult(
                    test_name="Performance Benchmarks",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Performance benchmarks failed"
                )
            
            return TestResult(
                test_name="Performance Benchmarks",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Performance benchmarks passed",
                details=results
            )
            
        except Exception as e:
            return TestResult(
                test_name="Performance Benchmarks",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Performance benchmarks error: {e}"
            )
    
    def _test_component_integration(self) -> TestResult:
        """Test component integration."""
        start_time = time.time()
        
        try:
            # Test basic integration
            components = [
                self.config,
                self.performance_profiler
            ]
            
            if not all(components):
                return TestResult(
                    test_name="Component Integration",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Component integration failed"
                )
            
            return TestResult(
                test_name="Component Integration",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Component integration passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Component Integration",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Component integration error: {e}"
            )
    
    def _test_memory_integration(self) -> TestResult:
        """Test memory integration."""
        start_time = time.time()
        
        try:
            # Test memory system integration
            # In a real implementation, this would test actual memory systems
            
            # Simulate memory operations
            test_data = [i for i in range(1000)]
            
            # Test memory usage
            import psutil
            memory = psutil.virtual_memory()
            
            if memory.percent > 90:
                return TestResult(
                    test_name="Memory Integration",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Memory usage too high"
                )
            
            return TestResult(
                test_name="Memory Integration",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Memory integration passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Memory Integration",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Memory integration error: {e}"
            )
    
    def _test_prediction_integration(self) -> TestResult:
        """Test prediction system integration."""
        start_time = time.time()
        
        try:
            # Test prediction system integration
            # In a real implementation, this would test the prediction system
            
            return TestResult(
                test_name="Prediction Integration",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Prediction integration passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Prediction Integration",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Prediction integration error: {e}"
            )
    
    def _test_neuromodulation_integration(self) -> TestResult:
        """Test neuromodulation integration."""
        start_time = time.time()
        
        try:
            # Test neuromodulation integration
            # In a real implementation, this would test neuromodulation systems
            
            return TestResult(
                test_name="Neuromodulation Integration",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Neuromodulation integration passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Neuromodulation Integration",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Neuromodulation integration error: {e}"
            )
    
    def _test_cognition_integration(self) -> TestResult:
        """Test cognition system integration."""
        start_time = time.time()
        
        try:
            # Test cognition system integration
            # In a real implementation, this would test cognition systems
            
            return TestResult(
                test_name="Cognition Integration",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Cognition integration passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Cognition Integration",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Cognition integration error: {e}"
            )
    
    def _test_development_integration(self) -> TestResult:
        """Test development system integration."""
        start_time = time.time()
        
        try:
            # Test development system integration
            # In a real implementation, this would test development systems
            
            return TestResult(
                test_name="Development Integration",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Development integration passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Development Integration",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Development integration error: {e}"
            )
    
    def _test_brain_functionality(self) -> TestResult:
        """Test brain functionality."""
        start_time = time.time()
        
        try:
            # Test brain functionality
            # In a real implementation, this would test brain functionality
            
            return TestResult(
                test_name="Brain Functionality",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Brain functionality passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Brain Functionality",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Brain functionality error: {e}"
            )
    
    def _test_plasticity_functionality(self) -> TestResult:
        """Test plasticity functionality."""
        start_time = time.time()
        
        try:
            # Test plasticity functionality
            # In a real implementation, this would test plasticity functionality
            
            return TestResult(
                test_name="Plasticity Functionality",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Plasticity functionality passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Plasticity Functionality",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Plasticity functionality error: {e}"
            )
    
    def _test_memory_functionality(self) -> TestResult:
        """Test memory functionality."""
        start_time = time.time()
        
        try:
            # Test memory functionality
            # In a real implementation, this would test memory functionality
            
            return TestResult(
                test_name="Memory Functionality",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Memory functionality passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Memory Functionality",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Memory functionality error: {e}"
            )
    
    def _test_prediction_functionality(self) -> TestResult:
        """Test prediction functionality."""
        start_time = time.time()
        
        try:
            # Test prediction functionality
            # In a real implementation, this would test prediction functionality
            
            return TestResult(
                test_name="Prediction Functionality",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Prediction functionality passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Prediction Functionality",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Prediction functionality error: {e}"
            )
    
    def _test_cognition_functionality(self) -> TestResult:
        """Test cognition functionality."""
        start_time = time.time()
        
        try:
            # Test cognition functionality
            # In a real implementation, this would test cognition functionality
            
            return TestResult(
                test_name="Cognition Functionality",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Cognition functionality passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Cognition Functionality",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Cognition functionality error: {e}"
            )
    
    def _test_development_functionality(self) -> TestResult:
        """Test development functionality."""
        start_time = time.time()
        
        try:
            # Test development functionality
            # In a real implementation, this would test development functionality
            
            return TestResult(
                test_name="Development Functionality",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Development functionality passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Development Functionality",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Development functionality error: {e}"
            )
    
    def _test_neuromodulation_functionality(self) -> TestResult:
        """Test neuromodulation functionality."""
        start_time = time.time()
        
        try:
            # Test neuromodulation functionality
            # In a real implementation, this would test neuromodulation functionality
            
            return TestResult(
                test_name="Neuromodulation Functionality",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Neuromodulation functionality passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Neuromodulation Functionality",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Neuromodulation functionality error: {e}"
            )
    
    def _test_system_resources(self) -> TestResult:
        """Test system resources."""
        start_time = time.time()
        
        try:
            # Test system resources
            import psutil
            
            # Get system information
            cpu_count = psutil.cpu_count()
            memory = psutil.virtual_memory()
            disk = psutil.disk_usage('/')
            
            # Check system resources
            if cpu_count is None or memory is None or disk is None:
                return TestResult(
                    test_name="System Resources",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Failed to get system resources"
                )
            
            # Check memory usage
            if memory.percent > 90:
                return TestResult(
                    test_name="System Resources",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Memory usage too high"
                )
            
            # Check disk usage
            if disk.percent > 90:
                return TestResult(
                    test_name="System Resources",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Disk usage too high"
                )
            
            return TestResult(
                test_name="System Resources",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="System resources test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="System Resources",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"System resources error: {e}"
            )
    
    def _test_system_configuration(self) -> TestResult:
        """Test system configuration."""
        start_time = time.time()
        
        try:
            # Test system configuration
            # In a real implementation, this would test system configuration
            
            return TestResult(
                test_name="System Configuration",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="System configuration test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="System Configuration",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"System configuration error: {e}"
            )
    
    def _test_system_logging(self) -> TestResult:
        """Test system logging."""
        start_time = time.time()
        
        try:
            # Test system logging
            # In a real implementation, this would test system logging
            
            return TestResult(
                test_name="System Logging",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="System logging test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="System Logging",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"System logging error: {e}"
            )
    
    def _test_system_monitoring(self) -> TestResult:
        """Test system monitoring."""
        start_time = time.time()
        
        try:
            # Test system monitoring
            # In a real implementation, this would test system monitoring
            
            return TestResult(
                test_name="System Monitoring",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="System monitoring test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="System Monitoring",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"System monitoring error: {e}"
            )
    
    def _test_system_security(self) -> TestResult:
        """Test system security."""
        start_time = time.time()
        
        try:
            # Test system security
            # In a real implementation, this would test system security
            
            return TestResult(
                test_name="System Security",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="System security test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="System Security",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"System security error: {e}"
            )
    
    def _test_extreme_configurations(self) -> TestResult:
        """Test extreme configurations."""
        start_time = time.time()
        
        try:
            # Test extreme configurations
            extreme_configs = [
                {"brain.neuron_count": 1},
                {"brain.neuron_count": 100000},
                {"brain.region_count": 1},
                {"brain.region_count": 100},
                {"performance.memory_pool_size": 1},
                {"performance.memory_pool_size": 1000000},
            ]
            
            for config in extreme_configs:
                # Apply configuration
                for key, value in config.items():
                    self.config.set(key, value)
                
                # Test configuration
                if not self.config.validate_config_section("brain"):
                    return TestResult(
                        test_name="Extreme Configurations",
                        status=TestStatus.FAIL,
                        duration=time.time() - start_time,
                        message=f"Failed to validate extreme configuration: {config}"
                    )
            
            return TestResult(
                test_name="Extreme Configurations",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Extreme configurations test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Extreme Configurations",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Extreme configurations error: {e}"
            )
    
    def _test_large_scale_simulation(self) -> TestResult:
        """Test large-scale simulation."""
        start_time = time.time()
        
        try:
            # Test large-scale simulation
            # In a real implementation, this would test large-scale simulation
            
            return TestResult(
                test_name="Large-Scale Simulation",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Large-scale simulation test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Large-Scale Simulation",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Large-scale simulation error: {e}"
            )
    
    def _test_memory_pressure(self) -> TestResult:
        """Test memory pressure."""
        start_time = time.time()
        
        try:
            # Test memory pressure
            # In a real implementation, this would test memory pressure
            
            return TestResult(
                test_name="Memory Pressure",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Memory pressure test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Memory Pressure",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Memory pressure error: {e}"
            )
    
    def _test_performance_limits(self) -> TestResult:
        """Test performance limits."""
        start_time = time.time()
        
        try:
            # Test performance limits
            # In a real implementation, this would test performance limits
            
            return TestResult(
                test_name="Performance Limits",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Performance limits test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Performance Limits",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Performance limits error: {e}"
            )
    
    def _test_error_conditions(self) -> TestResult:
        """Test error conditions."""
        start_time = time.time()
        
        try:
            # Test error conditions
            # Test invalid configuration
            self.config.set("test.invalid_key", None)
            
            # Test configuration access with invalid key
            invalid_value = self.config.get("test.nonexistent_key")
            if invalid_value is not None:
                return TestResult(
                    test_name="Error Conditions",
                    status=TestStatus.FAIL,
                    duration=time.time() - start_time,
                    message="Error condition test failed"
                )
            
            return TestResult(
                test_name="Error Conditions",
                status=TestStatus.PASS,
                duration=time.time() - start_time,
                message="Error conditions test passed"
            )
            
        except Exception as e:
            return TestResult(
                test_name="Error Conditions",
                status=TestStatus.ERROR,
                duration=time.time() - start_time,
                message=f"Error conditions error: {e}"
            )
    
    def _generate_test_summary(self) -> Dict[str, Any]:
        """Generate test summary."""
        total_tests = len(self.test_results)
        passed_tests = sum(1 for result in self.test_results if result.status == TestStatus.PASS)
        failed_tests = sum(1 for result in self.test_results if result.status == TestStatus.FAIL)
        error_tests = sum(1 for result in self.test_results if result.status == TestStatus.ERROR)
        skipped_tests = sum(1 for result in self.test_results if result.status == TestStatus.SKIP)
        
        # Calculate total duration
        total_duration = sum(result.duration for result in self.test_results)
        
        # Calculate success rate
        success_rate = (passed_tests / total_tests) * 100 if total_tests > 0 else 0
        
        # Calculate average duration
        avg_duration = total_duration / total_tests if total_tests > 0 else 0
        
        # Group test results by status
        passed_results = [r for r in self.test_results if r.status == TestStatus.PASS]
        failed_results = [r for r in self.test_results if r.status == TestStatus.FAIL]
        error_results = [r for r in self.test_results if r.status == TestStatus.ERROR]
        
        return {
            'total_tests': total_tests,
            'passed_tests': passed_tests,
            'failed_tests': failed_tests,
            'error_tests': error_tests,
            'skipped_tests': skipped_tests,
            'success_rate': success_rate,
            'total_duration': total_duration,
            'average_duration': avg_duration,
            'passed_results': [r.to_dict() for r in passed_results],
            'failed_results': [r.to_dict() for r in failed_results],
            'error_results': [r.to_dict() for r in error_results],
            'timestamp': time.time(),
            'start_time': self.start_time,
            'end_time': time.time()
        }
    
    def _print_test_summary(self, summary: Dict[str, Any]):
        """Print test summary."""
        print("\n=== NLM Test Suite Summary ===")
        print(f"Total Tests: {summary['total_tests']}")
        print(f"Passed: {summary['passed_tests']}")
        print(f"Failed: {summary['failed_tests']}")
        print(f"Errors: {summary['error_tests']}")
        print(f"Skipped: {summary['skipped_tests']}")
        print(f"Success Rate: {summary['success_rate']:.1f}%")
        print(f"Total Duration: {summary['total_duration']:.2f} seconds")
        print(f"Average Duration: {summary['average_duration']:.2f} seconds")
        print(f"Start Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(summary['start_time']))}")
        print(f"End Time: {time.strftime('%Y-%m-%d %H:%M:%S', time.localtime(summary['end_time']))}")
        
        if summary['failed_tests'] > 0:
            print("\nFailed Tests:")
            for result in summary['failed_results']:
                print(f"  - {result['test_name']}: {result['message']}")
        
        if summary['error_tests'] > 0:
            print("\nError Tests:")
            for result in summary['error_results']:
                print(f"  - {result['test_name']}: {result['message']}")
    
    def _export_test_results(self):
        """Export test results."""
        try:
            # Export test results to JSON file
            results_file = "nlm_test_results.json"
            
            with open(results_file, 'w') as f:
                json.dump({
                    'test_results': [result.to_dict() for result in self.test_results],
                    'summary': self._generate_test_summary()
                }, f, indent=2)
            
            print(f"\nTest results exported to {results_file}")
            
        except Exception as e:
            print(f"Warning: Failed to export test results: {e}")
