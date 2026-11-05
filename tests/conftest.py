#!/usr/bin/env python3
"""
Fixtures and utilities for NLM test suite.

Provides common fixtures and utility functions used across different test modules.
"""

import pytest
import tempfile
import os
import json
import yaml
import logging
import time
import sys
from pathlib import Path
from typing import Any, Dict, List, Optional, Generator
from unittest.mock import Mock, patch, MagicMock
import argparse

# Import NLM modules (with error handling)
try:
    import pynlm
    PYNLM_AVAILABLE = True
except ImportError:
    PYNLM_AVAILABLE = False

logger = logging.getLogger("nlm_test_utils")


class NLMTestConfig:
    """Configuration for NLM test suite."""

    def __init__(self):
        self.test_data_dir = Path(__file__).parent / "test_data"
        self.configs_dir = self.test_data_dir / "configs"
        self.cli_dir = self.test_data_dir / "cli"
        self.performance_dir = self.test_data_dir / "performance"
        self.docs_dir = self.test_data_dir / "docs"

        # Create directories if they don't exist
        self.test_data_dir.mkdir(exist_ok=True)
        self.configs_dir.mkdir(exist_ok=True)
        self.cli_dir.mkdir(exist_ok=True)
        self.performance_dir.mkdir(exist_ok=True)
        self.docs_dir.mkdir(exist_ok=True)

    def get_config_file_path(self, filename: str) -> Path:
        """Get path to configuration test file."""
        return self.configs_dir / filename

    def get_cli_file_path(self, filename: str) -> Path:
        """Get path to CLI test file."""
        return self.cli_dir / filename

    def get_performance_file_path(self, filename: str) -> Path:
        """Get path to performance test file."""
        return self.performance_dir / filename

    def get_docs_file_path(self, filename: str) -> Path:
        """Get path to documentation test file."""
        return self.docs_dir / filename


@pytest.fixture(scope="session")
def test_config() -> Generator[NLMTestConfig, None, None]:
    """Fixture providing NLM test configuration."""
    config = NLMTestConfig()
    yield config


@pytest.fixture(scope="session")
def pynlm_available() -> bool:
    """Fixture indicating whether pynlm is available."""
    return PYNLM_AVAILABLE


@pytest.fixture(scope="function")
def mock_pynlm() -> Generator[Mock, None, None]:
    """Fixture providing mocked pynlm module."""
    with patch.dict('sys.modules', {'pynlm': Mock()}):
        yield Mock()


@pytest.fixture(scope="function")
def temp_config_file() -> Generator[Path, None, None]:
    """Fixture providing temporary configuration file."""
    with tempfile.NamedTemporaryFile(mode='w', suffix='.cfg', delete=False) as f:
        f.write("""# Test configuration
random_seed = 42
neuron_count = 1000
log_level = INFO
""")
        temp_path = Path(f.name)

    yield temp_path

    # Cleanup
    if temp_path.exists():
        temp_path.unlink()


@pytest.fixture(scope="function")
def temp_json_config() -> Generator[Path, None, None]:
    """Fixture providing temporary JSON configuration file."""
    with tempfile.NamedTemporaryFile(mode='w', suffix='.json', delete=False) as f:
        json.dump({
            "random_seed": 42,
            "neuron_count": 1000,
            "log_level": "DEBUG"
        }, f)
        temp_path = Path(f.name)

    yield temp_path

    # Cleanup
    if temp_path.exists():
        temp_path.unlink()


@pytest.fixture(scope="function")
def temp_yaml_config() -> Generator[Path, None, None]:
    """Fixture providing temporary YAML configuration file."""
    with tempfile.NamedTemporaryFile(mode='w', suffix='.yaml', delete=False) as f:
        yaml.dump({
            "random_seed": 42,
            "neuron_count": 1000,
            "log_level": "WARN"
        }, f)
        temp_path = Path(f.name)

    yield temp_path

    # Cleanup
    if temp_path.exists():
        temp_path.unlink()


@pytest.fixture(scope="function")
def sample_config_dict() -> Dict[str, Any]:
    """Fixture providing sample configuration dictionary."""
    return {
        "random_seed": 42,
        "simulation_timestep": 0.001,
        "neuron_count": 1000,
        "region_count": 2,
        "connection_probability": 0.1,
        "log_level": "INFO",
        "enable_feature": True
    }


