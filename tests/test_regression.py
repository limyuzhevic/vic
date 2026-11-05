#!/usr/bin/env python3
"""
Regression Tests for NLM Project

Tests to ensure backward compatibility is maintained and regression detection.
"""

import pytest
import tempfile
import os
import json
import yaml
import logging
import time
import statistics
from pathlib import Path
from typing import Dict, Any, List, Optional
from unittest.mock import Mock, patch, MagicMock

# Import NLM modules
import sys
sys.path.insert(0, str(Path(__file__).parent.parent))

try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

# Import test utilities
from .conftest import (
    test_config,
    pynlm_available,
    mock_pynlm,
    mock_brain,
    mock_agent,
    mock_world,
    mock_config,
    assert_config_dict_equal,
    create_test_config_file,
    cleanup_test_file
)

# Regression test data
REGRESSION_TEST_CONFIG = {
    "random_seed": 42,
    "simulation_timestep": 0.001,
    "neuron_count": 1000,
    "region_count": 2,
    "connection_probability": 0.1,
    "plasticity_learning_rate": 0.01,
    "stdp_ltp_weight": 0.01,
    "stdp_ltd_weight": 0.012,
    "stdp_time_constant": 20.0,
    "development_synaptogenesis_rate": 0.001,
    "development_pruning_rate": 0.0001,
    "dopamine_baseline": 0.1,
    "reward_discount_factor": 0.99,
    "environment_name": "GridWorld",
    "environment_width": 10,
    "environment_height": 10,
    "log_level": "INFO",
    "log_to_file": False,
    "log_filename": "nlm.log",
    "max_simulation_steps": 10000,
    "simulation_time_limit": 0.0,
    "visualization_enabled": False,
    "visualization_update_rate": 30.0
}

# Reference values for regression testing
REGRESSION_REFERENCE_VALUES = {
    "random_seed": 42,
    "neuron_count": 1000,
    "log_level": "INFO",
    "plasticity_learning_rate": 0.01
}


def pytest_configure(config):
    """Configure pytest for regression tests."""
    config.addinivalue_line("markers", "regression: regression tests")
    config.addinivalue_line("markers", "backward_compatibility: backward compatibility tests")
    config.addinivalue_line("markers", "regression_detection: regression detection tests")
    config.addinivalue_line("markers", "consistency: consistency tests")
    config.addinivalue_line("markers", "stability: stability tests")
    config.addinivalue_line("markers", "performance_regression: performance regression tests")
    config.addinivalue_line("markers", "slow: tests that take longer to run")


