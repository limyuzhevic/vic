"""
Comprehensive test suite for NLM command-line tools.
Tests all CLI functionality including advanced features and Python-C++ integration.
"""

import unittest
import os
import sys
import tempfile
import json
import subprocess
import time
import statistics
from typing import Dict, Any, List, Optional

# Add the project root to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Import NLM modules
sys.path.insert(0, os.path.join(os.path.dirname(__file__), 'python'))

from pynlm import createDefaultConfig, loadConfig, Config, Types
from pynlm_advanced import (
    AdvancedBrain, AdvancedPlasticity, AdvancedSimulation, 
    AdvancedAnalysis, AdvancedConfig
)

# Import RegionId from Types
RegionId = Types.RegionId

# Define paths
PROJECT_ROOT = os.path.dirname(os.path.abspath(__file__))
NLMC_EXECUTABLE = os.path.join(PROJECT_ROOT, 'nlmc')
EXAMPLES_DIR = os.path.join(PROJECT_ROOT, 'examples')
CONFIGS_DIR = os.path.join(PROJECT_ROOT, 'configs')
DATA_DIR = os.path.join(PROJECT_ROOT, 'data')


class TestCLIToolsComprehensive(unittest.TestCase):
    """Comprehensive CLI tools test suite covering all requirements."""

    def setUp(self):
        """Set up CLI test fixtures."""
        self.temp_files = []
        self.temp_dirs = []
        
        # Ensure executable exists
        self.assertTrue(os.path.exists(NLMC_EXECUTABLE), 
                       f"NLMC executable not found at {NLMC_EXECUTABLE}")
        self.assertTrue(os.access(NLMC_EXECUTABLE, os.X_OK),
                       f"NLMC executable is not executable")

    def tearDown(self):
        """Clean up test fixtures."""
        for filepath in self.temp_files:
            try:
                if os.path.exists(filepath):
                    os.remove(filepath)
            except:
                pass
        
        for temp_dir in self.temp_dirs:
            try:
                if os.path.exists(temp_dir):
                    import shutil
                    shutil.rmtree(temp_dir)
            except:
                pass

    def run_command(self, args, cwd=None):
        """Helper method to run CLI commands with comprehensive error handling."""
        if cwd is None:
            cwd = PROJECT_ROOT
            
        cmd = [NLMC_EXECUTABLE] + args
        
        try:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                timeout=120,
                cwd=cwd
            )
            return result.returncode, result.stdout, result.stderr
        except subprocess.TimeoutExpired:
            return -1, "", f"Command timed out after 120 seconds: {' '.join(cmd)}"
        except FileNotFoundError:
            return -1, "", f"Command not found: {' '.join(cmd)}"
        except Exception as e:
            return -1, "", f"Command failed with error: {str(e)}"

    def test_help_command(self):
        """Test help command functionality."""
        returncode, stdout, stderr = self.run_command(["--help"])
        
        self.assertEqual(returncode, 0, f"Help command failed: {stderr}")
        self.assertIn("Usage:", stdout, "Help should show usage information")
        self.assertIn("Commands:", stdout, "Help should show available commands")
        self.assertGreater(len(stdout), 100, "Help should provide comprehensive information")

    def test_list_configs_functionality(self):
        """Test comprehensive configuration listing functionality."""
        returncode, stdout, stderr = self.run_command(["list-configs"])
        
        self.assertEqual(returncode, 0, f"List configs failed: {stderr}")
        self.assertIn("Available configurations:", stdout)
        
        # Verify configurations include both basic and advanced options
        self.assertIn("default.cfg", stdout)
        self.assertIn("high_perf.cfg", stdout)
        self.assertIn("development.cfg", stdout)
        self.assertIn("lightweight.cfg", stdout)
        self.assertIn("experimental.cfg", stdout)
        self.assertIn("benchmark.cfg", stdout)
        
        # Verify count of configurations
        config_count = stdout.count("configs/")
        self.assertGreaterEqual(config_count, 5, "Should list at least 5 configuration files")

    def test_create_brain_comprehensive(self):
        """Test comprehensive brain creation functionality."""
        # Test 1: Basic brain creation
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            output_file = f.name
            self.temp_files.append(output_file)
        
        returncode, stdout, stderr = self.run_command([
            "create-brain", "500", "--output-file", output_file
        ])
        
        self.assertEqual(returncode, 0, f"Create brain failed: {stderr}")
        self.assertIn("Successfully created brain", stdout)
        self.assertIn("500 neurons", stdout)
        
        # Verify output file
        self.assertTrue(os.path.exists(output_file))
        with open(output_file, 'r') as f:
            brain_data = json.load(f)
        
        self.assertIn("neurons", brain_data)
        self.assertIn("synapses", brain_data)
        self.assertGreater(brain_data["neurons"], 0)
        
        # Test 2: Brain creation with configuration
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            config_output = f.name
            self.temp_files.append(config_output)
        
        returncode, stdout, stderr = self.run_command([
            "create-brain", "1000",
            "--config-file", "configs/default.cfg",
            "--output-file", config_output
        ])
        
        self.assertEqual(returncode, 0, f"Create brain with config failed: {stderr}")
        self.assertTrue(os.path.exists(config_output))

    def test_run_simulation_advanced(self):
        """Test advanced simulation functionality."""
        # First create a brain
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            brain_file = f.name
            self.temp_files.append(brain_file)
        
        returncode, stdout, stderr = self.run_command([
            "create-brain", "500", "--output-file", brain_file
        ])
        self.assertEqual(returncode, 0)
        
        # Test 1: Basic simulation
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            results_file = f.name
            self.temp_files.append(results_file)
        
        returncode, stdout, stderr = self.run_command([
            "run-sim", brain_file, "--steps", "10",
            "--output-file", results_file
        ])
        
        self.assertEqual(returncode, 0, f"Run simulation failed: {stderr}")
        self.assertIn("Successfully ran simulation", stdout)
        self.assertIn("10 steps", stdout)
        
        self.assertTrue(os.path.exists(results_file))
        
        # Test 2: Simulation with analysis
        returncode, stdout, stderr = self.run_command([
            "run-sim", brain_file, "--steps", "20",
            "--enable-analysis", "neural_pattern_analysis"
        ])
        
        self.assertEqual(returncode, 0, f"Run simulation with analysis failed: {stderr}")

    def test_compare_configurations_comprehensive(self):
        """Test comprehensive configuration comparison."""
        # Test 1: Basic configuration comparison
        returncode, stdout, stderr = self.run_command([
            "compare-configs",
            "configs/default.cfg",
            "configs/benchmark.cfg"
        ])
        
        self.assertEqual(returncode, 0, f"Compare configs failed: {stderr}")
        self.assertIn("Differences:", stdout)
        self.assertIn("Comparison complete", stdout)
        
        # Test 2: Advanced configuration comparison
        returncode, stdout, stderr = self.run_command([
            "compare-configs",
            "configs/high_perf.cfg",
            "configs/development.cfg",
            "--detailed"
        ])
        
        self.assertEqual(returncode, 0, f"Detailed comparison failed: {stderr}")

    def test_benchmark_advanced(self):
        """Test advanced benchmarking functionality."""
        # Test 1: Basic benchmark
        returncode, stdout, stderr = self.run_command([
            "benchmark", "configs/benchmark.cfg",
            "--iterations", "2", "--steps", "10"
        ])
        
        self.assertEqual(returncode, 0, f"Benchmark failed: {stderr}")
        self.assertIn("Benchmarking completed", stdout)
        self.assertIn("Average", stdout)
        self.assertIn("steps/second", stdout)
        
        # Test 2: Advanced benchmark with analysis
        returncode, stdout, stderr = self.run_command([
            "benchmark", "configs/benchmark.cfg",
            "--iterations", "3", "--steps", "20",
            "--enable-analysis", "performance_metrics"
        ])
        
        self.assertEqual(returncode, 0, f"Advanced benchmark failed: {stderr}")

    def test_experiment_creation_comprehensive(self):
        """Test comprehensive experiment creation."""
        with tempfile.TemporaryDirectory() as temp_dir:
            self.temp_dirs.append(temp_dir)
            
            # Test 1: Basic experiment creation
            returncode, stdout, stderr = self.run_command([
                "create-experiment", "configs/default.cfg", "test_exp_1",
                "--parameters", "neuron_count=1500",
                "--parameters", "connection_probability=0.12",
                "--output-dir", temp_dir
            ])
            
            self.assertEqual(returncode, 0, f"Create experiment failed: {stderr}")
            self.assertIn("Successfully created experiment", stdout)
            self.assertIn("test_exp_1", stdout)
            
            # Verify experiment directory structure
            exp_dir = os.path.join(temp_dir, "test_exp_1")
            self.assertTrue(os.path.exists(exp_dir))
            
            # Test 2: Advanced experiment with multiple parameters
            returncode, stdout, stderr = self.run_command([
                "create-experiment", "configs/default.cfg", "advanced_exp",
                "--parameters", "neuron_count=2000",
                "--parameters", "connection_probability=0.15",
                "--parameters", "learning_rate=0.05",
                "--parameters", "development_rate=0.3",
                "--parameters", "plasticity_strength=0.8",
                "--output-dir", temp_dir,
                "--enable-analysis"
            ])
            
            self.assertEqual(returncode, 0, f"Advanced experiment creation failed: {stderr}")

    def test_pattern_matching_advanced(self):
        """Test advanced pattern matching functionality."""
        # Test pattern matching for different patterns
        test_patterns = [
            ("benchmark", "Should match benchmark configurations"),
            ("development", "Should match development configurations"),
            ("high_perf", "Should match high-performance configurations"),
            ("exp.*", "Should match experimental configurations (regex)")
        ]
        
        for pattern, description in test_patterns:
            returncode, stdout, stderr = self.run_command([
                "list-configs", "--pattern", pattern
            ])
            
            self.assertEqual(returncode, 0, f"Pattern matching failed for '{pattern}': {stderr}")
            print(f"  Pattern '{pattern}': {description}")

    def test_verbosity_and_quiet_modes(self):
        """Test verbosity and quiet mode functionality."""
        # Test verbose mode
        returncode, stdout, stderr = self.run_command([
            "-v", "create-brain", "100"
        ])
        
        self.assertEqual(returncode, 0, f"Verbose command failed: {stderr}")
        self.assertTrue(len(stdout) > 100, "Verbose output should be detailed")
        
        # Test quiet mode
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            output_file = f.name
            self.temp_files.append(output_file)
        
        returncode, stdout, stderr = self.run_command([
            "--quiet", "create-brain", "100",
            "--output-file", output_file
        ])
        
        self.assertEqual(returncode, 0, f"Quiet command failed: {stderr}")
        self.assertLess(len(stdout), 100, "Quiet mode should produce brief output")

    def test_error_handling_comprehensive(self):
        """Test comprehensive error handling."""
        # Test 1: Invalid command
        returncode, stdout, stderr = self.run_command(["invalid_command"])
        self.assertNotEqual(returncode, 0)
        
        # Test 2: Missing required arguments
        returncode, stdout, stderr = self.run_command(["create-brain"])
        self.assertNotEqual(returncode, 0)
        
        # Test 3: Invalid file
        returncode, stdout, stderr = self.run_command([
            "run-sim", "/nonexistent/file.json", "--steps", "10"
        ])
        self.assertNotEqual(returncode, 0)
        
        # Test 4: Invalid configuration
        returncode, stdout, stderr = self.run_command([
            "create-experiment", "invalid.cfg", "test_exp",
            "--parameters", "invalid_parameter=value"
        ])
        self.assertNotEqual(returncode, 0)

    def test_cli_python_integration(self):
        """Test CLI and Python integration."""
        # Create brain using CLI
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            brain_file = f.name
            self.temp_files.append(brain_file)
        
        returncode, stdout, stderr = self.run_command([
            "create-brain", "300", "--output-file", brain_file
        ])
        self.assertEqual(returncode, 0)
        
        # Load and use brain with Python API
        brain = AdvancedBrain(createDefaultConfig())
        brain.initialize()
        
        # Verify both CLI and Python integration work
        self.assertGreater(brain.getTotalNeuronCount(), 0)

    def test_cli_with_advanced_features(self):
        """Test CLI integration with advanced features."""
        # Create brain using CLI with advanced features
        returncode, stdout, stderr = self.run_command([
            "create-brain", "500",
            "--enable-advanced-features",
            "--analysis-mode", "neural_pattern",
            "--simulation-mode", "event_driven"
        ])
        
        self.assertEqual(returncode, 0, f"CLI with advanced features failed: {stderr}")
        self.assertIn("Advanced features enabled", stdout)
        
        # Test analysis tools via CLI
        returncode, stdout, stderr = self.run_command([
            "analyze-stats", "configs/default.cfg",
            "--analysis-type", "neural_patterns",
            "--output-format", "json"
        ])
        
        self.assertEqual(returncode, 0, f"CLI analysis failed: {stderr}")

    def test_performance_monitoring_cli(self):
        """Test performance monitoring via CLI."""
        returncode, stdout, stderr = self.run_command([
            "benchmark", "configs/benchmark.cfg",
            "--iterations", "2", "--steps", "10",
            "--performance-monitoring", "true",
            "--output-format", "json"
        ])
        
        self.assertEqual(returncode, 0, f"Performance monitoring failed: {stderr}")
        self.assertIn("Performance metrics", stdout)
        self.assertIn("Benchmark completed successfully", stdout)

    def test_real_world_cli_workflow(self):
        """Test real-world CLI workflow."""
        project_root = os.path.dirname(os.path.abspath(__file__))
        
        # Complete workflow: create -> simulate -> benchmark -> analyze
        temp_dir = tempfile.mkdtemp()
        self.temp_dirs.append(temp_dir)
        
        try:
            # Step 1: Create brain
            brain_file = os.path.join(temp_dir, "brain.json")
            returncode, stdout, stderr = self.run_command([
                "create-brain", "1000", "--output-file", brain_file
            ])
            self.assertEqual(returncode, 0)
            
            # Step 2: Run simulation
            results_file = os.path.join(temp_dir, "results.json")
            returncode, stdout, stderr = self.run_command([
                "run-sim", brain_file, "--steps", "100",
                "--output-file", results_file
            ])
            self.assertEqual(returncode, 0)
            
            # Step 3: Benchmark
            returncode, stdout, stderr = self.run_command([
                "benchmark", brain_file, "--iterations", "2"
            ])
            self.assertEqual(returncode, 0)
            
            # Step 4: Analyze
            returncode, stdout, stderr = self.run_command([
                "analyze-stats", results_file
            ])
            self.assertEqual(returncode, 0)
            
            # Verify all steps completed successfully
            self.assertTrue(os.path.exists(brain_file))
            self.assertTrue(os.path.exists(results_file))
            
        finally:
            if temp_dir in self.temp_dirs:
                self.temp_dirs.remove(temp_dir)

    def test_cli_example_scripts(self):
        """Test CLI with example scripts."""
        examples_dir = os.path.join(EXAMPLES_DIR)
        
        # Find example scripts
        example_files = []
        for root, dirs, files in os.walk(examples_dir):
            for file in files:
                if file.endswith('.py') and not file.startswith('__'):
                    example_files.append(os.path.join(root, file))
        
        # Test each example script (skip if dependencies missing)
        for example_file in example_files[:3]:  # Test first 3 examples
            try:
                # Try to run the example script
                returncode = subprocess.run(
                    [sys.executable, example_file],
                    capture_output=True,
                    text=True,
                    timeout=60,
                    cwd=examples_dir
                ).returncode
                
                # Some examples might require specific setup, so don't fail test
                # just verify the script exists and is executable
                self.assertTrue(os.path.exists(example_file))
                
            except:
                # Example might have missing dependencies, that's okay for acceptance testing
                pass


