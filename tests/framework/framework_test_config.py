#!/usr/bin/env python3
"""
NLM Test Framework Configuration

This file contains configuration and utilities for the NLM testing framework.
It provides common test utilities, fixtures, and configuration management.
"""

import os
import sys
import json
import logging
from pathlib import Path
from typing import Dict, Any, Optional, List
import tempfile
import shutil

# Add the project root to the path
sys.path.insert(0, str(Path(__file__).parent.parent))

class NLMTestConfig:
    """Configuration management for NLM tests."""
    
    def __init__(self, config_file: Optional[str] = None):
        self.base_dir = Path(__file__).parent.parent
        self.test_dir = self.base_dir / "tests"
        self.data_dir = self.test_dir / "data"
        self.results_dir = self.test_dir / "results"
        
        # Create directories
        self.data_dir.mkdir(exist_ok=True)
        self.results_dir.mkdir(exist_ok=True)
        
        # Load configuration
        if config_file:
            with open(config_file, 'r') as f:
                self.config = json.load(f)
        else:
            self.config = self._get_default_config()
    
    def _get_default_config(self) -> Dict[str, Any]:
        """Get default test configuration."""
        return {
            "logging_level": "INFO",
            "test_timeout": 300,  # seconds
            "performance_benchmarks": {
                "min_speed": 1000.0,  # operations per second
                "max_memory_mb": 1024.0,
                "min_efficiency": 0.8
            },
            "test_categories": [
                "core", "brain", "plasticity", "neuromodulation", 
                "memory", "cognition", "sensory", "motor", "environment",
                "world", "agent", "development", "prediction", "phase_specific",
                "integration", "python", "performance", "config", "experiment",
                "visualization"
            ],
            "test_fixtures": {
                "default_config": "test_config_default.json",
                "test_neurons": "test_neuron_data.json",
                "test_synapses": "test_synapse_data.json",
                "test_plasticity": "test_plasticity_data.json"
            }
        }
    
    def get(self, key: str, default=None):
        """Get configuration value."""
        keys = key.split('.')
        value = self.config
        for k in keys:
            if isinstance(value, dict) and k in value:
                value = value[k]
            else:
                return default
        return value

class NLMTestLogger:
    """Test logging utilities."""
    
    def __init__(self, config: NLMTestConfig):
        self.config = config
        self.logger = logging.getLogger("nlm_tests")
        self.logger.setLevel(getattr(logging, config.get("logging_level", "INFO")))
        
        # Create console handler
        handler = logging.StreamHandler()
        formatter = logging.Formatter(
            '%(asctime)s - %(name)s - %(levelname)s - %(message)s'
        )
        handler.setFormatter(formatter)
        self.logger.addHandler(handler)
    
    def log_test_info(self, test_name: str, message: str):
        """Log test information."""
        self.logger.info(f"[{test_name}] {message}")
    
    def log_test_error(self, test_name: str, message: str):
        """Log test error."""
        self.logger.error(f"[{test_name}] {message}")
    
    def log_test_warning(self, test_name: str, message: str):
        """Log test warning."""
        self.logger.warning(f"[{test_name}] {message}")
    
    def log_test_debug(self, test_name: str, message: str):
        """Log test debug message."""
        self.logger.debug(f"[{test_name}] {message}")

class NLMTestResults:
    """Test results management."""
    
    def __init__(self, config: NLMTestConfig, logger: NLMTestLogger):
        self.config = config
        self.logger = logger
        self.results_dir = config.results_dir
    
    def save_results(self, test_name: str, results: Dict[str, Any]):
        """Save test results to file."""
        result_file = self.results_dir / f"{test_name}_results.json"
        with open(result_file, 'w') as f:
            json.dump(results, f, indent=2)
        
        self.logger.log_test_info(test_name, f"Results saved to {result_file}")
    
    def load_results(self, test_name: str) -> Dict[str, Any]:
        """Load test results from file."""
        result_file = self.results_dir / f"{test_name}_results.json"
        if result_file.exists():
            with open(result_file, 'r') as f:
                return json.load(f)
        return {}
    
    def get_summary(self) -> Dict[str, Any]:
        """Get test summary from all result files."""
        summary = {
            "total_tests": 0,
            "passed_tests": 0,
            "failed_tests": 0,
            "skipped_tests": 0
        }
        
        for result_file in self.results_dir.glob("*_results.json"):
            with open(result_file, 'r') as f:
                results = json.load(f)
                summary["total_tests"] += results.get("total_tests", 0)
                summary["passed_tests"] += results.get("passed_tests", 0)
                summary["failed_tests"] += results.get("failed_tests", 0)
                summary["skipped_tests"] += results.get("skipped_tests", 0)
        
        return summary

