#!/usr/bin/env python3
"""
NLM Test Suite

Comprehensive test suite for the NLM (Neural Learning Machine) project.

This test suite provides extensive testing coverage for:
1. Documentation Tests - Verify documentation accuracy and examples
2. JSON/YAML Configuration Tests - Test configuration system
3. CLI Tool Tests - Test advanced CLI tools
4. Performance Profiling Tests - Test profiling utilities
5. Integration Tests - Test component integration
6. Regression Tests - Ensure backward compatibility
7. Performance Tests - Benchmark performance
8. Documentation Tests - Verify documentation examples

The test suite is organized into:
- Unit tests for individual components
- Integration tests for component interactions
- End-to-end tests for complete workflows
- Performance benchmarks for optimization
- Documentation tests for accuracy verification
- Regression tests for compatibility

Usage:
    pytest tests/ -v --tb=short
    pytest tests/ -k "performance" --tb=short
    pytest tests/ -k "integration" --tb=short

Test Categories:
- Documentation Tests
- Configuration Tests (JSON/YAML)
- CLI Tool Tests
- Performance Profiling Tests
- Integration Tests
- Regression Tests
- Performance Benchmarks
- Fixtures and Utilities

Best Practices:
- Use pytest with comprehensive fixtures
- Mock external dependencies
- Include both happy path and error cases
- Performance-aware testing
- CI/CD friendly
- Well-documented tests
- Modular and scalable structure
"""

__version__ = "1.0.0"
__author__ = "NLM Test Suite"
__email__ = "test@nlm.ai"

import sys
import os

# Add current directory to Python path
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

# Test configuration
TEST_CONFIG = {
    'test_paths': {
        'test_data_dir': 'tests/test_data',
        'configs_dir': 'tests/test_data/configs',
        'cli_dir': 'tests/test_data/cli',
        'performance_dir': 'tests/test_data/performance',
        'docs_dir': 'tests/test_data/docs'
    },
    'test_constants': {
        'sample_config': {
            'random_seed': 42,
            'neuron_count': 1000,
            'log_level': 'INFO'
        },
        'performance_thresholds': {
            'max_execution_time': 1.0,
            'max_memory_increase': 50.0
        }
    }
}

# Available test modules
AVAILABLE_TESTS = [
    'test_documentation',
    'test_config_json_yaml', 
    'test_cli_tools',
    'test_performance_profiling',
    'test_integration',
    'test_regression',
    'test_performance_benchmark',
    'test_documentation_reports'
]

# Performance test data
PERFORMANCE_TEST_DATA = {
    'steps_per_second': [1000.0, 1200.0, 800.0, 1500.0, 900.0],
    'memory_usage': [128.0, 256.0, 200.0, 300.0, 180.0],
    'execution_times': [0.5, 0.8, 0.3, 1.2, 0.6]
}

# Error test cases
ERROR_TEST_CASES = [
    {
        'name': 'invalid_json_file',
        'path': 'invalid.json',
        'description': 'Test handling of invalid JSON files'
    },
    {
        'name': 'nonexistent_file',
        'path': '/nonexistent/path/file.txt',
        'description': 'Test handling of non-existent files'
    },
    {
        'name': 'invalid_cli_command',
        'command': 'nlm invalid_command',
        'description': 'Test handling of invalid CLI commands'
    }
]

# Test markers
TEST_MARKERS = [
    'unit',
    'integration', 
    'end_to_end',
    'performance',
    'benchmark',
    'regression',
    'slow',
    'cli',
    'docs',
    'config',
    'profiling'
]

# Custom pytest configuration
pytest_configure = lambda config: None

# Test utilities and helpers
class TestConfig:
    """Test configuration utility class."""
    
    def __init__(self):
        self.test_data_dir = os.path.join(os.path.dirname(__file__), 'test_data')
        self.setup_directories()
    
    def setup_directories(self):
        """Create test data directories."""
        directories = [
            self.test_data_dir,
            os.path.join(self.test_data_dir, 'configs'),
            os.path.join(self.test_data_dir, 'cli'),
            os.path.join(self.test_data_dir, 'performance'),
            os.path.join(self.test_data_dir, 'docs')
        ]
        
        for directory in directories:
            os.makedirs(directory, exist_ok=True)
    
    def get_config_path(self, filename):
        """Get full path to configuration test file."""
        return os.path.join(self.test_data_dir, 'configs', filename)
    
    def get_cli_path(self, filename):
        """Get full path to CLI test file."""
        return os.path.join(self.test_data_dir, 'cli', filename)

# Create test configuration
_test_config = TestConfig()

# Export commonly used functions and classes
__all__ = [
    'TestConfig',
    'TEST_CONFIG',
    'AVAILABLE_TESTS',
    'PERFORMANCE_TEST_DATA',
    'ERROR_TEST_CASES',
    'TEST_MARKERS'
]

# Initialize test configuration
if __name__ == '__main__':
    print("NLM Test Suite")
    print(f"Version: {__version__}")
    print(f"Available tests: {len(AVAILABLE_TESTS)}")
    print("Test modules:", ', '.join(AVAILABLE_TESTS))
    
    # Run tests if executed directly
    import subprocess
    sys.exit(subprocess.call([sys.executable, '-m', 'pytest', __file__, '-v', '--tb=short']))