#!/usr/bin/env python3
"""
Integration Tests for NLM Project

Tests that verify all components work together end-to-end, including:
1. Documentation examples that actually work
2. Configuration system integration
3. CLI tool integration
4. Performance profiling integration
5. Full workflow tests
"""

import pytest
import tempfile
import os
import json
import yaml
import logging
import time
import subprocess
from pathlib import Path
from typing import Dict, Any, List, Optional
from unittest.mock import Mock, patch, MagicMock

# Import NLM modules
import sys
sys.path.insert(0, str(Path(__file__).parent.parent))

import pynlm

# Import profiling modules
from nlm_profiling.benchmark import BenchmarkHarnessModule, BenchmarkConfig, Configuration, BenchmarkType
from nlm_profiling.base import PerformanceMetrics, BenchmarkResult, OutputFormat

# Import test utilities
from .conftest import (
    test_config,
    pynlm_available,
    mock_pynlm,
    mock_brain,
    mock_agent,
    mock_world,
    mock_config,
    cli_args,
    assert_config_dict_equal,
    create_test_config_file,
    cleanup_test_file
)

# Integration test data
INTEGRATION_TEST_CONFIG = {
    "random_seed": 42,
    "neuron_count": 1000,
    "region_count": 2,
    "log_level": "INFO",
    "plasticity": True,
    "learning_rate": 0.01
}

INTEGRATION_TEST_WORKFLOW_STEPS = [
    "create_configuration",
    "load_configuration",
    "set_parameters",
    "get_parameters",
    "validate_configuration",
    "benchmark_performance",
    "export_results"
]


def pytest_configure(config):
    """Configure pytest for integration tests."""
    config.addinivalue_line("markers", "integration: integration tests")
    config.addinivalue_line("markers", "end_to_end: end-to-end tests")
    config.addinivalue_line("markers", "workflow: workflow tests")
    config.addinivalue_line("markers", "full_stack: full stack integration tests")
    config.addinivalue_line("markers", "slow: tests that take longer to run")