@pytest.fixture(scope="function")
def mock_brain() -> Generator[Mock, None, None]:
    """Fixture providing mocked brain object."""
    mock_brain = Mock()
    mock_brain.getTotalNeuronCount.return_value = 1000
    mock_brain.getRegionCount.return_value = 2
    mock_brain.getTotalSynapseCount.return_value = 5000
    mock_brain.getActiveNeuronCount.return_value = 100
    mock_brain.getFiringNeuronCount.return_value = 50
    mock_brain.getAverageFiringRate.return_value = 2.5
    mock_brain.getExcitationInhibitionRatio.return_value = 1.2
    mock_brain.getTotalSpikeCount.return_value = 10000
    mock_brain.getDevelopmentalStage.return_value = "adult"
    mock_brain.getLearnedPreference.return_value = 0.8
    mock_brain.initialize.return_value = True
    mock_brain.save.return_value = True
    mock_brain.step.return_value = None

    yield mock_brain


@pytest.fixture(scope="function")
def mock_agent() -> Generator[Mock, None, None]:
    """Fixture providing mocked agent object."""
    mock_agent = Mock()
    mock_agent.enableRewardModulation.return_value = None
    mock_agent.enableStructuralPlasticity.return_value = None
    mock_agent.enableDevelopment.return_value = None
    mock_agent.enableCuriosity.return_value = None
    mock_agent.initialize.return_value = None
    mock_agent.processSensoryInput.return_value = None
    mock_agent.decodeMotorCommand.return_value = [0.1, 0.5, 0.9]
    mock_agent.applyRewardModulation.return_value = None
    mock_agent.updateDevelopment.return_value = None
    mock_agent.getCuriosityLevel.return_value = 0.5
    mock_agent.getNoveltyLevel.return_value = 0.3
    mock_agent.getPredictionError.return_value = 0.1

    yield mock_agent


@pytest.fixture(scope="function")
def mock_world() -> Generator[Mock, None, None]:
    """Fixture providing mocked world object."""
    mock_world = Mock()
    mock_world.configure.return_value = None
    mock_world.reset.return_value = None
    mock_world.setAgentStart.return_value = None
    mock_world.update.return_value = None
    mock_world.getSensoryPercept.return_value = Mock()
    mock_world.applyMotorCommand.return_value = None
    mock_world.getSimulationTime.return_value = 0.0

    # Mock sensory percept
    mock_percept = Mock()
    mock_percept.getInternal.return_value = [0.0, 1.0, 0.5]  # reward, curiosity, novelty
    mock_world.getSensoryPercept.return_value = mock_percept

    yield mock_world


@pytest.fixture(scope="function")
def mock_config() -> Generator[Mock, None, None]:
    """Fixture providing mocked configuration object."""
    mock_config = Mock()
    mock_config.has.return_value = True
    mock_config.get.return_value = Mock()
    mock_config.get.return_value.has_value.return_value = True
    mock_config.get.return_value.value.return_value = "test_value"
    mock_config.getKeys.return_value = ["key1", "key2", "key3"]
    mock_config.summary.return_value = "Test summary"
    mock_config.loadFromFile.return_value = True
    mock_config.loadFromJsonFile.return_value = True
    mock_config.loadFromYamlFile.return_value = True
    mock_config.saveToFile.return_value = True
    mock_config.saveToJsonFile.return_value = True
    mock_config.saveToYamlFile.return_value = True
    mock_config.set.return_value = None
    mock_config.remove.return_value = None

    yield mock_config


@pytest.fixture(scope="function")
def mock_performance_metrics() -> Generator[Mock, None, None]:
    """Fixture providing mocked performance metrics."""
    from nlm_profiling.base import PerformanceMetrics, OutputFormat

    mock_metrics = Mock(spec=PerformanceMetrics)
    mock_metrics.timestamp = time.time()
    mock_metrics.metric_name = "test_metric"
    mock_metrics.value = 1.0
    mock_metrics.unit = "test_unit"
    mock_metrics.to_text.return_value = "Test metric: 1.0 test_unit"

    yield mock_metrics


@pytest.fixture(scope="function")
def cli_args() -> Generator[argparse.Namespace, None, None]:
    """Fixture providing CLI arguments namespace."""
    args = argparse.Namespace(
        verbose=False,
        json=False,
        help=False,
        version=False,
        command="test",
        neurons="1000",
        regions="2",
        config_file=None,
        brain_file=None,
        max_steps=1000,
        start_step=0,
        steps=100,
        runs=3,
        warmup_steps=10,
        benchmark_steps=100,
        param_values=[],
        warmup=None,
        output_file=None,
        format="auto",
        key="test_key",
        value="test_value"
    )

    yield args