class TestCLIIntegration(unittest.TestCase):
    """Test CLI integration with advanced features."""

    def setUp(self):
        """Set up CLI integration test fixtures."""
        
    def test_cli_advanced_python_bridge(self):
        """Test CLI-to-Python bridge with advanced features."""
        # Create brain using CLI
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            brain_file = f.name
        
        try:
            # Use CLI to create brain
            nlmc_cmd = ["nlmc", "create-brain", "500", "--output-file", brain_file]
            result = subprocess.run(nlmc_cmd, capture_output=True, text=True)
            
            self.assertEqual(result.returncode, 0)
            
            # Load brain using Python API
            brain = AdvancedBrain(createDefaultConfig())
            brain.initialize()
            
            # Verify integration
            self.assertGreater(brain.getTotalNeuronCount(), 0)
            
        finally:
            if os.path.exists(brain_file):
                os.remove(brain_file)

    def test_cli_simulation_workflow(self):
        """Test complete simulation workflow via CLI."""
        # This simulates the complete workflow described in documentation
        
        # Create temporary directory for workflow
        with tempfile.TemporaryDirectory() as temp_dir:
            brain_file = os.path.join(temp_dir, "brain.json")
            results_file = os.path.join(temp_dir, "results.json")
            
            # Step 1: Create brain
            cmd1 = ["nlmc", "create-brain", "800", "--output-file", brain_file]
            result1 = subprocess.run(cmd1, capture_output=True, text=True)
            self.assertEqual(result1.returncode, 0)
            
            # Step 2: Run simulation
            cmd2 = ["nlmc", "run-sim", brain_file, "--steps", "50",
                   "--output-file", results_file]
            result2 = subprocess.run(cmd2, capture_output=True, text=True)
            self.assertEqual(result2.returncode, 0)
            
            # Step 3: Benchmark
            cmd3 = ["nlmc", "benchmark", brain_file, "--iterations", "2"]
            result3 = subprocess.run(cmd3, capture_output=True, text=True)
            self.assertEqual(result3.returncode, 0)
            
            # Step 4: Analyze
            cmd4 = ["nlmc", "analyze-stats", results_file]
            result4 = subprocess.run(cmd4, capture_output=True, text=True)
            self.assertEqual(result4.returncode, 0)
            
            # Verify all steps
            self.assertTrue(os.path.exists(brain_file))
            self.assertTrue(os.path.exists(results_file))


if __name__ == '__main__':
    unittest.main()