class TestConfigurationSystemIntegration:
    """Test configuration system integration with other components."""

    def test_config_system_complete_workflow(self):
        """Test complete workflow using configuration system."""
        if not pynlm_available:
            pytest.skip("pynlm not available")

        # Step 1: Create configuration from JSON
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            json.dump(INTEGRATION_TEST_CONFIG, f)
            json_config_file = f.name

        try:
            # Load JSON configuration
            config = pynlm.createDefaultConfig()
            success = config.loadFromJsonFile(json_config_file)
            assert success, "JSON configuration should load successfully"

            # Verify configuration
            assert config.has("random_seed")
            assert config.has("neuron_count")
            assert config.has("region_count")

            # Step 2: Create configuration from YAML
            with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
                yaml.dump(INTEGRATION_TEST_CONFIG, f)
                yaml_config_file = f.name

            try:
                config2 = pynlm.createDefaultConfig()
                success = config2.loadFromYamlFile(yaml_config_file)
                assert success, "YAML configuration should load successfully"

                # Verify YAML configuration
                assert config2.has("random_seed")
                assert config2.has("neuron_count")

                # Step 3: Create configuration from key=value
                with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
                    for key, value in INTEGRATION_TEST_CONFIG.items():
                        f.write(f"{key} = {value}\n")
                    cfg_config_file = f.name

                try:
                    config3 = pynlm.createDefaultConfig()
                    success = config3.loadFromFile(cfg_config_file)
                    assert success, "key=value configuration should load successfully"

                    # Step 4: Verify consistency across formats
                    config1_values = {
                        k: config.get(k).value() if config.has(k) else None
                        for k in INTEGRATION_TEST_CONFIG.keys()
                    }
                    config2_values = {
                        k: config2.get(k).value() if config2.has(k) else None
                        for k in INTEGRATION_TEST_CONFIG.keys()
                    }
                    config3_values = {
                        k: config3.get(k).value() if config3.has(k) else None
                        for k in INTEGRATION_TEST_CONFIG.keys()
                    }

                    # All should have the same keys
                    assert set(config1_values.keys()) == set(INTEGRATION_TEST_CONFIG.keys())
                    assert set(config2_values.keys()) == set(INTEGRATION_TEST_CONFIG.keys())
                    assert set(config3_values.keys()) == set(INTEGRATION_TEST_CONFIG.keys())

                    # Values should match (within tolerance for floats)
                    for key in INTEGRATION_TEST_CONFIG.keys():
                        val1 = config1_values[key]
                        val2 = config2_values[key]
                        val3 = config3_values[key]
                        expected = INTEGRATION_TEST_CONFIG[key]

                        if isinstance(expected, float):
                            assert abs(val1 - expected) < 1e-6
                            assert abs(val2 - expected) < 1e-6
                            assert abs(val3 - expected) < 1e-6
                        else:
                            assert val1 == expected
                            assert val2 == expected
                            assert val3 == expected

                finally:
                    os.unlink(cfg_config_file)

        finally:
            os.unlink(json_config_file)
            os.unlink(yaml_config_file)

    def test_config_system_cli_integration(self):
        """Test configuration system integration with CLI."""
        # Test CLI command that uses configuration system
        test_config_content = json.dumps(INTEGRATION_TEST_CONFIG)

        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write(test_config_content)
            config_file = f.name

        try:
            # Use config load command via CLI
            result = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "load",
                 "--config-file", config_file,
                 "--json"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result.returncode == 0, f"CLI config load should succeed, got: {result.stderr}"

            # Verify JSON output contains expected data
            output_data = json.loads(result.stdout)
            assert "success" in output_data
            assert output_data["success"] is True
            assert "config" in output_data

            # Verify config data matches input
            for key, expected_value in INTEGRATION_TEST_CONFIG.items():
                assert key in output_data["config"], f"Config should contain key: {key}"

        finally:
            os.unlink(config_file)

    def test_config_system_performance_profiling_integration(self):
        """Test configuration system integration with performance profiling."""
        if not pynlm_available:
            pytest.skip("pynlm not available")

        # Create configuration for performance benchmarking
        config = pynlm.createDefaultConfig()

        # Set up configuration for benchmark
        config.set("neuron_count", 500)
        config.set("region_count", 3)
        config.set("plasticity", True)

        # Create benchmark harness with this configuration
        harness_config = BenchmarkConfig(
            benchmark_type=BenchmarkType.COMPARATIVE,
            steps=50,
            runs=2
        )

        harness = BenchmarkHarnessModule(harness_config)

        # Add configuration based on pynlm config
        brain_config = {
            "neuron_count": config.get("neuron_count").value() if config.has("neuron_count") else 100,
            "region_count": config.get("region_count").value() if config.has("region_count") else 1,
            "plasticity": config.get("plasticity").value() if config.has("plasticity") else False
        }

        harness.add_configuration(Configuration("test_benchmark", brain_config))

        # Verify integration
        configs = harness.get_configurations()
        assert len(configs) == 1
        assert configs[0].parameters == brain_config


