#!/usr/bin/env python3
"""
Performance Benchmark Tests for NLM Project

Tests for performance benchmarking, benchmarking harnesses, and performance comparisons.
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

# Import profiling modules
from nlm_profiling.benchmark import (
    BenchmarkHarnessModule, 
    BenchmarkConfig, 
    Configuration, 
    BenchmarkType,
    BenchmarkCLInterface
)
from nlm_profiling.base import (
    PerformanceMetrics, 
    BenchmarkResult, 
    ProfilingFramework, 
    OutputFormat
)

# Import test utilities
from .conftest import (
    test_config,
    pynlm_available,
    mock_pynlm,
    mock_brain,
    mock_performance_metrics,
    performance_timer,
    test_logger,
    capture_test_output,
    generate_performance_data
)

# Benchmark test data
SAMPLE_BENCHMARK_RESULTS = {
    "creation_time": 0.5,
    "initialization_time": 0.2,
    "step_time": 0.001,
    "steps_per_second": 1000.0,
    "memory_usage": 150.5,
    "cpu_usage": 25.3,
    "spike_count": 10000,
    "neuron_count": 1000,
    "synapse_count": 5000,
    "firing_rate": 2.5,
    "development_stage": "mature"
}

SAMPLE_BENCHMARK_CONFIGS = [
    BenchmarkConfig(
        benchmark_type=BenchmarkType.CREATION,
        steps=100,
        runs=3,
        warmup_steps=10,
        config_file=None
    ),
    BenchmarkConfig(
        benchmark_type=BenchmarkType.STEP_PERFORMANCE,
        steps=500,
        runs=5,
        warmup_steps=20,
        config_file="benchmark.json"
    ),
    BenchmarkConfig(
        benchmark_type=BenchmarkType.COMPARATIVE,
        steps=200,
        runs=2,
        warmup_steps=5,
        config_file=None
    )
]

SAMPLE_BRAIN_CONFIGURATIONS = [
    Configuration("default", {
        "neuron_count": 100,
        "region_count": 2,
        "plasticity": True,
        "learning_rate": 0.01
    }),
    Configuration("high_performance", {
        "neuron_count": 500,
        "region_count": 3,
        "plasticity": True,
        "learning_rate": 0.001,
        "simd_enabled": True
    }),
    Configuration("optimized", {
        "neuron_count": 200,
        "region_count": 2,
        "plasticity": True,
        "learning_rate": 0.005,
        "optimization_level": "aggressive"
    }),
    Configuration("minimal", {
        "neuron_count": 50,
        "region_count": 1,
        "plasticity": False,
        "learning_rate": 0.1
    })
]


def pytest_configure(config):
    """Configure pytest for performance benchmark tests."""
    config.addinivalue_line("markers", "benchmark: benchmark tests")
    config.addinivalue_line("markers", "performance: performance tests")
    config.addinivalue_line("markers", "profiling: profiling tests")
    config.addinivalue_line("markers", "timing: timing tests")
    config.addinivalue_line("markers", "comparison: comparison tests")
    config.addinivalue_line("markers", "slow: tests that take longer to run")
    config.addinivalue_line("markers", "performance_regression: performance regression tests")


class TestBenchmarkSetup:
    """Test benchmark setup and configuration."""

    def test_benchmark_config_creation(self):
        """Test BenchmarkConfig object creation."""
        config = BenchmarkConfig(
            benchmark_type=BenchmarkType.CREATION,
            steps=100,
            runs=3,
            warmup_steps=10,
            config_file="test.json"
        )

        assert config.benchmark_type == BenchmarkType.CREATION
        assert config.steps == 100
        assert config.runs == 3
        assert config.warmup_steps == 10
        assert config.config_file == "test.json"

    def test_benchmark_config_types(self):
        """Test all benchmark configuration types."""
        benchmark_types = [
            BenchmarkType.CREATION,
            BenchmarkType.STEP_PERFORMANCE,
            BenchmarkType.CONFIGURATION,
            BenchmarkType.COMPARATIVE
        ]

        for bench_type in benchmark_types:
            config = BenchmarkConfig(benchmark_type=bench_type)
            assert config.benchmark_type == bench_type
            assert config.benchmark_type.value == bench_type.value

    def test_benchmark_config_to_dict(self):
        """Test BenchmarkConfig to_dict method."""
        config = BenchmarkConfig(
            benchmark_type=BenchmarkType.STEP_PERFORMANCE,
            steps=200,
            runs=5,
            warmup_steps=20
        )

        config_dict = config.to_dict()
        assert config_dict["benchmark_type"] == "step_performance"
        assert config_dict["steps"] == 200
        assert config_dict["runs"] == 5
        assert config_dict["warmup_steps"] == 20
        assert config_dict["config_file"] is None

    def test_benchmark_config_defaults(self):
        """Test BenchmarkConfig default values."""
        config = BenchmarkConfig()

        assert config.benchmark_type == BenchmarkType.CREATION
        assert config.steps == 100
        assert config.runs == 3
        assert config.warmup_steps == 10
        assert config.config_file is None

    @pytest.mark.benchmark
    def test_benchmark_config_validation(self):
        """Test benchmark configuration validation."""
        # Test valid configurations
        valid_configs = [
            BenchmarkConfig(steps=1, runs=1),
            BenchmarkConfig(steps=1000, runs=10, warmup_steps=100),
        ]

        for config in valid_configs:
            assert config.steps > 0
            assert config.runs > 0
            assert config.warmup_steps >= 0

        # Test edge cases
        config = BenchmarkConfig(steps=1, runs=1, warmup_steps=0)
        assert config.warmup_steps == 0


class TestBenchmarkConfigurationManagement:
    """Test benchmark configuration management."""

    def test_configuration_creation(self):
        """Test Configuration object creation."""
        config = Configuration("test_config", {"neuron_count": 100})

        assert config.name == "test_config"
        assert config.parameters == {"neuron_count": 100}

    def test_configuration_to_dict(self):
        """Test Configuration to_dict method."""
        config = Configuration("test", {"key1": "value1", "key2": 42})

        config_dict = config.to_dict()
        assert config_dict["name"] == "test"
        assert config_dict["parameters"] == {"key1": "value1", "key2": 42}

    def test_multiple_configurations(self):
        """Test multiple configuration creation."""
        configurations = [
            Configuration(f"config_{i}", {"value": i})
            for i in range(10)
        ]

        assert len(configurations) == 10
        for i, config in enumerate(configurations):
            assert config.name == f"config_{i}"
            assert config.parameters == {"value": i}

    def test_configuration_with_complex_parameters(self):
        """Test configuration with complex parameters."""
        complex_config = {
            "nested": {
                "inner": "value",
                "list": [1, 2, 3],
                "dict": {"key": "value"}
            },
            "arrays": [1, 2, 3, 4, 5],
            "booleans": [True, False],
            "nulls": [None, "value", 42]
        }

        config = Configuration("complex", complex_config)
        assert config.parameters == complex_config


class TestBenchmarkHarnessSetup:
    """Test benchmark harness setup and basic functionality."""

    def test_harness_initialization(self):
        """Test benchmark harness initialization."""
        harness = BenchmarkHarnessModule()

        assert harness.name == "benchmark_harness"
        assert harness.config is not None
        assert len(harness.benchmark_results) == 0
        assert harness.results is None

    def test_harness_with_custom_config(self):
        """Test benchmark harness with custom configuration."""
        custom_config = BenchmarkConfig(
            benchmark_type=BenchmarkType.COMPARATIVE,
            steps=50,
            runs=2,
            warmup_steps=5
        )

        harness = BenchmarkHarnessModule(custom_config)
        assert harness.config == custom_config

    @pytest.mark.benchmark
    def test_harness_configuration_management(self):
        """Test benchmark harness configuration management."""
        harness = BenchmarkHarnessModule()

        # Add multiple configurations
        configurations = [
            Configuration("config1", {"value": 1}),
            Configuration("config2", {"value": 2}),
            Configuration("config3", {"value": 3})
        ]

        for config in configurations:
            harness.add_configuration(config)

        # Verify configurations were added
        assert len(harness.benchmark_results) == 3

        # Test removal
        assert harness.remove_configuration("config2") is True
        assert len(harness.benchmark_results) == 2

        assert harness.remove_configuration("nonexistent") is False

        # Test clearing
        harness.clear_configurations()
        assert len(harness.benchmark_results) == 0

    def test_harness_configuration_retrieval(self):
        """Test benchmark harness configuration retrieval."""
        harness = BenchmarkHarnessModule()

        # Add configurations
        config1 = Configuration("config1", {"value": 1})
        config2 = Configuration("config2", {"value": 2})

        harness.add_configuration(config1)
        harness.add_configuration(config2)

        # Test get_configurations
        configs = harness.get_configurations()

        assert len(configs) == 2
        assert configs[0] == config1
        assert configs[1] == config2

        # Should return a copy, not reference
        configs[0] = Configuration("modified", {"value": 999})
        assert harness.benchmark_results[0] == config1  # Original unchanged

    def test_harness_configuration_consistency(self):
        """Test benchmark harness configuration consistency."""
        harness = BenchmarkHarnessModule()

        # Add configurations with duplicate names
        config1 = Configuration("same_name", {"value": 1})
        config2 = Configuration("same_name", {"value": 2})  # Same name

        harness.add_configuration(config1)
        harness.add_configuration(config2)

        # Both should be added (duplicate names allowed)
        assert len(harness.benchmark_results) == 2
        assert harness.benchmark_results[0] == config1
        assert harness.benchmark_results[1] == config2


class TestBenchmarkExecution:
    """Test benchmark execution and result generation."""

    def test_benchmark_result_creation(self):
        """Test BenchmarkResult object creation."""
        metrics = [
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec"),
            PerformanceMetrics(time.time(), "memory_usage", 256.0, "MB")
        ]

        result = BenchmarkResult(
            test_name="test_benchmark",
            parameters={"steps": 100},
            metrics=metrics,
            duration=5.0,
            success=True
        )

        assert result.test_name == "test_benchmark"
        assert result.parameters == {"steps": 100}
        assert len(result.metrics) == 2
        assert result.duration == 5.0
        assert result.success is True
        assert result.error is None

    def test_benchmark_result_with_error(self):
        """Test BenchmarkResult with error state."""
        result = BenchmarkResult(
            test_name="failed_benchmark",
            parameters={},
            metrics=[],
            duration=0.0,
            success=False,
            error="Benchmark failed with error"
        )

        assert result.success is False
        assert result.error == "Benchmark failed with error"

    def test_benchmark_result_to_dict(self):
        """Test BenchmarkResult to_dict method."""
        metrics = [
            PerformanceMetrics(time.time(), "metric1", 100.0, "unit1")
        ]

        result = BenchmarkResult(
            test_name="test",
            parameters={"param1": "value1"},
            metrics=metrics,
            duration=1.0,
            success=True
        )

        result_dict = result.to_dict()
        assert result_dict["test_name"] == "test"
        assert result_dict["parameters"] == {"param1": "value1"}
        assert len(result_dict["metrics"]) == 1
        assert result_dict["duration"] == 1.0
        assert result_dict["success"] is True

    @pytest.mark.benchmark
    def test_benchmark_result_performance_metrics(self):
        """Test benchmark result performance metrics."""
        # Create a result with various performance metrics
        metrics = [
            PerformanceMetrics(time.time(), "creation_time", 0.5, "seconds"),
            PerformanceMetrics(time.time(), "initialization_time", 0.2, "seconds"),
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec"),
            PerformanceMetrics(time.time(), "memory_usage", 256.0, "MB"),
            PerformanceMetrics(time.time(), "cpu_usage", 25.3, "percent")
        ]

        result = BenchmarkResult(
            test_name="performance_benchmark",
            parameters={"steps": 1000},
            metrics=metrics,
            duration=2.0,
            success=True
        )

        # Verify all metrics are present
        metric_names = [m.metric_name for m in result.metrics]
        assert "creation_time" in metric_names
        assert "initialization_time" in metric_names
        assert "steps_per_second" in metric_names
        assert "memory_usage" in metric_names
        assert "cpu_usage" in metric_names

        # Calculate statistics
        steps_per_second_values = [m.value for m in result.metrics if m.metric_name == "steps_per_second"]
        assert len(steps_per_second_values) == 1
        assert steps_per_second_values[0] == 1000.0

        memory_usage_values = [m.value for m in result.metrics if m.metric_name == "memory_usage"]
        assert len(memory_usage_values) == 1
        assert memory_usage_values[0] == 256.0


class TestBenchmarkInsights:
    """Test benchmark insights generation."""

    def test_insights_generation_basic(self):
        """Test basic insights generation."""
        # Create a mock harness
        harness = BenchmarkHarnessModule()

        # Create a mock benchmark result
        metrics = [
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec"),
            PerformanceMetrics(time.time(), "steps_per_second", 1200.0, "steps/sec")
        ]

        result = BenchmarkResult(
            test_name="test_insights",
            parameters={},
            metrics=metrics,
            duration=5.0,
            success=True
        )

        insights = harness.generate_benchmark_insights(result)

        # Verify insights structure
        assert "configurations_benchmarked" in insights
        assert "average_performance" in insights
        assert "performance_analysis" in insights
        assert "recommendations" in insights

        # Verify values
        assert insights["configurations_benchmarked"] == 2
        assert insights["average_performance"] == 1100.0  # (1000 + 1200) / 2
        assert "recommendations" in insights

    def test_insights_performance_analysis(self):
        """Test performance analysis in insights."""
        harness = BenchmarkHarnessModule()

        # Test with high performance improvement
        insights = {
            "configurations_benchmarked": 2,
            "average_performance": 100.0,
            "best_configuration_performance": 200.0,
            "performance_improvement_percentage": 100.0
        }

        recommendations = harness._generate_benchmark_recommendations(insights)

        assert len(recommendations) > 0
        assert any("significant optimizations" in rec.lower() for rec in recommendations)

        # Test with moderate performance improvement
        insights["performance_improvement_percentage"] = 15.0
        insights["performance_analysis"] = "Good performance variation - moderate optimization opportunities"

        recommendations = harness._generate_benchmark_recommendations(insights)

        assert any("moderate" in rec.lower() for rec in recommendations)

        # Test with low performance improvement
        insights["performance_improvement_percentage"] = 5.0
        insights["performance_analysis"] = "Acceptable performance variation - minor optimizations available"

        recommendations = harness._generate_benchmark_recommendations(insights)

        assert any("minor" in rec.lower() for rec in recommendations)

    def test_insights_error_handling(self):
        """Test insights generation error handling."""
        harness = BenchmarkHarnessModule()

        # Create result with no performance metrics
        result = BenchmarkResult(
            test_name="no_performance",
            parameters={},
            metrics=[],
            duration=0.0,
            success=True
        )

        insights = harness.generate_benchmark_insights(result)

        # Should have error
        assert "error" in insights
        assert "Failed to generate insights" in insights["error"]

    @pytest.mark.benchmark
    def test_insights_with_real_data(self):
        """Test insights generation with realistic performance data."""
        harness = BenchmarkHarnessModule()

        # Create performance metrics based on realistic scenarios
        metrics = []

        # Simulate different benchmark configurations
        scenarios = [
            {"steps_per_second": 500, "memory": 128, "cpu": 15},
            {"steps_per_second": 800, "memory": 200, "cpu": 25},
            {"steps_per_second": 1200, "memory": 300, "cpu": 35},
            {"steps_per_second": 2000, "memory": 500, "cpu": 50}
        ]

        for i, scenario in enumerate(scenarios):
            metrics.append(PerformanceMetrics(
                time.time(), "steps_per_second", scenario["steps_per_second"], "steps/sec"
            ))
            metrics.append(PerformanceMetrics(
                time.time(), "memory_usage", scenario["memory"], "MB"
            ))
            metrics.append(PerformanceMetrics(
                time.time(), "cpu_usage", scenario["cpu"], "percent"
            ))

        result = BenchmarkResult(
            test_name="realistic_benchmark",
            parameters={"scenarios": len(scenarios)},
            metrics=metrics,
            duration=10.0,
            success=True
        )

        insights = harness.generate_benchmark_insights(result)

        # Verify insights are meaningful
        assert insights["configurations_benchmarked"] == 4
        assert insights["average_performance"] > 0
        assert insights["best_configuration_performance"] > 0
        assert "performance_analysis" in insights
        assert len(insights["recommendations"]) > 0


class TestBenchmarkComparison:
    """Test benchmark comparison functionality."""

    def test_comparative_report_generation(self):
        """Test comparative report generation."""
        # Create multiple benchmark results for comparison
        results = []

        for i in range(3):
            metrics = [
                PerformanceMetrics(time.time(), "steps_per_second", 1000.0 * (i + 1), "steps/sec"),
                PerformanceMetrics(time.time(), "memory_usage", 100.0 * (i + 1), "MB")
            ]

            result = BenchmarkResult(
                test_name=f"benchmark_{i}",
                parameters={"run": i, "config_type": f"type_{i}"},
                metrics=metrics,
                duration=5.0 * (i + 1),
                success=True
            )
            results.append(result)

        # Create harness and generate comparative report
        harness = BenchmarkHarnessModule()
        report = harness.generate_comparative_report(results)

        # Verify report structure
        assert "total_benchmarks" in report
        assert "successful_benchmarks" in report
        assert "failed_benchmarks" in report
        assert "average_duration" in report

        assert report["total_benchmarks"] == 3
        assert report["successful_benchmarks"] == 3
        assert report["failed_benchmarks"] == 0

        # Verify aggregated metrics
        assert "steps_per_second_average" in report
        assert "steps_per_second_min" in report
        assert "steps_per_second_max" in report
        assert "steps_per_second_stddev" in report

        assert "memory_usage_average" in report
        assert "memory_usage_min" in report
        assert "memory_usage_max" in report
        assert "memory_usage_stddev" in report

        # Verify calculations
        assert report["steps_per_second_average"] == 2000.0  # (1000 + 2000 + 3000) / 3
        assert report["steps_per_second_min"] == 1000.0
        assert report["steps_per_second_max"] == 3000.0

        assert report["memory_usage_average"] == 200.0  # (100 + 200 + 300) / 3
        assert report["memory_usage_min"] == 100.0
        assert report["memory_usage_max"] == 300.0

    def test_comparison_empty_results(self):
        """Test comparison with empty results."""
        harness = BenchmarkHarnessModule()
        report = harness.generate_comparative_report([])

        assert report == {}, "Empty results should return empty report"

    @pytest.mark.benchmark
    def test_comparison_with_failures(self):
        """Test comparison with some failed benchmarks."""
        results = []

        # Successful benchmark
        metrics1 = [
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec")
        ]
        result1 = BenchmarkResult(
            test_name="successful_benchmark",
            parameters={},
            metrics=metrics1,
            duration=5.0,
            success=True
        )
        results.append(result1)

        # Failed benchmark
        result2 = BenchmarkResult(
            test_name="failed_benchmark",
            parameters={},
            metrics=[],
            duration=0.0,
            success=False,
            error="Benchmark failed"
        )
        results.append(result2)

        # Create harness and generate report
        harness = BenchmarkHarnessModule()
        report = harness.generate_comparative_report(results)

        # Verify report includes failed benchmarks
        assert report["total_benchmarks"] == 2
        assert report["successful_benchmarks"] == 1
        assert report["failed_benchmarks"] == 1

    def test_comparison_different_metric_types(self):
        """Test comparison with different metric types."""
        # Create results with different metric types
        results = []

        for i in range(2):
            metrics = [
                PerformanceMetrics(time.time(), f"metric_{i}_1", 100.0 * (i + 1), "unit1"),
                PerformanceMetrics(time.time(), f"metric_{i}_2", 200.0 * (i + 1), "unit2"),
                PerformanceMetrics(time.time(), f"metric_{i}_3", 300.0 * (i + 1), "unit3")
            ]

            result = BenchmarkResult(
                test_name=f"benchmark_{i}",
                parameters={"metric_set": i},
                metrics=metrics,
                duration=1.0 * (i + 1),
                success=True
            )
            results.append(result)

        harness = BenchmarkHarnessModule()
        report = harness.generate_comparative_report(results)

        # Verify all metric types are included
        for i in range(2):
            for j in range(3):
                metric_key = f"metric_{i}_{j}_average"
                assert metric_key in report, f"Metric {metric_key} should be in report"

                metric_min_key = f"metric_{i}_{j}_min"
                assert metric_min_key in report, f"Metric {metric_min_key} should be in report"

                metric_max_key = f"metric_{i}_{j}_max"
                assert metric_max_key in report, f"Metric {metric_max_key} should be in report"

                metric_stddev_key = f"metric_{i}_{j}_stddev"
                assert metric_stddev_key in report, f"Metric {metric_stddev_key} should be in report"


class TestBenchmarkExport:
    """Test benchmark export functionality."""

    def test_export_results_json(self):
        """Test exporting benchmark results to JSON."""
        harness = BenchmarkHarnessModule()

        # Create a mock result
        metrics = [
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec"),
            PerformanceMetrics(time.time(), "memory_usage", 256.0, "MB")
        ]

        result = BenchmarkResult(
            test_name="export_test",
            parameters={"format": "json"},
            metrics=metrics,
            duration=5.0,
            success=True
        )

        # Set harness result
        harness.results = result

        # Export to JSON
        json_output = harness.export_results(OutputFormat.JSON)

        # Parse and verify
        parsed = json.loads(json_output)

        assert "test_name" in parsed
        assert "parameters" in parsed
        assert "metrics" in parsed
        assert "duration" in parsed
        assert "success" in parsed

        assert parsed["test_name"] == "export_test"
        assert parsed["parameters"]["format"] == "json"
        assert len(parsed["metrics"]) == 2
        assert parsed["duration"] == 5.0
        assert parsed["success"] is True

    def test_export_results_with_insights(self):
        """Test exporting benchmark results with insights."""
        harness = BenchmarkHarnessModule()

        # Create a mock result
        metrics = [
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec")
        ]

        result = BenchmarkResult(
            test_name="export_with_insights",
            parameters={},
            metrics=metrics,
            duration=3.0,
            success=True
        )

        # Set harness result
        harness.results = result

        # Mock insights
        harness.generate_benchmark_insights = Mock(return_value={
            "performance_analysis": "Good performance",
            "recommendations": ["Test recommendation"]
        })

        # Export to JSON with insights
        json_output = harness.export_results(OutputFormat.JSON)

        parsed = json.loads(json_output)

        # Verify insights are included
        assert "benchmark_insights" in parsed
        assert "performance_analysis" in parsed["benchmark_insights"]
        assert "recommendations" in parsed["benchmark_insights"]

    def test_export_results_yaml(self):
        """Test exporting benchmark results to YAML."""
        harness = BenchmarkHarnessModule()

        # Create a mock result
        metrics = [
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec")
        ]

        result = BenchmarkResult(
            test_name="yaml_export_test",
            parameters={"format": "yaml"},
            metrics=metrics,
            duration=2.0,
            success=True
        )

        # Set harness result
        harness.results = result

        # Export to YAML
        yaml_output = harness.export_results(OutputFormat.YAML)

        # Parse and verify (yaml parsing is manual for this test)
        # Check that it contains key elements
        assert "steps_per_second" in yaml_output
        assert "1000.0" in yaml_output
        assert "export_test" in yaml_output

    def test_export_results_no_results(self):
        """Test exporting when no results are available."""
        harness = BenchmarkHarnessModule()

        # No results set
        harness.results = None

        # Export should handle gracefully
        json_output = harness.export_results(OutputFormat.JSON)

        # Should return a message indicating no results
        assert "No results to export" in json_output or json_output.strip() == "{}"


class TestBenchmarkCLI:
    """Test benchmark CLI interface."""

    def setup_method(self):
        """Setup before each test method."""
        self.cli = BenchmarkCLInterface()

    def test_cli_sample_configuration_creation(self):
        """Test CLI sample configuration creation."""
        # Test different configuration types
        configs_to_test = ["default", "high_performance", "minimal", "balanced"]

        for config_type in configs_to_test:
            config = self.cli.create_sample_configuration(config_type)

            assert config.name is not None
            assert config.parameters is not None
            assert len(config.parameters) > 0

            # Verify configuration-specific parameters
            if config_type == "default":
                assert config.parameters["neuron_count"] == 100
            elif config_type == "high_performance":
                assert config.parameters["neuron_count"] == 500
                assert config.parameters["simd_enabled"] is True
            elif config_type == "minimal":
                assert config.parameters["plasticity"] is False
            elif config_type == "balanced":
                assert config.parameters["optimization_level"] == "balanced"

    def test_cli_sample_configuration_fallback(self):
        """Test CLI sample configuration fallback."""
        # Test that unknown configuration type falls back to default
        config = self.cli.create_sample_configuration("unknown_type")

        assert config.name == "default", "Should fallback to default configuration"

    def test_cli_parameter_parsing(self):
        """Test CLI parameter parsing."""
        # This would test the actual CLI parameter parsing
        # For now, we'll test that the CLI interface methods exist
        assert hasattr(self.cli, 'create_sample_configuration')
        assert hasattr(self.cli, 'run_benchmark')

    def test_cli_error_handling(self):
        """Test CLI error handling."""
        # Test error handling in CLI methods
        # For unknown configuration type
        config = self.cli.create_sample_configuration("unknown")
        assert config.name == "default"


class TestBenchmarkTiming:
    """Test benchmark timing and performance measurement."""

    def test_timing_accuracy(self):
        """Test timing accuracy."""
        import time

        timer = performance_timer

        # Test timing a short operation
        timer.start()
        time.sleep(0.001)  # 1ms
        duration = timer.stop()

        assert duration is not None
        assert duration >= 0.0005
        assert duration <= 0.1

    def test_timing_multiple_operations(self):
        """Test timing multiple operations."""
        import time

        timer = performance_timer

        # Time multiple operations
        durations = []
        for i in range(5):
            timer.start()
            time.sleep(0.001 * (i + 1))  # Varying delays
            duration = timer.stop()
            durations.append(duration)

        # Calculate statistics
        mean_duration = statistics.mean(durations)
        stddev_duration = statistics.stdev(durations) if len(durations) > 1 else 0.0

        assert mean_duration >= 0.001
        assert mean_duration <= 0.05

        # Check that timing is reasonably consistent
        coefficient_of_variation = stddev_duration / mean_duration if mean_duration > 0 else 0
        assert coefficient_of_variation < 1.0, f"Timing too inconsistent: {coefficient_of_variation:.1%}"

    def test_timing_precision(self):
        """Test timing precision."""
        timer = performance_timer

        # Time very short operations
        short_durations = []
        for i in range(10):
            timer.start()
            # Do a small computation
            _ = sum(range(100))
            duration = timer.stop()
            short_durations.append(duration)

        # Very short operations should have some consistency
        mean_duration = statistics.mean(short_durations)
        stddev_duration = statistics.stdev(short_durations) if len(short_durations) > 1 else 0.0

        assert mean_duration > 0.0001, f"Too slow: {mean_duration:.6f}s"
        assert mean_duration < 0.01, f"Too slow: {mean_duration:.6f}s"

        # Coefficient of variation should be reasonable
        coefficient_of_variation = stddev_duration / mean_duration if mean_duration > 0 else 0
        assert coefficient_of_variation < 1.0, f"Too inconsistent: {coefficient_of_variation:.1%}"

    @pytest.mark.performance_regression
    def test_timing_regression(self):
        """Test for timing regressions."""
        # This test would run timing benchmarks over time to detect regressions
        # For now, we'll run a single timing test

        timer = performance_timer

        # Time configuration operations
        start_time = time.perf_counter()

        config = BenchmarkConfig(steps=100, runs=3)
        harness = BenchmarkHarnessModule(config)

        end_time = time.perf_counter()
        config_time = end_time - start_time

        # Configuration setup should be fast
        assert config_time < 0.1, f"Configuration setup too slow: {config_time:.3f}s"

        # Time metrics operations
        start_time = time.perf_counter()

        metrics = [
            PerformanceMetrics(time.time(), "test_metric", 1.0, "unit"),
            PerformanceMetrics(time.time(), "test_metric2", 2.0, "unit2")
        ]

        end_time = time.perf_counter()
        metrics_time = end_time - start_time

        # Metrics operations should be fast
        assert metrics_time < 0.01, f"Metrics operations too slow: {metrics_time:.6f}s"

        logger.info(f"Performance regression test - Config setup: {config_time:.3f}s, Metrics: {metrics_time:.6f}s")


class TestBenchmarkIntegration:
    """Test benchmark integration with other components."""

    def test_benchmark_with_nlm_integration(self):
        """Test benchmark integration with NLM (if available)."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")

        # Create NLM configuration
        config = pynlm.createDefaultConfig()
        config.set("neuron_count", 200)
        config.set("region_count", 2)

        # Create benchmark harness
        harness_config = BenchmarkConfig(
            benchmark_type=BenchmarkType.STEP_PERFORMANCE,
            steps=100,
            runs=2,
            warmup_steps=10
        )

        harness = BenchmarkHarnessModule(harness_config)

        # Test integration with mock brain
        mock_brain = Mock()
        mock_brain.getTotalNeuronCount.return_value = 200
        mock_brain.getRegionCount.return_value = 2
        mock_brain.initialize.return_value = True
        mock_brain.step.return_value = None

        with patch('pynlm.createBrain', return_value=mock_brain):
            with patch('pynlm.Config', return_value=Mock()):
                # This would run actual benchmark (mocked)
                result = harness.run_profiling()

        assert result.success is True
        assert result.duration >= 0
        assert len(result.metrics) > 0

    def test_benchmark_with_configuration_system(self):
        """Test benchmark integration with configuration system."""
        # Test that benchmarks can use configuration system data
        harness = BenchmarkHarnessModule()

        # Create a configuration based on external data source
        external_config = {
            "neuron_count": 500,
            "region_count": 3,
            "learning_rate": 0.01,
            "optimization_level": "high"
        }

        # Convert to benchmark configuration
        benchmark_config = Configuration("external_config", external_config)
        harness.add_configuration(benchmark_config)

        # Verify integration
        configs = harness.get_configurations()
        assert len(configs) == 1
        assert configs[0].name == "external_config"
        assert configs[0].parameters == external_config

    @pytest.mark.benchmark
    def test_benchmark_with_performance_metrics(self):
        """Test benchmark integration with performance metrics."""
        # Test that benchmarks can work with performance metrics
        harness = BenchmarkHarnessModule()

        # Create performance metrics for benchmarking
        performance_data = [
            {"steps_per_second": 1000, "memory": 256, "cpu": 25},
            {"steps_per_second": 1200, "memory": 300, "cpu": 30},
            {"steps_per_second": 800, "memory": 200, "cpu": 20}
        ]

        # Create benchmark results from performance data
        results = []
        for i, data in enumerate(performance_data):
            metrics = [
                PerformanceMetrics(time.time(), "steps_per_second", data["steps_per_second"], "steps/sec"),
                PerformanceMetrics(time.time(), "memory_usage", data["memory"], "MB"),
                PerformanceMetrics(time.time(), "cpu_usage", data["cpu"], "percent")
            ]

            result = BenchmarkResult(
                test_name=f"perf_test_{i}",
                parameters=data,
                metrics=metrics,
                duration=1.0 + i,
                success=True
            )
            results.append(result)

        # Generate comparative report
        report = harness.generate_comparative_report(results)

        # Verify report includes performance metrics
        assert "steps_per_second_average" in report
        assert "memory_usage_average" in report
        assert "cpu_usage_average" in report

        # Verify performance is meaningful
        assert report["steps_per_second_average"] > 0
        assert report["memory_usage_average"] > 0
        assert report["cpu_usage_average"] > 0


