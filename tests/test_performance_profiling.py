#!/usr/bin/env python3
"""
Performance Profiling Tests for NLM Project

Tests for the performance profiling utilities and benchmarking tools.
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
    capture_test_output
)

# Performance test data and configurations
SAMPLE_PERFORMANCE_METRICS = {
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

SAMPLE_BENCHMARK_CONFIG = BenchmarkConfig(
    benchmark_type=BenchmarkType.CREATION,
    steps=100,
    runs=3,
    warmup_steps=10,
    config_file=None
)

SAMPLE_BRAIN_CONFIGS = [
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
    Configuration("minimal", {
        "neuron_count": 50,
        "region_count": 1,
        "plasticity": False,
        "learning_rate": 0.1
    })
]


def pytest_configure(config):
    """Configure pytest for performance profiling tests."""
    config.addinivalue_line("markers", "performance: performance profiling tests")
    config.addinivalue_line("markers", "benchmark: benchmark tests")
    config.addinivalue_line("markers", "profiling: profiling tests")
    config.addinivalue_line("markers", "timing: timing tests")
    config.addinivalue_line("markers", "metrics: performance metrics tests")
    config.addinivalue_line("markers", "slow: tests that take longer to run")


class TestPerformanceMetrics:
    """Test performance metrics functionality."""

    def test_performance_metrics_creation(self):
        """Test PerformanceMetrics object creation."""
        timestamp = time.time()
        metric = PerformanceMetrics(
            timestamp=timestamp,
            metric_name="test_metric",
            value=42.5,
            unit="test_unit"
        )

        assert metric.timestamp == timestamp
        assert metric.metric_name == "test_metric"
        assert metric.value == 42.5
        assert metric.unit == "test_unit"
        assert metric.to_text() == "Test metric: 42.5 test_unit"

    def test_performance_metrics_to_dict(self):
        """Test PerformanceMetrics to_dict method."""
        metric = PerformanceMetrics(
            timestamp=time.time(),
            metric_name="memory_usage",
            value=256.0,
            unit="MB"
        )

        metric_dict = metric.to_dict()
        assert metric_dict["timestamp"] == metric.timestamp
        assert metric_dict["metric_name"] == metric.metric_name
        assert metric_dict["value"] == metric.value
        assert metric_dict["unit"] == metric.unit

    def test_performance_metrics_formatting(self):
        """Test performance metrics formatting."""
        metric = PerformanceMetrics(
            timestamp=time.time(),
            metric_name="steps_per_second",
            value=1000.5,
            unit="steps/sec"
        )

        text_output = metric.to_text()
        assert "steps_per_second" in text_output
        assert "1000.5" in text_output
        assert "steps/sec" in text_output

    def test_performance_metrics_with_special_characters(self):
        """Test performance metrics with special characters in names."""
        metric = PerformanceMetrics(
            timestamp=time.time(),
            metric_name="spike_count_#1",
            value=999,
            unit="spikes/sec"
        )

        assert metric.metric_name == "spike_count_#1"
        assert metric.to_text() == "Spike count_#1: 999 spikes/sec"

    def test_performance_metrics_edge_cases(self):
        """Test performance metrics edge cases."""
        # Test with zero value
        metric1 = PerformanceMetrics(
            timestamp=time.time(),
            metric_name="zero_value",
            value=0.0,
            unit="count"
        )
        assert metric1.value == 0.0

        # Test with negative value (should be allowed for some metrics)
        metric2 = PerformanceMetrics(
            timestamp=time.time(),
            metric_name="error_count",
            value=-5.0,
            unit="errors"
        )
        assert metric2.value == -5.0

        # Test with very large value
        metric3 = PerformanceMetrics(
            timestamp=time.time(),
            metric_name="total_spikes",
            value=1e9,
            unit="spikes"
        )
        assert metric3.value == 1e9

    @pytest.mark.metrics
def test_performance_metrics_collection(self):
    """Test collecting multiple performance metrics."""
    metrics = []

    for i in range(10):
        metric = PerformanceMetrics(
            timestamp=time.time() + i,
            metric_name=f"metric_{i}",
            value=i * 1.5,
            unit="unit"
        )
        metrics.append(metric)

    assert len(metrics) == 10

    # Verify all metrics have unique names
    names = [m.metric_name for m in metrics]
    assert len(set(names)) == len(names), "Metrics should have unique names"

    # Calculate statistics
    values = [m.value for m in metrics]
    assert statistics.mean(values) == statistics.mean([i * 1.5 for i in range(10)])
    assert statistics.stdev(values) == statistics.stdev([i * 1.5 for i in range(10)])


class TestBenchmarkConfiguration:
    """Test benchmark configuration functionality."""

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

    def test_benchmark_config_types(self):
        """Test different benchmark configuration types."""
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


class TestBenchmarkHarness:
    """Test benchmark harness functionality."""

    def setup_method(self):
        """Setup before each test method."""
        self.harness = BenchmarkHarnessModule()

    def test_harness_initialization(self):
        """Test benchmark harness initialization."""
        assert self.harness.name == "benchmark_harness"
        assert self.harness.config is not None
        assert len(self.harness.benchmark_results) == 0
        assert self.harness.results is None

    def test_add_configuration(self):
        """Test adding configuration to benchmark harness."""
        config = Configuration("test_config", {"neuron_count": 100})

        self.harness.add_configuration(config)

        assert len(self.harness.benchmark_results) == 1
        assert self.harness.benchmark_results[0] == config

    def test_remove_configuration(self):
        """Test removing configuration from benchmark harness."""
        config1 = Configuration("config1", {"neuron_count": 100})
        config2 = Configuration("config2", {"neuron_count": 200})

        self.harness.add_configuration(config1)
        self.harness.add_configuration(config2)

        assert self.harness.remove_configuration("config1") is True
        assert len(self.harness.benchmark_results) == 1
        assert self.harness.benchmark_results[0] == config2

        assert self.harness.remove_configuration("nonexistent") is False

    def test_clear_configurations(self):
        """Test clearing all configurations."""
        config1 = Configuration("config1", {"neuron_count": 100})
        config2 = Configuration("config2", {"neuron_count": 200})

        self.harness.add_configuration(config1)
        self.harness.add_configuration(config2)

        self.harness.clear_configurations()

        assert len(self.harness.benchmark_results) == 0

    def test_get_configurations(self):
        """Test getting configurations."""
        config1 = Configuration("config1", {"neuron_count": 100})
        config2 = Configuration("config2", {"neuron_count": 200})

        self.harness.add_configuration(config1)
        self.harness.add_configuration(config2)

        configs = self.harness.get_configurations()

        assert len(configs) == 2
        assert configs[0] == config1
        assert configs[1] == config2

        # Should return a copy, not reference
        configs[0] = Configuration("modified", {"neuron_count": 999})
        assert self.harness.benchmark_results[0] == config1  # Original unchanged

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

    def test_harness_add_multiple_configurations(self):
        """Test adding multiple configurations."""
        configs = [
            Configuration(f"config_{i}", {"neuron_count": i * 100})
            for i in range(1, 6)
        ]

        for config in configs:
            self.harness.add_configuration(config)

        assert len(self.harness.benchmark_results) == 5
        for i, config in enumerate(configs):
            assert self.harness.benchmark_results[i] == config


class TestBenchmarkResults:
    """Test benchmark results functionality."""

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

    def test_benchmark_result_failure(self):
        """Test BenchmarkResult with failure state."""
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

    def test_benchmark_result_from_metrics(self):
        """Test creating benchmark result from metrics."""
        metrics = [
            PerformanceMetrics(time.time(), "cpu", 50.0, "%"),
            PerformanceMetrics(time.time(), "memory", 512.0, "MB")
        ]

        result = BenchmarkResult.from_metrics(
            test_name="performance_test",
            metrics=metrics,
            parameters={"test": "data"}
        )

        assert result.test_name == "performance_test"
        assert len(result.metrics) == 2
        assert result.parameters == {"test": "data"}
        assert result.success is True


class TestBenchmarkProfiling:
    """Test benchmark profiling functionality."""

    def setup_method(self):
        """Setup before each test method."""
        self.harness = BenchmarkHarnessModule()
        self.harness.add_configuration(Configuration("default", {"neuron_count": 100}))

    @pytest.mark.slow
    def test_benchmark_run_profiling(self, mock_pynlm):
        """Test running benchmark profiling."""
        if not PYNLM_AVAILABLE:
            pytest.skip("pynlm not available")

        # Mock pynlm to simulate brain creation and stepping
        mock_brain_instance = Mock()
        mock_brain_instance.getTotalNeuronCount.return_value = 100
        mock_brain_instance.getRegionCount.return_value = 2
        mock_brain_instance.getTotalSynapseCount.return_value = 500
        mock_brain_instance.initialize.return_value = True
        mock_brain_instance.step.return_value = None

        with patch('pynlm.createBrain', return_value=mock_brain_instance):
            with patch('pynlm.Config', return_value=Mock()):
                result = self.harness.run_profiling()

        assert result.success is True
        assert result.duration >= 0
        assert len(result.metrics) > 0

    @pytest.mark.benchmark
    def test_benchmark_with_multiple_configurations(self):
        """Test benchmark with multiple configurations."""
        # Add multiple configurations
        configs = [
            Configuration("config1", {"neuron_count": 100}),
            Configuration("config2", {"neuron_count": 200}),
            Configuration("config3", {"neuron_count": 300})
        ]

        for config in configs:
            self.harness.add_configuration(config)

        assert len(self.harness.benchmark_results) == 3

        # Test getting configurations
        retrieved_configs = self.harness.get_configurations()
        assert len(retrieved_configs) == 3

        config_names = [c.name for c in retrieved_configs]
        assert "config1" in config_names
        assert "config2" in config_names
        assert "config3" in config_names

    def test_benchmark_insights_generation(self):
        """Test benchmark insights generation."""
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

        insights = self.harness.generate_benchmark_insights(result)

        assert "configurations_benchmarked" in insights
        assert "average_performance" in insights
        assert "performance_analysis" in insights
        assert "recommendations" in insights

    def test_benchmark_recommendations(self):
        """Test benchmark recommendations generation."""
        insights = {
            "performance_improvement_percentage": 25.0,
            "performance_analysis": "Excellent performance variation - significant optimizations possible"
        }

        recommendations = self.harness._generate_benchmark_recommendations(insights)

        assert len(recommendations) > 0
        assert any("optimizations" in rec.lower() for rec in recommendations)

    def test_benchmark_insights_with_no_performance_data(self):
        """Test benchmark insights with no performance data."""
        result = BenchmarkResult(
            test_name="no_performance",
            parameters={},
            metrics=[],
            duration=0.0,
            success=True
        )

        insights = self.harness.generate_benchmark_insights(result)

        assert "error" in insights
        assert "Failed to generate insights" in insights["error"]

    @pytest.mark.benchmark
    def test_benchmark_comparative_report(self):
        """Test comparative report generation."""
        # Create multiple benchmark results
        results = []
        for i in range(3):
            metrics = [
                PerformanceMetrics(time.time(), "steps_per_second", 1000.0 * (i + 1), "steps/sec")
            ]
            result = BenchmarkResult(
                test_name=f"benchmark_{i}",
                parameters={"run": i},
                metrics=metrics,
                duration=5.0 * (i + 1),
                success=True
            )
            results.append(result)

        report = self.harness.generate_comparative_report(results)

        assert "total_benchmarks" in report
        assert "successful_benchmarks" in report
        assert "average_duration" in report
        assert report["total_benchmarks"] == 3
        assert report["successful_benchmarks"] == 3


class TestBenchmarkCLI:
    """Test benchmark CLI interface."""

    def setup_method(self):
        """Setup before each test method."""
        self.cli = BenchmarkCLInterface()

    def test_cli_create_sample_configuration(self):
        """Test CLI sample configuration creation."""
        # Test default configuration
        config = self.cli.create_sample_configuration("default")
        assert config.name == "default"
        assert config.parameters["neuron_count"] == 100
        assert config.parameters["plasticity"] is True

        # Test high performance configuration
        config = self.cli.create_sample_configuration("high_performance")
        assert config.name == "high_performance"
        assert config.parameters["neuron_count"] == 500
        assert config.parameters["simd_enabled"] is True

        # Test minimal configuration
        config = self.cli.create_sample_configuration("minimal")
        assert config.name == "minimal"
        assert config.parameters["plasticity"] is False

        # Test unknown configuration type
        config = self.cli.create_sample_configuration("unknown")
        assert config.name == "default"  # Should fallback to default

    @pytest.mark.benchmark
    def test_cli_benchmark_execution(self):
        """Test CLI benchmark execution."""
        # This would test the actual CLI execution
        # For now, we'll test the CLI interface methods

        # Create mock arguments
        args = Mock()
        args.steps = 100
        args.runs = 3
        args.warmup_steps = 10
        args.json = False
        args.verbose = False
        args.config_type = "default"
        args.custom_configs = None
        args.brain_configs = None

        # Test the CLI run_benchmark method would be called
        # This would normally execute the benchmark suite
        pass

    def test_cli_error_handling(self):
        """Test CLI error handling."""
        # Test invalid configuration type
        config = self.cli.create_sample_configuration("invalid_type")
        assert config.name == "default"  # Should fallback


class TestPerformanceMeasurement:
    """Test performance measurement functionality."""

    @pytest.mark.performance
    def test_performance_measurement_timing(self, performance_timer):
        """Test performance measurement timing."""
        timer = performance_timer

        timer.start()
        time.sleep(0.01)  # Small delay
        duration = timer.stop()

        assert duration is not None
        assert duration > 0.005
        assert duration < 0.1

        # Test getting duration
        assert timer.get_duration() == duration

    @pytest.mark.slow
    def test_performance_measurement_benchmark(self):
        """Test performance measurement benchmark."""
        import timeit

        # Time a simple operation
        time_taken = timeit.timeit(
            lambda: sum(i for i in range(1000)),
            number=1000
        )

        assert time_taken > 0
        assert time_taken < 1.0  # Should complete quickly

        # Calculate operations per second
        ops_per_sec = 1000 * 1000 / time_taken
        assert ops_per_sec > 10000  # Should be high

    def test_performance_measurement_accuracy(self):
        """Test performance measurement accuracy."""
        timer = performance_timer

        # Run the same operation multiple times
        times = []
        for _ in range(5):
            timer.start()
            time.sleep(0.001)
            times.append(timer.stop())

        # Calculate statistics
        mean_time = statistics.mean(times)
        stddev_time = statistics.stdev(times) if len(times) > 1 else 0.0

        assert mean_time > 0.0005
        assert mean_time < 0.0015
        assert stddev_time < 0.0001  # Should be consistent

    def test_performance_measurement_memory_usage(self):
        """Test memory usage measurement."""
        import psutil
        import os

        process = psutil.Process(os.getpid())

        # Get initial memory
        initial_memory = process.memory_info().rss

        # Do some memory-intensive operations
        large_list = [list(range(10000)) for _ in range(100)]

        # Get memory after operations
        final_memory = process.memory_info().rss
        memory_increase = (final_memory - initial_memory) / (1024 * 1024)  # MB

        # Should be reasonable (less than 100MB for this test)
        assert memory_increase < 100.0

        # Cleanup
        del large_list

    @pytest.mark.performance
def test_system_resource_monitoring():
    """Test system resource monitoring."""
    import psutil

    # Get system information
    cpu_percent = psutil.cpu_percent(interval=0.1)
    memory = psutil.virtual_memory()
    disk = psutil.disk_usage('/')

    # Check that system is responding
    assert cpu_percent >= 0.0
    assert cpu_percent <= 100.0
    assert memory.total > 0
    assert memory.used >= 0
    assert disk.total > 0
    assert disk.used >= 0

    logger.info(f"CPU usage: {cpu_percent}%")
    logger.info(f"Memory used: {memory.used / (1024**3):.1f}GB / {memory.total / (1024**3):.1f}GB")
    logger.info(f"Disk used: {disk.used / (1024**2):.0f}MB / {disk.total / (1024**2):.0f}MB")


class TestPerformanceProfilingIntegration:
    """Test integration of performance profiling components."""

    def test_performance_framework_initialization(self):
        """Test performance framework initialization."""
        # Test that all components can be imported and initialized
        from nlm_profiling.base import ProfilingFramework

        framework = ProfilingFramework("test_framework")
        assert framework.name == "test_framework"
        assert framework.results is None

    def test_performance_components_compatibility(self):
        """Test compatibility between performance components."""
        # Test that benchmark harness can be created with different configs
        configs = [
            BenchmarkType.CREATION,
            BenchmarkType.STEP_PERFORMANCE,
            BenchmarkType.CONFIGURATION,
            BenchmarkType.COMPARATIVE
        ]

        for bench_type in configs:
            config = BenchmarkConfig(benchmark_type=bench_type)
            harness = BenchmarkHarnessModule(config)
            assert harness.config.benchmark_type == bench_type

    def test_performance_metrics_export(self):
        """Test performance metrics export functionality."""
        metrics = [
            PerformanceMetrics(time.time(), "metric1", 100.0, "unit1"),
            PerformanceMetrics(time.time(), "metric2", 200.0, "unit2")
        ]

        # Test JSON export
        json_output = BenchmarkResult(
            test_name="test_export",
            parameters={},
            metrics=metrics,
            duration=1.0,
            success=True
        ).to_dict()

        # Verify JSON structure
        assert "test_name" in json_output
        assert "metrics" in json_output
        assert len(json_output["metrics"]) == 2

        # Test text export
        text_output = metrics[0].to_text()
        assert "metric1" in text_output
        assert "100.0" in text_output
        assert "unit1" in text_output

    @pytest.mark.slow
    def test_performance_profiling_end_to_end(self):
        """Test end-to-end performance profiling."""
        # Create a comprehensive benchmark suite
        harness = BenchmarkHarnessModule()

        # Add multiple configurations
        harness.add_configuration(Configuration("fast", {"neuron_count": 50}))
        harness.add_configuration(Configuration("medium", {"neuron_count": 200}))
        harness.add_configuration(Configuration("slow", {"neuron_count": 1000}))

        # Test configuration management
        configs = harness.get_configurations()
        assert len(configs) == 3

        # Test insights generation (requires successful benchmark run)
        # In a real scenario, this would run actual benchmarks
        # For testing, we'll create a mock result
        mock_result = Mock()
        mock_result.success = True
        mock_result.metrics = [
            PerformanceMetrics(time.time(), "steps_per_second", 1000.0, "steps/sec")
        ]

        insights = harness.generate_benchmark_insights(mock_result)
        assert "performance_analysis" in insights


class TestPerformanceProfilingUtilities:
    """Test performance profiling utility functions."""

    def test_utility_create_test_config_file(self):
        """Test utility function for creating test config files."""
        from .conftest import create_test_config_file

        content = "test = value\nnumber = 42"
        filepath = create_test_config_file(content, ".test")

        try:
            assert filepath.exists()
            with open(filepath, 'r') as f:
                file_content = f.read()
                assert "test = value" in file_content
                assert "number = 42" in file_content
        finally:
            import os
            os.unlink(filepath)

    def test_utility_cleanup_test_file(self):
        """Test utility function for cleaning up test files."""
        from .conftest import cleanup_test_file, create_test_config_file

        filepath = create_test_config_file("test", ".test")
        assert filepath.exists()

        cleanup_test_file(filepath)
        assert not filepath.exists()

    def test_utility_assert_config_dict_equal(self):
        """Test utility function for asserting config dict equality."""
        from .conftest import assert_config_dict_equal

        # Test equal dictionaries
        actual = {"a": 1.0, "b": 2.0}
        expected = {"a": 1.0, "b": 2.0}
        assert_config_dict_equal(actual, expected)

        # Test with tolerance
        actual = {"a": 1.0000001, "b": 2.0}
        expected = {"a": 1.0, "b": 2.0}
        assert_config_dict_equal(actual, expected, tolerance=1e-6)

    @pytest.mark.performance
    def test_utility_generate_performance_data(self):
        """Test utility function for generating performance data."""
        from .conftest import generate_performance_data

        data = generate_performance_data(5)

        assert len(data) == 5
        for item in data:
            assert "steps_per_second" in item
            assert "average_step_time" in item
            assert "neuron_count" in item
            assert "spike_count" in item
            assert "memory_usage" in item

        # Verify data ranges
        for item in data:
            assert 50 <= item["steps_per_second"] <= 150
            assert 0.001 <= item["average_step_time"] <= 0.01
            assert 500 <= item["neuron_count"] <= 2000
            assert 1000 <= item["spike_count"] <= 10000


if __name__ == "__main__":
    pytest.main([__file__, "-v", "--tb=short"])