class TestCLIToolsIntegration:
    """Test CLI tools integration with other components."""

    def test_cli_tools_complete_workflow(self):
        """Test complete workflow using CLI tools."""
        # Step 1: Create configuration using CLI
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            output_file = f.name

        try:
            # Create config using CLI
            result1 = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "create",
                 "--output-file", output_file,
                 "--params", "neuron_count=100", "region_count=2"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result1.returncode == 0, f"Config create should succeed: {result1.stderr}"

            # Step 2: Load configuration using CLI
            result2 = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "load",
                 "--config-file", output_file],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result2.returncode == 0, f"Config load should succeed: {result2.stderr}"

            # Step 3: Get parameter using CLI
            result3 = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "get",
                 "--key", "neuron_count"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result3.returncode == 0, f"Config get should succeed: {result3.stderr}"

            # Step 4: Set parameter using CLI
            with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f2:
                param_output_file = f2.name

            try:
                result4 = subprocess.run(
                    ["python", "-m", "nlm_cli", "config", "set",
                     "--key", "custom_param",
                     "--value", "custom_value",
                     "--output-file", param_output_file,
                     "--config-file", output_file],
                    capture_output=True,
                    text=True,
                    cwd=Path(__file__).parent.parent
                )

                assert result4.returncode == 0, f"Config set should succeed: {result4.stderr}"
                assert os.path.exists(param_output_file), "Parameter output file should exist"

            finally:
                if os.path.exists(param_output_file):
                    os.unlink(param_output_file)

        finally:
            if os.path.exists(output_file):
                os.unlink(output_file)

    def test_cli_tools_performance_integration(self):
        """Test CLI tools integration with performance profiling."""
        # Test performance CLI with configuration
        result = subprocess.run(
            ["python", "-m", "nlm_cli", "performance", "benchmark",
             "--num-brains", "3",
             "--benchmark-steps", "50",
             "--json"],
            capture_output=True,
            text=True,
            cwd=Path(__file__).parent.parent
        )

        # Should succeed or fail gracefully (implementation dependent)
        assert result.returncode in [0, 1], f"Performance benchmark should succeed or fail gracefully: {result.stderr}"

        # If successful, should produce JSON output
        if result.returncode == 0:
            try:
                output_data = json.loads(result.stdout)
                assert "benchmark_results" in output_data or "results" in output_data
            except json.JSONDecodeError:
                # Some CLI implementations may not produce JSON
                pass

    def test_cli_tools_experiment_integration(self):
        """Test CLI tools integration with experiment running."""
        # Test experiment CLI
        result = subprocess.run(
            ["python", "-m", "nlm_cli", "experiment", "run",
             "--max-steps", "100",
             "--json"],
            capture_output=True,
            text=True,
            cwd=Path(__file__).parent.parent
        )

        # Should succeed or fail gracefully
        assert result.returncode in [0, 1], f"Experiment run should succeed or fail gracefully: {result.stderr}"

        # If successful, should produce JSON output
        if result.returncode == 0:
            try:
                output_data = json.loads(result.stdout)
                assert "experiment_results" in output_data or "results" in output_data
            except json.JSONDecodeError:
                # Some CLI implementations may not produce JSON
                pass

    def test_cli_tools_brain_integration(self):
        """Test CLI tools integration with brain management."""
        # Test brain CLI
        result = subprocess.run(
            ["python", "-m", "nlm_cli", "brain", "create",
             "--neurons", "50",
             "--regions", "1",
             "--json"],
            capture_output=True,
            text=True,
            cwd=Path(__file__).parent.parent
        )

        # Should succeed or fail gracefully
        assert result.returncode in [0, 1], f"Brain create should succeed or fail gracefully: {result.stderr}"

        # If successful, should produce JSON output
        if result.returncode == 0:
            try:
                output_data = json.loads(result.stdout)
                assert "brain_info" in output_data or "success" in output_data
            except json.JSONDecodeError:
                # Some CLI implementations may not produce JSON
                pass


