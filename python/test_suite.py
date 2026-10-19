#!/usr/bin/env python3
"""
NLM Python Bindings Comprehensive Test Suite

This module provides a comprehensive test framework for the enhanced NLM Python bindings.
It includes unit tests, integration tests, performance tests, and edge case tests.
"""

import os
import sys
import time
import json
import tempfile
import shutil
from pathlib import Path
from typing import Dict, List, Any, Optional, Union, Callable
import numpy as np

# Import NLM bindings
try:
    import pynlm
except ImportError as e:
    print(f"Failed to import NLM bindings: {e}")
    print("Make sure NLM is properly installed and built.")
    sys.exit(1)

# Test configuration
class TestConfig:
    """Configuration for NLM tests."""
    
    # Test constants
    DEFAULT_CONFIG_FILE = "test_config.json"
    DEFAULT_CHECKPOINT_DIR = "test_checkpoints"
    DEFAULT_LOG_FILE = "nlm_test.log"
    
    # Performance test settings
    PERFORMANCE_TIMEOUT = 60.0  # seconds
    PERFORMANCE_SAMPLE_COUNT = 10
    
    # Memory test settings
    MEMORY_SAMPLES = 20
    MEMORY_THRESHOLD_MB = 500  # MB
    
    # Integration test settings
    MAX_INTEGRATION_STEPS = 1000
    MAX_INTEGRATION_EPISODES = 50
    
    # Edge case test settings
    MAX_NEURON_COUNT = 10000
    MAX_SYNAPSE_COUNT = 50000

# Test exceptions
class NLMTestError(Exception):
    """Base exception for NLM test errors."""
    pass

class NLMTestValidationError(NLMTestError):
    """Validation error in tests."""
    pass

class NLMTestPerformanceError(NLMTestError):
    """Performance-related test error."""
    pass

class NLMTestMemoryError(NLMTestError):
    """Memory-related test error."""
    pass

# Test utilities
class NLMSimpleTestFramework:
    """Simple test framework for NLM testing."""
    
    def __init__(self):
        self.passed_tests = 0
        self.failed_tests = 0
        self.skipped_tests = 0
        self.start_time = time.time()
        
    def assert_equal(self, actual: Any, expected: Any, message: str = ""):
        """Assert that actual equals expected."""
        if actual != expected:
            raise NLMTestValidationError(f"{message}: Expected {expected}, got {actual}")
        self.passed_tests += 1
        
    def assert_true(self, condition: bool, message: str = ""):
        """Assert that condition is True."""
        if not condition:
            raise NLMTestValidationError(f"{message}: Expected True, got False")
        self.passed_tests += 1
        
    def assert_false(self, condition: bool, message: str = ""):
        """Assert that condition is False."""
        if condition:
            raise NLMTestValidationError(f"{message}: Expected False, got True")
        self.passed_tests += 1
        
    def assert_raises(self, exception_type: type, func: Callable, *args, **kwargs):
        """Assert that a function raises a specific exception."""
        try:
            func(*args, **kwargs)
            raise NLMTestValidationError(f"Expected {exception_type.__name__} to be raised")
        except exception_type:
            self.passed_tests += 1
        except Exception as e:
            raise NLMTestValidationError(f"Expected {exception_type.__name__}, got {type(e).__name__}: {e}")
    
    def measure_performance(self, func: Callable, *args, **kwargs):
        """Measure performance of a function."""
        start_time = time.time()
        result = func(*args, **kwargs)
        end_time = time.time()
        return result, end_time - start_time
    
    def log_performance(self, test_name: str, duration: float, **metrics):
        """Log performance metrics."""
        print(f"[{test_name}] Performance: {duration:.4f}s")
        for key, value in metrics.items():
            print(f"  {key}: {value}")
    
    def cleanup(self):
        """Clean up test resources."""
        pass

# Test data generators
class NLMSyntheticDataGenerator:
    """Generate synthetic test data for NLM testing."""
    
    @staticmethod
    def generate_sensory_percept(size: int = 256) -> pynlm.SensoryPercept:
        """Generate synthetic sensory percept."""
        percept = pynlm.SensoryPercept()
        
        # Generate vision data (16x16 grid)
        vision_data = np.random.randn(size).astype(np.float32)
        percept.setVision(vision_data.tolist())
        
        # Generate touch data (8 sensors)
        touch_data = np.random.randn(8).astype(np.float32)
        percept.setTouch(touch_data.tolist())
        
        # Generate internal signals (4 signals)
        internal_data = np.random.randn(4).astype(np.float32)
        percept.setInternal(internal_data.tolist())
        
        # Generate proprioception data (6 values)
        proprio_data = np.random.randn(6).astype(np.float32)
        percept.setProprioception(proprio_data.tolist())
        
        return percept
    
    @staticmethod
    def generate_motor_commands(count: int = 10) -> List[pynlm.MotorCommand]:
        """Generate synthetic motor commands."""
        commands = []
        for _ in range(count):
            # Random motor command type
            cmd_type = np.random.randint(0, 7)
            action = pynlm.Action()
            action.setType(pynlm.ActionType(cmd_type))
            commands.append(action)
        return commands
    
    @staticmethod
    def generate_batch_percepts(count: int, percept_size: int = 256) -> List[pynlm.SensoryPercept]:
        """Generate batch of sensory percepts."""
        return [NLMSyntheticDataGenerator.generate_sensory_percept(percept_size) for _ in range(count)]
    
    @staticmethod
    def generate_config_data() -> Dict[str, Any]:
        """Generate configuration data."""
        return {
            "neuronConfig": {
                "defaultLifNeurons": 1000,
                "defaultConnectionStrength": 0.1,
                "defaultExcitatory": True,
                "defaultDevelopmentalStage": 3
            },
            "plasticityConfig": {
                "stdpLearningRate": 0.01,
                "hebbianLearningRate": 0.005,
                "structuralPlasticityEnabled": True
            },
            "memoryConfig": {
                "workingMemoryCapacity": 1000,
                "episodicMemoryCapacity": 5000,
                "associativeMemoryEnabled": True
            },
            "neuromodulationConfig": {
                "dopamineModulationEnabled": True,
                "curiosityModulationEnabled": True,
                "noveltyDetectionEnabled": True
            },
            "developmentConfig": {
                "initialStageDuration": 10,
                "criticalPeriodDuration": 50,
                "maturationDuration": 200,
                "adultStageDuration": 500,
                "agingStageDuration": 100
            }
        }
    
    @staticmethod
    def generate_performance_data() -> Dict[str, Any]:
        """Generate synthetic performance data."""
        return {
            "processing_efficiency": np.random.uniform(0.5, 0.95),
            "steps_per_second": np.random.uniform(10.0, 1000.0),
            "memory_usage_bytes": np.random.randint(1024 * 1024, 100 * 1024 * 1024),
            "average_step_time_ms": np.random.uniform(1.0, 100.0),
            "total_steps": np.random.randint(100, 10000),
            "total_reward": np.random.uniform(0.0, 1000.0),
            "avg_prediction_error": np.random.uniform(0.0, 1.0),
            "avg_curiosity": np.random.uniform(0.0, 1.0),
            "avg_novelty": np.random.uniform(0.0, 1.0)
        }

