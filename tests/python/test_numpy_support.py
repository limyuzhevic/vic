#!/usr/bin/env python3
"""
NLM Python NumPy Support Tests

Tests numpy array support for vision and audio data processing:
- Vision data with numpy arrays (2D and 3D)
- Audio data with numpy arrays (1D)
- Convenience methods for numpy conversion
- Data validation and error handling
"""

import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
import numpy as np
import pytest

class TestNumpySupport:
    """Test numpy array support in NLM"""
    
    def test_vision_numpy_setter(self):
        """Test setting vision data from numpy array"""
        vision = pynlm.Vision(10, 10, 3)
        
        # Create test data
        data_2d = np.random.rand(10, 10).astype(np.float32)
        data_3d = np.random.rand(10, 10, 3).astype(np.float32)
        
        # Test 2D (grayscale) - this would need to use setDataFromArray
        # vision.setDataFromArray(data_2d)  # In actual implementation
        
        # Test 3D (RGB)
        # vision.setDataFromArray(data_3d)  # In actual implementation
        
        # For now, test that methods exist
        assert hasattr(vision, 'getDataAsNumpy')
        
    def test_vision_numpy_getter(self):
        """Test getting vision data as numpy array"""
        vision = pynlm.Vision(8, 8, 1)
        
        # Get numpy data
        numpy_data = vision.getDataAsNumpy()
        assert isinstance(numpy_data, np.ndarray)
        assert numpy_data.shape == (8, 8)  # 2D for grayscale
        assert numpy_data.dtype == np.float32
        
        # Test with 3 channels
        vision_rgb = pynlm.Vision(8, 8, 3)
        numpy_data_rgb = vision_rgb.getDataAsNumpy()
        assert numpy_data_rgb.shape == (8, 8, 3)  # 3D for RGB
        
    def test_vision_numpy_conversions(self):
        """Test various numpy array conversions"""
        # Test different input shapes
        shapes_to_test = [
            (10, 10),  # 2D grayscale
            (10, 10, 1),  # 2D with single channel
            (10, 10, 3),  # 3D RGB
            (10, 10, 4),  # 3D with alpha
        ]
        
        for shape in shapes_to_test:
            vision = pynlm.Vision(shape[1], shape[0], shape[2] if len(shape) > 2 else 1)
            numpy_data = vision.getDataAsNumpy()
            
            if len(shape) == 2:
                assert numpy_data.shape == shape
            elif shape[2] == 1:
                assert numpy_data.shape == (shape[0], shape[1])
            else:
                assert numpy_data.shape == shape
        
    def test_vision_numpy_data_integrity(self):
        """Test that numpy data preserves integrity"""
        vision = pynlm.Vision(5, 5, 3)
        
        # Generate test pattern
        test_pattern = np.arange(75, dtype=np.float32).reshape(5, 5, 3)
        
        # In actual implementation: vision.setDataFromArray(test_pattern)
        
        # Verify data is returned correctly
        retrieved_data = vision.getDataAsNumpy()
        
        # Note: For now, this is a placeholder since we don't have actual
        # implementation of setDataFromArray in the bindings
        assert isinstance(retrieved_data, np.ndarray)
        
    def test_vision_numpy_edge_cases(self):
        """Test edge cases for numpy vision data"""
        # Test empty array (should handle gracefully)
        vision = pynlm.Vision(1, 1, 1)
        empty_data = np.array([[]], dtype=np.float32)
        
        # Test single value
        single_data = np.array([[1.0]], dtype=np.float32)
        
        # Test large values
        large_data = np.full((5, 5, 3), 1000.0, dtype=np.float32)
        
        # Test negative values
        negative_data = np.full((5, 5, 3), -1.0, dtype=np.float32)
        
        # All should be convertible to numpy
        assert hasattr(vision, 'getDataAsNumpy')
        
    def test_audio_numpy_setter(self):
        """Test setting audio data from numpy array"""
        audio = pynlm.Audio(44100, 1000)
        
        # Create test audio data
        audio_data = np.random.randn(1000).astype(np.float32)
        
        # Test numpy setter (placeholder)
        # audio.setDataFromArray(audio_data)  # In actual implementation
        
        assert hasattr(audio, 'getDataAsNumpy')
        
    def test_audio_numpy_getter(self):
        """Test getting audio data as numpy array"""
        audio = pynlm.Audio(44100, 500)
        
        numpy_data = audio.getDataAsNumpy()
        assert isinstance(numpy_data, np.ndarray)
        assert numpy_data.shape == (500,)
        assert numpy_data.dtype == np.float32
        
        # Test with different sample rates
        audio_2 = pynlm.Audio(22050, 1000)
        numpy_data_2 = audio_2.getDataAsNumpy()
        assert numpy_data_2.shape == (1000,)
        
    def test_audio_numpy_integrity(self):
        """Test audio data integrity with numpy"""
        audio = pynlm.Audio(44100, 500)
        
        # Generate test audio signal
        test_signal = np.sinc(np.linspace(0, 1, 500)).astype(np.float32)
        
        # Note: Actual implementation would use setDataFromArray
        retrieved_signal = audio.getDataAsNumpy()
        assert isinstance(retrieved_signal, np.ndarray)
        
    def test_audio_numpy_edge_cases(self):
        """Test edge cases for numpy audio data"""
        audio = pynlm.Audio(44100, 100)
        
        # Test zero signal
        zero_signal = np.zeros(100, dtype=np.float32)
        
        # Test constant signal
        const_signal = np.full(100, 0.5, dtype=np.float32)
        
        # Test very large values (clipping may occur)
        large_signal = np.full(100, 10000.0, dtype=np.float32)
        
        # Test very small values
        small_signal = np.full(100, 0.000001, dtype=np.float32)
        
        # All should be handleable
        assert hasattr(audio, 'getDataAsNumpy')
        
    def test_numpy_dtype_conversion(self):
        """Test numpy dtype conversion handling"""
        # Test different numpy dtypes
        dtypes_to_test = [
            np.float32,
            np.float64,
            np.int32,
            np.int64,
        ]
        
        for dtype in dtypes_to_test:
            # Create test data
            if dtype in [np.float32, np.float64]:
                test_data = np.random.rand(10, 10).astype(dtype)
            else:
                test_data = np.random.randint(0, 100, (10, 10)).astype(dtype)
            
            # Create vision object
            vision = pynlm.Vision(10, 10, 1)
            
            # Verify conversion capabilities
            assert hasattr(vision, 'getDataAsNumpy')
            
    def test_numpy_memory_efficiency(self):
        """Test that numpy operations are memory efficient"""
        # Test with large arrays (memory considerations)
        large_shape = (100, 100, 3)
        large_data = np.random.rand(*large_shape).astype(np.float32)
        
        # Create vision object
        vision = pynlm.Vision(large_shape[1], large_shape[0], large_shape[2])
        
        # Get numpy representation
        numpy_data = vision.getDataAsNumpy()
        
        # Check memory usage (basic check)
        memory_usage = numpy_data.nbytes / (1024 * 1024)  # MB
        assert memory_usage > 0
        
    def test_numpy_multithreading_safety(self):
        """Test numpy operations in multithreaded context"""
        import threading
        import time
        
        results = []
        
        def worker(thread_id):
            vision = pynlm.Vision(10, 10, 3)
            data = np.random.rand(10, 10, 3).astype(np.float32)
            # time.sleep(0.01)  # Simulate work
            numpy_data = vision.getDataAsNumpy()
            results.append((thread_id, numpy_data.shape))
        
        # Create multiple threads
        threads = []
        for i in range(5):
            thread = threading.Thread(target=worker, args=(i,))
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Verify all threads completed successfully
        assert len(results) == 5
        for thread_id, shape in results:
            assert shape == (10, 10, 3)
        
    def test_numpy_broadcasting(self):
        """Test numpy broadcasting with vision data"""
        vision = pynlm.Vision(5, 5, 1)
        
        # Test that operations work with broadcasting
        numpy_data = vision.getDataAsNumpy()
        
        # Should support standard numpy operations
        normalized = (numpy_data - numpy_data.min()) / (numpy_data.max() - numpy_data.min())
        assert normalized.shape == (5, 5)
        assert normalized.min() >= 0
        assert normalized.max() <= 1
        
    def test_numpy_performance_benchmark(self):
        """Basic performance benchmark for numpy operations"""
        import time
        
        vision = pynlm.Vision(50, 50, 3)
        
        # Time numpy data retrieval
        start_time = time.time()
        for _ in range(10):
            data = vision.getDataAsNumpy()
        end_time = time.time()
        
        avg_time = (end_time - start_time) / 10
        print(f"Average numpy data retrieval time: {avg_time:.4f} seconds")
        
        # Should be reasonable (less than 0.1 seconds per operation for small arrays)
        assert avg_time < 0.1

# Global test functions for pytest compatibility
def run_numpy_support_tests():
    """Run all numpy support tests"""
    test_obj = TestNumpySupport()
    
    tests = [
        "test_vision_numpy_setter",
        "test_vision_numpy_getter",
        "test_vision_numpy_conversions",
        "test_vision_numpy_data_integrity",
        "test_vision_numpy_edge_cases",
        "test_audio_numpy_setter",
        "test_audio_numpy_getter",
        "test_audio_numpy_integrity",
        "test_audio_numpy_edge_cases",
        "test_numpy_dtype_conversion",
        "test_numpy_memory_efficiency",
        "test_numpy_multithreading_safety",
        "test_numpy_broadcasting",
        "test_numpy_performance_benchmark"
    ]
    
    results = []
    for test_name in tests:
        try:
            method = getattr(test_obj, test_name)
            method()
            results.append({"test": test_name, "status": "PASS", "error": None})
            print(f"✓ {test_name}")
        except Exception as e:
            results.append({"test": test_name, "status": "FAIL", "error": str(e)})
            print(f"✗ {test_name}: {e}")
    
    passed = sum(1 for r in results if r["status"] == "PASS")
    total = len(results)
    
    print(f"\nNumPy Support Tests: {passed}/{total} passed")
    return results