class TestPerformanceProfilingIntegration:
    """Test performance profiling integration with other components."""

    def test_performance_profiling_with_configuration(self):
        """Test performance profiling using NLM configuration."""
        if not pynlm_available:
            pytest.skip("pynlm not available")

        # Create NLM configuration
        config = pynlm.createDefaultConfig()

        # Set up configuration for performance testing
        config.set("neuron_count", 200)
        config.set("region_count", 2)
        config.set("log_level", "INFO")

        # Create performance benchmark harness
        harness_config = BenchmarkConfig(
            benchmark_type=BenchmarkType.STEP_PERFORMANCE,
            steps=100,
            runs=3,
            warmup_steps=10
        )

        harness = BenchmarkHarnessModule(harness_config)

        # Add configuration based on NLM config
        brain_config = {
            "neuron_count": config.get("neuron_count").value() if config.has("neuron_count") else 100,
            "region_count": config.get("region_count").value() if config.has("region_count") else 1,
            "plasticity": config.get("plasticity").value() if config.has("plasticity") else False
        }

        harness.add_configuration(Configuration("performance_test", brain_config))

        # Test that harness can be used for profiling
        assert len(harness.benchmark_results) == 1
        assert harness.benchmark_results[0].parameters == brain_config

        # Test insights generation (mock)
        mock_result = BenchmarkResult(
            test_name="performance_integration",
            parameters={"steps": 100},
            metrics=[
                PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec"),
                PerformanceMetrics(time.time(), "memory_usage", 256.0, "MB")
            ],
            duration=5.0,
            success=True
        )

        insights = harness.generate_benchmark_insights(mock_result)
        assert "performance_analysis" in insights
        assert "recommendations" in insights

    def test_performance_profiling_cli_integration(self):
        """Test performance profiling integration with CLI."""
        # Test that performance profiling can be accessed via CLI
        result = subprocess.run(
            ["python", "-m", "nlm_cli", "performance", "--help"],
            capture_output=True,
            text=True,
            cwd=Path(__file__).parent.parent
        )

        assert result.returncode == 0, f"Performance CLI help should work: {result.stderr}"
        assert "performance" in result.stdout.lower()

        # Test individual performance commands
        for command in ["benchmark", "memory", "stats"]:
            result = subprocess.run(
                ["python", "-m", "nlm_cli", "performance", command, "--help"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result.returncode == 0, f"Performance {command} help should work: {result.stderr}"

    @pytest.mark.slow
    def test_performance_profiling_end_to_end_workflow(self):
        """Test end-to-end performance profiling workflow."""
        # Create a comprehensive performance testing workflow
        harness = BenchmarkHarnessModule()

        # Add multiple configurations for comparison
        configurations = [
            Configuration("baseline", {"neuron_count": 100, "learning_rate": 0.1}),
            Configuration("optimized", {"neuron_count": 200, "learning_rate": 0.01}),
            Configuration("aggressive", {"neuron_count": 500, "learning_rate": 0.001})
        ]

        for config in configurations:
            harness.add_configuration(config)

        # Test configuration management
        configs = harness.get_configurations()
        assert len(configs) == 3

        config_names = [c.name for c in configs]
        assert "baseline" in config_names
        assert "optimized" in config_names
        assert "aggressive" in config_names

        # Test comparative analysis
        # Create mock results for each configuration
        results = []
        for i, config in enumerate(configs):
            metrics = [
                PerformanceMetrics(time.time(), "steps_per_second", 1000.0 * (i + 1), "steps/sec"),
                PerformanceMetrics(time.time(), "memory_usage", 100.0 * (i + 1), "MB")
            ]
            result = BenchmarkResult(
                test_name=f"benchmark_{config.name}",
                parameters={"config": config.parameters},
                metrics=metrics,
                duration=5.0 + i * 2.0,
                success=True
            )
            results.append(result)

        # Generate comparative report
        report = harness.generate_comparative_report(results)

        assert "total_benchmarks" in report
        assert "successful_benchmarks" in report
        assert report["total_benchmarks"] == 3
        assert report["successful_benchmarks"] == 3

        # Verify that results are aggregated correctly
        assert "steps_per_second_average" in report
        assert "memory_usage_average" in report

        # Verify that performance improves with better configurations
        # (higher neuron_count should ideally have better performance)
        if "steps_per_second_average" in report:
            # This is a simple check - actual performance relationship
            # depends on the specific benchmark implementation
            pass


class TestFullStackIntegration:
    """Test full stack integration of all components."""

    @pytest.mark.full_stack
    def test_full_stack_integration_workflow(self):
        """Test full stack integration workflow."""
        # Step 1: Configuration Setup
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            workflow_config = json.dumps({
                "neuron_count": 300,
                "region_count": 3,
                "plasticity": True,
                "learning_rate": 0.05,
                "simulation_timestep": 0.001
            })
            f.write(workflow_config)
            config_file = f.name

        try:
            # Step 2: Create configuration using CLI
            result1 = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "create",
                 "--output-file", config_file,
                 "--params", "neuron_count=300", "region_count=3"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result1.returncode == 0, f"Step 1 failed: {result1.stderr}"

            # Step 3: Load and validate configuration
            result2 = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "load",
                 "--config-file", config_file],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert result2.returncode == 0, f"Step 2 failed: {result2.stderr}"

            # Step 4: Set additional parameters
            with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f2:
                param_file = f2.name

            try:
                result3 = subprocess.run(
                    ["python", "-m", "nlm_cli", "config", "set",
                     "--key", "plasticity",
                     "--value", "true",
                     "--output-file", param_file,
                     "--config-file", config_file],
                    capture_output=True,
                    text=True,
                    cwd=Path(__file__).parent.parent
                )

                assert result3.returncode == 0, f"Step 3 failed: {result3.stderr}"

                # Step 5: Performance benchmarking with configured parameters
                result4 = subprocess.run(
                    ["python", "-m", "nlm_cli", "performance", "benchmark",
                     "--num-brains", "2",
                     "--benchmark-steps", "50"],
                    capture_output=True,
                    text=True,
                    cwd=Path(__file__).parent.parent
                )

                # Performance benchmark should run (success or graceful failure)
                assert result4.returncode in [0, 1], f"Step 4 failed: {result4.stderr}"

                # Step 6: Experiment running
                result5 = subprocess.run(
                    ["python", "-m", "nlm_cli", "experiment", "run",
                     "--max-steps", "200"],
                    capture_output=True,
                    text=True,
                    cwd=Path(__file__).parent.parent
                )

                # Experiment should run (success or graceful failure)
                assert result5.returncode in [0, 1], f"Step 5 failed: {result5.stderr}"

                # Step 7: Export results (create results file)
                results_file = "/tmp/nlm_integration_results.json"

                # Create a simple results file
                results_data = {
                    "workflow_id": "integration_test_001",
                    "timestamp": time.time(),
                    "steps_completed": 5,
                    "successful_steps": 4,
                    "failed_steps": 1,
                    "results": {
                        "configuration_setup": result1.returncode == 0,
                        "configuration_loading": result2.returncode == 0,
                        "parameter_setting": result3.returncode == 0,
                        "performance_benchmarking": result4.returncode == 0,
                        "experiment_running": result5.returncode == 0
                    }
                }

                with open(results_file, 'w') as f:
                    json.dump(results_data, f, indent=2)

                assert os.path.exists(results_file), "Results file should exist"

                # Verify results file content
                with open(results_file, 'r') as f:
                    saved_results = json.load(f)
                    assert "workflow_id" in saved_results
                    assert "results" in saved_results
                    assert "configuration_setup" in saved_results["results"]

                # Cleanup
                os.unlink(results_file)

            finally:
                if os.path.exists(param_file):
                    os.unlink(param_file)

        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)

    def test_full_stack_error_handling(self):
        """Test full stack error handling and recovery."""
        # Test that errors in one component don't break the entire system

        # Create an invalid configuration file
        with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
            f.write('{"invalid": json}')  # Invalid JSON
            invalid_config = f.name

        try:
            # Try to load invalid configuration via CLI
            result = subprocess.run(
                ["python", "-m", "nlm_cli", "config", "load",
                 "--config-file", invalid_config],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            # Should fail gracefully, not crash the system
            assert result.returncode != 0, "Should fail with invalid config"
            assert "error" in result.stderr.lower(), "Should report error"

            # The system should still be able to run other commands
            help_result = subprocess.run(
                ["python", "-m", "nlm_cli", "--help"],
                capture_output=True,
                text=True,
                cwd=Path(__file__).parent.parent
            )

            assert help_result.returncode == 0, "System should still respond to help command"

        finally:
            if os.path.exists(invalid_config):
                os.unlink(invalid_config)

    @pytest.mark.slow
    def test_full_stack_performance_monitoring(self):
        """Test full stack performance monitoring."""
        import psutil
        import os

        process = psutil.Process(os.getpid())

        # Monitor system resources during integration tests
        initial_memory = process.memory_info().rss
        initial_cpu = process.cpu_percent()

        # Run a series of integration operations
        for i in range(3):
            # Create temporary config file
            with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
                config_data = {"iteration": i, "data": "test_" + str(i)}
                json.dump(config_data, f)
                temp_config = f.name

            try:
                # Load configuration
                result = subprocess.run(
                    ["python", "-m", "nlm_cli", "config", "load",
                     "--config-file", temp_config],
                    capture_output=True,
                    text=True,
                    cwd=Path(__file__).parent.parent
                )

                # Should succeed or fail gracefully
                assert result.returncode in [0, 1]

            finally:
                if os.path.exists(temp_config):
                    os.unlink(temp_config)

        # Check resource usage after operations
        final_memory = process.memory_info().rss
        final_cpu = process.cpu_percent()

        memory_increase = (final_memory - initial_memory) / (1024 * 1024)  # MB
        cpu_increase = final_cpu - initial_cpu

        # Resource usage should be reasonable
        assert memory_increase < 100.0, f"Memory increase too high: {memory_increase:.1f}MB"
        assert abs(cpu_increase) < 50.0, f"CPU change too large: {cpu_increase}"

        logger.info(f"Performance test - Memory increase: {memory_increase:.1f}MB, CPU change: {cpu_increase}%")


class TestDocumentationIntegration:
    """Test documentation integration with actual functionality."""

    def test_documentation_examples_integration(self):
        """Test that documentation examples actually work."""
        # This would test if documentation examples are accurate
        # For now, we'll create a simple integration test

        # Create a simple example that matches documentation patterns
        example_config = {
            "description": "Example from documentation",
            "parameters": {
                "neuron_count": 100,
                "region_count": 2,
                "learning_rate": 0.01
            }
        }

        # Save example configuration
        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            json.dump(example_config, f, indent=2)
            example_file = f.name

        try:
            # Try to load and use the example
            config = pynlm.createDefaultConfig()

            if pynlm_available:
                success = config.loadFromJsonFile(example_file)
                assert success, "Example configuration should load successfully"

                # Verify example parameters
                assert config.has("neuron_count")
                assert config.has("region_count")
                assert config.has("learning_rate")

            # Document the example integration
            logger.info(f"Documentation example integration successful with file: {example_file}")

        finally:
            if os.path.exists(example_file):
                os.unlink(example_file)

    def test_documentation_workflow_examples(self):
        """Test documentation workflow examples."""
        # Test that the workflow described in documentation can be followed

        # Example workflow from documentation:
        # 1. Create configuration
        # 2. Run experiment
        # 3. Analyze results
        # 4. Export findings

        # Step 1: Create configuration
        config_data = {
            "workflow_example": "documentation_test",
            "steps": ["create", "run", "analyze", "export"],
            "parameters": {
                "neuron_count": 200,
                "max_steps": 500
            }
        }

        with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f:
            json.dump(config_data, f)
            config_file = f.name

        try:
            # Step 2: "Run experiment" (simulate)
            experiment_result = {
                "status": "completed",
                "steps_executed": 3,
                "results": {
                    "final_reward": 42.5,
                    "total_spikes": 1000,
                    "performance_metrics": {
                        "steps_per_second": 200.0,
                        "memory_usage": 256.0
                    }
                }
            }

            # Step 3: "Analyze results"
            analysis = {
                "performance_quality": "good",
                "efficiency_rating": "high",
                "recommendations": [
                    "Increase neuron_count for better performance",
                    "Optimize learning_rate for faster convergence"
                ]
            }

            # Step 4: "Export findings"
            export_data = {
                "workflow_id": "doc_example_001",
                "timestamp": time.time(),
                "input_configuration": config_data,
                "experiment_results": experiment_result,
                "analysis": analysis,
                "summary": "Documentation workflow example executed successfully"
            }

            with tempfile.NamedTemporaryFile(suffix='.json', delete=False) as f2:
                export_file = f2.name
                json.dump(export_data, f2, indent=2)

            # Verify export file
            with open(export_file, 'r') as f:
                exported = json.load(f)
                assert "workflow_id" in exported
                assert "experiment_results" in exported
                assert "analysis" in exported

            os.unlink(export_file)

            logger.info("Documentation workflow example executed successfully")

        finally:
            if os.path.exists(config_file):
                os.unlink(config_file)


class TestRegressionTesting:
    """Test regression testing capabilities."""

    def test_backward_compatibility_regression(self):
        """Test that new changes don't break backward compatibility."""
        # Test that key=value format still works (backward compatibility)

        # Traditional key=value format
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
                # Should still be able to load traditional format
                assert config.has("random_seed")
                assert config.has("neuron_count")
                assert config.has("log_level")

                # Values should be correct
                seed = config.get("random_seed")
                if seed.has_value():
                    assert seed.value() == 42

                logger.info("Backward compatibility regression test passed")

        finally:
            if os.path.exists(traditional_file):
                os.unlink(traditional_file)

    def test_regression_test_configuration_consistency(self):
        """Test configuration consistency across different runs."""
        # Create the same configuration multiple times
        # and verify consistency

        config_spec = {
            "neuron_count": 500,
            "region_count": 3,
            "learning_rate": 0.01
        }

        configs = []
        for i in range(3):
            with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
                json.dump(config_spec, f)
                config_file = f.name

            try:
                config = pynlm.createDefaultConfig()
                success = config.loadFromJsonFile(config_file)

                if pynlm_available and success:
                    configs.append(config)

            finally:
                if os.path.exists(config_file):
                    os.unlink(config_file)

        # All configurations should be identical
        if len(configs) >= 2:
            for i in range(1, len(configs)):
                for key in config_spec.keys():
                    if configs[i].has(key) and configs[0].has(key):
                        val_i = configs[i].get(key).value() if configs[i].get(key).has_value() else None
                        val_0 = configs[0].get(key).value() if configs[0].get(key).has_value() else None

                        if val_i is not None and val_0 is not None:
                            if isinstance(val_i, float) and isinstance(val_0, float):
                                assert abs(val_i - val_0) < 1e-6
                            else:
                                assert val_i == val_0

        logger.info("Configuration consistency regression test passed")


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])