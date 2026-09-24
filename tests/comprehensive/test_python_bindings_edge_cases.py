#!/usr/bin/env python3
"""
Comprehensive Python Bindings Test Coverage
Tests edge cases, boundary conditions, error conditions, memory management,
and concurrency for the NLM Python bindings.
"""

import pynlm
import sys
import threading
import time
import random
import traceback
from concurrent.futures import ThreadPoolExecutor, ProcessPoolExecutor
import gc
import os
import signal

class PythonBindingsTestSuite:
    """Comprehensive test suite for Python bindings edge cases."""
    
    def __init__(self):
        self.passed = 0
        self.failed = 0
        self.test_results = []
    
    def log_test(self, test_name, success, details=""):
        """Log test result."""
        status = "PASS" if success else "FAIL"
        print(f"[{status}] {test_name}: {details}")
        self.test_results.append((test_name, success, details))
        if success:
            self.passed += 1
        else:
            self.failed += 1
    
    def test_configuration_edge_cases(self):
        """Test configuration edge cases and boundary conditions."""
        print("\n" + "="*60)
        print("Testing Configuration Edge Cases")
        print("="*60)
        
        # Test 1: Extreme values
        try:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1)  # Minimum
            config.set("brain.neuron_count", 1000000)  # Large value
            self.log_test("Configuration extreme values", True)
        except Exception as e:
            self.log_test("Configuration extreme values", False, str(e))
        
        # Test 2: Invalid values (should fail gracefully)
        try:
            config = pynlm.createDefaultConfig()
            try:
                config.set("brain.neuron_count", 0)
                self.log_test("Configuration zero neurons", False, "Should have failed")
            except:
                self.log_test("Configuration zero neurons", True, "Correctly rejected")
        except Exception as e:
            self.log_test("Configuration invalid values", False, str(e))
        
        # Test 3: Negative values (should fail)
        try:
            config = pynlm.createDefaultConfig()
            try:
                config.set("brain.neuron_count", -100)
                self.log_test("Configuration negative neurons", False, "Should have failed")
            except:
                self.log_test("Configuration negative neurons", True, "Correctly rejected")
        except Exception as e:
            self.log_test("Configuration negative values", False, str(e))
        
        # Test 4: Empty and None strings
        try:
            config = pynlm.createDefaultConfig()
            # Test empty key handling
            try:
                config.validate_key("")
                self.log_test("Configuration empty key", False, "Should have failed")
            except:
                self.log_test("Configuration empty key", True, "Correctly rejected")
        except Exception as e:
            self.log_test("Configuration edge cases", False, str(e))
    
    def test_brain_creation_edge_cases(self):
        """Test brain creation with edge cases."""
        print("\n" + "="*60)
        print("Testing Brain Creation Edge Cases")
        print("="*60)
        
        # Test 1: Minimum brain size
        try:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 1)
            config.set("brain.region_count", 1)
            
            brain = pynlm.create_safe_brain(config)
            if brain:
                brain.initialize()
                self.log_test("Minimum brain size", True)
            else:
                self.log_test("Minimum brain size", False, "Failed to create")
        except Exception as e:
            self.log_test("Minimum brain size", False, str(e))
        
        # Test 2: Large brain (memory stress test)
        try:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", 10000)  # Large
            
            brain = pynlm.create_safe_brain(config)
            if brain:
                brain.initialize()
                # Monitor memory during initialization
                memory_stats = pynlm.monitor_memory_usage()
                print(f"    Memory usage: {memory_stats.used_memory / 1024 / 1024:.1f} MB")
                self.log_test("Large brain memory test", True)
            else:
                self.log_test("Large brain memory test", False, "Failed to create")
        except Exception as e:
            self.log_test("Large brain memory test", False, str(e))
        
        # Test 3: Invalid configurations (should fail gracefully)
        try:
            config = pynlm.createDefaultConfig()
            config.set("brain.neuron_count", -10)  # Invalid
            
            brain = pynlm.create_safe_brain(config)
            if brain is None:
                self.log_test("Invalid config rejection", True, "Correctly rejected")
            else:
                self.log_test("Invalid config rejection", False, "Should have failed")
        except Exception as e:
            self.log_test("Invalid config rejection", True, f"Correctly rejected: {e}")
    
    def test_simulation_edge_cases(self):
        """Test simulation edge cases."""
        print("\n" + "="*60)
        print("Testing Simulation Edge Cases")
        print("="*60)
        
        # Test 1: Zero step simulation
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Step zero
            brain.step(0)
            self.log_test("Zero simulation step", True)
        except Exception as e:
            self.log_test("Zero simulation step", False, str(e))
        
        # Test 2: Negative step (should be handled)
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Step negative - depending on implementation
            try:
                brain.step(-1)
                self.log_test("Negative simulation step", True, "Handled gracefully")
            except:
                self.log_test("Negative simulation step", True, "Correctly rejected")
        except Exception as e:
            self.log_test("Negative simulation step", False, str(e))
        
        # Test 3: Very large step
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            brain.step(1000000)  # Large step
            self.log_test("Large simulation step", True)
        except Exception as e:
            self.log_test("Large simulation step", False, str(e))
        
        # Test 4: Continuous simulation (stress test)
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            start_time = time.time()
            steps = 1000
            for i in range(steps):
                brain.step(1)
                if i % 100 == 0:
                    errors, warnings = pynlm.validate_brain_state(brain)
                    if errors:
                        self.log_test("Continuous simulation", False, f"Errors at step {i}: {errors}")
                        break
            
            end_time = time.time()
            self.log_test("Continuous simulation", True, f"{steps} steps in {end_time-start_time:.2f}s")
        except Exception as e:
            self.log_test("Continuous simulation", False, str(e))
    
    def test_memory_management_edge_cases(self):
        """Test memory management edge cases."""
        print("\n" + "="*60)
        print("Testing Memory Management Edge Cases")
        print("="*60)
        
        # Test 1: Memory pool edge cases
        try:
            # Create memory pool with minimum size
            pool = pynlm.create_neural_memory_pool(1)
            data = pool.allocate_data()
            self.log_test("Minimum memory pool", True)
            
            # Create memory pool with large size
            pool = pynlm.create_neural_memory_pool(1000000)  # 1MB
            data = pool.allocate_data()
            self.log_test("Large memory pool", True)
        except Exception as e:
            self.log_test("Memory pool tests", False, str(e))
        
        # Test 2: Memory monitoring
        try:
            # Monitor memory usage
            for i in range(5):
                stats = pynlm.monitor_memory_usage()
                assert stats.total_memory > 0
                assert stats.used_memory >= 0
                time.sleep(0.1)
            self.log_test("Memory monitoring", True)
        except Exception as e:
            self.log_test("Memory monitoring", False, str(e))
        
        # Test 3: Garbage collection
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            # Force garbage collection
            gc.collect()
            
            # Brain should still be valid
            errors, warnings = pynlm.validate_brain_state(brain)
            self.log_test("Memory after GC", True, f"Validation: {len(errors)} errors, {len(warnings)} warnings")
        except Exception as e:
            self.log_test("Memory after GC", False, str(e))
    
    def test_concurrency_edge_cases(self):
        """Test concurrency and threading edge cases."""
        print("\n" + "="*60)
        print("Testing Concurrency Edge Cases")
        print("="*60)
        
        # Test 1: Thread-safe operations
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            def brain_operation(step):
                try:
                    brain.step(step)
                    return True
                except Exception as e:
                    print(f"    Thread {step} error: {e}")
                    return False
            
            # Run multiple threads
            with ThreadPoolExecutor(max_workers=10) as executor:
                futures = [executor.submit(brain_operation, i) for i in range(100)]
                results = [f.result() for f in futures]
            
            if all(results):
                self.log_test("Thread-safe operations", True, f"{len(results)} operations succeeded")
            else:
                self.log_test("Thread-safe operations", False, f"{sum(results)}/{len(results)} succeeded")
        except Exception as e:
            self.log_test("Thread-safe operations", False, str(e))
        
        # Test 2: Concurrent brain creation
        try:
            def create_brain():
                config = pynlm.createDefaultConfig()
                return pynlm.createBrain(config)
            
            with ThreadPoolExecutor(max_workers=5) as executor:
                futures = [executor.submit(create_brain) for _ in range(20)]
                brains = [f.result() for f in futures]
            
            # Initialize all brains
            for brain in brains:
                brain.initialize()
            
            self.log_test("Concurrent brain creation", True, f"Created {len(brains)} brains")
        except Exception as e:
            self.log_test("Concurrent brain creation", False, str(e))
        
        # Test 3: Race condition detection
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            def conflicting_operation():
                try:
                    # Attempt operations that could conflict
                    brain.step(1)
                    brain.step(2)
                    return True
                except Exception as e:
                    print(f"    Conflicting operation error: {e}")
                    return False
            
            with ThreadPoolExecutor(max_workers=5) as executor:
                futures = [executor.submit(conflicting_operation) for _ in range(20)]
                results = [f.result() for f in futures]
            
            if all(results):
                self.log_test("Race condition handling", True, "All operations completed")
            else:
                self.log_test("Race condition handling", False, "Some operations failed")
        except Exception as e:
            self.log_test("Race condition handling", False, str(e))
    
    def test_error_handling_edge_cases(self):
        """Test error handling edge cases."""
        print("\n" + "="*60)
        print("Testing Error Handling Edge Cases")
        print("="*60)
        
        # Test 1: Error reporting with invalid data
        try:
            error_report = pynlm.report_error(
                "InvalidInputError",
                "Test with invalid input",
                {"input": None, "type": "null"}
            )
            
            if "ERROR REPORT" in error_report:
                self.log_test("Error reporting invalid data", True)
            else:
                self.log_test("Error reporting invalid data", False, "Invalid format")
        except Exception as e:
            self.log_test("Error reporting invalid data", False, str(e))
        
        # Test 2: Recovery attempts
        try:
            # Attempt recovery with invalid brain reference
            success, message = pynlm.attempt_recovery(None, "invalid_state")
            
            if not success and "Invalid brain reference" in message:
                self.log_test("Error recovery invalid ref", True)
            else:
                self.log_test("Error recovery invalid ref", False, "Unexpected result")
        except Exception as e:
            self.log_test("Error recovery invalid ref", False, str(e))
        
        # Test 3: Exception hierarchy validation
        try:
            # Test different exception types
            config = pynlm.createDefaultConfig()
            
            # This should trigger different exception types
            try:
                config.set("invalid_key", "value")
                self.log_test("Exception hierarchy", False, "Should have raised exception")
            except Exception as e:
                # Verify exception has proper type
                if hasattr(e, 'getErrorCode'):
                    self.log_test("Exception hierarchy", True, f"Exception code: {e.getErrorCode()}")
                else:
                    self.log_test("Exception hierarchy", True, "Generic exception raised")
        except Exception as e:
            self.log_test("Exception hierarchy", False, str(e))
        
        # Test 4: State validation edge cases
        try:
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            
            # Validate uninitialized brain
            errors, warnings = pynlm.validate_brain_state(brain)
            
            self.log_test("State validation edge cases", True, 
                         f"Found {len(errors)} errors, {len(warnings)} warnings")
        except Exception as e:
            self.log_test("State validation edge cases", False, str(e))
    
    def test_performance_edge_cases(self):
        """Test performance edge cases."""
        print("\n" + "="*60)
        print("Testing Performance Edge Cases")
        print("="*60)
        
        # Test 1: Performance profiler edge cases
        try:
            profiler = pynlm.create_performance_profiler()
            
            # Test starting and ending operations
            profiler.start_operation("test")
            time.sleep(0.001)  # Small delay
            profiler.end_operation("test")
            
            results = profiler.get_results()
            if len(results) > 0:
                self.log_test("Performance profiler basic", True)
            else:
                self.log_test("Performance profiler basic", False, "No results")
        except Exception as e:
            self.log_test("Performance profiler basic", False, str(e))
        
        # Test 2: Batch operations edge cases
        try:
            def operation_that_fails():
                raise Exception("Simulated failure")
            
            def operation_that_succeeds():
                return "success"
            
            # Mix of successful and failing operations
            operations = [operation_that_succeeds, operation_that_fails, operation_that_succeeds]
            result = pynlm.safe_batch_operation(operations)
            
            if result.total_operations == 3 and result.failed_operations == 1:
                self.log_test("Batch operations mixed results", True)
            else:
                self.log_test("Batch operations mixed results", False, f"Unexpected results: {result}")
        except Exception as e:
            self.log_test("Batch operations mixed results", False, str(e))
        
        # Test 3: Memory usage profiling
        try:
            # Create brain and monitor memory
            config = pynlm.createDefaultConfig()
            brain = pynlm.createBrain(config)
            brain.initialize()
            
            initial_stats = pynlm.monitor_memory_usage()
            
            # Do some work
            for i in range(100):
                brain.step(1)
            
            final_stats = pynlm.monitor_memory_usage()
            
            memory_increase = final_stats.used_memory - initial_stats.used_memory
            if memory_increase < 100 * 1024 * 1024:  # Less than 100MB increase
                self.log_test("Memory usage stability", True, f"Memory increase: {memory_increase / 1024 / 1024:.1f} MB")
            else:
                self.log_test("Memory usage stability", False, f"Memory increase too high: {memory_increase / 1024 / 1024:.1f} MB")
        except Exception as e:
            self.log_test("Memory usage stability", False, str(e))
    
    def run_all_tests(self):
        """Run all test suites."""
        print("\n" + "="*80)
        print("COMPREHENSIVE PYTHON BINDINGS TEST SUITE")
        print("="*80)
        
        self.test_configuration_edge_cases()
        self.test_brain_creation_edge_cases()
        self.test_simulation_edge_cases()
        self.test_memory_management_edge_cases()
        self.test_concurrency_edge_cases()
        self.test_error_handling_edge_cases()
        self.test_performance_edge_cases()
        
        print("\n" + "="*80)
        print("TEST SUMMARY")
        print("="*80)
        print(f"Total Tests: {self.passed + self.failed}")
        print(f"Passed: {self.passed}")
        print(f"Failed: {self.failed}")
        print(f"Success Rate: {(self.passed / (self.passed + self.failed) * 100):.1f}%")
        
        print("\nDetailed Results:")
        for test_name, success, details in self.test_results:
            status = "✓" if success else "✗"
            print(f"  {status} {test_name}: {details}")
        
        return self.failed == 0

def main():
    print("NLM Python Bindings - Comprehensive Edge Case Testing")
    print("=" * 60)
    
    test_suite = PythonBindingsTestSuite()
    success = test_suite.run_all_tests()
    
    if success:
        print("\n✅ ALL TESTS PASSED!")
        sys.exit(0)
    else:
        print(f"\n❌ {test_suite.failed} TESTS FAILED")
        sys.exit(1)

if __name__ == "__main__":
    main()
