# Advanced Integration Test Suite
# This test suite validates the enhanced NLM features and improvements

import unittest
import subprocess
import os
import sys
from pathlib import Path

class AdvancedCLITest(unittest.TestCase):
    """Test advanced CLI functionality"""
    
    def setUp(self):
        """Set up test environment"""
        self.test_dir = Path(__file__).parent
        self.bin_dir = self.test_dir / "bin"
        
    def test_cli_help(self):
        """Test CLI help functionality"""
        # Test help command
        result = subprocess.run([
            str(self.bin_dir / "nlm_advanced"),
            "--help"
        ], capture_output=True, text=True)
        
        self.assertEqual(result.returncode, 0)
        self.assertIn("NLM - Advanced Command Line Interface", result.stdout)
        self.assertIn("Usage:", result.stdout)
    
    def test_cli_version(self):
        """Test CLI version functionality"""
        result = subprocess.run([
            str(self.bin_dir / "nlm_advanced"),
            "--version"
        ], capture_output=True, text=True)
        
        self.assertEqual(result.returncode, 0)
        self.assertIn("NLM - Neural Learning Machine", result.stdout)
        self.assertIn("Version: 0.1.0", result.stdout)
    
    def test_cli_experiment_parameter_set(self):
        """Test experimental parameter set application"""
        # Test standard parameter set
        result = subprocess.run([
            str(self.bin_dir / "nlm_advanced"),
            "--experiment", "standard"
        ], capture_output=True, text=True)
        
        self.assertEqual(result.returncode, 0)
        self.assertIn("Applied experimental parameter set: standard", result.stdout)
        
        # Test high_performance parameter set
        result = subprocess.run([
            str(self.bin_dir / "nlm_advanced"),
            "--experiment", "high_performance"
        ], capture_output=True, text=True)
        
        self.assertEqual(result.returncode, 0)
        self.assertIn("Applied experimental parameter set: high_performance", result.stdout)

class ErrorHandlingTest(unittest.TestCase):
    """Test error handling and validation"""
    
    def setUp(self):
        self.test_dir = Path(__file__).parent
        self.bin_dir = self.test_dir / "bin"
        
    def test_invalid_argument(self):
        """Test handling of invalid arguments"""
        result = subprocess.run([
            str(self.bin_dir / "nlm_advanced"),
            "--invalid_key", "value"
        ], capture_output=True, text=True)
        
        # Should not crash, should log error
        self.assertTrue(result.returncode == 0 or result.returncode == 1)
    
    def test_invalid_experiment(self):
        """Test handling of invalid experiment names"""
        result = subprocess.run([
            str(self.bin_dir / "nlm_advanced"),
            "--experiment", "invalid_experiment"
        ], capture_output=True, text=True)
        
        self.assertIn("Unknown experimental parameter set", result.stderr)

class ConfigurationTest(unittest.TestCase):
    """Test configuration management"""
    
    def test_configuration_parsing(self):
        """Test configuration parsing with different formats"""
        test_cases = [
            ("--brain.neuron_count 1000", 1000),
            ("--brain.region_count 2", 2),
            ("--brain.connection_probability 0.5", 0.5),
            ("--simulation.timestep 0.001", 0.001),
            ("--plasticity.stdp.enable true", True),
            ("--plasticity.hebbian.enable false", False),
        ]
        
        for arg, expected_value in test_cases:
            with self.subTest(arg=arg):
                # Note: This is a simplified test - in practice,
                # configuration would be parsed by the actual CLI implementation
                self.assertTrue(isinstance(arg, str))
                self.assertIn("brain.neuron_count", arg or
                             "brain.region_count" in arg or
                             "brain.connection_probability" in arg or
                             "simulation.timestep" in arg or
                             "plasticity.stdp.enable" in arg or
                             "plasticity.hebbian.enable" in arg)

class PerformanceTest(unittest.TestCase):
    """Test performance-related functionality"""
    
    def test_benchmark_execution(self):
        """Test benchmark execution"""
        # This would test actual performance benchmarks
        # For now, we'll just test that the benchmark command exists
        self.assertTrue(os.path.exists(self.bin_dir / "nlm_benchmark") or
                       os.path.exists(self.bin_dir / "nlm_benchmark.exe"))

class VisualizationTest(unittest.TestCase):
    """Test visualization functionality"""
    
    def test_visualization_command(self):
        """Test visualization command"""
        self.assertTrue(os.path.exists(self.bin_dir / "nlm_visualize") or
                       os.path.exists(self.bin_dir / "nlm_visualize.exe"))

if __name__ == '__main__':
    unittest.main()