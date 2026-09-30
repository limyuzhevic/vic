#!/usr/bin/env python3
"""
Pytest configuration and fixtures for NLM Python tests.
"""
import pytest
import sys
import os
from pathlib import Path

# Add the current directory to Python path so we can import nlm
sys.path.insert(0, str(Path(__file__).parent))

# Try to import the nlm module
try:
    import pynlm
    PYNL_AVAILABLE = True
except ImportError as e:
    print(f"Warning: pynlm module not available: {e}")
    PYNL_AVAILABLE = False
    pynlm = None

# Skip all tests if pynlm is not available
pytestmark = pytest.mark.skipif(not PYNL_AVAILABLE, reason="pynlm module not available")

@pytest.fixture
def config():
    """Fixture that creates a default configuration."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.createDefaultConfig()

@pytest.fixture
def brain(config):
    """Fixture that creates a default brain."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.createBrain(config)

@pytest.fixture
def simple_world():
    """Fixture that creates a simple world."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.createSimpleWorld()

@pytest.fixture
def agent_brain(brain, simple_world):
    """Fixture that creates an agent brain connected to a world."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    agent = pynlm.createAgentBrain(brain)
    agent.initialize(simple_world)
    return agent

@pytest.fixture
def vision_input():
    """Fixture that creates a vision input."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.Vision(64, 64, 3)

@pytest.fixture
def audio_input():
    """Fixture that creates an audio input."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.Audio(44100, 1024)

@pytest.fixture
def internal_signals():
    """Fixture that creates internal signals."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.InternalSignals()

@pytest.fixture
def world_object():
    """Fixture that creates a world object."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.WorldObject(10.0, 10.0, pynlm.WorldObjectType.Resource, 100.0)

@pytest.fixture
def action():
    """Fixture that creates an action."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.Action(pynlm.ActionType.MoveForward)

@pytest.fixture
def action_result():
    """Fixture that creates an action result."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.ActionResult(10.0, True, "Success")

@pytest.fixture
def sensory_percept():
    """Fixture that creates a sensory percept."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.SensoryPercept()

@pytest.fixture
def agent_body():
    """Fixture that creates an agent body."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    return pynlm.AgentBody()

@pytest.fixture
def brain_with_regions(brain):
    """Fixture that creates a brain with some regions."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    # Add some regions
    brain.addRegion("Region_1")
    brain.addRegion("Region_2")
    brain.addRegion("Region_3")
    return brain

@pytest.fixture
def populated_brain(brain):
    """Fixture that creates a brain with some neurons and synapses."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    # Initialize the brain (this would normally happen in a real scenario)
    # For testing purposes, we'll just return the brain as-is
    return brain

@pytest.fixture
def configured_config():
    """Fixture that creates a pre-configured configuration."""
    if not PYNL_AVAILABLE:
        pytest.skip("pynlm module not available")
    
    config = pynlm.createDefaultConfig()
    config.set("neuron_count", 100)
    config.set("region_count", 2)
    config.set("connection_probability", 0.1)
    config.set("simulation_timestep", 0.001)
    config.set("random_seed", 42)
    return config

# Test marker for integration tests
@pytest.fixture
def skip_if_no_integration():
    """Marker to skip integration tests if certain conditions are not met."""
    pass

# Performance testing utilities
import time
import statistics

@pytest.fixture
def timer():
    """Fixture that provides timing utilities."""
    class Timer:
        def __init__(self):
            self.times = []
        
        def __enter__(self):
            self.start = time.time()
            return self
        
        def __exit__(self, exc_type, exc_val, exc_tb):
            self.times.append(time.time() - self.start)
        
        def average(self):
            return statistics.mean(self.times) if self.times else 0
        
        def max(self):
            return max(self.times) if self.times else 0
        
        def min(self):
            return min(self.times) if self.times else 0
        
        def count(self):
            return len(self.times)
    
    return Timer()

# Error testing utilities
@pytest.fixture
def error_tester():
    """Fixture that provides utilities for testing error handling."""
    class ErrorTester:
        def __init__(self, module):
            self.module = module
        
        def test_invalid_arguments(self, func, *args, **kwargs):
            """Test that function raises appropriate errors for invalid arguments."""
            # Implementation would depend on the specific function
            pass
        
        def test_out_of_bounds(self, func, *args, **kwargs):
            """Test that function handles out-of-bounds errors."""
            # Implementation would depend on the specific function
            pass
    
    return ErrorTester(pynlm)

# Memory testing utilities
import gc
import tracemalloc

@pytest.fixture
def memory_tracker():
    """Fixture that tracks memory usage."""
    def start_tracking():
        tracemalloc.start()
        gc.collect()
        return tracemalloc.take_snapshot()
    
    def stop_tracking(start_snapshot):
        current = tracemalloc.take_snapshot()
        tracemalloc.stop()
        return current.compare_to(start_snapshot, 'lineno')
    
    return type('MemoryTracker', (), {
        'start': start_tracking,
        'stop': stop_tracking
    })()

# Regression testing utilities
def regression_test_decorator(test_func):
    """Decorator for marking tests as regression tests."""
    setattr(test_func, '_is_regression_test', True)
    return test_func

# Performance test utilities
def performance_test_decorator(max_time=None):
    """Decorator for performance testing."""
    def decorator(test_func):
        setattr(test_func, '_max_time', max_time)
        return test_func
    return decorator

# Memory leak detection utilities
def detect_memory_leak(test_func):
    """Detect memory leaks in test function."""
    def wrapper(*args, **kwargs):
        if not PYNL_AVAILABLE:
            pytest.skip("pynlm module not available")
        
        tracker = memory_tracker()
        start = tracker.start()
        
        try:
            result = test_func(*args, **kwargs)
            
            # Check for memory leaks
            diff = tracker.stop(start)
            total_diff = sum(stat.size_diff for stat in diff)
            
            if total_diff > 1024:  # More than 1KB leak
                pytest.fail(f"Possible memory leak detected: {total_diff} bytes")
            
            return result
        except Exception as e:
            tracker.stop(start)
            raise
    
    return wrapper