class TestBenchmarkUtilities:
    """Test benchmark utility functions."""

    def test_performance_data_generation(self):
        """Test performance data generation."""
        data = generate_performance_data(10)

        assert len(data) == 10

        for item in data:
            assert "steps_per_second" in item
            assert "average_step_time" in item
            assert "neuron_count" in item
            assert "spike_count" in item
            assert "memory_usage" in item

            # Verify data ranges
            assert 50 <= item["steps_per_second"] <= 150
            assert 0.001 <= item["average_step_time"] <= 0.01
            assert 500 <= item["neuron_count"] <= 2000
            assert 1000 <= item["spike_count"] <= 10000
            assert 100 <= item["memory_usage"] <= 500

    def test_performance_data_statistics(self):
        """Test performance data statistics."""
        data = generate_performance_data(20)

        # Calculate statistics
        steps_per_second_values = [item["steps_per_second"] for item in data]
        mean_spss = statistics.mean(steps_per_second_values)
        stddev_spss = statistics.stdev(steps_per_second_values) if len(steps_per_second_values) > 1 else 0.0

        # Verify statistics
        assert mean_spss > 0
        assert stddev_spss > 0
        assert stddev_spss < mean_spss  # Coefficient of variation < 100%

    def test_utility_functions(self):
        """Test various utility functions."""
        # Test create_test_config_file
        from .conftest import create_test_config_file, cleanup_test_file

        content = "test = value\nnumber = 42"
        filepath = create_test_config_file(content, ".test")

        try:
            assert filepath.exists()
            with open(filepath, 'r') as f:
                file_content = f.read()
                assert "test = value" in file_content
                assert "number = 42" in file_content
        finally:
            cleanup_test_file(filepath)

        # Test cleanup_test_file
        filepath2 = create_test_config_file("test", ".test2")
        assert filepath2.exists()
        cleanup_test_file(filepath2)
        assert not filepath2.exists()


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])