class NLMTestFixtureManager:
    """Test fixture management."""
    
    def __init__(self, config: NLMTestConfig, logger: NLMTestLogger):
        self.config = config
        self.logger = logger
        self.fixtures_dir = config.test_dir / "fixtures"
        self.fixtures_dir.mkdir(exist_ok=True)
    
    def create_fixture(self, name: str, data: Dict[str, Any]):
        """Create a test fixture."""
        fixture_file = self.fixtures_dir / f"{name}.json"
        with open(fixture_file, 'w') as f:
            json.dump(data, f, indent=2)
        
        self.logger.log_test_info("fixture_manager", f"Fixture '{name}' created")
    
    def load_fixture(self, name: str) -> Dict[str, Any]:
        """Load a test fixture."""
        fixture_file = self.fixtures_dir / f"{name}.json"
        if fixture_file.exists():
            with open(fixture_file, 'r') as f:
                return json.load(f)
        return {}
    
    def cleanup_fixtures(self):
        """Clean up all fixtures."""
        for fixture_file in self.fixtures_dir.glob("*.json"):
            fixture_file.unlink()
        self.logger.log_test_info("fixture_manager", "All fixtures cleaned up")

class NLMTestPerformanceProfiler:
    """Test performance profiling utilities."""
    
    def __init__(self, config: NLMTestConfig, logger: NLMTestLogger):
        self.config = config
        self.logger = logger
        self.performance_dir = config.test_dir / "performance"
        self.performance_dir.mkdir(exist_ok=True)
    
    def profile_performance(self, test_name: str, execution_time: float, 
                          memory_usage: float, operations: int):
        """Profile test performance."""
        performance_data = {
            "test_name": test_name,
            "execution_time_ms": execution_time * 1000,
            "memory_usage_mb": memory_usage,
            "operations_per_second": operations / execution_time if execution_time > 0 else 0,
            "timestamp": str(Path.now())
        }
        
        # Save performance data
        perf_file = self.performance_dir / f"{test_name}_perf.json"
        with open(perf_file, 'w') as f:
            json.dump(performance_data, f, indent=2)
        
        # Check performance thresholds
        min_speed = self.config.get("performance_benchmarks", {}).get("min_speed", 1000.0)
        if performance_data["operations_per_second"] < min_speed:
            self.logger.log_test_warning(
                test_name, 
                f"Performance below minimum: {performance_data['operations_per_second']} < {min_speed}"
            )
        
        self.logger.log_test_info(test_name, f"Performance: {performance_data['operations_per_second']:.0f} ops/sec")
    
    def get_performance_summary(self) -> Dict[str, Any]:
        """Get performance summary."""
        summary = {
            "total_tests_profiled": 0,
            "avg_speed": 0.0,
            "min_speed": float('inf'),
            "max_speed": 0.0
        }
        
        for perf_file in self.performance_dir.glob("*_perf.json"):
            with open(perf_file, 'r') as f:
                perf_data = json.load(f)
                
                summary["total_tests_profiled"] += 1
                speed = perf_data["operations_per_second"]
                
                if speed < summary["min_speed"]:
                    summary["min_speed"] = speed
                if speed > summary["max_speed"]:
                    summary["max_speed"] = speed
                
                summary["avg_speed"] += speed
        
        if summary["total_tests_profiled"] > 0:
            summary["avg_speed"] /= summary["total_tests_profiled"]
        
        return summary

def setup_test_environment():
    """Setup the test environment."""
    print("Setting up NLM Test Environment...")
    
    # Create necessary directories
    test_dirs = [
        "tests/data",
        "tests/results", 
        "tests/performance",
        "tests/fixtures",
        "tests/reports"
    ]
    
    for test_dir in test_dirs:
        Path(test_dir).mkdir(parents=True, exist_ok=True)
    
    print("Test environment setup complete.")

def cleanup_test_environment():
    """Cleanup the test environment."""
    print("Cleaning up NLM Test Environment...")
    
    # Remove temporary files and results
    for result_dir in Path("tests/results").glob("*"):
        if result_dir.is_dir():
            shutil.rmtree(result_dir)
    
    print("Test environment cleanup complete.")

if __name__ == "__main__":
    print("NLM Test Framework Configuration")
    print("=" * 50)
    
    config = NLMTestConfig()
    logger = NLMTestLogger(config)
    results = NLMTestResults(config, logger)
    fixture_manager = NLMTestFixtureManager(config, logger)
    profiler = NLMTestPerformanceProfiler(config, logger)
    
    print(f"Test configuration loaded")
    print(f"Test categories: {', '.join(config.get('test_categories', []))}")
    print(f"Logging level: {config.get('logging_level')}")
    print(f"Test timeout: {config.get('test_timeout')} seconds")
    
    # Print configuration
    print("\nCurrent configuration:")
    print(json.dumps(config.config, indent=2))