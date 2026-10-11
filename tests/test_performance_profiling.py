#!/usr/bin/env python3
"""
Performance Profiling Unit Tests for NLM
Comprehensive unit tests for NLM performance profiling functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import pandas as pd
import numpy as np
from pathlib import Path
import time
import threading

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.performance.Profiler import PerformanceProfiler, PerformanceProfile

class TestPerformanceProfiling(unittest.TestCase):
    """Test cases for performance profiling functionality"""
    
    def setUp(self):
        """Set up test fixtures"""
        # Mock psutil to avoid system dependencies
        self.psutil_patch = patch.dict('sys.modules', {
            'psutil': Mock(),
        })
        self.mock_psutil = self.psutil_patch.start()
        
        # Configure mock psutil
        mock_virtual_memory = Mock()
        mock_virtual_memory.total = 8 * 1024 * 1024 * 1024  # 8GB
        mock_virtual_memory.available = 4 * 1024 * 1024 * 1024  # 4GB
        mock_virtual_memory.percent = 50.0
        mock_virtual_memory.used = 4 * 1024 * 1024 * 1024  # 4GB
        self.mock_psutil.virtual_memory.return_value = mock_virtual_memory
        
        mock_cpu_percent = Mock()
        mock_cpu_percent.return_value = 25.0
        self.mock_psutil.cpu_percent.return_value = mock_cpu_percent
        
        mock_process = Mock()
        mock_process.pid = 12345
        mock_memory_info = Mock()
        mock_memory_info.rss = 100 * 1024 * 1024  # 100MB
        mock_memory_info.vms = 200 * 1024 * 1024  # 200MB
        mock_process.memory_info.return_value = mock_memory_info
        mock_process.cpu_times = Mock(return_value=Mock(user=10.0, system=5.0))
        mock_process.num_threads = Mock(return_value=4)
        mock_process.num_fds = Mock(return_value=10)
        self.mock_psutil.Process.return_value = mock_process
    
    def tearDown(self):
        """Clean up test fixtures"""
        self.psutil_patch.stop()
        # Close any open figures
        plt.close('all')
    
    def testPerformanceProfilerInitialization(self):
        """Test PerformanceProfiler initialization"""
        profiler = PerformanceProfiler()
        
        self.assertEqual(profiler.is_profiling, False)
        self.assertEqual(profiler.profiles, {})
    
    def testPerformanceProfilerStartStop(self):
        """Test starting and stopping performance profiler"""
        profiler = PerformanceProfiler()
        
        # Start profiling
        profiler.start_profiling("test_profile")
        
        self.assertTrue(profiler.is_profiling)
        self.assertIn("test_profile", profiler.profiles)
        self.assertTrue(profiler.profiles["test_profile"].is_running)
        
        # Stop profiling
        profiler.stop_profiling("test_profile")
        
        self.assertFalse(profiler.is_profiling)
        self.assertFalse(profiler.profiles["test_profile"].is_running)
    
    def testPerformanceProfilerBenchmarkBrain(self):
        """Test benchmarking brain performance"""
        # Mock brain
        mock_brain = Mock()
        mock_brain.initialize = Mock(return_value=True)
        mock_brain.step = Mock()
        mock_brain.active_neuron_count = Mock(return_value=10)
        mock_brain.firing_neuron_count = Mock(return_value=5)
        mock_brain.total_spike_count = Mock(return_value=100)
        mock_brain.average_firing_rate = Mock(return_value=10.0)
        
        # Mock pynlm functions
        with patch('pynlm.createDefaultConfig') as mock_create_default:
            with patch('pynlm.createBrain') as mock_create_brain:
                mock_create_default.return_value = Mock()
                mock_create_brain.return_value = mock_brain
                
                # Create profiler and run benchmark
                profiler = PerformanceProfiler()
                profile = profiler.benchmark_brain(
                    {'brain': {'regions': 1, 'neurons_per_region': 100}},
                    "brain_benchmark",
                    max_steps=50
                )
                
                # Verify profile was created
                self.assertIsNotNone(profile)
                self.assertEqual(profile.name, "brain_benchmark")
                self.assertEqual(len(profile.step_times), 50)
                self.assertTrue(profile.is_running is False)  # Should be stopped
    
    def testPerformanceProfilerMultipleProfiles(self):
        """Test managing multiple performance profiles"""
        profiler = PerformanceProfiler()
        
        # Start multiple profiles
        profile1 = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 100}},
            "profile1",
            max_steps=30
        )
        
        profile2 = profiler.benchmark_brain(
            {'brain': {'regions': 2, 'neurons_per_region': 200}},
            "profile2",
            max_steps=40
        )
        
        # Verify both profiles exist
        self.assertIn("profile1", profiler.profiles)
        self.assertIn("profile2", profiler.profiles)
        
        # Verify profiles are different
        self.assertEqual(profiler.profiles["profile1"].name, "profile1")
        self.assertEqual(profiler.profiles["profile2"].name, "profile2")
        
        # Verify both profiles are stopped
        self.assertFalse(profiler.profiles["profile1"].is_running)
        self.assertFalse(profiler.profiles["profile2"].is_running)
    
    def testPerformanceProfilerProfileComparison(self):
        """Test comparing performance profiles"""
        profiler = PerformanceProfiler()
        
        # Create two profiles with different performance characteristics
        profile1 = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 100}},
            "fast_profile",
            max_steps=50
        )
        
        profile2 = profiler.benchmark_brain(
            {'brain': {'regions': 2, 'neurons_per_region': 200}},
            "slow_profile",
            max_steps=50
        )
        
        # Compare profiles
        comparison = profiler.compare_profiles(["fast_profile", "slow_profile"])
        
        # Verify comparison contains both profiles
        self.assertIn("profiles", comparison)
        self.assertIn("metrics", comparison)
        self.assertIn("rankings", comparison)
        
        self.assertIn("fast_profile", comparison["profiles"])
        self.assertIn("slow_profile", comparison["profiles"])
        
        # Verify metrics comparison
        self.assertIn("duration", comparison["metrics"])
        self.assertIn("peak_memory_mb", comparison["metrics"])
        self.assertIn("avg_cpu_percent", comparison["metrics"])
        self.assertIn("avg_step_time_ms", comparison["metrics"])
        self.assertIn("total_steps", comparison["metrics"])
        
        # Verify rankings
        self.assertIn("duration", comparison["rankings"])
        self.assertIn("peak_memory_mb", comparison["rankings"])
        self.assertIn("avg_cpu_percent", comparison["rankings"])
        self.assertIn("avg_step_time_ms", comparison["rankings"])
        self.assertIn("total_steps", comparison["rankings"])
    
    def testPerformanceProfilerProfileExportImport(self):
        """Test exporting and importing performance profiles"""
        profiler = PerformanceProfiler()
        
        # Create a profile
        profile = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 100}},
            "export_test_profile",
            max_steps=20
        )
        
        # Export profile to file
        export_file = "/tmp/export_test_profile.json"
        profiler.export_profile("export_test_profile", export_file)
        
        # Verify file was created
        self.assertTrue(os.path.exists(export_file))
        
        # Clear profiler files
        profiler.profiles.clear()
        
        # Import profile from file
        profiler.import_profile(export_file)
        
        # Verify profile was imported
        self.assertIn("export_test_profile", profiler.profiles)
        
        # Verify imported profile
        imported_profile = profiler.profiles["export_test_profile"]
        self.assertEqual(imported_profile.name, "export_test_profile")
        self.assertEqual(len(imported_profile.step_times), 20)
        
        # Clean up
        os.unlink(export_file)
    
    def testPerformanceProfilerListProfiles(self):
        """Test listing performance profiles"""
        profiler = PerformanceProfiler()
        
        # Create and start some profiles
        profile1 = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 100}},
            "list_profile1",
            max_steps=20
        )
        
        profile2 = profiler.benchmark_brain(
            {'brain': {'regions': 2, 'neurons_per_region': 200}},
            "list_profile2",
            max_steps=30
        )
        
        # List profiles
        profile_names = profiler.list_profiles()
        
        self.assertIn("list_profile1", profile_names)
        self.assertIn("list_profile2", profile_names)
        self.assertEqual(len(profile_names), 2)
    
    def testPerformanceProfilerProfileSummary(self):
        """Test getting profile summary"""
        profiler = PerformanceProfiler()
        
        # Create a profile
        profile = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 100}},
            "summary_test_profile",
            max_steps=20
        )
        
        # Get profile summary
        summary = profiler.get_profile_summary("summary_test_profile")
        
        # Verify summary contains expected fields
        self.assertIn("name", summary)
        self.assertIn("duration", summary)
        self.assertIn("start_time", summary)
        self.assertIn("end_time", summary)
        self.assertIn("sample_count", summary)
        self.assertIn("peak_memory_mb", summary)
        self.assertIn("avg_cpu_percent", summary)
        self.assertIn("total_steps", summary)
        self.assertIn("avg_step_time_ms", summary)
        self.assertIn("min_step_time_ms", summary)
        self.assertIn("max_step_time_ms", summary)
        self.assertIn("stddev_step_time_ms", summary)
        
        # Verify specific values
        self.assertEqual(summary["name"], "summary_test_profile")
        self.assertEqual(summary["total_steps"], 20)
        self.assertEqual(summary["sample_count"], 20)
    
    def testPerformanceProfilerCleanup(self):
        """Test profiler cleanup"""
        profiler = PerformanceProfiler()
        
        # Create and start profiles
        profile1 = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 100}},
            "cleanup_profile1",
            max_steps=10
        )
        
        profile2 = profiler.benchmark_brain(
            {'brain': {'regions': 2, 'neurons_per_region': 200}},
            "cleanup_profile2",
            max_steps=15
        )
        
        # Verify profiles are created
        self.assertIn("cleanup_profile1", profiler.profiles)
        self.assertIn("cleanup_profile2", profiler.profiles)
        
        # Cleanup
        profiler.cleanup()
        
        # Verify cleanup
        self.assertEqual(profiler.profiles, {})
        self.assertFalse(profiler.is_profiling)
    
    def testPerformanceProfilerConcurrentProfiling(self):
        """Test concurrent profiling"""
        profiler = PerformanceProfiler()
        
        # Create profiles concurrently using threading
        import threading
        import queue
        
        results = queue.Queue()
        
        def create_profile(profile_name, step_multiplier):
            try:
                profile = profiler.benchmark_brain(
                    {'brain': {'regions': 1, 'neurons_per_region': 100}},
                    profile_name,
                    max_steps=20 * step_multiplier
                )
                results.put((profile_name, profile, None))
            except Exception as e:
                results.put((profile_name, None, e))
        
        # Create profiles in parallel
        threads = []
        for i in range(3):
            thread = threading.Thread(
                target=create_profile,
                args=(f"concurrent_profile_{i}", i + 1)
            )
            threads.append(thread)
            thread.start()
        
        # Wait for all threads to complete
        for thread in threads:
            thread.join()
        
        # Collect results
        created_profiles = []
        errors = []
        while not results.empty():
            name, profile, error = results.get()
            if error:
                errors.append((name, error))
            elif profile:
                created_profiles.append((name, profile))
        
        # Verify results
        self.assertEqual(len(created_profiles), 3)
        self.assertEqual(len(errors), 0)
        
        for name, profile in created_profiles:
            self.assertIsNotNone(profile)
            self.assertEqual(profile.name, name)
    
    def testPerformanceProfilerMemoryProfiling(self):
        """Test memory profiling functionality"""
        profiler = PerformanceProfiler()
        
        # Create profile with memory-intensive operations
        profile = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 1000}},
            "memory_test_profile",
            max_steps=100
        )
        
        # Verify profile contains memory information
        summary = profiler.get_profile_summary("memory_test_profile")
        
        self.assertIn("peak_memory_mb", summary)
        self.assertGreaterEqual(summary["peak_memory_mb"], 0)
        
        # Test with smaller brain to ensure memory tracking works
        profile2 = profiler.benchmark_brain(
            {'brain': {'regions': 1, 'neurons_per_region': 100}},
            "small_memory_profile",
            max_steps=10
        )
        
        summary2 = profiler.get_profile_summary("small_memory_profile")
        self.assertIn("peak_memory_mb", summary2)

if __name__ == '__main__':
    unittest.main()