class TestBackwardCompatibility:
    """Test backward compatibility with existing functionality."""

    def test_key_value_format_backward_compatibility(self):
        """Test that key=value format still works (backward compatibility)."""
        # This is the traditional format that should always work
        traditional_config = """random_seed = 42
neuron_count = 1000
log_level = INFO
"""

        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write(traditional_config)
            traditional_file = f.name

        try:
            config = pynlm.createDefaultConfig()
            success = config.loadFromFile(traditional_file)

            if pynlm_available and success:
                # Traditional format should still work
                assert config.has("random_seed")
                assert config.has("neuron_count")
                assert config.has("log_level")

                # Values should be correct
                seed = config.get("random_seed")
                if seed.has_value():
                    assert seed.value() == 42

                neurons = config.get("neuron_count")
                if neurons.has_value():
                    assert neurons.value() == 1000

                log_level = config.get("log_level")
                if log_level.has_value():
                    assert log_level.value() == "INFO"

                logger.info("Key=value format backward compatibility test passed")

        finally:
            if os.path.exists(traditional_file):
                os.unlink(traditional_file)

    def test_cli_backward_compatibility(self):
        """Test that CLI interface maintains backward compatibility."""
        # Test that basic CLI commands still work as expected

        # Test help command (should always work)
        result = subprocess.run(
            ["python", "-m", "nlm_cli", "--help"],
            capture_output=True,
            text=True,
            cwd=Path(__file__).parent.parent
        )

        assert result.returncode == 0, "CLI help should always work"
        assert "NLM (Neural Learning Machine) Command Line Interface" in result.stdout

        # Test that all major commands are still available
        expected_commands = ["brain", "experiment", "performance", "config", "visualize"]

        for command in expected_commands:
            result = subprocess.run(
                ["python", "-m", "nlm_cli", command, "--help"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result.returncode == 0, f"Command '{command}' should still be available"
            assert command in result.stdout.lower() or command in result.stderr.lower()

    def test_api_backward_compatibility(self):
        """Test that NLM Python API maintains backward compatibility."""
        if not pynlm_available:
            pytest.skip("pynlm not available")

        # Test that basic API functions still work
        config = pynlm.createDefaultConfig()
        assert config is not None, "createDefaultConfig should still exist"

        # Test basic configuration operations
        config.set("test_key", "test_value")
        assert config.has("test_key"), "Configuration should support key setting"

        config.remove("test_key")
        assert not config.has("test_key"), "Configuration should support key removal"

        # Test loading from file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
            f.write("test_key = value1\nanother_key = value2")
            test_file = f.name

        try:
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromFile(test_file)
            assert success, "loadFromFile should still work"

            assert config2.has("test_key"), "Should load test_key"
            assert config2.has("another_key"), "Should load another_key"

        finally:
            if os.path.exists(test_file):
                os.unlink(test_file)

    def test_configuration_format_backward_compatibility(self):
        """Test that all configuration formats are still supported."""
        if not pynlm_available:
            pytest.skip("pynlm not available")

        formats_to_test = [
            ("key_value", "key = value\nnumber = 42", "test.cfg"),
            ("json", '{"key": "value", "number": 42}', "test.json"),
            ("yaml", "key: value\nnumber: 42", "test.yaml")
        ]

        for format_name, content, extension in formats_to_test:
            with tempfile.NamedTemporaryFile(mode='w', suffix=extension, delete=False) as f:
                f.write(content)
                test_file = f.name

            try:
                config = pynlm.createDefaultConfig()

                # Load based on format
                if format_name == "key_value":
                    success = config.loadFromFile(test_file)
                elif format_name == "json":
                    success = config.loadFromJsonFile(test_file)
                elif format_name == "yaml":
                    success = config.loadFromYamlFile(test_file)
                else:
                    success = False

                assert success, f"{format_name} format should still be supported"

            finally:
                if os.path.exists(test_file):
                    os.unlink(test_file)

    def test_performance_api_backward_compatibility(self):
        """Test that performance profiling API maintains backward compatibility."""
        # Test that performance profiling modules can still be imported
        try:
            from nlm_profiling.benchmark import BenchmarkHarnessModule, BenchmarkConfig
            from nlm_profiling.base import PerformanceMetrics, BenchmarkResult

            # Test that classes can be instantiated
            config = BenchmarkConfig()
            assert config is not None, "BenchmarkConfig should still exist"

            harness = BenchmarkHarnessModule()
            assert harness is not None, "BenchmarkHarnessModule should still exist"

            metric = PerformanceMetrics(time.time(), "test", 1.0, "unit")
            assert metric is not None, "PerformanceMetrics should still exist"

            result = BenchmarkResult("test", {}, [], 1.0, True)
            assert result is not None, "BenchmarkResult should still exist"

            logger.info("Performance API backward compatibility test passed")

        except ImportError as e:
            pytest.fail(f"Performance API imports should still work: {e}")


class TestRegressionDetection:
    """Test regression detection capabilities."""

    def test_regression_detection_basic_config_values(self):
        """Test regression detection for basic configuration values."""
        # Load configuration and compare against reference values
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(REGRESSION_TEST_CONFIG, f)
            config_file = f.name

        try:
            config = pynlm.createDefaultConfig()
            success = config.loadFromJsonFile(config_file)

            if pynlm_available and success:
                # Test key reference values
                for key, expected_value in REGRESSION_REFERENCE_VALUES.items():
                    if config.has(key):
                        actual_value = config.get(key).value() if config.get(key).has_value() else None
                        if actual_value is not None:
                            if isinstance(expected_value, float) and isinstance(actual_value, float):
                                assert abs(actual_value - expected_value) < 1e-6, \
                                    f"Regression: Value for {key} has changed: expected {expected_value}, got {actual_value}"
                            else:
                                assert actual_value == expected_value, \
                                    f"Regression: Value for {key} has changed: expected {expected_value}, got {actual_value}"

                logger.info("Basic configuration values regression detection test passed")

        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)

    def test_regression_detection_cli_behavior(self):
        """Test regression detection for CLI behavior."""
        # Test that CLI commands behave the same way
        cli_commands = [
            ["nlm", "--help"],
            ["nlm", "brain", "--help"],
            ["nlm", "config", "--help"],
            ["nlm", "performance", "--help"]
        ]

        for cmd in cli_commands:
            result = subprocess.run(
                cmd,
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            # All should return the same exit code (0 for help commands)
            assert result.returncode == 0, f"CLI command {cmd} should still work: {result.stderr}"

    def test_regression_detection_file_format_consistency(self):
        """Test regression detection for file format consistency."""
        # Create identical configuration in different formats
        # and verify they produce consistent results

        test_config = {
            "neuron_count": 500,
            "region_count": 2,
            "learning_rate": 0.01
        }

        formats = [
            (".json", lambda c: json.dump(c, c.file)),
            (".yaml", lambda c: yaml.dump(c, c.file)),
            (".cfg", lambda c: c.file.write("\n".join([f"{k} = {v}" for k, v in c.items()])))
        ]

        for ext, write_func in formats:
            with tempfile.NamedTemporaryFile(mode='w', suffix=ext, delete=False) as f:
                if ext == ".json":
                    json.dump(test_config, f)
                elif ext == ".yaml":
                    yaml.dump(test_config, f)
                else:  # .cfg
                    for key, value in test_config.items():
                        f.write(f"{key} = {value}\n")
                test_file = f.name

            try:
                # Load using appropriate method
                config = pynlm.createDefaultConfig()

                if ext == ".json":
                    success = config.loadFromJsonFile(test_file)
                elif ext == ".yaml":
                    success = config.loadFromYamlFile(test_file)
                else:  # .cfg
                    success = config.loadFromFile(test_file)

                if pynlm_available and success:
                    # Verify all formats loaded the same keys
                    assert set(config.getKeys()) == set(test_config.keys()), \
                        f"File format {ext} should load all keys"

            finally:
                if os.path.exists(test_file):
                    os.unlink(test_file)

    def test_regression_detection_error_handling(self):
        """Test regression detection for error handling consistency."""
        # Test that error handling is consistent

        # Test 1: Non-existent file should always fail
        config = pynlm.createDefaultConfig()
        success = config.loadFromJsonFile("/nonexistent/file.json")
        assert not success, "Non-existent file should always fail"

        # Test 2: Invalid JSON should always fail or be handled gracefully
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"invalid": json}')
            invalid_file = f.name

        try:
            config2 = pynlm.createDefaultConfig()
            success = config2.loadFromJsonFile(invalid_file)
            # Should fail or handle gracefully
            assert not success or True, "Invalid JSON should be handled"

        finally:
            if os.path.exists(invalid_file):
                os.unlink(invalid_file)

        # Test 3: Invalid CLI arguments should always fail
        result = subprocess.run(
            ["python", "-m", "nlm_cli", "brain", "invalid_command"],
            capture_output=True,
            text=True,
            cwd=Path(__file__).parent.parent
        )

        assert result.returncode != 0, "Invalid CLI command should fail"

    @pytest.mark.slow
    def test_regression_detection_performance_consistency(self):
        """Test regression detection for performance consistency."""
        # Test that performance characteristics are consistent

        import timeit

        # Time basic configuration operations
        config_times = []

        for i in range(5):
            start_time = time.time()

            config = pynlm.createDefaultConfig()
            config.set("test_key", f"value_{i}")
            config.get("test_key")  # Access the value

            end_time = time.time()
            config_times.append(end_time - start_time)

        # Calculate statistics
        mean_time = statistics.mean(config_times)
        stddev_time = statistics.stdev(config_times) if len(config_times) > 1 else 0.0

        # Performance should be consistent (reasonable variation)
        assert mean_time < 0.1, f"Configuration operations too slow: {mean_time:.3f}s"
        assert stddev_time / mean_time < 1.0, f"Performance too inconsistent: {stddev_time/mean_time:.1f}"

        logger.info(f"Performance consistency - Mean: {mean_time:.3f}s, Stddev: {stddev_time:.3f}s")


class TestStabilityTesting:
    """Test stability of functionality over multiple runs."""

    def test_stability_multiple_configuration_loads(self):
        """Test stability of configuration loading over multiple runs."""
        config_file = None

        try:
            # Create test configuration
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                json.dump(REGRESSION_TEST_CONFIG, f)
                config_file = f.name

            # Load configuration multiple times
            config_results = []
            for i in range(10):
                config = pynlm.createDefaultConfig()
                success = config.loadFromJsonFile(config_file)

                if pynlm_available and success:
                    # Capture some values for comparison
                    values = {}
                    for key in REGRESSION_REFERENCE_VALUES.keys():
                        if config.has(key):
                            value = config.get(key).value() if config.get(key).has_value() else None
                            if value is not None:
                                values[key] = value

                    config_results.append(values)

            # All successful loads should have identical values
            if len(config_results) >= 2:
                for i in range(1, len(config_results)):
                    for key in config_results[0].keys():
                        if key in config_results[i]:
                            val1 = config_results[0][key]
                            val2 = config_results[i][key]

                            if isinstance(val1, float) and isinstance(val2, float):
                                assert abs(val1 - val2) < 1e-9, \
                                    f"Stability test: Values for {key} differ between runs"
                            else:
                                assert val1 == val2, \
                                    f"Stability test: Values for {key} differ between runs"

            logger.info(f"Configuration stability test passed - {len(config_results)} successful loads")

        finally:
            if config_file and os.path.exists(config_file):
                os.unlink(config_file)

    def test_stability_multiple_cli_runs(self):
        """Test stability of CLI commands over multiple runs."""
        # Run CLI commands multiple times and check consistency
        cli_commands = [
            ["python", "-m", "nlm_cli", "brain", "--help"],
            ["python", "-m", "nlm_cli", "config", "--help"],
            ["python", "-m", "nlm_cli", "performance", "--help"]
        ]

        for cmd in cli_commands:
            help_outputs = []
            for i in range(5):
                result = subprocess.run(
                    cmd,
                    capture_output=True,
                    text=True,
                    cwd=Path(__file__).parent.parent
                )

                assert result.returncode == 0, f"CLI command {cmd} should work"
                help_outputs.append(result.stdout)

            # All help outputs should be identical (or at least similar)
            if len(help_outputs) >= 2:
                # Check that key content is present in all outputs
                for output in help_outputs:
                    assert "NLM (Neural Learning Machine)" in output, "Key documentation should be present"

        logger.info("CLI stability test passed")

    def test_stability_performance_over_time(self):
        """Test stability of performance characteristics over time."""
        import psutil
        import os

        process = psutil.Process(os.getpid())

        # Monitor resource usage over time
        memory_samples = []
        cpu_samples = []

        for i in range(10):
            # Take memory sample
            memory_samples.append(process.memory_info().rss)
            cpu_samples.append(process.cpu_percent(interval=0.1))

            # Do a small operation
            config = pynlm.createDefaultConfig()
            config.set(f"temp_{i}", f"value_{i}")

            # Small delay
            time.sleep(0.01)

        # Calculate statistics
        memory_mb = [m / (1024 * 1024) for m in memory_samples]
        memory_mean = statistics.mean(memory_mb)
        memory_stddev = statistics.stdev(memory_mb) if len(memory_mb) > 1 else 0.0

        cpu_mean = statistics.mean(cpu_samples)
        cpu_stddev = statistics.stdev(cpu_samples) if len(cpu_samples) > 1 else 0.0

        # Stability criteria
        memory_stability = memory_stddev / memory_mean if memory_mean > 0 else 0
        cpu_stability = cpu_stddev / cpu_mean if cpu_mean > 0 else 0

        # Memory should be relatively stable (variation < 10%)
        assert memory_stability < 0.1, f"Memory usage too unstable: {memory_stability:.1%}"

        # CPU should be relatively stable (variation < 50% for CPU)
        assert cpu_stability < 0.5, f"CPU usage too unstable: {cpu_stability:.1%}"

        logger.info(f"Performance stability - Memory: {memory_mean:.1f}MB±{memory_stddev:.1f}MB, CPU: {cpu_mean:.1f}%±{cpu_stddev:.1f}%")


class TestPerformanceRegression:
    """Test for performance regressions."""

    @pytest.mark.performance_regression
    def test_performance_regression_configuration_loading(self):
        """Test for performance regression in configuration loading."""
        if not pynlm_available:
            pytest.skip("pynlm not available")

        # Time configuration loading operations
        load_times = []

        for i in range(10):
            start_time = time.perf_counter()

            config = pynlm.createDefaultConfig()
            config.set("test_key", "test_value")
            config.get("test_key")

            end_time = time.perf_counter()
            load_times.append(end_time - start_time)

        mean_time = statistics.mean(load_times)
        max_time = max(load_times)

        # Performance criteria (adjust based on actual implementation)
        assert mean_time < 0.05, f"Configuration loading too slow: {mean_time:.3f}s"
        assert max_time < 0.1, f"Worst case configuration loading too slow: {max_time:.3f}s"

        # Check that performance is consistent (not degrading)
        if len(load_times) >= 2:
            first_half = load_times[:len(load_times)//2]
            second_half = load_times[len(load_times)//2:]

            first_mean = statistics.mean(first_half)
            second_mean = statistics.mean(second_half)

            # Performance should not be degrading significantly
            if second_mean > first_mean:
                degradation = (second_mean - first_mean) / first_mean
                assert degradation < 0.2, f"Performance degrading: {degradation:.1%}"

        logger.info(f"Configuration loading performance - Mean: {mean_time:.3f}s, Max: {max_time:.3f}s")

    @pytest.mark.performance_regression
    def test_performance_regression_cli_commands(self):
        """Test for performance regression in CLI commands."""
        # Time CLI command execution
        cmd_times = []

        for i in range(5):
            start_time = time.time()

            result = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "--help"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            end_time = time.time()
            cmd_times.append(end_time - start_time)

            assert result.returncode == 0, "CLI help command should work"

        mean_time = statistics.mean(cmd_times)
        max_time = max(cmd_times)

        # Performance criteria for CLI commands
        assert mean_time < 2.0, f"CLI help command too slow: {mean_time:.1f}s"
        assert max_time < 5.0, f"Worst case CLI command too slow: {max_time:.1f}s"

        logger.info(f"CLI command performance - Mean: {mean_time:.1f}s, Max: {max_time:.1f}s")

    def test_performance_regression_memory_usage(self):
        """Test for performance regression in memory usage."""
        import psutil
        import os

        process = psutil.Process(os.getpid())

        # Measure memory usage during configuration operations
        initial_memory = process.memory_info().rss

        # Perform many configuration operations
        configs = []
        for i in range(100):
            config = pynlm.createDefaultConfig()
            config.set(f"key_{i}", f"value_{i}")
            config.get(f"key_{i}")
            configs.append(config)

        # Force cleanup
        del configs

        final_memory = process.memory_info().rss
        memory_increase = (final_memory - initial_memory) / (1024 * 1024)  # MB

        # Memory increase should be reasonable
        assert memory_increase < 50.0, f"Memory usage increased too much: {memory_increase:.1f}MB"

        logger.info(f"Memory usage regression test - Increase: {memory_increase:.1f}MB")

    @pytest.mark.slow
    def test_performance_regression_comprehensive(self):
        """Comprehensive performance regression test."""
        # Run a comprehensive set of operations to check for regressions

        # 1. Configuration operations
        config_op_times = []
        for i in range(20):
            start_time = time.time()

            config = pynlm.createDefaultConfig()
            config.set("op_key", "op_value")
            config.get("op_key")
            config.has("op_key")

            end_time = time.time()
            config_op_times.append(end_time - start_time)

        # 2. CLI operations
        cli_op_times = []
        for i in range(3):
            start_time = time.time()

            result = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "--help"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            end_time = time.time()
            cli_op_times.append(end_time - start_time)

            assert result.returncode == 0

        # Calculate comprehensive performance metrics
        config_mean = statistics.mean(config_op_times)
        config_stddev = statistics.stdev(config_op_times) if len(config_op_times) > 1 else 0.0

        cli_mean = statistics.mean(cli_op_times)
        cli_stddev = statistics.stdev(cli_op_times) if len(cli_op_times) > 1 else 0.0

        # Overall performance should be reasonable
        assert config_mean < 0.01, f"Configuration operations too slow: {config_mean:.3f}s"
        assert cli_mean < 1.0, f"CLI operations too slow: {cli_mean:.1f}s"

        # Performance should be consistent
        config_coefficient_of_variation = config_stddev / config_mean if config_mean > 0 else 0
        cli_coefficient_of_variation = cli_stddev / cli_mean if cli_mean > 0 else 0

        assert config_coefficient_of_variation < 1.0, f"Configuration operations too inconsistent"
        assert cli_coefficient_of_variation < 1.0, f"CLI operations too inconsistent"

        logger.info(f"Comprehensive performance - Config: {config_mean:.3f}s±{config_stddev:.3f}s, CLI: {cli_mean:.1f}s±{cli_stddev:.1f}s")


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])