# Main test suite base class
class NLMTestSuite:
    """Base class for NLM test suites."""
    
    def __init__(self):
        self.framework = NLMSimpleTestFramework()
        self.data_generator = NLMSyntheticDataGenerator()
        self.test_config = TestConfig()
        self.start_time = time.time()
        
    def setup_test_environment(self):
        """Setup test environment."""
        # Create temporary directories
        if not os.path.exists(self.test_config.DEFAULT_CHECKPOINT_DIR):
            os.makedirs(self.test_config.DEFAULT_CHECKPOINT_DIR)
            
        # Create test configuration file
        config_data = self.data_generator.generate_config_data()
        with open(self.test_config.DEFAULT_CONFIG_FILE, 'w') as f:
            json.dump(config_data, f, indent=2)
    
    def teardown_test_environment(self):
        """Clean up test environment."""
        # Clean up temporary files
        if os.path.exists(self.test_config.DEFAULT_CONFIG_FILE):
            os.remove(self.test_config.DEFAULT_CONFIG_FILE)
            
        if os.path.exists(self.test_config.DEFAULT_CHECKPOINT_DIR):
            shutil.rmtree(self.test_config.DEFAULT_CHECKPOINT_DIR)
        
    def setup_test_environment(self):
        """Setup test environment."""
        # Create temporary directories
        if not os.path.exists(self.test_config.DEFAULT_CHECKPOINT_DIR):
            os.makedirs(self.test_config.DEFAULT_CHECKPOINT_DIR)
            
        # Create test configuration file
        config_data = self.data_generator.generate_config_data()
        with open(self.test_config.DEFAULT_CONFIG_FILE, 'w') as f:
            json.dump(config_data, f, indent=2)
    
    def teardown_test_environment(self):
        """Clean up test environment."""
        # Clean up temporary files
        if os.path.exists(self.test_config.DEFAULT_CONFIG_FILE):
            os.remove(self.test_config.DEFAULT_CONFIG_FILE)
            
        if os.path.exists(self.test_config.DEFAULT_CHECKPOINT_DIR):
            shutil.rmtree(self.test_config.DEFAULT_CHECKPOINT_DIR)
    
    def run_all_tests(self):
        """Run all tests in this suite."""
        print(f"\n{'='*60}")
        print(f"Running NLM Test Suite: {self.__class__.__name__}")
        print(f"{'='*60}")
        
        test_methods = [method for method in dir(self) if method.startswith('test_')]
        
        for test_method in test_methods:
            try:
                print(f"\nRunning {test_method}...")
                getattr(self, test_method)()
                print(f"✓ {test_method} PASSED")
            except Exception as e:
                print(f"✗ {test_method} FAILED: {e}")
                import traceback
                traceback.print_exc()
                self.framework.failed_tests += 1
    
    def get_test_summary(self) -> Dict[str, Any]:
        """Get test summary statistics."""
        end_time = time.time()
        duration = end_time - self.start_time
        
        return {
            'test_suite': self.__class__.__name__,
            'passed_tests': self.framework.passed_tests,
            'failed_tests': self.framework.failed_tests,
            'skipped_tests': self.framework.skipped_tests,
            'total_tests': self.framework.passed_tests + self.framework.failed_tests,
            'duration_seconds': duration,
            'average_time_per_test': duration / max(1, self.framework.passed_tests + self.framework.failed_tests)
        }

# Test file location
__test_file__ = __file__

# Integration with pytest
if __name__ == "__main__":
    print("NLM Python Bindings Test Suite")
    print("This is a framework for comprehensive testing of NLM Python bindings.")
    print("Run with pytest for full functionality: pytest -v")
    
    # Auto-discover and run test suites
    import importlib.util
    import inspect
    
    test_dir = os.path.dirname(os.path.abspath(__file__))
    test_files = [f for f in os.listdir(test_dir) if f.startswith('test_') and f.endswith('.py')]
    
    print(f"\nFound test files: {test_files}")
    print("\nTo run all tests, use:")
    print("  pytest -v")
    print("Or run individual test suites:")
    for test_file in test_files:
        module_name = test_file[:-3]
        print(f"  python -m pytest {module_name}.py -v")