@pytest.fixture(scope="session")
def performance_timer():
    """Fixture providing performance timing utility."""

    class Timer:
        def __init__(self):
            self.start_time = None
            self.end_time = None
            self.duration = None

        def start(self):
            self.start_time = time.perf_counter()

        def stop(self):
            self.end_time = time.perf_counter()
            self.duration = self.end_time - self.start_time
            return self.duration

        def get_duration(self):
            return self.duration

    yield Timer


@pytest.fixture(scope="session")
def test_logger():
    """Fixture providing test logger."""
    logger = logging.getLogger("nlm_test_logger")
    handler = logging.StreamHandler()
    formatter = logging.Formatter(
        '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
    )
    handler.setFormatter(formatter)
    logger.addHandler(handler)
    logger.setLevel(logging.DEBUG)

    yield logger


@pytest.fixture(scope="function")
def capture_test_output():
    """Fixture to capture test output."""
    import io
    from contextlib import redirect_stdout, redirect_stderr

    stdout_capture = io.StringIO()
    stderr_capture = io.StringIO()

    with redirect_stdout(stdout_capture), redirect_stderr(stderr_capture):
        yield stdout_capture, stderr_capture

    # Reset captured output
    stdout_capture.seek(0)
    stderr_capture.seek(0)


def create_test_config_file(content: str, suffix: str = '.cfg') -> Path:
    """Helper function to create test configuration file."""
    with tempfile.NamedTemporaryFile(mode='w', suffix=suffix, delete=False) as f:
        f.write(content)
        return Path(f.name)


def cleanup_test_file(filepath: Path) -> None:
    """Helper function to clean up test file."""
    if filepath.exists():
        filepath.unlink()


def assert_config_dict_equal(actual: Dict[str, Any], expected: Dict[str, Any], tolerance: float = 1e-6) -> None:
    """Assert that configuration dictionaries are equal."""
    assert actual.keys() == expected.keys(), "Configuration keys don't match"

    for key in actual:
        actual_value = actual[key]
        expected_value = expected[key]

        if isinstance(actual_value, (int, float)) and isinstance(expected_value, (int, float)):
            assert abs(actual_value - expected_value) < tolerance, \
                f"Values for key '{key}' don't match within tolerance"
        else:
            assert actual_value == expected_value, \
                f"Values for key '{key}' don't match"


def generate_performance_data(num_samples: int = 10) -> List[Dict[str, float]]:
    """Generate performance test data."""
    import random

    data = []
    for i in range(num_samples):
        data.append({
            'timestamp': time.time() - (num_samples - i) * 0.1,
            'steps_per_second': random.uniform(50, 150),
            'average_step_time': random.uniform(0.001, 0.01),
            'neuron_count': random.randint(500, 2000),
            'spike_count': random.randint(1000, 10000),
            'memory_usage': random.uniform(100, 500)
        })

    return data


@pytest.fixture(scope="session")
def performance_test_data() -> List[Dict[str, float]]:
    """Fixture providing performance test data."""
    return generate_performance_data()


# Test utility functions
@pytest.mark.unit
class TestTestUtilities:
    """Test the test utilities themselves."""

    def test_create_test_config_file(self):
        """Test creating test configuration file."""
        content = "random_seed = 42\nneuron_count = 1000"
        filepath = create_test_config_file(content, ".cfg")

        try:
            assert filepath.exists()
            with open(filepath, 'r') as f:
                file_content = f.read()
                assert "random_seed = 42" in file_content
        finally:
            cleanup_test_file(filepath)

    def test_cleanup_test_file(self):
        """Test cleaning up test file."""
        filepath = create_test_config_file("test", ".test")
        assert filepath.exists()

        cleanup_test_file(filepath)
        assert not filepath.exists()

    def test_assert_config_dict_equal(self):
        """Test configuration dictionary assertion."""
        actual = {"a": 1.0, "b": 2.0}
        expected = {"a": 1.0 + 1e-7, "b": 2.0}

        assert_config_dict_equal(actual, expected, tolerance=1e-6)

    def test_generate_performance_data(self):
        """Test performance data generation."""
        data = generate_performance_data(5)

        assert len(data) == 5
        for item in data:
            assert "steps_per_second" in item
            assert "average_step_time" in item
            assert "neuron_count" in item

    def test_test_config_object(self):
        """Test test configuration object."""
        config = NLMTestConfig()

        assert config.test_data_dir.exists()
        assert config.configs_dir.exists()
        assert config.cli_dir.exists()
        assert config.performance_dir.exists()
        assert config.docs_dir.exists()

    def test_pynlm_availability(self):
        """Test pynlm availability fixture."""
        # This test will pass or fail depending on pynlm availability
        # The fixture just returns the boolean
        assert isinstance(PYNLM_AVAILABLE, bool)