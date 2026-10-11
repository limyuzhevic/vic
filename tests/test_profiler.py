#!/usr/bin/env python3
"""
Performance Profiler Unit Tests for NLM
Comprehensive unit tests for NLM PerformanceProfiler functionality
"""

import unittest
import sys
import os
from unittest.mock import Mock, patch, MagicMock
import tempfile
import json
import time
import threading
import psutil
import statistics
from pathlib import Path

# Add the python directory to the path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)) + '/..')

import pynlm
from python.performance.Profiler import (
    PerformanceProfiler,
    PerformanceProfile,
    PerformanceSnapshot,
    ProfileMetric
)

class TestPerformanceProfiler(unittest.TestCase):
    """Test cases for PerformanceProfiler class"""
    
    def setUp(self):
        """Set up test fixtures"""
        self.profiler = PerformanceProfiler()
        
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
    
    def testPerformanceProfilerInitialization(self):
        """Test PerformanceProfiler initialization"""
        profiler = PerformanceProfiler()
        
        self.assertEqual(profiler.is_profiling, False)
        self.assertEqual(profiler.profiles, {})
    
    def testStartStopProfiling(self):
        """Test starting and stopping profiling"""
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
    
    def testProfileCreation(self):
        """Test PerformanceProfile creation"""
        profile = PerformanceProfile("test_profile")
        
        self.assertEqual(profile.name, "test_profile")
        self.assertEqual(profile.is_running, False)
        self.assertEqual(profile.snapshots, [])
        self.assertEqual(profile.step_times, [])
        self.assertEqual(profile.neuron_counts, [])
        self.assertEqual(profile.spike_counts, [])
        self.assertEqual(profile.sample_interval, 0.1)
    
    def testStartStopProfile(self):
        """Test starting and stopping a performance profile"""
        profile = PerformanceProfile("test_profile")
        
        # Start profile
        profile.start()
        
        self.assertTrue(profile.is_running)
        self.assertIsNotNone(profile.start_time)
        self.assertEqual(profile.snapshots, [])
        
        # Add some sample data
        mock_brain = Mock()
        mock_brain.active_neuron_count = Mock(return_value=10)
        mock_brain.firing_neuron_count = Mock(return_value=5)
        mock_brain.total_spike_count = Mock(return_value=100)
        mock_brain.average_firing_rate = Mock(return_value=10.0)
        
        profile.add_step_metrics(1, mock_brain, 100, 50, 0.1)
        profile.add_step_metrics(2, mock_brain, 120, 60, 0.15)
        
        self.assertEqual(len(profile.snapshots), 2)
        self.assertEqual(profile.step_times, [0.1, 0.15])
        self.assertEqual(profile.neuron_counts, [100, 120])
        self.assertEqual(profile.spike_counts, [50, 60])
        
        # Stop profile
        profile.stop()
        
        self.assertFalse(profile.is_running)
        self.assertIsNotNone(profile.end_time)
        self.assertEqual(profile.duration, profile.end_time - profile.start_time)
    
    def testPerformanceSnapshot(self):
        """Test PerformanceSnapshot creation and metrics"""
        snapshot = PerformanceSnapshot(time.time())
        
        # Add metrics
        snapshot.add_metric("memory_usage_mb", 512.5, "MB")
        snapshot.add_metric("cpu_percent", 25.0, "%")
        snapshot.add_metric("step_time_ms", 15.5, "ms")
        
        # Verify snapshot
        self.assertEqual(len(snapshot.metrics), 3)
        self.assertIn("memory_usage_mb", snapshot.metrics)
        self.assertIn("cpu_percent", snapshot.metrics)
        self.assertIn("step_time_ms", snapshot.metrics)
        
        # Check metric values
        memory_metric = snapshot.metrics["memory_usage_mb"]
        self.assertEqual(memory_metric.name, "memory_usage_mb")
        self.assertEqual(memory_metric.value, 512.5)
        self.assertEqual(memory_metric.unit, "MB")
    
    def testProfileSummary(self):
        """Test performance profile summary generation"""
        profile = PerformanceProfile("test_profile")
        
        # Add sample data
        mock_brain = Mock()
        mock_brain.active_neuron_count = Mock(return_value=10)
        mock_brain.firing_neuron_count = Mock(return_value=5)
        mock_brain.total_spike_count = Mock(return_value=100)
        mock_brain.average_firing_rate = Mock(return_value=10.0)
        
        for i in range(5):
            profile.add_step_metrics(i, mock_brain, 100 + i*10, 50 + i*10, 0.1 + i*0.05)
        
        # Get summary
        summary = profile.get_summary()
        
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
        
        # Verify calculations
        self.assertEqual(summary["name"], "test_profile")
        self.assertEqual(summary["total_steps"], 5)
        self.assertEqual(summary["sample_count"], 5)
        self.assertGreater(summary["avg_step_time_ms"], 0)
        self.assertLess(summary["min_step_time_ms"], summary["max_step_time_ms"])
    
    def testExportProfile(self):
        """Test exporting performance profile to file"""
        profile = PerformanceProfile("test_profile")
        
        # Add sample data
        mock_brain = Mock()
        mock_brain.active_neuron_count = Mock(return_value=10)
        mock_brain.firing_neuron_count = Mock(return_value=5)
        mock_brain.total_spike_count = Mock(return_value=100)
        mock_brain.average_firing_rate = Mock(return_value=10.0)
        
        profile.add_step_metrics(1, mock_brain, 100, 50, 0.1)
        
        # Export to temporary file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            export_file = f.name
        
        try:
            profile.export_profile(export_file)
            
            # Verify file was created
            self.assertTrue(os.path.exists(export_file))
            
            # Load and verify content
            with open(export_file, 'r') as f:
                profile_data = json.load(f)
            
            self.assertIn("name", profile_data)
            self.assertIn("start_time", profile_data)
            self.assertIn("end_time", profile_data)
            self.assertIn("duration", profile_data)
            self.assertIn("snapshots", profile_data)
            self.assertIn("step_times", profile_data)
            self.assertIn("neuron_counts", profile_data)
            self.assertIn("spike_counts", profile_data)
            self.assertIn("summary", profile_data)
            
            self.assertEqual(profile_data["name"], "test_profile")
            self.assertEqual(profile_data["total_steps"], 1)
            
        finally:
            if os.path.exists(export_file):
                os.unlink(export_file)
    
    def testProfilerBenchmarkBrain(self):
        """Test profiling brain benchmark"""
        # Mock brain config
        brain_config = {
            'brain': {
                'regions': 1,
                'neurons_per_region': 100,
                'neuron_type': 'Internal'
            }
        }
        
        # Mock brain
        mock_brain = Mock()
        mock_brain.initialize = Mock(return_value=True)
        mock_brain.step = Mock()
        
        # Mock pynlm functions
        with patch('pynlm.createDefaultConfig') as mock_create_default:
            with patch('pynlm.createBrain') as mock_create_brain:
                mock_create_default.return_value = Mock()
                mock_create_brain.return_value = mock_brain
                
                # Run benchmark
                profile = self.profiler.benchmark_brain(brain_config, "brain_benchmark", max_steps=50)
                
                # Verify profile was created
                self.assertIsNotNone(profile)
                self.assertEqual(profile.name, "brain_benchmark")
                self.assertEqual(len(profile.step_times), 50)
                self.assertTrue(profile.is_running is False)  # Should be stopped after benchmark
    
    def testProfilerListProfiles(self):
        """Test listing performance profiles"""
        # Start two profiles
        profile1 = PerformanceProfile("profile1")
        profile1.start()
        profile1.stop()
        
        profile2 = PerformanceProfile("profile2")
        profile2.start()
        profile2.stop()
        
        # Manually add them to profiler (since start_profiling is mocked)
        self.profiler.profiles["profile1"] = profile1
        self.profiler.profiles["profile2"] = profile2
        
        # List profiles
        profile_names = self.profiler.list_profiles()
        
        self.assertIn("profile1", profile_names)
        self.assertIn("profile2", profile_names)
        self.assertEqual(len(profile_names), 2)
    
    def testProfilerGetProfile(self):
        """Test getting a specific performance profile"""
        # Create and add a profile
        profile = PerformanceProfile("test_profile")
        profile.start()
        profile.stop()
        
        self.profiler.profiles["test_profile"] = profile
        
        # Get profile
        retrieved_profile = self.profiler.get_profile("test_profile")
        
        self.assertIsNotNone(retrieved_profile)
        self.assertEqual(retrieved_profile.name, "test_profile")
        self.assertEqual(retrieved_profile.is_running, False)
    
    def testProfilerCompareProfiles(self):
        """Test comparing multiple performance profiles"""
        # Create and add two profiles
        profile1 = PerformanceProfile("profile1")
        profile1.start()
        profile1.stop()
        
        profile2 = PerformanceProfile("profile2")
        profile2.start()
        profile2.stop()
        
        self.profiler.profiles["profile1"] = profile1
        self.profiler.profiles["profile2"] = profile2
        
        # Add sample data to both profiles
        mock_brain = Mock()
        mock_brain.active_neuron_count = Mock(return_value=10)
        mock_brain.firing_neuron_count = Mock(return_value=5)
        mock_brain.total_spike_count = Mock(return_value=100)
        mock_brain.average_firing_rate = Mock(return_value=10.0)
        
        for i in range(3):
            profile1.add_step_metrics(i, mock_brain, 100, 50, 0.1)
            profile2.add_step_metrics(i, mock_brain, 110, 55, 0.12)
        
        # Compare profiles
        comparison = self.profiler.compare_profiles(["profile1", "profile2"])
        
        self.assertIn("profiles", comparison)
        self.assertIn("metrics", comparison)
        self.assertIn("rankings", comparison)
        
        self.assertIn("profile1", comparison["profiles"])
        self.assertIn("profile2", comparison["profiles"])
        
        # Check metrics comparison
        self.assertIn("duration", comparison["metrics"])
        self.assertIn("peak_memory_mb", comparison["metrics"])
        self.assertIn("avg_cpu_percent", comparison["metrics"])
        self.assertIn("avg_step_time_ms", comparison["metrics"])
        self.assertIn("total_steps", comparison["metrics"])
    
    def testProfilerCleanup(self):
        """Test profiler cleanup"""
        # Create and start profiles
        profile1 = PerformanceProfile("profile1")
        profile1.is_running = True
        
        profile2 = PerformanceProfile("profile2")
        profile2.is_running = True
        
        self.profiler.profiles["profile1"] = profile1
        self.profiler.profiles["profile2"] = profile2
        self.profiler.is_profiling = True
        
        # Cleanup
        self.profiler.cleanup()
        
        # Verify cleanup
        self.assertEqual(self.profiler.profiles, {})
        self.assertFalse(self.profiler.is_profiling)

if __name__ == '__main__':
    unittest.main()
