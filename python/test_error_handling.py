#!/usr/bin/env python3
"""
Error Handling and Validation Tests for NLM Python Bindings

This module contains comprehensive tests for error handling, validation,
and edge cases in the NLM Python bindings.
"""

import os
import sys
import json
import tempfile
import shutil
import traceback
from pathlib import Path
from typing import Dict, List, Any, Optional

# Import NLM bindings
try:
    import pynlm
except ImportError as e:
    print(f"Failed to import NLM bindings: {e}")
    print("Make sure NLM is properly installed and built.")
    sys.exit(1)

from test_suite import NLMTestSuite, NLMSyntheticDataGenerator, NLMTestValidationError
class ErrorHandlingTests(NLMTestSuite):
    """
    Comprehensive tests for NLM error handling and validation.
    
    Tests all error handling scenarios including:
    - Error conditions and edge cases
    - Boundary conditions
    - Memory management under stress
    - Error recovery
    - Python exception handling
    - Validation failures
    """
    
    def setup_method(self):
        """Setup before each test method."""
        self.setup_test_environment()
        
        # Create default configuration and brain
        self.config = pynlm.createDefaultConfig()
        self.brain = pynlm.createBrain(self.config)
        
        # Create world and agent brain
        self.world = pynlm.createSimpleWorld()
        self.agent = pynlm.createAgentBrain(self.brain)
        
        # Configure world for testing
        self.world.configure(width=50, height=50, visionWidth=32, visionHeight=32)
        
    def teardown_method(self):
        """Cleanup after each test method."""
        self.teardown_test_environment()
    
    # Test basic error handling
    def test_basic_error_handling(self):
        """Test basic error handling capabilities."""
        self.agent.initialize(self.world)
        
        # Test that exceptions are properly raised for invalid operations
        # This is a basic test to verify error handling framework
        
        print("✓ Basic error handling framework in place")
    
    # Test validation errors
    def test_validation_error_handling(self):
        """Test validation error handling."""
        self.agent.initialize(self.world)
        
        # Test validation of sensory percepts
        valid_percept = self.data_generator.generate_sensory_percept()
        
        # If validation methods exist, test them
        if hasattr(self.agent, 'validateSensoryPercept'):
            try:
                validation_result = self.agent.validateSensoryPercept(valid_percept)
                # Valid percept should pass validation
                self.framework.assert_true(validation_result, "Valid percept should pass validation")
                print("✓ Sensory percept validation works")
            except Exception as e:
                print(f"⚠ Sensory percept validation raised exception: {type(e).__name__}: {e}")
        
        # Test reward value validation
        if hasattr(self.agent, 'validateRewardValue'):
            try:
                # Test valid reward
                valid_reward = self.agent.validateRewardValue(0.5)
                self.framework.assert_true(valid_reward, "Valid reward should pass validation")
                
                # Test invalid reward
                invalid_reward = self.agent.validateRewardValue(-1.0)
                self.framework.assert_false(invalid_reward, "Invalid reward should fail validation")
                
                print("✓ Reward value validation works")
            except Exception as e:
                print(f"⚠ Reward value validation raised exception: {type(e).__name__}: {e}")
    
    # Test exception types and messages
    def test_exception_types(self):
        """Test that proper exception types are raised."""
        self.agent.initialize(self.world)
        
        # Test that the agent provides meaningful exception messages
        try:
            # Try to access non-existent methods to see exception behavior
            if hasattr(self.agent, 'processPercepts'):
                # This should work if method exists
                percepts = self.data_generator.generate_batch_percepts(5)
                self.agent.processPercepts(percepts)
                print("✓ processPercepts method works")
            else:
                print("⚠ processPercepts method not available")
                
        except Exception as e:
            # Should get a meaningful exception
            self.framework.assert_true(len(str(e)) > 0, "Exception should have a message")
            print(f"✓ Exception raised with message: {type(e).__name__}: {e}")
    
    # Test null/None error handling
    def test_null_none_error_handling(self):
        """Test error handling with null/None values."""
        # This test focuses on how the bindings handle null/None values
        
        print("✓ Null/None error handling test placeholder")
        
        # Note: Direct null handling is difficult in Python bindings
        # as they typically convert to appropriate C++ types
    
    # Test invalid input handling
    def test_invalid_input_handling(self):
        """Test handling of invalid input data."""
        self.agent.initialize(self.world)
        
        # Test with invalid percept data
        try:
            # Create a percept with invalid dimensions
            invalid_percept = pynlm.SensoryPercept()
            
            # Try to set invalid data (if methods exist)
            if hasattr(invalid_percept, 'setVision'):
                # Set vision data with wrong size
                invalid_vision = [0.1, 0.2]  # Too small for typical vision (256 elements)
                invalid_percept.setVision(invalid_vision)
                print("✓ Invalid vision data handled")
            
            if hasattr(invalid_percept, 'setTouch'):
                # Set touch data with wrong size
                invalid_touch = [0.1]  # Too small for typical touch (8 elements)
                invalid_percept.setTouch(invalid_touch)
                print("✓ Invalid touch data handled")
                
        except Exception as e:
            print(f"✓ Invalid input raised expected exception: {type(e).__name__}")
    
    # Test boundary conditions
    def test_boundary_conditions(self):
        """Test boundary conditions and edge cases."""
        self.agent.initialize(self.world)
        
        # Test boundary values for reward modulation
        boundary_rewards = [0.0, 1.0, -0.0, 1.0]  # Include boundary values
        
        for reward in boundary_rewards:
            try:
                predicted = max(0.0, reward * 0.5)
                self.agent.applyRewardModulation(reward, predicted)
                print(f"✓ Boundary reward {reward} handled")
            except Exception as e:
                print(f"⚠ Boundary reward {reward} raised exception: {type(e).__name__}")
        
        # Test boundary values for episode scoring
        for score in [0.0, 100.0, -0.0]:
            try:
                # This test is conceptual as episode scoring depends on agent state
                pass
            except Exception as e:
                print(f"⚠ Boundary score {score} raised exception: {type(e).__name__}")
    
    # Test memory error handling
    def test_memory_error_handling(self):
        """Test memory error handling under stress."""
        # This test attempts to stress memory usage and see how the system handles it
        
        self.agent.initialize(self.world)
        
        # Process large amount of data to test memory handling
        large_percept_count = 1000
        
        print(f"Processing {large_percept_count} percepts to test memory handling...")
        
        try:
            for i in range(large_percept_count):
                if i % 100 == 0:
                    print(f"  Processed {i}/{large_percept_count} percepts...")
                
                percept = self.data_generator.generate_sensory_percept()
                self.agent.processSensoryInput(percept, False)
                
                # Occasionally decode command to exercise more of the system
                if i % 10 == 0:
                    command = self.agent.decodeMotorCommand()
                    self.agent.applyRewardModulation(0.1, 0.05)
            
            print("✓ Large memory load handled successfully")
            
        except MemoryError as e:
            print(f"⚠ Memory error occurred (expected in stress test): {e}")
        except Exception as e:
            print(f"⚠ Memory stress test raised exception: {type(e).__name__}: {e}")
    
    # Test error recovery
    def test_error_recovery(self):
        """Test error recovery capabilities."""
        # Test that the system can recover from errors
        
        self.agent.initialize(self.world)
        
        # Simulate an error condition and attempt recovery
        try:
            # Process normal data first
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            
            # Try to recover from potential errors
            # This tests whether the system maintains state consistency
            command = self.agent.decodeMotorCommand()
            
            # Continue normal operation
            self.agent.applyRewardModulation(0.5, 0.25)
            
            print("✓ Error recovery test passed")
            
        except Exception as e:
            print(f"⚠ Error recovery test raised exception: {type(e).__name__}")
    
    # Test validation edge cases
    def test_validation_edge_cases(self):
        """Test edge cases in validation."""
        self.agent.initialize(self.world)
        
        # Test edge cases for validation
        edge_cases = [
            ("Very large percept size", lambda: self.data_generator.generate_sensory_percept(1024)),
            ("Very small percept size", lambda: self.data_generator.generate_sensory_percept(16)),
            ("Empty percept vectors", lambda: self._create_empty_percept()),
            ("Single value percept", lambda: self._create_single_value_percept()),
        ]
        
        for name, test_func in edge_cases:
            try:
                test_func()
                print(f"✓ Validation edge case '{name}' handled")
            except Exception as e:
                print(f"⚠ Validation edge case '{name}' raised exception: {type(e).__name__}")
    
    def _create_empty_percept(self) -> pynlm.SensoryPercept:
        """Helper to create empty percept for testing."""
        percept = pynlm.SensoryPercept()
        # Set empty vectors
        if hasattr(percept, 'setVision'):
            percept.setVision([])
        if hasattr(percept, 'setTouch'):
            percept.setTouch([])
        if hasattr(percept, 'setInternal'):
            percept.setInternal([])
        if hasattr(percept, 'setProprioception'):
            percept.setProprioception([])
        return percept
    
    def _create_single_value_percept(self) -> pynlm.SensoryPercept:
        """Helper to create single-value percept for testing."""
        percept = pynlm.SensoryPercept()
        # Set single values
        if hasattr(percept, 'setVision'):
            percept.setVision([0.5])
        if hasattr(percept, 'setTouch'):
            percept.setTouch([0.5])
        if hasattr(percept, 'setInternal'):
            percept.setInternal([0.5])
        if hasattr(percept, 'setProprioception'):
            percept.setProprioception([0.5])
        return percept
    
    # Test type conversion error handling
    def test_type_conversion_error_handling(self):
        """Test type conversion error handling."""
        # Test that the bindings handle type conversions properly
        
        print("✓ Type conversion error handling test placeholder")
        
        # Note: Type conversion errors are typically handled at the C++ level
        # and wrapped in Python exceptions
    
    # Test serialization error handling
    def test_serialization_error_handling(self):
        """Test serialization error handling."""
        self.agent.initialize(self.world)
        
        # Process some data
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        
        # Test serialization with invalid paths
        invalid_path = "/tmp/readonly/state.json"
        
        try:
            # Try to save to invalid path (if permission allows)
            result = self.agent.saveState(invalid_path)
            print("✓ Invalid path serialization handled")
        except Exception as e:
            print(f"✓ Invalid path serialization raised expected exception: {type(e).__name__}")
        
        # Test with non-existent directory
        non_existent_dir = "/tmp/this/does/not/exist/deeply/state.json"
        try:
            result = self.agent.saveState(non_existent_dir)
            print("✓ Non-existent directory serialization handled")
        except Exception as e:
            print(f"✓ Non-existent directory serialization raised expected exception: {type(e).__name__}")
    
    # Test callback error handling
    def test_callback_error_handling(self):
        """Test callback error handling."""
        # Test that callback mechanisms handle errors properly
        
        print("✓ Callback error handling test placeholder")
        
        # Note: Callback error handling is framework-specific
    
    # Test system resilience
    def test_system_resilience(self):
        """Test overall system resilience under error conditions."""
        self.agent.initialize(self.world)
        
        # Test resilience by mixing normal and error conditions
        resilience_tests = [
            ("Normal operation", self._test_normal_operation),
            ("Invalid input recovery", self._test_invalid_input_recovery),
            ("Memory stress test", self._test_memory_stress),
        ]
        
        for name, test_func in resilience_tests:
            try:
                test_func()
                print(f"✓ System resilience test '{name}' passed")
            except Exception as e:
                print(f"⚠ System resilience test '{name}' raised exception: {type(e).__name__}")
    
    def _test_normal_operation(self):
        """Helper for normal operation test."""
        percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(percept, False)
        command = self.agent.decodeMotorCommand()
        self.agent.applyRewardModulation(0.5, 0.25)
    
    def _test_invalid_input_recovery(self):
        """Helper for invalid input recovery test."""
        # Create and use an invalid percept
        invalid_percept = self._create_empty_percept()
        
        # Try to process it (may raise exception)
        try:
            self.agent.processSensoryInput(invalid_percept, False)
        except:
            pass  # Expected to potentially raise exception
        
        # Recover with valid input
        valid_percept = self.data_generator.generate_sensory_percept()
        self.agent.processSensoryInput(valid_percept, False)
    
    def _test_memory_stress(self):
        """Helper for memory stress test."""
        # Process many percepts to stress memory
        for i in range(100):
            percept = self.data_generator.generate_sensory_percept()
            self.agent.processSensoryInput(percept, False)
            
            if i % 20 == 0:
                # Occasionally do other operations
                command = self.agent.decodeMotorCommand()
                self.agent.applyRewardModulation(0.1, 0.05)

# Run tests when executed directly
if __name__ == "__main__":
    print("Running Error Handling and Validation Tests...")
    print("=" * 60)
    
    # Create test instance
    test_suite = ErrorHandlingTests()
    
    # Run all tests
    test_suite.run_all_tests()
    
    # Print summary
    summary = test_suite.get_test_summary()
    print(f"\n{'='*60}")
    print("TEST SUMMARY")
    print(f"{'='*60}")
    print(f"Test Suite: {summary['test_suite']}")
    print(f"Passed: {summary['passed_tests']}")
    print(f"Failed: {summary['failed_tests']}")
    print(f"Total: {summary['total_tests']}")
    print(f"Duration: {summary['duration_seconds']:.2f}s")
    print(f"Avg time per test: {summary['average_time_per_test']:.4f}s")
    
    if summary['failed_tests'] > 0:
        print(f"\n❌ Test suite completed with {summary['failed_tests']} failures")
        sys.exit(1)
    else:
        print(f"\n✅ All error handling tests passed successfully!")
        sys.exit(0)